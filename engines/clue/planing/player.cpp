/*
**      $Filename: planing/player.c
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.player for "Der Clou!"
**
** (c) 1994 ...and avoid panic by, Kaweh Kazemi
**      All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "clue/base/base.h"
#include "clue/sound/newsound.h"
#include "clue/clue.h"
#include "clue/present/interac.h"
#include "clue/data/dataappl.h"
#include "clue/organisa/organisa.h"
#include "clue/planing/planer.h"
#include "clue/planing/player.h"
#include "clue/scenes/scenes.h"

namespace Clue {

/* Menu ids - player */
#define PLANING_PLAYER_PERSON_CHANGE   0
#define PLANING_PLAYER_RADIO_ALL       1
#define PLANING_PLAYER_RADIO_ONE       2
#define PLANING_PLAYER_ESCAPE          3

#ifdef THECLOU_DEBUG
#define PLANING_PLAYER_DEBUG           4
#endif

/* Player defines */
#define PLANING_EXHAUST_MAX         240
#define PLANING_MOOD_MIN            40
#define PLANING_LOUDNESS_STD        5
#define PLANING_LOUDNESS_RADIO      25
#define PLANING_LOUDNESS_OPEN_CLOSE 14

/* pseudo actions */
#define ACTION_EXHAUST  1000

/* derivation defines */
#define PLANING_DERI_ALARM              (PD.realTime / g_clue->calcRandomNr(1, 3))
#define PLANING_DERI_WATCHDOG           (PD.realTime / g_clue->calcRandomNr(6, 10))
#define PLANING_DERI_UNABLE_TO_WORK     3
#define PLANING_DERI_IGNORE_ACTION      (PD.realTime / g_clue->calcRandomNr(15, 20))
#define PLANING_DERI_GUARD_ESCAPED      (PD.realTime / g_clue->calcRandomNr(4, 8))

/* sound system */
#define PLANING_MUSIC_PLAYER_BEGIN_BARRACKS  "final.bk"
#define PLANING_MUSIC_PLAYER_BEGIN_STD       "bruch1.bk"
#define PLANING_MUSIC_PLAYER_END_STD         "bruch2.bk"

/* if no checking should be done define this */
/* #define PLAN_IS_PERFECT */


/* Player data structure */
static struct {
	ActionNode *action;

	byte handlerEnded[PLANING_NR_PERSONS];
	byte guardKO[PLANING_NR_GUARDS];
	byte currLoudness[PLANING_NR_PERSONS];
	byte unableToWork[PLANING_NR_PERSONS];

	byte ende;

	uint32 maxTimer;
	uint32 timer;
	uint32 realTime;

	byte badPlaning;
	byte mood;

	uint32 bldId;
	BuildingNode *bldObj;

	uint16 changeCount;
	uint16 totalCount;
	byte patrolCount;

	uint32 alarmTimer;

	byte isItDark;

	byte sndState;  /* Careful! Behaviour of sndState is different from the Amiga version */

	uint32 actionTime;
	byte(*actionFunc)(uint32, uint32);
} PD;


static void CheckSurrounding(uint32 current) {
#ifndef PLAN_IS_PERFECT
	if (has(PersonsList->getNthNode(current)->_nr, Ability_Surveillance)) {
		int32 watch = 0;

		if (Search.EscapeBits & FAHN_QUIET_ALARM)
			watch = tcWatchDogWarning(PersonsList->getNthNode(current)->_nr);
		else
			watch =
			    tcWrongWatchDogWarning(PersonsList->getNthNode(current)->_nr);

		if (watch) {
			plSay("PLAYER_WATCHDOG", current);
			inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
			Search.DeriTime += PLANING_DERI_WATCHDOG;
			Search.WarningCount++;
		}
	}
#endif
}

static void UnableToWork(uint32 current, uint32 action) {
	PD.currLoudness[current] = PLANING_LOUDNESS_STD;
	Search.Exhaust[current] = tcGuyIsWaiting(PersonsList->getNthNode(current)->_nr, Search.Exhaust[current]);
	Search.WaitTime[current]++;

	livAnimate(Planing_Name[current], ANM_STAND, 0, 0);

	/* special stuff */
	if (!PD.unableToWork[current]) {
		switch (action) {
		case ACTION_EXHAUST:
			plSay("PLAYER_UTW_EXHAUST", current);
			inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
			break;

		case ACTION_GO:
			plSay("PLAYER_UTW_GO", current);
			inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
			break;

		case ACTION_OPEN:
			plSay("PLAYER_UTW_OPEN", current);
			inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
			break;

		case ACTION_CLOSE:
			plSay("PLAYER_UTW_CLOSE", current);
			inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
			break;
		}

		PD.unableToWork[current] = 1;
	}
	/* if we can not go on wait and check */
	CheckSurrounding(current);

	if ((action != ACTION_WAIT) && (action != ACTION_WAIT_SIGNAL)) {
		Search.DeriTime += PLANING_DERI_UNABLE_TO_WORK;

		if (!PD.badPlaning) {
			plSay("PLAYER_BAD_PLANING", 0);
			inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
			PD.badPlaning = 1;
		}
	}

	if (action != ACTION_WAIT)
		IncCurrentTimer(plSys, 1, false);
}

static byte plGetMood(uint32 time) {
	uint32 guyId[PLANING_NR_PERSONS];

	for (uint16 i = 0; i < PLANING_NR_PERSONS; i++)
		guyId[i] = 0;

	for (uint16 i = 0; i < BurglarsNr; i++)
		guyId[i] = PersonsList->getNthNode(i)->_nr;

	return (byte) tcGetTeamMood(guyId, time);
}

static void plPersonLearns(uint32 persId, uint32 toolId) {
	hasAll(persId, OLF_NORMAL, Object_Ability);

	uint32 para;
	switch (toolId) {
	case Tool_Hand:
	case Tool_Foot:
	case Tool_Chloroform:
		para = learnedGet(persId, Ability_Fight);

		if (para == NO_PARAMETER)
			learnedSetP(persId, Ability_Fight, 1);
		else
			learnedSetP(persId, Ability_Fight, para + 1);
		break;

	case Tool_Angle_grinder:
	case Tool_Core_drill:
	case Tool_Cutting_torch:
	case Tool_Oxyhydrogen_torch:
	case Tool_Stethoscope:
	case Tool_Electric_hammer:
		para = learnedGet(persId, Ability_Safes);

		if (para == NO_PARAMETER)
			learnedSetP(persId, Ability_Safes, 1);
		else
			learnedSetP(persId, Ability_Safes, para + 1);
		break;

	case Tool_Lockpick:
	case Tool_Crowbar:
	case Tool_Drilling_machine:
	case Tool_Drilling_winch:
	case Tool_Castle_engraver:
	case Tool_Glass_cutter:
		para = learnedGet(persId, Ability_Locks);

		if (para == NO_PARAMETER)
			learnedSetP(persId, Ability_Locks, 1);
		else
			learnedSetP(persId, Ability_Locks, para + 1);
		break;

	case Tool_Electrical_set:
		para = learnedGet(persId, Ability_Electronic);

		if (para == NO_PARAMETER)
			learnedSetP(persId, Ability_Electronic, 1);
		else
			learnedSetP(persId, Ability_Electronic, para + 1);
		break;

	case Tool_Dynamite:
		para = learnedGet(persId, Ability_Explosive);

		if (para == NO_PARAMETER)
			learnedSetP(persId, Ability_Explosive, 1);
		else
			learnedSetP(persId, Ability_Explosive, para + 1);
		break;
	}
}

static byte plCarTooFull() {
	NewList<NewNode> *l = tcMakeLootList(Person_Matt_Stuvysunt, Relation_has);

	CompleteLootNode *complete = (CompleteLootNode *) dbGetObject(CompleteLoot_LastLoot);
	CarNode *car = (CarNode *) dbGetObject(Organisation.CarID);
	byte ret = 0;

	if (complete->TotalVolume > car->Capacity)
		ret = 1;

	l->removeList();

	return ret;
}

static void plPlayerAction() {
	byte patroCounter = 3;
	byte DoScrolling = 0;

	PD.timer++;
	PD.realTime = PD.timer / PLANING_CORRECT_TIME;

	plDisplayTimer(PD.realTime, false);

	if (PD.sndState && (PD.timer > (PD.maxTimer * 20) / 100) && (PD.bldId != Building_Starford_Barracks)) {
		g_clue->_sndMgr->sndPlaySound(PLANING_MUSIC_PLAYER_END_STD, 0);
		PD.sndState = 0;
	}
#ifndef PLAN_IS_PERFECT
	if (!(Search.EscapeBits & FAHN_ALARM_TIMECLOCK) && !(PD.timer % 3) && tcCheckTimeClocks(PD.bldId)) {
		Search.EscapeBits |= FAHN_ALARM | FAHN_ALARM_TIMECLOCK;
		Search.DeriTime += PLANING_DERI_ALARM;
		plSay("PLAYER_TIMECLOCK", 0);
		inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
	}
#endif

#ifndef PLAN_IS_PERFECT
	if (!(PD.timer % 15)) {
		if (tcAlarmByLoudness(PD.bldObj, tcGetTotalLoudness(PD.currLoudness[0], PD.currLoudness[1], PD.currLoudness[2], PD.currLoudness[3])))
			Search.EscapeBits |= FAHN_QUIET_ALARM | FAHN_ALARM_LOUDN;
	}
#endif

	if (PD.bldId == Building_Tower_of_London)
		patroCounter = 9;

	if (PD.bldId == Building_Starford_Barracks)
		patroCounter = 30;

	if (!(PD.timer % patroCounter)) {
#ifndef PLAN_IS_PERFECT
		if (g_clue->calcRandomNr(0, 30 * (270 - tcRGetGRate(PD.bldObj) / 2 + 1)) == 0) {
			plSay("PLAYER_PATROL", 0);
			inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);

			PD.patrolCount++;

			if (tcAlarmByPatrol(PD.changeCount, PD.totalCount, PD.patrolCount))
				Search.EscapeBits |= FAHN_QUIET_ALARM | FAHN_ALARM_PATRO;
		}
#endif
	}
#ifndef PLAN_IS_PERFECT
	if (PD.alarmTimer && !(PD.timer % 180)) {
		if (!(--PD.alarmTimer))
			Search.EscapeBits |= FAHN_QUIET_ALARM | FAHN_ALARM_TIMER;
	}
#endif

#ifndef PLAN_IS_PERFECT
	if ((Search.EscapeBits & FAHN_QUIET_ALARM) || (Search.EscapeBits & FAHN_ALARM)) {
		if (!Search.TimeOfAlarm) {
			Search.TimeOfAlarm = PD.realTime;

			if (Search.EscapeBits & FAHN_ALARM) {
				g_clue->_sndMgr->sndPrepareFX("sirene.voc");
				g_clue->_sndMgr->sndPlayFX();
			}
		}

		if ((Search.EscapeBits & FAHN_ALARM) && ((PD.realTime - Search.TimeOfAlarm) % 120) == 119) {
			g_clue->_sndMgr->sndPrepareFX("sirene.voc");
			g_clue->_sndMgr->sndPlayFX();
		}

		if (PD.realTime >= Search.TimeOfAlarm + PD.bldObj->PoliceTime) {
			Search.EscapeBits |= FAHN_SURROUNDED;

			for (int i = 0; i < PLANING_NR_PERSONS; i++)
				PD.handlerEnded[i] = 1;

			g_clue->_sndMgr->sndPrepareFX("marthorn.voc");
			g_clue->_sndMgr->sndPlayFX();
		}
	}
#endif

#ifndef PLAN_IS_PERFECT
	if ((PD.bldId != Building_Starford_Barracks) && !(PD.timer % 15)) {
		if ((PD.mood = plGetMood(PD.realTime)) < PLANING_MOOD_MIN) {
			plSay("PLAYER_FLUCHT", 0);
			inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);

			Search.EscapeBits |= FAHN_ESCAPE;

			for (int i = 0; i < PLANING_NR_PERSONS; i++)
				PD.handlerEnded[i] = 1;
		}
	}
#endif

	if ((PD.realTime == PD.actionTime) && PD.actionFunc) {
		byte actionRet;

		if ((actionRet = PD.actionFunc(PD.actionTime, PD.bldId))) {
			char actionText[TXT_KEY_LENGTH];

			sprintf(actionText, "PLAYER_ACTION_%d", actionRet);
			plSay(actionText, 0);
			inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);

			Search.EscapeBits |= FAHN_ESCAPE;

			for (int i = 0; i < PLANING_NR_PERSONS; i++)
				PD.handlerEnded[i] = 1;
		}
	}
#ifndef PLAN_IS_PERFECT
	if (!(PD.timer % 3)) {
		if (PD.isItDark) {
			uint16 xpos[PLANING_NR_PERSONS];
			uint16 ypos[PLANING_NR_PERSONS];
			uint32 area[PLANING_NR_PERSONS];

			/* there are more efficient solutions, but no safer ones */
			for (int i = 0; i < PLANING_NR_PERSONS; i++) {
				xpos[i] = (uint16) -1;
				ypos[i] = (uint16) -1;
				area[i] = (uint32) -1;
			}

			for (int j = 0; j < BurglarsNr; j++) {
				xpos[j] = livGetXPos(Planing_Name[j]);
				ypos[j] = livGetYPos(Planing_Name[j]);
				area[j] = livWhereIs(Planing_Name[j]);
			}

			lsGuyInsideSpot(xpos, ypos, area);
		}
	}
#endif

	for (uint32 i = 0; i < PersonsNr; i++) {
		if ((i >= BurglarsNr) || !PD.handlerEnded[i]) {
			SetActivHandler(plSys, PersonsList->getNthNode(i)->_nr);

			if ((i >= BurglarsNr) && PD.guardKO[i - BurglarsNr]) {
				switch (PD.guardKO[i - BurglarsNr]) {
				case 1:
					livAnimate(Planing_Name[i], livGetViewDirection(Planing_Name[i]), 0, 0);
					break;

				case 2:
					livAnimate(Planing_Name[i], ANM_DUSEL_POLICE, 0, 0);
					break;

				case 3:
					livAnimate(Planing_Name[i], ANM_WORK_CONTROL, 0, 0);
					break;
				}
			} else if ((PD.action = NextAction(plSys))) {
				if (!(PD.timer % 12) && (i >= BurglarsNr)
				        && !(Search.EscapeBits & FAHN_ALARM_GUARD)) {
					byte dir = livGetViewDirection(Planing_Name[i]);
					uint16 xpos = livGetXPos(Planing_Name[i]);
					uint16 ypos = livGetYPos(Planing_Name[i]);

#ifndef PLAN_IS_PERFECT
					for (byte j = 0; j < BurglarsNr; j++) {
						if (tcGuardDetectsGuy(Planing_GuardRoomList[i - BurglarsNr], xpos, ypos,
						         dir, Planing_Name[i], Planing_Name[j])) {
							if ((PD.bldId == Building_Starford_Barracks) && j)
								break;

							plSay("PLAYER_GUARD_ALARM_1", i);
							inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
							Search.DeriTime += PLANING_DERI_ALARM;
							Search.EscapeBits |= FAHN_ALARM | FAHN_ALARM_GUARD;

							PD.guardKO[((PoliceNode *)dbGetObject(PersonsList->getNthNode(i)->_nr))->LivingID - BurglarsNr] = 3;
							break;
						}
					}
#endif
				}
#ifndef PLAN_IS_PERFECT
				if (!(PD.timer % 3) && (i < BurglarsNr) && (livWhereIs(Planing_Name[i]) == lsGetActivAreaID())) {
					if (PD.bldId != Building_Starford_Barracks) {
						if (!(Search.EscapeBits & FAHN_ALARM_MICRO) && tcAlarmByMicro(livGetXPos(Planing_Name[i]), livGetYPos(Planing_Name[i]), PD.currLoudness[i])) {
							Search.DeriTime += PLANING_DERI_ALARM;
							Search.EscapeBits |= FAHN_ALARM | FAHN_ALARM_MICRO;
							plSay("PLAYER_MICRO", i);
							inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
						}
					}
				}
#endif

				if (PD.action->Type != ACTION_GO) {
					if (i < BurglarsNr) {
						if (PD.action->Type == ACTION_SIGNAL)
							livAnimate(Planing_Name[i], ANM_MAKE_CALL, 0, 0);
						else
							plWork(i);
					} else
						livAnimate(Planing_Name[i], ANM_WORK_CONTROL, 0, 0);
				}

				switch (PD.action->Type) {
				case ACTION_GO: {
					ActionGoNode *curAct = (ActionGoNode *)PD.action;
					if (i < BurglarsNr) {
						if (Search.Exhaust[i] > PLANING_EXHAUST_MAX)
							UnableToWork(i, ACTION_EXHAUST);
						else {
							plMove(i, curAct->Direction);

							if (livCanWalk(Planing_Name[i])) {
								PD.currLoudness[i] = tcGetWalkLoudness();
								PD.unableToWork[i] = 0;

								Search.Exhaust[i] = tcGuyInAction(PersonsList->getNthNode(i)->_nr, Search.Exhaust[i]);
								Search.WalkTime[i]++;

								if (i == CurrentPerson) {
									lsInitScrollLandScape(curAct->Direction, LS_SCROLL_PREPARE);
									DoScrolling = 1;
								}
							} else if (livWhereIs(Planing_Name[i]) == lsGetActivAreaID())
								UnableToWork(i, ACTION_GO);
						}
					} else
						plMove(i, curAct->Direction);
					}
					break;

				case ACTION_WAIT:
					if (i < BurglarsNr)
						UnableToWork(i, ACTION_WAIT);
					break;

				case ACTION_SIGNAL: {
					ActionSignalNode *curAct = (ActionSignalNode *)PD.action;
					if (i < BurglarsNr) {
						PD.currLoudness[i] = PLANING_LOUDNESS_RADIO;

						if (ActionStarted(plSys)) {
							Search.CallCount++;

							InitSignal(plSys, PersonsList->getNthNode(i)->_nr, curAct->ReceiverId);

#ifndef PLAN_IS_PERFECT
							if (tcAlarmByRadio(PD.bldObj))
								Search.EscapeBits |= FAHN_QUIET_ALARM | FAHN_ALARM_RADIO;
#endif
						}

						if (ActionEnded(plSys)) {
							plSignalNode *sig = IsSignal(plSys, PersonsList->getNthNode(i)->_nr, curAct->ReceiverId);

							if (sig)
								CloseSignal(sig);
						}
					}
					}
					break;

				case ACTION_WAIT_SIGNAL: {
					ActionWaitSignalNode *curAct = (ActionWaitSignalNode *)PD.action;
					if (i < BurglarsNr) {
						plSignalNode *sig = IsSignal(plSys, curAct->SenderId, PersonsList->getNthNode(i)->_nr);

						if (sig)
							CloseSignal(sig);
						else
							UnableToWork(i, ACTION_WAIT_SIGNAL);
					}
					}
					break;

				case ACTION_USE: {
					ActionUseNode *curAct = (ActionUseNode *)PD.action;
					if (i < BurglarsNr) {
						Search.WorkTime[i]++;
						Search.Exhaust[i] = tcGuyInAction(PersonsList->getNthNode(i)->_nr, Search.Exhaust[i]);
						PD.unableToWork[i] = 0;

						if (ActionStarted(plSys)) {
							if (plIsStair(curAct->ItemId))
								livLivesInArea(Planing_Name[i], StairConnectsGet(curAct->ItemId, curAct->ItemId));
							else {
								uint32 needTime = PD.action->TimeNeeded;
								uint32 realTime = 0;

								if (dbIsObject(curAct->ItemId, Object_Police)) {
									if (plObjectInReach(i, curAct->ItemId)) {
										PD.currLoudness[i] = tcGetToolLoudness(PersonsList->getNthNode(i)->_nr, curAct->ToolId, Item_Guard);
										PD.guardKO[((PoliceNode *)dbGetObject(curAct->ItemId))->LivingID - BurglarsNr] = 1;
										PD.changeCount += 5;

										realTime = tcGuyUsesToolInPlayer(PersonsList->getNthNode(i)->_nr,
												PD.bldObj, curAct->ToolId, Item_Guard, needTime / PLANING_CORRECT_TIME);
										Search.KillTime[i] += realTime;
									} else {
										Search.DeriTime += PLANING_DERI_GUARD_ESCAPED;
										IgnoreAction(plSys);

										plSay("PLAYER_IGNORE_1", i);
										inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);

										break;
									}
								} else {
									if (!CHECK_STATE(lsGetObjectState(curAct->ItemId),Const_tcLOCK_UNLOCK_BIT))
										PD.currLoudness[i] = tcGetToolLoudness(PersonsList->getNthNode(i)->_nr,
											curAct->ToolId, ((LSObjectNode *)dbGetObject(curAct->ItemId))->Type);
									else if (((LSObjectNode *)dbGetObject(curAct->ItemId))->Type == Item_Window)
										PD.currLoudness[i] = PLANING_LOUDNESS_STD;
									else if (!plIgnoreLock(curAct->ItemId)) {
										Search.DeriTime += PLANING_DERI_IGNORE_ACTION;
										IgnoreAction(plSys);

										plSay("PLAYER_IGNORE_2", i);
										inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);

										break;
									}
#ifndef PLAN_IS_PERFECT
									if (tcGetDanger(PersonsList->getNthNode(i)->_nr, curAct->ToolId,
										((LSObjectNode *)dbGetObject(curAct->ItemId))->Type))
										PD.handlerEnded[i] = 2;
#endif

									realTime = tcGuyUsesToolInPlayer(PersonsList->getNthNode(i)->_nr, PD.bldObj,
											curAct->ToolId, ((LSObjectNode *)dbGetObject(curAct->ItemId))->Type,
											needTime / PLANING_CORRECT_TIME);
								}

								if (realTime) {
									Search.DeriTime += ((realTime * PLANING_CORRECT_TIME - needTime) * 5) / PLANING_CORRECT_TIME;

									IncCurrentTimer(plSys, realTime * PLANING_CORRECT_TIME - needTime, false);
								}

								plPersonLearns(PersonsList->getNthNode(i)->_nr, curAct->ToolId);
							}
						}
#ifndef PLAN_IS_PERFECT
						if (!(Search.EscapeBits & FAHN_ALARM_ALARM) && tcAlarmByTouch(curAct->ItemId)) {
							Search.DeriTime += PLANING_DERI_ALARM;
							Search.EscapeBits |= FAHN_ALARM | FAHN_ALARM_ALARM;
							plSay("PLAYER_ALARM", i);
							inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
						}
#endif

						if (ActionEnded(plSys)) {
							if (!plIgnoreLock(curAct->ItemId)) {
								if (plIsStair(curAct->ItemId)) {
									if (i == CurrentPerson) {
										uint32 newAreaId = StairConnectsGet(curAct->ItemId, curAct->ItemId);

										lsDoneActivArea(newAreaId);
										lsInitActivArea(newAreaId, livGetXPos(Planing_Name[i]), livGetYPos(Planing_Name[i]), Planing_Name[i]);

										if (lsGetStartArea() == lsGetActivAreaID())
											lsShowEscapeCar();  /* Auto neu zeichnen */

										livRefreshAll();
									}
								} else if (dbIsObject (curAct->ItemId, Object_Police)) {
									if (tcKillTheGuard (PersonsList->getNthNode(i)->_nr, PD.bldId))
										PD.guardKO[((PoliceNode *)dbGetObject(curAct->ItemId))->LivingID - BurglarsNr] = 2;
									else {
										if (!(Search.EscapeBits & FAHN_ALARM_GUARD)) {
											plSay("PLAYER_GUARD_ALARM_5", ((PoliceNode *)dbGetObject(curAct->ItemId))->LivingID);
											inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
											Search.DeriTime += PLANING_DERI_ALARM;
											Search.EscapeBits |= FAHN_ALARM | FAHN_ALARM_GUARD;
										}

										PD.handlerEnded[i] = 32;
										PD.guardKO[((PoliceNode *)dbGetObject(curAct->ItemId))->LivingID - BurglarsNr] = 3;
									}
								} else {
									if (!CHECK_STATE(lsGetObjectState(curAct->ItemId), Const_tcLOCK_UNLOCK_BIT)) {
										PD.changeCount++;

										lsSetObjectState(curAct->ItemId, Const_tcLOCK_UNLOCK_BIT, 1);

										if (((ToolNode *)dbGetObject(curAct->ToolId))->Effect & Const_tcTOOL_OPENS) {
											lsSetObjectState(curAct->ItemId, Const_tcOPEN_CLOSE_BIT, 1);
											plCorrectOpened((LSObjectNode *)dbGetObject(curAct->ItemId), true);
										}

										if (curAct->ToolId == Tool_Stabbing_card)
											tcRefreshTimeClock(PD.bldId, curAct->ItemId);
									} else if (((LSObjectNode *)dbGetObject(curAct->ItemId))->Type == Item_Window) {
										uint16 xpos, ypos;

										lsWalkThroughWindow((LSObjectNode *)dbGetObject(curAct->ItemId),
											livGetXPos(Planing_Name[i]), livGetYPos(Planing_Name[i]),
											&xpos, &ypos);
										livSetPos(Planing_Name[i], xpos, ypos);

										livRefreshAll();
									}
								}
							} else {
								uint32 state = lsGetObjectState(curAct->ItemId);

								if (CHECK_STATE(state, Const_tcON_OFF)) {
									lsSetObjectState(curAct->ItemId, Const_tcON_OFF, 0); /* on setzen  */

									PD.changeCount--;

									if (plIgnoreLock(curAct->ItemId) == PLANING_POWER) {
										lsSetSpotStatus(curAct->ItemId, LS_SPOT_ON);
										lsShowAllSpots(PD.realTime, LS_ALL_VISIBLE_SPOTS);
									}
								} else {
									lsSetObjectState(curAct->ItemId, Const_tcON_OFF, 1); /* off setzen */

									PD.changeCount++;

									switch (plIgnoreLock(curAct->ItemId)) {
									case PLANING_ALARM_TOP3:
										if (PD.alarmTimer)
											PD.alarmTimer = MIN(PD.alarmTimer, (uint32)PLANING_ALARM_TOP3);
										else
											PD.alarmTimer = PLANING_ALARM_TOP3;
										break;

									case PLANING_POWER:
										lsSetSpotStatus(curAct->ItemId, LS_SPOT_OFF);
										lsShowAllSpots(PD.realTime, LS_ALL_INVISIBLE_SPOTS);

#ifndef PLAN_IS_PERFECT
										if (!(Search.EscapeBits & FAHN_ALARM_POWER) && tcAlarmByPowerLoss(curAct->ItemId)) {
											Search.EscapeBits |= FAHN_ALARM | FAHN_ALARM_POWER;
											Search.DeriTime += PLANING_DERI_ALARM;
											plSay("PLAYER_POWER", i);
											inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
										}
#endif
										break;
									}
								}
							}

							plRefresh(curAct->ItemId);
							plDisplayInfo();
						}
					}
					}
					break;

				case ACTION_TAKE: {
					ActionTakeNode *curAct = (ActionTakeNode *)PD.action;
					if (i < BurglarsNr) {
						PD.currLoudness[i] = PLANING_LOUDNESS_STD;
						Search.WorkTime[i]++;
						PD.unableToWork[i] = 0;

						if (ActionStarted(plSys)) {
							PD.changeCount++;
						}

						if (Ask(dbGetObject(curAct->ItemId), hasLoot(i), dbGetObject(curAct->LootId)) && plObjectInReach(i, curAct->ItemId)) {
							lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 1);

#ifndef PLAN_IS_PERFECT
							if (!(Search.EscapeBits & FAHN_ALARM_ALARM) && tcAlarmByTouch(curAct->ItemId)) {
								Search.DeriTime += PLANING_DERI_ALARM;
								Search.EscapeBits |= FAHN_ALARM | FAHN_ALARM_ALARM;
								plSay("PLAYER_ALARM", i);
								inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
							}
#endif

							if (ActionEnded(plSys)) {
								uint32 newValue = GetP(dbGetObject(curAct->ItemId), hasLoot(i), dbGetObject(curAct->LootId));

								Planing_Weight[i] += ((LootNode *)dbGetObject(curAct->LootId))->Weight;
								Planing_Volume[i] += ((LootNode *)dbGetObject(curAct->LootId))->Volume;

								lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 0);

								if ((curAct->ItemId >= 9701) && (curAct->ItemId <= 9708)) {
									lsRemLootBag(curAct->ItemId);
									Planing_Loot[curAct->ItemId - 9701] = 0;
								} else if (CHECK_STATE(lsGetObjectState(curAct->ItemId), Const_tcTAKE_BIT)) {
									lsSetObjectState(curAct->ItemId, Const_tcACCESS_BIT, 0);
									lsTurnObject((LSObjectNode *)dbGetObject(curAct->ItemId), LS_OBJECT_INVISIBLE, LS_NO_COLLISION);
								}

								UnSet(dbGetObject(curAct->ItemId), hasLoot(i), dbGetObject(curAct->LootId));

								if (Ask(dbGetObject(PersonsList->getNthNode(i)->_nr), take_RelId, dbGetObject(curAct->LootId))) {
									uint32 oldValue = GetP(dbGetObject(PersonsList->getNthNode(i)->_nr), take_RelId, dbGetObject(curAct->LootId));

									SetP(dbGetObject(PersonsList->getNthNode(i)->_nr), take_RelId,
										dbGetObject(curAct->LootId), oldValue + newValue);
								} else
									SetP(dbGetObject(PersonsList->getNthNode(i)->_nr), take_RelId,
										dbGetObject(curAct->LootId), newValue);

								plRefresh(curAct->ItemId);
								plDisplayInfo();
							}
						} else {
							Search.DeriTime += PLANING_DERI_IGNORE_ACTION;
							IgnoreAction(plSys);

							plSay("PLAYER_IGNORE_3", i);
							inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
						}
					}
					}
					break;

				case ACTION_DROP: {
					ActionDropNode *curAct = (ActionDropNode *)PD.action;

					if (i < BurglarsNr) {
						PD.currLoudness[i] = PLANING_LOUDNESS_STD;
						Search.WorkTime[i]++;
						PD.unableToWork[i] = 0;

						if (ActionStarted(plSys))
							PD.changeCount++;

						if (Ask(dbGetObject(PersonsList->getNthNode(i)->_nr), take_RelId, dbGetObject(curAct->LootId))) {
							lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 1);

							if (ActionEnded(plSys)) {
								uint32 newValue = GetP(dbGetObject(PersonsList->getNthNode(i)->_nr), take_RelId, dbGetObject(curAct->LootId));

								Planing_Weight[i] -= ((LootNode *)dbGetObject(curAct->LootId))->Weight;
								Planing_Volume[i] -= ((LootNode *)dbGetObject(curAct->LootId))->Volume;

								lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 1);

								if (curAct->ItemId >= 9701 && curAct->ItemId <= 9708) {
									lsAddLootBag(livGetXPos(Planing_Name[i]), livGetYPos(Planing_Name[i]), curAct->ItemId - 9700);
									Planing_Loot[curAct->ItemId - 9701] = 1;
								} else if (CHECK_STATE(lsGetObjectState(curAct->ItemId), Const_tcTAKE_BIT))
									lsTurnObject((LSObjectNode *)dbGetObject(curAct->ItemId), LS_OBJECT_VISIBLE, LS_COLLISION);

								SetP(dbGetObject(curAct->ItemId), hasLoot(i), dbGetObject(curAct->LootId), newValue);
								UnSet(dbGetObject(PersonsList->getNthNode(i)->_nr), take_RelId, dbGetObject(curAct->LootId));

								plRefresh(curAct->ItemId);
								plDisplayInfo();
							}
						} else {
							Search.DeriTime += PLANING_DERI_IGNORE_ACTION;
							IgnoreAction(plSys);
						}
					}
					}
					break;

				case ACTION_OPEN: {
					ActionOpenNode *curAct = (ActionOpenNode *)PD.action;

					if (ActionStarted(plSys)) {
						PD.unableToWork[i] = 0;

						if (i >= BurglarsNr) {
#ifndef PLAN_IS_PERFECT
							if (CHECK_STATE(lsGetObjectState(curAct->ItemId), Const_tcOPEN_CLOSE_BIT)) {
								if (!(Search.EscapeBits & FAHN_ALARM_GUARD)) {
									plSay("PLAYER_GUARD_ALARM_3", i);
									inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
									Search.DeriTime += PLANING_DERI_ALARM;
									Search.EscapeBits |= FAHN_ALARM | FAHN_ALARM_GUARD;
								}

								PD.guardKO[((PoliceNode *)dbGetObject(PersonsList->getNthNode(i)->_nr))->LivingID - BurglarsNr] = 3;

								break;
							}
#endif
						}
					}

					if (i >= BurglarsNr || plIgnoreLock(curAct->ItemId)
						|| CHECK_STATE(lsGetObjectState(curAct->ItemId), Const_tcLOCK_UNLOCK_BIT)) {
						if (i >= BurglarsNr || plIgnoreLock(curAct->ItemId)
							|| !CHECK_STATE(lsGetObjectState(curAct->ItemId), Const_tcOPEN_CLOSE_BIT)) {
							if (i < BurglarsNr) {
								PD.currLoudness[i] = PLANING_LOUDNESS_OPEN_CLOSE;
								Search.WorkTime[i]++;
								lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 1);

#ifndef PLAN_IS_PERFECT
								if (!(Search.EscapeBits & FAHN_ALARM_ALARM) && tcAlarmByTouch(curAct->ItemId)) {
									Search.DeriTime += PLANING_DERI_ALARM;
									Search.EscapeBits |= FAHN_ALARM | FAHN_ALARM_ALARM;
									plSay("PLAYER_ALARM", i);
									inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
								}
#endif
							}

							if (ActionEnded(plSys)) {
								if (i < BurglarsNr) {
									PD.changeCount++;
									lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 0);
								}

								if (((LSObjectNode *)dbGetObject(curAct->ItemId))->Type == Item_WC && g_clue->calcRandomNr(0, 3) == 1) {
									g_clue->_sndMgr->sndPrepareFX("wc.voc");
									g_clue->_sndMgr->sndPlayFX();
								}

								lsSetObjectState(curAct->ItemId, Const_tcOPEN_CLOSE_BIT, 1);

								plCorrectOpened((LSObjectNode *)dbGetObject(curAct->ItemId), true);

								plRefresh(curAct->ItemId);
								plDisplayInfo();
							}
						} else {
							Search.DeriTime += PLANING_DERI_IGNORE_ACTION;
							IgnoreAction(plSys);

							plSay("PLAYER_IGNORE_4", i);
							inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
						}
					} else
						UnableToWork(i, ACTION_OPEN);
					}
					break;

				case ACTION_CLOSE: {
					ActionCloseNode * curAct = (ActionCloseNode *)PD.action;

					if (ActionStarted(plSys)) {
						PD.unableToWork[i] = 0;

						if (i >= BurglarsNr) {
#ifndef PLAN_IS_PERFECT
							if (!CHECK_STATE(lsGetObjectState(curAct->ItemId), Const_tcOPEN_CLOSE_BIT)) {
								if (!(Search.EscapeBits & FAHN_ALARM_GUARD)) {
									plSay("PLAYER_GUARD_ALARM_4", i);
									inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
									Search.DeriTime += PLANING_DERI_ALARM;
									Search.EscapeBits |= FAHN_ALARM | FAHN_ALARM_GUARD;
								}

								PD.guardKO[((PoliceNode *)dbGetObject(PersonsList->getNthNode(i)->_nr))->LivingID - BurglarsNr] = 3;

								break;
							}
#endif
						}
					}

					if ((i >= BurglarsNr) || plIgnoreLock(curAct->ItemId)
						|| CHECK_STATE(lsGetObjectState (curAct->ItemId), Const_tcLOCK_UNLOCK_BIT)) {
						if ((i >= BurglarsNr) || plIgnoreLock(curAct->ItemId)
							|| CHECK_STATE(lsGetObjectState (curAct->ItemId), Const_tcOPEN_CLOSE_BIT)) {
							if (i < BurglarsNr) {
								PD.currLoudness[i] = PLANING_LOUDNESS_OPEN_CLOSE;
								Search.WorkTime[i]++;
								lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 1);

#ifndef PLAN_IS_PERFECT
								if (!(Search.EscapeBits & FAHN_ALARM_ALARM) && tcAlarmByTouch(curAct-> ItemId)) {
									Search.DeriTime += PLANING_DERI_ALARM;
									Search.EscapeBits |= FAHN_ALARM | FAHN_ALARM_ALARM;
									plSay("PLAYER_ALARM", i);
									inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
								}
#endif
							}

							if (ActionEnded(plSys)) {
								if (i < BurglarsNr) {
									PD.changeCount--;
									lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 0);
								}

								lsSetObjectState(curAct->ItemId, Const_tcOPEN_CLOSE_BIT, 0);
								plCorrectOpened((LSObjectNode *)dbGetObject(curAct->ItemId), false);

								plRefresh(curAct->ItemId);
								plDisplayInfo();
							}
						}
						else {
							Search.DeriTime += PLANING_DERI_IGNORE_ACTION;
							IgnoreAction(plSys);

							plSay("PLAYER_IGNORE_5", i);
							inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
						}
					}
					else
						UnableToWork(i, ACTION_CLOSE);
					}
					break;

				case ACTION_CONTROL: {
					ActionControlNode *curAct = (ActionControlNode *)PD.action;

#ifndef PLAN_IS_PERFECT
					if (tcGuardChecksObject((LSObjectNode *)dbGetObject(curAct->ItemId))) {
						if (!(Search.EscapeBits & FAHN_ALARM_GUARD)) {
							plSay("PLAYER_GUARD_ALARM_2", i);
							inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
							Search.DeriTime += PLANING_DERI_ALARM;
							Search.EscapeBits |= FAHN_ALARM | FAHN_ALARM_GUARD;
						}

						PD.guardKO[((PoliceNode *)dbGetObject(PersonsList->getNthNode(i)->_nr))->LivingID - BurglarsNr] = 3;
					}
#endif

					if (((LSObjectNode *)dbGetObject(curAct->ItemId))->Type == Item_Clock)
						tcRefreshTimeClock(PD.bldId,((ActionUseNode *)PD.action)->ItemId);
					}
					break;
				}
			} else {
				PD.handlerEnded[i] = 1;
				livAnimate(Planing_Name[i], ANM_STAND, 0, 0);
			}
		} else {
			PD.currLoudness[i] = PLANING_LOUDNESS_STD;

			if (PD.handlerEnded[i] >= 2 && PD.handlerEnded[i] <= 30) {
				livAnimate(Planing_Name[i], ANM_ELEKTRO, 0, 0);
				PD.handlerEnded[i]++;
			}

			if (PD.handlerEnded[i] == 31) {
				plSay("PLAYER_VERLETZUNG", i);
				inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
				PD.handlerEnded[i]++;
			}

			if (PD.handlerEnded[i] == 32)
				livAnimate(Planing_Name[i], ANM_DUSEL, 0, 0);
		}
	}

	if (DoScrolling)
		lsScrollLandScape((byte) -1);

	livDoAnims((AnimCounter++) % 2, true);

	PD.ende = 1;

	for (int i = 0; i < PLANING_NR_PERSONS; i++) {
		if (!PD.handlerEnded[i])
			PD.ende = 0;
	}

	if (PD.ende)
		SetMenuTimeOutFunc(nullptr);
}

int32 plPlayer(uint32 objId, uint32 actionTime, byte(*actionFunc)(uint32, uint32)) {
	Common::Stream *fh = nullptr;
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(PLAN_TXT, "PLAYER_MENU");
	uint32 timeLeft = 0, choice1;
	int32 ret = 0;

	plPrepareSys(0, objId,
	             PLANING_INIT_PERSONSLIST | PLANING_HANDLER_ADD |
	             PLANING_HANDLER_OPEN | PLANING_GUARDS_LOAD |
	             PLANING_HANDLER_SET);
	plPrepareGfx(objId, LS_COLL_NORMAL,
	             PLANING_GFX_LANDSCAPE | PLANING_GFX_SPRITES |
	             PLANING_GFX_BACKGROUND);
	plPrepareRel();
	plPrepareData();

	AnimCounter = 0;

	byte activ = plOpen(objId, PLANING_OPEN_READ_BURGLARY, &fh);
	if (activ == PLANING_OPEN_OK) {
		NewList<NewNode> *l = LoadSystem(fh, plSys);
		if (!l) {
			l = plLoadTools(fh);
			if (!l) {
				PD.ende = 0;
				PD.timer = 0;
				PD.realTime = 0;
				PD.badPlaning = 0;
				PD.mood = plGetMood(0);
				PD.bldId = objId;
				PD.bldObj = (BuildingNode *) dbGetObject(objId);
				PD.changeCount = 0;
				PD.totalCount = lsGetObjectCount();
				PD.patrolCount = 0;
				PD.alarmTimer = 0;
				PD.isItDark = ((LSAreaNode *) dbGetObject(lsGetActivAreaID()))->uch_Darkness;
				PD.sndState = 1;
				PD.actionTime = actionTime;
				PD.actionFunc = actionFunc;

				/* Init data & search structure */
				for (byte i = 0; i < PLANING_NR_PERSONS; i++) {
					PD.handlerEnded[i] = 1;
					PD.currLoudness[i] = 0;
					PD.unableToWork[i] = 0;

					Search.Exhaust[i] = 0;

					Search.WalkTime[i] = 0;
					Search.WaitTime[i] = 0;
					Search.WorkTime[i] = 0;
					Search.KillTime[i] = 0;

					Search.SpotTouchCount[i] = 0;
				}

				/* Init activ handler & load them */
				for (byte i = 0; i < PLANING_NR_PERSONS; i++) {
					Search.GuyXPos[i] = -1;
					Search.GuyYPos[i] = -1;
				}

				PD.maxTimer = 0;

				for (byte i = 0; i < BurglarsNr; i++) {
					PD.handlerEnded[i] = 0;
					LoadHandler(fh, plSys, PersonsList->getNthNode(i)->_nr);

					PD.maxTimer = MAX(GetMaxTimer(plSys), PD.maxTimer);
				}

				for (byte i = 0; i < PLANING_NR_GUARDS; i++)
					PD.guardKO[i] = 0;

				/* Init search structure */
				Search.BuildingId = objId;

				Search.TimeOfBurglary = 0;
				Search.TimeOfAlarm = 0;

				Search.DeriTime = 0;
				Search.CallValue = 0;
				Search.CallCount = 0;
				Search.WarningCount = 0;
				Search.EscapeBits = 0;

				if (PD.bldId == Building_Starford_Barracks)
					g_clue->_sndMgr->sndPlaySound(PLANING_MUSIC_PLAYER_BEGIN_BARRACKS, 0);
				else
					g_clue->_sndMgr->sndPlaySound(PLANING_MUSIC_PLAYER_BEGIN_STD, 0);

				if (g_clue->getFeatures() & GF_PROFIDISK) {
					if (objId == Building_Mail_Train) {
						EnvironmentNode *env = (EnvironmentNode *) dbGetObject(Environment_TheClou);

						env->PostzugDone = 1;
					}
				}

				/* start main loop */
				SetMenuTimeOutFunc(plPlayerAction);

				while (!PD.ende) {
					uint32 bitset = BIT(PLANING_PLAYER_ESCAPE);

					if ((CurrentPerson < BurglarsNr) ? BurglarsNr > 1 : PersonsNr > 1)
						bitset += BIT(PLANING_PLAYER_PERSON_CHANGE);

					if (has(Person_Matt_Stuvysunt, Tool_Radio_equipment))
						bitset += BIT(PLANING_PLAYER_RADIO_ALL) + BIT(PLANING_PLAYER_RADIO_ONE);

					plDisplayTimer(PD.realTime, true);
					plDisplayInfo();

					ShowMenuBackground();

					inpTurnFunctionKey(false);
					inpTurnESC(false);

					activ = Menu(menu, bitset, activ, nullptr, INP_AS_FAST_AS_POSSIBLE);

					inpTurnFunctionKey(true);
					inpTurnESC(true);

					switch (activ) {
					case PLANING_PLAYER_PERSON_CHANGE:
						plMessage("CHANGE_PERSON_2", PLANING_MSG_REFRESH);

						if (_gamePlayMode & GP_MODE_GUARD_DESIGN) {
							if (PersonsNr > 2)
								choice1 = (uint32) Bubble((NewList<NewNode>*)PersonsList, CurrentPerson, nullptr, 0);
							else
								choice1 = CurrentPerson ? 0 : 1;
						} else {
							if (BurglarsNr > 2)
								choice1 = (uint32) Bubble((NewList<NewNode>*)BurglarsList, CurrentPerson, nullptr, 0);
							else
								choice1 = CurrentPerson ? 0 : 1;
						}

						if (choice1 != GET_OUT) {
							if (livWhereIs(Planing_Name[choice1]) != lsGetActivAreaID()) {
								lsDoneActivArea(livWhereIs(Planing_Name[choice1]));
								lsInitActivArea(livWhereIs(Planing_Name[choice1]),
								                livGetXPos(Planing_Name[choice1]),
								                livGetYPos(Planing_Name[choice1]),
								                Planing_Name[choice1]);
							}

							plPrepareSys(choice1, 0, PLANING_HANDLER_SET);
							lsSetActivLiving(Planing_Name[CurrentPerson], (uint16) -1, (uint16) -1);
						}
						break;

					case PLANING_PLAYER_RADIO_ALL:
						l = g_clue->_txtMgr->goKey(PLAN_TXT, "PLAYER_RADIO_1");

						plMessage("PLAYER_RADIO_ALL", PLANING_MSG_REFRESH);

						SetPictID(((PersonNode *)dbGetObject(BurglarsList->getNthNode(0)->_nr))->PictID);
						SetBubbleType(RADIO_BUBBLE);

						choice1 = Bubble(l, 0, nullptr, 0);
						if (choice1 != GET_OUT && choice1 < 2) {
							tcCalcCallValue(choice1, PD.realTime, 0);
							Search.CallCount++;
						}

						l->removeList();
						l = nullptr;
						break;

					case PLANING_PLAYER_RADIO_ONE:
						if (BurglarsNr > 2) {
							dbObjectNode *help;

							plMessage("RADIO_1", PLANING_MSG_REFRESH);
							SetPictID(((PersonNode *)dbGetObject(BurglarsList->getNthNode(0)->_nr))->PictID);
							dbObjectNode* node = BurglarsList->unLink(BurglarsList->getNthNode(0)->_name, &help);

							Common::String exp = g_clue->_txtMgr->getFirstLine(PLAN_TXT, "EXPAND_RADIO");
							BurglarsList->expandObjectList(exp);

							choice1 = Bubble((NewList<NewNode>*)BurglarsList, 0, nullptr, 0);

							if (ChoiceOk(choice1, GET_OUT, BurglarsList))
								choice1 = BurglarsList->getNthNode(choice1)->_nr;
							else
								choice1 = GET_OUT;

							BurglarsList->link(node, help);
							dbRemObjectNode(BurglarsList, 0);
						} else
							choice1 = CurrentPerson ? BurglarsList->getNthNode(0)->_nr : BurglarsList->getNthNode(1)->_nr;

						if (choice1 != GET_OUT) {
							l = g_clue->_txtMgr->goKey(PLAN_TXT, "PLAYER_RADIO_2");
							SetPictID(((PersonNode *)dbGetObject(BurglarsList->getNthNode(0)->_nr))->PictID);
							SetBubbleType(RADIO_BUBBLE);

							uint32 choice2 = (uint32)Bubble(l, 0, NULL, 0);
							if (choice2 != GET_OUT && choice2 < 3) {
								tcCalcCallValue(choice2 + 2, PD.realTime, choice1);
								Search.CallCount++;
							}

							l->removeList();
							l = nullptr;
						}
						break;

					case PLANING_PLAYER_ESCAPE:
						Search.EscapeBits |= FAHN_ESCAPE;

						for (byte i = 0; i < PLANING_NR_PERSONS; i++)
							PD.handlerEnded[i] = 1;
						break;

					default:
						activ = 0;
						break;

					}
				}

				SetMenuTimeOutFunc(NULL);
				inpSetWaitTicks(0); /* normales Ausmaß */

				Search.LastAreaId = lsGetActivAreaID();
				Search.TimeOfBurglary = PD.realTime;

				if (!(Search.EscapeBits & FAHN_SURROUNDED) && !(Search.EscapeBits & FAHN_ESCAPE)
				        && !plLivingsPosAtCar(objId)) {
					plSay("PLAYER_NOT_FINISHED", 0);
					Search.EscapeBits |= FAHN_ESCAPE;
				}

				for (byte i = 0; i < PersonsNr; i++)
					livTurn(Planing_Name[i], LIV_DISABLED); /* alle Sprites ausschalten! */

				for (byte i = 0; i < BurglarsNr; i++) {
					Search.GuyXPos[i] = livGetXPos(Planing_Name[i]);
					Search.GuyYPos[i] = livGetYPos(Planing_Name[i]);

					SetObjectListAttr(OLF_NORMAL, 0);
					AskAll(dbGetObject(PersonsList->getNthNode(i)->_nr), take_RelId, BuildObjectList);

					for (dbObjectNode* n = ObjectList->getListHead(); n->_succ; n = (dbObjectNode *)n->_succ) {
						if (has(Person_Matt_Stuvysunt, n->_nr)) {
							uint32 oldValue =
							    hasGet(Person_Matt_Stuvysunt, n->_nr);
							uint32 newValue =
							    GetP(dbGetObject(PersonsList->getNthNode(i)->_nr),
							         take_RelId, dbGetObject(n->_nr));

							hasSetP(Person_Matt_Stuvysunt, n->_nr, oldValue + newValue);
						} else {
							uint32 newValue =
							    GetP(dbGetObject(PersonsList->getNthNode(i)->_nr), take_RelId, dbGetObject(n->_nr));

							hasSetP(Person_Matt_Stuvysunt, n->_nr, newValue);
						}
					}
				}

				if (plCarTooFull()) {
					plSay("PLAYER_LEAVE_LOOTS_1", 0);

					while (plCarTooFull()) {
						hasAll(Person_Matt_Stuvysunt, OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_NORMAL, Object_Loot);

						uint32 choice = Bubble((NewList<NewNode>*)ObjectList, 0, 0, 0);
						if (choice != GET_OUT)
							hasUnSet(Person_Matt_Stuvysunt, ObjectList->getNthNode((uint32) choice)->_nr);
						else
							plSay("PLAYER_LEAVE_LOOTS_2", 0);
					}
				}

				if (!(Search.EscapeBits & FAHN_ALARM) && !(Search.EscapeBits & FAHN_QUIET_ALARM)
				        && !(Search.EscapeBits & FAHN_ESCAPE))
					Search.EscapeBits |= FAHN_STD_ESCAPE;

				lsSetViewPort(0, 0);

				timeLeft = tcEscapeFromBuilding(Search.EscapeBits);

				ret = 1;
			}
		}

		if (l) {
			Bubble(l, 0, nullptr, 0);
			l->removeList();
		}

		dskClose(fh);
	} else {
		if (activ == PLANING_OPEN_ERR_NO_PLAN)
			plMessage("NO_PLAN", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
	}

	if (Search.BuildingId == Building_Starford_Barracks)
		Search.BarracksOk = tcIsLastBurglaryOk();

	plUnprepareRel();
	plUnprepareGfx();
	plUnprepareSys();

	menu->removeList();

	if (ret)
		return tcEscapeByCar(Search.EscapeBits, timeLeft);

	return 0;
}

} // End of namespace Clue

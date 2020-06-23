/*
**      $Filename: planing/planer.c
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.planer for "Der Clou!"
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
#include "clue/clue.h"
#include "clue/present/interac.h"
#include "clue/data/dataappl.h"
#include "clue/organisa/organisa.h"
#include "clue/planing/player.h"
#include "clue/scenes/scenes.h"

namespace Clue {

/* Menu ids - planning */
#define PLANING_START      0
#define PLANING_NOTE       1
#define PLANING_SAVE       2
#define PLANING_LOAD       3
#define PLANING_CLEAR      4
#define PLANING_LOOK       5
#define PLANING_RETURN     6

#define PLANING_PERSON_WALK   0
#define PLANING_ACTION_USE    1
#define PLANING_ACTION_OPEN   2
#define PLANING_ACTION_CLOSE  3
#define PLANING_ACTION_TAKE   4
#define PLANING_ACTION_DROP   5
#define PLANING_ACTION_WAIT   6
#define PLANING_ACTION_RADIO  7
#define PLANING_PERSON_CHANGE 8
#define PLANING_ACTION_RETURN 9

#define PLANING_WAIT          0
#define PLANING_WAIT_RADIO    1
#define PLANING_WAIT_RETURN   2


/* Menu ids - notebook */
#define PLANING_NOTE_TARGET            0
#define PLANING_NOTE_TEAM              1
#define PLANING_NOTE_CAR               2
#define PLANING_NOTE_TOOLS             3
#define PLANING_NOTE_LOOTS             4

/* Menu ids - look */
#define PLANING_LOOK_PLAN              0
#define PLANING_LOOK_PERSON_CHANGE     1
#define PLANING_LOOK_RETURN            2

/* Menu ids - leveldesigner */
#define PLANING_LD_MOVE                     0
#define PLANING_LD_REFRESH                  1
#define PLANING_LD_OK                       2
#define PLANING_LD_CANCEL                   3

/* Std time defines */
#define PLANING_TIME_TAKE              3
#define PLANING_TIME_DROP              3
#define PLANING_TIME_RADIO             5
#define PLANING_TIME_CONTROL           5
#define PLANING_TIME_FIGHT             5
#define PLANING_TIME_THROUGH_WINDOW    6
#define PLANING_TIME_USE_STAIRS        8


byte AnimCounter = 0;
bool PlanChanged = false;

static uint32 UseObject;


/* action support functions */
static bool plRemLastAction() {
	if (!IsHandlerCleared(plSys)) {
		plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys) - CurrentAction(plSys)->TimeNeeded, CurrentAction(plSys)->TimeNeeded, 0);
		lsSetActivLiving(Planing_Name[CurrentPerson], (uint16) -1, (uint16) -1);

		RemLastAction(plSys);

		plDisplayTimer(0, true);
		plDisplayInfo();

		return true;
	}

	return false;
}

/* actions */
static void plActionGo() {
	ActionNode *action = CurrentAction(plSys);

	plMessage("WALK", PLANING_MSG_REFRESH);

	inpTurnFunctionKey(false);
	inpTurnMouse(false);
	inpSetKeyRepeat((0 << 5) | 0);

	while (true) {
		uint32 direction = 0;

		uint32 choice = inpWaitFor(INP_MOVEMENT | INP_LBUTTONP);

		if (choice & INP_LBUTTONP)
			break;

		if (!(choice & INP_MOUSE)) {
			if (choice & INP_ESC) {
				if (plRemLastAction())
					action = CurrentAction(plSys);
			} else {
				if (choice & INP_LEFT)
					direction += LS_SCROLL_LEFT;

				if (choice & INP_RIGHT)
					direction += LS_SCROLL_RIGHT;

				if (choice & INP_UP)
					direction += LS_SCROLL_UP;

				if (choice & INP_DOWN)
					direction += LS_SCROLL_DOWN;

				if (!lsInitScrollLandScape(direction, LS_SCROLL_PREPARE)) {
					if (!action || (action->Type != ACTION_GO)) {
						if ((action = InitAction(plSys, ACTION_GO, (uint32) direction, 0, 0)))
							PlanChanged = true;
						else {
							plSay("PLANING_END", CurrentPerson);
							inpSetKeyRepeat((1 << 5) | 10);
							inpTurnMouse(true);
							inpTurnFunctionKey(true);
							return;
						}
					}

					if (((ActionGoNode *) action)->Direction == (uint16) direction)
						IncCurrentTimer(plSys, 1, true);
					else if ((action = InitAction(plSys, ACTION_GO, direction, 0, 1)))
						PlanChanged = true;
					else {
						plSay("PLANING_END", CurrentPerson);
						inpSetKeyRepeat((1 << 5) | 10);
						inpTurnMouse(true);
						inpTurnFunctionKey(true);
						return;
					}

					plSync(PLANING_ANIMATE_STD, GetMaxTimer(plSys), 1, 1);
					plMove(CurrentPerson, direction);

					lsScrollLandScape((byte) -1);
					livDoAnims((AnimCounter++) % 2, true);

					plDisplayTimer(0, false);
				}
			}
		}
	}

	inpSetKeyRepeat((1 << 5) | 10);
	inpTurnMouse(true);
	inpTurnFunctionKey(true);
}

static void plActionWait() {
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(PLAN_TXT, "MENU_4");
	byte activ = 0;
	uint32 choice1 = 0, choice2 = 0;

	while (activ != PLANING_WAIT_RETURN) {
		uint32 bitset = BIT(PLANING_WAIT) + BIT(PLANING_WAIT_RETURN);

		if (CurrentPerson < BurglarsNr && BurglarsNr > 1)
			bitset += BIT(PLANING_WAIT_RADIO);

		plDisplayTimer(0, true);
		plDisplayInfo();

		ShowMenuBackground();

		inpTurnESC(false);
		inpTurnFunctionKey(false);
		inpTurnMouse(false);

		activ = Menu(menu, bitset, activ, nullptr, 0);

		inpTurnMouse(true);
		inpTurnFunctionKey(true);
		inpTurnESC(true);

		switch (activ) {
		case PLANING_WAIT: {
			choice1 = 0;
			choice2 = 0;

			plMessage("WAIT_1", PLANING_MSG_REFRESH);
			plDrawWait(choice2);

			inpTurnESC(false);
			inpTurnFunctionKey(false);
			inpTurnMouse(false);

			while (true) {
				choice1 = inpWaitFor(INP_RIGHT | INP_LEFT | INP_UP | INP_DOWN | INP_LBUTTONP);

				if (choice1 & INP_LBUTTONP)
					break;

				if (!(choice1 & INP_MOUSE)) {
					if ((choice1 & INP_RIGHT) && (choice2 < 1800)) {
						choice2++;
						plDrawWait(choice2);
					}

					if ((choice1 & INP_LEFT) && choice2) {
						choice2--;
						plDrawWait(choice2);
					}

					if ((choice1 & INP_UP) && (choice2 <= 1740)) {
						choice2 += 60;
						plDrawWait(choice2);
					}

					if ((choice1 & INP_DOWN) && (choice2 >= 60)) {
						choice2 -= 60;
						plDrawWait(choice2);
					}
				}
			}

			inpTurnMouse(true);
			inpTurnFunctionKey(true);
			inpTurnESC(true);

			if (choice2) {
				if (InitAction(plSys, ACTION_WAIT, 0, 0, choice2 * PLANING_CORRECT_TIME)) {
					PlanChanged = true;

					livAnimate(Planing_Name[CurrentPerson], ANM_STAND, 0, 0);
					plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys), choice2 * PLANING_CORRECT_TIME, 1);
					livRefreshAll();
				} else {
					plSay("PLANING_END", CurrentPerson);
					activ = PLANING_WAIT_RETURN;
				}
			}
		}
		break;

		case PLANING_WAIT_RADIO:
			if (has(Person_Matt_Stuvysunt, Tool_Radio_equipment)) {
				if (BurglarsNr > 2) {
					plMessage("RADIO_2", PLANING_MSG_REFRESH);
					SetPictID(((PersonNode *)dbGetObject(BurglarsList->getNthNode(CurrentPerson)->_nr))->PictID);
					dbObjectNode *help;
					dbObjectNode *node = BurglarsList->unLink(BurglarsList->getNthNode(CurrentPerson)->_name.c_str(), &help);

					Common::String exp = g_clue->_txtMgr->getFirstLine(PLAN_TXT, "EXPAND_RADIO");
					BurglarsList->expandObjectList(exp);

					choice1 = Bubble((NewList<NewNode>*)BurglarsList, 0, nullptr, 0);

					if (ChoiceOk(choice1, GET_OUT, BurglarsList))
						choice1 = BurglarsList->getNthNode(choice1)->_nr;
					else
						choice1 = GET_OUT;

					BurglarsList->link(node, help);
					dbRemObjectNode(BurglarsList, 0);
				} else {
					choice1 = CurrentPerson ? BurglarsList->getNthNode(0)->_nr : BurglarsList->getNthNode(1)->_nr;
					plMessage("RADIO_4", PLANING_MSG_WAIT | PLANING_MSG_REFRESH);
				}

				if (choice1 != GET_OUT) {
					if (InitAction(plSys, ACTION_WAIT_SIGNAL, choice1, 0, 1)) {
						PlanChanged = true;

						livAnimate(Planing_Name[CurrentPerson], ANM_STAND, 0, 0);
						plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys), PLANING_CORRECT_TIME, 1);
						livRefreshAll();
					} else {
						plSay("PLANING_END", CurrentPerson);
						activ = PLANING_WAIT_RETURN;
					}
				}
			} else
				plMessage("NO_RADIO", PLANING_MSG_WAIT);
			break;
		}
	}

	menu->removeList();
}

static void plLevelDesigner(LSObjectNode *lso) {
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(PLAN_TXT, "MENU_8");
	bool endLoop = false;
	byte activ = 0;
	uint16 originX = lso->us_DestX;
	uint16 originY = lso->us_DestY;
	uint32 area = lsGetActivAreaID();

	uint32 bitset = BIT(PLANING_LD_MOVE) + BIT(PLANING_LD_REFRESH) + BIT(PLANING_LD_OK) + BIT(PLANING_LD_CANCEL);

	while (!endLoop) {
		plDisplayTimer(0, true);
		plDisplayInfo();

		ShowMenuBackground();

		inpTurnESC(false);
		inpTurnFunctionKey(false);
		inpTurnMouse(false);

		activ = Menu(menu, bitset, activ, nullptr, 0);

		inpTurnMouse(true);
		inpTurnFunctionKey(true);
		inpTurnESC(true);

		switch (activ) {
		case PLANING_LD_MOVE:
			while (true) {
				int32 choice = inpWaitFor(INP_RIGHT | INP_LEFT | INP_UP | INP_DOWN | INP_LBUTTONP);

				if (choice & INP_LBUTTONP)
					break;

				if (choice & INP_RIGHT)
					lso->us_DestX++;

				if (choice & INP_LEFT)
					lso->us_DestX--;

				if (choice & INP_DOWN)
					lso->us_DestY++;

				if (choice & INP_UP)
					lso->us_DestY--;

				lsShowOneObject(lso, lso->us_DestX, lso->us_DestY, 32);
			}
			break;

		case PLANING_LD_REFRESH:
			lsDoneActivArea(area);
			lsInitActivArea(area, livGetXPos(Planing_Name[CurrentPerson]),
			                livGetYPos(Planing_Name[CurrentPerson]),
			                Planing_Name[CurrentPerson]);

			if (lsGetStartArea() == lsGetActivAreaID())
				lsShowEscapeCar();

			livRefreshAll();
			break;

		case PLANING_LD_OK: {
			char fileName[DSK_PATH_MAX];
			Common::String areaName = dbGetObjectName(area);
			areaName += ".dat";
			dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, areaName.c_str(), fileName);

			dbSaveAllObjects(fileName, ((LSAreaNode *) dbGetObject(area))->ul_ObjectBaseNr, 10000);
		}

		endLoop = true;
		break;

		case PLANING_LD_CANCEL:
			lso->us_DestX = originX;
			lso->us_DestY = originY;

			endLoop = true;
			break;
		}
	}

	menu->removeList();

	lsDoneActivArea(area);
	lsInitActivArea(area, livGetXPos(Planing_Name[CurrentPerson]), livGetYPos(Planing_Name[CurrentPerson]), Planing_Name[CurrentPerson]);

	if (lsGetStartArea() == lsGetActivAreaID())
		lsShowEscapeCar();

	livRefreshAll();
}

static void plActionOpenClose(uint16 what) {
	NewObjectList<dbObjectNode> *actionList = plGetObjectsList(CurrentPerson, false);

	if (actionList->isEmpty())
		plMessage("NO_OBJECTS", PLANING_MSG_WAIT);
	else {
		if (what == ACTION_OPEN)
			plMessage("OPEN", PLANING_MSG_REFRESH);
		else
			plMessage("CLOSE", PLANING_MSG_REFRESH);

		Common::String exp = g_clue->_txtMgr->getFirstLine(PLAN_TXT, "EXPAND_ALL");

		actionList->expandObjectList(exp);
		SetPictID(((PersonNode *) dbGetObject(PersonsList->getNthNode(CurrentPerson)->_nr))->PictID);

		uint32 choice1 = Bubble((NewList<NewNode>*)actionList, 0, nullptr, 0);

		if (ChoiceOk(choice1, GET_OUT, actionList)) {
			choice1 = actionList->getNthNode(choice1)->_nr;

			if (_gamePlayMode & GP_MODE_LEVEL_DESIGN)
				plLevelDesigner((LSObjectNode*)dbGetObject(choice1));
			else if ((CurrentPerson >= BurglarsNr) || !CHECK_STATE(lsGetObjectState(choice1), Const_tcIN_PROGRESS_BIT)) {
				if ((CurrentPerson >= BurglarsNr) || plIgnoreLock(choice1) || CHECK_STATE(lsGetObjectState(choice1), Const_tcLOCK_UNLOCK_BIT)) {
					uint32 state = CHECK_STATE(lsGetObjectState(choice1), Const_tcOPEN_CLOSE_BIT);

					if ((what == ACTION_OPEN) ? !state : state) {
						if (InitAction(plSys, what, choice1, 0, opensGet(((LSObjectNode *) dbGetObject(choice1))->Type,
						                  Tool_Hand) * PLANING_CORRECT_TIME)) {
							PlanChanged = true;

							if (CurrentPerson < BurglarsNr)
								plWork(CurrentPerson);

							plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
							       opensGet(((LSObjectNode *) dbGetObject(choice1))->Type, Tool_Hand) * PLANING_CORRECT_TIME, 1);

							lsSetObjectState(choice1, Const_tcOPEN_CLOSE_BIT, what == ACTION_OPEN ? 1 : 0);
							plCorrectOpened((LSObjectNode *)dbGetObject(choice1), what == ACTION_OPEN);

							plRefresh(choice1);
							livRefreshAll();
						} else
							plSay("PLANING_END", CurrentPerson);
					} else if (what == ACTION_OPEN)
						plMessage("OPEN_OPENED", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
					else
						plMessage("CLOSE_CLOSED", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
				} else
					plMessage("LOCKED", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
			} else
				plMessage("IN_PROGRESS", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
		}
	}

	actionList->removeList();
}

static void plActionTake() {
	NewList<dbObjectNode> *actionList = plGetObjectsList(CurrentPerson, true);

	if (actionList->isEmpty())
		plMessage("NO_OBJECTS", PLANING_MSG_WAIT);
	else {
		NewObjectList<dbObjectNode> *takeableList = new NewObjectList<dbObjectNode>;

		for (dbObjectNode *n = actionList->getListHead(); n->_succ; n = (dbObjectNode *) n->_succ) {
			SetObjectListAttr(OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_NORMAL, 0);
			AskAll(dbGetObject(n->_nr), hasLoot(CurrentPerson), BuildObjectList);

			uint32 state = lsGetObjectState(n->_nr);

			if (!ObjectList->isEmpty()) {
				if (CHECK_STATE(state, Const_tcTAKE_BIT)) {
					dbObjectNode *h2 = ObjectList->getListHead();
					dbObjectNode *h = takeableList->createNode(h2->_name);
					h->_nr = h2->_nr;  /* Loot */
					h->_type = n->_nr; /* Original */
					h->_fakePtr = false;
				} else if (CHECK_STATE(state, Const_tcOPEN_CLOSE_BIT)) {
					for (dbObjectNode *h2 = ObjectList->getListHead(); h2->_succ; h2 = (dbObjectNode *) h2->_succ) {
						dbObjectNode *h = takeableList->createNode(h2->_name);
						h->_nr = h2->_nr;  /* Loot */
						h->_type = n->_nr; /* Original */
						h->_fakePtr = true;
					}
				}
			}
		}

		if (!takeableList->isEmpty()) {
			plMessage("TAKE", PLANING_MSG_REFRESH);

			SetPictID(((PersonNode *) dbGetObject(BurglarsList->getNthNode(CurrentPerson)->_nr))->PictID);

			Common::String exp = g_clue->_txtMgr->getFirstLine(PLAN_TXT, "EXPAND_ALL");
			takeableList->expandObjectList(exp);

			uint32 choice = Bubble((NewList<NewNode>*)takeableList, 0, NULL, 0);

			if (ChoiceOk(choice, GET_OUT, takeableList)) {
				uint32 weightPerson = tcWeightPersCanCarry((PersonNode *)dbGetObject(BurglarsList->getNthNode(CurrentPerson)->_nr));
				uint32 volumePerson = tcVolumePersCanCarry((PersonNode *)dbGetObject(BurglarsList->getNthNode(CurrentPerson)->_nr));

				uint32 choice1 = takeableList->getNthNode(choice)->_nr;
				uint32 choice2 = takeableList->getNthNode(choice)->_type;

				uint32 weightLoot = ((LootNode *)dbGetObject(choice1))->Weight;
				uint32 volumeLoot = ((LootNode *)dbGetObject(choice1))->Volume;

				if (Planing_Weight[CurrentPerson] + weightLoot <= weightPerson) {
					if (Planing_Volume[CurrentPerson] + volumeLoot <= volumePerson) {
						if (InitAction(plSys, ACTION_TAKE, choice2, choice1, PLANING_TIME_TAKE * PLANING_CORRECT_TIME)) {
							PlanChanged = true;

							plWork(CurrentPerson);
							plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys), PLANING_TIME_TAKE * PLANING_CORRECT_TIME, 1);

							if (!takeableList->getNthNode(choice)) {
								if (choice2 >= 9701 && choice2 <= 9708) {
									lsRemLootBag(choice2);
									Planing_Loot[choice2 - 9701] = 0;
								} else {
									lsTurnObject((LSObjectNode *)dbGetObject(choice2), LS_OBJECT_INVISIBLE, LS_NO_COLLISION);
									lsSetObjectState(choice2, Const_tcACCESS_BIT, 0);

									plMessage("TAKEN_LOOT", PLANING_MSG_REFRESH);
								}
							}

							uint32 newValue = GetP(dbGetObject(choice2), hasLoot(CurrentPerson), dbGetObject(choice1));

							if (Ask(dbGetObject(BurglarsList->getNthNode(CurrentPerson)->_nr), take_RelId, dbGetObject(choice1))) {
								uint32 oldValue = GetP(dbGetObject(BurglarsList->getNthNode(CurrentPerson)->_nr), take_RelId, dbGetObject(choice1));

								SetP(dbGetObject(BurglarsList->getNthNode(CurrentPerson)->_nr), take_RelId, dbGetObject(choice1), oldValue + newValue);
							} else
								SetP(dbGetObject(BurglarsList->getNthNode(CurrentPerson)->_nr), take_RelId, dbGetObject(choice1), newValue);

							Planing_Weight[CurrentPerson] += weightLoot;
							Planing_Volume[CurrentPerson] += volumeLoot;

							plRefresh(choice2);
							livRefreshAll();

							SetP(dbGetObject(choice1), hasLoot(CurrentPerson), dbGetObject(choice1), GetP(dbGetObject(choice2), hasLoot(CurrentPerson), dbGetObject(choice1)));
							Present(choice1, "RasterObject", InitOneLootPresent);
							UnSet(dbGetObject(choice1), hasLoot(CurrentPerson), dbGetObject(choice1));

							UnSet(dbGetObject(choice2), hasLoot(CurrentPerson), dbGetObject(choice1));
						} else
							plSay("PLANING_END", CurrentPerson);
					} else
						plMessage("TOO_BIG", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
				} else
					plMessage("TOO_HEAVY", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
			}
		} else
			plMessage("NO_OBJECTS", PLANING_MSG_WAIT);

		takeableList->removeList();
	}

	actionList->removeList();
}

static Common::String plSetUseString(uint32 nr, uint32 type, dbObjectNode *node) {
	uint32 actLoudness =
	    lsGetLoudness(livGetXPos(Planing_Name[CurrentPerson]),
	                  livGetYPos(Planing_Name[CurrentPerson]));
	uint32 objLoudness = soundGet(((LSObjectNode *) dbGetObject(UseObject))->Type, nr);

	Common::String useString;

	if (break_(((LSObjectNode *) dbGetObject(UseObject))->Type, nr)) {
		useString = Common::String::format("%u %s",
		        tcGuyUsesTool(PersonsList->getNthNode(CurrentPerson)->_nr,
		                      (BuildingNode *) dbGetObject(Planing_BldId), nr,
		                      ((LSObjectNode *) dbGetObject(UseObject))->Type),
		        txtSeconds.c_str());

		if (objLoudness >= actLoudness) {
			useString += ", ";
			useString += txtTooLoud;
		}
	}

	return useString;
}

static bool plCheckAbilities(uint32 persId, uint32 checkToolId) {
	bool ret = true;

	toolRequiresAll(checkToolId, OLF_PRIVATE_LIST, Object_Ability);
	NewList<dbObjectNode> *requires = ObjectListPrivate;

	for (dbObjectNode *n = requires->getListHead(); n->_succ; n = (dbObjectNode *)n->_succ) {
		if (!has(persId, n->_nr)) {
			ret = false;
			break;
		}

		if (hasGet(persId, n->_nr) < toolRequiresGet(checkToolId, n->_nr)) {
			ret = false;
			break;
		}
	}

	requires->removeList();
	return ret;
}

static bool plCheckRequiredTools(uint32 checkToolId) {
	bool ret = true;

	toolRequiresAll(checkToolId, OLF_PRIVATE_LIST, Object_Tool);
	NewList<dbObjectNode> *trl = ObjectListPrivate;

	hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Tool);

	for (dbObjectNode *n = trl->getListHead(); n->_succ; n = (dbObjectNode *)n->_succ) {
		bool found = false;

		ret = false;

		for (dbObjectNode *h = ObjectList->getListHead(); h->_succ; h = (dbObjectNode *)h->_succ) {
			if (n->_nr == h->_nr)
				found = true;
		}

		if (found) {
			ret = true;
			break;
		}
	}

	trl->removeList();

	return ret;
}

static void plCorrectToolsList(uint32 flags) {
	/* only insert time-punch card if a guard has been neutralized */
	if (PersonsNr > BurglarsNr) {
		for (byte i = BurglarsNr; i < PersonsNr; i++) {
			if (Planing_Guard[i - BurglarsNr] == 2) {
				if (!dbHasObjectNode(ObjectList, Tool_Stabbing_card))
					dbAddObjectNode(ObjectList, Tool_Stabbing_card, flags);
				break;
			}
		}
	}

	dbRemObjectNode(ObjectList, Tool_Gloves);
	dbRemObjectNode(ObjectList, Tool_Shoes);
	dbRemObjectNode(ObjectList, Tool_Mask);
	dbRemObjectNode(ObjectList, Tool_Power_generation);
	dbRemObjectNode(ObjectList, Tool_Batterie);
	dbRemObjectNode(ObjectList, Tool_Protective_suit);
}

void plActionRadio() {
	if (has(Person_Matt_Stuvysunt, Tool_Radio_equipment)) {
		uint32 choice1;
		if (BurglarsNr > 2) {
			dbObjectNode* help;

			plMessage("RADIO_1", PLANING_MSG_REFRESH);
			SetPictID(((PersonNode*)dbGetObject(BurglarsList->getNthNode(CurrentPerson)->_nr))->PictID);
			dbObjectNode* node = BurglarsList->unLink(BurglarsList->getNthNode(CurrentPerson)->_name, &help);
			Common::String exp = g_clue->_txtMgr->getFirstLine(PLAN_TXT, "EXPAND_ALL");
			BurglarsList->expandObjectList(exp);

			choice1 = Bubble((NewList<NewNode>*)BurglarsList, 0, nullptr, 0);

			if (ChoiceOk(choice1, GET_OUT, BurglarsList))
				choice1 = BurglarsList->getNthNode(choice1)->_nr;
			else
				choice1 = GET_OUT;

			BurglarsList->link(node, help);
			dbRemObjectNode(BurglarsList, 0);
		} else {
			choice1 = CurrentPerson ? BurglarsList->getNthNode(0)->_nr : BurglarsList->getNthNode(1)->_nr;
			plMessage("RADIO_3", PLANING_MSG_WAIT | PLANING_MSG_REFRESH);
		}

		if (choice1 != GET_OUT) {
			if (InitAction(plSys, ACTION_SIGNAL, choice1, 0, PLANING_TIME_RADIO * PLANING_CORRECT_TIME)) {
				PlanChanged = true;

				livAnimate(Planing_Name[CurrentPerson], ANM_MAKE_CALL, 0, 0);
				plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys), PLANING_TIME_RADIO * PLANING_CORRECT_TIME, 1);
				livRefreshAll();
			} else
				plSay("PLANING_END", CurrentPerson);
		}
	} else
		plMessage("NO_RADIO", PLANING_MSG_WAIT);

}

void plActionDrop() {
	SetObjectListAttr(OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_NORMAL, 0);
	AskAll(dbGetObject(BurglarsList->getNthNode(CurrentPerson)->_nr), take_RelId, BuildObjectList);

	if (ObjectList->isEmpty())
		plMessage("DROP_1", PLANING_MSG_WAIT);
	else {
		plMessage("DROP_2", PLANING_MSG_REFRESH);

		SetPictID(((PersonNode*)dbGetObject(BurglarsList->getNthNode(CurrentPerson)->_nr))->PictID);
		Common::String exp = g_clue->_txtMgr->getFirstLine(PLAN_TXT, "EXPAND_ALL");
		ObjectList->expandObjectList(exp);

		uint32 choice1 = Bubble((NewList<NewNode>*)ObjectList, 0, nullptr, 0);

		if (ChoiceOk(choice1, GET_OUT, ObjectList)) {
			choice1 = ObjectList->getNthNode(choice1)->_nr;

			uint32 weightLoot = ((LootNode*)dbGetObject(choice1))->Weight;
			uint32 volumeLoot = ((LootNode*)dbGetObject(choice1))->Volume;

			uint32 choice2 = plGetNextLoot();
			if (choice2) {
				if (InitAction(plSys, ACTION_DROP, choice2, choice1, PLANING_TIME_DROP * PLANING_CORRECT_TIME)) {
					PlanChanged = true;

					plWork(CurrentPerson);
					plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys), PLANING_TIME_DROP * PLANING_CORRECT_TIME, 1);

					SetP(dbGetObject(choice2), hasLoot(CurrentPerson), dbGetObject(choice1),
						GetP(dbGetObject(BurglarsList->getNthNode(CurrentPerson)->_nr), take_RelId, dbGetObject(choice1)));
					UnSet(dbGetObject(BurglarsList->getNthNode(CurrentPerson)->_nr), take_RelId, dbGetObject(choice1));
					Planing_Weight[CurrentPerson] -= weightLoot;
					Planing_Volume[CurrentPerson] -= volumeLoot;

					plRefresh(choice2);
					livRefreshAll();
				} else
					plSay("PLANING_END", CurrentPerson);
			} else
				plMessage("DROP_3", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
		}
	}
}

static void plActionUse() {
	NewObjectList<dbObjectNode> *actionList = plGetObjectsList(CurrentPerson, false);

	if (CurrentPerson < BurglarsNr) {
		for (byte i = BurglarsNr; i < PersonsNr; i++)
			plInsertGuard(actionList, CurrentPerson, i);

		hasAll(Person_Matt_Stuvysunt, OLF_NORMAL | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Tool);
		plCorrectToolsList(OLF_INCLUDE_NAME | OLF_INSERT_STAR);

		if (ObjectList->isEmpty())
			plMessage("USE_1", PLANING_MSG_WAIT);
		else {
			if (actionList->isEmpty())
				plMessage("NO_OBJECTS", PLANING_MSG_WAIT);
			else {
				plMessage("USE_3", PLANING_MSG_REFRESH);

				SetPictID(((PersonNode *)dbGetObject(PersonsList->getNthNode(CurrentPerson)->_nr))->PictID);

				Common::String exp = g_clue->_txtMgr->getFirstLine(PLAN_TXT, "EXPAND_ALL");
				actionList->expandObjectList(exp);

				uint32 choice1 = Bubble((NewList<NewNode>*)actionList, 0, nullptr, 0);

				if (ChoiceOk(choice1, GET_OUT, actionList)) {
					choice1 = actionList->getNthNode(choice1)->_nr;

					if (plIsStair(choice1)) {
						uint32 newAreaId = StairConnectsGet(choice1, choice1);

						if (InitAction(plSys, ACTION_USE, choice1, lsGetActivAreaID(), PLANING_TIME_USE_STAIRS * PLANING_CORRECT_TIME)) {
							PlanChanged = true;

							livLivesInArea(Planing_Name[CurrentPerson], newAreaId);

							lsDoneActivArea(newAreaId);
							lsInitActivArea(newAreaId, livGetXPos(Planing_Name[CurrentPerson]), livGetYPos(Planing_Name[CurrentPerson]), Planing_Name[CurrentPerson]);

							if (lsGetStartArea() == lsGetActivAreaID())
								lsShowEscapeCar();  /* Auto neu zeichnen */

							livRefreshAll();

							plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys), PLANING_TIME_USE_STAIRS * PLANING_CORRECT_TIME, 1);
							lsSetActivLiving(Planing_Name[CurrentPerson], (uint16) -1, (uint16) -1);
						} else
							plSay("PLANING_END", CurrentPerson);
					} else if (dbIsObject(choice1, Object_Police)) {
						if (has(PersonsList->getNthNode(CurrentPerson)->_nr, Ability_Fight)) {
							NewObjectList<dbObjectNode> *objList = new NewObjectList<dbObjectNode>;

							dbAddObjectNode(objList, Tool_Hand, OLF_INCLUDE_NAME | OLF_INSERT_STAR);
							dbAddObjectNode(objList, Tool_Foot, OLF_INCLUDE_NAME | OLF_INSERT_STAR);

							if (has(Person_Matt_Stuvysunt, Tool_Chloroform))
								dbAddObjectNode(objList, Tool_Chloroform, OLF_INCLUDE_NAME | OLF_INSERT_STAR);

							exp = g_clue->_txtMgr->getFirstLine(PLAN_TXT, "EXPAND_ALL");
							objList->expandObjectList(exp);

							plMessage("USE_4", PLANING_MSG_REFRESH);

							SetPictID(((PersonNode *)dbGetObject(PersonsList->getNthNode(CurrentPerson)->_nr))->PictID);

							uint32 choice2 = Bubble((NewList<NewNode>*)objList, 0, nullptr, 0);

							if (ChoiceOk(choice2, GET_OUT, objList)) {
								choice2 = objList->getNthNode(choice2)->_nr;

								if (InitAction(plSys, ACTION_USE, choice1, choice2,
								         tcGuyUsesTool(PersonsList->getNthNode(CurrentPerson)->_nr,
											(BuildingNode *)dbGetObject(Planing_BldId), choice2, Item_Guard) * PLANING_CORRECT_TIME)) {
									PlanChanged = true;

									Planing_Guard[((PoliceNode *)dbGetObject(choice1))->LivingID - BurglarsNr] = 2;

									plWork(CurrentPerson);
									plSync(PLANING_ANIMATE_NO,
									       GetMaxTimer(plSys),
									       tcGuyUsesTool((PersonsList->getNthNode(CurrentPerson)->_nr),
									                     (BuildingNode *)dbGetObject(Planing_BldId),
									                     choice2, Item_Guard) * PLANING_CORRECT_TIME, 1);
									livRefreshAll();
								} else
									plSay("PLANING_END", CurrentPerson);
							}

							objList->removeList();
						} else
							plMessage("WRONG_ABILITY", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
					} else {
						uint32 state = lsGetObjectState(choice1);

						if (!CHECK_STATE(state, Const_tcIN_PROGRESS_BIT)) {
							if (plIgnoreLock(choice1) && !CHECK_STATE(state, Const_tcOPEN_CLOSE_BIT)) {
								switch (((LSObjectNode *) dbGetObject(choice1))->Type) {
								case Item_Alarm_system_Z3:
								case Item_Alarm_system_X3:
								case Item_Alarm_system_Top:
									plMessage("ALARM_OPEN", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
									break;

								case Item_Control_Box:
									plMessage("POWER_OPEN", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
									break;
								}
							} else {
								if (!CHECK_STATE(state, Const_tcLOCK_UNLOCK_BIT)) {
									plMessage("USE_2", PLANING_MSG_REFRESH);

									UseObject = choice1;
									ObjectListSuccString = plSetUseString;
									ObjectListWidth = 48;

									hasAll(Person_Matt_Stuvysunt,
									       OLF_NORMAL | OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_ADD_SUCC_STRING | OLF_ALIGNED, Object_Tool);
									plCorrectToolsList(OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_ADD_SUCC_STRING | OLF_ALIGNED);

									ObjectListWidth = 0;
									ObjectListSuccString = nullptr;
									UseObject = 0;

									exp = g_clue->_txtMgr->getFirstLine(PLAN_TXT, "EXPAND_ALL");
									ObjectList->expandObjectList(exp);

									SetPictID(((PersonNode *)dbGetObject(PersonsList->getNthNode(CurrentPerson)->_nr))->PictID);

									uint32 choice2 = Bubble((NewList<NewNode>*)ObjectList, 0, nullptr, 0);

									if (ChoiceOk(choice2, GET_OUT, ObjectList)) {
										choice2 = ObjectList->getNthNode(choice2)->_nr;

										if (plCheckAbilities(PersonsList->getNthNode(CurrentPerson)->_nr, choice2)) {
											if (plCheckRequiredTools(choice2)) {
												if (break_(((LSObjectNode *)dbGetObject(choice1))->Type, choice2)) {
													if (InitAction(plSys, ACTION_USE, choice1, choice2,
													         tcGuyUsesTool(PersonsList->getNthNode(CurrentPerson)->_nr,
													                       (BuildingNode *)dbGetObject(Planing_BldId), choice2,
													                       ((LSObjectNode *) dbGetObject(choice1))->Type) * PLANING_CORRECT_TIME)) {
														PlanChanged = true;

														plWork(CurrentPerson);
														plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
														 tcGuyUsesTool(PersonsList->getNthNode(CurrentPerson)->_nr,
														               (BuildingNode *)dbGetObject(Planing_BldId), choice2,
														               ((LSObjectNode *) dbGetObject(choice1))->Type) * PLANING_CORRECT_TIME, 1);

														if (!plIgnoreLock(choice1)) {
															lsSetObjectState(choice1, Const_tcLOCK_UNLOCK_BIT, 1);

															if (((ToolNode *)dbGetObject(choice2))->Effect & Const_tcTOOL_OPENS) {
																lsSetObjectState(choice1, Const_tcOPEN_CLOSE_BIT, 1);
																plCorrectOpened((LSObjectNode *) dbGetObject(choice1), true);
															}
														} else {
															state = lsGetObjectState(choice1);

															if (CHECK_STATE(state, Const_tcON_OFF)) {
																lsSetObjectState(choice1, Const_tcON_OFF, 0);   /* on setzen  */

																if (plIgnoreLock(choice1) == PLANING_POWER) {
																	lsSetSpotStatus(choice1, LS_SPOT_ON);
																	lsShowAllSpots(CurrentTimer(plSys), LS_ALL_VISIBLE_SPOTS);
																	plMessage("POWER_ON", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
																} else
																	plMessage("ALARM_ON", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
															} else {
																lsSetObjectState(choice1, Const_tcON_OFF, 1);   /* off setzen */

																if (plIgnoreLock(choice1) == PLANING_POWER) {
																	lsSetSpotStatus(choice1, LS_SPOT_OFF);
																	lsShowAllSpots(CurrentTimer(plSys), LS_ALL_INVISIBLE_SPOTS);
																	plMessage("POWER_OFF", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
																} else
																	plMessage("ALARM_OFF", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
															}
														}

														plRefresh(choice1);
														livRefreshAll();
													} else
														plSay("PLANING_END", CurrentPerson);
												} else
													plMessage("DOES_NOT_WORK", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
											} else
												plMessage("TOOL_DOES_NOT_WORK", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
										} else
											plMessage("WRONG_ABILITY", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
									}
								} else if (((LSObjectNode *) dbGetObject(choice1))->Type == Item_Window) {

									if (g_clue->getFeatures() & GF_PROFIDISK && Planing_BldId == Building_Mail_Train)
										plSay("PLANING_TRAIN", CurrentPerson);
									else if (CHECK_STATE(lsGetObjectState(choice1), Const_tcOPEN_CLOSE_BIT)) {
										if (has(Person_Matt_Stuvysunt, Tool_Rope_ladder)) {
											uint16 xpos, ypos;

											if (InitAction(plSys, ACTION_USE, choice1, 0,
											         PLANING_TIME_THROUGH_WINDOW * PLANING_CORRECT_TIME)) {
												PlanChanged = true;

												lsWalkThroughWindow((LSObjectNode *)dbGetObject(choice1),
												                    livGetXPos(Planing_Name[CurrentPerson]),
												                    livGetYPos(Planing_Name[CurrentPerson]),
												                    &xpos, &ypos);

												livSetPos(Planing_Name[CurrentPerson], xpos, ypos);
												plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
												       PLANING_TIME_THROUGH_WINDOW * PLANING_CORRECT_TIME, 1);
												livRefreshAll();
											} else
												plSay("PLANING_END", CurrentPerson);
										} else
											plMessage("NO_STAIRS", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
									} else
										plMessage("WALK_WINDOW", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
								} else
									plMessage("UNLOCK_UNLOCKED", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
							}
						} else
							plMessage("IN_PROGRESS", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
					}
				}
			}
		}
	} else {
		if (actionList->isEmpty())
			plMessage("NO_OBJECTS", PLANING_MSG_WAIT);
		else {
			plMessage("CONTROL", PLANING_MSG_REFRESH);

			SetPictID(((PersonNode *)dbGetObject(PersonsList->getNthNode(CurrentPerson)->_nr))->PictID);

			Common::String exp = g_clue->_txtMgr->getFirstLine(PLAN_TXT, "EXPAND_ALL");
			actionList->expandObjectList(exp);

			uint32 choice1 = Bubble((NewList<NewNode>*)actionList, 0, nullptr, 0);

			if (ChoiceOk(choice1, GET_OUT, actionList)) {
				choice1 = actionList->getNthNode(choice1)->_nr;

				if (InitAction(plSys, ACTION_CONTROL, choice1, 0, PLANING_TIME_CONTROL * PLANING_CORRECT_TIME)) {
					PlanChanged = true;

					plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys), PLANING_TIME_CONTROL * PLANING_CORRECT_TIME, 1);

					plRefresh(choice1);
					livRefreshAll();
				} else
					plSay("PLANING_END", CurrentPerson);
			}
		}
	}

	actionList->removeList();
}

static void plAction() {
	NewList<NewNode> *menu = NULL;
	if (CurrentPerson < BurglarsNr)
		menu = g_clue->_txtMgr->goKey(PLAN_TXT, "MENU_2");
	else
		menu = g_clue->_txtMgr->goKey(PLAN_TXT, "MENU_5");

	byte activ = 0;

	while (activ != PLANING_ACTION_RETURN) {
		uint32 choice1 = 0, bitset;
		if (CurrentPerson < BurglarsNr) {
			bitset = BIT(PLANING_PERSON_WALK) + BIT(PLANING_ACTION_USE) +
			         BIT(PLANING_ACTION_OPEN) + BIT(PLANING_ACTION_CLOSE) +
			         BIT(PLANING_ACTION_TAKE) + BIT(PLANING_ACTION_DROP) +
			         BIT(PLANING_ACTION_WAIT) + BIT(PLANING_ACTION_RETURN);


			if ((_gamePlayMode & GP_MODE_GUARD_DESIGN) ? (PersonsNr > 1) : (BurglarsNr > 1))
				bitset += BIT(PLANING_ACTION_RADIO);

			if (!(Planing_BldId == Building_Starford_Barracks) || (_gamePlayMode & GP_MODE_GUARD_DESIGN))
				bitset += BIT(PLANING_PERSON_CHANGE);
		} else {
			bitset = BIT(PLANING_PERSON_WALK) + BIT(PLANING_ACTION_USE) +
			         BIT(PLANING_ACTION_OPEN) + BIT(PLANING_ACTION_CLOSE) +
			         BIT(PLANING_ACTION_WAIT) + BIT(PLANING_ACTION_RETURN);

			if (PersonsNr > 1)
				bitset += BIT(PLANING_PERSON_CHANGE);
		}

		plDisplayTimer(0, true);
		plDisplayInfo();

		ShowMenuBackground();

		inpTurnESC(false);
		inpTurnFunctionKey(false);
		inpTurnMouse(false);

		activ = Menu(menu, bitset, activ, nullptr, 0);

		inpTurnMouse(true);
		inpTurnFunctionKey(true);
		inpTurnESC(true);

		switch (activ) {
		case PLANING_PERSON_WALK:
			plActionGo();
			break;

		case PLANING_PERSON_CHANGE:
			plMessage("CHANGE_PERSON_1", PLANING_MSG_REFRESH);

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
				uint32 oldTime = CurrentTimer(plSys);

				plMessage("PERSON_NOTES", PLANING_MSG_REFRESH);

				plPrepareSys(choice1, 0, PLANING_HANDLER_SET);

				if (livWhereIs(Planing_Name[choice1]) != lsGetActivAreaID()) {
					lsDoneActivArea(livWhereIs(Planing_Name[choice1]));
					lsInitActivArea(livWhereIs(Planing_Name[choice1]), livGetXPos(Planing_Name[choice1]),
					                livGetYPos(Planing_Name[choice1]), Planing_Name[choice1]);
				}

				if (oldTime < GetMaxTimer(plSys))
					plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys), GetMaxTimer(plSys) - oldTime, 1);

				if (oldTime > GetMaxTimer(plSys))
					plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys), oldTime - GetMaxTimer(plSys), 0);

				lsSetActivLiving(Planing_Name[CurrentPerson], (uint16) -1, (uint16) -1);

				menu->removeList();

				if (CurrentPerson < BurglarsNr)
					menu = g_clue->_txtMgr->goKey(PLAN_TXT, "MENU_2");
				else
					menu = g_clue->_txtMgr->goKey(PLAN_TXT, "MENU_5");
			}
			break;

		case PLANING_ACTION_USE:
			plActionUse();
			break;

		case PLANING_ACTION_OPEN:
			plActionOpenClose(ACTION_OPEN);
			break;

		case PLANING_ACTION_CLOSE:
			plActionOpenClose(ACTION_CLOSE);
			break;

		case PLANING_ACTION_TAKE:
			plActionTake();
			break;

		case PLANING_ACTION_DROP:
			plActionDrop();
			break;

		case PLANING_ACTION_WAIT:
			plActionWait();
			break;

		case PLANING_ACTION_RADIO:
			plActionRadio();
			break;
		}
	}

	menu->removeList();
}

static void plNoteBook() {
	NewList<NewNode> *bubble = g_clue->_txtMgr->goKey(PLAN_TXT, "MENU_6");

	uint32 choice1 = 0;
	while (choice1 != GET_OUT) {
		SetBubbleType(THINK_BUBBLE);

		choice1 = Bubble(bubble, choice1, nullptr, 0);
		uint32 choice2 = 0;

		switch (choice1) {
		case PLANING_NOTE_TARGET:
			Present(Organisation.BuildingID, "Building", InitBuildingPresent);
			break;

		case PLANING_NOTE_TEAM:
			while (choice2 != GET_OUT) {
				Common::String exp = g_clue->_txtMgr->getFirstLine(PLAN_TXT, "EXPAND_NOTEBOOK");
				BurglarsList->expandObjectList(exp);

				SetBubbleType(THINK_BUBBLE);

				choice2 = Bubble((NewList<NewNode>*)BurglarsList, choice2, nullptr, 0);

				if (ChoiceOk(choice2, GET_OUT, BurglarsList))
					Present(BurglarsList->getNthNode(choice2)->_nr, "Person", InitPersonPresent);
				else
					choice2 = GET_OUT;

				dbRemObjectNode(BurglarsList, 0);
			}
			break;

		case PLANING_NOTE_CAR:
			Present(Organisation.CarID, "Car", InitCarPresent);
			break;

		case PLANING_NOTE_TOOLS: {
			hasAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Tool);
			NewObjectList<dbObjectNode> *l = ObjectListPrivate;

			if (!l->isEmpty()) {
				Common::String exp = g_clue->_txtMgr->getFirstLine(PLAN_TXT, "EXPAND_NOTEBOOK");
				l->expandObjectList(exp);

				while (choice2 != GET_OUT) {
					SetBubbleType(THINK_BUBBLE);

					choice2 = Bubble((NewList<NewNode>*)l, choice2, nullptr, 0);

					if (ChoiceOk(choice2, GET_OUT, l))
						Present(l->getNthNode(choice2)->_nr, "Tool", InitToolPresent);
					else
						choice2 = GET_OUT;
				}
			}

			l->removeList();
			}
			break;

		default:
			choice1 = GET_OUT;
			break;
		}
	}

	bubble->removeList();
}

static void plLook() {
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(PLAN_TXT, "MENU_7");
	byte activ = 0;
	uint32 timer = 0, maxTimer = GetMaxTimer(plSys), realCurrentPerson = CurrentPerson, choice1;

	plMessage("PERSON_NOTES", PLANING_MSG_REFRESH);
	plSync(PLANING_ANIMATE_NO, timer, maxTimer, 0);
	lsSetActivLiving(Planing_Name[CurrentPerson], (uint16) -1, (uint16) -1);

	while (activ != PLANING_LOOK_RETURN) {
		plDisplayTimer(timer / PLANING_CORRECT_TIME, false);
		plDisplayInfo();

		ShowMenuBackground();

		uint32 bitset = BIT(PLANING_LOOK_PLAN) + BIT(PLANING_LOOK_RETURN);

		if ((CurrentPerson < BurglarsNr) ? BurglarsNr > 1 : PersonsNr > 1)
			bitset += BIT(PLANING_LOOK_PERSON_CHANGE);

		inpTurnFunctionKey(false);
		inpTurnESC(false);

		activ = Menu(menu, bitset, activ, nullptr, 0);

		inpTurnFunctionKey(true);
		inpTurnESC(true);

		switch (activ) {
		case PLANING_LOOK_PLAN: {
			byte last = 0;

			plMessage("LOOK_START", PLANING_MSG_REFRESH);

			inpTurnESC(false);
			inpTurnFunctionKey(false);
			inpTurnMouse(false);
			inpSetKeyRepeat((0 << 5) | 0);

			while (true) {
				plDisplayTimer(timer / PLANING_CORRECT_TIME, false);

				byte choice = inpWaitFor(INP_MOVEMENT | INP_LBUTTONP);

				if (choice & INP_LBUTTONP)
					break;

				if (!(choice & INP_MOUSE)) {
					if (choice & INP_RIGHT) {
						if (timer < maxTimer) {
							if (last != 1) {
								livSetPlayMode(LIV_PM_NORMAL);
								last = 1;
							}

							timer++;
							plSync(PLANING_ANIMATE_STD |
							       PLANING_ANIMATE_FOCUS, timer, 1, 1);
							livDoAnims((AnimCounter++) % 2, true);
						}
					}

					if (choice & INP_LEFT) {
						if (timer > 0) {
							if (last != 2) {
								livSetPlayMode(LIV_PM_REVERSE);
								last = 2;
							}

							timer--;
							plSync(PLANING_ANIMATE_STD | PLANING_ANIMATE_FOCUS, timer, 1, 0);
							livDoAnims((AnimCounter++) % 2, true);
						}
					}
				}
			}

			inpSetKeyRepeat((1 << 5) | 10);
			inpTurnMouse(true);
			inpTurnFunctionKey(true);
			inpTurnESC(true);
		}
		break;

		case PLANING_LOOK_PERSON_CHANGE:
			plMessage("CHANGE_PERSON_2", PLANING_MSG_REFRESH);

			if (PersonsNr > 2)
				choice1 = (uint32) Bubble((NewList<NewNode>*)PersonsList, CurrentPerson, nullptr, 0);
			else
				choice1 = CurrentPerson ? 0 : 1;

			if (choice1 != GET_OUT) {
				plPrepareSys(choice1, 0, PLANING_HANDLER_SET);
				maxTimer = GetMaxTimer(plSys);

				if (livWhereIs(Planing_Name[choice1]) != lsGetActivAreaID()) {
					lsDoneActivArea(livWhereIs(Planing_Name[choice1]));
					lsInitActivArea(livWhereIs(Planing_Name[choice1]), livGetXPos(Planing_Name[choice1]),
					                livGetYPos(Planing_Name[choice1]), Planing_Name[choice1]);
				}

				lsSetActivLiving(Planing_Name[CurrentPerson], (uint16) -1, (uint16) -1);
			}
			break;
		}
	}

	livSetPlayMode(LIV_PM_NORMAL);

	CurrentPerson = realCurrentPerson;
	plPrepareSys(CurrentPerson, 0, PLANING_HANDLER_SET);

	if (livWhereIs(Planing_Name[CurrentPerson]) != lsGetActivAreaID()) {
		lsDoneActivArea(livWhereIs(Planing_Name[CurrentPerson]));
		lsInitActivArea(livWhereIs(Planing_Name[CurrentPerson]), livGetXPos(Planing_Name[CurrentPerson]),
		                livGetYPos(Planing_Name[CurrentPerson]), Planing_Name[CurrentPerson]);
	}

	plMessage("PERSON_NOTES", PLANING_MSG_REFRESH);

	if (timer < GetMaxTimer(plSys))
		plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys), GetMaxTimer(plSys) - timer, 1);

	if (timer > GetMaxTimer(plSys))
		plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys), timer - GetMaxTimer(plSys), 0);

	lsSetActivLiving(Planing_Name[CurrentPerson], (uint16) -1, (uint16) -1);

	menu->removeList();
}

/* Planer */
void plPlaner(uint32 objId) {
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(PLAN_TXT, "MENU_1");
	byte activ = 0;

	plPrepareSys(0, objId, PLANING_INIT_PERSONSLIST | PLANING_HANDLER_ADD |
	    PLANING_HANDLER_OPEN | PLANING_GUARDS_LOAD | PLANING_HANDLER_SET);
	plPrepareGfx(objId, LS_COLL_PLAN, PLANING_GFX_LANDSCAPE |
		PLANING_GFX_SPRITES | PLANING_GFX_BACKGROUND);
	plPrepareRel();
	plPrepareData();

	AnimCounter = 0;
	PlanChanged = false;

	if ((Planing_BldId == Building_Starford_Barracks) && !(_gamePlayMode & GP_MODE_LEVEL_DESIGN)) {
		/* in the case of the Starford Barracks the plan must be loaded */
		plLoad(Planing_BldId);

		plPrepareSys(0, 0, PLANING_HANDLER_SET);

		plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys), GetMaxTimer(plSys), 1);
		lsSetActivLiving(Planing_Name[CurrentPerson], (uint16) -1, (uint16) -1);

		PlanChanged = false;
	}

	while (activ != PLANING_RETURN) {
		plDisplayTimer(0, true);
		plDisplayInfo();

		ShowMenuBackground();

		uint32 bitset = BIT(PLANING_START) + BIT(PLANING_NOTE) + BIT(PLANING_SAVE) +
			BIT(PLANING_LOAD) + BIT(PLANING_CLEAR) + BIT(PLANING_LOOK) + BIT(PLANING_RETURN);

		inpTurnESC(false);
		inpTurnFunctionKey(false);
		inpTurnMouse(false);

		activ = Menu(menu, bitset, activ, nullptr, 0);

		inpTurnMouse(true);
		inpTurnFunctionKey(true);
		inpTurnESC(true);

		switch (activ) {
		case PLANING_START:
			plAction();
			break;

		case PLANING_NOTE:
			plNoteBook();
			break;

		case PLANING_SAVE:
			if (!(_gamePlayMode & GP_MODE_GUARD_DESIGN) && !plAllInCar(objId))
				plSay("PLAN_NOT_FINISHED", 0);

			plSave(objId);

			PlanChanged = false;
			break;

		case PLANING_LOAD:
			plSaveChanged(objId);

			plMessage("PERSON_NOTES", PLANING_MSG_REFRESH);
			plSync(PLANING_ANIMATE_NO, 0, GetMaxTimer(plSys), 0);

			plPrepareSys(0, 0, PLANING_HANDLER_CLEAR);
			plPrepareData();

			plLoad(objId);

			plPrepareSys(0, 0, PLANING_HANDLER_SET);

			plMessage("PERSON_NOTES", PLANING_MSG_REFRESH);
			plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys), GetMaxTimer(plSys), 1);
			lsSetActivLiving(Planing_Name[CurrentPerson], (uint16) -1, (uint16) -1);

			PlanChanged = false;
			break;

		case PLANING_CLEAR:
			plSaveChanged(objId);

			plMessage("CLEAR_PLAN", PLANING_MSG_REFRESH);

			plSync(PLANING_ANIMATE_NO, 0, GetMaxTimer(plSys), 0);

			plPrepareSys(0, 0, PLANING_HANDLER_CLEAR | PLANING_HANDLER_SET);
			plPrepareData();

			lsSetActivLiving(Planing_Name[CurrentPerson], (uint16) -1, (uint16) -1);

			PlanChanged = false;
			break;

		case PLANING_LOOK:
			plLook();
			break;
		}
	}

	plSaveChanged(objId);

	plUnprepareRel();
	plUnprepareGfx();
	plUnprepareSys();

	menu->removeList();
}

} // End of namespace Clue

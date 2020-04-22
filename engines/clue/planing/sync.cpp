/*
**      $Filename: planing/sync.c
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.sync for "Der Clou!"
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

#include "clue/planing/sync.h"

namespace Clue {

/* Sync functions */
static uint16 plXMoveSync(uint32 id, uint16 xpos, byte animate, byte direction, uint16 gowhere) {
	if (direction) {
		if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
			lsInitScrollLandScape(gowhere, LS_SCROLL_PREPARE);

		switch (gowhere) {
		case DIRECTION_LEFT:
			if (animate & PLANING_ANIMATE_STD)
				livAnimate(Planing_Name[id], ANM_MOVE_LEFT,
				           -1 * LS_STD_SCROLL_SPEED, 0);

			if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
				lsScrollLandScape((byte) - 1);

			xpos = (uint16)(xpos - LS_STD_SCROLL_SPEED);
			break;

		case DIRECTION_RIGHT:
			if (animate & PLANING_ANIMATE_STD)
				livAnimate(Planing_Name[id], ANM_MOVE_RIGHT,
				           1 * LS_STD_SCROLL_SPEED, 0);

			if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
				lsScrollLandScape((byte) - 1);

			xpos = (uint16)(xpos + LS_STD_SCROLL_SPEED);
			break;
		}
	} else {
		if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson)) {
			switch (gowhere) {
			case DIRECTION_LEFT:
				lsInitScrollLandScape(DIRECTION_RIGHT, LS_SCROLL_PREPARE);
				break;

			case DIRECTION_RIGHT:
				lsInitScrollLandScape(DIRECTION_LEFT, LS_SCROLL_PREPARE);
				break;
			}
		}

		switch (gowhere) {
		case DIRECTION_LEFT:
			if (animate & PLANING_ANIMATE_STD)
				livAnimate(Planing_Name[id], ANM_MOVE_LEFT,
				           1 * LS_STD_SCROLL_SPEED, 0);

			if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
				lsScrollLandScape((byte) - 1);

			xpos = (uint16)(xpos + LS_STD_SCROLL_SPEED);
			break;

		case DIRECTION_RIGHT:
			if (animate & PLANING_ANIMATE_STD)
				livAnimate(Planing_Name[id], ANM_MOVE_RIGHT,
				           -1 * LS_STD_SCROLL_SPEED, 0);

			if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
				lsScrollLandScape((byte) - 1);

			xpos = (uint16)(xpos - LS_STD_SCROLL_SPEED);
			break;
		}
	}

	return xpos;
}

static uint16 plYMoveSync(uint32 id, uint16 ypos, byte animate, byte direction, uint16 gowhere) {
	if (direction) {
		if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
			lsInitScrollLandScape(gowhere, LS_SCROLL_PREPARE);

		switch (gowhere) {
		case DIRECTION_UP:
			if (animate & PLANING_ANIMATE_STD)
				livAnimate(Planing_Name[id], ANM_MOVE_UP, 0,
				           -1 * LS_STD_SCROLL_SPEED);

			if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
				lsScrollLandScape((byte) - 1);

			ypos = (uint16)(ypos - LS_STD_SCROLL_SPEED);
			break;

		case DIRECTION_DOWN:
			if (animate & PLANING_ANIMATE_STD)
				livAnimate(Planing_Name[id], ANM_MOVE_DOWN, 0,
				           1 * LS_STD_SCROLL_SPEED);

			if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
				lsScrollLandScape((byte) - 1);

			ypos = (uint16)(ypos + LS_STD_SCROLL_SPEED);
			break;
		}
	} else {
		if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson)) {
			switch (gowhere) {
			case DIRECTION_UP:
				lsInitScrollLandScape(DIRECTION_DOWN, LS_SCROLL_PREPARE);
				break;

			case DIRECTION_DOWN:
				lsInitScrollLandScape(DIRECTION_UP, LS_SCROLL_PREPARE);
				break;
			}
		}

		switch (gowhere) {
		case DIRECTION_UP:
			if (animate & PLANING_ANIMATE_STD)
				livAnimate(Planing_Name[id], ANM_MOVE_UP, 0,
				           1 * LS_STD_SCROLL_SPEED);

			if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
				lsScrollLandScape((byte) - 1);

			ypos = (uint16)(ypos + LS_STD_SCROLL_SPEED);
			break;

		case DIRECTION_DOWN:
			if (animate & PLANING_ANIMATE_STD)
				livAnimate(Planing_Name[id], ANM_MOVE_DOWN, 0,
				           -1 * LS_STD_SCROLL_SPEED);

			if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
				lsScrollLandScape((byte) - 1);

			ypos = (uint16)(ypos - LS_STD_SCROLL_SPEED);
			break;
		}
	}

	return ypos;
}

void plSync(byte animate, uint32 targetTime, uint32 times, byte direction) {
	uint32 lastAreaId = 0;

	for (uint32 seconds = 0; seconds < times; seconds++) {
		for (byte i = 0; i < PersonsNr; i++) {
			uint16 xpos = livGetXPos(Planing_Name[i]);
			uint16 ypos = livGetYPos(Planing_Name[i]);

			SetActivHandler(plSys, PersonsList->getNthNode(i)->_nr);

			if (CurrentTimer(plSys) == targetTime)
				continue;

			if (!direction && (i < BurglarsNr) && (GetMaxTimer(plSys) < targetTime + times - seconds))
				continue;

			if ((i >= BurglarsNr) && Planing_Guard[i - BurglarsNr]) {
				if (animate & PLANING_ANIMATE_STD) {
					switch (Planing_Guard[i - BurglarsNr]) {
					case 1:
						livAnimate(Planing_Name[i],
						           livGetViewDirection(Planing_Name[i]), 0, 0);
						break;

					case 2:
						livAnimate(Planing_Name[i], ANM_DUSEL_POLICE, 0, 0);
						break;
					}
				}
			} else {
				ActionNode *action;
				if (direction)
					action = NextAction(plSys);
				else
					action = CurrentAction(plSys);

				if (!action) {
					if (direction)
						GoLastAction(plSys);
					else
						GoFirstAction(plSys);

					if (animate & PLANING_ANIMATE_STD)
						livAnimate(Planing_Name[i], ANM_STAND, 0, 0);
				} else {
					if (action->Type != ACTION_GO) {
						if (animate & PLANING_ANIMATE_STD) {
							if (i < BurglarsNr) {
								if ((action->Type == ACTION_WAIT)
								        || (action->Type == ACTION_WAIT_SIGNAL))
									livAnimate(Planing_Name[i], ANM_STAND, 0,
									           0);
								else if (action->Type == ACTION_SIGNAL)
									livAnimate(Planing_Name[i], ANM_MAKE_CALL,
									           0, 0);
								else
									plWork(i);
							} else
								livAnimate(Planing_Name[i], ANM_WORK_CONTROL, 0,
								           0);
						}
					}

					switch (action->Type) {
					case ACTION_GO:
						xpos = plXMoveSync(i, xpos, animate, direction, ((ActionGoNode *)action)->Direction);
						ypos = plYMoveSync(i, ypos, animate, direction, ((ActionGoNode *)action)->Direction);
						break;

					case ACTION_USE:
						if (ActionStarted(plSys)) {
							if (plIsStair(((ActionUseNode *)action)->ItemId)) {
								if (direction)
									livLivesInArea(Planing_Name[i],
										StairConnectsGet(((ActionUseNode *)action)->ItemId,
										((ActionUseNode *)action)->ItemId));
								else
									livLivesInArea(Planing_Name[i], ((ActionUseNode *)action)->ToolId);
							} else if (dbIsObject(((ActionUseNode *)action)->ItemId, Object_Police)) {
								PoliceNode *pol = (PoliceNode *) dbGetObject(((ActionUseNode *)action)->ItemId);

								if (direction)
									Planing_Guard[pol->LivingID - BurglarsNr] = 1;
								else
									Planing_Guard[pol->LivingID - BurglarsNr] = 0;
							} else {
								if (direction)
									lsSetObjectState(((ActionUseNode *)action)->ItemId,
									                 Const_tcIN_PROGRESS_BIT,
									                 1);
								else
									lsSetObjectState(((ActionUseNode *)action)->
									                 ItemId,
									                 Const_tcIN_PROGRESS_BIT,
									                 0);
							}
						}

						if (ActionEnded(plSys)) {
							if (plIsStair(((ActionUseNode *)action)->ItemId)) {
								if (i == CurrentPerson) {
									if (direction)
										lastAreaId =
										    StairConnectsGet(((ActionUseNode *)action)->ItemId,
										((ActionUseNode *)action)->ItemId);
									else
										lastAreaId = ((ActionUseNode *)action)->ToolId;
								}
							} else if (dbIsObject(((ActionUseNode *)action)->ItemId, Object_Police)) {
								PoliceNode *pol = (PoliceNode *) dbGetObject(((ActionUseNode *)action)->ItemId);

								if (direction)
									Planing_Guard[pol->LivingID - BurglarsNr] = 2;
								else
									Planing_Guard[pol->LivingID - BurglarsNr] = 1;
							} else {
								if (direction) {
									lsSetObjectState(((ActionUseNode *)action)->
									                 ItemId,
									                 Const_tcIN_PROGRESS_BIT,
									                 0);

									if (!plIgnoreLock(((ActionUseNode *)action)->ItemId)) {
										if (!CHECK_STATE
										        (lsGetObjectState
										         (((ActionUseNode *)action)->ItemId),
										         Const_tcLOCK_UNLOCK_BIT)) {
											lsSetObjectState(((ActionUseNode *)action)->ItemId,
											                 Const_tcLOCK_UNLOCK_BIT,
											                 1);

											if (((ToolNode *)
											        dbGetObject(((ActionUseNode *)action)->ToolId))->
											        Effect & Const_tcTOOL_OPENS) {
												lsSetObjectState(((ActionUseNode *)action)->ItemId,
												                 Const_tcOPEN_CLOSE_BIT,
												                 1);
												plCorrectOpened((LSObjectNode *)
												                dbGetObject(((ActionUseNode *)action)->ItemId), 1);
											}
										} else {
											if ((((LSObjectNode *)
											        dbGetObject(((ActionUseNode *)action)->ItemId))->Type == Item_Fenster)) {
												lsWalkThroughWindow((LSObjectNode *)
												                    dbGetObject
												                    (((ActionUseNode *)action)->ItemId),
												                    xpos, ypos,
												                    &xpos,
												                    &ypos);
												livSetPos(Planing_Name[i], xpos,
												          ypos);

												livRefreshAll();
											}
										}
									}
								} else {
									lsSetObjectState(((ActionUseNode *)action)->ItemId,
									                 Const_tcIN_PROGRESS_BIT,
									                 1);

									if (!plIgnoreLock(((ActionUseNode *)action)->ItemId)) {
										if (CHECK_STATE
										        (lsGetObjectState
										         (((ActionUseNode *)action)->ItemId),
										         Const_tcLOCK_UNLOCK_BIT)) {
											if ((((LSObjectNode *)
											        dbGetObject(((ActionUseNode *)action)->ItemId))->Type == Item_Fenster)
											        && !((ActionUseNode *)action)->ToolId) {
												lsWalkThroughWindow((LSObjectNode *)
												                    dbGetObject
												                    (((ActionUseNode *)action)->
												                     ItemId),
												                    xpos, ypos,
												                    &xpos,
												                    &ypos);
												livSetPos(Planing_Name[i], xpos,
												          ypos);

												livRefreshAll();
											} else {
												lsSetObjectState(((ActionUseNode *)action)->ItemId,
												                 Const_tcLOCK_UNLOCK_BIT,
												                 0);

												if (((ToolNode *)
												        dbGetObject(((ActionUseNode *)action)->ToolId))->Effect & Const_tcTOOL_OPENS) {
													lsSetObjectState(((ActionUseNode *)action)->ItemId,
													                 Const_tcOPEN_CLOSE_BIT,
													                 0);
													plCorrectOpened((LSObjectNode *)
													                dbGetObject
													                (((ActionUseNode *)action)->ItemId),
													                0);
												}
											}
										}
									}
								}

								if (plIgnoreLock(((ActionUseNode *)action)->ItemId)) {
									uint32 state = lsGetObjectState(((ActionUseNode *)action)->ItemId);

									if (CHECK_STATE(state, Const_tcON_OFF)) {
										lsSetObjectState(((ActionUseNode *)action)->ItemId, Const_tcON_OFF, 0);    /* on setzen  */

										if (plIgnoreLock(((ActionUseNode *)action)->ItemId) == PLANING_POWER) {
											lsSetSpotStatus(((ActionUseNode *)action)->ItemId, LS_SPOT_ON);
											lsShowAllSpots(CurrentTimer(plSys), LS_ALL_VISIBLE_SPOTS);
										}
									} else {
										lsSetObjectState(((ActionUseNode *)action)->ItemId, Const_tcON_OFF, 1);    /* off setzen */

										if (plIgnoreLock(((ActionUseNode *)action)->ItemId) == PLANING_POWER) {
											lsSetSpotStatus(((ActionUseNode *)action)->ItemId, LS_SPOT_OFF);
											lsShowAllSpots(CurrentTimer(plSys), LS_ALL_INVISIBLE_SPOTS);
										}
									}
								}

								plRefresh(((ActionUseNode *)action)->ItemId);
							}
						}
						break;

					case ACTION_TAKE:
						if (ActionStarted(plSys)) {
							if (direction)
								lsSetObjectState(((ActionTakeNode *)action)->ItemId, Const_tcIN_PROGRESS_BIT, 1);
							else
								lsSetObjectState(((ActionTakeNode *)action)->ItemId, Const_tcIN_PROGRESS_BIT, 0);
						}

						if (ActionEnded(plSys)) {
							if (direction) {
								uint32 weightLoot =
								    ((LootNode *) dbGetObject(((ActionTakeNode *)action)->LootId))->Weight;
								uint32 volumeLoot =
								    ((LootNode *) dbGetObject(((ActionTakeNode *)action)->LootId))->Volume;

								lsSetObjectState(((ActionTakeNode *)action)->ItemId, Const_tcIN_PROGRESS_BIT, 0);

								if (((ActionTakeNode *)action)->ItemId >= 9701
								        && ((ActionTakeNode *)action)->ItemId <= 9708) {
									lsRemLootBag(((ActionTakeNode *)action)->ItemId);
									Planing_Loot[((ActionTakeNode *)action)->ItemId - 9701] = 0;
								} else {
									if (CHECK_STATE(lsGetObjectState(((ActionTakeNode *)action)->ItemId), Const_tcTAKE_BIT)) {
										lsTurnObject((LSObjectNode *)
										             dbGetObject(((ActionTakeNode *)action)->ItemId),
										             LS_OBJECT_INVISIBLE,
										             LS_NO_COLLISION);
										lsSetObjectState(((ActionTakeNode *)action)->
										                 ItemId,
										                 Const_tcACCESS_BIT, 0);
									}
								}

								{
									uint32 newValue =
									    GetP(dbGetObject
									         (((ActionTakeNode *)action)->ItemId), hasLoot(i),
									         dbGetObject(((ActionTakeNode *)action)->LootId));

									if (Ask(dbGetObject(BurglarsList->getNthNode(i)->_nr), take_RelId,
									         dbGetObject(((ActionTakeNode *)action)->LootId))) {
										uint32 oldValue =
										    GetP(dbGetObject(BurglarsList->getNthNode(i)->_nr),
										         take_RelId,
										         dbGetObject(((ActionTakeNode *)action)->LootId));

										SetP(dbGetObject(BurglarsList->getNthNode(i)->_nr),
										     take_RelId,
										     dbGetObject(((ActionTakeNode *)action)->
										                 LootId),
										     oldValue + newValue);
									} else
										SetP(dbGetObject(BurglarsList->getNthNode(i)->_nr),
										     take_RelId,
										     dbGetObject(((ActionTakeNode *)action)->
										                 LootId), newValue);
								}

								UnSet(dbGetObject
								      (((ActionTakeNode *)action)->ItemId), hasLoot(i),
								      dbGetObject(((ActionTakeNode *)action)->LootId));

								Planing_Weight[i] += weightLoot;
								Planing_Volume[i] += volumeLoot;
							} else {
								uint32 weightLoot = ((LootNode *)dbGetObject(((ActionTakeNode *)action)->LootId))->Weight;
								uint32 volumeLoot = ((LootNode *)dbGetObject(((ActionTakeNode *)action)->LootId))->Volume;

								lsSetObjectState(((ActionTakeNode *)action)->ItemId,
								                 Const_tcIN_PROGRESS_BIT, 1);

								if (((ActionTakeNode *)action)->ItemId >= 9701 && ((ActionTakeNode *)action)->ItemId <= 9708) {
									lsAddLootBag(xpos, ypos, ((ActionTakeNode *)action)->ItemId - 9700);
									Planing_Loot[((ActionTakeNode *)action)->ItemId - 9701] = 1;
									SetP(dbGetObject
									     (((ActionTakeNode *)action)->ItemId),
									     hasLoot(i),
									     dbGetObject(((ActionTakeNode *)action)->LootId),
									     GetP(dbGetObject(PersonsList->getNthNode(i)->_nr),
									          take_RelId,
									          dbGetObject(((ActionTakeNode *)action)->LootId)));
								} else {
									if (CHECK_STATE
									        (lsGetObjectState
									         (((ActionTakeNode *)action)->ItemId),
									         Const_tcTAKE_BIT)) {
										lsTurnObject((LSObjectNode *)
										             dbGetObject(((ActionTakeNode *)action)->ItemId),
										             LS_OBJECT_VISIBLE,
										             LS_COLLISION);
										lsSetObjectState(((ActionTakeNode *)action)->
										                 ItemId,
										                 Const_tcACCESS_BIT, 1);
									}

									SetP(dbGetObject
									     (((ActionTakeNode *)action)->ItemId),
									     hasLoot(i),
									     dbGetObject(((ActionTakeNode *)action)->LootId),
									     GetP(dbGetObject
									          (((ActionTakeNode *)action)->ItemId),
									          hasLoot_Clone_RelId,
									          dbGetObject(((ActionTakeNode *)action)->LootId)));
								}

								UnSet(dbGetObject
								      (PersonsList->getNthNode(i)->_nr),
								      take_RelId,
								      dbGetObject(((ActionTakeNode *)action)->LootId));
								Planing_Weight[i] -= weightLoot;
								Planing_Volume[i] -= volumeLoot;
							}

							plRefresh(((ActionTakeNode *)action)->ItemId);
						}
						break;

					case ACTION_DROP:
						if (ActionStarted(plSys)) {
							if (direction)
								lsSetObjectState(((ActionDropNode *)action)->ItemId,
								                 Const_tcIN_PROGRESS_BIT, 1);
							else
								lsSetObjectState(((ActionDropNode *)action)->ItemId,
								                 Const_tcIN_PROGRESS_BIT, 0);
						}

						if (ActionEnded(plSys)) {
							if (direction) {
								uint32 weightLoot = ((LootNode *)dbGetObject(((ActionDropNode *)action)->LootId))->Weight;
								uint32 volumeLoot = ((LootNode *)dbGetObject(((ActionDropNode *)action)->LootId))->Volume;

								lsSetObjectState(((ActionDropNode *)action)->ItemId, Const_tcIN_PROGRESS_BIT, 1);

								if (((ActionDropNode *)action)->ItemId >= 9701 && ((ActionDropNode *)action)->ItemId <= 9708) {
									lsAddLootBag(xpos, ypos, ((ActionDropNode *)action)->ItemId - 9700);
									Planing_Loot[((ActionDropNode *)action)->ItemId - 9701] = 1;
									SetP(dbGetObject
									     (((ActionDropNode *)action)->ItemId),
									     hasLoot(i),
									     dbGetObject(((ActionDropNode *)action)->LootId),
									     GetP(dbGetObject(PersonsList->getNthNode(i)->_nr),
									          take_RelId,
									          dbGetObject(((ActionDropNode *)action)->LootId)));
								} else {
									if (CHECK_STATE
									        (lsGetObjectState
									         (((ActionDropNode *)action)->ItemId),
									         Const_tcTAKE_BIT)) {
										lsTurnObject((LSObjectNode *)
										             dbGetObject(((ActionDropNode *)action)->ItemId),
										             LS_OBJECT_VISIBLE,
										             LS_COLLISION);
										lsSetObjectState(((ActionDropNode *)action)->
										                 ItemId,
										                 Const_tcACCESS_BIT, 1);
									}

									SetP(dbGetObject(((ActionDropNode *)action)->ItemId),
									     hasLoot(i),
									     dbGetObject(((ActionDropNode *)action)->LootId),
									     GetP(dbGetObject(((ActionDropNode *)action)->ItemId),
									          hasLoot_Clone_RelId,
									          dbGetObject(((ActionDropNode *)action)->LootId)));
								}

								UnSet(dbGetObject(PersonsList->getNthNode(i)->_nr),
								      take_RelId,
								      dbGetObject(((ActionDropNode *)action)->LootId));
								Planing_Weight[i] -= weightLoot;
								Planing_Volume[i] -= volumeLoot;
							} else {
								uint32 weightLoot =
								    ((LootNode *)
								     dbGetObject(((ActionDropNode *)action)->LootId))->Weight;
								uint32 volumeLoot =
								    ((LootNode *)
								     dbGetObject(((ActionDropNode *)action)->LootId))->Volume;

								lsSetObjectState(((ActionDropNode *)action)->ItemId, Const_tcIN_PROGRESS_BIT, 0);

								if (((ActionDropNode *)action)->ItemId >= 9701 && ((ActionDropNode *)action)->ItemId <= 9708) {
									lsRemLootBag(((ActionDropNode *)action)->ItemId);
									Planing_Loot[((ActionDropNode *)action)->ItemId - 9701] = 0;
								} else {
									if (CHECK_STATE(lsGetObjectState(((ActionTakeNode *)action)->ItemId), Const_tcTAKE_BIT)) {
										lsTurnObject((LSObjectNode *)
										             dbGetObject(((ActionTakeNode *)action)->ItemId),
										             LS_OBJECT_INVISIBLE,
										             LS_NO_COLLISION);
										lsSetObjectState(((ActionTakeNode *)action)->
										                 ItemId,
										                 Const_tcACCESS_BIT, 0);
									}
								}

								{
									uint32 newValue =
									    GetP(dbGetObject
									         (((ActionDropNode *)action)->ItemId),
									         hasLoot(i),
									         dbGetObject(((ActionDropNode *)action)->
									                     LootId));

									if (Ask
									        (dbGetObject(BurglarsList->getNthNode(i)->_nr),
									         take_RelId,
									         dbGetObject(((ActionDropNode *)action)->LootId))) {
										uint32 oldValue =
										    GetP(dbGetObject(BurglarsList->getNthNode(i)->_nr),
										         take_RelId,
										         dbGetObject(((ActionDropNode *)action)->LootId));

										SetP(dbGetObject(BurglarsList->getNthNode(i)->_nr),
										     take_RelId,
										     dbGetObject(((ActionDropNode *)action)->LootId),
										     oldValue + newValue);
									} else
										SetP(dbGetObject(BurglarsList->getNthNode(i)->_nr),
										     take_RelId,
										     dbGetObject(((ActionDropNode *)action)->LootId), newValue);
								}

								UnSet(dbGetObject
								      (((ActionDropNode *)action)->
								       ItemId), hasLoot(i),
								      dbGetObject(((ActionDropNode *)action)->LootId));

								Planing_Weight[i] += weightLoot;
								Planing_Volume[i] += volumeLoot;
							}

							plRefresh(((ActionDropNode *)action)->ItemId);
						}
						break;

					case ACTION_OPEN:
						if (ActionStarted(plSys)) {
							if (i < BurglarsNr) {
								if (direction)
									lsSetObjectState(((ActionOpenNode *)action)->ItemId,
									                 Const_tcIN_PROGRESS_BIT,
									                 1);
								else
									lsSetObjectState(((ActionOpenNode *)action)->ItemId,
									                 Const_tcIN_PROGRESS_BIT,
									                 0);
							}
						}

						if (ActionEnded(plSys)) {
							if (direction) {
								if (i < BurglarsNr)
									lsSetObjectState(((ActionOpenNode *)action)->ItemId,
									                 Const_tcIN_PROGRESS_BIT,
									                 0);

								lsSetObjectState(((ActionOpenNode *)action)->ItemId, Const_tcOPEN_CLOSE_BIT, 1);

								plCorrectOpened((LSObjectNode *)dbGetObject(((ActionOpenNode *)action)->ItemId), 1);
							} else {
								if (i < BurglarsNr)
									lsSetObjectState(((ActionOpenNode *)action)->ItemId,
									                 Const_tcIN_PROGRESS_BIT,
									                 1);

								lsSetObjectState(((ActionOpenNode *)action)->ItemId,
								                 Const_tcOPEN_CLOSE_BIT, 0);

								plCorrectOpened((LSObjectNode *)dbGetObject(((ActionOpenNode *)action)->ItemId), 0);
							}

							plRefresh(((ActionOpenNode *)action)->ItemId);
						}
						break;

					case ACTION_CLOSE:
						if (ActionStarted(plSys)) {
							if (i < BurglarsNr) {
								if (direction)
									lsSetObjectState(((ActionCloseNode *)action)->
									                 ItemId,
									                 Const_tcIN_PROGRESS_BIT,
									                 1);
								else
									lsSetObjectState(((ActionCloseNode *)action)->
									                 ItemId,
									                 Const_tcIN_PROGRESS_BIT,
									                 0);
							}
						}

						if (ActionEnded(plSys)) {
							if (direction) {
								if (i < BurglarsNr)
									lsSetObjectState(((ActionCloseNode *)action)->ItemId,
									                 Const_tcIN_PROGRESS_BIT,
									                 0);

								lsSetObjectState(((ActionCloseNode *)action)->ItemId,
								                 Const_tcOPEN_CLOSE_BIT, 0);

								plCorrectOpened((LSObjectNode *)
								                dbGetObject(((ActionCloseNode *)action)->ItemId), 0);
							} else {
								if (i < BurglarsNr)
									lsSetObjectState(((ActionCloseNode *)action)->ItemId,
									                 Const_tcIN_PROGRESS_BIT,
									                 1);

								lsSetObjectState(((ActionCloseNode *)action)->ItemId,
								                 Const_tcOPEN_CLOSE_BIT, 1);

								plCorrectOpened((LSObjectNode *)
								                dbGetObject(((ActionCloseNode *)action)->ItemId), 1);
							}

							plRefresh(((ActionCloseNode *)action)->ItemId);
						}
						break;
					}
				}

				if (!direction)
					action = PrevAction(plSys);
			}

			if (animate & PLANING_ANIMATE_NO) {
				HandlerNode *h = plSys->ActivHandler;
				uint16 dir;

				if (i < BurglarsNr)
					dir = ANM_STAND;
				else
					dir = ANM_MOVE_DOWN;

				if (CurrentTimer(plSys) != 0) {
					for (ActionNode *a = h->Actions->getListHead(); a->_succ; a = (ActionNode *) a->_succ) {
						switch (a->Type) {
						case ACTION_GO:
							switch (((ActionGoNode *)a)->Direction) {
							case DIRECTION_LEFT:
								dir = ANM_MOVE_LEFT;
								break;

							case DIRECTION_RIGHT:
								dir = ANM_MOVE_RIGHT;
								break;

							case DIRECTION_UP:
								dir = ANM_MOVE_UP;
								break;

							case DIRECTION_DOWN:
								dir = ANM_MOVE_DOWN;
								break;
							}
							break;

						case ACTION_WAIT:
						case ACTION_SIGNAL:
						case ACTION_WAIT_SIGNAL:
						case ACTION_CONTROL:
							dir = ANM_MOVE_DOWN;
							break;
						default:
							break;
						}

						if (a == h->CurrentAction)
							break;
					}
				}

				if ((i < BurglarsNr) || (!Planing_Guard[i - BurglarsNr]))
					livAnimate(Planing_Name[i], dir, 0, 0);

				livSetPos(Planing_Name[i], xpos, ypos);
			}
		}
	}

	SetActivHandler(plSys, PersonsList->getNthNode(CurrentPerson)->_nr);

	if (lastAreaId && (lastAreaId != lsGetActivAreaID())) {
		lsDoneActivArea(lastAreaId);
		lsInitActivArea(lastAreaId, livGetXPos(Planing_Name[CurrentPerson]),
		                livGetYPos(Planing_Name[CurrentPerson]),
		                Planing_Name[CurrentPerson]);

		if (lsGetStartArea() == lsGetActivAreaID())
			lsShowEscapeCar();  /* Auto neu zeichnen */

		livRefreshAll();
	}
}

} // End of namespace Clue

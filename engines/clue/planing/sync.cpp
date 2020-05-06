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
				lsScrollLandScape((byte) -1);

			xpos = (uint16)(xpos - LS_STD_SCROLL_SPEED);
			break;

		case DIRECTION_RIGHT:
			if (animate & PLANING_ANIMATE_STD)
				livAnimate(Planing_Name[id], ANM_MOVE_RIGHT,
				           1 * LS_STD_SCROLL_SPEED, 0);

			if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
				lsScrollLandScape((byte) -1);

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
				lsScrollLandScape((byte) -1);

			xpos = (uint16)(xpos + LS_STD_SCROLL_SPEED);
			break;

		case DIRECTION_RIGHT:
			if (animate & PLANING_ANIMATE_STD)
				livAnimate(Planing_Name[id], ANM_MOVE_RIGHT,
				           -1 * LS_STD_SCROLL_SPEED, 0);

			if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
				lsScrollLandScape((byte) -1);

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
				lsScrollLandScape((byte) -1);

			ypos = (uint16)(ypos - LS_STD_SCROLL_SPEED);
			break;

		case DIRECTION_DOWN:
			if (animate & PLANING_ANIMATE_STD)
				livAnimate(Planing_Name[id], ANM_MOVE_DOWN, 0,
				           1 * LS_STD_SCROLL_SPEED);

			if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
				lsScrollLandScape((byte) -1);

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
				lsScrollLandScape((byte) -1);

			ypos = (uint16)(ypos + LS_STD_SCROLL_SPEED);
			break;

		case DIRECTION_DOWN:
			if (animate & PLANING_ANIMATE_STD)
				livAnimate(Planing_Name[id], ANM_MOVE_DOWN, 0,
				           -1 * LS_STD_SCROLL_SPEED);

			if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
				lsScrollLandScape((byte) -1);

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
					case ACTION_GO: {
						ActionGoNode *curAct = (ActionGoNode *)action;
						xpos = plXMoveSync(i, xpos, animate, direction, curAct->Direction);
						ypos = plYMoveSync(i, ypos, animate, direction, curAct->Direction);
						}
						break;

					case ACTION_USE: {
						ActionUseNode *curAct = (ActionUseNode *)action;
						if (ActionStarted(plSys)) {
							if (plIsStair(curAct->ItemId)) {
								if (direction)
									livLivesInArea(Planing_Name[i],
										StairConnectsGet(curAct->ItemId, curAct->ItemId));
								else
									livLivesInArea(Planing_Name[i], curAct->ToolId);
							} else if (dbIsObject(curAct->ItemId, Object_Police)) {
								PoliceNode *pol = (PoliceNode *)dbGetObject(curAct->ItemId);

								if (direction)
									Planing_Guard[pol->LivingID - BurglarsNr] = 1;
								else
									Planing_Guard[pol->LivingID - BurglarsNr] = 0;
							} else {
								if (direction)
									lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 1);
								else
									lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 0);
							}
						}

						if (ActionEnded(plSys)) {
							if (plIsStair(curAct->ItemId)) {
								if (i == CurrentPerson) {
									if (direction)
										lastAreaId = StairConnectsGet(curAct->ItemId, curAct->ItemId);
									else
										lastAreaId = curAct->ToolId;
								}
							}
							else if (dbIsObject(curAct->ItemId, Object_Police)) {
								PoliceNode *pol = (PoliceNode *)dbGetObject(curAct->ItemId);

								if (direction)
									Planing_Guard[pol->LivingID - BurglarsNr] = 2;
								else
									Planing_Guard[pol->LivingID - BurglarsNr] = 1;
							} else {
								if (direction) {
									lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 0);

									if (!plIgnoreLock(curAct->ItemId)) {
										if (!CHECK_STATE(lsGetObjectState(curAct->ItemId), Const_tcLOCK_UNLOCK_BIT)) {
											lsSetObjectState(curAct->ItemId, Const_tcLOCK_UNLOCK_BIT, 1);

											if (((ToolNode *)dbGetObject(curAct->ToolId))->Effect & Const_tcTOOL_OPENS) {
												lsSetObjectState(curAct->ItemId, Const_tcOPEN_CLOSE_BIT, 1);
												plCorrectOpened((LSObjectNode *)dbGetObject(curAct->ItemId), true);
											}
										} else {
											if (((LSObjectNode *)dbGetObject(curAct->ItemId))->Type == Item_Window) {
												lsWalkThroughWindow(
													(LSObjectNode *)dbGetObject(curAct->ItemId),
													xpos, ypos,
													&xpos, &ypos);
												livSetPos(Planing_Name[i], xpos, ypos);

												livRefreshAll();
											}
										}
									}
								} else {
									lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 1);

									if (!plIgnoreLock(curAct->ItemId)) {
										if (CHECK_STATE(lsGetObjectState(curAct->ItemId), Const_tcLOCK_UNLOCK_BIT)) {
											if (((LSObjectNode *)dbGetObject(curAct->ItemId))->Type == Item_Window && !curAct->ToolId) {
												lsWalkThroughWindow(
													(LSObjectNode *)dbGetObject(curAct->ItemId),
													xpos, ypos,
													&xpos, &ypos);
												livSetPos(Planing_Name[i], xpos, ypos);

												livRefreshAll();
											} else {
												lsSetObjectState(curAct->ItemId, Const_tcLOCK_UNLOCK_BIT, 0);

												if (((ToolNode *)dbGetObject(curAct->ToolId))->Effect & Const_tcTOOL_OPENS) {
													lsSetObjectState(curAct->ItemId, Const_tcOPEN_CLOSE_BIT, 0);
													plCorrectOpened((LSObjectNode *)dbGetObject(curAct->ItemId), false);
												}
											}
										}
									}
								}

								if (plIgnoreLock(curAct->ItemId)) {
									uint32 state = lsGetObjectState(curAct->ItemId);

									if (CHECK_STATE(state, Const_tcON_OFF)) {
										lsSetObjectState(curAct->ItemId, Const_tcON_OFF, 0);    /* on setzen  */

										if (plIgnoreLock(curAct->ItemId) == PLANING_POWER) {
											lsSetSpotStatus(curAct->ItemId, LS_SPOT_ON);
											lsShowAllSpots(CurrentTimer(plSys), LS_ALL_VISIBLE_SPOTS);
										}
									} else {
										lsSetObjectState(curAct->ItemId, Const_tcON_OFF, 1);    /* off setzen */

										if (plIgnoreLock(curAct->ItemId) == PLANING_POWER) {
											lsSetSpotStatus(curAct->ItemId, LS_SPOT_OFF);
											lsShowAllSpots(CurrentTimer(plSys), LS_ALL_INVISIBLE_SPOTS);
										}
									}
								}

								plRefresh(curAct->ItemId);
							}
						}
						}
						break;

					case ACTION_TAKE: {
						ActionTakeNode *curAct = (ActionTakeNode*)action;
						if (ActionStarted(plSys)) {
							if (direction)
								lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 1);
							else
								lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 0);
						}

						if (ActionEnded(plSys)) {
							if (direction) {
								uint32 weightLoot = ((LootNode*)dbGetObject(curAct->LootId))->Weight;
								uint32 volumeLoot = ((LootNode*)dbGetObject(curAct->LootId))->Volume;

								lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 0);

								if (curAct->ItemId >= 9701 && curAct->ItemId <= 9708) {
									lsRemLootBag(curAct->ItemId);
									Planing_Loot[curAct->ItemId - 9701] = 0;
								} else if (CHECK_STATE(lsGetObjectState(curAct->ItemId), Const_tcTAKE_BIT)) {
									lsTurnObject((LSObjectNode*)dbGetObject(curAct->ItemId), LS_OBJECT_INVISIBLE, LS_NO_COLLISION);
									lsSetObjectState(curAct->ItemId, Const_tcACCESS_BIT, 0);
								}

								uint32 newValue = GetP(dbGetObject(curAct->ItemId), hasLoot(i), dbGetObject(curAct->LootId));

								if (Ask(dbGetObject(BurglarsList->getNthNode(i)->_nr), take_RelId, dbGetObject(curAct->LootId))) {
									uint32 oldValue = GetP(dbGetObject(BurglarsList->getNthNode(i)->_nr), take_RelId, dbGetObject(curAct->LootId));
									SetP(dbGetObject(BurglarsList->getNthNode(i)->_nr), take_RelId, dbGetObject(curAct-> LootId), oldValue + newValue);
								} else
									SetP(dbGetObject(BurglarsList->getNthNode(i)->_nr), take_RelId, dbGetObject(curAct->LootId), newValue);

								UnSet(dbGetObject(curAct->ItemId), hasLoot(i), dbGetObject(curAct->LootId));

								Planing_Weight[i] += weightLoot;
								Planing_Volume[i] += volumeLoot;
							} else {
								uint32 weightLoot = ((LootNode*)dbGetObject(curAct->LootId))->Weight;
								uint32 volumeLoot = ((LootNode*)dbGetObject(curAct->LootId))->Volume;

								lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 1);

								if (curAct->ItemId >= 9701 && curAct->ItemId <= 9708) {
									lsAddLootBag(xpos, ypos, curAct->ItemId - 9700);
									Planing_Loot[curAct->ItemId - 9701] = 1;
									SetP(dbGetObject(curAct->ItemId), hasLoot(i), dbGetObject(curAct->LootId),
										GetP(dbGetObject(PersonsList->getNthNode(i)->_nr), take_RelId, dbGetObject(curAct->LootId)));
								} else {
									if (CHECK_STATE(lsGetObjectState(curAct->ItemId), Const_tcTAKE_BIT)) {
										lsTurnObject((LSObjectNode*)dbGetObject(curAct->ItemId), LS_OBJECT_VISIBLE, LS_COLLISION);
										lsSetObjectState(curAct->ItemId, Const_tcACCESS_BIT, 1);
									}

									SetP(dbGetObject(curAct->ItemId), hasLoot(i), dbGetObject(curAct->LootId),
										GetP(dbGetObject(curAct->ItemId), hasLoot_Clone_RelId, dbGetObject(curAct->LootId)));
								}

								UnSet(dbGetObject(PersonsList->getNthNode(i)->_nr), take_RelId, dbGetObject(curAct->LootId));
								Planing_Weight[i] -= weightLoot;
								Planing_Volume[i] -= volumeLoot;
							}

							plRefresh(curAct->ItemId);
						}
						}
						break;

					case ACTION_DROP: {
						ActionDropNode* curAct = (ActionDropNode *)action;

						if (ActionStarted(plSys)) {
							if (direction)
								lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 1);
							else
								lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 0);
						}

						if (ActionEnded(plSys)) {
							if (direction) {
								uint32 weightLoot = ((LootNode *)dbGetObject(curAct->LootId))->Weight;
								uint32 volumeLoot = ((LootNode *)dbGetObject(curAct->LootId))->Volume;

								lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 1);

								if (curAct->ItemId >= 9701 && curAct->ItemId <= 9708) {
									lsAddLootBag(xpos, ypos, curAct->ItemId - 9700);
									Planing_Loot[curAct->ItemId - 9701] = 1;
									SetP(dbGetObject(curAct->ItemId), hasLoot(i), dbGetObject(curAct->LootId),
										GetP(dbGetObject(PersonsList->getNthNode(i)->_nr), take_RelId, dbGetObject(curAct->LootId)));
								} else {
									if (CHECK_STATE(lsGetObjectState(curAct->ItemId), Const_tcTAKE_BIT)) {
										lsTurnObject((LSObjectNode *)dbGetObject(curAct->ItemId), LS_OBJECT_VISIBLE, LS_COLLISION);
										lsSetObjectState(curAct->ItemId, Const_tcACCESS_BIT, 1);
									}

									SetP(dbGetObject(curAct->ItemId), hasLoot(i), dbGetObject(curAct->LootId),
										GetP(dbGetObject(curAct->ItemId), hasLoot_Clone_RelId, dbGetObject(curAct->LootId)));
								}

								UnSet(dbGetObject(PersonsList->getNthNode(i)->_nr), take_RelId, dbGetObject(curAct->LootId));
								Planing_Weight[i] -= weightLoot;
								Planing_Volume[i] -= volumeLoot;
							} else {
								uint32 weightLoot = ((LootNode *)dbGetObject(curAct->LootId))->Weight;
								uint32 volumeLoot = ((LootNode *)dbGetObject(curAct->LootId))->Volume;

								lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 0);

								if (curAct->ItemId >= 9701 && curAct->ItemId <= 9708) {
									lsRemLootBag(curAct->ItemId);
									Planing_Loot[curAct->ItemId - 9701] = 0;
								} else if (CHECK_STATE(lsGetObjectState(curAct->ItemId), Const_tcTAKE_BIT)) {
									lsTurnObject((LSObjectNode *)dbGetObject(curAct->ItemId), LS_OBJECT_INVISIBLE, LS_NO_COLLISION);
									lsSetObjectState(curAct->ItemId, Const_tcACCESS_BIT, 0);
								}

								uint32 newValue = GetP(dbGetObject(curAct->ItemId), hasLoot(i), dbGetObject(curAct->LootId));

								if (Ask(dbGetObject(BurglarsList->getNthNode(i)->_nr), take_RelId, dbGetObject(curAct->LootId))) {
									uint32 oldValue = GetP(dbGetObject(BurglarsList->getNthNode(i)->_nr), take_RelId, dbGetObject(curAct->LootId));
									SetP(dbGetObject(BurglarsList->getNthNode(i)->_nr), take_RelId, dbGetObject(curAct->LootId), oldValue + newValue);
								} else
									SetP(dbGetObject(BurglarsList->getNthNode(i)->_nr), take_RelId, dbGetObject(curAct->LootId), newValue);

								UnSet(dbGetObject(curAct->ItemId), hasLoot(i), dbGetObject(curAct->LootId));

								Planing_Weight[i] += weightLoot;
								Planing_Volume[i] += volumeLoot;
							}

							plRefresh(curAct->ItemId);
						}
						}
						break;

					case ACTION_OPEN: {
						ActionOpenNode* curAct = (ActionOpenNode*)action;

						if (ActionStarted(plSys)) {
							if (i < BurglarsNr) {
								if (direction)
									lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 1);
								else
									lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 0);
							}
						}

						if (ActionEnded(plSys)) {
							if (direction) {
								if (i < BurglarsNr)
									lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 0);

								lsSetObjectState(curAct->ItemId, Const_tcOPEN_CLOSE_BIT, 1);
								plCorrectOpened((LSObjectNode*)dbGetObject(curAct->ItemId), true);
							} else {
								if (i < BurglarsNr)
									lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 1);

								lsSetObjectState(curAct->ItemId, Const_tcOPEN_CLOSE_BIT, 0);
								plCorrectOpened((LSObjectNode*)dbGetObject(curAct->ItemId), false);
							}

							plRefresh(curAct->ItemId);
						}
						}
						break;

					case ACTION_CLOSE: {
						ActionOpenNode* curAct = (ActionOpenNode*)action;

						if (ActionStarted(plSys)) {
							if (i < BurglarsNr) {
								if (direction)
									lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 1);
								else
									lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 0);
							}
						}

						if (ActionEnded(plSys)) {
							if (direction) {
								if (i < BurglarsNr)
									lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 0);

								lsSetObjectState(curAct->ItemId, Const_tcOPEN_CLOSE_BIT, 0);
								plCorrectOpened((LSObjectNode*)dbGetObject(curAct->ItemId), false);
							} else {
								if (i < BurglarsNr)
									lsSetObjectState(curAct->ItemId, Const_tcIN_PROGRESS_BIT, 1);

								lsSetObjectState(curAct->ItemId, Const_tcOPEN_CLOSE_BIT, 1);
								plCorrectOpened((LSObjectNode*)dbGetObject(curAct->ItemId), true);
							}

							plRefresh(curAct->ItemId);
						}
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
		                livGetYPos(Planing_Name[CurrentPerson]), Planing_Name[CurrentPerson]);

		if (lsGetStartArea() == lsGetActivAreaID())
			lsShowEscapeCar();  /* Auto neu zeichnen */

		livRefreshAll();
	}
}

} // End of namespace Clue

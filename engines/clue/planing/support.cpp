/*
**      $Filename: planing/support.c
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.support for "Der Clou!"
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
#include "clue/data/dataappl.h"
#include "clue/planing/player.h"

namespace Clue {

/* loot support */
uint32 plGetNextLoot() {
	for (int i = 0; i < PLANING_NR_LOOTS; i++) {
		if (!Planing_Loot[i]) {
			Planing_Loot[i] = 1;
			return lsAddLootBag(livGetXPos(Planing_Name[CurrentPerson]), livGetYPos(Planing_Name[CurrentPerson]) + 5, i + 1);
		}
	}

	return 0;
}

/* check support */
bool plLivingsPosAtCar(uint32 bldId) {
	BuildingNode *bldObj = (BuildingNode *) dbGetObject(bldId);

	uint16 carxpos = bldObj->CarXPos + 8;
	uint16 carypos = bldObj->CarYPos + 8;

	startsWithAll(bldId, OLF_NORMAL, Object_LSArea);

	bool atCar = true;
	for (byte i = 0; i < BurglarsNr; i++) {
		uint16 xpos = livGetXPos(Planing_Name[i]);
		uint16 ypos = livGetYPos(Planing_Name[i]);

		if (livWhereIs(Planing_Name[i]) != ObjectList->getListHead()->_nr) {
			atCar = false;
			break;
		}

		if (((xpos < (carxpos - PLANING_AREA_CAR))
		        || (xpos > (carxpos + PLANING_AREA_CAR)))
		        && ((ypos < (carypos - PLANING_AREA_CAR))
		            || (ypos > (carypos + PLANING_AREA_CAR)))) {
			atCar = false;
			break;
		}
	}

	return atCar;
}

bool plAllInCar(uint32 bldId) {
	bool ret = true;
	uint32 realCurrentPerson = CurrentPerson;
	uint32 oldTimer = CurrentTimer(plSys);

	byte maxPerson = 0;
	uint32 maxTimer = 0;
	for (byte i = 0; i < BurglarsNr; i++) {
		SetActivHandler(plSys, PersonsList->getNthNode(i)->_nr);
		if (maxTimer < GetMaxTimer(plSys)) {
			maxTimer = GetMaxTimer(plSys);
			maxPerson = i;
		}
	}

	SetActivHandler(plSys, PersonsList->getNthNode(maxPerson)->_nr);

	if (maxPerson != realCurrentPerson) {
		SetActivHandler(plSys, PersonsList->getNthNode(maxPerson)->_nr);
		plMessage("PERSON_NOTES", PLANING_MSG_REFRESH);
		plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
		       GetMaxTimer(plSys) - oldTimer, 1);
	}

	if (!plLivingsPosAtCar(bldId))
		ret = false;

	CurrentPerson = realCurrentPerson;

	if (maxPerson != realCurrentPerson) {
		SetActivHandler(plSys, PersonsList->getNthNode(CurrentPerson)->_nr);
		plMessage("PERSON_NOTES", PLANING_MSG_REFRESH);
		plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys), maxTimer - GetMaxTimer(plSys), 0);
	}

	return ret;
}

byte plIsStair(uint32 objId) {
	return (byte)((((LSObjectNode *) dbGetObject(objId))->Type == Item_Stairway));
}

/* special items support */
void plCorrectOpened(LSObjectNode *obj, bool open) {
	switch (obj->Type) {
	case Item_Wooden_door:
	case Item_Steel_door:
	case Item_Vault:
	case Item_Wall_gate:
		if (open)
			lsTurnObject(obj, LS_OBJECT_INVISIBLE, LS_NO_COLLISION);
		else
			lsTurnObject(obj, LS_OBJECT_VISIBLE, LS_COLLISION);

		livRefreshAll();
		break;
	}
}

byte plIgnoreLock(uint32 objId) {
	byte back = 0;

	switch (((LSObjectNode *) dbGetObject(objId))->Type) {
	case Item_Alarm_system_Z3:
		back = PLANING_ALARM_Z3;
		break;

	case Item_Alarm_system_X3:
		back = PLANING_ALARM_X5;
		break;

	case Item_Alarm_system_Top:
		back = PLANING_ALARM_TOP3;
		break;

	case Item_Control_Box:
		back = PLANING_POWER;
		break;
	}

	return back;
}

/* livings support */
void plMove(uint32 current, byte direction) {
	switch (direction) {
	case DIRECTION_LEFT:
		livAnimate(Planing_Name[current], ANM_MOVE_LEFT,
		           -1 * LS_STD_SCROLL_SPEED, 0);
		break;

	case DIRECTION_RIGHT:
		livAnimate(Planing_Name[current], ANM_MOVE_RIGHT,
		           1 * LS_STD_SCROLL_SPEED, 0);
		break;

	case DIRECTION_UP:
		livAnimate(Planing_Name[current], ANM_MOVE_UP, 0,
		           -1 * LS_STD_SCROLL_SPEED);
		break;

	case DIRECTION_DOWN:
		livAnimate(Planing_Name[current], ANM_MOVE_DOWN, 0,
		           1 * LS_STD_SCROLL_SPEED);
		break;
	}
}

void plWork(uint32 current) {
	switch (livGetViewDirection(Planing_Name[current])) {
	case ANM_MOVE_LEFT:
		livAnimate(Planing_Name[current], ANM_WORK_LEFT, 0, 0);
		break;

	case ANM_MOVE_RIGHT:
		livAnimate(Planing_Name[current], ANM_WORK_RIGHT, 0, 0);
		break;

	case ANM_MOVE_UP:
		livAnimate(Planing_Name[current], ANM_WORK_UP, 0, 0);
		break;

	case ANM_MOVE_DOWN:
		livAnimate(Planing_Name[current], ANM_WORK_DOWN, 0, 0);
		break;
	}
}

NewObjectList<dbObjectNode> *plGetObjectsList(uint32 current, bool addLootBags) {
	NewObjectList<dbObjectNode> *list = nullptr;

	uint32 areaId = livWhereIs(Planing_Name[current]);
	uint32 oldAreaId = lsGetCurrObjectRetrieval();
	lsSetObjectRetrievalList(areaId);

	switch (livGetViewDirection(Planing_Name[current])) {
	case ANM_MOVE_LEFT:
		list = lsGetObjectsByList(livGetXPos(Planing_Name[current]) - 9,
		                          livGetYPos(Planing_Name[current]) + 2,
		                          14, 12, 1, addLootBags);
		break;

	case ANM_MOVE_RIGHT:
		list = lsGetObjectsByList(livGetXPos(Planing_Name[current]) + 9,
		                          livGetYPos(Planing_Name[current]) + 2,
		                          14, 12, 1, addLootBags);
		break;

	case ANM_MOVE_UP:
		list = lsGetObjectsByList(livGetXPos(Planing_Name[current]) + 2,
		                          livGetYPos(Planing_Name[current]) - 9,
		                          12, 14, 1, addLootBags);
		break;

	case ANM_MOVE_DOWN:
		list = lsGetObjectsByList(livGetXPos(Planing_Name[current]) + 2,
		                          livGetYPos(Planing_Name[current]) + 9,
		                          12, 14, 1, addLootBags);
		break;
	}

	lsSetObjectRetrievalList(oldAreaId);
	return list;
}

void plInsertGuard(NewObjectList<dbObjectNode> *list, uint32 current, uint32 guard) {
	switch (livGetViewDirection(Planing_Name[current])) {
	case ANM_MOVE_LEFT:
		tcInsertGuard(list, Planing_GuardRoomList[guard - BurglarsNr],
		              livGetXPos(Planing_Name[current]) - 9,
		              livGetYPos(Planing_Name[current]),
		              14, 14,
		              PersonsList->getNthNode(guard)->_nr,
		              guard + 1 + (PLANING_NR_PERSONS - BurglarsNr),
		              livWhereIs(Planing_Name[current]));
		break;

	case ANM_MOVE_RIGHT:
		tcInsertGuard(list, Planing_GuardRoomList[guard - BurglarsNr],
		              livGetXPos(Planing_Name[current]) + 9,
		              livGetYPos(Planing_Name[current]),
		              14, 14,
		              PersonsList->getNthNode(guard)->_nr,
		              guard + 1 + (PLANING_NR_PERSONS - BurglarsNr),
		              livWhereIs(Planing_Name[current]));
		break;

	case ANM_MOVE_UP:
		tcInsertGuard(list, Planing_GuardRoomList[guard - BurglarsNr],
		              livGetXPos(Planing_Name[current]),
		              livGetYPos(Planing_Name[current]) - 9,
		              14, 14,
		              PersonsList->getNthNode(guard)->_nr,
		              guard + 1 + (PLANING_NR_PERSONS - BurglarsNr),
		              livWhereIs(Planing_Name[current]));
		break;

	case ANM_MOVE_DOWN:
		tcInsertGuard(list, Planing_GuardRoomList[guard - BurglarsNr],
		              livGetXPos(Planing_Name[current]),
		              livGetYPos(Planing_Name[current]) + 9,
		              14, 14,
		              PersonsList->getNthNode(guard)->_nr,
		              guard + 1 + (PLANING_NR_PERSONS - BurglarsNr),
		              livWhereIs(Planing_Name[current]));
		break;
	}
}

bool plObjectInReach(uint32 current, uint32 objId) {
	NewObjectList<dbObjectNode> *actionList = plGetObjectsList(current, true);
	bool ret = false;

	for (byte i = BurglarsNr; i < PersonsNr; i++)
		plInsertGuard(actionList, current, i);

	if (!actionList->isEmpty()) {
		if (dbHasObjectNode(actionList, objId))
			ret = true;
	}

	actionList->removeList();
	return ret;
}

} // End of namespace Clue

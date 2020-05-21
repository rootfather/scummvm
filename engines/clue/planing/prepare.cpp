/*
**      $Filename: planing/prepare.c
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.prepare for "Der Clou!"
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
#include "clue/scenes/scenes.h"

namespace Clue {

NewObjectList<dbObjectNode> *PersonsList = nullptr;
NewObjectList<dbObjectNode> *BurglarsList = nullptr;

byte PersonsNr = 0;
byte BurglarsNr = 0;
uint32 CurrentPerson = 0;

uint32 Planing_Weight[PLANING_NR_PERSONS];
uint32 Planing_Volume[PLANING_NR_PERSONS];

byte Planing_Loot[PLANING_NR_LOOTS];
byte Planing_Guard[PLANING_NR_GUARDS];

char Planing_Name[PLANING_NR_PERSONS + PLANING_NR_GUARDS][20];

NewObjectList<dbObjectNode> *Planing_GuardRoomList[PLANING_NR_GUARDS];

uint32 Planing_BldId;


/* Handler functions */
void plBuildHandler(dbObjectNode *n) {
	uint32 flags = SHF_NORMAL;

	if (n->_type == Object_Police)
		flags |= SHF_AUTOREVERS;

	InitHandler(plSys, n->_nr, flags);
}

void plClearHandler(dbObjectNode *n) {
	ClearHandler(plSys, n->_nr);
}

void plCloseHandler(dbObjectNode *n) {
	CloseHandler(plSys, n->_nr);
}

/* Preparation & Unpreparation functions */
void plPrepareData() {
	for (uint i = 0; i < PLANING_NR_LOOTS; i++)
		Planing_Loot[i] = 0;

	for (uint i = 0; i < PLANING_NR_PERSONS; i++)
		Planing_Weight[i] = 0;

	for (uint i = 0; i < PLANING_NR_PERSONS; i++)
		Planing_Volume[i] = 0;

	for (uint i = 0; i < PLANING_NR_GUARDS; i++)
		Planing_Guard[i] = 0;
}

void plPrepareSprite(uint32 livNr, uint32 areaId) {
	LSAreaNode *activArea = (LSAreaNode *) dbGetObject(areaId);
	uint16 xpos, ypos;

	livLivesInArea(Planing_Name[livNr], areaId);

	if (livNr < BurglarsNr) {
		switch (livNr) {
		case 0:
			xpos = activArea->us_StartX0;
			ypos = activArea->us_StartY0;
			break;

		case 1:
			xpos = activArea->us_StartX1;
			ypos = activArea->us_StartY1;
			break;

		case 2:
			xpos = activArea->us_StartX2;
			ypos = activArea->us_StartY2;
			break;

		case 3:
			xpos = activArea->us_StartX3;
			ypos = activArea->us_StartY3;
			break;

		default:
			return;
		}
	} else {
		switch (livNr + (PLANING_NR_PERSONS - BurglarsNr)) {
		case 4:
		case 6:
			xpos = activArea->us_StartX4 - 4;
			ypos = activArea->us_StartY4 + 11;
			break;

		case 5:
		case 7:
			xpos = activArea->us_StartX5 - 4;
			ypos = activArea->us_StartY5 + 11;
			break;

		default:
			return;
		}
	}

	livSetPos(Planing_Name[livNr], xpos + 9, ypos - 18);

	if (livNr < BurglarsNr)
		livAnimate(Planing_Name[livNr], ANM_STAND, 0, 0);
	else {
		livAnimate(Planing_Name[livNr], ANM_MOVE_DOWN, 0, 0);
		Planing_GuardRoomList[livNr - BurglarsNr] = lsGetRoomsOfArea(areaId);
	}
}

void plPrepareGfx(uint32 objId, byte landscapMode, byte prepareMode) {
	if (prepareMode & PLANING_GFX_LANDSCAPE)
		lsInitLandScape(objId, landscapMode);

	if (prepareMode & PLANING_GFX_BACKGROUND) {
		CurrentBackground = BGD_PLANUNG;
		ShowMenuBackground();

		/* to blend the menu colours */
		gfxShow(CurrentBackground, GFX_NO_REFRESH | GFX_BLEND_UP, 5, -1, -1);
	}

	if (prepareMode & PLANING_GFX_SPRITES) {
		for (uint32 i = 0; i < PersonsNr; i++) {
			if (dbIsObject(PersonsList->getNthNode(i)->_nr, Object_Person))
				plPrepareSprite(i, lsGetActivAreaID());
			else {
				((PoliceNode *) dbGetObject(PersonsList->getNthNode(i)->_nr))->LivingID = i;
				plPrepareSprite(i, isGuardedbyGet(objId, PersonsList->getNthNode(i)->_nr));
			}
		}

		lsSetActivLiving(Planing_Name[CurrentPerson], (uint16) -1, (uint16) -1);
	}
}

void plUnprepareGfx() {
	gfxShow(CurrentBackground, GFX_NO_REFRESH | GFX_FADE_OUT, 5, -1, -1);
	_upperGc->gfxClearArea();

	for (int i = BurglarsNr; i < PersonsNr; i++)
		Planing_GuardRoomList[i - BurglarsNr]->removeList();

	lsDoneLandScape();
	gfxShow(CurrentBackground, GFX_NO_REFRESH | GFX_BLEND_UP, 0, -1, -1);
}

void plPrepareRel() {
	consistsOfAll(Planing_BldId, OLF_PRIVATE_LIST, Object_LSArea);
	NewObjectList<dbObjectNode> *areas = ObjectListPrivate;

	for (dbObjectNode *n = areas->getListHead(); n->_succ; n = (dbObjectNode *)n->_succ) {
		LSAreaNode *area = (LSAreaNode *)n;

		if (!CloneRelation(area->ul_ObjectBaseNr + REL_HAS_LOOT_OFFSET, hasLoot_Clone_RelId))
			ErrorMsg(No_Mem, ERROR_MODULE_PLANING, 0);
	}

	areas->removeList();

	if (!AddRelation(take_RelId))
		ErrorMsg(No_Mem, ERROR_MODULE_PLANING, 0);
}

void plUnprepareRel() {
	RemRelation(take_RelId);
	RemRelation(hasLoot_Clone_RelId);
}

void plPrepareNames() {
	for (int i = 0; i < PersonsNr; i++) {
		if (dbIsObject(PersonsList->getNthNode(i)->_nr, Object_Person))
			sprintf(Planing_Name[i], "Person_%d", i + 1);
		else
			sprintf(Planing_Name[i], "Police_%d", i + 1 + (PLANING_NR_PERSONS - BurglarsNr));
	}
}

void plPrepareSys(uint32 currPer, uint32 objId, byte sysMode) {
	CurrentPerson = currPer;

	if (objId)
		Planing_BldId = objId;

	if (sysMode & PLANING_INIT_PERSONSLIST) {
		delete PersonsList;
		delete BurglarsList;

		joined_byAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Person);
		PersonsList = ObjectListPrivate;

		joined_byAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Person);
		BurglarsList = ObjectListPrivate;

		dbSortObjectList(&PersonsList, dbStdCompareObjects);
		dbSortObjectList(&BurglarsList, dbStdCompareObjects);

		PersonsNr = PersonsList->getNrOfNodes();
		BurglarsNr = BurglarsList->getNrOfNodes();

		plPrepareNames();
	}

	if (sysMode & PLANING_HANDLER_ADD) {
		if (grdAddToList(objId, PersonsList)) {
			dbSortObjectList(&PersonsList, dbStdCompareObjects);

			PersonsNr = PersonsList->getNrOfNodes();

			plPrepareNames();
		}
	}

	if (sysMode & PLANING_HANDLER_CLEAR) {
		for (dbObjectNode *node = BurglarsList->getListHead(); node->_succ; node = (dbObjectNode *)node->_succ)
			plClearHandler(node);
	}

	if (sysMode & PLANING_HANDLER_CLOSE) {
		for (dbObjectNode *node = PersonsList->getListHead(); node->_succ; node = (dbObjectNode *)node->_succ)
			plCloseHandler(node);
	}
	
	if (sysMode & PLANING_HANDLER_OPEN) {
		for (dbObjectNode *node = PersonsList->getListHead(); node->_succ; node = (dbObjectNode *)node->_succ)
			plBuildHandler(node);
	}

	if ((sysMode & PLANING_GUARDS_LOAD) && PersonsNr > BurglarsNr && !(_gamePlayMode & GP_MODE_LEVEL_DESIGN)) {
		Common::Stream *fh = nullptr;

		startsWithAll(objId, OLF_NORMAL, Object_LSArea);

		if (grdInit(&fh, 0, Planing_BldId, ObjectList->getNthNode(0)->_nr))
			grdDo(fh, plSys, PersonsList, BurglarsNr, PersonsNr, GUARDS_DO_LOAD);

		grdDone(fh);
	}

	if (sysMode & PLANING_HANDLER_SET)
		SetActivHandler(plSys, PersonsList->getNthNode(CurrentPerson)->_nr);
}

void plUnprepareSys() {
	plPrepareSys(0, 0, PLANING_HANDLER_CLOSE);

	if (PersonsList)
		PersonsList->removeList();

	if (BurglarsList)
		BurglarsList->removeList();

	PersonsList = nullptr;
	BurglarsList = nullptr;
}

} // End of namespace Clue

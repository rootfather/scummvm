/*
**  $Filename: landscap/init.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-02-94
**
**  landscap init functions for "Der Clou!"
**
**   (c) 1994 ...and avoid panic by, H. Gaberschek
**      All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "clue/landscap/landscap.h"
#include "clue/landscap/landscap_p.h"
#include "clue/living/bob.h"

namespace Clue {

static void lsInitFloorSquares();
static void lsLoadAllSpots();
static void lsSetCurrFloorSquares(uint32 areaId);

/*------------------------------------------------------------------------------
 *   global functions for landscape
 *------------------------------------------------------------------------------*/

void lsInitLandScape(uint32 bID, byte mode) {
	/* initialisiert das Landschaftsmodul */
	if (!ls)
		ls = (LandScape *)TCAllocMem(sizeof(*ls), 0);

	ls->ul_BuildingID = bID;

	ls->us_RasInfoScrollX = 0;
	ls->us_RasInfoScrollY = 0;

	lsInitGfx();        /* dont change location of this line */

	ls->uch_ScrollSpeed = LS_STD_SCROLL_SPEED;
	ls->uch_ShowObjectMask = 0;

	lsSetCollMode(mode);

	lsInitObjects();

	lsInitSpots();

	lsLoadAllSpots();

	ls->uch_FloorsPerWindowColumn = (byte)(LS_FLOORS_PER_COLUMN);
	ls->uch_FloorsPerWindowLine = (byte)(LS_FLOORS_PER_LINE);

	ls->ul_AreaID = lsGetStartArea();   /* !! MOD 04-01 - vor Sprites!!! */

	BobInitLists();

	ls->us_EscapeCarBobId = BobInit(14, 14);

	/* lootbags must be initialized prior to Livings because they
	   have a lower priority (appear below maxis) */
	for (int32 i = 9701; i <= 9708; i++) {
		LSObjectNode *lso = (LSObjectNode *)dbGetObject(i);

		/* OffsetFact on the PC is not used as offset in the plane,
		       but as handle for the bob */
		lso->us_OffsetFact = BobInit(14, 14);

		hasLootBagUnSet(ls->ul_AreaID, (uint32) i);
	}

	livInit(0, 0, LS_VISIBLE_X_SIZE, LS_VISIBLE_Y_SIZE, LS_MAX_AREA_WIDTH,
	        LS_MAX_AREA_HEIGHT, 8, ls->ul_AreaID);

	ls->p_CurrFloor = NULL;

	ls->us_DoorXOffset = 0;
	ls->us_DoorYOffset = 32;    /* above are 16er objects */

	ls->p_DoorRefreshList = new NewList<LSDoorRefreshNode>;

	lsInitFloorSquares();

	lsInitActivArea(ls->ul_AreaID, (uint16) -1, (uint16) -1, nullptr);
	lsShowEscapeCar();
}

void lsInitActivArea(uint32 areaID, uint16 x, uint16 y, char *livingName) {
	LSAreaNode *area = (LSAreaNode *) dbGetObject(areaID);

	ls->ul_AreaID = areaID;

	lsSetRelations(areaID);
	lsSetObjectRetrievalList(areaID);

	ls->us_WindowXSize = area->us_Width;
	ls->us_WindowYSize = area->us_Height;

	if (x == (uint16) -1)
		x = area->us_StartX0;
	if (y == (uint16) -1)
		y = area->us_StartY0;

	lsSetVisibleWindow(x, y);

	livSetActivAreaId(areaID);

	lsSetCurrFloorSquares(areaID);

	gfxCollToMem(area->us_Coll16ID, LS_COLL16_MEM_RP);
	gfxCollToMem(area->us_Coll32ID, LS_COLL32_MEM_RP);
	gfxCollToMem(area->us_Coll48ID, LS_COLL48_MEM_RP);

	gfxCollToMem(area->us_FloorCollID, LS_FLOOR_MEM_RP);

	lsBuildScrollWindow();
	lsRefreshAllLootBags();
}

void lsInitRelations(uint32 areaID) {
	LSAreaNode *area = (LSAreaNode *)dbGetObject(areaID);

	AddRelation(area->ul_ObjectBaseNr + REL_CONSIST_OFFSET);
	AddRelation(area->ul_ObjectBaseNr + REL_HAS_LOCK_OFFSET);
	AddRelation(area->ul_ObjectBaseNr + REL_HAS_ALARM_OFFSET);
	AddRelation(area->ul_ObjectBaseNr + REL_HAS_POWER_OFFSET);
	AddRelation(area->ul_ObjectBaseNr + REL_HAS_LOOT_OFFSET);
	AddRelation(area->ul_ObjectBaseNr + REL_HAS_ROOM_OFFSET);
}

void lsSetRelations(uint32 areaID) {
	LSAreaNode *area = (LSAreaNode *)dbGetObject(areaID);

	ConsistOfRelationID = area->ul_ObjectBaseNr + REL_CONSIST_OFFSET;
	hasLockRelationID = area->ul_ObjectBaseNr + REL_HAS_LOCK_OFFSET;
	hasAlarmRelationID = area->ul_ObjectBaseNr + REL_HAS_ALARM_OFFSET;
	hasPowerRelationID = area->ul_ObjectBaseNr + REL_HAS_POWER_OFFSET;
	hasLootRelationID = area->ul_ObjectBaseNr + REL_HAS_LOOT_OFFSET;
	hasRoomRelationID = area->ul_ObjectBaseNr + REL_HAS_ROOM_OFFSET;
}

void lsInitObjects() {
	uint32 areaCount = 0;

	/* alle Relationen erzeugen */
	consistsOfAll(ls->ul_BuildingID, OLF_PRIVATE_LIST, Object_LSArea);
	NewList<dbObjectNode> *areas = ObjectListPrivate;

	/* jetzt alle Stockwerke durchgehen! */
	for (uint32 i = 0; i < 3; i++) {
		ls->ul_ObjectRetrievalAreaId[i] = 0;
		ls->p_ObjectRetrievalLists[i] = NULL;
	}

	dbObjectNode* n;
	for (n = areas->getListHead(); n->_succ; n = (dbObjectNode *)n->_succ) {
		lsInitRelations(n->_nr);

		/* Daten laden */
		lsInitObjectDB(ls->ul_BuildingID, n->_nr);

		lsSetRelations(n->_nr);
		lsRefreshObjectList(n->_nr);  /* ObjectRetrievalList erstellen */

		/* there's a lot to be patched! */
		lsPatchObjects();

		ls->p_ObjectRetrievalLists[areaCount] = ls->p_ObjectRetrieval;  /* und merken */
		ls->ul_ObjectRetrievalAreaId[areaCount] = n->_nr;

		areaCount++;
	}

	lsLoadGlobalData(ls->ul_BuildingID, ((dbObjectNode *)n->_pred)->_nr);

	areas->removeList();
}

void lsLoadGlobalData(uint32 bld, uint32 ul_AreaId) {
	char fileName[DSK_PATH_MAX];

	Common::String areaName = dbGetObjectName(ul_AreaId);
	areaName.deleteLastChar();
	areaName += OBJ_GLOBAL_REL_EXTENSION;

	dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, areaName.c_str(), fileName);

	if (!LoadRelations(fileName))
		ErrorMsg(Disk_Defect, ERROR_MODULE_LANDSCAP, 2);
}

void lsInitObjectDB(uint32 bld, uint32 areaID) {
	char fileName[DSK_PATH_MAX];
	Common::String areaName = dbGetObjectName(areaID);
	areaName += OBJ_DATA_EXTENSION;
	dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, areaName.c_str(), fileName);

	dbSetLoadObjectsMode(DB_LOAD_MODE_NO_NAME); /* dont fetch names of objects */

	if (dbLoadAllObjects(fileName)) {
		areaName = dbGetObjectName(areaID);
		areaName += OBJ_REL_EXTENSION;
		dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, areaName.c_str(), fileName);

		if (!LoadRelations(fileName))
			ErrorMsg(Disk_Defect, ERROR_MODULE_LANDSCAP, 3);
	} else
		ErrorMsg(Disk_Defect, ERROR_MODULE_LANDSCAP, 4);

	dbSetLoadObjectsMode(DB_LOAD_MODE_STD);
}

/* contrary to the Amiga version this loads all floor data into memory at once */
static void lsInitFloorSquares() {
	for (uint8 i = 0; i < 3; i++)
		ls->p_AllFloors[i] = NULL;

	uint32 count = LS_FLOORS_PER_LINE * LS_FLOORS_PER_COLUMN;

	consistsOfAll(ls->ul_BuildingID, OLF_PRIVATE_LIST, Object_LSArea);
	NewList<dbObjectNode>* areas = ObjectListPrivate;

	/* jetzt alle Stockwerke durchgehen! */
	dbObjectNode *n;
	int i;
	for (n = areas->getListHead(), i = 0; n->_succ; n = (dbObjectNode *)n->_succ, i++) {
		size_t size = sizeof(struct LSFloorSquare) * count;

		ls->p_AllFloors[i] = (LSFloorSquare *)TCAllocMem(size, 0);
		ls->ul_FloorAreaId[i] = n->_nr;

		Common::String areaName = dbGetObjectName(ls->ul_FloorAreaId[i]);
		areaName += FLOOR_DATA_EXTENSION;

		char fileName[TXT_KEY_LENGTH];
		dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, areaName.c_str(), fileName);

		Common::Stream *fh = dskOpen(fileName, 0);
		if (fh) {
			for (uint32 j = 0; j < count; j++)
				dskRead(fh, &ls->p_AllFloors[i][j].uch_FloorType, sizeof(uint8));

			dskClose(fh);
		}
	}

	areas->removeList();
}

static void lsLoadAllSpots() {
	consistsOfAll(ls->ul_BuildingID, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME, Object_LSArea);
	NewList<dbObjectNode> *areas = ObjectListPrivate;
	dbObjectNode *n = areas->getListHead();

	char fileName[TXT_KEY_LENGTH];
	strcpy(fileName, n->_name.c_str());
	fileName[strlen(fileName) - 1] = '\0';
	strcat(fileName, SPOT_DATA_EXTENSION);
	lsLoadSpots(ls->ul_BuildingID, fileName);

	areas->removeList();
}

static void lsSetCurrFloorSquares(uint32 areaId) {
	for (int i = 0; i < 3; i++) {
		if (areaId == ls->ul_FloorAreaId[i])
			ls->p_CurrFloor = ls->p_AllFloors[i];
	}
}

/*------------------------------------------------------------------------------
 *   done functions for landscape
 *------------------------------------------------------------------------------*/

/* release all floor data */
static void lsDoneFloorSquares() {
	for (int i = 0; i < 3; i++) {
		if (ls->p_AllFloors[i]) {
			int32 count = LS_FLOORS_PER_LINE * LS_FLOORS_PER_COLUMN;
			size_t size = sizeof(struct LSFloorSquare) * count;

			TCFreeMem(ls->p_AllFloors[i], size);

			ls->p_AllFloors[i] = NULL;
		}
	}
}

void lsDoneObjectDB(uint32 areaID) {
	LSAreaNode *area = (LSAreaNode *)dbGetObject(areaID);

	RemRelations(area->ul_ObjectBaseNr, DB_tcBuild_SIZE);
	dbDeleteAllObjects(area->ul_ObjectBaseNr, DB_tcBuild_SIZE);

	/* globale Relation wieder löschen */
	RemRelations(Relation_hasClock - 1, 3);

	/* und die "Relationsheader" wieder anlegen */
	AddRelation(Relation_hasClock);
	AddRelation(Relation_ClockTimer);
	AddRelation(Relation_StairConnects);
}

void lsDoneLandScape() {
	if (ls) {
		consistsOfAll(ls->ul_BuildingID, OLF_PRIVATE_LIST, Object_LSArea);
		NewList<dbObjectNode> *areas = ObjectListPrivate;

		int32 areaCount = 0;
		for (dbObjectNode *n = areas->getListHead(); n->_succ; n = (dbObjectNode *)n->_succ, areaCount++) {
			lsDoneObjectDB(n->_nr);

			if (ls->p_ObjectRetrievalLists[areaCount]) {
				ls->p_ObjectRetrievalLists[areaCount]->removeList();
				ls->p_ObjectRetrievalLists[areaCount] = nullptr;
				ls->ul_ObjectRetrievalAreaId[areaCount] = 0;
			}
		}

		areas->removeList();

		lsDoneActivArea(0);

		livDone();

		for (int i = 9701; i <= 9708; i++) {
			LSObjectNode *lso = (LSObjectNode *)dbGetObject(i);
			BobDone(lso->us_OffsetFact);
		}

		BobDone(ls->us_EscapeCarBobId);

		lsDoneSpots();

		lsCloseGfx();

		lsDoneFloorSquares();

		if (ls->p_DoorRefreshList)
			ls->p_DoorRefreshList->removeList();

		delete ls;
		ls = nullptr;
	}
}

void lsDoneActivArea(uint32 newAreaID) {
	livSetAllInvisible();   /* MOD 14-01-94 */
}

} // End of namespace Clue

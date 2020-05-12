/*
**  $Filename: data/database.c
**  $Release:  1
**  $Revision: 0
**  $Date:     09-03-94
**
**  database implementation for "Der Clou!"
**
** (c) 1994 ...and avoid panic by, Kaweh Kazemi
**  All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

/* includes */
#include "clue/base/base.h"

#include "clue/data/database_p.h"
#include "clue/clue.h"
#include "clue/planing/player.h"

namespace Clue {

/* public declarations */
NewObjectList<dbObjectNode> *ObjectList = nullptr;
NewObjectList<dbObjectNode> *ObjectListPrivate = nullptr;
uint32 ObjectListWidth = 0;
// Common::String (*ObjectListPrevString)(uint32, uint32, void *) = nullptr;
Common::String (*ObjectListSuccString)(uint32, uint32, dbObjectNode *) = nullptr;


/* private declarations */
uint32 ObjectListType = 0;
uint32 ObjectListFlags = OLF_NORMAL;

uint8 ObjectLoadMode = DB_LOAD_MODE_STD;

NewList<dbObjectNode> *objHash[OBJ_HASH_SIZE];
char decodeStr[11];


/* private functions - RELATION */
int dbCompare(dbObjectNode *key1, dbObjectNode *key2) {
	return (key1 == key2);
}

#if 0
char *dbDecode(KEY key) {
	dbObject *obj = dbGetObjectReal(key);

	sprintf(decodeStr, "%u", obj->nr);

	return decodeStr;
}


KEY dbEncode(char *key) {
	dbObjectNode *obj = dbGetObject(atol(key));

	if (!obj) {
		DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
		         "Could not find object <%u>!", dbGetObjectReal(key)->nr);
	}

	return obj;
}
#endif
	
dbObjectNode *dbFindRealObject(uint32 realNr, uint32 offset, uint32 size) {
	for (uint8 objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++) {
		for (dbObjectNode *obj = objHash[objHashValue]->getListHead(); obj->_succ; obj = (dbObjectNode *) obj->_succ) {
			if (obj->_nr > offset) {
				if (size && (obj->_nr > offset + size))
					continue;

				if (obj->_realNr == realNr)
					return obj;
			}
		}
	}

	return nullptr;
}


/* public functions - OBJECTS */
void dbSetLoadObjectsMode(uint8 mode) {
	ObjectLoadMode = mode;
}

static void dbRWObject(dbObjectNode *obj, int RW, uint32 type, Common::Stream *fp) {
	void(*U8_RW)(Common::Stream * fp, uint8 * x);
	void(*S8_RW)(Common::Stream * fp, int8 * x);
	void(*U16LE_RW)(Common::Stream * fp, uint16 * x);
	//void (*S16LE_RW)(Common::Stream * fp, int16 * x);
	void(*U32LE_RW)(Common::Stream * fp, uint32 * x);
	//void (*S32LE_RW)(Common::Stream * fp, int32 * x);

	if (RW == 0) {
		U8_RW = dskRead_U8;
		S8_RW = dskRead_S8;
		U16LE_RW = dskRead_U16LE;
		//S16LE_RW = dskRead_S16LE;
		U32LE_RW = dskRead_U32LE;
		//S32LE_RW = dskRead_S32LE;
	}
	else {
		U8_RW = dskWrite_U8;
		S8_RW = dskWrite_S8;
		U16LE_RW = dskWrite_U16LE;
		//S16LE_RW = dskWrite_S16LE;
		U32LE_RW = dskWrite_U32LE;
		//S32LE_RW = dskWrite_S32LE;
	}

	switch (type) {
		/* tcMain */
	case Object_Person: {
		PersonNode *x = (PersonNode *)obj;
		uint16 tmp;

		(*U16LE_RW)(fp, &x->PictID);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Job = (JobE)tmp;
		}
		else {
			tmp = x->Job;
			(*U16LE_RW)(fp, &tmp);
		}

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Sex = (SexE)tmp;
		}
		else {
			tmp = x->Sex;
			(*U16LE_RW)(fp, &tmp);
		}

		(*S8_RW)(fp, &x->Age);

		(*U8_RW)(fp, &x->Health);
		(*U8_RW)(fp, &x->Mood);

		(*U8_RW)(fp, &x->Intelligence);
		(*U8_RW)(fp, &x->Strength);
		(*U8_RW)(fp, &x->Stamina);

		(*U8_RW)(fp, &x->Loyality);
		(*U8_RW)(fp, &x->Skill);
		(*U8_RW)(fp, &x->Known);
		(*U8_RW)(fp, &x->Popularity);
		(*U8_RW)(fp, &x->Avarice);
		(*U8_RW)(fp, &x->Panic);
		(*U8_RW)(fp, &x->KnownToPolice);

		(*U32LE_RW)(fp, &x->TalkBits);
		(*U8_RW)(fp, &x->TalkFileID);

		(*U8_RW)(fp, &x->OldHealth);
		}
		break;

	case Object_Player: {
		PlayerNode *x = (PlayerNode *)obj;

		(*U32LE_RW)(fp, &x->Money);

		(*U32LE_RW)(fp, &x->StolenMoney);
		(*U32LE_RW)(fp, &x->MyStolenMoney);

		(*U8_RW)(fp, &x->NrOfBurglaries);
		(*U8_RW)(fp, &x->JobOfferCount);

		(*U8_RW)(fp, &x->MattsPart);

		(*U32LE_RW)(fp, &x->CurrScene);
		(*U32LE_RW)(fp, &x->CurrDay);
		(*U32LE_RW)(fp, &x->CurrMinute);
		(*U32LE_RW)(fp, &x->CurrLocation);
		}
		break;

	case Object_Car: {
		CarNode *x = (CarNode *)obj;
		uint16 tmp;

		(*U16LE_RW)(fp, &x->PictID);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Land = (LandE)tmp;
		}
		else {
			tmp = x->Land;
			(*U16LE_RW)(fp, &tmp);
		}

		(*U32LE_RW)(fp, &x->Value);
		(*U16LE_RW)(fp, &x->YearOfConstruction);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->ColorIndex = (ColorE)tmp;
		}
		else {
			tmp = x->ColorIndex;
			(*U16LE_RW)(fp, &tmp);
		}

		(*U8_RW)(fp, &x->Strike);

		(*U32LE_RW)(fp, &x->Capacity);
		(*U8_RW)(fp, &x->PS);
		(*U8_RW)(fp, &x->Speed);
		(*U8_RW)(fp, &x->State);
		(*U8_RW)(fp, &x->MotorState);
		(*U8_RW)(fp, &x->BodyWorkState);
		(*U8_RW)(fp, &x->TyreState);
		(*U8_RW)(fp, &x->PlacesInCar);
		}
		break;

	case Object_Location: {
		LocationNode *x = (LocationNode *)obj;

		(*U32LE_RW)(fp, &x->LocationNr);
		(*U16LE_RW)(fp, &x->OpenFromMinute);
		(*U16LE_RW)(fp, &x->OpenToMinute);
		}
		break;

	case Object_Ability: {
		AbilityNode *x = (AbilityNode *)obj;
		uint16 tmp;

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Name = (AbilityE)tmp;
		}
		else {
			tmp = x->Name;
			(*U16LE_RW)(fp, &tmp);
		}

		(*U32LE_RW)(fp, &x->Use);
		}
		break;

	case Object_Item: {
		ItemNode *x = (ItemNode *)obj;
		uint16 tmp;

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Type = (ItemE)tmp;
		}
		else {
			tmp = x->Type;
			(*U16LE_RW)(fp, &tmp);
		}

		(*U16LE_RW)(fp, &x->OffsetFact);
		(*U16LE_RW)(fp, &x->HExactXOffset);
		(*U16LE_RW)(fp, &x->HExactYOffset);
		(*U16LE_RW)(fp, &x->HExactWidth);
		(*U16LE_RW)(fp, &x->HExactHeight);
		(*U16LE_RW)(fp, &x->VExactXOffset);
		(*U16LE_RW)(fp, &x->VExactYOffset);
		(*U16LE_RW)(fp, &x->VExactWidth);
		(*U16LE_RW)(fp, &x->VExactHeight);
		(*U8_RW)(fp, &x->Size);
		(*U8_RW)(fp, &x->ColorNr);
		}
		break;

	case Object_Tool: {
		ToolNode *x = (ToolNode *)obj;

		(*U16LE_RW)(fp, &x->PictID);
		(*U32LE_RW)(fp, &x->Value);
		(*U8_RW)(fp, &x->Danger);
		(*U8_RW)(fp, &x->Volume);
		(*U8_RW)(fp, &x->Effect);
		}
		break;

	case Object_Environment: {
		EnvironmentNode *x = (EnvironmentNode *)obj;

		(*U8_RW)(fp, &x->MattHasHotelRoom);
		(*U8_RW)(fp, &x->MattHasIdentityCard);
		(*U8_RW)(fp, &x->WithOrWithoutYou);
		(*U8_RW)(fp, &x->MattIsInLove);
		(*U8_RW)(fp, &x->SouthhamptonHappened);
		(*U8_RW)(fp, &x->Present);
		(*U8_RW)(fp, &x->FirstTimeInSouth);
		if (g_clue->getFeatures() & GF_PROFIDISK)
			(*U8_RW)(fp, &x->PostzugDone);
		else
			x->PostzugDone = 0;
		}
		break;

	case Object_London: {
		LondonNode *x = (LondonNode *)obj;

		(*U8_RW)(fp, &x->Useless);
		}
		break;

	case Object_Evidence: {
		EvidenceNode *x = (EvidenceNode *)obj;

		(*U32LE_RW)(fp, &x->pers);
		(*U8_RW)(fp, &x->Recognition);
		(*U8_RW)(fp, &x->WalkTrail);
		(*U8_RW)(fp, &x->WaitTrail);
		(*U8_RW)(fp, &x->WorkTrail);
		(*U8_RW)(fp, &x->KillTrail);
		(*U8_RW)(fp, &x->CallTrail);
		(*U8_RW)(fp, &x->PaperTrail);
		(*U8_RW)(fp, &x->FotoTrail);
		}
		break;

	case Object_Loot: {
		LootNode *x = (LootNode *)obj;
		uint16 tmp;

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Type = (LootE)tmp;
		}
		else {
			tmp = x->Type;
			(*U16LE_RW)(fp, &tmp);
		}

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Name = (LootNameE)tmp;
		}
		else {
			tmp = x->Name;
			(*U16LE_RW)(fp, &tmp);
		}

		(*U32LE_RW)(fp, &x->Volume);
		(*U16LE_RW)(fp, &x->Weight);
		(*U16LE_RW)(fp, &x->PictID);
		}
		break;

	case Object_CompleteLoot: {
		CompleteLootNode *x = (CompleteLootNode *)obj;

		(*U32LE_RW)(fp, &x->Bild);
		(*U32LE_RW)(fp, &x->Gold);
		(*U32LE_RW)(fp, &x->Geld);
		(*U32LE_RW)(fp, &x->Juwelen);
		(*U32LE_RW)(fp, &x->Delikates);
		(*U32LE_RW)(fp, &x->Statue);
		(*U32LE_RW)(fp, &x->Kuriositaet);
		(*U32LE_RW)(fp, &x->HistKunst);
		(*U32LE_RW)(fp, &x->GebrauchsArt);
		(*U32LE_RW)(fp, &x->Vase);
		(*U32LE_RW)(fp, &x->TotalWeight);
		(*U32LE_RW)(fp, &x->TotalVolume);
		}
		break;

	case Object_LSOLock: {
		LSOLockNode *x = (LSOLockNode *)obj;
		uint16 tmp;

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Type = (LockE)tmp;
		}
		else {
			tmp = x->Type;
			(*U16LE_RW)(fp, &tmp);
		}
		}
		break;

	case Object_LSObject: {
		LSObjectNode *x = (LSObjectNode *)obj;

		(*U16LE_RW)(fp, &x->us_OffsetFact);
		(*U16LE_RW)(fp, &x->us_DestX);
		(*U16LE_RW)(fp, &x->us_DestY);
		(*U8_RW)(fp, &x->uch_ExactX);
		(*U8_RW)(fp, &x->uch_ExactY);
		(*U8_RW)(fp, &x->uch_ExactX1);
		(*U8_RW)(fp, &x->uch_ExactY1);
		(*U8_RW)(fp, &x->uch_Size);
		(*U8_RW)(fp, &x->uch_Visible);
		(*U8_RW)(fp, &x->uch_Chained);
		(*U32LE_RW)(fp, &x->ul_Status);
		(*U32LE_RW)(fp, &x->Type);
		}
		break;

	/* tcBuild */
	case Object_Building: {
		BuildingNode *x = (BuildingNode *)obj;
		uint16 tmp;

		(*U32LE_RW)(fp, &x->LocationNr);

		(*U16LE_RW)(fp, &x->PoliceTime);
		(*U16LE_RW)(fp, &x->GTime);

		(*U8_RW)(fp, &x->Exactlyness);

		(*U8_RW)(fp, &x->GRate);
		(*U8_RW)(fp, &x->Strike);
		(*U32LE_RW)(fp, &x->Values);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->EscapeRoute = (RouteE)tmp;
		}
		else {
			tmp = x->EscapeRoute;
			(*U16LE_RW)(fp, &tmp);
		}

		(*U8_RW)(fp, &x->EscapeRouteLength);
		(*U8_RW)(fp, &x->RadioGuarding);
		(*U8_RW)(fp, &x->MaxVolume);
		(*U8_RW)(fp, &x->GuardStrength);

		(*U16LE_RW)(fp, &x->CarXPos);
		(*U16LE_RW)(fp, &x->CarYPos);

		if (g_clue->getFeatures() & GF_PROFIDISK)
			(*U8_RW)(fp, &x->DiskId);
		else
			x->DiskId = 0;
		}
		break;

	case Object_Police: {
		PoliceNode *x = (PoliceNode *)obj;

		(*U16LE_RW)(fp, &x->PictID);
		(*U8_RW)(fp, &x->LivingID);
		}
		break;

	case Object_LSArea: {
		LSAreaNode *x = (LSAreaNode *)obj;

		(*U16LE_RW)(fp, &x->us_Coll16ID);
		(*U16LE_RW)(fp, &x->us_Coll32ID);
		(*U16LE_RW)(fp, &x->us_Coll48ID);

		(*U16LE_RW)(fp, &x->us_PlanColl16ID);
		(*U16LE_RW)(fp, &x->us_PlanColl32ID);
		(*U16LE_RW)(fp, &x->us_PlanColl48ID);

		(*U16LE_RW)(fp, &x->us_FloorCollID);
		(*U16LE_RW)(fp, &x->us_PlanFloorCollID);

		(*U16LE_RW)(fp, &x->us_Width);
		(*U16LE_RW)(fp, &x->us_Height);
		(*U32LE_RW)(fp, &x->ul_ObjectBaseNr);

		(*U8_RW)(fp, &x->uch_Darkness);

		(*U16LE_RW)(fp, &x->us_StartX0);
		(*U16LE_RW)(fp, &x->us_StartX1);
		(*U16LE_RW)(fp, &x->us_StartX2);
		(*U16LE_RW)(fp, &x->us_StartX3);
		(*U16LE_RW)(fp, &x->us_StartX4);
		(*U16LE_RW)(fp, &x->us_StartX5);

		(*U16LE_RW)(fp, &x->us_StartY0);
		(*U16LE_RW)(fp, &x->us_StartY1);
		(*U16LE_RW)(fp, &x->us_StartY2);
		(*U16LE_RW)(fp, &x->us_StartY3);
		(*U16LE_RW)(fp, &x->us_StartY4);
		(*U16LE_RW)(fp, &x->us_StartY5);
		}
		break;

	case Object_LSRoom: {
		LSRoomNode *x = (LSRoomNode *)obj;

		(*U16LE_RW)(fp, &x->us_LeftEdge);
		(*U16LE_RW)(fp, &x->us_TopEdge);
		(*U16LE_RW)(fp, &x->us_Width);
		(*U16LE_RW)(fp, &x->us_Height);
		}
		break;

	default:
		DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
			"Unknown type #%u", type);
		break;
	}
}

bool dbLoadAllObjects(const char *fileName) {
	Common::Stream *fh = dskOpen(fileName, 0);

	if (fh) {
		uint32 realNr = 1;

		while (!dskEOF(fh)) {
			uint32 objNr = 0;
			uint32 objType = 0;
			uint32 objSize = 0; // No longer used. Kept for savegame compatibility

			dskRead_U32LE(fh, &objNr);
			dskRead_U32LE(fh, &objType);
			dskRead_U32LE(fh, &objSize);

			if (objNr != (uint32) -1 && objType != (uint32) -1) {
				NewList<NewNode> *list = nullptr;
				Common::String name = Common::String("");

				if (ObjectLoadMode) {
					if ((list = g_clue->_txtMgr->goKey(OBJECTS_TXT, nullptr)))   /* MOD: old version GoNextKey */
						name = list->getListHead()->_name;
				}

				dbObjectNode *obj = dbNewObject(objNr, objType, name, realNr++);

				if (list)
					list->removeList();

				dbRWObject(obj, 0, objType, fh);
			}

			dskPeek(fh);
		}

		dskClose(fh);
		return true;
	}
	return false;
}

bool dbSaveAllObjects(const char *fileName, uint32 offset, uint32 size) {
	uint32 realNr = 1;
	uint32 dbSize = dbGetObjectCountOfDB(offset, size);

	Common::Stream* fh = dskOpen(fileName, 1);
	if (!fh)
		return false;

	while (realNr <= dbSize) {
		dbObjectNode* obj = dbFindRealObject(realNr++, offset, size);
		if (obj) {
			uint32 objNr = obj->_nr;
			uint32 objType = obj->_type;
			uint32 objSize = 0; // No longer used, but mandatory to keep savegames compatibility

			dskWrite_U32LE(fh, &objNr);
			dskWrite_U32LE(fh, &objType);
			dskWrite_U32LE(fh, &objSize);

			dbRWObject(obj, 1, objType, fh);
		}
	}

	dskClose(fh);
	return true;
}

void dbDeleteAllObjects(uint32 offset, uint32 size) {
	for (uint8 objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++) {
		for (dbObjectNode *obj = objHash[objHashValue]->getListHead(); obj->_succ; obj = (dbObjectNode *) obj->_succ) {
			if (obj->_nr > offset) {
				if (size && (obj->_nr > offset + size))
					continue;

				dbObjectNode *pred = (dbObjectNode *) obj->_pred;
				obj->remNode();   /* MOD: old version Remove */
				delete obj;
				obj = pred;
			}
		}
	}
}

uint32 dbGetObjectCountOfDB(uint32 offset, uint32 size) {
	uint32 count = 0;

	for (uint8 i = 0; i < OBJ_HASH_SIZE; i++) {
		for (dbObjectNode *obj = objHash[i]->getListHead(); obj->_succ; obj = (dbObjectNode *) obj->_succ) {
			if (obj->_nr > offset && obj->_nr < offset + size)
				count++;
		}
	}

	return count;
}


/* public functions - OBJECT */

dbObjectNode *dbNewNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) {
	dbObjectNode *newNode;

	switch (type) {
	case Object_Person:
		newNode = new PersonNode(nr, type, name, realNr);
		break;
	case Object_Player:
		newNode = new PlayerNode(nr, type, name, realNr);
		break;
	case Object_Car:
		newNode = new CarNode(nr, type, name, realNr);
		break;
	case Object_Location:
		newNode = new LocationNode(nr, type, name, realNr);
		break;
	case Object_Ability:
		newNode = new AbilityNode(nr, type, name, realNr);
		break;
	case Object_Item:
		newNode = new ItemNode(nr, type, name, realNr);
		break;
	case Object_Tool:
		newNode = new ToolNode(nr, type, name, realNr);
		break;
	case Object_Environment:
		newNode = new EnvironmentNode(nr, type, name, realNr);
		break;
	case Object_London:
		newNode = new LondonNode(nr, type, name, realNr);
		break;
	case Object_Evidence:
		newNode = new EvidenceNode(nr, type, name, realNr);
		break;
	case Object_Loot:
		newNode = new LootNode(nr, type, name, realNr);
		break;
	case Object_CompleteLoot:
		newNode = new CompleteLootNode(nr, type, name, realNr);
		break;
	case Object_LSOLock:
		newNode = new LSOLockNode(nr, type, name, realNr);
		break;
	case Object_LSObject:
		newNode = new LSObjectNode(nr, type, name, realNr);
		break;
	case Object_Building:
		newNode = new BuildingNode(nr, type, name, realNr);
		break;
	case Object_Police:
		newNode = new PoliceNode(nr, type, name, realNr);
		break;
	case Object_LSArea:
		newNode = new LSAreaNode(nr, type, name, realNr);
		break;
	case Object_LSRoom:
		newNode = new LSRoomNode(nr, type, name, realNr);
		break;
	default:
		error("dbNewNode - Unknown type");
	}

	return newNode;
}

dbObjectNode *dbNewObject(uint32 nr, uint32 type, Common::String name, uint32 realNr) {
	dbObjectNode *newNode = dbNewNode(nr, type, name, realNr);
	uint8 objHashValue = dbGetObjectHashNr(nr);
	
	objHash[objHashValue]->addTailNode(newNode);
	return newNode;
}

dbObjectNode *dbGetObject(uint32 nr) {
	uint8 objHashValue = dbGetObjectHashNr(nr);

	for (dbObjectNode *obj = objHash[objHashValue]->getListHead(); obj->_succ; obj = (dbObjectNode *)obj->_succ) {
		if (obj->_nr == nr)
			return obj;
	}

	return nullptr;
}

Common::String dbGetObjectName(uint32 nr) {
	uint8 objHashValue = dbGetObjectHashNr(nr);

	for (dbObjectNode *obj = objHash[objHashValue]->getListHead(); obj->_succ; obj = (dbObjectNode *)obj->_succ) {
		if (obj->_nr == nr)
			return obj->_name;
	}

	return nullptr;
}

bool dbIsObject(uint32 nr, uint32 type) {
	uint8 objHashValue = dbGetObjectHashNr(nr);

	for (dbObjectNode *obj = objHash[objHashValue]->getListHead(); obj->_succ; obj = (dbObjectNode *) obj->_succ) {
		if (obj->_nr == nr) {
			if (obj->_type == type)
				return true;

			break;
		}
	}

	return false;
}

void dbObjectMapper(dbObjectNode* destNode, dbObjectNode* srcNode) {
	if (destNode->_type != srcNode->_type)
		error("dbObjectMapper - Type mismatch");

	switch (destNode->_type) {
	case Object_Person:
		((PersonNode *)destNode)->mapper(srcNode);
		break;
	case Object_Player:
		((PlayerNode *)destNode)->mapper(srcNode);
		break;
	case Object_Car:
		((CarNode *)destNode)->mapper(srcNode);
		break;
	case Object_Location:
		((LocationNode *)destNode)->mapper(srcNode);
		break;
	case Object_Ability:
		((AbilityNode *)destNode)->mapper(srcNode);
		break;
	case Object_Item:
		((ItemNode *)destNode)->mapper(srcNode);
		break;
	case Object_Tool:
		((ToolNode *)destNode)->mapper(srcNode);
		break;
	case Object_Environment:
		((EnvironmentNode *)destNode)->mapper(srcNode);
		break;
	case Object_London:
		((LondonNode *)destNode)->mapper(srcNode);
		break;
	case Object_Evidence:
		((EvidenceNode *)destNode)->mapper(srcNode);
		break;
	case Object_Loot:
		((LootNode *)destNode)->mapper(srcNode);
		break;
	case Object_CompleteLoot:
		((CompleteLootNode *)destNode)->mapper(srcNode);
		break;
	case Object_LSOLock:
		((LSOLockNode *)destNode)->mapper(srcNode);
		break;
	case Object_LSObject:
		((LSObjectNode *)destNode)->mapper(srcNode);
		break;
	case Object_Building:
		((BuildingNode *)destNode)->mapper(srcNode);
		break;
	case Object_Police:
		((PoliceNode *)destNode)->mapper(srcNode);
		break;
	case Object_LSArea:
		((LSAreaNode *)destNode)->mapper(srcNode);
		break;
	case Object_LSRoom:
		((LSRoomNode *)destNode)->mapper(srcNode);
		break;
	default:
		error("dbObjectMapper - Unknown type");
	}	
}

/* public prototypes - OBJECTNODE */
void dbAddObjectNode(NewObjectList<dbObjectNode> *objectList, uint32 nr, uint32 flags) {	
	dbObjectNode *obj = dbGetObject(nr);
	char name[TXT_KEY_LENGTH], *namePtr;

	name[0] = EOS;

	if (flags & OLF_INSERT_STAR)
		strcpy(name, "*");

	if (flags & OLF_INCLUDE_NAME) {
		Common::String succString;

//		if ((flags & OLF_ADD_PREV_STRING) && ObjectListPrevString)
//			strcat(name, ObjectListPrevString(obj->nr, obj->type, dbGetObjectKey(obj)).c_str());

		strcat(name, obj->_name.c_str());
		namePtr = name;

		if ((flags & OLF_ADD_SUCC_STRING) && ObjectListSuccString)
			succString = ObjectListSuccString(obj->_nr, obj->_type, obj);

		if ((flags & (OLF_ADD_SUCC_STRING | OLF_ALIGNED)) && ObjectListWidth) {
			uint8 len = strlen(name) + succString.size();

			if (flags & OLF_INSERT_STAR)
				len--;

			for (uint8 i = len; i < ObjectListWidth; i++)
				strcat(name, " ");
		}

		if ((flags & OLF_ADD_SUCC_STRING) && ObjectListSuccString)
			strcat(name, succString.c_str());
	} else
		namePtr = nullptr;

	dbObjectNode *newNode = dbNewNode(obj->_nr, obj->_type, namePtr, obj->_realNr); // CHECKME : _realNr has been added during a refactoring
	dbObjectMapper(newNode, obj);
	objectList->addTailNode(newNode);
}

void dbRemObjectNode(NewList<dbObjectNode> *objectList, uint32 nr) {
	dbObjectNode *n = dbHasObjectNode(objectList, nr);

	if (n) {
		n->remNode();
		delete n;
	}
}

dbObjectNode *dbHasObjectNode(NewList<dbObjectNode> *objectList, uint32 nr) {
	for (dbObjectNode *n = objectList->getListHead(); n->_succ; n = (dbObjectNode *)n->_succ) {
		if (n->_nr == nr)
			return n;
	}

	return nullptr;
}

void SetObjectListAttr(uint32 flags, uint32 type) {
	ObjectListType = type;
	ObjectListFlags = flags;

	if (ObjectListFlags & OLF_PRIVATE_LIST)
		ObjectListPrivate = new NewObjectList<dbObjectNode>;
	else {
		ObjectList->removeList();
		delete ObjectList;
		ObjectList = new NewObjectList<dbObjectNode>;
	}
}

void BuildObjectList(dbObjectNode* obj) {
	if (!ObjectListType || (obj->_type == ObjectListType)) {
		NewObjectList<dbObjectNode>* list;
		if (ObjectListFlags & OLF_PRIVATE_LIST)
			list = ObjectListPrivate;
		else
			list = ObjectList;

		dbAddObjectNode(list, obj->_nr, ObjectListFlags);
	}
}

int16 dbStdCompareObjects(dbObjectNode *obj1, dbObjectNode *obj2) {
	if (obj1->_nr > obj2->_nr)
		return -1;

	if (obj1->_nr < obj2->_nr)
		return 1;

	return 0;
}

void dbSortPartOfList(NewObjectList<dbObjectNode> *l, dbObjectNode *start, dbObjectNode *end,
                      int16(*processNode)(dbObjectNode *, dbObjectNode *)) {
	NewObjectList<dbObjectNode> *newList = new NewObjectList<dbObjectNode>;
	dbObjectNode *n1, *startPred;

	if (start == l->getListHead())
		startPred = nullptr;
	else
		startPred = (dbObjectNode *) start->_pred;

	int32 i = 1;
	for (dbObjectNode* node = start; node != end && node->_succ; node = (dbObjectNode *) node->_succ)
		++i;

	dbObjectNode *n = start;
	for (int32 j = 0; j < i; j++) {
		n1 = n;
		n = (dbObjectNode*) n->_succ;

		n1->remNode();
		newList->addHeadNode(n1);
	}

	dbSortObjectList(&newList, processNode);

	for (n = newList->getListHead(); n->_succ;) {
		n1 = n;
		n = (dbObjectNode *)n->_succ;

		n1->remNode();
		l->addNode(n1, startPred);

		startPred = n1;
	}

	newList->removeList();
}

void dbSortObjectList(NewObjectList<dbObjectNode> **objectList, int16(*processNode)(dbObjectNode *, dbObjectNode *)) {
	if ((*objectList)->isEmpty())
		return;

	NewObjectList<dbObjectNode> *newList = new NewObjectList<dbObjectNode>;

	for (dbObjectNode *n1 = (*objectList)->getListHead(); n1->_succ; n1 = (dbObjectNode *) n1->_succ) {
		 dbObjectNode *pred = nullptr;

		if (!newList->isEmpty()) {
			for (dbObjectNode *n2 = newList->getListHead(); !pred && n2->_succ; n2 = (dbObjectNode *) n2->_succ) {
				if (processNode(n1, n2) >= 0)
					pred = n2;
			}
		}

		dbObjectNode *newNode = dbNewNode(n1->_nr, n1->_type, n1->_name, n1->_realNr);
		dbObjectMapper(newNode, n1);

		if (pred) {
			if (pred == (dbObjectNode *) newList->getListHead())
				newList->addHeadNode(newNode);
			else
				newList->addNode(newNode, (dbObjectNode *)pred->_pred);
		} else
			newList->addTailNode(newNode);
	}

	if (!newList->isEmpty()) {
		(*objectList)->removeList();
		*objectList = newList;
	} else
		newList->removeList();
}


/* public prototypes */
void dbInit() {
	ObjectList = new NewObjectList<dbObjectNode>;

	for (uint8 objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++)
		objHash[objHashValue] = new NewList<dbObjectNode>;

	CompareKey = dbCompare;
//	EncodeKey = dbEncode;
//	DecodeKey = dbDecode;
}

void dbDone() {
	for (uint8 objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++) {
		if (objHash[objHashValue])
			objHash[objHashValue]->removeList();
	}

	if (ObjectList)
		ObjectList->removeList();
}

static uint32 getKeyStd(KeyConflictE key) {
	switch (key) {
	case _Environment_TheClou:
		return   20;
	case _Player_Player_1:
		return   21;
	case _London_London_1:
		return   22;
	case _London_Jail:
		return   23;
	case _London_Escape:
		return   24;
	case _Evidence_Evidence_1:
		return   25;
	case _CompleteLoot_LastLoot:
		return   27;
	case _Person_Old_Matt:
		return   28;

	case _Location_Holland_Street:
		return   77;
	case _Location_Fat_Mans_Pub:
		return   81;
	case _Location_Cars_Vans_Office:
		return   82;
	case _Location_Tools_Shop:
		return   83;
	case _Location_Policestation:
		return   84;
	case _Location_Highgate_Out:
		return   98;
	case _Location_Hotel:
		return  124;
	case _Location_Walrus:
		return  125;
	case _Location_Parker:
		return  128;
	case _Location_Maloya:
		return  129;
	case _Location_Pooly:
		return  130;
	case _Location_Nirvana:
		return  133;

	case _Ability_Autos:
		return  140;
	case _Ability_Locks:
		return  141;
	case _Ability_Explosive:
		return  142;
	case _Ability_Safes:
		return  143;
	case _Ability_Electronic:
		return  144;
	case _Ability_Surveillance:
		return  145;
	case _Ability_Fight:
		return  146;

	default:
		DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
			"Unknown trouble key #%u", key);
		return 0;
	}
}

static uint32 getKeyProfi(KeyConflictE key) {
	switch (key) {
	case _Environment_TheClou:
		return   28;
	case _Player_Player_1:
		return   29;
	case _London_London_1:
		return   30;
	case _London_Jail:
		return   31;
	case _London_Escape:
		return   32;
	case _Evidence_Evidence_1:
		return   33;
	case _CompleteLoot_LastLoot:
		return   35;
	case _Person_Old_Matt:
		return   36;

	case _Location_Holland_Street:
		return   95;
	case _Location_Fat_Mans_Pub:
		return   99;
	case _Location_Cars_Vans_Office:
		return  100;
	case _Location_Tools_Shop:
		return  101;
	case _Location_Policestation:
		return  102;
	case _Location_Highgate_Out:
		return  116;
	case _Location_Hotel:
		return  142;
	case _Location_Walrus:
		return  143;
	case _Location_Parker:
		return  146;
	case _Location_Maloya:
		return  147;
	case _Location_Pooly:
		return  148;
	case _Location_Nirvana:
		return  151;

	case _Ability_Autos:
		return  173;
	case _Ability_Locks:
		return  174;
	case _Ability_Explosive:
		return  175;
	case _Ability_Safes:
		return  176;
	case _Ability_Electronic:
		return  177;
	case _Ability_Surveillance:
		return  178;
	case _Ability_Fight:
		return  179;

	default:
		DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
			"Unknown trouble key #%u", key);
		return 0;
	}
}

uint32 getKey(KeyConflictE key) {
	if (g_clue->getFeatures() & GF_PROFIDISK)
		return getKeyProfi(key);
	
	return getKeyStd(key);
}

#if 0
void dbDeleteObject(uint32 nr) {
	dbObject *obj = NULL;
	uint8 objHashValue = dbGetObjectHashNr(nr);

	for (obj = (dbObject *) LIST_HEAD(objHash[objHashValue]);
		NODE_SUCC(obj); obj = (dbObject *) NODE_SUCC(obj)) {
			if (obj->nr == nr) {
				UnSetAll((KEY)(obj + 1), NULL);
				RemNode(obj);
				FreeNode(obj);
				return;
			}
	}
}
#endif

void PersonNode::mapper(dbObjectNode *srcNode) {
	PersonNode *src = (PersonNode *)srcNode;
	PictID = src->PictID;
	Job = src->Job;
	Sex = src->Sex;
	Age = src->Age;
	Health = src->Health;
	Mood = src->Mood;
	Intelligence = src->Intelligence;
	Strength = src->Strength;
	Stamina = src->Stamina;
	Loyality = src->Loyality;
	Skill = src->Skill;
	Known = src->Known;
	Popularity = src->Popularity;
	Avarice = src->Avarice;
	Panic = src->Panic;
	KnownToPolice = src->KnownToPolice;
	TalkBits = src->TalkBits;
	TalkFileID = src->TalkFileID;
	OldHealth = src->OldHealth;
}

void PlayerNode::mapper(dbObjectNode* srcNode) {
	PlayerNode *src = (PlayerNode *)srcNode;

	Money = src->Money;
	StolenMoney = src->StolenMoney;
	MyStolenMoney = src->MyStolenMoney;
	NrOfBurglaries = src->NrOfBurglaries;
	JobOfferCount = src->JobOfferCount;
	MattsPart = src->MattsPart;
	CurrScene = src->CurrScene;
	CurrDay = src->CurrDay;
	CurrMinute = src->CurrMinute;
	CurrLocation = src->CurrLocation;
}

void CarNode::mapper(dbObjectNode* srcNode) {
	CarNode *src = (CarNode *)srcNode;
	PictID = src->PictID;
	Land = src->Land;
	Value = src->Value;
	YearOfConstruction = src->YearOfConstruction;
	ColorIndex = src->ColorIndex;
	Strike = src->Strike;
	Capacity = src->Capacity;
	PS = src->PS;
	Speed = src->Speed;
	State = src->State;
	MotorState = src->MotorState;
	BodyWorkState = src->BodyWorkState;
	TyreState = src->TyreState;
	PlacesInCar = src->PlacesInCar;
}

void LocationNode::mapper(dbObjectNode* srcNode) {
	LocationNode *src = (LocationNode *)srcNode;
	LocationNr = src->LocationNr;
	OpenFromMinute = src->OpenFromMinute;
	OpenToMinute = src->OpenToMinute;
}

void AbilityNode::mapper(dbObjectNode* srcNode) {
	AbilityNode *src = (AbilityNode *)srcNode;
	Name = src->Name;
	Use = src->Use;
}

void ItemNode::mapper(dbObjectNode* srcNode) {
	ItemNode *src = (ItemNode *)srcNode;
	Type = src->Type;
	OffsetFact = src->OffsetFact;
	HExactXOffset = src->HExactXOffset;
	HExactYOffset = src->HExactYOffset;
	HExactWidth = src->HExactWidth;
	HExactHeight = src->HExactHeight;
	VExactXOffset = src->VExactXOffset;
	VExactYOffset = src->VExactYOffset;
	VExactWidth = src->VExactWidth;
	VExactHeight = src->VExactHeight;
	Size = src->Size;
	ColorNr = src->ColorNr;
}

void ToolNode::mapper(dbObjectNode* srcNode) {
	ToolNode *src = (ToolNode *)srcNode;
	PictID = src->PictID;
	Value = src->Value;
	Danger = src->Danger;
	Volume = src->Volume;
	Effect = src->Effect;
}

void EnvironmentNode::mapper(dbObjectNode* srcNode) {
	EnvironmentNode *src = (EnvironmentNode *)srcNode;
	MattHasHotelRoom = src->MattHasHotelRoom;
	MattHasIdentityCard = src->MattHasIdentityCard;
	WithOrWithoutYou = src->WithOrWithoutYou;
	MattIsInLove = src->MattIsInLove;
	SouthhamptonHappened = src->SouthhamptonHappened;
	Present = src->Present;
	FirstTimeInSouth = src->FirstTimeInSouth;
	PostzugDone = src->PostzugDone;
}

void LondonNode::mapper(dbObjectNode* srcNode) {
	LondonNode *src = (LondonNode *)srcNode;
	Useless = src->Useless;
}

void EvidenceNode::mapper(dbObjectNode* srcNode) {
	EvidenceNode *src = (EvidenceNode *)srcNode;
	pers = src->pers;
	Recognition = src->Recognition;
	WalkTrail = src->WalkTrail;
	WaitTrail = src->WaitTrail;
	WorkTrail = src->WorkTrail;
	KillTrail = src->KillTrail;
	CallTrail = src->CallTrail;
	PaperTrail = src->PaperTrail;
	FotoTrail = src->FotoTrail;
}

void LootNode::mapper(dbObjectNode* srcNode) {
	LootNode *src = (LootNode *)srcNode;
	Type = src->Type;
	Name = src->Name;
	Volume = src->Volume;
	Weight = src->Weight;
	PictID = src->PictID;
}

void CompleteLootNode::mapper(dbObjectNode* srcNode) {
	CompleteLootNode *src = (CompleteLootNode *)srcNode;
	Bild = src->Bild;
	Gold = src->Gold;
	Geld  =src->Geld;
	Juwelen = src->Juwelen;
	Delikates = src->Delikates;
	Statue = src->Statue;
	Kuriositaet = src->Kuriositaet;
	HistKunst = src->HistKunst;
	GebrauchsArt = src->GebrauchsArt;
	Vase = src->Vase;
	TotalWeight = src->TotalWeight;
	TotalVolume = src->TotalVolume;
}

void LSOLockNode::mapper(dbObjectNode* srcNode) {
	LSOLockNode *src = (LSOLockNode *)srcNode;
	Type = src->Type;
}

void LSObjectNode::mapper(dbObjectNode* srcNode) {
	LSObjectNode *src = (LSObjectNode *)srcNode;
	us_OffsetFact = src->us_OffsetFact;
	us_DestX = src->us_DestX;
	us_DestY = src->us_DestY;
	uch_ExactX = src->uch_ExactX;
	uch_ExactY = src->uch_ExactY;
	uch_ExactX1 = src->uch_ExactX1;
	uch_ExactY1 = src->uch_ExactY1;
	uch_Size = src->uch_Size;
	uch_Visible = src->uch_Visible;
	uch_Chained = src->uch_Chained;
	ul_Status = src->ul_Status;
	Type = src->Type;
}

void BuildingNode::mapper(dbObjectNode* srcNode) {
	BuildingNode *src = (BuildingNode *)srcNode;
	LocationNr = src->LocationNr;
	PoliceTime = src->PoliceTime;
	GTime = src->GTime;
	Exactlyness = src->Exactlyness;
	GRate = src->GRate;
	Strike = src->Strike;
	Values = src->Values;
	EscapeRoute = src->EscapeRoute;
	EscapeRouteLength = src->EscapeRouteLength;
	RadioGuarding = src->RadioGuarding;
	MaxVolume = src->MaxVolume;
	GuardStrength = src->GuardStrength;
	CarXPos = src->CarXPos;
	CarYPos = src->CarYPos;
	DiskId = src->DiskId;
}

void PoliceNode::mapper(dbObjectNode* srcNode) {
	PoliceNode *src = (PoliceNode *)srcNode;
	PictID = src->PictID;
	LivingID = src->LivingID;
}

void LSAreaNode::mapper(dbObjectNode* srcNode) {
	LSAreaNode *src = (LSAreaNode *)srcNode;
	us_Coll16ID = src->us_Coll16ID;
	us_Coll32ID = src->us_Coll32ID;
	us_Coll48ID = src->us_Coll48ID;
	us_PlanColl16ID = src->us_PlanColl16ID;
	us_PlanColl32ID = src->us_PlanColl32ID;
	us_PlanColl48ID = src->us_PlanColl48ID;
	us_FloorCollID = src->us_FloorCollID;
	us_PlanFloorCollID = src->us_PlanFloorCollID;
	us_Width = src->us_Width;
	us_Height = src->us_Height;
	ul_ObjectBaseNr = src->ul_ObjectBaseNr;
	uch_Darkness = src->uch_Darkness;
	us_StartX0 = src->us_StartX0;
	us_StartX1 = src->us_StartX1;
	us_StartX2 = src->us_StartX2;
	us_StartX3 = src->us_StartX3;
	us_StartX4 = src->us_StartX4;
	us_StartX5 = src->us_StartX5;
	us_StartY0 = src->us_StartY0;
	us_StartY1 = src->us_StartY1;
	us_StartY2 = src->us_StartY2;
	us_StartY3 = src->us_StartY3;
	us_StartY4 = src->us_StartY4;
	us_StartY5 = src->us_StartY5;
}

void LSRoomNode::mapper(dbObjectNode* srcNode) {
	LSRoomNode *src = (LSRoomNode *)srcNode;
	us_LeftEdge = src->us_LeftEdge;
	us_TopEdge = src->us_TopEdge;
	us_Width = src->us_Width;
	us_Height = src->us_Height;
}

} // End of namespace Clue

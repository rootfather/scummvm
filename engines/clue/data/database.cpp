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

#include "clue/data/database.h"
#include "clue/data/database_p.h"
#include "clue/clue.h"

namespace Clue {

/* public declarations */
LIST *ObjectList = NULL;
LIST *ObjectListPrivate = NULL;
uint32 ObjectListWidth = 0L;
char *(*ObjectListPrevString)(uint32, uint32, void *) = NULL;
char *(*ObjectListSuccString)(uint32, uint32, void *) = NULL;


/* private declarations */
uint32 ObjectListType = 0L;
uint32 ObjectListFlags = OLF_NORMAL;

uint8 ObjectLoadMode = DB_LOAD_MODE_STD;

LIST *objHash[OBJ_HASH_SIZE];
char decodeStr[11];


/* private functions - RELATION */
int dbCompare(KEY key1, KEY key2) {
	return (key1 == key2);
}

char *dbDecode(KEY key) {
	struct dbObject *obj = dbGetObjectReal(key);

	sprintf(decodeStr, "%u", obj->nr);

	return decodeStr;
}


KEY dbEncode(char *key) {
	void *obj = dbGetObject(atol(key));

	if (!obj) {
		DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
		         "Could not find object <%u>!", dbGetObjectReal(key)->nr);
	}

	return obj;
}

struct dbObject *dbFindRealObject(uint32 realNr, uint32 offset, uint32 size) {
	struct dbObject *obj;
	uint8 objHashValue;

	for (objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++) {
		for (obj = (struct dbObject *) LIST_HEAD(objHash[objHashValue]);
		        NODE_SUCC(obj); obj = (struct dbObject *) NODE_SUCC(obj)) {
			if (obj->nr > offset) {
				if (size && (obj->nr > offset + size))
					continue;

				if (obj->realNr == realNr)
					return obj;
			}
		}
	}

	return NULL;
}


/* public functions - OBJECTS */
void dbSetLoadObjectsMode(uint8 mode) {
	ObjectLoadMode = mode;
}

#define DB_CHECK_SIZE(x) \
	if (sizeof(*(x)) != localSize) \
	{ \
		DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE, \
		         "Size mismatch (%d != %u)", sizeof(*(x)), localSize); \
	}

static uint32 dbGetStdDskSize(uint32 type) {
	switch (type) {
	/* tcMain */
	case Object_Person:
		return 25;
	case Object_Player:
		return 31;
	case Object_Car:
		return 24;
	case Object_Location:
		return 8;
	case Object_Ability:
		return 6;
	case Object_Item:
		return 22;
	case Object_Tool:
		return 9;
	case Object_Environment:
		return 7;
	case Object_London:
		return 1;
	case Object_Evidence:
		return 12;
	case Object_Loot:
		return 12;
	case Object_CompleteLoot:
		return 48;
	case Object_LSOLock:
		return 2;
	case Object_LSObject:
		return 21;

	/* tcBuild */
	case Object_Building:
		return 25;
	case Object_Police:
		return 3;
	case Object_LSArea:
		return 49;
	case Object_LSRoom:
		return 8;
	default:
		DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
		         "Unknown type #%u", type);
		return 0;
	}
}

static uint32 dbGetProfiDskSize(uint32 type) {
	switch (type) {
	/* tcMain */
	case Object_Person:
		return 25;
	case Object_Player:
		return 31;
	case Object_Car:
		return 24;
	case Object_Location:
		return 8;
	case Object_Ability:
		return 6;
	case Object_Item:
		return 22;
	case Object_Tool:
		return 9;
	case Object_Environment:
		return 8;
	case Object_London:
		return 1;
	case Object_Evidence:
		return 12;
	case Object_Loot:
		return 12;
	case Object_CompleteLoot:
		return 48;
	case Object_LSOLock:
		return 2;
	case Object_LSObject:
		return 21;

	/* tcBuild */
	case Object_Building:
		return 26;
	case Object_Police:
		return 3;
	case Object_LSArea:
		return 49;
	case Object_LSRoom:
		return 8;
	default:
		DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
		         "Unknown type #%u", type);
		return 0;
	}
}

static uint32 dbGetDskSize(uint32 type) {
	if (g_clue->getFeatures() & GF_PROFIDISK) {
		return dbGetProfiDskSize(type);
	} else {
		return dbGetStdDskSize(type);
	}
}


static uint32 dbGetMemSize(uint32 type) {
	switch (type) {
	/* tcMain */
	case Object_Person:
		return Object_Person_Size;
	case Object_Player:
		return Object_Player_Size;
	case Object_Car:
		return Object_Car_Size;
	case Object_Location:
		return Object_Location_Size;
	case Object_Ability:
		return Object_Ability_Size;
	case Object_Item:
		return Object_Item_Size;
	case Object_Tool:
		return Object_Tool_Size;
	case Object_Environment:
		return Object_Environment_Size;
	case Object_London:
		return Object_London_Size;
	case Object_Evidence:
		return Object_Evidence_Size;
	case Object_Loot:
		return Object_Loot_Size;
	case Object_CompleteLoot:
		return Object_CompleteLoot_Size;
	case Object_LSOLock:
		return Object_LSOLock_Size;
	case Object_LSObject:
		return Object_LSObject_Size;

	/* tcBuild */
	case Object_Building:
		return Object_Building_Size;
	case Object_Police:
		return Object_Police_Size;
	case Object_LSArea:
		return Object_LSArea_Size;
	case Object_LSRoom:
		return Object_LSRoom_Size;
	default:
		DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
		         "Unknown type #%u", type);
		return 0;
	}
}

static void
dbRWStdObject(void *obj, int RW, uint32 type, uint32 size, uint32 localSize, FILE *fp) {
	void (*U8_RW)(FILE * fp, uint8 * x);
	void (*S8_RW)(FILE * fp, int8 * x);
	void (*U16LE_RW)(FILE * fp, uint16 * x);
	void (*S16LE_RW)(FILE * fp, int16 * x);
	void (*U32LE_RW)(FILE * fp, uint32 * x);
	void (*S32LE_RW)(FILE * fp, int32 * x);

	if (RW == 0) {
		U8_RW = dskRead_U8;
		S8_RW = dskRead_S8;
		U16LE_RW = dskRead_U16LE;
		S16LE_RW = dskRead_S16LE;
		U32LE_RW = dskRead_U32LE;
		S32LE_RW = dskRead_S32LE;
	} else {
		U8_RW = dskWrite_U8;
		S8_RW = dskWrite_S8;
		U16LE_RW = dskWrite_U16LE;
		S16LE_RW = dskWrite_S16LE;
		U32LE_RW = dskWrite_U32LE;
		S32LE_RW = dskWrite_S32LE;
	}

	switch (type) {
	/* tcMain */
	case Object_Person: {
		Person x = (Person)obj;
		uint16 tmp;

		DB_CHECK_SIZE(x);

		(*U16LE_RW)(fp, &x->PictID);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Job = (JobE)tmp;
		} else {
			tmp = x->Job;
			(*U16LE_RW)(fp, &tmp);
		}

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Sex = (SexE)tmp;
		} else {
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
		Player x = (Player)obj;

		DB_CHECK_SIZE(x);

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
		Car x = (Car)obj;
		uint16 tmp;

		DB_CHECK_SIZE(x);

		(*U16LE_RW)(fp, &x->PictID);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Land = (LandE)tmp;
		} else {
			tmp = x->Land;
			(*U16LE_RW)(fp, &tmp);
		}

		(*U32LE_RW)(fp, &x->Value);
		(*U16LE_RW)(fp, &x->YearOfConstruction);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->ColorIndex = (ColorE)tmp;
		} else {
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
		Location x = (Location)obj;

		DB_CHECK_SIZE(x);

		(*U32LE_RW)(fp, &x->LocationNr);
		(*U16LE_RW)(fp, &x->OpenFromMinute);
		(*U16LE_RW)(fp, &x->OpenToMinute);
	}
	break;

	case Object_Ability: {
		Ability x = (Ability)obj;
		uint16 tmp;

		DB_CHECK_SIZE(x);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Name = (AbilityE)tmp;
		} else {
			tmp = x->Name;
			(*U16LE_RW)(fp, &tmp);
		}

		(*U32LE_RW)(fp, &x->Use);
	}
	break;

	case Object_Item: {
		Item x = (Item)obj;
		uint16 tmp;

		DB_CHECK_SIZE(x);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Type = (ItemE)tmp;
		} else {
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
		Tool x = (Tool)obj;

		DB_CHECK_SIZE(x);

		(*U16LE_RW)(fp, &x->PictID);
		(*U32LE_RW)(fp, &x->Value);
		(*U8_RW)(fp, &x->Danger);
		(*U8_RW)(fp, &x->Volume);
		(*U8_RW)(fp, &x->Effect);
	}
	break;

	case Object_Environment: {
		Environment x = (Environment)obj;

		DB_CHECK_SIZE(x);

		(*U8_RW)(fp, &x->MattHasHotelRoom);
		(*U8_RW)(fp, &x->MattHasIdentityCard);
		(*U8_RW)(fp, &x->WithOrWithoutYou);
		(*U8_RW)(fp, &x->MattIsInLove);
		(*U8_RW)(fp, &x->SouthhamptonHappened);
		(*U8_RW)(fp, &x->Present);
		(*U8_RW)(fp, &x->FirstTimeInSouth);

		x->PostzugDone = 0;
	}
	break;

	case Object_London: {
		London x = (London)obj;

		DB_CHECK_SIZE(x);

		(*U8_RW)(fp, &x->Useless);
	}
	break;

	case Object_Evidence: {
		Evidence x = (Evidence)obj;

		DB_CHECK_SIZE(x);

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
		Loot x = (Loot)obj;
		uint16 tmp;

		DB_CHECK_SIZE(x);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Type = (LootE)tmp;
		} else {
			tmp = x->Type;
			(*U16LE_RW)(fp, &tmp);
		}

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Name = (LootNameE)tmp;
		} else {
			tmp = x->Name;
			(*U16LE_RW)(fp, &tmp);
		}

		(*U32LE_RW)(fp, &x->Volume);
		(*U16LE_RW)(fp, &x->Weight);
		(*U16LE_RW)(fp, &x->PictID);
	}
	break;

	case Object_CompleteLoot: {
		CompleteLoot x = (CompleteLoot)obj;

		DB_CHECK_SIZE(x);

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
		LSOLock x = (LSOLock)obj;
		uint16 tmp;

		DB_CHECK_SIZE(x);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Type = (LockE)tmp;
		} else {
			tmp = x->Type;
			(*U16LE_RW)(fp, &tmp);
		}
	}
	break;

	case Object_LSObject: {
		LSObject x = (LSObject)obj;

		DB_CHECK_SIZE(x);

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
		Building x = (Building)obj;
		uint16 tmp;

		DB_CHECK_SIZE(x);

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
		} else {
			tmp = x->EscapeRoute;
			(*U16LE_RW)(fp, &tmp);
		}

		(*U8_RW)(fp, &x->EscapeRouteLength);
		(*U8_RW)(fp, &x->RadioGuarding);
		(*U8_RW)(fp, &x->MaxVolume);
		(*U8_RW)(fp, &x->GuardStrength);

		(*U16LE_RW)(fp, &x->CarXPos);
		(*U16LE_RW)(fp, &x->CarYPos);

		x->DiskId = 0;
	}
	break;

	case Object_Police: {
		Police x = (Police)obj;

		DB_CHECK_SIZE(x);

		(*U16LE_RW)(fp, &x->PictID);
		(*U8_RW)(fp, &x->LivingID);
	}
	break;

	case Object_LSArea: {
		LSArea x = (LSArea)obj;

		DB_CHECK_SIZE(x);

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
		LSRoom x = (LSRoom)obj;

		DB_CHECK_SIZE(x);

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

static void
dbRWProfiObject(void *obj, int RW, uint32 type, uint32 size, uint32 localSize, FILE *fp) {
	void (*U8_RW)(FILE * fp, uint8 * x);
	void (*S8_RW)(FILE * fp, int8 * x);
	void (*U16LE_RW)(FILE * fp, uint16 * x);
	void (*S16LE_RW)(FILE * fp, int16 * x);
	void (*U32LE_RW)(FILE * fp, uint32 * x);
	void (*S32LE_RW)(FILE * fp, int32 * x);

	if (RW == 0) {
		U8_RW = dskRead_U8;
		S8_RW = dskRead_S8;
		U16LE_RW = dskRead_U16LE;
		S16LE_RW = dskRead_S16LE;
		U32LE_RW = dskRead_U32LE;
		S32LE_RW = dskRead_S32LE;
	} else {
		U8_RW = dskWrite_U8;
		S8_RW = dskWrite_S8;
		U16LE_RW = dskWrite_U16LE;
		S16LE_RW = dskWrite_S16LE;
		U32LE_RW = dskWrite_U32LE;
		S32LE_RW = dskWrite_S32LE;
	}

	switch (type) {
	/* tcMain */
	case Object_Person: {
		Person x = (Person)obj;
		uint16 tmp;

		DB_CHECK_SIZE(x);

		(*U16LE_RW)(fp, &x->PictID);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Job = (JobE)tmp;
		} else {
			tmp = x->Job;
			(*U16LE_RW)(fp, &tmp);
		}

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Sex = (SexE)tmp;
		} else {
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
		Player x = (Player)obj;

		DB_CHECK_SIZE(x);

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
		Car x = (Car)obj;
		uint16 tmp;

		DB_CHECK_SIZE(x);

		(*U16LE_RW)(fp, &x->PictID);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Land = (LandE)tmp;
		} else {
			tmp = x->Land;
			(*U16LE_RW)(fp, &tmp);
		}

		(*U32LE_RW)(fp, &x->Value);
		(*U16LE_RW)(fp, &x->YearOfConstruction);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->ColorIndex = (ColorE)tmp;
		} else {
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
		Location x = (Location)obj;

		DB_CHECK_SIZE(x);

		(*U32LE_RW)(fp, &x->LocationNr);
		(*U16LE_RW)(fp, &x->OpenFromMinute);
		(*U16LE_RW)(fp, &x->OpenToMinute);
	}
	break;

	case Object_Ability: {
		Ability x = (Ability)obj;
		uint16 tmp;

		DB_CHECK_SIZE(x);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Name = (AbilityE)tmp;
		} else {
			tmp = x->Name;
			(*U16LE_RW)(fp, &tmp);
		}

		(*U32LE_RW)(fp, &x->Use);
	}
	break;

	case Object_Item: {
		Item x = (Item)obj;
		uint16 tmp;

		DB_CHECK_SIZE(x);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Type = (ItemE)tmp;
		} else {
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
		Tool x = (Tool)obj;

		DB_CHECK_SIZE(x);

		(*U16LE_RW)(fp, &x->PictID);
		(*U32LE_RW)(fp, &x->Value);
		(*U8_RW)(fp, &x->Danger);
		(*U8_RW)(fp, &x->Volume);
		(*U8_RW)(fp, &x->Effect);
	}
	break;

	case Object_Environment: {
		Environment x = (Environment)obj;

		DB_CHECK_SIZE(x);

		(*U8_RW)(fp, &x->MattHasHotelRoom);
		(*U8_RW)(fp, &x->MattHasIdentityCard);
		(*U8_RW)(fp, &x->WithOrWithoutYou);
		(*U8_RW)(fp, &x->MattIsInLove);
		(*U8_RW)(fp, &x->SouthhamptonHappened);
		(*U8_RW)(fp, &x->Present);
		(*U8_RW)(fp, &x->FirstTimeInSouth);
		(*U8_RW)(fp, &x->PostzugDone);
	}
	break;

	case Object_London: {
		London x = (London)obj;

		DB_CHECK_SIZE(x);

		(*U8_RW)(fp, &x->Useless);
	}
	break;

	case Object_Evidence: {
		Evidence x = (Evidence)obj;

		DB_CHECK_SIZE(x);

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
		Loot x = (Loot)obj;
		uint16 tmp;

		DB_CHECK_SIZE(x);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Type = (LootE)tmp;
		} else {
			tmp = x->Type;
			(*U16LE_RW)(fp, &tmp);
		}

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Name = (LootNameE)tmp;
		} else {
			tmp = x->Name;
			(*U16LE_RW)(fp, &tmp);
		}

		(*U32LE_RW)(fp, &x->Volume);
		(*U16LE_RW)(fp, &x->Weight);
		(*U16LE_RW)(fp, &x->PictID);
	}
	break;

	case Object_CompleteLoot: {
		CompleteLoot x = (CompleteLoot)obj;

		DB_CHECK_SIZE(x);

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
		LSOLock x = (LSOLock)obj;
		uint16 tmp;

		DB_CHECK_SIZE(x);

		if (RW == 0) {
			(*U16LE_RW)(fp, &tmp);
			x->Type = (LockE)tmp;
		} else {
			tmp = x->Type;
			(*U16LE_RW)(fp, &tmp);
		}
	}
	break;

	case Object_LSObject: {
		LSObject x = (LSObject)obj;

		DB_CHECK_SIZE(x);

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
		Building x = (Building)obj;
		uint16 tmp;

		DB_CHECK_SIZE(x);

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
		} else {
			tmp = x->EscapeRoute;
			(*U16LE_RW)(fp, &tmp);
		}

		(*U8_RW)(fp, &x->EscapeRouteLength);
		(*U8_RW)(fp, &x->RadioGuarding);
		(*U8_RW)(fp, &x->MaxVolume);
		(*U8_RW)(fp, &x->GuardStrength);

		(*U16LE_RW)(fp, &x->CarXPos);
		(*U16LE_RW)(fp, &x->CarYPos);

		(*U8_RW)(fp, &x->DiskId);
	}
	break;

	case Object_Police: {
		Police x = (Police)obj;

		DB_CHECK_SIZE(x);

		(*U16LE_RW)(fp, &x->PictID);
		(*U8_RW)(fp, &x->LivingID);
	}
	break;

	case Object_LSArea: {
		LSArea x = (LSArea)obj;

		DB_CHECK_SIZE(x);

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
		LSRoom x = (LSRoom)obj;

		DB_CHECK_SIZE(x);

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

static void
dbRWObject(void *obj, int RW, uint32 type, uint32 size, uint32 localSize, FILE *fp) {
	int32 start;


	start = ftell(fp);

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		dbRWProfiObject(obj, RW, type, size, localSize, fp);
	} else {
		dbRWStdObject(obj, RW, type, size, localSize, fp);
	}

	if ((uint32)(ftell(fp) - start) != size) {
		ErrorMsg(Disk_Defect, ERROR_MODULE_DATABASE, 1);
	}
}


uint8 dbLoadAllObjects(const char *fileName, uint16 diskId) {
	FILE *fh;

	if ((fh = dskOpen(fileName, "rb"))) {
		uint32 realNr = 1;
		struct dbObjectHeader objHd;

		while (!feof(fh)) {
			int ch;

			objHd.nr = 0;
			objHd.type = 0;
			objHd.size = 0;

			dskRead_U32LE(fh, &objHd.nr);
			dskRead_U32LE(fh, &objHd.type);
			dskRead_U32LE(fh, &objHd.size);

			if ((objHd.nr != (uint32) - 1) && (objHd.type != (uint32) - 1)
			        && (objHd.size != (uint32) - 1)) {
				LIST *list = NULL;
				void *obj;
				char *name = NULL;
				uint32 localSize;

				if (ObjectLoadMode) {
					if ((list = txtGoKey(OBJECTS_TXT, NULL)))   /* MOD: old version GoNextKey */
						name = NODE_NAME(LIST_HEAD(list));
				}

				localSize = dbGetMemSize(objHd.type);

				if (!
				        (obj =
				             dbNewObject(objHd.nr, objHd.type, localSize, name,
				                         realNr++))) {
					dskClose(fh);
					dbDeleteAllObjects(0L, 0L);
					return 0;
				}

				if (objHd.size != dbGetDskSize(objHd.type)) {
					DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
					         "Expected #%u of size %u got %u",
					         objHd.type, dbGetDskSize(objHd.type), objHd.size);
				}

				if (list)
					RemoveList(list);

				dbRWObject(obj, 0, objHd.type, objHd.size, localSize, fh);
			}

			if ((ch = fgetc(fh)) != EOF)
				ungetc(ch, fh);
		}

		dskClose(fh);
		return 1;
	}

	return 0;
}

uint8 dbSaveAllObjects(const char *fileName, uint32 offset, uint32 size, uint16 diskId) {
	FILE *fh;
	struct dbObject *obj;
	uint32 realNr = 1;
	uint32 dbSize = dbGetObjectCountOfDB(offset, size);

	if ((fh = dskOpen(fileName, "wb"))) {
		while (realNr <= dbSize) {
			if ((obj = dbFindRealObject(realNr++, offset, size))) {
				struct dbObjectHeader objHd;
				uint32 localSize;

				objHd.nr = obj->nr;
				objHd.type = obj->type;
				objHd.size = dbGetDskSize(obj->type);

				localSize = NODE_SIZE(obj) - sizeof(struct dbObject);

				dskWrite_U32LE(fh, &objHd.nr);
				dskWrite_U32LE(fh, &objHd.type);
				dskWrite_U32LE(fh, &objHd.size);

				dbRWObject(obj + 1, 1, objHd.type, objHd.size, localSize, fh);
			}
		}

		dskClose(fh);
		return 1;
	}

	return 0;
}

void dbDeleteAllObjects(uint32 offset, uint32 size) {
	struct dbObject *obj, *pred;
	uint8 objHashValue;

	for (objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++) {
		for (obj = (struct dbObject *) LIST_HEAD(objHash[objHashValue]);
		        NODE_SUCC(obj); obj = (struct dbObject *) NODE_SUCC(obj)) {
			if (obj->nr > offset) {
				if (size && (obj->nr > offset + size))
					continue;

				pred = (struct dbObject *) NODE_PRED(obj);
				RemNode(obj);   /* MOD: old version Remove */
				FreeNode(obj);
				obj = pred;
			}
		}
	}
}

uint32 dbGetObjectCountOfDB(uint32 offset, uint32 size) {
	uint32 count = 0;
	struct dbObject *obj;
	uint8 i;

	for (i = 0; i < OBJ_HASH_SIZE; i++) {
		for (obj = (struct dbObject *) LIST_HEAD(objHash[i]); NODE_SUCC(obj);
		        obj = (struct dbObject *) NODE_SUCC(obj)) {
			if ((obj->nr > offset) && (obj->nr < (offset + size)))
				count++;
		}
	}

	return count;
}


/* public functions - OBJECT */
void *dbNewObject(uint32 nr, uint32 type, uint32 size, char *name, uint32 realNr) {
	struct dbObject *obj;
	uint8 objHashValue = dbGetObjectHashNr(nr);

	if (!
	        (obj =
	             (struct dbObject *) CreateNode(objHash[objHashValue],
	                     sizeof(struct dbObject) + size, name)))
		return NULL;

	obj->nr = nr;
	obj->type = type;
	obj->realNr = realNr;

	return dbGetObjectKey(obj);
}

void *dbGetObject(uint32 nr) {
	struct dbObject *obj;
	uint8 objHashValue = dbGetObjectHashNr(nr);

	for (obj = (struct dbObject *) LIST_HEAD(objHash[objHashValue]);
	        NODE_SUCC(obj); obj = (struct dbObject *) NODE_SUCC(obj)) {
		if (obj->nr == nr)
			return dbGetObjectKey(obj);
	}

	return NULL;
}

uint32 dbGetObjectNr(void *key) {
	return dbGetObjectReal(key)->nr;
}

char *dbGetObjectName(uint32 nr, char *objName) {
	struct dbObject *obj;
	uint8 objHashValue = dbGetObjectHashNr(nr);

	for (obj = (struct dbObject *) LIST_HEAD(objHash[objHashValue]);
	        NODE_SUCC(obj); obj = (struct dbObject *) NODE_SUCC(obj)) {
		if (obj->nr == nr) {
			strcpy(objName, NODE_NAME(obj));
			return objName;
		}
	}

	return NULL;
}

void *dbIsObject(uint32 nr, uint32 type) {
	struct dbObject *obj;
	uint8 objHashValue = dbGetObjectHashNr(nr);

	for (obj = (struct dbObject *) LIST_HEAD(objHash[objHashValue]);
	        NODE_SUCC(obj); obj = (struct dbObject *) NODE_SUCC(obj)) {
		if (obj->nr == nr) {
			if (obj->type == type)
				return dbGetObjectKey(obj);

			break;
		}
	}

	return NULL;
}

/* public prototypes - OBJECTNODE */
struct ObjectNode *dbAddObjectNode(LIST *objectList, uint32 nr, uint32 flags) {
	struct ObjectNode *n = NULL;
	struct dbObject *obj = dbGetObjectReal(dbGetObject(nr));
	char name[TXT_KEY_LENGTH], *namePtr;

	name[0] = EOS;

	if (flags & OLF_INSERT_STAR)
		strcpy(name, "*");

	if (flags & OLF_INCLUDE_NAME) {
		char *succString = NULL;

		if ((flags & OLF_ADD_PREV_STRING) && ObjectListPrevString)
			strcat(name,
			       ObjectListPrevString(obj->nr, obj->type,
			                            dbGetObjectKey(obj)));

		strcat(name, NODE_NAME(obj));
		namePtr = name;

		if ((flags & OLF_ADD_SUCC_STRING) && ObjectListSuccString)
			succString =
			    ObjectListSuccString(obj->nr, obj->type, dbGetObjectKey(obj));

		if ((flags & (OLF_ADD_SUCC_STRING | OLF_ALIGNED)) && ObjectListWidth) {
			uint8 i, len = strlen(name) + strlen(succString);

			if (flags & OLF_INSERT_STAR)
				len--;

			for (i = len; i < ObjectListWidth; i++)
				strcat(name, " ");
		}

		if ((flags & OLF_ADD_SUCC_STRING) && ObjectListSuccString)
			strcat(name, succString);
	} else
		namePtr = NULL;

	if ((n =
	            (struct ObjectNode *) CreateNode(objectList, sizeof(*n), namePtr))) {
		n->nr = obj->nr;
		n->type = obj->type;
		n->data = dbGetObjectKey(obj);
	}

	return n;
}

void dbRemObjectNode(LIST *objectList, uint32 nr) {
	struct ObjectNode *n = dbHasObjectNode(objectList, nr);

	if (n) {
		RemNode(n);
		FreeNode(n);
	}
}

struct ObjectNode *dbHasObjectNode(LIST *objectList, uint32 nr) {
	struct ObjectNode *n;

	for (n = (struct ObjectNode *) LIST_HEAD(objectList); NODE_SUCC(n);
	        n = (struct ObjectNode *) NODE_SUCC(n)) {
		if (OL_NR(n) == nr)
			return n;
	}

	return NULL;
}

void SetObjectListAttr(uint32 flags, uint32 type) {
	ObjectListType = type;
	ObjectListFlags = flags;

	if (ObjectListFlags & OLF_PRIVATE_LIST)
		ObjectListPrivate = CreateList();
	else {
		RemoveList(ObjectList);
		ObjectList = CreateList();
	}
}

void BuildObjectList(void *key) {
	struct dbObject *obj = dbGetObjectReal(key);

	if (!ObjectListType || (obj->type == ObjectListType)) {
		LIST *list;
		if (ObjectListFlags & OLF_PRIVATE_LIST)
			list = ObjectListPrivate;
		else
			list = ObjectList;

		dbAddObjectNode(list, obj->nr, ObjectListFlags);
	}
}

void ExpandObjectList(LIST *objectList, char *expandItem) {
	struct ObjectNode *objNode;

	if (!
	        (objNode =
	             (struct ObjectNode *) CreateNode(objectList, sizeof(*objNode),
	                     expandItem)))
		ErrorMsg(Internal_Error, ERROR_MODULE_DATABASE, 2);

	objNode->nr = 0;
	objNode->type = 0;
	objNode->data = NULL;
}


int16 dbStdCompareObjects(struct ObjectNode *obj1, struct ObjectNode *obj2) {
	if (obj1->nr > obj2->nr)
		return -1;

	if (obj1->nr < obj2->nr)
		return 1;

	return 0;
}

void dbSortPartOfList(LIST *l, struct ObjectNode *start,
                      struct ObjectNode *end,
                      int16(*processNode)(struct ObjectNode *,
                              struct ObjectNode *)) {
	LIST *newList = CreateList();
	struct ObjectNode *n, *n1, *startPred;
	int32 i, j;

	if (start == (struct ObjectNode *) LIST_HEAD(l))
		startPred = 0L;
	else
		startPred = (struct ObjectNode *) NODE_PRED(start);

	for (n = start, i = 1; n != end;
	        n = (struct ObjectNode *) NODE_SUCC(n), i++);

	n = start;

	for (j = 0; j < i; j++) {
		n1 = n;
		n = (struct ObjectNode *) NODE_SUCC(n);

		RemNode(n1);
		AddHeadNode(newList, n1);
	}

	dbSortObjectList(&newList, processNode);

	for (n = (struct ObjectNode *) LIST_HEAD(newList); NODE_SUCC(n);) {
		n1 = n;
		n = (struct ObjectNode *) NODE_SUCC(n);

		RemNode(n1);
		AddNode(l, n1, startPred);

		startPred = n1;
	}

	RemoveList(newList);
}

int32 dbSortObjectList(LIST **objectList,
                       int16(*processNode)(struct ObjectNode *,
                               struct ObjectNode *)) {
	LIST *newList;
	struct ObjectNode *n1, *n2;
	int32 i = 0;

	if (!LIST_EMPTY(*objectList)) {
		newList = CreateList();

		for (n1 = (struct ObjectNode *) LIST_HEAD(*objectList); NODE_SUCC(n1);
		        n1 = (struct ObjectNode *) NODE_SUCC(n1), i++) {
			struct ObjectNode *pred = 0;

			if (!LIST_EMPTY(newList)) {
				for (n2 = (struct ObjectNode *) LIST_HEAD(newList);
				        !pred && NODE_SUCC(n2);
				        n2 = (struct ObjectNode *) NODE_SUCC(n2)) {
					if (processNode(n1, n2) >= 0)
						pred = n2;
				}
			}

			struct ObjectNode *newNode =
			    (struct ObjectNode *) CreateNode(NULL, sizeof(*newNode),
			                                     NODE_NAME(n1));
			newNode->nr = n1->nr;
			newNode->type = n1->type;
			newNode->data = n1->data;

			if (pred) {
				if (pred == (struct ObjectNode *) LIST_HEAD(newList))
					AddHeadNode(newList, newNode);
				else
					AddNode(newList, newNode, NODE_PRED(pred));
			} else
				AddTailNode(newList, newNode);
		}

		if (!LIST_EMPTY(newList)) {
			RemoveList(*objectList);
			*objectList = newList;
		} else
			RemoveList(newList);
	}

	return i;
}


/* public prototypes */
void dbInit(void) {
	uint8 objHashValue;

	if (!(ObjectList = CreateList()))
		ErrorMsg(No_Mem, ERROR_MODULE_DATABASE, 3);

	for (objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++) {
		if (!(objHash[objHashValue] = CreateList()))
			ErrorMsg(No_Mem, ERROR_MODULE_DATABASE, 4);
	}

	CompareKey = dbCompare;
	EncodeKey = dbEncode;
	DecodeKey = dbDecode;
}

void dbDone(void) {
	for (uint8 objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++) {
		if (objHash[objHashValue])
			RemoveList(objHash[objHashValue]);
	}

	if (ObjectList)
		RemoveList(ObjectList);
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
	case _Ability_Schloesser:
		return  141;
	case _Ability_Sprengstoff:
		return  142;
	case _Ability_Safes:
		return  143;
	case _Ability_Elektronik:
		return  144;
	case _Ability_Aufpassen:
		return  145;
	case _Ability_Kampf:
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
	case _Ability_Schloesser:
		return  174;
	case _Ability_Sprengstoff:
		return  175;
	case _Ability_Safes:
		return  176;
	case _Ability_Elektronik:
		return  177;
	case _Ability_Aufpassen:
		return  178;
	case _Ability_Kampf:
		return  179;

	default:
		DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
			"Unknown trouble key #%u", key);
		return 0;
	}
}

uint32 getKey(KeyConflictE key) {
	if (g_clue->getFeatures() & GF_PROFIDISK) {
		return getKeyProfi(key);
	} else {
		return getKeyStd(key);
	}
}

#if 0
void dbDeleteObject(uint32 nr) {
	struct dbObject *obj = NULL;
	uint8 objHashValue = dbGetObjectHashNr(nr);

	for (obj = (struct dbObject *) LIST_HEAD(objHash[objHashValue]);
		NODE_SUCC(obj); obj = (struct dbObject *) NODE_SUCC(obj)) {
			if (obj->nr == nr) {
				UnSetAll((KEY)(obj + 1), NULL);
				RemNode(obj);
				FreeNode(obj);
				return;
			}
	}
}
#endif

} // End of namespace Clue

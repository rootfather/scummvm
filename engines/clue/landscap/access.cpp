/*
**  $Filename: landscap/access.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-04-94
**
**  landscap access functions for "Der Clou!"
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

#include "clue/base/base.h"
#include "clue/landscap/landscap_p.h"
#include "clue/clue.h"

namespace Clue {

bool lsIsLSObjectInActivArea(LSObjectNode *lso) {
	if (AskP(dbGetObject(ls->ul_AreaID), ConsistOfRelationID, lso, NO_PARAMETER, CMP_NO))
		return true;

	return false;
}

void lsSetObjectRetrievalList(uint32 ul_AreaId) {
	for (int32 i = 0; i < 3; i++) {
		if (ul_AreaId == ls->ul_ObjectRetrievalAreaId[i])
			ls->p_ObjectRetrieval = ls->p_ObjectRetrievalLists[i];
	}
}

uint32 lsGetCurrObjectRetrieval() {
	for (int32 i = 0; i < 3; i++) {
		if (ls->p_ObjectRetrieval == ls->p_ObjectRetrievalLists[i])
			return ls->ul_ObjectRetrievalAreaId[i];
	}

	return 0;
}

bool lsIsObjectAStdObj(LSObjectNode *lso) {
	if (lsIsObjectADoor(lso))
		return false;

	if (lsIsObjectAWall(lso))
		return false;

	if (lsIsObjectAnAddOn(lso))
		return false;

	if (lsIsObjectSpecial(lso))
		return false;

	return true;
}

bool lsIsObjectADoor(LSObjectNode *lso) {
	switch (lso->Type) {
	case Item_Wooden_door:
	case Item_Steel_door:
	case Item_Wall_gate:
	case Item_Vault:
		return true;
	}

	return false;
}

bool lsIsObjectAWall(LSObjectNode *lso) {
	switch (lso->Type) {
	case Item_Wall:
	case Item_Wall_corner:
	case Item_Stone_wall:
	case Item_Pedestal:
		return true;
	}

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		switch (lso->Type) {
		case Item_Decorated_column:
		case Item_Railing:
		case Item_Barrier:
		case Item_Mailbag:
		case Item_Locomotive_front_right:
		case Item_Locomotive_front_left:
		case Item_Locomotive_above:
		case Item_Locomotive_side:
		case Item_Locomotive_cabin:
		case Item_Locomotive_door:
			return true;
		}
	}

	return false;
}

/* objects which need the same refresh as doors */
bool lsIsObjectSpecial(LSObjectNode *lso) {
	if (g_clue->getFeatures() & GF_PROFIDISK) {
		switch (lso->Type) {
		case Item_Statue_of_saint:
		case Item_African_figure:
		case Item_Batman_figure:
		case Item_Fat_man:
		case Item_Unknown:
		case Item_Jack_the_Ripper_figure:
		case Item_King_figure:
		case Item_Guard_figure:
		case Item_Miss_World_1952:
			return true;
		}
	}

	return false;
}


bool lsIsObjectAnAddOn(LSObjectNode *lso) {
	switch (lso->Type) {
	case Item_Cash_register:
	case Item_Vase:
	case Item_Statue:
	case Item_Cross:
	case Item_Chaplet:
		return true;
	}

	return false;
}

uint16 lsGetObjectCount() {
	return (uint16)ls->p_ObjectRetrieval->getNrOfNodes();
}

byte lsGetLoudness(uint16 x, uint16 y) {
	int16 floorIndex = lsGetFloorIndex(x, y);

	/* Ursprünglich wurde loudness hier mit MaxVolume initialisiert    */
	/* dadurch waren in der Anzeige der Loudness auch die Nachbarn     */
	/* inbegriffen waren. Nebenwirkung: Es gab aber plötzlich, sobald  */
	/* man die Lautstärke überschritt Alarm durch Mikrophone ->        */
	/* tcAlarmByMicro, bei einer Änderung der Initialiserung, muß auch */
	/* TcAlarmByMicro geändert werden.   (hg, 14-02-94)                */
	byte loudness = 255;

	if ((!LS_NO_FLOOR(ls->p_CurrFloor[floorIndex].uch_FloorType)) &&
	        LS_IS_MICRO_ON_FLOOR(ls->p_CurrFloor[floorIndex].uch_FloorType))
		loudness = 15;
	else {
		for (int16 i = -1; (i < 2) && (loudness == 255); i++) {
			for (int16 j = -1; (j < 2) && (loudness == 255); j++) {
				int16 k = floorIndex + i * LS_FLOORS_PER_LINE + j;

				if ((k >= 0) && (k < LS_FLOORS_PER_LINE * LS_FLOORS_PER_COLUMN)) {
					if ((!LS_NO_FLOOR(ls->p_CurrFloor[k].uch_FloorType)) &&
					        LS_IS_MICRO_ON_FLOOR(ls->p_CurrFloor[k].uch_FloorType))
						loudness = 50;
				}
			}
		}
	}

	return loudness;
}

uint32 lsGetObjectState(uint32 objID) {
	LSObjectNode *obj = (LSObjectNode *)dbGetObject(objID);

	return lsGetNewState(obj);
}

uint32 lsGetStartArea() {
	startsWithAll(ls->ul_BuildingID, OLF_NORMAL, Object_LSArea);

	/* attention, planing has to be changed to! */
	uint32 areaID = ObjectList->getListHead()->_nr;

	return areaID;
}

uint16 lsGetFloorIndex(uint16 x, uint16 y) {
	uint16 fpl = LS_FLOORS_PER_LINE;

	uint16 row = x / LS_FLOOR_X_SIZE;
	uint16 line = y / LS_FLOOR_Y_SIZE;

	return (uint16)(line * fpl + row);
}

static void lsExtendGetList(NewObjectList<dbObjectNode> * list, uint32 nr, uint32 type, dbObjectNode *data) {
	dbAddObjectNode(list, type, OLF_INCLUDE_NAME | OLF_INSERT_STAR);
	dbObjectNode* newNode = list->getListTail();

	newNode->_nr = nr;
	newNode->_type = type;
	warning("HACK - lsExtendGetList");
	newNode->_fakePtr = data->_fakePtr;
	return;
	dbObjectMapper(newNode, data);
}

NewObjectList<dbObjectNode> *lsGetObjectsByList(uint16 x, uint16 y, uint16 width, uint16 height,
                         byte showInvisible, bool addLootBags) {
	NewObjectList<dbObjectNode> *list = new NewObjectList<dbObjectNode>;

	/* diverse Objekte eintragen */
	/* enter various objects */
	for (dbObjectNode* node = ls->p_ObjectRetrieval->getListHead(); node->_succ; node = (dbObjectNode *) node->_succ) {
		LSObjectNode *lso = (LSObjectNode *) node;

		if ((lso->ul_Status & (1 << Const_tcACCESS_BIT)) || (_gamePlayMode & GP_MODE_LEVEL_DESIGN))
			if (showInvisible || lso->uch_Visible)
				if (lsIsInside(lso, x, y, x + width, y + height))
					lsExtendGetList(list, node->_nr, lso->Type, lso);
	}

	/* Ausnahme: Beutesack eintragen! */
	/* Exception: enter the loot bag! */
	if (addLootBags) {
		for (uint32 i = 9701; i <= 9708; i++) {
			LSObjectNode *lso = (LSObjectNode *)dbGetObject(i);

			if (lso->uch_Visible == LS_OBJECT_VISIBLE)
				if (lsIsInside(lso, x, y, x + width, y + height))
					lsExtendGetList(list, i, Item_Loot_bag, lso);
		}
	}

	return list;
}

void lsSetCollMode(byte collMode) {
	ls->uch_CollMode = collMode;
}

uint32 lsGetCurrBuildingID() {
	return ls->ul_BuildingID;
}

uint32 lsGetActivAreaID() {
	return (ls->ul_AreaID);
}

NewObjectList<dbObjectNode> *lsGetRoomsOfArea(uint32 ul_AreaId) {
	LSAreaNode *area = (LSAreaNode *) dbGetObject(ul_AreaId);
	uint32 roomRelId = area->ul_ObjectBaseNr + REL_HAS_ROOM_OFFSET;

	SetObjectListAttr(OLF_PRIVATE_LIST, Object_LSRoom);
	AskAll(area, roomRelId, BuildObjectList);


	for (dbObjectNode* room = ObjectListPrivate->getListHead(); room->_succ; room = (dbObjectNode*)room->_succ) {
		LSRoomNode *myroom = (LSRoomNode *)room;

		if ((int16) myroom->us_LeftEdge < 0)
			myroom->us_LeftEdge = 0;

		if ((int16) myroom->us_TopEdge < 0)
			myroom->us_TopEdge = 0;
	}

	return ObjectListPrivate;
}

#if 0
uint16 lsGetWindowXPos() {
	return (ls->us_WindowXPos);
}

uint16 lsGetWindowYPos() {
	return (ls->us_WindowYPos);
}

uint16 lsGetTotalXPos() {
	return (uint16)(ls->us_WindowXPos + ls->us_PersonXPos);
}

uint16 lsGetTotalYPos() {
	return (uint16)(ls->us_WindowYPos + ls->us_PersonYPos);
}
#endif 

} // End of namespace Clue

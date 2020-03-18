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

bool lsIsLSObjectInActivArea(LSObject lso) {
	if (AskP
	        (dbGetObject(ls->ul_AreaID), ConsistOfRelationID, lso, NO_PARAMETER,
	         CMP_NO))
		return true;

	return false;
}

void lsSetObjectRetrievalList(uint32 ul_AreaId) {
	int32 i;

	for (i = 0; i < 3; i++)
		if (ul_AreaId == ls->ul_ObjectRetrievalAreaId[i])
			ls->p_ObjectRetrieval = ls->p_ObjectRetrievalLists[i];
}

uint32 lsGetCurrObjectRetrieval() {
	int32 i;

	for (i = 0; i < 3; i++)
		if (ls->p_ObjectRetrieval == ls->p_ObjectRetrievalLists[i])
			return ls->ul_ObjectRetrievalAreaId[i];

	return 0;
}

bool lsIsObjectAStdObj(LSObject lso) {
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

bool lsIsObjectADoor(LSObject lso) {
	switch (lso->Type) {
	case Item_Holztuer:
	case Item_Stahltuer:
	case Item_Mauertor:
	case Item_Tresorraum:
		return true;
	}

	return false;
}

bool lsIsObjectAWall(LSObject lso) {
	switch (lso->Type) {
	case Item_Mauer:
	case Item_Mauerecke:
	case Item_Steinmauer:
	case Item_Sockel:
		return true;
	}

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		switch (lso->Type) {
		case Item_verzierte_Saeule:
		case Item_Gelaender:
		case Item_Absperrung:
		case Item_Postsack:
		case Item_Lokomotive_vorne_rechts:
		case Item_Lokomotive_vorne_links:
		case Item_Lokomotive_oben:
		case Item_Lokomotive_seitlich:
		case Item_Lokomotive_Kabine:
		case Item_Lokomotive_Tuer:
			return true;
		}
	}

	return false;
}

/* objects which need the same refresh as doors */
bool lsIsObjectSpecial(LSObject lso) {
	if (g_clue->getFeatures() & GF_PROFIDISK) {
		switch (lso->Type) {
		case Item_Heiligenstatue:
		case Item_Hottentotten_Figur:
		case Item_Batman_Figur:
		case Item_Dicker_Man:
		case Item_Unbekannter:
		case Item_Jack_the_Ripper_Figur:
		case Item_Koenigs_Figur:
		case Item_Wache_Figur:
		case Item_Miss_World_1952:
			return true;
		}
	}

	return false;
}


bool lsIsObjectAnAddOn(LSObject lso) {
	switch (lso->Type) {
	case Item_Kasse:
	case Item_Vase:
	case Item_Statue:
	case Item_Kreuz:
	case Item_Kranz:
		return true;
	}

	return false;
}

uint16 lsGetObjectCount() {
	return ((uint16) GetNrOfNodes(ls->p_ObjectRetrieval));
}

byte lsGetLoudness(uint16 x, uint16 y) {
	int16 floorIndex = lsGetFloorIndex(x, y), i, j, k;

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
		for (i = -1; (i < 2) && (loudness == 255); i++) {
			for (j = -1; (j < 2) && (loudness == 255); j++) {
				k = floorIndex + i * LS_FLOORS_PER_LINE + j;

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
	LSObject obj = (LSObject)dbGetObject(objID);

	return (lsGetNewState(obj));
}

uint32 lsGetStartArea() {
	uint32 areaID;          /* attention, planing has to be changed to! */

	startsWithAll(ls->ul_BuildingID, OLF_NORMAL, Object_LSArea);

	areaID = OL_NR(LIST_HEAD(ObjectList));

	return (areaID);
}

uint16 lsGetFloorIndex(uint16 x, uint16 y) {
	uint16 fpl, row, line;

	fpl = LS_FLOORS_PER_LINE;

	row = x / LS_FLOOR_X_SIZE;
	line = y / LS_FLOOR_Y_SIZE;

	return (uint16)(line * fpl + row);
}

static void lsExtendGetList(LIST *list, uint32 nr, uint32 type, void *data) {
	struct ObjectNode *newNode =
	    dbAddObjectNode(list, type, OLF_INCLUDE_NAME | OLF_INSERT_STAR);

	newNode->nr = nr;
	newNode->type = type;
	newNode->data = data;
}

LIST *lsGetObjectsByList(uint16 x, uint16 y, uint16 width, uint16 height,
                         byte showInvisible, byte addLootBags) {
	LIST *list = CreateList();

	/* diverse Objekte eintragen */
	for (ObjectNode* node = (ObjectNode*)LIST_HEAD(ls->p_ObjectRetrieval);
	        NODE_SUCC((NODE *) node);
	        node = (ObjectNode *) NODE_SUCC((NODE *) node)) {
		LSObject lso = (LSObject) OL_DATA(node);

		if ((lso->ul_Status & (1L << Const_tcACCESS_BIT))
		        || (GamePlayMode & GP_LEVEL_DESIGN))
			if (showInvisible || lso->uch_Visible)
				if (lsIsInside(lso, x, y, x + width, y + height))
					lsExtendGetList(list, OL_NR(node), lso->Type, lso);
	}

	/* Ausnahme: Beutesack eintragen! */
	if (addLootBags) {
		for (uint32 i = 9701; i <= 9708; i++) {
			LSObject lso = (LSObject)dbGetObject(i);

			if (lso->uch_Visible == LS_OBJECT_VISIBLE)
				if (lsIsInside(lso, x, y, x + width, y + height))
					lsExtendGetList(list, i, Item_Beutesack, lso);
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

LIST *lsGetRoomsOfArea(uint32 ul_AreaId) {
	LSArea area = (LSArea) dbGetObject(ul_AreaId);
	uint32 roomRelId = area->ul_ObjectBaseNr + REL_HAS_ROOM_OFFSET;
	NODE *room;

	SetObjectListAttr(OLF_PRIVATE_LIST, Object_LSRoom);
	AskAll(area, roomRelId, BuildObjectList);


	for (room = LIST_HEAD(ObjectListPrivate); NODE_SUCC(room); room = NODE_SUCC(room)) {
		LSRoom myroom = (LSRoom)OL_DATA(room);

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

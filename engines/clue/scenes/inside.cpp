/*
**  $Filename: scenes/inside.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-02-94
**
**   functions for inside of houses for "Der Clou!"
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

#include "clue/scenes/scenes.h"

namespace Clue {

uint32 CurrAreaId;
LSObject CurrLSO;       /* for FadeObjectInside */

static void FadeInsideObject() {
	static byte status = 1;

	lsFadeRasterObject(CurrAreaId, CurrLSO, (status++) % 2);
}

uint32 tcGoInsideOfHouse(uint32 buildingID) {
	List *menu = g_clue->_txtMgr->goKey(MENU_TXT, "INSIDE_MENU"), *areas;
	uint32 areaID = 0;

	consistsOfAll(buildingID, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME,
	              Object_LSArea);
	areas = ObjectListPrivate;
	dbSortObjectList(&areas, dbStdCompareObjects);

	uint32 count = GetNrOfNodes(areas) + 1;

	if (count > 2) {
		uint32 i;
		for (i = count; i < 5; i++)
			RemoveNode(menu, NODE_NAME(GetNthNode(menu, count)));

		ShowMenuBackground();

		inpTurnFunctionKey(0);
		inpTurnESC(0);
		i = (uint32) Menu(menu, (1L << count) - 1, 0, 0L, 0L);
		inpTurnESC(1);
		inpTurnFunctionKey(1);

		consistsOfAll(buildingID, OLF_NORMAL, Object_LSArea);

		if (i)
			areaID = OL_NR(GetNthNode(areas, i - 1));
	}

	ShowMenuBackground();
	ShowTime(0);

	RemoveList(areas);
	RemoveList(menu);

	return (areaID);
}

void tcInsideOfHouse(uint32 buildingID, uint32 areaID, byte perc) {
	LSArea area = (LSArea)dbGetObject(areaID);
	List *menu = g_clue->_txtMgr->goKey(MENU_TXT, "LookMenu");

	Common::String alarm = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "PROTECTED");
	Common::String power = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "SUPPLIED");

	inpTurnESC(0);
	inpTurnFunctionKey(0);

	/* liste und node initialisieren */
	SetObjectListAttr(OLF_PRIVATE_LIST, Object_LSObject);
	AskAll(area, ConsistOfRelationID, BuildObjectList);
	List *objects = ObjectListPrivate;

	/*lsSortObjectList(&objects);*/

	uint32 count = (GetNrOfNodes(objects) * perc) / 255;
	Node *node = lsGetSuccObject(LIST_HEAD(objects));

	CurrAreaId = areaID;

	if ((GetNodeNrByAddr(objects, node)) > count) {
		SetBubbleType(THINK_BUBBLE);
		Say(BUSINESS_TXT, 0, MATT_PICTID, "CANT_LOOK");
	} else {
		uint32 action = 0;
		while (action != 4) {
			LSObject lso = (LSObject) OL_DATA(node);
			Common::String name = dbGetObjectName(lso->Type);

			if (lso->uch_Chained) {
				name += "(";
				if (lso->uch_Chained & Const_tcCHAINED_TO_ALARM) {
					name += alarm.c_str();
					if (lso->uch_Chained & Const_tcCHAINED_TO_POWER) {
						name += ", ";
						name += power;
					}
				}
				else if (lso->uch_Chained & Const_tcCHAINED_TO_POWER)
					name += power;

				name += ")";
			}
// TODO : use console
#ifdef THECLOU_DEBUG
			{
				char debugtxt[TXT_KEY_LENGTH];

				sprintf(debugtxt, " ObjNr=%d", OL_NR(node));
				strcat(name, debugtxt);
			}
#endif

			ShowMenuBackground();
			PrintStatus(name);

			CurrLSO = lso;

			SetMenuTimeOutFunc(FadeInsideObject);

			inpTurnFunctionKey(false);
			inpTurnESC(false);
			action = Menu(menu, 31, (byte) action, nullptr, 10);
			inpTurnESC(true);
			inpTurnFunctionKey(true);

			lsFadeRasterObject(areaID, lso, 1);

			switch (action) {
			case 0: {
				Node *n = lsGetSuccObject(node);
				if (GetNodeNrByAddr(objects, n) < (count - 1))
					node = n;
				}
				break;
			case 1:
				node = lsGetPredObject(node);
				break;
			case 2:
				tcShowObjectData(areaID, node, perc);
				break;
			case 3:
				isGuardedbyAll(buildingID, OLF_NORMAL, Object_Police);
				if (!(LIST_EMPTY(ObjectList))) {
					gfxSetPens(l_gc, 4, GFX_SAME_PEN, GFX_SAME_PEN);
					grdDraw(l_gc, buildingID, areaID);
				} else
					Say(BUSINESS_TXT, 0, MATT_PICTID, "NO_GUARD");
				break;
			case 4:
				break;
			default:
				action = 0;
				break;
			}

			lso = (LSObject) OL_DATA(node);
			lsFadeRasterObject(areaID, lso, 1);
		}
	}

	RemoveList(menu);
	RemoveList(objects);
	inpSetWaitTicks(0);
}

void tcShowObjectData(uint32 areaID, Node *node, byte perc) {
	/* Objekt selbst präsentieren */
	Present(OL_NR(node), "RasterObject", InitObjectPresent);

	/* Inhalt zusammenstellen */
	SetObjectListAttr(OLF_NORMAL, Object_Loot);
	AskAll(OL_DATA(node), hasLootRelationID, BuildObjectList);

	if (!LIST_EMPTY(ObjectList)) {
		/* alle Loots durchgehen und anzeigen! */
		for (Node *n = LIST_HEAD(ObjectList); NODE_SUCC(n); n = NODE_SUCC(n)) {
			/* zur Variablenübergabe... (DIRTY, DIRTY...) */
			SetP(OL_DATA(n), hasLootRelationID, OL_DATA(n),
			     GetP(OL_DATA(node), hasLootRelationID, OL_DATA(n)));

			Present(OL_NR(n), "RasterObject", InitOneLootPresent);

			/* kill these dirty things! */
			UnSet(OL_DATA(n), hasLootRelationID, OL_DATA(n));
		}
	}

	/* Vebindungen zeigen! */
	lsShowAllConnections(areaID, node, perc);
}

} // End of namespace Clue

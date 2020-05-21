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
#include "clue/present/interac.h"
#include "clue/planing/player.h"


namespace Clue {

uint32 CurrAreaId;
LSObjectNode *CurrLSO;       /* for FadeObjectInside */

static void FadeInsideObject() {
	static byte status = 1;

	lsFadeRasterObject(CurrAreaId, CurrLSO, (status++) % 2);
}

uint32 tcGoInsideOfHouse(uint32 buildingID) {
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "INSIDE_MENU");
	uint32 areaID = 0;

	consistsOfAll(buildingID, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME, Object_LSArea);
	NewObjectList<dbObjectNode> *areas = ObjectListPrivate;
	dbSortObjectList(&areas, dbStdCompareObjects);

	uint32 count = areas->getNrOfNodes() + 1;

	if (count > 2) {
		uint32 i;
		for (i = count; i < 5; i++)
			menu->removeNode(menu->getNthNode(count)->_name);

		ShowMenuBackground();

		inpTurnFunctionKey(false);
		inpTurnESC(false);
		i = (uint32) Menu(menu, (1 << count) - 1, 0, nullptr, 0);
		inpTurnESC(true);
		inpTurnFunctionKey(true);

		consistsOfAll(buildingID, OLF_NORMAL, Object_LSArea);

		if (i)
			areaID = areas->getNthNode(i - 1)->_nr;
	}

	ShowMenuBackground();
	ShowTime(0);

	areas->removeList();
	menu->removeList();

	return areaID;
}

void tcInsideOfHouse(uint32 buildingID, uint32 areaID, byte perc) {
	LSAreaNode *area = (LSAreaNode *)dbGetObject(areaID);
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "LookMenu");

	Common::String alarm = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "PROTECTED");
	Common::String power = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "SUPPLIED");

	inpTurnESC(false);
	inpTurnFunctionKey(false);

	/* liste und node initialisieren */
	SetObjectListAttr(OLF_PRIVATE_LIST, Object_LSObject);
	AskAll(area, ConsistOfRelationID, BuildObjectList);
	NewObjectList<dbObjectNode> *objects = ObjectListPrivate;

	/*lsSortObjectList(&objects);*/

	uint32 count = (objects->getNrOfNodes() * perc) / 255;
	dbObjectNode *node = lsGetSuccObject(objects->getListHead());

	CurrAreaId = areaID;

	if (objects->getNodeNrByAddr(node) > count) {
		SetBubbleType(THINK_BUBBLE);
		Say(BUSINESS_TXT, 0, MATT_PICTID, "CANT_LOOK");
	} else {
		uint32 action = 0;
		while (action != 4) {
			LSObjectNode *lso = (LSObjectNode *)node;
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
				dbObjectNode *n = lsGetSuccObject(node);
				if (objects->getNodeNrByAddr(n) < (count - 1))
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
				if (!ObjectList->isEmpty()) {
					_lowerGc->setPens(4, GFX_SAME_PEN, GFX_SAME_PEN);
					grdDraw(_lowerGc, buildingID, areaID);
				} else
					Say(BUSINESS_TXT, 0, MATT_PICTID, "NO_GUARD");
				break;
			case 4:
				break;
			default:
				action = 0;
				break;
			}

			lso = (LSObjectNode *) node;
			lsFadeRasterObject(areaID, lso, 1);
		}
	}

	menu->removeList();
	objects->removeList();
	inpSetWaitTicks(0);
}

void tcShowObjectData(uint32 areaID, dbObjectNode *node, byte perc) {
	/* Objekt selbst präsentieren */
	Present(node->_nr, "RasterObject", InitObjectPresent);

	/* Inhalt zusammenstellen */
	SetObjectListAttr(OLF_NORMAL, Object_Loot);
	AskAll(node, hasLootRelationID, BuildObjectList);

	if (!ObjectList->isEmpty()) {
		/* alle Loots durchgehen und anzeigen! */
		for (dbObjectNode *n = ObjectList->getListHead(); n->_succ; n = (dbObjectNode *)n->_succ) {
			/* zur Variablenübergabe... (DIRTY, DIRTY...) */
			SetP(n, hasLootRelationID, n, GetP(node, hasLootRelationID, n));

			Present(n->_nr, "RasterObject", InitOneLootPresent);

			/* kill these dirty things! */
			UnSet(n, hasLootRelationID, n);
		}
	}

	/* Vebindungen zeigen! */
	lsShowAllConnections(areaID, node, perc);
}

} // End of namespace Clue

/*
**  $Filename: landscap/landscap.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-04-94
**
**  landscap functions for "Der Clou!"
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
#include "clue/living/bob.h"
#include "clue/data/dataappl.h"

namespace Clue {

uint32 ConsistOfRelationID = 0;
uint32 hasLockRelationID = 0;
uint32 hasAlarmRelationID = 0;
uint32 hasPowerRelationID = 0;
uint32 hasLootRelationID = 0;
uint32 hasRoomRelationID = 0;
uint32 FloorLinkRelationID = 0;

struct LandScape *ls = nullptr;

void lsSafeRectFill(uint16 x0, uint16 y0, uint16 x1, uint16 y1, uint8 color) {
	x0 = MIN(x0, (uint16)638);
	x1 = MIN(x1, (uint16)639);

	y0 = MIN(y0, (uint16)254);
	y1 = MIN(y1, (uint16)255);

	if (x1 > x0 && y1 > y0)
		gfxLSRectFill(x0, y0, x1, y1, color);
}

void lsSetVisibleWindow(uint16 x, uint16 y) {
	int16 halfX = LS_VISIBLE_X_SIZE / 2;
	int16 halfY = LS_VISIBLE_Y_SIZE / 2;

	int16 wX = x - halfX;
	if (wX < 0)
		wX = 0;

	int16 wY = y - halfY;
	if (wY < 0)
		wY = 0;

	/* -1 is important, so that no position 320, 128 is possible     */
	/* (window from (320, 128) - (640, 256) instead of (639, 255) !!! */
	wX = MIN(wX, int16(LS_MAX_AREA_WIDTH - LS_VISIBLE_X_SIZE - 1));
	wY = MIN(wY, int16(LS_MAX_AREA_HEIGHT - LS_VISIBLE_Y_SIZE - 1));

	ls->us_WindowXPos = (uint16) wX;
	ls->us_WindowYPos = (uint16) wY;

	gfxNCH4SetViewPort(ls->us_WindowXPos, ls->us_WindowYPos);

	livSetVisLScape(ls->us_WindowXPos, ls->us_WindowYPos);
}

void lsBuildScrollWindow() {
	int32 i;
	dbObjectNode *node;
	LSAreaNode *area = (LSAreaNode *) dbGetObject(ls->ul_AreaID);
	uint8 palette[GFX_PALETTE_SIZE];

	gfxSetColorRange(0, 255);
	gfxChangeColors(_upperGc, 0, GFX_FADE_OUT, 0);

	/* Boden aufbauen */
	for (i = 0; i < LS_FLOORS_PER_COLUMN; i++) {
		for (int32 j = 0; j < LS_FLOORS_PER_LINE; j++) {
			/* if no floor is available fill with collision colour */

			if (LS_NO_FLOOR((ls->p_CurrFloor[i * LS_FLOORS_PER_LINE + j].uch_FloorType)))
				lsSafeRectFill(j * 32, i * 32, j * 32 + 31, i * 32 + 31, LS_COLLIS_COLOR_2);
			else
				lsBlitFloor(i * LS_FLOORS_PER_LINE + j, j * 32, i * 32);
		}
	}

	/* Objekte setzen - zuerst Wände */
	for (node = ls->p_ObjectRetrieval->getListHead(); node->_succ; node = (dbObjectNode *)node->_succ) {
		LSObjectNode *lso = (LSObjectNode *)node;

		if (lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_WALL))
			lsTurnObject(lso, lso->uch_Visible, LS_COLLISION);

	}

	/* dann andere */
	for (node = ls->p_ObjectRetrieval->getListHead(); node->_succ; node = (dbObjectNode*)node->_succ) {
		LSObjectNode *lso = (LSObjectNode *)node;

		if (lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_OTHER_0))
			lsTurnObject(lso, lso->uch_Visible, LS_COLLISION);
	}

	/* jetzt noch ein paar Sondefälle (Kassa, Vase, ...) */
	for (node = ls->p_ObjectRetrieval->getListHead(); node->_succ; node = (dbObjectNode*)node->_succ) {
		LSObjectNode *lso = (LSObjectNode *)node;

		if (lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_OTHER_1))
			lsTurnObject(lso, lso->uch_Visible, LS_COLLISION);

		/* because of the dirty hack statues need to be refreshed specially */
		if (lso->Type == Item_Statue) {
			if (lso->uch_Visible != LS_OBJECT_VISIBLE)
				lsRefreshStatue(lso);
		}
	}

	/* now refresh all doors and special objects */
	for (node = ls->p_ObjectRetrieval->getListHead(); node->_succ; node = (dbObjectNode*)node->_succ) {
		LSObjectNode *lso = (LSObjectNode *)node;

		if (lsIsObjectADoor(lso))
			lsInitDoorRefresh(node->_nr);
		else if (lsIsObjectSpecial(lso))
			lsInitDoorRefresh(node->_nr);
	}

	/* finally build the doors and specials on the PC */
	for (node = ls->p_ObjectRetrieval->getListHead(); node->_succ; node = (dbObjectNode *)node->_succ) {
		LSObjectNode *lso = (LSObjectNode *)node;

		if (lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_SPECIAL))
			lsTurnObject(lso, lso->uch_Visible, LS_COLLISION);
	}

	for (node = ls->p_ObjectRetrieval->getListHead(); node->_succ; node = (dbObjectNode *)node->_succ) {
		LSObjectNode *lso = (LSObjectNode *)node;

		if (lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_DOOR)) {
			if (!(lso->ul_Status & 1 << Const_tcOPEN_CLOSE_BIT))
				lsTurnObject((LSObjectNode *)node, lso->uch_Visible, LS_COLLISION);
			else
				lsTurnObject((LSObjectNode *)node, lso->uch_Visible, LS_NO_COLLISION);
		}
	}

	/* und weiter gehts mit der Musi... */
	BobSetDarkness(area->uch_Darkness);
	lsSetDarkness(area->uch_Darkness);

	lsShowAllSpots(0, LS_ALL_VISIBLE_SPOTS);

	/* 32er Collection einblenden */
	gfxPrepareColl(area->us_Coll32ID);
	gfxGetPalette(area->us_Coll32ID, palette);
	gfxSetColorRange(0, 127);

	/* only copy the background ! */
	for (i = 64 * 3; i < 65 * 3; i++)
		palette[i] = palette[i - 64 * 3];

	/* calculate other colours (darken!) */
	for (i = 65 * 3; i < 128 * 3; i++) {
		palette[i] = palette[i - 64 * 3];   /* light */
		palette[i - 64 * 3] /= 2;   /* dark */
	}

	gfxChangeColors(nullptr, 5, GFX_BLEND_UP, palette);

	/* now show colors of the maxis */
	gfxPrepareColl(137);
	gfxGetPalette(137, palette);
	gfxSetColorRange(128, 191);
	gfxChangeColors(nullptr, 0, GFX_BLEND_UP, palette);

	/* and just to be safe also set the menu colors */
	tcSetPermanentColors();

#ifdef THECLOU_DEBUG
	lsShowRooms();
#endif
}

void lsSetDarkness(byte value) {
	gfxSetDarkness(value);
}

void lsTurnObject(LSObjectNode *lso, byte status, byte Collis) {
	uint16 floorIndex = lsGetFloorIndex(lso->us_DestX, lso->us_DestY);

	lso->uch_Visible = status;

	if (status == LS_OBJECT_VISIBLE)
		LS_SET_OBJECT(ls->p_CurrFloor[floorIndex].uch_FloorType);

	if (lso->Type == Item_Microphone)
		LS_SET_MICRO_ON_FLOOR(ls->p_CurrFloor[floorIndex].uch_FloorType);
}

bool lsIsInside(LSObjectNode *lso, uint16 x, uint16 y, uint16 x1, uint16 y1) {
	uint16 lsoX, lsoY, lsoX1, lsoY1;

	lsCalcExactSize(lso, &lsoX, &lsoY, &lsoX1, &lsoY1);

	if (lsoX <= x1 && lsoX1 >= x && lsoY <= y1 && lsoY1 >= y)
		return true;

	return false;
}

void lsSetActivLiving(const char *Name, uint16 x, uint16 y) {
	if (Name) {
		strcpy(ls->uch_ActivLiving, Name);

		if (x == (uint16) -1)
			x = livGetXPos(Name);
		if (y == (uint16) -1)
			y = livGetYPos(Name);

		lsSetVisibleWindow(x, y);
		livRefreshAll();

		ls->us_PersonXPos = (uint16)(x - ls->us_WindowXPos);
		ls->us_PersonYPos = (uint16)(y - ls->us_WindowYPos);
	}
}

void lsSetObjectState(uint32 objID, byte bitNr, byte value) {
	LSObjectNode *object = (LSObjectNode *) dbGetObject(objID);

	/* for a time clock the status must not change */
	if (object->Type == Item_Clock)
		return;

	if (value == 0)
		object->ul_Status &= (0xffffffff - (1 << bitNr));
	else if (value == 1)
		object->ul_Status |= (1 << bitNr);
}

static int16 lsSortByXCoord(dbObjectNode *n1, dbObjectNode *n2) {
	LSObjectNode *lso1 = (LSObjectNode *)n1;
	LSObjectNode *lso2 = (LSObjectNode *)n2;

	if (lso1->us_DestX > lso2->us_DestX)
		return 1;

	return -1;
}

static int16 lsSortByYCoord(dbObjectNode *n1, dbObjectNode *n2) {
	LSObjectNode *lso1 = (LSObjectNode *)n1;
	LSObjectNode *lso2 = (LSObjectNode *)n2;

	if (lso1->us_DestY < lso2->us_DestY)
		return 1;

	return -1;
}

static void lsSortObjectList(NewObjectList<dbObjectNode> **l) {
	if ((*l)->isEmpty())
		return;
	
	bool lastNode = false;
	dbSortObjectList(l, lsSortByYCoord);

	for (dbObjectNode *node = (*l)->getListHead(); !lastNode && node->_succ->_succ;) {
		dbObjectNode *node1 = node;

		LSObjectNode* lso1, * lso2;
		do {
			node1 = (dbObjectNode *)node1->_succ;
			lso1 = (LSObjectNode *)node;
			lso2 = (LSObjectNode *)node1;
		} while (lso1->us_DestY == lso2->us_DestY && node1->_succ->_succ);

		dbObjectNode* next = node1;

		/* wenn Abbruch wegen NODE_SUCC(NODE_SUCC(.. erflogte, darf
		 * nicht der NODE_PRED(node1) genomen werden!
		 * nach dem Sortieren ist außerdem Schluß!
		 */

		if (lso1->us_DestY != lso2->us_DestY)
			node1 = (dbObjectNode*)node1->_pred;
		else
			lastNode = true;

		if (node != node1) {
			dbSortPartOfList(*l, node, node1, lsSortByXCoord);
			node = next;
		} else
			node = (dbObjectNode*)node->_succ;
	}
}

void lsRefreshObjectList(uint32 areaID) {
	SetObjectListAttr(OLF_PRIVATE_LIST, Object_LSObject);
	AskAll(dbGetObject(areaID), ConsistOfRelationID, BuildObjectList);
	ls->p_ObjectRetrieval = ObjectListPrivate;

	lsSortObjectList(&ls->p_ObjectRetrieval);
}

uint32 lsAddLootBag(uint16 x, uint16 y, byte bagNr) {
	/* bagNr : 1 - 8! */
	LSObjectNode *lso = (LSObjectNode *)dbGetObject(9700 + bagNr);

	lso->uch_Visible = LS_OBJECT_VISIBLE;

	/* add loot bag */
	if (!hasLootBag(ls->ul_AreaID, (uint32)(9700 + bagNr))) {
		lso->us_DestX = x;
		lso->us_DestY = y;

		BobSet(lso->us_OffsetFact, x, y, LS_LOOTBAG_X_OFFSET, LS_LOOTBAG_Y_OFFSET);
		BobVis(lso->us_OffsetFact);

		hasLootBagSet(ls->ul_AreaID, (uint32)(9700 + bagNr));
	}

	return (uint32)(9700 + bagNr);
}

void lsRemLootBag(uint32 bagId) {
	LSObjectNode *lso = (LSObjectNode *)dbGetObject(bagId);

	lso->uch_Visible = LS_OBJECT_INVISIBLE;
	BobInVis(lso->us_OffsetFact);

	hasLootBagUnSet(ls->ul_AreaID, bagId);
}

void lsRefreshAllLootBags() {
	livPrepareAnims();

	for (uint32 i = 1; i < 9; i++) {
		LSObjectNode *lso = (LSObjectNode *)dbGetObject(9700 + i);

		if (lso->uch_Visible == LS_OBJECT_VISIBLE && hasLootBag(ls->ul_AreaID, (uint32)(9700 + i))) {
			BobSet(lso->us_OffsetFact, lso->us_DestX, lso->us_DestY, LS_LOOTBAG_X_OFFSET, LS_LOOTBAG_Y_OFFSET);
			BobVis(lso->us_OffsetFact);
		}
	}
}

void lsGuyInsideSpot(uint16 *us_XPos, uint16 *us_YPos, uint32 *areaId) {
	NewList<SpotNode> *spots = lsGetSpotList();

	for (SpotNode* s = spots->getListHead(); s->_succ; s = (SpotNode *) s->_succ) {
		if (s->uch_Status & LS_SPOT_ON) {
			for (int32 i = 0; i < 4; i++) {
				if (us_XPos[i] != (uint16) -1 && us_YPos[i] != (uint16) -1) {
					if (areaId[i] == s->ul_AreaId) {
						if (s->p_CurrPos) {
							int32 x = s->p_CurrPos->us_XPos;    /* linke, obere */
							int32 y = s->p_CurrPos->us_YPos;    /* Ecke des Spot! */
							int32 size = s->us_Size - 4;

							if (us_XPos[i] < x + size - 2 && us_XPos[i] > x + 2) {
								if (us_YPos[i] < y + size - 2 && us_YPos[i] > y + 2)
									Search.SpotTouchCount[i]++;
							}
						}
					}
				}
			}
		}
	}
}

void lsWalkThroughWindow(LSObjectNode *lso, uint16 us_LivXPos, uint16 us_LivYPos, uint16 *us_XPos, uint16 *us_YPos) {
	int16 deltaX, deltaY;

	*us_XPos = us_LivXPos;
	*us_YPos = us_LivYPos;

	if (us_LivXPos < lso->us_DestX)
		deltaX = 25;
	else
		deltaX = -25;

	if (us_LivYPos < lso->us_DestY)
		deltaY = 25;
	else
		deltaY = -25;

	if (lso->ul_Status & (1 << Const_tcHORIZ_VERT_BIT))
		*us_XPos += deltaX;   /* vertical */
	else
		*us_YPos += deltaY;   /* horizontal */
}

void lsPatchObjects() {
	((ItemNode *) dbGetObject(Item_Window))->OffsetFact = 16;

	for (dbObjectNode* n = ls->p_ObjectRetrieval->getListHead(); n->_succ; n = (dbObjectNode *)n->_succ) {
		LSObjectNode *lso = (LSObjectNode *)n;

		ItemNode *item = (ItemNode *)dbGetObject(lso->Type);

		lso->uch_Size = item->Size; /* sizes are the same everywhere! */

		switch (lso->Type) {
		case Item_Statue:
			lso->ul_Status |= (1 << Const_tcACCESS_BIT);
			break;
		case Item_WC:
		case Item_Fridge:
		case Item_Night_box:
			lso->ul_Status |= (1 << Const_tcLOCK_UNLOCK_BIT);  /* unlocked! */
			break;
		case Item_Stone_wall:
		case Item_Bar:
		case Item_Vase:
		case Item_Pedestal:
			lso->ul_Status &= (~(1 << Const_tcACCESS_BIT));    /* Steinmauern kein Access */
			break;
		default:
			break;
		}

		if (g_clue->getFeatures() & GF_PROFIDISK) {
			switch (lso->Type) {
			case Item_Confessional:
			case Item_Mailbag:
				lso->ul_Status |= (1 << Const_tcLOCK_UNLOCK_BIT);
				/* unlocked! */
				break;
			case Item_Ladder:
				lso->ul_Status &= (~(1 << Const_tcACCESS_BIT));
				/* Steinmauern kein Access */
				break;
			default:
				break;
			}
		}

		if (n->_nr == tcLAST_BURGLARY_LEFT_CTRL_OBJ)
			lso->ul_Status |= (1 << Const_tcON_OFF);

		lsSetOldState(lso); /* muß sein! */

		/* change for correcting amiga plans (corrects walls) before using pc level designer
		   lso->us_DestX -= 9;
		   lso->us_DestY += 17;
		 */
	}
}

void lsCalcExactSize(LSObjectNode *lso, uint16 *x0, uint16 *y0, uint16 *x1, uint16 *y1) {
	ItemNode *item = (ItemNode *)dbGetObject(lso->Type);
	byte vertical = 0;

	*x0 = lso->us_DestX;
	*y0 = lso->us_DestY;

	/* no idea why OPEN_CLOSE_BIT & HORIZ_VERT_BIT are swapped
	   for painting and image, but they are */
	if (lso->Type == Item_Picture || lso->Type == Item_Painting)
		vertical = lso->ul_Status & 3;
	else
		vertical = lso->ul_Status & 1;

	if (vertical) {     /* vertical */
		*x0 += item->VExactXOffset;
		*y0 += item->VExactYOffset;

		*x1 = (*x0) + item->VExactWidth;
		*y1 = (*y0) + item->VExactHeight;
	} else {
		*x0 += item->HExactXOffset;
		*y0 += item->HExactYOffset;

		*x1 = (*x0) + item->HExactWidth;
		*y1 = (*y0) + item->HExactHeight;
	}
}

void lsInitDoorRefresh(uint32 ObjId) {
/* copies a background that is covered by a door into a mem buffer */
	LSObjectNode *lso = (LSObjectNode *)dbGetObject(ObjId);
	LSDoorRefreshNode *drn;
	bool found = false;

	for (drn = ls->p_DoorRefreshList->getListHead(); drn->_succ; drn = (LSDoorRefreshNode *)drn->_succ) {
		if (drn->lso == lso)
			found = true;
	}

	if (found)
		return;

	uint16 width = lso->uch_Size;
	uint16 height = lso->uch_Size;

	uint16 destX = ls->us_DoorXOffset;
	uint16 destY = ls->us_DoorYOffset;

	Rect srcR, dstR;
	srcR.x = lso->us_DestX;
	srcR.y = lso->us_DestY;
	srcR.w = width;
	srcR.h = height;

	dstR.x = destX;
	dstR.y = destY;
	dstR.w = width;
	dstR.h = height;

	MemBlit(LSRPInMem, &srcR, LS_DOOR_REFRESH_MEM_RP, &dstR, GFX_ROP_BLIT);

	drn = ls->p_DoorRefreshList->createNode(nullptr);
	drn->lso = lso;
	drn->us_XOffset = ls->us_DoorXOffset;
	drn->us_YOffset = ls->us_DoorYOffset;

	if (ls->us_DoorXOffset + width >= 320) {
		if (ls->us_DoorYOffset <= 128) { /* prevent overflow */
			ls->us_DoorXOffset = 0;
			ls->us_DoorYOffset += height;
		}
	} else
		ls->us_DoorXOffset += width;
}

void lsDoDoorRefresh(LSObjectNode *lso) {
/* restore the background of a door from a mem buffer */
	LSDoorRefreshNode *drn;

	for (drn = ls->p_DoorRefreshList->getListHead(); drn->_succ; drn = (LSDoorRefreshNode *)drn->_succ) {
		if (drn->lso == lso)
			break;
	}

	uint16 width = lso->uch_Size;
	uint16 height = lso->uch_Size;

	gfxLSPut(LS_DOOR_REFRESH_MEM_RP, drn->us_XOffset, drn->us_YOffset, lso->us_DestX, lso->us_DestY, width, height);
}

} // End of namespace Clue

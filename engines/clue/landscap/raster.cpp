/*
**  $Filename: landscap/raster.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-02-94
**
**  landscape raster functions for "Der Clou!"
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

#include "clue/landscap/raster.h"
#include "common/util.h"

namespace Clue {

void lsShowRaster(uint32 areaID, byte perc) {
	LSAreaNode *area = (LSAreaNode *)dbGetObject(areaID);

	gfxShow(154, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);

	SetObjectListAttr(OLF_PRIVATE_LIST, Object_LSObject);
	AskAll(area, ConsistOfRelationID, BuildObjectList);
	NewList<dbObjectNode> *objects = ObjectListPrivate;

	/*lsSortObjectList(&objects);*/

	if (!objects->isEmpty()) {
		int32 count = (objects->getNrOfNodes() * perc) / 255;

		dbObjectNode *node;
		int32 i;

		for (node = objects->getListHead(), i = 0; node->_succ && i < count; node = (dbObjectNode *) node->_succ, i++) {
			LSObjectNode *lso = (LSObjectNode *)node;

			switch (lso->Type) {
			case Item_Wall:
			case Item_Wall_corner:
			case Item_Stone_wall:
				lsFadeRasterObject(areaID, lso, 1);
				break;
			default:
				break;
			}
		}

		for (node = objects->getListHead(), i = 0; node->_succ && i < count; node = (dbObjectNode *) node->_succ, i++) {
			LSObjectNode *lso = (LSObjectNode *)node;

			switch (lso->Type) {
			case Item_Wall:
			case Item_Wall_corner:
			case Item_Stone_wall:
				break;
			default:
				lsFadeRasterObject(areaID, lso, 1);
				break;
			}
		}
	} else
		Say(THECLOU_TXT, 0, MATT_PICTID, "KEIN_GRUNDRISS");

	objects->removeList();
}

dbObjectNode *lsGetSuccObject(dbObjectNode *start) {
	dbObjectNode *n = (dbObjectNode*)start->_succ;

	while (n->_succ) {
		LSObjectNode *lso = (LSObjectNode *) n;

		if (lso->ul_Status & (1 << Const_tcACCESS_BIT))
			return n;

		n = (dbObjectNode *) n->_succ;
	}

	return start;
}

dbObjectNode *lsGetPredObject(dbObjectNode *start) {
	dbObjectNode *n = (dbObjectNode *) start->_pred;

	while (n->_pred) {
		LSObjectNode *lso = (LSObjectNode *) n;

		if (lso->ul_Status & (1 << Const_tcACCESS_BIT))
			return n;

		n = (dbObjectNode *) n->_pred;
	}

	return start;
}


void lsFadeRasterObject(uint32 areaID, LSObjectNode *lso, byte status) {
	uint32 col;
	uint32 rasterXSize = lsGetRasterXSize(areaID);
	uint32 rasterYSize = lsGetRasterYSize(areaID);

	uint32 rasterSize = MIN(rasterXSize, rasterYSize);

	uint16 xStart, yStart, xEnd, yEnd;
	lsCalcExactSize(lso, &xStart, &yStart, &xEnd, &yEnd);

	xStart = (xStart * rasterSize) / LS_RASTER_X_SIZE;
	yStart = (yStart * rasterSize) / LS_RASTER_Y_SIZE;

	xEnd = (xEnd * rasterSize) / LS_RASTER_X_SIZE;
	yEnd = (yEnd * rasterSize) / LS_RASTER_Y_SIZE;

	xEnd = MAX(uint16(xStart + 3), xEnd);
	yEnd = MAX(uint16(yStart + 3), yEnd);

	if (status)
		col = (((ItemNode *) dbGetObject(lso->Type))->ColorNr);
	else
		col = 10;

	_lowerGc->setPens(col, col, col);
	_lowerGc->rectFill(xStart, yStart, xEnd, yEnd);
}

void lsShowAllConnections(uint32 areaID, dbObjectNode *node, byte perc) {
	// CHECKME: Initial value?
	static byte Alarm_Power;

	LSObjectNode *lso1 = (LSObjectNode *)node;

	uint32 rasterXSize = lsGetRasterXSize(areaID);
	uint32 rasterYSize = lsGetRasterYSize(areaID);
	uint32 rasterSize = MIN(rasterXSize, rasterYSize);
	uint32 relID = 0;

	switch (lso1->Type) {
	case Item_Alarm_system_Z3:
	case Item_Alarm_system_X3:
	case Item_Alarm_system_Top:
		relID = hasAlarmRelationID;
		if (Alarm_Power & 1)
			lsShowRaster(areaID, perc);
		Alarm_Power |= 1;
		break;
	case Item_Control_Box:
		relID = hasPowerRelationID;
		if (Alarm_Power & 2)
			lsShowRaster(areaID, perc);
		Alarm_Power |= 2;
		break;
	default:
		break;
	}

	if (relID) {
		uint32 col = ((ItemNode *) dbGetObject(lso1->Type))->ColorNr;

		SetObjectListAttr(OLF_NORMAL, Object_LSObject);
		AskAll(lso1, relID, BuildObjectList);

		uint32 srcX =
		    lso1->us_DestX + lso1->uch_ExactX + (lso1->uch_ExactX1 -
		            lso1->uch_ExactX) / 2;
		uint32 srcY =
		    lso1->us_DestY + lso1->uch_ExactY + (lso1->uch_ExactY1 -
		            lso1->uch_ExactY) / 2;

		srcX = (srcX * rasterSize) / LS_RASTER_X_SIZE;
		srcY = (srcY * rasterSize) / LS_RASTER_Y_SIZE;

		for (dbObjectNode *n = ObjectList->getListHead(); n->_succ; n = (dbObjectNode *)n->_succ) {
			_lowerGc->setPens(col, GFX_SAME_PEN, GFX_SAME_PEN);

			LSObjectNode *lso2 = (LSObjectNode *)n;

			uint16 x0, y0, x1, y1;
			lsCalcExactSize(lso2, &x0, &y0, &x1, &y1);

			uint32 destX = x0 + (x1 - x0) / 2;
			uint32 destY = y0 + (y1 - y0) / 2;

			destX = (destX * rasterSize) / LS_RASTER_X_SIZE;
			destY = (destY * rasterSize) / LS_RASTER_Y_SIZE;

			_lowerGc->moveCursor(srcX, srcY);
			_lowerGc->draw(destX, srcY);
			_lowerGc->draw(destX, destY);

			_lowerGc->setPens(0, 0, col);
			_lowerGc->rectFill(destX - 1, destY - 1, destX + 2, destY + 2);
		}
	}
}

uint16 lsGetRasterXSize(uint32 areaID) {
	LSAreaNode *area = (LSAreaNode *)dbGetObject(areaID);

	return (uint16)(LS_RASTER_DISP_WIDTH /
	                ((area->us_Width) / LS_RASTER_X_SIZE));
}

uint16 lsGetRasterYSize(uint32 areaID) {
	LSAreaNode *area = (LSAreaNode *)dbGetObject(areaID);

	return (uint16)(LS_RASTER_DISP_HEIGHT /
	                ((area->us_Height) / LS_RASTER_Y_SIZE));
}

} // End of namespace Clue

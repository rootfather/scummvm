/*
**  $Filename: landscap/hardware.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-02-94
**
**
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

namespace Clue {

static MemRastPort *lsPrepareFromMemBySize(byte uch_Size);
static MemRastPort *lsPrepareFromMem(LSObjectNode *lso);
void lsBlitOneObject(MemRastPort *rp, uint16 offsetFact, uint16 dx, uint16 dy, uint16 size);

uint16 us_ScrollX, us_ScrollY;

void lsShowEscapeCar() {
	BuildingNode *b = (BuildingNode *)dbGetObject(ls->ul_BuildingID);

	livPrepareAnims();

	BobSet(ls->us_EscapeCarBobId, b->CarXPos, b->CarYPos,
	       LS_ESCAPE_CAR_X_OFFSET, LS_ESCAPE_CAR_Y_OFFSET);
	BobVis(ls->us_EscapeCarBobId);
}

static void lsSetAlarmPict(LSObjectNode *lso) {
	uint16 x0, x1, y0, y1;
	lsCalcExactSize(lso, &x0, &y0, &x1, &y1);

	if (lso->uch_Chained & Const_tcCHAINED_TO_ALARM) {
		MemRastPort *rp = lsPrepareFromMemBySize(16);

		uint16 destx = ((int16) x1 - (int16) x0 - 6) / 2 + x0 - 1;
		uint16 desty = ((int16) y1 - (int16) y0 - 7) / 2 + y0 + 2;

		lsBlitOneObject(rp, 24, destx, desty, 16);
	}
}

/* repaints all visible doors in a certain area */
static void lsRefreshClosedDoors(uint16 us_X0, uint16 us_Y0,
                                 uint16 us_X1, uint16 us_Y1) {
	ls->uch_ShowObjectMask = 0x40;

	for (dbObjectNode *node = ls->p_ObjectRetrieval->getListHead(); node->_succ; node = (dbObjectNode *)node->_succ) {
		LSObjectNode *lso = (LSObjectNode *)node;

		if (lsIsInside(lso, us_X0, us_Y0, us_X1, us_Y1)) {
			if (lsIsObjectADoor(lso)) {
				if (lso->uch_Visible == LS_OBJECT_VISIBLE)
					warning("FIXME: Required call to lsShowOneObject in lsRefreshClosedDoor");
					// lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_DOOR);
			}
		}
	}
	ls->uch_ShowObjectMask = 0x0;
}

void lsRefreshStatue(LSObjectNode *lso) {
/* dirty, dirty, violates pretty much all conventions for DerClou!
   and, unbelievably, should work nevertheless.
   This lso represents the statue, it is not displayed, we only
   put a pedestal at lso->us_DestX, lso->us_DestY.
   The source coordinates are not taken from the data (eek!) but
   directly from the image; to get the correct source image, the
   function lsPrepareFromMem is called with a dummy object */

	ls->uch_ShowObjectMask = 0x40;

	uint16 size = 16;          /* is the size correct ?? */

	LSObjectNode dummy; /* only passed to lsPrepareFromMem */
	dummy.uch_Size = (byte) size;

	MemRastPort *rp = lsPrepareFromMem(&dummy);

	uint16 srcX = 16;          /* take from the image and put here! */
	uint16 srcY = 0;

	uint16 destX = lso->us_DestX + 7;
	uint16 destY = lso->us_DestY + 10;

	gfxLSPutMsk(rp, srcX, srcY, destX, destY, size, size);

	ls->uch_ShowObjectMask = 0x0;
}

void lsFastRefresh(LSObjectNode *lso) {
	ls->uch_ShowObjectMask = 0x40;  /* ignore bit 6 */

	switch (lso->Type) {
	case Item_Wooden_door:
	case Item_Steel_door:
	case Item_Vault:
	case Item_Wall_gate:
		if (lso->ul_Status & (1 << Const_tcOPEN_CLOSE_BIT)) {
			lsDoDoorRefresh(lso);

			int16 x0 = MAX((int16) lso->us_DestX - 32, 0);
			int16 y0 = MAX((int16) lso->us_DestY - 32, 0);

			lsRefreshClosedDoors(x0, y0, lso->us_DestX + 32,
			                     lso->us_DestY + 32);
		} else
			lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_ALL);
		break;

	case Item_Painting:
	case Item_Picture:
		/* paintings and pictures are simply painted over with a gray RectFill
		       (shadow colour) when they are taken */

		if (lso->uch_Visible == LS_OBJECT_VISIBLE)
			lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_ALL);
		else {
			LSAreaNode *area = (LSAreaNode *)dbGetObject(lsGetActivAreaID());
			byte color = LS_REFRESH_SHADOW_COLOR1;

			uint16 x0, x1, y0, y1;
			lsCalcExactSize(lso, &x0, &y0, &x1, &y1);

			if (area->uch_Darkness == LS_DARKNESS)
				color = LS_REFRESH_SHADOW_COLOR0;

			gfxLSRectFill(x0, y0, x1, y1, color);

			lsSetAlarmPict(lso);
		}
		break;
	case Item_Statue:
		if (lso->uch_Visible == LS_OBJECT_VISIBLE)
			lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_ALL);
		else
			lsRefreshStatue(lso);
		break;
	default:
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
				if (lso->uch_Visible == LS_OBJECT_VISIBLE)
					lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS,
					                LS_SHOW_ALL);
				else
					lsDoDoorRefresh(lso);
				break;
			case Item_Mailbag:
				break;
			default:
				lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_ALL);
				break;
			}
		} else {
			lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_ALL);
		}
		break;
	}

	switch (lso->Type) {
	case Item_Window:
		lsSetAlarmPict(lso);
		break;

	default:
		break;
	}

	ls->uch_ShowObjectMask = 0; /* copy all bits */
}

static MemRastPort *lsPrepareFromMemBySize(byte uch_Size) {
	MemRastPort *rp = nullptr;

	switch (uch_Size) {
	case 16:
		rp = LS_COLL16_MEM_RP;
		break;
	case 32:
		rp = LS_COLL32_MEM_RP;
		break;
	case 48:
		rp = LS_COLL48_MEM_RP;
		break;
	}

	assert(rp);
	return rp;
}

/* put the collection with the image of an lso into the LS_PREPARE_BUFFER */
static MemRastPort *lsPrepareFromMem(LSObjectNode *lso) {
	return lsPrepareFromMemBySize(lso->uch_Size);
}

void lsBlitOneObject(MemRastPort *rp, uint16 offsetFact, uint16 dx, uint16 dy, uint16 size) {
	uint16 srcWidth = 288;  /* source screen is usually 288 wide */

	/* 32er objects are on a screen that is 320 pixels wide */
	if (size == 32)
		srcWidth = 320;

	uint16 perRow = srcWidth / size;
	uint16 srcY = (offsetFact / perRow) * size;
	uint16 srcX = (offsetFact % perRow) * size;

	if (ls->uch_ShowObjectMask)
		gfxLSPutMsk(rp, srcX, srcY, dx, dy, size, size);
	else
		gfxLSPutMsk(rp, srcX, srcY, dx, dy, size, size);

}

bool lsShowOneObject(LSObjectNode *lso, int16 destx, int16 desty, uint32 ul_Mode) {
	ItemNode *item = (ItemNode *)dbGetObject(lso->Type);
	bool show = false;

	switch (lso->Type) {
	case Item_Pedestal:       /* pedestal should not be displayed */
		break;

	default:
		if (ul_Mode & LS_SHOW_ALL)
			show = true;

		if ((!show) && (ul_Mode & LS_SHOW_WALL))
			if (lsIsObjectAWall(lso))
				show = true;

		/* only display doors if they are visible */

		if ((!show) && (ul_Mode & LS_SHOW_DOOR))
			if (lsIsObjectADoor(lso) && lso->uch_Visible == LS_OBJECT_VISIBLE)
				show = true;

		if ((!show) && (ul_Mode & LS_SHOW_SPECIAL))
			if (lsIsObjectSpecial(lso) && lso->uch_Visible == LS_OBJECT_VISIBLE)
				show = true;

		if ((!show) && (ul_Mode & LS_SHOW_OTHER_0))
			if (lsIsObjectAStdObj(lso) && lso->uch_Visible == LS_OBJECT_VISIBLE)
				show = true;

		if ((!show) && (ul_Mode & LS_SHOW_OTHER_1))
			if (lsIsObjectAnAddOn(lso) && lso->uch_Visible == LS_OBJECT_VISIBLE)
				show = true;

		if (show) {
			MemRastPort *rp = lsPrepareFromMem(lso);

			uint16 offsetFact = item->OffsetFact + (lso->ul_Status & 3);

			if (destx == LS_STD_COORDS)
				destx = lso->us_DestX;

			if (desty == LS_STD_COORDS)
				desty = lso->us_DestY;

			lsBlitOneObject(rp, offsetFact, destx, desty, item->Size);
		}
		break;
	}

	if (show)
		lsSetAlarmPict(lso);

	return show;
}

void lsBlitFloor(uint16 floorIndex, uint16 destx, uint16 desty) {
	MemRastPort *rp = LS_FLOOR_MEM_RP;
	uint16 srcX = ((ls->p_CurrFloor[floorIndex].uch_FloorType) & 0xf) * LS_FLOOR_X_SIZE;

	gfxLSPut(rp, srcX, 0, destx, desty, LS_FLOOR_X_SIZE, LS_FLOOR_Y_SIZE);
}

void lsSetViewPort(uint16 x, uint16 y) {
	gfxNCH4SetViewPort(x, y);
}

void lsInitGfx() {
	gfxSetVideoMode(GFX_VIDEO_NCH4);

	lsSetViewPort(0, 0);
	gfxNCH4SetSplit(256);
}

void lsCloseGfx() {
	gfxSetVideoMode(GFX_VIDEO_MCGA);

	tcSetPermanentColors();

	inpMousePtrOn();
}

void lsScrollCorrectData(int32 dx, int32 dy) {
	ls->us_WindowXPos += dx;
	ls->us_WindowYPos += dy;

	ls->us_RasInfoScrollX = (int16) dx;
	ls->us_RasInfoScrollY = (int16) dy;

	livSetVisLScape(ls->us_WindowXPos, ls->us_WindowYPos);
}

void lsDoScroll() {
	gfxNCH4Scroll(ls->us_RasInfoScrollX, ls->us_RasInfoScrollY);

	ls->us_RasInfoScrollX = 0;
	ls->us_RasInfoScrollY = 0;
}

} // End of namespace Clue

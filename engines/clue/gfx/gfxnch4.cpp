/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa
  Portions copyright (c) 2005 Jens Granseuer

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "clue/landscap/landscap.h"
#include "clue/living/bob.h"
#include "common/util.h"

namespace Clue {

int ScrX, ScrY;

static uint8 ScrDark;

void gfxSetDarkness(uint8 value) {
	uint8 *dp = LSRPInMem->pixels;

	if (value == LS_BRIGHTNESS) {
		int h = LS_MAX_AREA_WIDTH;
		do {
			int i = LS_MAX_AREA_HEIGHT;

			do {
				*dp++ |= 64;
			} while (--i);
		} while (--h);
	} else {
		int h = LS_MAX_AREA_WIDTH;
		do {
			int i = LS_MAX_AREA_HEIGHT;

			do {
				*dp++ &= ~64;
			} while (--i);
		} while (--h);
	}

	ScrDark = value;

	gfxNCH4Refresh();
}

void gfxNCH4Refresh() {
	gfxScreenFreeze();

	_upperGc->gfxBlit(LSRPInMem, ScrX, ScrY, 0, 0, LS_MAX_AREA_WIDTH, LS_MAX_AREA_HEIGHT, false);

	BobDisplayLists(_upperGc);

	_upperGc->gfxScreenThaw(0, 0, 320, 128);
	/*printf("REFRESH!\n");*/
}

void gfxNCH4SetViewPort(int x, int y) {
	ScrX = x;
	ScrY = y;

	gfxNCH4Refresh();
}

void gfxNCH4Scroll(int x, int y) {
	int sx = CLIP(ScrX + x, ScrLEFT, ScrRIGHT);
	int sy = CLIP(ScrY + y, ScrTOP, ScrBOTTOM);

	gfxNCH4SetViewPort(sx, sy);
}

void gfxNCH4SetSplit(uint16 line) {
}

void gfxLSInit() {
	ScrX = 0;
	ScrY = 0;

	gfxLSRectFill(0, 0, LS_MAX_AREA_WIDTH - 1, LS_MAX_AREA_HEIGHT - 1, 0);
}

void gfxLSPut(MemRastPort *sp, uint16 sx, uint16 sy, uint16 dx, uint16 dy, uint16 w, uint16 h) {
	Rect srcR;
	srcR.x = sx;
	srcR.y = sy;
	srcR.w = w;
	srcR.h = h;

	Rect dstR;
	dstR.x = dx;
	dstR.y = dy;
	dstR.w = w;
	dstR.h = h;

	MemBlit(sp, &srcR, LSRPInMem, &dstR, GFX_ROP_BLIT);
}

void gfxLSPutMsk(MemRastPort *sp, uint16 sx, uint16 sy, uint16 dx, uint16 dy, uint16 w, uint16 h) {
	Rect srcR;
	srcR.x = sx;
	srcR.y = sy;
	srcR.w = w;
	srcR.h = h;

	Rect dstR;
	dstR.x = dx;
	dstR.y = dy;
	dstR.w = w;
	dstR.h = h;

	MemBlit(sp, &srcR, LSRPInMem, &dstR, GFX_ROP_MASK_BLIT);
}

void gfxLSPutClr(MemRastPort *sp, uint16 sx, uint16 sy, uint16 dx, uint16 dy, uint16 w, uint16 h) {
	Rect srcR;
	srcR.x = sx;
	srcR.y = sy;
	srcR.w = w;
	srcR.h = h;

	Rect dstR;
	dstR.x = dx;
	dstR.y = dy;
	dstR.w = w;
	dstR.h = h;

	MemBlit(sp, &srcR, LSRPInMem, &dstR, GFX_ROP_CLR);
}

void gfxLSPutSet(MemRastPort *sp, uint16 sx, uint16 sy, uint16 dx, uint16 dy, uint16 w, uint16 h) {
	Rect srcR;
	srcR.x = sx;
	srcR.y = sy;
	srcR.w = w;
	srcR.h = h;

	Rect dstR;
	dstR.x = dx;
	dstR.y = dy;
	dstR.w = w;
	dstR.h = h;

	MemBlit(sp, &srcR, LSRPInMem, &dstR, GFX_ROP_SET);
}

void gfxLSRectFill(uint16 sx, uint16 sy, uint16 ex, uint16 ey, uint8 color) {
	if (sx > ex)
		SWAP(sx, ex);

	if (sy > ey)
		SWAP(sy, ey);

	ex = MIN(ex, uint16(LS_MAX_AREA_WIDTH - 1));
	ey = MIN(ey, uint16(LS_MAX_AREA_HEIGHT - 1));

	uint16 w = ex - sx + 1;
	uint16 h = ey - sy + 1;

	uint8 *dp = LSRPInMem->pixels;
	dp += sy * LS_MAX_AREA_WIDTH + sx;

	do {
		memset(dp, color, w);
		dp += LS_MAX_AREA_WIDTH;
	} while (--h);
}

uint8 gfxLSReadPixel(uint16 x, uint16 y) {
	x = MIN(x, uint16(LS_MAX_AREA_WIDTH - 1));
	y = MIN(y, uint16(LS_MAX_AREA_HEIGHT - 1));

	return LSRPInMem->pixels[y * LS_MAX_AREA_WIDTH + x];
}

} // End of namespace Clue

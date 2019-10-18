/*
**  $Filename: gfx/gfx.h
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-02-94
**
**  gfx functions for "Der Clou!"
**
**   (c) 1994 ...and avoid panic by, H. Gaberschek
**      All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa
  Portions copyright (c) 2005 Jens Granseuer

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include <assert.h>

#include "clue/base/base.h"

#include "common/events.h"
#include "common/rect.h"
#include "common/system.h"
#include "engines/engine.h"
#include "engines/util.h"
#include "graphics/palette.h"
#include "graphics/surface.h"

struct _GC {
	Rect clip;

	GfxDrawModeE mode;

	uint8 foreground;              /* entspricht dem Farbregister */
	uint8 background;              /* ebenfalls absolut und nicht relativ zum */
	uint8 outline;                 /* Registerstart */

	uint8 colorStart;
	uint8 End;

	uint16 cursorX;
	uint16 cursorY;

	Font *font;
};

#include "clue/gfx/gfx.h"
#include "clue/gfx/gfx_p.h"

void gfxILBMToRAW(const uint8 *src, uint8 *dst, size_t size);

void gfxRealRefreshArea(uint16 x, uint16 y, uint16 w, uint16 h);

/********************************************************************
 * inits & dons
 */

void gfxInit(void) {
	ScreenFormat = Graphics::PixelFormat::createFormatCLUT8();
	initGraphics(SCREEN_WIDTH, SCREEN_HEIGHT);

	Screen = new Graphics::Surface();
	Screen->create(SCREEN_WIDTH, SCREEN_HEIGHT, ScreenFormat);

	gfxSetGC(NULL);

	/* diese RP müssen nur ein Bild maximaler Größe aufnehmen können */
	/* in anderen Modulen wird vorausgesetzt, daß alle RastPorts gleich */
	/* groß sind und auch gleich groß wie die StdBuffer sind */
	/* StdBuffer = 61 * 1024 = 62464, Mem: 62400 */

	/* Ausnahme (nachträglich) : der RefreshRP ist nur 320 * 140 Pixel groß!! */

	gfxInitMemRastPort(&StdRP0InMem, SCREEN_WIDTH, SCREEN_HEIGHT); /* CMAP muß auch Platz haben ! */
	gfxInitMemRastPort(&StdRP1InMem, SCREEN_WIDTH, SCREEN_HEIGHT);

	gfxInitMemRastPort(&AnimRPInMem, SCREEN_WIDTH, SCREEN_HEIGHT);
	gfxInitMemRastPort(&AddRPInMem, SCREEN_WIDTH, SCREEN_HEIGHT);
	gfxInitMemRastPort(&LSObjectRPInMem, SCREEN_WIDTH, SCREEN_HEIGHT);

	gfxInitMemRastPort(&LSFloorRPInMem, SCREEN_WIDTH, 32);

	/* der RefreshRP muß den ganzen Bildschirm aufnehmen können */
	gfxInitMemRastPort(&RefreshRPInMem, SCREEN_WIDTH, SCREEN_HEIGHT);

	gfxInitMemRastPort(&ScratchRP, SCREEN_WIDTH, SCREEN_HEIGHT);

	gfxInitMemRastPort(&BobRPInMem, SCREEN_WIDTH, SCREEN_HEIGHT);

	gfxInitMemRastPort(&LSRPInMem, LS_MAX_AREA_WIDTH, LS_MAX_AREA_HEIGHT);

	bubbleFont =
	    gfxOpenFont(GFX_BUBBLE_FONT_NAME, 4, 8, 32, 255, SCREEN_WIDTH, 24);
	menuFont =
	    gfxOpenFont(GFX_MENU_FONT_NAME, 5, 9, 32, 255, SCREEN_WIDTH, 36);

	gfxInitGC(&LowerGC,
	          0, 0, 320, 140,
	          0, 191,
	          bubbleFont);
	gfxInitGC(&MenuGC,
	          0, 140, 320, 60,
	          191, 255,
	          menuFont);

	gfxInitGC(&LSUpperGC,
	          0, 0, 320, 128,
	          0, 191,
	          bubbleFont);
	gfxInitGC(&LSMenuGC,
	          0, 128, 320, 72,
	          191, 255,
	          menuFont);

	gfxInitCollList();
	gfxInitPictList();

	gfxSetVideoMode(GFX_VIDEO_MCGA);    /* after rastports !! */
}

void gfxDone(void) {
	if (PictureList) {
		RemoveList(PictureList);
		PictureList = NULL;
	}

	if (CollectionList) {
		RemoveList(CollectionList);
		CollectionList = NULL;
	}

	gfxCloseFont(bubbleFont);
	gfxCloseFont(menuFont);

	gfxDoneMemRastPort(&StdRP0InMem);
	gfxDoneMemRastPort(&StdRP1InMem);

	gfxDoneMemRastPort(&AnimRPInMem);
	gfxDoneMemRastPort(&AddRPInMem);
	gfxDoneMemRastPort(&LSObjectRPInMem);

	gfxDoneMemRastPort(&LSFloorRPInMem);

	gfxDoneMemRastPort(&RefreshRPInMem);

	gfxDoneMemRastPort(&ScratchRP);

	gfxDoneMemRastPort(&BobRPInMem);

	gfxDoneMemRastPort(&LSRPInMem);

	Screen->free();
}

void gfxSetGC(GC *gc) {
	GfxBase.gc = gc;
}

void gfxSetVideoMode(byte uch_NewMode) {
	GfxBase.uch_VideoMode = uch_NewMode;

	switch (uch_NewMode) {
	case GFX_VIDEO_MCGA:
		l_gc = &LowerGC;
		u_gc = &LowerGC;
		m_gc = &MenuGC;
		break;

	case GFX_VIDEO_NCH4:
		l_gc = &LowerGC;
		u_gc = &LSUpperGC;
		m_gc = &LSMenuGC;

		gfxLSInit();
		break;

	default:
	case GFX_VIDEO_TEXT:
		return;
	}

	gfxClearArea(NULL);
}

void gfxCorrectUpperRPBitmap(void)
/* should be called after each scrolling (and before displaying a bubble) */
{
	/*
	    NCH4UpperRP.p_BitMap = (void *) ((uint32) GfxBoardBase + gfxNCH4GetCurrScrollOffset());
	*/
}

/********************************************************************
 * lists
 */

static void gfxInitCollList(void) {
	char pathname[DSK_PATH_MAX];
	LIST *tempList = CreateList();
	NODE *n;

	CollectionList = CreateList();

	dskBuildPathName(DISK_CHECK_FILE, TEXT_DIRECTORY, COLL_LIST_TXT, pathname);
	ReadList(tempList, 0, pathname);

	for (n = (NODE *) LIST_HEAD(tempList); NODE_SUCC(n);
	        n = (NODE *) NODE_SUCC(n)) {
		struct Collection *coll;

		coll =
		    (Collection *)CreateNode(CollectionList, sizeof(struct Collection),
		                             txtGetKey(2, NODE_NAME(n)));

		coll->us_CollId = (uint16) txtGetKeyAsULONG(1, NODE_NAME(n));

		coll->puch_Filename = NODE_NAME(coll);
		coll->prepared = NULL;

		coll->us_TotalWidth = (uint16) txtGetKeyAsULONG(3, NODE_NAME(n));
		coll->us_TotalHeight = (uint16) txtGetKeyAsULONG(4, NODE_NAME(n));

		coll->uch_ColorRangeStart = (uint16) txtGetKeyAsULONG(5, NODE_NAME(n));
		coll->uch_ColorRangeEnd = (uint16) txtGetKeyAsULONG(6, NODE_NAME(n));
	}

	RemoveList(tempList);
}

static void gfxInitPictList(void) {
	char pathname[DSK_PATH_MAX];
	LIST *tempList = CreateList();
	NODE *n;

	PictureList = CreateList();

	dskBuildPathName(DISK_CHECK_FILE, TEXT_DIRECTORY, PICT_LIST_TXT, pathname);
	ReadList(tempList, 0, pathname);

	for (n = LIST_HEAD(tempList); NODE_SUCC(n); n = NODE_SUCC(n)) {
		struct Picture *pict;

		pict = (Picture *)CreateNode(PictureList, sizeof(*pict), NULL);

		pict->us_PictId = (uint16) txtGetKeyAsULONG(1, NODE_NAME(n));
		pict->us_CollId = (uint16) txtGetKeyAsULONG(2, NODE_NAME(n));

		pict->us_XOffset = (uint16) txtGetKeyAsULONG(3, NODE_NAME(n));
		pict->us_YOffset = (uint16) txtGetKeyAsULONG(4, NODE_NAME(n));

		pict->us_Width = (uint16) txtGetKeyAsULONG(5, NODE_NAME(n));
		pict->us_Height = (uint16) txtGetKeyAsULONG(6, NODE_NAME(n));

		pict->us_DestX = (uint16) txtGetKeyAsULONG(7, NODE_NAME(n));
		pict->us_DestY = (uint16) txtGetKeyAsULONG(8, NODE_NAME(n));
	}

	RemoveList(tempList);
}

typedef struct {
	int x;
	int y;
	int w;
	int h;
} Rectangle;

/*
 * Clips the rectangle B against rectangle A.
 */
static Rectangle Clip(Rectangle A, Rectangle B) {
	int dist;
	Rectangle C = { 0, 0, -1, -1 };

	if (B.x >= A.x + A.w) {
		return C;
	}
	if (B.x + B.w <= A.x) {
		return C;
	}
	if (B.y >= A.y + A.h) {
		return C;
	}
	if (B.y + B.h <= A.y) {
		return C;
	}

	dist = A.x - B.x;
	if (dist > 0) {
		B.x += dist;
		B.w -= dist;
	}
	dist = (B.x + B.w) - (A.x + A.w);
	if (dist > 0) {
		B.w -= dist;
	}
	dist = A.y - B.y;
	if (dist > 0) {
		B.y += dist;
		B.h -= dist;
	}
	dist = (B.y + B.h) - (A.y + A.h);
	if (dist > 0) {
		B.h -= dist;
	}

	return B;
}

/********************************************************************
 * Rastports...
 */

static void gfxInitGC(GC *gc, uint16 x, uint16 y, uint16 w, uint16 h,
                      uint8 colorStart, uint8 End, Font *font) {
	Rectangle dstR, dstR2;

	dstR.x = 0;
	dstR.y = 0;
	dstR.w = SCREEN_WIDTH;
	dstR.h = SCREEN_HEIGHT;

	dstR2.x = x;
	dstR2.y = y;
	dstR2.w = w;
	dstR2.h = h;
	dstR = Clip(dstR, dstR2);

	if (dstR.w <= 0 || dstR.h <= 0) {
		DebugMsg(ERR_ERROR, ERROR_MODULE_GFX, "gfxInitGC");
	}

	gc->clip.x      = dstR.x;
	gc->clip.y      = dstR.y;
	gc->clip.w      = dstR.w;
	gc->clip.h      = dstR.h;

	gc->mode        = GFX_JAM_1;

	gc->foreground  = colorStart + 1;
	gc->background  = colorStart;
	gc->outline     = colorStart + 1;

	gc->colorStart  = colorStart;
	gc->End         = End;

	gfxMoveCursor(gc, 0, 0);

	gc->font        = font;
}



/*******************************************************************
 * FONTS
 * gfxOpenFont
 * gfxCloseFont
 */

static Font *gfxOpenFont(const char *fileName, uint16 w, uint16 h,
                         unsigned char first, unsigned char last,
                         uint16 sw, uint16 sh) {
	Font *font;

	char path[DSK_PATH_MAX];
	uint8 *lbm;

	Graphics::Surface *bmp;
	uint32 size;


	/* create font structure. */
	font = (Font *)TCAllocMem(sizeof(*font), true);

	font->w     = w;
	font->h     = h;

	font->first = first;
	font->last  = last;


	dskBuildPathName(DISK_CHECK_FILE, PICTURE_DIRECTORY, fileName, path);
	lbm = (uint8 *)dskLoad(path);


	bmp = new Graphics::Surface();
	bmp->create(sw, sh, ScreenFormat);
	size = sw * sh;

	gfxILBMToRAW(lbm, (uint8 *)bmp->getPixels(), size);

	free(lbm);

	font->bmp = bmp;

	return font;
}

void gfxCloseFont(Font *font) {
	if (font) {
		font->bmp->free();

		TCFreeMem(font, sizeof(*font));
	}
}

/*******************************************************************
 * gfxMoveCursor (amiga function: Move)
 * gfxSetPens    (amiga defines:   SetAPen, SetBPen, SetOPen)
 * gfxRectFill   (amiga function: RectFill)
 * gfxTextLength (amiga function: TextLength)
 * gfxSetDrMd    (amiga define:   SetDrMd)
 * gfxDraw       (amiga function: Draw)
 * gfxReadPixel  (amiga function: ReadPixel)
 */

void gfxDraw(GC *gc, uint16 x, uint16 y) {
	/* this function doesn't perform clipping properly... but that's ok! */
	x += gc->clip.x;
	y += gc->clip.y;

	if (x < gc->clip.w && y < gc->clip.h) {
		uint16 rx, ry, rx1, ry1, dx, dy, sx, sy, dw, i;
		Graphics::Surface *dst;
		uint8 color, *dp;

		rx = x;
		ry = y;

		rx1 = gc->cursorX;
		ry1 = gc->cursorY;

		if (rx < rx1) {
			sx = rx;
			dx = rx1 - rx + 1;
		} else {
			sx = rx1;
			dx = rx - rx1 + 1;
		}

		if (ry < ry1) {
			sy = ry;
			dy = ry1 - ry + 1;
		} else {
			sy = ry1;
			dy = ry - ry1 + 1;
		}

		color = gc->foreground;
		dst = Screen;

		dw = dst->w;

		dp = (uint8 *)dst->getPixels();
		dp += sy * dw + sx;

		if (rx == rx1) {
			for (i = 0; i < dy; i++) {
				*dp = color;
				dp += dw;
			}
		} else if (ry == ry1) {
			memset(dp, color, dx);
		} else {
			/* the line is slanted. oops. */
		}

		gc->cursorX = x;
		gc->cursorY = y;

		if (rx == rx1) {
			gfxRefreshArea(sx, 1, sy, dy);
		} else if (ry == ry1) {
			gfxRefreshArea(sx, dx, sy, 1);
		} else {
			/* the line is slanted. oops. */
			DebugMsg(ERR_DEBUG, ERROR_MODULE_GFX, "gfxDraw");
		}
	}
}

void gfxMoveCursor(GC *gc, uint16 x, uint16 y) {
	gc->cursorX = gc->clip.x + MIN(x, uint16(gc->clip.w - 1));
	gc->cursorY = gc->clip.y + MIN(y, uint16(gc->clip.h - 1));
}

void gfxSetPens(GC *gc, uint8 foreground, uint8 background, uint8 outline) {
	if (foreground != GFX_SAME_PEN)
		gc->foreground = foreground;

	if (background != GFX_SAME_PEN)
		gc->background = background;

	if (outline != GFX_SAME_PEN)
		gc->outline = outline;
}

void gfxRectFill(GC *gc, uint16 sx, uint16 sy, uint16 ex, uint16 ey)
/* minimum size: 3 * 3 pixels ! */
{
	Common::Rect dst, dst2;

	if (sx > ex) {
		SWAP(sx, ex);
	}

	if (sy > ey) {
		SWAP(sy, ey);
	}

	dst.left = gc->clip.x + sx;
	dst.top = gc->clip.y + sy;
	dst.right = gc->clip.x + ex + 1;
	dst.bottom = gc->clip.y + ey + 1;

	Screen->fillRect(dst, gc->foreground);
	Screen->frameRect(dst, gc->outline);

	gfxRefreshArea(dst.left, dst.top, dst.width(), dst.height());
}

void gfxSetDrMd(GC *gc, GfxDrawModeE mode) {
	gc->mode = mode;
}

void gfxSetFont(GC *gc, Font *font) {
	gc->font = font;
}

/* berechnet die Länge eines Textes in Pixel */
uint16 gfxTextWidth(GC *gc, const char *txt, size_t len) {
	size_t w;

	w = len * gc->font->w;

	if (w > USHRT_MAX) {
		return 0;
	} else {
		return w;
	}
}

/*******************************************************************
 * access & calc functions
 */

struct Collection *gfxGetCollection(uint16 us_CollId) {
	return (Collection *)GetNthNode(CollectionList, (uint32)(us_CollId - 1));
}

struct Picture *gfxGetPicture(uint16 us_PictId) {
	return (Picture *)GetNthNode(PictureList, (uint32)(us_PictId - 1));
}

/* the collection must have been prepared (PrepareColl) beforehand */
void gfxGetPalette(uint16 collId, uint8 *palette) {
	memcpy(palette, ScratchRP.palette, GFX_PALETTE_SIZE);
}

static int32 gfxGetRealDestY(GC *gc, int32 destY) {
	destY -= gc->clip.y;

	return destY;
}

/*******************************************************************
 * collections...
 */

/* nach dieser Funktion befindet sich im ScratchRP die entpackte Collection */
void gfxPrepareColl(uint16 collId) {
	struct Collection *coll;

	if (!(coll = gfxGetCollection(collId))) {
		DebugMsg(ERR_DEBUG, ERROR_MODULE_GFX, "gfxPrepareColl");
		return;
	}

	if (coll->prepared) {
		gfxScratchFromMem(coll->prepared);
	} else {
		char pathname[DSK_PATH_MAX];

		/* Dateiname erstellen */
		dskBuildPathName(DISK_CHECK_FILE, PICTURE_DIRECTORY, coll->puch_Filename, pathname);

		gfxLoadILBM(pathname);

		/*
		     * coll->prepared wird nicht mit dem ScratchRP initialisert, da
		 * es sonst zu Inkonsistenzen kommen könnte. Collections im Scratch
		 * werden als nicht vorbereitet betrachtet, da der ScratchRP ständig
		 * durch andere Bilder überschrieben wird
		     */
		coll->prepared = NULL;
	}
}

void gfxLoadILBM(const char *fileName) {
	uint8 *lbm;

	/* Collection laden */
	lbm = (uint8 *)dskLoad(fileName);

	gfxSetCMAP(lbm);
	gfxILBMToRAW(lbm, ScratchRP.pixels, SCREEN_SIZE);
	free(lbm);
}

void gfxUnPrepareColl(uint16 collId) {
	/* dummy function */
}

void gfxCollFromMem(uint16 collId) {
	struct Collection *coll;
	MemRastPort *rp;

	if ((coll = gfxGetCollection(collId)) && (rp = coll->prepared)) {
		gfxScratchFromMem(rp);
	}
}

void gfxCollToMem(uint16 collId, MemRastPort *rp) {
	struct Collection *coll;

	/*
	 * wenn sich in diesem MemRastPort ein anderes Bild befindet so wird dieses
	 * nun aus dem MemRastPort "entfernt"
	 */
	if (rp->collId != GFX_NO_COLL_IN_MEM && collId != rp->collId) {
		struct Collection *oldColl;

		if ((oldColl = gfxGetCollection(rp->collId))) {
			oldColl->prepared = NULL;
		}
	}

	gfxPrepareColl(collId);
	gfxScratchToMem(rp);

	/* enter the MemRastPort in the new collection */
	if ((coll = gfxGetCollection(collId))) {
		coll->prepared = rp;
	}
	rp->collId = collId;
}

/*******************************************************************
 * print functions
 */

void gfxSetRect(uint16 us_X, uint16 us_Width) {
	GlobalPrintRect.us_X = us_X;
	GlobalPrintRect.us_Width = us_Width;
}

static void
ScreenBlitChar(GC *gc, Graphics::Surface *src, Rect *src_rect,
               Graphics::Surface *dst, Rect *dst_rect, uint8 color) {
	Rectangle srcR, dstR, dstR2, areaR;

	uint8 *dp, *sp;
	uint16 h;

	/* clip. */
	srcR.x = src_rect->x;
	srcR.y = src_rect->y;
	srcR.w = src_rect->w;
	srcR.h = src_rect->h;

	dstR.x = 0;
	dstR.y = 0;
	dstR.w = dst->w;
	dstR.h = dst->h;

	dstR2.x = dst_rect->x + gc->clip.x;
	dstR2.y = dst_rect->y + gc->clip.y;
	dstR2.w = gc->clip.w;
	dstR2.h = gc->clip.h;
	dstR = Clip(dstR, dstR2);

	if (dstR.w <= 0 || dstR.h <= 0) {
		return;
	}

	/* blit. */
	areaR.x = dstR.x;
	areaR.y = dstR.y;
	areaR.w = MIN(dstR.w, srcR.w);
	areaR.h = MIN(dstR.h, srcR.h);

	dp = (uint8 *)dst->getPixels();
	sp = (uint8 *)src->getPixels();

	dp += dstR.y * dst->w + dstR.x;
	sp += srcR.y * src->w + srcR.x;

	h = areaR.h;

	while (h--) {
		uint16 x;
		for (x = 0; x < areaR.w; x++) {
			if (sp[x] != 0) {
				dp[x] = color;
			}
		}
		dp += dst->pitch;
		sp += src->pitch;
	}
}

void gfxPrintExact(GC *gc, const char *txt, uint16 x, uint16 y) {
	if (txt[0] == '\0')
		return;

	const Font *font = gc->font;
	const uint16 w = font->w,
	             h = font->h,
	             base = font->first;
	const uint16 chars_per_line = SCREEN_WIDTH / w;
	const uint8 fg = gc->foreground;
	size_t len = strlen(txt), t;

	Common::Rect area;
	Rect srcR, dstR;
	Graphics::Surface *src, *dst;

	area.left = gc->clip.x;
	area.top = gc->clip.y;
	area.setWidth(gc->clip.w);
	area.setHeight(gc->clip.h);

	area.left += x;
	area.top += y;
	area.setWidth(len * w);
	area.setHeight(h);

	if (gc->mode == GFX_JAM_2)
		Screen->fillRect(area, gc->background);

	srcR.w = w;
	srcR.h = h;

	dstR.x = x;
	dstR.y = y;

	src = font->bmp;
	dst = Screen;

	for (t = 0; t < len; t++) {
		uint16 ch;

		ch  = (uint8)txt[t];
		ch -= base;

		srcR.y = (ch / chars_per_line) * h;
		srcR.x = (ch % chars_per_line) * w;

		ScreenBlitChar(gc, src, &srcR, dst, &dstR, fg);

		dstR.x += w;
	}

	gfxRefreshArea(area.left, area.top, area.width(), area.height());
}

void gfxPrint(GC *gc, const char *txt, uint16 y, uint32 mode) {
	if (txt[0] == '\0')
		return;

	uint16 x = GlobalPrintRect.us_X;
	uint16 w = gfxTextWidth(gc, txt, strlen(txt));

	if (mode & GFX_PRINT_RIGHT)
		x += GlobalPrintRect.us_Width - w;

	if (mode & GFX_PRINT_CENTER) {
		if (w <= GlobalPrintRect.us_Width)
			x += (GlobalPrintRect.us_Width - w) / 2;
	}

	if (mode & GFX_PRINT_SHADOW) {
		uint8 tmp;

		tmp = gc->foreground;
		gc->foreground = gc->background;

		gfxPrintExact(gc, txt, x + 1, y + 1);

		gc->foreground = tmp;
	}

	gfxPrintExact(gc, txt, x, y);
}

/*******************************************************************
 * refresh...
 */

/* kopiert aktuelles Bild in den RefreshRP */
void gfxPrepareRefresh(void) {
	memcpy(RefreshRPInMem.pixels, Screen->getPixels(), SCREEN_SIZE);
}

/* kopiert aktuellen Inhalt des RefreshRP in den Bildschirmspeicher */
void gfxRefresh(void) {
	memcpy(Screen->getPixels(), RefreshRPInMem.pixels, SCREEN_SIZE);

	gfxRefreshArea(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

/*******************************************************************
 * presentation
 */

void gfxRAWBlit(uint8 *sp, uint8 *dp, const int x1, const int y1, const int x2,
                const int y2, const int w, const int h, const int sw,
                const int dw) {
	const int ds = sw - w, dd = dw - w;
	int x, y;

	sp += (y1 * sw) + x1;
	dp += (y2 * dw) + x2;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			*dp++ = *sp++;
		}
		sp += ds;
		dp += dd;
	}
}

static GC *gfxGetGC(int32 l_DestY) {
	GC *gc = NULL;

	switch (GfxBase.uch_VideoMode) {
	case GFX_VIDEO_NCH4:
		gc = u_gc;

		if (l_DestY >= 60) {
			gc = l_gc;

			if (l_DestY >= gc->clip.y + gc->clip.h)
				gc = m_gc;
		}
		break;

	default:
	case GFX_VIDEO_MCGA:
		gc = u_gc;

		if (l_DestY >= gc->clip.y + gc->clip.h) {
			gc = l_gc;

			if (l_DestY >= gc->clip.y + gc->clip.h)
				gc = m_gc;
		}
		break;
	}

	return gc;
}

static uint32 timeLeft(uint32 interval) {
	static uint32 next_time = 0;
	uint32 now;

	now = g_system->getMillis();
	if (next_time <= now) {
		next_time = now + interval;
		return (0);
	}
	return (next_time - now);
}


void gfxWaitTOF(void) {
	g_system->delayMillis(timeLeft(40));
}

void gfxWaitTOR(void) {
	g_system->delayMillis(timeLeft(20));
}

void gfxWaitTOS(void) {
	Common::Event ev;
	uint32 next_time;
	const uint32 interval = 250;
	uint32 now = g_system->getMillis();

	next_time = now + interval;
	while (now < next_time) {
		inpWaitFor(INP_ALL_MODES);
		g_system->updateScreen();
		g_system->delayMillis(10);
		now = g_system->getMillis();
	}
}

void gfxClearArea(GC *gc) {
	Common::Rect area;

	if (gc) {
		area.left = gc->clip.x;
		area.top = gc->clip.y;
		area.setWidth(gc->clip.w);
		area.setHeight(gc->clip.h);
	} else {
		area.left = 0;
		area.top = 0;
		area.setWidth(SCREEN_WIDTH);
		area.setHeight(SCREEN_HEIGHT);
	}
	Screen->fillRect(area, 0);

	gfxRefreshArea(area.left, area.top, area.width(), area.height());
}

void gfxSetRGB(GC *gc, uint8 color, uint8 r, uint8 g, uint8 b) {
	byte rgb[] = { r, g, b };

	g_system->getPaletteManager()->setPalette(rgb, color, 1);

	g_system->updateScreen();
}

void gfxSetColorRange(byte uch_ColorStart, byte uch_End) {
	GlobalColorRange.uch_Start = uch_ColorStart;
	GlobalColorRange.uch_End = uch_End;
}

void gfxSetRGBRange(uint8 *colors, uint32 start, uint32 num) {
	g_system->getPaletteManager()->setPalette(colors, start, num);

	g_system->updateScreen();
}

void gfxGetPaletteFromReg(uint8 *palette, uint32 start, uint32 num) {
	g_system->getPaletteManager()->grabPalette(palette, start, num);
}

void gfxChangeColors(GC *gc, uint32 delay, uint32 mode, uint8 *palette)
/* l_Delay is min 1 */
{
	uint16 t, st, en;
	byte rgb[GFX_PALETTE_SIZE];
	uint8 cols[GFX_PALETTE_SIZE];
	int32 time, fakt, s;
	byte back[3];

	if (gc) {
		st = gc->colorStart;
		en = gc->End;
	} else {
		st = GlobalColorRange.uch_Start;
		en = GlobalColorRange.uch_End;
	}
	gfxRealRefreshArea(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	delay = MAX(delay, 1u); /* delay must not be zero! */

	time = delay;

	gfxGetPaletteFromReg(back, 0, 1);

	switch (mode) {
	case GFX_FADE_OUT:

		gfxGetPaletteFromReg(cols, 0, 256);

		fakt = 128 / time;

		for (s = time; s >= 0; s--) {
			gfxWaitTOR();

			for (t = st; t <= en; t++) {
				rgb[t * 3 + 0] = back[0] + (((int32)cols[t * 3 + 0] - back[0]) * (fakt * s)) / 128;
				rgb[t * 3 + 1] = back[1] + (((int32)cols[t * 3 + 1] - back[1]) * (fakt * s)) / 128;
				rgb[t * 3 + 2] = back[2] + (((int32)cols[t * 3 + 2] - back[2]) * (fakt * s)) / 128;
			}
			gfxSetRGBRange(&rgb[st * 3], st, en - st + 1);
		}

		for (t = st; t <= en; t++) {
			rgb[t * 3 + 0] = back[0];
			rgb[t * 3 + 1] = back[1];
			rgb[t * 3 + 2] = back[2];
		}
		gfxSetRGBRange(&rgb[st * 3], st, en - st + 1);
		break;

	case GFX_BLEND_UP:

		fakt = 128 / time;


		for (s = 0; s <= time; s++) {
			gfxWaitTOR();

			for (t = st; t <= en; t++) {
				rgb[t * 3 + 0] = back[0] + (((int32)palette[t * 3 + 0] - back[0]) * (fakt * s)) / 128;
				rgb[t * 3 + 1] = back[1] + (((int32)palette[t * 3 + 1] - back[1]) * (fakt * s)) / 128;
				rgb[t * 3 + 2] = back[2] + (((int32)palette[t * 3 + 2] - back[2]) * (fakt * s)) / 128;
			}
			gfxSetRGBRange(&rgb[st * 3], st, en - st + 1);
		}

		gfxWaitTOR();

		for (t = st; t <= en; t++) {
			rgb[t * 3 + 0] = palette[t * 3 + 0];
			rgb[t * 3 + 1] = palette[t * 3 + 1];
			rgb[t * 3 + 2] = palette[t * 3 + 2];
		}
		gfxSetRGBRange(&rgb[st * 3], st, en - st + 1);
		break;
	}
}


void gfxShow(uint16 us_PictId, uint32 ul_Mode, int32 l_Delay, int32 l_XPos, int32 l_YPos) {
	struct Picture *pict;
	struct Collection *coll;
	GC *gc;
	uint16 destX;
	uint16 destY;

	if (!(pict = gfxGetPicture(us_PictId)))
		return;
	if (!(coll = gfxGetCollection(pict->us_CollId)))
		return;

	destX = pict->us_DestX;
	destY = pict->us_DestY;

	if (l_XPos != -1)
		destX = l_XPos;

	if (l_YPos != -1)
		destY = l_YPos;

	if (GfxBase.gc)
		gc = GfxBase.gc;
	else
		gc = gfxGetGC(destY);

	destY = gfxGetRealDestY(gc, destY);

	gfxPrepareColl(pict->us_CollId);

	if (!(ul_Mode & GFX_NO_REFRESH))
		gfxPrepareRefresh();

	if (ul_Mode & GFX_CLEAR_FIRST)
		gfxClearArea(gc);

	if (ul_Mode & GFX_FADE_OUT) {
		gfxSetColorRange(coll->uch_ColorRangeStart, coll->uch_ColorRangeEnd);
		gfxChangeColors(NULL, l_Delay, GFX_FADE_OUT, NULL);
	}

	if (!l_Delay && (ul_Mode & GFX_BLEND_UP)) {
		gfxSetColorRange(coll->uch_ColorRangeStart,
		                 coll->uch_ColorRangeEnd);
		gfxChangeColors(NULL, l_Delay, GFX_BLEND_UP, ScratchRP.palette);

	}

	gfxScreenFreeze();
	if (ul_Mode & GFX_OVERLAY)
		gfxBlit(gc, &ScratchRP, pict->us_XOffset, pict->us_YOffset,
		        destX, destY, pict->us_Width, pict->us_Height, true);
	if (ul_Mode & GFX_ONE_STEP)
		gfxBlit(gc, &ScratchRP, pict->us_XOffset, pict->us_YOffset,
		        destX, destY, pict->us_Width, pict->us_Height, false);

	if (l_Delay && (ul_Mode & GFX_BLEND_UP)) {
		gfxSetColorRange(coll->uch_ColorRangeStart,
		                 coll->uch_ColorRangeEnd);
		gfxChangeColors(NULL, l_Delay, GFX_BLEND_UP, ScratchRP.palette);
	}
	gfxScreenThaw(gc, destX, destY, pict->us_Width, pict->us_Height);

	gfxSetGC(NULL);
}

/*******************************************************************
 * misc
 */


int32 gfxGetILBMSize(struct Collection *coll) {
	uint16 w = coll->us_TotalWidth;
	uint16 h = coll->us_TotalHeight;
	int32 size;

	w = ((w + 15) & 0xfff0);    /* round up to a int16 */

	size = w * h;

	return size;
}

static void gfxSetCMAP(const uint8 *src) {
	/* look for CMAP chunk */
	while (memcmp(src, "CMAP", 4) != 0)
		src++;

	src += 4;           /* skip CMAP chunk */
	src += 4;           /* skip size of CMAP chunk */

	memcpy(ScratchRP.palette, src, GFX_PALETTE_SIZE);
}

static void MakeMCGA(uint16 b, uint8 *pic, uint16 PlSt, int16 c) {
	uint16 bit;
	int16 i;

	bit = 0x80;
	for (i = 0; i < 8; i++) {
		if ((b & bit) && (c > i)) {
			pic[i] |= PlSt;
		}
		bit >>= 1;
	}
}

static uint16 MemRead_U16BE(const uint8 *p) {
	return ((uint16) p[1] | ((uint16) p[0] << 8));
}

void gfxILBMToRAW(const uint8 *src, uint8 *dst, size_t size) {
	uint16 bpp = 0, w = 0, h = 0;
	const uint8 *sp;
	uint8 *pic, *pic1;
	int s, t, b, x;
	uint16 a, y;
	uint16 flag;

	sp = src;
	pic = dst;

	sp += 8;

	if (memcmp(sp, "ILBM", 4) != 0) {
		return;
	}

	sp += 12;

	w = MemRead_U16BE(sp);
	sp += 2;
	h = MemRead_U16BE(sp);
	sp += 6;
	bpp = *sp;
	sp += 2;
	flag = *sp;

	h = MIN(h, (uint16)192);

	while (memcmp(sp, "BODY", 4) != 0) {
		sp++;
	}

	sp += 8;

	memset(pic, 0, size);

	if (flag) {
		pic1 = pic;
		for (t = 0; t < h; t++) {
			for (s = 0; s < bpp; s++) {
				pic = pic1; /* Anfang der aktuellen Zeile */
				b = ((w + 15) & 0xfff0);
				do {
					a = *sp;    /* Kommando (wiederholen oder übernehmen */
					sp++;   /* nächstes Zeichen */
					if (a > 128) {  /* Zeichen wiederholen */

						a = 257 - a;

						y = *sp;
						sp++;
						for (x = 1; x <= a; x++) {
							MakeMCGA(y, pic, (1 << s), b);
							pic += 8;
							b -= 8;
						}
					} else {    /* Zeichen übernehmen */

						for (x = 0; x <= a; x++) {
							y = *sp;
							sp++;
							MakeMCGA(y, pic, (1 << s), b);
							pic += 8;
							b -= 8;
						}
					}
				} while (b > 0);
			}
			pic1 += SCREEN_WIDTH;
		}
	} else {
		pic1 = pic;
		for (t = 0; t < h; t++) {
			for (s = 0; s < bpp; s++) {
				pic = pic1; /* Anfang der aktuellen Zeile */
				b = ((w + 15) & 0xfff0);
				do {
					y = *sp;
					sp++;
					MakeMCGA(y, pic, (1 << s), b);
					pic += 8;
					b -= 8;
				} while (b > 0);
			}
			pic1 += SCREEN_WIDTH;
		}
	}
}

size_t XMSOffset = 0;
uint8 *XMSHandle;


static const char *names[5] = {
	"an1_1.anm",
	"an2_4.anm",
	"an3_11.anm",
	"an4_11.anm",
	"an5_11.anm"
};

int frames[5] = { 9, 67, 196, 500, 180 };
int rate[5] = { 60, 17, 7, 7, 7 };


#define MaxAnm 6

int sync[MaxAnm * 2] = {
	0, 1,
	0, 5,
	1, 2,
	1, 27,
	1, 39,
	1, 53
};

static const char *fname[MaxAnm] = {
	"church.voc",
	"church.voc",
	"church.voc",
	"church.voc",
	"church.voc",
	"church.voc"
};

#define MAX_TRACKS 24
int CDFrames[MAX_TRACKS * 6] = {
	/* Anim#, Frame#, (0..full frame; 1..partial frame), Frame Start, Frame End */

	2, 100, 0, 3, 0, 0,
	2, 143, 0, 4, 0, 0,
	3, 5, 0, 23, 0, 0,
	3, 74, 0, 8, 0, 0,
	3, 90, 0, 24, 0, 0,
	3, 118, 0, 9, 0, 0,
	3, 137, 0, 14, 0, 0,
	3, 177, 0, 19, 0, 0,
	3, 205, 0, 10, 0, 0,
	3, 222, 0, 15, 0, 0,
	3, 242, 0, 20, 0, 0,
	3, 260, 0, 16, 0, 0,
	3, 293, 0, 21, 0, 0,
	3, 327, 0, 11, 0, 0,
	3, 344, 0, 17, 0, 0,
	3, 375, 0, 5, 0, 0,
	3, 427, 1, 12, 0, 2 * 75,
	3, 445, 1, 12, 3 * 75, 5 * 75,
	3, 467, 1, 12, 8 * 75, 11 * 75,
	4, 5, 0, 18, 0, 0,
	4, 44, 0, 22, 0, 0,
	4, 66, 0, 6, 0, 0,
	4, 115, 0, 13, 0, 0,
	4, 153, 0, 7, 0, 0
};

static uint32 Amg2Pc(uint32 s) {
	return ((s & 255) << 24) +
	       (((s >> 8) & 255) << 16) +
	       (((s >> 16) & 255) << 8) + (((s >> 24) & 255));
}


static void orbyte(uint8 *ptr, uint8 data, uint8 dmask) {
	uint8 dmaskoff = ~dmask;

	if (0x80 & data) *ptr++ |= dmask;
	else *ptr++ &= dmaskoff;
	if (0x40 & data) *ptr++ |= dmask;
	else *ptr++ &= dmaskoff;
	if (0x20 & data) *ptr++ |= dmask;
	else *ptr++ &= dmaskoff;
	if (0x10 & data) *ptr++ |= dmask;
	else *ptr++ &= dmaskoff;
	if (0x08 & data) *ptr++ |= dmask;
	else *ptr++ &= dmaskoff;
	if (0x04 & data) *ptr++ |= dmask;
	else *ptr++ &= dmaskoff;
	if (0x02 & data) *ptr++ |= dmask;
	else *ptr++ &= dmaskoff;
	if (0x01 & data) *ptr   |= dmask;
	else *ptr   &= dmaskoff;
}


static void ProcessAnimation(uint8 *dp, uint8 *sp) {
	uint32 size, rsize, *lp;
	uint8 *me, *me1, *me2, *st;
	uint32 offs[8];
	uint8 plane, planeMask;
	int i, j, col;

	/* skip header */
	XMSOffset += 4;
	/* get size */
	memcpy(&size, XMSHandle + XMSOffset, 4);
	XMSOffset += 4;

	rsize = Amg2Pc(size);

	me = dp;

	sp = XMSHandle + XMSOffset;
	XMSOffset += rsize;

	while (memcmp(sp, "DLTA", 4) != 0) {
		sp++;
	}

	sp += 8;
	lp = (uint32 *)sp;
	st = sp;            /* start of DLTA block */

	for (i = 0; i < 8; i++)
		offs[i] = Amg2Pc(lp[i]);

	me2 = me;

	for (plane = 0, planeMask = 1; plane < 8; plane++, planeMask <<= 1) {
		if (offs[plane] != 0) {
			sp = st + offs[plane];
			me1 = me2;

			for (col = 0; col < 40; col++) {
				uint8 op_cnt;

				me = me1;
				op_cnt = *sp++;

				for (i = 0; i < op_cnt; i++) {
					uint8 op;

					op = *sp++;

					if (op == 0) {
						/* same op */
						uint8 cnt;
						uint8 val;

						cnt = *sp++;
						val = *sp++;

						for (j = 0; j < cnt; j++) {
							orbyte(me, val, planeMask);
							me += SCREEN_WIDTH;
						}
					} else {
						if ((op & 0x80)) {
							/* uniq op */
							uint8 cnt;

							cnt = op & 0x7f;
							for (j = 0; j < cnt; j++) {
								uint8 val;

								val = *sp++;
								orbyte(me, val, planeMask);
								me += SCREEN_WIDTH;
							}
						} else {
							/* skip op */
							me += (SCREEN_WIDTH * op);
						}
					}

				}
				me1 += 8;   /* next column */
			}
		}
	}
}

void ShowIntro(void) {
	uint8 *cp;
	char head[4];
	int t, s, anims;
	size_t size, rsize;
	uint8 colorTABLE[GFX_PALETTE_SIZE];
	bool endi = false;
	GC ScreenGC;
	MemRastPort A, B;

	XMSHandle = (uint8 *)malloc(818 * 1024);

	/******************************** Init Gfx ********************************/
	gfxInitMemRastPort(&A, SCREEN_WIDTH, SCREEN_HEIGHT);
	gfxInitMemRastPort(&B, SCREEN_WIDTH, SCREEN_HEIGHT);

	gfxInitGC(&ScreenGC,
	          0, 0, 320, 200,
	          0, 255,
	          NULL);
	gfxSetColorRange(0, 255);

	memset(colorTABLE, 0, sizeof(colorTABLE));
	gfxSetRGBRange(colorTABLE, 0, 256);

	for (anims = 0; anims < 5; anims++) {
		FILE *fp;
		char pathName[DSK_PATH_MAX];
		bool showA;

		if (setup.CDAudio) {
			CDROM_StopAudioTrack();
		}

		if (!dskBuildPathName(DISK_CHECK_FILE, "intropix", names[anims], pathName)) {
			continue;
		}

		fp = dskOpen(pathName, "rb");

		if (fp) {
			XMSOffset = 0;

			fread(&head[0], 1, 4, fp);
			fread(&size, 1, 4, fp);
			rsize = Amg2Pc(size);

			dskRead(fp, XMSHandle, rsize);
			dskClose(fp);

			/* skip header */
			XMSOffset += 4;

			cp = XMSHandle + XMSOffset;

			/* skip header */
			XMSOffset += 4;
			/* get size */
			memcpy(&size, XMSHandle + XMSOffset, 4);
			XMSOffset += 4;

			rsize = Amg2Pc(size);

			XMSOffset += rsize;

			gfxChangeColors(NULL, 20, GFX_BLEND_UP, colorTABLE);
			gfxClearArea(NULL);

			/* copy from file to A & B */
			gfxSetCMAP(cp);
			gfxILBMToRAW(cp, ScratchRP.pixels, SCREEN_SIZE);
			gfxScratchToMem(&A);
			gfxScratchToMem(&B);

			for (t = 0, showA = true; t < frames[anims]; t++, showA = !showA) {
				Common::Event ev;

				while (g_system->getEventManager()->pollEvent(ev)) {
					switch (ev.type) {
					case Common::EVENT_KEYDOWN:
					case Common::EVENT_LBUTTONDOWN:
					case Common::EVENT_RBUTTONDOWN:
						endi = true;
						goto endit;

					case Common::EVENT_QUIT:
						tcDone();
						g_system->quit(); // TODO: Hack
						break;

					default:
						break;
					}
				}

				gfxScreenFreeze();

				if (showA) {
					gfxBlit(&ScreenGC, &A, 0, 0, 0, 0,
					        SCREEN_WIDTH, SCREEN_HEIGHT, false);
				} else {
					gfxBlit(&ScreenGC, &B, 0, 0, 0, 0,
					        SCREEN_WIDTH, SCREEN_HEIGHT, false);
				}

				if (t == 0) {
					gfxChangeColors(NULL, 20, GFX_BLEND_UP, ScratchRP.palette);
				}

				gfxScreenThaw(&ScreenGC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


				for (s = 0; s < rate[anims]; s++) {
					gfxWaitTOF();
				}


				if (showA) {
					ProcessAnimation(B.pixels, cp);
				} else {
					ProcessAnimation(A.pixels, cp);
				}

				for (s = 0; s < MaxAnm; s++) {
					if (sync[s * 2] == anims && sync[s * 2 + 1] - 1 == t) {
						sndPrepareFX(fname[s]);
						sndPlayFX();
					}
				}

				if (anims == 1 && t == 62) {
					sndPlaySound("title.bk", 0);
				}

				if (setup.CDAudio) {
#if 0
					for (s = 0; s < MAX_TRACKS; s++) {
						if ((CDFrames[s * 6] == anims)
						        && (CDFrames[s * 6 + 1] == t)) {
							NEOSOUNDSYSTEM(3, 16, 0x1f);        /* CMD 3: Fade to 0x1f */

							CDROM_StopAudioTrack();
							if (CDFrames[s * 6 + 2] == 0)
								CDROM_PlayAudioTrack((unsigned
								                      char)CDFrames[s * 6 + 3]);
							else
								CDROM_PlayAudioSequence((unsigned char) CDFrames[s * 6 + 3], (unsigned long) CDFrames[s * 6 + 4], (unsigned long) CDFrames[s * 6 + 5]);
						}
					}
#endif
				}

				endi = false;
			}

endit:
			gfxChangeColors(NULL, 1, GFX_FADE_OUT, colorTABLE);

			if (endi) {
				goto endit2;
			}
		}
	}

endit2:
	gfxClearArea(NULL);

	if (setup.CDAudio) {
		CDROM_StopAudioTrack();
		sndFading(0);
	}

	gfxDoneMemRastPort(&A);
	gfxDoneMemRastPort(&B);

	free(XMSHandle);
}

void gfxInitMemRastPort(MemRastPort *rp, uint16 width, uint16 height) {
	rp->w = width;
	rp->h = height;

	memset(rp->palette, 0, GFX_PALETTE_SIZE);

	rp->pixels = (uint8 *)TCAllocMem(width * height, true);
	rp->collId = GFX_NO_COLL_IN_MEM;
}

void gfxDoneMemRastPort(MemRastPort *rp) {
	TCFreeMem(rp->pixels, rp->w * rp->h);
	rp->pixels = NULL;
}

void gfxScratchFromMem(MemRastPort *src) {
	if (src) {
		memcpy(ScratchRP.palette, src->palette, GFX_PALETTE_SIZE);
		memcpy(ScratchRP.pixels, src->pixels, src->w * src->h);
	}
}

void gfxScratchToMem(MemRastPort *dst) {
	if (dst) {
		memcpy(dst->palette, ScratchRP.palette, GFX_PALETTE_SIZE);
		memcpy(dst->pixels, ScratchRP.pixels, dst->w * dst->h);
	}
}


void gfxBlit(GC *gc, MemRastPort *src, uint16 sx, uint16 sy, uint16 dx, uint16 dy,
             uint16 w, uint16 h, bool has_mask) {
	Rectangle srcR, srcR2, dstR, dstR2, areaR;
	Graphics::Surface *dst;
	uint8 *dp, *sp;
	uint16 x, y;

	/* clip. */
	srcR.x = 0;
	srcR.y = 0;
	srcR.w = src->w;
	srcR.h = src->h;

	srcR2.x = sx;
	srcR2.y = sy;
	srcR2.w = w;
	srcR2.h = h;
	srcR = Clip(srcR, srcR2);

	if (srcR.w <= 0 || srcR.h <= 0)
		return;

	dstR.x = gc->clip.x;
	dstR.y = gc->clip.y;
	dstR.w = gc->clip.w;
	dstR.h = gc->clip.h;

	dstR2.x = gc->clip.x + dx;
	dstR2.y = gc->clip.y + dy;
	dstR2.w = w;
	dstR2.h = h;
	dstR = Clip(dstR, dstR2);

	if (dstR.w <= 0 || dstR.h <= 0)
		return;

	/* blit. */
	areaR.x = dstR.x;
	areaR.y = dstR.y;
	areaR.w = MIN(dstR.w, srcR.w);
	areaR.h = MIN(dstR.h, srcR.h);

	dst = Screen;

	dp = (uint8 *)dst->getPixels();
	sp = src->pixels;

	dp += dstR.y * SCREEN_WIDTH + dstR.x;
	sp += srcR.y * src->w + srcR.x;

	w = areaR.w;
	h = areaR.h;

	if (has_mask) {
		for (y = 0; y < h; y++) {
			for (x = 0; x < w; x++) {
				if (sp[x] != 0) {
					dp[x] = sp[x];
				}
			}
			dp += SCREEN_WIDTH;
			sp += src->w;
		}
	} else {
		for (y = 0; y < h; y++) {
			memcpy(dp, sp, w);
			dp += SCREEN_WIDTH;
			sp += src->w;
		}
	}

	gfxRefreshArea(areaR.x, areaR.y, areaR.w, areaR.h);
}

static int screen_freeze_count = 0;

/* ZZZ */
void gfxRealRefreshArea(uint16 x, uint16 y, uint16 w, uint16 h) {
	Common::Rect dst;
	Rectangle areaR, areaR2;

	areaR.x = 0;
	areaR.y = 0;
	areaR.w = SCREEN_WIDTH;
	areaR.h = SCREEN_HEIGHT;

	areaR2.x = x;
	areaR2.y = y;
	areaR2.w = w;
	areaR2.h = h;
	areaR = Clip(areaR, areaR2);

	x = areaR.x;
	y = areaR.y;
	w = areaR.w;
	h = areaR.h;

	dst.left = x;
	dst.top = y;
	dst.setWidth(w);
	dst.setHeight(h);

	Graphics::Surface *screen = g_system->lockScreen();
	screen->copyRectToSurface(*Screen, x, y, dst);
	g_system->unlockScreen();

	g_system->updateScreen();
}


void gfxRefreshArea(uint16 x, uint16 y, uint16 w, uint16 h) {
	if (screen_freeze_count == 0) {
		gfxRealRefreshArea(x, y, w, h);
	}
}

void gfxScreenFreeze(void) {
	screen_freeze_count++;
}

void gfxScreenThaw(GC *gc, uint16 x, uint16 y, uint16 w, uint16 h) {
	if (screen_freeze_count > 0) {
		Rectangle dstR, dstR2;

		screen_freeze_count--;

		dstR.x = gc->clip.x;
		dstR.y = gc->clip.y;
		dstR.w = gc->clip.w;
		dstR.h = gc->clip.h;

		dstR2.x = gc->clip.x + x;
		dstR2.y = gc->clip.y + y;
		dstR2.w = w;
		dstR2.h = h;
		dstR = Clip(dstR, dstR2);

		if (dstR.w > 0 && dstR.h > 0) {
			gfxRefreshArea(dstR.x, dstR.y, dstR.w, dstR.h);
		}
	}
}

void MemBlit(MemRastPort *src, Rect *src_rect,
             MemRastPort *dst, Rect *dst_rect, ROpE op) {
	Rectangle srcR, srcR2, dstR, dstR2, areaR;
	uint16 sw, dw, x, y;
	register uint8 *dp, *sp;

	/* clip. */
	srcR.x = 0;
	srcR.y = 0;
	srcR.w = src->w;
	srcR.h = src->h;

	srcR2.x = src_rect->x;
	srcR2.y = src_rect->y;
	srcR2.w = src_rect->w;
	srcR2.h = src_rect->h;
	srcR = Clip(srcR, srcR2);

	if (srcR.w <= 0 || srcR.h <= 0)
		return;

	dstR.x = 0;
	dstR.y = 0;
	dstR.w = dst->w;
	dstR.h = dst->h;

	dstR2.x = dst_rect->x;
	dstR2.y = dst_rect->y;
	dstR2.w = dst_rect->w;
	dstR2.h = dst_rect->h;
	dstR = Clip(dstR, dstR2);

	if (dstR.w <= 0 || dstR.h <= 0)
		return;

	areaR.x = dstR.x;
	areaR.y = dstR.y;
	areaR.w = MIN(dstR.w, srcR.w);
	areaR.h = MIN(dstR.h, srcR.h);

	/* blit. */
	sw = src->w;
	dw = dst->w;

	dp = dst->pixels;
	sp = src->pixels;

	dp += dstR.y * dw + dstR.x;
	sp += srcR.y * sw + srcR.x;

	switch (op) {
	case GFX_ROP_BLIT:
		for (y = 0; y < areaR.h; y++) {
			memcpy(dp, sp, areaR.w);
			dp += dw;
			sp += sw;
		}
		break;

	case GFX_ROP_MASK_BLIT:
		for (y = 0; y < areaR.h; y++) {
			for (x = 0; x < areaR.w; x++) {
				if (sp[x] != 0) {
					dp[x] = sp[x];
				}
			}
			dp += dw;
			sp += sw;
		}
		break;

	case GFX_ROP_CLR:
		for (y = 0; y < areaR.h; y++) {
			for (x = 0; x < areaR.w; x++) {
				dp[x] &= ~sp[x];
			}
			dp += dw;
			sp += sw;
		}
		break;

	case GFX_ROP_SET:
		for (y = 0; y < areaR.h; y++) {
			for (x = 0; x < areaR.w; x++) {
				dp[x] |= sp[x];
			}
			dp += dw;
			sp += sw;
		}
		break;
	}
}

void gfxGetMouseXY(GC *gc, uint16 *pMouseX, uint16 *pMouseY) {
	Common::Point mouse = g_engine->getEventManager()->getMousePos();

	if (pMouseX) {
		if (mouse.x < gc->clip.x)
			mouse.x = 0;
		else
			mouse.x -= gc->clip.x;
		*pMouseX = mouse.x;
	}

	if (pMouseY) {
		if (mouse.y < gc->clip.y)
			mouse.y = 0;
		else
			mouse.y -= gc->clip.y;
		*pMouseY = mouse.y;
	}
}


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


#include "common/events.h"
#include "common/system.h"
#include "engines/util.h"
#include "graphics/palette.h"

#include "clue/base/base.h"
#include "clue/clue.h"
#include "clue/gfx/gfx.h"
#include "clue/gfx/gfx_p.h"
#include "clue/planing/player.h"

namespace Clue {
PictureNode::PictureNode() {
	_pictId = 0;
	_collId = 0;
	
	_xOffset = _yOffset = 0;
	_width = _height = 0;
	_destX = _destY = 0;
}

Font::Font(const char *fileName, uint16 width, uint16 height, uint8 firstChar, uint8 lastChar, uint16 sw, uint16 sh) {
	_width = width;
	_height = height;

	_firstChar = firstChar;
	_lastChar = lastChar;

	char path[DSK_PATH_MAX];
	dskBuildPathName(DISK_CHECK_FILE, PICTURE_DIRECTORY, fileName, path);

	uint8 *lbm = (uint8 *)dskLoad(path);

	_bmp = new Graphics::Surface();
	_bmp->create(sw, sh, ScreenFormat);
	uint32 size = sw * sh;

	gfxILBMToRAW(lbm, (uint8 *)_bmp->getPixels(), size);

	free(lbm);
}

Font::~Font() {
	delete _bmp;
}

_GC::~_GC() {
	delete _font;
}

	
void gfxRealRefreshArea(uint16 x, uint16 y, uint16 w, uint16 h);

/********************************************************************
 * inits & dons
 */

	
void gfxInit() {
	ScreenFormat = Graphics::PixelFormat::createFormatCLUT8();
	initGraphics(SCREEN_WIDTH, SCREEN_HEIGHT);

	Screen = new Graphics::Surface();
	Screen->create(SCREEN_WIDTH, SCREEN_HEIGHT, ScreenFormat);

	gfxSetGC(nullptr);

	/* diese RP müssen nur ein Bild maximaler Größe aufnehmen können */
	/* in anderen Modulen wird vorausgesetzt, daß alle RastPorts gleich */
	/* groß sind und auch gleich groß wie die StdBuffer sind */
	/* StdBuffer = 61 * 1024 = 62464, Mem: 62400 */

	/* Ausnahme (nachträglich) : der RefreshRP ist nur 320 * 140 Pixel groß!! */
	StdRP0InMem = new MemRastPort(SCREEN_WIDTH, SCREEN_HEIGHT); /* CMAP muß auch Platz haben ! */
	StdRP1InMem = new MemRastPort(SCREEN_WIDTH, SCREEN_HEIGHT);
	AnimRPInMem = new MemRastPort(SCREEN_WIDTH, SCREEN_HEIGHT);
	AddRPInMem = new MemRastPort(SCREEN_WIDTH, SCREEN_HEIGHT);
	LSObjectRPInMem = new MemRastPort(SCREEN_WIDTH, SCREEN_HEIGHT);
	LSFloorRPInMem = new MemRastPort(SCREEN_WIDTH, 32);

	/* der RefreshRP muß den ganzen Bildschirm aufnehmen können */
	RefreshRPInMem = new MemRastPort(SCREEN_WIDTH, SCREEN_HEIGHT);
	ScratchRP = new MemRastPort(SCREEN_WIDTH, SCREEN_HEIGHT);
	BobRPInMem = new MemRastPort(SCREEN_WIDTH, SCREEN_HEIGHT);
	LSRPInMem = new MemRastPort(LS_MAX_AREA_WIDTH, LS_MAX_AREA_HEIGHT);

	bubbleFont = new Font(GFX_BUBBLE_FONT_NAME, 4, 8, 32, 255, SCREEN_WIDTH, 24);
	menuFont = new Font(GFX_MENU_FONT_NAME, 5, 9, 32, 255, SCREEN_WIDTH, 36);

	LowerGC = new _GC(0, 0, 320, 140, 0, 191, bubbleFont);
	MenuGC = new _GC(0, 140, 320, 60, 191, 255, menuFont);
	LSUpperGC = new _GC(0, 0, 320, 128, 0, 191, bubbleFont);
	LSMenuGC = new _GC(0, 128, 320, 72, 191, 255, menuFont);

	gfxInitCollList();
	gfxInitPictList();

	gfxSetVideoMode(GFX_VIDEO_MCGA);    /* after rastports !! */
}

void gfxDone() {
	if (_pictureList) {
		_pictureList->removeList();
		_pictureList = nullptr;
	}

	if (_collectionList) {
		_collectionList->removeList();
		_collectionList = nullptr;
	}

	delete bubbleFont;
	delete menuFont;
	bubbleFont = menuFont = nullptr;

	delete StdRP0InMem;
	delete StdRP1InMem;
	delete AnimRPInMem;
	delete AddRPInMem;
	delete LSObjectRPInMem;
	delete LSFloorRPInMem;

	delete RefreshRPInMem;
	delete ScratchRP;
	delete BobRPInMem;
	delete LSRPInMem;

	StdRP0InMem = StdRP1InMem = AnimRPInMem = AddRPInMem = LSObjectRPInMem = LSFloorRPInMem = nullptr;
	RefreshRPInMem = ScratchRP = BobRPInMem = LSRPInMem = nullptr;

	if (Screen) {
		Screen->free();
		Screen = nullptr;
	}
}

void gfxSetGC(_GC *gc) {
	GfxBase.gc = gc;
}

void gfxSetVideoMode(byte _newMode) {
	GfxBase._videoMode = _newMode;

	switch (_newMode) {
	case GFX_VIDEO_MCGA:
		_lowerGc = LowerGC;
		_upperGc = LowerGC;
		_menuGc = MenuGC;
		break;

	case GFX_VIDEO_NCH4:
		_lowerGc = LowerGC;
		_upperGc = LSUpperGC;
		_menuGc = LSMenuGC;

		gfxLSInit();
		break;

	default:
	case GFX_VIDEO_TEXT:
		return;
	}

	gfxClearScreen();
}

/********************************************************************
 * lists
 */

static void gfxInitCollList() {
	NewList<NewNode> *tempList = new NewList<NewNode>;
	_collectionList = new NewList<CollectionNode>;

	char pathname[DSK_PATH_MAX];
	dskBuildPathName(DISK_CHECK_FILE, TEXT_DIRECTORY, COLL_LIST_TXT, pathname);
	tempList->readList(pathname);

	for (NewNode *n = tempList->getListHead(); n->_succ; n = n->_succ) {
		CollectionNode *coll = _collectionList->createNode(g_clue->_txtMgr->getKey(2, n->_name.c_str()));

		coll->_collId = (uint16)g_clue->_txtMgr->getKeyAsUint32(1, n->_name);

		coll->_filename = coll->_name;
		coll->_prepared = nullptr;

		coll->_totalWidth = (uint16)g_clue->_txtMgr->getKeyAsUint32(3, n->_name);
		coll->_totalHeight = (uint16)g_clue->_txtMgr->getKeyAsUint32(4, n->_name);

		coll->_colorRangeStart = (uint16)g_clue->_txtMgr->getKeyAsUint32(5, n->_name);
		coll->_colorRangeEnd = (uint16)g_clue->_txtMgr->getKeyAsUint32(6, n->_name);
	}

	tempList->removeList();
}

static void gfxInitPictList() {
	NewList<NewNode> *tempList = new NewList<NewNode>;
	_pictureList = new NewList<PictureNode>;

	char pathname[DSK_PATH_MAX];
	dskBuildPathName(DISK_CHECK_FILE, TEXT_DIRECTORY, PICT_LIST_TXT, pathname);
	tempList->readList(pathname);

	for (NewNode *n = tempList->getListHead(); n->_succ; n = n->_succ) {
		PictureNode *pict = _pictureList->createNode(nullptr);

		pict->_pictId = (uint16)g_clue->_txtMgr->getKeyAsUint32(1, n->_name);
		pict->_collId = (uint16)g_clue->_txtMgr->getKeyAsUint32(2, n->_name);

		pict->_xOffset = (uint16)g_clue->_txtMgr->getKeyAsUint32(3, n->_name);
		pict->_yOffset = (uint16)g_clue->_txtMgr->getKeyAsUint32(4, n->_name);

		pict->_width = (uint16)g_clue->_txtMgr->getKeyAsUint32(5, n->_name);
		pict->_height = (uint16)g_clue->_txtMgr->getKeyAsUint32(6, n->_name);

		pict->_destX = (uint16)g_clue->_txtMgr->getKeyAsUint32(7, n->_name);
		pict->_destY = (uint16)g_clue->_txtMgr->getKeyAsUint32(8, n->_name);
	}

	tempList->removeList();
}

struct Rectangle {
	int x;
	int y;
	int w;
	int h;
};

/*
 * Clips the rectangle B against rectangle A.
 */
static Rectangle Clip(Rectangle A, Rectangle B) {
	Rectangle C = { 0, 0, -1, -1 };
	if (B.x >= A.x + A.w)
		return C;

	if (B.x + B.w <= A.x)
		return C;

	if (B.y >= A.y + A.h)
		return C;

	if (B.y + B.h <= A.y)
		return C;

	int dist = A.x - B.x;
	if (dist > 0) {
		B.x += dist;
		B.w -= dist;
	}
	dist = (B.x + B.w) - (A.x + A.w);
	if (dist > 0)
		B.w -= dist;

	dist = A.y - B.y;
	if (dist > 0) {
		B.y += dist;
		B.h -= dist;
	}
	dist = (B.y + B.h) - (A.y + A.h);
	if (dist > 0)
		B.h -= dist;

	return B;
}

/********************************************************************
 * Rastports...
 */

_GC::_GC(uint16 x, uint16 y, uint16 w, uint16 h, uint8 colorStart, uint8 End, Font *font) {
	Rectangle dstR;
	dstR.x = 0;
	dstR.y = 0;
	dstR.w = SCREEN_WIDTH;
	dstR.h = SCREEN_HEIGHT;

	Rectangle dstR2;
	dstR2.x = x;
	dstR2.y = y;
	dstR2.w = w;
	dstR2.h = h;
	dstR = Clip(dstR, dstR2);

	if (dstR.w <= 0 || dstR.h <= 0)
		DebugMsg(ERR_ERROR, ERROR_MODULE_GFX, "gfxInitGC");

	_clipRect.x = dstR.x;
	_clipRect.y = dstR.y;
	_clipRect.w = dstR.w;
	_clipRect.h = dstR.h;

	_gfxMode    = GFX_JAM_1;

	_foreground  = colorStart + 1;
	_background  = colorStart;
	_outline     = colorStart + 1;

	_colorStart  = colorStart;
	_colorEnd    = End;

	moveCursor(0, 0);

	_font = font;
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

void _GC::draw(uint16 x, uint16 y) {
	/* this function doesn't perform clipping properly... but that's ok! */
	x += _clipRect.x;
	y += _clipRect.y;

	if (x < _clipRect.w && y < _clipRect.h) {
		uint16 rx = x;
		uint16 ry = y;

		uint16 rx1 = _cursorX;
		uint16 ry1 = _cursorY;

		uint16 dx, sx;
		if (rx < rx1) {
			sx = rx;
			dx = rx1 - rx + 1;
		} else {
			sx = rx1;
			dx = rx - rx1 + 1;
		}

		uint16 dy, sy;
		if (ry < ry1) {
			sy = ry;
			dy = ry1 - ry + 1;
		} else {
			sy = ry1;
			dy = ry - ry1 + 1;
		}

		uint8 color = _foreground;
		Graphics::Surface *dst = Screen;

		uint16 dw = dst->w;

		uint8 *dp = (uint8 *)dst->getPixels();
		dp += sy * dw + sx;

		if (rx == rx1) {
			for (uint16 i = 0; i < dy; i++) {
				*dp = color;
				dp += dw;
			}
		} else if (ry == ry1) {
			memset(dp, color, dx);
		} else {
			/* the line is slanted. oops. */
		}

		_cursorX = x;
		_cursorY = y;

		if (rx == rx1)
			gfxRefreshArea(sx, 1, sy, dy);
		else if (ry == ry1)
			gfxRefreshArea(sx, dx, sy, 1);
		else {
			/* the line is slanted. oops. */
			DebugMsg(ERR_DEBUG, ERROR_MODULE_GFX, "gfxDraw");
		}
	}
}

void _GC::moveCursor(uint16 x, uint16 y) {
	_cursorX = _clipRect.x + MIN(x, uint16(_clipRect.w - 1));
	_cursorY = _clipRect.y + MIN(y, uint16(_clipRect.h - 1));
}

void _GC::setPens(uint8 foreground, uint8 background, uint8 outline) {
	if (foreground != GFX_SAME_PEN)
		_foreground = foreground;

	if (background != GFX_SAME_PEN)
		_background = background;

	if (outline != GFX_SAME_PEN)
		_outline = outline;
}

void _GC::rectFill(uint16 sx, uint16 sy, uint16 ex, uint16 ey) {
/* minimum size: 3 * 3 pixels ! */
	if (sx > ex)
		SWAP(sx, ex);

	if (sy > ey)
		SWAP(sy, ey);

	Common::Rect dst;
	dst.left = _clipRect.x + sx;
	dst.top = _clipRect.y + sy;
	dst.right = _clipRect.x + ex + 1;
	dst.bottom = _clipRect.y + ey + 1;

	Screen->fillRect(dst, _foreground);
	Screen->frameRect(dst, _outline);

	gfxRefreshArea(dst.left, dst.top, dst.width(), dst.height());
}

void _GC::setMode(GfxDrawModeE mode) {
	_gfxMode = mode;
}

void _GC::setFont(Font *font) {
	_font = font;
}

/* Compute the length of a text in pixels */
uint16 _GC::gfxTextWidth(Common::String txt) {
	size_t w = txt.size() * _font->_width;

	if (w > USHRT_MAX)
		return 0;

	return w;
}

/*******************************************************************
 * access & calc functions
 */

CollectionNode *gfxGetCollection(uint16 us_CollId) {
	return _collectionList->getNthNode((uint32)(us_CollId - 1));
}

PictureNode *gfxGetPicture(uint16 us_PictId) {
	return _pictureList->getNthNode((uint32)(us_PictId - 1));
}

/* the collection must have been prepared (PrepareColl) beforehand */
void gfxGetPalette(uint16 collId, uint8 *palette) {
	memcpy(palette, ScratchRP->_palette, GFX_PALETTE_SIZE);
}

static int32 gfxGetRealDestY(_GC *gc, int32 destY) {
	destY -= gc->_clipRect.y;

	return destY;
}

/*******************************************************************
 * collections...
 */

/* nach dieser Funktion befindet sich im ScratchRP die entpackte Collection */
void gfxPrepareColl(uint16 collId) {
	CollectionNode *coll = gfxGetCollection(collId);
	if (!coll) {
		DebugMsg(ERR_DEBUG, ERROR_MODULE_GFX, "gfxPrepareColl");
		return;
	}

	if (coll->_prepared) {
		coll->_prepared->gfxScratchFromMem();
	} else {
		char pathname[DSK_PATH_MAX];

		/* Dateiname erstellen */
		dskBuildPathName(DISK_CHECK_FILE, PICTURE_DIRECTORY, coll->_filename.c_str(), pathname);

		gfxLoadILBM(pathname);

		/*
		     * coll->prepared wird nicht mit dem ScratchRP initialisert, da
		 * es sonst zu Inkonsistenzen kommen könnte. Collections im Scratch
		 * werden als nicht vorbereitet betrachtet, da der ScratchRP ständig
		 * durch andere Bilder überschrieben wird
		     */
		coll->_prepared = nullptr;
	}
}

void gfxLoadILBM(const char *fileName) {
	/* Collection laden */
	uint8 *lbm = (uint8 *)dskLoad(fileName);

	gfxSetCMAP(lbm);
	gfxILBMToRAW(lbm, ScratchRP->_pixels, SCREEN_SIZE);
	free(lbm);
}

void gfxUnPrepareColl(uint16 collId) {
	/* dummy function */
}

void gfxCollToMem(uint16 collId, MemRastPort *rp) {
	/*
	 * wenn sich in diesem MemRastPort ein anderes Bild befindet so wird dieses
	 * nun aus dem MemRastPort "entfernt"
	 */
	if (rp->_collId != GFX_NO_COLL_IN_MEM && collId != rp->_collId) {
		CollectionNode *oldColl = gfxGetCollection(rp->_collId);
		if (oldColl)
			oldColl->_prepared = nullptr;
	}

	gfxPrepareColl(collId);
	rp->gfxScratchToMem();

	/* enter the MemRastPort in the new collection */
	CollectionNode *coll = gfxGetCollection(collId);
	if (coll)
		coll->_prepared = rp;

	rp->_collId = collId;
}

/*******************************************************************
 * print functions
 */

void gfxSetRect(uint16 us_X, uint16 us_Width) {
	GlobalPrintRect.us_X = us_X;
	GlobalPrintRect.us_Width = us_Width;
}

void _GC::ScreenBlitChar(Graphics::Surface *src, Rect *src_rect, Graphics::Surface *dst, Rect *dst_rect, uint8 color) {
	/* clip. */
	Rectangle srcR;
	srcR.x = src_rect->x;
	srcR.y = src_rect->y;
	srcR.w = src_rect->w;
	srcR.h = src_rect->h;

	Rectangle dstR;
	dstR.x = 0;
	dstR.y = 0;
	dstR.w = dst->w;
	dstR.h = dst->h;

	Rectangle dstR2;
	dstR2.x = dst_rect->x + _clipRect.x;
	dstR2.y = dst_rect->y + _clipRect.y;
	dstR2.w = _clipRect.w;
	dstR2.h = _clipRect.h;
	dstR = Clip(dstR, dstR2);

	if (dstR.w <= 0 || dstR.h <= 0)
		return;

	/* blit. */
	Rectangle areaR;
	areaR.x = dstR.x;
	areaR.y = dstR.y;
	areaR.w = MIN(dstR.w, srcR.w);
	areaR.h = MIN(dstR.h, srcR.h);

	uint8 *dp = (uint8 *)dst->getPixels();
	uint8 *sp = (uint8 *)src->getPixels();

	dp += areaR.y * dst->w + areaR.x;
	sp += srcR.y * src->w + srcR.x;

	uint16 h = areaR.h;

	while (h--) {
		for (uint16 x = 0; x < areaR.w; x++) {
			if (sp[x] != 0) {
				dp[x] = color;
			}
		}
		dp += dst->pitch;
		sp += src->pitch;
	}
}

void _GC::gfxPrintExact(Common::String txt, uint16 x, uint16 y) {
	if (txt.empty())
		return;

	const Font *font = _font;
	const uint16 w = font->_width,
	             h = font->_height,
	             base = font->_firstChar;
	const uint16 chars_per_line = SCREEN_WIDTH / w;
	const uint8 fg = _foreground;
	uint len = txt.size();

	Common::Rect area;
	area.left = _clipRect.x;
	area.top = _clipRect.y;
	area.setWidth(_clipRect.w);
	area.setHeight(_clipRect.h);

	area.left += x;
	area.top += y;
	area.setWidth(len * w);
	area.setHeight(h);

	if (_gfxMode == GFX_JAM_2)
		Screen->fillRect(area, _background);

	Rect srcR;
	srcR.w = w;
	srcR.h = h;

	Rect dstR;
	dstR.x = x;
	dstR.y = y;

	Graphics::Surface *src = font->_bmp;
	Graphics::Surface *dst = Screen;

	for (size_t t = 0; t < len; t++) {
		uint16 ch = (uint8)txt[t];
		ch -= base;

		srcR.y = (ch / chars_per_line) * h;
		srcR.x = (ch % chars_per_line) * w;

		ScreenBlitChar(src, &srcR, dst, &dstR, fg);

		dstR.x += w;
	}

	gfxRefreshArea(area.left, area.top, area.width(), area.height());
}

void _GC::gfxPrint(Common::String txt, uint16 y, uint32 mode) {
	if (txt.empty())
		return;

	uint16 x = GlobalPrintRect.us_X;
	uint16 w = gfxTextWidth(txt);

	if (mode & GFX_PRINT_RIGHT)
		x += GlobalPrintRect.us_Width - w;

	if (mode & GFX_PRINT_CENTER) {
		if (w <= GlobalPrintRect.us_Width)
			x += (GlobalPrintRect.us_Width - w) / 2;
	}

	if (mode & GFX_PRINT_SHADOW) {
		uint8 tmp = _foreground;
		_foreground = _background;

		gfxPrintExact(txt, x + 1, y + 1);

		_foreground = tmp;
	}

	gfxPrintExact(txt, x, y);
}
/*******************************************************************
 * refresh...
 */

/* kopiert aktuelles Bild in den RefreshRP */
void gfxPrepareRefresh() {
	memcpy(RefreshRPInMem->_pixels, Screen->getPixels(), SCREEN_SIZE);
}

/* kopiert aktuellen Inhalt des RefreshRP in den Bildschirmspeicher */
void gfxRefresh() {
	memcpy(Screen->getPixels(), RefreshRPInMem->_pixels, SCREEN_SIZE);

	gfxRefreshArea(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

/*******************************************************************
 * presentation
 */
static _GC *gfxGetGC(int32 l_DestY) {
	_GC *gc = nullptr;

	switch (GfxBase._videoMode) {
	case GFX_VIDEO_NCH4:
		gc = _upperGc;

		if (l_DestY >= 60) {
			gc = _lowerGc;

			if (l_DestY >= gc->_clipRect.y + gc->_clipRect.h)
				gc = _menuGc;
		}
		break;

	default:
	case GFX_VIDEO_MCGA:
		gc = _upperGc;

		if (l_DestY >= gc->_clipRect.y + gc->_clipRect.h) {
			gc = _lowerGc;

			if (l_DestY >= gc->_clipRect.y + gc->_clipRect.h)
				gc = _menuGc;
		}
		break;
	}

	return gc;
}

void wfr() {
// fueür Bildschirmsync verwenden!!!
	g_system->delayMillis(10);
	// TODO: Only refresh if screen changed
	g_system->updateScreen();
}

void wfd() {
// sperrt im Gegensatz zu wfr die Interrupts nicht
// wenn die Musik laeuft kann es daher vorkommen, dass
// dieses wfd einmal aussetzt
// !!! NICHT fueür Bildschirmsync verwenden!!
	wfr();
}

void _GC::gfxClearArea() {
	Common::Rect area;

	area.left = _clipRect.x;
	area.top = _clipRect.y;
	area.setWidth(_clipRect.w);
	area.setHeight(_clipRect.h);

	Screen->fillRect(area, 0);

	gfxRefreshArea(area.left, area.top, area.width(), area.height());
}

void gfxClearScreen() {
	Common::Rect area;

	area.left = 0;
	area.top = 0;
	area.setWidth(SCREEN_WIDTH);
	area.setHeight(SCREEN_HEIGHT);

	Screen->fillRect(area, 0);

	gfxRefreshArea(area.left, area.top, area.width(), area.height());
}

void gfxSetRGB(_GC *gc, uint8 color, uint8 r, uint8 g, uint8 b) {
	byte rgb[] = { r, g, b };

	g_system->getPaletteManager()->setPalette(rgb, color, 1);

	g_system->updateScreen();
}

void gfxSetColorRange(byte uch_ColorStart, byte uch_End) {
	GlobalColorRange._startCol = uch_ColorStart;
	GlobalColorRange._endCol = uch_End;
}

void gfxSetRGBRange(uint8 *colors, uint32 start, uint32 num) {
	g_system->getPaletteManager()->setPalette(colors, start, num);

	g_system->updateScreen();
}

void gfxGetPaletteFromReg(uint8 *palette, uint32 start, uint32 num) {
	g_system->getPaletteManager()->grabPalette(palette, start, num);
}

void gfxChangeColors(_GC *gc, uint32 delay, uint32 mode, uint8 *palette) {
	uint16 st, en;
	if (gc) {
		st = gc->_colorStart;
		en = gc->_colorEnd;
	} else {
		st = GlobalColorRange._startCol;
		en = GlobalColorRange._endCol;
	}
	gfxRealRefreshArea(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	delay = MAX(delay, 1u); /* delay must not be zero! */
	inpSetWaitTicks(delay);

	int32 time = delay;
	byte back[3];
	gfxGetPaletteFromReg(back, 0, 1);

	byte rgb[GFX_PALETTE_SIZE];
	uint8 cols[GFX_PALETTE_SIZE];
	int32 fakt;
	switch (mode) {
	case GFX_FADE_OUT:
		gfxGetPaletteFromReg(cols, 0, 256);
		fakt = 128 / time;

		for (int32 s = time; s >= 0; s--) {
			inpWaitFor(INP_TIME);

			for (uint16 t = st; t <= en; t++) {
				rgb[t * 3 + 0] = back[0] + (((int32)cols[t * 3 + 0] - back[0]) * (fakt * s)) / 128;
				rgb[t * 3 + 1] = back[1] + (((int32)cols[t * 3 + 1] - back[1]) * (fakt * s)) / 128;
				rgb[t * 3 + 2] = back[2] + (((int32)cols[t * 3 + 2] - back[2]) * (fakt * s)) / 128;
			}
			gfxSetRGBRange(&rgb[st * 3], st, en - st + 1);
		}

		for (uint16 t = st; t <= en; t++) {
			rgb[t * 3 + 0] = back[0];
			rgb[t * 3 + 1] = back[1];
			rgb[t * 3 + 2] = back[2];
		}
		gfxSetRGBRange(&rgb[st * 3], st, en - st + 1);
		break;

	case GFX_BLEND_UP:

		fakt = 128 / time;
		for (int32 s = 0; s <= time; s++) {
			inpWaitFor(INP_TIME);

			for (uint16 t = st; t <= en; t++) {
				rgb[t * 3 + 0] = back[0] + (((int32)palette[t * 3 + 0] - back[0]) * (fakt * s)) / 128;
				rgb[t * 3 + 1] = back[1] + (((int32)palette[t * 3 + 1] - back[1]) * (fakt * s)) / 128;
				rgb[t * 3 + 2] = back[2] + (((int32)palette[t * 3 + 2] - back[2]) * (fakt * s)) / 128;
			}
			gfxSetRGBRange(&rgb[st * 3], st, en - st + 1);
		}

		inpWaitFor(INP_TIME);

		for (uint16 t = st; t <= en; t++) {
			rgb[t * 3 + 0] = palette[t * 3 + 0];
			rgb[t * 3 + 1] = palette[t * 3 + 1];
			rgb[t * 3 + 2] = palette[t * 3 + 2];
		}
		gfxSetRGBRange(&rgb[st * 3], st, en - st + 1);
		break;
	}
}


void gfxShow(uint16 us_PictId, uint32 ul_Mode, int32 l_Delay, int32 l_XPos, int32 l_YPos) {
	PictureNode *pict = gfxGetPicture(us_PictId);
	if (!pict)
		return;

	CollectionNode *coll = gfxGetCollection(pict->_collId);
	if (!coll)
		return;

	uint16 destX = pict->_destX;
	uint16 destY = pict->_destY;

	if (l_XPos != -1)
		destX = l_XPos;

	if (l_YPos != -1)
		destY = l_YPos;

	_GC *gc;
	if (GfxBase.gc)
		gc = GfxBase.gc;
	else
		gc = gfxGetGC(destY);

	destY = gfxGetRealDestY(gc, destY);

	gfxPrepareColl(pict->_collId);

	if (!(ul_Mode & GFX_NO_REFRESH))
		gfxPrepareRefresh();

	if (ul_Mode & GFX_CLEAR_FIRST)
		gc->gfxClearArea();

	if (ul_Mode & GFX_FADE_OUT) {
		gfxSetColorRange(coll->_colorRangeStart, coll->_colorRangeEnd);
		gfxChangeColors(nullptr, l_Delay, GFX_FADE_OUT, NULL);
	}

	if (!l_Delay && (ul_Mode & GFX_BLEND_UP)) {
		gfxSetColorRange(coll->_colorRangeStart, coll->_colorRangeEnd);
		gfxChangeColors(nullptr, l_Delay, GFX_BLEND_UP, ScratchRP->_palette);
	}

	gfxScreenFreeze();
	if (ul_Mode & GFX_OVERLAY)
		gc->gfxBlit(ScratchRP, pict->_xOffset, pict->_yOffset, destX, destY, pict->_width, pict->_height, true);

	if (ul_Mode & GFX_ONE_STEP)
		gc->gfxBlit(ScratchRP, pict->_xOffset, pict->_yOffset, destX, destY, pict->_width, pict->_height, false);

	if (l_Delay && (ul_Mode & GFX_BLEND_UP)) {
		gfxSetColorRange(coll->_colorRangeStart, coll->_colorRangeEnd);
		gfxChangeColors(nullptr, l_Delay, GFX_BLEND_UP, ScratchRP->_palette);
	}
	gc->gfxScreenThaw(destX, destY, pict->_width, pict->_height);

	gfxSetGC(nullptr);
}

/*******************************************************************
 * misc
 */
void gfxSetCMAP(const uint8 *src) {
	/* look for CMAP chunk */
	while (memcmp(src, "CMAP", 4) != 0)
		src++;

	src += 4;           /* skip CMAP chunk */
	src += 4;           /* skip size of CMAP chunk */

	memcpy(ScratchRP->_palette, src, GFX_PALETTE_SIZE);
}

static void MakeMCGA(uint16 b, uint8 *pic, uint16 PlSt, int16 c) {
	uint16 bit = 0x80;
	for (int16 i = 0; i < 8; i++) {
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

	const uint8 *sp = src;
	uint8 *pic = dst;

	sp += 8;

	if (memcmp(sp, "ILBM", 4) != 0)
		return;

	sp += 12;

	w = MemRead_U16BE(sp);
	sp += 2;
	h = MemRead_U16BE(sp);
	sp += 6;
	bpp = *sp;
	sp += 2;

	uint16 flag = *sp;

	h = MIN(h, (uint16)192);

	while (memcmp(sp, "BODY", 4) != 0) {
		sp++;
	}

	sp += 8;

	memset(pic, 0, size);

	if (flag) {
		uint8 *pic1 = pic;
		for (int t = 0; t < h; t++) {
			for (int s = 0; s < bpp; s++) {
				pic = pic1; /* Anfang der aktuellen Zeile */
				int b = ((w + 15) & 0xfff0);
				do {
					uint16 a = *sp;    /* Kommando (wiederholen oder übernehmen */
					sp++;   /* nächstes Zeichen */
					if (a > 128) {  /* Zeichen wiederholen */

						a = 257 - a;

						uint16 y = *sp;
						sp++;
						for (int x = 1; x <= a; x++) {
							MakeMCGA(y, pic, (1 << s), b);
							pic += 8;
							b -= 8;
						}
					} else {    /* Zeichen übernehmen */
						for (int x = 0; x <= a; x++) {
							uint16 y = *sp;
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
		uint8 *pic1 = pic;
		for (int t = 0; t < h; t++) {
			for (int s = 0; s < bpp; s++) {
				pic = pic1; /* Anfang der aktuellen Zeile */
				int b = ((w + 15) & 0xfff0);
				do {
					uint16 y = *sp;
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

MemRastPort::MemRastPort(uint16 width, uint16 height) {
	_width = width;
	_height = height;

	memset(_palette, 0, GFX_PALETTE_SIZE);

	_pixels = new uint8[width * height];
	memset(_pixels, 0, width * height);

	_collId = GFX_NO_COLL_IN_MEM;
}

MemRastPort::~MemRastPort() {
	delete[] _pixels;
}

void MemRastPort::gfxScratchFromMem() {
	memcpy(ScratchRP->_palette, _palette, GFX_PALETTE_SIZE);
	memcpy(ScratchRP->_pixels, _pixels, _width * _height);
}

void MemRastPort::gfxScratchToMem() {
	memcpy(_palette, ScratchRP->_palette, GFX_PALETTE_SIZE);
	memcpy(_pixels, ScratchRP->_pixels, _width * _height);
}


void _GC::gfxBlit(MemRastPort *src, uint16 sx, uint16 sy, uint16 dx, uint16 dy, uint16 w, uint16 h, bool has_mask) {
	/* clip. */
	Rectangle srcR;
	srcR.x = 0;
	srcR.y = 0;
	srcR.w = src->_width;
	srcR.h = src->_height;

	Rectangle srcR2;
	srcR2.x = sx;
	srcR2.y = sy;
	srcR2.w = w;
	srcR2.h = h;
	srcR = Clip(srcR, srcR2);

	if (srcR.w <= 0 || srcR.h <= 0)
		return;

	Rectangle dstR;
	dstR.x = _clipRect.x;
	dstR.y = _clipRect.y;
	dstR.w = _clipRect.w;
	dstR.h = _clipRect.h;

	Rectangle dstR2;
	dstR2.x = _clipRect.x + dx;
	dstR2.y = _clipRect.y + dy;
	dstR2.w = w;
	dstR2.h = h;
	dstR = Clip(dstR, dstR2);

	if (dstR.w <= 0 || dstR.h <= 0)
		return;

	/* blit. */
	Rectangle areaR;
	areaR.x = dstR.x;
	areaR.y = dstR.y;
	areaR.w = MIN(dstR.w, srcR.w);
	areaR.h = MIN(dstR.h, srcR.h);

	Graphics::Surface *dst = Screen;

	uint8 *dp = (uint8 *)dst->getPixels();
	uint8 *sp = src->_pixels;

	dp += dstR.y * SCREEN_WIDTH + dstR.x;
	sp += srcR.y * src->_width + srcR.x;

	w = areaR.w;
	h = areaR.h;

	if (has_mask) {
		for (uint16 y = 0; y < h; y++) {
			for (uint16 x = 0; x < w; x++) {
				if (sp[x] != 0) {
					dp[x] = sp[x];
				}
			}
			dp += SCREEN_WIDTH;
			sp += src->_width;
		}
	} else {
		for (uint16 y = 0; y < h; y++) {
			memcpy(dp, sp, w);
			dp += SCREEN_WIDTH;
			sp += src->_width;
		}
	}

	gfxRefreshArea(areaR.x, areaR.y, areaR.w, areaR.h);
}

static int screen_freeze_count = 0;

/* ZZZ */
void gfxRealRefreshArea(uint16 x, uint16 y, uint16 w, uint16 h) {
	Rectangle areaR;
	areaR.x = 0;
	areaR.y = 0;
	areaR.w = SCREEN_WIDTH;
	areaR.h = SCREEN_HEIGHT;

	Rectangle areaR2;
	areaR2.x = x;
	areaR2.y = y;
	areaR2.w = w;
	areaR2.h = h;
	areaR = Clip(areaR, areaR2);

	x = areaR.x;
	y = areaR.y;
	w = areaR.w;
	h = areaR.h;

	Common::Rect dst;
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

void gfxScreenFreeze() {
	screen_freeze_count++;
}

void _GC::gfxScreenThaw(uint16 x, uint16 y, uint16 w, uint16 h) {
	if (screen_freeze_count > 0) {
		screen_freeze_count--;

		Rectangle dstR;
		dstR.x = _clipRect.x;
		dstR.y = _clipRect.y;
		dstR.w = _clipRect.w;
		dstR.h = _clipRect.h;

		Rectangle dstR2;
		dstR2.x = _clipRect.x + x;
		dstR2.y = _clipRect.y + y;
		dstR2.w = w;
		dstR2.h = h;
		dstR = Clip(dstR, dstR2);

		if (dstR.w > 0 && dstR.h > 0)
			gfxRefreshArea(dstR.x, dstR.y, dstR.w, dstR.h);
	}
}

void MemBlit(MemRastPort *src, Rect *src_rect,
			 MemRastPort *dst, Rect *dst_rect, ROpE op) {
	/* clip. */
	Rectangle srcR;
	srcR.x = 0;
	srcR.y = 0;
	srcR.w = src->_width;
	srcR.h = src->_height;

	Rectangle srcR2;
	srcR2.x = src_rect->x;
	srcR2.y = src_rect->y;
	srcR2.w = src_rect->w;
	srcR2.h = src_rect->h;
	srcR = Clip(srcR, srcR2);

	if (srcR.w <= 0 || srcR.h <= 0)
		return;

	Rectangle dstR;
	dstR.x = 0;
	dstR.y = 0;
	dstR.w = dst->_width;
	dstR.h = dst->_height;

	Rectangle dstR2;
	dstR2.x = dst_rect->x;
	dstR2.y = dst_rect->y;
	dstR2.w = dst_rect->w;
	dstR2.h = dst_rect->h;
	dstR = Clip(dstR, dstR2);

	if (dstR.w <= 0 || dstR.h <= 0)
		return;

	Rectangle areaR;
	areaR.x = dstR.x;
	areaR.y = dstR.y;
	areaR.w = MIN(dstR.w, srcR.w);
	areaR.h = MIN(dstR.h, srcR.h);

	/* blit. */
	uint16 sw = src->_width;
	uint16 dw = dst->_width;

	uint8 *dp = dst->_pixels;
	uint8 *sp = src->_pixels;

	dp += dstR.y * dw + dstR.x;
	sp += srcR.y * sw + srcR.x;

	switch (op) {
	case GFX_ROP_BLIT:
		for (uint16 y = 0; y < areaR.h; y++) {
			memcpy(dp, sp, areaR.w);
			dp += dw;
			sp += sw;
		}
		break;

	case GFX_ROP_MASK_BLIT:
		for (uint16 y = 0; y < areaR.h; y++) {
			for (uint16 x = 0; x < areaR.w; x++) {
				if (sp[x] != 0) {
					dp[x] = sp[x];
				}
			}
			dp += dw;
			sp += sw;
		}
		break;

	case GFX_ROP_CLR:
		for (uint16 y = 0; y < areaR.h; y++) {
			for (uint16 x = 0; x < areaR.w; x++) {
				dp[x] &= ~sp[x];
			}
			dp += dw;
			sp += sw;
		}
		break;

	case GFX_ROP_SET:
		for (uint16 y = 0; y < areaR.h; y++) {
			for (uint16 x = 0; x < areaR.w; x++) {
				dp[x] |= sp[x];
			}
			dp += dw;
			sp += sw;
		}
		break;
	}
}

void _GC::gfxGetMouseXY(uint16 *pMouseX, uint16 *pMouseY) {
	Common::Point mouse = g_clue->getEventManager()->getMousePos();

	if (pMouseX) {
		if (mouse.x < _clipRect.x)
			mouse.x = 0;
		else
			mouse.x -= _clipRect.x;
		*pMouseX = mouse.x;
	}

	if (pMouseY) {
		if (mouse.y < _clipRect.y)
			mouse.y = 0;
		else
			mouse.y -= _clipRect.y;
		*pMouseY = mouse.y;
	}
}

#if 0
void gfxCorrectUpperRPBitmap() {
/* should be called after each scrolling (and before displaying a bubble) */
	/*
	    NCH4UpperRP.p_BitMap = (void *) ((uint32) GfxBoardBase + gfxNCH4GetCurrScrollOffset());
	*/
}

void gfxCollFromMem(uint16 collId) {
	MemRastPort *rp;

	Collection *coll = gfxGetCollection(collId);
	if (coll && (rp = coll->prepared)) {
		gfxScratchFromMem(rp);
	}
}

void gfxRAWBlit(uint8 *sp, uint8 *dp, const int x1, const int y1, const int x2,
	const int y2, const int w, const int h, const int sw,
	const int dw) {
		const int ds = sw - w, dd = dw - w;

		sp += (y1 * sw) + x1;
		dp += (y2 * dw) + x2;

		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				*dp++ = *sp++;
			}
			sp += ds;
			dp += dd;
		}
}

int32 gfxGetILBMSize(Collection *coll) {
	uint16 w = coll->us_TotalWidth;
	uint16 h = coll->us_TotalHeight;

	w = ((w + 15) & 0xfff0);    /* round up to a int16 */

	int32 size = w * h;

	return size;
}
#endif

} // End of namespace Clue


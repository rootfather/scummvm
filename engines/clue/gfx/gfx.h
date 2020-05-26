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

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_GFX
#define MODULE_GFX

#include "graphics/surface.h"
#include "clue/text.h"

namespace Clue {
#define GFX_NO_MEM_HANDLE   ((uint16) -1)

/* defines for gfxPrint */
#define GFX_PRINT_CENTER        (1)
#define GFX_PRINT_RIGHT         (1<<1)
#define GFX_PRINT_LEFT          (1<<2)
#define GFX_PRINT_SHADOW        (1<<3)

/* defines for SetDrMd */

enum GfxDrawModeE {
	GFX_JAM_1               = 0,    /* Hintergrund wird gleichbelassen */
	GFX_JAM_2               = 1     /* Hintergrund wird mit BackPen übermalt */
};

/* defines for Show */
#define GFX_NO_REFRESH             1
#define GFX_BLEND_UP               2
#define GFX_FADE_OUT               4
#define GFX_CLEAR_FIRST            8
#define GFX_OVERLAY               16
#define GFX_ONE_STEP              32    /* "plain" blit */
#define GFX_DONT_SHOW_FIRST_PIC   64

/* defines for gfxSetPens */
#define GFX_SAME_PEN                        ((byte)255)

#define GFX_NO_COLL_IN_MEM               USHRT_MAX

#define GFX_PALETTE_SIZE            768 /* 256 Farben * 3 Bytes */

#define  gfxWaitBOF                     wfd

#define GFX_VIDEO_MCGA                  1
#define GFX_VIDEO_NCH4                  2
#define GFX_VIDEO_TEXT                  3

class MemRastPort {
public:
	uint16 _width;
	uint16 _height;

	uint8  _palette [GFX_PALETTE_SIZE];
	uint8 *_pixels;

	uint16 _collId;                 /* Collection, die sich gerade hier befindet! */

	MemRastPort() { _pixels = nullptr; }
	MemRastPort(uint16 width, uint16 height);
	~MemRastPort();
	
	void gfxScratchToMem();
	void gfxScratchFromMem();
	void loadSpotBitMap();
};

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   200
#define SCREEN_SIZE     (SCREEN_WIDTH * SCREEN_HEIGHT)

void gfxCollToMem(uint16 collId, MemRastPort *rp);

struct Rect {
	uint16 x;
	uint16 y;

	uint16 w;
	uint16 h;
};

class CollectionNode : public NewNode {
public:
	uint16 _collId;
	Common::String _filename;

	MemRastPort *_prepared;
	void *_colorTable;     /* not always correct (only as long as nothing
			                   else modified the buffer!) */

	uint16 _totalWidth;
	uint16 _totalHeight;

	byte _colorRangeStart;
	byte _colorRangeEnd;

	CollectionNode() { /* TODO - Initialize members */ }
	~CollectionNode() { /* TODO - Implement destructor */ }
}; 

class PictureNode : public NewNode {
public:
	uint16 _pictId;
	uint16 _collId;       /* in welcher Collection sich dieses Bild befindet */

	uint16 _xOffset;      /* innerhalb der Collection */
	uint16 _yOffset;

	uint16 _width;
	uint16 _height;

	uint16 _destX;
	uint16 _destY;

	PictureNode();
	// No need for a specific destructor
};

class Font {
public:
	Graphics::Surface *_bmp;

	uint16 _width;                   /* width of one character */
	uint16 _height;                  /* height of one character */

	uint8 _firstChar;
	uint8 _lastChar;

	Font(const char *fileName, uint16 width, uint16 height, uint8 firstChar, uint8 lastChar, uint16 sw, uint16 sh);
	~Font();
};

class _GC {
public:
	Rect _clipRect;

	GfxDrawModeE _gfxMode;

	uint8 _foreground;              /* entspricht dem Farbregister */
	uint8 _background;              /* ebenfalls absolut und nicht relativ zum */
	uint8 _outline;                 /* Registerstart */

	uint8 _colorStart;
	uint8 _colorEnd;

	uint16 _cursorX;
	uint16 _cursorY;

	Font *_font;

	_GC(uint16 x, uint16 y, uint16 w, uint16 h, uint8 colorStart, uint8 End, Font* font);
	~_GC();

	void moveCursor(uint16 x, uint16 y);
	void draw(uint16 x, uint16 y);
	void setPens(uint8 foreground, uint8 background, uint8 outline);
	void rectFill(uint16 sx, uint16 sy, uint16 ex, uint16 ey);
	void setMode(GfxDrawModeE mode);
	void setFont(Font* font);
	void gfxPrint(Common::String txt, uint16 y, uint32 mode);
	uint16 gfxTextWidth(Common::String txt);
	void gfxPrintExact(Common::String txt, uint16 x, uint16 y);
	void ScreenBlitChar(Graphics::Surface* src, Rect* src_rect, Graphics::Surface* dst, Rect* dst_rect, uint8 color);
	void gfxClearArea();
	void gfxGetMouseXY(uint16* pMouseX, uint16* pMouseY);
	void gfxBlit(MemRastPort* src, uint16 sx, uint16 sy, uint16 dx, uint16 dy, uint16 w, uint16 h, bool has_mask);
	void gfxScreenThaw(uint16 x, uint16 y, uint16 w, uint16 h);
};

extern _GC *_lowerGc;
extern _GC *_upperGc;
extern _GC *_menuGc;

extern MemRastPort *ScratchRP;
extern MemRastPort *StdRP0InMem;
extern MemRastPort *StdRP1InMem;
extern MemRastPort *AnimRPInMem;
extern MemRastPort *AddRPInMem;
extern MemRastPort *LSFloorRPInMem;
extern MemRastPort *LSObjectRPInMem;
extern MemRastPort *BobRPInMem;
extern MemRastPort *LSRPInMem;

extern Font *bubbleFont;
extern Font *menuFont;

extern void gfxInit();
extern void gfxDone();

extern void gfxSetVideoMode(byte _newMode);

extern void wfd();
extern void wfr();

void gfxSetRGB(_GC* gc, uint8 color, uint8 r, uint8 g, uint8 b); // CHECKME: the first parameter isn't use. Why?

extern void gfxPrepareRefresh();
extern void gfxRefresh();

extern void gfxSetRect(uint16 us_X, uint16 us_Width);

extern void gfxSetColorRange(byte uch_ColorStart, byte uch_ColorEnd);

extern void gfxChangeColors(_GC *gc, uint32 delay, uint32 mode, uint8 *palette);
extern void gfxShow(uint16 us_PictId, uint32 ul_Mode, int32 l_Delay, int32 l_XPos, int32 l_YPos);

extern void gfxSetGC(_GC *gc);

extern void gfxGetPaletteFromReg(uint8 *palette, uint32 start, uint32 num);
extern void gfxGetPalette(uint16 collId, uint8 *palette);

void gfxPrepareColl(uint16 collId);
void gfxUnPrepareColl(uint16 collId);

extern CollectionNode *gfxGetCollection(uint16 us_CollId);
extern PictureNode *gfxGetPicture(uint16 us_PictId);

void gfxLoadILBM(const char *fileName);

void gfxRAWBlit(uint8 *sp, uint8 *dp, const int x1, const int y1, const int x2,
                const int y2, const int w, const int h, const int sw,
                const int dw);

enum ROpE {
	GFX_ROP_BLIT      = 0,
	GFX_ROP_MASK_BLIT = 1,
	GFX_ROP_CLR       = 2,
	GFX_ROP_SET       = 3
};

void MemBlit(MemRastPort *src, Rect *src_rect,
             MemRastPort *dst, Rect *dst_rect, ROpE op);

void gfxRefreshArea(uint16 x, uint16 y, uint16 w, uint16 h);

void gfxScreenFreeze();
void gfxScreenUnFreeze();

void gfxSetRGBRange(uint8 *colors, uint32 start, uint32 num);
void gfxSetCMAP(const uint8 *src);
void gfxILBMToRAW(const uint8 *src, uint8 *dst, size_t size);

void gfxClearScreen();

#if 0
extern int32 gfxGetILBMSize(struct Collection *coll);
void gfxCollFromMem(uint16 collId);
#define GFX_SIZE_OF_COLLECTION(c)   (gfxGetILBMSize(c) + GFX_COLORTABLE_SIZE)
extern void gfxCorrectUpperRPBitmap();
#endif
} // End of namespace Clue

#include "clue/gfx/gfxnch4.h"
#endif

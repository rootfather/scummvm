/*
**  $Filename: gfx/gfxnch4.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     20-04-94
**
**  basic functions for "Der Clou!"
**
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_GFXNCH4
#define MODULE_GFXNCH4

namespace Clue {

#define GFX_NCH4_SCROLLOFFSET   (160 * 72)  /* memory used by the fixed display */

#define ScrLEFT 0
#define ScrRIGHT 639
#define ScrTOP 0
#define ScrBOTTOM 255

struct RastPort;

extern uint32 gfxNCH4GetCurrScrollOffset(void);
void gfxSetDarkness(byte value);

void gfxNCH4Refresh(void);

extern void gfxNCH4SetViewPort(int x, int y);
extern void gfxNCH4Scroll(int x, int y);
extern void gfxNCH4SetSplit(uint16 line);
void gfxLSInit(void);

void gfxLSPut(MemRastPort *sp, uint16 sx, uint16 sy, uint16 dx, uint16 dy, uint16 w, uint16 h);
void gfxLSPutMsk(MemRastPort *sp, uint16 sx, uint16 sy, uint16 dx, uint16 dy, uint16 w, uint16 h);
void gfxLSPutClr(MemRastPort *sp, uint16 sx, uint16 sy, uint16 dx, uint16 dy, uint16 w, uint16 h);
void gfxLSPutSet(MemRastPort *sp, uint16 sx, uint16 sy, uint16 dx, uint16 dy, uint16 w, uint16 h);

void gfxLSRectFill(uint16 sx, uint16 sy, uint16 ex, uint16 ey, uint8 color);
uint8 gfxLSReadPixel(uint16 x, uint16 y);

} // End of namespace Clue

#endif

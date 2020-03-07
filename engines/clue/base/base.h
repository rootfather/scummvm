/*
**  $Filename: Base/Base.h
**  $Release:
**  $Revision:
**  $Date:
**
**
**
**  (C) 1993, 1994 ...and avoid panic by, H. Gaberschek
**      All Rights Reserved
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_BASE
#define MODULE_BASE

#include "clue/sound/hsc.h"
#include "clue/gameplay/gp.h"

namespace Clue {

/*
 *
 *
 */
struct Setup {
	bool Profidisk;
	bool CDRom;
	int32 SfxVolume;
	int32 MusicVolume;
	bool CDAudio;
};

extern struct Setup setup;

/*
 * Usage:
 * StdBuffer0: loaded picture
 *
 * StdBuffer1: uncompressed picture
 */

#define STD_BUFFER0_SIZE  (320 * 140)
#define STD_BUFFER1_SIZE  (61 * 1024)   /* smaller buffer size is impossible! (picture + CMAP (color map? - felsqualle)) */

extern char prgname[255];

extern void *StdBuffer0, *StdBuffer1;

#if 0
// TODO : Remove later when it's 100% sure it's unused
extern void tcClearStdBuffer(void *p_Buffer);
#endif

extern void tcSetPermanentColors(void);
void tcDone(void);
int clue_main(const char *path);

uint32 tcGetProcessorType(void);

#define PROC_80286      1L
#define PROC_80386      2L
#define PROC_80486      3L

} // End of namespace Clue

#endif

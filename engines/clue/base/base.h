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

#include "clue/gameplay/gp.h"

namespace Clue {
/*
 * Usage:
 * StdBuffer0: loaded picture
 *
 * StdBuffer1: uncompressed picture
 */

#define STD_BUFFER0_SIZE  (320 * 140)
#define STD_BUFFER1_SIZE  (61 * 1024)   /* smaller buffer size is impossible! (picture + CMAP (color map? - felsqualle)) */


extern void *StdBuffer0, *StdBuffer1;

extern void tcSetPermanentColors();
void tcDone();
int clue_main(const char *path);

} // End of namespace Clue

#endif

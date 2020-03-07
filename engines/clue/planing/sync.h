/*
**      $Filename: planing/sync.h
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.sync interface for "Der Clou!"
**
** (c) 1994 ...and avoid panic by, Kaweh Kazemi
**      All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_PLANING_SYNC
#define MODULE_PLANING_SYNC

#include "clue/landscap/landscap.h"
#include "clue/planing/graphics.h"
#include "clue/planing/io.h"
#include "clue/planing/prepare.h"
#include "clue/planing/support.h"
#include "clue/planing/system.h"
#include "clue/planing/guards.h"

namespace Clue {

/* Sync modes */
#define PLANING_ANIMATE_NO     (1)
#define PLANING_ANIMATE_STD    (1<<1)
#define PLANING_ANIMATE_FOCUS  (1<<2)


void plSync(byte animate, uint32 targetTime, uint32 times, byte direction);

} // End of namespace Clue

#endif

/*
**      $Filename: planing/player.h
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.player interface for "Der Clou!"
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

#ifndef MODULE_PLANING_PLAYER
#define MODULE_PLANING_PLAYER

#include "clue/planing/main.h"

namespace Clue {

int32 plPlayer(uint32 objId, uint32 actionTime, byte(*actionFunc)(uint32, uint32));

} // End of namespace Clue

#endif

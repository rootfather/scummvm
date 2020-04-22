/*
**      $Filename: planing/main.h
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.main interface for "Der Clou!"
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

#ifndef MODULE_PLANING_MAIN
#define MODULE_PLANING_MAIN

#include "clue/planing/sync.h"

namespace Clue {
class System;
	
/* Database offset & relations */
#define PLANING_DB_OFFSET              1300000
#define take_RelId                     PLANING_DB_OFFSET+1
#define hasLoot_Clone_RelId            PLANING_DB_OFFSET+2

extern System *plSys;

extern Common::String txtTooLoud;
extern Common::String txtTimer;
extern Common::String txtWeight;
extern Common::String txtSeconds;

void plInit();
void plDone();

} // End of namespace Clue

#endif

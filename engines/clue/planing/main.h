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

#include <stdio.h>
#include "clue/theclou.h"

#ifndef MODULE_MEMORY
#include "clue/memory/memory.h"
#endif

#ifndef MODULE_ERROR
#include "clue/error/error.h"
#endif

#ifndef MODULE_DISK
#include "clue/disk/disk.h"
#endif

#ifndef MODULE_LIST
#include "clue/list/list.h"
#endif

#ifndef MODULE_DATABASE
#include "clue/data/database.h"
#endif

#ifndef MODULE_RELATION
#include "clue/data/relation.h"
#endif

#include "clue/data/objstd/tcdata.h"

#ifndef MODULE_TEXT
#include "clue/text/text.h"
#endif

#ifndef MODULE_LANDSCAP
#include "clue/landscap/landscap.h"
#endif

#ifndef MODULE_PLANING_MAIN
#include "clue/planing/main.h"
#endif

#ifndef MODULE_PLANING_GRAPHICS
#include "clue/planing/graphics.h"
#endif

#ifndef MODULE_PLANING_IO
#include "clue/planing/io.h"
#endif

#ifndef MODULE_PLANING_PREPARE
#include "clue/planing/prepare.h"
#endif

#ifndef MODULE_PLANING_SUPPORT
#include "clue/planing/support.h"
#endif

#ifndef MODULE_PLANING_SYSTEM
#include "clue/planing/system.h"
#endif

#ifndef MODULE_PLANING_SYNC
#include "clue/planing/sync.h"
#endif

#ifndef MODULE_PLANING_GUARDS
#include "clue/planing/guards.h"
#endif

namespace Clue {

/* Database offset & relations */
#define PLANING_DB_OFFSET              1300000
#define take_RelId                     PLANING_DB_OFFSET+1
#define hasLoot_Clone_RelId            PLANING_DB_OFFSET+2


extern struct System *plSys;

extern char txtTooLoud[20];
extern char txtTimer[20];
extern char txtWeight[20];
extern char txtSeconds[20];


void plInit(void);
void plDone(void);

} // End of namespace Clue

#endif

/*
**      $Filename: planing/support.h
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.support interface for "Der Clou!"
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

#ifndef MODULE_PLANING_SUPPORT
#define MODULE_PLANING_SUPPORT

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


/* Area defines */
#define PLANING_AREA_PERSON  3
#define PLANING_AREA_CAR     16

/* functions for bits */
#define BIT(x)               (1<<(x))
#define CHECK_STATE(v,b)     ((v) & BIT(b))

/* Alarms & power control */
#define PLANING_ALARM_Z3          3
#define PLANING_ALARM_X5          8
#define PLANING_ALARM_TOP3        15
#define PLANING_POWER             1

/* all times in 1/3 sec */
#define PLANING_CORRECT_TIME      3


uint32 plGetNextLoot(void);

byte plLivingsPosAtCar(uint32 bldId);
byte plAllInCar(uint32 bldId);
byte plIsStair(uint32 objId);

void plCorrectOpened(LSObject obj, byte open);
byte plIgnoreLock(uint32 objId);

void plMove(uint32 current, byte direction);
void plWork(uint32 current);

LIST *plGetObjectsList(uint32 current, byte addLootBags);
void plInsertGuard(LIST *list, uint32 current, uint32 guard);
byte plObjectInReach(uint32 current, uint32 objId);
#endif

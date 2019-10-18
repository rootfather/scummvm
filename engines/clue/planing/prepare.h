/*
**      $Filename: planing/prepare.h
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.prepare interface for "Der Clou!"
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

#ifndef MODULE_PLANING_PREPARE
#define MODULE_PLANING_PREPARE

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


/* Gfx prepare modes */
#define PLANING_GFX_LANDSCAPE  (1)
#define PLANING_GFX_SPRITES    (1<<1)
#define PLANING_GFX_BACKGROUND (1<<2)

/* Sys modes */
#define PLANING_INIT_PERSONSLIST    (1)
#define PLANING_HANDLER_OPEN        (1<<1)
#define PLANING_HANDLER_CLOSE       (1<<2)
#define PLANING_HANDLER_CLEAR       (1<<3)
#define PLANING_HANDLER_SET         (1<<4)
#define PLANING_HANDLER_ADD         (1<<5)
#define PLANING_GUARDS_LOAD         (1<<6)

/* Counts */
#define PLANING_NR_PERSONS          4
#define PLANING_NR_GUARDS           4
#define PLANING_NR_LOOTS            8
#define PLANING_NR_PLANS            NRBLINES-1

/* getting correct has loot relation */
#define hasLoot(current)            (((LSArea)dbGetObject(livWhereIs(Planing_Name[current])))->ul_ObjectBaseNr + REL_HAS_LOOT_OFFSET)


extern LIST *PersonsList;
extern LIST *BurglarsList;

extern byte PersonsNr;
extern byte BurglarsNr;
extern uint32 CurrentPerson;

extern uint32 Planing_Weight[PLANING_NR_PERSONS];
extern uint32 Planing_Volume[PLANING_NR_PERSONS];

extern byte Planing_Loot[PLANING_NR_LOOTS];
extern byte Planing_Guard[PLANING_NR_GUARDS];

extern char Planing_Name[PLANING_NR_PERSONS + PLANING_NR_GUARDS][20];

extern LIST *Planing_GuardRoomList[PLANING_NR_GUARDS];

extern uint32 Planing_BldId;



void plBuildHandler(NODE *n);
void plClearHandler(NODE *n);
void plCloseHandler(NODE *n);

void plPrepareData(void);
void plPrepareSprite(uint32 livNr, uint32 areaId);
void plPrepareNames(void);

void plPrepareGfx(uint32 objId, byte landscapMode, byte prepareMode);
void plPrepareRel(void);
void plPrepareSys(uint32 currPer, uint32 objId, byte sysMode);

void plUnprepareGfx(void);
void plUnprepareRel(void);
void plUnprepareSys(void);
#endif

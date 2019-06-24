/*
**      $Filename: planing/guards.h
**      $Release:  1
**      $Revision: 0
**      $Date:     24-04-94
**
**      planing.guards interface for "Der Clou!"
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

#ifndef MODULE_PLANING_GUARDS
#define MODULE_PLANING_GUARDS

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

/* method definition */
#define GUARDS_DO_SAVE     1
#define GUARDS_DO_LOAD     2

/* disk definition */
#define GUARD_DISK         0
#define GUARD_EXTENSION    ".gua"
#define GUARD_DIRECTORY    DATA_DIRECTORY


struct System;

struct _GC;

/* main method */
void grdDo(FILE * fh, struct System *sys, LIST * PersonsList, U32 BurglarsNr,
	   U32 PersonsNr, ubyte grdAction);

/* support */
ubyte grdAddToList(U32 bldId, LIST * l);
ubyte grdDraw(struct _GC *gc, U32 bldId, U32 areaId);

/* con- & destructor */
ubyte grdInit(FILE ** fh, char *mode, U32 bldId, U32 areaId);
void grdDone(FILE * fh);
#endif

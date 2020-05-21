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

#include "clue/planing/main.h"

namespace Clue {

class System;
class _GC;

/* method definition */
#define GUARDS_DO_SAVE     1
#define GUARDS_DO_LOAD     2

/* disk definition */
#define GUARD_DISK         0
#define GUARD_EXTENSION    ".gua"
#define GUARD_DIRECTORY    DATA_DIRECTORY

/* main method */
void grdDo(Common::Stream *fh, System *sys, NewObjectList<dbObjectNode> *PersonsList, uint32 BurglarsNr, uint32 PersonsNr, byte grdAction);

/* support */
bool grdAddToList(uint32 bldId, NewObjectList<dbObjectNode>*l);
bool grdDraw(_GC *gc, uint32 bldId, uint32 areaId);

/* con- & destructor */
bool grdInit(Common::Stream **fh, int RW, uint32 bldId, uint32 areaId);
void grdDone(Common::Stream *fh);

} // End of namespace Clue

#endif

/*
**      $Filename: planing/io.h
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.io interface for "Der Clou!"
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

#ifndef MODULE_PLANING_IO
#define MODULE_PLANING_IO

namespace Clue {

/* Extensions */
#define PLANING_PLAN_EXTENSION       ".pln"
#define PLANING_PLAN_LIST_EXTENSION  ".pll"

/* Disks */
#define PLANING_PLAN_DISK             0

/* Tool sequences in file */
#define PLANING_PLAN_TOOL_BEGIN_ID  "TOOB"  /* TOOl Begin */
#define PLANING_PLAN_TOOL_END_ID    "TOOE"  /* TOOl End */

/* open cmd */
#define PLANING_OPEN_READ_PLAN        0
#define PLANING_OPEN_WRITE_PLAN       1
#define PLANING_OPEN_READ_BURGLARY    2

/* open error */
#define PLANING_OPEN_OK               0
#define PLANING_OPEN_ERR_NO_CHOICE    1
#define PLANING_OPEN_ERR_NO_PLAN      2


byte plOpen(uint32 objId, byte mode, Common::Stream **fh);
void plLoad(uint32 objId);
void plSave(uint32 objId);
void plSaveChanged(uint32 objId);

NewList<NewNode> *plLoadTools(Common::Stream *fh);
void plSaveTools(Common::Stream *fh);

} // End of namespace Clue

#endif

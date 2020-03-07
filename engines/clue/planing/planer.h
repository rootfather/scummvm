/*
**      $Filename: planing/planer.h
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.planer interface for "Der Clou!"
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

#ifndef MODULE_PLANING_PLANER
#define MODULE_PLANING_PLANER

namespace Clue {

extern byte AnimCounter;
extern bool PlanChanged;

void plPlaner(uint32 objId);

} // End of namespace Clue

#endif

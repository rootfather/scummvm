/*
**  $Filename: dialog/talkAppl.h
**  $Release:  0
**  $Revision: 0.1
**  $Date:     07-04-94
**
**  dialog functions for "Der Clou!"
**
**   (c) 1994 ...and avoid panic by, H. Gaberschek
**      All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_TALKAPPL
#define MODULE_TALKAPPL

#include "clue/data/dataappl.h"

namespace Clue {

extern void tcJobOffer(PersonNode *p);
extern void tcMyJobAnswer(PersonNode *p);
extern void tcPrisonAnswer(PersonNode *p);
extern void tcAbilityAnswer(uint32 personID);

} // End of namespace Clue

#endif

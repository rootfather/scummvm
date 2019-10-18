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

#include "clue/theclou.h"

#ifndef MODULE_LIST
#include "clue/list/list.h"
#endif

#ifndef MODULE_TEXT
#include "clue/text/text.h"
#endif

#ifndef MODULE_DIALOG
#include "clue/dialog/dialog.h"
#endif

#ifndef MODULE_RELATION
#include "clue/data/relation.h"
#endif

#ifndef MODULE_DATABASE
#include "clue/data/database.h"
#endif

#ifndef MODULE_DATACALC
#include "clue/data/datacalc.h"
#endif

#ifndef MODULE_DATAAPPL
#include "clue/data/dataappl.h"
#endif

extern void tcJobOffer(Person p);
extern void tcMyJobAnswer(Person p);
extern void tcPrisonAnswer(Person p);
extern void tcAbilityAnswer(uint32 personID);

#endif

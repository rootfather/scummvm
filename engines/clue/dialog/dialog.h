/*
**	$Filename: dialog/dialog.h
**	$Release:  0
**	$Revision: 0.1
**	$Date:     07-04-94
**
**	dialog functions for "Der Clou!"
**
**   (c) 1994 ...and avoid panic by, H. Gaberschek
**	    All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_DIALOG
#define MODULE_DIALOG

#include "clue/theclou.h"

#ifndef MODULE_LIST
#include "clue/list/list.h"
#endif

#ifndef MODULE_TEXT
#include "clue/text/text.h"
#endif

#ifndef MODULE_INTERAC
#include "clue/present/interac.h"
#endif

#include "clue/data/objstd/tcdata.h"

#ifndef MODULE_RELATION
#include "clue/data/relation.h"
#endif

#ifndef MODULE_DATABASE
#include "clue/data/database.h"
#endif

#ifndef MODULE_GAMEPLAY
#include "clue/gameplay/gp.h"
#endif

#ifndef MODULE_SOUND
#include "clue/sound/newsound.h"
#endif

#include "clue/cdrom/cdrom.h"

void PlayFromCDROM(void);

#define   DLG_TALKMODE_BUSINESS    1
#define   DLG_TALKMODE_STANDARD    2

extern void DynamicTalk(U32 Person1ID, U32 Person2ID, ubyte TalkMode);

extern ubyte Say(U32 TextID, ubyte activ, uword Person, const char *text);

extern U32 Talk(void);

#endif

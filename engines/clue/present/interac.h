/*
**  $Filename: present/interac.h
**  $Release:  0
**  $Revision: 0.1
**  $Date:     05-04-94
**
**  interactiv presentations for "Der Clou!"
**
**   (c) 1994 ...and avoid panic by K. Kazemi, H. Gaberschek
**      All Rights Reserved.
**
*/
/****************************************************************************
 Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

 Please read the license terms contained in the LICENSE and
 publiclicensecontract.doc files which should be contained with this
 distribution.
****************************************************************************/

#ifndef MODULE_INTERAC
#define MODULE_INTERAC

#include "clue/theclou.h"

#ifndef MODULE_LIST
#include "clue/list/list.h"
#endif

#ifndef MODULE_MEMORY
#include "clue/memory/memory.h"
#endif

#ifndef MODULE_GFX
#include "clue/gfx/gfx.h"
#endif

#ifndef MODULE_TEXT
#include "clue/text/text.h"
#endif

namespace Clue {

#define X_OFFSET    112
#define NRBLINES    5U

#define BG_ACTIVE_COLOR   252
#define VG_ACTIVE_COLOR   254

#define BG_TXT_COLOR      252
#define VG_TXT_COLOR      254

#define BG_BAR_COLOR      230
#define VG_BAR_COLOR      230

#define TXT_MENU_TIMEOUT        (-2)

extern byte ChoiceOk(byte choice, byte exit, LIST *l);

extern byte Bubble(LIST *bubble, byte activ, void (*func)(byte),
                   uint32 waitTime);
extern byte Menu(LIST *menu, uint32 possibility, byte activ,
                 void (*func)(byte), uint32 waitTime);

extern void RefreshMenu(void);

extern void SetBubbleType(uint16 type);
extern void SetPictID(uint16 PictID);
void SetMenuTimeOutFunc(void (*func)(void));

extern byte GetExtBubbleActionInfo(void);

} // End of namespace Clue

#endif

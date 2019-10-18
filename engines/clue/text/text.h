/*
**  $Filename: text/text.h
**  $Release:  1
**  $Revision: 0
**  $Date:     10-03-94
**
**  text interface for "Der Clou!"
**
** (c) 1994 ...and avoid panic by, Kaweh Kazemi
**  All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_TEXT
#define MODULE_TEXT

/* includes */

#ifndef __CTYPE_H
#include <ctype.h>
#endif

#include "clue/theclou.h"

#ifndef MODULE_ERROR
#include "clue/error/error.h"
#endif

#ifndef MODULE_LIST
#include "clue/list/list.h"
#endif

#ifndef MODULE_MEMORY
#include "clue/memory/memory.h"
#endif


/* public defines */
typedef enum {
	TXT_LANG_GERMAN,
	TXT_LANG_ENGLISH,
	TXT_LANG_FRENCH,
	TXT_LANG_SPANISH,
	TXT_LANG_SLOWAKISCH,
	TXT_LANG_LAST
} TxtLanguageE;

#define TXT_KEY_LENGTH     256


#define txtGetFirstLine(id, key, dest) (txtGetNthString(id, key, 0, dest))


/* public prototypes - TEXT */
void txtInit(char lang);
void txtDone(void);

void txtLoad(uint32 textId);
void txtUnLoad(uint32 textId);

void txtPrepare(uint32 textId);
void txtUnPrepare(uint32 textId);

void txtReset(uint32 textId);


/* public prototypes - KEY */
char *txtGetKey(uint16 keyNr, const char *key);
uint32 txtGetKeyAsULONG(uint16 keyNr, const char *key);

LIST *txtGoKey(uint32 textId, const char *key);
LIST *txtGoKeyAndInsert(uint32 textId, const char *key, ...);

bool txtKeyExists(uint32 textId, const char *key);
uint32 txtCountKey(const char *key);


/* public prototypes - STRING */
char *txtGetString(uint32 textId, const char *key, char *dest);
char *txtGetNthString(uint32 textId, const char *key, uint32 nth, char *dest);
void txtPutCharacter(LIST *list, uint16 pos, uint8 c);

#endif

/*
**  $Filename: data\database.ph
**  $Release:  1
**  $Revision: 0
**  $Date:     09-03-94
**
**  database private definitions for "Der Clou!"
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

#ifndef MODULE_DATABASE_PH
#define MODULE_DATABASE_PH

/* includes */
#include "clue/planing/player.h"

namespace Clue {

/* private defines */
// #define dbGetObjectReal(key)  (((struct dbObject *)key)-1)
// #define dbGetObjectKey(obj)   ((void *)(obj+1))
#define dbGetObjectHashNr(nr) ((uint8)(nr % OBJ_HASH_SIZE))

#define EOS                   ((char)'\0')


/* private definitions */
extern NewList<dbObjectNode> *objHash[OBJ_HASH_SIZE];
extern char decodeStr[11];

extern uint32 ObjectListType;
extern uint32 ObjectListFlags;


/* private prototypes - RELATION */
int dbCompare(dbObjectNode *key1, dbObjectNode *key2);
char *dbDecode(dbObjectNode *key);
dbObjectNode *dbEncode(const char *key);

/* private prototypes - OBJECT */
dbObjectNode *dbFindRealObject(uint32 realNr, uint32 offset, uint32 size);

} // End of namespace Clue

#endif

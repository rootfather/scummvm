/*
**  $Filename: data\database.h
**  $Release:  1
**  $Revision: 0
**  $Date:     09-03-94
**
**  database interface for "Der Clou!"
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

#ifndef MODULE_DATABASE
#define MODULE_DATABASE

#include "clue/text.h"
namespace Clue {

#define GET_OUT                 ((uint8)-1)

/* public defines */
#define DB_LOAD_MODE_STD        1
#define DB_LOAD_MODE_NO_NAME    0

#define OLF_NORMAL            (0)
#define OLF_INCLUDE_NAME      (1)
#define OLF_INSERT_STAR       (1 << 1)
#define OLF_PRIVATE_LIST      (1 << 2)
// #define OLF_ADD_PREV_STRING   (1 << 3) // Unused
#define OLF_ADD_SUCC_STRING   (1 << 4)
#define OLF_ALIGNED           (1 << 5)

#define OBJ_HASH_SIZE      31

/* public global data */
extern NewObjectList<dbObjectNode>* ObjectList;
extern NewObjectList<dbObjectNode>* ObjectListPrivate;
extern uint32 ObjectListWidth;
// extern Common::String (*ObjectListPrevString)(uint32, uint32, void *);
extern Common::String (*ObjectListSuccString)(uint32, uint32, dbObjectNode *);

extern NewList<dbObjectNode> *objHash[OBJ_HASH_SIZE];


/* public prototypes - OBJECTS */
bool dbLoadAllObjects(const char *fileName);
bool dbSaveAllObjects(const char *fileName, uint32 offset, uint32 size);
void dbDeleteAllObjects(uint32 offset, uint32 size);

uint32 dbGetObjectCountOfDB(uint32 offset, uint32 size);

void dbSetLoadObjectsMode(uint8 mode);

/* public prototypes - OBJECT */
dbObjectNode *dbNewNode(uint32 nr, uint32 type, Common::String name, uint32 realNr);
dbObjectNode *dbNewObject(uint32 nr, uint32 type, Common::String name, uint32 realNr);

dbObjectNode *dbGetObject(uint32 nr);
Common::String dbGetObjectName(uint32 nr);

bool dbIsObject(uint32 nr, uint32 type);

/* public prototypes - OBJECTNODE */
void dbAddObjectNode(NewObjectList<dbObjectNode> *objectList, uint32 nr, uint32 flags);
void dbRemObjectNode(NewList<dbObjectNode> *objectList, uint32 nr);
dbObjectNode *dbHasObjectNode(NewList<dbObjectNode> *objectList, uint32 nr);

void SetObjectListAttr(uint32 flags, uint32 type);
void BuildObjectList(dbObjectNode *key);

int16 dbStdCompareObjects(dbObjectNode *obj1, dbObjectNode *obj2);
int32 dbSortObjectList(NewObjectList<dbObjectNode> **objectList, int16(*processNode)(dbObjectNode *, dbObjectNode *));
void dbSortPartOfList(NewObjectList<dbObjectNode> *l, dbObjectNode *start, dbObjectNode *end, int16(*processNode)(dbObjectNode *, dbObjectNode *));

void dbAddObjectNode(NewObjectList<dbObjectNode> *objectList, uint32 nr, uint32 flags);

/* public prototypes */
void dbInit();
void dbDone();

#if 0
void dbDeleteObject(uint32 nr);
#endif
} // End of namespace Clue

#endif

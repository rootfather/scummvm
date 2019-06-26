/*
**	$Filename: data\database.h
**	$Release:  1
**	$Revision: 0
**	$Date:     09-03-94
**
**	database interface for "Der Clou!"
**
** (c) 1994 ...and avoid panic by, Kaweh Kazemi
**	All Rights Reserved.
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

/* includes */
#include "clue/theclou.h"

#include "clue/data/relation.h"
#include "clue/list/list.h"
#include "clue/disk/disk.h"
#include "clue/text/text.h"

#include "clue/data/objstd/tcdata.h"


#define GET_OUT					((uint8)-1)

/* public defines */
#define DB_LOAD_MODE_STD        1
#define DB_LOAD_MODE_NO_NAME	0

#define OLF_NORMAL            (0)
#define OLF_INCLUDE_NAME      (1)
#define OLF_INSERT_STAR       (1 << 1)
#define OLF_PRIVATE_LIST      (1 << 2)
#define OLF_ADD_PREV_STRING   (1 << 3)
#define OLF_ADD_SUCC_STRING   (1 << 4)
#define OLF_ALIGNED           (1 << 5)

#define OL_NAME(n)         ((const char *)NODE_NAME((NODE *)n))
#define OL_NR(n)           (((struct ObjectNode *)n)->nr)
#define OL_TYPE(n)         (((struct ObjectNode *)n)->type)
#define OL_DATA(n)         (((struct ObjectNode *)n)->data)

#define OBJ_HASH_SIZE      31


/* public structures */
struct dbObject {
    NODE link;
    uint32 nr;
    uint32 type;
    uint32 realNr;
};

struct dbObjectHeader {
    uint32 nr;
    uint32 type;
    uint32 size;
};

struct ObjectNode {
    NODE Link;
    uint32 nr;
    uint32 type;
    void *data;
};


/* public global data */
extern LIST *ObjectList;
extern LIST *ObjectListPrivate;
extern uint32 ObjectListWidth;
extern char *(*ObjectListPrevString) (uint32, uint32, void *);
extern char *(*ObjectListSuccString) (uint32, uint32, void *);

extern LIST *objHash[OBJ_HASH_SIZE];


/* public prototypes - OBJECTS */
uint8 dbLoadAllObjects(const char *fileName, uint16 diskId);
uint8 dbSaveAllObjects(const char *fileName, uint32 offset, uint32 size, uint16 diskId);
void dbDeleteAllObjects(uint32 offset, uint32 size);

uint32 dbGetObjectCountOfDB(uint32 offset, uint32 size);

void dbSetLoadObjectsMode(uint8 mode);

/* public prototypes - OBJECT */
void *dbNewObject(uint32 nr, uint32 type, uint32 size, char *name, uint32 realNr);
void dbDeleteObject(uint32 nr);

void *dbGetObject(uint32 nr);
uint32 dbGetObjectNr(void *key);
char *dbGetObjectName(uint32 nr, char *objName);

void *dbIsObject(uint32 nr, uint32 type);

/* public prototypes - OBJECTNODE */
struct ObjectNode *dbAddObjectNode(LIST * objectList, uint32 nr, uint32 flags);
void dbRemObjectNode(LIST * objectList, uint32 nr);
struct ObjectNode *dbHasObjectNode(LIST * objectList, uint32 nr);

void SetObjectListAttr(uint32 flags, uint32 type);
void BuildObjectList(void *key);
void ExpandObjectList(LIST * objectList, char *expandItem);

int16 dbStdCompareObjects(struct ObjectNode *obj1, struct ObjectNode *obj2);
int32 dbSortObjectList(LIST ** objectList,
		     int16(*processNode) (struct ObjectNode *,
					 struct ObjectNode *));
void dbSortPartOfList(LIST * objectList, struct ObjectNode *start,
		      struct ObjectNode *end,
		      int16(*processNode) (struct ObjectNode *,
					  struct ObjectNode *));


struct ObjectNode *dbAddObjectNode(LIST * objectList, uint32 nr, uint32 flags);
void dbRemObjectNode(LIST * objectList, uint32 nr);

/* public prototypes */
void dbInit(void);
void dbDone(void);

#endif

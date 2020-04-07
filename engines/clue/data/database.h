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

namespace Clue {

#define GET_OUT                 ((uint8)-1)

/* public defines */
#define DB_LOAD_MODE_STD        1
#define DB_LOAD_MODE_NO_NAME    0

#define OLF_NORMAL            (0)
#define OLF_INCLUDE_NAME      (1)
#define OLF_INSERT_STAR       (1 << 1)
#define OLF_PRIVATE_LIST      (1 << 2)
#define OLF_ADD_PREV_STRING   (1 << 3)
#define OLF_ADD_SUCC_STRING   (1 << 4)
#define OLF_ALIGNED           (1 << 5)

#define OBJ_HASH_SIZE      31


/* public structures */
struct dbObject {
	Node link;
	uint32 nr;
	uint32 type;
	uint32 realNr;
};

struct dbObjectHeader {
	uint32 nr;
	uint32 type;
	uint32 size;
};

/* public global data */
extern NewObjectList<NewObjectNode>* ObjectList;
extern NewObjectList<NewObjectNode>* ObjectListPrivate;
extern uint32 ObjectListWidth;
extern Common::String (*ObjectListPrevString)(uint32, uint32, void *);
extern Common::String (*ObjectListSuccString)(uint32, uint32, void *);

extern List *objHash[OBJ_HASH_SIZE];


/* public prototypes - OBJECTS */
bool dbLoadAllObjects(const char *fileName, uint16 diskId);
bool dbSaveAllObjects(const char *fileName, uint32 offset, uint32 size, uint16 diskId);
void dbDeleteAllObjects(uint32 offset, uint32 size);

uint32 dbGetObjectCountOfDB(uint32 offset, uint32 size);

void dbSetLoadObjectsMode(uint8 mode);

/* public prototypes - OBJECT */
void *dbNewObject(uint32 nr, uint32 type, uint32 size, Common::String name, uint32 realNr);

void *dbGetObject(uint32 nr);
uint32 dbGetObjectNr(void *key);
Common::String dbGetObjectName(uint32 nr);

void *dbIsObject(uint32 nr, uint32 type);

/* public prototypes - OBJECTNODE */
NewObjectNode *dbAddObjectNode(NewObjectList<NewObjectNode> *objectList, uint32 nr, uint32 flags);
void dbRemObjectNode(NewList<NewObjectNode> *objectList, uint32 nr);
NewObjectNode *dbHasObjectNode(NewList<NewObjectNode> *objectList, uint32 nr);

void SetObjectListAttr(uint32 flags, uint32 type);
void BuildObjectList(void *key);

int16 dbStdCompareObjects(NewObjectNode *obj1, NewObjectNode *obj2);
int32 dbSortObjectList(NewObjectList<NewObjectNode> **objectList,
					int16(*processNode)(NewObjectNode *, NewObjectNode *));
void dbSortPartOfList(NewObjectList<NewObjectNode>* l, NewObjectNode* start, NewObjectNode* end,
					int16(*processNode)(NewObjectNode*, NewObjectNode*));

NewObjectNode *dbAddObjectNode(NewObjectList<NewObjectNode> *objectList, uint32 nr, uint32 flags);
void dbRemObjectNode(List *objectList, uint32 nr);

/* public prototypes */
void dbInit();
void dbDone();

#if 0
void dbDeleteObject(uint32 nr);
#endif
} // End of namespace Clue

#endif

/*
**  Der Clou!
**
** $VER: list/list.h 0.2 (10.11.2000)
**
** Reproduced by Oliver Gantert <lucyg@t-online.de>
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_LIST
#define MODULE_LIST

#include "clue/theclou.h"
#include "common/str.h"

namespace Common {
	class String;
}

namespace Clue {

/**************
 * Structures *
 **************/

struct Node {
	Node *Succ;
	Node *Pred;
	char *Name;
	size_t Size;
};

struct List {
	Node Head;
	Node Tail;
};

/**********
 * Macros *
 **********/

#define NODE_SUCC(node) (((Node *)(node))->Succ)
#define NODE_PRED(node) (((Node *)(node))->Pred)
#define NODE_NAME(node) (((Node *)(node))->Name)
#define NODE_SIZE(node) (((Node *)(node))->Size)

#define INNER_HEAD(list) (&(list)->Head)
#define INNER_TAIL(list) (&(list)->Tail)

#define LIST_HEAD(list) (NODE_SUCC(INNER_HEAD((list))))
#define LIST_TAIL(list) (NODE_PRED(INNER_TAIL((list))))

#define LIST_EMPTY(list) (!NODE_SUCC(LIST_HEAD((list))))

/**************
 * Prototypes *
 **************/

List *CreateList();
void RemoveList(List *list);
void FreeList(List *list);
void *AddNode(List *list, void *node, void *predNode);
void *AddTailNode(List *list, void *node);
void *AddHeadNode(List *list, void *node);
void *RemNode(void *node);
void *RemTailNode(List *list);
void *CreateNode(List *list, size_t size, Common::String name);
void *CreateNode(List *list, size_t size, const char *name);
void RemoveNode(List *list, const char *name);
void FreeNode(void *node);
void *GetNode(List *list, const char *name);
void *GetNthNode(List *list, uint32 nth);
uint32 GetNrOfNodes(List *list);
uint32 GetNodeNrByAddr(List *list, void *node);
void foreach(List *list, void (*processNode)(void *));
void Link(List *list, void *node, void *predNode);
void *UnLinkByAddr(List *list, void *node, Node **predNode);
void *UnLink(List *list, const char *name, Node **predNode);
void ReplaceNodeByAddr(List *list, void *node, Node *newNode);
void ReplaceNode(List *list, const char *name, Node *newNode);
uint32 ReadList(List *list, size_t size, char *fileName);
void WriteList(List *list, char *fileName);

#if 0
void *RemHeadNode(LIST *list);
uint32 GetNodeNr(LIST *list, const char *name);
#endif
} // End of namespace Clue

#endif              /* MODULE_LIST */

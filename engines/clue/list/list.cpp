/*
 * list.c
 * (c) 1994 by Kaweh Kazemi
 * All rights reserved.
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/


#include "clue/list/list.h"
#include "clue/memory/memory.h"

namespace Clue {

List *CreateList() {
	List *list = (List *) TCAllocMem(sizeof(*list), true);

	if (list) {
		NODE_SUCC(INNER_HEAD(list)) = INNER_TAIL(list);
		NODE_PRED(INNER_HEAD(list)) = NULL;
		NODE_NAME(INNER_HEAD(list)) = NULL;

		NODE_SUCC(INNER_TAIL(list)) = NULL;
		NODE_PRED(INNER_TAIL(list)) = INNER_HEAD(list);
		NODE_NAME(INNER_TAIL(list)) = NULL;
	}

	return list;
}

void RemoveList(List *list) {
	RemoveNode(list, NULL);
	FreeList(list);
}

void FreeList(List *list) {
	TCFreeMem(list, sizeof(*list));
}

void *AddNode(List *list, void *node, void *predNode) {
	if (!predNode)
		predNode = INNER_HEAD(list);

	NODE_SUCC(node) = NODE_SUCC(predNode);
	NODE_PRED(node) = (Node *)predNode;

	NODE_PRED(NODE_SUCC(predNode)) = (Node *)node;
	NODE_SUCC(predNode) = (Node *)node;
	return node;
}

void *AddTailNode(List *list, void *node) {
	return AddNode(list, node, LIST_TAIL(list));
}

void *AddHeadNode(List *list, void *node) {
	return AddNode(list, node, INNER_HEAD(list));
}

void *RemNode(void *node) {
	NODE_SUCC(NODE_PRED(node)) = NODE_SUCC(node);
	NODE_PRED(NODE_SUCC(node)) = NODE_PRED(node);

	/* just to be safe */
	NODE_SUCC(node) = NULL;
	NODE_PRED(node) = NULL;

	return node;
}

void *RemTailNode(List *list) {
	if (!LIST_EMPTY(list))
		return RemNode(LIST_TAIL(list));

	return NULL;
}
void *CreateNode(List *list, size_t size, Common::String name) {
	return CreateNode(list, size, name.c_str());
}
	
void *CreateNode(List *list, size_t size, const char *name) {
	if (!size)
		size = sizeof(Node);

	size_t len = 0;
	if (name)
		len = strlen(name) + 1;

	Node *node = NULL;
	if (size >= sizeof(Node)) {
		if ((node = (Node *) TCAllocMem(size + len, true))) {
			NODE_SUCC(node) = NULL;
			NODE_PRED(node) = NULL;
			NODE_SIZE(node) = size;

			if (name)
				NODE_NAME(node) = strcpy((char *) node + size, name);
			else
				NODE_NAME(node) = NULL;

			if (list)
				AddTailNode(list, node);
		}
	} else {
		ErrorMsg(Internal_Error, ERROR_MODULE_MEMORY, 666);
	}

	return (void *) node;
}

void RemoveNode(List *list, const char *name) {
	Node *node;
	if (name) {
		if ((node = (Node *)GetNode(list, name))) {
			RemNode(node);
			FreeNode(node);
		}
	} else {
		if (!LIST_EMPTY(list)) {
			while ((node = (Node *)RemTailNode(list)))
				FreeNode(node);
		}
	}
}

void FreeNode(void *node) {
	size_t size = NODE_SIZE(node);

	if (NODE_NAME(node))
		size += strlen(NODE_NAME(node)) + 1;

	TCFreeMem(node, size);
}

void *GetNode(List *list, const char *name) {
	for (Node *node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node)) {
		if (strcmp(NODE_NAME(node), name) == 0)
			return node;
	}

	return NULL;
}

void *GetNthNode(List *list, uint32 nth) {
	for (Node *node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node)) {
		if (nth == 0)
			return node;
		nth--;
	}

	return NULL;
}

uint32 GetNrOfNodes(List *list) {
	Node *node = NULL;
	uint32 i = 0;

	for (i = 0, node = LIST_HEAD(list); NODE_SUCC(node); i++, node = NODE_SUCC(node))
		;

	return i;
}

uint32 GetNodeNrByAddr(List *list, void *node) {
	Node *s;
	uint32 i;

	for (s = LIST_HEAD(list), i = 0; NODE_SUCC(s) && (s != node);
	        s = NODE_SUCC(s), i++);

	return i;
}

void foreach (List *list, void (*processNode)(void *)) {
	for (Node *node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node))
		processNode(node);
}

void Link(List *list, void *node, void *predNode) {
	if (predNode)
		AddNode(list, node, predNode);
	else
		AddTailNode(list, node);
}

void *UnLinkByAddr(List *list, void *node, Node **predNode) {
	if (predNode)
		*predNode = NODE_PRED(node);

	return RemNode(node);
}

void *UnLink(List *list, const char *name, Node **predNode) {
	return UnLinkByAddr(list, GetNode(list, name), predNode);
}

void ReplaceNodeByAddr(List *list, void *node, Node *newNode) {
	Node *predNode;
	if ((node = UnLinkByAddr(list, node, &predNode))) {
		Link(list, newNode, predNode);
		FreeNode(node);
	}
}

void ReplaceNode(List *list, const char *name, Node *newNode) {
	ReplaceNodeByAddr(list, GetNode(list, name), newNode);
}

uint32 ReadList(List *list, size_t size, char *fileName) {
	uint32 i = 0;
	Common::Stream *fh = dskOpen(fileName, 0);
	if (fh) {
		char buffer[256];
		while (dskGetLine(buffer, sizeof(buffer), fh)) {
			if (buffer[0] != ';') { /* skip comments */
				if (!CreateNode(list, size, buffer)) {
					RemoveNode(list, NULL);
					dskClose(fh);
					return 0;
				}

				i++;
			}
		}

		dskClose(fh);
	}
	return i;
}

void WriteList(List *list, char *fileName) {
	Common::Stream *fh = dskOpen(fileName, 1);
	if (fh) {
		for (Node *node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node))
			dskSetLine(fh, NODE_NAME(node));

		dskClose(fh);
	}
}

#if 0
void *RemHeadNode(LIST *list) {
	if (!LIST_EMPTY(list))
		return RemNode(LIST_HEAD(list));

	return NULL;
}

uint32 GetNodeNr(LIST *list, const char *name) {
	return GetNodeNrByAddr(list, GetNode(list, name));
}
#endif

} // End of namespace Clue

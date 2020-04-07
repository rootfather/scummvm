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
#include "clue/text.h"

namespace Clue {

List *CreateList() {
	List *list = (List *) TCAllocMem(sizeof(*list), true);

	if (list) {
		NODE_SUCC(INNER_HEAD(list)) = INNER_TAIL(list);
		NODE_PRED(INNER_HEAD(list)) = nullptr;
		NODE_NAME(INNER_HEAD(list)) = nullptr;

		NODE_SUCC(INNER_TAIL(list)) = nullptr;
		NODE_PRED(INNER_TAIL(list)) = INNER_HEAD(list);
		NODE_NAME(INNER_TAIL(list)) = nullptr;
	}

	return list;
}

void RemoveList(List *list) {
	RemoveNode(list, nullptr);
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
	NODE_SUCC(node) = nullptr;
	NODE_PRED(node) = nullptr;

	return node;
}

void *RemTailNode(List *list) {
	if (!LIST_EMPTY(list))
		return RemNode(LIST_TAIL(list));

	return nullptr;
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

	Node *node = nullptr;
	if (size >= sizeof(Node)) {
		if ((node = (Node *) TCAllocMem(size + len, true))) {
			NODE_SUCC(node) = nullptr;
			NODE_PRED(node) = nullptr;
			NODE_SIZE(node) = size;

			if (name)
				NODE_NAME(node) = strcpy((char *) node + size, name);
			else
				NODE_NAME(node) = nullptr;

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

	return nullptr;
}

void *GetNthNode(List *list, uint32 nth) {
	for (Node *node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node)) {
		if (nth == 0)
			return node;
		nth--;
	}

	return nullptr;
}

uint32 GetNrOfNodes(List *list) {
	uint32 i = 0;

	for (Node *node = LIST_HEAD(list); NODE_SUCC(node); i++, node = NODE_SUCC(node))
		;

	return i;
}

} // End of namespace Clue

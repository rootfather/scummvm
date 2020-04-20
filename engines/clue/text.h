/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

/*
* This code is based on :
* - original source code of The Clue!
*	Copyright (c) 1994 ...and avoid panic by, Kaweh Kazemi
* - Costa's DerClou! SDL port
*	Copright (c) 2005-2017 Vasco Alexandre da Silva Costa
*/

#ifndef CLUE_TEXT_H
#define CLUE_TEXT_H

/* includes */
#include "clue/list/list.h"
#include "common/util.h"
#include "common/str.h"

#include "clue/error/error.h"
#include "clue/clue.h"

namespace Clue {
/* private defines */
#define TXT_LIST        "texts.lst"
#define TXT_SUFFIX      ".txt"
#define TXT_BUFFER_WORK    StdBuffer1
#define TXT_CHAR_EOL             '\n'
#define TXT_CHAR_EOS             '\0'
#define TXT_CHAR_MARK            '#'
#define TXT_CHAR_REMARK          ';'
#define TXT_CHAR_KEY_SEPERATOR   ','
#define TXT_CHAR_EOF             '^'

#define TXT_XOR_VALUE               0x75

#define ERR_TXT_READING_LIST     1
#define ERR_TXT_FAILED_LIST      2
#define ERR_TXT_NO_MEM           3
#define ERR_TXT_NO_KEY           4
#define ERR_TXT_FAILED_BASE      5

/* public defines */
#define TXT_KEY_LENGTH     256

class ClueEngine;

class NewNode {
public:
	NewNode* _succ;
	NewNode* _pred;
	Common::String _name;

	NewNode();
	NewNode(Common::String name);
	virtual ~NewNode();

	void remNode();
};

class NewTCEventNode : public NewNode {
public:
	uint32 _eventNr;

	NewTCEventNode() { _eventNr = 0; };
	~NewTCEventNode() {}
};

class dbObjectNode : public NewNode {
public:
	uint32 _nr;
	uint32 _type;
	uint32 _realNr;

	dbObjectNode() { _nr = _type = _realNr = 0; } // CHECKME : Is it possible to get rid of it?
	dbObjectNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : NewNode(name), _nr(nr), _type(type), _realNr(realNr) {}
};

class IODataNode : public NewNode {
public:
	byte _ioData;

	IODataNode() { _ioData = 0; }
	~IODataNode() {}
};

class PresentationInfoNode : public NewNode {
public:
	char _extendedText[70];
	uint32 _extendedNr;
	uint32 _maxNr;

	byte _presentHow;

	PresentationInfoNode() { _extendedText[0] = '\0'; _extendedNr = _maxNr = _presentHow = 0; }
	~PresentationInfoNode() {}
};

template <typename T>
class NewList {
public: // Temporary dor debug purposes. To be removed.
	T* _head;
	T* _tail;
public:

	NewList();
	~NewList();

	T *getNthNode(uint32 nth);
	uint32 getNrOfNodes();
	void removeList();
	void removeNode(Common::String name);
	T *getNode(Common::String name);
	uint32 getNodeNrByAddr(T *node);
	T *remTailNode();

	uint32 readList(Common::String fileName);
	void writeList(Common::String fileName);

	void addNode(T *node, T *predNode);
	void addTailNode(T *node);
	void addHeadNode(T *node);
	void putCharacter(uint16 pos, uint8 c);

	T *createNode(Common::String name);
	void link(T *node, T *predNode);
	T* unLinkByAddr(T *node, T **predNode);
	T* unLink(Common::String name, T **predNode);
	void replaceNodeByAddr(T *node, T *newNode);
	void replaceNode(Common::String name, T *newNode);

	T *getListHead() { return (T *)_head->_succ; }
	bool isEmpty() { return getListHead()->_succ == nullptr; }
};

template <typename T>
class NewObjectList : public NewList<T> {
public:
	NewObjectList() {};
	~NewObjectList() {};

	void expandObjectList(Common::String expandItem);
};

/* private structures */
class Text : public NewNode {
public:
	char* _handle;
	char* _lastMark;
	size_t _length;

	Text();
	~Text();
};

class TextMgr {
public:
	TextMgr(ClueEngine *vm);
	~TextMgr();

private:
	ClueEngine *_vm;
	NewList<Text> *_txtBase;

	char *getLine(Text *txt, uint8 lineNr);

public:
	/* public prototypes - TEXT */
	void init();

	void load(uint32 textId);
	void unLoad(uint32 textId);

	void prepare(uint32 textId);
	void reset(uint32 textId);

	/* public prototypes - KEY */
	Common::String getKey(uint16 keyNr, const char *key);
	uint32 getKeyAsUint32(uint16 keyNr, const char *key);
	uint32 getKeyAsUint32(uint16 keyNr, Common::String key);

	NewList<NewNode> *goKey(uint32 textId, const char *key);
	NewList<NewNode> *goKeyAndInsert(uint32 textId, const char *key, ...);

	bool keyExists(uint32 textId, const char *key);
	uint32 countKey(Common::String key);

	/* public prototypes - STRING */
	Common::String getNthString(uint32 textId, const char *key, uint32 nth);
	Common::String getFirstLine(uint32 textId, const char *key);
};

// Template code

template <typename T>
NewList < T > ::NewList() {
	_head = new T;
	_tail = new T;
	_head->_succ = _tail;
	_tail->_pred = _head;
}

template <typename T>
NewList<T>::~NewList() {
	delete _head;
	delete _tail;
}

template <typename T>
void NewList<T>::addNode(T* node, T* predNode) {
	if (!predNode)
		predNode = _head;

	node->_succ = predNode->_succ;
	node->_pred = predNode;

	predNode->_succ->_pred = node;
	predNode->_succ = node;
}

template <typename T>
void NewList<T>::addHeadNode(T* node) {
	addNode(node, _head);
}

template <typename T>
void NewList<T>::addTailNode(T* node) {
	addNode(node, (T*)_tail->_pred);
}

template <typename T>
T* NewList<T>::getNthNode(uint32 nth) {
	for (T* node = getListHead(); node->_succ; node = (T *)node->_succ) {
		if (nth == 0)
			return node;
		nth--;
	}

	return nullptr;
}

template <typename T>
uint32 NewList<T>::getNrOfNodes() {
	uint32 i = 0;
	for (T *node = getListHead(); node->_succ; node = (T *)node->_succ)
		++i;

	return i;
}

template <typename T>
void NewList<T>::removeList() {
	removeNode(nullptr);
	// freeList();
}

template <typename T>
T* NewList<T>::remTailNode() {
	T* result = nullptr;
	if (!isEmpty()) {
		result = _tail;
		NewNode *old = _tail->_pred;
		_tail->_pred->remNode();
		delete old;
	}

	return result;
}

template <typename T>
void NewList<T>::removeNode(Common::String name) {
	T* node;
	if (!name.empty()) {
		if ((node = getNode(name))) {
			node->remNode();
			delete node;
		}
	} else if (!isEmpty()) {
		node = remTailNode();
		while (node) {
			// delete node;
			node = remTailNode();
		}
	}
}

template <typename T>
T* NewList<T>::createNode(Common::String name) {
	T* node = new T;
	node->_succ = nullptr;
	node->_pred = nullptr;
	node->_name = name;

	addTailNode(node);

	return node;
}

template <typename T>
T* NewList<T>::getNode(Common::String name) {
	for (T* node = getListHead(); node->_succ; node = (T *)node->_succ) {
		if (node->_name.equals(name))
			return node;
	}

	return nullptr;
}

template <typename T>
uint32 NewList<T>::getNodeNrByAddr(T *node) {
	uint32 i = 0;

	for (T* s = getListHead(); s->_succ && (s != node); s = (T *)s->_succ, i++)
		;

	return i;
}

template <typename T>
uint32 NewList<T>::readList(Common::String fileName) {
	uint32 i = 0;
	Common::Stream* fh = dskOpen(fileName.c_str(), 0);
	if (fh) {
		char buffer[256];
		while (dskGetLine(buffer, sizeof(buffer), fh)) {
			if (buffer[0] != ';') { // skip comments
				createNode(buffer);
				++i;
			}
		}

		dskClose(fh);
	}
	return i;
}

template <typename T>
void NewList<T>::putCharacter(uint16 pos, uint8 c) {
	for (T *node = getListHead(); node->_succ; node = (T *)node->_succ)
		node->_name.setChar(c, pos);
}

template <typename T>
void NewList<T>::link(T *node, T *predNode) {
	if (predNode)
		addNode(node, predNode);
	else
		addTailNode(node);
}

template <typename T>
T *NewList<T>::unLinkByAddr(T *node, T **predNode) {
	if (predNode)
		*predNode = (T *)node->_pred;

	((NewNode*)node)->remNode();
	return node;
}

template <typename T>
T *NewList<T>::unLink(Common::String name, T** predNode) {
	return unLinkByAddr(getNode(name), predNode);
}

template <typename T>
void NewList<T>::replaceNodeByAddr(T* node, T* newNode) {
	T* predNode;
	if ((node = unLinkByAddr(node, &predNode))) {
		link(newNode, predNode);
		delete node;
	}
}

template <typename T>
void NewList<T>::replaceNode(Common::String name, T* newNode) {
	replaceNodeByAddr(getNode(name), newNode);
}

template <typename T>
void NewList<T>::writeList(Common::String fileName) {
	Common::Stream *fh = dskOpen(fileName.c_str(), 1);
	if (fh) {
		for (T *node = getListHead(); node->_succ; node = (T *)node->_succ)
			dskSetLine(fh, node->_name.c_str());

		dskClose(fh);
	}
}

template <typename T>
void NewObjectList<T>::expandObjectList(Common::String expandItem) {
	T *objNode = (T *) this->createNode(expandItem);

	objNode->_nr = 0;
	objNode->_type = 0;
//	objNode->_data = nullptr;
	warning("expandObjectList - objNode->data not handled");
}

} // End of namespace Clue

#endif

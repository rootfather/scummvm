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
	~NewNode();

	void remNode();
};

class NewTCEventNode : public NewNode {
public:
	uint32 _eventNr;

	NewTCEventNode() { _eventNr = 0; };
	~NewTCEventNode() {}
};

template <typename T>
class NewList {
public:
	T *_head;
	T *_tail;

	NewList();
	~NewList();

	T *getNthNode(uint32 nth);
	uint32 getNrOfNodes();
	void removeList();
	void removeNode(const char *name);
	T *getNode(const char *name);
	T *remTailNode();
	void readList(const char *fileName);
	void addNode(T *node, T *predNode);
	void addTailNode(T *node);

	T *createNode(const char *name);

	T* getListHead() { return (T *)_head->_succ; }
	bool isEmpty() { return getListHead()->_succ == nullptr; }
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

class TextControl : public NewList<Text> {
public:
	char _language;

	TextControl(char lang) : _language(lang) {}
};

class TextMgr {
public:
	TextMgr(ClueEngine *vm, char lang);
	~TextMgr();

private:
	ClueEngine *_vm;
	TextControl *_txtBase;

	char *getLine(Text *txt, uint8 lineNr);

public:
	/* public prototypes - TEXT */
	void init(char lang);

	void load(uint32 textId);
	void unLoad(uint32 textId);

	void prepare(uint32 textId);
	void reset(uint32 textId);

	/* public prototypes - KEY */
	Common::String getKey(uint16 keyNr, const char *key);
	uint32 getKeyAsUint32(uint16 keyNr, const char *key);
	uint32 getKeyAsUint32(uint16 keyNr, Common::String key);

	List* goKey(uint32 textId, const char *key);
	List* goKeyAndInsert(uint32 textId, const char *key, ...);

	bool keyExists(uint32 textId, const char *key);
	uint32 countKey(Common::String key);

	/* public prototypes - STRING */
	Common::String getNthString(uint32 textId, const char *key, uint32 nth);
	Common::String getFirstLine(uint32 textId, const char *key);
	void putCharacter(List *list, uint16 pos, uint8 c);
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
void NewList<T>::addTailNode(T* node) {
	addNode(node, (T*)_tail->_pred);
}

template <typename T>
T* NewList<T>::getNthNode(uint32 nth) {
	for (NewNode* node = _head->_succ; node->_succ; node = node->_succ) {
		if (nth == 0)
			return (T*)node;
		nth--;
	}

	return nullptr;
}

template <typename T>
uint32 NewList<T>::getNrOfNodes() {
	uint32 i = 0;
	for (NewNode* node = _head->_succ; node->_succ; node = node->_succ)
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
	if (!_head->_succ) {
		result = _tail;
		_tail->_pred->remNode();
	}

	return result;
}

template <typename T>
void NewList<T>::removeNode(const char* name) {
	T* node;
	if (name) {
		if ((node = getNode(name))) {
			node->remNode();
			delete node;
		}
	}
	else if (_head->_succ) {
		while ((node = remTailNode()))
			delete node;
	}
}

template <typename T>
T* NewList<T>::createNode(const char* name) {
	T* node = new T;
	node->_succ = nullptr;
	node->_pred = nullptr;
	node->_name = Common::String(name);

	addTailNode(node);

	return node;
}

template <typename T>
T* NewList<T>::getNode(const char* name) {
	for (T* node = (T *)_head->_succ; node->_succ; node = (T *)node->_succ) {
		if (node->_name.equals(name))
			return node;
	}

	return nullptr;
}

template <typename T>
void NewList<T>::readList(const char* fileName) {
	Common::Stream* fh = dskOpen(fileName, 0);
	if (fh) {
		char buffer[256];
		while (dskGetLine(buffer, sizeof(buffer), fh)) {
			if (buffer[0] != ';') // skip comments
				createNode(buffer);
		}

		dskClose(fh);
	}
}

} // End of namespace Clue

#endif

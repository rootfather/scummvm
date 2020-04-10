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

#ifndef MODULE_TXT
#define MODULE_TXT

#include "clue/base/base.h"
#include "clue/error/error.h"
#include "clue/text.h"
#include "clue/clue.h"

namespace Clue {

NewNode::NewNode() {
	this->_pred = nullptr;
	this->_succ = nullptr;
}

NewNode::~NewNode() {
}

void NewNode::remNode() {
	_pred->_succ = _succ;
	_succ->_pred = _pred;

	_succ = nullptr;
	_pred = nullptr;
}

Text::Text() {
	_handle = nullptr;
	_lastMark = nullptr;
	_length = 0;
}

Text::~Text() {
	if (_handle)
		free(_handle);
	if (_lastMark)
		free(_lastMark);
}

	
/* TextMgr */
TextMgr::TextMgr(ClueEngine* vm) : _vm(vm) {
	_txtBase = nullptr;
	init();
}

TextMgr::~TextMgr() {
	if (_txtBase) {
		uint32 nr = _txtBase->getNrOfNodes();

		for (uint32 i = 0; i < nr; i++)
			unLoad(i);
	}
}

/* private functions */
char *TextMgr::getLine(Text *txt, uint8 lineNr) {
	char *line = nullptr;
	if (txt && txt->_lastMark && lineNr) {
		line = txt->_lastMark;
		uint8 i = 0;

		while (i < lineNr) {
			if (*line == TXT_CHAR_EOF)
				return nullptr;

			if (i > 0 && (*line == TXT_CHAR_MARK))
				return nullptr;

			if (*line == TXT_CHAR_EOS) {
				line++;     /* skip second EOS */
				i++;

				if (*line == TXT_CHAR_EOF)
					return nullptr;

				/* skip comments */
				while (*(line + 1) == TXT_CHAR_REMARK) {
					while (*(++line) != TXT_CHAR_EOS);
					line++; /* skip second EOS */
				}
			}

			line++;
		}

		if (*line == TXT_CHAR_EOF || *line == TXT_CHAR_MARK)
			return nullptr;
	}

	return line;
}

/*  public functions - TEXT */
void TextMgr::init() {
	_txtBase = new NewList<Text>;

	char txtListPath[DSK_PATH_MAX];
	dskBuildPathName(DISK_CHECK_FILE, TEXT_DIRECTORY, TXT_LIST, txtListPath);
	_txtBase->readList(txtListPath);
	uint32 nr = _txtBase->getNrOfNodes();
	for (uint32 i = 0; i < nr; i++)
		load(i);
}

void TextMgr::load(uint32 textId) {
	Text *txt = _txtBase->getNthNode(textId);

	if (txt) {
		if (!txt->_handle) {
			char txtPath[DSK_PATH_MAX];

			Common::String txtFile = txt->_name + g_clue->getTxtLanguage() + TXT_SUFFIX;
			dskBuildPathName(DISK_CHECK_FILE, TEXT_DIRECTORY, txtFile.c_str(), txtPath);

			size_t length = dskFileLength(txtPath);
			txt->_length = length;

			/* loading text into buffer */
			uint8 *text = (uint8 *)dskLoad(txtPath);

			/* correcting text */
			for (uint8 *ptr = text; length--; ptr++) {

				*ptr ^= TXT_XOR_VALUE;

				switch (*ptr) {
				case '\r':
				case '\n':
					*ptr = '\0';
					break;

				default:
					break;
				}
			}

			/* save text into xms */
			if (text) {
				txt->_handle = (char *)malloc(txt->_length + 1);
				memcpy(txt->_handle, text, txt->_length);
				free(text);

				/* let's play safe here... */
				txt->_handle[txt->_length] = TXT_CHAR_EOF;
				txt->_length++;
			} else
				ErrorMsg(No_Mem, ERROR_MODULE_TXT, ERR_TXT_NO_MEM);
		}
	}
}

void TextMgr::unLoad(uint32 textId) {
	Text *txt = _txtBase->getNthNode(textId);

	if (txt) {
		if (txt->_handle) {
			free(txt->_handle);
		}

		txt->_handle = nullptr;
		txt->_lastMark = nullptr;
		txt->_length = 0;
	}
}

void TextMgr::prepare(uint32 textId) {
	Text *txt = _txtBase->getNthNode(textId);

	if (txt) {
		memcpy(TXT_BUFFER_WORK, txt->_handle, txt->_length);
		txt->_lastMark = (char *)TXT_BUFFER_WORK;
	}
}

void TextMgr::reset(uint32 textId) {
	Text *txt = _txtBase->getNthNode(textId);

	if (txt)
		txt->_lastMark = (char *)TXT_BUFFER_WORK;
}


/* public functions - KEY */
Common::String TextMgr::getKey(uint16 keyNr, const char *key) {
	Common::String retVal = Common::String("");
	if (!key)
		return retVal;

	for (uint16 i = 1; i < keyNr; i++) {
		while (*key && (*key != TXT_CHAR_KEY_SEPERATOR))
			key++;

		if (!*key)
			return retVal;

		key++;
	}

	while (Common::isSpace(*key))
		key++;

	if (!*key)
		return retVal;

	for (uint16 i = 0; (i < TXT_KEY_LENGTH) && *key && (*key != TXT_CHAR_KEY_SEPERATOR); ++i, ++key)
		retVal += *key;

	return retVal;
}

uint32 TextMgr::getKeyAsUint32(uint16 keyNr, const char *key) {
	const Common::String res = getKey(keyNr, key);

	if (!res.empty())
		return (uint32) atoi(res.c_str());

	return (uint32) -1;
}

uint32 TextMgr::getKeyAsUint32(uint16 keyNr, Common::String key) {
	return getKeyAsUint32(keyNr, key.c_str());
}

NewList<NewNode> *TextMgr::goKey(uint32 textId, const char *key) {
	NewList<NewNode> *txtList = nullptr;

	Text *txt = _txtBase->getNthNode(textId);
	if (txt) {
		char *LastMark = nullptr;

		/* MOD: 08-04-94 hg
		     * if no key was given take the next one
		 * -> last position is temporarily saved in LastMark because
		 * txt->LastMark is modified in txtPrepare!
		 *
		 * Special case: no key, text never used !!
		 */
		if ((!key) && (txt->_lastMark))
			LastMark = txt->_lastMark;

		prepare(textId);

		/* Explanation for +1: LastMark points to the last key
		   -> without "+1" the same key would be returned */
		if (!key && LastMark)
			txt->_lastMark = LastMark + 1;

		for (; *txt->_lastMark != TXT_CHAR_EOF; txt->_lastMark++) {
			if (*txt->_lastMark == TXT_CHAR_MARK) {
				uint8 found = 1;

				if (key) {
					char mark[TXT_KEY_LENGTH];

					strcpy(mark, txt->_lastMark + 1);

					if (strcmp(key, mark) != 0)
						found = 0;
				}

				if (found) {
					uint8 i = 1;
					char *line;

					txtList = new NewList<NewNode>;

					while ((line = getLine(txt, i++)))
						txtList->createNode(line);
					break;
				}
			}
		}
	}

	if (!txtList)
		DebugMsg(ERR_ERROR, ERROR_MODULE_TXT, "NOT FOUND KEY '%s'", key);

	return txtList;
}

NewList<NewNode> *TextMgr::goKeyAndInsert(uint32 textId, const char *key, ...) {
	NewList<NewNode> *txtList = new NewList<NewNode>;

	va_list argument;
	va_start(argument, key);

	NewList<NewNode> *originList = goKey(textId, key);

	for (NewNode *node = originList->getListHead(); node->_succ; node = node->_succ) {
		char originLine[256], txtLine[256];

		strcpy(originLine, node->_name.c_str());
		strcpy(txtLine, node->_name.c_str());

		for (size_t i = 2; i < strlen(originLine); i++) {
			if (originLine[i - 2] == '%') {
				sprintf(txtLine, originLine, va_arg(argument, uint32));
				i = strlen(originLine) + 1;
			}
		}

		txtList->createNode(txtLine);
	}

	originList->removeList();

	va_end(argument);
	return txtList;
}

bool TextMgr::keyExists(uint32 textId, const char *key) {
	bool found = false;
	Text *txt = _txtBase->getNthNode(textId);

	if (txt && key) {
		prepare(textId);

		/* after txtPrepare _lastMark points to TXT_BUFFER_PREPARE in every case */
		for (; *txt->_lastMark != TXT_CHAR_EOF; txt->_lastMark++) {
			if (*txt->_lastMark == TXT_CHAR_MARK) {
				char mark[TXT_KEY_LENGTH];

				strcpy(mark, txt->_lastMark + 1);

				if (strcmp(key, mark) == 0) {
					found = true;
					break;
				}
			}
		}
	}

	return found;
}

uint32 TextMgr::countKey(Common::String key) {
	const uint size = key.size();
	uint count = 1;

	for (uint j = 0; j < size; j++) {
		if (key[j] == TXT_CHAR_KEY_SEPERATOR)
			++count;
	}

	return count;
}
/* functions - STRING */
Common::String TextMgr::getNthString(uint32 textId, const char *key, uint32 nth) {
	Common::String dest;
	NewList<NewNode> *txtList = goKey(textId, key);
	NewNode *src = txtList->getNthNode(nth);

	if (src)
		dest = src->_name;

	txtList->removeList();

	return dest;
}

Common::String TextMgr::getFirstLine(uint32 id, const char *key) {
	return Common::String(getNthString(id, key, 0));
}

} // End of namespace Clue

#endif

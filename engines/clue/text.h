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

#ifndef MODULE_TEXT
#define MODULE_TEXT

/* includes */
#include "clue/list/list.h"
#include "common/str.h"

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

/* private structures */
struct TextControl {
	LIST *_textList;
	char _language;
};

struct Text {
	NODE txt_Link; // Unused? But required for alignment during casting?

	char *_handle;
	char *_lastMark;
	size_t _length;
};

class TextMgr {
public:
	TextMgr(ClueEngine *vm, char lang);
	~TextMgr();

private:
	ClueEngine *_vm;
	TextControl *_txtBase;
	char _keyBuffer[TXT_KEY_LENGTH];

	char *getLine(Text *txt, uint8 lineNr);

public:
	/* public prototypes - TEXT */
	void init(char lang);

	void load(uint32 textId);
	void unLoad(uint32 textId);

	void prepare(uint32 textId);
	void reset(uint32 textId);

	/* public prototypes - KEY */
	char* getKey(uint16 keyNr, const char *key);
	uint32 getKeyAsUint32(uint16 keyNr, const char *key);

	LIST* goKey(uint32 textId, const char *key);
	LIST* goKeyAndInsert(uint32 textId, const char *key, ...);

	bool keyExists(uint32 textId, const char *key);
	uint32 countKey(const char *key);

	/* public prototypes - STRING */
	char *getNthString(uint32 textId, const char *key, uint32 nth, char *dest);
	Common::String getFirstLine(uint32 textId, const char *key, char *dest);
	void putCharacter(LIST *list, uint16 pos, uint8 c);
};
	
} // End of namespace Clue

#endif

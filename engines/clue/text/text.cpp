/*
**  $Filename: text\text.c
**  $Release:  1
**  $Revision: 0
**  $Date:     10-03-94
**
**  text implementation for "Der Clou!"
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

#ifndef MODULE_TXT
#define MODULE_TXT

/* public header(s) */
#include "clue/text/text.h"
#include "clue/clue.h"

/* private header(s) */
#include "clue/text/text_e.h"
#include "clue/text/text_p.h"

#include "common/util.h"

namespace Clue {

/* private globals declaration */
TextMgr::TextMgr(ClueEngine* vm, char lang) : _vm(vm) {
	_txtBase = nullptr;
	_keyBuffer[0] = '\0';
	init(lang);
}

TextMgr::~TextMgr() {
	if (_txtBase) {
		uint32 nr = GetNrOfNodes(_txtBase->_textList);

		for (uint32 i = 0; i < nr; i++)
			unLoad(i);

		RemoveList(_txtBase->_textList);
		TCFreeMem(_txtBase, sizeof(*_txtBase));
	}
}
	
/* private functions */
char *TextMgr::getLine(struct Text *txt, uint8 lineNr) {
	char *line = nullptr;
	if (txt && txt->_lastMark && lineNr) {
		line = txt->_lastMark;
		uint8 i = 0;

		while (i < lineNr) {
			if (*line == TXT_CHAR_EOF)
				return NULL;

			if (i > 0 && (*line == TXT_CHAR_MARK))
				return NULL;

			if (*line == TXT_CHAR_EOS) {
				line++;     /* skip second EOS */
				i++;

				if (*line == TXT_CHAR_EOF)
					return NULL;

				/* skip comments */
				while (*(line + 1) == TXT_CHAR_REMARK) {
					while (*(++line) != TXT_CHAR_EOS);
					line++; /* skip second EOS */
				}
			}

			line++;
		}

		if (*line == TXT_CHAR_EOF)
			return NULL;
		if (*line == TXT_CHAR_MARK)
			return NULL;
	}

	return line;
}

/*  public functions - TEXT */
void TextMgr::init(char lang) {
	if ((_txtBase = (TextControl *)TCAllocMem(sizeof(TextControl), 0))) {
		_txtBase->_textList = CreateList();
		_txtBase->_language = lang;

		char txtListPath[DSK_PATH_MAX];
		dskBuildPathName(DISK_CHECK_FILE, TEXT_DIRECTORY, TXT_LIST, txtListPath);

		if (ReadList(_txtBase->_textList, sizeof(struct Text), txtListPath)) {
			uint32 nr = GetNrOfNodes(_txtBase->_textList);
			for (uint32 i = 0; i < nr; i++)
				load(i);
		} else {
			ErrorMsg(No_Mem, ERROR_MODULE_TXT, ERR_TXT_READING_LIST);
		}
	} else {
		ErrorMsg(No_Mem, ERROR_MODULE_TXT, ERR_TXT_FAILED_BASE);
	}
}

void TextMgr::load(uint32 textId) {
	struct Text *txt = (Text *)GetNthNode(_txtBase->_textList, textId);

	if (txt) {
		if (!txt->_handle) {
			char txtFile[DSK_PATH_MAX];
			char txtPath[DSK_PATH_MAX];

			sprintf(txtFile, "%s%c%s", NODE_NAME(txt), _txtBase->_language, TXT_SUFFIX);
			dskBuildPathName(DISK_CHECK_FILE, TEXT_DIRECTORY, txtFile, txtPath);

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
	struct Text *txt = (Text *)GetNthNode(_txtBase->_textList, textId);

	if (txt) {
		if (txt->_handle) {
			free(txt->_handle);
		}

		txt->_handle = NULL;
		txt->_lastMark = NULL;
		txt->_length = 0;
	}
}

void TextMgr::prepare(uint32 textId) {
	struct Text *txt = (Text *)GetNthNode(_txtBase->_textList, textId);

	if (txt) {
		memcpy(TXT_BUFFER_WORK, txt->_handle, txt->_length);
		txt->_lastMark = (char *)TXT_BUFFER_WORK;
	}
}

void TextMgr::reset(uint32 textId) {
	struct Text *txt = (Text *)GetNthNode(_txtBase->_textList, textId);

	if (txt)
		txt->_lastMark = (char *)TXT_BUFFER_WORK;
}


/* public functions - KEY */
char *TextMgr::getKey(uint16 keyNr, const char *key) {
	if (!key)
		return NULL;

	for (uint16 i = 1; i < keyNr; i++) {
		while (*key && (*key != TXT_CHAR_KEY_SEPERATOR))
			key++;

		if (!*key)
			return NULL;
		else
			key++;
	}

	while (Common::isSpace(*key))
		key++;

	if (!*key)
		return NULL;

	uint16 i;
	for (i = 0; (i < TXT_KEY_LENGTH) && *key && (*key != TXT_CHAR_KEY_SEPERATOR); i++)
		_keyBuffer[i] = *key++;

	_keyBuffer[i] = TXT_CHAR_EOS;
	return _keyBuffer;
}

uint32 TextMgr::getKeyAsUint32(uint16 keyNr, const char *key) {
	char *res = getKey(keyNr, key);

	if (res)
		return ((uint32) atoi(res));
	else
		return ((uint32) - 1);
}

LIST * TextMgr::goKey(uint32 textId, const char *key) {
	LIST *txtList = NULL;

	struct Text *txt = (Text *)GetNthNode(_txtBase->_textList, textId);
	if (txt) {
		char *LastMark = NULL;

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

					txtList = CreateList();

					while ((line = getLine(txt, i++)))
						CreateNode(txtList, 0, line);
					break;
				}
			}
		}

	}

	if (!txtList) {
		DebugMsg(ERR_ERROR, ERROR_MODULE_TXT, "NOT FOUND KEY '%s'", key);
	}

	return txtList;
}

LIST * TextMgr::goKeyAndInsert(uint32 textId, const char *key, ...) {
	LIST *txtList = CreateList();

	va_list argument;
	va_start(argument, key);

	LIST *originList = goKey(textId, key);

	for (NODE *node = LIST_HEAD(originList); NODE_SUCC(node); node = NODE_SUCC(node)) {
		char originLine[256], txtLine[256];

		strcpy(originLine, NODE_NAME(node));
		strcpy(txtLine, NODE_NAME(node));

		for (size_t i = 2; i < strlen(originLine); i++) {
			if (originLine[i - 2] == '%') {
				sprintf(txtLine, originLine, va_arg(argument, uint32));
				i = strlen(originLine) + 1;
			}
		}

		CreateNode(txtList, 0, txtLine);
	}

	RemoveList(originList);
	va_end(argument);
	return txtList;
}

bool TextMgr::keyExists(uint32 textId, const char *key) {
	bool found = false;
	struct Text *txt = (Text *)GetNthNode(_txtBase->_textList, textId);

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

uint32 TextMgr::countKey(const char *key) {
	uint32 i = strlen(key);
	uint32 j, k;

	for (j = 0, k = 0; j < i; j++) {
		if (key[j] == TXT_CHAR_KEY_SEPERATOR)
			k++;
	}

	return k + 1;
}


/* functions - STRING */
char * TextMgr::getNthString(uint32 textId, const char *key, uint32 nth, char *dest) {
	LIST *txtList = goKey(textId, key);
	void *src = GetNthNode(txtList, nth);

	if (src)
		strcpy(dest, NODE_NAME(src));
	else
		strcpy(dest, "");

	RemoveList(txtList);
	return dest;
}

char *TextMgr::getFirstLine(uint32 id, const char *key, char *dest) {
	return getNthString(id, key, 0, dest);
}

void TextMgr::putCharacter(LIST *list, uint16 pos, uint8 c) {
	for (NODE *node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node))
		NODE_NAME(node)[pos] = c;
}

} // End of namespace Clue

#endif

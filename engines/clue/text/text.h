/*
**  $Filename: text/text.h
**  $Release:  1
**  $Revision: 0
**  $Date:     10-03-94
**
**  text interface for "Der Clou!"
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

#ifndef MODULE_TEXT
#define MODULE_TEXT

/* includes */
#include "clue/list/list.h"
#include "common/str.h"

namespace Clue {
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

	char *getLine(struct Text *txt, uint8 lineNr);

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

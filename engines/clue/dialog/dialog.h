/*
**  $Filename: dialog/dialog.h
**  $Release:  0
**  $Revision: 0.1
**  $Date:     07-04-94
**
**  dialog functions for "Der Clou!"
**
**   (c) 1994 ...and avoid panic by, H. Gaberschek
**      All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_DIALOG
#define MODULE_DIALOG

#include "clue/text.h"

namespace Clue {

class NewNode;
class NewDynDlgNode : public NewNode {
public:
	byte _knownBefore;       /* wie gut Sie bekannt sein müssen */
	byte _knownAfter;        /* wie gut Sie danach bekannt sind ! */

	NewDynDlgNode() { _knownBefore = _knownAfter = 0; }
	~NewDynDlgNode() {}
};

void PlayFromCDROM();

#define   DLG_TALKMODE_BUSINESS    1
#define   DLG_TALKMODE_STANDARD    2

extern void DynamicTalk(uint32 Person1ID, uint32 Person2ID, byte TalkMode);

extern byte Say(uint32 TextID, byte activ, uint16 person, const char *text);

extern uint32 Talk();

} // End of namespace Clue

#endif

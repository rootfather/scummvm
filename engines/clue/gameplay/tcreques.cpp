/*
**  $Filename: gameplay/tcreques.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     08-04-94
**
**
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

#include "clue/gameplay/gp_app.h"
#include "clue/present/interac.h"
#include "clue/planing/player.h"


namespace Clue {

void PrintStatus(Common::String text) {
	_menuGc->setMode(GFX_JAM_1);
	gfxSetRect(0, 320);
	_menuGc->setPens(249, 254, GFX_SAME_PEN);
	_menuGc->gfxPrint(text, 10, GFX_PRINT_SHADOW | GFX_PRINT_CENTER);
}

void ShowTheClouRequester() {
	PlayerNode *player = (PlayerNode *)dbGetObject(Player_Player_1);

	inpTurnESC(false);

	ShowMenuBackground();
	ShowMenuBackground();

	NewList<NewNode>* menu = g_clue->_txtMgr->goKey(MENU_TXT, "ESCMenu_STD");

	inpTurnFunctionKey(false);

	byte choices = Menu(menu, (_gamePlayMode & GP_MODE_DEMO) ? 3 : 15, 0, nullptr, 0);

	inpTurnFunctionKey(true);

	switch (choices) {
	case 1:
		player->CurrScene = SCENE_THE_END;
		ShowMenuBackground();
		break;
	case 2:     /* save, then...*/
		tcSaveTheClou();
	// No break on purpose
	case 0:     /* continue playing */
		player->CurrScene = 0;
		ShowMenuBackground();
		tcRefreshLocationInTitle(_film->getLocation());
		break;
	case 3:     /* load */
		tcLoadTheClou();
		ShowMenuBackground();
		break;
	default:
		break;
	}

	menu->removeList();
	inpTurnESC(true);
}

} // End of namespace Clue

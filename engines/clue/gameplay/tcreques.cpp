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
	gfxSetDrMd(m_gc, GFX_JAM_1);
	gfxSetRect(0, 320);

	gfxSetPens(m_gc, 249, 254, GFX_SAME_PEN);

	gfxPrint(m_gc, text, 10, GFX_PRINT_SHADOW | GFX_PRINT_CENTER);
}

uint32 ShowTheClouRequester(int32 error_class) {
	NewList<NewNode> *menu;
	uint32 answer = 0;
	byte choices;
	PlayerNode *player = (PlayerNode *)dbGetObject(Player_Player_1);

	inpTurnESC(false);

	ShowMenuBackground();

	switch (error_class) {
	case No_Error:

		ShowMenuBackground();

		menu = g_clue->_txtMgr->goKey(MENU_TXT, "ESCMenu_STD");

		inpTurnFunctionKey(false);

		if (_gamePlayMode & GP_MODE_DEMO)
			choices = Menu(menu, 3, 0, nullptr, 0);
		else
			choices = Menu(menu, 15, 0, NULL, 0);

		inpTurnFunctionKey(true);

		switch (choices) {
		case 0:     /* continue playing */
			player->CurrScene = 0;
			ShowMenuBackground();
			tcRefreshLocationInTitle(_film->getLocation());
			break;
		case 1:
			player->CurrScene = SCENE_THE_END;
			ShowMenuBackground();
			break;
		case 2:
			tcSaveTheClou();
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

		break;
	case Internal_Error:
		PrintStatus("Gravierender Fehler !");
		inpWaitFor(INP_LBUTTONP);
		ShowMenuBackground();
		break;
	case No_Mem:
		PrintStatus("Speichermangel !! - No Memory !!");
		inpWaitFor(INP_LBUTTONP);
		ShowMenuBackground();
		break;
	case Disk_Defect:
		PrintStatus("Disk Defekt !");
		inpWaitFor(INP_LBUTTONP);
		ShowMenuBackground();
		break;
	}

	inpTurnESC(true);

	return answer;
}

} // End of namespace Clue

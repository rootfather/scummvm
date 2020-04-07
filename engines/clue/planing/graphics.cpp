/*
**      $Filename: planing/graphics.c
**      $Release:  1
**      $Revision: 0
**      $Date:     24-04-94
**
**      planing.graphics for "Der Clou!"
**
** (c) 1994 ...and avoid panic by, Kaweh Kazemi
**      All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "clue/base/base.h"

namespace Clue {

void plPrintInfo(const char *person) {
	char info[80];
	sprintf(info, "%s", &person[1]);

	gfxSetRect(2, 320);
	gfxSetPens(m_gc, 249, GFX_SAME_PEN, GFX_SAME_PEN);
	gfxPrint(m_gc, info, 12, GFX_PRINT_CENTER);
}

void plMessage(const char *msg, byte flags) {
	NewList<NewNode> *m = g_clue->_txtMgr->goKey(PLAN_TXT, msg);

	if (flags & PLANING_MSG_REFRESH)
		ShowMenuBackground();

	if (m)
		plPrintInfo(m->getListHead()->_name.c_str());

	m->removeList();

	if (flags & PLANING_MSG_WAIT) {
		inpSetWaitTicks(140L);
		inpWaitFor(INP_LBUTTONP | INP_TIME);
		inpSetWaitTicks(0L);
	}
}

void plMessage(Common::String msg, byte flags) {
	plMessage(msg.c_str(), flags);
}
	
void plDisplayTimer(uint32 time, byte doSpotsImmediatly) {
	/* 2014-06-28 LucyG : static */
	static uint32 oldTimer = (uint32) -1;

	if (!time)
		time = CurrentTimer(plSys) / PLANING_CORRECT_TIME;

	if (GamePlayMode & GP_GUARD_DESIGN) {
		char info[80];
		sprintf(info, "x:%d, y:%d   %s %.2d:%.2d:%.2d %s",
		        livGetXPos(Planing_Name[CurrentPerson]),
		        livGetYPos(Planing_Name[CurrentPerson]), txtTimer.c_str(),
		        (uint32)(time / 3600), (uint32)((time / 60) % 60),
		        (uint32)(time % 60), txtSeconds.c_str());

		gfxSetPens(m_gc, 0, 0, 0);
		gfxRectFill(m_gc, 120, 0, 320, 10);

		gfxSetRect(2, 320);
		gfxSetPens(m_gc, 248, GFX_SAME_PEN, GFX_SAME_PEN);
		gfxPrint(m_gc, info, 2, GFX_PRINT_RIGHT);
	} else {
		char info[80];
		sprintf(info, "%s %.2d:%.2d:%.2d %s", txtTimer.c_str(), (uint32)(time / 3600),
		        (uint32)((time / 60) % 60), (uint32)(time % 60), txtSeconds.c_str());

		gfxSetPens(m_gc, 0, 0, 0);
		gfxRectFill(m_gc, 220, 0, 320, 10);

		gfxSetRect(2, 320);
		gfxSetPens(m_gc, 248, GFX_SAME_PEN, GFX_SAME_PEN);
		gfxPrint(m_gc, info, 2, GFX_PRINT_RIGHT);
	}

	if (doSpotsImmediatly || (oldTimer != CurrentTimer(plSys))) {
		oldTimer = CurrentTimer(plSys);
		lsMoveAllSpots(time);
	}
}

void plDisplayInfo() {
	Common::String info = dbGetObjectName(PersonsList->getNthNode(CurrentPerson)->_nr);

	gfxSetPens(m_gc, 0, 0, 0);
	gfxRectFill(m_gc, 0, 0, 120, 10);

	gfxSetRect(2, 320);
	gfxSetPens(m_gc, 248, GFX_SAME_PEN, GFX_SAME_PEN);
	gfxPrint(m_gc, info, 2, GFX_PRINT_LEFT | GFX_PRINT_SHADOW);
}

byte plSay(const char *msg, uint32 persId) {
	NewList<NewNode> *l = g_clue->_txtMgr->goKey(PLAN_TXT, msg);

	SetPictID(((Person) dbGetObject(PersonsList->getNthNode(persId)->_nr))->PictID);

	inpTurnESC(0);
	inpTurnFunctionKey(0);

	byte choice = Bubble(l, 0, NULL, 200);

	inpTurnFunctionKey(1);
	inpTurnESC(1);

	l->removeList();

	plDisplayTimer(0, 1);
	plDisplayInfo();

	return choice;
}

void plDrawWait(uint32 sec) {
	char time[10];
	sprintf(time, "%.2d:%.2d", (uint32)(sec / 60), (uint32)(sec % 60));

	gfxSetDrMd(m_gc, GFX_JAM_2);
	gfxSetPens(m_gc, 248, GFX_SAME_PEN, GFX_SAME_PEN);
	gfxSetRect(0, 320);
	gfxPrint(m_gc, time, 31, GFX_PRINT_CENTER);
	gfxSetDrMd(m_gc, GFX_JAM_1);
}

void plRefresh(uint32 ItemId) {
	LSObject obj = (LSObject) dbGetObject(ItemId);

	if (lsIsLSObjectInActivArea(obj))
		lsFastRefresh(obj);

	if (lsGetStartArea() == lsGetActivAreaID())
		lsShowEscapeCar();  /* repaint car */

	lsRefreshAllLootBags(); /* repaint all LootBags */
}

} // End of namespace Clue

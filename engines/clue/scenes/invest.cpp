/*
**  $Filename: scenes/invest.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-02-94
**
**   functions for investigations for "Der Clou!"
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

#include "clue/scenes/scenes.h"
#include "clue/sound/newsound.h"
#include "clue/clue.h"
#include "clue/present/interac.h"
#include "clue/data/dataappl.h"


namespace Clue {

static uint32 tcShowPatrol(NewList<NewNode> *bubble_l, Common::String c_time, Common::String patr, byte first, BuildingNode *bui, uint32 raise) {
	uint32 choice = 0;
	Common::String patrol = c_time + ' ' + patr;

	bubble_l->createNode(patrol);

	SetBubbleType(THINK_BUBBLE);

	Bubble(bubble_l, first, nullptr, 140);
	choice = GetExtBubbleActionInfo();

	tcAddBuildExactlyness(bui, raise);

	ShowTime(0);

	return choice;
}

void Investigate(const char *location) {
	uint32 minutes = 0, choice = 0, first = 0;

	uint32 buiID = GetObjNrOfBuilding(_film->getLocation());
	BuildingNode *bui = (BuildingNode *)dbGetObject(buiID);

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		if (buiID == Building_Buckingham_Palace) {
			NewList<NewNode> *bubble_l = g_clue->_txtMgr->goKey(INVESTIGATIONS_TXT, "BuckinghamBeobachtet");
			SetBubbleType(THINK_BUBBLE);
			Bubble(bubble_l, 0, nullptr, 0);
			bubble_l->removeList();
			return;
		}
	}

	if (!(_gamePlayMode & GP_MODE_MUSIC_OFF))
		g_clue->_sndMgr->sndPlaySound("invest.bk", 0);

	inpTurnESC(false);

	ShowMenuBackground();   /* Bildschirmaufbau */
	ShowTime(0);
	inpSetWaitTicks(50);

	gfxSetRect(0, 320);
	gfxSetPens(l_gc, 249, GFX_SAME_PEN, 0);

	Common::String line = g_clue->_txtMgr->getFirstLine(INVESTIGATIONS_TXT, "Abbrechen");
	Common::String patr = g_clue->_txtMgr->getFirstLine(INVESTIGATIONS_TXT, "Patrolie");

	gfxPrint(m_gc, line, 24, GFX_PRINT_CENTER);

	/* Beobachtungstexte von Disk lesen */
	NewList<NewNode> *origin = g_clue->_txtMgr->goKey(INVESTIGATIONS_TXT, location);
	NewList<NewNode> *bubble_l = new NewList<NewNode>;
	uint32 count = origin->getNrOfNodes();
	uint32 guarding = (uint32) tcRGetGRate(bui);
	uint32 patrolCount = (270 - guarding) / 4 + 1;

	/* Wissensgewinn pro Ereignis =
	 * (255-guarding) = alle wieviel Minuten Streife kommt
	 * count = diverse andere Ereignisse
	 * 1439 / (255- guarding) = Anzahl der Streifen / Tag
	 * 3 = Konstante zur Beschleunigung (GamePlay)
	 */

	uint32 raise = 255 / (1439 / patrolCount + count + 1) + 3;

	hasSet(Person_Matt_Stuvysunt, buiID);

	/* bis zur 1. Meldung Zeit vergehen lassen! */
	NewNode *nextMsg;
	for (nextMsg = nullptr; nextMsg == nullptr;) {
		Common::String c_time = buildTime(_film->getMinute());

		if (!(_film->getMinute() % 60))
			ShowTime(0);

		for (NewNode *n = origin->getListHead(); n->_succ; n = n->_succ) {
			if (strncmp(n->_name.c_str(), c_time.c_str(), 5) == 0)
				nextMsg = n;
		}

		if (!nextMsg)
			addVTime(1);

		if (_film->getMinute() % patrolCount == 0)
			choice = tcShowPatrol(bubble_l, c_time, patr, first++, bui, raise);

		if (g_clue->calcRandomNr(0, 6) == 1)
			tcAddBuildStrike(bui, 1);
	}

	while (minutes < MINUTES_PER_DAY && !(choice & INP_LBUTTONP) && !(choice & INP_RBUTTONP) && !(choice & INP_ESC)) {
		choice = 0;

		/* Anzeigen jeder vollen Stunde */
		if (_film->getMinute() % 60 == 0)
			ShowTime(0);

		Common::String c_time = buildTime(_film->getMinute());

		/* je nach Bewachungsgrad Meldung : "Patrolie" einsetzen ! */
		if (_film->getMinute() % patrolCount == 0)
			choice = tcShowPatrol(bubble_l, c_time, patr, first++, bui, raise);

		/* Überprüfen ob zur aktuellen Zeit (time) etwas geschieht : */
		if (!choice) {
			if (strncmp(nextMsg->_name.c_str(), c_time.c_str(), 5) == 0) {
				if (_film->getMinute() % 60 != 0)
					ShowTime(0);

				bubble_l->createNode(nextMsg->_name);

				SetBubbleType(THINK_BUBBLE);

				Bubble(bubble_l, (byte) first++, nullptr, 140);
				choice = GetExtBubbleActionInfo();

				tcAddBuildExactlyness(bui, raise);

				ShowTime(0);

				if (nextMsg->_succ->_succ)
					nextMsg = nextMsg->_succ;
				else
					nextMsg = origin->getListHead();
			}
		}

		/* Zeit erhöhen und nach dem Spieler sehen ! */
		if (g_clue->calcRandomNr(0, 6) == 1)
			tcAddBuildStrike(bui, 1);

		addVTime(1);
		minutes++;

		if (!choice)
			choice = inpWaitFor(INP_TIME | INP_LBUTTONP | INP_RBUTTONP | INP_ESC);
	}

	bubble_l->removeList();

	if (minutes >= 1440) {  /* wurde 24 Stunden lang beobachtet ? */
		bubble_l = g_clue->_txtMgr->goKey(INVESTIGATIONS_TXT, "24StundenBeobachtet");
		SetBubbleType(THINK_BUBBLE);
		Bubble(bubble_l, 0, nullptr, 0);
		bubble_l->removeList();
	}

	origin->removeList();

	Present(buiID, "Building", InitBuildingPresent);

	/* Im Plan dazuaddieren : wie lange beobachtet */
	/* wenn Gebäiude 2 mal beobachtet wird -> Auffällig !! */
	/* Auffällugkeitgrad erhöhen */
	/* Ergebnisse ! (Genauigkeit) */
	/* Abschlußbericht zeigen ! */

	ShowMenuBackground();
	tcRefreshLocationInTitle(_film->getLocation());

	inpSetWaitTicks(0);
	ShowTime(0);

	if (!(_gamePlayMode & GP_MODE_MUSIC_OFF))
		tcPlayStreetSound();

	if (_gamePlayMode & GP_MODE_DEMO) {
		uint8 palette[GFX_PALETTE_SIZE];

		g_clue->_animMgr->SuspendAnim();
		gfxPrepareRefresh();

		gfxGetPaletteFromReg(palette, 0, 256);

		gfxShow(224, GFX_NO_REFRESH | GFX_FADE_OUT | GFX_BLEND_UP | GFX_ONE_STEP, 2, -1, -1);

		inpWaitFor(INP_LBUTTONP);

		gfxChangeColors(l_gc, 0, GFX_FADE_OUT, NULL);
		gfxClearArea(l_gc);
		gfxChangeColors(l_gc, 0, GFX_BLEND_UP, palette);

		gfxRefresh();

		g_clue->_animMgr->ContinueAnim();
	}

	inpTurnESC(true);
}

} // End of namespace Clue

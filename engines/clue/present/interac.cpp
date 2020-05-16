/*
**  $Filename: present/interac.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     05-04-94
**
**  interactiv presentations for "Der Clou!"
**
**   (c) 1994 ...and avoid panic by K. Kazemi, H. Gaberschek
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
#include "clue/clue.h"
#include "clue/present/interac_p.h"
#include "clue/present/interac.h"
#include "clue/data/dataappl.h"
#include "clue/scenes/scenes.h"


namespace Clue {

void SetBubbleType(uint16 type) {
	CurrentBubbleType = type;
}

void SetPictID(uint16 PictID) {
	ActivPersonPictID = PictID;
}

byte GetExtBubbleActionInfo() {
	return ExtBubbleActionInfo;
}

void SetMenuTimeOutFunc(void (*func)()) {
	MenuTimeOutFunc = func;
}

bool ChoiceOk(byte choice, byte exit, NewObjectList<dbObjectNode> *l) {
	if (choice == GET_OUT || choice == exit)
		return false;

	if (l && !l->isEmpty()) {
		dbObjectNode *objNode = l->getNthNode(choice);

		if (!objNode->_nr && !objNode->_type && !objNode->_fakePtr)
			return false;
	}

	return true;
}

bool ChoiceOkHack(byte choice, byte exit, NewList<NewNode> *l) {
	warning("ChoiceOkHack");
	if (choice == GET_OUT || choice == exit)
		return false;

	// The original was calling ChoiceOK with lists of NewNode in some cases.
	// This accidentally work as the extra fields of dbObjectNode were in fact
	// the pointers of the next node, and thus were never set to 0.
	// So, when the function was called, as a node is behind (or the tail), the
	// check was never fulfilled and the function was never returning false.

	return true;
}

static void DrawMenu(NewList<NewNode>* menu, byte nr, int32 mode) {
	if (mode == ACTIV_POSS)
		gfxSetPens(m_gc, 249, GFX_SAME_PEN, GFX_SAME_PEN);
	else
		gfxSetPens(m_gc, 248, GFX_SAME_PEN, GFX_SAME_PEN);

	Common::String m1 = "";
	Common::String m2 = "";

	int32 x = 8;
	int16 lastx = 0;
	byte i;

	for (i = 0; i <= nr; i += 2) {
			m1 = menu->getNthNode(i)->_name;

		if (i + 1 <= nr)
			m2 = menu->getNthNode(i + 1)->_name;
		else
			m2 = "";

		lastx = MAX(gfxTextWidth(m_gc, m1), gfxTextWidth(m_gc, m2));
		x += lastx;
	}

	if (nr == i - 2)
		gfxPrintExact(m_gc, m1, x + 8 * nr - lastx, TXT_1ST_MENU_LINE_Y);
	else if (!m2.empty())
		gfxPrintExact(m_gc, m2, x + 8 * (nr - 1) - lastx, TXT_2ND_MENU_LINE_Y);
}

static char SearchActiv(int16 delta, byte activ, uint32 possibility, byte max) {
	do {
		activ = MIN<uint8>(activ + delta, max);

		if (possibility & (1 << activ))
			return (char) activ;
	} while (activ > 0 && activ < max);

	return -1;
}

static char SearchMouseActiv(uint32 possibility, byte max) {
	/* MOD : 14.12.93 hg */
	uint16 x, y;
	gfxGetMouseXY(m_gc, &x, &y);

	if (y > TXT_1ST_MENU_LINE_Y - 10 && y < TXT_2ND_MENU_LINE_Y + 10) {
		max -= 1;

		int activ;
		for (activ = 0; x >= MenuCoords[activ] && activ < max / 2 + 1; activ++)
			;

		activ = (activ - 1) * 2;

		if (y > TXT_1ST_MENU_LINE_Y + 7)
			activ++;

		if (activ > max)
			activ = max;

		if (possibility & (1 << activ))
			return (char) activ;
	}

	return -1;
}

void RefreshMenu() {
	if (refreshMenu) {
		byte max = refreshMenu->getNrOfNodes();

		for (byte i = 0; i < max; i++) {
			if (refreshPoss & (1 << i))
				DrawMenu(refreshMenu, i, INACTIV_POSS);
		}

		DrawMenu(refreshMenu, refreshActiv, ACTIV_POSS);
	}
}

byte Menu(NewList<NewNode> *menu, uint32 possibility, byte activ, void (*func)(byte), uint32 waitTime) {
	if (menu && !menu->isEmpty()) {
		if (!possibility)
			return 0;

		uint16 x = 8;
		byte max = 0;

		for (NewNode *n = menu->getListHead(); n->_succ; n = n->_succ, max++) {
			if (max % 2 == 0) {
				MenuCoords[max / 2] = x - 8;

				uint16 l1 = gfxTextWidth(m_gc, n->_name);
				uint16 l2 = 0;

				if (n->_succ->_succ)
					l2 = gfxTextWidth(m_gc, n->_succ->_name);

				x += MAX(l1, l2) + 16;
			}
		}

		for (byte i = 0; i < max; i++) {
			if (possibility & (1 << i))
				DrawMenu(menu, i, INACTIV_POSS);
		}

		DrawMenu(menu, activ, ACTIV_POSS);

		if (func)
			func(activ);

		if (waitTime)
			inpSetWaitTicks(waitTime);

		bool endLoop = false;
		while (!endLoop) {
			int32 action = INP_LEFT | INP_RIGHT | INP_UP | INP_DOWN | INP_LBUTTONP;

			if (waitTime)
				action |= INP_TIME;

			action = inpWaitFor(action);

			if (action & INP_TIME) {
				refreshMenu = nullptr;

				if (MenuTimeOutFunc)
					MenuTimeOutFunc();
				else
					return (byte)TXT_MENU_TIMEOUT;
			}

			if (action & INP_FUNCTION_KEY) {
				refreshMenu = menu;
				refreshPoss = possibility;
				refreshActiv = activ;
				return (byte)-1;
			}

			if ((action & INP_ESC) || (action & INP_RBUTTONP))
				return GET_OUT;

			if (action & INP_LBUTTONP)
				endLoop = true;

			if (action & INP_MOUSE) {   /* MOD : 14.12.93 hg */
				char nextActiv = SearchMouseActiv(possibility, max);
				if (nextActiv != (char)-1) {
					if (nextActiv != activ) {
						DrawMenu(menu, activ, INACTIV_POSS);
						activ = nextActiv;
						DrawMenu(menu, activ, ACTIV_POSS);

						if (func)
							func(activ);
					}
				}
			}
			else {
				if ((action & INP_UP) && (activ & 1)) {
					char nextActiv = SearchActiv(-1, activ, possibility, max);
					if (nextActiv != (char)-1) {
						if (!(nextActiv & 1)) {
							DrawMenu(menu, activ, INACTIV_POSS);
							activ = nextActiv;
							DrawMenu(menu, activ, ACTIV_POSS);

							if (func)
								func(activ);
						}
					}
				}

				if ((action & INP_DOWN) && !(activ & 1)) {
					char nextActiv = SearchActiv(+1, activ, possibility, max);
					if (nextActiv != (char)-1) {
						if (nextActiv & 1) {
							DrawMenu(menu, activ, INACTIV_POSS);
							activ = nextActiv;
							DrawMenu(menu, activ, ACTIV_POSS);

							if (func)
								func(activ);
						}
					}
				}

				if (action & INP_LEFT) {
					char nextActiv = SearchActiv(-2, activ, possibility, max);
					if (nextActiv != (char)-1) {
						DrawMenu(menu, activ, INACTIV_POSS);
						activ = nextActiv;
						DrawMenu(menu, activ, ACTIV_POSS);

						if (func)
							func(activ);
					}
				}

				if (action & INP_RIGHT) {
					char nextActiv = SearchActiv(+2, activ, possibility, max);
					if (nextActiv != (char)-1) {
						DrawMenu(menu, activ, INACTIV_POSS);
						activ = nextActiv;
						DrawMenu(menu, activ, ACTIV_POSS);

						if (func)
							func(activ);
					}
				}
			}
		}

		refreshMenu = nullptr;
		return activ;
	}

	refreshMenu = nullptr;
	return activ;
}

void DrawBubble(NewList<NewNode> *bubble, uint8 firstLine, uint8 activ, _GC *gc, uint32 max) {
	gfxScreenFreeze();
	gfxSetPens(gc, 224, 224, 224);
	gfxRectFill(gc, X_OFFSET, 3, X_OFFSET + INT_BUBBLE_WIDTH, 49);

	if (firstLine) {
		gfxSetGC(gc);
		gfxShow(145, GFX_OVERLAY | GFX_NO_REFRESH, 0, X_OFFSET + 135, 5);
	}

	if (max > firstLine + NRBLINES) {
		gfxSetGC(gc);
		gfxShow(146, GFX_OVERLAY | GFX_NO_REFRESH, 0, X_OFFSET + 135, 40);
	}

	for (uint i = firstLine, j = 4; (max < firstLine + NRBLINES) ? i < max : i < firstLine + NRBLINES; i++, j += 9) {
		const char *line = bubble->getNthNode(i)->_name.c_str();

		if (!line)
			break;

		if (*line != '*') {
			gfxSetPens(gc, BG_TXT_COLOR, GFX_SAME_PEN, GFX_SAME_PEN);
			gfxPrintExact(gc, line, X_OFFSET, j + 1);

			gfxSetPens(gc, VG_TXT_COLOR, GFX_SAME_PEN, GFX_SAME_PEN);
			gfxPrintExact(gc, line, X_OFFSET, j);
		} else {
			line = line + 1;

			gfxSetPens(gc, (activ == i) ? BG_ACTIVE_COLOR : BG_INACTIVE_COLOR, GFX_SAME_PEN, GFX_SAME_PEN);
			gfxPrintExact(gc, line, X_OFFSET + 1, j + 1);
			gfxSetPens(gc, (activ == i) ? VG_ACTIVE_COLOR : VG_INACTIVE_COLOR, GFX_SAME_PEN, GFX_SAME_PEN);

			gfxPrintExact(gc, line, X_OFFSET, j);
		}
	}

	gfxScreenThaw(gc, X_OFFSET, 3, INT_BUBBLE_WIDTH, 46);
}

byte Bubble(NewList<NewNode> *bubble, byte activ, void (*func)(byte), uint32 waitTime) {
	int32 max = bubble->getNrOfNodes();

	g_clue->_animMgr->suspendAnim();
	gfxPrepareRefresh();
	gfxScreenFreeze();

	if (activ == GET_OUT)
		activ = 0;

	byte firstVis = 0;
	if (activ >= NRBLINES)
		firstVis = activ - NRBLINES + 1;

	if (func)
		func(activ);

	if (ActivPersonPictID == (uint16) -1)
		gfxShow((uint16) CurrentBubbleType, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);
	else {
		gfxShow((uint16) ActivPersonPictID, GFX_NO_REFRESH | GFX_OVERLAY | GFX_BLEND_UP, 0, -1, -1);
		gfxShow((uint16) CurrentBubbleType, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);

		if (CurrentBubbleType == SPEAK_BUBBLE)
			gfxShow(9, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);

		if (CurrentBubbleType == THINK_BUBBLE)
			gfxShow(10, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);
	}

	gfxSetDrMd(u_gc, GFX_JAM_1);
	gfxSetFont(u_gc, bubbleFont);
	DrawBubble(bubble, firstVis, activ, u_gc, max);

	gfxScreenThaw(u_gc, 0, 0, 320, 67);

	if (g_clue->getFeatures() & ADGF_CD)
		PlayFromCDROM();

	if (waitTime) {
		inpSetWaitTicks(waitTime);
		uint32 action = inpWaitFor(INP_LBUTTONP | INP_TIME | INP_RBUTTONP);

		if (action & INP_LBUTTONP)
			activ = 1;

		if ((action & INP_ESC) || (action & INP_RBUTTONP) || (action & INP_TIME))
			activ = GET_OUT;

		inpSetWaitTicks(0);

		ExtBubbleActionInfo = action;
	} else {
		bool endLoop = false;
		while (!endLoop) {
			uint32 action = inpWaitFor(INP_UP | INP_DOWN | INP_LBUTTONP | INP_RBUTTONP | INP_LEFT | INP_RIGHT);

			ExtBubbleActionInfo = action;

			if ((action & INP_ESC) || (action & INP_RBUTTONP)) {
				activ = GET_OUT;
				endLoop = true;
			}

			if (!endLoop) {
				if ((action & INP_LBUTTONP))
					endLoop = true;

				if (action & INP_MOUSE) {
					uint16 x, y;

					gfxGetMouseXY(u_gc, &x, &y);

					if (x >= X_OFFSET && x <= X_OFFSET + INT_BUBBLE_WIDTH) {
						if (y < 4 && firstVis > 0) {    /* Scroll up */
							while (y < 4 && firstVis > 0) {
								firstVis -= 1;
								activ = firstVis;

								DrawBubble(bubble, firstVis, activ, u_gc, max);

								if (func)
									func(activ);

								inpDelay(20);

								gfxGetMouseXY(u_gc, nullptr, &y);
							}
						} else if (y > 48 && y <= 58 && firstVis < max - 5) {   /* Scroll down */
							while (y > 48 && y <= 58 && firstVis < max - 5) {
								++firstVis;
								
								activ = MIN(firstVis + 4, max - 1);

								DrawBubble(bubble, firstVis, activ, u_gc, max);

								if (func)
									func(activ);

								inpDelay(20);

								gfxGetMouseXY(u_gc, nullptr, &y);
							}
						} else if (y >= 4 && y <= 48) {
							byte newactiv = firstVis + (y - 4) / 9;

							if (newactiv != activ) {
								activ = CLIP(newactiv, firstVis, MIN<byte>(firstVis + 4, (byte) (max - 1)));

								DrawBubble(bubble, firstVis, activ, u_gc, max);

								if (func)
									func(activ);
							}
						}
					}
				} else {
					if (action & INP_UP) {
						if (activ > 0) {
							int cl = abs(firstVis - activ) + 1;

							while (activ > 0 && cl) {
								activ--;
								cl--;
								
								if (bubble->getNthNode(activ)->_name.c_str()[0] == '*')
									break;
							}

							if (activ < firstVis)
								firstVis = activ;

							DrawBubble(bubble, firstVis, activ, u_gc, max);

							if (func)
								func(activ);
						}
					}

					if (action & INP_DOWN) {
						if (activ < max - 1) {
							int cl = NRBLINES - abs(firstVis - activ);

							while ((activ < max - 1) && cl) {
								activ++;
								cl--;

								if (bubble->getNthNode(activ)->_name.c_str()[0] == '*')
									break;
							}

							if (activ > (firstVis + NRBLINES - 1))
								firstVis = activ - NRBLINES + 1;

							DrawBubble(bubble, firstVis, activ, u_gc, max);

							if (func)
								func(activ);
						}
					}
				}
			}
		}
	}

	if (activ != GET_OUT) {
		if (bubble->getNthNode(0)->_name.c_str()[0] != '*')
			activ = 0;
	}

	SetBubbleType(SPEAK_BUBBLE);
	SetPictID(MATT_PICTID);

	gfxRefresh();

	g_clue->_animMgr->continueAnim();

	return activ;
}

} // End of namespace Clue

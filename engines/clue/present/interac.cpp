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

namespace Clue {

void SetBubbleType(uint16 type) {
	CurrentBubbleType = type;
}

void SetPictID(uint16 PictID) {
	ActivPersonPictID = PictID;
}

byte GetExtBubbleActionInfo(void) {
	return ExtBubbleActionInfo;
}

void SetMenuTimeOutFunc(void (*func)(void)) {
	MenuTimeOutFunc = func;
}

bool ChoiceOk(byte choice, byte exit, LIST *l) {
	if (choice == GET_OUT)
		return false;

	if (choice == exit)
		return false;

	if (l && !LIST_EMPTY(l)) {
		struct ObjectNode *objNode =
		    (struct ObjectNode *) GetNthNode(l, choice);

		if (!objNode->nr && !objNode->type && !objNode->data)
			return false;
	}

	return true;
}

static void DrawMenu(LIST *menu, byte nr, int32 mode) {
	if (mode == ACTIV_POSS)
		gfxSetPens(m_gc, 249, GFX_SAME_PEN, GFX_SAME_PEN);
	else
		gfxSetPens(m_gc, 248, GFX_SAME_PEN, GFX_SAME_PEN);

	char *m1 = NULL;
	char *m2 = NULL;
	int32 x = 8;
	int32 lastx = 0;
	byte i;

	for (i = 0; i <= nr; i += 2) {
		m1 = NODE_NAME(GetNthNode(menu, i));

		if ((i + 1) <= nr)
			m2 = NODE_NAME(GetNthNode(menu, i + 1));
		else
			m2 = NULL;

		if (m2) {
			if (strlen(m1) > strlen(m2)) {
				lastx = gfxTextWidth(m_gc, m1, strlen(m1));
				x += lastx;
			} else {
				lastx = gfxTextWidth(m_gc, m2, strlen(m2));
				x += lastx;
			}
		} else {
			lastx = gfxTextWidth(m_gc, m1, strlen(m1));
			x += lastx;
		}
	}

	if (nr == (i - 2))
		gfxPrintExact(m_gc, m1, x + 8 * nr - lastx, TXT_1ST_MENU_LINE_Y);
	else {
		if (m2)
			gfxPrintExact(m_gc, m2, x + 8 * (nr - 1) - lastx,
			              TXT_2ND_MENU_LINE_Y);
	}
}

static char SearchActiv(int16 delta, byte activ, uint32 possibility, byte max) {
	do {
		activ += delta;

		if (activ > max)
			activ = max;

		if (possibility & (1L << activ))
			return (char) activ;
	} while ((activ > 0) && (activ < max));

	return -1;
}

static char SearchMouseActiv(uint32 possibility, byte max) {
	/* MOD : 14.12.93 hg */
	uint16 x, y;
	gfxGetMouseXY(m_gc, &x, &y);

	if ((y > TXT_1ST_MENU_LINE_Y - 10) && (y < TXT_2ND_MENU_LINE_Y + 10)) {
		max -= 1;

		int activ;
		for (activ = 0; (x >= MenuCoords[activ]) && (activ < max / 2 + 1); activ++)
			;

		activ = (activ - 1) * 2;

		if (y > TXT_1ST_MENU_LINE_Y + 7)
			activ++;

		if (activ > max)
			activ = max;

		if (possibility & (1L << activ))
			return (char) activ;
	}

	return -1;
}

void RefreshMenu(void) {
	if (refreshMenu) {
		byte max = GetNrOfNodes(refreshMenu);

		for (byte i = 0; i < max; i++) {
			if (refreshPoss & (1L << i))
				DrawMenu(refreshMenu, i, INACTIV_POSS);
		}

		DrawMenu(refreshMenu, refreshActiv, ACTIV_POSS);
	}
}

byte Menu(LIST *menu, uint32 possibility, byte activ, void (*func)(byte), uint32 waitTime) {
	if (menu && !LIST_EMPTY(menu)) {
		if (!possibility)
			return 0;


		uint16 x = 8;
		NODE *n;
		byte max;

		for (max = 0, n = LIST_HEAD(menu); NODE_SUCC(n); n = NODE_SUCC(n), max++) {
			if ((max % 2) == 0) {
				uint16 l1 = 0, l2 = 0;

				MenuCoords[max / 2] = x - 8;

				l1 = gfxTextWidth(m_gc, NODE_NAME(n),
				                  strlen(NODE_NAME(n)));

				if (NODE_SUCC(NODE_SUCC(n)))
					l2 = gfxTextWidth(m_gc, NODE_NAME(NODE_SUCC(n)),
					                  strlen(NODE_NAME(NODE_SUCC(n))));

				x += MAX(l1, l2) + 16;
			}
		}

		for (byte i = 0; i < max; i++) {
			if (possibility & (1L << i))
				DrawMenu(menu, i, INACTIV_POSS);
		}

		DrawMenu(menu, activ, ACTIV_POSS);

		if (func)
			func(activ);

		if (waitTime)
			inpSetWaitTicks(waitTime);

		bool ende = false;
		while (!ende) {
			int32 action = INP_LEFT | INP_RIGHT | INP_UP | INP_DOWN | INP_LBUTTONP;

			if (waitTime)
				action |= INP_TIME;

			action = inpWaitFor(action);

			if (action & INP_TIME) {
				refreshMenu = NULL;

				if (MenuTimeOutFunc)
					MenuTimeOutFunc();
				else
					return (byte) TXT_MENU_TIMEOUT;
			}

			if (action & INP_FUNCTION_KEY) {
				refreshMenu = menu;
				refreshPoss = possibility;
				refreshActiv = activ;
				return ((byte) - 1);
			}

			if ((action & INP_ESC) || (action & INP_RBUTTONP))
				return GET_OUT;

			if (action & INP_LBUTTONP)
				ende = true;

			if (action & INP_MOUSE) {   /* MOD : 14.12.93 hg */
				char nextActiv = SearchMouseActiv(possibility, max);
				if (nextActiv != ((char) -1)) {
					if (nextActiv != activ) {
						DrawMenu(menu, activ, INACTIV_POSS);
						activ = nextActiv;
						DrawMenu(menu, activ, ACTIV_POSS);

						if (func)
							func(activ);
					}
				}
			} else {
				if ((action & INP_UP) && (activ & 1)) {
					char nextActiv = SearchActiv(-1, activ, possibility, max);
					if (nextActiv != (char) -1) {
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
					if (nextActiv != (char) -1) {
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
					if (nextActiv != (char) -1) {
						DrawMenu(menu, activ, INACTIV_POSS);
						activ = nextActiv;
						DrawMenu(menu, activ, ACTIV_POSS);

						if (func)
							func(activ);
					}
				}

				if (action & INP_RIGHT) {
					char nextActiv = SearchActiv(+2, activ, possibility, max);
					if (nextActiv != (char) -1) {
						DrawMenu(menu, activ, INACTIV_POSS);
						activ = nextActiv;
						DrawMenu(menu, activ, ACTIV_POSS);

						if (func)
							func(activ);
					}
				}
			}
		}

		refreshMenu = NULL;

		return activ;
	}

	refreshMenu = NULL;

	return activ;
}

static void DrawBubble(LIST *bubble, uint8 firstLine, uint8 activ, GC *gc, uint32 max) {
	gfxScreenFreeze();
	gfxSetPens(gc, 224, 224, 224);
	gfxRectFill(gc, X_OFFSET, 3, X_OFFSET + INT_BUBBLE_WIDTH, 49);

	if (firstLine) {
		gfxSetGC(gc);
		gfxShow(145, GFX_OVERLAY | GFX_NO_REFRESH, 0, X_OFFSET + 135, 5);
	}

	if ((max > (firstLine + NRBLINES))) {
		gfxSetGC(gc);
		gfxShow(146, GFX_OVERLAY | GFX_NO_REFRESH, 0, X_OFFSET + 135, 40);
	}

	for (uint i = firstLine, j = 4; (max < firstLine + NRBLINES) ? i < max : i < firstLine + NRBLINES; i++, j += 9) {
		char *line = NODE_NAME(GetNthNode(bubble, i));

		if (!line)
			break;

		if (*line != '*') {
			gfxSetPens(gc, BG_TXT_COLOR, GFX_SAME_PEN, GFX_SAME_PEN);
			gfxPrintExact(gc, line, X_OFFSET, j + 1);

			gfxSetPens(gc, VG_TXT_COLOR, GFX_SAME_PEN, GFX_SAME_PEN);
			gfxPrintExact(gc, line, X_OFFSET, j);
		} else {
			line = line + 1;

			if (activ == i)
				gfxSetPens(gc, BG_ACTIVE_COLOR, GFX_SAME_PEN,
				           GFX_SAME_PEN);
			else
				gfxSetPens(gc, BG_INACTIVE_COLOR, GFX_SAME_PEN,
				           GFX_SAME_PEN);

			gfxPrintExact(gc, line, X_OFFSET + 1, j + 1);

			if (activ == i)
				gfxSetPens(gc, VG_ACTIVE_COLOR, GFX_SAME_PEN,
				           GFX_SAME_PEN);
			else
				gfxSetPens(gc, VG_INACTIVE_COLOR, GFX_SAME_PEN,
				           GFX_SAME_PEN);

			gfxPrintExact(gc, line, X_OFFSET, j);
		}
	}

	gfxScreenThaw(gc, X_OFFSET, 3, INT_BUBBLE_WIDTH, 46);
}


byte Bubble(LIST *bubble, byte activ, void (*func)(byte), uint32 waitTime) {
	int32 max = GetNrOfNodes(bubble);

	SuspendAnim();
	gfxPrepareRefresh();
	gfxScreenFreeze();

	if (activ == GET_OUT)
		activ = 0;

	byte firstVis = 0;
	if (activ >= NRBLINES)
		firstVis = activ - NRBLINES + 1;

	if (func)
		func(activ);

	if (ActivPersonPictID == (uint16) - 1)
		gfxShow((uint16) CurrentBubbleType, GFX_NO_REFRESH | GFX_OVERLAY, 0,
		        -1, -1);
	else {
		gfxShow((uint16) ActivPersonPictID,
		        GFX_NO_REFRESH | GFX_OVERLAY | GFX_BLEND_UP, 0, -1, -1);
		gfxShow((uint16) CurrentBubbleType, GFX_NO_REFRESH | GFX_OVERLAY, 0,
		        -1, -1);

		if (CurrentBubbleType == SPEAK_BUBBLE)
			gfxShow(9, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);

		if (CurrentBubbleType == THINK_BUBBLE)
			gfxShow(10, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);
	}

	gfxSetDrMd(u_gc, GFX_JAM_1);
	gfxSetFont(u_gc, bubbleFont);
	DrawBubble(bubble, firstVis, activ, u_gc, max);

	gfxScreenThaw(u_gc, 0, 0, 320, 67);

	if (g_clue->getFeatures() & GF_CDAUDIO) {
		PlayFromCDROM();
	}

	if (waitTime) {
		inpSetWaitTicks(waitTime);
		uint32 action = inpWaitFor(INP_LBUTTONP | INP_TIME | INP_RBUTTONP);

		if (action & INP_LBUTTONP)
			activ = 1;

		if ((action & INP_ESC) || (action & INP_RBUTTONP)
		        || (action & INP_TIME))
			activ = GET_OUT;

		inpSetWaitTicks(0L);

		ExtBubbleActionInfo = action;
	} else {
		bool ende = false;
		while (!ende) {
			uint32 action =
			    inpWaitFor(INP_UP | INP_DOWN | INP_LBUTTONP | INP_RBUTTONP
			               | INP_LEFT | INP_RIGHT);

			ExtBubbleActionInfo = action;

			if ((action & INP_ESC) || (action & INP_RBUTTONP)) {
				activ = GET_OUT;
				ende = true;
			}

			if (!ende) {
				if ((action & INP_LBUTTONP))
					ende = true;

				if ((action & INP_MOUSE)) {
					uint16 x, y;

					gfxGetMouseXY(u_gc, &x, &y);

					if ((x >= X_OFFSET)
					        && (x <= X_OFFSET + INT_BUBBLE_WIDTH)) {
						if ((y < 4) && (firstVis > 0)) {    /* Scroll up */
							while ((y < 4) && (firstVis > 0)) {
								firstVis -= 1;
								activ = firstVis;

								DrawBubble(bubble, firstVis, activ, u_gc, max);

								if (func)
									func(activ);

								gfxWaitTOS();

								gfxGetMouseXY(u_gc, NULL, &y);
							}
						} else if ((y > 48) && (y <= 58) && (firstVis < (max - 5))) {   /* Scroll down */
							while ((y > 48) && (y <= 58) && (firstVis < (max - 5))) {
								firstVis += 1;
								activ = firstVis + 4;

								if (activ > (max - 1))
									activ = max - 1;

								DrawBubble(bubble, firstVis, activ, u_gc, max);

								if (func)
									func(activ);

								gfxWaitTOS();

								gfxGetMouseXY(u_gc, NULL, &y);
							}
						} else if ((y >= 4) && (y <= 48)) {
							byte newactiv = firstVis + (y - 4) / 9;

							if (newactiv != activ) {
								activ = newactiv;

								if (activ > (max - 1))
									activ = max - 1;

								if (activ < firstVis)
									activ = firstVis;

								if (activ > firstVis + 4)
									activ = firstVis + 4;

								DrawBubble(bubble, firstVis, activ, u_gc, max);

								if (func)
									func(activ);
							}
						}
					}
				} else {
					if ((action & INP_UP)) {
						if (activ > 0) {
							int cl = abs(firstVis - activ) + 1;

							while ((activ > 0) && cl) {
								activ--;
								cl--;

								if (*NODE_NAME(GetNthNode(bubble, activ))
								        == '*')
									break;
							}

							if (activ < firstVis)
								firstVis = activ;

							DrawBubble(bubble, firstVis, activ, u_gc, max);

							if (func)
								func(activ);
						}
					}

					if ((action & INP_DOWN)) {
						if (activ < max - 1) {
							int cl = NRBLINES - abs(firstVis - activ);

							while ((activ < max - 1) && cl) {
								activ++;
								cl--;

								if (*NODE_NAME(GetNthNode(bubble, activ))
								        == '*')
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
		if (*NODE_NAME(GetNthNode(bubble, 0L)) != '*')
			activ = 0;
	}

	SetBubbleType(SPEAK_BUBBLE);
	SetPictID(MATT_PICTID);

	gfxRefresh();

	ContinueAnim();

	return activ;
}

} // End of namespace Clue

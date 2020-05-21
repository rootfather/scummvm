/*
**  $Filename: present/present.h
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-02-94
**
**  basic functions for "Der Clou!"
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

#include "clue/present/present.h"
#include "clue/present/present_p.h"
#include "clue/present/presenta.cpp"
#include "clue/present/interac.h"


namespace Clue {

struct PresentControl PresentControl = {nullptr, 0, 0, 0, 0, 0 };

void DrawPresent(NewList<PresentationInfoNode> *present, uint8 firstLine, _GC *gc, uint8 max) {
	gfxScreenFreeze();
	gc->setPens(224, 224, 224);
	gc->rectFill(88, 3, 320, 49);

	if (firstLine) {
		gfxSetGC(gc);
		gfxShow(145, GFX_OVERLAY | GFX_NO_REFRESH, 0, 120, 5);
	}

	if (max > firstLine + NRBLINES) {
		gfxSetGC(gc);
		gfxShow(146, GFX_OVERLAY | GFX_NO_REFRESH, 0, 120, 40);
	}

	for (uint i = firstLine, j = 4;
	        (max < firstLine + NRBLINES) ? i < max : i < firstLine + NRBLINES;
	        i++, j += 9) {
		PresentationInfoNode *p = present->getNthNode(i);

		Common::String txt = p->_name;

		switch (p->_presentHow) {
		case PRESENT_AS_TEXT:
			if (!p->_extendedText.empty()) {
				for (uint k = 0; k < 57 - p->_name.size() - p->_extendedText.size(); k++)
					txt += " ";

				txt += p->_extendedText;
			}
			break;

		case PRESENT_AS_BAR: {
			gc->setPens(250, 250, 251);
			gc->rectFill(205, j, 315, j + 7);
			gc->setPens(251, 251, 251);
			gc->rectFill(205, j, 205 + ((315 - 205) * p->_extendedNr) / p->_maxNr, j + 7);

			gc->setPens(249, 252, 253);

			gfxSetRect(206, 315 - 205);
			Common::String s = Common::String::format("%u %%", (p->_extendedNr * 100) / p->_maxNr);

			gc->gfxPrint(s, j, GFX_PRINT_CENTER);
			}
			break;
		default:
			break;
		}

		gc->setPens(252, 224, GFX_SAME_PEN);
		gc->gfxPrintExact(txt, 89, j + 1);

		gc->setPens(254, 224, GFX_SAME_PEN);
		gc->gfxPrintExact(txt, 88, j);
	}

	gc->gfxScreenThaw(88, 3, 228, 46);
}

uint8 Present(uint32 nr, const char *presentationText, void (*initPresentation)(uint32, NewList<PresentationInfoNode> *, NewList<NewNode> *)) {
	NewList<PresentationInfoNode> *presentationData = new NewList<PresentationInfoNode>;

	g_clue->_animMgr->suspendAnim();

	gfxPrepareRefresh();

	inpTurnFunctionKey(false);

	if (dbIsObject(nr, Object_Person)) {
		PersonNode *obj = (PersonNode *) dbGetObject(nr);

		gfxShow(obj->PictID, GFX_NO_REFRESH | GFX_OVERLAY | GFX_BLEND_UP, 0, -1, -1);
	} else if (dbIsObject(nr, Object_Evidence)) {
		EvidenceNode *e = (EvidenceNode *)dbGetObject(nr);

		gfxShow((uint16)((PersonNode *) dbGetObject(e->pers))->PictID,
		        GFX_NO_REFRESH | GFX_OVERLAY | GFX_BLEND_UP, 0, -1, -1);
	} else if (dbIsObject(nr, Object_Tool))
		gfxShow(((ToolNode *)(dbGetObject(nr)))->PictID,
		        GFX_NO_REFRESH | GFX_OVERLAY | GFX_BLEND_UP, 0, -1, -1);
	else if (dbIsObject(nr, Object_Loot))
		gfxShow(((LootNode *)(dbGetObject(nr)))->PictID,
		        GFX_NO_REFRESH | GFX_OVERLAY | GFX_BLEND_UP, 0, -1, -1);

	gfxShow((uint16) BIG_SHEET, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);   /* nur die Farben ! */

	NewList<NewNode> *list = g_clue->_txtMgr->goKey(PRESENT_TXT, presentationText);

	if (list)
		initPresentation(nr, presentationData, list);

	uint8 max = presentationData->getNrOfNodes();

	_upperGc->setMode(GFX_JAM_1);
	_upperGc->setFont(bubbleFont);

	uint8 firstVis = 0;
	DrawPresent(presentationData, firstVis, _upperGc, max);

	uint8 exit = 0;
	while (!exit) {
		uint32 action = inpWaitFor(INP_UP | INP_DOWN | INP_LBUTTONP | INP_RBUTTONP);

		if ((action & INP_ESC) || (action & INP_RBUTTONP))
			exit = 1;

		if ((action & INP_LBUTTONP))
			exit = 2;

		if ((action & INP_MOUSE)) {
			uint16 y;

			_upperGc->gfxGetMouseXY(NULL, &y);

			while (y < 9 && firstVis > 0) { /* Scroll up */
				firstVis--;
				DrawPresent(presentationData, firstVis, _upperGc, max);

				inpDelay(20);

				_upperGc->gfxGetMouseXY(NULL, &y);
			}

			while (y > 48 && y <= 58 && firstVis < (max - 5)) { /* Scroll down */
				firstVis++;
				DrawPresent(presentationData, firstVis, _upperGc, max);

				inpDelay(20);

				_upperGc->gfxGetMouseXY(NULL, &y);
			}
		} else {
			if (action & INP_UP) {
				if (firstVis > 0) {
					firstVis--;
					DrawPresent(presentationData, firstVis, _upperGc, max);
				}
			}

			if (action & INP_DOWN) {
				if (max - NRBLINES > 0 && firstVis < max - NRBLINES) {
					firstVis++;
					DrawPresent(presentationData, firstVis, _upperGc, max);
				}
			}
		}
	}

	list->removeList();
	presentationData->removeList();

	gfxRefresh();

	g_clue->_animMgr->continueAnim();

	if (exit == 1)
		return GET_OUT;
	else
		return ((uint8)(exit - 1));
}

static PresentationInfoNode *AddPresentInfo(NewList<PresentationInfoNode> *l, uint32 max, NewList<NewNode> *texts, uint16 textNr) {
	Common::String name = Common::String("");

	if (textNr != ((uint16) -1))
		name = texts->getNthNode(textNr)->_name;

	PresentationInfoNode *p = l->createNode(name);
	p->_maxNr = max;

	return p;
}


void AddPresentTextLine(NewList<PresentationInfoNode> *l, Common::String data, uint32 max, NewList<NewNode>*texts, uint16 textNr) {
	PresentationInfoNode* p = AddPresentInfo(l, max, texts, textNr);
	p->_presentHow = PRESENT_AS_TEXT;
	p->_extendedText = data;
}

/* XXX: ONLY FOR NUMBERIC VALUES! */
void AddPresentLine(NewList<PresentationInfoNode> *l, PresentationType presentHow, uint32 data, uint32 max, NewList<NewNode> *texts, uint16 textNr) {
	PresentationInfoNode *p = AddPresentInfo(l, max, texts, textNr);
	p->_presentHow = presentHow;

	switch (presentHow) {
	case PRESENT_AS_TEXT:
		ErrorMsg(Internal_Error, ERROR_MODULE_PRESENT, 0);
		break;
	case PRESENT_AS_NUMBER:
		p->_extendedText = Common::String::format("%u", data);
		p->_presentHow = PRESENT_AS_TEXT;
		break;
	case PRESENT_AS_BAR:
		p->_extendedNr = data;
		break;
	default:
		break;
	}
}

void prSetBarPrefs(_GC *gc, uint16 us_BarWidth, uint16 us_BarHeight, byte uch_FCol, byte uch_BCol, byte uch_TCol) {
	PresentControl.gc = gc;
	PresentControl.us_BarWidth = us_BarWidth;
	PresentControl.us_BarHeight = us_BarHeight;
	PresentControl.uch_FCol = uch_FCol;
	PresentControl.uch_BCol = uch_BCol;
	PresentControl.uch_TCol = uch_TCol;
}

void prDrawTextBar(Common::String puch_Text, uint32 ul_Value, uint32 ul_Max, uint16 us_XPos, uint16 us_YPos) {
	_GC *gc = PresentControl.gc;
	uint16 us_Width = PresentControl.us_BarWidth;
	uint16 us_Height = PresentControl.us_BarHeight;

	if (gc) {
		gfxSetRect(us_XPos, us_Width);
		gc->setPens(PresentControl.uch_BCol, GFX_SAME_PEN, PresentControl.uch_FCol);

		gc->rectFill(us_XPos, us_YPos, us_XPos + us_Width - 1, us_YPos + us_Height - 1);

		us_Width = (uint16)((us_Width * ul_Value) / ul_Max);

		gc->setPens(PresentControl.uch_FCol, GFX_SAME_PEN, GFX_SAME_PEN);
		gc->rectFill(us_XPos, us_YPos, us_XPos + us_Width - 1, us_YPos + us_Height - 1);
		gc->setPens(PresentControl.uch_TCol, GFX_SAME_PEN, GFX_SAME_PEN);

		if (!puch_Text.empty()) {
			gc->setMode(GFX_JAM_1);
			gc->gfxPrint(puch_Text, us_YPos + 2, GFX_PRINT_CENTER);
		}
	}
}

} // End of namespace Clue

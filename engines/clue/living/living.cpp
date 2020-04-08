/*
**  $Filename: living/living.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     13-04-94
**
**  "sprite" functions for "Der Clou!"
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

#include "clue/living/living.h"
#include "clue/clue.h"
#include "clue/living/living_p.h"
#include "clue/living/bob.h"

namespace Clue {

void livInit(uint16 us_VisLScapeX, uint16 us_VisLScapeY,
             uint16 us_VisLScapeWidth, uint16 us_VisLScapeHeight,
             uint16 us_TotalLScapeWidth, uint16 us_TotalLScapeHeight,
             byte uch_FrameCount, uint32 ul_StartArea) {
	sc = (SpriteControl *)TCAllocMem(sizeof(*sc), 0);

	sc->p_Livings = new NewList<NewLiving>;
	sc->p_Template = new NewList<NewAnimTemplate>;

	sc->us_VisLScapeX = us_VisLScapeX;
	sc->us_VisLScapeY = us_VisLScapeY;

	sc->us_VisLScapeWidth = us_VisLScapeWidth;
	sc->us_VisLScapeHeight = us_VisLScapeHeight;

	sc->us_TotalLScapeWidth = us_TotalLScapeWidth;
	sc->us_TotalLScapeHeight = us_TotalLScapeHeight;

	livSetActivAreaId(ul_StartArea);

	sc->uch_FrameCount = uch_FrameCount;

	livLoadTemplates();     /* load all anim phases (frames) */

	livLoadLivings();       /* and the corresponding creatures */

	livSetPlayMode(LIV_PM_NORMAL);
}

void livDone() {
	if (sc) {
		if (sc->p_Livings) {
			for (NewLiving *node = sc->p_Livings->getListHead(); node->_succ; node = (NewLiving *)node->_succ)
				node->livRem();

			sc->p_Livings->removeList();
			sc->p_Livings = nullptr;
		}

		if (sc->p_Template) {
			for (NewAnimTemplate *node = sc->p_Template->getListHead(); node->_succ; node = (NewAnimTemplate *)node->_succ)
				node->livRemTemplate();

			sc->p_Template->removeList();
			sc->p_Template = nullptr;
		}

		TCFreeMem(sc, sizeof(*sc));

		sc = nullptr;
	}
}

void livSetActivAreaId(uint32 areaId) {
	if (sc)
		sc->ul_ActivAreaId = areaId;
}

void livLivesInArea(const char *uch_Name, uint32 areaId) {
	NewLiving *liv = sc->p_Livings->getNode(uch_Name);

	if (liv)
		liv->ul_LivesInAreaId = areaId;
}

void livRefreshAll() {
	livDoAnims(0, 0);
}

void livSetAllInvisible() {
	if (sc) {
		for (NewLiving *liv = sc->p_Livings->getListHead(); liv->_succ; liv = (NewLiving *) liv->_succ)
			liv->livHide();
	}
}

void livSetPlayMode(uint32 playMode) {
	sc->ul_SprPlayMode = playMode;

	if (sc->ul_SprPlayMode & LIV_PM_NORMAL) {
		sc->uch_FirstFrame = 0;
		sc->uch_LastFrame = sc->uch_FrameCount;
		sc->ch_PlayDirection = 1;
	}

	if (sc->ul_SprPlayMode & LIV_PM_REVERSE) {
		sc->uch_FirstFrame = sc->uch_FrameCount - 1;
		sc->uch_LastFrame = (byte) - 1;
		sc->ch_PlayDirection = (byte) - 1;
	}
}

uint32 livWhereIs(const char *uch_Name) {
	NewLiving *liv = sc->p_Livings->getNode(uch_Name);
	uint32 loc = 0;

	if (liv)
		loc = liv->ul_LivesInAreaId;

	return loc;
}

void livSetPos(const char *uch_Name, uint16 XPos, uint16 YPos) {
	NewLiving *liv = sc->p_Livings->getNode(uch_Name);

	if (liv) {
		liv->us_XPos = XPos;
		liv->us_YPos = YPos;
	}
}

void livAnimate(const char *uch_Name, byte uch_Action, int16 s_XSpeed, int16 s_YSpeed) {
	NewLiving *liv = sc->p_Livings->getNode(uch_Name);

	if (liv) {
		liv->uch_Status = LIV_ENABLED;

		liv->uch_OldAction = liv->uch_Action;

		liv->uch_Action = uch_Action;
		liv->s_XSpeed = s_XSpeed;
		liv->s_YSpeed = s_YSpeed;

		if (((byte) liv->ch_CurrFrameNr == sc->uch_LastFrame))
			liv->ch_CurrFrameNr = (char) sc->uch_FirstFrame;
	}
}

void NewLiving::livAnimate(byte action, int16 xSpeed, int16 ySpeed) {
	uch_Status = LIV_ENABLED;

	uch_OldAction = uch_Action;

	uch_Action = action;
	s_XSpeed = xSpeed;
	s_YSpeed = ySpeed;

	if (((byte)ch_CurrFrameNr == sc->uch_LastFrame))
		ch_CurrFrameNr = (char)sc->uch_FirstFrame;
}

void livTurn(const char *puch_Name, byte uch_Status) {
	NewLiving *liv = sc->p_Livings->getNode(puch_Name);

	if (liv)
		liv->uch_Status = uch_Status;   /* enable or disable */
}

void NewLiving::livCorrectViewDirection() {
	Common::String name = p_OriginTemplate->_name;

	if (uch_Action <= ANM_MOVE_LEFT)
		uch_ViewDirection = uch_Action;
	else {
		if (!strcmp(name.c_str(), LIV_TEMPL_BULLE_NAME) && (uch_Action == ANM_WORK_CONTROL))
			uch_ViewDirection = ANM_MOVE_DOWN;
	}
}

void livPrepareAnims() {
}

void livDoAnims(byte uch_Play, byte uch_Move) {
	livPrepareAnims();
	lsDoScroll();

	for (NewLiving *liv = sc->p_Livings->getListHead(); liv->_succ; liv = (NewLiving *)liv->_succ) {
		if (liv->uch_Status == LIV_ENABLED) {
			if (uch_Move) {
				liv->us_XPos += liv->s_XSpeed;
				liv->us_YPos += liv->s_YSpeed;
			}

			/*
			 * die View Direction muß hier gesetzt werden und NICHT
			 * in AnimateLiving, da die Aktion in AnimateLiving nicht
			 * stattfinden muß, die ViewDirection zwischenzeitlich
			 * aber eine falschen Wert annimt, was z.B. im Planing
			 * zu Fehler führt!
			 * muß in jedem Fall geschehen, auch wenn Maxi unsichtbar ist
			 */

			liv->livCorrectViewDirection();

			if (liv->livIsVisible()) {
				liv->livShow();

				/* Action != ANM_STAND -> shitty exception because Marx
				           didn't provide a standing anim */
				if (uch_Play && (liv->uch_Action != ANM_STAND))
					liv->ch_CurrFrameNr += (char) sc->ch_PlayDirection;

				if ((byte)liv->ch_CurrFrameNr == sc->uch_LastFrame) {
					// CHECKME : is it useful to use liv2? Couldn't we use directly liv?
					NewLiving *liv2 = sc->p_Livings->getNode(liv->_name);
					if (liv2)
						liv2->livAnimate(ANM_STAND, 0, 0);
				}
			} else
				liv->livHide();
		}
	}
}

void livSetVisLScape(uint16 us_VisLScapeX, uint16 us_VisLScapeY) {
	sc->us_VisLScapeX = us_VisLScapeX;
	sc->us_VisLScapeY = us_VisLScapeY;
}

uint16 livGetXPos(const char *Name) {
	NewLiving *liv = sc->p_Livings->getNode(Name);

	return liv->us_XPos;
}

uint16 livGetYPos(const char *Name) {
	NewLiving *liv = sc->p_Livings->getNode(Name);

	return liv->us_YPos;
}

byte livGetViewDirection(const char *uch_Name) {
	NewLiving *liv = sc->p_Livings->getNode(uch_Name);

	return liv->uch_ViewDirection;
}

bool livIsPositionInViewDirection(uint16 us_GXPos, uint16 us_GYPos,
                                  uint16 us_XPos, uint16 us_YPos,
                                  byte uch_ViewDirection) {
	bool InDirection = true;

	switch (uch_ViewDirection) {
	case ANM_MOVE_LEFT:
		if (us_XPos > us_GXPos)
			InDirection = false;
		break;
	case ANM_MOVE_RIGHT:
		if (us_XPos < us_GXPos)
			InDirection = false;
		break;
	case ANM_MOVE_DOWN:
		if (us_YPos < us_GYPos)
			InDirection = false;
		break;
	case ANM_MOVE_UP:
		if (us_YPos > us_GYPos)
			InDirection = false;
		break;
	default:
		InDirection = false;
		break;
	}

	return InDirection;
}

bool livCanWalk(const char *puch_Name) {
	NewLiving *liv = sc->p_Livings->getNode(puch_Name);

	if (liv) {
		byte direction;

		switch (liv->uch_Action) {
		case ANM_MOVE_UP:
			direction = LS_SCROLL_UP;
			break;
		case ANM_MOVE_DOWN:
			direction = LS_SCROLL_DOWN;
			break;
		case ANM_MOVE_LEFT:
			direction = LS_SCROLL_LEFT;
			break;
		case ANM_MOVE_RIGHT:
			direction = LS_SCROLL_RIGHT;
			break;
		default:
			return false;
		}

		if (!lsIsCollision(liv->us_XPos + liv->s_XSpeed, liv->us_YPos + liv->s_YSpeed, direction))
			return true;
	}

	return false;
}

void NewLiving::livAdd(Common::String templateName, byte xSize, byte ySize, int16 xSpeed, int16 ySpeed) {
	uch_XSize = xSize;
	uch_YSize = ySize;

	NewAnimTemplate *tlt = p_OriginTemplate = sc->p_Template->getNode(templateName);

	us_LivingNr = BobInit(tlt->us_Width, tlt->us_Height);

	uch_OldAction = 0;
	uch_Action = 0;

	uch_ViewDirection = 0;

	ch_CurrFrameNr = 0;

	s_XSpeed = xSpeed;
	s_YSpeed = ySpeed;

	us_XPos = 0;
	us_YPos = 0;

	ul_LivesInAreaId = sc->ul_ActivAreaId;

	uch_Status = LIV_DISABLED;
}

void NewLiving::livRem() {
	BobDone(us_LivingNr);
}
	
static void livLoadTemplates() {
	char pathname[DSK_PATH_MAX];
	dskBuildPathName(DISK_CHECK_FILE, TEXT_DIRECTORY, LIV_ANIM_TEMPLATE_LIST, pathname);

	NewList<NewNode> *l = new NewList<NewNode>;
	uint16 cnt = l->readList(pathname);
	if (!cnt)
		ErrorMsg(Disk_Defect, ERROR_MODULE_LIVING, 3);

	for (uint16 i = 0; i < cnt; i++) {
		Common::String line = l->getNthNode(i)->_name;

		NewAnimTemplate *tlt = sc->p_Template->createNode(g_clue->_txtMgr->getKey(1, line.c_str()));

		tlt->us_Width = (uint16)g_clue->_txtMgr->getKeyAsUint32(2, line);
		tlt->us_Height = (uint16)g_clue->_txtMgr->getKeyAsUint32(3, line);
		tlt->us_FrameOffsetNr = (uint16)g_clue->_txtMgr->getKeyAsUint32(4, line);
	}

	l->removeList();
}

static void livLoadLivings() {
	char pathname[DSK_PATH_MAX];
	dskBuildPathName(DISK_CHECK_FILE, TEXT_DIRECTORY, LIV_LIVINGS_LIST, pathname);

	NewList<NewNode> *l = new NewList<NewNode>;
	uint16 cnt = l->readList(pathname);
	if (!cnt)
		ErrorMsg(Disk_Defect, ERROR_MODULE_LIVING, 2);

	for (uint16 i = 0; i < cnt; i++) {
		Common::String line = l->getNthNode(i)->_name;
		Common::String name = g_clue->_txtMgr->getKey(1, line.c_str());
		Common::String templateName = g_clue->_txtMgr->getKey(2, line.c_str());

		NewLiving *liv = sc->p_Livings->createNode(name);
		liv->livAdd(templateName,
		       (byte)g_clue->_txtMgr->getKeyAsUint32(3, line),
		       (byte)g_clue->_txtMgr->getKeyAsUint32(4, line),
		       (int16)g_clue->_txtMgr->getKeyAsUint32(5, line),
		       (int16)g_clue->_txtMgr->getKeyAsUint32(6, line));
	}

	l->removeList();
}

void NewLiving::livHide() {
	BobInVis(us_LivingNr);
}

void NewLiving::livShow() {
	NewAnimTemplate *tlt = p_OriginTemplate;
	uint16 action;

	/* shitty exception because Marx didn't provide a standing anim */
	if (uch_Action == ANM_STAND) {
		action = uch_ViewDirection;
		ch_CurrFrameNr = 4;
	}
	else
		action = uch_Action;

	uint16 frameNr = action * sc->uch_FrameCount + ch_CurrFrameNr;
	frameNr += tlt->us_FrameOffsetNr;
	uint16 offset = frameNr * tlt->us_Width;

	uint16 srcY = (offset / LIV_COLL_WIDTH) * tlt->us_Height;
	uint16 srcX = (offset % LIV_COLL_WIDTH);

	if (BobSet(us_LivingNr, us_XPos, us_YPos, srcX, srcY))
		BobVis(us_LivingNr);
}

bool NewLiving::livIsVisible() {
	bool visible = false;

	uint16 left = us_XPos;
	uint16 right = left + uch_XSize;
	uint16 up = us_YPos;
	uint16 down = up + uch_YSize;

	if (ul_LivesInAreaId == sc->ul_ActivAreaId) {
		if (right > sc->us_VisLScapeX) {
			if (left < sc->us_VisLScapeX + sc->us_VisLScapeWidth) {
				if (down > sc->us_VisLScapeY) {
					if (up < sc->us_VisLScapeY + sc->us_VisLScapeHeight)
						visible = true;
				}
			}
		}
	}

	return visible;
}
} // End of namespace Clue

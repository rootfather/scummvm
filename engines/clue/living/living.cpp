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

	sc->_livings = new NewList<NewLiving>;
	sc->_template = new NewList<NewAnimTemplate>;

	sc->_visLScapeX = us_VisLScapeX;
	sc->_visLScapeY = us_VisLScapeY;

	sc->_visLScapeWidth = us_VisLScapeWidth;
	sc->_visLScapeHeight = us_VisLScapeHeight;

	sc->_totalLScapeWidth = us_TotalLScapeWidth;
	sc->_totalLScapeHeight = us_TotalLScapeHeight;

	livSetActivAreaId(ul_StartArea);

	sc->_frameCount = uch_FrameCount;

	livLoadTemplates();     /* load all anim phases (frames) */

	livLoadLivings();       /* and the corresponding creatures */

	livSetPlayMode(LIV_PM_NORMAL);
}

void livDone() {
	if (sc) {
		if (sc->_livings) {
			for (NewLiving *node = sc->_livings->getListHead(); node->_succ; node = (NewLiving *)node->_succ)
				node->livRem();

			sc->_livings->removeList();
			sc->_livings = nullptr;
		}

		if (sc->_template) {
			for (NewAnimTemplate *node = sc->_template->getListHead(); node->_succ; node = (NewAnimTemplate *)node->_succ)
				node->livRemTemplate();

			sc->_template->removeList();
			sc->_template = nullptr;
		}

		TCFreeMem(sc, sizeof(*sc));

		sc = nullptr;
	}
}

void livSetActivAreaId(uint32 areaId) {
	if (sc)
		sc->_activAreaId = areaId;
}

void livLivesInArea(const char *uch_Name, uint32 areaId) {
	NewLiving *liv = sc->_livings->getNode(uch_Name);

	if (liv)
		liv->_livesInAreaId = areaId;
}

void livRefreshAll() {
	livDoAnims(0, false);
}

void livSetAllInvisible() {
	if (sc) {
		for (NewLiving *liv = sc->_livings->getListHead(); liv->_succ; liv = (NewLiving *) liv->_succ)
			liv->livHide();
	}
}

void livSetPlayMode(uint32 playMode) {
	sc->_sprPlayMode = playMode;

	if (sc->_sprPlayMode & LIV_PM_NORMAL) {
		sc->_firstFrame = 0;
		sc->_lastFrame = sc->_frameCount;
		sc->_playDirection = 1;
	}

	if (sc->_sprPlayMode & LIV_PM_REVERSE) {
		sc->_firstFrame = sc->_frameCount - 1;
		sc->_lastFrame = (byte) -1;
		sc->_playDirection = (byte) -1;
	}
}

uint32 livWhereIs(const char *uch_Name) {
	NewLiving *liv = sc->_livings->getNode(uch_Name);
	uint32 loc = 0;

	if (liv)
		loc = liv->_livesInAreaId;

	return loc;
}

void livSetPos(const char *uch_Name, uint16 XPos, uint16 YPos) {
	NewLiving *liv = sc->_livings->getNode(uch_Name);

	if (liv) {
		liv->_xPos = XPos;
		liv->_yPos = YPos;
	}
}

void livAnimate(const char *uch_Name, byte uch_Action, int16 s_XSpeed, int16 s_YSpeed) {
	NewLiving *liv = sc->_livings->getNode(uch_Name);

	if (liv) {
		liv->_status = LIV_ENABLED;

		liv->_oldAction = liv->_action;

		liv->_action = uch_Action;
		liv->_xSpeed = s_XSpeed;
		liv->_ySpeed = s_YSpeed;

		if ((byte) liv->_currFrameNr == sc->_lastFrame)
			liv->_currFrameNr = (char) sc->_firstFrame;
	}
}

void NewLiving::livAnimate(byte action, int16 xSpeed, int16 ySpeed) {
	_status = LIV_ENABLED;

	_oldAction = _action;

	_action = action;
	_xSpeed = xSpeed;
	_ySpeed = ySpeed;

	if (((byte)_currFrameNr == sc->_lastFrame))
		_currFrameNr = (char)sc->_firstFrame;
}

void livTurn(const char *puch_Name, byte uch_Status) {
	NewLiving *liv = sc->_livings->getNode(puch_Name);

	if (liv)
		liv->_status = uch_Status;   /* enable or disable */
}

void NewLiving::livCorrectViewDirection() {
	Common::String name = _originTemplate->_name;

	if (_action <= ANM_MOVE_LEFT)
		_viewDirection = _action;
	else {
		if (!strcmp(name.c_str(), LIV_TEMPL_BULLE_NAME) && (_action == ANM_WORK_CONTROL))
			_viewDirection = ANM_MOVE_DOWN;
	}
}

void livPrepareAnims() {
}

void livDoAnims(byte uch_Play, bool uch_Move) {
	livPrepareAnims();
	lsDoScroll();

	for (NewLiving *liv = sc->_livings->getListHead(); liv->_succ; liv = (NewLiving *)liv->_succ) {
		if (liv->_status == LIV_ENABLED) {
			if (uch_Move) {
				liv->_xPos += liv->_xSpeed;
				liv->_yPos += liv->_ySpeed;
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
				if (uch_Play && liv->_action != ANM_STAND)
					liv->_currFrameNr += sc->_playDirection;

				if ((byte)liv->_currFrameNr == sc->_lastFrame) {
					// CHECKME : is it useful to use liv2? Couldn't we use directly liv?
					NewLiving *liv2 = sc->_livings->getNode(liv->_name);
					if (liv2)
						liv2->livAnimate(ANM_STAND, 0, 0);
				}
			} else
				liv->livHide();
		}
	}
}

void livSetVisLScape(uint16 us_VisLScapeX, uint16 us_VisLScapeY) {
	sc->_visLScapeX = us_VisLScapeX;
	sc->_visLScapeY = us_VisLScapeY;
}

uint16 livGetXPos(const char *Name) {
	NewLiving *liv = sc->_livings->getNode(Name);

	return liv->_xPos;
}

uint16 livGetYPos(const char *Name) {
	NewLiving *liv = sc->_livings->getNode(Name);

	return liv->_yPos;
}

byte livGetViewDirection(const char *uch_Name) {
	NewLiving *liv = sc->_livings->getNode(uch_Name);

	return liv->_viewDirection;
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
	NewLiving *liv = sc->_livings->getNode(puch_Name);

	if (liv) {
		byte direction;

		switch (liv->_action) {
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

		if (!lsIsCollision(liv->_xPos + liv->_xSpeed, liv->_yPos + liv->_ySpeed, direction))
			return true;
	}

	return false;
}

void NewLiving::livAdd(Common::String templateName, byte xSize, byte ySize, int16 xSpeed, int16 ySpeed) {
	_xSize = xSize;
	_ySize = ySize;

	NewAnimTemplate *tlt = _originTemplate = sc->_template->getNode(templateName);

	_livingNr = BobInit(tlt->_width, tlt->_height);

	_oldAction = 0;
	_action = 0;

	_viewDirection = 0;

	_currFrameNr = 0;

	_xSpeed = xSpeed;
	_ySpeed = ySpeed;

	_xPos = 0;
	_yPos = 0;

	_livesInAreaId = sc->_activAreaId;

	_status = LIV_DISABLED;
}

void NewLiving::livRem() {
	BobDone(_livingNr);
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

		NewAnimTemplate *tlt = sc->_template->createNode(g_clue->_txtMgr->getKey(1, line.c_str()));

		tlt->_width = (uint16)g_clue->_txtMgr->getKeyAsUint32(2, line);
		tlt->_height = (uint16)g_clue->_txtMgr->getKeyAsUint32(3, line);
		tlt->_frameOffsetNr = (uint16)g_clue->_txtMgr->getKeyAsUint32(4, line);
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

		NewLiving *liv = sc->_livings->createNode(name);
		liv->livAdd(templateName,
		       (byte)g_clue->_txtMgr->getKeyAsUint32(3, line),
		       (byte)g_clue->_txtMgr->getKeyAsUint32(4, line),
		       (int16)g_clue->_txtMgr->getKeyAsUint32(5, line),
		       (int16)g_clue->_txtMgr->getKeyAsUint32(6, line));
	}

	l->removeList();
}

void NewLiving::livHide() {
	BobInVis(_livingNr);
}

void NewLiving::livShow() {
	NewAnimTemplate *tlt = _originTemplate;
	uint16 action;

	/* shitty exception because Marx didn't provide a standing anim */
	if (_action == ANM_STAND) {
		action = _viewDirection;
		_currFrameNr = 4;
	} else
		action = _action;

	uint16 frameNr = action * sc->_frameCount + _currFrameNr;
	frameNr += tlt->_frameOffsetNr;
	uint16 offset = frameNr * tlt->_width;

	uint16 srcY = (offset / LIV_COLL_WIDTH) * tlt->_height;
	uint16 srcX = (offset % LIV_COLL_WIDTH);

	if (BobSet(_livingNr, _xPos, _yPos, srcX, srcY))
		BobVis(_livingNr);
}

bool NewLiving::livIsVisible() {
	bool visible = false;

	uint16 left = _xPos;
	uint16 right = left + _xSize;
	uint16 up = _yPos;
	uint16 down = up + _ySize;

	if (_livesInAreaId == sc->_activAreaId) {
		if (right > sc->_visLScapeX) {
			if (left < sc->_visLScapeX + sc->_visLScapeWidth) {
				if (down > sc->_visLScapeY) {
					if (up < sc->_visLScapeY + sc->_visLScapeHeight)
						visible = true;
				}
			}
		}
	}

	return visible;
}
} // End of namespace Clue

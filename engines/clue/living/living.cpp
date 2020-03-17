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

namespace Clue {

void livInit(uint16 us_VisLScapeX, uint16 us_VisLScapeY,
             uint16 us_VisLScapeWidth, uint16 us_VisLScapeHeight,
             uint16 us_TotalLScapeWidth, uint16 us_TotalLScapeHeight,
             byte uch_FrameCount, uint32 ul_StartArea) {
	sc = (SpriteControl *)TCAllocMem(sizeof(*sc), 0);

	sc->p_Livings = CreateList();
	sc->p_Template = CreateList();

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
			for (NODE *node = (NODE *) LIST_HEAD(sc->p_Livings); NODE_SUCC(node);
			        node = (NODE *) NODE_SUCC(node))
				livRem((struct Living *) node);

			RemoveList(sc->p_Livings);
			sc->p_Livings = NULL;
		}

		if (sc->p_Template) {
			for (NODE *node = (NODE *) LIST_HEAD(sc->p_Template); NODE_SUCC(node);
			        node = (NODE *) NODE_SUCC(node))
				livRemTemplate((struct AnimTemplate *) node);

			RemoveList(sc->p_Template);
			sc->p_Template = NULL;
		}

		TCFreeMem(sc, sizeof(*sc));

		sc = NULL;
	}
}

void livSetActivAreaId(uint32 areaId) {
	if (sc)
		sc->ul_ActivAreaId = areaId;
}

void livLivesInArea(const char *uch_Name, uint32 areaId) {
	struct Living *liv = livGet(uch_Name);

	if (liv)
		liv->ul_LivesInAreaId = areaId;
}

void livRefreshAll() {
	livDoAnims(0, 0);
}

void livSetAllInvisible() {
	if (sc) {
		for (struct Living *liv = (struct Living *) LIST_HEAD(sc->p_Livings); NODE_SUCC(liv);
		        liv = (struct Living *) NODE_SUCC(liv))
			livHide(liv);
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
	struct Living *liv = livGet(uch_Name);
	uint32 loc = 0;

	if (liv)
		loc = liv->ul_LivesInAreaId;

	return loc;
}

void livSetPos(const char *uch_Name, uint16 XPos, uint16 YPos) {
	struct Living *liv = livGet(uch_Name);

	if (liv) {
		liv->us_XPos = XPos;
		liv->us_YPos = YPos;
	}
}

void livAnimate(const char *uch_Name, byte uch_Action, int16 s_XSpeed, int16 s_YSpeed) {
	struct Living *liv = livGet(uch_Name);

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

void livTurn(const char *puch_Name, byte uch_Status) {
	struct Living *liv = livGet(puch_Name);

	if (liv)
		liv->uch_Status = uch_Status;   /* enable or disable */
}

static void livCorrectViewDirection(struct Living *liv) {
	char *name = NODE_NAME(liv->p_OriginTemplate);

	if (liv->uch_Action <= ANM_MOVE_LEFT)
		liv->uch_ViewDirection = liv->uch_Action;
	else {
		if (!strcmp(name, LIV_TEMPL_BULLE_NAME)
		        && (liv->uch_Action == ANM_WORK_CONTROL))
			liv->uch_ViewDirection = ANM_MOVE_DOWN;
	}
}

void livPrepareAnims() {
}

void livDoAnims(byte uch_Play, byte uch_Move) {
	livPrepareAnims();
	lsDoScroll();

	for (struct Living *liv = (struct Living *) LIST_HEAD(sc->p_Livings);
	        NODE_SUCC(liv); liv = (struct Living *) NODE_SUCC(liv)) {
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

			livCorrectViewDirection(liv);

			if (livIsVisible(liv)) {
				livShow(liv);

				/* Action != ANM_STAND -> shitty exception because Marx
				           didn't provide a standing anim */
				if ((uch_Play) && (liv->uch_Action != ANM_STAND))
					liv->ch_CurrFrameNr += (char) sc->ch_PlayDirection;

				if ((byte) liv->ch_CurrFrameNr == sc->uch_LastFrame)
					livAnimate(NODE_NAME(liv), ANM_STAND, 0, 0);
			} else
				livHide(liv);
		}
	}
}

void livSetVisLScape(uint16 us_VisLScapeX, uint16 us_VisLScapeY) {
	sc->us_VisLScapeX = us_VisLScapeX;
	sc->us_VisLScapeY = us_VisLScapeY;
}

uint16 livGetXPos(const char *Name) {
	struct Living *liv = livGet(Name);

	return (liv->us_XPos);
}

uint16 livGetYPos(const char *Name) {
	struct Living *liv = livGet(Name);

	return (liv->us_YPos);
}

byte livGetViewDirection(const char *uch_Name) {
	struct Living *liv = livGet(uch_Name);

	return (liv->uch_ViewDirection);
}

#if 0
void livStopAll() {
	for (struct Living *liv = (struct Living *) LIST_HEAD(sc->p_Livings); NODE_SUCC(liv); liv = (struct Living *) NODE_SUCC(liv)) {
		if (liv->uch_Status == LIV_ENABLED)
			livAnimate(NODE_NAME(liv), ANM_STAND, 0, 0);
	}
}

byte livGetOldAction(const char *uch_Name) {
	struct Living *liv = livGet(uch_Name);
	return (liv->uch_OldAction);
}
#endif 

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
	struct Living *liv = livGet(puch_Name);

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

static struct Living *livGet(const char *uch_Name) {
	struct Living *liv = (struct Living *) GetNode(sc->p_Livings, uch_Name);

	if (!liv)
		ErrorMsg(Internal_Error, ERROR_MODULE_LIVING, 1);

	return (liv);
}

static void livAdd(const char *uch_Name, char *uch_TemplateName, byte uch_XSize,
                   byte uch_YSize, int16 s_XSpeed, int16 s_YSpeed) {
	struct Living *liv = (struct Living *)
	      CreateNode(sc->p_Livings, sizeof(struct Living), uch_Name);

	liv->uch_XSize = uch_XSize;
	liv->uch_YSize = uch_YSize;

	struct AnimTemplate *tlt = liv->p_OriginTemplate =
	          (struct AnimTemplate *) GetNode(sc->p_Template, uch_TemplateName);

	liv->us_LivingNr = BobInit(tlt->us_Width, tlt->us_Height);

	liv->uch_OldAction = 0;
	liv->uch_Action = 0;

	liv->uch_ViewDirection = 0;

	liv->ch_CurrFrameNr = 0;

	liv->s_XSpeed = s_XSpeed;
	liv->s_YSpeed = s_YSpeed;

	liv->us_XPos = 0;
	liv->us_YPos = 0;

	liv->ul_LivesInAreaId = sc->ul_ActivAreaId;

	liv->uch_Status = LIV_DISABLED;
}

static void livRem(struct Living *liv) {
	BobDone(liv->us_LivingNr);
}

static void livLoadTemplates() {
	LIST *l = CreateList();

	char pathname[DSK_PATH_MAX];
	dskBuildPathName(DISK_CHECK_FILE, TEXT_DIRECTORY, LIV_ANIM_TEMPLATE_LIST, pathname);

	uint16 cnt = ReadList(l, 0L, pathname);
	if (!cnt)
		ErrorMsg(Disk_Defect, ERROR_MODULE_LIVING, 3);

	for (uint16 i = 0; i < cnt; i++) {
		char *line = NODE_NAME(GetNthNode(l, i));

		struct AnimTemplate *tlt = (struct AnimTemplate *) CreateNode(sc->p_Template,
		        sizeof(struct AnimTemplate),
				g_clue->_txtMgr->getKey(1, line));

		tlt->us_Width = (uint16)g_clue->_txtMgr->getKeyAsUint32(2, line);
		tlt->us_Height = (uint16)g_clue->_txtMgr->getKeyAsUint32(3, line);

		tlt->us_FrameOffsetNr = (uint16)g_clue->_txtMgr->getKeyAsUint32(4, line);
	}

	RemoveList(l);
}

static void livRemTemplate(struct AnimTemplate *tlt) {
	/* dummy function */
}

static void livLoadLivings() {
	LIST *l = CreateList();

	char pathname[DSK_PATH_MAX];
	dskBuildPathName(DISK_CHECK_FILE, TEXT_DIRECTORY, LIV_LIVINGS_LIST, pathname);

	uint16 cnt = ReadList(l, 0L, pathname);
	if (!cnt)
		ErrorMsg(Disk_Defect, ERROR_MODULE_LIVING, 2);

	for (uint16 i = 0; i < cnt; i++) {
		char name[TXT_KEY_LENGTH], templateName[TXT_KEY_LENGTH];
		char *line = NODE_NAME(GetNthNode(l, i));

		strcpy(name, g_clue->_txtMgr->getKey(1, line));
		strcpy(templateName, g_clue->_txtMgr->getKey(2, line));

		livAdd(name,
		       templateName,
		       (byte)g_clue->_txtMgr->getKeyAsUint32(3, line),
		       (byte)g_clue->_txtMgr->getKeyAsUint32(4, line),
		       (int16)g_clue->_txtMgr->getKeyAsUint32(5, line),
		       (int16)g_clue->_txtMgr->getKeyAsUint32(6, line));
	}

	RemoveList(l);
}

static void livHide(struct Living *liv) {
	BobInVis(liv->us_LivingNr);
}

static void livShow(struct Living *liv) {
	struct AnimTemplate *tlt = liv->p_OriginTemplate;
	uint16 action;

	/* shitty exception because Marx didn't provide a standing anim */
	if (liv->uch_Action == ANM_STAND) {
		action = liv->uch_ViewDirection;
		liv->ch_CurrFrameNr = 4;
	} else
		action = liv->uch_Action;

	uint16 frameNr = action * sc->uch_FrameCount + liv->ch_CurrFrameNr;
	frameNr += tlt->us_FrameOffsetNr;
	uint16 offset = frameNr * tlt->us_Width;

	uint16 srcY = (offset / LIV_COLL_WIDTH) * tlt->us_Height;
	uint16 srcX = (offset % LIV_COLL_WIDTH);

	if (BobSet(liv->us_LivingNr, liv->us_XPos, liv->us_YPos, srcX, srcY))
		BobVis(liv->us_LivingNr);
}

static bool livIsVisible(struct Living *liv) {
	bool visible = false;

	uint16 left = liv->us_XPos;
	uint16 right = left + liv->uch_XSize;
	uint16 up = liv->us_YPos;
	uint16 down = up + liv->uch_YSize;

	if (liv->ul_LivesInAreaId == sc->ul_ActivAreaId) {
		if (right > sc->us_VisLScapeX) {
			if (left < (sc->us_VisLScapeX + sc->us_VisLScapeWidth)) {
				if (down > sc->us_VisLScapeY) {
					if (up < (sc->us_VisLScapeY + sc->us_VisLScapeHeight))
						visible = true;
				}
			}
		}
	}

	return visible;
}

} // End of namespace Clue

/*
**  $Filename: anim/sysanim.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-02-94
**
**  anim functions for "Der Clou!"
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

#include "clue/inphdl/inphdl.h"
#include "clue/clue.h"

namespace Clue {

#define PIC_MODE_POS         1
#define PIC_P_SEC_POS        2
#define PIC_1_ID_POS         3
#define ANIM_COLL_ID_POS     4
#define PIC_COUNT_POS        5
#define PHASE_WIDTH_POS      6
#define PHASE_HEIGHT_POS     7
#define PHASE_OFFSET_POS     8
#define X_DEST_OFFSET_POS    9
#define Y_DEST_OFFSET_POS   10
#define PLAY_MODE_POS       11

/* Defines for play mode */
#define PM_NORMAL            1
#define PM_PING_PONG         2
#define PM_SYNCHRON          4

/* Defines for AnimPic */
#define Y_OFFSET             0 /* 1 pixel between 2 rows/lines */

struct AnimHandler {
	char *RunningAnimID;       /* currently running animation */

	uint16 destX;
	uint16 destY;
	uint16 width;
	uint16 height;

	uint16 offset;
	uint16 frameCount;

	uint16 pictsPerRow;
	uint16 totalWidth;

	uint16 NrOfAnims;          // Not Used
	uint16 PictureRate;        /* playback rate of the animation */
	uint16 Repeatation;        /* total number of repetitions */

	uint16 RepeatationCount;   /* how many repetitions already passed */

	uint16 AnimCollection;     /* single animation phases */

	uint32 WaitCounter;

	uint16 CurrPictNr;
	int16 Direction;

	uint8 PlayMode;

	uint8 AnimatorState;
};

static struct AnimHandler Handler;
char RunningAnimLine[TXT_KEY_LENGTH];

#define ANIM_FRAME_MEM_RP   AnimRPInMem

#define ANIM_STATE_SUSPENDED    (1<<0)

void LoadAnim(const char *AnimID);

/*
 * init & dones
 */

void InitAnimHandler() {
	Handler.RunningAnimID = RunningAnimLine;
	Handler.RunningAnimID[0] = '\0';
}

void CloseAnimHandler() {
	StopAnim();
}

void SuspendAnim() {
	Handler.AnimatorState |= ANIM_STATE_SUSPENDED;
}

void ContinueAnim() {
	Handler.AnimatorState &= (0xff - ANIM_STATE_SUSPENDED);
}

/*
 * prepare...
 */

static void PrepareAnim(const char *AnimID)
/* initializes various values and afterwards copies anim phases into memory */
{
	Common::String pict_list = GetAnim(AnimID);

	if ((uint32)g_clue->_txtMgr->countKey(pict_list) > PIC_1_ID_POS) {
		CollectionNode* coll = gfxGetCollection(g_clue->_txtMgr->getKeyAsUint32(ANIM_COLL_ID_POS, pict_list));
		Handler.frameCount = g_clue->_txtMgr->getKeyAsUint32(PIC_COUNT_POS, pict_list);

		Handler.width = (uint16)g_clue->_txtMgr->getKeyAsUint32(PHASE_WIDTH_POS, pict_list);
		Handler.height = (uint16)g_clue->_txtMgr->getKeyAsUint32(PHASE_HEIGHT_POS, pict_list);

		Handler.offset = (uint16)g_clue->_txtMgr->getKeyAsUint32(PHASE_OFFSET_POS, pict_list);
		Handler.destX = (uint16)g_clue->_txtMgr->getKeyAsUint32(X_DEST_OFFSET_POS, pict_list);
		Handler.destY = (uint16)g_clue->_txtMgr->getKeyAsUint32(Y_DEST_OFFSET_POS, pict_list);

		/* need to add an offset for total width! Example:
		 * 3 images with Width = 80, Offset = 2 -> TotalWidth = 244
		 * but 244 / 3 is only 2, even though there are 3 images in
		     * this row!
		 */
		Handler.pictsPerRow = (coll->_totalWidth + Handler.offset) / (Handler.width + Handler.offset);
		Handler.totalWidth = coll->_totalWidth;
		Handler.AnimCollection = coll->_collId;

		/* jetzt die Animphasen vorbereiten und ins Mem kopieren */
		gfxCollToMem(coll->_collId, &ANIM_FRAME_MEM_RP);
	}
}

/*
 * PlayAnim
 * StopAnim
 */

void PlayAnim(const char *AnimID, uint16 how_often, uint32 mode) {
	Common::String pict_list = GetAnim(AnimID);

	if (pict_list.empty())
		gfxClearArea(l_gc);
	else {
		StopAnim();

		PrepareAnim(AnimID);

		uint16 pict_id = 0;
		if (!(mode & GFX_DONT_SHOW_FIRST_PIC)) {
			if (!mode)
				mode = g_clue->_txtMgr->getKeyAsUint32((uint16) PIC_MODE_POS, pict_list);

			pict_id = (uint16)g_clue->_txtMgr->getKeyAsUint32((uint16) PIC_1_ID_POS, pict_list);
		}

		if (pict_id)
			gfxShow(pict_id, mode, 2, -1L, -1L);

		if (g_clue->_txtMgr->countKey(pict_list) > PIC_1_ID_POS) {
			uint16 rate = (uint16)g_clue->_txtMgr->getKeyAsUint32((uint16)PIC_P_SEC_POS, pict_list);

			/* ZZZZZZXXXZZZZZ NOTE: UHHHHHHHHHHHHHHH? WTF???
			   LOOK AT 'texts/animd.txt! WTF?'
			   PLAY_MODE_POS is allegedly field 11, with possible values of
			   PlayMode (1=Normal, 2=PingPong, 4=Syncron)
			   but no stinking entry in 'texts/animd.txt' has that field!
			   dismiss as programmer error? maybe this was for amiga version and
			   they 'dropped' it for the PC version? too hard to do pingpong?
			   who knows!

			Handler.PlayMode = (uint8) getKeyAsUint32((uint16)PLAY_MODE_POS, pict_list);
			*/
			Handler.PlayMode = PM_PING_PONG;

			Handler.PictureRate = rate;
			Handler.Repeatation = how_often;

			Handler.CurrPictNr = 0;
			Handler.Direction = 1;
			Handler.RepeatationCount = 0;

			Handler.WaitCounter = 1;

			/* DoAnim is ready to play and our anim is decrunched */
			strcpy(Handler.RunningAnimID, AnimID);

			ContinueAnim(); /* in case anim has been suspended */
		} else
			Handler.RunningAnimID[0] = '\0';
	}
}

void StopAnim() {
	if (Handler.RunningAnimID) {    /* anim currently playing */
		if (Handler.RunningAnimID[0] != '\0') {
			Common::String pict_list = GetAnim(Handler.RunningAnimID);

			/* "unprepare" pictures for the sake of completeness */
			PictureNode *pict = gfxGetPicture((uint16)g_clue->_txtMgr->getKeyAsUint32((uint16) PIC_1_ID_POS, pict_list));

			if (pict)
				gfxUnPrepareColl(pict->_collId);

			if (g_clue->_txtMgr->countKey(pict_list) > PIC_1_ID_POS)
				gfxUnPrepareColl((uint16)g_clue->_txtMgr->getKeyAsUint32((uint16) ANIM_COLL_ID_POS, pict_list));

			Handler.RunningAnimID[0] = '\0';
		}
	}
}

/*
 * access function
 * GetAnim
 */

Common::String GetAnim(const char *AnimID) {
	char id[TXT_KEY_LENGTH];

	strcpy(id, AnimID);

	for (int i = 0; id[i] != '\0'; i++) {
		if (id[i] == ',')
			id[i] = '_';
	}

	return g_clue->_txtMgr->getFirstLine(ANIM_TXT, id);
}

/*
 * Animator
 */

void animator() {
	uint16 destX = Handler.destX;
	uint16 destY = Handler.destY;

	if (!(Handler.AnimatorState & ANIM_STATE_SUSPENDED)) {
		if (Handler.RunningAnimID && Handler.RunningAnimID[0] != '\0') {
			if (Handler.RepeatationCount <= Handler.Repeatation) {
				if ((--Handler.WaitCounter) == 0) {
					Handler.WaitCounter =
					    Handler.PictureRate + g_clue->calcRandomNr(0, 3);

					if (Handler.CurrPictNr == 0) {
						Handler.RepeatationCount++;

						Handler.Direction = 1;
					}

					if (Handler.CurrPictNr == Handler.frameCount - 1) {
						Handler.RepeatationCount++;

						if (Handler.PlayMode & PM_NORMAL)
							Handler.CurrPictNr = 0;
						else
							Handler.Direction = -1;
					}

					if (Handler.RepeatationCount <= Handler.Repeatation) {
						uint16 sourceX = ((Handler.width + Handler.offset) * Handler.CurrPictNr) % (Handler.totalWidth);
						uint16 sourceY = 0;

						if (Handler.pictsPerRow != 0)
							sourceY = (Handler.CurrPictNr / Handler.pictsPerRow) * (Handler.height + Y_OFFSET);

						/* sicherstellen, daß Animframes immer vorhanden sind */

						inpMousePtrOff();

						gfxBlit(l_gc, &ANIM_FRAME_MEM_RP, sourceX, sourceY,
						        destX, destY, Handler.width, Handler.height,
						        false);

						inpMousePtrOn();
					}

					Handler.CurrPictNr = Handler.CurrPictNr + Handler.Direction;
				}
			}
		}
	}
}

} // End of namespace Clue

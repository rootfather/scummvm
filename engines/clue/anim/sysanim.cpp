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

/*
 * init & dones
 */

AnimManager::AnimManager(ClueEngine* vm) : _vm(vm) {
	RunningAnimID = nullptr;

	destX = destY = 0;
	width = height = 0;

	offset = 0;
	frameCount = 0;

	pictsPerRow = 0;
	totalWidth = 0;

	NrOfAnims = 0;
	PictureRate = 0;
	Repeatation = 0;
	
	RepeatationCount = 0;
	AnimCollection = 0;

	WaitCounter = 0;

	CurrPictNr = 0;
	Direction = 0;

	PlayMode = 0;

	AnimatorState = 0;
}

	
void AnimManager::InitAnimHandler() {
	RunningAnimID = RunningAnimLine;
	RunningAnimID[0] = '\0';
}

void AnimManager::CloseAnimHandler() {
	StopAnim();
}

void AnimManager::SuspendAnim() {
	AnimatorState |= ANIM_STATE_SUSPENDED;
}

void AnimManager::ContinueAnim() {
	AnimatorState &= (0xff - ANIM_STATE_SUSPENDED);
}

/*
 * prepare...
 */

void AnimManager::PrepareAnim(const char *AnimID) {
/* initializes various values and afterwards copies anim phases into memory */
	Common::String pict_list = GetAnim(AnimID);

	if ((uint32)_vm->_txtMgr->countKey(pict_list) > PIC_1_ID_POS) {
		CollectionNode* coll = gfxGetCollection(_vm->_txtMgr->getKeyAsUint32(ANIM_COLL_ID_POS, pict_list));
		frameCount = _vm->_txtMgr->getKeyAsUint32(PIC_COUNT_POS, pict_list);

		width = (uint16)_vm->_txtMgr->getKeyAsUint32(PHASE_WIDTH_POS, pict_list);
		height = (uint16)_vm->_txtMgr->getKeyAsUint32(PHASE_HEIGHT_POS, pict_list);

		offset = (uint16)_vm->_txtMgr->getKeyAsUint32(PHASE_OFFSET_POS, pict_list);
		destX = (uint16)_vm->_txtMgr->getKeyAsUint32(X_DEST_OFFSET_POS, pict_list);
		destY = (uint16)_vm->_txtMgr->getKeyAsUint32(Y_DEST_OFFSET_POS, pict_list);

		/* need to add an offset for total width! Example:
		 * 3 images with Width = 80, Offset = 2 -> TotalWidth = 244
		 * but 244 / 3 is only 2, even though there are 3 images in
		 * this row!
		 */
		pictsPerRow = (coll->_totalWidth + offset) / (width + offset);
		totalWidth = coll->_totalWidth;
		AnimCollection = coll->_collId;

		/* jetzt die Animphasen vorbereiten und ins Mem kopieren */
		gfxCollToMem(coll->_collId, &ANIM_FRAME_MEM_RP);
	}
}

/*
 * PlayAnim
 * StopAnim
 */

void AnimManager::PlayAnim(const char *AnimID, uint16 how_often, uint32 mode) {
	Common::String pict_list = GetAnim(AnimID);

	if (pict_list.empty())
		gfxClearArea(l_gc);
	else {
		StopAnim();

		PrepareAnim(AnimID);

		uint16 pict_id = 0;
		if (!(mode & GFX_DONT_SHOW_FIRST_PIC)) {
			if (!mode)
				mode = _vm->_txtMgr->getKeyAsUint32((uint16) PIC_MODE_POS, pict_list);

			pict_id = (uint16)_vm->_txtMgr->getKeyAsUint32((uint16) PIC_1_ID_POS, pict_list);
		}

		if (pict_id)
			gfxShow(pict_id, mode, 2, -1, -1);

		if (_vm->_txtMgr->countKey(pict_list) > PIC_1_ID_POS) {
			uint16 rate = (uint16)_vm->_txtMgr->getKeyAsUint32((uint16)PIC_P_SEC_POS, pict_list);

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
			PlayMode = PM_PING_PONG;

			PictureRate = rate;
			Repeatation = how_often;

			CurrPictNr = 0;
			Direction = 1;
			RepeatationCount = 0;

			WaitCounter = 1;

			/* DoAnim is ready to play and our anim is decrunched */
			strcpy(RunningAnimID, AnimID);

			ContinueAnim(); /* in case anim has been suspended */
		} else
			RunningAnimID[0] = '\0';
	}
}

void AnimManager::StopAnim() {
	if (RunningAnimID) {    /* anim currently playing */
		if (RunningAnimID[0] != '\0') {
			Common::String pict_list = GetAnim(RunningAnimID);

			/* "unprepare" pictures for the sake of completeness */
			PictureNode *pict = gfxGetPicture((uint16)_vm->_txtMgr->getKeyAsUint32((uint16) PIC_1_ID_POS, pict_list));

			if (pict)
				gfxUnPrepareColl(pict->_collId);

			if (_vm->_txtMgr->countKey(pict_list) > PIC_1_ID_POS)
				gfxUnPrepareColl((uint16)_vm->_txtMgr->getKeyAsUint32((uint16) ANIM_COLL_ID_POS, pict_list));

			RunningAnimID[0] = '\0';
		}
	}
}

/*
 * access function
 * GetAnim
 */

Common::String AnimManager::GetAnim(const char *AnimID) {
	char id[TXT_KEY_LENGTH];
	Common::String test = Common::String(AnimID);

	strcpy(id, AnimID);

	for (int i = 0; id[i] != '\0'; i++) {
		if (id[i] == ',')
			id[i] = '_';
	}

	return _vm->_txtMgr->getFirstLine(ANIM_TXT, id);
}

/*
 * Animator
 */

void AnimManager::animator() {
	uint16 destX_ = destX;
	uint16 destY_ = destY;

	if (!(AnimatorState & ANIM_STATE_SUSPENDED)) {
		if (RunningAnimID && RunningAnimID[0] != '\0') {
			if (RepeatationCount <= Repeatation) {
				--WaitCounter;
				if (WaitCounter == 0) {
					WaitCounter = PictureRate + _vm->calcRandomNr(0, 3);

					if (CurrPictNr == 0) {
						RepeatationCount++;
						Direction = 1;
					}

					if (CurrPictNr == frameCount - 1) {
						RepeatationCount++;

						if (PlayMode & PM_NORMAL)
							CurrPictNr = 0;
						else
							Direction = -1;
					}

					if (RepeatationCount <= Repeatation) {
						uint16 sourceX = ((width + offset) * CurrPictNr) % totalWidth;
						uint16 sourceY = 0;

						if (pictsPerRow != 0)
							sourceY = (CurrPictNr / pictsPerRow) * (height + Y_OFFSET);

						/* sicherstellen, daß Animframes immer vorhanden sind */

						inpMousePtrOff();
						gfxBlit(l_gc, &ANIM_FRAME_MEM_RP, sourceX, sourceY, destX_, destY_, width, height, false);
						inpMousePtrOn();
					}

					CurrPictNr += Direction;
				}
			}
		}
	}
}

} // End of namespace Clue

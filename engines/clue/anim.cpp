/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

 /*
 * This code is based on :
 * - original source code of The Clue!
 *	Copyright (c) 1994 ...and avoid panic by, Kaweh Kazemi
 * - Costa's DerClou! SDL port
 *	Copright (c) 2005-2017 Vasco Alexandre da Silva Costa
 */

#include "clue/inphdl/inphdl.h"
#include "clue/clue.h"

namespace Clue {

/*
 * init & dones
 */

AnimManager::AnimManager(ClueEngine* vm) : _vm(vm) {
	_runningAnimID = nullptr;

	_destX = _destY = 0;
	_width = _height = 0;

	_offset = 0;
	_frameCount = 0;

	_pictsPerRow = 0;
	_totalWidth = 0;

	_pictureRate = 0;
	_repetitionMax = 0;
	
	_repetitionCount = 0;
	_animCollection = 0;

	_waitCounter = 0;

	_currPictNr = 0;
	_direction = 0;

	_playMode = 0;

	_state = 0;
}

	
void AnimManager::initAnimHandler() {
	_runningAnimID = _runningAnimLine;
	_runningAnimID[0] = '\0';
}

void AnimManager::closeAnimHandler() {
	stopAnim();
}

void AnimManager::suspendAnim() {
	_state |= ANIM_STATE_SUSPENDED;
}

void AnimManager::continueAnim() {
	_state &= (0xff - ANIM_STATE_SUSPENDED);
}

/*
 * prepare...
 */

void AnimManager::prepareAnim(const char *AnimID) {
/* initializes various values and afterwards copies anim phases into memory */
	Common::String pict_list = getAnim(AnimID);

	if ((uint32)_vm->_txtMgr->countKey(pict_list) > PIC_1_ID_POS) {
		CollectionNode* coll = gfxGetCollection(_vm->_txtMgr->getKeyAsUint32(ANIM_COLL_ID_POS, pict_list));
		_frameCount = _vm->_txtMgr->getKeyAsUint32(PIC_COUNT_POS, pict_list);

		_width = (uint16)_vm->_txtMgr->getKeyAsUint32(PHASE_WIDTH_POS, pict_list);
		_height = (uint16)_vm->_txtMgr->getKeyAsUint32(PHASE_HEIGHT_POS, pict_list);

		_offset = (uint16)_vm->_txtMgr->getKeyAsUint32(PHASE_OFFSET_POS, pict_list);
		_destX = (uint16)_vm->_txtMgr->getKeyAsUint32(X_DEST_OFFSET_POS, pict_list);
		_destY = (uint16)_vm->_txtMgr->getKeyAsUint32(Y_DEST_OFFSET_POS, pict_list);

		/* need to add an offset for total width! Example:
		 * 3 images with Width = 80, Offset = 2 -> TotalWidth = 244
		 * but 244 / 3 is only 2, even though there are 3 images in
		 * this row!
		 */
		_pictsPerRow = (coll->_totalWidth + _offset) / (_width + _offset);
		_totalWidth = coll->_totalWidth;
		_animCollection = coll->_collId;

		/* jetzt die Animphasen vorbereiten und ins Mem kopieren */
		gfxCollToMem(coll->_collId, ANIM_FRAME_MEM_RP);
	}
}

/*
 * PlayAnim
 * StopAnim
 */

void AnimManager::playAnim(const char *AnimID, uint16 how_often, uint32 mode) {
	Common::String pict_list = getAnim(AnimID);

	if (pict_list.empty())
		_lowerGc->gfxClearArea();
	else {
		stopAnim();

		prepareAnim(AnimID);

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
			_playMode = PM_PING_PONG;

			_pictureRate = rate;
			_repetitionMax = how_often;

			_currPictNr = 0;
			_direction = 1;
			_repetitionCount = 0;

			_waitCounter = 1;

			/* DoAnim is ready to play and our anim is decrunched */
			strcpy(_runningAnimID, AnimID);

			continueAnim(); /* in case anim has been suspended */
		} else
			_runningAnimID[0] = '\0';
	}
}

void AnimManager::stopAnim() {
	if (!_runningAnimID || _runningAnimID[0] == '\0')
		return;

	/* anim currently playing */
	Common::String pict_list = getAnim(_runningAnimID);

	/* "unprepare" pictures for the sake of completeness */
	PictureNode *pict = gfxGetPicture((uint16)_vm->_txtMgr->getKeyAsUint32((uint16) PIC_1_ID_POS, pict_list));

	if (pict)
		gfxUnPrepareColl(pict->_collId);

	if (_vm->_txtMgr->countKey(pict_list) > PIC_1_ID_POS)
		gfxUnPrepareColl((uint16)_vm->_txtMgr->getKeyAsUint32((uint16) ANIM_COLL_ID_POS, pict_list));

	_runningAnimID[0] = '\0';
}

/*
 * access function
 * GetAnim
 */

Common::String AnimManager::getAnim(const char *AnimID) {
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
	if (_state & ANIM_STATE_SUSPENDED)
		return;

	if (!_runningAnimID || _runningAnimID[0] == '\0' || _repetitionCount > _repetitionMax)
		return;

	uint16 destX = _destX;
	uint16 destY = _destY;

	--_waitCounter;
	if (_waitCounter == 0) {
		_waitCounter = _pictureRate + _vm->calcRandomNr(0, 3);

		if (_currPictNr == 0) {
			++_repetitionCount;
			_direction = 1;
		}

		if (_currPictNr == _frameCount - 1) {
			++_repetitionCount;

			if (_playMode & PM_NORMAL)
				_currPictNr = 0;
			else
				_direction = -1;
		}

		if (_repetitionCount <= _repetitionMax) {
			uint16 sourceX = ((_width + _offset) * _currPictNr) % _totalWidth;
			uint16 sourceY = 0;

			if (_pictsPerRow != 0)
				sourceY = (_currPictNr / _pictsPerRow) * (_height + Y_OFFSET);

			/* sicherstellen, daß Animframes immer vorhanden sind */

			inpMousePtrOff();
			_lowerGc->gfxBlit(ANIM_FRAME_MEM_RP, sourceX, sourceY, destX, destY, _width, _height, false);
			inpMousePtrOn();
		}

		_currPictNr += _direction;
	}
}

} // End of namespace Clue

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

#ifndef MODULE_ANIM
#define MODULE_ANIM

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

#define Y_OFFSET             0 /* 1 pixel between 2 rows/lines */

#define ANIM_FRAME_MEM_RP   AnimRPInMem
#define ANIM_STATE_SUSPENDED    (1<<0)

class AnimManager {
private:
	ClueEngine* _vm;

	char* _runningAnimID;       /* currently running animation */

	uint16 _destX;
	uint16 _destY;
	uint16 _width;
	uint16 _height;

	uint16 _offset;
	uint16 _frameCount;

	uint16 _pictsPerRow;
	uint16 _totalWidth;

	uint16 _pictureRate;        /* playback rate of the animation */
	uint16 _repetitionMax;      /* total number of repetitions */

	uint16 _repetitionCount;    /* how many repetitions already passed */

	uint16 _animCollection;      /* single animation phases */

	uint32 _waitCounter;

	uint16 _currPictNr;
	int16 _direction;

	uint8 _playMode;

	uint8 _state;
	char _runningAnimLine[TXT_KEY_LENGTH];

	
	void prepareAnim(const char* AnimID);
	void loadAnim(const char* AnimID);

public:
	AnimManager(ClueEngine *vm);

	void initAnimHandler();
	void closeAnimHandler();

	void playAnim(const char* AnimID, uint16 how_often, uint32 mode);    /* -> docs from 16.08.92 ! */
	void stopAnim();

	Common::String getAnim(const char* AnimID);
	void animator();

	void suspendAnim();
	void continueAnim();
};

} // End of namespace Clue

#endif

/*
 * newanim.c
 * (c) 1993 by Helmut Gaberschek & Kaweh Kazemi, ...and avoid panic by
 * All rights reserved.
 *
 * new animation module for the PANIC-System
 *
 * Rev   Date        Comment
 * 1     26.08.93    First implementation
 *
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

/*
 * Structure of the Anim.List file (structure of one line/row)
 *
 *  PictureMode,PictsPerSecc,Pic1,AnimPic,PicCount,Animphase
 *  width, Animphase height, Animphase offset, PlayMode
 *  XDest, YDest (as offset to 1st picture)
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

	char* RunningAnimID;       /* currently running animation */

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

	void PrepareAnim(const char* AnimID);
	char RunningAnimLine[TXT_KEY_LENGTH];
	void LoadAnim(const char* AnimID);

public:
	AnimManager(ClueEngine *vm);

	void InitAnimHandler();
	void CloseAnimHandler();

	void PlayAnim(const char* AnimID, uint16 how_often, uint32 mode);    /* -> docs from 16.08.92 ! */
	void StopAnim();

	Common::String GetAnim(const char* AnimID);
	void animator();

	void SuspendAnim();
	void ContinueAnim();
};

} // End of namespace Clue

#endif

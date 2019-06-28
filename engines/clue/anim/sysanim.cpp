/*
**	$Filename: anim/sysanim.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-02-94
**
**	anim functions for "Der Clou!"
**
**   (c) 1994 ...and avoid panic by, H. Gaberschek
**	    All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "clue/inphdl/inphdl.h"

#include "clue/anim/sysanim.h"

#define	PIC_MODE_POS             1
#define	PIC_P_SEC_POS		 2
#define	PIC_1_ID_POS		 3
#define	ANIM_COLL_ID_POS	 4
#define	PIC_COUNT_POS		 5
#define	PHASE_WIDTH_POS          6
#define	PHASE_HEIGHT_POS	 7
#define	PHASE_OFFSET_POS	 8
#define	X_DEST_OFFSET_POS	 9
#define	Y_DEST_OFFSET_POS	10
#define	PLAY_MODE_POS		11

/* Defines fÅr Playmode */
#define	PM_NORMAL			     1
#define	PM_PING_PONG		     2
#define  PM_SYNCHRON            4

/* Defines fÅr AnimPic Aufbau */
#define 	Y_OFFSET		           0	/* 1 Pixel zwischen 2 Reihen */

struct AnimHandler {
    char *RunningAnimID;	/* Anim, die gerade lÑuft */

    uint16 destX;
    uint16 destY;
    uint16 width;
    uint16 height;

    uint16 offset;
    uint16 frameCount;

    uint16 pictsPerRow;
    uint16 totalWidth;

    uint16 NrOfAnims;
    uint16 PictureRate;		/* Rate mit der Anim gespielt wird */
    uint16 Repeatation;		/* wie oft wiederholen */

    uint16 RepeatationCount;	/* wie oft schon wiederholt */

    uint16 AnimCollection;	/* einzelnen Animphasen */

    uint32 WaitCounter;

    uint16 CurrPictNr;
    int16 Direction;

    uint8 PlayMode;

    uint8 AnimatorState;
};

static struct AnimHandler Handler;
char RunningAnimLine[TXT_KEY_LENGTH];

#define ANIM_FRAME_MEM_RP	AnimRPInMem

#define ANIM_STATE_SUSPENDED	(1<<0)

void LoadAnim(const char *AnimID);

/*
 * init & dones
 */

void InitAnimHandler(void)
{
    Handler.RunningAnimID = RunningAnimLine;
    Handler.RunningAnimID[0] = '\0';
}

void CloseAnimHandler(void)
{
    StopAnim();
}

void SuspendAnim(void)
{
    Handler.AnimatorState |= ANIM_STATE_SUSPENDED;
}

void ContinueAnim(void)
{
    Handler.AnimatorState &= (0xff - ANIM_STATE_SUSPENDED);
}

/*
 * prepare...
 */

static void PrepareAnim(const char *AnimID)
/* initializes various values and afterwards copies anim phases into memory */
{
    char pict_list[TXT_KEY_LENGTH];
    struct Collection *coll;

    GetAnim(AnimID, pict_list);

    if ((uint32) (txtCountKey(pict_list)) > PIC_1_ID_POS) {
	coll =
	    gfxGetCollection(txtGetKeyAsULONG(ANIM_COLL_ID_POS, pict_list));

	Handler.frameCount = txtGetKeyAsULONG(PIC_COUNT_POS, pict_list);

	Handler.width =
	    (uint16) txtGetKeyAsULONG(PHASE_WIDTH_POS, pict_list);
	Handler.height =
	    (uint16) txtGetKeyAsULONG(PHASE_HEIGHT_POS, pict_list);

	Handler.offset =
	    (uint16) txtGetKeyAsULONG(PHASE_OFFSET_POS, pict_list);

	Handler.destX =
	    (uint16) txtGetKeyAsULONG(X_DEST_OFFSET_POS, pict_list);
	Handler.destY =
	    (uint16) txtGetKeyAsULONG(Y_DEST_OFFSET_POS, pict_list);

	/* need to add an offset for total width! Example:
	 * 3 images with Width = 80, Offset = 2 -> TotalWidth = 244
	 * but 244 / 3 is only 2, even though there are 3 images in
         * this row!
	 */
	Handler.pictsPerRow =
	    (coll->us_TotalWidth + Handler.offset) / (Handler.width +
						      Handler.offset);
	Handler.totalWidth = coll->us_TotalWidth;

	Handler.AnimCollection = coll->us_CollId;

	/* jetzt die Animphasen vorbereiten und ins Mem kopieren */
	gfxCollToMem(coll->us_CollId, &ANIM_FRAME_MEM_RP);
    }
}

/*
 * PlayAnim
 * StopAnim
 */

void PlayAnim(const char *AnimID, uint16 how_often, uint32 mode)
{
    char pict_list[TXT_KEY_LENGTH];
    uint16 pict_id = 0, rate;

    GetAnim(AnimID, pict_list);

    if (pict_list[0] == '\0')
	gfxClearArea(l_gc);
    else {
	StopAnim();

	PrepareAnim(AnimID);

	if (!(mode & GFX_DONT_SHOW_FIRST_PIC)) {
	    if (!mode)
		mode = (uint32) txtGetKeyAsULONG((uint16) PIC_MODE_POS, pict_list);

	    pict_id = (uint16) txtGetKeyAsULONG((uint16) PIC_1_ID_POS, pict_list);
	}

	if (pict_id)
	    gfxShow(pict_id, mode, 2, -1L, -1L);

	if ((uint32) (txtCountKey(pict_list)) > PIC_1_ID_POS) {
	    rate = (uint16) txtGetKeyAsULONG((uint16) PIC_P_SEC_POS, pict_list);

/* ZZZZZZXXXZZZZZ NOTE: UHHHHHHHHHHHHHHH? WTF???
   LOOK AT 'texts/animd.txt! WTF?'
   PLAY_MODE_POS is allegedly field 11, with possible values of
	 PlayMode (1=Normal, 2=PingPong, 4=Syncron)
	 but no stinking entry in 'texts/animd.txt' has that field!
	 dismiss as programmer error? maybe this was for amiga version and
	 they 'dropped' it for the PC version? too hard to do pingpong?
	 who knows!

			Handler.PlayMode    = (uint8) txtGetKeyAsULONG((uint16)PLAY_MODE_POS, pict_list);
*/
	    Handler.PlayMode = PM_PING_PONG;

	    Handler.PictureRate = rate;
	    Handler.Repeatation = how_often;

	    Handler.CurrPictNr = 0;
	    Handler.Direction = 1;
	    Handler.RepeatationCount = 0;

	    Handler.WaitCounter = 1;

	    /* DoAnim ist ready to play and our anim is decrunched */
	    strcpy(Handler.RunningAnimID, AnimID);

	    ContinueAnim();	/* in case anim has been suspended */
	} else
	    Handler.RunningAnimID[0] = '\0';
    }
}

void StopAnim(void)
{
    char pict_list[TXT_KEY_LENGTH];
    struct Picture *pict;

    if (Handler.RunningAnimID) {	/* anim currently playing */
	if (Handler.RunningAnimID[0] != '\0') {
	    GetAnim(Handler.RunningAnimID, pict_list);

	    /* "unprepare" pictures for the sake of completeness */
	    pict =
		gfxGetPicture((uint16)
			      txtGetKeyAsULONG((uint16) PIC_1_ID_POS,
					       pict_list));

	    if (pict)
		gfxUnPrepareColl((uint16) pict->us_CollId);

	    if (txtCountKey(pict_list) > PIC_1_ID_POS)
		gfxUnPrepareColl((uint16)
				 txtGetKeyAsULONG((uint16) ANIM_COLL_ID_POS,
						  pict_list));

	    Handler.RunningAnimID[0] = '\0';
	}
    }
}

/*
 * access function
 * GetAnim
 */

void GetAnim(const char *AnimID, char *Dest)
{
    int i;
    char ID[TXT_KEY_LENGTH];

    strcpy(ID, AnimID);

    for (i = 0; ID[i] != '\0'; i++)
	if (ID[i] == ',')
	    ID[i] = '_';

    txtGetNthString(ANIM_TXT, ID, 0, Dest);
}

/*
 * Animator
 */

void animator(void)
{
    uint16 destX = Handler.destX;
    uint16 destY = Handler.destY;

    if (!(Handler.AnimatorState & ANIM_STATE_SUSPENDED)) {
	if (Handler.RunningAnimID && Handler.RunningAnimID[0] != '\0') {
	    if (Handler.RepeatationCount <= Handler.Repeatation) {
		if ((--Handler.WaitCounter) == 0) {
		    Handler.WaitCounter =
			Handler.PictureRate + CalcRandomNr(0, 3);

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
			uint16 sourceX;
			uint16 sourceY;

                        sourceX =
			    ((Handler.width +
			      Handler.offset) * Handler.CurrPictNr) %
			    (Handler.totalWidth);

                        if (Handler.pictsPerRow != 0)
                            sourceY =
			    (Handler.CurrPictNr / Handler.pictsPerRow) *
			    (Handler.height + Y_OFFSET);
                        else
                            sourceY = 0;

			/* sicherstellen, da· Animframes immer vorhanden sind */

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

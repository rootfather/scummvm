/*
**	$Filename: living/living.h
**	$Release:  0
**	$Revision: 0.1
**	$Date:     13-04-94
**
**	"sprite" functions for "Der Clou!"
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

#ifndef MODULE_LIVING
#define MODULE_LIVING

#include "clue/theclou.h"

#ifndef MODULE_LIST
#include "clue/list/list.h"
#endif

#ifndef MODULE_MEMORY
#include "clue/memory/memory.h"
#endif

#ifndef MODULE_LANDSCAP
#include "clue/landscap/landscap.h"
#endif

#ifndef MODULE_DISK
#include "clue/disk/disk.h"
#endif

#ifndef MODULE_TEXT
#include "clue/text/text.h"
#endif

#ifndef MODULE_GFX
#include "clue/gfx/gfx.h"
#endif

#ifndef MODULE_BOB
#include "clue/living/bob.h"
#endif

/* defines fÅr AnimPlayMode */
#define LIV_PM_NORMAL    	1
#define LIV_PM_REVERSE   	2

/* defines fÅr Action */
#define ANM_MOVE_UP 	   	((byte)0)
#define ANM_MOVE_DOWN	   ((byte)1)
#define ANM_MOVE_RIGHT	   ((byte)2)
#define ANM_MOVE_LEFT	   ((byte)3)
#define ANM_STAND          ((byte)9)
#define ANM_WORK_UP        ((byte)4)
#define ANM_WORK_DOWN      ((byte)5)
#define ANM_WORK_LEFT      ((byte)6)
#define ANM_WORK_RIGHT     ((byte)7)
#define ANM_DUSEL          ((byte)8)	/* for burglars */
#define ANM_ELEKTRO        ((byte)8)	/* for burglars */
#define ANM_WORK_CONTROL   ((byte)4)	/* for policemen */
#define ANM_DUSEL_POLICE   ((byte)5)	/* for policemen */

#define ANM_MAKE_CALL		((byte)9)	/* for burglars :MAKECALL */

#define LIV_ENABLED      	1
#define LIV_DISABLED     	0

extern void livInit(uint16 us_VisLScapeX, uint16 us_VisLScapeY,
		    uint16 us_VisLScapeWidth, uint16 us_VisLScapeHeight,
		    uint16 us_TotalLScapeWidth, uint16 us_TotalLScapeHeight,
		    byte uch_FrameCount, uint32 ul_StartArea);

extern void livDone(void);

extern void livSetAllInvisible(void);

extern void livSetPos(char *uch_Name, uint16 XPos, uint16 YPos);
extern void livSetPlayMode(uint32 playMode);
extern void livSetActivAreaId(uint32 areaId);
extern void livRefreshAll(void);

extern void livLivesInArea(char *uch_Name, uint32 areaId);
extern uint32 livWhereIs(char *uch_Name);

extern uint16 livGetXPos(char *uch_Name);
extern uint16 livGetYPos(char *uch_Name);

extern byte livCanWalk(char *puch_Name);
extern byte livGetViewDirection(char *uch_Name);
extern byte livIsPositionInViewDirection(uint16 us_GXPos, uint16 us_GYPos,
					  uint16 us_XPos, uint16 us_YPos,
					  byte uch_ViewDirection);

extern void livTurn(char *puch_Name, byte uch_Status);	/* enabled or disabled */

extern void livSetVisLScape(uint16 us_VisLScapeX, uint16 us_VisLScapeY);

extern void livSetSpeed(char *uch_Name, int16 s_Speed);

extern void livAnimate(char *uch_Name, byte uch_Action, int16 s_XSpeed,
		       int16 s_YSpeed);
extern void livPrepareAnims(void);
extern void livDoAnims(byte uch_Play, byte uch_Move);
extern void livStopAll(void);

extern byte livGetOldAction(char *uch_Name);

/* uint16 spotInit(bitmap, uint16 1 = 48, 2 = 32, 3 = 16 ); */
/* uint16 spotDone */
/* uint16 spotSetPos(uint16, uint16, uint16); */

#endif

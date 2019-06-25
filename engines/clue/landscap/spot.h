/*
**	$Filename: landscap/spot.h
**	$Release:  0
**	$Revision: 0.1
**	$Date:     19-02-94
**
**	spot functions for "Der Clou!"
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

#ifndef MODULE_SPOT
#define MODULE_SPOT

#include "clue/theclou.h"

#ifndef MODULE_LIST
#include "clue/list/list.h"
#endif

#ifndef MODULE_TEXT
#include "clue/text/text.h"
#endif

#ifndef MODULE_GFX
#include "clue/gfx/gfx.h"
#endif

#ifndef MODULE_MEMORY
#include "clue/memory/memory.h"
#endif

#ifndef MODULE_DISK
#include "clue/disk/disk.h"
#endif

#define LS_SPOT_NAME					((byte*)"s")

#define LS_SPOT_SMALL_SIZE         16
#define LS_SPOT_MEDIUM_SIZE        32
#define LS_SPOT_LARGE_SIZE         48

#define LS_SPOT_ON                  1
#define LS_SPOT_OFF                 2

#define LS_ALL_VISIBLE_SPOTS        1
#define LS_ALL_INVISIBLE_SPOTS      2

struct SpotPosition {
    NODE Link;

    uint16 us_XPos;
    uint16 us_YPos;
};

struct Spot {
    NODE Link;

    uint16 us_Size;
    uint16 us_Speed;		/* secconds per move */

    uint32 ul_CtrlObjId;		/* data : objId */

    uint32 ul_AreaId;

    byte uch_Status;

    uint16 us_OldXPos;
    uint16 us_OldYPos;

    uint16 us_PosCount;

    LIST *p_positions;

    struct SpotPosition *p_CurrPos;	/* for fast access */
};

extern void lsInitSpots(void);
extern void lsDoneSpots(void);

extern void lsMoveAllSpots(uint32 time);
extern void lsShowAllSpots(uint32 time, uint32 mode);
extern void lsShowSpot(struct Spot *s, uint32 time);
extern void lsHideSpot(struct Spot *s);

extern void lsBlitSpot(uint16 us_Size, uint16 us_XPos, uint16 us_YPos,
		       byte visible);

extern void lsSetSpotStatus(uint32 CtrlObjId, byte uch_Status);

extern void lsAddSpotPosition(struct Spot *spot, uint16 us_XPos, uint16 us_YPos);
extern void lsLoadSpots(uint32 bldId, char *uch_FileName);
extern void lsWriteSpots(char *uch_FileName);
extern void lsFreeAllSpots(void);
extern void lsRemSpot(struct Spot *spot);

extern LIST *lsGetSpotList(void);

extern struct Spot *lsAddSpot(uint16 us_Size, uint16 us_Speed, uint32 ul_CtrlObjId);
extern struct Spot *lsGetSpot(char *uch_Name);

#endif

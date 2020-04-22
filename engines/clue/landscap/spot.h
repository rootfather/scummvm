/*
**  $Filename: landscap/spot.h
**  $Release:  0
**  $Revision: 0.1
**  $Date:     19-02-94
**
**  spot functions for "Der Clou!"
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

#ifndef MODULE_SPOT
#define MODULE_SPOT

#include "clue/theclou.h"
#include "clue/list/list.h"
#include "clue/text.h"
#include "clue/gfx/gfx.h"
#include "clue/memory/memory.h"
#include "clue/disk/disk.h"

namespace Clue {

#define LS_SPOT_NAME                    ("s")

#define LS_SPOT_SMALL_SIZE         16
#define LS_SPOT_MEDIUM_SIZE        32
#define LS_SPOT_LARGE_SIZE         48

#define LS_SPOT_ON                  1
#define LS_SPOT_OFF                 2

#define LS_ALL_VISIBLE_SPOTS        1
#define LS_ALL_INVISIBLE_SPOTS      2

class SpotPosition : public NewNode {
public:
	uint16 us_XPos;
	uint16 us_YPos;
};

class SpotNode : public NewNode {
public:
	uint16 us_Size;
	uint16 us_Speed;        /* secconds per move */

	uint32 ul_CtrlObjId;        /* data : objId */

	uint32 ul_AreaId;

	byte uch_Status;

	uint16 us_OldXPos;
	uint16 us_OldYPos;

	uint16 us_PosCount;

	NewList<SpotPosition> *p_positions;
	SpotPosition *p_CurrPos; /* for fast access */
};

extern void lsInitSpots();
extern void lsDoneSpots();

extern void lsMoveAllSpots(uint32 time);
extern void lsShowAllSpots(uint32 time, uint32 mode);
extern void lsShowSpot(SpotNode *s, uint32 time);
extern void lsHideSpot(SpotNode *s);

extern void lsBlitSpot(uint16 us_Size, uint16 us_XPos, uint16 us_YPos, byte visible);

extern void lsSetSpotStatus(uint32 CtrlObjId, byte uch_Status);

extern void lsAddSpotPosition(SpotNode *spot, uint16 us_XPos, uint16 us_YPos);
extern void lsLoadSpots(uint32 bldId, char *uch_FileName);
extern void lsWriteSpots(const char *uch_FileName);
extern void lsFreeAllSpots();
extern void lsRemSpot(SpotNode *spot);

extern NewList<SpotNode> *lsGetSpotList();
extern SpotNode *lsAddSpot(uint16 us_Size, uint16 us_Speed, uint32 ul_CtrlObjId);

#if 0
extern Spot *lsGetSpot(const char *uch_Name);
#endif
} // End of namespace Clue

#endif

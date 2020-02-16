/*
**  $Filename: landscap/landscap.h
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-04-94
**
**  landscap functions for "Der Clou!"
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

#ifndef MODULE_LANDSCAP
#define MODULE_LANDSCAP

#include "clue/theclou.h"

#ifndef MODULE_ERROR
#include "clue/error/error.h"
#endif

#ifndef MODULE_LIST
#include "clue/list/list.h"
#endif

#ifndef MODULE_MEMORY
#include "clue/memory/memory.h"
#endif

#ifndef MODULE_DISK
#include "clue/disk/disk.h"
#endif

#ifndef MODULE_INPHDL
#include "clue/inphdl/inphdl.h"
#endif

#ifndef MODULE_GFX
#include "clue/gfx/gfx.h"
#endif

#ifndef MODULE_GFXNCH4
#include "clue/gfx/gfxnch4.h"
#endif

#ifndef MODULE_TEXT
#include "clue/text/text.h"
#endif

#ifndef MODULE_RELATION
#include "clue/data/relation.h"
#endif

#ifndef MODULE_DATABASE
#include "clue/data/database.h"
#endif

#include "clue/data/objstd/tcdata.h"

#ifndef MODULE_LIVING
#include "clue/living/living.h"
#endif

#ifndef MODULE_SPOT
#include "clue/landscap/spot.h"
#endif

namespace Clue {

#define   REL_CONSIST_OFFSET       3
#define   REL_HAS_LOCK_OFFSET      4
#define   REL_HAS_ALARM_OFFSET     5
#define   REL_HAS_POWER_OFFSET     6
#define   REL_HAS_LOOT_OFFSET      7
#define   REL_HAS_ROOM_OFFSET     10

#define   LS_OBJECT_VISIBLE     ((byte) 1)
#define   LS_OBJECT_INVISIBLE   ((byte) 0)


#define   LS_SCROLL_CHECK         0
#define   LS_SCROLL_PREPARE       1

/* Scroll defines */
#define   LS_SCROLL_LEFT                1
#define   LS_SCROLL_RIGHT               2
#define   LS_SCROLL_UP              4
#define   LS_SCROLL_DOWN                8

#define   LS_NO_COLLISION           0
#define   LS_COLLISION              1


#define   LS_MAX_AREA_WIDTH        640
#define   LS_MAX_AREA_HEIGHT       256  /* so that wall fits, also in the last line */

#define   LS_FLOOR_X_SIZE           32
#define   LS_FLOOR_Y_SIZE           32

#define   LS_STD_SCROLL_SPEED        2
#define   LS_STD_MAX_PERSONS         7

#define   LS_PERSON_X_SIZE          14
#define   LS_PERSON_Y_SIZE          14

#define   LS_PERSON_ACCESS_FOCUS_X   7
#define   LS_PERSON_ACCESS_FOCUS_Y   7

#define   LS_DARKNESS              ((byte)255)
#define   LS_BRIGHTNESS            ((byte)0)
#define   LS_DARK_FUNNY            ((byte)85)

#define LS_PC_CORRECT_X             (-9)    /* when sign changes landscap.c */
#define  LS_PC_CORRECT_Y                (19)

/* Coll Mode */
#define   LS_COLL_PLAN                 1
#define   LS_COLL_NORMAL               2
#define   LS_LEVEL_DESIGNER            4

#define   lsGetOldState(lso)           ((((lso)->ul_Status) >> 16) & 0xffff)
#define   lsGetNewState(lso)           (((lso)->ul_Status) & 0xffff)
#define   lsSetOldState(lso)           ((lso)->ul_Status = (((lso)->ul_Status << 16) & 0xffff0000) + (((lso)->ul_Status) & 0xffff))

/* globale functions ! */
extern void lsInitLandScape(uint32 bID, byte mode);
extern void lsDoneLandScape(void);

extern void lsSetScrollSpeed(byte pixel);
extern void lsSetActivLiving(const char *Name, uint16 x, uint16 y);

extern void lsRefreshObjectList(uint32 areaID);

extern uint32 lsAddLootBag(uint16 x, uint16 y, byte bagNr); /* bagNr : 1 - 8! */
extern void lsRemLootBag(uint32 bagId);
extern void lsRefreshAllLootBags(void);

extern void lsShowEscapeCar(void);

extern void lsFastRefresh(LSObject lso);

extern void lsSetObjectState(uint32 objID, byte bitNr, byte value);

extern void lsSetViewPort(uint16 x, uint16 y);

extern void lsSetDarkness(byte value);
extern void lsSetCollMode(byte collMode);

extern LIST *lsGetRoomsOfArea(uint32 ul_AreaId);

extern void lsSetObjectRetrievalList(uint32 ul_AreaId);
extern uint32 lsGetCurrObjectRetrieval(void);

extern uint32 lsGetObjectState(uint32 objID);
extern uint16 lsGetObjectCount(void);
extern byte lsGetLoudness(uint16 x, uint16 y);

extern void lsDoScroll(void);
extern void lsScrollCorrectData(int32 dx, int32 dy);
extern byte lsScrollLandScape(byte direction);
extern byte lsInitScrollLandScape(byte direction, byte mode);

extern LIST *lsGetObjectsByList(uint16 x, uint16 y, uint16 width, uint16 height,
                                byte showInvisible, byte addLootBags);

void lsInitActivArea(uint32 areaID, uint16 x, uint16 y, char *livingName);
extern void lsDoneActivArea(uint32 newAreaID);
extern void lsTurnObject(LSObject lso, byte status, byte Collis);

extern void lsCalcExactSize(LSObject lso, uint16 *x0, uint16 *y0, uint16 *x1,
                            uint16 *y1);

extern void lsInitDoorRefresh(uint32 ObjId);
extern void lsDoDoorRefresh(LSObject lso);

extern uint16 lsGetFloorIndex(uint16 x, uint16 y);

extern void lsGuyInsideSpot(uint16 *us_XPos, uint16 *us_YPos, uint32 *areaId);

extern bool lsIsObjectAWall(LSObject lso);
extern bool lsIsObjectAnAddOn(LSObject lso);
extern bool lsIsObjectADoor(LSObject lso);
extern bool lsIsObjectAStdObj(LSObject lso);
extern bool lsIsObjectSpecial(LSObject lso);

extern bool lsIsLSObjectInActivArea(LSObject lso);

extern byte lsIsCollision(int32 x, int32 y, byte direction);    /* sprite needs it */

extern uint32 lsGetCurrBuildingID(void);
extern uint32 lsGetActivAreaID(void);

extern void lsInitObjectDB(uint32 bld, uint32 areaID);
extern void lsLoadGlobalData(uint32 bld, uint32 ul_AreaId);
extern void lsDoneObjectDB(uint32 areaID);
extern void lsInitRelations(uint32 areaID);
extern void lsSetRelations(uint32 areaID);

extern void lsBlitFloor(uint16 floorIndex, uint16 destx, uint16 desty);

extern void lsWalkThroughWindow(LSObject lso, uint16 us_LivXPos,
                                uint16 us_LivYPos, uint16 *us_XPos,
                                uint16 *us_YPos);

/* internal functions ! */
extern void lsPatchObjects(void);

extern void lsSetVisibleWindow(uint16 x, uint16 y);

extern byte lsIsInside(LSObject lso, uint16 x, uint16 y, uint16 x1, uint16 y1);

extern uint32 lsGetStartArea(void);

extern void lsInitObjects(void);

extern void lsBuildScrollWindow(void);
extern void lsShowAlarmAndPower(LSObject lso, uint32 x, uint32 y);

extern bool lsShowOneObject(LSObject lso, int16 destx, int16 desty,
                             uint32 ObjTypeFilter);

void  lsSafeRectFill(uint16 x0, uint16 y0, uint16 x1, uint16 y1, uint8 color);

/* external functions */
extern void lsInitGfx(void);
extern void lsCloseGfx(void);

extern uint32 ConsistOfRelationID;
extern uint32 hasLockRelationID;
extern uint32 hasAlarmRelationID;
extern uint32 hasPowerRelationID;
extern uint32 hasLootRelationID;
extern uint32 hasRoomRelationID;

#if 0
extern uint16 lsGetTotalXPos(void);
extern uint16 lsGetTotalYPos(void);
extern uint16 lsGetWindowXPos(void);
extern uint16 lsGetWindowYPos(void);
#endif
} // End of namespace Clue

#endif

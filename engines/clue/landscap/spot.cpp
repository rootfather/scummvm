/*
**	$Filename: landscap/spot.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     19-04-94
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

#include "clue/base/base.h"

#include "clue/landscap/spot.h"

#define LS_SPOT_BITMAP_WIDTH 		(96)
#define LS_SPOT_BITMAP_HEIGHT		(LS_SPOT_LARGE_SIZE)

#define LS_SPOT_LOAD_BUFFER		StdBuffer0
#define LS_SPOT_DECR_BUFFER		StdBuffer1

#define LS_SPOT_FILENAME			("Spot")

void lsLoadSpotBitMap(MemRastPort *rp);

struct SpotControl {
    LIST *p_spots;

    MemRastPort RP;
};

static struct SpotControl *sc = NULL;

void lsInitSpots(void)
{
    if (sc)
	lsDoneSpots();

    sc = (SpotControl *)TCAllocMem(sizeof(*sc), 0);

    sc->p_spots = CreateList();

    gfxInitMemRastPort(&sc->RP, LS_SPOT_BITMAP_WIDTH, LS_SPOT_BITMAP_HEIGHT);

    lsLoadSpotBitMap(&sc->RP);
}

void lsDoneSpots(void)
{
    if (sc) {
	lsFreeAllSpots();

	RemoveList(sc->p_spots);

        gfxDoneMemRastPort(&sc->RP);

	TCFreeMem(sc, sizeof(*sc));
    }

    sc = NULL;
}

static int32 lsIsSpotVisible(struct Spot *spot)
{
    return 1;
}

void lsMoveAllSpots(uint32 time)
	/*
	 * zeigt alle Spots, die sich bewegen
	 */
{
    struct Spot *spot;

    for (spot = (struct Spot *) LIST_HEAD(sc->p_spots); NODE_SUCC(spot);
	 spot = (struct Spot *) NODE_SUCC(spot))
	if (spot->us_PosCount > 1)
	    if (lsIsSpotVisible(spot))
		if (lsIsLSObjectInActivArea((LSObject)dbGetObject(spot->ul_CtrlObjId)))	/* wenn der Steuerkasten in dieser Area -> */
		    if (spot->uch_Status & LS_SPOT_ON)
			lsShowSpot(spot, time);	/* Spot darstellen (auch in der aktiven Area */
}

void lsShowAllSpots(uint32 time, uint32 mode)
{
    struct Spot *spot;

    for (spot = (struct Spot *) LIST_HEAD(sc->p_spots); NODE_SUCC(spot);
	 spot = (struct Spot *) NODE_SUCC(spot)) {
	if (lsIsLSObjectInActivArea((LSObject)dbGetObject(spot->ul_CtrlObjId))) {	/* wenn der Steuerkasten in dieser Area -> */
	    if (mode & LS_ALL_VISIBLE_SPOTS)
		if (spot->uch_Status & LS_SPOT_ON)
		    lsShowSpot(spot, time);	/* Spot darstellen */

	    if (mode & LS_ALL_INVISIBLE_SPOTS)
		if (spot->uch_Status & LS_SPOT_OFF)
		    lsHideSpot(spot);	/* Spot (alte Position) l”schen */
	}
    }
}

void lsShowSpot(struct Spot *s, uint32 time)
{				/* zum Abspielen! */
    struct SpotPosition *pos;

    if (!(time % s->us_Speed)) {	/* nur alle x Sekunden Bewegung */
	uint32 count = (time / s->us_Speed);

	/* wegen Ping-Pong dauert ein Zyklus doppelt so lang -> * 2 */
	/* abzglich 2 (letztes und erstes kommen nur einmal        */

	if (s->us_PosCount > 1) {
	    count = count % (s->us_PosCount * 2 - 2);

	    if (count >= s->us_PosCount)	/* Sonderfall Rckw„rts! (in Ping Pong) */
		count = (s->us_PosCount * 2 - 2) - count;
	} else
	    count = 0;

	s->p_CurrPos = pos =
	    (struct SpotPosition *) GetNthNode(s->p_positions, count);

	/* alte Position l”schen */
	lsHideSpot(s);

	/* Spot setzen */
	lsBlitSpot(s->us_Size, pos->us_XPos, pos->us_YPos, 1);

	s->us_OldXPos = pos->us_XPos;
	s->us_OldYPos = pos->us_YPos;
    }
}

void lsHideSpot(struct Spot *s)
{
    /* den alten Spot l”schen!  */
    if ((s->us_OldXPos != (uint16) - 1) && (s->us_OldYPos != (uint16) - 1))
	lsBlitSpot(s->us_Size, s->us_OldXPos, s->us_OldYPos, 0);
}

static void lsGetAreaForSpot(struct Spot *spot)
{
    uint32 area = (spot->ul_CtrlObjId / 1000) * 1000;

    switch (area) {
    case 170000:
	spot->ul_AreaId = 508016;
	break;
    case 320000:
	spot->ul_AreaId = 508031;
	break;
    case 330000:
	spot->ul_AreaId = 508032;
	break;
    case 340000:
	spot->ul_AreaId = 508033;
	break;
    case 360000:
	spot->ul_AreaId = 508035;
	break;
    case 380000:
	spot->ul_AreaId = 508037;
	break;
    case 400000:
	spot->ul_AreaId = 508039;
	break;
    case 420000:
	spot->ul_AreaId = 508041;
	break;
    case 430000:
	spot->ul_AreaId = 508042;
	break;
    case 450000:
	spot->ul_AreaId = 508043;
	break;
    case 440000:
	spot->ul_AreaId = 508044;
	break;
    case 460000:
	spot->ul_AreaId = 508045;
	break;
    case 470000:
	spot->ul_AreaId = 508046;
	break;
    default:
	spot->ul_AreaId = 0;
	break;
    }

    if (setup.Profidisk) {
        switch (area) {
        case 520000:
	    spot->ul_AreaId = 508101;
	    break;
        case 530000:
	    spot->ul_AreaId = 508102;
	    break;
        case 540000:
	    spot->ul_AreaId = 508103;
	    break;
        case 560000:
	    spot->ul_AreaId = 508105;
	    break;
        case 580000:
	    spot->ul_AreaId = 508107;
	    break;
        case 600000:
	    spot->ul_AreaId = 508109;
	    break;
        case 610000:
	    spot->ul_AreaId = 508110;
	    break;
        case 620000:
	    spot->ul_AreaId = 508111;
	    break;
        case 640000:
	    spot->ul_AreaId = 508113;
	    break;
        case 650000:
	    spot->ul_AreaId = 508114;
	    break;
        case 660000:
	    spot->ul_AreaId = 508115;
	    break;
        case 670000:
	    spot->ul_AreaId = 508116;
	    break;
        default:
	    spot->ul_AreaId = 0;
	    break;
        }
    }
}

struct Spot *lsAddSpot(uint16 us_Size, uint16 us_Speed, uint32 ul_CtrlObjId)
{
    struct Spot *spot;
    uint32 SpotNr;
    char line[TXT_KEY_LENGTH];

    SpotNr = GetNrOfNodes(sc->p_spots);

    sprintf(line, "*%s%u", LS_SPOT_NAME, SpotNr);

    spot =
	(struct Spot *) CreateNode(sc->p_spots, sizeof(*spot), line);

    spot->us_Size = us_Size;
    spot->us_Speed = us_Speed;
    spot->p_positions = CreateList();

    spot->us_OldXPos = (uint16) - 1;
    spot->us_OldYPos = (uint16) - 1;

    spot->uch_Status = LS_SPOT_ON;
    spot->us_PosCount = 0;

    spot->p_CurrPos = NULL;

    spot->ul_CtrlObjId = ul_CtrlObjId;

    lsGetAreaForSpot(spot);

    return (spot);
}

void lsSetSpotStatus(uint32 CtrlObjId, byte uch_Status)
{
    struct Spot *s;

    for (s = (struct Spot *) LIST_HEAD(sc->p_spots); NODE_SUCC(s);
	 s = (struct Spot *) NODE_SUCC(s))
	if (s->ul_CtrlObjId == CtrlObjId)
	    s->uch_Status = uch_Status;
}

void lsAddSpotPosition(struct Spot *spot, uint16 us_XPos, uint16 us_YPos)
{
    struct SpotPosition *pos;

    pos =
	(struct SpotPosition *) CreateNode(spot->p_positions, sizeof(*pos),
					   NULL);

    pos->us_XPos = (int16) us_XPos + (int16) LS_PC_CORRECT_X;
    pos->us_YPos = (int16) us_YPos + (int16) LS_PC_CORRECT_Y;

    spot->us_PosCount++;
}

void lsLoadSpotBitMap(MemRastPort *rp)
{
    char path[DSK_PATH_MAX];
    uint16 i, j;
    uint8 *sp, *dp;

    /* create file name */
    dskBuildPathName(DISK_CHECK_FILE, PICTURE_DIRECTORY, LS_SPOT_FILENAME, path);

    /* der Decr Buffer entspricht dem ScratchRP */
    gfxLoadILBM(path);

    /* now copy to the right buffer */

    dp = rp->pixels;
    sp = ScratchRP.pixels;

    for (j = 0; j < rp->h; j++) {
	for (i = 0; i < rp->w; i++) {
	    if (*sp == 63)
		*sp = 64;

	    if (*sp == 255)
		*sp = 0;

	    *dp++ = *sp++;
	}

        sp += SCREEN_WIDTH - rp->w;	/* modulo */
    }
}

void lsLoadSpots(uint32 bldId, char *uch_FileName)
{
    FILE *file;
    char filename[DSK_PATH_MAX], buffer[TXT_KEY_LENGTH];
    uint16 SpotCount, i, j;
    uint32 CtrlObjId;
    struct Spot *spot;

    dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, uch_FileName, filename);

    file = dskOpen(filename, "r");

    fgets(buffer, TXT_KEY_LENGTH - 1, file);
    SpotCount = (uint16) atol(buffer);

    for (i = 0; i < SpotCount; i++) {
	uint16 Size, Speed, Count, XPos, YPos;

	fgets(buffer, TXT_KEY_LENGTH - 1, file);
	Size = (uint16) atol(buffer);

	fgets(buffer, TXT_KEY_LENGTH - 1, file);
	Speed = (uint16) atol(buffer);

	fgets(buffer, TXT_KEY_LENGTH - 1, file);
	CtrlObjId = atol(buffer);

	fgets(buffer, TXT_KEY_LENGTH - 1, file);
	Count = (uint16) atol(buffer);

	spot = lsAddSpot(Size, Speed, CtrlObjId);

	for (j = 0; j < Count; j++) {
	    fgets(buffer, TXT_KEY_LENGTH - 1, file);
	    XPos = (uint16) atol(buffer);

	    fgets(buffer, TXT_KEY_LENGTH - 1, file);
	    YPos = (uint16) atol(buffer);

	    lsAddSpotPosition(spot, XPos, YPos);
	}

	if (!(LIST_EMPTY(sc->p_spots)))
	    spot->p_CurrPos = (struct SpotPosition *) LIST_HEAD(sc->p_spots);
    }

    dskClose(file);
}

void lsFreeAllSpots(void)
{
    struct Spot *spot;

    for (spot = (struct Spot *) LIST_HEAD(sc->p_spots); NODE_SUCC(spot);
	 spot = (struct Spot *) NODE_SUCC(spot))
	RemoveList(spot->p_positions);
}

struct Spot *lsGetSpot(const char *uch_Name)
{
    return ((struct Spot *) GetNode(sc->p_spots, uch_Name));
}

LIST *lsGetSpotList(void)
{
    return (sc->p_spots);
}

void lsBlitSpot(uint16 us_Size, uint16 us_XPos, uint16 us_YPos, byte visible)
{
    uint16 sourceX = 0;

    switch (us_Size) {
    case LS_SPOT_SMALL_SIZE:
	sourceX = LS_SPOT_LARGE_SIZE + LS_SPOT_MEDIUM_SIZE;
	break;
    case LS_SPOT_MEDIUM_SIZE:
	sourceX = LS_SPOT_LARGE_SIZE;
	break;
    case LS_SPOT_LARGE_SIZE:
	break;
    }

    if (visible)
        gfxLSPutSet(&sc->RP, sourceX, 0, us_XPos, us_YPos, us_Size, us_Size);
    else
        gfxLSPutClr(&sc->RP, sourceX, 0, us_XPos, us_YPos, us_Size, us_Size);
}

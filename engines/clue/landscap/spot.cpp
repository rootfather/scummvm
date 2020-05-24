/*
**  $Filename: landscap/spot.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     19-04-94
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

#include "clue/base/base.h"
#include "clue/clue.h"
#include "clue/planing/player.h"

namespace Clue {

#define LS_SPOT_BITMAP_WIDTH        (96)
#define LS_SPOT_BITMAP_HEIGHT       (LS_SPOT_LARGE_SIZE)

#define LS_SPOT_LOAD_BUFFER     StdBuffer0
#define LS_SPOT_DECR_BUFFER     StdBuffer1

#define LS_SPOT_FILENAME            ("Spot")

void lsLoadSpotBitMap(MemRastPort *rp);

class SpotControl {
public:
	NewList<SpotNode> *p_spots;
	MemRastPort *RP;
};

static SpotControl *sc = nullptr;

void lsInitSpots() {
	if (sc)
		lsDoneSpots();

	sc = new SpotControl;
	sc->p_spots = new NewList<SpotNode>;
	sc->RP = new MemRastPort;
	sc->RP->gfxInitMemRastPort(LS_SPOT_BITMAP_WIDTH, LS_SPOT_BITMAP_HEIGHT);

	lsLoadSpotBitMap(sc->RP);
}

void lsDoneSpots() {
	if (sc) {
		lsFreeAllSpots();
		sc->p_spots->removeList();
		sc->RP->gfxDoneMemRastPort();
		delete sc->RP;
		delete sc;
		sc = nullptr;
	}
}

static int32 lsIsSpotVisible(SpotNode *spot) {
	return 1;
}

void lsMoveAllSpots(uint32 time) {
/*
 * zeigt alle Spots, die sich bewegen
 */
	for (SpotNode *spot = sc->p_spots->getListHead(); spot->_succ; spot = (SpotNode *) spot->_succ) {
		if (spot->us_PosCount > 1) {
			if (lsIsSpotVisible(spot)) {
				if (lsIsLSObjectInActivArea((LSObjectNode *)dbGetObject(spot->ul_CtrlObjId))) { /* wenn der Steuerkasten in dieser Area -> */
					if (spot->uch_Status & LS_SPOT_ON)
						lsShowSpot(spot, time); /* Spot darstellen (auch in der aktiven Area */
				}
			}
		}
	}
}

void lsShowAllSpots(uint32 time, uint32 mode) {
	for (SpotNode *spot = sc->p_spots->getListHead(); spot->_succ; spot = (SpotNode *) spot->_succ) {
		if (lsIsLSObjectInActivArea((LSObjectNode *)dbGetObject(spot->ul_CtrlObjId))) {   /* wenn der Steuerkasten in dieser Area -> */
			if (mode & LS_ALL_VISIBLE_SPOTS) {
				if (spot->uch_Status & LS_SPOT_ON)
					lsShowSpot(spot, time); /* Spot darstellen */
			}

			if (mode & LS_ALL_INVISIBLE_SPOTS) {
				if (spot->uch_Status & LS_SPOT_OFF)
					lsHideSpot(spot);   /* Spot (alte Position) löschen */
			}
		}
	}
}

void lsShowSpot(SpotNode *s, uint32 time) {
	/* zum Abspielen! */
	if (!(time % s->us_Speed)) {    /* nur alle x Sekunden Bewegung */
		uint32 count = (time / s->us_Speed);

		/* wegen Ping-Pong dauert ein Zyklus doppelt so lang -> * 2 */
		/* abzüglich 2 (letztes und erstes kommen nur einmal        */

		if (s->us_PosCount > 1) {
			count = count % (s->us_PosCount * 2 - 2);

			if (count >= s->us_PosCount)    /* Sonderfall Rückwärts! (in Ping Pong) */
				count = (s->us_PosCount * 2 - 2) - count;
		} else
			count = 0;

		SpotPosition *pos = s->p_CurrPos = s->p_positions->getNthNode(count);

		/* alte Position löschen */
		lsHideSpot(s);

		/* Spot setzen */
		lsBlitSpot(s->us_Size, pos->us_XPos, pos->us_YPos, true);

		s->us_OldXPos = pos->us_XPos;
		s->us_OldYPos = pos->us_YPos;
	}
}

void lsHideSpot(SpotNode *s) {
	/* den alten Spot löschen!  */
	if ((s->us_OldXPos != (uint16) -1) && (s->us_OldYPos != (uint16) -1))
		lsBlitSpot(s->us_Size, s->us_OldXPos, s->us_OldYPos, false);
}

static void lsGetAreaForSpot(SpotNode *spot) {
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

	if (g_clue->getFeatures() & GF_PROFIDISK) {
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

SpotNode *lsAddSpot(uint16 us_Size, uint16 us_Speed, uint32 ul_CtrlObjId) {
	uint32 SpotNr = sc->p_spots->getNrOfNodes();
	Common::String line = Common::String::format("*%s%u", LS_SPOT_NAME, SpotNr);

	SpotNode *spot = sc->p_spots->createNode(line);
	spot->us_Size = us_Size;
	spot->us_Speed = us_Speed;
	spot->p_positions = new NewList<SpotPosition>;
	spot->us_OldXPos = (uint16) -1;
	spot->us_OldYPos = (uint16) -1;
	spot->uch_Status = LS_SPOT_ON;
	spot->us_PosCount = 0;
	spot->p_CurrPos = nullptr;
	spot->ul_CtrlObjId = ul_CtrlObjId;

	lsGetAreaForSpot(spot);

	return spot;
}

void lsSetSpotStatus(uint32 CtrlObjId, byte uch_Status) {
	for (SpotNode *s = sc->p_spots->getListHead(); s->_succ; s = (SpotNode *) s->_succ) {
		if (s->ul_CtrlObjId == CtrlObjId)
			s->uch_Status = uch_Status;
	}
}

void lsAddSpotPosition(SpotNode *spot, uint16 us_XPos, uint16 us_YPos) {
	SpotPosition *pos = spot->p_positions->createNode(nullptr);

	pos->us_XPos = (int16) us_XPos + (int16) LS_PC_CORRECT_X;
	pos->us_YPos = (int16) us_YPos + (int16) LS_PC_CORRECT_Y;

	spot->us_PosCount++;
}

void lsLoadSpotBitMap(MemRastPort *rp) {
	char path[DSK_PATH_MAX];

	/* create file name */
	dskBuildPathName(DISK_CHECK_FILE, PICTURE_DIRECTORY, LS_SPOT_FILENAME, path);

	/* der Decr Buffer entspricht dem ScratchRP */
	gfxLoadILBM(path);

	/* now copy to the right buffer */
	uint8* dp = rp->pixels;
	uint8* sp = ScratchRP->pixels;

	for (uint16 j = 0; j < rp->h; j++) {
		for (uint16 i = 0; i < rp->w; i++) {
			if (*sp == 63)
				*sp = 64;

			if (*sp == 255)
				*sp = 0;

			*dp++ = *sp++;
		}

		sp += SCREEN_WIDTH - rp->w; /* modulo */
	}
}

void lsLoadSpots(uint32 bldId, char *uch_FileName) {
	char filename[DSK_PATH_MAX];
	dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, uch_FileName, filename);
	Common::Stream *file = dskOpen(filename, 0);

	uint16 SpotCount;
	dskGetLine_U16(file, &SpotCount);

	for (uint16 i = 0; i < SpotCount; i++) {
		uint16 Size;
		dskGetLine_U16(file, &Size);

		uint16 Speed;
		dskGetLine_U16(file, &Speed);

		uint32 CtrlObjId;
		dskGetLine_U32(file, &CtrlObjId);

		uint16 Count;
		dskGetLine_U16(file, &Count);

		SpotNode *spot = lsAddSpot(Size, Speed, CtrlObjId);

		for (uint16 j = 0; j < Count; j++) {
			uint16 XPos;
			dskGetLine_U16(file, &XPos);

			uint16 YPos;
			dskGetLine_U16(file, &YPos);

			lsAddSpotPosition(spot, XPos, YPos);
		}

		if (!sc->p_spots->isEmpty())
			// CHECKME: The original was casting the spot list head to a position
			spot->p_CurrPos = sc->p_spots->getListHead()->p_positions->getListHead();
	}

	dskClose(file);
}

void lsFreeAllSpots() {
	for (SpotNode *spot = sc->p_spots->getListHead(); spot->_succ; spot = (SpotNode *) spot->_succ)
		spot->p_positions->removeList();
}

NewList<SpotNode> *lsGetSpotList() {
	return sc->p_spots;
}

void lsBlitSpot(uint16 us_Size, uint16 us_XPos, uint16 us_YPos, bool visible) {
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
		gfxLSPutSet(sc->RP, sourceX, 0, us_XPos, us_YPos, us_Size, us_Size);
	else
		gfxLSPutClr(sc->RP, sourceX, 0, us_XPos, us_YPos, us_Size, us_Size);
}

#if 0
struct Spot *lsGetSpot(const char *uch_Name) {
	return ((Spot *) GetNode(sc->p_spots, uch_Name));
}
#endif
} // End of namespace Clue

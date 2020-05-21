/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "clue/living/living.h"

namespace Clue {

#define BOB_USED        1
#define BOB_VISIBLE     2
#define BOB_UPDATED     4

struct Bob {
	uint16 w, h;
	uint16 xsrc, ysrc;
	uint16 xdst, ydst;

	uint16 sx, sy;

	int flags;
};

#define BOB_MAX     256

struct Bob list[BOB_MAX];

struct Bob bob_zero;

void gfxNCH4Refresh();

void BobInitLists() {
	static bool init;

	if (!init) {
		gfxCollToMem(137, &BobRPInMem);
		init = true;
	}
}

static struct Bob *GetNthBob(uint16 BobID) {
	if (BobID < BOB_MAX)
		return &list[BobID];

	return NULL;
}

uint16 BobInit(uint16 width, uint16 height) {
	uint16 BobID;
	for (BobID = 0; BobID < BOB_MAX && list[BobID].flags != 0; BobID++);

	if (BobID < BOB_MAX) {
		struct Bob *bob = &list[BobID];

		*bob = bob_zero;
		bob->w = width;
		bob->h = height;
		bob->flags |= BOB_USED;
	}
	return BobID;
}

void BobDone(uint16 BobID) {
	struct Bob *bob = GetNthBob(BobID);

	bob->flags = 0;
}

byte BobSet(uint16 BobID, uint16 xdst, uint16 ydst, uint16 xsrc, uint16 ysrc) {
	struct Bob *bob = GetNthBob(BobID);

	bob->xsrc = xsrc;
	bob->ysrc = ysrc;

	bob->xdst = xdst;
	bob->ydst = ydst;
	return 1;
}

extern int ScrX, ScrY;

void BobVis(uint16 BobID) {
	struct Bob *bob = GetNthBob(BobID);

	bob->flags |= BOB_VISIBLE;

	gfxNCH4Refresh();
}

void BobInVis(uint16 BobID) {
	struct Bob *bob = GetNthBob(BobID);

	bob->flags &= ~BOB_VISIBLE;

	gfxNCH4Refresh();
}

void BobSetDarkness(byte darkness) {
}

void BobDisplayLists(_GC *gc) {
	gfxPrepareColl(137);

	for (uint16 i = 0; i < BOB_MAX; i++) {
		struct Bob *bob = &list[i];

		if (bob->flags & BOB_VISIBLE)
			gc->gfxBlit(&BobRPInMem, bob->xsrc, bob->ysrc, bob->xdst - ScrX, bob->ydst - ScrY, bob->w, bob->h, true);
	}
}

} // End of namespace Clue

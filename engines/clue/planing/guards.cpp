/*
**      $Filename: planing/guards.c
**      $Release:  1
**      $Revision: 0
**      $Date:     24-04-94
**
**      planing.guards for "Der Clou!"
**
** (c) 1994 ...and avoid panic by, Kaweh Kazemi
**      All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "clue/planing/guards.h"

namespace Clue {

void grdDo(Common::Stream *fh, struct System *sys, LIST *PersonsList, uint32 BurglarsNr,
           uint32 PersonsNr, byte grdAction) {
	for (uint32 i = BurglarsNr; i < PersonsNr; i++) {
		switch (grdAction) {
		case GUARDS_DO_SAVE:
			SaveHandler(fh, sys, OL_NR(GetNthNode(PersonsList, i)));
			break;

		case GUARDS_DO_LOAD:
			LoadHandler(fh, sys, OL_NR(GetNthNode(PersonsList, i)));
			break;
		}
	}
}

bool grdInit(Common::Stream **fh, int RW, uint32 bldId, uint32 areaId) {
	char fileName[DSK_PATH_MAX];
	dbGetObjectName(areaId, fileName);
	fileName[strlen(fileName) - 1] = '\0';
	char bldName[TXT_KEY_LENGTH];
	sprintf(bldName, "%s%s", fileName, GUARD_EXTENSION);

	dskBuildPathName(DISK_CHECK_FILE, GUARD_DIRECTORY, bldName, fileName);

	if ((*fh = dskOpen(fileName, RW)))
		return true;

	return false;
}

void grdDone(Common::Stream *fh) {
	dskClose(fh);
}

bool grdAddToList(uint32 bldId, LIST *l) {
	isGuardedbyAll(bldId, OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Police);

	if (!LIST_EMPTY(ObjectList)) {
		for (struct ObjectNode *n = (struct ObjectNode *) LIST_HEAD(ObjectList); NODE_SUCC(n);
		        n = (struct ObjectNode *) NODE_SUCC(n))
			dbAddObjectNode(l, OL_NR(n), OLF_INCLUDE_NAME | OLF_INSERT_STAR);

		return true;
	}

	return false;
}

bool grdDraw(GC *gc, uint32 bldId, uint32 areaId) {
	bool ret = false;
	LIST *GuardsList = CreateList();
	if (grdAddToList(bldId, GuardsList)) {
		Common::Stream *fh;
		if (grdInit(&fh, 0, bldId, areaId)) {
			uint16 xpos = 0, ypos = 0;

			uint32 GuardsNr = GetNrOfNodes(GuardsList);
			struct System *grdSys = InitSystem();

			for (uint32 i = 0; i < GuardsNr; i++) {
				InitHandler(grdSys, OL_NR(GetNthNode(GuardsList, i)),
				            SHF_AUTOREVERS);
			}

			grdDo(fh, grdSys, GuardsList, 0, GuardsNr, GUARDS_DO_LOAD);
			grdDone(fh);

			dbSortObjectList(&GuardsList, dbStdCompareObjects);

			for (uint32 i = 0; i < GuardsNr; i++) {
				if (areaId == isGuardedbyGet(bldId, OL_NR(GetNthNode(GuardsList, i)))) {
					struct Handler *h = FindHandler(grdSys, OL_NR(GetNthNode(GuardsList, i)));

					/* getting start coordinates */
					switch (i) {
					case 0:
					case 2:
						xpos = (((LSArea) dbGetObject(areaId))->us_StartX4) / 2;
						ypos = (((LSArea) dbGetObject(areaId))->us_StartY4) / 2;
						break;

					case 1:
					case 3:
						xpos = (((LSArea) dbGetObject(areaId))->us_StartX5) / 2;
						ypos = (((LSArea) dbGetObject(areaId))->us_StartY5) / 2;
						break;
					}

					xpos += 4;

					gfxMoveCursor(gc, xpos, ypos);

					/* drawing system */
					for (struct Action *action = (struct Action *) LIST_HEAD(h->Actions);
					        NODE_SUCC(action);
					        action = (struct Action *) NODE_SUCC(action)) {
						switch (action->Type) {
						case ACTION_GO:
							switch ((ActionData(action, struct ActionGo *))->
							        Direction) {
							case DIRECTION_LEFT:
								xpos -= action->TimeNeeded;
								break;

							case DIRECTION_RIGHT:
								xpos += action->TimeNeeded;
								break;

							case DIRECTION_UP:
								ypos -= action->TimeNeeded;
								break;

							case DIRECTION_DOWN:
								ypos += action->TimeNeeded;
								break;
							}

							gfxDraw(gc, xpos, ypos);
							break;

						case ACTION_WAIT:
						case ACTION_OPEN:
						case ACTION_CLOSE:
						case ACTION_CONTROL:
							/*DrawCircle(rp, xpos, ypos, 1);*/
							break;
						}
					}
				}
			}

			ret = true;

			for (uint32 i = 0; i < GuardsNr; i++)
				CloseHandler(grdSys, OL_NR(GetNthNode(GuardsList, i)));

			CloseSystem(grdSys);
		}
	}

	RemoveList(GuardsList);

	return ret;
}

} // End of namespace Clue

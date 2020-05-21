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

void grdDo(Common::Stream *fh, System *sys, NewObjectList<dbObjectNode> *personsList, uint32 burglarsNr, uint32 personsNr, byte grdAction) {
	for (uint32 i = burglarsNr; i < personsNr; i++) {
		switch (grdAction) {
		case GUARDS_DO_SAVE:
			SaveHandler(fh, sys, personsList->getNthNode(i)->_nr);
			break;

		case GUARDS_DO_LOAD:
			LoadHandler(fh, sys, personsList->getNthNode(i)->_nr);
			break;
		}
	}
}

bool grdInit(Common::Stream **fh, int RW, uint32 bldId, uint32 areaId) {
	char grdPath[DSK_PATH_MAX];

	Common::String fileName = dbGetObjectName(areaId);
	fileName.deleteLastChar();
	Common::String bldName = fileName + GUARD_EXTENSION;
	dskBuildPathName(DISK_CHECK_FILE, GUARD_DIRECTORY, bldName.c_str(), grdPath);

	if ((*fh = dskOpen(grdPath, RW)))
		return true;

	return false;
}

void grdDone(Common::Stream *fh) {
	dskClose(fh);
}

bool grdAddToList(uint32 bldId, NewObjectList<dbObjectNode> *l) {
	isGuardedbyAll(bldId, OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Police);

	if (!ObjectList->isEmpty()) {
		for (dbObjectNode *n = ObjectList->getListHead(); n->_succ; n = (dbObjectNode *) n->_succ)
			dbAddObjectNode(l, n->_nr, OLF_INCLUDE_NAME | OLF_INSERT_STAR);

		return true;
	}

	return false;
}

bool grdDraw(_GC *gc, uint32 bldId, uint32 areaId) {
	bool ret = false;
	NewObjectList<dbObjectNode> *GuardsList = new NewObjectList<dbObjectNode>;
	if (grdAddToList(bldId, GuardsList)) {
		Common::Stream *fh;
		if (grdInit(&fh, 0, bldId, areaId)) {
			uint16 xpos = 0, ypos = 0;

			uint32 GuardsNr = GuardsList->getNrOfNodes();
			System *grdSys = InitSystem();

			for (uint32 i = 0; i < GuardsNr; i++) {
				InitHandler(grdSys, GuardsList->getNthNode(i)->_nr, SHF_AUTOREVERS);
			}

			grdDo(fh, grdSys, GuardsList, 0, GuardsNr, GUARDS_DO_LOAD);
			grdDone(fh);

			dbSortObjectList(&GuardsList, dbStdCompareObjects);

			for (uint32 i = 0; i < GuardsNr; i++) {
				if (areaId == isGuardedbyGet(bldId, GuardsList->getNthNode(i)->_nr)) {
					HandlerNode *h = FindHandler(grdSys, GuardsList->getNthNode(i)->_nr);

					/* getting start coordinates */
					switch (i) {
					case 0:
					case 2:
						xpos = (((LSAreaNode *) dbGetObject(areaId))->us_StartX4) / 2;
						ypos = (((LSAreaNode *) dbGetObject(areaId))->us_StartY4) / 2;
						break;

					case 1:
					case 3:
						xpos = (((LSAreaNode *) dbGetObject(areaId))->us_StartX5) / 2;
						ypos = (((LSAreaNode *) dbGetObject(areaId))->us_StartY5) / 2;
						break;
					}

					xpos += 4;

					gc->moveCursor(xpos, ypos);

					/* drawing system */
					for (ActionNode *action = h->Actions->getListHead(); action->_succ; action = (ActionNode *) action->_succ) {
						switch (action->Type) {
						case ACTION_GO:
							switch (((ActionGoNode *)action)-> Direction) {
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
							default:
								break;
							}

							gc->draw(xpos, ypos);
							break;

						case ACTION_WAIT:
						case ACTION_OPEN:
						case ACTION_CLOSE:
						case ACTION_CONTROL:
							/*DrawCircle(rp, xpos, ypos, 1);*/
							break;
						default:
							break;
						}
					}
				}
			}

			ret = true;

			for (uint32 i = 0; i < GuardsNr; i++)
				CloseHandler(grdSys, GuardsList->getNthNode(i)->_nr);

			CloseSystem(grdSys);
		}
	}

	GuardsList->removeList();
	return ret;
}

} // End of namespace Clue

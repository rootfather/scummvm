/*
**      $Filename: planing/io.c
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.io for "Der Clou!"
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

#include "clue/base/base.h"
#include "clue/data/database_p.h"
#include "clue/present/interac.h"
#include "clue/planing/planer.h"
#include "clue/planing/player.h"


namespace Clue {
/* Loading & saving functions */
void plSaveTools(Common::Stream *fh) {
	if (fh) {
		hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Tool);

		dskSetLine(fh, PLANING_PLAN_TOOL_BEGIN_ID);

		for (dbObjectNode *n = ObjectList->getListHead(); n->_succ; n = (dbObjectNode *) n->_succ)
			dskSetLine_U32(fh, n->_nr);

		dskSetLine(fh, PLANING_PLAN_TOOL_END_ID);
	}
}

NewList<NewNode> *plLoadTools(Common::Stream *fh) {
	NewList<NewNode> *l = g_clue->_txtMgr->goKey(PLAN_TXT, "SYSTEM_TOOLS_MISSING_1");
	bool foundAll = true;
	byte canGet = 2, toolsNr = 0;

	char buffer[64];
	buffer[0] = '\0';

	if (fh) {
		while (dskGetLine(buffer, sizeof(buffer), fh) && strcmp(buffer, PLANING_PLAN_TOOL_END_ID) != 0) {
			uint32 id;
			if (sscanf(buffer, "%u\r\n", &id) == 1) {
				toolsNr++;

				if (!has(Person_Matt_Stuvysunt, id)) {
					if (has(Person_Mary_Bolton, id)) {
						canGet++;
						// CHECKME: The original code was mixing node types.
						// The new node doesn't, but needs extra testing.
						// dbAddObjectNode(l, id, OLF_INCLUDE_NAME);
						warning("CHECKME - Code modified in LoadSystem");
						dbObjectNode *obj = dbGetObject(id);
						l->createNode(obj->_name);
					}

					foundAll = false;
				}
			}
		}
	}

	if (foundAll) {
		l->removeList();
		l = nullptr;
	} else {
		NewList<NewNode> *extList = nullptr;

		if (canGet == 2)
			extList = g_clue->_txtMgr->goKey(PLAN_TXT, "SYSTEM_TOOLS_MISSING_3");
		else if ((toolsNr - canGet) > 1)
			extList = g_clue->_txtMgr->goKeyAndInsert(PLAN_TXT, "SYSTEM_TOOLS_MISSING_2",
				                      (uint32)(toolsNr - canGet));
		else if (toolsNr - canGet)
			extList = g_clue->_txtMgr->goKey(PLAN_TXT, "SYSTEM_TOOLS_MISSING_4");

		if (extList) {
			for (NewNode *n = extList->getListHead(); n->_succ; n = n->_succ)
				l->createNode(n->_name);

			extList->removeList();
		}
	}

	return l;
}

byte plOpen(uint32 objId, byte mode, Common::Stream **fh) {
	static const int Planing_Open[3] = { 0, 1, 0 };

	if (_gamePlayMode & GP_MODE_GUARD_DESIGN) {
		if (grdInit(fh, Planing_Open[mode], objId, lsGetActivAreaID()))
			return PLANING_OPEN_OK;
	} else {
		Common::String name1 = dbGetObjectName(lsGetActivAreaID());
		name1.deleteLastChar();
		Common::String name2 = name1 + PLANING_PLAN_LIST_EXTENSION;

		char pllPath[DSK_PATH_MAX];
		dskBuildPathName(DISK_CHECK_FILE, DATADISK, name2.c_str(), pllPath);

		Common::Stream *pllFh = dskOpen(pllPath, 0);
		if (pllFh) {
			uint32 pllData = 0;
			dskGetLine_U32(pllFh, &pllData);
			dskClose(pllFh);

			if ((mode == PLANING_OPEN_WRITE_PLAN) || pllData) {
				name2 = Common::String::format("MODE_%d", mode);
				plMessage(name2, PLANING_MSG_REFRESH);

				NewObjectList<IODataNode> *PlanList = new NewObjectList<IODataNode>;

				name1 = dbGetObjectName(objId);

				Common::String exp;

				for (uint32 i = 0; i < PLANING_NR_PLANS; i++) {
					if ((mode == PLANING_OPEN_WRITE_PLAN) || (pllData & (1 << i))) {
						if (mode == PLANING_OPEN_WRITE_PLAN) {
							if (pllData & (1 << i))
								exp = g_clue->_txtMgr->getFirstLine(PLAN_TXT, "ATTENTION_1");
							else
								exp = g_clue->_txtMgr->getFirstLine(PLAN_TXT, "ATTENTION_2");
						}

						name2 = Common::String::format("*%s Plan %d    %s", name1.c_str(), i + 1, exp.c_str());

						IODataNode* data = PlanList->createNode(name2);
						data->_ioData = i;
					}
				}

				name2 = Common::String::format("EXPAND_MODE_%d", mode);
				exp = g_clue->_txtMgr->getFirstLine(PLAN_TXT, name2.c_str());
				// FIXME : weird call. Replaced by createNode to have something visible for a test
				// PlanList->expandObjectList(exp);
				warning("plOpen - Workaround used - to be fixed.");
				PlanList->createNode(exp);
				// 

				int i = Bubble((NewList<NewNode>*)PlanList, 0, nullptr, 0);

				if (ChoiceOkHack(i, GET_OUT, (NewList<NewNode>*)PlanList)) {
					IODataNode *data = PlanList->getNthNode(i);

					if (data)
						i = data->_ioData;

					name1 = dbGetObjectName(lsGetActivAreaID());
					name1.deleteLastChar();
					name2 = Common::String::format("%s%d%s", name1.c_str(), i + 1, PLANING_PLAN_EXTENSION);

					char pllPath2[DSK_PATH_MAX];
					dskBuildPathName(DISK_CHECK_FILE, DATADISK, name2.c_str(), pllPath2);

					*fh = dskOpen(pllPath2, Planing_Open[mode]);

					if (mode == PLANING_OPEN_WRITE_PLAN) {
						pllData |= 1 << i;

						if ((pllFh = dskOpen(pllPath, 1))) {
							dskSetLine_U32(pllFh, pllData);
							dskClose(pllFh);
						}
					}

					if (*fh) {
						PlanList->removeList();
						return PLANING_OPEN_OK;
					} else
						ErrorMsg(Disk_Defect, ERROR_MODULE_PLANING, 0);
				}

				PlanList->removeList();
				return PLANING_OPEN_ERR_NO_CHOICE;
			}

			return PLANING_OPEN_ERR_NO_PLAN;
		}
	}

	ErrorMsg(Disk_Defect, ERROR_MODULE_PLANING, 0);
	return PLANING_OPEN_OK;
}

void plSave(uint32 objId) {
	Common::Stream *fh = NULL;

	if (plOpen(objId, PLANING_OPEN_WRITE_PLAN, &fh) == PLANING_OPEN_OK) {
		if (_gamePlayMode & GP_MODE_GUARD_DESIGN) {
			grdDo(fh, plSys, PersonsList, BurglarsNr, PersonsNr, GUARDS_DO_SAVE);
		} else {
			SaveSystem(fh, plSys);
			plSaveTools(fh);

			for (int i = 0; i < BurglarsNr; i++)
				SaveHandler(fh, plSys, PersonsList->getNthNode(i)->_nr);
		}

		dskClose(fh);
	}
}

void plSaveChanged(uint32 objId) {
	if (PlanChanged) {
		NewList<NewNode> *l = g_clue->_txtMgr->goKey(PLAN_TXT, "PLAN_CHANGED");

		inpTurnESC(false);

		if (Bubble(l, 0, nullptr, 0) == 0) {
			inpTurnESC(true);

			if (!plAllInCar(objId))
				plSay("PLAN_NOT_FINISHED", 0);

			plSave(objId);
		} else
			inpTurnESC(true);

		l->removeList();
	}
}

void plLoad(uint32 objId) {
	Common::Stream *fh = NULL;
	byte ret;

	if (objId == Building_Starford_Kaserne)
		while ((ret = plOpen(objId, PLANING_OPEN_READ_PLAN, &fh)) != PLANING_OPEN_OK);
	else
		ret = plOpen(objId, PLANING_OPEN_READ_PLAN, &fh);

	if (ret == PLANING_OPEN_OK) {
		if (_gamePlayMode & GP_MODE_GUARD_DESIGN)
			grdDo(fh, plSys, PersonsList, BurglarsNr, PersonsNr,
			      GUARDS_DO_LOAD);
		else {
			NewList<NewNode> *l = nullptr;
			bool goon = true;

			if ((l = LoadSystem(fh, plSys))) {
				inpTurnESC(false);
				Bubble(l, 0, nullptr, 0);
				inpTurnESC(true);
				l->removeList();

				goon = false;
				l = nullptr;
			}

			if ((l = plLoadTools(fh))) {
				inpTurnESC(false);
				Bubble(l, 0, nullptr, 0);
				inpTurnESC(true);
				l->removeList();

				goon = false;
			}

			if (goon) {
				for (byte i = 0; i < BurglarsNr; i++)
					LoadHandler(fh, plSys, PersonsList->getNthNode(i)->_nr);
			}
		}

		dskClose(fh);
	} else if (ret == PLANING_OPEN_ERR_NO_PLAN)
		plMessage("NO_PLAN", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
}

} // End of namespace Clue

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

namespace Clue {

struct IOData {
	NODE io_Link;
	byte io_Data;
};


static int Planing_Open[3] = { 0, 1, 0 };


/* Loading & saving functions */
void plSaveTools(Common::Stream *fh) {
	if (fh) {
		hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Tool);

		dskSetLine(fh, PLANING_PLAN_TOOL_BEGIN_ID);

		for (struct ObjectNode *n = (struct ObjectNode *) LIST_HEAD(ObjectList); NODE_SUCC(n);
		        n = (struct ObjectNode *) NODE_SUCC(n))
			dskSetLine_U32(fh, OL_NR(n));

		dskSetLine(fh, PLANING_PLAN_TOOL_END_ID);
	}
}

LIST *plLoadTools(Common::Stream *fh) {
	LIST *l = g_clue->_txtMgr->goKey(PLAN_TXT, "SYSTEM_TOOLS_MISSING_1");
	byte foundAll = 1, canGet = 2, toolsNr = 0;

	char buffer[64];
	buffer[0] = '\0';

	if (fh) {
		while (dskGetLine(buffer, sizeof(buffer), fh)
		        && strcmp(buffer, PLANING_PLAN_TOOL_END_ID) != 0) {
			uint32 id;
			if (sscanf(buffer, "%u\r\n", &id) == 1) {
				toolsNr++;

				if (!has(Person_Matt_Stuvysunt, id)) {
					if (has(Person_Mary_Bolton, id)) {
						canGet++;
						dbAddObjectNode(l, id, OLF_INCLUDE_NAME);
					}

					foundAll = 0;
				}
			}
		}
	}

	if (foundAll) {
		RemoveList(l);
		l = NULL;
	} else {
		LIST *extList = NULL;

		if (canGet == 2)
			extList = g_clue->_txtMgr->goKey(PLAN_TXT, "SYSTEM_TOOLS_MISSING_3");
		else if ((toolsNr - canGet) > 1)
			extList = g_clue->_txtMgr->goKeyAndInsert(PLAN_TXT, "SYSTEM_TOOLS_MISSING_2",
				                      (uint32)(toolsNr - canGet));
		else if (toolsNr - canGet)
			extList = g_clue->_txtMgr->goKey(PLAN_TXT, "SYSTEM_TOOLS_MISSING_4");

		if (extList) {
			for (NODE *n = LIST_HEAD(extList); NODE_SUCC(n); n = NODE_SUCC(n))
				CreateNode(l, 0, NODE_NAME(n));

			RemoveList(extList);
		}
	}

	return l;
}

byte plOpen(uint32 objId, byte mode, Common::Stream **fh) {
	if (GamePlayMode & GP_GUARD_DESIGN) {
		if (grdInit(fh, Planing_Open[mode], objId, lsGetActivAreaID()))
			return PLANING_OPEN_OK;
	} else {
		char name1[TXT_KEY_LENGTH];
		dbGetObjectName(lsGetActivAreaID(), name1);
		name1[strlen(name1) - 1] = '\0';

		char name2[TXT_KEY_LENGTH];
		/* MOD 25-04-94 HG - new paths on pc */
		sprintf(name2, "%s%s", name1, PLANING_PLAN_LIST_EXTENSION);

		char pllPath[DSK_PATH_MAX];
		dskBuildPathName(DISK_CHECK_FILE, DATADISK, name2, pllPath);

		Common::Stream *pllFh = dskOpen(pllPath, 0);
		if (pllFh) {
			uint32 pllData = 0;
			// TODO: Make sure this still works
			//fscanf(pllFh, "%u", &pllData);
			dskGetLine_U32(pllFh, &pllData);
			dskClose(pllFh);

			if ((mode == PLANING_OPEN_WRITE_PLAN) || pllData) {
				sprintf(name2, "MODE_%d", mode);
				plMessage(name2, PLANING_MSG_REFRESH);

				LIST *PlanList = CreateList();

				dbGetObjectName(objId, name1);

				char exp[TXT_KEY_LENGTH];
				exp[0] = EOS;

				for (uint32 i = 0; i < PLANING_NR_PLANS; i++) {
					if ((mode == PLANING_OPEN_WRITE_PLAN)
					        || (pllData & (1L << i))) {
						struct IOData *data;

						if (mode == PLANING_OPEN_WRITE_PLAN) {
							if (pllData & (1L << i))
								g_clue->_txtMgr->getFirstLine(PLAN_TXT, "ATTENTION_1", exp);
							else
								g_clue->_txtMgr->getFirstLine(PLAN_TXT, "ATTENTION_2", exp);
						}

						sprintf(name2, "*%s Plan %d    %s", name1, i + 1, exp);

						if ((data =
						            (IOData *)CreateNode(PlanList, sizeof(struct IOData),
						                                 name2)))
							data->io_Data = i;
					}
				}

				sprintf(name2, "EXPAND_MODE_%d", mode);
				g_clue->_txtMgr->getFirstLine(PLAN_TXT, name2, exp);
				ExpandObjectList(PlanList, exp);

				int i = Bubble(PlanList, 0, NULL, 0L);

				if (ChoiceOk(i, GET_OUT, PlanList)) {
					struct IOData *data;

					if ((data = (IOData *)GetNthNode(PlanList, i)))
						i = data->io_Data;

					dbGetObjectName(lsGetActivAreaID(), name1);
					name1[strlen(name1) - 1] = '\0';

					sprintf(name2, "%s%d%s", name1, i + 1, PLANING_PLAN_EXTENSION);

					char pllPath2[DSK_PATH_MAX];
					dskBuildPathName(DISK_CHECK_FILE, DATADISK, name2, pllPath2);

					*fh = dskOpen(pllPath2, Planing_Open[mode]);

					if (mode == PLANING_OPEN_WRITE_PLAN) {
						pllData |= 1L << i;

						if ((pllFh = dskOpen(pllPath, 1))) {
							// TODO: Make sure this still works
							//dskPrintf(pllFh, "%u", pllData);
							dskSetLine_U32(pllFh, pllData);
							dskClose(pllFh);
						}
					}

					if (*fh) {
						RemoveList(PlanList);
						return PLANING_OPEN_OK;
					} else
						ErrorMsg(Disk_Defect, ERROR_MODULE_PLANING, 0);
				}

				RemoveList(PlanList);

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
		if (GamePlayMode & GP_GUARD_DESIGN) {
			grdDo(fh, plSys, PersonsList, BurglarsNr, PersonsNr, GUARDS_DO_SAVE);
		} else {
			SaveSystem(fh, plSys);
			plSaveTools(fh);

			for (int i = 0; i < BurglarsNr; i++)
				SaveHandler(fh, plSys, OL_NR(GetNthNode(PersonsList, i)));
		}

		dskClose(fh);
	}
}

void plSaveChanged(uint32 objId) {
	if (PlanChanged) {
		LIST *l = g_clue->_txtMgr->goKey(PLAN_TXT, "PLAN_CHANGED");

		inpTurnESC(0);

		if (Bubble(l, 0, NULL, 0L) == 0) {
			inpTurnESC(1);

			if (!plAllInCar(objId))
				plSay("PLAN_NOT_FINISHED", 0);

			plSave(objId);
		} else
			inpTurnESC(1);

		RemoveList(l);
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
		if (GamePlayMode & GP_GUARD_DESIGN)
			grdDo(fh, plSys, PersonsList, BurglarsNr, PersonsNr,
			      GUARDS_DO_LOAD);
		else {
			LIST *l = NULL;
			byte i;
			byte goon = 1;

			if ((l = LoadSystem(fh, plSys))) {
				inpTurnESC(0);
				Bubble(l, 0, NULL, 0L);
				inpTurnESC(1);
				RemoveList(l);

				goon = 0;
				l = NULL;
			}

			if ((l = plLoadTools(fh))) {
				inpTurnESC(0);
				Bubble(l, 0, NULL, 0L);
				inpTurnESC(1);
				RemoveList(l);

				goon = 0;
			}

			if (goon) {
				for (i = 0; i < BurglarsNr; i++)
					LoadHandler(fh, plSys, OL_NR(GetNthNode(PersonsList, i)));
			}
		}

		dskClose(fh);
	} else if (ret == PLANING_OPEN_ERR_NO_PLAN)
		plMessage("NO_PLAN", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
}

} // End of namespace Clue

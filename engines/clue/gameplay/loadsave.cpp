/*
**  $Filename: gameplay/loadsave.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     08-04-94
**
**
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

#include "clue/gameplay/gp_app.h"

namespace Clue {

void tcSaveTheClou() {
	LIST *games = CreateList();
	Player player = (Player)dbGetObject(Player_Player_1);

	/* in welche Datei ?? */
	ShowMenuBackground();
	char line[TXT_KEY_LENGTH];
	txtGetFirstLine(THECLOU_TXT, "SaveGame", line);

	player->CurrScene = film->act_scene->EventNr;
	player->CurrDay = GetDay;
	player->CurrMinute = GetMinute;
	player->CurrLocation = GetLocation;

	char pathname[DSK_PATH_MAX];
	dskBuildPathName(DISK_CHECK_FILE, DATADISK, GAMES_LIST_TXT, pathname);
	if (ReadList(games, 0L, pathname)) {
		inpTurnESC(1);
		inpTurnFunctionKey(0);
		uint16 activ = (uint16) Menu(games, 15L, 0, NULL, 0L);
		inpTurnFunctionKey(1);

		/* Name erstellen */

		if (activ != GET_OUT) {
			char location[TXT_KEY_LENGTH];
			strcpy(location, GetCurrLocName());
			char date[TXT_KEY_LENGTH];
			BuildDate(GetDay, date);

			tcCutName(location, (byte) ' ', 15);

			strcat(location, ", ");
			strcat(location, date);

			sprintf(date, "(%d)", activ + 1);
			strcat(location, date);

			/* Games.list abspeichern */

			NODE *game = (NODE *)CreateNode(0L, 0L, location);

			ReplaceNode(games, NODE_NAME(GetNthNode(games, activ)), game);

			ShowMenuBackground();
			txtGetFirstLine(THECLOU_TXT, "SAVING", line);
			PrintStatus(line);

			WriteList(games, pathname);

			/* Speichern von tcMain */
			sprintf(line, "%s%d%s", MAIN_DATA_NAME, activ, GAME_DATA_EXT);
			dskBuildPathName(DISK_CHECK_DIR, DATADISK, line, pathname);
			dbSaveAllObjects(pathname, DB_tcMain_OFFSET, DB_tcMain_SIZE, 0);

			sprintf(line, "%s%d%s", MAIN_DATA_NAME, activ, GAME_REL_EXT);
			dskBuildPathName(DISK_CHECK_DIR, DATADISK, line, pathname);
			SaveRelations(pathname, DB_tcMain_OFFSET, DB_tcMain_SIZE, 0);

			/* Speichern von tcBuild */
			sprintf(line, "%s%d%s", BUILD_DATA_NAME, activ, GAME_DATA_EXT);
			dskBuildPathName(DISK_CHECK_DIR, DATADISK, line, pathname);
			dbSaveAllObjects(pathname, (uint32)(DB_tcBuild_OFFSET),
			                 (uint32)(DB_tcBuild_SIZE), 0);

			sprintf(line, "%s%d%s", BUILD_DATA_NAME, activ, GAME_REL_EXT);
			dskBuildPathName(DISK_CHECK_DIR, DATADISK, line, pathname);
			SaveRelations(pathname, (uint32) DB_tcBuild_OFFSET, (uint32) DB_tcBuild_SIZE, 0);

			/* Speichern der Story */
			sprintf(line, "%s%d%s", STORY_DATA_NAME, activ, GAME_DATA_EXT);
			dskBuildPathName(DISK_CHECK_DIR, DATADISK, line, pathname);
			tcSaveChangesInScenes(pathname);
		}
	}

	RemoveList(games);
}

bool tcLoadIt(char activ) {
	ShowMenuBackground();
	char line[TXT_KEY_LENGTH];
	txtGetFirstLine(THECLOU_TXT, "LOADING", line);
	PrintStatus(line);

	/* alte Daten löschen */

	tcResetOrganisation();

	RemRelations((uint32) DB_tcMain_OFFSET, (uint32) DB_tcMain_SIZE);
	RemRelations((uint32) DB_tcBuild_OFFSET, (uint32) DB_tcBuild_SIZE);

	dbDeleteAllObjects((uint32) DB_tcMain_OFFSET, (uint32) DB_tcMain_SIZE);
	dbDeleteAllObjects((uint32) DB_tcBuild_OFFSET, (uint32) DB_tcBuild_SIZE);

	/* neue Daten laden ! */

	g_clue->_txtMgr->txtReset(OBJECTS_TXT);

	sprintf(line, "%s%d%s", MAIN_DATA_NAME, (int) activ, GAME_DATA_EXT);
	char pathname[DSK_PATH_MAX];
	dskBuildPathName(DISK_CHECK_FILE, DATADISK, line, pathname);

	bool loaded = false;
	if (dbLoadAllObjects(pathname, 0)) {
		sprintf(line, "%s%d%s", BUILD_DATA_NAME, (int) activ, GAME_DATA_EXT);
		dskBuildPathName(DISK_CHECK_FILE, DATADISK, line, pathname);

		if (dbLoadAllObjects(pathname, 0)) {
			sprintf(line, "%s%d%s", MAIN_DATA_NAME, (int) activ, GAME_REL_EXT);
			dskBuildPathName(DISK_CHECK_FILE, DATADISK, line, pathname);

			if (LoadRelations(pathname, 0)) {
				sprintf(line, "%s%d%s", BUILD_DATA_NAME, (int) activ, GAME_REL_EXT);
				dskBuildPathName(DISK_CHECK_FILE, DATADISK, line, pathname);

				if (LoadRelations(pathname, 0)) {
					sprintf(line, "%s%d%s", STORY_DATA_NAME, (int) activ, GAME_DATA_EXT);
					dskBuildPathName(DISK_CHECK_FILE, DATADISK, line, pathname);

					if (tcLoadChangesInScenes(pathname))
						loaded = true;
				}
			}
		}
	}

	return loaded;
}

bool tcLoadTheClou() {
	char pathname1[DSK_PATH_MAX];
	char pathname2[DSK_PATH_MAX];
	dskBuildPathName(DISK_CHECK_FILE, DATADISK, GAMES_LIST_TXT, pathname1);
	dskBuildPathName(DISK_CHECK_FILE, DATADISK, GAMES_ORIG_TXT, pathname2);

	LIST *games = CreateList();
	LIST *origin = CreateList();
	bool loaded = false;
	if (ReadList(games, 0L, pathname1) && ReadList(origin, 0L, pathname2)) {
		ShowMenuBackground();
		char line[TXT_KEY_LENGTH];
		txtGetFirstLine(THECLOU_TXT, "LoadAGame", line);

		inpTurnFunctionKey(0);
		inpTurnESC(1);
		uint32 activ = (uint32) Menu(games, 15L, 0, NULL, 0L);
		inpTurnFunctionKey(1);

		if ((activ != GET_OUT) 
			&& (strcmp(NODE_NAME(GetNthNode(games, (int32) activ)), NODE_NAME(GetNthNode(origin, (int32) activ))))) {
			loaded = tcLoadIt((byte) activ);
		} else {
			ShowMenuBackground();

			txtGetFirstLine(THECLOU_TXT, "NOT_LOADING", line);
			PrintStatus(line);
			inpWaitFor(INP_LBUTTONP);

			ShowMenuBackground();
			SetLocation(-1);

			RemoveList(games);
			RemoveList(origin);

			Player player = (Player)dbGetObject(Player_Player_1);
			if (player) {  /* MOD 04-02 */
				player->CurrScene = film->act_scene->EventNr;

				SceneArgs.ReturnValue = film->act_scene->EventNr;
			}

			return false;
		}
	} else
		ErrorMsg(Disk_Defect, ERROR_MODULE_LOADSAVE, 1);

	tcRefreshAfterLoad(loaded);

	RemoveList(games);
	RemoveList(origin);

	return loaded;
}

void tcRefreshAfterLoad(bool loaded) {
	Player player = (Player)dbGetObject(Player_Player_1);   /* muß hier geholt werden -> sonst alte Adresse */

	if (!loaded) {
		if (player)
			player->CurrScene = 0L;
		ErrorMsg(Disk_Defect, ERROR_MODULE_LOADSAVE, 2);
	} else if (player) {
		SetDay(player->CurrDay);
		SetTime(player->CurrMinute);
		SetLocation(-1);    /* auf alle Fälle ein Refresh! */

		SceneArgs.ReturnValue = GetLocScene(player->CurrLocation)->EventNr;
	}
}

bool tcSaveChangesInScenes(const char *fileName) {
	bool back = false;
	Common::Stream *file = dskOpen(fileName, 1);
	if (file) {
		dskSetLine_U32(file, film->EnabledChoices);

		for (uint32 i = 0; i < film->AmountOfScenes; i++) {
			dskSetLine_U32(file, film->gameplay[i].EventNr);
			dskSetLine_U16(file, film->gameplay[i].Geschehen);
		}

		dskClose(file);
		back = true;
	}

	return (back);
}

bool tcLoadChangesInScenes(const char *fileName) {
	bool back = true;
	Common::Stream *file = dskOpen(fileName, 0);
	if (file) {
		uint32 choice;
		dskGetLine_U32(file, &choice);
		SetEnabledChoices(choice);

		for (uint32 i = 0; i < film->AmountOfScenes; i++) {
			uint32 eventNr;
			uint16 count;
			dskGetLine_U32(file, &eventNr);
			dskGetLine_U16(file, &count);

			struct Scene *sc = GetScene(eventNr);
			if (sc)
				sc->Geschehen = count;
			else
				back = false;
		}
		dskClose(file);
	} else
		back = false;

	return back;
}

} // End of namespace Clue

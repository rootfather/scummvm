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
	NewList<class NewNode> *games = new NewList<class NewNode>;
	PlayerNode *player = (PlayerNode *)dbGetObject(Player_Player_1);

	/* in welche Datei ?? */
	ShowMenuBackground();
	Common::String line = g_clue->_txtMgr->getFirstLine(THECLOU_TXT, "SaveGame");

	player->CurrScene = film->act_scene->EventNr;
	player->CurrDay = GetDay;
	player->CurrMinute = GetMinute;
	player->CurrLocation = GetLocation;

	char pathname[DSK_PATH_MAX];
	dskBuildPathName(DISK_CHECK_FILE, DATADISK, GAMES_LIST_TXT, pathname);
	games->readList(pathname);
	
	inpTurnESC(true);
	inpTurnFunctionKey(false);
	uint16 activ = (uint16) Menu(games, 15, 0, nullptr, 0);
	inpTurnFunctionKey(true);

	/* Name erstellen */

	if (activ != GET_OUT) {
		char location[TXT_KEY_LENGTH];
		strcpy(location, GetCurrLocName().c_str());
		char date[TXT_KEY_LENGTH];
		BuildDate(GetDay, date);

		tcCutName(location, (byte) ' ', 15);

		strcat(location, ", ");
		strcat(location, date);

		sprintf(date, "(%d)", activ + 1);
		strcat(location, date);

		/* Games.list abspeichern */

		NewNode *game = new NewNode();
		game->_name = location;
		games->replaceNode(games->getNthNode(activ)->_name, game);

		ShowMenuBackground();
		line = g_clue->_txtMgr->getFirstLine(THECLOU_TXT, "SAVING");
		PrintStatus(line);

		games->writeList(pathname);

		/* Speichern von tcMain */
		line = Common::String::format("%s%d%s", MAIN_DATA_NAME, activ, GAME_DATA_EXT);
		dskBuildPathName(DISK_CHECK_DIR, DATADISK, line.c_str(), pathname);
		dbSaveAllObjects(pathname, DB_tcMain_OFFSET, DB_tcMain_SIZE);

		line = Common::String::format("%s%d%s", MAIN_DATA_NAME, activ, GAME_REL_EXT);
		dskBuildPathName(DISK_CHECK_DIR, DATADISK, line.c_str(), pathname);
		SaveRelations(pathname, DB_tcMain_OFFSET, DB_tcMain_SIZE);

		/* Speichern von tcBuild */
		line = Common::String::format("%s%d%s", BUILD_DATA_NAME, activ, GAME_DATA_EXT);
		dskBuildPathName(DISK_CHECK_DIR, DATADISK, line.c_str(), pathname);
		dbSaveAllObjects(pathname, (uint32)(DB_tcBuild_OFFSET), (uint32)DB_tcBuild_SIZE);

		line = Common::String::format("%s%d%s", BUILD_DATA_NAME, activ, GAME_REL_EXT);
		dskBuildPathName(DISK_CHECK_DIR, DATADISK, line.c_str(), pathname);
		SaveRelations(pathname, (uint32) DB_tcBuild_OFFSET, (uint32) DB_tcBuild_SIZE);

		/* Speichern der Story */
		line = Common::String::format("%s%d%s", STORY_DATA_NAME, activ, GAME_DATA_EXT);
		dskBuildPathName(DISK_CHECK_DIR, DATADISK, line.c_str(), pathname);
		tcSaveChangesInScenes(pathname);
	}

	games->removeList();
}

bool tcLoadIt(char activ) {
	ShowMenuBackground();
	Common::String line = g_clue->_txtMgr->getFirstLine(THECLOU_TXT, "LOADING");
	PrintStatus(line);

	/* alte Daten löschen */

	tcResetOrganisation();

	RemRelations((uint32) DB_tcMain_OFFSET, (uint32) DB_tcMain_SIZE);
	RemRelations((uint32) DB_tcBuild_OFFSET, (uint32) DB_tcBuild_SIZE);

	dbDeleteAllObjects((uint32) DB_tcMain_OFFSET, (uint32) DB_tcMain_SIZE);
	dbDeleteAllObjects((uint32) DB_tcBuild_OFFSET, (uint32) DB_tcBuild_SIZE);

	/* neue Daten laden ! */

	g_clue->_txtMgr->reset(OBJECTS_TXT);

	line = Common::String::format("%s%d%s", MAIN_DATA_NAME, (int) activ, GAME_DATA_EXT);
	char pathname[DSK_PATH_MAX];
	dskBuildPathName(DISK_CHECK_FILE, DATADISK, line.c_str(), pathname);

	bool loaded = false;
	if (dbLoadAllObjects(pathname)) {
		line = Common::String::format("%s%d%s", BUILD_DATA_NAME, (int) activ, GAME_DATA_EXT);
		dskBuildPathName(DISK_CHECK_FILE, DATADISK, line.c_str(), pathname);

		if (dbLoadAllObjects(pathname)) {
			line = Common::String::format("%s%d%s", MAIN_DATA_NAME, (int) activ, GAME_REL_EXT);
			dskBuildPathName(DISK_CHECK_FILE, DATADISK, line.c_str(), pathname);

			if (LoadRelations(pathname)) {
				line = Common::String::format("%s%d%s", BUILD_DATA_NAME, (int) activ, GAME_REL_EXT);
				dskBuildPathName(DISK_CHECK_FILE, DATADISK, line.c_str(), pathname);

				if (LoadRelations(pathname)) {
					line = Common::String::format("%s%d%s", STORY_DATA_NAME, (int) activ, GAME_DATA_EXT);
					dskBuildPathName(DISK_CHECK_FILE, DATADISK, line.c_str(), pathname);

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

	NewList<NewNode> *games = new NewList<NewNode>;
	NewList<NewNode> *origin = new NewList<NewNode>;
	bool loaded = false;
	games->readList(pathname1);
	origin->readList(pathname2);

	ShowMenuBackground();
	Common::String line = g_clue->_txtMgr->getFirstLine(THECLOU_TXT, "LoadAGame");

	inpTurnFunctionKey(false);
	inpTurnESC(true);
	uint32 activ = (uint32) Menu(games, 15, 0, NULL, 0);
	inpTurnFunctionKey(true);

	if ((activ != GET_OUT) 
		&& strcmp(games->getNthNode((int32) activ)->_name.c_str(), origin->getNthNode((int32) activ)->_name.c_str())) {
		loaded = tcLoadIt((byte) activ);
	} else {
		ShowMenuBackground();

		line = g_clue->_txtMgr->getFirstLine(THECLOU_TXT, "NOT_LOADING");
		PrintStatus(line);
		inpWaitFor(INP_LBUTTONP);

		ShowMenuBackground();
		SetLocation(-1);

		games->removeList();
		origin->removeList();

		PlayerNode *player = (PlayerNode *)dbGetObject(Player_Player_1);
		if (player) {  /* MOD 04-02 */
			player->CurrScene = film->act_scene->EventNr;

			_sceneArgs._returnValue = film->act_scene->EventNr;
		}

		return false;
	}

	tcRefreshAfterLoad(loaded);

	games->removeList();
	origin->removeList();

	return loaded;
}

void tcRefreshAfterLoad(bool loaded) {
	PlayerNode *player = (PlayerNode *)dbGetObject(Player_Player_1);   /* muß hier geholt werden -> sonst alte Adresse */

	if (!loaded) {
		if (player)
			player->CurrScene = 0;
		ErrorMsg(Disk_Defect, ERROR_MODULE_LOADSAVE, 2);
	} else if (player) {
		SetDay(player->CurrDay);
		SetTime(player->CurrMinute);
		SetLocation(-1);    /* auf alle Fälle ein Refresh! */

		_sceneArgs._returnValue = GetLocScene(player->CurrLocation)->EventNr;
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

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

#include "clue/gameplay/gp.h"
#include "clue/gameplay/gp_app.h"
#include "clue/organisa/organisa.h"

namespace Clue {

void tcSaveTheClou(void) {
	char line[TXT_KEY_LENGTH];
	char location[TXT_KEY_LENGTH];
	char date[TXT_KEY_LENGTH];
	char pathname[DSK_PATH_MAX];
	LIST *games = CreateList();
	NODE *game;
	uint16 activ;
	Player player = (Player)dbGetObject(Player_Player_1);

	/* in welche Datei ?? */
	ShowMenuBackground();
	txtGetFirstLine(THECLOU_TXT, "SaveGame", line);

	player->CurrScene = film->act_scene->EventNr;
	player->CurrDay = GetDay;
	player->CurrMinute = GetMinute;
	player->CurrLocation = GetLocation;

	dskBuildPathName(DISK_CHECK_FILE, DATADISK, GAMES_LIST_TXT, pathname);
	if (ReadList(games, 0L, pathname)) {
		inpTurnESC(1);
		inpTurnFunctionKey(0);
		activ = (uint16) Menu(games, 15L, 0, NULL, 0L);
		inpTurnFunctionKey(1);

		/* Name erstellen */

		if (activ != GET_OUT) {
			strcpy(location, GetCurrLocName());
			BuildDate(GetDay, date);

			tcCutName(location, (byte) ' ', 15);

			strcat(location, ", ");
			strcat(location, date);

			sprintf(date, "(%d)", activ + 1);
			strcat(location, date);

			/* Games.list abspeichern */

			game = (NODE *)CreateNode(0L, 0L, location);

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
			SaveRelations(pathname, (uint32) DB_tcBuild_OFFSET, (uint32) DB_tcBuild_SIZE,
			              0);

			/* Speichern der Story */
			sprintf(line, "%s%d%s", STORY_DATA_NAME, activ, GAME_DATA_EXT);
			dskBuildPathName(DISK_CHECK_DIR, DATADISK, line, pathname);
			tcSaveChangesInScenes(pathname);
		}
	}

	RemoveList(games);
}

byte tcLoadIt(char activ) {
	char pathname[DSK_PATH_MAX];
	char line[TXT_KEY_LENGTH];
	byte loaded = 0;

	ShowMenuBackground();
	txtGetFirstLine(THECLOU_TXT, "LOADING", line);
	PrintStatus(line);

	/* alte Daten löschen */

	tcResetOrganisation();

	RemRelations((uint32) DB_tcMain_OFFSET, (uint32) DB_tcMain_SIZE);
	RemRelations((uint32) DB_tcBuild_OFFSET, (uint32) DB_tcBuild_SIZE);

	dbDeleteAllObjects((uint32) DB_tcMain_OFFSET, (uint32) DB_tcMain_SIZE);
	dbDeleteAllObjects((uint32) DB_tcBuild_OFFSET, (uint32) DB_tcBuild_SIZE);

	/* neue Daten laden ! */

	txtReset(OBJECTS_TXT);

	sprintf(line, "%s%d%s", MAIN_DATA_NAME, (int) activ, GAME_DATA_EXT);
	dskBuildPathName(DISK_CHECK_FILE, DATADISK, line, pathname);
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
						loaded = 1;
				}
			}
		}
	}

	return loaded;
}

byte tcLoadTheClou(void) {
	char line[TXT_KEY_LENGTH];
	byte loaded = 0;
	LIST *games = CreateList();
	LIST *origin = CreateList();
	uint32 activ;
	Player player;
	char pathname1[DSK_PATH_MAX];
	char pathname2[DSK_PATH_MAX];

	dskBuildPathName(DISK_CHECK_FILE, DATADISK, GAMES_LIST_TXT, pathname1);
	dskBuildPathName(DISK_CHECK_FILE, DATADISK, GAMES_ORIG_TXT, pathname2);

	if (ReadList(games, 0L, pathname1) && ReadList(origin, 0L, pathname2)) {
		ShowMenuBackground();
		txtGetFirstLine(THECLOU_TXT, "LoadAGame", line);

		inpTurnFunctionKey(0);
		inpTurnESC(1);
		activ = (uint32) Menu(games, 15L, 0, NULL, 0L);
		inpTurnFunctionKey(1);

		if ((activ != GET_OUT)
		        &&
		        (strcmp
		         (NODE_NAME(GetNthNode(games, (int32) activ)),
		          NODE_NAME(GetNthNode(origin, (int32) activ))))) {
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

			if ((player = (Player)dbGetObject(Player_Player_1))) {  /* MOD 04-02 */
				player->CurrScene = film->act_scene->EventNr;

				SceneArgs.ReturnValue = film->act_scene->EventNr;
			}

			return 0;
		}
	} else
		ErrorMsg(Disk_Defect, ERROR_MODULE_LOADSAVE, 1);

	tcRefreshAfterLoad(loaded);

	RemoveList(games);
	RemoveList(origin);

	return loaded;
}

void tcRefreshAfterLoad(byte loaded) {
	Player player = (Player)dbGetObject(Player_Player_1);   /* muß hier geholt werden -> sonst alte Adresse */

	if (!loaded) {
		if (player)
			player->CurrScene = 0L;
		ErrorMsg(Disk_Defect, ERROR_MODULE_LOADSAVE, 2);
	} else {
		if (player) {
			SetDay(player->CurrDay);
			SetTime(player->CurrMinute);
			SetLocation(-1);    /* auf alle Fälle ein Refresh! */

			SceneArgs.ReturnValue = GetLocScene(player->CurrLocation)->EventNr;
		}
	}
}

byte tcSaveChangesInScenes(const char *fileName) {
	uint32 i;
	byte back = 0;
	FILE *file;

	if ((file = dskOpen(fileName, "wb"))) {
		fprintf(file, "%u\r\n", film->EnabledChoices);

		for (i = 0; i < film->AmountOfScenes; i++) {
			fprintf(file, "%u\r\n", film->gameplay[i].EventNr);
			fprintf(file, "%hu\r\n", film->gameplay[i].Geschehen);
		}

		dskClose(file);
		back = 1;
	}

	return (back);
}

byte tcLoadChangesInScenes(const char *fileName) {
	uint32 i;
	byte back = 1;
	uint32 eventNr, choice;
	uint16 count;
	FILE *file;
	struct Scene *sc;

	if ((file = dskOpen(fileName, "rb"))) {
		fscanf(file, "%u\r\n", &choice);
		SetEnabledChoices(choice);

		for (i = 0; i < film->AmountOfScenes; i++) {
			fscanf(file, "%u\r\n", &eventNr);
			fscanf(file, "%hu\r\n", &count);

			if ((sc = GetScene(eventNr)))
				sc->Geschehen = count;
			else
				back = 0;
		}
		dskClose(file);
	} else
		back = 0;

	return (back);
}

} // End of namespace Clue

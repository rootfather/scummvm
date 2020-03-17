/*
**      $Filename: base/base.c
**      $Release:  0
**      $Revision: 0.1
**      $Date:     06-02-94
**
**      basic functions for "Der Clou!"
**
**   (c) 1994 ...and avoid panic by, H. Gaberschek
**          All Rights Reserved.
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
#include "clue/sound/hsc.h"


namespace Clue {

void *StdBuffer0 = NULL;
void *StdBuffer1 = NULL;

void tcDone() {
	static bool inprogress;

	if (!inprogress) {
		inprogress = true;

		plDone();
		sndDoneFX();        /* attention! */
		sndDone();
		RemoveAudio();
		dbDone();
		CloseAnimHandler();

		delete g_clue->_txtMgr;
		g_clue->_txtMgr = nullptr;

		gfxDone();

		if (g_clue->getFeatures() & GF_CDAUDIO) {
			if (CDRomInstalled) {
				CDROM_StopAudioTrack();
				CDROM_UnInstall();
			}
		}

		if (StdBuffer1)
			TCFreeMem(StdBuffer1, STD_BUFFER1_SIZE);

		if (StdBuffer0)
			TCFreeMem(StdBuffer0, STD_BUFFER0_SIZE);

		if (memGetAllocatedMem())
			DebugMsg(ERR_DEBUG, ERROR_MODULE_BASE, "Attention: dirty mem: %ld bytes!!!", memGetAllocatedMem());
		else
			DebugMsg(ERR_DEBUG, ERROR_MODULE_BASE, "all mem returned to pool");
	}
}

bool ClueEngine::tcInit() {
	InitAudio();

	StdBuffer1 = TCAllocMem(STD_BUFFER1_SIZE, true);
	StdBuffer0 = TCAllocMem(STD_BUFFER0_SIZE, true);

	if (!StdBuffer0 || !StdBuffer1) {
		return false;
	}
	 
	if (g_clue->getFeatures() & GF_CDAUDIO) {
		if ((CDRomInstalled = CDROM_Install())) {
			CDROM_WaitForMedia();
			return false;
		}
	}

	gfxInit();

	sndInit();

	if (!(GamePlayMode & GP_NO_SAMPLES))
		sndInitFX();

	ShowIntro();

	hscReset();

	gfxSetRGB(NULL, 0, 0, 64, 48);
	gfxSetRGB(NULL, 255, 255, 255, 255);    /* mouse */

	/* Start game. */
	inpOpenAllInputDevs();

	g_clue->_txtMgr->txtInit(g_clue->getTxtLanguage());

	InitAnimHandler();

	dbInit();
	plInit();

	gfxCollToMem(128, &StdRP0InMem);    /* cache Menu in StdRP0InMem */
	gfxCollToMem(129, &StdRP1InMem);    /* cache Bubbles in StdRP1InMem */

	CurrentBackground = BGD_LONDON;
	return true;
}

static void InitData() {
	char MainData [DSK_PATH_MAX];
	char BuildData[DSK_PATH_MAX];
	char MainRel  [DSK_PATH_MAX];
	char BuildRel [DSK_PATH_MAX];
	bool result;

	result = false;

	dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, MAIN_DATA_NAME GAME_DATA_EXT, MainData);
	dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, BUILD_DATA_NAME GAME_DATA_EXT, BuildData);

	dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, MAIN_DATA_NAME GAME_REL_EXT, MainRel);
	dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, BUILD_DATA_NAME GAME_REL_EXT, BuildRel);

	g_clue->_txtMgr->txtReset(OBJECTS_TXT);

	if (dbLoadAllObjects(MainData, 0))
		if (dbLoadAllObjects(BuildData, 0))
			if (LoadRelations(MainRel, 0))
				if (LoadRelations(BuildRel, 0)) {
					InitTaxiLocations();

					result = true;
				}

	if (!result)
		ErrorMsg(Disk_Defect, ERROR_MODULE_BASE, 1);
}

void ClueEngine::closeData() {
	RemRelations(0L, 0L);
	dbDeleteAllObjects(0L, 0L);
}

void tcSetPermanentColors() {
	uint8 palette[GFX_PALETTE_SIZE];

	palette[248 * 3 + 0] = 116;
	palette[248 * 3 + 1] = 224;
	palette[248 * 3 + 2] = 142;

	palette[249 * 3 + 0] = 224;
	palette[249 * 3 + 1] = 224;
	palette[249 * 3 + 2] = 224;

	palette[250 * 3 + 0] = 55;
	palette[250 * 3 + 1] = 12;
	palette[250 * 3 + 2] = 32;

	palette[251 * 3 + 0] = 128;
	palette[251 * 3 + 1] = 29;
	palette[251 * 3 + 2] = 75;

	palette[252 * 3 + 0] = 170;
	palette[252 * 3 + 1] = 170;
	palette[252 * 3 + 2] = 170;

	palette[253 * 3 + 0] = 104;
	palette[253 * 3 + 1] = 104;
	palette[253 * 3 + 2] = 104;

	palette[254 * 3 + 0] = 0;
	palette[254 * 3 + 1] = 0;
	palette[254 * 3 + 2] = 0;

	gfxSetColorRange(248, 254);
	gfxChangeColors(NULL, 0, GFX_BLEND_UP, palette);
}

void ClueEngine::setFullEnviroment() {
	hasSetP(Person_Matt_Stuvysunt, Ability_Elektronik, 251);
	hasSetP(Person_Matt_Stuvysunt, Ability_Schloesser, 210);
	hasSetP(Person_Matt_Stuvysunt, Ability_Aufpassen, 180);
	hasSetP(Person_Matt_Stuvysunt, Ability_Schloesser, 180);

	hasSetP(Person_Marc_Smith, Ability_Autos, 255);
	hasSetP(Person_Mohammed_Abdula, Ability_Kampf, 255);

	/* Matt has all cars */
	hasSet(Person_Matt_Stuvysunt, Car_Triumph_Roadstar_1947);
	hasSet(Person_Matt_Stuvysunt, Car_Triumph_Roadstar_1949);
	hasSet(Person_Matt_Stuvysunt, Car_Cadillac_Club_1952);
	hasSet(Person_Matt_Stuvysunt, Car_Opel_Olympia_1953);
	hasSet(Person_Matt_Stuvysunt, Car_Standard_Vanguard_1950);
	hasSet(Person_Matt_Stuvysunt, Car_Standard_Vanguard_1951);
	hasSet(Person_Matt_Stuvysunt, Car_Standard_Vanguard_1953);
	hasSet(Person_Matt_Stuvysunt, Car_Jaguar_XK_1950);
	hasSet(Person_Matt_Stuvysunt, Car_Pontiac_Streamliner_1944);
	hasSet(Person_Matt_Stuvysunt, Car_Pontiac_Streamliner_1946);
	hasSet(Person_Matt_Stuvysunt, Car_Pontiac_Streamliner_1949);
	hasSet(Person_Matt_Stuvysunt, Car_Fiat_Topolino_1936);
	hasSet(Person_Matt_Stuvysunt, Car_Fiat_Topolino_1940);
	hasSet(Person_Matt_Stuvysunt, Car_Fiat_Topolino_1942);
	hasSet(Person_Matt_Stuvysunt, Car_Morris_Minor_1948);
	hasSet(Person_Matt_Stuvysunt, Car_Morris_Minor_1950);
	hasSet(Person_Matt_Stuvysunt, Car_Morris_Minor_1953);

	/* Matt has all tools
	   Removed : Hand,
	   Fuss,
	   Stechkarte */
	hasSet(Person_Matt_Stuvysunt, Tool_Dietrich);
	hasSet(Person_Matt_Stuvysunt, Tool_Hammer);
	hasSet(Person_Matt_Stuvysunt, Tool_Axt);
	hasSet(Person_Matt_Stuvysunt, Tool_Kernbohrer);
	hasSet(Person_Matt_Stuvysunt, Tool_Handschuhe);
	hasSet(Person_Matt_Stuvysunt, Tool_Schuhe);
	hasSet(Person_Matt_Stuvysunt, Tool_Maske);
	hasSet(Person_Matt_Stuvysunt, Tool_Chloroform);
	hasSet(Person_Matt_Stuvysunt, Tool_Brecheisen);
	hasSet(Person_Matt_Stuvysunt, Tool_Winkelschleifer);
	hasSet(Person_Matt_Stuvysunt, Tool_Bohrmaschine);
	hasSet(Person_Matt_Stuvysunt, Tool_Bohrwinde);
	hasSet(Person_Matt_Stuvysunt, Tool_Schloszstecher);
	hasSet(Person_Matt_Stuvysunt, Tool_Schneidbrenner);
	hasSet(Person_Matt_Stuvysunt, Tool_Sauerstofflanze);
	hasSet(Person_Matt_Stuvysunt, Tool_Stethoskop);
	hasSet(Person_Matt_Stuvysunt, Tool_Batterie);
	hasSet(Person_Matt_Stuvysunt, Tool_Stromgenerator);
	hasSet(Person_Matt_Stuvysunt, Tool_Elektroset);
	hasSet(Person_Matt_Stuvysunt, Tool_Schutzanzug);
	hasSet(Person_Matt_Stuvysunt, Tool_Dynamit);
	hasSet(Person_Matt_Stuvysunt, Tool_Elektrohammer);
	hasSet(Person_Matt_Stuvysunt, Tool_Glasschneider);
	hasSet(Person_Matt_Stuvysunt, Tool_Strickleiter);
	hasSet(Person_Matt_Stuvysunt, Tool_Funkgeraet);

	/* Matt has asked all Persons for a job
	   Removed : Old Matt,
	   Matt Stuvysunt,
	   John Gludo,
	   Miles Chickenwing,
	   Sabien Pardo,
	   Red Stanson */
	joinSet(Person_Matt_Stuvysunt, Person_Paul_O_Conner);
	joinSet(Person_Matt_Stuvysunt, Person_Marc_Smith);
	joinSet(Person_Matt_Stuvysunt, Person_Frank_Meier);
	joinSet(Person_Matt_Stuvysunt, Person_Herbert_Briggs);
	joinSet(Person_Matt_Stuvysunt, Person_Tony_Allen);
	joinSet(Person_Matt_Stuvysunt, Person_Margret_Allen);
	joinSet(Person_Matt_Stuvysunt, Person_Mathew_Black);
	joinSet(Person_Matt_Stuvysunt, Person_Miguel_Garcia);
	joinSet(Person_Matt_Stuvysunt, Person_Mike_Seeger);
	joinSet(Person_Matt_Stuvysunt, Person_Albert_Liet);
	joinSet(Person_Matt_Stuvysunt, Person_Mohammed_Abdula);
	joinSet(Person_Matt_Stuvysunt, Person_Serge_Fontane);
	joinSet(Person_Matt_Stuvysunt, Person_Frank_Maloya);
	joinSet(Person_Matt_Stuvysunt, Person_Neil_Grey);
	joinSet(Person_Matt_Stuvysunt, Person_Mark_Hart);
	joinSet(Person_Matt_Stuvysunt, Person_Jim_Danner);
	joinSet(Person_Matt_Stuvysunt, Person_Robert_Bull);
	joinSet(Person_Matt_Stuvysunt, Person_Thomas_Groul);
	joinSet(Person_Matt_Stuvysunt, Person_Helen_Parker);
	joinSet(Person_Matt_Stuvysunt, Person_Dan_Stanford);
	joinSet(Person_Matt_Stuvysunt, Person_Mary_Bolton);
	joinSet(Person_Matt_Stuvysunt, Person_Eric_Pooly);
	joinSet(Person_Matt_Stuvysunt, Person_Prof_Emil_Schmitt);
	joinSet(Person_Matt_Stuvysunt, Person_Elisabeth_Schmitt);
	joinSet(Person_Matt_Stuvysunt, Person_Margrete_Briggs);
	joinSet(Person_Matt_Stuvysunt, Person_Peter_Brook);
	joinSet(Person_Matt_Stuvysunt, Person_Samuel_Rosenblatt);
	joinSet(Person_Matt_Stuvysunt, Person_Lucas_Grull);
	joinSet(Person_Matt_Stuvysunt, Person_Ken_Addison);
	joinSet(Person_Matt_Stuvysunt, Person_Jiri_Poulin);
	joinSet(Person_Matt_Stuvysunt, Person_Garry_Stevenson);
	joinSet(Person_Matt_Stuvysunt, Person_Al_Mel);
	joinSet(Person_Matt_Stuvysunt, Person_Jack_Motart);
	joinSet(Person_Matt_Stuvysunt, Person_Kevin_Smith);
	joinSet(Person_Matt_Stuvysunt, Person_Mike_Kahn);
	joinSet(Person_Matt_Stuvysunt, Person_Frank_De_Silva);
	joinSet(Person_Matt_Stuvysunt, Person_Justin_White);
	joinSet(Person_Matt_Stuvysunt, Person_John_O_Keef);
	joinSet(Person_Matt_Stuvysunt, Person_Luthmilla_Nervesaw);
	joinSet(Person_Matt_Stuvysunt, Person_Thomas_Smith);
	joinSet(Person_Matt_Stuvysunt, Person_Ben_Riggley);
	joinSet(Person_Matt_Stuvysunt, Person_Richard_Doil);
	joinSet(Person_Matt_Stuvysunt, Person_Pater_James);

	/* Matt has investigated all buildings */
	hasSet(Person_Matt_Stuvysunt, Building_Kiosk);
	((Building) dbGetObject(Building_Kiosk))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Tante_Emma_Laden);
	((Building) dbGetObject(Building_Tante_Emma_Laden))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Juwelier);
	((Building) dbGetObject(Building_Juwelier))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Old_Curiosity_Shop);
	((Building) dbGetObject(Building_Old_Curiosity_Shop))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Pink_Villa);
	((Building) dbGetObject(Building_Pink_Villa))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Seniorenheim);
	((Building) dbGetObject(Building_Seniorenheim))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Grab_von_Karl_Marx);
	((Building) dbGetObject(Building_Grab_von_Karl_Marx))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Villa);
	((Building) dbGetObject(Building_Villa))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Kenwood_House);
	((Building) dbGetObject(Building_Kenwood_House))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Ham_House);
	((Building) dbGetObject(Building_Ham_House))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Osterly_Park_House);
	((Building) dbGetObject(Building_Osterly_Park_House))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Chiswick_House);
	((Building) dbGetObject(Building_Chiswick_House))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Suterbys);
	((Building) dbGetObject(Building_Suterbys))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_British_Museum);
	((Building) dbGetObject(Building_British_Museum))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Natural_Museum);
	((Building) dbGetObject(Building_Natural_Museum))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_National_Galery);
	((Building) dbGetObject(Building_National_Galery))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Victoria_und_Albert_Museum);
	((Building) dbGetObject(Building_Victoria_und_Albert_Museum))->Exactlyness =
	    255;
	hasSet(Person_Matt_Stuvysunt, Building_Bank_of_England);
	((Building) dbGetObject(Building_Bank_of_England))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Tower_of_London);
	((Building) dbGetObject(Building_Tower_of_London))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Starford_Kaserne);
	((Building) dbGetObject(Building_Starford_Kaserne))->Exactlyness = 255;

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		hasSet(Person_Matt_Stuvysunt, Car_Rover_75_1949);
		hasSet(Person_Matt_Stuvysunt, Car_Rover_75_1950);
		hasSet(Person_Matt_Stuvysunt, Car_Rover_75_1952);
		hasSet(Person_Matt_Stuvysunt, Car_Ford_Model_T__1926);
		hasSet(Person_Matt_Stuvysunt, Car_Bentley_Continental_Typ_R_1952);
		hasSet(Person_Matt_Stuvysunt, Car_Bentley_Continental_Typ_R_1953);
		hasSet(Person_Matt_Stuvysunt, Car_Fiat_634_N_1936);
		hasSet(Person_Matt_Stuvysunt, Car_Fiat_634_N_1943);

		joinSet(Person_Matt_Stuvysunt, Person_Tom_Cooler);
		joinSet(Person_Matt_Stuvysunt, Person_Sid_Palmer);
		joinSet(Person_Matt_Stuvysunt, Person_Dean_Esters);
		joinSet(Person_Matt_Stuvysunt, Person_Prof_Marcus_Green);
		joinSet(Person_Matt_Stuvysunt, Person_Melanie_Morgan);
		joinSet(Person_Matt_Stuvysunt, Person_Rosi_Lambert);
		joinSet(Person_Matt_Stuvysunt, Person_Rod_Masterson);
		joinSet(Person_Matt_Stuvysunt, Person_Tina_Olavson);
		joinSet(Person_Matt_Stuvysunt, Person_Phil_Ciggy);
		joinSet(Person_Matt_Stuvysunt, Person_Pere_Ubu);

		hasSet(Person_Matt_Stuvysunt, Building_Baker_Street);
		((Building) dbGetObject(Building_Baker_Street))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Madame_Tussaud);
		((Building) dbGetObject(Building_Madame_Tussaud))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Westminster_Abbey);
		((Building) dbGetObject(Building_Westminster_Abbey))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Downing_Street_10);
		((Building) dbGetObject(Building_Downing_Street_10))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Postzug);
		((Building) dbGetObject(Building_Postzug))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Tate_Gallery);
		((Building) dbGetObject(Building_Tate_Gallery))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Buckingham_Palace);
		((Building) dbGetObject(Building_Buckingham_Palace))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Bulstrode_Chemestry_Ltd);
		((Building) dbGetObject(Building_Bulstrode_Chemestry_Ltd))->Exactlyness =
		    255;
	}
}

byte ClueEngine::startupMenu() {
	LIST *menu = g_clue->_txtMgr->txtGoKey(MENU_TXT, "STARTUP_MENU");
	uint32 activ;
	char line[TXT_KEY_LENGTH];
	byte ret = 0;

	ShowMenuBackground();

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		if (g_clue->getFeatures() & ADGF_CD) {
			PrintStatus(txtGetFirstLine(THECLOU_TXT,
			                            "BITTE_WARTEN_PC_CD_ROM_PROFI", line));
		} else {
			PrintStatus(txtGetFirstLine(THECLOU_TXT,
			                            "BITTE_WARTEN_PC_PROFI", line));
		}
	} else {
		if (g_clue->getFeatures() & ADGF_CD) {
			PrintStatus(txtGetFirstLine(THECLOU_TXT,
			                            "BITTE_WARTEM_PC_CD_ROM", line));
		} else {
			PrintStatus(txtGetFirstLine(THECLOU_TXT,
			                            "BITTE_WARTEN_PC", line));
		}
	}

	inpTurnFunctionKey(0);
	inpTurnESC(0);

	activ = Menu(menu, 7L, 0, NULL, 0L);

	inpTurnESC(1);
	inpTurnFunctionKey(1);

	switch (activ) {
	case 0:
		InitData();
		ret = 1;
		break;

	case 1:
		g_clue->_txtMgr->txtReset(OBJECTS_TXT);

		if (tcLoadTheClou()) {
			film->StartScene = SceneArgs.ReturnValue;
			ret = 1;
		}
		break;

	case 2:
		ret = 2;
		break;
	}

	RemoveList(menu);

	return ret;
}

#if 0
TODO : Implement trainer in the debugger
	
/**********************************************************/
static void parseOptions(int argc, char *argv[]) {
// MOST OF THE CODE HAS BEEN REMOVED
// The remaining code has to be moved to debugger commands.

	for (int i = 1; i < argc; i++) {
		const char *s = argv[i];

		if (s[0] == '-') {
			switch (s[1]) {

			case 't':
				if (OptionSet(s + 2, 'd'))
					GamePlayMode |= GP_DEMO | GP_STORY_OFF;

				if (OptionSet(s + 2, 's'))
					GamePlayMode |= GP_STORY_OFF;

				if (OptionSet(s + 2, 'f'))
					GamePlayMode |= GP_FULL_ENV;

				if (OptionSet(s + 2, 'l'))
					GamePlayMode |= GP_LEVEL_DESIGN;

				if (OptionSet(s + 2, 'g'))
					GamePlayMode |= GP_GUARD_DESIGN;

				if (OptionSet(s + 2, 'x'))
					GamePlayMode |= GP_NO_SAMPLES;

				if (OptionSet(s + 2, 'c'))
					GamePlayMode |= GP_COLLISION_CHECKING_OFF;

				if (OptionSet(s + 2, 'r'))
					GamePlayMode |= GP_SHOW_ROOMS;
				break;
			}
		}
	}
}
#endif

/**********************************************************/

} // End of namespace Clue

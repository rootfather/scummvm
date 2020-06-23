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
#include "clue/cdrom/cdrom.h"
#include "clue/sound/newsound.h"
#include "clue/present/interac.h"
#include "clue/data/dataappl.h"
#include "clue/planing/player.h"
#include "clue/scenes/scenes.h"
#include "clue/sound/fx.h"


namespace Clue {

void *StdBuffer1 = nullptr;

#define MAX_INTRO_TRACK 24
#define MAX_INTRO_ANIM 6

void tcDone() {
	static bool inprogress;

	if (!inprogress) {
		inprogress = true;

		plDone();
		g_clue->_sndMgr->sndDoneFX();        /* attention! */
		g_clue->_sndMgr->sndDone();
		g_clue->_sndMgr->removeAudio();
		dbDone();
		g_clue->_animMgr->closeAnimHandler();

		delete g_clue->_txtMgr;
		g_clue->_txtMgr = nullptr;

		gfxDone();

		if (g_clue->getFeatures() & ADGF_CD) {
			g_clue->_cdMgr->stop();
			g_clue->_cdMgr->unInstall();
		}

		if (StdBuffer1)
			TCFreeMem(StdBuffer1, STD_BUFFER1_SIZE);

		if (memGetAllocatedMem())
			DebugMsg(ERR_DEBUG, ERROR_MODULE_BASE, "Attention: dirty mem: %ld bytes!!!", memGetAllocatedMem());
		else
			DebugMsg(ERR_DEBUG, ERROR_MODULE_BASE, "all mem returned to pool");
	}
}

void orbyte(uint8 *ptr, uint8 data, uint8 dmask) {
	uint8 dmaskoff = ~dmask;

	if (0x80 & data)
		*ptr++ |= dmask;
	else
		*ptr++ &= dmaskoff;

	if (0x40 & data)
		*ptr++ |= dmask;
	else
		*ptr++ &= dmaskoff;

	if (0x20 & data)
		*ptr++ |= dmask;
	else
		*ptr++ &= dmaskoff;

	if (0x10 & data)
		*ptr++ |= dmask;
	else
		*ptr++ &= dmaskoff;

	if (0x08 & data)
		*ptr++ |= dmask;
	else
		*ptr++ &= dmaskoff;

	if (0x04 & data)
		*ptr++ |= dmask;
	else
		*ptr++ &= dmaskoff;

	if (0x02 & data)
		*ptr++ |= dmask;
	else
		*ptr++ &= dmaskoff;

	if (0x01 & data)
		*ptr |= dmask;
	else
		*ptr &= dmaskoff;
}

void ClueEngine::processIntroAnimation(uint8 *dp, uint8 *sp) {
	/* skip header */
	XMSOffset += 4;
	/* get size */
	uint32 size;
	memcpy(&size, XMSHandle + XMSOffset, 4);
	XMSOffset += 4;

	uint32 rsize = amg2Pc(size);
	uint8 *me = dp;

	sp = XMSHandle + XMSOffset;
	XMSOffset += rsize;

	while (memcmp(sp, "DLTA", 4) != 0)
		sp++;

	sp += 8;
	uint32 *lp = (uint32 *)sp;
	uint8 *st = sp;            /* start of DLTA block */

	uint32 offs[8];
	for (int i = 0; i < 8; i++)
		offs[i] = amg2Pc(lp[i]);

	uint8 *me2 = me;

	uint8 plane, planeMask;
	for (plane = 0, planeMask = 1; plane < 8; plane++, planeMask <<= 1) {
		if (offs[plane] != 0) {
			sp = st + offs[plane];
			uint8 *me1 = me2;

			for (int col = 0; col < 40; col++) {
				me = me1;
				uint8 op_cnt = *sp++;

				for (int i = 0; i < op_cnt; i++) {
					uint8 op = *sp++;

					if (op == 0) {
						uint8 cnt = *sp++;
						uint8 val = *sp++;

						for (int j = 0; j < cnt; j++) {
							orbyte(me, val, planeMask);
							me += SCREEN_WIDTH;
						}
					} else {
						if ((op & 0x80)) {
							uint8 cnt = op & 0x7f;
							for (int j = 0; j < cnt; j++) {
								uint8 val = *sp++;
								orbyte(me, val, planeMask);
								me += SCREEN_WIDTH;
							}
						} else {
							/* skip op */
							me += (SCREEN_WIDTH * op);
						}
					}

				}
				me1 += 8;   /* next column */
			}
		}
	}
}

uint32 ClueEngine::amg2Pc(uint32 s) {
	return ((s & 255) << 24) +
		(((s >> 8) & 255) << 16) +
		(((s >> 16) & 255) << 8) + (((s >> 24) & 255));
}
void ClueEngine::showIntro() {
	static const char *names[5] = {
		"an1_1.anm",
		"an2_4.anm",
		"an3_11.anm",
		"an4_11.anm",
		"an5_11.anm"
	};

	static const char *fname[MAX_INTRO_ANIM] = {
	"church.voc",
	"church.voc",
	"church.voc",
	"church.voc",
	"church.voc",
	"church.voc"
	};

	static const int sync[MAX_INTRO_ANIM * 2] = {
		0, 1,
		0, 5,
		1, 2,
		1, 27,
		1, 39,
		1, 53
	};

	static const int CDFrames[MAX_INTRO_TRACK * 6] = {
		/* Anim#, Frame#, (0..full frame; 1..partial frame), Frame Start, Frame End */

		2, 100, 0, 3, 0, 0,
		2, 143, 0, 4, 0, 0,
		3, 5, 0, 23, 0, 0,
		3, 74, 0, 8, 0, 0,
		3, 90, 0, 24, 0, 0,
		3, 118, 0, 9, 0, 0,
		3, 137, 0, 14, 0, 0,
		3, 177, 0, 19, 0, 0,
		3, 205, 0, 10, 0, 0,
		3, 222, 0, 15, 0, 0,
		3, 242, 0, 20, 0, 0,
		3, 260, 0, 16, 0, 0,
		3, 293, 0, 21, 0, 0,
		3, 327, 0, 11, 0, 0,
		3, 344, 0, 17, 0, 0,
		3, 375, 0, 5, 0, 0,
		3, 427, 1, 12, 0, 2 * 75,
		3, 445, 1, 12, 3 * 75, 5 * 75,
		3, 467, 1, 12, 8 * 75, 11 * 75,
		4, 5, 0, 18, 0, 0,
		4, 44, 0, 22, 0, 0,
		4, 66, 0, 6, 0, 0,
		4, 115, 0, 13, 0, 0,
		4, 153, 0, 7, 0, 0
	};

	static const int frames[5] = { 9, 67, 196, 500, 180 };
	static const int rate[5] = { 60, 17, 7, 7, 7 };

	XMSHandle = (uint8 *)malloc(818 * 1024);

	MemRastPort* A = new MemRastPort(SCREEN_WIDTH, SCREEN_HEIGHT);
	MemRastPort* B = new MemRastPort(SCREEN_WIDTH, SCREEN_HEIGHT);

	_GC *ScreenGC = new _GC(0, 0, 320, 200, 0, 255, nullptr);
	gfxSetColorRange(0, 255);

	uint8 colorTABLE[GFX_PALETTE_SIZE];
	memset(colorTABLE, 0, sizeof(colorTABLE));
	gfxSetRGBRange(colorTABLE, 0, 256);

	for (int anims = 0; anims < 5; anims++) {
		char pathName[DSK_PATH_MAX];

		if (getFeatures() & ADGF_CD)
			_cdMgr->stop();

		if (!dskBuildPathName(DISK_CHECK_FILE, INTRO_DIRECTORY, names[anims], pathName))
			continue;

		Common::Stream *fp = dskOpen(pathName, 0);
		if (fp) {
			XMSOffset = 0;

			uint32 head;
			dskRead_U32LE(fp, &head);
			uint32 size;
			dskRead_U32LE(fp, &size);
			uint32 rsize = amg2Pc(size);

			dskRead(fp, XMSHandle, rsize);
			dskClose(fp);

			/* skip header */
			XMSOffset += 4;

			uint8 *cp = XMSHandle + XMSOffset;

			/* skip header */
			XMSOffset += 4;
			/* get size */
			memcpy(&size, XMSHandle + XMSOffset, 4);
			XMSOffset += 4;

			rsize = amg2Pc(size);

			XMSOffset += rsize;

			gfxChangeColors(nullptr, 4, GFX_BLEND_UP, colorTABLE);
			gfxClearScreen();

			/* copy from file to A & B */
			gfxSetCMAP(cp);
			gfxILBMToRAW(cp, ScratchRP->_pixels, SCREEN_SIZE);
			A->gfxScratchToMem();
			B->gfxScratchToMem();

			bool endi = false;
			bool showA;
			int t;
			for (t = 0, showA = true; t < frames[anims]; t++, showA = !showA) {

				gfxScreenFreeze();

				if (showA)
					ScreenGC->gfxBlit(A, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, false);
				else
					ScreenGC->gfxBlit(B, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, false);

				if (t == 0) {
					gfxChangeColors(nullptr, 4, GFX_BLEND_UP, ScratchRP->_palette);
				}

				ScreenGC->gfxScreenThaw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

				inpSetWaitTicks(rate[anims]);
				int32 action = inpWaitFor(INP_TIME | INP_KEYBOARD | INP_BUTTON);
				if (action & (INP_KEYBOARD | INP_BUTTON)) {
					endi = true;
					goto endit;
				}

				if (showA) {
					processIntroAnimation(B->_pixels, cp);
				} else {
					processIntroAnimation(A->_pixels, cp);
				}

				for (int s = 0; s < MAX_INTRO_ANIM; s++) {
					if (sync[s * 2] == anims && sync[s * 2 + 1] - 1 == t) {
						_sndMgr->sndPrepareFX(fname[s]);
						_sndMgr->sndPlayFX();
					}
				}

				if (anims == 1 && t == 62)
					_sndMgr->sndPlaySound("title.bk", 0);

				if (getFeatures() & ADGF_CD) {
					for (int s = 0; s < MAX_INTRO_TRACK; s++) {
						if (CDFrames[s * 6] == anims && CDFrames[s * 6 + 1] == t) {
							_sndMgr->sndFading(16);

							_cdMgr->stop();
							if (CDFrames[s * 6 + 2] == 0)
								_cdMgr->playTrack(CDFrames[s * 6 + 3]);
							else
								_cdMgr->playSequence(CDFrames[s * 6 + 3], CDFrames[s * 6 + 4], CDFrames[s * 6 + 5]);
						}
					}
				}

				endi = false;
			}

		endit:
			gfxChangeColors(nullptr, 1, GFX_FADE_OUT, colorTABLE);

			if (endi) {
				goto endit2;
			}
		}
	}

endit2:
	gfxClearScreen();

	if (getFeatures() & ADGF_CD) {
		_cdMgr->stop();
		_sndMgr->sndFading(0);
	}

	delete A;
	delete B;

	free(XMSHandle);
	delete ScreenGC;
}
	
bool ClueEngine::tcInit() {
	_sndMgr->initAudio();

	StdBuffer1 = TCAllocMem(STD_BUFFER1_SIZE, true);

	if (!StdBuffer1)
		return false;
	 
	if (getFeatures() & ADGF_CD)
		_cdMgr->install();

	gfxInit();

	if (!(_gamePlayMode & GP_MODE_NO_SAMPLES))
		_sndMgr->sndInitFX();

	inpOpenAllInputDevs();
	inpMousePtrOff();

	showIntro();

	hscReset();

	gfxSetRGB(nullptr, 0, 0, 64, 48);
	gfxSetRGB(nullptr, 255, 255, 255, 255);    /* mouse */

	/* Start game. */
	inpMousePtrOn();

	_txtMgr->init();

	_animMgr->initAnimHandler();

	dbInit();
	plInit();

	gfxCollToMem(128, StdRP0InMem);    /* cache Menu in StdRP0InMem */
	gfxCollToMem(129, StdRP1InMem);    /* cache Bubbles in StdRP1InMem */

	CurrentBackground = BGD_LONDON;
	return true;
}

static void InitData() {
	char MainData [DSK_PATH_MAX];
	char BuildData[DSK_PATH_MAX];
	char MainRel  [DSK_PATH_MAX];
	char BuildRel [DSK_PATH_MAX];

	bool result = false;

	dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, MAIN_DATA_NAME GAME_DATA_EXT, MainData);
	dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, BUILD_DATA_NAME GAME_DATA_EXT, BuildData);

	dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, MAIN_DATA_NAME GAME_REL_EXT, MainRel);
	dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, BUILD_DATA_NAME GAME_REL_EXT, BuildRel);

	g_clue->_txtMgr->reset(OBJECTS_TXT);

	if (dbLoadAllObjects(MainData))
		if (dbLoadAllObjects(BuildData))
			if (LoadRelations(MainRel))
				if (LoadRelations(BuildRel)) {
					InitTaxiLocations();

					result = true;
				}

	if (!result)
		ErrorMsg(Disk_Defect, ERROR_MODULE_BASE, 1);
}

void ClueEngine::closeData() {
	RemRelations(0, 0);
	dbDeleteAllObjects(0, 0);
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
	gfxChangeColors(nullptr, 0, GFX_BLEND_UP, palette);
}

void ClueEngine::setFullEnviroment() {
	hasSetP(Person_Matt_Stuvysunt, Ability_Electronic, 251);
	hasSetP(Person_Matt_Stuvysunt, Ability_Locks, 210);
	hasSetP(Person_Matt_Stuvysunt, Ability_Surveillance, 180);
	hasSetP(Person_Matt_Stuvysunt, Ability_Locks, 180);

	hasSetP(Person_Marc_Smith, Ability_Autos, 255);
	hasSetP(Person_Mohammed_Abdula, Ability_Fight, 255);

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
	hasSet(Person_Matt_Stuvysunt, Tool_Lockpick);
	hasSet(Person_Matt_Stuvysunt, Tool_Hammer);
	hasSet(Person_Matt_Stuvysunt, Tool_Axe);
	hasSet(Person_Matt_Stuvysunt, Tool_Core_drill);
	hasSet(Person_Matt_Stuvysunt, Tool_Gloves);
	hasSet(Person_Matt_Stuvysunt, Tool_Shoes);
	hasSet(Person_Matt_Stuvysunt, Tool_Mask);
	hasSet(Person_Matt_Stuvysunt, Tool_Chloroform);
	hasSet(Person_Matt_Stuvysunt, Tool_Crowbar);
	hasSet(Person_Matt_Stuvysunt, Tool_Angle_grinder);
	hasSet(Person_Matt_Stuvysunt, Tool_Drilling_machine);
	hasSet(Person_Matt_Stuvysunt, Tool_Drilling_winch);
	hasSet(Person_Matt_Stuvysunt, Tool_Castle_engraver);
	hasSet(Person_Matt_Stuvysunt, Tool_Cutting_torch);
	hasSet(Person_Matt_Stuvysunt, Tool_Oxyhydrogen_torch);
	hasSet(Person_Matt_Stuvysunt, Tool_Stethoscope);
	hasSet(Person_Matt_Stuvysunt, Tool_Batterie);
	hasSet(Person_Matt_Stuvysunt, Tool_Power_generation);
	hasSet(Person_Matt_Stuvysunt, Tool_Electrical_set);
	hasSet(Person_Matt_Stuvysunt, Tool_Protective_suit);
	hasSet(Person_Matt_Stuvysunt, Tool_Dynamite);
	hasSet(Person_Matt_Stuvysunt, Tool_Electric_hammer);
	hasSet(Person_Matt_Stuvysunt, Tool_Glass_cutter);
	hasSet(Person_Matt_Stuvysunt, Tool_Rope_ladder);
	hasSet(Person_Matt_Stuvysunt, Tool_Radio_equipment);

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
	((BuildingNode *) dbGetObject(Building_Kiosk))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Tante_Emma_Laden);
	((BuildingNode *) dbGetObject(Building_Tante_Emma_Laden))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Jeweler);
	((BuildingNode *) dbGetObject(Building_Jeweler))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Old_Curiosity_Shop);
	((BuildingNode *) dbGetObject(Building_Old_Curiosity_Shop))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Pink_Villa);
	((BuildingNode*) dbGetObject(Building_Pink_Villa))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Retirement_Home);
	((BuildingNode *) dbGetObject(Building_Retirement_Home))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Karl_Marx_Grave);
	((BuildingNode *) dbGetObject(Building_Karl_Marx_Grave))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Villa);
	((BuildingNode *) dbGetObject(Building_Villa))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Kenwood_House);
	((BuildingNode *) dbGetObject(Building_Kenwood_House))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Ham_House);
	((BuildingNode *) dbGetObject(Building_Ham_House))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Osterly_Park_House);
	((BuildingNode *) dbGetObject(Building_Osterly_Park_House))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Chiswick_House);
	((BuildingNode *) dbGetObject(Building_Chiswick_House))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Suterbys);
	((BuildingNode *) dbGetObject(Building_Suterbys))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_British_Museum);
	((BuildingNode *) dbGetObject(Building_British_Museum))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Natural_Museum);
	((BuildingNode *) dbGetObject(Building_Natural_Museum))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_National_Galery);
	((BuildingNode *) dbGetObject(Building_National_Galery))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Victoria_and_Albert_Museum);
	((BuildingNode *) dbGetObject(Building_Victoria_and_Albert_Museum))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Bank_of_England);
	((BuildingNode *) dbGetObject(Building_Bank_of_England))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Tower_of_London);
	((BuildingNode *) dbGetObject(Building_Tower_of_London))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Starford_Barracks);
	((BuildingNode *) dbGetObject(Building_Starford_Barracks))->Exactlyness = 255;

	if (getFeatures() & GF_PROFIDISK) {
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
		((BuildingNode *) dbGetObject(Building_Baker_Street))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Madame_Tussaud);
		((BuildingNode *) dbGetObject(Building_Madame_Tussaud))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Westminster_Abbey);
		((BuildingNode *) dbGetObject(Building_Westminster_Abbey))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Downing_Street_10);
		((BuildingNode *) dbGetObject(Building_Downing_Street_10))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Mail_Train);
		((BuildingNode *) dbGetObject(Building_Mail_Train))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Tate_Gallery);
		((BuildingNode *) dbGetObject(Building_Tate_Gallery))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Buckingham_Palace);
		((BuildingNode *) dbGetObject(Building_Buckingham_Palace))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Bulstrode_Chemestry_Ltd);
		((BuildingNode *) dbGetObject(Building_Bulstrode_Chemestry_Ltd))->Exactlyness = 255;
	}
}

byte ClueEngine::startupMenu() {
	NewList<NewNode> *menu = _txtMgr->goKey(MENU_TXT, "STARTUP_MENU");
	Common::String line;
	byte ret = 0;

	ShowMenuBackground();

	if (getFeatures() & GF_PROFIDISK) {
		if (getFeatures() & ADGF_CD) {
			line = _txtMgr->getFirstLine(THECLOU_TXT, "BITTE_WARTEN_PC_CD_ROM_PROFI");
		} else {
			line = _txtMgr->getFirstLine(THECLOU_TXT, "BITTE_WARTEN_PC_PROFI");
		}
		PrintStatus(line);
	} else {
		if (getFeatures() & ADGF_CD) {
			line = _txtMgr->getFirstLine(THECLOU_TXT, "BITTE_WARTEM_PC_CD_ROM");
		} else {
			line = _txtMgr->getFirstLine(THECLOU_TXT, "BITTE_WARTEN_PC");
		}
		PrintStatus(line);
	}

	inpTurnFunctionKey(false);
	inpTurnESC(false);

	void (*dummyFct)(byte) = nullptr;
	uint32 activ = Menu(menu, 7, 0, dummyFct, 0);

	inpTurnESC(true);
	inpTurnFunctionKey(true);

	switch (activ) {
	case 0:
		InitData();
		ret = 1;
		break;

	case 1:
		_txtMgr->reset(OBJECTS_TXT);

		if (tcLoadTheClou()) {
			_film->_startScene = _sceneArgs._returnValue;
			ret = 1;
		}
		break;

	case 2:
		ret = 2;
		break;
	}

	menu->removeList();

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
					_gamePlayMode |= GP_MODE_DEMO | GP_MODE_DISABLE_STORY;

				if (OptionSet(s + 2, 's'))
					_gamePlayMode |= GP_MODE_DISABLE_STORY;

				if (OptionSet(s + 2, 'f'))
					_gamePlayMode |= GP_MODE_FULL_ENV;

				if (OptionSet(s + 2, 'l'))
					_gamePlayMode |= GP_MODE_LEVEL_DESIGN;

				if (OptionSet(s + 2, 'g'))
					_gamePlayMode |= GP_MODE_GUARD_DESIGN;

				if (OptionSet(s + 2, 'x'))
					_gamePlayMode |= GP_MODE_NO_SAMPLES;

				if (OptionSet(s + 2, 'c'))
					_gamePlayMode |= GP_MODE_DISABLE_COLLISION;

				if (OptionSet(s + 2, 'r'))
					_gamePlayMode |= GP_MODE_SHOW_ROOMS;
				break;
			}
		}
	}
}
#endif

/**********************************************************/

} // End of namespace Clue

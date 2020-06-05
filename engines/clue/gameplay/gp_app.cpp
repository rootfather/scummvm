/*
**  $Filename: gameplay/gp_app.c
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
#include "clue/sound/newsound.h"
#include "clue/clue.h"
#include "clue/present/interac.h"
#include "clue/data/dataappl.h"
#include "clue/organisa/organisa.h"
#include "clue/planing/player.h"
#include "clue/scenes/scenes.h"


namespace Clue {

// 2018-09-26 LucyG: in-game time was too fast
#define GP_TICKS_PER_MINUTE		5
#define GP_TICKS_PER_DAY		15

void tcAsTimeGoesBy(uint32 untilMinute) {
	untilMinute %= 1440;

	while (_film->getMinute() != untilMinute) {
		inpDelay(GP_TICKS_PER_MINUTE);

		addVTime(1);

		if (!(_film->getMinute() % 60))
			ShowTime(0);
	}
}

void tcAsDaysGoBy(uint32 day, uint32 stepSize) {
	while (_film->getDay() < day) {
		inpDelay(GP_TICKS_PER_DAY);

		uint32 add = g_clue->calcRandomNr(stepSize - stepSize / 30, stepSize + stepSize / 30);

		_film->setDay(_film->getDay() + add);

		tcRefreshLocationInTitle(_film->getLocation());
	}
}

void tcMattGoesTo(uint32 locNr) {
	NewTCEventNode *node = _film->_locationNames->getNthNode(locNr);

	_film->setLocation(locNr);
	tcRefreshLocationInTitle(locNr);
	ShowTime(0);

	gfxChangeColors(_lowerGc, 0, GFX_FADE_OUT, 0);
	g_clue->_animMgr->playAnim(node->_name.c_str(), (int16) 30000, GFX_NO_REFRESH | GFX_ONE_STEP | GFX_BLEND_UP);
}

void tcTheAlmighty(uint32 time) {
	tcMovePersons(2, time);
}

void tcMovePersons(uint32 personCount, uint32 time) {
	for (uint32 i = 0; i < personCount; i++) {
		uint32 persID;

		if (g_clue->getFeatures() & GF_PROFIDISK) {
			persID = g_clue->calcRandomNr(Person_Paul_O_Conner, Person_Pere_Ubu + 1);
		} else {
			persID = g_clue->calcRandomNr(Person_Paul_O_Conner, Person_Red_Stanson + 1);
		}

		if (livesIn(London_London_1, persID)) {
			likes_to_beAll(persID, 0, Object_Location);

			if (!ObjectList->isEmpty()) {
				uint32 count = ObjectList->getNrOfNodes();
				time = time * count / 1441;

				uint32 locID = ObjectList->getNthNode(time)->_nr;

				tcMoveAPerson(persID, locID);
			}
		}
	}
}

void tcMoveAPerson(uint32 persID, uint32 newLocID) {
	hasAll(persID, 0, Object_Location); /* wo is er denn ? */

	if (!ObjectList->isEmpty()) {
		for (dbObjectNode *n = ObjectList->getListHead(); n->_succ; n = (dbObjectNode *)n->_succ) {
			uint32 oldLocID = n->_nr;
			hasUnSet(persID, oldLocID);
			hasUnSet(oldLocID, persID);
		}
	}

	hasSet(persID, newLocID);   /* neuen Ort setzen ! */
	hasSet(newLocID, persID);
}

uint32 tcBurglary(uint32 buildingID) {
/* wird von 2 Stellen aufgerufen! (story_9)! */
	int32 ret;
	BuildingNode *b = (BuildingNode *)dbGetObject(buildingID);

	if (buildingID == Building_Seniorenheim)
		ret = plPlayer(buildingID, 200, tcKarateOpa);
	else
		ret = plPlayer(buildingID, 0, NULL);

	SetMenuTimeOutFunc(NULL);   /* sicher ist sicher... */

	if (ret) {          /* nur wenn Einbruch stattgefunden hat! */
		/* bei Grab nicht das Grab sondern "Highgate" aus der Taxiliste */
		/* entfernen */

		if (buildingID != Building_Grab_von_Karl_Marx)
			RemTaxiLocation(b->LocationNr);
		else
			RemTaxiLocation(((BuildingNode *)dbGetObject(Location_Highgate_Out))->LocationNr);

		hasUnSet(Person_Matt_Stuvysunt, buildingID);
	}

	CurrentBackground = BGD_LONDON;
	ShowMenuBackground();

	switch (ret) {
	case 0:
		refreshCurrScene();

		tcPlaySound();

		return 0;       /* Plan hat nicht funktioniert! */
	case FAHN_ESCAPED:
		g_clue->_sndMgr->sndPlaySound("ok.bk", 0);

		inpDelay(300);

		return SCENE_FAHNDUNG;  /*Plan hat funktioniert & Flucht gelungen */
	case FAHN_NOT_ESCAPED:
		g_clue->_sndMgr->sndPlaySound("failed.bk", 0);

		inpDelay(300);

		return SCENE_PRISON;    /* Plan hat funktioniert & Flucht nicht gelungen */
	default:
		assert(0);
		return 0;
	}
}

void tcRefreshLocationInTitle(uint32 locNr) {
	_menuGc->setPens(3, GFX_SAME_PEN, GFX_SAME_PEN);

	Common::String date = buildDate(_film->getDay());
	NewTCEventNode *node = _film->_locationNames->getNthNode(locNr);

	Common::String line = node->_name + " " + date;
	ShowMenuBackground();
	PrintStatus(line);
}

void stdInit() {
	Scene *sc = getCurrentScene();
	bool sameLocation = (sc->_locationNr == _film->getLocation());

	if (sc->_locationNr != (uint32) -1 && !sameLocation)
		_film->setLocation(sc->_locationNr);

	tcRefreshLocationInTitle(sc->_locationNr);

	NewTCEventNode *node = _film->_locationNames->getNthNode(sc->_locationNr);

	if (_refreshMode || !sameLocation)
		g_clue->_animMgr->playAnim(node->_name.c_str(), (int16) 30000, GFX_NO_REFRESH | GFX_ONE_STEP | GFX_BLEND_UP);

	ShowTime(0);        /* Zeit sollte nach der Anim gezeigt werden, sonst Probleme mit Diskversion */

	tcPersonGreetsMatt();
}

void tcPlaySound() {
	if (!(_gamePlayMode & GP_MODE_MUSIC_OFF)) {
		switch (getCurrentScene()->_eventNr) {
		case SCENE_PARKER:
			g_clue->_sndMgr->sndPlaySound("parker.bk", 0);
			break;
		case SCENE_MALOYA:
			g_clue->_sndMgr->sndPlaySound("maloya.bk", 0);
			break;
		case SCENE_POOLY:
			g_clue->_sndMgr->sndPlaySound("dealer.bk", 0);
			break;

		case SCENE_WALRUS:
			g_clue->_sndMgr->sndPlaySound("bar1.bk", 0);
			break;
		case SCENE_FAT_MANS:
			g_clue->_sndMgr->sndPlaySound("bar2.bk", 0);
			break;
		case SCENE_HOTEL_ROOM:
			g_clue->_sndMgr->sndPlaySound("hotel.bk", 0);
			break;
		case SCENE_GARAGE:
			g_clue->_sndMgr->sndPlaySound("cars.bk", 0);
			break;
		case SCENE_PARKING:
			g_clue->_sndMgr->sndPlaySound("cars.bk", 0);
			break;
		case SCENE_CARS_OFFICE:
			g_clue->_sndMgr->sndPlaySound("cars.bk", 0);
			break;

		case SCENE_POLICE:
		case SCENE_GLUDO_SAILOR:
		case SCENE_ARRESTED_MATT:
			g_clue->_sndMgr->sndPlaySound("gludo.bk", 0);
			break;

		case SCENE_TOOLS:
			g_clue->_sndMgr->sndPlaySound("shop.bk", 0);
			break;

		case SCENE_AT_SABIEN:
			g_clue->_sndMgr->sndPlaySound("sabien.bk", 0);
			break;

		case SCENE_THE_END:
		case SCENE_PRISON:
			g_clue->_sndMgr->sndPlaySound("end.bk", 0);
			break;

		case SCENE_SOUTH_WITHOUT:
		case SCENE_SOUTH_UNKNOWN:
		case SCENE_SOUTHHAMPTON:
			g_clue->_sndMgr->sndPlaySound("south.bk", 0);
			break;

		case SCENE_FAHNDUNG:
			g_clue->_sndMgr->sndPlaySound("fahndung.bk", 0);
			break;

		default:
			tcPlayStreetSound();
			break;
		}
	}
}

void tcPlayStreetSound() {
	if (!(_gamePlayMode & GP_MODE_MUSIC_OFF)) {
		if (g_clue->getFeatures() & GF_PROFIDISK) {
			switch (getCurrentScene()->_eventNr) {
			case SCENE_PROFI_21:
				g_clue->_sndMgr->sndPlaySound("snd21.bk", 0);
				return;
			case SCENE_PROFI_22:
				g_clue->_sndMgr->sndPlaySound("snd22.bk", 0);
				return;
			case SCENE_PROFI_23:
				g_clue->_sndMgr->sndPlaySound("snd23.bk", 0);
				return;
			case SCENE_PROFI_24:
				g_clue->_sndMgr->sndPlaySound("snd24.bk", 0);
				return;
			case SCENE_PROFI_25:
				g_clue->_sndMgr->sndPlaySound("snd25.bk", 0);
				return;
			case SCENE_PROFI_26:
				g_clue->_sndMgr->sndPlaySound("snd26.bk", 0);
				return;
			case SCENE_PROFI_27:
				g_clue->_sndMgr->sndPlaySound("snd27.bk", 0);
				return;
			case SCENE_PROFI_28:
				g_clue->_sndMgr->sndPlaySound("snd28.bk", 0);
				return;
			default:
				break;
			}
		}

		static byte counter = 0;
		bool noStreetMusic = false;

		if (strcmp(g_clue->_sndMgr->sndGetCurrSoundName(), "street1.bk") && strcmp(g_clue->_sndMgr->sndGetCurrSoundName(), "street2.bk") && strcmp(g_clue->_sndMgr->sndGetCurrSoundName(), "street3.bk"))
			noStreetMusic = true;

		if (!counter || noStreetMusic) {
			counter = g_clue->calcRandomNr(7, 13);

			switch (g_clue->calcRandomNr(0, 3)) {
			case 0:
				g_clue->_sndMgr->sndPlaySound("street1.bk", 0);
				break;
			case 1:
				g_clue->_sndMgr->sndPlaySound("street2.bk", 0);
				break;
			default:
				g_clue->_sndMgr->sndPlaySound("street3.bk", 0);
				break;
			}
		} else
			counter--;
	}
}

void ShowTime(uint32 delay) {
	Common::String time = buildTime(_film->getMinute());

	gfxShow(25, GFX_NO_REFRESH | GFX_OVERLAY, delay, -1, -1);

	_upperGc->setFont(bubbleFont);
	_upperGc->setPens(254, 252, GFX_SAME_PEN);
	gfxSetRect(280, 32);
	_upperGc->setMode(GFX_JAM_1);

	_upperGc->gfxPrint(time, 5, GFX_PRINT_CENTER | GFX_PRINT_SHADOW);
}

uint32 StdHandle(uint32 choice) {
	uint32 succ_eventnr = 0;
	Scene *scene = getCurrentScene();

	switch (choice) {
	case GP_CHOICE_GO:
		succ_eventnr = Go(scene->_nextEvents);

		if (succ_eventnr) {
			uint32 locNr = getScene(succ_eventnr)->_locationNr;

			if (locNr != (uint32) -1) {
				uint32 objNr = GetObjNrOfLocation(locNr);

				if (objNr) {
					LocationNode *loc = (LocationNode *)dbGetObject(objNr);
					if (_film->getMinute() < loc->OpenFromMinute || _film->getMinute() > loc->OpenToMinute) {
						ShowMenuBackground();

						Common::String line =  g_clue->_txtMgr->getFirstLine(THECLOU_TXT, "No_Entry");

						PrintStatus(line);
						inpWaitFor(INP_LBUTTONP);

						ShowMenuBackground();
						tcRefreshLocationInTitle(_film->getLocation());

						succ_eventnr = 0;
					} else
						g_clue->_animMgr->stopAnim();
				}
			}
		}
		break;
	case GP_CHOICE_BUSINESS_TALK:
		succ_eventnr = Talk();
		addVTime(7);
		ShowTime(0);
		break;
	case GP_CHOICE_LOOK:
		Look(getCurrentScene()->_locationNr);
		addVTime(1);
		ShowTime(0);
		break;
	case GP_CHOICE_INVESTIGATE:
		Investigate(_film->_locationNames->getNthNode(getCurrentScene()->_locationNr)->_name.c_str());
		ShowTime(0);
		break;
	case GP_CHOICE_MAKE_CALL:
		addVTime(4);
		succ_eventnr = tcTelefon();
		ShowTime(0);
		break;
	case GP_CHOICE_CALL_TAXI:
		if (g_clue->calcRandomNr(0, 10) == 1) {
			g_clue->_sndMgr->sndPrepareFX("taxi.voc");
			g_clue->_sndMgr->sndPlayFX();
		}

		succ_eventnr = getLocScene(8)->_eventNr; /* taxi */
		break;
	case GP_CHOICE_PLAN:
		if (!(_gamePlayMode & GP_MODE_DEMO)) {
			g_clue->_animMgr->stopAnim();

			_film->_storyIsRunning = GP_STORY_PLAN;

			hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Building);

			if (ObjectList->isEmpty()) {
				SetBubbleType(THINK_BUBBLE);
				Say(THECLOU_TXT, 0, MATT_PICTID, "NO_PLAN");
				ShowTime(0);
			} else {
				hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Car);

				if (ObjectList->isEmpty()) {
					SetBubbleType(THINK_BUBBLE);
					Say(THECLOU_TXT, 0, MATT_PICTID, "NO_CAR_FOR_PLAN");
					ShowTime(0);
				} else {
					uint32 building;

					gfxChangeColors(_lowerGc, 0, GFX_FADE_OUT, 0);

					if (_gamePlayMode & GP_MODE_NO_MUSIC_IN_PLANING)
						g_clue->_sndMgr->sndStopSound(0);

					if ((building = tcOrganisation())) {
						addVTime(27153);    /* etwas über 15 Tage ! */

						succ_eventnr = tcBurglary(building);
					} else {
						addVTime(443);  /* etwas über 7 Stunden */

						CurrentBackground = BGD_LONDON;
						ShowMenuBackground();

						gfxShow(173,
						        GFX_ONE_STEP | GFX_NO_REFRESH | GFX_BLEND_UP, 3,
						        -1, -1);
						tcRefreshLocationInTitle(_film->getLocation());

						succ_eventnr = 0;
					}
				}
			}

			_film->_storyIsRunning = GP_STORY_TOWN;
		}
		ShowTime(0);
		break;
	case GP_CHOICE_INFO:
		addVTime(1);
		Information();
		ShowTime(0);
		break;
	case GP_CHOICE_WAIT:
		tcWait();
		break;
	default:
		break;
	}

	return succ_eventnr;
}

void stdDone() {
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "Mainmenu");

	_sceneArgs._returnValue = 0;

	while (!_sceneArgs._returnValue) {
		if (tcPersonIsHere() && !(_sceneArgs._options & GP_CHOICE_BUSINESS_TALK))
			_sceneArgs._options |= (GP_CHOICE_BUSINESS_TALK & _film->getEnabledChoices());

		if ((g_clue->getFeatures() & GF_PROFIDISK) && getCurrentScene()->_eventNr == SCENE_PROFI_26) {
			EnvironmentNode *env = (EnvironmentNode *)dbGetObject(Environment_TheClou);

			if (env->PostzugDone)
				_sceneArgs._options &= ~GP_CHOICE_INVESTIGATE;
		}

		if (_sceneArgs._options) {
			inpTurnESC(false);
			inpTurnFunctionKey(true);

			byte activ = 0;
			activ = Menu(menu, _sceneArgs._options, activ, nullptr, 0);

			if (activ == (byte) -1) {
				ShowTheClouRequester();
				_sceneArgs._returnValue = ((PlayerNode *) dbGetObject(Player_Player_1))->CurrScene;
			} else if (activ != (byte)TXT_MENU_TIMEOUT) {
				uint32 choice = (uint32) 1 << activ;
				_sceneArgs._returnValue = StdHandle(choice);
			}

			inpTurnESC(true);
		} else
			ErrorMsg(Internal_Error, ERROR_MODULE_GAMEPLAY, 1);
	}

	g_clue->_animMgr->stopAnim();

	if (!_sceneArgs._overwritten)
		gfxChangeColors(_lowerGc, 5, GFX_FADE_OUT, nullptr);

	menu->removeList();
}


void InitTaxiLocations() {
	RemRelation(Relation_taxi); /* delete all relations! */
	AddRelation(Relation_taxi);

	if (_gamePlayMode & GP_MODE_DISABLE_STORY) {
		AddTaxiLocation(1); /* cars */
		AddTaxiLocation(12);    /* aunt */
		AddTaxiLocation(14);    /* jewels */
		AddTaxiLocation(27);    /* kenw */
		AddTaxiLocation(45);    /* bank */

		if (!(_gamePlayMode & GP_MODE_DEMO)) {
			AddTaxiLocation(0); /* holland */
			AddTaxiLocation(2); /* watling */
			AddTaxiLocation(10);    /* trafik */
			AddTaxiLocation(16);    /* anti */
			AddTaxiLocation(18);    /* pink */
			AddTaxiLocation(20);    /* senioren */
			AddTaxiLocation(22);    /* highgate */
			AddTaxiLocation(25);    /* villa */
			AddTaxiLocation(29);    /* ham */
			AddTaxiLocation(31);    /* osterly */
			AddTaxiLocation(33);    /* chiswick */
			AddTaxiLocation(35);    /* sotherbys */
			AddTaxiLocation(37);    /* brit */
			AddTaxiLocation(39);    /* natur */
			AddTaxiLocation(41);    /* national */
			AddTaxiLocation(43);    /* vict & alb */
			AddTaxiLocation(47);    /* tower */
			AddTaxiLocation(58);    /* vict */

			if (g_clue->getFeatures() & GF_PROFIDISK) {
				AddTaxiLocation(81);    /* bulstrode */
				AddTaxiLocation(79);    /* buck */
				AddTaxiLocation(77);    /* tate */
				AddTaxiLocation(75);    /* train */
				AddTaxiLocation(74);    /* downing */
				AddTaxiLocation(72);    /* abbey */
				AddTaxiLocation(70);    /* madame */
				AddTaxiLocation(68);    /* baker */
			}
		}
	} else {
		AddTaxiLocation(0); /* holland */
		AddTaxiLocation(58);    /* victoria */
	}
}

void LinkScenes() {
	getLocScene(8)->doneFct = DoneTaxi;
	getLocScene(51)->doneFct = DoneParking;
	getLocScene(50)->doneFct = DoneGarage;
	getLocScene(6)->doneFct = DoneTools;
	getLocScene(54)->doneFct = DoneDealer;
	getLocScene(53)->doneFct = DoneDealer;
	getLocScene(52)->doneFct = DoneDealer;

	getScene(SCENE_KASERNE_OUTSIDE)->setFunctions(stdInit, tcDoneKaserne);
	getScene(SCENE_HOTEL_ROOM)->setFunctions(stdInit, DoneHotelRoom);
	getScene(SCENE_CREDITS)->setFunctions(nullptr, tcDoneCredits);
	getScene(SCENE_FREIFAHRT)->setFunctions(nullptr, tcDoneFreeTicket);
	getScene(SCENE_ARRIVAL)->setFunctions(nullptr, tcDoneArrival);
	getScene(SCENE_HOTEL_1ST_TIME)->setFunctions(nullptr, tcDoneHotelReception);
	getScene(SCENE_DANNER)->setFunctions(nullptr, tcDoneDanner);
	getScene(SCENE_GLUDO_MONEY)->setFunctions(nullptr, tcDoneGludoMoney);
	getScene(SCENE_MAMI_CALLS)->setFunctions(nullptr, tcDoneMamiCalls);
	getScene(SCENE_FST_MEET_BRIGGS)->setFunctions(nullptr, tcDoneMeetBriggs);
	getScene(SCENE_FAHNDUNG)->setFunctions(tcInitFahndung, tcDoneFahndung);
	getScene(SCENE_GLUDO_SAILOR)->setFunctions(nullptr, tcDoneGludoAsSailor);
	getScene(SCENE_CALL_BRIGGS)->setFunctions(nullptr, tcDoneCallFromBriggs);
	getScene(SCENE_1ST_BURG)->setFunctions(nullptr, tcDone1stBurglary);
	getScene(SCENE_2ND_BURG)->setFunctions(nullptr, tcDone2ndBurglary);
	getScene(SCENE_3RD_BURG)->setFunctions(nullptr, tcDone3rdBurglary);
	getScene(SCENE_4TH_BURG)->setFunctions(nullptr, tcDone4thBurglary);
	getScene(SCENE_5TH_BURG)->setFunctions(nullptr, tcDone5thBurglary);
	getScene(SCENE_6TH_BURG)->setFunctions(nullptr, tcDone6thBurglary);
	getScene(SCENE_7TH_BURG)->setFunctions(nullptr, tcDone7thBurglary);
	getScene(SCENE_8TH_BURG)->setFunctions(nullptr, tcDone8thBurglary);
	getScene(SCENE_9TH_BURG)->setFunctions(nullptr, tcDone9thBurglary);
	getScene(SCENE_ARRESTED_MATT)->setFunctions(nullptr, tcDoneMattIsArrested);

	getScene(SCENE_POOLY_AFRAID)->setFunctions(nullptr, tcDoneDealerIsAfraid);
	getScene(SCENE_MALOYA_AFRAID)->setFunctions(nullptr, tcDoneDealerIsAfraid);
	getScene(SCENE_PARKER_AFRAID)->setFunctions(nullptr, tcDoneDealerIsAfraid);
	getScene(SCENE_RAID)->setFunctions(nullptr, tcDoneRaidInWalrus);
	getScene(SCENE_DART)->setFunctions(nullptr, tcDoneDartJager);
	getScene(SCENE_CALL_FROM_POOLY)->setFunctions(nullptr, tcDoneCallFromPooly);

	getScene(SCENE_GLUDO_BURNS)->setFunctions(nullptr, tcDoneGludoBurnsOffice);
	getScene(SCENE_MORNING)->setFunctions(nullptr, tcDoneBeautifullMorning);
	getScene(SCENE_VISITING)->setFunctions(nullptr, tcDoneVisitingSabien);
	getScene(SCENE_A_DREAM)->setFunctions(nullptr, tcDoneADream);
	getScene(SCENE_ROSENBLATT)->setFunctions(nullptr, tcMeetingRosenblatt);
	getScene(SCENE_BRIGGS_ANGRY)->setFunctions(nullptr, tcBriggsAngry);
	getScene(SCENE_SABIEN_WALRUS)->setFunctions(nullptr, tcSabienInWalrus);
	getScene(SCENE_SABIEN_DINNER)->setFunctions(nullptr, tcSabienDinner);
	getScene(SCENE_TOMBOLA)->setFunctions(nullptr, tcWalrusTombola);
	getScene(SCENE_PRESENT_HOTEL)->setFunctions(nullptr, tcPresentInHotel);
	getScene(SCENE_INFO_TOWER)->setFunctions(nullptr, tcPoliceInfoTower);
	getScene(SCENE_RAINY_EVENING)->setFunctions(nullptr, tcRainyEvening);
	getScene(SCENE_MEETING_AGAIN)->setFunctions(nullptr, tcDoneMeetingAgain);

	getScene(SCENE_SABIEN_CALL)->setFunctions(nullptr, tcDoneSabienCall);
	getScene(SCENE_BIRTHDAY)->setFunctions(nullptr, tcDoneBirthday);
	getScene(SCENE_WALK_WITH)->setFunctions(nullptr, tcWalkWithSabien);
	getScene(SCENE_AGENT)->setFunctions(nullptr, tcDoneAgent);
	getScene(SCENE_JAGUAR)->setFunctions(nullptr, tcDoneGoAndFetchJaguar);
	getScene(SCENE_THINK_OF)->setFunctions(nullptr, tcDoneThinkOfSabien);
	getScene(SCENE_TERROR)->setFunctions(nullptr, tcDoneTerror);
	getScene(SCENE_CONFESSING)->setFunctions(nullptr, tcDoneConfessingSabien);

	getScene(SCENE_PRISON)->setFunctions(tcInitPrison, tcDonePrison);

	getScene(SCENE_SOUTH_WITHOUT)->setFunctions(nullptr, tcDoneSouthhamptonWithoutSabien);
	getScene(SCENE_SOUTH_UNKNOWN)->setFunctions(nullptr, tcDoneSouthhamptonSabienUnknown);
	getScene(SCENE_SOUTHHAMPTON)->setFunctions(stdInit, tcDoneSouthhampton);

	getScene(SCENE_MISSED_DATE_0)->setFunctions(nullptr, tcDoneMissedDate);
	getScene(SCENE_MISSED_DATE_1)->setFunctions(nullptr, tcDoneMissedDate);

	getScene(SCENE_VILLA_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	getScene(SCENE_KENWO_INSIDE)->setFunctions(stdInit, DoneInsideHouse);

	getScene(SCENE_TANTE_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	getScene(SCENE_JUWEL_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	getScene(SCENE_ANTIQ_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	getScene(SCENE_TRANS_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	getScene(SCENE_SENIO_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	getScene(SCENE_KENWO_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	getScene(SCENE_HAMHO_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	getScene(SCENE_OSTER_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	getScene(SCENE_CHISW_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	getScene(SCENE_SOTHE_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	getScene(SCENE_BRITI_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	getScene(SCENE_NATUR_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	getScene(SCENE_NATIO_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	getScene(SCENE_VICTO_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	getScene(SCENE_BANKO_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	getScene(SCENE_TOWER_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	getScene(SCENE_KASERNE_INSIDE)->setFunctions(stdInit, DoneInsideHouse);

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		getScene(SCENE_PROFI_21_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
		getScene(SCENE_PROFI_22_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
		getScene(SCENE_PROFI_23_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
		getScene(SCENE_PROFI_24_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
		getScene(SCENE_PROFI_25_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
		/* getScene(SCENE_PROFI_26_INSIDE)->setFunctions(stdInit, DoneInsideHouse); */
		getScene(SCENE_PROFI_27_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
		getScene(SCENE_PROFI_28_INSIDE)->setFunctions(stdInit, DoneInsideHouse);
	}
}

void Scene::setFunctions(void (*init)(), void (*done)()) {
	initFct = init;
	doneFct = done;
}

bool tcPersonIsHere() {
	uint32 locNr = GetObjNrOfLocation(_film->getLocation());

	if (!locNr)
		return false;

	if (locNr == Location_Fat_Mans_Pub) {
		tcMoveAPerson(Person_Richard_Doil, locNr);
	} else if (locNr == Location_Cars_Vans_Office) {
		tcMoveAPerson(Person_Marc_Smith, locNr);
	} else if (locNr == Location_Walrus) {
		tcMoveAPerson(Person_Thomas_Smith, locNr);
	} else if (locNr == Location_Holland_Street) {
		tcMoveAPerson(Person_Frank_Maloya, locNr);
	} else if (locNr == Location_Policestation) {
		tcMoveAPerson(Person_John_Gludo, locNr);
		tcMoveAPerson(Person_Miles_Chickenwing, locNr);
	} else if (locNr == Location_Hotel) {
		tcMoveAPerson(Person_Ben_Riggley, locNr);
	}

	hasAll(locNr, 0, Object_Person);

	if (ObjectList->isEmpty())
		return false;

	return true;
}

void tcPersonGreetsMatt() {
	static uint32 upper = 4;

	if (g_clue->calcRandomNr(0, upper) == 1) { /* alle upper mal wird Matt gegrüßt ! */
		if (g_clue->calcRandomNr(0, 4) == 1)  /* alle 4 mal */
			upper += 2;     /* wahrscheinlichkeit wird kleiner ! */

		uint32 locNr = GetObjNrOfLocation(_film->getLocation());

		if (locNr) {
			hasAll(locNr, 0, Object_Person);

			if (!ObjectList->isEmpty()) {
				uint32 persNr = ObjectList->getListHead()->_nr;

				if (knows(Person_Matt_Stuvysunt, persNr)) {
					PersonNode *pers = (PersonNode *)dbGetObject(persNr);
					Say(BUSINESS_TXT, 0, pers->PictID, "HI_MATT");
				}
			}
		}
	}
	ShowTime(0);
}

void tcGetLastName(const char *Name, char *dest, uint32 maxLength) {
	const char *s;
	char lastName[TXT_KEY_LENGTH];

	for (s = Name; *s != '\0'; s++) {
		if (*s == ' ')
			break;
	}

	strcpy(lastName, s + 1);

	if (strlen(lastName) > maxLength)
		lastName[maxLength] = '\0';

	strcpy(dest, lastName);
}

void tcCutName(char *Name, byte Sign, uint32 maxLength) {
	Common::String wrkStr;

	for (uint32 i = 0; i < maxLength && Name[i]; ++i) {
		if (Name[i] == Sign)
			break;
		wrkStr += Name[i];
	}

	strcpy(Name, wrkStr.c_str());
}

Common::String tcCutName(Common::String Name, byte Sign, uint32 maxLength) {
	char Source[TXT_KEY_LENGTH];
	strcpy(Source, Name.c_str());

	uint32 j = MIN(Name.size(), maxLength);

	for (int32 i = j - 1; i >= 0; i--) {
		if (Source[i] == Sign)
			Source[i] = '\0';
	}

	Common::String retVal = Common::String(Source);
	return retVal;
}

void ShowMenuBackground() {
	if (CurrentBackground != BGD_CLEAR) /* MOD */
		gfxShow(CurrentBackground, GFX_ONE_STEP | GFX_NO_REFRESH, 0, -1, -1);
	else
		_menuGc->gfxClearArea();
}

} // End of namespace Clue

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
#include "clue/clue.h"

namespace Clue {

// 2018-09-26 LucyG: in-game time was too fast
#define GP_TICKS_PER_MINUTE		5
#define GP_TICKS_PER_DAY		15

void tcAsTimeGoesBy(uint32 untilMinute) {
	untilMinute %= 1440;

	while (_film->getMinute() != untilMinute) {
		inpDelay(GP_TICKS_PER_MINUTE);

		AddVTime(1);

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

	gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);
	PlayAnim(node->_name.c_str(), (int16) 30000, GFX_NO_REFRESH | GFX_ONE_STEP | GFX_BLEND_UP);
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
		RefreshCurrScene();

		tcPlaySound();

		return 0;       /* Plan hat nicht funktioniert! */
	case FAHN_ESCAPED:
		sndPlaySound("ok.bk", 0);

		inpDelay(300);

		return SCENE_FAHNDUNG;  /*Plan hat funktioniert & Flucht gelungen */
	case FAHN_NOT_ESCAPED:
		sndPlaySound("failed.bk", 0);

		inpDelay(300);

		return SCENE_PRISON;    /* Plan hat funktioniert & Flucht nicht gelungen */
	default:
		assert(0);
		return 0;
	}
}

void tcRefreshLocationInTitle(uint32 locNr) {
	gfxSetPens(m_gc, 3, GFX_SAME_PEN, GFX_SAME_PEN);

	Common::String date = BuildDate(_film->getDay());
	NewTCEventNode *node = _film->_locationNames->getNthNode(locNr);

	Common::String line = node->_name + " " + date;
	ShowMenuBackground();
	PrintStatus(line);
}

void StdInit() {
	struct Scene *sc = GetCurrentScene();
	bool sameLocation = (sc->_locationNr == _film->getLocation());

	if (sc->_locationNr != (uint32) -1 && !sameLocation)
		_film->setLocation(sc->_locationNr);

	tcRefreshLocationInTitle(sc->_locationNr);

	NewTCEventNode *node = _film->_locationNames->getNthNode(sc->_locationNr);

	if (RefreshMode || !sameLocation)
		PlayAnim(node->_name.c_str(), (int16) 30000, GFX_NO_REFRESH | GFX_ONE_STEP | GFX_BLEND_UP);

	ShowTime(0);        /* Zeit sollte nach der Anim gezeigt werden, sonst Probleme mit Diskversion */

	tcPersonGreetsMatt();
}

void tcPlaySound() {
	if (!(GamePlayMode & GP_MUSIC_OFF)) {
		switch (GetCurrentScene()->_eventNr) {
		case SCENE_PARKER:
			sndPlaySound("parker.bk", 0);
			break;
		case SCENE_MALOYA:
			sndPlaySound("maloya.bk", 0);
			break;
		case SCENE_POOLY:
			sndPlaySound("dealer.bk", 0);
			break;

		case SCENE_WALRUS:
			sndPlaySound("bar1.bk", 0);
			break;
		case SCENE_FAT_MANS:
			sndPlaySound("bar2.bk", 0);
			break;
		case SCENE_HOTEL_ROOM:
			sndPlaySound("hotel.bk", 0);
			break;
		case SCENE_GARAGE:
			sndPlaySound("cars.bk", 0);
			break;
		case SCENE_PARKING:
			sndPlaySound("cars.bk", 0);
			break;
		case SCENE_CARS_OFFICE:
			sndPlaySound("cars.bk", 0);
			break;

		case SCENE_POLICE:
		case SCENE_GLUDO_SAILOR:
		case SCENE_ARRESTED_MATT:
			sndPlaySound("gludo.bk", 0);
			break;

		case SCENE_TOOLS:
			sndPlaySound("shop.bk", 0);
			break;

		case SCENE_AT_SABIEN:
			sndPlaySound("sabien.bk", 0);
			break;

		case SCENE_THE_END:
		case SCENE_PRISON:
			sndPlaySound("end.bk", 0);
			break;

		case SCENE_SOUTH_WITHOUT:
		case SCENE_SOUTH_UNKNOWN:
		case SCENE_SOUTHHAMPTON:
			sndPlaySound("south.bk", 0);
			break;

		case SCENE_FAHNDUNG:
			sndPlaySound("fahndung.bk", 0);
			break;

		default:
			tcPlayStreetSound();
			break;
		}
	}
}

void tcPlayStreetSound() {
	if (!(GamePlayMode & GP_MUSIC_OFF)) {
		if (g_clue->getFeatures() & GF_PROFIDISK) {
			switch (GetCurrentScene()->_eventNr) {
			case SCENE_PROFI_21:
				sndPlaySound("snd21.bk", 0);
				return;
			case SCENE_PROFI_22:
				sndPlaySound("snd22.bk", 0);
				return;
			case SCENE_PROFI_23:
				sndPlaySound("snd23.bk", 0);
				return;
			case SCENE_PROFI_24:
				sndPlaySound("snd24.bk", 0);
				return;
			case SCENE_PROFI_25:
				sndPlaySound("snd25.bk", 0);
				return;
			case SCENE_PROFI_26:
				sndPlaySound("snd26.bk", 0);
				return;
			case SCENE_PROFI_27:
				sndPlaySound("snd27.bk", 0);
				return;
			case SCENE_PROFI_28:
				sndPlaySound("snd28.bk", 0);
				return;
			default:
				break;
			}
		}

		static byte counter = 0;
		bool noStreetMusic = false;

		if (strcmp(sndGetCurrSoundName(), "street1.bk") && strcmp(sndGetCurrSoundName(), "street2.bk") && strcmp(sndGetCurrSoundName(), "street3.bk"))
			noStreetMusic = true;

		if (!counter || noStreetMusic) {
			counter = g_clue->calcRandomNr(7, 13);

			switch (g_clue->calcRandomNr(0, 3)) {
			case 0:
				sndPlaySound("street1.bk", 0);
				break;
			case 1:
				sndPlaySound("street2.bk", 0);
				break;
			default:
				sndPlaySound("street3.bk", 0);
				break;
			}
		} else
			counter--;
	}
}

void ShowTime(uint32 delay) {
	Common::String time = BuildTime(_film->getMinute());

	gfxShow(25, GFX_NO_REFRESH | GFX_OVERLAY, delay, -1, -1);

	gfxSetFont(u_gc, bubbleFont);
	gfxSetPens(u_gc, 254, 252, GFX_SAME_PEN);
	gfxSetRect(280, 32);
	gfxSetDrMd(u_gc, GFX_JAM_1);

	gfxPrint(u_gc, time, 5, GFX_PRINT_CENTER | GFX_PRINT_SHADOW);
}

uint32 StdHandle(uint32 choice) {
	uint32 succ_eventnr = 0;
	struct Scene *scene = GetCurrentScene();

	switch (choice) {
	case GO:
		succ_eventnr = Go(scene->std_succ);

		if (succ_eventnr) {
			uint32 locNr = GetScene(succ_eventnr)->_locationNr;

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
						StopAnim();
				}
			}
		}
		break;
	case BUSINESS_TALK:
		succ_eventnr = Talk();
		AddVTime(7);
		ShowTime(0);
		break;
	case LOOK:
		Look(GetCurrentScene()->_locationNr);
		AddVTime(1);
		ShowTime(0);
		break;
	case INVESTIGATE:
		Investigate(_film->_locationNames->getNthNode(GetCurrentScene()->_locationNr)->_name.c_str());
		ShowTime(0);
		break;
	case MAKE_CALL:
		AddVTime(4);
		succ_eventnr = tcTelefon();
		ShowTime(0);
		break;
	case CALL_TAXI:
		if (g_clue->calcRandomNr(0, 10) == 1) {
			sndPrepareFX("taxi.voc");
			sndPlayFX();
		}

		succ_eventnr = GetLocScene(8)->_eventNr; /* taxi */
		break;
	case PLAN:
		if (!(GamePlayMode & GP_DEMO)) {
			StopAnim();

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

					gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);

					if (GamePlayMode & GP_NO_MUSIC_IN_PLANING)
						sndStopSound(0);

					if ((building = tcOrganisation())) {
						AddVTime(27153);    /* etwas über 15 Tage ! */

						succ_eventnr = tcBurglary(building);
					} else {
						AddVTime(443);  /* etwas über 7 Stunden */

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
	case INFO:
		AddVTime(1);
		Information();
		ShowTime(0);
		break;
	case WAIT:
		tcWait();
		break;
	default:
		break;
	}

	return succ_eventnr;
}

void StdDone() {
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "Mainmenu");

	_sceneArgs._returnValue = 0;

	while (!_sceneArgs._returnValue) {
		if (tcPersonIsHere())
			if (!(_sceneArgs._options & BUSINESS_TALK))
				_sceneArgs._options |= (BUSINESS_TALK & _film->_enabledChoices);

		if (g_clue->getFeatures() & GF_PROFIDISK) {
			if (GetCurrentScene()->_eventNr == SCENE_PROFI_26) {
				EnvironmentNode *env = (EnvironmentNode *)dbGetObject(Environment_TheClou);

				if (env->PostzugDone)
					_sceneArgs._options &= ~INVESTIGATE;
			}
		}

		if (_sceneArgs._options) {
			inpTurnESC(false);
			inpTurnFunctionKey(true);

			byte activ = 0;
			activ = Menu(menu, _sceneArgs._options, activ, nullptr, 0);

			if (activ == (byte) - 1) {
				ShowTheClouRequester(No_Error);
				_sceneArgs._returnValue = ((PlayerNode *) dbGetObject(Player_Player_1))->CurrScene;
			} else if (activ != (byte)TXT_MENU_TIMEOUT) {
				uint32 choice = (uint32) 1 << activ;
				_sceneArgs._returnValue = StdHandle(choice);
			}

			inpTurnESC(true);
		} else
			ErrorMsg(Internal_Error, ERROR_MODULE_GAMEPLAY, 1);
	}

	StopAnim();

	if (!_sceneArgs._overwritten)
		gfxChangeColors(l_gc, 5, GFX_FADE_OUT, nullptr);

	menu->removeList();
}


void InitTaxiLocations() {
	RemRelation(Relation_taxi); /* alle Relationen löschen! */
	AddRelation(Relation_taxi);

	if (GamePlayMode & GP_STORY_OFF) {
		AddTaxiLocation(1); /* cars */
		AddTaxiLocation(12);    /* aunt */
		AddTaxiLocation(14);    /* jewels */
		AddTaxiLocation(27);    /* kenw */
		AddTaxiLocation(45);    /* bank */

		if (!(GamePlayMode & GP_DEMO)) {
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
	GetLocScene(8)->Done = DoneTaxi;
	GetLocScene(51)->Done = DoneParking;
	GetLocScene(50)->Done = DoneGarage;
	GetLocScene(6)->Done = DoneTools;
	GetLocScene(54)->Done = DoneDealer;
	GetLocScene(53)->Done = DoneDealer;
	GetLocScene(52)->Done = DoneDealer;

	SetFunc(GetScene(SCENE_KASERNE_OUTSIDE), StdInit, tcDoneKaserne);
	SetFunc(GetScene(SCENE_HOTEL_ROOM), StdInit, DoneHotelRoom);
	SetFunc(GetScene(SCENE_CREDITS), nullptr, tcDoneCredits);
	SetFunc(GetScene(SCENE_FREIFAHRT), nullptr, tcDoneFreeTicket);
	SetFunc(GetScene(SCENE_ARRIVAL), nullptr, tcDoneArrival);
	SetFunc(GetScene(SCENE_HOTEL_1ST_TIME), nullptr, tcDoneHotelReception);
	SetFunc(GetScene(SCENE_DANNER), nullptr, tcDoneDanner);
	SetFunc(GetScene(SCENE_GLUDO_MONEY), nullptr, tcDoneGludoMoney);
	SetFunc(GetScene(SCENE_MAMI_CALLS), nullptr, tcDoneMamiCalls);
	SetFunc(GetScene(SCENE_FST_MEET_BRIGGS), nullptr, tcDoneMeetBriggs);
	SetFunc(GetScene(SCENE_FAHNDUNG), tcInitFahndung, tcDoneFahndung);
	SetFunc(GetScene(SCENE_GLUDO_SAILOR), nullptr, tcDoneGludoAsSailor);
	SetFunc(GetScene(SCENE_CALL_BRIGGS), nullptr, tcDoneCallFromBriggs);
	SetFunc(GetScene(SCENE_1ST_BURG), nullptr, tcDone1stBurglary);
	SetFunc(GetScene(SCENE_2ND_BURG), nullptr, tcDone2ndBurglary);
	SetFunc(GetScene(SCENE_3RD_BURG), nullptr, tcDone3rdBurglary);
	SetFunc(GetScene(SCENE_4TH_BURG), nullptr, tcDone4thBurglary);
	SetFunc(GetScene(SCENE_5TH_BURG), nullptr, tcDone5thBurglary);
	SetFunc(GetScene(SCENE_6TH_BURG), nullptr, tcDone6thBurglary);
	SetFunc(GetScene(SCENE_7TH_BURG), nullptr, tcDone7thBurglary);
	SetFunc(GetScene(SCENE_8TH_BURG), nullptr, tcDone8thBurglary);
	SetFunc(GetScene(SCENE_9TH_BURG), nullptr, tcDone9thBurglary);
	SetFunc(GetScene(SCENE_ARRESTED_MATT), nullptr, tcDoneMattIsArrested);

	SetFunc(GetScene(SCENE_POOLY_AFRAID), nullptr, tcDoneDealerIsAfraid);
	SetFunc(GetScene(SCENE_MALOYA_AFRAID), nullptr, tcDoneDealerIsAfraid);
	SetFunc(GetScene(SCENE_PARKER_AFRAID), nullptr, tcDoneDealerIsAfraid);
	SetFunc(GetScene(SCENE_RAID), nullptr, tcDoneRaidInWalrus);
	SetFunc(GetScene(SCENE_DART), nullptr, tcDoneDartJager);
	SetFunc(GetScene(SCENE_CALL_FROM_POOLY), nullptr, tcDoneCallFromPooly);

	SetFunc(GetScene(SCENE_GLUDO_BURNS), nullptr, tcDoneGludoBurnsOffice);
	SetFunc(GetScene(SCENE_MORNING), nullptr, tcDoneBeautifullMorning);
	SetFunc(GetScene(SCENE_VISITING), nullptr, tcDoneVisitingSabien);
	SetFunc(GetScene(SCENE_A_DREAM), nullptr, tcDoneADream);
	SetFunc(GetScene(SCENE_ROSENBLATT), nullptr, tcMeetingRosenblatt);
	SetFunc(GetScene(SCENE_BRIGGS_ANGRY), nullptr, tcBriggsAngry);
	SetFunc(GetScene(SCENE_SABIEN_WALRUS), nullptr, tcSabienInWalrus);
	SetFunc(GetScene(SCENE_SABIEN_DINNER), nullptr, tcSabienDinner);
	SetFunc(GetScene(SCENE_TOMBOLA), nullptr, tcWalrusTombola);
	SetFunc(GetScene(SCENE_PRESENT_HOTEL), nullptr, tcPresentInHotel);
	SetFunc(GetScene(SCENE_INFO_TOWER), nullptr, tcPoliceInfoTower);
	SetFunc(GetScene(SCENE_RAINY_EVENING), nullptr, tcRainyEvening);
	SetFunc(GetScene(SCENE_MEETING_AGAIN), nullptr, tcDoneMeetingAgain);

	SetFunc(GetScene(SCENE_SABIEN_CALL), nullptr, tcDoneSabienCall);
	SetFunc(GetScene(SCENE_BIRTHDAY), nullptr, tcDoneBirthday);
	SetFunc(GetScene(SCENE_WALK_WITH), nullptr, tcWalkWithSabien);
	SetFunc(GetScene(SCENE_AGENT), nullptr, tcDoneAgent);
	SetFunc(GetScene(SCENE_JAGUAR), nullptr, tcDoneGoAndFetchJaguar);
	SetFunc(GetScene(SCENE_THINK_OF), nullptr, tcDoneThinkOfSabien);
	SetFunc(GetScene(SCENE_TERROR), nullptr, tcDoneTerror);
	SetFunc(GetScene(SCENE_CONFESSING), nullptr, tcDoneConfessingSabien);

	SetFunc(GetScene(SCENE_PRISON), tcInitPrison, tcDonePrison);

	SetFunc(GetScene(SCENE_SOUTH_WITHOUT), nullptr, tcDoneSouthhamptonWithoutSabien);
	SetFunc(GetScene(SCENE_SOUTH_UNKNOWN), nullptr, tcDoneSouthhamptonSabienUnknown);
	SetFunc(GetScene(SCENE_SOUTHHAMPTON), StdInit, tcDoneSouthhampton);

	SetFunc(GetScene(SCENE_MISSED_DATE_0), nullptr, tcDoneMissedDate);
	SetFunc(GetScene(SCENE_MISSED_DATE_1), nullptr, tcDoneMissedDate);

	SetFunc(GetScene(SCENE_VILLA_INSIDE), StdInit, DoneInsideHouse);
	SetFunc(GetScene(SCENE_KENWO_INSIDE), StdInit, DoneInsideHouse);

	SetFunc(GetScene(SCENE_TANTE_INSIDE), StdInit, DoneInsideHouse);
	SetFunc(GetScene(SCENE_JUWEL_INSIDE), StdInit, DoneInsideHouse);
	SetFunc(GetScene(SCENE_ANTIQ_INSIDE), StdInit, DoneInsideHouse);
	SetFunc(GetScene(SCENE_TRANS_INSIDE), StdInit, DoneInsideHouse);
	SetFunc(GetScene(SCENE_SENIO_INSIDE), StdInit, DoneInsideHouse);
	SetFunc(GetScene(SCENE_KENWO_INSIDE), StdInit, DoneInsideHouse);
	SetFunc(GetScene(SCENE_HAMHO_INSIDE), StdInit, DoneInsideHouse);
	SetFunc(GetScene(SCENE_OSTER_INSIDE), StdInit, DoneInsideHouse);
	SetFunc(GetScene(SCENE_CHISW_INSIDE), StdInit, DoneInsideHouse);
	SetFunc(GetScene(SCENE_SOTHE_INSIDE), StdInit, DoneInsideHouse);
	SetFunc(GetScene(SCENE_BRITI_INSIDE), StdInit, DoneInsideHouse);
	SetFunc(GetScene(SCENE_NATUR_INSIDE), StdInit, DoneInsideHouse);
	SetFunc(GetScene(SCENE_NATIO_INSIDE), StdInit, DoneInsideHouse);
	SetFunc(GetScene(SCENE_VICTO_INSIDE), StdInit, DoneInsideHouse);
	SetFunc(GetScene(SCENE_BANKO_INSIDE), StdInit, DoneInsideHouse);
	SetFunc(GetScene(SCENE_TOWER_INSIDE), StdInit, DoneInsideHouse);
	SetFunc(GetScene(SCENE_KASERNE_INSIDE), StdInit, DoneInsideHouse);

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		SetFunc(GetScene(SCENE_PROFI_21_INSIDE), StdInit, DoneInsideHouse);
		SetFunc(GetScene(SCENE_PROFI_22_INSIDE), StdInit, DoneInsideHouse);
		SetFunc(GetScene(SCENE_PROFI_23_INSIDE), StdInit, DoneInsideHouse);
		SetFunc(GetScene(SCENE_PROFI_24_INSIDE), StdInit, DoneInsideHouse);
		SetFunc(GetScene(SCENE_PROFI_25_INSIDE), StdInit, DoneInsideHouse);
		/* SetFunc( GetScene(SCENE_PROFI_26_INSIDE),StdInit, DoneInsideHouse); */
		SetFunc(GetScene(SCENE_PROFI_27_INSIDE), StdInit, DoneInsideHouse);
		SetFunc(GetScene(SCENE_PROFI_28_INSIDE), StdInit, DoneInsideHouse);
	}
}

void SetFunc(struct Scene *sc, void (*init)(), void (*done)()) {
	sc->Init = init;
	sc->Done = done;
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
	char Source[TXT_KEY_LENGTH];
	strcpy(Source, Name);

	uint32 j = strlen(Source);
	if (j > maxLength)
		j = maxLength;

	for (int32 i = j - 1; i >= 0; i--) {
		if (Source[i] == Sign)
			Source[i] = '\0';
	}

	strcpy(Name, Source);
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
		gfxClearArea(m_gc);
}

} // End of namespace Clue

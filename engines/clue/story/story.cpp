/*
**  $Filename: story/story.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-02-94
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

#include "clue/story/story.h"
#include "clue/sound/newsound.h"
#include "clue/clue.h"
#include "clue/present/interac.h"
#include "clue/data/dataappl.h"
#include "clue/organisa/organisa.h"
#include "clue/planing/planer.h"
#include "clue/planing/player.h"
#include "clue/scenes/scenes.h"

namespace Clue {

byte tcDoLastBurglarySpot(uint32 ul_Time, uint32 ul_BuildingId);

static void tcSomebodyIsComing() {
	for (uint8 i = 0; i < 3; i++) {
		inpDelay(50);
		g_clue->_sndMgr->sndPrepareFX("klopfen.voc");
		g_clue->_sndMgr->sndPlayFX();
	}
}

static void tcSomebodyIsCalling() {
	for (uint32 i = 0; i < g_clue->calcRandomNr(1, 4); i++) {
		inpDelay(180);
		g_clue->_sndMgr->sndPrepareFX("ring.voc");
		g_clue->_sndMgr->sndPlayFX();
	}
}

byte tcKarateOpa(uint32 ul_ActionTime, uint32 ul_BuildingId) {
	livSetAllInvisible();
	lsSetViewPort(0, 0);    /* links, oben */

	gfxShow(217, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);

	return 1;
}

void tcDoneCredits() {
	PersonNode *ben = (PersonNode *)dbGetObject(Person_Ben_Riggley);

	tcSomebodyIsComing();

	Say(THECLOU_TXT, 0, ben->PictID, "A_LETTER_FOR_YOU");
	Say(THECLOU_TXT, 0, LETTER_PICTID, "SOME_CREDITS");
	Say(THECLOU_TXT, 0, MATT_PICTID, "SO_EIN_SCH");

	_sceneArgs._returnValue = SCENE_HOTEL_ROOM;
}


/* THE ARRIVAL (ST_30)
-----------------------------------------------------------------*/

void tcDoneArrival() {
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "ST_30_OLD");

	AddTaxiLocation(0);     /* holland */
	AddTaxiLocation(58);    /* victoria station */

	_sceneArgs._returnValue = SCENE_STATION;
}

/* HOTEL RECEPTION
-----------------------------------------------------------------*/

void tcDoneHotelReception() {
	PersonNode *rig = (PersonNode *) dbGetObject(Person_Ben_Riggley);
	EnvironmentNode *env = (EnvironmentNode *) dbGetObject(Environment_TheClou);

	knowsSet(Person_Matt_Stuvysunt, Person_Ben_Riggley);

	if (env->MattHasHotelRoom == 2) /* er hat es bereits ! */
		_sceneArgs._returnValue = SCENE_HOTEL;
	else {
		AddTaxiLocation(2); /* watling */
		AddTaxiLocation(1); /* cars */

		if (tcGetPlayerMoney >= tcCOSTS_FOR_HOTEL) {
			Say(STORY_0_TXT, 0, MATT_PICTID, "THE_KEY_PLEASE");
			Say(STORY_0_TXT, 0, rig->PictID, "I_LL_FETCH_THE_KEY");
			Say(STORY_0_TXT, 0, MATT_PICTID, "THANKS_FOR_KEY");

			env->MattHasHotelRoom = 2;

			tcSetPlayerMoney(tcGetPlayerMoney - tcCOSTS_FOR_HOTEL);

			_sceneArgs._returnValue = SCENE_HOTEL_ROOM;
		} else if (env->MattHasHotelRoom == 0) {   /* 1. mal da */
			byte choice = 2, evaluation = 0;

			while ((choice == 2) || (choice == 3)) {
				choice = Say(STORY_0_TXT, 0, MATT_PICTID, "HOTEL_MATT_1");

				switch (choice) {
				case 0:
					Say(STORY_0_TXT, 0, rig->PictID, "HOTEL_ANT_11");

					if (Say(STORY_0_TXT, 0, MATT_PICTID, "HOTEL_MATT_2") ==
					        1) {
						evaluation = 1;
						Say(STORY_0_TXT, 0, rig->PictID, "HOTEL_ANT_22");
					} else {
						Say(STORY_0_TXT, 0, rig->PictID, "HOTEL_ANT_21");
						Say(STORY_0_TXT, 0, MATT_PICTID, "HOTEL_MATT_3");
						Say(STORY_0_TXT, 0, rig->PictID, "HOTEL_ANT_31");
					}
					break;
				case 1:
					Say(STORY_0_TXT, 0, rig->PictID, "HOTEL_ANT_12");
					Say(STORY_0_TXT, 0, MATT_PICTID, "HOTEL_MATT_3");
					Say(STORY_0_TXT, 0, rig->PictID, "HOTEL_ANT_31");
					break;
				case 2:
					Say(STORY_0_TXT, 0, rig->PictID, "HOTEL_ANT_13");
					break;
				case 3:
					Say(STORY_0_TXT, 0, rig->PictID, "HOTEL_ANT_14");
					break;
				case 4:
					break;
				}
			}

			if (!evaluation) {
				_sceneArgs._returnValue = SCENE_HOLLAND_STR;
				env->MattHasHotelRoom = 1;
			} else {
				env->MattHasHotelRoom = 2;
				SetBubbleType(THINK_BUBBLE);
				Say(STORY_0_TXT, 0, MATT_PICTID, "MILLIONAIRE");
				Say(STORY_0_TXT, 0, MATT_PICTID, "THANKS_FOR_KEY");

				_sceneArgs._returnValue = SCENE_HOTEL_ROOM;
			}
		} else if (env->MattHasHotelRoom == 1) {
			Say(STORY_0_TXT, 0, rig->PictID, "YOU_HAVE_NO_MONEY");
			_sceneArgs._returnValue = SCENE_HOLLAND_STR;
		}
	}

	gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);
}

/* ZIMMER _ MAMI (ST_31)
-----------------------------------------------------------------*/

void tcDoneMamiCalls() {
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "ST_31_OLD_0");
	Say(STORY_0_TXT, 0, MATT_PICTID, "ST_31_MATT_0");
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "ST_31_OLD_1");

	_sceneArgs._returnValue = SCENE_HOTEL_ROOM;
}

/* CASH FROM GLUDO
-----------------------------------------------------------------*/

void tcDoneGludoMoney() {
	PersonNode *Gludo = (PersonNode *) dbGetObject(Person_John_Gludo);
	EnvironmentNode *env = (EnvironmentNode *)dbGetObject(Environment_TheClou);

	knowsSet(Person_Matt_Stuvysunt, Person_John_Gludo);

	byte choice = Say(STORY_0_TXT, 0, MATT_PICTID, "POLI_MATT_1");
	switch (choice) {
	case 0:
		Say(STORY_0_TXT, 0, Gludo->PictID, "POLI_ANT_11");
		Say(STORY_0_TXT, 0, MATT_PICTID, "POLI_MATT_2");
		Say(STORY_0_TXT, 0, Gludo->PictID, "POLI_ANT_21");
		Say(STORY_0_TXT, 0, MATT_PICTID, "POLI_MATT_5");
		Say(STORY_0_TXT, 0, Gludo->PictID, "POLI_ANT_51");
		tcSetPlayerMoney(tcGetPlayerMoney + tcCOSTS_FOR_HOTEL);
		break;
	case 1:
		Say(STORY_0_TXT, 0, Gludo->PictID, "POLI_ANT_12");
		break;
	case 2:
		Say(STORY_0_TXT, 0, Gludo->PictID, "POLI_ANT_13");
		break;
	case 3:
		Say(BUSINESS_TXT, 0, Gludo->PictID, "Bye");
		break;
	}

	SetBubbleType(THINK_BUBBLE);
	Say(STORY_0_TXT, 0, MATT_PICTID, "POLI_MATT_6");

	env->MattHasIdentityCard = 1;

	StopAnim();
	gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);

	_sceneArgs._returnValue = SCENE_WATLING;
}

/* CASH FROM DANNER
-----------------------------------------------------------------*/

void tcDoneDanner() {
	PersonNode *Jim = (PersonNode *) dbGetObject(Person_Jim_Danner);

	if (tcGetPlayerMoney < tcCOSTS_FOR_HOTEL) {
		knowsSet(Person_Matt_Stuvysunt, Person_Jim_Danner);
		Say(STORY_0_TXT, 0, Jim->PictID, "DANNER_1");

		byte choice = Say(STORY_0_TXT, 0, MATT_PICTID, "DANNER_2");
		switch (choice) {
		case 0:
			Say(STORY_0_TXT, 0, Jim->PictID, "DANNER_21");
			break;
		case 1:
			Say(STORY_0_TXT, 0, Jim->PictID, "DANNER_22");
			break;
		case 2:
			Say(STORY_0_TXT, 0, Jim->PictID, "DANNER_23");
			SetBubbleType(THINK_BUBBLE);
			break;
		}

		Say(STORY_0_TXT, 0, MATT_PICTID, "DANNER_3");
		tcAddPlayerMoney(20);

		livesInUnSet(London_London_1, Person_Jim_Danner);
		tcMoveAPerson(Person_Jim_Danner, Location_Nirvana);
	} else
		livesInSet(London_London_1, Person_Jim_Danner);

	_sceneArgs._returnValue = SCENE_CARS_VANS;
}

/* MEETING BRIGGS
-----------------------------------------------------------------*/

void tcDoneMeetBriggs() {
	PersonNode *Briggs = (PersonNode *) dbGetObject(Person_Herbert_Briggs);

#ifdef DEEP_DEBUG
	printf("tcDoneMeetBriggs!\n");
#endif
	knowsSet(Person_Matt_Stuvysunt, Person_Herbert_Briggs);

	Say(STORY_0_TXT, 0, Briggs->PictID, "BRIGGS_BRIGGS_1");
	Say(STORY_0_TXT, 0, MATT_PICTID, "BRIGGS_MATT_1");
	Say(STORY_0_TXT, 0, Briggs->PictID, "BRIGGS_BRIGGS_2");
	Say(STORY_0_TXT, 0, MATT_PICTID, "BRIGGS_MATT_2");
	Say(STORY_0_TXT, 0, Briggs->PictID, "BRIGGS_BRIGGS_3");

	byte choice = Say(STORY_0_TXT, 0, MATT_PICTID, "BRIGGS_MATT_3");
	if (choice == 0) {      /* angenommen ! */
		BuildingNode *bui = (BuildingNode *) dbGetObject(Building_Kiosk);

		hasSet(Person_Matt_Stuvysunt, Building_Kiosk);

		tcAddPlayerMoney(15);
		tcAddBuildExactlyness(bui, 255);
		tcAddBuildStrike(bui, 5);

		Say(STORY_0_TXT, 0, Briggs->PictID, "BRIGGS_BRIGGS_4");

		Present(Car_Fiat_Topolino_1940, "Car", InitCarPresent);
		Present(Building_Kiosk, "Building", InitBuildingPresent);

		Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "AFTER_MEETING_BRIGGS");

		hasSet(Person_Matt_Stuvysunt, Car_Fiat_Topolino_1940);

		_sceneArgs._returnValue = SCENE_FAT_MANS;
	} else {            /* nicht angenommen ! */
		PersonNode *james = (PersonNode *)dbGetObject(Person_Pater_James);

		Say(STORY_0_TXT, 0, Briggs->PictID, "BRIGGS_BRIGGS_5");
		gfxShow(170, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);
		Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "BRIGGS_MR_WHISKY");

		tcAsTimeGoesBy(_film->getMinute() + 793);

		StopAnim();
		gfxChangeColors(l_gc, 8, GFX_FADE_OUT, 0);

		tcMattGoesTo(60);   /* Kloster */

		Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "KLOSTER");
		Say(STORY_0_TXT, 0, james->PictID, "ABT");

		choice = Say(STORY_0_TXT, 0, MATT_PICTID, "HOLY_MATT");

		if (choice == 0) {  /* holy */
			g_clue->_sndMgr->sndPlaySound("end.bk", 0);

			Say(STORY_0_TXT, 0, 155, "THE_END_MONASTERY");  /* pict = holy matt ! */
			Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "THE_EDGE");

			_sceneArgs._returnValue = SCENE_NEW_GAME;
		} else {        /* evil */
			Say(STORY_0_TXT, 0, MATT_PICTID, "EVIL_MATT");
			Say(STORY_0_TXT, 0, james->PictID, "EVIL_MATT_ABT");
			Say(STORY_0_TXT, 0, MATT_PICTID, "EVIL_MATT_1");
			Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "EVIL_OLD_MATT");

			hasSetP(Person_Matt_Stuvysunt, Loot_Abbots_ring, tcVALUE_OF_RING_OF_PATER);

			addVTime(1440 + 525 - _film->getMinute());

			_sceneArgs._returnValue = SCENE_HOLLAND_STR;
		}

		StopAnim();
		gfxChangeColors(l_gc, 5, GFX_FADE_OUT, 0);
	}

	AddTaxiLocation(10);    /* trafik */
	AddTaxiLocation(18);    /* pink */
	AddTaxiLocation(20);    /* senioren */
	AddTaxiLocation(12);    /* aunt */

	if (g_clue->getFeatures() & GF_PROFIDISK)
		AddTaxiLocation(68);    /* baker street */
}

void tcDoneFreeTicket() {
	PersonNode *dan = (PersonNode *) dbGetObject(Person_Dan_Stanford);

	knowsSet(Person_Matt_Stuvysunt, Person_Dan_Stanford);

	Say(STORY_0_TXT, 0, 7, "AEHHH");
	Say(STORY_0_TXT, 0, dan->PictID, "FREE_TICKET");

	_sceneArgs._returnValue = getLocScene(8)->_eventNr;
}
#if 0
static void tcDoneAfterMeetingBriggs() {
	SetBubbleType(THINK_BUBBLE);
	Say(STORY_0_TXT, 0, MATT_PICTID, "AFTER_MEETING_BRIGGS");

	_sceneArgs._returnValue = SCENE_WATLING;
}
#endif

void tcDoneCallFromPooly() {
	knowsSet(Person_Matt_Stuvysunt, Person_Eric_Pooly);

	if (has(Person_Matt_Stuvysunt, Loot_Abbots_ring)) {
		tcSomebodyIsCalling();

		Say(STORY_0_TXT, 0, PHONE_PICTID, "A_CALL_FOR_YOU");
		Say(STORY_0_TXT, 0, PHONE_PICTID, "DEALER_0");

		byte choice = Say(STORY_0_TXT, 0, MATT_PICTID, "DEALER_MATT_1");
		if ((choice == 0) || (choice == 1))
			Say(STORY_0_TXT, 0, PHONE_PICTID, "DEALER_1");
		else
			Say(STORY_0_TXT, 0, PHONE_PICTID, "DEALER_2");
	}

	_sceneArgs._returnValue = SCENE_HOTEL_ROOM;
}

void tcInitPrison() {
	/* es ist wichtig, daß die Location hier gesetzt wird */
	/* da sonst diverse Szenen in die Gefängnisszene      */
	/* hineinplatzen können */

	_film->setLocation(64);        /* auf ins Gefängnis */
}

void tcDonePrison() {
/*
 * wird schmutzigerweise von tcDoneGludoAsSailor,
 * tcLastBurglary
 * direkt angesprungen
 */
	_film->setDay(719792);        /* 13.01.1972? */

	g_clue->_sndMgr->sndPlaySound("end.bk", 0);  /* dont change it! (-> story_9) */
	tcMattGoesTo(7);

	gfxShow(169, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "THE_END_PRISON");
	StopAnim();

	tcMattGoesTo(60);
	inpDelay(190);
	Say(STORY_1_TXT, 0, 155, "THE_END_MONASTERY");  /* holy matt */

	StopAnim();
	gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);

	_sceneArgs._returnValue = SCENE_NEW_GAME;
}

bool tcIsDeadlock() {
	bool deadlock = false;
	CompleteLootNode *comp = (CompleteLootNode *)dbGetObject(CompleteLoot_LastLoot);

	hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Car);

	if (ObjectList->isEmpty()) {
		int32 money = tcGetPlayerMoney;
		bool enough = false;

		/* jetzt zum Geld noch die vorhandene Beute addieren */
		tcMakeLootList(Person_Matt_Stuvysunt, Relation_has)->removeList();

		int32 total = comp->Bild + comp->Gold + comp->Geld + comp->Juwelen +
		        comp->Statue + comp->Kuriositaet + comp->HistKunst +
		        comp->GebrauchsArt + comp->Vase + comp->Delikates;

		money += total / 15;    /* im schlimmsten Fall bleibt Matt in */
		/* etwa nur ein Fünfzehnten           */

		hasAll(Person_Marc_Smith, OLF_NORMAL, Object_Car);

		/* get cheapest car! */
		for (dbObjectNode *n = ObjectList->getListHead(); n->_succ; n = (dbObjectNode*)n->_succ) {
			CarNode *car = (CarNode *)n;

			if (tcGetCarPrice(car) < money)
				enough = true;
		}

		if (!enough) {
			Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "DEADLOCK");

			g_clue->_sndMgr->sndPlaySound("end.bk", 0);

			tcMattGoesTo(60);
			inpDelay(190);
			Say(STORY_1_TXT, 0, 155, "THE_END_MONASTERY");  /* holy matt */

			StopAnim();
			gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);

			deadlock = true;
		}
	}

	return deadlock;
}

/***********************************************************************
 ***
 *** 1st BURGLARY
 ***
 ***********************************************************************/


void tcDone1stBurglary() {
	AddTaxiLocation(22);    /* highgate */
	AddTaxiLocation(16);    /* anti */

	if (g_clue->getFeatures() & GF_PROFIDISK)
		AddTaxiLocation(75);    /* train */

	hasSet(Person_Marc_Smith, Car_Morris_Minor_1950);
	hasSet(Person_Marc_Smith, Car_Fiat_Topolino_1942);
	hasSet(Person_Marc_Smith, Car_Jeep_1945);
	hasSet(Person_Marc_Smith, Car_Pontiac_Streamliner_1946);

	if (g_clue->getFeatures() & GF_PROFIDISK)
		hasSet(Person_Marc_Smith, Car_Ford_Model_T__1926);

	hasSet(Person_Mary_Bolton, Tool_Lockpick);
	hasSet(Person_Mary_Bolton, Tool_Drilling_machine);
	hasSet(Person_Mary_Bolton, Tool_Rope_ladder);
	hasSet(Person_Mary_Bolton, Tool_Castle_engraver);
	hasSet(Person_Mary_Bolton, Tool_Angle_grinder);
	hasSet(Person_Mary_Bolton, Tool_Protective_suit);

	livesInSet(London_London_1, Person_Marc_Smith);
	livesInSet(London_London_1, Person_Robert_Bull);
	livesInSet(London_London_1, Person_Thomas_Groul);
	livesInSet(London_London_1, Person_Lucas_Grull);
	livesInSet(London_London_1, Person_Peter_Brook);
	livesInSet(London_London_1, Person_Luthmilla_Nervesaw);

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		livesInSet(London_London_1, Person_Tom_Cooler);
		livesInSet(London_London_1, Person_Tina_Olavson);
	}

	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "LOBHUDEL");

	getScene(SCENE_FAHNDUNG)->_occurrence = 0;    /* so that Burglary 2 doesn't happen */

	_sceneArgs._returnValue = getLocScene(_film->getLocation())->_eventNr;
}

void tcDoneGludoAsSailor() {
	knowsSet(Person_Matt_Stuvysunt, Person_John_Gludo);

	g_clue->_sndMgr->sndPlaySound("gludo.bk", 0);

	Say(STORY_0_TXT, 0, (uint16) FACE_GLUDO_SAILOR, "SAILOR_GLUDO_0");
	Say(STORY_0_TXT, 0, MATT_PICTID, "SAILOR_MATT_0");
	Say(STORY_0_TXT, 0, (uint16) FACE_GLUDO_SAILOR, "SAILOR_GLUDO_1");

	if (Say(STORY_0_TXT, 0, MATT_PICTID, "SAILOR_MATT_1")) {
		Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "SAILOR_OLD_MATT_0");

		g_clue->_sndMgr->sndPlaySound("street1.bk", 0);
		_sceneArgs._returnValue = SCENE_HOLLAND_STR;
	} else {
		Say(STORY_0_TXT, 0, (uint16) FACE_GLUDO_SAILOR, "SAILOR_GLUDO_2");
		StopAnim();

		tcDonePrison();
	}
}

void tcDoneCallFromBriggs() {
	tcAsTimeGoesBy(_film->getMinute() + 130);

	tcSomebodyIsCalling();

	Say(STORY_0_TXT, 0, PHONE_PICTID, "A_CALL_FOR_YOU");
	Say(STORY_0_TXT, 0, PHONE_PICTID, "CALL_BRIGGS_0");
	Say(STORY_0_TXT, 0, MATT_PICTID, "CALL_MATT_0");
	Say(STORY_0_TXT, 0, PHONE_PICTID, "CALL_BRIGGS_1");

	_sceneArgs._returnValue = SCENE_HOTEL_ROOM;
}

/***********************************************************************
 ***
 *** 2nd BURGLARY
 ***
 ***********************************************************************/

void tcDone2ndBurglary() {
	AddTaxiLocation(14);    /* jewels */

	if (g_clue->getFeatures() & GF_PROFIDISK)
		AddTaxiLocation(72);    /* abbey */

	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "FAHNDUNG");

	hasSet(Person_Marc_Smith, Car_Pontiac_Streamliner_1944);
	hasSet(Person_Marc_Smith, Car_Standard_Vanguard_1953);

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		hasSet(Person_Marc_Smith, Car_Rover_75_1950);
		hasSet(Person_Marc_Smith, Car_Bentley_Continental_Typ_R_1952);
	}

	hasSet(Person_Mary_Bolton, Tool_Radio_equipment);
	hasSet(Person_Mary_Bolton, Tool_Glass_cutter);
	hasSet(Person_Mary_Bolton, Tool_Drilling_winch);
	hasSet(Person_Mary_Bolton, Tool_Electrical_set);

	livesInSet(London_London_1, Person_Margrete_Briggs);
	livesInSet(London_London_1, Person_Paul_O_Conner);
	livesInSet(London_London_1, Person_Tony_Allen);


	getScene(SCENE_FAHNDUNG)->_occurrence = 0;    /* So that burglary 2 doesn't happen */

	_sceneArgs._returnValue = getLocScene(_film->getLocation())->_eventNr;
}

/* wird von DoneHotel aufgerufen */
void tcCheckForBones() {
	PersonNode *luthm = (PersonNode *)dbGetObject(Person_Luthmilla_Nervesaw);

	if (has(Person_Matt_Stuvysunt, Loot_Relics)) {
		if (knows(Person_Matt_Stuvysunt, Person_Luthmilla_Nervesaw)) {
			PlayerNode *player = (PlayerNode *)dbGetObject(Player_Player_1);

			tcSomebodyIsComing();

			Say(STORY_0_TXT, 0, luthm->PictID, "KARL_MARX");

			tcAddPlayerMoney(20 * player->MattsPart);

			player->StolenMoney += 2000;
			player->MyStolenMoney += (20 * player->MattsPart);

			livesInUnSet(London_London_1, Person_Luthmilla_Nervesaw);
			tcMoveAPerson(Person_Luthmilla_Nervesaw, Location_Nirvana);
			hasUnSet(Person_Matt_Stuvysunt, Loot_Relics);
		}
	}
}


/***********************************************************************
 ***
 *** 3rd BURGLARY
 ***
 ***********************************************************************/

void tcDone3rdBurglary() {
	AddTaxiLocation(35);    /* sotherbys */
	AddTaxiLocation(33);    /* chiswick */

	if (g_clue->getFeatures() & GF_PROFIDISK)
		AddTaxiLocation(74);    /* downing */

	knowsSet(Person_Matt_Stuvysunt, Person_John_Gludo);

	hasSet(Person_Mary_Bolton, Tool_Cutting_torch);
	hasSet(Person_Mary_Bolton, Tool_Stethoscope);
	hasSet(Person_Mary_Bolton, Tool_Power_generation);
	hasSet(Person_Mary_Bolton, Tool_Mask);

	livesInSet(London_London_1, Person_Miguel_Garcia);
	livesInSet(London_London_1, Person_John_O_Keef);
	livesInSet(London_London_1, Person_Samuel_Rosenblatt);

	gfxShow(166, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);

	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "READ_TIMES_0");

	g_clue->_sndMgr->sndPlaySound("gludo.bk", 0);

	Say(STORY_0_TXT, 0, FACE_GLUDO_MAGIC, "READ_TIMES_GLUDO");
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "READ_TIMES_1");

	g_clue->_sndMgr->sndPlaySound("street1.bk", 0);

	gfxShow(150, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);

	getScene(SCENE_FAHNDUNG)->_occurrence = 0;    /* so that burglary 2 doesn't happen */

	_sceneArgs._returnValue = getLocScene(_film->getLocation())->_eventNr;
}

void tcCheckForDowning() {
	if (has(Person_Matt_Stuvysunt, Loot_Deed)) {
		BuildingNode *bui = (BuildingNode *) dbGetObject(Building_Buckingham_Palace);

		hasSet(Person_Matt_Stuvysunt, Building_Buckingham_Palace);

		tcAddBuildExactlyness(bui, 255);
		tcAddBuildStrike(bui, 5);

		SetBubbleType(THINK_BUBBLE);
		Say(INVESTIGATIONS_TXT, 0, OLD_MATT_PICTID, "BuckinghamPlansFound");

		Present(Building_Buckingham_Palace, "Building", InitBuildingPresent);
		hasUnSet(Person_Matt_Stuvysunt, Loot_Deed);
	}
}


/***********************************************************************
 ***
 *** 4th BURGLARY
 ***
 ***********************************************************************/

void tcDone4thBurglary() {
	PersonNode *Gludo = (PersonNode *) dbGetObject(Person_John_Gludo);

	AddTaxiLocation(31);    /* osterly */
	AddTaxiLocation(29);    /* ham */

	if (g_clue->getFeatures() & GF_PROFIDISK)
		AddTaxiLocation(70);    /* madame */

	tcSomebodyIsComing();

	g_clue->_sndMgr->sndPlaySound("gludo.bk", 0);
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "ARREST_OLD_MATT_0");
	Say(STORY_0_TXT, 0, Gludo->PictID, "ARREST_GLUDO_0");
	Say(STORY_0_TXT, 0, MATT_PICTID, "ARREST_MATT_0");
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "ARREST_OLD_MATT_1");
	Say(STORY_0_TXT, 0, Gludo->PictID, "ARREST_GLUDO_1");

	hasSet(Person_Marc_Smith, Car_Standard_Vanguard_1950);
	hasSet(Person_Marc_Smith, Car_Cadillac_Club_1952);

	if (g_clue->getFeatures() & GF_PROFIDISK)
		hasSet(Person_Marc_Smith, Car_Fiat_634_N_1943);

	hasSet(Person_Mary_Bolton, Tool_Dynamite);
	hasSet(Person_Mary_Bolton, Tool_Core_drill);
	hasSet(Person_Mary_Bolton, Tool_Oxyhydrogen_torch);
	hasSet(Person_Mary_Bolton, Tool_Chloroform);

	livesInSet(London_London_1, Person_Garry_Stevenson);
	livesInSet(London_London_1, Person_Jiri_Poulin);
	livesInSet(London_London_1, Person_Prof_Emil_Schmitt);

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		livesInSet(London_London_1, Person_Melanie_Morgan);
		livesInSet(London_London_1, Person_Sid_Palmer);
	}

	gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);

	getScene(SCENE_FAHNDUNG)->_occurrence = 0;    /* So thqt burglary 2 doesn't happen */
	_sceneArgs._returnValue = getLocScene(7)->_eventNr;    /* Police */
}

void tcDoneMattIsArrested() {
	StopAnim();

	gfxShow(169, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1); /* prison */

	addVTime(1439);

	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "IN_PRISON_OLD_0");
	gfxShow(161, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1); /* polizei */

	_sceneArgs._returnValue = SCENE_POLICE;
}

/***********************************************************************
 ***
 *** 5th BURGLARY
 ***
 ***********************************************************************/

void tcDone5thBurglary() {
	AddTaxiLocation(27);    /* kenw */
	AddTaxiLocation(39);    /* natural museum */

	if (g_clue->getFeatures() & GF_PROFIDISK)
		AddTaxiLocation(77);    /* tate */

	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_0");

	hasSet(Person_Marc_Smith, Car_Standard_Vanguard_1951);

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		hasSet(Person_Marc_Smith, Car_Rover_75_1952);
		hasSet(Person_Marc_Smith, Car_Bentley_Continental_Typ_R_1953);
	}

	hasSet(Person_Mary_Bolton, Tool_Shoes);
	hasSet(Person_Mary_Bolton, Tool_Electric_hammer);

	livesInSet(London_London_1, Person_Thomas_Smith);
	livesInSet(London_London_1, Person_Albert_Liet);
	livesInSet(London_London_1, Person_Frank_Meier);
	livesInSet(London_London_1, Person_Mike_Kahn);
	livesInSet(London_London_1, Person_Mark_Hart);
	livesInSet(London_London_1, Person_Frank_De_Silva);
	livesInSet(London_London_1, Person_Neil_Grey);
	livesInSet(London_London_1, Person_Serge_Fontane);
	livesInSet(London_London_1, Person_Mike_Seeger);
	livesInSet(London_London_1, Person_Mathew_Black);

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		livesInSet(London_London_1, Person_Prof_Marcus_Green);
		livesInSet(London_London_1, Person_Pere_Ubu);
	}

	getScene(SCENE_FAHNDUNG)->_occurrence = 0;    /* So that burglary 2 doesn't happen */
	_sceneArgs._returnValue = getLocScene(_film->getLocation())->_eventNr;
}

void tcDoneDealerIsAfraid() {
	uint32 persID;

	switch (_film->getLocation()) {
	case 52:
		persID = Person_Helen_Parker;
		_sceneArgs._returnValue = SCENE_WATLING;
		break;
	case 53:
		persID = Person_Frank_Maloya;
		_sceneArgs._returnValue = SCENE_HOLLAND_STR;
		break;
	case 54:
		persID = Person_Eric_Pooly;
		_sceneArgs._returnValue = SCENE_HOLLAND_STR;
		break;
	default:
		return;
	}

	knowsSet(Person_Matt_Stuvysunt, persID);

	PersonNode *pers = (PersonNode *) dbGetObject(persID);
	Say(STORY_0_TXT, 0, pers->PictID, "DEALER_IS_AFRAID");

	gfxChangeColors(l_gc, 5, GFX_FADE_OUT, 0);
}

void tcDoneRaidInWalrus() {
	PersonNode *Red = (PersonNode *) dbGetObject(Person_Red_Stanson);
	EnvironmentNode *Env = (EnvironmentNode *) dbGetObject(Environment_TheClou);

	knowsSet(Person_Matt_Stuvysunt, Person_Red_Stanson);
	g_clue->_sndMgr->sndPlaySound("gludo.bk", 0);

	Say(STORY_0_TXT, 0, Red->PictID, "RAID_POLICE_0");
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "RAID_OLD_MATT_0");

	Say(STORY_0_TXT, 0, MATT_PICTID, "RAID_MATT_0");
	Say(STORY_0_TXT, 0, Red->PictID, "RAID_POLICE_1");

	if (Env->MattHasIdentityCard) {
		Say(STORY_0_TXT, 0, MATT_PICTID, "RAID_MATT_1");
		Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "RAID_OLD_MATT_1");

		_sceneArgs._returnValue = SCENE_HOTEL_ROOM;
	} else {
		Say(STORY_0_TXT, 0, MATT_PICTID, "RAID_MATT_2");
		Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "RAID_OLD_MATT_2");

		_sceneArgs._returnValue = SCENE_POLICE;
	}

	StopAnim();
	gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);
}

void tcDoneDartJager() {
	PersonNode *Grull = (PersonNode *) dbGetObject(Person_Lucas_Grull);
	EnvironmentNode *Env = (EnvironmentNode *) dbGetObject(Environment_TheClou);

	if (!Env->MattHasIdentityCard) {
		StopAnim();

		gfxShow(169, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1); /* Knast */
		knowsSet(Person_Matt_Stuvysunt, Person_Lucas_Grull);
		Say(STORY_0_TXT, 0, Grull->PictID, "DART_GRULL_0");

		byte choice = Say(STORY_0_TXT, 0, MATT_PICTID, "DART_MATT_1");
		if (choice == 0) {
			Say(STORY_0_TXT, 0, Grull->PictID, "DART_GRULL_1");

			g_clue->_sndMgr->sndPrepareFX("darth.voc");
			g_clue->_sndMgr->sndPlayFX();

			Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "DART_JAEGER_0");

			gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);
			gfxShow(221, GFX_NO_REFRESH | GFX_ONE_STEP | GFX_BLEND_UP, 0, -1, -1);

			g_clue->_sndMgr->sndPrepareFX("darth.voc");
			g_clue->_sndMgr->sndPlayFX();

			Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "DART_JAEGER_1");

			g_clue->_sndMgr->sndPlaySound("end.bk", 0);
			tcMattGoesTo(60);
			Say(STORY_0_TXT, 0, 155, "THE_END_MONASTERY");  /* pict = holy matt */

			StopAnim();
			gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);

			_sceneArgs._returnValue = SCENE_NEW_GAME;
		} else {
			Say(STORY_0_TXT, 0, Grull->PictID, "DART_GRULL_2");
			Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "DART_OLD_MATT_0");

			gfxShow(161, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1); /* Polizei - refresh */

			_sceneArgs._returnValue = SCENE_POLICE;
		}
	} else
		_sceneArgs._returnValue = SCENE_POLICE;
}

void tcDoneGludoBurnsOffice() {
	PersonNode *Gludo = (PersonNode *) dbGetObject(Person_John_Gludo);

	StopAnim();

	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_1");
	Say(STORY_0_TXT, 0, MATT_PICTID, "5TH_MATT_0");
	Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_0");

	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_2");
	Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_1");
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_3");

	Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_2");

	g_clue->_sndMgr->sndPrepareFX("brille.voc");
	gfxShow(162, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);
	g_clue->_sndMgr->sndPlayFX();

	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_4");
	Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_3");
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_5");

	Say(STORY_0_TXT, 0, MATT_PICTID, "5TH_MATT_1");
	Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_4");
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_6");

	g_clue->_sndMgr->sndPrepareFX("streich.voc");
	g_clue->_sndMgr->sndPlayFX();

	gfxShow(153, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);

	Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_5");
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_7");

	gfxChangeColors(l_gc, 5, GFX_FADE_OUT, 0);

	_sceneArgs._returnValue = SCENE_WATLING;
}

void tcDoneBeautifullMorning() {
	tcAsTimeGoesBy(_film->getMinute() + 187);

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "MORNING_MATT_0");

	PlayAnim("Sleep", 30000, GFX_DONT_SHOW_FIRST_PIC);
	tcAsTimeGoesBy(546);
	StopAnim();

	gfxShow(173, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1); /* Hotel */

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "MORNING_MATT_1");

	AddTaxiLocation(61);

	_sceneArgs._returnValue = SCENE_HOTEL_ROOM;
}

void tcDoneVisitingSabien() {
	PersonNode *Sabien = (PersonNode *)dbGetObject(Person_Sabien_Pardo);

	knowsSet(Person_Matt_Stuvysunt, Person_Sabien_Pardo);

	Say(STORY_1_TXT, 0, Sabien->PictID, "GROVE_SABIEN_0");
	Say(STORY_1_TXT, 0, MATT_PICTID, "GROVE_MATT_0");
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "GROVE_OLD_MATT_0");
	Say(STORY_1_TXT, 0, Sabien->PictID, "GROVE_SABIEN_1");
	Say(STORY_1_TXT, 0, MATT_PICTID, "GROVE_MATT_1");
	Say(STORY_1_TXT, 0, Sabien->PictID, "GROVE_SABIEN_2");
	Say(STORY_1_TXT, 0, MATT_PICTID, "GROVE_MATT_2");
	Say(STORY_1_TXT, 0, Sabien->PictID, "GROVE_SABIEN_3");
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "GROVE_OLD_MATT_1");

	gfxChangeColors(l_gc, 5, GFX_FADE_OUT, 0);
	_sceneArgs._returnValue = SCENE_LISSON_GROVE;
}

void tcDoneADream() {
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST1_OLD_0");

	PlayAnim("Sleep", 30000, GFX_DONT_SHOW_FIRST_PIC);
	tcAsTimeGoesBy(517);
	StopAnim();

	gfxShow(173, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1); /* Hotel */

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST1_OLD_1");

	tcSomebodyIsCalling();

	Say(STORY_1_TXT, 0, PHONE_PICTID, "ST1_BRIGGS_0");
	Say(STORY_1_TXT, 0, MATT_PICTID, "ST1_MATT_0");
	Say(STORY_1_TXT, 0, PHONE_PICTID, "ST1_BRIGGS_1");
	Say(STORY_1_TXT, 0, MATT_PICTID, "ST1_MATT_1");
	Say(STORY_1_TXT, 0, PHONE_PICTID, "ST1_BRIGGS_2");
	Say(STORY_1_TXT, 0, MATT_PICTID, "ST1_MATT_2");
	Say(STORY_1_TXT, 0, PHONE_PICTID, "ST1_BRIGGS_3");
	Say(STORY_1_TXT, 0, MATT_PICTID, "ST1_MATT_3");
	Say(STORY_1_TXT, 0, PHONE_PICTID, "ST1_BRIGGS_4");

	_sceneArgs._returnValue = SCENE_HOTEL_ROOM;
}

void tcMeetingRosenblatt() {
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_2_OLD_0");

	_sceneArgs._returnValue = SCENE_STATION;
}

void tcBriggsAngry() {
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_2_NOT_OLD_0");

	_sceneArgs._returnValue = SCENE_HOTEL_REC;
}

void tcSabienInWalrus() {
	PersonNode *Sabien = (PersonNode *)dbGetObject(Person_Sabien_Pardo);

	g_clue->_sndMgr->sndPlaySound("sabien.bk", 0);
	StopAnim();

	Say(STORY_1_TXT, 0, Sabien->PictID, "ST_3_SABIEN_0");
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_3_OLD_0");
	gfxShow(157, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1); /* Kiss */

	inpSetWaitTicks(200);
	inpWaitFor(INP_LBUTTONP | INP_TIME);
	inpSetWaitTicks(0);

	gfxShow(141, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1); /* Walrus */

	_sceneArgs._returnValue = SCENE_WALRUS;
}

void tcWalrusTombola() {
	EnvironmentNode *Env = (EnvironmentNode *)dbGetObject(Environment_TheClou);

	Env->Present = 1;
	g_clue->_sndMgr->sndPlaySound("sabien.bk", 0);

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_5_OLD_0");

	_sceneArgs._returnValue = SCENE_WALRUS;
}

void tcRainyEvening() {
	PersonNode *Briggs = (PersonNode *)dbGetObject(Person_Herbert_Briggs);

	tcAsTimeGoesBy(1220);

	Say(STORY_1_TXT, 0, MATT_PICTID, "ST_8_MATT_0");
	Say(STORY_1_TXT, 0, LETTER_PICTID, "ST_8_SABIEN_0");
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_8_OLD_0");

	PlayAnim("Sleep", 30000, GFX_DONT_SHOW_FIRST_PIC);
	tcAsTimeGoesBy(424);
	StopAnim();

	gfxShow(173, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1); /* Hotel */

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_9_OLD_0");
	Say(STORY_1_TXT, 0, Briggs->PictID, "ST_9_BRIGGS_0");
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_9_OLD_1");

	_sceneArgs._returnValue = SCENE_HOTEL_ROOM;
}

void tcDoneMissedDate() {
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "VERPASST_BRIEF");
	Say(STORY_1_TXT, 0, LETTER_PICTID, "DATE_VERPASST");

	_sceneArgs._returnValue = SCENE_HOTEL_ROOM;
}

/***********************************************************************
 ***
 *** 6th BURGLARY
 ***
 ***********************************************************************/

void tcDone6thBurglary() {
	AddTaxiLocation(43);    /* vict & alb */
	AddTaxiLocation(37);    /* brit */

	if (g_clue->getFeatures() & GF_PROFIDISK)
		AddTaxiLocation(79);    /* buckingham */

	hasSet(Person_Marc_Smith, Car_Pontiac_Streamliner_1949);
	hasSet(Person_Marc_Smith, Car_Triumph_Roadstar_1949);

	livesInSet(London_London_1, Person_Kevin_Smith);
	livesInSet(London_London_1, Person_Al_Mel);

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		livesInSet(London_London_1, Person_Phil_Ciggy);
		livesInSet(London_London_1, Person_Rod_Masterson);
	}

	getScene(SCENE_FAHNDUNG)->_occurrence = 0;    /* So that burglary 2 doesn't happen */
	_sceneArgs._returnValue = getLocScene(_film->getLocation())->_eventNr;
}

void tcPoliceInfoTower() {
	BuildingNode *tower = (BuildingNode *)dbGetObject(Building_Tower_of_London);
	EnvironmentNode *Env = (EnvironmentNode *)dbGetObject(Environment_TheClou);

	if (Env->Present) {
		Say(STORY_1_TXT, 0, 223, "ST_7_OLD_0"); /* Matt mit Bart */
		tower->Exactlyness = 175;

		Present(Building_Tower_of_London, "Building", InitBuildingPresent);

		_sceneArgs._returnValue = SCENE_WATLING;

		StopAnim();
		gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);
	} else
		_sceneArgs._returnValue = SCENE_POLICE;
}

void tcPresentInHotel() {
	PersonNode *Ben = (PersonNode *)dbGetObject(Person_Ben_Riggley);
	EnvironmentNode *Env = (EnvironmentNode *)dbGetObject(Environment_TheClou);

	Env->Present = 1;

	Say(STORY_1_TXT, 0, Ben->PictID, "ST_6_BEN_0");
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_6_OLD_0");

	_sceneArgs._returnValue = SCENE_HOTEL_REC;
}

void tcSabienDinner() {
	tcAsTimeGoesBy(1210);
	g_clue->_sndMgr->sndPlaySound("sabien.bk", 0);

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_4_OLD_0");
	tcMattGoesTo(62);

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_4_OLD_1");
	tcAsTimeGoesBy(_film->getMinute() + 160);

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_4_OLD_2");

	gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);
	_sceneArgs._returnValue = SCENE_WALRUS;
}

/***********************************************************************
 ***
 *** 7th BURGLARY
 ***
 ***********************************************************************/

void tcDone7thBurglary() {
	AddTaxiLocation(41);    /* national */
	AddTaxiLocation(45);    /* bank */

	if (g_clue->getFeatures() & GF_PROFIDISK)
		AddTaxiLocation(81);    /* bulstrode  */

	/* Jaguar kommt hier, da: 1. man barucht ihn für Villa, */
	/* 2. Jaguar wird in Fahndung nie erwischt (sonst könnte er nicht explodieren) */
	/* 3. man soll Jaguar nicht zu lange haben können (da man nie erwischt wird  */

	hasSet(Person_Marc_Smith, Car_Jaguar_XK_1950);

	livesInSet(London_London_1, Person_Mohammed_Abdula);

	getScene(SCENE_FAHNDUNG)->_occurrence = 0;    /* So that burglary 2 doesn't happen */
	_sceneArgs._returnValue = getLocScene(_film->getLocation())->_eventNr;
}

void tcDoneBirthday() {
	StopAnim();
	gfxShow(172, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);

	g_clue->_sndMgr->sndPrepareFX("birthd2.voc");    /* applause */
	g_clue->_sndMgr->sndPlayFX();

	knowsAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST, Object_Person);
	NewList<dbObjectNode> *persons = ObjectListPrivate;

	for (dbObjectNode *n = persons->getListHead(); n->_succ; n = (dbObjectNode *) n->_succ) {
		PersonNode *p = (PersonNode *)dbGetObject(n->_nr);

		switch (n->_nr) {
		case Person_Sabien_Pardo:
		case Person_Herbert_Briggs:
		case Person_John_Gludo:
		case Person_Miles_Chickenwing:
		/*case Person_Old_Matt:*/
		case Person_Red_Stanson:
			break;
		default:
			if (livesIn(London_London_1, n->_nr) && (g_clue->calcRandomNr(0, 10) < 7)) {
				tcMoveAPerson(n->_nr, Location_Walrus);

				Say(STORY_1_TXT, 0, p->PictID, "ST_11_ALL_0");
			}
			break;
		}
	}

	g_clue->_sndMgr->sndPrepareFX("birthd1.voc");    /* cork popping */
	g_clue->_sndMgr->sndPlayFX();

	persons->removeList();

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_11_OLD_0");
	gfxShow(141, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);

	_sceneArgs._returnValue = SCENE_WALRUS;
}

void tcWalkWithSabien() {
	EnvironmentNode *Env = (EnvironmentNode *)dbGetObject(Environment_TheClou);

	g_clue->_sndMgr->sndPlaySound("sabien.bk", 0);

	gfxShow(165, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_13_OLD_0");

	gfxShow(158, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1); /* Sabien outside! */

	Env->MattIsInLove = 1;

	_sceneArgs._returnValue = SCENE_LISSON_GROVE;
}

void tcDoneSabienCall() {
	tcSomebodyIsCalling();

	Say(STORY_0_TXT, 0, PHONE_PICTID, "A_CALL_FOR_YOU");
	Say(STORY_1_TXT, 0, PHONE_PICTID, "ST_10_SABIEN");

	_sceneArgs._returnValue = SCENE_HOTEL_ROOM;
}

void tcDoneMeetingAgain() {
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_10_OLD_0");

	gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);

	_sceneArgs._returnValue = SCENE_LISSON_GROVE;
}

/***********************************************************************
 ***
 *** 8th BURGLARY
 ***
 ***********************************************************************/


void tcDone8thBurglary() {
	AddTaxiLocation(25);    /* villa */

	getScene(SCENE_FAHNDUNG)->_occurrence = 0;    /* So that burglary 2 doesn't happen */
	_sceneArgs._returnValue = getLocScene(_film->getLocation())->_eventNr;
}

void tcDoneAgent() {
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_14_OLD_0");

	tcSomebodyIsCalling();

	Say(STORY_1_TXT, 0, PHONE_PICTID, "ST_14_AGENT_0");
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_14_OLD_1");

	tcAddPlayerMoney(15000);

	_sceneArgs._returnValue = SCENE_HOTEL_ROOM;
}

/***********************************************************************
 ***
 *** 9th BURGLARY
 ***
 ***********************************************************************/

void tcDone9thBurglary() {
	EnvironmentNode *Env = (EnvironmentNode *)dbGetObject(Environment_TheClou);

	_film->setMinute(540);

	if (has(Person_Matt_Stuvysunt, Loot_Trunk)) {
		Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_15_OLD_0");
		gfxShow(174, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);  /* cracks */
	} else {
		Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_15_OLD_NOT_0");
		SetBubbleType(THINK_BUBBLE);
		Say(STORY_1_TXT, 0, MATT_PICTID, "ST_15_MATT_NOT_0");

		Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_15_OLD_NOT_1");
		gfxShow(174, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);  /* cracks */
	}

	inpDelay(150);
	gfxShow(173, GFX_ONE_STEP | GFX_NO_REFRESH, 0, -1, -1); /* hotel */

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_15_OLD_1");

	tcSomebodyIsCalling();

	Say(STORY_1_TXT, 0, PHONE_PICTID, "ST_15_ALLEN_0");

	/* keine Polizisten sind mehr unterwegs */
	livesInUnSet(London_London_1, Person_John_Gludo);
	livesInUnSet(London_London_1, Person_Miles_Chickenwing);
	livesInUnSet(London_London_1, Person_Red_Stanson);

	tcMoveAPerson(Person_John_Gludo, Location_Nirvana);
	tcMoveAPerson(Person_Miles_Chickenwing, Location_Nirvana);
	tcMoveAPerson(Person_Red_Stanson, Location_Nirvana);

	/* nur mehr cars&vans bzw. Lisson Grove zu erreichen */
	RemRelation(Relation_taxi);
	AddRelation(Relation_taxi);

	AddTaxiLocation(1);     /* cars */

	if (Env->MattIsInLove)
		AddTaxiLocation(61);    /* lisson */

	/* nur mehr Möglichkeiten: gehen */
	_film->setEnabledChoices(GP_CHOICE_GO | GP_CHOICE_WAIT);

	getScene(SCENE_FAHNDUNG)->_occurrence = 0;    /* so that burglary 2 doesn't happen */
	_sceneArgs._returnValue = getLocScene(_film->getLocation())->_eventNr;
}

void tcDoneGoAndFetchJaguar() {
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_16_OLD_0");

	_sceneArgs._returnValue = SCENE_HOLLAND_STR;
}

void tcDoneThinkOfSabien() {
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_16_OLD_1");

	_sceneArgs._returnValue = SCENE_HOLLAND_STR;
}

void tcDoneTerror() {
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_17_OLD_0");
	/* XXX
	    gfxPrepareColl (211);
	    gfxPrepareColl (210);
	    gfxPrepareColl (209);
	*/
	gfxShow(176, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);
	inpDelay(150);

	g_clue->_sndMgr->sndPrepareFX("explosio.voc");
	g_clue->_sndMgr->sndPlayFX();

	PlayAnim("Explo1", 1, GFX_DONT_SHOW_FIRST_PIC);
	inpDelay(200);

	StopAnim();

	PlayAnim("Explo2", 50, GFX_DONT_SHOW_FIRST_PIC);
	inpDelay(260);

	StopAnim();

	hasUnSet(Person_Matt_Stuvysunt, Car_Jaguar_XK_1950);

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_17_OLD_1");
	gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);

	_sceneArgs._returnValue = SCENE_CARS_VANS;
}

void tcDoneConfessingSabien() {
	PersonNode *Sabien = (PersonNode *)dbGetObject(Person_Sabien_Pardo);
	EnvironmentNode *Env = (EnvironmentNode *)dbGetObject(Environment_TheClou);

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_18_OLD_0");
	Say(STORY_1_TXT, 0, Sabien->PictID, "ST_18_SABIEN_0");
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_18_OLD_1");
	Say(STORY_1_TXT, 0, Sabien->PictID, "ST_18_SABIEN_1");

	Env->WithOrWithoutYou = Say(STORY_1_TXT, 0, MATT_PICTID, "ST_18_MATT_0");

	StopAnim();

	if (Env->WithOrWithoutYou) {    /* bleibt bei Sabien! */
		gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);
		ShowMenuBackground();

		gfxShow(163, GFX_NO_REFRESH | GFX_ONE_STEP | GFX_BLEND_UP, 0, -1, -1);  /* south 1 */
		gfxShow(152, GFX_NO_REFRESH | GFX_OVERLAY, 3, -1, -1);  /* family */

		g_clue->_sndMgr->sndPlaySound("sabien.bk", 0);

		Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_19_OLD_0");

		/* hier eventuell glückliche Anim zeigen! */
		gfxShow(164, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);  /*  The End */

		inpWaitFor(INP_LBUTTONP);
		gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);

		_sceneArgs._returnValue = SCENE_NEW_GAME;
	} else {            /* bleibt nicht bei Sabien! */

		addVTime(2713);     /* hier nicht ausblenden? */

		gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);

		_sceneArgs._returnValue = SCENE_SOUTHHAMPTON;

		_film->setLocation((uint32) -1);    /* damit sicher eingeblendet wird! */
	}
}

void tcDoneSouthhamptonWithoutSabien() {
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_20_OLD_0");

	_sceneArgs._returnValue = SCENE_SOUTHHAMPTON;
}

void tcDoneSouthhamptonSabienUnknown() {
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_20W_OLD_0");

	StopAnim();
	gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);

	_sceneArgs._returnValue = SCENE_SOUTHHAMPTON;
}

static void tcDoneFirstTimeLonelyInSouth() {
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "SouthhamptonMenu");
	EnvironmentNode *Env = (EnvironmentNode *)dbGetObject(Environment_TheClou);
	PersonNode *Herb = (PersonNode *)dbGetObject(Person_Herbert_Briggs);

	tcAsDaysGoBy(713518, 30);
	uint32 startTime = _film->getDay() * 1440 + _film->getMinute();

	byte activ = 0;
	bool endLoop = false;
	while (!endLoop) {
		inpTurnFunctionKey(false);
		inpTurnESC(false);

		activ = Menu(menu, 46, activ, nullptr, 0);
		inpTurnESC(true);
		inpTurnFunctionKey(true);

		uint32 actionTime = g_clue->calcRandomNr(180, 300);    /* 2 bis 5 Stunden */

		switch (activ) {
		case 1:
			tcAsTimeGoesBy(_film->getMinute() + actionTime);
			break;      /* spazieren */
		case 2:
			tcAsTimeGoesBy(_film->getMinute() + actionTime);
			break;      /* warten  */
		case 3:
			tcAsTimeGoesBy(_film->getMinute() + actionTime);
			break;      /* fischen */
		case 5:
			Information();
			ShowTime(0);
			break;
		default:
			break;
		}

		if (_film->getDay() * 1440 + _film->getMinute() > startTime + 420 && !(Env->SouthhamptonHappened & 1)) {  /* nach 7 Stunden */
			Env->SouthhamptonHappened |= 1;

			Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_21_OLD");  /* Kummer */
		}

		if (_film->getDay() * 1440 + _film->getMinute() > startTime + 1020 && !(Env->SouthhamptonHappened & 2)) { /* nach 17 Stunden */
			Env->SouthhamptonHappened |= 2;

			SetBubbleType(THINK_BUBBLE);

			if (Env->MattIsInLove)
				Say(STORY_1_TXT, 0, MATT_PICTID, "ST_22_MATT"); /* Neugier */
			else
				Say(STORY_1_TXT, 0, MATT_PICTID, "ST_22O_MATT");
		}

		if (_film->getDay() * 1440 + _film->getMinute() > startTime + 1500 && !(Env->SouthhamptonHappened & 4)) { /* nach 25 Stunden */
			Env->SouthhamptonHappened |= 4;

			if (Env->MattIsInLove)
				Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_23_MATT"); /* Neugier */
			else
				Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_23O_MATT");
		}

		if (_film->getDay() * 1440 + _film->getMinute() > startTime + 2100 && !(Env->SouthhamptonHappened & 8)) {
			Env->SouthhamptonHappened |= 8;

			Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_24_OLD_0");    /* The Return */
			Say(STORY_1_TXT, 0, Herb->PictID, "ST_24_BRIGGS_0");
			Say(STORY_1_TXT, 0, MATT_PICTID, "ST_24_MATT_0");
			Say(STORY_1_TXT, 0, Herb->PictID, "ST_24_BRIGGS_1");
			Say(STORY_1_TXT, 0, MATT_PICTID, "ST_24_MATT_1");
			Say(STORY_1_TXT, 0, Herb->PictID, "ST_24_BRIGGS_2");

			if (Env->MattIsInLove) {
				Say(STORY_1_TXT, 0, MATT_PICTID, "ST_24_MATT_2");
				Say(STORY_1_TXT, 0, Herb->PictID, "ST_24_BRIGGS_3");
			}

			Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_24_OLD_1");
			Say(STORY_1_TXT, 0, MATT_PICTID, "ST_24_MATT_3");
			Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_24_OLD_2");

			if (Env->MattIsInLove)
				Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_24_OLD_3");
			else
				Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_24O_OLD_3");

			hasSet(Person_Matt_Stuvysunt, Building_Tower_of_London);

			endLoop = true;
		}

		ShowTime(0);
	}

	menu->removeList();
}

void tcDoneSouthhampton() {
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "SouthhamptonMenu");
	EnvironmentNode *Env = (EnvironmentNode *)dbGetObject(Environment_TheClou);

	_sceneArgs._overwritten = true;
	_sceneArgs._returnValue = 0;  /* MUß SEIN! */

	_film->setEnabledChoices(GP_ALL_CHOICES);

	addVTime(g_clue->calcRandomNr(560, 830));
	ShowTime(0);

	if (Env->FirstTimeInSouth) {
		Env->FirstTimeInSouth = 0;
		tcDoneFirstTimeLonelyInSouth();
		tcInitTowerBurglary();  /* wird absichtlich X mal aufgerufen - siehe weiter unten! */
	}

	/* mit Gehen oder Planen kommt man aus dem Menü raus */
	byte activ = 1;     /* !! */
	while ((activ != 0) && (_sceneArgs._returnValue == 0)) {
		inpTurnESC(false);
		inpTurnFunctionKey(true);
		activ = Menu(menu, 127, activ, nullptr, 0);
		inpTurnFunctionKey(false);
		inpTurnESC(true);

		if (activ == (byte) -1) {
			ShowTheClouRequester(No_Error);
			_sceneArgs._returnValue =
			    ((PlayerNode *) dbGetObject(Player_Player_1))->CurrScene;

			activ = 1;      /* nicht 0! */
		} else {
			uint32 actionTime = g_clue->calcRandomNr(180, 300);    /* 2 bis 5 Stunden */

			switch (activ) {
			case 0:
				break;

			case 1:
				tcAsTimeGoesBy(_film->getMinute() + actionTime);
				break;      /* spazieren */
			case 2:
				tcAsTimeGoesBy(_film->getMinute() + actionTime);
				break;      /* warten  */
			case 3:
				tcAsTimeGoesBy(_film->getMinute() + actionTime);
				break;      /* fischen */
			case 4:
				g_clue->_sndMgr->sndPlaySound("hotel.bk", 0);
				tcInitTowerBurglary();  /* just to be sure */
				plPlaner(Building_Tower_of_London);
				refreshCurrScene();
				break;
			case 5:
				Information();
				ShowTime(0);
				inpTurnFunctionKey(true);
				break;
			case 6:
				if (tcDoTowerBurglary())
					_sceneArgs._returnValue = SCENE_KASERNE_OUTSIDE;
				else
					refreshCurrScene();
				break;
			/* Vorsicht bei Erweiterung -> Speichern weiter oben */
			default:
				break;
			}

			ShowTime(0);
		}
	}

	if (!activ) {
		addVTime(g_clue->calcRandomNr(560, 830));
		_sceneArgs._returnValue = SCENE_TOWER_OUT;
	}

	StopAnim();
	gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);

	menu->removeList();
}

void tcInitTowerBurglary() {
	CarNode *car = (CarNode *)dbGetObject(Car_Cadillac_Club_1952);
	PlayerNode *player = (PlayerNode *)dbGetObject(Player_Player_1);

	/* alle Personen entfernen! */
	player->MattsPart = 25;

	joined_byAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Person);

	for (dbObjectNode *node = ObjectList->getListHead(); node->_succ; node = (dbObjectNode *)node->_succ)
		joined_byUnSet(Person_Matt_Stuvysunt, node->_nr);

	/* und Personen, Abilities neu setzen! */
	joined_bySet(Person_Matt_Stuvysunt, Person_Matt_Stuvysunt);
	joined_bySet(Person_Matt_Stuvysunt, Person_Herbert_Briggs);
	joined_bySet(Person_Matt_Stuvysunt, Person_Marc_Smith);
	joined_bySet(Person_Matt_Stuvysunt, Person_Mohammed_Abdula);

	joinSet(Person_Matt_Stuvysunt, Person_Matt_Stuvysunt);
	joinSet(Person_Matt_Stuvysunt, Person_Herbert_Briggs);
	joinSet(Person_Matt_Stuvysunt, Person_Marc_Smith);
	joinSet(Person_Matt_Stuvysunt, Person_Mohammed_Abdula);

	knowsSet(Person_Matt_Stuvysunt, Person_Marc_Smith);
	knowsSet(Person_Matt_Stuvysunt, Person_Mohammed_Abdula);

	/* all tools */
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

	if (hasGet(Person_Matt_Stuvysunt, Ability_Locks) < 210)
		hasSetP(Person_Matt_Stuvysunt, Ability_Locks, 210);

	if (hasGet(Person_Matt_Stuvysunt, Ability_Electronic) < 251)
		hasSetP(Person_Matt_Stuvysunt, Ability_Electronic, 251);

	hasSetP(Person_Marc_Smith, Ability_Autos, 255);
	hasSetP(Person_Mohammed_Abdula, Ability_Fight, 255);

	hasSet(Person_Matt_Stuvysunt, Car_Cadillac_Club_1952);

	/* Auto */
	car->State = 255;
	car->MotorState = 255;
	car->BodyWorkState = 180;
	car->TyreState = 255;
	car->Strike = 80;       /* extrem niedrig! (für diesen Wagen) */

	/* Organisatorisches! */
	Organisation.CarID = Car_Cadillac_Club_1952;
	Organisation.DriverID = Person_Marc_Smith;
	Organisation.BuildingID = Building_Tower_of_London;

	Organisation.GuyCount = 4;
}

int32 tcDoTowerBurglary() {
	/* das Organisatorische muß hier wiederholt werden, da */
	/* es sonst zu Fehler kommen kann, da die Organisation */
	/* nicht abgespeichert wird!                           */
	tcInitTowerBurglary();

	/* und los gehts! */
	int32 burglary = plPlayer(Building_Tower_of_London, 0, nullptr);

	if (burglary) {
		if (has(Person_Matt_Stuvysunt, Loot_Crown_jewels))
			Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_25_OLD_0");
		else
			Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_25O_OLD_0");

		tcDoneMafia();
	}

	return burglary;
}

void tcDoneMafia() {
	PersonNode *Ken = (PersonNode *)dbGetObject(Person_Ken_Addison);
	PersonNode *Briggs = (PersonNode *)dbGetObject(Person_Herbert_Briggs);

	CurrentBackground = BGD_LONDON;
	ShowMenuBackground();

	gfxShow(168, GFX_NO_REFRESH | GFX_ONE_STEP | GFX_CLEAR_FIRST | GFX_FADE_OUT | GFX_BLEND_UP, 3, -1, -1); /* Mafia */

	Say(STORY_1_TXT, 0, Ken->PictID, "ST_25_PATE_0");

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_25_OLD_1");
	Say(STORY_1_TXT, 0, Briggs->PictID, "ST_25_BRIGGS_0");

	Say(STORY_1_TXT, 0, MATT_PICTID, "ST_25_MATT_0");
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_25_OLD_2");

	g_clue->_sndMgr->sndPlaySound("street1.bk", 0);

	gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);
	tcAsTimeGoesBy(_film->getMinute() + 420);

	tcMattGoesTo(25);       /* Villa */

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_26_OLD_0");
	Say(STORY_1_TXT, 0, Ken->PictID, "ST_26_PATE_0");
	Say(STORY_1_TXT, 0, Ken->PictID, "KEN_PLAN");

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_26_OLD_1");
}

void tcDoneKaserne() {
	EnvironmentNode *Env = (EnvironmentNode *)dbGetObject(Environment_TheClou);
	CarNode *car = (CarNode *)dbGetObject(Car_Cadillac_Club_1952);
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "KaserneMenu");

	joined_bySet(Person_Matt_Stuvysunt, Person_Matt_Stuvysunt);
	joined_bySet(Person_Matt_Stuvysunt, Person_Herbert_Briggs);
	joined_bySet(Person_Matt_Stuvysunt, Person_Marc_Smith);
	joined_bySet(Person_Matt_Stuvysunt, Person_Ken_Addison);

	joined_byUnSet(Person_Matt_Stuvysunt, Person_Mohammed_Abdula);

	joinSet(Person_Matt_Stuvysunt, Person_Ken_Addison);
	knowsSet(Person_Matt_Stuvysunt, Person_Ken_Addison);

	((BuildingNode *) dbGetObject(Building_Starford_Kaserne))->Exactlyness = 255;

	hasSet(Person_Matt_Stuvysunt, Building_Starford_Kaserne);

	/* Auto */
	car->State = 255;
	car->MotorState = 255;
	car->BodyWorkState = 180;
	car->TyreState = 255;
	car->Strike = 80;       /* extrem niedrig! (für diesen Wagen) */

	/* Organisatorisches! */
	Organisation.CarID = Car_Cadillac_Club_1952;
	Organisation.DriverID = Person_Marc_Smith;

	Organisation.GuyCount = 4;

	/* und los gehts! */
	byte burglary = 0;
	uint32 successor = 0;
	byte activ = 0;
	while ((!burglary) && (!successor)) {
		inpTurnESC(false);
		activ = Menu(menu, 15, activ, nullptr, 0);
		inpTurnESC(true);

		/* change possibilites in patchStory too! */
		if (activ == (byte) -1) {
			ShowTheClouRequester(No_Error);
			successor = ((PlayerNode *) dbGetObject(Player_Player_1))->CurrScene;

			activ = 1;
		} else {
			switch (activ) {
			case 0:
				StopAnim();
				gfxChangeColors(l_gc, 3, GFX_FADE_OUT, nullptr);
				_film->setLocation(65);
				_sceneArgs._options = 265; // TODO: Set the variable properly, using GP_CHOICE values
				DoneInsideHouse();
				tcMattGoesTo(66);
				break;
			case 1:
				Information();
				break;
			case 2:
				StopAnim();
				plPlaner(Building_Starford_Kaserne);
				tcMattGoesTo(66);   /* refresh! */
				break;
			case 3:
				StopAnim();
				Search.KaserneOk = 0;
				if (!(burglary = plPlayer(Building_Starford_Kaserne, 0, nullptr)))
					tcMattGoesTo(66);   /* refresh! */
				break;
			default:
				break;
			}
		}
	}

	StopAnim();

	if (burglary) {
		if (Search.KaserneOk) {
			g_clue->_sndMgr->sndPlaySound("end.bk", 0);

			if (Env->MattIsInLove)
				Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_27_OLD_0");
			else
				Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_28_OLD_0");

			gfxShow(224, GFX_NO_REFRESH | GFX_ONE_STEP | GFX_FADE_OUT | GFX_BLEND_UP, 3, -1, -1);
			Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_29_OLD_0");
		} else {
			/* sndPlaySound() in tcDonePrison() */

			Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_28_OLD_1");
			tcDonePrison();
		}

		successor = SCENE_NEW_GAME;
	}

	gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);
	menu->removeList();

	_sceneArgs._returnValue = successor;
}

int32 tcIsLastBurglaryOk() {
	BuildingNode *kaserne = (BuildingNode *)dbGetObject(Building_Starford_Kaserne);
	int16 carXPos0 = kaserne->CarXPos - 40;
	int16 carYPos0 = kaserne->CarYPos - 40;
	int16 carXPos1 = kaserne->CarXPos + 40;
	int16 carYPos1 = kaserne->CarYPos + 40;
	LSObjectNode *left = (LSObjectNode *)dbGetObject(tcLAST_BURGLARY_LEFT_CTRL_OBJ);
	LSObjectNode *right = (LSObjectNode *)dbGetObject(tcLAST_BURGLARY_RIGHT_CTRL_OBJ);

	/* Links muß ein, Rechts muß ausgeschalten sein */
	/* 1.. OFF!    */

	bool madeIt = true;
	if ((left->ul_Status & (1 << Const_tcON_OFF)))
		madeIt = false;

	if (!(right->ul_Status & (1 << Const_tcON_OFF)))
		madeIt = false;

	if (!has(Person_Matt_Stuvysunt, Loot_Deed))
		madeIt = false;

	/* alle anderen müssen beim Auto sein... */
	for (int32 i = 1; i < 4; i++) {
		if (Search.GuyXPos[i] < carXPos0 || Search.GuyXPos[i] > carXPos1 || Search.GuyYPos[i] < carYPos0 || Search.GuyYPos[i] > carYPos1)
			madeIt = false;
	}

	// The original code was using i for the last check, thus making a read out of bound.
	if (Search.GuyXPos[0] >= carXPos0 && Search.GuyXPos[0] <= carXPos1 && Search.GuyYPos[0] >= carYPos0 && Search.GuyYPos[0] <= carYPos1) {
		warning("CHECKME: Modified check in tcIsLastBurglaryOk()");
		madeIt = false;
	}

	return madeIt;
}

} // End of namespace Clue

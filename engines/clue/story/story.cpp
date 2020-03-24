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
#include "clue/clue.h"

namespace Clue {

byte tcDoLastBurglarySpot(uint32 ul_Time, uint32 ul_BuildingId);

static void tcSomebodyIsComing() {
	for (uint8 i = 0; i < 3; i++) {
		inpDelay(50);
		sndPrepareFX("klopfen.voc");
		sndPlayFX();
	}
}

static void tcSomebodyIsCalling() {
	for (uint32 i = 0; i < g_clue->calcRandomNr(1, 4); i++) {
		inpDelay(180);
		sndPrepareFX("ring.voc");
		sndPlayFX();
	}
}

byte tcKarateOpa(uint32 ul_ActionTime, uint32 ul_BuildingId) {
	livSetAllInvisible();
	lsSetViewPort(0, 0);    /* links, oben */

	gfxShow(217, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);

	return 1;
}

void tcDoneCredits() {
	Person ben = (Person)dbGetObject(Person_Ben_Riggley);

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
	Person rig = (Person) dbGetObject(Person_Ben_Riggley);
	Environment env = (Environment) dbGetObject(Environment_TheClou);

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
	Person Gludo = (Person) dbGetObject(Person_John_Gludo);
	Environment env = (Environment)dbGetObject(Environment_TheClou);

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
	Person Jim = (Person) dbGetObject(Person_Jim_Danner);

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
	Person Briggs = (Person) dbGetObject(Person_Herbert_Briggs);

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
		Building bui = (Building) dbGetObject(Building_Kiosk);

		hasSet(Person_Matt_Stuvysunt, Building_Kiosk);

		tcAddPlayerMoney(15);
		tcAddBuildExactlyness(bui, 255L);
		tcAddBuildStrike(bui, 5L);

		Say(STORY_0_TXT, 0, Briggs->PictID, "BRIGGS_BRIGGS_4");

		Present(Car_Fiat_Topolino_1940, "Car", InitCarPresent);
		Present(Building_Kiosk, "Building", InitBuildingPresent);

		Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "AFTER_MEETING_BRIGGS");

		hasSet(Person_Matt_Stuvysunt, Car_Fiat_Topolino_1940);

		_sceneArgs._returnValue = SCENE_FAT_MANS;
	} else {            /* nicht angenommen ! */
		Person james = (Person)dbGetObject(Person_Pater_James);

		Say(STORY_0_TXT, 0, Briggs->PictID, "BRIGGS_BRIGGS_5");
		gfxShow(170, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);
		Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "BRIGGS_MR_WHISKY");

		tcAsTimeGoesBy(GetMinute + 793);

		StopAnim();
		gfxChangeColors(l_gc, 8, GFX_FADE_OUT, 0);

		tcMattGoesTo(60);   /* Kloster */

		Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "KLOSTER");
		Say(STORY_0_TXT, 0, james->PictID, "ABT");

		choice = Say(STORY_0_TXT, 0, MATT_PICTID, "HOLY_MATT");

		if (choice == 0) {  /* holy */
			sndPlaySound("end.bk", 0);

			Say(STORY_0_TXT, 0, 155, "THE_END_MONASTERY");  /* pict = holy matt ! */
			Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "THE_EDGE");

			_sceneArgs._returnValue = SCENE_NEW_GAME;
		} else {        /* evil */
			Say(STORY_0_TXT, 0, MATT_PICTID, "EVIL_MATT");
			Say(STORY_0_TXT, 0, james->PictID, "EVIL_MATT_ABT");
			Say(STORY_0_TXT, 0, MATT_PICTID, "EVIL_MATT_1");
			Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "EVIL_OLD_MATT");

			hasSetP(Person_Matt_Stuvysunt, Loot_Ring_des_Abtes, tcVALUE_OF_RING_OF_PATER);

			AddVTime(1440 + 525 - GetMinute);

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
	Person dan = (Person) dbGetObject(Person_Dan_Stanford);

	knowsSet(Person_Matt_Stuvysunt, Person_Dan_Stanford);

	Say(STORY_0_TXT, 0, 7, "AEHHH");
	Say(STORY_0_TXT, 0, dan->PictID, "FREE_TICKET");

	_sceneArgs._returnValue = GetLocScene(8)->EventNr;
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

	if (has(Person_Matt_Stuvysunt, Loot_Ring_des_Abtes)) {
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

	SetLocation(64);        /* auf ins Gefängnis */
}

void tcDonePrison()
/*
 * wird schmutzigerweise von tcDoneGludoAsSailor,
 * tcLastBurglary
 * direkt angesprungen
 */
{
	SetDay(719792L);        /* 13.01.1972? */

	sndPlaySound("end.bk", 0);  /* dont change it! (-> story_9) */
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
	CompleteLoot comp = (CompleteLoot)dbGetObject(CompleteLoot_LastLoot);

	hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Car);

	if (LIST_EMPTY(ObjectList)) {
		int32 money = tcGetPlayerMoney;
		bool enough = false;

		/* jetzt zum Geld noch die vorhandene Beute addieren */
		RemoveList(tcMakeLootList(Person_Matt_Stuvysunt, Relation_has));

		int32 total = comp->Bild + comp->Gold + comp->Geld + comp->Juwelen +
		        comp->Statue + comp->Kuriositaet + comp->HistKunst +
		        comp->GebrauchsArt + comp->Vase + comp->Delikates;

		money += total / 15;    /* im schlimmsten Fall bleibt Matt in */
		/* etwa nur ein Fünfzehnten           */

		hasAll(Person_Marc_Smith, OLF_NORMAL, Object_Car);

		/* get cheapest car! */
		for (Node *n = LIST_HEAD(ObjectList); NODE_SUCC(n); n = NODE_SUCC(n)) {
			Car car = (Car)OL_DATA(n);

			if (tcGetCarPrice(car) < money)
				enough = true;
		}

		if (!enough) {
			Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "DEADLOCK");

			sndPlaySound("end.bk", 0);

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

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		AddTaxiLocation(75);    /* train */
	}

	hasSet(Person_Marc_Smith, Car_Morris_Minor_1950);
	hasSet(Person_Marc_Smith, Car_Fiat_Topolino_1942);
	hasSet(Person_Marc_Smith, Car_Jeep_1945);
	hasSet(Person_Marc_Smith, Car_Pontiac_Streamliner_1946);

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		hasSet(Person_Marc_Smith, Car_Ford_Model_T__1926);
	}

	hasSet(Person_Mary_Bolton, Tool_Dietrich);
	hasSet(Person_Mary_Bolton, Tool_Bohrmaschine);
	hasSet(Person_Mary_Bolton, Tool_Strickleiter);
	hasSet(Person_Mary_Bolton, Tool_Schloszstecher);
	hasSet(Person_Mary_Bolton, Tool_Winkelschleifer);
	hasSet(Person_Mary_Bolton, Tool_Schutzanzug);

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

	GetScene(SCENE_FAHNDUNG)->Geschehen = 0;    /* damit nicht gleich Burglary 2 geschieht */

	_sceneArgs._returnValue = GetLocScene(GetLocation)->EventNr;
}

void tcDoneGludoAsSailor() {
	knowsSet(Person_Matt_Stuvysunt, Person_John_Gludo);

	sndPlaySound("gludo.bk", 0);

	Say(STORY_0_TXT, 0, (uint16) FACE_GLUDO_SAILOR, "SAILOR_GLUDO_0");
	Say(STORY_0_TXT, 0, MATT_PICTID, "SAILOR_MATT_0");
	Say(STORY_0_TXT, 0, (uint16) FACE_GLUDO_SAILOR, "SAILOR_GLUDO_1");

	if (Say(STORY_0_TXT, 0, MATT_PICTID, "SAILOR_MATT_1")) {
		Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "SAILOR_OLD_MATT_0");

		sndPlaySound("street1.bk", 0);
		_sceneArgs._returnValue = SCENE_HOLLAND_STR;
	} else {
		Say(STORY_0_TXT, 0, (uint16) FACE_GLUDO_SAILOR, "SAILOR_GLUDO_2");
		StopAnim();

		tcDonePrison();
	}
}

void tcDoneCallFromBriggs() {
	tcAsTimeGoesBy(GetMinute + 130);

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

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		AddTaxiLocation(72);    /* abbey */
	}

	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "FAHNDUNG");

	hasSet(Person_Marc_Smith, Car_Pontiac_Streamliner_1944);
	hasSet(Person_Marc_Smith, Car_Standard_Vanguard_1953);

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		hasSet(Person_Marc_Smith, Car_Rover_75_1950);
		hasSet(Person_Marc_Smith, Car_Bentley_Continental_Typ_R_1952);
	}

	hasSet(Person_Mary_Bolton, Tool_Funkgeraet);
	hasSet(Person_Mary_Bolton, Tool_Glasschneider);
	hasSet(Person_Mary_Bolton, Tool_Bohrwinde);
	hasSet(Person_Mary_Bolton, Tool_Elektroset);

	livesInSet(London_London_1, Person_Margrete_Briggs);
	livesInSet(London_London_1, Person_Paul_O_Conner);
	livesInSet(London_London_1, Person_Tony_Allen);


	GetScene(SCENE_FAHNDUNG)->Geschehen = 0;    /* damit nicht gleich Burglary 2 geschieht */

	_sceneArgs._returnValue = GetLocScene(GetLocation)->EventNr;
}

/* wird von DoneHotel aufgerufen */
void tcCheckForBones() {
	Person luthm = (Person)dbGetObject(Person_Luthmilla_Nervesaw);

	if (has(Person_Matt_Stuvysunt, Loot_Gebeine)) {
		if (knows(Person_Matt_Stuvysunt, Person_Luthmilla_Nervesaw)) {
			Player player = (Player)dbGetObject(Player_Player_1);

			tcSomebodyIsComing();

			Say(STORY_0_TXT, 0, luthm->PictID, "KARL_MARX");

			tcAddPlayerMoney(20 * player->MattsPart);

			player->StolenMoney += 2000;
			player->MyStolenMoney += (20 * player->MattsPart);

			livesInUnSet(London_London_1, Person_Luthmilla_Nervesaw);
			tcMoveAPerson(Person_Luthmilla_Nervesaw, Location_Nirvana);
			hasUnSet(Person_Matt_Stuvysunt, Loot_Gebeine);
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

	hasSet(Person_Mary_Bolton, Tool_Schneidbrenner);
	hasSet(Person_Mary_Bolton, Tool_Stethoskop);
	hasSet(Person_Mary_Bolton, Tool_Stromgenerator);
	hasSet(Person_Mary_Bolton, Tool_Maske);

	livesInSet(London_London_1, Person_Miguel_Garcia);
	livesInSet(London_London_1, Person_John_O_Keef);
	livesInSet(London_London_1, Person_Samuel_Rosenblatt);

	gfxShow(166, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);

	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "READ_TIMES_0");

	sndPlaySound("gludo.bk", 0);

	Say(STORY_0_TXT, 0, FACE_GLUDO_MAGIC, "READ_TIMES_GLUDO");
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "READ_TIMES_1");

	sndPlaySound("street1.bk", 0);

	gfxShow(150, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);

	GetScene(SCENE_FAHNDUNG)->Geschehen = 0;    /* damit nicht gleich Burglary 2 geschieht */

	_sceneArgs._returnValue = GetLocScene(GetLocation)->EventNr;
}

void tcCheckForDowning() {
	if (has(Person_Matt_Stuvysunt, Loot_Dokument)) {
		Building bui = (Building) dbGetObject(Building_Buckingham_Palace);

		hasSet(Person_Matt_Stuvysunt, Building_Buckingham_Palace);

		tcAddBuildExactlyness(bui, 255L);
		tcAddBuildStrike(bui, 5L);

		SetBubbleType(THINK_BUBBLE);
		Say(INVESTIGATIONS_TXT, 0, OLD_MATT_PICTID, "BuckinghamPlansFound");

		Present(Building_Buckingham_Palace, "Building", InitBuildingPresent);
		hasUnSet(Person_Matt_Stuvysunt, Loot_Dokument);
	}
}


/***********************************************************************
 ***
 *** 4th BURGLARY
 ***
 ***********************************************************************/

void tcDone4thBurglary() {
	Person Gludo = (Person) dbGetObject(Person_John_Gludo);

	AddTaxiLocation(31);    /* osterly */
	AddTaxiLocation(29);    /* ham */

	if (g_clue->getFeatures() & GF_PROFIDISK)
		AddTaxiLocation(70);    /* madame */

	tcSomebodyIsComing();

	sndPlaySound("gludo.bk", 0);
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "ARREST_OLD_MATT_0");
	Say(STORY_0_TXT, 0, Gludo->PictID, "ARREST_GLUDO_0");
	Say(STORY_0_TXT, 0, MATT_PICTID, "ARREST_MATT_0");
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "ARREST_OLD_MATT_1");
	Say(STORY_0_TXT, 0, Gludo->PictID, "ARREST_GLUDO_1");

	hasSet(Person_Marc_Smith, Car_Standard_Vanguard_1950);
	hasSet(Person_Marc_Smith, Car_Cadillac_Club_1952);

	if (g_clue->getFeatures() & GF_PROFIDISK)
		hasSet(Person_Marc_Smith, Car_Fiat_634_N_1943);

	hasSet(Person_Mary_Bolton, Tool_Dynamit);
	hasSet(Person_Mary_Bolton, Tool_Kernbohrer);
	hasSet(Person_Mary_Bolton, Tool_Sauerstofflanze);
	hasSet(Person_Mary_Bolton, Tool_Chloroform);

	livesInSet(London_London_1, Person_Garry_Stevenson);
	livesInSet(London_London_1, Person_Jiri_Poulin);
	livesInSet(London_London_1, Person_Prof_Emil_Schmitt);

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		livesInSet(London_London_1, Person_Melanie_Morgan);
		livesInSet(London_London_1, Person_Sid_Palmer);
	}

	gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);

	GetScene(SCENE_FAHNDUNG)->Geschehen = 0;    /* damit nicht gleich Burglary 2 geschieht */
	_sceneArgs._returnValue = GetLocScene(7)->EventNr;    /* Polizei */
}

void tcDoneMattIsArrested() {
	StopAnim();

	gfxShow(169, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1); /* prison */

	AddVTime(1439);

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

	hasSet(Person_Mary_Bolton, Tool_Schuhe);
	hasSet(Person_Mary_Bolton, Tool_Elektrohammer);

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

	GetScene(SCENE_FAHNDUNG)->Geschehen = 0;    /* damit nicht gleich Burglary 2 geschieht */
	_sceneArgs._returnValue = GetLocScene(GetLocation)->EventNr;
}

void tcDoneDealerIsAfraid() {
	uint32 persID;

	switch (GetLocation) {
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

	Person pers = (Person) dbGetObject(persID);
	Say(STORY_0_TXT, 0, pers->PictID, "DEALER_IS_AFRAID");

	gfxChangeColors(l_gc, 5L, GFX_FADE_OUT, 0L);
}

void tcDoneRaidInWalrus() {
	Person Red = (Person) dbGetObject(Person_Red_Stanson);
	Environment Env = (Environment) dbGetObject(Environment_TheClou);

	knowsSet(Person_Matt_Stuvysunt, Person_Red_Stanson);
	sndPlaySound("gludo.bk", 0);

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
	Person Grull = (Person) dbGetObject(Person_Lucas_Grull);
	Environment Env = (Environment) dbGetObject(Environment_TheClou);

	if (!Env->MattHasIdentityCard) {
		StopAnim();

		gfxShow(169, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1); /* Knast */
		knowsSet(Person_Matt_Stuvysunt, Person_Lucas_Grull);
		Say(STORY_0_TXT, 0, Grull->PictID, "DART_GRULL_0");

		byte choice = Say(STORY_0_TXT, 0, MATT_PICTID, "DART_MATT_1");
		if (choice == 0) {
			Say(STORY_0_TXT, 0, Grull->PictID, "DART_GRULL_1");

			sndPrepareFX("darth.voc");
			sndPlayFX();

			Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "DART_JAEGER_0");

			gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);
			gfxShow(221, GFX_NO_REFRESH | GFX_ONE_STEP | GFX_BLEND_UP, 0, -1, -1);

			sndPrepareFX("darth.voc");
			sndPlayFX();

			Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "DART_JAEGER_1");

			sndPlaySound("end.bk", 0);
			tcMattGoesTo(60);
			Say(STORY_0_TXT, 0, 155, "THE_END_MONASTERY");  /* pict = holy matt */

			StopAnim();
			gfxChangeColors(l_gc, 3L, GFX_FADE_OUT, 0L);

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
	Person Gludo = (Person) dbGetObject(Person_John_Gludo);

	StopAnim();

	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_1");
	Say(STORY_0_TXT, 0, MATT_PICTID, "5TH_MATT_0");
	Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_0");

	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_2");
	Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_1");
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_3");

	Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_2");

	sndPrepareFX("brille.voc");
	gfxShow(162, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);
	sndPlayFX();

	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_4");
	Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_3");
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_5");

	Say(STORY_0_TXT, 0, MATT_PICTID, "5TH_MATT_1");
	Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_4");
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_6");

	sndPrepareFX("streich.voc");
	sndPlayFX();

	gfxShow(153, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);

	Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_5");
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_7");

	gfxChangeColors(l_gc, 5, GFX_FADE_OUT, 0);

	_sceneArgs._returnValue = SCENE_WATLING;
}

void tcDoneBeautifullMorning() {
	tcAsTimeGoesBy(GetMinute + 187);

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
	Person Sabien = (Person)dbGetObject(Person_Sabien_Pardo);

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
	Person Sabien = (Person)dbGetObject(Person_Sabien_Pardo);

	sndPlaySound("sabien.bk", 0);
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
	Environment Env = (Environment)dbGetObject(Environment_TheClou);

	Env->Present = 1;
	sndPlaySound("sabien.bk", 0);

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_5_OLD_0");

	_sceneArgs._returnValue = SCENE_WALRUS;
}

void tcRainyEvening() {
	Person Briggs = (Person)dbGetObject(Person_Herbert_Briggs);

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

	GetScene(SCENE_FAHNDUNG)->Geschehen = 0;    /* damit nicht gleich Burglary 2 geschieht */
	_sceneArgs._returnValue = GetLocScene(GetLocation)->EventNr;
}

void tcPoliceInfoTower() {
	Building tower = (Building)dbGetObject(Building_Tower_of_London);
	Environment Env = (Environment)dbGetObject(Environment_TheClou);

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
	Person Ben = (Person)dbGetObject(Person_Ben_Riggley);
	Environment Env = (Environment)dbGetObject(Environment_TheClou);

	Env->Present = 1;

	Say(STORY_1_TXT, 0, Ben->PictID, "ST_6_BEN_0");
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_6_OLD_0");

	_sceneArgs._returnValue = SCENE_HOTEL_REC;
}

void tcSabienDinner() {
	tcAsTimeGoesBy(1210);
	sndPlaySound("sabien.bk", 0);

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_4_OLD_0");
	tcMattGoesTo(62);

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_4_OLD_1");
	tcAsTimeGoesBy(GetMinute + 160);

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

	GetScene(SCENE_FAHNDUNG)->Geschehen = 0;    /* damit nicht gleich Burglary 2 geschieht */
	_sceneArgs._returnValue = GetLocScene(GetLocation)->EventNr;
}

void tcDoneBirthday() {
	StopAnim();
	gfxShow(172, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);

	sndPrepareFX("birthd2.voc");    /* applause */
	sndPlayFX();

	knowsAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST, Object_Person);
	List *persons = ObjectListPrivate;

	for (ObjectNode *n = (ObjectNode *) LIST_HEAD(persons); NODE_SUCC(n); n = (ObjectNode *) NODE_SUCC(n)) {
		Person p = (Person)dbGetObject(OL_NR(n));

		switch (OL_NR(n)) {
		case Person_Sabien_Pardo:
		case Person_Herbert_Briggs:
		case Person_John_Gludo:
		case Person_Miles_Chickenwing:
		/*case Person_Old_Matt:*/
		case Person_Red_Stanson:
			break;
		default:
			if (livesIn(London_London_1, OL_NR(n)) && (g_clue->calcRandomNr(0, 10) < 7)) {
				tcMoveAPerson(OL_NR(n), Location_Walrus);

				Say(STORY_1_TXT, 0, p->PictID, "ST_11_ALL_0");
			}
			break;
		}
	}

	sndPrepareFX("birthd1.voc");    /* cork popping */
	sndPlayFX();

	RemoveList(persons);

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_11_OLD_0");
	gfxShow(141, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);

	_sceneArgs._returnValue = SCENE_WALRUS;
}

void tcWalkWithSabien() {
	Environment Env = (Environment)dbGetObject(Environment_TheClou);

	sndPlaySound("sabien.bk", 0);

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

	GetScene(SCENE_FAHNDUNG)->Geschehen = 0;    /* damit nicht gleich Burglary 2 geschieht */
	_sceneArgs._returnValue = GetLocScene(GetLocation)->EventNr;
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
	Environment Env = (Environment)dbGetObject(Environment_TheClou);

	SetMinute(540);

	if (has(Person_Matt_Stuvysunt, Loot_Koffer)) {
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
	SetEnabledChoices(GO | WAIT);

	GetScene(SCENE_FAHNDUNG)->Geschehen = 0;    /* damit nicht gleich Burglary 2 geschieht */
	_sceneArgs._returnValue = GetLocScene(GetLocation)->EventNr;
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

	sndPrepareFX("explosio.voc");
	sndPlayFX();

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
	Person Sabien = (Person)dbGetObject(Person_Sabien_Pardo);
	Environment Env = (Environment)dbGetObject(Environment_TheClou);

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

		sndPlaySound("sabien.bk", 0);

		Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_19_OLD_0");

		/* hier eventuell glückliche Anim zeigen! */
		gfxShow(164, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);  /*  The End */

		inpWaitFor(INP_LBUTTONP);
		gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);

		_sceneArgs._returnValue = SCENE_NEW_GAME;
	} else {            /* bleibt nicht bei Sabien! */

		AddVTime(2713);     /* hier nicht ausblenden? */

		gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);

		_sceneArgs._returnValue = SCENE_SOUTHHAMPTON;

		SetLocation(-1);    /* damit sicher eingeblendet wird! */
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
	List *menu = g_clue->_txtMgr->goKey(MENU_TXT, "SouthhamptonMenu");
	Environment Env = (Environment)dbGetObject(Environment_TheClou);
	Person Herb = (Person)dbGetObject(Person_Herbert_Briggs);

	tcAsDaysGoBy(713518L, 30);
	uint32 startTime = GetDay * 1440 + GetMinute;

	byte ende = 0;
	byte activ = 0;
	while (!ende) {
		inpTurnFunctionKey(0);
		inpTurnESC(0);

		activ = Menu(menu, 46, activ, NULL, 0L);
		inpTurnESC(1);
		inpTurnFunctionKey(1);

		uint32 actionTime = g_clue->calcRandomNr(180, 300);    /* 2 bis 5 Stunden */

		switch (activ) {
		case 1:
			tcAsTimeGoesBy(GetMinute + actionTime);
			break;      /* spazieren */
		case 2:
			tcAsTimeGoesBy(GetMinute + actionTime);
			break;      /* warten  */
		case 3:
			tcAsTimeGoesBy(GetMinute + actionTime);
			break;      /* fischen */
		case 5:
			Information();
			ShowTime(0);
			break;
		default:
			break;
		}

		if (((GetDay * 1440 + GetMinute) > (startTime + 420)) && (!(Env->SouthhamptonHappened & 1))) {  /* nach 7 Stunden */
			Env->SouthhamptonHappened |= 1;

			Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_21_OLD");  /* Kummer */
		}

		if (((GetDay * 1440 + GetMinute) > (startTime + 1020)) && (!(Env->SouthhamptonHappened & 2))) { /* nach 17 Stunden */
			Env->SouthhamptonHappened |= 2;

			SetBubbleType(THINK_BUBBLE);

			if (Env->MattIsInLove)
				Say(STORY_1_TXT, 0, MATT_PICTID, "ST_22_MATT"); /* Neugier */
			else
				Say(STORY_1_TXT, 0, MATT_PICTID, "ST_22O_MATT");
		}

		if (((GetDay * 1440 + GetMinute) > (startTime + 1500)) && (!(Env->SouthhamptonHappened & 4))) { /* nach 25 Stunden */
			Env->SouthhamptonHappened |= 4;

			if (Env->MattIsInLove)
				Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_23_MATT"); /* Neugier */
			else
				Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_23O_MATT");
		}

		if (((GetDay * 1440 + GetMinute) > (startTime + 2100)) && (!(Env->SouthhamptonHappened & 8))) {
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

			ende = 1;
		}

		ShowTime(0);
	}

	RemoveList(menu);
}

void tcDoneSouthhampton() {
	List *menu = g_clue->_txtMgr->goKey(MENU_TXT, "SouthhamptonMenu");
	Environment Env = (Environment)dbGetObject(Environment_TheClou);

	_sceneArgs._overwritten = true;
	_sceneArgs._returnValue = 0;  /* MUß SEIN! */

	SetEnabledChoices(GP_ALL_CHOICES_ENABLED);

	AddVTime(g_clue->calcRandomNr(560, 830));
	ShowTime(0);

	if (Env->FirstTimeInSouth) {
		Env->FirstTimeInSouth = 0;
		tcDoneFirstTimeLonelyInSouth();
		tcInitTowerBurglary();  /* wird absichtlich X mal aufgerufen - siehe weiter unten! */
	}

	/* mit Gehen oder Planen kommt man aus dem Menü raus */
	byte activ = 1;     /* !! */
	while ((activ != 0) && (_sceneArgs._returnValue == 0)) {
		inpTurnESC(0);
		inpTurnFunctionKey(1);
		activ = Menu(menu, 127, activ, NULL, 0L);
		inpTurnFunctionKey(0);
		inpTurnESC(1);

		if (activ == (byte) - 1) {
			ShowTheClouRequester(No_Error);
			_sceneArgs._returnValue =
			    ((Player) dbGetObject(Player_Player_1))->CurrScene;

			activ = 1;      /* nicht 0! */
		} else {
			uint32 actionTime = g_clue->calcRandomNr(180, 300);    /* 2 bis 5 Stunden */

			switch (activ) {
			case 0:
				break;

			case 1:
				tcAsTimeGoesBy(GetMinute + actionTime);
				break;      /* spazieren */
			case 2:
				tcAsTimeGoesBy(GetMinute + actionTime);
				break;      /* warten  */
			case 3:
				tcAsTimeGoesBy(GetMinute + actionTime);
				break;      /* fischen */
			case 4:
				sndPlaySound("hotel.bk", 0);
				tcInitTowerBurglary();  /* just to be sure */
				plPlaner(Building_Tower_of_London);
				RefreshCurrScene();
				break;
			case 5:
				Information();
				ShowTime(0);
				inpTurnFunctionKey(1);
				break;
			case 6:
				if (tcDoTowerBurglary())
					_sceneArgs._returnValue = SCENE_KASERNE_OUTSIDE;
				else
					RefreshCurrScene();
				break;
			/* Vorsicht bei Erweiterung -> Speichern weiter oben */
			default:
				break;
			}

			ShowTime(0);
		}
	}

	if (!activ) {
		AddVTime(g_clue->calcRandomNr(560, 830));
		_sceneArgs._returnValue = SCENE_TOWER_OUT;
	}

	StopAnim();
	gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);

	RemoveList(menu);
}

void tcInitTowerBurglary() {
	Car car = (Car)dbGetObject(Car_Cadillac_Club_1952);
	Player player = (Player)dbGetObject(Player_Player_1);

	/* alle Personen entfernen! */
	player->MattsPart = 25;

	joined_byAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Person);

	for (Node *node = (Node *) LIST_HEAD(ObjectList); NODE_SUCC(node);
	        node = (Node *) NODE_SUCC(node))
		joined_byUnSet(Person_Matt_Stuvysunt, OL_NR(node));

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

	if (hasGet(Person_Matt_Stuvysunt, Ability_Schloesser) < 210)
		hasSetP(Person_Matt_Stuvysunt, Ability_Schloesser, 210);

	if (hasGet(Person_Matt_Stuvysunt, Ability_Elektronik) < 251)
		hasSetP(Person_Matt_Stuvysunt, Ability_Elektronik, 251);

	hasSetP(Person_Marc_Smith, Ability_Autos, 255);
	hasSetP(Person_Mohammed_Abdula, Ability_Kampf, 255);

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
	int32 burglary = plPlayer(Building_Tower_of_London, 0, NULL);

	if (burglary) {
		if (has(Person_Matt_Stuvysunt, Loot_Kronjuwelen))
			Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_25_OLD_0");
		else
			Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_25O_OLD_0");

		tcDoneMafia();
	}

	return burglary;
}

void tcDoneMafia() {
	Person Ken = (Person)dbGetObject(Person_Ken_Addison);
	Person Briggs = (Person)dbGetObject(Person_Herbert_Briggs);

	CurrentBackground = BGD_LONDON;
	ShowMenuBackground();

	gfxShow(168, GFX_NO_REFRESH | GFX_ONE_STEP | GFX_CLEAR_FIRST | GFX_FADE_OUT | GFX_BLEND_UP, 3, -1, -1); /* Mafia */

	Say(STORY_1_TXT, 0, Ken->PictID, "ST_25_PATE_0");

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_25_OLD_1");
	Say(STORY_1_TXT, 0, Briggs->PictID, "ST_25_BRIGGS_0");

	Say(STORY_1_TXT, 0, MATT_PICTID, "ST_25_MATT_0");
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_25_OLD_2");

	sndPlaySound("street1.bk", 0);

	gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);
	tcAsTimeGoesBy(GetMinute + 420);

	tcMattGoesTo(25);       /* Villa */

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_26_OLD_0");
	Say(STORY_1_TXT, 0, Ken->PictID, "ST_26_PATE_0");
	Say(STORY_1_TXT, 0, Ken->PictID, "KEN_PLAN");

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_26_OLD_1");
}

void tcDoneKaserne() {
	Environment Env = (Environment)dbGetObject(Environment_TheClou);
	Car car = (Car)dbGetObject(Car_Cadillac_Club_1952);
	List *menu = g_clue->_txtMgr->goKey(MENU_TXT, "KaserneMenu");

	joined_bySet(Person_Matt_Stuvysunt, Person_Matt_Stuvysunt);
	joined_bySet(Person_Matt_Stuvysunt, Person_Herbert_Briggs);
	joined_bySet(Person_Matt_Stuvysunt, Person_Marc_Smith);
	joined_bySet(Person_Matt_Stuvysunt, Person_Ken_Addison);

	joined_byUnSet(Person_Matt_Stuvysunt, Person_Mohammed_Abdula);

	joinSet(Person_Matt_Stuvysunt, Person_Ken_Addison);
	knowsSet(Person_Matt_Stuvysunt, Person_Ken_Addison);

	((Building) dbGetObject(Building_Starford_Kaserne))->Exactlyness = 255;

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
		inpTurnESC(0);
		activ = Menu(menu, 15, activ, NULL, 0L);
		inpTurnESC(1);

		/* change possibilites in PatchStory too! */
		if (activ == (byte) - 1) {
			ShowTheClouRequester(No_Error);
			successor = ((Player) dbGetObject(Player_Player_1))->CurrScene;

			activ = 1;
		} else {
			switch (activ) {
			case 0:
				StopAnim();
				gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);
				SetLocation(65);
				_sceneArgs._options = 265;
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
				if (!(burglary = plPlayer(Building_Starford_Kaserne, 0, NULL)))
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
			sndPlaySound("end.bk", 0);

			if (Env->MattIsInLove)
				Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_27_OLD_0");
			else
				Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_28_OLD_0");

			gfxShow(224,
			        GFX_NO_REFRESH | GFX_ONE_STEP | GFX_FADE_OUT | GFX_BLEND_UP,
			        3, -1, -1);
			Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_29_OLD_0");
		} else {
			/* sndPlaySound() in tcDonePrison() */

			Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_28_OLD_1");
			tcDonePrison();
		}

		successor = SCENE_NEW_GAME;
	}

	gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);
	RemoveList(menu);

	_sceneArgs._returnValue = successor;
}

int32 tcIsLastBurglaryOk() {
	Building kaserne = (Building)dbGetObject(Building_Starford_Kaserne);
	int16 carXPos0 = kaserne->CarXPos - 40;
	int16 carYPos0 = kaserne->CarYPos - 40;
	int16 carXPos1 = kaserne->CarXPos + 40;
	int16 carYPos1 = kaserne->CarYPos + 40;
	LSObject left = (LSObject)dbGetObject(tcLAST_BURGLARY_LEFT_CTRL_OBJ);
	LSObject right = (LSObject)dbGetObject(tcLAST_BURGLARY_RIGHT_CTRL_OBJ);

	/* Links muß ein, Rechts muß ausgeschalten sein */
	/* 1.. OFF!    */

	bool madeIt = true;
	if ((left->ul_Status & (1 << Const_tcON_OFF)))
		madeIt = false;

	if (!(right->ul_Status & (1 << Const_tcON_OFF)))
		madeIt = false;

	if (!has(Person_Matt_Stuvysunt, Loot_Dokument))
		madeIt = false;

	/* alle anderen müssen beim Auto sein... */
	int32 i;
	for (i = 1; i < 4; i++)
		if ((Search.GuyXPos[i] < carXPos0) || (Search.GuyXPos[i] > carXPos1) ||
		        (Search.GuyYPos[i] < carYPos0) || (Search.GuyYPos[i] > carYPos1))
			madeIt = false;
	
	// CHECKME: This looks absolutely wrong. i is equal to 4, thus it's our of bounds.
	if ((Search.GuyXPos[0] >= carXPos0) && (Search.GuyXPos[0] <= carXPos1) &&
	        (Search.GuyYPos[0] >= carYPos0) && (Search.GuyYPos[i] <= carYPos1))
		madeIt = false;

	return madeIt;
}

} // End of namespace Clue

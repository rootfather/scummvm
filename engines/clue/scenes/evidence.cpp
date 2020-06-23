/*
**  $Filename: scenes/evidence.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-02-94
**
**   functions for evidence for "Der Clou!"
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

#include "clue/scenes/evidence.h"
#include "clue/clue.h"
#include "clue/present/interac.h"
#include "clue/data/dataappl.h"
#include "clue/organisa/organisa.h"
#include "clue/sound/newsound.h"
#include "clue/scenes/scenes.h"

namespace Clue {

bool tcPersonWanted(uint32 persId);
bool tcPersonQuestioning(PersonNode *person);

struct Search Search;

static bool tcCarFound(CarNode *car, uint32 time) {
	PersonNode *john = (PersonNode *)dbGetObject(Person_John_Gludo);
	PersonNode *miles = (PersonNode *)dbGetObject(Person_Miles_Chickenwing);

	bool found = false;

	/* Der Jaguar darf nicht gefunden werden - sonst könnte er ja */
	/* nicht explodieren! */

	if (car != dbGetObject(Car_Jaguar_XK_1950)) {
		if (tcIsCarRecognised(car, time)) { /* Wagen wird erkannt! */
			Say(BUSINESS_TXT, 0, john->PictID, "CAR_RECOG");

			int32 hours = g_clue->calcRandomNr(2, 5);
			int32 i = 0;

			while ((i++) < hours) {
				addVTime(60);
				inpDelay(35);
				ShowTime(2);
			}

			if (!tcIsCarRecognised(car, time)) {    /* Wagen wird nicht gefunden */
				Say(BUSINESS_TXT, 0, john->PictID, "CAR_NOT_FOUND");
				car->Strike = g_clue->calcRandomNr(200, 255);
			} else {        /* Wagen wird gefunden! */
				found = true;
				Say(BUSINESS_TXT, 0, john->PictID, "CAR_FOUND");
				Say(BUSINESS_TXT, 0, miles->PictID, "GUTE_ARBEIT");
				UnSet(dbGetObject(Person_Matt_Stuvysunt), Relation_has, car);
			}
		}
	}

	return found;
}

static bool tcATraitor(uint32 traitorId) {
	NewList<NewNode> *bubble = g_clue->_txtMgr->goKey(BUSINESS_TXT, "A_TRAITOR");
	NewList<NewNode> *newList = new NewList<NewNode>;
	PersonNode *john = (PersonNode *)dbGetObject(Person_John_Gludo);

	Common::String name = dbGetObjectName(traitorId);
	Common::String line = Common::String::format(bubble->getListHead()->_name.c_str(), name.c_str());

	newList->createNode(line);
	newList->createNode(bubble->getNthNode(1)->_name);
	newList->createNode(bubble->getNthNode(2)->_name);

	SetPictID(john->PictID);
	Bubble(newList, 0, nullptr, 0);

	bubble->removeList();
	newList->removeList();

	Say(BUSINESS_TXT, 0, john->PictID, "ARRESTED");

	return true;           /* gefangen! */
}

static bool tcIsThereATraitor() {
	PlayerNode *player = (PlayerNode *)dbGetObject(Player_Player_1);
	PersonNode *matt = (PersonNode *)dbGetObject(Person_Matt_Stuvysunt);
	uint32 traitorId = 0;
	bool caught = false;

	if (player->JobOfferCount > 50 + g_clue->calcRandomNr(0, 20)) { /* ein Verrat?! */
		if (g_clue->calcRandomNr(0, 255) < matt->Popularity) {  /* Verrat ! */
			joined_byAll(Person_Matt_Stuvysunt, OLF_INCLUDE_NAME,
			             Object_Person);

			byte symp = 255;
			for (dbObjectNode *n = ObjectList->getListHead(); n->_succ; n = (dbObjectNode *)n->_succ) {
				PersonNode *pers = (PersonNode *)n;

				if (n->_nr != Person_Matt_Stuvysunt) {    /* Matt verrät sich nicht selbst */
					if ((traitorId == 0) || (pers->Known < symp)) {
						traitorId = n->_nr;
						symp = pers->Known;
					}
				}
			}

			caught = tcATraitor(traitorId);
		}
	}

	return caught;
}

bool tcStartEvidence() {
	EvidenceNode *evidence = (EvidenceNode *)dbGetObject(Evidence_Evidence_1); /* just for presentation */

	if ((!(Search.EscapeBits & FAHN_ALARM)) && (!(Search.EscapeBits & FAHN_QUIET_ALARM)))
		Say(BUSINESS_TXT, 0, ((PersonNode *) dbGetObject(Person_John_Gludo))->PictID,
		    "A_BURGLARY_SIR");

	Say(BUSINESS_TXT, 0, ((PersonNode *) dbGetObject(Person_Miles_Chickenwing))->PictID, 
		"START_EVIDENCE");

	joined_byAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST, Object_Person);

	NewObjectList<dbObjectNode> *guys = ObjectListPrivate;
	dbSortObjectList(&guys, dbStdCompareObjects);

	byte guyCount = (byte)guys->getNrOfNodes();
	NewList<NewNode> *spuren = g_clue->_txtMgr->goKey(BUSINESS_TXT, "SPUREN");

	PersonNode *p[4];
	p[0] = p[1] = p[2] = p[3] = nullptr;

	int32 guarded = ChangeAbs(((BuildingNode *) dbGetObject(Search.BuildingId))->GRate,
	                    ((BuildingNode *) dbGetObject(Search.BuildingId))->Strike / 7,
	                    0, 255);

	int32 radio = (int32)((BuildingNode *) dbGetObject(Search.BuildingId))->RadioGuarding;

	int32 MyEvidence[4][7];
	uint32 totalEvidence[7];
	uint32 shownEvidence[4];
	uint32 Recognition[4];

	uint i = 0;
	for (dbObjectNode* n = guys->getListHead(); n->_succ; n = (dbObjectNode *) n->_succ, i++) {
		int32 div = 380;

		p[i] = (PersonNode *)n;

		/* alle folgenden Werte sind zwischen 0 und 255 */

		/* statt durch 765 zu dividieren wurde ursprünglich durch 255 dividiert -> */
		/* viel zu große Werte */

		if (Search.SpotTouchCount[i])
			div = 1;

		MyEvidence[i][0] =
		    (((tcGetTrail(p[i], 0) * (int32) Search.WalkTime[i] *
		       (MAX(1, 255 - guarded))) / ((int32) Search.TimeOfBurglary +
		                                   1))) / div;
		MyEvidence[i][1] =
		    (((tcGetTrail(p[i], 1) * (int32) Search.WaitTime[i] *
		       (MAX(1, 255 - guarded))) / ((int32) Search.TimeOfBurglary +
		                                   1))) / div;
		MyEvidence[i][2] =
		    (((tcGetTrail(p[i], 2) * (int32) Search.WorkTime[i] *
		       (MAX(1, 255 - guarded))) / ((int32) Search.TimeOfBurglary +
		                                   1))) / div;
		MyEvidence[i][3] =
		    (((tcGetTrail(p[i], 3) * (int32) Search.KillTime[i] *
		       (MAX(1, 255 - guarded))) / ((int32) Search.TimeOfBurglary + 1)));
		MyEvidence[i][4] =
		    ChangeAbs(0, (int32) Search.CallCount * (int32) radio / 5, 0, 255);
		MyEvidence[i][5] =
		    (p[i]->KnownToPolice * (MAX(1, guarded))) / (div * 3);
		MyEvidence[i][6] =
		    ChangeAbs(0, (int32)g_clue->calcRandomNr(200, 255) * (int32) Search.SpotTouchCount[i], 0, 255);

		for (uint32 j = 0; j < 7; j++) /* jeden Betrag != 0 AUFRUNDEN auf 1% ! */
			if (MyEvidence[i][j])
				MyEvidence[i][j] = MAX(MyEvidence[i][j], 5);

		if (has(Person_Matt_Stuvysunt, Tool_Mask))
			MyEvidence[i][6] = (MyEvidence[i][6] * 2) / 3;

		/* im Fluchtfall viele Gehspuren! */
		if (Search.EscapeBits & FAHN_ESCAPE)
			MyEvidence[i][0] =
			    CalcValue(MyEvidence[i][0], g_clue->calcRandomNr(80, 120), 255, 255,
			              50);

		totalEvidence[i] =
		    MyEvidence[i][0] + MyEvidence[i][1] + MyEvidence[i][2] +
		    MyEvidence[i][3] + MyEvidence[i][4] + MyEvidence[i][5] +
		    MyEvidence[i][6];

		shownEvidence[i] = Recognition[i] = 0;

		tcPersonLearns(n->_nr);
	}

	prSetBarPrefs(_menuGc, 300, 12, 1, 3, 0);

	byte guyReady = 0;
	Common::String line = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "FAHNDUNG");

	bool shown = false;
	while (guyReady != ((1 << guyCount) - 1)) {
		if (shown) {
			ShowTime(0);
			inpDelay(35);
			addVTime(g_clue->calcRandomNr(1, 11));

			shown = false;
		}

		byte guyNr = g_clue->calcRandomNr(0, guyCount);

		/* wer ist den noch nicht fertig? */
		while ((1 << guyNr) & guyReady)
			guyNr = (guyNr + 1) % guyCount;

		/* bei folgendem CalcRandomNr darf nicht 4 - guyCount stehe, sonst
		 * Division durch 0!
		 */

		/* zufällig eine Spurenart auswählen */
		byte evidenceNr = g_clue->calcRandomNr(0, 7);

		/* wenn diese Spurenart schon angzeigt wurde, eine freie
		 * Spur suchen
		 */
		uint32 j = 0;

		while (((1 << evidenceNr) & shownEvidence[guyNr]) && (j++ < 7))
			evidenceNr = (evidenceNr + 1) % 7;

		if (j < 8) {        /* 8 stimmt! -> ober wird anschließend noch erhöht */
			shownEvidence[guyNr] |= (1 << evidenceNr);

			if (MyEvidence[guyNr][evidenceNr] > 0) {
				ShowMenuBackground();
				PrintStatus(spuren->getNthNode(evidenceNr)->_name);

				Recognition[guyNr] += MyEvidence[guyNr][evidenceNr];

				evidence->Recognition = Recognition[guyNr] / 3; /* change also: totalEvidence /= 3.... */
				evidence->pers = guys->getNthNode((uint32) guyNr)->_nr;

				/* für alle "Evidences" - stimmt so, da für alle */
				/* Personen die selbe Evidence Struct benutzt wird -> */
				/* bestimmte Werte sind von vorher gesetzt und müssen gelöscht */
				/* werden */

				evidence->WalkTrail = 0;
				evidence->WaitTrail = 0;
				evidence->WorkTrail = 0;
				evidence->KillTrail = 0;
				evidence->CallTrail = 0;
				evidence->PaperTrail = 0;
				evidence->FotoTrail = 0;

				if (shownEvidence[guyNr] & 1)
					evidence->WalkTrail = MyEvidence[guyNr][0];

				if (shownEvidence[guyNr] & 2)
					evidence->WaitTrail = MyEvidence[guyNr][1];

				if (shownEvidence[guyNr] & 4)
					evidence->WorkTrail = MyEvidence[guyNr][2];

				if (shownEvidence[guyNr] & 8)
					evidence->KillTrail = MyEvidence[guyNr][3];

				if (shownEvidence[guyNr] & 16)
					evidence->CallTrail = MyEvidence[guyNr][4];

				if (shownEvidence[guyNr] & 32)
					evidence->PaperTrail = MyEvidence[guyNr][5];

				if (shownEvidence[guyNr] & 64)
					evidence->FotoTrail = MyEvidence[guyNr][6];

				Present(Evidence_Evidence_1, "Fahndung", InitEvidencePresent);

				ShowMenuBackground();
				PrintStatus(line);

				shown = true;
			}
		}

		if (shownEvidence[guyNr] == ((1 << 7) - 1))
			guyReady |= (1 << guyNr);
	}

	/* ein gewisses Restwissen bleibt der Polizei ! */
	bool caught = false;
	for (i = 0; i < guyCount; i++) {
		totalEvidence[i] /= 3;  /* change als in recognition = ... */

		if (p[i] != 0) {
			if (totalEvidence[i] > 255)
				totalEvidence[i] = 255;

			p[i]->KnownToPolice = (byte)(totalEvidence[i]);

			if (p[i]->KnownToPolice > tcPERSON_IS_ARRESTED)
				caught |= tcPersonWanted(guys->getNthNode(i)->_nr);

			if (!caught) {
				if (p[i] == dbGetObject(Person_Robert_Bull))
					caught |= tcATraitor(Person_Robert_Bull);
			}

			if (!caught) {
				if (p[i] == dbGetObject(Person_Marc_Smith))
					caught |= tcATraitor(Person_Marc_Smith);
			}

			if ((g_clue->getFeatures() & GF_PROFIDISK)  && !caught) {
				if (p[i] == dbGetObject(Person_Phil_Ciggy))
					caught |= tcATraitor(Person_Phil_Ciggy);
			}
		}
	}

	caught |= tcIsThereATraitor();

	if (!(tcCarFound((CarNode *) dbGetObject(Organisation.CarID),
	          Search.TimeOfBurglary - Search.TimeOfAlarm))) {
		CarNode *car = (CarNode *)dbGetObject(Organisation.CarID);
		int32 newStrike = CalcValue((int32) car->Strike, 0, 255, 255, 15);
		if (newStrike < (car->Strike + 40))
			newStrike = ChangeAbs((int32) car->Strike, 40, 0, 255);

		car->Strike = newStrike;
	}

	((PlayerNode *) dbGetObject(Player_Player_1))->MattsPart = (byte) tcCalcMattsPart();
	tcForgetGuys();

	spuren->removeList();
	guys->removeList();

	return caught;
}

void tcForgetGuys() {
	joined_byAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST, Object_Person);
	NewList<dbObjectNode> *guys = ObjectListPrivate;

	for (dbObjectNode *node = guys->getListHead(); node->_succ; node = (dbObjectNode *)node->_succ) {
		if (node->_nr != Person_Matt_Stuvysunt) {
			PersonNode *pers = (PersonNode *)node;

			pers->TalkBits |= (1 << Const_tcTALK_JOB_OFFER);    /* über Jobs kann man wieder reden! */

			joined_byUnSet(Person_Matt_Stuvysunt, node->_nr);
			joinUnSet(Person_Matt_Stuvysunt, node->_nr);
			rememberUnSet(Person_Matt_Stuvysunt, node->_nr);
		}
	}

	guys->removeList();
}

bool tcPersonWanted(uint32 persId) {
	PersonNode *john = (PersonNode *)dbGetObject(Person_John_Gludo);
	PersonNode *miles = (PersonNode *)dbGetObject(Person_Miles_Chickenwing);
	NewList<NewNode> *jobs = g_clue->_txtMgr->goKey(OBJECTS_ENUM_TXT, "enum_JobE");

	Common::String name = dbGetObjectName(persId);

	NewList<NewNode> *bubble = g_clue->_txtMgr->goKey(BUSINESS_TXT, "BURGLAR_RECOG");

	Common::String line = Common::String::format("%s %s.", bubble->getNthNode(3)->_name.c_str(), name.c_str());

	bubble->removeNode(bubble->getNthNode(3)->_name);
	bubble->createNode(line);

	SetPictID(john->PictID);
	Bubble(bubble, 0, nullptr, 0);
	bubble->removeList();

	Say(BUSINESS_TXT, 0, miles->PictID, "ARREST_HIM");
	livesInUnSet(London_London_1, persId);
	tcMoveAPerson(persId, Location_Nirvana);

	uint32 hours = g_clue->calcRandomNr(4, 7);

	uint32 i = 0;
	while ((i++) < hours) {
		addVTime(60);
		inpDelay(35);
		ShowTime(2);
	}

	bool caught = false;
	if (tcGuyCanEscape((PersonNode *)dbGetObject(persId)) > g_clue->calcRandomNr(100, 255)) { /* Flucht gelingt */
		Say(BUSINESS_TXT, 0, john->PictID, "ESCAPED");
		livesInSet(London_Escape, persId);
	} else {            /* nicht */
		Say(BUSINESS_TXT, 0, john->PictID, "ARRESTED");
		livesInSet(London_Jail, persId);
		caught = tcPersonQuestioning((PersonNode *)dbGetObject(persId));
	}

	jobs->removeList();

	return caught;
}

bool tcPersonQuestioning(PersonNode *person) {
	bool caught = false;
	PersonNode *john = (PersonNode *)dbGetObject(Person_John_Gludo);
	PersonNode *miles = (PersonNode *)dbGetObject(Person_Miles_Chickenwing);

	if (person != dbGetObject(Person_Matt_Stuvysunt)) {
		if (tcGuyTellsAll(person) > g_clue->calcRandomNr(0, 180)) { /* er spricht */
			Say(BUSINESS_TXT, 0, john->PictID, "ER_GESTEHT");
			Say(BUSINESS_TXT, 0, miles->PictID, "GUTE_ARBEIT");
			caught = true;
		} else          /* er spricht nicht */
			Say(BUSINESS_TXT, 0, john->PictID, "ER_GESTEHT_NICHT");
	} else {
		Say(BUSINESS_TXT, 0, john->PictID, "ER_GESTEHT_NICHT");
		caught = true;
	}

	return caught;
}

int32 tcEscapeFromBuilding(uint32 escBits) {
	PersonNode *gludo = (PersonNode *)dbGetObject(Person_John_Gludo);
	byte escapeSucc = FAHN_NOT_ESCAPED;
	int32 timeLeft = INT_MAX;

	/* Fluchtbilder zeigen! */
	livSetAllInvisible();
	lsSetDarkness(((LSAreaNode *) dbGetObject(lsGetActivAreaID()))->uch_Darkness);

	if (escBits & FAHN_ESCAPE) {
		gfxShow(213, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);
		inpDelay(80);

		gfxShow(215, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);
		inpDelay(80);
	}

	/*
	 * Behandlung von "Umstellt"
	 * sowie Berechnung eines Fluchtversuches (in jedem Flucht - fall)
	 */

	if (!(escBits & FAHN_SURROUNDED)) {
		if ((escBits & FAHN_ALARM) || (escBits & FAHN_QUIET_ALARM)) {
			BuildingNode *build = (BuildingNode *)dbGetObject(Search.BuildingId);

			timeLeft =
			    build->PoliceTime - (Search.TimeOfBurglary -
			                         Search.TimeOfAlarm);
			timeLeft -= tcCalcEscapeTime();

			if (timeLeft > 0)
				escapeSucc = FAHN_ESCAPED;
		} else
			escapeSucc = FAHN_ESCAPED;
	}

	if (escapeSucc == FAHN_ESCAPED) {
		gfxShow(211, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);
		inpDelay(120);
	} else {
		Say(BUSINESS_TXT, 0, gludo->PictID, "UMSTELLT");    /* noch ein Umstellt */
		timeLeft = 0;       /* wenn Zeit nicht gereicht hat */
	}

	return timeLeft;
}

int32 tcEscapeByCar(uint32 escBits, int32 timeLeft) {
	PersonNode *gludo = (PersonNode *)dbGetObject(Person_John_Gludo);
	PersonNode *miles = (PersonNode *)dbGetObject(Person_Miles_Chickenwing);

	byte escapeSucc;
	if (timeLeft > 0)
		escapeSucc = FAHN_ESCAPED;
	else
		escapeSucc = FAHN_NOT_ESCAPED;

	/* Flucht per Auto! */
	if (escapeSucc == FAHN_ESCAPED) {
		if ((Search.BuildingId != Building_Tower_of_London) &&
		        (Search.BuildingId != Building_Starford_Barracks))
			escapeSucc = tcCalcCarEscape(timeLeft);
	} else
		tcMattGoesTo(7);    /* Polizei */

	/* Ausgabe : Flucht gelungen, Flucht nicht gelungen */
	if (escapeSucc == FAHN_ESCAPED) {
		if ((escBits & FAHN_ALARM) || (escBits & FAHN_QUIET_ALARM)) {
			Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_0");   /* entkommen!  */
			Say(BUSINESS_TXT, 0, miles->PictID, "HINWEIS_2");   /* woher Hinweis? */
		} else
			Say(BUSINESS_TXT, 0, OLD_MATT_PICTID, "I_ESCAPED"); /* no problems */
	} else {
		Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_1");   /* geschnappt! */
		Say(BUSINESS_TXT, 0, miles->PictID, "HINWEIS_2");   /* woher Hinweis? */
	}

	/* Gludo erzählt woher Hinweise stammten (falls es welche gibt) */
	if (escBits & FAHN_ALARM) {
		if (escBits & FAHN_ALARM_ALARM)
			Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_6");   /* Alarmanlage */

		if (escBits & FAHN_ALARM_POWER)
			Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_8");

		if (escBits & FAHN_ALARM_TIMER)
			Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_6");   /* Alarmanlage */

		if (escBits & FAHN_ALARM_MICRO)
			Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_9");

		if (escBits & FAHN_ALARM_TIMECLOCK)
			Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_7");   /* Alarmanlage */

		if (escBits & FAHN_ALARM_GUARD)
			Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_10");
	} else {
		if (escBits & FAHN_QUIET_ALARM) {
			if (escBits & FAHN_ALARM_RADIO)
				Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_5");

			if (escBits & FAHN_ALARM_PATRO)
				Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_4");

			if (escBits & FAHN_ALARM_LOUDN)
				Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_3");
		}
	}

	if (escapeSucc != FAHN_ESCAPED)
		g_clue->_animMgr->stopAnim();

	return ((int32) escapeSucc);
}

int32 tcCalcCarEscape(int32 timeLeft) {
	byte kmhWeight[4] = { 32, 44, 60, 67 };
	byte psWeight[4] = { 68, 56, 40, 33 };
	byte driverWeight[4] = { 50, 40, 25, 20 };
	byte policeSpeed[4] = { 90, 95, 103, 107 };
	CarNode *car = (CarNode *)dbGetObject(Organisation.CarID);
	BuildingNode *build = (BuildingNode *)dbGetObject(Organisation.BuildingID);
	int32 result = FAHN_ESCAPED;

	if ((Organisation.BuildingID != Building_Tower_of_London) && (Organisation.BuildingID != Building_Starford_Barracks)) {
		int32 wayType = build->EscapeRoute;
		int32 length = build->EscapeRouteLength;

		int32 kmh = car->Speed;
		int32 ps = car->PS;

		/* Motor : pro 10% Schaden -> 5% Leistung weniger */
		kmh = CalcValue(kmh, 0, 65535, (car->MotorState) / 2, 50);
		ps = CalcValue(ps, 0, 65535, (car->MotorState) / 2, 50);

		/* pro Jahr 3% weniger */
		int32 i = (car->Speed * 3 * (tcRGetCarAge(car) + 1)) / 100;
		int32 j = (car->PS * 3 * (tcRGetCarAge(car) + 1)) / 100;

		if (kmh >= i)
			kmh -= i;
		else
			kmh = 0;

		if (ps >= j)
			ps -= j;
		else
			ps = 0;

		/* Reifen pro 10% um 2% weniger */
		i = (car->Speed * 2 * (255 - car->TyreState)) / 2500;
		j = (car->Speed * 2 * (255 - car->PS)) / 2500;

		if (kmh >= i)
			kmh -= i;
		else
			kmh = 0;

		if (ps >= j)
			ps -= j;
		else
			ps = 0;

		/* Fahrer */
		/* kann aus einem Auto um ein Viertel mehr herausholen, als eigentlich */
		/* drinnen ist! */
		i = hasGet(Organisation.DriverID, Ability_Autos);
		kmh =
		    CalcValue(kmh, 0, car->Speed + car->Speed / 4, i,
		              driverWeight[wayType]);
		ps = CalcValue(ps, 0, car->PS + car->PS / 4, i, driverWeight[wayType]);

		/* Maßzahl für Geschwindigkeit */
		/* Einheit = m pro Schleifendurchlauf */
		int32 unrealSpeed = (kmh * kmhWeight[wayType] + ps * psWeight[wayType]) / 100;

		unrealSpeed = unrealSpeed + 5 - (int32)(g_clue->calcRandomNr(0, 10));

		if (unrealSpeed <= 0)
			unrealSpeed = 5;

		policeSpeed[wayType] = policeSpeed[wayType] + 5 - g_clue->calcRandomNr(0, 10);
		policeSpeed[wayType] =
		    CalcValue(policeSpeed[wayType], 0, 255, build->GRate, 25);

		/* Vorsprung berechnen */
		/* Vorsprung ist maximal die halbe Strecke (length * 500 m) und */
		/* mindestens 2000 Meter (damit man von der Fluct etwas sieht!) */

		i = MIN(((unrealSpeed * timeLeft) / 5), length * 500);
		i = MAX(2000, i);   /* mindestens 2000 Meter */

		j = 0;
		length *= 1000;     /* Fluchtweg in Meter */

		/* Bildschirmdarstellung */
		Common::String line = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "FLUCHT");
		ShowMenuBackground();
		PrintStatus(line);

		gfxChangeColors(_lowerGc, 0, GFX_FADE_OUT, 0);
		gfxShow(car->PictID, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);

		gfxPrepareColl(GFX_COLL_PARKING);
		uint8 palette[GFX_PALETTE_SIZE];
		gfxGetPalette(GFX_COLL_PARKING, palette);

		gfxChangeColors(_lowerGc, 0, GFX_BLEND_UP, palette);
		SetCarColors(car->ColorIndex);

		_lowerGc->setPens(20, GFX_SAME_PEN, 21);
		_lowerGc->rectFill(60, 40, 60 + 199, 40 + 19);

		i = MAX(i, 60);
		j = MAX(j, 5);

		g_clue->_sndMgr->sndPrepareFX("flucht.voc");
		g_clue->_sndMgr->sndPlayFX();

		int32 YardsInFront;
		int32 xOldMatt = -1;
		int32 xOldPoli = -1;
		do {
			i += unrealSpeed;   /* KmH von Matt */
			i = MAX(i, 0);

			bool paint = false;

			if (Search.TimeOfAlarm)
				j += policeSpeed[wayType];

			YardsInFront = i - j;   /* zum Vorsprung addieren */

			/* Berechnung der Darstellung */
			int32 x = (i * 190) / length; /* Matts Car */

			if (x != xOldMatt) {
				if (xOldMatt != -1) {
					_lowerGc->setPens(20, GFX_SAME_PEN, 20);

					if ((195 - x + 1) < (198 + 195 - xOldMatt))
						_lowerGc->rectFill(60 + 195 - xOldMatt, 45, 60 + 198 - xOldMatt, 55);
				}

				_lowerGc->setPens(11, GFX_SAME_PEN, 11);
				_lowerGc->rectFill(60 + 195 - x, 45, 60 + 198 - x, 55);

				xOldMatt = x;
				paint = true;
			}

			if (Search.TimeOfAlarm) {   /* Police Car */
				x = (j * 190) / length;

				if (x != xOldPoli) {
					if (xOldPoli != -1) {
						_lowerGc->setPens(20, GFX_SAME_PEN, 20);
						_lowerGc->rectFill(60 + 195 - xOldPoli, 45, 60 + 198 - xOldPoli, 55);
					}

					_lowerGc->setPens(23, GFX_SAME_PEN, 23);
					_lowerGc->rectFill(60 + 195 - x, 45, 60 + 198 - x, 55);

					paint = true;
				}

				xOldPoli = x;
			}

			if (paint)
				inpDelay(3);
		} while ((i < length) && (YardsInFront > 50));

		if (YardsInFront > 50)
			result = FAHN_ESCAPED;
		else
			result = FAHN_NOT_ESCAPED;
	}

	return result;
}

} // End of namespace Clue

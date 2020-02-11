/*
 * DataAppl.c
 * (c) 1993 by Helmut Gaberschek & Kaweh Kazemi, ...and avoid panic by
 * All rights reserved.
 *
 * Rev   Date        Comment
 *  1   05-09-93     GetObjNrOfLocation
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "clue/data/dataappl.h"
#include <assert.h>
#include "clue/clue.h"

namespace Clue {

static int32 tcGetWeightOfNerves(int32 teamMood);
static bool tcIsConnectedWithEnabledAlarm(uint32 lsoId);

#define tcESCAPE_MOOD         30    /* ab hier flüchtet einer ! */
#define tcWALK_LOUDNESS       20
#define tcPATROL_ALARM        5 /* ab 5 % veränderte Objekte schlägt Patrolie Alarm */

#define tcPIXEL_PER_SECCOND   4

#define tcX_HOTSPOT           8

uint8 TeamMood = 127;

bool tcSpendMoney(uint32 money, uint8 breakAnim) {
	bool enough = true;

	if (money > tcGetPlayerMoney) {
		enough = false;

		if (breakAnim)
			StopAnim();

		SetBubbleType(THINK_BUBBLE);
		Say(BUSINESS_TXT, 0, MATT_PICTID, "NO MONEY");
	} else
		tcSetPlayerMoney(tcGetPlayerMoney - money);

	return (enough);
}

/* berechnet Auswirkung eines Funkspruches
 * setzt Auswirkungen automatisch
 * pro Funkspruch sinkt Wirkung um 30 Punkte (von 255) -> ab dem 6 Spruch
 * automatisch demotivierend.
 */
void tcCalcCallValue(uint32 callNr, uint32 timer, uint32 persId) {
	int32 callCount = Search.CallCount;
	int32 callWeight = ChangeAbs(255, callCount * (-30), 0, 255);
	int32 perfect = tcIsPlanPerfect(timer), situation;
	int32 good = -15, nerves;
	Person guy;

	if (persId) {
		guy = (Person)dbGetObject(persId);
		nerves = 255 - guy->Panic;

		situation = (perfect * callWeight) / 255;

		switch (callNr) {
		case 0:     /* hervorragend! */
			if (situation > 200)
				good = situation - 200;
			break;
		case 1:     /* Streng Dich an! */
			if (situation > 80 && situation < 200)
				good = 15;
			break;
		case 2:     /* Beeile Dich sonst... */
			if ((nerves + situation > 235) && (situation < 80))
				good = nerves + situation - 235;
			break;
		case 3:     /* Beeile Dich, Du hast genug Zeit */
			if ((situation < 80) && (nerves + situation < 180))
				good = 15;
			break;
		case 4:     /* Du Trottel! */
			if ((perfect < 100) && (nerves > 200))
				good = 50;

			guy->Known = CalcValue(guy->Known, 0, 255, 0, 20);
			break;
		default:
			break;
		}

		if ((good > 0))
			guy->Known = CalcValue(guy->Known, 0, 255, 0, 6);
	} else {
		assert(0);
	}

	Search.CallValue = ChangeAbs(Search.CallValue, good, -500, 500);
}

/* Berechnet überschlagsmääßig den Ausgang der Flucht */
/* basiert alleine auf den Koordinaten */
/* vernachlässigt Stockwerke und Personen */
int32 tcCalcEscapeTime() {
	int32 time = 0, i;
	Building build = (Building)dbGetObject(Search.BuildingId);

	for (i = 0; i < 4; i++) {
		if ((Search.GuyXPos[i] != -1) && (Search.GuyYPos[i] != -1)) {
			int32 distance = abs((int32) Search.GuyXPos[i] - (int32) build->CarXPos);

			distance += abs((int32) Search.GuyYPos[i] - (int32) build->CarYPos);
			distance = CalcValue(distance, 0, 0xffff, 255, 20); /* um 20% mehr! */

			distance /= tcPIXEL_PER_SECCOND;

			time = MAX(distance, time);
		}
	}

	time /= 4;          /* Fluchtzeit kürzen - war zu lang! */

	return time;
}

/* bestimmt den Ausgang eines Kampfes */
/* Werkzeug wirkt sich nur in Geschwindigkeit und Verletzung aus */
bool tcKillTheGuard(uint32 guyId, uint32 buildingId) {
	Person p = (Person)dbGetObject(guyId);
	Building b = (Building)dbGetObject(buildingId);
	uint32 power = hasGet(guyId, Ability_Kampf);

	if (power >= b->GuardStrength)
		return true;
	else {
		p->OldHealth = p->Health;
		p->Health = CalcValue(p->Health, 0, 255, 0, 90);
	}

	return false;
}

int32 tcGetCarStrike(Car car) {
	uint32 strike = car->Strike;

	/* XXX: had to reoder & cast some stuff to stop underflow bugs... */
	strike += ((((int32) car->ColorIndex - 5) * 9 * strike) / 255);
	strike = CalcValue(strike, 0, 255, 127 + abs(127 - car->BodyWorkState), 30);

	return strike;
}

uint32 tcGuyCanEscape(Person p) {
	/* XXX: reordered some stuff just in case... */
	uint32 v = (255 + p->Intelligence - p->Panic) / 2;

	v = CalcValue(v, 0, 255, abs(255 - p->KnownToPolice), 15);
	v = CalcValue(v, 0, 255, abs(255 - p->Popularity), 25);

	return v;           /* the higher the better */
}

uint32 tcGuyTellsAll(Person p) {
	uint32 v;

	v = abs(512 - p->Loyality - p->Known) / 2;

	return v;           /* the higher the worser */
}

int32 tcGetCarTraderOffer(Car car) {
	int32 offer = tcGetCarPrice(car);
	Person marc = (Person)dbGetObject(Person_Marc_Smith);

	offer = CalcValue(offer, 1, 0xffff, 0, 25); /* 25% weniger */
	offer = CalcValue(offer, 1, 0xffff, 126 + (marc->Known / 2), 30);

	return offer;
}

#if 0
// TODO: Check and remove if unused
uint32 tcGetItemID(uint32 itemType) {
	struct dbObject *obj;
	Item item;
	short objHashValue;

	for (objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++) {
		for (obj = (struct dbObject *) LIST_HEAD(objHash[objHashValue]);
		        NODE_SUCC(obj); obj = (struct dbObject *) NODE_SUCC(obj)) {
			if (obj->type == Object_Item) {
				item = (Item)((void *)(obj + 1));

				if (item->Type == itemType)
					return (obj->nr);
			}
		}
	}

	return (0L);
}
#endif

uint32 GetObjNrOfLocation(uint32 LocNr) {
	for (short objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++) {
		for (struct dbObject *obj = (struct dbObject *) LIST_HEAD(objHash[objHashValue]);
		        NODE_SUCC(obj); obj = (struct dbObject *) NODE_SUCC(obj)) {
			if (obj->type == Object_Location) {
				Location loc = (Location)((void *)(obj + 1));

				if (loc->LocationNr == LocNr)
					return (obj->nr);
			}
		}
	}

	return (0L);
}

uint32 GetObjNrOfBuilding(uint32 LocNr) {
	struct dbObject *obj;
	Building bui;
	short objHashValue;

	for (objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++) {
		for (obj = (struct dbObject *) LIST_HEAD(objHash[objHashValue]);
		        NODE_SUCC(obj); obj = (struct dbObject *) NODE_SUCC(obj)) {
			if (obj->type == Object_Building) {
				bui = (Building)((void *)(obj + 1));

				if (bui->LocationNr == LocNr)
					return (obj->nr);
			}
		}
	}

	return (0L);
}

uint32 tcGetPersOffer(Person person, uint8 persCount) {
	uint32 persCapability = 1, mattCapability = 1;
	uint32 persID = dbGetObjectNr(person);
	uint32 assesment, offer, i;
	Person Pers;

	Pers = (Person) dbGetObject(persID);

	/* Fähigkeiten der Person */
	if ((i = hasGet(persID, Ability_Autos)) != NO_PARAMETER)
		persCapability += i;
	if ((i = hasGet(persID, Ability_Sprengstoff)) != NO_PARAMETER)
		persCapability += i;
	if ((i = hasGet(persID, Ability_Safes)) != NO_PARAMETER)
		persCapability += i;
	if ((i = hasGet(persID, Ability_Elektronik)) != NO_PARAMETER)
		persCapability += i;
	if ((i = hasGet(persID, Ability_Aufpassen)) != NO_PARAMETER)
		persCapability += i;
	if ((i = hasGet(persID, Ability_Schloesser)) != NO_PARAMETER)
		persCapability += i;

	/* Fähigkeiten von Matt */
	if ((i = hasGet(Person_Matt_Stuvysunt, Ability_Autos)) != NO_PARAMETER)
		mattCapability += i;
	if ((i =
	            hasGet(Person_Matt_Stuvysunt, Ability_Sprengstoff)) != NO_PARAMETER)
		mattCapability += i;
	if ((i = hasGet(Person_Matt_Stuvysunt, Ability_Safes)) != NO_PARAMETER)
		mattCapability += i;
	if ((i = hasGet(Person_Matt_Stuvysunt, Ability_Elektronik)) != NO_PARAMETER)
		mattCapability += i;
	if ((i = hasGet(Person_Matt_Stuvysunt, Ability_Aufpassen)) != NO_PARAMETER)
		mattCapability += i;
	if ((i = hasGet(Person_Matt_Stuvysunt, Ability_Schloesser)) != NO_PARAMETER)
		mattCapability += i;

	/* wieviel Prozent Leistung er bringt (Matt = 100 %) */
	assesment = (persCapability * 100) / mattCapability;

	/* MOD 18-02-04 HG 92 statt 100 (Marx Testspielempfehlung) */
	offer = 100 / persCount;    /* sein Anteil bei gleichen Teilen */
	offer = (offer * assesment) / 92;   /* ... gewichtet mit Fähigkeiten   */

	/* Beispiel zu oben: 2 Personen -> offer = 50, assesment = 200 */
	/* (andere Person ist "zweimal so gut") -> offer = 50 * 300 / 200 = 75 % */
	/* wieviel Prozent Habsucht (Matt = 100 %) */
	offer = CalcValue(offer, 1, 90, Pers->Avarice, 20);
	offer = CalcValue(offer, 1, 90, 255 - Pers->Known, 10);

	return (offer);
}

void tcPersonLearns(uint32 pId) {
	Person pers = (Person)dbGetObject(pId);
	int32 growth;

	/* Abilites */
	hasAll(pId, OLF_NORMAL, Object_Ability);

	for (struct ObjectNode *n = (struct ObjectNode *) LIST_HEAD(ObjectList); NODE_SUCC(n); n = (struct ObjectNode *) NODE_SUCC(n)) {
		int32 ability = hasGet(pId, OL_NR(n));    /* Jetztzustand der Fähigkeit */

		if (learned(pId, OL_NR(n))) {   /* er hat dazugelernt ! */
			int32 count = learnedGet(pId, OL_NR(n));  /* wie oft er gelernt hat ! */

			growth = 7 + count * 4;
			learnedUnSet(pId, OL_NR(n));    /* Gelerntes nicht nocheinmal lernen! */
		} else
			growth = 10;

		growth = MAX((ability / 10), growth);   /* Wachstum um das Minimum von
                         * x Punkten oder x Prozent    */

		if (ability + growth < 255)
			hasSetP(pId, OL_NR(n), ability + growth);
		else
			hasSetP(pId, OL_NR(n), 255);
	}

	/* other attributes ! */

	if (pers->OldHealth)
		pers->Health = pers->OldHealth; /* Verletzung ist kuriert */

	tcImprovePanic(pers, (1 + CalcRandomNr(4, 6))); /* 25 bis 16 % besser ! */

	/* XXX: casted some stuff just in case... */
	growth = MAX(int32((int32) pers->Known / (1 + CalcRandomNr(8, 10))), 20);
	tcImproveKnown(pers, (int32) pers->Known + ((growth * pers->Known) / 100)); /* 10 bis 12 % */
}

uint32 tcGetBuildValues(Building bui) {
	uint32 x = (255 - bui->Exactlyness) / 3;

	/* XXX: reordered some stuff just in case... */
	uint32 v = CalcValue(bui->Values, 0, 500000L + bui->Values, CalcRandomNr(0, 255),
	              x);

	return ((uint32)(Round(v, 3)));
}

/*
 * Berechnet Teamstimmung
 * zu verwenden für Anzeige der Teamstimmung
 * 0 - 255
 */

int32 tcGetTeamMood(uint32 *guyId, uint32 timer) {
	/* ptr auf 4 U32s */
	int32 team = 0;
	int8 i;
	/* Summe aus Einzelstimmungen */
	for (i = 0; (i < 4) && (guyId[i]); i++) {
		int32 mood = tcGetPersMood((dbGetObject(guyId[i])));  /* Stimmung eines Einzelnen */
		team += mood;
	}

	team /= i;
	TeamMood = CalcValue(team, 0, 255, (tcIsPlanPerfect(timer) * 20) / 35, 100);

	return ((int32) TeamMood);
}

/*
 * retourniert neuen Erschöpfungszustand eines Einbrechers
 * Aufzurufen, wenn Person Aktion durchführt
 * nur alle x Aktionsschritte aufrufen !
 */

int32 tcGuyInAction(uint32 persId, int32 exhaustion) {
	int32 state = tcGetGuyState(persId);

	if (CalcRandomNr(0, 15) == 1)
		state = (255 - state) / 90; /* Erschöpfungszuwachs = Invers von Zustand */
	else
		state = 0;

	return ChangeAbs(exhaustion, state, 0, 255);
}

/*
 * Setzt die Erschöpfungsabnahme eines Einbrechers
 * Aufzurufen, wenn Person wartet
 * nur alle x Warteschritte aufrufen !
 */

int32 tcGuyIsWaiting(uint32 persId, int32 exhaustion) {
	int32 state = tcGetGuyState(persId);

	if (CalcRandomNr(0, 4) == 1)
		state = state / 10; /* Erschöpfungsabnahme */
	else
		state = 0;

	state = MAX(10, state);

	return ChangeAbs(exhaustion, state * (-1), 0, 255);
}

/*
 * 255 : Plan wird (zeitlich) zu 100% eingehalten
 */
/* bei jeder Warnung eines Aufpasser , wird dieser Wert nach unten */
/* korriegiert */
/* Rückgabewert steigt mit Zunahme der Zeit (Einbrecher werden immer
    selbstsicherer
    udn sinkt mit der Differenz der Realität zum Plan!
    durch Funksprüche diesen Rückgabewert beeinflussssen!
*/

int32 tcIsPlanPerfect(uint32 timer) {
	int32 perfect =
	    (255 * ((int32)(timer + 1) - (int32) Search.DeriTime)) /
	    ((int32)(timer + 1));

	perfect = MAX(perfect, 0);
	perfect = ChangeAbs(perfect, Search.CallValue, 0, 255);
	perfect = ChangeAbs(perfect, Search.WarningCount * (-35), 0, 255);

	return perfect;
}

/*
 * berechnet die Spuren
 *
 */

int32 tcGetTrail(Person p, uint8 which) {
	int32 nerves = tcGetWeightOfNerves(TeamMood) / 7;
	int32 trail;

	switch (which) {
	case 0:         /* Walk */
		trail = (255 - p->Skill) / 4;
		trail = CalcValue(trail, 0, 255, 255 - p->Stamina, 20);
		trail = CalcValue(trail, 0, 255, 255 - p->Health, 10);
		break;
	case 1:         /* Wait */
		trail = (p->Panic) / 4; /* umso größer Panik umso mehr Spuren! */
		break;
	case 2:         /* Work */
		trail = (255 - p->Skill) / 4;
		trail = CalcValue(trail, 0, 255, 255 - p->Stamina, 15);
		trail = CalcValue(trail, 0, 255, 255 - p->Health, 15);
		trail = CalcValue(trail, 0, 255, 255 - p->Strength, 5);

		if (!(has(Person_Matt_Stuvysunt, Tool_Handschuhe)))
			trail = CalcValue(trail, 0, 255, 255, 50);  /* um 50 % mehr ! */
		break;

	case 3:         /* Kill */
		trail = (255 - p->Strength) / 4;
		trail = CalcValue(trail, 0, 255, 255 - p->Stamina, 10);
		trail = CalcValue(trail, 0, 255, 255 - p->Health, 10);
		trail = CalcValue(trail, 0, 255, 255 - p->Skill, 20);
		break;

	default:
		trail = 0;
		break;
	}

	trail = CalcValue(trail, 0, 255, p->Panic, nerves);

	return trail;
}

static uint32 tcGetNecessaryAbility(uint32 persId, uint32 toolId) {
	uint32 ability = 255;       /* andere Arbeiten optimal! */

	switch (toolId) {
	case Tool_Fusz:
	case Tool_Chloroform:
	case Tool_Hand:
		ability = hasGet(persId, Ability_Kampf);
		break;
	case Tool_Kernbohrer:
	case Tool_Winkelschleifer:
	case Tool_Schneidbrenner:
	case Tool_Sauerstofflanze:
	case Tool_Stethoskop:
	case Tool_Elektrohammer:
		ability = hasGet(persId, Ability_Safes);
		break;
	case Tool_Bohrmaschine:
	case Tool_Bohrwinde:
	case Tool_Dietrich:
		ability = hasGet(persId, Ability_Schloesser);
		break;
	case Tool_Dynamit:
		ability = hasGet(persId, Ability_Sprengstoff);
		break;
	case Tool_Elektroset:
		ability = hasGet(persId, Ability_Elektronik);
		break;
	}

	return ability;
}

/*
 * Berechnet Zeit, die Einbrecher für eine Aktion benötigt
 */

uint32 tcGuyUsesToolInPlayer(uint32 persId, Building b, uint32 toolId, uint32 itemId,
                             uint32 needTime) {
	uint32 time = tcGuyUsesTool(persId, b, toolId, itemId);
	uint32 ability = tcGetNecessaryAbility(persId, toolId);

	if (ability < (CalcRandomNr(0, 230)))
		if (CalcRandomNr(0, ability / 20) == 1)
			time = CalcValue(time, 0, time * 4, ability / 2, 10);

	if (time < needTime)
		time = needTime;

	return time;
}

uint32 tcGuyUsesTool(uint32 persId, Building b, uint32 toolId, uint32 itemId)
/*
 * diese Funktion darf keine Zufälligkeit enthalten -> Sync!!
 */
{
	uint32 origin, time;
	Person p = (Person)dbGetObject(persId);

	origin = time = breakGet(itemId, toolId);

	if (time != (uint32) - 1) {
		switch (toolId) {   /* Zeit varieren */
		case Tool_Elektrohammer:
		case Tool_Hammer:
		case Tool_Axt:
			time =
			    CalcValue(time, 0, origin * 4, 127 + (255 - p->Strength) / 2,
			              5);
			time =
			    CalcValue(time, 0, origin * 4, 127 + (255 - p->Stamina) / 2,
			              10);
			break;
		case Tool_Hand:
		case Tool_Fusz:
		case Tool_Chloroform:
			time =
			    CalcValue(time, 0, origin * 4, 127 + (255 - p->Skill) / 2, 5);
			time =
			    CalcValue(time, 0, origin * 4, 127 + (255 - p->Strength) / 2,
			              10);
			break;
		case Tool_Bohrwinde:
			time =
			    CalcValue(time, 0, origin * 4, 127 + (255 - p->Stamina) / 2, 5);
			time =
			    CalcValue(time, 0, origin * 4, 127 + (255 - p->Skill) / 2, 10);
			break;
		case Tool_Schloszstecher:
		case Tool_Glasschneider:
		case Tool_Bohrmaschine:
		case Tool_Brecheisen:
			time =
			    CalcValue(time, 0, origin * 4, 127 + (255 - p->Strength) / 2,
			              10);
			time =
			    CalcValue(time, 0, origin * 4, 127 + (255 - p->Skill) / 2, 10);
			break;
		case Tool_Winkelschleifer:
		case Tool_Schneidbrenner:
		case Tool_Sauerstofflanze:
		case Tool_Kernbohrer:
			time =
			    CalcValue(time, 0, origin * 4, 127 + (255 - p->Strength) / 2,
			              0);
			time =
			    CalcValue(time, 0, origin * 4, 127 + (255 - p->Skill) / 2, 10);
			break;
		case Tool_Dietrich:
		case Tool_Strickleiter:
			time =
			    CalcValue(time, 0, origin * 4, 127 + (255 - p->Skill) / 2, 10);
			break;
		case Tool_Stethoskop:
		case Tool_Elektroset:
		case Tool_Dynamit:
			time =
			    CalcValue(time, 0, origin * 4,
			              127 + (255 - p->Intelligence) / 2, 10);
			time =
			    CalcValue(time, 0, origin * 4, 127 + (255 - p->Skill) / 2, 10);
			break;
		default:
			break;
		}

		time =
		    CalcValue(time, 0, origin * 4,
		              127 +
		              ((255 - (tcGetNecessaryAbility(persId, toolId))) / 2),
		              50);

		switch (itemId) {   /* Alarmanglagen bonus! */
		case Item_Alarmanlage_Z3:
			break;
		case Item_Alarmanlage_X3:
			time = CalcValue(time, 0, origin * 4, 255, 30);
			break;
		case Item_Alarmanlage_Top:
			time = CalcValue(time, 0, origin * 4, 255, 50);
			break;
		default:
			break;
		}

		time =
		    CalcValue(time, 0, origin * 4, 120 + (255 - b->Exactlyness) / 2,
		              20);
		time = CalcValue(time, 0, origin * 4, 127 + (p->Panic / 2), 10);

		time = MAX(origin, time);   /* Schneller kann man nicht sein! */
	} else
		time = 0;       /* kein Tool oder kein Item angegeben! */

	return (time);
}

/*
 * Berechnet Verletzungsgefahr sowie Verletzung
 * bei > 0 -> Funkspruch "Au! Ich habe mich verletzt!"
 */

int32 tcGetDanger(uint32 persId, uint32 toolId, uint32 itemId) {
	Person p = (Person)dbGetObject(persId);
	uint32 danger = hurtGet(itemId, toolId);

	danger = CalcValue(danger, 0, 255, 255 - p->Skill, 30);
	danger = CalcValue(danger, 0, 255, 255 - p->Stamina, 10);
	danger = CalcValue(danger, 0, 255, p->Panic, 5);

	if (danger > CalcRandomNr(40, 255)) {   /* Verletzt ! */
		if (CalcRandomNr(0, 10) == 1) { /* ...oder vielleicht doch nicht */
			p->OldHealth = p->Health;
			p->Health = CalcValue(p->Health, 0, 255, 127 - danger, 90);
		} else
			danger = 0;
	} else
		danger = 0;

	return danger;
}

/*
 * berechnet die Lautstärke einer Aktion
 *
 */

int32 tcGetToolLoudness(uint32 persId, uint32 toolId, uint32 itemId) {
	Person p = (Person)dbGetObject(persId);
	int32 loudness = soundGet(itemId, toolId);

	loudness = CalcValue(loudness, 0, 255, 255 - p->Skill, 10);
	loudness = CalcValue(loudness, 0, 255, p->Panic, 5);

	return loudness;
}

/*
 * berechnet die Lautstärke vom Gehen
 */

int32 tcGetWalkLoudness(void) {
	int32 loudness = tcWALK_LOUDNESS;

	if (has(Person_Matt_Stuvysunt, Tool_Schuhe))
		loudness /= 2;

	return loudness;
}

/*
 * berechnet Lautstärkenpegel aller 4 Einbrecher
 */

int32 tcGetTotalLoudness(int32 loudp0, int32 loudp1, int32 loudp2, int32 loudp3) {
	int32 total;

	total = MAX(loudp0, loudp1);
	total = MAX(total, loudp2);
	total = MAX(total, loudp3);

	total = CalcValue(total, 0, 255, 255, 20);  /* um ca 20 % erhöhen */

	return (total);
}

/*
 * stellen fest, ob ein Funkspruch aufgefangen oder
 * ob durch die Lautstärke ein Passant, Nachbar die Polizei alarmiert wurde
 * wenn diese Funktionen 1 zurückliefern, ab jetzt ständig WatchDog
 * aufrufen
 */

/* am besten jede Sekunde aufrufen (loudness) */

bool tcAlarmByLoudness(Building b, int32 totalLoudness) {
	return (totalLoudness > b->MaxVolume);
}

/* nach jedem Funkspruch aufrufen */
bool tcAlarmByRadio(Building b) {
	int32 random = CalcRandomNr(0, 2500) + CalcRandomNr(0, 2500);   /* 10 mal funken bei Guarding = 250 -> Alarm */

	return (random < b->RadioGuarding);
}

bool tcAlarmByPatrol(uint16 objChangedCount, uint16 totalCount, uint8 patrolCount) {
	/* Patrolien entdecken nun schneller etwas: 100 -> 125 */

	if (((totalCount * tcPATROL_ALARM) / (125 * patrolCount)) < objChangedCount)
		return true;
	else
		return false;
}

/* für jedes Objekt, das vom Wächter kontrolliert wird, aufrufen! */
/* wenn 1 -> richtiger Alarm! */

bool tcGuardChecksObject(LSObject lso) {
	/* hier darf NICHT das OPEN_CLOSE_BIT sonst, schlägt der Wächter */
	/* Alarm, wenn er eine Tür öffnet!                               */
	if ((lso->ul_Status & (1 << Const_tcIN_PROGRESS_BIT)) ||
	        (lso->ul_Status & (1 << Const_tcLOCK_UNLOCK_BIT)))
		return true;
	else {
		switch (lso->Type) {
		case Item_Alarmanlage_Z3:
		case Item_Alarmanlage_X3:
		case Item_Alarmanlage_Top:
		case Item_Steuerkasten:
			/* hier darf NICHT das OPEN_CLOSE_BIT geprüft werden */
			/* sonst schlägt der Wächter Alarm, wenn er es öffnet! */
			if (lso->ul_Status & (1 << Const_tcON_OFF))
				return true;
			break;
		case Item_Bild:
		case Item_Vase:
		case Item_Gemaelde:
		case Item_Statue:
			if (lso->uch_Visible == 0)
				return true;
			break;
		default:
			break;
		}

		if (g_clue->getFeatures() & GF_PROFIDISK) {
			switch (lso->Type) {
			case Item_Heiligenstatue:
			case Item_Kerzenstaender:
			case Item_Kirchenkreuz:

			case Item_Hottentotten_Figur:
			case Item_Batman_Figur:
			case Item_Dicker_Man:
			case Item_Unbekannter:
			case Item_Jack_the_Ripper_Figur:
			case Item_Koenigs_Figur:
			case Item_Wache_Figur:
			case Item_Miss_World_1952:
				if (lso->uch_Visible == 0)
					return true;
				break;
			default:
				break;
			}
		}
	}

	return false;
}

/* vor Beginn jeder Arbeit oder alle 32 Pixel die ein Einbrecher geht,
 * aufrufen!
 * loudness = tcGetToolLoudness oder tcWalkLoudness
 */

bool tcAlarmByMicro(uint16 us_XPos, uint16 us_YPos, int32 loudness) {
	return (loudness > lsGetLoudness(us_XPos, us_YPos));
}

/*
 * sobald ein Alarm Marke "Nachbar" existiert, für jeden Aufpasser, der gerade
 * wartet, diese Funktion aufrufen
 * wenn diese Funktion 1 retourniert -> Aufpasser hat etwas bemerkt ->
 * Warnung an Matt !
 */

bool tcWatchDogWarning(uint32 persId) {
	int32 watch = hasGet(persId, Ability_Aufpassen);
	int32 random;

	random = CalcRandomNr(0, 200) + /* Joe soll nicht gleich in der ersten */
	         CalcRandomNr(0, 200) +  /* Sekunde etwas bemerken!             */
	         CalcRandomNr(0, 200);   /* Risiko wird durch Addition GRÖßER!! */

	if ((watch > random) && (CalcRandomNr(0, 40) == 1))
		return true;

	return false;
}

/*
 * sobald KEIN Alarm Marke "Nachbar" existiert, für jeden Aufpasser, der
 * gerade wartet, diese Funktion aufrufen (Fehlalarm)
 */

bool tcWrongWatchDogWarning(uint32 persId) {
	uint32 watch = hasGet(persId, Ability_Aufpassen);

	if (CalcRandomNr(0, 255) > watch) { /* Irrtum */
		if (CalcRandomNr(0, watch * 50) == 1)   /* umso besser, umso kleiner Wahrscheinlichkeit */
			return true;
	}

	return false;
}

bool tcIsCarRecognised(Car car, uint32 time) {
	uint32 strike = tcGetCarStrike(car);
	uint32 weight;

	/* Zeit spielt eine Rolle ! */
	/* nach einer Viertelstunde mit Alarm, ist Strike == 255 */

	weight = 127 + CalcValue(time / 8, 0, 127, 0, 0);   /* nur wegen Bereichsüberprüfung */
	strike = CalcValue(strike, 0, 255, weight, 100);

	if ((strike > 220)
	        || ((strike > (CalcRandomNr(10, 220) + CalcRandomNr(20, 220)))))
		return true;
	else
		return false;
}

int32 tcGetGuyState(uint32 persId) {
	Person p = (Person)dbGetObject(persId);
	int32 state;

	state = tcGetPersHealth(p);
	state = CalcValue(state, 0, 255, p->Stamina, 50);
	state = CalcValue(state, 0, 255, p->Strength, 15);

	return state;
}

int32 tcCalcMattsPart(void) {
	LIST *guys;
	NODE *node;
	int32 count = 0, part = 0;

	joined_byAll(Person_Matt_Stuvysunt, OLF_INCLUDE_NAME | OLF_PRIVATE_LIST,
	             Object_Person);
	guys = ObjectListPrivate;

	count = GetNrOfNodes(guys);

	for (node = LIST_HEAD(guys); NODE_SUCC(node);
	        node = NODE_SUCC(node)) {
		if (OL_NR(node) != Person_Matt_Stuvysunt)
			part += tcGetPersOffer((Person)(OL_DATA(node)), count);
	}

	if (part > 99)
		part = 99;

	RemoveList(guys);

	return 100 - part;
}

/* immer wenn eine Stechuhr betätigt wird */

void tcRefreshTimeClock(uint32 buildId, uint32 timerId) {
	uint32 time = hasClockGet(buildId, timerId);

	ClockTimerSetP(timerId, timerId, time);
}

/* jedes Sekunde aufrufen
 * wenn 1 -> Alarm durch Stechuhr!
 */

bool tcCheckTimeClocks(uint32 buildId) {
	NODE *n;
	bool alarm = false;

	/* alle Stechuhren holen */

	hasClockAll(buildId, OLF_NORMAL, Object_LSObject);

	/* und kontrollieren, ob die Zeit abgelaufen ist */

	for (n = LIST_HEAD(ObjectList); NODE_SUCC(n);
	        n = NODE_SUCC(n)) {
		uint32 timerId = OL_NR(n);
		int32 time = (int32) ClockTimerGet(timerId, timerId);

		if (time - 1)
			ClockTimerSetP(timerId, timerId, time - 1);
		else
			alarm = true;
	}

	return alarm;
}

/* stellt fest, ob 2 Positionen innerhalb des selben Raumes sind */

static bool tcInsideSameRoom(LIST *roomsList, int16 polX, int16 polY, int16 livX,
                             int16 livY) {
	NODE *node;
	bool detected = false;
	LSRoom room;

	for (node = LIST_HEAD(roomsList); NODE_SUCC(node) && detected == 0;
	        node = NODE_SUCC(node)) {
		room = (LSRoom)OL_DATA(node);

		if ((polX >= room->us_LeftEdge)
		        && (polX <= room->us_LeftEdge + room->us_Width)
		        && (polY >= room->us_TopEdge)
		        && (polY <= room->us_TopEdge + room->us_Height)) {
			if ((livX >= room->us_LeftEdge)
			        && (livX <= room->us_LeftEdge + room->us_Width)
			        && (livY >= room->us_TopEdge)
			        && (livY <= room->us_TopEdge + room->us_Height))
				detected = true;
		}
	}

	return detected;
}

/* Für jeden(?) Schritt einer für JEDEN Einbrecher aufrufen */
/* LivingName = names des Einbrechers */
/* XPos, YPos = Position des Wächters */
/* wenn 1 -> Alarm! */

bool tcGuardDetectsGuy(LIST *roomsList, uint16 us_XPos, uint16 us_YPos,
                       uint8 uch_ViewDirection, char *puch_GuardName,
                       char *puch_LivingName) {
	bool detected = false;
	uint16 livXPos = livGetXPos(puch_LivingName);
	uint16 livYPos = livGetYPos(puch_LivingName);

	if (livWhereIs(puch_GuardName) == livWhereIs(puch_LivingName)) {
		if (livIsPositionInViewDirection(us_XPos, us_YPos, livXPos, livYPos, uch_ViewDirection)) {
			if (tcInsideSameRoom(roomsList, us_XPos + tcX_HOTSPOT, us_YPos,
			         livXPos + tcX_HOTSPOT, livYPos))
				detected = true;
		}
	}

	return detected;
}

/* jedesmal aufrufen, wenn ein Objekt bearbeitet wird */
/* überprüft, ob dieses Objekt mit einer Alarmanlafe verbunden ist */
/* 1 -> Alarm */

bool tcAlarmByTouch(uint32 lsoId) {
	LSObject lso = (LSObject)dbGetObject(lsoId);

	if (lso->uch_Chained & Const_tcCHAINED_TO_ALARM)
		if (tcIsConnectedWithEnabledAlarm(lsoId))
			return true;

	return false;
}


/* kontrolliert, ob durch Stromverlust ein Alarm ausgelöst wird
 * jedesmal aufrufen, wenn ein Steuerkasten ausgeschaltet wird!
 * wenn 1 -> Alarm
 */

bool tcAlarmByPowerLoss(uint32 powerId) {
	LIST *friendlyList;
	NODE *n;

	/* alle Objekte, mit denen diese Stromversorgung verbunden ist */

	SetObjectListAttr(OLF_PRIVATE_LIST, Object_LSObject);
	AskAll(dbGetObject(powerId), hasPowerRelationID, BuildObjectList);
	friendlyList = ObjectListPrivate;

	/* eine davon mit einer Alarmanlage verbunden? */

	for (n = LIST_HEAD(friendlyList); NODE_SUCC(n);
	        n = NODE_SUCC(n)) {
		LSObject lso = (LSObject)OL_DATA(n);

		if (lso->ul_Status & Const_tcCHAINED_TO_ALARM)
			if (tcIsConnectedWithEnabledAlarm(OL_NR(n))) {
				RemoveList(friendlyList);
				return true;
			}
	}

	RemoveList(friendlyList);
	return false;
}

static bool tcIsConnectedWithEnabledAlarm(uint32 lsoId) {
	NODE *n;

	/* alle Alarmanlagen, mit denen dieser Gegenstand verbunden ist */

	SetObjectListAttr(OLF_NORMAL, Object_LSObject);
	AskAll(dbGetObject(lsoId), hasAlarmRelationID, BuildObjectList);

	/* ist eine davon eingeschalten? */

	for (n = LIST_HEAD(ObjectList); NODE_SUCC(n);
	        n = NODE_SUCC(n)) {
		LSObject alarm = (LSObject)OL_DATA(n);

		if (!(alarm->ul_Status & (1L << Const_tcON_OFF)))
			return true;
	}

	return false;
}

static int32 tcGetWeightOfNerves(int32 teamMood) {
	return (255 - teamMood);
}

void tcInsertGuard(LIST *list, LIST *roomsList, uint16 x, uint16 y, uint16 width,
                   uint16 height, uint32 guardId, uint8 livId, uint32 areaId) {
	char name[TXT_KEY_LENGTH];
	uint16 gx, gy;
	uint32 guardedArea = isGuardedbyGet(lsGetCurrBuildingID(), guardId);

	sprintf(name, "Police_%d", livId);

	gx = livGetXPos(name);
	gy = livGetYPos(name);

	if (guardedArea == areaId)
		if ((x <= (gx + 16)) && ((x + width) >= gx) && (y <= (gy + 23))
		        && ((y + height) >= gy))
			if (tcInsideSameRoom
			        (roomsList, gx + tcX_HOTSPOT, gy, x + tcX_HOTSPOT, y))
				dbAddObjectNode(list, guardId,
				                OLF_INCLUDE_NAME | OLF_INSERT_STAR);
}

} // End of namespace Clue

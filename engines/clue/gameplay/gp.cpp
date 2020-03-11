/*
**  $Filename: gameplay/gp.c
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
#include "clue/gameplay/gamefunc.h"
#include "clue/clue.h"

namespace Clue {

void InitLocations();
void FreeLocations();

void InitSceneInfo();
void FreeSceneInfo();

void PrepareStory(const char *filename);
void LoadSceneforStory(struct NewScene *dest, Common::Stream *file);

void InitConditions(struct Scene *scene, struct NewScene *ns);
void FreeConditions(struct Scene *scene);
int32 GetEventCount(uint32 EventNr);
int32 CheckConditions(struct Scene *scene);
void EventDidHappen(uint32 EventNr);

struct Scene *GetStoryScene(struct Scene *scene);

uint32 GamePlayMode = 0;
byte RefreshMode = 0;

struct Film *film = NULL;
struct SceneArgs SceneArgs;


void InitStory(const char *story_filename) {
	if (film)
		CloseStory();

	film = (struct Film *) TCAllocMem(sizeof(*film), 0);

	film->EnabledChoices = 0xffffffffL;
	PrepareStory(story_filename);
	InitLocations();
	LinkScenes();
	PatchStory();
}

void CloseStory() {
	uint32 i;

	if (film) {
		if (film->loc_names)
			RemoveList(film->loc_names);

		for (i = 0; i < film->AmountOfScenes; i++) {
			if (film->gameplay[i].bed)
				FreeConditions(&film->gameplay[i]);
			if (film->gameplay[i].std_succ)
				RemoveList(film->gameplay[i].std_succ);
		}

		if (film->gameplay)
			TCFreeMem(film->gameplay,
			          sizeof(struct Scene) * film->AmountOfScenes);

		TCFreeMem(film, sizeof(*film));

		film = NULL;
	}
}

void SetEnabledChoices(uint32 ChoiceMask) {
	film->EnabledChoices = ChoiceMask;
}

void RefreshCurrScene() {
	NODE *node = (NODE *)GetNthNode(film->loc_names, GetLocation);

	tcRefreshLocationInTitle(GetLocation);
	PlayAnim(NODE_NAME(node), 30000,
	         GFX_NO_REFRESH | GFX_ONE_STEP | GFX_BLEND_UP);

	RefreshMenu();
}

void InitLocations() {
	LIST *l = CreateList();
	char pathname[DSK_PATH_MAX];

	dskBuildPathName(DISK_CHECK_FILE, TEXT_DIRECTORY, LOCATIONS_TXT, pathname);

	if (ReadList(l, sizeof(struct TCEventNode), pathname))
		film->loc_names = (LIST *) l;
	else
		ErrorMsg(Disk_Defect, ERROR_MODULE_GAMEPLAY, 1);
}

void PatchStory() {
	if (!(GamePlayMode & GP_DEMO)) {
		GetScene(26214400L)->bed->Ort = 3;  /* 4th Burglary, Hotelzimmer */
		GetScene(26738688L)->bed->Ort = 7;  /* Arrest, Polizei!          */

		GetScene(SCENE_KASERNE_OUTSIDE)->Moeglichkeiten = 15;
		GetScene(SCENE_KASERNE_INSIDE)->Moeglichkeiten = 265;

		GetScene(SCENE_KASERNE_OUTSIDE)->LocationNr = 66;
		GetScene(SCENE_KASERNE_INSIDE)->LocationNr = 65;

		GetScene(SCENE_KASERNE_OUTSIDE)->Dauer = 17;
		GetScene(SCENE_KASERNE_INSIDE)->Dauer = 57;

		GetScene(SCENE_STATION)->Moeglichkeiten |= WAIT;

		if (g_clue->getFeatures() & GF_PROFIDISK) {
			GetScene(SCENE_PROFI_26)->LocationNr = 75;
		}

		/* change possibilites in story_9 too! */

		/* für die Kaserne hier einen Successor eingetragen! */
		GetLocScene(65)->std_succ = CreateList();
		struct TCEventNode *node = (struct TCEventNode *) CreateNode(GetLocScene(65)->std_succ, sizeof(*node), NULL);
		node->EventNr = SCENE_KASERNE_OUTSIDE;  /* wurscht... */

		GetLocScene(66)->std_succ = CreateList();
		node = (struct TCEventNode *) CreateNode(GetLocScene(66)->std_succ, sizeof(*node), NULL);
		node->EventNr = SCENE_KASERNE_INSIDE;   /* wurscht... */

		film->StartScene = SCENE_STATION;
	}
}

uint32 PlayStory() {
	struct Scene *curr, *next = NULL;
	struct Scene *story_scene = 0;
	uint8 interr_allowed = 1, first = 1;

	if (GamePlayMode & GP_STORY_OFF) {
		if (GamePlayMode & GP_DEMO)
			curr = GetScene(SCENE_CARS_VANS);
		else {
			tcAddPlayerMoney(5000);
			curr = GetScene(SCENE_HOTEL_ROOM);
		}
	} else
		curr = GetScene(film->StartScene);

	SetCurrentScene(curr);
	film->alter_Ort = curr->LocationNr;
	SetLocation(-2);

	while ((curr->EventNr != SCENE_THE_END)
	        && (curr->EventNr != SCENE_NEW_GAME)) {
		if (!CheckConditions(curr))
			ErrorMsg(Internal_Error, ERROR_MODULE_GAMEPLAY, 2);

		/* Entscheidung für eine Szene getroffen ! */

		SceneArgs.ReturnValue = 0L;
		SceneArgs.Ueberschrieben = 0;

		/* wenn Szene Storyszene ist, dann Musik beibehalten */
		/* (Storyszenen ändern sie selbst in Done */
		/* ansonsten wird die Musi hier verändert */

		if (!story_scene)
			tcPlaySound();

		/* ab jetzt soll nach einem Diskettenwechsel refresht werden */
		/* -> GP_STORY_TOWN Flag setzen */
		/* Achtung: dieses Flag nur nach der 1.Szene setzen -> "first" */

		if (first) {
			film->StoryIsRunning = GP_STORY_TOWN;
			first = 0;
		}

		/* die neue Szene initialisieren ! ( Bühnenbild aufbauen ) */
#ifdef DEEP_DEBUG
		printf("----------------------------------------\n");
		printf("SCENE_INIT %u\n", curr->EventNr);
#endif
		if (curr->Init)
			curr->Init();

		/* gibt es irgendein Ereignis das jetzt geschehen kann, und */
		/* den Standardablauf unterbricht ? */

		/* if (GamePlayMode & GP_DEMO)
		   DemoDialog(); */

		story_scene = 0;

		if (interr_allowed && (!(GamePlayMode & GP_STORY_OFF))) {
#ifdef DEEP_DEBUG
			printf("STEP_A1\n");
#endif
			if ((story_scene = GetStoryScene(curr)))
				interr_allowed = 0;
		} else {
#ifdef DEEP_DEBUG
			printf("STEP_A2\n");
#endif
			story_scene = 0;
		}

		if (!story_scene) {
			SceneArgs.Ueberschrieben = 0;

#ifdef DEEP_DEBUG
			printf("STEP_B\n");
#endif
			if (curr->Done) {
				SceneArgs.Moeglichkeiten =
				    curr->Moeglichkeiten & film->EnabledChoices;

#ifdef DEEP_DEBUG
				printf("SCENE_DONE\n");
#endif
				curr->Done();   /* Funktionalität */
			} else
				ErrorMsg(Internal_Error, ERROR_MODULE_GAMEPLAY, 3);

			EventDidHappen(curr->EventNr);
			AddVTime(curr->Dauer);

			/* jetzt kann wieder unterbrochen werden ! */
			interr_allowed = 1;

			/* der Spieler ist fertig mit dieser Szene - aber wie geht es weiter ? */
			if (SceneArgs.ReturnValue)
				next = GetScene(SceneArgs.ReturnValue); /* Nachfolger festlegen */
			else
				ErrorMsg(Internal_Error, ERROR_MODULE_GAMEPLAY, 4);
		} else
			next = story_scene; /* Unterbrechung durch die Storyszene ! */

		SetCurrentScene(curr = next);
	}               /* While Schleife */

	film->StoryIsRunning = GP_STORY_BEFORE;

	StopAnim();

	return curr->EventNr;
}

struct Scene *GetStoryScene(struct Scene *curr) {
	for (uint32 i = 0; i < film->AmountOfScenes; i++) {
		if (film->gameplay[i].LocationNr == (uint32) - 1) {
			struct Scene *sc = &film->gameplay[i];

			if (sc != curr) {
				uint32 j = g_clue->calcRandomNr(0L, 255L);

				if (j <= (uint32)(sc->Probability))
					if (CheckConditions(sc))
						return (sc);
			}
		}
	}

	return (0);
}

struct Scene *GetScene(uint32 EventNr) {
	struct Scene *sc = NULL;

	for (uint32 i = 0; i < film->AmountOfScenes; i++)
		if (EventNr == film->gameplay[i].EventNr)
			sc = &(film->gameplay[i]);

	return sc;
}

int32 GetEventCount(uint32 EventNr) {
	struct Scene *sc = GetScene(EventNr);

	if (sc)
		return ((int32)(sc->Geschehen));

	return 0;
}

void EventDidHappen(uint32 EventNr) {
	uint32 max = CAN_ALWAYS_HAPPEN;
	struct Scene *sc = GetScene(EventNr);

	if (sc) {
		if (sc->Geschehen < max)
			sc->Geschehen += 1;
	}
}

int32 CheckConditions(struct Scene *scene) {
	/* wenn Std Szene, dann muß nichts überprüft werden ! */

	if (scene->LocationNr != (uint32) - 1)
		return (1L);

	/* es handelt sich um keine Std Szene -> Überprüfen ! */
	/* überprüfen, ob Szene nicht schon zu oft geschehen ist ! */
	if ((scene->Anzahl != ((uint16)(CAN_ALWAYS_HAPPEN))) &&
	        (scene->Geschehen) >= (scene->Anzahl))
		return (0L);

	/* Jetzt die einzelnen Bedingungen überprüfen */
	struct Bedingungen *bed = scene->bed;
	if (!bed)
		return (1L);

	if (bed->Ort != (uint32) - 1)       /* spielt der Ort eine Rolle ? */
		if (GetLocation != (bed->Ort))
			return (0L);    /* spielt eine Rolle und ist nicht erfüllt */

	/*
	 * Überprüfen, ob ein Event eingetreten ist,
	 * das nicht geschehen darf !
	 */

	if (bed->n_events) {
		for (NODE *node = LIST_HEAD(bed->n_events); NODE_SUCC(node); node = NODE_SUCC(node)) {
			if (GetEventCount(((struct TCEventNode *) node)->EventNr))
				return (0L);
		}
	}

	/*
	 * sind alle Events eingetreten, die Bedingung sind ?
	 *
	 */

	if (bed->events) {
		for (NODE *node = LIST_HEAD(bed->events); NODE_SUCC(node); node = NODE_SUCC(node)) {
			if (!GetEventCount(((struct TCEventNode *) node)->EventNr))
				return (0L);
		}
	}

	return (1L);
}

void PrepareStory(const char *filename) {
/*
 * completely revised 02-02-93
 * tested : 26-03-93
 */
	char pathname[DSK_PATH_MAX];
	dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, filename, pathname);

	/* StoryHeader laden ! */
	Common::Stream *file = dskOpen(pathname, 0);

	struct StoryHeader SH;
	dskRead(file, SH.StoryName, sizeof(SH.StoryName));
	dskRead_U32LE(file, &SH.EventCount);
	dskRead_U32LE(file, &SH.SceneCount);

	dskRead_U32LE(file, &SH.AmountOfScenes);
	dskRead_U32LE(file, &SH.AmountOfEvents);

	dskRead_U32LE(file, &SH.StartZeit);
	dskRead_U32LE(file, &SH.StartOrt);
	dskRead_U32LE(file, &SH.StartSzene);

	film->AmountOfScenes = SH.AmountOfScenes;

	film->StartZeit = SH.StartZeit;

	film->akt_Minute = 543; /* 09:03 */
	film->akt_Tag = film->StartZeit;
	film->akt_Ort = film->StartOrt = SH.StartOrt;
	film->StartScene = SH.StartSzene;

	if (film->AmountOfScenes) {
		if (!
		        (film->gameplay =
		             ((struct Scene *)TCAllocMem(sizeof(struct Scene) * (film->AmountOfScenes), 0))))
			ErrorMsg(No_Mem, ERROR_MODULE_GAMEPLAY, 6);
	} else
		ErrorMsg(Disk_Defect, ERROR_MODULE_GAMEPLAY, 7);

	for (uint32 i = 0; i < film->AmountOfScenes; i++) {
		struct NewScene NS;
		LoadSceneforStory(&NS, file);

		struct Scene *scene = &(film->gameplay[i]);

		scene->EventNr = NS.EventNr;

		if (NS.NewOrt == (uint32) - 1 || NS.AnzahlderEvents || NS.AnzahlderN_Events) /* Storyszene ? */
			InitConditions(scene, &NS); /* ja ! -> Bedingungen eintragen */
		else
			film->gameplay[i].bed = NULL;   /* Spielablaufszene : keine Bedingungen ! */

		/* Scene Struktur füllen : */
		scene->Done = StdDone;
		scene->Init = StdInit;
		scene->Moeglichkeiten = NS.Moeglichkeiten;
		scene->Dauer = NS.Dauer;
		scene->Anzahl = NS.Anzahl;
		scene->Geschehen = (uint16)(NS.Geschehen);
		scene->Probability = NS.Possibility;
		scene->LocationNr = NS.NewOrt;

		/* Nachfolgerliste aufbauen !  */
		/* Achtung! auch Patch ändern! */

		if (NS.AnzahlderNachfolger) {
			scene->std_succ = CreateList();

			for (uint32 j = 0; j < NS.AnzahlderNachfolger; j++) {
				struct TCEventNode *node = (struct TCEventNode *) CreateNode(scene->std_succ, sizeof(*node), NULL);
				node->EventNr = NS.nachfolger[j];
			}

			if (NS.nachfolger)
				TCFreeMem(NS.nachfolger, sizeof(uint32) * NS.AnzahlderNachfolger);
		} else
			scene->std_succ = NULL;
	}

	/* von den Events muß nichts geladen werden ! */
	dskClose(file);
}

void InitConditions(struct Scene *scene, struct NewScene *ns) {
	struct Bedingungen *bed = (struct Bedingungen *) TCAllocMem(sizeof(*bed), 0);

	bed->Ort = ns->Ort;

	if (ns->AnzahlderEvents) {
		bed->events = CreateList();

		for (uint32 i = 0; i < ns->AnzahlderEvents; i++) {
			struct TCEventNode *node =
			    (struct TCEventNode *) CreateNode(bed->events, sizeof(*node),
			                                      NULL);

			node->EventNr = ns->events[i];
		}

		if (ns->events)
			TCFreeMem(ns->events, sizeof(uint32) * (ns->AnzahlderEvents));
	} else
		bed->events = NULL;

	if (ns->AnzahlderN_Events) {
		bed->n_events = CreateList();

		for (uint32 i = 0; i < ns->AnzahlderN_Events; i++) {
			struct TCEventNode *node =
			    (struct TCEventNode *) CreateNode(bed->n_events,
			                                      sizeof(*node), NULL);

			node->EventNr = ns->n_events[i];
		}

		if (ns->n_events)
			TCFreeMem(ns->n_events, sizeof(uint32) * (ns->AnzahlderN_Events));
	} else
		bed->n_events = NULL;

	scene->bed = bed;
}

void FreeConditions(struct Scene *scene) {
	if (scene->bed) {
		if (scene->bed->events)
			RemoveList(scene->bed->events);
		if (scene->bed->n_events)
			RemoveList(scene->bed->n_events);

		TCFreeMem(scene->bed, sizeof(struct Bedingungen));

		scene->bed = NULL;
	}
}

void LoadSceneforStory(struct NewScene *dest, Common::Stream *file) {
	dskRead_U32LE(file, &dest->EventNr);

	dskRead(file, dest->SceneName, sizeof(dest->SceneName));

	dskRead_S32LE(file, &dest->Tag);
	dskRead_S32LE(file, &dest->MinZeitPunkt);
	dskRead_S32LE(file, &dest->MaxZeitPunkt);
	dskRead_U32LE(file, &dest->Ort);

	dskRead_U32LE(file, &dest->AnzahlderEvents);
	dskRead_U32LE(file, &dest->AnzahlderN_Events);

	uint32 dummy;
	dskRead_U32LE(file, &dummy);
	dskRead_U32LE(file, &dummy);

	dskRead_U32LE(file, &dest->AnzahlderNachfolger);

	dskRead_U32LE(file, &dummy);

	dskRead_U32LE(file, &dest->Moeglichkeiten);
	dskRead_U32LE(file, &dest->Dauer);

	dskRead_U16LE(file, &dest->Anzahl);
	dskRead_U16LE(file, &dest->Geschehen);

	dskRead_U8(file, &dest->Possibility);

	dskRead_U32LE(file, &dest->Sample);
	dskRead_U32LE(file, &dest->Anim);
	dskRead_U32LE(file, &dest->NewOrt);

	/* allocate mem for events and read them */
	uint32 *event_nrs;
	if (dest->AnzahlderEvents) {
		event_nrs = (uint32 *) TCAllocMem(sizeof(uint32) * (dest->AnzahlderEvents), 0);
		if (!event_nrs)
			ErrorMsg(No_Mem, ERROR_MODULE_GAMEPLAY, 8);

		for (uint32 i = 0; i < dest->AnzahlderEvents; i++)
			dskRead_U32LE(file, &event_nrs[i]);
	} else
		event_nrs = NULL;

	dest->events = event_nrs;

	/* allocate mem for n_events and read them */
	if (dest->AnzahlderN_Events) {
		event_nrs = (uint32 *) TCAllocMem(sizeof(uint32) * (dest->AnzahlderN_Events), 0);
		if (!event_nrs)
			ErrorMsg(No_Mem, ERROR_MODULE_GAMEPLAY, 9);

		for (uint32 i = 0; i < dest->AnzahlderN_Events; i++)
			dskRead_U32LE(file, &event_nrs[i]);
	} else
		event_nrs = NULL;

	dest->n_events = event_nrs;

	/* allocate mem for successors and read them */
	if (dest->AnzahlderNachfolger) {
		event_nrs = (uint32 *) TCAllocMem(sizeof(uint32) * (dest->AnzahlderNachfolger), 0);
		if (!event_nrs)
			ErrorMsg(No_Mem, ERROR_MODULE_GAMEPLAY, 10);

		for (uint32 i = 0; i < dest->AnzahlderNachfolger; i++)
			dskRead_U32LE(file, &event_nrs[i]);
	} else
		event_nrs = NULL;

	dest->nachfolger = event_nrs;
}

void AddVTime(uint32 add) {

	uint32 time = GetMinute + add;

	if (time >= MINUTES_PER_DAY) {
		uint32 tag = GetDay + time / MINUTES_PER_DAY;

		SetDay(tag);
		time = time % MINUTES_PER_DAY;
	}

	SetTime(time);
	tcTheAlmighty(time);
}

void SetCurrentScene(struct Scene *scene) {
	film->act_scene = scene;
}

struct Scene *GetCurrentScene() {
	return (film ? film->act_scene : NULL);
}

struct Scene *GetLocScene(uint32 locNr) {

	for (uint32 i = 0; i < film->AmountOfScenes; i++) {
		struct Scene *sc = &film->gameplay[i];
		if (sc->LocationNr == locNr)
			return (sc);
	}
	ErrorMsg(Internal_Error, ERROR_MODULE_GAMEPLAY, 12);
	return NULL;
}

void FormatDigit(uint32 digit, char *s) {
	if (digit < 10)
		sprintf(s, "0%u", digit);
	else
		sprintf(s, "%u", digit);
}

char *BuildTime(uint32 min, char *time) {
	uint32 h = (min / 60) % 24;
	char s[TXT_KEY_LENGTH];

	min %= 60;

	FormatDigit(h, s);
	sprintf(time, "%s:", s);
	FormatDigit(min, s);
	strcat(time, s);

	return (time);
}

char *BuildDate(uint32 days, char *date) {
	const uint8 days_per_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	uint32 p_year, i;
	for (i = 0, p_year = 0; i < 12; i++)
		p_year += days_per_month[i];

	uint32 year = days / p_year;   /* which daywelches Jahr ? */
	days = days % p_year;   /* wieviele Tage noch in diesem Jahr */

	uint32 month = 0;

	for (i = 0, p_year = 0; i < 12; i++) {
		p_year += days_per_month[i];

		if (days < p_year) {
			month = i;
			break;
		}
	}

	uint32 day = days - (p_year - days_per_month[month]);

	char s[TXT_KEY_LENGTH];
	FormatDigit(day + 1, s);
	strcpy(date, s);
	strcat(date, ".");
	FormatDigit(month + 1, s);
	strcat(date, s);
	strcat(date, ".");
	FormatDigit(year, s);
	strcat(date, s);

	return (date);
}

char *GetCurrLocName() {
	uint32 index = GetCurrentScene()->LocationNr;
	return (NODE_NAME(GetNthNode(film->loc_names, index)));
}

#if 0
static uint32 GetAmountOfScenes() {
	/* for extern modules */
	return (film ? film->AmountOfScenes : 0);
}
#endif

} // End of namespace Clue

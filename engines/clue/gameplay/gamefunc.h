/*
 * SEdFunc.h
 * Functions to work with Stories/Events/Scenes for StoryEd
 * (c) 1992 by H.Gaberschek, K.Kazemi
 */

/*
 *
 * revision history :
 *
 * 17/08/92 first structures und functions (hg)
 * 20/08/92 changed struct Scene         (hg)
 * 31/08/92 changed struct NewScene (Tag) (hg)
 * 11/12/92 revised (kk)
 * 12/12/92 Nodenames changed (kk)
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

namespace Clue {

#define SCENE_NODE              100
#define EVENT_NODE              101
#define MAX_SCENES_AMOUNT   (1<<14) /* 14 + 18 = 32 Bits !!! */
#define MAX_EVENTS_AMOUNT   (1<<18) /* wichtig da story das benötigt ! */


struct StoryHeader {
	byte StoryName[20];

	uint32 EventCount;      /* Zaehler mit der höchsten EventNr */
	uint32 SceneCount;

	uint32 AmountOfScenes;
	uint32 AmountOfEvents;

	uint32 StartZeit;
	uint32 StartOrt;
	uint32 StartSzene;
};

struct NewStory {
	byte StoryName[20];

	struct StoryHeader *sh;

	struct List *scenes;    /* Liste von SceneNodes */
	struct List *events;

	uint32 StartZeit;
	uint32 StartOrt;
};

struct NewScene {
	uint32 EventNr;
	uint8 SceneName[20];

	int32 Tag;          /* der Tag an dem sie eintritt */
	int32 MinZeitPunkt;     /* zeitlicher Bereich in dem    */
	int32 MaxZeitPunkt;     /* sie eintritt                         */
	uint32 Ort;         /* der erfüllt sein muß                 */

	uint32 AnzahlderEvents;
	uint32 AnzahlderN_Events;

	uint32 *events;     /* Nr. der Events, die erfüllt sein müssen */
	uint32 *n_events;       /* Nr. der Events, die nicht erfüllt sein müssen */

	uint32 AnzahlderNachfolger;
	uint32 *nachfolger;     /* Nr. der NachfolgerEvents */

	uint32 Moeglichkeiten;      /* siehe defines oben                   */
	uint32 Dauer;           /* Dauer dieser Szene in Minuten     */
	uint16 Anzahl;          /* wie oft sie geschehen kann                */
	uint16 Geschehen;       /* wie oft sie SCHON geschehen ist */
	uint8 Possibility;      /* mit der sie eintritt 0-255   */

	uint32 Sample;          /* Nummer des Samples */
	uint32 Anim;            /* Nummer der Animation */
	uint32 NewOrt;          /* Ort der Scene */
};

struct SceneNode {
	struct Node Link;
	struct NewScene ns;
};

struct NewEvent {
	uint32 EventNr;
	byte EventName[20];
};

struct EventNode {
	struct Node Link;
	struct NewEvent ne;
};


/* Prototypes */
/* Story */
extern void NewStory(byte *name, uint32 StartZeit, uint32 StartOrt);
extern void ChangeStory(uint32 StartZeit, uint32 StartOrt);
extern void RemoveStory(void);
extern void WriteStory(byte *filename);
extern void ReadStory(byte *filename);

/* Scenes */
extern void AddScene(struct NewScene *ns);
extern void ChangeScene(byte *name, struct NewScene *ns);
extern void RemoveScene(byte *name);
extern void RemoveAllScenes(void);

/* Events */
extern void AddEvent(struct NewEvent *ev);
extern void RemoveEvent(byte *name);
extern void RemoveAllEvents(void);

/* Specials */
char *GetName(uint32 EventNr);


extern struct NewStory *story;

} // End of namespace Clue

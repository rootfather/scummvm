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

class StoryHeader {
public:
	byte _storyName[20];

	uint32 _eventCount;      /* Zaehler mit der höchsten EventNr */
	uint32 _sceneCount;

	uint32 _amountOfScenes;
	uint32 _amountOfEvents;

	uint32 _startTime;
	uint32 _startLocation;
	uint32 _startScene;

	void load(Common::Stream* file);
};

class NewScene {
public:
	uint32 EventNr;
	uint8 SceneName[20];

	int32 Tag;          /* der Tag an dem sie eintritt */
	int32 MinZeitPunkt;     /* zeitlicher Bereich in dem    */
	int32 MaxZeitPunkt;     /* sie eintritt                         */
	uint32 _location;         /* der erfüllt sein muß                 */

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

	NewScene();
};

/* Prototypes */
/* Story */
extern void NewStory(byte *name, uint32 StartZeit, uint32 StartOrt);
extern void ChangeStory(uint32 StartZeit, uint32 StartOrt);
extern void RemoveStory();
extern void WriteStory(byte *filename);
extern void ReadStory(byte *filename);

/* Scenes */
extern void AddScene(NewScene *ns);
extern void ChangeScene(byte *name, NewScene *ns);
extern void RemoveScene(byte *name);
extern void RemoveAllScenes();

/* Events */
extern void AddEvent(struct NewEvent *ev);
extern void RemoveEvent(byte *name);
extern void RemoveAllEvents();

/* Specials */
char *GetName(uint32 EventNr);

} // End of namespace Clue

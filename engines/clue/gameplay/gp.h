/*
**  $Filename: gameplay/gp.h
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

#ifndef MODULE_GAMEPLAY
#define MODULE_GAMEPLAY


#include "clue/base/base.h"
#include "clue/story/story.h"

namespace Clue {

#define GO                      (1)
#define WAIT                    (1<<1)
#define BUSINESS_TALK           (1<<2)
#define LOOK                    (1<<3)
#define INVESTIGATE             (1<<4)
#define PLAN                    (1<<5)
#define CALL_TAXI               (1<<6)
#define MAKE_CALL               (1<<7)
#define INFO                    (1<<8)
#define SLEEP                   (1<<9)

#define GP_ALL_CHOICES_ENABLED  UINT_MAX

/* Szene kann UNENDLICH oft geschehen */
#define CAN_ALWAYS_HAPPEN       USHRT_MAX

#define MINUTES_PER_DAY          1440L

#define GP_STORY_BEFORE             0
#define GP_STORY_TOWN               1
#define GP_STORY_PLAN               2

#define GP_STORY_OFF              (1)
#define GP_DEBUG_ON               (1<< 1)
#define GP_DEMO                   (1<< 2)
#define GP_LEVEL_DESIGN           (1<< 3)
#define GP_GUARD_DESIGN           (1<< 4)
#define GP_COORDINATES            (1<< 5)
#define GP_DEBUG_PLAYER           (1<< 6)
#define GP_FULL_ENV               (1<< 7)
#define GP_MUSIC_OFF              (1<< 8)
#define GP_NO_MUSIC_IN_PLANING    (1<<10)
#define GP_NO_SAMPLES             (1<<11)
#define GP_COLLISION_CHECKING_OFF (1<<12)
#define GP_SHOW_ROOMS             (1<<13)

/* Zugriffsdefines */

#define SetMinute(zeit)         (film->akt_Minute=(uint32)(zeit))
#define SetLocation(loc)    {film->alter_Ort = film->akt_Ort; film->akt_Ort=(uint32)(loc);}
#define SetDay(tag)             (film->akt_Tag=(uint32)(tag))
#define SetTime(time)       (film->akt_Minute=(uint32)(time))

#define GetDay                  (film->akt_Tag)
#define GetMinute               (film->akt_Minute)
#define GetLocation             (film->akt_Ort)
#define GetOldLocation          (film->alter_Ort)

#define GetFromDay(x)            ((x) >> 16)
#define GetToDay(x)             (((x) << 16) >> 16)

struct Film {
	uint32 AmountOfScenes;

	struct Scene *act_scene;
	struct Scene *gameplay;

	LIST *loc_names;        /* Liste aller Orte im Spiel */
	/* OrtNr = Nr der Node in der */
	/* Liste */
	uint32 StartScene;
	uint32 StartZeit;       /* =Tag seit dem Jahr 0 */
	uint32 StartOrt;

	uint32 akt_Tag;
	uint32 akt_Minute;
	uint32 akt_Ort;
	uint32 alter_Ort;

	uint32 EnabledChoices;

	byte StoryIsRunning;
};

struct SceneArgs {
	uint32 _options;
	uint32 _returnValue;     /* is ALSO (!) used as input, when used as Output = EventNr the successor Scene */
	bool _overwritten;       /* False...direct descendant, True......overwritten method */
};

struct Scene {
	uint32 EventNr;

	void (*Init)();
	void (*Done)();

	struct Bedingungen *bed;    /* damit das Ereignis eintritt */

	LIST *std_succ;     /* Standardnachfolger TCEventNode */

	uint32 Moeglichkeiten;      /* siehe defines oben                   */
	uint32 Dauer;           /* Dauer dieser Szene in Sekunden       */
	uint16 Anzahl;      /* wie oft sie geschehen kann           */
	uint16 Geschehen;       /* wie oft sie SCHON geschehen ist */
	byte Probability;       /* mit der sie eintritt         0-255   */

	uint32 LocationNr;      /* Ort, den diese Szene darstellt       */
	/* == -1 falls Szene = StorySzene       */
	/* ansonsten Nr des Ortes               */
};

struct Bedingungen {
	uint32 Ort;         /* der erfüllt sein muß */

	LIST *events;       /* welche Events schon geschehen sein muessen */
	LIST *n_events;     /* Events, die nicht geschehen sein dürfen */
};

struct TCEventNode {
	NODE Node;

	uint32 EventNr;
};

/* global functions */
void InitStory(const char *story_filename);
extern void CloseStory();

extern uint32 PlayStory();
extern void PatchStory();

extern void SetEnabledChoices(uint32 ChoiceMask);

extern void StdDone();
extern void StdInit();

extern void RefreshCurrScene();
extern void SetCurrentScene(struct Scene *scene);

extern char *GetCurrLocName();

char *BuildDate(uint32 days, char *date);
char *BuildTime(uint32 min, char *time);

void FormatDigit(uint32 digit, char *s);

extern struct Scene *GetCurrentScene();
extern struct Scene *GetLocScene(uint32 locNr);
extern struct Scene *GetScene(uint32 EventNr);

extern void AddVTime(uint32 Zeit);

extern void LinkScenes();   /* Init und Done in jeder Scene Struktur setzen */

extern struct SceneArgs _sceneArgs;
extern struct Film *film;
extern uint32 GamePlayMode;
extern byte RefreshMode;

} // End of namespace Clue

#endif

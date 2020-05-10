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

#include "clue/story/story.h"
#include "clue/text.h"

namespace Clue {
class NewScene;
class Scene;

// Constants
#define GP_CHOICE_GO            (1)
#define GP_CHOICE_WAIT          (1<<1)
#define GP_CHOICE_BUSINESS_TALK (1<<2)
#define GP_CHOICE_LOOK          (1<<3)
#define GP_CHOICE_INVESTIGATE   (1<<4)
#define GP_CHOICE_PLAN          (1<<5)
#define GP_CHOICE_CALL_TAXI     (1<<6)
#define GP_CHOICE_MAKE_CALL     (1<<7)
#define GP_CHOICE_INFO          (1<<8)
#define GP_CHOICE_SLEEP         (1<<9) // Unused

#define GP_ALL_CHOICES          GP_CHOICE_GO + GP_CHOICE_WAIT + GP_CHOICE_BUSINESS_TALK + GP_CHOICE_LOOK + GP_CHOICE_INVESTIGATE + GP_CHOICE_PLAN + GP_CHOICE_CALL_TAXI + GP_CHOICE_MAKE_CALL + GP_CHOICE_INFO + GP_CHOICE_SLEEP

/* Szene kann UNENDLICH oft geschehen */
#define CAN_ALWAYS_HAPPEN       USHRT_MAX

#define MINUTES_PER_DAY          1440

#define GP_STORY_BEFORE             0
#define GP_STORY_TOWN               1
#define GP_STORY_PLAN               2

#define GP_MODE_DISABLE_STORY        (1)
// #define GP_DEBUG_ON               (1<< 1) // Unused
#define GP_MODE_DEMO                 (1<< 2)
#define GP_MODE_LEVEL_DESIGN         (1<< 3) // CHECKME : Should we keep that? If no : deadcode to be remove. If yes: to be set in the console
#define GP_MODE_GUARD_DESIGN         (1<< 4) // CHECKME : Should we keep that? If no : deadcode to be remove. If yes: to be set in the console
// #define GP_COORDINATES            (1<< 5) // Unused
// #define GP_DEBUG_PLAYER           (1<< 6) // Unused
#define GP_MODE_FULL_ENV             (1<< 7) // CHECKME: never set (to be confirmed), hiding dead code
#define GP_MODE_MUSIC_OFF            (1<< 8) // TODO: set this mode based on ScummVM's config
#define GP_MODE_NO_MUSIC_IN_PLANING  (1<<10) // CHECKME: never set (to be confirmed), hiding dead code
#define GP_MODE_NO_SAMPLES           (1<<11) // CHECKME: never set, leads to useless check
#define GP_MODE_DISABLE_COLLISION    (1<<12) // TODO: allow to set this mode in the console
// #define GP_SHOW_ROOMS             (1<<13) // Unused

// And finally the classes
class Film {
public:
	Film();

	uint32 _amountOfScenes;

	Scene *_currScene;
	Scene *_gameplay;

	NewList<NewTCEventNode>* _locationNames;        /* Liste aller Orte im Spiel */
	/* OrtNr = Nr der Node in der */
	/* Liste */
	uint32 _startScene;
	uint32 _startTime;       /* =Tag seit dem Jahr 0 */

private:
	uint32 _currHour;
	uint32 _currMinute;
	uint32 _currLocation;
	uint32 _oldLocation;
	uint32 _enabledChoices;

public:

	byte _storyIsRunning;

	void setMinute(uint32 zeit)  { _currMinute = zeit; }
	void setLocation(uint32 loc) { _oldLocation = _currLocation; _currLocation = loc; }
	void setCurrLocation(uint32 loc) { _currLocation = loc; }
	void setOldLocation(uint32 loc) { _oldLocation = loc; }
	void setDay(uint32 tag)      { _currHour = tag; }
	void setTime(uint32 time)    { _currMinute = time; }

	uint32 getDay()              { return _currHour; }
	uint32 getMinute()           { return _currMinute; }
	uint32 getLocation()         { return _currLocation; }
	uint32 getOldLocation()      { return _oldLocation; }

	void setEnabledChoices(uint32 ChoiceMask);
	uint32 getEnabledChoices() { return _enabledChoices; }

	void setCurrentScene(Scene* scene);
	void InitLocations();
};

struct SceneArgs {
	uint32 _options;
	uint32 _returnValue;     /* is ALSO (!) used as input, when used as Output = EventNr the successor Scene */
	bool _overwritten;       /* False...direct descendant, True......overwritten method */
};

class Conditions {
public:
	uint32 _location;			/* mandatory condition */

	NewList<NewTCEventNode>* _triggerEvents;	/* those events must have already happened */
	NewList<NewTCEventNode>* _blockerEvents;	/* those events shouldn't have happened */

	Conditions() { _location = 0; _triggerEvents = _blockerEvents = nullptr; }
};

class Scene {
public:
	uint32 _eventNr;

	void (*initFct)();
	void (*doneFct)();

	Conditions *_cond;    /* conditions to trigger the event      */

	NewList<NewTCEventNode> *_nextEvents;     /* Standard successor TCEventNode */

	uint32 _options;			/* See defines above                    */
	uint32 _duration;			/* Duration of the scene in seconds     */
	uint16 _quantity;			/* How often it may happen              */
	uint16 _occurrence;			/* How many times it already happened   */
	uint8 _probability;			/* in the range 0 -255                  */

	uint32 _locationNr;			/* Location of the scene == -1 if Scene = StoryScene otherwise, location number */

	Scene() { initFct = nullptr; doneFct = nullptr;  _cond = nullptr; _nextEvents = nullptr; _eventNr = _options = _duration = _quantity = _occurrence = _probability = _locationNr = 0; }

	bool CheckConditions();
	void InitConditions(NewScene* ns);
	void FreeConditions();
	void SetFunc(void (*init)(), void (*done)());
};

/* global functions */
void initStory(const char *story_filename);
extern void closeStory();

extern uint32 playStory();
extern void patchStory();

extern void stdDone();
extern void stdInit();

extern void refreshCurrScene();

extern Common::String getCurrLocName();

Common::String buildDate(uint32 days);
Common::String buildTime(uint32 min);
Common::String formatDigit(uint32 digit);

extern Scene *getCurrentScene();
extern Scene *getLocScene(uint32 locNr);
extern Scene *getScene(uint32 EventNr);

extern void addVTime(uint32 time);

extern void linkScenes();   /* set init and done in each scene */

extern SceneArgs _sceneArgs;
extern Film *_film;
extern uint32 _gamePlayMode;
extern byte _refreshMode;

} // End of namespace Clue

#endif

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

void FreeLocations();

void InitSceneInfo();
void FreeSceneInfo();

void PrepareStory(const char *filename);
int32 GetEventCount(uint32 EventNr);
void EventDidHappen(uint32 EventNr);

Scene *GetStoryScene(Scene *scene);

uint32 _gamePlayMode = 0;
byte _refreshMode = 0;

Film *_film = nullptr;
SceneArgs _sceneArgs;

NewScene::NewScene() {
	_eventNr = 0;
	_sceneName[0] = '\0';
	_day = 0;
	_minTime = 0;
	_maxTime = 0;
	_location = 0;
	_eventCounter = 0;
	_blockerEventsCounter = 0;
	_events = nullptr;
	_blockerEvents = nullptr;
	_nextEventCounter = 0;
	_nextEvents = nullptr;
	_options = 0;
	_duration = 0;
	_quantity = 0;
	_occurrence = 0;
	_probability = 0;
	_sample = 0;
	_anim = 0;
	_locationNr = 0;
}

NewScene::~NewScene() {
	delete[] _events;
	delete[] _blockerEvents;
	delete[] _nextEvents;
}

Film::Film() {
	_amountOfScenes = 0;

	_currScene = nullptr;
	_gameplay = nullptr;
	_locationNames = nullptr;

	_startScene = 0;
	_startTime = 0;

	_currHour = 0;
	_currMinute = 0;
	_currLocation = 0;
	_oldLocation = 0;

	_enabledChoices = 0;
	_storyIsRunning = GP_STORY_BEFORE;
}

void StoryHeader::load(Common::Stream* file) {
	dskRead(file, _storyName, sizeof(_storyName));
	dskRead_U32LE(file, &_eventCount);
	dskRead_U32LE(file, &_sceneCount);

	dskRead_U32LE(file, &_amountOfScenes);
	dskRead_U32LE(file, &_amountOfEvents);

	dskRead_U32LE(file, &_startTime);
	dskRead_U32LE(file, &_startLocation);
	dskRead_U32LE(file, &_startScene);
}

void initStory(const char *story_filename) {
	closeStory();

	_film = new Film;
	_film->setEnabledChoices(GP_ALL_CHOICES);

	PrepareStory(story_filename);
	_film->InitLocations();
	LinkScenes();
	patchStory();
}

void closeStory() {
	if (!_film)
		return;

	if (_film->_locationNames) {
		_film->_locationNames->removeList();
		delete _film->_locationNames;
		_film->_locationNames = nullptr;
	}

	for (uint32 i = 0; i < _film->_amountOfScenes; i++) {
		if (_film->_gameplay[i]._cond)
			_film->_gameplay[i].FreeConditions();
		if (_film->_gameplay[i]._nextEvents) {
			delete _film->_gameplay[i]._nextEvents;
			_film->_gameplay[i]._nextEvents = nullptr;
		}
	}

	delete[] _film->_gameplay;
	delete _film;
	_film = nullptr;
}

void Film::setEnabledChoices(uint32 ChoiceMask) {
	_enabledChoices = ChoiceMask;
}

void refreshCurrScene() {
	const uint32 curLoc = _film->getLocation();
	NewNode *node = _film->_locationNames->getNthNode(curLoc);

	tcRefreshLocationInTitle(curLoc);
	PlayAnim(node->_name.c_str(), 30000, GFX_NO_REFRESH | GFX_ONE_STEP | GFX_BLEND_UP);

	RefreshMenu();
}

void Film::InitLocations() {
	NewList<NewTCEventNode> *l = new NewList<NewTCEventNode>;
	char pathname[DSK_PATH_MAX];

	dskBuildPathName(DISK_CHECK_FILE, TEXT_DIRECTORY, LOCATIONS_TXT, pathname);
	l->readList(pathname);

	_locationNames = l;
}

void patchStory() {
	if (_gamePlayMode & GP_MODE_DEMO)
		return;

	getScene(SCENE_4TH_BURG)->_cond->_location = 3;  /* 4th Burglary, Hotel room */
	getScene(SCENE_ARRESTED_MATT)->_cond->_location = 7;  /* Arrest, Police!          */

	// TODO : set _options properly using the defined values
	getScene(SCENE_KASERNE_OUTSIDE)->_options = 15;
	getScene(SCENE_KASERNE_INSIDE)->_options = 265;

	getScene(SCENE_KASERNE_OUTSIDE)->_locationNr = 66;
	getScene(SCENE_KASERNE_INSIDE)->_locationNr = 65;

	getScene(SCENE_KASERNE_OUTSIDE)->_duration = 17;
	getScene(SCENE_KASERNE_INSIDE)->_duration = 57;

	getScene(SCENE_STATION)->_options |= GP_CHOICE_WAIT;

	if (g_clue->getFeatures() & GF_PROFIDISK)
		getScene(SCENE_PROFI_26)->_locationNr = 75;

	/* change possibilites in story_9 too! */
	/* für die Kaserne hier einen Successor eingetragen! */
	getLocScene(65)->_nextEvents = new NewList<NewTCEventNode>;
	NewTCEventNode *node = getLocScene(65)->_nextEvents->createNode(nullptr);
	node->_eventNr = SCENE_KASERNE_OUTSIDE;  /* wurscht... */

	getLocScene(66)->_nextEvents = new NewList<NewTCEventNode>;
	node = getLocScene(66)->_nextEvents->createNode(nullptr);
	node->_eventNr = SCENE_KASERNE_INSIDE;   /* wurscht... */

	_film->_startScene = SCENE_STATION;
}

uint32 playStory() {
	Scene *curr, *next = nullptr;
	Scene *story_scene = nullptr;
	bool interr_allowed = true;
	bool first = true;

	if (_gamePlayMode & GP_MODE_DISABLE_STORY) {
		if (_gamePlayMode & GP_MODE_DEMO)
			curr = getScene(SCENE_CARS_VANS);
		else {
			tcAddPlayerMoney(5000);
			curr = getScene(SCENE_HOTEL_ROOM);
		}
	} else
		curr = getScene(_film->_startScene);

	_film->setCurrentScene(curr);
	_film->setOldLocation(curr->_locationNr);
	_film->setLocation((uint32) -2);

	while (curr->_eventNr != SCENE_THE_END && curr->_eventNr != SCENE_NEW_GAME) {
		if (!curr->CheckConditions())
			ErrorMsg(Internal_Error, ERROR_MODULE_GAMEPLAY, 2);

		/* Entscheidung für eine Szene getroffen ! */

		_sceneArgs._returnValue = 0;
		_sceneArgs._overwritten = false;

		/* wenn Szene Storyszene ist, dann Musik beibehalten */
		/* (Storyszenen ändern sie selbst in Done */
		/* ansonsten wird die Musi hier verändert */

		if (!story_scene)
			tcPlaySound();

		/* ab jetzt soll nach einem Diskettenwechsel refresht werden */
		/* -> GP_STORY_TOWN Flag setzen */
		/* Achtung: dieses Flag nur nach der 1.Szene setzen -> "first" */

		if (first) {
			_film->_storyIsRunning = GP_STORY_TOWN;
			first = false;
		}

		/* die neue Szene initialisieren ! ( Bühnenbild aufbauen ) */
#ifdef DEEP_DEBUG
		printf("----------------------------------------\n");
		printf("SCENE_INIT %u\n", curr->EventNr);
#endif
		if (curr->initFct)
			curr->initFct();

		/* gibt es irgendein Ereignis das jetzt geschehen kann, und */
		/* den Standardablauf unterbricht ? */

		/* if (_gamePlayMode & GP_DEMO)
		   DemoDialog(); */

		story_scene = nullptr;

		if (interr_allowed && !(_gamePlayMode & GP_MODE_DISABLE_STORY)) {
#ifdef DEEP_DEBUG
			printf("STEP_A1\n");
#endif
			story_scene = GetStoryScene(curr);
			if (story_scene)
				interr_allowed = false;
		}
#ifdef DEEP_DEBUG
		else {
			printf("STEP_A2\n");
		}
#endif

		if (!story_scene) {
			_sceneArgs._overwritten = false;

#ifdef DEEP_DEBUG
			printf("STEP_B\n");
#endif
			if (curr->doneFct) {
				_sceneArgs._options = (curr->_options & _film->getEnabledChoices());

#ifdef DEEP_DEBUG
				printf("SCENE_DONE\n");
#endif
				curr->doneFct();   /* Funktionalität */
			} else
				ErrorMsg(Internal_Error, ERROR_MODULE_GAMEPLAY, 3);

			EventDidHappen(curr->_eventNr);
			addVTime(curr->_duration);

			/* jetzt kann wieder unterbrochen werden ! */
			interr_allowed = true;

			/* der Spieler ist fertig mit dieser Szene - aber wie geht es weiter ? */
			if (_sceneArgs._returnValue)
				next = getScene(_sceneArgs._returnValue); /* Nachfolger festlegen */
			else
				ErrorMsg(Internal_Error, ERROR_MODULE_GAMEPLAY, 4);
		} else
			next = story_scene; /* Unterbrechung durch die Storyszene ! */

		curr = next;
		_film->setCurrentScene(curr);
	}               /* While Schleife */

	_film->_storyIsRunning = GP_STORY_BEFORE;

	StopAnim();

	return curr->_eventNr;
}

Scene *GetStoryScene(Scene *curr) {
	for (uint32 i = 0; i < _film->_amountOfScenes; i++) {
		Scene* sc = &_film->_gameplay[i];
		if (sc->_locationNr == (uint32) -1 && sc != curr) {
			uint8 j = g_clue->calcRandomNr(0, 255);

			if (j <= sc->_probability && sc->CheckConditions())
				return sc;
		}
	}

	return nullptr;
}

Scene *getScene(uint32 EventNr) {
	Scene *sc = nullptr;

	for (uint32 i = 0; i < _film->_amountOfScenes; i++) {
		if (EventNr == _film->_gameplay[i]._eventNr)
			sc = &_film->_gameplay[i];
	}

	return sc;
}

int32 GetEventCount(uint32 EventNr) {
	Scene *sc = getScene(EventNr);

	if (sc)
		return (int32)sc->_occurrence;

	return 0;
}

void EventDidHappen(uint32 EventNr) {
	uint16 max = CAN_ALWAYS_HAPPEN;
	Scene *sc = getScene(EventNr);

	if (sc && sc->_occurrence < max)
		++sc->_occurrence;
}

bool Scene::CheckConditions() {
	/* wenn Std Szene, dann muß nichts überprüft werden ! */

	if (_locationNr != (uint32) -1)
		return true;

	/* es handelt sich um keine Std Szene -> Überprüfen ! */
	/* überprüfen, ob Szene nicht schon zu oft geschehen ist ! */
	if (_quantity != (uint16)CAN_ALWAYS_HAPPEN && _occurrence >= _quantity)
		return false;

	/* Jetzt die einzelnen Bedingungen überprüfen */
	Conditions *cond = _cond;
	if (!cond)
		return true;

	// If there's a condition on the location and it's not fulfilled
	if (cond->_location != (uint32)-1 && _film->getLocation() != cond->_location)
		return false;

	/*
	 * Überprüfen, ob ein Event eingetreten ist,
	 * das nicht geschehen darf !
	 */

	if (cond->_blockerEvents) {
		for (NewTCEventNode *node = cond->_blockerEvents->getListHead(); node->_succ; node = (NewTCEventNode *)node->_succ) {
			if (GetEventCount(node->_eventNr))
				return false;
		}
	}

	/*
	 * Check if all the mandatory condition events have been triggered
	 *
	 */

	if (cond->_triggerEvents) {
		for (NewTCEventNode *node = cond->_triggerEvents->getListHead(); node->_succ; node = (NewTCEventNode *)node->_succ) {
			if (!GetEventCount(node->_eventNr))
				return false;
		}
	}

	return true;
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

	StoryHeader *storyHeader = new StoryHeader;
	storyHeader->load(file);

	_film->_amountOfScenes = storyHeader->_amountOfScenes;
	_film->_startTime = storyHeader->_startTime;
	_film->setTime(543); /* 09:03 */
	_film->setDay(_film->_startTime);
	_film->setCurrLocation(storyHeader->_startLocation);
	_film->_startScene = storyHeader->_startScene;

	delete storyHeader;
	storyHeader = nullptr;
	
	NewScene* NS = nullptr;
	if (_film->_amountOfScenes) {
		_film->_gameplay = new Scene[_film->_amountOfScenes];
	} else
		ErrorMsg(Disk_Defect, ERROR_MODULE_GAMEPLAY, 7);

	for (uint32 i = 0; i < _film->_amountOfScenes; i++) {
		NS = new NewScene;
		NS->LoadSceneforStory(file);

		Scene *scene = &(_film->_gameplay[i]);

		scene->_eventNr = NS->_eventNr;

		if (NS->_locationNr == (uint32) -1 || NS->_eventCounter || NS->_blockerEventsCounter) /* Storyszene ? */
			scene->InitConditions(NS); /* ja ! -> Bedingungen eintragen */
		else
			_film->_gameplay[i]._cond = nullptr;   /* Spielablaufszene : keine Bedingungen ! */
		
		/* Scene Struktur füllen : */
		scene->doneFct = stdDone;
		scene->initFct = stdInit;
		scene->_options = NS->_options;
		scene->_duration = NS->_duration;
		scene->_quantity = NS->_quantity;
		scene->_occurrence = NS->_occurrence;
		scene->_probability = NS->_probability;
		scene->_locationNr = NS->_locationNr;

		/* Nachfolgerliste aufbauen !  */
		/* Achtung! auch Patch ändern! */

		if (NS->_nextEventCounter) {
			scene->_nextEvents = new NewList<NewTCEventNode>;

			for (uint32 j = 0; j < NS->_nextEventCounter; j++) {
				NewTCEventNode *node = scene->_nextEvents->createNode(nullptr);
				node->_eventNr = NS->_nextEvents[j];
			}

			delete[] NS->_nextEvents;
			NS->_nextEvents = nullptr;
		} else
			scene->_nextEvents = nullptr;
	}

	/* von den Events muß nichts geladen werden ! */
	dskClose(file);
	delete NS;
}

void Scene::InitConditions(NewScene *ns) {
	Conditions* newCond = new Conditions;

	newCond->_location = ns->_location;

	if (ns->_eventCounter) {
		newCond->_triggerEvents = new NewList<NewTCEventNode>;

		for (uint32 i = 0; i < ns->_eventCounter; i++) {
			NewTCEventNode *node = newCond->_triggerEvents->createNode(nullptr);
			node->_eventNr = ns->_events[i];
		}

		delete[] ns->_events;
		ns->_events = nullptr;
	} else
		newCond->_triggerEvents = nullptr;

	if (ns->_blockerEventsCounter) {
		newCond->_blockerEvents = new NewList<NewTCEventNode>;

		for (uint32 i = 0; i < ns->_blockerEventsCounter; i++) {
			NewTCEventNode *node = newCond->_blockerEvents->createNode(nullptr);
			node->_eventNr = ns->_blockerEvents[i];
		}

		delete[] ns->_blockerEvents;
		ns->_blockerEvents = nullptr;
	} else
		newCond->_blockerEvents = nullptr;

	_cond = newCond;
}

void Scene::FreeConditions() {
	if (_cond) {
		if (_cond->_triggerEvents) {
			_cond->_triggerEvents->removeList();
			delete _cond->_triggerEvents;
			_cond->_triggerEvents = nullptr;
		}

		if (_cond->_blockerEvents) {
			_cond->_blockerEvents->removeList();
			delete _cond->_blockerEvents;
			_cond->_blockerEvents = nullptr;
		}

		delete _cond;
		_cond = nullptr;
	}
}

void NewScene::LoadSceneforStory(Common::Stream *file) {
	dskRead_U32LE(file, &_eventNr);

	dskRead(file, _sceneName, sizeof(_sceneName));

	dskRead_S32LE(file, &_day);
	dskRead_S32LE(file, &_minTime);
	dskRead_S32LE(file, &_maxTime);
	dskRead_U32LE(file, &_location);

	dskRead_U32LE(file, &_eventCounter);
	dskRead_U32LE(file, &_blockerEventsCounter);

	uint32 dummy;
	dskRead_U32LE(file, &dummy);
	dskRead_U32LE(file, &dummy);

	dskRead_U32LE(file, &_nextEventCounter);

	dskRead_U32LE(file, &dummy);

	dskRead_U32LE(file, &_options);
	dskRead_U32LE(file, &_duration);

	dskRead_U16LE(file, &_quantity);
	dskRead_U16LE(file, &_occurrence);

	dskRead_U8(file, &_probability);

	dskRead_U32LE(file, &_sample);
	dskRead_U32LE(file, &_anim);
	dskRead_U32LE(file, &_locationNr);

	/* allocate mem for events and read them */
	uint32 *event_nrs = nullptr;
	if (_eventCounter) {
		event_nrs = new uint32[_eventCounter];
		for (uint32 i = 0; i < _eventCounter; ++i)
			dskRead_U32LE(file, &event_nrs[i]);
	}
	_events = event_nrs;

	event_nrs = nullptr;
	/* allocate mem for _blockerEvents and read them */
	if (_blockerEventsCounter) {
		event_nrs = new uint32[_blockerEventsCounter];
		for (uint32 i = 0; i < _blockerEventsCounter; ++i)
			dskRead_U32LE(file, &event_nrs[i]);
	}
	_blockerEvents = event_nrs;

	event_nrs = nullptr;
	/* allocate mem for successors and read them */
	if (_nextEventCounter) {
		event_nrs = new uint32[_nextEventCounter];
		for (uint32 i = 0; i < _nextEventCounter; ++i)
			dskRead_U32LE(file, &event_nrs[i]);
	}

	_nextEvents = event_nrs;
}

void addVTime(uint32 add) {

	uint32 time = _film->getMinute() + add;

	if (time >= MINUTES_PER_DAY) {
		uint32 tag = _film->getDay() + time / MINUTES_PER_DAY;

		_film->setDay(tag);
		time = time % MINUTES_PER_DAY;
	}

	_film->setTime(time);
	tcTheAlmighty(time);
}

void Film::setCurrentScene(Scene *scene) {
	_currScene = scene;
}

Scene *getCurrentScene() {
	return _film ? _film->_currScene : nullptr;
}

Scene *getLocScene(uint32 locNr) {

	for (uint32 i = 0; i < _film->_amountOfScenes; i++) {
		Scene *sc = &_film->_gameplay[i];
		if (sc->_locationNr == locNr)
			return sc;
	}
	ErrorMsg(Internal_Error, ERROR_MODULE_GAMEPLAY, 12);
	return nullptr;
}

Common::String FormatDigit(uint32 digit) {
	Common::String s;
	if (digit < 10)
		s = Common::String::format("0%u", digit);
	else
		s = Common::String::format("%u", digit);

	return s;
}

Common::String buildTime(uint32 min) {
	uint32 h = (min / 60) % 24;
	min %= 60;

	Common::String time = FormatDigit(h) + ':' + FormatDigit(min);

	return time;
}

Common::String buildDate(uint32 days) {
	const uint8 days_per_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	uint32 p_year = 0;
	for (uint32 i = 0; i < 12; i++)
		p_year += days_per_month[i];

	uint32 year = days / p_year;   /* which daywelches Jahr ? */
	days = days % p_year;   /* wieviele Tage noch in diesem Jahr */

	uint32 month = 0;

	p_year = 0;
	for (uint32 i = 0; i < 12; i++) {
		p_year += days_per_month[i];

		if (days < p_year) {
			month = i;
			break;
		}
	}

	uint32 day = days - (p_year - days_per_month[month]);

	Common::String date = FormatDigit(day + 1) + '.' + FormatDigit(month + 1) + '.' + FormatDigit(year);

	return date;
}

Common::String getCurrLocName() {
	uint32 index = getCurrentScene()->_locationNr;
	return _film->_locationNames->getNthNode(index)->_name;
}

} // End of namespace Clue

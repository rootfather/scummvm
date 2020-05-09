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

	uint32 _eventCount;      /* Total number of events */
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
	uint32 _eventNr;
	uint8 _sceneName[20];

	int32 _day;			/* the day of occurrence */
	int32 _minTime;		/* Time range in which it occurs */
	int32 _maxTime;		/*                               */
	uint32 _location;	/* mandatory condition */

	uint32 _eventCounter;
	uint32 _blockerEventsCounter;

	uint32 *_events;		/* Nr. der Events, die erfüllt sein müssen */
	uint32 *_blockerEvents;	/* Nr. der Events, die nicht erfüllt sein müssen */

	uint32 _nextEventCounter;
	uint32 *_nextEvents;	/* Nr. der NachfolgerEvents */

	uint32 _options;		/* Based on GP_CHOICEs definition */
	uint32 _duration;		/* Duration of the scene in minutes */
	uint16 _quantity;		/* maximum number of occurrences */
	uint16 _occurrence;		/* number of past occurrences */
	uint8 _probability;		/* occurrence probability 0-255 */

	uint32 _sample;			/* Number of Samples (unused)*/
	uint32 _anim;			/* Number of animations (unused) */
	uint32 _locationNr;		/* Location of the scene */

	NewScene();
	~NewScene();
};

} // End of namespace Clue

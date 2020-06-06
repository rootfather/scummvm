/*
**  $Filename: living\living.ph
**  $Release:  0
**  $Revision: 0.1
**  $Date:     13-04-94
**
**  "sprite" functions for "Der Clou!"
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

namespace Clue {

#define LIV_LOAD_BUFFER         StdBuffer0  /* muß ein StdBuffer sein -> sonst ClearBuffer in .c ändern */
#define LIV_DECR_BUFFER         StdBuffer1

#define LIV_TEMPLATE_BUFFER StdBuffer1  /* auch LS_LOOTBAG_BUFFER ändern!!! */

#define LIV_NO_FRAME                ((uint16) -1)

#define LIV_COLL_WIDTH          308 /* ein Frame ist 14 * 14 */

#define LIV_COLL_NAME           ("allmaxi")

#define LIV_TEMPL_BULLE_NAME    ("bullemax")

class NewAnimTemplate : public NewNode {
public:
	uint16 _width;
	uint16 _height;
	uint16 _frameOffsetNr;

	NewAnimTemplate() { _width = _height = _frameOffsetNr = 0; }
	~NewAnimTemplate() {}

	void livRemTemplate() { /* dummy function */ };
};

class NewLiving : public NewNode {
public:
	/* komplette Daten einer Instanz   *//* eines Lebewesens                */
	// Node Link;

	uint32 _livesInAreaId;    /* Area -> LandScap */
	uint16 _livingNr;

	NewAnimTemplate *_originTemplate;

	byte _xSize;
	byte _ySize;

	int16 _xSpeed;
	int16 _ySpeed;

	uint16 _xPos;     /* absolut */
	uint16 _yPos;

	byte _viewDirection; /* 0 .. left, right, up, down */

	byte _action;
	byte _oldAction;

	char _currFrameNr;

	byte _status;        /* enabled or disabled */

	NewLiving() { _originTemplate = nullptr; } // TODO : initialize the other members properly and move the constructor to the CPP file
	~NewLiving() {}

	void livAnimate(byte action, int16 xSpeed, int16 ySpeed);
	void livCorrectViewDirection();
	void livAdd(Common::String templateName, byte xSize, byte ySize, int16 xSpeed, int16 ySpeed);
	void livRem();
	void livHide();
	void livShow();
	bool livIsVisible();
};

class SpriteControl {
public:
	NewList<NewLiving> *_livings;
	NewList<NewAnimTemplate> *_template;

	uint32 _sprPlayMode;
	uint32 _activAreaId;

	uint16 _visLScapeX;
	uint16 _visLScapeY;
	uint16 _visLScapeWidth;
	uint16 _visLScapeHeight;

	uint16 _totalLScapeWidth;
	uint16 _totalLScapeHeight;

	byte _firstFrame;
	byte _lastFrame;
	byte _frameCount;    /* Anzahl der Frames pro Anim */

	char _playDirection;
};

static SpriteControl *sc = nullptr;

static void livLoadTemplates();
static void livLoadLivings();


} // End of namespace Clue

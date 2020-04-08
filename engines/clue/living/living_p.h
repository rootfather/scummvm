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


struct SpriteControl {
	NewList<NewLiving> *p_Livings;
	NewList<NewAnimTemplate> *p_Template;

	uint32 ul_SprPlayMode;
	uint32 ul_ActivAreaId;

	uint16 us_VisLScapeX;
	uint16 us_VisLScapeY;
	uint16 us_VisLScapeWidth;
	uint16 us_VisLScapeHeight;

	uint16 us_TotalLScapeWidth;
	uint16 us_TotalLScapeHeight;

	byte uch_FirstFrame;
	byte uch_LastFrame;
	byte uch_FrameCount;    /* Anzahl der Frames pro Anim */

	char ch_PlayDirection;
};

static struct SpriteControl *sc = nullptr;

static void livLoadTemplates();
static void livLoadLivings();


} // End of namespace Clue

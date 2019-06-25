/*
**	$Filename: living\living.ph
**	$Release:  0
**	$Revision: 0.1
**	$Date:     13-04-94
**
**	"sprite" functions for "Der Clou!"
**
**   (c) 1994 ...and avoid panic by, H. Gaberschek
**	    All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#define LIV_LOAD_BUFFER			StdBuffer0	/* muá ein StdBuffer sein -> sonst ClearBuffer in .c „ndern */
#define LIV_DECR_BUFFER			StdBuffer1

#define LIV_TEMPLATE_BUFFER	StdBuffer1	/* auch LS_LOOTBAG_BUFFER „ndern!!! */

#define LIV_NO_FRAME				((uint16) -1)

#define LIV_COLL_WIDTH			308	/* ein Frame ist 14 * 14 */

#define LIV_COLL_NAME			("allmaxi")

#define LIV_TEMPL_BULLE_NAME	("bullemax")


struct SpriteControl {
    LIST *p_Livings;
    LIST *p_Template;

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
    byte uch_FrameCount;	/* Anzahl der Frames pro Anim */

    char ch_PlayDirection;
};

struct AnimTemplate {
    NODE Link;

    uint16 us_Width;
    uint16 us_Height;

    uint16 us_FrameOffsetNr;
};

struct Living {			/* komplette Daten einer Instanz   *//* eines Lebewesens                */
    NODE Link;

    uint32 ul_LivesInAreaId;	/* Area -> LandScap */

    uint16 us_LivingNr;

    struct AnimTemplate *p_OriginTemplate;

    byte uch_XSize;
    byte uch_YSize;

    int16 s_XSpeed;
    int16 s_YSpeed;

    uint16 us_XPos;		/* absolut */
    uint16 us_YPos;

    byte uch_ViewDirection;	/* 0 .. left, right, up, down */

    byte uch_Action;
    byte uch_OldAction;

    char ch_CurrFrameNr;

    byte uch_Status;		/* enabled or disabled */
};

static struct SpriteControl *sc = NULL;

static struct Living *livGet(char *uch_Name);

static void livLoadTemplates(void);
static void livLoadLivings(void);
static void livRemTemplate(struct AnimTemplate *tlt);
static void livRem(struct Living *liv);
static void livHide(struct Living *liv);
static void livShow(struct Living *liv);

static void livAdd(char *uch_Name, char *uch_TemplateName, byte uch_XSize,
		   byte uch_YSize, int16 s_XSpeed, int16 s_YSpeed);

static byte livIsVisible(struct Living *liv);

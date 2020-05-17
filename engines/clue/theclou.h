/*
 * TheClou.h
 *
 * (c) 1993 K. Kazemi, H. Gaberschek
 *
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef THECLOU_MODULE
#define THECLOU_MODULE
/*
 * verwendete Typen
 */
#include "common/scummsys.h"

namespace Clue {

#define EOS                     ((char)'\0')

/*
 * Textdateien, die auch als Daten verwendet werden
 *
 */

#define LOCATIONS_TXT       "location.lst"
#define COLL_LIST_TXT       "coll.lst"
#define PICT_LIST_TXT       "pict.lst"
#define GAMES_LIST_TXT      "games.lst"
#define GAMES_ORIG_TXT      "origin.lst"

#define LIV_ANIM_TEMPLATE_LIST  "template.lst"
#define LIV_LIVINGS_LIST        "livings.lst"

#define SND_SOUND_LIST          "sounds.list"
#define SND_SOUND_DIRECTORY     "sounds"

/*
 * Namen der anderen Dateien
 *
 * Diese Dateien befinden sich im Verzeichnis theclou:data
 *
 */

#define STORY_DAT               "tcstory.pc"
#define STORY_DAT_DEMO          "tcstory.dem"

/*
 * Nummern der Textdateien in denen die Menüs stehen
 *
 * siehe Texts.list im Verzeichnis Texts
 *
 */

#define MENU_TXT             0
#define OBJECTS_TXT          1
#define BUSINESS_TXT         2
#define HOUSEDESC_TXT        3
#define THECLOU_TXT          4
#define INVESTIGATIONS_TXT   5
#define CDROM_TXT            6
#define OBJECTS_ENUM_TXT     7
#define ANIM_TXT             8
#define PRESENT_TXT          9
#define STORY_0_TXT         10
#define PLAN_TXT            11
#define TOOLS_TXT           12
#define LOOK_TXT            13
#define ABILITY_TXT         14
#define STORY_1_TXT         15
#define TALK_0_TXT          16
#define TALK_1_TXT          17

/*
 * Disk Nr.
 *
 */

/*
 * Defines betreffend Disketten und Verzeichnisse
 *
 */

#define PICTURE_DIRECTORY   "pictures"
#define TEXT_DIRECTORY      "texts"
#define INCLUDE_DIRECTORY   "include"
#define DATA_DIRECTORY      "data"
#define SOUND_DIRECTORY     "sounds"
#define SAMPLES_DIRECTORY   "samples"
#define INTRO_DIRECTORY     "intropix"

#define DATADISK            "datadisk"

#define BUILD_DATA_NAME     "tcbuild"
#define MAIN_DATA_NAME      "tcmain"
#define GAME_DATA_EXT       ".dat"
#define GAME_REL_EXT        ".rel"
#define TAXI_LOC_EXT        ".loc"

#define STORY_DATA_NAME     "tcstory"

/*
 * defines for Error Module
 *
 */

void ShowTheClouRequester(); /* siehe Base.c */

/*
 * defines for Disk Module
 *
 */

/*
 * some defines ...
 */

#define MATT_PICTID               7
#define OLD_MATT_PICTID         125
#define BIG_SHEET                 5

#define BGD_LONDON               21
#define BGD_PLANUNG              23
#define BGD_EINBRUCH             23
#define BGD_CLEAR                 0

#define RADIO_BUBBLE             12
#define SPEAK_BUBBLE             12
#define THINK_BUBBLE             13

#define GFX_COLL_PARKING         27

} // End of namespace Clue

#endif

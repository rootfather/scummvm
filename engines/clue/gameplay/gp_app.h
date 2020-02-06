/*
**  $Filename: gameplay/gp_app.h
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

#ifndef MODULE_GAMEPLAY_APP
#define MODULE_GAMEPLAY_APP

#include "clue/theclou.h"

#ifndef MODULE_MEMORY
#include "clue/memory/memory.h"
#endif

#ifndef MODULE_ERROR
#include "clue/error/error.h"
#endif

#ifndef MODULE_LIST
#include "clue/list/list.h"
#endif

#ifndef MODULE_DISK
#include "clue/disk/disk.h"
#endif

#ifndef MODULE_GFX
#include "clue/gfx/gfx.h"
#endif

#ifndef MODULE_ANIM
#include "clue/anim/sysanim.h"
#endif

#ifndef MODULE_STORY
#include "clue/story/story.h"
#endif

#ifndef MODULE_DATAAPPL
#include "clue/data/dataappl.h"
#endif

#ifndef MODULE_RANDOM
#include "clue/random/random.h"
#endif

#ifndef MODULE_GAMEPLAY
#include "clue/gameplay/gp.h"
#endif

#ifndef MODULE_SOUND
#include "clue/sound/newsound.h"
#endif

#ifndef MODULE_EVIDENCE
#include "clue/scenes/evidence.h"
#endif

#ifndef MODULE_SCENES
#include "clue/scenes/scenes.h"
#endif

namespace Clue {

struct Scene;

extern void TCGamePause(byte activ);

extern bool tcPersonIsHere(void);

extern void tcPlaySound(void);
extern void tcPlayStreetSound(void);
extern void tcPersonGreetsMatt(void);

extern uint32 StdHandle(uint32 choice);

extern void SetFunc(struct Scene *sc, void (*init)(void), void (*done)(void));
void PrintStatus(const char *text);
extern void InitTaxiLocations(void);
extern void LinkScenes(void);

extern void tcGetLastName(const char *Name, char *dest, uint32 maxLength);
extern void tcCutName(char *Name, byte Sign, uint32 maxLength);

extern bool tcLoadTheClou(void);
extern void tcSaveTheClou(void);
extern uint32 tcBurglary(uint32 buildingID);

extern bool tcLoadChangesInScenes(const char *fileName);
extern bool tcSaveChangesInScenes(const char *fileName);

extern void tcRefreshLocationInTitle(uint32 locNr);
extern void ShowTime(uint32 delay);
extern void RefreshAfterDiskRequester(void);

extern bool tcLoadIt(char activ);
extern void tcRefreshAfterLoad(bool loaded);

extern void ShowMenuBackground(void);

} // End of namespace Clue

#endif

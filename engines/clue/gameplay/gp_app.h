/*
**	$Filename: gameplay/gp_app.h
**	$Release:  0
**	$Revision: 0.1
**	$Date:     08-04-94
**
**
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

struct Scene;

extern void TCGamePause(ubyte activ);

extern ubyte tcPersonIsHere(void);

extern void tcPlaySound(void);
extern void tcPlayStreetSound(void);
extern void tcPersonGreetsMatt(void);

extern U32 StdHandle(U32 choice);

extern void SetFunc(struct Scene *sc, void (*init) (void), void (*done) (void));
void PrintStatus(char *text);
extern void InitTaxiLocations(void);
extern void LinkScenes(void);

extern void tcGetLastName(char *Name, char *dest, U32 maxLength);
extern void tcCutName(char *Name, ubyte Sign, U32 maxLength);

extern ubyte tcLoadTheClou(void);
extern void tcSaveTheClou(void);
extern U32 tcBurglary(U32 buildingID);

extern ubyte tcLoadChangesInScenes(char *fileName);
extern ubyte tcSaveChangesInScenes(char *fileName);

extern void tcRefreshLocationInTitle(U32 locNr);
extern void ShowTime(U32 delay);
extern void RefreshAfterDiskRequester(void);

extern ubyte tcLoadIt(char activ);
extern void tcRefreshAfterLoad(ubyte loaded);

extern void ShowMenuBackground(void);

#endif

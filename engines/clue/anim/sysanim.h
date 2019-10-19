/*
 * newanim.c
 * (c) 1993 by Helmut Gaberschek & Kaweh Kazemi, ...and avoid panic by
 * All rights reserved.
 *
 * new animation module for the PANIC-System
 *
 * Rev   Date        Comment
 * 1     26.08.93    First implementation
 *
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

/*
 * Structure of the Anim.List file (structure of one line/row)
 *
 *  PictureMode,PictsPerSecc,Pic1,AnimPic,PicCount,Animphase
 *  width, Animphase height, Animphase offset, PlayMode
 *  XDest, YDest (as offset to 1st picture)
 */

#ifndef MODULE_ANIM
#define MODULE_ANIM

#include "clue/theclou.h"

#ifndef MODULE_TEXT
#include "clue/text/text.h"
#endif

#include "clue/gfx/gfx.h"

#ifndef MODULE_RANDOM
#include "clue/random/random.h"
#endif

/* global functions */
extern void InitAnimHandler(void);
extern void CloseAnimHandler(void);

extern void PlayAnim(const char *AnimID, uint16 how_often, uint32 mode);    /* -> docs from 16.08.92 ! */
extern void StopAnim(void);

extern void GetAnim(const char *AnimID, char *Dest);
extern void animator(void);

extern void SuspendAnim(void);
extern void ContinueAnim(void);

#endif

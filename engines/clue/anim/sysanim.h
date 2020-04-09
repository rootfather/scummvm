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

#include "clue/text.h"

namespace Clue {

/* global functions */
extern void InitAnimHandler();
extern void CloseAnimHandler();

extern void PlayAnim(const char *AnimID, uint16 how_often, uint32 mode);    /* -> docs from 16.08.92 ! */
extern void StopAnim();

extern Common::String GetAnim(const char *AnimID);
extern void animator();

extern void SuspendAnim();
extern void ContinueAnim();

} // End of namespace Clue

#endif

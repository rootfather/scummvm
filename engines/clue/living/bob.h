/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef TC__BOB_H
#define TC__BOB_H

#include "SDL.h"

struct RastPort;

void BobInitLists(void);

uint16 BobInit(uint16 width, uint16 height);
void BobDone(uint16 BobID);

byte BobSet(uint16 BobID, uint16 xpos, uint16 ypos, uint16 xsrc, uint16 ysrc);

void BobVis(uint16 BobID);
void BobInVis(uint16 BobID);

void BobSetDarkness(byte darkness);
void BobDisplayLists(GC *gc);

#endif

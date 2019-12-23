/****************************************************************************
  Copyright (c) 2001 Thomas Trummer
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#ifndef TC_HSC_H
#define TC_HSC_H

#include "clue/theclou.h"

namespace Clue {

void hscInit(void);
void hscReset(void);
void hscDone(void);
int hscLoad(const char *fileName);

void hscMusicPlayer(void);

} // End of namespace Clue

#endif

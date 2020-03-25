/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#ifndef TC_BUFFER_H
#define TC_BUFFER_H

#include "clue/theclou.h"

namespace Clue {

struct SndBuffer;

SndBuffer *sndCreateBuffer(unsigned size);
void sndResetBuffer(SndBuffer *buffer);

#if 0
unsigned sndLenBuffer(SndBuffer *buffer);
void sndFreeBuffer(SndBuffer *buffer);
unsigned sndInsertBuffer(SndBuffer *buffer, const void *src, unsigned srcLen);
unsigned sndRemoveBuffer(SndBuffer *buffer, void *dst, unsigned dstLen);
#endif
} // End of namespace Clue

#endif


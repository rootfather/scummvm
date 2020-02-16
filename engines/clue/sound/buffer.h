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
typedef struct SndBuffer SND_BUFFER;

SND_BUFFER *sndCreateBuffer(unsigned size);
void sndResetBuffer(SND_BUFFER *buffer);

#if 0
unsigned sndLenBuffer(SND_BUFFER *buffer);
void sndFreeBuffer(SND_BUFFER *buffer);
unsigned sndInsertBuffer(SND_BUFFER *buffer, const void *src, unsigned srcLen);
unsigned sndRemoveBuffer(SND_BUFFER *buffer, void *dst, unsigned dstLen);
#endif
} // End of namespace Clue

#endif


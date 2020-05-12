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

class SndBuffer {
public:
	unsigned char* data;    /* data */
	unsigned size;      /* size of data buffer */

	unsigned insertPos;     /* insert at (insertPos % size) */
	unsigned removePos;     /* remove from (removePos % size) */

	SndBuffer() { data = nullptr; size = insertPos = removePos = 0; }
	void sndResetBuffer();

#if 0
	unsigned sndLenBuffer();
	unsigned sndInsertBuffer(const void* src, unsigned srcLen);
	unsigned sndRemoveBuffer(void* dst, unsigned dstLen);
#endif
};

SndBuffer *sndCreateBuffer(unsigned size);
void sndFreeBuffer(SndBuffer *buffer);

} // End of namespace Clue

#endif


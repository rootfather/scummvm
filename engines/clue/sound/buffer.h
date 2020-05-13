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
	uint8* _data;    /* data */
	uint32 _size;      /* size of data buffer */

	uint32 _insertPos;     /* insert at (_insertPos % _size) */
	uint32 _removePos;     /* remove from (_removePos % _size) */

	SndBuffer(uint32 size);
	~SndBuffer();

#if 0
	unsigned sndLenBuffer();
	unsigned sndInsertBuffer(const void* src, unsigned srcLen);
	unsigned sndRemoveBuffer(void* dst, unsigned dstLen);
#endif
};

} // End of namespace Clue

#endif


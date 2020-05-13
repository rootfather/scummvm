/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#include "clue/sound/buffer.h"
#include "clue/sound/newsound.h"

namespace Clue {
SndBuffer::SndBuffer(uint32 size) : _size(size){
	_insertPos = _removePos = 0;
	_data = new uint8[size];
	memset(_data, 0, size);
}

SndBuffer::~SndBuffer() {
	delete[] _data;
}

#if 0
unsigned SndBuffer::sndLenBuffer() {
	return _insertPos - _removePos;
}

unsigned SndBuffer::sndInsertBuffer(const void *src, unsigned srcLen) {
	const uint8 *psrc = (const uint8 *)src;

	srcLen = MIN(srcLen, _size - sndLenBuffer());

	uint pos = _insertPos % _size;
	uint len = MIN(srcLen, _size - pos);

	/* insert to the end */
	memcpy(_data + pos, psrc, len);

	/* insert to the start */
	memcpy(_data, psrc + len, srcLen - len);

	_insertPos += srcLen;

	return srcLen;
}

unsigned SndBuffer::sndRemoveBuffer(void *dst, uint dstLen) {
	uint8 *pdst = (uint8 *)dst;

	dstLen = MIN(dstLen, sndLenBuffer());

	uint pos = _removePos % _size;
	uint len = MIN(dstLen, _size - pos);

	/* remove from the end */
	memcpy(pdst, _data + pos, len);

	/* remove from the start */
	memcpy(pdst + len, _data, dstLen - len);

	_removePos += dstLen;

	return dstLen;
}
#endif
} // End of namespace Clue


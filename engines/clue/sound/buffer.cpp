/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#include "clue/memory/memory.h"
#include "clue/sound/buffer.h"

namespace Clue {

SndBuffer *sndCreateBuffer(unsigned size) {
	SndBuffer *buffer = new SndBuffer;
	uint8 *data = new uint8[size];

	buffer->data = data;
	buffer->size = size;
	buffer->sndResetBuffer();

	return buffer;
}

void SndBuffer::sndResetBuffer() {
	insertPos = 0;
	removePos = 0;
}

void sndFreeBuffer(SndBuffer* buffer) {
	delete[] buffer->data;
	delete buffer;
}

#if 0
unsigned SndBuffer::sndLenBuffer() {
	return insertPos - removePos;
}

unsigned SndBuffer->sndInsertBuffer(const void *src, unsigned srcLen) {
	const unsigned char *psrc = (const unsigned char *)src;

	srcLen = MIN(srcLen, size - sndLenBuffer());

	uint pos = insertPos % size;
	uint len = MIN(srcLen, size - pos);

	/* insert to the end */
	memcpy(data + pos, psrc, len);

	/* insert to the start */
	memcpy(data, psrc + len, srcLen - len);

	insertPos += srcLen;

	return srcLen;
}

unsigned buffer->sndRemoveBuffer(void *dst, uint dstLen) {
	unsigned char *pdst = (unsigned char *)dst;

	dstLen = MIN(dstLen, sndLenBuffer());

	uint pos = removePos % size;
	uint len = MIN(dstLen, size - pos);

	/* remove from the end */
	memcpy(pdst, data + pos, len);

	/* remove from the start */
	memcpy(pdst + len, data, dstLen - len);

	removePos += dstLen;

	return dstLen;
}
#endif
} // End of namespace Clue


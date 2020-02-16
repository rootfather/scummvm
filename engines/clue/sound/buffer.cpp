/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#include "clue/memory/memory.h"
#include "clue/sound/buffer.h"

namespace Clue {

struct SndBuffer {
	unsigned char *data;    /* data */
	unsigned size;      /* size of data buffer */

	unsigned insertPos;     /* insert at (insertPos % size) */
	unsigned removePos;     /* remove from (removePos % size) */
};


SND_BUFFER *sndCreateBuffer(unsigned size) {
	SND_BUFFER *buffer = (SND_BUFFER *)TCAllocMem(sizeof(*buffer), false);
	unsigned char *data = (unsigned char *)TCAllocMem(size, true);

	buffer->data = data;
	buffer->size = size;

	sndResetBuffer(buffer);

	return buffer;
}

void sndResetBuffer(SND_BUFFER *buffer) {
	buffer->insertPos = 0;
	buffer->removePos = 0;
}

unsigned sndLenBuffer(SND_BUFFER *buffer) {
	return buffer->insertPos - buffer->removePos;
}

#if 0
void sndFreeBuffer(SND_BUFFER *buffer) {
	TCFreeMem(buffer->data, buffer->size);
	TCFreeMem(buffer, sizeof(*buffer));
}

unsigned sndInsertBuffer(SND_BUFFER *buffer, const void *src, unsigned srcLen) {
	const unsigned char *psrc = (const unsigned char *)src;

	srcLen = MIN(srcLen, buffer->size - sndLenBuffer(buffer));

	uint pos = buffer->insertPos % buffer->size;
	uint len = MIN(srcLen, buffer->size - pos);

	/* insert to the end */
	memcpy(buffer->data + pos, psrc, len);

	/* insert to the start */
	memcpy(buffer->data, psrc + len, srcLen - len);

	buffer->insertPos += srcLen;

	return srcLen;
}

unsigned sndRemoveBuffer(SND_BUFFER *buffer, void *dst, uint dstLen) {
	unsigned char *pdst = (unsigned char *)dst;

	dstLen = MIN(dstLen, sndLenBuffer(buffer));

	uint pos = buffer->removePos % buffer->size;
	uint len = MIN(dstLen, buffer->size - pos);

	/* remove from the end */
	memcpy(pdst, buffer->data + pos, len);

	/* remove from the start */
	memcpy(pdst + len, buffer->data, dstLen - len);

	buffer->removePos += dstLen;

	return dstLen;
}
#endif
} // End of namespace Clue


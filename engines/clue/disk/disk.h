/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef TC_DISK_H
#define TC_DISK_H

#include "clue/theclou.h"

namespace Common {
class Stream;
class String;
}

namespace Clue {

#define DSK_PATH_MAX 512
#define ERR_DISK_OPEN_FAILED        1
#define ERR_DISK_WRITE_FAILED       2
#define ERR_DISK_READ_FAILED        3

enum DiskCheckE {
	DISK_CHECK_FILE,
	DISK_CHECK_DIR
};

#if 0
// CHECKME: Remove this when it's confirmed to be unused
extern char *dskGetRootPath(char *result);
void dskSave(const char *Pathname, void *src, size_t size);
#endif

extern void dskSetRootPath(const char *newRootPath);

void *dskLoad(const char *Pathname);
bool dskBuildPathName(DiskCheckE check, const char *Directory, const char *Filename, char *Result);
void dskClose(Common::Stream *fp);
void dskWrite(Common::Stream *fp, void *src, uint32 size);

void dskWrite_U8(Common::Stream *fp, uint8 *x);
void dskWrite_S8(Common::Stream *fp, int8 *x);
void dskWrite_U16LE(Common::Stream *fp, uint16 *x);
void dskWrite_S16LE(Common::Stream *fp, int16 *x);
void dskWrite_U32LE(Common::Stream *fp, uint32 *x);
void dskWrite_S32LE(Common::Stream *fp, int32 *x);

void dskRead(Common::Stream *fp, void *dest, uint32 size);

void dskRead_U8(Common::Stream *fp, uint8 *x);
void dskRead_S8(Common::Stream *fp, int8 *x);
void dskRead_U16LE(Common::Stream *fp, uint16 *x);
void dskRead_S16LE(Common::Stream *fp, int16 *x);
void dskRead_U32LE(Common::Stream *fp, uint32 *x);
void dskRead_S32LE(Common::Stream *fp, int32 *x);


int32 dskFileLength(const char *Pathname);

Common::Stream *dskOpen(const char *Pathname, int RW);

extern bool dskGetLine(char *s, int size, Common::Stream *fp);
extern void dskSetLine(Common::Stream *fp, Common::String s);
extern bool dskGetLine_U16(Common::Stream *fp, uint16 *x);
extern bool dskGetLine_U32(Common::Stream *fp, uint32 *x);
extern void dskSetLine_U16(Common::Stream *fp, uint16 x);
extern void dskSetLine_U32(Common::Stream *fp, uint32 x);

bool dskEOF(Common::Stream *fp);
int32 dskTell(Common::Stream *fp);
void dskPeek(Common::Stream *fp);
void dskSeek(Common::Stream* fp, int32 offset);

} // End of namespace Clue

#endif

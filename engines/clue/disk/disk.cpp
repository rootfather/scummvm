/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include <ctype.h>

#include "clue/error/error.h"

#include "clue/disk/disk.h"
#include "clue/disk/disk_e.h"

// HACK: Replace with proper streams
#include "backends/fs/stdiostream.h"

namespace Clue {

char RootPathName[DSK_PATH_MAX];


void dskSetRootPath(const char *newRootPath) {
	strcpy(RootPathName, newRootPath);
}

#if 0
char *dskGetRootPath(char *result) {
	return strcpy(result, RootPathName);
}
#endif

Common::Stream *dskOpen(const char *Pathname, int RW) {
	Common::Stream *fp = NULL;
	if (RW == 0) {
		DebugMsg(ERR_DEBUG, ERROR_MODULE_DISK, "Reading :%s", Pathname);
		fp = StdioStream::makeFromPath(Pathname, false);
	} else {
		DebugMsg(ERR_DEBUG, ERROR_MODULE_DISK, "Writing :%s", Pathname);
		fp = StdioStream::makeFromPath(Pathname, true);
	}

	if (!fp)
		DebugMsg(ERR_ERROR, ERROR_MODULE_DISK, "Open :%s", Pathname);

	return fp;
}

void *dskLoad(const char *Pathname) {

	Common::Stream *fp = dskOpen(Pathname, 0);
	Common::ReadStream *stream = dynamic_cast<Common::ReadStream *>(fp);
	if (fp) {
		size_t nread;
		size_t pos = 0;
		size_t size = BUFSIZ;
		uint8 *ptr = (uint8 *)malloc(size);
		while ((nread = stream->read(ptr + pos, BUFSIZ)) == BUFSIZ) {
			pos  += nread;
			ptr   = (uint8 *)realloc(ptr, size + BUFSIZ);
			size += nread;
		}

		pos += nread;

		ptr = (uint8 *)realloc(ptr, pos);
		dskClose(fp);
		return (void *)ptr;
	}
	return NULL;
}

#if 0
void dskSave(const char *Pathname, void *src, size_t size) {
	Common::Stream *fp = dskOpen(Pathname, 1);
	if (fp) {
		dskWrite(fp, src, size);
		dskClose(fp);
	}
}
#endif

static void strUpper(char *s) {
	while (*s != '\0') {
		*s = toupper(*s);
		s++;
	}
}

static void strLower(char *s) {
	while (*s != '\0') {
		*s = tolower(*s);
		s++;
	}
}

bool dskBuildPathName(DiskCheckE check, const char *Directory, const char *Filename, char *Result) {
	int step = 0;

	char Dir [DSK_PATH_MAX];
	char File[DSK_PATH_MAX];
	struct stat status;

	do {
		switch (step++) {
		case 0:
			strcpy(Dir, Directory);
			strcpy(File, Filename);
			break;

		case 1:
			strUpper(Dir);
			strUpper(File);
			break;

		case 2:
			strLower(Dir);
			strLower(File);
			break;

		case 3:
			sprintf(Result, "%s" DIR_SEP "%s" DIR_SEP "%s",
			        RootPathName, Directory, Filename);

			DebugMsg(ERR_DEBUG, ERROR_MODULE_DISK,
			         "Path failure: %s", Result);
			return false;
		}

		if (check == DISK_CHECK_FILE) {
			sprintf(Result, "%s" DIR_SEP "%s" DIR_SEP "%s",
			        RootPathName, Dir, File);
		} else {
			sprintf(Result, "%s" DIR_SEP "%s", RootPathName, Dir);
		}

	} while (stat(Result, &status) == -1);

	if (check == DISK_CHECK_DIR) {
		strcat(Result, DIR_SEP);
		strcat(Result, File);
	}

	return true;
}

size_t dskFileLength(const char *Pathname) {
	struct stat status;

	if (stat(Pathname, &status) == -1)
		return 0;

	return status.st_size;
}

void dskClose(Common::Stream *fp) {
	if (fp) {
		delete fp;
	}
}

void dskWrite(Common::Stream *fp, void *src, uint32 size) {
	Common::WriteStream *stream = dynamic_cast<Common::WriteStream *>(fp);
	stream->write(src, size);
	if (stream->err()) {
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_WRITE_FAILED);
	}
}

void dskWrite_U8(Common::Stream *fp, uint8 *x) {
	Common::WriteStream *stream = dynamic_cast<Common::WriteStream*>(fp);
	stream->writeByte(*x);
	if (stream->err()) {
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_WRITE_FAILED);
	}
}

void dskWrite_S8(Common::Stream *fp, int8 *x) {
	Common::WriteStream *stream = dynamic_cast<Common::WriteStream *>(fp);
	stream->writeSByte(*x);
	if (stream->err()) {
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_WRITE_FAILED);
	}
}

void dskWrite_U16LE(Common::Stream *fp, uint16 *x) {
	Common::WriteStream *stream = dynamic_cast<Common::WriteStream *>(fp);
	stream->writeUint16LE(*x);
	if (stream->err()) {
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_WRITE_FAILED);
	}
}

void dskWrite_S16LE(Common::Stream *fp, int16 *x) {
	Common::WriteStream *stream = dynamic_cast<Common::WriteStream *>(fp);
	stream->writeSint16LE(*x);
	if (stream->err()) {
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_WRITE_FAILED);
	}
}

void dskWrite_U32LE(Common::Stream *fp, uint32 *x) {
	Common::WriteStream *stream = dynamic_cast<Common::WriteStream *>(fp);
	stream->writeUint32LE(*x);
	if (stream->err()) {
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_WRITE_FAILED);
	}
}

void dskWrite_S32LE(Common::Stream *fp, int32 *x) {
	Common::WriteStream *stream = dynamic_cast<Common::WriteStream *>(fp);
	stream->writeSint32LE(*x);
	if (stream->err()) {
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_WRITE_FAILED);
	}
}

void dskRead(Common::Stream *fp, void *dest, size_t size) {
	Common::ReadStream *stream = dynamic_cast<Common::ReadStream *>(fp);
	stream->read(dest, size);
	if (stream->err()) {
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_READ_FAILED);
	}
}

void dskRead_U8(Common::Stream *fp, uint8 *x) {
	Common::ReadStream *stream = dynamic_cast<Common::ReadStream *>(fp);
	*x = stream->readByte();
	if (stream->err()) {
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_READ_FAILED);
	}
}

void dskRead_S8(Common::Stream *fp, int8 *x) {
	Common::ReadStream *stream = dynamic_cast<Common::ReadStream *>(fp);
	*x = stream->readSByte();
	if (stream->err()) {
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_READ_FAILED);
	}
}

void dskRead_U16LE(Common::Stream *fp, uint16 *x) {
	Common::ReadStream *stream = dynamic_cast<Common::ReadStream *>(fp);
	*x = stream->readUint16LE();
	if (stream->err()) {
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_READ_FAILED);
	}
}

void dskRead_S16LE(Common::Stream *fp, int16 *x) {
	Common::ReadStream *stream = dynamic_cast<Common::ReadStream *>(fp);
	*x = stream->readSint16LE();
	if (stream->err()) {
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_READ_FAILED);
	}
}

void dskRead_U32LE(Common::Stream *fp, uint32 *x) {
	Common::ReadStream *stream = dynamic_cast<Common::ReadStream *>(fp);
	*x = stream->readUint32LE();
	if (stream->err()) {
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_READ_FAILED);
	}
}

void dskRead_S32LE(Common::Stream *fp, int32 *x) {
	Common::ReadStream *stream = dynamic_cast<Common::ReadStream *>(fp);
	*x = stream->readSint32LE();
	if (stream->err()) {
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_READ_FAILED);
	}
}

bool dskGetLine(char *s, int size, Common::Stream *fp) {
	Common::SeekableReadStream *stream = dynamic_cast<Common::SeekableReadStream *>(fp);
	if (stream->readLine(s, size)) {
		char *p;
		if (p = strrchr(s, '\n'))
			*p = '\0';

		if (p = strrchr(s, '\r'))
			*p = '\0';

		return true;
	}
	return false;
}

void dskSetLine(Common::Stream *fp, const char *s) {
	Common::WriteStream *stream = dynamic_cast<Common::WriteStream *>(fp);
	stream->writeString(s);
	stream->writeString("\r\n");
}

bool dskGetLine_U16(Common::Stream *fp, uint16 *x) {
	Common::SeekableReadStream *stream = dynamic_cast<Common::SeekableReadStream *>(fp);
	Common::String line = stream->readLine();
	if (stream->err() || stream->eos()) {
		return false;
	}
	*x = atol(line.c_str());
	return true;
}

bool dskGetLine_U32(Common::Stream *fp, uint32 *x) {
	Common::SeekableReadStream *stream = dynamic_cast<Common::SeekableReadStream *>(fp);
	Common::String line = stream->readLine();
	if (stream->err() || stream->eos()) {
		return false;
	}
	*x = atol(line.c_str());
	return true;
}

void dskSetLine_U16(Common::Stream *fp, uint16 x) {
	char buf[BUFSIZ];
	sprintf(buf, "%hu", x);
	dskSetLine(fp, buf);
}

void dskSetLine_U32(Common::Stream *fp, uint32 x) {
	char buf[BUFSIZ];
	sprintf(buf, "%u", x);
	dskSetLine(fp, buf);
}

bool dskEOF(Common::Stream *fp) {
	Common::ReadStream *stream = dynamic_cast<Common::ReadStream *>(fp);
	return stream->eos();
}

int32 dskTell(Common::Stream *fp) {
	Common::SeekableReadStream *stream = dynamic_cast<Common::SeekableReadStream *>(fp);
	return stream->pos();
}

void dskPeek(Common::Stream *fp) {
	Common::SeekableReadStream *stream = dynamic_cast<Common::SeekableReadStream *>(fp);
	stream->readByte();
	if (!stream->eos()) {
		stream->seek(-1, SEEK_CUR);
	}
}

} // End of namespace Clue

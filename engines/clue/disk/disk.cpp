/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "common/fs.h"
#include "common/stream.h"
#include "common/bufferedstream.h"
#include "clue/error/error.h"

namespace Clue {

#define DIR_SEP "/"

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
	// HACK: Replace with proper streams
	Common::FSNode node(Pathname);
	Common::Stream *fp = NULL;
	if (RW == 0) {
		DebugMsg(ERR_DEBUG, ERROR_MODULE_DISK, "Reading :%s", Pathname);
		fp = dynamic_cast<Common::Stream*>(node.createReadStream());
	} else {
		DebugMsg(ERR_DEBUG, ERROR_MODULE_DISK, "Writing :%s", Pathname);
		fp = dynamic_cast<Common::Stream*>(node.createWriteStream());
	}

	if (!fp)
		DebugMsg(ERR_ERROR, ERROR_MODULE_DISK, "Open :%s", Pathname);

	return fp;
}

void *dskLoad(const char *Pathname) {

	Common::SeekableReadStream *fp = dynamic_cast<Common::SeekableReadStream*>(dskOpen(Pathname, 0));
	Common::SeekableReadStream *stream = Common::wrapBufferedSeekableReadStream(fp, BUFSIZ, DisposeAfterUse::YES);
	if (stream) {
		void *ptr = malloc(stream->size());
		stream->read(ptr, stream->size());
		dskClose(stream);
		return ptr;
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

bool dskBuildPathName(DiskCheckE check, const char *Directory, const char *Filename, char *Result) {
	int step = 0;

	Common::String Dir;
	Common::String File;

	do {
		switch (step++) {
		case 0:
			Dir = Common::String(Directory);
			File = Common::String(Filename);
			break;

		case 1:
			Dir.toUppercase();
			File.toUppercase();
			break;

		case 2:
			Dir.toLowercase();
			File.toLowercase();
			break;

		case 3:
			sprintf(Result, "%s" DIR_SEP "%s" DIR_SEP "%s", RootPathName, Directory, Filename);
			DebugMsg(ERR_DEBUG, ERROR_MODULE_DISK, "Path failure: %s", Result);
			return false;
		}

		if (check == DISK_CHECK_FILE) {
			sprintf(Result, "%s" DIR_SEP "%s" DIR_SEP "%s", RootPathName, Dir.c_str(), File.c_str());
		} else {
			sprintf(Result, "%s" DIR_SEP "%s", RootPathName, Dir.c_str());
		}

	} while (!Common::FSNode(Result).exists());

	if (check == DISK_CHECK_DIR) {
		strcat(Result, DIR_SEP);
		strcat(Result, File.c_str());
	}

	return true;
}

int32 dskFileLength(const char *Pathname) {
	int32 length = 0;
	Common::SeekableReadStream *fp = dynamic_cast<Common::SeekableReadStream*>(dskOpen(Pathname, 0));
	if (fp && fp->size() > 0) {
		length = fp->size();
	}
	dskClose(fp);
	return length;
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

void dskRead(Common::Stream *fp, void *dest, uint32 size) {
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
		char *p = strrchr(s, '\n');
		if (p)
			*p = '\0';

		p = strrchr(s, '\r');
		if (p)
			*p = '\0';

		return true;
	}
	return false;
}

void dskSetLine(Common::Stream *fp, Common::String s) {
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
	Common::String line = Common::String::format("%hu", x);
	dskSetLine(fp, line);
}

void dskSetLine_U32(Common::Stream *fp, uint32 x) {
	Common::String line = Common::String::format("%u", x);
	dskSetLine(fp, line);
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

void dskSeek(Common::Stream* fp, int32 offset) {
	Common::SeekableReadStream* stream = dynamic_cast<Common::SeekableReadStream*>(fp);
	stream->seek(offset);
}

} // End of namespace Clue

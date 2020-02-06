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

FILE *dskOpen(const char *Pathname, const char *Mode) {
	DebugMsg(ERR_DEBUG, ERROR_MODULE_DISK, "Opening :%s (%s)", Pathname, Mode);

	FILE *fp = fopen(Pathname, Mode);
	if (!fp)
		DebugMsg(ERR_ERROR, ERROR_MODULE_DISK, "Open :%s", Pathname);

	return fp;
}

void *dskLoad(const char *Pathname) {

	FILE *fp = dskOpen(Pathname, "rb");
	if (fp) {
		size_t nread;
		size_t pos = 0;
		size_t size = BUFSIZ;
		uint8 *ptr = (uint8 *)malloc(size);

		while ((nread = fread(ptr + pos, 1, BUFSIZ, fp)) == BUFSIZ) {
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
	FILE *fp = dskOpen(Pathname, "wb");
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

void dskClose(FILE *fp) {
	if (fp) {
		fclose(fp);
	}
}

void dskWrite(FILE *fp, void *src, size_t size) {
	if (fwrite(src, 1, size, fp) != size) {
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_WRITE_FAILED);
	}
}

void dskWrite_U8(FILE *fp, uint8 *x) {
	uint8 tmp = *x;
	dskWrite(fp, &tmp, sizeof(tmp));
}

void dskWrite_S8(FILE *fp, int8 *x) {
	int8 tmp = *x;
	dskWrite(fp, &tmp, sizeof(tmp));
}

void dskWrite_U16LE(FILE *fp, uint16 *x) {
	uint8 tmp[2];

	tmp[0] = (uint8)((*x) & 0xff);
	tmp[1] = (uint8)((*x >> 8) & 0xff);
	dskWrite(fp, &tmp, sizeof(tmp));
}

void dskWrite_S16LE(FILE *fp, int16 *x) {
	uint8 tmp[2];

	tmp[0] = (uint8)((*x) & 0xff);
	tmp[1] = (uint8)((*x >> 8) & 0xff);
	dskWrite(fp, &tmp, sizeof(tmp));
}

void dskWrite_U32LE(FILE *fp, uint32 *x) {
	uint8 tmp[4];

	tmp[0] = (uint8)((*x) & 0xff);
	tmp[1] = (uint8)((*x >> 8) & 0xff);
	tmp[2] = (uint8)((*x >> 16) & 0xff);
	tmp[3] = (uint8)((*x >> 24) & 0xff);
	dskWrite(fp, &tmp, sizeof(tmp));
}

void dskWrite_S32LE(FILE *fp, int32 *x) {
	uint8 tmp[4];

	tmp[0] = (uint8)((*x) & 0xff);
	tmp[1] = (uint8)((*x >> 8) & 0xff);
	tmp[2] = (uint8)((*x >> 16) & 0xff);
	tmp[3] = (uint8)((*x >> 24) & 0xff);
	dskWrite(fp, &tmp, sizeof(tmp));
}

void dskRead(FILE *fp, void *dest, size_t size) {
	if (fread(dest, 1, size, fp) != size) {
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_READ_FAILED);
	}
}

void dskRead_U8(FILE *fp, uint8 *x) {
	uint8 tmp;

	dskRead(fp, &tmp, sizeof(tmp));
	*x = tmp;
}

void dskRead_S8(FILE *fp, int8 *x) {
	int8 tmp;

	dskRead(fp, &tmp, sizeof(tmp));
	*x = tmp;
}

void dskRead_U16LE(FILE *fp, uint16 *x) {
	uint8 tmp[2];

	dskRead(fp, &tmp, sizeof(tmp));
	*x = (uint16)((uint16) tmp[0] | ((uint16) tmp[1] << 8));
}

void dskRead_S16LE(FILE *fp, int16 *x) {
	uint8 tmp[2];

	dskRead(fp, &tmp, sizeof(tmp));
	*x = (int16)((uint16) tmp[0] | ((uint16) tmp[1] << 8));
}

void dskRead_U32LE(FILE *fp, uint32 *x) {
	uint8 tmp[4];

	dskRead(fp, &tmp, sizeof(tmp));
	*x = (uint32)((uint32) tmp[0] | ((uint32) tmp[1] << 8)
	              | ((uint32) tmp[2] << 16) | ((uint32) tmp[3] << 24));
}

void dskRead_S32LE(FILE *fp, int32 *x) {
	uint8 tmp[4];

	dskRead(fp, &tmp, sizeof(tmp));
	*x = (int32)((uint32) tmp[0] | ((uint32) tmp[1] << 8)
	             | ((uint32) tmp[2] << 16) | ((uint32) tmp[3] << 24));
}

bool dskGetLine(char *s, int size, FILE *fp) {
	if (fgets(s, size, fp)) {
		char *p;
		if (p = strrchr(s, '\n'))
			*p = '\0';

		if (p = strrchr(s, '\r'))
			*p = '\0';

		return true;
	}
	return false;
}

} // End of namespace Clue

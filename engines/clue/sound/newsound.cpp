/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#include "clue/disk/disk.h"
#include "clue/sound/fx.h"
#include "clue/sound/newsound.h"
#include "clue/sound/hsc.h"

#include "common/textconsole.h"

namespace Clue {
char currSoundName[DSK_PATH_MAX];

void sndInit(void) {
	currSoundName[0] = '\0';

	hscInit();
}

void sndDone(void) {
	hscDone();
}

void sndPlaySound(const char *name, uint32 mode) {
	if (strcmp(currSoundName, name) != 0) {
		strcpy(currSoundName, name);

		if (FXBase.us_AudioOk) {
			char path[DSK_PATH_MAX];
			dskBuildPathName(DISK_CHECK_FILE, SOUND_DIRECTORY, name, path);
			hscLoad(path);
		}
	}
}

char *sndGetCurrSoundName(void) {
	return currSoundName;
}

void sndFading(short int targetVol) {
	if (FXBase.us_AudioOk) {
	}

	warning("STUB: sndFading");
}

void sndStopSound(uint8 dummy) {
	if (FXBase.us_AudioOk)
		hscReset();
}

} // End of namespace Clue

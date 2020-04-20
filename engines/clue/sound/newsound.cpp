/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#include "clue/clue.h"
#include "clue/disk/disk.h"
#include "clue/sound/fx.h"
#include "clue/sound/hsc.h"

#include "common/textconsole.h"
#include "common/config-manager.h"
#include "audio/mixer.h"

namespace Clue {
static bool fading = false;

char currSoundName[DSK_PATH_MAX];

void sndInit() {
	currSoundName[0] = '\0';

	hscInit();
}

void sndDone() {
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

char *sndGetCurrSoundName() {
	return currSoundName;
}

// 2014-07-17 LucyG : called from inpDoPseudoMultiTasking
void sndDoFading()
{
	// 2018-09-25: volume can now be changed any time
	int targetMusicVolume;
	if (fading) {
		targetMusicVolume = ConfMan.getInt("music_volume") / 4;
	}
	else {
		targetMusicVolume = ConfMan.getInt("music_volume");
	}

	int currMusicVolume = g_clue->_mixer->getVolumeForSoundType(Audio::Mixer::kMusicSoundType);

	if (currMusicVolume < targetMusicVolume) {
		currMusicVolume++;
	}
	else if (currMusicVolume > targetMusicVolume) {
		currMusicVolume--;
	}

	g_clue->_mixer->setVolumeForSoundType(Audio::Mixer::kMusicSoundType, currMusicVolume);
}

void sndFading(uint16 targetVol)
{
	if (FXBase.us_AudioOk) {
		/* 2014-07-17 LucyG : this is called from dialog.c (and intro.c)
		   with targetVol = 16 before and 0 after voice playback */
		if (!targetVol) {
			fading = false;
		}
		else {
			fading = true;
		}
	}
}

void sndStopSound(uint8 dummy) {
	if (FXBase.us_AudioOk)
		hscReset();
}

} // End of namespace Clue

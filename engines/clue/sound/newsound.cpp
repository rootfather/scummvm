/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#include "clue/clue.h"
#include "clue/disk/disk.h"
#include "clue/sound/hsc.h"
#include "clue/sound/newsound.h"

#include "common/config-manager.h"
#include "audio/mixer.h"

namespace Clue {
SndManager::SndManager(ClueEngine *vm) : _vm(vm) {
	_audioOk = false;

	_sfxFile = nullptr;
	_sfxBuffer = nullptr;
	_musicBuffer = nullptr;

	_currSoundName[0] = '\0';

	_fading = false;

	_sfxChannelOn = false;
	_musicChannelOn = true;

	hscInit();
}

void SndManager::sndDone() {
	hscDone();
}

void SndManager::sndPlaySound(const char *name, uint32 mode) {
	if (strcmp(_currSoundName, name) != 0) {
		strcpy(_currSoundName, name);

		if (_audioOk) {
			char path[DSK_PATH_MAX];
			dskBuildPathName(DISK_CHECK_FILE, SOUND_DIRECTORY, name, path);
			hscLoad(path);
		}
	}
}

char *SndManager::sndGetCurrSoundName() {
	return _currSoundName;
}

// 2014-07-17 LucyG : called from inpDoPseudoMultiTasking
void SndManager::sndDoFading() {
	// 2018-09-25: volume can now be changed any time
	int targetMusicVolume;
	if (_fading)
		targetMusicVolume = ConfMan.getInt("music_volume") / 4;
	else
		targetMusicVolume = ConfMan.getInt("music_volume");

	int currMusicVolume = _vm->_mixer->getVolumeForSoundType(Audio::Mixer::kMusicSoundType);

	if (currMusicVolume < targetMusicVolume)
		currMusicVolume++;
	else if (currMusicVolume > targetMusicVolume)
		currMusicVolume--;

	g_clue->_mixer->setVolumeForSoundType(Audio::Mixer::kMusicSoundType, currMusicVolume);
}

void SndManager::sndFading(uint16 targetVol) {
	if (_audioOk) {
		/* 2014-07-17 LucyG : this is called from dialog.c (and intro.c)
		   with targetVol = 16 before and 0 after voice playback */
		_fading = (targetVol != 0);
	}
}

void SndManager::sndStopSound(uint8 dummy) {
	if (_audioOk)
		hscReset();
}

} // End of namespace Clue

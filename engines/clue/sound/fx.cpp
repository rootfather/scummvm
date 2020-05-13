/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa
  Portions copyright (c) 2005 Jens Granseuer

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#include "common/stream.h"
#include "audio/mixer.h"
#include "audio/audiostream.h"
#include "audio/decoders/raw.h"
#include "audio/decoders/voc.h"

#include "clue/base/base.h"
#include "clue/clue.h"
#include "clue/sound/fx.h"
#include "clue/sound/newsound.h"

namespace Clue {

void SndManager::initAudio() {
	_audioOk = true;
	if (!_vm->_mixer->isReady()) {
		DebugMsg(ERR_WARNING, ERROR_MODULE_SOUND, "Mixer error");
		_audioOk = false;
		return;
	}

	_sfxBuffer = new SndBuffer(SND_BUFFER_SIZE);
	_musicBuffer = new SndBuffer(SND_BUFFER_SIZE);
}

void SndManager::removeAudio() {
	_vm->_mixer->stopAll();
	_audioOk = false;

	delete _sfxBuffer;
	delete _musicBuffer;

	_sfxBuffer = _musicBuffer = nullptr;
}

void SndManager::sndInitFX() {
	_sfxChannelOn = false;
	_vm->_mixer->stopHandle(_sfx);
	_sfxFile = nullptr;
}

void SndManager::sndDoneFX() {
	_sfxChannelOn = false;
	_vm->_mixer->stopHandle(_sfx);
	_sfxFile = nullptr;
}

void SndManager::sndPrepareFX(const char *name) {
	sndDoneFX();

	if (_audioOk) {
		// TODO: Use proper Dsk functions
		char fileName[DSK_PATH_MAX];

		dskBuildPathName(DISK_CHECK_FILE, SAMPLES_DIRECTORY, name, fileName);
		Common::Stream *file = dskOpen(fileName, 0);
		Common::SeekableReadStream *stream = dynamic_cast<Common::SeekableReadStream *>(file);
		_sfxFile = Audio::makeVOCStream(stream, Audio::FLAG_UNSIGNED, DisposeAfterUse::YES);
	}
}

void SndManager::sndPlayFX() {
	_sfxChannelOn = true;
	if (_sfxFile)
		_vm->_mixer->playStream(Audio::Mixer::kSFXSoundType, &_sfx, _sfxFile);
}

} // End of namespace Clue

/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#ifndef TC_NEWSOUND_H
#define TC_NEWSOUND_H

#include "audio/audiostream.h"
#include "audio/mixer.h"

#include "clue/disk/disk.h"

namespace Clue {

class ClueEngine;
class SndBuffer;
	
class SndManager {
private:
	ClueEngine *_vm;

	uint16 _audioOk;

	SndBuffer *_sfxBuffer;   // CHECKME: Unused
	SndBuffer *_musicBuffer; // CHECKME: Unused

	Audio::SoundHandle _sfx;
	Audio::AudioStream *_sfxFile;

	char _currSoundName[DSK_PATH_MAX];
	bool _fading;

	bool _sfxChannelOn;
	bool _musicChannelOn;

public:
	void initAudio();
	void removeAudio();

	void sndPrepareFX(const char *name);
	void sndInitFX();
	void sndPlayFX();
	void sndDoneFX();
	
	void sndDone();

	void sndPlaySound(const char *name, uint32 mode);
	void sndStopSound(uint8 dummy);
	void sndDoFading();
	void sndFading(uint16 targetVol);

	char *sndGetCurrSoundName();

	SndManager(ClueEngine *vm);
};

} // End of namespace Clue

#endif


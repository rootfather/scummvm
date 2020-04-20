/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#include "clue/cdrom/cdrom.h"
#include "backends/audiocd/audiocd.h"
#include "clue/clue.h"

namespace Clue {

unsigned int CDRomInstalled = 0;

int CDROM_Install() {
	return g_system->getAudioCDManager()->open() ? 1 : 0;
}

void CDROM_UnInstall() {
	g_system->getAudioCDManager()->close();
}

void CDROM_WaitForMedia() {
	// No wait required
}

void CDROM_PlayAudioTrack(uint8 TrackNum) {
	// TODO: Does duration 0 play the whole thing?
	g_system->getAudioCDManager()->play(TrackNum - 1, 1, 0, 0, false, Audio::Mixer::kSpeechSoundType);

}

void CDROM_PlayAudioSequence(uint8 TrackNum, uint32 StartOffset, uint32 EndOffset) {
	// Assumption that extracted audio tracks are named Track1 -> 23. The original starts counting at 2.
	g_system->getAudioCDManager()->play(TrackNum - 1, 1, StartOffset, EndOffset - StartOffset, false, Audio::Mixer::kSpeechSoundType);

}

void CDROM_StopAudioTrack() {
	g_system->getAudioCDManager()->stop();
}

} // End of namespace Clue

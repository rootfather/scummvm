/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#ifndef MODULE_CDROM
#define MODULE_CDROM

#include "clue/theclou.h"

namespace Clue {

extern unsigned int CDRomInstalled;

int CDROM_Install();
void CDROM_UnInstall();

void CDROM_WaitForMedia();

void CDROM_PlayAudioSequence(uint8 TrackNum, uint32 StartOffset, uint32 EndOffset);
void CDROM_StopAudioTrack();

} // End of namespace Clue

#endif

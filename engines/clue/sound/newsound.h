/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#ifndef TC_NEWSOUND_H
#define TC_NEWSOUND_H

namespace Clue {

void sndInit();
void sndDone();

void sndPlaySound(const char *name, uint32 mode);
void sndStopSound(uint8 dummy);
void sndFading(short int targetVol);

char *sndGetCurrSoundName();

} // End of namespace Clue

#endif


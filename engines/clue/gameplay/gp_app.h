/*
**  $Filename: gameplay/gp_app.h
**  $Release:  0
**  $Revision: 0.1
**  $Date:     08-04-94
**
**
**
**   (c) 1994 ...and avoid panic by, H. Gaberschek
**      All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_GAMEPLAY_APP
#define MODULE_GAMEPLAY_APP

#include "clue/gameplay/gp.h"

namespace Clue {

class Scene;

extern void TCGamePause(byte activ);

extern bool tcPersonIsHere();

extern void tcPlaySound();
extern void tcPlayStreetSound();
extern void tcPersonGreetsMatt();

extern uint32 StdHandle(uint32 choice);

void PrintStatus(Common::String text);
extern void InitTaxiLocations();
extern void LinkScenes();

extern void tcGetLastName(const char *Name, char *dest, uint32 maxLength);
extern Common::String tcCutName(Common::String Name, byte Sign, uint32 maxLength);

extern bool tcLoadTheClou();
extern void tcSaveTheClou();
extern uint32 tcBurglary(uint32 buildingID);

extern bool tcLoadChangesInScenes(const char *fileName);
extern bool tcSaveChangesInScenes(const char *fileName);

extern void tcRefreshLocationInTitle(uint32 locNr);
extern void ShowTime(uint32 delay);
extern void RefreshAfterDiskRequester();

extern bool tcLoadIt(char activ);
extern void tcRefreshAfterLoad(bool loaded);

extern void ShowMenuBackground();

} // End of namespace Clue

#endif

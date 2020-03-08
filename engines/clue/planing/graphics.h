/*
**      $Filename: planing/graphics.h
**      $Release:  1
**      $Revision: 0
**      $Date:     24-04-94
**
**      planing.graphics interface for "Der Clou!"
**
** (c) 1994 ...and avoid panic by, Kaweh Kazemi
**      All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_PLANING_GRAPHICS
#define MODULE_PLANING_GRAPHICS

namespace Clue {

/* Messages modes */
#define PLANING_MSG_REFRESH   1
#define PLANING_MSG_WAIT      2

/* RefreshRP */
#define PLANING_REFRESH_RP_WIDTH    200
#define PLANING_REFRESH_RP_HEIGHT   50
#define PLANING_REFRESH_RP_DEPTH    2

void plPrintInfo(const char *person);
void plMessage(const char *msg, byte flags);
void plPersonPict(uint32 personId);
void plDisplayAbilities();
void plDisplayTimer(uint32 time, byte doSpotsImmediatly);
void plDisplayInfo();
void plRefresh(uint32 ItemId);
void plDrawWait(uint32 sec);
byte plSay(const char *msg, uint32 persId);

} // End of namespace Clue

#endif

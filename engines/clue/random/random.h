/*
**  $Filename: random/random.h
**  $Release:  0
**  $Revision: 0.1
**  $Date:     28-03-94
**
**  random functions for "Der Clou!"
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

#ifndef MODULE_RANDOM
#define MODULE_RANDOM


namespace Clue {

extern void rndInit();
extern void rndDone();

extern uint32 CalcRandomNr(uint32 l_limit, uint32 u_limit);

} // End of namespace Clue

#endif

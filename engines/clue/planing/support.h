/*
**      $Filename: planing/support.h
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.support interface for "Der Clou!"
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

#ifndef MODULE_PLANING_SUPPORT
#define MODULE_PLANING_SUPPORT

namespace Clue {

/* Area defines */
#define PLANING_AREA_PERSON  3
#define PLANING_AREA_CAR     16

/* functions for bits */
#define BIT(x)               (1<<(x))
#define CHECK_STATE(v,b)     ((v) & BIT(b))

/* Alarms & power control */
#define PLANING_ALARM_Z3          3
#define PLANING_ALARM_X5          8
#define PLANING_ALARM_TOP3        15
#define PLANING_POWER             1

/* all times in 1/3 sec */
#define PLANING_CORRECT_TIME      3


uint32 plGetNextLoot();

bool plLivingsPosAtCar(uint32 bldId);
bool plAllInCar(uint32 bldId);
byte plIsStair(uint32 objId);

void plCorrectOpened(LSObject obj, byte open);
byte plIgnoreLock(uint32 objId);

void plMove(uint32 current, byte direction);
void plWork(uint32 current);

List *plGetObjectsList(uint32 current, byte addLootBags);
void plInsertGuard(List *list, uint32 current, uint32 guard);
bool plObjectInReach(uint32 current, uint32 objId);

} // End of namespace Clue

#endif

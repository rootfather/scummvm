/*
**  $Filename: scenes/scenes.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-04-94
**
**  some scenes for "Der Clou!"
**
**   (c) 1994 ...and avoid panic by, K. Kazemi, H. Gaberschek
**      All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_SCENES
#define MODULE_SCENES

#ifndef MODULE_LIST
#include "clue/list/list.h"
#endif

#ifndef MODULE_TEXT
#include "clue/text/text.h"
#endif

#ifndef MODULE_INTERAC
#include "clue/present/interac.h"
#endif

#ifndef MODULE_PRESENT
#include "clue/present/present.h"
#endif

#ifndef MODULE_DIALOG
#include "clue/dialog/dialog.h"
#endif

#ifndef MODULE_RELATION
#include "clue/data/relation.h"
#endif

#ifndef MODULE_DATABASE
#include "clue/data/database.h"
#endif

#ifndef MODULE_DATACALC
#include "clue/data/datacalc.h"
#endif

#ifndef MODULE_DATAAPPL
#include "clue/data/dataappl.h"
#endif

#include "clue/data/objstd/tcdata.h"

#ifndef MODULE_GAMEPLAY_APP
#include "clue/gameplay/gp_app.h"
#endif

#ifndef MODULE_RASTER
#include "clue/landscap/raster.h"
#endif

namespace Clue {

extern uint16 CurrentBackground;

/* standard scene functions - look at scenes.c */

extern uint32 Go(LIST *succ);
extern uint32 tcTelefon(void);

extern void Look(uint32 locNr);
extern void Information(void);
extern void tcWait(void);

/* taxi functions */

extern void AddTaxiLocation(uint32 locNr);
extern void RemTaxiLocation(uint32 locNr);

/* Done Funktionen */

void DoneHotelRoom(void);
void DoneTaxi(void);
void DoneGarage(void);
void DoneParking(void);
void DoneTools(void);
void DoneDealer(void);
void DoneInsideHouse(void);

void tcInitFahndung(void);
void tcDoneFahndung(void);

/* Hilfsfunktionen */

void Investigate(const char *location);

void SetCarColors(byte index);

void tcSellCar(uint32 ObjectID);
void tcColorCar(Car car);
void tcRepairCar(Car car, const char *repairWhat);
void tcCarGeneralOverhoul(Car car);
void tcToolsShop(void);

int32 tcEscapeFromBuilding(uint32 escBits);
int32 tcEscapeByCar(uint32 escBits, int32 timeLeft);

uint32 tcChooseCar(uint32 backgroundNr);

LIST *tcMakeLootList(uint32 containerID, uint32 relID);

char *tcShowPriceOfCar(uint32 nr, uint32 type, void *data);
char *tcShowPriceOfTool(uint32 nr, uint32 type, void *data);

void tcDealerSays(Person dealer, byte textNr, int32 perc);
void tcDealerOffer(Person dealer, byte which);

void tcBuyCar(void);
void tcCarInGarage(uint32 carID);

byte tcBuyTool(byte choice);
void tcSellTool(void);
byte tcShowTool(byte choice);
byte tcDescTool(byte choice);
void tcDealerDlg(void);

uint32 tcStartEvidence(void);
uint32 tcGoInsideOfHouse(uint32 buildingID);

void tcShowLootInfo(Loot loot);
void tcInsideOfHouse(uint32 buildingID, uint32 areaID, byte perc);
void tcShowObjectData(uint32 areaID, NODE *node, byte perc);

} // End of namespace Clue

#endif

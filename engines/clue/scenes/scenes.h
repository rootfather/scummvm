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

#include "clue/text.h"
#include "clue/present/present.h"
#include "clue/gameplay/gp.h"
#include "clue/gameplay/gp_app.h"
#include "clue/landscap/raster.h"

namespace Clue {

extern uint16 CurrentBackground;

/* standard scene functions - look at scenes.c */	
extern uint32 Go(NewList<NewTCEventNode> *succ);
extern uint32 tcTelefon();

extern void Look(uint32 locNr);
extern void Information();
extern void tcWait();

/* taxi functions */

extern void AddTaxiLocation(uint32 locNr);
extern void RemTaxiLocation(uint32 locNr);

/* Done Funktionen */

void DoneHotelRoom();
void DoneTaxi();
void DoneGarage();
void DoneParking();
void DoneTools();
void DoneDealer();
void DoneInsideHouse();

void tcInitFahndung();
void tcDoneFahndung();

/* Hilfsfunktionen */

void Investigate(const char *location);

void SetCarColors(byte index);

void tcSellCar(uint32 ObjectID);
void tcColorCar(CarNode *car);
void tcRepairCar(CarNode *car, const char *repairWhat);
void tcCarGeneralOverhoul(CarNode *car);
void tcToolsShop();

int32 tcEscapeFromBuilding(uint32 escBits);
int32 tcEscapeByCar(uint32 escBits, int32 timeLeft);

uint32 tcChooseCar(uint32 backgroundNr);

NewList<NewNode>* tcMakeLootList(uint32 containerID, uint32 relID);

Common::String tcShowPriceOfCar(uint32 nr, uint32 type, void *data);
Common::String tcShowPriceOfTool(uint32 nr, uint32 type, void *data);

void tcDealerSays(PersonNode *dealer, byte textNr, int32 perc);
void tcDealerOffer(PersonNode *dealer, byte which);

void tcBuyCar();
void tcCarInGarage(uint32 carID);

byte tcBuyTool(byte choice);
void tcSellTool();
byte tcShowTool(byte choice);
byte tcDescTool(byte choice);
void tcDealerDlg();

uint32 tcStartEvidence();
uint32 tcGoInsideOfHouse(uint32 buildingID);

void tcInsideOfHouse(uint32 buildingID, uint32 areaID, byte perc);
void tcShowObjectData(uint32 areaID, dbObjectNode *node, byte perc);

} // End of namespace Clue

#endif

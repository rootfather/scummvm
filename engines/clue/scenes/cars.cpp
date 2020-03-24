/*
**  $Filename: scenes/cars.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-02-94
**
**  "car & vans" functions for "Der Clou!"
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

#include "clue/scenes/scenes.h"
#include "common/util.h"

namespace Clue {

void SetCarColors(byte index) {
	uint8 Col[10][4][3] = {
		{{8, 8, 8}, {7, 7, 7}, {6, 6, 6}, {5, 5, 5}},
		{{1, 12, 8}, {3, 9, 6}, {3, 6, 4}, {2, 5, 3}},
		{{7, 15, 15}, {7, 13, 12}, {5, 10, 10}, {4, 8, 8}},
		{{12, 0, 7}, {10, 0, 9}, {9, 0, 8}, {7, 0, 6}},
		{{12, 8, 7}, {11, 7, 6}, {9, 6, 6}, {8, 5, 5}},
		{{13, 13, 13}, {12, 12, 12}, {10, 10, 10}, {8, 8, 8}},
		{{14, 0, 3}, {13, 0, 3}, {11, 0, 2}, {9, 0, 2}},
		{{0, 14, 1}, {0, 12, 0}, {0, 10, 0}, {0, 8, 0}},
		{{0, 6, 15}, {0, 4, 13}, {0, 2, 11}, {0, 0, 9}},
		{{15, 15, 6}, {13, 13, 3}, {11, 11, 1}, {9, 9, 0}}
	};

	gfxSetRGB(l_gc, 8, (uint32) Col[index][0][0] << 4,
	          (uint32) Col[index][0][1] << 4, (uint32) Col[index][0][2] << 4);
	gfxSetRGB(l_gc, 9, (uint32) Col[index][1][0] << 4,
	          (uint32) Col[index][1][1] << 4, (uint32) Col[index][1][2] << 4);
	gfxSetRGB(l_gc, 10, (uint32) Col[index][2][0] << 4,
	          (uint32) Col[index][2][1] << 4, (uint32) Col[index][2][2] << 4);
	gfxSetRGB(l_gc, 11, (uint32) Col[index][3][0] << 4,
	          (uint32) Col[index][3][1] << 4, (uint32) Col[index][3][2] << 4);
	gfxSetRGB(l_gc, 40, (uint32) Col[index][0][0] << 3,
	          (uint32) Col[index][0][1] << 3, (uint32) Col[index][0][2] << 3);
	gfxSetRGB(l_gc, 41, (uint32) Col[index][1][0] << 3,
	          (uint32) Col[index][1][1] << 3, (uint32) Col[index][1][2] << 3);
	gfxSetRGB(l_gc, 42, (uint32) Col[index][2][0] << 3,
	          (uint32) Col[index][2][1] << 3, (uint32) Col[index][2][2] << 3);
	gfxSetRGB(l_gc, 43, (uint32) Col[index][3][0] << 3,
	          (uint32) Col[index][3][1] << 3, (uint32) Col[index][3][2] << 3);
}

Common::String tcShowPriceOfCar(uint32 nr, uint32 type, void *data) {
	Car car = (Car) data;

	Common::String line1 = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "PRICE_AND_MONEY");
	Common::String line = Common::String::format(line1.c_str(), tcGetCarPrice(car));

	return line;
}

void tcBuyCar() {
	Person marc = (Person) dbGetObject(Person_Marc_Smith);

	byte choice = 0, choice1 = 0;
	while ((choice1 != 2) && (choice != GET_OUT)) {
		ObjectListSuccString = tcShowPriceOfCar;
		ObjectListWidth = 48;

		hasAll(Person_Marc_Smith,
		       OLF_ALIGNED | OLF_PRIVATE_LIST | OLF_INCLUDE_NAME |
		       OLF_INSERT_STAR | OLF_ADD_SUCC_STRING, Object_Car);
		List *bubble = ObjectListPrivate;

		ObjectListSuccString = nullptr;
		ObjectListWidth = 0;

		if (!LIST_EMPTY(bubble)) {
			Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "THANKS");
			ExpandObjectList(bubble, exp);

			ShowMenuBackground();

			if (ChoiceOk(choice = Bubble(bubble, 0, 0L, 0L), GET_OUT, bubble)) {
				Car matts_car = (Car) dbGetObject(((ObjectNode *)GetNthNode(bubble, (uint32) choice))->nr);

				SetCarColors((byte) matts_car->ColorIndex);
				gfxShow((uint16) matts_car->PictID, GFX_NO_REFRESH | GFX_OVERLAY, 0L, -1L, -1L);

				if (Present(((ObjectNode *) GetNthNode(bubble, (uint32) choice))->nr, "Car", InitCarPresent)) {
					choice1 = Say(BUSINESS_TXT, 0, MATT_PICTID, "AUTOKAUF");

					AddVTime(7);

					if (choice1 == 1) {
						uint32 price = tcGetCarPrice(matts_car);

						if (tcSpendMoney(price, 0)) {
							uint32 carID = ((ObjectNode *)GetNthNode(bubble, (uint32) choice))->nr;

							hasSet(Person_Matt_Stuvysunt, carID);
							hasUnSet(Person_Marc_Smith, carID);

							Say(BUSINESS_TXT, 0, marc->PictID, "GOOD CAR");
						}

						if (Say(BUSINESS_TXT, 0, MATT_PICTID, "NACH_AUTOKAUF")
						        == 1)
							choice1 = 2;
					}
				}

				gfxShow(27, GFX_NO_REFRESH | GFX_ONE_STEP, 0L, -1L, -1L);
			} else
				choice = GET_OUT;
		} else {
			Say(BUSINESS_TXT, 0, marc->PictID, "NO_CAR");
			choice = GET_OUT;
		}

		RemoveList(bubble);
	}
}

void tcCarInGarage(uint32 carID) {
	Person marc = (Person) dbGetObject(Person_Marc_Smith);
	Car matts_car = (Car) dbGetObject(carID);

	byte choice1 = 0;
	while (choice1 != 5) {
		ShowMenuBackground();

		switch (choice1 = Say(BUSINESS_TXT, choice1, 7, "GARAGE")) {
		case 0:
			/* Analyse des Wagens */
			tcCarGeneralOverhoul(matts_car);
			break;
		case 1:
			/* Karosserie */
			tcRepairCar(matts_car, "BodyRepair");
			break;
		case 2:
			/* Reifen */
			tcRepairCar(matts_car, "TyreRepair");
			break;
		case 3:
			/* Motor  */
			tcRepairCar(matts_car, "MotorRepair");
			break;
		case 4:
			if (carID != Car_Jaguar_XK_1950)
				tcColorCar(matts_car);
			else
				Say(BUSINESS_TXT, 0, marc->PictID, "JAGUAR_COLOR");
			break;
		default:
			break;
		}
	}
}

void tcColorCar(Car car) {
	Person marc = (Person) dbGetObject(Person_Marc_Smith);
	uint32 costs = (uint32)tcColorCosts(car);

	List *bubble = g_clue->_txtMgr->goKeyAndInsert(BUSINESS_TXT, "LACKIEREN", (uint32) costs, NULL);

	SetPictID(marc->PictID);
	Bubble(bubble, 0, 0L, 0L);
	RemoveList(bubble);

	if (Say(BUSINESS_TXT, 0, MATT_PICTID, "LACKIEREN_ANT") == 0) {
		List *colors = g_clue->_txtMgr->goKey(OBJECTS_ENUM_TXT, "enum_ColorE");

		g_clue->_txtMgr->putCharacter(colors, 0, '*');

		if (tcSpendMoney(costs, 1)) {
			Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "NO_CHOICE");
			ExpandObjectList(colors, exp);

			byte choice = Bubble(colors, (byte) car->ColorIndex, 0L, 0L);
			if (ChoiceOk(choice, GET_OUT, colors)) {
				car->ColorIndex = (ColorE) choice;

				SetCarColors(car->ColorIndex);
				gfxPrepareRefresh();
				PlayAnim("Umlackieren", 3000, GFX_DONT_SHOW_FIRST_PIC);

				inpSetWaitTicks(200L);

				inpWaitFor(INP_LBUTTONP | INP_TIME);

				StopAnim();
				inpSetWaitTicks(1L);

				gfxRefresh();
				/*gfxShow(26,GFX_NO_REFRESH|GFX_ONE_STEP,0L,-1L,-1L);*/
				/*gfxShow((uint16)car->PictID,GFX_NO_REFRESH|GFX_OVERLAY,1L,-1L,-1L);*/
			}
		}

		RemoveList(colors);
	}

	AddVTime(137);
}

void tcSellCar(uint32 ObjectID) {
	Person marc = (Person) dbGetObject(Person_Marc_Smith);
	Car car = (Car) dbGetObject(ObjectID);
	uint32 offer = tcGetCarTraderOffer(car);

	List *bubble;
	if (tcRGetCarAge(car) < 1)
		bubble = g_clue->_txtMgr->goKeyAndInsert(BUSINESS_TXT, "ANGEBOT_1", tcRGetCarValue(car), offer, NULL);
	else
		bubble = g_clue->_txtMgr->goKeyAndInsert(BUSINESS_TXT, "ANGEBOT", tcRGetCarValue(car), tcRGetCarAge(car), offer, NULL);

	SetPictID(marc->PictID);
	Bubble(bubble, 0, 0L, 0L);
	RemoveList(bubble);

	if ((Say(BUSINESS_TXT, 0, MATT_PICTID, "VERKAUF")) == 0) {
		tcAddPlayerMoney(offer);

		hasSet(Person_Marc_Smith, ObjectID);
		hasUnSet(Person_Matt_Stuvysunt, ObjectID);
	}

	gfxShow(27, GFX_NO_REFRESH | GFX_ONE_STEP, 0L, -1L, -1L);
	AddVTime(97);
}

void tcRepairCar(Car car, const char *repairWhat) {
	List *presentationData = CreateList();
	bool enough = true;
	Person marc = (Person) dbGetObject(Person_Marc_Smith);

	List *list = NULL;
	byte *item = NULL;
	uint32 costs = 0;
	byte type = 7;
	if (strcmp(repairWhat, "MotorRepair") == 0) {
		item = &car->MotorState;
		costs = tcCostsPerEngineRepair(car);
		type = 1;
	} else if (strcmp(repairWhat, "BodyRepair") == 0) {
		item = &car->BodyWorkState;
		costs = tcCostsPerBodyRepair(car);
		type = 2;
	} else if (strcmp(repairWhat, "TyreRepair") == 0) {
		item = &car->TyreState;
		costs = tcCostsPerTyreRepair(car);
		type = 4;
	} else
		costs = tcCostsPerTotalRepair(car);
	
	if (!(enough = tcSpendMoney(costs, 0)))
		return;

	list = g_clue->_txtMgr->goKey(PRESENT_TXT, repairWhat);

	gfxPrepareRefresh();
	gfxShow((uint16) BIG_SHEET, GFX_NO_REFRESH | GFX_OVERLAY, 0L, -1L, -1L);
	inpSetWaitTicks(3L);

	PlayAnim("Reperatur", 30000, GFX_DONT_SHOW_FIRST_PIC);

	byte ready = 0;
	uint32 choice = 0L;
	while (!(choice & INP_LBUTTONP) && enough && (!ready)) {
		uint16 line = 0;

		AddVTime(3);

		AddPresentTextLine(presentationData, NULL, 0L, list, line++);

		if (item)
			AddPresentLine(presentationData, PRESENT_AS_BAR, (uint32)(*item), 255L, list, line++);

		AddPresentLine(presentationData, PRESENT_AS_BAR, (uint32)(car->State),
		               255L, list, line++);

		uint32 totalCosts = 0L;
		AddPresentLine(presentationData, PRESENT_AS_NUMBER, totalCosts, 0L, list, line++);
		AddPresentLine(presentationData, PRESENT_AS_NUMBER, (uint32) tcGetPlayerMoney, 0L, list, line++);

		DrawPresent(presentationData, 0, u_gc, (byte) GetNrOfNodes(presentationData));

		RemoveNode(presentationData, NULL);

		choice = inpWaitFor(INP_LBUTTONP | INP_TIME);

		if (choice & INP_TIME) {
			if ((enough = tcSpendMoney(costs, 1))) {
				totalCosts += costs;

				if (type & 1)
					tcSetCarMotorState(car, 1);

				if (type & 2)
					tcSetCarBodyState(car, 1);

				if (type & 4)
					tcSetCarTyreState(car, 1);
			}
		}

		if (item) {
			if ((*item) == 255) {
				StopAnim();
				gfxRefresh();
				Say(BUSINESS_TXT, 0, marc->PictID, "REP_READY");
				ready = 1;
			}
		} else {
			if (tcGetCarTotalState(car) > 253) {
				StopAnim();
				gfxRefresh();
				Say(BUSINESS_TXT, 0, marc->PictID, "REP_READY");
				ready = 1;
			}
		}
	}

	if (!ready) {
		StopAnim();
		gfxRefresh();
	}

	inpSetWaitTicks(0L);

	/*gfxShow(26,GFX_NO_REFRESH|GFX_ONE_STEP,0L,-1L,-1L);*/
	/*gfxShow((uint16)car->PictID,GFX_NO_REFRESH|GFX_OVERLAY,1L,-1L,-1L);*/

	RemoveList(presentationData);
	RemoveList(list);
}

uint32 tcChooseCar(uint32 backgroundNr) {
	hasAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Car);

	List *bubble = ObjectListPrivate;
	uint32 carID = 0L;
	if (!(LIST_EMPTY(bubble))) {
		uint32 carCount = GetNrOfNodes(bubble);
		byte choice;
		if (carCount == 1) {
			carID = OL_NR(LIST_HEAD(bubble));
			choice = 1;
		} else {
			Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "NO_CHOICE");
			ExpandObjectList(bubble, exp);

			Say(BUSINESS_TXT, 0, 7, "ES GEHT UM..");

			if (ChoiceOk((choice = Bubble(bubble, 0, 0L, 0L)), GET_OUT, bubble))
				carID = OL_NR(GetNthNode(bubble, (uint32) choice));
			else
				choice = GET_OUT;
		}

		if (choice != GET_OUT) {
			Car matts_car = (Car) dbGetObject(carID);
			SetCarColors((byte) matts_car->ColorIndex);
			gfxShow(backgroundNr, GFX_NO_REFRESH | GFX_ONE_STEP, 0L, -1L, -1L);
			gfxShow((uint16) matts_car->PictID, GFX_NO_REFRESH | GFX_OVERLAY, 1L, -1L, -1L);
		}
	}

	RemoveList(bubble);

	return (carID);
}

void tcCarGeneralOverhoul(Car car) {
	Person marc = (Person)dbGetObject(Person_Marc_Smith);

	SetPictID(marc->PictID);

	List *bubble = g_clue->_txtMgr->goKeyAndInsert(BUSINESS_TXT, "GENERAL_OVERHOUL",
	                      (uint32)((tcCostsPerTotalRepair(car) * 255) / 8), NULL);
	Bubble(bubble, 0, 0L, 0L);
	RemoveList(bubble);

	byte choice = Say(BUSINESS_TXT, 0, MATT_PICTID, "GENERAL_OVERHOUL_QUEST");

	if (choice == 0)
		tcRepairCar(car, "TotalRepair");
}

} // End of namespace Clue

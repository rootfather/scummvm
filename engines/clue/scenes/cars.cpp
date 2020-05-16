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

#include "common/util.h"

#include "clue/scenes/scenes.h"
#include "clue/present/interac.h"
#include "clue/data/dataappl.h"

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

Common::String tcShowPriceOfCar(uint32 nr, uint32 type, dbObjectNode *node) {
	CarNode *car = (CarNode *) node;

	Common::String line1 = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "PRICE_AND_MONEY");
	Common::String line = Common::String::format(line1.c_str(), tcGetCarPrice(car));

	return line;
}

void tcBuyCar() {
	PersonNode *marc = (PersonNode *) dbGetObject(Person_Marc_Smith);

	byte choice = 0, choice1 = 0;
	while (choice1 != 2 && choice != GET_OUT) {
		ObjectListSuccString = tcShowPriceOfCar;
		ObjectListWidth = 48;

		hasAll(Person_Marc_Smith,
		       OLF_ALIGNED | OLF_PRIVATE_LIST | OLF_INCLUDE_NAME |
		       OLF_INSERT_STAR | OLF_ADD_SUCC_STRING, Object_Car);
		NewObjectList<dbObjectNode> *bubble = ObjectListPrivate;

		ObjectListSuccString = nullptr;
		ObjectListWidth = 0;

		if (!bubble->isEmpty()) {
			Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "THANKS");
			bubble->expandObjectList(exp);

			ShowMenuBackground();

			choice = Bubble((NewList<NewNode> *)bubble, 0, 0, 0);
			if (ChoiceOk(choice, GET_OUT, bubble)) {
				CarNode *matts_car = (CarNode *) dbGetObject(bubble->getNthNode((uint32) choice)->_nr);

				SetCarColors((byte) matts_car->ColorIndex);
				gfxShow((uint16) matts_car->PictID, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);

				if (Present(bubble->getNthNode((uint32) choice)->_nr, "Car", InitCarPresent)) {
					choice1 = Say(BUSINESS_TXT, 0, MATT_PICTID, "AUTOKAUF");

					addVTime(7);

					if (choice1 == 1) {
						uint32 price = tcGetCarPrice(matts_car);

						if (tcSpendMoney(price, false)) {
							uint32 carID = bubble->getNthNode((uint32) choice)->_nr;

							hasSet(Person_Matt_Stuvysunt, carID);
							hasUnSet(Person_Marc_Smith, carID);

							Say(BUSINESS_TXT, 0, marc->PictID, "GOOD CAR");
						}

						if (Say(BUSINESS_TXT, 0, MATT_PICTID, "NACH_AUTOKAUF") == 1)
							choice1 = 2;
					}
				}

				gfxShow(27, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);
			} else
				choice = GET_OUT;
		} else {
			Say(BUSINESS_TXT, 0, marc->PictID, "NO_CAR");
			choice = GET_OUT;
		}

		bubble->removeList();
	}
}

void tcCarInGarage(uint32 carID) {
	PersonNode *marc = (PersonNode *) dbGetObject(Person_Marc_Smith);
	CarNode *matts_car = (CarNode *) dbGetObject(carID);

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

void tcColorCar(CarNode *car) {
	PersonNode *marc = (PersonNode *) dbGetObject(Person_Marc_Smith);
	uint32 costs = (uint32)tcColorCosts(car);

	NewList<NewNode> *bubble = g_clue->_txtMgr->goKeyAndInsert(BUSINESS_TXT, "LACKIEREN", (uint32) costs, NULL);

	SetPictID(marc->PictID);
	Bubble(bubble, 0, nullptr, 0);
	bubble->removeList();

	if (Say(BUSINESS_TXT, 0, MATT_PICTID, "LACKIEREN_ANT") == 0) {
		NewList<NewNode> *colors = g_clue->_txtMgr->goKey(OBJECTS_ENUM_TXT, "enum_ColorE");

		colors->putCharacter(0, '*');

		if (tcSpendMoney(costs, true)) {
			Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "NO_CHOICE");
			// FIXME : weird call. Replaced by createNode to have something visible for a test
			// colors->expandObjectList(exp);
			warning("plOpen - Workaround used - to be fixed.");
			colors->createNode(exp);
			// 

			byte choice = Bubble(colors, (byte) car->ColorIndex, nullptr, 0);
			if (ChoiceOkHack(choice, GET_OUT, colors)) {
				car->ColorIndex = (ColorE) choice;

				SetCarColors(car->ColorIndex);
				gfxPrepareRefresh();
				g_clue->_animMgr->playAnim("Umlackieren", 3000, GFX_DONT_SHOW_FIRST_PIC);

				inpSetWaitTicks(200);

				inpWaitFor(INP_LBUTTONP | INP_TIME);

				g_clue->_animMgr->stopAnim();
				inpSetWaitTicks(1);

				gfxRefresh();
				/*gfxShow(26,GFX_NO_REFRESH|GFX_ONE_STEP,0,-1,-1);*/
				/*gfxShow((uint16)car->PictID,GFX_NO_REFRESH|GFX_OVERLAY,1,-1,-1);*/
			}
		}

		colors->removeList();
	}

	addVTime(137);
}

void tcSellCar(uint32 ObjectID) {
	PersonNode *marc = (PersonNode *) dbGetObject(Person_Marc_Smith);
	CarNode *car = (CarNode *) dbGetObject(ObjectID);
	uint32 offer = tcGetCarTraderOffer(car);

	NewList<NewNode> *bubble;
	if (tcRGetCarAge(car) < 1)
		bubble = g_clue->_txtMgr->goKeyAndInsert(BUSINESS_TXT, "ANGEBOT_1", tcRGetCarValue(car), offer, NULL);
	else
		bubble = g_clue->_txtMgr->goKeyAndInsert(BUSINESS_TXT, "ANGEBOT", tcRGetCarValue(car), tcRGetCarAge(car), offer, NULL);

	SetPictID(marc->PictID);
	Bubble(bubble, 0, nullptr, 0);
	bubble->removeList();

	if ((Say(BUSINESS_TXT, 0, MATT_PICTID, "VERKAUF")) == 0) {
		tcAddPlayerMoney(offer);

		hasSet(Person_Marc_Smith, ObjectID);
		hasUnSet(Person_Matt_Stuvysunt, ObjectID);
	}

	gfxShow(27, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);
	addVTime(97);
}

void tcRepairCar(CarNode *car, const char *repairWhat) {
	NewList<PresentationInfoNode> *presentationData = new NewList<PresentationInfoNode>;
	bool enough = true;
	PersonNode *marc = (PersonNode *) dbGetObject(Person_Marc_Smith);

	NewList<NewNode> *list = NULL;
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
	
	if (!(enough = tcSpendMoney(costs, false)))
		return;

	list = g_clue->_txtMgr->goKey(PRESENT_TXT, repairWhat);

	gfxPrepareRefresh();
	gfxShow((uint16) BIG_SHEET, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);
	inpSetWaitTicks(3);

	g_clue->_animMgr->playAnim("Reperatur", 30000, GFX_DONT_SHOW_FIRST_PIC);

	byte ready = 0;
	uint32 choice = 0;
	while (!(choice & INP_LBUTTONP) && enough && (!ready)) {
		uint16 line = 0;

		addVTime(3);

		AddPresentTextLine(presentationData, nullptr, 0, list, line++);

		if (item)
			AddPresentLine(presentationData, PRESENT_AS_BAR, (uint32)(*item), 255, list, line++);

		AddPresentLine(presentationData, PRESENT_AS_BAR, (uint32)(car->State),
		               255, list, line++);

		uint32 totalCosts = 0;
		AddPresentLine(presentationData, PRESENT_AS_NUMBER, totalCosts, 0, list, line++);
		AddPresentLine(presentationData, PRESENT_AS_NUMBER, (uint32) tcGetPlayerMoney, 0, list, line++);

		DrawPresent(presentationData, 0, u_gc, (byte)presentationData->getNrOfNodes());

		presentationData->removeNode(nullptr);

		choice = inpWaitFor(INP_LBUTTONP | INP_TIME);

		if (choice & INP_TIME) {
			if ((enough = tcSpendMoney(costs, true))) {
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
				g_clue->_animMgr->stopAnim();
				gfxRefresh();
				Say(BUSINESS_TXT, 0, marc->PictID, "REP_READY");
				ready = 1;
			}
		} else {
			if (tcGetCarTotalState(car) > 253) {
				g_clue->_animMgr->stopAnim();
				gfxRefresh();
				Say(BUSINESS_TXT, 0, marc->PictID, "REP_READY");
				ready = 1;
			}
		}
	}

	if (!ready) {
		g_clue->_animMgr->stopAnim();
		gfxRefresh();
	}

	inpSetWaitTicks(0);

	/*gfxShow(26,GFX_NO_REFRESH|GFX_ONE_STEP,0,-1,-1);*/
	/*gfxShow((uint16)car->PictID,GFX_NO_REFRESH|GFX_OVERLAY,1,-1,-1);*/

	presentationData->removeList();
	list->removeList();
}

uint32 tcChooseCar(uint32 backgroundNr) {
	hasAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Car);

	NewObjectList<dbObjectNode> *bubble = ObjectListPrivate;
	uint32 carID = 0;
	if (!bubble->isEmpty()) {
		uint32 carCount = bubble->getNrOfNodes();
		byte choice;
		if (carCount == 1) {
			carID = bubble->getListHead()->_nr;
			choice = 1;
		} else {
			Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "NO_CHOICE");
			bubble->expandObjectList(exp);

			Say(BUSINESS_TXT, 0, 7, "ES GEHT UM..");

			choice = Bubble((NewList<NewNode>*)bubble, 0, nullptr, 0);
			if (ChoiceOk(choice, GET_OUT, bubble))
				carID = bubble->getNthNode((uint32) choice)->_nr;
			else
				choice = GET_OUT;
		}

		if (choice != GET_OUT) {
			CarNode *matts_car = (CarNode *) dbGetObject(carID);
			SetCarColors((byte) matts_car->ColorIndex);
			gfxShow(backgroundNr, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);
			gfxShow((uint16) matts_car->PictID, GFX_NO_REFRESH | GFX_OVERLAY, 1, -1, -1);
		}
	}

	bubble->removeList();

	return carID;
}

void tcCarGeneralOverhoul(CarNode *car) {
	PersonNode *marc = (PersonNode *)dbGetObject(Person_Marc_Smith);

	SetPictID(marc->PictID);

	NewList<NewNode> *bubble = g_clue->_txtMgr->goKeyAndInsert(BUSINESS_TXT, "GENERAL_OVERHOUL",
	                      (uint32)((tcCostsPerTotalRepair(car) * 255) / 8), NULL);
	Bubble(bubble, 0, nullptr, 0);
	bubble->removeList();

	byte choice = Say(BUSINESS_TXT, 0, MATT_PICTID, "GENERAL_OVERHOUL_QUEST");

	if (choice == 0)
		tcRepairCar(car, "TotalRepair");
}

} // End of namespace Clue

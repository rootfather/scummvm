/*
**  $Filename: organisa/organisa.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     07-04-94
**
**  functions for organisation of a burglary for "Der Clou!"
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

#include "clue/base/base.h"
#include "clue/clue.h"
#include "clue/present/interac.h"
#include "clue/data/dataappl.h"
#include "clue/organisa/organisa.h"
#include "clue/planing/planer.h"


namespace Clue {

uint32 tcChooseDestBuilding(uint32);
uint32 tcChooseEscapeCar(uint32);
uint32 tcChooseDriver(uint32);

void tcChooseAccomplice();
void tcSpreadTools();
void tcChooseTime();
void tcChooseGuys();
void tcCheckGuyCount();
void tcAddGuyToParty();
void tcRemGuyFromParty();

void tcAddToolToGuy();
void tcRemToolFromGuy();

bool tcCheckOrganisation();

/* display functions */

void tcInitDisplayOrganisation();
void tcDoneDisplayOrganisation();

void tcDisplayOrganisation();

void tcDisplayCommon();
void tcDisplayPerson(uint32 displayMode);
void tcDisplayAbilities(uint32 personNr, uint32 displayData);
void tcDisplayTools(uint32 personNr, uint32 displayData);

struct Organisation Organisation;

}

#include "clue/organisa/display.cpp"

namespace Clue {

void tcResetOrganisation() {
	Organisation.CarID = 0;
	Organisation.DriverID = 0;
	Organisation.BuildingID = 0;
	Organisation.GuyCount = 0;
	Organisation.PlacesInCar = 0;
}

static void tcOrganisationSetBuilding() {
	hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Building);
	Organisation.BuildingID = ObjectList->getListHead()->_nr;
}

static void tcOrganisationSetCar() {
	hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Car);

	Organisation.CarID = ObjectList->getListHead()->_nr;

	CarNode *car = (CarNode *)dbGetObject(Organisation.CarID);

	Organisation.PlacesInCar = car->PlacesInCar;
}

static byte tcMakeCarOk() {
	joined_byAll(Person_Matt_Stuvysunt, OLF_INCLUDE_NAME | OLF_INSERT_STAR,
	             Object_Person);

	if (ObjectList->getNrOfNodes() > Organisation.PlacesInCar) {
		SetBubbleType(THINK_BUBBLE);
		if (GET_OUT ==
		        Say(BUSINESS_TXT, 0,
		            ((PersonNode *) dbGetObject(Person_Matt_Stuvysunt))->PictID,
		            "PLAN_TO_MANY_GUYS"))
			return 0;

		while (ObjectList->getNrOfNodes() > Organisation.PlacesInCar) {
			dbRemObjectNode(ObjectList, Person_Matt_Stuvysunt);

			inpTurnESC(false);

			byte choice = Bubble((NewList<NewNode> *)ObjectList, 0, nullptr, 0);
			Organisation.GuyCount--;
			joined_byUnSet(Person_Matt_Stuvysunt, ObjectList->getNthNode((uint32) choice)->_nr);

			inpTurnESC(true);

			joined_byAll(Person_Matt_Stuvysunt, OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Person);
		}
	}

	return 1;
}

uint32 tcOrganisation() {
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "ORGANISATION");
	byte activ = 0, ende = 0;

	/* activate first or memorized building */
	Organisation.BuildingID = 0;
	Organisation.CarID = 0;
	Organisation.DriverID = 0;

	rememberAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Building);

	if (!ObjectList->isEmpty())
		Organisation.BuildingID = ObjectList->getListHead()->_nr;

	if (Organisation.BuildingID) {
		if (!has(Person_Matt_Stuvysunt, Organisation.BuildingID))
			tcOrganisationSetBuilding();
	} else
		tcOrganisationSetBuilding();

	/* remember current building */
	rememberSet(Person_Matt_Stuvysunt, Organisation.BuildingID);

	/* activate first or memorized car */
	rememberAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Car);

	if (!ObjectList->isEmpty()) {
		Organisation.CarID = ObjectList->getListHead()->_nr;

		CarNode *car = (CarNode *)dbGetObject(Organisation.CarID);

		Organisation.PlacesInCar = car->PlacesInCar;
	}

	if (Organisation.CarID) {
		if (!has(Person_Matt_Stuvysunt, Organisation.CarID))
			tcOrganisationSetCar();
	} else
		tcOrganisationSetCar();

	/* remember current car */
	rememberSet(Person_Matt_Stuvysunt, Organisation.CarID);

	/* activate first or memorized driver */
	rememberAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Person);

	if (!ObjectList->isEmpty())
		Organisation.DriverID = ObjectList->getListHead()->_nr;

	if (!tcMakeCarOk())
		return 0;

	tcInitDisplayOrganisation();
	tcDisplayOrganisation();

	Common::String line = g_clue->_txtMgr->getFirstLine(THECLOU_TXT, "ORGANISATION");

	while (!ende) {
		inpTurnESC(false);
		inpTurnFunctionKey(false);

		PrintStatus(line);

		activ = Menu(menu, (uint32) 255, activ, 0, 0);
		inpTurnESC(true);

		switch (activ) {
		case 0:
			Organisation.BuildingID =
			    tcChooseDestBuilding(Organisation.BuildingID);
			tcDisplayOrganisation();
			break;
		case 1:
			tcChooseGuys();
			/* tcDisplayOrganisation(); done in tcChooseGuys */
			break;
		case 2:
			Organisation.CarID = tcChooseEscapeCar(Organisation.CarID);
			tcDisplayOrganisation();
			break;
		case 3:
			Organisation.DriverID = tcChooseDriver(Organisation.DriverID);
			tcDisplayOrganisation();
			break;
		case 4:
			Information();
			tcDisplayOrganisation();
			break;
		case 5:
			if (!Organisation.BuildingID)
				Organisation.BuildingID =
				    tcChooseDestBuilding(Organisation.BuildingID);

			if (tcCheckOrganisation()) {
				gfxClearArea(l_gc);

				tcDoneDisplayOrganisation();

				plPlaner(Organisation.BuildingID);

				tcInitDisplayOrganisation();
				tcDisplayOrganisation();
			}
			break;
		case 6:
			if (tcCheckOrganisation())
				ende = 2;
			break;
		case 7:
			ende = 1;       /* ende */
			break;
		default:
			break;
		}
	}

	menu->removeList();

	tcDoneDisplayOrganisation();

	return ((ende - 1) * Organisation.BuildingID);
}

bool tcCheckOrganisation() {
	PlayerNode *player = (PlayerNode *)dbGetObject(Player_Player_1);
	bool check = false;

	if (Organisation.BuildingID) {
		if ((((BuildingNode *) dbGetObject(Organisation.BuildingID))->Exactlyness) > 127) {
			if (Organisation.DriverID) {
				if (player->NrOfBurglaries == 8) {
					if (Organisation.CarID == Car_Jaguar_XK_1950)
						check = true;
					else
						Say(BUSINESS_TXT, 0, MATT_PICTID, "PLAN_NO_JAGUAR");
				} else {
					if (g_clue->getFeatures() & GF_PROFIDISK) {
						if (Organisation.BuildingID == Building_Westminster_Abbey) {
							if ((Organisation.CarID == Car_Fiat_634_N_1936)
							        || (Organisation.CarID == Car_Fiat_634_N_1943))
								check = true;
							else
								Say(BUSINESS_TXT, 0, MATT_PICTID, "PLAN_NO_FIAT");
						} else
							check = true;
					} else {
						check = true;
					}
				}
			} else
				Say(BUSINESS_TXT, 0, MATT_PICTID, "PLAN_NO_DRIVER");
		} else
			Say(BUSINESS_TXT, 0, MATT_PICTID, "PLAN_NO_KNOWL");
	} else
		Say(BUSINESS_TXT, 0, MATT_PICTID, "PLAN_NO_BUILDING");

	return check;
}

uint32 tcChooseDriver(uint32 persID) {
	PersonNode *matt = (PersonNode *) dbGetObject(Person_Matt_Stuvysunt);

	joined_byAll(Person_Matt_Stuvysunt,
	             OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_PRIVATE_LIST,
	             Object_Person);
	NewObjectList<dbObjectNode> *list = ObjectListPrivate;

	if (list->isEmpty()) {
		SetBubbleType(THINK_BUBBLE);
		Say(BUSINESS_TXT, 0, matt->PictID, "PLAN_TO_FEW_GUYS");
	} else {
		Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "NO_CHOICE");
		list->expandObjectList(exp);

		byte choice = Bubble((NewList<NewNode>*)list, 0, nullptr, 0);
		if (ChoiceOk(choice, GET_OUT, list)) {
			uint32 newPersID = list->getNthNode((uint32) choice)->_nr;

			if (!has(newPersID, Ability_Autos)) {
				PersonNode *pers = (PersonNode *)dbGetObject(newPersID);

				Say(BUSINESS_TXT, 0, pers->PictID, "PLAN_CANT_DRIVE");
			} else {
				persID = newPersID;

				rememberAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Person);

				if (!ObjectList->isEmpty())
					rememberUnSet(Person_Matt_Stuvysunt, ObjectList->getListHead()->_nr);

				rememberSet(Person_Matt_Stuvysunt, persID);
			}
		}
	}

	list->removeList();

	return (persID);
}

uint32 tcChooseDestBuilding(uint32 objID) {
	hasAll(Person_Matt_Stuvysunt,
	       OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_PRIVATE_LIST,
	       Object_Building);

	NewObjectList<dbObjectNode> *list = ObjectListPrivate;
	Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "NO_CHOICE");
	list->expandObjectList(exp);

	byte choice = Bubble((NewList<NewNode>*)list, 0, nullptr, 0);
	if (ChoiceOk(choice, GET_OUT, list)) {
		objID = list->getNthNode((uint32) choice)->_nr;

		rememberAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Building);

		if (!ObjectList->isEmpty())
			rememberUnSet(Person_Matt_Stuvysunt, ObjectList->getListHead()->_nr);

		rememberSet(Person_Matt_Stuvysunt, objID);
	}

	list->removeList();

	return objID;
}

uint32 tcChooseEscapeCar(uint32 objID) {
	PersonNode *matt = (PersonNode *) dbGetObject(Person_Matt_Stuvysunt);
	hasAll(Person_Matt_Stuvysunt,
	       OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_PRIVATE_LIST, Object_Car);

	NewObjectList<dbObjectNode> *l1 = ObjectListPrivate;

	joined_byAll(Person_Matt_Stuvysunt, OLF_INCLUDE_NAME | OLF_PRIVATE_LIST, Object_Person);
	NewObjectList<dbObjectNode> *l2 = ObjectListPrivate;

	if (!l1->isEmpty()) {
		Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "NO_CHOICE");
		l1->expandObjectList(exp);

		byte choice = Bubble((NewList<NewNode>*)l1, 0, nullptr, 0);
		if (ChoiceOk(choice, GET_OUT, l1)) {
			uint32 newObjID = l1->getNthNode((uint32) choice)->_nr;

			CarNode *car = (CarNode *)l1->getNthNode((uint32) choice);

			if (l2->getNrOfNodes() <= car->PlacesInCar) {
				Organisation.PlacesInCar = car->PlacesInCar;
				objID = newObjID;

				rememberAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Car);

				if (!ObjectList->isEmpty())
					rememberUnSet(Person_Matt_Stuvysunt,
					              ObjectList->getListHead()->_nr);

				rememberSet(Person_Matt_Stuvysunt, objID);
			} else {
				SetBubbleType(THINK_BUBBLE);
				Say(BUSINESS_TXT, 0, matt->PictID, "PLAN_NO_PLACE");
			}
		}
	} else {
		SetBubbleType(THINK_BUBBLE);
		Say(BUSINESS_TXT, 0, matt->PictID, "PLAN_WITHOUT_CAR");
	}

	l2->removeList();
	l1->removeList();

	return objID;
}

void tcChooseGuys() {
	PersonNode *matt = (PersonNode *) dbGetObject(Person_Matt_Stuvysunt);

	joinAll(Person_Matt_Stuvysunt,
	        OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_PRIVATE_LIST,
	        Object_Person);
	NewList<dbObjectNode> *list = ObjectListPrivate;

	dbRemObjectNode(list, Person_Matt_Stuvysunt);

	if (list->isEmpty()) {
		SetBubbleType(THINK_BUBBLE);
		Say(BUSINESS_TXT, 0, matt->PictID, "PLAN_WITHOUT_GUYS");
	} else {
		NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "ORG_KOMPLIZEN");
		byte activ = 0;

		ShowMenuBackground();
		Common::String line = g_clue->_txtMgr->getFirstLine(THECLOU_TXT, "KOMPLIZEN");
		PrintStatus(line);

		while (activ != 2) {
			inpTurnESC(false);
			activ = Menu(menu, 7, activ, nullptr, 0);
			inpTurnESC(true);

			switch (activ) {
			case 0:
				tcAddGuyToParty();
				break;
			case 1:
				tcRemGuyFromParty();
				break;
			default:
				break;
			}
		}

		ShowMenuBackground();
		menu->removeList();
	}

	list->removeList();
}

void tcAddGuyToParty() {
	joinAll(Person_Matt_Stuvysunt,
	        OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_PRIVATE_LIST,
	        Object_Person);

	NewObjectList<dbObjectNode> *l1 = ObjectListPrivate;
	joined_byAll(Person_Matt_Stuvysunt, OLF_INCLUDE_NAME | OLF_PRIVATE_LIST, Object_Person);

	NewObjectList<dbObjectNode> *l2 = ObjectListPrivate;
	if (l2->getNrOfNodes() < Organisation.PlacesInCar) {
		for (dbObjectNode *n = l2->getListHead(); n->_succ; n = (dbObjectNode *) n->_succ)
			dbRemObjectNode(l1, n->_nr);

		if (!l1->isEmpty()) {
			Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "NO_CHOICE");
			l1->expandObjectList(exp);

			byte choice = Bubble((NewList<NewNode>*)l1, 0, nullptr, 0);
			if (ChoiceOk(choice, GET_OUT, l1)) {
				uint32 persID = l1->getNthNode((uint32)choice)->_nr;

				Organisation.GuyCount++;
				joined_bySet(Person_Matt_Stuvysunt, persID);

				tcDisplayOrganisation();
			}
		} else {
			SetBubbleType(THINK_BUBBLE);
			Say(BUSINESS_TXT, 0,
			    ((PersonNode *) dbGetObject(Person_Matt_Stuvysunt))->PictID, "PLAN_DO_NOT_KNOW_ANYBODY");
		}
	} else {
		SetBubbleType(THINK_BUBBLE);
		Say(BUSINESS_TXT, 0,
		    ((PersonNode *) dbGetObject(Person_Matt_Stuvysunt))->PictID, "PLAN_CAR_FULL");
	}

	l2->removeList();
	l1->removeList();
}

void tcRemGuyFromParty() {
	PersonNode *matt = (PersonNode *) dbGetObject(Person_Matt_Stuvysunt);

	joined_byAll(Person_Matt_Stuvysunt,
	             OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_PRIVATE_LIST,
	             Object_Person);
	NewObjectList<dbObjectNode> *list = ObjectListPrivate;

	dbRemObjectNode(list, Person_Matt_Stuvysunt);

	if (list->isEmpty()) {
		SetBubbleType(THINK_BUBBLE);
		Say(BUSINESS_TXT, 0, matt->PictID, "PLAN_TO_FEW_GUYS");
	} else {
		Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "NO_CHOICE");
		list->expandObjectList(exp);

		byte choice = Bubble((NewList<NewNode>*)list, 0, nullptr, 0);
		if (ChoiceOk(choice, GET_OUT, list)) {
			uint32 persID = list->getNthNode((uint32) choice)->_nr;

			Organisation.GuyCount--;
			joined_byUnSet(Person_Matt_Stuvysunt, persID);

			if (persID == Organisation.DriverID) {
				rememberAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Person);

				if (!ObjectList->isEmpty())
					rememberUnSet(Person_Matt_Stuvysunt, ObjectList->getListHead()->_nr);

				Organisation.DriverID = 0;
			}

			tcDisplayOrganisation();
		}
	}

	list->removeList();
}

} // End of namespace Clue

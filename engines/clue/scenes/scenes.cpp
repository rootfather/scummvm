/*
**  $Filename: scenes/scenes.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-04-94
**
**  standard scenes for "Der Clou!"
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
#include "clue/gameplay/gp.h"
#include "clue/clue.h"
#include "clue/present/interac.h"
#include "clue/data/dataappl.h"


namespace Clue {

uint16 CurrentBackground = BGD_LONDON;


/**************************************************
 * a little static func for presentation of data
 */

static byte tcDisplayInfoAboutPerson(uint32 objID) {
	if (has(Person_Matt_Stuvysunt, objID))  /* hat Matt Wissen über ihn? */
		return Present(objID, "Person", InitPersonPresent);

	Common::String name = dbGetObjectName(objID);
	NewList<NewNode> *bubble = g_clue->_txtMgr->goKey(LOOK_TXT, name.c_str());

	SetBubbleType(THINK_BUBBLE);
	Bubble(bubble, 0, nullptr, 0);
	bubble->removeList();

	return 0;
}


/**************************************************
 * taxi stuff
 *
 * AddTaxiLocation
 * RemTaxiLocation
 */

void AddTaxiLocation(uint32 locNr) {
	uint32 objNr = GetObjNrOfLocation(locNr);

	if (objNr)
		taxiSet(Person_Matt_Stuvysunt, objNr);
}

void RemTaxiLocation(uint32 locNr) {
	uint32 objNr = GetObjNrOfLocation(locNr);

	if (objNr)
		taxiUnSet(Person_Matt_Stuvysunt, objNr);
}

/***************************************************
 * standard scenes
 * Go
 * Information
 * Look
 * Telefon
 * Wait
 */

uint32 Go(NewList<NewTCEventNode> *succ) {
	uint32 succ_eventnr;
	inpTurnFunctionKey(false);

	if (succ->getNrOfNodes() > 1) {
		uint32 prob = 0;

		ShowMenuBackground();

		Common::String line = g_clue->_txtMgr->getFirstLine(THECLOU_TXT, "Gehen");
		PrintStatus(line);

		for (NewTCEventNode *node = succ->getListHead(); node->_succ; node = (NewTCEventNode *) node->_succ) {
			Scene *sc = getScene(node->_eventNr);
			NewTCEventNode *location = _film->_locationNames->getNthNode(sc->_locationNr);

			node->_name = location->_name;
			prob++;
		}

		prob = (1 << prob) - 1;
		prob = Menu((NewList<NewNode> *)succ, prob, 0, nullptr, 0);

		succ_eventnr = succ->getNthNode(prob)->_eventNr;

		/* jetzt die Zuweisung wieder entfernen, damit der Name */
		/* nicht 2 mal freigegeben wird */

		for (NewTCEventNode *node = succ->getListHead(); node->_succ; node = (NewTCEventNode *) node->_succ)
			node->_name = "";
	} else {
		succ_eventnr = succ->getNthNode(0)->_eventNr;
	}

	inpTurnFunctionKey(true);

	return succ_eventnr;
}


void Information() {
	inpTurnESC(true);

	byte choice = 0;
	byte ret = 0;
	while (choice != 6 && ret != GET_OUT) {
		SetBubbleType(THINK_BUBBLE);
		NewList<NewNode> *bubble = g_clue->_txtMgr->goKey(THECLOU_TXT, "INFORMATION");
		choice = Bubble(bubble, choice, nullptr, 0);
		bubble->removeList();

		byte choice1 = 0;
		switch (choice) {
		case 0:
			if (Present(Player_Player_1, "Player", InitPlayerPresent) != GET_OUT)
				Present(Person_Matt_Stuvysunt, "Person", InitPersonPresent);
			break;

		case 1: {
			hasAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Car);
			NewObjectList<dbObjectNode> *list = ObjectListPrivate;

			if (!list->isEmpty()) {
				Common::String enough =  g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "GENUG");
				list->expandObjectList(enough);
				SetBubbleType(THINK_BUBBLE);

				choice1 = Bubble((NewList<NewNode>*)list, choice1, nullptr, 0);
				while (ChoiceOk(choice1, GET_OUT, list)) {
					SetBubbleType(THINK_BUBBLE);

					uint32 objID = list->getNthNode((uint32) choice1)->_nr;
					ret = Present(objID, "Car", InitCarPresent);

					choice1 = Bubble((NewList<NewNode>*)list, choice1, nullptr, 0);
				}
			} else
				Say(BUSINESS_TXT, 0, MATT_PICTID, "POOR");

			list->removeList();
			}
			break;

		case 2: {
			knowsAll(Person_Matt_Stuvysunt,
			         OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR,
			         Object_Person);
			NewObjectList<dbObjectNode> *list = ObjectListPrivate;

			if (!list->isEmpty()) {
				Common::String enough = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "GENUG");
				list->expandObjectList(enough);
				SetBubbleType(THINK_BUBBLE);

				choice1 = Bubble((NewList<NewNode>*)list, choice1, nullptr, 0);
				while (ChoiceOk(choice1, GET_OUT, list)) {
					SetBubbleType(THINK_BUBBLE);

					uint32 objID = list->getNthNode((uint32) choice1)->_nr;
					ret = tcDisplayInfoAboutPerson(objID);

					choice1 = Bubble((NewList<NewNode>*)list, choice1, nullptr, 0);
				}
			} else
				Say(BUSINESS_TXT, 0, MATT_PICTID, "POOR");

			list->removeList();
			}
			break;

		case 3: {
			hasAll(Person_Matt_Stuvysunt,
			       OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR,
			       Object_Tool);
			NewObjectList<dbObjectNode> *list = ObjectListPrivate;

			if (!list->isEmpty()) {
				SetBubbleType(THINK_BUBBLE);
				Common::String enough = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "GENUG");
				list->expandObjectList(enough);

				choice1 = Bubble((NewList<NewNode>*)list, choice1, nullptr, 0);
				while (ChoiceOk(choice1, GET_OUT, list)) {
					SetBubbleType(THINK_BUBBLE);

					uint32 objID = list->getNthNode((uint32) choice1)->_nr;
					ret = Present(objID, "Tool", InitToolPresent);

					choice1 = Bubble((NewList<NewNode>*)list, choice1, nullptr, 0);
				}
			} else
				Say(BUSINESS_TXT, 0, MATT_PICTID, "POOR");

			list->removeList();
			}
			break;

		case 4: {
			hasAll(Person_Matt_Stuvysunt,
			       OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR,
			       Object_Building);
			NewObjectList<dbObjectNode> *list = ObjectListPrivate;

			if (!list->isEmpty()) {
				SetBubbleType(THINK_BUBBLE);
				Common::String enough = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "GENUG");
				list->expandObjectList(enough);

				choice1 = Bubble((NewList<NewNode>*)list, choice1, nullptr, 0);
				while (ChoiceOk(choice1, GET_OUT, list)) {
					SetBubbleType(THINK_BUBBLE);

					uint32 objID = list->getNthNode((uint32) choice1)->_nr;
					ret = Present(objID, "Building", InitBuildingPresent);

					choice1 = Bubble((NewList<NewNode>*)list, choice1, nullptr, 0);
				}
			} else
				Say(BUSINESS_TXT, 0, MATT_PICTID, "POOR");

			list->removeList();
			}
			break;

		case 5:
			Present(0, "Beute", InitLootPresent);
			break;

		default:
			choice = 6;
			break;
		}
	}
}


void Look(uint32 locNr) {
	inpTurnESC(true);

	byte choice = 0;
	while (choice != 2) {
		NewList<NewNode> *menu = g_clue->_txtMgr->goKey(THECLOU_TXT, "UMSEHEN");

		SetBubbleType(THINK_BUBBLE);
		choice = Bubble(menu, 0, nullptr, 0);
		menu->removeList();

		byte choice1 = 0;

		switch (choice) {
		case 0: {
			NewList<NewNode> *bubble = g_clue->_txtMgr->goKey(HOUSEDESC_TXT, _film->_locationNames->getNthNode(locNr)->_name.c_str());

			SetBubbleType(THINK_BUBBLE);
			choice1 = Bubble(bubble, 0, nullptr, 0);
			bubble->removeList();
			}
			break;

		case 1: {
			uint32 objID = GetObjNrOfLocation(locNr);
			NewObjectList<dbObjectNode> *bubble = nullptr;
			if (objID) {
				hasAll(objID, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Person);
				bubble = ObjectListPrivate;

				if (!bubble->isEmpty()) {
					Common::String enough = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "GENUG_2");
					bubble->expandObjectList(enough);

					SetBubbleType(THINK_BUBBLE);

					choice1 = Bubble((NewList<NewNode>*)bubble, choice1, nullptr, 0);
					while (ChoiceOk(choice1, GET_OUT, bubble)) {
						objID = bubble->getNthNode(choice1)->_nr;
						tcDisplayInfoAboutPerson(objID);

						choice1 = Bubble((NewList<NewNode>*)bubble, choice1, nullptr, 0);
					}
				} else
					Say(BUSINESS_TXT, 0, MATT_PICTID, "NOBODY TO SEE");
			} else
				Say(BUSINESS_TXT, 0, MATT_PICTID, "NOBODY TO SEE");

			bubble->removeList();
			}
			break;
		default:
			choice = 2;
			break;
		}
	}
}

uint32 tcTelefon() {
	// CHECKME: Always returns 0

	uint32 eventNr = 0;
	PersonNode *ben = (PersonNode *)dbGetObject(Person_Ben_Riggley);
	gfxShow(175, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);
	knowsAll(Person_Matt_Stuvysunt, OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Person);

	if (!ObjectList->isEmpty()) {
		byte choice = 0;

		Common::String connect = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "DONT_CONNECT_ME");
		ObjectList->expandObjectList(connect);

		inpTurnESC(false);
		choice = Say(THECLOU_TXT, 0, MATT_PICTID, "CONNECT_ME");
		inpTurnESC(true);

		if (choice != GET_OUT) {
			choice = Bubble((NewList<NewNode>*)ObjectList, 0, nullptr, 0);
			if (ChoiceOk(choice, GET_OUT, ObjectList)) {
				uint32 persID = ObjectList->getNthNode((uint32) choice)->_nr;

				if (persID == Person_Ben_Riggley)
					Say(BUSINESS_TXT, 0, ben->PictID, "ALREADY_PHONING");

				if (livesIn(London_London_1, persID)) {
					if (g_clue->calcRandomNr(0, 9) == 3)
						Say(BUSINESS_TXT, 0, ben->PictID, "OCCUPIED");
					else {
						inpTurnESC(false);
						DynamicTalk(Person_Matt_Stuvysunt, persID, DLG_TALKMODE_STANDARD);
						inpTurnESC(true);
					}
				} else
					Say(BUSINESS_TXT, 0, ben->PictID, "NOBODY_AT_HOME");
			}
		}
	} else
		Say(THECLOU_TXT, 0, MATT_PICTID, "POOR_MATT");

	gfxShow(173, GFX_ONE_STEP | GFX_NO_REFRESH, 0, -1, -1);

	return (eventNr);
}

void tcWait() {
	uint32 locNr = GetObjNrOfLocation(_film->getLocation());

	inpTurnESC(false);

	ShowMenuBackground();
	ShowTime(0);
	inpSetWaitTicks(25);

	gfxSetRect(0, 320);
	_menuGc->setPens(249, 0, GFX_SAME_PEN);
	Common::String line = g_clue->_txtMgr->getFirstLine(THECLOU_TXT, "WAIT");
	_menuGc->gfxPrint(line, 22, GFX_PRINT_CENTER);

	bool endLoop = false;
	uint32 minutes = 0;
	while ((!endLoop) && (minutes < 960)) {
		minutes += 60;

		addVTime(60);
		ShowTime(0);

		if (locNr) {
			hasAll(locNr, OLF_NORMAL, Object_Person);

			uint32 newCount = ObjectList->getNrOfNodes();
			uint32 oldCount = 0;

			if ((newCount > oldCount) && (oldCount != 0)) {
				SetBubbleType(THINK_BUBBLE);
				Say(THECLOU_TXT, 0, MATT_PICTID, "NEW_PEOPLE");
				ShowTime(0);
			}

			oldCount = newCount;
		}

		uint32 action = inpWaitFor(INP_TIME | INP_LBUTTONP | INP_RBUTTONP | INP_ESC);

		if ((action & INP_LBUTTONP) || (action & INP_RBUTTONP) || (action & INP_ESC))
			endLoop = true;
	}

	if (minutes >= 960) {
		SetBubbleType(THINK_BUBBLE);
		Say(THECLOU_TXT, 0, MATT_PICTID, "WURZEL_SEPP");
	}

	inpSetWaitTicks(0);
	ShowMenuBackground();
	tcRefreshLocationInTitle(_film->getLocation());

	inpTurnESC(true);
	ShowTime(0);
}

} // End of namespace Clue

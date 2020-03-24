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
#include "clue/clue.h"

namespace Clue {

uint16 CurrentBackground = BGD_LONDON;


/**************************************************
 * a little static func for presentation of data
 */

static byte tcDisplayInfoAboutPerson(uint32 objID) {
	if (has(Person_Matt_Stuvysunt, objID))  /* hat Matt Wissen über ihn? */
		return Present(objID, "Person", InitPersonPresent);

	Common::String name = dbGetObjectName(objID);
	List *bubble = g_clue->_txtMgr->goKey(LOOK_TXT, name.c_str());

	SetBubbleType(THINK_BUBBLE);
	Bubble(bubble, 0, nullptr, 0L);
	RemoveList(bubble);

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

uint32 Go(List *succ) {
	uint32 succ_eventnr;
	inpTurnFunctionKey(0);

	if (GetNrOfNodes(succ) > 1) {
		uint32 prob = 0L;

		ShowMenuBackground();

		Common::String line = g_clue->_txtMgr->getFirstLine(THECLOU_TXT, "Gehen");
		PrintStatus(line);

		for (TCEventNode *node = (TCEventNode *) LIST_HEAD(succ); NODE_SUCC(node);
		        node = (TCEventNode *) NODE_SUCC(node)) {
			Scene *sc = GetScene(node->EventNr);
			Node *location = (Node *)GetNthNode(film->loc_names, sc->LocationNr);

			NODE_NAME(node) = NODE_NAME(location);

			prob++;
		}

		prob = (1 << prob) - 1;
		prob = Menu(succ, prob, 0, nullptr, 0L);

		succ_eventnr = ((TCEventNode *) GetNthNode(succ, prob))->EventNr;

		/* jetzt die Zuweisung wieder entfernen, damit der Name */
		/* nicht 2 mal freigegeben wird */

		for (TCEventNode *node = (TCEventNode *) LIST_HEAD(succ); NODE_SUCC(node);
		        node = (TCEventNode *) NODE_SUCC(node))
			NODE_NAME(node) = NULL;
	} else {
		succ_eventnr = ((TCEventNode *) GetNthNode(succ, 0L))->EventNr;
	}

	inpTurnFunctionKey(1);

	return succ_eventnr;
}


void Information() {
	inpTurnESC(true);

	byte choice = 0;
	byte ret = 0;
	while ((choice != 6) && (ret != GET_OUT)) {
		SetBubbleType(THINK_BUBBLE);
		List *bubble = g_clue->_txtMgr->goKey(THECLOU_TXT, "INFORMATION");
		choice = Bubble(bubble, choice, 0L, 0L);
		RemoveList(bubble);

		byte choice1 = 0;

		switch (choice) {
		case 0:
			if ((Present(Player_Player_1, "Player", InitPlayerPresent)) !=
			        GET_OUT)
				Present(Person_Matt_Stuvysunt, "Person", InitPersonPresent);
			break;

		case 1: {
			hasAll(Person_Matt_Stuvysunt,
			       OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR,
			       Object_Car);
			List *list = ObjectListPrivate;

			if (!(LIST_EMPTY(list))) {
				Common::String enough =  g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "GENUG");
				ExpandObjectList(list, enough);
				SetBubbleType(THINK_BUBBLE);

				while (ChoiceOk((choice1 = Bubble(list, choice1, 0L, 0L)), GET_OUT, list)) {
					SetBubbleType(THINK_BUBBLE);

					uint32 objID = (uint32)(((ObjectNode *) GetNthNode(list, (uint32) choice1))->nr);
					ret = Present(objID, "Car", InitCarPresent);
				}
			} else
				Say(BUSINESS_TXT, 0, MATT_PICTID, "POOR");

			RemoveList(list);
			}
			break;

		case 2: {
			knowsAll(Person_Matt_Stuvysunt,
			         OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR,
			         Object_Person);
			List *list = ObjectListPrivate;

			if (!(LIST_EMPTY(list))) {
				Common::String enough = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "GENUG");
				ExpandObjectList(list, enough);
				SetBubbleType(THINK_BUBBLE);

				while (ChoiceOk((choice1 = Bubble(list, choice1, 0L, 0L)), GET_OUT, list)) {
					SetBubbleType(THINK_BUBBLE);

					uint32 objID = (uint32)(((ObjectNode *) GetNthNode(list, (uint32) choice1))->nr);
					ret = tcDisplayInfoAboutPerson(objID);
				}
			} else
				Say(BUSINESS_TXT, 0, MATT_PICTID, "POOR");

			RemoveList(list);
			}
			break;

		case 3: {
			hasAll(Person_Matt_Stuvysunt,
			       OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR,
			       Object_Tool);
			List *list = ObjectListPrivate;

			if (!(LIST_EMPTY(list))) {
				SetBubbleType(THINK_BUBBLE);
				Common::String enough = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "GENUG");
				ExpandObjectList(list, enough);

				while (ChoiceOk((choice1 = Bubble(list, choice1, 0L, 0L)), GET_OUT, list)) {
					SetBubbleType(THINK_BUBBLE);

					uint32 objID = (uint32)(((ObjectNode *) GetNthNode(list, (uint32) choice1))->nr);
					ret = Present(objID, "Tool", InitToolPresent);
				}
			} else
				Say(BUSINESS_TXT, 0, MATT_PICTID, "POOR");

			RemoveList(list);
			}
			break;

		case 4: {
			hasAll(Person_Matt_Stuvysunt,
			       OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR,
			       Object_Building);
			List *list = ObjectListPrivate;

			if (!(LIST_EMPTY(list))) {
				SetBubbleType(THINK_BUBBLE);
				Common::String enough = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "GENUG");
				ExpandObjectList(list, enough);

				while (ChoiceOk((choice1 = Bubble(list, choice1, 0L, 0L)), GET_OUT, list)) {
					SetBubbleType(THINK_BUBBLE);

					uint32 objID = (uint32)(((ObjectNode *)GetNthNode(list, (uint32) choice1))->nr);
					ret = Present(objID, "Building", InitBuildingPresent);
				}
			} else
				Say(BUSINESS_TXT, 0, MATT_PICTID, "POOR");

			RemoveList(list);
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
	inpTurnESC(1);

	byte choice = 0;
	while (choice != 2) {
		List *menu = g_clue->_txtMgr->goKey(THECLOU_TXT, "UMSEHEN");

		SetBubbleType(THINK_BUBBLE);
		choice = Bubble(menu, 0, 0L, 0L);
		RemoveList(menu);

		byte choice1 = 0;

		switch (choice) {
		case 0: {
			List *bubble = g_clue->_txtMgr->goKey(HOUSEDESC_TXT,
			                  NODE_NAME(GetNthNode(film->loc_names, (locNr))));

			SetBubbleType(THINK_BUBBLE);
			choice1 = Bubble(bubble, 0, 0L, 0L);
			RemoveList(bubble);
			}
			break;

		case 1: {
			uint32 objID = GetObjNrOfLocation(locNr);
			List *bubble = nullptr;
			if (objID) {
				hasAll(objID, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Person);
				bubble = ObjectListPrivate;

				if (!(LIST_EMPTY(bubble))) {
					Common::String enough = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "GENUG_2");
					ExpandObjectList(bubble, enough);

					SetBubbleType(THINK_BUBBLE);

					while (ChoiceOk((choice1 = Bubble(bubble, choice1, 0L, 0L)), GET_OUT, bubble)) {
						objID = ((ObjectNode *) GetNthNode(bubble, (uint32) choice1))->nr;
						tcDisplayInfoAboutPerson(objID);
					}
				} else
					Say(BUSINESS_TXT, 0, MATT_PICTID, "NOBODY TO SEE");
			} else
				Say(BUSINESS_TXT, 0, MATT_PICTID, "NOBODY TO SEE");

			RemoveList(bubble);
			}
			break;
		default:
			choice = 2;
			break;
		}
	}
}

uint32 tcTelefon() {
	// CHECME: Always returns 0

	uint32 eventNr = 0L;
	Person ben = (Person)dbGetObject(Person_Ben_Riggley);
	gfxShow(175, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);
	knowsAll(Person_Matt_Stuvysunt, OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Person);

	if (!(LIST_EMPTY(ObjectList))) {
		byte choice = 0;

		Common::String connect = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "DONT_CONNECT_ME");
		ExpandObjectList(ObjectList, connect);

		inpTurnESC(0);
		choice = Say(THECLOU_TXT, 0, MATT_PICTID, "CONNECT_ME");
		inpTurnESC(1);

		if (choice != GET_OUT) {
			if (ChoiceOk((choice = Bubble(ObjectList, 0, 0L, 0L)), GET_OUT, ObjectList)) {
				uint32 persID = (uint32)(((ObjectNode *) GetNthNode(ObjectList, (uint32) choice))->nr);

				if (persID == Person_Ben_Riggley)
					Say(BUSINESS_TXT, 0, ben->PictID, "ALREADY_PHONING");

				if (livesIn(London_London_1, persID)) {
					if (g_clue->calcRandomNr(0L, 9L) == 3)
						Say(BUSINESS_TXT, 0, ben->PictID, "OCCUPIED");
					else {
						inpTurnESC(0);
						DynamicTalk(Person_Matt_Stuvysunt, persID,
						            DLG_TALKMODE_STANDARD);
						inpTurnESC(1);
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
	uint32 locNr = GetObjNrOfLocation(GetLocation);

	inpTurnESC(0);

	ShowMenuBackground();
	ShowTime(0);
	inpSetWaitTicks(25L);

	gfxSetRect(0, 320);
	gfxSetPens(m_gc, 249, 0, GFX_SAME_PEN);
	Common::String line = g_clue->_txtMgr->getFirstLine(THECLOU_TXT, "WAIT");
	gfxPrint(m_gc, line, 22, GFX_PRINT_CENTER);

	byte ende = 0;
	uint32 minutes = 0L;
	while ((!ende) && (minutes < 960)) {
		minutes += 60;

		AddVTime(60L);
		ShowTime(0);

		if (locNr) {
			hasAll(locNr, OLF_NORMAL, Object_Person);

			uint32 newCount = GetNrOfNodes(ObjectList);
			uint32 oldCount = 0;

			if ((newCount > oldCount) && (oldCount != 0)) {
				SetBubbleType(THINK_BUBBLE);
				Say(THECLOU_TXT, 0, MATT_PICTID, "NEW_PEOPLE");
				ShowTime(0);
			}

			oldCount = newCount;
		}

		uint32 action = inpWaitFor(INP_TIME | INP_LBUTTONP | INP_RBUTTONP | INP_ESC);

		if ((action & INP_LBUTTONP) || (action & INP_RBUTTONP) ||
		        (action & INP_ESC))
			ende = 1;
	}

	if (minutes >= 960) {
		SetBubbleType(THINK_BUBBLE);
		Say(THECLOU_TXT, 0, MATT_PICTID, "WURZEL_SEPP");
	}

	inpSetWaitTicks(0L);
	ShowMenuBackground();
	tcRefreshLocationInTitle(GetLocation);

	inpTurnESC(1);
	ShowTime(0);
}

} // End of namespace Clue

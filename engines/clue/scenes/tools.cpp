/*
**  $Filename: scenes/evidence.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-02-94
**
**   functions for evidence for "Der Clou!"
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

Common::String tcShowPriceOfTool(uint32 nr, uint32 type, void *data) {
	Tool tool = (Tool) data;

	Common::String line1 = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "PRICE_AND_MONEY");
	Common::String line = Common::String::format(line1.c_str(), tool->Value);

	return line;
}

byte tcBuyTool(byte choice) {
	Person mary = (Person)dbGetObject(Person_Mary_Bolton);

	ObjectListSuccString = tcShowPriceOfTool;
	ObjectListWidth = 48;

	hasAll(Person_Mary_Bolton,
	       OLF_ALIGNED | OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR |
	       OLF_ADD_SUCC_STRING, Object_Tool);
	List *tools = ObjectListPrivate;

	ObjectListSuccString = NULL;
	ObjectListWidth = 0;

	Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "THANKS");
	ExpandObjectList(tools, exp);

	SetBubbleType(THINK_BUBBLE);

	uint32 count = GetNrOfNodes(tools) - 1;

	choice = MIN((uint32)choice, count);
	uint8 oldChoice = GET_OUT;
	while (choice != GET_OUT) {
		SetPictID(MATT_PICTID);

		oldChoice = choice;

		if (ChoiceOk(choice = Bubble(tools, choice, 0L, 0L), GET_OUT, tools)) {
			Node *node = (Node *)GetNthNode(tools, (uint32) choice);
			uint32 toolID = OL_NR(node);
			Tool tool = (Tool) dbGetObject(toolID);
			uint32 price = tcGetToolPrice(tool);

			if (has(Person_Matt_Stuvysunt, toolID))
				Say(BUSINESS_TXT, 0, mary->PictID, "AUSVERKAUFT");
			else {
				if (tcSpendMoney(price, 0)) {
					hasSet(Person_Matt_Stuvysunt, toolID);
					Say(BUSINESS_TXT, 0, mary->PictID, "GOOD TOOL");
				}
			}
		} else
			choice = GET_OUT;
	}

	RemoveList(tools);

	return (oldChoice);
}

byte tcDescTool(byte choice) {
	uint8 oldChoice = GET_OUT;
	Person mary = (Person) dbGetObject(Person_Mary_Bolton);

	ObjectListSuccString = tcShowPriceOfTool;
	ObjectListWidth = 48;

	hasAll(Person_Mary_Bolton,
	       OLF_ALIGNED | OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR |
	       OLF_ADD_SUCC_STRING, Object_Tool);
	List *tools = ObjectListPrivate;

	ObjectListWidth = 0;
	ObjectListSuccString = nullptr;

	Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "THANKS");
	ExpandObjectList(tools, exp);

	choice = MIN((uint32)choice, (GetNrOfNodes(tools) - 1));

	while (choice != GET_OUT) {
		SetPictID(MATT_PICTID);

		oldChoice = choice;

		if (ChoiceOk(choice = Bubble(tools, choice, 0L, 0L), GET_OUT, tools)) {
			Common::String line = dbGetObjectName(OL_NR(GetNthNode(tools, (uint32) choice)));
			List* desc = g_clue->_txtMgr->goKey(TOOLS_TXT, line.c_str());

			SetPictID(mary->PictID);
			Bubble(desc, 0, nullptr, 0L);

			RemoveList(desc);
		} else
			choice = GET_OUT;
	}

	RemoveList(tools);

	return (oldChoice);
}

byte tcShowTool(byte choice) {
	uint8 oldChoice = GET_OUT;

	ObjectListSuccString = tcShowPriceOfTool;
	ObjectListWidth = 48;

	hasAll(Person_Mary_Bolton,
	       OLF_ALIGNED | OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR |
	       OLF_ADD_SUCC_STRING, Object_Tool);
	List *tools = ObjectListPrivate;

	ObjectListSuccString = NULL;
	ObjectListWidth = 0;

	Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "THANKS");
	ExpandObjectList(tools, exp);

	choice = MIN((uint32)choice, (GetNrOfNodes(tools) - 1));

	while (choice != GET_OUT) {
		SetPictID(MATT_PICTID);

		oldChoice = choice;

		if (ChoiceOk(choice = Bubble(tools, choice, 0L, 0L), GET_OUT, tools)) {
			Node *node = (Node *)GetNthNode(tools, (uint32) choice);
			uint32 toolID = OL_NR(node);

			Present(toolID, "Tool", InitToolPresent);
		} else
			choice = GET_OUT;
	}

	RemoveList(tools);

	return (oldChoice);
}

void tcSellTool() {
	Person mary = (Person) dbGetObject(Person_Mary_Bolton);

	hasAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Tool);
	List *tools = ObjectListPrivate;

	dbRemObjectNode(tools, Tool_Hand);
	dbRemObjectNode(tools, Tool_Fusz);

	if (LIST_EMPTY(tools))
		Say(BUSINESS_TXT, 0, MATT_PICTID, "MATT_HAS_NO_TOOL");

	byte choice = 0;
	while ((choice != GET_OUT) && (!LIST_EMPTY(tools))) {
		Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "NO_CHOICE");
		ExpandObjectList(tools, exp);

		SetPictID(MATT_PICTID);
		if (ChoiceOk(choice = Bubble(tools, 0, NULL, 0L), GET_OUT, tools)) {
			byte choice2 = 0;
			Node *node = (Node *)GetNthNode(tools, (uint32) choice);
			uint32 toolID = OL_NR(node);

			Tool tool = (Tool) dbGetObject(toolID);
			uint32 price = tcGetToolTraderOffer(tool);

			List *bubble = g_clue->_txtMgr->goKeyAndInsert(BUSINESS_TXT, "ANGEBOT_WERKZ", price);

			SetPictID(mary->PictID);
			Bubble(bubble, 0, 0L, 0L);
			RemoveList(bubble);

			bubble = g_clue->_txtMgr->goKey(BUSINESS_TXT, "VERKAUF");

			if (ChoiceOk(choice2 = Bubble(bubble, choice2, 0L, 0L), GET_OUT, bubble)) {
				if (choice2 == 0) {
					tcAddPlayerMoney(price);
					hasSet(Person_Mary_Bolton, toolID);
					hasUnSet(Person_Matt_Stuvysunt, toolID);
				}
			}

			RemoveList(bubble);
		} else
			choice = GET_OUT;

		RemoveList(tools);

		hasAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Tool);
		tools = ObjectListPrivate;

		dbRemObjectNode(tools, Tool_Hand);
		dbRemObjectNode(tools, Tool_Fusz);
	}

	RemoveList(tools);
}

void tcToolsShop() {
	static byte choice1 = 0;

	if (!(knows(Person_Matt_Stuvysunt, Person_Mary_Bolton)))
		knowsSet(Person_Matt_Stuvysunt, Person_Mary_Bolton);

	byte choice = 0;
	while (choice != 4) {
		choice = Say(BUSINESS_TXT, choice, MATT_PICTID, "Tools Shop");

		switch (choice) {
		case 0:
			choice1 = tcBuyTool(choice1);
			break;
		case 1:
			tcSellTool();
			break;
		case 2:
			choice1 = tcShowTool(choice1);
			break;
		case 3:
			choice1 = tcDescTool(choice1);
			break;
		default:
			choice = 4;
			break;
		}

		if (choice1 == GET_OUT)
			choice1 = 0;
	}
}

} // End of namespace Clue

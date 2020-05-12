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
#include "clue/present/interac.h"
#include "clue/data/dataappl.h"


namespace Clue {

Common::String tcShowPriceOfTool(uint32 nr, uint32 type, dbObjectNode *node) {
	ToolNode *tool = (ToolNode *) node;

	Common::String line1 = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "PRICE_AND_MONEY");
	Common::String line = Common::String::format(line1.c_str(), tool->Value);

	return line;
}

byte tcBuyTool(byte choice) {
	PersonNode *mary = (PersonNode *)dbGetObject(Person_Mary_Bolton);

	ObjectListSuccString = tcShowPriceOfTool;
	ObjectListWidth = 48;

	hasAll(Person_Mary_Bolton,
	       OLF_ALIGNED | OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR |
	       OLF_ADD_SUCC_STRING, Object_Tool);
	NewObjectList<dbObjectNode> *tools = ObjectListPrivate;

	ObjectListSuccString = nullptr;
	ObjectListWidth = 0;

	Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "THANKS");
	tools->expandObjectList(exp);

	SetBubbleType(THINK_BUBBLE);

	uint32 count = tools->getNrOfNodes() - 1;

	choice = MIN((uint32)choice, count);
	uint8 oldChoice = GET_OUT;
	while (choice != GET_OUT) {
		SetPictID(MATT_PICTID);

		oldChoice = choice;

		choice = Bubble((NewList<NewNode>*)tools, choice, nullptr, 0);
		if (ChoiceOk(choice, GET_OUT, tools)) {
			dbObjectNode *node = tools->getNthNode((uint32) choice);
			uint32 toolID = node->_nr;
			ToolNode *tool = (ToolNode *) dbGetObject(toolID);
			uint32 price = tcGetToolPrice(tool);

			if (has(Person_Matt_Stuvysunt, toolID))
				Say(BUSINESS_TXT, 0, mary->PictID, "AUSVERKAUFT");
			else {
				if (tcSpendMoney(price, false)) {
					hasSet(Person_Matt_Stuvysunt, toolID);
					Say(BUSINESS_TXT, 0, mary->PictID, "GOOD TOOL");
				}
			}
		} else
			choice = GET_OUT;
	}

	tools->removeList();
	return oldChoice;
}

byte tcDescTool(byte choice) {
	uint8 oldChoice = GET_OUT;
	PersonNode *mary = (PersonNode *) dbGetObject(Person_Mary_Bolton);

	ObjectListSuccString = tcShowPriceOfTool;
	ObjectListWidth = 48;

	hasAll(Person_Mary_Bolton,
	       OLF_ALIGNED | OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR |
	       OLF_ADD_SUCC_STRING, Object_Tool);
	NewObjectList<dbObjectNode> *tools = ObjectListPrivate;

	ObjectListWidth = 0;
	ObjectListSuccString = nullptr;

	Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "THANKS");
	tools->expandObjectList(exp);

	choice = MIN((uint32)choice, tools->getNrOfNodes() - 1);

	while (choice != GET_OUT) {
		SetPictID(MATT_PICTID);

		oldChoice = choice;
		choice = Bubble((NewList<NewNode>*)tools, choice, nullptr, 0);

		if (ChoiceOk(choice, GET_OUT, tools)) {
			Common::String line = dbGetObjectName(tools->getNthNode((uint32) choice)->_nr);
			NewList<NewNode> *desc = g_clue->_txtMgr->goKey(TOOLS_TXT, line.c_str());

			SetPictID(mary->PictID);
			Bubble(desc, 0, nullptr, 0);

			desc->removeList();
		} else
			choice = GET_OUT;
	}

	tools->removeList();
	return oldChoice;
}

byte tcShowTool(byte choice) {
	uint8 oldChoice = GET_OUT;

	ObjectListSuccString = tcShowPriceOfTool;
	ObjectListWidth = 48;

	hasAll(Person_Mary_Bolton,
	       OLF_ALIGNED | OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR |
	       OLF_ADD_SUCC_STRING, Object_Tool);
	NewObjectList<dbObjectNode> *tools = ObjectListPrivate;

	ObjectListSuccString = nullptr;
	ObjectListWidth = 0;

	Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "THANKS");
	tools->expandObjectList(exp);

	choice = MIN((uint32)choice, tools->getNrOfNodes() - 1);

	while (choice != GET_OUT) {
		SetPictID(MATT_PICTID);

		oldChoice = choice;
		choice = Bubble((NewList<NewNode>*)tools, choice, nullptr, 0);
		if (ChoiceOk(choice, GET_OUT, tools)) {
			dbObjectNode *node = tools->getNthNode((uint32) choice);
			uint32 toolID = node->_nr;

			Present(toolID, "Tool", InitToolPresent);
		} else
			choice = GET_OUT;
	}

	tools->removeList();

	return oldChoice;
}

void tcSellTool() {
	PersonNode *mary = (PersonNode *) dbGetObject(Person_Mary_Bolton);

	hasAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Tool);
	NewObjectList<dbObjectNode> *tools = ObjectListPrivate;

	dbRemObjectNode(tools, Tool_Hand);
	dbRemObjectNode(tools, Tool_Foot);

	if (tools->isEmpty())
		Say(BUSINESS_TXT, 0, MATT_PICTID, "MATT_HAS_NO_TOOL");

	byte choice = 0;
	while ((choice != GET_OUT) && !tools->isEmpty()) {
		Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "NO_CHOICE");
		tools->expandObjectList(exp);

		SetPictID(MATT_PICTID);
		choice = Bubble((NewList<NewNode>*)tools, 0, nullptr, 0);
		if (ChoiceOk(choice, GET_OUT, tools)) {
			byte choice2 = 0;
			dbObjectNode *node = tools->getNthNode((uint32)choice);
			uint32 toolID = node->_nr;

			ToolNode *tool = (ToolNode *) dbGetObject(toolID);
			uint32 price = tcGetToolTraderOffer(tool);

			NewList<NewNode> *bubble = g_clue->_txtMgr->goKeyAndInsert(BUSINESS_TXT, "ANGEBOT_WERKZ", price);

			SetPictID(mary->PictID);
			Bubble(bubble, 0, nullptr, 0);
			bubble->removeList();

			bubble = g_clue->_txtMgr->goKey(BUSINESS_TXT, "VERKAUF");
			choice2 = Bubble(bubble, choice2, nullptr, 0);
			if (ChoiceOkHack(choice2, GET_OUT, bubble)) {
				if (choice2 == 0) {
					tcAddPlayerMoney(price);
					hasSet(Person_Mary_Bolton, toolID);
					hasUnSet(Person_Matt_Stuvysunt, toolID);
				}
			}

			bubble->removeList();
		} else
			choice = GET_OUT;

		tools->removeList();

		hasAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Tool);
		tools = ObjectListPrivate;

		dbRemObjectNode(tools, Tool_Hand);
		dbRemObjectNode(tools, Tool_Foot);
	}

	tools->removeList();
}

void tcToolsShop() {
	static byte choice1 = 0;

	if (!knows(Person_Matt_Stuvysunt, Person_Mary_Bolton))
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

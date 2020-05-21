/*
**  $Filename: scenes/done.c
**  $Release:
**  $Revision:
**  $Date:
**
**
**
**  (C) 1993, 1994 ...and avoid panic by, H. Gaberschek
**      All Rights Reserved
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "clue/scenes/scenes.h"
#include "clue/clue.h"
#include "clue/present/interac.h"
#include "clue/data/dataappl.h"


namespace Clue {

void DoneTaxi() {
	static byte i = 0;
	NewObjectList<dbObjectNode> *locs = new NewObjectList<dbObjectNode>;

	knowsSet(Person_Matt_Stuvysunt, Person_Dan_Stanford);
	taxiAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Location);

	for (dbObjectNode *n = ObjectList->getListHead(); n->_succ; n = (dbObjectNode *) n->_succ) {
		LocationNode *loc = (LocationNode *)n;
		uint32 locNr = loc->LocationNr;

		char name[TXT_KEY_LENGTH];
		sprintf(name, "*%s", _film->_locationNames->getNthNode(locNr)->_name.c_str());

		dbObjectNode *newNode = locs->createNode(name);
		newNode->_nr = locNr + 1;    /* because of ChoiceOk */
	}

	i = MIN((uint32)i, locs->getNrOfNodes() -1);

	Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "NO_CHOICE");
	locs->expandObjectList(exp);

	byte j = Bubble((NewList<NewNode>*)locs, i, nullptr, 0);
	if (ChoiceOk(j, GET_OUT, locs)) {
		i = j;

		uint32 locNr = locs->getNthNode(i)->_nr - 1;
		_sceneArgs._returnValue = getLocScene(locNr)->_eventNr;
	} else {
		Say(BUSINESS_TXT, 0, MATT_PICTID, "LOVELY_TAXI");

		_sceneArgs._returnValue = getLocScene(_film->getOldLocation())->_eventNr;
	}

	_sceneArgs._overwritten = true;
	_sceneArgs._options = 0;

	locs->removeList();

	gfxChangeColors(_lowerGc, 2, GFX_FADE_OUT, 0);
}

void DoneInsideHouse() {
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "Mainmenu");

	_sceneArgs._returnValue = 0;
	_sceneArgs._overwritten = true;

	ShowMenuBackground();

	uint32 buildingID = GetObjNrOfBuilding(_film->getLocation() + 1);
	consistsOfAll(buildingID, OLF_NORMAL, Object_LSArea);

	/* jetzt alle Stockwerke laden */
	uint32 areaID;
	for (dbObjectNode *node = ObjectList->getListHead(); node->_succ; node = (dbObjectNode *)node->_succ) {
		areaID = node->_nr;

		lsInitRelations(areaID);
		lsInitObjectDB(buildingID, areaID);
	}

	startsWithAll(buildingID, OLF_NORMAL, Object_LSArea);

	areaID = ObjectList->getListHead()->_nr;
	lsLoadGlobalData(buildingID, areaID);   /* Stechuhren und so... */
	/* muß nur einmal geladen werden.. */

	lsSetRelations(areaID);

	byte perc = ((BuildingNode *) dbGetObject(buildingID))->Exactlyness;
	lsShowRaster(areaID, perc);
	gfxShow(154, GFX_FADE_OUT | GFX_BLEND_UP, 5, -1, -1);

	tcRefreshLocationInTitle(_film->getLocation());

	byte activ = 0;
	while (!_sceneArgs._returnValue) {
		if (_sceneArgs._options) {
			inpTurnFunctionKey(false);
			inpTurnESC(false);
			activ = Menu(menu, _sceneArgs._options, activ, nullptr, 0);
			inpTurnFunctionKey(true);
			inpTurnESC(true);

			uint32 choice = (uint32) 1 << activ;

			switch (choice) {
			case GP_CHOICE_LOOK:
				tcInsideOfHouse(buildingID, areaID, perc);
				ShowMenuBackground();
				tcRefreshLocationInTitle(_film->getLocation());
				addVTime(19);
				ShowTime(0);
				break;
			case GP_CHOICE_GO:
				areaID = tcGoInsideOfHouse(buildingID);
				if (!areaID)
					_sceneArgs._returnValue = getCurrentScene()->_nextEvents->getListHead()->_eventNr;
				else {
					lsSetRelations(areaID);

					lsShowRaster(areaID, perc);
				}
				tcRefreshLocationInTitle(_film->getLocation());
				break;
			default:
				_sceneArgs._returnValue = StdHandle(choice);
				break;
			}
		}
	}

	consistsOfAll(buildingID, OLF_NORMAL, Object_LSArea);

	/* jetzt alle Stockwerke entfernen */
	for (dbObjectNode *node = ObjectList->getListHead(); node->_succ; node = (dbObjectNode *)node->_succ)
		lsDoneObjectDB(node->_nr);

	g_clue->_animMgr->stopAnim();
	gfxChangeColors(_lowerGc, 5, GFX_FADE_OUT, nullptr);

	menu->removeList();
}

void DoneTools() {
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "Mainmenu");

	_sceneArgs._overwritten = true;

	tcMoveAPerson(Person_Mary_Bolton, Location_Tools_Shop);
	livesInUnSet(London_London_1, Person_Mary_Bolton);

	byte activ = 0;
	while (!_sceneArgs._returnValue) {
		tcPersonIsHere();

		inpTurnFunctionKey(false);  /* dont save in tools shop */
		inpTurnESC(false);
		activ = Menu(menu, _sceneArgs._options, activ, nullptr, 0);
		inpTurnESC(true);

		uint32 choice = (uint32) 1 << (activ);

		if (choice == GP_CHOICE_BUSINESS_TALK) {
			tcToolsShop();
			addVTime(9);
			ShowTime(0);
		} else
			_sceneArgs._returnValue = StdHandle(choice);
	}

	inpTurnFunctionKey(true);

	livesInSet(London_London_1, Person_Mary_Bolton);

	menu->removeList();
	g_clue->_animMgr->stopAnim();
	gfxChangeColors(_lowerGc, 5, GFX_FADE_OUT, 0);
}

void DoneDealer() {
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "Mainmenu");

	_sceneArgs._overwritten = true;
	_sceneArgs._returnValue = 0;

	tcMoveAPerson(Person_Frank_Maloya, Location_Maloya);
	tcMoveAPerson(Person_Eric_Pooly, Location_Pooly);
	tcMoveAPerson(Person_Helen_Parker, Location_Parker);

	livesInUnSet(London_London_1, Person_Frank_Maloya); /* damit sie sich */
	livesInUnSet(London_London_1, Person_Eric_Pooly);   /* nicht bewegen! */
	livesInUnSet(London_London_1, Person_Helen_Parker);

	byte activ = 0;
	while (!_sceneArgs._returnValue) {
		inpTurnFunctionKey(false);  /* or call save functions in case of space */
		inpTurnESC(false);
		activ = Menu(menu, _sceneArgs._options, activ, nullptr, 0);
		inpTurnESC(true);
		inpTurnFunctionKey(true);

		uint32 choice = (uint32) 1 << (activ);

		if (choice == GP_CHOICE_BUSINESS_TALK)
			tcDealerDlg();
		else
			_sceneArgs._returnValue = StdHandle(choice);
	}

	livesInSet(London_London_1, Person_Frank_Maloya);
	livesInSet(London_London_1, Person_Eric_Pooly);
	livesInSet(London_London_1, Person_Helen_Parker);

	menu->removeList();
	g_clue->_animMgr->stopAnim();
	gfxChangeColors(_lowerGc, 5, GFX_FADE_OUT, 0);
}

void DoneParking() {
	NewList<NewNode> *bubble = g_clue->_txtMgr->goKey(BUSINESS_TXT, "PARKING");
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "Mainmenu");
	PersonNode *marc = (PersonNode *)dbGetObject(Person_Marc_Smith);

	_sceneArgs._overwritten = true;
	_sceneArgs._returnValue = 0;

	byte activ = 0;
	while (!_sceneArgs._returnValue) {
		inpTurnFunctionKey(false);  /* or call save functions in case of space */
		inpTurnESC(false);

		activ = Menu(menu, _sceneArgs._options, (byte)activ, nullptr, 0);

		inpTurnESC(true);
		inpTurnFunctionKey(true);

		if ((1 << activ) == GP_CHOICE_BUSINESS_TALK) {
			byte choice = 0;

			while (choice != 2) {
				choice = Bubble(bubble, 0, nullptr, 0);

				switch (choice) {
				case 0:
					tcBuyCar();
					addVTime(9);
					choice = 2;
					break;
				case 1: {
					uint32 carID = tcChooseCar(27);
					if (carID) {
						tcSellCar(carID);
						addVTime(11);
					} else {
						Say(BUSINESS_TXT, 0, marc->PictID, "SELL_HERE");
						choice = 2;
					}
					}
					break;
				default:
					choice = 2;
					break;
				}
			}
			ShowTime(0);
		} else {
			inpTurnESC(false);
			_sceneArgs._returnValue = StdHandle(1 << activ);
			inpTurnESC(true);
		}
	}

	bubble->removeList();
	menu->removeList();

	g_clue->_animMgr->stopAnim();
	gfxChangeColors(_lowerGc, 5, GFX_FADE_OUT, nullptr);
}

void DoneGarage() {
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "Mainmenu");
	byte activ = 0;
	PersonNode *marc = (PersonNode *)dbGetObject(Person_Marc_Smith);

	_sceneArgs._overwritten = true;
	_sceneArgs._returnValue = 0;

	while (!_sceneArgs._returnValue) {
		inpTurnFunctionKey(false);  /* or call save functions in case of space */
		inpTurnESC(false);

		activ = Menu(menu, _sceneArgs._options, activ, nullptr, 0);

		inpTurnESC(true);
		inpTurnFunctionKey(true);

		uint32 choice = (uint32) 1 << activ;

		if (choice == GP_CHOICE_BUSINESS_TALK) {
			uint32 carID = tcChooseCar(26);
			if (carID) {
				tcCarInGarage(carID);
				addVTime(9);
				ShowTime(0);
			} else {
				Say(BUSINESS_TXT, 0, marc->PictID, "REPAIR_HERE");
				inpTurnESC(false);
				_sceneArgs._returnValue = StdHandle(GP_CHOICE_GO);
				inpTurnESC(true);
			}
		} else {
			inpTurnESC(false);
			_sceneArgs._returnValue = StdHandle(choice);
			inpTurnESC(true);
		}
	}

	menu->removeList();
	g_clue->_animMgr->stopAnim();
	gfxChangeColors(_lowerGc, 5, GFX_FADE_OUT, 0);
}

void tcInitFahndung() {
	PlayerNode *player = (PlayerNode *)dbGetObject(Player_Player_1);

	tcMattGoesTo(59);       /* Büro */

	player->NrOfBurglaries++;
}

void tcDoneFahndung() {
	if (tcStartEvidence()) {
		tcDonePrison();
		_sceneArgs._returnValue = SCENE_NEW_GAME;
	} else {
		switch (((PlayerNode *)dbGetObject(Player_Player_1))->NrOfBurglaries) {
		case 3:
			_sceneArgs._returnValue = SCENE_STATION;
			break;
		default:
			_sceneArgs._returnValue = SCENE_HOTEL_ROOM;
			break;
		}
	}

	if (tcIsDeadlock())
		_sceneArgs._returnValue = SCENE_NEW_GAME;

	gfxChangeColors(_lowerGc, 5, GFX_FADE_OUT, nullptr);
}

/*
 * ein schlimmer Murks, aber anders geht s nicht!
 * und, .... die ganze Scheiße nur wegen den Knochen
 */

void DoneHotelRoom() {
	tcCheckForBones();

	if (g_clue->getFeatures() & GF_PROFIDISK)
		tcCheckForDowning();

	stdDone();
}

} // End of namespace Clue

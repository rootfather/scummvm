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

namespace Clue {

void DoneTaxi() {
	static byte i = 0;
	NewObjectList<NewObjectNode> *locs = new NewObjectList<NewObjectNode>;

	knowsSet(Person_Matt_Stuvysunt, Person_Dan_Stanford);
	taxiAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Location);

	for (NewObjectNode *n = ObjectList->getListHead(); n->_succ; n = (NewObjectNode *) n->_succ) {
		Location loc = (Location)n->_data;
		uint32 locNr = loc->LocationNr;

		char name[TXT_KEY_LENGTH];
		sprintf(name, "*%s", film->loc_names->getNthNode(locNr)->_name.c_str());

		NewObjectNode *newNode = locs->createNode(name);
		newNode->_nr = locNr + 1;    /* because of ChoiceOk */
	}

	i = MIN((uint32)i, locs->getNrOfNodes() - 1);

	Common::String exp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "NO_CHOICE");
	locs->expandObjectList(exp);

	byte j = Bubble((NewList<NewNode>*)locs, i, nullptr, 0L);
	if (ChoiceOk(j, GET_OUT, locs)) {
		i = j;

		uint32 locNr = locs->getNthNode(i)->_nr - 1;
		_sceneArgs._returnValue = GetLocScene(locNr)->EventNr;
	} else {
		Say(BUSINESS_TXT, 0, MATT_PICTID, "LOVELY_TAXI");

		_sceneArgs._returnValue = GetLocScene(GetOldLocation)->EventNr;
	}

	_sceneArgs._overwritten = true;
	_sceneArgs._options = 0L;

	locs->removeList();

	gfxChangeColors(l_gc, 2L, GFX_FADE_OUT, 0L);
}

void DoneInsideHouse() {
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "Mainmenu");

	_sceneArgs._returnValue = 0;
	_sceneArgs._overwritten = true;

	ShowMenuBackground();

	uint32 buildingID = GetObjNrOfBuilding(GetLocation + 1);
	consistsOfAll(buildingID, OLF_NORMAL, Object_LSArea);

	/* jetzt alle Stockwerke laden */
	uint32 areaID;
	for (NewObjectNode *node = ObjectList->getListHead(); node->_succ; node = (NewObjectNode *)node->_succ) {
		areaID = node->_nr;

		lsInitRelations(areaID);
		lsInitObjectDB(buildingID, areaID);
	}

	startsWithAll(buildingID, OLF_NORMAL, Object_LSArea);

	areaID = ObjectList->getListHead()->_nr;
	lsLoadGlobalData(buildingID, areaID);   /* Stechuhren und so... */
	/* muß nur einmal geladen werden.. */

	lsSetRelations(areaID);

	byte perc = ((Building) dbGetObject(buildingID))->Exactlyness;
	lsShowRaster(areaID, perc);
	gfxShow(154, GFX_FADE_OUT | GFX_BLEND_UP, 5, -1, -1);

	tcRefreshLocationInTitle(GetLocation);

	byte activ = 0;
	while (!_sceneArgs._returnValue) {
		if (_sceneArgs._options) {
			inpTurnFunctionKey(0);
			inpTurnESC(0);
			activ = Menu(menu, _sceneArgs._options, activ, NULL, 0L);
			inpTurnFunctionKey(1);
			inpTurnESC(1);

			uint32 choice = (uint32) 1L << activ;

			switch (choice) {
			case LOOK:
				tcInsideOfHouse(buildingID, areaID, perc);
				ShowMenuBackground();
				tcRefreshLocationInTitle(GetLocation);
				AddVTime(19);
				ShowTime(0);
				break;
			case GO:
				if (!(areaID = tcGoInsideOfHouse(buildingID)))
					_sceneArgs._returnValue = GetCurrentScene()->std_succ->getListHead()->_eventNr;
				else {
					lsSetRelations(areaID);

					lsShowRaster(areaID, perc);
				}
				tcRefreshLocationInTitle(GetLocation);
				break;
			default:
				_sceneArgs._returnValue = StdHandle(choice);
				break;
			}
		}
	}

	consistsOfAll(buildingID, OLF_NORMAL, Object_LSArea);

	/* jetzt alle Stockwerke entfernen */
	for (NewObjectNode *node = ObjectList->getListHead(); node->_succ; node = (NewObjectNode *)node->_succ)
		lsDoneObjectDB(node->_nr);

	StopAnim();
	gfxChangeColors(l_gc, 5L, GFX_FADE_OUT, 0L);

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

		inpTurnFunctionKey(0);  /* dont save in tools shop */
		inpTurnESC(0);
		activ = Menu(menu, _sceneArgs._options, (byte)(activ), NULL, 0L);
		inpTurnESC(1);

		uint32 choice = (uint32) 1L << (activ);

		if (choice == BUSINESS_TALK) {
			tcToolsShop();
			AddVTime(9);
			ShowTime(0);
		} else
			_sceneArgs._returnValue = StdHandle(choice);
	}

	inpTurnFunctionKey(1);

	livesInSet(London_London_1, Person_Mary_Bolton);

	menu->removeList();
	StopAnim();
	gfxChangeColors(l_gc, 5L, GFX_FADE_OUT, 0L);
}

void DoneDealer() {
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "Mainmenu");

	_sceneArgs._overwritten = true;
	_sceneArgs._returnValue = 0L;

	tcMoveAPerson(Person_Frank_Maloya, Location_Maloya);
	tcMoveAPerson(Person_Eric_Pooly, Location_Pooly);
	tcMoveAPerson(Person_Helen_Parker, Location_Parker);

	livesInUnSet(London_London_1, Person_Frank_Maloya); /* damit sie sich */
	livesInUnSet(London_London_1, Person_Eric_Pooly);   /* nicht bewegen! */
	livesInUnSet(London_London_1, Person_Helen_Parker);

	byte activ = 0;
	while (!_sceneArgs._returnValue) {
		inpTurnFunctionKey(0);  /* or call save functions in case of space */
		inpTurnESC(0);
		activ = Menu(menu, _sceneArgs._options, (byte)(activ), NULL, 0L);
		inpTurnESC(1);
		inpTurnFunctionKey(1);

		uint32 choice = (uint32) 1L << (activ);

		if (choice == BUSINESS_TALK)
			tcDealerDlg();
		else
			_sceneArgs._returnValue = StdHandle(choice);
	}

	livesInSet(London_London_1, Person_Frank_Maloya);
	livesInSet(London_London_1, Person_Eric_Pooly);
	livesInSet(London_London_1, Person_Helen_Parker);

	menu->removeList();
	StopAnim();
	gfxChangeColors(l_gc, 5L, GFX_FADE_OUT, 0L);
}

void DoneParking() {
	NewList<NewNode> *bubble = g_clue->_txtMgr->goKey(BUSINESS_TXT, "PARKING");
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "Mainmenu");
	Person marc = (Person)dbGetObject(Person_Marc_Smith);

	_sceneArgs._overwritten = true;
	_sceneArgs._returnValue = 0;

	byte activ = 0;
	while (!_sceneArgs._returnValue) {
		inpTurnFunctionKey(0);  /* or call save functions in case of space */
		inpTurnESC(0);

		activ = Menu(menu, _sceneArgs._options, (byte)(activ), 0L, 0L);

		inpTurnESC(1);
		inpTurnFunctionKey(1);

		if ((1L << activ) == BUSINESS_TALK) {
			byte choice = 0;

			while (choice != 2) {
				choice = Bubble(bubble, 0, 0L, 0L);

				switch (choice) {
				case 0:
					tcBuyCar();
					AddVTime(9);
					choice = 2;
					break;
				case 1: {
					uint32 carID;
					if ((carID = tcChooseCar(27))) {
						tcSellCar(carID);
						AddVTime(11);
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
			inpTurnESC(0);
			_sceneArgs._returnValue = StdHandle(1L << activ);
			inpTurnESC(1);
		}
	}

	bubble->removeList();
	menu->removeList();

	StopAnim();
	gfxChangeColors(l_gc, 5L, GFX_FADE_OUT, 0L);
}

void DoneGarage() {
	NewList<NewNode> *menu = g_clue->_txtMgr->goKey(MENU_TXT, "Mainmenu");
	byte activ = 0;
	Person marc = (Person)dbGetObject(Person_Marc_Smith);

	_sceneArgs._overwritten = true;
	_sceneArgs._returnValue = 0;

	while (!_sceneArgs._returnValue) {
		inpTurnFunctionKey(0);  /* or call save functions in case of space */
		inpTurnESC(0);

		activ = Menu(menu, _sceneArgs._options, (byte)(activ), NULL, 0);

		inpTurnESC(1);
		inpTurnFunctionKey(1);

		uint32 choice = (uint32) 1L << (activ);

		if (choice == BUSINESS_TALK) {
			uint32 carID;
			if ((carID = tcChooseCar(26))) {
				tcCarInGarage(carID);
				AddVTime(9);
				ShowTime(0);
			} else {
				Say(BUSINESS_TXT, 0, marc->PictID, "REPAIR_HERE");
				inpTurnESC(0);
				_sceneArgs._returnValue = StdHandle(GO);
				inpTurnESC(1);
			}
		} else {
			inpTurnESC(0);
			_sceneArgs._returnValue = StdHandle(choice);
			inpTurnESC(1);
		}
	}

	menu->removeList();
	StopAnim();
	gfxChangeColors(l_gc, 5L, GFX_FADE_OUT, 0L);
}

void tcInitFahndung() {
	Player player = (Player)dbGetObject(Player_Player_1);

	tcMattGoesTo(59);       /* Büro */

	player->NrOfBurglaries++;
}

void tcDoneFahndung() {
	if (tcStartEvidence()) {
		tcDonePrison();
		_sceneArgs._returnValue = SCENE_NEW_GAME;
	} else {
		switch (((Player)(dbGetObject(Player_Player_1)))->NrOfBurglaries) {
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

	gfxChangeColors(l_gc, 5L, GFX_FADE_OUT, 0L);
}

/*
 * ein schlimmer Murks, aber anders geht s nicht!
 * und, .... die ganze Scheiße nur wegen den Knochen
 */

void DoneHotelRoom() {
	tcCheckForBones();

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		tcCheckForDowning();
	}

	StdDone();
}

} // End of namespace Clue

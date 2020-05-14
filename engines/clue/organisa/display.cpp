/*
**  $Filename: organisation/display.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-02-94
**
**  display functions for organisation of "Der Clou!"
**
**   (c) 1994 ...and avoid panic by, H. Gaberschek, K. Kazemi
**      All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "clue/data/dataappl.h"
#include "clue/organisa/organisa.h"
#include "clue/scenes/scenes.h"

namespace Clue {

#define   ORG_DISP_GUY_Y           60
#define   ORG_DISP_ABILITIES_Y     75
#define   ORG_DISP_GUY_WIDTH       80
#define   ORG_DISP_LINE            10

#define   ORG_PICT_ID                 3

static void RefreshDisplayConfig() {
	gfxScreenFreeze();

	gfxSetGC(l_gc);
	gfxShow(ORG_PICT_ID, GFX_NO_REFRESH | GFX_BLEND_UP, 0, -1, -1);

	gfxScreenThaw(l_gc, 0, 0, 320, 140);

	CurrentBackground = BGD_PLANUNG;
	ShowMenuBackground();
}

void tcInitDisplayOrganisation() {
	gfxChangeColors(l_gc, 5, GFX_FADE_OUT, 0);
	gfxClearArea(l_gc);

	RefreshDisplayConfig();
}

void tcDoneDisplayOrganisation() {
	gfxChangeColors(l_gc, 5, GFX_FADE_OUT, 0);
	gfxClearArea(l_gc);
	gfxSetPens(m_gc, GFX_SAME_PEN, GFX_SAME_PEN, 0);
}

void tcDisplayOrganisation() {
	gfxScreenFreeze();

	tcDisplayCommon();
	tcDisplayPerson(ORG_DISP_ABILITIES);

	gfxScreenThaw(l_gc, 0, 0, 320, 140);
}

void tcDisplayCommon() {
	NewList<NewNode> *texts = g_clue->_txtMgr->goKey(BUSINESS_TXT, "PLAN_COMMON_DATA");

	gfxSetGC(l_gc);
	gfxShow(ORG_PICT_ID, GFX_ONE_STEP | GFX_NO_REFRESH, 0, -1, -1);

	/* Gebäude anzeigen  */

	gfxSetFont(l_gc, menuFont);
	gfxSetDrMd(l_gc, GFX_JAM_1);
	gfxSetRect(0, 320);

	gfxSetPens(l_gc, 249, 254, GFX_SAME_PEN);

	Common::String line;
	BuildingNode *building = nullptr;
	if (Organisation.BuildingID) {
		building = (BuildingNode *)dbGetObject(Organisation.BuildingID);

		line = dbGetObjectName(Organisation.BuildingID);
		gfxPrint(l_gc, line, 9, GFX_PRINT_CENTER | GFX_PRINT_SHADOW);
	}


	gfxSetFont(l_gc, bubbleFont);
	gfxSetPens(l_gc, 249, GFX_SAME_PEN, GFX_SAME_PEN);

	/*************************************************************
	 *  Fluchtwagen anzeigen
	 */

	gfxSetRect(0, 106);
	line = texts->getNthNode(0L)->_name;

	Common::String name;
	if (Organisation.CarID) {
		name = tcCutName(dbGetObjectName(Organisation.CarID), (byte) ' ', 12);
		line += name;
	} else
		line += " ? ";

	gfxPrint(l_gc, line, 25, GFX_PRINT_LEFT);

	/*************************************************************
	 *  Zulassung anzeigen
	 */

	gfxSetRect(106, 106);
	line = texts->getNthNode(2L)->_name;

	if (Organisation.CarID) {
		name = Common::String::format("%d", Organisation.PlacesInCar);
		line += name;
	} else
		line += " ? ";

	gfxPrint(l_gc, line, 25, GFX_PRINT_CENTER);

	/*************************************************************
	 *  Fahrer anzeigen
	 */

	gfxSetRect(212, 106);
	line = texts->getNthNode(1L)->_name;

	if (Organisation.DriverID) {
		name = dbGetObjectName(Organisation.DriverID);
		line += name;
	} else
		line += " ? ";

	gfxPrint(l_gc, line, 25, GFX_PRINT_RIGHT);

	/*************************************************************
	 *  Fluchtweg
	 */

	gfxSetRect(0, 106);
	line = texts->getNthNode(3L)->_name;

	if (Organisation.BuildingID) {
		NewList<NewNode> *enums = g_clue->_txtMgr->goKey(OBJECTS_ENUM_TXT, "enum_RouteE");

		name = enums->getNthNode(building->EscapeRoute)->_name;
		line += name;

		enums->removeList();
	} else
		line += " ? ";

	gfxPrint(l_gc, line, 35, GFX_PRINT_LEFT);

	/*************************************************************
	 *  Fluchtweg-länge
	 */

	gfxSetRect(106, 106);
	line = texts->getNthNode(4L)->_name;

	if (Organisation.BuildingID) {
		name = Common::String::format("%d (km)", building->EscapeRouteLength);
		line += name;
	} else
		line += " ? ";

	gfxPrint(l_gc, line, 35, GFX_PRINT_CENTER);

	/*************************************************************
	 *  mein Anteil
	 */

	gfxSetRect(212, 106);
	line = texts->getNthNode(5L)->_name;

	name = Common::String::format(" %d%%", tcCalcMattsPart());
	line += name;

	gfxPrint(l_gc, line, 35, GFX_PRINT_RIGHT);

	texts->removeList();
}

void tcDisplayPerson(uint32 displayMode) {
	joined_byAll(Person_Matt_Stuvysunt, OLF_INCLUDE_NAME | OLF_PRIVATE_LIST, Object_Person);
	NewObjectList<dbObjectNode>* guys = ObjectListPrivate;
	dbSortObjectList(&guys, dbStdCompareObjects);

	uint32 i;
	dbObjectNode *node;
	for (node = guys->getListHead(), i = 0; node->_succ; node = (dbObjectNode *)node->_succ, i++) {
		char line[TXT_KEY_LENGTH];
		uint32 objNr = node->_nr;

		if (node->_name.size() >= 16)
			tcGetLastName(node->_name.c_str(), line, 15);
		else
			strcpy(line, node->_name.c_str());

		gfxSetRect(ORG_DISP_GUY_WIDTH * i + 5, ORG_DISP_GUY_WIDTH - 5);
		gfxSetPens(l_gc, 248, GFX_SAME_PEN, GFX_SAME_PEN);
		gfxSetFont(l_gc, menuFont);
		gfxSetDrMd(l_gc, GFX_JAM_1);
		gfxPrint(l_gc, line, ORG_DISP_GUY_Y, GFX_PRINT_CENTER);
		gfxSetFont(l_gc, bubbleFont);

		if (displayMode & ORG_DISP_ABILITIES)
			tcDisplayAbilities(objNr, i);
	}

	guys->removeList();
}

void tcDisplayAbilities(uint32 personNr, uint32 displayData) {
	hasAll(personNr, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME, Object_Ability);

	NewList<dbObjectNode>* abilities = ObjectListPrivate;

	prSetBarPrefs(l_gc, ORG_DISP_GUY_WIDTH - 5, ORG_DISP_LINE + 1, 251, 250, 249);

	if (!abilities->isEmpty()) {
		dbObjectNode *node;
		unsigned i;

		for (node = abilities->getListHead(), i = 0; node->_succ; node = (dbObjectNode *)node->_succ, i++) {
			uint32 abiNr = abilities->getNthNode(i)->_nr;
			uint32 ability = hasGet(personNr, abiNr);

			Common::String line = node->_name + Common::String::format(" %d%%", (uint16)((ability * 100) / 255));
			prDrawTextBar(line, ability, 255L,
			              displayData * ORG_DISP_GUY_WIDTH + 5,
			              ORG_DISP_ABILITIES_Y + i * (ORG_DISP_LINE + 4));
		}
	} else {
		Common::String line = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "PLAN_NO_CAPABILITY");
		gfxSetRect(ORG_DISP_GUY_WIDTH + 5, ORG_DISP_GUY_WIDTH - 5);
		gfxSetDrMd(l_gc, GFX_JAM_1);
		gfxPrint(l_gc, line, ORG_DISP_ABILITIES_Y + ORG_DISP_LINE, GFX_PRINT_LEFT);
	}

	abilities->removeList();
}

} // End of namespace Clue

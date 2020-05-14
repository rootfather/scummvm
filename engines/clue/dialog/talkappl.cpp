/*
**  $Filename: dialog/talkAppl.c
**  $Release:  0
**  $Revision: 0.1
**  $Date:     07-04-94
**
**  dialog functions for "Der Clou!"
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

#include "clue/dialog/talkappl.h"
#include "clue/present/interac.h"
#include "clue/scenes/scenes.h"


namespace Clue {

void tcJobOffer(PersonNode *p) {
	PlayerNode *player = (PlayerNode *)dbGetObject(Player_Player_1);

	if (!join(Person_Matt_Stuvysunt, p->_nr))
		player->JobOfferCount++;

	byte choice = Say(BUSINESS_TXT, 0, MATT_PICTID, "PERS_ANZ");
	int32 part = tcGetPersOffer(p, (choice + 2));
	NewList<NewNode> *bubble = g_clue->_txtMgr->goKeyAndInsert(BUSINESS_TXT, "JOB_ANSWER", (uint32) part, NULL);

	SetPictID(p->PictID);
	Bubble(bubble, 0, nullptr, 0);
	bubble->removeList();

	choice = Say(BUSINESS_TXT, 0, MATT_PICTID, "NEW_THEEF");

	if (choice == 0) {
		joinSet(Person_Matt_Stuvysunt, p->_nr);
		hasSet(Person_Matt_Stuvysunt, p->_nr);    /* Matt hat jetzt Daten von ihm */
		p->TalkBits &= (0xffffffffL - (1 << Const_tcTALK_JOB_OFFER));   /* Joboffer löschen! */
	} else {
		p->Known = CalcValue(p->Known, 0, 255, 0, 20);
		Say(BUSINESS_TXT, 0, p->PictID, "MY_OFFER");
	}
}

void tcMyJobAnswer(PersonNode *p) {
	NewList<NewNode> *bubble = new NewList<NewNode>;
	NewList<NewNode> *jobs = g_clue->_txtMgr->goKey(OBJECTS_ENUM_TXT, "enum_JobE");
	char job[TXT_KEY_LENGTH];

	Common::String line;
	strcpy(job, jobs->getNthNode(p->Job)->_name.c_str());

	if (strcmp(job, jobs->getNthNode(10)->_name.c_str())) {
		Common::String temp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "MY_JOB_IS");
		line = Common::String::format(temp.c_str(), job);
	} else
		line = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "NO_JOB");

	SetPictID(p->PictID);
	bubble->createNode(line);
	Bubble(bubble, 0, nullptr, 0);

	jobs->removeList();
	bubble->removeList();
}

void tcPrisonAnswer(PersonNode *p) {
	NewList<NewNode> *bubble = new NewList<NewNode>;
	NewList<NewNode> *source = g_clue->_txtMgr->goKey(BUSINESS_TXT, "IN_PRISON_ANSWER");
	char line[TXT_KEY_LENGTH];

	strcpy(line, source->getNthNode(p->KnownToPolice / 52)->_name.c_str());

	SetPictID(p->PictID);
	bubble->createNode(line);
	Bubble(bubble, 0, nullptr, 0);

	source->removeList();
	bubble->removeList();
}

void tcAbilityAnswer(uint32 personID) {
	PersonNode *p = (PersonNode *)dbGetObject(personID);

	Common::String name = dbGetObjectName(personID);
	NewList<NewNode> *bubble = g_clue->_txtMgr->goKey(ABILITY_TXT, name.c_str());
	SetPictID(p->PictID);
	Bubble(bubble, 0, nullptr, 0);

	bubble->removeList();
}

} // End of namespace Clue

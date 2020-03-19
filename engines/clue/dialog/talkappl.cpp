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

namespace Clue {

void tcJobOffer(Person p) {
	Player player = (Player)dbGetObject(Player_Player_1);

	if (!(join(Person_Matt_Stuvysunt, dbGetObjectNr(p))))
		player->JobOfferCount++;

	byte choice = Say(BUSINESS_TXT, 0, MATT_PICTID, "PERS_ANZ");
	int32 part = tcGetPersOffer(p, (choice + 2));
	LIST *bubble = g_clue->_txtMgr->goKeyAndInsert(BUSINESS_TXT, "JOB_ANSWER", (uint32) part, NULL);

	SetPictID(p->PictID);
	Bubble(bubble, 0, 0L, 0L);
	RemoveList(bubble);

	choice = Say(BUSINESS_TXT, 0, MATT_PICTID, "NEW_THEEF");

	if (choice == 0) {
		joinSet(Person_Matt_Stuvysunt, dbGetObjectNr(p));
		hasSet(Person_Matt_Stuvysunt, dbGetObjectNr(p));    /* Matt hat jetzt Daten von ihm */
		p->TalkBits &= (0xffffffffL - (1 << Const_tcTALK_JOB_OFFER));   /* Joboffer löschen! */
	} else {
		p->Known = CalcValue(p->Known, 0, 255, 0, 20);
		Say(BUSINESS_TXT, 0, p->PictID, "MY_OFFER");
	}
}

void tcMyJobAnswer(Person p) {
	LIST *bubble = CreateList();
	LIST *jobs = g_clue->_txtMgr->goKey(OBJECTS_ENUM_TXT, "enum_JobE");
	char job[TXT_KEY_LENGTH];

	Common::String line;
	strcpy(job, NODE_NAME(GetNthNode(jobs, p->Job)));

	if (strcmp(job, NODE_NAME(GetNthNode(jobs, 10)))) {
		Common::String temp = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "MY_JOB_IS");
		line = Common::String::format(temp.c_str(), job);
	} else
		line = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "NO_JOB");

	SetPictID(p->PictID);
	CreateNode(bubble, 0L, line);
	Bubble(bubble, 0, nullptr, 0L);

	RemoveList(jobs);
	RemoveList(bubble);
}

void tcPrisonAnswer(Person p) {
	LIST *bubble = CreateList();
	LIST *source = g_clue->_txtMgr->goKey(BUSINESS_TXT, "IN_PRISON_ANSWER");
	char line[TXT_KEY_LENGTH];

	strcpy(line, NODE_NAME(GetNthNode(source, p->KnownToPolice / 52)));

	SetPictID(p->PictID);
	CreateNode(bubble, 0L, line);
	Bubble(bubble, 0, 0, 0);

	RemoveList(source);
	RemoveList(bubble);
}

void tcAbilityAnswer(uint32 personID) {
	Person p = (Person)dbGetObject(personID);

	Common::String name = dbGetObjectName(personID);
	LIST *bubble = g_clue->_txtMgr->goKey(ABILITY_TXT, name.c_str());
	SetPictID(p->PictID);
	Bubble(bubble, 0, 0, 0);

	RemoveList(bubble);
}

} // End of namespace Clue

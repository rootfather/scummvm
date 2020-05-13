/*
**  $Filename: dialog/dialog.c
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
#include "clue/sound/newsound.h"
#include "clue/clue.h"
#include "clue/cdrom/cdrom.h"
#include "clue/present/interac.h"


namespace Clue {

#define DLG_NO_SPEECH   ((uint32) -1)
uint32 StartFrame = DLG_NO_SPEECH;
uint32 EndFrame = DLG_NO_SPEECH;

NewList<NewNode> *PrepareQuestions(NewList<NewDynDlgNode> *keyWords, uint32 talkBits, byte textID) {
	NewList<NewNode> *preparedList = new NewList<NewNode>;
	NewList<NewNode> *stdQuestionList = g_clue->_txtMgr->goKey(BUSINESS_TXT, "STD_QUEST");
	NewList<NewNode> *questionList = g_clue->_txtMgr->goKey((uint32) textID, "QUESTIONS");

	Common::String question;
	for (NewNode *n = keyWords->getListHead(); n->_succ; n = n->_succ) {
		byte r = (byte)g_clue->calcRandomNr(0, 6);
		question = Common::String::format(questionList->getNthNode(r)->_name.c_str(), n->_name.c_str());
		preparedList->createNode(question);
	}

	for (byte i = 0; i < 32; i++) {
		if (talkBits & (1 << i)) {
			question = Common::String(stdQuestionList->getNthNode(i)->_name);
			preparedList->createNode(question);
		}
	}

	question = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "Bye_says_Matt");
	preparedList->createNode(question);

	stdQuestionList->removeList();
	questionList->removeList();

	return preparedList;
}

static NewList<NewDynDlgNode> *ParseTalkText(NewList<NewNode> *origin, NewList<NewNode> *bubble, byte known) {
	NewList<NewDynDlgNode> *keyWords = new NewList<NewDynDlgNode>;

	for (NewNode *n = origin->getListHead(); n->_succ; n = n->_succ) {
		char line[TXT_KEY_LENGTH], key[TXT_KEY_LENGTH], keyWord[TXT_KEY_LENGTH];
		byte line_pos = 0;
		byte key_pos = 0;

		const char* start = n->_name.c_str();
		const char *mem = start;

		while (mem < start + strlen(start)) {
			if (*mem != '[')
				line[line_pos++] = *(mem++);
			else {
				mem++;      /* Klammer überspringen ! */
				key_pos = 0;

				while ((*mem) != ']')
					key[key_pos++] = *(mem++);

				key[key_pos++] = EOS;

				mem++;      /* Klammer überspringen ! */

				char snr[10], snr1[10];
				for (uint32 i = 0; i < 3; i++) {   /* Zahlen entfernen */
					snr[i] = key[i];
					snr1[i] = key[key_pos - 4 + i];
				}

				for (uint32 i = 3; i < (strlen(key) - 3); i++) /* keyWord */
					keyWord[i - 3] = key[i];

				/* umwandeln ! */
				snr[3] = EOS;
				snr1[3] = EOS;
				keyWord[strlen(key) - 6] = EOS;

				byte nr = (byte) atol(snr);
				byte nr1 = (byte) atol(snr1);

				/* keyword einfügen */
				for (uint32 i = 0; i < strlen(keyWord); i++)
					line[line_pos++] = keyWord[i];

				if (known >= nr) {
					NewDynDlgNode *keyNode = keyWords->createNode(keyWord);
					keyNode->_knownBefore = nr;
					keyNode->_knownAfter = nr1;
				}
			}
		}

		line[line_pos++] = EOS;

		bubble->createNode(line);
	}

	return keyWords;
}

void DynamicTalk(uint32 Person1ID, uint32 Person2ID, byte TalkMode) {
	const char *Extension[4] = { "_UNKNOWN", "_KNOWN", "_FRIENDLY", "_BUSINESS" };
	PersonNode *p1 = (PersonNode *) dbGetObject(Person1ID);
	PersonNode *p2 = (PersonNode *) dbGetObject(Person2ID);
	NewList<NewNode> *bubble = new NewList<NewNode>;

	tcChgPersPopularity(p1, 5); /* Bekanntheit steigt sehr gering */

	/* je nach Bekanntheitsgrad wird Matt begrüßt ! */
	char key[TXT_KEY_LENGTH];
	Common::String name = dbGetObjectName(Person2ID);
	strcpy(key, name.c_str());

	uint8 known;
	if (TalkMode & DLG_TALKMODE_BUSINESS) {
		knowsSet(Person1ID, Person2ID);
		known = 3;      /* Business */
	} else if (!(knows(Person1ID, Person2ID))) {
		known = 0;
		knowsSet(Person1ID, Person2ID);
	} else
		known = 1;      /* MOD - kein "FRIENDLY mehr möglich!" */

	strcat(key, Extension[known]);

	uint8 textID;
	if (p2->TalkFileID)
		textID = TALK_1_TXT;
	else
		textID = TALK_0_TXT;

	if (!g_clue->_txtMgr->keyExists(textID, key)) {
		const char *Standard = "STANDARD";
		strcpy(key, Standard);
		strcat(key, Extension[known]);
	}

	uint8 quit;
	uint8 choice = 0, max = 1, stdcount = 0;
	do {
		NewList<NewNode> *origin = g_clue->_txtMgr->goKey(textID, key);
		NewList<NewDynDlgNode> *keyWords = ParseTalkText(origin, bubble, p2->Known);
		NewList<NewNode> *questions = PrepareQuestions(keyWords, p2->TalkBits, textID);

		if (choice < (max - stdcount)) {
			SetPictID(p2->PictID);
			Bubble(bubble, 0, nullptr, 0);
		}

		SetPictID(MATT_PICTID);
		choice = Bubble(questions, 0, nullptr, 0);
		quit = max = questions->getNrOfNodes() -1;

		uint8 i;
		for (i = 0, stdcount = 0; i < 32; i++) {  /* Std Fragen zählen */
			if (p2->TalkBits & (1 << i))
				stdcount++;
		}

		uint8 gencount = max - stdcount;

		if (choice < gencount) {
			NewDynDlgNode *n = keyWords->getNthNode((uint32) choice);

			strcpy(key, name.c_str());
			strcat(key, "_");
			strcat(key, n->_name.c_str());

			if (n->_knownAfter > p2->Known)
				p2->Known = n->_knownAfter;
		}

		if (choice >= gencount && choice < quit) {
			for (i = 0; i < 32; i++) {    /* beim 1. gesetzten Bit anfangen ! */
				if (1 << i & p2->TalkBits)
					break;
			}

			for (uint8 j = 0; (i < 32) && (j != (choice - gencount)); i++) {
				if (1 << i & p2->TalkBits)
					j++;
			}

			switch (i) {
			case 0:
				tcJobOffer(p2);
				tcChgPersPopularity(p1, 10);
				break;
			case 1:
				tcMyJobAnswer(p2);
				break;
			case 2:
				tcPrisonAnswer(p2);
				break;
			case 3:
				tcAbilityAnswer(Person2ID);
				break;
			default:
				break;
			}
		}

		keyWords->removeList();
		origin->removeList();
		questions->removeList();
		bubble->removeNode(nullptr);
	} while (choice != quit);

	bubble->removeList();
}

void PlayFromCDROM() {
	if (StartFrame != DLG_NO_SPEECH && EndFrame != DLG_NO_SPEECH) {
		g_clue->_sndMgr->sndFading(16);
		g_clue->_cdMgr->playSequence(2, StartFrame, EndFrame);
	}
}

byte Say(uint32 TextID, byte activ, uint16 person, const char *text) {
	NewList<NewNode> *bubble = g_clue->_txtMgr->goKey(TextID, text);

	if (person != (uint16)-1)
		SetPictID(person);

	if (g_clue->getFeatures() & ADGF_CD) {
		/* speech output must be started out of the bubble because
		   after starting it there must be no access to the CDROM
		   (neither pictures or text, nor any other directory) or
		   speech would be interrupted */

		if (g_clue->_txtMgr->keyExists(CDROM_TXT, text)) {
			Common::String keys = g_clue->_txtMgr->getFirstLine(CDROM_TXT, text);

			StartFrame = (g_clue->_txtMgr->getKeyAsUint32(1, keys) * 60 +
				g_clue->_txtMgr->getKeyAsUint32(2, keys)) * 75 + g_clue->_txtMgr->getKeyAsUint32(3, keys);
			EndFrame = (g_clue->_txtMgr->getKeyAsUint32(4, keys) * 60 +
				g_clue->_txtMgr->getKeyAsUint32(5, keys)) * 75 + g_clue->_txtMgr->getKeyAsUint32(6, keys);
		} else {
			StartFrame = DLG_NO_SPEECH;
			EndFrame = DLG_NO_SPEECH;
		}
	}

	byte choice = Bubble(bubble, activ, nullptr, 0);

	if (g_clue->getFeatures() & ADGF_CD) {
		g_clue->_cdMgr->stop();
		g_clue->_sndMgr->sndFading(0);

		StartFrame = DLG_NO_SPEECH;
		EndFrame = DLG_NO_SPEECH;
	}
	
	bubble->removeList();
	return choice;
}

uint32 Talk() {
	inpTurnESC(false);

	uint32 locNr = GetObjNrOfLocation(_film->getLocation());

	if (locNr) {
		hasAll(locNr, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Person);
		NewObjectList<dbObjectNode> *bubble = ObjectListPrivate;

		if (!bubble->isEmpty()) {
			inpTurnESC(true);

			Common::String helloFriends = g_clue->_txtMgr->getFirstLine(BUSINESS_TXT, "NO_CHOICE");
			bubble->expandObjectList(helloFriends);

			byte choice = Bubble((NewList<NewNode> *)bubble, 0, nullptr, 0);
			if (ChoiceOk(choice, GET_OUT, bubble)) {
				uint32 personID = bubble->getNthNode((uint32) choice)->_nr;
				inpTurnESC(false);

				if (PersonWorksHere(personID, locNr))
					DynamicTalk(Person_Matt_Stuvysunt, personID, DLG_TALKMODE_BUSINESS);
				else
					DynamicTalk(Person_Matt_Stuvysunt, personID, DLG_TALKMODE_STANDARD);
			}
		} else
			Say(BUSINESS_TXT, 0, MATT_PICTID, "NOBODY HERE");

		bubble->removeList();
	}

	inpTurnESC(true);

	// CHECKME: Is there a reason to return something?
	uint32 succ_event_nr = 0;
	return succ_event_nr;
}

} // End of namespace Clue

/*
**  $Filename: scenes/dealer.c
**  $Release:
**  $Revision:
**  $Date:
**
** "dealer" functions for "Der Clou!"
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

#include "common/util.h"

#include "clue/scenes/scenes.h"
#include "clue/present/interac.h"
#include "clue/data/dataappl.h"


namespace Clue {

void tcDealerDlg() {
	uint32 locNr = GetObjNrOfLocation(_film->getLocation());
	PersonNode *dealer = nullptr;
	byte dealerNr, choice = 0;

	if (locNr == Location_Parker) {
		dealer = (PersonNode *)dbGetObject(Person_Helen_Parker);
		knowsSet(Person_Matt_Stuvysunt, Person_Helen_Parker);
		dealerNr = 2;
	} else if (locNr == Location_Maloya) {
		dealer = (PersonNode *)dbGetObject(Person_Frank_Maloya);
		knowsSet(Person_Matt_Stuvysunt, Person_Frank_Maloya);
		dealerNr = 0;
	} else if (locNr == Location_Pooly) {
		dealer = (PersonNode *)dbGetObject(Person_Eric_Pooly);
		knowsSet(Person_Matt_Stuvysunt, Person_Eric_Pooly);
		dealerNr = 1;
	} else {
		return;
	}

	while ((choice != 2) && (choice != GET_OUT)) {
		choice = Say(BUSINESS_TXT, 0, MATT_PICTID, "DEALER_QUEST");

		switch (choice) {
		case 0:     /* womit ? */
			if (locNr == Location_Parker) {
				Say(BUSINESS_TXT, 0, dealer->PictID, "DEALER_PARKER");
			} else if (locNr == Location_Maloya) {
				Say(BUSINESS_TXT, 0, dealer->PictID, "DEALER_MALOYA");
			} else if (locNr == Location_Pooly) {
				Say(BUSINESS_TXT, 0, dealer->PictID, "DEALER_POOLY");
			}
			break;
		case 1:     /* offer */
			hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Loot);

			if (ObjectList->isEmpty()) {
				Say(BUSINESS_TXT, 0, dealer->PictID, "NO_LOOT");
				addVTime(17);
			} else
				tcDealerOffer(dealer, dealerNr);
			break;
		case 2:
		default:
			break;
		}
	}

	addVTime(11);
	ShowTime(2);
}

void tcDealerOffer(PersonNode *dealer, byte which) {
	static const int32 Price[3][10] = {
		{70, 150, 220, 90, 210, 110, 200, 0, 190, 80}, /* maloya */
		{120, 200, 180, 220, 79, 110, 0, 0, 110, 200}, /* pooly */
		{220, 66, 0, 110, 0, 220, 0, 212, 20, 130}     /* parker */
	};
	CompleteLootNode *comp = (CompleteLootNode *)dbGetObject(CompleteLoot_LastLoot);

	tcMakeLootList(Person_Matt_Stuvysunt, Relation_has)->removeList();

	if (comp->Bild)
		tcDealerSays(dealer, 0, Price[which][0]);
	if (comp->Gold)
		tcDealerSays(dealer, 1, Price[which][1]);
	if (comp->Geld)
		tcDealerSays(dealer, 2, Price[which][2]);
	if (comp->Juwelen)
		tcDealerSays(dealer, 3, Price[which][3]);
	if (comp->Delikates)
		tcDealerSays(dealer, 4, Price[which][4]);
	if (comp->Statue)
		tcDealerSays(dealer, 5, Price[which][5]);
	if (comp->Kuriositaet)
		tcDealerSays(dealer, 6, Price[which][6]);
	if (comp->HistKunst)
		tcDealerSays(dealer, 7, Price[which][7]);
	if (comp->GebrauchsArt)
		tcDealerSays(dealer, 8, Price[which][8]);
	if (comp->Vase)
		tcDealerSays(dealer, 9, Price[which][9]);
}

void tcDealerSays(PersonNode *dealer, byte textNr, int32 perc) {
	NewList<NewNode> *lootNames = g_clue->_txtMgr->goKey(OBJECTS_ENUM_TXT, "enum_LootE");
	NewList<NewNode> *specialLoot = g_clue->_txtMgr->goKey(OBJECTS_ENUM_TXT, "enum_LootNameE");
	NewList<NewNode> *dealerText = g_clue->_txtMgr->goKey(BUSINESS_TXT, "DEALER_OFFER");
	NewList<NewNode> *dealerOffer = new NewList<NewNode>;
	PlayerNode *player = (PlayerNode *)dbGetObject(Player_Player_1);

	PersonNode *others[3];
	others[0] = (PersonNode *)dbGetObject(Person_Frank_Maloya);
	others[1] = (PersonNode *)dbGetObject(Person_Eric_Pooly);
	others[2] = (PersonNode *)dbGetObject(Person_Helen_Parker);

	char line[TXT_KEY_LENGTH];
	if (perc == 0) {
		sprintf(line, dealerText->getNthNode(4)->_name.c_str(), lootNames->getNthNode((uint32) textNr)->_name.c_str());
		dealerOffer->createNode(line);
		dealerOffer->createNode(dealerText->getNthNode(5)->_name);

		SetPictID(dealer->PictID);
		Bubble(dealerOffer, 0, nullptr, 0);
	} else {
		hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Loot);
		perc = tcGetDealerPerc(dealer, perc);

		for (dbObjectNode *n = ObjectList->getListHead(); n->_succ; n = (dbObjectNode *) n->_succ) {
			LootNode *loot = (LootNode *)n;
			uint32 price = hasGet(Person_Matt_Stuvysunt, n->_nr);

			uint32 offer = tcGetDealerOffer(price, perc);
			offer = MAX(offer, 1u);

			dealerOffer->removeNode(NULL);

			if (loot->Type == textNr) {
				byte symp;
				if (loot->Name) {
					symp = 10;

					sprintf(line, dealerText->getNthNode(2)->_name.c_str(),
					        specialLoot->getNthNode((uint32) loot->Name)->_name.c_str());
					dealerOffer->createNode(line);

					sprintf(line, dealerText->getNthNode(3)->_name.c_str(), offer);
					dealerOffer->createNode(line);
				} else {
					symp = 1;

					sprintf(line, dealerText->getNthNode(0)->_name.c_str(), lootNames->getNthNode((uint32) textNr)->_name.c_str());
					dealerOffer->createNode(line);

					sprintf(line, dealerText->getNthNode(1)->_name.c_str(), price, offer);
					dealerOffer->createNode(line);
				}

				SetPictID(dealer->PictID);
				Bubble(dealerOffer, 0, nullptr, 0);

				if (!(Say(BUSINESS_TXT, 0, MATT_PICTID, "DEALER_ANSWER"))) {
					hasUnSet(Person_Matt_Stuvysunt, n->_nr);

					int32 mattsMoney = MAX((((int32)offer * (player->MattsPart)) / 100), 1);

					tcAddDealerSymp(dealer, symp);
					tcAddPlayerMoney(mattsMoney);

					player->StolenMoney += offer;
					player->MyStolenMoney += mattsMoney;

					for (byte i = 0; i < 3; i++) {
						if (dealer != others[i])
							tcAddDealerSymp((others[i]), (symp * (-1)));
					}
				}
			}
		}
	}

	specialLoot->removeList();
	dealerOffer->removeList();
	dealerText->removeList();
	lootNames->removeList();
}

NewList<NewNode>* tcMakeLootList(uint32 containerID, uint32 relID) {
	CompleteLootNode *comp = (CompleteLootNode *)dbGetObject(CompleteLoot_LastLoot);
	NewList<NewNode> *lootE = g_clue->_txtMgr->goKey(OBJECTS_ENUM_TXT, "enum_LootE");
	NewList<NewNode> *lootNameE = g_clue->_txtMgr->goKey(OBJECTS_ENUM_TXT, "enum_LootNameE");
	NewList<NewNode> *out = new NewList<NewNode>;

	/* Listen initialisieren */

	SetObjectListAttr(OLF_PRIVATE_LIST, Object_Loot);
	AskAll(dbGetObject(containerID), relID, BuildObjectList);
	NewList<dbObjectNode> *loots = ObjectListPrivate;

	comp->Bild = comp->Gold = comp->Geld = comp->Juwelen = 0;
	comp->Delikates = comp->Statue = comp->Kuriositaet = 0;
	comp->HistKunst = comp->GebrauchsArt = comp->Vase = 0;

	comp->TotalWeight = comp->TotalVolume = 0;

	/* Liste durcharbeiten */

	if (!loots->isEmpty()) {
		for (dbObjectNode *n = loots->getListHead(); n->_succ; n = (dbObjectNode *)n->_succ) {
			if (n->_type == Object_Loot) {
				LootNode *loot = (LootNode *)n;
				uint32 value = GetP(dbGetObject(containerID), relID, loot);

				switch (loot->Type) {
				case Ein_Bild:
					comp->Bild += value;
					break;
				case Gold:
					comp->Gold += value;
					break;
				case Geld:
					comp->Geld += value;
					break;
				case Juwelen:
					comp->Juwelen += value;
					break;
				case Delikatessen:
					comp->Delikates += value;
					break;
				case Eine_Statue:
					comp->Statue += value;
					break;
				case Eine_Kuriositaet:
					comp->Kuriositaet += value;
					break;
				case Eine_Vase:
					comp->Vase += value;
					break;
				case Ein_historisches_Kunstobjekt:
					comp->HistKunst += value;
					break;
				case Gebrauchsartikel:
					comp->GebrauchsArt += value;
					break;
				default:
					break;
				}

				comp->TotalWeight += loot->Weight;
				comp->TotalVolume += loot->Volume;

				char data[TXT_KEY_LENGTH];
				if (loot->Name)
					strcpy(data, lootNameE->getNthNode(loot->Name)->_name.c_str());
				else
					strcpy(data, lootE->getNthNode(loot->Type)->_name.c_str());

				out->createNode(data);

				sprintf(data, "%u", value);
				out->createNode(data);

				sprintf(data, "%u", loot->Volume);
				out->createNode(data);

				sprintf(data, "%u", loot->Weight);
				out->createNode(data);
			}
		}
	}

	lootE->removeList();
	lootNameE->removeList();
	loots->removeList();

	return out;
}

} // End of namespace Clue

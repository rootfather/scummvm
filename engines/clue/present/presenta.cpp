/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
/*
 * Module Present
 *
 * PresentApp.c
 *
 */

#include "clue/present/present.h"
#include "clue/clue.h"
#include "clue/data/dataappl.h"
#include "clue/scenes/scenes.h"

namespace Clue {

void InitEvidencePresent(uint32 nr, NewList<PresentationInfoNode> *presentationData, NewList<NewNode> *texts) {
	EvidenceNode *e = (EvidenceNode *)dbGetObject(nr);
	Common::String data = dbGetObjectName(e->pers);

	AddPresentTextLine(presentationData, data, 0, texts, 0);
	AddPresentLine(presentationData, PRESENT_AS_BAR, e->Recognition, 255, texts, 1);
	AddPresentLine(presentationData, PRESENT_AS_BAR, e->WalkTrail, 255, texts, 2);
	AddPresentLine(presentationData, PRESENT_AS_BAR, e->WaitTrail, 255, texts, 3);
	AddPresentLine(presentationData, PRESENT_AS_BAR, e->WorkTrail, 255, texts, 4);
	AddPresentLine(presentationData, PRESENT_AS_BAR, e->KillTrail, 255, texts, 5);
	AddPresentLine(presentationData, PRESENT_AS_BAR, e->CallTrail, 255, texts, 6);
	AddPresentLine(presentationData, PRESENT_AS_BAR, e->FotoTrail, 255, texts, 8);
	AddPresentLine(presentationData, PRESENT_AS_BAR, e->PaperTrail, 255, texts, 7);
}

void InitLootPresent(uint32 nr, NewList<PresentationInfoNode>* presentationData, NewList<NewNode>* texts) {
	CompleteLootNode *comp = (CompleteLootNode *)dbGetObject(CompleteLoot_LastLoot);

	tcMakeLootList(Person_Matt_Stuvysunt, Relation_has)->removeList();

	uint32 total = comp->Bild + comp->Gold + comp->Geld + comp->Juwelen +
	        comp->Statue + comp->Kuriositaet + comp->HistKunst +
	        comp->GebrauchsArt + comp->Vase + comp->Delikates;

	AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->Bild, 0, texts, 0);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->Gold, 0, texts, 1);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->Geld, 0, texts, 2);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->Juwelen, 0, texts, 3);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->Delikates, 0, texts, 4);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->Statue, 0, texts, 5);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->Kuriositaet, 0, texts, 6);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->HistKunst, 0, texts, 7);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->GebrauchsArt, 0, texts, 8);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->Vase, 0, texts, 9);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, total, 0, texts, 10);
}

void InitOneLootPresent(uint32 nr, NewList<PresentationInfoNode>* presentationData, NewList<NewNode>* texts) {
	LootNode *loot = (LootNode *)dbGetObject(nr);

	Common::String data;
	if (loot->Name == Kein_Name)
		data = dbGetObjectName(nr);
	else
		data = g_clue->_txtMgr->getNthString(OBJECTS_ENUM_TXT, "enum_LootNameE", loot->Name);

	AddPresentTextLine(presentationData, data, 0, texts, 4);

	/* some nice guy put the value in this relation */
	/* (... and because he is really nice, he will kill this one after usage) */
	uint32 value = GetP(loot, hasLootRelationID, loot);

	AddPresentLine(presentationData, PRESENT_AS_NUMBER, value, 0, texts, 5);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, loot->Weight, 0, texts, 6);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, loot->Volume, 0, texts, 7);
}

void InitObjectPresent(uint32 nr, NewList<PresentationInfoNode>* presentationData, NewList<NewNode>* texts) {
	LSObjectNode *lso = (LSObjectNode *)dbGetObject(nr);

	Common::String data = dbGetObjectName(lso->Type);

	AddPresentTextLine(presentationData, data, 0, texts, 0);

	if (lso->uch_Chained & Const_tcCHAINED_TO_ALARM)
		AddPresentTextLine(presentationData, nullptr, 0, texts, 2);

	if (lso->uch_Chained & Const_tcCHAINED_TO_POWER)
		AddPresentTextLine(presentationData, nullptr, 0, texts, 1);

	if (lso->Type == Item_Clock)
		AddPresentLine(presentationData, PRESENT_AS_NUMBER, ClockTimerGet(nr, nr), 0, texts, 8);

	NewList<NewNode> *l = tcMakeLootList(nr, hasLootRelationID);

	if (l->isEmpty())
		AddPresentTextLine(presentationData, nullptr, 0, texts, 3);

	l->removeList();
}

void InitToolPresent(uint32 nr, NewList<PresentationInfoNode>* presentationData, NewList<NewNode>* texts) {
	NewList<NewNode> *tools = g_clue->_txtMgr->goKey(OBJECTS_ENUM_TXT, "enum_ItemE");
	NewList<NewNode> *abilities = g_clue->_txtMgr->goKey(OBJECTS_ENUM_TXT, "enum_AbilityE");

	ToolNode *obj = (ToolNode *) dbGetObject(nr);

	Common::String data = dbGetObjectName(nr);

	AddPresentTextLine(presentationData, data, 0, texts, 0);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, tcGetToolTraderOffer(obj), 0, texts, 1);
	AddPresentLine(presentationData, PRESENT_AS_BAR, tcRGetDanger(obj), 255, texts, 3);
	AddPresentLine(presentationData, PRESENT_AS_BAR, tcRGetVolume(obj), 255, texts, 4);

	/*** Werkzeuge ***/

	toolRequiresAll(nr, OLF_INCLUDE_NAME | OLF_NORMAL, Object_Tool);

	dbObjectNode *n;
	byte i;
	for (n = ObjectList->getListHead(), i = 5; n->_succ; n = (dbObjectNode *)n->_succ, i = 6)
		AddPresentTextLine(presentationData, n->_name, 0, texts, i);

	/*** Eigenschaften ***/

	toolRequiresAll(nr, OLF_INCLUDE_NAME | OLF_NORMAL, Object_Ability);

	if (!ObjectList->isEmpty()) {
		AddPresentTextLine(presentationData, nullptr, 0, texts, 8);    /* "benötigt Wissen über..." */

		for (n = ObjectList->getListHead(); n->_succ; n = (dbObjectNode *)n->_succ) {
			AbilityNode *ability = (AbilityNode *)n;

			AddPresentLine(presentationData, PRESENT_AS_BAR,
			               toolRequiresGet(nr, n->_nr), 255, abilities,
			               ability->Name);
		}
	}

	/*** Verwendung ***/

	breakAll(nr, OLF_INCLUDE_NAME | OLF_NORMAL, Object_Item);

	if (!ObjectList->isEmpty())
		AddPresentTextLine(presentationData, nullptr, 0, texts, 7);

	for (n = ObjectList->getListHead(); n->_succ; n = (dbObjectNode *)n->_succ) {
		uint32 itemNr = n->_nr;
		uint32 time = breakGet(nr, itemNr);
		ItemNode *item = (ItemNode *)n;

		data = Common::String::format("%.2d:%.2d", time / 60, time % 60);
		AddPresentTextLine(presentationData, data, 0, tools, item->Type);
	}

	tools->removeList();
	abilities->removeList();
}

void InitBuildingPresent(uint32 nr, NewList<PresentationInfoNode>* presentationData, NewList<NewNode>* texts) {
	BuildingNode *obj = (BuildingNode *) dbGetObject(nr);

	Common::String data = dbGetObjectName(nr);
	AddPresentTextLine(presentationData, data, 0, texts, 0);

	data = buildTime(tcGetBuildPoliceT(obj));
	AddPresentTextLine(presentationData, data, 0, texts, 1);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, tcGetBuildValues(obj), 255, texts, 2);

	data = g_clue->_txtMgr->getNthString(OBJECTS_ENUM_TXT, "enum_RouteE", obj->EscapeRoute);
	AddPresentTextLine(presentationData, data, 0, texts, 3);

	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Exactlyness, 255, texts, 4);
	AddPresentLine(presentationData, PRESENT_AS_BAR, tcGetBuildGRate(obj), 255, texts, 5);
	AddPresentLine(presentationData, PRESENT_AS_BAR, tcRGetBuildStrike(obj), 255, texts, 6);
	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->RadioGuarding, 255, texts, 7);
	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->GuardStrength, 255, texts, 8);
	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->MaxVolume, 255, texts, 9);
}


void InitPlayerPresent(uint32 nr, NewList<PresentationInfoNode>* presentationData, NewList<NewNode>* texts) {
	PlayerNode *player = (PlayerNode *) dbGetObject(nr);

	AddPresentLine(presentationData, PRESENT_AS_NUMBER, player->Money, 0, texts, 0);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, player->StolenMoney, 0, texts, 1);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, player->MyStolenMoney, 0, texts, 2);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, player->MattsPart, 0, texts, 3);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, player->NrOfBurglaries, 0, texts, 4);
}

void InitPersonPresent(uint32 nr, NewList<PresentationInfoNode>* presentationData, NewList<NewNode>* texts) {
	PersonNode *obj = (PersonNode *)dbGetObject(nr);

	Common::String data = dbGetObjectName(nr);
	AddPresentTextLine(presentationData, data, 0, texts, 0);

	data = g_clue->_txtMgr->getNthString(OBJECTS_ENUM_TXT, "enum_JobE", obj->Job);
	AddPresentTextLine(presentationData, data, 0L, texts, 1);

	data = g_clue->_txtMgr->getNthString(OBJECTS_ENUM_TXT, "enum_SexE", obj->Sex);
	AddPresentTextLine(presentationData, data, 0L, texts, 2);

	AddPresentLine(presentationData, PRESENT_AS_NUMBER, obj->Age, 0L, texts, 3);

	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Health, 255, texts, 4);
	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Mood, 255, texts, 5);
	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Intelligence, 255, texts, 6);
	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Strength, 255, texts, 7);
	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Stamina, 255, texts, 8);
	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Loyality, 255, texts, 9);
	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Skill, 255, texts, 10);
	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Known, 255, texts, 11);
	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Popularity, 255, texts, 12);
	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Avarice, 255, texts, 13);
	AddPresentLine(presentationData, PRESENT_AS_BAR, (255 - obj->Panic), 255, texts, 14);
	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->KnownToPolice, 255, texts, 15);

	hasAll(nr, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Ability);
	NewList<dbObjectNode> *abilities = ObjectListPrivate;

	if (!abilities->isEmpty()) {
		AddPresentTextLine(presentationData, nullptr, 0, texts, 16);

		byte i;
		NewNode *node;
		for (node = (NewNode *) abilities->getListHead(), i = 0; node->_succ; node = (NewNode *) node->_succ, i++) {
			uint32 abiNr = abilities->getNthNode(i)->_nr;
			AbilityNode *abi = (AbilityNode *) dbGetObject(abiNr);
	
			data = g_clue->_txtMgr->getNthString(OBJECTS_ENUM_TXT, "enum_AbilityE", abi->Name);
			texts->createNode(data.c_str());
	
			AddPresentLine(presentationData, PRESENT_AS_BAR, hasGet(nr, abiNr), 255, texts, 17 + i);
		}
	}

	abilities->removeList();
}

void InitCarPresent(uint32 nr, NewList<PresentationInfoNode>* presentationData, NewList<NewNode>* texts) {
	CarNode *obj = (CarNode *) dbGetObject(nr);

	Common::String data = dbGetObjectName(nr);
	AddPresentTextLine(presentationData, data, 0, texts, 0);

	data = g_clue->_txtMgr->getNthString(OBJECTS_ENUM_TXT, "enum_LandE", obj->Land);
	AddPresentTextLine(presentationData, data, 0, texts, 1);

	AddPresentLine(presentationData, PRESENT_AS_NUMBER, tcRGetCarAge(obj), 0, texts, 2);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, tcGetCarPrice(obj), 0, texts, 3);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, tcGetCarPS(obj), 0, texts, 4);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, tcGetCarSpeed(obj), 0, texts, 5);
	AddPresentLine(presentationData, PRESENT_AS_NUMBER, obj->PlacesInCar, 0, texts, 6);

	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->State, 255, texts, 7);
	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->BodyWorkState, 255, texts, 8);
	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->TyreState, 255, texts, 9);
	AddPresentLine(presentationData, PRESENT_AS_BAR, obj->MotorState, 255, texts, 10);

	if (g_clue->getFeatures() & GF_PROFIDISK) {
		AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Capacity, 9000000, texts, 11);
	} else {
		AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Capacity, 7000000, texts, 11);
	}

	AddPresentLine(presentationData, PRESENT_AS_BAR, tcGetCarStrike(obj), 255, texts, 12);
}

} // End of namespace Clue

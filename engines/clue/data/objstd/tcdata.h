/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_TCDATA
#define MODULE_TCDATA

#include "clue/theclou.h"

#include "clue/data/objstd/tckeys.h"

namespace Clue {

/*
 * Data header file created with dc
 * (c) 1993 by ...and avoid panic by
 *
 * Database (offset, size) : tcMain (0, 10000)
 */

#define DB_tcMain_OFFSET                    0
#define DB_tcMain_SIZE                  10000

#define Const_tcACCESS_BIT                  4
#define Const_tcCHAINED_TO_ALARM            1
#define Const_tcCHAINED_TO_POWER            2
#define Const_tcHORIZ_VERT_BIT              0
#define Const_tcIN_PROGRESS_BIT             5
#define Const_tcLOCK_UNLOCK_BIT             2
#define Const_tcON_OFF                      6
#define Const_tcOPEN_CLOSE_BIT              1
#define Const_tcTAKE_BIT                    3
#define Const_tcTALK_JOB_OFFER              0
#define Const_tcTOOL_OPENS                  1

#define Relation_hasClock      1
#define hasClockAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 1, BuildObjectList); }
#define hasClockGet(l, r)       GetP(dbGetObject(l), 1, dbGetObject(r))

#define Relation_ClockTimer      2
#define ClockTimerSetP(l,r,p)    SetP   (dbGetObject (l), 2, dbGetObject (r), p)
#define ClockTimerGet(l,r)       GetP   (dbGetObject (l), 2, dbGetObject (r))

#define Relation_StairConnects      3
#define StairConnectsGet(l,r)       GetP   (dbGetObject (l), 3, dbGetObject (r))

#define Relation_has      4
#define has(l,r)          AskP   (dbGetObject (l), 4, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define hasAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 4, BuildObjectList); }
#define hasGet(l,r)       GetP   (dbGetObject (l), 4, dbGetObject (r))
#define hasSet(l,r)       SetP   (dbGetObject (l), 4, dbGetObject (r), NO_PARAMETER)
#define hasSetP(l,r,p)    SetP   (dbGetObject (l), 4, dbGetObject (r), p)
#define hasUnSet(l,r)     UnSet  (dbGetObject (l), 4, dbGetObject (r))

//#define Relation_knows      5
#define knows(l,r)          AskP   (dbGetObject (l), 5, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define knowsAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 5, BuildObjectList); }
#define knowsSet(l, r)       SetP(dbGetObject(l), 5, dbGetObject(r), NO_PARAMETER)

//#define Relation_likes      6
#define likes_to_beAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 6, BuildObjectList); }

//#define Relation_join      7
#define join(l,r)          AskP   (dbGetObject (l), 7, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define joinAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 7, BuildObjectList); }
#define joinSet(l,r)       SetP   (dbGetObject (l), 7, dbGetObject (r), NO_PARAMETER)
#define joinUnSet(l,r)     UnSet  (dbGetObject (l), 7, dbGetObject (r))

//#define Relation_joined_by      8
#define joined_byAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 8, BuildObjectList); }
#define joined_bySet(l,r)       SetP   (dbGetObject (l), 8, dbGetObject (r), NO_PARAMETER)
#define joined_byUnSet(l,r)     UnSet  (dbGetObject (l), 8, dbGetObject (r))

//#define Relation_uses      9

//#define Relation_livesIn      10
#define livesIn(l,r)          AskP   (dbGetObject (l), 10, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define livesInSet(l,r)       SetP   (dbGetObject (l), 10, dbGetObject (r), NO_PARAMETER)
#define livesInUnSet(l,r)     UnSet  (dbGetObject (l), 10, dbGetObject (r))

//#define Relation_break      11
#define break_(l,r)         AskP   (dbGetObject (l), 11, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define breakAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 11, BuildObjectList); }
#define breakGet(l,r)       GetP   (dbGetObject (l), 11, dbGetObject (r))

//#define Relation_hurt      12
#define hurtGet(l,r)       GetP   (dbGetObject (l), 12, dbGetObject (r))

//#define Relation_sound      13
#define soundGet(l,r)       GetP   (dbGetObject (l), 13, dbGetObject (r))

//#define Relation_opens      14
#define opensGet(l,r)       GetP   (dbGetObject (l), 14, dbGetObject (r))

//#define Relation_toolRequires      15
#define toolRequiresAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 15, BuildObjectList); }
#define toolRequiresGet(l,r)       GetP   (dbGetObject (l), 15, dbGetObject (r))

#define Relation_taxi      16
#define taxiAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 16, BuildObjectList); }
#define taxiSet(l,r)       SetP   (dbGetObject (l), 16, dbGetObject (r), NO_PARAMETER)
#define taxiUnSet(l,r)     UnSet  (dbGetObject (l), 16, dbGetObject (r))

//#define Relation_learned      17
#define learned(l,r)          AskP   (dbGetObject (l), 17, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define learnedGet(l,r)       GetP   (dbGetObject (l), 17, dbGetObject (r))
#define learnedSetP(l,r,p)    SetP   (dbGetObject (l), 17, dbGetObject (r), p)
#define learnedUnSet(l,r)     UnSet  (dbGetObject (l), 17, dbGetObject (r))
	
//#define Relation_remember      18
#define rememberAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 18, BuildObjectList); }
#define rememberSet(l,r)       SetP   (dbGetObject (l), 18, dbGetObject (r), NO_PARAMETER)
#define rememberUnSet(l,r)     UnSet  (dbGetObject (l), 18, dbGetObject (r))

//#define Relation_hasLootBag      19
#define hasLootBag(l,r)          AskP   (dbGetObject (l), 19, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define hasLootBagSet(l,r)       SetP   (dbGetObject (l), 19, dbGetObject (r), NO_PARAMETER)
#define hasLootBagUnSet(l,r)     UnSet  (dbGetObject (l), 19, dbGetObject (r))

//#define Relation_PersonWorksHere      20
#define PersonWorksHere(l,r)          AskP   (dbGetObject (l), 20, dbGetObject (r), NO_PARAMETER, CMP_NO)

enum LootNameE { Kein_Name, Eine_Zigarrenkiste, Ein_Tiefgekuehlter_Hummer,
                 Eine_Cartier_Uhr, Ein_Sanskrit_Buch, Ein_Autogramm_von_Oscar_Wilde,
                 Ein_Beauty_Case, Ein_Sparstrumpf, Eine_Toilettetasche,
                 Die_Gebeine_von_Karl_Marx, Eine_Bibel, Eine_Bronzestatue,
                 Ein_Silbergeschirr, Eine_Buddhastatue, Eine_Chinavase,
                 Der_Stein_von_Rosetta, Die_Magna_Charta, Eine_Muenzsammlung,
                 Ein_Goldring, Ein_Saurierknochen, Eine_Insektensammlung,
                 Ein_Bernstein, Das_Jadepferd, Eine_Victorianische_Vase,
                 Die_Kronjuwelen, Der_Ring_des_Abtes, Ein_Koffer, Ein_Dokument,
                 Sherlock_Holmes_Opiumpfeife, Eine_Hottentotten_Figur,
                 Eine_Batman_Figur, Ein_Dicker_Man, Ein_Unbekannter,
                 Eine_Jack_the_Ripper_Figur, Eine_Koenigs_Figur, Eine_Wache_Figur,
                 Eine_Miss_World_1952_Figur, Eine_Formel, Die_Unterhose_der_Queen,
                 Der_Stone_of_Cone
               };
enum SexE { Maennlich, Weiblich };
enum ItemE { Holztuer, Stahltuer, Ausstellungskasten, Kasse, Kasten,
             Schreibtisch, Nachtkaestchen, Wertschrank, Stahlschrank,
             Panzerschrank, Tresorraum, Mauer, Mauerecke, Steinmauer, Mauertor,
             Zaun, Zauntor, Saeule, Fenster, Alarmanlage_Z3, Alarmanlage_X3,
             Alarmanlage_Top, Mikrophon, Stechuhr, Scheinwerfer, Lichtschranke,
             Steuerkasten, Grab, Grabhuegel, Kreuz, Kranz, Gruft, Treppe,
             Sockel, WC, Sessel, Pult, Statue, Beutesack, Bett, Billardtisch,
             Sofa, Spiegel, Tisch, Kamin, Badewanne, Tresen, Gemaelde, Klavier,
             Buecherregal, Skelett, Dinosaurier, Elch, Kanone, Beefeater, Busch,
             Strauch, Gebuesch, Laubbaum, Nadelbaum, Hecke, Hantel, Regal,
             Flugzeugheck, linker_Flugzeugfluegel, rechter_Flugzeugfluegel,
             Bild, Vase, Kuehlschrank, Wache, Beichtstuhl, Kirchenbank, Altar,
             Kroehnungsstuhl, Opferkassa, Schrein, Kerzen, Weihwasserbecken,
             Heiligenstatue, Kerzenstaender, Kirchenkreuz, Gelaender,
             verzierte_Saeule, Regal_mit_Chemikalien, Versuchstisch, Kiste,
             Fasz, leere_Palette, Palette_mit_Schachteln, Absperrung,
             Hottentotten_Figur, Batman_Figur, Dicker_Man, Unbekannter,
             Jack_the_Ripper_Figur, Koenigs_Figur, Wache_Figur, Miss_World_1952,
             groszer_Koffer, Sitzbank, Speisetisch, Holzkiste, Palette_mit_Kisten,
             Postsack, Leiter, Zuggelaender, Lokomotive_vorne_rechts,
             Lokomotive_vorne_links, Lokomotive_oben, Lokomotive_seitlich,
             Lokomotive_Kabine, Lokomotive_Tuer
           };
enum ColorE { Grau, Gruen, Tuerkis, Violett, Beige, Silber, Rot, Hellgruen,
              Blau, Gelb
            };
enum AbilityE { Autos, Schloesser, Sprengstoff, Safes, Elektronik,
                Aufpassen, Kampf
              };
enum LockE { Buntbartschloss, Kombinationsschloss, Zylinderschloss };
enum LandE { GB, USA, Italien, Deutschland };
enum JobE { Detektiv, Sprengmeister, Schlosser, Hehler, Haendler,
            Taxifahrer, Mechaniker, Elektriker, Sekretaerin, Anwalt, Unbekannt,
            Profiboxer, Angestellte, Soldat, Student, Literat, Professor,
            Hausfrau, Politiker, Abenteurer, Agent, Musiker, Pilot, Kellner,
            Rezeptionist, Polizeichef, Polizist, Moench, Inspektor,
            Pizzafahrer, Kellner_und_Student, Arbeiter, Butler,
            Krankenschwester
          };
enum LootE { Ein_Bild, Gold, Geld, Juwelen, Delikatessen, Eine_Statue,
             Eine_Kuriositaet, Ein_historisches_Kunstobjekt, Gebrauchsartikel,
             Eine_Vase
           };


#define Object_Person              9900L

class PersonNode : public dbObjectNode {
public:
	uint16 PictID;
	enum JobE Job;
	enum SexE Sex;
	int8 Age;
	uint8 Health;
	uint8 Mood;
	uint8 Intelligence;
	uint8 Strength;
	uint8 Stamina;
	uint8 Loyality;
	uint8 Skill;
	uint8 Known;
	uint8 Popularity;
	uint8 Avarice;
	uint8 Panic;
	uint8 KnownToPolice;
	uint32 TalkBits;
	uint8 TalkFileID;
	uint8 OldHealth;

	PersonNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

#define Object_Player              9901L
class PlayerNode : public dbObjectNode {
public:
	uint32 Money;
	uint32 StolenMoney;
	uint32 MyStolenMoney;
	uint8 NrOfBurglaries;
	uint8 JobOfferCount;
	uint8 MattsPart;
	uint32 CurrScene;
	uint32 CurrDay;
	uint32 CurrMinute;
	uint32 CurrLocation;

	PlayerNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

#define Object_Car              9902L
class CarNode : public dbObjectNode {
public:
	uint16 PictID;
	enum LandE Land;
	uint32 Value;
	uint16 YearOfConstruction;
	enum ColorE ColorIndex;
	uint8 Strike;
	uint32 Capacity;
	uint8 PS;
	uint8 Speed;
	uint8 State;
	uint8 MotorState;
	uint8 BodyWorkState;
	uint8 TyreState;
	uint8 PlacesInCar;

	CarNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

#define Object_Location              9903L
class LocationNode : public dbObjectNode {
public:
	uint32 LocationNr;
	uint16 OpenFromMinute;
	uint16 OpenToMinute;

	LocationNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

#define Object_Ability              9904L
class AbilityNode : public dbObjectNode {
public:
	enum AbilityE Name;
	uint32 Use;

	AbilityNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

#define Object_Item              9905L
class ItemNode : public dbObjectNode {
public:
	enum ItemE Type;
	uint16 OffsetFact;
	uint16 HExactXOffset;
	uint16 HExactYOffset;
	uint16 HExactWidth;
	uint16 HExactHeight;
	uint16 VExactXOffset;
	uint16 VExactYOffset;
	uint16 VExactWidth;
	uint16 VExactHeight;
	uint8 Size;
	uint8 ColorNr;

	ItemNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

#define Object_Tool              9906L
class ToolNode : public dbObjectNode {
public:
	uint16 PictID;
	uint32 Value;
	uint8 Danger;
	uint8 Volume;
	uint8 Effect;

	ToolNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

#define Object_Environment              9907L
class EnvironmentNode : public dbObjectNode {
public:
	uint8 MattHasHotelRoom;
	uint8 MattHasIdentityCard;
	uint8 WithOrWithoutYou;
	uint8 MattIsInLove;
	uint8 SouthhamptonHappened;
	uint8 Present;
	uint8 FirstTimeInSouth;
	uint8 PostzugDone;                     /* PROFIDISK */

	EnvironmentNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

#define Object_London              9908L
class LondonNode : public dbObjectNode {
public:
	uint8 Useless;

	LondonNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

#define Object_Evidence              9909L
class EvidenceNode : public dbObjectNode {
public:
	uint32 pers;
	uint8 Recognition;
	uint8 WalkTrail;
	uint8 WaitTrail;
	uint8 WorkTrail;
	uint8 KillTrail;
	uint8 CallTrail;
	uint8 PaperTrail;
	uint8 FotoTrail;

	EvidenceNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

#define Object_Loot              9910L
class LootNode : public dbObjectNode {
public:
	enum LootE Type;
	enum LootNameE Name;
	uint32 Volume;
	uint16 Weight;
	uint16 PictID;

	LootNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

#define Object_CompleteLoot              9911L
class CompleteLootNode : public dbObjectNode {
public:
	uint32 Bild;
	uint32 Gold;
	uint32 Geld;
	uint32 Juwelen;
	uint32 Delikates;
	uint32 Statue;
	uint32 Kuriositaet;
	uint32 HistKunst;
	uint32 GebrauchsArt;
	uint32 Vase;
	uint32 TotalWeight;
	uint32 TotalVolume;

	CompleteLootNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

#define Object_LSOLock              9912L
class LSOLockNode : public dbObjectNode {
public:
	enum LockE Type;

	LSOLockNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

#define Object_LSObject              9913L
class LSObjectNode : public dbObjectNode {
public:
	uint16 us_OffsetFact;
	uint16 us_DestX;
	uint16 us_DestY;
	uint8 uch_ExactX;
	uint8 uch_ExactY;
	uint8 uch_ExactX1;
	uint8 uch_ExactY1;
	uint8 uch_Size;
	uint8 uch_Visible;
	uint8 uch_Chained;
	uint32 ul_Status;
	uint32 Type;

	LSObjectNode() {}
	LSObjectNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

/*
 * Data header file created with dc
 * (c) 1993 by ...and avoid panic by
 *
 * Database (offset, size) : tcBuild (500000, 10000)
 */

#define DB_tcBuild_OFFSET        500000
#define DB_tcBuild_SIZE          10000

//#define Relation_isGuardedby      500003
#define isGuardedbyAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 500003, BuildObjectList); }
#define isGuardedbyGet(l,r)       GetP   (dbGetObject (l), 500003, dbGetObject (r))
	
//#define Relation_startsWith      500001
#define startsWithAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 500001, BuildObjectList); }
	
//#define Relation_consistsOf      500002
#define consistsOfAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 500002, BuildObjectList); }

enum RouteE { Innenstadt, Stadt, Landstrasse, Autobahn };


#define Object_Building              509990L
class BuildingNode : public dbObjectNode {
public:
	uint32 LocationNr;
	uint16 PoliceTime;
	uint16 GTime;
	uint8 Exactlyness;
	uint8 GRate;
	uint8 Strike;
	uint32 Values;
	enum RouteE EscapeRoute;
	uint8 EscapeRouteLength;
	uint8 RadioGuarding;
	uint8 MaxVolume;
	uint8 GuardStrength;
	uint16 CarXPos;
	uint16 CarYPos;
	uint8 DiskId;                      /* PROFIDISK */

	BuildingNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

#define Object_Police              509991L
class PoliceNode : public dbObjectNode {
public:
	uint16 PictID;
	uint8 LivingID;

	PoliceNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

#define Object_LSArea              509992L
class LSAreaNode : public dbObjectNode {
public:
	uint16 us_Coll16ID;
	uint16 us_Coll32ID;
	uint16 us_Coll48ID;
	uint16 us_PlanColl16ID;
	uint16 us_PlanColl32ID;
	uint16 us_PlanColl48ID;
	uint16 us_FloorCollID;
	uint16 us_PlanFloorCollID;
	uint16 us_Width;
	uint16 us_Height;
	uint32 ul_ObjectBaseNr;
	uint8 uch_Darkness;
	uint16 us_StartX0;
	uint16 us_StartX1;
	uint16 us_StartX2;
	uint16 us_StartX3;
	uint16 us_StartX4;
	uint16 us_StartX5;
	uint16 us_StartY0;
	uint16 us_StartY1;
	uint16 us_StartY2;
	uint16 us_StartY3;
	uint16 us_StartY4;
	uint16 us_StartY5;

	LSAreaNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

#define Object_LSRoom              509993L
class LSRoomNode : public dbObjectNode {
public:
	uint16 us_LeftEdge;
	uint16 us_TopEdge;
	uint16 us_Width;
	uint16 us_Height;

	LSRoomNode(uint32 nr, uint32 type, Common::String name, uint32 realNr) : dbObjectNode(nr, type, name, realNr) {}
	void mapper(dbObjectNode *srcNode);
};

} // End of namespace Clue

#endif                              /* MODULE_TCDATA */

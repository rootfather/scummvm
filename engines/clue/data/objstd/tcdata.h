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

/*
 * Data header file created with dc
 * (c) 1993 by ...and avoid panic by
 *
 * Database (offset, size) : tcMain (0, 10000)
 */

#define DB_tcMain_OFFSET                    0
#define DB_tcMain_SIZE                  10000


#define Const_tcTALK_ABILITY                4
#define Const_tcIN_PROGRESS_BIT             5
#define Const_tcLOCK_UNLOCK_BIT             2
#define Const_tcTALK_JOB_OFFER              0
#define Const_tcON_OFF                      6
#define Const_tcCAR_PICT_OFFSET            24
#define Const_tcCHAINED_TO_ALARM            1
#define Const_tcHORIZ_VERT_BIT              0
#define Const_tcTAKE_BIT                    3
#define Const_tcALARM_PICT_OFFSET          25
#define Const_tcTALK_PRISON_BEFORE          2
#define Const_tcACCESS_BIT                  4
#define Const_tcTALK_YOUR_JOB               1
#define Const_tcOPEN_CLOSE_BIT              1
#define Const_tcNO_CHAINING                 0
#define Const_tcTOOL_OPENS                  1
#define Const_tcCHAINED_TO_POWER            2
#define Const_tcSYMBOL_PICT_SIZE           16
#define Const_tcPOWER_PICT_OFFSET          26


#define Relation_hasClock      1
#define hasClockP(l,r,p,c)     AskP   (dbGetObject (l), 1, dbGetObject (r), p, c)
#define hasClock(l,r)          AskP   (dbGetObject (l), 1, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define hasClockAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 1, BuildObjectList); }
#define hasClockSetP(l,r,p)    SetP   (dbGetObject (l), 1, dbGetObject (r), p)
#define hasClockSet(l,r)       SetP   (dbGetObject (l), 1, dbGetObject (r), NO_PARAMETER)
#define hasClockUnSet(l,r)     UnSet  (dbGetObject (l), 1, dbGetObject (r))
#define hasClockGet(l,r)       GetP   (dbGetObject (l), 1, dbGetObject (r))

#define Relation_ClockTimer      2
#define ClockTimerP(l,r,p,c)     AskP   (dbGetObject (l), 2, dbGetObject (r), p, c)
#define ClockTimer(l,r)          AskP   (dbGetObject (l), 2, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define ClockTimerAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 2, BuildObjectList); }
#define ClockTimerSetP(l,r,p)    SetP   (dbGetObject (l), 2, dbGetObject (r), p)
#define ClockTimerSet(l,r)       SetP   (dbGetObject (l), 2, dbGetObject (r), NO_PARAMETER)
#define ClockTimerUnSet(l,r)     UnSet  (dbGetObject (l), 2, dbGetObject (r))
#define ClockTimerGet(l,r)       GetP   (dbGetObject (l), 2, dbGetObject (r))

#define Relation_StairConnects      3
#define StairConnectsP(l,r,p,c)     AskP   (dbGetObject (l), 3, dbGetObject (r), p, c)
#define StairConnects(l,r)          AskP   (dbGetObject (l), 3, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define StairConnectsAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 3, BuildObjectList); }
#define StairConnectsSetP(l,r,p)    SetP   (dbGetObject (l), 3, dbGetObject (r), p)
#define StairConnectsSet(l,r)       SetP   (dbGetObject (l), 3, dbGetObject (r), NO_PARAMETER)
#define StairConnectsUnSet(l,r)     UnSet  (dbGetObject (l), 3, dbGetObject (r))
#define StairConnectsGet(l,r)       GetP   (dbGetObject (l), 3, dbGetObject (r))

#define Relation_has      4
#define hasP(l,r,p,c)     AskP   (dbGetObject (l), 4, dbGetObject (r), p, c)
#define has(l,r)          AskP   (dbGetObject (l), 4, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define hasAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 4, BuildObjectList); }
#define hasSetP(l,r,p)    SetP   (dbGetObject (l), 4, dbGetObject (r), p)
#define hasSet(l,r)       SetP   (dbGetObject (l), 4, dbGetObject (r), NO_PARAMETER)
#define hasUnSet(l,r)     UnSet  (dbGetObject (l), 4, dbGetObject (r))
#define hasGet(l,r)       GetP   (dbGetObject (l), 4, dbGetObject (r))

#define Relation_knows      5
#define knowsP(l,r,p,c)     AskP   (dbGetObject (l), 5, dbGetObject (r), p, c)
#define knows(l,r)          AskP   (dbGetObject (l), 5, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define knowsAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 5, BuildObjectList); }
#define knowsSetP(l,r,p)    SetP   (dbGetObject (l), 5, dbGetObject (r), p)
#define knowsSet(l,r)       SetP   (dbGetObject (l), 5, dbGetObject (r), NO_PARAMETER)
#define knowsUnSet(l,r)     UnSet  (dbGetObject (l), 5, dbGetObject (r))
#define knowsGet(l,r)       GetP   (dbGetObject (l), 5, dbGetObject (r))

#define Relation_likes_to_be      6
#define likes_to_beP(l,r,p,c)     AskP   (dbGetObject (l), 6, dbGetObject (r), p, c)
#define likes_to_be(l,r)          AskP   (dbGetObject (l), 6, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define likes_to_beAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 6, BuildObjectList); }
#define likes_to_beSetP(l,r,p)    SetP   (dbGetObject (l), 6, dbGetObject (r), p)
#define likes_to_beSet(l,r)       SetP   (dbGetObject (l), 6, dbGetObject (r), NO_PARAMETER)
#define likes_to_beUnSet(l,r)     UnSet  (dbGetObject (l), 6, dbGetObject (r))
#define likes_to_beGet(l,r)       GetP   (dbGetObject (l), 6, dbGetObject (r))

#define Relation_join      7
#define joinP(l,r,p,c)     AskP   (dbGetObject (l), 7, dbGetObject (r), p, c)
#define join(l,r)          AskP   (dbGetObject (l), 7, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define joinAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 7, BuildObjectList); }
#define joinSetP(l,r,p)    SetP   (dbGetObject (l), 7, dbGetObject (r), p)
#define joinSet(l,r)       SetP   (dbGetObject (l), 7, dbGetObject (r), NO_PARAMETER)
#define joinUnSet(l,r)     UnSet  (dbGetObject (l), 7, dbGetObject (r))
#define joinGet(l,r)       GetP   (dbGetObject (l), 7, dbGetObject (r))

#define Relation_joined_by      8
#define joined_byP(l,r,p,c)     AskP   (dbGetObject (l), 8, dbGetObject (r), p, c)
#define joined_by(l,r)          AskP   (dbGetObject (l), 8, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define joined_byAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 8, BuildObjectList); }
#define joined_bySetP(l,r,p)    SetP   (dbGetObject (l), 8, dbGetObject (r), p)
#define joined_bySet(l,r)       SetP   (dbGetObject (l), 8, dbGetObject (r), NO_PARAMETER)
#define joined_byUnSet(l,r)     UnSet  (dbGetObject (l), 8, dbGetObject (r))
#define joined_byGet(l,r)       GetP   (dbGetObject (l), 8, dbGetObject (r))

#define Relation_uses      9
#define usesP(l,r,p,c)     AskP   (dbGetObject (l), 9, dbGetObject (r), p, c)
#define uses(l,r)          AskP   (dbGetObject (l), 9, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define usesAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 9, BuildObjectList); }
#define usesSetP(l,r,p)    SetP   (dbGetObject (l), 9, dbGetObject (r), p)
#define usesSet(l,r)       SetP   (dbGetObject (l), 9, dbGetObject (r), NO_PARAMETER)
#define usesUnSet(l,r)     UnSet  (dbGetObject (l), 9, dbGetObject (r))
#define usesGet(l,r)       GetP   (dbGetObject (l), 9, dbGetObject (r))

#define Relation_livesIn      10
#define livesInP(l,r,p,c)     AskP   (dbGetObject (l), 10, dbGetObject (r), p, c)
#define livesIn(l,r)          AskP   (dbGetObject (l), 10, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define livesInAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 10, BuildObjectList); }
#define livesInSetP(l,r,p)    SetP   (dbGetObject (l), 10, dbGetObject (r), p)
#define livesInSet(l,r)       SetP   (dbGetObject (l), 10, dbGetObject (r), NO_PARAMETER)
#define livesInUnSet(l,r)     UnSet  (dbGetObject (l), 10, dbGetObject (r))
#define livesInGet(l,r)       GetP   (dbGetObject (l), 10, dbGetObject (r))

#define Relation_break      11
#define breakP(l,r,p,c)     AskP   (dbGetObject (l), 11, dbGetObject (r), p, c)
#define break_(l,r)         AskP   (dbGetObject (l), 11, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define breakAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 11, BuildObjectList); }
#define breakSetP(l,r,p)    SetP   (dbGetObject (l), 11, dbGetObject (r), p)
#define breakSet(l,r)       SetP   (dbGetObject (l), 11, dbGetObject (r), NO_PARAMETER)
#define breakUnSet(l,r)     UnSet  (dbGetObject (l), 11, dbGetObject (r))
#define breakGet(l,r)       GetP   (dbGetObject (l), 11, dbGetObject (r))

#define Relation_hurt      12
#define hurtP(l,r,p,c)     AskP   (dbGetObject (l), 12, dbGetObject (r), p, c)
#define hurt(l,r)          AskP   (dbGetObject (l), 12, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define hurtAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 12, BuildObjectList); }
#define hurtSetP(l,r,p)    SetP   (dbGetObject (l), 12, dbGetObject (r), p)
#define hurtSet(l,r)       SetP   (dbGetObject (l), 12, dbGetObject (r), NO_PARAMETER)
#define hurtUnSet(l,r)     UnSet  (dbGetObject (l), 12, dbGetObject (r))
#define hurtGet(l,r)       GetP   (dbGetObject (l), 12, dbGetObject (r))

#define Relation_sound      13
#define soundP(l,r,p,c)     AskP   (dbGetObject (l), 13, dbGetObject (r), p, c)
#define sound(l,r)          AskP   (dbGetObject (l), 13, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define soundAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 13, BuildObjectList); }
#define soundSetP(l,r,p)    SetP   (dbGetObject (l), 13, dbGetObject (r), p)
#define soundSet(l,r)       SetP   (dbGetObject (l), 13, dbGetObject (r), NO_PARAMETER)
#define soundUnSet(l,r)     UnSet  (dbGetObject (l), 13, dbGetObject (r))
#define soundGet(l,r)       GetP   (dbGetObject (l), 13, dbGetObject (r))

#define Relation_opens      14
#define opensP(l,r,p,c)     AskP   (dbGetObject (l), 14, dbGetObject (r), p, c)
#define opens(l,r)          AskP   (dbGetObject (l), 14, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define opensAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 14, BuildObjectList); }
#define opensSetP(l,r,p)    SetP   (dbGetObject (l), 14, dbGetObject (r), p)
#define opensSet(l,r)       SetP   (dbGetObject (l), 14, dbGetObject (r), NO_PARAMETER)
#define opensUnSet(l,r)     UnSet  (dbGetObject (l), 14, dbGetObject (r))
#define opensGet(l,r)       GetP   (dbGetObject (l), 14, dbGetObject (r))

#define Relation_toolRequires      15
#define toolRequiresP(l,r,p,c)     AskP   (dbGetObject (l), 15, dbGetObject (r), p, c)
#define toolRequires(l,r)          AskP   (dbGetObject (l), 15, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define toolRequiresAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 15, BuildObjectList); }
#define toolRequiresSetP(l,r,p)    SetP   (dbGetObject (l), 15, dbGetObject (r), p)
#define toolRequiresSet(l,r)       SetP   (dbGetObject (l), 15, dbGetObject (r), NO_PARAMETER)
#define toolRequiresUnSet(l,r)     UnSet  (dbGetObject (l), 15, dbGetObject (r))
#define toolRequiresGet(l,r)       GetP   (dbGetObject (l), 15, dbGetObject (r))

#define Relation_taxi      16
#define taxiP(l,r,p,c)     AskP   (dbGetObject (l), 16, dbGetObject (r), p, c)
#define taxi(l,r)          AskP   (dbGetObject (l), 16, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define taxiAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 16, BuildObjectList); }
#define taxiSetP(l,r,p)    SetP   (dbGetObject (l), 16, dbGetObject (r), p)
#define taxiSet(l,r)       SetP   (dbGetObject (l), 16, dbGetObject (r), NO_PARAMETER)
#define taxiUnSet(l,r)     UnSet  (dbGetObject (l), 16, dbGetObject (r))
#define taxiGet(l,r)       GetP   (dbGetObject (l), 16, dbGetObject (r))

#define Relation_learned      17
#define learnedP(l,r,p,c)     AskP   (dbGetObject (l), 17, dbGetObject (r), p, c)
#define learned(l,r)          AskP   (dbGetObject (l), 17, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define learnedAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 17, BuildObjectList); }
#define learnedSetP(l,r,p)    SetP   (dbGetObject (l), 17, dbGetObject (r), p)
#define learnedSet(l,r)       SetP   (dbGetObject (l), 17, dbGetObject (r), NO_PARAMETER)
#define learnedUnSet(l,r)     UnSet  (dbGetObject (l), 17, dbGetObject (r))
#define learnedGet(l,r)       GetP   (dbGetObject (l), 17, dbGetObject (r))

#define Relation_remember      18
#define rememberP(l,r,p,c)     AskP   (dbGetObject (l), 18, dbGetObject (r), p, c)
#define remember(l,r)          AskP   (dbGetObject (l), 18, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define rememberAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 18, BuildObjectList); }
#define rememberSetP(l,r,p)    SetP   (dbGetObject (l), 18, dbGetObject (r), p)
#define rememberSet(l,r)       SetP   (dbGetObject (l), 18, dbGetObject (r), NO_PARAMETER)
#define rememberUnSet(l,r)     UnSet  (dbGetObject (l), 18, dbGetObject (r))
#define rememberGet(l,r)       GetP   (dbGetObject (l), 18, dbGetObject (r))

#define Relation_hasLootBag      19
#define hasLootBagP(l,r,p,c)     AskP   (dbGetObject (l), 19, dbGetObject (r), p, c)
#define hasLootBag(l,r)          AskP   (dbGetObject (l), 19, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define hasLootBagAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 19, BuildObjectList); }
#define hasLootBagSetP(l,r,p)    SetP   (dbGetObject (l), 19, dbGetObject (r), p)
#define hasLootBagSet(l,r)       SetP   (dbGetObject (l), 19, dbGetObject (r), NO_PARAMETER)
#define hasLootBagUnSet(l,r)     UnSet  (dbGetObject (l), 19, dbGetObject (r))
#define hasLootBagGet(l,r)       GetP   (dbGetObject (l), 19, dbGetObject (r))

#define Relation_PersonWorksHere      20
#define PersonWorksHereP(l,r,p,c)     AskP   (dbGetObject (l), 20, dbGetObject (r), p, c)
#define PersonWorksHere(l,r)          AskP   (dbGetObject (l), 20, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define PersonWorksHereAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 20, BuildObjectList); }
#define PersonWorksHereSetP(l,r,p)    SetP   (dbGetObject (l), 20, dbGetObject (r), p)
#define PersonWorksHereSet(l,r)       SetP   (dbGetObject (l), 20, dbGetObject (r), NO_PARAMETER)
#define PersonWorksHereUnSet(l,r)     UnSet  (dbGetObject (l), 20, dbGetObject (r))
#define PersonWorksHereGet(l,r)       GetP   (dbGetObject (l), 20, dbGetObject (r))


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
#define Object_Person_Size         sizeof(struct _Person)
struct _Person {
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
};
typedef struct _Person *Person;

#define Object_Player              9901L
#define Object_Player_Size         sizeof(struct _Player)
struct _Player {
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
};
typedef struct _Player *Player;

#define Object_Car              9902L
#define Object_Car_Size         sizeof(struct _Car)
struct _Car {
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
};
typedef struct _Car *Car;

#define Object_Location              9903L
#define Object_Location_Size         sizeof(struct _Location)
struct _Location {
	uint32 LocationNr;
	uint16 OpenFromMinute;
	uint16 OpenToMinute;
};
typedef struct _Location *Location;

#define Object_Ability              9904L
#define Object_Ability_Size         sizeof(struct _Ability)
struct _Ability {
	enum AbilityE Name;
	uint32 Use;
};
typedef struct _Ability *Ability;

#define Object_Item              9905L
#define Object_Item_Size         sizeof(struct _Item)
struct _Item {
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
};
typedef struct _Item *Item;

#define Object_Tool              9906L
#define Object_Tool_Size         sizeof(struct _Tool)
struct _Tool {
	uint16 PictID;
	uint32 Value;
	uint8 Danger;
	uint8 Volume;
	uint8 Effect;
};
typedef struct _Tool *Tool;

#define Object_Environment              9907L
#define Object_Environment_Size         sizeof(struct _Environment)
struct _Environment {
	uint8 MattHasHotelRoom;
	uint8 MattHasIdentityCard;
	uint8 WithOrWithoutYou;
	uint8 MattIsInLove;
	uint8 SouthhamptonHappened;
	uint8 Present;
	uint8 FirstTimeInSouth;
	uint8 PostzugDone;                     /* PROFIDISK */
};
typedef struct _Environment *Environment;

#define Object_London              9908L
#define Object_London_Size         sizeof(struct _London)
struct _London {
	uint8 Useless;
};
typedef struct _London *London;

#define Object_Evidence              9909L
#define Object_Evidence_Size         sizeof(struct _Evidence)
struct _Evidence {
	uint32 pers;
	uint8 Recognition;
	uint8 WalkTrail;
	uint8 WaitTrail;
	uint8 WorkTrail;
	uint8 KillTrail;
	uint8 CallTrail;
	uint8 PaperTrail;
	uint8 FotoTrail;
};
typedef struct _Evidence *Evidence;

#define Object_Loot              9910L
#define Object_Loot_Size         sizeof(struct _Loot)
struct _Loot {
	enum LootE Type;
	enum LootNameE Name;
	uint32 Volume;
	uint16 Weight;
	uint16 PictID;

};
typedef struct _Loot *Loot;

#define Object_CompleteLoot              9911L
#define Object_CompleteLoot_Size         sizeof(struct _CompleteLoot)
struct _CompleteLoot {
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
};
typedef struct _CompleteLoot *CompleteLoot;

#define Object_LSOLock              9912L
#define Object_LSOLock_Size         sizeof(struct _LSOLock)
struct _LSOLock {
	enum LockE Type;
};
typedef struct _LSOLock *LSOLock;

#define Object_LSObject              9913L
#define Object_LSObject_Size         sizeof(struct _LSObject)
struct _LSObject {
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
};
typedef struct _LSObject *LSObject;




/*
 * Data header file created with dc
 * (c) 1993 by ...and avoid panic by
 *
 * Database (offset, size) : tcBuild (500000, 10000)
 */

#define DB_tcBuild_OFFSET        500000
#define DB_tcBuild_SIZE          10000


#define Relation_isGuardedby      500003
#define isGuardedbyP(l,r,p,c)     AskP   (dbGetObject (l), 500003, dbGetObject (r), p, c)
#define isGuardedby(l,r)          AskP   (dbGetObject (l), 500003, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define isGuardedbyAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 500003, BuildObjectList); }
#define isGuardedbySetP(l,r,p)    SetP   (dbGetObject (l), 500003, dbGetObject (r), p)
#define isGuardedbySet(l,r)       SetP   (dbGetObject (l), 500003, dbGetObject (r), NO_PARAMETER)
#define isGuardedbyUnSet(l,r)     UnSet  (dbGetObject (l), 500003, dbGetObject (r))
#define isGuardedbyGet(l,r)       GetP   (dbGetObject (l), 500003, dbGetObject (r))

#define Relation_startsWith      500001
#define startsWithP(l,r,p,c)     AskP   (dbGetObject (l), 500001, dbGetObject (r), p, c)
#define startsWith(l,r)          AskP   (dbGetObject (l), 500001, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define startsWithAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 500001, BuildObjectList); }
#define startsWithSetP(l,r,p)    SetP   (dbGetObject (l), 500001, dbGetObject (r), p)
#define startsWithSet(l,r)       SetP   (dbGetObject (l), 500001, dbGetObject (r), NO_PARAMETER)
#define startsWithUnSet(l,r)     UnSet  (dbGetObject (l), 500001, dbGetObject (r))
#define startsWithGet(l,r)       GetP   (dbGetObject (l), 500001, dbGetObject (r))

#define Relation_consistsOf      500002
#define consistsOfP(l,r,p,c)     AskP   (dbGetObject (l), 500002, dbGetObject (r), p, c)
#define consistsOf(l,r)          AskP   (dbGetObject (l), 500002, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define consistsOfAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 500002, BuildObjectList); }
#define consistsOfSetP(l,r,p)    SetP   (dbGetObject (l), 500002, dbGetObject (r), p)
#define consistsOfSet(l,r)       SetP   (dbGetObject (l), 500002, dbGetObject (r), NO_PARAMETER)
#define consistsOfUnSet(l,r)     UnSet  (dbGetObject (l), 500002, dbGetObject (r))
#define consistsOfGet(l,r)       GetP   (dbGetObject (l), 500002, dbGetObject (r))


enum RouteE { Innenstadt, Stadt, Landstrasse, Autobahn };


#define Object_Building              509990L
#define Object_Building_Size         sizeof(struct _Building)
struct _Building {
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
};
typedef struct _Building *Building;

#define Object_Police              509991L
#define Object_Police_Size         sizeof(struct _Police)
struct _Police {
	uint16 PictID;
	uint8 LivingID;
};
typedef struct _Police *Police;

#define Object_LSArea              509992L
#define Object_LSArea_Size         sizeof(struct _LSArea)
struct _LSArea {
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

};
typedef struct _LSArea *LSArea;

#define Object_LSRoom              509993L
#define Object_LSRoom_Size         sizeof(struct _LSRoom)
struct _LSRoom {
	uint16 us_LeftEdge;
	uint16 us_TopEdge;
	uint16 us_Width;
	uint16 us_Height;
};
typedef struct _LSRoom *LSRoom;

#endif                              /* MODULE_TCDATA */

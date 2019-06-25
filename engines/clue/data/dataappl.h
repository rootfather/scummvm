/*
 * dataAppl.h
 * (c) 1993 by Helmut Gaberschek & Kaweh Kazemi, ...and avoid panic by
 * All rights reserved.
 *
 * new animation module for the PANIC-System
 *
 * Rev   Date        Comment
 *  1   05-09-93
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_DATAAPPL
#define MODULE_DATAAPPL

#include "clue/theclou.h"

#ifndef MODULE_LIST
#include "clue/list/list.h"
#endif

#ifndef MODULE_TEXT
#include "clue/text/text.h"
#endif

#ifndef MODULE_RELATION
#include "clue/data/relation.h"
#endif

#ifndef MODULE_DATABASE
#include "clue/data/database.h"
#endif

#ifndef MODULE_DATACALC
#include "clue/data/datacalc.h"
#endif

#ifndef MODULE_DATAAPPL
#include "clue/data/dataapplh"
#endif

#include "clue/data/objstd/tcdata.h"

#ifndef MODULE_EVIDENCE
#include "clue/scenes/evidence.h"
#endif

#ifndef MODULE_LIVING
#include "clue/living/living.h"
#endif

#ifndef MODULE_LANDSCAP
#include "clue/landscap/landscap.h"
#endif


extern bool tcSpendMoney(uint32 money, uint8 breakAnim);

extern uint32 GetObjNrOfLocation(uint32 LocNr);
extern uint32 GetObjNrOfBuilding(uint32 LocNr);

extern void tcCalcCallValue(uint32 callNr, uint32 timer, uint32 persId);
extern void tcPersonLearns(uint32 pId);
extern void tcRefreshTimeClock(uint32 buildId, uint32 timerId);
extern uint32 tcGetItemID(uint32 itemType);
extern uint32 tcGetPersOffer(Person person, uint8 persCount);

extern int32 tcCalcEscapeTime(void);
extern bool tcKillTheGuard(uint32 guyId, uint32 buildingId);
extern bool tcAlarmByPowerLoss(uint32 powerId);
extern bool tcAlarmByTouch(uint32 lsoId);
extern bool tcGuardDetectsGuy(LIST * roomsList, uint16 us_XPos, uint16 us_YPos,
			      uint8 uch_ViewDirection, char *puch_GuardName,
			      char *puch_LivingName);
extern int32 tcGetCarStrike(Car car);
extern int32 tcCalcEscapeOfTeam(void);
extern uint32 tcGuyTellsAll(Person p);
extern uint32 tcGuyCanEscape(Person p);
extern int32 tcGetCarTraderOffer(Car car);
extern int32 tcGetTeamMood(uint32 * guyId, uint32 timer);	/* ptr auf 4 U32s */
extern int32 tcGuyInAction(uint32 persId, int32 exhaustion);
extern int32 tcGuyIsWaiting(uint32 persId, int32 exhaustion);
extern int32 tcIsPlanPerfect(uint32 timer);
extern int32 tcGetTrail(Person p, uint8 which);
extern int32 tcGetDanger(uint32 persId, uint32 toolId, uint32 itemId);
extern int32 tcGetToolLoudness(uint32 persId, uint32 toolId, uint32 itemId);
extern int32 tcGetWalkLoudness(void);
extern int32 tcGetTotalLoudness(int32 loudp0, int32 loudp1, int32 loudp2, int32 loudp3);
extern bool tcAlarmByLoudness(Building b, int32 totalLoudness);
extern bool tcAlarmByRadio(Building b);
extern bool tcAlarmByMicro(uint16 us_XPos, uint16 us_YPos, int32 loudness);
extern bool tcAlarmByPatrol(uint16 objChangedCount, uint16 totalCount,
			    uint8 patrolCount);
extern bool tcWatchDogWarning(uint32 persId);
extern bool tcWrongWatchDogWarning(uint32 persId);
extern int32 tcGetGuyState(uint32 persId);
extern bool tcIsCarRecognised(Car car, uint32 time);
extern bool tcGuardChecksObject(LSObject lso);
extern bool tcCheckTimeClocks(uint32 builId);

extern int32 tcCalcMattsPart(void);

extern uint32 tcGuyEscapes(void);
extern uint32 tcGuyUsesTool(uint32 persId, Building b, uint32 toolId, uint32 itemId);
extern uint32 tcGuyUsesToolInPlayer(uint32 persId, Building b, uint32 toolId, uint32 itemId,
				 uint32 neededTime);

extern void tcInsertGuard(LIST * list, LIST * roomsList, uint16 x, uint16 y,
			  uint16 width, uint16 height, uint32 guardId, uint8 livId,
			  uint32 areaId);

/* defines for Persons */
#define   tcPERSON_IS_ARRESTED          240	/* KnownToPolice */
#define   tcVALUE_OF_RING_OF_PATER      320

#define   tcSetPersKnown(p,v)           ((p)->Known = (v))
#define   tcChgPersPopularity(p, v)     ((p)->Popularity = (uint8) ChangeAbs((p)->Popularity, v, 0, 255))

#define   tcGetPersHealth(p)            ((p)->Health)
#define   tcGetPersMood(p)              (((Person)p)->Mood)

#define   tcWeightPersCanCarry(p)       (((int32)(p)->Stamina + (int32)(p)->Strength) * 200)	/* 0 - 100000 (100 kg) in gramm ! */
#define   tcVolumePersCanCarry(p)       (((int32)(p)->Stamina + (int32)(p)->Strength) * 200)	/* in cm3 -> max 1 m3 fr eine Person */

#define   tcImprovePanic(p, v)          ((p)->Panic = (p)->Panic - ((p)->Panic / (v)))
#define   tcImproveKnown(p, v)          ((p)->Known = MIN(255, (v)))

/* defines for object Car */
#define   tcENGINE_PERC                 50	/* 50 % of costs of a car is engine   */
#define   tcBODY_PERC                   40	/* 40 % of costs of a car is bodywork */
#define   tcTYRE_PERC                   10	/* 10 % of costs of a car are tyres   */

#define   tcCalcCarState(car)           (car->State = ( (int32)(car->MotorState) + (int32)(car->BodyWorkState) + (int32)(car->TyreState)) /3)

#define   tcCostsPerEngineRepair(car)   (MAX(((car->Value * tcENGINE_PERC) / 25500), 2u))
#define   tcCostsPerBodyRepair(car)     (MAX(((car->Value * tcBODY_PERC  ) / 25500), 2u))
#define   tcCostsPerTyreRepair(car)     (MAX(((car->Value * tcTYRE_PERC  ) / 25500), 1u))
#define   tcCostsPerTotalRepair(car)    (MAX(((car->Value) / 255), 5u))
#define   tcColorCosts(car)             (MAX((Round(car->Value / 200,1)), 1))

#define   tcGetCarSpeed(car)            (car->Speed)
#define   tcGetCarPS(car)               (car->PS)

#define   tcRGetCarAge(car)             (MAX((GetDay /365) - (car->YearOfConstruction), 1u))
#define   tcRGetCarValue(car)           (car->Value)

#define   tcGetCarPrice(car)            (Round(MAX(((car->Value) * (car->State)) / 255, 100u), 1))
#define   tcGetCarTotalState(car)       (((int32)car->MotorState + (int32)car->BodyWorkState + (int32)car->TyreState) / 3)

#define   tcGetDealerPerc(d, p)         (MIN((p + (((int32)p * ((int32)127L - (int32)d->Known)) / (int32)1270)), 255))
#define   tcGetDealerOffer(v, p)        (((int32)v * (int32)p) / (int32)255)
#define   tcAddDealerSymp(d, v)         ((d)->Known = ChangeAbs ((d)->Known, v, 0, 255))

#define   tcSetCarMotorState(car,v)     {car->MotorState = (uint8)ChangeAbs((int32)car->MotorState,(int32)v, 0, 255); tcCalcCarState(car);}
#define   tcSetCarBodyState(car,v)      {car->BodyWorkState = (uint8)ChangeAbs((int32)car->BodyWorkState,(int32)v, 0, 255); tcCalcCarState(car);}
#define   tcSetCarTyreState(car,v)      {car->TyreState = (uint8)ChangeAbs((int32)car->TyreState,(int32)v, 0, 255); tcCalcCarState(car);}

/* defines for object Player */
#define   tcGetPlayerMoney              (((Player)dbGetObject(Player_Player_1))->Money)
#define   tcSetPlayerMoney(amount)      (((Player)dbGetObject(Player_Player_1))->Money = amount)
#define   tcAddPlayerMoney(amount)      (((Player)dbGetObject(Player_Player_1))->Money = (tcGetPlayerMoney + amount))

/* defines for object Building */
#define   tcRGetGRate(bui)              (bui->GRate)
#define   tcRGetBuildStrike(bui)        (bui->Strike)

#define   tcGetBuildPoliceT(bui)        (bui->PoliceTime)
#define   tcGetBuildGRate(bui)          (bui->GRate)

uint32 tcGetBuildValues(Building bui);

#define   tcAddBuildExactlyness(bui,v)  (bui->Exactlyness = (uint8)ChangeAbs((int32)bui->Exactlyness,(int32)v, 0, 255))
#define   tcAddBuildStrike(bui,v)       (bui->Strike = (uint8)ChangeAbs((int32)bui->Strike,(int32)v, 0, 255))

/* defines for object Tool */
#define   tcRGetDanger(tool)            (tool->Danger)
#define   tcRGetVolume(tool)            (tool->Volume)

#define   tcGetToolPrice(tool)          (tool->Value)
#define   tcGetToolTraderOffer(tool)    (MAX((tcGetToolPrice(tool) - (tcGetToolPrice(tool) / tcDerivation(10,20))), 1u))

/* costs */
#define   tcCOSTS_FOR_HOTEL             10

/* Plan */
#define   tcEscapeTime(building)        (16400 / building->EscapeRouteLength)


#endif

/*
**  $Filename: story/story.h
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-02-94
**
**  story functions for "Der Clou!"
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

#ifndef MODULE_STORY
#define MODULE_STORY

#include "clue/text.h"

namespace Clue {

#define tcLAST_BURGLARY_LEFT_CTRL_OBJ   440221
#define tcLAST_BURGLARY_RIGHT_CTRL_OBJ  440228

#define PHONE_PICTID                 144
#define LETTER_PICTID             151

#define SCENE_NEW_GAME        45350912
#define SCENE_CREDITS         46399488

#define SCENE_GARAGE          16777216
#define SCENE_PARKING         17039360
#define SCENE_CARS_OFFICE      2621440
#define SCENE_TOOLS            3145728

#define SCENE_PARKER          19398656
#define SCENE_MALOYA          17825792
#define SCENE_POOLY           19136512

#define SCENE_TOWER_OUT       15204352
#define SCENE_AT_SABIEN       45088768
#define SCENE_FREE_TRAVEL     21233664
#define SCENE_ARRIVAL         22282240
#define SCENE_STATION         21495808
#define SCENE_HOTEL_REC       18612224
#define SCENE_HOTEL_1ST_TIME  22544384
#define SCENE_DANNER          22806528
#define SCENE_GLUDO_MONEY     23068672
#define SCENE_HOLLAND_STR     20185088
#define SCENE_HOTEL           18612224
#define SCENE_LISSON_GROVE    33292288
#define SCENE_CARS_VANS       1048576
#define SCENE_HOTEL_ROOM      2097152
#define SCENE_FAT_MANS        2359296
#define SCENE_MAMI_CALLS      23855104
#define SCENE_FST_MEET_BRIGGS 24117248
#define SCENE_CALL_FROM_POOLY 31457280
#define SCENE_WALRUS          16515072
#define SCENE_WATLING         20971520
#define SCENE_THE_END          3932160
#define SCENE_FAHNDUNG        24641536
#define SCENE_1ST_BURG        24903680
#define SCENE_GLUDO_SAILOR    25165824
#define SCENE_CALL_BRIGGS     25427968
#define SCENE_2ND_BURG        25690112
#define SCENE_3RD_BURG        25952256
#define SCENE_4TH_BURG        26214400
#define SCENE_5TH_BURG        26476544
#define SCENE_6TH_BURG        28573696
#define SCENE_7TH_BURG        28835840
#define SCENE_8TH_BURG        29097984
#define SCENE_9TH_BURG        29360128
#define SCENE_POLICE           3407872
#define SCENE_ARRESTED_MATT   26738688
#define SCENE_POOLY_AFRAID    27787264
#define SCENE_MALOYA_AFRAID   28049408
#define SCENE_PARKER_AFRAID   28311552
#define SCENE_RAID            30932992
#define SCENE_DART            31195136
#define SCENE_MEETING_AGAIN   38273024

#define SCENE_GLUDO_BURNS     27000832
#define SCENE_MORNING         31981568
#define SCENE_VISITING        32243712
#define SCENE_A_DREAM         35127296
#define SCENE_ROSENBLATT      35389440
#define SCENE_BRIGGS_ANGRY    35651584
#define SCENE_SABIEN_WALRUS   35913728
#define SCENE_SABIEN_DINNER   36175872
#define SCENE_TOMBOLA         36438016
#define SCENE_PRESENT_HOTEL   36700160
#define SCENE_INFO_TOWER      36962304
#define SCENE_RAINY_EVENING   37224448

#define SCENE_SABIEN_CALL     38010880
#define SCENE_BIRTHDAY        38797312
#define SCENE_WALK_WITH       39059456
#define SCENE_AGENT           39321600
#define SCENE_JAGUAR          44302336
#define SCENE_THINK_OF        40370176
#define SCENE_TERROR          40108032
#define SCENE_CONFESSING      40632320

#define SCENE_PRISON          44826624

/* #define SCENE_SOUTH_WITH */
#define SCENE_SOUTH_WITHOUT   44040192
#define SCENE_SOUTH_UNKNOWN   41418752
#define SCENE_SOUTHHAMPTON    43778048

#define SCENE_MISSED_DATE_0   37748736
#define SCENE_MISSED_DATE_1   43515904

#define SCENE_VILLA_INSIDE     8650752
#define SCENE_KENWO_INSIDE     9175040

#define SCENE_TANTE_INSIDE     5242880
#define SCENE_JUWEL_INSIDE     5767168
#define SCENE_ANTIQ_INSIDE     6291456
#define SCENE_TRANS_INSIDE     6815744
#define SCENE_SENIO_INSIDE     7340032
#define SCENE_KENWO_INSIDE     9175040
#define SCENE_HAMHO_INSIDE     9699328
#define SCENE_OSTER_INSIDE    10223616
#define SCENE_CHISW_INSIDE    10747904
#define SCENE_SOTHE_INSIDE    11796480
#define SCENE_BRITI_INSIDE    12320768
#define SCENE_NATUR_INSIDE    12845056
#define SCENE_NATIO_INSIDE    13369344
#define SCENE_VICTO_INSIDE    13893632
#define SCENE_BANKO_INSIDE    14417920
#define SCENE_TOWER_INSIDE    14942208

#define SCENE_BARRACKS_INSIDE  47185920
#define SCENE_BARRACKS_OUTSIDE 47448064

#define SCENE_PROFI_21        52428800
#define SCENE_PROFI_22        51380224
#define SCENE_PROFI_23        48496640
#define SCENE_PROFI_24        51904512
#define SCENE_PROFI_25        47972352
#define SCENE_PROFI_26        50855936
#define SCENE_PROFI_27        50069504
#define SCENE_PROFI_28        50593792

#define SCENE_PROFI_21_INSIDE 52166656
#define SCENE_PROFI_22_INSIDE 51118080
#define SCENE_PROFI_23_INSIDE 48234496
#define SCENE_PROFI_24_INSIDE 51642368
#define SCENE_PROFI_25_INSIDE 47710208
#define SCENE_PROFI_26_INSIDE   /* NO TRAIN INSIDE */
#define SCENE_PROFI_27_INSIDE 49807360
#define SCENE_PROFI_28_INSIDE 50331648

#define FACE_GLUDO_SAILOR     126
#define FACE_GLUDO_MAGIC      124

byte tcKarateGrandpa(uint32 ul_ActionTime, uint32 ul_BuildingId);

void tcTheAlmighty(uint32 time);
void tcMattGoesTo(uint32 locNr);
void tcMovePersons(uint32 personCount, uint32 time);
void tcMoveAPerson(uint32 persID, uint32 newLocID);
void tcAsTimeGoesBy(uint32 untilMinute);
void tcAsDaysGoBy(uint32 day, uint32 stepSize);

void tcDoneFreeTicket();
void tcDoneMamiCalls();
void tcDoneArrival();
void tcDoneHotelReception();
void tcDoneDanner();
void tcDoneGludoMoney();
void tcDoneMeetBriggs();
void tcDoneAfterMeetBriggs();
void tcDone1stBurglary();
void tcDoneGludoAsSailor();
void tcDoneCallFromBriggs();
void tcDone2ndBurglary();
void tcDone3rdBurglary();
void tcDone4thBurglary();
void tcDone5thBurglary();
void tcDone6thBurglary();
void tcDone7thBurglary();
void tcDone8thBurglary();
void tcDone9thBurglary();
void tcDoneMattIsArrested();
void tcDoneGludoBurnsOffice();
void tcDoneDealerIsAfraid();
void tcDoneDartJager();
void tcDoneRaidInWalrus();
void tcDoneBeautifullMorning();
void tcDoneVisitingSabien();
void tcDoneADream();
void tcDoneCallFromPooly();
void tcMeetingRosenblatt();
void tcBriggsAngry();
void tcSabienInWalrus();
void tcSabienDinner();
void tcWalrusTombola();
void tcPresentInHotel();
void tcPoliceInfoTower();
void tcRainyEvening();
void tcDoneMissedDate();
void tcDoneSabienCall();
void tcDoneBirthday();
void tcWalkWithSabien();
void tcDoneAgent();
void tcDoneGoAndFetchJaguar();
void tcDoneThinkOfSabien();
void tcDoneTerror();
void tcDoneConfessingSabien();
void tcDoneSouthhamptonWithSabien();
void tcDoneSouthhamptonWithoutSabien();
void tcDoneSouthhamptonSabienUnknown();
void tcDoneSouthhampton();
void tcDonePrison();
void tcDoneMeetingAgain();
void tcInitTowerBurglary();
void tcDoneCredits();
void tcDoneMafia();
void tcDoneBarracks();
void tcInitPrison();

int32 tcIsLastBurglaryOk();
int32 tcDoTowerBurglary();
bool tcIsDeadlock();
void tcCheckForBones();

uint32 tcLastBurglary();

void tcCheckForDowning();

} // End of namespace Clue

#endif

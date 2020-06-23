/*
**  $Filename: scenes/evidence.h
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-04-94
**
** functions for evidence for "Der Clou!"
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

#ifndef MODULE_EVIDENCE
#define MODULE_EVIDENCE

#include "clue/text.h"
#include "clue/present/present.h"

namespace Clue {

#define FAHN_NOT_ESCAPED      1
#define FAHN_ESCAPED          2

#define FAHN_ALARM            (1)
#define FAHN_ALARM_LOUDN      (1<< 1)
#define FAHN_ALARM_RADIO      (1<< 2)
#define FAHN_ALARM_PATRO      (1<< 3)
#define FAHN_SURROUNDED       (1<< 4)
#define FAHN_ESCAPE           (1<< 5)
#define FAHN_QUIET_ALARM      (1<< 6)
#define FAHN_STD_ESCAPE       (1<< 7) /* wenn Einbruch glatt geht (Standardflucht) */
#define FAHN_ALARM_ALARM      (1<< 8)
#define FAHN_ALARM_TIMECLOCK  (1<< 9)
#define FAHN_ALARM_POWER      (1<<10)
#define FAHN_ALARM_TIMER      (1<<11)
#define FAHN_ALARM_MICRO      (1<<12)
#define FAHN_ALARM_GUARD      (1<<13)

struct Search {         /* wegen Arrays nicht in objects */
	int16 GuyXPos[4], GuyYPos[4];   /* letzte Position im Falle einer Flucht! */

	byte Exhaust[4];

	uint16 WalkTime[4];
	uint16 WaitTime[4];
	uint16 WorkTime[4];
	uint16 KillTime[4];

	uint32 DeriTime;        /* Abweichung vom Soll */

	uint32 TimeOfBurglary;      /* Zeitpunkt! */
	uint32 TimeOfAlarm;     /* Zeitpunkt! */

	uint32 BuildingId;      /* Einbruch */
	uint32 LastAreaId;      /* zum Zeitpunkt der Flucht */

	uint32 EscapeBits;

	int16 CallValue;        /* "Wert" der Funkspr., int16!!! */
	uint16 CallCount;       /* Anzahl d. FUnkspr. */

	uint16 WarningCount;        /* Number of warnings */
	byte SpotTouchCount[4];

	byte BarracksOk;     /* Whether the barracks were done! */
};

extern int32 tcCalcCarEscape(int32 timeLeft);
extern void tcForgetGuys();

extern Search Search;

} // End of namespace Clue

#endif

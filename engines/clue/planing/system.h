/*
**      $Filename: planing/system.h
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.system interface for "Der Clou!"
**
** (c) 1994 ...and avoid panic by, Kaweh Kazemi
**      All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_PLANING_SYSTEM
#define MODULE_PLANING_SYSTEM

#include "clue/planing/main.h"

namespace Clue {

/* System main structure - like the kernel in an OS */
struct System {
	List *Handlers;     /* Pointer to all started handlers */
	List *Signals;      /* Pointer to signal in the system */

	Node *ActivHandler;
};

struct System *InitSystem();    /* Initialize system for use */
void CloseSystem(struct System *sys);   /* Close all system immedietly */
void SetActivHandler(struct System *sys, uint32 id);

void SaveSystem(Common::Stream *fh, struct System *sys);
NewList<NewNode> *LoadSystem(Common::Stream *fh, struct System *sys);


/* System Handler Flags */
#define SHF_NORMAL              0
#define SHF_AUTOREVERS          1

/* Handler structure - like a task in an OS */
struct Handler {
	Node Link;          /* Link to next handler */

	uint32 Id;          /* ID of handler (all handlers will be identified with their ID and
                   not through pointers, which will save global data) */

	uint32 Timer;           /* Handler time in seconds/3 */

	uint32 Flags;           /* Handler flags */

	List *Actions;      /* Action table */
	Node *CurrentAction;    /* Current action */
};


struct Handler *InitHandler(struct System *sys, uint32 id, uint32 flags);   /* Initialize handler         */
void CloseHandler(struct System *sys, uint32 id);   /* Close Handler              */
struct Handler *ClearHandler(struct System *sys, uint32 id);    /* Clear Handlers action list */
struct Handler *FindHandler(struct System *sys, uint32 id);

bool IsHandlerCleared(struct System *sys);

void SaveHandler(Common::Stream *fh, struct System *sys, uint32 id);
bool LoadHandler(Common::Stream *fh, struct System *sys, uint32 id);

/* here we are at the real part - the actions */
#define ACTION_GO                1
#define ACTION_WAIT              2
#define ACTION_SIGNAL            3
#define ACTION_WAIT_SIGNAL       4
#define ACTION_USE               5
#define ACTION_TAKE              6
#define ACTION_DROP              7
#define ACTION_OPEN              8
#define ACTION_CLOSE             9
#define ACTION_CONTROL          10

struct Action {
	Node Link;

	uint16 Type;

	uint16 TimeNeeded;      /* times in seconds/3 */
	uint16 Timer;
};

/* Type : ACTION_GO
   Figure will go in one direction for x steps */
struct ActionGo {
	uint16 Direction;
};

#define DIRECTION_NO    0
#define DIRECTION_LEFT  1
#define DIRECTION_RIGHT 2
#define DIRECTION_UP    4
#define DIRECTION_DOWN  8


/* Type : ACTION_WAIT
   Figure waits for x seconds
   Does not need an extended data structure */

/* Type : ACTION_SIGNAL
   Figure sends out a signal of a special type to a receiver */
struct ActionSignal {
	uint32 ReceiverId;
};

/* Type : ACTION_WAIT_SIGNAL
   Figure waits until it receives a signal of a special type from a special sender */
struct ActionWaitSignal {
	uint32 SenderId;
};

/* Type : ACTION_USE */
struct ActionUse {
	uint32 ItemId;
	uint32 ToolId;
};

/* Type : ACTION_TAKE */
struct ActionTake {
	uint32 ItemId;
	uint32 LootId;
};

/* Type : ACTION_DROP */
struct ActionDrop {
	uint32 ItemId;
	uint32 LootId;
};

/* Type : ACTION_OPEN */
struct ActionOpen {
	uint32 ItemId;
};

/* Type : ACTION_CLOSE */
struct ActionClose {
	uint32 ItemId;
};

/* Type : ACTION_CONTROL */
struct ActionControl {
	uint32 ItemId;
};


#define ActionData(ac,type)      ((type)(ac+1))

struct Action *InitAction(struct System *sys, uint16 type, uint32 data1, uint32 data2,
                          uint32 time);
struct Action *CurrentAction(struct System *sys);
struct Action *GoFirstAction(struct System *sys);
struct Action *GoLastAction(struct System *sys);
struct Action *NextAction(struct System *sys);
struct Action *PrevAction(struct System *sys);
bool ActionStarted(struct System *sys);
byte ActionEnded(struct System *sys);
void RemLastAction(struct System *sys);
void IgnoreAction(struct System *sys);


/* Signal structure - to make it possible to communicate between handlers, our small attempt of an message system */
#define SIGNAL_HURRY_UP    1    /* come on, get on */
#define SIGNAL_DONE        2    /* well, my work is done */
#define SIGNAL_ESCAPE      3    /* f..., police is coming, let's go */

struct plSignal {
	Node Link;

	uint32 SenderId;
	uint32 ReceiverId;
};

struct plSignal *InitSignal(struct System *sys, uint32 sender, uint32 receiver);
void CloseSignal(struct plSignal *sig);
struct plSignal *IsSignal(struct System *sys, uint32 sender, uint32 receiver);


uint32 CurrentTimer(struct System *sys);
void IncCurrentTimer(struct System *sys, uint32 time, byte alsoTime);
uint32 GetMaxTimer(struct System *sys);

void CorrectMem(List *l);

#if 0
void ResetMem();
size_t plGetUsedMem();
#endif
} // End of namespace Clue

#endif

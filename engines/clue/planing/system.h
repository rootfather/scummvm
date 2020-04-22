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
class ActionNode;
class HandlerNode;
class plSignalNode;

	/* System main structure - like the kernel in an OS */
class System {
public:
	NewList<HandlerNode> *Handlers;     /* Pointer to all started handlers */
	NewList<plSignalNode> *Signals;      /* Pointer to signal in the system */
	HandlerNode *ActivHandler;
};

System *InitSystem();    /* Initialize system for use */
void CloseSystem(System *sys);   /* Close all system immedietly */
void SetActivHandler(System *sys, uint32 id);

void SaveSystem(Common::Stream *fh, System *sys);
NewList<NewNode> *LoadSystem(Common::Stream *fh, System *sys);

/* System Handler Flags */
#define SHF_NORMAL              0
#define SHF_AUTOREVERS          1

/* Handler structure - like a task in an OS */
class HandlerNode : public NewNode {
public:
	uint32 Id;          /* ID of handler (all handlers will be identified with their ID and
                   not through pointers, which will save global data) */

	uint32 Timer;           /* Handler time in seconds/3 */

	uint32 Flags;           /* Handler flags */

	NewList<ActionNode> *Actions;      /* Action table */
	ActionNode *CurrentAction;    /* Current action */
};

HandlerNode *InitHandler(System *sys, uint32 id, uint32 flags);   /* Initialize handler         */
void CloseHandler(System *sys, uint32 id);   /* Close Handler              */
HandlerNode *ClearHandler(System *sys, uint32 id);    /* Clear Handlers action list */
HandlerNode *FindHandler(System *sys, uint32 id);

bool IsHandlerCleared(System *sys);

void SaveHandler(Common::Stream *fh, System *sys, uint32 id);
bool LoadHandler(Common::Stream *fh, System *sys, uint32 id);

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

class ActionNode : public NewNode {
public:
	uint16 Type;
	uint16 TimeNeeded;      /* times in seconds/3 */
	uint16 Timer;
};

/* Type : ACTION_GO
   Figure will go in one direction for x steps */
class ActionGoNode : public ActionNode {
public:
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
class ActionSignalNode : public ActionNode {
public:
	uint32 ReceiverId;
};

/* Type : ACTION_WAIT_SIGNAL
   Figure waits until it receives a signal of a special type from a special sender */
class ActionWaitSignalNode : public ActionNode {
public:
	uint32 SenderId;
};

/* Type : ACTION_USE */
class ActionUseNode : public ActionNode {
public:
	uint32 ItemId;
	uint32 ToolId;
};

/* Type : ACTION_TAKE */
class ActionTakeNode : public ActionNode {
public:
	uint32 ItemId;
	uint32 LootId;
};

/* Type : ACTION_DROP */
class ActionDropNode : public ActionNode {
public:
	uint32 ItemId;
	uint32 LootId;
};

/* Type : ACTION_OPEN */
class ActionOpenNode : public ActionNode {
public:
	uint32 ItemId;
};

/* Type : ACTION_CLOSE */
class ActionCloseNode : public ActionNode {
public:
	uint32 ItemId;
};

/* Type : ACTION_CONTROL */
class ActionControlNode : public ActionNode {
public:
	uint32 ItemId;
};

ActionNode *InitAction(System *sys, uint16 type, uint32 data1, uint32 data2, uint32 time);
ActionNode *CurrentAction(System *sys);
ActionNode *GoFirstAction(System *sys);
ActionNode *GoLastAction(System *sys);
ActionNode *NextAction(System *sys);
ActionNode *PrevAction(System *sys);
bool ActionStarted(System *sys);
bool ActionEnded(System *sys);
void RemLastAction(System *sys);
void IgnoreAction(System *sys);

/* Signal structure - to make it possible to communicate between handlers, our small attempt of an message system */
#define SIGNAL_HURRY_UP    1    /* come on, get on */
#define SIGNAL_DONE        2    /* well, my work is done */
#define SIGNAL_ESCAPE      3    /* f..., police is coming, let's go */

class plSignalNode : public NewNode {
public:
	uint32 SenderId;
	uint32 ReceiverId;
};

plSignalNode *InitSignal(System *sys, uint32 sender, uint32 receiver);
void CloseSignal(plSignalNode *sig);
plSignalNode *IsSignal(System *sys, uint32 sender, uint32 receiver);

uint32 CurrentTimer(System *sys);
void IncCurrentTimer(System *sys, uint32 time, bool alsoTime);
uint32 GetMaxTimer(System *sys);

} // End of namespace Clue

#endif

/*
**      $Filename: planing/system.c
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.system for "Der Clou!"
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

#include "clue/planing/system.h"

#include "clue/clue.h"
#include "clue/data/database_p.h"

#define FILE_SYSTEM_ID      "SYS "  /* SYStem plan start */
#define FILE_HANDLER_ID     "HAND"  /* HANDler x needed  */
#define FILE_ACTION_LIST_ID "ACLI"  /* ACtionLIst for handler x started */
#define FILE_ACTION_ID      "ACTI"  /* ACTIon */

#define SYS_MAX_MEMORY_SIZE   1024*25L

namespace Clue {

#if 0
size_t sizeofAction[] = {
	0,              /* first index is not used yet */
	sizeof(struct ActionGo),
	0,
	sizeof(struct ActionSignal),
	sizeof(struct ActionWaitSignal),
	sizeof(struct ActionUse),
	sizeof(struct ActionTake),
	sizeof(struct ActionDrop),
	sizeof(struct ActionOpen),
	sizeof(struct ActionClose),
	sizeof(struct ActionControl)
};
#endif

HandlerNode *FindHandler(System *sys, uint32 id) {
	if (sys) {
		for (HandlerNode *h = sys->Handlers->getListHead(); h->_succ; h = (HandlerNode *) h->_succ) {
			if (h->Id == id)
				return h;
		}
	}

	return nullptr;
}

System *InitSystem() {
	System *sys = new System;

	sys->Handlers = new NewList<HandlerNode>;
	sys->Signals = new NewList<plSignalNode>;
	sys->ActivHandler = nullptr;

	return sys;
}

void CloseSystem(System *sys) {
	if (sys) {
		delete sys->Handlers;
		delete sys->Signals;
	}
	delete sys;
}

void SetActivHandler(System *sys, uint32 id) {
	HandlerNode *h = FindHandler(sys, id);
	sys->ActivHandler = h;
}

void SaveSystem(Common::Stream *fh, System *sys) {
	if (fh) {
		dskSetLine(fh, FILE_SYSTEM_ID);

		for (HandlerNode *h = sys->Handlers->getListHead(); h->_succ; h = (HandlerNode *) h->_succ) {
			dskSetLine(fh, FILE_HANDLER_ID);
			dskSetLine_U32(fh, h->Id);
		}
	}
}

NewList<NewNode> *LoadSystem(Common::Stream *fh, System *sys) {
	NewList<NewNode> *l = g_clue->_txtMgr->goKey(PLAN_TXT, "SYSTEM_GUYS_MISSING_1");
	bool foundAll = true;
	uint8 knowsSomebody = 1, handlerNr = 0;

	if (fh) {
		char buffer[64];
		if (dskGetLine(buffer, sizeof(buffer), fh) && strcmp(buffer, FILE_SYSTEM_ID) == 0) {
			while (dskGetLine(buffer, sizeof(buffer), fh) && strcmp(buffer, FILE_HANDLER_ID) == 0) {
				uint32 id;

				if (dskGetLine_U32(fh, &id) && !dbIsObject(id, Object_Police)) {
					handlerNr++;

					if (!FindHandler(sys, id)) {
						if (knows(Person_Matt_Stuvysunt, id)) {
							knowsSomebody++;
							// CHECKME: The original code was mixing node types.
							// The new node doesn't, but needs extra testing.
							// dbAddObjectNode(l, id, OLF_INCLUDE_NAME);
							warning("CHECKME - Code modified in LoadSystem");
							dbObjectNode *test = dbGetObject(id);
							l->createNode(test->_name);
							//the intermediate code:
							//dbObject* obj = dbGetObjectReal(dbGetObject(id));
							//l->createNode(obj->link.Name);
						}
						foundAll = false;
					}
				}
			}
		}
	}

	if (foundAll) {
		l->removeList();
		l = nullptr;
	} else {
		NewList<NewNode> *extList = nullptr;

		if (knowsSomebody == 1)
			extList = g_clue->_txtMgr->goKey(PLAN_TXT, "SYSTEM_GUYS_MISSING_3");
		else if ((handlerNr - knowsSomebody) > 1)
			extList = g_clue->_txtMgr->goKeyAndInsert(PLAN_TXT, "SYSTEM_GUYS_MISSING_2", (uint32)(handlerNr - knowsSomebody));
		else if (handlerNr - knowsSomebody)
			extList = g_clue->_txtMgr->goKey(PLAN_TXT, "SYSTEM_GUYS_MISSING_4");

		if (extList) {
			for (NewNode *n = extList->getListHead(); n->_succ; n = n->_succ)
				l->createNode(n->_name);

			extList->removeList();
		}
	}

	return l;
}

HandlerNode *InitHandler(System *sys, uint32 id, uint32 flags) {
	HandlerNode *h = nullptr;

	if (sys && !FindHandler(sys, id)) {
		h = sys->Handlers->createNode(nullptr);
		h->Id = id;
		h->Timer = 0;
		h->Flags = flags;
		h->CurrentAction = nullptr;
		h->Actions = new NewList<ActionNode>();

		sys->ActivHandler = h;
	}

	return h;
}

void CloseHandler(System *sys, uint32 id) {
	HandlerNode *h = FindHandler(sys, id);

	if (h) {
		if (h->Actions)
			h->Actions->removeList();

		h->remNode();
		delete h;
	}
}

HandlerNode *ClearHandler(System *sys, uint32 id) {
	HandlerNode *h = FindHandler(sys, id);

	if (h) {
		if (h->Actions)
			h->Actions->removeList();

		h->Actions = new NewList<ActionNode>;
		h->Timer = 0;
		h->CurrentAction = nullptr;
	}

	return h;
}

bool IsHandlerCleared(System *sys) {
	if (!sys)
		return true;

	HandlerNode *h = sys->ActivHandler;

	if (h && h->Actions->isEmpty())
		return true;

	return false;
}

void SaveHandler(Common::Stream *fh, System *sys, uint32 id) {
	if (!fh || !sys)
		return;
	
	HandlerNode *h = FindHandler(sys, id);

	if (!h)
		return;

	dskSetLine(fh, FILE_ACTION_LIST_ID);
	dskSetLine_U32(fh, id);

	for (ActionNode *a = h->Actions->getListHead(); a->_succ; a = (ActionNode *) a->_succ) {
		dskSetLine(fh, FILE_ACTION_ID);
		dskSetLine_U16(fh, a->Type);
		dskSetLine_U16(fh, a->TimeNeeded);

		switch (a->Type) {
		case ACTION_GO:
			dskSetLine_U16(fh, ((ActionGoNode *) a)->Direction);
			break;

		case ACTION_USE:
			dskSetLine_U16(fh, ((ActionUseNode *)a)->ToolId);
			dskSetLine_U16(fh, ((ActionUseNode *)a)->ItemId);
			break;
		case ACTION_TAKE:
			dskSetLine_U16(fh, ((ActionTakeNode *)a)->LootId);
			dskSetLine_U16(fh, ((ActionTakeNode *)a)->ItemId);
			break;
		case ACTION_DROP:
			dskSetLine_U16(fh, ((ActionDropNode *)a)->LootId);
			dskSetLine_U16(fh, ((ActionDropNode *)a)->ItemId);
			break;

		case ACTION_OPEN:
			dskSetLine_U16(fh, ((ActionOpenNode *)a)->ItemId);
			break;
		case ACTION_CLOSE:
			dskSetLine_U16(fh, ((ActionCloseNode *)a)->ItemId);
			break;
		case ACTION_CONTROL:
			dskSetLine_U16(fh, ((ActionControlNode *)a)->ItemId);
			break;
		case ACTION_SIGNAL:
			dskSetLine_U16(fh, ((ActionSignalNode *)a)->ReceiverId);
			break;
		case ACTION_WAIT_SIGNAL:
			dskSetLine_U16(fh, ((ActionWaitSignalNode *)a)->SenderId);
			break;
		}
	}
}

bool LoadHandler(Common::Stream *fh, System *sys, uint32 id) {
	if (fh && sys && (FindHandler(sys, id))) {
		dskSeek(fh, 0);

		char buffer[64];
		while (dskGetLine(buffer, sizeof(buffer), fh)) {
			if (strcmp(buffer, FILE_ACTION_LIST_ID) == 0) {
				uint32 rid;

				if (dskGetLine_U32(fh, &rid) && id == rid) {
					SetActivHandler(sys, id);

					while (dskGetLine(buffer, sizeof(buffer), fh)
					        && (strcmp(buffer, FILE_ACTION_ID) == 0)) {
						uint16 type;
						uint16 time;
						dskGetLine_U16(fh, &type);
						dskGetLine_U16(fh, &time);

						if (type) {
							ActionNode *a = InitAction(sys, type, 0, 0, time);
							uint16 value16;
							uint32 value32;

							switch (type) {
							case ACTION_GO:
								dskGetLine_U16(fh, &value16);
								((ActionGoNode *)a)->Direction = value16;
								break;
							case ACTION_USE: {
								ActionUseNode *curAct = (ActionUseNode *)a;
								dskGetLine_U32(fh, &value32);
								curAct->ToolId = value32;

								dskGetLine_U32(fh, &value32);
								curAct->ItemId = value32;
								}
								break;
							case ACTION_TAKE: {
								ActionTakeNode *curAct = (ActionTakeNode *)a;
								dskGetLine_U32(fh, &value32);
								curAct->LootId = value32;

								dskGetLine_U32(fh, &value32);
								curAct->ItemId = value32;
								}
								break;
							case ACTION_DROP: {
								ActionDropNode *curAct= (ActionDropNode *)a;
								dskGetLine_U32(fh, &value32);
								curAct->LootId = value32;

								dskGetLine_U32(fh, &value32);
								curAct->ItemId = value32;
								break;
								}
							case ACTION_OPEN:
								dskGetLine_U32(fh, &value32);
								((ActionOpenNode *)a)->ItemId = value32;
								break;
							case ACTION_CLOSE:
								dskGetLine_U32(fh, &value32);
								((ActionCloseNode *)a)->ItemId = value32;
								break;
							case ACTION_CONTROL:
								dskGetLine_U32(fh, &value32);
								((ActionControlNode *)a)->ItemId = value32;
								break;
							case ACTION_WAIT_SIGNAL:
								dskGetLine_U32(fh, &value32);
								((ActionWaitSignalNode *)a)->SenderId = value32;
								break;
							case ACTION_SIGNAL:
								dskGetLine_U32(fh, &value32);
								((ActionSignalNode *)a)->ReceiverId = value32;
								break;
							}
						} else
							return false;
					}

					GoFirstAction(sys);
					return true;
				}
			}
		}
	}

	return false;
}

ActionNode* createActionNode(NewList<ActionNode>* list, uint16 type)
{
	ActionNode *act = nullptr;
	switch (type) {
	case 0: // Unknown/Unused? action created as an ActionNode in the original
	case ACTION_WAIT: // ACTION_WAIT uses a standard ActionNode
		act = new ActionNode;
		break;
	case ACTION_GO:
		act = new ActionGoNode;
		break;
	case ACTION_SIGNAL:
		act = new ActionSignalNode;
		break;
	case ACTION_WAIT_SIGNAL:
		act = new ActionWaitSignalNode;
		break;
	case ACTION_USE:
		act = new ActionUseNode;
		break;
	case ACTION_TAKE:
		act = new ActionTakeNode;
		break;
	case ACTION_DROP:
		act = new ActionDropNode;
		break;
	case ACTION_OPEN:
		act = new ActionOpenNode;
		break;
	case ACTION_CLOSE:
		act = new ActionCloseNode;
		break;
	case ACTION_CONTROL:
		act = new ActionControlNode;
		break;
	default:
		error("createActionNode - Invalid Type");
	}

	list->addTailNode(act);
	return act;
}

ActionNode *InitAction(System *sys, uint16 type, uint32 data1, uint32 data2, uint32 time) {
	if (!sys)
		return nullptr;

	HandlerNode* h = sys->ActivHandler;
	if (!h)
		return nullptr;

	ActionNode *a = createActionNode(h->Actions, type);
	if (a) {
		a->Type = type;
		a->TimeNeeded = time;
		a->Timer = time;

		h->Timer += time;
		h->CurrentAction = a;

		switch (type) {
		case ACTION_GO:
			((ActionGoNode *)a)->Direction = (uint16) data1;
			break;
		case ACTION_USE: {
			ActionUseNode *curAct = (ActionUseNode *)a;
			curAct->ItemId = data1;
			curAct->ToolId = data2;
			}
			break;
		case ACTION_TAKE: {
			ActionTakeNode *curAct = (ActionTakeNode *)a;
			curAct->ItemId = data1;
			curAct->LootId = data2;
			}
			break;
		case ACTION_DROP: {
			ActionDropNode *curAct = (ActionDropNode *)a;
			curAct->ItemId = data1;
			curAct->LootId = data2;
			}
			break;
		case ACTION_SIGNAL:
			((ActionSignalNode *)a)->ReceiverId = data1;
			break;
		case ACTION_WAIT_SIGNAL:
			((ActionWaitSignalNode *)a)->SenderId = data1;
			break;
		case ACTION_OPEN:
			((ActionOpenNode *)a)->ItemId = data1;
			break;
		case ACTION_CLOSE:
			((ActionCloseNode *)a)->ItemId = data1;
			break;
		case ACTION_CONTROL:
			((ActionControlNode *)a)->ItemId = data1;
			break;
		}
	}

	return a;
}

ActionNode *CurrentAction(System *sys) {
	HandlerNode *h;

	if (sys && (h = sys->ActivHandler))
		return h->CurrentAction;

	return nullptr;
}

ActionNode *GoFirstAction(System *sys) {
	if (!sys)
		return nullptr;

	HandlerNode* h = sys->ActivHandler;
	if (!h)
		return nullptr;
	
	if (!h->Actions->isEmpty()) {
		h->CurrentAction = h->Actions->getListHead();
		h->CurrentAction->Timer = 0;
		h->Timer = 0;
	} else
		h->CurrentAction = nullptr;

	return h->CurrentAction;
}

ActionNode *GoLastAction(System *sys) {
	if (!sys)
		return nullptr;

	HandlerNode* h = sys->ActivHandler;
	if (!h)
		return nullptr;

	if (!h->Actions->isEmpty()) {
		h->CurrentAction = h->Actions->getListTail();
		h->CurrentAction->Timer = h->CurrentAction->TimeNeeded;
		h->Timer = GetMaxTimer(sys);
	} else
		h->CurrentAction = nullptr;

	return h->CurrentAction;
}

ActionNode *NextAction(System *sys) {
	if (!sys)
		return nullptr;
	
	HandlerNode* h = sys->ActivHandler;
	if (!h)
		return nullptr;

	if (h->CurrentAction) {
		if (h->CurrentAction->Timer == h->CurrentAction->TimeNeeded) {
			if (h->CurrentAction->_succ->_succ) {
				h->CurrentAction = (ActionNode *)h->CurrentAction->_succ;
				h->CurrentAction->Timer = 1;
				h->Timer++;
			} else if (h->Flags & SHF_AUTOREVERS) {
				h->CurrentAction = h->Actions->getListHead();
				h->CurrentAction->Timer = 1;
				h->Timer++;
			} else {
				h->CurrentAction = h->Actions->getListTail();
				h->CurrentAction->Timer = h->CurrentAction->TimeNeeded;
				h->Timer = GetMaxTimer(sys);

				return nullptr;
			}
		} else {
			h->CurrentAction->Timer++;
			h->Timer++;
		}

		return h->CurrentAction;
	}

	return nullptr;
}

ActionNode *PrevAction(System *sys) {
	if (!sys)
		return nullptr;
	
	HandlerNode *h = sys->ActivHandler;
	if (!h)
		return nullptr;

	if (h->CurrentAction) {
		if (h->CurrentAction->Timer == 1) {
			if (h->CurrentAction->_pred->_pred) {
				h->CurrentAction = (ActionNode *)h->CurrentAction->_pred;

				h->CurrentAction->Timer = h->CurrentAction->TimeNeeded;
				h->Timer--;
			} else if ((h->Flags & SHF_AUTOREVERS) && h->Timer) {
				h->CurrentAction = h->Actions->getListTail();
				h->CurrentAction->Timer = h->CurrentAction->TimeNeeded;
				h->Timer--;
			} else {
				h->CurrentAction = h->Actions->getListHead();
				h->CurrentAction->Timer = 0;
				h->Timer = 0;

				return nullptr;
			}
		} else {
			h->CurrentAction->Timer--;
			h->Timer--;
		}
	}

	return h->CurrentAction;
}

bool ActionStarted(System *sys) {
	if (!sys)
		return false;
	HandlerNode *h = sys->ActivHandler;

	if (h) {
		ActionNode *a = h->CurrentAction;
		if (a) {
			if (a->Timer == 1)
				return true;
		}
	}

	return false;
}

bool ActionEnded(System *sys) {
	if (!sys)
		return false;
	
	HandlerNode *h = sys->ActivHandler;
	if (h) {
		ActionNode *a = h->CurrentAction;
		if (a) {
			if (a->Timer == a->TimeNeeded)
				return true;
		}
	}

	return false;
}

void RemLastAction(System *sys) {
	if (!sys)
		return;
	
	HandlerNode *h = sys->ActivHandler;
	if (!h)
		return;
	
	if (!h->Actions->isEmpty()) {
		if (h->Actions->getNrOfNodes() > 1) {
			ActionNode *n = h->Actions->remTailNode();
			delete n;

			h->Timer = GetMaxTimer(sys);
			h->CurrentAction = h->Actions->getListTail();
			h->CurrentAction->Timer = h->CurrentAction->TimeNeeded;
		} else
			ClearHandler(sys, h->Id);
	}
}

void IgnoreAction(System *sys) {
	if (!sys)
		return;
	
	HandlerNode *h = sys->ActivHandler;
	if (!h)
		return;

	if (h->CurrentAction) {
		if (h->CurrentAction->_succ->_succ) {
			h->CurrentAction = (ActionNode *) h->CurrentAction->_succ;
			h->CurrentAction->Timer = 0;
		} else {
			h->CurrentAction = h->Actions->getListTail();
			h->CurrentAction->Timer = h->CurrentAction->TimeNeeded;
		}
	}
}

plSignalNode *InitSignal(System *sys, uint32 sender, uint32 receiver) {
	plSignalNode *s = nullptr;

	if (sys) {
		s = sys->Signals->createNode(nullptr);
		if (s) {
			s->SenderId = sender;
			s->ReceiverId = receiver;
		}
	}

	return s;
}

void CloseSignal(plSignalNode *s) {
	if (s) {
		s->remNode();
		delete s;
		s = nullptr;
	}
}

plSignalNode *IsSignal(System *sys, uint32 sender, uint32 receiver) {
	if (sys) {
		for (plSignalNode *s = sys->Signals->getListHead(); s->_succ; s = (plSignalNode *) s->_succ) {
			if (s->SenderId == sender && s->ReceiverId == receiver)
				return s;
		}
	}

	return nullptr;
}

uint32 CurrentTimer(System *sys) {
	if (!sys)
		return 0;
	
	HandlerNode *h = sys->ActivHandler;
	if (h)
		return h->Timer;

	return 0;
}

void IncCurrentTimer(System *sys, uint32 time, bool alsoTime) {
	if (!sys)
		return;
	
	HandlerNode *h = sys->ActivHandler;

	if (h) {
		if (h->CurrentAction) {
			h->CurrentAction->TimeNeeded += time;

			if (alsoTime) {
				h->CurrentAction->Timer += time;
				h->Timer += time;
			}
		}
	}
}

uint32 GetMaxTimer(System *sys) {
	if (!sys)
		return 0;

	HandlerNode *h = sys->ActivHandler;
	uint32 time = 0;

	if (h) {
		for (ActionNode *a = h->Actions->getListHead(); a->_succ; a = (ActionNode *) a->_succ)
			time += a->TimeNeeded;
	}

	return time;
}

} // End of namespace Clue

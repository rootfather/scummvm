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

#define FILE_SYSTEM_ID      "SYS "  /* SYStem plan start */
#define FILE_HANDLER_ID     "HAND"  /* HANDler x needed  */
#define FILE_ACTION_LIST_ID "ACLI"  /* ACtionLIst for handler x started */
#define FILE_ACTION_ID      "ACTI"  /* ACTIon */

#define SYS_MAX_MEMORY_SIZE   1024L*25L

namespace Clue {

size_t sysUsedMem = 0;

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

struct Handler *FindHandler(struct System *sys, uint32 id) {
	if (sys) {
		for (struct Handler *h = (struct Handler *) LIST_HEAD(sys->Handlers); NODE_SUCC(h);
		        h = (struct Handler *) NODE_SUCC(h)) {
			if (h->Id == id)
				return h;
		}
	}

	return NULL;
}

struct System *InitSystem() {
	struct System *sys = (struct System *) TCAllocMem(sizeof(*sys), 0);

	if (sys) {
		sys->Handlers = CreateList();
		sys->Signals = CreateList();

		sys->ActivHandler = NULL;

		if (!sys->Handlers || !sys->Signals) {
			CloseSystem(sys);
			sys = NULL;
		}
	}

	return sys;
}

void CloseSystem(struct System *sys) {
	if (sys) {
		RemoveList(sys->Handlers);
		RemoveList(sys->Signals);

		TCFreeMem(sys, sizeof(*sys));
	}
}

void SetActivHandler(struct System *sys, uint32 id) {
	struct Handler *h = FindHandler(sys, id);

	if (h)
		sys->ActivHandler = (NODE *) h;
	else
		sys->ActivHandler = NULL;
}

void SaveSystem(Common::Stream *fh, struct System *sys) {
	if (fh) {
		dskSetLine(fh, FILE_SYSTEM_ID);

		for (struct Handler *h = (struct Handler *) LIST_HEAD(sys->Handlers); NODE_SUCC(h);
		        h = (struct Handler *) NODE_SUCC(h)) {
			dskSetLine(fh, FILE_HANDLER_ID);
			dskSetLine_U32(fh, h->Id);
		}
	}
}

LIST *LoadSystem(Common::Stream *fh, struct System *sys) {
	LIST *l = g_clue->_txtMgr->txtGoKey(PLAN_TXT, "SYSTEM_GUYS_MISSING_1");
	bool foundAll = true;
	uint8 knowsSomebody = 1, handlerNr = 0;

	if (fh) {
		char buffer[64];
		if (dskGetLine(buffer, sizeof(buffer), fh)
		        && strcmp(buffer, FILE_SYSTEM_ID) == 0) {
			while (dskGetLine(buffer, sizeof(buffer), fh)
			        && strcmp(buffer, FILE_HANDLER_ID) == 0) {
				uint32 id;

				if (dskGetLine_U32(fh, &id)
				        && !dbIsObject(id, Object_Police)) {
					handlerNr++;

					if (!FindHandler(sys, id)) {
						if (knows(Person_Matt_Stuvysunt, id)) {
							knowsSomebody++;
							dbAddObjectNode(l, id, OLF_INCLUDE_NAME);
						}

						foundAll = false;
					}
				}
			}
		}
	}

	if (foundAll) {
		RemoveList(l);
		l = NULL;
	} else {
		LIST *extList = NULL;

		if (knowsSomebody == 1)
			extList = g_clue->_txtMgr->txtGoKey(PLAN_TXT, "SYSTEM_GUYS_MISSING_3");
		else if ((handlerNr - knowsSomebody) > 1)
			extList = g_clue->_txtMgr->txtGoKeyAndInsert(PLAN_TXT, "SYSTEM_GUYS_MISSING_2", (uint32)(handlerNr - knowsSomebody));
		else if (handlerNr - knowsSomebody)
			extList = g_clue->_txtMgr->txtGoKey(PLAN_TXT, "SYSTEM_GUYS_MISSING_4");

		if (extList) {
			for (NODE *n = LIST_HEAD(extList); NODE_SUCC(n); n = NODE_SUCC(n))
				CreateNode(l, 0, NODE_NAME(n));

			RemoveList(extList);
		}
	}

	return l;
}

struct Handler *InitHandler(struct System *sys, uint32 id, uint32 flags) {
	struct Handler *h = NULL;

	if (sys && !FindHandler(sys, id)) {
		if ((h = (struct Handler *) CreateNode(sys->Handlers, sizeof(*h), NULL))) {
			h->Id = id;
			h->Timer = 0L;
			h->Flags = flags;
			h->CurrentAction = NULL;

			if (!(h->Actions = CreateList())) {
				RemNode(h);
				FreeNode(h);
				h = NULL;
			}

			sys->ActivHandler = (NODE *) h;
		}
	}

	return h;
}

void CloseHandler(struct System *sys, uint32 id) {
	struct Handler *h = FindHandler(sys, id);

	if (h) {
		CorrectMem(h->Actions);

		if (h->Actions)
			RemoveList(h->Actions);

		RemNode(h);
		FreeNode(h);
	}
}

struct Handler *ClearHandler(struct System *sys, uint32 id) {
	struct Handler *h = FindHandler(sys, id);

	if (h) {
		CorrectMem(h->Actions);

		if (h->Actions)
			RemoveList(h->Actions);

		if (!(h->Actions = CreateList())) {
			RemNode(h);
			FreeNode(h);
			h = NULL;
		}

		// FIXME: This is obviously wrong. I guess those two lines should be in an else statement
		h->Timer = 0L;
		h->CurrentAction = NULL;
	}

	return h;
}

bool IsHandlerCleared(struct System *sys) {
	struct Handler *h;

	if (sys && (h = (struct Handler *) sys->ActivHandler)) {
		if (LIST_EMPTY(h->Actions))
			return true;
	}

	return false;
}

void SaveHandler(Common::Stream *fh, struct System *sys, uint32 id) {
	struct Handler *h;

	if (fh && sys && (h = FindHandler(sys, id))) {
		dskSetLine(fh, FILE_ACTION_LIST_ID);
		dskSetLine_U32(fh, id);

		for (struct Action *a = (struct Action *) LIST_HEAD(h->Actions); NODE_SUCC(a);
		        a = (struct Action *) NODE_SUCC(a)) {
			dskSetLine(fh, FILE_ACTION_ID);
			dskSetLine_U16(fh, a->Type);
			dskSetLine_U16(fh, a->TimeNeeded);

			switch (a->Type) {
			case ACTION_GO:
				dskSetLine_U16(fh,
				        ActionData(a, struct ActionGo *)->Direction);
				break;

			case ACTION_USE:
			case ACTION_TAKE:
			case ACTION_DROP:
				dskSetLine_U16(fh,
				        ActionData(a, struct ActionUse *)->ToolId);
				dskSetLine_U16(fh,
				        ActionData(a, struct ActionUse *)->ItemId);
				break;

			case ACTION_OPEN:
			case ACTION_CLOSE:
			case ACTION_CONTROL:
			case ACTION_SIGNAL:
			case ACTION_WAIT_SIGNAL:
				dskSetLine_U16(fh,
				        ActionData(a, struct ActionOpen *)->ItemId);
				break;
			}
		}
	}
}

bool LoadHandler(Common::Stream *fh, struct System *sys, uint32 id) {
	if (fh && sys && (FindHandler(sys, id))) {
		//rewind(fh);

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
							struct Action *a = InitAction(sys, type, 0L, 0L, time);
							uint16 value16;
							uint32 value32;

							switch (type) {
							case ACTION_GO:
								dskGetLine_U16(fh, &value16);
								ActionData(a, struct ActionGo *)->Direction = value16;
								break;

							case ACTION_USE:
							case ACTION_TAKE:
							case ACTION_DROP:
								dskGetLine_U32(fh, &value32);
								ActionData(a, struct ActionUse *)->ToolId = value32;

								dskGetLine_U32(fh, &value32);
								ActionData(a, struct ActionUse *)->ItemId = value32;
								break;

							case ACTION_OPEN:
							case ACTION_CLOSE:
							case ACTION_CONTROL:
							case ACTION_WAIT_SIGNAL:
							case ACTION_SIGNAL:
								dskGetLine_U32(fh, &value32);
								ActionData(a, struct ActionOpen *)->ItemId = value32;
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

struct Action *InitAction(struct System *sys, uint16 type, uint32 data1, uint32 data2, uint32 time) {
	struct Handler *h;
	struct Action *a = nullptr;

	if (sys && (h = (struct Handler *) sys->ActivHandler)) {
		if ((sysUsedMem + sizeof(struct Action) + sizeofAction[type]) <= SYS_MAX_MEMORY_SIZE) {
			sysUsedMem += sizeof(struct Action) + sizeofAction[type];

			a = (struct Action *) CreateNode(h->Actions, sizeof(struct Action) + sizeofAction[type], NULL);
			if (a) {
				a->Type = type;
				a->TimeNeeded = time;
				a->Timer = time;

				h->Timer += time;
				h->CurrentAction = (NODE *) a;

				switch (type) {
				case ACTION_GO:
					ActionData(a, struct ActionGo *)->Direction = (uint16) data1;
					break;

				case ACTION_USE:
				case ACTION_TAKE:
				case ACTION_DROP:
					ActionData(a, struct ActionUse *)->ItemId = data1;
					ActionData(a, struct ActionUse *)->ToolId = data2;
					break;

				case ACTION_SIGNAL:
				case ACTION_WAIT_SIGNAL:
				case ACTION_OPEN:
				case ACTION_CLOSE:
				case ACTION_CONTROL:
					ActionData(a, struct ActionOpen *)->ItemId = data1;
					break;
				}
			}
		}
	}

	return a;
}

struct Action *CurrentAction(struct System *sys) {
	struct Handler *h;

	if (sys && (h = (struct Handler *) sys->ActivHandler))
		return (struct Action *) h->CurrentAction;

	return NULL;
}

struct Action *GoFirstAction(struct System *sys) {
	struct Handler *h;

	if (sys && (h = (struct Handler *) sys->ActivHandler)) {
		if (!LIST_EMPTY(h->Actions)) {
			h->CurrentAction = (NODE *) LIST_HEAD(h->Actions);
			((struct Action *) h->CurrentAction)->Timer = 0;
			h->Timer = 0;
		} else
			h->CurrentAction = NULL;

		return (struct Action *) h->CurrentAction;
	}

	return NULL;
}

struct Action *GoLastAction(struct System *sys) {
	struct Handler *h;

	if (sys && (h = (struct Handler *) sys->ActivHandler)) {
		if (!LIST_EMPTY(h->Actions)) {
			h->CurrentAction = (NODE *) LIST_TAIL(h->Actions);
			((struct Action *) h->CurrentAction)->Timer =
			    ((struct Action *) h->CurrentAction)->TimeNeeded;
			h->Timer = GetMaxTimer(sys);
		} else
			h->CurrentAction = NULL;

		return (struct Action *) h->CurrentAction;
	}

	return NULL;
}

struct Action *NextAction(struct System *sys) {
	struct Handler *h;

	if (sys && (h = (struct Handler *) sys->ActivHandler)) {
		if ((struct Action *) h->CurrentAction) {
			if (((struct Action *) h->CurrentAction)->Timer ==
			        ((struct Action *) h->CurrentAction)->TimeNeeded) {
				if (NODE_SUCC(NODE_SUCC(h->CurrentAction))) {
					h->CurrentAction = (NODE *) NODE_SUCC(h->CurrentAction);

					((struct Action *) h->CurrentAction)->Timer = 1;
					h->Timer++;
				} else {
					if (h->Flags & SHF_AUTOREVERS) {
						h->CurrentAction = (NODE *) LIST_HEAD(h->Actions);
						((struct Action *) h->CurrentAction)->Timer = 1;
						h->Timer++;
					} else {
						h->CurrentAction = (NODE *) LIST_TAIL(h->Actions);
						((struct Action *) h->CurrentAction)->Timer =
						    ((struct Action *) h->CurrentAction)->TimeNeeded;
						h->Timer = GetMaxTimer(sys);

						return NULL;
					}
				}
			} else {
				((struct Action *) h->CurrentAction)->Timer++;
				h->Timer++;
			}
		}

		return (struct Action *) h->CurrentAction;
	}

	return NULL;
}

struct Action *PrevAction(struct System *sys) {
	struct Handler *h;

	if (sys && (h = (struct Handler *) sys->ActivHandler)) {
		if ((struct Action *) h->CurrentAction) {
			if (((struct Action *) h->CurrentAction)->Timer == 1) {
				if (NODE_PRED(NODE_PRED(h->CurrentAction))) {
					h->CurrentAction = (NODE *) NODE_PRED(h->CurrentAction);

					((struct Action *) h->CurrentAction)->Timer =
					    ((struct Action *) h->CurrentAction)->TimeNeeded;
					h->Timer--;
				} else {
					if ((h->Flags & SHF_AUTOREVERS) && h->Timer) {
						h->CurrentAction = (NODE *) LIST_TAIL(h->Actions);
						((struct Action *) h->CurrentAction)->Timer =
						    ((struct Action *) h->CurrentAction)->TimeNeeded;
						h->Timer--;
					} else {
						h->CurrentAction = (NODE *) LIST_HEAD(h->Actions);
						((struct Action *) h->CurrentAction)->Timer = 0;
						h->Timer = 0;

						return NULL;
					}
				}
			} else {
				((struct Action *) h->CurrentAction)->Timer--;
				h->Timer--;
			}
		}

		return (struct Action *) h->CurrentAction;
	}

	return NULL;
}

bool ActionStarted(struct System *sys) {
	struct Handler *h;

	if (sys && (h = (struct Handler *) sys->ActivHandler)) {
		struct Action *a = (struct Action *) h->CurrentAction;
		if (a) {
			if (a->Timer == 1)
				return true;
		}
	}

	return false;
}

byte ActionEnded(struct System *sys) {
	struct Handler *h;

	if (sys && (h = (struct Handler *) sys->ActivHandler)) {
		struct Action *a = (struct Action *) h->CurrentAction;
		if (a) {
			if (a->Timer == a->TimeNeeded)
				return 1;
		}
	}

	return 0;
}

void RemLastAction(struct System *sys) {
	struct Handler *h;

	if (sys && (h = (struct Handler *) sys->ActivHandler)) {
		if (!LIST_EMPTY(h->Actions)) {
			if (GetNrOfNodes(h->Actions) > 1) {
				NODE *n = (NODE *) RemTailNode(h->Actions);
				sysUsedMem -= NODE_SIZE(n);
				FreeNode(n);

				h->Timer = GetMaxTimer(sys);
				h->CurrentAction = (NODE *) LIST_TAIL(h->Actions);
				((struct Action *) h->CurrentAction)->Timer =
				    ((struct Action *) h->CurrentAction)->TimeNeeded;
			} else
				ClearHandler(sys, h->Id);
		}
	}
}

void IgnoreAction(struct System *sys) {
	struct Handler *h;

	if (sys && (h = (struct Handler *) sys->ActivHandler)) {
		if ((struct Action *) h->CurrentAction) {
			if (NODE_SUCC(NODE_SUCC(h->CurrentAction))) {
				h->CurrentAction = (NODE *) NODE_SUCC(h->CurrentAction);

				((struct Action *) h->CurrentAction)->Timer = 0;
			} else {
				h->CurrentAction = (NODE *) LIST_TAIL(h->Actions);
				((struct Action *) h->CurrentAction)->Timer =
				    ((struct Action *) h->CurrentAction)->TimeNeeded;
			}
		}
	}
}

struct plSignal *InitSignal(struct System *sys, uint32 sender, uint32 receiver) {
	struct plSignal *s = NULL;

	if (sys) {
		if ((s = (struct plSignal *) CreateNode(sys->Signals, sizeof(*s), NULL))) {
			s->SenderId = sender;
			s->ReceiverId = receiver;
		}
	}

	return s;
}

void CloseSignal(struct plSignal *s) {
	if (s) {
		RemNode(s);
		FreeNode(s);
	}
}

struct plSignal *IsSignal(struct System *sys, uint32 sender, uint32 receiver) {

	if (sys) {
		for (struct plSignal *s = (struct plSignal *) LIST_HEAD(sys->Signals); NODE_SUCC(s);
		        s = (struct plSignal *) NODE_SUCC(s)) {
			if ((s->SenderId == sender) && (s->ReceiverId == receiver))
				return s;
		}
	}

	return NULL;
}

uint32 CurrentTimer(struct System *sys) {
	struct Handler *h;

	if (sys && (h = (struct Handler *) sys->ActivHandler))
		return h->Timer;

	return 0L;
}

void IncCurrentTimer(struct System *sys, uint32 time, byte alsoTime) {
	struct Handler *h;

	if (sys && (h = (struct Handler *) sys->ActivHandler)) {
		if (h->CurrentAction) {
			((struct Action *) h->CurrentAction)->TimeNeeded += time;

			if (alsoTime) {
				((struct Action *) h->CurrentAction)->Timer += time;
				h->Timer += time;
			}
		}
	}
}

uint32 GetMaxTimer(struct System *sys) {
	struct Handler *h;
	uint32 time = 0;

	if (sys && (h = (struct Handler *) sys->ActivHandler)) {
		for (struct Action *a = (struct Action *) LIST_HEAD(h->Actions); NODE_SUCC(a);
		        a = (struct Action *) NODE_SUCC(a))
			time += a->TimeNeeded;
	}

	return time;
}

void CorrectMem(LIST *l) {
	for (NODE *n = LIST_HEAD(l); NODE_SUCC(n); n = NODE_SUCC(n))
		sysUsedMem -= NODE_SIZE(n);
}

#if 0
void ResetMem() {
	sysUsedMem = 0;
}

size_t plGetUsedMem() {
	return sysUsedMem;
}
#endif
} // End of namespace Clue

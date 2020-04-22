/*
**      $Filename: planing/main.c
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.main for "Der Clou!"
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

#include "clue/planing/main.h"

#include "clue/clue.h"

namespace Clue {

System *plSys = nullptr;

Common::String txtTooLoud;
Common::String txtTimer;
Common::String txtWeight;
Common::String txtSeconds;


/* System functions */
void plInit() {
	/* Get texts */
	NewList<NewNode> *l = g_clue->_txtMgr->goKey(PLAN_TXT, "TXT_TOO_LOUD");
	txtTooLoud = l->getListHead()->_name;
	l->removeList();

	l = g_clue->_txtMgr->goKey(PLAN_TXT, "TXT_TIMER");
	txtTimer = l->getListHead()->_name;
	l->removeList();

	l = g_clue->_txtMgr->goKey(PLAN_TXT, "TXT_WEIGHT");
	txtWeight = l->getListHead()->_name;
	l->removeList();

	l = g_clue->_txtMgr->goKey(PLAN_TXT, "TXT_SECONDS");
	txtSeconds = l->getListHead()->_name;
	l->removeList();

	plSys = InitSystem();
}

void plDone() {
	CloseSystem(plSys);
}

} // End of namespace Clue

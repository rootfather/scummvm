/*
**  $Filename: error/error.c
**  $Release:
**  $Revision:  04.10.1994 (hg)
**  $Date:
**
**  functions for error handling
**
**  (C) 1993, 1994 ...and avoid panic by, H. Gaberschek
**      All Rights Reserved
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#define ERR_EXIT_ERROR      20L
#define ERR_EXIT_SHUTDOWN   30L

#include "clue/clue.h"
#include "clue/base/base.h"
#include "common/debug.h"
#include "common/textconsole.h"

namespace Clue {

void ErrorMsg(ErrorE type, ErrorModuleE moduleId, uint32 errorId) {
	DebugMsg(ERR_DEBUG, moduleId, "Error %d", errorId);

	tcDone();

	switch (type) {
	case Internal_Error:
		error("Internal Error!");
		break;

	case No_Mem:
		error("You don't have enough memory!");
		break;

	case Disk_Defect:
		error("Can't open file! Please install DER CLOU! again");
		break;
	default:
		break;
	}
}

static void ErrDebugMsg(DebugE type, ErrorModuleE moduleId, const char *txt) {
	switch (type) {
	case ERR_DEBUG:
		debugC(ERR_CHANNEL(moduleId), "%s\t: %s", moduleNames[moduleId], txt);
		break;

	case ERR_WARNING:
		warning("Module %s: %s", moduleNames[moduleId], txt);
		break;

	case ERR_ERROR:
		tcDone();
		error("Module %s: %s", moduleNames[moduleId], txt);
		break;
	}
}

void DebugMsg(DebugE type, ErrorModuleE moduleId, const char *format, ...) {
	va_list arglist;
	char txt[512];

	va_start(arglist, format);
	vsprintf(txt, format, arglist);
	va_end(arglist);

	ErrDebugMsg(type, moduleId, txt);
}

} // End of namespace Clue

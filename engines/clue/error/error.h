/*
**  $Filename: Error/Error.h
**  $Release:
**  $Revision:
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

#ifndef MODULE_ERROR
#define MODULE_ERROR

#include "clue/disk/disk.h"

namespace Clue {

enum ErrorModuleE {
	ERROR_MODULE_BASE       =  3,
	ERROR_MODULE_TXT        =  4,
	ERROR_MODULE_DISK       =  5,
	ERROR_MODULE_MEMORY     =  6,
	ERROR_MODULE_DATABASE   =  7,
	ERROR_MODULE_GAMEPLAY   =  8,
	ERROR_MODULE_LOADSAVE   =  9,
	ERROR_MODULE_LANDSCAP   = 10,
	ERROR_MODULE_LIVING     = 11,
	ERROR_MODULE_PLANING    = 12,
	ERROR_MODULE_SOUND      = 13,
	ERROR_MODULE_PRESENT    = 14,
	ERROR_MODULE_GFX        = 15,
	ERROR_MODULE_INPUT      = 16,
	ERROR_MODULE_LAST
};

static const char moduleNames[][ERROR_MODULE_LAST] = {
	"",
	"",
	"",
	"Base",
	"Txt",
	"Dsk",
	"Mem",
	"Data",
	"GP",
	"L/S",
	"Land",
	"Liv",
	"Plan",
	"Snd",
	"Pres",
	"Gfx",
	"Input"
};

#define ERR_CHANNEL(x) 1 << x

/* Parameter : mode */

#define  ERR_NO_OUTPUT         1
#define  ERR_OUTPUT_TO_DISK    2

/* some error types */

enum ErrorE {
	No_Error        = 0,
	Internal_Error,
	No_Mem,
	Disk_Defect
};

/* some debug types */

enum DebugE {
	ERR_ERROR,
	ERR_WARNING,
	ERR_DEBUG
};

void ErrorMsg(ErrorE type, ErrorModuleE moduleId, uint32 errorId);
void DebugMsg(DebugE type, ErrorModuleE moduleId, const char *format, ...);

} // End of namespace Clue

#endif

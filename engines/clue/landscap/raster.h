/*
**  $Filename: landscap/raster.h
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-02-94
**
**  landscape raster functions for "Der Clou!"
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

#ifndef MODULE_RASTER
#define MODULE_RASTER

#include "clue/dialog/dialog.h"
#include "clue/landscap/landscap.h"

namespace Clue {

#define LS_RASTER_DISP_WIDTH       320
#define LS_RASTER_DISP_HEIGHT      140

#define LS_RASTER_X_SIZE           4    /* Raster is used for collision */
#define LS_RASTER_Y_SIZE           4    /* dedection, in pixel */


dbObjectNode *lsGetSuccObject(dbObjectNode *start);
dbObjectNode *lsGetPredObject(dbObjectNode *start);

void lsFadeRasterObject(uint32 areaID, LSObjectNode *lso, byte status);
void lsShowAllConnections(uint32 areaID, dbObjectNode *node, byte perc);
void lsShowRaster(uint32 areaID, byte perc);

uint16 lsGetRasterXSize(uint32 areaID);
uint16 lsGetRasterYSize(uint32 areaID);

} // End of namespace Clue

#endif

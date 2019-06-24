/*
**	$Filename: present/present.h
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-02-94
**
**	presentation functions for "Der Clou!"
**
**   (c) 1994 ...and avoid panic by, K. Kazemi, H. Gaberschek
**	    All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_PRESENT
#define MODULE_PRESENT

#include <math.h>

#include "clue/theclou.h"

#ifndef MODULE_LIST
#include "clue/list/list.h"
#endif

#ifndef MODULE_TEXT
#include "clue/text/text.h"
#endif

#include "clue/gfx/gfx.h"

#ifndef MODULE_DATABASE
#include "clue/data/database.h"
#endif

#ifndef MODULE_RELATION
#include "clue/data/relation.h"
#endif

#include "clue/data/objstd/tcdata.h"

#ifndef MODULE_DATAAPPL
#include "clue/data/dataappl.h"
#endif

#ifndef MODULE_SCENES
#include "clue/scenes/scenes.h"
#endif

#ifndef MODULE_LANDSCAP
#include "clue/landscap/landscap.h"
#endif

#define PRESENT_AS_TEXT       1
#define PRESENT_AS_BAR        2
#define PRESENT_AS_NUMBER     3

struct _GC;

extern ubyte Present(U32 nr, char *presentationText,
		     void (*initPresentation) (U32, LIST *, LIST *));

extern void InitPersonPresent(U32 nr, LIST * presentationData, LIST * texts);
extern void InitCarPresent(U32 nr, LIST * presentationData, LIST * texts);
extern void InitPlayerPresent(U32 nr, LIST * presentationData, LIST * texts);
extern void InitBuildingPresent(U32 nr, LIST * presentationData, LIST * texts);
extern void InitToolPresent(U32 nr, LIST * presentationData, LIST * texts);
extern void InitObjectPresent(U32 nr, LIST * presentationData, LIST * texts);
extern void InitEvidencePresent(U32 nr, LIST * presentationData, LIST * texts);
extern void InitLootPresent(U32 nr, LIST * presentationData, LIST * texts);
extern void InitOneLootPresent(U32 nr, LIST * presentationData, LIST * texts);

extern void prSetBarPrefs(struct _GC *gc, uword us_BarWidth,
			  uword us_BarHeight, ubyte uch_FCol,
			  ubyte uch_BCol, ubyte uch_TCol);
extern void prDrawTextBar(char *puch_Text, U32 ul_Value, U32 ul_Max,
			  uword us_XPos, uword us_YPos);

extern void DrawPresent(LIST * present, U8 firstLine, struct _GC *gc, U8 max);
void AddPresentTextLine(LIST * l, const char *data, U32 max, LIST * texts,
			U16 textNr);
void AddPresentLine(LIST * l, U8 presentHow, U32 data, U32 max,
		    LIST * texts, U16 textNr);

#endif

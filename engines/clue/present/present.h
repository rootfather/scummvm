/*
**  $Filename: present/present.h
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-02-94
**
**  presentation functions for "Der Clou!"
**
**   (c) 1994 ...and avoid panic by, K. Kazemi, H. Gaberschek
**      All Rights Reserved.
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

#include "clue/gfx/gfx.h"
#include "clue/scenes/scenes.h"

namespace Clue {

#define PRESENT_AS_TEXT       1
#define PRESENT_AS_BAR        2
#define PRESENT_AS_NUMBER     3

struct _GC;

extern byte Present(uint32 nr, const char *presentationText, void (*initPresentation)(uint32, NewList<PresentationInfoNode> *, NewList<NewNode> *));

extern void InitPersonPresent(uint32 nr, NewList<PresentationInfoNode>* presentationData, NewList<NewNode>* texts);
extern void InitCarPresent(uint32 nr, NewList<PresentationInfoNode>* presentationData, NewList<NewNode>* texts);
extern void InitPlayerPresent(uint32 nr, NewList<PresentationInfoNode>* presentationData, NewList<NewNode>* texts);
extern void InitBuildingPresent(uint32 nr, NewList<PresentationInfoNode>* presentationData, NewList<NewNode>* texts);
extern void InitToolPresent(uint32 nr, NewList<PresentationInfoNode>* presentationData, NewList<NewNode>* texts);
extern void InitObjectPresent(uint32 nr, NewList<PresentationInfoNode>* presentationData, NewList<NewNode>* texts);
extern void InitEvidencePresent(uint32 nr, NewList<PresentationInfoNode>* presentationData, NewList<NewNode>* texts);
extern void InitLootPresent(uint32 nr, NewList<PresentationInfoNode>* presentationData, NewList<NewNode>* texts);
extern void InitOneLootPresent(uint32 nr, NewList<PresentationInfoNode>* presentationData, NewList<NewNode>* texts);

extern void prSetBarPrefs(struct _GC *gc, uint16 us_BarWidth, uint16 us_BarHeight, byte uch_FCol, byte uch_BCol, byte uch_TCol);
extern void prDrawTextBar(const char *puch_Text, uint32 ul_Value, uint32 ul_Max, uint16 us_XPos, uint16 us_YPos);

extern void DrawPresent(NewList<PresentationInfoNode> *present, uint8 firstLine, struct _GC *gc, uint8 max);
void AddPresentTextLine(NewList<PresentationInfoNode> *l, Common::String data, uint32 max, NewList<NewNode> *texts, uint16 textNr);
void AddPresentLine(NewList<PresentationInfoNode> *l, uint8 presentHow, uint32 data, uint32 max, NewList<NewNode> *texts, uint16 textNr);

} // End of namespace Clue

#endif

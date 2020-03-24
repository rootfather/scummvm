/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
/*
 *
 * Present.ph
 *
 */

namespace Clue {

#define BAR_WIDTH            255

struct PresentControl {
	GC *gc;

	uint16 us_BarWidth;
	uint16 us_BarHeight;

	byte uch_FCol;
	byte uch_BCol;
	byte uch_TCol;
};

struct presentationInfo {
	Node link;

	char extendedText[70];
	uint32 extendedNr;
	uint32 maxNr;

	byte presentHow;
};

} // End of namespace Clue

/*
 * dataCalc.c
 * (c) 1993 by Helmut Gaberschek & Kaweh Kazemi, ...and avoid panic by
 * All rights reserved.
 *
 * 
 *
 * Rev   Date        Comment
 * 1     14-09-93    defines for cars
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "clue/data/datacalc.h"

int32 Round(int32 v, int32 p)
{				/* p...Stellen ! */
    register int32 i, z;

    for (i = 0, z = 1; i < p; i++, z *= 10);

    if (v / z != 0)
	v = (v / z) * z;

    return (v);
}

int32 CalcValue(int32 value, int32 min, int32 max, int32 fact, int32 perc)
{
    perc = ((perc * fact) - (perc * 128)) / 127;

    value = value + (value * perc) / 100;

    return CLIP(value, min, max);
}

int32 ChangeAbs(int32 item, int32 value, int32 min, int32 max)
{
    item += value;

    return CLIP(item, min, max);
}

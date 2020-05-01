/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

 /*
 * This code is based on :
 * - original source code of The Clue!
 *	Copyright (c) 1994 ...and avoid panic by, Kaweh Kazemi
 * - Costa's DerClou! SDL port
 *	Copright (c) 2005-2017 Vasco Alexandre da Silva Costa
 */

#ifndef MODULE_CDROM
#define MODULE_CDROM

#include "clue/clue.h"

namespace Clue {
class CDManager {
private:
	// Not used currently, may be useful later
	ClueEngine *_vm;
	
	bool _installed;

public:

	CDManager(ClueEngine *vm) : _vm(vm) { _installed = false; }

	void install();
	void unInstall();
	void playTrack(uint8 trackNum);
	void playSequence(uint8 trackNum, uint32 startOffset, uint32 endOffset);
	void stop();
};

} // End of namespace Clue

#endif

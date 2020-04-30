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

#include "clue/cdrom/cdrom.h"
#include "backends/audiocd/audiocd.h"
#include "clue/clue.h"

namespace Clue {
void CDManager::install() {
	_installed = g_system->getAudioCDManager()->open();
}

void CDManager::unInstall() {
	if (_installed)
		g_system->getAudioCDManager()->close();
}

void CDManager::playTrack(uint8 TrackNum) {
	// Note: Duration 0 play the whole track
	g_system->getAudioCDManager()->play(TrackNum - 1, 1, 0, 0, false, Audio::Mixer::kSpeechSoundType);
}

void CDManager::playSequence(uint8 TrackNum, uint32 StartOffset, uint32 EndOffset) {
	// Assumption that extracted audio tracks are named Track1 -> 23. The original starts counting at 2.
	g_system->getAudioCDManager()->play(TrackNum - 1, 1, StartOffset, EndOffset - StartOffset, false, Audio::Mixer::kSpeechSoundType);
}

void CDManager::stop() {
	if (_installed)
		g_system->getAudioCDManager()->stop();
}

} // End of namespace Clue

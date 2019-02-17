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

#include "common/error.h"
#include "common/events.h"
#include "common/system.h"

#include "engines/engine.h"
#include "engines/util.h"

#include "clue/clue.h"

namespace Clue {

ClueEngine::ClueEngine(OSystem *syst, const ClueGameDescription *gameDesc)
	: Engine(syst),
	_gameDescription(gameDesc),
	_rnd("clue") {
}

ClueEngine::~ClueEngine() {
}

Common::Error ClueEngine::run() {
	// Initialize backend
	initGraphics(320, 200);

    // Run a dummy loop
	Common::Event event;

	while (!shouldQuit()) {
		while (g_system->getEventManager()->pollEvent(event)) {
			if (event.type == Common::EVENT_KEYDOWN && event.kbd.keycode == Common::KEYCODE_ESCAPE)
				g_engine->quitGame();
		}

		g_system->delayMillis(10);
	}

	return Common::kNoError;
}

void initialize() {
}

} // End of namespace Clue
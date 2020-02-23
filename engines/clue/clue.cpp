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

#include "clue/theclou.h"

#include "common/error.h"
#include "common/events.h"
#include "common/system.h"
#include "common/config-manager.h"

#include "engines/engine.h"
#include "engines/util.h"

#include "clue/clue.h"
#include "clue/base/base.h"

namespace Clue {

ClueEngine *g_clue = NULL;

ClueEngine::ClueEngine(OSystem *syst, const ADGameDescription *gameDesc)
	: Engine(syst),
	_gameDescription(gameDesc) {
	g_clue = this;
}

ClueEngine::~ClueEngine() {
	g_clue = NULL;
}

Common::Error ClueEngine::run() {
	// Game entry point
	clue_main(ConfMan.get("path").c_str());
	quitGame();

	/*
	// Run a dummy loop
	Common::Event event;

	while (!shouldQuit()) {
		while (g_system->getEventManager()->pollEvent(event)) {
			if (event.type == Common::EVENT_KEYDOWN && event.kbd.keycode == Common::KEYCODE_ESCAPE)
				quitGame();
		}

		g_system->delayMillis(10);
	}
	*/

	return Common::kNoError;
}

} // End of namespace Clue

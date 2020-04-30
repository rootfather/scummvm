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

#include "common/config-manager.h"
#include "common/debug-channels.h"

#include "clue/clue.h"
#include "clue/error/error.h"
#include "clue/base/base.h"
#include "clue/text.h"
#include "clue/cdrom/cdrom.h"

namespace Clue {

ClueEngine *g_clue = NULL;

ClueEngine::ClueEngine(OSystem *syst, const ADGameDescription *gameDesc) : Engine(syst), _gameDescription(gameDesc) {
	g_clue = this;

	XMSOffset = 0;
	XMSHandle = nullptr;

	// Set up debug channels
	for (uint32 moduleId = ERROR_MODULE_BASE; moduleId < ERROR_MODULE_LAST; moduleId++) {
		DebugMan.addDebugChannel(ERR_CHANNEL(moduleId), moduleNames[moduleId], "");
	}

	rnd = new Common::RandomSource("clue");
	const char* path = ConfMan.get("path").c_str();
	dskSetRootPath(path);
	_txtMgr = new TextMgr(g_clue);
	_cdMgr = new CDManager(g_clue);
}

ClueEngine::~ClueEngine() {
	g_clue = nullptr;
	delete _txtMgr;
	delete _cdMgr;
	delete rnd;
}

uint32 ClueEngine::calcRandomNr(uint32 lowLimit, uint32 highLimit) {
	return rnd->getRandomNumberRng(lowLimit, highLimit - 1);
}
	
Common::Error ClueEngine::run() {

	if (getFeatures() & ADGF_DEMO)
		GamePlayMode |= GP_DEMO | GP_STORY_OFF;

	if (tcInit()) {
		uint32 sceneId = SCENE_NEW_GAME;

		gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);
		gfxChangeColors(m_gc, 0, GFX_FADE_OUT, 0);

		tcSetPermanentColors();

		/* to blend the menu colours once: */
		gfxShow(CurrentBackground, GFX_ONE_STEP | GFX_NO_REFRESH | GFX_BLEND_UP, 0,
			-1, -1);

		/* mouse to white - assume we need to set 15 and 16 */
		gfxSetRGB(nullptr, 15, 255, 255, 255);
		gfxSetRGB(nullptr, 16, 255, 255, 255);

		SetBubbleType(SPEAK_BUBBLE);

		ShowMenuBackground();

		while (sceneId == SCENE_NEW_GAME) {
			byte ret = 0;

			if (!(GamePlayMode & GP_DEMO))
				InitStory(STORY_DAT);
			else
				InitStory(STORY_DAT_DEMO);

			while (!ret)
				ret = startupMenu();

			if (ret != 2) {
				if (GamePlayMode & GP_FULL_ENV)
					setFullEnviroment();

				sceneId = PlayStory();
			}
			else
				sceneId = SCENE_THE_END;

			closeData();
			CloseStory();
		}
	}

	tcDone();

	quitGame();
	return Common::kNoError;
}

} // End of namespace Clue

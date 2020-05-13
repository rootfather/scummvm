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

#ifndef CLUE_H
#define CLUE_H

#include "engines/engine.h"
#include "engines/advancedDetector.h"

#include "common/random.h"
#include "common/system.h"

namespace Clue {
	
enum ClueGameFeatures {
	GF_PROFIDISK = (1 << 0) // Expansion pack
};

class TextMgr;
class CDManager;
class SndManager;

class ClueEngine : public Engine {
private:
	size_t XMSOffset;
	uint8 *XMSHandle;

	uint32 amg2Pc(uint32 s);
	void processIntroAnimation(uint8 *dp, uint8 *sp);
	void showIntro();

protected:
	// Engine APIs
	virtual Common::Error run();
	virtual bool hasFeature(EngineFeature f) const;

public:
 	ClueEngine(OSystem *syst, const ADGameDescription *gameDesc);
	~ClueEngine();

	uint32 getFeatures() const; 
	Common::Language getLanguage() const;
	char getTxtLanguage() const;
	Common::Platform getPlatform() const;
	uint32 calcRandomNr(uint32 lowLimit, uint32 highLimit);

	const ADGameDescription *_gameDescription;

	TextMgr* _txtMgr;
	CDManager *_cdMgr;
	SndManager *_sndMgr;

private:
	Common::RandomSource* rnd;

	byte startupMenu();
	bool tcInit();
	void setFullEnviroment();
	void closeData();
};

extern ClueEngine *g_clue;

} // End of namespace Clue

#endif

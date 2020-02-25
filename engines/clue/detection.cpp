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

#include "common/savefile.h"
#include "common/system.h"
#include "base/plugins.h"

#include "engines/advancedDetector.h"
#include "engines/engine.h"

#include "clue/clue.h"

namespace Clue {

uint32 ClueEngine::getFeatures() const {
	return _gameDescription->flags;
}

Common::Language ClueEngine::getLanguage() const {
	return _gameDescription->language;
}

char ClueEngine::getTxtLanguage() const {
	switch (_gameDescription->language)
	{
	case Common::EN_ANY:
	case Common::CZ_CZE:
		return 'e';
	case Common::FR_FRA:
		return 'f';
	case Common::ES_ESP:
		return 's';
	}
	return 'd'; // default to 'deutsche'
}

Common::Platform ClueEngine::getPlatform() const {
	return _gameDescription->platform;
}

static const PlainGameDescriptor clueGames[] = {
	{"clue", "The Clue!"},
	{0, 0}
};

static const ADGameDescription gameDescriptions[] = {

	{
		"clue",
		"",
		{
			{ "THECLOUE.TXT", 0, "2ef27892c565679069d97fd830b97dd0", 2564 },
			{ "TCMAINOE.TXT", 0, "2973a01ecd8d13d82f201e1f38fcb763", 14160},
			AD_LISTEND
		},
		Common::EN_ANY,
		Common::kPlatformDOS,
		ADGF_NO_FLAGS,
		GUIO0()
	},
	{
		"clue",
		"",
		{
			{ "THECLOUE.TXT", 0, "e11f380d78fc41ffe6347f4a68303cee", 2288 },
			{ "TCMAINOE.TXT", 0, "3424153390d762521ad8b64e6feb4e97", 14141 },
			AD_LISTEND
		},
		Common::CZ_CZE,
		Common::kPlatformDOS,
		ADGF_NO_FLAGS,
		GUIO0()
	},
	{
		"clue",
		"",
		{
			{ "THECLOUD.TXT", 0, "6d490c644b1ecee70fa67bc6aa16be22", 2766 },
			{ "TITLE.BK", 0, "8007db7e506015cf7f82388414c8add7", 11955 },
			{ "PICT.LST", 0, "852a0c798b272eacb86a8dcbb939621d", 5465 },
			AD_LISTEND
		},
		Common::DE_DEU,
		Common::kPlatformDOS,
		ADGF_NO_FLAGS,
		GUIO0()
	},
	{
		"clue",
		"CD",
		{
			{ "TITLE.BK", 0, "d6da5307af9bbfcefc6ab015cac8914f", 11955 },
			{ "PICT.LST", 0, "102cc1e183541b7a23f7493459aef632", 5463 },
			AD_LISTEND
		},
		Common::DE_DEU,
		Common::kPlatformDOS,
		ADGF_CD,
		GUIO0()
	},
	{
		"clue",
		"Profi-Diskette",
		{
			{ "TITLE.BK", 0, "8007db7e506015cf7f82388414c8add7", 11955 },
			{ "PICT.LST", 0, "852a0c798b272eacb86a8dcbb939621d", 6124 },
			AD_LISTEND
		},
		Common::DE_DEU,
		Common::kPlatformDOS,
		GF_PROFIDISK,
		GUIO0()
	},
	{
		"clue",
		"Profi-CD",
		{
			{ "TITLE.BK", 0, "d6da5307af9bbfcefc6ab015cac8914f", 11955 },
			{ "PICT.LST", 0, "852a0c798b272eacb86a8dcbb939621d", 6124 },
			AD_LISTEND
		},
		Common::DE_DEU,
		Common::kPlatformDOS,
		GF_PROFIDISK | ADGF_CD,
		GUIO0()
	},

	AD_TABLE_END_MARKER
};

static const char *directoryGlobs[] = {
	"TEXTS",
	"SOUNDS",
	0
};

class ClueMetaEngine : public AdvancedMetaEngine {
public:
	ClueMetaEngine() : AdvancedMetaEngine(Clue::gameDescriptions, sizeof(ADGameDescription), clueGames) {
		_maxScanDepth = 2;
		_directoryGlobs = directoryGlobs;
	}

	const char *getEngineId() const override {
		return "clue";
	}

	const char *getName() const override {
		return "Clue";
	}

	const char *getOriginalCopyright() const override {
		return "The Clue! (C) 1994 neo Software Produktions GmbH";
	}

	bool hasFeature(MetaEngineFeature f) const override;
	bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const override;
};

bool ClueMetaEngine::hasFeature(MetaEngineFeature f) const {
	return false;
}

bool Clue::ClueEngine::hasFeature(EngineFeature f) const {
	return false;
}

bool ClueMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	if (desc) {
		*engine = new Clue::ClueEngine(syst, desc);
	}
	return desc != 0;
}

} // End of namespace Clue

#if PLUGIN_ENABLED_DYNAMIC(CLUE)
REGISTER_PLUGIN_DYNAMIC(CLUE, PLUGIN_TYPE_ENGINE, Clue::ClueMetaEngine);
#else
REGISTER_PLUGIN_STATIC(CLUE, PLUGIN_TYPE_ENGINE, Clue::ClueMetaEngine);
#endif

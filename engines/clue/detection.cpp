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

struct ClueGameDescription {
	ADGameDescription desc;
};

uint32 ClueEngine::getFeatures() const {
	return _gameDescription->desc.flags;
}

Common::Language ClueEngine::getLanguage() const {
	return _gameDescription->desc.language;
}

}

static const PlainGameDescriptor clueGames[] = {
	{"clue", "The Clue!"},
	{0, 0}
};


namespace Clue {

static const ClueGameDescription gameDescriptions[] = {

	{
		{
			"clue",
			"",
			AD_ENTRY1s("THECLOU.EXE", "0e65d33e68e8d902cb5068eab5c5aedc", 270496),
			Common::DE_DEU,
			Common::kPlatformDOS,
			ADGF_NO_FLAGS,
			GUIO1(GUIO_NONE)
		},
	},

	{ AD_TABLE_END_MARKER }
};

} // End of namespace Clue

class ClueMetaEngine : public AdvancedMetaEngine {
public:
	ClueMetaEngine() : AdvancedMetaEngine(Clue::gameDescriptions, sizeof(Clue::ClueGameDescription), clueGames) {
		_singleId = "clue";
	}

	virtual const char *getName() const {
		return "Clue Engine";
	}

	virtual const char *getOriginalCopyright() const {
		return "The Clue! (C) 199x";
	}

	virtual bool hasFeature(MetaEngineFeature f) const;
	virtual bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const;
	SaveStateList listSaves(const char *target) const;
	virtual int getMaximumSaveSlot() const;
	void removeSaveState(const char *target, int slot) const;
	SaveStateDescriptor querySaveMetaInfos(const char *target, int slot) const;
};

bool ClueMetaEngine::hasFeature(MetaEngineFeature f) const {
	return
		(f == kSupportsListSaves) ||
		(f == kSupportsLoadingDuringStartup) ||
		(f == kSupportsDeleteSave) ||
		(f == kSavesSupportMetaInfo) ||
		(f == kSavesSupportThumbnail) ||
		(f == kSavesSupportCreationDate) ||
		(f == kSavesSupportPlayTime);
}

bool Clue::ClueEngine::hasFeature(EngineFeature f) const {
	return
		(f == kSupportsRTL) ||
		(f == kSupportsLoadingDuringRuntime) ||
		(f == kSupportsSavingDuringRuntime);
}

bool ClueMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	const Clue::ClueGameDescription *gd = (const Clue::ClueGameDescription *)desc;
	if (gd) {
		*engine = new Clue::ClueEngine(syst, gd);
	}
	return gd != 0;
}

SaveStateList ClueMetaEngine::listSaves(const char *target) const {
	SaveStateList saveList;

	return saveList;
}

int ClueMetaEngine::getMaximumSaveSlot() const {
	return 999;
}

void ClueMetaEngine::removeSaveState(const char *target, int slot) const {
}

SaveStateDescriptor ClueMetaEngine::querySaveMetaInfos(const char *target, int slot) const {

	return SaveStateDescriptor();
} // End of namespace Clue

#if PLUGIN_ENABLED_DYNAMIC(CLUE)
	REGISTER_PLUGIN_DYNAMIC(CLUE, PLUGIN_TYPE_ENGINE, ClueMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(CLUE, PLUGIN_TYPE_ENGINE, ClueMetaEngine);
#endif
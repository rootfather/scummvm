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

}

static const PlainGameDescriptor clueGames[] = {
	{"clue", "The Clue!"},
	{0, 0}
};


namespace Clue {

static const ADGameDescription gameDescriptions[] = {

	{
		"clue",
		"",
		AD_ENTRY1s("THECLOU.EXE", "0e65d33e68e8d902cb5068eab5c5aedc", 270496),
		Common::DE_DEU,
		Common::kPlatformDOS,
		ADGF_NO_FLAGS,
		GUIO0()
	},
	{
		"clue",
		"",
		AD_ENTRY1s("THECLOU.EXE", "e8f62d94f94402a2b1735636350fcb4c", 250192),
		Common::DE_DEU,
		Common::kPlatformDOS,
		ADGF_CD,
		GUIO0()
	},
	{
		"clue",
		"",
		AD_ENTRY1s("THECLUE!.EXE", "5dff3defc96466f2719b4189cf5cddf2", 271040),
		Common::EN_ANY,
		Common::kPlatformDOS,
		ADGF_NO_FLAGS,
		GUIO0()
	},

	AD_TABLE_END_MARKER
};

} // End of namespace Clue

class ClueMetaEngine : public AdvancedMetaEngine {
public:
	ClueMetaEngine() : AdvancedMetaEngine(Clue::gameDescriptions, sizeof(ADGameDescription), clueGames) {
		_singleId = "clue";
	}

	virtual const char *getName() const {
		return "Clue Engine";
	}

	virtual const char *getOriginalCopyright() const {
		return "The Clue! (C) 1994 neo Software";
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
	if (desc) {
		*engine = new Clue::ClueEngine(syst, desc);
	}
	return desc != 0;
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
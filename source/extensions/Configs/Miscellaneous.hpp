#pragma once
#include <cassert>
#include "app_debug.h"
#include "AudioEngine.h"
#include "Common.h"
#include "FileMgr.h"
#include "GenericGameStorage.h"
#include "MenuManager.h"

#include "extensions/Configuration.hpp"

inline struct MiscConfig {
    INI_CONFIG_SECTION("Misc");

    std::optional<std::string> SaveDirectoryPath{};
    bool LoadSavesWithMismatchingVersion{};
    bool SkipDeviceSelection{}; //< NOTSA: with several GPUs, skip the device/video mode dialog and reuse gta_sa.set (or the defaults)

    void Load() {
        STORE_INI_CONFIG_VALUE_OPT(SaveDirectoryPath);

        if (SaveDirectoryPath.has_value() && SaveDirectoryPath->size() >= 255) {
            SaveDirectoryPath->resize(255);
            NOTSA_LOG_WARN("La ruta personalizada del directorio de guardado es demasiado larga; se recorta a: '{}'", *SaveDirectoryPath);
        }

        STORE_INI_CONFIG_VALUE(LoadSavesWithMismatchingVersion, false);
        STORE_INI_CONFIG_VALUE(SkipDeviceSelection, false);
    }
} g_MiscConfig{};

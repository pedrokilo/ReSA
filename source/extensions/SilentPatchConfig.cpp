// SilentPatch (MIT, Silent): ver docs/THIRD_PARTY_NOTICES.md
#include "StdInc.h"
#include "SilentPatchConfig.h"

#include <cstdio>
#include <cstdlib>
#include <string>

namespace {
constexpr auto SP_INI_FILE = "gta-reversed.ini";
constexpr auto SP_INI_SECTION = "SilentPatch";

// -1 = no definido
int ReadEnv(const char* name) {
    char buf[16]{};
    const DWORD n = GetEnvironmentVariableA(name, buf, sizeof(buf));
    if (n == 0 || n >= sizeof(buf)) {
        return -1;
    }
    return buf[0] == '0' ? 0 : 1;
}

int ReadIni(const char* key) {
    char buf[16]{};
    if (GetPrivateProfileStringA(SP_INI_SECTION, key, "", buf, sizeof(buf), SP_INI_FILE) == 0) {
        return -1;
    }
    return (buf[0] == '0' || buf[0] == 'f' || buf[0] == 'F') ? 0 : 1;
}
} // namespace

bool SilentPatchEnabled() {
    static const bool enabled = []() {
        if (const int e = ReadEnv("GTASA_SILENTPATCH"); e != -1) {
            return e != 0;
        }
        if (const int i = ReadIni("SilentPatch"); i != -1) {
            return i != 0;
        }
        return true; // valor por defecto del interruptor general: ENCENDIDO (cambiar aqui o con GTASA_SILENTPATCH=0 / SilentPatch=0)
    }();
    return enabled;
}

bool SilentPatchOption(const char* name, bool defaultValue) {
    if (!SilentPatchEnabled()) {
        return false;
    }
    std::string env = "GTASA_SP_";
    for (const char* c = name; *c; ++c) {
        env += static_cast<char>(toupper(static_cast<unsigned char>(*c)));
    }
    if (const int e = ReadEnv(env.c_str()); e != -1) {
        return e != 0;
    }
    if (const int i = ReadIni(name); i != -1) {
        return i != 0;
    }
    return defaultValue;
}

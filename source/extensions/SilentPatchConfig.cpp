// SilentPatch (MIT, Silent): ver docs/THIRD_PARTY_NOTICES.md
#include "StdInc.h"
#include "SilentPatchConfig.h"

#include <string>

namespace {
constexpr auto SP_INI_FILE = ".\\gta-reversed.ini"; // con punto y barra delante: sin ruta, GetPrivateProfile* busca en el directorio de Windows
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

int ReadIniInt(const char* key, int defaultValue) {
    return static_cast<int>(GetPrivateProfileIntA(SP_INI_SECTION, key, defaultValue, SP_INI_FILE));
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
        env += (*c >= 'a' && *c <= 'z') ? static_cast<char>(*c - 32) : *c;
    }
    if (const int e = ReadEnv(env.c_str()); e != -1) {
        return e != 0;
    }
    if (const int i = ReadIni(name); i != -1) {
        return i != 0;
    }
    return defaultValue;
}

int SilentPatchIntOption(const char* name, int defaultValue, int disabledValue) {
    if (!SilentPatchEnabled()) {
        return disabledValue;
    }
    std::string env = "GTASA_SP_";
    for (const char* c = name; *c; ++c) {
        env += (*c >= 'a' && *c <= 'z') ? static_cast<char>(*c - 32) : *c;
    }
    char buf[16]{};
    if (const DWORD n = GetEnvironmentVariableA(env.c_str(), buf, sizeof(buf)); n != 0 && n < sizeof(buf)) {
        int  v    = 0;
        bool neg  = false;
        const char* c = buf;
        if (*c == '-') { neg = true; ++c; }
        for (; *c >= '0' && *c <= '9'; ++c) { v = v * 10 + (*c - '0'); }
        return neg ? -v : v;
    }
    return ReadIniInt(name, defaultValue);
}

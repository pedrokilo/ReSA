// Comprobacion SIN el juego (la lanza host_check.py): compila el CUERPO REAL de FxInfoManager_c::Load (extraido de source/game_sa/Fx/FxInfoManager.cpp por host_check.py, con el sscanf del
// EXE sustituido por el del CRT del host) junto a los falsos de Core.hpp (ReadLine, GetMem, AddFxInfo, Load virtual) y vuelca lo observable de cada uno de los 4608 casos, para compararlo
// con fixture_oracle.py. Valida a la vez Fixtures.hpp, Core.hpp (falsos y observacion), el oraculo y la logica de la fuente de produccion; NO valida el EXE ni el CRT del EXE.
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <vector>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using uint8 = std::uint8_t; using uint16 = std::uint16_t; using uint32 = std::uint32_t; using int8 = std::int8_t; using int32 = std::int32_t;
typedef FILE* FILESTREAM;
namespace ReversingProbe {
struct EntryPatch { template<class F> EntryPatch(uintptr_t, F) {} };
inline std::vector<uint8> Bytes(void* p, size_t n) { return std::vector<uint8>((uint8*)p, (uint8*)p + n); }
inline bool JumpsToDll(uintptr_t) { return false; }
inline void PumpMessages() {}
}
namespace OriginalDependencies499 {
class HookRoute {
public:
    struct RegisteredTarget {};
    HookRoute(const char*, const char*, uintptr_t, RegisteredTarget) {}
    uintptr_t Body() const { return 0; }
    bool Registered() const { return false; }
    template<class F> auto Invoke(bool, F call) { return call(); }
    bool Restore() { return true; }
};
}
#include "Core.hpp"
#include "eFxInfoType.h"

bool ReadLine(FILESTREAM f, char* b, int32 s) { return FxInfoLoad28Live::FakeReadLine(f, b, s); }
class FxInfoManager_c;
class FxInfo_c {
protected:
    eFxInfoType m_nType;
    bool        m_bTimeModeParticle;
public:
    virtual ~FxInfo_c() = 0;
    virtual void Load(FILESTREAM file, int32 version) = 0;
    virtual void GetValue(float, float, float, float, bool, void*) = 0;
    friend class FxInfoManager_c;
};
static_assert(sizeof(FxInfo_c) == 8);
struct FxManager_c {
    template <typename Type>
    Type* Allocate(int32 count) {
        const auto size = sizeof(Type) * count;
        const auto align = std::min(sizeof(Type), sizeof(int32));
        return (Type*)FxInfoLoad28Live::FakeGetMem((void*)uintptr_t(0xA9AF2C), nullptr, int32(size), int32(align));
    }
};
static FxManager_c g_fxMan_obj;
static FxManager_c& g_fxMan = g_fxMan_obj;
class FxInfoManager_c {
public:
    int32      m_nNumInfos;
    FxInfo_c** m_pInfos;
    int8       m_MovementOffset;
    int8       m_RenderOffset;
    char       _align[2];
    uint16     m_nLodStart;
    uint16     m_nLodEnd;
    bool       m_bHasFlatParticleEmitter;
    bool       m_bHasHeatHazeParticleEmitter;
    char       field_12;
    char       field_13;
    FxInfo_c* AddFxInfo(int32 info) { return (FxInfo_c*)FxInfoLoad28Live::FakeAddFx(this, nullptr, info); }
    void Load(FILESTREAM file, int32 version);
};
static_assert(sizeof(FxInfoManager_c) == 0x14);
static int host_sscanf(const char* s, const char* f, void* a, void* b) { return sscanf(s, f, a, b); }
#include "load_extract.inc"

int main() {
    using namespace FxInfoLoad28Live;
    InitFakes();
    for (unsigned n = 0; n < kCases; ++n) {
        const Fx f = Make(n);
        Refs* refs = new Refs();
        FillRef(f, n, *refs);
        ResetCase(f, *refs);
        reinterpret_cast<FxInfoManager_c*>(MgrObj())->Load(reinterpret_cast<FILESTREAM>(kFile), f.version);
        json row = {{"case", n}, {"observed", Observe(*refs)}};
        std::puts(row.dump().c_str());
        delete refs;
    }
    return 0;
}

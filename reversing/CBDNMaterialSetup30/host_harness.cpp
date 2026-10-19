// Banco de ANFITRION (sin juego) del banco CBDNMaterialSetup30: compila el TEXTO REAL de CustomPipeMaterialSetup extraido de
// source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp (host_check.py lo extrae a host_function.inc) contra una maqueta minima de los tipos del juego, lo ejecuta con las
// mismas fixtures (Fixtures.hpp) y vuelca lo observado en el mismo esquema que Core.hpp; host_check.py lo compara con fixture_oracle.py. Es una comprobacion de la FUENTE C++ contra el
// oraculo (que se modela desde el EXE); no sustituye al banco EXE-vs-DLL. Argumentos: envOff mfxOff.
#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <float.h>
#include <map>
#include <string>
#include "Fixtures.hpp"
using namespace CBDNMatFix;
using uint8 = std::uint8_t; using uint32 = std::uint32_t; using int32 = std::int32_t;

// ---- maqueta de los tipos / globales que usa la funcion
struct RpMaterial {};
struct RwTexture {};
struct CustomEnvMapPipeMaterialData { u8 pad[8]; RwTexture* Texture; };
enum RpMatFXMaterialFlags : int { rpMATFXEFFECTENVMAP = 2 };
static int32 MatFXMaterialDataOffset;
struct CCustomCarEnvMapPipeline { static inline int32 ms_envMapPluginOffset; };
template <class T> T& StaticRef(std::uintptr_t a) {
    static float f255, f254;
    if (a == 0x859A3C) { f255 = std::bit_cast<float>(0x3B808081u); return reinterpret_cast<T&>(f255); }
    if (a == 0x86BE90) { f254 = 254.0f; return reinterpret_cast<T&>(f254); }
    std::abort();
}
static RpMatFXMaterialFlags RpMatFXMaterialGetEffects(const RpMaterial* m) {
    auto* data = *reinterpret_cast<const u8* const*>(reinterpret_cast<const u8*>(m) + MatFXMaterialDataOffset);
    return data ? RpMatFXMaterialFlags(*reinterpret_cast<const u32*>(data + 0x30)) : RpMatFXMaterialFlags(0);
}

alignas(16) static std::array<u8, kMatArena> matArena;
alignas(16) static std::array<u8, kEnvArena> envAArena, envBArena;
alignas(16) static std::array<u8, kTexArena> tex0Arena, tex1Arena;
alignas(16) static std::array<u8, kMfxArena> mfxArena;
static u8* MatBase() { return matArena.data() + kMatAt; }
static u8* EnvA() { return envAArena.data() + kEnvAt; }
static u8* EnvB() { return envBArena.data() + kEnvAt; }
static u8* Tex0() { return tex0Arena.data() + kTexAt; }
static u8* Tex1() { return tex1Arena.data() + kTexAt; }
static u8* Mfx() { return mfxArena.data() + kMfxAt; }
static u32 P(const void* p) { return u32(std::uintptr_t(p)); }
static u32 PtrOfLabel(u32 l) {
    switch (l) { case kLblNull: return 0; case kLblEnvA: return P(EnvA()); case kLblEnvB: return P(EnvB()); case kLblTex0: return P(Tex0()); case kLblTex1: return P(Tex1()); case kLblMfx: return P(Mfx()); default: return l; }
}
static u32 LabelOfPtr(u32 p) {
    if (!p) return kLblNull;
    if (p == P(EnvA())) return kLblEnvA; if (p == P(EnvB())) return kLblEnvB; if (p == P(Tex0())) return kLblTex0; if (p == P(Tex1())) return kLblTex1; if (p == P(Mfx())) return kLblMfx;
    return p;
}
static u32 Dw(const u8* p) { u32 v; std::memcpy(&v, p, 4); return v; }
static void SetDw(u8* p, u32 v) { std::memcpy(p, &v, 4); }
static u32 curMode, curFlagsWord;
static std::string trace;
static void Push(u32 k, u32 a, u32 b) { char t[96]; std::snprintf(t, sizeof t, "%s[%u,%u,%u,0]", trace.empty() ? "" : ",", k, a, b); trace += t; }

struct CCustomBuildingDNPipeline {
    static RpMaterial* CustomPipeMaterialSetup(RpMaterial* material, void* data);
    static CustomEnvMapPipeMaterialData* SetFxEnvTexture(CustomEnvMapPipeMaterialData** slot) {   // el mismo falso que Core.hpp
        Push(1, u32(reinterpret_cast<u8*>(slot) - MatBase()), LabelOfPtr(P(*slot)));
        auto** s = reinterpret_cast<void**>(slot);
        switch (curMode) {
        case 0: return static_cast<CustomEnvMapPipeMaterialData*>(*s);
        case 1: *s = EnvA(); return reinterpret_cast<CustomEnvMapPipeMaterialData*>(EnvA());
        case 2: *s = nullptr; return nullptr;
        case 3: *s = EnvB(); return reinterpret_cast<CustomEnvMapPipeMaterialData*>(EnvB());
        case 4: return reinterpret_cast<CustomEnvMapPipeMaterialData*>(EnvA());
        case 5: *s = EnvA(); return nullptr;
        default: SetDw(MatBase() + 0x10, curFlagsWord); return static_cast<CustomEnvMapPipeMaterialData*>(*s);
        }
    }
};
#include "host_function.inc"

static std::string Hx(const u8* p, std::size_t n) { return Hex(p, n); }
static bool GuardOk(const u8* a, std::size_t size, std::size_t at, std::size_t len) { for (std::size_t i = 0; i < size; ++i) { if (i >= at && i < at + len) continue; if (a[i] != u8(kGuardByte)) return false; } return true; }

int main(int argc, char** argv) {
    const u32 envOff = argc >= 3 ? u32(std::strtoul(argv[1], nullptr, 0)) : 0x28, mfxOff = argc >= 3 ? u32(std::strtoul(argv[2], nullptr, 0)) : 0x1C;
    CCustomCarEnvMapPipeline::ms_envMapPluginOffset = int32(envOff);
    MatFXMaterialDataOffset = int32(mfxOff);
    _controlfp(_PC_24 | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM);
    for (unsigned n = 0; n < kCases; ++n) {
        const Fx f = Make(n);
        matArena.fill(u8(kGuardByte)); envAArena.fill(u8(kGuardByte)); envBArena.fill(u8(kGuardByte)); tex0Arena.fill(u8(kGuardByte)); tex1Arena.fill(u8(kGuardByte)); mfxArena.fill(u8(kGuardByte));
        const auto m = MatImage(f, envOff, mfxOff);
        std::memcpy(MatBase(), m.data(), kMatWin);
        SetDw(MatBase() + 0x10, f.preFlags);
        SetDw(MatBase() + envOff, PtrOfLabel(f.envPre));
        SetDw(MatBase() + mfxOff, f.mfxPresent ? P(Mfx()) : 0u);
        std::memcpy(EnvA(), f.envA.data(), kEnvSize); SetDw(EnvA() + 8, PtrOfLabel(f.envATex));
        std::memcpy(EnvB(), f.envB.data(), kEnvSize); SetDw(EnvB() + 8, PtrOfLabel(f.envBTex));
        std::memcpy(Tex0(), f.tex0.data(), kTexSize); std::memcpy(Tex1(), f.tex1.data(), kTexSize);
        std::memcpy(Mfx(), f.mfx.data(), kMfxSize); SetDw(Mfx() + 4, PtrOfLabel(f.mfxTex)); SetDw(Mfx() + 0x30, f.effects);
        trace.clear(); curMode = f.mode; curFlagsWord = f.flagsWord;
        RpMaterial* const ret = CCustomBuildingDNPipeline::CustomPipeMaterialSetup(reinterpret_cast<RpMaterial*>(MatBase()), reinterpret_cast<void*>(std::uintptr_t(f.data)));
        std::array<u8, kMatWin> mw; std::memcpy(mw.data(), MatBase(), kMatWin);
        const u32 flagsNow = Dw(MatBase() + 0x10), envSlot = LabelOfPtr(Dw(MatBase() + envOff)), mfxSlot = LabelOfPtr(Dw(MatBase() + mfxOff));
        SetDw(mw.data() + 0x10, 0); SetDw(mw.data() + envOff, 0); SetDw(mw.data() + mfxOff, 0);
        auto envDump = [&](u8* base) { std::array<u8, kEnvSize> d; std::memcpy(d.data(), base, kEnvSize); SetDw(d.data() + 8, 0); return Hx(d.data(), d.size()); };
        std::array<u8, kMfxSize> mf; std::memcpy(mf.data(), Mfx(), kMfxSize); SetDw(mf.data() + 4, 0); SetDw(mf.data() + 0x30, 0);
        std::printf("{\"ret_ok\":%s,\"flags\":%u,\"env_slot\":%u,\"mfx_slot\":%u,\"mat\":\"%s\",\"mat_guard_ok\":%s,", ret == reinterpret_cast<RpMaterial*>(MatBase()) ? "true" : "false", flagsNow, envSlot, mfxSlot,
                    Hx(mw.data(), mw.size()).c_str(), GuardOk(matArena.data(), matArena.size(), kMatAt, kMatWin) ? "true" : "false");
        std::printf("\"env_a\":\"%s\",\"env_a_tex\":%u,\"env_a_guard_ok\":%s,\"env_b\":\"%s\",\"env_b_tex\":%u,\"env_b_guard_ok\":%s,", envDump(EnvA()).c_str(), LabelOfPtr(Dw(EnvA() + 8)),
                    GuardOk(envAArena.data(), envAArena.size(), kEnvAt, kEnvSize) ? "true" : "false", envDump(EnvB()).c_str(), LabelOfPtr(Dw(EnvB() + 8)), GuardOk(envBArena.data(), envBArena.size(), kEnvAt, kEnvSize) ? "true" : "false");
        std::printf("\"tex0\":\"%s\",\"tex0_guard_ok\":%s,\"tex1\":\"%s\",\"tex1_guard_ok\":%s,\"mfx\":\"%s\",\"mfx_tex\":%u,\"mfx_eff\":%u,\"mfx_guard_ok\":%s,\"trace\":[%s]}\n",
                    Hx(Tex0(), kTexSize).c_str(), GuardOk(tex0Arena.data(), tex0Arena.size(), kTexAt, kTexSize) ? "true" : "false", Hx(Tex1(), kTexSize).c_str(),
                    GuardOk(tex1Arena.data(), tex1Arena.size(), kTexAt, kTexSize) ? "true" : "false", Hx(mf.data(), mf.size()).c_str(), LabelOfPtr(Dw(Mfx() + 4)), Dw(Mfx() + 0x30),
                    GuardOk(mfxArena.data(), mfxArena.size(), kMfxAt, kMfxSize) ? "true" : "false", trace.c_str());
    }
}

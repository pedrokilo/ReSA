#pragma once
// Banco dentro del motor: emisores de particulas de pisada de CPed::DoFootLanded.
//   lluvia: EXE 0x5E3630 (EBX = peaton, EDI = posicion, EAX = 4, arg de pila ignorado)  vs  EmitFootRainParticles (Ped.cpp)
//   polvo : EXE 0x5E37C0 (ESI = peaton, EBX = posicion, EAX = 4, arg de pila ignorado)  vs  EmitFootDustParticles (Ped.cpp)
// Las estaticas de Ped.cpp no son visibles desde Game.cpp: build_probe.py anade temporalmente a Ped.cpp tres envoltorios exportados
// (PedFootEmittersProbeRain/Dust/CtorEntry) y los quita siempre. El original es el codigo del EXE llamado por su direccion con un
// adaptador de registros (ensamblador permitido en el banco); el candidato es la estatica de la DLL llamada por su envoltorio (cdecl).
// Colaboradores externos simulados y trazados en AMBOS lados (EntryPatch en la direccion del EXE y en la entrada de la DLL):
//   CEntity::GetIsOnScreen 0x534540, FxPrtMult_c::FxPrtMult_c(7 floats) 0x4AB290, FxSystem_c::AddParticle 0x4AA440,
//   SurfaceInfos_c::ProducesFootDust 0x55E8B0, rand 0x821B1E (EXE) / CGeneral::GetRandomNumber (DLL).
// La traza (llamadas y argumentos en bits exactos) debe ser identica; tambien ESP, no volatiles, x87 vacia y estado restaurado.
#include <fstream>
#include <float.h>
#include <bit>
#include <array>
#include <vector>
#include <string>
#include "Game.h"
#include "Camera.h"
#include "General.h"
#include "SurfaceInfos_c.h"
#include "Fx/Fx.h"
#include "Fx/FxSystem.h"
#include "Fx/FxPrtMult.h"
#include "../ProbeRuntime.hpp"

// Envoltorios temporales anadidos por build_probe.py al final de Ped.cpp.
void PedFootEmittersProbeRain(CPed* ped, CVector* position);
void PedFootEmittersProbeDust(CPed* ped, CVector* position);
uintptr_t PedFootEmittersProbeCtorEntry();

namespace CppPedFootEmitters1Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,2> fns{{
    {"CPed","EmitFootRainParticles",0x5E3630,0,-1,1536},
    {"CPed","EmitFootDustParticles",0x5E37C0,0,-1,1536},
}};

constexpr uint64 kRainHash = 0x4DAFC6FF75E5C437ull, kDustHash = 0x4E36CDFDC14EDBD4ull; // FNV-1a 64 de los bytes del EXE original
constexpr size_t kRainSize = 0x190, kDustSize = 0x194;
constexpr uintptr_t kBytePtr = 0xB72C70, kCameraAddr = 0xB6F028, kFxAddr = 0xA9AE00, kSurfaceInfosAddr = 0xB79538;
constexpr size_t kCameraBlock = 0x40;

inline uint64 Fnv(const void* p, size_t n) {
    uint64 h = 0xCBF29CE484222325ull; const auto* b = static_cast<const uint8*>(p);
    for (size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 0x100000001B3ull; }
    return h;
}
template<class M> uintptr_t MemberAddress(M m) { static_assert(sizeof(M) == sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a, &m, sizeof(a)); return a; }
inline uintptr_t Body(uintptr_t p) { for (int k = 0; k < 4 && *reinterpret_cast<const uint8*>(p) == 0xE9; ++k) p = p + 5 + *reinterpret_cast<const int32*>(p + 1); return p; }
inline uint32 FBits(float f) { return std::bit_cast<uint32>(f); }
inline json Bits(const void* p, size_t bytes) { json a = json::array(); for (size_t i = 0; i < bytes; i += 4) { uint32 v; std::memcpy(&v, static_cast<const uint8*>(p) + i, 4); a.push_back(v); } return a; }

// RNG propio y determinista (xorshift32) para los fixtures: no depende del RNG del juego.
struct Rng {
    uint32 s;
    explicit Rng(uint32 seed) : s(seed * 2654435761u + 0x9E3779B9u) { if (!s) s = 1; for (int i = 0; i < 4; ++i) next(); }
    uint32 next() { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s; }
    float value(float scale) { return (float(int32(next() >> 8) - (1 << 23)) / float(1 << 23)) * scale; } // finito en [-scale, scale)
};

// ---------- memoria del fixture (alineada, con guardas) ----------
alignas(16) inline std::array<uint8, sizeof(CPed)> pedBuf{};
alignas(16) inline std::array<uint8, 0x60> pedMat{}, camMat{};
alignas(16) inline std::array<uint8, 16 + 12 + 16> posBuf{};
alignas(16) inline std::array<uint8, 0x40> splashDummy{}, smokeDummy{};
inline CPed* Ped() { return reinterpret_cast<CPed*>(pedBuf.data()); }
inline CVector* Pos() { return reinterpret_cast<CVector*>(posBuf.data() + 16); }

// ---------- estado de los falsos ----------
inline json trace = json::array();
inline bool onScreenRet{}, dustRet{};
inline uint32 rngState = 1;
inline uint32 rngCalls{};
inline const void* ctorThis{};

inline int NextRandom() {
    uint32 s = rngState; s ^= s << 13; s ^= s >> 17; s ^= s << 5; rngState = s; ++rngCalls;
    int v = int((s >> 9) & 0x7FFF);
    if (((s >> 3) & 7) == 0) { static const int edge[]{0, 1, 16383, 16384, 32766, 32767}; v = edge[(s >> 20) % 6]; }
    return v;
}
inline int __cdecl FRand() { const int v = NextRandom(); trace.push_back(json::array({"rand", v})); return v; }
inline uint16 __cdecl FRandomNumber() { const int v = NextRandom(); trace.push_back(json::array({"rand", v})); return uint16(v); }
inline bool __fastcall FOnScreen(CEntity* e, void*) { trace.push_back(json::array({"get_is_on_screen", static_cast<const void*>(e) == static_cast<const void*>(Ped())})); return onScreenRet; }
inline void* __fastcall FCtor(void* self, void*, uint32 a, uint32 b, uint32 c, uint32 d, uint32 e, uint32 f, uint32 g) {
    const uint32 v[7]{a, b, c, d, e, f, g};
    trace.push_back(json::array({"fx_prtmult_ctor", json::array({a, b, c, d, e, f, g})}));
    std::memcpy(self, v, sizeof(v)); // como el constructor real: color RGBA, tamano, rotacion, vida
    ctorThis = self;
    return self;
}
inline bool __fastcall FDust(void* self, void*, uint32 id) {
    trace.push_back(json::array({"produces_foot_dust", self == reinterpret_cast<void*>(kSurfaceInfosAddr), id}));
    return dustRet;
}
inline void __fastcall FAdd(void* sys, void*, const CVector* pos, const CVector* vel, uint32 t, const void* mult, uint32 rot, uint32 light, uint32 limit, uint32 createLocal) {
    const char* which = sys == splashDummy.data() ? "splash" : sys == smokeDummy.data() ? "smoke" : "other";
    trace.push_back(json::array({"add_particle", which, Bits(pos, 12), Bits(vel, 12), t, Bits(mult, 28), mult == ctorThis, rot, light, limit, createLocal}));
}

// ---------- llamadas ----------
struct Abi { bool stack, registers, x87; };
inline bool X87Empty(const uint8* env) { uint16 tag; std::memcpy(&tag, env + 8, 2); return tag == 0xFFFF; }

// EXE: los argumentos van en registros (ver README). Se guardan y restauran EBX/ESI/EDI en variables locales; se comprueba que el cuerpo
// del EXE los conserva (ESP, EBX, ESI, EDI, EBP) y que deja la pila x87 vacia. El argumento de pila (ignorado por el EXE) se aleatoriza.
__declspec(noinline) inline Abi CallExe(uintptr_t fn, uint32 ebxIn, uint32 esiIn, uint32 ediIn, uint32 stackArg) {
    uintptr_t espBefore{}, espAfter{}, ebpBefore{}, ebpAfter{};
    uint32 ebxAfter{}, esiAfter{}, ediAfter{}, savedEbx{}, savedEsi{}, savedEdi{};
    alignas(4) uint8 env[32]{};
    __asm {
        mov savedEbx, ebx
        mov savedEsi, esi
        mov savedEdi, edi
        mov espBefore, esp
        mov ebpBefore, ebp
        mov ebx, ebxIn
        mov esi, esiIn
        mov edi, ediIn
        mov edx, stackArg
        mov ecx, fn
        push edx
        mov eax, 4
        call ecx
        add esp, 4
        mov espAfter, esp
        mov ebpAfter, ebp
        mov ebxAfter, ebx
        mov esiAfter, esi
        mov ediAfter, edi
        mov ebx, savedEbx
        mov esi, savedEsi
        mov edi, savedEdi
        lea eax, env
        fnstenv [eax]
        fldenv [eax]
    }
    return {espBefore == espAfter, ebxAfter == ebxIn && esiAfter == esiIn && ediAfter == ediIn && ebpBefore == ebpAfter, X87Empty(env)};
}
// DLL: envoltorio cdecl (ped, posicion).
__declspec(noinline) inline Abi CallDll(uintptr_t fn, uint32 a0, uint32 a1) {
    uintptr_t espBefore{}, espAfter{}, ebxBefore{}, ebxAfter{}, esiBefore{}, esiAfter{}, ediBefore{}, ediAfter{}, ebpBefore{}, ebpAfter{};
    alignas(4) uint8 env[32]{};
    __asm {
        mov espBefore, esp
        mov ebxBefore, ebx
        mov esiBefore, esi
        mov ediBefore, edi
        mov ebpBefore, ebp
        mov edx, a1
        mov ecx, a0
        mov eax, fn
        push edx
        push ecx
        call eax
        add esp, 8
        mov espAfter, esp
        mov ebxAfter, ebx
        mov esiAfter, esi
        mov ediAfter, edi
        mov ebpAfter, ebp
        lea eax, env
        fnstenv [eax]
        fldenv [eax]
    }
    return {espBefore == espAfter, ebxBefore == ebxAfter && esiBefore == esiAfter && ediBefore == ediAfter && ebpBefore == ebpAfter, X87Empty(env)};
}

// ---------- fixtures ----------
struct Fixture {
    std::array<uint32, 3> pos, fwd, cam, camDecoy;
    bool camMatrix, onScreen, dust;
    uint32 surface, byteFlag, seed, stackArg, cls;
};
inline const std::array<uint32, 17> kSpecial{0x7FC00000u, 0xFFC00000u, 0x7FA00001u, 0xFFA55555u, 0x7F800000u, 0xFF800000u, 0x80000000u, 0x00000000u, 0x00000001u,
    0x807FFFFFu, 0x00800000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x3F800000u, 0xC2C80000u, 0x42C80000u, 0x7E967699u};
inline const char* const kClassNames[16]{"near0", "near1", "near2", "near3", "threshold", "far", "huge", "pos_x_special", "pos_y_special", "pos_z_special",
    "pos_xy_special", "camera_special", "forward_special", "forward_large", "near_mixed", "offscreen"};
inline Fixture MakeFixture(unsigned kind, unsigned idx) {
    Rng r(idx * 4u + kind + 1u);
    Fixture f{};
    f.cls = idx % 16;
    const float camScale = (idx % 3 == 0) ? 10.0f : (idx % 3 == 1) ? 1000.0f : 3000.0f;
    float cam[3]{r.value(camScale), r.value(camScale), r.value(camScale)};
    if (f.cls == 4 && idx % 4 != 0) { cam[0] = 0.0f; cam[1] = 0.0f; } // posicion = desplazamiento exacto
    static const float thr[16]{10.0f, 0.0f, -10.0f, 6.0f, 8.0f, -6.0f, -8.0f, 7.0710678f, -7.0710678f, 10.000001f, 9.999999f, -10.000001f, -9.999999f, 5.0f, 8.660254f, -5.0f};
    float d[2]{};
    switch (f.cls) {
    case 4: d[0] = thr[r.next() % 16]; d[1] = thr[r.next() % 16]; break;
    case 5: d[0] = r.value(2000.0f); d[1] = r.value(2000.0f); break;
    case 6: d[0] = r.value(1e30f); d[1] = r.value(1e30f); break;
    default: d[0] = r.value(9.0f); d[1] = r.value(9.0f); break;
    }
    f.pos = {FBits(cam[0] + d[0]), FBits(cam[1] + d[1]), FBits(r.value(100.0f))};
    auto special = [&] { return kSpecial[r.next() % kSpecial.size()]; };
    switch (f.cls) {
    case 7: f.pos[0] = special(); break;
    case 8: f.pos[1] = special(); break;
    case 9: f.pos[2] = special(); break;
    case 10: f.pos[0] = special(); f.pos[1] = special(); break;
    default: break;
    }
    f.cam = {FBits(cam[0]), FBits(cam[1]), FBits(cam[2])};
    if (f.cls == 11) f.cam[r.next() % 2] = special();
    f.camDecoy = {FBits(r.value(3000.0f)), FBits(r.value(3000.0f)), FBits(r.value(3000.0f))};
    const float fwdScale = f.cls == 13 ? 1e30f : 1.0f;
    f.fwd = {FBits(r.value(fwdScale)), FBits(r.value(fwdScale)), FBits(r.value(fwdScale))};
    if (f.cls == 12) for (auto& v : f.fwd) if (r.next() % 2) v = special();
    if (idx % 11 == 5) f.fwd[r.next() % 3] = 0x00000001u + r.next() % 8u; // denormales
    f.camMatrix = r.next() & 1;
    f.onScreen = !(idx % 7 == 3 || f.cls == 15);
    f.dust = idx % 3 != 0;
    f.surface = r.next() & 0xFF;
    static const uint32 bytes[8]{0, 0, 0, 0, 1, 2, 0x80, 0xFF};
    f.byteFlag = bytes[r.next() % 8];
    f.seed = idx * 7919u + kind * 104729u + 1u;
    f.stackArg = r.next();
    return f;
}

inline json FixtureJson(const Fixture& f, const char* path) {
    return {{"class", kClassNames[f.cls]}, {"path", path}, {"pos", f.pos}, {"fwd", f.fwd}, {"cam", f.cam}, {"cam_matrix", f.camMatrix}, {"on_screen", f.onScreen},
            {"dust_ret", f.dust}, {"surface", f.surface}, {"byte_0xB72C70", f.byteFlag}, {"seed", f.seed}, {"stack_arg", f.stackArg}};
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "cpppedfootemitters1-runtime.start")) return;
    if (!log.is_open()) {
        log.open(dir / "cpppedfootemitters1-runtime.jsonl");
        ReversingProbe::InstallFailureCapture("cpppedfootemitters1");
        Write({{"event", "waiting_for_player"}});
    }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done = true;

    using ReversingProbe::EntryPatch;
    // --- rutas ---
    const std::array<uintptr_t, 2> dllEntry{std::bit_cast<uintptr_t>(&PedFootEmittersProbeRain), std::bit_cast<uintptr_t>(&PedFootEmittersProbeDust)};
    const std::array<uintptr_t, 2> dllBody{Body(dllEntry[0]), Body(dllEntry[1])};
    const bool exeUnhooked = !ReversingProbe::JumpsToDll(fns[0].address) && !ReversingProbe::JumpsToDll(fns[1].address);
    const bool dllOutside = dllEntry[0] >= 0x1000000 && dllEntry[1] >= 0x1000000 && dllBody[0] >= 0x1000000 && dllBody[1] >= 0x1000000;
    const bool direct = exeUnhooked && dllOutside && dllEntry[0] != fns[0].address && dllEntry[1] != fns[1].address;
    const bool exeBytesOK = Fnv(reinterpret_cast<void*>(fns[0].address), kRainSize) == kRainHash && Fnv(reinterpret_cast<void*>(fns[1].address), kDustSize) == kDustHash;

    // --- estado a restaurar ---
    const auto savedCamera = ReversingProbe::Bytes(reinterpret_cast<void*>(kCameraAddr), kCameraBlock);
    const auto savedFx = ReversingProbe::Bytes(&g_fx, sizeof(Fx_c));
    const uint8 savedByte = StaticRef<uint8>(kBytePtr);
    const unsigned savedFP = _controlfp(0, 0);
    // --- direcciones y disposicion de los datos (los desplazamientos crudos que lee el EXE deben coincidir con los de la DLL) ---
    auto* const pedBase = pedBuf.data();
    Ped()->m_matrix = reinterpret_cast<CMatrixLink*>(pedMat.data());
    TheCamera.m_matrix = reinterpret_cast<CMatrixLink*>(camMat.data());
    const bool layoutOK =
        reinterpret_cast<uint8*>(&Ped()->m_matrix) - pedBase == 0x14 &&
        reinterpret_cast<uint8*>(&Ped()->m_nContactSurface) - pedBase == 0xBA &&
        reinterpret_cast<uint8*>(&Ped()->GetMatrix().GetForward()) - pedMat.data() == 0x10 &&
        reinterpret_cast<uint8*>(&TheCamera) == reinterpret_cast<uint8*>(kCameraAddr) &&
        reinterpret_cast<uint8*>(&TheCamera.m_matrix) - reinterpret_cast<uint8*>(&TheCamera) == 0x14 &&
        reinterpret_cast<uint8*>(&TheCamera.GetPosition()) == camMat.data() + 0x30 &&
        reinterpret_cast<uint8*>(&g_fx) == reinterpret_cast<uint8*>(kFxAddr) &&
        reinterpret_cast<uint8*>(&g_fx.m_SmokeII3expand) - reinterpret_cast<uint8*>(&g_fx) == 0x24 &&
        reinterpret_cast<uint8*>(&g_fx.m_Splash) - reinterpret_cast<uint8*>(&g_fx) == 0x30 &&
        reinterpret_cast<uint8*>(&g_surfaceInfos) == reinterpret_cast<uint8*>(kSurfaceInfosAddr) &&
        &StaticRef<uint8>(kBytePtr) == reinterpret_cast<uint8*>(kBytePtr);
    std::memcpy(reinterpret_cast<void*>(kCameraAddr), savedCamera.data(), kCameraBlock); // deshace la prueba de disposicion

    // Los falsos y sus destinos: EXE y DLL.
    const uintptr_t ctorDll = Body(PedFootEmittersProbeCtorEntry());
    const std::array<uintptr_t, 5> dllCollab{
        Body(MemberAddress(&CEntity::GetIsOnScreen)), ctorDll,
        Body(MemberAddress(static_cast<void (FxSystem_c::*)(const CVector&, const CVector&, float, const FxPrtMult_c&, float, float, float, bool)>(&FxSystem_c::AddParticle))),
        Body(MemberAddress(&SurfaceInfos_c::ProducesFootDust)), Body(std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber))};
    const std::array<uintptr_t, 5> exeCollab{0x534540, 0x4AB290, 0x4AA440, 0x55E8B0, 0x821B1E};
    bool collabOK = true;
    // ProducesFootDust no esta reconstruida en la DLL: su simbolo es la propia direccion del EXE (un solo EntryPatch sirve a los dos lados).
    for (size_t k = 0; k < 5; ++k) collabOK &= (k == 3 ? dllCollab[k] == exeCollab[k] : dllCollab[k] >= 0x1000000) && exeCollab[k] < 0x1000000;
    // entries: direcciones que nunca se parchean; patched: las de los colaboradores (cambian mientras viven los EntryPatch).
    std::vector<std::pair<uintptr_t, std::vector<uint8>>> entries, patched;
    auto watch = [&](auto& list, uintptr_t at) { list.push_back({at, ReversingProbe::Bytes(reinterpret_cast<void*>(at), 16)}); };
    for (auto at : {fns[0].address, fns[1].address, dllEntry[0], dllEntry[1], dllBody[0], dllBody[1]}) watch(entries, at);
    for (auto at : dllCollab) watch(patched, at);
    for (auto at : exeCollab) watch(patched, at);
    auto restore = [&] {
        std::memcpy(reinterpret_cast<void*>(kCameraAddr), savedCamera.data(), kCameraBlock);
        std::memcpy(&g_fx, savedFx.data(), sizeof(Fx_c));
        StaticRef<uint8>(kBytePtr) = savedByte;
        _controlfp(savedFP, _MCW_PC | _MCW_RC);
    };
    auto restored = [&](bool includePatched) {
        bool ok = std::memcmp(reinterpret_cast<void*>(kCameraAddr), savedCamera.data(), kCameraBlock) == 0 && std::memcmp(&g_fx, savedFx.data(), sizeof(Fx_c)) == 0
            && StaticRef<uint8>(kBytePtr) == savedByte && _controlfp(0, 0) == savedFP;
        for (auto& [at, bytes] : entries) ok &= bytes == ReversingProbe::Bytes(reinterpret_cast<void*>(at), 16);
        if (includePatched) for (auto& [at, bytes] : patched) ok &= bytes == ReversingProbe::Bytes(reinterpret_cast<void*>(at), 16);
        return ok;
    };
    const int totalPairs = fns[0].cases + fns[1].cases;
    Write({{"event", "begin"}, {"functions", 2}, {"pairs", totalPairs}, {"registered_hook", false}, {"direct_dll_call", direct}, {"exe_unhooked", exeUnhooked},
           {"exe_bytes_ok", exeBytesOK}, {"layout_ok", layoutOK}, {"collaborators_ok", collabOK}, {"dll_collaborators", dllCollab}, {"exe_collaborators", exeCollab}, {"collaborators_stubbed", true},
           {"dll_entry", json::array({dllEntry[0], dllEntry[1]})}, {"dll_body", json::array({dllBody[0], dllBody[1]})},
           {"exe_entry", json::array({fns[0].address, fns[1].address})}, {"fp_control", savedFP}});
    if (!direct || !exeBytesOK || !layoutOK || !collabOK) {
        restore(); const bool stateOK = restored(true);
        Write({{"event", "error"}, {"reason", "precondiciones del banco: ruta directa, bytes del EXE, disposicion de datos o colaboradores"},
               {"direct", direct}, {"exe_bytes_ok", exeBytesOK}, {"layout_ok", layoutOK}, {"collaborators_ok", collabOK}, {"dll_collaborators", dllCollab}, {"state_restored", stateOK}});
        return;
    }

    unsigned pair = 0; bool passed = true, allRestored = true;
    {
        // Falsos en la direccion del EXE y en la entrada de la DLL (la DLL llama a sus propias entradas).
        EntryPatch a1(dllCollab[0], FOnScreen), a1e(exeCollab[0], FOnScreen);
        EntryPatch a2(dllCollab[1], FCtor), a2e(exeCollab[1], FCtor);
        EntryPatch a3(dllCollab[2], FAdd), a3e(exeCollab[2], FAdd);
        EntryPatch a4e(exeCollab[3], FDust);
        EntryPatch a5(dllCollab[4], FRandomNumber), a5e(exeCollab[4], FRand);

        for (size_t f = 0; f < fns.size(); ++f) {
            int failures = 0;
            const bool rain = f == 0;
            const std::string name = std::string(fns[f].cls) + "::" + fns[f].name;
            for (int idx = 0; idx < fns[f].cases; ++idx, ++pair) {
                const Fixture fx = MakeFixture(unsigned(f), unsigned(idx));
                auto apply = [&] {
                    for (size_t k = 0; k < pedBuf.size(); ++k) pedBuf[k] = uint8(0xA5u ^ (k * 131u + unsigned(idx)));
                    for (size_t k = 0; k < pedMat.size(); ++k) pedMat[k] = uint8(0x3Cu ^ (k * 29u + unsigned(idx)));
                    for (size_t k = 0; k < camMat.size(); ++k) camMat[k] = uint8(0xD2u ^ (k * 17u + unsigned(idx)));
                    for (size_t k = 0; k < posBuf.size(); ++k) posBuf[k] = uint8(0x6Bu ^ (k * 53u + unsigned(idx)));
                    Ped()->m_matrix = reinterpret_cast<CMatrixLink*>(pedMat.data());
                    Ped()->m_nContactSurface = static_cast<eSurfaceType>(fx.surface);
                    std::memcpy(pedMat.data() + 0x10, fx.fwd.data(), 12);
                    std::memcpy(posBuf.data() + 16, fx.pos.data(), 12);
                    // camara: con matriz (posicion en +0x30) o sin ella (posicion compacta en +4); la otra posicion es un senuelo
                    std::memcpy(camMat.data() + 0x30, fx.camMatrix ? fx.cam.data() : fx.camDecoy.data(), 12);
                    std::memcpy(&TheCamera.m_placement.m_vPosn, (fx.camMatrix ? fx.camDecoy : fx.cam).data(), 12);
                    TheCamera.m_matrix = fx.camMatrix ? reinterpret_cast<CMatrixLink*>(camMat.data()) : nullptr;
                    g_fx.m_Splash = reinterpret_cast<FxSystem_c*>(splashDummy.data());
                    g_fx.m_SmokeII3expand = reinterpret_cast<FxSystem_c*>(smokeDummy.data());
                    StaticRef<uint8>(kBytePtr) = uint8(fx.byteFlag);
                    onScreenRet = fx.onScreen; dustRet = fx.dust;
                    rngState = fx.seed ? fx.seed : 1; rngCalls = 0; ctorThis = nullptr;
                    trace = json::array();
                };
                auto run = [&](bool candidate) -> json {
                    restore(); apply();
                    const auto pedBefore = pedBuf;
                    const auto pedMatBefore = pedMat;
                    const auto camMatBefore = camMat;
                    const auto posBefore = posBuf;
                    const auto cameraBefore = ReversingProbe::Bytes(reinterpret_cast<void*>(kCameraAddr), kCameraBlock);
                    const auto fxBefore = ReversingProbe::Bytes(&g_fx, sizeof(Fx_c));
                    const uint8 byteBefore = StaticRef<uint8>(kBytePtr);
                    const uint32 pedPtr = uint32(uintptr_t(pedBuf.data())), posPtr = uint32(uintptr_t(posBuf.data() + 16));
                    _controlfp(_PC_24 | _RC_NEAR, _MCW_PC | _MCW_RC);
                    Abi abi{};
                    if (candidate) abi = CallDll(dllEntry[f], pedPtr, posPtr);
                    else if (rain) abi = CallExe(fns[f].address, pedPtr /*EBX*/, 0x5E5E5E5Eu /*ESI*/, posPtr /*EDI*/, fx.stackArg);
                    else abi = CallExe(fns[f].address, posPtr /*EBX*/, pedPtr /*ESI*/, 0xD1D1D1D1u /*EDI*/, fx.stackArg);
                    const unsigned fpAfter = _controlfp(0, 0);
                    _controlfp(savedFP, _MCW_PC | _MCW_RC);
                    const bool protectedOK = pedBuf == pedBefore && pedMat == pedMatBefore && camMat == camMatBefore && posBuf == posBefore
                        && ReversingProbe::Bytes(reinterpret_cast<void*>(kCameraAddr), kCameraBlock) == cameraBefore
                        && ReversingProbe::Bytes(&g_fx, sizeof(Fx_c)) == fxBefore && StaticRef<uint8>(kBytePtr) == byteBefore
                        && (fpAfter & (_MCW_PC | _MCW_RC)) == (_PC_24 | _RC_NEAR);
                    const bool routeOK = candidate ? dllBody[f] >= 0x1000000 : !ReversingProbe::JumpsToDll(fns[f].address);
                    json result{{"route_ok", routeOK}, {"stack_restored", abi.stack}, {"nonvolatile_registers_preserved", abi.registers}, {"x87_empty", abi.x87},
                                {"protected_state_unchanged", protectedOK}, {"rand_calls", rngCalls}, {"trace", trace}, {"ped_hash", Fnv(pedBuf.data(), pedBuf.size())}};
                    restore();
                    return result;
                };
                const auto original = run(false), candidate = run(true);
                // Ruta: la deducida de la traza del original (solo informativa; la cobertura la exige el analizador).
                const char* path = "emit";
                if (!fx.onScreen) path = "offscreen";
                else if (original["trace"].size() < 2 || original["trace"][1][0] != "fx_prtmult_ctor") path = "far";
                else if (!rain && !fx.dust) path = "no_dust";
                else if (!rain && fx.byteFlag != 0) path = "blocked_by_byte";
                const bool stateOK = restored(false), equal = original == candidate;
                const bool accepted = equal && stateOK && original["route_ok"] == true && original["stack_restored"] == true && original["nonvolatile_registers_preserved"] == true
                    && original["x87_empty"] == true && original["protected_state_unchanged"] == true;
                failures += !accepted; passed &= accepted; allRestored &= stateOK;
                Write({{"event", "comparison"}, {"function", name}, {"pair", pair}, {"case", idx}, {"fixture", FixtureJson(fx, path)},
                       {"equal", equal}, {"accepted", accepted}, {"state_restored", stateOK}, {"original", original}, {"candidate", candidate}});
            }
            Write({{"event", "function_summary"}, {"function", name}, {"cases", fns[f].cases}, {"failures", failures}});
        }
    }
    restore(); allRestored &= restored(true);
    Write({{"event", passed && allRestored ? "complete" : "error"}, {"pairs", pair}, {"state_restored", allRestored}});
}
}

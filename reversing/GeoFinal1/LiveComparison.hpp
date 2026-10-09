#pragma once
// Banco dentro del motor (flujo viejo, simple, estilo Small2/PadMouse1): GeoFinal1, 8 funciones de 4 clases.
//   CCollision::SphereVsEntity                           0x41A5A0  (cdecl; ignora su parametro y lee 16 bytes de basura de pila)
//   CPlayerPed::SetupPlayerPed                           0x60D790
//   CPedGeometryAnalyser::CanPedTargetPed                0x5F1C40
//   CPedGeometryAnalyser::ComputeClosestSurfacePoint-ped 0x5F3B70
//   CRealTimeShadowManager::{DoShadowThisFrame 0x706BA0, Exit 0x706A60, Init 0x7067C0, Update 0x706AB0}
// Original (hook desactivado) frente a candidato (hook activo) sobre OBJETOS FALSOS (buffers con guardas de 16 bytes);
// ningun singleton real se usa salvo CWorld::Players, PlayerInFocus y los arrays de sombras del CTimeCycle, que se
// guardan al empezar y se restauran tras cada ejecucion. Colaboradores simulados con EntryPatch en la direccion del EXE y
// en el simbolo de la DLL (miembro/hilo del banco unicamente: los hilos ajenos no tocan el registro de trazas).
#include <fstream>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Collision.h"
#include "ColModel.h"
#include "CollisionData.h"
#include "PedGeometryAnalyser.h"
#include "PlayerPed.h"
#include "RealTimeShadow.h"
#include "RealTimeShadowManager.h"
#include "ShadowCamera.h"
#include "Shadows.h"
#include "TimeCycle.h"
#include "SimpleTransform.h"
#include "../ProbeRuntime.hpp"

namespace GeoFinal1Live {
using Hook = ReversibleHooks::ReversibleHook::Base;
inline bool done{}; inline std::ofstream log;
// Hilo del banco: otros hilos del juego (audio) llaman a funciones parcheadas; sus llamadas no se trazan.
inline DWORD owner{};
inline bool Mine() { return GetCurrentThreadId() == owner; }
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
inline uint64 Fnv(const void* p, size_t n, uint64 h = 1469598103934665603ull) { auto* b = static_cast<const uint8*>(p); for (size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 1099511628211ull; } return h; }
inline uint32 P(const void* p) { return uint32(uintptr_t(p)); }
inline uint32 Bits(float f) { return std::bit_cast<uint32>(f); }
inline uint32 Rnd(uint32& s) { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s; }
inline uint32 Seed(unsigned i, uint32 salt) { uint32 s = i * 2654435761u + salt * 0x9E3779B9u + 1u; if (!s) s = 1; for (int k = 0; k < 4; ++k) Rnd(s); return s; }
template<class T> inline T& At(uint8* base, size_t off) { return *reinterpret_cast<T*>(base + off); }

// Buffer con guardas de 16 bytes a cada lado (se comparan enteros: detectan escrituras fuera de rango).
template<size_t N> struct Buf {
    alignas(16) std::array<uint8, N + 32> raw{};
    uint8* p() { return raw.data() + 16; }
    void Fill(uint8 v) { raw.fill(v); }
    std::vector<uint8> Bytes() const { return { raw.begin(), raw.end() }; }
};

// Floats de los fixtures. Se evitan magnitudes que desborden/infradesborden al elevar al cuadrado: el x87 conserva el
// exponente extendido y SSE no. Hay inf y NaN silencioso (nunca NaN de senalizacion).
inline float PickNear(uint32& s) { static constexpr float t[] = { 0.f, 0.5f, -0.5f, 1.f, -1.f, 2.f, -2.f, 3.f, -3.f, 1.5f, -1.5f }; return t[Rnd(s) % std::size(t)]; }
inline float PickRand(uint32& s) { const uint32 r = Rnd(s); return std::bit_cast<float>((r & 0x80000000u) | ((100u + Rnd(s) % 50u) << 23) | (r & 0x7FFFFFu)); }
inline float PickF(uint32& s) {
    static constexpr float t[] = { 0.f, 1.f, -1.f, 0.5f, -0.5f, 2.5f, -2.5f, 0.75f, -0.25f, 1000.125f, -1000.125f, 123456.7f, -123456.7f, 1e10f, -3e14f, 1e-3f, -1e-3f, 7.f, -7.f, 0.1f,
        std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), std::bit_cast<float>(0x7FC00000u) };
    if (Rnd(s) % 100 < 35) return PickRand(s);
    return t[Rnd(s) % std::size(t)];
}

// --- registro de trazas sin reservas (valido mientras operator new/delete estan sustituidos) ---
struct Rec { uint32 tag; uint32 n; uint32 a[8]; };
inline std::array<Rec, 2048> recs{}; inline size_t nrec{};
inline void Push(uint32 t, std::initializer_list<uint32> v = {}) {
    if (!Mine() || nrec >= recs.size()) return;
    auto& r = recs[nrec++]; r = {}; r.tag = t; for (auto x : v) if (r.n < 8) r.a[r.n++] = x;
}
inline json Recs() { json r = json::array(); for (size_t i = 0; i < nrec; ++i) { json e = json::array({ recs[i].tag }); for (uint32 k = 0; k < recs[i].n; ++k) e.push_back(recs[i].a[k]); r.push_back(e); } return r; }

// =====================================================================================================================
// CCollision::SphereVsEntity
// =====================================================================================================================
inline Buf<0x200> sveEnt; inline Buf<0x50> sveMat; inline Buf<0x40> sveCm; inline Buf<0x30> sveCd; inline Buf<0x400> sveMem; inline Buf<0x20> sveDecoy;
inline std::array<uint8, 0x48> sveMatInit{};
inline uint32 sveBoxMask{}, sveTriMask{}, sveBoxCalls{}, sveTriCalls{}; inline bool sveIgnore{};
constexpr size_t SVE_SPH = 0x000, SVE_BOX = 0x040, SVE_VERT = 0x0A0, SVE_TRIS = 0x158, SVE_PLANES = 0x180;

inline bool __fastcall SveIgnore(void* self, void*, void* ent) { if (!Mine()) return false; Push(10, { P(self), P(ent) }); return sveIgnore; }
inline void* __fastcall SveColModel(void* self, void*) { if (!Mine()) return nullptr; Push(11, { P(self) }); return sveCm.p(); }
inline void __fastcall SveAlloc(void* self, void*) { if (!Mine()) return; Push(12, { P(self) }); At<void*>(static_cast<uint8*>(self), 0x14) = sveMat.p(); }
inline void __fastcall SveUpdateMatrix(void* self, void*, void* out) { if (!Mine()) return; Push(13, { P(self), P(out) }); std::memcpy(out, sveMatInit.data(), 0x48); }
inline void __cdecl SveTrianglePlanes(void* cd) { if (!Mine()) return; Push(14, { P(cd) }); }
inline bool __cdecl SveTestBox(const void* s, const void* b) {
    if (!Mine()) return false; const auto* f = static_cast<const uint32*>(s);
    Push(15, { P(b), f[0], f[1], f[2], f[3] }); return (sveBoxMask >> (sveBoxCalls++ % 32)) & 1;
}
inline bool __cdecl SveTestTri(const void* s, const void* verts, const void* tri, const void* plane) {
    if (!Mine()) return false; const auto* f = static_cast<const uint32*>(s);
    Push(16, { f[0], f[1], f[2], f[3], P(verts), P(tri), P(plane) }); return (sveTriMask >> (sveTriCalls++ % 32)) & 1;
}

// Llama a fn(sphere, entity) (cdecl) dejando los 16 bytes de 'seed' justo donde el EXE lee su CColSphere local sin
// inicializar: [S-0x68 .. S-0x59], con S = ESP al entrar (direccion del retorno). Sin push/call entre la escritura y la llamada
// que pisen ese rango. Devuelve EAX (el EXE solo define AL).
inline __declspec(naked) uint32 __cdecl CallSve(uintptr_t fn, void* sphere, void* entity, const uint32* seed) {
    __asm {
        push ebp
        mov ebp, esp
        push esi
        mov esi, dword ptr [ebp+20]
        sub esp, 0x80
        mov eax, dword ptr [esi]
        mov dword ptr [esp-0x74], eax
        mov eax, dword ptr [esi+4]
        mov dword ptr [esp-0x70], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [esp-0x6C], eax
        mov eax, dword ptr [esi+12]
        mov dword ptr [esp-0x68], eax
        push dword ptr [ebp+16]
        push dword ptr [ebp+12]
        call dword ptr [ebp+8]
        add esp, 8
        add esp, 0x80
        pop esi
        pop ebp
        ret
    }
}

inline json CaseSve(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x41A5);
    auto R = [&]() { return Rnd(s); };
    sveEnt.Fill(0xCD); sveMat.Fill(0xCD); sveCm.Fill(0xCD); sveCd.Fill(0xCD); sveMem.Fill(0xCD); sveDecoy.Fill(0xEE);
    const bool uses = R() % 100 < 85, hasMat = R() % 100 < 60, hasData = R() % 100 >= 8;
    sveIgnore = R() % 100 < 15;
    // matriz (se usa para Invert; la salida se descarta) y su copia que reescribe UpdateMatrix
    uint8* m = sveMat.p();
    for (size_t off : { 0x00u, 0x04u, 0x08u, 0x10u, 0x14u, 0x18u, 0x20u, 0x24u, 0x28u, 0x30u, 0x34u, 0x38u }) At<float>(m, off) = (R() % 100 < 90) ? PickNear(s) : PickF(s);
    At<uint32>(m, 0x40) = 0; At<uint32>(m, 0x44) = 0;
    std::memcpy(sveMatInit.data(), m, 0x48);
    // entidad: +0x14 matriz, +0x1C banderas (bit 0 = usa colision)
    uint8* e = sveEnt.p();
    At<uint8>(e, 0x1C) = uint8((R() & 0xFE) | (uses ? 1 : 0));
    At<void*>(e, 0x14) = hasMat ? static_cast<void*>(m) : nullptr;
    // modelo de colision: +0x2C datos
    At<void*>(sveCm.p(), 0x2C) = hasData ? static_cast<void*>(sveCd.p()) : nullptr;
    // datos de colision
    uint8* cd = sveCd.p(); uint8* mem = sveMem.p();
    auto Cnt3 = [&]() -> uint16 { const uint32 k = R() % 16; return k < 4 ? 0 : k < 8 ? 1 : k < 11 ? 2 : k < 14 ? 3 : k == 14 ? 0xFFFF : 0x8000; };
    auto Cnt4 = [&]() -> uint16 { const uint32 k = R() % 16; return k < 3 ? 0 : k < 6 ? 1 : k < 9 ? 2 : k < 11 ? 3 : k < 14 ? 4 : 0xFFFF; };
    At<uint16>(cd, 0) = Cnt3(); At<uint16>(cd, 2) = Cnt3(); At<uint16>(cd, 4) = Cnt4();
    At<uint8>(cd, 6) = uint8(R());
    At<uint8>(cd, 7) = uint8((R() & 0xFD) | ((R() & 1) ? 2 : 0));   // bit 1 = grupos de caras
    At<void*>(cd, 0x08) = mem + SVE_SPH; At<void*>(cd, 0x0C) = mem + SVE_BOX; At<void*>(cd, 0x10) = nullptr;
    At<void*>(cd, 0x14) = mem + SVE_VERT; At<void*>(cd, 0x18) = mem + SVE_TRIS; At<void*>(cd, 0x1C) = mem + SVE_PLANES;
    for (int k = 0; k < 3; ++k) {   // esferas: centro y radio
        uint8* sp = mem + SVE_SPH + k * 0x14;
        for (int c = 0; c < 3; ++c) At<float>(sp, c * 4) = PickNear(s);
        static constexpr float rads[] = { 0.f, 0.5f, 1.f, 2.f, -0.5f, 3.f };
        At<float>(sp, 0xC) = (R() % 100 < 90) ? rads[R() % std::size(rads)] : PickF(s);
    }
    {   // contador y grupos de caras (justo antes de los triangulos): grupo g en tris - 0x20 - g*0x1C, contador en tris - 4
        static constexpr uint32 counts[] = { 0, 1, 2, 3, 3, 0xFFFFFFFFu, 0x80000000u };
        At<uint32>(mem, SVE_TRIS - 4) = counts[R() % std::size(counts)];
        for (int g = 0; g < 3; ++g) { uint8* gr = mem + SVE_TRIS - 0x20 - g * 0x1C; At<int16>(gr, 0x18) = int16(int(R() % 10) - 3); At<int16>(gr, 0x1A) = int16(int(R() % 10) - 3); }
    }
    { const uint32 k = R() % 10; sveBoxMask = k == 0 ? 0u : k == 1 ? 0xFFFFFFFFu : (R() & R()); }
    { const uint32 k = R() % 10; sveTriMask = k == 0 ? 0u : k == 1 ? 0xFFFFFFFFu : (R() & R()); }
    sveBoxCalls = sveTriCalls = 0; nrec = 0;
    // 16 bytes de basura de pila = la "esfera local" que el EXE lee
    uint32 seed[4];
    for (int c = 0; c < 3; ++c) seed[c] = Bits((R() % 100 < 85) ? PickNear(s) : PickF(s));
    { static constexpr float rads[] = { 0.f, 0.5f, 1.f, 2.f, -0.5f, 3.f }; seed[3] = Bits((R() % 100 < 85) ? rads[R() % std::size(rads)] : PickF(s)); }
    for (int c = 0; c < 8; ++c) At<uint32>(sveDecoy.p(), c * 4) = 0x7FC00001u + c;   // esfera 'sphere' (debe ignorarse)

    hook->State(cand);
    const bool sw = ReversingProbe::JumpsToDll(addr) == cand;
    const unsigned savedFP = _controlfp(0, 0); _controlfp(_PC_24, _MCW_PC);
    const uint32 eax = CallSve(addr, sveDecoy.p(), e, seed);
    const unsigned fp = _controlfp(0, 0); _controlfp(savedFP, _MCW_PC | _MCW_RC);
    return { {"switches", sw}, {"ret", eax & 0xFF}, {"fp", fp}, {"trace", Recs()}, {"ent", sveEnt.Bytes()}, {"mat", sveMat.Bytes()}, {"cm", sveCm.Bytes()},
             {"cd", sveCd.Bytes()}, {"mem", sveMem.Bytes()}, {"decoy", sveDecoy.Bytes()}, {"seed", std::vector<uint32>(seed, seed + 4)} };
}

// =====================================================================================================================
// CPlayerPed::SetupPlayerPed
// =====================================================================================================================
inline Buf<0x7A4> spPed;
inline void* __cdecl SpPedNew(unsigned n) { if (!Mine()) return nullptr; Push(20, { n }); return spPed.p(); }
// El EXE usa EAX (this) tras el constructor: se devuelve this. Escribe un marcador en +0x598 (tipo de ped) para ver si SetupPlayerPed lo pisa.
inline void* __fastcall SpCtor(void* self, void*, int32 id, uint32 grp) { if (Mine()) { Push(21, { P(self), uint32(id), grp & 0xFF }); At<uint32>(static_cast<uint8*>(self), 0x598) = 0x11223344u; } return self; }
inline void __fastcall SpOrient(void* self, void*, float x, float y, float z) { Push(22, { P(self), Bits(x), Bits(y), Bits(z) }); }
inline void __cdecl SpWorldAdd(void* e) { Push(23, { P(e) }); }
constexpr uintptr_t PLAYERS = 0xB7CD98, PLAYER_FOCUS = 0xB7CD74; constexpr size_t PLAYERS_SIZE = 0x320;

inline json CaseSetup(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    const int32 id = int32(I % 2); const uint8 focus = uint8((I / 2) % 2); const uint8 fill = ((I / 4) % 2) ? 0xCD : 0x5A;
    spPed.Fill(fill); nrec = 0;
    auto* players = reinterpret_cast<uint8*>(PLAYERS);
    *reinterpret_cast<uint8*>(PLAYER_FOCUS) = focus;
    At<uint32>(players, size_t(id) * 0x190) = 0x11110000u + I;                       // m_pPed previo
    At<uint8>(players, size_t(id) * 0x190 + 0xDC) = uint8(1 + I % 4);                // m_nPlayerState previo
    hook->State(cand);
    const bool sw = ReversingProbe::JumpsToDll(addr) == cand;
    const unsigned savedFP = _controlfp(0, 0); _controlfp(_PC_24, _MCW_PC);
    reinterpret_cast<void(__cdecl*)(int32)>(addr)(id);
    const unsigned fp = _controlfp(0, 0); _controlfp(savedFP, _MCW_PC | _MCW_RC);
    json out = { {"switches", sw}, {"fp", fp}, {"trace", Recs()}, {"ped", spPed.Bytes()}, {"players", ReversingProbe::Bytes(players, PLAYERS_SIZE)}, {"focus", *reinterpret_cast<uint8*>(PLAYER_FOCUS)} };
    return out;   // el estado global lo restaura quien llama (RestoreGlobals)
}

// =====================================================================================================================
// CPedGeometryAnalyser::CanPedTargetPed y ComputeClosestSurfacePoint (variante ped)
// =====================================================================================================================
inline Buf<0x7A4> gaShooter, gaTarget; inline Buf<0x50> gaMat; inline Buf<0x100> gaIntel; inline Buf<0x200> gaEntity; inline Buf<0x10> gaPoint;
inline bool gaPointResult{}; inline uint32 gaSeed{}; inline const void* gaLastCorners{}; inline const void* gaExpectPoint{}; inline const void* gaExpectPosn{}; inline bool gaRectResult{};

inline bool __cdecl GaCanTargetPoint(const void* ped, const void* v, uint32 b) {
    if (!Mine()) return false; const auto* f = static_cast<const uint32*>(v);
    Push(50, { P(ped), f[0], f[1], f[2], b & 0xFF }); return gaPointResult;
}
inline void __cdecl GaCorners(float z, void* entity, void* corners) {
    if (!Mine()) return;
    uint32 s = gaSeed; auto* c = static_cast<float*>(corners);
    for (int k = 0; k < 12; ++k) c[k] = PickF(s);
    gaLastCorners = corners; Push(51, { Bits(z), P(entity) });
}
inline bool __cdecl GaRect(const void* posn, const void* corners, void* point) {
    if (!Mine()) return false; const auto* f = static_cast<const uint32*>(posn);
    Push(52, { f[0], f[1], f[2], uint32(corners == gaLastCorners), uint32(point == gaExpectPoint), uint32(posn == gaExpectPosn), uint32(Fnv(corners, 48)) });
    uint32 s = gaSeed ^ 0x5A5A5A5Au; if (!s) s = 1;
    if (Rnd(s) % 2) { auto* p = static_cast<float*>(point); p[0] = PickF(s); p[1] = PickF(s); p[2] = PickF(s); }
    return gaRectResult;
}

inline void FillPed(Buf<0x7A4>& ped, uint32& s, bool wantMatrix, uint32 matrixBias) {
    ped.Fill(0xCD); gaMat.Fill(0xCD); gaIntel.Fill(0xCD);
    uint8* p = ped.p(); uint8* m = gaMat.p();
    for (size_t off : { 0x04u, 0x08u, 0x0Cu }) At<float>(p, off) = PickF(s);
    for (size_t off : { 0x30u, 0x34u, 0x38u }) At<float>(m, off) = PickF(s);
    At<uint32>(m, 0x40) = 0; At<uint8>(m, 0x44) = 0;
    At<void*>(p, 0x14) = wantMatrix ? static_cast<void*>(m) : nullptr;
    At<void*>(p, 0x47C) = gaIntel.p();
    (void)matrixBias;
}

inline json CaseCanTarget(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x5F1C);
    FillPed(gaTarget, s, Rnd(s) % 2, 0);
    gaShooter.Fill(0xA5);
    for (size_t off = 0x14; off < 0x40; off += 4) At<uint32>(gaIntel.p(), off) = Rnd(s) % 2 ? 0 : 0x20000000u + uint32(off);   // tareas secundarias (+4 + 0x14 + idx*4)
    At<uint32>(gaIntel.p(), 0x1C) = (Rnd(s) % 2) ? 0x12345678u : 0u;                                                          // TASK_SECONDARY_DUCK (1)
    gaPointResult = Rnd(s) % 2; const bool dir = Rnd(s) % 2; nrec = 0;
    hook->State(cand);
    const bool sw = ReversingProbe::JumpsToDll(addr) == cand;
    const unsigned savedFP = _controlfp(0, 0); _controlfp(_PC_24, _MCW_PC);
    const bool r = reinterpret_cast<bool(__cdecl*)(void*, void*, bool)>(addr)(gaShooter.p(), gaTarget.p(), dir);
    const unsigned fp = _controlfp(0, 0); _controlfp(savedFP, _MCW_PC | _MCW_RC);
    return { {"switches", sw}, {"ret", r}, {"fp", fp}, {"trace", Recs()}, {"shooter", gaShooter.Bytes()}, {"target", gaTarget.Bytes()}, {"mat", gaMat.Bytes()}, {"intel", gaIntel.Bytes()}, {"dir", dir} };
}

inline json CaseSurface(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x5F3B);
    FillPed(gaShooter, s, Rnd(s) % 2, 0);
    gaEntity.Fill(0xB4); gaPoint.Fill(0xCD);
    gaSeed = Seed(I, 0x77); gaRectResult = Rnd(s) % 2; gaLastCorners = nullptr; nrec = 0;
    uint8* p = gaShooter.p();
    gaExpectPosn = At<void*>(p, 0x14) ? static_cast<const void*>(gaMat.p() + 0x30) : static_cast<const void*>(p + 4);
    gaExpectPoint = gaPoint.p();
    hook->State(cand);
    const bool sw = ReversingProbe::JumpsToDll(addr) == cand;
    const unsigned savedFP = _controlfp(0, 0); _controlfp(_PC_24, _MCW_PC);
    const bool r = reinterpret_cast<bool(__cdecl*)(const void*, void*, void*)>(addr)(p, gaEntity.p(), gaPoint.p());
    const unsigned fp = _controlfp(0, 0); _controlfp(savedFP, _MCW_PC | _MCW_RC);
    return { {"switches", sw}, {"ret", r}, {"fp", fp}, {"trace", Recs()}, {"ped", gaShooter.Bytes()}, {"mat", gaMat.Bytes()}, {"entity", gaEntity.Bytes()}, {"point", gaPoint.Bytes()} };
}

// =====================================================================================================================
// CRealTimeShadowManager::{Init, Exit, Update, DoShadowThisFrame}
// =====================================================================================================================
inline Buf<0x54> rtMgr; inline std::array<Buf<0x4C>, 16> rtSh, rtArena; inline std::array<Buf<0x140>, 16> rtOw; inline Buf<0x5C0> rtPhys;
inline uint32 rtArenaNext{}; inline int32 rtQuality{}; inline bool rtGetShadowNull{}; inline uint32 rtGetShadowIdx{};

inline void* __cdecl RtNew(size_t n) { Push(1, { uint32(n) }); return rtArenaNext < rtArena.size() ? rtArena[rtArenaNext++].p() : nullptr; }
inline void __cdecl RtDelete(void* p) { Push(2, { P(p) }); }
inline void __fastcall RtReInit(void* self, void*) { Push(60, { P(self) }); }
inline void* __fastcall RtShadowUpdate(void* self, void*) { Push(61, { P(self) }); return nullptr; }
inline void __cdecl RtStore(void* owner_, float a, float b, float c, float d, float e, float f) { Push(62, { P(owner_), Bits(a), Bits(b), Bits(c), Bits(d), Bits(e), Bits(f) }); }
inline int32 __fastcall RtFxQuality(void* self, void*) { if (!Mine()) return *reinterpret_cast<int32*>(uintptr_t(self) + 0x54); Push(70, { P(self) }); return rtQuality; }
inline void* __fastcall RtGetShadow(void* self, void*, void* phys) { Push(71, { P(self), P(phys) }); return rtGetShadowNull ? nullptr : rtSh[rtGetShadowIdx % 16].p(); }
inline void __fastcall RtShadowDestroy(void* self, void*) { Push(80, { P(self) }); }
inline void* __fastcall RtCamDestroy(void* self, void*) { Push(81, { P(self) }); return nullptr; }
inline void* __fastcall RtCamCreate(void* self, void*, int32 size) { Push(82, { P(self), uint32(size) }); return reinterpret_cast<void*>(1); }
inline void* __fastcall RtCamGradient(void* self, void*) { Push(83, { P(self) }); return nullptr; }
inline bool __fastcall RtShadowCreate(void* self, void*, uint32 a, int32 b, uint32 c) { Push(84, { P(self), a & 0xFF, uint32(b), c & 0xFF }); return true; }

constexpr uintptr_t TC_BEGIN = 0xB79E50, TC_SIZE = 0x184, TC_INDEX = 0xB79FD0;

inline void RtPrepareShadows(uint32& s, bool owners, bool alwaysShadow) {
    rtMgr.Fill(0xCD);
    for (auto& b : rtSh) b.Fill(0xCD);
    for (auto& b : rtOw) b.Fill(0xCD);
    for (int k = 0; k < 16; ++k) {
        uint8* sh = rtSh[k].p();
        (void)alwaysShadow;
        At<void*>(rtMgr.p(), 4 + k * 4) = sh;
        At<void*>(sh, 0) = (owners && Rnd(s) % 100 < 65) ? static_cast<void*>(rtOw[Rnd(s) % 16].p()) : nullptr;
        At<uint8>(sh, 4) = uint8(Rnd(s) % 2);
        static constexpr uint8 inten[] = { 0, 1, 2, 3, 4, 50, 97, 98, 99, 100, 101, 127, 128, 129, 200, 253, 255 };
        At<uint8>(sh, 5) = inten[Rnd(s) % std::size(inten)];
    }
    for (int k = 0; k < 16; ++k) {
        uint8* ow = rtOw[k].p();
        At<void*>(ow, 0x134) = (Rnd(s) % 100 < 70) ? static_cast<void*>(rtSh[Rnd(s) % 16].p()) : nullptr;
    }
}

inline json RtResult(bool sw, unsigned fp) {
    json out = { {"switches", sw}, {"fp", fp}, {"trace", Recs()}, {"mgr", rtMgr.Bytes()} };
    json sh = json::array(), ow = json::array();
    for (auto& b : rtSh) sh.push_back(b.Bytes());
    for (auto& b : rtOw) ow.push_back(b.Bytes());
    out["shadows"] = sh; out["owners"] = ow;
    return out;
}

inline json CaseRtUpdate(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x706A);
    RtPrepareShadows(s, true, false);
    static constexpr uint8 flags[] = { 0, 1, 1, 1, 7 };
    At<uint8>(rtMgr.p(), 0) = flags[Rnd(s) % std::size(flags)];
    At<uint8>(rtMgr.p(), 1) = uint8((Rnd(s) % 3 == 0) ? 3 : Rnd(s) % 2);
    At<uint32>(rtMgr.p(), 0x44) = 0xC0DE0044; At<uint32>(rtMgr.p(), 0x4C) = 0xC0DE004C;
    for (uintptr_t a = TC_BEGIN; a < TC_INDEX; a += 4) *reinterpret_cast<float*>(a) = PickF(s);
    *reinterpret_cast<uint32*>(TC_INDEX) = Rnd(s) % 16;
    nrec = 0;
    hook->State(cand);
    const bool sw = ReversingProbe::JumpsToDll(addr) == cand;
    const unsigned savedFP = _controlfp(0, 0); _controlfp(_PC_24, _MCW_PC);
    reinterpret_cast<void(__thiscall*)(void*)>(addr)(rtMgr.p());
    const unsigned fp = _controlfp(0, 0); _controlfp(savedFP, _MCW_PC | _MCW_RC);
    return RtResult(sw, fp);
}

inline json CaseRtDo(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x706B);
    RtPrepareShadows(s, false, true);
    rtPhys.Fill(0xCD);
    uint8* ph = rtPhys.p();
    const uint32 tl = Rnd(s) % 100 < 55 ? 3u : Rnd(s) % 8;
    At<uint8>(ph, 0x36) = uint8((Rnd(s) & 0xF8) | tl);
    static constexpr uint32 pedTypes[] = { 0, 0, 1, 2, 5, 0xFFFFFFFFu };
    At<uint32>(ph, 0x598) = pedTypes[Rnd(s) % std::size(pedTypes)];
    At<void*>(ph, 0x134) = (Rnd(s) % 2) ? static_cast<void*>(rtSh[Rnd(s) % 16].p()) : nullptr;
    At<uint8>(rtMgr.p(), 0) = uint8(Rnd(s) % 2); At<uint8>(rtMgr.p(), 1) = uint8(Rnd(s) % 2);
    static constexpr int32 quals[] = { 3, 2, 2, 1, 0, 4, -1, 3, 2 };
    rtQuality = quals[Rnd(s) % std::size(quals)];
    rtGetShadowNull = Rnd(s) % 3 == 0; rtGetShadowIdx = Rnd(s);
    nrec = 0;
    hook->State(cand);
    const bool sw = ReversingProbe::JumpsToDll(addr) == cand;
    const unsigned savedFP = _controlfp(0, 0); _controlfp(_PC_24, _MCW_PC);
    reinterpret_cast<void(__thiscall*)(void*, void*)>(addr)(rtMgr.p(), ph);
    const unsigned fp = _controlfp(0, 0); _controlfp(savedFP, _MCW_PC | _MCW_RC);
    json out = RtResult(sw, fp); out["phys"] = rtPhys.Bytes(); return out;
}

inline json CaseRtExit(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x706C);
    RtPrepareShadows(s, true, false);
    for (int k = 0; k < 16; ++k) if (Rnd(s) % 100 < 25) At<void*>(rtMgr.p(), 4 + k * 4) = nullptr;
    static constexpr uint8 flags[] = { 0, 1, 1, 9 };
    At<uint8>(rtMgr.p(), 0) = flags[Rnd(s) % std::size(flags)]; At<uint8>(rtMgr.p(), 1) = uint8(Rnd(s) % 2);
    At<uint32>(rtMgr.p(), 0x44) = 0xC0DE0044; At<uint32>(rtMgr.p(), 0x4C) = 0xC0DE004C;
    nrec = 0;
    hook->State(cand);
    const bool sw = ReversingProbe::JumpsToDll(addr) == cand;
    const unsigned savedFP = _controlfp(0, 0); _controlfp(_PC_24, _MCW_PC);
    {
        using ReversingProbe::EntryPatch;
        EntryPatch n1(0x82119A, RtNew), d1(0x8214BD, RtDelete);   // solo durante la llamada
        reinterpret_cast<void(__thiscall*)(void*)>(addr)(rtMgr.p());
    }
    const unsigned fp = _controlfp(0, 0); _controlfp(savedFP, _MCW_PC | _MCW_RC);
    return RtResult(sw, fp);
}

inline json CaseRtInit(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x706D);
    RtPrepareShadows(s, true, false);
    for (auto& b : rtArena) b.Fill(0xCD);
    rtArenaNext = 0;
    static constexpr uint8 flags[] = { 0, 0, 0, 1, 4 };
    At<uint8>(rtMgr.p(), 0) = flags[Rnd(s) % std::size(flags)]; At<uint8>(rtMgr.p(), 1) = uint8(Rnd(s) % 2);
    nrec = 0;
    hook->State(cand);
    const bool sw = ReversingProbe::JumpsToDll(addr) == cand;
    const unsigned savedFP = _controlfp(0, 0); _controlfp(_PC_24, _MCW_PC);
    {
        using ReversingProbe::EntryPatch;
        EntryPatch n1(0x82119A, RtNew), d1(0x8214BD, RtDelete);
        reinterpret_cast<void(__thiscall*)(void*)>(addr)(rtMgr.p());
    }
    const unsigned fp = _controlfp(0, 0); _controlfp(savedFP, _MCW_PC | _MCW_RC);
    json out = RtResult(sw, fp); json ar = json::array(); for (auto& b : rtArena) ar.push_back(b.Bytes()); out["arena"] = ar; out["next"] = rtArenaNext; return out;
}

// =====================================================================================================================
inline Hook* FindHook(const char* cls, const char* fn) {
    Hook* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if (c.Name() == cls) for (auto& h : c.Items()) if (h->Name() == fn) r = h.get(); });
    return r;
}
// Sigue la cadena de saltos E9 (thunk incremental -> cuerpo).
inline uintptr_t Body(uintptr_t a) { for (int j = 0; j < 16 && *reinterpret_cast<uint8*>(a) == 0xE9; ++j) a += 5 + *reinterpret_cast<int32*>(a + 1); return a; }
// Constructor de la DLL (no se puede tomar la direccion de un constructor): primera llamada a la DLL que sigue a la del
// operator new correspondiente dentro de la funcion (copiado de Misc8).
inline uintptr_t FindCtorCall(uintptr_t fn, uintptr_t newFn) {
    HMODULE mod{}; if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(fn), &mod)) return 0;
    const auto modBase = uintptr_t(mod); const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase + reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew);
    const uintptr_t modEnd = modBase + nt->OptionalHeader.SizeOfImage;
    auto FollowIn = [&](uintptr_t q) { for (int k = 0; k < 4 && q >= modBase && q < modEnd && *reinterpret_cast<const uint8*>(q) == 0xE9; ++k) q = q + 5 + *reinterpret_cast<const int32*>(q + 1); return q; };
    fn = FollowIn(fn); const auto nf = FollowIn(newFn); bool seen = false;
    for (uintptr_t p = fn; p < fn + 0x1200; ++p) {
        if (*reinterpret_cast<const uint8*>(p) != 0xE8) continue;
        const uintptr_t t = p + 5 + *reinterpret_cast<const int32*>(p + 1);
        if (!seen) { if (t >= modBase && t < modEnd && FollowIn(t) == nf) { seen = true; p += 4; } continue; }
        if (t < modBase || t >= modEnd) continue;
        if (*reinterpret_cast<const uint8*>(t) != 0xE9 && (t & 15)) continue;
        return t;
    }
    return 0;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn, 8> fns{{
    {"CCollision","SphereVsEntity",0x41A5A0,0,-1,1024},
    {"CPlayerPed","SetupPlayerPed",0x60D790,0,-1,128},
    {"CPedGeometryAnalyser","CanPedTargetPed",0x5F1C40,0,-1,1024},
    {"CPedGeometryAnalyser","ComputeClosestSurfacePoint-ped",0x5F3B70,0,-1,512},
    {"CRealTimeShadowManager","DoShadowThisFrame",0x706BA0,0,-1,512},
    {"CRealTimeShadowManager","Exit",0x706A60,0,-1,256},
    {"CRealTimeShadowManager","Init",0x7067C0,0,-1,128},
    {"CRealTimeShadowManager","Update",0x706AB0,0,-1,1024},
}};

inline json RunCase(size_t f, unsigned i, bool cand, Hook* hook) {
    const auto a = fns[f].address;
    switch (f) {
    case 0: return CaseSve(i, cand, hook, a);
    case 1: return CaseSetup(i, cand, hook, a);
    case 2: return CaseCanTarget(i, cand, hook, a);
    case 3: return CaseSurface(i, cand, hook, a);
    case 4: return CaseRtDo(i, cand, hook, a);
    case 5: return CaseRtExit(i, cand, hook, a);
    case 6: return CaseRtInit(i, cand, hook, a);
    default: return CaseRtUpdate(i, cand, hook, a);
    }
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "geofinal1-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "geofinal1-runtime.jsonl"); Write({ {"event", "waiting_for_player"} }); ReversingProbe::InstallFailureCapture("geofinal1"); }
    auto* player = FindPlayerPed(); if (!player || CCutsceneMgr::IsRunning()) return; done = true; owner = GetCurrentThreadId();
    std::string only; { std::ifstream f(dir / "geofinal1-only.txt"); if (f) std::getline(f, only); }
    std::array<Hook*, 8> hooks{};
    for (size_t f = 0; f < fns.size(); ++f) if (!(hooks[f] = FindHook(fns[f].cls, fns[f].name))) { Write({ {"event", "error"}, {"reason", "hook not found"}, {"function", std::string(fns[f].cls) + "::" + fns[f].name} }); return; }
    std::array<bool, 8> enabled{}, locked{};
    for (size_t f = 0; f < fns.size(); ++f) { enabled[f] = hooks[f]->Hooked(); locked[f] = hooks[f]->Locked(); hooks[f]->LockState(false); }

    using std::bit_cast;
    // direcciones de la DLL de los colaboradores simulados
    struct Site { const char* name; uintptr_t exeAddr, dll; };
    const uintptr_t dllPedNew = bit_cast<uintptr_t>(static_cast<void*(*)(unsigned)>(&CPed::operator new));
    const uintptr_t dllSetup = bit_cast<uintptr_t>(&CPlayerPed::SetupPlayerPed);
    const uintptr_t dllPlayerCtor = FindCtorCall(dllSetup, dllPedNew);
    if (!dllPlayerCtor) { Write({ {"event", "error"}, {"reason", "constructor call not found"} }); return; }
    // (cada llamada se parchea en el EXE, en el simbolo de la DLL y en su cuerpo)
    struct Stub { const char* name; uintptr_t exeAddr, dll, stub; };
    const std::vector<Stub> stubs = {
        {"CCamera::IsExtraEntityToIgnore", 0x50CE80, bit_cast<uintptr_t>(&CCamera::IsExtraEntityToIgnore), bit_cast<uintptr_t>(&SveIgnore)},
        {"CEntity::GetColModel", 0x535300, bit_cast<uintptr_t>(&CEntity::GetColModel), bit_cast<uintptr_t>(&SveColModel)},
        {"CPlaceable::AllocateMatrix", 0x54F560, bit_cast<uintptr_t>(&CPlaceable::AllocateMatrix), bit_cast<uintptr_t>(&SveAlloc)},
        {"CSimpleTransform::UpdateMatrix", 0x54F1B0, bit_cast<uintptr_t>(&CSimpleTransform::UpdateMatrix), bit_cast<uintptr_t>(&SveUpdateMatrix)},
        {"CCollision::CalculateTrianglePlanes", 0x416330, bit_cast<uintptr_t>(static_cast<void(*)(CCollisionData*)>(&CCollision::CalculateTrianglePlanes)), bit_cast<uintptr_t>(&SveTrianglePlanes)},
        {"CCollision::TestSphereBox", 0x4120C0, bit_cast<uintptr_t>(&CCollision::TestSphereBox), bit_cast<uintptr_t>(&SveTestBox)},
        {"CCollision::TestSphereTriangle", 0x4165B0, bit_cast<uintptr_t>(&CCollision::TestSphereTriangle), bit_cast<uintptr_t>(&SveTestTri)},
        {"CPed::operator new", 0x5E4720, dllPedNew, bit_cast<uintptr_t>(&SpPedNew)},
        {"CPlayerPed::CPlayerPed", 0x60D5B0, dllPlayerCtor, bit_cast<uintptr_t>(&SpCtor)},
        {"CPlaceable::SetOrientation", 0x439A80, bit_cast<uintptr_t>(static_cast<void(CPlaceable::*)(float, float, float)>(&CPlaceable::SetOrientation)), bit_cast<uintptr_t>(&SpOrient)},
        {"CWorld::Add", 0x563220, bit_cast<uintptr_t>(&CWorld::Add), bit_cast<uintptr_t>(&SpWorldAdd)},
        {"CPedGeometryAnalyser::CanPedTargetPoint", 0x5F1B70, bit_cast<uintptr_t>(&CPedGeometryAnalyser::CanPedTargetPoint), bit_cast<uintptr_t>(&GaCanTargetPoint)},
        {"CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached", 0x5F1FA0, bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached), bit_cast<uintptr_t>(&GaCorners)},
        {"CPedGeometryAnalyser::ComputeClosestSurfacePoint-rect", 0x5F2C10, bit_cast<uintptr_t>(static_cast<bool(*)(const CVector&, const CVector*, CVector&)>(&CPedGeometryAnalyser::ComputeClosestSurfacePoint)), bit_cast<uintptr_t>(&GaRect)},
        {"CRealTimeShadowManager::ReInit", 0x706870, bit_cast<uintptr_t>(&CRealTimeShadowManager::ReInit), bit_cast<uintptr_t>(&RtReInit)},
        {"CRealTimeShadow::Update", 0x706600, bit_cast<uintptr_t>(&CRealTimeShadow::Update), bit_cast<uintptr_t>(&RtShadowUpdate)},
        {"CShadows::StoreRealTimeShadow", 0x707CA0, bit_cast<uintptr_t>(&CShadows::StoreRealTimeShadow), bit_cast<uintptr_t>(&RtStore)},
        {"Fx_c::GetFxQuality", 0x49EA50, bit_cast<uintptr_t>(&Fx_c::GetFxQuality), bit_cast<uintptr_t>(&RtFxQuality)},
        {"CRealTimeShadowManager::GetRealTimeShadow", 0x706970, bit_cast<uintptr_t>(&CRealTimeShadowManager::GetRealTimeShadow), bit_cast<uintptr_t>(&RtGetShadow)},
        {"CRealTimeShadow::Destroy", 0x705990, bit_cast<uintptr_t>(&CRealTimeShadow::Destroy), bit_cast<uintptr_t>(&RtShadowDestroy)},
        {"CShadowCamera::Destroy", 0x705400, bit_cast<uintptr_t>(&CShadowCamera::Destroy), bit_cast<uintptr_t>(&RtCamDestroy)},
        {"CShadowCamera::Create", 0x705B60, bit_cast<uintptr_t>(&CShadowCamera::Create), bit_cast<uintptr_t>(&RtCamCreate)},
        {"CShadowCamera::MakeGradientRaster", 0x705D20, bit_cast<uintptr_t>(&CShadowCamera::MakeGradientRaster), bit_cast<uintptr_t>(&RtCamGradient)},
        {"CRealTimeShadow::Create", 0x706460, bit_cast<uintptr_t>(&CRealTimeShadow::Create), bit_cast<uintptr_t>(&RtShadowCreate)},
    };
    bool known = true; json siteInfo = json::array();
    for (auto& s : stubs) {
        const bool ok = s.exeAddr >= 0x401000 && s.exeAddr < 0x1000000 && s.dll >= 0x1000000 && Body(s.dll) >= 0x1000000; known &= ok;
        siteInfo.push_back({ {"name", s.name}, {"exe", s.exeAddr}, {"dll", s.dll}, {"body", Body(s.dll)}, {"ok", ok} });
    }
    // bytes iniciales de todas las entradas parcheables y de las funciones probadas, para comprobar la restauracion
    std::vector<std::pair<uintptr_t, std::vector<uint8>>> entryBefore;
    for (auto& s : stubs) for (auto a : { s.exeAddr, s.dll, Body(s.dll) }) entryBefore.push_back({ a, ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8) });
    for (auto a : { uintptr_t(0x82119A), uintptr_t(0x8214BD) }) entryBefore.push_back({ a, ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8) });
    for (auto& fn : fns) entryBefore.push_back({ fn.address, ReversingProbe::Bytes(reinterpret_cast<void*>(fn.address), 8) });
    // estado global tocado: Players (m_pPed y estado), PlayerInFocus y los arrays de sombras del CTimeCycle
    const std::array<std::pair<uintptr_t, size_t>, 3> globals{{ {PLAYERS, PLAYERS_SIZE}, {PLAYER_FOCUS, 1}, {TC_BEGIN, TC_SIZE} }};
    std::vector<std::vector<uint8>> saved; for (auto [p, n] : globals) saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(p), n));
    auto RestoreGlobals = [&]() { for (size_t j = 0; j < globals.size(); ++j) std::memcpy(reinterpret_cast<void*>(globals[j].first), saved[j].data(), saved[j].size()); };
    const unsigned savedFP = _controlfp(0, 0);
    Write({ {"event", "begin"}, {"sites", siteInfo}, {"known", known}, {"fp_control", savedFP}, {"player_ctor", dllPlayerCtor} });

    bool passed = known; int pair = 0;
    if (known) {
        using ReversingProbe::EntryPatch;
        std::vector<std::unique_ptr<EntryPatch>> patches;
        // El cuerpo se calcula ANTES de parchear el simbolo: tras parchear, el E9 del simbolo apunta al stub y Body() devolveria
        // el propio stub, que quedaria parcheado con un salto a si mismo (bucle infinito en la primera llamada al stub).
        std::vector<uintptr_t> bodies; for (auto& s : stubs) bodies.push_back(Body(s.dll));
        for (size_t k = 0; k < stubs.size(); ++k) {
            const auto& s = stubs[k];
            patches.push_back(std::make_unique<EntryPatch>(s.exeAddr, reinterpret_cast<void*>(s.stub)));
            patches.push_back(std::make_unique<EntryPatch>(s.dll, reinterpret_cast<void*>(s.stub)));
            if (bodies[k] != s.dll) patches.push_back(std::make_unique<EntryPatch>(bodies[k], reinterpret_cast<void*>(s.stub)));
        }
        for (size_t f = 0; f < fns.size(); ++f) {
            const auto& fn = fns[f]; int failures = 0; const std::string full = std::string(fn.cls) + "::" + fn.name;
            if (!only.empty() && only != full) continue;
            for (int i = 0; i < fn.cases; ++i) {
                json original = RunCase(f, unsigned(i), false, hooks[f]); RestoreGlobals();
                json candidate = RunCase(f, unsigned(i), true, hooks[f]); RestoreGlobals();
                const bool ok = original == candidate && original["switches"] == true && candidate["switches"] == true;
                const std::string od = original.dump(), cd = candidate.dump();
                passed &= ok; failures += !ok;
                if (ok) Write({ {"event", "comparison"}, {"pair", pair++}, {"function", full}, {"case", i}, {"equal", true}, {"accepted", true},
                                {"original", {{"trace", original["trace"]}, {"ret", original.value("ret", json(nullptr))}, {"state_fnv", Fnv(od.data(), od.size())}}},
                                {"candidate", {{"trace", candidate["trace"]}, {"ret", candidate.value("ret", json(nullptr))}, {"state_fnv", Fnv(cd.data(), cd.size())}}},
                                {"events", original["trace"].size()} });
                else { json diff = json::array(); for (auto& [k, v] : original.items()) if (candidate[k] != v) diff.push_back(k);
                    Write({ {"event", "comparison"}, {"pair", pair++}, {"function", full}, {"case", i}, {"equal", false}, {"accepted", false}, {"differs", diff},
                            {"original", failures <= 2 ? original : json(nullptr)}, {"candidate", failures <= 2 ? candidate : json(nullptr)} }); }
            }
            Write({ {"event", "function_summary"}, {"function", full}, {"failures", failures}, {"cases", fn.cases} });
        }
        patches.clear();   // restaura las entradas
    }
    for (size_t f = 0; f < fns.size(); ++f) { hooks[f]->State(enabled[f]); hooks[f]->LockState(locked[f]); }
    RestoreGlobals();
    _controlfp(savedFP, _MCW_PC | _MCW_RC);
    bool restored = _controlfp(0, 0) == savedFP;
    for (size_t j = 0; j < globals.size(); ++j) restored &= saved[j] == ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].first), globals[j].second);
    for (auto& [a, b] : entryBefore) restored &= b == ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8);
    Write({ {"event", passed ? "complete" : "error"}, {"pairs", pair}, {"state_restored", restored} });
}
}

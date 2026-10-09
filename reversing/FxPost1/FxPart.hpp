#pragma once
// FxPost1 - parte Fx: FxSystem_c (31 funciones pendientes de docs/hooks.csv con validated=0).
// Original (todos los hooks de FxSystem_c desactivados: EXE puro) frente a candidato (todos activos: DLL) sobre OBJETOS FALSOS
// (buffers con guardas de 16 bytes: sistema, plantilla, primitivas con vtables trazadas, particulas, matrices, esferas y una
// arena fija para CMemoryMgr::Malloc). Los colaboradores del asignador, del RNG, de CAEFireAudioEntity (AddAudioEvent/Terminate),
// de RwMatrixCreate/Destroy y de los accesores de huesos (GetAnimHierarchyFromSkinClump, RpHAnimIDGetIndex,
// RpHAnimHierarchyGetMatrixArray) se doblan con EntryPatch (EXE + simbolo de la DLL) y trazan sus argumentos.
// Se usan reales: RwMatrixMultiply/RwV3dTransformPoints/RwMatrixUpdate (envoltorios del EXE), la pila de matrices de g_fxMan
// (con sus matrices y el contador guardados/restaurados), FxFrustumInfo_c::IsCollision (llama al EXE) y CKeyGen.
// Trazas con etiquetas 100..199.
#include "FxPostCommon.hpp"
#include "FxSystem.h"
#include "FxSystemBP.h"
#include "FxPrim.h"
#include "FxPrimBP.h"
#include "FxManager.h"
#include "FxSphere.h"
#include "Particle.h"
#include "AEFireAudioEntity.h"
#include "KeyGen.h"
#include "RwHelper.h"

namespace FxPost1Live { namespace Fx {

// ---------------------------------------------------------------------------------------------------------------------
// Tabla
// ---------------------------------------------------------------------------------------------------------------------
inline const std::array<Fn, 31> fns{{
    {"FxSystem_c","AddOffsetPos",0x4AA690,0,-1,256},
    {"FxSystem_c","AttachToBone",0x4AA400,0,-1,256},
    {"FxSystem_c","Constructor",0x4AAF00,0,-1,128},
    {"FxSystem_c","CopyParentMatrix",0x4AA890,0,-1,128},
    {"FxSystem_c","Destructor",0x4AA260,0,-1,256},
    {"FxSystem_c","DoFxAudio",0x4AAC90,0,-1,512},
    {"FxSystem_c","EnablePrim",0x4AA610,0,-1,128},
    {"FxSystem_c","Exit",0x4AA840,0,-1,256},
    {"FxSystem_c","ForAllParticles",0x4AA930,0,-1,512},
    {"FxSystem_c","GetBoundingSphereLcl",0x4AAB50,0,-1,128},
    {"FxSystem_c","GetBoundingSphereWld",0x4AAAD0,0,-1,512},
    {"FxSystem_c","GetCompositeMatrix",0x4AA8C0,0,-1,256},
    {"FxSystem_c","GetPlayStatus",0x4AA900,0,-1,64},
    {"FxSystem_c","Init",0x4AA750,0,-1,512},
    {"FxSystem_c","IsVisible",0x4AAF30,0,-1,512},
    {"FxSystem_c","Kill",0x4AA3F0,0,-1,256},
    {"FxSystem_c","Pause",0x4AA370,0,-1,128},
    {"FxSystem_c","Play",0x4AA2F0,0,-1,512},
    {"FxSystem_c","PlayAndKill",0x4AA3D0,0,-1,256},
    {"FxSystem_c","ResetBoundingSphere",0x4AABF0,0,-1,256},
    {"FxSystem_c","SetBoundingSphere",0x4AAB80,0,-1,256},
    {"FxSystem_c","SetConstTime",0x4AA6C0,0,-1,256},
    {"FxSystem_c","SetLocalParticles",0x4AA910,0,-1,128},
    {"FxSystem_c","SetMatrix",0x4AA630,0,-1,128},
    {"FxSystem_c","SetMustCreatePrts",0x4AAC70,0,-1,128},
    {"FxSystem_c","SetOffsetPos",0x4AA660,0,-1,256},
    {"FxSystem_c","SetRateMult",0x4AA6F0,0,-1,256},
    {"FxSystem_c","SetTimeMult",0x4AA710,0,-1,256},
    {"FxSystem_c","SetVelAdd",0x4AA730,0,-1,128},
    {"FxSystem_c","SetZTestEnable",0x4AAC50,0,-1,128},
    {"FxSystem_c","Stop",0x4AA390,0,-1,256},
}};

// ---------------------------------------------------------------------------------------------------------------------
// Mundo falso
// ---------------------------------------------------------------------------------------------------------------------
constexpr size_t kSysSize = 0x104;
struct World {
    Buf<kSysSize> sys;
    Buf<0x24> bp;
    std::array<Buf<0x40>, 4> bpPrim;
    std::array<Buf<0x10>, 4> inst;
    std::array<Buf<0x2C>, 8> part;
    std::array<Buf<0x40>, 8> mat;          // 0..3 del fixture; 4..7 las entrega RwMatrixCreate
    std::array<Buf<0x14>, 4> sph;          // 0..1 del fixture, 2 de la plantilla, 3 destino/dato
    std::array<Buf<0x80>, 4> arena;        // bloques de CMemoryMgr::Malloc
    Buf<16> primTable;                     // m_Prims (4 punteros) cuando no se reserva en el monton
    Buf<16> bpPrimTable;                   // plantilla->m_Prims (4 punteros)
    Buf<0x40> entity;                      // CEntity falsa (solo se lee m_pRwObject, +0x18)
    Buf<0x10> clump, hier;                 // fichas opacas de RpClump / RpHAnimHierarchy
    Buf<0x100> bones;                      // base de la matriz de huesos devuelta por el doble
    Buf<0x40> out;                         // destino de GetBoundingSphere* / GetCompositeMatrix (0x40: rep movs de 16 dwords y RwMatrixMultiply, 0x4AA8D0 / 0x7F18B0)
    size_t nArena{}, nMat{};
    bool badCall{};
    uint32 rng{};
};
inline World W;
inline uint8* SysP() { return W.sys.p(); }

// Identificador estable de un puntero del fixture (para las trazas).
inline uint32 Id(const void* p) {
    const auto a = uintptr_t(p);
    if (!a) return 0;
    auto in = [&](const uint8* b, size_t n) { return a >= uintptr_t(b) && a < uintptr_t(b) + n; };
    if (in(W.sys.p(), kSysSize)) return 0x100 + uint32(a - uintptr_t(W.sys.p()));
    if (in(W.bp.p(), 0x24)) return 2;
    for (size_t k = 0; k < 4; ++k) if (in(W.bpPrim[k].p(), 0x40)) return uint32(10 + k);
    for (size_t k = 0; k < 4; ++k) if (in(W.inst[k].p(), 0x10)) return uint32(20 + k);
    for (size_t k = 0; k < 8; ++k) if (in(W.part[k].p(), 0x2C)) return uint32(30 + k);
    for (size_t k = 0; k < 8; ++k) if (in(W.mat[k].p(), 0x40)) return uint32(40 + k);
    for (size_t k = 0; k < 4; ++k) if (in(W.sph[k].p(), 0x14)) return uint32(50 + k);
    for (size_t k = 0; k < 4; ++k) if (in(W.arena[k].p(), 0x80)) return uint32(60 + k);
    if (in(W.entity.p(), 0x40)) return 70;
    if (in(W.clump.p(), 0x10)) return 71;
    if (in(W.hier.p(), 0x10)) return 72;
    if (in(W.bones.p(), 0x100)) return 73;
    if (in(W.out.p(), 0x40)) return 74;
    return 1000000 + uint32(a & 0xFFFF);
}

// ---------------------------------------------------------------------------------------------------------------------
// Dobles (respetan el ret N del original, regla 18)
// ---------------------------------------------------------------------------------------------------------------------
// Vtable de FxPrim_c: 0 destructor con borrado (ret 4), 1 Init(proto, sistema) (ret 8), 2 Update, 3 Reset (ret), 4/5 AddParticle (no se usan).
inline void __fastcall PrimDtor(void* self, void*, uint32 flag) { Push(100, { Id(self), flag }); }
inline bool __fastcall PrimInit(void* self, void*, void* proto, void* sys) {
    Push(101, { Id(self), Id(proto), Id(sys) });
    if (Mine()) { At<void*>(static_cast<uint8*>(self), 4) = proto; At<void*>(static_cast<uint8*>(self), 8) = sys; }
    return true;
}
inline void __fastcall PrimBad(void* self, void*) { if (Mine()) { W.badCall = true; Push(199, { Id(self) }); } }
inline void __fastcall PrimReset(void* self, void*) { Push(103, { Id(self) }); }
inline void* primVtbl[6]{};
// Vtable de FxPrimBP_c: 3 = CreateInstance() (sin argumentos).
inline void* __fastcall BpCreate(void* self, void*) {
    Push(110, { Id(self) });
    for (size_t k = 0; k < 4; ++k) if (self == W.bpPrim[k].p()) return W.inst[k].p();
    return nullptr;
}
inline void* bpVtbl[7]{};

inline void* __cdecl FakeMalloc(uint32 size, uint32) {
    if (!Mine()) return reinterpret_cast<void* (__cdecl*)(size_t)>(0x824257)(size);   // CRT malloc del EXE (0x72F420 lo envuelve)
    Push(120, { size });
    if (W.nArena >= W.arena.size()) { W.badCall = true; return nullptr; }
    return W.arena[W.nArena++].p();
}
inline void __cdecl FakeFree(void* p) {
    if (!Mine()) { reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; }     // CRT free del EXE (0x72F430 salta a el)
    Push(121, { Id(p) });
}
inline int __cdecl FakeRand() {
    if (!Mine()) return 0;
    W.rng = W.rng * 1103515245u + 12345u;
    const int v = int((W.rng >> 16) & 0x7FFF);
    Push(130, { uint32(v) });
    return v;
}
inline uint16 __cdecl FakeRandomNumber() { return uint16(FakeRand()); }
inline void __fastcall FakeAddAudio(void* self, void*, uint32 event, CVector* pos) {
    Push(140, { Id(self), event, Bits(pos->x), Bits(pos->y), Bits(pos->z) });
}
inline void __fastcall FakeTerminate(void* self, void*) { Push(141, { Id(self) }); }
inline RwMatrix* __cdecl FakeMatCreate() {
    Push(150, {});
    if (!Mine() || W.nMat >= 4) { W.badCall = true; return nullptr; }
    return reinterpret_cast<RwMatrix*>(W.mat[4 + W.nMat++].p());
}
inline RwBool __cdecl FakeMatDestroy(RwMatrix* m) { Push(151, { Id(m) }); return TRUE; }
inline void* __cdecl FakeSkinHier(void* clump) { Push(160, { Id(clump) }); return W.hier.p(); }
inline int32 __cdecl FakeBoneIndex(void* hier, int32 id) { Push(161, { Id(hier), U(id) }); return id * 2 - 3; }
inline void* __cdecl FakeMatArray(void* hier) { Push(162, { Id(hier) }); return W.bones.p(); }
inline void __cdecl FakeCallback(void* particle, int32 a, void** data) { Push(170, { Id(particle), U(a), Id(*data) }); }

inline std::vector<IsoSite> Sites() {
    return {
        { 0x72F420, DA(static_cast<void* (*)(uint32, uint32)>(&CMemoryMgr::Malloc)), SP(FakeMalloc) },
        { 0x72F430, DA(&CMemoryMgr::Free), SP(FakeFree) },
        { 0x821B1E, 0, SP(FakeRand) },                                       // rand del EXE
        { 0, DA(&CGeneral::GetRandomNumber), SP(FakeRandomNumber) },         // GetRandomNumber de la DLL
        { 0x4DD3C0, DA(&CAEFireAudioEntity::AddAudioEvent), SP(FakeAddAudio) },
        { 0x4DCF40, DA(&CAEFireAudioEntity::Terminate), SP(FakeTerminate) },
        { 0x7F2A50, 0, SP(FakeMatCreate) },
        { 0x7F2A20, 0, SP(FakeMatDestroy) },
        { 0x734A40, DA(&GetAnimHierarchyFromSkinClump), SP(FakeSkinHier) },
        { 0x7C51A0, 0, SP(FakeBoneIndex) },
        { 0x7C5120, 0, SP(FakeMatArray) },
    };
}

// ---------------------------------------------------------------------------------------------------------------------
// Estado global tocado: frustum y contador de la pila de matrices del gestor global de efectos
// ---------------------------------------------------------------------------------------------------------------------
inline std::vector<GlobalRange> Globals() {
    return {
        { 0xA9AE80 + 0x34, 0x54 },     // g_fxMan.m_Frustum
        { 0xA9AE80 + 0x88, 4 },        // g_fxMan.m_nCurrentMatrix
    };
}
// Las matrices temporales de la pila de g_fxMan que pueden usarse (desde el contador actual) se guardan y restauran por ejecucion.
inline std::vector<GlobalRange> PoolMatrixRanges() {
    std::vector<GlobalRange> g;
    const auto base = uintptr_t(&g_fxMan);
    const uint32 idx = *reinterpret_cast<uint32*>(base + 0x88);
    for (uint32 k = idx; k < idx + 3 && k < 8; ++k) g.push_back({ uintptr_t(*reinterpret_cast<uint32*>(base + 0x8C + 4 * k)), 0x40 });
    return g;
}

// ---------------------------------------------------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------------------------------------------------
inline void RandBytes(uint8* p, size_t n, uint32& s) { for (size_t i = 0; i < n; ++i) p[i] = uint8(Rnd(s) >> 11); }
inline void RandMatrix(uint8* m, uint32& s) {
    static constexpr uint32 flags[] = { 0, 1, 2, 3, 0x20000, 0x20003, 0x30000, 0x10003 };
    for (int k = 0; k < 16; ++k) At<float>(m, 4 * k) = PickSmall(s);
    At<uint32>(m, 0x0C) = flags[Rnd(s) % std::size(flags)];
    At<uint32>(m, 0x1C) = Rnd(s); At<uint32>(m, 0x2C) = Rnd(s); At<uint32>(m, 0x3C) = Rnd(s);
}
inline void RandSphere(uint8* p, uint32& s) {
    for (int k = 0; k < 3; ++k) At<float>(p, 4 * k) = PickSmall(s);
    At<float>(p, 0x0C) = std::fabs(PickSmall(s)); At<uint32>(p, 0x10) = Rnd(s);
}
inline void WorldReset(uint32& s) {
    static constexpr int primTypes[] = { 0, 0, 1, 2, -1 };
    W.nArena = 0; W.nMat = 0; W.badCall = false; W.rng = Rnd(s) | 1u;
    primVtbl[0] = SP(PrimDtor); primVtbl[1] = SP(PrimInit); primVtbl[2] = SP(PrimBad); primVtbl[3] = SP(PrimReset); primVtbl[4] = SP(PrimBad); primVtbl[5] = SP(PrimBad);
    for (auto& v : bpVtbl) v = SP(PrimBad);
    bpVtbl[3] = SP(BpCreate);
    RandBytes(W.sys.raw.data(), W.sys.raw.size(), s);
    RandBytes(W.bp.raw.data(), W.bp.raw.size(), s);
    for (auto& b : W.bpPrim) RandBytes(b.raw.data(), b.raw.size(), s);
    for (auto& b : W.inst) RandBytes(b.raw.data(), b.raw.size(), s);
    for (auto& b : W.part) RandBytes(b.raw.data(), b.raw.size(), s);
    for (auto& b : W.mat) { b.Fill(0xCD); RandMatrix(b.p(), s); }
    for (size_t k = 0; k < 4; ++k) { W.sph[k].Fill(0xCD); RandSphere(W.sph[k].p(), s); }
    for (auto& b : W.arena) b.Fill(0xCD);
    W.primTable.Fill(0); W.bpPrimTable.Fill(0);
    RandBytes(W.entity.raw.data(), W.entity.raw.size(), s);
    W.clump.Fill(0xAB); W.hier.Fill(0xBC); W.bones.Fill(0xCD); W.out.Fill(0xCD);
    for (size_t k = 0; k < 4; ++k) {
        auto* bpp = W.bpPrim[k].p();
        At<void*>(bpp, 0) = bpVtbl;
        At<int8>(bpp, 4) = int8(primTypes[Rnd(s) % 5]);
        At<void*>(bpp, 0x20) = nullptr; At<void*>(bpp, 0x24) = nullptr; At<uint32>(bpp, 0x28) = 0;
        At<void*>(W.inst[k].p(), 0) = primVtbl;
        At<uint8>(W.inst[k].p(), 0xC) = uint8(Rnd(s) & 1);
        At<void*>(W.primTable.p(), 4 * k) = W.inst[k].p();
        At<void*>(W.bpPrimTable.p(), 4 * k) = bpp;
    }
    // sistema: punteros coherentes con el mundo; FireAE+0x10 (m_tempSound) a cero como lo deja el constructor del EXE
    auto* sys = W.sys.p();
    At<void*>(sys, 0x08) = W.bp.p();
    At<void*>(sys, 0x0C) = Chance(s, 70) ? W.mat[0].p() : nullptr;
    At<void*>(sys, 0x74) = nullptr;
    At<void*>(sys, 0x78) = W.primTable.p();
    At<uint32>(sys, 0x8C) = 0;
    At<uint8>(sys, 0x50) = uint8(Rnd(s) % 4); At<uint8>(sys, 0x51) = uint8(Rnd(s) % 4);
    for (int off : { 0x54, 0x58, 0x64, 0x68, 0x6C, 0x70 }) At<float>(sys, off) = PickSmall(s);
    // plantilla
    auto* bp = W.bp.p();
    At<uint32>(bp, 8) = Rnd(s);
    At<float>(bp, 0x0C) = std::fabs(PickSmall(s)); At<float>(bp, 0x10) = PickSmall(s); At<float>(bp, 0x14) = PickSmall(s);
    At<uint16>(bp, 0x18) = uint16(Rnd(s)); At<uint8>(bp, 0x1A) = uint8(Rnd(s) % 4); At<uint8>(bp, 0x1B) = uint8(RgI(s, 0, 4));
    At<void*>(bp, 0x1C) = W.bpPrimTable.p(); At<void*>(bp, 0x20) = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------
// Captura de estado
// ---------------------------------------------------------------------------------------------------------------------
inline uint32 HashAll(const uint8* p, size_t stride, size_t count) { uint64 h = 1469598103934665603ull; for (size_t k = 0; k < count; ++k) h = Fnv(p + k * stride, stride, h); return uint32(h ^ (h >> 32)); }
template<class A> inline uint32 HashArr(const A& a) { uint64 h = 1469598103934665603ull; for (auto& b : a) h = Fnv(b.raw.data(), b.raw.size(), h); return uint32(h ^ (h >> 32)); }
inline json Snap(const std::vector<uint8>& sysBytes) {
    json j;
    j["sys"] = Arr(sysBytes.data(), sysBytes.size());
    j["bp"] = Fnv32(W.bp.raw.data(), W.bp.raw.size());
    j["bpPrim"] = HashArr(W.bpPrim); j["inst"] = HashArr(W.inst); j["part"] = HashArr(W.part);
    j["mat"] = json::array(); for (auto& b : W.mat) j["mat"].push_back(Fnv32(b.raw.data(), b.raw.size()));
    j["sph"] = json::array(); for (auto& b : W.sph) j["sph"].push_back(Fnv32(b.raw.data(), b.raw.size()));
    j["arena"] = json::array(); for (auto& b : W.arena) j["arena"].push_back(Fnv32(b.raw.data(), b.raw.size()));
    j["tables"] = json::array({ Fnv32(W.primTable.raw.data(), W.primTable.raw.size()), Fnv32(W.bpPrimTable.raw.data(), W.bpPrimTable.raw.size()), Fnv32(W.entity.raw.data(), W.entity.raw.size()),
                                Fnv32(W.out.raw.data(), W.out.raw.size()), Fnv32(W.bones.raw.data(), W.bones.raw.size()), Fnv32(W.clump.raw.data(), W.clump.raw.size()), Fnv32(W.hier.raw.data(), W.hier.raw.size()) });
    j["bad_call"] = W.badCall;
    j["n_arena"] = W.nArena; j["n_mat"] = W.nMat;
    return j;
}

// ---------------------------------------------------------------------------------------------------------------------
// Ejecucion de un caso
// ---------------------------------------------------------------------------------------------------------------------
template<class R, class... A> inline R ThisCall(uintptr_t addr, void* self, A... a) { return reinterpret_cast<R(__thiscall*)(void*, A...)>(addr)(self, a...); }

inline void SetAllHooks(const std::vector<Hook*>& hooks, bool cand) { for (auto* h : hooks) if (h->Hooked() != cand) h->State(cand); }

inline json Run(size_t f, unsigned i, bool cand, const std::vector<Hook*>& hooks) {
    const auto& fn = fns[f]; const std::string_view nm = fn.name;
    uint32 s = Seed(i, uint32(f) + 101);
    Ctl ctl;
    WorldReset(s);
    json ret = nullptr, extra = json::object();
    const auto A = fn.address;
    void* const self = W.sys.p();
    auto* const sys = W.sys.p();
    auto* const bp = W.bp.p();
    const auto poolSaved = SaveRanges(PoolMatrixRanges());
    bool maskPrimsPtr = false, maskFireAE = false, heapPrims = false;
    FxPrim_c** heapTable = nullptr;
    auto* const g = reinterpret_cast<uint8*>(&g_fxMan);
    const uint32 poolIndexBefore = At<uint32>(g, 0x88);

    // ---- preparacion especifica (antes de activar el hook; sin trazas) ----
    std::function<void()> call;
    if (nm == "AddOffsetPos") {
        CVector pos{ PickF(s), PickF(s), PickF(s) };
        RandMatrix(sys + 0x10, s);                     // la matriz local es finita: como mucho un NaN por suma
        call = [=]() { ThisCall<void>(A, self, &pos); };
    } else if (nm == "AttachToBone") {
        At<void*>(W.entity.p(), 0x18) = W.clump.p();
        const int32 bone = Chance(s, 70) ? RgI(s, 0, 40) : int32(Rnd(s) % 5000) - 1000;
        call = [=]() { ThisCall<void>(A, self, W.entity.p(), U(bone)); };
    } else if (nm == "Constructor") {
        maskFireAE = true;
        call = [=, &ret]() { ret = P(ThisCall<void*>(A, self)) == P(self); };
    } else if (nm == "CopyParentMatrix") {
        At<void*>(sys, 0x0C) = W.mat[0].p();
        call = [=]() { ThisCall<void>(A, self); };
    } else if (nm == "Destructor") {
        const bool hasSphere = Chance(s, 60), ownsMatrix = Chance(s, 60);
        At<void*>(sys, 0x74) = hasSphere ? W.sph[0].p() : nullptr;
        At<void*>(sys, 0x0C) = W.mat[0].p();
        At<uint8>(sys, 0x62) = uint8((At<uint8>(sys, 0x62) & ~1) | (ownsMatrix ? 1 : 0));
        maskFireAE = true;
        call = [=]() { ThisCall<void>(A, self); };
    } else if (nm == "DoFxAudio") {
        static const char* names[] = { "fire", "fire_med", "fire_large", "fire_car", "fire_bike", "Flame", "molotov_flame", "water_hydrant", "water_fountain", "water_fnt_tme", "smoke_flare", "teargas", "heli_dust", "nada", "FIRE", "fire " };
        const char* n = Pick(s, names);
        At<uint32>(bp, 8) = Chance(s, 90) ? CKeyGen::GetUppercaseKey(n) : Rnd(s);
        CVector pos{ PickSmall(s), PickSmall(s), PickSmall(s) };
        call = [=]() { reinterpret_cast<void(__thiscall*)(void*, CVector)>(A)(self, pos); };
        extra["name"] = n;
    } else if (nm == "EnablePrim") {
        const int32 idx = RgI(s, 0, 3); const bool en = Chance(s, 50);
        call = [=]() { ThisCall<void>(A, self, idx, en); };
    } else if (nm == "Exit") {
        const int n = Chance(s, 15) ? 200 : RgI(s, 0, 4);      // 200 = -56 como byte con signo: el EXE no recorre nada
        At<uint8>(bp, 0x1B) = uint8(n);
        const int count = n >= 0 && n <= 4 ? n : 0;
        heapTable = new FxPrim_c* [size_t(std::max(count, 1))];
        for (int k = 0; k < count; ++k) heapTable[k] = Chance(s, 15) ? nullptr : reinterpret_cast<FxPrim_c*>(W.inst[k].p());
        At<void*>(sys, 0x78) = heapTable; heapPrims = true; maskPrimsPtr = true;
        call = [=]() { ThisCall<void>(A, self); };
    } else if (nm == "ForAllParticles") {
        const int n = Chance(s, 10) ? 200 : RgI(s, 0, 3);
        At<uint8>(bp, 0x1B) = uint8(n);
        size_t used = 0;
        for (int k = 0; k < (n >= 0 && n <= 3 ? n : 0); ++k) {
            auto* bpp = W.bpPrim[k].p();
            const int cnt = RgI(s, 0, 2);
            void* prev = nullptr; void* head = nullptr;
            for (int q = 0; q < cnt && used < W.part.size(); ++q, ++used) {
                auto* pt = W.part[used].p();
                At<void*>(pt, 0) = prev; At<void*>(pt, 4) = nullptr;
                At<void*>(pt, 0x28) = Chance(s, 60) ? static_cast<void*>(sys) : static_cast<void*>(W.sph[1].p());
                if (prev) At<void*>(static_cast<uint8*>(prev), 4) = pt; else head = pt;
                prev = pt;
            }
            At<void*>(bpp, 0x20) = head;
        }
        call = [=, &ret]() { ret = ThisCall<uint32>(A, self, SP(FakeCallback), W.sph[3].p()); };
    } else if (nm == "GetBoundingSphereLcl") {
        At<void*>(sys, 0x74) = Chance(s, 70) ? W.sph[0].p() : nullptr;
        call = [=, &ret]() { ret = ThisCall<bool>(A, self, W.out.p()); };
    } else if (nm == "GetBoundingSphereWld") {
        At<void*>(sys, 0x74) = Chance(s, 80) ? W.sph[0].p() : nullptr;
        RandMatrix(sys + 0x10, s);
        call = [=, &ret]() { ret = ThisCall<bool>(A, self, W.out.p()); };
    } else if (nm == "GetCompositeMatrix") {
        RandMatrix(sys + 0x10, s);
        call = [=]() { ThisCall<void>(A, self, W.out.p()); };
    } else if (nm == "GetPlayStatus") {
        call = [=, &ret]() { ret = uint32(ThisCall<uint8>(A, self)); };
    } else if (nm == "Init") {
        const int n = RgI(s, 0, 4);
        At<uint8>(bp, 0x1B) = uint8(n);
        At<void*>(bp, 0x20) = Chance(s, 50) ? W.sph[2].p() : nullptr;
        for (int k = 0; k < 4; ++k) { At<uint32>(W.inst[k].p(), 4) = 0; At<uint32>(W.inst[k].p(), 8) = 0; }
        At<void*>(sys, 0x78) = nullptr; heapPrims = true; maskPrimsPtr = true;
        void* parent = Chance(s, 50) ? W.mat[2].p() : nullptr;
        call = [=, &ret]() { ret = ThisCall<bool>(A, self, bp, W.mat[1].p(), parent); };
    } else if (nm == "IsVisible") {
        At<void*>(sys, 0x74) = Chance(s, 85) ? W.sph[0].p() : nullptr;
        RandMatrix(sys + 0x10, s);
        auto* fr = g + 0x34;                       // g_fxMan.m_Frustum: esfera (centro, radio, hint) + 4 planos (normal, d)
        for (int k = 0; k < 3; ++k) At<float>(fr, 4 * k) = PickSmall(s) * 4.f;
        At<float>(fr, 0x0C) = std::fabs(PickSmall(s)) * 20.f + 1.f; At<uint32>(fr, 0x10) = 0;
        for (int p = 0; p < 4; ++p) { for (int k = 0; k < 3; ++k) At<float>(fr, 0x14 + 16 * p + 4 * k) = PickSmall(s); At<float>(fr, 0x14 + 16 * p + 12) = PickSmall(s) * 3.f; }
        call = [=, &ret]() { ret = ThisCall<bool>(A, self); };
    } else if (nm == "Kill" || nm == "Pause" || nm == "Play" || nm == "PlayAndKill" || nm == "Stop") {
        At<uint8>(bp, 0x1B) = uint8(RgI(s, 0, 4));
        call = [=]() { ThisCall<void>(A, self); };
    } else if (nm == "ResetBoundingSphere") {
        At<void*>(bp, 0x20) = Chance(s, 55) ? W.sph[2].p() : nullptr;
        At<void*>(sys, 0x74) = Chance(s, 50) ? W.sph[0].p() : nullptr;
        call = [=]() { ThisCall<void>(A, self); };
    } else if (nm == "SetBoundingSphere") {
        At<void*>(sys, 0x74) = Chance(s, 50) ? W.sph[0].p() : nullptr;
        void* arg = Chance(s, 70) ? W.sph[1].p() : nullptr;
        call = [=]() { ThisCall<void>(A, self, arg); };
    } else if (nm == "SetConstTime") {
        const bool on = Chance(s, 50); const float t = PickF(s);
        call = [=]() { ThisCall<void>(A, self, on, t); };
    } else if (nm == "SetRateMult" || nm == "SetTimeMult") {
        const float v = PickF(s);
        call = [=]() { ThisCall<void>(A, self, v); };
    } else if (nm == "SetLocalParticles" || nm == "SetMustCreatePrts" || nm == "SetZTestEnable") {
        const bool v = Chance(s, 50);
        call = [=]() { ThisCall<void>(A, self, v); };
    } else if (nm == "SetMatrix") {
        const bool owns = Chance(s, 50);
        At<uint8>(sys, 0x62) = uint8((At<uint8>(sys, 0x62) & ~1) | (owns ? 1 : 0));
        At<void*>(sys, 0x0C) = owns || Chance(s, 80) ? W.mat[0].p() : nullptr;
        call = [=]() { ThisCall<void>(A, self, W.mat[1].p()); };
    } else if (nm == "SetOffsetPos" || nm == "SetVelAdd") {
        CVector pos;
        for (int k = 0; k < 3; ++k) At<uint32>(reinterpret_cast<uint8*>(&pos), 4 * k) = QuietNaN(Rnd(s));      // bits arbitrarios (copia cruda), sin NaN de senalizacion
        call = [=]() { ThisCall<void>(A, self, &pos); };
    } else {
        return json{ {"switches", false}, {"error", "funcion sin fixture"} };
    }

    // ---- activacion (despues de preparar; la comprobacion va tras State()) y ejecucion ----
    SetAllHooks(hooks, cand);
    const bool switches = ReversingProbe::JumpsToDll(A) == cand;
    nrec = 0;
    call();
    const unsigned fp = _controlfp(0, 0) & _MCW_PC;

    // ---- captura ----
    std::vector<uint8> sb(sys, sys + kSysSize);
    if (maskPrimsPtr) std::memset(sb.data() + 0x78, 0xEE, 4);
    if (maskFireAE) std::memset(sb.data() + 0x7C, 0xEE, kSysSize - 0x7C);
    if (nm == "Constructor") {                  // Como el original: solo se escriben FireAE+0x10 (sys+0x8C) y FireAE+0x84 (sys+0x100); el vptr y el resto del FireAE no se comparan
        std::memset(sb.data() + 0x7C, 0xEE, kSysSize - 0x7C);
        std::memcpy(sb.data() + 0x8C, sys + 0x8C, 4); std::memcpy(sb.data() + 0x100, sys + 0x100, 4);
    }
    if (heapPrims) {
        auto* tab = At<FxPrim_c**>(sys, 0x78);
        if (nm == "Init") {
            json t = json::array();
            for (int k = 0; k < int(int8(At<uint8>(bp, 0x1B))) && k < 4; ++k) t.push_back(Id(tab[k]));
            extra["prims"] = t;
            delete[] tab;                    // reservado por new[] (EXE 0x821195, tambien desde la DLL) y liberado igual
        }                                    // Exit: la tabla ya la libero la funcion con delete[]
    }
    json out = Snap(sb);
    out["extra"] = extra; out["ret"] = ret; out["switches"] = switches; out["fp"] = fp; out["trace"] = Recs();
    out["pool_index"] = json::array({ poolIndexBefore, At<uint32>(g, 0x88) });
    if (nm == "AttachToBone") out["extra"]["parent_off"] = int64(intptr_t(At<void*>(sys, 0x0C)) - intptr_t(W.bones.p()));
    if (nm == "CopyParentMatrix" || nm == "SetMatrix" || nm == "Destructor") out["extra"]["parent_id"] = Id(At<void*>(sys, 0x0C));
    out["frustum"] = Fnv32(g + 0x34, 0x54);
    RestoreRanges(poolSaved);
    At<uint32>(g, 0x88) = poolIndexBefore;
    return out;
}
}}

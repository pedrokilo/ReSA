#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <new>
#include <set>
#include <map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn208A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 15 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedGlobalInstall / RH_ScopedNamedGlobalInstall). El banco invoca la direccion del EXE con objetos
// sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair) cuando el colaborador
// tiene hook RH (apagado durante el banco), y solo en la direccion del EXE cuando la DLL lo llama por ella:
//   0 PhoneAnimFinishCallback: CPad::GetPad 0x53FB70 y CPed::SetPedState 0x5E4500 (hook).
//   3, 4 LoadSurfaceInfos / LoadSurfaceAudioInfos: CFileMgr::OpenFile 0x538900, CFileLoader::LoadLine 0x536F80, CFileMgr::CloseFile 0x5389D0 y SurfaceInfos_c::GetSurfaceIdFromName 0x55D220. El sscanf corre real (el
//     del EXE en el lado original y el de la DLL en el candidato); las lineas las da el falso de LoadLine.
//   6 RpClumpGtaStreamReadAtOnce: RwStreamFindChunk, RwStreamRead, RpClumpCreate, _rwFrameListStreamRead, _rwFrameListDeinitialize, RpGeometryStreamRead, RpGeometryDestroy, RpAtomicCreate, RpAtomicSetFrame,
//     RpAtomicSetGeometry, RpAtomicDestroy, RpClumpAddAtomic, RpClumpDestroy, CMemoryMgr::Malloc y Free (guiones por tipo de llamada, como Fn95C). Los ayudantes con registros propios (0x72E080, 0x72E270,
//     0x72E360) corren reales en el EXE y como funciones privadas en la DLL.
//   8 CMemoryHeap::Shutdown: free() del CRT 0x82413F. 9 FillInBlockData: JoinAdjacentFreeBlocks 0x72E870. 10 Malloc: FillInBlockData 0x72EC80. 11 Init: CMemoryHeap::Malloc 0x72ED80.
//   14 UserTracksLegacyString::Reallocate: operator new 0x82119A y operator delete 0x8214BD del EXE (el falso de operator new lanza una excepcion de C++ segun el caso: SEH real, la excepcion que escapa se recoge con
//     __try/__except en GateTry). Los falsos de funciones de uso masivo (free, new, delete, CMemoryMgr::Malloc/Free) reenvian a la funcion original (trampolin) las llamadas de hilos ajenos al banco.
// Se observan: los bytes cambiados de las cuatro arenas, los cambios de la memoria global, los eventos de los falsos (con sus argumentos), el valor devuelto y el estado x87.

namespace F208ALive {
using namespace F208AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F208AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;
inline u32 G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline void SetG32(uintptr_t a, u32 v) { *reinterpret_cast<uint32*>(a) = v; }

// ================================================================================================ estado de los falsos
struct State {
    const C* c{};
    // PhoneAnimFinishCallback
    uintptr_t assoc{}, ped{};
    // superficies
    std::vector<std::string> lines; unsigned line{}, id{};
    // clump
    unsigned fc{}, rd{}, ge{}, at{};
    // heap / strings
    u32 joinRet{}, mallocRet{};
    unsigned news{};
};
inline State g;
inline void* gTrampFree{}; inline void* gTrampNew{}; inline void* gTrampDelete{}; inline void* gTrampMalloc{}; inline void* gTrampFree2{};
inline u32 StrHash(const char* s) { return s ? Fnv(s, strlen(s)) : 0u; }

// ------------------------------------------------ 0: PhoneAnimFinishCallback
inline u32 __cdecl FkGetPad(u32 idx) {                                                 // 0x53FB70
    if (!OnBenchThread()) return 0xB73458u + idx * 0x134u;
    Ev& e = PushEv(0x53FB70, 1); e.v[0] = idx;
    const u32 mode = g.c->a[12];
    if (mode & 1) SetG32(g.assoc + 0x18, g.c->a[13]);
    if (mode & 2) SetG32(g.ped + 0x530, g.c->a[14]);
    return u32(arenaC.Obj(g.c->a[10]));
}
inline u32 __fastcall FkSetPedState(void* self, void*, u32 state) {                    // 0x5E4500 (thiscall, ret 4)
    Ev& e = PushEv(0x5E4500, 2); e.v[0] = Tag(self); e.v[1] = state;
    return 0;
}
// ------------------------------------------------ 3, 4: superficies
inline u32 __cdecl FkOpenFile(const char* path, const char* mode) {                    // 0x538900
    Ev& e = PushEv(0x538900, 2); e.v[0] = StrHash(path); e.v[1] = StrHash(mode);
    return g.c->a[20];
}
inline const char* __cdecl FkLoadLine(u32 file) {                                      // 0x536F80
    Ev& e = PushEv(0x536F80, 1); e.v[0] = file;
    return g.line < g.lines.size() ? g.lines[g.line++].c_str() : nullptr;
}
inline u32 __cdecl FkCloseFile(u32 file) { Ev& e = PushEv(0x5389D0, 1); e.v[0] = file; return 0; }   // 0x5389D0
inline u32 __fastcall FkGetSurfaceId(void* self, void*, const char* name) {            // 0x55D220 (thiscall, ret 4)
    Ev& e = PushEv(0x55D220, 2); e.v[0] = Tag(self); e.v[1] = StrHash(name);
    const unsigned k = g.id++;
    return k < 6 ? g.c->a[6 + k] : 0u;
}
// ------------------------------------------------ 6: lectura de clump (guiones, como Fn95C)
// Eventos: 1 FindChunk [stream, tipo, hay_longitud, hay_version]; 2 Read [stream, buffer, longitud]; 9 Malloc [tamano, hint]; 10 Free [puntero]; 11 RpGeometryStreamRead [stream]; 12 RpGeometryDestroy [geometria];
// 13 RpAtomicCreate []; 14 RpAtomicSetFrame [atomico, frame]; 15 RpAtomicSetGeometry [atomico, geometria, flags]; 16 RpAtomicDestroy [atomico]; 17 RpClumpCreate []; 18 RpClumpAddAtomic [clump, atomico];
// 19 RpClumpDestroy [clump]; 20 _rwFrameListStreamRead [stream, lista]; 21 _rwFrameListDeinitialize [lista].
inline u32 V(unsigned i) { return g.c->v[i]; }
inline i32 __cdecl FkFind(void* stream, u32 type, u32* len, u32* ver) {                // 0x7ED2D0
    const u32 k = g.fc++;
    Ev& e = PushEv(1, 4); e.v[0] = Tag(stream); e.v[1] = type; e.v[2] = len != nullptr; e.v[3] = ver != nullptr;
    if (len) *len = k < kFc ? V(kVFcLen + k) : 0u;
    if (ver) *ver = 0x1803FFFFu;
    return i32(k < kFc ? V(kVFcVal + k) : 0u);
}
inline u32 __cdecl FkRead(void* stream, void* buf, u32 len) {                          // 0x7EC9D0
    const u32 k = g.rd++;
    Ev& e = PushEv(2, 3); e.v[0] = Tag(stream); e.v[1] = Tag(buf); e.v[2] = len;
    const u32 mode = k < kRd ? V(kVRdMode + k) : 0u;
    u32 pay[4]{};
    if (k < kRd) for (unsigned i = 0; i < 4; ++i) pay[i] = V(kVRdPay + 4 * k + i);
    memcpy(buf, pay, len < 16 ? len : 16);
    switch (mode) {
    case 1: return len - 1;
    case 2: return len + 1;
    case 3: return 0;
    case 4: return 0xFFFFFFFFu;
    default: return len;
    }
}
inline void* __cdecl FkMallocRw(u32 size, u32 hint) {                                  // 0x72F420
    if (!OnBenchThread()) return reinterpret_cast<void*(__cdecl*)(u32, u32)>(gTrampMalloc)(size, hint);
    Ev& e = PushEv(9, 2); e.v[0] = size; e.v[1] = hint;
    return V(kVMallocNull) ? nullptr : reinterpret_cast<void*>(arenaB.Obj(0x100));
}
inline void __cdecl FkFreeRw(void* p) {                                                // 0x72F430
    if (!OnBenchThread()) { reinterpret_cast<void(__cdecl*)(void*)>(gTrampFree2)(p); return; }
    Ev& e = PushEv(10, 1); e.v[0] = Tag(p);
}
inline u32 __cdecl FkGeoRead(void* stream) {                                           // 0x74D190
    const u32 k = g.ge++;
    Ev& e = PushEv(11, 1); e.v[0] = Tag(stream);
    return k < kGeo ? V(kVGeo + k) : 0u;
}
inline u32 __cdecl FkGeoDestroy(u32 geo) { Ev& e = PushEv(12, 1); e.v[0] = geo; return 1; }                                   // 0x74CCC0
inline void* __cdecl FkAtomCreate() {                                                                                        // 0x749C50
    const u32 k = g.at++;
    PushEv(13, 0);
    if (k >= kAtoms || ((V(kVAtomNull) >> k) & 1u)) return nullptr;
    return reinterpret_cast<void*>(arenaD.Obj(0x40 * k));
}
inline void* __cdecl FkSetFrame(void* atom, u32 frame) { Ev& e = PushEv(14, 2); e.v[0] = Tag(atom); e.v[1] = frame; return atom; }   // 0x74BF20
inline void* __cdecl FkSetGeom(void* atom, u32 geo, u32 flags) { Ev& e = PushEv(15, 3); e.v[0] = Tag(atom); e.v[1] = geo; e.v[2] = flags; return atom; }   // 0x749D40
inline u32 __cdecl FkAtomDestroy(void* atom) { Ev& e = PushEv(16, 1); e.v[0] = Tag(atom); return 1; }                          // 0x749DC0
inline void* __cdecl FkClumpCreate() { PushEv(17, 0); return V(kVClumpNull) ? nullptr : reinterpret_cast<void*>(arenaC.Obj(0)); }   // 0x74A290
inline void* __cdecl FkAddAtom(void* clump, void* atom) { Ev& e = PushEv(18, 2); e.v[0] = Tag(clump); e.v[1] = Tag(atom); return clump; }   // 0x74A490
inline u32 __cdecl FkClumpDestroy(void* clump) { Ev& e = PushEv(19, 1); e.v[0] = Tag(clump); return 1; }                       // 0x74A310
inline u32 __cdecl FkFlRead(void* stream, u32* fl) {                                                                           // 0x807970
    Ev& e = PushEv(20, 2); e.v[0] = Tag(stream); e.v[1] = Tag(fl);
    const u32 mode = V(kVFlMode);
    fl[0] = mode == 0 ? u32(arenaB.Obj(0)) : (mode == 1 ? 0u : 0xC0DE0000u);
    fl[1] = V(kVFlNum);
    return V(kVFlRet);
}
inline u32 __cdecl FkFlDeinit(u32* fl) { Ev& e = PushEv(21, 1); e.v[0] = Tag(fl); return u32(uintptr_t(fl)); }                  // 0x807720
// ------------------------------------------------ 8, 9, 10, 11: heap
inline void __cdecl FkFree(void* p) {                                                  // 0x82413F (free del CRT del EXE)
    if (!OnBenchThread()) { reinterpret_cast<void(__cdecl*)(void*)>(gTrampFree)(p); return; }
    Ev& e = PushEv(0x82413F, 1); e.v[0] = Tag(p);
}
inline u32 __fastcall FkJoin(void* self, void*, void* blk) {                           // 0x72E870 (thiscall, ret 4)
    Ev& e = PushEv(0x72E870, 2); e.v[0] = Tag(self); e.v[1] = Tag(blk);
    return g.joinRet;
}
inline u32 __fastcall FkFillIn(void* self, void*, void* desc, void* next, u32 size) {  // 0x72EC80 (thiscall, ret 0Ch)
    Ev& e = PushEv(0x72EC80, 4); e.v[0] = Tag(self); e.v[1] = Tag(desc); e.v[2] = Tag(next); e.v[3] = size;
    return 0;
}
inline u32 __fastcall FkHeapMalloc(void* self, void*, u32 size) {                      // 0x72ED80 (thiscall, ret 4)
    Ev& e = PushEv(0x72ED80, 3); e.v[0] = Tag(self); e.v[1] = size; e.v[2] = BitsOf(reinterpret_cast<const u8*>(self) + 0x38);
    return g.mallocRet;
}
// ------------------------------------------------ 14: operator new / delete del EXE
inline void* __cdecl FkNew(u32 size) {                                                 // 0x82119A
    if (!OnBenchThread()) return reinterpret_cast<void*(__cdecl*)(u32)>(gTrampNew)(size);
    const unsigned k = g.news++;
    Ev& e = PushEv(0x82119A, 1); e.v[0] = size;
    const u32 mode = g.c->a[10];
    if ((k == 0 && mode >= 1) || (k == 1 && mode == 2)) throw std::bad_alloc();
    return reinterpret_cast<void*>(k == 0 ? arenaC.Obj(g.c->a[9]) : arenaC.Obj(g.c->a[11]));
}
inline void __cdecl FkDelete(void* p) {                                                // 0x8214BD
    if (!OnBenchThread()) { reinterpret_cast<void(__cdecl*)(void*)>(gTrampDelete)(p); return; }
    Ev& e = PushEv(0x8214BD, 1); e.v[0] = Tag(p);
}

// ================================================================================================ cierre de un lado
inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = RegionsHash();
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

// Descripcion de la llamada de un caso
struct Call {
    std::array<u32, 8> args{};
    u32 argCount{}, popBytes{}, ecx{};
};

// Direccion de una arena desde un indice de arena (0..3)
inline uintptr_t ArenaObj(unsigned arena, size_t off) {
    switch (arena) { case 0: return arenaA.Obj(off); case 1: return arenaB.Obj(off); case 2: return arenaC.Obj(off); default: return arenaD.Obj(off); }
}
// Cadena de la lista libre del heap (nodos en D desde a[19], tamanos a[14..17]) y sentinelas dentro del objeto heap en A (Begin en +8, End en +0x20)
inline void BuildFreeList(const C& c, u32 heap) {
    const auto& a = c.a;
    const u32 k = a[13];
    const u32 end = heap + 0x20, begin = heap + 8;
    arenaA.Put32(a[0] + 0x20, 0x7FFFFFFFu);
    std::vector<u32> nodes;
    for (u32 i = 0; i < k; ++i) nodes.push_back(u32(arenaD.Obj(a[19] + 0x20 * i)));
    arenaA.Put32(a[0] + 0x18, k ? nodes[0] : end);
    arenaA.Put32(a[0] + 0x1C, 0);
    for (u32 i = 0; i < k; ++i) {
        const u32 off = a[19] + 0x20 * i;
        arenaD.Put32(off + 0, a[14 + i]);
        arenaD.Put32(off + 0x10, i + 1 < k ? nodes[i + 1] : end);
        arenaD.Put32(off + 0x14, i ? nodes[i - 1] : begin);
    }
    arenaA.Put32(a[0] + 0x34, k ? nodes[k - 1] : begin);
}
// Tabla de CommonSize (entradas -1..63 de 0x3C bytes en B desde a[5]); la lista de cada entrada la fija `emptyFn` (true: vacia = Start.next == End propio)
inline u32 CsEntry(const C& c, int idx) { return u32(arenaB.Obj(c.a[5] + idx * 0x3C)); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    g = State{}; g.c = &c;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kPhoneCb: {
        arenaA.Put32(o + 0x2C, a[4]); arenaA.Put32(o + 0x18, a[5]); arenaA.Put32(o + 0x1C, a[6]);
        arenaB.Put32(a[7] + 0x530, a[8]); arenaB.Put32(a[7] + 0x46C, a[9]);
        arenaC.Put32(a[10] + 0x10C, a[11]);
        g.assoc = arenaA.Obj(o); g.ped = arenaB.Obj(a[7]);
        call.args[0] = u32(g.assoc); call.args[1] = u32(g.ped); call.argCount = 2; call.popBytes = 8;
        break;
    }
    case kPhoneCtor:
    case kMgrInit:
        if (kKind[c.fn] == kMgrInit) *reinterpret_cast<u8*>(kMarker) = 0x5A;
        break;
    case kSlot:
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 4;
        break;
    case kSurfInfo:
    case kSurfAud: {
        // lineas: las a[4] primeras de `s` (cadenas terminadas en NUL)
        size_t pos = 0;
        for (u32 i = 0; i < a[4] && pos < c.s.size(); ++i) {
            const char* p = reinterpret_cast<const char*>(c.s.data() + pos);
            g.lines.emplace_back(p);
            pos += strlen(p) + 1;
        }
        break;
    }
    case kPoint:
        for (u32 i = 0; i < 8; ++i) arenaA.Put16(o + 2 * i, a[4 + i]);
        call.args[0] = a[12]; call.args[1] = a[13]; call.args[2] = a[14]; call.argCount = 3; call.popBytes = 0;
        break;
    case kClump:
        for (u32 i = 0; i < kFa; ++i) arenaB.Put32(4 * i, V(kVFa + i));
        for (u32 i = 0; i < kGa; ++i) arenaB.Put32(0x100 + 4 * i, V(kVGa + i));
        arenaC.Put32(4, V(kVClumpParent));
        for (u32 i = 0; i < kAtoms; ++i) arenaD.Put8(0x40 * i + 2, V(kVAtomFlags + i));
        call.args[0] = u32(arenaA.Obj(0x10)); call.argCount = 1; call.popBytes = 4;
        break;
    case kJoin: {
        const u32 b = a[4], sk = a[5], sP = a[6], sD = a[7], sN = a[8], mode = a[10];
        u32 pOff = 0, dOff;
        if (mode == 1) { pOff = b + sk; dOff = pOff + sP + 0x10; }
        else if (mode == 2) { pOff = a[19] + sk; dOff = b + sk; }
        else dOff = b + sk;
        const u32 nOff = dOff + sD + 0x10;
        const u32 Pa = u32(arenaB.Obj(pOff)), Da = u32(arenaB.Obj(dOff)), Na = u32(arenaB.Obj(nOff));
        auto node = [&](u32 sel) -> u32 { return sel < 8 ? u32(arenaC.Obj(a[18] + 0x20 * sel)) : (sel == 8 ? Da : (sel == 9 ? Na : (mode ? Pa : u32(arenaC.Obj(a[18]))))); };
        if (mode != 0) { arenaB.Put32(pOff + 0, sP); arenaB.Put8(pOff + 4, a[11]); arenaB.Put32(pOff + 0x10, node(a[14])); arenaB.Put32(pOff + 0x14, node(a[15])); }
        arenaB.Put32(dOff + 0, sD); arenaB.Put32(dOff + 0xC, mode == 0 ? 0u : Pa);
        arenaB.Put8(nOff + 4, a[12]); arenaB.Put32(nOff + 0, sN); arenaB.Put32(nOff + 0x10, node(a[16])); arenaB.Put32(nOff + 0x14, node(a[17]));
        call.args[0] = Da; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kShut: {
        const u32 value = a[4] == 0 ? a[3] : (a[4] == 1 ? u32(arenaB.Obj(a[5])) : 0u);
        arenaA.Put32(o, value);
        break;
    }
    case kFill: {
        const u32 H = thisA;
        arenaA.Put32(o + 0x38, a[4] ? u32(arenaB.Obj(a[5])) : 0u);
        arenaA.Put32(o + 0x40, a[6]); arenaA.Put32(o + 0x44, a[7]);
        BuildFreeList(c, H);
        // bloque `desc` en C (a[8]); el siguiente bloque ocupado `nd` = desc + 0x10 + tam + (a[10] - 0x20)
        const u32 desc = u32(arenaC.Obj(a[8])), size = a[9], nd = desc + 0x10u + size + (a[10] - 0x20u);
        g.joinRet = u32(arenaD.Obj(a[18]));
        arenaD.Put32(a[18] + 0, a[11]);
        if (a[4]) {
            for (int idx = -1; idx < 64; ++idx) {
                const u32 ent = CsEntry(c, idx);
                const u32 h = Mix(a[12] + 0x9E3779B1u * u32(idx + 2));
                const u32 off = a[5] + u32(idx * 0x3C);
                arenaB.Put32(off + 0x10, h % 10 < 7 ? ent + 0x18 : u32(arenaC.Obj(0x40 + 0x20 * ((h >> 8) % 8))));
            }
        }
        call.args[0] = desc; call.args[1] = nd; call.args[2] = size; call.argCount = 3; call.popBytes = 0;
        break;
    }
    case kMalloc: {
        const u32 H = thisA;
        arenaA.Put32(o + 0x38, a[4] ? u32(arenaB.Obj(a[5])) : 0u);
        arenaA.Put32(o + 0x40, a[6]); arenaA.Put32(o + 0x44, a[7]);
        BuildFreeList(c, H);
        u32 blk = a[8];
        if (blk & 0xF) blk = (blk & ~0xFu) + 0x10u;
        const int target = int(blk >> 4) - 1;
        if (a[4]) {
            for (int idx = -1; idx < 64; ++idx) {
                const u32 ent = CsEntry(c, idx);
                const u32 off = a[5] + u32(idx * 0x3C);
                const u32 h = Mix(a[10] + 0x9E3779B1u * u32(idx + 2));
                const bool nonEmpty = a[9] == 3 || (a[9] == 1 && (h & 1)) || (a[9] == 2 && idx == target);
                if (!nonEmpty) { arenaB.Put32(off + 0x10, ent + 0x18); continue; }
                const u32 coff = a[11] + 0x20u * u32(idx + 1);
                const u32 node = u32(arenaC.Obj(coff)), other = u32(arenaC.Obj(a[11]));
                const u32 h2 = Mix(h + 1), h3 = Mix(h + 2);
                arenaB.Put32(off + 0x10, node);
                arenaC.Put32(coff + 0, h3); arenaC.Put8(coff + 4, h2 >> 20);
                arenaC.Put32(coff + 0x10, h2 % 100 < 60 ? ent + 0x18 : (h2 % 100 < 75 ? ent : (h2 % 100 < 85 ? node : other)));
                arenaC.Put32(coff + 0x14, h3 % 100 < 60 ? ent : (h3 % 100 < 75 ? ent + 0x18 : (h3 % 100 < 85 ? node : other)));
            }
        }
        call.args[0] = a[8]; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kInit:
        g.mallocRet = u32(arenaC.Obj(a[8] + a[9]));
        call.args[0] = u32(arenaB.Obj(a[4])); call.args[1] = a[5]; call.args[2] = a[6]; call.argCount = 3; call.popBytes = 0;
        break;
    case kUCopy: {
        const u32 first = u32(arenaB.Obj(a[5]));
        call.args[0] = first; call.args[1] = first + 4 * a[4]; call.args[2] = a[7] ? 0u : u32(arenaC.Obj(a[6] + a[10]));
        call.args[3] = a[8]; call.args[4] = a[9]; call.argCount = 5; call.popBytes = 20;
        break;
    }
    case kRealloc:
        if (a[4] >= 0x10) arenaA.Put32(o + 4, u32(arenaB.Obj(a[5])));
        arenaA.Put32(o + 0x14, a[6]); arenaA.Put32(o + 0x18, a[4]);
        call.args[0] = a[7]; call.args[1] = a[8]; call.argCount = 2; call.popBytes = 0;
        break;
    }
    return call;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data())), u32(uintptr_t(arenaD.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap(); arenaD.Snap();
        SnapRegions();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0;
        bool threw = false;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            if (kKind[fn] == kRealloc) {
                const RegT r = GateTry(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
                threw = r.threw;
                return r.r;
            }
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        obs["threw"] = threw;
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kPhoneCtor: case kClump: case kJoin: case kMalloc: case kUCopy: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case kSlot: case kPoint: obs["ret"] = eax & 0xFFu; break;                                                        // bool en AL
        default: obs["ret"] = nullptr; break;                                                                           // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN208A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN208A_CASES", buffer, sizeof(buffer)) > 0) {
        const unsigned v = unsigned(std::atoi(buffer));
        if (v >= kFamilies && v <= kCases && v % kFamilies == 0) return v;
    }
    return kCases;
}
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
    }
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    const unsigned nCases = CasesPerFunction();
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = nCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * nCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        gBenchThread = GetCurrentThreadId();
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < nCases; ++c) CaseRun(env, totals, f, c);
            Write({{"event", "function_summary"}, {"function", kFn[f]}, {"cases", nCases}, {"failures", totals.failures - before}, {"state_restored", env.WatchesIntact() && RegionsIntact()}});
        }
    }
    RestoreAllRegions();
    const bool entriesIntact = env.WatchesIntact() && RegionsIntact();
    bool stateRestored = entriesIntact;
    for (auto& r : env.collab) if (r) stateRestored &= r->Restore();
    for (auto& r : env.route) if (r) stateRestored &= r->Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
}

// Busca el hook registrado para una direccion del EXE (categoria y nombre). Hooks Simple (direccion del EXE).
inline bool FindHook(uintptr_t exe, std::string& cat, std::string& name) {
    bool found = false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items())
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
            }
    });
    return found;
}
// Colaborador: si la DLL tiene hook (apagado) se parchea tambien el cuerpo de la DLL; si no (dll = 0), la DLL llama a la direccion del EXE.
inline Pair MakeCollab(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name;
    if (FindHook(exe, cat, name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}});
        if (env.collab.back()->Registered()) { p.dll = Body(env.collab.back()->Body()); p.hooked = true; }
    }
    return p;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn208a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn208a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn208a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name;
        const bool found = FindHook(kEntries[f], cat, name);
        if (found) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en el EXE y en el cuerpo de la DLL; se EXIGE el hook
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Colaborador de RenderWare / CRT: con hook RH en la DLL si lo tiene (mismo falso en ambos), si no la DLL lo llama por la direccion del EXE
    auto Fopt = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE: el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {F(0x53FB70, &FkGetPad), F(0x5E4500, &FkSetPedState)};
    env.stubs[3] = {F(0x538900, &FkOpenFile), F(0x536F80, &FkLoadLine), F(0x5389D0, &FkCloseFile), F(0x55D220, &FkGetSurfaceId)};
    env.stubs[4] = env.stubs[3];
    env.stubs[6] = {Fopt(0x7ED2D0, &FkFind), Fopt(0x7EC9D0, &FkRead), Fopt(0x74A290, &FkClumpCreate), Fopt(0x807970, &FkFlRead), Fopt(0x807720, &FkFlDeinit), Fopt(0x74D190, &FkGeoRead), Fopt(0x74CCC0, &FkGeoDestroy),
                    Fopt(0x749C50, &FkAtomCreate), Fopt(0x74BF20, &FkSetFrame), Fopt(0x749D40, &FkSetGeom), Fopt(0x749DC0, &FkAtomDestroy), Fopt(0x74A490, &FkAddAtom), Fopt(0x74A310, &FkClumpDestroy),
                    Fopt(0x72F420, &FkMallocRw), Fopt(0x72F430, &FkFreeRw)};
    env.stubs[8] = {ByExeAddress(0x82413F, &FkFree)};
    env.stubs[9] = {F(0x72E870, &FkJoin)};
    env.stubs[10] = {F(0x72EC80, &FkFillIn)};
    env.stubs[11] = {F(0x72ED80, &FkHeapMalloc)};
    env.stubs[14] = {ByExeAddress(0x82119A, &FkNew), ByExeAddress(0x8214BD, &FkDelete)};
    // trampolines hacia las funciones originales (hooks de los colaboradores ya apagados): comprueban los bytes que copian
    {
        auto expect = [&](uintptr_t at, std::initializer_list<u8> bytes) { size_t i = 0; for (u8 b : bytes) if (*reinterpret_cast<u8*>(at + i++) != b) ready = false; };
        expect(0x82413F, {0x6A, 0x0C, 0x68, 0x38, 0x80, 0x88, 0x00}); expect(0x82119A, {0x56, 0x8B, 0x74, 0x24, 0x08}); expect(0x72F420, {0x8B, 0x44, 0x24, 0x04, 0x50});
        expect(0x72F430, {0xE9}); expect(0x8214BD, {0xE9});
        static Trampoline t1, t2, t3, t4, t5;
        gTrampFree = t1.Make(0x82413F, 7, 0x824146); gTrampNew = t2.Make(0x82119A, 5, 0x82119F); gTrampDelete = t3.Make(0, 0, 0x82413F);
        gTrampMalloc = t4.Make(0x72F420, 5, 0x72F425); gTrampFree2 = t5.Make(0, 0, 0x82413F);
    }
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {1u, 2u, 5u, 7u, 12u, 13u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[3] = dllReal[4] = "toda la funcion salvo los falsos de fichero y de GetSurfaceIdFromName; el sscanf corre real (el del EXE en el lado original, el de la DLL en el candidato)";
    dllReal[6] = "toda la funcion y los ayudantes privados de la DLL (0x72E080, 0x72E270, 0x72E360, que en el EXE corren reales) salvo los falsos de RenderWare y de CMemoryMgr";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada: el marcador de la prueba negativa (CMemoryMgr::Init) y, para la lectura de clump, los globales de las otras dos fases (deben quedar intactos: el estado vive en la pila)
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kMarker, 1); regionsOf[12] = {regions.size() - 1};
    regions.emplace_back(0xC87AE8, 0x20); regions.emplace_back(0x8D607C, 4); regions.emplace_back(0x8D6088, 4); regions.emplace_back(0xC87FE8, 4);
    regionsOf[6] = {regions.size() - 4, regions.size() - 3, regions.size() - 2, regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}

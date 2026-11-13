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

// Banco par a par Fn182A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 23 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos
// en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): constructor base de CTaskSimple 0x61A390 y de
// CTaskComplex 0x61A3B0 (estos dos corren REALES en el EXE con el hook apagado), CEntity::RegisterReference 0x571B70, CTask::operator new 0x61A5A0 y delete 0x61A5B0, el constructor de CTaskSimpleTriggerLookAt 0x634440 (otra funcion probada), IKChainManager_c::LookAt 0x618970,
// CVector::Normalise 0x59C910, CPointRoute::operator new 0x41B7C0, el constructor de CTaskComplexWalkRoundObject 0x655020 y los cuerpos de destructor 0x6550D0 y 0x655370 (todos con hook), y solo en la direccion del EXE
// (la DLL los llama por ella): free 0x8214BD y operator new 0x82119A del CRT. Los objetos COM de los buffers D3D son sinteticos (la ranura 2 de su vtable es un falso `stdcall` con efectos reentrantes opcionales sobre el
// buffer). Memoria global del juego tocada (restaurada tras cada lado): bytes del menu 0xBA6831 / 0xBA67A4 / 0xBA677B, estados de teclado 0xB733A8 / 0xB73138, del raton 0xB73419 / 0xB7342D y ZonesVisited 0xBA3730
// (100 bytes; ademas se lee la region 0xBA3600..0xBA41FF, cuyo CRC se anota en cada caso).
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria global, los eventos de los falsos (con sus
// argumentos), el valor devuelto (punteros como etiqueta de arena; EAX completo en la constante; solo AL en los bool) y el estado x87 (siempre vacio). En los constructores con vtable la DLL deja la suya (en el ASI): se
// comprueba que [this] sea una direccion del ASI (DLL) o la del EXE y se normaliza al valor del EXE antes de comparar (campo `vtable_ok`). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F182ALive {
using namespace F182AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F182AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u8 v) { m[kGuard + off] = v; }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos de cuerpo y de constructor
inline u32 gDtorWrite{};             // dword que el falso del cuerpo / constructor deja en [this]
inline u32 gTaskNew{};               // lo que devuelve CTask::operator new
inline u32 gNew{};                   // lo que devuelve operator new del CRT (0x82119A)
inline std::array<u32, 2> gRoute{};  // lo que devuelven las dos llamadas a CPointRoute::operator new
inline unsigned gRouteIdx{};
inline DWORD gBenchThread{};
inline u32 gBufThis{}, gOffSize{}, gOffStack{}, gMutKind{}, gMutAt{}, gMutSize{}, gMutArr2{}, gMutRaw{}, gRelCount{};   // buffers D3D: mutacion reentrante durante Release

inline u32 Crc32(const void* p, size_t n) {
    static const auto table = [] { std::array<u32, 256> t{}; for (u32 i = 0; i < 256; ++i) { u32 c = i; for (int k = 0; k < 8; ++k) c = (c & 1) ? 0xEDB88320u ^ (c >> 1) : c >> 1; t[i] = c; } return t; }();
    u32 c = 0xFFFFFFFFu;
    for (size_t i = 0; i < n; ++i) c = table[(c ^ static_cast<const u8*>(p)[i]) & 0xFF] ^ (c >> 8);
    return c ^ 0xFFFFFFFFu;
}

// free del CRT (0x8214BD; cdecl, un puntero que puede ser crudo y no se desreferencia): evento [direccion, argumento (etiqueta)]. No libera nada.
inline void __cdecl FkFree(void* p) {
    Ev& e = PushEv(kFreeAddr, 1);
    e.v[0] = Tag(p);
}
// operator new del CRT (0x82119A; cdecl, el tamano): evento [direccion, tamano]; devuelve gNew.
inline void* __cdecl FkNew(u32 size) {
    Ev& e = PushEv(kNewAddr, 1);
    e.v[0] = size;
    return reinterpret_cast<void*>(uintptr_t(gNew));
}
// CTask::operator delete (0x61A5B0; cdecl, un argumento valido): evento [direccion, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(0x61A5B0, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// Cuerpo del destructor / constructor base (thiscall, sin argumentos de pila): evento [direccion del EXE, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CEntity::RegisterReference (0x571B70; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]. No escribe nada.
inline void __fastcall FkRef(void* self, void*, void* ref) {
    Ev& e = PushEv(0x571B70, 2);
    e.v[0] = Tag(self);
    e.v[1] = Tag(ref);
}
// CTask::operator new (0x61A5A0; cdecl, el tamano): evento [direccion, tamano]; devuelve gTaskNew (un objeto de la arena B o nulo).
inline void* __cdecl FkTaskNew(u32 size) {
    Ev& e = PushEv(0x61A5A0, 1);
    e.v[0] = size;
    return reinterpret_cast<void*>(uintptr_t(gTaskNew));
}
// Constructor de CTaskSimpleTriggerLookAt (0x634440; thiscall, diez dwords de pila, `ret 28h`): evento [direccion, ECX (etiqueta), entidad (etiqueta), tiempo, hueso, x, y, z, torso (byte bajo), velocidad, mezcla, prioridad];
// deja gDtorWrite en [ECX]; devuelve gRet.
inline u32 __fastcall FkCtorTla(void* self, void*, u32 ent, u32 time, u32 bone, u32 x, u32 y, u32 z, u32 torso, u32 speed, u32 blend, u32 prio) {
    Ev& e = PushEv(0x634440, 11);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(ent)); e.v[2] = time; e.v[3] = bone; e.v[4] = x; e.v[5] = y; e.v[6] = z; e.v[7] = torso & 0xFFu; e.v[8] = speed; e.v[9] = blend; e.v[10] = prio;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor de CTaskComplexWalkRoundObject (0x655020; thiscall, tres dwords de pila, `ret 0Ch`): evento [direccion, ECX (etiqueta), estado, puntero al punto (etiqueta), objeto (etiqueta)]; deja gDtorWrite en [ECX]; devuelve gRet.
inline u32 __fastcall FkCtorWro(void* self, void*, u32 state, void* point, u32 object) {
    Ev& e = PushEv(0x655020, 4);
    e.v[0] = Tag(self); e.v[1] = state; e.v[2] = Tag(point); e.v[3] = Tag(uintptr_t(object));
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// IKChainManager_c::LookAt (0x618970; thiscall, once dwords de pila, `ret 2Ch`): evento [direccion, ECX (crudo), CRC32 del nombre, ped (etiqueta), entidad (etiqueta), tiempo, hueso, puntero al desplazamiento (etiqueta),
// torso (byte bajo), velocidad, mezcla, prioridad, ultimo].
inline u32 __fastcall FkLookAt(void* self, void*, const char* name, u32 ped, u32 ent, u32 time, u32 bone, void* off, u32 torso, u32 speed, u32 blend, u32 prio, u32 last) {
    Ev& e = PushEv(0x618970, 12);
    e.v[0] = Tag(self); e.v[1] = Crc32(name, strlen(name)); e.v[2] = Tag(uintptr_t(ped)); e.v[3] = Tag(uintptr_t(ent)); e.v[4] = time; e.v[5] = bone; e.v[6] = Tag(off);
    e.v[7] = torso & 0xFFu; e.v[8] = speed; e.v[9] = blend; e.v[10] = prio; e.v[11] = last;
    return 0;
}
// CVector::Normalise (0x59C910; thiscall, sin pila): evento [direccion, ECX (etiqueta), x, y, z]. No escribe nada.
inline void __fastcall FkNormalise(void* self, void*) {
    Ev& e = PushEv(0x59C910, 4);
    const u32* v = static_cast<const u32*>(self);
    e.v[0] = Tag(self); e.v[1] = v[0]; e.v[2] = v[1]; e.v[3] = v[2];
}
// CPointRoute::operator new (0x41B7C0; cdecl, el tamano): evento [direccion, tamano]; devuelve gRoute[0] y despues gRoute[1].
inline void* __cdecl FkRouteNew(u32 size) {
    Ev& e = PushEv(0x41B7C0, 1);
    e.v[0] = size;
    return reinterpret_cast<void*>(uintptr_t(gRoute[gRouteIdx++ & 1]));
}
// Release de un objeto COM sintetico (ranura 2 de su vtable; stdcall, un argumento): evento [kComRelease, objeto (etiqueta)]; tras el Release numero gMutAt aplica la mutacion reentrante sobre el buffer.
inline u32 __stdcall FkComRelease(void* self) {
    Ev& e = PushEv(kComRelease, 1);
    e.v[0] = Tag(self);
    if (gMutKind && gRelCount == gMutAt) {
        auto put = [](u32 off, u32 v) { memcpy(reinterpret_cast<void*>(uintptr_t(gBufThis) + off), &v, 4); };
        switch (gMutKind) {
        case 1: put(gOffSize, gMutSize); break;
        case 2: put(gOffSize, gMutSize | 0x80000000u); break;
        case 3: put(gOffStack, gMutArr2); break;
        case 4: put(gOffStack, gMutArr2); put(gOffSize, 8); break;
        case 5: put(gOffStack, 0); put(gOffSize, 0); break;
        default: put(gOffStack, gMutRaw); put(gOffSize, 0); break;
        }
    }
    ++gRelCount;
    return 1;
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
    std::array<u32, 16> args{};
    u32 argCount{}, popBytes{}, ecx{}, eax{};
};

inline void PutRaw(uintptr_t at, const void* data, size_t n) { memcpy(reinterpret_cast<void*>(at), data, n); }

// Pone un objeto de tarea de la prueba en la arena A (campos comunes de CTaskSimpleTriggerLookAt)
inline void PutTla(u32 o, const std::array<u32, 40>& a, u32 ent, bool withExists) {
    arenaA.Put32(o + 8, ent);
    arenaA.Put32(o + 0xC, a[7]); arenaA.Put32(o + 0x10, a[8]); arenaA.Put32(o + 0x14, a[9]); arenaA.Put32(o + 0x18, a[10]); arenaA.Put32(o + 0x1C, a[11]);
    arenaA.Put8(o + 0x20, u8(a[12])); arenaA.Put32(o + 0x24, a[13]); arenaA.Put32(o + 0x28, a[14]);
    if (withExists) { arenaA.Put8(o + 0x2C, u8(a[15])); arenaA.Put8(o + 0x2D, u8(a[16])); }
    else arenaA.Put8(o + 0x2D, u8(a[15]));
}
inline u32 EntityOf(const std::array<u32, 40>& a, u32 at) { return a[at] == 0 ? 0u : (a[at] == 1 ? u32(arenaB.Obj(a[at + 1])) : a[at + 2]); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx; call.eax = c.eax;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    gRet = 0; gDtorWrite = 0; gTaskNew = 0; gNew = 0; gRoute = {}; gRouteIdx = 0; gMutKind = 0; gRelCount = 0;
    const u32 o = a[0];
    const u32 self = u32(arenaA.Obj(o));
    const auto kind = kKind[c.fn];
    if (kind != kGrowFree && kind != kGrowResize) arenaA.Put32(o, a[3]);
    switch (kind) {
    case kMenu: {
        const u8 b0 = u8(a[4]), b1 = u8(a[5]), b2 = u8(a[6]);
        PutRaw(kMenuMain, &b0, 1); PutRaw(kMenuActive, &b1, 1); PutRaw(kMenuNext, &b2, 1);
        break;
    }
    case kRel: {
        call.ecx = self;
        const u32 ptr = a[4] == 0 ? 0u : (a[4] == 1 ? u32(arenaB.Obj(a[5])) : a[6]);
        arenaA.Put32(o + 4, ptr); arenaA.Put32(o + 8, a[7]); arenaA.Put32(o + 0xC, a[8]);
        break;
    }
    case kZone:
        PutRaw(kZonesVisited, &a[6], 100);
        call.args = {a[4], a[5]}; call.argCount = 2; call.popBytes = 8;
        break;
    case kEsc: {
        const u16 n = u16(a[4]), old = u16(a[5]);
        PutRaw(kEscNew, &n, 2); PutRaw(kEscOld, &old, 2);
        break;
    }
    case kMouseR: {
        const u8 n = u8(a[4]), old = u8(a[5]);
        PutRaw(kMouseNew, &n, 1); PutRaw(kMouseOld, &old, 1);
        break;
    }
    case kGarIn:
    case kGarOut: {
        call.ecx = self;
        u8 name[33]{};
        for (u32 i = 0; i < 32; ++i) name[i] = u8(a[6 + i / 4] >> (8 * (i % 4)));
        const u32 len = a[5];
        auto& src = kind == kGarIn ? arenaB : arenaA;
        const u32 so = kind == kGarIn ? a[4] : o;
        for (u32 i = 0; i < len; ++i) src.Put8(so + 0x44 + i, name[i]);
        src.Put8(so + 0x44 + len, 0);
        call.args[0] = u32(arenaB.Obj(a[4])); call.argCount = 1;
        break;
    }
    case kTlaCtor:
        call.ecx = self;
        gRet = a[16]; gDtorWrite = a[17];
        call.args = {EntityOf(a, 4), a[7], a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15]}; call.argCount = 10;
        break;
    case kConst:
        call.ecx = self;
        break;
    case kTlaClone:
        call.ecx = self;
        PutTla(o, a, EntityOf(a, 4), false);
        gTaskNew = a[16] == 0 ? 0u : u32(arenaB.Obj(a[17]));
        gRet = a[18]; gDtorWrite = a[19];
        break;
    case kRetTrue:
        call.ecx = self;
        call.args = {a[4], a[5], a[6]}; call.argCount = 3;
        break;
    case kTlaProc:
        call.ecx = self;
        PutTla(o, a, EntityOf(a, 4), true);
        call.args[0] = a[17]; call.argCount = 1;
        break;
    case kBldCtor: {
        call.ecx = self;
        for (u32 i = 0; i < 3; ++i) { arenaC.Put32(a[11] + 4 * i, a[8 + i]); arenaB.Put32(a[15] + 4 * i, a[12 + i]); arenaD.Put32(a[19] + 4 * i, a[16 + i]); }
        gRoute[0] = a[23] == 0 ? 0u : u32(arenaC.Obj(a[24]));
        gRoute[1] = a[25] == 0 ? 0u : u32(arenaD.Obj(a[26]));
        gRet = a[21]; gDtorWrite = a[22];
        call.args = {a[4], EntityOf(a, 5), u32(arenaC.Obj(a[11])), u32(arenaB.Obj(a[15])), u32(arenaD.Obj(a[19])), a[20]}; call.argCount = 6;
        break;
    }
    case kWroClone:
        call.ecx = self;
        arenaA.Put32(o + 0xC, a[4]); arenaA.Put32(o + 0x10, a[5]); arenaA.Put32(o + 0x14, a[6]); arenaA.Put32(o + 0x18, a[7]); arenaA.Put32(o + 0x1C, a[8]);
        gTaskNew = a[9] == 0 ? 0u : u32(arenaB.Obj(a[10]));
        gRet = a[11]; gDtorWrite = a[12];
        break;
    case kSdd:
        call.ecx = self;
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kBufTex:
    case kBufIdx: {
        call.ecx = self;
        const bool tex = kind == kBufTex;
        const u32 offCount = tex ? 0x10u : 0xCu, offSize = tex ? 0x14u : 0x10u, offStack = tex ? 0x18u : 0x14u;
        const u32 arr1 = u32(arenaB.Obj(a[7])), arr2 = u32(arenaB.Obj(a[7] + 0x40));
        const u32 stack = a[6] == 0 ? 0u : (a[6] == 1 ? arr1 : a[13]);
        arenaA.Put32(o + offCount, a[4]); arenaA.Put32(o + offSize, a[5]); arenaA.Put32(o + offStack, stack);
        for (u32 j = 0; j < 8; ++j) { arenaB.Put32(a[7] + 4 * j, u32(arenaC.Obj(a[8] + 8 * j))); arenaB.Put32(a[7] + 0x40 + 4 * j, u32(arenaC.Obj(a[8] + 8 * (8 + j)))); }
        for (u32 j = 0; j < 16; ++j) arenaC.Put32(a[8] + 8 * j, u32(arenaD.Obj(a[9])));
        arenaD.Put32(a[9] + 8, u32(reinterpret_cast<uintptr_t>(&FkComRelease)));
        gBufThis = self; gOffSize = offSize; gOffStack = offStack;
        gMutKind = a[10]; gMutAt = a[11]; gMutSize = a[12]; gMutArr2 = arr2; gMutRaw = a[13];
        break;
    }
    case kGrowFree: {
        call.ecx = self;
        const u32 data = a[6] == 0 ? 0u : (a[6] == 1 ? u32(arenaB.Obj(a[7])) : a[8]);
        arenaA.Put32(o, a[4]); arenaA.Put32(o + 4, a[5]); arenaA.Put32(o + 8, data);
        break;
    }
    case kGrowResize: {
        call.ecx = self;
        const u32 data = a[6] == 0 ? 0u : (a[6] == 1 ? u32(arenaB.Obj(a[7])) : a[11]);
        arenaA.Put32(o, a[4]); arenaA.Put32(o + 4, a[5]); arenaA.Put32(o + 8, data);
        gNew = a[10] == 0 ? 0u : u32(arenaC.Obj(a[9]));
        call.args[0] = a[8]; call.argCount = 1;
        break;
    }
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
        const u32 zoneCrc = kKind[fn] == kZone ? Crc32(reinterpret_cast<const void*>(uintptr_t(kZoneRegion)), kZoneRegionSize) : 0u;   // estado de la region que lee la funcion justo antes de la llamada
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, call.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        if (fn == 8 || fn == 13) {   // la vtable de la DLL (en el ASI) se normaliza a la del EXE
            const u32 expVt = fn == 8 ? 0x86E3CCu : 0x86F334u;
            const u32 vt = arenaA.Get32(c.a[0]);
            const bool ok = candidate ? (vt != 0 && ModuleOf(vt) == env.asi) : vt == expVt;
            obs["vtable_ok"] = ok;
            if (ok) arenaA.Put32(c.a[0], expVt);
        }
        if (kKind[fn] == kZone) obs["zone_crc"] = zoneCrc;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kTlaCtor: case kBldCtor: case kSdd: case kTlaClone: case kWroClone: obs["ret"] = Tag(uintptr_t(eax)); break;   // this / el objeto nuevo como etiqueta de arena
        case kConst: obs["ret"] = eax; break;                                                                               // valor completo de EAX
        case kZone: case kEsc: case kMouseR: case kRetTrue: case kTlaProc: obs["ret"] = eax & 0xFFu; break;                  // bool: solo AL
        default: obs["ret"] = nullptr; break;                                                                               // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN182A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN182A_CASES", buffer, sizeof(buffer)) > 0) {
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
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}, {"zone_region", HexBytes(reinterpret_cast<const void*>(uintptr_t(kZoneRegion)), kZoneRegionSize)}});
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

// Busca el hook registrado para una direccion del EXE (categoria y nombre). Hooks Simple (direccion del EXE) y Virtual (direccion de la funcion del EXE que ocupa la ranura): de un Virtual devuelve ademas
// el cuerpo de la DLL (la entrada de su vtabla), que HookRoute usa como candidato. Un Virtual lleva dentro un Simple sobre la misma direccion (Virtual::Switch).
inline bool FindHook(uintptr_t exe, std::string& cat, std::string& name, uintptr_t* virtualBody = nullptr) {
    bool found = false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items()) {
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
            } else if (virtualBody && h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Virtual
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
                *virtualBody = reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookOurAddress());
            }
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
    if (!std::filesystem::exists(dir / "fn182a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn182a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn182a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name; uintptr_t vbody = 0;
        const bool found = FindHook(kEntries[f], cat, name, &vbody);
        if (found) {
            if (vbody) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], vbody));
            else env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        }
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}, {"virtual", vbody != 0}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en el EXE y en el cuerpo de la DLL
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (free 0x8214BD, operator new 0x82119A): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    env.stubs[1] = {ByExeAddress(kFreeAddr, &FkFree)};
    env.stubs[2] = {ByExeAddress(kFreeAddr, &FkFree)};
    // Constructores base (CTaskSimple 0x61A390 y CTaskComplex 0x61A3B0): colaboradores REALES con el hook apagado (el EXE corre sus bytes; la DLL escribe los mismos campos en linea)
    auto R = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return it->second;
    };
    env.reals[8] = {R(0x61A390)};
    env.reals[13] = {R(0x61A3B0)};
    env.stubs[8] = {F(0x571B70, &FkRef)};
    env.stubs[10] = {F(0x61A5A0, &FkTaskNew), Rt(8, &FkCtorTla)};
    env.stubs[12] = {F(0x618970, &FkLookAt)};
    env.stubs[13] = {F(0x59C910, &FkNormalise), F(0x41B7C0, &FkRouteNew), F(0x571B70, &FkRef)};
    env.stubs[14] = {F(0x61A5A0, &FkTaskNew), F(0x655020, &FkCtorWro)};
    env.stubs[15] = {F(0x6550D0, &FkBody<0x6550D0>), F(0x61A5B0, &FkDelete)};
    env.stubs[16] = {F(0x655370, &FkBody<0x655370>), F(0x61A5B0, &FkDelete)};
    env.stubs[18] = {ByExeAddress(kFreeAddr, &FkFree)};
    env.stubs[20] = {ByExeAddress(kFreeAddr, &FkFree)};
    env.stubs[21] = {ByExeAddress(kFreeAddr, &FkFree)};
    env.stubs[22] = {ByExeAddress(kNewAddr, &FkNew), ByExeAddress(kFreeAddr, &FkFree)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL los llama por ella)";
    for (unsigned f : {0u, 3u, 4u, 5u, 6u, 7u, 9u, 11u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    for (unsigned f : {17u, 19u}) dllReal[f] = "toda la funcion (sin colaboradores; los Release son un falso COM sintetico en la vtable de la arena)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        for (const auto& pr : env.reals[f]) {
            env.watches.emplace_back(pr.exe, 16);
            real.push_back({{"exe", pr.exe}, {"dll", pr.dll != 0}, {"hooked", pr.hooked}, {"dll_addr", pr.dll}, {"dll_module", ModuleOf(pr.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada; se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    auto reg = [&](u32 at, size_t n, std::initializer_list<unsigned> fns) { regions.emplace_back(at, n); for (unsigned f : fns) regionsOf[f].push_back(regions.size() - 1); };
    reg(kMenuMain, 1, {0}); reg(kMenuActive, 1, {0}); reg(kMenuNext, 1, {0});
    reg(kEscNew, 2, {4}); reg(kEscOld, 2, {4});
    reg(kMouseNew, 1, {5}); reg(kMouseOld, 1, {5});
    reg(kZonesVisited, 100, {3});
    Run(env, ready, registered, bodies, collaborators);
}
}

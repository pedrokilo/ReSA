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

// Banco par a par Fn199A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 30 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall / RH_ScopedOverloadedInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion
// del EXE con objetos sinteticos en cuatro arenas propias (A de 0x3000 bytes, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair):
// CTask::operator new 0x61A5A0, CEventPool::New 0x4B5570, constructores 0x620780, 0x6786C0, 0x61A390, 0x620E30, 0x4AEAC0, 0x4AECD0, 0x4AEDF0, 0x4AF010, 0x4AF130 y 0x4AF240 (con hook, 0x620B00 incluido desde el lote 198A), CPed::GetWeaponSkill 0x5E3B60, CWeaponInfo::GetWeaponInfo 0x743C60, cuerpo de CPed 0x5E8620, CMatrix::Destructor 0x59ACD0, CWorld::ProcessLineOfSight 0x56BA00, CVector::Normalise
// 0x59C910, SetDefaults 0x517330, CIdleCam::Init 0x50E6D0, IKChainManager_c 0x6181A0 y 0x6181D0, CTaskManager::FindActiveTaskByType 0x681740, el cuerpo de CCamera 0x50A870 (otra funcion probada: `Rt`) y operator
// delete del CRT 0x8214BD (sin hook; la DLL lo llama por esa direccion). Reales (hook apagado: el EXE corre sus bytes y la DLL su cuerpo o codigo en linea): CPed::IsAlive 0x5E0170, ~CPlaceable 0x54F490 (con hook desde 198A) y
// _ftol 0x821B40 (sin hook). Memoria global del juego tocada (restaurada tras cada lado): el puntero del pool de peatones 0xB74490, los punteros de los dos jugadores 0xB7CD98 y 0xB7CF28, el byte 0x8CCB80 y el contador
// de matrices 0xB74238. Las vtables de las tareas sinteticas son propias (ranuras 4, 6 y 9 con falsos; las demas anotan una llamada inesperada).
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los bytes cambiados de la memoria global, los eventos de los falsos
// (con sus argumentos), el valor devuelto (punteros como etiqueta de arena; EAX completo en constantes; solo AL en bool; bits del float de ST0 en GetLocalSoundLevel) y el estado x87 (siempre vacio). EAX/ECX/EDX
// tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F199ALive {
using namespace F199AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F199AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWinA> arenaA;
inline Arena<kWin> arenaB, arenaC, arenaD;
inline u32 AddrOf(unsigned arena, size_t off) {
    switch (arena) { case 0: return u32(arenaA.Obj(off)); case 1: return u32(arenaB.Obj(off)); case 2: return u32(arenaC.Obj(off)); default: return u32(arenaD.Obj(off)); }
}

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos de cuerpo y de constructor
inline bool gRetSelf{};              // true: los falsos de constructor devuelven `this` (ECX) en vez de gRet
inline u32 gDtorWrite{};             // dword que el falso del cuerpo / constructor deja en [this]
inline u32 gNew{};                   // memoria que devuelve el falso de operator new / New (0 = sin memoria)
inline u32 gSubType{}, gSubAbort{}, gRewriteValue{};   // GetTaskType de la subtarea, EAX de su MakeAbortable, dword que reescribe en [this+8]
inline u32* gRewriteAt{};            // direccion (en la arena A) de [this+8] que reescribe el GetTaskType de la subtarea (nulo: no escribe)
inline u32 gSkill{}, gInfo{};        // EAX de GetWeaponSkill y puntero que devuelve GetWeaponInfo
inline std::array<u32, 2> gLos{};    // EAX de la primera y la segunda ProcessLineOfSight
inline unsigned losCalls{};
inline u32 gLookEax{}, gLookEnt{};   // EAX de IsLooking y de GetLookAtEntity
inline u32 gMgr0{}, gMgr1{}, gTaskP{}, gTaskS{}, gTP{}, gTS{};   // gestores de tareas del ped y del atractivo, sus tareas y lo que devuelve su GetTaskType
inline std::array<u32, 3> gFindP{}, gFindS{};                    // FindActiveTaskByType por tipo (4B2, 4BF, 4B3)
inline DWORD gBenchThread{};

inline u32 CtorRet(void* self) { return gRetSelf ? u32(uintptr_t(self)) : gRet; }

// operator delete / liberacion (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// operator new (cdecl, un tamano): evento [direccion, tamano]; devuelve gNew.
template<u32 ADDR> inline u32 __cdecl FkNew(u32 size) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = size;
    return gNew;
}
// CEventPool::New (0x4B5570; thiscall, sin argumentos; ECX = el pool [0xB744AC]): evento [direccion, ECX == [0xB744AC]]; devuelve gNew.
inline u32 __fastcall FkNewPool(void* self, void*) {
    Ev& e = PushEv(0x4B5570, 1);
    e.v[0] = u32(uintptr_t(self)) == *reinterpret_cast<const u32*>(kEventPoolPtrAddr) ? 1u : 0u;
    return gNew;
}
// Cuerpo de destructor / constructor sin argumentos de pila (thiscall): evento [direccion del EXE, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructores con n dwords de pila (thiscall, `ret 4n`): evento [direccion, ECX (etiqueta), argumentos (crudos)]; dejan gDtorWrite en [ECX]; devuelven gRet (o `this` con gRetSelf).
template<u32 ADDR> inline u32 __fastcall FkCtor1(void* self, void*, u32 a) {
    Ev& e = PushEv(ADDR, 2); e.v[0] = Tag(self); e.v[1] = a;
    memcpy(self, &gDtorWrite, 4); return CtorRet(self);
}
template<u32 ADDR> inline u32 __fastcall FkCtor2(void* self, void*, u32 a, u32 b) {
    Ev& e = PushEv(ADDR, 3); e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b;
    memcpy(self, &gDtorWrite, 4); return CtorRet(self);
}
template<u32 ADDR> inline u32 __fastcall FkCtor3(void* self, void*, u32 a, u32 b, u32 c) {
    Ev& e = PushEv(ADDR, 4); e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c;
    memcpy(self, &gDtorWrite, 4); return CtorRet(self);
}
template<u32 ADDR> inline u32 __fastcall FkCtor4(void* self, void*, u32 a, u32 b, u32 c, u32 d) {
    Ev& e = PushEv(ADDR, 5); e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c; e.v[4] = d;
    memcpy(self, &gDtorWrite, 4); return CtorRet(self);
}
template<u32 ADDR> inline u32 __fastcall FkCtor5(void* self, void*, u32 a, u32 b, u32 c, u32 d, u32 f) {
    Ev& e = PushEv(ADDR, 6); e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c; e.v[4] = d; e.v[5] = f;
    memcpy(self, &gDtorWrite, 4); return CtorRet(self);
}
template<u32 ADDR> inline u32 __fastcall FkCtor6(void* self, void*, u32 a, u32 b, u32 c, u32 d, u32 f, u32 h) {
    Ev& e = PushEv(ADDR, 7); e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c; e.v[4] = d; e.v[5] = f; e.v[6] = h;
    memcpy(self, &gDtorWrite, 4); return CtorRet(self);
}
// Igual que FkCtor6 pero el ultimo argumento es un int8 (CTaskComplexKillPedOnFoot::Constructor 0x620E30: el constructor solo lee el byte bajo con movsx): se anota el byte bajo con signo ampliado, que es lo que
// empuja el EXE (movsx); la DLL empuja el int8 sin normalizar los bits altos (el llamador de un parametro de 8 bits no esta obligado a ampliarlo).
template<u32 ADDR> inline u32 __fastcall FkCtor6S(void* self, void*, u32 a, u32 b, u32 c, u32 d, u32 f, u32 h) {
    Ev& e = PushEv(ADDR, 7); e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c; e.v[4] = d; e.v[5] = f; e.v[6] = u32(int32(int8(h & 0xFFu)));
    memcpy(self, &gDtorWrite, 4); return CtorRet(self);
}
// CPed::GetWeaponSkill (0x5E3B60; thiscall, un argumento): evento [direccion, ECX (etiqueta), tipo]; devuelve gSkill.
inline u32 __fastcall FkSkill(void* self, void*, u32 type) {
    Ev& e = PushEv(0x5E3B60, 2); e.v[0] = Tag(self); e.v[1] = type;
    return gSkill;
}
// CWeaponInfo::GetWeaponInfo (0x743C60; cdecl, dos argumentos): evento [direccion, tipo, habilidad]; devuelve gInfo.
inline u32 __cdecl FkInfo(u32 type, u32 skill) {
    Ev& e = PushEv(0x743C60, 2); e.v[0] = type; e.v[1] = skill;
    return gInfo;
}
// CWorld::ProcessLineOfSight (0x56BA00; cdecl, 12 argumentos): evento [direccion, origen x, y, z (bits), destino x, y, z, ocho banderas (dwords)]; devuelve gLos[llamada] (el llamador solo mira AL).
inline u32 __cdecl FkLos(const u32* origin, const u32* target, void* /*colPoint*/, void* /*entity*/, u32 f1, u32 f2, u32 f3, u32 f4, u32 f5, u32 f6, u32 f7, u32 f8) {
    Ev& e = PushEv(0x56BA00, 14);
    e.v[0] = origin[0]; e.v[1] = origin[1]; e.v[2] = origin[2]; e.v[3] = target[0]; e.v[4] = target[1]; e.v[5] = target[2];
    e.v[6] = f1; e.v[7] = f2; e.v[8] = f3; e.v[9] = f4; e.v[10] = f5; e.v[11] = f6; e.v[12] = f7; e.v[13] = f8;
    return gLos[losCalls++ == 0 ? 0 : 1];
}
// IKChainManager_c::IsLooking (0x6181A0) y GetLookAtEntity (0x6181D0): thiscall, un argumento; evento [direccion, ECX (crudo: el objeto global), ped (etiqueta)].
inline u32 __fastcall FkIsLooking(void* self, void*, u32 ped) {
    Ev& e = PushEv(0x6181A0, 2); e.v[0] = u32(uintptr_t(self)); e.v[1] = Tag(uintptr_t(ped));
    return gLookEax;
}
inline u32 __fastcall FkLookAt(void* self, void*, u32 ped) {
    Ev& e = PushEv(0x6181D0, 2); e.v[0] = u32(uintptr_t(self)); e.v[1] = Tag(uintptr_t(ped));
    return gLookEnt;
}
// CTaskManager::FindActiveTaskByType (0x681740; thiscall, un argumento): evento [direccion, ECX (etiqueta), tipo]; devuelve la tarea sintetica de la tabla del gestor (ped o atractivo) o 0.
inline u32 __fastcall FkFind(void* self, void*, u32 type) {
    Ev& e = PushEv(0x681740, 2); e.v[0] = Tag(self); e.v[1] = type;
    const u32 idx = type == 0x4B2 ? 0u : (type == 0x4BF ? 1u : (type == 0x4B3 ? 2u : 3u));
    if (idx > 2) return 0;
    if (u32(uintptr_t(self)) == gMgr0) return gFindP[idx] ? gTaskP : 0u;
    if (u32(uintptr_t(self)) == gMgr1) return gFindS[idx] ? gTaskS : 0u;
    return 0;
}
// CVector::Normalise (0x59C910; thiscall, sin argumentos): evento [direccion, x, y, z (bits)]; multiplica cada componente por 0.5f (sustituye a la normalizacion con una operacion exacta y observable).
inline void __fastcall FkNorm(void* self, void*) {
    Ev& e = PushEv(0x59C910, 3);
    float v[3]; memcpy(v, self, 12);
    for (unsigned i = 0; i < 3; ++i) { e.v[i] = BitsOf(&v[i]); v[i] *= 0.5f; }
    memcpy(self, v, 12);
}
// vtables propias de las tareas sinteticas: ranuras 4 (GetTaskType), 6 (MakeAbortable) y 9 (CreateFirstSubTask) con falsos; las demas anotan una llamada inesperada [90 + k, ECX]
inline std::array<uintptr_t, 16> vtGeneric{}, vtTaskP{}, vtTaskS{};
template<unsigned K> inline u32 __fastcall FkVt(void* self, void*) { Ev& e = PushEv(90 + K, 1); e.v[0] = Tag(self); return 0; }
inline u32 __fastcall FkGetType(void* self, void*) {   // ranura 4 de la subtarea / de this: evento [94, ECX]; puede reescribir [this+8]
    Ev& e = PushEv(94, 1); e.v[0] = Tag(self);
    if (gRewriteAt) *gRewriteAt = gRewriteValue;
    return gSubType;
}
inline u32 __fastcall FkAbortSub(void* self, void*, u32 ped, u32 prio, u32 evt) {   // ranura 6: evento [96, ECX, ped, prioridad, evento]
    Ev& e = PushEv(96, 4); e.v[0] = Tag(self); e.v[1] = ped; e.v[2] = prio; e.v[3] = evt;
    return gSubAbort;
}
inline u32 __fastcall FkCreateFirst(void* self, void*, u32 ped) {   // ranura 9: evento [99, ECX, ped]
    Ev& e = PushEv(99, 2); e.v[0] = Tag(self); e.v[1] = ped;
    return gRet;
}
inline u32 __fastcall FkTaskTypeP(void* self, void*) { Ev& e = PushEv(95, 1); e.v[0] = Tag(self); return gTP; }
inline u32 __fastcall FkTaskTypeS(void* self, void*) { Ev& e = PushEv(95, 1); e.v[0] = Tag(self); return gTS; }
template<unsigned... I> inline void FillVt(std::array<uintptr_t, 16>& v, std::integer_sequence<unsigned, I...>) { ((v[I] = uintptr_t(&FkVt<I>)), ...); }
inline void InitTaskVt() {
    FillVt(vtGeneric, std::make_integer_sequence<unsigned, 16>{}); FillVt(vtTaskP, std::make_integer_sequence<unsigned, 16>{}); FillVt(vtTaskS, std::make_integer_sequence<unsigned, 16>{});
    vtGeneric[4] = uintptr_t(&FkGetType); vtGeneric[6] = uintptr_t(&FkAbortSub); vtGeneric[9] = uintptr_t(&FkCreateFirst);
    vtTaskP[4] = uintptr_t(&FkTaskTypeP); vtTaskS[4] = uintptr_t(&FkTaskTypeS);
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
    bool floatRet{};
};

// Rellena los estados de un pool falso (bytes) en una arena
template<class AR> inline void PutStates(AR& ar, u32 off, u32 cap, u32 seed) { for (u32 i = 0; i < cap; ++i) ar.Put8(off + i, SlotByte(seed, i)); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[44]); arenaB.Pattern(a[2], a[45]); arenaC.Pattern(a[2], a[46]); arenaD.Pattern(a[2], a[47]);
    gRet = 0; gRetSelf = false; gDtorWrite = 0; gNew = 0; gSubType = 0; gSubAbort = 0; gRewriteValue = 0; gRewriteAt = nullptr; gSkill = 0; gInfo = 0; gLos = {}; losCalls = 0; gLookEax = 0; gLookEnt = 0;
    gMgr0 = gMgr1 = gTaskP = gTaskS = gTP = gTS = 0; gFindP = {}; gFindS = {};
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    call.ecx = thisA;
    const auto newMem = [&](u32 sel, u32 off) { return sel == 0 ? 0u : u32(arenaB.Obj(off)); };
    switch (c.fn) {
    case kRaAbort: {
        arenaA.Put32(o, a[3]);
        arenaA.Put32(o + 8, u32(arenaB.Obj(a[4])));
        arenaB.Put32(a[4], u32(uintptr_t(vtGeneric.data())));
        arenaA.Put8(o + 0xD, a[5]);
        gSubAbort = a[9];
        call.args[0] = a[7]; call.args[1] = a[6]; call.args[2] = a[8]; call.argCount = 3;
        break;
    }
    case kRaCreate: {
        arenaA.Put32(o, a[3]);
        arenaA.Put32(o + 0x14, a[5] == 0 ? 0u : u32(arenaB.Obj(a[6])));
        if (a[5]) arenaB.Put8(a[6] + 0x36, a[7]);
        arenaA.Put32(o + 0x18, a[8]); arenaA.Put32(o + 0x1C, a[9]); arenaA.Put32(o + 0x20, a[10]);
        gNew = a[11] == 0 ? 0u : u32(arenaD.Obj(a[12]));
        gRet = a[13]; gDtorWrite = a[14];
        call.args[0] = a[4]; call.argCount = 1;
        break;
    }
    case kRaControl: {
        arenaA.Put32(o, u32(uintptr_t(vtGeneric.data())));
        arenaA.Put32(o + 8, u32(arenaB.Obj(a[5])));
        arenaB.Put32(a[5], u32(uintptr_t(vtGeneric.data())));
        arenaA.Put8(o + 0xC, a[4]);
        gRet = a[7];
        if (a[8]) { gRewriteAt = reinterpret_cast<u32*>(arenaA.Obj(o + 8)); gRewriteValue = a[9]; }
        call.args[0] = a[6]; call.argCount = 1;
        break;
    }
    case kRaClone:
        arenaA.Put32(o, a[3]);
        for (u32 i = 0; i < 5; ++i) arenaA.Put32(o + 0x10 + 4 * i, a[8 + i]);
        gNew = newMem(a[4], a[5]); gRet = a[6]; gDtorWrite = a[7];
        break;
    case kKpClone:
        arenaA.Put32(o, a[3]);
        arenaA.Put32(o + 0x10, a[6]); arenaA.Put32(o + 0x14, a[7]); arenaA.Put32(o + 0x18, a[8]); arenaA.Put32(o + 0x1C, a[9]); arenaA.Put32(o + 0x20, a[10]); arenaA.Put32(o + 0x28, a[11]);
        arenaA.Put8(o + 0xC, a[12]);
        gNew = u32(arenaB.Obj(a[4])); gRetSelf = true; gDtorWrite = a[5];
        break;
    case kVhrType: case kVhrLife: case kVhrSound: case kVhrPrio:
        arenaA.Put32(o, a[3]);
        break;
    case kVhrAffects:
        arenaA.Put32(o, a[3]);
        arenaB.Put32(a[4] + kPedStateOff, a[5]);
        call.args[0] = u32(arenaB.Obj(a[4])); call.argCount = 1;
        break;
    case kPemvAffects:
        arenaA.Put32(o, a[3]);
        arenaA.Put32(o + 0x14, a[4]); arenaA.Put32(o + 0x18, a[5]);
        arenaB.Put32(a[6] + kPedStateOff, a[7]); arenaB.Put32(a[6] + kPedVehOff, a[8]); arenaB.Put32(a[6] + kPedFlagsOff, a[9]);
        call.args[0] = u32(arenaB.Obj(a[6])); call.argCount = 1;
        break;
    case kPemvClone:
        arenaA.Put32(o, a[3]);
        arenaA.Put32(o + 0x14, a[8]); arenaA.Put32(o + 0x18, a[9]); arenaA.Put32(o + 0x1C, a[10]);
        gNew = newMem(a[4], a[5]); gRet = a[6]; gDtorWrite = a[7];
        break;
    case kCpAffects:
        arenaA.Put32(o, a[3]);
        arenaA.Put32(o + 0x10, a[4]);
        arenaB.Put32(a[5] + kPedStateOff, a[6]);
        call.args[0] = u32(arenaB.Obj(a[5])); call.argCount = 1;
        break;
    case kCpClone:
        arenaA.Put32(o, a[3]);
        arenaA.Put8(o + 0xC, a[8]); arenaA.Put32(o + 0x10, a[9]);
        gNew = newMem(a[4], a[5]); gRet = a[6]; gDtorWrite = a[7];
        break;
    case kSpAffects: {
        arenaA.Put32(o, a[3]);
        const u32 sexy = a[4] == 0 ? 0u : u32(arenaD.Obj(a[5]));
        arenaA.Put32(o + 0x14, sexy);
        arenaB.Put32(a[6] + kPedStateOff, a[7]); arenaB.Put32(a[6] + kPedIntelOff, u32(arenaC.Obj(a[8])));
        arenaD.Put32(a[5] + kPedStateOff, a[9]); arenaD.Put32(a[5] + kPedIntelOff, u32(arenaC.Obj(a[10])));
        arenaC.Put32(a[11], u32(uintptr_t(vtTaskP.data()))); arenaC.Put32(a[12], u32(uintptr_t(vtTaskS.data())));
        gMgr0 = u32(arenaC.Obj(a[8])) + 4; gMgr1 = u32(arenaC.Obj(a[10])) + 4;
        gTaskP = u32(arenaC.Obj(a[11])); gTaskS = u32(arenaC.Obj(a[12])); gTP = a[22]; gTS = a[23];
        for (u32 i = 0; i < 3; ++i) { gFindP[i] = a[16 + i]; gFindS[i] = a[19 + i]; }
        gLookEax = a[13]; gLookEnt = a[15] ? sexy : a[14];
        call.args[0] = u32(arenaB.Obj(a[6])); call.argCount = 1;
        break;
    }
    case kSpClone:
        arenaA.Put32(o, a[3]);
        arenaA.Put32(o + 0x14, a[8]);
        gNew = newMem(a[4], a[5]); gRet = a[6]; gDtorWrite = a[7];
        break;
    case kSvAffects:
        arenaA.Put32(o, a[3]);
        arenaA.Put32(o + 0xC, a[4]);
        arenaB.Put32(a[5] + kPedStateOff, a[6]);
        call.args[0] = u32(arenaB.Obj(a[5])); call.argCount = 1;
        break;
    case kSvClone: case kPtcClone: case kPtfClone:
        arenaA.Put32(o, a[3]);
        arenaA.Put32(o + 0xC, a[8]);
        gNew = newMem(a[4], a[5]); gRet = a[6]; gDtorWrite = a[7];
        break;
    case kPoolCreate: {
        arenaA.Put32(o, a[9]); arenaA.Put32(o + 4, u32(arenaB.Obj(a[4]))); arenaA.Put32(o + 8, a[5]); arenaA.Put32(o + 0xC, a[6]);
        PutStates(arenaB, a[4], a[5], a[10]);
        arenaB.Put8(a[4] + a[7], SlotByte(a[10], a[7]) | 0x80u);   // la ranura a crear esta libre (la plantilla de la DLL lo comprueba)
        {   // y hay otro hueco libre (ancla) para que la busqueda del cursor termine dentro del arreglo
            u32 anchor = Rand(a[10], 0x3E7) % a[5];
            if (anchor == a[7]) anchor = (a[7] + 1) % a[5];
            arenaB.Put8(a[4] + anchor, SlotByte(a[10], anchor) | 0x80u);
        }
        call.args[0] = (a[7] << 8) | (a[8] & 0xFFu); call.argCount = 1;
        break;
    }
    case kPedDelete: {
        arenaA.Put32(o, a[9]); arenaA.Put32(o + 4, u32(arenaB.Obj(a[4]))); arenaA.Put32(o + 8, a[5]); arenaA.Put32(o + 0xC, a[6]);
        PutStates(arenaB, a[4], a[5], a[10]);
        arenaB.Put8(a[4] + a[7], SlotByte(a[10], a[7]) & 0x7Fu);   // la ranura a liberar esta ocupada (la plantilla de la DLL lo comprueba)
        *reinterpret_cast<u32*>(kPedPoolPtrAddr) = u32(arenaA.Obj(o));
        call.args[0] = a[9] + a[7] * kPedDeleteStride + a[8]; call.args[1] = a[11]; call.argCount = 2; call.popBytes = 8; call.ecx = c.ecx;
        break;
    }
    case kPedCanRun: {
        arenaA.Put8(o + kPedSlotOff, a[4]);
        const int slot = int(int8(a[4]));
        arenaA.Put32(size_t(int64(o) + kPedWeaponsOff + int64(slot) * kWeaponStride), a[5]);
        gSkill = a[6]; gInfo = u32(arenaC.Obj(a[7]));
        arenaC.Put32(a[7] + 0x18, a[8]);
        break;
    }
    case kPedSdd: {
        const u32 idx = a[9], rem = a[10];
        arenaA.Put32(o, a[3]);
        arenaC.Put32(a[5], thisA - idx * kPedDeleteStride - rem); arenaC.Put32(a[5] + 4, u32(arenaD.Obj(a[6]))); arenaC.Put32(a[5] + 8, a[7]); arenaC.Put32(a[5] + 0xC, a[8]);
        PutStates(arenaD, a[6], a[7], a[12]);
        arenaD.Put8(a[6] + idx, SlotByte(a[12], idx) & 0x7Fu);
        *reinterpret_cast<u32*>(kPedPoolPtrAddr) = u32(arenaC.Obj(a[5]));
        gDtorWrite = a[11];
        call.args[0] = a[4]; call.argCount = 1;
        break;
    }
    case kCamDtor:
        arenaA.Put32(o, a[3]); arenaA.Put32(o + 0x14, a[4]);
        gDtorWrite = a[5];
        break;
    case kCamSdd:
        arenaA.Put32(o, a[3]); arenaA.Put32(o + 0x14, a[6]);
        gDtorWrite = a[5];
        call.args[0] = a[4]; call.argCount = 1;
        break;
    case kCamVis: {
        const u32 ped0 = u32(arenaB.Obj(a[4])), ped1 = u32(arenaB.Obj(a[5])), m0 = u32(arenaB.Obj(a[6])), m1 = u32(arenaB.Obj(a[7]));
        for (u32 i = 0; i < 3; ++i) {   // la posicion que lee la funcion (la de la matriz si el ped la tiene, si no la del ped) lleva el valor del caso; la otra, basura
            arenaB.Put32(a[4] + 4 + 4 * i, a[8] ? (a[10 + i] ^ 0x5A5A5A5Au) : a[10 + i]);  arenaB.Put32(a[6] + 0x30 + 4 * i, a[8] ? a[10 + i] : (a[10 + i] ^ 0x5A5A5A5Au));
            arenaB.Put32(a[5] + 4 + 4 * i, a[9] ? (a[13 + i] ^ 0x5A5A5A5Au) : a[13 + i]);  arenaB.Put32(a[7] + 0x30 + 4 * i, a[9] ? a[13 + i] : (a[13 + i] ^ 0x5A5A5A5Au));
        }
        arenaB.Put32(a[4] + 0x14, a[8] ? m0 : 0u); arenaB.Put32(a[5] + 0x14, a[9] ? m1 : 0u);
        *reinterpret_cast<u32*>(kPlayer0Addr) = ped0; *reinterpret_cast<u32*>(kPlayer1Addr) = ped1;
        *reinterpret_cast<uint8*>(kLosByteAddr) = uint8(a[21]);
        gLos = {a[19], a[20]};
        call.args[0] = a[16]; call.args[1] = a[17]; call.args[2] = a[18]; call.argCount = 3;
        break;
    }
    case kCamGeo: {
        const u32 ped0 = u32(arenaB.Obj(a[6])), ped1 = u32(arenaB.Obj(a[7])), m0 = u32(arenaB.Obj(a[11])), m1 = u32(arenaB.Obj(a[12]));
        arenaA.Put32(o + 0xAC, a[4]);
        for (u32 i = 0; i < 3; ++i) {
            arenaB.Put32(a[6] + 4 + 4 * i, a[13] ? (a[15 + i] ^ 0x5A5A5A5Au) : a[15 + i]);  arenaB.Put32(a[11] + 0x30 + 4 * i, a[13] ? a[15 + i] : (a[15 + i] ^ 0x5A5A5A5Au));
            arenaB.Put32(a[7] + 4 + 4 * i, a[14] ? (a[18 + i] ^ 0x5A5A5A5Au) : a[18 + i]);  arenaB.Put32(a[12] + 0x30 + 4 * i, a[14] ? a[18 + i] : (a[18 + i] ^ 0x5A5A5A5Au));
        }
        arenaB.Put32(a[6] + 0x14, a[13] ? m0 : 0u); arenaB.Put32(a[7] + 0x14, a[14] ? m1 : 0u);
        *reinterpret_cast<u32*>(kPlayer0Addr) = ped0; *reinterpret_cast<u32*>(kPlayer1Addr) = ped1;
        call.args[0] = a[5]; call.args[1] = u32(arenaC.Obj(a[8])); call.args[2] = u32(arenaC.Obj(a[9])); call.args[3] = u32(arenaD.Obj(a[10])); call.argCount = 4;
        break;
    }
    case kHsCtor: case kIcCtor:
        arenaA.Put32(o, a[3]);
        gDtorWrite = ~a[3];
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
        std::vector<std::vector<uint8>> before;                       // memoria global de la funcion tras la preparacion (para gchanges)
        for (const auto i : regionsOf[fn]) before.push_back(ReversingProbe::Bytes(regions[i].Ptr(), regions[i].bytes));
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, kRet[fn] == kRetFloat);
        });
        const u32 floatBits = gFloatRet;
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        json g = json::array();
        {
            size_t k = 0;
            for (const auto i : regionsOf[fn]) {
                const auto& r = regions[i]; const auto now = ReversingProbe::Bytes(r.Ptr(), r.bytes);
                for (size_t b = 0; b < r.bytes; ++b) if (now[b] != before[k][b]) g.push_back(json::array({uint64(r.at + b), now[b]}));
                ++k;
            }
        }
        obs["gchanges"] = g;
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kRet[fn]) {
        case kRetPtr: obs["ret"] = Tag(uintptr_t(eax)); break;      // puntero como etiqueta de arena
        case kRetEax: obs["ret"] = eax; break;                      // valor completo de EAX
        case kRetFloat: obs["ret"] = floatBits; break;              // bits del float de ST0
        case kRetAl: obs["ret"] = eax & 0xFFu; break;               // bool: solo AL
        default: obs["ret"] = nullptr; break;                       // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN199A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN199A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn199a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn199a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn199a"); }
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
    // Colaborador REAL con hook RH en la DLL (apagado durante el banco): el EXE corre sus bytes y la DLL su cuerpo (o su codigo en linea)
    auto R = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return it->second;
    };
    // Colaborador REAL sin hook RH (el EXE corre sus bytes; la DLL tiene su propio codigo o llama a la misma direccion)
    auto N = [&](uintptr_t exe) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return Pair{exe, 0, false};
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE: un solo parche, en el EXE.
    auto E = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    InitTaskVt();
    const auto newTask = [&] { return F(0x61A5A0, &FkNew<0x61A5A0>); };
    const auto newPool = [&] { return F(0x4B5570, &FkNewPool); };
    // fn 0 (MakeAbortable) y fn 2 (ControlSubTask): solo ranuras de vtable sinteticas (sin colaboradores con direccion)
    env.stubs[kRaCreate] = {newTask(), F(0x620780, &FkCtor4<0x620780>), F(0x6786C0, &FkCtor3<0x6786C0>), F(0x61A390, &FkBody<0x61A390>)};
    env.reals[kRaCreate] = {N(0x821B40)};   // _ftol del CRT del EXE (sin hook; la DLL convierte en linea)
    env.stubs[kRaClone] = {newTask(), F(0x620B00, &FkCtor5<0x620B00>)};
    env.stubs[kKpClone] = {newTask(), F(0x620E30, &FkCtor6S<0x620E30>)};
    for (unsigned f : {unsigned(kVhrAffects), unsigned(kPemvAffects), unsigned(kCpAffects), unsigned(kSpAffects), unsigned(kSvAffects)}) env.reals[f] = {R(0x5E0170)};   // CPed::IsAlive: el EXE corre sus bytes; la DLL lo hace en linea
    env.stubs[kPemvClone] = {newPool(), F(0x4AEAC0, &FkCtor3<0x4AEAC0>)};
    env.stubs[kCpClone] = {newPool(), F(0x4AECD0, &FkCtor2<0x4AECD0>)};
    env.stubs[kSpAffects] = {F(0x6181A0, &FkIsLooking), F(0x6181D0, &FkLookAt), F(0x681740, &FkFind)};
    env.stubs[kSpClone] = {newPool(), F(0x4AEDF0, &FkCtor1<0x4AEDF0>)};
    env.stubs[kSvClone] = {newPool(), F(0x4AF010, &FkCtor1<0x4AF010>)};
    env.stubs[kPtcClone] = {newPool(), F(0x4AF130, &FkCtor1<0x4AF130>)};
    env.stubs[kPtfClone] = {newPool(), F(0x4AF240, &FkCtor1<0x4AF240>)};
    env.stubs[kPedCanRun] = {F(0x5E3B60, &FkSkill), F(0x743C60, &FkInfo)};
    env.stubs[kPedSdd] = {F(0x5E8620, &FkBody<0x5E8620>)};
    env.stubs[kCamDtor] = {F(0x59ACD0, &FkBody<0x59ACD0>)};
    env.reals[kCamDtor] = {R(0x54F490)};   // ~CPlaceable (CPlaceable::DestructorBody, con hook RH desde el lote 198A, apagado): el EXE corre sus bytes; la DLL su cuerpo o su codigo en linea
    env.stubs[kCamSdd] = {Rt(kCamDtor, &FkBody<0x50A870>), E(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[kCamVis] = {F(0x56BA00, &FkLos)};
    env.stubs[kCamGeo] = {F(0x59C910, &FkNorm)};
    env.stubs[kHsCtor] = {F(0x517330, &FkBody<0x517330>)};
    env.stubs[kIcCtor] = {F(0x50E6D0, &FkBody<0x50E6D0>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {unsigned(kRaAbort), unsigned(kRaControl), unsigned(kVhrType), unsigned(kVhrLife), unsigned(kVhrSound), unsigned(kVhrPrio), unsigned(kPoolCreate), unsigned(kPedDelete)}) dllReal[f] = "toda la funcion (sin colaboradores con direccion)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        for (const auto& r : env.reals[f]) {
            env.watches.emplace_back(r.exe, 16);
            real.push_back({{"exe", r.exe}, {"dll", r.dll != 0}, {"hooked", r.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada: se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kPedPoolPtrAddr, 4);   // 0
    regions.emplace_back(kPlayer0Addr, 4);      // 1
    regions.emplace_back(kPlayer1Addr, 4);      // 2
    regions.emplace_back(kLosByteAddr, 1);      // 3
    regions.emplace_back(kNumMatricesAddr, 4);  // 4
    regionsOf[kPedDelete] = {0}; regionsOf[kPedSdd] = {0};
    regionsOf[kCamDtor] = {4}; regionsOf[kCamSdd] = {4};
    regionsOf[kCamVis] = {1, 2, 3}; regionsOf[kCamGeo] = {1, 2};
    Run(env, ready, registered, bodies, collaborators);
}
}

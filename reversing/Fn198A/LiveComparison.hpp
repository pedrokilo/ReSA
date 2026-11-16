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

// Banco par a par Fn198A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 31 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos
// en cuatro arenas propias (A, B, C y D con guardas de 0x40; C mide 0x4000). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair) para los que tienen hook RH:
// cuerpos de destructor 0x632630, 0x632720, 0x6389F0, 0x6593A0, 0x668630 y 0x659240 (funcion 15 de este lote), CTask::operator delete 0x61A5B0 y operator new 0x61A5A0, ~CTaskComplex 0x61A3D0 y constructor 0x61A3B0, CPed::IsAlive 0x5E0170,
// CEntityScanner::ScanForEntitiesInRange 0x5FFA20 (V), CEntity::CleanUpOldReference 0x571A00 y RegisterReference 0x571B70, CBaseModelInfo::Get2dEffect 0x4C4C70, CTaskManager::GetActiveTask 0x681720, CScripted2dEffects::GetIndex 0x6F9F60,
// CMatrix::SetScale 0x59AED0, CPedAttractorManager::IsApproachable 0x5EA220, CPedScriptedTaskRecord::GetStatus 0x608750, CEventHandlerHistory::GetCurrentEvent 0x4B8CA0, ComputeResponseTaskOfType 0x4B5730,
// FindRespectedFriendInInformRange 0x600CF0, CGeneral 0x53CBE0 y 0x53CB50, constructores 0x61ABE0, 0x667E20, 0x62F310, 0x48E750, 0x63B8C0 y 0x667CD0, CVector 0x40FE60 y 0x40FDB0, CPed::Say 0x5EFFE0, CStreaming 0x409C90,
// y las funciones del propio lote 0x6591D0, 0x6592A0, 0x65A110 y 0x668250 (ruta propia con el mismo falso); solo en la direccion del EXE (la DLL la llama por ella): 0x5FFFD0. Las llamadas virtuales (GetTaskType, GetEventType, el
// calculador de GetSeekPos) usan una vtable falsa al final de la arena D. Memoria global del juego tocada (restaurada tras cada lado): CModelInfo::ms_modelInfoPtrs (indices -3..7), puntero del pool de peatones 0xB74490,
// CScripted2dEffects::ms_userLists (8 entradas) y el paso de tiempo 0xB7CB5C.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria global, los eventos de los falsos (con sus
// argumentos), el valor devuelto (punteros como etiqueta de arena; EAX completo en constantes; solo AL en bool; bits del float de ST0 en ComputeHeading) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en
// original_residue / candidate_residue (informativos).

namespace F198ALive {
using namespace F198AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F198AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = u8(v); }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaD;
inline Arena<kWinC> arenaC;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos de cuerpo y de CreateSubTask
inline u32 gDtorWrite{};             // dword que el falso del cuerpo / constructor deja en [objeto]
inline u32 gTaskNew{};               // lo que devuelve CTask::operator new
inline u32 gThis{}, gRefOff{};       // objeto de la prueba y desplazamiento de su campo de referencia (los falsos de referencia anotan [this] y [this + gRefOff])
inline u32 gAlive{}, gStatus{}, gR3{}, gR4{};
inline u32 gHeading{}, gAtan{}, gLimit{}, gDot{};
inline u32 gMask{}, gCall{}, gIndex{}, gEffBase{};
inline std::array<u32, 3> gVec{};
inline DWORD gBenchThread{};

// Marcas de los eventos de las llamadas virtuales (no son direcciones del EXE)
inline constexpr u32 kEvType = 0x50000001u, kEvCalc = 0x50000002u, kEvEvent = 0x50000003u, kEvTrap = 0x5000FFFFu;

// Cuerpo del destructor / constructor base (thiscall, sin argumentos de pila): evento [direccion, ECX (etiqueta), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// operator delete (cdecl, un argumento): evento [direccion, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// CEntity::CleanUpOldReference (0x571A00) / RegisterReference (0x571B70) (thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta), [this], [this + gRefOff]]. No escribe nada.
template<u32 ADDR> inline void __fastcall FkRef(void* self, void*, void* ref) {
    Ev& e = PushEv(ADDR, 4);
    e.v[0] = Tag(self);
    e.v[1] = Tag(ref);
    e.v[2] = BitsOf(reinterpret_cast<const void*>(uintptr_t(gThis)));
    e.v[3] = BitsOf(reinterpret_cast<const void*>(uintptr_t(gThis) + gRefOff));
}
// CPed::IsAlive (0x5E0170; thiscall, sin pila): evento [direccion, ECX (etiqueta)]; devuelve gAlive (el llamador solo mira AL).
inline u32 __fastcall FkIsAlive(void* self, void*) {
    Ev& e = PushEv(0x5E0170, 1);
    e.v[0] = Tag(self);
    return gAlive;
}
// CEntityScanner::ScanForEntitiesInRange (0x5FFA20, V; thiscall, lista y ped, `ret 8`): evento [direccion, ECX (etiqueta), lista, ped (etiqueta)].
inline void __fastcall FkScanEnt(void* self, void*, u32 list, void* ped) {
    Ev& e = PushEv(0x5FFA20, 3);
    e.v[0] = Tag(self); e.v[1] = list; e.v[2] = Tag(ped);
}
// CBaseModelInfo::Get2dEffect (0x4C4C70; thiscall, un indice, `ret 4`): evento [direccion, ECX (etiqueta), indice]; devuelve el efecto indice del bloque de efectos de la arena D.
inline u32 __fastcall FkGet2d(void* self, void*, u32 index) {
    Ev& e = PushEv(0x4C4C70, 2);
    e.v[0] = Tag(self); e.v[1] = index;
    return gEffBase + 0x40u * index;
}
// 0x5FFFD0 (thiscall, efecto, entidad y ped, `ret 0Ch`; la DLL la llama por la direccion del EXE): evento [direccion, ECX (etiqueta), efecto, entidad, ped (etiquetas)].
inline void __fastcall FkAttr(void* self, void*, void* effect, void* entity, void* ped) {
    Ev& e = PushEv(0x5FFFD0, 4);
    e.v[0] = Tag(self); e.v[1] = Tag(effect); e.v[2] = Tag(entity); e.v[3] = Tag(ped);
}
// CTaskManager::GetActiveTask (0x681720; thiscall, sin pila): evento [direccion, ECX (etiqueta)]; devuelve el dword de [ECX + 0x10] (la tarea activa del banco).
inline u32 __fastcall FkActiveTask(void* self, void*) {
    Ev& e = PushEv(0x681720, 1);
    e.v[0] = Tag(self);
    return BitsOf(reinterpret_cast<const u8*>(self) + 0x10);
}
// CScripted2dEffects::GetIndex (0x6F9F60; cdecl, el efecto): evento [direccion, efecto (etiqueta)]; devuelve gIndex.
inline u32 __cdecl FkGetIndex(void* effect) {
    Ev& e = PushEv(0x6F9F60, 1);
    e.v[0] = Tag(effect);
    return gIndex;
}
// CMatrix::SetScale(float) (0x59AED0; thiscall, un float, `ret 4`): evento [direccion, ECX (etiqueta), bits].
inline void __fastcall FkSetScale(void* self, void*, u32 bits) {
    Ev& e = PushEv(0x59AED0, 2);
    e.v[0] = Tag(self); e.v[1] = bits;
}
// CPedAttractorManager::IsApproachable (0x5EA220; cdecl, cuatro argumentos): evento [direccion, efecto (etiqueta), matriz (etiqueta), tercero, ped (etiqueta)]; devuelve un bit de gMask por llamada.
inline u32 __cdecl FkApproach(void* effect, void* matrix, u32 third, void* ped) {
    Ev& e = PushEv(0x5EA220, 4);
    e.v[0] = Tag(effect); e.v[1] = Tag(matrix); e.v[2] = third; e.v[3] = Tag(ped);
    return (gMask >> (gCall++ & 31u)) & 1u;
}
// CPedScriptedTaskRecord::GetStatus (0x608750; cdecl, el ped): evento [direccion, ped (etiqueta)]; devuelve gStatus (dword completo).
inline u32 __cdecl FkStatus(void* ped) {
    Ev& e = PushEv(0x608750, 1);
    e.v[0] = Tag(ped);
    return gStatus;
}
// CEventHandlerHistory::GetCurrentEvent (0x4B8CA0; thiscall, sin pila): evento [direccion, ECX (etiqueta)]; devuelve el dword de [ECX + 8].
inline u32 __fastcall FkCurEvent(void* self, void*) {
    Ev& e = PushEv(0x4B8CA0, 1);
    e.v[0] = Tag(self);
    return BitsOf(reinterpret_cast<const u8*>(self) + 8);
}
// CEventEditableResponse::ComputeResponseTaskOfType (0x4B5730; thiscall, ped y tipo, `ret 8`): evento [direccion, ECX (etiqueta), ped (etiqueta), tipo]; devuelve gR3.
inline u32 __fastcall FkResp(void* self, void*, void* ped, u32 type) {
    Ev& e = PushEv(0x4B5730, 3);
    e.v[0] = Tag(self); e.v[1] = Tag(ped); e.v[2] = type;
    return gR3;
}
// CPedIntelligence::FindRespectedFriendInInformRange (0x600CF0; thiscall, sin pila): evento [direccion, ECX (etiqueta)]; devuelve gR4.
inline u32 __fastcall FkFriend(void* self, void*) {
    Ev& e = PushEv(0x600CF0, 1);
    e.v[0] = Tag(self);
    return gR4;
}
// Llamadas virtuales (vtable falsa de la arena D): GetTaskType (ranura 4), calculador (ranura 0) y GetEventType (ranura 1); devuelven un dword de [ECX + 0x30].
inline u32 __fastcall FkVirtType(void* self, void*) {
    Ev& e = PushEv(kEvType, 1);
    e.v[0] = Tag(self);
    return BitsOf(reinterpret_cast<const u8*>(self) + 0x30);
}
inline u32 __fastcall FkVirtEvent(void* self, void*) {
    Ev& e = PushEv(kEvEvent, 1);
    e.v[0] = Tag(self);
    return BitsOf(reinterpret_cast<const u8*>(self) + 0x30);
}
inline u32 __fastcall FkTrap(void* self, void*) {
    Ev& e = PushEv(kEvTrap, 1);
    e.v[0] = Tag(self);
    return 0;
}
// Calculador de GetSeekPos (ranura 0; thiscall, ped, entidad y salida, `ret 0Ch`): evento [marca, ECX (etiqueta), ped, entidad, salida (etiquetas)]; escribe gVec en la salida.
inline void __fastcall FkCalc(void* self, void*, void* ped, void* entity, u32* out) {
    Ev& e = PushEv(kEvCalc, 4);
    e.v[0] = Tag(self); e.v[1] = Tag(ped); e.v[2] = Tag(entity); e.v[3] = Tag(out);
    out[0] = gVec[0]; out[1] = gVec[1]; out[2] = gVec[2];
}
// CGeneral::GetRadianAngleBetweenPoints (0x53CBE0; cdecl, cuatro floats; resultado float en ST0) y LimitRadianAngle (0x53CB50; cdecl, un float; ST0)
inline float __cdecl FkAtan(u32 x1, u32 y1, u32 x2, u32 y2) {
    Ev& e = PushEv(0x53CBE0, 4);
    e.v[0] = x1; e.v[1] = y1; e.v[2] = x2; e.v[3] = y2;
    return std::bit_cast<float>(gAtan);
}
inline float __cdecl FkLimit(u32 angle) {
    Ev& e = PushEv(0x53CB50, 1);
    e.v[0] = angle;
    return std::bit_cast<float>(gLimit);
}
// CTask::operator new (0x61A5A0; cdecl, el tamano): evento [direccion, tamano]; devuelve gTaskNew (un objeto de la arena B o nulo).
inline void* __cdecl FkTaskNew(u32 size) {
    Ev& e = PushEv(0x61A5A0, 1);
    e.v[0] = size;
    return reinterpret_cast<void*>(uintptr_t(gTaskNew));
}
// Constructores: eventos [direccion, ECX (etiqueta), argumentos (los punteros como etiquetas)]; dejan gDtorWrite en [ECX] y devuelven this (como el constructor real).
inline u32 __fastcall FkPresCtor(void* self, void*, void* cop) {
    Ev& e = PushEv(0x6591D0, 3);
    e.v[0] = Tag(self); e.v[1] = Tag(cop); e.v[2] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return u32(uintptr_t(self));
}
inline u32 __fastcall FkRunTimed(void* self, void*, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8) {
    Ev& e = PushEv(0x61ABE0, 9);
    e.v[0] = Tag(self);
    const u32 v[8]{a1, a2, a3, a4, a5, a6, a7, a8};
    for (u32 i = 0; i < 8; ++i) e.v[1 + i] = v[i];
    memcpy(self, &gDtorWrite, 4);
    return u32(uintptr_t(self));
}
inline u32 __fastcall FkAchieve(void* self, void*, u32 a, u32 b, u32 c) {
    Ev& e = PushEv(0x667E20, 4);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c;
    memcpy(self, &gDtorWrite, 4);
    return u32(uintptr_t(self));
}
inline u32 __fastcall FkStand(void* self, void*, u32 a, u32 b, u32 c, u32 d) {
    Ev& e = PushEv(0x62F310, 5);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c; e.v[4] = d;
    memcpy(self, &gDtorWrite, 4);
    return u32(uintptr_t(self));
}
inline u32 __fastcall FkPause(void* self, void*, u32 a) {
    Ev& e = PushEv(0x48E750, 2);
    e.v[0] = Tag(self); e.v[1] = a;
    memcpy(self, &gDtorWrite, 4);
    return u32(uintptr_t(self));
}
inline u32 __fastcall FkLeave(void* self, void*, u32 a, u32 b, u32 c, u32 d, u32 e5) {
    Ev& e = PushEv(0x63B8C0, 6);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(a)); e.v[2] = b; e.v[3] = c; e.v[4] = d; e.v[5] = e5;
    memcpy(self, &gDtorWrite, 4);
    return u32(uintptr_t(self));
}
inline u32 __fastcall FkGoToCtor(void* self, void*, u32 a, u32 b, u32 c, u32 d, u32 e5) {
    Ev& e = PushEv(0x667CD0, 6);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = Tag(uintptr_t(b)); e.v[3] = c; e.v[4] = d; e.v[5] = e5;
    memcpy(self, &gDtorWrite, 4);
    return u32(uintptr_t(self));
}
// CVector::VectorSubtract (0x40FE60; cdecl, salida y dos operandos; devuelve la salida) y DotProduct (0x40FDB0; cdecl, dos punteros; float en ST0)
inline u32* __cdecl FkVecSub(u32* out, u32* a, u32* b) {
    Ev& e = PushEv(0x40FE60, 3);
    e.v[0] = Tag(out); e.v[1] = Tag(a); e.v[2] = Tag(b);
    out[0] = gVec[0]; out[1] = gVec[1]; out[2] = gVec[2];
    return out;
}
inline float __cdecl FkDot(u32* a, u32* b) {
    Ev& e = PushEv(0x40FDB0, 2);
    e.v[0] = Tag(a); e.v[1] = Tag(b);
    return std::bit_cast<float>(gDot);
}
// CTaskComplexGoToPointAndStandStill::SelectMoveState (0x668250; thiscall, tarea, ped y dos floats, `ret 10h`)
// El 0x668250 del EXE solo usa ECX y EAX: conserva EDX, y CreateSubTask (0x6682D0) devuelve en EAX el EDX que guardo antes de la llamada (la tarea): el falso lo restaura al salir.
inline void __fastcall FkSelect(void* self, void* edxIn, void* task, void* ped, u32 r1, u32 r2) {
    Ev& e = PushEv(0x668250, 5);
    e.v[0] = Tag(self); e.v[1] = Tag(task); e.v[2] = Tag(ped); e.v[3] = r1; e.v[4] = r2;
    __asm { mov edx, edxIn }
}
// CPed::Say (0x5EFFE0; thiscall, seis dwords de pila, `ret 18h`)
inline void __fastcall FkSay(void* self, void*, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5) {
    Ev& e = PushEv(0x5EFFE0, 7);
    e.v[0] = Tag(self); e.v[1] = a0; e.v[2] = a1; e.v[3] = a2; e.v[4] = a3; e.v[5] = a4; e.v[6] = a5;
}
// CTaskComplexPresentIDToCop::CreateSubTask (0x65A110; thiscall, tipo y ped, `ret 8`): evento [direccion, ECX (etiqueta), tipo, ped (etiqueta)]; devuelve gRet.
inline u32 __fastcall FkCreateSub(void* self, void*, u32 type, void* ped) {
    Ev& e = PushEv(0x65A110, 3);
    e.v[0] = Tag(self); e.v[1] = type; e.v[2] = Tag(ped);
    return gRet;
}
// CTaskComplexPresentIDToCop::ComputeHeading (0x6592A0; thiscall, ped, `ret 4`; float en ST0)
inline float __fastcall FkHeading(void* self, void*, void* ped) {
    Ev& e = PushEv(0x6592A0, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(ped);
    return std::bit_cast<float>(gHeading);
}
// CStreaming::SetMissionDoesntRequireModel (0x409C90; cdecl, un dword)
inline void __cdecl FkMission(u32 id) {
    Ev& e = PushEv(0x409C90, 1);
    e.v[0] = id;
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
    u32 argCount{}, popBytes{}, ecx{};
    bool floatRet{};
};

inline void PutRaw(uintptr_t at, const void* data, size_t n) { memcpy(reinterpret_cast<void*>(at), data, n); }
inline void PutRaw32(uintptr_t at, u32 v) { PutRaw(at, &v, 4); }
inline u32 VtblPtr() { return u32(arenaD.Obj(kVtbl)); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[kSeedAt]); arenaB.Pattern(a[2], a[kSeedAt + 1]); arenaC.Pattern(a[2], a[kSeedAt + 2]); arenaD.Pattern(a[2], a[kSeedAt + 3]);
    gRet = gDtorWrite = gTaskNew = gAlive = gStatus = gR3 = gR4 = gHeading = gAtan = gLimit = gDot = gMask = gCall = gIndex = gEffBase = 0;
    gVec = {};
    const u32 o = a[0];
    const u32 self = u32(arenaA.Obj(o));
    gThis = self; gRefOff = 0;
    arenaA.Put32(o, a[3]);
    call.ecx = self;
    // vtable falsa: GetTaskType (ranura 4), calculador (ranura 0), GetEventType (ranura 1); las demas anotan un evento de alarma
    for (u32 i = 0; i < 12; ++i) arenaD.Put32(kVtbl + 4 * i, u32(uintptr_t(&FkTrap)));
    arenaD.Put32(kVtbl + 0, u32(uintptr_t(&FkCalc))); arenaD.Put32(kVtbl + 4, u32(uintptr_t(&FkVirtEvent))); arenaD.Put32(kVtbl + 0x10, u32(uintptr_t(&FkVirtType)));
    const u32 vt = VtblPtr();
    switch (kKind[c.fn]) {
    case kSdd:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kConst:
        break;
    case kScanVeh:
    case kScanObj: {
        arenaC.Put32(a[4] + 0x58C, a[5]); arenaC.Put32(a[4] + 0x46C, a[6]); arenaC.Put8(a[4] + 0x484, a[7]);
        if (kKind[c.fn] == kScanVeh) arenaA.Put32(o + 0x4C, a[8]);
        gAlive = a[9]; gRefOff = 0x4C;
        call.args[0] = u32(arenaC.Obj(a[4])); call.argCount = 1;
        break;
    }
    case kEntList: {
        arenaB.Put32(a[4], a[7] ? u32(arenaB.Obj(a[5])) : 0u);
        for (u32 i = 0; i < a[7]; ++i) {
            arenaB.Put32(a[5] + 8 * i, u32(arenaC.Obj(a[6] + kEntStride * i)));
            arenaB.Put32(a[5] + 8 * i + 4, i + 1 < a[7] ? u32(arenaB.Obj(a[5] + 8 * (i + 1))) : 0u);
        }
        for (u32 e = 0; e < 5; ++e) {
            const u32 ent = a[6] + kEntStride * e;
            arenaC.Put8(ent + 0x36, a[8 + 4 * e]); arenaC.Put32(ent + 0x1C, a[9 + 4 * e]); arenaC.Put32(ent + 0x140, a[10 + 4 * e]); arenaC.Put16(ent + 0x22, a[11 + 4 * e]);
        }
        for (u32 k = 0; k < kModelSlots; ++k) {
            PutRaw32(kModelTable + 4u * u32(int(k) + kModelLo), u32(arenaD.Obj(0x40 * k)));
            arenaD.Put8(0x40 * k + 0xD, a[40 + k]);
        }
        gEffBase = u32(arenaD.Obj(0x400));
        for (u32 i = 0; i < 6; ++i) { arenaD.Put8(0x400 + 0x40 * i + 0xC, a[52 + i]); arenaD.Put8(0x400 + 0x40 * i + 0x37, a[58 + i]); }
        call.args[0] = u32(arenaB.Obj(a[4])); call.args[1] = a[28]; call.argCount = 2;
        break;
    }
    case kClosest: {
        arenaB.Put32(a[4], u32(arenaC.Obj(a[5]))); arenaB.Put32(a[4] + 4, u32(arenaD.Obj(a[6]))); arenaB.Put32(a[4] + 8, a[7]);
        PutRaw32(kPedPool, u32(arenaB.Obj(a[4])));
        for (u32 p = 0; p < 5; ++p) {
            const u32 b = 13 + 11 * p, ped = a[5] + kPedStride * p, mat = 0x100 + 0x40 * p, intel = 0x300 + 0x40 * p, task = 0x500 + 0x40 * p;
            arenaD.Put8(a[6] + p, a[8 + p]);
            for (u32 k = 0; k < 3; ++k) { arenaC.Put32(ped + 4 + 4 * k, a[b + 5 + k]); arenaD.Put32(mat + 0x30 + 4 * k, a[b + 8 + k]); }
            arenaC.Put32(ped + 0x14, a[b + 2] ? u32(arenaD.Obj(mat)) : 0u);
            arenaC.Put16(ped + 0x22, a[b]); arenaC.Put32(ped + 0x598, a[b + 1]);
            arenaC.Put32(ped + 0x47C, u32(arenaD.Obj(intel)));
            arenaD.Put32(intel + 0x14, a[b + 3] ? u32(arenaD.Obj(task)) : 0u);
            if (a[b + 3]) { arenaD.Put32(task, vt); arenaD.Put32(task + 0x30, a[b + 3] == 1 ? 0xE9u : a[b + 4]); }
        }
        for (u32 k = 0; k < 3; ++k) arenaC.Put32(0x3000 + 4 * k, a[68 + k]);
        const u32 ul = kUserLists + 0x24 * a[71];
        for (u32 k = 0; k < 4; ++k) { PutRaw32(ul + 4 * k, a[72 + k]); PutRaw32(ul + 0x10 + 4 * k, a[77 + k]); }
        const u8 flag = u8(a[76]); PutRaw(ul + 0x20, &flag, 1);
        gIndex = a[71]; gMask = a[81];
        call.args[0] = u32(arenaC.Obj(0x3000)); call.argCount = 1; call.popBytes = 4;
        break;
    }
    case kCanScan: {
        arenaC.Put8(a[4] + 0x484, a[5]); arenaC.Put32(a[4] + 0x46C, a[6]); arenaC.Put32(a[4] + 0x47C, u32(arenaD.Obj(a[7])));
        arenaD.Put32(a[7] + 0x40, a[13] ? u32(arenaB.Obj(a[14])) : 0u); arenaD.Put32(a[7] + 0xC4, a[16]);
        if (a[13]) { arenaB.Put32(a[14], vt); arenaB.Put32(a[14] + 0x30, a[15]); }
        arenaA.Put8(o + 0xC, a[8]); arenaA.Put8(o + 0xD, a[9]); arenaA.Put8(o + 0xE, a[10]);
        gAlive = a[11]; gStatus = a[12]; gR3 = a[17]; gR4 = a[18];
        call.args[0] = u32(arenaC.Obj(a[4])); call.argCount = 1;
        break;
    }
    case kMedicDtor:
        arenaA.Put32(o + 0x18, a[4] ? u32(arenaB.Obj(a[5])) : 0u);
        arenaB.Put32(a[5], a[6]); arenaB.Put8(a[5] + 4, a[7]); arenaB.Put8(a[5] + 5, a[8]);
        arenaA.Put32(o + 0xC, a[9]); arenaA.Put32(o + 0x10, a[10]);
        gDtorWrite = a[11]; gRefOff = 0xC;
        break;
    case kPresentCtor:
        gDtorWrite = a[5]; gRefOff = 0xC;
        call.args[0] = a[4]; call.argCount = 1;
        break;
    case kPresentDtor:
        arenaA.Put32(o + 0xC, a[4]);
        gDtorWrite = a[5]; gRefOff = 0xC;
        break;
    case kHeading: {
        arenaA.Put32(o + 0xC, u32(arenaC.Obj(a[4])));
        arenaC.Put32(a[4] + 0x14, a[6] ? u32(arenaD.Obj(a[7])) : 0u);
        arenaC.Put32(a[5] + 0x14, a[8] ? u32(arenaD.Obj(a[9])) : 0u);
        for (u32 k = 0; k < 3; ++k) {
            arenaC.Put32(a[4] + 4 + 4 * k, a[10 + k]); arenaD.Put32(a[7] + 0x30 + 4 * k, a[13 + k]);
            arenaC.Put32(a[5] + 4 + 4 * k, a[16 + k]); arenaD.Put32(a[9] + 0x30 + 4 * k, a[19 + k]);
        }
        gAtan = a[22]; gLimit = a[23];
        call.args[0] = u32(arenaC.Obj(a[5])); call.argCount = 1; call.floatRet = true;
        break;
    }
    case kClone:
        arenaA.Put32(o + 0xC, a[4]);
        gTaskNew = a[5] ? u32(arenaB.Obj(a[6])) : 0u; gDtorWrite = a[7];
        break;
    case kPresentSub:
        gTaskNew = a[6] ? u32(arenaB.Obj(a[7])) : 0u; gHeading = a[8]; gDtorWrite = a[9];
        call.args[0] = a[4]; call.args[1] = a[5]; call.argCount = 2;
        break;
    case kNext:
        arenaA.Put32(o + 8, u32(arenaC.Obj(a[4])));
        arenaC.Put32(a[4], vt); arenaC.Put32(a[4] + 0x30, a[5]);
        gRet = a[7];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kFirst:
        gRet = a[5];
        call.args[0] = a[4]; call.argCount = 1;
        break;
    case kControl:
        arenaA.Put32(o + 0xC, a[4]); arenaA.Put32(o + 8, a[5]);
        gRet = a[7];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kSeekPos:
        arenaA.Put32(o + 0xC, a[4]); arenaA.Put32(o + 0x40, vt);
        gVec = {a[6], a[7], a[8]};
        call.args[0] = u32(arenaB.Obj(a[9])); call.args[1] = a[5]; call.argCount = 2;
        break;
    case kStay:
        arenaA.Put32(o + 0x46C, a[4]);
        call.args[0] = a[5]; call.argCount = 1;
        break;
    case kStream:
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 4;
        break;
    case kMinMax:
        for (u32 i = 0; i < 6; ++i) call.args[i] = a[4 + i];
        call.argCount = 6;
        break;
    case kBodyCopy:
        gDtorWrite = a[4];
        break;
    case kSelect:
        arenaB.Put32(a[4] + 0xC, a[12]); arenaB.Put32(a[4] + 0x10, a[13]);
        arenaC.Put32(a[5] + 0x14, a[6] ? u32(arenaD.Obj(a[7])) : 0u);
        arenaC.Put32(a[5] + 4, a[8]); arenaC.Put32(a[5] + 8, a[9]);
        arenaD.Put32(a[7] + 0x30, a[10]); arenaD.Put32(a[7] + 0x34, a[11]);
        call.args[0] = u32(arenaB.Obj(a[4])); call.args[1] = u32(arenaC.Obj(a[5])); call.args[2] = a[14]; call.args[3] = a[15]; call.argCount = 4;
        break;
    case kGoTo:
        arenaA.Put32(o + 8, a[4] ? u32(arenaB.Obj(a[5])) : 0u);
        if (a[4]) { arenaB.Put32(a[5], vt); arenaB.Put32(a[5] + 0x30, a[6]); }
        arenaA.Put32(o + 0xC, a[7]);
        for (u32 k = 0; k < 3; ++k) arenaA.Put32(o + 0x10 + 4 * k, a[8 + k]);
        arenaA.Put32(o + 0x1C, a[11]); arenaA.Put32(o + 0x20, a[12]); arenaA.Put8(o + 0x24, a[13]);
        arenaC.Put32(a[15] + 0x14, a[16] ? u32(arenaD.Obj(a[17])) : 0u);
        for (u32 k = 0; k < 3; ++k) { arenaC.Put32(a[15] + 4 + 4 * k, a[18 + k]); arenaD.Put32(a[17] + 0x30 + 4 * k, a[21 + k]); }
        arenaC.Put32(a[15] + 0x4DC, a[24]); arenaC.Put32(a[15] + 0x58C, a[25]);
        gDot = a[26]; gVec = {a[27], a[28], a[29]};
        gTaskNew = a[30] ? u32(arenaB.Obj(a[31])) : 0u;
        PutRaw32(kTimeStep, a[32]);
        gDtorWrite = a[33];
        call.args[0] = a[14]; call.args[1] = u32(arenaC.Obj(a[15])); call.argCount = 2;
        break;
    case kReactCtor:
        for (u32 i = 0; i < 5; ++i) call.args[i] = a[4 + i];
        call.argCount = 5;
        gDtorWrite = a[9]; gRefOff = 0x14;
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
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        const u32 floatBits = gFloatRet;
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kSdd: case kClosest: case kPresentCtor: case kReactCtor: case kClone: case kPresentSub: case kNext: case kFirst: case kControl: case kSeekPos: case kMinMax: case kGoTo:
            obs["ret"] = Tag(uintptr_t(eax)); break;                                                                 // this / el objeto nuevo / un puntero como etiqueta de arena
        case kConst: obs["ret"] = eax; break;                                                                        // valor completo de EAX
        case kCanScan: obs["ret"] = eax & 0xFFu; break;                                                              // bool: solo AL
        case kHeading: obs["ret"] = floatBits; break;                                                                // bits del float de ST0
        default: obs["ret"] = nullptr; break;                                                                        // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN198A_CASES (solo para las rondas de la prueba negativa y las pruebas rapidas, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN198A_CASES", buffer, sizeof(buffer)) > 0) {
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
// Colaborador con hook V (RH_ScopedVMTInstall): la ruta usa el cuerpo de la DLL de la ranura (la funcion que la DLL llama de forma no virtual) y el hook se apaga.
inline Pair MakeCollabV(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name; uintptr_t vbody = 0;
    if (FindHook(exe, cat, name, &vbody)) {
        if (vbody) env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, vbody));
        else env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}, {"virtual", vbody != 0}});
        if (env.collab.back()->Registered()) { p.dll = Body(env.collab.back()->Body()); p.hooked = true; }
    }
    return p;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn198a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn198a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn198a"); }
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
    // Colaborador con hook V (la DLL lo llama de forma no virtual: el cuerpo es la funcion de la ranura)
    auto FV = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollabV(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (0x5FFFD0): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    auto Del = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };   // CTask::operator delete (con hook)
    env.stubs[0] = {F(0x632630, &FkBody<0x632630>), Del()};
    env.stubs[1] = {F(0x632720, &FkBody<0x632720>), Del()};
    env.stubs[2] = {F(0x6389F0, &FkBody<0x6389F0>), Del()};
    env.stubs[3] = {F(0x6593A0, &FkBody<0x6593A0>), Del()};
    env.stubs[4] = {F(0x668630, &FkBody<0x668630>), Del()};
    env.stubs[5] = {Rt(15, &FkBody<0x659240>), Del()};
    env.stubs[6] = {F(0x5E0170, &FkIsAlive), FV(0x5FFA20, &FkScanEnt), F(0x571A00, &FkRef<0x571A00>), F(0x571B70, &FkRef<0x571B70>)};
    env.stubs[7] = {F(0x5E0170, &FkIsAlive), FV(0x5FFA20, &FkScanEnt)};
    env.stubs[8] = {F(0x4C4C70, &FkGet2d), ByExeAddress(0x5FFFD0, &FkAttr)};
    env.stubs[9] = {F(0x681720, &FkActiveTask), F(0x6F9F60, &FkGetIndex), F(0x59AED0, &FkSetScale), F(0x5EA220, &FkApproach)};
    env.stubs[10] = {F(0x5E0170, &FkIsAlive), F(0x608750, &FkStatus), F(0x4B8CA0, &FkCurEvent), F(0x4B5730, &FkResp), F(0x600CF0, &FkFriend)};
    env.stubs[12] = {F(0x571A00, &FkRef<0x571A00>), F(0x61A3D0, &FkBody<0x61A3D0>)};
    env.stubs[13] = {F(0x61A3B0, &FkBody<0x61A3B0>), F(0x571B70, &FkRef<0x571B70>)};
    env.stubs[15] = {F(0x571A00, &FkRef<0x571A00>), F(0x61A3D0, &FkBody<0x61A3D0>)};
    env.stubs[16] = {F(0x53CBE0, &FkAtan), F(0x53CB50, &FkLimit)};
    env.stubs[17] = {F(0x61A5A0, &FkTaskNew), Rt(13, &FkPresCtor)};
    env.stubs[18] = {F(0x61A5A0, &FkTaskNew), F(0x61ABE0, &FkRunTimed), Rt(16, &FkHeading), F(0x667E20, &FkAchieve)};
    env.stubs[19] = {F(0x5EFFE0, &FkSay), Rt(18, &FkCreateSub)};
    env.stubs[20] = {Rt(18, &FkCreateSub)};
    env.stubs[21] = {Rt(18, &FkCreateSub)};
    env.stubs[24] = {F(0x409C90, &FkMission)};
    env.stubs[26] = {F(0x61A3D0, &FkBody<0x61A3D0>)};
    env.stubs[28] = {F(0x61A5A0, &FkTaskNew), F(0x62F310, &FkStand), F(0x48E750, &FkPause), F(0x63B8C0, &FkLeave), F(0x667CD0, &FkGoToCtor), F(0x40FE60, &FkVecSub), F(0x40FDB0, &FkDot), Rt(27, &FkSelect)};
    env.stubs[29] = {F(0x61A3B0, &FkBody<0x61A3B0>), F(0x571B70, &FkRef<0x571B70>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {11u, 14u, 22u, 23u, 25u, 27u, 30u}) dllReal[f] = "toda la funcion (sin colaboradores; las llamadas virtuales usan una vtable falsa en la arena D)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada; se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kModelTable + 4u * u32(kModelLo), 4u * kModelSlots); regionsOf[8] = {regions.size() - 1};
    regions.emplace_back(kPedPool, 4); regions.emplace_back(kUserLists, 0x24 * kUserCount); regionsOf[9] = {regions.size() - 2, regions.size() - 1};
    regions.emplace_back(kTimeStep, 4); regionsOf[28] = {regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}

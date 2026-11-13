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

// Banco par a par Fn181A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 29 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos
// sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): FindPlayerVehicle 0x56E0D0,
// CleanUpOldReference 0x571A00, RegisterReference 0x571B70, CCamera::TakeControl 0x50C7C0, SetZoomValueCamStringScript 0x50C1B0, CPed::IsPlayer 0x5DF8F0, CEvent::TakesPriorityOver 0x4ABC30, GetActiveTask 0x681720,
// IsInSeeingRange 0x600C60, IsAlive 0x5E0170, AreInSameGroup 0x5F7F40, IsPassenger 0x6D1BD0, FindPlayerPed 0x56E210, ReportCrime 0x532010, CDecision::From 0x6006B0, constructores base de tareas 0x61A3B0 / 0x61A390,
// CTask::operator new 0x61A5A0 y constructor de CTaskComplexDie 0x630040 (todos con hook, apagado durante el banco). Solo en la direccion del EXE (la DLL los llama por ella): operator new 0x82119A y operator delete 0x8214BD.
// Las llamadas virtuales de TakesPriorityOver / AffectsPed pasan por vtables falsas estaticas (identicas para el EXE y la DLL).
// Memoria global del juego tocada (restaurada tras cada lado): CGame::currArea 0xB72914, jugador en foco 0xB7CD74, tabla de jugadores 0xB7CE48 y tabla de slots de TXD por defecto 0xC87F00..0xC88610.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria global, los eventos de los falsos (con sus
// argumentos), el valor devuelto (punteros como etiqueta de arena; solo AL en las funciones bool; bits del float en ST0 en GetLocalSoundLevel; sin valor en las funciones void) y el estado x87 (siempre vacio).
// EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F181ALive {
using namespace F181AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F181AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(i32 off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(i32 off, u32 v) { m[kGuard + off] = uint8(v); }
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
inline u32 gRet{};                   // EAX que devuelven los falsos (constructores, CDecision::From)
inline u32 gDtorWrite{};             // dword que el falso del constructor base deja en [this]
inline u32 gMem{};                   // memoria que devuelve el falso de CTask::operator new
inline std::array<u32, 2> gNewArr{}; // lo que devuelve operator new escalar falso, en orden
inline unsigned gNewArrN{};
inline u32 gSelf{}, gAlt{}, gSwap{}, gSwapOff{};   // CTask::operator new falso: si gSwap, deja gAlt en [gSelf + gSwapOff] (el codigo debe releer el campo tras la reserva)
inline std::array<u32, 2> gFpv{};    // FindPlayerVehicle: primer y segundo valor
inline unsigned gFpvN{};
inline u32 gIsPlayer{}, gBaseRet{}, gOwnPrio{}, gRefPrio{}, gRefType{}, gSrc{};   // CEventGunAimedAt::TakesPriorityOver
inline std::array<u32, 2> gTaskRet{}; inline unsigned gTaskN{};                   // AffectsPed: tareas activas devueltas en orden
inline u32 gT1{}, gT2{}, gType1{}, gType2{}, gSee{}, gAlive{}, gSame{}, gPass{}, gPlayerPed{};
inline DWORD gBenchThread{};
inline std::array<u32, 16> vtThis{}, vtRef{}, vtTask{};   // vtables falsas (estaticas)

// operator delete escalar del CRT (0x8214BD; cdecl, un puntero CRUDO): evento [direccion, argumento (etiqueta o crudo)]. No libera nada y no lo desreferencia. En otros hilos no hace nada.
template<u32 ADDR> inline void __cdecl FkDeleteRaw(void* p) {
    if (GetCurrentThreadId() != gBenchThread) return;
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(p);
}
// operator new escalar del EXE (0x82119A; cdecl, un tamano): en el hilo del banco evento [direccion, tamano] y devuelve el siguiente de gNewArr; en otros hilos llama a malloc del CRT (0x824257)
inline void* __cdecl FkNewScalar(u32 size) {
    if (GetCurrentThreadId() != gBenchThread) return reinterpret_cast<void*(__cdecl*)(u32)>(0x824257)(size);
    Ev& e = PushEv(0x82119A, 1);
    e.v[0] = size;
    return reinterpret_cast<void*>(uintptr_t(gNewArr[gNewArrN < 2 ? gNewArrN++ : 1]));
}
// CTask::operator new (0x61A5A0; cdecl, un tamano): evento [direccion, tamano]; si gSwap deja gAlt en [gSelf + gSwapOff]; devuelve gMem (memoria de B o nulo).
inline u32 __cdecl FkTaskNew(u32 size) {
    Ev& e = PushEv(0x61A5A0, 1);
    e.v[0] = size;
    if (gSwap) memcpy(reinterpret_cast<void*>(uintptr_t(gSelf) + gSwapOff), &gAlt, 4);
    return gMem;
}
// Constructor base sin argumentos de pila (CTaskComplex 0x61A3B0, CTaskSimple 0x61A390; thiscall, `ret`): evento [direccion, ECX (etiqueta)]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtorBase(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor de CTaskComplexDie (0x630040; thiscall, nueve dwords de pila, `ret 24h`): evento [direccion, ECX, los nueve argumentos]; de los argumentos 6, 7 y 9 (banderas) solo cuenta el byte bajo
// (el EXE empuja basura en los altos). Devuelve gRet.
inline u32 __fastcall FkCtor9(void* self, void*, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9) {
    Ev& e = PushEv(0x630040, 10);
    e.v[0] = Tag(self); e.v[1] = a1; e.v[2] = a2; e.v[3] = a3; e.v[4] = a4; e.v[5] = a5; e.v[6] = a6 & 0xFFu; e.v[7] = a7 & 0xFFu; e.v[8] = a8; e.v[9] = a9 & 0xFFu;
    return gRet;
}
// CDecision::From (0x6006B0; thiscall, un puntero en la pila, `ret 4`): evento [direccion, ECX (etiqueta), origen (etiqueta)]; devuelve gRet.
inline u32 __fastcall FkFrom(void* self, void*, void* src) {
    Ev& e = PushEv(0x6006B0, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(src);
    return gRet;
}
// FindPlayerVehicle (0x56E0D0; cdecl, dos argumentos): evento [direccion, primero, segundo (byte bajo)]; devuelve gFpv[0] la primera vez y gFpv[1] las demas.
inline u32 __cdecl FkFindPlayerVehicle(u32 a, u32 b) {
    Ev& e = PushEv(0x56E0D0, 2);
    e.v[0] = a; e.v[1] = b & 0xFFu;
    return gFpv[gFpvN < 2 ? gFpvN++ : 1];
}
// CEntity::CleanUpOldReference (0x571A00) y RegisterReference (0x571B70) (thiscall, un puntero, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta o crudo)]
template<u32 ADDR> inline void __fastcall FkRef(void* self, void*, void* ref) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(ref);
}
// CCamera::TakeControl (0x50C7C0; thiscall, cuatro dwords, `ret 10h`): evento [direccion, ECX, vehiculo (etiqueta), modo, tipo de cambio, ultimo]
inline void __fastcall FkTakeControl(void* self, void*, void* veh, u32 mode, u32 sw, u32 who) {
    Ev& e = PushEv(0x50C7C0, 5);
    e.v[0] = Tag(self); e.v[1] = Tag(veh); e.v[2] = mode; e.v[3] = sw; e.v[4] = who;
}
// CCamera::SetZoomValueCamStringScript (0x50C1B0; thiscall, un dword, `ret 4`): evento [direccion, ECX, argumento]
inline void __fastcall FkZoom(void* self, void*, u32 zoom) {
    Ev& e = PushEv(0x50C1B0, 2);
    e.v[0] = Tag(self); e.v[1] = zoom;
}
// Funciones de las vtables falsas (thiscall sin argumentos): evento [etiqueta propia, ECX]
inline u32 __fastcall FkVtOwnPrio(void* self, void*) { Ev& e = PushEv(0xA002, 1); e.v[0] = Tag(self); return gOwnPrio; }
inline u32 __fastcall FkVtRefType(void* self, void*) { Ev& e = PushEv(0xB001, 1); e.v[0] = Tag(self); return gRefType; }
inline u32 __fastcall FkVtRefPrio(void* self, void*) { Ev& e = PushEv(0xB002, 1); e.v[0] = Tag(self); return gRefPrio; }
inline u32 __fastcall FkVtRefSource(void* self, void*) { Ev& e = PushEv(0xB00A, 1); e.v[0] = Tag(self); return gSrc; }
inline u32 __fastcall FkVtTaskType(void* self, void*) { Ev& e = PushEv(0xC004, 1); e.v[0] = Tag(self); return uintptr_t(self) == gT1 ? gType1 : gType2; }
// CPed::IsPlayer (0x5DF8F0; thiscall, sin argumentos, `ret`): evento [direccion, ECX]; devuelve gIsPlayer (solo cuenta AL)
inline u32 __fastcall FkIsPlayer(void* self, void*) { Ev& e = PushEv(0x5DF8F0, 1); e.v[0] = Tag(self); return gIsPlayer; }
// CEvent::TakesPriorityOver (0x4ABC30; thiscall, un puntero, `ret 4`): evento [direccion, ECX, argumento]; devuelve gBaseRet
inline u32 __fastcall FkBasePrio(void* self, void*, void* ref) { Ev& e = PushEv(0x4ABC30, 2); e.v[0] = Tag(self); e.v[1] = Tag(ref); return gBaseRet; }
// CTaskManager::GetActiveTask (0x681720; thiscall, sin argumentos): evento [direccion, ECX]; devuelve gTaskRet[0] la primera vez y gTaskRet[1] las demas
inline u32 __fastcall FkGetActiveTask(void* self, void*) { Ev& e = PushEv(0x681720, 1); e.v[0] = Tag(self); return gTaskRet[gTaskN < 2 ? gTaskN++ : 1]; }
// CPedIntelligence::IsInSeeingRange (0x600C60; thiscall, un puntero, `ret 4`): evento [direccion, ECX, posicion (etiqueta)]
inline u32 __fastcall FkSeeing(void* self, void*, void* pos) { Ev& e = PushEv(0x600C60, 2); e.v[0] = Tag(self); e.v[1] = Tag(pos); return gSee; }
inline u32 __fastcall FkAlive(void* self, void*) { Ev& e = PushEv(0x5E0170, 1); e.v[0] = Tag(self); return gAlive; }
// CPedGroups::AreInSameGroup (0x5F7F40; cdecl, dos punteros): evento [direccion, primero, segundo]
inline u32 __cdecl FkSameGroup(void* a, void* b) { Ev& e = PushEv(0x5F7F40, 2); e.v[0] = Tag(a); e.v[1] = Tag(b); return gSame; }
// CVehicle::IsPassenger (0x6D1BD0; thiscall, un puntero, `ret 4`): evento [direccion, ECX, argumento]
inline u32 __fastcall FkPassenger(void* self, void*, void* ped) { Ev& e = PushEv(0x6D1BD0, 2); e.v[0] = Tag(self); e.v[1] = Tag(ped); return gPass; }
// FindPlayerPed (0x56E210; cdecl, un entero): evento [direccion, argumento]
inline u32 __cdecl FkFindPlayerPed(u32 id) { Ev& e = PushEv(0x56E210, 1); e.v[0] = id; return gPlayerPed; }
// CCrime::ReportCrime (0x532010; cdecl, tipo y dos punteros): evento [direccion, tipo, ped, jugador]
inline void __cdecl FkReportCrime(u32 type, void* ped, void* player) { Ev& e = PushEv(0x532010, 3); e.v[0] = type; e.v[1] = Tag(ped); e.v[2] = Tag(player); }
inline void InitVtables() {
    vtThis = {}; vtRef = {}; vtTask = {};
    vtThis[2] = u32(uintptr_t(&FkVtOwnPrio));
    vtRef[1] = u32(uintptr_t(&FkVtRefType)); vtRef[2] = u32(uintptr_t(&FkVtRefPrio)); vtRef[10] = u32(uintptr_t(&FkVtRefSource));
    vtTask[4] = u32(uintptr_t(&FkVtTaskType));
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

inline u32 Ptr(u32 mode, const u32 arena, u32 raw) { return mode == 0 ? 0u : (mode == 1 ? arena : raw); }
inline void W32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[60]); arenaB.Pattern(a[2], a[61]); arenaC.Pattern(a[2], a[62]); arenaD.Pattern(a[2], a[63]);
    gRet = 0; gDtorWrite = 0; gMem = 0; gNewArr = {}; gNewArrN = 0; gSelf = 0; gAlt = 0; gSwap = 0; gSwapOff = 0; gFpv = {}; gFpvN = 0;
    gIsPlayer = gBaseRet = gOwnPrio = gRefPrio = gRefType = gSrc = 0; gTaskRet = {}; gTaskN = 0; gT1 = gT2 = gType1 = gType2 = gSee = gAlive = gSame = gPass = gPlayerPed = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    const auto k = kKind[c.fn];
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (k) {
    case kConst: case kBool: case kVag:
        break;
    case kConstF:
        call.floatRet = true;
        break;
    case kArea:
        arenaA.Put8(o + 0x2F, a[4]);
        W32(kCurrArea, a[5]);
        break;
    case kRemote: {
        const u32 vNew = thisA, vB = u32(arenaB.Obj(a[6])), vC = u32(arenaC.Obj(a[8]));
        const auto pick = [&](u32 mode) { return mode == 0 ? 0u : (mode == 1 ? vNew : (mode == 2 ? vB : vC)); };
        arenaA.Put8(o + 0x36, a[10]); arenaA.Put8(o + 0x428, a[11]); arenaA.Put8(o + 0x42D, a[12]);
        arenaB.Put8(i32(a[6] + 0x36), a[13]); arenaC.Put8(i32(a[8] + 0x36), a[14]);
        *reinterpret_cast<uint8*>(kPlayerInFocus) = uint8(a[4]);
        W32(kPlayers + a[4] * kPlayerStride, pick(a[5]));
        gFpv = {pick(a[7]), pick(a[9])};
        call.args[0] = vNew; call.argCount = 1; call.popBytes = 4;
        break;
    }
    case kScale:
        for (u32 i = 0; i < 12; ++i) arenaA.Put32(o + 16 * (i / 3) + 4 * (i % 3), a[8 + i]);
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 0;
        break;
    case kTurn:
        call.args[0] = a[4]; call.args[1] = a[5]; call.args[2] = a[6]; call.argCount = 3; call.popBytes = 0;
        break;
    case kGunPrio: {
        arenaA.Put32(o, u32(uintptr_t(vtThis.data())));
        arenaA.Put32(o + 0x14, a[4] ? u32(arenaC.Obj(a[5])) : 0u);
        arenaB.Put32(a[6], u32(uintptr_t(vtRef.data())));
        gIsPlayer = a[7]; gOwnPrio = a[13]; gRefPrio = a[12]; gRefType = a[11]; gBaseRet = a[14];
        gSrc = a[8] == 0 ? 0u : (a[8] == 1 ? u32(arenaC.Obj(a[5])) : (a[8] == 2 ? u32(arenaD.Obj(a[9])) : a[10]));
        call.args[0] = u32(arenaB.Obj(a[6])); call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kGunAffects: {
        const u32 ped = u32(arenaB.Obj(a[6])), aimed = u32(arenaC.Obj(a[5]));
        arenaA.Put32(o + 0x14, a[4] ? aimed : 0u);
        arenaB.Put32(a[6] + 0x47C, u32(arenaD.Obj(kDIntel))); arenaB.Put32(a[6] + 0x46C, a[16]);
        arenaB.Put32(a[6] + 0x58C, a[17] ? u32(arenaD.Obj(kDVehicle)) : 0u); arenaB.Put32(a[6] + 0x598, a[19]);
        arenaD.Put32(i32(kDVehicle + 0x460), a[18]);
        for (u32 i = 0; i < 3; ++i) { arenaC.Put32(a[5] + 4 + 4 * i, a[23 + i]); arenaD.Put32(kDMatrix + 0x30 + 4 * i, a[26 + i]); }
        arenaC.Put32(a[5] + 0x14, a[20] ? u32(arenaD.Obj(kDMatrix)) : 0u);
        arenaD.Put32(kDT1, u32(uintptr_t(vtTask.data()))); arenaD.Put32(kDT2, u32(uintptr_t(vtTask.data())));
        gT1 = u32(arenaD.Obj(kDT1)); gT2 = u32(arenaD.Obj(kDT2)); gType1 = a[10]; gType2 = a[11];
        gTaskRet = {a[8] == 0 ? 0u : (a[8] == 1 ? gT1 : gT2), a[9] == 1 ? gT1 : gT2};
        gIsPlayer = a[7]; gSee = a[12]; gAlive = a[13]; gSame = a[14]; gPass = a[15];
        gPlayerPed = a[21] == 0 ? 0u : (a[21] == 1 ? u32(arenaD.Obj(kDPlayer)) : a[22]);
        call.args[0] = ped; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kDm: {
        const u32 off = kDmOffset[c.fn];
        call.ecx = thisA - off;
        gRet = a[5];
        call.args[0] = u32(arenaB.Obj(a[4])); call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kDieCtor:
        gRet = a[6]; gDtorWrite = a[5];
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 0;
        break;
    case kCloneDie:
        arenaA.Put32(o + 0xC, a[6]);
        gMem = a[4] == 0 ? 0u : u32(arenaB.Obj(a[5]));
        gRet = a[10]; gDtorWrite = a[9]; gSelf = thisA; gSwap = a[8]; gSwapOff = 0xC; gAlt = a[7];
        break;
    case kCloneDrown:
        gMem = a[4] == 0 ? 0u : u32(arenaB.Obj(a[5]));
        gRet = a[10]; gDtorWrite = a[9];
        break;
    case kCloneComplex: {
        static constexpr u32 offs[7]{0xC, 0x10, 0x14, 0x18, 0x1C, 0x20, 0x24};
        for (u32 i = 0; i < 7; ++i) arenaA.Put32(o + offs[i], a[6 + i]);
        gMem = a[4] == 0 ? 0u : u32(arenaB.Obj(a[5]));
        gRet = a[16]; gSelf = thisA; gSwap = a[13]; gSwapOff = offs[a[14] % 7]; gAlt = a[15];
        break;
    }
    case kTxdSlot:
        FillKind(reinterpret_cast<u8*>(kTxdSlots), kTxdSlotsBytes, a[2], a[62]);
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 4;
        break;
    case kFlush:
    case kPSdd:
        arenaA.Put32(o + 0, Ptr(a[4], u32(arenaB.Obj(a[5])), a[6])); arenaA.Put32(o + 4, Ptr(a[7], u32(arenaC.Obj(a[8])), a[9]));
        arenaA.Put32(o + 8, a[10]); arenaA.Put32(o + 0xC, a[11]); arenaA.Put8(o + 0x10, a[12]);
        if (k == kPSdd) { call.args[0] = a[13]; call.argCount = 1; call.popBytes = 0; }
        break;
    case kPCtor:
        gNewArr = {Ptr(a[4], u32(arenaB.Obj(a[5])), a[6]), Ptr(a[7], u32(arenaC.Obj(a[8])), a[9])};
        call.args[0] = a[10]; call.args[1] = a[11]; call.argCount = 2; call.popBytes = 0;
        break;
    case kPNew: {
        arenaA.Put32(o + 0, a[4]); arenaA.Put32(o + 4, u32(arenaD.Obj(a[5]))); arenaA.Put32(o + 8, a[6]); arenaA.Put32(o + 0xC, a[7]); arenaA.Put8(o + 0x10, a[8]);
        for (u32 i = 0; i < a[6]; ++i) {
            const u32 counter = a[11] == 0 ? 0u : (a[11] == 1 ? 0x7Fu : Rand(a[63], 1000 + i) & 0x7Fu);
            bool free = false;
            switch (a[9]) {
            case 0: free = false; break;
            case 1: free = i == a[10]; break;
            case 2: free = Rand(a[63], 5000 + i) % 10 == 0; break;
            case 3: free = (Rand(a[63], 6000 + i) & 1) != 0; break;
            default: free = true; break;
            }
            arenaD.Put8(i32(a[5] + i), counter | (free ? 0x80u : 0u));
        }
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
        nev = 0; gNewArrN = 0; gFpvN = 0; gTaskN = 0;
        u32 fret = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            const Reg r = Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
            fret = gFloatRet;
            return r;
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kPCtor: case kPNew: case kPSdd: case kVag: case kDieCtor: case kCloneDie: case kCloneDrown: case kCloneComplex: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case kDm: obs["ret"] = fn == 12 ? json(Tag(uintptr_t(eax))) : json(nullptr); break;
        case kConst: case kTxdSlot: obs["ret"] = eax; break;                                                                                                    // valor completo de EAX
        case kBool: case kArea: case kGunPrio: case kGunAffects: obs["ret"] = eax & 0xFFu; break;                                                               // solo AL
        case kConstF: obs["ret"] = fret; break;                                                                                                                 // bits del float en ST0
        default: obs["ret"] = nullptr; break;                                                                                                                   // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN181A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN181A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn181a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn181a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn181a"); }
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
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (operator new[] 0x821195, delete[] 0x8213AE, operator delete 0x8214BD): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    InitVtables();
    env.stubs[1] = {F(0x56E0D0, &FkFindPlayerVehicle), F(0x571A00, &FkRef<0x571A00>), F(0x571B70, &FkRef<0x571B70>), F(0x50C7C0, &FkTakeControl), F(0x50C1B0, &FkZoom)};
    env.stubs[9] = {F(0x5DF8F0, &FkIsPlayer), F(0x4ABC30, &FkBasePrio)};
    env.stubs[10] = {F(0x5DF8F0, &FkIsPlayer), F(0x681720, &FkGetActiveTask), F(0x600C60, &FkSeeing), F(0x5E0170, &FkAlive), F(0x5F7F40, &FkSameGroup), F(0x6D1BD0, &FkPassenger), F(0x56E210, &FkFindPlayerPed),
                     F(0x532010, &FkReportCrime)};
    for (unsigned f = 12; f <= 17; ++f) env.stubs[f] = {F(0x6006B0, &FkFrom)};
    env.stubs[18] = {F(0x61A3B0, &FkCtorBase<0x61A3B0>)};
    env.stubs[20] = {F(0x61A5A0, &FkTaskNew), F(0x61A3B0, &FkCtorBase<0x61A3B0>)};
    env.stubs[21] = {F(0x61A5A0, &FkTaskNew), F(0x61A390, &FkCtorBase<0x61A390>)};
    env.stubs[22] = {F(0x61A5A0, &FkTaskNew), F(0x630040, &FkCtor9)};
    env.stubs[24] = {ByExeAddress(0x8214BD, &FkDeleteRaw<0x8214BD>)};
    env.stubs[25] = {ByExeAddress(0x82119A, &FkNewScalar)};
    env.stubs[26] = {ByExeAddress(0x8214BD, &FkDeleteRaw<0x8214BD>)};
    env.stubs[28] = {ByExeAddress(0x8214BD, &FkDeleteRaw<0x8214BD>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {0u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 11u, 19u, 23u, 27u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[9] = dllReal[10] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL); las llamadas virtuales van por vtables falsas estaticas";
    dllReal[2] = "toda la funcion (sin colaboradores); la aritmetica x87 corre real en ambos lados";
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
    // Memoria global del juego tocada: CGame::currArea, jugador en foco, tabla de jugadores y slots de TXD por defecto; se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kCurrArea, 4); regionsOf[0] = {regions.size() - 1};
    regions.emplace_back(kPlayerInFocus, 4); regions.emplace_back(kPlayers, kPlayersBytes); regionsOf[1] = {regions.size() - 2, regions.size() - 1};
    regions.emplace_back(kTxdSlots, kTxdSlotsBytes); regionsOf[23] = {regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}

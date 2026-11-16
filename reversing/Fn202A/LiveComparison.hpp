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
#include "Tasks/TaskTypes/TaskSimpleAnim.h"

// Banco par a par Fn202A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 32 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos
// sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Rt / Pair): constructores de eventos
// (0x4AC990, 0x4ACCF0, 0x4ACF00, 0x4AD250, 0x4ADEA0, 0x4ADFD0, 0x4AE1A0, 0x4AE320, 0x4AE5D0, 0x4AE6E0), CTask::operator new 0x61A5A0 y delete 0x61A5B0, constructor base 0x61A390, cuerpo 0x61A6F0, CAnimManager 0x4D3940 / 0x4D42F0 /
// 0x4D4410 / 0x4D4610, SetFinishCallback 0x4CEBE0, RpAnimBlendClumpGetAssociation 0x4D68B0, CTaskTimer::IsOutOfTime 0x420E30, SetCurrentTime 0x4CEA80, CWorld::Remove 0x563280, constructor 0x61AE50, MakeAbortable 0x61A790 (V),
// y para el escaner TransformFromObjectSpace 0x5334F0, GetIndex 0x6F9F60, GetPedAttractorManager 0x5EE190, HasEmptySlot 0x5EBB00, operator= 0x59BBC0, SetScale 0x59AED0 e IsApproachable 0x5EA220. El destructor de la matriz local (EXE: 0x59ACD0; DLL: ~CMatrix, que no es el cuerpo enganchado) y CPlaceable::GetMatrix (0x411990) corren reales en ambos lados.
// El pool de eventos: el EXE reserva con CEventPool::New (0x4B5570, llamada directa con ECX = [0xB744AC]); la DLL lo hace por CEvent::operator new (0x4B5620): se falsea 0x4B5570 en la direccion del EXE y el cuerpo de la
// DLL de 0x4B5620 con un falso que anota el mismo evento. Los cuerpos de las funciones probadas que otras llaman (0x61AC90, 0x61AD10, 0x61ADF0, 0x61BCA0, 0x61AEB0) llevan el falso en su direccion del EXE y en su cuerpo (Rt).
// CPlaceable::GetMatrix (0x411990) y el destructor de CMatrix (0x59ACD0) corren reales en ambos lados (el escaner usa entidades sinteticas con matriz).
// Memoria global del juego tocada (restaurada tras cada lado): puntero del pool de eventos 0xB744AC, contador 0xBB4A70, reloj 0xB7CB84, lluvia 0xC81324 y radios 0xC39EE0.

namespace F202ALive {
using namespace F202AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F202AFix::Arena(WIN);
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
inline Arena<kWin>& ArenaBySel(u32 sel) { return sel == 0 ? arenaA : (sel == 1 ? arenaB : (sel == 2 ? arenaC : arenaD)); }

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos
inline u32 gDtorWrite{};             // dword que el falso del constructor / cuerpo deja en [ECX]
inline u32 gMem{};                   // memoria que devuelven los falsos de New
inline u32 gInner{};                 // EAX del Clone del evento interno
inline u32 gV0{}, gV4{}, gVC{};      // lo que escribe el falso del constructor base (+0, +4, byte +0xC)
inline u32 gBlock{}, gHier{}, gAssoc{}, gNewAssoc{}, gFound{}, gTimerRet{};
inline u32 gXf[3]{}, gIdx{}, gMgr{}, gHas{}, gApp{};
inline u32 gSelf{}, gSwapAt{}, gSwapOff{}, gSwapVal{}, gRemoveCalls{};
inline u32 gFinishCbDll{};
inline DWORD gBenchThread{};

// CEventPool::New (0x4B5570; thiscall, sin argumentos de pila): evento [0x4B5570, ECX (etiqueta)]; devuelve gMem.
inline u32 __fastcall FkPoolNewExe(void* self, void*) {
    Ev& e = PushEv(0x4B5570, 1);
    e.v[0] = Tag(self);
    return gMem;
}
// CEvent::operator new (0x4B5620 en la DLL; cdecl, un tamano): evento [0x4B5570, [0xB744AC] (etiqueta)], el mismo que anota el EXE con su llamada directa; devuelve gMem.
inline u32 __cdecl FkOpNewEvent(u32 /*size*/) {
    Ev& e = PushEv(0x4B5570, 1);
    e.v[0] = Tag(uintptr_t(*reinterpret_cast<const u32*>(kEventPool)));
    return gMem;
}
// CTask::operator new (0x61A5A0; cdecl, un tamano): evento [direccion, tamano]; devuelve gMem.
inline u32 __cdecl FkTaskNew(u32 size) {
    Ev& e = PushEv(0x61A5A0, 1);
    e.v[0] = size;
    return gMem;
}
// CTask::operator delete (0x61A5B0; cdecl, un puntero VALIDO): evento [direccion, argumento (etiqueta), dword de [argumento] al entrar].
inline void __cdecl FkTaskDelete(void* p) {
    Ev& e = PushEv(0x61A5B0, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// Cuerpo / base sin argumentos de pila (thiscall): evento [direccion del EXE, ECX (etiqueta), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor base de CTaskSimple (0x61A390; thiscall, sin argumentos): evento [direccion, ECX, dword de [ECX] al entrar]; escribe +0 = gV0, +4 = gV4 y el byte +0xC = gVC; devuelve gRet.
inline u32 __fastcall FkBaseCtor(void* self, void*) {
    Ev& e = PushEv(0x61A390, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gV0, 4);
    memcpy(static_cast<uint8*>(self) + 4, &gV4, 4);
    *(static_cast<uint8*>(self) + 0xC) = uint8(gVC);
    return gRet;
}
// Constructores con 1 a 8 argumentos de pila (thiscall, `ret 4*N`): evento [direccion, ECX (etiqueta), cada argumento enmascarado (etiqueta)]; deja gDtorWrite en [ECX]; devuelve gRet. Las mascaras recogen que el constructor
// real solo lee los 16 (o 8) bits bajos de algunos argumentos.
template<u32 ADDR, u32 M1 = ~0u, u32 M2 = ~0u, u32 M3 = ~0u, u32 M4 = ~0u, u32 M5 = ~0u, u32 M6 = ~0u, u32 M7 = ~0u, u32 M8 = ~0u> struct Ctor {
    static void Log(void* self, std::initializer_list<u32> args) {
        constexpr u32 masks[8]{M1, M2, M3, M4, M5, M6, M7, M8};
        Ev& e = PushEv(ADDR, u32(args.size()) + 1);
        e.v[0] = Tag(self);
        u32 i = 1;
        for (u32 v : args) { e.v[i] = Tag(uintptr_t(v & masks[i - 1])); ++i; }
        memcpy(self, &gDtorWrite, 4);
    }
    static u32 __fastcall N1(void* self, void*, u32 a1) { Log(self, {a1}); return gRet; }
    static u32 __fastcall N2(void* self, void*, u32 a1, u32 a2) { Log(self, {a1, a2}); return gRet; }
    static u32 __fastcall N3(void* self, void*, u32 a1, u32 a2, u32 a3) { Log(self, {a1, a2, a3}); return gRet; }
    static u32 __fastcall N4(void* self, void*, u32 a1, u32 a2, u32 a3, u32 a4) { Log(self, {a1, a2, a3, a4}); return gRet; }
    static u32 __fastcall N6(void* self, void*, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6) { Log(self, {a1, a2, a3, a4, a5, a6}); return gRet; }
    static u32 __fastcall N7(void* self, void*, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7) { Log(self, {a1, a2, a3, a4, a5, a6, a7}); return gRet; }
    static u32 __fastcall N8(void* self, void*, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8) { Log(self, {a1, a2, a3, a4, a5, a6, a7, a8}); return gRet; }
};
// Casilla K de la vtable del evento interno de CEventGroupEvent (thiscall, sin argumentos): evento [0x1000 + K, ECX]; la ranura 4 (Clone) devuelve gInner, las demas un valor distinto
template<u32 K> inline u32 __fastcall FkInnerSlot(void* self, void*) {
    Ev& e = PushEv(0x1000u + K, 1);
    e.v[0] = Tag(self);
    return K == 4 ? gInner : 0x5000u + K;
}
// Casilla K de la vtable de una mano de CTaskSimplePlayHandSignalAnim (thiscall, un dword de pila: banderas, `ret 4`): evento [0x2000 + K, ECX, argumento]; devuelve gRet
template<u32 K> inline u32 __fastcall FkHandSlot(void* self, void*, u32 flags) {
    Ev& e = PushEv(0x2000u + K, 2);
    e.v[0] = Tag(self); e.v[1] = flags;
    return gRet;
}
template<size_t... I> constexpr auto MakeInnerTable(std::index_sequence<I...>) { return std::array<void*, sizeof...(I)>{reinterpret_cast<void*>(&FkInnerSlot<u32(I)>)...}; }
template<size_t... I> constexpr auto MakeHandTable(std::index_sequence<I...>) { return std::array<void*, sizeof...(I)>{reinterpret_cast<void*>(&FkHandSlot<u32(I)>)...}; }
inline const auto kInnerTable = MakeInnerTable(std::make_index_sequence<kVtableSlots>{});
inline const auto kHandTable = MakeHandTable(std::make_index_sequence<kVtableSlots>{});

// CAnimManager::GetAnimationBlock(const char*) (0x4D3940; cdecl, un puntero): evento [direccion, argumento (etiqueta), FNV de la cadena]; devuelve gBlock.
inline u32 __cdecl FkGetAnimBlock(const char* p) {
    Ev& e = PushEv(0x4D3940, 2);
    e.v[0] = Tag(p); e.v[1] = FnvStr(p);
    return gBlock;
}
// CAnimManager::GetAnimation(const char*, bloque) (0x4D42F0; cdecl, dos argumentos): evento [direccion, nombre (etiqueta), FNV, bloque]; devuelve gHier.
inline u32 __cdecl FkGetAnim(const char* name, u32 block) {
    Ev& e = PushEv(0x4D42F0, 3);
    e.v[0] = Tag(name); e.v[1] = FnvStr(name); e.v[2] = block;
    return gHier;
}
// CAnimManager::BlendAnimation (0x4D4410 con jerarquia, 0x4D4610 por grupo e identificador; cdecl, cuatro argumentos): evento [direccion, clump, jerarquia o grupo (etiqueta), banderas o animacion, mezcla]; devuelve gAssoc.
inline u32 __cdecl FkBlendHier(u32 clump, u32 hier, u32 flags, u32 blend) {
    Ev& e = PushEv(0x4D4410, 4);
    e.v[0] = clump; e.v[1] = Tag(uintptr_t(hier)); e.v[2] = flags; e.v[3] = blend;
    return gAssoc;
}
inline u32 __cdecl FkBlendId(u32 clump, u32 group, u32 anim, u32 blend) {
    Ev& e = PushEv(0x4D4610, 4);
    e.v[0] = clump; e.v[1] = group; e.v[2] = anim; e.v[3] = blend;
    return gAssoc;
}
// CAnimBlendAssociation::SetFinishCallback (0x4CEBE0; thiscall, dos argumentos de pila, `ret 8`): evento [direccion, ECX (etiqueta), callback, dato (etiqueta)]. El EXE pasa 0x61A8A0 (CTaskSimpleAnim::FinishRunAnimCB, sin hook);
// la DLL pasa su propia reconstruccion: se anota la misma etiqueta si es esa funcion.
inline u32 __fastcall FkSetFinishCb(void* assoc, void*, u32 callback, u32 data) {
    Ev& e = PushEv(0x4CEBE0, 3);
    e.v[0] = Tag(assoc); e.v[1] = (callback == 0x61A8A0u || callback == gFinishCbDll) ? 0x61A8A0u : callback; e.v[2] = Tag(uintptr_t(data));
    return gRet;
}
// StartAnim (0x61ADF0, funcion probada: Rt; thiscall, un argumento de pila, `ret 4`): evento [direccion, ECX, peaton (etiqueta)]; deja gNewAssoc en [ECX + 8].
inline u32 __fastcall FkStartAnimBody(void* self, void*, u32 ped) {
    Ev& e = PushEv(0x61ADF0, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(ped));
    memcpy(static_cast<uint8*>(self) + 8, &gNewAssoc, 4);
    return gRet;
}
// RpAnimBlendClumpGetAssociation (0x4D68B0; cdecl, dos argumentos): evento [direccion, clump, selector]; devuelve gFound.
inline u32 __cdecl FkGetAssoc(u32 clump, u32 selector) {
    Ev& e = PushEv(0x4D68B0, 2);
    e.v[0] = clump; e.v[1] = selector;
    return gFound;
}
// CTaskTimer::IsOutOfTime (0x420E30; thiscall): evento [direccion, ECX (etiqueta)]; devuelve gTimerRet (el llamador solo prueba AL).
inline u32 __fastcall FkIsOutOfTime(void* self, void*) {
    Ev& e = PushEv(0x420E30, 1);
    e.v[0] = Tag(self);
    return gTimerRet;
}
// CAnimBlendAssociation::SetCurrentTime (0x4CEA80; thiscall, un float de pila como patron de bits, `ret 4`): evento [direccion, ECX (etiqueta), argumento].
inline u32 __fastcall FkSetCurTime(void* assoc, void*, u32 time) {
    Ev& e = PushEv(0x4CEA80, 2);
    e.v[0] = Tag(assoc); e.v[1] = time;
    return gRet;
}
// CWorld::Remove (0x563280; cdecl, un puntero): evento [direccion, argumento (etiqueta)]; en la llamada gSwapAt cambia el campo gSwapOff de la tarea por gSwapVal (el EXE y la DLL deben releerlo).
inline void __cdecl FkWorldRemove(void* p) {
    Ev& e = PushEv(0x563280, 1);
    e.v[0] = Tag(p);
    if (++gRemoveCalls == gSwapAt) memcpy(reinterpret_cast<void*>(uintptr_t(gSelf) + gSwapOff), &gSwapVal, 4);
}
// CTaskSimpleAnim::MakeAbortable (0x61A790, V; thiscall, tres argumentos de pila, `ret 0Ch`): evento [direccion, ECX, peaton (etiqueta), prioridad, evento (etiqueta)]; devuelve gRet.
inline u32 __fastcall FkMakeAbort(void* self, void*, u32 ped, u32 priority, u32 event) {
    Ev& e = PushEv(0x61A790, 4);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(ped)); e.v[2] = priority; e.v[3] = Tag(uintptr_t(event));
    return gRet;
}
// CEntity::TransformFromObjectSpace (0x5334F0; thiscall, destino y vector por puntero, `ret 8`): evento [direccion, ECX (etiqueta), destino (etiqueta), tres dwords del vector]; escribe gXf en el destino; devuelve el destino.
inline void* __fastcall FkXform(void* self, void*, void* out, const u32* in) {
    Ev& e = PushEv(0x5334F0, 6);
    e.v[0] = Tag(self); e.v[1] = Tag(out); e.v[2] = in[0]; e.v[3] = in[1]; e.v[4] = in[2]; e.v[5] = Tag(in);
    memcpy(out, gXf, 12);
    return out;
}
// CScripted2dEffects::GetIndex (0x6F9F60; cdecl, un puntero): evento [direccion, argumento (etiqueta)]; devuelve gIdx.
inline u32 __cdecl FkGetIndex(void* effect) {
    Ev& e = PushEv(0x6F9F60, 1);
    e.v[0] = Tag(effect);
    return gIdx;
}
// CPedAttractorManager::GetPedAttractorManager (0x5EE190; cdecl, sin argumentos): evento [direccion]; devuelve gMgr.
inline u32 __cdecl FkGetMgr() {
    PushEv(0x5EE190, 0);
    return gMgr;
}
// CPedAttractorManager::HasEmptySlot (0x5EBB00; thiscall, dos argumentos, `ret 8`): evento [direccion, ECX, efecto, entidad (etiquetas)]; devuelve gHas.
inline u32 __fastcall FkHasEmpty(void* mgr, void*, void* effect, void* entity) {
    Ev& e = PushEv(0x5EBB00, 3);
    e.v[0] = Tag(mgr); e.v[1] = Tag(effect); e.v[2] = Tag(entity);
    return gHas;
}
// CMatrix::operator= (0x59BBC0; thiscall, un puntero, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]; devuelve ECX.
inline void* __fastcall FkMatAssign(void* self, void*, void* other) {
    Ev& e = PushEv(0x59BBC0, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(other);
    return self;
}
// CMatrix::SetScale(float) (0x59AED0; thiscall, un float como bits, `ret 4`): evento [direccion, ECX (etiqueta), argumento]; devuelve ECX.
inline void* __fastcall FkSetScale(void* self, void*, u32 scale) {
    Ev& e = PushEv(0x59AED0, 2);
    e.v[0] = Tag(self); e.v[1] = scale;
    return self;
}
// CPedAttractorManager::IsApproachable (0x5EA220; cdecl, cuatro argumentos): evento [direccion, efecto, matriz, tercero, peaton] (punteros como etiquetas); devuelve gApp.
inline u32 __cdecl FkApproach(void* effect, void* matrix, u32 third, void* ped) {
    Ev& e = PushEv(0x5EA220, 4);
    e.v[0] = Tag(effect); e.v[1] = Tag(matrix); e.v[2] = third; e.v[3] = Tag(ped);
    return gApp;
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

// EAX que devuelve el falso de un constructor segun el selector (0 la memoria, 1 la memoria + 4, 2 cero, 3 un valor crudo)
inline u32 CtorEax(u32 selector, u32 memory, u32 raw) {
    switch (selector) {
    case 0: return memory;
    case 1: return memory + 4;
    case 2: return 0;
    default: return raw;
    }
}
inline void PutString(Arena<kWin>& arena, u32 off, u32 length, u32 seed) {
    for (u32 j = 0; j < length; ++j) arena.Put8(i32(off + j), StrByte(seed, j));
    arena.Put8(i32(off + length), 0);
}
inline u32 PtrIn(u32 sel, u32 off, u32 thisOff) { return u32(sel == 0 ? ArenaBySel(0).Obj(thisOff + off) : ArenaBySel(sel).Obj(off)); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[92]); arenaB.Pattern(a[2], a[93]); arenaC.Pattern(a[2], a[94]); arenaD.Pattern(a[2], a[95]);
    gRet = 0; gDtorWrite = 0; gMem = 0; gInner = 0; gV0 = gV4 = gVC = 0; gBlock = gHier = gAssoc = gNewAssoc = gFound = gTimerRet = 0;
    gXf[0] = gXf[1] = gXf[2] = 0; gIdx = gMgr = gHas = gApp = 0; gSelf = gSwapAt = gSwapOff = gSwapVal = gRemoveCalls = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(i32(o), a[3]);
    call.ecx = thisA;
    const auto k = kKind[c.fn];
    switch (k) {
    case kConst:
        if (c.fn == 2) { call.args[0] = a[4]; call.argCount = 1; }
        break;
    case kClone:
    case kDeath:
    case kGroup:
    case kHsClone:
    case kLmClone: {
        gMem = a[4] == 0 ? 0u : u32(arenaB.Obj(a[5]));
        gDtorWrite = a[6];
        gRet = CtorEax(a[8], gMem, a[7]);
        if (c.fn <= 13) *reinterpret_cast<u32*>(kEventPool) = u32(arenaD.Obj(a[9]));   // el pool de eventos solo se toca en las funciones de eventos (su region se restaura)
        for (u32 i = 0; i < kFieldCount[c.fn]; ++i) arenaA.Put32(i32(o + kFields[c.fn][i].off), a[10 + i]);
        if (k == kGroup) {
            arenaA.Put32(i32(o + 0x10), u32(arenaC.Obj(a[19])));
            arenaC.Put32(i32(a[19]), u32(arenaD.Obj(a[20])));
            for (u32 s = 0; s < kVtableSlots; ++s) arenaD.Put32(i32(a[20] + 4 * s), u32(uintptr_t(kInnerTable[s])));
            gInner = a[21];
        }
        break;
    }
    case kLmCtorId:
        for (u32 i = 0; i < 7; ++i) call.args[i] = a[4 + i];
        call.argCount = 7;
        gV0 = a[11]; gV4 = a[12]; gVC = a[13];
        break;
    case kLmCtorName: {
        PutString(arenaC, a[4], a[12], a[14]);
        if (!a[15]) PutString(arenaC, a[5], a[13], a[14] + 1u);
        call.args[0] = u32(arenaC.Obj(a[4])); call.args[1] = u32(arenaC.Obj(a[5]));
        for (u32 i = 0; i < 6; ++i) call.args[2 + i] = a[6 + i];
        call.argCount = 8;
        gV0 = a[16]; gV4 = a[17]; gVC = a[18]; gBlock = a[19]; gHier = a[20];
        break;
    }
    case kLmStart:
        arenaA.Put32(i32(o + 0x44), a[4]); arenaA.Put32(i32(o + 0x18), a[5]); arenaA.Put32(i32(o + 0x10), a[6]); arenaA.Put32(i32(o + 0x14), a[7]); arenaA.Put32(i32(o + 0x48), a[8]);
        arenaA.Put32(i32(o + 8), a[12]);
        arenaC.Put32(i32(a[9] + 0x18), a[10]);
        call.args[0] = u32(arenaC.Obj(a[9])); call.argCount = 1;
        gAssoc = a[11];
        break;
    case kSdd:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kBodyOnly:
        gRet = a[4]; gDtorWrite = a[5];
        break;
    case kLmProcess: {
        arenaA.Put32(i32(o + 0xC), a[4]);
        arenaA.Put32(i32(o + 8), a[5] ? u32(arenaD.Obj(a[6])) : 0u);
        arenaA.Put32(i32(o + 0x14), a[7]);
        arenaA.Put32(i32(o + 0x68), a[8]);
        arenaA.Put32(i32(o + 0x54), a[20]); arenaA.Put32(i32(o + 0x58), a[19]);
        arenaA.Put32(i32(o + 0x4C), a[21]); arenaA.Put32(i32(o + 0x50), a[22]); arenaA.Put32(i32(o + 0x5C), a[23]);
        arenaA.Put32(i32(o + 0x60), a[26]); arenaA.Put32(i32(o + 0x64), a[27]);
        // asociacion existente: tiempo actual +0x20, paso +0x28, dword +0x2C (BYTE +0x2E arriba)
        arenaD.Put32(i32(a[6] + 0x20), a[17]); arenaD.Put32(i32(a[6] + 0x28), a[18]); arenaD.Put32(i32(a[6] + 0x2C), a[24]);
        arenaD.Put32(i32(a[6] + 0x14), u32(arenaD.Obj(a[14])));
        // asociacion nueva (la que deja el falso de StartAnim): jerarquia +0x14, tiempo total +0x10 de la jerarquia
        arenaD.Put32(i32(a[13] + 0x14), u32(arenaD.Obj(a[14])));
        arenaD.Put32(i32(a[14] + 0x10), a[15]);
        arenaC.Put32(i32(a[9] + 0x18), a[10]);
        call.args[0] = u32(arenaC.Obj(a[9])); call.argCount = 1;
        gFound = a[11]; gTimerRet = a[12]; gNewAssoc = u32(arenaD.Obj(a[13]));
        *reinterpret_cast<u32*>(kClockMs) = a[16];
        break;
    }
    case kHsDtor: {
        const u32 h1 = u32(arenaC.Obj(a[6])), h2 = u32(arenaC.Obj(a[7])), h3 = u32(arenaC.Obj(0x700));
        for (const u32 h : {h1, h2, h3}) arenaC.Put32(i32(h - arenaC.Obj(0)), u32(arenaD.Obj(a[8])));
        for (u32 s = 0; s < kVtableSlots; ++s) arenaD.Put32(i32(a[8] + 4 * s), u32(uintptr_t(kHandTable[s])));
        arenaA.Put32(i32(o + 0x1C), a[4] ? h1 : 0u);
        arenaA.Put32(i32(o + 0x20), a[5] == 0 ? 0u : (a[5] == 1 ? h2 : h1));
        *reinterpret_cast<u32*>(kNoTempObjects) = a[9];
        gSelf = thisA; gSwapAt = a[10]; gSwapOff = a[11] ? 0x20u : 0x1Cu; gSwapVal = a[12] ? h3 : 0u;
        gDtorWrite = a[13]; gRet = a[14];
        break;
    }
    case kHsAbort:
        call.args[0] = a[4]; call.args[1] = a[5]; call.args[2] = a[6]; call.argCount = 3;
        gRet = a[7];
        break;
    case kInterior:
    case kIgCtor:
        break;
    case kEulerTo: {
        static constexpr u32 offs[9]{0, 4, 8, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28};
        for (u32 q = 0; q < 9; ++q) arenaA.Put32(i32(o + offs[q]), a[6 + q]);
        call.args[0] = PtrIn(a[15], a[16], o); call.args[1] = PtrIn(a[17], a[18], o); call.args[2] = PtrIn(a[19], a[20], o); call.args[3] = a[5];
        call.argCount = 4;
        break;
    }
    case kEulerFrom:
        call.args[0] = a[6]; call.args[1] = a[7]; call.args[2] = a[8]; call.args[3] = a[5];
        call.argCount = 4;
        break;
    case kScan: {
        arenaA.Put32(i32(o + 0x18 + 4 * a[5]), a[28]); arenaA.Put32(i32(o + 0x40 + 4 * a[5]), a[29]); arenaA.Put32(i32(o + 0x68 + 4 * a[5]), a[21]);
        for (u32 q = 0; q < 3; ++q) arenaC.Put32(i32(a[8] + 4 * q), a[9 + q]);
        arenaC.Put32(i32(a[8] + 0x34), a[30]);
        if (a[6]) arenaB.Put32(i32(a[7] + 0x14), u32(arenaB.Obj(a[27])));
        arenaD.Put32(i32(a[16] + 0x14), a[15] ? u32(arenaD.Obj(a[17])) : 0u);
        for (u32 q = 0; q < 3; ++q) {
            if (a[15]) arenaD.Put32(i32(a[17] + 0x30 + 4 * q), a[18 + q]);
            else arenaD.Put32(i32(a[16] + 4 + 4 * q), a[18 + q]);
        }
        *reinterpret_cast<u32*>(kRain) = a[4];
        memcpy(reinterpret_cast<void*>(kRadii + 4 * a[23]), &a[22], 4);
        gXf[0] = a[12]; gXf[1] = a[13]; gXf[2] = a[14]; gIdx = a[23]; gMgr = a[24]; gHas = a[25]; gApp = a[26];
        call.args[0] = u32(arenaC.Obj(a[8])); call.args[1] = a[6] ? u32(arenaB.Obj(a[7])) : 0u; call.args[2] = u32(arenaD.Obj(a[16])); call.argCount = 3;
        break;
    }
    case kBest:
        for (u32 q = 0; q < 10; ++q) { arenaA.Put32(i32(o + 0x18 + 4 * q), a[4 + q]); arenaA.Put32(i32(o + 0x40 + 4 * q), a[14 + q]); arenaA.Put32(i32(o + 0x68 + 4 * q), a[24 + q]); }
        call.args[0] = PtrIn(a[34], a[35], o); call.args[1] = PtrIn(a[36], a[37], o); call.argCount = 2;
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
        nev = 0; gRemoveCalls = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kClone: case kDeath: case kGroup: case kHsClone: case kLmClone: case kLmCtorId: case kLmCtorName: case kSdd: case kInterior: case kIgCtor:
            obs["ret"] = Tag(uintptr_t(eax)); break;                                    // puntero como etiqueta de arena
        case kConst: obs["ret"] = fn == 2 ? (eax & 0xFFu) : eax; break;                // valor de EAX (solo AL en la funcion que devuelve bool)
        case kLmProcess: case kHsAbort: obs["ret"] = eax & 0xFFu; break;               // bool: solo AL
        default: obs["ret"] = nullptr; break;                                          // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN202A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN202A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn202a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn202a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn202a"); }
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
    // Colaborador con hook RH en la DLL del que SOLO se falsea la direccion del EXE (la DLL llega por otra funcion: ver env.dllOnly); la ruta se crea para dejar el hook apagado
    auto ExeOnly = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador REAL con hook apagado (el EXE corre sus bytes y la DLL su cuerpo)
    auto Real = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return it->second;
    };
    // Cuerpo de la DLL de CEvent::operator new (0x4B5620): falso que anota el mismo evento que el EXE con su llamada directa a 0x4B5570
    {
        auto it = collabCache.find(0x4B5620);
        if (it == collabCache.end()) it = collabCache.emplace(0x4B5620, MakeCollab(env, 0x4B5620, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        env.dllOnly.push_back(S(Pair{0, it->second.dll, true}, reinterpret_cast<void*>(&FkOpNewEvent)));
    }
    gFinishCbDll = u32(uintptr_t(&CTaskSimpleAnim::FinishRunAnimCB));
    // eventos
    env.stubs[3] = {ExeOnly(0x4B5570, &FkPoolNewExe), F(0x4AC990, &Ctor<0x4AC990, 0xFFFF, ~0u, ~0u, ~0u, ~0u, 0xFFFF, 0xFFFF>::N7)};
    env.stubs[4] = {ExeOnly(0x4B5570, &FkPoolNewExe), F(0x4ACCF0, &Ctor<0x4ACCF0, 0xFFFF, ~0u, ~0u, ~0u, ~0u, 0xFFFF>::N6)};
    env.stubs[5] = {ExeOnly(0x4B5570, &FkPoolNewExe), F(0x4ACF00, &Ctor<0x4ACF00, 0xFFFF, ~0u, ~0u, ~0u, ~0u, 0xFFFF>::N6)};
    env.stubs[6] = {ExeOnly(0x4B5570, &FkPoolNewExe), F(0x4AD250, &Ctor<0x4AD250, ~0u, ~0u, 0xFF>::N3)};
    env.stubs[7] = {ExeOnly(0x4B5570, &FkPoolNewExe)};
    env.stubs[8] = {ExeOnly(0x4B5570, &FkPoolNewExe), F(0x4ADEA0, &Ctor<0x4ADEA0, ~0u, 0xFF, ~0u>::N3)};
    env.stubs[9] = {ExeOnly(0x4B5570, &FkPoolNewExe), F(0x4ADFD0, &Ctor<0x4ADFD0>::N2)};
    env.stubs[10] = {ExeOnly(0x4B5570, &FkPoolNewExe), F(0x4AE1A0, &Ctor<0x4AE1A0>::N1)};
    env.stubs[11] = {ExeOnly(0x4B5570, &FkPoolNewExe), F(0x4AE320, &Ctor<0x4AE320>::N2)};
    env.stubs[12] = {ExeOnly(0x4B5570, &FkPoolNewExe), F(0x4AE5D0, &Ctor<0x4AE5D0>::N2)};
    env.stubs[13] = {ExeOnly(0x4B5570, &FkPoolNewExe), F(0x4AE6E0, &Ctor<0x4AE6E0>::N3)};
    // tarea LoopedMiddle
    env.stubs[14] = {F(0x61A390, &FkBaseCtor)};
    env.stubs[15] = {F(0x61A390, &FkBaseCtor), F(0x4D3940, &FkGetAnimBlock), F(0x4D42F0, &FkGetAnim)};
    env.stubs[16] = {F(0x4D4410, &FkBlendHier), F(0x4D4610, &FkBlendId), F(0x4CEBE0, &FkSetFinishCb)};
    env.stubs[17] = {F(0x61A5A0, &FkTaskNew), Rt(14, &Ctor<0x61AC90>::N7), Rt(15, &Ctor<0x61AD10>::N8)};
    env.stubs[18] = {Rt(19, &FkBody<0x61BCA0>), F(0x61A5B0, &FkTaskDelete)};
    env.stubs[19] = {F(0x61A6F0, &FkBody<0x61A6F0>)};
    env.stubs[20] = {F(0x4D68B0, &FkGetAssoc), F(0x420E30, &FkIsOutOfTime), Rt(16, &FkStartAnimBody), F(0x4CEA80, &FkSetCurTime)};
    // tarea HandSignal
    env.stubs[22] = {F(0x563280, &FkWorldRemove), F(0x61A6F0, &FkBody<0x61A6F0>)};
    env.stubs[23] = {FV(0x61A790, &FkMakeAbort)};
    env.stubs[24] = {F(0x61A5A0, &FkTaskNew), F(0x61AE50, &Ctor<0x61AE50>::N4)};
    env.stubs[25] = {Rt(22, &FkBody<0x61AEB0>), F(0x61A5B0, &FkTaskDelete)};
    // escaner
    env.stubs[30] = {F(0x5334F0, &FkXform), F(0x6F9F60, &FkGetIndex), F(0x5EE190, &FkGetMgr), F(0x5EBB00, &FkHasEmpty), F(0x59BBC0, &FkMatAssign), F(0x59AED0, &FkSetScale), F(0x5EA220, &FkApproach)};
    env.reals[30] = {Real(0x411990), Real(0x59ACD0)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {0u, 1u, 2u, 21u, 26u, 27u, 28u, 29u, 31u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    for (unsigned f : {3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u}) dllReal[f] = "toda la funcion salvo el pool de eventos (0x4B5570 en el EXE; CEvent::operator new 0x4B5620 en la DLL, mismo evento) y el constructor del evento (falso identico)";
    dllReal[30] = "toda la funcion salvo los colaboradores falseados; CPlaceable::GetMatrix (0x411990) y el destructor de CMatrix (0x59ACD0 en el EXE, ~CMatrix en la DLL) corren reales (hook apagado) y la aritmetica x87 corre real en ambos lados";
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
        json dllOnly = json::array();
        for (const auto& s : env.dllOnly) dllOnly.push_back({{"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}, {"what", "CEvent::operator new 0x4B5620"}});
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", dllOnly}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada: puntero del pool de eventos, contador de objetos temporales, reloj, lluvia y radios; se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kEventPool, 4); for (unsigned f : {3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u}) regionsOf[f] = {regions.size() - 1};
    regions.emplace_back(kNoTempObjects, 4); regionsOf[22] = {regions.size() - 1};
    regions.emplace_back(kClockMs, 4); regionsOf[20] = {regions.size() - 1};
    regions.emplace_back(kRain, 4); regions.emplace_back(kRadii, kRadiiBytes); regionsOf[30] = {regions.size() - 2, regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}

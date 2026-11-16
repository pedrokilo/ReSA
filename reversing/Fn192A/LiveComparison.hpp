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
#include "AnimBlendAssociation.h"
#include "Core.hpp"

// Banco par a par Fn192A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 33 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos
// en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de destructor (0x678CC0, 0x4B0D00 y los otros
// nueve de eventos, 0x61FC50 (funcion probada), 0x59F660 (funcion probada), 0x61A3A0), CTask::operator delete 0x61A5B0, CEntity::CleanUpOldReference 0x571A00, CStreaming::SetMissionDoesntRequireModel 0x409C90, CTheScripts::
// RemoveFromWaitingForScriptBrainArray 0x46ABC0, CColStore::RemoveRef 0x4107D0, CRadar::ClearBlipForEntity 0x587C60, CBaseModelInfo::RemoveRef 0x4C4BB0, CTxdStore::RemoveRef 0x731A30, CObject::RemoveFromControlCodeList 0x59F450,
// CPathFind::SetLinksBridgeLights 0x44D960, CFire::Extinguish 0x5393F0, CMatrix::Destructor 0x59ACD0, CAnimBlendAssociation::SetFinishCallback 0x4CEBE0, PoolNew 0x552B50 (funcion probada) (todos con hook), y solo en la direccion del EXE
// (la DLL los llama por ella): CreateSubTask de CTaskComplexFallAndStayDown 0x678BD0 y operator new 0x82119A; rand() 0x821B1E en el EXE y CGeneral::GetRandomNumber en la DLL. CEvent::operator delete 0x4B5630 y CObject::operator delete
// 0x5A1F20 NO se falsean (el EXE libera el hueco EN LINEA): corren reales (hook apagado) en la DLL. Memoria global tocada (restaurada tras cada lado): marca 0xA9B0B0, punteros de los pools 0xB745BC / 0xB7449C / 0xB744AC,
// tabla de scripts 0xA90CF0, tabla de model infos 0xA9B0C8, contador de temporales 0xBB4A70 y MI_TRAINCROSSING1 0x8CD6B8.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria global, los eventos de los falsos (con sus
// argumentos), el valor devuelto (punteros como etiqueta de arena; EAX completo en la constante; sin valor en las funciones void) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en
// original_residue / candidate_residue (informativos).

namespace F192ALive {
using namespace F192AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F192AFix::Arena(WIN);
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
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;
inline void P32(uintptr_t addr, u32 v) { memcpy(reinterpret_cast<void*>(addr), &v, 4); }
inline void P16(uintptr_t addr, u32 v) { memcpy(reinterpret_cast<void*>(addr), &v, 2); }
inline void P8(uintptr_t addr, u32 v) { *reinterpret_cast<u8*>(addr) = u8(v); }
inline u32 G32(uintptr_t addr) { u32 v; memcpy(&v, reinterpret_cast<const void*>(addr), 4); return v; }

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos
inline u32 gDtorWrite{};             // dword que el falso de un cuerpo deja en [this]
inline u32 gType{};                  // lo que devuelve el falso de GetTaskType de la subtarea
inline std::array<u32, 3> gRand{};   // lo que devuelve rand() la primera, la segunda y la tercera vez
inline unsigned nRand{};
inline std::array<u32, 2> gAlloc{};  // lo que devuelve operator new la primera y la segunda vez
inline unsigned nAlloc{};
inline uintptr_t gAsi{};             // modulo del ASI (para reconocer las vtables de la DLL)
inline DWORD gBenchThread{};

// Cuerpo / destructor sin argumentos de pila (thiscall): evento [direccion del EXE, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Los destructores con borrado de los eventos del EXE usan ECX tras llamar al cuerpo (`mov edx,ecx; sub edx,ebx`): el falso de cada cuerpo lo conserva (envoltorio sin marco que guarda ECX).
#define F192A_KEEP(NAME, ADDR) \
    inline void* const NAME##Target = reinterpret_cast<void*>(&FkBody<ADDR>); \
    __declspec(naked) static void NAME() { __asm { push ecx } __asm { call dword ptr [NAME##Target] } __asm { pop ecx } __asm { ret } }
F192A_KEEP(FkKeep0, 0x4B0D00) F192A_KEEP(FkKeep1, 0x4B0DA0) F192A_KEEP(FkKeep2, 0x4B1170) F192A_KEEP(FkKeep3, 0x4B1320) F192A_KEEP(FkKeep4, 0x4B1520)
F192A_KEEP(FkKeep5, 0x4B17F0) F192A_KEEP(FkKeep6, 0x4B1990) F192A_KEEP(FkKeep7, 0x4B1C10) F192A_KEEP(FkKeep8, 0x4B1D50) F192A_KEEP(FkKeep9, 0x4B1F00)
inline const std::array<void*, 10> kKeepFakes{reinterpret_cast<void*>(&FkKeep0), reinterpret_cast<void*>(&FkKeep1), reinterpret_cast<void*>(&FkKeep2), reinterpret_cast<void*>(&FkKeep3), reinterpret_cast<void*>(&FkKeep4),
                                              reinterpret_cast<void*>(&FkKeep5), reinterpret_cast<void*>(&FkKeep6), reinterpret_cast<void*>(&FkKeep7), reinterpret_cast<void*>(&FkKeep8), reinterpret_cast<void*>(&FkKeep9)};
// Funcion thiscall sin argumentos que no toca memoria (RemoveRef de model info, RemoveFromControlCodeList, Extinguish, New del pool): evento [direccion, ECX (etiqueta)]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkThis(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(self);
    return gRet;
}
// operator delete de tareas (0x61A5B0; cdecl, un argumento): evento [direccion, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// Igual, para la tarea cuyo destructor real deja la vtable de la DLL en [this]: el dword se anota como constante si apunta al modulo del ASI.
inline void __cdecl FkDeleteVt(void* p) {
    Ev& e = PushEv(0x61A5B0, 2);
    e.v[0] = Tag(p);
    const u32 bits = BitsOf(p);
    e.v[1] = ModuleOf(bits) == gAsi ? 0xD11D11D1u : bits;
}
// CEntity::CleanUpOldReference (0x571A00; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)].
inline void __fastcall FkRef(void* self, void*, void* ref) {
    Ev& e = PushEv(0x571A00, 2);
    e.v[0] = Tag(self);
    e.v[1] = Tag(ref);
}
// Funciones cdecl con uno o dos dwords de pila (SetMissionDoesntRequireModel, CColStore::RemoveRef, CTxdStore::RemoveRef; RemoveFromWaitingForScriptBrainArray y ClearBlipForEntity): evento [direccion, argumentos].
template<u32 ADDR> inline void __cdecl FkCdecl1(u32 a) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = a;
}
template<u32 ADDR> inline void __cdecl FkCdecl2(u32 a, u32 b) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = a; e.v[1] = b;
}
// RemoveFromWaitingForScriptBrainArray (0x46ABC0; cdecl, entidad y un int16): evento [direccion, entidad (etiqueta), los 16 bits bajos].
inline void __cdecl FkWaiting(void* entity, u32 index) {
    Ev& e = PushEv(0x46ABC0, 2);
    e.v[0] = Tag(entity);
    e.v[1] = index & 0xFFFFu;
}
// CPathFind::SetLinksBridgeLights (0x44D960; thiscall, cuatro floats y un bool, `ret 14h`): evento [direccion, ECX (crudo), bits de los cuatro floats, byte del bool].
inline void __fastcall FkBridge(void* self, void*, u32 a, u32 b, u32 c, u32 d, u32 e5) {
    Ev& e = PushEv(0x44D960, 6);
    e.v[0] = u32(uintptr_t(self)); e.v[1] = a; e.v[2] = b; e.v[3] = c; e.v[4] = d; e.v[5] = e5 & 0xFFu;
}
// CAnimBlendAssociation::SetFinishCallback (0x4CEBE0; thiscall, funcion y datos, `ret 8`): evento [direccion, ECX (etiqueta), 1 si la funcion es CDefaultAnimCallback::DefaultAnimCB (0x4D1390 en el EXE, la funcion de la DLL en la DLL), datos].
inline void __fastcall FkFinishCb(void* self, void*, u32 callback, u32 data) {
    Ev& e = PushEv(0x4CEBE0, 3);
    e.v[0] = Tag(self);
    e.v[1] = (callback == 0x4D1390u || callback == u32(reinterpret_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB))) ? 1u : 0xBADu;
    e.v[2] = data;
}
// GetTaskType de la subtarea (ranura 4; thiscall, sin argumentos): evento [0x67CBE0 + 4, ECX (etiqueta)]; devuelve gType.
inline u32 __fastcall FkGetType(void* self, void*) {
    Ev& e = PushEv(0x67CBE4, 1);
    e.v[0] = Tag(self);
    return gType;
}
// GetSubTask de la tarea compleja (ranura 2; thiscall const, sin argumentos): devuelve [this + 8]. El EXE no lo llama; la DLL puede llamarlo por la vtable del objeto (vtable falsa en la arena C).
inline u32 __fastcall FkGetSub(void* self, void*) {
    return BitsOf(static_cast<const u8*>(self) + 8);
}
// CTaskComplexFallAndStayDown::CreateSubTask (0x678BD0; thiscall, un tipo, `ret 4`): evento [direccion, ECX (etiqueta), tipo]; devuelve gRet.
inline u32 __fastcall FkCreateSub(void* self, void*, u32 type) {
    Ev& e = PushEv(0x678BD0, 2);
    e.v[0] = Tag(self); e.v[1] = type;
    return gRet;
}
// operator new (0x82119A; cdecl, un tamano): evento [direccion, tamano]; devuelve gAlloc la primera y la segunda vez. Solo en el hilo del banco (los demas hilos reciben malloc del CRT).
inline u32 __cdecl FkOpNew(u32 size) {
    if (GetCurrentThreadId() != gBenchThread) return reinterpret_cast<u32(__cdecl*)(u32)>(0x824257)(size);
    Ev& e = PushEv(0x82119A, 1);
    e.v[0] = size;
    return gAlloc[nAlloc < 2 ? nAlloc++ : 1];
}
// rand() del CRT (0x821B1E en el EXE; CGeneral::GetRandomNumber en la DLL): evento [0x821B1E]; devuelve gRand (los llamadores usan sus 16 bits bajos).
inline u32 __cdecl FkRand() {
    if (GetCurrentThreadId() != gBenchThread) return 0;
    PushEv(0x821B1E, 0);
    return gRand[nRand < 3 ? nRand++ : 2];
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

inline uintptr_t ObjectC(const C& c) { return arenaC.Obj(c.a[7] + c.a[6] * (kKind[c.fn] == kEvSdd ? kEventSize : kObjSize) + (kKind[c.fn] == kEvSdd ? c.a[10] : 0u)); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[kSeedAt]); arenaB.Pattern(a[2], a[kSeedAt + 1]); arenaC.Pattern(a[2], a[kSeedAt + 2]); arenaD.Pattern(a[2], a[kSeedAt + 3]);
    gRet = 0; gDtorWrite = 0; gType = 0; gRand = {}; nRand = 0; gAlloc = {}; nAlloc = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    const bool objDtor = kKind[c.fn] == kObjDtor;
    if (!objDtor) arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kFall: {
        const u32 sub = u32(arenaB.Obj(a[5])), vt = u32(arenaC.Obj(a[6])), vt2 = vt + 0x40;
        P32(sub, vt); P32(vt + 0x10, u32(uintptr_t(&FkGetType)));
        P32(vt2 + 8, u32(uintptr_t(&FkGetSub))); arenaA.Put32(o, vt2);      // vtable propia de `this` (solo la usa la DLL si GetSubTask es una llamada virtual)
        arenaA.Put32(o + 8, sub);
        gType = a[4]; gRet = a[7];
        call.args[0] = a[8]; call.argCount = 1;
        break;
    }
    case kSddTask:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kSddThrow: {
        const u32 entity = a[4] == 0 ? 0u : (a[4] == 1 ? u32(arenaB.Obj(a[5])) : a[7]);
        arenaA.Put32(o + 0xC, entity);
        call.args[0] = a[6]; call.argCount = 1;
        break;
    }
    case kQtCtor:
        for (u32 i = 0; i < 4; ++i) arenaB.Put32(a[4] + 4 * i, a[5 + i]);
        call.args[0] = u32(arenaB.Obj(a[4])); call.args[1] = a[9]; call.argCount = 2;
        break;
    case kPoolCtor: {
        const i32 size = i32(a[4]);
        gAlloc[0] = size > 0 ? u32(arenaC.Obj(a[6])) : a[9];
        gAlloc[1] = size > 0 ? u32(arenaD.Obj(a[7])) : a[10];
        call.args[0] = a[4]; call.args[1] = a[5]; call.argCount = 2;
        break;
    }
    case kPoolNew:
        arenaA.Put32(o + 0, u32(arenaC.Obj(a[4]))); arenaA.Put32(o + 4, u32(arenaD.Obj(a[5]))); arenaA.Put32(o + 8, a[6]); arenaA.Put32(o + 0xC, a[7]);
        for (size_t i = 0; i < c.s.size(); ++i) arenaD.Put8(a[5] + i, c.s[i]);
        break;
    case kAlloc:
        P32(kQtPoolPtr, u32(arenaB.Obj(a[4])));
        gRet = a[5];
        break;
    case kPoolDel: {
        arenaA.Put32(o + 0, u32(arenaC.Obj(a[4]))); arenaA.Put32(o + 4, u32(arenaD.Obj(a[5]))); arenaA.Put32(o + 8, a[6]); arenaA.Put32(o + 0xC, a[7]);
        uint8* state = arenaD.m.data() + kGuard + a[5] + a[8];
        *state = u8((*state & 0x7Fu) | (a[10] ? 0x80u : 0u));
        const u32 node = u32(arenaC.Obj(a[4] + a[8] * kNodeSize + a[9]));
        call.args[0] = node; call.argCount = 1;
        if (c.fn == 7) { P32(kQtPoolPtr, thisA); call.popBytes = 4; }
        break;
    }
    case kRandRect:
        for (u32 i = 0; i < 4; ++i) arenaA.Put32(o + 4 * i, a[4 + i]);
        gRand = {a[8], a[9], 0};
        call.args[0] = u32(arenaB.Obj(a[10])); call.argCount = 1;
        break;
    case kRandBox:
        for (u32 i = 0; i < 6; ++i) arenaA.Put32(o + 4 * i, a[4 + i]);
        gRand = {a[10], a[11], a[12]};
        call.args[0] = u32(arenaB.Obj(a[13])); call.argCount = 1;
        break;
    case kNop:
        *reinterpret_cast<u8*>(kMarker) = 0x5A;   // marca que la prueba negativa (mutate.py) lee para que las funciones vacias tengan algo observable; el juego real nunca la pone
        break;
    case kNoWrite:
    case kPedCtor:
    case kConst:
        break;
    case kCreateAtRef:
        arenaA.Put32(o + 0, u32(arenaC.Obj(a[4]))); arenaA.Put32(o + 4, u32(arenaD.Obj(a[5]))); arenaA.Put32(o + 8, a[6]); arenaA.Put32(o + 0xC, a[7]);
        for (size_t i = 0; i < c.s.size(); ++i) arenaD.Put8(a[5] + i, c.s[i]);
        call.args[0] = (a[8] << 8) | (a[9] & 0xFFu); call.argCount = 1;
        break;
    case kObjDtor:
    case kObjSdd: {
        const u32 pool = u32(arenaB.Obj(a[4]));
        const u32 thisC = u32(ObjectC(c));
        P32(pool + 0, u32(arenaC.Obj(a[7]))); P32(pool + 4, u32(arenaD.Obj(a[8]))); P32(pool + 8, a[5]); P32(pool + 0xC, a[9]);
        for (size_t i = 0; i < c.s.size(); ++i) arenaD.Put8(a[8] + i, c.s[i]);
        P32(kObjectPoolPtr, pool);
        call.ecx = thisC;
        if (kKind[c.fn] == kObjSdd) {
            gRet = a[12]; gDtorWrite = a[13];
            call.args[0] = a[11]; call.argCount = 1;
            break;
        }
        for (u32 off : {0x14u, 0x18u, 0x24u, 0x30u, 0xB0u, 0x134u}) P32(thisC + off, 0);
        P32(thisC + 0x140, a[10]); P16(thisC + 0x168, a[11]); P16(thisC + 0x22, a[12]); P16(thisC + 0x14A, a[13]); P16(thisC + 0x16A, a[14]); P8(thisC + 0x13C, a[15]); P32(thisC + 0x164, a[16]);
        const u32 dummy = u32(arenaA.Obj(a[17])), matrix = u32(arenaA.Obj(a[18]));
        P32(thisC + 0x170, dummy);
        for (u32 i = 0; i < 3; ++i) { P32(dummy + 4 + 4 * i, a[20 + i]); P32(matrix + 0x30 + 4 * i, a[23 + i]); }
        P32(dummy + 0x14, a[19] ? matrix : 0u);
        const u32 mi1 = u32(arenaB.Obj(a[28])), cm1 = u32(arenaB.Obj(a[29])), mi2 = u32(arenaB.Obj(a[30])), cm2 = u32(arenaB.Obj(a[31]));
        P32(mi1 + 0x14, cm1); P8(cm1 + 0x28, a[32]); P32(mi2 + 0x14, cm2); P8(cm2 + 0x28, a[33]);
        if (a[10] & 0x10000u) P32(kModelTable + 4 * (a[12] & 0xFFFFu), mi1);
        if (a[13] != 0xFFFFu) P32(kModelTable + 4 * (a[13] & 0xFFFFu), mi2);
        if (a[11] < kScriptCount) P16(kScriptTable + kScriptStride * a[11], a[34]);
        P32(kTempObjects, a[26]); P32(kTrainCrossing, a[27]);
        break;
    }
    case kCentre:
        call.args[0] = a[4]; call.args[1] = a[5]; call.args[2] = a[6]; call.argCount = 3;
        break;
    case kMatDtor:
        gRet = a[4]; gDtorWrite = a[5];
        break;
    case kEvSdd: {
        const u32 pool = u32(arenaB.Obj(a[4]));
        P32(pool + 0, u32(arenaC.Obj(a[7]))); P32(pool + 4, u32(arenaD.Obj(a[8]))); P32(pool + 8, a[5]); P32(pool + 0xC, a[9]);
        for (size_t i = 0; i < c.s.size(); ++i) arenaD.Put8(a[8] + i, c.s[i]);
        P32(kEventPoolPtr, pool);
        gRet = a[12]; gDtorWrite = a[13];
        call.ecx = u32(ObjectC(c));
        call.args[0] = a[11]; call.argCount = 1;
        break;
    }
    case kKicked: {
        const u32 anim = a[4] ? u32(arenaB.Obj(a[5])) : 0u;
        arenaA.Put32(o + 0xC, anim);
        gRet = a[6]; gDtorWrite = a[7];
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
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        json obs;
        // Normalizaciones documentadas: la vtable que deja un destructor compilado de la DLL es la de la DLL (el EXE deja la suya): se anota como constante.
        if (kKind[fn] == kSddThrow) {
            const uintptr_t self = arenaA.Obj(c.a[0]);
            if (ModuleOf(G32(self)) == gAsi) P32(self, 0xD11D11D1u);
        }
        if (kKind[fn] == kObjDtor) {
            // ~CPhysical -> ~CEntity -> ~CPlaceable corren de verdad en ambos lados (el EXE entra por el hook 0x542450 y la DLL por la llamada de la clase base): se anota que dejaron la vtable de la DLL
            // y se descartan los bytes que esa cadena escribe (+0, +0x14 y +0x1C) para compararlos igual en los dos lados.
            const uintptr_t self = ObjectC(c);
            obs["chain_vtable_dll"] = ModuleOf(G32(self)) == gAsi;
            const size_t base = size_t(self - uintptr_t(arenaC.m.data()));
            for (size_t off : {size_t(0), size_t(0x14), size_t(0x1C)}) memcpy(arenaC.m.data() + base + off, arenaC.r.data() + base + off, 4);
        }
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kFall: case kSddTask: case kSddThrow: case kQtCtor: case kPoolCtor: case kPoolNew: case kAlloc: case kObjSdd: case kPedCtor: case kEvSdd:
            obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case kConst: obs["ret"] = eax; break;           // valor completo de EAX
        default: obs["ret"] = nullptr; break;           // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN192A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN192A_CASES", buffer, sizeof(buffer)) > 0) {
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
        gAsi = env.asi;
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
    if (!std::filesystem::exists(dir / "fn192a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn192a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn192a"); }
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
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE: el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH cuya llamada, en la DLL, va a una funcion propia (direccion dada) y no a la del EXE: el falso va en la direccion del EXE y en esa funcion (rand: CGeneral::GetRandomNumber)
    auto D = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        Pair p{exe, Body(dllAddress), false};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador REAL con hook apagado: el EXE corre sus bytes (si los llama) y la DLL su cuerpo
    auto Real = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return it->second;
    };
    const uintptr_t randDll = std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber);
    env.stubs[0] = {ByExeAddress(0x678BD0, &FkCreateSub)};
    env.stubs[1] = {F(0x678CC0, &FkBody<0x678CC0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[3] = {ByExeAddress(0x82119A, &FkOpNew)};
    env.stubs[6] = {Rt(4, &FkThis<0x552B50>)};
    env.stubs[8] = {D(0x821B1E, randDll, &FkRand)};
    env.stubs[9] = {D(0x821B1E, randDll, &FkRand)};
    env.stubs[13] = {F(0x409C90, &FkCdecl1<0x409C90>), F(0x46ABC0, &FkWaiting), F(0x4107D0, &FkCdecl1<0x4107D0>), F(0x587C60, &FkCdecl2<0x587C60>), F(0x4C4BB0, &FkThis<0x4C4BB0>),
                     F(0x731A30, &FkCdecl1<0x731A30>), F(0x59F450, &FkThis<0x59F450>), F(0x44D960, &FkBridge), F(0x5393F0, &FkThis<0x5393F0>)};
    env.stubs[14] = {Rt(13, &FkBody<0x59F660>)};
    env.reals[14] = {Real(0x5A1F20)};
    env.stubs[18] = {F(0x59ACD0, &FkBody<0x59ACD0>)};
    for (unsigned k = 0; k < 10; ++k) {
        env.stubs[20 + k] = {F(kEventBody[k], kKeepFakes[k])};
        env.reals[20 + k] = {Real(0x4B5630)};
    }
    env.stubs[30] = {F(0x571A00, &FkRef), F(0x61A5B0, &FkDeleteVt)};
    env.stubs[31] = {Rt(32, &FkBody<0x61FC50>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[32] = {F(0x4CEBE0, &FkFinishCb), F(0x61A3A0, &FkBody<0x61A3A0>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {2u, 4u, 5u, 7u, 10u, 11u, 12u, 15u, 16u, 17u, 19u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[13] = "toda la funcion salvo los falsos; ~CPhysical (0x542450 con hook en el EXE, el destructor de la clase base en la DLL) corre real en ambos lados con los campos que usa a cero";
    dllReal[30] = "toda la funcion salvo los falsos; el destructor de la clase (0x61F950) corre real en ambos lados (el EXE entra por el hook, la DLL por la llamada directa)";
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
    // Memoria global del juego tocada: se guarda ahora y se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    const auto add = [&](uintptr_t at, size_t bytes) { regions.emplace_back(at, bytes); return regions.size() - 1; };
    const size_t rMarker = add(kMarker, 1), rQt = add(kQtPoolPtr, 4), rObj = add(kObjectPoolPtr, 4), rEv = add(kEventPoolPtr, 4), rScript = add(kScriptTable, kScriptCount * kScriptStride),
                 rModel = add(kModelTable, kModelCount * 4), rTemp = add(kTempObjects, 4), rTrain = add(kTrainCrossing, 4);
    regionsOf[10] = regionsOf[11] = {rMarker};
    regionsOf[6] = regionsOf[7] = {rQt};
    regionsOf[13] = {rObj, rScript, rModel, rTemp, rTrain};
    regionsOf[14] = {rObj};
    for (unsigned k = 0; k < 10; ++k) regionsOf[20 + k] = {rEv};
    Run(env, ready, registered, bodies, collaborators);
}
}

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
#include <utility>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"
#include "Tasks/TaskTypes/TaskComplexExtinguishFireOnFoot.h"

// Banco par a par Fn193A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 34 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos
// sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (F / Rt) o solo en la direccion del EXE
// cuando la DLL llama a esa direccion (X): cuerpos de los destructores 0x620370, 0x620810, 0x620EF0, 0x620FF0, 0x621090, 0x6588A0 y 0x658AE0 y las bases CTask::Destructor 0x61A3A0 y ~CTaskComplex 0x61A3D0, ~CTaskSimpleAnim
// 0x61A6F0, CTask::operator new 0x61A5A0 y delete 0x61A5B0, SetFinishCallback 0x4CEBE0, CleanUpOldReference 0x571A00, CreateSubTask 0x620C30 de CTaskComplexReactToAttack, IsPlayer 0x5DF8F0, GetPad 0x53FB70, GetTarget
// 0x540670, ApplyMoveForce 0x5429F0, CVector2D::Normalise 0x44E480 (solo EXE), los constructores de CTaskSimpleGunControl 0x61F3F0 y CTaskComplexGoToPointAndStandStill 0x668120, CFireManager::FindNearestFire 0x538F40,
// FxManager_c::Update 0x4A9A80, Render 0x4A92A0, CreateFxSystem 0x4A9BE0 y DestroyFxSystem 0x4A9810, y las funciones propias del lote 0x61FE70, 0x620B90, 0x659E00, 0x6598B0 y 0x659990. Reales en ambos lados: la base
// de CTaskComplex (0x61A3B0 / 0x61A3D0) en el cuerpo del destructor de CTaskComplexUseWaterCannon y en el constructor y el Clone de CTaskComplexExtinguishFireOnFoot (subtarea sintetica con tabla virtual de falsos; la vtable
// resultante se clasifica porque la de la DLL no tiene la direccion de la del EXE) y la lista de Fx_c::Constructor.
// Memoria global del juego tocada: ninguna. Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos
// (con sus argumentos), el valor devuelto (punteros como etiqueta de arena; EAX completo en constantes; solo AL en bool), la clase de la vtable que deja un constructor / destructor real y el estado x87 (siempre vacio).

namespace F193ALive {
using namespace F193AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F193AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { const uint16 w = uint16(v); memcpy(m.data() + kGuard + off, &w, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
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
inline u32 gNew{};                   // memoria que devuelve el falso de CTask::operator new (0 = sin memoria)
inline u32 gFire{};                  // lo que devuelven los falsos de FindNearestFire (0x659990 y 0x538F40)
inline u32 gTaskType{};              // lo que devuelve la ranura 4 de la subtarea sintetica
inline u32 gVirt{};                  // lo que devuelve la ranura 9 del objeto sintetico
inline u32 gMutate{};                // el falso de CFireManager::FindNearestFire reescribe la posicion del ped
inline std::array<u32, 3> gNewPos{};
inline std::array<u32, 2> gIsP{};    // EAX de las dos llamadas de IsPlayer
inline unsigned gIsPCount{};
inline u32 gPad{}, gTarget{};        // CPad que devuelve GetPad y EAX de GetTarget
inline u32 gNx{}, gNy{}, gNormMask{}, gPedPtr{};   // lo que escribe el falso de Normalise y mascara XOR sobre +0x46C del ped
inline u32 gFxSeed{}, gFxCount{};    // semilla y contador de los valores que devuelve CreateFxSystem
inline u32 gDllExtVt{}, gDllBaseVt{};   // vtable de CTaskComplexExtinguishFireOnFoot de la DLL y la base final que deja su destructor (aprendidas al empezar)
inline DWORD gBenchThread{};

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
// Cuerpo del destructor sin argumentos (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CAnimBlendAssociation::SetFinishCallback (0x4CEBE0; thiscall, dos argumentos, `ret 8`): evento [direccion, ECX (etiqueta), funcion, datos, dword de +0x1C de la animacion]. No escribe nada.
inline void __fastcall FkSfc(void* self, void*, u32 cb, u32 data) {
    Ev& e = PushEv(0x4CEBE0, 4);
    e.v[0] = Tag(self); e.v[1] = cb; e.v[2] = data;
    memcpy(&e.v[3], static_cast<const u8*>(self) + 0x1C, 4);
}
// CEntity::CleanUpOldReference (0x571A00; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (crudo: la entidad no se lee), argumento (etiqueta)].
inline void __fastcall FkClean(void* self, void*, u32 ref) {
    Ev& e = PushEv(0x571A00, 2);
    e.v[0] = u32(uintptr_t(self)); e.v[1] = Tag(uintptr_t(ref));
}
// CTaskComplexReactToAttack::CreateSubTask (0x620C30; thiscall, un tipo de pila, `ret 4`): evento [direccion, ECX (etiqueta), tipo]; devuelve gRet.
inline u32 __fastcall FkRtaCreate(void* self, void*, u32 type) {
    Ev& e = PushEv(0x620C30, 2);
    e.v[0] = Tag(self); e.v[1] = type;
    return gRet;
}
// Ranura 9 (CreateFirstSubTask) de la tabla virtual sintetica de `this` (thiscall, un puntero de pila, `ret 4`): evento [0xD1000109, ECX (etiqueta), ped (etiqueta)]; devuelve gVirt.
inline u32 __fastcall FkVirt9(void* self, void*, u32 ped) {
    Ev& e = PushEv(0xD1000109u, 2);
    e.v[0] = Tag(self); e.v[1] = TagArena(uintptr_t(ped));
    return gVirt;
}
// Constructor de CTaskSimpleGunControl (0x61F3F0; thiscall, seis dwords de pila, `ret 18h`): evento [direccion, ECX (etiqueta), entidad (etiqueta), objetivo (etiqueta), destino (etiqueta), orden, rafaga, duracion];
// deja gDtorWrite en [ECX]; devuelve gRet.
inline u32 __fastcall FkGunCtor(void* self, void*, u32 a, u32 b, u32 c, u32 d, u32 e2, u32 f) {
    Ev& e = PushEv(0x61F3F0, 7);
    e.v[0] = Tag(self); e.v[1] = TagArena(uintptr_t(a)); e.v[2] = TagArena(uintptr_t(b)); e.v[3] = TagArena(uintptr_t(c)); e.v[4] = d; e.v[5] = e2; e.v[6] = f;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor de CTaskComplexGoToPointAndStandStill (0x668120; thiscall, seis dwords de pila, `ret 18h`): evento [direccion, ECX (etiqueta), estado, punto (etiqueta), radio, radio de estado, bandera, bandera];
// deja gDtorWrite en [ECX]; devuelve gRet.
inline u32 __fastcall FkGoToCtor(void* self, void*, u32 a, u32 b, u32 c, u32 d, u32 e2, u32 f) {
    Ev& e = PushEv(0x668120, 7);
    e.v[0] = Tag(self); e.v[1] = TagArena(uintptr_t(a)); e.v[2] = TagArena(uintptr_t(b)); e.v[3] = c; e.v[4] = d; e.v[5] = e2; e.v[6] = f;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CTaskComplexExtinguishFireOnFoot::FindNearestFire (0x659990; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), ped (etiqueta)]; devuelve gFire.
inline u32 __fastcall FkFindFire(void* self, void*, u32 ped) {
    Ev& e = PushEv(0x659990, 2);
    e.v[0] = Tag(self); e.v[1] = TagArena(uintptr_t(ped));
    return gFire;
}
// CFireManager::FindNearestFire (0x538F40; thiscall, tres dwords de pila, `ret 0Ch`): evento [direccion, ECX (crudo), posicion (etiqueta), x, y, z, byte de la 1a bandera, byte de la 2a]; con gMutate reescribe los tres dwords
// de la posicion con gNewPos (despues de anotarlos); devuelve gFire.
inline u32 __fastcall FkFmFind(void* self, void*, u32 pos, u32 a, u32 b) {
    Ev& e = PushEv(0x538F40, 7);
    e.v[0] = u32(uintptr_t(self)); e.v[1] = Tag(uintptr_t(pos));
    memcpy(&e.v[2], reinterpret_cast<const void*>(uintptr_t(pos)), 12);
    e.v[5] = a & 0xFFu; e.v[6] = b & 0xFFu;
    if (gMutate) memcpy(reinterpret_cast<void*>(uintptr_t(pos)), gNewPos.data(), 12);
    return gFire;
}
// CPed::IsPlayer (0x5DF8F0; thiscall, sin argumentos, `ret`): evento [direccion, ECX (etiqueta)]; devuelve gIsP[numero de llamada].
inline u32 __fastcall FkIsPlayer(void* self, void*) {
    Ev& e = PushEv(0x5DF8F0, 1);
    e.v[0] = Tag(self);
    return gIsP[gIsPCount++ & 1u];
}
// CPad::GetPad (0x53FB70; cdecl, un entero): evento [direccion, argumento]; devuelve gPad.
inline u32 __cdecl FkGetPad(u32 n) {
    Ev& e = PushEv(0x53FB70, 1);
    e.v[0] = n;
    return gPad;
}
// CPad::GetTarget (0x540670; thiscall, sin argumentos, `ret`): evento [direccion, ECX (etiqueta)]; devuelve gTarget.
inline u32 __fastcall FkGetTarget(void* self, void*) {
    Ev& e = PushEv(0x540670, 1);
    e.v[0] = Tag(self);
    return gTarget;
}
// CVector2D::Normalise (0x44E480; thiscall, sin argumentos, `ret`; solo en el EXE): evento [direccion, ECX (etiqueta), x, y]; escribe gNx y gNy y aplica gNormMask con XOR a [ped+0x46C].
inline void __fastcall FkNorm(void* self, void*) {
    Ev& e = PushEv(0x44E480, 3);
    e.v[0] = Tag(self); memcpy(&e.v[1], self, 4); memcpy(&e.v[2], static_cast<const u8*>(self) + 4, 4);
    memcpy(self, &gNx, 4); memcpy(static_cast<u8*>(self) + 4, &gNy, 4);
    if (gPedPtr) *reinterpret_cast<u32*>(uintptr_t(gPedPtr) + 0x46C) ^= gNormMask;
}
// CPhysical::ApplyMoveForce (0x5429F0; thiscall, tres dwords de pila, `ret 0Ch`): evento [direccion, ECX (etiqueta), x, y, z, dword de +0x46C del ped].
inline void __fastcall FkMoveForce(void* self, void*, u32 x, u32 y, u32 z) {
    Ev& e = PushEv(0x5429F0, 5);
    e.v[0] = Tag(self); e.v[1] = x; e.v[2] = y; e.v[3] = z; memcpy(&e.v[4], static_cast<const u8*>(self) + 0x46C, 4);
}
// FxManager_c::Update (0x4A9A80; thiscall, dos dwords de pila, `ret 8`): evento [direccion, ECX (crudo), camara, tiempo].
inline void __fastcall FkFxUpdate(void* self, void*, u32 cam, u32 dt) {
    Ev& e = PushEv(0x4A9A80, 3);
    e.v[0] = u32(uintptr_t(self)); e.v[1] = cam; e.v[2] = dt;
}
// FxManager_c::Render (0x4A92A0; thiscall, dos dwords de pila, `ret 8`): evento [direccion, ECX (crudo), camara, byte de la bandera].
inline void __fastcall FkFxRender(void* self, void*, u32 cam, u32 flag) {
    Ev& e = PushEv(0x4A92A0, 3);
    e.v[0] = u32(uintptr_t(self)); e.v[1] = cam; e.v[2] = flag & 0xFFu;
}
// FxManager_c::CreateFxSystem (0x4A9BE0; thiscall, cuatro dwords de pila, `ret 10h`): evento [direccion, ECX (crudo), FNV-1a del nombre, x, y, z del punto, matriz (cruda), byte de la bandera]; devuelve un valor de la serie de gFxSeed.
inline u32 __fastcall FkCreateFx(void* self, void*, u32 name, u32 point, u32 matrix, u32 flag) {
    Ev& e = PushEv(0x4A9BE0, 7);
    e.v[0] = u32(uintptr_t(self)); e.v[1] = FnvStr(reinterpret_cast<const char*>(uintptr_t(name)));
    memcpy(&e.v[2], reinterpret_cast<const void*>(uintptr_t(point)), 12);
    e.v[5] = matrix; e.v[6] = flag & 0xFFu;
    return Mix(gFxSeed + gFxCount++ * 0x9E3779B1u);
}
// FxManager_c::DestroyFxSystem (0x4A9810; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (crudo), argumento (crudo)].
inline void __fastcall FkDestroyFx(void* self, void*, u32 fx) {
    Ev& e = PushEv(0x4A9810, 2);
    e.v[0] = u32(uintptr_t(self)); e.v[1] = fx;
}
// Subtarea sintetica: tabla virtual de 20 ranuras en la arena D. Ranura 0 = destructor con borrado (thiscall, banderas, `ret 4`), ranura 4 = GetTaskType; el resto son trampas que anotan el uso.
inline u32 __fastcall FkSubDtor(void* self, void*, u32 flags) {
    Ev& e = PushEv(0xD1000000u, 2);
    e.v[0] = Tag(self);
    e.v[1] = flags;
    return u32(uintptr_t(self));
}
inline u32 __fastcall FkSubType(void* self, void*) {
    Ev& e = PushEv(0xD1000004u, 1);
    e.v[0] = Tag(self);
    return gTaskType;
}
template<unsigned N> inline u32 __fastcall FkTrap(void* self, void*) {
    Ev& e = PushEv(0xD1000100u + N, 1);
    e.v[0] = Tag(self);
    return 0;
}
template<size_t... I> inline void FillSlots(std::array<void*, 20>& t, std::index_sequence<I...>) { ((t[I] = reinterpret_cast<void*>(&FkTrap<unsigned(I)>)), ...); }
// Tabla de la subtarea (ranura 0 y 4) y tabla de `this` de CreateNextSubTask (ranura 9 = FkVirt9)
inline void PutVtable(size_t off, bool thisTable = false) {
    std::array<void*, 20> t{};
    FillSlots(t, std::make_index_sequence<20>{});
    if (thisTable) {
        t[9] = reinterpret_cast<void*>(&FkVirt9);
    } else {
        t[0] = reinterpret_cast<void*>(&FkSubDtor);
        t[4] = reinterpret_cast<void*>(&FkSubType);
    }
    for (size_t i = 0; i < t.size(); ++i) arenaD.Put32(off + 4 * i, u32(uintptr_t(t[i])));
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
// Bytes que la funcion cambio en las regiones globales que toca: [direccion, byte nuevo]
inline std::vector<std::vector<uint8>> caseSnap;   // regiones de la funcion tras preparar el caso (referencia de los cambios globales)
inline void SnapRegions() {
    caseSnap.clear();
    for (const auto i : regionsOf[curFn]) caseSnap.push_back(ReversingProbe::Bytes(regions[i].Ptr(), regions[i].bytes));
}
inline json GlobalChanges() {
    json out = json::array();
    size_t j = 0;
    for (const auto i : regionsOf[curFn]) {
        for (size_t k = 0; k < regions[i].bytes; ++k)
            if (regions[i].Ptr()[k] != caseSnap[j][k]) out.push_back(json::array({regions[i].at + k, regions[i].Ptr()[k]}));
        ++j;
    }
    return out;
}

// Descripcion de la llamada de un caso
struct Call {
    std::array<u32, 8> args{};
    u32 argCount{}, popBytes{}, ecx{};
};

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    gRet = 0; gDtorWrite = 0; gNew = 0; gFire = 0; gTaskType = 0; gVirt = 0; gMutate = 0; gNewPos = {0, 0, 0}; gIsP = {0, 0}; gIsPCount = 0; gPad = 0; gTarget = 0; gNx = 0; gNy = 0; gNormMask = 0; gPedPtr = 0; gFxSeed = 0; gFxCount = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kConst:
    case kFxCtor:
        break;
    case kMakeAb:
        call.args[0] = a[4]; call.args[1] = a[5]; call.args[2] = a[6]; call.argCount = 3;
        break;
    case kSdd:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kBodyRef:
        arenaA.Put32(o + kRefOff[c.fn], a[4]);
        gRet = a[5]; gDtorWrite = a[6];
        break;
    case kBodyVt:
    case kBodyJmp:
        gRet = a[4]; gDtorWrite = a[5];
        break;
    case kBhwm:
        arenaA.Put32(o + 0x14, a[4] == 0 ? 0u : u32(arenaB.Obj(a[5])));
        gDtorWrite = a[6]; gRet = a[7];
        break;
    case kWcBody:
        if (a[4] != 0) {
            arenaA.Put32(o + 8, u32(arenaB.Obj(a[5])));
            arenaB.Put32(a[5], u32(arenaD.Obj(a[6])));
            PutVtable(a[6]);
        } else {
            arenaA.Put32(o + 8, 0);
        }
        break;
    case kExtCtor:
        arenaB.Put32(a[4], a[5]); arenaB.Put32(a[4] + 4, a[6]); arenaB.Put32(a[4] + 8, a[7]);
        call.args[0] = u32(arenaB.Obj(a[4])); call.argCount = 1;
        break;
    case kExtClone:
        arenaA.Put32(o + 0xC, a[5]); arenaA.Put32(o + 0x10, a[6]); arenaA.Put32(o + 0x14, a[7]);
        gNew = a[4] == 0 ? 0u : u32(arenaB.Obj(a[8]));
        break;
    case kExtNext:
        arenaA.Put32(o, u32(arenaD.Obj(a[4])));
        PutVtable(a[4], true);
        arenaA.Put32(o + 8, u32(arenaB.Obj(a[5])));
        arenaB.Put32(a[5], u32(arenaD.Obj(a[6])));
        PutVtable(a[6]);
        gTaskType = a[7];
        gNew = a[8] == 0 ? 0u : u32(arenaB.Obj(a[9]));
        gRet = a[10]; gDtorWrite = a[11]; gVirt = a[12];
        arenaA.Put32(o + 0xC, a[14]); arenaA.Put32(o + 0x10, a[15]); arenaA.Put32(o + 0x14, a[16]);
        call.args[0] = a[13]; call.argCount = 1;
        break;
    case kCtrlSub:
        arenaA.Put32(o + 8, a[4]);
        call.args[0] = a[5]; call.argCount = 1;
        break;
    case kExtFind: {
        const u32 ped = u32(arenaB.Obj(a[4]));
        if (a[5] != 0) {
            arenaB.Put32(a[4] + 0x14, u32(arenaC.Obj(a[6])));
            arenaC.Put32(a[6] + 0x30, a[7]); arenaC.Put32(a[6] + 0x34, a[8]); arenaC.Put32(a[6] + 0x38, a[9]);
        } else {
            arenaB.Put32(a[4] + 0x14, 0);
            arenaB.Put32(a[4] + 4, a[7]); arenaB.Put32(a[4] + 8, a[8]); arenaB.Put32(a[4] + 0xC, a[9]);
        }
        if (a[10] != 0) {
            arenaD.Put32(a[11] + 4, a[12]); arenaD.Put32(a[11] + 8, a[13]); arenaD.Put32(a[11] + 0xC, a[14]);
            gFire = u32(arenaD.Obj(a[11]));
        }
        gMutate = a[15]; gNewPos = {a[16], a[17], a[18]};
        call.args[0] = ped; call.argCount = 1;
        break;
    }
    case kExtFirst:
        if (a[4] != 0) {
            arenaB.Put32(a[5] + 4, a[6]); arenaB.Put32(a[5] + 8, a[7]); arenaB.Put32(a[5] + 0xC, a[8]);
            gFire = u32(arenaB.Obj(a[5]));
        }
        gNew = a[9] == 0 ? 0u : u32(arenaC.Obj(a[10]));
        gRet = a[11]; gDtorWrite = a[12];
        call.args[0] = a[13]; call.argCount = 1;
        break;
    case kRtaNext:
        arenaA.Put8(o + 0xD, a[4]);
        arenaA.Put32(o + 8, u32(arenaB.Obj(a[5])));
        arenaB.Put32(a[5], u32(arenaD.Obj(a[6])));
        PutVtable(a[6]);
        gTaskType = a[7]; gRet = a[8];
        call.args[0] = a[9]; call.argCount = 1;
        break;
    case kRtaFirst: {
        const u32 ped = u32(arenaB.Obj(a[4]));
        arenaA.Put32(o + 0x10, a[5]); arenaA.Put32(o + 0x14, u32(arenaC.Obj(a[7]))); arenaA.Put32(o + 0x20, a[6]);
        // agresor (entidad en C) y su matriz
        if (a[8] != 0) {
            arenaC.Put32(a[7] + 0x14, u32(arenaC.Obj(a[9])));
            arenaC.Put32(a[9] + 0x30, a[10]); arenaC.Put32(a[9] + 0x34, a[11]); arenaC.Put32(a[9] + 0x38, a[12]);
        } else {
            arenaC.Put32(a[7] + 0x14, 0);
            arenaC.Put32(a[7] + 4, a[10]); arenaC.Put32(a[7] + 8, a[11]); arenaC.Put32(a[7] + 0xC, a[12]);
        }
        // ped y su matriz
        if (a[13] != 0) {
            arenaB.Put32(a[4] + 0x14, u32(arenaD.Obj(a[14])));
            arenaD.Put32(a[14] + 0x30, a[15]); arenaD.Put32(a[14] + 0x34, a[16]); arenaD.Put32(a[14] + 0x38, a[17]);
        } else {
            arenaB.Put32(a[4] + 0x14, 0);
            arenaB.Put32(a[4] + 4, a[15]); arenaB.Put32(a[4] + 8, a[16]); arenaB.Put32(a[4] + 0xC, a[17]);
        }
        arenaB.Put32(a[4] + 0x46C, a[18]); arenaB.Put32(a[4] + 0x470, a[19]); arenaB.Put32(a[4] + 0x540, a[20]);
        // pad en D
        arenaD.Put16(a[28] + 0x10E, a[21]);
        arenaD.Put16(a[28] + 0, a[22]); arenaD.Put16(a[28] + 2, a[23]); arenaD.Put16(a[28] + 0x10, a[24]); arenaD.Put16(a[28] + 0x12, a[25]); arenaD.Put16(a[28] + 0x14, a[26]); arenaD.Put16(a[28] + 0x16, a[27]);
        gPad = u32(arenaD.Obj(a[28])); gTarget = a[29]; gIsP = {a[30], a[31]}; gRet = a[32]; gNx = a[33]; gNy = a[34]; gNormMask = a[35]; gPedPtr = ped;
        call.args[0] = ped; call.argCount = 1;
        break;
    }
    case kFxUpd:
        call.args[0] = a[4]; call.args[1] = a[5]; call.argCount = 2;
        break;
    case kFxInit:
        gFxSeed = a[4];
        break;
    case kFxExit:
        for (u32 i = 0; i < kFxPtrs; ++i) arenaA.Put32(o + 4 * i, a[4 + i]);
        break;
    }
    return call;
}

// Clasifica la vtable que deja una funcion real (EXE: valores conocidos; DLL: las aprendidas al empezar): 1 = la de la clase, 2 = la base final, 3 = sin cambios, otro = 4 + valor
inline u64 VtToken(u32 v, bool candidate, u32 prev) {
    const u32 klass = candidate ? gDllExtVt : 0x86F5FCu, base = candidate ? gDllBaseVt : 0x86D48Cu;
    return v == klass ? 1u : (v == base ? 2u : (v == prev ? 3u : 4ull + v));
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
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap(); arenaD.Snap(); SnapRegions();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, false);
        });
        json obs;
        const u32 o = c.a[0];
        const Kind k = kKind[fn];
        if (k == kExtCtor || k == kWcBody) {
            // la vtable que deja la funcion se clasifica aparte (la de la DLL no es la del EXE): se devuelve a su valor previo para que no cuente como cambio de bytes
            u32 vt; memcpy(&vt, arenaA.m.data() + kGuard + o, 4);
            u32 prev; memcpy(&prev, arenaA.r.data() + kGuard + o, 4);
            obs["vt"] = VtToken(vt, candidate, prev);
            memcpy(arenaA.m.data() + kGuard + o, arenaA.r.data() + kGuard + o, 4);
        } else if (k == kExtClone) {
            if (c.a[4] != 0) {
                u32 vt; memcpy(&vt, arenaB.m.data() + kGuard + c.a[8], 4);
                u32 prev; memcpy(&prev, arenaB.r.data() + kGuard + c.a[8], 4);
                obs["vt"] = VtToken(vt, candidate, prev);
                memcpy(arenaB.m.data() + kGuard + c.a[8], arenaB.r.data() + kGuard + c.a[8], 4);
            } else {
                obs["vt"] = 0;
            }
        }
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = GlobalChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (k) {
        case kConst: obs["ret"] = eax; break;                                                                                         // valor completo de EAX
        case kMakeAb: obs["ret"] = eax & 0xFFu; break;                                                                                // bool: solo AL
        case kSdd: case kExtCtor: case kExtClone: case kExtNext: case kCtrlSub: case kExtFind: case kExtFirst: case kRtaNext: case kRtaFirst: case kFxCtor:
            obs["ret"] = TagArena(uintptr_t(eax)); break;                                                                                 // puntero como etiqueta de arena
        default: obs["ret"] = nullptr; break;                                                                                         // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN193A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN193A_CASES", buffer, sizeof(buffer)) > 0) {
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
        {"dll_vtables", {{"ext", gDllExtVt}, {"base", gDllBaseVt}}},
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

inline void LearnVtables() {
    alignas(16) static unsigned char buffer[0x40]{};
    auto* task = ::new (static_cast<void*>(buffer)) CTaskComplexExtinguishFireOnFoot(CVector{});
    memcpy(&gDllExtVt, buffer, 4);
    task->~CTaskComplexExtinguishFireOnFoot();
    memcpy(&gDllBaseVt, buffer, 4);
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn193a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn193a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn193a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    LearnVtables();
    bool ready = gDllExtVt != 0 && gDllBaseVt != 0 && gDllExtVt != gDllBaseVt && ModuleOf(gDllExtVt) == env.asi && ModuleOf(gDllBaseVt) == env.asi;
    json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
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
    // Colaborador sin hook en la DLL: la DLL llama a su direccion del EXE; el falso va solo alli
    auto X = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;   // se esperaba que NO tuviera hook
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    const auto Del = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };
    const auto New = [&] { return F(0x61A5A0, &FkNew<0x61A5A0>); };
    env.stubs[0] = {Rt(1, &FkBody<0x61FE70>), Del()};
    env.stubs[1] = {F(0x4CEBE0, &FkSfc), F(0x61A3A0, &FkBody<0x61A3A0>)};
    env.stubs[2] = {F(0x620370, &FkBody<0x620370>), Del()};
    env.stubs[3] = {F(0x620810, &FkBody<0x620810>), Del()};
    env.stubs[4] = {Rt(5, &FkBody<0x620B90>), Del()};
    env.stubs[5] = {F(0x571A00, &FkClean), F(0x61A3D0, &FkBody<0x61A3D0>)};
    env.stubs[6] = {F(0x620C30, &FkRtaCreate)};
    env.stubs[7] = {F(0x5DF8F0, &FkIsPlayer), F(0x53FB70, &FkGetPad), F(0x540670, &FkGetTarget), X(0x44E480, &FkNorm), F(0x5429F0, &FkMoveForce), F(0x620C30, &FkRtaCreate)};
    env.stubs[8] = {F(0x620EF0, &FkBody<0x620EF0>), Del()};
    env.stubs[9] = {F(0x620FF0, &FkBody<0x620FF0>), Del()};
    env.stubs[10] = {F(0x621090, &FkBody<0x621090>), Del()};
    env.stubs[15] = {F(0x61A3D0, &FkBody<0x61A3D0>)};
    env.stubs[16] = {New(), F(0x61F3F0, &FkGunCtor)};
    env.stubs[18] = {F(0x538F40, &FkFmFind)};
    env.stubs[19] = {New()};
    env.stubs[20] = {Rt(21, &FkBody<0x659E00>), Del()};
    env.stubs[21] = {F(0x61A6F0, &FkBody<0x61A6F0>)};
    env.stubs[22] = {F(0x6588A0, &FkBody<0x6588A0>), Del()};
    env.stubs[23] = {F(0x658AE0, &FkBody<0x658AE0>), Del()};
    env.stubs[24] = {Rt(15, &FkBody<0x6598B0>), Del()};
    env.stubs[25] = {Rt(18, &FkFindFire), New(), F(0x668120, &FkGoToCtor)};
    env.stubs[27] = {F(0x4A9A80, &FkFxUpdate)};
    env.stubs[28] = {F(0x4A92A0, &FkFxRender)};
    env.stubs[29] = {F(0x4A9BE0, &FkCreateFx)};
    env.stubs[30] = {F(0x4A9810, &FkDestroyFx)};
    env.stubs[32] = {F(0x571A00, &FkClean), F(0x61A3D0, &FkBody<0x61A3D0>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {11u, 14u, 17u, 26u, 31u, 33u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[12] = "toda la funcion (destructor base de CTaskComplex real en ambos lados; subtarea sintetica con tabla virtual de falsos)";
    dllReal[13] = "toda la funcion (constructor base de CTaskComplex real en ambos lados)";
    dllReal[19] = "toda la funcion salvo CTask::operator new (el constructor base de CTaskComplex corre real en ambos lados)";
    dllReal[7] = "toda la funcion salvo los colaboradores falseados (CVector2D::Normalise solo en la direccion del EXE: la DLL lo llama por ella)";
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
    // Memoria global del juego tocada: ninguna (los falsos no escriben fuera de las arenas)
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    Run(env, ready, registered, bodies, collaborators);
}
}

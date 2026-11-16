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

// Banco par a par Fn200A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 31 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos
// en cuatro arenas propias (A, B, C y D con guardas de 0x40).
//
// Colaboradores: falso identico en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / F) para los que tienen hook y que la DLL llama por su funcion: CTask::operator new 0x61A5A0 y operator delete 0x61A5B0,
// el constructor de CBoundingBox 0x40FAF0, los cuerpos de destructor 0x677F80 (CTaskSimpleGetUp) y 0x6F54D0 (CTrain), CDoor::GetAngleOpenRatio 0x6F47E0 y CDoor::Open 0x6F4790, CMatrix::SetRotate 0x59B120 y SetTranslate 0x59AF40,
// RegisterReference 0x571B70 y CleanUpOldReference 0x571A00, GetEventGlobalGroup 0x4ABA50 y CEventGroup::Add 0x4AB420, CWorld::Remove 0x563280 y Add 0x563220, SetOrientation 0x439A80, RemoveMatrix 0x54F3B0 y Flush 0x552400.
// Falso solo en la direccion del EXE (la DLL los llama por esa direccion): RwFrameForAllObjects 0x7F1200, RwMatrixUpdate 0x7F18A0, operator delete 0x8214BD y delete[] 0x8213AE del CRT.
// Las llamadas virtuales (subtarea del destructor de las tareas complejas y las ranuras de las puertas de CTrain) van a una vtable falsa de la arena C (la misma memoria para EXE y DLL).
// Colaboradores REALES (hook apagado, sin falso): las cadenas de constructores/destructores base del EXE (0x61A390, 0x61A3B0, 0x61A3D0, 0x678CD0, 0x679040), la cadena de CMatrix ligada (0x59C050, 0x59BD10, 0x59BBB0, 0x59AD70,
// 0x59ACD0), CDoor::IsClosed 0x6F4800 y el constructor y destructor de CEventSoundQuiet (0x5E05B0, 0x5DEA00, 0x4AC450, 0x4AC480); CVehicle::operator delete (0x6E2D90, sin hook) corre real en el EXE sobre un pool de vehiculos
// sintetico (la DLL usa CPool::Delete). Las vtables que escriben los constructores/destructores se comparan como clase (EXE o vtable de la DLL: la vtable de C++ de la DLL es distinta de la del EXE por diseno; ver README.md).
// Memoria global del juego tocada (restaurada tras cada lado): marca del banco 0xA9B0B0, reloj 0xB7CB84, contador de matrices 0xB74238 y puntero del pool de vehiculos 0xB74494.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria global, los eventos de los falsos (con sus
// argumentos), el valor devuelto y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F200ALive {
using namespace F200AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F200AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { const u16 w = u16(v); memcpy(m.data() + kGuard + off, &w, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = u8(v); }
    u32 Get32(size_t off) const { u32 v{}; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]; `skip` = desplazamientos (respecto a la ventana) de dwords de vtable, cuyos 4 bytes se omiten
    json Changes(const std::vector<u32>& skip = {}) const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            if (m[i] == r[i]) continue;
            const int64 rel = int64(i) - int64(kGuard);
            bool masked = false;
            for (const u32 s : skip) masked |= rel >= int64(s) && rel < int64(s) + 4;
            if (!masked) out.push_back(json::array({rel, m[i]}));
        }
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;
inline Arena<kWin>& ArenaOf(unsigned i) { return i == 0 ? arenaA : (i == 1 ? arenaB : (i == 2 ? arenaC : arenaD)); }

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this]
inline u32 gNew{};                   // memoria que devuelve el falso de CTask::operator new (0 = sin memoria)
inline u32 gFloat{};                 // bits del float que devuelven los falsos de GetDooorAngleOpenRatio y de CDoor::GetAngleOpenRatio (por ST0)
inline u32 gDoorNewAngle{};          // dword que el falso de CDoor::Open deja en +0Ch de la puerta
inline u32 gGroup{};                 // lo que devuelve el falso de GetEventGlobalGroup
inline std::array<u32, 9> gRot{};    // las nueve palabras de rotacion que deja el falso de CMatrix::SetRotate
inline DWORD gBenchThread{};
inline constexpr u32 kEvVDtor = 0x7E1EA5E0, kEvSlot30 = 0x7E1EA5F1, kEvTrap = 0x7E1EA5FF;

// operator new de CTask (0x61A5A0; cdecl, un tamano): evento [direccion, tamano]; devuelve gNew.
inline u32 __cdecl FkNew(u32 size) {
    Ev& e = PushEv(0x61A5A0, 1);
    e.v[0] = size;
    return gNew;
}
// operator delete de CTask (0x61A5B0) y operator delete escalar del CRT (0x8214BD) (cdecl, un puntero valido): evento [direccion, puntero (etiqueta), dword de [puntero] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// operator delete[] del CRT (0x8213AE; cdecl, un puntero crudo que puede no ser memoria valida): evento [direccion, puntero (etiqueta o crudo)].
inline void __cdecl FkDeleteArray(void* p) {
    Ev& e = PushEv(0x8213AE, 1);
    e.v[0] = Tag(p);
}
// Cuerpo / constructor sin argumentos de pila (thiscall): evento [direccion, ECX (etiqueta), dword que tenia [ECX]]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Funcion thiscall sin argumentos que no toca memoria (RemoveMatrix, Flush): evento [direccion, ECX (etiqueta)].
template<u32 ADDR> inline void __fastcall FkThis(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(self);
}
// Ranura 0 de la vtable falsa de la subtarea: destructor con borrado (thiscall, banderas en la pila, `ret 4`): evento [kEvVDtor, ECX (etiqueta), banderas]
inline u32 __fastcall FkVDtor(void* self, void*, u32 flags) { Ev& e = PushEv(kEvVDtor, 2); e.v[0] = Tag(self); e.v[1] = flags; return u32(uintptr_t(self)); }
// Ranura 30 de la vtable falsa de CTrain (GetDooorAngleOpenRatio; thiscall, la puerta en la pila, `ret 4`): evento [kEvSlot30, ECX (etiqueta), puerta]; devuelve gFloat por ST0.
inline float __fastcall FkSlot30(void* self, void*, u32 door) { Ev& e = PushEv(kEvSlot30, 2); e.v[0] = Tag(self); e.v[1] = door; return std::bit_cast<float>(gFloat); }
// Cualquier otra ranura de la vtable falsa: evento [kEvTrap, ECX (etiqueta)] (una llamada a otra ranura seria una diferencia)
inline void __fastcall FkTrap(void* self, void*) { Ev& e = PushEv(kEvTrap, 1); e.v[0] = Tag(self); }
// CDoor::GetAngleOpenRatio (0x6F47E0; thiscall sin argumentos): evento [direccion, ECX (etiqueta)]; devuelve gFloat por ST0.
inline float __fastcall FkDoorRatio(void* self, void*) { Ev& e = PushEv(0x6F47E0, 1); e.v[0] = Tag(self); return std::bit_cast<float>(gFloat); }
// CDoor::Open (0x6F4790; thiscall, la razon como float en la pila, `ret 4`): evento [direccion, ECX (etiqueta), razon (bits)]; deja gDoorNewAngle en +0Ch de la puerta (el angulo actual).
inline void __fastcall FkDoorOpen(void* self, void*, u32 ratio) {
    Ev& e = PushEv(0x6F4790, 2);
    e.v[0] = Tag(self); e.v[1] = ratio;
    memcpy(static_cast<u8*>(self) + 0xC, &gDoorNewAngle, 4);
}
// CMatrix::SetRotate(x, y, z) (0x59B120; thiscall, tres floats en la pila, `ret 0Ch`): evento [direccion, x, y, z (bits)]; deja las nueve palabras gRot como rotacion (derecha +0, adelante +10h, arriba +20h) y la posicion a cero.
inline void __fastcall FkSetRotate(u32* self, void*, u32 x, u32 y, u32 z) {
    Ev& e = PushEv(0x59B120, 3);
    e.v[0] = x; e.v[1] = y; e.v[2] = z;
    for (u32 i = 0; i < 3; ++i) { self[i] = gRot[i]; self[4 + i] = gRot[3 + i]; self[8 + i] = gRot[6 + i]; self[12 + i] = 0; }
}
// CMatrix::SetTranslate(CVector) (0x59AF40; thiscall, un CVector por valor = tres floats en la pila, `ret 0Ch`): evento [direccion, x, y, z (bits)]; deja la identidad y la posicion (x, y, z) sin tocar los bits.
inline void __fastcall FkSetTranslate(u32* self, void*, u32 x, u32 y, u32 z) {
    Ev& e = PushEv(0x59AF40, 3);
    e.v[0] = x; e.v[1] = y; e.v[2] = z;
    constexpr u32 one = 0x3F800000u;
    self[0] = one; self[1] = 0; self[2] = 0; self[4] = 0; self[5] = one; self[6] = 0; self[8] = 0; self[9] = 0; self[10] = one;
    self[12] = x; self[13] = y; self[14] = z;
}
// RwFrameForAllObjects (0x7F1200; cdecl, marco, funcion y dato): evento [direccion, marco (etiqueta), funcion (ClearAtomicFlagCB / SetAtomicFlagCB normalizadas a la direccion del EXE), dato]; devuelve el marco.
inline u32 gDllClearCb{}, gDllSetCb{};
inline u32 __cdecl FkRfao(void* frame, void* cb, void* data) {
    Ev& e = PushEv(0x7F1200, 3);
    const u32 f = u32(uintptr_t(cb));
    e.v[0] = Tag(frame);
    e.v[1] = (f == 0x4C7BB0u || f == gDllClearCb || Body(f) == Body(gDllClearCb)) ? 0x4C7BB0u : ((f == 0x4C7B90u || f == gDllSetCb || Body(f) == Body(gDllSetCb)) ? 0x4C7B90u : f);
    e.v[2] = Tag(data);
    return u32(uintptr_t(frame));
}
// RwMatrixUpdate (0x7F18A0; cdecl, una matriz): evento [direccion, matriz (etiqueta)]; devuelve 1.
inline u32 __cdecl FkRwMatrixUpdate(void* m) { Ev& e = PushEv(0x7F18A0, 1); e.v[0] = Tag(m); return 1; }
// CEntity::RegisterReference (0x571B70) / CleanUpOldReference (0x571A00): thiscall, un puntero; evento [direccion, ECX (etiqueta), argumento (etiqueta)].
template<u32 ADDR> inline void __fastcall FkRef(void* self, void*, void* arg) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(arg);
}
// GetEventGlobalGroup (0x4ABA50; cdecl sin argumentos): evento [direccion]; devuelve gGroup.
inline u32 __cdecl FkGroup() { PushEv(0x4ABA50, 0); return gGroup; }
// CEventGroup::Add (0x4AB420; thiscall, evento y bool, `ret 8`): evento [direccion, ECX (etiqueta), evento (etiqueta), bool (byte bajo), entidad (etiqueta), nivel (bits), tiempo, x, y, z (bits)] con los campos de CEventSoundQuiet.
inline u32 __fastcall FkAdd(void* self, void*, const CEventSoundQuiet* ev, u32 flag) {
    Ev& e = PushEv(0x4AB420, 9);
    e.v[0] = Tag(self); e.v[1] = Tag(ev); e.v[2] = flag & 0xFFu; e.v[3] = Tag(ev->m_entity); e.v[4] = BitsOf(&ev->m_fLocalSoundLevel); e.v[5] = ev->m_startTimeInMs;
    e.v[6] = BitsOf(&ev->m_position.x); e.v[7] = BitsOf(&ev->m_position.y); e.v[8] = BitsOf(&ev->m_position.z);
    return 0;
}
// CWorld::Remove (0x563280) / Add (0x563220): cdecl, una entidad; evento [direccion, entidad (etiqueta)].
template<u32 ADDR> inline void __cdecl FkWorld(void* entity) { Ev& e = PushEv(ADDR, 1); e.v[0] = Tag(entity); }
// CPlaceable::SetOrientation(x, y, z) (0x439A80; thiscall, tres floats en la pila, `ret 0Ch`): evento [direccion, ECX (etiqueta), x, y, z (bits)].
inline void __fastcall FkSetOrient(void* self, void*, u32 x, u32 y, u32 z) { Ev& e = PushEv(0x439A80, 4); e.v[0] = Tag(self); e.v[1] = x; e.v[2] = y; e.v[3] = z; }

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
    std::array<u32, 12> args{};
    u32 argCount{}, popBytes{}, ecx{};
    bool floatRet{};
};
// Dwords de vtable que la funcion escribe (arena, desplazamiento, vtable del EXE): se omiten de los cambios y se anotan como clase ("vt" si es la del EXE o una de la DLL)
struct VptrSlot { unsigned arena; u32 off; u32 exeVt; };
inline std::vector<VptrSlot> vptrSlots;

// Subtarea falsa: objeto en B (su primer dword apunta a la vtable falsa de C)
inline void MakeFakeSub(u32 subOff, u32 vtOff) {
    arenaC.Put32(vtOff + 0x0, u32(uintptr_t(&FkVDtor)));
    arenaB.Put32(subOff, u32(arenaC.Obj(vtOff)));
}
// Vtable falsa de CTrain en C: todas las ranuras a FkTrap salvo la 30
inline void MakeTrainVtable(u32 vtOff) {
    for (u32 i = 0; i < kFakeVtableSpan / 4; ++i) arenaC.Put32(vtOff + 4 * i, u32(uintptr_t(&FkTrap)));
    arenaC.Put32(vtOff + 0x78, u32(uintptr_t(&FkSlot30)));
}
inline constexpr std::array<std::array<u32, 3>, 5> kPosModes{{{0, 0, 0}, {0x80000000u, 0x80000000u, 0x80000000u}, {0x80000000u, 0u, 0x3F800000u}, {0x7F800001u, 0x7FA00000u, 0xFF800001u}, {0x7F800000u, 0xFF800000u, 0x00000001u}}};

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    gRet = 0; gDtorWrite = 0; gNew = 0; gFloat = 0; gDoorNewAngle = 0; gGroup = 0; gRot = {};
    vptrSlots.clear();
    *reinterpret_cast<u8*>(kBenchMarker) = 0x5A;   // marca que solo pone el banco (el juego real usa 0 o 1): la prueba negativa (mutate.py) condiciona sus mutaciones a ella
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    const auto stdcall_ = [&](u32 n) { call.argCount = n; call.popBytes = 0; };
    const auto clone = [&](u32 memflag, u32 memoff, u32 exeVt) {
        gNew = memflag ? u32(arenaB.Obj(memoff)) : 0u;
        if (memflag) vptrSlots.push_back({1, memoff, exeVt});
    };
    switch (kKind[c.fn]) {
    case kBBox:
        gRet = a[4]; gDtorWrite = a[5];
        break;
    case kComplexDtor: {
        const u32 sub = a[4] ? u32(arenaB.Obj(a[5])) : 0u;
        if (a[4]) MakeFakeSub(a[5], a[6]);
        arenaA.Put32(o + 8, sub);
        vptrSlots.push_back({0, o, 0x86D48C});
        break;
    }
    case kCloneGetUp: clone(a[4], a[5], 0x8703E0); break;
    case kCloneStand: clone(a[4], a[5], 0x870404); break;
    case kCloneFall:
        arenaA.Put32(o + 0x10, a[4]); arenaA.Put32(o + 0x18, a[5]); arenaA.Put32(o + 0xC, a[6]);
        clone(a[7], a[8], 0x870430);
        break;
    case kCloneFallGetUp:
        arenaA.Put32(o + 0xC, a[4]); arenaA.Put32(o + 0x10, a[5]); arenaA.Put32(o + 0x14, a[6]);
        clone(a[7], a[8], 0x870454);
        break;
    case kCloneAirLand:
        arenaA.Put8(o + 0xC, a[4]); arenaA.Put8(o + 0xD, a[5]);
        clone(a[6], a[7], 0x8704AC);
        break;
    case kCloneInAir:
        arenaA.Put8(o + 0x24, a[4]);
        *reinterpret_cast<u32*>(kClock) = a[5];
        clone(a[6], a[7], 0x8704D8);
        break;
    case kCloneLand:
        arenaA.Put32(o + 0xC, a[4]);
        clone(a[5], a[6], 0x8704FC);
        break;
    case kCloneDeath:
        arenaA.Put32(o + 0xC, a[4]); arenaA.Put32(o + 0x10, a[5]); arenaA.Put32(o + 0x14, a[6]); arenaA.Put8(o + 0x20, a[7]); arenaA.Put8(o + 0x21, a[8]);
        clone(a[9], a[10], 0x870520);
        break;
    case kTaskSdd:
        gRet = a[5]; gDtorWrite = a[6];
        call.args[0] = a[4]; stdcall_(1);
        break;
    case kTrainSdd: {
        gRet = a[5]; gDtorWrite = a[6];
        const i32 idx = i32(a[9]);
        const int64 diff = idx >= 0 ? int64(idx) * kVehicleSize + a[10] : int64(idx) * kVehicleSize - a[10];
        const u32 pool = a[7];
        arenaB.Put32(pool + 0, u32(int64(thisA) - diff)); arenaB.Put32(pool + 4, u32(arenaD.Obj(a[8]))); arenaB.Put32(pool + 8, 0x40); arenaB.Put32(pool + 0xC, a[11]);
        arenaD.m[kGuard + a[8] + u32(idx)] &= 0x7Fu;   // el hueco esta usado (la DLL asserta contra un doble borrado)
        *reinterpret_cast<u32*>(kVehiclePool) = u32(arenaB.Obj(pool));
        call.args[0] = a[4]; stdcall_(1);
        break;
    }
    case kDoorU32:
        MakeTrainVtable(a[6]);
        arenaA.Put32(o, u32(arenaC.Obj(a[6])));
        gFloat = a[5];
        call.args[0] = a[4]; stdcall_(1); call.floatRet = true;
        break;
    case kDoorRatio:
        gFloat = a[5];
        call.args[0] = a[4]; stdcall_(1); call.floatRet = true;
        break;
    case kIsRoom:
        call.args[0] = a[4]; call.args[1] = a[5]; stdcall_(2);
        break;
    case kOpenDoor: {
        const u32 ped = a[4] ? u32(arenaC.Obj(a[5])) : 0u;
        arenaA.Put16(o + 0x22, a[10]);
        const u32 lf = a[11] ? u32(arenaB.Obj(a[13])) : 0u, rf = a[12] ? u32(arenaB.Obj(a[14])) : 0u;
        arenaA.Put32(o + kNodesBase + 4, lf); arenaA.Put32(o + kNodesBase + 8, rf);
        if (a[25] >= 1 && a[25] <= 4) {
            for (u32 i = 0; i < 3; ++i) {
                if (a[11]) arenaB.Put32(a[13] + 0x40 + 4 * i, kPosModes[a[25]][i]);
                if (a[12]) arenaB.Put32(a[14] + 0x40 + 4 * i, kPosModes[a[25]][i]);
            }
        }
        const u32 door = a[7];
        if (door == 2 || door == 3) {
            const u32 d = o + kDoorsBase + kDoorSize * door;
            arenaA.Put32(d + 0, a[17]); arenaA.Put32(d + 4, a[18]); arenaA.Put8(d + 0xA, a[21]); arenaA.Put32(d + 0xC, a[19] == 0 ? a[18] : a[20]);
        }
        if (a[4]) arenaC.Put32(a[5] + 0x14, a[4] == 2 ? u32(arenaD.Obj(a[15])) : 0u);
        *reinterpret_cast<u32*>(kClock) = a[23];
        const u32 current = a[19] == 0 ? a[18] : a[20];
        gDoorNewAngle = a[22] == 0 ? a[18] : (a[22] == 1 ? current : a[24]);
        for (u32 i = 0; i < 9; ++i) gRot[i] = a[26 + i];
        gGroup = u32(arenaD.Obj(a[16]));
        call.args[0] = ped; call.args[1] = a[6]; call.args[2] = a[7]; call.args[3] = a[8]; call.args[4] = a[9]; stdcall_(5);
        break;
    }
    case kTeleport:
        arenaA.Put32(o + 0x14, a[8] ? u32(arenaB.Obj(a[9])) : 0u);
        call.args[0] = a[4]; call.args[1] = a[5]; call.args[2] = a[6]; call.args[3] = a[7]; stdcall_(4);
        break;
    case kDivide:
        arenaA.Put32(o + 0, a[5]); arenaA.Put32(o + 4, a[6]); arenaA.Put32(o + 8, a[7]);
        call.args[0] = a[4]; stdcall_(1);
        break;
    case kNormalAddr:
    case kFlushJump:
    case kMllCtor:
        break;
    case kSetSurface:
        call.args[0] = a[4]; stdcall_(1);
        break;
    case kPlDtor:
    case kPlSdd:
        arenaA.Put32(o + 0x14, a[5]);
        *reinterpret_cast<u32*>(kNumMatrices) = a[4];
        if (kKind[c.fn] == kPlSdd) { call.args[0] = a[6]; stdcall_(1); }
        break;
    case kArrReset:
        arenaA.Put32(o + 0, a[4]); arenaA.Put32(o + 4, a[5]); arenaA.Put32(o + 8, a[6]); arenaA.Put32(o + 0xC, a[7]); arenaA.Put8(o + 0x10, a[8]);
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
        gFloatRet = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        const u32 floatBits = gFloatRet;
        json obs;
        std::array<std::vector<u32>, 4> skip;
        json vptrs = json::array();
        for (const auto& v : vptrSlots) {
            skip[v.arena].push_back(v.off);
            u32 val{}; memcpy(&val, ArenaOf(v.arena).m.data() + kGuard + v.off, 4);
            const bool ok = val == v.exeVt || ModuleOf(val) == env.asi;
            vptrs.push_back(json::array({v.arena, v.off, ok ? json("vt") : json(val)}));
        }
        obs["changes"] = {{"A", arenaA.Changes(skip[0])}, {"B", arenaB.Changes(skip[1])}, {"C", arenaC.Changes(skip[2])}, {"D", arenaD.Changes(skip[3])}};
        obs["vptrs"] = vptrs;
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kDoorU32: case kDoorRatio: obs["ret"] = floatBits; break;                                                                                 // float por ST0 (bits)
        case kIsRoom: obs["ret"] = eax & 0xFFu; break;                                                                                                 // bool: solo AL
        case kBBox: case kCloneGetUp: case kCloneStand: case kCloneFall: case kCloneFallGetUp: case kCloneAirLand: case kCloneInAir: case kCloneLand: case kCloneDeath:
        case kTaskSdd: case kTrainSdd: case kDivide: case kNormalAddr: case kMllCtor: case kPlSdd: obs["ret"] = Tag(uintptr_t(eax)); break;            // puntero como etiqueta de arena
        default: obs["ret"] = nullptr; break;                                                                                                          // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN200A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN200A_CASES", buffer, sizeof(buffer)) > 0) {
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
        unsigned firstFn = 0;   // solo para depurar: FN200A_FIRST = primera funcion que se ejecuta (la corrida de aprobacion las ejecuta todas)
        {char b[16]{}; if (GetEnvironmentVariableA("FN200A_FIRST", b, sizeof(b)) > 0) firstFn = unsigned(std::atoi(b));}
        for (unsigned f = firstFn; f < kFunctions; ++f) {
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
    if (!std::filesystem::exists(dir / "fn200a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn200a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn200a"); }
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
        if (!(found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi)) Write({{"event", "not_ready_function"}, {"function", kFn[f]}, {"found", found}, {"body", found ? env.route[f]->Body() : 0}, {"body_module", found ? ModuleOf(env.route[f]->Body()) : 0}});
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
    // Colaborador REAL: si tiene hook RH se apaga (ruta) y corre el codigo del EXE; si no, es codigo del EXE sin mas
    auto Real = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return it->second;
    };
    // Colaborador REAL que debe tener hook RH (hook apagado)
    auto R = [&](uintptr_t exe) {
        const Pair p = Real(exe);
        if (!p.hooked || !p.dll) ready = false;
        return p;
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (CRT, RW): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto E = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    auto Del = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };
    env.stubs[0] = {F(0x40FAF0, &FkBody<0x40FAF0>)};
    env.reals[1] = {R(0x61A3D0)};
    for (unsigned f : {2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u}) env.stubs[f] = {F(0x61A5A0, &FkNew)};
    env.reals[2] = {R(0x61A390)};
    env.reals[3] = {R(0x61A3B0)};
    env.reals[4] = {R(0x61A390)};
    env.reals[5] = {R(0x61A3B0)};
    env.reals[6] = {R(0x61A3B0)};
    env.reals[7] = {R(0x678CD0), R(0x61A390)};
    env.reals[8] = {R(0x61A390)};
    env.reals[9] = {R(0x679040), R(0x61A3B0)};
    env.stubs[10] = {F(0x677F80, &FkBody<0x677F80>), Del()};
    env.stubs[11] = {F(0x6F54D0, &FkBody<0x6F54D0>)};
    env.reals[11] = {Real(0x6E2D90)};
    env.stubs[13] = {F(0x6F47E0, &FkDoorRatio)};
    {   // las dos funciones de callback de atomicos: normalizadas en el falso de RwFrameForAllObjects (la DLL pasa su propia funcion)
        const Pair clear = R(0x4C7BB0), set = R(0x4C7B90);
        gDllClearCb = u32(reinterpret_cast<uintptr_t>(&CVehicleModelInfo::ClearAtomicFlagCB)); gDllSetCb = u32(reinterpret_cast<uintptr_t>(&CVehicleModelInfo::SetAtomicFlagCB));   // la DLL pasa estos punteros (con el hook apagado el salto al EXE va dentro de la funcion)
        env.stubs[15] = {F(0x6F4790, &FkDoorOpen), F(0x59B120, &FkSetRotate), F(0x59AF40, &FkSetTranslate), E(0x7F1200, &FkRfao), E(0x7F18A0, &FkRwMatrixUpdate), F(0x4ABA50, &FkGroup), F(0x4AB420, &FkAdd),
                         F(0x571B70, &FkRef<0x571B70>), F(0x571A00, &FkRef<0x571A00>)};
        env.reals[15] = {R(0x59C050), R(0x59BD10), R(0x6F4800), R(0x59BBB0), R(0x59AD70), R(0x59ACD0), R(0x5E05B0), R(0x5DEA00), R(0x4AC450), R(0x4AC480), clear, set};
    }
    env.stubs[16] = {F(0x563280, &FkWorld<0x563280>), F(0x563220, &FkWorld<0x563220>), F(0x439A80, &FkSetOrient)};
    env.stubs[20] = {F(0x552400, &FkThis<0x552400>)};
    env.stubs[22] = {F(0x54F3B0, &FkThis<0x54F3B0>)};
    env.stubs[23] = {F(0x54F3B0, &FkThis<0x54F3B0>), E(0x8214BD, &FkDelete<0x8214BD>)};
    for (unsigned f = 24; f <= 30; ++f) env.stubs[f] = {E(0x8213AE, &FkDeleteArray)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {12u, 14u, 17u, 18u, 19u, 21u}) dllReal[f] = "toda la funcion (sin colaboradores; las llamadas virtuales van a la vtable falsa de la arena C)";
    for (unsigned f : {1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u}) dllReal[f] = "toda la funcion; las cadenas de constructores y destructores base del EXE son colaboradores reales (hook apagado) y en la DLL son los constructores/destructores de C++ de las clases base";
    dllReal[11] = "toda la funcion; CVehicle::operator delete (0x6E2D90, sin hook) corre real en el EXE y en la DLL es CPool::Delete, ambos sobre el pool de vehiculos sintetico";
    dllReal[15] = "toda la funcion; la cadena de CMatrix ligada (constructor, Attach, UpdateRW, UpdateRwMatrix, destructor), CDoor::IsClosed y el constructor y destructor del evento CEventSoundQuiet corren reales (hook apagado); CDoor::Open, SetRotate, SetTranslate, RegisterReference, GetEventGlobalGroup y CEventGroup::Add son falsos";
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
    // Memoria global del juego tocada: reloj (CTaskSimpleInAir, OpenDoor), contador de matrices (CPlaceable), puntero del pool de vehiculos (CTrain); se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kClock, 4); regionsOf[7] = regionsOf[15] = {regions.size() - 1};
    regions.emplace_back(kNumMatrices, 4); regionsOf[22] = regionsOf[23] = {regions.size() - 1};
    regions.emplace_back(kVehiclePool, 4); regionsOf[11] = {regions.size() - 1};
    regions.emplace_back(kBenchMarker, 1);
    for (unsigned f = 0; f < kFunctions; ++f) regionsOf[f].push_back(regions.size() - 1);
    Run(env, ready, registered, bodies, collaborators);
}
}

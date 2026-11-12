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
#include "General.h"

// Banco par a par Fn172A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 23 funciones del EXE (audio, cutscene, pools de saltos y cheats; ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedInstall / RH_ScopedNamedGlobalInstall). El banco invoca la
// direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40) y, donde la funcion usa memoria del CRT (cadenas, pool de saltos), con bloques reales del monton del juego que el propio banco
// reserva, lee y libera (los punteros del monton nunca se comparan: se enmascaran en la imagen de referencia y se normalizan tras la llamada).
// Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair) para los que tienen hook; los que no lo tienen se parchean solo en la direccion del EXE (la DLL los llama por
// ella): constructor por defecto de CObject 0x5A1D10. El cuerpo de CObject 0x59F660 es una funcion propia de la DLL (CObjectBody::Destructor): falso en 0x59F660 y en esa funcion. Los COM falsos de los buffers de
// DirectSound son tablas de ranuras dentro de las arenas. Se observan: los bytes cambiados de las arenas, los eventos de los falsos (con todos sus argumentos), los bytes finales de la memoria global que toca cada funcion, el
// contenido de los bloques del monton, el valor devuelto y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Funcion propia de la DLL sin hook (Object.cpp): cuerpo del destructor de CObject (0x59F660)
namespace CObjectBody { void __fastcall Destructor(CObject* self, void* edx); }

namespace F172ALive {
using namespace F172AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t MAXWIN> struct ArenaT {
    static constexpr size_t NMAX = F172AFix::Arena(MAXWIN);
    alignas(4096) std::array<uint8, NMAX> m{};
    alignas(4096) std::array<uint8, NMAX> r{};
    size_t win{kWin};
    size_t N() const { return F172AFix::Arena(win); }
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N(), kind, seed); }
    void Snap() { memcpy(r.data(), m.data(), N()); }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u8 v) { m[kGuard + off] = v; }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Enmascara la imagen de referencia (los punteros del monton no son reproducibles)
    void MaskRef(size_t off, size_t n, u8 b) { memset(r.data() + kGuard + off, b, n); }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N(); ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
    // Resumen compacto: [numero de bytes cambiados, FNV-1a de (desplazamiento int32, byte nuevo) en orden ascendente]
    json ChangesCompact() const {
        u32 h = 2166136261u; u32 count = 0;
        for (size_t i = 0; i < N(); ++i)
            if (m[i] != r[i]) {
                ++count;
                const i32 off = i32(int64(i) - int64(kGuard));
                u8 b[5]; memcpy(b, &off, 4); b[4] = m[i];
                for (u8 x : b) { h ^= x; h *= 16777619u; }
            }
        return json::array({count, h});
    }
};
inline ArenaT<kWinBig> arenaA;
inline ArenaT<kWin> arenaB, arenaC;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos
inline u32 gDtorWrite{};             // dword que el falso de un cuerpo de destructor deja en [this]
inline u32 gOwner{};                 // objeto que contiene al destruido (arena A): los falsos de destructores anotan su vtable
inline u32 gChan{};                  // canal estatico (arena A) cuyo destructor libera los COM falsos
inline u32 gXor{};                   // mascara que el falso de SetIsStatic aplica a +0x1C
inline u32 gTarget{}, gStatusBase{}, gTimeBase{}, gSvcCalls{}, gTimeCalls{};   // guion de PreloadCutsceneTrack
inline u32 gPed{};                   // ped que devuelve el falso de FindPlayerPed
inline std::array<u32, 12> gVehQ{};  // vehiculos que devuelve, por llamada, el falso de FindPlayerVehicle
inline u32 gVehIdx{};
inline DWORD gBenchThread{};
inline constexpr uintptr_t kCounterAddress = 0xB5F898;    // g_numSoundChannelsUsed
inline constexpr uintptr_t kPauseAddress = 0xB7CB48;      // CTimer::m_CodePause (+0) y m_UserPause (+1)
inline constexpr uintptr_t kMaxHealthAddress = 0xB7CEE7;  // vida maxima del jugador 0

// Metodo del hardware o de otro objeto sin argumentos de pila / con n dwords de pila (thiscall, el callee limpia): evento [direccion del EXE, ECX (etiqueta), argumentos crudos]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkT0(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(self);
    return gRet;
}
template<u32 ADDR> inline u32 __fastcall FkT3(void* self, void*, u32 a1, u32 a2, u32 a3) {
    Ev& e = PushEv(ADDR, 4);
    e.v[0] = Tag(self); e.v[1] = a1; e.v[2] = a2; e.v[3] = a3;
    return gRet;
}
template<u32 ADDR> inline u32 __fastcall FkT4(void* self, void*, u32 a1, u32 a2, u32 a3, u32 a4) {
    Ev& e = PushEv(ADDR, 5);
    e.v[0] = Tag(self); e.v[1] = a1; e.v[2] = a2; e.v[3] = a3; e.v[4] = a4;
    return gRet;
}
// CAEAudioHardware::GetTrackPlayTime (0x4D8F60; sin argumentos de pila): evento [direccion, ECX]; devuelve un tiempo que depende de la llamada
inline u32 __fastcall FkTrackTime(void* self, void*) {
    Ev& e = PushEv(0x4D8F60, 1);
    e.v[0] = Tag(self);
    return gTimeBase ^ (gTimeCalls++ * 0x9E3779B1u);
}
// CAECutsceneTrackManager::Service (0x4DBFB0; un dword de pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento]; escribe el estado de +8: 2 en la vuelta gTarget y otro valor en las anteriores
inline u32 __fastcall FkSvc(void* self, void*, u32 a1) {
    Ev& e = PushEv(0x4DBFB0, 2);
    e.v[0] = Tag(self); e.v[1] = a1;
    ++gSvcCalls;
    u32 status = gSvcCalls == gTarget ? 2u : (gStatusBase + gSvcCalls * 7u) % 11u;
    if (gSvcCalls != gTarget && status == 2u) status = 3u;
    memcpy(static_cast<u8*>(self) + 8, &status, 4);
    return 0;
}
// Cuerpos de destructor / destructores de CAESound o de CAETwinLoopSoundEntity sin argumentos de pila: evento [direccion, ECX (etiqueta), dword de [ECX] al entrar, dword de [objeto contenedor] al entrar];
// deja el centinela de su direccion en [ECX]; devuelve gRet.
template<u32 ADDR, u32 SENTINEL> inline u32 __fastcall FkDtorSent(void* self, void*) {
    Ev& e = PushEv(ADDR, 3);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    e.v[2] = BitsOf(reinterpret_cast<void*>(uintptr_t(gOwner)));
    const u32 s = SENTINEL;
    memcpy(self, &s, 4);
    return gRet;
}
// Cuerpo del destructor de CObject (0x59F660, propio de la DLL): evento [direccion, ECX (etiqueta), dword de [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor por defecto de CObject (0x5A1D10): evento [direccion, ECX (etiqueta), dword +0x1C, dword +0x34 al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
inline u32 __fastcall FkObjCtor(void* self, void*) {
    Ev& e = PushEv(0x5A1D10, 3);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(static_cast<u8*>(self) + 0x1C);
    e.v[2] = BitsOf(static_cast<u8*>(self) + 0x34);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CObject::SetIsStatic (0x5A0760; un dword de pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento, dwords +0x1C, +0x34, +0x13C, +0x17C, +0x184 al entrar]; aplica XOR gXor a +0x1C (como el real
// toca las banderas); devuelve gRet.
inline u32 __fastcall FkSetStatic(void* self, void*, u32 a1) {
    Ev& e = PushEv(0x5A0760, 7);
    u8* p = static_cast<u8*>(self);
    e.v[0] = Tag(self); e.v[1] = a1; e.v[2] = BitsOf(p + 0x1C); e.v[3] = BitsOf(p + 0x34); e.v[4] = BitsOf(p + 0x13C); e.v[5] = BitsOf(p + 0x17C); e.v[6] = BitsOf(p + 0x184);
    u32 flags = BitsOf(p + 0x1C) ^ gXor;
    memcpy(p + 0x1C, &flags, 4);
    return gRet;
}
// CustomCarPipeAtomicSetup (0x5D5B20; cdecl, un argumento): evento [direccion, argumento]; devuelve gRet (el cuerpo del EXE ignora el resultado y devuelve el atomic).
inline u32 __cdecl FkAtomicSetup(u32 a1) {
    Ev& e = PushEv(0x5D5B20, 1);
    e.v[0] = a1;
    return gRet;
}
// FindPlayerPed (0x56E210; cdecl, un argumento): evento [direccion, argumento]; devuelve el ped de la arena A.
inline u32 __cdecl FkFindPed(u32 a1) {
    Ev& e = PushEv(0x56E210, 1);
    e.v[0] = a1;
    return gPed;
}
// FindPlayerVehicle (0x56E0D0; cdecl, dos argumentos): evento [direccion, argumento 1, argumento 2]; devuelve el vehiculo guionizado para esa llamada (el ultimo si se acaban).
inline u32 __cdecl FkFindVeh(u32 a1, u32 a2) {
    Ev& e = PushEv(0x56E0D0, 2);
    e.v[0] = a1; e.v[1] = a2;
    const u32 i = gVehIdx < gVehQ.size() ? gVehIdx : u32(gVehQ.size() - 1);
    ++gVehIdx;
    return gVehQ[i];
}
// Ranura 0x32 de la vtable de los vehiculos falsos (CVehicle::Fix; thiscall sin argumentos de pila): evento [marca, ECX (etiqueta), dword de [ECX] al entrar]
inline u32 __fastcall FkFix(void* self, void*) {
    Ev& e = PushEv(0xF1F1, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    return gRet;
}
// IUnknown::Release de los COM falsos (stdcall, un argumento): evento [marca, argumento (etiqueta), dword +8 y dword +0xC del canal, contador global 0xB5F898 al entrar]; devuelve gRet.
inline u32 __stdcall FkRelease(void* self) {
    Ev& e = PushEv(0xC0DE, 4);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(reinterpret_cast<void*>(uintptr_t(gChan) + 8));
    e.v[2] = BitsOf(reinterpret_cast<void*>(uintptr_t(gChan) + 0xC));
    e.v[3] = BitsOf(reinterpret_cast<void*>(kCounterAddress));
    return gRet;
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

// Bloques del monton del juego que el banco reserva por lado (los punteros no se comparan)
inline std::array<void*, 4> gHeap{};
inline u32 gHeapCount{};
inline std::vector<u8> HeapBytes(u32 len, u32 seed) { std::vector<u8> v(len ? len : 1); FillWords(v.data(), len, seed); v.resize(len); return v; }

// Regiones de memoria global (indices en `regions`): 0 = 0xB7CB48 (2 bytes), 1 = 0xB5F898 (4 bytes), 2 = 0xB7CEE7 (1 byte)
inline void SetCounter(u32 v) { memcpy(reinterpret_cast<void*>(kCounterAddress), &v, 4); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.win = WinA(c.fn); arenaB.win = kWin; arenaC.win = kWin;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    gRet = 0; gDtorWrite = 0; gOwner = 0; gChan = 0; gXor = 0; gTarget = gStatusBase = gTimeBase = gSvcCalls = gTimeCalls = 0; gPed = 0; gVehQ.fill(0); gVehIdx = 0; gHeap.fill(nullptr); gHeapCount = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kStart:
        arenaA.Put32(o, a[5]); arenaA.Put32(o + 4, a[3]);
        gRet = a[4];
        break;
    case kPause:
        arenaA.Put32(o, a[4]); arenaA.Put32(o + 4, a[3]);
        gRet = a[7];
        *reinterpret_cast<u8*>(kPauseAddress + 1) = u8(a[5]);
        *reinterpret_cast<u8*>(kPauseAddress) = u8(a[6]);
        break;
    case kPreload:
        arenaA.Put32(o, a[8]); arenaA.Put32(o + 4, a[9]); arenaA.Put32(o + 8, a[6]); arenaA.Put32(o + 0xC, a[7]); arenaA.Put32(o + 0x10, a[10]); arenaA.Put32(o + 0x14, a[8] ^ a[9]);
        gTarget = a[3]; gStatusBase = a[6]; gTimeBase = a[7];
        call.args[0] = a[4]; call.args[1] = a[5]; call.argCount = 2; call.popBytes = 0;
        break;
    case kXform:
        arenaA.Put32(o, a[3]);
        gRet = a[4];
        break;
    case kSoundMgr:
        break;
    case kChanCtor:
        call.args[0] = a[3]; call.args[1] = a[4]; call.args[2] = a[5]; call.args[3] = a[6]; call.args[4] = a[7]; call.argCount = 5; call.popBytes = 0;
        break;
    case kChanDtor: {
        arenaA.Put32(o, a[3]);
        const auto place = [&](u32 mode, u32 off) -> u32 {
            if (mode == 0) return 0;
            auto& ar = mode == 1 ? arenaB : arenaC;
            ar.Put32(off, u32(ar.Obj(kVtOff)));
            ar.Put32(kVtOff, 0xDEAD0000u); ar.Put32(kVtOff + 4, 0xDEAD0001u); ar.Put32(kVtOff + 8, u32(reinterpret_cast<uintptr_t>(&FkRelease)));
            return u32(ar.Obj(off));
        };
        const u32 p1 = place(a[4], a[5]);
        const u32 p2 = a[6] == 3 ? p1 : place(a[6], a[7]);
        arenaA.Put32(o + 8, p1); arenaA.Put32(o + 0xC, p2);
        gChan = thisA; gRet = a[9];
        SetCounter(a[10]);
        break;
    }
    case kFrontDtor: case kScriptDtor: case kAudioDtor:
        arenaA.Put32(o, a[3]);
        gOwner = thisA; gRet = a[4];
        break;
    case kTextAppend: {
        const u32 len = a[3];
        void* data = (len == 0 && a[5] == 1) ? nullptr : ::operator new(len);
        if (data) { const auto bytes = HeapBytes(len, a[8]); memcpy(data, bytes.data(), len); gHeap[gHeapCount++] = data; }
        arenaA.Put32(o, u32(uintptr_t(data))); arenaA.Put32(o + 4, len);
        auto& src = a[6] == 0 ? arenaB : arenaC;
        call.args[0] = u32(src.Obj(a[7])); call.args[1] = a[4]; call.argCount = 2; call.popBytes = 0;
        break;
    }
    case kTextClear: {
        void* data = a[3] ? ::operator new(a[5]) : nullptr;
        if (data) { memset(data, 0x6B, a[5]); gHeap[gHeapCount++] = data; }
        arenaA.Put32(o, u32(uintptr_t(data))); arenaA.Put32(o + 4, a[4]);
        break;
    }
    case kCutCtor:
        gRet = a[3]; gDtorWrite = a[4]; gXor = a[5];
        break;
    case kCutDtor:
        arenaA.Put32(o, a[3]);
        gDtorWrite = a[4];
        break;
    case kAtomicCb:
        call.args[0] = a[3]; call.args[1] = a[4]; call.argCount = 2; call.popBytes = 8;
        gRet = a[5];
        break;
    case kPoolCtor:
        call.args[0] = a[3]; call.args[1] = a[4]; call.argCount = 2; call.popBytes = 0;
        break;
    case kPoolFlush: {
        void* p0 = ::operator new[](16); void* p1 = ::operator new[](16);
        memset(p0, 0x11, 16); memset(p1, 0x22, 16);
        gHeap[0] = p0; gHeap[1] = p1; gHeapCount = 2;
        arenaA.Put32(o, u32(uintptr_t(p0))); arenaA.Put32(o + 4, u32(uintptr_t(p1))); arenaA.Put32(o + 8, a[3]); arenaA.Put32(o + 0xC, a[5]); arenaA.Put32(o + 0x10, a[4]);
        break;
    }
    case kPoolClear: {
        const u32 states = a[4] == 1 ? u32(arenaA.Obj(o + 8)) : u32(arenaB.Obj(a[5]));
        arenaA.Put32(o + 4, states); arenaA.Put32(o + 8, a[3]);
        break;
    }
    case kPoolNew: {
        for (u32 i = 0; i < a[3]; ++i) {
            const bool free = a[7] == 0 ? false : a[7] == 1 ? true : a[7] == 2 ? Rand(a[9], i) % 4 == 0 : a[7] == 3 ? Rand(a[9], i) % 4 != 0 : i == a[10] % a[3];
            arenaB.Put8(a[5] + i, u8((Rand(a[8], i) & 0x7F) | (free ? 0x80 : 0)));
        }
        arenaA.Put32(o, a[6]); arenaA.Put32(o + 4, u32(arenaB.Obj(a[5]))); arenaA.Put32(o + 8, a[3]); arenaA.Put32(o + 0xC, a[4]);
        break;
    }
    case kHealth: {
        const u32 ped = u32(arenaA.Obj(a[10]));
        gPed = ped;
        *reinterpret_cast<u8*>(kMaxHealthAddress) = u8(a[3]);
        const auto vehicle = [&](ArenaT<kWin>& ar, u32 off, u32 type) {
            ar.Put32(off, u32(ar.Obj(kVtOff)));
            ar.Put32(off + 0x590, type);
            for (u32 i = 0; i < 0x33; ++i) ar.Put32(kVtOff + 4 * i, 0xDEAD0000u + i);
            ar.Put32(kVtOff + 4 * 0x32, u32(reinterpret_cast<uintptr_t>(&FkFix)));
            return u32(ar.Obj(off));
        };
        const u32 v1 = vehicle(arenaB, a[8], a[6]), v2 = vehicle(arenaC, a[9], a[7]);
        for (u32 j = 0; j < gVehQ.size(); ++j) {
            const bool second = j == 0 ? (a[4] & 1) : ((a[4] >> j) & 1);
            gVehQ[j] = (j == 0 && a[5] % 4 == 0) ? 0u : (second ? v2 : v1);
        }
        gRet = a[11];
        break;
    }
    case kFemale:
        call.args[0] = a[3]; call.argCount = 1; call.popBytes = 4;
        break;
    }
    return call;
}

// Despues de Snap: enmascara en la imagen de referencia los punteros del monton que la funcion reemplaza o libera
inline void MaskRefs(const C& c) {
    const u32 o = c.a[0];
    switch (kKind[c.fn]) {
    case kTextAppend: case kTextClear: arenaA.MaskRef(o, 4, 0x5A); break;
    case kPoolCtor: case kPoolFlush: arenaA.MaskRef(o, 8, 0x5A); break;
    default: break;
    }
}

inline u32 FnvBytes(const u8* p, size_t n) { u32 h = 2166136261u; for (size_t i = 0; i < n; ++i) { h ^= p[i]; h *= 16777619u; } return h; }

// Despues de la llamada: lee y libera los bloques del monton, normaliza los punteros del monton en las arenas y devuelve lo observado del monton
inline json PostCall(const C& c) {
    const auto& a = c.a;
    const u32 o = a[0];
    json heap = nullptr;
    switch (kKind[c.fn]) {
    case kTextAppend: {
        const u32 newLen = arenaA.Get32(o + 4);
        const u32 data = arenaA.Get32(o);
        heap = json::object();
        heap["len"] = newLen;
        if (data && newLen <= 700) {
            heap["data"] = HexBytes(reinterpret_cast<void*>(uintptr_t(data)), newLen);
        } else {
            heap["data"] = nullptr;
        }
        if (data && data != u32(uintptr_t(gHeap[0]))) ::operator delete(reinterpret_cast<void*>(uintptr_t(data)));
        arenaA.Put32(o, 0xA5A5A5A5u);
        break;
    }
    case kTextClear: {
        const u32 data = arenaA.Get32(o);
        if (data) arenaA.Put32(o, 0xA5A5A5A5u);
        break;
    }
    case kPoolCtor: {
        const u32 p0 = arenaA.Get32(o), p1 = arenaA.Get32(o + 4), count = a[3];
        heap = json::object();
        heap["nonnull"] = json::array({p0 != 0, p1 != 0});
        if (p1 && count <= 5000) {
            const u8* st = reinterpret_cast<const u8*>(uintptr_t(p1));
            heap["states_hash"] = FnvBytes(st, count);
            heap["states_head"] = HexBytes(st, count < 8 ? count : 8);
        } else {
            heap["states_hash"] = nullptr; heap["states_head"] = nullptr;
        }
        if (p0) ::operator delete[](reinterpret_cast<void*>(uintptr_t(p0)));
        if (p1) ::operator delete[](reinterpret_cast<void*>(uintptr_t(p1)));
        if (p0) arenaA.Put32(o, 0xA5A5A5A5u);
        if (p1) arenaA.Put32(o + 4, 0xA5A5A5A5u);
        break;
    }
    case kPoolFlush: {
        const bool freed = i32(a[3]) > 0 && (a[4] & 0xFF) != 0;
        if (!freed) { ::operator delete[](gHeap[0]); ::operator delete[](gHeap[1]); }
        // si la funcion no toco los punteros siguen siendo los del monton: se normalizan al valor enmascarado de la referencia
        if (arenaA.Get32(o) == u32(uintptr_t(gHeap[0]))) arenaA.Put32(o, 0x5A5A5A5Au);
        if (arenaA.Get32(o + 4) == u32(uintptr_t(gHeap[1]))) arenaA.Put32(o + 4, 0x5A5A5A5Au);
        break;
    }
    default: break;
    }
    gHeap.fill(nullptr); gHeapCount = 0;
    return heap;
}

inline json RegionsHex() {
    json out = json::array();
    for (const auto i : regionsOf[curFn]) out.push_back(HexBytes(regions[i].Ptr(), regions[i].bytes));
    return out;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap();
        MaskRefs(c);
        TagClear(); TagAdd(arenaA.m.data(), arenaA.N()); TagAdd(arenaB.m.data(), arenaB.N()); TagAdd(arenaC.m.data(), arenaC.N());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        const json gch = RegionsHex();
        const json heap = PostCall(c);
        obs["changes"] = {{"A", fn == 4 ? arenaA.ChangesCompact() : arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}};
        obs["gchanges"] = gch;
        obs["heap"] = heap;
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kXform: case kSoundMgr: case kChanCtor: case kCutCtor: case kPoolCtor: obs["ret"] = Tag(uintptr_t(eax)); break;    // this como etiqueta de arena
        case kAtomicCb: case kPoolNew: obs["ret"] = eax; break;                                                                      // valor completo de EAX
        case kFemale: obs["ret"] = eax & 0xFFu; break;                                                                               // bool: solo AL
        default: obs["ret"] = nullptr; break;                                                                                        // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN172A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN172A_CASES", buffer, sizeof(buffer)) > 0) {
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
// Igual que MakeCollab para un colaborador con hook VIRTUAL (ranura de vtable): el cuerpo de la DLL es la entrada de la vtabla
inline Pair MakeCollabVirtual(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name; uintptr_t vbody = 0;
    if (FindHook(exe, cat, name, &vbody) && vbody) {
        env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, vbody));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}, {"virtual", true}});
        if (env.collab.back()->Registered()) { p.dll = Body(env.collab.back()->Body()); p.hooked = true; }
    }
    return p;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn172a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn172a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn172a"); }
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
    // Colaborador con hook RH VIRTUAL en la DLL (RH_ScopedVMTInstall)
    auto FV = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollabVirtual(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE: el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH cuya llamada, en la DLL, va a una funcion propia (direccion dada) y no a la del EXE: el falso va en la direccion del EXE y en esa funcion
    auto D = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        Pair p{exe, Body(dllAddress), false};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    env.stubs[0]  = {F(0x4D94D0, &FkT0<0x4D94D0>), F(0x4D9500, &FkT3<0x4D9500>), F(0x4D8870, &FkT4<0x4D8870>), F(0x4D8F30, &FkT0<0x4D8F30>)};
    env.stubs[1]  = {F(0x4D8960, &FkT3<0x4D8960>)};
    env.stubs[2]  = {F(0x4D8F60, &FkTrackTime), F(0x4DBFB0, &FkSvc), F(0x4D9870, &FkT0<0x4D9870>)};
    env.stubs[3]  = {F(0x4F1750, &FkT0<0x4F1750>)};
    env.stubs[7]  = {F(0x4F2B80, &FkDtorSent<0x4F2B80, 0x5E570001u>), F(0x4EF660, &FkDtorSent<0x4EF660, 0x5E570002u>)};
    env.stubs[8]  = {F(0x4EF660, &FkDtorSent<0x4EF660, 0x5E570002u>)};
    env.stubs[9]  = {F(0x4EF660, &FkDtorSent<0x4EF660, 0x5E570002u>)};
    env.stubs[10] = {F(0x4EF660, &FkDtorSent<0x4EF660, 0x5E570002u>)};
    env.stubs[14] = {ByExeAddress(0x5A1D10, &FkObjCtor), FV(0x5A0760, &FkSetStatic)};
    env.stubs[15] = {D(0x59F660, std::bit_cast<uintptr_t>(&CObjectBody::Destructor), &FkBody<0x59F660>)};
    // 0x5D5B20 es un `jmp 0x5DA610` y ese destino (CCustomCarEnvMapPipeline::CustomPipeAtomicSetup) tiene hook: se registra tambien (apagado, con falso que no llega a usarse) para que la entrada siga siendo codigo del EXE
    env.stubs[16] = {F(0x5D5B20, &FkAtomicSetup), F(0x5DA610, &FkAtomicSetup)};
    env.stubs[21] = {F(0x56E210, &FkFindPed), F(0x56E0D0, &FkFindVeh)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {4u, 5u, 6u, 11u, 12u, 13u, 17u, 18u, 19u, 20u, 22u}) dllReal[f] = "toda la funcion (sin colaboradores falseados; memoria del CRT real, bloques del monton del banco)";
    dllReal[14] = "toda la funcion salvo el constructor de CObject 0x5A1D10 (sin hook: falso en la direccion del EXE, que la DLL llama) y SetIsStatic 0x5A0760 (hook virtual: falso en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[15] = "toda la funcion salvo el cuerpo del destructor de CObject (0x59F660; funcion propia de la DLL CObjectBody::Destructor: falso en la direccion del EXE y en ella)";
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
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", nullptr}};
    }
    // Memoria global del juego que el banco toca: se guarda al empezar y se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kPauseAddress, 2);       // 0: CTimer m_CodePause / m_UserPause (CheckForPause)
    regions.emplace_back(kCounterAddress, 4);     // 1: g_numSoundChannelsUsed (cuerpo del destructor de CAEStaticChannel)
    regions.emplace_back(kMaxHealthAddress, 1);   // 2: vida maxima del jugador 0 (HealthCheat)
    regionsOf[1].push_back(0);
    regionsOf[6].push_back(1);
    regionsOf[21].push_back(2);
    Run(env, ready, registered, bodies, collaborators);
}
}

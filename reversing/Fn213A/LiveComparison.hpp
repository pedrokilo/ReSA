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
#include "X87Math.h"

// Banco par a par Fn213A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 32 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall). El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas
// propias (A, B, C y D con guardas de 0x40) y, para los estados de los pools, un bloque grande (kBig) que devuelve el falso de operator new[].
// Colaboradores: los que tienen hook RH en la DLL (apagado durante el banco) llevan el MISMO falso en la direccion del EXE y en el cuerpo de la DLL (F); los que la DLL llama con otra funcion distinta de la del hook
// (el miembro que envuelve el hook, MatrixScaleAllCpp, Detach en vez del cuerpo del destructor) llevan el falso del mismo evento en la direccion del EXE y en la funcion real de la DLL (Dll); los demas (CRT sin hook)
// llevan el falso solo en la direccion del EXE (ByExeAddress; la DLL los llama por esa direccion). Falsos: operator new[] 0x821195, delete[] 0x8213AE y delete 0x8214BD (CRT); CompressFromFullMatrix 0x59BAD0,
// FindPlayerVehicle 0x56E0D0, DecompressIntoFullMatrix 0x59B9F0, constructor de copia de CMatrix 0x59BCF0, ScaleAll 0x45AF40, operator+= 0x59ADF0, operator= 0x59BBC0, destructor 0x59ACD0, ApplyPanelDamageToCar
// 0x59CA10, GetDoorStatus 0x6C2230, SetDoorStatus 0x6C21C0, SetDoorDamage 0x6B1600, CWorld::Remove 0x563280, CWorld::Add 0x563220, CBike::CalculateLeanMatrix 0x6B7150 y la ranura 47 (GetRideAnimData) de una
// vtabla sintetica. Se observan: los bytes cambiados de las cuatro arenas y del bloque grande, los eventos de los falsos (con sus argumentos), el valor devuelto (punteros como etiqueta de arena; AL en IsMemberOfList)
// y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Diagnostico de arranque: si el juego muere con una violacion de acceso ANTES de que empiece el banco, escribe la direccion y la pila en fn213a-crash.txt (junto al EXE).
namespace F213ACrash {
inline LONG WINAPI Handler(EXCEPTION_POINTERS* p) {
    if (p->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) return EXCEPTION_CONTINUE_SEARCH;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::string path = std::string(exe); path = path.substr(0, path.find_last_of("\\/") + 1) + "fn213a-crash.txt";
    char text[2048]{};
    const auto* c = p->ContextRecord;
    int n = wsprintfA(text, "av eip=%08X addr=%08X rw=%u eax=%08X ecx=%08X edx=%08X esi=%08X edi=%08X ebp=%08X esp=%08X\r\nstack:", unsigned(c->Eip), unsigned(p->ExceptionRecord->ExceptionInformation[1]), unsigned(p->ExceptionRecord->ExceptionInformation[0]),
        unsigned(c->Eax), unsigned(c->Ecx), unsigned(c->Edx), unsigned(c->Esi), unsigned(c->Edi), unsigned(c->Ebp), unsigned(c->Esp));
    const unsigned* sp = reinterpret_cast<const unsigned*>(c->Esp);
    for (int i = 0; i < 48 && !IsBadReadPtr(sp + i, 4); ++i) n += wsprintfA(text + n, " %08X", sp[i]);
    HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(c->Eip), &m);
    n += wsprintfA(text + n, "\r\nmodule=%08X\r\n", unsigned(reinterpret_cast<uintptr_t>(m)));
    HANDLE f = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f != INVALID_HANDLE_VALUE) { DWORD w{}; WriteFile(f, text, DWORD(n), &w, nullptr); CloseHandle(f); }
    return EXCEPTION_CONTINUE_SEARCH;
}
inline const bool installed = [] { AddVectoredExceptionHandler(1, Handler); return true; }();
}

namespace F213ALive {
using namespace F213AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F213AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
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
inline Arena<kWin>& ArenaAt(unsigned i) { return i == 0 ? arenaA : (i == 1 ? arenaB : (i == 2 ? arenaC : arenaD)); }

// Bloque grande de los estados de los pools: memoria reservada aparte (cero al empezar y tras cada lado); la direccion se anota en la entrada de cada caso (input["big"]).
inline uint8* gBigBase{};
inline uint8* BigBase() {
    if (!gBigBase) gBigBase = static_cast<uint8*>(VirtualAlloc(nullptr, kBig, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
    return gBigBase;
}
// Runs [inicio, fin, valor] de los bytes distintos de cero de [0, hasta) y vuelve a poner a cero lo escrito
inline json BigRuns(size_t upTo) {
    json out = json::array();
    uint8* const b = BigBase();
    if (upTo > kBig) upTo = kBig;
    size_t i = 0;
    while (i < upTo) {
        if (b[i] == 0) { ++i; continue; }
        const size_t start = i; const uint8 v = b[i];
        while (i < upTo && b[i] == v) ++i;
        out.push_back(json::array({start, i, v}));
    }
    std::memset(b, 0, upTo);
    return out;
}

// ================================================================================================ falsos de los colaboradores
inline u32 gNewRet[2]{};             // lo que devuelven las dos llamadas de operator new[]
inline unsigned gNewCalls{};
inline u32 gPlayerVeh{};             // lo que devuelve el falso de FindPlayerVehicle
inline u32 gRideData{};              // lo que devuelve el falso de GetRideAnimData
inline u32 gDoorRet[8]{};            // lo que devuelve el falso de GetDoorStatus por puerta
inline DWORD gBenchThread{};

inline void DumpTag(Ev& e, unsigned i, uintptr_t v) { e.v[i] = Tag(v); }

// operator new[] (0x821195, cdecl, un tamano): evento [direccion, tamano]; devuelve gNewRet[n-esima llamada]
inline u32 __cdecl FkNewArr(u32 size) {
    Ev& e = PushEv(0x821195, 1);
    e.v[0] = size;
    return gNewRet[gNewCalls++ & 1];
}
// operator delete[] (0x8213AE) y delete (0x8214BD): cdecl, un puntero CRUDO (no se desreferencia); evento [direccion, puntero (etiqueta)].
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 1);
    DumpTag(e, 0, uintptr_t(p));
}
// CCompressedMatrixNotAligned::CompressFromFullMatrix (0x59BAD0, thiscall (matriz), `ret 4`) y DecompressIntoFullMatrix (0x59B9F0): evento [direccion, ECX, matriz]
template<u32 ADDR> inline void __fastcall FkThisArg(void* self, void*, void* arg) {
    Ev& e = PushEv(ADDR, 2);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, uintptr_t(arg));
}
// FindPlayerVehicle (0x56E0D0, cdecl (jugador, incluir remoto)): evento [direccion, jugador, remoto (byte)]; devuelve gPlayerVeh
inline u32 __cdecl FkFindPlayerVehicle(u32 playerId, u32 remote) {
    Ev& e = PushEv(0x56E0D0, 2);
    e.v[0] = playerId; e.v[1] = remote & 0xFFu;
    return gPlayerVeh;
}
// GetRideAnimData (ranura 47 de la vtabla, thiscall sin argumentos): evento [0xF00000BC, ECX]; devuelve gRideData
inline u32 __fastcall FkRideAnim(void* self, void*) {
    Ev& e = PushEv(0xF00000BCu, 1);
    DumpTag(e, 0, uintptr_t(self));
    return gRideData;
}
// Funcion thiscall sin argumentos que no toca memoria (CalculateLeanMatrix 0x6B7150, destructor de CMatrix 0x59ACD0): evento [direccion, ECX]
template<u32 ADDR> inline void __fastcall FkThis(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    DumpTag(e, 0, uintptr_t(self));
}
// CMatrix(const CMatrix&) (0x59BCF0, thiscall (otra), `ret 4`): evento [direccion, ECX, otra]; devuelve this
inline u32 __fastcall FkMatCopyCtor(void* self, void*, void* other) {
    Ev& e = PushEv(0x59BCF0, 2);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, uintptr_t(other));
    return u32(uintptr_t(self));
}
// CMatrix::ScaleAll del EXE (0x45AF40, thiscall (float), `ret 4`) y MatrixScaleAllCpp de la DLL (cdecl (matriz, float)): el mismo evento [0x45AF40, matriz, bits del float]
inline void __fastcall FkScaleAllExe(void* self, void*, u32 bits) {
    Ev& e = PushEv(0x45AF40, 2);
    DumpTag(e, 0, uintptr_t(self)); e.v[1] = bits;
}
inline void __cdecl FkScaleAllDll(void* matrix, u32 bits) {
    Ev& e = PushEv(0x45AF40, 2);
    DumpTag(e, 0, uintptr_t(matrix)); e.v[1] = bits;
}
// CMatrix::operator+= (0x59ADF0) y operator= (0x59BBC0): thiscall (otra), `ret 4`; evento [direccion, ECX, otra]
template<u32 ADDR> inline void __fastcall FkMatOp(void* self, void*, void* other) {
    Ev& e = PushEv(ADDR, 2);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, uintptr_t(other));
}
// CReplay::ApplyPanelDamageToCar (0x59CA10, cdecl (paneles, vehiculo, crear)): evento [direccion, paneles, vehiculo, crear (byte)]
inline void __cdecl FkPanels(u32 panels, void* veh, u32 create) {
    Ev& e = PushEv(0x59CA10, 3);
    e.v[0] = panels; DumpTag(e, 1, uintptr_t(veh)); e.v[2] = create & 0xFFu;
}
// CDamageManager::GetDoorStatus (0x6C2230, thiscall (puerta), `ret 4`): evento [direccion, ECX, puerta]; devuelve gDoorRet[puerta] (0..4, EAX limpio como el real)
inline u32 __fastcall FkGetDoor(void* self, void*, u32 door) {
    Ev& e = PushEv(0x6C2230, 2);
    DumpTag(e, 0, uintptr_t(self)); e.v[1] = door;
    return gDoorRet[door & 7];
}
// CDamageManager::SetDoorStatus (0x6C21C0, thiscall (puerta, estado), `ret 8`): evento [direccion, ECX, puerta, estado (byte)]
inline void __fastcall FkSetDoor(void* self, void*, u32 door, u32 status) {
    Ev& e = PushEv(0x6C21C0, 3);
    DumpTag(e, 0, uintptr_t(self)); e.v[1] = door; e.v[2] = status & 0xFFu;
}
// CAutomobile::SetDoorDamage (0x6B1600, thiscall (puerta, rotura), `ret 8`): evento [direccion, ECX, puerta, rotura (byte)]
inline void __fastcall FkSetDoorDamage(void* self, void*, u32 door, u32 flag) {
    Ev& e = PushEv(0x6B1600, 3);
    DumpTag(e, 0, uintptr_t(self)); e.v[1] = door; e.v[2] = flag & 0xFFu;
}
// CWorld::Remove (0x563280) y Add (0x563220): cdecl (entidad); evento [direccion, entidad]
template<u32 ADDR> inline void __cdecl FkWorld(void* entity) {
    Ev& e = PushEv(ADDR, 1);
    DumpTag(e, 0, uintptr_t(entity));
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
    size_t bigScan{};   // bytes del bloque grande a examinar tras la llamada (solo los constructores de pool)
};

// Memoria global del juego que el banco toca (se registra en Update): gbFirstPersonRunThisFrame (0xB6EC20, un byte) y CGame::currArea (0xB72914, un dword)
inline constexpr uintptr_t kFirstPersonAddr = 0xB6EC20, kCurrAreaAddr = 0xB72914;

inline bool IsBikeKind(Kind k) { return k == kStoreBmx || k == kStoreBike || k == kExtractBmx || k == kExtractBike; }

// Matriz del vehiculo: puntero en +0x14
inline u32 MatrixPtr(const C& c) { return c.a[6] == 0 ? 0u : (c.a[6] == 1 ? u32(arenaC.Obj(c.a[7])) : c.a[8]); }
inline void PutVtable(u32 vtOff, u32 rdOff, u32 vehOff, u32 lean, u32 bar) {
    for (u32 k = 0; k < 48; ++k) arenaC.Put32(vtOff + 4 * k, 0xBAD00000u + k);
    arenaC.Put32(vtOff + 4 * 47, u32(uintptr_t(&FkRideAnim)));
    arenaA.Put32(vehOff, u32(arenaC.Obj(vtOff)));
    arenaD.Put32(rdOff + 4, bar); arenaD.Put32(rdOff + 8, lean);
    gRideData = u32(arenaD.Obj(rdOff));
}

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], c.sd[0]); arenaB.Pattern(a[2], c.sd[1]); arenaC.Pattern(a[2], c.sd[2]); arenaD.Pattern(a[2], c.sd[3]);
    gNewRet[0] = gNewRet[1] = 0; gNewCalls = 0; gPlayerVeh = 0; gRideData = 0;
    for (auto& d : gDoorRet) d = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    const Kind k = kKind[c.fn];
    switch (k) {
    case kPoolCtor: {
        const u32 cap = a[4];
        gNewRet[0] = a[5];
        gNewRet[1] = i32(cap) > 0 ? u32(uintptr_t(BigBase()) + a[6]) : a[7];
        call.args[0] = cap; call.args[1] = a[8]; call.argCount = 2; call.popBytes = 0;
        call.bigScan = i32(cap) > 0 ? size_t(cap) + a[6] + 0x100 : 0;
        break;
    }
    case kPoolDtor:
    case kPoolScalar:
        arenaA.Put32(o + 8, a[4]); arenaA.Put32(o + 0x10, a[5]); arenaA.Put32(o + 0, a[6]); arenaA.Put32(o + 4, a[7]); arenaA.Put32(o + 0xC, a[8]);
        if (k == kPoolScalar) { call.args[0] = a[9]; call.argCount = 1; call.popBytes = 0; }
        break;
    case kIsMember: {
        const u32 n = a[4];
        arenaA.Put32(o, n ? u32(arenaB.Obj(a[5])) : 0u);
        for (u32 i = 0; i < n; ++i) {
            arenaB.Put32(a[5 + i], a[13 + i]);
            arenaB.Put32(a[5 + i] + 4, i + 1 < n ? u32(arenaB.Obj(a[5 + i + 1])) : 0u);
        }
        const u32 item = (a[21] == 0 && n) ? a[13 + a[22]] : (a[21] == 2 ? 0u : a[23]);
        call.args[0] = item; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kStoreCar:
    case kStoreBmx:
    case kStoreBike: {
        arenaA.Put32(o + 0x14, MatrixPtr(c));
        arenaA.Put32(o + 0x4C0, a[9]); arenaA.Put32(o + 0x49C, a[10]); arenaA.Put32(o + 0x590, a[11]); arenaA.Put32(o + 0x5B4, a[12]);
        arenaA.Put32(o + 0x44, a[13]); arenaA.Put32(o + 0x48, a[14]); arenaA.Put32(o + 0x4C, a[15]);
        arenaA.Put16(o + 0x22, a[16]); arenaA.Put8(o + 0x434, a[17]); arenaA.Put8(o + 0x435, a[18]);
        arenaA.Put32(o + 0x1C, a[19]); arenaA.Put32(o + 0x40, a[20]); arenaA.Put32(o + 0x94C, a[21]); arenaA.Put32(o + 0x494, a[22]);
        for (u32 i = 0; i < 4; ++i) { arenaA.Put32(o + 0x7D4 + 4 * i, a[23 + i]); arenaA.Put32(o + 0x828 + 4 * i, a[27 + i]); }
        arenaA.Put32(o + 0x5F4, a[31]); arenaA.Put32(o + 0x60C, a[32]);
        for (u32 i = 0; i < 6; ++i) arenaA.Put8(o + 0x5A9 + i, a[33 + i]);
        arenaA.Put8(o + 0x594, a[39]);
        gPlayerVeh = a[40] == 0 ? thisA : (a[40] == 1 ? a[46] : 0u);
        *reinterpret_cast<uint8*>(kFirstPersonAddr) = uint8(a[41]);
        if (IsBikeKind(k)) PutVtable(a[42], a[43], o, a[44], a[45]);
        call.ecx = u32(arenaB.Obj(a[4]));
        call.args[0] = thisA; call.args[1] = a[5]; call.argCount = 2; call.popBytes = 0;
        break;
    }
    case kExtractVeh:
    case kExtractBmx:
    case kExtractBike: {
        const u32 p = a[4];
        arenaA.Put32(o + 0x14, MatrixPtr(c)); arenaA.Put32(o + 0x590, a[30]); arenaA.Put16(o + 0x22, a[31]);
        arenaB.Put8(p + 2, a[9]); arenaB.Put8(p + 3, a[10]); arenaB.Put32(p + 0x1C, a[11]);
        arenaB.Put8(p + 0x20, a[12]); arenaB.Put8(p + 0x21, a[13]); arenaB.Put8(p + 0x22, a[14]);
        arenaB.Put8(p + 0x2F, a[15]); arenaB.Put8(p + 0x23, a[16]);
        for (u32 i = 0; i < 4; ++i) { arenaB.Put8(p + 0x24 + i, a[17 + i]); arenaB.Put8(p + 0x28 + i, a[21 + i]); }
        arenaB.Put8(p + 0x18, a[25]); arenaB.Put8(p + 0x19, a[26]); arenaB.Put8(p + 0x2C, a[27]); arenaB.Put8(p + 0x34, a[28]); arenaB.Put8(p + 0x35, a[29]);
        *reinterpret_cast<u32*>(kCurrAreaAddr) = a[32];
        for (u32 i = 0; i < 6; ++i) gDoorRet[i] = a[33 + i];
        if (IsBikeKind(k)) PutVtable(a[40], a[41], o, 0, 0);
        call.ecx = u32(arenaB.Obj(p));
        call.args[0] = (k == kExtractVeh && a[42]) ? 0u : thisA; call.args[1] = a[5]; call.argCount = 2; call.popBytes = 0;
        break;
    }
    }
    return call;
}

enum RetKind { rNone, rTag, rAl };
inline RetKind RetOf(Kind k) {
    switch (k) {
    case kPoolCtor: case kPoolScalar: return rTag;
    case kIsMember: return rAl;
    default: return rNone;
    }
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data())), u32(uintptr_t(arenaD.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    input["big"] = u32(uintptr_t(BigBase()));
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap(); arenaD.Snap();
        SnapRegions();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0; gThrew = false;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["big"] = call.bigScan ? BigRuns(call.bigScan) : json::array();
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        obs["threw"] = gThrew;
        const u32 eax = sides[s].reg.eax;
        switch (gThrew ? rNone : RetOf(kKind[fn])) {
        case rTag: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case rAl: obs["ret"] = eax & 0xFFu; break;           // los llamadores del EXE solo leen AL
        default: obs["ret"] = nullptr; break;
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN213A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN213A_CASES", buffer, sizeof(buffer)) > 0) {
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
    // Cada direccion debe saltar a SU cuerpo: ningun par de funciones probadas comparte cuerpo en la DLL (RH no admite dos hooks hacia la misma funcion)
    bool distinct = true;
    for (unsigned f = 0; f < kFunctions; ++f)
        for (unsigned g = f + 1; g < kFunctions; ++g)
            if (env.route[f] && env.route[g] && Body(env.route[f]->Body()) == Body(env.route[g]->Body())) distinct = false;
    ready &= distinct;
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    const unsigned nCases = CasesPerFunction();
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = nCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * nCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"distinct_bodies", distinct}, {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
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
// Direccion de un miembro no virtual (4 bytes en x86), resuelta hasta su cuerpo
template<class F> inline uintptr_t MemberBody(F f) { return Body(std::bit_cast<uintptr_t>(f)); }

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn213a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn213a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn213a"); }
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
    if (!ready) {
        for (unsigned f = 0; f < kFunctions; ++f) collaborators[kFn[f]] = {{"own_hook", ownHooks[kFn[f]]}, {"body", env.route[f] ? env.route[f]->Body() : 0}, {"body_module", env.route[f] ? ModuleOf(env.route[f]->Body()) : 0}};
        Run(env, false, registered, bodies, collaborators);
        return;
    }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    // Un colaborador con hook RH que es tambien una funcion probada reutiliza su ruta (el hook ya esta apagado); si no, se crea una ruta de colaborador
    auto Collab = [&](uintptr_t exe) -> Pair {
        auto it = collabCache.find(exe);
        if (it != collabCache.end()) return it->second;
        Pair p{exe, 0, false};
        for (unsigned f = 0; f < kFunctions; ++f)
            if (kEntries[f] == exe && env.route[f]) { p.dll = Body(env.route[f]->Body()); p.hooked = true; hooksFound.push_back({{"exe", exe}, {"tested_function", kFn[f]}}); }
        if (!p.hooked) p = MakeCollab(env, exe, hooksFound);
        return collabCache.emplace(exe, p).first->second;
    };
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en el EXE y en el cuerpo de la DLL
    auto F = [&](uintptr_t exe, auto* fake) {
        const Pair p = Collab(exe);
        if (!p.hooked || !p.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador con hook RH cuyo uso en la DLL es OTRA funcion (la DLL llama a `dllAddr`): el falso del EXE va en la direccion del EXE (con el hook apagado); el de la DLL, en `dllAddr` (dllFakes)
    auto Dll = [&](uintptr_t exe, uintptr_t dllAddr, auto* exeFake, auto* dllFake) {
        const Pair p = Collab(exe);
        if (!p.hooked) ready = false;
        if (!dllAddr || ModuleOf(dllAddr) != env.asi) ready = false;
        return std::pair<StubSpec, StubSpec>{S(Pair{exe, 0, false}, reinterpret_cast<void*>(exeFake)), StubSpec{Pair{dllAddr, 0, false}, reinterpret_cast<void*>(dllFake)}};
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE: el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };

    // ---- pools
    for (unsigned f = 0; f < 4; ++f) env.stubs[f] = {ByExeAddress(0x821195, &FkNewArr)};
    for (unsigned f = 4; f < 8; ++f) env.stubs[f] = {ByExeAddress(0x8213AE, &FkDelete<0x8213AE>)};
    for (unsigned f = 8; f < 25; ++f) env.stubs[f] = {ByExeAddress(0x8213AE, &FkDelete<0x8213AE>), ByExeAddress(0x8214BD, &FkDelete<0x8214BD>)};
    // ---- escrituras de paquetes (0x45B730, 0x45BF90, 0x45C070)
    for (unsigned f = 26; f < 29; ++f) env.stubs[f] = {F(0x59BAD0, &FkThisArg<0x59BAD0>), F(0x56E0D0, &FkFindPlayerVehicle)};
    // ---- lecturas de paquetes (0x45BA90, 0x45BFF0, 0x45C0D0): colaboradores del vehiculo; las de moto y BMX anaden CalculateLeanMatrix (la ranura de GetRideAnimData va en la vtabla sintetica)
    const auto doorGet = Dll(0x6C2230, MemberBody(static_cast<eDoorStatus (CDamageManager::*)(eDoors) const>(&CDamageManager::GetDoorStatus)), &FkGetDoor, &FkGetDoor);
    const auto doorSet = Dll(0x6C21C0, MemberBody(static_cast<void (CDamageManager::*)(eDoors, eDoorStatus)>(&CDamageManager::SetDoorStatus)), &FkSetDoor, &FkSetDoor);
    const auto scaleAll = Dll(0x45AF40, Body(uintptr_t(&MatrixScaleAllCpp)), &FkScaleAllExe, &FkScaleAllDll);
    // El destructor de la DLL (~CMatrix) llama a Detach (0x59ACF0), no al cuerpo del destructor del EXE (0x59ACD0, con hook): el falso del EXE va en 0x59ACD0 y el de la DLL en Detach
    const uintptr_t detachBody = MemberBody(&CMatrix::Detach);
    if (!detachBody || ModuleOf(detachBody) != env.asi) ready = false;
    if (!Collab(0x59ACD0).hooked) ready = false;   // apaga el hook del cuerpo del destructor (la DLL no lo usa: ~CMatrix llama a Detach)
    const StubSpec matDtorExe = S(Pair{0x59ACD0, 0, false}, reinterpret_cast<void*>(&FkThis<0x59ACD0>));
    const StubSpec matDtorDll{Pair{detachBody, 0, false}, reinterpret_cast<void*>(&FkThis<0x59ACD0>)};
    {
        std::vector<StubSpec> ex{F(0x59B9F0, &FkThisArg<0x59B9F0>), F(0x59BCF0, &FkMatCopyCtor), scaleAll.first, F(0x59ADF0, &FkMatOp<0x59ADF0>), F(0x59BBC0, &FkMatOp<0x59BBC0>),
            F(0x59CA10, &FkPanels), doorGet.first, doorSet.first, F(0x6B1600, &FkSetDoorDamage), F(0x563280, &FkWorld<0x563280>), F(0x563220, &FkWorld<0x563220>), matDtorExe};
        for (unsigned f = 29; f < 32; ++f) {
            env.stubs[f] = ex;
            env.dllFakes[f] = {doorGet.second, doorSet.second, scaleAll.second, matDtorDll};
        }
        env.stubs[30].push_back(F(0x6B7150, &FkThis<0x6B7150>));
        env.stubs[31].push_back(F(0x6B7150, &FkThis<0x6B7150>));
    }
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    dllReal[25] = "toda la funcion (sin colaboradores falseados)";
    // Memoria global del juego tocada: se guarda al empezar y se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kFirstPersonAddr, 4);
    regions.emplace_back(kCurrAreaAddr, 4);
    for (unsigned f = 26; f < 29; ++f) regionsOf[f] = {0};
    for (unsigned f = 29; f < 32; ++f) regionsOf[f] = {1};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        json dllFakes = json::array();
        for (const auto& s : env.dllFakes[f]) dllFakes.push_back({{"dll_addr", s.pair.exe}, {"dll_module", ModuleOf(s.pair.exe)}});
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"dll_fakes", dllFakes}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // los falsos solo de la DLL no son colaboradores de la lista: su direccion debe estar en el ASI
    for (unsigned f = 0; f < kFunctions; ++f) for (const auto& s : env.dllFakes[f]) ready &= s.pair.exe && ModuleOf(s.pair.exe) == env.asi;
    Run(env, ready, registered, bodies, collaborators);
}
}

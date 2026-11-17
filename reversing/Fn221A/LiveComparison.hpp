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
#include "TaskComplexSeekEntityRadiusAngleOffset.h"

// Banco par a par Fn221A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 25 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hooks S (RH_ScopedNamedGlobalInstall, RH_ScopedInstall). El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas
// propias (A, B, C y D con guardas de 0x40) y, para el bloque protector del inicio de .text, con una copia sembrada de sus datos (0xC9E000..0xC9EA00, que se guarda y se restaura).
// Colaboradores: los que tienen hook RH en la DLL (apagado durante el banco) llevan el MISMO falso en la direccion del EXE y en el cuerpo de la DLL (F) o solo en la direccion del EXE cuando la DLL los llama por ella
// (FE); los demas llevan el falso solo en la direccion del EXE (ByExeAddress).
// Falsos: constructor base de CPedAttractor 0x5EDFB0 (en la DLL, el constructor real de C++ que llaman los de las clases), CPedAttractor::Destructor 0x5EC410, CPedAttractor::BroadcastArrival 0x5EEF80, CPedAttractor::RegisterPed
// 0x5EEE30, push_back de punteros 0x5EE400 (la DLL lo llama por la direccion del EXE), cuerpo del destructor de CTaskComplexSeekEntity<RadiusAngleOffset> 0x493890, CTask::operator delete 0x61A5B0, IsPointWithinSearchLight 0x493280,
// constructor de CTaskComplexWander 0x66F450 (en la DLL, el constructor real), la rutina 0x402370 (desde 0x401FBB), GetProcAddress (casilla de la IAT del EXE usada por el salto 0x81E40C y la del ASI) y las API que el bloque protector
// llama por punteros guardados en sus datos (GetVersionEx, RegOpenKey, RegQueryValueEx, LocalAlloc, LocalFree, RegClose, y las dos API de carga de 0x402370).
// Colaboradores que corren REALES (con el hook apagado): CPedAttractorPool::New 0x5EA9F0 sobre un pool sintetico, los constructores de las diez clases de atractor (en RegisterPed), la transformacion del MD5 0x4012E9 y MD5Update 0x40104A
// (en MD5Final), y el cuerpo del destructor de la clase en los destructores con borrado.
// Memoria global del juego tocada (restaurada tras cada lado): marca del banco 0xA9B0B0, puntero del pool de atractores 0xB744C4, focos de scripts 0xA94D68 y los datos del bloque protector 0xC9E000..0xC9EA00.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto
// (punteros como etiqueta de arena), si la funcion termino con la salida no local y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Diagnostico de arranque: si el juego muere con una violacion de acceso ANTES de que empiece el banco, escribe la direccion y la pila en fn221a-crash.txt (junto al EXE).
namespace F221ACrash {
inline LONG WINAPI Handler(EXCEPTION_POINTERS* p) {
    if (p->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) return EXCEPTION_CONTINUE_SEARCH;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::string path = std::string(exe); path = path.substr(0, path.find_last_of("\\/") + 1) + "fn221a-crash.txt";
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

namespace F221ALive {
using namespace F221AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F221AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
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
inline Arena<kWin>& ArenaAt(unsigned i) { return i == 0 ? arenaA : (i == 1 ? arenaB : (i == 2 ? arenaC : arenaD)); }
// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos del cuerpo 0x5EC410, de BroadcastArrival, de RegisterPed y del cuerpo 0x493890
inline u32 gDtorWrite{};             // dword que los falsos del cuerpo dejan en [this]
inline u32 gLightMask{}, gLightHigh{};   // mascara de las respuestas de IsPointWithinSearchLight y bits altos de EAX
inline std::array<u32, 3> gOut{};    // los tres floats que escribe el falso del calculador de posiciones
inline DWORD gBenchThread{};

inline void DumpTag(Ev& e, unsigned i, uintptr_t v) { e.v[i] = Tag(v); }

// CPedAttractor::CPedAttractor (0x5EDFB0; thiscall, once dwords de pila, `ret 2Ch`): evento [direccion, ECX (etiqueta), los once argumentos (crudos)]; devuelve this; no escribe nada.
inline u32 __fastcall FkBaseCtor(void* self, void*, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9, u32 a10) {
    Ev& e = PushEv(0x5EDFB0, 12);
    DumpTag(e, 0, uintptr_t(self));
    e.v[1] = a0; e.v[2] = a1; e.v[3] = a2; e.v[4] = a3; e.v[5] = a4; e.v[6] = a5; e.v[7] = a6; e.v[8] = a7; e.v[9] = a8; e.v[10] = a9; e.v[11] = a10;
    return u32(uintptr_t(self));
}
// Cuerpo sin argumentos de pila (thiscall; 0x5EC410, 0x493890): evento [direccion, ECX (etiqueta), dword que tenia [ECX]]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    DumpTag(e, 0, uintptr_t(self));
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CPedAttractor::BroadcastArrival (0x5EEF80; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), peaton]; devuelve gRet.
inline u32 __fastcall FkBroadcast(void* self, void*, u32 ped) {
    Ev& e = PushEv(kEvBroadcast, 2);
    DumpTag(e, 0, uintptr_t(self)); e.v[1] = ped;
    return gRet;
}
// CPedAttractor::RegisterPed (0x5EEE30; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), peaton]; devuelve gRet.
inline u32 __fastcall FkRegisterPed(void* self, void*, u32 ped) {
    Ev& e = PushEv(kEvRegisterPed, 2);
    DumpTag(e, 0, uintptr_t(self)); e.v[1] = ped;
    return gRet;
}
// vector<CPedAttractor*>::push_back (instancia C, 0x5EE400; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), &valor (etiqueta), valor]. No escribe nada.
inline void __fastcall FkPushBack(void* self, void*, const u32* x) {
    Ev& e = PushEv(kEvPushBack, 3);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, uintptr_t(x)); DumpTag(e, 2, *x);
}
// CTask::operator delete (0x61A5B0; cdecl, un puntero): evento [direccion, argumento (etiqueta), dword de [argumento]]. No libera nada.
inline void __cdecl FkTaskDelete(void* p) {
    Ev& e = PushEv(kEvTaskDelete, 2);
    DumpTag(e, 0, uintptr_t(p)); e.v[1] = BitsOf(p);
}
// Calculador de posiciones (ranura 0 de la vtable falsa de +0x40; thiscall, tres punteros de pila, `ret 0Ch`): evento [marca, ECX (etiqueta), peaton, entidad, salida (etiqueta o pila)]; escribe los tres floats del caso.
inline void __fastcall FkCalc(void* self, void*, void* seeker, void* target, u32* out) {
    Ev& e = PushEv(0xFA11CA1Cu, 4);
    DumpTag(e, 0, uintptr_t(self));
    e.v[1] = u32(uintptr_t(seeker));
    e.v[2] = u32(uintptr_t(target));
    e.v[3] = Tag(uintptr_t(out)) == u32(uintptr_t(out)) ? 0x7F000000u : Tag(uintptr_t(out));
    out[0] = gOut[0]; out[1] = gOut[1]; out[2] = gOut[2];
}
inline std::array<void*, 2> gCalcVtbl{reinterpret_cast<void*>(&FkCalc), nullptr};     // vtable falsa del calculador de posiciones
// CTheScripts::IsPointWithinSearchLight (0x493280; cdecl, (posicion, indice)): evento [direccion, x, y, z (bits), indice]; devuelve el bit del indice en gLightMask y gLightHigh en los bits altos.
inline u32 __cdecl FkSearch(const u32* pos, u32 index) {
    Ev& e = PushEv(kEvSearch, 4);
    e.v[0] = pos[0]; e.v[1] = pos[1]; e.v[2] = pos[2]; e.v[3] = index;
    return ((gLightMask >> (index & 31)) & 1u) | (gLightHigh & 0xFFFFFF00u);
}
// CTaskComplexWander::CTaskComplexWander (0x66F450; thiscall, cuatro dwords de pila, `ret 10h`): evento [direccion, ECX (etiqueta), estado, direccion, bWanderSensibly, radio (bits)]; devuelve this; no escribe nada.
inline u32 __fastcall FkWanderCtor(void* self, void*, u32 state, u32 dir, u32 sens, u32 radius) {
    Ev& e = PushEv(kEvWander, 5);
    DumpTag(e, 0, uintptr_t(self)); e.v[1] = state; e.v[2] = dir; e.v[3] = sens; e.v[4] = radius;
    return u32(uintptr_t(self));
}
// La rutina 0x402370 que 0x401FBB llama al terminar (cdecl, sin argumentos): evento [direccion]. No hace nada.
inline void __cdecl FkLoaderMark() { PushEv(kEvLoaderMark, 0); }
// GetProcAddress (stdcall (modulo, nombre): casilla de la IAT del EXE (la que usa el salto 0x81E40C) y casilla de la IAT del ASI): evento [0x81E40C, modulo, nombre, FNV de la cadena (hasta 24 bytes)];
// devuelve gGpaRet[indice de llamada]. Si la llamada no es del hilo del banco reenvia a la API real.
inline std::array<u32, 64> gGpaRet{};
inline unsigned gGpaIdx{};
inline FARPROC(WINAPI* gRealGpa)(HMODULE, LPCSTR){};
inline u32 StrFnv(const char* s, u32 max) {
    u32 h = 2166136261u;
    for (u32 i = 0; i < max; ++i) {
        if (IsBadReadPtr(s + i, 1)) { h ^= 0xFFu; break; }
        h ^= u8(s[i]); h *= 16777619u;
        if (!s[i]) break;
    }
    return h;
}
inline u32 __stdcall FkGpa(HMODULE module, LPCSTR name) {
    if (GetCurrentThreadId() != gBenchThread && gRealGpa) return u32(uintptr_t(gRealGpa(module, name)));
    Ev& e = PushEv(kEvGpa, 3);
    e.v[0] = u32(uintptr_t(module)); e.v[1] = u32(uintptr_t(name)); e.v[2] = StrFnv(name, 24);
    return gGpaIdx < gGpaRet.size() ? gGpaRet[gGpaIdx++] : 0u;
}
// API guardadas en los datos del bloque protector (stdcall); cada una anota [direccion de la casilla, argumentos...] y devuelve el valor del caso
inline std::array<u32, 8> gRet3{}, gRet1{};
inline unsigned gIdx3{}, gIdx1{};
inline u32 __stdcall FkSlot3(u32 a, u32 b, u32 c) {   // 0xC9E5CF
    Ev& e = PushEv(0xC9E5CF, 3); e.v[0] = a; e.v[1] = b; e.v[2] = c;
    return gIdx3 < gRet3.size() ? gRet3[gIdx3++] : 0u;
}
inline u32 __stdcall FkSlot1(u32 a) {   // 0xC9E1F3
    Ev& e = PushEv(0xC9E1F3, 1); e.v[0] = a;
    return gIdx1 < gRet1.size() ? gRet1[gIdx1++] : 0u;
}
inline u32 gVerMajor{}, gVerMask{}, gOpenRet{}, gKeyOut{}, gQ1Ret{}, gQ2Ret{}, gQ1Size{}, gAllocPtr{};
inline std::array<u8, 64> gList{};
inline u32 __stdcall FkVer(u8* p) {   // 0xC9E61B (GetVersionEx): [puntero, tamano]; escribe la version mayor en +4 y, con tamano 0x9C, la mascara en +0x98
    Ev& e = PushEv(0xC9E61B, 2); e.v[0] = u32(uintptr_t(p)); e.v[1] = BitsOf(p);
    const u32 size = BitsOf(p);
    memcpy(p + 4, &gVerMajor, 4);
    if (size == 0x9C) { const u16 m = u16(gVerMask); memcpy(p + 0x98, &m, 2); }
    return 1;
}
inline u32 __stdcall FkOpen(u32 hkey, u32 sub, u32* out) {   // 0xC9E83D (RegOpenKey): [clave, subclave, salida]; escribe gKeyOut en la salida
    Ev& e = PushEv(0xC9E83D, 3); e.v[0] = hkey; e.v[1] = sub; e.v[2] = u32(uintptr_t(out));
    memcpy(out, &gKeyOut, 4);
    return gOpenRet;
}
inline u32 __stdcall FkQuery(u32 key, u32 name, u32 reserved, u8* type, u8* data, u8* size) {   // 0xC9E862 (RegQueryValueEx): [clave, nombre, reservado, tipo, datos (etiqueta), tamano, tamano previo]
    Ev& e = PushEv(0xC9E862, 7); e.v[0] = key; e.v[1] = name; e.v[2] = reserved; e.v[3] = u32(uintptr_t(type)); DumpTag(e, 4, uintptr_t(data)); e.v[5] = u32(uintptr_t(size)); e.v[6] = BitsOf(size);
    const u32 seven = 7;
    memcpy(type, &seven, 4);
    if (!data) memcpy(size, &gQ1Size, 4);
    else memcpy(data, gList.data(), 64);
    return data ? gQ2Ret : gQ1Ret;
}
inline u32 __stdcall FkAlloc(u32 flags, u32 size) {   // 0xC9E532 (LocalAlloc): [banderas, tamano]
    Ev& e = PushEv(0xC9E532, 2); e.v[0] = flags; e.v[1] = size;
    return gAllocPtr;
}
inline u32 __stdcall FkFree(u32 p) {   // 0xC9E541 (LocalFree): [puntero (etiqueta)]
    Ev& e = PushEv(0xC9E541, 1); DumpTag(e, 0, p);
    return 0;
}
inline u32 __stdcall FkClose(u32 key) {   // 0xC9E84D (RegCloseKey): [clave]
    Ev& e = PushEv(0xC9E84D, 1); e.v[0] = key;
    return 0;
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
    std::array<u32, 12> args{};
    u32 argCount{}, popBytes{}, ecx{};
    bool floatRet{};
};
// Dwords de vtable que la funcion escribe (arena, desplazamiento, vtable del EXE, clase): se omiten de los cambios y se anotan como el numero de clase (la vtable del EXE o la del ASI de esa clase); clase 100 = cualquier
// vtable del ASI (la clase no tiene tabla propia)
struct VptrSlot { unsigned arena; u32 off; u32 exeVt; u32 cls; };
inline std::vector<VptrSlot> vptrSlots;

// Memoria global del banco: se rellena la region entera de forma determinista y despues se escriben los campos del caso
inline void FillGlobal(u32 at, u32 bytes, u32 kind, u32 seed) {
    std::vector<u8> tmp(bytes);
    FillKind(tmp.data(), bytes, kind, seed);
    memcpy(reinterpret_cast<void*>(uintptr_t(at)), tmp.data(), bytes);
}
inline void PutG32(u32 at, u32 v) { memcpy(reinterpret_cast<void*>(uintptr_t(at)), &v, 4); }
inline void PutG16(u32 at, u32 v) { memcpy(reinterpret_cast<void*>(uintptr_t(at)), &v, 2); }
inline void PutG8(u32 at, u32 v) { *reinterpret_cast<u8*>(uintptr_t(at)) = u8(v); }
inline u32 SlotAddr(void* f) { return u32(uintptr_t(f)); }
// Direcciones de los falsos del bloque protector, para el oraculo (las escribe el banco en las casillas): [GetProcAddress, API3, API1, GetVersionEx, RegOpenKey, RegQueryValueEx, LocalAlloc, LocalFree, RegCloseKey]
inline std::array<u32, 9> FakeAddrs() {
    return {SlotAddr(reinterpret_cast<void*>(&FkGpa)), SlotAddr(reinterpret_cast<void*>(&FkSlot3)), SlotAddr(reinterpret_cast<void*>(&FkSlot1)), SlotAddr(reinterpret_cast<void*>(&FkVer)), SlotAddr(reinterpret_cast<void*>(&FkOpen)),
            SlotAddr(reinterpret_cast<void*>(&FkQuery)), SlotAddr(reinterpret_cast<void*>(&FkAlloc)), SlotAddr(reinterpret_cast<void*>(&FkFree)), SlotAddr(reinterpret_cast<void*>(&FkClose))};
}

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    const Kind k = kKind[c.fn];
    arenaA.Pattern(a[2], a[92]); arenaB.Pattern(a[2], a[93]); arenaC.Pattern(a[2], a[94]); arenaD.Pattern(a[2], a[95]);
    gRet = 0; gDtorWrite = 0; gLightMask = 0; gLightHigh = 0; gOut = {}; gGpaRet = {}; gGpaIdx = 0; gRet3 = {}; gRet1 = {}; gIdx3 = 0; gIdx1 = 0; gAllocPtr = 0; gList = {};
    vptrSlots.clear();
    *reinterpret_cast<u8*>(kBenchMarker) = u8(kMarkerValue);   // marca que solo pone el banco (el juego real usa 0 o 1): la prueba negativa (mutate.py) condiciona sus mutaciones a ella
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    const auto stdcall_ = [&](u32 n) { call.argCount = n; call.popBytes = 0; };
    const auto cdecl_ = [&](u32 n) { call.argCount = n; call.popBytes = 4 * n; };
    switch (k) {
    case kACtor:
        vptrSlots.push_back({0, o, kClsVt[kCls[c.fn]], kCls[c.fn]});
        call.args[0] = a[4]; call.args[1] = a[5]; call.args[2] = a[6]; stdcall_(3);
        break;
    case kASdd: {
        gRet = a[5]; gDtorWrite = a[6];
        const i32 idx = i32(a[9]);
        const int64 diff = int64(idx) * kAttractorSize + a[10];
        const u32 pool = a[7];
        arenaB.Put32(pool + 0, u32(int64(thisA) - diff)); arenaB.Put32(pool + 4, u32(arenaD.Obj(a[8]))); arenaB.Put32(pool + 8, 0x40); arenaB.Put32(pool + 0xC, a[11]);
        arenaD.m[kGuard + a[8] + u32(idx)] &= 0x7Fu;   // el hueco esta usado
        *reinterpret_cast<u32*>(kAttractorPool) = u32(arenaB.Obj(pool));
        call.args[0] = a[4]; stdcall_(1);
        break;
    }
    case kABody:
        gRet = a[5]; gDtorWrite = a[6];
        break;
    case kBcast: {
        const u32 first = a[4] == 0 ? 0u : u32(arenaB.Obj(a[6]));
        const i32 sz = i32(a[5]);
        const u32 last = first == 0 ? a[24] : first + 4u * u32(sz);
        arenaA.Put32(o + 4, first); arenaA.Put32(o + 8, last); arenaA.Put32(o + 0xC, a[23]);
        if (first) for (u32 i = 0; i < 12; ++i) arenaB.Put32(a[6] + 4 * i, a[11 + i]);
        const u32 att = a[8] == 0 ? 0u : (a[8] == 1 ? a[11 + (a[9] % 12)] : a[25]);
        gRet = a[10];
        call.args[0] = a[7]; call.args[1] = att; call.args[2] = thisA; stdcall_(3);
        break;
    }
    case kReg: {
        const u32 first = a[4] == 0 ? 0u : u32(arenaB.Obj(a[6]));
        const i32 sz = i32(a[5]);
        const u32 last = first == 0 ? a[15] : first + 4u * u32(sz);
        arenaA.Put32(o + 4, first); arenaA.Put32(o + 8, last); arenaA.Put32(o + 0xC, a[14]);
        const u32 fx = u32(arenaB.Obj(a[7]));
        arenaB.Put8(a[7] + 0x34, a[8]);
        for (u32 i = 0; i < 8; ++i) {
            const u32 rec = a[12] + 0x20 * i;
            arenaB.Put32(a[6] + 4 * i, u32(arenaC.Obj(rec)));
            arenaC.Put32(rec + 4, (a[16 + i] & 1) ? fx : a[24 + i]);
            arenaC.Put32(rec + 8, (a[16 + i] & 2) ? a[9] : a[32 + i]);
        }
        const u32 psz = a[40];
        arenaD.Put32(a[41] + 0, u32(arenaD.Obj(a[42]))); arenaD.Put32(a[41] + 4, u32(arenaD.Obj(a[41] + 0x10))); arenaD.Put32(a[41] + 8, psz); arenaD.Put32(a[41] + 0xC, a[43]);
        for (u32 i = 0; i < psz; ++i) arenaD.Put8(a[41] + 0x10 + i, a[44 + i]);
        *reinterpret_cast<u32*>(kAttractorPool) = u32(arenaD.Obj(a[41]));
        gRet = a[13];
        call.args[0] = a[10]; call.args[1] = fx; call.args[2] = a[9]; call.args[3] = a[11]; call.args[4] = thisA; stdcall_(5);
        break;
    }
    case kSeekSdd:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; stdcall_(1);
        break;
    case kLight: {
        FillGlobal(kLights, kLightBytes, a[2], a[90]);
        for (u32 i = 0; i < 8; ++i) { PutG8(kLights + 0x7C * i, a[16 + i]); PutG16(kLights + 0x7C * i + 4, a[24 + i]); }
        arenaA.Put32(o + 4, a[6]); arenaA.Put32(o + 8, a[7]); arenaA.Put32(o + 0xC, a[8]);
        if (a[4] == 0) arenaA.Put32(o + 0x14, 0);
        else {
            arenaA.Put32(o + 0x14, u32(arenaB.Obj(a[5])));
            arenaB.Put32(a[5] + 0x30, a[9]); arenaB.Put32(a[5] + 0x34, a[10]); arenaB.Put32(a[5] + 0x38, a[11]);
        }
        arenaC.Put32(a[12], a[13]);
        gLightMask = a[14] & 0xFF; gLightHigh = a[15];
        call.args[0] = thisA; call.args[1] = u32(arenaC.Obj(a[12])); cdecl_(2);
        break;
    }
    case kSeekPos:
        arenaA.Put32(o + 0xC, a[3]);
        arenaA.Put32(o + 0x40, u32(uintptr_t(gCalcVtbl.data())));
        gOut = {a[5], a[6], a[7]};
        call.args[0] = a[8] == 0 ? u32(arenaB.Obj(a[9])) : u32(arenaA.Obj(o + kSeekAlias[a[8] - 1]));
        call.args[1] = a[4]; stdcall_(2);
        break;
    case kBeCop:
        vptrSlots.push_back({0, o, 0x85A428, 100});
        call.args[0] = a[4]; call.args[1] = a[5]; call.args[2] = a[6]; stdcall_(3);
        break;
    case kMd5Init:
        call.args[0] = thisA; cdecl_(1);
        break;
    case kMd5Upd:
        arenaA.Put32(o, a[3]); arenaA.Put32(o + 4, a[4]);
        for (u32 i = 0; i < 4; ++i) arenaA.Put32(o + 8 + 4 * i, a[5 + i]);
        call.args[0] = thisA; call.args[1] = u32(arenaC.Obj(a[9])); call.args[2] = a[10]; cdecl_(3);
        break;
    case kMd5Fin:
        arenaA.Put32(o, a[3]); arenaA.Put32(o + 4, a[4]);
        for (u32 i = 0; i < 4; ++i) arenaA.Put32(o + 8 + 4 * i, a[5 + i]);
        FillGlobal(kProt, kProtBytes, a[2], a[90]);
        for (u32 i = 0; i < 64; ++i) PutG8(0xC9E068 + i, a[25] == 0 ? (i == 0 ? 0x80u : 0u) : ((a[9 + i / 4] >> (8 * (i % 4))) & 0xFFu));
        call.args[0] = thisA; cdecl_(1);
        break;
    case kResK:
        FillGlobal(kProt, kProtBytes, a[2], a[90]);
        PutG32(0xC9E408, a[3]); PutG32(0xC9E410, a[4]);
        for (u32 i = 0; i < gGpaRet.size(); ++i) gGpaRet[i] = (Rand(a[5], i) % 100 < a[6]) ? 0u : (Rand(a[5], i + 500) | 0x10000u);
        call.argCount = 0; call.popBytes = 0;
        break;
    case kResL:
        FillGlobal(kProt, kProtBytes, a[2], a[90]);
        PutG32(0xC9E408, a[3]);
        PutG32(0xC9E5CF, SlotAddr(reinterpret_cast<void*>(&FkSlot3))); PutG32(0xC9E1F3, SlotAddr(reinterpret_cast<void*>(&FkSlot1)));
        gRet3 = {a[4], a[5], a[6]}; gRet1 = {a[7], a[8]};
        for (u32 i = 0; i < 6; ++i) gGpaRet[i] = a[9 + i];
        call.argCount = 0; call.popBytes = 0;
        break;
    case kCbTab:
        FillGlobal(kProt, kProtBytes, a[2], a[90]);
        call.argCount = 0; call.popBytes = 0;
        break;
    case kOsVer: {
        FillGlobal(kProt, kProtBytes, a[2], a[90]);
        const auto fa = FakeAddrs();
        PutG32(0xC9E61B, fa[3]); PutG32(0xC9E83D, fa[4]); PutG32(0xC9E862, fa[5]); PutG32(0xC9E532, fa[6]); PutG32(0xC9E541, fa[7]); PutG32(0xC9E84D, fa[8]);
        for (u32 i = 0; i < 4; ++i) PutG32(0xC9E1A8 + 4 * i, a[32 + i]);
        gVerMajor = a[3]; gVerMask = a[4]; gOpenRet = a[5]; gQ1Ret = a[6]; gAllocPtr = a[7] ? u32(arenaC.Obj(a[9])) : 0u; gQ2Ret = a[8]; gKeyOut = a[12]; gQ1Size = a[15];
        for (u32 i = 0; i < 16; ++i) memcpy(gList.data() + 4 * i, &a[16 + i], 4);
        call.argCount = 0; call.popBytes = 0;
        break;
    }
    }
    return call;
}

enum RetKind { rNone, rTag, rRaw, rBool };
inline RetKind RetOf(Kind k) {
    switch (k) {
    case kACtor: case kASdd: case kReg: case kSeekSdd: case kSeekPos: case kBeCop: return rTag;   // puntero como etiqueta de arena
    case kBcast: case kLight: return rBool;                                                         // solo AL
    case kOsVer: return rRaw;                                                                       // EAX completo
    default: return rNone;
    }
}
// Clase de una vtable del ASI por su RTTI (vtable[-1] -> CompleteObjectLocator -> TypeDescriptor con el nombre decorado): 0..9 atractores, 10 CTaskComplexBeCop, -1 si no se reconoce
inline int AsiClassOf(u32 vt) {
    static const char* const names[11]{".?AVCPedATMAttractor@@", ".?AVCPedSeatAttractor@@", ".?AVCPedStopAttractor@@", ".?AVCPedPizzaAttractor@@", ".?AVCPedShelterAttractor@@", ".?AVCPedTriggerScriptAttractor@@",
                                       ".?AVCPedLookAtAttractor@@", ".?AVCPedScriptedAttractor@@", ".?AVCPedParkAttractor@@", ".?AVCPedStepAttractor@@", ".?AVCTaskComplexBeCop@@"};
    if (vt < 0x10000u || IsBadReadPtr(reinterpret_cast<const void*>(uintptr_t(vt) - 4), 4)) return -1;
    const u32 col = *reinterpret_cast<const u32*>(uintptr_t(vt) - 4);
    if (col < 0x10000u || IsBadReadPtr(reinterpret_cast<const void*>(uintptr_t(col)), 16)) return -1;
    const u32 td = *reinterpret_cast<const u32*>(uintptr_t(col) + 12);
    if (td < 0x10000u || IsBadReadPtr(reinterpret_cast<const void*>(uintptr_t(td)), 32)) return -1;
    const char* nm = reinterpret_cast<const char*>(uintptr_t(td) + 8);
    for (int i = 0; i < 11; ++i) if (!strncmp(nm, names[i], strlen(names[i]) + 1)) return i;
    return -1;
}
// Clasifica un dword de vtable: numero de clase si es la del EXE o una del ASI (por RTTI); si no, el valor crudo
inline json ClassifyVt(u32 val, const VptrSlot&, uintptr_t) {
    for (u32 cl = 0; cl < 10; ++cl) if (val == kClsVt[cl]) return cl;
    if (val == 0x85A428) return 100;
    const int k = AsiClassOf(val);
    if (k >= 0) return k == 10 ? 100u : u32(k);
    return val;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data())), u32(uintptr_t(arenaD.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    {
        const auto fa = FakeAddrs();
        input["fakes"] = json::array({fa[0], fa[1], fa[2], fa[3], fa[4], fa[5], fa[6], fa[7], fa[8], u32(uintptr_t(gCalcVtbl.data()))});   // direcciones de los falsos que el banco escribe en memoria (para el oraculo)
    }
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
        std::array<std::vector<u32>, 4> skip;
        json vptrs = json::array();
        for (const auto& v : vptrSlots) {
            skip[v.arena].push_back(v.off);
            u32 val{}; memcpy(&val, ArenaAt(v.arena).m.data() + kGuard + v.off, 4);
            vptrs.push_back(json::array({v.arena, v.off, ClassifyVt(val, v, env.asi)}));
        }
        if (kKind[fn] == kReg) {   // el objeto nuevo del pool: su primer dword (vtable) cambio en el hueco asignado
            const u32 psz = c.a[40];
            for (u32 i = 0; i < psz; ++i) {
                const u32 off = c.a[42] + kAttractorSize * i;
                if (memcmp(arenaD.m.data() + kGuard + off, arenaD.r.data() + kGuard + off, 4) == 0) continue;
                u32 val{}; memcpy(&val, arenaD.m.data() + kGuard + off, 4);
                const json cls = ClassifyVt(val, VptrSlot{}, env.asi);
                skip[3].push_back(off);
                vptrs.push_back(json::array({3, off, cls}));
            }
        }
        obs["changes"] = {{"A", arenaA.Changes(skip[0])}, {"B", arenaB.Changes(skip[1])}, {"C", arenaC.Changes(skip[2])}, {"D", arenaD.Changes(skip[3])}};
        obs["vptrs"] = vptrs;
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        obs["threw"] = gThrew;
        const u32 eax = sides[s].reg.eax;
        switch (gThrew ? rNone : RetOf(kKind[fn])) {
        case rTag: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case rRaw: obs["ret"] = eax; break;                  // valor crudo
        case rBool: obs["ret"] = eax & 0xFFu; break;         // solo AL
        default: obs["ret"] = nullptr; break;
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN221A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN221A_CASES", buffer, sizeof(buffer)) > 0) {
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

// Constructor base de CPedAttractor y de CTaskComplexWander: la funcion de la DLL (hook S sobre la envoltura `Constructor`) llama a std::construct_at / al constructor, que llama al constructor real; los constructores de las clases llaman a este ultimo.
// Se busca por la cadena de llamadas de la envoltura (hasta cuatro niveles dentro del ASI): la primera funcion que escribe una vtable del ASI (`mov dword ptr [reg], imm32` con el inmediato en el ASI) es el constructor.
inline u32 VptrImm(uintptr_t fn, uintptr_t asi) {
    const uint8* q = reinterpret_cast<const uint8*>(fn);
    for (size_t i = 0; i + 6 <= 0x400; ++i) {
        if (q[i] == 0xC7 && (q[i + 1] <= 0x03 || q[i + 1] == 0x06 || q[i + 1] == 0x07)) {
            const uint32 imm = *reinterpret_cast<const uint32*>(q + i + 2);
            if (ModuleOf(imm) == asi && imm >= 0x10000) return imm;
        }
    }
    return 0;
}
inline bool HasVptrStore(uintptr_t fn, uintptr_t asi) { return VptrImm(fn, asi) != 0; }
inline uintptr_t FindCtorChain(uintptr_t fn, unsigned depth, uintptr_t asi) {
    if (!fn || depth > 4 || ModuleOf(fn) != asi) return 0;
    if (HasVptrStore(fn, asi)) return fn;
    const uint8* q = reinterpret_cast<const uint8*>(fn);
    for (size_t i = 0; i + 5 <= 0x300; ++i) {
        if (q[i] == 0xE8) {
            const uintptr_t raw = fn + i + 5 + *reinterpret_cast<const int32*>(q + i + 1);
            if (ModuleOf(raw) != asi || IsBadReadPtr(reinterpret_cast<const void*>(raw), 8)) continue;
            const uintptr_t target = Body(raw);
            if (ModuleOf(target) == asi && target != fn && !IsBadReadPtr(reinterpret_cast<const void*>(target), 0x400)) {
                if (const uintptr_t r = FindCtorChain(target, depth + 1, asi)) return r;
            }
        }
    }
    return 0;
}
// Casilla de la IAT de un modulo para una funcion importada por nombre (0 si no esta)
inline uintptr_t* FindImportSlot(uintptr_t module, const char* func) {
    const auto* base = reinterpret_cast<const uint8*>(module);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(base + dos->e_lfanew);
    const auto dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (!dir.VirtualAddress) return nullptr;
    for (const auto* d = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(base + dir.VirtualAddress); d->Name; ++d) {
        const auto* names = reinterpret_cast<const IMAGE_THUNK_DATA32*>(base + (d->OriginalFirstThunk ? d->OriginalFirstThunk : d->FirstThunk));
        auto* slots = reinterpret_cast<IMAGE_THUNK_DATA32*>(const_cast<uint8*>(base) + d->FirstThunk);
        for (; names->u1.AddressOfData; ++names, ++slots) {
            if (names->u1.Ordinal & IMAGE_ORDINAL_FLAG32) continue;
            const auto* by = reinterpret_cast<const IMAGE_IMPORT_BY_NAME*>(base + names->u1.AddressOfData);
            if (!std::strcmp(reinterpret_cast<const char*>(by->Name), func)) return reinterpret_cast<uintptr_t*>(&slots->u1.Function);
        }
    }
    return nullptr;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn221a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn221a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn221a"); }
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
    // Colaborador con hook RH que la DLL llama POR LA DIRECCION DEL EXE (plugin::Call): el falso va solo en la direccion del EXE (el hook queda apagado)
    auto FE = [&](uintptr_t exe, auto* fake) {
        const Pair p = Collab(exe);
        if (!p.hooked) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador CON hook RH cuya llamada, en la funcion probada, va a una funcion propia de la DLL distinta de la envoltura del hook (direccion dada): el hook se apaga (ruta) y el falso va en la direccion del EXE
    // y en esa funcion.
    auto H = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        const Pair p0 = Collab(exe);
        if (!p0.hooked) ready = false;
        Pair p = p0;
        p.dll = Body(dllAddress); p.hooked = true;
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Constructor base (hook S sobre la envoltura): el falso va en la entrada del EXE (hook apagado) y en el constructor real de C++ que llaman los de las clases derivadas de la DLL
    auto FC = [&](uintptr_t exe, auto* fake) {
        Pair p = Collab(exe);
        const uintptr_t ctor = p.dll ? FindCtorChain(p.dll, 0, env.asi) : 0;
        hooksFound.push_back({{"ctor_wrapper", p.dll}, {"ctor", ctor}, {"wrapper_bytes", p.dll ? HexBytes(reinterpret_cast<void*>(p.dll), 96) : std::string()}});
        if (!p.hooked || !ctor || ModuleOf(ctor) != env.asi) ready = false;
        p.dll = ctor;
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador con hook RH en la DLL que corre REAL (sin falso) en ambos lados: su hook queda apagado (ruta de colaborador) y el EXE y la DLL (que lo llama por la direccion del EXE) ejecutan los bytes originales.
    auto RealOff = [&](uintptr_t exe) -> Pair {
        const Pair p = Collab(exe);
        if (!p.hooked) ready = false;
        return Pair{exe, 0, false};
    };
    // Colaborador REAL que es funcion con hook (hook apagado): el EXE corre sus bytes y la DLL llama a su cuerpo
    auto R = [&](uintptr_t exe) -> Pair {
        const Pair p = Collab(exe);
        if (!p.hooked || !p.dll) ready = false;
        return p;
    };
    using SeekRAO = CTaskComplexSeekEntity<CEntitySeekPosCalculatorRadiusAngleOffset>;
    for (unsigned f = 0; f < kFunctions; ++f) {
        switch (kKind[f]) {
        case kACtor: env.stubs[f] = {FC(0x5EDFB0, &FkBaseCtor)}; break;
        case kASdd: env.stubs[f] = {F(0x5EC410, &FkBody<0x5EC410>)}; env.reals[f] = {R(kEntry[f + 1])}; break;
        case kABody: env.stubs[f] = {F(0x5EC410, &FkBody<0x5EC410>)}; break;
        default: break;
        }
    }
    env.stubs[12] = {F(kEvBroadcast, &FkBroadcast)};
    env.stubs[13] = {FC(0x5EDFB0, &FkBaseCtor), FE(kEvPushBack, &FkPushBack), F(kEvRegisterPed, &FkRegisterPed)};
    env.reals[13] = {R(0x5EA9F0)};
    for (unsigned cl = 0; cl < 10; ++cl) env.reals[13].push_back(RealOff(kClsCtor[cl]));
    env.stubs[14] = {H(kEvSeekBody, reinterpret_cast<uintptr_t>(&SeekRAO::DestructorBodyExe), &FkBody<0x493890>), F(kEvTaskDelete, &FkTaskDelete)};
    env.stubs[15] = {F(kEvSearch, &FkSearch)};
    env.stubs[17] = {FC(kEvWander, &FkWanderCtor)};
    env.reals[19] = {R(0x4012E9)};
    env.reals[20] = {R(0x40104A), R(0x4012E9)};
    env.stubs[21] = {F(0x402370, &FkLoaderMark)};
    // GetProcAddress: el EXE llama al salto 0x81E40C (`jmp dword ptr [casilla]`); la DLL, a su propia casilla de la IAT. Los dos falsos son el mismo; las llamadas de otros hilos se reenvian a la API real.
    // casilla de la IAT que usa el salto 0x81E40C (`jmp dword ptr [0x85806C]`, comprobado en el desensamblado y por run_local.py)
    uintptr_t* const exeSlot = reinterpret_cast<uintptr_t*>(0x85806C);
    uintptr_t* const dllSlot = FindImportSlot(env.asi, "GetProcAddress");
    json gpaInfo = {{"exe_thunk", kEvGpa}, {"exe_thunk_bytes", HexBytes(reinterpret_cast<void*>(kEvGpa), 6)}, {"exe_slot", 0x85806C}, {"dll_slot_found", dllSlot != nullptr}};
    if (!exeSlot || !dllSlot || *exeSlot != *dllSlot || !*exeSlot) ready = false;
    if (exeSlot && dllSlot) {
        gRealGpa = reinterpret_cast<decltype(gRealGpa)>(*exeSlot);
        gpaInfo["same_function"] = *dllSlot == *exeSlot;
        for (unsigned f : {21u, 22u}) env.iat[f] = {IatSpec{exeSlot, reinterpret_cast<void*>(&FkGpa)}, IatSpec{dllSlot, reinterpret_cast<void*>(&FkGpa)}};
        env.watches.emplace_back(reinterpret_cast<uintptr_t>(exeSlot), 4);
        env.watches.emplace_back(kEvGpa, 6);
    }
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        if (kKind[f] == kASdd) dllReal[f] = "toda la funcion; el cuerpo del destructor de la clase corre real (el EXE sus bytes, la DLL su cuerpo), CPedAttractor::Destructor 0x5EC410 es falso identico y el borrado del pool de atractores (EXE: en linea; DLL: CPedAttractor::operator delete) corre real sobre el pool sintetico";
        if (kKind[f] == kABody) dllReal[f] = "toda la funcion; CPedAttractor::Destructor 0x5EC410 es falso identico (en la DLL, la llamada a CPedAttractor::Destructor)";
        if (kKind[f] == kACtor) dllReal[f] = "toda la funcion; el constructor base 0x5EDFB0 es falso (en la DLL, el constructor real de C++ que llama el de la clase)";
    }
    dllReal[13] = "toda la funcion; CPedAttractorPool::New (0x5EA9F0) y los constructores de las diez clases corren reales sobre un pool sintetico (el EXE sus bytes, la DLL su cuerpo / el constructor de C++), el constructor base 0x5EDFB0 es falso identico, RegisterPed 0x5EEE30 es falso identico y push_back 0x5EE400 es falso (la DLL lo llama por la direccion del EXE)";
    dllReal[14] = "toda la funcion; el cuerpo del destructor 0x493890 (en la DLL, DestructorBodyExe) y CTask::operator delete 0x61A5B0 son falsos identicos";
    dllReal[16] = "toda la funcion (sin colaboradores; la llamada virtual va a la vtable falsa del calculador en las arenas)";
    dllReal[17] = "toda la funcion; el constructor de CTaskComplexWander 0x66F450 es falso (en la DLL, el constructor real de C++)";
    dllReal[19] = "toda la funcion; la transformacion del MD5 corre real (el EXE 0x4012E9, la DLL su cuerpo)";
    dllReal[20] = "toda la funcion; MD5Update y la transformacion corren reales (el EXE sus bytes, la DLL sus cuerpos)";
    dllReal[21] = "toda la funcion salvo GetProcAddress (casillas de la IAT del EXE y del ASI con el mismo falso) y la rutina 0x402370 (falso identico en el EXE y en la funcion de la DLL)";
    dllReal[22] = "toda la funcion salvo GetProcAddress (casillas de la IAT del EXE y del ASI con el mismo falso) y las dos API guardadas en los datos (falsos de este banco en las casillas 0xC9E5CF y 0xC9E1F3)";
    dllReal[24] = "toda la funcion salvo las seis API guardadas en los datos del bloque (falsos de este banco en sus casillas)";
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
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()},
                                 {"iat", (f == 21 || f == 22) ? gpaInfo : json(nullptr)}};
    }
    // Memoria global del juego tocada: marca del banco (todas las funciones), puntero del pool de atractores (destructores con borrado y RegisterPed), focos de scripts y datos del bloque protector; se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kAttractorPool, 4);
    const size_t poolRegion = regions.size() - 1;
    regions.emplace_back(kBenchMarker, 1);
    const size_t markerRegion = regions.size() - 1;
    regions.emplace_back(kLights, kLightBytes);
    const size_t lightRegion = regions.size() - 1;
    regions.emplace_back(kProt, kProtBytes);
    const size_t protRegion = regions.size() - 1;
    for (unsigned f = 0; f < kFunctions; ++f) {
        switch (kKind[f]) {
        case kASdd: case kReg: regionsOf[f].push_back(poolRegion); break;
        case kLight: regionsOf[f].push_back(lightRegion); break;
        case kMd5Fin: case kResK: case kResL: case kCbTab: case kOsVer: regionsOf[f].push_back(protRegion); break;
        default: break;
        }
        regionsOf[f].push_back(markerRegion);
    }
    Run(env, ready, registered, bodies, collaborators);
}
}

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
#include "X87Math.h"
#include "Core.hpp"

// Banco par a par Fn186A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 18 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedGlobalInstall / RH_ScopedNamedGlobalInstall / RH_ScopedOverloadedInstall) y V (RH_ScopedVMTInstall). El banco
// invoca la direccion del EXE con objetos sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40; la A de 0x5000 bytes en las funciones 7 y 8). Falsos de los colaboradores: el mismo falso en la direccion del EXE
// y en el cuerpo de la DLL (MakeCollab / Pair): CTask::operator new 0x61A5A0 y operator delete 0x61A5B0, constructores de CTaskSimpleDie 0x62FA00 y 0x62FAF0, de CTaskComplexLeaveCar 0x63B8C0 y de
// CTaskUtilityLineUpPedWithCar 0x64FBB0, CCarEnterExit::ComputeDoorFlag 0x64E550, CVehicle::SetGettingOutFlags 0x6D3020, CPed::IsPlayer 0x5DF8F0, cuerpos de destructor de LeaveCar 0x63B970 y LeaveBoat 0x63BBD0, y operator new
// 0x82119A (la DLL lo llama por la direccion del EXE: falso solo en ella). Colaboradores REALES: el constructor de la base CTaskSimple 0x61A390 (hook apagado, en las Clone en linea), los constructores 0x5A2E90 y 0x5A3070 (hook
// apagado) y 0x4A8DB0 / 0x4A8DD0 / el CRT 0x821D30 y 0x821E02 (EXE) de ProcObjectMan_c, y el delete[] real (0x8213AE) de la segunda copia de Flush. Memoria global tocada (restaurada tras cada lado): las tablas de
// modificadores y de precios de CShopping (0xA98650..0xA9A7D4).
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso; en las funciones 7 y 8 solo [numero, FNV-1a] de los cambios de A), los bytes
// cambiados de la memoria global, los eventos de los falsos (con sus argumentos), el valor devuelto (punteros como etiqueta de arena; EAX completo en constantes; solo AL en bool) y el estado x87 (siempre vacio). EAX/ECX/EDX
// tras la llamada quedan en original_residue / candidate_residue. En las Clone con construccion en linea (9 y 11) el EXE deja su vtable en +0 del objeto nuevo y la DLL la suya: ese dword se enmascara en ambas lados.

namespace F186ALive {
using namespace F186AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t MAXWIN> struct ArenaT {
    static constexpr size_t NMAX = F186AFix::Arena(MAXWIN);
    alignas(4096) std::array<uint8, NMAX> m{};
    alignas(4096) std::array<uint8, NMAX> r{};
    size_t win{kWin};
    size_t N() const { return F186AFix::Arena(win); }
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N(), kind, seed); }
    void Snap() { memcpy(r.data(), m.data(), N()); }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u8 v) { m[kGuard + off] = v; }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Enmascara la imagen de referencia y la actual en un trozo (punteros del monton no reproducibles; vtable del EXE o de la DLL)
    void MaskRef(size_t off, size_t n, u8 b) { memset(r.data() + kGuard + off, b, n); }
    void MaskBoth(size_t off, size_t n, u8 b) { memset(r.data() + kGuard + off, b, n); memset(m.data() + kGuard + off, b, n); }
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
inline ArenaT<kWin> arenaB, arenaC, arenaD;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos
inline u32 gDtorWrite{};             // dword que el falso del cuerpo / constructor deja en [this]
inline u32 gNew{};                   // memoria que devuelve el falso de operator new (0 = sin memoria)
inline std::array<u32, 2> gPl{};     // EAX de la primera y la segunda llamada a CPed::IsPlayer
inline unsigned gPlIdx{};
inline std::array<void*, 4> gHeap{};
inline u32 gHeapCount{};
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
// Cuerpo de destructor sin argumentos de pila (thiscall): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword de [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor con cuatro dwords de pila (thiscall, `ret 10h`): evento [direccion, ECX (etiqueta), a, b, c, d]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtor4(void* self, void*, u32 a, u32 b, u32 c, u32 d) {
    Ev& e = PushEv(ADDR, 5);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c; e.v[4] = d;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor con cinco dwords de pila (thiscall, `ret 14h`): evento [direccion, ECX (etiqueta), a, b, c, d, e]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtor5(void* self, void*, u32 a, u32 b, u32 c, u32 d, u32 f) {
    Ev& e = PushEv(ADDR, 6);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c; e.v[4] = d; e.v[5] = f;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CTaskUtilityLineUpPedWithCar::CTaskUtilityLineUpPedWithCar (0x64FBB0; puntero a un CVector local, dos dwords y la puerta; `ret 10h`): evento [direccion, ECX (etiqueta), puntero (etiqueta de pila), x, y, z del vector, b, c, d];
// deja gDtorWrite en [ECX]; devuelve gRet.
inline u32 __fastcall FkLineUpCtor(void* self, void*, const u32* vec, u32 b, u32 c, u32 d) {
    Ev& e = PushEv(0x64FBB0, 8);
    e.v[0] = Tag(self); e.v[1] = TagStk(vec); e.v[2] = vec[0]; e.v[3] = vec[1]; e.v[4] = vec[2]; e.v[5] = b; e.v[6] = c; e.v[7] = d;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CCarEnterExit::ComputeDoorFlag (0x64E550; cdecl, vehiculo, puerta, bandera): evento [direccion, vehiculo (etiqueta), puerta, bandera]; devuelve gRet (el EXE solo usa AL).
inline u32 __cdecl FkDoorFlag(void* veh, u32 door, u32 flag) {
    Ev& e = PushEv(0x64E550, 3);
    e.v[0] = Tag(veh); e.v[1] = door; e.v[2] = flag;
    return gRet;
}
// CVehicle::SetGettingOutFlags (0x6D3020; thiscall, un dword de pila, `ret 4`): evento [direccion, ECX (etiqueta), BYTE bajo del argumento]. El EXE solo lee el byte bajo (los otros tres bytes son basura de su pila).
inline void __fastcall FkSetOut(void* self, void*, u32 flags) {
    Ev& e = PushEv(0x6D3020, 2);
    e.v[0] = Tag(self); e.v[1] = flags & 0xFFu;
}
// CPed::IsPlayer (0x5DF8F0; sin argumentos de pila): evento [direccion, ECX (etiqueta)]; devuelve gPl[0] la primera vez y gPl[1] las demas.
inline u32 __fastcall FkIsPlayer(void* self, void*) {
    Ev& e = PushEv(0x5DF8F0, 1);
    e.v[0] = Tag(self);
    return gPl[gPlIdx++ == 0 ? 0 : 1];
}
inline void PutRaw(uintptr_t at, const void* data, size_t n) { memcpy(reinterpret_cast<void*>(at), data, n); }
inline void PutRaw32(uintptr_t at, u32 v) { PutRaw(at, &v, 4); }

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
};

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.win = WinA(c.fn); arenaB.win = kWin; arenaC.win = kWin; arenaD.win = kWin;
    arenaA.Pattern(a[2], a[kSeed]); arenaB.Pattern(a[2], a[kSeed + 1]); arenaC.Pattern(a[2], a[kSeed + 2]); arenaD.Pattern(a[2], a[kSeed + 3]);
    gRet = 0; gDtorWrite = 0; gNew = 0; gPl = {}; gPlIdx = 0; gHeap.fill(nullptr); gHeapCount = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kStat:
        for (u32 i = 0; i < 4; ++i) arenaD.Put32(a[3] + 4 * i, a[4 + i]);
        call.args[0] = u32(arenaD.Obj(a[3])); call.argCount = 1; call.popBytes = 4;
        break;
    case kRemMod:
    case kAddMod: {
        const bool add = kKind[c.fn] == kAddMod;
        FillKind(reinterpret_cast<u8*>(kGlobBase), kGlobSize, a[2], a[kSeed + 1]);
        PutRaw32(kNumMods, a[3]);
        const u32 key = a[4];
        if (!add) {
            PutRaw32(kNumPrices, a[8]);
            if (a[5] >= 1 && i32(a[3]) > 0) { PutRaw32(kModTab + 8 * a[6], key); if (a[5] == 2) PutRaw32(kModTab + 8 * a[7], key); }
            call.args[0] = key; call.argCount = 1; call.popBytes = 4;
        } else {
            PutRaw32(kNumPrices, a[9]);
            if (a[6] >= 1 && i32(a[3]) > 0) { PutRaw32(kModTab + 8 * a[7], key); if (a[6] == 2) PutRaw32(kModTab + 8 * a[8], key); }
            if (a[10] >= 1 && i32(a[9]) > 0) { PutRaw32(kPriceTab + 0x18 * a[11], key); if (a[10] == 2) PutRaw32(kPriceTab + 0x18 * a[12], key); }
            call.args[0] = key; call.args[1] = a[5]; call.argCount = 2; call.popBytes = 8;
        }
        break;
    }
    case kGetAt:
        arenaA.Put32(o, a[3]); arenaA.Put32(o + 4, u32(arenaD.Obj(a[6])) - a[4]); arenaA.Put32(o + 8, a[7]); arenaA.Put32(o + 0xC, a[8]); arenaA.Put32(o + 0x10, a[9]);
        arenaD.Put8(a[6], u8(a[5]));
        call.args[0] = a[4]; call.argCount = 1;
        break;
    case kFlush: {
        void* p0 = ::operator new[](16); void* p1 = ::operator new[](16);
        memset(p0, 0x11, 16); memset(p1, 0x22, 16);
        gHeap[0] = p0; gHeap[1] = p1; gHeapCount = 2;
        arenaA.Put32(o, u32(uintptr_t(p0))); arenaA.Put32(o + 4, u32(uintptr_t(p1))); arenaA.Put32(o + 8, a[3]); arenaA.Put32(o + 0xC, a[5]); arenaA.Put32(o + 0x10, a[4]);
        break;
    }
    case kTri:
        for (u32 i = 0; i < 14; ++i) call.args[i] = a[3 + i];
        call.args[14] = u32(arenaD.Obj(a[17])); call.argCount = 15;
        break;
    case kLiCtor: case kManDtor: case kManCtor: case kConst:
        break;
    case kStandClone:
        arenaA.Put32(o + 8, a[5]); arenaA.Put8(o + 0x18, u8(a[6])); arenaA.Put8(o + 0x19, u8(a[7])); arenaA.Put32(o + 0x1C, a[8]);
        gNew = a[3] == 0 ? 0u : u32(arenaB.Obj(a[4]));
        break;
    case kDieClone:
        arenaA.Put32(o + 8, a[6]); arenaA.Put32(o + 0xC, a[7]); arenaA.Put32(o + 0x10, a[5]); arenaA.Put32(o + 0x14, a[8]); arenaA.Put32(o + 0x18, a[9]); arenaA.Put32(o + 0x1C, a[10]);
        gNew = a[3] == 0 ? 0u : u32(arenaB.Obj(a[4]));
        gRet = a[11]; gDtorWrite = a[12];
        break;
    case kDrownClone:
        gNew = a[3] == 0 ? 0u : u32(arenaB.Obj(a[4]));
        break;
    case kRegLeave: {
        const u32 ped = a[17] == 0 ? u32(arenaC.Obj(a[7])) : a[18];
        const u32 driver = a[6] == 0 ? 0u : (a[6] == 1 ? ped : (a[6] == 2 ? u32(arenaC.Obj(a[8])) : a[9]));
        const u32 vb = a[3];
        arenaA.Put32(o + 0xC, u32(arenaB.Obj(vb))); arenaA.Put32(o + 0x10, a[4]); arenaA.Put8(o + 0x18, u8(a[5])); arenaA.Put32(o + 0x20, a[19]);
        arenaB.Put8(vb + 0x36, u8(a[13])); arenaB.Put8(vb + 0x3BA, u8(a[14])); arenaB.Put8(vb + 0x3D0, u8(a[15])); arenaB.Put32(vb + 0x460, driver); arenaB.Put8(vb + 0x485, u8(a[16]));
        gRet = a[12]; gPl = {a[10], a[11]};
        call.args[0] = ped; call.argCount = 1;
        break;
    }
    case kLineUp:
        arenaA.Put32(o + 0x10, a[3]);
        gNew = a[4] == 0 ? 0u : u32(arenaC.Obj(a[5]));
        gRet = a[6]; gDtorWrite = a[7];
        call.args[0] = a[8]; call.argCount = 1;
        break;
    case kLcClone:
        arenaA.Put32(o + 0xC, a[5]); arenaA.Put32(o + 0x10, a[6]); arenaA.Put32(o + 0x14, a[7]); arenaA.Put8(o + 0x18, u8(a[8])); arenaA.Put8(o + 0x19, u8(a[9]));
        gNew = a[3] == 0 ? 0u : u32(arenaB.Obj(a[4]));
        gRet = a[10]; gDtorWrite = a[11];
        break;
    case kSdd:
        arenaA.Put32(o, a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    }
    return call;
}

// Despues de Snap: enmascara en la imagen de referencia los punteros del monton que la funcion reemplaza o libera
inline void MaskRefs(const C& c) {
    const u32 o = c.a[0];
    if (kKind[c.fn] == kFlush) arenaA.MaskRef(o, 8, 0x5A);
}

// Despues de la llamada: libera los bloques del monton y normaliza los punteros del monton en la arena A (segunda copia de Flush)
inline void PostCall(const C& c) {
    const auto& a = c.a;
    const u32 o = a[0];
    if (kKind[c.fn] == kFlush) {
        const bool freed = i32(a[3]) > 0 && (a[4] & 0xFF) != 0;
        if (!freed) { ::operator delete[](gHeap[0]); ::operator delete[](gHeap[1]); }
        // si la funcion no toco los punteros siguen siendo los del monton: se normalizan al valor enmascarado de la referencia
        if (arenaA.Get32(o) == u32(uintptr_t(gHeap[0]))) arenaA.Put32(o, 0x5A5A5A5Au);
        if (arenaA.Get32(o + 4) == u32(uintptr_t(gHeap[1]))) arenaA.Put32(o + 4, 0x5A5A5A5Au);
    }
    gHeap.fill(nullptr); gHeapCount = 0;
}

// Clone con construccion en linea (9 y 11): la vtable (+0 del objeto nuevo) es la del EXE o la de la DLL segun el lado; se enmascara
inline void MaskVtable(const C& c) {
    if ((c.fn == 9 || c.fn == 11) && c.a[3] != 0) arenaB.MaskBoth(c.a[4], 4, 0x77);
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
        MaskRefs(c);
        SnapRegions();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.N()); TagAdd(arenaB.m.data(), arenaB.N()); TagAdd(arenaC.m.data(), arenaC.N()); TagAdd(arenaD.m.data(), arenaD.N());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        PostCall(c);
        MaskVtable(c);
        json obs;
        obs["changes"] = {{"A", fn == 7 || fn == 8 ? arenaA.ChangesCompact() : arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kGetAt: case kLiCtor: case kManCtor: case kStandClone: case kDieClone: case kDrownClone: case kLcClone: case kSdd: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case kStat: case kConst: obs["ret"] = eax; break;                                                                                                                  // valor completo de EAX
        case kTri: obs["ret"] = eax & 0xFFu; break;                                                                                                                        // bool: solo AL
        default: obs["ret"] = nullptr; break;                                                                                                                              // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN186A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN186A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn186a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn186a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn186a"); }
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
    // Colaborador REAL con hook RH en la DLL: el EXE corre sus bytes (hook apagado por su ruta) y la DLL su codigo; solo se comprueba que el hook este apagado y que el EXE conserve sus bytes.
    auto REAL = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked) ready = false;
        return it->second;
    };
    env.stubs[9]  = {F(0x61A5A0, &FkNew<0x61A5A0>)};
    env.reals[9]  = {REAL(0x61A390)};
    env.stubs[10] = {F(0x61A5A0, &FkNew<0x61A5A0>), F(0x62FAF0, &FkCtor4<0x62FAF0>), F(0x62FA00, &FkCtor4<0x62FA00>)};
    env.stubs[11] = {F(0x61A5A0, &FkNew<0x61A5A0>)};
    env.reals[11] = {REAL(0x61A390)};
    env.reals[8]  = {REAL(0x5A2E90), REAL(0x5A3070)};
    env.stubs[13] = {F(0x64E550, &FkDoorFlag), F(0x6D3020, &FkSetOut), F(0x5DF8F0, &FkIsPlayer)};
    env.stubs[14] = {ByExeAddress(0x82119A, &FkNew<0x82119A>), F(0x64FBB0, &FkLineUpCtor)};
    env.stubs[15] = {F(0x61A5A0, &FkNew<0x61A5A0>), F(0x63B8C0, &FkCtor5<0x63B8C0>)};
    env.stubs[16] = {F(0x63B970, &FkBody<0x63B970>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[17] = {F(0x63BBD0, &FkBody<0x63BBD0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 12u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[4] = "toda la funcion (la liberacion del bloque real corre con el delete[] real; la DLL llama a su propia primera copia de Flush)";
    dllReal[8] = "toda la funcion; en el EXE corren reales los constructores 0x5A2E90 y 0x5A3070 (hook apagado), 0x4A8DB0, 0x4A8DD0 y el constructor de vectores del CRT 0x821D30";
    dllReal[7] = "toda la funcion (los destructores de vectores del EXE, 0x821E02, no escriben memoria)";
    dllReal[9] = "toda la funcion salvo CTask::operator new (falso); la base CTaskSimple 0x61A390 corre real en el EXE (hook apagado) y en linea en la DLL; la vtable +0 se enmascara";
    dllReal[11] = dllReal[9];
    dllReal[10] = "toda la funcion salvo CTask::operator new y los constructores 0x62FA00 y 0x62FAF0 (con hook, falso identico en el EXE y en la DLL)";
    dllReal[13] = "toda la funcion salvo ComputeDoorFlag, SetGettingOutFlags e IsPlayer (con hook, falso identico en el EXE y en la DLL)";
    dllReal[14] = "toda la funcion salvo operator new (0x82119A, por la direccion del EXE, falso) y el constructor 0x64FBB0 (con hook, falso)";
    dllReal[15] = "toda la funcion salvo CTask::operator new y el constructor 0x63B8C0 (con hook, falso)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        for (const auto& p : env.reals[f]) {
            env.watches.emplace_back(p.exe, 16);
            real.push_back({{"exe", p.exe}, {"dll", p.dll != 0}, {"hooked", p.hooked}, {"dll_addr", p.dll}, {"dll_module", ModuleOf(p.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada: se guarda al empezar y se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kGlobBase, kGlobSize);
    regionsOf[1] = {regions.size() - 1};
    regionsOf[2] = {regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}

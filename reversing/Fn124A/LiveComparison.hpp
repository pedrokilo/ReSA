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

// Banco par a par Fn124A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Trece funciones diminutas (hook S = RH_ScopedInstall / RH_ScopedGlobalInstall / RH_ScopedNamedGlobalInstall: enlace de la entrada hacia la DLL; la 5 es V = RH_ScopedVMTInstall), cada una con 16384 pares EXE/DLL
// (ver Fixtures.hpp para la lista, los datos de cada caso y el esquema de familias):
//   0 BoneNode_c::ArrayElementConstructor 0x617850      1 CZone::GetTranslatedName 0x5720C0       2 CGenericGameStorage::GetNameOfSavedGame 0x5D0F40   3 CTaskComplexBeStill::Constructor 0x5F6660
//   4 CObject::CeilFloat 0x538AD0                       5 CTaskSimpleCarWaitToSlowDown::SetPedPosition 0x646AB0 (V)                              6 CVector2D::Magnitude 0x420860
//   7 CTimer::GetOSWPerformanceTime 0x5617A0            8 BreakManager_c::Constructor 0x59E620    9 CAEVorbisDecoder 0x502700   10 CAEWaveDecoder 0x503430   11 CWanted 0x56F850   12 CRealTimeShadow 0x706850
// El banco invoca la direccion del EXE con un objeto sintetico (arena propia, `this` en un desplazamiento alineado, desalineado o al borde de la ventana).
// Falsos de los colaboradores con hook RH (apagado durante el banco): el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab). Colaboradores sin hook que la DLL llama por la direccion del
// EXE (operator delete 0x8214BD): un solo parche (la DLL los llama por esa direccion, MemoryMgr.cpp). Colaboradores REALES: se ejecutan tal cual (0x4A8DB0, 0x823820, 0x59D170, 0x821D30, 0x56F200).
// QueryPerformanceCounter (funcion 7): se sustituye la casilla de la IAT del EXE (0x8580C8) y la de la DLL por un falso `__stdcall` que escribe el contador del caso (los demas hilos pasan a la funcion real).
// Se observan: los bytes cambiados de las dos arenas, los eventos de los falsos (colaborador, argumentos como etiquetas de arena o crudos, dword que tenia el objeto al entrar), el valor de retorno
// (EAX como etiqueta o crudo, AL, EDX:EAX) y, en las funciones que devuelven en ST0 (4 y 6), el estado x87 completo (el oraculo comprueba ST0 de 80 bits).

namespace F124ALive {
using namespace F124AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x617850, 0x5720C0, 0x5D0F40, 0x5F6660, 0x538AD0, 0x646AB0, 0x420860, 0x5617A0, 0x59E620, 0x502700, 0x503430, 0x56F850, 0x706850};
inline constexpr uintptr_t kQpcExeSlot = 0x8580C8;

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F124AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Pattern(u32 kind, u32 seed) { if (kind == 0) Fill(seed); else memset(m.data(), kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), N); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u8 v) { m[kGuard + off] = v; }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kObjWin> objA;          // 0: objeto (this)
inline Arena<kAWin> aA;              // 1: arena secundaria (ped, vehiculo, centinela)

// ================================================================================================ falsos de los colaboradores
// Datos del caso en curso para los falsos
inline u32 gRet{};                   // EAX que devuelve el falso de CText::Get / SetPedPositionInCar
inline u32 gQpcLo{}, gQpcHi{}, gQpcBool{};
inline DWORD gBenchThread{};
inline uintptr_t gRealQpc{};

// Evento: [direccion del EXE del colaborador, ecx (etiqueta de arena), dword en [ecx] al entrar]
template<u32 ADDR> inline void* __fastcall FkDtor(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    return self;
}
// 0x61A3B0 CTaskComplex::Constructor: [this, dword en [this] al entrar]; no escribe nada (la vtable propia y los campos son del constructor probado)
inline void* __fastcall FkComplexCtor(void* self, void*) {
    Ev& e = PushEv(0x61A3B0, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    return self;
}
// 0x6A0050 CText::Get (thiscall, un argumento de pila): [ecx crudo, clave (etiqueta de arena), dos dwords de la clave]; devuelve el valor del caso
inline u32 __fastcall FkTextGet(void* self, void*, const char* key) {
    Ev& e = PushEv(0x6A0050, 4);
    e.v[0] = u32(uintptr_t(self));
    e.v[1] = Tag(key);
    e.v[2] = BitsOf(key);
    e.v[3] = BitsOf(key + 4);
    return gRet;
}
// 0x5DF910 CPed::SetPedPositionInCar (thiscall sin argumentos de pila): [ped (etiqueta o crudo)]; devuelve el valor del caso (el EXE lo pisa con AL = 1)
inline u32 __fastcall FkSetPedPos(void* self, void*) {
    Ev& e = PushEv(0x5DF910, 1);
    e.v[0] = Tag(self);
    return gRet;
}
// 0x8214BD operator delete (cdecl): la DLL lo llama por esta misma direccion (MemoryMgr.cpp). No libera nada: [puntero (etiqueta)]
inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(0x8214BD, 1);
    e.v[0] = Tag(p);
}
// QueryPerformanceCounter (stdcall, un argumento): escribe el contador del caso; evento [alineacion del puntero, el buffer esta justo encima del marco del falso]. Otros hilos: la funcion real.
inline BOOL __stdcall FkQpc(LARGE_INTEGER* p) {
    if (GetCurrentThreadId() != gBenchThread) return reinterpret_cast<BOOL(__stdcall*)(LARGE_INTEGER*)>(gRealQpc)(p);
    volatile int probe = 0;
    Ev& e = PushEv(1, 2);
    e.v[0] = u32(uintptr_t(p) & 3);
    e.v[1] = (uintptr_t(p) > uintptr_t(&probe) && uintptr_t(p) - uintptr_t(&probe) < 0x400) ? 1u : 0u;
    p->LowPart = gQpcLo;
    p->HighPart = LONG(gQpcHi);
    return BOOL(gQpcBool);
}

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

// Puntero de un caso de la funcion 5 segun su modo: 0 nulo, 1 arena secundaria (+ desplazamiento), 2 crudo
inline u32 PtrOf(u32 mode, u32 raw, size_t off) { return mode == 0 ? 0u : mode == 1 ? u32(aA.Obj(off)) : raw; }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        objA.Pattern(c.a[2], sd++); aA.Fill(sd++);
        const bool hasThis = fn != 2 && fn != 4 && fn != 7;
        const size_t off = c.a[0];
        const uintptr_t self = hasThis ? objA.Obj(off) : 0;
        u32 args[2]{};
        u32 argCount = 0, popBytes = 0, ecxSeed = hasThis ? u32(self) : c.ecx;
        switch (fn) {
        case 0: objA.Put32(off, c.a[6]); objA.Put32(off + 4, c.a[7]); break;
        case 1: objA.Put32(off + 8, c.a[6]); objA.Put32(off + 12, c.a[7]); gRet = c.a[8]; break;
        case 2: args[0] = c.a[0]; argCount = 1; popBytes = 4; break;
        case 3: objA.Put32(off, c.a[6]); objA.Put32(off + 4, c.a[7]); objA.Put32(off + 8, c.a[8]); break;
        case 4: args[0] = c.a[0]; argCount = 1; popBytes = 4; break;
        case 5:
            objA.Put32(off + 8, PtrOf(c.a[6], c.a[7], 0x20));
            args[0] = PtrOf(c.a[8], c.a[9], c.a[10]); argCount = 1; gRet = c.a[11];
            break;
        case 6: objA.Put32(off, c.a[6]); objA.Put32(off + 4, c.a[7]); break;
        case 7: gQpcLo = c.a[6]; gQpcHi = c.a[7]; gQpcBool = c.a[8]; break;
        case 8:
            for (u32 e = 0; e < 64; ++e) {
                const bool active = ((e < 32 ? c.a[9] >> e : c.a[10] >> (e - 32)) & 1) != 0;
                const size_t at = off + 0x20 * e;
                objA.Put8(at, active ? u8((e + 1) * c.a[6]) : 0);
                objA.Put8(at + 1, active ? u8((e + 3) * c.a[7]) : 0);
                objA.Put32(at + 8, active ? u32((e + 1) * c.a[8]) : 0u);
            }
            break;
        default:   // 9..12: dword previo en [ECX] y banderas
            objA.Put32(off + kEcxAdj[fn], c.a[6]); args[0] = c.a[7]; argCount = 1; break;
        }
        objA.Snap(); aA.Snap();
        TagClear(); TagAdd(objA.m.data(), objA.m.size()); TagAdd(aA.m.data(), aA.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, args, argCount, c.eax, ecxSeed, c.edx, c.stack, popBytes);
        });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, objA.Changes()); add(1, aA.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax, edx = sides[s].reg.edx;
        switch (fn) {
        case 1: case 2: obs["ret"] = eax; break;                                    // valor crudo
        case 0: case 3: case 8: case 9: case 10: case 11: case 12: obs["ret"] = Tag(uintptr_t(eax)); break;   // this como etiqueta de arena
        default: obs["ret"] = nullptr; break;
        }
        obs["al"] = fn == 5 ? json(eax & 0xFFu) : json(nullptr);
        obs["q"] = fn == 7 ? json::array({eax, edx}) : json(nullptr);
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
    }
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        gBenchThread = GetCurrentThreadId();
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < kCases; ++c) CaseRun(env, totals, f, c);
            Write({{"event", "function_summary"}, {"function", kFn[f]}, {"cases", kCases}, {"failures", totals.failures - before}, {"state_restored", env.WatchesIntact() && RegionsIntact()}});
        }
    }
    RestoreRegions();
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
    if (!std::filesystem::exists(dir / "fn124a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn124a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn124a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado (el Virtual apaga tambien el salto de la entrada del EXE).
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
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    auto Real = [](uintptr_t exe) { return Pair{exe, 0, false}; };
    env.stubs[0] = {};                          env.reals[0] = {Real(0x4A8DB0)};
    env.stubs[1] = {F(0x6A0050, &FkTextGet)};
    env.stubs[2] = {};
    env.stubs[3] = {F(0x61A3B0, &FkComplexCtor)};
    env.stubs[4] = {};                          env.reals[4] = {Real(0x823820)};
    env.stubs[5] = {F(0x5DF910, &FkSetPedPos)};
    env.stubs[6] = {};
    env.stubs[7] = {};
    env.stubs[8] = {};                          env.reals[8] = {Real(0x59D170), Real(0x821D30)};
    env.stubs[9] = {F(0x5023F0, &FkDtor<0x5023F0>), F(0x8214BD, &FkDelete)};
    env.stubs[10] = {F(0x503290, &FkDtor<0x503290>), F(0x8214BD, &FkDelete)};
    env.stubs[11] = {F(0x4E6E00, &FkDtor<0x4E6E00>), F(0x8214BD, &FkDelete)};   env.reals[11] = {Real(0x56F200)};
    env.stubs[12] = {F(0x706440, &FkDtor<0x706440>), F(0x8214BD, &FkDelete)};
    // QueryPerformanceCounter: casilla de la IAT del EXE (0x8580C8) y la de la DLL; las dos deben apuntar a la misma funcion del sistema
    uintptr_t* const exeSlot = reinterpret_cast<uintptr_t*>(kQpcExeSlot);
    uintptr_t* const dllSlot = FindImportSlot(env.asi, "QueryPerformanceCounter");
    json qpc = {{"exe_slot", kQpcExeSlot}, {"dll_slot_found", dllSlot != nullptr}};
    if (!dllSlot || *dllSlot != *exeSlot || !*exeSlot) ready = false;
    if (dllSlot) {
        qpc["same_function"] = *dllSlot == *exeSlot;
        gRealQpc = *exeSlot;
        env.iat[7] = {IatSpec{exeSlot, reinterpret_cast<void*>(&FkQpc)}, IatSpec{dllSlot, reinterpret_cast<void*>(&FkQpc)}};
        env.watches.emplace_back(kQpcExeSlot, 4);
    }
    const char* dllReal[kFunctions] = {
        "toda la funcion: pone a 0 m_pPrev y m_pNext (ListItem_c::ListItem_c escrito en linea); el EXE llama a 0x4A8DB0 (real)",
        "toda la funcion salvo CText::Get (0x6A0050, falso identico): pasa this + 8 y devuelve su resultado",
        "toda la funcion: 0xC16368 + slot * 0x104 en 32 bits",
        "escritura de la vtable (0x86C7BC) en [this]; la base CTaskComplex::Constructor (0x61A3B0) es un falso identico",
        "toda la funcion salvo el ceil del CRT (0x823820, real, llamado por la direccion del EXE): amplia el float a double y devuelve el resultado en ST0",
        "toda la funcion salvo CPed::SetPedPositionInCar (0x5DF910, falso identico): lee [this+8], llama con el ped y devuelve true",
        "toda la funcion: raiz de x*x + y*y en la x87 de 24 bits, ST0 sin redondear a float",
        "toda la funcion salvo QueryPerformanceCounter (falso en la casilla de la IAT del EXE y en la de la DLL)",
        "toda la funcion: 64 BreakObject_c en orden con el constructor de la clase (los campos +0, +1 y +8 a 0); el EXE llama al iterador CRT 0x821D30 y a 0x59D170 (reales)",
        "toda la funcion salvo el destructor CAEVorbisDecoder (0x5023F0, envoltura Destructor() con falso identico) y operator delete (0x8214BD, falso)",
        "toda la funcion salvo el destructor CAEWaveDecoder (0x503290, envoltura Destructor() con falso identico) y operator delete (0x8214BD, falso)",
        "toda la funcion salvo el destructor de CAEPoliceScannerAudioEntity (0x4E6E00, envoltura Destructor() con falso identico; el EXE llega por 0x56F200 real) y operator delete (0x8214BD, falso)",
        "toda la funcion salvo el destructor de CRealTimeShadow (0x706440, envoltura Destructor() con falso identico) y operator delete (0x8214BD, falso)"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        for (const auto& r : env.reals[f]) {
            env.watches.emplace_back(r.exe, 16);
            real.push_back({{"exe", r.exe}, {"dll", r.dll != 0}, {"hooked", r.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()},
                                 {"qpc", f == 7 ? qpc : json(nullptr)}};
    }
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}

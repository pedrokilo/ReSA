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

// Banco par a par Fn126A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Trece destructores virtuales con borrado (hook S: RH_ScopedInstall de la envoltura/miembro ScalarDeletingDestructor: enlace de la entrada del EXE hacia la DLL), cada uno con 16384 pares EXE/DLL
// (ver Fixtures.hpp para la lista de clases, cuerpos y operator delete, y el esquema de familias).
// El banco invoca la direccion del EXE con un objeto sintetico (arena propia, `this` en un desplazamiento alineado, desalineado o al borde de la ventana, [this] = vtable previa del caso).
// Falsos de los colaboradores: el cuerpo del destructor (con hook RH, apagado durante el banco, en doce clases de tarea; sin hook en CTaskSimpleStealthKill 0x622680, donde el cuerpo de
// la DLL es una funcion propia exportada por su .cpp) y el operator delete de la clase (CTask 0x61A5B0 con hook). El mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair).
// Se observan: los bytes cambiados de la arena (la vtable que deja el falso del cuerpo), los eventos de los falsos (colaborador, ECX o argumento como etiqueta de arena, dword que tenia [ECX] o [argumento]
// al entrar: el del borrado prueba que se borra DESPUES de destruir), el valor de retorno (EAX como etiqueta de arena = this) y el estado x87 (debe quedar vacio). EAX/ECX/EDX tras la llamada quedan en
// original_residue / candidate_residue (informativos: ningun llamador del EXE los lee salvo EAX = this).

// Cuerpos de destructor SIN hook de la DLL (lote 126A): funciones propias, exportadas por el .cpp de cada clase; son lo que el cuerpo de la DLL llama como el `call BODY` del EXE.
uintptr_t Lote126AStealthKillDestructorBody();      // 0x622680, TaskSimpleStealthKill.cpp

namespace F126ALive {
using namespace F126AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x640590, 0x640F10, 0x640640, 0x640710, 0x640BC0, 0x6560C0, 0x697820, 0x6629F0, 0x64ABE0, 0x697B50, 0x68F9F0, 0x6296B0, 0x626FA0};
inline constexpr uintptr_t kBodies[kFunctions] = {0x63AE00, 0x63B850, 0x63B0C0, 0x63B3C0, 0x63B4A0, 0x653A30, 0x696E80, 0x65EBB0, 0x645BE0, 0x697230, 0x68C8C0, 0x622680, 0x621250};
inline constexpr uintptr_t kDeletes[kFunctions] = {0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F126AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Pattern(u32 kind, u32 seed) { if (kind == 0) Fill(seed); else memset(m.data(), kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), N); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    // Cambios respecto a la imagen de referencia
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kObjWin> objA;          // 0: objeto (this)

// ================================================================================================ falsos de los colaboradores
// Datos del caso en curso para los falsos
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this] (la vtable de la base)
inline u32 gRet{};                   // EAX que devuelven los falsos (no es this)
inline DWORD gBenchThread{};

// Cuerpo del destructor (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// operator delete de la clase (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
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

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        objA.Pattern(c.a[2], sd++);
        const size_t off = c.a[0];
        const uintptr_t self = objA.Obj(off);
        u32 args[1]{c.a[7]};                    // banderas (el EXE lee solo el byte bajo; el resto de la ranura de pila son bits del caso)
        const u32 argCount = 1, popBytes = 0;   // `ret 4` del callee
        objA.Put32(off, c.a[6]);
        gDtorWrite = c.a[8]; gRet = c.a[9];
        objA.Snap();
        TagClear(); TagAdd(objA.m.data(), objA.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, args, argCount, c.eax, u32(self), c.edx, c.stack, popBytes);
        });
        json obs;
        json ch = json::array();
        for (const auto& e : objA.Changes()) ch.push_back(json::array({0u, e[0], e[1]}));
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        obs["ret"] = Tag(uintptr_t(sides[s].reg.eax));    // this como etiqueta de arena
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
    if (!std::filesystem::exists(dir / "fn126a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn126a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn126a"); }
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
    // Colaborador SIN hook RH: la DLL tiene su propia funcion (direccion dada); el falso va en la direccion del EXE y en esa funcion. Se exige que de verdad no tenga hook.
    auto U = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        const uintptr_t body = Body(dllAddress);
        if (!body || ModuleOf(body) != env.asi) ready = false;
        return S(Pair{exe, body, false}, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {F(0x63AE00, &FkBody<0x63AE00>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[1] = {F(0x63B850, &FkBody<0x63B850>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[2] = {F(0x63B0C0, &FkBody<0x63B0C0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[3] = {F(0x63B3C0, &FkBody<0x63B3C0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[4] = {F(0x63B4A0, &FkBody<0x63B4A0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[5] = {F(0x653A30, &FkBody<0x653A30>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[6] = {F(0x696E80, &FkBody<0x696E80>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[7] = {F(0x65EBB0, &FkBody<0x65EBB0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[8] = {F(0x645BE0, &FkBody<0x645BE0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[9] = {F(0x697230, &FkBody<0x697230>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[10] = {F(0x68C8C0, &FkBody<0x68C8C0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[11] = {U(0x622680, Lote126AStealthKillDestructorBody(), &FkBody<0x622680>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[12] = {F(0x621250, &FkBody<0x621250>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    const char* dllReal[kFunctions] = {
        "toda la funcion salvo ~CTaskComplexEnterCarAsDriverTimed (0x63AE00, envoltura Destructor() con falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)",
        "toda la funcion salvo ~CTaskComplexStealCar (0x63B850, envoltura Destructor() con falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)",
        "toda la funcion salvo ~CTaskComplexEnterCarAsPassengerTimed (0x63B0C0, envoltura Destructor() con falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)",
        "toda la funcion salvo ~CTaskComplexEnterCarAsPassengerWait (0x63B3C0, envoltura Destructor() con falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)",
        "toda la funcion salvo ~CTaskComplexLeaveCarAsPassengerWait (0x63B4A0, envoltura Destructor() con falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)",
        "toda la funcion salvo ~CTaskComplexHitPedWithCar (0x653A30, envoltura Destructor() con falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)",
        "toda la funcion salvo ~CTaskComplexInvestigateDeadPed (0x696E80, envoltura Destructor() con falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)",
        "toda la funcion salvo ~CTaskComplexGangFollower (0x65EBB0, envoltura Destructor() con falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)",
        "toda la funcion salvo ~CTaskSimpleCarAlign (0x645BE0, envoltura Destructor() con falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)",
        "toda la funcion salvo ~CTaskComplexReactToGunAimedAt (0x697230, envoltura Destructor() con falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)",
        "toda la funcion salvo ~CTaskComplexCopInCar (0x68C8C0, envoltura Destructor() con falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)",
        "toda la funcion salvo ~CTaskSimpleStealthKill (0x622680, funcion propia sin hook, falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)",
        "toda la funcion salvo ~CTaskComplexKillPedOnFootArmed (0x621250, envoltura Destructor() con falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}

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

// Banco par a par Fn142A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 23 funciones diminutas del EXE (ocho destructores con borrado, cinco cuerpos de destructor, cuatro constructores, tres retornos de constante de CEventDanger, CEventVehicleHitAndRun::IsCriminalEvent y el
// destructor de CAEStreamThread; ver Fixtures.hpp para la lista y el esquema de familias), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall) o V
// (RH_ScopedVMTInstall: ranura de la vtable del EXE; el Virtual lleva dentro un Simple sobre la entrada del EXE, que es lo que el banco enciende). El banco invoca la direccion del EXE con objetos
// sinteticos en arenas propias (A, B, C con guardas de 0x40). Falsos de los colaboradores: el mismo falso en el EXE y en el cuerpo/funcion de la DLL (MakeCollab / Pair): cuerpos de destructor con hook
// (0x658850, 0x48E600, 0x4636E0, 0x492D40, 0x5A40E0 y 0x697330, que son tambien funciones del lote o de lotes anteriores), ~CTaskComplexWander (0x669D20), ~CTask (0x61A3A0), CTask::operator delete
// (0x61A5B0), CPedList::Empty (0x699DB0) y CPed::IsPlayer (0x5DF8F0), todos con hook; SIN hook: ~CObject (0x59F660), CObject::operator delete (0x5A1F20) y los cuerpos 0x4F0B50 y 0x5F67E0 (funciones
// propias de la DLL), mas operator delete 0x8214BD (la DLL lo llama por la direccion del EXE) y la casilla de DeleteCriticalSection de la IAT (la del EXE y la de la DLL).
// Se observan: los bytes cambiados de las tres arenas, los eventos de los falsos (con sus argumentos, ECX y el dword de [ECX] al entrar), el valor devuelto (EAX completo, etiqueta de this, o AL en
// IsCriminalEvent), el estado x87 (vacio) y el contrato de ABI. EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Funciones propias SIN hook de la DLL (lote 142A): lo que el cuerpo de la DLL llama como el `call BODY` del EXE; las exportan los .cpp de cada clase (solo las usa el banco).
uintptr_t Lote142AStaticChannelDestructorBody();           // 0x4F0B50, AEStaticChannel.cpp
uintptr_t Lote142ALeaderAreaCodesMatchDestructorBody();    // 0x5F67E0, TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp
namespace CObjectBody { void __fastcall Destructor(CObject* self, void* edx); }   // 0x59F660, Object.cpp (sin hook)

namespace F142ALive {
using namespace F142AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x658830, 0x658850, 0x48E5E0, 0x48E600, 0x4636C0, 0x4636E0, 0x492D40, 0x492DF0, 0x4C5680, 0x4C5690, 0x5A40E0, 0x5A40F0, 0x4F1020, 0x4F11D0, 0x4F11E0,
                                                  0x697EF0, 0x6986E0, 0x5F6880, 0x5F68A0, 0x4B2670, 0x4B2680, 0x4B2690, 0x4B27B0};
inline constexpr uintptr_t kDeleteCsExeSlot = 0x8580E0;   // casilla de la IAT del EXE de DeleteCriticalSection

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F142AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos (no es this)
inline u32 gDtorWrite{};             // dword que el falso del cuerpo del destructor deja en [this] (la vtable de la base)
inline DWORD gBenchThread{};

// Cuerpo del destructor con borrado (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self); e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Colaborador de un cuerpo de destructor (~CTaskComplexWander 0x669D20, ~CTask 0x61A3A0, ~CObject 0x59F660) y CPedList::Empty (0x699DB0): evento [direccion, ECX (etiqueta), dword de [ECX] al entrar
// (la vtable que dejo el cuerpo del destructor)]; devuelve gRet. No escribe nada.
template<u32 ADDR> inline u32 __fastcall FkCollab(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self); e.v[1] = BitsOf(self);
    return gRet;
}
// operator delete (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p); e.v[1] = BitsOf(p);
}
// DeleteCriticalSection (stdcall, un argumento): evento [direccion de la casilla de la IAT del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No hace nada.
inline void __stdcall FkDeleteCs(void* cs) {
    Ev& e = PushEv(0x8580E0, 2);
    e.v[0] = Tag(cs); e.v[1] = BitsOf(cs);
}
// CPed::IsPlayer (0x5DF8F0, thiscall sin pila): evento [direccion, ECX (etiqueta o crudo)]; devuelve gRet. No desreferencia ECX (puede ser un valor crudo).
inline u32 __fastcall FkIsPlayer(void* self, void*) {
    Ev& e = PushEv(0x5DF8F0, 1);
    e.v[0] = Tag(self);
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
    std::array<u32, 4> args{};
    u32 argCount{}, popBytes{}, ecx{};
};

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    auto thisCall = [&](u32 argCount) {
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = argCount; call.popBytes = 0;   // `ret n` del callee: lo limpia el EXE o la DLL
    };
    if (IsSdd(c.fn)) {                // thiscall con un argumento (`ret 4`): banderas
        arenaA.Put32(a[0], a[3]);
        gDtorWrite = a[5]; gRet = a[6];
        thisCall(1); call.args[0] = a[4];
        return call;
    }
    if (IsBodyWithCollab(c.fn)) {     // cuerpo de destructor: [this] = vtable previa
        arenaA.Put32(a[0], a[3]);
        gRet = a[4];
        thisCall(0);
        return call;
    }
    switch (c.fn) {
    case kIdxLodCtor:
        arenaA.Put32(a[0], a[3]); arenaA.Put32(a[0] + 8, a[4]); arenaA.Put32(a[0] + 0x20, a[5]); arenaA.Put32(a[0] + 0x24, a[6]);
        thisCall(0);
        break;
    case kIdxStreamDtor:   // ECX = this; la seccion critica esta en +0x38
        arenaA.Put32(a[0] + 0x38, a[3]);
        thisCall(0);
        break;
    case kIdxPointCtor:
        arenaA.Put32(a[0], a[3]); arenaA.Put32(a[0] + 0x124, a[4]);
        thisCall(0);
        break;
    case kIdxHitRunCriminal: {   // [this + 0x10] = vehiculo (arena B); [vehiculo + 0x460] = conductor
        gRet = a[7];
        thisCall(0);
        const u32 driver = a[4] == 0 ? 0u : (a[4] == 1 ? u32(arenaC.Obj(a[5])) : a[6]);
        arenaB.Put32(a[3] + 0x460, driver);
        arenaA.Put32(a[0] + 0x10, u32(arenaB.Obj(a[3])));
        break;
    }
    default:               // CTimeModelInfo::Destructor, CAEStreamThread::Constructor, CPedList::Constructor y las constantes: `this` en la arena A, [this] = dword del caso
        arenaA.Put32(a[0], a[3]);
        thisCall(0);
        break;
    }
    return call;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        gRet = 0; gDtorWrite = 0;
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}};
        obs["gchanges"] = json::array();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        if (IsSdd(fn) || fn == kIdxLodCtor || fn == kIdxStreamCtor || fn == kIdxPointCtor || fn == kIdxPedListCtor) obs["ret"] = Tag(uintptr_t(eax));   // this
        else if (fn == kIdxHitRunCriminal) obs["ret"] = eax & 0xFFu;                                                                              // bool: AL
        else if (fn == kIdxDangerType || fn == kIdxDangerLife || fn == kIdxDangerPriority) obs["ret"] = eax;                                       // EAX completo
        else obs["ret"] = nullptr;                                                                                                                 // void
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN142A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN142A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn142a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn142a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn142a"); }
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
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (operator delete 0x8214BD, MemoryMgr.cpp): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH: la DLL tiene su propia funcion (direccion dada); el falso va en la direccion del EXE y en esa funcion. Se exige que de verdad no tenga hook.
    auto U = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        const uintptr_t body = Body(dllAddress);
        if (!body || ModuleOf(body) != env.asi) ready = false;
        return S(Pair{exe, body, false}, reinterpret_cast<void*>(fake));
    };
    using VoidDelete = void (*)(void*);
    const uintptr_t objectDelete = reinterpret_cast<uintptr_t>(static_cast<VoidDelete>(&CObject::operator delete));
    const uintptr_t objectDtor = reinterpret_cast<uintptr_t>(&CObjectBody::Destructor);
    env.stubs[kIdxMedicSdd] = {F(0x658850, &FkBody<0x658850>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[kIdxMedicBody] = {F(0x669D20, &FkCollab<0x669D20>)};
    env.stubs[kIdxStandardSdd] = {F(0x48E600, &FkBody<0x48E600>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[kIdxStandardBody] = {F(0x669D20, &FkCollab<0x669D20>)};
    env.stubs[kIdxFinishSdd] = {F(0x4636E0, &FkBody<0x4636E0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[kIdxFinishBody] = {F(0x61A3A0, &FkCollab<0x61A3A0>)};
    env.stubs[kIdxScannerBody] = {F(0x61A3A0, &FkCollab<0x61A3A0>)};
    env.stubs[kIdxScannerSdd] = {F(0x492D40, &FkBody<0x492D40>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[kIdxProjectileBody] = {U(0x59F660, objectDtor, &FkCollab<0x59F660>)};
    env.stubs[kIdxProjectileSdd] = {F(0x5A40E0, &FkBody<0x5A40E0>), U(0x5A1F20, objectDelete, &FkDelete<0x5A1F20>)};
    env.stubs[kIdxStaticSdd] = {U(0x4F0B50, Lote142AStaticChannelDestructorBody(), &FkBody<0x4F0B50>), ByExeAddress(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[kIdxBackupSdd] = {F(0x697330, &FkBody<0x697330>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[kIdxLeaderSdd] = {U(0x5F67E0, Lote142ALeaderAreaCodesMatchDestructorBody(), &FkBody<0x5F67E0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[kIdxPedListCtor] = {F(0x699DB0, &FkCollab<0x699DB0>)};
    env.stubs[kIdxHitRunCriminal] = {F(0x5DF8F0, &FkIsPlayer)};
    // DeleteCriticalSection: casilla de la IAT del EXE (0x8580E0) y la de la DLL; las dos deben apuntar a la misma funcion del sistema
    uintptr_t* const exeSlot = reinterpret_cast<uintptr_t*>(kDeleteCsExeSlot);
    uintptr_t* const dllSlot = FindImportSlot(env.asi, "DeleteCriticalSection");
    json delCs = {{"exe_slot", kDeleteCsExeSlot}, {"dll_slot_found", dllSlot != nullptr}};
    if (!dllSlot || *dllSlot != *exeSlot || !*exeSlot) ready = false;
    if (dllSlot) {
        delCs["same_function"] = *dllSlot == *exeSlot;
        env.iat[kIdxStreamDtor] = {IatSpec{exeSlot, reinterpret_cast<void*>(&FkDeleteCs)}, IatSpec{dllSlot, reinterpret_cast<void*>(&FkDeleteCs)}};
        env.watches.emplace_back(kDeleteCsExeSlot, 4);
    }
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion (sin colaboradores)";
    const char* const sddHooked = "toda la funcion salvo el cuerpo del destructor (con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL) y CTask::operator delete (0x61A5B0, con hook, falso identico)";
    for (const unsigned f : {kIdxMedicSdd, kIdxStandardSdd, kIdxFinishSdd, kIdxScannerSdd, kIdxBackupSdd}) dllReal[f] = sddHooked;
    dllReal[kIdxMedicBody] = dllReal[kIdxStandardBody] = "toda la funcion salvo ~CTaskComplexWander (cuerpo, 0x669D20, con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kIdxFinishBody] = dllReal[kIdxScannerBody] = "toda la funcion salvo ~CTask (cuerpo, 0x61A3A0, con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kIdxProjectileBody] = "toda la funcion salvo ~CObject (0x59F660, sin hook: funcion propia de la DLL, falso identico en la direccion del EXE y en esa funcion)";
    dllReal[kIdxProjectileSdd] = "toda la funcion salvo el cuerpo ~CProjectile (0x5A40E0, con hook, falso identico) y CObject::operator delete (0x5A1F20, sin hook, falso identico en el EXE y en la funcion de la DLL)";
    dllReal[kIdxStaticSdd] = "toda la funcion salvo el cuerpo ~CAEStaticChannel (0x4F0B50, sin hook: funcion propia de la DLL, falso identico) y operator delete (0x8214BD: la DLL lo llama por la direccion del EXE, falso)";
    dllReal[kIdxLeaderSdd] = "toda la funcion salvo el cuerpo ~CTaskSimpleWaitUntilLeaderAreaCodesMatch (0x5F67E0, sin hook: funcion propia de la DLL, falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)";
    dllReal[kIdxLodCtor] = "toda la funcion (el constructor de CBaseModelInfo, 0x4C4A60, sin hook, va en linea en la DLL: sus escrituras se observan en los bytes del objeto)";
    dllReal[kIdxPointCtor] = "toda la funcion (0x699F00, sin hook, va en linea en la DLL: sus escrituras se observan en los bytes del objeto)";
    dllReal[kIdxPedListCtor] = "toda la funcion salvo CPedList::Empty (0x699DB0, con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kIdxStreamDtor] = "toda la funcion salvo DeleteCriticalSection (casilla de la IAT del EXE y de la DLL, falso identico)";
    dllReal[kIdxHitRunCriminal] = "toda la funcion salvo CPed::IsPlayer (0x5DF8F0, con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        if (f == kIdxLodCtor) env.watches.emplace_back(0x4C4A60, 16);     // el EXE sigue llamando a este constructor de CBaseModelInfo (sin hook): debe seguir intacto
        if (f == kIdxPointCtor) env.watches.emplace_back(0x699F00, 16);   // idem para la funcion que pone a cero el CPointList
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        for (const auto& r : env.reals[f]) {
            env.watches.emplace_back(r.exe, 16);
            real.push_back({{"exe", r.exe}, {"dll", r.dll != 0}, {"hooked", r.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()},
                                 {"iat", f == kIdxStreamDtor ? delCs : json(nullptr)}};
    }
    regions.clear();   // este lote no toca memoria global del juego
    Run(env, ready, registered, bodies, collaborators);
}
}

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

// Banco par a par Fn183A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 19 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall / RH_ScopedOverloadedInstall). El banco invoca la direccion del EXE con objetos sinteticos en
// cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): los cuerpos de destructor con hook (0x61A6F0 de CTaskSimpleAnim,
// 0x62F360, 0x62F5D0, 0x62FB40), CTask::Destructor 0x61A3A0 y CTask::operator delete 0x61A5B0 (todos con hook, apagado durante el banco). Los cuerpos 0x62F650 y 0x62F6D0 (`mov [ecx],vtable; jmp 0x61A3A0`) corren REALES en el EXE (con su
// hook apagado) y la DLL corre su funcion local equivalente (Lote183ABody_*). Solo en la direccion del EXE (la DLL lo llama por ella): operator delete del CRT 0x8214BD (Shutdown de CLinkList).
// Memoria global del juego tocada (restaurada tras cada lado): palabras de Enter / extenter de NewKeyState y OldKeyState (0xB733CC, 0xB7315C, 0xB733EA, 0xB7317A, con su vecina) y las 256 palabras de teclas estandar de ambos
// estados (0xB731A8 y 0xB72F38).
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria global, los eventos de los falsos (con sus argumentos),
// el valor devuelto (punteros como etiqueta de arena; solo AL en IsEnterJustPressed; EAX completo en las dos de teclas; el TEXTO de la cadena en ConvertBoneTag2BoneName; sin valor en Shutdown) y el estado x87 (siempre vacio).
// EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

uintptr_t Lote183ABody_CTaskSimpleSetKindaStayInSamePlace();   // 0x62F650 (hook Destructor del compilador): funcion local con el cuerpo del EXE
uintptr_t Lote183ABody_CTaskSimpleDuckToggle();                // 0x62F6D0 (idem)

namespace F183ALive {
using namespace F183AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F183AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(i32 off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(i32 off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
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
inline u32 gRet{};                   // EAX que devuelven los falsos de cuerpo
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this]
inline const u8* gLinksField{};      // CLinkList::Shutdown: direccion de [this + 0x30] (el falso de operator delete lee su valor al entrar)
inline DWORD gBenchThread{};

// Cuerpo del destructor / CTask::Destructor (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CTask::operator delete (0x61A5B0; cdecl, un argumento): evento [direccion, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// operator delete del CRT (0x8214BD; cdecl, un puntero CRUDO que puede no apuntar a memoria valida): en el hilo del banco evento [direccion, argumento (etiqueta o crudo), dword de [this + 0x30] de la lista al entrar
// (aun el puntero viejo)]; en otros hilos no hace nada. No libera nada y no lo desreferencia.
template<u32 ADDR> inline void __cdecl FkDeleteLink(void* p) {
    if (GetCurrentThreadId() != gBenchThread) return;
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = gLinksField ? BitsOf(gLinksField) : 0u;
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

inline void W32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }
inline void W16(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 2); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[60]); arenaB.Pattern(a[2], a[61]); arenaC.Pattern(a[2], a[62]); arenaD.Pattern(a[2], a[63]);
    gRet = 0; gDtorWrite = 0; gLinksField = nullptr;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kShut:
        arenaA.Put32(o + 0x30, a[4]);
        gLinksField = arenaA.m.data() + kGuard + o + 0x30;
        break;
    case kLInsert: {
        const u32 freeN = a[4], usedN = a[5];
        const uintptr_t list = thisA;
        const uintptr_t usedHead = list, usedTail = list + 0xC, freeHead = list + 0x18, freeTail = list + 0x24;
        auto node = [&](u32 j) { return arenaB.Obj(a[6] + 0xC * j); };
        for (u32 j = 0; j < 5; ++j) W32(node(j), a[9 + j]);                                 // data
        W32(usedHead + 8, usedN ? u32(node(0)) : u32(usedTail));
        W32(usedTail + 4, usedN ? u32(node(usedN - 1)) : u32(usedHead));
        for (u32 j = 0; j < usedN; ++j) {
            W32(node(j) + 4, j ? u32(node(j - 1)) : u32(usedHead));
            W32(node(j) + 8, j + 1 < usedN ? u32(node(j + 1)) : u32(usedTail));
        }
        W32(freeHead + 8, freeN ? u32(node(usedN)) : u32(freeTail));
        W32(freeTail + 4, freeN ? u32(node(usedN + freeN - 1)) : u32(freeHead));
        for (u32 j = 0; j < freeN; ++j) {
            W32(node(usedN + j) + 4, j ? u32(node(usedN + j - 1)) : u32(freeHead));
            W32(node(usedN + j) + 8, j + 1 < freeN ? u32(node(usedN + j + 1)) : u32(freeTail));
        }
        switch (a[14]) {
        case 1: W32(usedHead + 8, u32(node(usedN))); break;
        case 2: W32(node(usedN) + 8, u32(node(usedN))); break;
        case 3: W32(node(usedN) + 4, u32(node(usedN))); break;
        case 4: W32(usedHead + 8, u32(usedHead)); break;
        default: break;
        }
        arenaC.Put32(a[7], a[8]);
        call.args[0] = u32(arenaC.Obj(a[7])); call.argCount = 1;
        break;
    }
    case kBone:
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 4;   // cdecl: lo limpia el llamador
        break;
    case kEnter:
        W16(kNewEnter, a[4]); W16(kNewEnter + 2, a[8]);
        W16(kOldEnter, a[5]); W16(kOldEnter + 2, a[9]);
        W16(kNewExt, a[6]); W16(kNewExt + 2, a[10]);
        W16(kOldExt, a[7]); W16(kOldExt + 2, a[11]);
        call.ecx = a[12];
        break;
    case kKey: {
        FillKind(reinterpret_cast<u8*>(kNewKeys), kKeysBytes, a[2], a[56]);
        FillKind(reinterpret_cast<u8*>(kOldKeys), kKeysBytes, a[2], a[57]);
        const u32 key = a[4] & 0xFFu;
        W16(kNewKeys + 2 * key, a[5]); W16(kOldKeys + 2 * key, a[6]);
        call.args[0] = a[4]; call.argCount = 1;                      // `ret 4`: lo limpia el llamado
        call.ecx = a[7] == 0 ? thisA : (a[7] == 1 ? a[8] : 0u);
        break;
    }
    case kCross:
        arenaA.Put16(o + 0x20, a[4]); arenaA.Put16(o + 0x50, a[5]);
        break;
    case kSdd:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    }
    return call;
}

// Texto de una cadena devuelta por la funcion (o nulo): se lee con comprobacion de memoria legible
inline json StringOrNull(u32 p) {
    if (!p) return nullptr;
    if (IsBadReadPtr(reinterpret_cast<const void*>(uintptr_t(p)), 1)) return "<ptr invalido>";
    std::string s;
    for (u32 i = 0; i < 64; ++i) {
        if (IsBadReadPtr(reinterpret_cast<const void*>(uintptr_t(p) + i), 1)) return "<cadena truncada>";
        const char ch = *reinterpret_cast<const char*>(uintptr_t(p) + i);
        if (!ch) return s;
        s += ch;
    }
    return "<sin terminador>";
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
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kLInsert: case kSdd: obs["ret"] = Tag(uintptr_t(eax)); break;      // puntero como etiqueta de arena
        case kBone: obs["ret"] = StringOrNull(eax); break;                      // el TEXTO de la cadena (la DLL y el EXE devuelven punteros distintos al mismo texto)
        case kEnter: obs["ret"] = eax & 0xFFu; break;                           // solo AL
        case kKey: case kCross: obs["ret"] = eax; break;                        // EAX completo
        default: obs["ret"] = nullptr; break;                                   // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN183A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN183A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn183a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn183a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn183a"); }
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
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (operator delete del CRT 0x8214BD): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador REAL con hook (apagado): el EXE corre sus bytes y la DLL su funcion local equivalente (direccion dada)
    auto R = [&](uintptr_t exe, uintptr_t dllAddress) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked) ready = false;
        Pair p = it->second;
        p.dll = Body(dllAddress); p.hooked = true;
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return p;
    };
    env.stubs[0] = {ByExeAddress(0x8214BD, &FkDeleteLink<0x8214BD>)};
    for (unsigned f = 6; f <= 13; ++f) env.stubs[f] = {F(0x61A6F0, &FkBody<0x61A6F0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[14] = {F(0x62F360, &FkBody<0x62F360>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[15] = {F(0x62F5D0, &FkBody<0x62F5D0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[16] = {F(0x61A3A0, &FkBody<0x61A3A0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[17] = {F(0x61A3A0, &FkBody<0x61A3A0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[18] = {F(0x62FB40, &FkBody<0x62FB40>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.reals[16] = {R(0x62F650, Lote183ABody_CTaskSimpleSetKindaStayInSamePlace())};
    env.reals[17] = {R(0x62F6D0, Lote183ABody_CTaskSimpleDuckToggle())};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo el cuerpo del destructor (con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL) y CTask::operator delete (0x61A5B0, con hook, falso identico)";
    dllReal[0] = "toda la funcion salvo operator delete del CRT (0x8214BD: la DLL lo llama por la direccion del EXE, falso)";
    for (unsigned f : {1u, 2u, 3u, 4u, 5u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[16] = "toda la funcion salvo CTask::Destructor (0x61A3A0, con hook, falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico); el cuerpo 0x62F650 corre REAL (EXE: sus bytes con el hook apagado; DLL: su funcion local)";
    dllReal[17] = "toda la funcion salvo CTask::Destructor (0x61A3A0, con hook, falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico); el cuerpo 0x62F6D0 corre REAL (EXE: sus bytes con el hook apagado; DLL: su funcion local)";
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
    // Memoria global del juego tocada: palabras de Enter / extenter (nuevo y viejo, cada una con su vecina) y los dos arrays de 256 palabras de teclas estandar; se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kNewEnter, 4); regions.emplace_back(kOldEnter, 4); regions.emplace_back(kNewExt, 4); regions.emplace_back(kOldExt, 4);
    regionsOf[3] = {regions.size() - 4, regions.size() - 3, regions.size() - 2, regions.size() - 1};
    regions.emplace_back(kNewKeys, kKeysBytes); regions.emplace_back(kOldKeys, kKeysBytes); regionsOf[4] = {regions.size() - 2, regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}

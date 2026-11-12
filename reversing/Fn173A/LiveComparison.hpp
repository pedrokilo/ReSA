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

// Banco par a par Fn173A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 21 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedGlobalInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTOverloadedInstall). El banco invoca la direccion
// del EXE con objetos sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): CBaseModelInfo
// constructor 0x4C4A60, AddRef 0x4C4BA0 y RemoveRef 0x4C4BB0, CAtomicModelInfo::CreateInstance(matriz) 0x4C44D0 (V), RpAtomicSetFrame 0x74BF20, CPtrNodeSingleLinkPool::New 0x552240 y CPtrNodeDoubleLinkPool::New 0x5522E0
// (con hook), y solo en la direccion del EXE (la DLL los llama por esa direccion): delete[] del CRT 0x8213AE, RpAtomicClone 0x749E60 y RwFrameCreate 0x7F0410. CAtomicModelInfo::CreateInstance() 0x4C4530 (V) NO se
// falsea (se comprueba que corra su cuerpo: la DLL lo llama en lugar de las instrucciones en linea del EXE). Memoria global del juego tocada (restaurada tras cada lado): selector 0xA9B0B0, punteros de los pools
// 0xB74484 / 0xB74488, desfase del agua 0xC228A4 y reloj 0xB7CB84.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria global, los eventos de los falsos (con sus
// argumentos), el valor devuelto (punteros como etiqueta de arena; sin valor en las funciones void) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F173ALive {
using namespace F173AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F173AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
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
inline u32 gRet{};                   // EAX que devuelven los falsos
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this]
inline u32 gAtom{}, gFrame{};        // lo que devuelven los falsos de RpAtomicClone y RwFrameCreate
inline const u8* gLinksField{};      // CLinkList::Shutdown: direccion de [this + campo] (el falso de delete[] lee su valor al entrar)
inline DWORD gBenchThread{};

// Cuerpo / constructor base sin argumentos de pila (thiscall): evento [direccion del EXE, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Funcion thiscall sin argumentos que no toca memoria (AddRef, RemoveRef, New de los pools): evento [direccion, ECX (etiqueta)]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkThis(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(self);
    return gRet;
}
// operator delete[] del CRT (0x8213AE; cdecl, un argumento CRUDO que puede no apuntar a memoria valida): evento [direccion, argumento (etiqueta o crudo), dword de [this + campo] de la lista al entrar (aun el puntero viejo)].
inline void __cdecl FkDeleteArray(void* p) {
    Ev& e = PushEv(0x8213AE, 2);
    e.v[0] = Tag(p); e.v[1] = gLinksField ? BitsOf(gLinksField) : 0u;
}
// RpAtomicClone (0x749E60; cdecl, un puntero crudo): evento [direccion, argumento (etiqueta)]; devuelve gAtom.
inline u32 __cdecl FkAtomClone(u32 atomic) {
    Ev& e = PushEv(0x749E60, 1);
    e.v[0] = Tag(uintptr_t(atomic));
    return gAtom;
}
// RwFrameCreate (0x7F0410; cdecl, sin argumentos): evento [direccion]; devuelve gFrame.
inline u32 __cdecl FkFrameCreate() {
    PushEv(0x7F0410, 0);
    return gFrame;
}
// RpAtomicSetFrame (0x74BF20; cdecl, atomic y frame): evento [direccion, atomic (etiqueta), frame (etiqueta)]; devuelve 0 (los llamadores no lo leen).
inline u32 __cdecl FkSetFrame(u32 atomic, u32 frame) {
    Ev& e = PushEv(0x74BF20, 2);
    e.v[0] = Tag(uintptr_t(atomic)); e.v[1] = Tag(uintptr_t(frame));
    return 0;
}
// CAtomicModelInfo::CreateInstance(RwMatrix*) (0x4C44D0; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), matriz (etiqueta)]; devuelve gRet.
inline u32 __fastcall FkCreateMat(void* self, void*, void* matrix) {
    Ev& e = PushEv(0x4C44D0, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(matrix);
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
    bool floatRet{};
};

// Enlaces de una lista CLinkList<AlphaObjectInfo> (enlaces de 0x14 bytes: dato 0..0xB, anterior +0xC, siguiente +0x10): cabeza usada +0, cola usada +0x14, cabeza libre +0x28, cola libre +0x3C
inline void LinkChain(u32 headAddr, const std::vector<u32>& mids, u32 tailAddr) {
    std::vector<u32> chain{headAddr};
    chain.insert(chain.end(), mids.begin(), mids.end());
    chain.push_back(tailAddr);
    for (size_t i = 0; i < chain.size(); ++i) {
        if (i > 0) memcpy(reinterpret_cast<void*>(chain[i] + 0xC), &chain[i - 1], 4);
        if (i + 1 < chain.size()) memcpy(reinterpret_cast<void*>(chain[i] + 0x10), &chain[i + 1], 4);
    }
}

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    gRet = 0; gDtorWrite = 0; gAtom = 0; gFrame = 0; gLinksField = nullptr;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kFill:
        for (u32 i = 0; i < 30; ++i) arenaA.Put32(o + 4 + 4 * i, a[4 + i]);
        break;
    case kPlClear:
    case kDtor7:
        break;
    case kNop:
        *reinterpret_cast<u8*>(kDamageFlag) = 0x5A;   // marca que la prueba negativa (mutate.py) lee para que las funciones vacias tengan algo observable; el juego real nunca la pone
        break;
    case kShutdown: {
        const u32 field = c.fn == 2 ? 0x50u : 0x30u;
        const u32 value = a[6] == 0 ? 0u : (a[6] == 1 ? u32(arenaB.Obj(a[5])) : a[4]);
        arenaA.Put32(o + field, value);
        gLinksField = reinterpret_cast<const u8*>(arenaA.Obj(o + field));
        break;
    }
    case kInsertSorted:
    case kLlClear: {
        const u32 nUsed = a[4], nFree = a[5];
        std::vector<u32> used, freed;
        for (u32 i = 0; i < nUsed + nFree; ++i) {
            const u32 slot = a[8 + i];
            const u32 node = u32(arenaB.Obj(a[6] + 0x20 * slot));
            arenaB.Put32(a[6] + 0x20 * slot + 8, a[16 + slot]);
            (i < nUsed ? used : freed).push_back(node);
        }
        LinkChain(thisA + 0, used, thisA + 0x14);
        LinkChain(thisA + 0x28, freed, thisA + 0x3C);
        if (kKind[c.fn] == kInsertSorted) {
            arenaC.Put32(a[27], a[24]); arenaC.Put32(a[27] + 4, a[25]); arenaC.Put32(a[27] + 8, a[26]);
            call.args[0] = u32(arenaC.Obj(a[27])); call.argCount = 1;
        }
        break;
    }
    case kDmg: {
        arenaA.Put32(o + 0x1C, a[4]); arenaA.Put32(o + 0x20, a[5]);
        *reinterpret_cast<u8*>(kDamageFlag) = u8(a[6]);
        gAtom = a[8];
        const u32 mode = c.fn == 9 ? 0u : a[9];
        gFrame = mode == 2 ? a[11] : u32(arenaD.Obj(a[10]));
        gRet = a[12];
        if (c.fn == 9) { call.args[0] = u32(arenaC.Obj(a[7])); call.argCount = 1; }
        break;
    }
    case kLodCtor:
        gRet = a[4]; gDtorWrite = a[5];
        break;
    case kSync:
        *reinterpret_cast<u32*>(kClock) = a[4]; *reinterpret_cast<u32*>(kWaterOffset) = a[5];
        break;
    case kPoolNew:
        *reinterpret_cast<u32*>(c.fn == 14 ? kPoolSingle : kPoolDouble) = u32(arenaB.Obj(a[4]));
        arenaB.Put32(a[4] + 8, 5);   // capacidad pequena del pool sintetico (los reales tienen miles): la usa la prueba negativa (mutate.py) para no romper la partida
        gRet = a[5];
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 4;
        break;
    case kPoolDel: {
        const u32 sz = PoolNodeSize(c.fn);
        arenaB.Put32(a[4] + 0, u32(arenaC.Obj(a[7]))); arenaB.Put32(a[4] + 4, u32(arenaD.Obj(a[8])));
        arenaB.Put32(a[4] + 8, a[5]); arenaB.Put32(a[4] + 0xC, a[9]);
        uint8* state = arenaD.m.data() + kGuard + a[8] + a[6];
        *state = u8((*state & 0x7Fu) | (a[10] ? 0x80u : 0u));
        *reinterpret_cast<u32*>(c.fn == 15 ? kPoolSingle : kPoolDouble) = u32(arenaB.Obj(a[4]));
        call.args[0] = u32(arenaC.Obj(a[7] + a[6] * sz)); call.argCount = 1; call.popBytes = 4;
        break;
    }
    case kCompress:
        for (u32 i = 0; i < 4; ++i) arenaB.Put32(a[8] + 4 * i, a[4 + i]);
        call.args[0] = thisA; call.args[1] = u32(arenaB.Obj(a[8])); call.argCount = 2; call.popBytes = 8;
        break;
    case kDecompress4:
    case kDecompress3:
        for (u32 i = 0; i < (kKind[c.fn] == kDecompress4 ? 4u : 3u); ++i) arenaB.Put16(a[8] + 2 * i, a[4 + i]);
        call.args[0] = thisA; call.args[1] = u32(arenaB.Obj(a[8])); call.argCount = 2; call.popBytes = 8;
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
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kInsertSorted: case kDmg: case kLodCtor: case kPoolNew: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        default: obs["ret"] = nullptr; break;                                                                      // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN173A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN173A_CASES", buffer, sizeof(buffer)) > 0) {
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
// Colaborador con hook V (RH_ScopedVMTInstall): la ruta usa el cuerpo de la DLL de la ranura (la funcion que la DLL llama de forma no virtual) y el hook se apaga.
inline Pair MakeCollabV(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name; uintptr_t vbody = 0;
    if (FindHook(exe, cat, name, &vbody)) {
        if (vbody) env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, vbody));
        else env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}, {"virtual", vbody != 0}});
        if (env.collab.back()->Registered()) { p.dll = Body(env.collab.back()->Body()); p.hooked = true; }
    }
    return p;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn173a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn173a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn173a"); }
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
    // Colaborador con hook V (la DLL lo llama de forma no virtual: el cuerpo es la funcion de la ranura)
    auto FV = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollabV(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (delete[] 0x8213AE, RpAtomicClone 0x749E60, RwFrameCreate 0x7F0410): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    env.stubs[2] = {ByExeAddress(0x8213AE, &FkDeleteArray)};
    env.stubs[3] = {ByExeAddress(0x8213AE, &FkDeleteArray)};
    env.stubs[9] = {ByExeAddress(0x749E60, &FkAtomClone), ByExeAddress(0x7F0410, &FkFrameCreate), F(0x74BF20, &FkSetFrame), FV(0x4C44D0, &FkCreateMat)};
    env.stubs[10] = {ByExeAddress(0x749E60, &FkAtomClone), ByExeAddress(0x7F0410, &FkFrameCreate), F(0x74BF20, &FkSetFrame), F(0x4C4BA0, &FkThis<0x4C4BA0>), F(0x4C4BB0, &FkThis<0x4C4BB0>)};
    {   // CAtomicModelInfo::CreateInstance() (0x4C4530, V): colaborador REAL con el hook apagado (el EXE corre sus bytes y la DLL su cuerpo)
        auto it = collabCache.find(0x4C4530);
        if (it == collabCache.end()) it = collabCache.emplace(0x4C4530, MakeCollabV(env, 0x4C4530, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        env.reals[10] = {it->second};
    }
    env.stubs[12] = {F(0x4C4A60, &FkBody<0x4C4A60>)};
    env.stubs[14] = {F(0x552240, &FkThis<0x552240>)};
    env.stubs[16] = {F(0x5522E0, &FkThis<0x5522E0>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {0u, 1u, 4u, 5u, 6u, 7u, 8u, 11u, 13u, 15u, 17u, 18u, 19u, 20u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[10] = "toda la funcion salvo los falsos; CAtomicModelInfo::CreateInstance() (0x4C4530) corre real (hook apagado)";
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
    // Memoria global del juego tocada: selector 0xA9B0B0 (CreateInstance), punteros de los pools (operator new/delete), desfase del agua y reloj (SyncWater); se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kDamageFlag, 1); regionsOf[6] = regionsOf[7] = regionsOf[8] = regionsOf[9] = regionsOf[10] = {regions.size() - 1};
    regions.emplace_back(kPoolSingle, 8); regionsOf[14] = regionsOf[15] = regionsOf[16] = regionsOf[17] = {regions.size() - 1};
    regions.emplace_back(kWaterOffset, 4); regions.emplace_back(kClock, 4); regionsOf[13] = {regions.size() - 2, regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}

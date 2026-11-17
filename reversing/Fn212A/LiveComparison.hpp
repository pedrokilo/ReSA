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

// Banco par a par Fn212A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 32 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall). El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40).
// Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair) para los de RenderWare (todos con hook: 0x7F9FF0, 0x7F9FB0, 0x7FA320, 0x7FA360, 0x7FDE70, 0x7FC340, 0x7FC2D0,
// 0x7FC430, 0x7FC4D0), y solo en la direccion del EXE para los que la DLL llama por ella (operator delete 0x8214BD, delete[] 0x8213AE y operator new[] 0x821195).
// Se observan: los bytes cambiados de las cuatro arenas, los cambios de la memoria global (material D3D9 0x8D12D8 y AmbientSaturated 0x8E2418), los eventos de los falsos (con sus argumentos), el valor devuelto (punteros como etiqueta de
// arena) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F212ALive {
using namespace F212AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F212AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = u8(v); }
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
inline DWORD gBenchThread{};
inline const u8* gPool{};                       // constructores de CPool: objeto que se construye (el falso de new[] lee su +0 y +4 al entrar)
inline u32 gNewRet[2]{};                        // lo que devuelven las dos llamadas a operator new[]
inline unsigned gNewCall{};
inline u8* gHdr{};                              // dibujo: cabecera y malla (los falsos de los sombreadores reescriben campos para comprobar el orden de lectura)
inline u8* gMesh{};
inline const u32* gMut{};                       // a[13..21] del caso (modo y valores)
inline void Put32At(u8* p, u32 v) { memcpy(p, &v, 4); }

// operator delete (0x8214BD) y delete[] (0x8213AE) del CRT del EXE (cdecl, un puntero CRUDO): evento [direccion, argumento (etiqueta o crudo)]
inline void __cdecl FkDelete(void* p) { Ev& e = PushEv(0x8214BD, 1); e.v[0] = Tag(p); }
inline void __cdecl FkDeleteArray(void* p) { Ev& e = PushEv(0x8213AE, 1); e.v[0] = Tag(p); }
// operator new[] del CRT del EXE (0x821195; cdecl, tamano): evento [direccion, tamano, dword [pool+0], dword [pool+4]] (orden de las escrituras del constructor); devuelve el bloque que toca
inline u32 __cdecl FkNewArray(u32 size) {
    Ev& e = PushEv(0x821195, 3);
    e.v[0] = size; e.v[1] = gPool ? BitsOf(gPool) : 0u; e.v[2] = gPool ? BitsOf(gPool + 4) : 0u;
    return gNewRet[gNewCall < 2 ? gNewCall++ : 1];
}
// RenderWare (cdecl; todos con hook en la DLL, falso identico en el EXE y en el cuerpo de la DLL)
inline u32 __cdecl FkPixelShader(void* s) {   // 0x7F9FF0
    Ev& e = PushEv(0x7F9FF0, 1); e.v[0] = Tag(s);
    if (gMut && (gMut[0] & 1)) { Put32At(gHdr + 8, gMut[1]); Put32At(gHdr + 0xC, gMut[2]); Put32At(gMesh + 0x10, gMut[3]); Put32At(gMesh + 0x14, gMut[4]); }
    return 1;
}
inline u32 __cdecl FkVertexShader(void* s) {  // 0x7F9FB0
    Ev& e = PushEv(0x7F9FB0, 1); e.v[0] = Tag(s);
    if (gMut && (gMut[0] & 2)) { Put32At(gMesh + 0x18, gMut[5]); Put32At(gMesh + 0x1C, gMut[6]); Put32At(gMesh + 0x20, gMut[7]); Put32At(gHdr + 8, gMut[8]); }
    return 1;
}
inline u32 __cdecl FkDrawIndexed(u32 prim, u32 base, u32 minIdx, u32 numVerts, u32 start, u32 numPrims) {   // 0x7FA320
    Ev& e = PushEv(0x7FA320, 6); e.v[0] = prim; e.v[1] = base; e.v[2] = minIdx; e.v[3] = numVerts; e.v[4] = start; e.v[5] = numPrims; return 1;
}
inline u32 __cdecl FkDrawPrim(u32 prim, u32 start, u32 num) {   // 0x7FA360
    Ev& e = PushEv(0x7FA360, 3); e.v[0] = prim; e.v[1] = start; e.v[2] = num; return 1;
}
inline u32 __cdecl FkSetTexture(void* tex, u32 stage) {   // 0x7FDE70
    Ev& e = PushEv(0x7FDE70, 2); e.v[0] = Tag(tex); e.v[1] = stage; return 1;
}
inline u32 __cdecl FkStageState(u32 stage, u32 type, u32 value) {   // 0x7FC340
    Ev& e = PushEv(0x7FC340, 3); e.v[0] = stage; e.v[1] = type; e.v[2] = value; return 1;
}
inline u32 __cdecl FkRenderState(u32 state, u32 value) {   // 0x7FC2D0
    Ev& e = PushEv(0x7FC2D0, 2); e.v[0] = state; e.v[1] = value; return 1;
}
inline u32 __cdecl FkSetMaterial(const u32* m) {   // 0x7FC430: los 17 dwords del material en el momento de la llamada
    Ev& e = PushEv(0x7FC430, 17); for (u32 i = 0; i < 17; ++i) e.v[i] = m[i]; return 1;
}
inline u32 __cdecl FkSetSurface(void* props, void* color, u32 flags) {   // 0x7FC4D0
    Ev& e = PushEv(0x7FC4D0, 3); e.v[0] = u32(uintptr_t(props)); e.v[1] = u32(uintptr_t(color)); e.v[2] = flags; return 1;
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
inline void Put32G(u32 addr, u32 v) { memcpy(reinterpret_cast<void*>(addr), &v, 4); }
inline std::array<u32, 9> gMutVals{};

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    const Kind kind = kKind[c.fn];
    arenaA.Pattern(a[2], a[92]); arenaB.Pattern(a[2], a[93]); arenaC.Pattern(a[2], a[94]); arenaD.Pattern(a[2], a[95]);
    gPool = nullptr; gNewCall = 0; gNewRet[0] = gNewRet[1] = 0; gHdr = gMesh = nullptr; gMut = nullptr;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kind) {
    case kScalarDel: case kDtorBody: {
        const u32 p0 = a[4] == 0 ? a[5] : (a[4] == 1 ? u32(arenaB.Obj(a[9])) : 0u);
        const u32 p1 = a[6] == 0 ? a[7] : (a[6] == 1 ? u32(arenaC.Obj(a[11])) : 0u);
        arenaA.Put32(o + 0, p0); arenaA.Put32(o + 4, p1); arenaA.Put32(o + 8, a[8]); arenaA.Put32(o + 0xC, a[13]); arenaA.Put8(o + 0x10, a[12]);
        if (kind == kScalarDel) { call.args[0] = a[10]; call.argCount = 1; }   // `ret 4`
        call.popBytes = 0;
        break;
    }
    case kPoolCtor: {
        const i32 cap = i32(a[4]);
        gPool = reinterpret_cast<const u8*>(arenaA.Obj(o));
        gNewRet[0] = cap > 0 ? u32(arenaB.Obj(a[5])) : a[8];
        gNewRet[1] = cap > 0 ? u32(arenaC.Obj(a[6])) : a[9];
        call.args[0] = a[4]; call.args[1] = a[7]; call.argCount = 2; call.popBytes = 0;   // `ret 8`
        break;
    }
    case kRender: case kRenderLit: {
        const u32 meshA = u32(arenaB.Obj(a[5]));
        arenaA.Put32(o + 8, a[6]); arenaA.Put32(o + 0xC, a[7]);
        arenaB.Put32(a[5] + 0x10, a[8]);
        for (u32 i = 0; i < 4; ++i) arenaB.Put32(a[5] + 0x14 + 4 * i, a[9 + i]);
        gHdr = reinterpret_cast<u8*>(arenaA.Obj(o)); gMesh = reinterpret_cast<u8*>(arenaB.Obj(a[5]));
        gMutVals = {a[13], a[14], a[15], a[16], a[17], a[18], a[19], a[20], a[21]};
        gMut = gMutVals.data();
        call.args[0] = thisA; call.args[1] = meshA; call.argCount = 2; call.popBytes = 8;
        if (kind == kRenderLit) {
            call.args[2] = a[22]; call.args[3] = a[24] == 1 ? u32(arenaD.Obj(a[25])) : a[23]; call.argCount = 4; call.popBytes = 16;
        }
        break;
    }
    case kEnvMat: {
        arenaB.Put32(a[5] + 0, a[6]); arenaB.Put32(a[5] + 4, a[7]); arenaB.Put32(a[5] + 8, a[8]);
        for (u32 i = 0; i < 17; ++i) Put32G(kMaterial + 4 * i, a[16 + i]);
        for (u32 i = 0; i < 4; ++i) Put32G(kAmbientSat + 4 * i, a[12 + i]);
        call.args[0] = u32(arenaB.Obj(a[5])); call.args[1] = thisA; call.args[2] = a[9]; call.args[3] = a[10]; call.args[4] = a[11]; call.argCount = 5; call.popBytes = 20;
        break;
    }
    case kDefSurf:
        *reinterpret_cast<u8*>(kMarker) = 0x5A;   // marca que la prueba negativa (mutate.py) lee; el juego real nunca la pone
        call.popBytes = 0;
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
        case kScalarDel: case kPoolCtor:
            obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case kEnvMat: obs["ret"] = eax; break;         // RwBool
        default: obs["ret"] = nullptr; break;          // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}



// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN212A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN212A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn212a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn212a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn212a"); }
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
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0 && ModuleOf(env.route[f]->Body()) == env.asi;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}, {"virtual", vbody != 0}}) : json(nullptr);
        collaborators[kFn[f]] = {{"found", found}, {"body", found ? env.route[f]->Body() : 0}, {"body_module", found ? ModuleOf(env.route[f]->Body()) : 0}, {"asi", env.asi}, {"cat", cat}, {"name", name}};
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
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (operator delete 0x8214BD, delete[] 0x8213AE, operator new[] 0x821195): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    const std::vector<StubSpec> drawStubs = [&] { return std::vector<StubSpec>{F(0x7F9FF0, &FkPixelShader), F(0x7F9FB0, &FkVertexShader), F(0x7FA320, &FkDrawIndexed), F(0x7FA360, &FkDrawPrim)}; }();
    env.stubs[0] = {ByExeAddress(0x8214BD, &FkDelete)};
    env.stubs[1] = {ByExeAddress(0x8214BD, &FkDelete)};
    env.stubs[2] = drawStubs;
    env.stubs[3] = {F(0x7FDE70, &FkSetTexture), F(0x7FC340, &FkStageState)};
    env.stubs[3].insert(env.stubs[3].end(), drawStubs.begin(), drawStubs.end());
    env.stubs[4] = {F(0x7FC2D0, &FkRenderState), F(0x7FC430, &FkSetMaterial)};
    env.stubs[5] = {F(0x7FC4D0, &FkSetSurface)};
    for (unsigned f = 6; f < 19; ++f) env.stubs[f] = {ByExeAddress(0x821195, &FkNewArray)};
    for (unsigned f = 19; f < 32; ++f) env.stubs[f] = {ByExeAddress(0x8213AE, &FkDeleteArray)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[0] = dllReal[1] = "toda la funcion salvo operator delete (0x8214BD, falso solo en la direccion del EXE: la DLL lo llama por ella)";
    dllReal[2] = "toda la funcion salvo las cuatro llamadas de RenderWare (0x7F9FF0, 0x7F9FB0, 0x7FA320, 0x7FA360; con hook, falsos identicos en el EXE y en el cuerpo de la DLL)";
    dllReal[3] = "toda la funcion salvo las seis llamadas de RenderWare (0x7FDE70, 0x7FC340, 0x7F9FF0, 0x7F9FB0, 0x7FA320, 0x7FA360; con hook, falsos identicos)";
    dllReal[4] = "toda la funcion y los helpers x87 de X87Math salvo RwD3D9SetRenderState (0x7FC2D0) y RwD3D9SetMaterial (0x7FC430), con hook (falsos identicos)";
    dllReal[5] = "toda la funcion salvo RwD3D9SetSurfaceProperties (0x7FC4D0), con hook (falso identico)";
    for (unsigned f = 6; f < 19; ++f) dllReal[f] = "toda la funcion salvo operator new[] (0x821195, falso solo en la direccion del EXE: la DLL lo llama por ella)";
    for (unsigned f = 19; f < 32; ++f) dllReal[f] = "toda la funcion salvo delete[] (0x8213AE, falso solo en la direccion del EXE: la DLL lo llama por ella)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada (se guarda al empezar y se restaura tras cada lado)
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    auto R = [&](uintptr_t at, size_t n) { regions.emplace_back(at, n); return regions.size() - 1; };
    regionsOf[4] = {R(kMaterial, kMaterialBytes), R(kAmbientSat, kAmbientSatBytes)};
    regionsOf[5] = {R(kMarker, 1)};
    Run(env, ready, registered, bodies, collaborators);
}
}

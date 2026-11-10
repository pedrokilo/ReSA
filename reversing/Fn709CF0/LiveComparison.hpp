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
#include "Shadows.h"
#include "RenderBuffer.hpp"
#include "../ProbeRuntime.hpp"
#include "Core.hpp"

// Banco par a par Fn709CF0 dentro del juego (caso CASO-CONGELADOS, parte B): Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// CShadows::ShadowRenderTriangleCB (0x709CF0): __usercall del EXE (EAX = normal, EBX = triangulo, EDI = parametros, EAX = EBX al volver, `ret`) contra la version C++ de Shadows.cpp (cdecl, devuelve el
// triangulo). El EXE se invoca por su direccion con GateUser (envoltorio __asm del banco, en reversing/, fuera de source/game_sa); la version C++ se invoca directamente. No hay hook RH: nadie la llama por
// direccion (su unico llamador del EXE, 0x70A437, esta reemplazado). 16384 pares.
// Colaboradores sustituidos por falsos identicos en el EXE y en el cuerpo de la DLL: RwV3dTransformPoints 0x7EDD90 (la DLL lo llama por la direccion del EXE; el falso escribe las dos tablas de 9 floats
// del caso: mundo y proyectado), RenderBuffer::StartStoring 0x708290 (devuelve los buffers de la arena D) y StopStoring 0x7077A0 (solo registra el evento).
// Se observan los bytes cambiados de las cuatro arenas (A, B, C: entradas; D: buffers de indices y vertices), los eventos de los falsos con todos sus argumentos, el valor devuelto y el estado x87.

namespace F709Live {
using namespace F709Fix;
inline constexpr uintptr_t kEntry = 0x709CF0, kXform = 0x7EDD90, kStart = 0x708290, kStop = 0x7077A0;

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F709Fix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(int64 off = 0) const { return uintptr_t(int64(uintptr_t(m.data())) + int64(kGuard) + off); }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put(int64 off, const void* src, size_t n) { memcpy(reinterpret_cast<void*>(Obj(off)), src, n); }
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;

// ================================================================================================ falsos de los colaboradores
inline std::array<u32, 9> gWorld{}, gProj{};
inline unsigned gXformCalls{};
inline int32 gBufIndex{}, gBufVertex{};
inline uintptr_t gPrevOut{};
inline bool OnStack(const void* p) {
    const auto* tib = reinterpret_cast<const NT_TIB*>(NtCurrentTeb());
    const uintptr_t v = uintptr_t(p);
    return v >= uintptr_t(tib->StackLimit) && v < uintptr_t(tib->StackBase);
}
inline RwV3d* __cdecl FakeXform(RwV3d* out, const RwV3d* in, RwInt32 n, const RwMatrix* matrix) {
    const unsigned call = gXformCalls++;
    Ev& e = PushEv(0x10u + (call < 4 ? call : 4u), 14);
    e.v[0] = u32(n); e.v[1] = OnStack(in) ? 0xFFFFFFFFu : Tag(uintptr_t(in)); e.v[2] = Tag(uintptr_t(matrix)); e.v[3] = uintptr_t(in) == gPrevOut ? 1u : 0u; e.v[4] = OnStack(out) ? 1u : 0u;
    for (unsigned i = 0; i < 9; ++i) e.v[5 + i] = BitsOf(reinterpret_cast<const float*>(in) + i);
    const auto& src = call == 0 ? gWorld : gProj;
    for (RwInt32 k = 0; k < n * 3; ++k) { const u32 v = (call < 2 && k < 9) ? src[k] : 0u; memcpy(reinterpret_cast<float*>(out) + k, &v, 4); }
    gPrevOut = uintptr_t(out);
    return out;
}
inline void __cdecl FakeStart(int32 nIndices, int32 nVertices, RwImVertexIndex*& outIndex, RwIm3DVertex*& outVertex) {
    Ev& e = PushEv(0x20u, 4);
    e.v[0] = u32(nIndices); e.v[1] = u32(nVertices); e.v[2] = OnStack(&outIndex) ? 1u : 0u; e.v[3] = OnStack(&outVertex) ? 1u : 0u;
    outIndex = reinterpret_cast<RwImVertexIndex*>(arenaD.Obj(gBufIndex));
    outVertex = reinterpret_cast<RwIm3DVertex*>(arenaD.Obj(gBufVertex));
}
inline void __cdecl FakeStop() { PushEv(0x30u, 0); }

inline DWORD gBenchThread{};

// ================================================================================================ cierre de un lado
inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = 0;
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    out["globals_restored"] = env.WatchesIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

struct Placed { uintptr_t normal{}, tri{}, params{}; };

// Prepara las arenas del caso y devuelve las direcciones de la normal, el triangulo y los parametros
inline Placed Setup(const C& c) {
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]); arenaD.Pattern(a[2], a[15]);
    gBufIndex = int32(a[9]); gBufVertex = int32(a[10]);
    const u32 kind = a[1];
    auto& pa = arenaA; auto& na = kind == 0 || kind == 3 ? arenaB : arenaA; auto& ta = kind == 0 || kind == 3 ? arenaC : arenaA;
    std::array<u8, kParamBytes> params{};
    FillWords(params.data(), params.size(), c.seed ^ 0x5151u);       // matrices y relleno aleatorios
    memcpy(params.data() + 0x00, c.at.data(), 12);
    memcpy(params.data() + 0x4C, &c.shadowDword, 4);
    memcpy(params.data() + 0x50, &c.fadeDword, 4);
    memcpy(params.data() + 0x54, &c.batchDword, 4);
    pa.Put(int32(a[0]), params.data(), params.size());
    ta.Put(int32(a[4]), c.tri.data(), 36);
    na.Put(int32(a[3]), c.normal.data(), 12);
    Placed p; p.params = pa.Obj(int32(a[0])); p.tri = ta.Obj(int32(a[4])); p.normal = na.Obj(int32(a[3]));
    return p;
}

inline json InputJson(const C& c, const Placed& p) {
    json input = json::parse(JsonC(c));
    const auto* norm = reinterpret_cast<const float*>(p.normal);
    const auto* tri = reinterpret_cast<const float*>(p.tri);
    const auto* prm = reinterpret_cast<const uint8*>(p.params);
    json eff;
    eff["normal"] = json::array({BitsOf(norm), BitsOf(norm + 1), BitsOf(norm + 2)});
    json t = json::array(); for (unsigned i = 0; i < 9; ++i) t.push_back(BitsOf(tri + i)); eff["tri"] = t;
    eff["at"] = json::array({BitsOf(prm), BitsOf(prm + 4), BitsOf(prm + 8)});
    eff["shadow_dword"] = BitsOf(prm + 0x4C); eff["fade_dword"] = BitsOf(prm + 0x50);
    eff["tags"] = json::array({Tag(p.normal), Tag(p.tri), Tag(p.params)});
    input["effective"] = eff;
    return input;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input;
    Side sides[2]; json rows[2];
    curFn = fn;
    const uintptr_t dllEntry = uintptr_t(static_cast<CVector* (*)(CVector*, CVector*, _ProjectionParam*)>(&ShadowRenderTriangleCB));
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        const Placed pl = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap(); arenaD.Snap();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        if (s == 0) input = InputJson(c, pl);
        gWorld = c.world; gProj = c.proj; gXformCalls = 0; gPrevOut = 0;
        nev = 0;
        const std::array<u32, 3> args{u32(pl.normal), u32(pl.tri), u32(pl.params)};
        sides[s] = Execute(env, candidate ? dllEntry : kEntry, candidate, [&](uintptr_t entry) {
            if (!candidate) return GateUser(entry, args[0], args[1], args[2], c.ecx, c.edx, c.stack);
            return Gate(entry, args.data(), 3, c.eax, c.ecx, c.edx, c.stack, 12);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = json::array();
        obs["events"] = EvsJson();
        obs["ret"] = Tag(uintptr_t(sides[s].reg.eax));
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN709CF0_CASES", buffer, sizeof(buffer)) > 0) {
        const unsigned v = unsigned(std::atoi(buffer));
        if (v >= kFamilies && v <= kCases && v % kFamilies == 0) return v;
    }
    return kCases;
}
inline void Run(Env& env, bool ready, const json& stubbed, const json& bodies) {
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    const unsigned nCases = CasesPerFunction();
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = nCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", {{kFn[0], false}}}, {"functions", kFunctions}, {"pairs", kFunctions * nCases}, {"cases", cases}, {"no_rh_hook", true},
        {"direct_functions", json::array({kFn[0]})}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"stubbed", stubbed}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        gBenchThread = GetCurrentThreadId();
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < nCases; ++c) CaseRun(env, totals, f, c);
            Write({{"event", "function_summary"}, {"function", kFn[f]}, {"cases", nCases}, {"failures", totals.failures - before}, {"state_restored", env.WatchesIntact()}});
        }
    }
    const bool entriesIntact = env.WatchesIntact();
    bool stateRestored = entriesIntact;
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn709cf0-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn709cf0-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn709cf0"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json stubbed = json::array(); json bodies = json::object();
    // Colaboradores: el falso va en la entrada del EXE y en la funcion de la DLL (entrada y cuerpo, por si hay un salto de enlace incremental); RwV3dTransformPoints la DLL lo llama por la direccion del EXE.
    const auto add = [&](uintptr_t exeAddress, void* fake, uintptr_t dllAddress, bool hooked) {
        env.stubs.push_back({{exeAddress, dllAddress, hooked}, fake});
        const uintptr_t body = dllAddress ? Body(dllAddress) : 0;
        if (dllAddress && body != dllAddress) env.stubs.push_back({{0, body, hooked}, fake});
        stubbed.push_back({{"exe", exeAddress}, {"dll", dllAddress != 0}, {"dll_body", body != 0 && body != dllAddress}, {"hooked", hooked}});
        env.watches.emplace_back(exeAddress, 16);
        if (dllAddress) ready &= ModuleOf(dllAddress) == env.asi;
    };
    add(kXform, reinterpret_cast<void*>(&FakeXform), 0, false);
    add(kStart, reinterpret_cast<void*>(&FakeStart), uintptr_t(static_cast<void (*)(int32, int32, RwImVertexIndex*&, RwIm3DVertex*&)>(&RenderBuffer::StartStoring)), true);
    add(kStop, reinterpret_cast<void*>(&FakeStop), uintptr_t(static_cast<void (*)()>(&RenderBuffer::StopStoring)), true);
    env.watches.emplace_back(kEntry, 16);
    const uintptr_t dllEntry = uintptr_t(static_cast<CVector* (*)(CVector*, CVector*, _ProjectionParam*)>(&ShadowRenderTriangleCB));
    bodies[kFn[0]] = ModuleOf(Resolve(dllEntry)) == env.asi;
    ready &= ModuleOf(Resolve(dllEntry)) == env.asi && ModuleOf(kEntry) == env.game && !ReversingProbe::JumpsToDll(kEntry);
    regions.clear();
    Run(env, ready, stubbed, bodies);
}
}

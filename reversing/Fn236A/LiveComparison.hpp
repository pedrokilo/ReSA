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

// Banco par a par Fn236A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 4 funciones de jdapimin de la libjpeg 6b de descompresion embebida en el EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Cada caso construye una jpeg_decompress_struct sintetica en la arena A
// (con err, src, inputctl y master propios) y la tabla de componentes en la B. Los colaboradores se llaman por punteros de la propia estructura (error_exit, emit_message, consume_input,
// reset_input_controller, init_source, term_source, finish_output_pass: falsos identicos en el EXE y en la DLL) o por direccion fija: jinit_memory_mgr (0x5CF6D0), jinit_marker_reader (0x5CC7D0),
// jinit_input_controller (0x5CA840) y jpeg_abort (0x5C6B50) son falsos instalados en la entrada del EXE (ambos lados los llaman por esa direccion, con su hook de la DLL apagado); jpeg_consume_input
// (0x5CFFB0) es codigo real del EXE (original de jpeg_read_header) o el cuerpo propio de la DLL. error_exit de la libjpeg real no vuelve: con P_ERRMODE = 0 el falso sale con longjmp (ver Core.hpp, Gate);
// con P_ERRMODE = 1 vuelve y el cuerpo sigue (como el EXE). Se observan: los bytes cambiados de las cuatro arenas, los eventos de los falsos, el valor devuelto y si la llamada salio por error_exit.

namespace F236ALive {
using namespace F236AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries{0x5CFD20, 0x5CFFB0, 0x5D0110, 0x5D01D0};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F236AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void RestoreByte(uintptr_t at) { const uintptr_t base = uintptr_t(m.data()); if (at >= base && at < base + N) m[at - base] = r[at - base]; }
    // Cambios respecto a la imagen de referencia, en tramos contiguos: [[desplazamiento respecto a la ventana del primer byte, bytes nuevos en hexadecimal]]
    json Changes() const {
        json out = json::array();
        constexpr char d[] = "0123456789abcdef";
        for (size_t i = 0; i < N;) {
            if (m[i] == r[i]) { ++i; continue; }
            std::string hex;
            const size_t start = i;
            while (i < N && m[i] != r[i]) { hex += d[m[i] >> 4]; hex += d[m[i] & 15]; ++i; }
            out.push_back(json::array({int64(start) - int64(kGuard), hex}));
        }
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;

inline u32 Rd32(uintptr_t a) { u32 v; memcpy(&v, reinterpret_cast<const void*>(a), 4); return v; }
inline void Wr32(uintptr_t a, u32 v) { memcpy(reinterpret_cast<void*>(a), &v, 4); }
inline void Wr16(uintptr_t a, u32 v) { const u16 w = u16(v); memcpy(reinterpret_cast<void*>(a), &w, 2); }
inline void Wr8(uintptr_t a, u32 v) { *reinterpret_cast<u8*>(a) = u8(v); }

// ================================================================================================ falsos (se llaman por punteros de la estructura o por la direccion del EXE: iguales en el EXE y en la DLL)
inline u32 gErrMode{};                 // 0: error_exit no vuelve (longjmp a Gate)
inline u32 gCalls{};                   // llamadas a consume_input hechas en este lado
inline std::array<u32, 8> gCr{};
inline u32 gCrDef{}, gEoiAt{}, gPostOn{}, gPostNc{}, gPostJfif{}, gPostAdobe{}, gPostXf{}, gCompBad{};
inline std::array<u32, 3> gPostId{};
inline DWORD gBenchThread{};

inline u32 CinfoHash(u32 cinfo) { return Fnv(reinterpret_cast<const void*>(cinfo), kCiSize); }
inline void __cdecl FkErrExit(void* ci) {
    const u32 cinfo = u32(uintptr_t(ci)), err = Rd32(cinfo);
    Ev& e = PushEv(1, 4);
    e.v[0] = Rd32(err + 0x14); e.v[1] = Rd32(err + 0x18); e.v[2] = Rd32(err + 0x1C); e.v[3] = Tag(uintptr_t(cinfo));
    if (gErrMode == 0 && gEscArmed) longjmp(gEscBuf, 1);
}
inline void __cdecl FkEmitMsg(void* ci, i32 level) {
    const u32 cinfo = u32(uintptr_t(ci)), err = Rd32(cinfo);
    Ev& e = PushEv(2, 11);
    e.v[0] = u32(level); e.v[1] = Rd32(err + 0x14); e.v[2] = Tag(uintptr_t(cinfo));
    for (u32 k = 0; k < 8; ++k) e.v[3 + k] = Rd32(err + 0x18 + 4 * k);
}
// inputctl->consume_input: escribe (si el caso lo pide) los datos que habria leido del flujo (num_components, marcadores JFIF/Adobe, ids de componente), registra el estado, pone eoi_reached tras la
// llamada EOI_AT y devuelve el valor del guion (0 a partir de la llamada 40, para acotar los bucles).
inline u32 __cdecl FkConsume(void* ci) {
    const u32 cinfo = u32(uintptr_t(ci)), k = gCalls++;
    if (gPostOn) {
        Wr32(cinfo + 0x24, gPostNc); Wr8(cinfo + 0x100, gPostJfif); Wr8(cinfo + 0x108, gPostAdobe); Wr8(cinfo + 0x109, gPostXf);
        if (!gCompBad) { const u32 comp = Rd32(cinfo + 0xC4); Wr32(comp + 0x00, gPostId[0]); Wr32(comp + 0x54, gPostId[1]); Wr32(comp + 0xA8, gPostId[2]); }
    }
    Ev& e = PushEv(10, 4);
    e.v[0] = k; e.v[1] = Tag(uintptr_t(cinfo)); e.v[2] = Rd32(cinfo + 0x14); e.v[3] = CinfoHash(cinfo);
    if (k == gEoiAt) Wr8(Rd32(cinfo + 0x190) + 0x11, 1);
    return k >= 40 ? 0u : (k < 8 ? gCr[k] : gCrDef);
}
template<u32 ID> inline void __cdecl FkMod1(void* ci) {   // modulos de un argumento (cinfo): registran el estado completo de la estructura
    const u32 cinfo = u32(uintptr_t(ci));
    Ev& e = PushEv(ID, 3);
    e.v[0] = Tag(uintptr_t(cinfo)); e.v[1] = Rd32(cinfo + 0x14); e.v[2] = CinfoHash(cinfo);
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
    u32 argCount{}, popBytes{}, eax{}, ecx{}, edx{};
};

// Prepara arenas y la escena del caso (jpeg_decompress_struct y estructuras auxiliares); devuelve la llamada
inline Call Setup(const C& c) {
    Call call;
    const auto& a = c.a;
    const u32 fn = c.fn;
    arenaA.Pattern(a[P_FILL], a[SEED0]); arenaB.Pattern(a[P_FILL], a[SEED1]); arenaC.Pattern(a[P_FILL], a[SEED2]); arenaD.Pattern(a[P_FILL], a[SEED3]);
    const u32 S = u32(arenaA.Obj(a[P_OFF])), B0 = u32(arenaB.Obj(a[P_BOFF]));
    gErrMode = a[P_ERRMODE]; gCalls = 0;
    for (u32 i = 0; i < 8; ++i) gCr[i] = a[CR0 + i];
    gCrDef = a[CRDEF]; gEoiAt = a[EOI_AT]; gPostOn = a[POST_ON]; gPostNc = a[POST_NC]; gPostJfif = a[POST_JFIF]; gPostAdobe = a[POST_ADOBE]; gPostXf = a[POST_XF]; gCompBad = a[COMP_BAD];
    gPostId = {a[POST_ID0], a[POST_ID1], a[POST_ID2]};
    nev = 0;

    // estructuras auxiliares
    Wr32(S + kErr + 0x00, u32(uintptr_t(&FkErrExit))); Wr32(S + kErr + 0x04, u32(uintptr_t(&FkEmitMsg)));
    Wr32(S + kSrc + 0x08, u32(uintptr_t(&FkMod1<12>))); Wr32(S + kSrc + 0x18, u32(uintptr_t(&FkMod1<13>)));
    Wr32(S + kInp + 0x00, u32(uintptr_t(&FkConsume))); Wr32(S + kInp + 0x04, u32(uintptr_t(&FkMod1<11>))); Wr8(S + kInp + 0x11, a[EOI_INIT]);
    Wr32(S + kMaster + 0x04, u32(uintptr_t(&FkMod1<14>)));
    if (!a[COMP_BAD]) { Wr32(B0 + kBComps + 0x00, a[PRE_ID0]); Wr32(B0 + kBComps + 0x54, a[PRE_ID1]); Wr32(B0 + kBComps + 0xA8, a[PRE_ID2]); }
    // jpeg_decompress_struct
    const u32 ci = S + kCi;
    Wr32(ci + 0x00, S + kErr); Wr32(ci + 0x0C, a[F_CLIENT]); Wr32(ci + 0x14, a[F_GSTATE]); Wr32(ci + 0x18, S + kSrc);
    Wr32(ci + 0x24, a[PRE_NC]); Wr8(ci + 0x40, a[F_BUFIMG]); Wr32(ci + 0x60, a[F_OUTH]); Wr32(ci + 0x78, a[F_OUTSCAN]);
    Wr32(ci + 0xC4, a[COMP_BAD] ? 0xDEAD0000u + (a[P_BOFF] & 0xFFFFu) : B0 + kBComps);
    Wr8(ci + 0x100, a[PRE_JFIF]); Wr8(ci + 0x108, a[PRE_ADOBE]); Wr8(ci + 0x109, a[PRE_XF]);
    Wr32(ci + 0x180, S + kMaster); Wr32(ci + 0x190, S + kInp);

    call.eax = c.eax; call.ecx = c.ecx; call.edx = c.edx;
    call.args[0] = S; call.argCount = 1; call.popBytes = 4;
    if (fn == 0) { call.args[1] = a[A_VER]; call.args[2] = a[A_SIZE]; call.argCount = 3; call.popBytes = 12; }
    if (fn == 3) { call.args[1] = a[A_REQ]; call.argCount = 2; call.popBytes = 8; }
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
            return Gate(entry, call.args.data(), call.argCount, call.eax, call.ecx, call.edx, c.stack, call.popBytes, false);
        });
        const bool escaped = gEscaped;
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        obs["escaped"] = escaped;
        const u32 eax = sides[s].reg.eax;
        if (escaped || fn == 0) obs["ret"] = nullptr;                        // void (o no llego a volver)
        else if (fn == 2) obs["ret"] = eax & 0xFF;                           // boolean en AL
        else obs["ret"] = eax;                                               // int en EAX
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN236A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN236A_CASES", buffer, sizeof(buffer)) > 0) {
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

// Busca el hook registrado para una direccion del EXE (categoria y nombre).
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

// Colaborador REAL con hook RH en la DLL (jdiv_round_up, de otro lote): la ruta deja su hook apagado (en ambos lados el EXE corre sus bytes y la DLL la llama por la direccion del EXE).
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
    if (!std::filesystem::exists(dir / "fn236a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn236a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn236a"); }
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
    // Falsos en la entrada del EXE (los dos lados llaman por esa direccion); su hook (lotes 231A/233A/234A y de memoria) se deja apagado. jpeg_consume_input (0x5CFFB0) es del lote: el EXE corre la
    // suya (hook apagado) y la DLL el cuerpo propio de jpeg_read_header.
    {
        auto Real = [](uintptr_t exe) { return Pair{exe, 0, false}; };
        auto Stub = [&](uintptr_t exe, void* fake) { Pair p = MakeCollab(env, exe, hooksFound); p.dll = 0; return StubSpec{p, fake}; };
        env.stubs[0] = {Stub(0x5CF6D0, reinterpret_cast<void*>(&FkMod1<20>)), Stub(0x5CC7D0, reinterpret_cast<void*>(&FkMod1<21>)), Stub(0x5CA840, reinterpret_cast<void*>(&FkMod1<22>))};
        const StubSpec abortStub = Stub(0x5C6B50, reinterpret_cast<void*>(&FkMod1<23>));   // una sola ruta para el hook de jpeg_abort (lo usan finish_decompress y read_header)
        env.stubs[2] = {abortStub};
        env.stubs[3] = {abortStub};
        env.reals[3] = {Real(0x5CFFB0)};
    }
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"hooked", s.pair.hooked}});
        }
        for (const auto& pr : env.reals[f]) {
            env.watches.emplace_back(pr.exe, 16);
            real.push_back({{"exe", pr.exe}, {"dll", pr.dll != 0}, {"hooked", pr.hooked}, {"dll_addr", pr.dll}, {"dll_module", ModuleOf(pr.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"toda la funcion; los colaboradores se llaman por punteros de la estructura (falsos iguales en ambos lados) o son codigo real del EXE (jpeg_consume_input, solo en read_header) o falsos en la direccion del EXE (jinit_*)"})},
                                 {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    Run(env, ready, registered, bodies, collaborators);
}
}

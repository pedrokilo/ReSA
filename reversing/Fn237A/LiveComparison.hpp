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

// Banco par a par Fn237A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 2 funciones (CRunningScript::ProcessCommands1200To1299 y 1900To1999), cada una con 16384 pares EXE/DLL. Cada caso construye una escena (arenas A..D, memoria global del EXE: pools sinteticos de peds, vehiculos
// y objetos, jugadores, tablas de zonas...) descrita en bank_spec.py (tablas de GenTables.hpp), fija los parametros de script y llama a la funcion con un numero de comando.
// Colaboradores: los que tocan estado del juego son falsos (GenStubs.hpp) instalados en la entrada del EXE y en el cuerpo de la DLL, que registran la llamada y responden de forma determinista por
// semilla; los puros (pools, vectores, cuaterniones, constructores de tareas, claves) son reales con el gancho apagado en ambos lados. Se observan los bytes cambiados de las arenas y de la
// memoria global, los eventos de los falsos y AL.

namespace F237ALive {
using namespace F237AFix;
namespace G = F237AGen;
inline constexpr u32 kGuard = G::kGuard;
inline constexpr std::array<uintptr_t, kFunctions> kEntries{G::kEntries[0], G::kEntries[1]};

// ================================================================================================ arenas (alineadas a 4096)
inline constexpr size_t Win(size_t size) { return (kGuard + size + kGuard + 0xFFF) & ~size_t(0xFFF); }
inline constexpr size_t kWin[4] = {Win(G::kArenaSize[0]), Win(G::kArenaSize[1]), Win(G::kArenaSize[2]), Win(G::kArenaSize[3])};
inline constexpr size_t kMaxWin = 0xA000;
static_assert(kWin[0] <= kMaxWin && kWin[1] <= kMaxWin && kWin[2] <= kMaxWin && kWin[3] <= kMaxWin);
alignas(4096) inline std::array<std::array<uint8, kMaxWin>, 4> gArena{};
alignas(4096) inline std::array<std::array<uint8, kMaxWin>, 4> gRef{};
inline uintptr_t ArenaBase(unsigned i) { return uintptr_t(gArena[i].data()); }
inline uintptr_t AddrOf(unsigned arena, u32 off) { return arena < 4 ? ArenaBase(arena) + kGuard + off : uintptr_t(off); }
inline u32 gKeys[8]{};
inline u32 AnsV(u32 id, u32 j);
inline u32 ResolveVal(const G::Val& v);

// Cambios de una arena respecto a la imagen de referencia, en tramos contiguos: [[desplazamiento respecto al objeto 0 del primer byte, bytes nuevos en hexadecimal]]
inline uintptr_t gAsiBase{}, gAsiSize{};
// En la arena de reservas de tareas (1) los dwords alineados que cambian y apuntan a una tabla virtual (.rdata del EXE, o ASI fuera de las arenas) se sustituyen por 0xFEEDF00D (el EXE deja sus tablas y la
// DLL las suyas) y (2) los bytes que quedan a 0x00 se consideran sin escribir (rellenos de alineacion: el EXE no los escribe y la DLL pone a cero el miembro entero).
inline json ArenaChangesRuns(unsigned i) {
    json out = json::array();
    constexpr char d[] = "0123456789abcdef";
    const size_t n = kWin[i];
    std::vector<uint8> norm;
    const uint8* m = gArena[i].data();
    const uint8* r = gRef[i].data();
    if (i == 2) {
        norm.assign(m, m + n);
        auto inArena = [](u32 v) { for (unsigned a = 0; a < 4; ++a) if (v >= ArenaBase(a) && v < ArenaBase(a) + kWin[a]) return true; return false; };
        for (size_t k = 0; k + 3 < n; k += 4) {
            u32 dv, rv; memcpy(&dv, &norm[k], 4); memcpy(&rv, r + k, 4);
            if (dv != rv && ((dv >= 0x858000u && dv < 0x8A4000u) || (dv >= gAsiBase && dv < gAsiBase + gAsiSize && !inArena(dv)))) { const u32 t = 0xFEEDF00Du; memcpy(&norm[k], &t, 4); }
        }
        for (size_t k = 0; k < n; ++k) if (norm[k] == 0) norm[k] = r[k];
        m = norm.data();
    }
    for (size_t k = 0; k < n;) {
        if (m[k] == r[k]) { ++k; continue; }
        std::string hex;
        const size_t start = k;
        while (k < n && m[k] != r[k]) { hex += d[m[k] >> 4]; hex += d[m[k] & 15]; ++k; }
        out.push_back(json::array({int64(start) - int64(kGuard), hex}));
    }
    return out;
}

// ================================================================================================ falsos (iguales en el EXE y en la DLL; el oraculo emu_oracle.py los reproduce)
inline u32 gSeed{}, gNextSlot{}, gFloatBits{};
inline int gNa{};
inline std::array<u32, 32> gParams{};
inline std::array<u32, G::kCollabs> gCnt{};
inline u32 AnsV(u32 id, u32 j) { return Mix(u32(gSeed * 0x9E3779B1u + id * 0x7FEB352Du + (gCnt[id] * 16 + j) * 0x846CA68Bu + 0x1234567u)); }
inline u32 FnvStr64(const char* s) { u32 h = 2166136261u; for (unsigned i = 0; i < 64 && s[i]; ++i) { h ^= u8(s[i]); h *= 16777619u; } return h; }
inline u32 Rd32(uintptr_t a) { u32 v; memcpy(&v, reinterpret_cast<const void*>(a), 4); return v; }
inline void Wr32(uintptr_t a, u32 v) { memcpy(reinterpret_cast<void*>(a), &v, 4); }
inline void Wr8(uintptr_t a, u32 v) { *reinterpret_cast<u8*>(a) = u8(v); }
inline u32 ResolveVal(const G::Val& v) {
    switch (v.kind) {
    case 0: return v.v;
    case 1: return u32(AddrOf(v.arena, v.v));
    case 2: return 0;   // se resuelve mas abajo (necesita kFakeFn)
    case 3: return gKeys[v.v == 100 ? AnsV(G::kIdReadLabel, 0) % (sizeof(G::kLabels) / sizeof(G::kLabels[0])) : v.v];
    }
    return 0;
}
inline u32 __cdecl FakeCore(u32 id, u32 ecx, const u32* args);
#include "GenStubs.hpp"
inline u32 ResolveValFull(const G::Val& v) { return v.kind == 2 ? u32(uintptr_t(kFakeFn[v.v])) : ResolveVal(v); }

inline u32 __cdecl FakeCore(u32 id, u32 ecx, const u32* args) {
    const G::Collab& c = G::kCollab[id];
    const unsigned nargs = unsigned(std::strlen(c.args));
    Ev& e = PushEv(id, 0);
    auto add = [&](u32 v) { if (e.len < e.v.size()) e.v[e.len++] = v; };
    if (c.conv == 'T') add(Tag(ecx));
    const u32 a0 = AnsV(id, 0);
    u32 eax = 0;
    bool haveFloat = false;
    unsigned k = 0;
    for (unsigned i = 0; i < nargs; ++i) {
        const u32 v = args[i];
        switch (c.args[i]) {
        case 'w': add(Tag(uintptr_t(v))); break;
        case 'f': add(v); break;
        case 'B': add(v & 0xFF); break;
        case 'H': add(v & 0xFFFF); break;
        case 'v': add(Rd32(v)); add(Rd32(v + 4)); add(Rd32(v + 8)); break;
        case 'u': add(Rd32(v)); add(Rd32(v + 4)); break;
        case 's': add(FnvStr64(reinterpret_cast<const char*>(uintptr_t(v)))); break;
        case 'm': { const uintptr_t m = v; for (u32 o : {0u, 4u, 8u, 0x10u, 0x14u, 0x18u, 0x20u, 0x24u, 0x28u}) add(Rd32(m + o)); break; }
        case 'x': break;
        case 'L': {
            const unsigned li = a0 % (sizeof(G::kLabels) / sizeof(G::kLabels[0]));
            const char* label = G::kLabels[li];
            const unsigned n = args[i + 1] & 0xFF;
            const size_t len = std::strlen(label);
            char* dst = reinterpret_cast<char*>(uintptr_t(v));
            const size_t copy = n ? std::min<size_t>(len, n - 1) : 0;
            std::memcpy(dst, label, copy);
            dst[copy] = 0;
            add(li);
            break;
        }
        case 'o': Wr32(v, G::kNodes[AnsV(id, 1 + k) % (sizeof(G::kNodes) / 4)]); ++k; break;
        case 'g': Wr32(v, G::kFloats[AnsV(id, 1 + k) % (sizeof(G::kFloats) / 4)]); ++k; break;
        case 'i': Wr32(v, AnsV(id, 1 + k) & 7); ++k; break;
        case 'b': if (v) Wr8(v, AnsV(id, 1 + k) & 1); ++k; break;
        case 'O': for (unsigned j = 0; j < 3; ++j) Wr32(v + 4 * j, G::kFloats[AnsV(id, 1 + k + j) % (sizeof(G::kFloats) / 4)]); k += 3; eax = v; break;
        case 'N': Wr32(v, G::kNodes[AnsV(id, 1 + k) % (sizeof(G::kNodes) / 4)]); ++k; eax = v; break;
        default: std::abort();
        }
    }
    constexpr uintptr_t kScriptParams = 0xA43C78;
    const std::string_view name = c.name;
    if (name == "CollectParameters") { for (u32 i = 0; i < args[0]; ++i) Wr32(kScriptParams + 4 * i, gParams[i]); }
    else if (name == "StoreParameters") { for (u32 i = 0; i < args[0]; ++i) add(Rd32(kScriptParams + 4 * i)); }
    switch (c.ret) {
    case 'v': break;
    case 'w': eax = a0 & 0x7FFFFu; break;
    case 'b': eax = a0 & 1; break;
    case 'n': eax = a0 % 8; break;
    case 'f': gFloatBits = G::kFloats[a0 % (sizeof(G::kFloats) / 4)]; haveFloat = true; break;
    case 'z': eax = G::kZones[a0 % (sizeof(G::kZones) / 4)]; break;
    case 't': eax = G::kThings[a0 % (sizeof(G::kThings) / 4)]; break;
    case 'o': {
        if (unsigned(c.set) == G::kTaskMemSet) {
            const u32 slot = gNextSlot;
            if ((gNa && a0 % 4 == 0) || slot >= G::kNSlotC) eax = 0;
            else { ++gNextSlot; eax = u32(AddrOf(2, G::kSlotC * slot)); }
        } else {
            const G::PtrSet& ps = G::kPtrSets[c.set];
            eax = ResolveVal(ps.v[a0 % ps.n]);
        }
        break;
    }
    default: std::abort();
    }
    (void)haveFloat;
    ++gCnt[id];
    return eax;
}

// ================================================================================================ cierre de un lado
inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

// ================================================================================================ escena
inline void FillRandom(u32 seed) {
    for (unsigned k = 0; k < sizeof(G::kFills) / sizeof(G::kFills[0]); ++k) {
        const G::Fill& f = G::kFills[k];
        u32 s = Mix(seed ^ ((k + 1) * 0x9E3779B1u));
        if (!s) s = 1;
        uint8* dst = reinterpret_cast<uint8*>(AddrOf(f.arena, f.off));
        for (u32 i = 0; i < f.n; i += 4) {
            s ^= s << 13; s ^= s >> 17; s ^= s << 5;
            const u32 left = f.n - i;
            const u32 val = f.kind ? G::kFSet[(s >> 8) % (sizeof(G::kFSet) / 4)] : s;
            memcpy(dst + i, &val, left < 4 ? left : 4);
        }
    }
}
inline void PokeVal(unsigned arena, u32 off, unsigned size, u32 val) { memcpy(reinterpret_cast<void*>(AddrOf(arena, off)), &val, size); }

// Prepara arenas y la escena del caso
inline void Setup(const C& c) {
    for (unsigned i = 0; i < 4; ++i) {
        auto& m = gArena[i];
        std::fill(m.begin(), m.begin() + kWin[i], uint8(0xAA));
        std::fill(m.begin() + kGuard, m.begin() + kGuard + G::kArenaSize[i], uint8(i < 2 ? 0x00 : i == 2 ? 0xCD : 0xDD));
    }
    FillRandom(c.seed);
    gSeed = c.seed; gCnt.fill(0);
    for (const auto& p : G::kStruct) PokeVal(p.arena, p.off, p.size, ResolveValFull(p.val));
    for (unsigned i = 0; i < sizeof(G::kChoices) / sizeof(G::kChoices[0]); ++i) {
        const auto& ch = G::kChoices[i];
        PokeVal(ch.arena, ch.off, ch.size, ResolveValFull(ch.v[Hash(c.seed, i) % ch.n]));
    }
    gSeed = c.seed; gNextSlot = 0; gNa = c.na; gParams = c.params; gCnt.fill(0);
    nev = 0;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(ArenaBase(0)), u32(ArenaBase(1)), u32(ArenaBase(2)), u32(ArenaBase(3))});   // direcciones reales de las arenas (para el oraculo)
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        Setup(c);
        for (unsigned i = 0; i < 4; ++i) gRef[i] = gArena[i];
        SnapRegions();
        TagClear(); for (unsigned i = 0; i < 4; ++i) TagAdd(gArena[i].data(), kWin[i]);
        nev = 0;
        const u32 script = u32(AddrOf(0, G::kScript));
        const u32 arg = u32(c.cmd);
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, &arg, 1, c.eax, script, c.edx, c.stack, 0, false);
        });
        json obs;
        obs["changes"] = {{"A", ArenaChangesRuns(0)}, {"B", ArenaChangesRuns(1)}, {"C", ArenaChangesRuns(2)}, {"D", ArenaChangesRuns(3)}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        obs["ret"] = sides[s].reg.eax & 0xFF;
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN237A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN237A_CASES", buffer, sizeof(buffer)) > 0) {
        const unsigned v = unsigned(std::atoi(buffer));
        if (v >= kFamilies && v <= kCases && v % kFamilies == 0) return v;
    }
    return kCases;
}
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && InAsi(env.asi, body);
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

// Colaborador con hook RH en la DLL: la ruta deja su hook apagado (en ambos lados el EXE corre sus bytes y la DLL llama al cuerpo propio).
inline ReversibleHooks::ReversibleHook::Simple* FindSimple(uintptr_t exe) {
    ReversibleHooks::ReversibleHook::Simple* found = nullptr;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items())
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe)
                found = static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get());
    });
    return found;
}
// Cuerpo que llama de verdad el codigo de la DLL cuando el gancho registrado es una envoltura distinta (p. ej. X_Reversed que llama a la implementacion `const`).
inline uintptr_t DllCallee(const char* name) {
    const std::string_view n = name;
    if (n == "GetVehicleAppearance") return std::bit_cast<uintptr_t>(&CVehicle::GetVehicleAppearance);
    return 0;
}
// Colaborador con hook RH en la DLL: la ruta deja su hook apagado. Muchos ganchos de CRunningScript se instalan con un trampolin de ECX (GetHookOurAddress() es esa pagina propia, y con el gancho
// apagado su primer salto se pisa): el cuerpo que llama el codigo de la DLL es el destino del `call` del trampolin, que se lee un instante con el gancho encendido.
inline Pair MakeCollab(Env& env, uintptr_t exe, json& hooksFound, const char* collabName = "") {
    Pair p{exe, 0, false};
    std::string cat, name;
    if (FindHook(exe, cat, name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}});
        if (env.collab.back()->Registered()) {
            p.dll = Body(env.collab.back()->Body()); p.hooked = true;
            if (auto* simple = FindSimple(exe)) {
                simple->State(true);
                const uintptr_t our = reinterpret_cast<uintptr_t>(simple->GetHookOurAddress());
                const uintptr_t t = ThunkTarget(our);
                simple->State(false);
                if (t) p.dll = Body(t);
            }
            if (const uintptr_t over = DllCallee(collabName)) p.dll = Body(over);
        }
    }
    return p;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn237a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn237a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn237a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    gAsiBase = env.asi;
    { const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(env.asi); gAsiSize = reinterpret_cast<const IMAGE_NT_HEADERS*>(env.asi + dos->e_lfanew)->OptionalHeader.SizeOfImage; }
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name; uintptr_t vbody = 0;
        const bool found = FindHook(kEntries[f], cat, name, &vbody);
        if (found) {
            // Estos ganchos se instalan con { .stackArguments = 1 }: GetHookOurAddress() es el trampolin de ECX (pagina propia, y su primer salto se pisa con el gancho apagado); el cuerpo
            // real de la DLL se toma del puntero a la funcion miembro.
            if (f == 0) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], &CRunningScript::ProcessCommands1200To1299));
            else env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], &CRunningScript::ProcessCommands1900To1999));
        }
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && InAsi(env.asi, env.route[f]->Body());
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}, {"virtual", vbody != 0}, {"body", found ? env.route[f]->Body() : 0}, {"body_module", found ? ModuleOf(env.route[f]->Body()) : 0},
            {"thunk_target", found ? ThunkTarget(env.route[f]->Body()) : 0}, {"thunk_module", found ? ModuleOf(ThunkTarget(env.route[f]->Body())) : 0}, {"asi", env.asi}}) : json(nullptr);
    }
    if (!ready) { collaborators = ownHooks; Run(env, false, registered, bodies, collaborators); return; }
    // claves de las etiquetas (GetUppercaseKey de la DLL)
    for (unsigned j = 0; j < 8; ++j) gKeys[j] = CKeyGen::GetUppercaseKey(G::kLabels[j]);
    json hooksFound = json::array(), stubbedJson = json::array(), realJson = json::array();
    std::vector<StubSpec> stubs;
    std::vector<Pair> reals;
    for (unsigned i = 0; i < G::kCollabs; ++i) {
        const G::Collab& c = G::kCollab[i];
        if (!c.exe) continue;
        Pair p = MakeCollab(env, c.exe, hooksFound, c.name);
        if (c.mode == 'F') {
            stubs.push_back(StubSpec{p, kFakeFn[i]});
            stubbedJson.push_back({{"exe", c.exe}, {"hooked", p.hooked}, {"name", c.name}, {"dll", p.dll != 0}});
        } else {
            reals.push_back(p);
            realJson.push_back({{"exe", c.exe}, {"hooked", p.hooked}, {"name", c.name}, {"dll", p.dll != 0}, {"dll_module", p.dll ? ModuleOf(p.dll) : 0}});
        }
    }
    for (unsigned f = 0; f < kFunctions; ++f) {
        env.stubs[f] = stubs;
        env.reals[f] = reals;
        env.watches.emplace_back(kEntries[f], 16);
    }
    for (const auto& s : stubs) env.watches.emplace_back(s.pair.exe, 16);
    for (const auto& pr : reals) env.watches.emplace_back(pr.exe, 16);
    for (unsigned f = 0; f < kFunctions; ++f)
        collaborators[kFn[f]] = {{"stubbed", stubbedJson}, {"real", realJson}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    for (const auto& g : G::kGlobals) regions.emplace_back(g.at, g.n);
    for (unsigned f = 0; f < kFunctions; ++f) for (size_t i = 0; i < regions.size(); ++i) regionsOf[f].push_back(i);
    Run(env, ready, registered, bodies, collaborators);
}
}

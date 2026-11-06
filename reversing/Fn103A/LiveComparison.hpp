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
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn103A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Siete funciones (hook S), cada una con 16384 pares EXE/DLL (ver Fixtures.hpp para las firmas):
//   0 CMatrix::SetRotateXOnly 0x59AFA0 | 1 SetRotateYOnly 0x59AFE0 | 2 Rotate 0x59B460 | 3 ForceUpVector 0x59B7E0 | 4 SetRotate(quat) 0x59BBF0 | 5 operator+ 0x59BFA0 | 6 CVector::FromMultiply3x3 0x59C6D0
// Ninguna llama a otra funcion: toda la aritmetica x87 corre de verdad en ambos lados (x87 del EXE frente al C++ de la DLL, con la palabra de control del juego: 24 bits, redondeo al mas cercano).
// La memoria es una ventana de 96 dwords con punteros que pueden solaparse. operator+ copia basura de la pila a las palabras de relleno (+0x0C, +0x1C, +0x2C, +0x3C) del resultado: se enmascaran
// (se restauran a su valor inicial en ambos lados antes de comparar).

namespace F103ALayout {
static_assert(sizeof(CVector) == 12 && sizeof(CMatrix) == 0x48, "CVector y CMatrix");
}

namespace F103ALive {
using namespace F103AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x59AFA0, 0x59AFE0, 0x59B460, 0x59B7E0, 0x59BBF0, 0x59BFA0, 0x59C6D0};
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arena
template<size_t WIN> struct Arena {
    static constexpr size_t N = F103AFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            if (m[i] != r[i]) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kWin> memA;             // 0: ventana de 48 dwords con guarda de 0x40 bytes a cada lado

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
        memA.Fill(c.seed * 13u + 50u);
        TagClear();
        TagAdd(memA.m.data(), memA.m.size());     // 0
        nev = 0;
        for (unsigned k = 0; k < kWords; ++k) memA.Put32(4 * k, c.mem[k]);
        memA.Snap();
        const u32 pa = u32(memA.Obj(4 * c.pa)), pb = u32(memA.Obj(4 * c.pb)), po = u32(memA.Obj(4 * c.po));
        std::vector<u32> args;
        u32 ecx = c.seed, popBytes = 0;
        bool eaxIsOut = false;
        switch (fn) {
        case 0: case 1: ecx = po; args = {c.f0}; break;                                    // SetRotateXOnly/YOnly (matriz = po, angulo), `ret 4`
        case 2: case 3: ecx = po; args = {c.f0, c.f1, c.f2}; break;                        // Rotate (x, y, z) y ForceUpVector (up por valor), `ret 0Ch`
        case 4: ecx = po; args = {pb}; break;                                              // SetRotate(quat): matriz = po, CQuaternion* = pb, `ret 4`
        case 5: args = {po, pa, pb}; popBytes = 12; eaxIsOut = true; break;                // operator+: cdecl, puntero de retorno oculto (po), a (pa), b (pb); EAX = destino
        default: ecx = po; args = {pa, pb}; break;                                         // CVector::FromMultiply3x3 (this = po; matriz = pa; vector = pb), `ret 8`
        }
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs;
        if (fn == 5) {   // el relleno del resultado (basura de la pila en el EXE): se devuelve a su valor inicial en ambos lados
            for (u32 k = 3; k < 16; k += 4) {
                u32 init; memcpy(&init, memA.r.data() + kGuard + 4 * (c.po + k), 4);
                memA.Put32(4 * (c.po + k), init);
            }
        }
        json ch = json::array();
        for (const auto& e : memA.Changes()) ch.push_back(json::array({0, e[0], e[1]}));
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        // Lo que ven los llamadores del EXE: EAX (el puntero `out`) en Invert(in), que leen cuatro de sus cinco llamadores
        if (eaxIsOut) obs["exit_eax"] = Tag(uintptr_t(sides[s].reg.eax));
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
        for (const auto& st : env.stubs[f]) {
            if (st.pair.hooked) ready &= st.pair.dll != 0 && ModuleOf(st.pair.dll) == env.asi;
            else ready &= st.pair.dll == 0 || ModuleOf(st.pair.dll) == env.asi;
        }
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
    // Fin de la prueba: se termina el proceso en el acto. Sin cuadros posteriores del juego (y con todos los hooks del lote activos) no puede saltar ninguna aserción ajena al banco, como la VERIFY de
    // CShadowCamera::DrawOutlineBorder (RwIm2DRenderIndexedPrimitive) que salto en 3 de 5 corridas largas mientras el analizador leia el registro con el juego aun vivo.
    ExitProcess(0);
}

// Busca el hook Simple registrado para una direccion del EXE (categoria y nombre), para crear su ruta (apagada) y obtener el cuerpo de la DLL.
inline bool FindHook(uintptr_t exe, std::string& cat, std::string& name) {
    bool found = false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items())
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
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
    if (!std::filesystem::exists(dir / "fn103a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn103a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn103a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name;
        const bool found = FindHook(kEntries[f], cat, name);
        if (found) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}}) : json(nullptr);
    }
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    json hooksFound = json::array();
    const char* dllReal[kFunctions] = {
        "toda la aritmetica x87 (fcos/fsin guardados como float) y los ceros y el uno escritos; sin colaboradores",
        "toda la aritmetica x87 (fcos/fsin guardados como float) y los ceros y el uno escritos; sin colaboradores",
        "toda la aritmetica x87 (fcos, fsin, productos, sumas a 24 bits, las diez magnitudes guardadas como float y las escrituras en el orden del EXE); sin colaboradores",
        "toda la aritmetica x87 (productos vectoriales con operandos de registro y de memoria como el EXE, C guardado como float, up copiado como dwords); sin colaboradores",
        "toda la aritmetica x87 (productos de 2q por q y por la parte real, guardados como float o en registro, 1 - (a + b)); sin colaboradores",
        "las doce sumas x87 en un temporal, el resultado, +0x40 y +0x44 a cero; sin colaboradores",
        "toda la aritmetica x87 (productos y sumas a 24 bits, cada componente se escribe antes de calcular la siguiente); sin colaboradores"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& st : env.stubs[f]) {
            env.watches.emplace_back(st.pair.exe, 16);
            stubbed.push_back({{"exe", st.pair.exe}, {"dll", st.pair.dll != 0}, {"hooked", st.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);   // entradas del EXE: deben seguir siendo lo que eran al empezar
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}

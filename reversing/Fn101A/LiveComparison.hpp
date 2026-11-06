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


// Banco par a par Fn101A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Diez funciones (hook S), cada una con 16384 pares EXE/DLL (ver Fixtures.hpp para las firmas):
//   0 CMatrix::SetRotateX 0x59B060 | 1 SetRotateY 0x59B0A0 | 2 SetRotateZ 0x59B0E0 | 3 RotateX 0x59B1E0 | 4 RotateY 0x59B2C0 | 5 RotateZ 0x59B390 | 6 Reorthogonalise 0x59B6A0
//   7 CopyToRwMatrix 0x59B8B0 (falso: RwMatrixUpdate 0x7F18A0) | 8 Invert(in) 0x59BDD0 (falsos: Invert(in, out) 0x59B920 con hook RH y RwMatrixDestroy 0x7F2A20) | 9 CVector::FromMultiply 0x59C670
// Las siete primeras y la ultima no llaman a ninguna otra funcion: toda la aritmetica x87 corre de verdad en ambos lados (x87 del EXE frente al C++ de la DLL, con la palabra de control del juego: 24 bits,
// redondeo al mas cercano). Los colaboradores son falsos identicos en la entrada del EXE y (los que tienen hook RH, apagado durante el banco) en el cuerpo de la DLL; registran sus argumentos EN ORDEN
// (punteros a la ventana como etiquetas, temporales de pila como 0x7F000000). La memoria es una ventana de 96 dwords con punteros que pueden solaparse.

namespace F101ALayout {
static_assert(sizeof(CVector) == 12 && sizeof(CMatrix) == 0x48, "CVector y CMatrix");
}

namespace F101ALive {
using namespace F101AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x59B060, 0x59B0A0, 0x59B0E0, 0x59B1E0, 0x59B2C0, 0x59B390, 0x59B6A0, 0x59B8B0, 0x59BDD0, 0x59C670};
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arena
template<size_t WIN> struct Arena {
    static constexpr size_t N = F101AFix::Arena(WIN);
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

// Eventos: 50 RwMatrixUpdate 0x7F18A0 [rw]; 52 RwMatrixDestroy 0x7F2A20 [rw]; 54 Invert(in, out) 0x59B920 [in, out]
inline u32 __cdecl FkRwUpdate(void* rw) { Ev& e = PushEv(50, 1); e.v[0] = Tag(rw); return 1; }                                      // 0x7F18A0 (cdecl)
inline u32 __cdecl FkRwDestroy(void* rw) { Ev& e = PushEv(52, 1); e.v[0] = Tag(rw); return 1; }                                   // 0x7F2A20 (cdecl)
// 0x59B920 (cdecl, devuelve out): el falso escribe los 16 dwords de `out` (el relleno incluido, que el EXE copia al resultado) con el contenido de `in` mezclado con una constante por posicion,
// para que la copia posterior del temporal al resultado sea observable; devuelve out (el EXE copia desde EAX).
inline u32 __cdecl FkInvert1(const u32* in, u32* out) {
    Ev& e = PushEv(54, 2); e.v[0] = Tag(in); e.v[1] = Tag(out);
    u32 tmp[16];
    for (u32 k = 0; k < 16; ++k) tmp[k] = in[k] ^ (0x9E3779B1u * (k + 1));
    for (u32 k = 0; k < 16; ++k) out[k] = tmp[k];
    return u32(uintptr_t(out));
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
        case 0: case 1: case 2: case 3: case 4: case 5: ecx = po; args = {c.f0}; break;   // SetRotateX/Y/Z y RotateX/Y/Z (matriz = po, angulo), `ret 4`
        case 6: ecx = po; break;                                                           // Reorthogonalise (matriz = po), `ret`
        case 7: ecx = pa; args = {pb}; break;                                              // CopyToRwMatrix (matriz = pa, RwMatrix* = pb), `ret 4`
        case 8: args = {po, pa}; popBytes = 8; eaxIsOut = true; break;                     // Invert(in): cdecl, puntero de retorno oculto (po) e `in` (pa); EAX = destino
        default: ecx = po; args = {pa, pb}; break;                                         // CVector::FromMultiply (this = po; matriz = pa; vector = pb), `ret 8`
        }
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs;
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
    if (!std::filesystem::exists(dir / "fn101a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn101a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn101a"); }
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
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    env.stubs[7] = {F(0x7F18A0, &FkRwUpdate)};
    env.stubs[8] = {F(0x59B920, &FkInvert1), F(0x7F2A20, &FkRwDestroy)};
    const char* dllReal[kFunctions] = {
        "toda la aritmetica x87 (fcos/fsin guardados como float) y los ceros y el uno escritos; sin colaboradores",
        "toda la aritmetica x87 (fcos/fsin guardados como float) y los ceros y el uno escritos; sin colaboradores",
        "toda la aritmetica x87 (fcos/fsin guardados como float) y los ceros y el uno escritos; sin colaboradores",
        "toda la aritmetica x87 (fcos, fsin, productos, sumas y restas a 24 bits y las escrituras en el orden del EXE); sin colaboradores",
        "toda la aritmetica x87 (fcos, fsin, productos, sumas y restas a 24 bits y las escrituras en el orden del EXE); sin colaboradores",
        "toda la aritmetica x87 (fcos, fsin, productos, sumas y restas a 24 bits y las escrituras en el orden del EXE); sin colaboradores",
        "toda la aritmetica x87 (productos vectoriales, dos fsqrt y dos divisiones 1/x, sumas y restas a 24 bits); sin colaboradores",
        "la copia de los 12 dwords en el orden del EXE; RwMatrixUpdate (0x7F18A0) es un falso identico",
        "el temporal de pila, la copia de 16 dwords, el borrado de +0x40 y +0x44 y la comprobacion de la bandera del temporal; Invert(in, out) (0x59B920, con hook RH) y RwMatrixDestroy (0x7F2A20) son falsos identicos",
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

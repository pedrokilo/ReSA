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


// Banco par a par Fn105A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Diez funciones (hook S), cada una con 16384 pares EXE/DLL (ver Fixtures.hpp para las firmas):
//   0 TList_c::AddItem 0x4A8DF0 | 1 RemoveAll 0x4A8EB0 | 2 GetNumItems 0x4A8EC0 | 3 CMatrix::operator*= 0x411A80 | 4 CVector / float 0x4119D0 | 5 CompressedVector a CVector 0x411590
//   6 CColPoint::operator= 0x40FC80 | 7 iterador de constructores de vectores 0x403D60 | 8 CVector::operator-= 0x406D70 | 9 CStreaming::SetModelTxdIsDeletable 0x409C70
// Sin colaboradores: 0, 1, 2, 4, 5, 6, 8 (la aritmetica x87 y los movimientos de memoria corren de verdad en ambos lados) y 7 (el constructor es una funcion falsa pasada como argumento).
// Con colaboradores falseados (falsos identicos en la direccion del EXE y en el cuerpo de la DLL, todos con hook RH): operator*= (0x59BE30, 0x59BBC0, 0x59ACD0) y SetModelTxdIsDeletable (0x409C10).
// La memoria es una ventana de 96 dwords con punteros que pueden solaparse; la tabla de modelos 0xA9B0C8 (solo SetModelTxdIsDeletable) se guarda entera y se restaura tras cada lado.

namespace F105ALayout {
static_assert(sizeof(CVector) == 12 && sizeof(CMatrix) == 0x48, "CVector y CMatrix");
}

namespace F105ALive {
using namespace F105AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x4A8DF0, 0x4A8EB0, 0x4A8EC0, 0x411A80, 0x4119D0, 0x411590, 0x40FC80, 0x403D60, 0x406D70, 0x409C70};
// Depuracion: si existe fn105a-quick.txt (mascara de funciones y casos por funcion) el banco corre solo esa parte; la corrida aprobada NUNCA lo usa (run_local.py exige 16384 por funcion).
inline unsigned quickMask = (1u << kFunctions) - 1u, quickCases = kCases;
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arena
template<size_t WIN> struct Arena {
    static constexpr size_t N = F105AFix::Arena(WIN);
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

// Falsos de los colaboradores (cuerpo identico en el EXE y en el cuerpo de la DLL; los eventos llevan todos los argumentos):
//   10 0x59BE30 MatrixMultiplyExe cdecl (out, a, b) [out, a, b, hash de a, hash de b]: rellena out con una funcion de ambos hashes (el orden de a y b se nota) y devuelve out
//   11 0x59BBC0 CMatrix::operator= thiscall (this, origen) `ret 4` [this, origen, hash de las 16 palabras de origen]: copia 16 dwords (efecto visible en la ventana)
//   12 0x59ACD0 CMatrix::Destructor thiscall (this) [this]
//   13 0x409C10 CStreaming::SetModelIsDeletable cdecl (id) [id]
//   14 constructor del iterador de vectores, thiscall (this) [this]
inline u32 __cdecl FkMatMul(u32 out, u32 a, u32 b) {
    Ev& e = PushEv(10, 5);
    const u32 ha = Fnv(reinterpret_cast<const void*>(a), 0x48), hb = Fnv(reinterpret_cast<const void*>(b), 0x48);
    e.v[0] = Tag(out); e.v[1] = Tag(a); e.v[2] = Tag(b); e.v[3] = ha; e.v[4] = hb;
    u32* o = reinterpret_cast<u32*>(out);
    for (u32 k = 0; k < 16; ++k) o[k] = Rand(ha ^ (hb * 31u), k);
    o[16] = 0; o[17] = 0;
    return out;
}
inline u32 __fastcall FkMatAssign(void* self, void*, void* src) {
    Ev& e = PushEv(11, 3);
    e.v[0] = Tag(self); e.v[1] = Tag(src); e.v[2] = Fnv(src, 0x40);
    u32 tmp[16];
    memcpy(tmp, src, 0x40);
    memcpy(self, tmp, 0x40);
    return 0;
}
inline u32 __fastcall FkMatDtor(void* self, void*) { Ev& e = PushEv(12, 1); e.v[0] = Tag(self); return 0; }
inline u32 __cdecl FkSetDeletable(u32 id) { Ev& e = PushEv(13, 1); e.v[0] = id; return 0; }
inline u32 __fastcall FkCtor(void* self, void*) { Ev& e = PushEv(14, 1); e.v[0] = Tag(self); return 0; }

inline constexpr uintptr_t kModelInfoTable = 0xA9B0C8;
inline constexpr size_t kModelInfoBytes = 4 * 20000;

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
        for (u32 k = 0; k < c.nfix; ++k) memA.Put32(4 * c.fixAt[k], u32(memA.Obj(4 * c.fixTo[k])));
        memA.Snap();
        const u32 pa = u32(memA.Obj(4 * c.pa)), pb = u32(memA.Obj(4 * c.pb)), po = u32(memA.Obj(4 * c.po));
        std::vector<u32> args;
        u32 ecx = c.seed, popBytes = 0;
        enum class EaxObs { None, Raw, Pointer } eaxObs = EaxObs::None;
        switch (fn) {
        case 0: ecx = po; args = {pa}; eaxObs = EaxObs::Raw; break;                                                // AddItem(lista = po, nodo = pa), `ret 4`; EAX = recuento anterior
        case 1: ecx = po; eaxObs = EaxObs::Raw; break;                                                             // RemoveAll(lista = po); EAX = 0
        case 2: ecx = po; eaxObs = EaxObs::Raw; break;                                                             // GetNumItems(lista = po); EAX = recuento
        case 3: ecx = po; args = {pb}; break;                                                                      // operator*=(this = po, matriz = pb), `ret 4`
        case 4: args = {po, pa, c.f0}; popBytes = 12; eaxObs = EaxObs::Pointer; break;                             // vector / escalar: cdecl (out, vector, divisor); EAX = out
        case 5: args = {po, u32(memA.Obj(4 * c.pa + c.f1))}; popBytes = 8; break;                                  // descomprimir: cdecl (out, int16*)
        case 6: ecx = u32(memA.Obj(4 * c.po + c.f0)); args = {pa}; eaxObs = EaxObs::Pointer; break;                // CColPoint::operator=(this, origen), `ret 4`; EAX = this
        case 7: args = {pa, c.f0, c.f1, u32(uintptr_t(&FkCtor))}; break;                                           // iterador: stdcall (base, tamano, recuento, constructor), `ret 10h`
        case 8: ecx = po; args = {pa}; break;                                                                      // operator-=(this = po, otro = pa), `ret 4`
        default: args = {c.f0}; popBytes = 4; break;                                                               // SetModelTxdIsDeletable(id), cdecl
        }
        if (fn == 9) *reinterpret_cast<u32*>(kModelInfoTable + 4 * c.f0) = u32(memA.Obj(4 * c.pa));                // la entrada de la tabla de modelos apunta al objeto de la ventana (+0xA = TXD)
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs;
        json ch = json::array();
        for (const auto& e : memA.Changes()) ch.push_back(json::array({0, e[0], e[1]}));
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        obs["base"] = u32(memA.Obj(0));
        // Lo que ven los llamadores del EXE: EAX solo donde alguno lo lee (recuento de AddItem/GetNumItems, el puntero de salida de vector / escalar y de CColPoint::operator=)
        if (eaxObs == EaxObs::Raw) obs["exit_eax"] = sides[s].reg.eax;
        else if (eaxObs == EaxObs::Pointer) obs["exit_eax"] = Tag(uintptr_t(sides[s].reg.eax));
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
    json cases = json::object(); for (unsigned f = 0; f < kFunctions; ++f) cases[kFn[f]] = (quickMask >> f & 1u) ? quickCases : 0u;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", std::popcount(quickMask) * quickCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        for (unsigned f = 0; f < kFunctions; ++f) {
            if (!(quickMask >> f & 1u)) continue;
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < quickCases; ++c) CaseRun(env, totals, f, c);
            Write({{"event", "function_summary"}, {"function", kFn[f]}, {"cases", quickCases}, {"failures", totals.failures - before}, {"state_restored", env.WatchesIntact() && RegionsIntact()}});
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
    if (!std::filesystem::exists(dir / "fn105a-runtime.start")) { done = true; return; }
    if (!log.is_open()) {
        if (std::ifstream q(dir / "fn105a-quick.txt"); q) { unsigned m{}, nc{}; if (q >> m >> nc && nc <= kCases) { quickMask = m & ((1u << kFunctions) - 1u); quickCases = nc; } } log.open(dir / "fn105a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn105a"); }
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
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return StubSpec{it->second, reinterpret_cast<void*>(fake)};
    };
    env.stubs[3] = {F(0x59BE30, &FkMatMul), F(0x59BBC0, &FkMatAssign), F(0x59ACD0, &FkMatDtor)};
    env.stubs[9] = {F(0x409C10, &FkSetDeletable)};
    const char* dllReal[kFunctions] = {
        "movimientos de memoria en el orden del EXE (lista, nodo y recuento); sin colaboradores",
        "tres palabras a cero; sin colaboradores",
        "lectura del recuento; sin colaboradores",
        "la secuencia propia: matriz temporal en la pila sin constructor, llamadas al producto, a operator= y al destructor (falsos identicos con hook)",
        "toda la aritmetica x87 (reciproco con exponente ampliado para z y guardado como float para y, x; escrituras tras todas las lecturas); sin colaboradores",
        "lectura de int16, conversion y escritura por componente en el orden del EXE; sin colaboradores",
        "copia parcial con los anchos y el orden del EXE (dwords, words y bytes); sin colaboradores",
        "el bucle propio (recuento con signo, direccion de 32 bits); el constructor es una funcion falsa pasada como argumento",
        "toda la aritmetica x87 (fld + fsub + fstp por componente, memoria viva); sin colaboradores",
        "la lectura de la tabla de modelos 0xA9B0C8 (memoria real) y el TXD de 16 bits con signo; SetModelIsDeletable 0x409C10 es un falso identico con hook"};
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

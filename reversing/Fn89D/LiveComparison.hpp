#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <new>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Font.h"
#include "CutsceneMgr.h"
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn89D dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion (hook S): CFont::PrintSegment 0x719B40 (cdecl: left, top, begin, end, spacing en pila, los limpia el llamador; void). Se llama a si misma (sombra y contorno recursivos): en el lado original recursa el EXE,
// en el candidato la DLL. Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (los dos tienen hook RH, apagado durante el banco): CFont::RenderFontBuffer 0x719840
// (registra pEmptyChar, ejecuta la mutacion del caso y lo reinicia a 0xC716B0 como el original) y CFont::ParseToken 0x718F00 (registra texto, color, blip y etiqueta; avanza, escribe el color y el simbolo segun el caso).
// Memoria real del juego (se guarda y se restaura tras cada lado): pEmptyChar, el buffer y los estaticos de CFont (0xC716A8..0xC71AD0) y los enteros 0xC17044 / 0xC17048. El buffer empieza relleno de 0xA5.
// Se observa: los eventos de los falsos con todos sus argumentos, los bytes cambiados de toda la imagen (en tramos) y los de la arena del texto. Mutaciones (una vez, en el primer vaciado): 1 m_Color, 2 color de sombra,
// 3 sombra, 4 grosor del contorno, 5 inclinacion, 6 ancho de pantalla (0xC17044): el EXE los lee despues del vaciado.

namespace F89DLayout {
static_assert(sizeof(CFontChar) == 0x30, "CFontChar mide 0x30");
}

namespace F89DLive {
using namespace F89DFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x719B40};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint16& G16(uintptr_t a) { return *reinterpret_cast<uint16*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arena del texto
template<size_t WIN> struct Arena {
    static constexpr size_t N = F89DFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kTextWin> textA;

struct State { const C* c{}; bool mutDone[8]{}; unsigned parseCalls{}; };
inline State g;
inline bool Once(u32 site) { if (g.c->mutSite != site || g.mutDone[site]) return false; g.mutDone[site] = true; return true; }
// Los punteros a la pila (temporales del EXE o de la DLL) no son comparables entre lados: se etiquetan todos igual
inline u32 TagS(uintptr_t v) {
    const u32 t = Tag(v);
    const uintptr_t lo = __readfsdword(8), hi = __readfsdword(4);
    if (t == u32(v) && v >= lo && v < hi) return 0x7F000000u;
    return t;
}

// Eventos: 1 RenderFontBuffer [pEmptyChar]; 2 ParseToken [texto, color (puntero), color (contenido), blip, etiqueta].
inline void __cdecl FkFlush() {                                                                     // 0x719840
    Ev& e = PushEv(1, 1); e.v[0] = G32(0xC716A8);
    if (Once(1)) G32(kStatBase + Off::Color) = g.c->mutVal;
    if (Once(2)) G32(kStatBase + Off::Drop) = g.c->mutVal;
    if (Once(3)) G8(kStatBase + Off::Shadow) = uint8(g.c->mutVal);
    if (Once(4)) G8(kStatBase + Off::OutlineSize) = uint8(g.c->mutVal);
    if (Once(5)) G32(kStatBase + Off::Slant) = g.c->mutVal;
    if (Once(6)) G32(0xC17044) = g.c->mutVal;
    G32(0xC716A8) = uint32(kBufStart);
}
inline char* __cdecl FkParse(char* text, uint32* color, uint32 isBlip, char* tag) {                  // 0x718F00
    Ev& e = PushEv(2, 5); e.v[0] = Tag(text); e.v[1] = TagS(uintptr_t(color)); e.v[2] = *color; e.v[3] = isBlip & 0xFFu; e.v[4] = Tag(tag);
    const unsigned k = g.parseCalls < 3 ? g.parseCalls : 3;
    ++g.parseCalls;
    if (g.c->tmode[k] & 1u) *color = g.c->tcolor[k];
    if (g.c->tsym[k] != 0xFFu) G8(kStatBase + Off::PS2Symbol) = uint8(g.c->tsym[k]);
    return text + g.c->tadv[k];
}

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

// Tramos de bytes cambiados respecto a la imagen inicial: [[desplazamiento desde 0xC716A8, "hex"]] (tramos contiguos)
inline json ImageRuns(const std::array<uint8, kImgSize>& init) {
    json out = json::array();
    const uint8* now = reinterpret_cast<const uint8*>(kImgBase);
    size_t i = 0;
    while (i < kImgSize) {
        if (now[i] == init[i]) { ++i; continue; }
        size_t j = i;
        while (j < kImgSize && now[j] != init[j]) ++j;
        out.push_back(json::array({i, HexBytes(now + i, j - i)}));
        i = j;
    }
    return out;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    // el texto (hex) pasa a bytes
    std::vector<uint8> text(c.text.size() / 2);
    for (size_t i = 0; i < text.size(); ++i) text[i] = uint8(std::stoul(c.text.substr(i * 2, 2), nullptr, 16));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        textA.Fill(c.seed * 13u + 50u);
        memcpy(textA.m.data() + kGuard, text.data(), text.size());
        TagClear();
        TagAdd(textA.m.data(), textA.m.size());        // 0
        // memoria del juego: pEmptyChar, hueco y buffer rellenos de 0xA5, estaticos de CFont y enteros
        G32(0xC716A8) = c.pe;
        memset(reinterpret_cast<void*>(kImgBase + 4), kFill, kStatBase - kImgBase - 4);
        memcpy(reinterpret_cast<void*>(kStatBase), c.stat, kStatSize);
        G32(0xC17044) = c.maxw; G32(0xC17048) = c.maxh;
        std::array<uint8, kImgSize> init{};
        memcpy(init.data(), reinterpret_cast<void*>(kImgBase), kImgSize);
        textA.Snap();
        const uintptr_t begin = textA.Obj(c.boff), end = begin + uintptr_t(int32(c.len));
        const std::vector<u32> args = { c.left, c.top, u32(begin), u32(end), c.spacing };
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, c.ecx, c.edx, c.stack, 20); });
        json obs;
        obs["image"] = ImageRuns(init);
        obs["text_changes"] = textA.Changes();
        obs["events"] = EvsJson();
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
        for (const auto& s : env.stubs[f]) {
            if (s.pair.hooked) ready &= s.pair.dll != 0 && ModuleOf(s.pair.dll) == env.asi;
            else ready &= s.pair.dll == 0 || ModuleOf(s.pair.dll) == env.asi;
        }
    }
    for (const auto& s : env.dllOnly) ready &= s.pair.dll != 0 && ModuleOf(s.pair.dll) == env.asi;
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
    if (!std::filesystem::exists(dir / "fn89d-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn89d-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn89d"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de la funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name;
        const bool found = FindHook(kEntries[f], cat, name);
        if (found) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    auto F = [&](uintptr_t exe, auto* fake) { return S(MakeCollab(env, exe, hooksFound), reinterpret_cast<void*>(fake)); };
    env.stubs[0] = {F(0x719840, &FkFlush), F(0x718F00, &FkParse)};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"comparacion de textura, desplazamientos de sombra y contorno (x87 de 24 bits), recursion, limite del buffer, cabecera de 0x30 bytes, copia del texto y alineacion"})},
                                 {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}

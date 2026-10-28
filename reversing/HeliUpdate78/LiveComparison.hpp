#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <new>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Heli.h"
#include "Entity/Ped/Ped.h"
#include "Core/Matrix.h"
#include "Timer.h"
#include "World.h"
#include "Wanted.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par HeliUpdate78 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CHeli::UpdateHelis (0x6C79A0, hook S, cdecl sin argumentos, sin resultado). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL
// (con hook RH, apagado durante el banco): CWorld::FindPlayerWanted 0x56E230, CWanted::NumOfHelisRequired 0x561FA0, CCullZones::PlayerNoRain 0x72DDC0, CWorld::FindPlayerPed 0x56E210,
// CHeli::GenerateHeli 0x6C6520, CEntity::RegisterReference 0x571B70, CWorld::FindPlayerCoors 0x56E010 y CWorld::Remove 0x563280; el destructor virtual (ranura 0 de la vtabla de cada arena de
// helicoptero) apunta a un falso. Los globales (temporizador, pHelis, clima, banderas) son los reales del juego: se escriben antes de cada lado y se restauran despues.
// Corren de verdad en ambos lados: nada mas (el resto es la logica de la funcion). El orden de las llamadas se registra en los eventos.

namespace Hu78Layout {
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(sizeof(CHeli) == Hu78Fix::kHeliWin, "la arena del helicoptero es un CHeli completo");
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCarMission) == 0x3BA, "CAutoPilot: mision en +0x3BA");
static_assert(offsetof(CHeli, m_fMaxAltitude) == 0x9A8 && offsetof(CHeli, m_fMinAltitude) == 0x9B0, "CHeli: alturas en +0x9A8/+0x9B0");
static_assert(offsetof(CEntity, m_nModelIndex) == 0x22, "modelo en +0x22");
inline bool LayoutOK() {
    alignas(16) static std::array<uint8, 0xA40> mem{};
    bool ok = true;
    mem.fill(0);
    auto* m = reinterpret_cast<CMatrix*>(mem.data());
    ok &= uintptr_t(&m->GetPosition().x) == uintptr_t(mem.data()) + 0x30;
    ok &= uintptr_t(CHeli::pHelis.data()) == 0xC1C964 && uintptr_t(&CHeli::TestForNewRandomHelisTimer) == 0xC1C960 && uintptr_t(&CHeli::NumberOfSearchLights) == 0xC1C96C;
    ok &= uintptr_t(&CHeli::bPoliceHelisAllowed) == 0x8D338C && uintptr_t(&CWanted::UseNewsHeliInAdditionToPolice) == 0xB7CB8C;
    return ok;
}
}

namespace Hu78Live {
using namespace Hu78Fix;
inline constexpr uintptr_t kEntry = 0x6C79A0;
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }

// ================================================================================================ arenas
template<size_t N, size_t G = kGuard> struct Arena {
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj() const { return uintptr_t(m.data()) + G; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + G + off, &v, 4); }
    void Put16(size_t off, u32 v) { const u16 w = u16(v); memcpy(m.data() + G + off, &w, 2); }
    void Put8(size_t off, u32 v) { m[G + off] = u8(v); }
    json Changes() const { return ArenaChanges(m.data(), r.data(), N, G); }
    void Tagged() const { TagAdd(m.data(), N); }
};
inline std::array<Arena<kArena(kHeliWin)>, 3> heli;
inline std::array<Arena<kArena(kMatWin)>, 3> mat;
inline void* fakeVtable[4]{};   // ranura 0: destructor virtual (falso)

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

// ================================================================================================ UpdateHelis (0x6C79A0)
struct State { const C* c{}; };
inline State g;
// Eventos: 1 FindPlayerWanted(id); 2 NumOfHelisRequired(this); 3 PlayerNoRain(); 4 FindPlayerPed(id); 5 GenerateHeli(objetivo, bandera: solo el byte bajo); 6 RegisterReference(this, referencia);
// 7 FindPlayerCoors(id); 8 CWorld::Remove(entidad); 9 destructor virtual (this, flag). Los punteros dentro de una arena o de pHelis se etiquetan.
inline u32 __cdecl FakeWanted(u32 id) { Ev& e = PushEv(1, 1); e.v[0] = id; return g.c->wanted; }
inline i32 __fastcall FakeNumHelis(void* self, void*) { Ev& e = PushEv(2, 1); e.v[0] = Tag(self); return i32(g.c->maxHelis); }
inline u32 __cdecl FakeNoRain() { PushEv(3, 0); return g.c->noRain; }
inline u32 __cdecl FakePed(u32 id) { Ev& e = PushEv(4, 1); e.v[0] = id; return g.c->ped; }
inline u32 __cdecl FakeGenerate(u32 target, u32 flag) { Ev& e = PushEv(5, 2); e.v[0] = target; e.v[1] = flag & 0xFFu; return u32(heli[2].Obj()); }
inline void __fastcall FakeRegister(void* self, void*, void* ref) { Ev& e = PushEv(6, 2); e.v[0] = Tag(self); e.v[1] = Tag(ref); }
inline u32* __cdecl FakeCoors(u32* out, u32 id) { Ev& e = PushEv(7, 1); e.v[0] = id; for (int i = 0; i < 3; ++i) out[i] = g.c->player[i]; return out; }
inline void __cdecl FakeRemove(void* entity) { Ev& e = PushEv(8, 1); e.v[0] = Tag(entity); }
inline void __fastcall FakeDtor(void* self, void*, u32 flag) { Ev& e = PushEv(9, 2); e.v[0] = Tag(self); e.v[1] = flag; }

inline void PutHeli(int i, const C& c) {
    const Slot& s = c.s[i];
    auto& h = heli[i];
    auto& m = mat[i];
    h.Fill(c.seed * 13u + 1u + u32(i)); m.Fill(c.seed * 13u + 4u + u32(i));
    h.Put32(0, u32(uintptr_t(fakeVtable)));
    h.Put32(0x14, s.hasMat ? u32(m.Obj()) : 0u);
    for (int a = 0; a < 3; ++a) { if (s.hasMat) m.Put32(0x30 + 4 * a, s.pos[a]); else h.Put32(4 + 4 * a, s.pos[a]); }
    h.Put16(0x22, s.model);
    h.Put32(0x40, s.flags);
    h.Put8(0x3BA, s.mission);
    h.Put8(0x42B, s.b42b);
    h.Snap(); m.Snap();
}

inline void CaseUpd(Env& env, Totals& t, unsigned n) {
    const C c = MakeC(n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        for (int i = 0; i < 3; ++i) PutHeli(i, c);
        TagClear();
        for (int i = 0; i < 3; ++i) heli[i].Tagged();
        for (int i = 0; i < 3; ++i) mat[i].Tagged();
        TagAdd(reinterpret_cast<void*>(0xC1C964), 8);
        // globales reales del juego (se restauran tras cada lado)
        G32(0xB7CB84) = c.now; G32(0xC1C960) = c.test;
        G32(0xC1C964) = c.s[0].present ? u32(heli[0].Obj()) : 0u; G32(0xC1C968) = c.s[1].present ? u32(heli[1].Obj()) : 0u;
        G32(0xC1C96C) = c.lights;
        G8(0x8D338C) = u8(c.allowed); G32(0xB72914) = c.area; G8(0xB7CB8C) = u8(c.useNews); G8(0x8E73A4) = u8(c.b73a4); G8(0x8E72F0) = u8(c.b72f0);
        G32(0xC81320) = c.wOld; G32(0xC8131C) = c.wNew;
        g.c = &c; nev = 0;
        sides[s] = Execute(env, 0, kEntry, candidate, [&](uintptr_t entry) { return Gate(entry, nullptr, 0, c.eax, c.ecx, c.edx, c.stack, 0); });
        json obs;
        obs["helis"] = json::array({Tag(uintptr_t(G32(0xC1C964))), Tag(uintptr_t(G32(0xC1C968)))});
        obs["timer"] = G32(0xC1C960);
        obs["lights"] = G32(0xC1C96C);
        obs["now"] = G32(0xB7CB84);
        obs["changes"] = json::array();
        for (int i = 0; i < 3; ++i) obs["changes"].push_back(heli[i].Changes());
        for (int i = 0; i < 3; ++i) obs["changes"].push_back(mat[i].Changes());
        obs["events"] = EvsJson();
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[0], n, input, rows, sides);
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
    const bool layout = Hu78Layout::LayoutOK();
    ready &= layout;
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases}, {"layout_ok", layout},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < kCases; ++c) CaseUpd(env, totals, c);
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
// Colaborador con hook: se crea su ruta (el hook queda apagado) y se toma el cuerpo de la DLL; sin hook (dll = 0) la DLL llama a la direccion del EXE.
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
inline bool Prologue(uintptr_t f) { const auto* b = reinterpret_cast<const uint8*>(f); if (b[0] == 0x8B && b[1] == 0xFF) b += 2; return b[0] == 0x55 && b[1] == 0x8B && b[2] == 0xEC; }
// Primera llamada (E8) de `fn` (dentro del ASI) a una funcion del ASI con prologo push ebp / mov ebp,esp: el cuerpo real del constructor que llama la envoltura `Constructor` del hook.
inline uintptr_t FirstDllCall(uintptr_t asi, uintptr_t fn) {
    fn = Body(fn); if (!fn || ModuleOf(fn) != asi) return 0;
    for (uintptr_t p = fn; p < fn + 0x300; ++p) {
        if (*reinterpret_cast<const uint8*>(p) != 0xE8) continue;
        const uintptr_t t = p + 5 + *reinterpret_cast<const int32*>(p + 1);
        if (ModuleOf(t) == asi && Prologue(Body(t))) return Body(t);
    }
    return 0;
}
template<class M> inline uintptr_t MemberBody(M member) { return Body(std::bit_cast<uintptr_t>(member)); }

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "heliupdate78-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "heliupdate78-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("heliupdate78"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de la funcion probada: se construye ANTES que nada; el constructor deja el hook apagado. El cuerpo de la DLL se calcula aqui, antes de crear las rutas de los colaboradores y los parches de entrada.
    {
        std::string cat, name;
        const bool found = FindHook(kEntry, cat, name);
        if (found) env.route[0].reset(new HookRoute(cat.c_str(), name.c_str(), kEntry, HookRoute::RegisteredTarget{}));
        registered[kFn[0]] = found && env.route[0]->Registered(); bodies[kFn[0]] = found && env.route[0]->Body() != 0;
        ready &= found && env.route[0]->Registered() && env.route[0]->Body() && ModuleOf(env.route[0]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[0]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntry}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    fakeVtable[0] = reinterpret_cast<void*>(&FakeDtor);
    const Pair pWanted = MakeCollab(env, 0x56E230, hooksFound), pNum = MakeCollab(env, 0x561FA0, hooksFound), pRain = MakeCollab(env, 0x72DDC0, hooksFound), pPed = MakeCollab(env, 0x56E210, hooksFound);
    const Pair pGen = MakeCollab(env, 0x6C6520, hooksFound), pReg = MakeCollab(env, 0x571B70, hooksFound), pCoors = MakeCollab(env, 0x56E010, hooksFound), pRemove = MakeCollab(env, 0x563280, hooksFound);
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    env.stubs[0] = {S(pWanted, reinterpret_cast<void*>(&FakeWanted)), S(pNum, reinterpret_cast<void*>(&FakeNumHelis)), S(pRain, reinterpret_cast<void*>(&FakeNoRain)), S(pPed, reinterpret_cast<void*>(&FakePed)),
        S(pGen, reinterpret_cast<void*>(&FakeGenerate)), S(pReg, reinterpret_cast<void*>(&FakeRegister)), S(pCoors, reinterpret_cast<void*>(&FakeCoors)), S(pRemove, reinterpret_cast<void*>(&FakeRemove))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"ninguno: todo lo demas es la logica de la funcion"})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry, 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}

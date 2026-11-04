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
#include "Camera.h"
#include "Cam.h"
#include "World.h"
#include "Pad.h"
#include "Entity/Ped/Ped.h"
#include "CutsceneMgr.h"
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn89A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion (hook S): CCamera::IsItTimeForNewCamera 0x51D770 (thiscall, ECX = la camara; camSequence y startTime en pila, `ret 8`; devuelve AL). Colaboradores sustituidos por falsos identicos en la direccion del EXE
// y en el cuerpo de la DLL (los de hook RH, apagado durante el banco; RwCameraSetNearClipPlane 0x7EE1D0 no tiene hook y la DLL lo llama por su direccion): CWorld::GetIsLineOfSightClear 0x56A490, FindPlayerVehicle 0x56E0D0,
// FindPlayerCoors 0x56E010, FindPlayerSpeed 0x56E090, CEntity::GetIsOnScreen 0x534540 y los siete CCam::Process_DW_* (0x51A740 HeliChase, 0x51B120 CamMan, 0x51B850 Birdy, 0x51C250 PlaneSpotter, 0x51C760 PlaneCam1,
// 0x51CC30 PlaneCam2, 0x51D100 PlaneCam3). Registran sus argumentos EN ORDEN. La aritmetica (transcurrido en 24 bits, palo derecho, vector jugador - origen fijo, modulo, producto escalar con la velocidad) corre de verdad
// en ambos lados (CVector::Magnitude 0x4082C0 y operator- 0x40FE60 son codigo real del EXE; la DLL usa los ayudantes de X87Math). Globales reales (se guardan y restauran): reloj 0xB7CB84, aviso 0x8CCDF4, paso 0x8CC471,
// 0xB6F059 y el eje X del palo derecho del pad 0 (0xB7345C). La camara, el objetivo, el vehiculo del jugador, la matriz y la velocidad son arenas.
// Mutaciones (una sola vez, dentro de un falso): 1 (LOS) origen fijo.x de la camara; 2 (1a FindPlayerCoors) las coordenadas que devuelven las llamadas siguientes (x); 3 (LOS) velocidad.y; 4 (1a FindPlayerVehicle) tipo del vehiculo.

namespace F89ALive {
using namespace F89AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x51D770};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint16& G16(uintptr_t a) { return *reinterpret_cast<uint16*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { const uint16 w = uint16(v); memcpy(m.data() + kGuard + off, &w, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            if (m[i] != r[i]) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kCamArena> camA;
inline Arena<kEntArena> entA;
inline Arena<kVehArena> vehA;
inline Arena<kMatArena> matA;
inline Arena<kSpdArena> spdA;

// Estado del caso en curso para los falsos
struct State { const C* c{}; bool mutDone[8]{}; unsigned coorsCalls{}, fpvCalls{}; u32 coordX{}; bool coordXSet{}; };
inline State g;
inline bool Once(u32 site) { if (g.c->mutSite != site || g.mutDone[site]) return false; g.mutDone[site] = true; return true; }
// Los punteros a la pila (temporales del EXE o de la DLL) no son comparables entre lados: se etiquetan todos igual
inline u32 TagS(uintptr_t v) {
    const u32 t = Tag(v);
    const uintptr_t lo = __readfsdword(8), hi = __readfsdword(4);
    if (t == u32(v) && v >= lo && v < hi) return 0x7F000000u;
    return t;
}
inline void Vec3(Ev& e, u32 at, const void* p) { memcpy(&e.v[at], p, 12); }

// Eventos: 1 FindPlayerVehicle [id, incluir remoto]; 2 FindPlayerCoors [salida, id]; 3 FindPlayerSpeed [id]; 4 GetIsLineOfSightClear [origen, destino, origen.xyz, destino.xyz, 7 banderas];
// 5 RwCameraSetNearClipPlane [camara, bits del float]; 6 GetIsOnScreen [this]; 10+k Process_DW_* [this, argumento] (k = 0 HeliChase, 1 CamMan, 2 Birdy, 3 PlaneSpotter, 4 PlaneCam1, 5 PlaneCam2, 6 PlaneCam3).
inline u32 __cdecl FkFpv(u32 id, u32 remote) {                                                       // 0x56E0D0
    Ev& e = PushEv(1, 2); e.v[0] = id; e.v[1] = remote;
    if (g.fpvCalls++ == 0 && Once(4)) vehA.Put32(0x590, g.c->mutVal % 9);
    return g.c->hasVeh ? u32(vehA.Obj()) : 0u;
}
inline u32 __cdecl FkCoors(float* out, u32 id) {                                                    // 0x56E010
    Ev& e = PushEv(2, 2); e.v[0] = TagS(uintptr_t(out)); e.v[1] = id;
    u32 x = g.c->pcoords[0];
    if (g.coordXSet) x = g.coordX;
    memcpy(out, &x, 4); memcpy(reinterpret_cast<u8*>(out) + 4, &g.c->pcoords[1], 4); memcpy(reinterpret_cast<u8*>(out) + 8, &g.c->pcoords[2], 4);
    if (g.coorsCalls++ == 0 && Once(2)) { g.coordX = g.c->mutVal; g.coordXSet = true; }
    return u32(uintptr_t(out));
}
inline u32 __cdecl FkSpeed(u32 id) {                                                                // 0x56E090
    Ev& e = PushEv(3, 1); e.v[0] = id;
    return u32(spdA.Obj(kSpdOff));
}
inline u32 __cdecl FkLos(const float* a, const float* b, u32 f0, u32 f1, u32 f2, u32 f3, u32 f4, u32 f5, u32 f6) {   // 0x56A490
    Ev& e = PushEv(4, 15); e.v[0] = TagS(uintptr_t(a)); e.v[1] = TagS(uintptr_t(b)); Vec3(e, 2, a); Vec3(e, 5, b);
    e.v[8] = f0; e.v[9] = f1; e.v[10] = f2; e.v[11] = f3; e.v[12] = f4; e.v[13] = f5; e.v[14] = f6;
    if (Once(1)) camA.Put32(0x86C, g.c->mutVal);
    if (Once(3)) spdA.Put32(kSpdOff + 4, g.c->mutVal);
    return g.c->los;
}
inline u32 __cdecl FkNear(u32 cam, u32 bits) {                                                      // 0x7EE1D0
    Ev& e = PushEv(5, 2); e.v[0] = cam == G32(0xC1703C) ? 0xCAFE0001u : cam; e.v[1] = bits;   // el RwCamera* real del juego se normaliza
    return cam;
}
inline u32 __fastcall FkOnScreen(void* self, void*) {                                               // 0x534540
    Ev& e = PushEv(6, 1); e.v[0] = Tag(self);
    return g.c->onscreen;
}
template<u32 K> inline u32 __fastcall FkDw(void* self, void*, u32 arg) {                            // Process_DW_* (ret 4)
    Ev& e = PushEv(10 + K, 2); e.v[0] = Tag(self); e.v[1] = arg;
    return g.c->dw;
}

inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    obs["globals"] = json::array({G8(0x8CCDF4), G8(0x8CC471), G8(0xB6F059)});
    obs["globals_hash"] = RegionsHash();
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

inline void SetupArenas(const C& c) {
    camA.Fill(c.seed * 13u + 50u); entA.Fill(c.seed * 13u + 51u); vehA.Fill(c.seed * 13u + 52u);
    matA.Fill(c.seed * 13u + 53u); spdA.Fill(c.seed * 13u + 54u);
}
inline void SnapArenas() { camA.Snap(); entA.Snap(); vehA.Snap(); matA.Snap(); spdA.Snap(); }
inline void Put3(auto& a, size_t off, const u32* p) { a.Put32(off, p[0]); a.Put32(off + 4, p[1]); a.Put32(off + 8, p[2]); }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        SetupArenas(c);
        TagClear();
        TagAdd(camA.m.data(), camA.m.size());          // 0
        TagAdd(entA.m.data(), entA.m.size());          // 1
        TagAdd(vehA.m.data(), vehA.m.size());          // 2
        TagAdd(matA.m.data(), matA.m.size());          // 3
        TagAdd(spdA.m.data(), spdA.m.size());          // 4
        // camara: cam activa (+0x59), origen fijo (+0x86C), objetivo (+0x958) y origen de la camara activa (+0x310 + activa * 0x238)
        camA.Put8(0x59, c.active);
        camA.Put32(0x138, c.i138); camA.Put8(0x31, c.i31);
        Put3(camA, 0x86C, c.fixedSrc);
        camA.Put32(0x958, u32(entA.Obj()));
        Put3(camA, 0x310 + c.active * 0x238, c.csrc);
        // objetivo: modelo (+0x22), matriz (+0x14), posicion en la ubicacion activa (matriz +0x30 o compacta +4) y en la otra
        entA.Put16(0x22, c.tgtModel);
        entA.Put32(0x14, c.tmat ? u32(matA.Obj()) : 0u);
        if (c.tmat) { Put3(matA, 0x30, c.tpos); Put3(entA, 4, c.talt); }
        else { Put3(entA, 4, c.tpos); Put3(matA, 0x30, c.talt); }
        // vehiculo del jugador
        vehA.Put16(0x22, c.vehModel);
        vehA.Put32(0x590, c.vehType);
        Put3(spdA, kSpdOff, c.speed);
        G32(0xB7CB84) = c.now;
        G8(0x8CCDF4) = uint8(c.alert); G8(0x8CC471) = uint8(c.step); G8(0xB6F059) = uint8(c.gate);
        G16(0xB7345C) = uint16(c.stick);
        const std::vector<u32> args = { c.seq, c.start };
        SnapArenas();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, u32(camA.Obj()), c.edx, c.stack, 0); });
        json obs;
        obs["ret"] = int(sides[s].reg.eax & 0xFFu);
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, camA.Changes()); add(1, entA.Changes()); add(2, vehA.Changes()); add(3, matA.Changes()); add(4, spdA.Changes());
        obs["changes"] = ch;
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
    if (!std::filesystem::exists(dir / "fn89a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn89a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn89a"); }
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
    env.stubs[0] = {F(0x56E0D0, &FkFpv), F(0x56E010, &FkCoors), F(0x56E090, &FkSpeed), F(0x56A490, &FkLos), F(0x7EE1D0, &FkNear), F(0x534540, &FkOnScreen),
                    F(0x51A740, &FkDw<0>), F(0x51B120, &FkDw<1>), F(0x51B850, &FkDw<2>), F(0x51C250, &FkDw<3>), F(0x51C760, &FkDw<4>), F(0x51CC30, &FkDw<5>), F(0x51D100, &FkDw<6>)};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"transcurrido en 24 bits, palo derecho del pad 0, vector jugador - origen fijo, modulo (CVector::Magnitude y operator- reales en el EXE, ayudantes x87 en la DLL), producto escalar con la velocidad y despacho"})},
                                 {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}

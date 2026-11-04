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
#include "Entity/Ped/Ped.h"
#include "CutsceneMgr.h"
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn89B dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion (hook S): CCamera::AvoidTheGeometry 0x514030 (thiscall, ECX = la camara; source, target y out por puntero y el fov en pila, `ret 10h`; void). Colaboradores sustituidos por falsos identicos en la direccion
// del EXE y en el cuerpo de la DLL (los de hook RH, apagado durante el banco; RwCameraSetNearClipPlane 0x7EE1D0 no tiene hook y la DLL lo llama por su direccion): CWorld::ProcessLineOfSight 0x56BA00 (dos llamadas),
// FindPlayerPed 0x56E210, CWorld::TestSphereAgainstWorld 0x569E20, CPlaceable::GetMatrix 0x411990 y WellBufferMe 0x509AE0. Registran sus argumentos EN ORDEN (los vectores con su contenido en la llamada).
// REALES en ambos lados (la DLL usa los ayudantes de X87Math; el EXE su propio codigo, con el hook RH de la DLL apagado): CGeneral::GetATanOfXY 0x53CC70, CVector::Normalise 0x59C910, fsin/fcos/fptan y _CIpow 0x822130;
// ademas la resta de vectores 0x40FE60 y CrossProduct 0x59C730 (en linea en la DLL). Globales reales (se guardan y restauran): pIgnoreEntity 0xB7CD68, paso de tiempo 0xB7CB5C, 0xB6EC65, WellBufferMe 0xB6EC38/0xB6EC3C,
// proporcion 0xC3EFA4, punto y normal de la esfera 0xB9B250/0xB9B260 y RwCamera.nearPlane (+0x80 del 0xC1703C). La camara, los vectores, el objetivo, su matriz y la del falso GetMatrix son arenas.
// Mutaciones (una sola vez, dentro de un falso): 1 (1a ProcessLineOfSight) target.x; 2 (TestSphereAgainstWorld) out.y; 3 (2a GetMatrix) vector adelante.x.

namespace F89BLive {
using namespace F89BFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x514030};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint16& G16(uintptr_t a) { return *reinterpret_cast<uint16*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }
inline uintptr_t NearAddr() { return G32(0xC1703C) + 0x80; }

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
inline Arena<kSrcArena> srcA;
inline Arena<kTgtArena> tgtA;
inline Arena<kOutArena> outA;
inline Arena<kEntArena> entA;
inline Arena<kMatArena> matA;
inline Arena<kFwdArena> fwdA;
inline Arena<kPedArena> pedA;

// Estado del caso en curso para los falsos
struct State { const C* c{}; bool mutDone[8]{}; unsigned plosCalls{}, matCalls{}; };
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

// Eventos: 1 ProcessLineOfSight [origen, destino, origen.xyz, destino.xyz, punto de choque, entidad, pIgnoreEntity, 8 banderas]; 2 FindPlayerPed [id]; 3 RwCameraSetNearClipPlane [camara, bits del float];
// 4 TestSphereAgainstWorld [centro.xyz, radio, 7 argumentos]; 5 GetMatrix [this]; 6 WellBufferMe [valor, actual, velocidad, 0.2f, 0.05f, bandera].
inline u32 __cdecl FkPlos(const float* a, const float* b, float* cp, void* hit, u32 f0, u32 f1, u32 f2, u32 f3, u32 f4, u32 f5, u32 f6, u32 f7) {   // 0x56BA00
    Ev& e = PushEv(1, 19); e.v[0] = TagS(uintptr_t(a)); e.v[1] = TagS(uintptr_t(b)); Vec3(e, 2, a); Vec3(e, 5, b); e.v[8] = TagS(uintptr_t(cp)); e.v[9] = TagS(uintptr_t(hit)); e.v[10] = Tag(uintptr_t(G32(0xB7CD68)));
    e.v[11] = f0; e.v[12] = f1; e.v[13] = f2; e.v[14] = f3; e.v[15] = f4; e.v[16] = f5; e.v[17] = f6; e.v[18] = f7;
    const unsigned k = g.plosCalls++;
    const u32 res = k == 0 ? g.c->los1 : g.c->los2;
    if (res) memcpy(cp, k == 0 ? g.c->hit1 : g.c->hit2, 12);
    if (k == 0 && Once(1)) tgtA.Put32(kVecOff, g.c->mutVal);
    return res;
}
inline u32 __cdecl FkFpp(u32 id) {                                                                  // 0x56E210
    Ev& e = PushEv(2, 1); e.v[0] = id;
    return g.c->hasPed ? u32(pedA.Obj()) : 0u;
}
inline u32 __cdecl FkNear(u32 cam, u32 bits) {                                                      // 0x7EE1D0
    Ev& e = PushEv(3, 2); e.v[0] = cam == G32(0xC1703C) ? 0xCAFE0001u : cam; e.v[1] = bits;   // el RwCamera* real del juego se normaliza
    if (cam == G32(0xC1703C)) G32(cam + 0x80) = bits;                                           // como el original: el plano cercano queda cambiado
    return cam;
}
inline u32 __cdecl FkSphere(u32 cx, u32 cy, u32 cz, u32 radius, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9) {   // 0x569E20 (el centro va por valor)
    Ev& e = PushEv(4, 11); e.v[0] = cx; e.v[1] = cy; e.v[2] = cz; e.v[3] = radius; e.v[4] = a3; e.v[5] = a4; e.v[6] = a5; e.v[7] = a6; e.v[8] = a7; e.v[9] = a8; e.v[10] = a9;
    if (g.c->sphere) { memcpy(reinterpret_cast<void*>(0xB9B250), g.c->spt, 12); memcpy(reinterpret_cast<void*>(0xB9B260), g.c->snorm, 12); }
    if (Once(2)) outA.Put32(kVecOff + 4, g.c->mutVal);
    return g.c->sphere ? u32(entA.Obj()) : 0u;
}
inline u32 __fastcall FkGetMatrix(void* self, void*) {                                              // 0x411990
    Ev& e = PushEv(5, 1); e.v[0] = Tag(self);
    if (++g.matCalls == 2 && Once(3)) fwdA.Put32(0x10, g.c->mutVal);
    return u32(fwdA.Obj());
}
inline void __cdecl FkWell(u32 value, u32 p0, u32 p1, u32 a, u32 b, u32 flag) {                     // 0x509AE0
    Ev& e = PushEv(6, 6); e.v[0] = value; e.v[1] = p0; e.v[2] = p1; e.v[3] = a; e.v[4] = b; e.v[5] = flag;
    G32(p0) = g.c->wbOutCur; G32(p1) = g.c->wbOutSpd;
}

inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    obs["globals"] = json::array({G32(0xB7CD68), G32(NearAddr()), G32(0xB6EC38), G32(0xB6EC3C), G32(0xB9B250), G32(0xB9B254), G32(0xB9B258), G32(0xB9B260), G32(0xB9B264), G32(0xB9B268)});
    obs["globals_hash"] = RegionsHash();
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

inline void SetupArenas(const C& c) {
    camA.Fill(c.seed * 13u + 50u); srcA.Fill(c.seed * 13u + 51u); tgtA.Fill(c.seed * 13u + 52u); outA.Fill(c.seed * 13u + 53u);
    entA.Fill(c.seed * 13u + 54u); matA.Fill(c.seed * 13u + 55u); fwdA.Fill(c.seed * 13u + 56u); pedA.Fill(c.seed * 13u + 57u);
}
inline void SnapArenas() { camA.Snap(); srcA.Snap(); tgtA.Snap(); outA.Snap(); entA.Snap(); matA.Snap(); fwdA.Snap(); pedA.Snap(); }
inline void Put3(auto& a, size_t off, const u32* p) { a.Put32(off, p[0]); a.Put32(off + 4, p[1]); a.Put32(off + 8, p[2]); }

// Colaboradores reales del EXE (con hook RH en la DLL, apagado): sus entradas deben seguir siendo codigo del EXE
inline std::vector<uintptr_t> realEntries;

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
        TagAdd(srcA.m.data(), srcA.m.size());          // 1
        TagAdd(tgtA.m.data(), tgtA.m.size());          // 2
        TagAdd(outA.m.data(), outA.m.size());          // 3
        TagAdd(entA.m.data(), entA.m.size());          // 4
        TagAdd(matA.m.data(), matA.m.size());          // 5
        TagAdd(fwdA.m.data(), fwdA.m.size());          // 6
        TagAdd(pedA.m.data(), pedA.m.size());          // 7
        // camara: rumbo de reserva (+0x984, +0x988), temporizador (+0x164), sentido (+0x168), +0x2D y objetivo (+0x958)
        camA.Put32(0x984, c.head984); camA.Put32(0x988, c.head988);
        camA.Put32(0x164, c.timer); camA.Put16(0x168, c.dirn); camA.Put8(0x2D, c.i2d);
        camA.Put32(0x958, c.tgtNull ? 0u : u32(entA.Obj()));
        Put3(srcA, kVecOff, c.src); Put3(tgtA, kVecOff, c.tgt); Put3(outA, kVecOff, c.outInit);
        // objetivo: tipo de entidad (+0x36), matriz (+0x14), posicion en la ubicacion activa (matriz +0x30 o compacta +4) y en la otra
        entA.Put8(0x36, c.isPed);
        entA.Put32(0x14, c.tmat ? u32(matA.Obj()) : 0u);
        if (c.tmat) { Put3(matA, 0x30, c.tpos); Put3(entA, 4, c.talt); }
        else { Put3(entA, 4, c.tpos); Put3(matA, 0x30, c.talt); }
        Put3(fwdA, 0x10, c.fwd);
        // globales
        G32(0xB7CD68) = 0x11111111u; G32(0xB7CB5C) = c.timestep; G8(0xB6EC65) = uint8(c.b65);
        G32(0xB6EC38) = c.wbCur; G32(0xB6EC3C) = c.wbSpd; G32(0xC3EFA4) = c.aspect; G32(NearAddr()) = c.nearPlane;
        FillWords(reinterpret_cast<u8*>(0xB9B250), 0x20, c.seed + 1u);
        const bool realOK = std::ranges::none_of(realEntries, [](uintptr_t a) { return ReversingProbe::JumpsToDll(a); });
        const std::vector<u32> args = { u32(srcA.Obj(kVecOff)), u32(tgtA.Obj(kVecOff)), u32(outA.Obj(kVecOff)), c.fov };
        SnapArenas();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, u32(camA.Obj()), c.edx, c.stack, 0); });
        json obs;
        obs["real_exe"] = realOK;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, camA.Changes()); add(1, srcA.Changes()); add(2, tgtA.Changes()); add(3, outA.Changes()); add(4, entA.Changes()); add(5, matA.Changes()); add(6, fwdA.Changes()); add(7, pedA.Changes());
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
    for (uintptr_t a : realEntries) ready &= !ReversingProbe::JumpsToDll(a) && ModuleOf(Resolve(a)) == env.game;
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
// Colaborador REAL: solo se apaga su hook RH (si existe) para que el EXE ejecute su propio codigo.
inline void MakeReal(Env& env, uintptr_t exe, json& hooksFound) {
    std::string cat, name;
    if (FindHook(exe, cat, name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}, {"real", true}});
    }
    realEntries.push_back(exe);
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn89b-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn89b-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn89b"); }
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
    env.stubs[0] = {F(0x56BA00, &FkPlos), F(0x56E210, &FkFpp), F(0x7EE1D0, &FkNear), F(0x569E20, &FkSphere), F(0x411990, &FkGetMatrix), F(0x509AE0, &FkWell)};
    MakeReal(env, 0x53CC70, hooksFound);
    MakeReal(env, 0x59C910, hooksFound);
    MakeReal(env, 0x40FE60, hooksFound);
    MakeReal(env, 0x59C730, hooksFound);
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        json real = json::array(); for (uintptr_t a : realEntries) { env.watches.emplace_back(a, 16); real.push_back(a); }
        collaborators[kFn[f]] = {{"dll_real", json::array({"rumbo y paso con GetATanOfXY, direccion con fsin/fcos y Normalise, radio con fptan y la proporcion, proyeccion y normal de la esfera, temporizador con _CIpow, reescalado por WellBufferMe"})},
                                 {"stubbed", stubbed}, {"real_exe", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}

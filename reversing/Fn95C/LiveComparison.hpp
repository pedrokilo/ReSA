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
#include "Entity/Ped/Ped.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn95C dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Funcion 0 (hook S): GetFirstTextureCallback 0x734930 (cdecl; *dato = textura; devuelve nulo). Sin colaboradores.
// Funcion 1 (hook S): RwTexDictionaryGtaStreamRead1 0x731070 (cdecl, stream). Falsos: RwStreamFindChunk 0x7ED2D0, RwStreamRead 0x7EC9D0, RwTexDictionaryCreate 0x7F3600, RwTextureGtaStreamRead 0x730E60 (hook RH en la DLL),
//   RwTexDictionaryAddTexture 0x7F3980, RwTexDictionaryForAllTextures 0x7F3730 y RwTexDictionaryDestroy 0x7F36A0.
// Funcion 2 (hook S): RwTexDictionaryGtaStreamRead2 0x731150 (cdecl, stream, txd). Falsos: RwStreamSkip 0x7ECD00, 0x730E60, AddTexture, ForAllTextures y Destroy.
// Funcion 3 (hook S): RpClumpGtaStreamRead1 0x72E570 (cdecl, stream; AL). Falsos: FindChunk, Read, _rwFrameListStreamRead 0x807970, _rwFrameListDeinitialize 0x807720 (hook RH), CMemoryMgr::Malloc 0x72F420 (hook RH) y
//   RpGeometryStreamRead 0x74D190. Los cuatro ayudantes de registros del EXE (0x72E140, 0x72E220, 0x72E270 y 0x72E360) corren de verdad en el EXE; la DLL los tiene en C++ (privados): se validan a traves de estas tres funciones.
// Funcion 4 (hook S): RpClumpGtaStreamRead2 0x72E620 (cdecl, stream). Falsos: RpClumpCreate 0x74A290, Skip, FindChunk, GeometryStreamRead, Read, RpAtomicCreate 0x749C50, RpAtomicSetFrame 0x74BF20, RpAtomicSetGeometry 0x749D40,
//   RpAtomicDestroy 0x749DC0, RpGeometryDestroy 0x74CCC0, RpClumpAddAtomic 0x74A490, RpClumpDestroy 0x74A310, Deinitialize y CMemoryMgr::Free 0x72F430.
// Funcion 5 (hook S): RpClumpGtaCancelStream 0x72E700 (cdecl, sin argumentos). Falsos: RpGeometryDestroy, Free y Deinitialize.
// Los falsos son identicos en la direccion del EXE y (si el colaborador tiene hook RH, apagado durante el banco) en el cuerpo de la DLL; registran sus argumentos EN ORDEN. Memoria real del juego (se rellena y se restaura tras
// cada lado): los globales 0xC87AE8..0xC87B07, 0x8D607C, 0x8D6088 y 0xC87FE8. El stream es una arena cuya posicion (+0xC) avanzan los falsos.

namespace F95CLive {
using namespace F95CFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x734930, 0x731070, 0x731150, 0x72E570, 0x72E620, 0x72E700};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F95CFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kStreamWin> sA;
inline Arena<kClumpWin> cA;
inline std::array<Arena<kAtomWin>, kAtoms> aA;
inline Arena<kFramesWin> faA;
inline Arena<kGeomWin> gaA;
inline Arena<kDataWin> dA;
// etiquetas de arenas (orden de TagAdd en CaseRun): 0 stream, 1 clump, 2..9 atomicos, 10 vector de frames, 11 vector de geometrias, 12 dato

inline constexpr uintptr_t kFlAddr = 0xC87AF4;
struct State { const C* c{}; unsigned fc{}, rd{}, tx{}, ge{}, at{}; };
inline State g;
inline u32 TagS(uintptr_t v) {   // los punteros a la pila (locales del EXE o de la DLL) no son comparables entre lados: se etiquetan todos igual
    const u32 t = Tag(v);
    const uintptr_t lo = __readfsdword(8), hi = __readfsdword(4);
    if (t == u32(v) && v >= lo && v < hi) return 0x7F000000u;
    return t;
}
inline u32& StreamPos(void* stream) { return *reinterpret_cast<u32*>(uintptr_t(stream) + 0xC); }

// Eventos: 1 FindChunk [stream, tipo, hay_longitud, hay_version]; 2 Read [stream, buffer, longitud]; 3 Skip [stream, desplazamiento]; 4 TexDictionaryCreate []; 5 RwTextureGtaStreamRead [stream];
// 6 AddTexture [txd, textura]; 7 ForAllTextures [txd, callback, dato]; 8 TexDictionaryDestroy [txd]; 9 Malloc [tamano, hint]; 10 Free [puntero]; 11 RpGeometryStreamRead [stream]; 12 RpGeometryDestroy [geometria];
// 13 RpAtomicCreate []; 14 RpAtomicSetFrame [atomico, frame]; 15 RpAtomicSetGeometry [atomico, geometria, flags]; 16 RpAtomicDestroy [atomico]; 17 RpClumpCreate []; 18 RpClumpAddAtomic [clump, atomico];
// 19 RpClumpDestroy [clump]; 20 _rwFrameListStreamRead [stream, lista]; 21 _rwFrameListDeinitialize [lista].
inline i32 __cdecl FkFind(void* stream, u32 type, u32* len, u32* ver) {          // 0x7ED2D0
    const u32 k = g.fc++;
    Ev& e = PushEv(1, 4); e.v[0] = Tag(stream); e.v[1] = type; e.v[2] = len != nullptr; e.v[3] = ver != nullptr;
    StreamPos(stream) += g.c->step[0];
    if (len) *len = k < kFc ? g.c->fcLen[k] : 0u;
    if (ver) *ver = k < kFc ? g.c->fcVer[k] : 0u;
    return i32(k < kFc ? g.c->fcVal[k] : 0u);
}
inline u32 __cdecl FkRead(void* stream, void* buf, u32 len) {                    // 0x7EC9D0
    const u32 k = g.rd++;
    Ev& e = PushEv(2, 3); e.v[0] = Tag(stream); e.v[1] = TagS(uintptr_t(buf)); e.v[2] = len;
    StreamPos(stream) += g.c->step[1];
    const u32 mode = k < kRd ? g.c->rdMode[k] : 0u;
    u32 pay[4]{};
    if (k < kRd) memcpy(pay, g.c->rdPay[k], 16);
    memcpy(buf, pay, len < 16 ? len : 16);
    switch (mode) {
    case 1: return len - 1;
    case 2: return len + 1;
    case 3: return 0;
    case 4: return 0xFFFFFFFFu;
    case 5: return pay[1];
    default: return len;
    }
}
inline void* __cdecl FkSkip(void* stream, u32 off) { Ev& e = PushEv(3, 2); e.v[0] = Tag(stream); e.v[1] = off; return stream; }   // 0x7ECD00
inline u32 __cdecl FkCreate() { PushEv(4, 0); return g.c->createVal; }                                                          // 0x7F3600
inline u32 __cdecl FkTexRead(void* stream) {                                                                                   // 0x730E60
    const u32 k = g.tx++;
    Ev& e = PushEv(5, 1); e.v[0] = Tag(stream);
    StreamPos(stream) += g.c->step[2];
    if (k < 32 && ((g.c->pokeMask >> k) & 1u)) G32(kTexLeft) = g.c->pokeVal[k < kTex ? k : 0];
    return k < kTex ? g.c->tex[k] : 0u;
}
inline u32 __cdecl FkAddTex(u32 txd, u32 tex) { Ev& e = PushEv(6, 2); e.v[0] = txd; e.v[1] = tex; return tex; }                  // 0x7F3980
inline u32 __cdecl FkForAll(u32 txd, u32 cb, u32 data) { Ev& e = PushEv(7, 3); e.v[0] = txd; e.v[1] = cb; e.v[2] = data; return txd; }   // 0x7F3730
inline u32 __cdecl FkTxdDestroy(u32 txd) { Ev& e = PushEv(8, 1); e.v[0] = txd; return 1; }                                      // 0x7F36A0
inline void* __cdecl FkMalloc(u32 size, u32 hint) {                                                                            // 0x72F420
    Ev& e = PushEv(9, 2); e.v[0] = size; e.v[1] = hint;
    return g.c->mallocNull ? nullptr : reinterpret_cast<void*>(gaA.Obj());
}
inline void __cdecl FkFree(void* p) { Ev& e = PushEv(10, 1); e.v[0] = Tag(p); }                                                 // 0x72F430
inline u32 __cdecl FkGeoRead(void* stream) {                                                                                   // 0x74D190
    const u32 k = g.ge++;
    Ev& e = PushEv(11, 1); e.v[0] = Tag(stream);
    StreamPos(stream) += g.c->step[3];
    return k < kGeo ? g.c->geo[k] : 0u;
}
inline u32 __cdecl FkGeoDestroy(u32 geo) { Ev& e = PushEv(12, 1); e.v[0] = geo; return 1; }                                     // 0x74CCC0
inline void* __cdecl FkAtomCreate() {                                                                                          // 0x749C50
    const u32 k = g.at++;
    PushEv(13, 0);
    if (k >= kAtoms || ((g.c->atomNull >> k) & 1u)) return nullptr;
    return reinterpret_cast<void*>(aA[k].Obj());
}
inline void* __cdecl FkSetFrame(void* atom, u32 frame) { Ev& e = PushEv(14, 2); e.v[0] = Tag(atom); e.v[1] = frame; return atom; }   // 0x74BF20
inline void* __cdecl FkSetGeom(void* atom, u32 geo, u32 flags) { Ev& e = PushEv(15, 3); e.v[0] = Tag(atom); e.v[1] = geo; e.v[2] = flags; return atom; }   // 0x749D40
inline u32 __cdecl FkAtomDestroy(void* atom) { Ev& e = PushEv(16, 1); e.v[0] = Tag(atom); return 1; }                          // 0x749DC0
inline void* __cdecl FkClumpCreate() { PushEv(17, 0); return g.c->clumpNull ? nullptr : reinterpret_cast<void*>(cA.Obj()); }   // 0x74A290
inline void* __cdecl FkAddAtom(void* clump, void* atom) { Ev& e = PushEv(18, 2); e.v[0] = Tag(clump); e.v[1] = Tag(atom); return clump; }   // 0x74A490
inline u32 __cdecl FkClumpDestroy(void* clump) { Ev& e = PushEv(19, 1); e.v[0] = Tag(clump); return 1; }                       // 0x74A310
inline u32 __cdecl FkFlRead(void* stream, u32* fl) {                                                                           // 0x807970
    Ev& e = PushEv(20, 2); e.v[0] = Tag(stream); e.v[1] = u32(uintptr_t(fl));
    StreamPos(stream) += g.c->step[4];
    fl[0] = g.c->flMode == 0 ? u32(faA.Obj()) : (g.c->flMode == 1 ? 0u : 0xC0DE0000u);
    fl[1] = g.c->flNum;
    return g.c->flRet;
}
inline u32 __cdecl FkFlDeinit(u32* fl) { Ev& e = PushEv(21, 1); e.v[0] = u32(uintptr_t(fl)); return u32(uintptr_t(fl)); }      // 0x807720

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
        u32 sd = c.seed * 13u + 50u;
        sA.Fill(sd++); cA.Fill(sd++);
        for (auto& a : aA) a.Fill(sd++);
        faA.Fill(sd++); gaA.Fill(sd++); dA.Fill(sd++);
        TagClear();
        TagAdd(sA.m.data(), sA.m.size());           // 0
        TagAdd(cA.m.data(), cA.m.size());           // 1
        for (auto& a : aA) TagAdd(a.m.data(), a.m.size());   // 2..9
        TagAdd(faA.m.data(), faA.m.size());         // 10
        TagAdd(gaA.m.data(), gaA.m.size());         // 11
        TagAdd(dA.m.data(), dA.m.size());           // 12
        sA.Put32(0xC, c.spos);
        cA.Put32(4, c.clumpParent0);
        for (unsigned k = 0; k < kAtoms; ++k) aA[k].Put8(2, c.atomFlags0[k]);
        for (unsigned i = 0; i < kFa; ++i) faA.Put32(4 * i, c.fa[i]);
        for (unsigned i = 0; i < kGa; ++i) gaA.Put32(4 * i, c.ga[i]);
        dA.Put32(0, c.cbData);
        G32(kGeoPass1) = c.g607C; G32(kTexLeft) = c.g6088; G32(kTxdPos) = c.gFE8;
        G32(0xC87AE8) = c.hdr[0]; G32(0xC87AEC) = c.hdr[1]; G32(0xC87AF0) = c.hdr[2];
        G32(0xC87AF4) = c.glFrames == 1 ? u32(faA.Obj()) : c.glFrames;
        G32(0xC87AF8) = c.flN; G32(0xC87AFC) = c.gAFC;
        G32(0xC87B00) = c.glArr == 1 ? u32(gaA.Obj()) : c.glArr;
        G32(0xC87B04) = c.glNum;
        std::vector<u32> args;
        switch (fn) {
        case 0: args = {c.cbTex, u32(dA.Obj())}; break;
        case 2: args = {u32(sA.Obj()), c.txdArg}; break;
        case 5: break;
        default: args = {u32(sA.Obj())}; break;
        }
        sA.Snap(); cA.Snap();
        for (auto& a : aA) a.Snap();
        faA.Snap(); gaA.Snap(); dA.Snap();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, c.ecx, c.edx, c.stack, 4u * u32(args.size())); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, sA.Changes()); add(1, cA.Changes());
        for (unsigned k = 0; k < kAtoms; ++k) add(2 + k, aA[k].Changes());
        add(10, faA.Changes()); add(11, gaA.Changes()); add(12, dA.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        obs["ret"] = fn == 3 ? (eax & 0xFFu) : (fn == 5 ? 0u : (fn == 4 ? Tag(uintptr_t(eax)) : eax));
        obs["globals"] = json::array({G32(kGeoPass1), G32(kTexLeft), G32(kTxdPos), G32(0xC87AE8), G32(0xC87AEC), G32(0xC87AF0), Tag(uintptr_t(G32(0xC87AF4))), G32(0xC87AF8), G32(0xC87AFC),
                                      Tag(uintptr_t(G32(0xC87B00))), G32(0xC87B04)});
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
    if (!std::filesystem::exists(dir / "fn95c-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn95c-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn95c"); }
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
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {};
    env.stubs[1] = {F(0x7ED2D0, &FkFind), F(0x7EC9D0, &FkRead), F(0x7F3600, &FkCreate), F(0x730E60, &FkTexRead), F(0x7F3980, &FkAddTex), F(0x7F3730, &FkForAll), F(0x7F36A0, &FkTxdDestroy)};
    env.stubs[2] = {F(0x7ECD00, &FkSkip), F(0x730E60, &FkTexRead), F(0x7F3980, &FkAddTex), F(0x7F3730, &FkForAll), F(0x7F36A0, &FkTxdDestroy)};
    env.stubs[3] = {F(0x7ED2D0, &FkFind), F(0x7EC9D0, &FkRead), F(0x807970, &FkFlRead), F(0x807720, &FkFlDeinit), F(0x72F420, &FkMalloc), F(0x74D190, &FkGeoRead)};
    env.stubs[4] = {F(0x74A290, &FkClumpCreate), F(0x7ECD00, &FkSkip), F(0x7ED2D0, &FkFind), F(0x74D190, &FkGeoRead), F(0x7EC9D0, &FkRead), F(0x749C50, &FkAtomCreate), F(0x74BF20, &FkSetFrame),
                    F(0x749D40, &FkSetGeom), F(0x749DC0, &FkAtomDestroy), F(0x74CCC0, &FkGeoDestroy), F(0x74A490, &FkAddAtom), F(0x74A310, &FkClumpDestroy), F(0x807720, &FkFlDeinit), F(0x72F430, &FkFree)};
    env.stubs[5] = {F(0x74CCC0, &FkGeoDestroy), F(0x72F430, &FkFree), F(0x807720, &FkFlDeinit)};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"estado del EXE (globales 0xC87AE8..0xC87B07, 0x8D607C, 0x8D6088, 0xC87FE8) y los ayudantes privados de la DLL de la lectura de clumps"})},
                                 {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}

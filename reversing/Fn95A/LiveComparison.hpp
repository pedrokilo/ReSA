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
#include "RenderWare/D3DTextureBuffer.h"
#include "RenderWare/D3DIndexDataBuffer.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn95A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Nueve funciones (hook S): D3DTextureBuffer::Setup 0x72FE80, Pop(formato, ancho, alto, niveles) 0x72FF60, Push 0x72FFF0, Resize 0x730020, GetTotalDataSize 0x7300A0,
// PushWithoutIncreasingCounter 0x730AD0 y D3DIndexDataBuffer::Pop(indices) 0x730270, Resize 0x730330, GetTotalDataSize 0x7303B0.
//
// Los buffers son arenas del banco (no las caches reales de D3DResourceSystem) con una pila de punteros y objetos COM FALSOS (vtable propia con Release, GetLevelCount / GetDesc y
// GetLevelDesc, que registran sus llamadas y escriben la descripcion entera del caso): se prueba el algoritmo de las pilas sin depender del dispositivo D3D9 real.
// Colaboradores sustituidos por falsos identicos en la direccion del EXE: operator new 0x82119A (cdecl, devuelve siempre la pila nueva de la arena) y operator delete 0x8214BD (solo
// registra el puntero). Son de la CRT y no se reconstruyen, asi que no tienen hook en la DLL: la DLL los llama por su direccion del EXE (el mismo parche). Solo estan armados durante la
// llamada probada y desde el hilo del banco (fuera de eso reenvian a la CRT real: 0x824257 / 0x82413F).

namespace F95Layout {
static_assert(sizeof(D3DTextureBuffer) == 0x1C && offsetof(D3DTextureBuffer, m_nFormat) == 0 && offsetof(D3DTextureBuffer, m_nWidth) == 4 && offsetof(D3DTextureBuffer, m_nLevels) == 8
              && offsetof(D3DTextureBuffer, m_nCapcacity) == 0xC && offsetof(D3DTextureBuffer, m_nNumTexturesInBuffer) == 0x10 && offsetof(D3DTextureBuffer, m_nSize) == 0x14
              && offsetof(D3DTextureBuffer, m_apTextures) == 0x18, "D3DTextureBuffer: formato, ancho, niveles, capacidad, contador, cantidad, pila");
static_assert(sizeof(D3DIndexDataBuffer) == 0x18 && offsetof(D3DIndexDataBuffer, m_nFormat) == 0 && offsetof(D3DIndexDataBuffer, field_4) == 4 && offsetof(D3DIndexDataBuffer, m_nCapcacity) == 8
              && offsetof(D3DIndexDataBuffer, m_nNumDatasInBuffer) == 0xC && offsetof(D3DIndexDataBuffer, m_nSize) == 0x10 && offsetof(D3DIndexDataBuffer, m_apIndexData) == 0x14,
              "D3DIndexDataBuffer: formato, campo4, capacidad, contador, cantidad, pila");
}

namespace F95Live {
using namespace F95Fix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x72FE80, 0x72FF60, 0x72FFF0, 0x730020, 0x7300A0, 0x730AD0, 0x730270, 0x730330, 0x7303B0};
inline constexpr uintptr_t kNewAddr = 0x82119A, kDeleteAddr = 0x8214BD;
// Etiquetas de arena (Tag): 0 buffer, 1 pila vieja, 2 objetos, 3 pila nueva (ver Fixtures.hpp: ObjTag y el puntero de la pila de JsonC)

// ================================================================================================ objetos COM falsos
struct Obj {
    void** vt;
    u32 lc;
    u32 d[8];
    u32 pad[6];
};
static_assert(sizeof(Obj) == 0x40, "objeto falso de 0x40 bytes");
inline std::array<Obj, kObjs> objs{}, objsRef{};
inline void* texVt[24]{};
inline void* idxVt[24]{};
alignas(16) inline std::array<u8, kGuard + kWin + kGuard> bufA{}, bufRef{};
inline std::array<u32, kGuardWords + kSlots + kGuardWords> stkA{}, stkRef{};
inline std::array<u32, 8 + kBlockWords + 8> blkA{};
inline DWORD owner{};
inline bool armed{};
inline bool Mine() { return GetCurrentThreadId() == owner && armed; }

inline unsigned long __stdcall S_Release(void* self) { PushEv(3, 1).v[0] = Tag(self); return 1; }
inline unsigned long __stdcall S_GetLevelCount(void* self) { PushEv(4, 1).v[0] = Tag(self); return static_cast<Obj*>(self)->lc; }
inline long __stdcall S_GetLevelDesc(void* self, unsigned level, u32* desc) {
    Ev& e = PushEv(5, 2); e.v[0] = Tag(self); e.v[1] = level;
    std::memcpy(desc, static_cast<Obj*>(self)->d, 32);
    return 0;
}
inline long __stdcall S_GetDesc(void* self, u32* desc) {
    PushEv(6, 1).v[0] = Tag(self);
    std::memcpy(desc, static_cast<Obj*>(self)->d, 20);
    return 0;
}
inline void* __cdecl FakeNew(size_t n) {
    if (!Mine()) return reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(n);
    PushEv(1, 1).v[0] = u32(n);
    return blkA.data() + kGuardWords;
}
inline void __cdecl FakeDelete(void* p) {
    if (!Mine()) { if (p) reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; }
    PushEv(2, 1).v[0] = Tag(p);
}

inline u32 W(const void* base, size_t off) { u32 v; std::memcpy(&v, static_cast<const u8*>(base) + off, 4); return v; }

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
    const unsigned nw = Words(fn);
    json input;
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        // arenas
        FillMode(bufA.data(), bufA.size(), c.fillSeed, c.fillA);
        for (unsigned i = 0; i + 1 < nw; ++i) std::memcpy(bufA.data() + kGuard + 4 * i, &c.w[i], 4);
        const u32 stackPtr = u32(uintptr_t(stkA.data() + kGuardWords));
        std::memcpy(bufA.data() + kGuard + 4 * (nw - 1), &stackPtr, 4);
        stkA.fill(kGuardWord);
        for (unsigned i = 0; i < kSlots; ++i) {
            u32 v = kRestWord;
            if (i < c.logSlots) v = c.stkObj[i] != kNoObj ? u32(uintptr_t(&objs[c.stkObj[i]])) : c.stk[i];
            stkA[kGuardWords + i] = v;
        }
        blkA.fill(kBlockWord);
        for (unsigned o = 0; o < kObjs; ++o) {
            objs[o].vt = IsTexture(fn) ? texVt : idxVt;
            objs[o].lc = c.lc[o];
            std::memcpy(objs[o].d, c.d[o].data(), 32);
            std::memset(objs[o].pad, 0xAB, sizeof objs[o].pad);
        }
        TagClear();
        TagAdd(bufA.data(), bufA.size()); TagAdd(stkA.data(), stkA.size() * 4); TagAdd(objs.data(), sizeof objs); TagAdd(blkA.data(), blkA.size() * 4);
        std::vector<u32> args(c.a.begin(), c.a.begin() + c.nargs);
        const u32 thisPtr = u32(uintptr_t(bufA.data() + kGuard));
        if (s == 0) input = json::parse(JsonC(c));
        bufRef = bufA; stkRef = stkA; objsRef = objs;
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            armed = true;
            const Reg r = Gate(entry, args.data(), u32(args.size()), c.eax, thisPtr, c.edx, c.stack, 0u);
            armed = false;
            return r;
        });
        json obs;
        switch (fn) {
        case 1: case 6: obs["ret"] = Tag(uintptr_t(sides[s].reg.eax)); break;
        case 2: case 5: obs["ret"] = sides[s].reg.eax & 0xFFu; break;
        case 4: case 8: obs["ret"] = sides[s].reg.eax; break;
        default: break;
        }
        json buf = json::array();
        for (unsigned i = 0; i < nw; ++i) buf.push_back(i + 1 < nw ? W(bufA.data(), kGuard + 4 * i) : Tag(uintptr_t(W(bufA.data(), kGuard + 4 * i))));
        obs["buf"] = buf;
        json guard = json::array();
        for (size_t i = 0; i < bufA.size(); ++i)
            if (bufA[i] != bufRef[i] && !(i >= kGuard && i < kGuard + 4 * nw)) guard.push_back(json::array({int64(i) - int64(kGuard), bufA[i]}));
        obs["guard"] = guard;
        json sc = json::array();
        for (size_t i = 0; i < stkA.size(); ++i) if (stkA[i] != stkRef[i]) sc.push_back(json::array({int64(i) - int64(kGuardWords), Tag(uintptr_t(stkA[i]))}));
        obs["stk_changes"] = sc;
        json bc = json::array();
        for (size_t i = 0; i < blkA.size(); ++i) if (blkA[i] != kBlockWord) bc.push_back(json::array({int64(i) - int64(kGuardWords), Tag(uintptr_t(blkA[i]))}));
        obs["blk_changes"] = bc;
        obs["objs_intact"] = std::memcmp(objs.data(), objsRef.data(), sizeof objs) == 0;
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
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", true},
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
    if (!std::filesystem::exists(dir / "fn95a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn95a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn95a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    owner = GetCurrentThreadId();
    for (auto& v : texVt) v = nullptr;
    for (auto& v : idxVt) v = nullptr;
    texVt[2] = idxVt[2] = reinterpret_cast<void*>(&S_Release);
    texVt[13] = reinterpret_cast<void*>(&S_GetLevelCount);
    texVt[17] = reinterpret_cast<void*>(&S_GetLevelDesc);
    idxVt[13] = reinterpret_cast<void*>(&S_GetDesc);
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Rutas RH de las funciones probadas: se construyen ANTES que nada; el constructor deja el hook apagado. El cuerpo de la DLL se calcula aqui, antes de crear las rutas de los colaboradores y los parches de entrada.
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
    const Pair pNew = MakeCollab(env, kNewAddr, hooksFound);
    const Pair pDel = MakeCollab(env, kDeleteAddr, hooksFound);
    env.stubs[0] = {S(pNew, reinterpret_cast<void*>(&FakeNew))};
    env.stubs[3] = {S(pNew, reinterpret_cast<void*>(&FakeNew)), S(pDel, reinterpret_cast<void*>(&FakeDelete))};
    env.stubs[7] = env.stubs[3];
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"objetos COM falsos con vtable propia (Release, GetLevelCount/GetDesc, GetLevelDesc)"})},
            {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}

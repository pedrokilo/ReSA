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
#include "Entity/Ped/Ped.h"
#include "Entity/Object/Object.h"
#include "Population.h"
#include "SimpleTransform.h"
#include "Core/Matrix.h"
#include "World.h"
#include "Timer.h"
#include "CutsceneMgr.h"
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn86B dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion (hook S): CPopulation::CreateSunbatheObject 0x6EABA0 (cdecl; posicion e int32 tipo; devuelve el objeto o 0). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL:
// rand() del CRT del EXE 0x821B1E (sin hook: la DLL lo llama por su direccion), CObject::operator new 0x5A1EE0 (sin hook, cuerpo de la DLL parcheado) y el constructor CObject(int32, bool) 0x5A1D70
// (hook Constructor; la DLL llama al constructor real, que se localiza tras la llamada a operator new), CMatrix::UpdateRwMatrix 0x59AD70 (hook), CSimpleTransform::UpdateRwMatrix 0x54EF40 (sin hook, cuerpo
// de la DLL parcheado) y CWorld::Add 0x563220 (hook). SetIsStatic es la ranura 4 de una tabla de funciones virtuales propia que pone el constructor falso. Los falsos registran sus argumentos EN ORDEN y
// capturan el estado del objeto en el momento de la llamada (posicion, velocidades, tipo, contador de temporales, hora de borrado). Globales reales del juego (se guardan y se restauran tras cada lado):
// CObject::nNoTempObjects (0xBB4A70), CTimer::m_snTimeInMilliseconds (0xB7CB84) y los words de modelo 0x8CD638..0x8CD698.

namespace F86BLayout {
static_assert(offsetof(CObject, m_nObjectType) == 0x13C && offsetof(CObject, m_nRemovalTime) == 0x150, "CObject: tipo +0x13C, hora de borrado +0x150");
static_assert(offsetof(CPhysical, m_vecMoveSpeed) == 0x44 && offsetof(CPhysical, m_vecTurnSpeed) == 0x50, "CPhysical: velocidad +0x44, giro +0x50");
static_assert(offsetof(CObject, m_matrix) == 0x14 && offsetof(CObject, m_placement) == 4, "CPlaceable: transformacion +4, matriz +0x14 (el objeto RW +0x18 es privado de CEntity: lo comprueba el propio banco)");
static_assert(sizeof(CObject) == 0x17C, "CObject mide 0x17C");
}

namespace F86BLive {
using namespace F86BFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x6EABA0};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint16& G16(uintptr_t a) { return *reinterpret_cast<uint16*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    u32 Get8(size_t off) const { return m[kGuard + off]; }
    // Cambios respecto a la imagen de referencia, sin los bytes de los rangos [lo, hi) indicados (campos de puntero que fija el constructor falso)
    json Changes(std::initializer_list<std::pair<int64, int64>> masks = {}) const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            bool masked = false;
            for (const auto& mk : masks) masked |= off >= mk.first && off < mk.second;
            if (m[i] != r[i] && !masked) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kObjArena> objA;
inline Arena<kMatArena> matA;
inline Arena<kRwArena> rwA;
inline Arena<kFrameArena> frameA;
inline Arena<kPosArena> posA;

struct State { const C* c{}; };
inline State g;

// Eventos: 1 rand []; 2 operator new [tamano]; 3 constructor [objeto, modelo, bandera]; 4 CMatrix::UpdateRwMatrix [matriz, RwMatrix, x, y, z de la matriz];
// 5 CSimpleTransform::UpdateRwMatrix [transformacion, RwMatrix, x, y, z]; 6 SetIsStatic [objeto, bandera, byte +0x13C, dword +0x44, dword +0x50]; 7 CWorld::Add [objeto, contador de temporales, +0x150, byte +0x13C].
inline u32 __cdecl FkRand() {                                                                         // 0x821B1E
    PushEv(1, 0);
    return g.c->randv;
}
inline void* __cdecl FkNew(size_t size) {                                                             // 0x5A1EE0 (cdecl)
    Ev& e = PushEv(2, 1); e.v[0] = u32(size);
    return g.c->newnull ? nullptr : reinterpret_cast<void*>(objA.Obj());
}
inline void __fastcall FkDummy(void*, void*) {}
inline void __fastcall FkSetStatic(void* self, void*, u32 flag) {                                     // SetIsStatic (ranura 4, ret 4)
    Ev& e = PushEv(6, 5); e.v[0] = Tag(self); e.v[1] = flag & 0xFFu; e.v[2] = objA.Get8(0x13C); e.v[3] = objA.Get32(0x44); e.v[4] = objA.Get32(0x50);
}
inline void* gVtable[8]{reinterpret_cast<void*>(&FkDummy), reinterpret_cast<void*>(&FkDummy), reinterpret_cast<void*>(&FkDummy), reinterpret_cast<void*>(&FkDummy), reinterpret_cast<void*>(&FkSetStatic),
                        reinterpret_cast<void*>(&FkDummy), reinterpret_cast<void*>(&FkDummy), reinterpret_cast<void*>(&FkDummy)};
inline void* __fastcall FkCtor(void* self, void*, int32 model, u32 create) {                          // 0x5A1D70 (ret 8): devuelve this
    Ev& e = PushEv(3, 3); e.v[0] = Tag(self); e.v[1] = u32(model); e.v[2] = create & 0xFFu;
    objA.Put32(0, u32(reinterpret_cast<uintptr_t>(&gVtable[0])));
    objA.Put32(0x14, g.c->hasmat ? u32(matA.Obj()) : 0u);
    objA.Put32(0x18, g.c->hasrw ? u32(rwA.Obj()) : 0u);
    return self;
}
inline void __fastcall FkMatUpdate(void* self, void*, void* rw) {                                     // 0x59AD70 (ret 4)
    Ev& e = PushEv(4, 5); e.v[0] = Tag(self); e.v[1] = Tag(rw);
    const auto* p = static_cast<const u8*>(self);
    memcpy(&e.v[2], p + 0x30, 4); memcpy(&e.v[3], p + 0x34, 4); memcpy(&e.v[4], p + 0x38, 4);
}
inline void __fastcall FkSimpleUpdate(void* self, void*, void* rw) {                                  // 0x54EF40 (ret 4)
    Ev& e = PushEv(5, 5); e.v[0] = Tag(self); e.v[1] = Tag(rw);
    const auto* p = static_cast<const u8*>(self);
    memcpy(&e.v[2], p, 4); memcpy(&e.v[3], p + 4, 4); memcpy(&e.v[4], p + 8, 4);
}
inline void __cdecl FkWorldAdd(void* entity) {                                                        // 0x563220 (cdecl)
    Ev& e = PushEv(7, 4); e.v[0] = Tag(entity); e.v[1] = G16(kCountAddr); e.v[2] = objA.Get32(0x150); e.v[3] = objA.Get8(0x13C);
}

inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = RegionsHash();
    obs["count"] = G16(kCountAddr);
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

inline void SetupArenas(const C& c) {
    objA.Fill(c.seed * 13u + 50u); matA.Fill(c.seed * 13u + 51u); rwA.Fill(c.seed * 13u + 52u); frameA.Fill(c.seed * 13u + 53u); posA.Fill(c.seed * 13u + 54u);
}
inline void SnapArenas() { objA.Snap(); matA.Snap(); rwA.Snap(); frameA.Snap(); posA.Snap(); }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        SetupArenas(c);
        TagClear();
        TagAdd(objA.m.data(), objA.m.size());       // 0
        TagAdd(matA.m.data(), matA.m.size());       // 1
        TagAdd(rwA.m.data(), rwA.m.size());         // 2
        TagAdd(frameA.m.data(), frameA.m.size());   // 3
        TagAdd(posA.m.data(), posA.m.size());       // 4
        posA.Put32(0, c.pos[0]); posA.Put32(4, c.pos[1]); posA.Put32(8, c.pos[2]);
        rwA.Put32(4, u32(frameA.Obj()));
        G16(kCountAddr) = uint16(c.count);
        G32(kTimeAddr) = c.time;
        for (unsigned i = 0; i < kModels; ++i) G16(kModelAddr[i]) = uint16(c.models[i]);
        const std::vector<u32> args = { u32(posA.Obj()), c.type };
        SnapArenas();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, c.ecx, c.edx, c.stack, 8); });
        json obs;
        obs["ret"] = Tag(uintptr_t(sides[s].reg.eax));
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, objA.Changes({{0, 4}, {0x14, 0x1C}})); add(1, matA.Changes()); add(2, rwA.Changes()); add(3, frameA.Changes()); add(4, posA.Changes());
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
// Primera llamada a una funcion del mismo modulo tras la llamada a `newFn` dentro de `fn` (el constructor del objeto recien reservado): su direccion o 0.
inline uintptr_t FindCtorCall(uintptr_t fn, uintptr_t newFn) {
    HMODULE mod{}; if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(fn), &mod)) return 0;
    const auto modBase = uintptr_t(mod); const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase + reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew);
    const uintptr_t modEnd = modBase + nt->OptionalHeader.SizeOfImage;
    auto FollowIn = [&](uintptr_t q) { for (int k = 0; k < 4 && q >= modBase && q < modEnd && *reinterpret_cast<const uint8*>(q) == 0xE9; ++k) q = q + 5 + *reinterpret_cast<const int32*>(q + 1); return q; };
    fn = FollowIn(fn); const auto nf = FollowIn(newFn); bool seen = false;
    for (uintptr_t p = fn; p < fn + 0x900; ++p) {
        if (*reinterpret_cast<const uint8*>(p) != 0xE8) continue;
        const uintptr_t t = p + 5 + *reinterpret_cast<const int32*>(p + 1);
        if (!seen) { if (t >= modBase && t < modEnd && FollowIn(t) == nf) { seen = true; p += 4; } continue; }
        if (t < modBase || t >= modEnd) continue;
        if (*reinterpret_cast<const uint8*>(t) != 0xE9 && (t & 15)) continue;
        return t;
    }
    return 0;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn86b-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn86b-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn86b"); }
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
    const uintptr_t dllObjNew = Addr(static_cast<void*(*)(size_t)>(&CObject::operator new));
    const uintptr_t dllObjCtor = FindCtorCall(Body(env.route[0]->Body()), dllObjNew);
    if (!dllObjCtor) { Write({{"event", "error"}, {"reason", "CObject constructor not found"}}); return; }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    const Pair pRand = MakeCollab(env, 0x821B1E, hooksFound);
    Pair pNew = MakeCollab(env, 0x5A1EE0, hooksFound); pNew.dll = dllObjNew;   // con hook desde el lote 100A (apagado durante el banco)
    Pair pCtor = MakeCollab(env, 0x5A1D70, hooksFound); pCtor.dll = dllObjCtor;
    Pair pMat = MakeCollab(env, 0x59AD70, hooksFound);
    if (!pMat.hooked) pMat.dll = Body(std::bit_cast<uintptr_t>(static_cast<void (CMatrix::*)(RwMatrix*) const>(&CMatrix::UpdateRwMatrix)));   // sin hook registrado: cuerpo de la DLL por su simbolo
    const Pair pSimple{0x54EF40, Body(std::bit_cast<uintptr_t>(&CSimpleTransform::UpdateRwMatrix)), false};
    const Pair pAdd = MakeCollab(env, 0x563220, hooksFound);
    env.stubs[0] = {S(pRand, reinterpret_cast<void*>(&FkRand)), S(pNew, reinterpret_cast<void*>(&FkNew)), S(pCtor, reinterpret_cast<void*>(&FkCtor)),
                    S(pMat, reinterpret_cast<void*>(&FkMatUpdate)), S(pSimple, reinterpret_cast<void*>(&FkSimpleUpdate)), S(pAdd, reinterpret_cast<void*>(&FkWorldAdd))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"seleccion del tipo efectivo, modelo e indicador de estatico, posicion, velocidades, tipo temporal, contador y hora de borrado; CEntity::UpdateRwMatrix (con GetRwObject y GetModellingMatrix) del lado de la DLL"})},
                                 {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}

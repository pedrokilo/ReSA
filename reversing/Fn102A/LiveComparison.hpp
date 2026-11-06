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
#include "Fx/FxPrtMult.h"
#include "SimpleTransform.h"
#include "Core/Vector.h"
#include "Audio/AESound.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Placeable.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn102A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Nueve funciones (hook S), cada una con 16384 pares EXE/DLL:
//   0  FxPrtMult_c::Constructor7f                 0x4AB290  thiscall, objeto en ECX, 7 floats en pila, `ret 1Ch`; devuelve this. Sin colaboradores.
//   1  CSimpleTransform::UpdateMatrix             0x54F1B0  thiscall, transformacion en ECX, matriz en pila, `ret 4`; void. Colaboradores (con hook RH, thiscall): CMatrix::SetTranslate 0x59AF40 (x, y, z) y CMatrix::SetRotateZOnly 0x59B020 (rumbo).
//   2  CSimpleTransform::UpdateRwMatrix           0x54EF40  thiscall, transformacion en ECX, RwMatrix en pila, `ret 4`; void. Sin colaboradores sustituidos: fcos/fsin y RwMatrixUpdate (0x7F18A0) corren de verdad en ambos lados.
//   3  CVector::SquaredMagnitudeExe               0x406DA0  thiscall, vector en ECX; float en ST0; ECX y EDX intactos. Sin colaboradores.
//   4  CAESound::Destructor                       0x4EF660  thiscall, sonido en ECX; void. Colaborador (con hook RH): CEntity::CleanUpOldReference 0x571A00 (thiscall, &referencia en pila, `ret 4`).
//   5  RpClumpForAllAtomics                       0x749B70  cdecl (clump, callback, dato); devuelve el clump. El callback es una funcion del banco (cdecl) que registra sus llamadas.
//   6  RpGeometryForAllMaterials                  0x74C790  cdecl (geometria, callback, dato); devuelve la geometria. Callback del banco.
//   7  RpAtomicSetFrame                           0x74BF20  cdecl (atomico, marco); devuelve el atomico. Colaborador (sin hook RH): _rwObjectHasFrameSetFrame 0x804EF0 (cdecl).
//   8  AtomicDefaultRenderCallBack                0x7491C0  cdecl (atomico); devuelve el atomico o 0. Colaborador (sin hook RH): RxPipelineExecute 0x805710 (cdecl).
// Falsos identicos en la direccion del EXE y (los que tienen hook) en el cuerpo de la DLL. Registran sus argumentos EN ORDEN y los punteros a arenas como etiquetas.
// NO se sustituyen (corren de verdad en ambos lados): todo el cuerpo de cada funcion (la x87 de UpdateRwMatrix y SquaredMagnitudeExe, los bucles de recorrido, las escrituras).

namespace F102ALayout {
static_assert(sizeof(FxPrtMult_c) == 0x1C && offsetof(FxPrtMult_c, m_Color) == 0 && offsetof(FxPrtMult_c, m_fSize) == 0x10 && offsetof(FxPrtMult_c, m_Rot) == 0x14 && offsetof(FxPrtMult_c, m_fLife) == 0x18, "FxPrtMult_c");
static_assert(offsetof(CSimpleTransform, m_vPosn) == 0 && offsetof(CSimpleTransform, m_fHeading) == 0xC && sizeof(CSimpleTransform) == 0x10, "CSimpleTransform");
static_assert(sizeof(RwMatrix) == 0x40 && offsetof(RwMatrix, flags) == 0xC && offsetof(RwMatrix, pos) == 0x30 && offsetof(RwMatrix, up) == 0x10 && offsetof(RwMatrix, at) == 0x20, "RwMatrix");
static_assert(offsetof(CAESound, m_PhysicalEntity) == 8, "CAESound");
static_assert(offsetof(RpAtomic, inClumpLink) == 0x40 && offsetof(RpAtomic, pipeline) == 0x6C && offsetof(RwObject, privateFlags) == 3, "RpAtomic");
static_assert(offsetof(RpClump, atomicList) == 8, "RpClump");
static_assert(offsetof(RpGeometry, matList) + offsetof(RpMaterialList, materials) == 0x20 && offsetof(RpGeometry, matList) + offsetof(RpMaterialList, numMaterials) == 0x24, "RpGeometry");
}

namespace F102ALive {
using namespace F102AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x4AB290, 0x54F1B0, 0x54EF40, 0x406DA0, 0x4EF660, 0x749B70, 0x74C790, 0x74BF20, 0x7491C0};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F102AFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Pone a cero la palabra en la imagen y en la referencia (para que los punteros, que son direcciones reales, no cuenten como bytes cambiados: se observan aparte como etiquetas)
    void Mask(size_t off) { memset(m.data() + kGuard + off, 0, 4); memset(r.data() + kGuard + off, 0, 4); }
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            if (m[i] != r[i]) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kObjWin> objA;      // 0: objeto principal (FxPrtMult, transformacion, vector, sonido, geometria, atomico)
inline Arena<kOutWin> outA;      // 1: matriz de salida (UpdateMatrix, UpdateRwMatrix) o marco (RpAtomicSetFrame)
inline Arena<kListWin> listA;    // 2: clump y atomicos (funcion 5), arreglos de materiales (funcion 6)
inline Arena<kObjWin> entA;      // 3: entidad referenciada por el sonido (funcion 4)

// Estado del caso en curso para los falsos y los callbacks
struct State { const C* c{}; unsigned calls{}; };
inline State g;

// Eventos: 10 CEntity::CleanUpOldReference 0x571A00 [entidad, &referencia, valor de la referencia]; 20 CMatrix::SetTranslate 0x59AF40 [matriz, x, y, z]; 21 CMatrix::SetRotateZOnly 0x59B020 [matriz, rumbo];
// 30 callback de atomico [atomico, dato]; 31 callback de material [material, dato]; 32 RxPipelineExecute 0x805710 [tuberia, atomico, heapReset]; 33 _rwObjectHasFrameSetFrame 0x804EF0 [objeto, marco]
inline void __fastcall FkCleanUp(void* self, void*, void* slot) {                                    // 0x571A00 (`ret 4`)
    Ev& e = PushEv(10, 3); e.v[0] = Tag(self); e.v[1] = Tag(slot); e.v[2] = Tag(*static_cast<const u32*>(slot));
}
inline void __fastcall FkSetTranslate(void* self, void*, u32 x, u32 y, u32 z) {                      // 0x59AF40 (`ret 0Ch`)
    Ev& e = PushEv(20, 4); e.v[0] = Tag(self); e.v[1] = x; e.v[2] = y; e.v[3] = z;
}
inline void __fastcall FkSetRotateZ(void* self, void*, u32 angle) {                                  // 0x59B020 (`ret 4`)
    Ev& e = PushEv(21, 2); e.v[0] = Tag(self); e.v[1] = angle;
}
inline u32 __cdecl FkRxPipelineExecute(void* pipeline, void* data, u32 heapReset) {                  // 0x805710
    Ev& e = PushEv(32, 3); e.v[0] = Tag(pipeline); e.v[1] = Tag(data); e.v[2] = heapReset;
    return g.c->a[2];
}
inline void __cdecl FkSetFrame(void* object, void* frame) {                                          // 0x804EF0: deja un byte conocido en +3 del objeto (despues la funcion hace |= 1)
    Ev& e = PushEv(33, 2); e.v[0] = Tag(object); e.v[1] = Tag(frame);
    static_cast<uint8*>(object)[3] = uint8(g.c->a[3]);
}
// Callback de RpClumpForAllAtomics: registra (atomico, dato), sobrescribe el enlace actual si el caso lo pide y devuelve 0 en la llamada elegida (o a la 12.a) o un valor no nulo
inline void* __cdecl CbAtomic(void* atomic, void* data) {
    Ev& e = PushEv(30, 2); e.v[0] = Tag(atomic); e.v[1] = Tag(data);
    const unsigned i = g.calls++;
    uint8* const link = static_cast<uint8*>(atomic) + 0x40;
    if (g.c->a[2] == 1) memcpy(link, &g.c->a[6], 4);
    else if (g.c->a[2] == 2) memcpy(link + 4, &g.c->a[6], 4);
    return (i == g.c->a[1] || i >= 11) ? nullptr : reinterpret_cast<void*>(uintptr_t(g.c->a[4]));
}
// Callback de RpGeometryForAllMaterials: registra (material, dato); segun el caso cambia el arreglo de materiales (tras la primera llamada) o el contador de materiales de la geometria
inline uintptr_t geomObj{}, array2Addr{};
inline void* __cdecl CbMaterial(void* material, void* data) {
    Ev& e = PushEv(31, 2); e.v[0] = Tag(material); e.v[1] = Tag(data);
    const unsigned i = g.calls++;
    const u32 mode = g.c->a[2];
    if (mode == 1 && i == 0) G32(geomObj + 0x20) = u32(array2Addr);
    else if (mode == 2) G32(geomObj + 0x24) = 0;
    else if (mode == 3) G32(geomObj + 0x24) = 100;
    return (i == g.c->a[1] || i >= 11) ? nullptr : reinterpret_cast<void*>(uintptr_t(g.c->a[20]));
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

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        objA.Fill(sd++); outA.Fill(sd++); listA.Fill(sd++); entA.Fill(sd++);
        TagClear();
        TagAdd(objA.m.data(), objA.m.size());     // 0
        TagAdd(outA.m.data(), outA.m.size());     // 1
        TagAdd(listA.m.data(), listA.m.size());   // 2
        TagAdd(entA.m.data(), entA.m.size());     // 3
        g = State{}; g.c = &c; nev = 0;
        std::vector<u32> args;
        u32 ecx = u32(objA.Obj()), popBytes = 0;
        switch (fn) {
        case 0:
            for (u32 k = 0; k < 7; ++k) objA.Put32(4 * k, c.a[7 + k]);
            for (u32 k = 0; k < 7; ++k) args.push_back(c.a[k]);
            break;
        case 1:
            for (u32 k = 0; k < 4; ++k) objA.Put32(4 * k, c.a[k]);
            args = { u32(outA.Obj()) };
            break;
        case 2:
            for (u32 k = 0; k < 4; ++k) objA.Put32(4 * k, c.a[k]);
            for (u32 k = 0; k < 16; ++k) outA.Put32(4 * k, c.a[4 + k]);
            args = { u32(outA.Obj()) };
            break;
        case 3:
            for (u32 k = 0; k < 3; ++k) objA.Put32(4 * k, c.a[k]);
            break;
        case 4: {
            const u32 ref = c.a[0] == 0 ? 0u : (c.a[0] == 1 ? u32(entA.Obj()) : (c.a[0] == 2 ? c.a[1] : u32(objA.Obj())));
            objA.Put32(8, ref);
            break;
        }
        case 5: {
            const u32 cnt = c.a[0];
            const u32 sent = u32(listA.Obj(8));
            auto L = [&](u32 k) { return u32(listA.Obj(0x60 + 0x80 * k)); };
            listA.Put32(8, cnt ? L(0) : sent);
            listA.Put32(12, cnt ? L(cnt - 1) : sent);
            for (u32 k = 0; k < cnt; ++k) {
                listA.Put32(0x60 + 0x80 * k, k + 1 < cnt ? L(k + 1) : (c.a[5] ? L(0) : sent));
                listA.Put32(0x64 + 0x80 * k, k ? L(k - 1) : sent);
            }
            args = { u32(listA.Obj()), u32(Addr(&CbAtomic)), c.a[3] };
            ecx = c.seed; popBytes = 12;
            break;
        }
        case 6: {
            geomObj = objA.Obj(); array2Addr = listA.Obj(0x200);
            objA.Put32(0x20, u32(listA.Obj(0x100))); objA.Put32(0x24, c.a[0]);
            for (u32 k = 0; k < 8; ++k) { listA.Put32(0x100 + 4 * k, c.a[4 + k]); listA.Put32(0x200 + 4 * k, c.a[12 + k]); }
            args = { u32(objA.Obj()), u32(Addr(&CbMaterial)), c.a[3] };
            ecx = c.seed; popBytes = 12;
            break;
        }
        case 7: {
            objA.Put8(3, c.a[2]);
            const u32 frame = c.a[0] == 0 ? u32(outA.Obj()) : (c.a[0] == 1 ? 0u : c.a[1]);
            args = { u32(objA.Obj()), frame };
            ecx = c.seed; popBytes = 8;
            break;
        }
        default: {
            objA.Put32(0x6C, c.a[0] == 1 ? c.a[1] : 0u);
            args = { u32(objA.Obj()) };
            ecx = c.seed; popBytes = 4;
            break;
        }
        }
        objA.Snap(); outA.Snap(); listA.Snap(); entA.Snap();
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs;
        switch (fn) {
        case 0: case 5: case 6: case 7: case 8:
            obs["eax"] = Tag(sides[s].reg.eax);
            break;
        case 3:
            obs["ecx"] = Tag(sides[s].reg.ecx); obs["edx"] = Tag(sides[s].reg.edx);
            break;
        default: break;
        }
        // Los punteros que la funcion deja en la memoria (enlaces, referencia, geometria) se observan como etiquetas y se enmascaran antes de comparar los bytes (Arena::Mask)
        if (fn == 4) { obs["slot"] = Tag(objA.Get32(8)); objA.Mask(8); }
        if (fn == 5) {
            json links = json::array();
            links.push_back(Tag(listA.Get32(8))); links.push_back(Tag(listA.Get32(12))); listA.Mask(8); listA.Mask(12);
            for (u32 k = 0; k < c.a[0]; ++k) {
                links.push_back(Tag(listA.Get32(0x60 + 0x80 * k))); links.push_back(Tag(listA.Get32(0x64 + 0x80 * k)));
                listA.Mask(0x60 + 0x80 * k); listA.Mask(0x64 + 0x80 * k);
            }
            obs["links"] = links;
        }
        if (fn == 6) {
            obs["geom"] = json::array({Tag(objA.Get32(0x20)), objA.Get32(0x24)});
            objA.Mask(0x20); objA.Mask(0x24);
        }
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, objA.Changes()); add(1, outA.Changes()); add(2, listA.Changes()); add(3, entA.Changes());
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
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    // Tuberia por defecto del motor tal como la lee 0x7491C0: [0xC9BC60] + [0xC97B24] + 0x3C
    const u32 defaultPipeline = G32(G32(0xC9BC60) + G32(0xC97B24) + 0x3C);
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"default_pipeline", defaultPipeline},
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
    if (!std::filesystem::exists(dir / "fn102a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn102a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn102a"); }
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
    env.stubs[1] = {F(0x59AF40, &FkSetTranslate), F(0x59B020, &FkSetRotateZ)};
    env.stubs[2] = {};
    env.stubs[3] = {};
    env.stubs[4] = {F(0x571A00, &FkCleanUp)};
    env.stubs[5] = {};
    env.stubs[6] = {};
    env.stubs[7] = {F(0x804EF0, &FkSetFrame)};
    env.stubs[8] = {F(0x805710, &FkRxPipelineExecute)};
    const char* dllReal[kFunctions] = {
        "las siete escrituras de los campos (copia de bits)",
        "orden y argumentos de las dos llamadas a la matriz; las dos son falsos identicos",
        "fcos/fsin de precision completa, los once campos escritos y RwMatrixUpdate (0x7F18A0, del EXE en ambos lados)",
        "toda la aritmetica x87 (suma de cuadrados en ST0, ECX y EDX intactos)",
        "la comprobacion de la referencia y su puesta a cero; CleanUpOldReference es falso identico",
        "todo el recorrido de la lista enlazada de atomicos; el callback es del banco",
        "todo el bucle sobre el arreglo de materiales; el callback es del banco",
        "el byte +3 del atomico (|= 1 tras la llamada); _rwObjectHasFrameSetFrame es falso identico",
        "la eleccion de la tuberia (propia o la del motor, leida de memoria real) y el valor devuelto; RxPipelineExecute es falso identico"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}

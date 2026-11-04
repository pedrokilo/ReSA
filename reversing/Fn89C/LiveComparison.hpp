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
#include "Tasks/TaskTypes/TaskSimpleGangDriveBy.h"
#include "Entity/Ped/Ped.h"
#include "Camera.h"
#include "CutsceneMgr.h"
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn89C dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion (hook S): CTaskSimpleGangDriveBy::FireGun 0x627CC0 (thiscall: ECX = tarea, ped en pila, `ret 4`; void). Colaboradores sustituidos por falsos identicos en la direccion del EXE (y en el cuerpo de la DLL
// cuando tienen hook RH): GetAnimHierarchyFromSkinClump 0x734A40, RpHAnimIDGetIndex 0x7C51A0, RpHAnimHierarchyGetMatrixArray 0x7C5120 y RwV3dTransformPoints 0x7EDD90 (la DLL los llama por su direccion del EXE),
// CCamera::Find3rdPersonCamTargetVector 0x514970, CWeapon::Fire 0x742300 y CPed::DoGunFlash 0x5DF340 (con hook en la DLL). Los falsos registran sus argumentos EN ORDEN (con el contenido de los vectores que reciben).
// NO se sustituyen: CPed::IsPlayer (lee m_nPedType), CVector::Normalise 0x59C910, CMatrix * CVector 0x59C890, 0x40FE90 y 0x40FE30, ni la aritmetica de la propia funcion (resta de vectores, rango del apuntado, producto con el
// eje y corte con la caja del coche): corren de verdad en ambos lados (x87 del EXE frente a los ayudantes x87 de 24 bits de la DLL).
// Memoria real del juego (se guarda y se restaura tras cada lado): byte de la camara activa (0xB6F081), las tres camaras (0xB6F19C, modo +0xC y origen +0x19C) y 12 entradas libres de ms_modelInfoPtrs (indices 19980..19991).
// Mutaciones (una sola vez, dentro de un falso): 1 (GetAnimHierarchy) conmuta la entidad objetivo de la tarea; 2 (RpHAnimIDGetIndex) cambia el modelo del vehiculo; 3 (RwV3dTransformPoints) escribe coords.x;
// 4 (Find3rdPersonCamTargetVector, solo el jugador) conmuta la asociacion de animacion. El EXE lee todo eso despues de esas llamadas.

namespace F89CLayout {
static_assert(offsetof(CTaskSimpleGangDriveBy, m_nFakeShootDirn) == 0x14 && offsetof(CTaskSimpleGangDriveBy, m_pAnimAssoc) == 0x2C && offsetof(CTaskSimpleGangDriveBy, m_pWeaponInfo) == 0x30
              && offsetof(CTaskSimpleGangDriveBy, m_pTargetEntity) == 0x34 && offsetof(CTaskSimpleGangDriveBy, m_vecCoords) == 0x38, "campos de la tarea");
static_assert(offsetof(CPed, m_pVehicle) == 0x58C && offsetof(CPed, m_nPedType) == 0x598 && offsetof(CPed, m_aWeapons) == 0x5A0
              && offsetof(CPed, m_nActiveWeaponSlot) == 0x718 && offsetof(CPed, m_pedIK) + offsetof(CPedIK, m_nFlags) == 0x528 && offsetof(CPed, m_apBones) + 6 * 4 == 0x4A0, "campos de CPed");
static_assert(sizeof(CWeapon) == 0x1C, "CWeapon mide 0x1C");
static_assert(sizeof(CCam) == 0x238, "CCam mide 0x238");
static_assert(offsetof(CCam, m_vecSource) == 0x19C && offsetof(CCam, m_nMode) == 0xC, "CCam: modo en +0xC y origen en +0x19C");
static_assert(sizeof(CPed) <= 0x7C0, "la arena del ped cubre el objeto");
}

namespace F89CLive {
using namespace F89CFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x627CC0};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint16& G16(uintptr_t a) { return *reinterpret_cast<uint16*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F89CFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { const uint16 w = uint16(v); memcpy(m.data() + kGuard + off, &w, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia, sin los bytes de hasta dos campos de puntero [lo, hi) (cambian por direccion)
    json Changes(size_t lo1 = 0, size_t hi1 = 0, size_t lo2 = 0, size_t hi2 = 0) const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            const bool masked = (hi1 && off >= int64(lo1) && off < int64(hi1)) || (hi2 && off >= int64(lo2) && off < int64(hi2));
            if (m[i] != r[i] && !masked) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kTaskWin> taskA;
inline Arena<kPedWin> pedA;
inline Arena<kPdWin> pdA;
inline Arena<kWiWin> wiA;
inline Arena<kEntWin> entA;
inline Arena<kMatWin> ematA;
inline Arena<kVehWin> vehA;
inline Arena<kMatWin> vmatA;
inline Arena<kMiWin> miA, miB;
inline Arena<kColWin> colA, colB;
inline Arena<kClumpWin> clumpA;
inline Arena<kBoneWin> boneA;
inline Arena<kHierWin> hierA;
inline Arena<kMaWin> maA;
inline Arena<kAssocWin> assocA;
inline Arena<kMatWin> pmatA;

// Estado del caso en curso para los falsos
struct State { const C* c{}; bool mutDone[8]{}; };
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

// Eventos: 1 GetAnimHierarchyFromSkinClump [clump]; 2 RpHAnimIDGetIndex [jerarquia, id]; 3 RpHAnimHierarchyGetMatrixArray [jerarquia]; 4 RwV3dTransformPoints [out, in, n, matriz, in.xyz, out==in];
// 5 Find3rdPersonCamTargetVector [camara, rango, origen.xyz, source, target]; 6 CWeapon::Fire [arma, ped, start==barrel, start.xyz, barrel.xyz, entidad, hay objetivo, objetivo.xyz, alt.xyz]; 7 DoGunFlash [ped, duracion, mano].
inline u32 __cdecl FkHier(void* clump) {                                                            // 0x734A40
    Ev& e = PushEv(1, 1); e.v[0] = Tag(clump);
    if (Once(1)) taskA.Put32(0x34, g.c->ent ? 0u : u32(entA.Obj()));
    return u32(hierA.Obj());
}
inline u32 __cdecl FkIndex(void* hier, u32 id) {                                                    // 0x7C51A0
    Ev& e = PushEv(2, 2); e.v[0] = Tag(hier); e.v[1] = id;
    if (Once(2)) vehA.Put16(0x22, kModelBase + g.c->vmodel2);
    return g.c->boneidx;
}
inline u32 __cdecl FkMatArr(void* hier) {                                                           // 0x7C5120
    Ev& e = PushEv(3, 1); e.v[0] = Tag(hier);
    return u32(maA.Obj(kMaObjOff));
}
inline u32 __cdecl FkXform(float* out, float* in, u32 n, void* matrix) {                            // 0x7EDD90
    Ev& e = PushEv(4, 8); e.v[0] = TagS(uintptr_t(out)); e.v[1] = TagS(uintptr_t(in)); e.v[2] = n; e.v[3] = Tag(matrix); Vec3(e, 4, in); e.v[7] = out == in;
    memcpy(out, g.c->xform, 12);
    if (Once(3)) taskA.Put32(0x38, g.c->mutVal);
    return u32(uintptr_t(out));
}
inline void __fastcall FkFind(void* self, void*, u32 range, u32 x, u32 y, u32 z, float* src, float* tgt) {   // 0x514970 (ret 0x18)
    Ev& e = PushEv(5, 7); e.v[0] = u32(uintptr_t(self)); e.v[1] = range; e.v[2] = x; e.v[3] = y; e.v[4] = z; e.v[5] = TagS(uintptr_t(src)); e.v[6] = TagS(uintptr_t(tgt));
    memcpy(src, g.c->fsrc, 12); memcpy(tgt, g.c->ftgt, 12);
    if (Once(4)) taskA.Put32(0x2C, g.c->assoc ? 0u : u32(assocA.Obj()));
}
inline u32 __fastcall FkFire(void* self, void*, void* ped, float* start, float* barrel, void* ent, float* tgt, float* alt) {   // 0x742300 (ret 0x18)
    Ev& e = PushEv(6, 18); e.v[0] = Tag(self); e.v[1] = Tag(ped); e.v[2] = start == barrel; Vec3(e, 3, start); Vec3(e, 6, barrel); e.v[9] = Tag(ent); e.v[10] = tgt != nullptr;
    if (tgt) Vec3(e, 11, tgt);
    Vec3(e, 14, alt);
    return 0;
}
inline u32 __fastcall FkFlash(void* self, void*, u32 duration, u32 hand) {                          // 0x5DF340 (ret 8)
    Ev& e = PushEv(7, 3); e.v[0] = Tag(self); e.v[1] = duration; e.v[2] = hand;
    return 0;
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

inline void SetupArenas(const C& c) {
    u32 s = c.seed * 13u + 50u;
    taskA.Fill(s++); pedA.Fill(s++); pdA.Fill(s++); wiA.Fill(s++); entA.Fill(s++); ematA.Fill(s++); vehA.Fill(s++); vmatA.Fill(s++); miA.Fill(s++); miB.Fill(s++); colA.Fill(s++); colB.Fill(s++);
    clumpA.Fill(s++); boneA.Fill(s++); hierA.Fill(s++); maA.Fill(s++); assocA.Fill(s++); pmatA.Fill(s++);
}
inline void SnapArenas() {
    taskA.Snap(); pedA.Snap(); pdA.Snap(); wiA.Snap(); entA.Snap(); ematA.Snap(); vehA.Snap(); vmatA.Snap(); miA.Snap(); miB.Snap(); colA.Snap(); colB.Snap();
    clumpA.Snap(); boneA.Snap(); hierA.Snap(); maA.Snap(); assocA.Snap(); pmatA.Snap();
}
inline void Put3(auto& a, size_t off, const u32* p) { a.Put32(off, p[0]); a.Put32(off + 4, p[1]); a.Put32(off + 8, p[2]); }
inline void PutN(auto& a, size_t off, const u32* p, size_t n) { for (size_t i = 0; i < n; ++i) a.Put32(off + 4 * i, p[i]); }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        SetupArenas(c);
        TagClear();
        TagAdd(taskA.m.data(), taskA.m.size());     // 0
        TagAdd(pedA.m.data(), pedA.m.size());       // 1
        TagAdd(pdA.m.data(), pdA.m.size());         // 2
        TagAdd(wiA.m.data(), wiA.m.size());         // 3
        TagAdd(entA.m.data(), entA.m.size());       // 4
        TagAdd(ematA.m.data(), ematA.m.size());     // 5
        TagAdd(vehA.m.data(), vehA.m.size());       // 6
        TagAdd(vmatA.m.data(), vmatA.m.size());     // 7
        TagAdd(miA.m.data(), miA.m.size());         // 8
        TagAdd(miB.m.data(), miB.m.size());         // 9
        TagAdd(colA.m.data(), colA.m.size());       // 10
        TagAdd(colB.m.data(), colB.m.size());       // 11
        TagAdd(clumpA.m.data(), clumpA.m.size());   // 12
        TagAdd(boneA.m.data(), boneA.m.size());     // 13
        TagAdd(hierA.m.data(), hierA.m.size());     // 14
        TagAdd(maA.m.data(), maA.m.size());         // 15
        TagAdd(assocA.m.data(), assocA.m.size());   // 16
        TagAdd(pmatA.m.data(), pmatA.m.size());     // 17
        // tarea
        taskA.Put8(0x14, c.fdir);
        taskA.Put32(0x2C, c.assoc ? u32(assocA.Obj()) : 0u);
        taskA.Put32(0x30, u32(wiA.Obj()));
        taskA.Put32(0x34, c.ent ? u32(entA.Obj()) : 0u);
        Put3(taskA, 0x38, c.coords);
        assocA.Put16(0x2C, c.animid);
        Put3(wiA, 0x24, c.fireoff);
        entA.Put32(0x14, u32(ematA.Obj()));
        PutN(ematA, 0, c.emat, 16);
        // ped
        pedA.Put32(0x18, u32(clumpA.Obj()));
        pedA.Put32(0x480, u32(pdA.Obj()));
        pedA.Put32(0x4A0, u32(boneA.Obj()));
        pedA.Put32(0x528, c.ikinit);
        pedA.Put32(0x58C, u32(vehA.Obj()));
        pedA.Put32(0x598, c.ptype);
        pedA.Put8(0x718, c.slot);
        pedA.Put32(0x14, c.pmat ? u32(pmatA.Obj()) : 0u);
        if (c.pmat) { Put3(pmatA, 0x30, c.ppos); Put3(pedA, 4, c.palt); }
        else { Put3(pedA, 4, c.ppos); Put3(pmatA, 0x30, c.palt); }
        pdA.Put32(0x2C, c.atkinit);
        boneA.Put32(0x14, c.bonetag);
        // vehiculo, modelos y cajas
        vehA.Put16(0x22, kModelBase + c.vmodel);
        vehA.Put32(0x14, u32(vmatA.Obj()));
        PutN(vmatA, 0, c.vmat, 16);
        miA.Put32(0x14, u32(colA.Obj())); miB.Put32(0x14, u32(colB.Obj()));
        PutN(colA, 0, c.boxA, 6); PutN(colB, 0, c.boxB, 6);
        for (unsigned k = 0; k < kModels; ++k) G32(kModelTableAddr + 4 * (kModelBase + k)) = (k & 1) ? u32(miB.Obj()) : u32(miA.Obj());
        // camaras
        G8(kActiveCamAddr) = uint8(c.cam);
        for (unsigned k = 0; k < 3; ++k) {
            G16(kCamsAddr + k * 0x238 + 0xC) = uint16(c.cmode[k]);
            G16(kCamsAddr + k * 0x238 + 0xE) = uint16(Rand(c.seed, 900 + k));
            for (unsigned j = 0; j < 3; ++j) G32(kCamsAddr + k * 0x238 + 0x19C + 4 * j) = c.csrc[k * 3 + j];
        }
        const std::vector<u32> args = { u32(pedA.Obj()) };
        SnapArenas();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, u32(taskA.Obj()), c.edx, c.stack, 0); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, taskA.Changes(0x2C, 0x30, 0x34, 0x38)); add(1, pedA.Changes()); add(2, pdA.Changes()); add(3, wiA.Changes()); add(4, entA.Changes()); add(5, ematA.Changes()); add(6, vehA.Changes());
        add(7, vmatA.Changes()); add(8, miA.Changes()); add(9, miB.Changes()); add(10, colA.Changes()); add(11, colB.Changes()); add(12, clumpA.Changes()); add(13, boneA.Changes()); add(14, hierA.Changes());
        add(15, maA.Changes()); add(16, assocA.Changes()); add(17, pmatA.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        obs["task_ptrs"] = json::array({taskA.Get32(0x2C) != 0, taskA.Get32(0x34) != 0});
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
    if (!std::filesystem::exists(dir / "fn89c-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn89c-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn89c"); }
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
    env.stubs[0] = {F(0x734A40, &FkHier), F(0x7C51A0, &FkIndex), F(0x7C5120, &FkMatArr), F(0x7EDD90, &FkXform), F(0x514970, &FkFind), F(0x742300, &FkFire), F(0x5DF340, &FkFlash)};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"lectura de camara activa y modo, jugador (m_nPedType), resta de vectores, rango del apuntado, matriz x punto, normalizacion, producto con el eje, corte con la caja del coche y comprobacion de coordenadas nulas"})},
                                 {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}

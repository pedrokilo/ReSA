#pragma once
// Banco dentro del motor: CWorld, lineas de vision y lineas verticales (grupo B, 16 funciones).
// Original (hooks del grupo desactivados) frente a candidato con fixtures deterministas:
//  - Recorridos de sectores (GetIsLineOfSightClear, ProcessLineOfSight, ProcessVerticalLine y su variante FillGlobeColPoints) y
//    FindGroundZForCoord/FindGroundZFor3DCoord/FindRoofZFor3DCoord/FindLowestZForCoord: las funciones de nivel inferior se
//    sustituyen por funciones falsas trazadas (se compara la secuencia exacta de sectores, lineas y marcas).
//  - Funciones de sector (GetIsLineOfSightSectorClear, ProcessLineOfSightSector, ProcessVerticalLineSector y FillGlobe): sectores y
//    sectores repetidos FALSOS; las funciones de lista se sustituyen (EXE y las cinco instanciaciones de la DLL).
//  - Funciones de lista (las cuatro *SectorList): listas con nodos REALES del pool de nodos dobles y entidades/modelos de colision/
//    matrices falsos; CCollision::TestLineOfSight/ProcessLineOfSight/ProcessVerticalLine, CameraToIgnoreThisObject, CEntity::GetColModel,
//    AnimatePedColModelSkinned, CPed::IsAlive y SetUpWheelColModel (tabla virtual falsa) se simulan y se trazan.
// Todo el estado global tocado se guarda y se restaura en cada ejecucion y al final.
#include <fstream>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "World.h"
#include "Collision.h"
#include "ColModel.h"
#include "ColPoint.h"
#include "CollisionData.h"
#include "Sector.h"
#include "RepeatSector.h"
#include "PtrListSingleLink.h"
#include "PtrListDoubleLink.h"
#include <Pools/Pools.h>
#include "PedModelInfo.h"
#include "Vehicle.h"
#include "Ped.h"
#include "Object.h"
#include "Building.h"
#include "Dummy.h"
#include "../ProbeRuntime.hpp"

namespace WldBLive {
using ReversingProbe::EntryPatch;

// acceso a los miembros protegidos de CWorld: la instanciacion explicita ignora el control de acceso
template<class Tag, typename Tag::type M> struct RobWB { friend typename Tag::type Get(Tag) { return M; } };
#define WLDB_TAG(NAME, SIG) struct NAME { using type = SIG; friend type Get(NAME); };
#define WLDB_ROB(NAME, FN) template struct RobWB<NAME, FN>;
using LBld = CPtrListSingleLink<CBuilding*>;
using LVeh = CPtrListDoubleLink<CVehicle*>;
using LPed = CPtrListDoubleLink<CPed*>;
using LObj = CPtrListDoubleLink<CObject*>;
using LDum = CPtrListDoubleLink<CDummy*>;
WLDB_TAG(ClearTag, void (*)())
WLDB_ROB(ClearTag, &CWorld::ClearScanCodes)
WLDB_TAG(GloscTag, bool (*)(CSector&, CRepeatSector&, const CColLine&, bool, bool, bool, bool, bool, bool, bool))
WLDB_ROB(GloscTag, &CWorld::GetIsLineOfSightSectorClear)
WLDB_TAG(PlossTag, bool (*)(CSector&, CRepeatSector&, const CColLine&, CColPoint&, float&, CEntity*&, bool, bool, bool, bool, bool, bool, bool, bool))
WLDB_ROB(PlossTag, &CWorld::ProcessLineOfSightSector)
WLDB_TAG(PvsTag, bool (*)(CSector&, CRepeatSector&, const CColLine&, CColPoint&, CEntity*&, bool, bool, bool, bool, bool, bool, CStoredCollPoly*))
WLDB_ROB(PvsTag, &CWorld::ProcessVerticalLineSector)
WLDB_TAG(PvfsTag, bool (*)(CSector&, CRepeatSector&, const CColLine&, CEntity*&, bool, bool, bool, bool, bool, bool, CStoredCollPoly*))
WLDB_ROB(PvfsTag, &CWorld::ProcessVerticalLineSector_FillGlobeColPoints)
#define WLDB_GL(S, L) WLDB_TAG(GlT##S, bool (*)(L&, const CColLine&, bool, bool)) WLDB_ROB(GlT##S, &CWorld::GetIsLineOfSightSectorListClear<L>)
#define WLDB_PL(S, L) WLDB_TAG(PlT##S, bool (*)(L&, const CColLine&, CColPoint&, float&, CEntity*&, bool, bool, bool)) WLDB_ROB(PlT##S, &CWorld::ProcessLineOfSightSectorList<L>)
#define WLDB_VL(S, L) WLDB_TAG(VlT##S, bool (*)(L&, const CColLine&, CColPoint&, float&, CEntity*&, bool, CStoredCollPoly*)) WLDB_ROB(VlT##S, &CWorld::ProcessVerticalLineSectorList<L>)
#define WLDB_FL(S, L) WLDB_TAG(FlT##S, bool (*)(L&, const CColLine&, CEntity*&, bool, CStoredCollPoly*)) WLDB_ROB(FlT##S, &CWorld::ProcessVerticalLineSectorList_FillGlobeColPoints<L>)
WLDB_GL(Bld, LBld) WLDB_GL(Veh, LVeh) WLDB_GL(Ped, LPed) WLDB_GL(Obj, LObj) WLDB_GL(Dum, LDum)
WLDB_PL(Bld, LBld) WLDB_PL(Veh, LVeh) WLDB_PL(Ped, LPed) WLDB_PL(Obj, LObj) WLDB_PL(Dum, LDum)
WLDB_VL(Bld, LBld) WLDB_VL(Veh, LVeh) WLDB_VL(Ped, LPed) WLDB_VL(Obj, LObj) WLDB_VL(Dum, LDum)
WLDB_FL(Bld, LBld) WLDB_FL(Veh, LVeh) WLDB_FL(Ped, LPed) WLDB_FL(Obj, LObj) WLDB_FL(Dum, LDum)
template<class Tag> inline uintptr_t Addr() { return reinterpret_cast<uintptr_t>(Get(Tag{})); }

inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p, size_t n, uint64 h = 1469598103934665603ull) { auto* b = static_cast<const uint8*>(p); for (size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 1099511628211ull; } return h; }
template<class T> inline T& At(void* base, size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base) + off); }

// --- direcciones del EXE ---
constexpr uintptr_t G_CROSSINGS = 0xB7CD60, G_SPREAD = 0xB7CD64, G_IGNORE = 0xB7CD68, G_BIKERS = 0xB7CD6F, G_TYRES = 0xB7CD70, G_DEADPEDS = 0xB7CD71,
                    G_SCAN = 0xB7CD78, G_FILLED = 0xB7CD7C, G_TEMPCP = 0xB9B250, G_COLCROSS = 0x9655D0, G_MODELPTRS = 0xA9B0C8, SECTORS = 0xB7D0B8, REPEATS = 0xB992B8;
constexpr int PEDMI = 19000;
template<class T> inline T& G(uintptr_t a) { return *reinterpret_cast<T*>(a); }

// --- buferes falsos ---
constexpr int NENT = 6;
alignas(16) inline std::array<std::array<uint8, 0x500>, NENT> ents{};   // entidades (peatones, vehiculos, objetos...)
alignas(16) inline std::array<std::array<uint8, 0x48>, NENT> mats{};     // CMatrix falsas
alignas(16) inline std::array<std::array<uint8, 0x30>, NENT> cms{};      // CColModel falsos
alignas(16) inline std::array<std::array<uint8, 0x30>, NENT> cds{};      // CCollisionData falsos
alignas(16) inline std::array<std::array<uint8, 6 * 0x14>, NENT> sphs{}; // esferas
alignas(16) inline std::array<uint8, 0x40> miFake{};
alignas(16) inline std::array<std::array<uint8, 8>, 2> fsec{};           // CSector falsos
alignas(16) inline std::array<std::array<uint8, 12>, 2> frep{};          // CRepeatSector falsos
alignas(16) inline std::array<uint8, 16> listHead{};
alignas(16) inline std::array<uint8, 0x30> markPoly{};
inline std::array<void*, 64> vt{};
inline std::array<CColModel*, NENT> colFor{}, pedColFor{};
inline std::vector<std::pair<const void*, size_t>> regions;
inline void Reg(const void* p, size_t n) { regions.emplace_back(p, n); }
inline json Id(const void* p) {
    if (!p) return nullptr; const auto u = uintptr_t(p);
    for (size_t k = 0; k < regions.size(); ++k) { const auto b = uintptr_t(regions[k].first); if (u >= b && u < b + regions[k].second) return json::array({ k, u - b }); }
    return "ext";
}

// --- fuentes de "azar" deterministas para las funciones falsas ---
inline uint32 maskA{}, callsA{}, lc{};
inline bool Bit() { return (maskA >> (callsA++ % 32)) & 1; }
inline float Factor() { static constexpr float t[] = { 0.8f, 0.5f, 0.95f, 0.3f, 1.0f, 1.2f, 0.65f, 0.1f }; return t[(lc++ * 5 + (maskA >> 9)) % 8]; }

// --- registro de recorridos (hash + primeras entradas) ---
inline uint64 walkHash{}; inline uint32 walkCalls{}; inline json walkFirst; inline int failAt{-1};
inline uint64 SecIdx(const void* p) { return (uintptr_t(p) - SECTORS) / 8; }
inline uint64 RepIdx(const void* p) { return (uintptr_t(p) - REPEATS) / 12; }
inline void LineRec(std::vector<uint64>& v, const CColLine& l) { for (float x : { l.m_vecStart.x, l.m_vecStart.y, l.m_vecStart.z, l.m_vecEnd.x, l.m_vecEnd.y, l.m_vecEnd.z }) v.push_back(F(x)); }
inline json LineJ(const CColLine& l) { return json::array({ F(l.m_vecStart.x), F(l.m_vecStart.y), F(l.m_vecStart.z), F(l.m_vecEnd.x), F(l.m_vecEnd.y), F(l.m_vecEnd.z) }); }
inline void WalkVec(const std::vector<uint64>& v) {
    for (auto x : v) walkHash = (walkHash ^ x) * 1099511628211ull;
    if (walkFirst.size() < 6) walkFirst.push_back(json(v));
    ++walkCalls;
}
inline void* fakeEnt{};

// ===== funciones falsas: recorridos =====
inline void __cdecl FakeClear() { trace.push_back({ "clear_scan_codes" }); }
inline bool __cdecl FakeGlosc(CSector& s, CRepeatSector& r, const CColLine& l, bool b0, bool b1, bool b2, bool b3, bool b4, bool st, bool cam) {
    std::vector<uint64> v{ SecIdx(&s), RepIdx(&r) }; LineRec(v, l); for (bool b : { b0, b1, b2, b3, b4, st, cam }) v.push_back(b);
    WalkVec(v); return !(failAt >= 0 && int(walkCalls) - 1 == failAt);
}
inline bool __cdecl FakePloss(CSector& s, CRepeatSector& r, const CColLine& l, CColPoint& cp, float& mx, CEntity*& e, bool b0, bool b1, bool b2, bool b3, bool b4, bool st, bool cam, bool shoot) {
    std::vector<uint64> v{ SecIdx(&s), RepIdx(&r) }; LineRec(v, l); for (bool b : { b0, b1, b2, b3, b4, st, cam, shoot }) v.push_back(b); v.push_back(F(mx));
    WalkVec(v); const bool hit = Bit();
    if (hit) { mx = mx * Factor(); cp.m_vecPoint.x = float(walkCalls); e = static_cast<CEntity*>(fakeEnt); G<int32>(G_CROSSINGS) += 2; }
    return hit;
}
inline bool __cdecl FakePvs(CSector& s, CRepeatSector& r, const CColLine& l, CColPoint& cp, CEntity*& e, bool b0, bool b1, bool b2, bool b3, bool b4, bool st, CStoredCollPoly* poly) {
    std::vector<uint64> v{ SecIdx(&s), RepIdx(&r) }; LineRec(v, l); for (bool b : { b0, b1, b2, b3, b4, st }) v.push_back(b); v.push_back(uintptr_t(poly));
    WalkVec(v); const bool hit = Bit();
    if (hit) { cp.m_vecPoint.x = float(walkCalls); e = static_cast<CEntity*>(fakeEnt); }
    return hit;
}
inline bool __cdecl FakePvfs(CSector& s, CRepeatSector& r, const CColLine& l, CEntity*& e, bool b0, bool b1, bool b2, bool b3, bool b4, bool st, CStoredCollPoly* poly) {
    std::vector<uint64> v{ SecIdx(&s), RepIdx(&r) }; LineRec(v, l); for (bool b : { b0, b1, b2, b3, b4, st }) v.push_back(b); v.push_back(uintptr_t(poly)); v.push_back(G<uint32>(G_FILLED));
    WalkVec(v); return Bit();
}
// ProcessVerticalLine falsa para los Find*Z
inline float colZ{};
inline bool __cdecl FakeVertical(const CVector& o, float d, CColPoint& cp, CEntity*& e, bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, CStoredCollPoly* poly) {
    const bool r = Bit(); trace.push_back({ "vertical_line", F(o.x), F(o.y), F(o.z), F(d), b0, b1, b2, b3, b4, b5, uintptr_t(poly), r });
    if (r) { cp.m_vecPoint = CVector{ o.x, o.y, colZ }; e = static_cast<CEntity*>(fakeEnt); }
    return r;
}

// ===== funciones falsas: listas (sustituyen a las de lista en las pruebas de sector) =====
inline bool __cdecl FakeGlList(void* list, const CColLine& l, bool st, bool cam) {
    trace.push_back({ "gl_list", Id(list), LineJ(l), st, cam }); return !(failAt >= 0 && int(trace.size()) - 1 == failAt);
}
inline bool __cdecl FakePlList(void* list, const CColLine& l, CColPoint& cp, float& mn, CEntity*& e, bool st, bool cam, bool shoot) {
    trace.push_back({ "pl_list", Id(list), LineJ(l), F(mn), st, cam, shoot, G<uint8>(G_DEADPEDS), G<uint8>(G_BIKERS), F(G<float>(G_SPREAD)) });
    const bool hit = Bit();
    if (hit) { mn = mn * Factor(); cp.m_vecPoint.x = float(trace.size()); e = static_cast<CEntity*>(fakeEnt); }
    return hit;
}
inline bool __cdecl FakePvList(void* list, const CColLine& l, CColPoint& cp, float& mx, CEntity*& e, bool st, CStoredCollPoly* poly) {
    trace.push_back({ "pv_list", Id(list), LineJ(l), F(mx), st, uintptr_t(poly) });
    const bool hit = Bit();
    if (hit) { mx = mx * Factor(); cp.m_vecPoint.x = float(trace.size()); e = static_cast<CEntity*>(fakeEnt); }
    return hit;
}
inline bool __cdecl FakeFlList(void* list, const CColLine& l, CEntity*& e, bool st, CStoredCollPoly* poly) {
    trace.push_back({ "fl_list", Id(list), LineJ(l), uintptr_t(e), st, uintptr_t(poly) }); return Bit();
}

// ===== funciones falsas: entidades y colision (pruebas de lista) =====
inline void FillCp(CColPoint& cp, uint32 k) {
    auto* f = reinterpret_cast<float*>(&cp); auto* w = reinterpret_cast<uint32*>(&cp);
    for (int i = 0; i < 11; ++i) w[i] = 0x01010101u * (uint32(k + i) & 0x7F);
    f[0] = float(k) * 0.5f + 1.f; f[1] = float(k) * -0.25f + 2.f; f[2] = float(k) * 0.125f - 1.f; f[4] = 0.25f; f[5] = -0.5f; f[6] = 0.75f; f[10] = 0.125f * float(k);
}
inline uint64 ColHash(const CColModel* cm) {
    if (!cm) return 0; uint64 h = Fnv(cm, 0x28);
    auto* cd = At<uint8*>(const_cast<CColModel*>(cm), 0x2C);
    if (cd) { const int n = int16(At<uint16>(cd, 0)); auto* sp = At<uint8*>(cd, 8); h ^= n; for (int i = 0; i < n && i < 6 && sp; ++i) h = Fnv(sp + i * 0x14, 0x14, h); }
    return h;
}
inline bool __cdecl FakeTestLos(const CColLine& l, const CMatrix& m, CColModel& cm, bool st, bool shoot) {
    const bool r = Bit(); trace.push_back({ "test_los", LineJ(l), Id(&m), Id(&cm), st, shoot, r }); return r;
}
inline bool __cdecl FakeProcLos(const CColLine& l, const CMatrix& m, CColModel& cm, CColPoint& cp, float& mx, bool st, bool shoot) {
    trace.push_back({ "proc_los", LineJ(l), Id(&m), Id(&cm), ColHash(&cm), F(mx), st, shoot });
    const bool hit = Bit();
    if (hit) { mx = mx * Factor(); FillCp(cp, lc++); G<uint32>(G_COLCROSS) = 1 + (lc & 3); }
    return hit;
}
inline bool __cdecl FakeProcVert(const CColLine& l, const CMatrix& m, CColModel& cm, CColPoint& cp, float& mx, bool st, bool shoot, CStoredCollPoly* poly) {
    trace.push_back({ "proc_vert", LineJ(l), Id(&m), Id(&cm), F(mx), st, shoot, uintptr_t(poly) });
    const bool hit = Bit();
    if (hit) {
        // el punto cae entre y fuera del segmento para que la logica de FillGlobe pruebe todas las ramas
        const float t = (float((lc++ * 7 + (maskA >> 5)) % 15) - 2.f) / 10.f;
        FillCp(cp, lc++); cp.m_vecPoint.z = l.m_vecStart.z + (l.m_vecEnd.z - l.m_vecStart.z) * t; mx = mx * Factor(); G<uint32>(G_COLCROSS) = 1 + (lc & 3);
    }
    return hit;
}
inline bool __cdecl FakeCamIgnore(CEntity* e) { const bool r = Bit(); trace.push_back({ "camera_ignore", Id(e), r }); return r; }
inline CColModel* __fastcall FakeGetColModel(void* self, void*) {
    const int k = int((uintptr_t(self) - uintptr_t(ents[0].data())) / 0x500); trace.push_back({ "get_col_model", Id(self) });
    return (k >= 0 && k < NENT) ? colFor[k] : nullptr;
}
inline CColModel* __fastcall FakePedCol(void* self, void*, RpClump* clump) {
    // Como el original: el EXE usa la model info de la ranura; la DLL llama al miembro con el mismo puntero
    const int k = int((uintptr_t(clump) - 0x3000)); trace.push_back({ "ped_col_skinned", uintptr_t(self) - uintptr_t(miFake.data()), uintptr_t(clump) });
    return (k >= 0 && k < NENT) ? pedColFor[k] : nullptr;
}
inline bool __fastcall FakeIsAlive(void* self, void*) { const bool r = Bit(); trace.push_back({ "is_alive", Id(self), r }); return r; }
inline bool __fastcall FakeSetUpWheel(void* self, void*, CColModel* wm) {
    auto* cd = At<uint8*>(wm, 0x2C); uint64 hd = 0;
    if (cd) { hd = Fnv(cd, 7); { const uint8 fb = cd[7] & 7; hd = Fnv(&fb, 1, hd); } for (size_t o : { 0xC, 0x10, 0x14, 0x18, 0x1C }) hd = Fnv(cd + o, 4, hd);
        auto* sp = At<uint8*>(cd, 8); if (sp) { auto* q = reinterpret_cast<float*>(sp); for (int i = 0; i < 6; ++i) { for (int j = 0; j < 5; ++j) q[i * 5 + j] = 0.1f * float(i + 1) + 0.01f * float(j); } } }
    { auto* b = reinterpret_cast<float*>(wm); for (int i = 0; i < 10; ++i) b[i] = 0.5f * float(i) - 2.f; }
    const bool r = Bit(); trace.push_back({ "wheel_setup", Id(self), hd, r }); return r;
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls, const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if (c.Name() == cls) for (auto& h : c.Items()) if (h->Name() == fn) r = h.get(); });
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn, 16> fns{ {
    {"CWorld","FindGroundZForCoord",0x569660,0,-1,128},
    {"CWorld","FindGroundZFor3DCoord",0x5696C0,0,-1,256},
    {"CWorld","FindRoofZFor3DCoord",0x569750,0,-1,256},
    {"CWorld","FindLowestZForCoord",0x5697F0,0,-1,128},
    {"CWorld","GetIsLineOfSightClear",0x56A490,0,-1,1024},
    {"CWorld","GetIsLineOfSightSectorClear",0x568AD0,0,-1,256},
    {"CWorld","GetIsLineOfSightSectorListClear<CPtrListDoubleLink<CPhysical*>>",0x564970,0,-1,512},
    {"CWorld","ProcessLineOfSight",0x56BA00,0,-1,1024},
    {"CWorld","ProcessLineOfSightSector",0x56B5E0,0,-1,512},
    {"CWorld","ProcessLineOfSightSectorList<CPtrListDoubleLink<CPhysical*>>",0x566EE0,0,-1,2048},
    {"CWorld","ProcessVerticalLine",0x5674E0,0,-1,512},
    {"CWorld","ProcessVerticalLineSector",0x564500,0,-1,256},
    {"CWorld","ProcessVerticalLineSectorList<CPtrListDoubleLink<CPhysical*>>",0x5632B0,0,-1,512},
    {"CWorld","ProcessVerticalLineSectorList_FillGlobeColPoints<CPtrListDoubleLink<CPhysical*>>",0x5636A0,0,-1,1024},
    {"CWorld","ProcessVerticalLineSector_FillGlobeColPoints",0x564420,0,-1,256},
    {"CWorld","ProcessVerticalLine_FillGlobeColPoints",0x567620,0,-1,512},
} };

// conjunto de parches de entrada con direcciones unicas
struct StubSet {
    std::vector<std::unique_ptr<EntryPatch>> p; std::set<uintptr_t> seen;
    template<class Fx> void Add(uintptr_t at, Fx f) { if (!at || !seen.insert(at).second) return; p.push_back(std::make_unique<EntryPatch>(at, f)); }
};

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "wldb-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "wldb-runtime.jsonl"); Write({ {"event","waiting_for_player"} }); ReversingProbe::InstallFailureCapture("wldb"); }
    auto* player = FindPlayerPed(); if (!player || CCutsceneMgr::IsRunning()) return; done = true;
    std::string only; { std::ifstream f(dir / "wldb-only.txt"); if (f) std::getline(f, only); }
    std::array<ReversibleHooks::ReversibleHook::Base*, 16> hooks{};
    for (size_t f = 0; f < fns.size(); ++f) if (!(hooks[f] = FindHook(fns[f].cls, fns[f].name))) { Write({ {"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls) + "::" + fns[f].name} }); return; }
    using std::bit_cast;
    std::array<bool, 16> enabled{}, locked{};
    for (size_t f = 0; f < fns.size(); ++f) { enabled[f] = hooks[f]->Hooked(); locked[f] = hooks[f]->Locked(); hooks[f]->LockState(false); }
    if (regions.empty()) {
        for (auto& b : ents) Reg(b.data(), b.size()); for (auto& b : mats) Reg(b.data(), b.size()); for (auto& b : cms) Reg(b.data(), b.size());
        for (auto& b : fsec) Reg(b.data(), b.size()); for (auto& b : frep) Reg(b.data(), b.size()); Reg(listHead.data(), listHead.size());
    }
    for (auto& v : vt) v = nullptr; vt[43] = reinterpret_cast<void*>(&FakeSetUpWheel);   // CVehicle::SetUpWheelColModel (0xAC / 4)
    fakeEnt = ents[3].data();

    // estado global guardado
    auto Snap = [](uintptr_t a, size_t n) { auto* p = reinterpret_cast<uint8*>(a); return std::vector<uint8>(p, p + n); };
    auto Back = [](uintptr_t a, const std::vector<uint8>& v) { std::memcpy(reinterpret_cast<void*>(a), v.data(), v.size()); };
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for (auto [a, n] : std::initializer_list<std::pair<uintptr_t, size_t>>{ {G_CROSSINGS,0x24},{G_TEMPCP,32 * 0x2C},{G_COLCROSS,4},{G_MODELPTRS + 4 * PEDMI,4} })
        saved.push_back({ a, Snap(a, n) });
    // pool REAL de nodos dobles: las listas usan nodos de verdad (la DLL valida cada nodo contra este pool)
    auto* const nodePoolObj = GetPtrNodeDoubleLinkPool(); auto* const poolHdr = reinterpret_cast<uint8*>(nodePoolObj);
    const size_t nodeCap = At<uint32>(poolHdr, 8); auto* const nodeSlots = At<uint8*>(poolHdr, 4); const uintptr_t nodeBase = uintptr_t(At<uint8*>(poolHdr, 0)); const size_t nodeBytes = nodeCap * 12;
    const std::vector<uint8> savedPoolHdr(poolHdr, poolHdr + 0x14), savedSlots(nodeSlots, nodeSlots + nodeCap),
                             savedNodes(reinterpret_cast<uint8*>(nodeBase), reinterpret_cast<uint8*>(nodeBase) + nodeBytes);
    auto RestoreAll = [&] {
        for (auto& s : saved) Back(s.a, s.v);
        std::memcpy(poolHdr, savedPoolHdr.data(), savedPoolHdr.size()); std::memcpy(nodeSlots, savedSlots.data(), savedSlots.size());
        std::memcpy(reinterpret_cast<uint8*>(nodeBase), savedNodes.data(), savedNodes.size());
    };

    bool passed = true; int pair = 0;
    {
        // direcciones de la DLL de las funciones protegidas / instanciaciones por tipo de lista
        const uintptr_t dClear = Addr<ClearTag>(), dGlosc = Addr<GloscTag>(), dPloss = Addr<PlossTag>(), dPvs = Addr<PvsTag>(), dPvfs = Addr<PvfsTag>();
        const std::array<uintptr_t, 5> dGl{ Addr<GlTBld>(), Addr<GlTVeh>(), Addr<GlTPed>(), Addr<GlTObj>(), Addr<GlTDum>() };
        const std::array<uintptr_t, 5> dPl{ Addr<PlTBld>(), Addr<PlTVeh>(), Addr<PlTPed>(), Addr<PlTObj>(), Addr<PlTDum>() };
        const std::array<uintptr_t, 5> dVl{ Addr<VlTBld>(), Addr<VlTVeh>(), Addr<VlTPed>(), Addr<VlTObj>(), Addr<VlTDum>() };
        const std::array<uintptr_t, 5> dFl{ Addr<FlTBld>(), Addr<FlTVeh>(), Addr<FlTPed>(), Addr<FlTObj>(), Addr<FlTDum>() };
        const uintptr_t dVert = bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine);
        const uintptr_t dTestLos = bit_cast<uintptr_t>(&CCollision::TestLineOfSight), dProcLos = bit_cast<uintptr_t>(&CCollision::ProcessLineOfSight), dProcVert = bit_cast<uintptr_t>(&CCollision::ProcessVerticalLine);
        const uintptr_t dCam = bit_cast<uintptr_t>(&CWorld::CameraToIgnoreThisObject), dGetCol = bit_cast<uintptr_t>(&CEntity::GetColModel);
        const uintptr_t dPedCol = bit_cast<uintptr_t>(&CPedModelInfo::AnimatePedColModelSkinned), dAlive = bit_cast<uintptr_t>(&CPed::IsAlive);

        for (size_t f = 0; f < fns.size(); ++f) {
            const auto& fn = fns[f]; int failures = 0; const std::string nm = fn.name, full = std::string(fn.cls) + "::" + nm;
            if (!only.empty() && only != full) continue;
            for (int i = 0; i < fn.cases; ++i) {
                uint32 stt{}; auto next = [&] { stt ^= stt << 13; stt ^= stt >> 17; stt ^= stt << 5; return stt; };
                auto uni = [&](float a, float b) { return a + (b - a) * float(next() % 1000000) / 1000000.f; };
                auto fill = [&](void* p, size_t n) { auto* b = static_cast<uint8*>(p); for (size_t k = 0; k < n; ++k) b[k] = uint8(next()); };
                auto mkMat = [&](void* m, const CVector& pos) { const float h = uni(-3.2f, 3.2f), s = std::sin(h), c = std::cos(h); auto* q = static_cast<float*>(m);
                    std::fill(q, q + 0x12, 0.f); q[0] = c; q[1] = s; q[2] = uni(-0.05f, 0.05f); q[4] = -s; q[5] = c; q[6] = uni(-0.05f, 0.05f); q[8] = uni(-0.05f, 0.05f); q[9] = uni(-0.05f, 0.05f); q[10] = 1.f;
                    q[12] = pos.x; q[13] = pos.y; q[14] = pos.z; At<void*>(m, 0x40) = nullptr; At<uint8>(m, 0x44) = 0; };
                // coordenadas de mundo: mayoria dentro del mapa, algunas fuera, otras justo en los bordes de sector
                auto coord = [&]() -> float { const uint32 r = next() % 10; if (r < 6) return uni(-3300.f, 3300.f); if (r < 7) return uni(-9000.f, 9000.f);
                    if (r < 9) { const float e = -3000.f + 50.f * float(next() % 121); const uint32 d = next() % 3; return d == 0 ? e : (d == 1 ? std::nextafter(e, -1e9f) : std::nextafter(e, 1e9f)); } return uni(-3010.f, 3010.f); };
                auto mkPair = [&](CVector& o, CVector& t) { o = CVector{ coord(), coord(), uni(-50.f, 100.f) }; t = CVector{ coord(), coord(), uni(-50.f, 100.f) };
                    switch (next() % 6) { case 0: t.x = o.x + uni(-20.f, 20.f); t.y = o.y + uni(-20.f, 20.f); break; case 1: t.x = o.x + uni(-10.f, 10.f); break; case 2: t.y = o.y + uni(-10.f, 10.f); break; default: break; } };
                auto run = [&](bool candidate) -> json {
                    stt = uint32(i) * 2654435761u ^ uint32(f * 0x9E3779B9u) ^ 0x5742u; if (!stt) stt = 1; trace = json::array(); lc = 0;
                    maskA = next(); callsA = 0; if (next() % 6 == 0) maskA = ~0u; if (next() % 6 == 0) maskA = 0;
                    walkHash = 1469598103934665603ull; walkCalls = 0; walkFirst = json::array(); failAt = -1;
                    // estado global por defecto
                    G<int32>(G_CROSSINGS) = 77; G<float>(G_SPREAD) = 0.f; G<CEntity*>(G_IGNORE) = nullptr; G<uint8>(G_BIKERS) = 0; G<uint8>(G_TYRES) = 0; G<uint8>(G_DEADPEDS) = 0;
                    G<uint16>(G_SCAN) = (next() % 8 == 0) ? uint16(0xFFFF) : uint16(1 + next() % 0xFFF0); G<uint32>(G_FILLED) = 55; G<uint32>(G_COLCROSS) = 9;
                    std::memset(reinterpret_cast<void*>(G_TEMPCP), 0xCD, 32 * 0x2C);
                    for (auto* h : hooks) h->State(false);
                    hooks[f]->State(candidate);
                    const uintptr_t a = fn.address;
                    const bool switches = (ReversingProbe::JumpsToDll(fn.address) == candidate);
                    json out = { {"switches", switches} }; json ret = nullptr;
                    StubSet stubs;
                    auto walkOut = [&] { out["calls"] = walkCalls; out["hash"] = walkHash; out["first"] = walkFirst; out["scan"] = G<uint16>(G_SCAN); out["crossings"] = G<int32>(G_CROSSINGS); out["filled"] = G<uint32>(G_FILLED); };
                    auto sentinelCp = [&](CColPoint& cp) { std::memset(&cp, 0xAB, sizeof(cp)); };

                    if (nm == "FindGroundZForCoord" || nm == "FindLowestZForCoord" || nm == "FindGroundZFor3DCoord" || nm == "FindRoofZFor3DCoord") {
                        stubs.Add(0x5674E0, FakeVertical); stubs.Add(dVert, FakeVertical);
                        colZ = uni(-60.f, 220.f); const float x = coord(), y = coord(), z = uni(-100.f, 300.f);
                        if (nm == "FindGroundZForCoord") ret = F(reinterpret_cast<float(__cdecl*)(float, float)>(a)(x, y));
                        else if (nm == "FindLowestZForCoord") ret = F(reinterpret_cast<float(__cdecl*)(float, float)>(a)(x, y));
                        else if (nm == "FindGroundZFor3DCoord") {
                            uint8 res = 0xEE; CEntity* pe = reinterpret_cast<CEntity*>(0x1234); const uint32 m = next() % 4;
                            ret = F(reinterpret_cast<float(__cdecl*)(CVector, bool*, CEntity**)>(a)(CVector{ x, y, z }, (m & 1) ? reinterpret_cast<bool*>(&res) : nullptr, (m & 2) ? &pe : nullptr));
                            out["res"] = res; out["ent"] = (m & 2) ? Id(pe) : json(nullptr);
                        } else {
                            uint8 res = 0xEE; const bool withRes = next() % 5 != 0;
                            ret = F(reinterpret_cast<float(__cdecl*)(float, float, float, bool*)>(a)(x, y, z, withRes ? reinterpret_cast<bool*>(&res) : nullptr));
                            out["res"] = res;
                        }
                    }
                    else if (nm == "GetIsLineOfSightClear") {
                        stubs.Add(0x568AD0, FakeGlosc); stubs.Add(dGlosc, FakeGlosc); stubs.Add(0x563470, FakeClear); stubs.Add(dClear, FakeClear);
                        CVector o, t; mkPair(o, t); failAt = (next() % 3 == 0) ? int(next() % 8) : -1;
                        const uint32 fl = next();
                        ret = reinterpret_cast<bool(__cdecl*)(const CVector&, const CVector&, bool, bool, bool, bool, bool, bool, bool)>(a)(o, t, fl & 1, fl & 2, fl & 4, fl & 8, fl & 16, fl & 32, fl & 64);
                        walkOut();
                    }
                    else if (nm == "ProcessLineOfSight") {
                        stubs.Add(0x56B5E0, FakePloss); stubs.Add(dPloss, FakePloss); stubs.Add(0x563470, FakeClear); stubs.Add(dClear, FakeClear);
                        CVector o, t; mkPair(o, t); CColPoint cp; sentinelCp(cp); CEntity* e = reinterpret_cast<CEntity*>(ents[2].data()); const uint32 fl = next();
                        ret = reinterpret_cast<bool(__cdecl*)(const CVector&, const CVector&, CColPoint&, CEntity*&, bool, bool, bool, bool, bool, bool, bool, bool)>(a)(o, t, cp, e, fl & 1, fl & 2, fl & 4, fl & 8, fl & 16, fl & 32, fl & 64, fl & 128);
                        walkOut(); out["cp"] = ReversingProbe::Bytes(&cp, sizeof(cp)); out["ent"] = Id(e);
                    }
                    else if (nm == "ProcessVerticalLine") {
                        stubs.Add(0x564500, FakePvs); stubs.Add(dPvs, FakePvs); stubs.Add(0x563470, FakeClear); stubs.Add(dClear, FakeClear);
                        const CVector o{ coord(), coord(), uni(-50.f, 100.f) }; const float d = uni(-1200.f, 1200.f); CColPoint cp; sentinelCp(cp); CEntity* e = reinterpret_cast<CEntity*>(ents[2].data()); const uint32 fl = next();
                        CStoredCollPoly* poly = (fl & 128) ? reinterpret_cast<CStoredCollPoly*>(markPoly.data()) : nullptr;
                        ret = reinterpret_cast<bool(__cdecl*)(const CVector&, float, CColPoint&, CEntity*&, bool, bool, bool, bool, bool, bool, CStoredCollPoly*)>(a)(o, d, cp, e, fl & 1, fl & 2, fl & 4, fl & 8, fl & 16, fl & 32, poly);
                        walkOut(); out["cp"] = ReversingProbe::Bytes(&cp, sizeof(cp)); out["ent"] = Id(e);
                    }
                    else if (nm == "ProcessVerticalLine_FillGlobeColPoints") {
                        stubs.Add(0x564420, FakePvfs); stubs.Add(dPvfs, FakePvfs); stubs.Add(0x563470, FakeClear); stubs.Add(dClear, FakeClear);
                        const CVector o{ coord(), coord(), uni(-50.f, 100.f) }; const float d = uni(-1200.f, 1200.f); CEntity* e = reinterpret_cast<CEntity*>(ents[2].data()); const uint32 fl = next();
                        CStoredCollPoly* poly = (fl & 64) ? reinterpret_cast<CStoredCollPoly*>(markPoly.data()) : nullptr;
                        ret = reinterpret_cast<bool(__cdecl*)(const CVector&, float, CEntity*&, bool, bool, bool, bool, bool, bool, CStoredCollPoly*)>(a)(o, d, e, fl & 1, fl & 2, fl & 4, fl & 8, fl & 16, fl & 32, poly);
                        walkOut(); out["ent"] = Id(e);
                    }
                    else if (nm == "GetIsLineOfSightSectorClear" || nm == "ProcessLineOfSightSector" || nm == "ProcessVerticalLineSector" || nm == "ProcessVerticalLineSector_FillGlobeColPoints") {
                        for (auto& b : fsec) fill(b.data(), b.size()); for (auto& b : frep) fill(b.data(), b.size());
                        CColLine line; line.m_vecStart = CVector{ uni(-300.f, 300.f), uni(-300.f, 300.f), uni(-50.f, 100.f) }; line.m_vecEnd = CVector{ uni(-300.f, 300.f), uni(-300.f, 300.f), uni(-50.f, 100.f) };
                        line.m_fStartSize = 0.f; line.m_fEndSize = 0.f;
                        const uint32 fl = next(); failAt = (next() % 3 == 0) ? int(next() % 5) : -1; CColPoint cp; sentinelCp(cp); CEntity* e = nullptr; float mx = uni(0.4f, 1.f);
                        CStoredCollPoly* poly = (fl & 256) ? reinterpret_cast<CStoredCollPoly*>(markPoly.data()) : nullptr;
                        auto& sec = *reinterpret_cast<CSector*>(fsec[next() % 2].data()); auto& rep = *reinterpret_cast<CRepeatSector*>(frep[next() % 2].data());
                        if (nm == "GetIsLineOfSightSectorClear") {
                            stubs.Add(0x564970, FakeGlList); for (auto x : dGl) stubs.Add(x, FakeGlList);
                            ret = reinterpret_cast<bool(__cdecl*)(CSector&, CRepeatSector&, const CColLine&, bool, bool, bool, bool, bool, bool, bool)>(a)(sec, rep, line, fl & 1, fl & 2, fl & 4, fl & 8, fl & 16, fl & 32, fl & 64);
                        } else if (nm == "ProcessLineOfSightSector") {
                            stubs.Add(0x566EE0, FakePlList); for (auto x : dPl) stubs.Add(x, FakePlList);
                            const float sprs[] = { 0.f, 0.25f, 0.125f, -0.5f, std::bit_cast<float>(0x7FC00000u), 1.f };
                            G<float>(G_SPREAD) = sprs[next() % 6]; G<uint8>(G_DEADPEDS) = next() & 1; G<uint8>(G_BIKERS) = next() & 1;
                            e = reinterpret_cast<CEntity*>(ents[2].data());
                            ret = reinterpret_cast<bool(__cdecl*)(CSector&, CRepeatSector&, const CColLine&, CColPoint&, float&, CEntity*&, bool, bool, bool, bool, bool, bool, bool, bool)>(a)(sec, rep, line, cp, mx, e, fl & 1, fl & 2, fl & 4, fl & 8, fl & 16, fl & 32, fl & 64, fl & 128);
                            out["mx"] = F(mx); out["cp"] = ReversingProbe::Bytes(&cp, sizeof(cp)); out["ent"] = Id(e);
                            out["globals"] = json::array({ G<uint8>(G_DEADPEDS), G<uint8>(G_BIKERS), F(G<float>(G_SPREAD)) });
                        } else if (nm == "ProcessVerticalLineSector") {
                            stubs.Add(0x5632B0, FakePvList); for (auto x : dVl) stubs.Add(x, FakePvList);
                            ret = reinterpret_cast<bool(__cdecl*)(CSector&, CRepeatSector&, const CColLine&, CColPoint&, CEntity*&, bool, bool, bool, bool, bool, bool, CStoredCollPoly*)>(a)(sec, rep, line, cp, e, fl & 1, fl & 2, fl & 4, fl & 8, fl & 16, fl & 32, poly);
                            out["cp"] = ReversingProbe::Bytes(&cp, sizeof(cp)); out["ent"] = Id(e);
                        } else {
                            stubs.Add(0x5636A0, FakeFlList); for (auto x : dFl) stubs.Add(x, FakeFlList);
                            ret = reinterpret_cast<bool(__cdecl*)(CSector&, CRepeatSector&, const CColLine&, CEntity*&, bool, bool, bool, bool, bool, bool, CStoredCollPoly*)>(a)(sec, rep, line, e, fl & 1, fl & 2, fl & 4, fl & 8, fl & 16, fl & 32, poly);
                        }
                    }
                    else {
                        // ===== funciones de lista: nodos reales, entidades y modelos de colision falsos =====
                        stubs.Add(0x417730, FakeTestLos); stubs.Add(dTestLos, FakeTestLos); stubs.Add(0x417950, FakeProcLos); stubs.Add(dProcLos, FakeProcLos);
                        stubs.Add(0x417BF0, FakeProcVert); stubs.Add(dProcVert, FakeProcVert); stubs.Add(0x563F40, FakeCamIgnore); stubs.Add(dCam, FakeCamIgnore);
                        stubs.Add(0x535300, FakeGetColModel); stubs.Add(dGetCol, FakeGetColModel); stubs.Add(0x4C6F70, FakePedCol); stubs.Add(dPedCol, FakePedCol);
                        stubs.Add(0x5E0170, FakeIsAlive); stubs.Add(dAlive, FakeIsAlive);
                        const uint16 curScan = G<uint16>(G_SCAN) == 0xFFFF ? uint16(0x4000) : G<uint16>(G_SCAN); G<uint16>(G_SCAN) = curScan;
                        G<void*>(G_MODELPTRS + 4 * PEDMI) = miFake.data();
                        const bool isProc = nm.rfind("ProcessLineOfSight", 0) == 0;
                        // tipo de lista: 0 peatones, 1 vehiculos, 2 objetos/otros, 3 mezclada
                        const int kind = (nm.rfind("ProcessLineOfSight", 0) == 0) ? int(next() % 4) : int(next() % 3);
                        const int n = int(next() % 7 == 0 ? 0 : 1 + next() % NENT);
                        std::array<int, NENT> order{}; for (int k = 0; k < NENT; ++k) order[k] = k;
                        for (int k = NENT - 1; k > 0; --k) std::swap(order[k], order[next() % (k + 1)]);
                        CVector c0{ uni(-40.f, 40.f), uni(-40.f, 40.f), uni(-5.f, 20.f) };
                        for (int k = 0; k < NENT; ++k) {
                            void* e = ents[k].data(); fill(e, 0x500);
                            const int type = kind == 0 ? 3 : kind == 1 ? 2 : kind == 2 ? int(1 + (next() % 3 == 0 ? 3 : (next() % 2 ? 0 : 4))) : int(1 + next() % 5);
                            At<void*>(e, 0) = vt.data();
                            for (size_t o : { 4, 8, 12 }) At<float>(e, o) = uni(-40.f, 40.f);
                            mkMat(mats[k].data(), CVector{ c0.x + uni(-15.f, 15.f), c0.y + uni(-15.f, 15.f), c0.z + uni(-5.f, 5.f) }); At<void*>(e, 0x14) = mats[k].data();
                            At<void*>(e, 0x18) = reinterpret_cast<void*>(0x3000 + k);
                            { uint32 fl = next(); fl = (next() % 5 != 0) ? (fl | 1u) : (fl & ~1u); At<uint32>(e, 0x1C) = fl; }
                            At<int16>(e, 0x22) = int16(type == 3 ? PEDMI : int(next() % 1000));
                            At<uint16>(e, 0x2C) = (next() % 4 == 0) ? curScan : uint16(curScan + 1 + next() % 100); // algunas ya tienen el codigo actual
                            At<uint8>(e, 0x36) = uint8((next() & 0xF8u) | uint32(type));
                            At<void*>(e, 0xFC) = (next() % 4 == 0) ? ents[(k + 1) % NENT].data() : nullptr;
                            { uint32 pf = next(); pf = (next() % 3 == 0) ? (pf | 0x100000u) : (pf & ~0x100000u); At<uint32>(e, 0x478) = pf; }
                            // modelo de colision
                            auto* cm = cms[k].data(); fill(cm, 0x30);
                            { auto* q = reinterpret_cast<float*>(cm); for (int j = 0; j < 10; ++j) q[j] = uni(-5.f, 5.f); }
                            auto* cd = cds[k].data(); fill(cd, 0x30); const uint32 ns = next() % 9;
                            At<uint16>(cd, 0) = ns == 7 ? uint16(0xFFFE) : ns == 8 ? uint16(0) : uint16(next() % 7); At<void*>(cd, 8) = sphs[k].data();
                            { auto* q = reinterpret_cast<float*>(sphs[k].data()); for (int j = 0; j < 30; ++j) q[j] = uni(0.1f, 3.f); }
                            At<void*>(cm, 0x2C) = (next() % 7 == 0) ? nullptr : cd;
                            colFor[k] = (next() % 9 == 0) ? nullptr : reinterpret_cast<CColModel*>(cm);
                            pedColFor[k] = (next() % 9 == 0) ? nullptr : reinterpret_cast<CColModel*>(cm);
                        }
                        // lista con nodos reales del pool de nodos dobles
                        CColLine line; line.m_vecStart = CVector{ c0.x + uni(-30.f, 30.f), c0.y + uni(-30.f, 30.f), c0.z + uni(-10.f, 10.f) };
                        line.m_vecEnd = CVector{ c0.x + uni(-30.f, 30.f), c0.y + uni(-30.f, 30.f), c0.z + uni(-30.f, 30.f) }; line.m_fStartSize = 0.f; line.m_fEndSize = 0.f;
                        At<void*>(listHead.data(), 0) = nullptr;
                        std::array<uint32*, NENT> nodes{};
                        for (int k = 0; k < n; ++k) { nodes[k] = reinterpret_cast<uint32*>(nodePoolObj->New()); if (!nodes[k]) { nodes[k] = nullptr; break; } nodes[k][0] = uint32(uintptr_t(ents[order[k]].data())); }
                        int built = 0; for (int k = 0; k < n && nodes[k]; ++k) ++built;
                        for (int k = 0; k < built; ++k) { nodes[k][1] = k + 1 < built ? uint32(uintptr_t(nodes[k + 1])) : 0; nodes[k][2] = k ? uint32(uintptr_t(nodes[k - 1])) : 0; }
                        if (built) At<void*>(listHead.data(), 0) = nodes[0];
                        void* const list = listHead.data();
                        const uint32 fl = next(); CStoredCollPoly* poly = (fl & 16) ? reinterpret_cast<CStoredCollPoly*>(markPoly.data()) : nullptr;
                        CColPoint cp; sentinelCp(cp); CEntity* e = reinterpret_cast<CEntity*>(ents[5].data()); float mx = uni(0.3f, 1.f);
                        if (fl & 0x100) G<CEntity*>(G_IGNORE) = reinterpret_cast<CEntity*>(ents[order[next() % NENT]].data());
                        if (isProc) {
                            const float sprs[] = { 0.f, 0.f, 0.05f, 0.2f, -0.5f, std::bit_cast<float>(0x7FC00000u) };
                            G<float>(G_SPREAD) = sprs[next() % 6]; G<uint8>(G_DEADPEDS) = next() % 3 == 0; G<uint8>(G_BIKERS) = next() % 3 == 0; G<uint8>(G_TYRES) = next() % 3 != 0;
                        }
                        // las entidades se vuelcan antes y despues (codigo de exploracion, etc.)
                        if (nm.rfind("GetIsLineOfSightSectorListClear", 0) == 0) {
                            ret = reinterpret_cast<bool(__cdecl*)(void*, const CColLine*, bool, bool)>(a)(list, &line, fl & 1, fl & 2);
                        } else if (isProc) {
                            ret = reinterpret_cast<bool(__cdecl*)(void*, const CColLine*, CColPoint*, float*, CEntity**, bool, bool, bool)>(a)(list, &line, &cp, &mx, &e, fl & 1, fl & 2, fl & 4);
                            out["mx"] = F(mx); out["cp"] = ReversingProbe::Bytes(&cp, sizeof(cp)); out["ent"] = Id(e);
                            out["globals"] = json::array({ G<uint8>(G_DEADPEDS), G<uint8>(G_BIKERS), G<uint8>(G_TYRES), F(G<float>(G_SPREAD)) });
                        } else if (nm.rfind("ProcessVerticalLineSectorList_FillGlobeColPoints", 0) == 0) {
                            G<uint32>(G_FILLED) = (next() % 4 == 0) ? 30 + next() % 3 : next() % 4;
                            ret = reinterpret_cast<bool(__cdecl*)(void*, const CColLine*, CEntity**, bool, CStoredCollPoly*)>(a)(list, &line, &e, fl & 1, poly);
                            out["filled"] = G<uint32>(G_FILLED); out["pts"] = Fnv(reinterpret_cast<void*>(G_TEMPCP), 32 * 0x2C); out["ent"] = Id(e);
                        } else {
                            ret = reinterpret_cast<bool(__cdecl*)(void*, const CColLine*, CColPoint*, float*, CEntity**, bool, CStoredCollPoly*)>(a)(list, &line, &cp, &mx, &e, fl & 1, poly);
                            out["mx"] = F(mx); out["cp"] = ReversingProbe::Bytes(&cp, sizeof(cp)); out["ent"] = Id(e);
                        }
                        out["crossings"] = G<int32>(G_CROSSINGS); out["scan"] = G<uint16>(G_SCAN);
                        json ev = json::array(); for (int k = 0; k < NENT; ++k) ev.push_back({ Fnv(ents[k].data(), 0x40), Fnv(cms[k].data(), 0x30), Fnv(sphs[k].data(), sizeof(sphs[k])), Fnv(mats[k].data(), 0x48) });
                        out["ents"] = ev;
                    }
                    out["ret"] = ret; out["trace"] = trace;
                    RestoreAll();
                    return out;
                };
                const auto original = run(false), candidate = run(true);
                const bool ok = original == candidate && original["switches"] == true && candidate["switches"] == true;
                passed &= ok; failures += !ok;
                if (ok) Write({ {"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()} });
                else { json diff = json::array(); for (auto& [k, v] : original.items()) if (candidate[k] != v) diff.push_back(k);
                    Write({ {"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                            {"original",failures <= 2 ? original : json(nullptr)},{"candidate",failures <= 2 ? candidate : json(nullptr)} }); }
            }
            json summary = { {"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases} };
            Write(summary);
        }
    }
    for (size_t f = 0; f < fns.size(); ++f) { hooks[f]->State(enabled[f]); hooks[f]->LockState(locked[f]); }
    RestoreAll();
    bool restored = true; for (auto& s : saved) restored &= std::memcmp(reinterpret_cast<void*>(s.a), s.v.data(), s.v.size()) == 0;
    restored &= std::memcmp(poolHdr, savedPoolHdr.data(), savedPoolHdr.size()) == 0 && std::memcmp(nodeSlots, savedSlots.data(), savedSlots.size()) == 0
             && std::memcmp(reinterpret_cast<uint8*>(nodeBase), savedNodes.data(), savedNodes.size()) == 0;
    Write({ {"event", passed ? "complete" : "error"},{"pairs",pair},{"state_restored",restored} });
}
}

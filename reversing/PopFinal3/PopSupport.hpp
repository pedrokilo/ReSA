#pragma once
// Infraestructura del banco PopFinal3 (CPopulation): registro de trazas, parches de entrada, instantaneas de globales,
// objetos falsos (peatones, vehiculos, objetos, dummies, model infos, pools) y dobles de colaboradores.
#include <fstream>
#include <set>
#include <cmath>
#include <utility>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Population.h"
#include "World.h"
#include "Streaming.h"
#include "PopCycle.h"
#include "Pools/Pools.h"
#include "../ProbeRuntime.hpp"

namespace PopFinal3Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
inline uint64 Fnv(const void* p, size_t n, uint64 h = 1469598103934665603ull) { auto* b = static_cast<const uint8*>(p); for (size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 1099511628211ull; } return h; }
template<class T = uint32> T& At(void* base, size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base) + off); }
template<class T = uint32> T& AtA(uintptr_t a) { return *reinterpret_cast<T*>(a); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
struct Rng {
    uint32 s;
    uint32 next() { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s; }
    float uni(float a, float b) { return a + (b - a) * float(next() % 1000000) / 1000000.f; }
    bool pct(uint32 p) { return next() % 100 < p; }
    uint32 below(uint32 n) { return next() % n; }
    float pick(std::initializer_list<float> l) { return l.begin()[next() % l.size()]; }
    int32 pickI(std::initializer_list<int32> l) { return l.begin()[next() % l.size()]; }
};

// Hilo del banco: otros hilos del juego llaman a funciones parcheadas; no se trazan.
inline DWORD owner{};
inline bool Mine() { return GetCurrentThreadId() == owner; }

// --- buferes con nombre para identificar punteros en las trazas ---
struct Named { const void* p; size_t n; const char* name; };
inline std::vector<Named> named;
inline void Nm(const void* p, size_t n, const char* name) { named.push_back({ p, n, name }); }
inline json Name(uintptr_t v) {
    if (!v) return nullptr;
    for (auto& g : named) if (v >= uintptr_t(g.p) && v < uintptr_t(g.p) + g.n) return std::string(g.name) + "+" + std::to_string(v - uintptr_t(g.p));
    return "other";
}
template<class V> inline json Jv(V v) {
    if constexpr (std::is_floating_point_v<V>) return std::bit_cast<uint32>(float(v));
    else if constexpr (std::is_pointer_v<V>) return Name(reinterpret_cast<uintptr_t>(v));
    else if constexpr (std::is_enum_v<V>) return int64(v);
    else return v;
}
template<class... A> inline void T(const char* tag, A... a) { json e = json::array(); e.push_back(tag); (e.push_back(Jv(a)), ...); trace.push_back(std::move(e)); }

// --- parches de entrada sin duplicados (se deshacen en orden inverso) ---
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; std::set<uintptr_t> seen;
    template<class Fp> void Add(uintptr_t at, Fp f) { if (at && seen.insert(at).second) v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at, f)); }
    ~Patches() { while (!v.empty()) v.pop_back(); }
};

// --- regiones globales guardadas ---
struct Region { void* p; size_t n; std::vector<uint8> saved; };
inline std::vector<Region> regions;
inline void Snap(uintptr_t p, size_t n) { regions.push_back({ reinterpret_cast<void*>(p), n, ReversingProbe::Bytes(reinterpret_cast<void*>(p), n) }); }
inline void RestoreRegions() { for (auto& g : regions) std::memcpy(g.p, g.saved.data(), g.n); }
inline bool RegionsRestored() { for (auto& g : regions) if (std::memcmp(g.p, g.saved.data(), g.n)) return false; return true; }

// --- modulo de la DLL y busqueda de cuerpos ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule() { HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(&InitModule), &mod);
    modBase = uintptr_t(mod); const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase + reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd = modBase + nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a) { return a >= modBase && a < modEnd; }
inline uintptr_t FollowIn(uintptr_t q) { for (int k = 0; k < 8 && InDll(q) && *reinterpret_cast<const uint8*>(q) == 0xE9; ++k) { const uintptr_t t = q + 5 + *reinterpret_cast<const int32*>(q + 1); if (!InDll(t)) break; q = t; } return q; }
template<class Fp> inline uintptr_t Body(Fp f) { return FollowIn(Addr(f)); }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls, const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if (c.Name() == cls) for (auto& h : c.Items()) if (h->Name() == fn) r = h.get(); });
    return r;
}

// ======================= guiones de los dobles (rellenados por cada caso) =======================
inline uint32 rngSeed{}, rngN{};
inline int RndNext() { const uint32 x = (rngSeed * 2654435761u + rngN++ * 40503u) >> 7; return int(x & 0x7FFF); }
inline std::array<bool, 64> sb{}; inline uint32 sbN{};
inline bool NextB() { return sb[sbN++ & 63]; }
inline std::array<float, 16> sf{}; inline uint32 sfN{};
inline float NextF() { return sf[sfN++ & 15]; }
inline std::array<int32, 16> si{}; inline uint32 siN{};
inline int32 NextI() { return si[siN++ & 15]; }
inline CVector centreVec{};          // FindPlayerCentreOfWorld
inline void* playerPtr{};            // FindPlayerPed
inline void* vehPtr{};               // FindPlayerVehicle
inline void* heldTask{};             // GetTaskHold
inline void* pickCarSeq{};
inline std::array<int32, 16> pickList{}; inline uint32 pickN{};
inline std::array<uint32, 512> costs{};

// ======================= objetos falsos =======================
constexpr size_t kGuard = 32, kPedStride = 0x7C4, kObjStride = 0x19C, kDumStride = 0x38, kVehSize = 0xA18, kMiSize = 0x60;
constexpr int kPoolN = 16;
alignas(16) inline std::array<uint8, kPedStride * kPoolN> pedStore{};
alignas(16) inline std::array<uint8, kPoolN> pedState{};
alignas(16) inline std::array<uint8, kObjStride * kPoolN> objStore{};
alignas(16) inline std::array<uint8, kPoolN> objState{};
alignas(16) inline std::array<uint8, kDumStride * kPoolN> dumStore{};
alignas(16) inline std::array<uint8, kPoolN> dumState{};
alignas(16) inline std::array<std::array<uint8, 0x14>, 3> poolHdr{};       // 0 peds, 1 objetos, 2 dummies (storage, estado, capacidad, ultimo libre)
alignas(16) inline std::array<std::array<uint8, 0x400>, kPoolN> intelBuf{};
alignas(16) inline std::array<std::array<uint8, 0x40>, kPoolN> matBuf{};
alignas(16) inline std::array<std::array<uint8, 0x40>, 4> clumpBuf{};
alignas(16) inline std::array<uint8, kVehSize + 2 * kGuard> vehRaw{};
alignas(16) inline std::array<std::array<uint8, kMiSize>, 320> miBuf{};
alignas(16) inline std::array<std::array<uint8, kMiSize>, 4> miRes{};      // reservados 19990..19993 (vehiculo, objeto, ...)
alignas(16) inline std::array<std::array<uint8, 0x40>, 4> colBuf{};
alignas(16) inline std::array<std::array<uint8, 0x40>, 4> fxBuf{};         // C2dEffectPedAttractor falsos
alignas(16) inline std::array<uint8, 0x80> taskBuf{};
inline std::array<void*, 100> entVt{}, miVt{}, taskVt{};
inline uint8* Veh() { return vehRaw.data() + kGuard; }
inline uint8* PedAt(int i) { return pedStore.data() + size_t(i) * kPedStride; }
inline uint8* ObjAt(int i) { return objStore.data() + size_t(i) * kObjStride; }
inline uint8* DumAt(int i) { return dumStore.data() + size_t(i) * kDumStride; }
constexpr int32 kSlotVeh = 19990, kSlotObj = 19991;
inline uintptr_t MiPtrAddr(int32 id) { return 0xA9B0C8 + 4 * uintptr_t(uint32(id)); }

// vtable falsa: hueco 0 = destructor con borrado, hueco 4 = SetIsStatic, hueco 1 de model info = AsAtomicModelInfoPtr
inline void __fastcall V_Dtor(void* self, void*, uint32 flags) { if (Mine()) T("dtor", self, flags); }
inline void __fastcall V_SetStatic(void* self, void*, uint32 v) { if (Mine()) T("set_is_static", self, uint8(v)); }
inline void __fastcall V_Trap(void* self, void*) { if (Mine()) T("virtual_inesperada", self); }
inline bool atomicNull{};
inline void* __fastcall V_AsAtomic(void* self, void*) { return atomicNull ? nullptr : self; }
inline int32 __fastcall V_TaskType(void* self, void*) { return At<int32>(self, 0x40); }

// ======================= dobles de colaboradores =======================
inline int __cdecl S_RandExe() { if (!Mine()) return 0; const int r = RndNext(); T("rand", r); return r; }
inline uint16 __cdecl S_RandDll() { return uint16(S_RandExe()); }
inline void __cdecl S_WorldAdd(CEntity* e) { if (Mine()) T("world_add", e); }
inline void __cdecl S_WorldRemove(CEntity* e) { if (Mine()) T("world_remove", e); }
inline bool __cdecl S_Fewer() { if (!Mine()) return false; const bool r = NextB(); T("fewer_peds", r); return r; }
inline CVehicle* __cdecl S_FindVeh(int32 id, bool inc) { if (!Mine()) return nullptr; T("find_player_vehicle", id, uint8(inc)); return reinterpret_cast<CVehicle*>(vehPtr); }
inline const CVector* __cdecl S_Centre(int32 id) { if (Mine()) T("find_centre", id); return &centreVec; }
inline CPlayerPed* __cdecl S_FindPed(int32 id) { if (Mine()) T("find_player_ped", id); return reinterpret_cast<CPlayerPed*>(playerPtr); }
// bits de guion en la cola del peaton falso (+0x7B0): 1 es jugador, 2 se puede borrar, 4 en pantalla
inline bool PedBit(void* self, uint8 m) { return (At<uint8>(self, 0x7B0) & m) != 0; }
inline bool __fastcall S_IsPlayer(void* self, void*) { if (!Mine()) return false; const bool r = PedBit(self, 1); T("is_player", self, uint8(r)); return r; }
inline bool __fastcall S_CanDelete(void* self, void*) { if (!Mine()) return false; const bool r = PedBit(self, 2); T("can_be_deleted", self, uint8(r)); return r; }
inline bool __fastcall S_OnScreen(void* self, void*) { if (!Mine()) return false; const bool r = PedBit(self, 4); T("on_screen", self, uint8(r)); return r; }
inline bool __cdecl S_Frenzy() { if (!Mine()) return false; const bool r = NextB(); T("frenzy", uint8(r)); return r; }
inline bool __cdecl S_GangWar() { if (!Mine()) return false; const bool r = NextB(); T("gangwar", uint8(r)); return r; }
inline int32 __cdecl S_ClumpAlpha(void* clump) { if (!Mine()) return 0; const int32 r = NextI(); T("clump_alpha", clump, r); return r; }
inline void __cdecl S_RequestModel(int32 m, int32 f) { if (Mine()) T("request_model", m, f); }
inline int32 __cdecl S_Cab() { if (!Mine()) return 0; const int32 r = NextI(); T("cab_model", r); return r; }
inline void __cdecl S_SetDelExe(int32 m) { if (Mine()) T("set_model_deletable", m); }
inline void __cdecl S_SetDelDll(int32 m, bool mission) { if (Mine()) T("set_model_deletable", m); if (Mine() && mission) T("mission_flag_inesperado"); }
inline void __cdecl S_SetTxd(int32 m) { if (Mine()) T("set_txd_deletable", m); }
inline eModelID __cdecl S_GangModel(eGangID g) { if (!Mine()) return MODEL_INVALID; const int32 r = NextI(); T("gang_model", int32(g), r); return eModelID(r); }
inline bool __fastcall S_PlaceGroup(void*, void*, int32 type, uint32 n, const CVector* pos, uint32 alloc) { if (Mine()) T("place_group", type, n, pos->x, pos->y, pos->z, alloc); return true; }
inline bool __cdecl S_Pvl(const CVector* o, float d, CColPoint* cp, CEntity** ent, bool a, bool b, bool c, bool d2, bool e, bool f, void* poly) {
    if (!Mine()) return false;
    const bool hit = NextB(); *ent = hit ? reinterpret_cast<CEntity*>(PedAt(0)) : nullptr;
    At<uint8>(cp, 0x23) = uint8(NextI());
    T("process_vertical_line", o->x, o->y, o->z, d, uint8(a), uint8(b), uint8(c), uint8(d2), uint8(e), uint8(f), uint8(poly != nullptr), uint8(hit));
    return hit; }
inline bool __fastcall S_Skate(void*, void*, int32 surface) { if (!Mine()) return false; const bool r = NextB(); T("surface_skateable", surface & 0xFF, uint8(r)); return r; }
inline bool __cdecl S_Acceptable(int32 m) { if (!Mine()) return false; const bool r = NextB(); T("ped_acceptable", m, uint8(r)); return r; }
inline void __cdecl S_ManageObject(CObject* o, const CVector* p) { if (Mine()) T("manage_object", o, p->x, p->y, p->z); }
inline void __cdecl S_ManageDummy(CDummy* d, const CVector* p) { if (Mine()) T("manage_dummy", d, p->x, p->y, p->z); }
inline void __cdecl S_ManagePed(CPed* d, const CVector* p) { if (Mine()) T("manage_ped", d, p->x, p->y, p->z); }
inline void __cdecl S_ConvReal(CDummyObject* d) { if (Mine()) T("convert_to_real", d); }
inline void __cdecl S_ConvDummy(CObject* d) { if (Mine()) T("convert_to_dummy", d); }
inline bool __cdecl S_TestRoom(CObject* o) { if (!Mine()) return false; const bool r = NextB(); T("test_room", o, uint8(r)); return r; }
inline bool __cdecl S_TestSafe(CDummyObject* o) { if (!Mine()) return false; const bool r = NextB(); T("test_safe", o, uint8(r)); return r; }
inline int32 __fastcall S_PickCar(void* self, void*, bool a, bool b) { if (!Mine()) return -1; const int32 r = pickList[pickN++ & 15]; T("pick_random_car", self, uint8(a), uint8(b), r); return r; }
inline void __cdecl S_FindObjKinda(const CVector* p, float rad, bool b2d, int16* cnt, int16 maxc, CEntity** list, bool a, bool b, bool c, bool d, bool e) {
    if (!Mine()) { *cnt = 0; return; }
    *cnt = int16(NextI()); T("find_objects_kinda", p->x, p->y, p->z, rad, uint8(b2d), int32(maxc), uint8(list != nullptr), uint8(a), uint8(b), uint8(c), uint8(d), uint8(e), int32(*cnt)); }
inline CVector* __fastcall S_BoundCentre(void* self, void*, CVector* out) { if (Mine()) T("bound_centre", self); *out = CVector{ 11.f, -22.f, 33.f }; return out; }
inline void* __fastcall S_TaskHold(void*, void*, bool b) { if (!Mine()) return nullptr; T("get_task_hold", uint8(b)); return heldTask; }
inline bool __cdecl S_AddToPop(float a, float b, float c, float d) { if (!Mine()) return false; const bool r = NextB(); T("add_to_population", a, b, c, d, uint8(r)); return r; }
inline int32 __cdecl S_GenAttr(CVector pos, float a, float b, float c, float d, int32 dm, int32 n) { if (!Mine()) return 0; T("generate_peds_at_attractors", pos.x, pos.y, pos.z, a, b, c, d, dm, n); return 0; }
inline void __cdecl S_Void(const char* tag) { if (Mine()) T(tag); }
inline void __cdecl S_LoadPed() { if (Mine()) T("load_ped_groups"); }
inline void __cdecl S_LoadCar() { if (Mine()) T("load_car_groups"); }
inline void __cdecl S_ManagePop() { if (Mine()) T("manage_population"); }
inline void __cdecl S_RemoveFull() { if (Mine()) T("remove_peds_if_pool_full"); }
inline void __cdecl S_GenStart() { if (Mine()) T("generate_peds_at_start"); }
inline void __fastcall S_UpdFromObj(void* self, void*, void* obj) { if (Mine()) T("update_from_object", self, obj); }
inline void* __fastcall S_CreateObj(void* self, void*) { if (!Mine()) return nullptr; T("create_object", self); return NextB() ? ObjAt(1) : nullptr; }
inline void __fastcall S_Resolve(void* self, void*) { if (Mine()) T("resolve_references", self); }
inline void __fastcall S_SetRelated(void* self, void*, void* d) { if (Mine()) T("set_related_dummy", self, d); }
inline bool __cdecl S_IsGlass(void* e) { if (!Mine()) return false; const bool r = NextB(); T("is_object_glass", e, uint8(r)); return r; }
inline void __fastcall S_AddMoving(void* self, void*) { if (Mine()) T("add_to_moving_list", self); }
inline int32 __cdecl S_ChooseCiv(bool a, bool b, int32 c, int32 d, int32 e, bool f, bool g, bool h, const char* name) {
    if (!Mine()) return -1; const int32 r = NextI(); T("choose_civilian_occupation", uint8(a), uint8(b), c, d, e, uint8(f), uint8(g), uint8(h), uint8(name != nullptr), r); return r; }
inline int32 __cdecl S_CopCar(bool b) { if (!Mine()) return -1; const int32 r = NextI(); T("default_cop_car", uint8(b), r); return r; }
}

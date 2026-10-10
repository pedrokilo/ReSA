#pragma once
// PopFinal4: dobles de colaboradores y objetos falsos para AddPed, AddDeadPedInFrontOfCar, AddPedInCar, AddPedAtAttractor, PlaceCouple,
// CreateWaitingCoppers, GeneratePedsAtAttractors, PopulateInterior y TestSafeForRealObject. Reutiliza la infraestructura de PopFinal3.
#include "../PopFinal3/PopCases2.hpp"

namespace PopFinal4Live {
using namespace PopFinal3Live;

inline int32 __fastcall V_Ret2(void*, void*) { return 2; }
// ---------- memoria falsa ----------
constexpr int kNewPeds = 8, kNewVehs = 4, kArena = 64;
alignas(16) inline std::array<std::array<uint8, kPedStride + 0x40>, kNewPeds> newPed{};
alignas(16) inline std::array<std::array<uint8, 0x400>, kNewPeds> newIntel{};
alignas(16) inline std::array<std::array<uint8, 0x40>, kNewPeds> newPedMat{};
alignas(16) inline std::array<std::array<uint8, kVehSize + 0x40>, kNewVehs> newVeh{};
alignas(16) inline std::array<std::array<uint8, 0x60>, kNewVehs> newVehMat{};
alignas(16) inline std::array<std::array<uint8, 0x100>, kArena> taskArena{};
alignas(16) inline std::array<uint8, 0x400> playerIntel{};
alignas(16) inline std::array<uint8, 0x40> playerData{};
alignas(16) inline std::array<uint8, 0x80> wantedBuf{};
alignas(16) inline std::array<uint8, kPedStride> playerPed{};
alignas(16) inline std::array<uint8, 0x40> frameBuf{};
alignas(16) inline std::array<std::array<uint8, 0x60>, 24> entMat{};
alignas(16) inline std::array<std::array<uint8, 0x1C>, 16> pathNodes{};
alignas(16) inline std::array<uint8, 0x20> fakeTask{};
alignas(16) inline std::array<uint8, 0x10> attrMgr{};
inline int pedNext{}, vehNext{}, taskNext{}; inline int32 animA{}, animB{};
inline std::array<void*, 8> entList{}; inline int entListN{};     // entidades devueltas por FindObjectsInRange / IsPositionClear
inline std::array<int32, 4> nodeScript{};
inline uint32 areaPath{};

inline uint8* NewPedP(int i) { return newPed[size_t(i)].data(); }

inline void ResetFakes() {
    pedNext = vehNext = taskNext = 0; entListN = 0;
    for (auto& p : newPed) std::memset(p.data(), 0xCD, p.size());
    for (auto& v : newVeh) std::memset(v.data(), 0xCD, v.size());
    for (auto& t : taskArena) std::memset(t.data(), 0xCD, t.size());
    std::memset(fakeTask.data(), 0, fakeTask.size()); At<void*>(fakeTask.data(), 0) = taskVt.data();
    for (auto& m : entMat) { std::memset(m.data(), 0, m.size()); At<float>(m.data(), 0) = 1.f; At<float>(m.data(), 0x14) = 1.f; At<float>(m.data(), 0x28) = 1.f; }
    for (auto& m : newPedMat) std::memset(m.data(), 0, m.size());
    for (auto& m : newVehMat) std::memset(m.data(), 0, m.size());
    for (auto& i : newIntel) std::memset(i.data(), 0, i.size());
    std::memset(frameBuf.data(), 0, frameBuf.size());
    At<void*>(clumpBuf[1].data(), 4) = frameBuf.data();
    std::memset(playerPed.data(), 0, playerPed.size()); std::memset(playerIntel.data(), 0, playerIntel.size());
    std::memset(playerData.data(), 0, playerData.size()); std::memset(wantedBuf.data(), 0, wantedBuf.size());
    At<void*>(playerPed.data(), 0x47C) = playerIntel.data(); At<void*>(playerPed.data(), 0x480) = playerData.data(); At<void*>(playerData.data(), 0) = wantedBuf.data();
    At<void*>(playerPed.data(), 0) = entVt.data(); At<uint8>(playerPed.data(), 0x36) = 3;
    playerPtr = playerPed.data();
}
inline void InitNewPed(uint8* p, int idx, int32 type, int32 model) {
    std::memset(p, 0, kPedStride);
    At<void*>(p, 0) = entVt.data(); At<uint16>(p, 0x22) = uint16(model); At<uint8>(p, 0x36) = 3; At<int32>(p, 0x598) = type;
    At<void*>(p, 0x18) = clumpBuf[0].data(); At<void*>(p, 0x47C) = newIntel[size_t(idx)].data();
}

// ---------- descripcion de tareas y eventos ----------
inline bool IsFakeTask(const void* t) { return *reinterpret_cast<void* const*>(t) == taskVt.data(); }
inline bool InArena(const void* t) { return t >= taskArena.data() && t < taskArena.data() + kArena; }
inline json Words(const void* p, size_t size) {
    json w = json::array(); const auto* d = static_cast<const uint32*>(p);
    for (size_t k = 1; k < size / 4; ++k) {
        const uint32 v = d[k]; uint32 m = 0;
        if (Name(v) != "other" && !Name(v).is_null()) { w.push_back(Name(v)); continue; }
        for (int b = 0; b < 4; ++b) { const uint8 by = uint8(v >> (8 * b)); if (by != 0xCD) m |= uint32(by) << (8 * b); }
        w.push_back(m);
    }
    return w;
}
inline json DescribeTask(CTask* t) {
    if (!t) return nullptr;
    if (IsFakeTask(t)) return json{ { "fake", true } };
    json r = { { "type", int32(t->GetTaskType()) } };
    r["words"] = InArena(t) ? Words(t, 0x60) : json("fuera_de_arena");
    return r;
}
inline json DescribeEvent(CEvent* e) {
    if (!e) return nullptr;
    auto* raw = reinterpret_cast<uint8*>(e); const int32 type = int32(e->GetEventType());
    json r = { { "type", type } };
    if (type == EVENT_SCRIPT_COMMAND) r["f"] = json::array({ At<int32>(raw, 0xC), DescribeTask(At<CTask*>(raw, 0x10)), At<uint8>(raw, 0x14) });
    else {
        r["f"] = json::array({ At<uint8>(raw, 0xC), At<int16>(raw, 0xE), At<int16>(raw, 0x10), Name(At<uintptr_t>(raw, 0x14)) });   // CEventEditableResponse: +0xC bool, +0xE taskId, +0x10 facial; derivados desde +0x14
        if (type == EVENT_ATTRACTOR) { r["f"].push_back(Name(At<uintptr_t>(raw, 0x18))); r["f"].push_back(At<uint8>(raw, 0x1C)); }
    }
    return r;
}

// ---------- dobles ----------
inline void* __cdecl S_TaskNew(size_t n) { if (!Mine()) return nullptr; T("task_new", uint32(n)); if (taskNext >= kArena) return nullptr; auto* p = taskArena[size_t(taskNext++)].data(); std::memset(p, 0xCD, 0x100); return p; }
inline void __cdecl S_TaskDelete(void* p) { if (Mine()) T("task_delete", p); }
inline void* __cdecl S_PedNew(unsigned n) { if (!Mine()) return nullptr; T("ped_new", n); if (pedNext >= kNewPeds) return nullptr; return NewPedP(pedNext++); }
inline void* __cdecl S_VehNew(unsigned n) { if (!Mine()) return nullptr; T("veh_new", n); if (vehNext >= kNewVehs) return nullptr; return newVeh[size_t(vehNext++)].data(); }
inline int SlotOfNewPed(void* p) { for (int i = 0; i < kNewPeds; ++i) if (p == NewPedP(i)) return i; return 0; }
inline void* __fastcall S_CivCtor(void* self, void*, int32 type, uint32 model) { if (Mine()) T("civ_ctor", self, type, model); InitNewPed(static_cast<uint8*>(self), SlotOfNewPed(self), type, int32(model)); return self; }
inline void* __fastcall S_EmgCtor(void* self, void*, int32 type, uint32 model) { if (Mine()) T("emergency_ctor", self, type, model); InitNewPed(static_cast<uint8*>(self), SlotOfNewPed(self), type, int32(model)); return self; }
inline void* __fastcall S_CopCtor(void* self, void*, uint32 copType) { if (Mine()) T("cop_ctor", self, copType); InitNewPed(static_cast<uint8*>(self), SlotOfNewPed(self), 6, 0); At<uint32>(self, 0x79C) = copType; return self; }
inline void* __fastcall S_AutoCtor(void* self, void*, int32 model, uint32 createdBy, uint32 flag) {
    if (Mine()) T("automobile_ctor", self, model, createdBy, flag & 0xFF);
    int i = 0; for (; i < kNewVehs; ++i) if (self == newVeh[size_t(i)].data()) break;
    std::memset(self, 0, kVehSize); At<void*>(self, 0) = entVt.data(); At<uint16>(self, 0x22) = uint16(model); At<uint8>(self, 0x36) = 2;
    At<void*>(self, 0x14) = newVehMat[size_t(i & 3)].data(); At<void*>(self, 0x18) = clumpBuf[1].data(); return self; }
inline void __fastcall S_RegRef(void* self, void*, CEntity**) { if (Mine()) T("register_reference", self); }
inline void __fastcall S_CleanRef(void* self, void*, CEntity**) { if (Mine()) T("cleanup_reference", self); }
inline void __fastcall S_SetOrient(void* self, void*, float x, float y, float z) { if (Mine()) T("set_orientation", self, x, y, z); }
inline void __fastcall S_SetPosn(void* self, void*, float x, float y, float z) { if (Mine()) T("set_posn", self, x, y, z); auto* p = static_cast<uint8*>(self); float* d = At<void*>(p, 0x14) ? reinterpret_cast<float*>(static_cast<uint8*>(At<void*>(p, 0x14)) + 0x30) : reinterpret_cast<float*>(p + 4); d[0] = x; d[1] = y; d[2] = z; }
inline void __fastcall S_GiveDelayed(void* self, void*, int32 w, uint32 a) { if (Mine()) T("give_delayed_weapon", self, w, a); }
inline void __fastcall S_SetCurSlot(void* self, void*, int32 s) { if (Mine()) T("set_current_weapon_slot", self, s); }
inline void __fastcall S_SetCurType(void* self, void*, int32 w) { if (Mine()) T("set_current_weapon_type", self, w); }
inline int32 __fastcall S_GiveWeapon(void* self, void*, int32 w, uint32 a, bool b) { if (Mine()) T("give_weapon", self, w, a, uint8(b)); return 0; }
inline void __fastcall S_SetCreatedBy(void* self, void*, int32 c) { if (Mine()) T("set_created_by", self, c); }
inline void __fastcall S_SetDM(void* self, void*, int32 d) { if (Mine()) T("set_decision_maker", self, d); }
inline void __fastcall S_EvHandler(void* self, void*) { if (Mine()) T("process_event_handler", self); }
inline void __fastcall S_IntelProcess(void* self, void*) { if (Mine()) T("intelligence_process", self); }
inline float __fastcall S_WalkSpeed(void* self, void*) { if (!Mine()) return 1.f; const float r = At<float>(self, 0x7B4); T("walk_anim_speed", self, r); return r; }
inline CTask* __cdecl S_Wander(CPed* ped) { if (Mine()) T("wander_task", ped); return reinterpret_cast<CTask*>(fakeTask.data()); }
inline void __fastcall S_SetTask(void* self, void*, CTask* task, int32 idx, bool b) { if (Mine()) T("set_task", self, idx, uint8(b)); if (Mine()) trace.back().push_back(DescribeTask(task)); }
inline CEvent* __fastcall S_EvAdd(void* group, void*, CEvent* ev, bool valid) { if (Mine()) { T("event_add", group, uint8(valid)); trace.back().push_back(DescribeEvent(ev)); } return nullptr; }
inline void __cdecl S_PlacePed(const void* fx, CEntity* ent, CPed* ped, float f) { if (Mine()) T("place_ped_at_effect", fx, ent, ped, f); }
inline void* __cdecl S_AttrMgr() { return attrMgr.data(); }
inline bool __fastcall S_HasQueue(void*, void*, const void* fx, const void* ent) { if (!Mine()) return false; const bool r = NextB(); T("has_queue_tail_arrived", fx, ent, uint8(r)); return r; }
inline bool __cdecl S_SetInCar(CPed* p, CVehicle* v, int32 door, bool drv) { if (Mine()) T("set_ped_in_car_direct", p, v, door, uint8(drv)); return true; }
inline int32 __cdecl S_Door(CVehicle* v, int32 seat) { if (!Mine()) return 0; const int32 r = NextI(); T("compute_door", v, seat, r); return r; }
inline float __cdecl S_GroundZ(float x, float y, float z, bool* hit, CEntity** e) { if (!Mine()) return 0.f; const bool h = NextB(); const float r = NextF() * 2.f - 1.f; if (hit) *hit = h; if (e) *e = nullptr; T("ground_z", x, y, z, uint8(h), r); return r; }
inline bool __cdecl S_ClearPos(const CVector* p, float rad, int32 maxHit, CEntity** out, bool a, bool b, bool c) {
    if (!Mine()) return true; const bool clear = NextB();
    if (out && !clear) for (int k = 0; k < (maxHit < 0 ? 2 : maxHit) && k < entListN; ++k) out[k] = reinterpret_cast<CEntity*>(entList[size_t(k)]);
    T("is_position_clear", p->x, p->y, p->z, rad, maxHit, uint8(out != nullptr), uint8(a), uint8(b), uint8(c), uint8(clear)); return clear; }
inline bool __fastcall S_Sphere(void*, void*, const CVector* p, float r, void* m) { if (!Mine()) return false; const bool v = NextB(); T("is_sphere_visible", p->x, p->y, p->z, r, int32(uintptr_t(m) - 0xB6F028), uint8(v)); return v; }
inline bool __cdecl S_Los(const CVector* a, const CVector* b, bool c, bool d, bool e, bool f, bool g, bool h, bool i) { if (!Mine()) return true; const bool r = NextB(); T("line_of_sight", a->x, a->y, a->z, b->x, b->y, b->z, uint8(c), uint8(d), uint8(e), uint8(f), uint8(g), uint8(h), uint8(i), uint8(r)); return r; }
inline void __cdecl S_FindInRange(const CVector* p, float rad, bool b2d, int16* cnt, int16 maxc, CEntity** list, bool a, bool b, bool c, bool d, bool e) {
    if (!Mine()) { *cnt = 0; return; }
    int n = 0; if (list) for (; n < entListN && n < maxc; ++n) list[n] = reinterpret_cast<CEntity*>(entList[size_t(n)]);
    *cnt = int16(n); T("find_objects_in_range", p->x, p->y, p->z, rad, uint8(b2d), int32(maxc), uint8(list != nullptr), uint8(a), uint8(b), uint8(c), uint8(d), uint8(e), n); }
inline int32 __cdecl S_ProcessCol(const void* ma, void* ca, const void* mb, void* cb, void* sph, void* line, float* touch, bool f) { if (!Mine()) return 0; const int32 r = NextI(); T("process_col_models", ma, ca, mb, cb, uint8(line != nullptr), uint8(touch != nullptr), uint8(f), r); return r; }
inline void __cdecl S_SetAlpha(void* clump, int32 a) { if (Mine()) T("set_clump_alpha", clump, a); }
inline void __fastcall S_Normalise(void* self, void*) { auto* v = static_cast<float*>(self); const float s = v[0] * v[0] + v[1] * v[1] + v[2] * v[2]; if (s > 0.f) { const float l = std::sqrt(s); v[0] /= l; v[1] /= l; v[2] /= l; } else v[0] = 1.f; }
inline void __fastcall S_PlaceRoad(void* self, void*) { if (Mine()) T("place_on_road_properly", self); }
inline bool __fastcall S_Siren(void* self, void*) { if (!Mine()) return false; const bool r = NextB(); T("uses_siren", self, uint8(r)); return r; }
inline void __fastcall S_UpdRw(void* self, void*, void* rw) { if (Mine()) T("matrix_update_rw", self, rw); }
inline void __fastcall S_SimpleUpdRw(void* self, void*, void* rw) { if (Mine()) T("transform_update_rw", self, rw); }
inline void __cdecl S_Join(CVehicle* v) { if (Mine()) T("join_car_with_road_system", v); }
inline void* __fastcall S_FindNth(void*, void*, CNodeAddress* hidden, float x, float y, float z, uint32 type, float maxd, uint32 low, uint32 unk, int32 nth, uint32 boats, uint32 ign, CNodeAddress* out) {
    if (!Mine()) { *hidden = CNodeAddress{}; return hidden; }
    *hidden = CNodeAddress{ uint16(nodeScript[0]), uint16(nodeScript[0] < 0 ? 0xFFFF : 1) };
    if (out) { out[0] = CNodeAddress{ uint16(nodeScript[1]), uint16(nodeScript[1] < 0 ? 0xFFFF : 2) }; out[1] = CNodeAddress{ uint16(nodeScript[2]), uint16(nodeScript[2] < 0 ? 0xFFFF : 3) }; }
    T("find_nth_node", x, y, z, type & 0xFF, maxd, low & 0xFF, unk & 0xFF, nth, boats & 0xFF, ign & 0xFF, int32(nodeScript[0]), int32(nodeScript[1]), int32(nodeScript[2])); return hidden; }
inline bool __fastcall S_GenInterior(void*, void*, float x, float y, CVector* out, void* a, void* b, float* orient) { if (!Mine()) return false; const bool r = NextB(); if (r) { *out = CVector{ NextF() * 40.f, NextF() * 40.f, 1000.f + NextF() }; *orient = NextF(); } T("generate_ped_coors_interior", x, y, uint8(a != nullptr), uint8(b != nullptr), uint8(r)); return r; }
inline bool __cdecl S_FindNewPed(ePedType* t, eModelID* m, bool a, bool b) { if (!Mine()) return false; const bool r = NextB(); if (r) { *t = ePedType(4 + NextI() % 3); *m = eModelID(NextI() & 0xFF); } T("find_new_ped_type", uint8(a), uint8(b), uint8(r)); return r; }
inline void __cdecl S_LoadColl(CVector p, bool b) { if (Mine()) T("load_collision", p.x, p.y, p.z, uint8(b)); }
inline void __cdecl S_LoadAll(bool b) { if (Mine()) T("load_all_requested_models", uint8(b)); }
inline bool __cdecl S_Riots() { if (!Mine()) return false; const bool r = NextB(); T("la_riots", uint8(r)); return r; }
inline int32 __cdecl S_ModelA() { if (!Mine()) return 0; const int32 r = NextI(); T("default_model_a", r); return r; }
inline int32 __cdecl S_ModelB() { if (!Mine()) return 0; const int32 r = NextI(); T("default_model_b", r); return r; }
inline int32 __cdecl S_ModelC() { if (!Mine()) return 0; int32 r = NextI(); if (r < 0 || r > 319) r = 7;   /* el modelo de policia real siempre es valido: el EXE y la DLL leen su model info sin comprobar nulo */ T("default_model_c", r); return r; }
inline CPed* __cdecl S_AddPed(int32 type, int32 model, const CVector* pos, bool wander) {
    if (!Mine()) return nullptr; T("add_ped", type, model, pos->x, pos->y, pos->z, uint8(wander));
    if (pedNext >= kNewPeds) return nullptr; auto* p = NewPedP(pedNext); InitNewPed(p, pedNext, type, model);
    At<void*>(p, 0x14) = newPedMat[size_t(pedNext)].data(); ++pedNext;
    auto* mt = reinterpret_cast<float*>(static_cast<uint8*>(At<void*>(p, 0x14)) + 0x30); mt[0] = pos->x; mt[1] = pos->y; mt[2] = pos->z;
    At<float>(p, 0x7B4) = NextF() * 0.2f + 0.7f; At<int32>(p, 0x4D4) = NextB() ? animA : animB; return reinterpret_cast<CPed*>(p); }
inline int32 __cdecl S_ChooseForVeh(bool b, CVehicle* v) { if (!Mine()) return 7; const int32 r = NextI(); T("choose_civilian_for_vehicle", uint8(b), v, r); return r; }
inline bool __cdecl S_AddAtAttr(int32 model, void* fx, CVector pos, CEntity* ent, int32 dm) { if (!Mine()) return false; const bool r = NextB(); T("add_ped_at_attractor", model, fx, pos.x, pos.y, pos.z, ent, dm, uint8(r)); return r; }
inline int32 __cdecl S_GenAttr4(CVector pos, float a, float b, float c, float d, int32 dm, int32 n) { if (!Mine()) return 0; const int32 r = NextI() & 3; T("generate_peds_at_attractors", pos.x, pos.y, pos.z, a, b, c, d, dm, n, r); return r; }
inline void __cdecl S_RemoveAll() { if (Mine()) T("remove_all_random_peds"); }
inline void __fastcall S_BoundRef(void* self, void*, CVector* out) { if (Mine()) T("bound_centre_ref", self); *out = CVector{ At<float>(self, 4), At<float>(self, 8), At<float>(self, 0xC) }; }
inline bool __fastcall S_Touching(void* self, void*, const CVector* c, float r) { if (!Mine()) return false; const bool t = NextB(); T("is_touching", self, c->x, c->y, c->z, r, uint8(t)); return t; }
inline void __fastcall S_AllocMat(void* self, void*) { if (!Mine()) return; T("allocate_matrix", self); At<void*>(self, 0x14) = entMat[size_t(((uintptr_t(self) >> 4) & 15) + 8)].data(); }
inline void __fastcall S_UpdMat(void* self, void*, void* m) { if (Mine()) T("transform_update_matrix", self, m); }
inline void* __fastcall S_GetCol(void* self, void*) { return colBuf[1].data(); }
inline void* __fastcall S_Random2d(void* self, void*, int32 type, bool b) { if (!Mine()) return nullptr; const int32 k = NextI(); T("get_random_2d_effect", self, type, uint8(b), k & 1); return (k & 1) ? fxBuf[size_t(k & 3)].data() : nullptr; }
}

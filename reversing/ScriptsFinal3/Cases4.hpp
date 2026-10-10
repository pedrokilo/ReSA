#pragma once
// Casos con peds, vehiculos y objetos falsos: CleanUpThisObject/Ped/Vehicle, RemoveThisPed, IsPedStopped, IsVehicleStopped, ClearSpaceForMissionEntity.
// CleanUpThisPed: el fixture garantiza que, fuera de la rama del conductor, ya existe la tarea buscada (secuencia/wander), asi que nunca se llega a la
// creacion de tareas/eventos (ver README: esa cola no se puede doblar con seguridad).
#include "Cases3.hpp"

namespace ScriptsFinal3Live {

inline json EntOut(uint64 ret) {
    json o = Base(ret);
    o["ped"] = Fnv(pedStore.data(), pedStore.size()); o["veh"] = Fnv(vehStore.data(), vehStore.size()); o["obj"] = Fnv(objStore.data(), objStore.size());
    o["ent"] = Fnv(entStore.data(), entStore.size()); o["coldata"] = Hex(colDataStore.data(), 8);
    o["counters"] = HexAt(0xBB4A70, 2) + HexAt(0xC0EC24, 4);
    return o;
}
inline json CaseCleanObject(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x4001); ClearAllFakes(); BuildRanges();
    Mem<uint16>(0xBB4A70) = uint16(Rnd(s)); Mem<uint32>(0xB7CB84) = Rnd(s);
    uint8* o = ObjAt(Rnd(s) % N_OBJ); for (uint32 k = 0; k < OBJ_STRIDE; ++k) o[k] = uint8(Rnd(s));
    At<uint8>(o, 0x13C) = Rnd(s) % 2 ? uint8(Rnd(s) % 2 ? 2 : 6) : uint8(Rnd(s) % 8);
    void* arg = Rnd(s) % 10 == 0 ? nullptr : o;
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(void*)>(addr)(arg); return EntOut(0); });
}
inline json CaseCleanVehicle(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x4002); ClearAllFakes(); BuildRanges();
    uint8* v = VehAt(Rnd(s) % N_VEH); for (uint32 k = 0; k < 0x600; ++k) v[k] = uint8(Rnd(s));
    At<uint8>(v, 0x4A4) = uint8(Rnd(s) % 5);
    void* arg = Rnd(s) % 10 == 0 ? nullptr : v;
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(void*)>(addr)(arg); return EntOut(0); });
}
inline json CaseRemovePed(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x4003); ClearAllFakes(); BuildRanges(); BuildVtables();
    Mem<uint32>(0xC0EC24) = Rnd(s) % 100;
    uint8* p = PedAt(0); for (uint32 k = 0; k < 0x600; ++k) p[k] = uint8(Rnd(s));
    uint8* other = PedAt(1); uint8* v = VehAt(0); for (uint32 k = 0; k < 0x600; ++k) v[k] = uint8(Rnd(s));
    At<uint32>(p, 0) = P(pedVt.data()); At<uint32>(v, 0) = P(vehVt.data());
    At<uint8>(p, 0x484) = uint8(Rnd(s) % 4); At<uint32>(p, 0x598) = Rnd(s) % 3 == 0 ? 6u : Rnd(s) % 5;
    const bool inVeh = Rnd(s) % 3 != 0;
    At<uint32>(p, 0x46C) = (At<uint32>(p, 0x46C) & ~0x100u) | (inVeh ? 0x100u : 0u);
    At<uint32>(p, 0x58C) = Rnd(s) % 8 ? P(v) : 0u;
    At<uint32>(v, 0x460) = Rnd(s) % 2 ? P(p) : P(other);
    At<uint32>(v, 0x4F8) = Rnd(s) % 2 ? 5u : Rnd(s) % 8;
    void* arg = Rnd(s) % 12 == 0 ? nullptr : p;
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(void*)>(addr)(arg); return EntOut(0); });
}
inline json CaseIsPedStopped(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x4004); ClearAllFakes(); BuildRanges();
    static constexpr float ts[] = { 0.f, 0.5f, 1.f, 1.5f, 2.3f, 10.f, 50.f };
    Mem<float>(0xB7CB5C) = ts[Rnd(s) % std::size(ts)];
    uint8* p = PedAt(0); for (uint32 k = 0; k < 0x600; ++k) p[k] = uint8(Rnd(s));
    uint8* v = VehAt(0); At<float>(v, 0xD4) = Rnd(s) % 4 == 0 ? Mem<float>(0xB7CB5C) * 0.01f : float(Rnd(s) % 100) / 1000.f;
    At<uint32>(p, 0x58C) = P(v);
    At<uint32>(p, 0x46C) = (Rnd(s) & 1 ? 0x100u : 0u) | (Rnd(s) & 1) | (Rnd(s) % 4 == 0 ? 0x200u : 0u) | (Rnd(s) % 4 == 0 ? 0x400u : 0u);
    At<uint32>(p, 0x598) = Rnd(s) % 3 == 0 ? Rnd(s) % 2 : 3u;                 // jugador (0/1) o no
    At<int32>(p, 0x534) = int32(Rnd(s) % 8) - 1;
    At<float>(p, 0x4D8) = Rnd(s) % 3 ? 0.f : PickNear(s); At<float>(p, 0x4DC) = Rnd(s) % 3 ? 0.f : PickNear(s);
    rwObjStore[0] = 2; At<uint32>(p, 0x18) = P(rwObjStore.data());           // clump falso (tipo 2 = rpCLUMP)
    fx.assocMask = Rnd(s) % 2 ? 0u : Rnd(s); fx.assocPtr = TaskAt(0);
    return Run(hook, cand, addr, [&] { fx.assocMask = fx.assocMask; return Base(reinterpret_cast<bool(__cdecl*)(void*)>(addr)(p) ? 1 : 0); });
}
inline json CaseIsVehStopped(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x4005); ClearAllFakes(); BuildRanges();
    static constexpr float f[] = { 0.f, 0.5f, 1.f, 1.5f, 2.3f, 10.f, 50.f };
    Mem<float>(0xB7CB5C) = f[Rnd(s) % std::size(f)]; Mem<float>(0xB7CB54) = Rnd(s) % 3 == 0 ? Mem<float>(0xB7CB5C) : f[Rnd(s) % std::size(f)];
    if (Rnd(s) % 30 == 0) Mem<uint32>(0xB7CB54) = 0x7FC00000u;
    uint8* v = VehAt(0); At<float>(v, 0xD4) = Rnd(s) % 3 == 0 ? std::min(Mem<float>(0xB7CB5C), Mem<float>(0xB7CB54)) * 0.01f : float(Rnd(s) % 1000) / 10000.f;
    return Run(hook, cand, addr, [&] { return Base(reinterpret_cast<bool(__cdecl*)(void*)>(addr)(v) ? 1 : 0); });
}

// ---- CleanUpThisPed
inline json CaseCleanPed(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x4006); ClearAllFakes(); BuildRanges(); BuildVtables();
    Mem<uint32>(0xC0EC24) = Rnd(s) % 100;
    uint8* p = PedAt(0); for (uint32 k = 0; k < 0x600; ++k) p[k] = uint8(Rnd(s));
    uint8* v = VehAt(0); for (uint32 k = 0; k < 0xB00; ++k) v[k] = uint8(Rnd(s));
    uint8* intel = IntelAt(0); for (uint32 k = 0; k < 0x100; ++k) intel[k] = uint8(Rnd(s));
    At<uint32>(p, 0x47C) = P(intel); At<uint8>(p, 0x484) = Rnd(s) % 6 == 0 ? uint8(Rnd(s) % 4) : 2;
    const uint32 mode = Rnd(s) % 8;                      // 0 keepTasks; 1-3 conductor (heli, avion, otro); 4-5 pasajero; 6-7 a pie
    At<uint8>(p, 0x478) = (At<uint8>(p, 0x478) & ~0x10) | (mode == 0 ? 0x10 : 0);
    const bool inVeh = mode >= 1 && mode <= 5;
    At<uint32>(p, 0x46C) = (At<uint32>(p, 0x46C) & ~0x100u) | (inVeh ? 0x100u : 0u);
    At<uint32>(p, 0x58C) = inVeh ? P(v) : (Rnd(s) & 1 ? P(v) : 0u);
    At<uint32>(v, 0x460) = (mode >= 1 && mode <= 3) ? P(p) : P(PedAt(1));
    if (mode == 1) { At<uint32>(v, 0x594) = 3; } else if (mode == 2) { At<uint32>(v, 0x594) = 4; } else { At<uint32>(v, 0x594) = Rnd(s) % 3; At<uint32>(v, 0x590) = Rnd(s) % 2 ? 0u : (Rnd(s) % 2 ? 9u : Rnd(s) % 12); }
    if (Rnd(s) % 3 == 0) At<uint8>(v, 0x3BA) = uint8(57 + Rnd(s) % 2);
    fx.groupPtr = Rnd(s) % 2 ? GroupAt(0) : nullptr;
    // tareas existentes: la que el EXE busca debe aparecer en el evento o en la tarea primaria
    const uint32 want = (mode == 4 || mode == 5) ? 244u : 912u;
    uint8* t1 = TaskAt(0); uint8* t2 = TaskAt(1); uint8* ev = EventAt(0);
    At<uint32>(t1, 0) = P(taskVt.data()); At<uint32>(t2, 0) = P(taskVt.data()); At<uint32>(t1, 0x40) = want; At<uint32>(t2, 0x40) = Rnd(s) % 2 ? want : 1u;
    const uint32 sc = Rnd(s) % 4;
    fx.eventPtr = sc == 3 ? nullptr : ev;
    At<uint32>(ev, 0x10) = sc == 0 ? P(t1) : sc == 1 ? P(t2) : 0u;
    if (sc == 1) At<uint32>(t2, 0x40) = 1u;                                      // el evento no coincide
    At<uint32>(intel, 0x10) = (sc == 0) ? (Rnd(s) & 1 ? P(t2) : 0u) : P(t1);    // primaria que coincide
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(void*)>(addr)(p); return EntOut(0); });
}

// ---- ClearSpaceForMissionEntity
inline json CaseClearSpace(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x4007); ClearAllFakes(); BuildRanges(); BuildVtables();
    InstallFakeModel(float(1 + Rnd(s) % 20), uint8(int8(int32(Rnd(s) % 12) - 3)), 0);
    colModelOther.fill(0);
    uint8* ours = EntAt(0); EntInit(ours, 0x40, 2, FAKE_MODEL, Mat(0)); RandMatrix(Mat(0), s); fx.ourEnt = ours;
    const uint32 n = Rnd(s) % 9; fx.numColliding = n; fx.collMask = Rnd(s); fx.collCall = 0;
    fx.deletablePed = Rnd(s) & 1; fx.deletableVeh = Rnd(s) % 3 != 0;
    uint32 pedUsed = 0, vehUsed = 0, entUsed = 1;
    for (uint32 i = 0; i < 16; ++i) fx.colliding[i] = nullptr;
    for (uint32 i = 0; i < n; ++i) {
        const uint32 k = Rnd(s) % 5;
        if (k == 0 || k == 1) {
            if (pedUsed >= N_PED) continue;
            uint8* p = PedAt(pedUsed); EntInit(p, 0x600, 3, 0, Mat(10 + pedUsed)); ++pedUsed; At<uint32>(p, 0) = P(pedVt.data());
            At<uint32>(p, 0x46C) = Rnd(s) % 3 == 0 ? 0x100u : 0u; At<uint32>(p, 0x598) = Rnd(s) % 6 == 0 ? Rnd(s) % 2 : 4u;
            fx.colliding[i] = p;
        } else if (k == 2 || k == 3) {
            if (vehUsed >= N_VEH) continue;
            uint8* v = VehAt(vehUsed); EntInit(v, 0x600, 2, 0, Mat(30 + vehUsed)); ++vehUsed; At<uint32>(v, 0) = P(vehVt.data());
            At<uint32>(v, 0x428) = Rnd(s) % 4 == 0 ? 8u : 0u; At<uint8>(v, 0x488) = uint8(Rnd(s) % 9);
            if (Rnd(s) % 2 && pedUsed < N_PED) { At<uint32>(v, 0x460) = P(PedAt(N_PED - 1 - (pedUsed % 4))); }
            for (uint32 q = 0; q < 8; ++q) At<uint32>(v, 0x464 + 4 * q) = Rnd(s) % 3 ? 0u : P(PedAt(8 + q));
            fx.colliding[i] = v;
        } else {
            if (entUsed >= N_ENT) continue;
            uint8* e = EntAt(entUsed); EntInit(e, 0x40, Rnd(s) % 2 ? 1 : 4, 0, Mat(50 + entUsed)); ++entUsed; fx.colliding[i] = e;
        }
    }
    if (n && Rnd(s) % 4 == 0) fx.colliding[Rnd(s) % n] = ours;               // la propia entidad en la lista
    alignas(16) static float pos[3]; pos[0] = PickWorld(s); pos[1] = PickWorld(s); pos[2] = PickWorld(s);
    return Run(hook, cand, addr, [&] { fx.collCall = 0; reinterpret_cast<void(__cdecl*)(const float*, void*)>(addr)(pos, ours); return EntOut(0); });
}
}

#pragma once
// Casos de PopFinal3, parte 1: funciones puras, de modelo, de contadores y de recorrido de pools.
#include "PopSupport.hpp"

namespace PopFinal3Live {
template<class R, class... A> inline R CallExe(uintptr_t a, A... args) { return reinterpret_cast<R(__cdecl*)(A...)>(a)(args...); }
inline json H(const void* p, size_t n) { return std::format("{:016x}", Fnv(p, n)); }

// ---------- fixtures comunes ----------
inline void FillScripts(Rng& r) {
    rngSeed = r.next(); rngN = 0; sbN = siN = sfN = 0; pickN = 0;
    for (auto& b : sb) b = r.pct(50);
    for (auto& v : si) v = r.pickI({ 0, 1, 2, 7, 100, 255, -1 });
    for (auto& v : sf) v = r.uni(0.f, 3.f);
}
// model infos falsos para los IDs 0..319 (se sustituyen en ms_modelInfoPtrs; se restauran con RestoreRegions)
inline void SetupMis(Rng& r, bool interesting = false) {
    for (int i = 0; i < 320; ++i) {
        auto* m = miBuf[size_t(i)].data(); std::memset(m, 0, kMiSize);
        At<void*>(m, 0) = miVt.data();
        At<uint16>(m, 8) = uint16(r.below(9));                       // m_nRefCount
        At<void*>(m, 0x14) = colBuf[size_t(i & 3)].data();           // m_pColModel
        At<void*>(m, 0x1C) = r.pct(70) ? static_cast<void*>(clumpBuf[0].data()) : nullptr;   // m_pRwObject
        At<int32>(m, 0x24) = int32(r.below(40)) - 1;                 // m_nAnimType
        At<int32>(m, 0x28) = int32(r.below(32));                     // m_nPedType
        At<int32>(m, 0x2C) = interesting ? int32(r.below(46)) - 1 : int32(r.pickI({ 38, 39, 38, 0, 5, 28, 40, 44, 12, 18, 24 }));   // m_nStatType
        At<uint16>(m, 0x30) = uint16(r.next() | (r.pct(50) ? 0x1000 : 0));   // m_nCarsCanDriveMask
        AtA<void*>(MiPtrAddr(i)) = m;
    }
}
inline void BuildPoolHdr(int w, uint8* store, uint8* state) {
    auto* h = poolHdr[size_t(w)].data(); std::memset(h, 0, 0x14);
    At<void*>(h, 0) = store; At<void*>(h, 4) = state; At<uint32>(h, 8) = kPoolN; At<int32>(h, 0xC) = -1;
}
inline uintptr_t PoolGlobal(int w) { return w == 0 ? 0xB74490 : w == 1 ? 0xB7449C : 0xB744A0; }
inline void InstallPools(Rng& r, bool peds, bool objs, bool dums) {
    if (peds) { for (int i = 0; i < kPoolN; ++i) pedState[size_t(i)] = r.pct(60) ? uint8(r.below(128)) : uint8(0x80 | r.below(128)); BuildPoolHdr(0, pedStore.data(), pedState.data()); AtA<uintptr_t>(PoolGlobal(0)) = uintptr_t(poolHdr[0].data()); }
    if (objs) { for (int i = 0; i < kPoolN; ++i) objState[size_t(i)] = r.pct(60) ? uint8(r.below(128)) : uint8(0x80 | r.below(128)); BuildPoolHdr(1, objStore.data(), objState.data()); AtA<uintptr_t>(PoolGlobal(1)) = uintptr_t(poolHdr[1].data()); }
    if (dums) { for (int i = 0; i < kPoolN; ++i) dumState[size_t(i)] = r.pct(60) ? uint8(r.below(128)) : uint8(0x80 | r.below(128)); BuildPoolHdr(2, dumStore.data(), dumState.data()); AtA<uintptr_t>(PoolGlobal(2)) = uintptr_t(poolHdr[2].data()); }
}
inline float Coord(Rng& r) { return r.pct(8) ? r.pick({ 0.f, 25.f, -25.f, 54.5f, 65.f }) : r.uni(-130.f, 130.f); }
inline void InitPed(uint8* p, int i, Rng& r) {
    std::memset(p, 0, kPedStride);
    At<void*>(p, 0) = entVt.data();
    const bool viaMat = r.pct(50);
    const float x = Coord(r), y = Coord(r), z = r.uni(-5.f, 5.f);
    auto* mt = reinterpret_cast<float*>(matBuf[size_t(i)].data() + 0x30); std::memset(matBuf[size_t(i)].data(), 0, 0x40);
    auto* pl = reinterpret_cast<float*>(p + 4);
    if (viaMat) { At<void*>(p, 0x14) = matBuf[size_t(i)].data(); mt[0] = x; mt[1] = y; mt[2] = z; pl[0] = 9999.f; pl[1] = -9999.f; pl[2] = 7777.f; }
    else { At<void*>(p, 0x14) = nullptr; pl[0] = x; pl[1] = y; pl[2] = z; }
    At<void*>(p, 0x18) = r.pct(60) ? static_cast<void*>(clumpBuf[size_t(i & 3)].data()) : nullptr;
    At<uint16>(p, 0x22) = uint16(r.below(300));
    At<uint8>(p, 0x2F) = 0;
    At<uint8>(p, 0x36) = 3;                                                 // ENTITY_TYPE_PED
    At<uint32>(p, 0x46C) = r.pct(15) ? 0x100u : 0u;                          // bInVehicle
    At<uint32>(p, 0x470) = (r.pct(20) ? 8u : 0u) | (r.pct(20) ? 0x40u : 0u) | (r.pct(15) ? 0x400000u : 0u);
    At<uint32>(p, 0x474) = r.pct(50) ? 4u : 0u;                              // bHasBeenAddedToPopulation
    At<void*>(p, 0x47C) = intelBuf[size_t(i)].data();
    At<void*>(p, 0xFC) = r.pct(15) ? static_cast<void*>(Veh()) : nullptr;     // m_pAttachedTo
    At<int32>(p, 0x530) = r.pct(35) ? 55 : int32(r.below(60));                // m_nPedState (55 = DEAD)
    At<uint32>(p, 0x54C) = 100000u + r.below(20000);                          // m_nTimeTillWeNeedThisPed
    At<void*>(p, 0x590) = r.pct(40) ? static_cast<void*>(Veh()) : nullptr;    // m_VehDeadInFrontOf
    At<int32>(p, 0x598) = int32(r.pickI({ 4, 5, 6, 7, 10, 16, 17, 22, 3 }));  // m_nPedType
    At<int32>(p, 0x750) = int32(100000u + r.below(60000)) - int32(r.pct(10) ? 100000u : 0u);   // m_nDeathTimeMS
    At<float>(p, 0x790) = r.pick({ 1.f, 1.f, 0.75f, 1.25f, 2.f });           // m_fRemovalDistMultiplier
    At<uint8>(p, 0x7B0) = uint8((r.pct(10) ? 1 : 0) | (r.pct(70) ? 2 : 0) | (r.pct(50) ? 4 : 0));
}
inline void InitPoolPeds(Rng& r) { for (int i = 0; i < kPoolN; ++i) { InitPed(PedAt(i), i, r); } }
inline void InitVeh(Rng& r) {
    std::memset(vehRaw.data(), 0xCD, vehRaw.size()); std::memset(Veh(), 0, kVehSize);
    At<void*>(Veh(), 0) = entVt.data(); At<uint8>(Veh(), 0x36) = 2;                    // ENTITY_TYPE_VEHICLE
    At<float>(Veh(), 0x44) = r.uni(-1.f, 1.f); At<float>(Veh(), 0x48) = r.uni(-1.f, 1.f);   // m_vecMoveSpeed
    At<int16>(Veh(), 0x22) = int16(kSlotVeh);
}
inline void SetClock(Rng& r) { AtA<uint32>(0xB7CB84) = 150000u + r.below(60000); }
inline void SetCam(Rng& r) {
    AtA<float>(0xB6F11C) = r.pick({ 1.f, 0.75f, 1.2f, 1.5f });
    const uint8 idx = uint8(r.below(2)); AtA<uint8>(0xB6F081) = idx;
    auto* cam = reinterpret_cast<uint8*>(0xB6F19C) + size_t(idx) * 0x238;
    At<uint16>(cam, 0xC) = uint16(r.pickI({ 0, 7, 39, 46, 5, 18 }));
    At<uint8>(cam, 7) = uint8(r.pct(20)); At<uint8>(cam, 8) = uint8(r.pct(20)); At<uint8>(cam, 9) = uint8(r.pct(20));
}
inline json PedsState() { return H(pedStore.data(), pedStore.size()); }

// ---------- funciones puras / de modelo ----------
inline json CaseArePedStats(Rng& r) { const int32 a = int32(r.below(46)) - 1, b = int32(r.below(46)) - 1; json o; o["ret"] = CallExe<bool>(0x610F50, a, b); return o; }
inline json CaseModelBool(Rng& r, uintptr_t addr) { SetupMis(r); const int32 id = int32(r.below(320)); json o; o["ret"] = CallExe<bool>(addr, id); return o; }
inline json CaseThisAttractor(Rng& r) {
    SetupMis(r, true);
    static const int32 ids[] = { 12, 20, 22, 40, 46, 56, 58, 59, 60, 91, 93, 98, 101, 87, 244, 246, 256, 257, 27, 35, 50, 69, 71, 72, 73, 77, 134, 135, 136, 137, 142, 153, 170, 218, 225, 226, 229, 230, 232, 233, 236, 239, 11, 28, 300 };
    static const char* names[] = { "COPSIT", "COPLOOK", "BROWSE", "DANCER", "BARGUY", "PEDROUL", "PEDCARD", "PEDSLOT", "STRIPW", "STRIPM", "copsit", "dancer", "XYZ", "", "stripm" };
    const int32 id = r.pct(70) ? ids[r.below(uint32(std::size(ids)))] : int32(r.below(320));
    const char* name = r.pct(8) ? nullptr : names[r.below(uint32(std::size(names)))];
    if (r.pct(40)) At<int32>(miBuf[size_t(id)].data(), 0x28) = int32(r.pickI({ 6, 5, 4, 17, 22 }));
    json o; o["ret"] = CallExe<bool>(0x6110E0, id, name); return o;
}
inline json CaseSecurityGuard(Rng& r) { json o; o["ret"] = CallExe<bool>(0x6114B0, int32(r.below(34)) - 1); return o; }
inline json CaseRace(Rng& r) {
    alignas(8) char buf[8]{}; static const char c[] = "BHWOIbhwoiXx_.";
    const uint32 len = r.below(4); for (uint32 k = 0; k < len; ++k) buf[k] = c[r.below(uint32(sizeof(c) - 1))];
    json o; o["ret"] = int32(CallExe<int32>(0x5B6D40, static_cast<const char*>(buf))); return o;
}
inline json CaseCarGroup(Rng& r) {
    constexpr size_t N = std::tuple_size_v<std::remove_cvref_t<decltype(CPopulation::m_nNumCarsInGroup)>>;
    for (size_t g = 0; g < N; ++g) { CPopulation::m_nNumCarsInGroup[g] = uint16(r.below(24)); for (size_t k = 0; k < 23; ++k) CPopulation::m_CarGroups[g][k] = r.pct(15) ? int16(-1) : int16(400 + r.below(200)); }
    const int32 g = int32(r.below(uint32(N)));
    const int32 model = r.pct(60) ? int32(CPopulation::m_CarGroups[size_t(g)][r.below(23)]) : int32(400 + r.below(200));
    json o; o["ret"] = CallExe<bool>(0x406F50, g, model); return o;
}
inline json CaseTrivial(Rng& r, uintptr_t addr, int kind) {
    json o;
    switch (kind) {
    case 0: o["ret"] = int32(CallExe<int32>(addr)); break;                                       // ChoosePolicePedOccupation
    case 1: CallExe<void>(addr); o["ret"] = 0; break;                                            // Shutdown / MoveCars
    case 2: CallExe<void>(addr, int32(r.below(5)), int32(r.below(5)), bool(r.below(2))); o["ret"] = 0; break;   // DealWithZoneChange
    case 3: o["ret"] = CallExe<bool>(addr, int32(r.below(320))); break;                          // CanSolicitPlayerOnFoot
    case 4: { AtA<uint8>(0xC0FCB5) = uint8(r.pickI({ 0, 1, 2, 255 })); float f = CallExe<float>(addr); o["ret"] = std::bit_cast<uint32>(f); break; }
    case 5: { void* p = PedAt(r.below(8)); void* v = Veh(); o["ret"] = uintptr_t(CallExe<void*>(addr, p, v)) == uintptr_t(p); break; }   // AddExistingPedInCar
    }
    return o;
}
inline json CaseDummyDist(Rng& r) {
    AtA<uint16>(0x8CD6AC) = uint16(r.below(400)); AtA<uint16>(0x8CD6B0) = uint16(r.below(400));
    const int32 m = r.pct(50) ? int32(AtA<uint16>(r.pct(50) ? 0x8CD6AC : 0x8CD6B0)) : int32(r.below(400));
    json o; o["ret"] = std::bit_cast<uint32>(CallExe<float>(0x610ED0, m)); return o;
}
inline json CaseTimeOfDay(Rng& r) {
    auto* fx = fxBuf[0].data(); std::memset(fx, 0, 0x40); At<int8>(fx, 0x34) = int8(int(r.below(12)) - 1);
    AtA<uint8>(0xB70153) = uint8(r.below(24));
    json o; o["ret"] = CallExe<bool>(0x611B20, static_cast<const void*>(fx)); return o;
}
inline json CaseCullZone(Rng&) { json o; o["ret"] = std::bit_cast<uint32>(CallExe<float>(0x610F00)); return o; }
inline json CaseBenches(Rng& r) {
    AtA<uint32>(0x8D2538) = r.below(60); AtA<float>(0xC0BC40) = r.pct(10) ? 0.f : r.uni(0.f, 60.f); AtA<float>(0x8D2530) = r.pick({ 1.f, 0.5f, 1.5f, 0.25f });
    AtA<int32>(0xB72914) = r.pct(30) ? int32(r.below(4)) : 0; AtA<uint32>(0x8D253C) = r.below(60);
    AtA<uint32>(0xC0EC6C) = r.below(30); AtA<uint32>(0xC0EC70) = r.below(30);
    json o; o["ret"] = CallExe<int32>(0x612240); return o;
}
inline json CaseDistMult(Rng& r) {
    InitVeh(r); vehPtr = r.pct(25) ? nullptr : static_cast<void*>(Veh());
    json o; o["ret"] = std::bit_cast<uint32>(CallExe<float>(0x6116C0)); return o;
}
inline json CaseUpdatePedCount(Rng& r) {
    for (uint32 a = 0xC0EC20; a < 0xC0EC74; a += 4) AtA<uint32>(a) = r.below(50);
    auto* p = PedAt(0); InitPed(p, 0, r);
    json o; CallExe<void>(0x611570, p, bool(r.below(2)));
    o["counters"] = H(reinterpret_cast<void*>(0xC0EC20), 0x54); o["ped"] = H(p, 0x7B0); return o;
}
inline json CaseSkate(Rng& r) {
    const CVector v{ Coord(r), Coord(r), r.uni(-5.f, 5.f) };
    json o; o["ret"] = CallExe<bool>(0x6114C0, &v); return o;
}
inline json CaseGangOcc(Rng& r) { json o; o["ret"] = int32(CallExe<int32>(0x611550, int32(r.below(10)))); return o; }
inline json CasePlaceGang(Rng& r) { const CVector v{ Coord(r), Coord(r), Coord(r) }; CallExe<void>(0x6117D0, int32(7 + r.below(10)), r.below(8), &v); return json::object(); }
inline json CaseDriverModels(Rng& r, uintptr_t addr) {
    static const int32 cars[] = { 409, 420, 423, 428, 438, 448, 463, 481, 400, 600, 408, 482 };
    const int32 m = r.pct(85) ? cars[r.below(uint32(std::size(cars)))] : int32(r.below(700));
    json o; if (addr == 0x611900) o["ret"] = int32(CallExe<int32>(addr, m)); else { CallExe<void>(addr, m); o["ret"] = 0; } return o;
}

// ---------- pools ----------
inline json CaseDistNearestType(Rng& r) {
    InstallPools(r, true, false, false); InitPoolPeds(r);
    const CVector v{ Coord(r), Coord(r), r.uni(-5.f, 5.f) };
    json o; o["ret"] = std::bit_cast<uint32>(CallExe<float>(0x6143E0, int32(r.pickI({ 4, 5, 6, 7, 10, 16, 17, 22, 3 })), v)); return o;
}
inline json CaseRemoveAllRandom(Rng& r) { InstallPools(r, true, false, false); InitPoolPeds(r); CallExe<void>(0x6122C0); json o; o["peds"] = PedsState(); o["state"] = H(pedState.data(), kPoolN); return o; }
inline json CaseRemovePed(Rng& r) { InstallPools(r, true, false, false); InitPoolPeds(r); CallExe<void>(0x610F20, static_cast<void*>(PedAt(int(r.below(kPoolN))))); return json::object(); }
inline json CaseRemoveIfFull(Rng& r) {
    InstallPools(r, true, false, false); InitPoolPeds(r); AtA<uint32>(0xB7CB4C) = r.pct(70) ? 5u + 8u * r.below(100) : r.below(1000);
    if (r.pct(60)) for (int i = 0; i < kPoolN; ++i) pedState[size_t(i)] &= 0x7F;     // pool lleno
    AtA<uintptr_t>(0xB6F03C) = r.pct(50) ? uintptr_t(matBuf[15].data()) : 0;
    auto* cm = reinterpret_cast<float*>(matBuf[15].data() + 0x30); cm[0] = Coord(r); cm[1] = Coord(r); cm[2] = Coord(r);
    AtA<float>(0xB6F02C) = Coord(r); AtA<float>(0xB6F030) = Coord(r); AtA<float>(0xB6F034) = Coord(r);
    CallExe<void>(0x616300); json o; o["peds"] = PedsState(); o["state"] = H(pedState.data(), kPoolN); return o;
}
inline json CaseManagePed(Rng& r) {
    InstallPools(r, true, false, false); InitPoolPeds(r); InitVeh(r); SetClock(r); SetCam(r);
    vehPtr = r.pct(40) ? static_cast<void*>(Veh()) : nullptr; AtA<float>(0x8D2530) = 1.f;
    const CVector pos{ Coord(r), Coord(r), r.uni(-5.f, 5.f) };
    auto* p = PedAt(int(r.below(kPoolN)));
    CallExe<void>(0x611FC0, static_cast<void*>(p), &pos);
    json o; o["peds"] = PedsState(); o["state"] = H(pedState.data(), kPoolN); return o;
}
inline json CaseManagePopulation(Rng& r) {
    InstallPools(r, true, true, true); InitPoolPeds(r); SetClock(r); SetCam(r);
    for (int i = 0; i < kPoolN; ++i) { std::memset(ObjAt(i), 0, kObjStride); std::memset(DumAt(i), 0, kDumStride); }
    centreVec = CVector{ Coord(r), Coord(r), Coord(r) }; AtA<uint32>(0xB7CB4C) = r.below(200); AtA<uint8>(0xB7CD74) = 0;
    CallExe<void>(0x616190); return json::object();
}
inline json CaseManageAll(Rng& r) {
    InstallPools(r, false, true, true);
    for (int i = 0; i < kPoolN; ++i) { std::memset(ObjAt(i), 0, kObjStride); At<uint8>(ObjAt(i), 0x36) = 4; std::memset(DumAt(i), 0, kDumStride); }
    centreVec = CVector{ Coord(r), Coord(r), Coord(r) };
    std::memset(taskBuf.data(), 0, taskBuf.size()); At<void*>(taskBuf.data(), 0) = taskVt.data();
    At<void*>(taskBuf.data(), 8) = ObjAt(int(r.below(kPoolN)));
    static std::array<uint8, kPedStride> player{}; std::memset(player.data(), 0, player.size());
    At<void*>(player.data(), 0x47C) = intelBuf[0].data(); playerPtr = player.data();
    At<int32>(taskBuf.data(), 0x40) = r.pct(80) ? 307 : 5;
    heldTask = r.pct(60) ? static_cast<void*>(taskBuf.data()) : nullptr;
    if (r.pct(25)) At<uint8>(ObjAt(0), 0x36) = 2;
    CallExe<void>(0x6160A0); return json::object();
}
inline json CaseManageDummy(Rng& r) {
    auto* d = DumAt(0); std::memset(d, 0, kDumStride);
    At<void*>(d, 0) = entVt.data(); At<uint8>(d, 0x2F) = uint8(r.pickI({ 0, 0, 13, 2 })); At<uint8>(d, 0x1C) = r.pct(75) ? 0x80 : 0;
    float* pl = reinterpret_cast<float*>(d + 4); pl[0] = Coord(r); pl[1] = Coord(r); pl[2] = r.uni(-5.f, 5.f);
    At<uint16>(d, 0x22) = uint16(r.pct(30) ? r.pickI({ 300, 301 }) : int32(r.below(200)));
    AtA<uint16>(0x8CD6AC) = 300; AtA<uint16>(0x8CD6B0) = 301; AtA<int32>(0xB72914) = int32(r.pickI({ 0, 0, 0, 2 }));
    const CVector pos{ r.pct(50) ? 0.f : Coord(r), Coord(r), 0.f };
    CallExe<void>(0x616000, static_cast<void*>(d), &pos); return json::object();
}
inline json CaseConvertAll(Rng& r) {
    InstallPools(r, false, true, false);
    for (int i = 0; i < kPoolN; ++i) { std::memset(ObjAt(i), 0, kObjStride); At<uint8>(ObjAt(i), 0x13C) = uint8(r.pickI({ 0, 1, 1, 2, 3 })); }
    CallExe<void>(0x616420); return json::object();
}
}

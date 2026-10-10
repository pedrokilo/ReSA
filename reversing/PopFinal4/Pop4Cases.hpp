#pragma once
// PopFinal4: fixtures y llamadas de las 9 funciones.
#include "Pop4Support.hpp"

namespace PopFinal4Live {
inline CVector RPos(Rng& r) { return CVector{ Coord(r), Coord(r), r.uni(-5.f, 5.f) }; }
inline void PrepCommon(Rng& r, bool civWorld) {
    ResetFakes();
    if (civWorld) SetupCivWorld(r); else SetupMis(r);
    centreVec = RPos(r);
    auto* pp = reinterpret_cast<float*>(playerPed.data() + 4); pp[0] = Coord(r); pp[1] = Coord(r); pp[2] = r.uni(-5.f, 5.f);
    AtA<uint8>(0xB6F998) = 0;                                       // espejo de la camara apagado
    animA = int32(CAnimManager::GetAnimationGroupIdByName("jogger")); animB = int32(CAnimManager::GetAnimationGroupIdByName("man"));
}
inline uint8* MakeVeh(Rng& r, int32 model) {
    auto* v = newVeh[3].data(); std::memset(v, 0, kVehSize);
    At<void*>(v, 0) = entVt.data(); At<uint16>(v, 0x22) = uint16(model); At<uint8>(v, 0x36) = 2; At<void*>(v, 0x18) = clumpBuf[1].data();
    std::memset(newVehMat[3].data(), 0, 0x60); At<void*>(v, 0x14) = newVehMat[3].data();
    auto* mt = reinterpret_cast<float*>(newVehMat[3].data() + 0x30); mt[0] = Coord(r); mt[1] = Coord(r); mt[2] = r.uni(-5.f, 5.f);
    for (uint32 off : { 0x460u, 0x464u, 0x468u, 0x46Cu }) if (r.pct(25)) At<void*>(v, off) = PedAt(int(r.below(4)));
    return v;
}
inline json PedsH(int n) { json a = json::array(); for (int i = 0; i < n; ++i) a.push_back(H(NewPedP(i), 0x7B8)); return a; }

inline void RandomizeCheats(Rng& r) { for (int c : { 14, 16, 39, 40, 41, 42, 44, 45, 70, 75 }) AtA<uint8>(0x969130 + uint32(c)) = uint8(r.pct(30)); }

inline json CaseAddPed(Rng& r) {
    PrepCommon(r, false); InstallPools(r, true, false, false); InitPoolPeds(r); RandomizeCheats(r);
    for (uint32 g = 0; g < 10; ++g) for (uint32 k = 0; k < 3; ++k) AtA<int32>(0xC091F0 + g * 0x10 + 4 + 4 * k) = r.pickI({ 0, 0, 5, 22, 24, 30 });
    const int32 type = r.pickI({ 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 22, 23, 24, 25, 31 });
    const CVector pos = RPos(r); const bool wander = r.pct(75);
    CPed* ped = CallExe<CPed*>(0x612710, type, int32(r.below(320)), &pos, wander);
    json o; o["ret"] = Name(uintptr_t(ped)); if (ped) o["ped"] = H(ped, 0x7B8); return o;
}
inline json CaseAddDead(Rng& r) {
    PrepCommon(r, false); InitVeh(r); vehPtr = r.pct(40) ? static_cast<void*>(Veh()) : nullptr;
    auto* v = MakeVeh(r, 420);
    entListN = 3; void* cands[] = { v, nullptr, NewPedP(0), ObjAt(2) };
    for (int i = 0; i < 3; ++i) entList[size_t(i)] = cands[r.below(4)];
    AtA<uint8>(0xB6F998) = 0; const CVector pos = RPos(r);
    CPed* ped = CallExe<CPed*>(0x612CD0, &pos, static_cast<void*>(v));
    json o; o["ret"] = Name(uintptr_t(ped)); o["peds"] = PedsH(1); o["veh"] = H(v, 0x600); return o;
}
inline json CaseAddInCar(Rng& r) {
    PrepCommon(r, true); RandomizeCheats(r);
    static const int32 models[] = { 407, 416, 427, 430, 432, 433, 490, 497, 523, 570, 596, 597, 598, 599, 463, 409, 420, 423, 428, 438, 448, 481, 500, 450 };
    auto* v = MakeVeh(r, models[r.below(uint32(std::size(models)))]);
    for (auto& s : si) s = r.pickI({ 7, 23, 100, 255, -1, 300, 5 });
    CPed* ped = CallExe<CPed*>(0x613A00, static_cast<void*>(v), bool(r.below(2)), int32(r.pickI({ -1, 0, 14, 15, 23, 22, 5 })), int32(r.pickI({ -1, 0, 1, 3 })), bool(r.below(2)), bool(r.below(2)));
    json o; o["ret"] = Name(uintptr_t(ped)); o["peds"] = PedsH(1); o["counters"] = H(reinterpret_cast<void*>(0xC0EC20), 0x54); return o;
}
inline json CaseCouple2(Rng& r) {
    PrepCommon(r, false); InitVeh(r); vehPtr = r.pct(40) ? static_cast<void*>(Veh()) : nullptr;
    entListN = 3; void* cands[] = { NewPedP(0), NewPedP(1), nullptr, ObjAt(2) };
    for (int i = 0; i < 3; ++i) entList[size_t(i)] = cands[r.below(4)];
    const CVector pos = RPos(r);
    CallExe<void>(0x613D60, int32(r.pct(85) ? 4 : 7), int32(r.below(320)), int32(r.pct(85) ? 5 : 4), int32(r.below(320)), pos);
    json o; o["peds"] = PedsH(2); return o;
}
inline json CaseAddAtAttr(Rng& r) {
    PrepCommon(r, false); InstallPools(r, true, false, false); InitPoolPeds(r);
    const CVector pos = RPos(r); if (r.pct(25)) { auto* p0 = PedAt(0); float* q = At<void*>(p0, 0x14) ? reinterpret_cast<float*>(static_cast<uint8*>(At<void*>(p0, 0x14)) + 0x30) : reinterpret_cast<float*>(p0 + 4); q[0] = pos.x + r.uni(-0.3f, 0.3f); q[1] = pos.y + r.uni(-0.3f, 0.3f); pedState[0] &= 0x7F; }
    std::memset(fxBuf[0].data(), 0, 0x40);
    bool ok = CallExe<bool>(0x614210, int32(r.below(320)), static_cast<void*>(fxBuf[0].data()), pos, static_cast<void*>(ObjAt(0)), int32(r.pickI({ -1, 3, 0, 7 })));
    json o; o["ret"] = ok; o["peds"] = PedsH(1); return o;
}
inline json CaseCoppers(Rng& r) {
    PrepCommon(r, false); InitVeh(r);
    At<int32>(wantedBuf.data(), 0x2C) = int32(r.pickI({ 0, 1, 2, 3, 4, 5, 6, 7, -1 }));
    for (auto& n : nodeScript) n = r.pct(25) ? -1 : int32(5 + r.below(3));
    areaPath = 5; for (uint32 a = 5; a < 8; ++a) AtA<uintptr_t>(0x96F854 + 4 * a) = uintptr_t(pathNodes.data());
    for (auto& n : pathNodes) { std::memset(n.data(), 0, n.size()); At<int16>(n.data(), 8) = int16(int(r.below(1600)) - 800); At<int16>(n.data(), 0xA) = int16(int(r.below(1600)) - 800); At<int16>(n.data(), 0xC) = int16(int(r.below(80)) - 40); }
    entListN = r.pct(25) ? 1 : 0; entList[0] = ObjAt(2);
    const CVector pos = RPos(r); const float heading = r.uni(-3.f, 3.f);
    CallExe<void>(0x6133F0, pos, heading);
    json o; json vs = json::array(); for (int i = 0; i < kNewVehs; ++i) vs.push_back(H(newVeh[size_t(i)].data(), 0x600));
    o["veh"] = vs; o["peds"] = PedsH(kNewPeds); return o;
}
inline json CaseGenAttr(Rng& r) {
    PrepCommon(r, true); RandomizeCheats(r);
    AtA<uint8>(0xC0FCB6) = uint8(r.pct(40)); AtA<int32>(0xB72914) = int32(r.pickI({ 0, 0, 0, 3 })); AtA<uint8>(0xB70153) = uint8(r.below(24));
    entListN = int(r.below(5)); static const char* names[] = { "BARGUY", "DANCER", "STRIPM", "COPSIT", "ZZZ" };
    for (int i = 0; i < 4; ++i) {
        entList[size_t(i)] = ObjAt(i);
        auto* e = ObjAt(i); std::memset(e, 0, kObjStride); At<void*>(e, 0) = entVt.data(); At<void*>(e, 0x18) = r.pct(85) ? static_cast<void*>(clumpBuf[1].data()) : nullptr;
        At<uint8>(e, 0x2F) = uint8(r.pct(85) ? AtA<int32>(0xB72914) : 13); At<uint8>(e, 0x36) = uint8(r.pct(85) ? 4 : 2); At<uint32>(e, 0x140) = r.pct(50) ? 0x1000000u : 0u;
        At<void*>(e, 0x14) = r.pct(70) ? static_cast<void*>(entMat[size_t(i)].data()) : nullptr;
        float* ep = reinterpret_cast<float*>(At<void*>(e, 0x14) ? static_cast<uint8*>(At<void*>(e, 0x14)) + 0x30 : e + 4); ep[0] = Coord(r); ep[1] = Coord(r); ep[2] = r.uni(-5.f, 5.f);
        auto* fx = fxBuf[size_t(i)].data(); std::memset(fx, 0, 0x40); At<int32>(fx, 0xC) = 3;   // C2dEffectBase::m_Type = EFFECT_ATTRACTOR (cast_if_present lo comprueba)
        float* fp = reinterpret_cast<float*>(fx); fp[0] = r.uni(-10.f, 10.f); fp[1] = r.uni(-10.f, 10.f); fp[2] = r.uni(-2.f, 2.f);
        At<int8>(fx, 0x34) = int8(r.pickI({ 0, 1, 2, 3, 8, 9, 5 })); At<uint8>(fx, 0x37) = uint8(r.pct(30)); std::strcpy(reinterpret_cast<char*>(fx + 0x38), names[r.below(5)]);
    }
    for (auto& s : si) s = r.pickI({ 0, 7, 100, 255, -1, 12, 201 });
    const CVector pos = RPos(r);
    int32 ret = CallExe<int32>(0x615970, pos, r.uni(0.f, 30.f), r.uni(30.f, 120.f), r.uni(0.f, 30.f), r.uni(30.f, 120.f), int32(r.pickI({ -1, 7, 3, 1 })), int32(r.pickI({ 0, 1, 2, 3, -1 })));
    json o; o["ret"] = ret; json hs = json::array(); for (int i = 0; i < 4; ++i) hs.push_back(H(ObjAt(i), 0x150)); o["ents"] = hs; return o;
}
inline json CasePopInterior(Rng& r) {
    PrepCommon(r, false); InstallPools(r, true, false, false); InitPoolPeds(r);
    const CVector pos{ Coord(r), Coord(r), r.pct(85) ? r.uni(900.f, 1100.f) : r.uni(-5.f, 899.f) };
    CallExe<void>(0x616470, int32(r.pickI({ -1, 0, 1, 3, 4, 6 })), pos);   /* AddPed (0x6165A2) usa el ped sin comprobar nulo: como mucho 6 peds < 8 huecos */
    json o; o["peds"] = PedsH(kNewPeds); return o;
}
inline json CaseTestSafe(Rng& r) {
    PrepCommon(r, false); SetupObjMi(r);
    auto* d = DumAt(0); std::memset(d, 0, kDumStride); At<void*>(d, 0) = entVt.data(); At<uint16>(d, 0x22) = uint16(kSlotObj);
    At<void*>(d, 0x14) = r.pct(60) ? static_cast<void*>(entMat[0].data()) : nullptr;
    float* dp = reinterpret_cast<float*>(At<void*>(d, 0x14) ? static_cast<uint8*>(At<void*>(d, 0x14)) + 0x30 : d + 4); dp[0] = r.uni(-200.f, 200.f); dp[1] = r.uni(-200.f, 200.f); dp[2] = r.uni(-5.f, 5.f);
    if (r.pct(10)) { dp[0] = r.pick({ -3500.f, 3500.f }); }
    AtA<uint16>(0xB7CD78) = uint16(r.next());
    static std::array<std::array<uint32, 3>, 64> nodes{}; std::memset(nodes.data(), 0, sizeof(nodes));
    std::memset(reinterpret_cast<void*>(0xB992B8), 0, 256 * 12); int nn = 0;
    for (int k = 0; k < kNewVehs; ++k) { auto* v = newVeh[size_t(k)].data(); std::memset(v, 0, kVehSize); At<void*>(v, 0) = entVt.data(); At<uint8>(v, 0x36) = 2; At<void*>(v, 0x14) = r.pct(60) ? static_cast<void*>(newVehMat[size_t(k)].data()) : nullptr; At<uint16>(v, 0x2C) = r.pct(25) ? AtA<uint16>(0xB7CD78) : uint16(r.next()); }
    for (int c = 0; c < 256 && nn < 60; ++c) if (r.pct(25)) {
        uint32 head = 0; for (int l = 0; l < int(r.below(3)) + 1 && nn < 63; ++l) { nodes[size_t(nn)] = { uint32(uintptr_t(newVeh[r.below(kNewVehs)].data())), head, 0 }; head = uint32(uintptr_t(&nodes[size_t(nn)])); ++nn; }
        AtA<uint32>(0xB992B8 + uint32(c) * 12) = head;
    }
    for (auto& s : si) s = r.pickI({ 0, 0, 0, 1, 3, -1 });
    json o; o["ret"] = CallExe<bool>(0x6123A0, static_cast<void*>(d));
    json vs = json::array(); for (int k = 0; k < kNewVehs; ++k) vs.push_back(H(newVeh[size_t(k)].data(), 0x40)); o["veh"] = vs; o["dummy"] = H(d, kDumStride); o["flag"] = AtA<uint32>(0xC10280); return o;
}
}

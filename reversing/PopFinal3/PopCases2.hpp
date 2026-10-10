#pragma once
// Casos de PopFinal3, parte 2: funciones con colaboradores doblados (conversion de objetos, civiles, coches, arranque, Update).
#include "PopCases1.hpp"

namespace PopFinal3Live {
inline void SnapshotPrefill(Rng& r, uintptr_t a, size_t n) { auto* p = reinterpret_cast<uint8*>(a); for (size_t i = 0; i < n; ++i) p[i] = uint8(r.next()); }

inline json CaseGenStart(Rng& r) {
    InitVeh(r); vehPtr = r.pct(30) ? nullptr : static_cast<void*>(Veh()); centreVec = CVector{ Coord(r), Coord(r), Coord(r) };
    for (uint32 a = 0xC0EC20; a < 0xC0EC74; a += 4) AtA<uint32>(a) = r.below(40);
    AtA<uint8>(0xB7CD74) = 0;
    CallExe<void>(0x615C90);
    json o; o["counters"] = H(reinterpret_cast<void*>(0xC0EC20), 0x54); return o;
}
inline json CaseUpdate(Rng& r) {
    InitVeh(r); vehPtr = r.pct(30) ? nullptr : static_cast<void*>(Veh()); centreVec = CVector{ Coord(r), Coord(r), Coord(r) };
    AtA<int16>(0xC81314) = int16(r.below(5)); AtA<uint32>(0xC0FCBC) = 99;
    AtA<uint8>(0xA43088) = uint8(r.pct(20) ? 1 : 0); AtA<uint8>(0xC0EC23) = uint8(r.pct(35) ? r.below(4) : 0);
    AtA<uint8>(0xB5F852) = uint8(r.pct(20)); AtA<int32>(0xB72914) = int32(r.pickI({ 0, 0, 0, 3 }));
    AtA<float>(0xB6F11C) = r.pick({ 1.f, 1.25f, 0.8f });
    for (uint32 a = 0xC0EC20; a < 0xC0EC74; a += 4) AtA<uint32>(a) = r.below(40);
    AtA<uint8>(0xC0EC23) = AtA<uint8>(0xC0EC23);      // (se conserva el valor sorteado)
    AtA<uint8>(0xB7CD74) = 0;
    CallExe<void>(0x616650, bool(r.below(2)));
    json o; o["counters"] = H(reinterpret_cast<void*>(0xC0EC20), 0x54); o["zone"] = AtA<uint32>(0xC0FCBC); o["countdown"] = AtA<uint8>(0xC0EC23); return o;
}
inline json CaseInitialise(Rng& r) {
    SnapshotPrefill(r, 0xC0EC20, 0x54); SnapshotPrefill(r, 0x8D2530, 0x10); SnapshotPrefill(r, 0xC0FCB2, 0x0C);
    CallExe<void>(0x610E10);
    json o; o["g1"] = H(reinterpret_cast<void*>(0xC0EC20), 0x54); o["g2"] = H(reinterpret_cast<void*>(0x8D2530), 0x10); o["g3"] = H(reinterpret_cast<void*>(0xC0FCB2), 0x0C); return o;
}
inline json CaseLoadGroups(Rng& r, bool ped) {
    if (ped) {
        SnapshotPrefill(r, 0xC0ECC0, sizeof(CPopulation::m_nNumPedsInGroup)); SnapshotPrefill(r, 0xC0F358, sizeof(CPopulation::m_PedGroups));
        CallExe<void>(0x5BCFE0);
        json o; o["n"] = H(&CPopulation::m_nNumPedsInGroup, sizeof(CPopulation::m_nNumPedsInGroup)); o["m"] = H(&CPopulation::m_PedGroups, sizeof(CPopulation::m_PedGroups)); return o;
    }
    SnapshotPrefill(r, 0xC0EC78, sizeof(CPopulation::m_nNumCarsInGroup)); SnapshotPrefill(r, 0xC0ED38, sizeof(CPopulation::m_CarGroups));
    CallExe<void>(0x5BD1A0);
    json o; o["n"] = H(&CPopulation::m_nNumCarsInGroup, sizeof(CPopulation::m_nNumCarsInGroup)); o["m"] = H(&CPopulation::m_CarGroups, sizeof(CPopulation::m_CarGroups)); return o;
}
inline void SetupObjMi(Rng& r) {      // modelo reservado kSlotObj: +0x12 palabra de tipo especial, +0x14 colision (radio en +0x24)
    auto* m = miRes[1].data(); std::memset(m, 0, kMiSize); At<void*>(m, 0) = miVt.data();
    At<uint16>(m, 0x12) = uint16(r.pickI({ 0x2000, 0x2800, 0, 0x1000 })); At<void*>(m, 0x14) = colBuf[0].data(); At<float>(colBuf[0].data(), 0x24) = r.uni(1.f, 40.f);
    AtA<void*>(MiPtrAddr(kSlotObj)) = m; atomicNull = r.pct(25);
}
inline void InitObj(uint8* o, Rng& r, bool withDummy) {
    std::memset(o, 0, kObjStride); At<void*>(o, 0) = entVt.data(); At<uint16>(o, 0x22) = uint16(kSlotObj);
    At<uint8>(o, 0x36) = uint8(r.pct(85) ? 4 : 2); At<void*>(o, 0x170) = withDummy ? static_cast<void*>(DumAt(1)) : nullptr;
    At<uint32>(o, 0x40) = (r.pct(50) ? 0x2000000u : 0u); At<uint32>(o, 0x1C) = 0;
    if (r.pct(25)) At<uint16>(o, 0x22) = uint16(AtA<uint16>(0x8CD608));
}
inline json CaseTestRoom(Rng& r) {
    SetupObjMi(r); InitObj(ObjAt(0), r, true); std::memset(DumAt(1), 0, kDumStride);
    json o; o["ret"] = CallExe<bool>(0x612320, static_cast<void*>(ObjAt(0))); return o;
}
inline json CaseConvDummy(Rng& r) {
    SetupObjMi(r); const bool withDummy = r.pct(80); InitObj(ObjAt(0), r, withDummy);
    if (!withDummy) At<uint16>(miRes[1].data(), 0x12) = 0;   // el EXE hace `and [edi+1Ch],..` (0x6146F0) sin comprobar el dummy nulo con cristal tipo 1
    std::memset(DumAt(1), 0, kDumStride); At<void*>(DumAt(1), 0) = entVt.data(); At<uint32>(DumAt(1), 0x1C) = r.pct(50) ? 0x80u : 0u;
    CallExe<void>(0x614670, static_cast<void*>(ObjAt(0)));
    json o; o["dummy"] = H(DumAt(1), kDumStride); return o;
}
inline json CaseConvReal(Rng& r) {
    SetupObjMi(r); InitObj(ObjAt(1), r, false); std::memset(DumAt(1), 0, kDumStride); At<void*>(DumAt(1), 0) = entVt.data(); At<uint32>(DumAt(1), 0x1C) = 0x80;
    AtA<uint16>(0x8CD608) = uint16(kSlotObj);
    CallExe<void>(0x614580, static_cast<void*>(DumAt(1)));
    json o; o["dummy"] = H(DumAt(1), kDumStride); o["obj"] = H(ObjAt(1), kObjStride); return o;
}
// ---- civiles ----
inline void SetupCivWorld(Rng& r) {
    SetupMis(r, true);
    for (auto& e : *reinterpret_cast<std::array<int32, 8>*>(0x8E4C00)) e = r.pct(15) ? -1 : int32(r.below(320));
    for (int i = 0; i < 320; ++i) AtA<uint8>(0x8E4CC0 + 0x14 * uint32(i) + 0x10) = uint8(r.pct(80) ? 1 : 0);
    AtA<int32>(0xB72914) = int32(r.pickI({ 0, 0, 0, 2 })); AtA<uint32>(0x8D253C) = r.below(45);
    AtA<float>(0xC81324) = r.pick({ 0.f, 0.05f, 0.1f, 0.5f }); AtA<int16>(0xC81314) = int16(r.below(5));
}
inline json CaseChooseCiv(Rng& r) {
    SetupCivWorld(r);
    static const char* names[] = { "BARGUY", "DANCER", "STRIPM", "COPSIT", "ZZZ" };
    const bool male = r.pct(25), female = r.pct(25), foot = r.pct(30), test = r.pct(60), att = r.pct(30);
    json o; o["ret"] = int32(CallExe<int32>(0x612F90, male, female, int32(r.pct(30) ? int32(r.below(30)) : -1), int32(r.pct(30) ? int32(r.below(320)) : -1), int32(r.pct(30) ? int32(r.below(44)) : -1), foot, test, att, static_cast<const char*>(att ? names[r.below(5)] : nullptr)));
    return o;
}
inline json CaseCouple(Rng& r) {
    SetupCivWorld(r); int32 h = -1, w = -1;
    CallExe<void>(0x613180, &h, &w);
    json o; o["h"] = h; o["w"] = w; return o;
}
inline json CaseChooseForVeh(Rng& r) {
    SetupCivWorld(r); InitVeh(r);
    auto* vm = miRes[0].data(); std::memset(vm, 0, kMiSize); At<uint8>(vm, 0x4D) = uint8(r.below(11)); AtA<void*>(MiPtrAddr(kSlotVeh)) = vm;
    for (int i = 0; i < 8; ++i) AtA<uint8>(0x969130 + 0x27 + uint32(i)) = uint8(r.pct(10));
    for (uint32 off : { 0x460u, 0x464u, 0x468u, 0x46Cu }) { if (r.pct(40)) { auto* p = PedAt(int(r.below(4))); std::memset(p, 0, 0x30); At<uint16>(p, 0x22) = uint16(r.below(320)); At<void*>(Veh(), off) = p; } }
    json o; o["ret"] = int32(CallExe<int32>(0x613260, bool(r.below(2)), static_cast<void*>(Veh()))); return o;
}
inline json CasePickGang(Rng& r) { for (auto& v : pickList) v = r.pct(30) ? -1 : int32(r.below(600)); json o; o["ret"] = int32(CallExe<int32>(0x614490, int32(r.below(10)))); return o; }
inline json CasePickRiot(Rng& r) {
    for (auto& v : pickList) v = r.pct(70) ? -1 : int32(400 + r.below(100));
    for (uint32 g : { 0xC0E9F8u, 0xC0E9C8u }) for (uint32 k = 0; k < 23; ++k) AtA<int16>(g + 2 * k) = int16(400 + r.below(200));
    json o; o["ret"] = int32(CallExe<int32>(0x6144B0)); return o;
}
inline json CaseMall(Rng& r) {
    SetupMis(r); AtA<int32>(0xB72914) = r.pct(70) ? 4 : 0;
    for (auto& v : si) v = int32(r.below(300));
    const CVector v{ Coord(r), Coord(r), Coord(r) }; CallExe<void>(0x613CD0, &v); return json::object();
}
}

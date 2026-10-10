#pragma once
// Casos de CRadar que recorren la tabla de blips con colaboradores del mundo: marcadores 3D, blips, mapa, mapa de radar, zonas de bandas, aeropuertos,
// blips de coordenadas y guardado/carga. Entidades, pools, ModelInfo y el pool de EntryExit son FALSOS (se intercambian los punteros y se restauran).
#include "DrawCases.hpp"

namespace RadarFinal3Live {

inline Buf<0x60> worldMat[12];

// Mundo falso: pools de vehiculos/peds/objetos con 4 entidades en uso, ModelInfo falsos para los modelos de prueba y 8 pickups reales de aPickUps
inline void FixEntityWorld(uint32& s) {
    InitPools();
    Mem<uint32>(0xB74490) = P(poolPed.p()); Mem<uint32>(0xB74494) = P(poolVeh.p()); Mem<uint32>(0xB7449C) = P(poolObj.p());
    for (int j = 0; j < 3; ++j) {
        fakeModelInfo[j].Fill(0); fakeColModel[j].Fill(0);
        At<uint32>(fakeModelInfo[j].p(), 0x14) = P(fakeColModel[j].p());
        At<float>(fakeColModel[j].p(), 0x14) = PickIn(s, 0.2f, 4.f);          // CColModel::m_boundBox.m_vecMax.z
    }
    Mem<uint32>(MODEL_INFO_PTRS + 4u * uint32(FAKE_MODEL_A)) = P(fakeModelInfo[0].p());
    Mem<uint32>(MODEL_INFO_PTRS + 4u * uint32(FAKE_MODEL_C)) = P(fakeModelInfo[1].p());
    Mem<uint32>(MODEL_INFO_PTRS + 4u * uint32(NEVADA_MODEL)) = P(fakeModelInfo[2].p());
    statesVeh.Fill(0x80); statesPed.Fill(0x80); statesObj.Fill(0x80);
    for (int k = 0; k < 4; ++k) {
        statesVeh.p()[k] = uint8(1 + Rnd(s) % 100); statesPed.p()[k] = uint8(1 + Rnd(s) % 100); statesObj.p()[k] = uint8(1 + Rnd(s) % 100);
        uint8* v = storeVeh.p() + k * 0xA18; std::memset(v, 0, 0xA18);
        At<uint8>(v, 0x36) = 2; At<int8>(v, 0x48B) = -1; At<int16>(v, 0x22) = int16((Rnd(s) % 4 == 0) ? NEVADA_MODEL : FAKE_MODEL_A);
        SetVec3(v, 4, PickIn(s, -3000.f, 3000.f), PickIn(s, -3000.f, 3000.f), PickIn(s, -50.f, 400.f));
        if (Rnd(s) & 1) { RandMatrix(worldMat[k].p(), s); At<uint32>(v, 0x14) = P(worldMat[k].p()); }
        uint8* p = storePed.p() + k * 0x7C4; std::memset(p, 0, 0x7C4);
        SetVec3(p, 4, PickIn(s, -3000.f, 3000.f), PickIn(s, -3000.f, 3000.f), PickIn(s, -50.f, 400.f));
        if (Rnd(s) & 1) { RandMatrix(worldMat[4 + k].p(), s); At<uint32>(p, 0x14) = P(worldMat[4 + k].p()); }
        if (Rnd(s) % 3 == 0) { At<uint8>(p, 0x46D) |= 1; At<uint32>(p, 0x58C) = P(storeVeh.p() + 0xA18 * (Rnd(s) % 4)); }       // en un vehiculo del pool (bit 8 de 0x46C)
        uint8* o = storeObj.p() + k * 0x19C; std::memset(o, 0, 0x19C);
        At<int16>(o, 0x22) = int16(FAKE_MODEL_C);
        SetVec3(o, 4, PickIn(s, -3000.f, 3000.f), PickIn(s, -3000.f, 3000.f), PickIn(s, -50.f, 400.f));
        if (Rnd(s) & 1) { RandMatrix(worldMat[8 + k].p(), s); At<uint32>(o, 0x14) = P(worldMat[8 + k].p()); }
    }
    for (int k = 0; k < 8; ++k) {
        uint8* pk = reinterpret_cast<uint8*>(PICKUPS + 0x20u * uint32(k));
        for (int b = 0; b < 0x20; ++b) pk[b] = uint8(Rnd(s) >> 9);
        At<int16>(pk, 0x10) = int16(int32(Rnd(s) % 48000u) - 24000); At<int16>(pk, 0x12) = int16(int32(Rnd(s) % 48000u) - 24000); At<int16>(pk, 0x14) = int16(int32(Rnd(s) % 4000u) - 800);
        At<uint16>(pk, 0x1A) = uint16(1 + Rnd(s) % 60000u);
    }
}
inline uint32 EntityHandle(uint32 type, uint32& s) {
    const uint32 k = Rnd(s) % 4;
    if (type == 1) return (k << 8) | statesVeh.p()[k];
    if (type == 2) return (k << 8) | statesPed.p()[k];
    if (type == 3) return (k << 8) | statesObj.p()[k];
    const uint32 idx = Rnd(s) % 8;                                                          // pickup: (referencia << 16) | indice
    return (uint32(At<uint16>(reinterpret_cast<uint8*>(PICKUPS + 0x20u * idx), 0x1A)) << 16) | idx;
}

inline json CaseDraw3dMarkers(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7301, [](uint32& s, json&, uintptr_t a) {
        FixEntityWorld(s); FixPlayers(s);   // FindPlayerPed nunca nulo: el EXE desreferencia [eax+2Fh] sin comprobar en 0x585CA7 y 0x585F37 (OBJECT/PICKUP/CONTACT)
        static constexpr int32 areas[] = { 0, 0, 0, 1, 2 }; Mem<int32>(0xB72914) = areas[Rnd(s) % std::size(areas)];
        fixMission = Rnd(s) % 4 == 0;
        auto* base = reinterpret_cast<uint8*>(0xBA86F0);
        for (int i = 0; i < 175; ++i) At<uint8>(base + i * 0x28, 0x25) &= uint8(~2u);
        const uint32 n = 3 + Rnd(s) % 8;
        for (uint32 k = 0; k < n; ++k) {
            uint8* t = base + (Rnd(s) % 175) * 0x28;
            RandTrace(t, s, true);
            static constexpr uint32 types[] = { 1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2, 3, 5, 7 };
            const uint32 type = types[Rnd(s) % std::size(types)];
            At<uint8>(t, 0x25) |= 2;
            At<uint8>(t, 0x26) = uint8((At<uint8>(t, 0x26) & ~0x3Cu) | (type << 2) | ((Rnd(s) % 4 < 3) ? 3u : 0u));   // casi siempre BOTH
            At<uint8>(t, 0x26) = uint8((At<uint8>(t, 0x26) & ~3u) | (Rnd(s) % 4));
            if (type >= 1 && type <= 3) At<uint32>(t, 4) = EntityHandle(type, s);
            if (type == 7) At<uint32>(t, 4) = EntityHandle(7, s);
        }
        Call<void>(a);
    });
}

// ---------------- DrawBlips / DrawMap / DrawRadarMap / DrawRadarGangOverlay / DrawCoordBlip ----------------
inline void FixPlayerVehicles(uint32& s) {
    for (int i = 0; i < 2; ++i) {
        const uint32 kind = Rnd(s) % 7;
        static constexpr float heights[] = { 0.f, 59.99f, 60.f, 60.01f, 100.f, 179.9f, 180.f, 181.f, 300.f, -5.f };
        if (kind <= 1) { fixVeh[i] = nullptr; continue; }
        const uint32 subtype = (kind == 2 || kind == 3) ? 4u : (kind == 4) ? 3u : (kind == 5) ? 4u : 0u;
        const int32 model = (kind == 5) ? 539 : (kind == 6) ? 420 : 460;
        MakeVehicle(fakeVeh[i], fakeMat[2 + i], s, subtype, model, (Rnd(s) & 1) != 0);
        if (At<uint32>(fakeVeh[i].p(), 0x14)) At<float>(fakeMat[2 + i].p(), 0x38) = heights[Rnd(s) % std::size(heights)]; else At<float>(fakeVeh[i].p(), 0x0C) = heights[Rnd(s) % std::size(heights)];
        fixVeh[i] = fakeVeh[i].p();
    }
}
inline void FixPlayerRefs(uint32& s) {
    MakePed(fakePed[0], fakeMat[0], s, (Rnd(s) % 4 == 0) ? 1u : 0u, (Rnd(s) & 1) != 0); MakePed(fakePed[1], fakeMat[1], s, (Rnd(s) % 4 == 0) ? 1u : 0u, (Rnd(s) & 1) != 0);
    fixPed[0] = fakePed[0].p(); fixPed[1] = (Rnd(s) % 4 == 0) ? nullptr : fakePed[1].p();
    Mem<uint32>(PLAYERS) = P(fakePed[0].p()); Mem<uint32>(PLAYERS + 0x190) = P(fakePed[1].p());
    for (int i = 0; i < 2; ++i) { fixCentre[i] = CVector{ PickIn(s, -3500.f, 3500.f), PickIn(s, -3500.f, 3500.f), PickIn(s, -20.f, 200.f) }; fixHeading[i] = PickIn(s, -3.2f, 3.2f); fixHidden[i] = Rnd(s) & 1; }
    Mem<uint8>(PLAYER_FOCUS) = uint8(Rnd(s) & 1);
    if (!fixPed[Mem<uint8>(PLAYER_FOCUS) & 1]) Mem<uint8>(PLAYER_FOCUS) = 0;   // FindPlayerPed(-1) nunca nulo: DisplayThisBlip/DrawMap lo desreferencian (+0x2F) sin comprobar
}
inline json CaseDrawBlips(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7302, [](uint32& s, json&, uintptr_t a) {
        SetRwEngine(s); FixCamera(s, false); FixPlayerRefs(s); FixPlayerVehicles(s);
        FixFrontEnd(s, (Rnd(s) % 3 == 0) ? 1 : 0);
        Mem<uint8>(0xB73418) = uint8(Rnd(s) % 4 == 0); Mem<uint8>(0xA444A4) = uint8(Rnd(s) % 8 == 0);
        static constexpr int32 areas[] = { 0, 0, 0, 1, 2 }; Mem<int32>(0xB72914) = areas[Rnd(s) % std::size(areas)];
        Call<void>(a);
    });
}
inline json CaseDrawMap(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7303, [](uint32& s, json&, uintptr_t a) {
        FixPlayerRefs(s); FixPlayerVehicles(s);
        static constexpr float mags[] = { 0.f, 0.1f, 0.3f, 0.30000001f, 0.5f, 0.8999f, 0.9f, 1.2f, 0.29999f, 0.9000001f };
        const float mag = (Rnd(s) % 3 == 0) ? PickIn(s, 0.f, 1.5f) : mags[Rnd(s) % std::size(mags)];
        fixSpeed = (Rnd(s) & 1) ? CVector{ mag, 0.f, 0.f } : CVector{ mag * 0.6f, mag * 0.64f, mag * 0.48f };
        fixRemote = Rnd(s) % 6 == 0; fixCoop = Rnd(s) % 4 == 0;
        Mem<int32>(0x8A5E50) = int32(Rnd(s) % 3);
        Mem<uint8>(0xA444A3) = (Rnd(s) % 3 == 0) ? uint8(Rnd(s) % 100) : uint8(0);
        static constexpr int32 areas[] = { 0, 0, 0, 1, 2 }; Mem<int32>(0xB72914) = areas[Rnd(s) % std::size(areas)];
        Call<void>(a);
    });
}
inline json CaseDrawRadarMap(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7304, [](uint32& s, json&, uintptr_t a) {
        SetRwEngine(s); FixPlayerVehicles(s);
        // Dominio: el EXE lee la matriz del avion sin comprobar (0x586A76 fld [ecx+18h], ecx = [vehiculo+14h]); todo vehiculo lleva matriz
        for (int i = 0; i < 2; ++i) if (fixVeh[i] && !At<uint32>(fakeVeh[i].p(), 0x14)) { RandMatrix(fakeMat[2 + i].p(), s); At<uint32>(fakeVeh[i].p(), 0x14) = P(fakeMat[2 + i].p()); }
        fixCentre[0] = CVector{ PickIn(s, -3500.f, 3500.f), PickIn(s, -3500.f, 3500.f), PickIn(s, -20.f, 200.f) };
        // origen cerca de los limites de casilla (500 unidades) para el suelo / techo del producto por 0.002
        auto edge = [&] { const float base = float(int32(Rnd(s) % 13) * 500 - 3000); static constexpr float d[] = { 0.f, 0.001f, -0.001f, 0.25f, 499.99f, 250.f }; return (Rnd(s) & 1) ? base + d[Rnd(s) % std::size(d)] : PickIn(s, -3500.f, 3500.f); };
        Mem<float>(0xBAA248) = edge(); Mem<float>(0xBAA24C) = edge();
        Call<void>(a);
    });
}
inline json CaseDrawRadarGangOverlay(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7305, [](uint32& s, json&, uintptr_t a) {
        Mem<int16>(0xBA1DE8) = 8; Mem<int16>(0xBA3794) = int16(Rnd(s) % 7);
        for (int k = 0; k < 8; ++k) {   // CZoneInfo: 0xB..0xE color, 0xF banderas (RadarMode en los bits 5-6)
            uint8* z = reinterpret_cast<uint8*>(0xBA1DF0) + 0x11 * k;
            for (int b = 0; b < 0x11; ++b) z[b] = uint8(Rnd(s) >> 8);
            z[0xF] = uint8((z[0xF] & ~0x60u) | ((Rnd(s) % 4) << 5));
        }
        for (int k = 0; k < 7; ++k) {   // CZone: 0x10 x1, 0x12 y1, 0x14 z1, 0x16 x2, 0x18 y2, 0x1A z2, 0x1C indice de CZoneInfo
            uint8* z = reinterpret_cast<uint8*>(0xBA3798) + 0x20 * k;
            const int16 x1 = int16(int32(Rnd(s) % 5000u) - 2500), y1 = int16(int32(Rnd(s) % 5000u) - 2500);
            At<int16>(z, 0x10) = x1; At<int16>(z, 0x12) = y1; At<int16>(z, 0x14) = 0;
            At<int16>(z, 0x16) = int16(x1 + int32(Rnd(s) % 800u)); At<int16>(z, 0x18) = int16(y1 + int32(Rnd(s) % 800u)); At<int16>(z, 0x1A) = 100;
            At<int16>(z, 0x1C) = int16(Rnd(s) % 8);
        }
        Mem<uint8>(0x96AB90) = uint8(Rnd(s) % 6 != 0); Mem<uint8>(0xBA6791) = uint8(Rnd(s) % 6 != 0);
        fixGangMask = Rnd(s);
        Call<void>(a, Rnd(s) & 1);
    });
}
inline json CaseDrawCoordBlip(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7306, [](uint32& s, json&, uintptr_t a) {
        FixPlayerRefs(s);
        static constexpr uint32 reveals[] = { 0, 79, 80, 81, 100, 0xFFFFFFFFu };
        Mem<uint32>(0xBA372C) = reveals[Rnd(s) % std::size(reveals)];
        fixZoneLock = Rnd(s) & 1; fixMission = Rnd(s) % 5 == 0;
        Mem<uint8>(0x859CF8) = uint8(Rnd(s) % 3 == 0);
        Mem<uint8>(0xA444A3) = (Rnd(s) % 4 == 0) ? uint8(1 + Rnd(s) % 50) : uint8(0);
        const uint32 k = Rnd(s) % 175; uint8* t = reinterpret_cast<uint8*>(0xBA86F0) + k * 0x28;
        // tipos COORD (4) y CONTACT (5) casi siempre; icono (sprite) la mitad de las veces; pantalla de mapa a veces
        const uint32 type = (Rnd(s) % 6 == 0) ? Rnd(s) % 9 : 4 + Rnd(s) % 2;
        At<uint8>(t, 0x26) = uint8((At<uint8>(t, 0x26) & ~0x3Cu) | (type << 2));
        if (Rnd(s) & 1) At<int8>(t, 0x24) = 0;
        static constexpr float radii[] = { 1.0f, 1.05f, 1.1f, 0.5f, 1.1000001f, 5.f, 1.2f, 0.f };
        At<float>(t, 0x18) = (Rnd(s) % 3 == 0) ? radii[Rnd(s) % std::size(radii)] : PickIn(s, 0.2f, 7.f);       // m_fSphereRadius (el marcador de depuracion lo reduce 0.1)
        fixCentre[0].z = At<float>(t, 0x10) + PickIn(s, -8.f, 8.f);      // alturas cerca de la del blip (umbrales -4 / +2)
        static constexpr float dz[] = { 0.f, 0.001f, -0.001f, 6.f, 6.001f, 5.999f, 3.f, -3.f };      // centro.z = z - 2 + dz: fronteras BAJO (z - 2 > centro) y ALTO (z + 4 < centro)
        if (Rnd(s) % 3 == 0) fixCentre[0].z = At<float>(t, 0x10) - 2.f + dz[Rnd(s) % std::size(dz)];
        Call<void>(a, int32(k), Rnd(s) & 1);
    });
}
inline json CaseSetupAirstripBlips(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7307, [](uint32& s, json&, uintptr_t a) {
        MakeVehicle(fakeVeh[0], fakeMat[2], s, (Rnd(s) % 4 == 0) ? 3u : 4u, (Rnd(s) % 6 == 0) ? 539 : 460, (Rnd(s) & 1) != 0);
        // posiciones: aleatorias, justo encima de un aeropuerto o equidistantes de dos (empate)
        static constexpr float tab[4][2] = { { 1750.f, -2494.f }, { -1373.f, 120.f }, { 1478.f, 1461.f }, { 175.f, 2502.f } };
        float px = PickIn(s, -3500.f, 3500.f), py = PickIn(s, -3500.f, 3500.f);
        const uint32 mode = Rnd(s) % 5;
        if (mode == 0) { const auto& q = tab[Rnd(s) % 4]; px = q[0]; py = q[1]; }
        else if (mode == 1) { const auto& p = tab[Rnd(s) % 4]; const auto& q = tab[Rnd(s) % 4]; px = (p[0] + q[0]) * 0.5f; py = (p[1] + q[1]) * 0.5f; }   // punto medio: d(p) == d(q)
        if (At<uint32>(fakeVeh[0].p(), 0x14)) { At<float>(fakeMat[2].p(), 0x30) = px; At<float>(fakeMat[2].p(), 0x34) = py; } else { At<float>(fakeVeh[0].p(), 4) = px; At<float>(fakeVeh[0].p(), 8) = py; }
        fixVeh[0] = (Rnd(s) % 5 == 0) ? nullptr : fakeVeh[0].p(); fixVeh[1] = nullptr;
        // blip del aeropuerto: ninguno, uno valido (en seguimiento) o uno invalido
        Mem<uint8>(0xBA8300) = uint8(Rnd(s) % 4);
        uint32 blip = 0; const uint32 bm = Rnd(s) % 4;
        if (bm == 1) { const uint32 k = Rnd(s) % 175; uint8* t = reinterpret_cast<uint8*>(0xBA86F0) + k * 0x28; At<uint8>(t, 0x25) |= 2; blip = (uint32(At<uint16>(t, 0x14)) << 16) | k; }
        else if (bm == 2) blip = (Rnd(s) & 0xFFFF0000u) | (Rnd(s) % 175);
        Mem<uint32>(0xBA8304) = blip;
        Call<void>(a);
    });
}
inline json CaseLoad(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7308, [](uint32& s, json& o, uintptr_t a) {
        InitPools(); Mem<uint32>(0x96A7D8) = P(poolEnex.p()); statesEnex.Fill(0x80);
        for (int k = 0; k < 8; ++k) if (Rnd(s) % 3) statesEnex.p()[k] = uint8(1 + Rnd(s) % 100);
        for (int i = 0; i < 175; ++i) { RandTrace(fixLoadData[i], s, false); At<uint32>(fixLoadData[i], 0x20) = (Rnd(s) % 3 == 0) ? 0u : 1u + Rnd(s) % 8; }
        loadCursor = 0; FixRadarTextures(s);
        o["ret"] = Call<uint32>(a) & 0xFF;
    });
}
inline json CaseSave(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7309, [](uint32& s, json& o, uintptr_t a) {
        InitPools(); Mem<uint32>(0x96A7D8) = P(poolEnex.p()); statesEnex.Fill(0x80);
        for (int k = 0; k < 8; ++k) if (Rnd(s) % 3) statesEnex.p()[k] = uint8(1 + Rnd(s) % 100);
        o["ret"] = Call<uint32>(a) & 0xFF;
    });
}
}

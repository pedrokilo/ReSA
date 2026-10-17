#pragma once
// Casos de CRadar con dibujo: leyenda, trazas con altura, marcador de depuracion, sprites rotados, secciones del mapa, areas y mascara.
// Los estados RW pasan por la tabla de motor falsa (SetRwEngine); los colaboradores de dibujo son dobles con traza.
#include "SimpleCases.hpp"

namespace RadarFinal3Live {

// Pool de TXD falso: 8 ranuras en uso con diccionario falso o nulo; texturas de las secciones del radar entre esas ranuras o -1
inline void FixTxdPool(uint32& s) {
    InitPools(); Mem<uint32>(0xC8800C) = P(poolTxd.p());
    for (int k = 0; k < 8; ++k) {
        uint8* e = storeTxd.p() + 12 * k;
        At<uint32>(e, 0) = (Rnd(s) % 7 == 0) ? 0u : P(fakeDict[k & 3].p());
        statesTxd.p()[k] = uint8(1 + k);
    }
    fakeTexture.Fill(0xCD); fakeRaster.Fill(0xCD); At<uint32>(fakeTexture.p(), 0) = P(fakeRaster.p());
    fixFirstTex = (Rnd(s) % 6 == 0) ? nullptr : fakeTexture.p();
    for (int i = 0; i < 144; ++i) Mem<int32>(0xBA8478 + 4u * uint32(i)) = (Rnd(s) % 7 == 0) ? -1 : int32(Rnd(s) % 8);
}

inline json CaseDrawLegend(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7201, [](uint32& s, json&, uintptr_t a) {
        SetRwEngine(s);
        static constexpr int32 sprites[] = { -5, -4, -3, -2, -1, -3, -2, -1, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 57, 58, 59, 60, 61, 62, 63 };
        static constexpr uint32 deltas[] = { 0, 1, 599, 600, 601, 1000, 5000, 70000 };
        Mem<uint8>(0xBAA350) = uint8(Rnd(s) % 3); Mem<uint32>(0xBAA354) = Mem<uint32>(0xB7CB7C) - deltas[Rnd(s) % std::size(deltas)];
        Call<void>(a, int32(Rnd(s) % 2000), int32(Rnd(s) % 1100), sprites[Rnd(s) % std::size(sprites)]);
    });
}
inline json CaseShowRadarTrace(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7202, [](uint32& s, json&, uintptr_t a) {
        SetRwEngine(s);
        Call<void>(a, PickIn(s, -100.f, 1800.f), PickIn(s, -100.f, 1100.f), Rnd(s) % 22, Rnd(s) % 300, Rnd(s) % 300, Rnd(s) % 300, Rnd(s) % 300);
    });
}
inline json CaseShowRadarTraceWithHeight(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7203, [](uint32& s, json&, uintptr_t a) {
        SetRwEngine(s);
        Call<void>(a, PickIn(s, -100.f, 1800.f), PickIn(s, -100.f, 1100.f), Rnd(s) % 22, Rnd(s) % 300, Rnd(s) % 300, Rnd(s) % 300, Rnd(s) % 300, Rnd(s) % 3);
    });
}
inline json CaseShowRadarMarker(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7204, [](uint32& s, json&, uintptr_t a) {
        FixCamera(s, false);
        Call<void>(a, PickIn(s, -3000.f, 3000.f), PickIn(s, -3000.f, 3000.f), PickIn(s, -50.f, 300.f), Rnd(s), PickIn(s, 0.f, 8.f));
    });
}
inline json CaseDrawRotatingRadarSprite(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7205, [](uint32& s, json&, uintptr_t a) {
        void* sprite = reinterpret_cast<void*>(0xBAA250u + 4u * (Rnd(s) % 64));
        // Estres: anchos/altos que el EXE carga con fild (sin redondear a float) y suma 2^32 si son negativos, y angulos limite (el seno/coseno x87 no se redondea)
        static constexpr uint32 bigDims[] = { 0u, 16777216u, 16777217u, 16777219u, 33554433u, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, 0xFFFFFFFFu, 0xFFFFFF01u };
        static constexpr float specialAngles[] = { 0.f, -0.f, 1e-40f, -1e-40f, 100.f, -1e5f, 1.5707964f, 3.1415927f };
        const auto dim = [&s]() -> uint32 { return (Rnd(s) % 4 == 0) ? bigDims[Rnd(s) % std::size(bigDims)] : Rnd(s) % 60; };
        const float x = PickIn(s, -100.f, 1800.f), y = PickIn(s, -100.f, 1100.f);
        const float angle = (Rnd(s) % 5 == 0) ? specialAngles[Rnd(s) % std::size(specialAngles)] : PickIn(s, -7.f, 7.f);
        const uint32 w = dim(), h = dim();
        Call<void>(a, sprite, x, y, angle, w, h, Rnd(s));
    });
}
inline json CaseDrawYouAreHere(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7206, [](uint32& s, json&, uintptr_t a) {
        FixLegend(s, 170);
        static constexpr uint32 deltas[] = { 0, 199, 200, 201, 499, 500, 501, 700, 1000, 100000 };
        Mem<uint8>(0x8D0930) = uint8(Rnd(s) & 1); Mem<uint32>(0xBAA358) = Mem<uint32>(0xB7CB7C) - deltas[Rnd(s) % std::size(deltas)];
        fixHeading[0] = PickIn(s, -3.2f, 3.2f);
        Call<void>(a, PickIn(s, 0.f, 1800.f), PickIn(s, 0.f, 1100.f));
    });
}
inline json CaseDrawRadarSprite(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7207, [](uint32& s, json&, uintptr_t a) {
        FixPlayers(s); FixLegend(s, 170);
        static constexpr int32 areas[] = { 0, 0, 0, 1, 2 }; Mem<int32>(0xB72914) = areas[Rnd(s) % std::size(areas)];
        Call<void>(a, int32(Rnd(s) % 64), PickIn(s, -100.f, 1800.f), PickIn(s, -100.f, 1100.f), Rnd(s) % 256);
    });
}
inline json CaseDrawRadarSection(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7208, [](uint32& s, json&, uintptr_t a) {
        SetRwEngine(s); FixTxdPool(s); traceUv = true;
        Mem<uint8>(0xA444A4) = uint8(Rnd(s) % 7 == 0);
        const int32 x = int32(Rnd(s) % 18) - 3, y = int32(Rnd(s) % 18) - 3;
        // el origen cerca de la seccion (sus esquinas son (x-6)*500..(x-5)*500 y (5-y)*500..(6-y)*500) para que el recorte no quede vacio casi siempre
        Mem<float>(0xBAA248) = float((x - 6) * 500) + PickIn(s, -300.f, 800.f); Mem<float>(0xBAA24C) = float((5 - y) * 500) + PickIn(s, -300.f, 800.f);
        Call<void>(a, x, y);
        traceUv = false;
    });
}
inline json CaseDrawRadarSectionMap(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7209, [](uint32& s, json&, uintptr_t a) {
        SetRwEngine(s); FixTxdPool(s);
        Call<void>(a, int32(Rnd(s) % 18) - 3, int32(Rnd(s) % 18) - 3, PickIn(s, -100.f, 900.f), PickIn(s, -100.f, 900.f), PickIn(s, -100.f, 900.f), PickIn(s, -100.f, 900.f));
    });
}
inline json CaseDrawAreaOnRadar(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x720A, [](uint32& s, json&, uintptr_t a) {
        SetRwEngine(s); traceUv = false;
        const float ox = Mem<float>(0xBAA248), oy = Mem<float>(0xBAA24C);
        // rectangulo del radar (SetupRadarRect) cerca del origen o, a veces, lejos de el
        const int32 tx = int32(std::floor((ox + 3000.f) * 0.002f)), ty = int32(std::ceil(11.f - (oy + 3000.f) * 0.002f));
        const int32 x0 = (Rnd(s) % 3 == 0) ? int32(Rnd(s) % 14) : tx, y0 = (Rnd(s) % 3 == 0) ? int32(Rnd(s) % 14) : ty;
        Mem<float>(0x8D0920) = float(500 * (x0 - 7)); Mem<float>(0x8D0924) = float(500 * (7 - y0)); Mem<float>(0x8D0928) = float(500 * (x0 - 4)); Mem<float>(0x8D092C) = float(500 * (4 - y0));
        float rect[4];
        if (Rnd(s) % 5 == 0) { rect[0] = PickIn(s, -3500.f, 3500.f); rect[1] = PickIn(s, -3500.f, 3500.f); rect[2] = PickIn(s, -3500.f, 3500.f); rect[3] = PickIn(s, -3500.f, 3500.f); }   // cualquier cosa (incluso invertido)
        else {
            const float w = PickIn(s, 20.f, 700.f), h = PickIn(s, 20.f, 700.f);
            rect[0] = ox - w; rect[2] = ox + w;
            rect[3] = oy - h; rect[1] = oy + h;       // +4 = Y mayor, +0xC = Y menor (como CZone::GetRect)
            if (Rnd(s) % 3 == 0) { rect[0] += PickIn(s, -900.f, 900.f); rect[2] += PickIn(s, -900.f, 900.f); rect[1] += PickIn(s, -900.f, 900.f); rect[3] += PickIn(s, -900.f, 900.f); }
        }
        // Dominio: rectangulo ordenado (izquierda <= derecha, +0xC <= +4). Con uno invertido ClipRadarPoly del EXE (0x585040) puede emitir mas de 8 vertices
        // y pisar la pila de DrawAreaOnRadar (retorno corrupto, eip = un float).
        if (rect[0] > rect[2]) std::swap(rect[0], rect[2]);
        if (rect[3] > rect[1]) std::swap(rect[3], rect[1]);
        uint32 color = Rnd(s);
        Call<void>(a, rect, &color, Rnd(s) & 1);
    });
}
inline json CaseDrawRadarMask(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x720B, [](uint32& s, json&, uintptr_t a) {
        SetRwEngine(s); Mem<float>(0xC80460) = PickIn(s, 0.001f, 0.5f);
        Call<void>(a);
    });
}
}

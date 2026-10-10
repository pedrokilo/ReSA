#pragma once
// Casos de CRadar: tabla de blips (por manejador/indice), matematica del radar, colores, secciones/streaming y estado inicial.
// Todos llaman a la direccion del EXE con la convencion del EXE; con el hook activado esa entrada salta a la DLL (se comprueba con `switches`).
#include "Doubles.hpp"

namespace RadarFinal3Live {

// Llamada cdecl a una direccion del EXE con los tipos de argumento indicados (los floats van por pila como 4 bytes)
template<class R, class... A> inline R Call(uintptr_t addr, A... a) { return reinterpret_cast<R(__cdecl*)(A...)>(addr)(a...); }
// Llamada que conserva el valor de EDX de la funcion llamada: devuelve EDX tras la llamada (para funciones del EXE que solo tocan EAX/ECX)
__declspec(naked) inline uint32 __cdecl CallKeepsEdx(uintptr_t, void*, const void*) {
    __asm {
        mov eax, dword ptr [esp+4]
        mov ecx, dword ptr [esp+0Ch]
        mov edx, dword ptr [esp+8]
        push ecx
        push edx
        mov edx, 12345678h
        call eax
        add esp, 8
        mov eax, edx
        ret
    }
}

// Texturas de las secciones del radar: ranura de TXD aleatoria o -1
inline void FixRadarTextures(uint32& s) {
    for (int i = 0; i < 144; ++i) Mem<int32>(0xBA8478 + 4u * uint32(i)) = (Rnd(s) % 7 == 0) ? -1 : int32(100 + Rnd(s) % 900);
}
// Lista de leyenda con iconos pequenos aleatorios y contador
inline void FixLegend(uint32& s, uint32 maxCount) {
    for (int i = 0; i < 175; ++i) Mem<int16>(0xBA8318 + 2u * uint32(i)) = int16(int32(Rnd(s) % 70) - 6);
    Mem<uint16>(0xBA86B8) = uint16(Rnd(s) % maxCount);
    for (int i = 0; i < 6; ++i) Mem<uint32>(0xBA86D4 + 4u * uint32(i)) = Rnd(s);
}

// Manejador de blip: valido (contador de la entrada), -1, contador distinto, indice con contador ajeno o aleatorio (indice siempre < 175)
inline uint32 PickHandle(uint32& s) {
    auto* base = reinterpret_cast<uint8*>(0xBA86F0);
    const uint32 mode = Rnd(s) % 10, k = Rnd(s) % 175;
    const uint32 counter = At<uint16>(base + k * 0x28, 0x14);
    if (mode < 6) return (counter << 16) | k;
    if (mode == 6) return 0xFFFFFFFFu;
    if (mode == 7) return (((counter + 1) & 0xFFFFu) << 16) | k;
    if (mode == 8) return (counter << 16) | (Rnd(s) % 175);
    return (Rnd(s) & 0xFFFF0000u) | (Rnd(s) % 175);
}

// Caso comun de la tabla de blips: pantalla, temporizador, FrontEnd, radar y tabla aleatorios
template<class F> inline json CaseBlip(unsigned I, bool cand, Hook* hook, uintptr_t addr, uint32 salt, F&& f) {
    uint32 s = Seed(I, salt); FixScreen(s); FixTimer(s); FixFrontEnd(s, -1); FixRadar(s); FixTraces(s, true);
    return Run(hook, cand, addr, [&] { json o = json::object(); f(s, o, addr); return Snap(std::move(o)); });
}
// Variante con los jugadores falsos (DisplayThisBlip y compania usan FindPlayerPed)
inline void FixPlayers(uint32& s, bool vehicle = false, uint32 subtype = 4, int32 model = 460) {
    MakePed(fakePed[0], fakeMat[0], s, (Rnd(s) % 3 == 0) ? Rnd(s) % 4 : 0, (Rnd(s) & 1) != 0); MakePed(fakePed[1], fakeMat[1], s, Rnd(s) % 3, (Rnd(s) & 1) != 0);
    fixPed[0] = fakePed[0].p(); fixPed[1] = fakePed[1].p();
    MakeVehicle(fakeVeh[0], fakeMat[2], s, subtype, model, (Rnd(s) & 1) != 0); MakeVehicle(fakeVeh[1], fakeMat[3], s, 0, 420, (Rnd(s) & 1) != 0);
    fixVeh[0] = vehicle ? fakeVeh[0].p() : nullptr; fixVeh[1] = nullptr;
}

// ---------------- manejadores y tabla de blips ----------------
inline json CaseGetNewUnique(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7101, [](uint32& s, json& o, uintptr_t a) { o["ret"] = Call<uint32>(a, int32(Rnd(s) % 175)); });
}
inline json CaseGetActual(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7102, [](uint32& s, json& o, uintptr_t a) { o["ret"] = Call<int32>(a, PickHandle(s)); });
}
inline void MaybeFillTracking(uint32& s) {
    if (Rnd(s) % 6 == 0) for (int i = 0; i < 175; ++i) At<uint8>(reinterpret_cast<uint8*>(0xBA86F0) + i * 0x28, 0x25) |= 2;   // tabla llena: ningun hueco
}
inline json CaseSetCoordBlip(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7103, [](uint32& s, json& o, uintptr_t a) {
        MaybeFillTracking(s);
        const char* name = (Rnd(s) & 1) ? reinterpret_cast<const char*>(0x866B90) : nullptr;
        o["ret"] = Call<uint32>(a, Rnd(s) % 9, PickIn(s, -3000.f, 3000.f), PickIn(s, -3000.f, 3000.f), PickIn(s, -50.f, 300.f), Rnd(s) % 12, Rnd(s) % 4, name);
    });
}
inline json CaseSetShortRangeCoordBlip(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7104, [](uint32& s, json& o, uintptr_t a) {
        MaybeFillTracking(s);
        const char* name = (Rnd(s) & 1) ? reinterpret_cast<const char*>(0x866B90) : nullptr;
        o["ret"] = Call<uint32>(a, Rnd(s) % 9, PickIn(s, -3000.f, 3000.f), PickIn(s, -3000.f, 3000.f), PickIn(s, -50.f, 300.f), Rnd(s) % 12, Rnd(s) % 4, name);
    });
}
inline json CaseSetEntityBlip(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7105, [](uint32& s, json& o, uintptr_t a) {
        MaybeFillTracking(s);
        o["ret"] = Call<uint32>(a, Rnd(s) % 9, Rnd(s) % 0x1000u, Rnd(s), Rnd(s) % 4);
    });
}
inline json CaseChangeBlipColour(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7106, [](uint32& s, json&, uintptr_t a) { const uint32 h = PickHandle(s); Call<void>(a, h, Rnd(s) % 14); });
}
inline json CaseChangeBlipBrightness(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7107, [](uint32& s, json&, uintptr_t a) { static constexpr int32 v[] = { 0, 1, 2, -1, 5, 1, 0 }; const uint32 h = PickHandle(s); Call<void>(a, h, v[Rnd(s) % std::size(v)]); });
}
inline json CaseChangeBlipScale(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7108, [](uint32& s, json&, uintptr_t a) { const uint32 h = PickHandle(s); Call<void>(a, h, int32(Rnd(s) % 20)); });
}
inline json CaseChangeBlipDisplay(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7109, [](uint32& s, json&, uintptr_t a) { const uint32 h = PickHandle(s); Call<void>(a, h, Rnd(s) % 8); });
}
inline json CaseSetBlipSprite(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x710A, [](uint32& s, json&, uintptr_t a) { const uint32 h = PickHandle(s); Call<void>(a, h, int32(Rnd(s) % 76) - 6); });
}
inline json CaseSetBlipAlways(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x710B, [](uint32& s, json&, uintptr_t a) { const uint32 h = PickHandle(s); Call<void>(a, h, Rnd(s) & 1); });
}
inline json CaseSetBlipFade(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x710C, [](uint32& s, json&, uintptr_t a) { const uint32 h = PickHandle(s); Call<void>(a, h, Rnd(s) & 1); });
}
inline json CaseSetCoordBlipAppearance(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x710D, [](uint32& s, json&, uintptr_t a) {
        const uint32 h = PickHandle(s);
        if ((h & 0xFFFF) < 175 && Rnd(s) % 10 < 6) { uint8* t = reinterpret_cast<uint8*>(0xBA86F0) + (h & 0xFFFF) * 0x28; At<uint8>(t, 0x26) = uint8((At<uint8>(t, 0x26) & ~0x3Cu) | (4u << 2)); }   // tipo COORD
        Call<void>(a, h, Rnd(s) % 5);
    });
}
inline json CaseSetBlipFriendly(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x710E, [](uint32& s, json&, uintptr_t a) { const uint32 h = PickHandle(s); Call<void>(a, h, Rnd(s) & 1); });
}
inline json CaseSetBlipEntryExit(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x710F, [](uint32& s, json&, uintptr_t a) { const uint32 h = PickHandle(s); Call<void>(a, h, (Rnd(s) & 1) ? P(storeEnex.p() + 0x3C * (Rnd(s) % 8)) : Rnd(s)); });
}
inline json CaseClearActual(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7110, [](uint32& s, json&, uintptr_t a) { Call<void>(a, int32(Rnd(s) % 190) - 7); });
}
inline json CaseClearBlipForEntity(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7111, [](uint32& s, json&, uintptr_t a) {
        const uint32 k = Rnd(s) % 175; uint8* t = reinterpret_cast<uint8*>(0xBA86F0) + k * 0x28;
        uint32 type = Rnd(s) % 9, handle = Rnd(s) % 0x1000u;
        if (Rnd(s) % 10 < 7) handle = At<uint32>(t, 4);
        if (Rnd(s) % 10 < 7) type = (At<uint8>(t, 0x26) >> 2) & 0xF;
        if (Rnd(s) % 3 == 0) for (int i = 0; i < 175; ++i) { uint8* u = reinterpret_cast<uint8*>(0xBA86F0) + i * 0x28; if (Rnd(s) % 4 == 0) { At<uint32>(u, 4) = handle; At<uint8>(u, 0x26) = uint8((At<uint8>(u, 0x26) & ~0x3Cu) | (type << 2)); } }
        Call<void>(a, type, handle);
    });
}
inline json CaseClearBlip(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7112, [](uint32& s, json&, uintptr_t a) { Call<void>(a, PickHandle(s)); });
}
inline json CaseDisplayThisBlip(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7113, [](uint32& s, json& o, uintptr_t a) {
        FixPlayers(s);
        static constexpr int32 areas[] = { 0, 0, 0, 1, 2, 4 }; Mem<int32>(0xB72914) = areas[Rnd(s) % std::size(areas)];
        static constexpr int32 priorities[] = { -99, -1, 0, 1, 2, 3, -128, 5, 127, 1, 2, 3 };
        o["ret"] = Call<uint32>(a, int32(Rnd(s) % 80) - 8, priorities[Rnd(s) % std::size(priorities)]) & 0xFF;
    });
}

// ---------------- matematica del radar ----------------
inline json CaseSetupRadarRect(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7120, [](uint32& s, json&, uintptr_t a) { Call<void>(a, int32(Rnd(s) % 28) - 7, int32(Rnd(s) % 28) - 7); });
}
inline json CaseClipRadarTileCoords(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7121, [](uint32& s, json& o, uintptr_t a) {
        int32 x = int32(Rnd(s) % 60) - 24, y = int32(Rnd(s) % 60) - 24;
        o["ret"] = Call<uint32>(a, &x, &y) & 0xFF; o["x"] = x; o["y"] = y;
    });
}
inline float PickEdge(uint32& s) {
    static constexpr uint32 e[] = { 0x3F800000u, 0xBF800000u, 0x3F800001u, 0xBF800001u, 0x3F7FFFFFu, 0xBF7FFFFFu, 0u, 0x80000000u, 0x7FC00000u, 0x3F000000u, 0x40000000u };
    return (Rnd(s) % 3 == 0) ? Flt(e[Rnd(s) % std::size(e)]) : PickIn(s, -1.5f, 1.5f);
}
inline json CaseIsPointInsideRadar(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7122, [](uint32& s, json& o, uintptr_t a) {
        float v[2] = { PickEdge(s), PickEdge(s) };
        o["ret"] = Call<uint32>(a, v) & 0xFF;
    });
}
inline json CaseGetTextureCorners(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7123, [](uint32& s, json& o, uintptr_t a) {
        Buf<0x60> out; out.Fill(0xCD);
        Call<void>(a, int32(Rnd(s) % 22) - 5, int32(Rnd(s) % 22) - 5, out.p());
        o["out"] = out.Dump();
    });
}
inline json CaseLineRadarBox(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7124, [](uint32& s, json& o, uintptr_t a) {
        float st[2], en[2], res[2]; res[0] = res[1] = Flt(0xCDCDCDCDu);
        const uint32 mode = Rnd(s) % 6;
        if (mode == 0) { st[0] = PickEdge(s); st[1] = PickEdge(s); en[0] = PickEdge(s); en[1] = PickEdge(s); }
        else if (mode == 1) { st[0] = PickIn(s, -0.9f, 0.9f); st[1] = PickIn(s, -0.9f, 0.9f); en[0] = PickIn(s, -3.f, 3.f); en[1] = PickIn(s, -3.f, 3.f); }           // empieza dentro
        else if (mode == 2) { st[0] = PickIn(s, -3.f, 3.f); st[1] = PickIn(s, -3.f, 3.f); en[0] = PickIn(s, -0.9f, 0.9f); en[1] = PickIn(s, -0.9f, 0.9f); }           // acaba dentro
        else if (mode == 3) { st[0] = -2.f; st[1] = PickIn(s, -1.2f, 1.2f); en[0] = 2.f; en[1] = PickIn(s, -1.2f, 1.2f); }                                         // atraviesa en X
        else if (mode == 4) { st[0] = PickIn(s, -1.2f, 1.2f); st[1] = -2.f; en[0] = PickIn(s, -1.2f, 1.2f); en[1] = 2.f; }                                         // atraviesa en Y
        else { st[0] = PickIn(s, -3.f, 3.f); st[1] = PickIn(s, -3.f, 3.f); en[0] = PickIn(s, -3.f, 3.f); en[1] = PickIn(s, -3.f, 3.f); }
        o["ret"] = Call<int32>(a, res, st, en); o["res"] = Hex(res, 8);
    });
}
inline json CaseLimitRadarPoint(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7125, [](uint32& s, json& o, uintptr_t a) {
        const float mag = (Rnd(s) % 5 == 0) ? PickEdge(s) : PickIn(s, 0.f, 3.f), ang = PickIn(s, -3.2f, 3.2f);
        float v[2] = { mag * std::cos(ang), mag * std::sin(ang) };
        if (Rnd(s) % 6 == 0) { v[0] = PickEdge(s); v[1] = PickEdge(s); }
        const float ret = Call<float>(a, v);
        o["ret"] = Bits(ret); o["v"] = Hex(v, 8);
    });
}
inline json CaseLimitToMap(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7126, [](uint32& s, json& o, uintptr_t a) {
        float x = PickIn(s, -200.f, 1600.f), y = PickIn(s, -200.f, 1200.f);
        Call<void>(a, &x, &y); o["x"] = Bits(x); o["y"] = Bits(y);
    });
}
inline json CaseCalculateBlipAlpha(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7127, [](uint32& s, json& o, uintptr_t a) {
        static constexpr float sp[] = { 0.f, 1.f, 0.5f, 5.99f, 6.f, 6.01f, 12.f, -1.f, 1e5f, 1.0000001f, 3.f, 100.f };
        const float d = (Rnd(s) % 3 == 0) ? sp[Rnd(s) % std::size(sp)] : PickIn(s, -1.f, 20.f);
        o["ret"] = Call<uint32>(a, d) & 0xFF;
    });
}
inline json CaseTransformToScreen(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7128, [](uint32& s, json& o, uintptr_t a) {
        float in[2] = { PickIn(s, -2.f, 2.f), PickIn(s, -2.f, 2.f) }, out[2] = { Flt(0xCDCDCDCDu), Flt(0xCDCDCDCDu) };
        o["edx"] = CallKeepsEdx(a, out, in); o["out"] = Hex(out, 8);
    });
}
inline json CaseTransformToRadar(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7129, [](uint32& s, json& o, uintptr_t a) {
        float in[2] = { PickIn(s, -3500.f, 3500.f), PickIn(s, -3500.f, 3500.f) }, out[2] = { Flt(0xCDCDCDCDu), Flt(0xCDCDCDCDu) };
        Call<void>(a, out, in); o["out"] = Hex(out, 8);
    });
}
inline json CaseTransformToWorld(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x712A, [](uint32& s, json& o, uintptr_t a) {
        float in[2] = { PickIn(s, -2.f, 2.f), PickIn(s, -2.f, 2.f) }, out[2] = { Flt(0xCDCDCDCDu), Flt(0xCDCDCDCDu) };
        Call<void>(a, out, in); o["out"] = Hex(out, 8);
    });
}
inline json CaseGetRadarTraceColour(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x712B, [](uint32& s, json& o, uintptr_t a) { o["ret"] = Call<uint32>(a, Rnd(s) % 14, Rnd(s) & 1, Rnd(s) & 1); });
}
inline json CaseCalculateCachedSinCos(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x712C, [](uint32& s, json&, uintptr_t a) { FixFrontEnd(s, (Rnd(s) % 5 == 0) ? 1 : 0); FixCamera(s, false); Call<void>(a); });
}
inline json CaseInitFrontEndMap(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x712D, [](uint32& s, json&, uintptr_t a) { FixFrontEnd(s, (Rnd(s) % 3 == 0) ? 1 : 0); FixCamera(s, false); FixLegend(s, 175); Call<void>(a); });
}
inline json CaseAddBlipToLegendList(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x712E, [](uint32& s, json&, uintptr_t a) {
        FixFrontEnd(s, (Rnd(s) % 5 == 0) ? 0 : 1); FixLegend(s, 168);
        const uint32 noSprite = Rnd(s) & 1;
        Call<void>(a, noSprite, noSprite ? int32(Rnd(s) % 175) : int32(Rnd(s) % 64));
    });
}
inline json CaseSetMapCentre(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x712F, [](uint32& s, json&, uintptr_t a) {
        FixPlayers(s); if (Rnd(s) % 8 == 0) fixPed[0] = fixPed[1] = nullptr;
        fixCentre[0] = CVector{ PickIn(s, -3500.f, 3500.f), PickIn(s, -3500.f, 3500.f), PickIn(s, 0.f, 100.f) };
        Mem<uint8>(0xA444A1) = uint8(Rnd(s) % 5 == 0); Mem<uint8>(0xA444A4) = uint8(Rnd(s) % 5 == 0);
        Call<void>(a);
    });
}

// ---------------- streaming / texturas ----------------
inline json CaseRequestMapSection(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7140, [](uint32& s, json&, uintptr_t a) { FixRadarTextures(s); Call<void>(a, int32(Rnd(s) % 20) - 4, int32(Rnd(s) % 20) - 4); });
}
inline json CaseRemoveMapSection(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7141, [](uint32& s, json&, uintptr_t a) { FixRadarTextures(s); Call<void>(a, int32(Rnd(s) % 20) - 4, int32(Rnd(s) % 20) - 4); });
}
inline json CaseRemoveRadarSections(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7142, [](uint32& s, json&, uintptr_t a) { FixRadarTextures(s); Call<void>(a); });
}
inline json CaseStreamXY(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7143, [](uint32& s, json&, uintptr_t a) { FixRadarTextures(s); Call<void>(a, int32(Rnd(s) % 20) - 4, int32(Rnd(s) % 20) - 4); });
}
inline json CaseStreamVec(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7144, [](uint32& s, json&, uintptr_t a) {
        FixRadarTextures(s); Mem<uint8>(0x9654B0) = uint8(Rnd(s) % 5 == 0);
        // mitad de los casos cerca de los limites de las casillas de 500 unidades (suelo / techo del producto por 0.002)
        auto edge = [&] { const float base = float(int32(Rnd(s) % 13) * 500 - 3000); static constexpr float d[] = { 0.f, 0.001f, -0.001f, 0.25f, 499.99f, 250.f }; return (Rnd(s) & 1) ? base + d[Rnd(s) % std::size(d)] : PickIn(s, -3500.f, 3500.f); };
        float v[3] = { edge(), edge(), PickIn(s, 0.f, 100.f) };
        Call<void>(a, v);
    });
}
inline json CaseInitialise(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7145, [](uint32& s, json&, uintptr_t a) { FixRadarTextures(s); FixLegend(s, 175); Mem<uint8>(0xBA8300) = uint8(Rnd(s)); Mem<uint32>(0xBA8304) = Rnd(s); Call<void>(a); });
}
inline json CaseShutdown(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7146, [](uint32& s, json&, uintptr_t a) { FixRadarTextures(s); Call<void>(a); });
}
inline json CaseLoadTextures(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    return CaseBlip(I, cand, hook, addr, 0x7147, [](uint32&, json&, uintptr_t a) { Call<void>(a); });
}
}

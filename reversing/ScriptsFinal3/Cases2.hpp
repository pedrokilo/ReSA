#pragma once
// Casos: lista de scripts (StartNewScript, indices), utilidades de scripts y dibujo (esferas, rectangulos de script, focos, areas resaltadas, sondas de depuracion).
#include "Cases1.hpp"

namespace ScriptsFinal3Live {

constexpr uintptr_t RECTS = 0xA92D68, TEXT_LINES = 0xA913E8, ONMISSION_FLAG = 0xA476AC;

// Construye las dos listas de scripts como las deja Init (96 scripts); `nActive` de ellos en la lista de activos (en orden aleatorio)
inline void BuildScripts(uint32& s, uint32 nActive, bool markExternal) {
    FillRandom(SCRIPTS_ARRAY, 96 * 0xE0, s);
    std::array<uint8, 96> order{}; for (uint32 i = 0; i < 96; ++i) order[i] = uint8(i);
    for (uint32 i = 95; i > 0; --i) std::swap(order[i], order[Rnd(s) % (i + 1)]);
    auto S = [](uint32 i) { return reinterpret_cast<uint8*>(SCRIPTS_ARRAY + 0xE0u * i); };
    uint32 prev = 0xFFFFFFFFu;
    for (uint32 k = 0; k < nActive; ++k) {           // lista de activos
        uint8* sc = S(order[k]);
        At<uint32>(sc, 0) = 0; At<uint32>(sc, 4) = prev == 0xFFFFFFFFu ? 0u : P(S(prev));
        if (prev != 0xFFFFFFFFu) At<uint32>(S(prev), 0) = P(sc);
        sc[0xC4] = 1; prev = order[k];
        At<uint32>(sc, 0x14) = SCRIPT_SPACE + Rnd(s) % 100000;
        for (uint32 j = 0; j < 8; ++j) At<uint32>(sc, 0x18 + 4 * j) = Rnd(s) % 3 ? 0u : SCRIPT_SPACE + Rnd(s) % 100000;
        if (markExternal) { sc[0xC7] = Rnd(s) % 4 == 0; sc[0xC9] = Rnd(s) % 4 == 0 ? uint8(Rnd(s) % 5) : 0xFF; }
        else { sc[0xC7] = 0; sc[0xC9] = 0xFF; }
    }
    Mem<uint32>(PENDING_ACTIVE) = nActive ? P(S(order[0])) : 0u;
    prev = 0xFFFFFFFFu;
    for (uint32 k = nActive; k < 96; ++k) {          // lista de inactivos
        uint8* sc = S(order[k]);
        At<uint32>(sc, 0) = 0; At<uint32>(sc, 4) = prev == 0xFFFFFFFFu ? 0u : P(S(prev));
        if (prev != 0xFFFFFFFFu) At<uint32>(S(prev), 0) = P(sc);
        sc[0xC4] = 0; prev = order[k];
    }
    Mem<uint32>(PENDING_IDLE) = nActive < 96 ? P(S(order[nActive])) : 0u;
}
inline uint32 ScriptIdxOrNone(const void* p) { return p ? ScriptIdx(p) : 0xFFFFFFFFu; }
inline json ScriptsOut(uint64 ret) {
    json o = Base(ret); o["active_head"] = ScriptIdxOrNone(reinterpret_cast<void*>(Mem<uint32>(PENDING_ACTIVE))); o["idle_head"] = ScriptIdxOrNone(reinterpret_cast<void*>(Mem<uint32>(PENDING_IDLE))); return o;
}

inline json CaseStartNewScript(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2001); BuildScripts(s, Rnd(s) % 96, false);
    uint8* ip = reinterpret_cast<uint8*>(SCRIPT_SPACE + Rnd(s) % 200000);
    return Run(hook, cand, addr, [&] {
        auto* r = reinterpret_cast<void*(__cdecl*)(uint8*)>(addr)(ip);
        return ScriptsOut(ScriptIdxOrNone(r));
    });
}
inline json CaseStartNewScriptIdx(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2002); BuildScripts(s, Rnd(s) % 97 > 95 ? 95 : Rnd(s) % 96, false);
    if (Rnd(s) % 16 == 0) { Mem<uint32>(PENDING_IDLE) = 0; }             // lista de inactivos vacia
    uint8* ip = Rnd(s) % 3 == 0 ? nullptr : reinterpret_cast<uint8*>(SCRIPT_SPACE + Rnd(s) % 200000);
    const uint32 index = Rnd(s) % 96;
    return Run(hook, cand, addr, [&] {
        auto* r = reinterpret_cast<void*(__cdecl*)(uint8*, uint16)>(addr)(ip, uint16(index));
        return ScriptsOut(ScriptIdxOrNone(r));
    });
}
inline json CaseStartTestScript(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2003); BuildScripts(s, Rnd(s) % 96, false);
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); return ScriptsOut(0); });
}
inline json CaseGetScriptIndex(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2004); const uint32 idx = Rnd(s) % 96;
    return Run(hook, cand, addr, [&] {
        const int32 r = reinterpret_cast<int32(__cdecl*)(void*)>(addr)(reinterpret_cast<void*>(SCRIPTS_ARRAY + 0xE0u * idx));
        return Base(uint32(r));
    });
}
inline json CasePrintListSizes(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2005); BuildScripts(s, Rnd(s) % 97 > 96 ? 96 : Rnd(s) % 97, false);
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); return ScriptsOut(0); });
}
inline json CaseIsPlayerOnMission(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2006); FillRandom(SCRIPT_SPACE, 0x1000, s);
    const uint32 off = Rnd(s) % 4 == 0 ? 0u : 8u + (Rnd(s) % 0x400) * 4u;
    Mem<int32>(ONMISSION_FLAG) = int32(off);
    Mem<uint32>(SCRIPT_SPACE + off) = Rnd(s) % 2 ? 1u : Rnd(s);
    return Run(hook, cand, addr, [&] { return Base(reinterpret_cast<bool(__cdecl*)()>(addr)() ? 1 : 0); });
}
inline json CaseWipeLocals(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2007); FillRandom(0xA48960, 4096 + 64, s);
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); json o = Base(0); Diag(o, "locals", 0xA48960, 4096 + 64); return o; });
}
inline json CaseCheckStreamedVersion(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2008); const uint32 a = Rnd(s), b = Rnd(s);
    return Run(hook, cand, addr, [&] { return Base(reinterpret_cast<bool(__cdecl*)(uint32, uint32)>(addr)(a, b) ? 1 : 0); });
}

// =====================================================================================================================
// Dibujo
// =====================================================================================================================
inline json CaseDrawSpheres(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2010); FillRandom(SPHERES, 16 * 0x18, s);
    for (uint32 i = 0; i < 16; ++i) { uint8* e = reinterpret_cast<uint8*>(SPHERES) + 0x18 * i; e[0] = Rnd(s) % 2; SetVec(e, 8, PickWorld(s), PickWorld(s), PickWorld(s)); At<float>(e, 0x14) = float(Rnd(s) % 400) / 8.f; }
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); return Base(0); });
}
inline json CaseRenderSearchLights(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2011); FillSearchLights(s, false);
    for (uint32 i = 0; i < N_ENT; ++i) { uint8* m = Mat(i); RandMatrix(m, s); EntInit(EntAt(i), 0x40, 2, 0, m); }   // todas las entidades con matriz propia
    for (uint32 i = 0; i < 8; ++i) {
        uint8* e = reinterpret_cast<uint8*>(SEARCHLIGHTS) + 0x7C * i;
        e[0] = Rnd(s) % 3 != 0;
        for (size_t off : { 0x44u, 0x48u, 0x4Cu, 0x50u }) At<uint32>(e, off) = 0;
        At<uint32>(e, 0x44) = Rnd(s) % 3 == 0 ? P(EntAt(Rnd(s) % N_ENT)) : 0u;                 // entidad adjunta
        At<uint32>(e, 0x54) = Rnd(s) % 3 == 0 ? P(EntAt(Rnd(s) % N_ENT)) : 0u;                 // bombilla
    }
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); return Base(0); });
}
inline float PickMaybeNaN(uint32& s) { return Rnd(s) % 40 == 0 ? std::bit_cast<float>(0x7FC00000u) : PickWorld(s); }
inline json CaseHighlightArea(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2012);
    fx.groundZ = PickWorld(s);
    float x1 = PickMaybeNaN(s), y1 = PickMaybeNaN(s), x2 = Rnd(s) % 6 == 0 ? x1 : PickMaybeNaN(s), y2 = Rnd(s) % 6 == 0 ? y1 : PickMaybeNaN(s);
    static constexpr float zs[] = { -100.f, -100.5f, -99.5f, -1000.f, 0.f, 25.f };
    const float z = Rnd(s) % 3 == 0 ? zs[Rnd(s) % std::size(zs)] : (Rnd(s) % 2 ? -100.f - float(Rnd(s) % 100) : PickWorld(s));
    const uint32 id = Rnd(s);
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(uint32, float, float, float, float, float)>(addr)(id, x1, y1, x2, y2, z); return Base(0); });
}
inline json CaseHighlightAngled(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2013);
    fx.groundZ = PickWorld(s);
    float p[8]; for (auto& v : p) v = Rnd(s) % 8 == 0 ? PickNear(s) * 10.f : PickMaybeNaN(s);
    if (Rnd(s) % 6 == 0) { p[2] = p[0]; p[3] = p[1]; }
    static constexpr float zs[] = { -100.f, -100.5f, -99.5f, -1000.f, 0.f, 25.f };
    const float z = Rnd(s) % 3 == 0 ? zs[Rnd(s) % std::size(zs)] : (Rnd(s) % 2 ? -100.f - float(Rnd(s) % 100) : PickWorld(s));
    const uint32 id = Rnd(s);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(uint32, float, float, float, float, float, float, float, float, float)>(addr)(id, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], z);
        return Base(0);
    });
}
inline json CaseDebugSquare(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2014); float p[4]; for (auto& v : p) v = Rnd(s) % 10 == 0 ? PickNear(s) : PickWorld(s);
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(float, float, float, float)>(addr)(p[0], p[1], p[2], p[3]); return Base(0); });
}
inline json CaseDebugAngledSquare(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2015); float p[8]; for (auto& v : p) v = Rnd(s) % 10 == 0 ? PickNear(s) : PickWorld(s);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(float, float, float, float, float, float, float, float)>(addr)(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
        return Base(0);
    });
}
inline json CaseDebugLine(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2016); float a[3] = { PickWorld(s), PickWorld(s), PickWorld(s) }, b[3] = { PickWorld(s), PickWorld(s), PickWorld(s) }; const uint32 c1 = Rnd(s), c2 = Rnd(s);
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(const float*, const float*, uint32, uint32)>(addr)(a, b, c1, c2); return Base(0); });
}
inline json CaseDebugCircle(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2017);
    InstallFakeEngine();
    Mem<uint32>(0xB7CB84) = Rnd(s);                                  // la parte resaltada sale de (milisegundos >> 6) & 0xF
    const float x = PickWorld(s), y = PickWorld(s), w = float(Rnd(s) % 600) / 4.f, h = float(Rnd(s) % 600) / 4.f; const uint32 color = Rnd(s);
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(float, float, float, float, uint32)>(addr)(x, y, w, h, color); return Base(0); });
}

// ---- rectangulos de script
inline void FillRects(uint32& s) {
    for (uint32 i = 0; i < 128; ++i) {
        uint8* r = reinterpret_cast<uint8*>(RECTS) + 0x3C * i;
        for (uint32 k = 0; k < 0x3C; ++k) r[k] = uint8(Rnd(s));
        At<int32>(r, 0) = int32(Rnd(s) % 6);                         // tipo 0..5
        r[4] = Rnd(s) & 1;                                           // dibujar antes del fundido
        At<int16>(r, 6) = int16(Rnd(s) % 128);                       // textura
        SetVec(r, 8, PickWorld(s), PickWorld(s), 0.f); At<float>(r, 0x14) = PickWorld(s);   // esquinas A (x, y) y B (x, y)
        At<float>(r, 0x10) = PickWorld(s);
        At<float>(r, 0x18) = Rnd(s) % 4 == 0 ? PickNear(s) : PickM(s);                    // angulo
        r[0x27] = 0; r[0x31] = 0;                                    // gxt1 y gxt2 terminan en NUL dentro de sus 8 bytes
        At<uint8>(r, 0x34) = uint8(Rnd(s) % 4); At<uint8>(r, 0x35) = 0; At<uint8>(r, 0x36) = 0; At<uint8>(r, 0x37) = 0;   // alineacion
        At<uint32>(r, 0x38) = Rnd(s) % 5;
    }
}
inline json CaseDrawRects(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2018); FillRects(s); const bool before = Rnd(s) & 1;
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(bool)>(addr)(before); return Base(0); });
}
}

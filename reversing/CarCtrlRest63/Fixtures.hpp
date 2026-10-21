#pragma once
// Fixtures del banco CarCtrlRest63 (lote 63, CCarCtrl parte C): funciones de CCarCtrl, 4096 casos cada una. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp y fixture_mirror.cpp (programa autonomo de calibracion).
//
//   0 CCarCtrl::ThisVehicleShouldTryNotToTurn     0x421FE0  cdecl (CVehicle*), bool en AL
//   1 CCarCtrl::FindIntersection2Lines            0x4226F0  cdecl (8 floats, float*, float*), void; un llamador del EXE lee EDX tras la llamada
//   2 CCarCtrl::FindSpeedMultiplier               0x4224E0  cdecl (4 floats), float en ST0
//   3 CCarCtrl::FindPercDependingOnDistToLink     0x422620  cdecl (CVehicle*, CCarPathLinkAddress), float en ST0
//   4 CCarCtrl::FindPathDirection                 0x422090  cdecl (3 CNodeAddress, bool*), int8 en AL
//   5 CCarCtrl::StopCarIfNodesAreInvalid          0x422590  cdecl (CVehicle*), bool en AL
//   6 CCarCtrl::SwitchBetweenPhysicsAndGhost      0x4222A0  cdecl (CVehicle*), void
//
// Todos los floats viajan como patrones de 32 bits sin signo. Politica de especiales: a lo sumo UN campo NaN con carga util propia por caso.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace Cc63Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4096;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

// ------------------------------------------------------------------------------------------------ JSON minimo (mismas filas `input` en el juego y en el espejo)
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& A(const char* k, const std::vector<u32>& v) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < v.size(); ++i) { if (i) s += ','; s += std::to_string(v[i]); }
        s += ']'; return *this;
    }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ floats
constexpr u32 NanBits(u32 slot, bool signaling) {
    const u32 sign = (slot & 1u) << 31;
    return signaling ? (sign | 0x7F800000u | (0x2000u + slot * 0x41u)) : (sign | 0x7FC00000u | (0x100u + slot * 0x37u));
}
inline constexpr std::array<u32, 20> kFloatEdges{
    0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3DCCCCCDu, 0xBDCCCCCDu, 0x3D4CCCCDu, 0x00000001u, 0x80000001u, 0x007FFFFFu,
    0x807FFFFFu, 0x00800000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x3F000000u, 0x40000000u, 0x3E000000u, 0x45000000u, 0x00400000u, 0x80400000u};
inline u32 OrdinaryFloat(u32 n, u32 k, int minExp, int maxExp) {   // exponentes sesgados (127 = 1.0)
    const u32 sign = Rand(n, k) & 1u;
    const u32 exp = u32(minExp) + Rand(n, k + 1) % u32(maxExp - minExp + 1);
    return (sign << 31) | (exp << 23) | (Rand(n, k + 2) & 0x7FFFFFu);
}
inline u32 TinyFloat(u32 n, u32 k) {   // denormal o casi
    const u32 sign = Rand(n, k) & 1u;
    const u32 exp = Rand(n, k + 1) % 3 == 0 ? Rand(n, k + 3) % 6u : 0u;
    return (sign << 31) | (exp << 23) | (Rand(n, k + 2) & 0x7FFFFFu);
}
inline u32 HugeFloat(u32 n, u32 k) {
    const u32 sign = Rand(n, k) & 1u;
    return (sign << 31) | ((200u + Rand(n, k + 1) % 55u) << 23) | (Rand(n, k + 2) & 0x7FFFFFu);   // hasta 0x7F7FFFFF
}
inline u32 InfBits(u32 n, u32 k) { return Rand(n, k) & 1u ? 0xFF800000u : 0x7F800000u; }
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
inline bool IsInfBits(u32 b) { return (b & 0x7FFFFFFFu) == 0x7F800000u; }
inline float F(u32 b) { float f; std::memcpy(&f, &b, 4); return f; }
inline u32 B(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
// Valor de relleno para los registros de entrada (EAX/ECX/EDX)
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Resultado de bool de un colaborador: falso = 0 con basura en los bits altos, o AL != 0 con basura en los bits altos
inline u32 BoolRet(u32 n, u32 k, bool wantTrue) {
    static constexpr u32 als[]{1u, 2u, 0x80u, 0xFFu, 0x7Fu, 0x81u};
    if (!wantTrue) return Rand(n, k + 1) % 2 ? 0u : (Rand(n, k + 2) & 0xFFFFFF00u);
    return (Rand(n, k + 3) % 2 ? 0u : (Rand(n, k + 4) & 0xFFFFFF00u)) | als[Rand(n, k + 5) % std::size(als)];
}

// ------------------------------------------------------------------------------------------------ arenas (suma a mano: guarda + ventana + guarda)
constexpr u32 kGuard = 0x40;
// Vehiculo: ventana de 0x600 bytes con guardas de 0x40 a cada lado. 0x40 + 0x600 + 0x40 = 0x680.
constexpr u32 kVehWindow = 0x600, kVehArena = kGuard + kVehWindow + kGuard;
// Matriz falsa (solo +0x30 = posicion): ventana de 0x40, guardas de 0x20. 0x20 + 0x40 + 0x20 = 0x80.
constexpr u32 kMatGuard = 0x20, kMatWindow = 0x40, kMatArena = kMatGuard + kMatWindow + kMatGuard;
// Salidas de FindIntersection2Lines: 16 palabras (guarda de 4 + ventana de 8 + guarda de 4 = 16). Las ranuras estan en las palabras 4 y 5 (o 4 y 4, o 4 y 5 contiguas).
constexpr u32 kOutWords = 16, kOutArena = kOutWords * 4;   // 0x40
constexpr u32 kOutSlot1 = 4 * 4, kOutSlot2 = 6 * 4;       // desplazamientos de out1 y out2 (distintas); contiguas: out2 = out1 + 4
// Enlaces de ruta (14 bytes cada uno): ventana de 4 enlaces con guardas de 2 enlaces. 28 + 56 + 28 = 112 (0x70).
constexpr u32 kLinkBytes = 14, kLinkWindow = 4 * kLinkBytes, kLinkGuard = 2 * kLinkBytes, kLinkArena = kLinkGuard + kLinkWindow + kLinkGuard;
// Nodos de ruta (0x1C bytes cada uno): 4 areas de 8 nodos con guarda de 1 nodo a cada lado. 0x1C + 4*8*0x1C (0x380) + 0x1C = 0x3B8.
constexpr u32 kNodeBytes = 0x1C, kNodesPerArea = 8, kNodeAreas = 4, kNodeGuard = kNodeBytes, kNodeArena = kNodeGuard + kNodeAreas * kNodesPerArea * kNodeBytes + kNodeGuard;
// Bandera de FindPathDirection: 16 bytes, el flag en +8
constexpr u32 kFlagArena = 16, kFlagAt = 8;
constexpr u32 kPoolAreas = 72;   // NUM_TOTAL_PATH_NODE_AREAS (0x96F854 .. 0x96F974)

// ================================================================================================ 0: ThisVehicleShouldTryNotToTurn
struct C0 { u32 family{}, model{}, eax{}, ecx{}, edx{}, stack{}; };
inline bool TrueModel0(u32 model16) {
    const i32 m = i32(i16(model16));
    switch (m) { case 403: case 406: case 431: case 437: case 443: case 455: case 514: case 515: case 524: return true; default: return false; }
}
inline C0 MakeC0(u32 n) {
    static constexpr u32 yes[]{403, 406, 431, 437, 443, 455, 514, 515, 524};
    C0 c; const u32 f = n % 16; c.family = f;
    switch (f) {
    case 0: case 1: c.model = yes[Rand(n, 1) % 9]; break;
    case 2: case 3: c.model = u32(i32(yes[Rand(n, 1) % 9]) + i32(Rand(n, 2) % 5) - 2) & 0xFFFFu; break;
    case 4: case 5: c.model = 380 + Rand(n, 1) % 180; break;
    case 6: c.model = 0xFFFFu - Rand(n, 1) % 600; break;
    case 7: c.model = Rand(n, 1) & 0xFFFFu; break;
    case 8: c.model = 520 + Rand(n, 1) % 12; break;
    case 9: c.model = 398 + Rand(n, 1) % 12; break;
    case 10: c.model = Rand(n, 1) % 2 ? 0x8000u + Rand(n, 2) % 8 : 0x7FFFu - Rand(n, 2) % 8; break;
    case 11: c.model = Rand(n, 1) % 700; break;
    default: c.model = Rand(n, 1) % 3 == 0 ? yes[Rand(n, 2) % 9] : 395 + Rand(n, 3) % 150; break;
    }
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC0(const C0& c) {
    Js j; j.U("family", c.family).U("model", c.model).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 1: FindIntersection2Lines
struct C1 { u32 family{}, outMode{}, a[8]{}, stale[2]{}, eax{}, ecx{}, edx{}, stack{}; };
inline C1 MakeC1(u32 n) {
    C1 c; const u32 f = n % 16; c.family = f;
    auto ord = [&](u32 k, int lo, int hi) { return OrdinaryFloat(n, k, u32(lo), u32(hi)); };
    for (u32 i = 0; i < 8; ++i) c.a[i] = ord(10 + 4 * i, 110, 140);
    switch (f) {
    case 0: break;
    case 1: {   // paralelas exactas: (a7, a8) = k * (a3, a4) con k potencia de dos
        const float k = std::ldexp(1.0f, i32(Rand(n, 60) % 7) - 3) * (Rand(n, 61) % 2 ? 1.0f : -1.0f);
        c.a[6] = B(F(c.a[2]) * k); c.a[7] = B(F(c.a[3]) * k); break; }
    case 2: for (u32 i = 0; i < 8; ++i) if (Rand(n, 70 + i) % 2) c.a[i] = TinyFloat(n, 80 + 4 * i); break;
    case 3: for (u32 i = 0; i < 8; ++i) if (Rand(n, 70 + i) % 2) c.a[i] = HugeFloat(n, 80 + 4 * i); break;
    case 4: c.a[Rand(n, 60) % 8] = NanBits(Rand(n, 61) % 6, Rand(n, 62) % 3 == 0); break;
    case 5: c.a[Rand(n, 60) % 8] = InfBits(n, 61); if (Rand(n, 62) % 3 == 0) c.a[Rand(n, 63) % 8] = InfBits(n, 64); break;
    case 6: for (u32 i = 0; i < 8; ++i) c.a[i] = kFloatEdges[Rand(n, 60 + i) % kFloatEdges.size()]; break;
    case 7: for (u32 i = 0; i < 8; ++i) c.a[i] = B(float(i32(Rand(n, 60 + i) % 41) - 20)); break;   // enteros pequenos
    case 8: {   // casi paralelas: denominador diminuto
        c.a[6] = B(F(c.a[2]) * 0.5f); c.a[7] = B(F(c.a[3]) * 0.5f * (1.0f + std::ldexp(1.0f, -22 + i32(Rand(n, 60) % 3)))); break; }
    case 9: for (u32 i = 0; i < 8; ++i) c.a[i] = ord(60 + 4 * i, 80, 100); break;      // pequenos (productos subnormales en float)
    case 10: for (u32 i = 0; i < 8; ++i) c.a[i] = ord(60 + 4 * i, 150, 170); break;    // grandes (productos desbordan en float)
    case 11: for (u32 i = 0; i < 8; ++i) c.a[i] = Rand(n, 60 + i) % 3 == 0 ? 0u : ord(60 + 4 * i, 120, 135); break;   // ceros
    default: for (u32 i = 0; i < 8; ++i) c.a[i] = ord(60 + 4 * i, 100 + i32(f), 140 + i32(f)); break;
    }
    c.outMode = Rand(n, 90) % 12 == 0 ? 1u : Rand(n, 91) % 12 == 0 ? 2u : 0u;   // 0 distintas, 1 la misma direccion, 2 contiguas
    c.stale[0] = Rand(n, 92); c.stale[1] = Rand(n, 93);
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC1(const C1& c) {
    Js j; j.U("family", c.family).U("out_mode", c.outMode).A("a", {c.a[0], c.a[1], c.a[2], c.a[3], c.a[4], c.a[5], c.a[6], c.a[7]}).A("stale", {c.stale[0], c.stale[1]})
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 2: FindSpeedMultiplier
struct C2 { u32 family{}, a[4]{}, eax{}, ecx{}, edx{}, stack{}; };
inline C2 MakeC2(u32 n) {
    C2 c; const u32 f = n % 16; c.family = f;
    auto ord = [&](u32 k, int lo, int hi) { return OrdinaryFloat(n, k, u32(lo), u32(hi)); };
    auto pos = [&](u32 k, int lo, int hi) { return ord(k, lo, hi) & 0x7FFFFFFFu; };
    c.a[0] = ord(10, 120, 130);                   // |a1| < 16
    c.a[1] = pos(14, 110, 130); c.a[2] = pos(18, 118, 135); c.a[3] = ord(22, 118, 128);
    switch (f) {
    case 0: break;
    case 1: {   // alrededor de +-pi, +-2pi y +-3pi
        static constexpr u32 e[]{0x40490FDBu, 0x40490FDAu, 0x40490FDCu, 0xC0490FDBu, 0xC0490FDAu, 0xC0490FDCu, 0x40C90FDBu, 0xC0C90FDBu, 0x40C90FDAu, 0xC0C90FDCu, 0x4116CBE4u, 0xC116CBE4u};
        c.a[0] = e[Rand(n, 30) % std::size(e)]; break; }
    case 2: c.a[0] = ord(30, 127, 146); break;        // hasta ~5e5 (decenas de miles de vueltas, acotado)
    case 3: c.a[0] = ord(30, 127, 140); c.a[1] = Rand(n, 31) % 2 ? 0u : c.a[1]; break;
    case 4: c.a[0] = NanBits(Rand(n, 30) % 6, Rand(n, 31) % 3 == 0); break;
    case 5: c.a[0] = TinyFloat(n, 30); break;
    case 6: c.a[Rand(n, 30) % 2 + 1] = NanBits(Rand(n, 31) % 6, Rand(n, 32) % 3 == 0); break;
    case 7: c.a[2] = c.a[1]; break;                   // r = 0
    case 8: c.a[2] = B(F(c.a[1]) * 0.25f); break;     // r pequeno: d > r frecuente
    case 9: c.a[2] = B(-F(c.a[2])); break;            // r negativo
    case 10: c.a[3] = Rand(n, 30) % 3 == 0 ? NanBits(Rand(n, 31) % 6, Rand(n, 32) % 2 == 0) : Rand(n, 30) % 3 == 1 ? InfBits(n, 33) : TinyFloat(n, 34); break;
    case 11: c.a[1] = Rand(n, 30) % 2 ? InfBits(n, 31) : HugeFloat(n, 32); break;
    case 12: c.a[1] = TinyFloat(n, 30); c.a[2] = TinyFloat(n, 34); break;
    case 13: {   // a1 enorme o infinito haria que el bucle de +-2pi del EXE no terminase nunca (x + 2pi == x): nunca se prueba
        const u32 k = Rand(n, 30) % 4; u32 e = kFloatEdges[Rand(n, 31) % kFloatEdges.size()];
        if (k == 0 && (e & 0x7FFFFFFFu) >= 0x4B800000u) e = 0x40000000u;
        c.a[k] = e; break; }
    case 14: c.a[2] = HugeFloat(n, 30); c.a[1] = Rand(n, 34) % 2 ? 0u : c.a[1]; break;
    default: c.a[0] = ord(30, 100, 135); c.a[3] = ord(34, 100, 135); break;
    }
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC2(const C2& c) {
    Js j; j.U("family", c.family).A("a", {c.a[0], c.a[1], c.a[2], c.a[3]}).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 3: FindPercDependingOnDistToLink
struct C3 { u32 family{}, area{}, id{}, junkHi{}, matMode{}, lx{}, ly{}, pos[2]{}, decoy[2]{}, eax{}, ecx{}, edx{}, stack{}; };
inline C3 MakeC3(u32 n) {
    C3 c; const u32 f = n % 16; c.family = f;
    c.area = Rand(n, 1) % 64; c.id = Rand(n, 2) % 1024; c.junkHi = Rand(n, 3) & 0xFFFFu;
    c.matMode = Rand(n, 4) % 2;   // 0: m_matrix nulo (posicion en +4), 1: matriz (posicion en +0x30)
    c.lx = u32(i32(Rand(n, 5) % 65536u) - 32768); c.ly = u32(i32(Rand(n, 6) % 65536u) - 32768);
    if (f % 4 == 0) { c.lx = u32(i32(Rand(n, 5) % 2001u) - 1000); c.ly = u32(i32(Rand(n, 6) % 2001u) - 1000); }
    const float X = float(i32(c.lx)) * 0.125f, Y = float(i32(c.ly)) * 0.125f;
    float dist;
    switch (f) {
    case 0: case 1: case 2: dist = std::ldexp(1.0f, i32(Rand(n, 7) % 7)) * (0.75f + float(Rand(n, 8) % 512) / 1024.0f); break;
    case 3: { static constexpr float t[]{0.0f, 4.9999995f, 5.0f, 5.0000005f, 14.999999f, 15.0f, 15.000002f, 4.0f, 6.0f, 14.0f, 16.0f}; dist = t[Rand(n, 7) % std::size(t)]; break; }
    case 4: dist = float(Rand(n, 7) % 400) / 16.0f; break;
    default: dist = float(Rand(n, 7) % 4000) / 100.0f; break;
    }
    const float ang = float(Rand(n, 9) % 4096) * (6.2831853f / 4096.0f);
    const float px = X + dist * std::cos(ang), py = Y + dist * std::sin(ang);
    c.pos[0] = B(px); c.pos[1] = B(py);
    switch (f) {
    case 5: c.pos[Rand(n, 10) % 2] = NanBits(Rand(n, 11) % 6, Rand(n, 12) % 3 == 0); break;
    case 6: c.pos[Rand(n, 10) % 2] = InfBits(n, 11); break;
    case 7: c.pos[0] = TinyFloat(n, 10); c.pos[1] = TinyFloat(n, 14); c.lx = Rand(n, 15) % 3; c.ly = Rand(n, 16) % 3; break;
    case 8: c.pos[Rand(n, 10) % 2] = HugeFloat(n, 11); break;
    case 9: c.pos[0] = OrdinaryFloat(n, 10, 100, 160); c.pos[1] = OrdinaryFloat(n, 14, 100, 160); break;
    case 10: c.pos[0] = B(X); c.pos[1] = B(Y); break;   // dist = 0
    case 11: c.pos[0] = kFloatEdges[Rand(n, 10) % kFloatEdges.size()]; c.pos[1] = kFloatEdges[Rand(n, 11) % kFloatEdges.size()]; break;
    default: break;
    }
    c.decoy[0] = OrdinaryFloat(n, 20, 120, 135); c.decoy[1] = OrdinaryFloat(n, 24, 120, 135);
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC3(const C3& c) {
    Js j; j.U("family", c.family).U("area", c.area).U("id", c.id).U("junk_hi", c.junkHi).U("mat_mode", c.matMode).U("lx", c.lx).U("ly", c.ly)
     .A("pos", {c.pos[0], c.pos[1]}).A("decoy", {c.decoy[0], c.decoy[1]}).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 4: FindPathDirection
// Cuatro areas (ranuras 0..3) con 8 nodos cada una; cada referencia (a1, a2, a3) = (area, nodo). Las posiciones salen del falso de CPathNode::GetPosition (0x420A10).
// refSlot: 0..3 = area cargada de esa ranura, 4 = area del juego sin cargar, 5 = area 0xFFFF.
struct C4 {
    u32 family{}, perturbed{}, flagInit{};
    u32 areaIds[kNodeAreas]{};          // numero de area del juego de cada ranura (distintos, < 72)
    u32 ref[3][2]{};                    // (area de 16 bits, nodo 0..7)
    u32 refSlot[3]{};
    u32 pos[3][3]{};                    // posicion de la referencia r (x, y, z); si dos referencias comparten nodo gana la posterior
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline C4 MakeC4(u32 n) {
    C4 c; const u32 f = n % 16; c.family = f;
    c.perturbed = f >= 12 ? 1u : 0u;
    c.flagInit = Rand(n, 2) % 3 == 0 ? Rand(n, 3) & 0xFFu : (Rand(n, 3) % 2);
    u32 perm[kPoolAreas];
    for (u32 i = 0; i < kPoolAreas; ++i) perm[i] = i;
    for (u32 i = 0; i < 8; ++i) { const u32 j = i + Rand(n, 10 + i) % (kPoolAreas - i); std::swap(perm[i], perm[j]); }
    for (u32 s = 0; s < kNodeAreas; ++s) c.areaIds[s] = perm[s];
    for (u32 r = 0; r < 3; ++r) {
        const u32 sel = Rand(n, 20 + r) % 100;
        c.ref[r][1] = Rand(n, 24 + r) % kNodesPerArea;
        if (sel < 7) { c.refSlot[r] = 5; c.ref[r][0] = 0xFFFFu; }
        else if (sel < 14) { c.refSlot[r] = 4; c.ref[r][0] = perm[4 + r]; }
        else { c.refSlot[r] = Rand(n, 34 + r) % kNodeAreas; c.ref[r][0] = c.areaIds[c.refSlot[r]]; }
    }
    if (f == 15) { c.refSlot[1] = c.refSlot[0]; c.ref[1][0] = c.ref[0][0]; c.ref[1][1] = c.ref[0][1]; }   // misma referencia para 1 y 2
    auto ordf = [&](u32 k, int lo, int hi) { return OrdinaryFloat(n, k, u32(lo), u32(hi)); };
    float p[3][2];
    const float sc = std::ldexp(1.0f, i32(Rand(n, 40) % 6) - 1);
    switch (f) {
    case 1: p[0][0] = 0; p[0][1] = 0; p[1][0] = sc; p[1][1] = 0; p[2][0] = 2 * sc; p[2][1] = 0; break;                                  // recto
    case 2: p[0][0] = 0; p[0][1] = 0; p[1][0] = sc; p[1][1] = 0; p[2][0] = sc; p[2][1] = Rand(n, 41) % 2 ? sc : -sc; break;            // giro de 90
    case 3: p[0][0] = 0; p[0][1] = 0; p[1][0] = sc; p[1][1] = 0; p[2][0] = 0; p[2][1] = 0; break;                                      // vuelta atras
    case 4: p[0][0] = sc; p[0][1] = sc; p[1][0] = sc; p[1][1] = sc; p[2][0] = 3 * sc; p[2][1] = sc; break;                            // |d21| = 0
    case 5: p[0][0] = 0; p[0][1] = 0; p[1][0] = sc; p[1][1] = sc; p[2][0] = sc; p[2][1] = sc; break;                                  // |d32| = 0
    case 10: {   // cerca de los umbrales del producto escalar: cos = 0.4, -0.3
        const float th = std::acos(Rand(n, 41) % 2 ? 0.4f : -0.3f) + (float(i32(Rand(n, 42) % 9) - 4)) * 1e-6f;
        p[0][0] = 0; p[0][1] = 0; p[1][0] = sc; p[1][1] = 0; p[2][0] = sc + sc * std::cos(th); p[2][1] = (Rand(n, 43) % 2 ? 1.0f : -1.0f) * sc * std::sin(th); break; }
    default: for (u32 r = 0; r < 3; ++r) { p[r][0] = F(ordf(50 + 4 * r, 120, 135)); p[r][1] = F(ordf(52 + 4 * r, 120, 135)); } break;
    }
    for (u32 r = 0; r < 3; ++r) { c.pos[r][0] = B(p[r][0]); c.pos[r][1] = B(p[r][1]); c.pos[r][2] = ordf(70 + r, 120, 135); }
    switch (f) {
    case 6: c.pos[Rand(n, 80) % 3][Rand(n, 81) % 2] = NanBits(Rand(n, 82) % 6, Rand(n, 83) % 3 == 0); break;
    case 7: c.pos[Rand(n, 80) % 3][Rand(n, 81) % 2] = InfBits(n, 82); break;
    case 8: for (u32 r = 0; r < 3; ++r) { c.pos[r][0] = B(F(c.pos[r][0]) * std::ldexp(1.0f, -90)); c.pos[r][1] = B(F(c.pos[r][1]) * std::ldexp(1.0f, -90)); } break;   // diferencias minusculas
    case 9: for (u32 r = 0; r < 3; ++r) { c.pos[r][0] = B(F(c.pos[r][0]) * std::ldexp(1.0f, 80)); c.pos[r][1] = B(F(c.pos[r][1]) * std::ldexp(1.0f, 80)); } break;     // cuadrados desbordan en float
    case 11: for (u32 r = 0; r < 3; ++r) { c.pos[r][0] = B(F(c.pos[r][0]) * std::ldexp(1.0f, -60)); c.pos[r][1] = B(F(c.pos[r][1]) * std::ldexp(1.0f, 70)); } break;
    case 13: c.pos[Rand(n, 80) % 3][Rand(n, 81) % 2] = kFloatEdges[Rand(n, 82) % kFloatEdges.size()]; break;
    default: break;
    }
    c.eax = SeedReg(n, 90); c.ecx = SeedReg(n, 93); c.edx = SeedReg(n, 96); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC4(const C4& c) {
    Js j; j.U("family", c.family).U("perturbed", c.perturbed).U("flag_init", c.flagInit).A("area_ids", {c.areaIds[0], c.areaIds[1], c.areaIds[2], c.areaIds[3]})
     .A("ref", {c.ref[0][0], c.ref[0][1], c.ref[1][0], c.ref[1][1], c.ref[2][0], c.ref[2][1]}).A("ref_slot", {c.refSlot[0], c.refSlot[1], c.refSlot[2]})
     .A("pos", {c.pos[0][0], c.pos[0][1], c.pos[0][2], c.pos[1][0], c.pos[1][1], c.pos[1][2], c.pos[2][0], c.pos[2][1], c.pos[2][2]})
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 5: StopCarIfNodesAreInvalid
// Cuatro palabras del vehiculo: +0x3A4 y +0x3A6 (direcciones de enlace: area = palabra >> 10) y +0x390, +0x394 (direcciones de nodo: area = palabra entera)
struct C5 { u32 family{}, w[4]{}, loaded[kPoolAreas]{}, flagByte{}, eax{}, ecx{}, edx{}, stack{}; };   // w: 3A4, 3A6, 390, 394
inline C5 MakeC5(u32 n) {
    C5 c; const u32 f = n % 16; c.family = f;
    const u32 loadedPct = f < 8 ? 85u : 50u;
    for (u32 i = 0; i < kPoolAreas; ++i) c.loaded[i] = Rand(n, 100 + i) % 100 < loadedPct ? 1u : 0u;
    auto linkWord = [&](u32 k) { const u32 sel = Rand(n, k) % 100; return sel < 8 ? 0xFFFFu : ((Rand(n, k + 1) % 64u) << 10) | (Rand(n, k + 2) & 0x3FFu); };
    auto nodeWord = [&](u32 k) -> u32 {
        const u32 sel = Rand(n, k) % 100;
        if (sel < 8) return 0xFFFFu;
        if (sel < 85) return Rand(n, k + 1) % kPoolAreas;
        if (sel < 95) return kPoolAreas + Rand(n, k + 1) % 200u;   // mas alla de la tabla: memoria del juego (m_pNaviNodes y siguientes)
        return Rand(n, k + 1) % 0x1000u;
    };
    c.w[0] = linkWord(10); c.w[1] = linkWord(20); c.w[2] = nodeWord(30); c.w[3] = nodeWord(40);
    if (f % 8 == 7) { c.w[Rand(n, 50) % 4] = 0xFFFFu; }                // fuerza una invalida
    if (f % 8 == 6) { for (u32 i = 0; i < 4; ++i) if (c.w[i] != 0xFFFFu) { c.loaded[(i < 2 ? c.w[i] >> 10 : c.w[i]) % kPoolAreas] = 1; } }   // intenta que todas sean validas
    c.flagByte = Rand(n, 60) % 3 == 0 ? (Rand(n, 61) & 0xFFu) : (Rand(n, 61) % 2);
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC5(const C5& c) {
    Js j; j.U("family", c.family).A("w", {c.w[0], c.w[1], c.w[2], c.w[3]});
    std::vector<u32> l(c.loaded, c.loaded + kPoolAreas);
    j.A("loaded", l).U("flag_byte", c.flagByte).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 6: SwitchBetweenPhysicsAndGhost
struct C6 {
    u32 family{}, flags40{}, creator{}, sub594{}, type590{}, statusByte{}, matMode{}, loadedRet{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline C6 MakeC6(u32 n) {
    C6 c; const u32 f = n % 16; c.family = f;
    c.flags40 = Rand(n, 1) % 6 == 0 ? (Rand(n, 2) & ~0x4000u) : (Rand(n, 2) | 0x4000u);
    c.creator = Rand(n, 3) % 8 == 0 ? Rand(n, 4) & 0xFFu : 2u;
    static constexpr u32 subs[]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0xFFFFFFFFu, 0x100u, 0x80000000u};
    c.sub594 = Rand(n, 5) % 5 == 0 ? subs[Rand(n, 6) % std::size(subs)] : (Rand(n, 6) % 3 == 0 ? 3 + Rand(n, 7) % 3 : 0u);
    static constexpr u32 types[]{0, 0, 0, 9, 9, 1, 2, 3, 4, 5, 8, 10, 11, 0xFFFFFFFFu, 0x100u, 0x80000000u};
    c.type590 = types[Rand(n, 8) % std::size(types)];
    const u32 st = f % 4 < 2 ? 3u : f % 4 == 2 ? 0xCu : Rand(n, 9) % 32;
    c.statusByte = (st << 3) | (Rand(n, 10) & 7u);
    c.matMode = Rand(n, 11) % 2;
    c.loadedRet = BoolRet(n, 20, Rand(n, 19) % 2);
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC6(const C6& c) {
    Js j; j.U("family", c.family).U("flags40", c.flags40).U("creator", c.creator).U("sub594", c.sub594).U("type590", c.type590).U("status_byte", c.statusByte)
     .U("mat_mode", c.matMode).U("loaded_ret", c.loadedRet).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}
}

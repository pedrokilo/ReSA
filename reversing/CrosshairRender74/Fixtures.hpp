#pragma once
// Fixtures del banco HeatFire73 (lote 73): CVehicle::FireHeatSeakingMissile (EXE 0x6E05C0), 16384 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CVehicle::FireHeatSeakingMissile  0x6E05C0  thiscall, ECX = this (vehiculo), (CEntity* objetivo, eOrdnanceType tipo, bool arg2), `ret 0Ch`   (hook CVehicle / FireHeatSeakingMissile, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia).
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace Cr74Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kFamilies = 20;
constexpr unsigned kCases = kFamilies * 1024;    // 20480 pares
constexpr unsigned kFunctions = 1;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kInfoWin = 0x190, kTexWin = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kInfoArena = Arena(kInfoWin), kTexArena = Arena(kTexWin);
constexpr std::size_t kCrossOff = 0x158;   // CPlayerInfo::m_nCrosshairActivated (LiveComparison.hpp lo comprueba con offsetof)

constexpr u32 kTimeAddr = 0xB7CB84;   // CTimer::m_snTimeInMilliseconds
constexpr u32 kWidthAddr = 0xC17044;  // RsGlobal.maximumWidth
constexpr u32 kHeightAddr = 0xC17048; // RsGlobal.maximumHeight
constexpr u32 kTexIdxAddr = 0x8CDF1C; // indice en gpCoronaTexture
constexpr u32 kTexTable = 0xC3E000;   // gpCoronaTexture
constexpr u32 kHistT = 0xB9B8F8, kHistY = 0xB9B920, kHistX = 0xB9B948;   // historiales 2 jugadores x 5 entradas
constexpr unsigned kHist = 10;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 74075u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline u32 B(float f) { return std::bit_cast<u32>(f); }
inline float F(u32 b) { return std::bit_cast<float>(b); }

// ------------------------------------------------------------------------------------------------ JSON minimo
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ valores notables
// NaN: canonicos (quiet con signo +/-), NO canonicos (carga distinta, sNaN con y sin signo, carga maxima, carga minima) e infinitos
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFA00000u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00001u, 0xFFBFFFFFu};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};   // +-0, desnormales y el menor normal
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
inline constexpr std::array<u32, 14> kUnit{0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x40000000u, 0xC0000000u, 0x3F7FFFFFu, 0xBF7FFFFFu, 0x3F800001u, 0xBF800001u, 0x3DCCCCCDu, 0x3F99999Au, 0x3FA66666u, 0x41200000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
// Float con exponente insesgado e (-126..127) y mantisa/signo aleatorios; Denorm: desnormal con signo y mantisa aleatorios (nunca cero)
inline u32 ExpFloat(u32 c, u32 k, int e) { const u32 r = Rand(c, k); return ((r >> 31) << 31) | (u32(e + 127) << 23) | (Rand(c, k + 500) & 0x7FFFFFu); }
inline u32 Denorm(u32 c, u32 k) { const u32 r = Rand(c, k); return ((r >> 31) << 31) | ((Rand(c, k + 500) & 0x7FFFFFu) | 1u); }
inline u32 Sgn(u32 bits, u32 neg) { return (bits & 0x7FFFFFFFu) | (neg << 31); }
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Un float: modo 0 moderado, 1 pequeno (productos que caen en el rango desnormal), 2 grande (desbordamiento), 3 especial (cero, desnormal, inf, NaN), 4 unitario, 5 mezcla de todo
inline u32 FloatOf(u32 c, u32 k, u32 mode) {
    const u32 r = Rand(c, k) % 32;
    switch (mode) {
    case 0: return r == 0 ? B(0.0f) : RandFloat(c, k + 2, -4, 11);
    case 1: return r < 3 ? Pick(kTiny, c, k + 1) : RandFloat(c, k + 2, -80, -20);
    case 2: return r < 3 ? Pick(kBig, c, k + 1) : RandFloat(c, k + 2, 20, 120);
    case 3:
        if (r < 8) return Pick(kTiny, c, k + 1);
        if (r < 13) return Pick(kInf, c, k + 1);
        if (r < 21) return Pick(kNan, c, k + 1);
        if (r < 24) return Pick(kBig, c, k + 1);
        return RandFloat(c, k + 2, -20, 100);
    case 4: return r < 4 ? RandFloat(c, k + 2, -2, 2) : Pick(kUnit, c, k + 1);
    default: {
        const u32 m = Rand(c, k + 3) % 5;
        return FloatOf(c, k + 5, m == 4 ? 3u : m);
    }
    }
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 player{};        // indice de jugador (0 o 1)
    u32 active{};        // DWORD m_nCrosshairActivated (el EXE solo mira el byte bajo)
    u32 tx{}, ty{};      // m_vecCrosshairTarget (+4, +8)
    u32 time{};          // CTimer::m_snTimeInMilliseconds
    u32 width{}, height{};   // RsGlobal.maximumWidth / maximumHeight (int32)
    u32 texIdx{};        // indice en gpCoronaTexture (0xC3E000)
    u32 raster{};        // primer campo de la textura (el raster)
    u32 hx[kHist]{}, hy[kHist]{}, ht[kHist]{};   // historiales previos (X, Y y hora), dos jugadores x cinco entradas
};

inline u32 ScreenOf(u32 n, u32 k) { return RandFloat(n, k, -8, 1); }   // posicion de la mira en pantalla (-1..1 aprox.)
inline u32 SizeOf(u32 n, u32 k) { return Rand(n, k) % 5 == 0 ? Rand(n, k + 1) : 100u + Rand(n, k + 2) % 3000u; }

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n % kFamilies;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.player = Rand(n, 3) % 2;
    c.active = (Rand(n, 4) & 0xFFFFFF00u) | (Rand(n, 5) % 3 == 0 ? (1u + Rand(n, 6) % 255u) : 1u);
    c.tx = ScreenOf(n, 10); c.ty = ScreenOf(n, 11);
    c.time = Rand(n, 12) % 3 == 0 ? Rand(n, 13) % 1024u : Rand(n, 14);
    c.width = Rand(n, 15) % 4 == 0 ? 640u : SizeOf(n, 16);
    c.height = Rand(n, 17) % 4 == 0 ? 480u : SizeOf(n, 18);
    c.texIdx = Rand(n, 19) % 32;
    c.raster = Rand(n, 20);
    for (unsigned i = 0; i < kHist; ++i) { c.hx[i] = ScreenOf(n, 30 + i); c.hy[i] = ScreenOf(n, 50 + i); c.ht[i] = Rand(n, 70 + i); }
    const auto specialHist = [&](u32 mode) { for (unsigned i = 0; i < kHist; ++i) { c.hx[i] = FloatOf(n, 100 + i, mode); c.hy[i] = FloatOf(n, 120 + i, mode); } };
    switch (c.family) {
    case 0:  // inactiva: el byte bajo es cero (basura arriba); no se dibuja ni se toca nada
        c.active = Rand(n, 90) & 0xFFFFFF00u; if (Rand(n, 91) % 4 == 0) c.active = 0;
        break;
    case 1:  // pantalla normal
        c.width = 640u + Rand(n, 90) % 1400u; c.height = 480u + Rand(n, 91) % 700u;
        break;
    case 2:  // los dos jugadores con todo aleatorio
        break;
    case 3:  // historial con valores especiales (NaN, infinitos, ceros con signo, desnormales, grandes)
        specialHist(Rand(n, 90) % 2 ? 3u : 5u);
        break;
    case 4:  // la mira actual con valores especiales
        c.tx = FloatOf(n, 90, 3u); c.ty = FloatOf(n, 91, 3u); if (Rand(n, 92) % 3 == 0) specialHist(0u);
        break;
    case 5:  // anchura/altura especiales (0, negativas, extremos)
        {
            static constexpr std::array<u32, 10> sz{0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x10000u, 0xFFFF0000u, 0x00FFFFFFu, 0x3E8u, 0x80000001u};
            c.width = Rand(n, 90) % 2 ? Pick(sz, n, 91) : Rand(n, 92); c.height = Rand(n, 93) % 2 ? Pick(sz, n, 94) : Rand(n, 95);
        }
        break;
    case 6:  // la hora: todos los residuos de 10 bits, alta basura y valores con el bit 31
        c.time = Rand(n, 90) % 2 ? ((Rand(n, 91) & 0xFFFFFC00u) | (n / kFamilies) % 1024u) : (Rand(n, 92) % 2 ? 0x80000000u + Rand(n, 93) % 1024u : 0xFFFFFFFFu - Rand(n, 94) % 1024u);
        for (unsigned i = 0; i < kHist; ++i) c.ht[i] = Rand(n, 95) % 2 ? c.time : c.ht[i];
        break;
    case 7:  // indices de textura 0..31 con rasters distintos
        c.texIdx = (n / kFamilies) % 32; c.raster = Rand(n, 90) % 4 == 0 ? 0u : Rand(n, 91);
        break;
    case 8:  // magnitudes grandes
        specialHist(2u); c.tx = FloatOf(n, 90, 2u); c.ty = FloatOf(n, 91, 2u);
        break;
    case 9:  // magnitudes diminutas
        specialHist(1u); c.tx = FloatOf(n, 90, 1u); c.ty = FloatOf(n, 91, 1u);
        break;
    case 10:  // X o Y = -1 (suma exacta a cero), valores unitarios y ceros con signo
        for (unsigned i = 0; i < kHist; ++i) {
            c.hx[i] = Rand(n, 90 + i) % 3 == 0 ? Pick(kUnit, n, 100 + i) : Rand(n, 110 + i) % 2 ? 0xBF800000u : (Rand(n, 120 + i) % 2 ? 0u : 0x80000000u);
            c.hy[i] = Rand(n, 130 + i) % 3 == 0 ? Pick(kUnit, n, 140 + i) : Rand(n, 150 + i) % 2 ? 0xBF800000u : (Rand(n, 160 + i) % 2 ? 0u : 0x80000000u);
        }
        c.tx = Rand(n, 170) % 2 ? 0xBF800000u : 0x80000000u; c.ty = Rand(n, 171) % 2 ? 0xBF800000u : 0u;
        c.width = Rand(n, 172) % 3 == 0 ? 0u : c.width; c.height = Rand(n, 173) % 3 == 0 ? 0u : c.height;
        break;
    case 11:  // mezcla de especiales
        specialHist(5u); c.tx = FloatOf(n, 90, 5u); c.ty = FloatOf(n, 91, 5u);
        break;
    case 12:  // jugador 1
        c.player = 1;
        break;
    case 13:  // jugador 0
        c.player = 0;
        break;
    case 14:  // palabras crudas arbitrarias en todos los historiales (el desplazamiento copia bits)
        for (unsigned i = 0; i < kHist; ++i) { c.hx[i] = Rand(n, 90 + i); c.hy[i] = Rand(n, 100 + i); c.ht[i] = Rand(n, 110 + i); }
        c.tx = Rand(n, 120); c.ty = Rand(n, 121);
        break;
    case 15:  // dimensiones negativas / cero y hora grande
        c.width = Rand(n, 90) | 0x80000000u; c.height = Rand(n, 91) | 0x80000000u;
        break;
    case 16:  // posiciones fuera de pantalla moderadas (|X|, |Y| hasta 1e4)
        for (unsigned i = 0; i < kHist; ++i) { c.hx[i] = RandFloat(n, 90 + i, 0, 13); c.hy[i] = RandFloat(n, 110 + i, 0, 13); }
        break;
    case 17:  // indices de textura limite y raster nulo
        c.texIdx = Rand(n, 90) % 2 ? 31u : (Rand(n, 91) % 4); c.raster = Rand(n, 92) % 3 == 0 ? 0xFFFFFFFFu : Rand(n, 93);
        break;
    case 18:  // jugador 1 con el historial del jugador 0 distinto (la hora se copia de una direccion absoluta del jugador 0)
        c.player = 1;
        for (unsigned i = 0; i < 5; ++i) { c.hy[i] = Rand(n, 90 + i) | 1u; c.ht[i] = Rand(n, 100 + i); }
        break;
    default:  // 19: jugador 0 con la hora y la Y del jugador 0 especiales (la hora se copia de Y)
        c.player = 0;
        for (unsigned i = 0; i < 5; ++i) { c.hy[i] = FloatOf(n, 90 + i, 5u); c.ht[i] = Rand(n, 100 + i); }
        break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("player", c.player).U("active", c.active).U("tx", c.tx).U("ty", c.ty).U("time", c.time).U("width", c.width).U("height", c.height).U("tex_idx", c.texIdx).U("raster", c.raster);
    static const char* const hx[kHist]{"hx0", "hx1", "hx2", "hx3", "hx4", "hx5", "hx6", "hx7", "hx8", "hx9"};
    static const char* const hy[kHist]{"hy0", "hy1", "hy2", "hy3", "hy4", "hy5", "hy6", "hy7", "hy8", "hy9"};
    static const char* const ht[kHist]{"ht0", "ht1", "ht2", "ht3", "ht4", "ht5", "ht6", "ht7", "ht8", "ht9"};
    for (unsigned i = 0; i < kHist; ++i) j.U(hx[i], c.hx[i]).U(hy[i], c.hy[i]).U(ht[i], c.ht[i]);
    return j.End();
}
}

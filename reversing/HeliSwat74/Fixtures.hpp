#pragma once
// Fixtures del banco HeliSwat74 (lote 74): CHeli::SendDownSwat (EXE 0x6C69C0), 21504 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CHeli::SendDownSwat  0x6C69C0  thiscall, ECX = this (helicoptero), sin argumentos, resultado en AL   (hook CHeli / SendDownSwat, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) para n < 16384 y 1024 casos por cada familia 16..20 despues.
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
#include <utility>
#include <vector>

namespace Hs74Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;    // familias 0..15
constexpr unsigned kExtraFamilies = 5;    // familias 16..20
constexpr unsigned kCases = kBaseCases + kExtraFamilies * 1024;
constexpr unsigned kFamilies = 16 + kExtraFamilies;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto). La del helicoptero lleva una guarda de 0x200 bytes: el EXE escribe m_aSwatState[ocupantes - 1] con el indice sin signo (hasta 0xFE).
constexpr std::size_t kGuard = 0x40, kHeliGuard = 0x200;
constexpr std::size_t kHeliWin = 0xA18, kTgtWin = 0x60, kMatWin = 0x48, kPedWin = 0x500, kIntelWin = 0x40, kClumpWin = 0x10, kSeqWin = 0x40, kRopeWin = 0x1C, kWanderWin = 0x50;
constexpr std::size_t kArena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 kStreamAddr = 0x8E6314;   // byte de estado de carga del modelo SWAT (CStreaming::ms_aInfoForModel[285].m_LoadState)

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 74074u); }
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
    u32 numSwat{};       // byte +0x9B9 (rango completo 0..255)
    u32 phys{};          // dword +0x40 (bit 8 = bSubmergedInWater)
    u32 stream{};        // byte 0x8E6314 (estado de carga del modelo SWAT 285; solo 1 deja pasar)
    u32 rnd{};           // retorno de rand() (0x821B1E): solo se mira (rnd & 0x7F)
    u32 tgtMat{};        // el objetivo tiene matriz (posicion en +0x30) o no (posicion compacta en +4)
    u32 tgt[3]{}, hel[3]{};   // posiciones del objetivo y del helicoptero (matriz +0x30)
    u32 mat[9]{};        // matriz del helicoptero: right (0..2), forward (3..5), up (6..8)
    u32 spd[3]{}, rel[3]{};   // velocidad (+0x44) y posicion relativa de la cuerda (retorno de FindSwatPositionRelativeToHeli)
    u32 ground{};        // retorno de CWorld::FindGroundZFor3DCoord
    u32 water{};         // GetWaterLevelNoWaves devuelve verdadero
    u32 waterLvl{};      // nivel de agua escrito si hay agua
    u32 ropeRet{};       // retorno (0/1) de RegisterRope
    u32 nullSeq{}, nullRope{}, nullWander{};   // la reserva de la tarea devuelve nulo
    u32 range{};         // retorno de GetRandomNumberInRange(0, 8) (0..7 como el real)
    u32 pedFlags{};      // dword +0x1C del ped
    u32 gate{};          // informativo: 1 = el caso esta pensado para llegar al final
};

inline float Unit(u32 c, u32 k) { return float(int(Rand(c, k) % 2001u) - 1000) / 1000.0f; }   // [-1, 1]

// Caso base que pasa todas las compuertas: cerca del objetivo, casi parado, suelo cercano, sin agua o con el agua por debajo del suelo
inline void Baseline(C& c, unsigned n) {
    c.numSwat = 1 + Rand(n, 10) % 4;
    c.phys = Rand(n, 11) & ~0x100u;
    c.stream = 1;
    c.rnd = Rand(n, 12) % 5 == 0 ? (Rand(n, 13) & 0xFFFFFF80u) : (Rand(n, 13) % 256u) * 128u;
    c.tgtMat = Rand(n, 14) % 2;
    for (int i = 0; i < 3; ++i) c.tgt[i] = FloatOf(n, 20 + i, 0u);
    for (int i = 0; i < 3; ++i) c.hel[i] = B(F(c.tgt[i]) + Unit(n, 30 + i) * 20.0f);
    for (int i = 0; i < 3; ++i) c.spd[i] = B(Unit(n, 40 + i) * 0.05f);
    for (int i = 0; i < 9; ++i) c.mat[i] = FloatOf(n, 50 + i, 4u);
    static constexpr float table[4][3]{{-1.2f, -1.0f, -0.5f}, {1.2f, -1.0f, -0.5f}, {-1.2f, 1.0f, -0.5f}, {1.2f, 1.0f, -0.5f}};
    const u32 pick = Rand(n, 60) % 7;
    for (int i = 0; i < 3; ++i) c.rel[i] = pick < 4 ? B(table[pick][i]) : pick == 4 ? 0u : FloatOf(n, 61 + i, 0u);
    c.ground = B(F(c.tgt[2]) + Unit(n, 70) * 2.4f);
    c.water = Rand(n, 71) % 3 == 0;
    c.waterLvl = c.water ? B(F(c.ground) - 0.1f - float(Rand(n, 72) % 500u) / 10.0f) : Rand(n, 73);
    c.ropeRet = Rand(n, 74) % 2;
    c.nullSeq = Rand(n, 75) % 32 == 0; c.nullRope = Rand(n, 76) % 32 == 0; c.nullWander = Rand(n, 77) % 32 == 0;
    c.range = Rand(n, 78) % 8;
    c.pedFlags = Rand(n, 79);
    c.gate = 1;
}

inline u32 Nudge(u32 bits, int ulps) { return u32(i32(bits) + ulps); }   // ulps hacia mayor magnitud (para positivos)

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    Baseline(c, n);
    switch (c.family) {
    case 0:   // el caso base
        break;
    case 1: {  // ocupantes SWAT: 0 corta; el indice del estado (byte sin signo del valor nuevo) y la cuerda dependen de el
        static constexpr u32 v[]{0u, 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 0x7Fu, 0x80u, 0x81u, 0xFFu, 0xFEu, 0x40u};
        c.numSwat = Rand(n, 100) % 4 == 0 ? Rand(n, 101) & 0xFFu : v[Rand(n, 102) % std::size(v)];
        break;
    }
    case 2:   // bit 8 de +0x40 (sumergido) y el resto de bits
        c.phys = Rand(n, 100) % 3 == 0 ? 0xFFFFFFFFu : Rand(n, 101);
        if (Rand(n, 102) % 4 == 0) c.phys &= ~0x100u;
        if (Rand(n, 103) % 5 == 0) c.phys = 0x100u;
        break;
    case 3: {  // estado de carga del modelo SWAT
        static constexpr u32 v[]{0u, 1u, 1u, 2u, 3u, 4u, 0xFFu, 0x80u, 0x7Fu, 0x81u, 0x101u};
        c.stream = Rand(n, 100) % 4 == 0 ? Rand(n, 101) & 0xFFu : v[Rand(n, 102) % std::size(v)];
        break;
    }
    case 4: {  // rand(): solo cuentan los 7 bits bajos
        static constexpr u32 v[]{0u, 128u, 256u, 1u, 0x7Fu, 0x80u, 0xFFu, 0x7FFFu, 0x8000u, 0xFFFFFF80u, 0xFFFFFF7Fu, 0x7FFFFFFFu, 0x80000000u, 0xDEADBF80u, 0x7F80u, 0x7F00u, 0x100u};
        c.rnd = Rand(n, 100) % 4 == 0 ? Rand(n, 101) : v[Rand(n, 102) % std::size(v)];
        break;
    }
    case 5: {  // distancia al objetivo alrededor de 50
        const u32 t = Rand(n, 100) % 6;
        float o[3];
        if (t == 0) {            // 30-40-50 permutado y con signo
            static constexpr int p[6][3]{{0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};
            const float v3[3]{30.0f, 40.0f, 0.0f};
            const auto& q = p[Rand(n, 101) % 6];
            for (int i = 0; i < 3; ++i) o[i] = v3[q[i]] * (Rand(n, 110 + i) % 2 ? 1.0f : -1.0f);
        } else if (t == 1 || t == 2) {   // direccion aleatoria con norma 50 +- k ulp
            float d[3];
            for (int i = 0; i < 3; ++i) d[i] = Unit(n, 120 + i);
            float m = std::sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
            if (m == 0.0f) { d[0] = 1.0f; m = 1.0f; }
            const float norm = F(Nudge(B(50.0f), int(Rand(n, 123) % 7u) - 3));
            for (int i = 0; i < 3; ++i) o[i] = d[i] / m * norm;
        } else {
            for (int i = 0; i < 3; ++i) o[i] = Unit(n, 130 + i) * 60.0f;
        }
        for (int i = 0; i < 3; ++i) c.hel[i] = B(F(c.tgt[i]) + o[i]);
        if (t == 5) {            // especiales en una posicion
            const int i = int(Rand(n, 140) % 3);
            (Rand(n, 141) % 2 ? c.hel : c.tgt)[i] = FloatOf(n, 142, 3u);
        }
        break;
    }
    case 6: {  // velocidad alrededor de 0.1
        const u32 t = Rand(n, 100) % 6;
        if (t == 0) { c.spd[0] = B(0.1f); c.spd[1] = c.spd[2] = 0u; if (Rand(n, 101) % 2) std::swap(c.spd[0], c.spd[Rand(n, 102) % 3]); }
        else if (t == 1) { c.spd[0] = B(0.06f); c.spd[1] = B(0.08f); c.spd[2] = 0u; std::swap(c.spd[Rand(n, 103) % 3], c.spd[2]); }
        else if (t == 2 || t == 3) {
            float d[3];
            for (int i = 0; i < 3; ++i) d[i] = Unit(n, 120 + i);
            float m = std::sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
            if (m == 0.0f) { d[0] = 1.0f; m = 1.0f; }
            const float norm = F(Nudge(B(0.1f), int(Rand(n, 123) % 9u) - 4));
            for (int i = 0; i < 3; ++i) c.spd[i] = B(d[i] / m * norm);
        } else if (t == 4) {
            for (int i = 0; i < 3; ++i) c.spd[i] = FloatOf(n, 130 + i, Rand(n, 133 + i) % 2 ? 3u : 1u);
        } else {
            for (int i = 0; i < 3; ++i) c.spd[i] = B(Unit(n, 140 + i) * 0.2f);
        }
        break;
    }
    case 7: {  // suelo: |objetivo.z - suelo| frente a 2.5 (estrictamente menor)
        const float tz = F(c.tgt[2]);
        const u32 t = Rand(n, 100) % 8;
        float delta = 0.0f;
        switch (t) {
        case 0: delta = 2.5f; break;
        case 1: delta = -2.5f; break;
        case 2: delta = F(Nudge(B(2.5f), -1)); break;
        case 3: delta = -F(Nudge(B(2.5f), -1)); break;
        case 4: delta = F(Nudge(B(2.5f), 1)); break;
        case 5: delta = Unit(n, 101) * 6.0f; break;
        case 6: delta = 0.0f; break;
        default: delta = -F(Nudge(B(2.5f), 1)); break;
        }
        c.ground = B(tz + delta);
        if (Rand(n, 110) % 6 == 0) c.ground = FloatOf(n, 111, 3u);
        if (Rand(n, 112) % 10 == 0) c.tgt[2] = FloatOf(n, 113, 3u);
        break;
    }
    case 8: {  // agua: hay agua y su nivel frente al suelo (estrictamente menor)
        c.water = 1;
        const float g = F(c.ground);
        const u32 t = Rand(n, 100) % 9;
        switch (t) {
        case 0: c.waterLvl = c.ground; break;
        case 1: c.waterLvl = Nudge(c.ground, 1); break;
        case 2: c.waterLvl = Nudge(c.ground, -1); break;
        case 3: c.waterLvl = B(g + 1.0f); break;
        case 4: c.waterLvl = B(g - 1.0f); break;
        case 5: c.waterLvl = FloatOf(n, 101, 3u); break;
        case 6: c.waterLvl = Rand(n, 102) % 2 ? 0u : 0x80000000u; break;
        case 7: c.waterLvl = B(g - 100.0f); break;
        default: c.waterLvl = Rand(n, 103); break;
        }
        if (Rand(n, 110) % 5 == 0) c.ground = Rand(n, 111) % 2 ? 0u : 0x80000000u, c.tgt[2] = c.ground ^ (Rand(n, 112) % 2 ? 0u : 0x80000000u);
        break;
    }
    case 9: {  // especiales en matriz, posicion relativa y posicion del helicoptero (las compuertas siguen pasando)
        for (int i = 0; i < 9; ++i) if (Rand(n, 100 + i) % 3 == 0) c.mat[i] = FloatOf(n, 110 + i, 3u);
        for (int i = 0; i < 3; ++i) if (Rand(n, 120 + i) % 3 == 0) c.rel[i] = FloatOf(n, 123 + i, 3u);
        for (int i = 0; i < 3; ++i) if (Rand(n, 130 + i) % 4 == 0) c.hel[i] = Rand(n, 133 + i) % 2 ? Pick(kTiny, n, 136 + i) : Pick(kNan, n, 139 + i);
        break;
    }
    case 10:   // cualquier cantidad de ocupantes 1..255 con todo lo demas valido
        c.numSwat = 1 + Rand(n, 100) % 255;
        break;
    case 11:   // reservas de tareas agotadas
        c.nullSeq = Rand(n, 100) % 2; c.nullRope = Rand(n, 101) % 2; c.nullWander = Rand(n, 102) % 2;
        break;
    case 12:   // posicion relativa de la tabla real segun el ocupante, retorno de RegisterRope y del azar
        c.numSwat = 1 + Rand(n, 100) % 6;
        c.ropeRet = Rand(n, 101) % 2;
        c.range = Rand(n, 102) % 9;
        c.tgtMat = Rand(n, 103) % 2;
        break;
    case 13: {  // mezcla de rangos en todo (con compuertas aleatorias)
        for (int i = 0; i < 3; ++i) { c.tgt[i] = FloatOf(n, 100 + i, 5u); c.hel[i] = Rand(n, 103 + i) % 2 ? c.tgt[i] : FloatOf(n, 106 + i, 5u); c.spd[i] = FloatOf(n, 109 + i, 5u); c.rel[i] = FloatOf(n, 112 + i, 5u); }
        for (int i = 0; i < 9; ++i) c.mat[i] = FloatOf(n, 115 + i, 5u);
        c.ground = FloatOf(n, 125, 5u); c.water = Rand(n, 126) % 2; c.waterLvl = FloatOf(n, 127, 5u);
        if (Rand(n, 128) % 3 == 0) c.rnd = Rand(n, 129);
        break;
    }
    case 14:   // basura en todas las compuertas
        c.numSwat = Rand(n, 100) % 3 == 0 ? Rand(n, 101) & 0xFFu : c.numSwat;
        c.phys = Rand(n, 102); c.stream = Rand(n, 103) % 3 == 0 ? Rand(n, 104) & 0xFFu : 1u; c.rnd = Rand(n, 105) % 2 ? Rand(n, 106) : c.rnd;
        for (int i = 0; i < 3; ++i) { c.tgt[i] = Rand(n, 107 + i); c.hel[i] = Rand(n, 110 + i); c.spd[i] = Rand(n, 113 + i); }
        break;
    case 15:   // llega al final con basura en todo el resto (objetivo sin matriz, indicadores del ped aleatorios)
        c.tgtMat = 0; c.pedFlags = Rand(n, 100) | (Rand(n, 101) % 2); c.range = Rand(n, 102) % 8;
        break;
    case 16: {  // magnitudes pequenas: los productos de la rotacion caen en el rango desnormal / de exponente extendido
        for (int i = 0; i < 9; ++i) c.mat[i] = FloatOf(n, 100 + i, Rand(n, 109 + i) % 4 == 0 ? 3u : 1u);
        for (int i = 0; i < 3; ++i) { c.rel[i] = FloatOf(n, 112 + i, Rand(n, 115 + i) % 4 == 0 ? 3u : 1u); if (Rand(n, 118 + i) % 3 == 0) c.hel[i] = B(F(c.tgt[i]) + Unit(n, 121 + i) * 1e-30f); }
        if (Rand(n, 130) % 3 == 0) for (int i = 0; i < 3; ++i) { c.tgt[i] = FloatOf(n, 131 + i, 1u); c.hel[i] = FloatOf(n, 134 + i, 1u); }
        c.ground = B(F(c.tgt[2]) + Unit(n, 137) * 1e-3f);
        break;
    }
    case 17: {  // cancelaciones y ceros con signo: matriz identidad con ceros +-0, posicion relativa = -posicion del helicoptero
        static constexpr u32 id[9]{0x3F800000u, 0u, 0u, 0u, 0x3F800000u, 0u, 0u, 0u, 0x3F800000u};
        for (int i = 0; i < 9; ++i) c.mat[i] = id[i] | (id[i] == 0 && Rand(n, 100 + i) % 2 ? 0x80000000u : 0u);
        if (Rand(n, 110) % 3 == 0) c.mat[Rand(n, 111) % 9] = Rand(n, 112) % 2 ? 0u : 0x80000000u;
        for (int i = 0; i < 3; ++i) {
            c.rel[i] = Rand(n, 113 + i) % 5 == 0 ? (Rand(n, 116 + i) % 2 ? 0u : 0x80000000u) : FloatOf(n, 119 + i, 4u);
            c.hel[i] = Rand(n, 122 + i) % 4 == 0 ? (Rand(n, 125 + i) % 2 ? 0u : 0x80000000u) : (c.rel[i] ^ 0x80000000u);   // V = rel + hel -> 0 o -0
        }
        for (int i = 0; i < 3; ++i) c.tgt[i] = B(F(c.hel[i]) + Unit(n, 128 + i));
        c.ground = B(F(c.tgt[2]) + Unit(n, 131) * 2.0f);
        break;
    }
    case 18: {  // desbordamiento: matriz y posicion relativa enormes
        for (int i = 0; i < 9; ++i) c.mat[i] = FloatOf(n, 100 + i, Rand(n, 109 + i) % 3 == 0 ? 3u : 2u);
        for (int i = 0; i < 3; ++i) { c.rel[i] = FloatOf(n, 112 + i, 2u); c.hel[i] = FloatOf(n, 115 + i, 2u); c.tgt[i] = B(F(c.hel[i]) + Unit(n, 118 + i) * 4.0f); }
        c.ground = B(F(c.tgt[2]) + Unit(n, 121));
        break;
    }
    case 19: {  // NaN no canonicos (con carga distinta, de senal) y combinaciones de dos NaN en la rotacion y la suma
        for (int i = 0; i < 9; ++i) if (Rand(n, 100 + i) % 2 == 0) c.mat[i] = Pick(kNan, n, 110 + i);
        for (int i = 0; i < 3; ++i) { if (Rand(n, 120 + i) % 4 == 0) c.rel[i] = Pick(kNan, n, 123 + i); if (Rand(n, 126 + i) % 2 == 0) c.hel[i] = Pick(kNan, n, 129 + i); }
        if (Rand(n, 135) % 3 == 0) for (int i = 0; i < 3; ++i) c.spd[i] = Pick(kNan, n, 136 + i);
        break;
    }
    default: {  // 20: valores exactos de las fronteras (50, 0.1 y 2.5) con todo lo demas valido
        const u32 t = Rand(n, 100) % 3;
        if (t == 0) { const float k = float(1 + Rand(n, 101) % 4); c.hel[0] = B(F(c.tgt[0]) + 30.0f * k / k); c.hel[1] = B(F(c.tgt[1]) + 40.0f); c.hel[2] = c.tgt[2]; }
        else if (t == 1) { c.spd[0] = B(0.1f); c.spd[1] = c.spd[2] = Rand(n, 102) % 2 ? 0u : 0x80000000u; }
        else { c.ground = B(F(c.tgt[2]) + 2.5f); if (Rand(n, 103) % 2) c.ground = B(F(c.tgt[2]) - 2.5f); }
        break;
    }
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("num_swat", c.numSwat).U("phys", c.phys).U("stream", c.stream).U("rnd", c.rnd).U("tgt_mat", c.tgtMat).U("ground", c.ground).U("water", c.water).U("water_lvl", c.waterLvl);
    j.U("rope_ret", c.ropeRet).U("null_seq", c.nullSeq).U("null_rope", c.nullRope).U("null_wander", c.nullWander).U("range", c.range).U("ped_flags", c.pedFlags).U("gate", c.gate);
    j.U("tgt_x", c.tgt[0]).U("tgt_y", c.tgt[1]).U("tgt_z", c.tgt[2]).U("hel_x", c.hel[0]).U("hel_y", c.hel[1]).U("hel_z", c.hel[2]);
    j.U("spd_x", c.spd[0]).U("spd_y", c.spd[1]).U("spd_z", c.spd[2]).U("rel_x", c.rel[0]).U("rel_y", c.rel[1]).U("rel_z", c.rel[2]);
    static constexpr const char* mn[9]{"m_rx", "m_ry", "m_rz", "m_fx", "m_fy", "m_fz", "m_ux", "m_uy", "m_uz"};
    for (int i = 0; i < 9; ++i) j.U(mn[i], c.mat[i]);
    return j.End();
}
}

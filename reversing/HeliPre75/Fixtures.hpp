#pragma once
// Fixtures del banco HeliPre75 (lote 75): CHeli::PreRender (EXE 0x6C5420, virtual, ranura 17 de la vtable 0x871680), 21504 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CHeli::PreRender  0x6C5420  thiscall virtual, ECX = this (helicoptero), sin argumentos, void   (hook CHeli / PreRender, tipo V)
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

namespace Hp75Fix {
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

// Arenas: guarda + ventana + guarda (la ventana es el objeto). El helicoptero es un CHeli completo (LiveComparison.hpp lo comprueba con static_assert).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kHeliWin = 0xA18, kMatWin = 0x48, kFrameWin = 0x60, kHandWin = 0x100;
constexpr std::size_t kArena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 kStepAddr = 0xB7CB5C;      // CTimer::ms_fTimeStep
constexpr u32 kDustAddr = 0x8D33A0;      // multiplicador de giro del rotor (1.66f)
constexpr u32 kModelTable = 0xA9B0C8;    // CModelInfo::ms_modelInfoPtrs

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 75277u); }
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
inline u32 Denorm(u32 c, u32 k) { const u32 r = Rand(c, k); return ((r >> 31) << 31) | ((Rand(c, k + 500) & 0x7FFFFFu) | 1u); }
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
inline float Unit(u32 c, u32 k) { return float(int(Rand(c, k) % 2001u) - 1000) / 1000.0f; }   // [-1, 1]
// Un float cercano a un umbral (el umbral y sus vecinos de 1 y 2 ulp, con y sin signo) o un especial
inline u32 Near(u32 c, u32 k, float threshold) {
    const u32 b = B(threshold);
    switch (Rand(c, k) % 9) {
    case 0: return b;
    case 1: return b + 1;
    case 2: return b - 1;
    case 3: return b + 2;
    case 4: return b - 2;
    case 5: return b | 0x80000000u;
    case 6: return (b + 1) | 0x80000000u;
    case 7: return Pick(kTiny, c, k + 1);
    default: return B(threshold * (0.5f + float(Rand(c, k + 2) % 1000u) / 500.0f));
    }
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 model{};         // word +0x22 (indice de ModelInfo con signo: el EXE lo extiende con movsx)
    u32 b42b{};          // byte +0x42B (bit 0 = vehiculo con colision procesada)
    u32 flags40{};       // dword +0x40 (bit 26 = 0x4000000)
    u32 flags38c{};      // dword +0x38C (bit 17 = 0x20000)
    u32 handB8{};        // float en +0xB8 del manejo
    u32 a11{};           // byte +0xA11 (foco de busqueda)
    u32 intens{};        // float +0xA00
    u32 tgt[3]{};        // floats +0x9F4, +0x9F8, +0x9FC
    u32 timeRet{};       // EAX que devuelve CClock::GetIsTimeInRange (solo AL cuenta)
    u32 origin[3]{};     // resultado de la multiplicacion matriz * vector
    u32 comp[4]{}, spr[4]{}, line[4]{}, wp[4]{};   // +0x7D4, +0x878, +0x888, +0x838
    u32 wheel[4][3]{};   // lo que escribe GetWheelPosn por rueda
    u32 rotor{}, a0{}, a1{}, step{};   // +0x84C, +0x9A0, +0x9A4 y CTimer::ms_fTimeStep
    u32 nodePresent[4]{};  // m_aCarNodes[12..15]
    u32 frame[4][16]{};    // matriz de modelado de cada nodo
    u32 matPresent{};      // m_matrix distinto de nulo
    u32 gate{};
};

// Modelos validos como indice de ModelInfo (el EXE lee ms_modelInfoPtrs[(int16)modelo]): los de los helicopteros y vecinos, y rangos inocuos
inline u32 ModelOf(u32 n, u32 k) {
    static constexpr u32 list[]{0x1D1u, 0x1F5u, 0x1D5u, 0x1BFu, 0x1A9u, 0x1A1u, 0x1E7u, 0x1E8u, 0x1F1u, 0x1D2u, 0x1F4u, 0x1D0u, 0x1BEu, 0x1A0u, 0x1A2u, 0x1E6u, 0x1E9u, 0x1F0u, 0x1F2u, 0u, 1u, 0xFFFFu, 0x4E1Fu};
    return Rand(n, k) % 4 == 0 ? Rand(n, k + 1) % 0x4E20u : list[Rand(n, k + 2) % std::size(list)];
}

// Evita los bucles de envolvente que no terminan (a0 muy negativo, a1 muy positivo) sin tocar los NaN: el EXE y la DLL se quedarian ambos en el bucle
inline void SafeAngles(C& c) {
    const float step = F(c.step), rotor = F(c.rotor);
    const bool heavy = (c.model & 0xFFFF) == 0x1D5 || (c.model & 0xFFFF) == 0x1BF || (c.model & 0xFFFF) == 0x1E7 || (c.model & 0xFFFF) == 0x1E8 || (c.model & 0xFFFF) == 0x1F1;
    const double t0 = double(step) * double(rotor) * (heavy ? 1.66 : 1.0), t1 = double(step) * double(rotor) * 2.3;
    if (!std::isnan(t0) && std::fabs(t0) > 500.0) c.step = B(1.0f);
    if (!std::isnan(t1) && std::fabs(t1) > 500.0) c.step = B(1.0f);
    const double s2 = double(F(c.step)) * double(F(c.rotor));
    if (!std::isnan(s2) && std::fabs(s2) > 200.0) { c.step = B(1.0f); c.rotor = B(0.1f); }
    const float a0 = F(c.a0), a1 = F(c.a1);
    if (!std::isnan(a0) && a0 < -500.0f) c.a0 = B(-3.0f);
    if (!std::isnan(a1) && a1 > 500.0f) c.a1 = B(3.0f);
}

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.model = ModelOf(n, 10);
    c.b42b = Rand(n, 13) & 0xFFu; if (Rand(n, 14) % 4 != 0) c.b42b |= 1u;
    c.flags40 = Rand(n, 15); c.flags38c = Rand(n, 16);
    c.handB8 = Rand(n, 17) % 5 == 0 ? FloatOf(n, 18, 3u) : B(Unit(n, 19) * 0.5f);
    c.a11 = Rand(n, 20) % 4 == 0 ? Rand(n, 21) & 0xFFu : 1u;
    c.intens = Rand(n, 22) % 5 == 0 ? Near(n, 23, 0.0f) : B(0.2f + float(Rand(n, 24) % 1000u) / 1000.0f);
    for (int i = 0; i < 3; ++i) c.tgt[i] = Rand(n, 25 + i) % 6 == 0 ? FloatOf(n, 28 + i, 3u) : B(Unit(n, 31 + i) * 100.0f);
    c.timeRet = Rand(n, 34) % 3 == 0 ? Pick(kStackSeeds, n, 35) : Rand(n, 36) % 2;
    for (int i = 0; i < 3; ++i) c.origin[i] = B(Unit(n, 37 + i) * 100.0f);
    for (int i = 0; i < 4; ++i) {
        c.line[i] = B(0.5f + float(Rand(n, 40 + i) % 1500u) / 1000.0f);
        c.spr[i] = B(F(c.line[i]) * (0.2f + float(Rand(n, 44 + i) % 800u) / 1000.0f));
        c.comp[i] = B(float(Rand(n, 48 + i) % 1001u) / 1000.0f);
        c.wp[i] = B(Unit(n, 52 + i));
        for (int j = 0; j < 3; ++j) c.wheel[i][j] = B(Unit(n, 56 + 3 * i + j) * (j == 2 ? 1.5f : 1.0f) - (j == 2 ? 0.5f : 0.0f));
    }
    c.rotor = Rand(n, 70) % 5 == 0 ? FloatOf(n, 71, 3u) : B(float(Rand(n, 72) % 3000u) / 10000.0f);
    c.a0 = B(Unit(n, 73) * 20.0f); c.a1 = B(Unit(n, 74) * 20.0f);
    c.step = Rand(n, 75) % 6 == 0 ? FloatOf(n, 76, 4u) : B(0.5f + float(Rand(n, 77) % 3000u) / 1000.0f);
    for (int k = 0; k < 4; ++k) {
        c.nodePresent[k] = Rand(n, 78 + k) % 6 != 0;
        for (int i = 0; i < 16; ++i) c.frame[k][i] = B(Unit(n, 90 + 16 * k + i) * 50.0f);
    }
    c.matPresent = Rand(n, 160) % 8 != 0;
    c.gate = 1;
    switch (c.family) {
    case 0:   // foco de busqueda: bandera, intensidad, hora y objetivo con especiales (los NaN de señal se silencian en Y y Z)
        c.a11 = Rand(n, 200) % 3 == 0 ? Rand(n, 201) & 0xFFu : 1u;
        c.intens = Rand(n, 202) % 2 ? Near(n, 203, 0.0f) : B(0.01f + float(Rand(n, 204) % 1000u) / 100.0f);
        for (int i = 0; i < 3; ++i) c.tgt[i] = FloatOf(n, 205 + i, Rand(n, 208) % 2 ? 3u : 0u);
        c.b42b &= ~1u;
        break;
    case 1:   // foco de busqueda: argumentos y matriz; sin la suspension
        c.a11 = 1u; c.intens = B(0.5f + float(Rand(n, 200) % 100u) / 100.0f); c.timeRet = 1u;
        for (int i = 0; i < 3; ++i) c.origin[i] = FloatOf(n, 201 + i, 3u);
        c.b42b &= ~1u;
        break;
    case 2:   // suspension: relaciones normales
        c.b42b |= 1u; c.a11 = 0u;
        break;
    case 3:   // suspension: el trato directo o mezclado segun banderas
        c.b42b |= 1u; c.a11 = 0u;
        c.flags40 = Rand(n, 200) % 2 ? (c.flags40 | 0x4000000u) : (c.flags40 & ~0x4000000u);
        c.flags38c = Rand(n, 201) % 2 ? (c.flags38c | 0x20000u) : (c.flags38c & ~0x20000u);
        for (int i = 0; i < 4; ++i) c.wp[i] = Rand(n, 202 + i) % 2 ? B(-1.0f + Unit(n, 206 + i) * 0.1f) : B(0.5f + Unit(n, 206 + i));
        break;
    case 4:   // suspension: especiales en comp / spr / line / wp
        c.b42b |= 1u; c.a11 = 0u;
        for (int i = 0; i < 4; ++i) {
            if (Rand(n, 200 + i) % 3 == 0) c.comp[i] = FloatOf(n, 204 + i, 3u);
            if (Rand(n, 208 + i) % 3 == 0) c.spr[i] = FloatOf(n, 212 + i, 3u);
            if (Rand(n, 216 + i) % 3 == 0) c.line[i] = FloatOf(n, 220 + i, 3u);
            if (Rand(n, 224 + i) % 3 == 0) c.wp[i] = FloatOf(n, 228 + i, 3u);
        }
        break;
    case 5:   // suspension: especiales en la altura de la rueda y el manejo; lineas cero
        c.b42b |= 1u; c.a11 = 0u;
        for (int i = 0; i < 4; ++i) {
            if (Rand(n, 200 + i) % 3 == 0) c.wheel[i][2] = FloatOf(n, 204 + i, 3u);
            if (Rand(n, 208 + i) % 6 == 0) c.line[i] = B(0.0f);
            if (Rand(n, 212 + i) % 6 == 0) c.spr[i] = B(0.0f);
            if (Rand(n, 216 + i) % 6 == 0) c.comp[i] = c.line[i];
        }
        c.handB8 = FloatOf(n, 220, 3u);
        break;
    case 6:   // suspension: ratio cero, uno y desnormales
        c.b42b |= 1u; c.a11 = 0u;
        for (int i = 0; i < 4; ++i) {
            c.comp[i] = Pick(kUnit, n, 200 + i);
            if (Rand(n, 204 + i) % 2) c.spr[i] = Denorm(n, 208 + i);
            if (Rand(n, 212 + i) % 2) c.line[i] = Denorm(n, 216 + i);
            if (Rand(n, 220 + i) % 3 == 0) c.wp[i] = Denorm(n, 224 + i);
        }
        break;
    case 7:   // angulos: modelo pesado (1.66), normal, 1A1 (doble)
        c.b42b &= ~1u; c.a11 = 0u;
        c.model = Rand(n, 200) % 2 ? 0x1D5u : (Rand(n, 201) % 2 ? 0x1A1u : 0x1BFu);
        c.a0 = B(Unit(n, 202) * 30.0f); c.a1 = B(Unit(n, 203) * 30.0f);
        c.rotor = B(float(Rand(n, 204) % 3000u) / 1000.0f);
        break;
    case 8:   // angulos: a0 junto a -2pi, a1 junto a 2pi, paso y rotor con especiales
        c.b42b &= ~1u; c.a11 = 0u;
        c.a0 = Rand(n, 200) % 2 ? Near(n, 201, -6.2831855f) : B(-6.2831855f + Unit(n, 202) * 0.001f);
        c.a1 = Rand(n, 203) % 2 ? Near(n, 204, 6.2831855f) : B(6.2831855f + Unit(n, 205) * 0.001f);
        c.step = Rand(n, 206) % 2 ? FloatOf(n, 207, 3u) : c.step;
        c.rotor = Rand(n, 208) % 2 ? FloatOf(n, 209, 3u) : c.rotor;
        break;
    case 9:   // angulos con especiales (NaN no canonicos, desnormales): a0 / a1 sin bucle
        c.b42b &= ~1u; c.a11 = 0u;
        c.a0 = FloatOf(n, 200, 3u); c.a1 = FloatOf(n, 201, 3u);
        if (Rand(n, 202) % 2) c.step = FloatOf(n, 203, 3u);
        if (Rand(n, 204) % 2) c.rotor = FloatOf(n, 205, 3u);
        break;
    case 10:   // mezcla de todo
        break;
    case 11:   // paletas: nodos presentes y ausentes, posiciones de las paletas con especiales
        c.b42b &= ~1u; c.a11 = 0u;
        for (int k = 0; k < 4; ++k) for (int i = 12; i < 15; ++i) if (Rand(n, 200 + 3 * k + i) % 3 == 0) c.frame[k][i] = FloatOf(n, 230 + 3 * k + i, 3u);
        break;
    case 12:   // paletas: todos los nodos presentes, angulos moderados
        c.b42b &= ~1u; c.a11 = 0u;
        for (int k = 0; k < 4; ++k) c.nodePresent[k] = 1;
        break;
    case 13:   // modelos: pesados, 1A1, RC (sin polvo), vecinos de los umbrales de 16 bits
        c.model = ModelOf(n, 200);
        if (Rand(n, 201) % 2) c.model = (Rand(n, 202) % 2 ? 0x1D1u : 0x1F5u) + Rand(n, 203) % 3 - 1u;
        c.b42b &= ~1u;
        break;
    case 14:   // basura en los bits sin uso de las banderas
        c.b42b = Rand(n, 200) & 0xFFu; c.flags40 = Rand(n, 201); c.flags38c = Rand(n, 202); c.a11 = Rand(n, 203) & 0xFFu;
        break;
    case 15:   // tiempo: el valor de EAX tras GetIsTimeInRange (solo AL cuenta)
        c.a11 = 1u; c.intens = B(1.0f); c.b42b &= ~1u;
        c.timeRet = Rand(n, 200) % 2 ? (Rand(n, 201) & 0xFFFFFF00u) : (Rand(n, 202) | 1u);
        break;
    case 16:   // foco con matriz ausente seria un fallo del EXE; se prueba con matriz y orgenes especiales
        c.matPresent = 1; c.a11 = 1u; c.intens = B(2.0f); c.timeRet = 1u; c.b42b &= ~1u;
        for (int i = 0; i < 3; ++i) c.origin[i] = FloatOf(n, 200 + i, 5u);
        break;
    case 17:   // suspension mezclada con angulos y paletas
        c.b42b |= 1u; c.a11 = 0u;
        break;
    case 18:   // paso del temporizador cero y negativo, rotor cero
        c.step = Pick(kTiny, n, 200); if (Rand(n, 201) % 2) c.step = B(-F(c.step) - 1.0f);
        c.rotor = Rand(n, 202) % 2 ? B(0.0f) : c.rotor;
        break;
    case 19:   // modelo sin informacion util (indices extremos)
        c.model = Rand(n, 200) % 2 ? 0xFFFFu : 0u;
        c.b42b |= 1u;
        break;
    default:   // 20: mezcla de los extremos
        for (int i = 0; i < 4; ++i) { c.comp[i] = FloatOf(n, 200 + i, 5u); c.spr[i] = FloatOf(n, 204 + i, 5u); c.line[i] = FloatOf(n, 208 + i, 5u); c.wp[i] = FloatOf(n, 212 + i, 5u); c.wheel[i][2] = FloatOf(n, 216 + i, 5u); }
        c.rotor = FloatOf(n, 220, 5u); c.step = FloatOf(n, 221, 5u); c.a0 = FloatOf(n, 222, 5u); c.a1 = FloatOf(n, 223, 5u); c.handB8 = FloatOf(n, 224, 5u);
        break;
    }
    SafeAngles(c);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("model", c.model).U("b42b", c.b42b).U("flags40", c.flags40).U("flags38c", c.flags38c).U("hand_b8", c.handB8).U("a11", c.a11).U("intens", c.intens);
    j.U("tgt0", c.tgt[0]).U("tgt1", c.tgt[1]).U("tgt2", c.tgt[2]).U("time_ret", c.timeRet).U("origin0", c.origin[0]).U("origin1", c.origin[1]).U("origin2", c.origin[2]);
    for (int i = 0; i < 4; ++i) {
        j.U(("comp" + std::to_string(i)).c_str(), c.comp[i]).U(("spr" + std::to_string(i)).c_str(), c.spr[i]).U(("line" + std::to_string(i)).c_str(), c.line[i]).U(("wp" + std::to_string(i)).c_str(), c.wp[i]);
        for (int k = 0; k < 3; ++k) j.U(("wheel" + std::to_string(i) + "_" + std::to_string(k)).c_str(), c.wheel[i][k]);
    }
    j.U("rotor", c.rotor).U("a0", c.a0).U("a1", c.a1).U("step", c.step).U("mat_present", c.matPresent).U("gate", c.gate);
    for (int k = 0; k < 4; ++k) {
        j.U(("node" + std::to_string(k)).c_str(), c.nodePresent[k]);
        for (int i = 0; i < 16; ++i) j.U(("frame" + std::to_string(k) + "_" + std::to_string(i)).c_str(), c.frame[k][i]);
    }
    return j.End();
}
}

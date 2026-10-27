#pragma once
// Fixtures del banco HeliGen74 (lote 74): CHeli::GenerateHeli (EXE 0x6C6520), 21504 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CHeli::GenerateHeli  0x6C6520  cdecl (CPed* objetivo, bool noticias), el llamador limpia 8 bytes, resultado en EAX   (hook CHeli / GenerateHeli, tipo S)
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

namespace Hg74Fix {
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
constexpr std::size_t kHeliWin = 0xA18, kTgtWin = 0x60, kMatWin = 0x48;
constexpr std::size_t kArena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 74175u); }
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
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 rnd{};           // retorno de rand() (0x821B1E): solo cuenta el byte bajo
    u32 news{};          // argumento bool (dword): el EXE lee solo el byte bajo
    u32 tgtMat{};        // el objetivo tiene matriz (posicion en +0x30) o no (posicion compacta en +4)
    u32 base[3]{};       // posicion del objetivo
    u32 gz{};            // retorno de CWorld::FindGroundZForCoord
    u32 player{};        // retorno de FindPlayerPed (el EXE lo guarda y lo sobrescribe)
    u32 gate{};          // informativo
};

inline float Unit(u32 c, u32 k) { return float(int(Rand(c, k) % 2001u) - 1000) / 1000.0f; }   // [-1, 1]
inline u32 Nudge(u32 bits, int ulps) { return u32(i32(bits) + ulps); }

inline u32 RndByte(u32 n, u32 k, u32 byte) { return (Rand(n, k) & 0xFFFFFF00u) | (byte & 0xFFu); }   // rand() con basura en los bytes altos y el byte bajo elegido

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.ecx = SeedReg(n, 3); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.rnd = Rand(n, 10) % 5 == 0 ? Rand(n, 11) : Rand(n, 11) % 32768u;
    c.news = Rand(n, 12) % 3 == 0 ? Rand(n, 13) : (Rand(n, 13) & 0xFFFFFF00u) | (Rand(n, 14) % 2);
    c.tgtMat = Rand(n, 15) % 2;
    for (int i = 0; i < 3; ++i) c.base[i] = B(Unit(n, 20 + i) * 1500.0f);
    c.gz = B(Unit(n, 30) * 500.0f + 30.0f);
    c.player = Rand(n, 31);
    c.gate = 1;
    switch (c.family) {
    case 0:   // el caso base
        break;
    case 1: {  // cerca de los limites +-3000 (el desplazamiento de 250 cruza el limite): rama del angulo + pi
        for (int i = 0; i < 2; ++i) c.base[i] = B((Rand(n, 100 + i) % 2 ? 1.0f : -1.0f) * (2700.0f + Unit(n, 102 + i) * 420.0f));
        if (Rand(n, 105) % 3 == 0) c.base[Rand(n, 106) % 2] = B(Unit(n, 107) * 3000.0f);
        break;
    }
    case 2: {  // muy fuera: tras repetir con + pi sigue fuera (la segunda posicion no se vuelve a comprobar)
        static constexpr float big[]{1.0e5f, -1.0e5f, 1.0e7f, -1.0e7f, 1.0e30f, -1.0e30f, 3.0e38f, -3.0e38f, 3001.0f, -3001.0f, 6000.0f, -6000.0f};
        for (int i = 0; i < 3; ++i) if (Rand(n, 100 + i) % 2 || i == 0) c.base[i] = B(big[Rand(n, 103 + i) % std::size(big)] * (0.5f + Unit(n, 106 + i) * 0.5f));
        break;
    }
    case 3: {  // alrededor del Monte Chiliad (-2322, -1653): la posicion cae a menos de 350 de el
        c.base[0] = B(-2322.0f + Unit(n, 100) * 600.0f); c.base[1] = B(-1653.0f + Unit(n, 101) * 600.0f);
        const u32 t = Rand(n, 102) % 6;
        c.base[2] = t == 0 ? B(360.0f) : t == 1 ? Nudge(B(360.0f), int(Rand(n, 103) % 5u) - 2) : t == 2 ? B(Unit(n, 104) * 800.0f) : t == 3 ? B(560.0f - 200.0f) : t == 4 ? B(Unit(n, 105) * 5000.0f) : FloatOf(n, 106, 3u);
        break;
    }
    case 4: {  // altura final: la candidata (z + 50 o la elevada) frente a suelo + 20
        c.base[0] = B(Unit(n, 100) * 1500.0f); c.base[1] = B(Unit(n, 101) * 1500.0f);
        const float bz = Unit(n, 102) * 800.0f;
        c.base[2] = B(bz);
        const u32 t = Rand(n, 103) % 6;
        const float cand = bz + 50.0f;
        c.gz = t == 0 ? B(cand - 20.0f) : t == 1 ? Nudge(B(cand - 20.0f), int(Rand(n, 104) % 3u) - 1) : t == 2 ? B(cand - 20.0f + Unit(n, 105) * 5.0f) : t == 3 ? B(Unit(n, 106) * 900.0f) : t == 4 ? B(cand - 21.0f) : B(cand - 19.0f);
        if (Rand(n, 107) % 6 == 0) { c.base[0] = B(-2322.0f + Unit(n, 108) * 300.0f); c.base[1] = B(-1653.0f + Unit(n, 109) * 300.0f); c.gz = B(Unit(n, 110) * 800.0f); }
        break;
    }
    case 5:   // los 256 valores del byte bajo de rand() (angulo) con basura arriba
        c.rnd = RndByte(n, 100, (n / 16) % 256u);
        if (Rand(n, 101) % 4 == 0) c.rnd = Rand(n, 102) | 0x80000000u;
        break;
    case 6: {  // el argumento bool: solo cuenta el byte bajo
        static constexpr u32 v[]{0u, 1u, 2u, 0x80u, 0xFFu, 0x7Fu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0x01010100u};
        c.news = Rand(n, 100) % 3 == 0 ? Rand(n, 101) : v[Rand(n, 102) % std::size(v)];
        break;
    }
    case 7:   // objetivo con y sin matriz, con la misma posicion
        c.tgtMat = n / 16 % 2;
        break;
    case 8: {  // especiales en la posicion (NaN no canonicos, infinitos, ceros con signo, desnormales)
        for (int i = 0; i < 3; ++i) if (Rand(n, 100 + i) % 2 || i == 0) c.base[i] = FloatOf(n, 103 + i, 3u);
        break;
    }
    case 9:   // especiales en el suelo
        c.gz = FloatOf(n, 100, 3u);
        if (Rand(n, 101) % 3 == 0) c.base[2] = FloatOf(n, 102, 3u);
        break;
    case 10:   // solo la altura base especial (el resto normal), dentro y fuera del Chiliad
        c.base[2] = FloatOf(n, 100, 3u);
        if (Rand(n, 101) % 2) { c.base[0] = B(-2322.0f + Unit(n, 102) * 200.0f); c.base[1] = B(-1653.0f + Unit(n, 103) * 200.0f); }
        break;
    case 11:   // mezcla de rangos en todo
        for (int i = 0; i < 3; ++i) c.base[i] = FloatOf(n, 100 + i, 5u);
        c.gz = FloatOf(n, 104, 5u);
        break;
    case 12: {  // exponentes extremos de la base (el redondeo de cos * 250 + base a 24 bits cambia de exponente)
        for (int i = 0; i < 3; ++i) c.base[i] = ExpFloat(n, 100 + i, int(Rand(n, 103 + i) % 60u) - 30);
        c.gz = ExpFloat(n, 106, int(Rand(n, 107) % 60u) - 30);
        break;
    }
    case 13:   // magnitudes diminutas / desnormales de la base: gana el desplazamiento
        for (int i = 0; i < 3; ++i) c.base[i] = Rand(n, 100 + i) % 2 ? Denorm(n, 103 + i) : ExpFloat(n, 106 + i, -126 + int(Rand(n, 109 + i) % 20u));
        c.gz = Rand(n, 112) % 2 ? Denorm(n, 113) : ExpFloat(n, 114, -126 + int(Rand(n, 115) % 20u));
        break;
    case 14: {  // frontera exacta del cuadrado +-3000 con angulo 0 (cos = 1, sin = 0): px = base.x + 250, py = base.y
        c.rnd = RndByte(n, 100, 0u);
        static constexpr float xs[]{2750.0f, -3250.0f, 2749.0f, 2751.0f, -3249.0f, -3251.0f, 0.0f};
        static constexpr float ys[]{3000.0f, -3000.0f, 2999.0f, 3001.0f, -2999.0f, -3001.0f, 0.0f};
        float bx = xs[Rand(n, 101) % std::size(xs)], by = ys[Rand(n, 102) % std::size(ys)];
        c.base[0] = Rand(n, 103) % 2 ? Nudge(B(bx), int(Rand(n, 104) % 3u) - 1) : B(bx);
        c.base[1] = Rand(n, 105) % 2 ? Nudge(B(by), int(Rand(n, 106) % 3u) - 1) : B(by);
        if (Rand(n, 107) % 2) std::swap(c.base[0], c.base[1]);
        break;
    }
    case 15: {  // frontera exacta de la distancia al Chiliad (350) con angulo 0: (px + 2322, py + 1653) = triplete pitagorico
        c.rnd = RndByte(n, 100, 0u);
        static constexpr float dx[]{210.0f, -210.0f, 350.0f, -350.0f, 0.0f, 0.0f, 280.0f, -280.0f, 209.0f, 211.0f};
        static constexpr float dy[]{280.0f, -280.0f, 0.0f, 0.0f, 350.0f, -350.0f, 210.0f, -210.0f, 280.0f, 280.0f};
        const u32 i = Rand(n, 101) % std::size(dx);
        c.base[0] = B(-2322.0f + dx[i] - 250.0f); c.base[1] = B(-1653.0f + dy[i]);
        if (Rand(n, 102) % 3 == 0) c.base[Rand(n, 103) % 2] = Nudge(c.base[Rand(n, 103) % 2], int(Rand(n, 104) % 3u) - 1);
        c.base[2] = B(Unit(n, 105) * 800.0f);
        break;
    }
    case 16: {  // bases enormes pero finitas con angulos variados: cos * 250 + base redondea a 24 bits
        for (int i = 0; i < 2; ++i) c.base[i] = ExpFloat(n, 100 + i, 10 + int(Rand(n, 103 + i) % 18u));
        c.base[2] = B(Unit(n, 106) * 1.0e4f);
        break;
    }
    case 17:   // altura base NaN / infinita con el angulo variado, dentro y fuera del Chiliad
        c.base[2] = Pick(kNan, n, 100); if (Rand(n, 101) % 3 == 0) c.base[2] = Pick(kInf, n, 102);
        if (Rand(n, 103) % 2) { c.base[0] = B(-2322.0f + Unit(n, 104) * 250.0f); c.base[1] = B(-1653.0f + Unit(n, 105) * 250.0f); }
        break;
    case 18:   // suelo enorme / infinito / negativo grande
        c.gz = Rand(n, 100) % 3 == 0 ? Pick(kInf, n, 101) : Rand(n, 102) % 2 ? Pick(kBig, n, 103) : B(Unit(n, 104) * 1.0e6f);
        break;
    case 19:   // todo con NaN no canonicos y de senal (posicion y suelo)
        for (int i = 0; i < 3; ++i) if (Rand(n, 100 + i) % 2 == 0) c.base[i] = Pick(kNan, n, 103 + i);
        if (Rand(n, 106) % 2) c.gz = Pick(kNan, n, 107);
        break;
    default: {  // 20: angulo 0 y 255 (extremos) con posicion en el limite del cuadrado
        c.rnd = RndByte(n, 100, Rand(n, 101) % 2 ? 0u : 255u);
        c.base[0] = B(Rand(n, 102) % 2 ? 2750.0f : -3250.0f); c.base[1] = B(Unit(n, 103) * 3000.0f);
        break;
    }
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("rnd", c.rnd).U("news", c.news).U("tgt_mat", c.tgtMat).U("gz", c.gz).U("player", c.player).U("gate", c.gate);
    j.U("base_x", c.base[0]).U("base_y", c.base[1]).U("base_z", c.base[2]);
    return j.End();
}
}

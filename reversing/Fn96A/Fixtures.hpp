#pragma once
// Fixtures del banco Fn96A (lote 96A): 16384 casos por funcion (65536 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CVector::Normalise        0x59C910  thiscall (ECX = vector de 12 bytes), void, `ret`; in situ                          (hook S)
//   1 CVector::NormaliseAndMag  0x59C970  thiscall, in situ; devuelve float en ST0 (1 / inverso, sin redondear a float)      (hook S)
//   2 CVector::Magnitude        0x4082C0  thiscall const; devuelve float en ST0 (raiz sin redondear a float)                  (hook S)
//   3 CVector::Magnitude2D      0x406D50  thiscall const; devuelve float en ST0                                               (hook S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Familia = n % 16 (1024 casos por familia).
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

namespace F96AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVecWin = 0x10;   // vector (12 bytes) + 4 bytes de vecindad (+0xC)
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 96001u); }
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

// ------------------------------------------------------------------------------------------------ el caso
//   a[0..2] = x, y, z (patrones de bits), a[3] = dword vecino (+0xC) que ninguna funcion debe tocar
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    std::array<u32, 4> a{};
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 Pk(std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); }
    float Fl(float lo, float hi) { return lo + (hi - lo) * (float(R(100001)) / 100000.0f); }
    u32 W() { return Rand(n, nk()); }
};

inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
// NaN de entrada: indefinido, silencioso positivo, silencioso con carga, senalizantes (con y sin carga), silencioso grande
inline constexpr std::array<u32, 6> kNanBits{0xFFC00000u, 0x7FC00000u, 0x7FC12345u, 0x7F800001u, 0xFF812345u, 0xFFE00001u};
// NaN con partes significativas (tras silenciarlos) todas distintas: la x87 elige el de mayor significando, sin empates de signo
inline constexpr std::array<u32, 6> kNanDistinct{0xFFC00000u, 0x7FC12345u, 0x7F800001u, 0xFF812346u, 0x7FE00001u, 0xFFC00002u};

inline u32 SignedZero(Gen& g) { return g.P(50) ? 0x80000000u : 0u; }
// Un float de exponente (sin sesgo) en [e0, e1], mantisa de 23 bits aleatoria y signo aleatorio
inline u32 NormalBits(Gen& g, int e0, int e1) {
    const u32 e = u32(127 + e0 + int(g.R(u32(e1 - e0 + 1))));
    return (g.W() & 0x80000000u) | (e << 23) | (g.W() & 0x7FFFFFu);
}
// Un float "especial" que no es NaN: cero, desnormal, infinito, enorme, coordenada
inline u32 SpecialNoNan(Gen& g) {
    const u32 r = g.R(20);
    if (r < 8) return kTiny[g.R(12)];
    if (r < 12) return kInf[g.R(4)];
    if (r < 15) return kBig[g.R(6)];
    return B(float(int(g.R(2001)) - 1000) / 7.0f);
}
inline u32 Special(Gen& g) { return g.P(25) ? kNanBits[g.R(6)] : SpecialNoNan(g); }
inline u32 NextUp(u32 bits, int steps) {   // desplaza un float normal `steps` unidades de ultimo lugar (signo respetado)
    const bool neg = (bits >> 31) != 0;
    u32 mag = bits & 0x7FFFFFFFu;
    mag = u32(i32(mag) + (neg ? -steps : steps));
    return (bits & 0x80000000u) | (mag & 0x7FFFFFFFu);
}
// Un float de magnitud aleatoria (de 1e-30 a 1e30) con signo
inline u32 Wide(Gen& g) {
    const double e = double(int(g.R(61)) - 30);
    const double m = 1.0 + double(g.R(1000000)) / 1000000.0;
    return B(float((g.P(50) ? -1.0 : 1.0) * m * std::pow(10.0, e)));
}
// Una coordenada de juego en metros, con signo
inline u32 Coord(Gen& g, float range = 3000.0f) { return B(g.Fl(-range, range)); }

// ------------------------------------------------------------------------------------------------ generador (16 familias, el mismo para las cuatro funciones con semilla propia)
inline void MakeVec(C& c, unsigned n, unsigned fn) {
    const u32 f = c.family;
    Gen g(n, 100 + 50 * fn);
    u32* v = &c.a[0];
    c.a[3] = g.W();
    switch (f) {
    case 0:     // coordenadas de juego, en metros
        for (int k = 0; k < 3; ++k) v[k] = Coord(g);
        break;
    case 1:     // componentes en [-1, 1] (a veces ya unitario)
        for (int k = 0; k < 3; ++k) v[k] = B(g.Fl(-1.0f, 1.0f));
        if (g.P(15)) {
            const float x = g.Fl(-0.7f, 0.7f), y = g.Fl(-0.7f, 0.7f), z = std::sqrt(std::max(0.0f, 1.0f - x * x - y * y));
            v[0] = B(x); v[1] = B(y); v[2] = B(g.P(50) ? z : -z);
        }
        break;
    case 2: {   // ceros y ejes: todo cero (con signos), un solo eje, dos ejes
        const u32 m = g.R(4);
        for (int k = 0; k < 3; ++k) v[k] = SignedZero(g);
        if (m >= 1) v[g.R(3)] = g.P(50) ? Coord(g) : Wide(g);
        if (m >= 2) v[g.R(3)] = Coord(g);
        if (m == 3) v[g.R(3)] = kTiny[g.R(12)];
        break;
    }
    case 3:     // diminutos: sus cuadrados son 0 como float pero no en la x87
        for (int k = 0; k < 3; ++k) v[k] = g.P(15) ? kTiny[g.R(12)] : NormalBits(g, -75, -10);
        break;
    case 4:     // enormes: la suma de cuadrados sale del rango de float
        for (int k = 0; k < 3; ++k) v[k] = g.P(12) ? NormalBits(g, -20, 20) : NormalBits(g, 40, 126);
        break;
    case 5:     // un NaN (de cualquier carga, senalizante o no) en una componente
        for (int k = 0; k < 3; ++k) v[k] = g.P(30) ? SpecialNoNan(g) : Coord(g);
        v[g.R(3)] = kNanBits[g.R(6)];
        if (fn == 3 && g.P(50)) v[2] = kNanBits[g.R(6)];   // z (ignorada por Magnitude2D), con otra carga posible
        break;
    case 6:     // infinitos solos o con ceros y finitos
        for (int k = 0; k < 3; ++k) v[k] = g.P(50) ? kInf[g.R(4)] : (g.P(40) ? SignedZero(g) : Coord(g));
        if (g.P(50)) v[g.R(3)] = kInf[g.R(4)];
        break;
    case 7:     // escalas mezcladas
        for (int k = 0; k < 3; ++k) v[k] = g.P(50) ? NormalBits(g, -60, -30) : NormalBits(g, 40, 100);
        if (g.P(30)) v[g.R(3)] = SignedZero(g);
        break;
    case 8: {   // el normalizado de la componente pequena es denormal o 0 (x / |v| entre 2^-160 y 2^-130)
        const int bigExp = int(g.R(40)) + 10;
        const int smallExp = -(130 + int(g.R(30)) - bigExp);
        const u32 pos[3] = {NormalBits(g, smallExp, smallExp), NormalBits(g, bigExp, bigExp), g.P(50) ? NormalBits(g, bigExp - int(g.R(5)), bigExp) : SignedZero(g)};
        static const int P3[6][3] = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};
        const u32 perm = g.R(6);
        for (int k = 0; k < 3; ++k) v[P3[perm][k]] = pos[k];
        break;
    }
    case 9:     // mantisas completas aleatorias en un rango moderado: redondeo a 24 bits de producto, suma, raiz y division
        for (int k = 0; k < 3; ++k) v[k] = NormalBits(g, -6, 6);
        break;
    case 10:    // patrones de 32 bits cualesquiera
        for (int k = 0; k < 3; ++k) v[k] = g.W();
        break;
    case 11: {  // varios NaN con cargas distintas (o identicas): cual elige la x87
        const u32 cnt = 2 + g.R(2);
        for (int k = 0; k < 3; ++k) v[k] = g.P(50) ? Coord(g) : SignedZero(g);
        u32 idx[3] = {0, 1, 2};
        for (int k = 2; k > 0; --k) std::swap(idx[k], idx[g.R(u32(k + 1))]);
        u32 pool[6] = {kNanDistinct[0], kNanDistinct[1], kNanDistinct[2], kNanDistinct[3], kNanDistinct[4], kNanDistinct[5]};
        for (int k = 5; k > 0; --k) std::swap(pool[k], pool[g.R(u32(k + 1))]);
        const bool same = g.P(25);
        for (u32 k = 0; k < cnt; ++k) v[idx[k]] = same ? pool[0] : pool[k];
        break;
    }
    case 12:    // magnitud cerca de y por encima de FLT_MAX: el resultado en ST0 supera el rango de float
        for (int k = 0; k < 3; ++k) v[k] = g.P(15) ? SignedZero(g) : (g.P(35) ? kBig[g.R(4)] : NormalBits(g, 60, 126));
        break;
    case 13:    // x e y normales y z especial (Magnitude2D la ignora), o x e y nulos y z distinta de 0
        if (g.P(65)) { v[0] = Coord(g); v[1] = Coord(g); v[2] = Special(g); }
        else { v[0] = SignedZero(g); v[1] = SignedZero(g); v[2] = g.P(70) ? Coord(g) : Special(g); }
        if (g.P(25)) std::swap(v[g.R(2)], v[2]);
        break;
    case 14: {  // resultados exactos y empates: ternas pitagoricas, potencias de dos, y unos pocos ulps
        static const float T[][3] = {{3, 4, 0}, {0, 3, 4}, {2, 3, 6}, {1, 2, 2}, {1, 1, 1}, {1, 0, 0}, {0, 0, 1}, {2, 2, 1}, {6, 2, 3}, {4, 4, 7}, {12, 4, 3}, {1, 4, 8}};
        const float* t = T[g.R(12)];
        static const float S[] = {1.0f, 2.0f, 0.5f, 8.0f, 0.125f, 1024.0f, 3.0f};
        const float s = S[g.R(7)];
        for (int k = 0; k < 3; ++k) v[k] = B(t[k] * s);
        if (g.P(60)) for (int k = 0; k < 3; ++k) if (g.P(50)) v[k] = NextUp(v[k], int(g.R(7)) - 3);
        break;
    }
    default:    // ceros con signo y diminutos con signo
        for (int k = 0; k < 3; ++k) v[k] = g.P(50) ? SignedZero(g) : kTiny[g.R(12)];
        if (g.P(30)) v[g.R(3)] = Coord(g);
        break;
    }
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    MakeVec(c, n, fn);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    return j.End();
}
}

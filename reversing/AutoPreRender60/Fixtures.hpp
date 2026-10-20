#pragma once
// Fixtures del banco AutoPreRender60: CAutomobile::PreRender (0x6AAB50, virtual, thiscall sin argumentos, void; `ret 0`).
// Solo C++ estandar (sin tipos del juego): lo incluyen Core.hpp y dump_fixtures.cpp (compilado con cl para la autoprueba cruzada con fixture_oracle.py).
// El oraculo del banco es el propio EXE (hook apagado). Esta cabecera genera los PARAMETROS de cada caso (semilla, modelo, bandera de valores extremos, semillas de registros y de
// los falsos); ApplyWorld (Core.hpp) construye el mundo a partir de ellos. JsonCase escribe la fila `input`; fixture_oracle.py la regenera por su cuenta y la contrasta.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace AutoPreRender60Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 8192;   // pares (una sola funcion)
constexpr unsigned kTotal = kCases;

struct Rng {
    u64 s;
    explicit Rng(u64 seed) : s(seed * 0x9E3779B97F4A7C15ull + 0x1234567ull) { for (int i = 0; i < 6; ++i) Next(); }
    u32 Next() { s ^= s << 13; s ^= s >> 7; s ^= s << 17; return u32((s >> 11) ^ (s >> 32)); }
    float Unit() { return float(Next() & 0xFFFFFF) / 16777216.0f; }
    float Range(float a, float b) { return a + (b - a) * Unit(); }
    bool Chance(u32 num, u32 den) { return Next() % den < num; }
    u32 Pick(u32 n) { return Next() % n; }
};
inline float Bits(u32 b) { float f; std::memcpy(&f, &b, 4); return f; }
inline u32 BitsOf(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
constexpr float kFltMax = 3.402823466e+38f;
constexpr float kFltMin = 1.175494351e-38f;
inline float WildFloat(Rng& r) {
    switch (r.Pick(20)) {
    case 0: return 0.0f;
    case 1: return -0.0f;
    case 2: return Bits(0x7F800000u);
    case 3: return Bits(0xFF800000u);
    case 4: return Bits(0x7FC00000u);       // NaN silencioso
    case 5: return kFltMax;
    case 6: return -kFltMax;
    case 7: return kFltMin;
    case 8: return Bits(0x00000010u);       // denormal
    case 9: return 1e30f;
    case 10: return -1e-30f;
    case 11: { u32 b = r.Next(); if (((b >> 23) & 0xFF) == 0xFF && (b & 0x7FFFFF)) b |= 0x400000; return Bits(b); }   // bits crudos (nunca sNaN)
    case 12: return 1.0f;
    case 13: return -1.0f;
    case 14: return 1e19f;
    case 15: return Bits(r.Next() & 0x7FFFFF);                // subnormal cualquiera
    case 16: return Bits(0x00800000u + (r.Next() & 0xFFFF)); // normal junto a FLT_MIN
    case 17: return Bits(0x80000000u | (r.Next() & 0x7FFFFF)); // subnormal negativo
    case 18: return Bits(0x7FC00000u | (r.Next() & 0x3FFFFF) | ((r.Next() & 1) << 31)); // NaN silencioso con carga y signo cualesquiera
    case 19: return Bits(0x7F800000u | ((r.Next() & 1) << 31));   // infinito
    default: return r.Range(-1000.f, 1000.f);
    }
}
struct Gen {
    Rng& r;
    bool wild;
    float F(float lo, float hi) { return wild && r.Chance(1, 10) ? WildFloat(r) : r.Range(lo, hi); }
};

// 31 modelos: los 28 con rama propia + 400, 503 y -2 (0xFFFE)
inline const std::array<int, 31> kModels{400, 407, 416, 420, 424, 427, 432, 434, 438, 441, 443, 471, 477, 486, 490, 495, 524, 525, 530, 531, 532, 539, 544, 568, 596, 597, 598, 599, 601, 503, -2};

struct Case { unsigned n{}; u64 seed{}; bool wild{}; int model{}; u32 eax{}, edx{}, stack{}; u64 fakeSeed{}; };
inline Case MakeCase(unsigned n) {
    const unsigned k = n;
    Rng r(0xC0DEull * (k + 1));
    Case c;
    c.n = k;
    { const u64 lo = r.Next(); const u64 hi = r.Next(); c.seed = lo ^ (hi << 32); }   // orden de evaluacion explicito
    c.wild = (k % 5) >= 3;
    c.model = kModels[k % kModels.size()];
    c.eax = r.Next(); c.edx = r.Next(); c.stack = r.Next();
    { const u64 lo = r.Next(); const u64 hi = r.Next(); c.fakeSeed = lo | (hi << 32); }
    if (c.fakeSeed == 0) c.fakeSeed = 1;
    return c;
}
inline std::string JsonCase(const Case& c) {
    return "{\"n\":" + std::to_string(c.n) + ",\"seed\":" + std::to_string(c.seed) + ",\"wild\":" + std::to_string(int(c.wild)) + ",\"model\":" + std::to_string(c.model)
        + ",\"eax\":" + std::to_string(c.eax) + ",\"edx\":" + std::to_string(c.edx) + ",\"stack\":" + std::to_string(c.stack) + ",\"fake_seed\":" + std::to_string(c.fakeSeed)
        + ",\"boundary\":" + std::to_string(int(c.n % 11 == 0)) + "}";
}
inline u64 Fnv64(const u8* p, std::size_t n) {
    u64 h = 0xCBF29CE484222325ull;
    for (std::size_t i = 0; i < n; ++i) { h ^= p[i]; h *= 0x100000001B3ull; }
    return h;
}
}  // namespace AutoPreRender60Fix

#pragma once
// Fixtures del banco Fn94A (lote 94A): 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskComplexUseMobilePhone::Quit              0x634A40  thiscall (ECX = tarea de 0x20 bytes; ped en pila), `ret 4`; void   (hook S)
//   1 CTaskComplexKillPedOnFoot::SayGangAttack      0x65E9A0  cdecl (ped, objetivo); void                                          (hook S)
//   2 CTaskComplexWalkRoundObject::ComputeRouteRoundObject  0x6551D0  thiscall (ECX = tarea de 0x54 bytes; ped en pila), `ret 4`; devuelve float en ST0 (hook S)
//   3 CTaskComplexPartner::ComputeTargetPoints      0x681FE0  thiscall (ECX = tarea de 0x70 bytes; ped, &salida1, &salida2 en pila), `ret 0Ch`; void (hook S)
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

namespace F94AFix {
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
constexpr std::size_t kPedWin = 0x5A0, kMatWin = 0x40, kObjWin = 0x40, kRouteWin = 0x40, kTask0Win = 0x20, kTask2Win = 0x54, kTask3Win = 0x70;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 94001u); }
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
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }


// ------------------------------------------------------------------------------------------------ el caso
// Cada funcion interpreta `a` como se documenta en su generador; todos los valores viajan como u32 (los floats, como su patron de bits).
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    std::array<u32, 48> a{};
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
// Un float "especial": cero, desnormal, infinito, NaN (un unico patron), enorme
inline u32 Special(Gen& g) {
    const u32 r = g.R(32);
    if (r < 10) return kTiny[g.R(12)];
    if (r < 16) return kInf[g.R(4)];
    if (r < 24) return 0xFFC00000u;
    if (r < 28) return kBig[g.R(6)];
    return B(float(int(g.R(2001)) - 1000) / 7.0f);
}
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

// ------------------------------------------------------------------------------------------------ funcion 0: CTaskComplexUseMobilePhone::Quit (0x634A40)
//   a[0] = dword +0x1C de la tarea (byte +0x1C m_bIsAborting, +0x1D m_bQuit, +0x1E/+0x1F basura), a[1] = ped (0 arena, 1 nulo, 2 valor crudo 0x00BADC00), a[2] = valor que devuelve MakeAbortable
inline void Make0(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    u32 quit;
    switch (f) {
    case 0: quit = 0; break;
    case 1: quit = 1; break;
    case 2: quit = 2 + g.R(254); break;
    case 3: quit = g.Pk({2u, 0x7Fu, 0x80u, 0xFFu, 0x81u, 0x10u}); break;
    default: quit = g.P(40) ? 0u : (g.P(40) ? 1u : 1u + g.R(255)); break;
    }
    const u32 lo = (f == 6 || f == 7 || g.P(50)) ? g.R(256) : 0u;
    const u32 hi = (f == 6 || f == 7 || g.P(50)) ? (g.W() & 0xFFFFu) : 0u;
    if (f == 6) quit = 0;
    if (f == 7) quit = 1 + g.R(255);
    c.a[0] = lo | (quit << 8) | (hi << 16);
    c.a[1] = f == 4 ? 1u : (f == 5 ? 2u : (g.P(85) ? 0u : g.R(3)));
    c.a[2] = g.P(50) ? g.R(2) : g.W();
}

// ------------------------------------------------------------------------------------------------ funcion 1: CTaskComplexKillPedOnFoot::SayGangAttack (0x65E9A0)
//   a[0] = atacante (0 arena, 1 nulo), a[1] = objetivo (0 arena propia, 1 nulo, 2 el mismo objeto que el atacante), a[2] = tipo de peaton del atacante, a[3] = tipo del objetivo (+0x598),
//   a[4] = lo que devuelve FindPlayerPed(0) (0 el objetivo tal como se paso, 1 nulo, 2 el atacante, 3 valor crudo 0x00ABCD00)
inline u32 GangType(Gen& g) { return 7u + g.R(10); }
inline u32 OtherType(Gen& g) {
    return g.Pk({0u, 1u, 4u, 5u, 6u, 17u, 18u, 22u, 31u, 32u, 0x100u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0xFFFFFFF9u, 0xFFFFFFFEu, 0x10000007u, 0x00000107u});
}
inline u32 AnyType(Gen& g) { return g.P(50) ? GangType(g) : (g.P(75) ? OtherType(g) : g.R(40)); }
inline void Make1(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = 0; c.a[1] = 0;
    c.a[2] = AnyType(g); c.a[3] = AnyType(g); c.a[4] = g.Pk({0u, 0u, 1u, 2u, 3u});
    switch (f) {
    case 0: c.a[2] = GangType(g); c.a[3] = g.Pk({7u, 9u, 14u, 7u, 9u, 14u, 8u}); break;             // dice la frase (ataque a una pandilla)
    case 1: c.a[2] = GangType(g); c.a[3] = GangType(g); break;
    case 2: c.a[2] = GangType(g); c.a[3] = OtherType(g); c.a[4] = 0; break;                          // el objetivo es el jugador: pasa la comprobacion pero ningun caso coincide
    case 3: c.a[2] = GangType(g); c.a[3] = OtherType(g); c.a[4] = g.Pk({1u, 2u, 3u}); break;         // no es el jugador: sale
    case 4: c.a[2] = OtherType(g); break;                                                              // el atacante no es de pandilla
    case 5: c.a[0] = 1; break;
    case 6: c.a[1] = 1; break;
    case 7: c.a[2] = g.Pk({6u, 7u, 16u, 17u}); c.a[3] = g.Pk({6u, 7u, 9u, 14u, 16u, 17u}); c.a[4] = g.Pk({0u, 1u}); break;
    case 8: c.a[2] = g.Pk({0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 7u, 16u, 0xFFFFFFF9u}); c.a[3] = g.Pk({0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 7u, 9u, 14u, 16u, 0xFFFFFFF9u}); c.a[4] = g.R(4); break;
    case 9: c.a[1] = 2; c.a[2] = g.P(70) ? GangType(g) : AnyType(g); c.a[4] = g.R(4); break;           // atacante y objetivo son el mismo objeto
    case 10: c.a[2] = GangType(g); c.a[3] = g.Pk({7u, 9u, 14u}); c.a[4] = g.R(4); break;
    case 11: c.a[2] = GangType(g); c.a[3] = g.Pk({10u, 11u, 12u, 13u, 15u, 16u}); break;
    default:
        if (g.P(8)) c.a[0] = 1;
        if (g.P(8)) c.a[1] = g.P(60) ? 1u : 2u;
        break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 2: CTaskComplexWalkRoundObject::ComputeRouteRoundObject (0x6551D0)
//   a[0] = el peaton tiene matriz (la posicion esta en matriz + 0x30; si no, en el placement +4), a[1..3] = posicion en la activa, a[4..6] = posicion en la otra (basura),
//   a[7] = margen anterior (0x8D22B0), a[8..10] = punto destino de la tarea (+0x10), a[11] = objeto (0 arena, 1 valor crudo 0x00C0FFEE), a[12] = ruta (0 arena, 1 valor crudo 0x00A0B0C0),
//   a[13..24] = los 4 planos (x, y, z) que escribe el falso de 0x5F3660, a[25..28] = sus 4 distancias, a[29] = el lado que devuelve el falso de 0x5F3AC0 (0..3), a[30] = valor que devuelve 0x5F6110
inline void Make2(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(60);
    for (u32 k = 0; k < 3; ++k) { c.a[1 + k] = Coord(g); c.a[4 + k] = Coord(g); c.a[8 + k] = Coord(g); }
    c.a[7] = g.P(70) ? B(g.Fl(0.0f, 2.0f)) : g.W();
    c.a[11] = g.P(90) ? 0u : 1u; c.a[12] = g.P(90) ? 0u : 1u;
    for (u32 k = 0; k < 12; ++k) c.a[13 + k] = B(g.Fl(-1.0f, 1.0f));
    for (u32 k = 0; k < 4; ++k) c.a[25 + k] = Coord(g, 500.0f);
    c.a[29] = g.R(4); c.a[30] = g.W();
    switch (f) {
    case 0: c.a[0] = 1; break;
    case 1: c.a[0] = 0; break;
    case 2: c.a[29] = n / 16 % 4; break;
    case 3: for (u32 k = 0; k < 12; ++k) if (g.P(35)) c.a[13 + k] = Special(g); break;
    case 4: for (u32 k = 1; k < 4; ++k) if (g.P(40)) c.a[k] = Special(g); break;
    case 5: for (u32 k = 0; k < 4; ++k) if (g.P(50)) c.a[25 + k] = Special(g); break;
    case 6:   // magnitudes enormes: productos y sumas fuera del rango de float en los registros de la x87
        for (u32 k = 0; k < 12; ++k) if (g.P(60)) c.a[13 + k] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u, 0x7D000000u, 0x60000000u, 0xE0000000u});
        for (u32 k = 1; k < 4; ++k) if (g.P(60)) c.a[k] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0x70000000u, 0xF0000000u});
        for (u32 k = 0; k < 4; ++k) if (g.P(40)) c.a[25 + k] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u});
        break;
    case 7:   // magnitudes diminutas (desnormales tras guardar como float, normales en el registro)
        for (u32 k = 0; k < 12; ++k) if (g.P(60)) c.a[13 + k] = g.Pk({0x00000100u, 0x80000100u, 0x007FFFFFu, 0x00800000u, 0x80800000u, 0x1E000000u, 0x9E000000u, 0x20000000u, 0x00400000u});
        for (u32 k = 1; k < 4; ++k) if (g.P(60)) c.a[k] = g.Pk({0x00000100u, 0x80000100u, 0x007FFFFFu, 0x00800000u, 0x80800000u, 0x1E000000u, 0x9E000000u, 0x20000000u, 0x00400000u});
        for (u32 k = 0; k < 4; ++k) if (g.P(50)) c.a[25 + k] = g.Pk({0x00000100u, 0x80000100u, 0x00800000u, 0x80800000u, 0x00400000u, 0x1E000000u});
        break;
    case 8:   // NaN e infinitos
        for (u32 k = 0; k < 12; ++k) if (g.P(25)) c.a[13 + k] = g.Pk({0x7F800000u, 0xFF800000u, 0xFFC00000u, 0u, 0x80000000u});
        for (u32 k = 1; k < 4; ++k) if (g.P(25)) c.a[k] = g.Pk({0x7F800000u, 0xFF800000u, 0xFFC00000u, 0u, 0x80000000u});
        for (u32 k = 0; k < 4; ++k) if (g.P(25)) c.a[25 + k] = g.Pk({0x7F800000u, 0xFF800000u, 0xFFC00000u, 0u, 0x80000000u});
        break;
    case 9:   // cancelaciones: la distancia casi compensa el producto escalar
        {
            const u32 s = c.a[29];
            const double px = double(F(c.a[1])), py = double(F(c.a[2])), pz = double(F(c.a[3]));
            const double dot = double(F(c.a[13 + 3 * s + 2])) * pz + double(F(c.a[13 + 3 * s + 1])) * py + double(F(c.a[13 + 3 * s])) * px;
            c.a[25 + s] = NextUp(B(float(-dot)), int(g.R(5)) - 2);
        }
        break;
    case 10:  // ceros con signo y valores enteros pequenos
        for (u32 k = 0; k < 12; ++k) if (g.P(50)) c.a[13 + k] = g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u});
        for (u32 k = 1; k < 4; ++k) if (g.P(40)) c.a[k] = g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x40000000u});
        for (u32 k = 0; k < 4; ++k) if (g.P(40)) c.a[25 + k] = g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u});
        break;
    case 11:  // margen anterior con valores especiales y valores crudos de objeto y ruta
        c.a[7] = Special(g); c.a[11] = g.R(2); c.a[12] = g.R(2);
        break;
    case 12:  // posicion en la otra estructura con basura especial (no debe leerse)
        for (u32 k = 4; k < 7; ++k) c.a[k] = Special(g);
        break;
    case 13:  // coordenadas de rango amplio
        for (u32 k = 1; k < 4; ++k) c.a[k] = Wide(g);
        for (u32 k = 0; k < 12; ++k) c.a[13 + k] = Wide(g);
        for (u32 k = 0; k < 4; ++k) c.a[25 + k] = Wide(g);
        break;
    case 14:  // planos unitarios y posicion cerca del plano (distancia pequena)
        for (u32 k = 0; k < 12; ++k) c.a[13 + k] = g.Pk({0x3F800000u, 0xBF800000u, 0u, 0x80000000u, B(0.70710678f), B(-0.70710678f)});
        for (u32 k = 0; k < 4; ++k) c.a[25 + k] = B(g.Fl(-2.0f, 2.0f));
        for (u32 k = 1; k < 4; ++k) c.a[k] = B(g.Fl(-3.0f, 3.0f));
        break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 3: CTaskComplexPartner::ComputeTargetPoints (0x681FE0)
//   a[0] = el peaton tiene matriz, a[1..3] = su posicion en la activa (matriz + 0x30 o placement +4), a[4..6] = en la otra (basura); a[7] = el companero tiene matriz, a[8..10] = posicion, a[11..13] = la otra;
//   a[14] = factor (+0x3C), a[15..17] = el vector que deja el falso de CVector::Normalise (0x59C910), a[18] = tipo que devuelve GetTaskType (1204 = charla), a[19] = byte +0x5C inicial,
//   a[20..22] = salida 1 inicial (+0x40), a[23..25] = salida 2 inicial (+0x4C)
inline void NormDir(u32* out, u32 p0, u32 p1, u32 p2, u32 q0, u32 q1, u32 q2) {
    const double dx = double(F(q0)) - double(F(p0)), dy = double(F(q1)) - double(F(p1)), dz = double(F(q2)) - double(F(p2));
    const double len = std::sqrt(dx * dx + dy * dy + dz * dz);
    if (!(len > 1e-20)) { out[0] = B(1.0f); out[1] = 0; out[2] = 0; return; }
    out[0] = B(float(dx / len)); out[1] = B(float(dy / len)); out[2] = B(float(dz / len));
}
inline void Make3(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(60); c.a[7] = g.P(60);
    for (u32 k = 0; k < 3; ++k) { c.a[4 + k] = Coord(g); c.a[11 + k] = Coord(g); }
    // posiciones: el peaton en una base y el companero a una distancia aleatoria
    const float bx = g.Fl(-3000.0f, 3000.0f), by = g.Fl(-3000.0f, 3000.0f), bz = g.Fl(-100.0f, 100.0f);
    const float dists[]{0.0f, 0.001f, 0.3f, 0.7f, 1.0f, 1.4f, 2.0f, 3.0f, 5.0f, 12.0f, 30.0f, 300.0f};
    const float dist = dists[g.R(12)];
    const float ang = g.Fl(0.0f, 6.2831853f);
    const float dz = g.P(70) ? 0.0f : g.Fl(-3.0f, 3.0f);
    c.a[1] = B(bx); c.a[2] = B(by); c.a[3] = B(bz);
    c.a[8] = B(bx + dist * std::cos(ang)); c.a[9] = B(by + dist * std::sin(ang)); c.a[10] = B(bz + dz);
    c.a[14] = g.P(70) ? B(g.Fl(0.2f, 0.8f)) : g.Pk({0u, 0x3F800000u, 0xBF800000u, 0x40000000u, B(0.5f), B(0.07f), B(0.35f), B(0.65f), g.W()});
    NormDir(&c.a[15], c.a[1], c.a[2], c.a[3], c.a[8], c.a[9], c.a[10]);
    c.a[18] = g.P(10) ? 1204u : g.Pk({1202u, 1203u, 1000u, 0u, 0xFFFFFFFFu, 1205u, 0x4B4u + 0x10000u, 1204u + 0x100u, 100u});
    c.a[19] = g.P(60) ? 0u : g.Pk({1u, 2u, 0x80u, 0xFFu, g.W() & 0xFFu});
    for (u32 k = 0; k < 6; ++k) c.a[20 + k] = g.P(60) ? 0u : g.W();
    switch (f) {
    case 0: c.a[0] = 1; c.a[7] = 1; break;
    case 1: c.a[0] = 0; c.a[7] = 0; break;
    case 2: {   // companero lejos: converge en la primera pasada
        const float d = g.Fl(4.0f, 60.0f);
        c.a[8] = B(bx + d * std::cos(ang)); c.a[9] = B(by + d * std::sin(ang)); c.a[10] = B(bz + dz);
        c.a[14] = B(g.Fl(0.3f, 0.7f));
        NormDir(&c.a[15], c.a[1], c.a[2], c.a[3], c.a[8], c.a[9], c.a[10]);
        c.a[18] = 1000;
        break;
    }
    case 3: {   // companero cerca: hay que empujar (varias pasadas)
        const float d = g.Fl(0.5f, 2.5f);
        c.a[8] = B(bx + d * std::cos(ang)); c.a[9] = B(by + d * std::sin(ang)); c.a[10] = B(bz);
        c.a[14] = B(g.Fl(0.3f, 0.7f));
        NormDir(&c.a[15], c.a[1], c.a[2], c.a[3], c.a[8], c.a[9], c.a[10]);
        c.a[18] = 1000;
        break;
    }
    case 4: {   // companero muy cerca o en el mismo sitio: no converge
        const float d = g.P(40) ? 0.0f : g.Fl(0.0f, 0.6f);
        c.a[8] = B(bx + d * std::cos(ang)); c.a[9] = B(by + d * std::sin(ang)); c.a[10] = B(bz + (g.P(50) ? 0.0f : g.Fl(-1.0f, 1.0f)));
        c.a[14] = B(g.Fl(0.3f, 0.7f));
        NormDir(&c.a[15], c.a[1], c.a[2], c.a[3], c.a[8], c.a[9], c.a[10]);
        c.a[18] = 1000;
        break;
    }
    case 5: {   // direccion casi vertical: el empuje apenas cambia la distancia horizontal
        const float d = g.Fl(0.1f, 1.5f);
        c.a[8] = B(bx + d * std::cos(ang)); c.a[9] = B(by + d * std::sin(ang)); c.a[10] = B(bz + g.Fl(5.0f, 40.0f));
        c.a[14] = B(g.Fl(0.3f, 0.7f));
        NormDir(&c.a[15], c.a[1], c.a[2], c.a[3], c.a[8], c.a[9], c.a[10]);
        c.a[18] = 1000;
        break;
    }
    case 6:     // charla: no ajusta el punto
        c.a[18] = 1204; c.a[0] = g.P(50); c.a[7] = g.P(50);
        break;
    case 7: {   // frontera del umbral 0.7: el punto base cae a 0.7 +- ulps de uno de los dos
        const float d = g.Pk({10u, 20u, 50u, 100u}) * 1.0f;
        const float fac = 0.7f / d;
        c.a[8] = B(bx + d); c.a[9] = B(by); c.a[10] = B(bz);
        c.a[14] = NextUp(B(fac), int(g.R(9)) - 4);
        if (g.P(40)) c.a[14] = NextUp(B(1.0f - fac), int(g.R(9)) - 4);
        NormDir(&c.a[15], c.a[1], c.a[2], c.a[3], c.a[8], c.a[9], c.a[10]);
        c.a[18] = 1000;
        break;
    }
    case 8: {   // posiciones especiales (NaN, infinitos, enormes, diminutos)
        for (u32 k = 1; k < 4; ++k) if (g.P(35)) c.a[k] = Special(g);
        for (u32 k = 8; k < 11; ++k) if (g.P(35)) c.a[k] = Special(g);
        if (g.P(50)) NormDir(&c.a[15], c.a[1], c.a[2], c.a[3], c.a[8], c.a[9], c.a[10]);
        c.a[18] = g.P(80) ? 1000u : 1204u;
        break;
    }
    case 9:     // factor especial
        c.a[14] = Special(g);
        c.a[18] = g.P(85) ? 1000u : 1204u;
        break;
    case 10:    // direccion devuelta por Normalise arbitraria (no unitaria, enorme, diminuta, con NaN)
        for (u32 k = 0; k < 3; ++k) c.a[15 + k] = g.P(50) ? B(g.Fl(-3.0f, 3.0f)) : Special(g);
        c.a[18] = g.P(85) ? 1000u : 1204u;
        break;
    case 11:    // magnitudes enormes en las posiciones: los productos salen del rango de float en los registros
        for (u32 k = 1; k < 4; ++k) if (g.P(60)) c.a[k] = g.Pk({0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x7D000000u});
        for (u32 k = 8; k < 11; ++k) if (g.P(60)) c.a[k] = g.Pk({0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x7D000000u});
        c.a[14] = g.Pk({B(0.5f), B(8.0f), B(64.0f), B(0.25f), 0x40000000u, B(1000.0f)});
        c.a[18] = g.P(85) ? 1000u : 1204u;
        break;
    case 12:    // posiciones diminutas
        for (u32 k = 1; k < 4; ++k) c.a[k] = g.Pk({0x00000100u, 0x80000100u, 0x00800000u, 0x80800000u, 0x1E000000u, 0x9E000000u, 0u});
        for (u32 k = 8; k < 11; ++k) c.a[k] = g.Pk({0x00000100u, 0x80000100u, 0x00800000u, 0x80800000u, 0x1E000000u, 0x9E000000u, 0u});
        c.a[14] = g.Pk({B(0.5f), 0x00800000u, 0x1E000000u, B(1.0e-20f)});
        c.a[18] = g.P(85) ? 1000u : 1204u;
        break;
    case 13: {  // posicion en la otra estructura con basura especial; mezcla de matriz y placement
        for (u32 k = 4; k < 7; ++k) c.a[k] = Special(g);
        for (u32 k = 11; k < 14; ++k) c.a[k] = Special(g);
        const float d = g.Fl(1.0f, 30.0f);
        c.a[8] = B(bx + d * std::cos(ang)); c.a[9] = B(by + d * std::sin(ang)); c.a[10] = B(bz);
        NormDir(&c.a[15], c.a[1], c.a[2], c.a[3], c.a[8], c.a[9], c.a[10]);
        c.a[18] = 1000;
        break;
    }
    case 14: {  // la segunda pasada: cerca del companero pero lejos del peaton (empuje del segundo bloque)
        const float d = g.Fl(1.4f, 6.0f);
        c.a[8] = B(bx + d * std::cos(ang)); c.a[9] = B(by + d * std::sin(ang)); c.a[10] = B(bz);
        c.a[14] = B(g.Fl(0.75f, 1.0f));
        NormDir(&c.a[15], c.a[1], c.a[2], c.a[3], c.a[8], c.a[9], c.a[10]);
        c.a[18] = 1000;
        break;
    }
    default: break;
    }
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    switch (fn) {
    case 0: Make0(c, n); break;
    case 1: Make1(c, n); break;
    case 2: Make2(c, n); break;
    default: Make3(c, n); break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    return j.End();
}
}

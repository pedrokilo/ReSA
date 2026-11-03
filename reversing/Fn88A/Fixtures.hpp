#pragma once
// Fixtures del banco Fn88A (lote 88A): una funcion, 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCam::GetArrestCamCopSourceNormal                      0x515D80  thiscall (ECX sin usar; target, cop, dest, source en pila), `ret 10h`; devuelve AL   (hook CCam / GetArrestCamCopSourceNormal, tipo S)
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

namespace F88AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kCamWin = 0x40, kEntWin = 0x40, kMatWin = 0x40, kPedWin = 0x40, kDestWin = 0x40, kSrcWin = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kCamArena = Arena(kCamWin), kEntArena = Arena(kEntWin), kMatArena = Arena(kMatWin), kPedArena = Arena(kPedWin), kDestArena = Arena(kDestWin), kSrcArena = Arena(kSrcWin);
// Desplazamientos (respecto al objeto de cada arena) de los vectores de entrada y de salida
constexpr std::size_t kDestOff = 0x08, kSrcOff = 0x10;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 88008u); }
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

inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Un float "especial": cero, desnormal, infinito, NaN (un unico patron), enorme
inline u32 Special(u32 c, u32 k) {
    const u32 r = Rand(c, k) % 32;
    if (r < 10) return Pick(kTiny, c, k + 1);
    if (r < 16) return Pick(kInf, c, k + 1);
    if (r < 24) return 0xFFC00000u;
    if (r < 28) return Pick(kBig, c, k + 1);
    return B(float(int(Rand(c, k + 3) % 2001) - 1000) / 7.0f);
}
inline u32 NextUp(u32 bits, int steps) {   // desplaza un float normal `steps` unidades de ultimo lugar (signo respetado)
    const bool neg = (bits >> 31) != 0;
    u32 mag = bits & 0x7FFFFFFFu;
    mag = u32(i32(mag) + (neg ? -steps : steps));
    return (bits & 0x80000000u) | (mag & 0x7FFFFFFFu);
}

// ------------------------------------------------------------------------------------------------ el caso
// Colaboradores falsos: CrossProduct (0x59C730) escribe `cross`; Normalise (0x59C910) escribe nS (1a llamada: el lado), nD1 (2a: la direccion) y nD2 (3a: tras reescalar o aplanar la direccion).
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 targetNull{}, copNull{};                    // argumentos nulos
    u32 copMat{};                                   // el ped tiene matriz (+0x14)
    u32 copPos[3]{}, copAlt[3]{};                   // posicion en la ubicacion activa y en la otra (matriz +0x30 o compacta +4)
    u32 dest[3]{}, srcInit[3]{};                    // destino (entrada) y contenido previo de source (salida)
    u32 cross[3]{}, nS[3]{}, nD1[3]{}, nD2[3]{};    // lo que escriben los falsos
    u32 mutSite{}, mutVal{};
    u32 gate{};
};

// ------------------------------------------------------------------------------------------------ generador
inline void UnitXY(u32 c, u32 k, double z, u32* out) {   // vector unitario con la z indicada (|z| <= 1)
    const double ang = double(Rand(c, k) % 62832) / 10000.0;
    const double r = std::sqrt(std::max(0.0, 1.0 - z * z));
    out[0] = B(float(r * std::cos(ang))); out[1] = B(float(r * std::sin(ang))); out[2] = B(float(z));
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.ecx = Rand(n, 3); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    u32 kk = 100;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    auto Pk = [&](std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); };
    auto Fl = [&](float lo, float hi) { return lo + (hi - lo) * (float(R(100001)) / 100000.0f); };
    auto Coord = [&] { return B(Fl(-3000.0f, 3000.0f)); };
    const float steep = F(0xBF350481u);
    // --- ped y objetivo
    c.targetNull = P(f == 3 ? 50 : 2); c.copNull = P(f == 3 ? 50 : 2);
    if (f == 4) { c.targetNull = 0; c.copNull = 1; }
    c.copMat = P(60);
    for (u32 k = 0; k < 3; ++k) {
        c.copPos[k] = (f == 15 && P(25)) ? Special(n, nk()) : (f == 11 ? Pk({0u, 0x80000000u}) : Coord());
        c.copAlt[k] = Coord();
        c.srcInit[k] = Rand(n, nk());
    }
    // --- direcciones que escriben los falsos
    UnitXY(n, nk(), double(int(R(2001)) - 1000) / 1000.0, c.nS);
    UnitXY(n, nk(), double(int(R(2001)) - 1000) / 1000.0, c.nD2);
    UnitXY(n, nk(), double(int(R(2001)) - 1000) / 1000.0, c.nD1);
    for (u32 k = 0; k < 3; ++k) c.cross[k] = Rand(n, nk());
    // nD1.z: A (< -0.7071), B (> 0), medio ([-0.7071, 0]) o especial
    const u32 zkind = R(100);
    auto SetD1 = [&](u32 kind) {
        if (kind == 0) UnitXY(n, nk(), -(0.7072 + 0.2928 * double(R(10001)) / 10000.0), c.nD1);
        else if (kind == 1) UnitXY(n, nk(), double(R(10001)) / 10000.0 + 1e-7, c.nD1);
        else UnitXY(n, nk(), -0.7071 * double(R(10001)) / 10000.0, c.nD1);
    };
    if (f == 15) {
        for (u32 k = 0; k < 3; ++k) {
            if (P(35)) c.nD1[k] = Special(n, nk());
            if (P(25)) c.nD2[k] = Special(n, nk());
            if (P(20)) c.nS[k] = Special(n, nk());
            if (P(20)) c.cross[k] = Special(n, nk());
        }
        if (P(40)) SetD1(R(3));
    } else if (f == 14) {
        // umbrales: z de nD1 alrededor de -0.7071f exacto, 0 y -0
        switch (R(8)) {
        case 0: c.nD1[2] = 0xBF350481u; break;
        case 1: c.nD1[2] = NextUp(0xBF350481u, 1); break;
        case 2: c.nD1[2] = NextUp(0xBF350481u, -1); break;
        case 3: c.nD1[2] = 0u; break;
        case 4: c.nD1[2] = 0x80000000u; break;
        case 5: c.nD1[2] = 1u; break;
        case 6: c.nD1[2] = 0x80000001u; break;
        default: c.nD1[2] = NextUp(0xBF350481u, int(R(5)) - 2); break;
        }
    } else if (f == 6) {
        // A con norma xy nula, diminuta o gigante al invertir
        c.nD1[2] = Pk({0xBF800000u, 0xBF350482u, NextUp(0xBF350481u, 3)});
        const u32 m = R(4);
        for (u32 k = 0; k < 2; ++k)
            c.nD1[k] = m == 0 ? Pk({0u, 0x80000000u}) : (m == 1 ? B(float(double(R(1000) + 1) * 1e-32)) : (m == 2 ? Pk({1u, 0x80000001u, 0x00400000u, 0x007FFFFFu}) : B(float(R(100001)) * 1e-5f)));
        if (m == 3 && P(50)) c.nD1[1] = 0u;
    } else if (f == 7) {
        SetD1(0);
    } else if (f == 8) {
        SetD1(1);
    } else if (f == 9) {
        SetD1(2);
        if (P(30)) c.nD1[2] = Pk({0u, 0x80000000u, 0xBF350481u, NextUp(0xBF350481u, 1), 0xFFC00000u});
    } else {
        SetD1(zkind < 35 ? 0u : (zkind < 70 ? 1u : 2u));
    }
    // --- direccion final (la de la normalizacion que acaba mandando) y geometria: dest = C'' + P
    const bool viaD2 = F(c.nD1[2]) < steep || F(c.nD1[2]) > 0.0f;
    const u32* fin = viaD2 ? c.nD2 : c.nD1;
    const double finX = double(F(fin[0])), finY = double(F(fin[1])), finZ = double(F(fin[2]));
    const double cx = double(F(c.copPos[0])), cy = double(F(c.copPos[1])), cz = double(F(c.copPos[2]));
    const double c2x = cx + 3.0 * double(F(c.nS[0])) - 5.0 * finX, c2y = cy + 3.0 * double(F(c.nS[1])) - 5.0 * finY, c2z = cz + 3.0 * double(F(c.nS[2])) - 5.0 * finZ;
    double radius;   // |dest - C''|
    switch (f) {
    case 1: case 5: radius = double(R(7000)) / 1000.0; break;
    case 2: radius = 8.5 + double(R(20000)) / 100.0; break;
    case 10: radius = 8.0 * (1.0 + double(int(R(2001)) - 1000) / 400000.0); break;
    case 14: radius = P(50) ? 8.0 * (1.0 + double(int(R(401)) - 200) / 4000000.0) : double(R(4000)) / 1000.0; break;
    case 13: radius = P(50) ? 0.0 : double(R(9000)) / 1000.0; break;
    case 11: radius = 0.0; break;
    default: radius = P(55) ? double(R(8200)) / 1000.0 : 8.0 + double(R(100000)) / 100.0; break;
    }
    double px[3];
    {
        u32 d[3];
        UnitXY(n, nk(), double(int(R(2001)) - 1000) / 1000.0, d);
        for (u32 k = 0; k < 3; ++k) px[k] = double(F(d[k])) * radius;
    }
    c.dest[0] = B(float(c2x + px[0])); c.dest[1] = B(float(c2y + px[1])); c.dest[2] = B(float(c2z + px[2]));
    if (f == 11) {
        for (u32 k = 0; k < 3; ++k) {
            c.dest[k] = Pk({0u, 0x80000000u}); c.nS[k] = Pk({0u, 0x80000000u}); c.nD1[k] = Pk({0u, 0x80000000u}); c.nD2[k] = Pk({0u, 0x80000000u});
        }
    }
    if (f == 13) {
        if (P(30)) for (u32 k = 0; k < 3; ++k) c.dest[k] = B(Fl(-1.0e30f, 1.0e30f));
        if (P(15)) for (u32 k = 0; k < 3; ++k) c.nD2[k] = Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x00800000u, 0x80000001u, 0x3F800000u});
    }
    if (f == 15) { for (u32 k = 0; k < 3; ++k) if (P(25)) c.dest[k] = Special(n, nk()); }
    // --- mutaciones: una escritura en memoria dentro de un falso (dest, que el EXE relee, o el ped, que ya copio al principio)
    if (f == 12) {
        c.mutSite = 1 + R(5);
        c.mutVal = P(30) ? Special(n, nk()) : Coord();
        c.targetNull = 0; c.copNull = 0;
    }
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    auto V = [](const u32* p, std::size_t k) { return std::vector<u32>(p, p + k); };
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("tnull", c.targetNull).U("cnull", c.copNull).U("cmat", c.copMat).A("cpos", V(c.copPos, 3)).A("calt", V(c.copAlt, 3)).A("dest", V(c.dest, 3)).A("srcinit", V(c.srcInit, 3));
    j.A("cross", V(c.cross, 3)).A("ns", V(c.nS, 3)).A("nd1", V(c.nD1, 3)).A("nd2", V(c.nD2, 3));
    j.U("mutsite", c.mutSite).U("mutval", c.mutVal).U("gate", c.gate);
    return j.End();
}
}

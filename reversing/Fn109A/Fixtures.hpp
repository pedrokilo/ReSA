#pragma once
// Fixtures del banco Fn109A (lote 109A): 16 funciones x 16384 casos (262144 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0  CSetPiece::GetSpawnCoord1                 0x4997A0  thiscall (ECX = pieza de 0x20), puntero de salida CVector2D en la pila, `ret 4`; devuelve el puntero en EAX  (hook S)
//   1  CSetPiece::GetSpawnCoord2                 0x4997E0  igual (par de int16 en +0x10)
//   2  CSetPiece::GetTargetCoord1                0x499820  igual (par de int16 en +0x14)
//   3  CSetPiece::GetTargetCoord2                0x499860  igual (par de int16 en +0x18)
//   4  CCam::ClipBeta                            0x509C50  thiscall (ECX = camara), sin argumentos, plain `ret`; ajusta el float +0xBC al rango [-pi, pi]
//   5  CCam::GetWeaponFirstPersonOn              0x509DC0  thiscall, devuelve AL (byte del arma activa del ped objetivo +0x21C)
//   6  IsLampPost                                0x509A30  cdecl (indice de modelo, 32 bits con signo), AL; compara con 14 palabras SIN signo de 0x8CD4FC..0x8CD620
//   7  vectores distintos                        0x509760  cdecl (const CVector*, const CVector*), AL
//   8  vectores iguales                          0x4DC670  cdecl (const CVector*, const CVector*), AL
//   9  CEventDamage::HasKilledPed                0x4ABCA0  thiscall, AL
//  10  CVehicleScanner::GetClosestVehicleInRange 0x5FFEE0 thiscall, EAX = dword +0x4C
//  11  CPedScanner::GetClosestPedInRange         0x5FFF20 thiscall, EAX = dword +0x4C
//  12  CPointRoute::operator new                 0x41B7C0 cdecl (size), EAX = objeto del pool 0xB744B0 o 0
//  13  CPointRoute::operator delete              0x41B7D0 cdecl (puntero), void
//  14  CPedClothesDesc::SetTextureAndModel(hash) 0x5A8050 thiscall (textura, modelo, parte en pila), `ret 0Ch`
//  15  CPedClothesDesc::SetTextureAndModel(name) 0x5A8080 thiscall (nombre textura, nombre modelo, parte), `ret 0Ch`
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

namespace F109AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 16;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x760;     // todas las arenas tienen la misma ventana
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 109001u); }
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
inline constexpr std::array<u32, 8> kNan{0xFFC00000u, 0x7FC00000u, 0x7FC00001u, 0xFFC12345u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFBFFFFFu};
// Un float "especial": cero, desnormal, infinito, NaN, enorme
inline u32 Special(Gen& g) {
    const u32 r = g.R(32);
    if (r < 10) return kTiny[g.R(12)];
    if (r < 16) return kInf[g.R(4)];
    if (r < 24) return kNan[g.R(8)];
    if (r < 28) return kBig[g.R(6)];
    return B(float(int(g.R(2001)) - 1000) / 7.0f);
}
inline u32 NextUp(u32 bits, int steps) {   // desplaza un float normal `steps` unidades de ultimo lugar (signo respetado)
    const bool neg = (bits >> 31) != 0;
    u32 mag = bits & 0x7FFFFFFFu;
    mag = u32(i32(mag) + (neg ? -steps : steps));
    return (bits & 0x80000000u) | (mag & 0x7FFFFFFFu);
}
inline u32 Wide(Gen& g) {
    const double e = double(int(g.R(61)) - 30);
    const double m = 1.0 + double(g.R(1000000)) / 1000000.0;
    return B(float((g.P(50) ? -1.0 : 1.0) * m * std::pow(10.0, e)));
}

// ------------------------------------------------------------------------------------------------ funciones 0..3: accesores de CSetPiece
//   a[0] = x (int16, en los 16 bits bajos), a[1] = y; el resto de la pieza y la salida son basura
inline u32 S16(Gen& g, u32 f) {
    switch (f) {
    case 0: return g.W() & 0xFFFFu;
    case 1: return 0u;
    case 2: return g.Pk({0x7FFFu, 0x8000u, 0xFFFFu, 0x0001u, 0x8001u, 0x7FFEu});
    case 3: return g.Pk({1u, 0xFFFFu, 2u, 0xFFFEu, 3u, 0xFFFDu, 4u, 0xFFFCu});
    case 4: return g.R(64);
    case 5: return (0x10000u - g.R(64)) & 0xFFFFu;
    case 6: return g.Pk({0x4000u, 0xC000u, 0x2000u, 0xE000u, 0x1000u, 0xF000u});
    case 7: return g.Pk({0x00FFu, 0xFF00u, 0x0100u, 0xFFFFu, 0x007Fu, 0x0080u});
    default: return g.R(8) < 2 ? g.Pk({0u, 0x7FFFu, 0x8000u, 0xFFFFu}) : (g.W() & 0xFFFFu);
    }
}
inline void MakeSetPiece(C& c, unsigned n) {
    Gen g(n);
    c.a[0] = S16(g, c.family);
    c.a[1] = S16(g, (c.family * 7 + 3) % 16);
}

// ------------------------------------------------------------------------------------------------ funcion 4: CCam::ClipBeta (a[0] = beta)
inline void Make4(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    const float pi = 3.14159274f;
    u32 beta;
    switch (f) {
    case 0: beta = B(g.Fl(-3.0f, 3.0f)); break;                                                       // dentro del rango
    case 1: beta = B(g.Fl(3.1415927f, 12.0f)); break;                                                 // por encima
    case 2: beta = B(g.Fl(-12.0f, -3.1415927f)); break;                                               // por debajo
    case 3: beta = NextUp(B(pi), int(g.R(17)) - 8); break;                                            // frontera superior con ulps
    case 4: beta = NextUp(B(-pi), int(g.R(17)) - 8); break;                                           // frontera inferior con ulps
    case 5: beta = g.Pk({B(pi), B(-pi), B(2.0f * pi), B(-2.0f * pi), B(3.0f * pi), B(-3.0f * pi), B(6.2831855f), B(-6.2831855f), B(pi + 1e-7f)}); break;
    case 6: beta = Wide(g); break;                                                                    // magnitudes de 1e-30 a 1e30
    case 7: beta = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x4B800000u, 0xCB800000u, 0x4B000000u, 0xCB000000u}); break;   // enormes
    case 8: beta = kNan[g.R(8)]; break;                                                               // NaN
    case 9: beta = kInf[g.R(4)]; break;                                                               // infinitos
    case 10: beta = kTiny[g.R(12)]; break;                                                            // ceros y desnormales
    case 11: beta = B(g.Fl(7.0f, 50.0f) * (g.P(50) ? -1.0f : 1.0f)); break;                           // varias vueltas
    case 12: beta = B(float(int(g.R(2001)) - 1000) / 100.0f); break;                                  // valores redondos
    case 13: beta = B(pi - (float(g.R(1000)) * 1e-7f)); break;                                        // justo dentro
    case 14: beta = NextUp(B(pi), 1 + int(g.R(3))); break;                                            // justo fuera (+)
    default: beta = g.P(50) ? Special(g) : B(g.Fl(-100.0f, 100.0f)); break;
    }
    c.a[0] = beta;
}

// ------------------------------------------------------------------------------------------------ funcion 5: CCam::GetWeaponFirstPersonOn
//   a[0] = objetivo (0 el ped de la arena, 1 nulo), a[1] = byte +0x36 de la entidad (tipo en los 3 bits bajos), a[2] = ranura activa (byte +0x718), a[3] = byte de la bandera del arma de la ranura
//   (con signo: ranuras -8..14), a[4] = bytes vecinos (relleno)
inline void Make5(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(8) ? 1u : 0u;
    c.a[1] = (g.W() & 0xF8u) | 3u;
    const int slot = int(g.R(23)) - 8;
    c.a[2] = u32(slot) & 0xFFu;
    c.a[3] = g.P(50) ? 1u : (g.P(50) ? 0u : (g.W() & 0xFFu));
    switch (f) {
    case 0: c.a[0] = 0; c.a[1] = 3; c.a[3] = 1; break;                                               // ped con la bandera a 1
    case 1: c.a[0] = 0; c.a[1] = 3; c.a[3] = 0; break;
    case 2: c.a[0] = 1; break;                                                                        // objetivo nulo
    case 3: c.a[0] = 0; c.a[1] = (g.W() & 0xF8u) | g.Pk({0u, 1u, 2u, 4u, 5u, 6u, 7u}); c.a[3] = 1; break;   // no es un ped
    case 4: c.a[0] = 0; c.a[1] = 3; c.a[2] = u32(-1) & 0xFFu; c.a[3] = g.P(50) ? 1u : 0u; break;     // ranura -1 (con signo)
    case 5: c.a[0] = 0; c.a[1] = 3; c.a[2] = g.Pk({0xF8u, 0xFFu, 0xFEu, 0xFCu, 0xF9u}); c.a[3] = g.Pk({1u, 0u, 2u}); break;   // negativas
    case 6: c.a[0] = 0; c.a[1] = 3; c.a[2] = g.R(15); c.a[3] = g.Pk({1u, 2u, 0x7Fu, 0x80u, 0xFFu}); break;                // bandera no booleana
    case 7: c.a[0] = 0; c.a[1] = 3 | 0x08u; c.a[3] = 1; break;                                       // tipo con bits altos
    case 8: c.a[0] = 0; c.a[1] = g.Pk({3u, 0xFBu, 0x03u, 0x73u, 0xF3u}); c.a[3] = g.R(2); break;
    case 9: c.a[0] = 0; c.a[1] = 3; c.a[2] = g.R(15); c.a[3] = 1; break;
    default: break;
    }
    c.a[4] = g.W();
}

// ------------------------------------------------------------------------------------------------ funcion 6: IsLampPost
//   a[0..13] = las 14 palabras de los indices de modelo (en el orden de lectura del EXE), a[14] = argumento (32 bits), a[15] = modo del argumento
inline void Make6(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    for (u32 k = 0; k < 14; ++k) c.a[k] = g.P(15) ? g.Pk({0xFFFFu, 0u, 0x8000u, 0x7FFFu, 0x9000u}) : (g.W() & 0xFFFFu);
    const u32 pick = g.R(14);
    const u32 w = c.a[pick];
    c.a[15] = f;
    switch (f) {
    case 0: c.a[14] = w; break;                                                                       // coincide (extension a cero)
    case 1: c.a[14] = u32(std::int32_t(std::int16_t(w))); break;                                      // extension con signo (solo coincide con palabras < 0x8000)
    case 2: c.a[14] = g.W(); break;
    case 3: c.a[14] = w ^ (1u << g.R(32)); break;                                                     // un bit distinto
    case 4: c.a[14] = 0xFFFFFFFFu; for (u32 k = 0; k < 14; ++k) if (g.P(30)) c.a[k] = 0xFFFFu; break;   // -1 con palabras sin cargar (0xFFFF)
    case 5: c.a[14] = 0xFFFFu; for (u32 k = 0; k < 14; ++k) if (g.P(30)) c.a[k] = 0xFFFFu; break;
    case 6: c.a[14] = 0x8000u + g.R(0x8000u); c.a[pick] = g.P(50) ? c.a[14] : (c.a[14] ^ 0x10000u); break;
    case 7: c.a[14] = w + 0x10000u * (1 + g.R(100)); break;                                           // bits altos distintos
    case 8: c.a[14] = 0u; for (u32 k = 0; k < 14; ++k) if (g.P(20)) c.a[k] = 0u; break;
    case 9: { const u32 v = 400u + g.R(600u); for (u32 k = 0; k < 14; ++k) c.a[k] = g.P(10) ? v : (g.W() & 0xFFFFu); c.a[14] = v; break; }   // varias iguales
    case 10: c.a[14] = c.a[13]; break;                                                                // la ultima
    case 11: c.a[14] = c.a[0]; break;                                                                 // la primera
    case 12: c.a[14] = u32(std::int32_t(-(std::int32_t)g.R(70000))); break;
    case 13: c.a[14] = 0x00010000u | w; break;
    default: c.a[14] = g.P(50) ? w : (g.W() & 0xFFFFu); break;
    }
    // las palabras llevan solo 16 bits (la memoria se escribe como palabra)
    for (u32 k = 0; k < 14; ++k) c.a[k] &= 0xFFFFu;
}

// ------------------------------------------------------------------------------------------------ funciones 7 y 8: igualdad / desigualdad exacta de dos vectores
//   a[0..2] = vector A, a[3..5] = vector B
inline void Make78(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    for (u32 k = 0; k < 3; ++k) c.a[k] = g.P(80) ? B(g.Fl(-1000.0f, 1000.0f)) : Special(g);
    for (u32 k = 0; k < 3; ++k) c.a[3 + k] = c.a[k];   // iguales de partida
    const u32 one = g.R(3);
    switch (f) {
    case 0: break;                                                                                    // todo igual
    case 1: c.a[3 + one] = B(g.Fl(-1000.0f, 1000.0f)); break;                                         // una componente distinta
    case 2: c.a[3 + one] = NextUp(c.a[one], g.P(50) ? 1 : -1); break;                                 // un ulp de diferencia
    case 3: for (u32 k = 0; k < 3; ++k) { c.a[k] = g.Pk({0u, 0x80000000u}); c.a[3 + k] = g.Pk({0u, 0x80000000u}); } break;     // ceros con signo
    case 4: c.a[one] = kNan[g.R(8)]; c.a[3 + one] = c.a[one]; break;                                  // NaN en ambos (nunca igual)
    case 5: c.a[3 + one] = kNan[g.R(8)]; break;                                                       // NaN solo en B
    case 6: c.a[one] = kNan[g.R(8)]; break;                                                           // NaN solo en A
    case 7: for (u32 k = 0; k < 3; ++k) { c.a[k] = kInf[g.R(4)]; c.a[3 + k] = g.P(50) ? c.a[k] : kInf[g.R(4)]; } break;
    case 8: for (u32 k = 0; k < 3; ++k) { c.a[k] = kTiny[g.R(12)]; c.a[3 + k] = g.P(60) ? c.a[k] : kTiny[g.R(12)]; } break;     // desnormales y ceros
    case 9: c.a[3] = c.a[0] ^ 0x80000000u; break;                                                     // signo opuesto en x
    case 10: c.a[3 + 2] = c.a[2] + 1u; break;                                                         // solo z distinta por un ulp
    case 11: c.a[3 + 1] = c.a[1] ^ 0x80000000u; break;
    case 12: for (u32 k = 0; k < 3; ++k) c.a[3 + k] = g.P(50) ? c.a[k] : Special(g); break;
    case 13: for (u32 k = 0; k < 3; ++k) { c.a[k] = Wide(g); c.a[3 + k] = g.P(70) ? c.a[k] : Wide(g); } break;
    case 14: c.a[0] = c.a[3] = 0u; c.a[1] = c.a[4] = 0x80000000u; c.a[2] = c.a[5] = g.Pk({0u, 0x80000000u}); break;   // ceros de ambos signos iguales entre si
    default: for (u32 k = 0; k < 3; ++k) if (g.P(35)) c.a[3 + k] = g.W(); break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 9: CEventDamage::HasKilledPed (a[0] = byte +0x40, a[1] = byte +0x0C)
inline void Make9(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    auto nz = [&] { return g.P(50) ? 1u : 1u + g.R(255); };
    switch (f) {
    case 0: c.a[0] = nz(); c.a[1] = nz(); break;
    case 1: c.a[0] = 0; c.a[1] = nz(); break;
    case 2: c.a[0] = nz(); c.a[1] = 0; break;
    case 3: c.a[0] = 0; c.a[1] = 0; break;
    case 4: c.a[0] = 1; c.a[1] = 1; break;
    case 5: c.a[0] = 0x80; c.a[1] = 0x80; break;
    case 6: c.a[0] = 0xFF; c.a[1] = 0xFF; break;
    case 7: c.a[0] = 0x100u >> (1 + g.R(8)); c.a[1] = 1; break;
    default: c.a[0] = g.P(50) ? 0u : g.R(256); c.a[1] = g.P(50) ? 0u : g.R(256); break;
    }
}

// ------------------------------------------------------------------------------------------------ funciones 10 y 11: escaneres (a[0] = dword +0x4C)
inline void Make1011(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    switch (f) {
    case 0: c.a[0] = 0; break;
    case 1: c.a[0] = 0xFFFFFFFFu; break;
    case 2: c.a[0] = 0x00BADC00u; break;
    case 3: c.a[0] = 0x80000000u; break;
    case 4: c.a[0] = 1u << g.R(32); break;
    case 5: c.a[0] = ~(1u << g.R(32)); break;
    default: c.a[0] = g.W(); break;
    }
}

// ------------------------------------------------------------------------------------------------ funciones 12 y 13: operator new / delete de CPointRoute sobre el pool 0xB744B0
//   a[0] = numero de ranuras N (1..16), a[1] = m_nFirstFree (con signo, -1..N-1), a[2..17] = bytes de estado (bit 7 = vacio, bits 0..6 = referencia), a[18] = argumento size / indice de la ranura a borrar,
//   a[19] = desplazamiento dentro del objeto (delete), a[20] = bytes +0x10/+0x11 del pool (basura)
inline void Make1213(C& c, unsigned n) {
    const u32 f = c.family;
    const bool del = c.fn == 13;
    Gen g(n);
    const u32 N = 1 + g.R(16);
    c.a[0] = N;
    c.a[1] = u32(std::int32_t(g.R(N + 1)) - 1);                       // -1..N-1
    for (u32 k = 0; k < 16; ++k) c.a[2 + k] = g.P(50) ? 0x80u | g.R(128) : g.R(128);
    c.a[18] = del ? g.R(N) : g.W();
    c.a[19] = del ? g.R(100) : 0u;
    c.a[20] = g.W() & 0xFFFFu;
    switch (f) {
    case 0: for (u32 k = 0; k < 16; ++k) c.a[2 + k] = 0x80u | g.R(128); break;                           // todo vacio
    case 1: for (u32 k = 0; k < 16; ++k) c.a[2 + k] = g.R(128); break;                                    // todo ocupado
    case 2: { for (u32 k = 0; k < 16; ++k) c.a[2 + k] = g.R(128); c.a[2 + g.R(N)] |= 0x80u; break; }      // una sola vacia
    case 3: { for (u32 k = 0; k < 16; ++k) c.a[2 + k] = g.R(128); c.a[1] = N - 1; c.a[2] |= 0x80u; break; }   // hay que dar la vuelta
    case 4: c.a[1] = u32(-1); break;
    case 5: c.a[0] = 1; c.a[1] = g.P(50) ? 0u : u32(-1); break;
    case 6: for (u32 k = 0; k < 16; ++k) c.a[2 + k] = 0x80u | 0x7Fu; break;                               // referencias a punto de dar la vuelta
    case 7: for (u32 k = 0; k < 16; ++k) c.a[2 + k] = g.P(50) ? 0xFFu : 0x7Fu; break;
    case 8: for (u32 k = 0; k < 16; ++k) c.a[2 + k] = g.P(50) ? 0x80u : 0x00u; break;
    case 9: c.a[0] = 16; break;
    default: break;
    }
    if (c.a[1] != u32(-1) && c.a[1] >= c.a[0]) c.a[1] = c.a[0] - 1;
    if (del) {
        if (c.a[18] >= c.a[0]) c.a[18] %= c.a[0];
        const u32 idx = c.a[18];
        // la ranura a borrar debe estar ocupada (el cuerpo de la DLL comprueba con assert que no se borra dos veces)
        c.a[2 + idx] &= 0x7Fu;
        if (f == 10) c.a[1] = idx;                                                                      // cursor == indice
        if (f == 11) c.a[1] = idx + 1 <= c.a[0] - 1 ? idx + 1 : idx;                                    // cursor justo por encima
        if (f == 12) c.a[1] = idx >= 1 ? idx - 1 : u32(-1);                                             // cursor por debajo
        if (f == 13) c.a[19] = 99;
        if (f == 14) c.a[19] = 0;
    }
}

// ------------------------------------------------------------------------------------------------ funciones 14 y 15: CPedClothesDesc::SetTextureAndModel
//   14: a[0] = textura, a[1] = modelo, a[2] = parte (con signo), a[3] = basura de las claves vecinas (semilla)
//   15: a[0] = modo del nombre de textura (0 normal, 1 nulo, 2 vacio), a[1] = modo del nombre del modelo, a[2] = parte (con signo), a[3] = longitud textura, a[4] = longitud modelo, a[5], a[6] = semillas
inline u32 Part(Gen& g, u32 f) {
    if (f < 10) return f == 9 ? 13u + g.R(5) : (f < 4 ? f : (f < 8 ? 4u + g.R(9) : 0u));
    switch (f) {
    case 10: return g.R(18);
    case 11: return u32(std::int32_t(g.R(8)) - 8);                       // negativas
    case 12: return 18u + g.R(13);                                       // por encima del limite (hasta 30)
    case 13: return g.Pk({17u, 18u, 19u, 0u, 0xFFFFFFFFu, 13u, 12u, 4u});
    default: return u32(std::int32_t(g.R(39)) - 8);                       // -8..30
    }
}
inline void Make14(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(10) ? g.Pk({0u, 0xFFFFFFFFu, 0x80000000u}) : g.W();
    c.a[1] = g.P(10) ? g.Pk({0u, 0xFFFFFFFFu, 0x80000000u}) : g.W();
    c.a[2] = Part(g, f);
    c.a[3] = g.W();
}
inline void Make15(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(15) ? (g.P(60) ? 1u : 2u) : 0u;
    c.a[1] = g.P(15) ? (g.P(60) ? 1u : 2u) : 0u;
    c.a[2] = Part(g, f);
    c.a[3] = 1 + g.R(30); c.a[4] = 1 + g.R(30);
    c.a[5] = g.W(); c.a[6] = g.W();
    if (f == 3 || f == 7) { c.a[0] = 1; }
    if (f == 5) { c.a[1] = 1; }
    if (f == 9) { c.a[0] = 1; c.a[1] = 1; }
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    switch (fn) {
    case 0: case 1: case 2: case 3: MakeSetPiece(c, n); break;
    case 4: Make4(c, n); break;
    case 5: Make5(c, n); break;
    case 6: Make6(c, n); break;
    case 7: case 8: Make78(c, n); break;
    case 9: Make9(c, n); break;
    case 10: case 11: Make1011(c, n); break;
    case 12: case 13: Make1213(c, n); break;
    case 14: Make14(c, n); break;
    default: Make15(c, n); break;
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

#pragma once
// Fixtures del banco Fn111A (lote 111A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 GxtCharStrcat            0x718630  cdecl (dst, src); devuelve dst en EAX                                           (hook S, categoria Common)
//   1 GxtCharStrcpy            0x718660  cdecl (dst, src); devuelve dst en EAX                                           (hook S)
//   2 GxtCharStrlen            0x718690  cdecl (cadena); devuelve la longitud en EAX                                     (hook S)
//   3 TextCopy                 0x69F910  cdecl (dst, src); void                                                          (hook S)
//   4 GxtCharToSafeAscii       0x69F8C0  cdecl (cadena); devuelve el buffer estatico 0xC1BED0 (257 bytes) en EAX         (hook S, C_PcSave)
//   5 CTaskCategories::ComputeTaskCategory 0x698680 cdecl (tarea); devuelve AL (tres consultas de GetTaskType)         (hook S)
//   6 CTaskCategories::IsKillPedTask       0x6985E0 cdecl (tarea, bool&, bool&); void                                  (hook S)
//   7 CTaskCategories::IsFollowPedTask     0x698610 cdecl (tarea, bool&, bool&); void                                  (hook S)
//   8 CTaskGoToVehicleAndLean::DoTidyUp    0x660F60 thiscall (ECX = tarea; ped en pila), `ret 4`; void                 (hook S)
//   9 IsDoorInUse (CTaskComplexLeaveCar)   0x646CF0 cdecl (vehiculo, puerta, bool&, bool&); void                       (hook S)
//  10 CTaskComplexBeInCouple::GetSideOfPartner 0x683840 thiscall (ECX = tarea; ped en pila), `ret 4`; devuelve 1 o 2 en EAX (hook S)
//  11 CWeaponInfo::GetRLoadA   0x73A280  thiscall (ECX = info de arma; bool en pila), `ret 4`; devuelve AX              (hook S)
//  12 CWeaponInfo::GetRLoadB   0x73A2B0  idem
//
// Todos los floats viajan como patrones de 32 bits sin signo. Familia = n % 16 (1024 casos por familia).
// Las cadenas se generan por StrByte (la misma formula esta en fixture_oracle.py).
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

namespace F111AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 13;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kTaskWin = 0x70, kPedWin = 0x5A0, kMatWin = 0x40, kVehWin = 0x500, kOutWin = 0x20, kSubWin = 0x20, kStrWin = 0x300, kWiWin = 0x70;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 111001u); }
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

// Byte de una cadena generada: modo 0 = 1..255, 1 = ASCII imprimible, 2 = caracteres "especiales" (separadores, 0x7F, >= 0x80), 3 = 0x80..0xFF. Nunca cero.
inline u8 StrByte(u32 seed, u32 i, u32 mode) {
    const u32 r = Mix(seed * 0x9E3779B1u + i * 0x85EBCA6Bu + 111u);
    static constexpr u8 sp[16] = {0x2F, 0x2A, 0x3F, 0x7E, 0x7F, 0x80, 0xFF, 0x20, 0x41, 0x61, 0x01, 0x7D, 0x81, 0x2E, 0x5C, 0x7E};
    switch (mode) {
    case 0: return u8(1 + r % 255);
    case 1: return u8(0x20 + r % 0x5F);
    case 2: return sp[r % 16];
    default: return u8(0x80 + r % 128);
    }
}

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
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    std::array<u32, 24> a{};
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
inline u32 Wide(Gen& g) {
    const double e = double(int(g.R(61)) - 30);
    const double m = 1.0 + double(g.R(1000000)) / 1000000.0;
    return B(float((g.P(50) ? -1.0 : 1.0) * m * std::pow(10.0, e)));
}
inline u32 Coord(Gen& g, float range = 3000.0f) { return B(g.Fl(-range, range)); }

// ------------------------------------------------------------------------------------------------ funciones 0..3: cadenas GXT (Strcat, Strcpy, Strlen, TextCopy)
//   a[0] = longitud de la cadena destino, a[1] = longitud de la cadena origen, a[2] / a[3] = desplazamiento (0..3) del destino / origen dentro de su arena, a[4] / a[5] = semilla de cada cadena,
//   a[6] / a[7] = modo de bytes (StrByte), a[8] / a[9] = semilla del relleno (bytes distintos de cero, StrByte modo 0) de cada arena fuera de la cadena.
//   En la funcion 2 (Strlen) solo cuenta la cadena origen.
inline u32 StrLen(Gen& g, u32 family) {
    switch (family) {
    case 0: return 0;
    case 3: return 1;
    case 4: return 2 + g.R(14);
    case 5: return 16 + g.R(49);
    case 6: return 65 + g.R(191);
    case 7: return 250 + g.R(6);
    default: { const u32 k = g.R(10); return k < 2 ? 0u : (k < 4 ? 1u : (k < 7 ? 2 + g.R(30) : (k < 9 ? 32 + g.R(100) : 100 + g.R(156)))); }
    }
}
inline void MakeStr(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = StrLen(g, f); c.a[1] = StrLen(g, f);
    c.a[2] = 0; c.a[3] = 0;
    c.a[4] = g.W(); c.a[5] = g.W(); c.a[6] = g.R(4); c.a[7] = g.R(4); c.a[8] = g.W(); c.a[9] = g.W();
    switch (f) {
    case 1: c.a[0] = 0; c.a[1] = 1 + g.R(60); break;
    case 2: c.a[1] = 0; c.a[0] = 1 + g.R(60); break;
    case 8: c.a[6] = 1; c.a[7] = 1; break;
    case 9: c.a[6] = 2; c.a[7] = 2; break;
    case 10: c.a[6] = 3; c.a[7] = 3; break;
    case 11: c.a[2] = 1 + g.R(3); c.a[3] = 1 + g.R(3); c.a[0] = g.R(40); c.a[1] = g.R(40); break;
    case 12: c.a[0] = g.R(256); c.a[1] = 0; break;
    case 13: c.a[0] = 0; c.a[1] = g.R(256); break;
    case 14: c.a[0] = 255 - g.R(3); c.a[1] = 255 - g.R(3); break;
    case 15: c.a[2] = g.R(4); c.a[3] = g.R(4); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 4: GxtCharToSafeAscii (0x69F8C0)
//   a[0] = longitud de la cadena (0..300), a[1] = desplazamiento (0..3), a[2] = semilla, a[3] = modo, a[4] = 1: el puntero es nulo, a[5] = semilla del relleno despues del terminador
inline void Make4(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(15) ? g.Pk({0u, 1u, 255u, 256u, 257u, 258u, 300u, 254u}) : g.R(301);
    c.a[1] = g.R(4); c.a[2] = g.W(); c.a[3] = g.R(4); c.a[4] = 0; c.a[5] = g.W();
    switch (f) {
    case 0: c.a[0] = 0; break;
    case 1: c.a[4] = 1; break;
    case 2: c.a[0] = 255 + g.R(3); c.a[3] = 2; break;
    case 3: c.a[0] = 256 + g.R(45); c.a[3] = g.R(4); break;
    case 4: c.a[3] = 2; break;
    case 5: c.a[3] = 2; c.a[0] = 1 + g.R(40); break;
    case 6: c.a[3] = 1; break;
    case 7: c.a[3] = 3; break;
    case 8: c.a[0] = 1 + g.R(8); c.a[3] = 2; break;
    case 9: c.a[1] = 0; c.a[3] = 0; break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funciones 5..7: CTaskCategories
//   5: a[0..2] = tipo que devuelve GetTaskType en la 1a, 2a y 3a consulta (las siguientes repiten a[2]).
//   6: a[0] = tipo; a[1] = desplazamiento del segundo bool respecto del primero (0 = mismo byte); a[2], a[3] = bytes iniciales.
//   7: a[0] = tarea (0 arena, 1 nula, 2 cruda 0x00BADC00); a[1], a[2], a[3] como en la 6.
inline u32 CatType(Gen& g) {
    return g.Pk({0x3FBu, 0x3FCu, 0x3FDu, 0x3FAu, 0x3FEu, 0x3FBu, 0x3FDu, 0u, 1u, 0x3F0u, 0x400u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x1003FBu, 0xFFFFFC05u, 0x3FCu, 0x384u, 0x3E8u});
}
inline void Make5(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = CatType(g); c.a[1] = CatType(g); c.a[2] = CatType(g);
    switch (f) {
    case 0: c.a[0] = c.a[1] = c.a[2] = g.Pk({0x3FBu, 0x3FCu, 0x3FDu}); break;
    case 1: c.a[1] = c.a[2] = g.Pk({0u, 1u, 0x3FAu, 0x3FEu}); c.a[0] = g.Pk({0x3FBu, 0x3FCu, 0x3FDu}); break;
    case 2: c.a[0] = c.a[2] = g.Pk({0u, 1u, 0x3FAu, 0x3FEu}); c.a[1] = g.Pk({0x3FBu, 0x3FCu, 0x3FDu}); break;
    case 3: c.a[0] = c.a[1] = g.Pk({0u, 1u, 0x3FAu, 0x3FEu}); c.a[2] = g.Pk({0x3FBu, 0x3FCu, 0x3FDu}); break;
    case 4: c.a[0] = c.a[1] = c.a[2] = g.Pk({0u, 1u, 0x3FAu, 0x3FEu, 0xFFFFFFFFu}); break;
    case 5: c.a[0] = g.Pk({0x3FAu, 0x3FEu}); c.a[1] = g.Pk({0x3FAu, 0x3FEu}); c.a[2] = g.Pk({0x3FBu, 0x3FDu}); break;
    case 6: c.a[0] = g.Pk({0x3FBu, 0x3FDu}); c.a[1] = g.Pk({0x3FAu, 0x3FEu}); c.a[2] = g.Pk({0x3FAu, 0x3FEu}); break;
    case 7: c.a[0] = c.a[1] = c.a[2] = g.Pk({0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0xFFFFFC05u, 0x1003FBu}); break;
    default: break;
    }
}
inline void Make6(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = CatType(g); c.a[1] = g.Pk({0u, 1u, 5u, 0u}); c.a[2] = g.R(256); c.a[3] = g.R(256);
    switch (f) {
    case 0: c.a[0] = g.Pk({0x3FBu, 0x3FCu, 0x3FDu}); c.a[1] = 0; break;
    case 1: c.a[0] = g.Pk({0x3FBu, 0x3FCu, 0x3FDu}); c.a[1] = 1; break;
    case 2: c.a[0] = g.Pk({0x3FAu, 0x3FEu}); c.a[1] = 0; break;
    case 3: c.a[0] = g.Pk({0x3FAu, 0x3FEu}); c.a[1] = 5; break;
    case 4: c.a[0] = g.Pk({0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0xFFFFFC05u}); break;
    case 5: c.a[2] = g.Pk({0u, 1u}); c.a[3] = g.Pk({0u, 1u}); break;
    case 6: c.a[2] = 0xFF; c.a[3] = 0x80; break;
    default: break;
    }
}
inline void Make7(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(70) ? 0u : g.R(3); c.a[1] = g.Pk({0u, 1u, 5u, 0u}); c.a[2] = g.R(256); c.a[3] = g.R(256);
    switch (f) {
    case 0: c.a[0] = 0; c.a[1] = 0; break;
    case 1: c.a[0] = 1; break;
    case 2: c.a[0] = 2; break;
    case 3: c.a[1] = 1; break;
    case 4: c.a[2] = g.Pk({0u, 1u}); c.a[3] = g.Pk({0u, 1u}); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 8: CTaskGoToVehicleAndLean::DoTidyUp (0x660F60)
//   a[0] = ped (0 arena, 1 nulo, 2 crudo 0x00BADC00), a[1] = vehiculo (0 arena, 1 nulo), a[2] = subtarea (0 arena, 1 nula), a[3] = byte +0x42E del vehiculo, a[4] = lo que devuelve MakeAbortable
inline void Make8(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(80) ? 0u : g.R(3); c.a[1] = g.P(70) ? 0u : 1u; c.a[2] = g.P(70) ? 0u : 1u; c.a[3] = g.R(256); c.a[4] = g.P(50) ? g.R(2) : g.W();
    switch (f) {
    case 0: c.a[1] = 0; c.a[2] = 0; c.a[3] = g.Pk({0xFFu, 0x01u, 0x81u, 0x03u}); break;
    case 1: c.a[1] = 0; c.a[2] = 0; c.a[3] = g.Pk({0x00u, 0xFEu, 0x80u, 0x7Eu}); break;
    case 2: c.a[1] = 1; c.a[2] = 0; break;
    case 3: c.a[1] = 0; c.a[2] = 1; break;
    case 4: c.a[1] = 1; c.a[2] = 1; break;
    case 5: c.a[0] = 1; c.a[1] = 0; c.a[2] = 0; break;
    case 6: c.a[0] = 2; c.a[1] = 0; c.a[2] = 0; break;
    case 7: c.a[3] = 0xFF; c.a[1] = 0; break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 9: IsDoorInUse (0x646CF0)
//   a[0] = puerta, a[1] = byte +0x486 (entrando), a[2] = byte +0x487 (saliendo) del vehiculo, a[3] = desplazamiento del segundo bool respecto del primero (0 = mismo byte), a[4], a[5] = bytes iniciales,
//   a[6] = vehiculo (0 arena, 1 nulo, 2 crudo 0x00C0FFEE; solo si la puerta no esta en 8..11, que es cuando el EXE no lo toca)
inline void Make9(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = 8 + g.R(4); c.a[1] = g.R(256); c.a[2] = g.R(256); c.a[3] = g.Pk({1u, 2u, 7u, 1u, 0u}); c.a[4] = g.R(256); c.a[5] = g.R(256); c.a[6] = 0;
    if (g.P(15)) c.a[0] = g.Pk({0u, 7u, 12u, 13u, 0xFFFFFFF8u, 0xFFFFFFFFu, 0x80000008u, 0x80000009u, 1u, 100u, 0xFFFFFFFCu, 0x7FFFFFFFu, 0x100u});
    switch (f) {
    case 0: c.a[0] = 8; break;
    case 1: c.a[0] = 9; break;
    case 2: c.a[0] = 10; break;
    case 3: c.a[0] = 11; break;
    case 4: c.a[0] = 8 + g.R(4); c.a[1] = 0; c.a[2] = 0; break;
    case 5: c.a[0] = 8 + g.R(4); c.a[1] = 0xFF; c.a[2] = 0xFF; break;
    case 6: c.a[0] = 8 + g.R(4); c.a[1] = 1u << g.R(4); c.a[2] = 1u << g.R(4); break;
    case 7: c.a[0] = 8 + g.R(4); c.a[3] = 0; break;
    case 8: c.a[0] = g.Pk({0u, 7u, 12u, 13u, 0xFFFFFFF8u, 0xFFFFFFFFu, 0x80000008u, 0xFFFFFFFCu}); break;
    default: break;
    }
    const bool valid = c.a[0] - 8u <= 3u;
    if (!valid) c.a[6] = g.P(50) ? 0u : g.R(3);
}

// ------------------------------------------------------------------------------------------------ funcion 10: CTaskComplexBeInCouple::GetSideOfPartner (0x683840)
//   a[0] = el peaton tiene matriz, a[1..3] = su posicion en la activa (matriz + 0x30 o placement + 4), a[4..6] = en la otra (basura); a[7..9] = posicion del companero (matriz + 0x30; el EXE lee su matriz sin
//   comprobar nulo), a[10..12] = el vector "derecha" de la matriz del companero, a[13] = 1: el companero es el propio peaton (usa su matriz; fuerza a[0] = 1), a[14..16] = basura en el placement del companero
inline void Make10(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(60);
    for (u32 k = 0; k < 3; ++k) { c.a[1 + k] = Coord(g, 300.0f); c.a[4 + k] = Coord(g); c.a[7 + k] = Coord(g, 300.0f); c.a[10 + k] = B(g.Fl(-1.0f, 1.0f)); c.a[14 + k] = Coord(g); }
    c.a[13] = g.P(8);
    switch (f) {
    case 0: c.a[0] = 1; c.a[13] = 0; break;
    case 1: c.a[0] = 0; c.a[13] = 0; break;
    case 2:   // el peaton cerca del companero: la suma de productos se cancela
        for (u32 k = 0; k < 3; ++k) c.a[1 + k] = NextUp(c.a[7 + k], int(g.R(5)) - 2);
        break;
    case 3:   // misma posicion exacta: suma cero (+0 / -0 segun los signos)
        for (u32 k = 0; k < 3; ++k) c.a[1 + k] = c.a[7 + k];
        c.a[13] = 0;
        break;
    case 4: for (u32 k = 0; k < 9; ++k) if (g.P(30)) c.a[1 + k] = Special(g); break;
    case 5: for (u32 k = 0; k < 3; ++k) if (g.P(40)) c.a[10 + k] = Special(g); break;
    case 6:   // magnitudes enormes: los productos salen del rango de float dentro de la x87
        for (u32 k = 0; k < 3; ++k) { if (g.P(60)) c.a[1 + k] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0x70000000u}); if (g.P(60)) c.a[7 + k] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xF0000000u}); if (g.P(60)) c.a[10 + k] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0x70000000u, 0x7E800000u}); }
        break;
    case 7:   // magnitudes diminutas: desnormales en float, normales en el registro
        for (u32 k = 0; k < 3; ++k) { if (g.P(60)) c.a[1 + k] = g.Pk({0x00000100u, 0x80000100u, 0x007FFFFFu, 0x00800000u, 0x1E000000u, 0x9E000000u}); if (g.P(60)) c.a[7 + k] = g.Pk({0x00000100u, 0x80000100u, 0x00800000u, 0x80800000u, 0x1E000000u}); if (g.P(60)) c.a[10 + k] = g.Pk({0x00000100u, 0x80000100u, 0x007FFFFFu, 0x20000000u, 0x00400000u}); }
        break;
    case 8: for (u32 k = 0; k < 9; ++k) if (g.P(25)) c.a[1 + k] = g.Pk({0x7F800000u, 0xFF800000u, 0xFFC00000u, 0u, 0x80000000u}); break;
    case 9: for (u32 k = 0; k < 9; ++k) if (g.P(50)) c.a[1 + k] = g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x40000000u}); for (u32 k = 0; k < 3; ++k) if (g.P(50)) c.a[10 + k] = g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u}); break;
    case 10:  // ejes unitarios: el signo de la diferencia es el de la coordenada
        for (u32 k = 0; k < 3; ++k) c.a[10 + k] = g.Pk({0x3F800000u, 0xBF800000u, 0u, 0x80000000u});
        break;
    case 11:  // coordenadas de rango amplio
        for (u32 k = 0; k < 3; ++k) { c.a[1 + k] = Wide(g); c.a[7 + k] = Wide(g); c.a[10 + k] = Wide(g); }
        break;
    case 12:  // posicion en la otra estructura con basura especial (no debe leerse)
        for (u32 k = 4; k < 7; ++k) c.a[k] = Special(g);
        for (u32 k = 14; k < 17; ++k) c.a[k] = Special(g);
        break;
    case 13:  // cancelacion fina: la diferencia es de unos ulps del tamano de los productos
        for (u32 k = 0; k < 3; ++k) { c.a[7 + k] = B(g.Fl(50.0f, 300.0f)); c.a[1 + k] = NextUp(c.a[7 + k], int(g.R(3)) - 1); }
        break;
    default: break;
    }
    if (c.a[13]) c.a[0] = 1;
}

// ------------------------------------------------------------------------------------------------ funciones 11 y 12: CWeaponInfo::GetRLoadA / GetRLoadB (0x73A280 / 0x73A2B0)
//   a[0] = dword del argumento bool (solo cuenta el byte bajo), a[1] = indice de apuntado (int16 en +0x6C: sin signo en a[1] & 0xFFFF), a[2] = los 16 bits altos del dword +0x6C
inline void MakeW(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(40) ? g.R(2) : g.W(); c.a[1] = g.P(85) ? g.R(12) : u32(i32(g.R(241)) - 40) & 0xFFFFu; c.a[2] = g.P(50) ? 0u : (g.W() & 0xFFFFu);
    switch (f) {
    case 0: c.a[0] = 0; c.a[1] = g.R(12); break;
    case 1: c.a[0] = 1; c.a[1] = g.R(12); break;
    case 2: c.a[0] = 0x100u + g.R(2) * 0x10000u; c.a[1] = g.R(12); break;          // byte bajo cero con bits altos
    case 3: c.a[0] = 0xFFFFFF00u | g.R(2); c.a[1] = g.R(12); break;
    case 4: c.a[1] = u32(i32(g.R(40)) - 40) & 0xFFFFu; break;                        // indices negativos
    case 5: c.a[1] = 12 + g.R(150); break;
    case 6: c.a[1] = g.R(12); c.a[2] = 0xFFFF; break;
    case 7: c.a[1] = 0; break;
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
    case 0: case 1: case 2: case 3: MakeStr(c, n); break;
    case 4: Make4(c, n); break;
    case 5: Make5(c, n); break;
    case 6: Make6(c, n); break;
    case 7: Make7(c, n); break;
    case 8: Make8(c, n); break;
    case 9: Make9(c, n); break;
    case 10: Make10(c, n); break;
    default: MakeW(c, n); break;
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

#pragma once
// Fixtures del banco Fn136A (lote 136A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado ya eran las correctas, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre                                   forma
//   0   0x6FD7C0   GetCharacterPositionInCharSetExe         __usercall: caracter en AL, (uint32* col, uint32* fila) en la pila, `ret`: tabla de casos 0x20..0x7A (matricula)
//   1   0x6FE260   RoadsignGetLineAndRowExe                 __usercall: caracter en AL, (int32* col, int32* fila) en la pila, `ret`: tabla de casos 0x21..0x7E (cartel de carretera)
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40). Las dos funciones no tienen colaboradores ni leen memoria global.
// Familia = n % 16: tipo de colocacion de los dos punteros = familia % 4 (0 en arenas distintas, 1 distintos en la misma arena, 2 iguales o solapados, 3 al borde de la ventana) y
// relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD). El caracter (AL) recorre los 256 valores (cada uno 4 veces por familia); los 24 bits altos de EAX son aleatorios.
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

namespace F136AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 2;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 100001u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
// Relleno de una region: 0 aleatorio (semilla), 1 ceros, 2 unos, 3 0xCD
inline void FillKind(u8* dst, std::size_t bytes, u32 kind, u32 seed) {
    if (kind == 0) FillWords(dst, bytes, seed);
    else std::memset(dst, kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), bytes);
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
    Js& H(const char* k, const std::vector<u8>& v) {
        constexpr char d[] = "0123456789abcdef";
        Key(k); s += '"';
        for (u8 b : v) { s += d[b >> 4]; s += d[b & 15]; }
        s += '"'; return *this;
    }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 16> a{};
    std::vector<u8> s;          // sin uso en este lote (hex vacio)
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 W() { return Rand(n, nk()); }
};

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento del primer puntero (columna) en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = desplazamiento del segundo puntero (fila),
//   a[4] = 1 si el segundo puntero esta en la arena A (0: arena B), a[5] = caracter (AL), a[12..15] = semillas de las arenas A, B, C y de la memoria global
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    const u32 win = u32(kWin);
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    const u32 q = n / 16;
    const u32 ch = (q + 37u * f) & 0xFFu;
    c.a[5] = ch;
    c.eax = (c.eax & 0xFFFFFF00u) | ch;   // los 24 bits altos de EAX son basura: la funcion solo lee AL
    switch (kind) {
    case 0:   // arenas distintas, alineados
        c.a[4] = 0;
        c.a[0] = 4 * g.R((win - 4) / 4 + 1);
        c.a[3] = 4 * g.R((win - 4) / 4 + 1);
        break;
    case 1: { // misma arena, dwords distintos y alineados (sin solaparse)
        c.a[4] = 1;
        c.a[0] = 68 + 4 * g.R((win - 140) / 4);
        const u32 d = 4 * (1 + g.R(16));
        c.a[3] = g.P(50) ? c.a[0] + d : c.a[0] - d;
        break;
    }
    case 2: { // misma arena, punteros iguales (gana la fila) o solapados por 1..3 bytes (en ambos sentidos), posiblemente desalineados
        c.a[4] = 1;
        c.a[0] = 8 + g.R(win - 20);
        const u32 shape = g.R(4);
        if (shape == 0) c.a[3] = c.a[0];
        else if (shape == 1) c.a[3] = c.a[0] + 1 + g.R(3);
        else if (shape == 2) c.a[3] = c.a[0] - 1 - g.R(3);
        else c.a[3] = c.a[0] + (g.P(50) ? 4 : -4);
        break;
    }
    default: { // al borde de la ventana: la escritura entra en la guarda inicial o final; el segundo puntero en A o en B
        c.a[4] = g.R(2);
        const auto edge = [&]() -> u32 { return g.P(50) ? 4 * g.R(4) + g.R(2) : win - 4 + g.R(0x30); };
        c.a[0] = edge();
        c.a[3] = edge();
        break;
    }
    }
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.ecx = Rand(n, 3) | 1u; c.stack = StackSeed(n);
    MakeAll(c, n);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    j.H("s", c.s);
    return j.End();
}
}

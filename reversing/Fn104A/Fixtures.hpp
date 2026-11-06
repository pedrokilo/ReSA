#pragma once
// Fixtures del banco Fn104A (lote 104A): 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CAutomobile::SetRandomDamage   0x6A2530  thiscall (ECX = coche de 0x988 bytes; bRemoveStuff en pila como dword, el EXE lee solo el byte bajo), `ret 4`; void   (hook S)
//
// Todos los valores viajan como u32. Familia = n % 16 (1024 casos por familia).
//   a[0]      = dword de pila del argumento (byte bajo = bRemoveStuff, los otros tres = basura)
//   a[1..11]  = lo que devuelve rand() del CRT del EXE (0x821B1E) en cada tirada; bits 0..14 = valor (0..32767), bit 15 siempre 0 (con 0x8000 o mas el EXE saldria de sus marcadores),
//               bits 16..31 = basura que `& 0xFFFF` debe quitar. Tras las 11 se devuelve 0 (no se gastan mas de 11)
//   a[12]     = indice de modelo (word +0x22 del coche), 0..19999; la entrada de la tabla 0xA9B0C8 se sustituye por un model info propio
//   a[13]     = mascara de componentes danables (+0x310 de la estructura del modelo)
//   a[14..19] = lo que devuelve CDamageManager::GetCarNodeIndexFromDoor para las puertas 0..5 ;  a[20..26] = lo mismo para los paneles 0..6 (GetCarNodeIndexFromPanel)
//   a[27..32] = estado que devuelve GetDoorStatus por puerta (0..4) ;  a[33..39] = GetPanelStatus por panel (0..4)
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <string>
#include <vector>

namespace F104AFix {
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
constexpr std::size_t kCarWin = 0x988, kMiWin = 0x308, kVsWin = 0x314;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 104001u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
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
    u32 W() { return Rand(n, nk()); }
};

inline constexpr unsigned kRands = 11;
// valor que devuelve rand(): 15 bits utiles y 16 bits altos de basura
inline u32 RandVal(Gen& g, u32 low, bool garbage) { return (low & 0x7FFFu) | (garbage ? (g.W() & 0xFFFF0000u) : 0u); }
// cerca de un umbral de truncamiento de r*(1/32768)*k (j*32768/k, j = 0..k) con desplazamiento -2..+2
inline u32 Edge(Gen& g, u32 k) {
    const u32 j = g.R(k + 1);
    const u32 base = (j * 32768u + k - 1) / k;
    const int v = int(base) + int(g.R(5)) - 2;
    return u32(v < 0 ? 0 : (v > 32767 ? 32767 : v));
}

inline void Make0(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    u32 flag;
    switch (f) {
    case 2: flag = 0; break;
    case 3: case 13: flag = g.P(50) ? 0u : (1u + g.R(255)); break;
    case 0: case 4: case 5: case 6: case 7: case 8: case 14: flag = 1; break;
    default: flag = g.P(25) ? 0u : 1u; break;
    }
    u32 hi = (f == 3 || f == 13 || g.P(30)) ? (g.W() & 0xFFFFFF00u) : 0u;
    if (f == 3 && flag == 0 && g.P(50)) hi |= 0x100u;
    c.a[0] = flag | hi;
    const bool garbage = f == 13 || g.P(15);
    for (unsigned k = 0; k < kRands; ++k) {
        u32 v;
        switch (f) {
        case 4: v = g.Pk({0u, 32767u, 0u, 32767u, 1u, 32766u, 16384u, 16383u}); break;
        case 5: v = k == 0 ? Edge(g, 6) : (g.P(50) ? g.R(32768) : Edge(g, 6 - (k % 6))); break;
        case 6: v = k == 0 ? g.Pk({0u, 1u, 5u}) : (k == 1 ? g.Pk({10922u, 10923u, 21845u, 21846u, 0u, 32767u, 10921u, 21844u, 10924u, 21847u}) : g.R(32768)); break;
        case 7: v = k == 0 ? g.Pk({32767u, 32000u, 6000u, 30000u, 20000u}) : (k == 1 ? g.Pk({8191u, 8192u, 16383u, 16384u, 24575u, 24576u, 0u, 32767u}) : g.R(32768)); break;
        case 8: v = g.Pk({0u, 32767u}); break;
        case 14: v = k == 0 ? 27307u + g.R(5461) : (g.P(50) ? g.R(32768) : g.Pk({0u, 32767u, g.R(32768)})); break;
        default: v = g.P(10) ? g.Pk({0u, 32767u, 16383u, 16384u}) : g.R(32768); break;
        }
        c.a[1 + k] = RandVal(g, v, garbage);
    }
    if (f == 12) c.a[12] = g.Pk({0u, 1u, 19999u, 400u, 611u, 0x4E1Fu, 0x1000u, 9999u});
    else c.a[12] = g.P(60) ? 400u + g.R(212) : g.R(20000);
    switch (f) {
    case 0: c.a[13] = 0xFFFFFFFFu; break;
    case 9: c.a[13] = g.Pk({0u, 1u, 0x80000000u, 0x0000FFFFu, 0xFFFF0000u, 1u << g.R(32), ~(1u << g.R(32)), 0x55555555u, 0xAAAAAAAAu}); break;
    default: c.a[13] = g.P(30) ? 0xFFFFFFFFu : (g.P(15) ? 0u : g.W()); break;
    }
    for (unsigned k = 0; k < 13; ++k) {
        u32 v = f == 10 ? g.R(48) : (g.P(80) ? g.R(28) : g.R(48));
        if (f == 10 && g.P(40)) v = 32 + g.R(16);
        c.a[14 + k] = v;
    }
    if (f == 10) {   // bits de la mascara de los nodos que se devuelven (con la envoltura a 5 bits del desplazamiento)
        u32 m = 0;
        for (unsigned k = 0; k < 13; ++k) if (g.P(60)) m |= 1u << (c.a[14 + k] & 31u);
        c.a[13] = m;
    }
    for (unsigned k = 0; k < 13; ++k) {
        u32 s;
        switch (f) {
        case 0: case 4: case 5: case 6: case 7: case 8: case 14: s = g.P(85) ? 0u : g.R(5); break;
        case 11: s = g.P(50) ? 0u : g.R(5); break;
        case 1: case 2: case 3: s = g.P(60) ? 0u : g.R(5); break;
        default: s = g.P(70) ? 0u : g.R(5); break;
        }
        c.a[27 + k] = s;
    }
    if (f == 15) {   // todo danable y casi todo intacto
        c.a[13] = 0xFFFFFFFFu;
        for (unsigned k = 0; k < 13; ++k) c.a[27 + k] = g.P(10) ? g.R(5) : 0u;
    }
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    Make0(c, n);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    return j.End();
}
}

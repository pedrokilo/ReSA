#pragma once
// Fixtures del banco Fn90A (lote piloto 90A, categoria 1: triviales y thunks muy llamados): diez funciones minimas del juego, 2048 casos cada una (20480 pares).
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y fixture_dump.cpp (autocomprobacion contra el oraculo Python).
//
//   0 CPad::GetPad                                0x53FB70  cdecl (n), `ret`; EAX = 0xB73458 + n * 0x134 (32 bits); ECX/EDX intactos
//   1 CRGBA::Constructor (CRGBA::CRGBA(r,g,b,a))  0x7170C0  thiscall (this, 4 dwords: usa el byte bajo), `ret 10h`; escribe 4 bytes; EAX = this
//   2 CTaskSimple::Constructor (cuerpo)           0x61A390  thiscall sin argumentos; [this] = 0x86D4A8, [this+4] = 0; EAX = this; ECX/EDX intactos
//   3 CTaskComplex::Constructor (cuerpo)          0x61A3B0  thiscall sin argumentos; [this] = 0x86D4CC, [this+4] = [this+8] = 0; EAX = this; EDX intacto (ECX = 0)
//   4 CAnimManager::RemoveAnimBlockRef            0x4D3FD0  cdecl (indice), `ret`; word [0xB5D4A0 + indice*32 + 0x12] -= 1; ECX/EDX intactos
//   5 Fx_c::GetFxQuality                          0x49EA50  thiscall sin argumentos; EAX = [this+0x54]; ECX/EDX intactos
//   6 Fx_c::SetFxQuality                          0x49EA40  thiscall (calidad), `ret 4`; [this+0x54] = calidad; ECX/EDX intactos
//   7 CSphere::Set                                0x40FCF0  thiscall (radio, centro*), `ret 8`; [this+0xC] = radio, luego copia 3 dwords de *centro a [this]; ECX intacto
//   8 FxPrtMult_c::Constructor                    0x4AB270  thiscall sin argumentos; 7 dwords = 1.0f; EAX = this; EDX intacto (ECX = 1.0f)
//   9 CPedDamageResponseCalculator::Destructor    0x4AD420  thiscall sin argumentos; solo `ret`: EAX, ECX y EDX intactos
//
// Memoria: una arena de 0x180 bytes (guarda 0x40 + ventana 0x100 + guarda 0x40) alineada a 32; `this`, los punteros y los bloques de animacion caen dentro de la ventana.
// Los valores van como patrones de 32 bits sin signo. Familia = n % 16 (128 casos por familia).
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace F90Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 2048;       // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 10;
constexpr std::size_t kGuard = 0x40, kWin = 0x100, kArena = kGuard + kWin + kGuard;
constexpr u32 kPadsBase = 0xB73458, kPadSize = 0x134, kAnimBase = 0xB5D4A0;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 90001u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
// Relleno de la ventana segun el estilo de la familia: 0 ruido, 1 ceros, 2 unos, 3 ruido con las palabras cortas a cero, 4 patron 0x80/0x7F
inline void FillWindow(u8* win, u32 style, u32 seed) {
    switch (style) {
    case 1: std::memset(win, 0x00, kWin); break;
    case 2: std::memset(win, 0xFF, kWin); break;
    case 4: for (std::size_t i = 0; i < kWin; ++i) win[i] = (i & 1) ? 0x7F : 0x80; break;
    default:
        FillWords(win, kWin, seed);
        if (style == 3) for (std::size_t i = 0; i < kWin; i += 8) { win[i] = 0; win[i + 1] = 0; }
        break;
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

inline constexpr std::array<u32, 16> kEdge{0u, 1u, 2u, 3u, 0x7Fu, 0x80u, 0xFFu, 0x100u, 0xFFFFu, 0x10000u, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x12345678u};
inline constexpr std::array<u32, 12> kFloatBits{0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x00000001u, 0x7F7FFFFFu, 0x42C80000u, 0x40490FDBu};
inline constexpr std::array<u32, 8> kWord16{0u, 1u, 2u, 0xFFFFu, 0x8000u, 0x7FFFu, 0x8001u, 0xFFFEu};
inline constexpr std::array<u32, 8> kQuality{0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 4u};
inline constexpr std::array<int, 9> kAlias{0, 4, 8, 12, -4, -8, -12, 1, -1};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }

// El caso. Los campos de cada funcion son independientes; `off` es el desplazamiento de `this` en la ventana.
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 fill{};                 // estilo del relleno de la ventana
    u32 off{};                  // `this` = ventana + off
    u32 a[4]{};                 // argumentos crudos (n, componentes r g b a, calidad, radio)
    u32 blk{}, hi{};            // 4: bloque de animacion en la ventana (multiplo de 32) y bits altos 27..31 del indice (el EXE los descarta con shl 5)
    u32 pokeOff{}, poke{}, pokeSize{};   // valor forzado en la ventana tras el relleno (tamano 0 = ninguno): contador del bloque, calidad del Fx
    u32 pOff{};                 // 7: desplazamiento del centro en la ventana
};

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n; c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.ecx = Rand(n, 3); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    u32 kk = 100 + fn * 50;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    c.fill = f < 8 ? 0u : (f == 8 ? 1u : (f == 9 ? 2u : (f == 10 ? 3u : (f == 11 ? 4u : 0u))));
    auto Word = [&]() -> u32 { return P(35) ? Pick(kEdge, n, nk()) : Rand(n, nk()); };
    switch (fn) {
    case 0:   // GetPad: indices de pad habituales, aleatorios y extremos
        c.a[0] = f < 4 ? R(4) : (f < 8 ? Rand(n, nk()) : (f < 12 ? Pick(kEdge, n, nk()) : u32(-int(R(100000)) - int(R(8)))));
        break;
    case 1:   // CRGBA: los cuatro dwords de pila con basura en los bytes altos
        for (u32 k = 0; k < 4; ++k) c.a[k] = f < 4 ? R(256) : (f < 8 ? Rand(n, nk()) : Word());
        c.off = R(0x9D);
        break;
    case 2:
    case 3:
    case 9:
        c.off = R(0xE0);
        break;
    case 4:   // RemoveAnimBlockRef: el contador (word en +0x12 del bloque) con extremos y bits altos del indice descartados
        c.blk = 32 * R(7);
        c.hi = f < 4 ? 0u : R(32);
        c.pokeOff = c.blk + 0x12; c.pokeSize = 2;
        c.poke = P(60) ? Pick(kWord16, n, nk()) : (Rand(n, nk()) & 0xFFFFu);
        if (P(10)) c.pokeSize = 0;
        break;
    case 5:
        c.off = R(0xA9);
        c.pokeOff = c.off + 0x54; c.pokeSize = 4; c.poke = P(60) ? Pick(kQuality, n, nk()) : Rand(n, nk());
        break;
    case 6:
        c.off = R(0xA9);
        c.a[0] = P(60) ? Pick(kQuality, n, nk()) : Rand(n, nk());
        break;
    case 7: {
        c.off = R(0xF1);
        c.pOff = R(0xF5);
        if (f >= 12) { const int dd = kAlias[R(9)]; const int p = int(c.off) + dd; c.pOff = p < 0 ? 0u : (p > 0xF4 ? 0xF4u : u32(p)); }
        c.a[0] = f < 6 ? Rand(n, nk()) : (f < 12 ? Pick(kFloatBits, n, nk()) : Word());
        break;
    }
    case 8:
        c.off = R(0xE5);
        break;
    default: break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack).U("fill", c.fill);
    j.U("off", c.off).A("a", std::vector<u32>(c.a, c.a + 4)).U("blk", c.blk).U("hi", c.hi).U("poke_off", c.pokeOff).U("poke", c.poke).U("poke_size", c.pokeSize).U("p_off", c.pOff);
    return j.End();
}
}

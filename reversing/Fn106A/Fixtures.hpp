#pragma once
// Fixtures del banco Fn106A (lote 106A): 16384 casos por funcion (7 x 16384 = 114688 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CrtPowThunk                      0x470000  cdecl (base float, exponente float); resultado double en ST0 (hook S)
//   1 RoundToDecimals                  0x50A0A0  cdecl (valor float, digitos int); resultado en ST0 (hook S)
//   2 CTaskSimpleCarSetPedOut ctor     0x6478B0  thiscall (ECX = tarea de 0x18 bytes; vehiculo, puerta, byte en la pila), `ret 0Ch`; EAX = this (hook S)
//   3 GxtCharToAscii                   0x69F7E0  cdecl (cadena, desplazamiento byte); EAX = buffer 0xC1BDD0 (hook S)
//   4 CEventKnockOffBike dtor (cuerpo) 0x4AFE70  thiscall (ECX = evento de 0x3C bytes), `ret`; void (hook S)
//   5 CTaskSimpleStandStill dtor       0x62F360  thiscall (ECX = tarea de 0x20 bytes), `ret`; void (hook S)
//   6 RwIm2DRenderPrimitiveThunk       0x734E90  cdecl (tipo, vertices, numero); salta a [RwEngineInstance + 0x30]; EAX = lo que devuelva (hook S)
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

namespace F106AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 7;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kTaskWin = 0x20, kEventWin = 0x3C, kEntWin = 0x40, kStrWin = 0x400, kGlobalsWin = 0x60;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 106001u); }
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
// Un float de magnitud aleatoria (de 10^-e a 10^e) con signo
inline u32 WideE(Gen& g, int e) {
    const double ex = double(int(g.R(u32(2 * e + 1))) - e);
    const double m = 1.0 + double(g.R(1000000)) / 1000000.0;
    return B(float((g.P(50) ? -1.0 : 1.0) * m * std::pow(10.0, ex)));
}

// ------------------------------------------------------------------------------------------------ funcion 0: CrtPowThunk (0x470000)
//   a[0] = base (bits float), a[1] = exponente (bits float). Resultado: pow(base, exponente) como double en ST0.
inline void Make0(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = B(g.Fl(0.0f, 20.0f)); c.a[1] = B(g.Fl(-10.0f, 10.0f));
    switch (f) {
    case 0: c.a[0] = B(g.Fl(0.01f, 100.0f)); c.a[1] = B(float(int(g.R(21)) - 10)); break;                              // exponente entero pequeno
    case 1: c.a[0] = B(g.Fl(0.01f, 100.0f)); c.a[1] = B(g.Fl(-8.0f, 8.0f)); break;                                      // exponente fraccionario
    case 2: c.a[0] = g.Pk({B(0.995f), B(0.9f), B(0.99f), B(2.0f), B(10.0f), B(0.5f), B(0.98f), B(0.97f), B(0.995f)});   // valores del juego: pow(amortiguacion, paso)
            c.a[1] = g.P(70) ? B(g.Fl(0.0f, 8.0f)) : B(g.Fl(0.0f, 4000.0f)); break;
    case 3: c.a[0] = B(-g.Fl(0.01f, 50.0f)); c.a[1] = B(float(int(g.R(41)) - 20)); break;                               // base negativa, exponente entero
    case 4: c.a[0] = B(-g.Fl(0.01f, 50.0f)); c.a[1] = B(g.Fl(-6.0f, 6.0f)); if (g.P(30)) c.a[1] = B(float(int(g.R(9)) - 4) + 0.5f); break;   // base negativa, exponente no entero (NaN)
    case 5: c.a[0] = g.Pk({0u, 0x80000000u}); c.a[1] = g.Pk({B(-3.0f), B(-2.0f), B(-1.0f), B(-0.5f), B(0.5f), B(1.0f), B(2.0f), B(3.0f), B(0.0f), 0x80000000u, 0xFFC00000u, 0x7F800000u, 0xFF800000u, B(1.0e10f), B(-1.0e10f), B(2.5f), B(-2.5f)}); break;
    case 6: c.a[0] = kInf[g.R(4)]; c.a[1] = g.Pk({B(-3.0f), B(-2.0f), B(-1.0f), B(-0.5f), B(0.5f), B(1.0f), B(2.0f), B(3.0f), B(0.0f), 0x80000000u, 0xFFC00000u, 0x7F800000u, 0xFF800000u, B(2.5f)}); break;
    case 7: c.a[0] = g.Pk({B(0.5f), B(-0.5f), B(1.0f), B(-1.0f), B(2.0f), B(-2.0f), B(0.0f), 0x80000000u, B(0.999f), B(1.001f)}); c.a[1] = kInf[g.R(4)]; break;
    case 8: if (g.P(50)) { c.a[0] = 0xFFC00000u; } else { c.a[1] = 0xFFC00000u; }
            if (g.P(40)) c.a[1] = g.Pk({0u, 0x80000000u, B(1.0f), B(2.0f)});
            if (g.P(20)) c.a[0] = g.Pk({0u, B(1.0f), B(-1.0f)});
            break;
    case 9: c.a[0] = g.Pk({B(1.0f), B(-1.0f)}); c.a[1] = g.Pk({0xFFC00000u, 0x7F800000u, 0xFF800000u, B(2.0f), B(0.5f), B(3.0f), 0u}); break;   // +-1 con NaN e infinito (el EXE da NaN, C99 da 1)
    case 10: c.a[0] = B(g.Fl(2.0f, 3.0e38f)); c.a[1] = B(g.Fl(2.0f, 300.0f)); if (g.P(30)) c.a[0] = g.Pk({0x7F7FFFFFu, B(10.0f), B(1.0e10f)}); break;   // desbordamiento
    case 11: c.a[0] = B(g.Fl(1.0e-30f, 0.5f)); c.a[1] = B(g.Fl(2.0f, 400.0f)); if (g.P(30)) c.a[0] = B(g.Fl(0.5f, 0.99f)); break;               // subdesbordamiento y desnormales
    case 12: c.a[0] = kTiny[g.R(12)]; c.a[1] = g.Pk({B(-2.0f), B(-1.0f), B(-0.5f), B(0.5f), B(1.0f), B(2.0f), B(3.0f), B(0.25f), B(-0.25f)}); break;
    case 13: c.a[0] = WideE(g, 30); c.a[1] = g.P(60) ? WideE(g, 3) : WideE(g, 30); break;
    case 14: c.a[0] = g.Pk({B(2.0f), B(10.0f), B(3.0f), B(5.0f), B(4.0f), B(0.5f), B(0.1f), B(7.0f), B(-2.0f), B(-3.0f)}); c.a[1] = B(float(int(g.R(61)) - 30)); break;   // potencias exactas
    case 15: c.a[0] = B(1.0f + g.Fl(-0.001f, 0.001f)); c.a[1] = B(g.Fl(-10000.0f, 10000.0f)); if (g.P(40)) c.a[0] = B(1.0f - g.Fl(0.0f, 1.0e-5f)); break;     // base casi uno con exponente grande
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 1: RoundToDecimals (0x50A0A0)
//   a[0] = valor (bits float), a[1] = digitos (int con signo). Resultado en ST0.
inline void Make1(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = B(g.Fl(-1000.0f, 1000.0f)); c.a[1] = g.R(5);
    switch (f) {
    case 0: c.a[0] = B(g.Fl(-1000.0f, 1000.0f)); c.a[1] = g.R(5); break;
    case 1: c.a[0] = B(g.Fl(-100000.0f, 100000.0f)); c.a[1] = u32(-int(1 + g.R(4))); break;                              // redondeo a decenas, centenas...
    case 2: c.a[0] = B(g.Fl(-3.2f, 3.2f)); c.a[1] = 2 + g.R(2); break;                                                    // valores de camara (radianes)
    case 3: { const int d = int(g.R(4)); const float s = std::pow(10.0f, float(d)); c.a[0] = B((float(int(g.R(2001)) - 1000) + 0.5f) / s); c.a[1] = u32(d); break; }   // mitades justas
    case 4: c.a[0] = kTiny[g.R(12)]; c.a[1] = g.R(6); if (g.P(30)) c.a[1] = u32(-int(g.R(4))); break;                    // ceros con signo y desnormales
    case 5: c.a[0] = g.Pk({0x7F800000u, 0xFF800000u, 0xFFC00000u, 0x7F800000u, 0xFFC00000u}); c.a[1] = g.P(80) ? g.R(5) : u32(int(g.R(11)) - 5); break;   // infinitos y NaN
    case 6: c.a[0] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u, B(1.0e30f), B(-3.0e38f), B(1.0e20f)}); c.a[1] = u32(int(g.R(25)) - 10); break;   // magnitudes enormes
    case 7: c.a[0] = B(g.Fl(-5.0f, 5.0f)); c.a[1] = g.Pk({300u, u32(-300), 307u, 308u, 309u, 310u, u32(-307), u32(-308), u32(-309), u32(-323), u32(-324), 1000u, u32(-1000), 0x7FFFFFFFu, 0x80000000u, 0x7FFFFFFEu, 0xFFFFFFFFu, 0x80000001u, 400u, u32(-400)}); break;   // digitos extremos
    case 8: c.a[0] = B(g.Fl(-1000.0f, 1000.0f)); c.a[1] = 10u + g.R(31); break;                                          // muchos decimales
    case 9: c.a[0] = B(g.Fl(-1.0e9f, 1.0e9f)); c.a[1] = u32(-int(1 + g.R(30))); break;                                   // muchos ceros
    case 10: c.a[0] = B(g.Fl(-1.0e-6f, 1.0e-6f)); c.a[1] = g.R(8); break;                                                 // el resultado debe ser cero
    case 11: { const int d = int(g.R(4)); c.a[0] = B(float(int(g.R(4001)) - 2000) / std::pow(10.0f, float(d)) + 0.5f / std::pow(10.0f, float(d))); c.a[1] = u32(d); break; }   // empates tras el escalado
    case 12: c.a[0] = WideE(g, 30); c.a[1] = u32(int(g.R(81)) - 40); break;
    case 13: c.a[0] = B(float(int(g.R(2001)) - 1000) * 0.1f); c.a[1] = 1; if (g.P(30)) c.a[1] = g.R(4); break;           // multiplos de 0.1
    case 14: c.a[0] = B(-g.Fl(0.0f, 1000.0f)); c.a[1] = g.R(4); break;                                                    // negativos
    case 15: c.a[0] = g.P(70) ? B(g.Fl(-2000.0f, 2000.0f)) : g.W(); c.a[1] = g.P(70) ? g.R(5) : g.W(); break;             // patrones libres
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 2: CTaskSimpleCarSetPedOut constructor (0x6478B0)
//   a[0] = vehiculo (0 arena de entidad, 1 nulo, 2 valor crudo 0x00C0FFEE), a[1] = puerta (dword), a[2] = byte, a[3] = basura de los bytes altos del dword del byte
inline void Make2(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = f == 4 ? 1u : (f == 5 ? 2u : (g.P(75) ? 0u : g.R(3)));
    c.a[1] = g.P(60) ? g.R(8) : g.W();
    c.a[2] = (f == 6 || f == 7) ? (1 + g.R(255)) : (g.P(60) ? g.R(2) : g.R(256));
    c.a[3] = g.P(50) ? 0u : (g.W() & 0xFFFFFF00u);
    if (f == 8) { c.a[1] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 0xFFFFFFFFu, 0x80000000u}); }
    if (f == 9) { c.a[2] = g.Pk({0u, 1u, 2u, 0x80u, 0xFFu}); c.a[3] = g.W() & 0xFFFFFF00u; }
}

// ------------------------------------------------------------------------------------------------ funcion 3: GxtCharToAscii (0x69F7E0)
//   a[0] = longitud de la cadena, a[1] = modo (0 normal con terminador, 1 sin terminador en la ventana, 2 puntero nulo), a[2] = desplazamiento (byte),
//   a[3] = basura de los bytes altos del argumento del desplazamiento, a[4] = modo de contenido (0 todos los codigos, 1 acentos, 2 ASCII, 3 codigos de 0xC8 a 0xFF), a[5] = semilla del contenido
inline u32 GxtByte(Gen& g, u32 mode) {
    switch (mode) {
    case 1: return 0x80 + g.R(0x51);                 // 0x80..0xD0
    case 2: return 1 + g.R(0x7F);                    // 0x01..0x7F
    case 3: return 0xC8 + g.R(0x38);                 // 0xC8..0xFF
    default: { const u32 v = g.R(256); return v ? v : 1u; }
    }
}
inline void Make3(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    u32 len = g.P(20) ? g.R(4) : g.R(60), mode = 0, start = 0, content = g.R(4);
    if (!g.P(50)) start = g.R(10);
    switch (f) {
    case 0: len = g.R(30); content = 0; start = 0; break;
    case 1: len = g.R(40); content = 1; break;
    case 2: len = g.R(40); content = 2; break;
    case 3: len = g.R(40); content = 3; break;
    case 4: len = 250 + g.R(50); mode = g.P(50) ? 1u : 0u; content = g.R(4); start = g.P(70) ? 0u : g.R(40); break;    // cerca y mas alla del limite de 255 caracteres
    case 5: len = 255 + g.R(3); mode = 0; start = 0; content = g.R(4); break;                                          // longitudes 255..257
    case 6: len = 254 + g.R(3); mode = 1; start = 0; break;                                                              // sin terminador, 254..256
    case 7: mode = 2; len = g.R(8); start = 0; break;                                                                    // puntero nulo
    case 8: len = 1 + g.R(60); start = 1 + g.R(len); break;                                                              // desplazamiento dentro de la cadena
    case 9: len = g.R(60); start = len + g.R(5); break;                                                                  // desplazamiento en o mas alla del terminador
    case 10: len = 20 + g.R(40); content = 1; start = g.R(10); break;
    case 11: len = 0; start = g.P(50) ? 0u : g.R(4); break;                                                              // cadena vacia
    case 12: len = 40; content = 0; start = g.R(3); break;
    case 13: len = g.R(60); content = g.R(4); start = g.Pk({0u, 0x80u, 0xFFu, 0x7Fu, 1u}); if (start >= 0x80) len = start + g.R(40); break;   // desplazamientos grandes (hasta 255)
    case 14: len = 100 + g.R(150); content = 0; start = g.R(20); break;
    default: break;
    }
    if (len > 330) len = 330;
    if (start > 255) start = 255;
    c.a[0] = len; c.a[1] = mode; c.a[2] = start;
    c.a[3] = g.P(50) ? 0u : (g.W() & 0xFFFFFF00u);
    c.a[4] = content;
    c.a[5] = g.W();
}
// bytes de la cadena (sin terminador): un byte distinto de cero por posicion; el banco coloca el terminador y rellena el resto de la ventana
inline std::vector<u8> StrBytes(const C& c) {
    Gen g(c.n, 1000);
    std::vector<u8> out;
    for (u32 i = 0; i < c.a[0]; ++i) out.push_back(u8(GxtByte(g, c.a[4])));
    return out;
}

// ------------------------------------------------------------------------------------------------ funcion 4: CEventKnockOffBike (cuerpo del destructor, 0x4AFE70)
//   a[0] = vehiculo (+0x38: 0 arena de entidad, 1 nulo, 2 crudo 0x00BADC00), a[1] = peaton (+0x30: 0 arena, 1 nulo, 2 crudo 0x00BADC40), a[2] = vtable inicial (dword)
inline void Make4(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(70) ? 0u : g.R(3); c.a[1] = g.P(70) ? 0u : g.R(3);
    c.a[2] = g.P(50) ? 0x0085B2E0u : g.W();
    switch (f) {
    case 0: c.a[0] = 0; c.a[1] = 0; break;
    case 1: c.a[0] = 1; c.a[1] = 1; break;
    case 2: c.a[0] = 0; c.a[1] = 1; break;
    case 3: c.a[0] = 1; c.a[1] = 0; break;
    case 4: c.a[0] = 2; c.a[1] = 2; break;
    case 5: c.a[0] = 2; c.a[1] = 0; break;
    case 6: c.a[0] = 0; c.a[1] = 2; break;
    case 7: c.a[2] = 0x0085AB38u; break;
    case 8: c.a[2] = 0u; break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 5: CTaskSimpleStandStill (cuerpo del destructor, 0x62F360)
//   a[0] = vtable inicial (dword)
inline void Make5(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(50) ? 0x0086DD2Cu : g.W();
    if (f == 1) c.a[0] = 0u;
    if (f == 2) c.a[0] = 0x0086D48Cu;
    if (f == 3) c.a[0] = 0xFFFFFFFFu;
}

// ------------------------------------------------------------------------------------------------ funcion 6: RwIm2DRenderPrimitiveThunk (0x734E90)
//   a[0] = tipo de primitiva, a[1] = vertices (0 arena de vertices, 1 nulo, 2 crudo 0x00C80468), a[2] = numero de vertices, a[3] = lo que devuelve la funcion del dispositivo falso
inline void Make6(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(70) ? 1u + g.R(5) : g.W();
    c.a[1] = g.P(60) ? 0u : g.R(3);
    c.a[2] = g.P(70) ? g.R(300) : g.W();
    c.a[3] = g.P(50) ? g.R(2) : g.W();
    if (f == 0) { c.a[0] = 5; c.a[1] = 2; c.a[2] = 4; }
    if (f == 1) { c.a[3] = 0; }
    if (f == 2) { c.a[3] = 1; }
    if (f == 3) { c.a[2] = 0; }
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
    case 3: Make3(c, n); break;
    case 4: Make4(c, n); break;
    case 5: Make5(c, n); break;
    default: Make6(c, n); break;
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

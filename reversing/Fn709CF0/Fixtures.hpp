#pragma once
// Fixtures del banco Fn709CF0 (caso CASO-CONGELADOS, parte B): 16384 casos de CShadows::ShadowRenderTriangleCB (0x709CF0, __usercall: EAX = normal, EBX = triangulo, EDI = parametros).
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// La funcion del EXE se invoca por su direccion con un envoltorio propio del banco (reversing/, fuera de source/game_sa); la version C++ (Shadows.cpp, cdecl) se invoca directamente.
// Colaboradores (los mismos falsos en el EXE y en el cuerpo de la DLL): RwV3dTransformPoints (0x7EDD90), RenderBuffer::StartStoring (0x708290) y StopStoring (0x7077A0). El falso de
// RwV3dTransformPoints escribe las dos tablas de 9 floats que da el caso (primera llamada = mundo, segunda = proyectado), de modo que se recorren todas las ramas de la funcion.
//
// Familia = n % 16: colocacion de los objetos = familia % 4 (0 en tres arenas distintas, 1 en la misma arena sin solaparse, 2 con solapes de solo lectura, 3 al borde de la ventana) y relleno
// de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F709Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kParamBytes = 0x98;     // _ProjectionParam
constexpr std::size_t kVertexBytes = 0x24;    // RwIm3DVertex

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
    std::string End() const { return s + "}"; }
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

inline u32 Bits(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
inline float FromBits(u32 b) { float f; std::memcpy(&f, &b, 4); return f; }

// ------------------------------------------------------------------------------------------------ valores
// Valores especiales (bits de float): ceros con signo, unos, desnormales, limites, infinitos, NaN (silenciosos, con carga util y senalizadores)
inline constexpr std::array<u32, 28> kSpecial{
    0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x40000000u, 0xC0000000u,
    0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x80800000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F800000u,
    0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7FC00123u, 0x7F800001u, 0xFF800123u, 0x3F7FFFFFu, 0x3F800001u,
    0x3F7FFFFEu, 0x3F800002u, 0x7E967699u, 0x01000000u};
inline u32 Special(Gen& g) { return kSpecial[g.R(u32(kSpecial.size()))]; }
inline u32 Sane(Gen& g) { return Bits(float(int(g.R(2001)) - 1000) / 16.0f); }
inline u32 Unit(Gen& g) { return Bits(float(g.R(1025)) / 1024.0f); }                      // [0, 1]
inline u32 Inside(Gen& g) { return Bits((float(g.R(1022)) + 1.0f) / 1024.0f); }          // (0, 1)
inline u32 Below0(Gen& g) {                                                                 // estrictamente < 0 (o -0)
    switch (g.R(6)) { case 0: return 0x80000000u; case 1: return 0x80000001u; case 2: return 0xFF800000u; case 3: return 0xFF7FFFFFu; case 4: return Bits(-float(g.R(2000) + 1) / 1024.0f); default: return Bits(-float(g.R(100) + 1)); }
}
inline u32 Above1(Gen& g) {                                                                 // estrictamente > 1
    switch (g.R(6)) { case 0: return 0x3F800001u; case 1: return 0x7F800000u; case 2: return 0x7F7FFFFFu; case 3: return Bits(1.0f + float(g.R(2000) + 1) / 1024.0f); case 4: return 0x40000000u; default: return Bits(float(g.R(100) + 2)); }
}
inline u32 Any(Gen& g) {
    switch (g.R(6)) { case 0: case 1: return Special(g); case 2: return Inside(g); case 3: return Below0(g); case 4: return Above1(g); default: return Sane(g); }
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 16> a{};
    std::array<u32, 3> normal{};
    std::array<u32, 9> tri{};
    std::array<u32, 3> at{};
    std::array<u32, 9> world{};   // salida de la primera llamada de RwV3dTransformPoints
    std::array<u32, 9> proj{};    // salida de la segunda llamada
    u32 shadowDword{}, fadeDword{}, batchDword{};
};

//   a[0] = desplazamiento de los parametros, a[1] = colocacion (familia % 4), a[2] = relleno, a[3] = desplazamiento de la normal, a[4] = desplazamiento del triangulo,
//   a[5] = clase del producto escalar, a[6] = escenario de la proyeccion, a[7] = clase de fade, a[8] = clase del valor de sombra, a[9] = desplazamiento del buffer de indices (arena D),
//   a[10] = desplazamiento del buffer de vertices (arena D), a[11] = alias (solo colocacion 2), a[12..15] = semillas de las arenas A, B, C y D
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100);
    const u32 kind = f % 4;
    const u32 win = u32(kWin);
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    // colocacion: normal 12 bytes, triangulo 36 bytes, parametros 0x98 bytes
    switch (kind) {
    case 0:   // tres arenas distintas
        c.a[0] = 4 * g.R((win - kParamBytes) / 4 + 1);
        c.a[3] = 4 * g.R((win - 12) / 4 + 1);
        c.a[4] = 4 * g.R((win - 36) / 4 + 1);
        break;
    case 1: { // la misma arena (A), sin solaparse: parametros en la primera mitad, normal y triangulo en la segunda, separados
        c.a[0] = 4 * g.R((win / 2 - kParamBytes) / 4);
        c.a[3] = win / 2 + 4 * g.R(32);
        c.a[4] = win / 2 + 64 + 4 * g.R(((win / 2) - 64 - 36) / 4);
        break;
    }
    case 2: { // solapes de solo lectura (arena A): normal == triangulo, triangulo dentro de los parametros, normal dentro de los parametros o desalineados
        c.a[11] = g.R(4);
        c.a[0] = 8 + g.R(win - kParamBytes - 16);
        c.a[4] = g.R(2) ? c.a[0] + g.R(kParamBytes - 36 + 1) : 8 + g.R(win - 52);
        c.a[3] = c.a[11] == 0 ? c.a[4] : (c.a[11] == 1 ? c.a[0] + g.R(kParamBytes - 12 + 1) : (c.a[11] == 2 ? c.a[4] + g.R(25) : 8 + g.R(win - 28)));
        break;
    }
    default: { // al borde de la ventana: la lectura entra en la guarda inicial o final (arenas distintas)
        const auto edge = [&](u32 size) -> u32 { return g.P(50) ? u32(-i32(g.R(0x28))) : win - size + g.R(0x28); };   // desplazamientos con signo: la lectura entra en la guarda inicial o final
        c.a[0] = edge(u32(kParamBytes)); c.a[3] = edge(12); c.a[4] = edge(36);
        break;
    }
    }
    // buffers de la arena D (sin solaparse con las entradas: los vertices se escriben, ver README.md)
    c.a[9] = 2 * g.R(0xF8);
    c.a[10] = 0x200 + (g.P(60) ? 4 * g.R(0x80) : g.R(0x100));
    if (g.P(10)) {   // un buffer al final de la ventana (la escritura entra en la guarda final), el otro en su zona habitual: nunca se solapan
        if (g.P(50)) c.a[10] = win - 3 * u32(kVertexBytes) + g.R(0x30);
        else c.a[9] = win - 6 + g.R(0x30);
    }

    // ---- producto escalar: ((ay*ny) + (az*nz)) + (ax*nx) > 0 descarta el triangulo
    const u32 dc = g.R(10); c.a[5] = dc;
    const auto sane3 = [&](std::array<u32, 3>& v) { for (auto& x : v) x = g.P(8) ? Special(g) : Sane(g); };
    sane3(c.at); sane3(c.normal);
    switch (dc) {
    case 0: for (int i = 0; i < 3; ++i) c.normal[i] = c.at[i] ^ 0x80000000u; break;                  // normal = -at: producto <= 0 (menos con NaN/inf)
    case 1: for (int i = 0; i < 3; ++i) c.normal[i] = c.at[i]; break;                                 // normal = at: producto >= 0
    case 2: c.normal = {0u, 0x80000000u, 0u}; break;                                                  // ceros
    case 3: c.normal[g.R(3)] = Special(g); break;                                                      // un valor especial (NaN, infinito...)
    case 4: {                                                                                          // casi cancelacion: ay*ny + az*nz ~ -(ax*nx)
        const double p = double(FromBits(c.at[1])) * double(FromBits(c.normal[1]));
        const double q = double(FromBits(c.at[2])) * double(FromBits(c.normal[2]));
        c.normal[0] = Bits(1.0f);
        const float s = float(p + q);
        c.at[0] = Bits(-s + (g.P(50) ? 0.0f : float(g.R(3)) * std::ldexp(std::fabs(s), -23)));
        break;
    }
    case 5: for (int i = 0; i < 3; ++i) { c.at[i] = Bits(float(int(g.R(9)) - 4)); c.normal[i] = Bits(float(int(g.R(9)) - 4)); } break;   // enteros pequenos
    default: break;
    }
    for (auto& x : c.tri) x = g.P(10) ? Special(g) : Sane(g);

    // ---- salida de la primera llamada (mundo) y de la segunda (proyectado)
    for (auto& x : c.world) x = g.P(25) ? Special(g) : Sane(g);
    if (g.P(10)) c.world[2] = 0x7F7FFFFFu - g.R(4);
    if (g.P(6)) c.world[5] = 0xFF7FFFFFu + g.R(4);
    const u32 sc = g.R(12); c.a[6] = sc;
    switch (sc) {
    case 0: case 1: case 2:                                                                            // todo dentro de (0, 1), z con mezcla (alfa)
        for (int v = 0; v < 3; ++v) { c.proj[v * 3] = Inside(g); c.proj[v * 3 + 1] = Inside(g); c.proj[v * 3 + 2] = g.P(70) ? Inside(g) : Any(g); }
        c.proj[g.R(3) * 3 + 2] = g.P(50) ? Unit(g) : Inside(g);
        break;
    case 3: for (int v = 0; v < 3; ++v) { c.proj[v * 3] = Inside(g); c.proj[v * 3 + 1] = Inside(g); c.proj[v * 3 + 2] = Below0(g); } break;   // z siempre < 0
    case 4: for (int v = 0; v < 3; ++v) { c.proj[v * 3] = Below0(g); c.proj[v * 3 + 1] = Inside(g); c.proj[v * 3 + 2] = Inside(g); } break;   // x siempre < 0
    case 5: for (int v = 0; v < 3; ++v) { c.proj[v * 3] = Above1(g); c.proj[v * 3 + 1] = Inside(g); c.proj[v * 3 + 2] = Inside(g); } break;   // x siempre > 1
    case 6: for (int v = 0; v < 3; ++v) { c.proj[v * 3] = Inside(g); c.proj[v * 3 + 1] = Below0(g); c.proj[v * 3 + 2] = Inside(g); } break;   // y siempre < 0
    case 7: for (int v = 0; v < 3; ++v) { c.proj[v * 3] = Inside(g); c.proj[v * 3 + 1] = Above1(g); c.proj[v * 3 + 2] = Inside(g); } break;   // y siempre > 1
    case 8: {                                                                                          // cada grupo lo cumple un solo vertice (elegido al azar o ninguno)
        for (int v = 0; v < 3; ++v) { c.proj[v * 3] = Inside(g); c.proj[v * 3 + 1] = Inside(g); c.proj[v * 3 + 2] = Inside(g); }
        const u32 zv = g.R(4), xlo = g.R(4), xhi = g.R(4), ylo = g.R(4), yhi = g.R(4);
        for (int v = 0; v < 3; ++v) {
            if (u32(v) != zv) c.proj[v * 3 + 2] = Below0(g);
            if (u32(v) != xlo) c.proj[v * 3] = Below0(g);
            else if (u32(v) != xhi) c.proj[v * 3] = Above1(g);
            if (u32(v) != ylo) c.proj[v * 3 + 1] = Below0(g);
            else if (u32(v) != yhi) c.proj[v * 3 + 1] = Above1(g);
        }
        break;
    }
    case 9: for (auto& x : c.proj) x = g.P(50) ? Special(g) : Unit(g); break;                         // frontera (0, -0, 1, NaN, ...)
    case 10: for (auto& x : c.proj) x = Any(g); break;
    default: for (auto& x : c.proj) x = g.P(50) ? Sane(g) : Inside(g); break;
    }
    // alfa: z de cada vertice a veces exactamente 1, > 1, 0 o NaN
    for (int v = 0; v < 3; ++v) if (g.P(20)) { static constexpr u32 zs[8] = {0x3F800000u, 0xBF800000u, 0x3FC00000u, 0x00000000u, 0x7FC00000u, 0x3F7FFFFFu, 0x3F800001u, 0x7F800000u}; c.proj[v * 3 + 2] = zs[g.R(8)]; }

    // ---- sombra: byte +0x4C con basura en los 24 bits altos del dword; fade (dword completo); numIm3DBatch
    c.a[8] = g.R(6);
    static constexpr u32 sv[6] = {0u, 255u, 128u, 1u, 100u, 200u};
    const u32 junk = g.W() & 0xFFFFFF00u;
    c.shadowDword = (c.a[8] < 4 ? sv[c.a[8]] : (g.W() & 0xFFu)) | junk;
    c.a[7] = g.R(8);
    static constexpr u32 fd[8] = {0u, 1u, 2u, 0x100u, 0x80000000u, 0xFFFFFFFFu, 1u, 0u};
    c.fadeDword = c.a[7] < 6 ? fd[c.a[7]] : g.W();
    c.batchDword = g.W();
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
    j.A("normal", std::vector<u32>(c.normal.begin(), c.normal.end()));
    j.A("tri", std::vector<u32>(c.tri.begin(), c.tri.end()));
    j.A("at", std::vector<u32>(c.at.begin(), c.at.end()));
    j.A("world", std::vector<u32>(c.world.begin(), c.world.end()));
    j.A("proj", std::vector<u32>(c.proj.begin(), c.proj.end()));
    j.U("shadow_dword", c.shadowDword).U("fade_dword", c.fadeDword).U("batch_dword", c.batchDword);
    return j.End();
}
}

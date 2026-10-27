#pragma once
// Fixtures del banco CarLink74 (lote 74): CCarCtrl::ClipTargetOrientationToLink (EXE 0x422760), 18432 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::ClipTargetOrientationToLink  0x422760  cdecl, `ret` (CVehicle*, CCarPathLinkAddress, int8, float*, float, float)   (hook CCarCtrl / ClipTargetOrientationToLink, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n / 1024 (18 familias de 1024 casos).
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace Cl74Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kFamilies = 18;
constexpr unsigned kCases = kFamilies * 1024;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x40, kMatWin = 0x48, kTgtWin = 0x40, kLinkWin = 0x20;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kMatArena = Arena(kMatWin), kTgtArena = Arena(kTgtWin), kLinkArena = Arena(kLinkWin);

constexpr u32 kPathNodesAddr = 0x96F854;   // ThePaths.m_pPathNodes[]
constexpr u32 kNaviNodesAddr = 0x96F974;   // ThePaths.m_pNaviNodes[]
constexpr u32 kPathSlots = 64;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 74074u); }
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

// ------------------------------------------------------------------------------------------------ JSON minimo
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ valores notables
// NaN canonicos y no canonicos (carga distinta, sNaN con y sin signo, carga maxima y minima) e infinitos
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFA00000u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00001u, 0xFFBFFFFFu};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};   // +-0, desnormales y el menor normal
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
inline constexpr u32 kPiBits = 0x40490FDBu, kTwoPiBits = 0x40C90FDBu, kNegPiBits = 0xC0490FDBu;
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
inline u32 ExpFloat(u32 c, u32 k, int e) { const u32 r = Rand(c, k); return ((r >> 31) << 31) | (u32(e + 127) << 23) | (Rand(c, k + 500) & 0x7FFFFFu); }
inline u32 Denorm(u32 c, u32 k) { const u32 r = Rand(c, k); return ((r >> 31) << 31) | ((Rand(c, k + 500) & 0x7FFFFFu) | 1u); }
// float uniforme en [lo, hi] (resolucion de 2^-20 del intervalo)
inline u32 Uni(u32 c, u32 k, float lo, float hi) { return B(lo + (hi - lo) * (float(Rand(c, k) & 0xFFFFFu) / 1048575.0f)); }
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Un float de posicion/argumento: modo 0 moderado, 1 pequeno (rango desnormal), 2 especial (cero, desnormal, inf, NaN), 3 mezcla
inline u32 FloatOf(u32 c, u32 k, u32 mode) {
    const u32 r = Rand(c, k) % 32;
    switch (mode) {
    case 0: return r == 0 ? B(0.0f) : Uni(c, k + 2, -3000.0f, 3000.0f);
    case 1: return r < 3 ? Pick(kTiny, c, k + 1) : RandFloat(c, k + 2, -80, -20);
    case 2:
        if (r < 8) return Pick(kTiny, c, k + 1);
        if (r < 13) return Pick(kInf, c, k + 1);
        if (r < 21) return Pick(kNan, c, k + 1);
        if (r < 24) return Pick(kBig, c, k + 1);
        return RandFloat(c, k + 2, -20, 100);
    default: {
        const u32 m = Rand(c, k + 3) % 4;
        return FloatOf(c, k + 5, m == 3 ? 2u : m == 2 ? 1u : 0u);
    }
    }
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 area{}, idx{};     // direccion del enlace: area (6 bits), indice (10 bits)
    u32 hi16{};            // 16 bits altos basura del dword de la direccion
    u32 nullNode{};        // 1 = ThePaths.m_pPathNodes[area] nulo: el EXE no hace nada
    u32 dirx{}, diry{};    // bytes +8 y +9 del enlace (int8)
    u32 width{};           // byte +0xA
    u32 flags{};           // byte +0xB
    u32 arg3{};            // dword completo (el EXE lee solo el byte bajo con signo)
    u32 hasMat{};
    u32 mpos[2]{}, ppos[2]{};   // posicion de la matriz (+0x30, +0x34) y compacta (+4, +8)
    u32 arg5{}, arg6{};
    u32 old{};             // *arg4 inicial
    u32 ret[3]{};          // retornos del falso de GetATanOfXY en la llamada 0, 1, 2
    u32 real{};            // 1 = atan real en ambos lados (sin falso)
};

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n / 1024;
    const u32 m = n % 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.area = Rand(n, 10) % 64; c.idx = Rand(n, 11) % 1024;
    c.hi16 = Rand(n, 12) % 3 == 0 ? 0u : Rand(n, 13) & 0xFFFFu;
    static constexpr std::array<u32, 6> dirs{0u, 127u, 0x80u, 0xFFu, 1u, 0x81u};   // 0, 127, -128, -1, 1, -127
    c.dirx = Rand(n, 14) % 4 == 0 ? Pick(dirs, n, 15) : Rand(n, 16) & 0xFFu;
    c.diry = Rand(n, 17) % 4 == 0 ? Pick(dirs, n, 18) : Rand(n, 19) & 0xFFu;
    c.width = Rand(n, 20) % 5 == 0 ? Pick(std::array<u32, 4>{0u, 255u, 1u, 128u}, n, 21) : Rand(n, 22) & 0xFFu;
    c.flags = Rand(n, 23) & 0xFFu;
    static constexpr std::array<u32, 6> a3{1u, 0xFFu, 0u, 0x7Fu, 0x80u, 0x01u};
    c.arg3 = (Rand(n, 24) % 2 ? Pick(a3, n, 25) : Rand(n, 26) & 0xFFu) | (Rand(n, 27) % 3 == 0 ? 0u : Rand(n, 28) & 0xFFFFFF00u);
    c.hasMat = Rand(n, 29) % 5 != 0;
    c.nullNode = 0;
    c.real = 0;
    auto pos = [&](u32 mode) { for (int i = 0; i < 2; ++i) { c.mpos[i] = FloatOf(n, 40 + i, mode); c.ppos[i] = FloatOf(n, 44 + i, mode); } };
    pos(0);
    const u32 refx = c.hasMat ? c.mpos[0] : c.ppos[0], refy = c.hasMat ? c.mpos[1] : c.ppos[1];
    c.arg5 = B(F(refx) + F(Uni(n, 50, -50.0f, 50.0f))); c.arg6 = B(F(refy) + F(Uni(n, 51, -50.0f, 50.0f)));
    c.old = Uni(n, 52, 0.0f, 6.2831855f);
    for (int i = 0; i < 3; ++i) c.ret[i] = Uni(n, 53 + i, 0.0f, 6.2831855f);
    // D objetivo: a = old + D
    auto from = [&](u32 k, float lo, float hi) { return B(F(c.old) + F(Uni(n, k, lo, hi))); };
    switch (c.family) {
    case 0:   // area de nodos no cargada: no hace nada (ni llama a 0x53CC70)
        c.nullNode = 1;
        break;
    case 1:   // normal
        break;
    case 2:   // D1 y D2 negativos (se suma el mayor)
        c.ret[1] = from(60, -3.1f, -0.001f); c.ret[2] = from(61, -3.1f, -0.001f);
        if (Rand(n, 62) % 8 == 0) c.ret[2] = c.ret[1];
        break;
    case 3:   // D1 y D2 positivos (se suma el menor)
        c.ret[1] = from(60, 0.001f, 3.1f); c.ret[2] = from(61, 0.001f, 3.1f);
        if (Rand(n, 62) % 8 == 0) c.ret[2] = c.ret[1];
        break;
    case 4:   // signos mezclados y ceros exactos
        switch (m % 6) {
        case 0: c.ret[1] = from(60, -3.0f, -0.01f); c.ret[2] = from(61, 0.01f, 3.0f); break;
        case 1: c.ret[1] = from(60, 0.01f, 3.0f); c.ret[2] = from(61, -3.0f, -0.01f); break;
        case 2: c.ret[1] = c.old; c.ret[2] = from(61, -3.0f, 3.0f); break;               // D1 = 0
        case 3: c.ret[2] = c.old; c.ret[1] = from(60, -3.0f, 3.0f); break;               // D2 = 0
        case 4: c.ret[1] = c.ret[2] = c.old; break;                                      // ambos 0
        default: c.ret[1] = c.old; c.ret[2] = c.old; c.old = Rand(n, 63) % 2 ? 0x80000000u : 0u; c.ret[1] = c.ret[2] = Rand(n, 64) % 2 ? 0x80000000u : 0u; break;   // ceros con signo
        }
        break;
    case 5: {  // vueltas de normalizacion de D1 y D2 y fronteras +-pi
        c.old = Uni(n, 52, -3.0f, 9.0f);
        c.ret[1] = Uni(n, 60, -40.0f, 40.0f); c.ret[2] = Uni(n, 61, -40.0f, 40.0f);
        switch (m % 8) {
        case 0: c.old = 0u; c.ret[1] = kPiBits; c.ret[2] = kNegPiBits; break;
        case 1: c.old = 0u; c.ret[1] = kNegPiBits; c.ret[2] = kPiBits; break;
        case 2: c.old = 0u; c.ret[1] = kPiBits + 1; c.ret[2] = kNegPiBits + 1; break;
        case 3: c.old = 0u; c.ret[1] = kPiBits - 1; c.ret[2] = kNegPiBits - 1; break;
        case 4: c.old = 0u; c.ret[1] = kTwoPiBits; c.ret[2] = B(-6.2831855f); break;
        case 5: c.old = 0u; c.ret[1] = B(F(kTwoPiBits) + F(kPiBits)); c.ret[2] = B(-F(kTwoPiBits) - F(kPiBits)); break;
        default: break;
        }
        break;
    }
    case 6:   // *arg4 final fuera de [0, 2pi]: viejo negativo, grande o en la frontera
        switch (m % 4) {
        case 0: c.old = Uni(n, 52, -100.0f, 100.0f); break;
        case 1: c.old = Uni(n, 52, -0.3f, 0.3f); break;
        case 2: c.old = Uni(n, 52, 6.0f, 6.6f); break;
        default: c.old = Pick(std::array<u32, 8>{0x80000000u, 0u, kTwoPiBits, kTwoPiBits + 1, kTwoPiBits - 1, 0x80000001u, B(-6.2831855f), B(12.566371f)}, n, 65); break;
        }
        c.ret[1] = from(60, -3.1f, 3.1f); c.ret[2] = from(61, -3.1f, 3.1f);
        break;
    case 7: {  // carriles: los 256 valores del byte +0xB por cuatro variantes de arg3 (0, 1, -1, aleatorio)
        c.flags = m / 4;
        const u32 sel = m % 4;
        c.arg3 = (sel == 0 ? 0u : sel == 1 ? 1u : sel == 2 ? 0xFFu : Rand(n, 66) & 0xFFu) | (Rand(n, 67) % 2 ? 0u : (Rand(n, 68) & 0xFFFFFF00u));
        c.width = Rand(n, 69) % 3 == 0 ? Pick(std::array<u32, 3>{0u, 255u, 128u}, n, 70) : Rand(n, 71) & 0xFFu;
        c.ret[1] = from(60, -3.1f, 3.1f); c.ret[2] = from(61, -3.1f, 3.1f);
        break;
    }
    case 8:   // posiciones y argumentos especiales (NaN no canonicos, infinitos, desnormales, ceros con signo)
        pos(2);
        c.arg5 = FloatOf(n, 80, 2); c.arg6 = FloatOf(n, 81, 2);
        if (Rand(n, 82) % 3 == 0) { c.arg5 = Rand(n, 83) % 2 ? c.mpos[0] : c.ppos[0]; }
        if (Rand(n, 84) % 3 == 0) { c.arg6 = Rand(n, 85) % 2 ? c.mpos[1] : c.ppos[1]; }
        c.ret[1] = from(60, -3.1f, 3.1f); c.ret[2] = from(61, -3.1f, 3.1f);
        break;
    case 9:   // magnitudes diminutas: productos y restas en el rango desnormal / exponente extendido
        pos(1);
        c.arg5 = Rand(n, 80) % 3 == 0 ? c.mpos[0] : FloatOf(n, 81, 1); c.arg6 = Rand(n, 82) % 3 == 0 ? c.mpos[1] : FloatOf(n, 83, 1);
        if (Rand(n, 84) % 2) { c.dirx = Rand(n, 85) % 2 ? 0u : c.dirx; c.diry = Rand(n, 86) % 2 ? 0u : c.diry; }
        c.ret[1] = from(60, -3.1f, 3.1f); c.ret[2] = from(61, -3.1f, 3.1f);
        break;
    case 10:  // retornos del atan especiales (NaN no canonicos, ceros con signo, desnormales, 2 pi) con *arg4 normal o especial
        for (int i = 0; i < 3; ++i) c.ret[i] = Rand(n, 90 + i) % 2 ? Pick(kNan, n, 93 + i) : Rand(n, 96 + i) % 2 ? Pick(kTiny, n, 99 + i) : Pick(std::array<u32, 6>{kPiBits, kNegPiBits, kTwoPiBits, 0x3F800000u, 0xBF800000u, B(1.5f)}, n, 102 + i);
        if (Rand(n, 105) % 3 == 0) c.ret[Rand(n, 106) % 3] = from(107, -3.0f, 3.0f);
        break;
    case 11:  // *arg4 inicial especial (NaN, ceros, desnormales, pi, 2 pi, algo grande pero finito)
        c.old = Rand(n, 90) % 3 == 0 ? Pick(kNan, n, 91) : Rand(n, 92) % 2 ? Pick(kTiny, n, 93) : Pick(std::array<u32, 8>{kPiBits, kNegPiBits, kTwoPiBits, kTwoPiBits + 1, kTwoPiBits - 1, B(100.0f), B(-100.0f), B(1000.0f)}, n, 94);
        c.ret[1] = Uni(n, 60, -3.2f, 6.4f); c.ret[2] = Uni(n, 61, -3.2f, 6.4f);
        break;
    case 12:  // D diminutos: a = viejo +- unos ulp (D desnormal o cero), viejo cero o desnormal
        c.old = Rand(n, 90) % 2 ? Pick(kTiny, n, 91) : Uni(n, 92, 0.0f, 6.2831855f);
        for (int i = 1; i < 3; ++i) {
            const u32 k = Rand(n, 93 + i) % 4;
            c.ret[i] = k == 0 ? c.old + Rand(n, 96 + i) % 5u : k == 1 ? c.old - Rand(n, 96 + i) % 5u : k == 2 ? (c.old ^ 0x80000000u) : Pick(kTiny, n, 98 + i);
        }
        break;
    case 13:  // sin matriz: posicion compacta (+4)
        c.hasMat = 0;
        c.arg5 = B(F(c.ppos[0]) + F(Uni(n, 50, -50.0f, 50.0f))); c.arg6 = B(F(c.ppos[1]) + F(Uni(n, 51, -50.0f, 50.0f)));
        c.ret[1] = from(60, -3.1f, 3.1f); c.ret[2] = from(61, -3.1f, 3.1f);
        break;
    case 14:  // atan REAL en ambos lados, valores moderados
        c.real = 1;
        c.old = Uni(n, 52, 0.0f, 6.2831855f);
        break;
    case 15:  // atan REAL con ceros con signo, destino coincidente con la posicion y desplazamientos minimos
        c.real = 1;
        pos(0);
        c.hasMat = Rand(n, 90) % 2;
        {
            const u32 rx = c.hasMat ? c.mpos[0] : c.ppos[0], ry = c.hasMat ? c.mpos[1] : c.ppos[1];
            c.arg5 = Rand(n, 91) % 2 ? rx : B(F(rx) + F(Uni(n, 92, -0.5f, 0.5f)));
            c.arg6 = Rand(n, 93) % 2 ? ry : B(F(ry) + F(Uni(n, 94, -0.5f, 0.5f)));
            if (Rand(n, 95) % 4 == 0) { c.mpos[0] = c.ppos[0] = Rand(n, 96) % 2 ? 0u : 0x80000000u; c.mpos[1] = c.ppos[1] = Rand(n, 97) % 2 ? 0u : 0x80000000u; c.arg5 = Rand(n, 98) % 2 ? 0u : 0x80000000u; c.arg6 = Rand(n, 99) % 2 ? 0u : 0x80000000u; }
        }
        c.old = Rand(n, 100) % 4 == 0 ? Pick(kTiny, n, 101) : Uni(n, 52, -1.0f, 7.0f);
        if (Rand(n, 102) % 3 == 0) c.dirx = c.diry = 0;
        break;
    case 16:  // extremos de arg3 y de la direccion del enlace (-128, 127, 0), con basura en los bytes altos
        c.arg3 = Pick(std::array<u32, 8>{0u, 1u, 0xFFu, 0x7Fu, 0x80u, 0x81u, 2u, 0xFEu}, n, 90) | (Rand(n, 91) & 0xFFFFFF00u);
        c.dirx = Pick(dirs, n, 92); c.diry = Pick(dirs, n, 93);
        c.ret[1] = from(60, -3.1f, 3.1f); c.ret[2] = from(61, -3.1f, 3.1f);
        break;
    default:  // 17: mezcla (posiciones con rangos mixtos, retornos y viejo moderados)
        pos(3);
        c.arg5 = FloatOf(n, 80, 3); c.arg6 = FloatOf(n, 81, 3);
        c.old = Rand(n, 90) % 6 == 0 ? Pick(kTiny, n, 91) : Uni(n, 52, -7.0f, 14.0f);
        for (int i = 0; i < 3; ++i) c.ret[i] = Uni(n, 93 + i, -7.0f, 14.0f);
        break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("area", c.area).U("idx", c.idx).U("hi16", c.hi16).U("null_node", c.nullNode).U("dirx", c.dirx).U("diry", c.diry).U("width", c.width).U("flags", c.flags).U("arg3", c.arg3).U("has_mat", c.hasMat);
    j.U("mpos_x", c.mpos[0]).U("mpos_y", c.mpos[1]).U("ppos_x", c.ppos[0]).U("ppos_y", c.ppos[1]).U("arg5", c.arg5).U("arg6", c.arg6).U("old", c.old);
    j.U("ret0", c.ret[0]).U("ret1", c.ret[1]).U("ret2", c.ret[2]).U("real", c.real);
    return j.End();
}
}

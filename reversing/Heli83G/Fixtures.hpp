#pragma once
// Fixtures del banco Heli83G (lote 83G): CHeli::SearchLightCone (EXE 0x6C58E0), 18432 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CHeli::SearchLightCone  0x6C58E0  cdecl, 19 dwords (coronaIndex, origen(3), objetivo(3), targetRadius, power, clipIfColliding (dword, no se lee), drawShadow (dword, solo el byte bajo),
//                                       &useless0, &useless1, &useless2, a11, baseRadius, a13, a14, a15 (a11, a13..a15 no se leen)), `ret`, void (hook CHeli / SearchLightCone, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) y las familias 16 y 17 (1024 casos cada una).
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

namespace H83GFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;    // familias 0..15
constexpr unsigned kExtraFamilies = 2;    // familias 16..17, 1024 casos cada una
constexpr unsigned kCases = kBaseCases + kExtraFamilies * 1024;
constexpr unsigned kFamilies = 16 + kExtraFamilies;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kOutWin = 0x30, kCamWin = 0x48;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kOutArena = Arena(kOutWin), kCamArena = Arena(kCamWin);

constexpr u32 kCamMatrixPtr = 0xB6F03C;                   // TheCamera.m_matrix (CPlaceable +0x14)
constexpr u32 kCamPlacement = 0xB6F02C;                   // TheCamera.m_placement.m_vPosn (CPlaceable +4), 12 bytes
constexpr u32 kShadowTex = 0xC403F4;                      // gpShadowExplosionTex
constexpr u32 kTempCounters = 0xC4B950, kTempCountersBytes = 8;       // vertices, indices
constexpr u32 kTempIndices = 0xC4B958, kTempIndicesBytes = 512;       // 240 indices usados
constexpr u32 kTempVertices = 0xC4D958, kTempVerticesBytes = 3072;    // 82 vertices de 36 bytes usados
constexpr unsigned kVertexStride = 36;
constexpr unsigned kMaxVertices = 82;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 83095u); }
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
    Js& A(const char* k, const u32* v, int count) { Key(k); s += '['; for (int i = 0; i < count; ++i) { if (i) s += ','; s += std::to_string(v[i]); } s += ']'; return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ valores notables
// Un unico patron de NaN (el indefinido de la x87): con dos NaN distintos en una operacion la carga que sale depende del orden de operandos que elija el compilador
inline constexpr std::array<u32, 4> kNan{0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Un float especial: cero, desnormal, infinito, NaN, enorme o corriente
inline u32 SpecialFloat(u32 c, u32 k) {
    const u32 r = Rand(c, k) % 32;
    if (r < 8) return Pick(kTiny, c, k + 1);
    if (r < 13) return Pick(kInf, c, k + 1);
    if (r < 21) return Pick(kNan, c, k + 1);
    if (r < 24) return Pick(kBig, c, k + 1);
    return RandFloat(c, k + 2, -20, 30);
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    // argumentos
    u32 corona{}, o[3]{}, t[3]{}, radius{}, power{}, clip{}, shadow{}, a11{}, base{}, a13{}, a14{}, a15{};
    // globales
    u32 camNull{}, camPos[3]{}, camMat[3]{}, tex{};
    // colaboradores falseados
    u32 los{}, col[3]{}, transformOk{};
};

inline void Split3(u32 c, u32 k, double len, double out[3]) {
    double a = double(Rand(c, k) % 2001) / 1000.0 - 1.0;
    double b = double(Rand(c, k + 1) % 2001) / 1000.0 - 1.0;
    double d = double(Rand(c, k + 2) % 2001) / 1000.0 - 1.0;
    double m = a * a + b * b + d * d;
    if (m < 1e-6) { a = 0.0; b = 0.0; d = -1.0; m = 1.0; }
    const double s = len / std::sqrt(m);
    out[0] = a * s; out[1] = b * s; out[2] = d * s;
}

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.ecx = SeedReg(n, 3); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    auto logmag = [&](u32 k, double lo, double hi) { return std::pow(10.0, std::log10(lo) + double(Rand(n, k) % 2001) / 2000.0 * (std::log10(hi) - std::log10(lo))); };
    auto fr = [&](u32 k, double lo, double hi) { return lo + double(Rand(n, k) % 100001) / 100000.0 * (hi - lo); };
    // ---- base: foco plausible (el helicoptero a unas decenas de metros sobre el objetivo, o un foco de guion)
    double o[3], d[3];
    for (int i = 0; i < 3; ++i) o[i] = double(int(Rand(n, 10 + i) % 60001) - 30000) / 10.0;
    Split3(n, 14, 1.0, d);
    if (Rand(n, 17) % 3 != 0) { d[2] = -std::fabs(d[2]) - 0.3; const double m = std::sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]); for (double& v : d) v /= m; }
    const double dist = logmag(18, 3.0, 90.0);
    for (int i = 0; i < 3; ++i) { c.o[i] = B(float(o[i])); c.t[i] = B(float(o[i] + d[i] * dist)); }
    c.radius = B(float(logmag(19, 0.3, 12.0)));
    c.power = B(float(fr(20, 0.1, 2.0)));
    c.base = Rand(n, 21) % 6 == 0 ? B(0.0f) : B(float(logmag(22, 0.02, 1.6)));
    c.corona = Rand(n, 23) % 3 == 0 ? Rand(n, 24) : 0x216u + Rand(n, 25) % 64;
    c.clip = Rand(n, 26); c.a11 = Rand(n, 27); c.a13 = Rand(n, 28); c.a14 = Rand(n, 29); c.a15 = Rand(n, 30);
    c.shadow = Rand(n, 31) % 3 == 0 ? (Rand(n, 32) & 0xFFFFFF00u) : ((Rand(n, 33) & 0xFFFFFF00u) | (1u + Rand(n, 34) % 255u));
    {   // camara: a unas decenas de metros del objetivo (en el plano) o lejos; con matriz o sin ella
        double cam[3];
        const double radius = Rand(n, 35) % 2 ? fr(36, 0.0, 30.0) : fr(37, 20.0, 400.0);
        const double ang = fr(38, 0.0, 6.2831853);
        for (int i = 0; i < 3; ++i) cam[i] = double(F(c.t[i]));
        cam[0] += radius * std::cos(ang); cam[1] += radius * std::sin(ang); cam[2] += fr(39, -20.0, 60.0);
        c.camNull = Rand(n, 40) % 2;
        const u32 mx = Rand(n, 41) % 2;
        for (int i = 0; i < 3; ++i) { const u32 w = B(float(cam[i])); c.camPos[i] = mx ? w : B(float(cam[i] + 7.0 + i)); c.camMat[i] = mx ? B(float(cam[i] + 5.0 - i)) : w; }
        if (!c.camNull) { for (int i = 0; i < 3; ++i) c.camMat[i] = B(float(cam[i])); }
        else { for (int i = 0; i < 3; ++i) c.camPos[i] = B(float(cam[i])); }
    }
    c.tex = Rand(n, 42);
    c.los = Rand(n, 43) % 4 == 0;
    for (int i = 0; i < 3; ++i) c.col[i] = B(float(double(F(c.o[i])) + d[i] * dist * fr(44, 0.05, 0.99) + fr(45 + i, -0.5, 0.5)));
    c.transformOk = Rand(n, 48) % 8 != 0;
    // ---- por familia
    auto setT = [&](double len, const double dir[3]) { for (int i = 0; i < 3; ++i) c.t[i] = B(float(double(F(c.o[i])) + dir[i] * len)); };
    auto camNear = [&](double r) {   // camara a r metros (en el plano) del objetivo, a su altura aproximada
        const double ang = fr(49, 0.0, 6.2831853);
        const double tx = double(F(c.t[0])) + r * std::cos(ang), ty = double(F(c.t[1])) + r * std::sin(ang), tz = double(F(c.t[2])) + fr(50, -5.0, 30.0);
        const double v[3]{tx, ty, tz};
        for (int i = 0; i < 3; ++i) { c.camPos[i] = B(float(v[i])); c.camMat[i] = B(float(v[i] + 3.0)); }
        if (!c.camNull) for (int i = 0; i < 3; ++i) c.camMat[i] = B(float(v[i]));
        else for (int i = 0; i < 3; ++i) c.camPos[i] = B(float(v[i]));
    };
    switch (f) {
    case 0:   // foco corriente con sombra, camara cerca (sombra dibujada) o no
        c.shadow = (Rand(n, 100) & 0xFFFFFF00u) | (1u + Rand(n, 101) % 255u); c.los = 0; c.transformOk = 1;
        camNear(fr(102, 0.0, 22.0));
        c.base = B(float(logmag(103, 0.05, 1.5)));
        break;
    case 1:   // sin sombra (con bytes altos basura)
        c.shadow = Rand(n, 104) & 0xFFFFFF00u; camNear(fr(105, 0.0, 22.0));
        break;
    case 2:   // la linea de vision toca un edificio: el objetivo se recorta
        c.los = 1;
        for (int i = 0; i < 3; ++i) c.col[i] = B(float(double(F(c.o[i])) + d[i] * dist * fr(106, 0.02, 0.95)));
        camNear(fr(107, 0.0, 30.0));
        break;
    case 3:   // radio de la base nulo, negativo o NaN: sin segundos triangulos
        {
            static constexpr std::array<u32, 6> bv{0x00000000u, 0x80000000u, 0xBF800000u, 0xBDCCCCCDu, 0x00000001u, 0xFFC00000u};
            c.base = Rand(n, 108) % 4 == 0 ? Pick(bv, n, 109) : B(float(-logmag(110, 0.01, 2.0)));
            camNear(fr(111, 0.0, 22.0));
        }
        break;
    case 4:   // segmento lejano largo (mas de 100): se acorta; radio grande o direccion casi horizontal
        {
            double h[3];
            Split3(n, 112, 1.0, h);
            h[2] = -fr(113, 0.002, 0.2);
            const double m = std::sqrt(h[0] * h[0] + h[1] * h[1] + h[2] * h[2]);
            for (double& v : h) v /= m;
            setT(logmag(114, 3.0, 40.0), h);
            c.radius = B(float(logmag(115, 3.0, 200.0)));
            camNear(fr(116, 0.0, 30.0));
        }
        break;
    case 5:   // umbrales de la sombra: camara a ~25 del objetivo y longitudes de los vectores a ~100
        {
            static constexpr double rs[]{24.9, 25.0, 25.1, 24.999, 25.001, 24.0, 26.0, 0.0};
            camNear(rs[Rand(n, 117) % std::size(rs)] + fr(118, -0.002, 0.002));
            c.shadow = (Rand(n, 119) & 0xFFFFFF00u) | 1u;
            c.radius = B(float(logmag(120, 20.0, 120.0)));
            if (Rand(n, 121) % 2) { double h[3]; Split3(n, 122, 1.0, h); h[2] = -std::fabs(h[2]) - 0.5; const double m = std::sqrt(h[0] * h[0] + h[1] * h[1] + h[2] * h[2]); for (double& v : h) v /= m; setT(logmag(123, 5.0, 60.0), h); }
        }
        break;
    case 6:   // camara con matriz (posicion de la matriz) frente a sin matriz
        c.camNull = Rand(n, 124) % 2;
        camNear(fr(125, 0.0, 24.0));
        break;
    case 7:   // origen == objetivo o muy cerca (direccion nula: Normalise solo pone x = 1)
        {
            const u32 w = Rand(n, 126) % 4;
            if (w == 0) { for (int i = 0; i < 3; ++i) c.t[i] = c.o[i]; }
            else if (w == 1) { for (int i = 0; i < 3; ++i) c.t[i] = B(float(double(F(c.o[i])) + fr(127 + i, -1e-4, 1e-4))); }
            else if (w == 2) { for (int i = 0; i < 3; ++i) c.t[i] = c.o[i]; c.t[Rand(n, 130) % 3] = B(float(double(F(c.o[0])) + fr(131, -2.0, 2.0))); }
            else { for (int i = 0; i < 3; ++i) c.t[i] = B(0.0f); for (int i = 0; i < 3; ++i) c.o[i] = Rand(n, 132) % 2 ? B(0.0f) : B(-0.0f); }
            camNear(fr(133, 0.0, 22.0));
        }
        break;
    case 8:   // camara detras del foco (coseno negativo, acotado a 0) y delante
        {
            const double dir[3]{double(F(c.t[0])) - double(F(c.o[0])), double(F(c.t[1])) - double(F(c.o[1])), double(F(c.t[2])) - double(F(c.o[2]))};
            const double len = std::sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
            const double k = Rand(n, 134) % 2 ? -1.0 : 1.0;
            const double reach = fr(135, 5.0, 300.0);
            for (int i = 0; i < 3; ++i) {
                const double v = double(F(c.o[i])) + k * (len > 0 ? dir[i] / len : 0.0) * reach + fr(136 + i, -3.0, 3.0);
                if (c.camNull) c.camPos[i] = B(float(v)); else c.camMat[i] = B(float(v));
            }
        }
        break;
    case 9:   // Im3DTransform falla (no se dibuja) y combinaciones de bytes altos
        c.transformOk = 0; c.shadow = Rand(n, 140) & 0xFFFFFF01u; camNear(fr(141, 0.0, 22.0));
        break;
    case 10:   // potencia: 0, negativa, grande, NaN, desnormal
        {
            static constexpr std::array<u32, 8> pv{0x00000000u, 0x80000000u, 0xBF800000u, 0x40A00000u, 0x7F800000u, 0xFFC00000u, 0x00000001u, 0x3F800000u};
            c.power = Rand(n, 142) % 3 == 0 ? Pick(pv, n, 143) : B(float(fr(144, -3.0, 6.0)));
            c.shadow = (Rand(n, 145) & 0xFFFFFF00u) | 1u; camNear(fr(146, 0.0, 22.0));
        }
        break;
    case 11:   // objetivo vertical (direccion paralela a la vertical: el producto vectorial se anula)
        {
            const double dir[3]{0.0, 0.0, Rand(n, 147) % 2 ? -1.0 : 1.0};
            setT(logmag(148, 3.0, 60.0), dir);
            if (Rand(n, 149) % 3 == 0) { c.t[0] = B(float(double(F(c.o[0])) + fr(150, -0.01, 0.01))); }
            camNear(fr(151, 0.0, 25.0));
        }
        break;
    case 12:   // altura del objetivo igual a la del punto cercano (division entre cero) y vecinos
        {
            double h[3]{fr(152, -1.0, 1.0), fr(153, -1.0, 1.0), 0.0};
            const double m = std::sqrt(h[0] * h[0] + h[1] * h[1]);
            if (m < 1e-6) { h[0] = 1.0; h[1] = 0.0; } else { h[0] /= m; h[1] /= m; }
            setT(logmag(154, 3.0, 60.0), h);
            if (Rand(n, 155) % 2) c.t[2] = B(float(double(F(c.o[2])) + fr(156, -1e-3, 1e-3)));
            camNear(fr(157, 0.0, 25.0));
        }
        break;
    case 13:   // radios y alturas extremos
        {
            static constexpr std::array<u32, 6> rv{0x00000000u, 0x80000000u, 0x7F000000u, 0x00800000u, 0x3F800000u, 0x42C80000u};
            if (Rand(n, 158) % 2) c.radius = Pick(rv, n, 159); else c.radius = B(float(logmag(160, 0.001, 5000.0)));
            if (Rand(n, 161) % 2) c.base = Pick(rv, n, 162);
            camNear(fr(163, 0.0, 30.0));
        }
        break;
    case 14:   // aleatorio general (coordenadas del mundo y camara cualquiera)
        break;
    case 15:   // aleatorio con foco de guion: origen y objetivo con indices 0x216 + i
        c.corona = 0x216u + Rand(n, 164) % 8; c.shadow = (Rand(n, 165) & 0xFFFFFF00u) | 1u;
        camNear(fr(166, 0.0, 24.0));
        break;
    case 16:   // NaN e infinitos repartidos
        {
            u32* t[] = {&c.o[0], &c.o[1], &c.o[2], &c.t[0], &c.t[1], &c.t[2], &c.radius, &c.power, &c.base, &c.camPos[0], &c.camPos[1], &c.camPos[2], &c.camMat[0], &c.camMat[1], &c.camMat[2], &c.col[0], &c.col[1], &c.col[2]};
            for (int k = 0; k < 18; ++k) if (Rand(n, 170 + k) % 8 == 0) *t[k] = Rand(n, 200 + k) % 2 ? Pick(kNan, n, 230 + k) : Pick(kInf, n, 260 + k);
            c.shadow = (Rand(n, 290) & 0xFFFFFF00u) | 1u;
        }
        break;
    default:   // 17: desnormales, ceros y valores enormes
        {
            u32* t[] = {&c.o[0], &c.o[1], &c.o[2], &c.t[0], &c.t[1], &c.t[2], &c.radius, &c.power, &c.base, &c.camPos[0], &c.camPos[1], &c.camPos[2], &c.camMat[0], &c.camMat[1], &c.camMat[2], &c.col[0], &c.col[1], &c.col[2]};
            for (int k = 0; k < 18; ++k) {
                const u32 w = Rand(n, 300 + k) % 7;
                if (w == 0) *t[k] = Pick(kTiny, n, 330 + k); else if (w == 1) *t[k] = (Rand(n, 360 + k) & 0x807FFFFFu) | 1u; else if (w == 2) *t[k] = SpecialFloat(n, 390 + k);
            }
            c.shadow = (Rand(n, 420) & 0xFFFFFF00u) | 1u;
        }
        break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("corona", c.corona).A("o", c.o, 3).A("t", c.t, 3).U("radius", c.radius).U("power", c.power).U("clip", c.clip).U("shadow", c.shadow).U("a11", c.a11).U("base", c.base).U("a13", c.a13).U("a14", c.a14).U("a15", c.a15);
    j.U("cam_null", c.camNull).A("cam_pos", c.camPos, 3).A("cam_mat", c.camMat, 3).U("tex", c.tex);
    j.U("los", c.los).A("col", c.col, 3).U("transform_ok", c.transformOk);
    return j.End();
}
}

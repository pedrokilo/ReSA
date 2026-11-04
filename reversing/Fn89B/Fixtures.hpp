#pragma once
// Fixtures del banco Fn89B (lote 89B): una funcion, 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCamera::AvoidTheGeometry                              0x514030  thiscall (ECX = camara; source, target, out por puntero y fov en pila), `ret 10h`; void   (hook CCamera / AvoidTheGeometry, tipo S)
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

namespace F89BFix {
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
constexpr std::size_t kCamWin = 0x1000, kVecWin = 0x40, kEntWin = 0x40, kMatWin = 0x40, kFwdWin = 0x40, kPedWin = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kCamArena = Arena(kCamWin), kSrcArena = Arena(kVecWin), kTgtArena = Arena(kVecWin), kOutArena = Arena(kVecWin), kEntArena = Arena(kEntWin), kMatArena = Arena(kMatWin),
                      kFwdArena = Arena(kFwdWin), kPedArena = Arena(kPedWin);
// Desplazamientos (respecto al objeto de cada arena) de los vectores de entrada y de salida
constexpr std::size_t kVecOff = 0x10;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 89002u); }
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

// ------------------------------------------------------------------------------------------------ el caso
// Colaboradores falsos: ProcessLineOfSight (0x56BA00, dos llamadas), FindPlayerPed (0x56E210), RwCameraSetNearClipPlane (0x7EE1D0), TestSphereAgainstWorld (0x569E20), GetMatrix (0x411990, ped objetivo) y
// WellBufferMe (0x509AE0). Reales en ambos lados: GetATanOfXY (0x53CC70), CVector::Normalise (0x59C910), fsin/fcos/fptan, _CIpow (0x822130).
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 src[3]{}, tgt[3]{}, outInit[3]{};           // vectores de entrada (source, target) y contenido previo de out
    u32 fov{};                                      // float
    u32 head984{}, head988{};                       // rumbo de reserva (+0x984, +0x988)
    u32 timer{}, dirn{}, i2d{};                     // +0x164 (float), +0x168 (word), +0x2D (byte) iniciales
    u32 tgtNull{}, isPed{};                         // objetivo nulo / tipo de entidad del objetivo (+0x36)
    u32 tmat{}, tpos[3]{}, talt[3]{};               // matriz del objetivo y posicion activa y la otra
    u32 fwd[3]{};                                   // vector adelante de GetMatrix
    u32 timestep{}, b65{};                          // 0xB7CB5C (float), 0xB6EC65 (byte)
    u32 wbCur{}, wbSpd{}, wbOutCur{}, wbOutSpd{};   // 0xB6EC38 / 0xB6EC3C iniciales y lo que escribe WellBufferMe
    u32 aspect{}, nearPlane{};                      // 0xC3EFA4 y RwCamera.nearPlane (+0x80)
    u32 hasPed{};                                   // FindPlayerPed distinto de nulo
    u32 los1{}, hit1[3]{}, los2{}, hit2[3]{};       // ProcessLineOfSight: resultados y puntos de choque
    u32 sphere{}, spt[3]{}, snorm[3]{};             // TestSphereAgainstWorld: choque, punto (0xB9B250) y normal (0xB9B260)
    u32 mutSite{}, mutVal{};
};

// ------------------------------------------------------------------------------------------------ generador
struct V3 { double x, y, z; };
inline V3 operator+(V3 a, V3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
inline V3 operator-(V3 a, V3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
inline V3 operator*(V3 a, double k) { return {a.x * k, a.y * k, a.z * k}; }
inline double Dot(V3 a, V3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline double Len(V3 a) { return std::sqrt(Dot(a, a)); }
inline V3 Unit(V3 a) { const double l = Len(a); return l > 0 ? a * (1.0 / l) : V3{1, 0, 0}; }

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.ecx = Rand(n, 3); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    const u32 f = n % 16;
    c.family = f;
    u32 kk = 100;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    auto U01 = [&] { return double(R(1000001)) / 1000000.0; };
    auto Fl = [&](float lo, float hi) { return lo + (hi - lo) * float(U01()); };
    auto Dir = [&](double zbias) {   // vector unitario aleatorio
        const double ang = U01() * 6.283185307179586;
        const double z = (U01() * 2.0 - 1.0) * zbias;
        const double r = std::sqrt(std::max(0.0, 1.0 - z * z));
        return V3{r * std::cos(ang), r * std::sin(ang), z};
    };
    auto To3 = [&](u32* o, V3 v) { o[0] = B(float(v.x)); o[1] = B(float(v.y)); o[2] = B(float(v.z)); };
    auto Rough = [&](double lo, double hi) { return lo + (hi - lo) * U01(); };

    // --- posiciones: source y target
    V3 S{Rough(-3000, 3000), Rough(-3000, 3000), Rough(-100, 300)};
    double len = P(20) ? Rough(0.2, 4.0) : (P(70) ? Rough(2.0, 40.0) : Rough(40.0, 400.0));
    V3 dir = Dir(P(30) ? 0.9 : 0.35);
    if (f == 1 || P(3)) {                         // dx = dy = 0 (rumbo de reserva), con dz distinto de cero o cero
        dir = {0, 0, P(35) ? 0.0 : (P(50) ? 1.0 : -1.0)};
        if (dir.z == 0.0 && P(60)) len = 0.0;
    }
    if (f == 2) {                                 // ejes alineados: dx o dy nulos, dz nulo
        const u32 k = R(6);
        dir = k == 0 ? V3{1, 0, 0} : k == 1 ? V3{-1, 0, 0} : k == 2 ? V3{0, 1, 0} : k == 3 ? V3{0, -1, 0} : k == 4 ? V3{0.0, 0.7071, 0.7071} : V3{0.7071, 0.0, -0.7071};
    }
    V3 T = S + dir * len;
    To3(c.src, S); To3(c.tgt, T);
    for (u32 k = 0; k < 3; ++k) c.outInit[k] = Rand(n, nk());
    c.fov = B(P(70) ? Fl(5.0f, 120.0f) : (P(50) ? Fl(0.0f, 360.0f) : Fl(-90.0f, 90.0f)));
    if (f == 8) { const float v[] = {0.0f, 1.0f, 45.0f, 90.0f, 120.0f, 179.0f, 180.0f, 359.0f, 360.0f, -45.0f, 0.001f, 1e-30f, 70.0f}; c.fov = B(v[R(13)]); }
    c.head984 = B(Fl(-2.0f, 2.0f)); c.head988 = B(Fl(-2.0f, 2.0f));
    if (P(20)) { c.head984 = 0u; } if (P(15)) { c.head988 = 0u; }
    c.timer = P(35) ? B(Fl(-3.0f, 3.0f)) : (P(30) ? 0u : B(Fl(-60.0f, 60.0f)));
    c.dirn = P(50) ? 0u : (P(50) ? 0xFFFFu : R(0x10000));
    c.i2d = R(256);
    c.tgtNull = P(f == 12 ? 40 : 5);
    c.isPed = P(f == 5 || f == 6 ? 85 : 50) ? ((R(32) << 3) | 3u) : (R(32) << 3) | R(8);
    if (!(c.isPed & 7u)) c.isPed |= 1u;
    c.tmat = P(55);
    To3(c.tpos, T + Dir(0.5) * Rough(0.0, 5.0));
    To3(c.talt, V3{Rough(-3000, 3000), Rough(-3000, 3000), Rough(-100, 300)});
    // --- globales
    c.timestep = P(65) ? B(Fl(0.05f, 3.0f)) : (P(30) ? Pick(std::array<u32, 6>{0u, 0x80000000u, B(1.0f), B(0.033f), B(5.0f), B(-0.5f)}, n, nk()) : B(Fl(-2.0f, 50.0f)));
    if (f == 9) { const float v[] = {0.0f, -0.0f, 1.0f, 1e-10f, 0.5f, 2.0f, 3.5f, -1.0f, 10.0f, 50.0f, -10.0f, 0.016f, 7.0f}; c.timestep = B(v[R(13)]); }
    c.b65 = P(55) ? 1u : (P(50) ? 0u : R(256));
    c.wbCur = B(Fl(0.0f, 1.0f)); c.wbSpd = B(Fl(-1.0f, 1.0f)); c.wbOutCur = B(Fl(0.0f, 1.0f)); c.wbOutSpd = B(Fl(-1.0f, 1.0f));
    if (P(15)) c.wbOutCur = Pick(std::array<u32, 5>{0u, B(1.0f), 0x80000000u, 1u, B(0.5f)}, n, nk());
    c.aspect = P(60) ? B(P(50) ? 1.3333334f : 1.7777778f) : B(Fl(0.5f, 2.5f));
    c.hasPed = P(f == 3 ? 40 : 85);
    // --- plano cercano: valores tipicos y umbrales (0.15, 0.3)
    {
        const u32 k = R(12);
        const float tab[] = {0.05f, 0.1f, 0.15f, 0.15f, 0.2f, 0.3f, 0.3f, 0.5f, 0.9f, 1.0f, 0.0f, 0.15000001f};
        c.nearPlane = P(55) ? B(tab[k]) : B(Fl(0.02f, 1.2f));
        if (f == 7) { const u32 v[] = {0u, 0x80000000u, B(-0.1f), B(0.3f), B(0.29999998f), B(0.30000004f), B(0.15f), B(0.14999999f), B(0.15000001f), B(2.0f), B(100.0f), 1u}; c.nearPlane = v[R(12)]; }
    }
    const double nearV = double(F(c.nearPlane));
    // --- primer choque: punto entre source y target a una distancia del objetivo cercana a near + 0.5 (aprox.)
    c.los1 = P(f == 15 ? 90 : 72);
    c.los2 = P(55);
    {
        const V3 toS = Unit(S - T);
        double d1;
        const u32 k = R(6);
        if (k == 0) d1 = 0.5 + nearV * (1.0 + (U01() - 0.5) * 0.02);
        else if (k == 1) d1 = 0.5 + 0.15 * (1.0 + (U01() - 0.5) * 0.02);
        else if (k == 2) d1 = Rough(0.0, 1.5);
        else d1 = Rough(0.0, std::max(0.5, len));
        V3 H1 = T + toS * d1;
        To3(c.hit1, H1);
        // segundo choque: distancia al primero (y a si mismo) alrededor de near
        const double d2 = P(50) ? nearV * (1.0 + (U01() - 0.5) * 0.04) : Rough(0.0, 2.0);
        To3(c.hit2, H1 + Dir(1.0) * d2);
        if (P(8)) To3(c.hit2, V3{double(F(c.hit1[0])), double(F(c.hit1[1])), double(F(c.hit1[2]))});
    }
    // --- esfera: choque con el punto alrededor de la direccion; proyeccion cerca de 0.15 / 0.9 / near
    c.sphere = P(f == 4 || f == 5 || f == 6 ? 90 : 50);
    {
        const V3 H1{double(F(c.hit1[0])), double(F(c.hit1[1])), double(F(c.hit1[2]))};
        const V3 O = c.los1 ? H1 : S;
        const V3 d = Unit(T - O);
        double proj;
        const u32 k = R(8);
        proj = k == 0 ? 0.15 * (1.0 + (U01() - 0.5) * 0.01) : k == 1 ? 0.9 * (1.0 + (U01() - 0.5) * 0.01) : k == 2 ? nearV * (1.0 + (U01() - 0.5) * 0.02) : k == 3 ? Rough(-0.5, 0.15) : k == 4 ? Rough(0.15, 0.9) : k == 5 ? Rough(0.9, 3.0) : Rough(-1.0, 2.0);
        const double radius = nearV * 1.3333 * std::tan(F(c.fov) * 0.0087266463) * 1.15;
        V3 perp = Dir(1.0);
        perp = Unit(perp - d * Dot(perp, d));
        const double rho = std::fabs(radius) * U01() * (P(20) ? 1.3 : 0.95);
        To3(c.spt, O + d * proj + perp * rho);
        V3 nrm = Dir(1.0);
        if (P(10)) nrm = nrm * Rough(0.0, 5.0);
        if (P(5)) nrm = V3{0, 0, 0};
        To3(c.snorm, nrm);
        // vector adelante del ped: alrededor del producto 0, 0.5 y signos con la normal (orientada hacia el punto)
        const V3 N = Unit(nrm);
        const u32 kf = R(6);
        V3 fw;
        if (kf == 0) fw = N; else if (kf == 1) fw = N * -1.0;
        else if (kf == 2) { const double cs = 0.5 * (1.0 + (U01() - 0.5) * 0.02); fw = N * cs + Unit(Dir(1.0) - N * Dot(Dir(1.0), N)) * std::sqrt(1.0 - cs * cs); }
        else if (kf == 3) fw = Unit(Dir(1.0) - N * Dot(Dir(1.0), N));
        else fw = Dir(1.0);
        To3(c.fwd, fw);
    }
    // --- casos especiales de f == 11 / 15: valores no finitos en entradas
    if (f == 11 || f == 15) {
        for (u32 k = 0; k < 3; ++k) {
            if (P(12)) c.src[k] = Special(n, nk());
            if (P(12)) c.tgt[k] = Special(n, nk());
            if (P(8)) c.hit1[k] = Special(n, nk());
            if (P(8)) c.hit2[k] = Special(n, nk());
            if (P(8)) c.spt[k] = Special(n, nk());
            if (P(8)) c.snorm[k] = Special(n, nk());
            if (P(8)) c.fwd[k] = Special(n, nk());
            if (P(5)) c.tpos[k] = Special(n, nk());
        }
        if (P(15)) c.fov = Pick(kTiny, n, nk());
        if (P(10)) c.aspect = Special(n, nk());
        if (P(10)) c.nearPlane = Special(n, nk());
        if (P(10)) c.timer = Special(n, nk());
        if (P(6)) c.head984 = Special(n, nk());
        if (P(6)) c.head988 = Special(n, nk());
    }
    // --- mutaciones: una escritura dentro de un falso (target.x tras la 1a linea de vision, out.y tras la esfera, vector adelante tras la 1a GetMatrix)
    if ((f == 13 && P(90)) || P(4)) { c.mutSite = 1 + R(3); c.mutVal = P(30) ? Special(n, nk()) : B(Fl(-100.0f, 100.0f)); }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    auto V = [](const u32* p, std::size_t k) { return std::vector<u32>(p, p + k); };
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.A("src", V(c.src, 3)).A("tgt", V(c.tgt, 3)).A("outinit", V(c.outInit, 3)).U("fov", c.fov).U("h984", c.head984).U("h988", c.head988);
    j.U("timer", c.timer).U("dirn", c.dirn).U("i2d", c.i2d).U("tnull", c.tgtNull).U("isped", c.isPed).U("tmat", c.tmat).A("tpos", V(c.tpos, 3)).A("talt", V(c.talt, 3)).A("fwd", V(c.fwd, 3));
    j.U("timestep", c.timestep).U("b65", c.b65).U("wbcur", c.wbCur).U("wbspd", c.wbSpd).U("wboutcur", c.wbOutCur).U("wboutspd", c.wbOutSpd).U("aspect", c.aspect).U("near", c.nearPlane).U("hasped", c.hasPed);
    j.U("los1", c.los1).A("hit1", V(c.hit1, 3)).U("los2", c.los2).A("hit2", V(c.hit2, 3)).U("sphere", c.sphere).A("spt", V(c.spt, 3)).A("snorm", V(c.snorm, 3));
    j.U("mutsite", c.mutSite).U("mutval", c.mutVal);
    return j.End();
}
}

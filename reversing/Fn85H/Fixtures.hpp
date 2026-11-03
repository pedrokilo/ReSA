#pragma once
// Fixtures del banco Fn85H (lote 85H): una funcion, 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskSimpleFight::TargetInReach                        0x61D6F0  thiscall (tarea en ECX, ped), `ret 4`; devuelve AL   (hook CTaskSimpleFight / TargetInReach, tipo S)
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

namespace F85HFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;
constexpr unsigned kOthers = 16;        // peds cercanos (+0x130 de la inteligencia)
constexpr unsigned kCombos = 17;        // filas de los combos 0..16

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kTaskWin = 0x28, kPedWin = 0x740, kTgtWin = 0x5A0, kIntWin = 0x170, kMatWin = 0x40, kOthWin = 0x560, kFTaskWin = 0x20, kAssocWin = 0x30, kHierWin = 0x20, kObjWin = 0x5A0;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kTaskArena = Arena(kTaskWin), kPedArena = Arena(kPedWin), kTgtArena = Arena(kTgtWin), kIntArena = Arena(kIntWin), kMatArena = Arena(kMatWin), kOthArena = Arena(kOthWin),
                      kFTaskArena = Arena(kFTaskWin), kAssocArena = Arena(kAssocWin), kHierArena = Arena(kHierWin), kObjArena = Arena(kObjWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 85008u); }
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
inline u32 NextUp(u32 bits, int steps) {   // desplaza un float normal `steps` unidades de ultimo lugar (signo respetado)
    const bool neg = (bits >> 31) != 0;
    u32 mag = bits & 0x7FFFFFFFu;
    mag = u32(i32(mag) + (neg ? -steps : steps));
    return (bits & 0x80000000u) | (mag & 0x7FFFFFFFu);
}

// ------------------------------------------------------------------------------------------------ el caso
// Entidades que pueden ser objetivo de la tarea: 0 = objetivo, 1 = segundo objetivo (al que cambian las mutaciones), 2 = objeto (solo como objetivo apuntado +0x568 del ped)
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 combo{};                                    // byte +0x24 de la tarea (0..16)
    u32 limit{};                                    // float global 0xC177EC
    u32 rowf[kCombos]{};                            // float +0x3C de las filas de los combos 0..16 (0xC1710C + (combo - 4) * 0x88)
    u32 kick{};                                     // AL que devuelve 0x56D270 (el resto es basura)
    u32 tmode{};                                    // 0 sin objetivo, 1 ped (byte +0x36 & 7 == 3), 2 otro no nulo
    u32 etype[3]{};                                 // byte +0x36 de las entidades 0, 1 y 2
    u32 eflags[2]{};                                // dword +0x46C de los objetivos 0 y 1
    u32 emat[2]{};                                  // los objetivos tienen matriz (+0x14)
    u32 epos[6]{}, ealt[6]{};                       // posicion en la ubicacion activa y en la otra (matriz +0x30 o compacta +4) de los objetivos 0 y 1
    u32 e590[3]{};                                  // dword +0x590 de las entidades 0, 1 y 2
    u32 ebone[6]{};                                 // posicion del hueso 3 de los objetivos 0 y 1 (x, y, z)
    u32 alive{};                                    // AL de 0x5E0170 (con basura)
    u32 isplayer{};                                 // AL de 0x5DF8F0 (con basura)
    u32 smode{};                                    // tarea activa mas simple del objetivo: 0 ninguna, 1 otro tipo, 2 tipo 0xCF sin animacion, 3 tipo 0xCF con animacion
    u32 stype{};                                    // tipo que devuelve la tarea (dword)
    u32 acur{}, atotal{};                           // floats +0x20 de la animacion y +0x10 de su jerarquia
    u32 m1r[3]{}, m1f[3]{}, m2r[3]{}, m2f[3]{};     // matrices que devuelven la primera y la segunda llamada a 0x411990: derecha (+0) y avance (+0x10)
    u32 pmat{};                                     // el ped tiene matriz
    u32 ppos[3]{}, palt[3]{};                       // posicion del ped (activa y otra)
    u32 rot{}, aim{};                               // rumbo (+0x558) y rumbo de apuntado (+0x55C) del ped
    u32 ptarg{};                                    // objetivo apuntado (+0x568): 0 nulo, 1 el objetivo 0, 2 el objeto
    u32 opres[kOthers]{}, ostate[kOthers]{}, ohealth[kOthers]{};
    u32 opos[kOthers * 3]{}, obone[kOthers * 3]{}, orf[kOthers * 6]{};   // posicion (matriz +0x30), hueso 3 y derecha/avance de la matriz de cada ped cercano
    u32 mutSite{}, mutA{}, mutB{}, mutC{}, mutD{}, mutPos[3]{};
    u32 gate{};
};

// ------------------------------------------------------------------------------------------------ generador
inline u32 UnitComp(u32 c, u32 k, double& x, double& y, double& z) {
    auto r = [&](u32 j) { return double(int(Rand(c, k + j) % 20001) - 10000) / 10000.0; };
    x = r(1); y = r(2); z = r(3);
    double m = std::sqrt(x * x + y * y + z * z);
    if (m < 0.05) { x = 1.0; y = 0.0; z = 0.0; m = 1.0; }
    x /= m; y /= m; z /= m;
    return 0;
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.ecx = Rand(n, 3); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    u32 kk = 100;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    auto Pk = [&](std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); };
    auto Junk = [&](u32 al) { return (Rand(n, nk()) & ~0xFFu) | al; };
    auto Fl = [&](float lo, float hi) { return lo + (hi - lo) * (float(R(100001)) / 100000.0f); };
    // --- datos base
    c.combo = P(80) ? R(17) : Pk({0u, 1u, 3u, 4u, 5u, 6u, 7u, 8u, 16u});
    c.limit = P(15) ? Pk({0u, B(0.5f), B(-0.25f), B(1.0f), B(0.1f)}) : B(Fl(-0.3f, 1.2f));
    for (u32 i = 0; i < kCombos; ++i) c.rowf[i] = P(10) ? Pk({0u, B(0.5f), B(-1.0f), B(1.5f), B(2.5f)}) : B(Fl(-0.4f, 2.0f));
    c.kick = Junk(P(97) ? 1u : 0u);
    c.tmode = P(70) ? 1u : (P(50) ? 0u : 2u);
    c.etype[0] = c.tmode == 1 ? ((R(32) << 3) | 3u) : (c.tmode == 2 ? ((R(32) << 3) | Pk({0u, 1u, 2u, 4u, 5u, 6u, 7u})) : R(256));
    c.etype[1] = P(70) ? ((R(32) << 3) | 3u) : R(256);
    c.etype[2] = P(65) ? ((R(32) << 3) | 2u) : R(256);
    c.eflags[0] = P(50) ? (0x4000000u | (Rand(n, nk()) & 0xFBFFFFFFu)) : (Rand(n, nk()) & ~0x4000000u);
    c.eflags[1] = P(50) ? (0x4000000u | (Rand(n, nk()) & 0xFBFFFFFFu)) : (Rand(n, nk()) & ~0x4000000u);
    c.emat[0] = P(60) ? 1u : 0u; c.emat[1] = P(60) ? 1u : 0u;
    c.pmat = P(60) ? 1u : 0u;
    // posiciones: el ped cerca del origen, los demas alrededor
    auto Pos3 = [&](u32* d, float cx, float cy, float cz, float spread) { d[0] = B(cx + Fl(-spread, spread)); d[1] = B(cy + Fl(-spread, spread)); d[2] = B(cz + Fl(-spread / 4, spread / 4)); };
    const float base = P(25) ? 0.0f : float(int(R(2001)) - 1000) / 10.0f;
    Pos3(c.ppos, base, base * 0.5f, 3.0f, 1.0f);
    Pos3(c.palt, base + 7.0f, base * 0.5f - 5.0f, 9.0f, 1.0f);
    for (u32 e = 0; e < 2; ++e) { Pos3(&c.epos[e * 3], F(c.ppos[0]), F(c.ppos[1]), F(c.ppos[2]), 2.0f); Pos3(&c.ealt[e * 3], F(c.ppos[0]) - 4.0f, F(c.ppos[1]) + 3.0f, F(c.ppos[2]) - 6.0f, 2.0f); }
    for (u32 e = 0; e < 2; ++e) Pos3(&c.ebone[e * 3], F(c.epos[e * 3]), F(c.epos[e * 3 + 1]), F(c.epos[e * 3 + 2]), 1.0f);
    c.e590[0] = P(50) ? 0u : (Rand(n, nk()) | 1u); c.e590[1] = P(50) ? 0u : (Rand(n, nk()) | 1u); c.e590[2] = P(50) ? 0u : (Rand(n, nk()) | 1u);
    c.alive = Junk(P(70) ? 1u : 0u);
    c.isplayer = Junk(P(85) ? 1u : 0u);
    c.smode = Pk({0u, 1u, 2u, 3u, 3u, 3u});
    c.stype = c.smode == 1 ? Pk({0xCEu, 0xD0u, 0u, 0xCDu, 0xFFFFFFFFu, 0x100CFu, R(1000)}) : 0xCFu;
    c.acur = B(Fl(0.0f, 3.0f)); c.atotal = B(Fl(0.0f, 3.0f));
    auto Mat = [&](u32* r, u32* fw) {
        double x, y, z;
        UnitComp(n, nk(), x, y, z); r[0] = B(float(x)); r[1] = B(float(y)); r[2] = B(float(z));
        UnitComp(n, nk(), x, y, z); fw[0] = B(float(x)); fw[1] = B(float(y)); fw[2] = B(float(z));
    };
    Mat(c.m1r, c.m1f); Mat(c.m2r, c.m2f);
    c.rot = P(15) ? Special(n, nk()) : B(float(int(R(6284)) - 3142) / 1000.0f);
    c.aim = Rand(n, nk());
    c.ptarg = Pk({0u, 1u, 2u, 2u});
    for (u32 i = 0; i < kOthers; ++i) {
        c.opres[i] = P(70) ? 1u : 0u;
        c.ostate[i] = P(15) ? 0x37u : Pk({0u, 1u, 0x36u, 0x38u, 0x7FFFFFFFu, 0x80000037u, R(100)});
        c.ohealth[i] = P(20) ? Pk({0u, 0x80000000u, B(-5.0f), 0x7F800000u, 0xFFC00000u, 1u, B(100.0f)}) : B(Fl(0.0f, 200.0f));
        Pos3(&c.opos[i * 3], F(c.ppos[0]), F(c.ppos[1]), F(c.ppos[2]) - 0.5f, 2.2f);
        Pos3(&c.obone[i * 3], F(c.opos[i * 3]), F(c.opos[i * 3 + 1]), F(c.opos[i * 3 + 2]) - 0.6f, 1.0f);
        Mat(&c.orf[i * 6], &c.orf[i * 6 + 3]);
    }
    // --- por familia
    auto Hits = [&](bool nearOk, bool farOk) {   // coloca el objetivo 0 para pasar (o no) los dos productos; usa matrices de las llamadas 1 (avance) y 2 (derecha)
        const double rowsum = double(F(c.limit)) + double(F(c.rowf[c.combo]));
        const double nearV = std::max(0.05, rowsum + 0.4), farV = std::max(0.05, rowsum + 0.2);
        const double fx = F(c.m1f[0]), fy = F(c.m1f[1]), fz = F(c.m1f[2]), rx = F(c.m2r[0]), ry = F(c.m2r[1]), rz = F(c.m2r[2]);
        // delta = a * fwd_perp... se busca un delta cuyo |d . avance| sea ~0.4*near (ok) o ~2*near (no) y |d . derecha| ~0.4*far o ~2*far
        const double tn = nearOk ? 0.4 * nearV : 2.5 * nearV + 0.1, tf = farOk ? 0.4 * farV : 2.5 * farV + 0.1;
        // resuelve en el plano: d = u*f + w*r con (f.f=1) -> dots aproximados; se usa el sistema 2x2 con g = f.r
        const double g = fx * rx + fy * ry + fz * rz;
        const double det = 1.0 - g * g;
        double u = 0, w = 0;
        if (std::fabs(det) > 1e-3) { u = (tn - g * tf) / det; w = (tf - g * tn) / det; } else { u = tn; w = 0; }
        if (P(50)) { u = -u; w = -w; }
        const double dx = u * fx + w * rx, dy = u * fy + w * ry, dz = u * fz + w * rz;
        c.epos[0] = B(float(double(F(c.ppos[0])) + dx)); c.epos[1] = B(float(double(F(c.ppos[1])) + dy)); c.epos[2] = B(float(double(F(c.ppos[2])) + dz));
        c.ebone[0] = c.epos[0]; c.ebone[1] = c.epos[1]; c.ebone[2] = B(F(c.epos[2]) - float(R(1000)) / 500.0f);
    };
    auto ZLow = [&] { c.ebone[2] = B(F(c.ppos[2]) + 0.3f + float(R(1000)) / 500.0f); };     // z del ped - 0.2 NO supera la del hueso
    auto ZHigh = [&] { c.ebone[2] = B(F(c.ppos[2]) - 0.3f - float(R(1000)) / 500.0f); };    // la supera
    auto Others = [&](u32 presPct, u32 maxRadius1000, bool allMatrixAligned) {
        for (u32 i = 0; i < kOthers; ++i) {
            c.opres[i] = P(presPct) ? 1u : 0u;
            const float radius = float(R(maxRadius1000 + 1)) / 1000.0f;
            const float angle = float(R(6284)) / 1000.0f;
            const float dz = float(int(R(1001)) - 500) / 2500.0f;
            c.opos[i * 3 + 0] = B(F(c.ppos[0]) + radius * std::cos(angle));
            c.opos[i * 3 + 1] = B(F(c.ppos[1]) + radius * std::sin(angle));
            c.opos[i * 3 + 2] = B(F(c.ppos[2]) + dz);
            c.obone[i * 3 + 2] = B(F(c.ppos[2]) - 0.3f - float(R(1000)) / 500.0f);   // la z del ped - 0.2 supera la del hueso
            if (allMatrixAligned) {   // avance y derecha de la matriz de cada ped colocados para pasar los dos productos: perpendiculares al delta
                const double dx = double(F(c.opos[i * 3])) - double(F(c.ppos[0])), dy = double(F(c.opos[i * 3 + 1])) - double(F(c.ppos[1]));
                const double m = std::sqrt(dx * dx + dy * dy) + 1e-9;
                const double px = -dy / m, py = dx / m;
                c.orf[i * 6 + 0] = B(float(px)); c.orf[i * 6 + 1] = B(float(py)); c.orf[i * 6 + 2] = B(0.0f);          // derecha
                c.orf[i * 6 + 3] = B(float(px * 0.8)); c.orf[i * 6 + 4] = B(float(py * 0.8)); c.orf[i * 6 + 5] = B(0.0f);   // avance
            }
        }
    };
    switch (f) {
    case 0:                                                                                                                                         // objetivo ped, vivo, z alta: pasa o no los dos productos
        c.tmode = 1; c.etype[0] = (R(32) << 3) | 3u; c.alive = Junk(1); c.kick = Junk(1); c.smode = 0;
        ZHigh(); Hits(P(75), P(75));
        break;
    case 1:                                                                                                                                         // objetivo ped, vivo, z baja: la bandera 0x4000000 decide y acorta el alcance
        c.tmode = 1; c.etype[0] = (R(32) << 3) | 3u; c.alive = Junk(1); c.kick = Junk(1); c.smode = P(60) ? 0u : 1u;
        ZLow(); Hits(P(70), P(70));
        if (P(15)) { const u32 which = R(3); c.ebone[2] = which == 0 ? B(F(c.ppos[2]) - 0.2f) : (which == 1 ? NextUp(B(F(c.ppos[2]) - 0.2f), int(R(5)) - 2) : Special(n, nk())); }
        break;
    case 2:                                                                                                                                         // objetivo ped muerto
        c.tmode = 1; c.etype[0] = (R(32) << 3) | 3u; c.alive = Junk(0); c.kick = Junk(1);
        if (P(50)) ZLow(); else ZHigh();
        Hits(P(75), P(75));
        break;
    case 3:                                                                                                                                         // tarea mas simple del objetivo: ninguna, otro tipo, 0xCF con o sin animacion; tiempos mayor, igual, menor o NaN
        c.tmode = 1; c.etype[0] = (R(32) << 3) | 3u; c.alive = Junk(P(50) ? 1u : 0u); c.kick = Junk(1);
        c.smode = R(4);
        if (c.smode == 1) c.stype = Pk({0xCEu, 0xD0u, 0u, 0xCDu, 0xFFFFFFFFu, 0x100CFu, 1u + R(900)});
        else c.stype = 0xCFu;
        { const u32 rel = R(6);
          const float a = Fl(0.0f, 3.0f);
          c.acur = B(a);
          c.atotal = rel == 0 ? B(a + 0.5f) : (rel == 1 ? c.acur : (rel == 2 ? B(a - 0.5f) : (rel == 3 ? NextUp(c.acur, 1) : (rel == 4 ? NextUp(c.acur, -1) : Special(n, nk()))))); }
        if (P(30)) c.acur = Special(n, nk());
        if (P(50)) ZLow(); else ZHigh();
        Hits(P(80), P(80));
        break;
    case 4:                                                                                                                                         // primer producto (avance) en el borde del alcance cercano
        c.tmode = 1; c.etype[0] = (R(32) << 3) | 3u; c.alive = Junk(1); c.kick = Junk(1); c.smode = 0;
        ZHigh(); Hits(true, true);
        for (u32 k = 0; k < 3; ++k) c.epos[k] = NextUp(c.epos[k], int(R(7)) - 3);
        if (P(35)) { c.limit = NextUp(c.limit, int(R(5)) - 2); }
        if (P(20)) { c.m1f[R(3)] = Special(n, nk()); }
        break;
    case 5:                                                                                                                                         // segundo producto (derecha, con el delta guardado como float) en el borde del alcance lejano
        c.tmode = 1; c.etype[0] = (R(32) << 3) | 3u; c.alive = Junk(1); c.kick = Junk(1); c.smode = 0;
        ZHigh(); Hits(true, true);
        for (u32 k = 0; k < 3; ++k) c.epos[k] = NextUp(c.epos[k], int(R(5)) - 2);
        if (P(30)) { c.m2r[R(3)] = Special(n, nk()); }
        if (P(20)) { c.ppos[R(3)] = Special(n, nk()); }
        break;
    case 6:                                                                                                                                         // objetivo que no es un ped: verdadero solo si es el objetivo apuntado del ped
        c.tmode = 2; c.etype[0] = (R(32) << 3) | Pk({0u, 1u, 2u, 4u, 5u, 6u, 7u}); c.kick = Junk(P(95) ? 1u : 0u);
        c.ptarg = Pk({0u, 1u, 1u, 2u});
        if (P(15)) c.etype[0] = R(256);
        break;
    case 7:                                                                                                                                         // sin objetivo, no es el jugador / 0x56D270 devuelve 0
        c.tmode = P(70) ? 0u : (P(50) ? 1u : 2u); c.kick = Junk(P(70) ? 1u : 0u); c.isplayer = Junk(P(40) ? 1u : 0u);
        if (c.tmode == 0) c.etype[0] = R(256);
        Others(75, 1900, P(50));
        break;
    case 8:                                                                                                                                         // sin objetivo, el jugador: pocos peds colocados para pasar los productos
        c.tmode = 0; c.kick = Junk(1); c.isplayer = Junk(1);
        Others(P(50) ? 20 : 45, 1900, true);
        c.rot = B(float(int(R(1800)) - 900) / 1000.0f);
        break;
    case 9:                                                                                                                                         // sin objetivo, muchos peds: empates, salud y desempate por |d|
        c.tmode = 0; c.kick = Junk(1); c.isplayer = Junk(1);
        Others(95, 2000, true);
        for (u32 i = 0; i < kOthers; ++i) {
            if (P(25)) c.ohealth[i] = Pk({0u, 0x80000000u, B(-3.0f), B(0.0f)});
            if (P(30) && i > 0) { for (u32 k = 0; k < 3; ++k) c.opos[i * 3 + k] = c.opos[(i - 1) * 3 + k]; c.ohealth[i] = c.ohealth[i - 1] ^ (P(20) ? 0x80000000u : 0u); }
            if (P(15)) c.ostate[i] = 0x37u;
        }
        break;
    case 10:                                                                                                                                        // vueltas de 2pi del rumbo y borde de 1.0472
        c.tmode = 0; c.kick = Junk(1); c.isplayer = Junk(1);
        Others(70, 1900, true);
        c.rot = Pk({B(3.1f), B(-3.1f), B(3.14159f), B(-3.14159f), B(3.2f), B(-3.2f), B(6.0f), B(-6.0f), B(1.0472f), B(0.0f), B(1.5f), B(-1.5f)});
        if (P(35)) c.rot = NextUp(c.rot, int(R(5)) - 2);
        break;
    case 11:                                                                                                                                        // sin candidatos: objetivo apuntado del ped
        c.tmode = 0; c.kick = Junk(1); c.isplayer = Junk(1);
        Others(P(50) ? 0 : 20, 1900, false);
        for (u32 i = 0; i < kOthers; ++i) if (P(60)) c.obone[i * 3 + 2] = B(F(c.ppos[2]) + 5.0f);   // z del hueso por encima: ninguno pasa
        c.ptarg = Pk({0u, 1u, 2u, 2u, 2u});
        c.etype[1] = c.etype[0];
        break;
    case 12:                                                                                                                                        // valores especiales
        if (P(50)) {
            c.tmode = 1; c.etype[0] = (R(32) << 3) | 3u; c.alive = Junk(1); c.kick = Junk(1); c.smode = P(50) ? 0u : 3u; ZHigh(); Hits(true, true);
            for (u32 k = 0; k < 3; ++k) if (P(15)) c.epos[k] = Special(n, nk());
            if (P(25)) c.m1f[R(3)] = Special(n, nk());
            if (P(25)) c.m2r[R(3)] = Special(n, nk());
            if (P(20)) c.ppos[R(3)] = Special(n, nk());
            if (P(20)) c.limit = Special(n, nk());
            if (P(20)) c.rowf[c.combo] = Special(n, nk());
            if (P(15)) c.ebone[2] = Special(n, nk());
        } else {
            c.tmode = 0; c.kick = Junk(1); c.isplayer = Junk(1); Others(85, 1500, true);
            for (u32 i = 0; i < kOthers; ++i) {
                if (P(25)) c.opos[i * 3 + R(3)] = Special(n, nk());
                if (P(15)) c.orf[i * 6 + R(6)] = Special(n, nk());
                if (P(10)) c.obone[i * 3 + 2] = Special(n, nk());
            }
            if (P(30)) c.rot = Special(n, nk());
            if (P(25)) c.ppos[R(3)] = Special(n, nk());
            if (P(20)) c.limit = Special(n, nk());
            if (P(20)) c.rowf[c.combo] = Special(n, nk());
        }
        break;
    case 13: {                                                                                                                                      // mutaciones de una lectura
        c.mutSite = 1u + R(8);
        c.mutA = R(256); c.mutB = Rand(n, nk()); c.mutC = R(256); c.mutD = R(5);
        for (u32 k = 0; k < 3; ++k) c.mutPos[k] = B(F(c.ppos[k]) + float(int(R(3001)) - 1500) / 1000.0f);
        const bool loop = c.mutSite == 4 || (P(50) && c.mutSite != 6 && c.mutSite != 7 && c.mutSite != 8);
        if (loop) { c.tmode = 0; c.kick = Junk(1); c.isplayer = Junk(1); Others(85, 1700, true); c.mutA = R(16);
            for (u32 i = 0; i < kOthers; ++i) if (P(20)) c.ostate[i] = 0x37u; }
        else { c.tmode = 1; c.etype[0] = (R(32) << 3) | 3u; c.kick = Junk(1); c.alive = Junk(P(80) ? 1u : 0u); c.smode = P(40) ? 0u : (P(50) ? 1u : 3u); c.mutA = R(16);
               if (P(50)) ZLow(); else ZHigh();
               Hits(P(80), P(80)); }
        if (c.mutSite == 5) c.mutB = Pk({0u, B(50.0f), B(-1.0f), 0x80000000u, B(1.0f)});
        if (c.mutSite == 4) c.mutB = Pk({0x37u, 0u, 1u, 0x38u, 0x80000037u});
        if (c.mutSite == 3) c.mutB = B(float(int(R(6284)) - 3142) / 1000.0f);
        if (c.mutSite == 7) c.mutB = P(50) ? (0x4000000u | (Rand(n, nk()) & 0xFBFFFFFFu)) : (Rand(n, nk()) & ~0x4000000u);
        break;
    }
    case 14:                                                                                                                                        // posicion en la matriz o compacta, del ped y de los objetivos
        if (P(60)) {
            c.tmode = 1; c.etype[0] = (R(32) << 3) | 3u; c.alive = Junk(1); c.kick = Junk(1); c.smode = P(60) ? 0u : 3u;
            if (P(50)) ZLow(); else ZHigh();
            c.pmat = R(2); c.emat[0] = R(2);
            Hits(P(85), P(85));
            if (P(50)) { for (u32 k = 0; k < 3; ++k) std::swap(c.epos[k], c.ealt[k]); }
        } else {
            c.tmode = 0; c.kick = Junk(1); c.isplayer = Junk(1); Others(80, 1700, true); c.pmat = R(2);
            if (P(40)) for (u32 k = 0; k < 3; ++k) std::swap(c.ppos[k], c.palt[k]);
        }
        break;
    default: {                                                                                                                                      // mezcla
        c.tmode = P(55) ? 1u : (P(50) ? 0u : 2u);
        if (c.tmode == 1) { c.etype[0] = (R(32) << 3) | 3u; if (P(50)) ZLow(); else ZHigh(); Hits(P(75), P(75)); }
        else if (c.tmode == 0) { Others(75, 1900, P(70)); }
        else { c.etype[0] = (R(32) << 3) | Pk({0u, 1u, 2u, 4u, 5u, 6u, 7u}); }
        if (P(10)) { c.mutSite = 1u + R(8); c.mutA = R(16); c.mutB = Rand(n, nk()); c.mutC = R(256); c.mutD = R(5);
                     for (u32 k = 0; k < 3; ++k) c.mutPos[k] = B(F(c.ppos[k]) + float(int(R(3001)) - 1500) / 1000.0f);
                     if (c.mutSite == 3) c.mutB = B(float(int(R(6284)) - 3142) / 1000.0f);
                     if (c.mutSite == 5) c.mutB = Pk({0u, B(50.0f), B(-1.0f), 0x80000000u, B(1.0f)});
                     if (c.mutSite == 4) c.mutB = Pk({0x37u, 0u, 1u, 0x38u}); }
        break;
    }
    }
    // la tarea falsa: con otro tipo (smode 1) nunca 0xCF; con smode 2 o 3 siempre 0xCF
    if (c.smode == 1 && c.stype == 0xCFu) c.stype = 0xCEu;
    if (c.smode >= 2) c.stype = 0xCFu;
    if (c.tmode != 0 && c.mutSite >= 1 && c.mutSite <= 5) c.mutA = 0;   // con objetivo solo hay una llamada a 0x5E4280
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("combo", c.combo).U("limit", c.limit).A("rowf", std::vector<u32>(c.rowf, c.rowf + kCombos)).U("kick", c.kick).U("tmode", c.tmode);
    j.A("etype", std::vector<u32>(c.etype, c.etype + 3)).A("eflags", std::vector<u32>(c.eflags, c.eflags + 2)).A("emat", std::vector<u32>(c.emat, c.emat + 2));
    j.A("epos", std::vector<u32>(c.epos, c.epos + 6)).A("ealt", std::vector<u32>(c.ealt, c.ealt + 6)).A("e590", std::vector<u32>(c.e590, c.e590 + 3)).A("ebone", std::vector<u32>(c.ebone, c.ebone + 6));
    j.U("alive", c.alive).U("isplayer", c.isplayer).U("smode", c.smode).U("stype", c.stype).U("acur", c.acur).U("atotal", c.atotal);
    j.A("m1r", std::vector<u32>(c.m1r, c.m1r + 3)).A("m1f", std::vector<u32>(c.m1f, c.m1f + 3)).A("m2r", std::vector<u32>(c.m2r, c.m2r + 3)).A("m2f", std::vector<u32>(c.m2f, c.m2f + 3));
    j.U("pmat", c.pmat).A("ppos", std::vector<u32>(c.ppos, c.ppos + 3)).A("palt", std::vector<u32>(c.palt, c.palt + 3)).U("rot", c.rot).U("aim", c.aim).U("ptarg", c.ptarg);
    j.A("opres", std::vector<u32>(c.opres, c.opres + kOthers)).A("ostate", std::vector<u32>(c.ostate, c.ostate + kOthers)).A("ohealth", std::vector<u32>(c.ohealth, c.ohealth + kOthers));
    j.A("opos", std::vector<u32>(c.opos, c.opos + kOthers * 3)).A("obone", std::vector<u32>(c.obone, c.obone + kOthers * 3)).A("orf", std::vector<u32>(c.orf, c.orf + kOthers * 6));
    j.U("mutsite", c.mutSite).U("muta", c.mutA).U("mutb", c.mutB).U("mutc", c.mutC).U("mutd", c.mutD).A("mutpos", std::vector<u32>(c.mutPos, c.mutPos + 3));
    j.U("gate", c.gate);
    return j.End();
}
}

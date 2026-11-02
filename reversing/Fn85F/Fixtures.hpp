#pragma once
// Fixtures del banco Fn85F (lote 85F): una funcion, 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskSimpleFight::FindTargetOnGround                   0x61C9B0  thiscall (tarea en ECX, ped), `ret 4`; sin valor de retorno que lean los llamadores   (hook CTaskSimpleFight / FindTargetOnGround, tipo S)
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

namespace F85FFix {
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
constexpr std::size_t kTaskWin = 0x28, kPedWin = 0x500, kPdWin = 0x40, kAssocWin = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kTaskArena = Arena(kTaskWin), kPedArena = Arena(kPedWin), kPdArena = Arena(kPdWin), kAssocArena = Arena(kAssocWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 85006u); }
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
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 clump{};                    // dword +0x18 del ped (el clump que se pasa a los colaboradores)
    u32 x{}, y{};                   // movimiento de pelea del jugador (+0xC, +0x10 de sus datos), patrones de float
    u32 xb{}, yb{};                 // idem en los datos alternativos (mutacion 2)
    u32 pres[4]{};                  // asociacion existente de las animaciones 0x29..0x2C (1 = existe)
    u32 amt[4]{}, dlt[4]{};         // peso (+0x18) y retardo (+0x1C) iniciales de esas asociaciones
    u32 newamt[4]{}, newdlt[4]{};   // idem de las asociaciones que devuelve AddAnimation (1.a a 4.a llamada)
    u32 next{}, combo{}, last{};    // bytes +0x26, +0x24 y +0x27 de la tarea
    u32 mutSite{}, mutA{}, mutB{};  // 1: tras la asociacion 0x29 cambia (siguiente, combo); 2: tras la 0x2C los datos del jugador pasan a los alternativos; 3: tras la 1.a AddAnimation cambia el clump;
                                    // 4: tras la 1.a AddAnimation cambian (x, y) de los datos; 5: tras la asociacion 0x2C cambian (x, y) de los datos
    u32 gate{};
};

inline float Pow10(int e) { float v = 1.0f; for (int i = 0; i < (e < 0 ? -e : e); ++i) v = e < 0 ? v / 10.0f : v * 10.0f; return v; }

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
    auto Neg = [&](u32 bits) { return bits | 0x80000000u; };
    auto Pos = [&](u32 bits) { return bits & 0x7FFFFFFFu; };
    auto Sign = [&](u32 bits) { return P(50) ? Neg(bits) : Pos(bits); };
    // magnitud general: mantisa 0.2..3.2 por una potencia de 10 entre 10^lo y 10^hi
    auto Mag = [&](int lo, int hi) { return B((0.2f + float(R(3000)) / 1000.0f) * Pow10(lo + int(R(u32(hi - lo + 1))))); };
    auto General = [&] { return Sign(Mag(-1, 3)); };
    auto Small = [&] { return P(15) ? Pk({0u, 0x80000000u}) : (P(15) ? Pick(kTiny, n, nk()) : B(float(int(R(2001)) - 1000) / 20000.0f)); };   // |v| <= 0.05
    // --- datos aleatorios base
    c.clump = Rand(n, nk()) | 0x00400000u;
    for (u32 i = 0; i < 4; ++i) {
        c.pres[i] = P(55) ? 1u : 0u;
        c.amt[i] = P(70) ? B(float(int(R(2001)) - 1000) / 1000.0f) : Special(n, nk());
        c.dlt[i] = P(70) ? B(float(int(R(2001)) - 1000) / 100.0f) : Special(n, nk());
        c.newamt[i] = P(70) ? B(float(int(R(2001)) - 1000) / 1000.0f) : Special(n, nk());
        c.newdlt[i] = P(70) ? B(float(int(R(2001)) - 1000) / 100.0f) : Special(n, nk());
    }
    c.next = P(50) ? 0u : 1u + R(255);
    c.combo = P(50) ? 0u : (P(50) ? 1u + R(127) : 0x80u + R(128));
    c.last = R(256);
    c.x = General(); c.y = General();
    c.xb = General(); c.yb = General();
    auto Moving = [&] { if (!c.next && !c.combo) { if (P(50)) c.next = 1u + R(255); else c.combo = 1u + R(255); } };
    auto Idle = [&] { c.next = 0; c.combo = 0; };
    // --- por familia
    switch (f) {
    case 0: Idle(); if (P(30)) { c.x = Special(n, nk()); c.y = Special(n, nk()); } break;                                                           // sin comando ni combo: parada
    case 1: Moving(); c.x = Small(); c.y = Small(); if (P(8)) c.x = Special(n, nk()); break;                                                       // movimiento de menos de 0.1: parada
    case 2: {                                                                                                                                       // justo en el umbral de 0.1
        Moving();
        const u32 base = 0x3DCCCCCDu;
        switch (R(4)) {
        case 0: c.x = Sign(NextUp(base, int(R(9)) - 4)); c.y = Pk({0u, 0x80000000u}); break;
        case 1: c.y = Sign(NextUp(base, int(R(9)) - 4)); c.x = Pk({0u, 0x80000000u}); break;
        case 2: { const u32 h = B(0.070710678f); c.x = Sign(NextUp(h, int(R(7)) - 3)); c.y = Sign(NextUp(h, int(R(7)) - 3)); break; }
        default: { const double a = double(R(1000)) / 1000.0 * 6.283185307; const float r = 0.1f * (1.0f + float(int(R(21)) - 10) * 1e-7f);
                   c.x = B(float(double(r) * std::cos(a))); c.y = B(float(double(r) * std::sin(a))); break; }
        }
        break;
    }
    case 3: case 4: case 5: case 6: {                                                                                                               // cuadrantes
        Moving();
        const bool nx = f == 4 || f == 6, ny = f == 5 || f == 6;                                                                                   // f=3 (+,+), 4 (-,+), 5 (+,-), 6 (-,-)
        const int lo = P(15) ? -3 : -1;
        c.x = nx ? Neg(Mag(lo, 3)) : Pos(Mag(lo, 3));
        c.y = ny ? Neg(Mag(lo, 3)) : Pos(Mag(lo, 3));
        break;
    }
    case 7: {                                                                                                                                       // x nula o minima frente a una y grande
        Moving();
        c.y = General();
        switch (R(4)) {
        case 0: c.x = Pk({0u, 0x80000000u}); break;
        case 1: c.x = Sign(Pick(kTiny, n, nk())); c.y = Sign(Pk({0x7F7FFFFFu, 0x7F000000u, 0x7E800000u, B(1.0e30f)})); break;                   // k * x se pierde bajo el menor desnormal: cero
        case 2: c.x = Sign(B(1.0e-35f * (1.0f + float(R(1000)) / 500.0f))); break;
        default: c.x = Sign(Pick(kTiny, n, nk())); break;
        }
        break;
    }
    case 8: {                                                                                                                                       // y nula o minima frente a una x grande
        Moving();
        c.x = General();
        switch (R(4)) {
        case 0: c.y = Pk({0u, 0x80000000u}); break;
        case 1: c.y = Sign(Pick(kTiny, n, nk())); c.x = Sign(Pk({0x7F7FFFFFu, 0x7F000000u, 0x7E800000u, B(1.0e30f)})); break;
        case 2: c.y = Sign(B(1.0e-35f * (1.0f + float(R(1000)) / 500.0f))); break;
        default: c.y = Sign(Pick(kTiny, n, nk())); break;
        }
        break;
    }
    case 9: {                                                                                                                                       // valores especiales (NaN, infinitos, enormes, desnormales, ceros)
        Moving();
        c.x = P(50) ? Special(n, nk()) : General();
        c.y = P(50) ? Special(n, nk()) : General();
        if (!P(80)) { if (P(50)) c.x = Special(n, nk()); else c.y = Special(n, nk()); }
        break;
    }
    case 10: {                                                                                                                                      // enormes: el cuadrado se sale del float pero no de la x87
        Moving();
        const float bigs[]{1.0e18f, 1.0e25f, 1.0e30f, 1.0e35f, 2.0e38f, 3.3e38f};
        auto Big = [&] { return Sign(B(bigs[R(6)] * (0.3f + float(R(700)) / 1000.0f))); };
        c.x = P(70) ? Big() : General();
        c.y = P(70) ? Big() : General();
        break;
    }
    case 11: {                                                                                                                                      // resultados desnormales al guardar el float
        Moving();
        const u32 den = Sign(P(50) ? 1u + R(0x7FFFFFu) : Pick(kTiny, n, nk()));
        const u32 den2 = Sign(0x00800000u + R(0x200000u));
        if (P(50)) { c.x = P(70) ? den : den2; c.y = Sign(Mag(-1, 3)); }
        else { c.y = P(70) ? den : den2; c.x = Sign(Mag(-1, 3)); }
        break;
    }
    case 12: {                                                                                                                                      // magnitudes iguales o en relacion simple (redondeos exactos y empates)
        Moving();
        static const float v[]{1.0f, 3.0f, 5.0f, 7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 0.5f, 0.75f, 10.0f, 100.0f};
        const float a = v[R(12)];
        const float b = P(40) ? a : v[R(12)] * Pow10(int(R(5)) - 2);
        c.x = Sign(B(a)); c.y = Sign(B(b));
        if (P(15)) c.y = Pk({0u, 0x80000000u});
        break;
    }
    case 13: {                                                                                                                                      // mutaciones de una lectura
        c.mutSite = 1u + R(5);
        auto Quadrant = [&] { c.x = Sign(Mag(-1, 3)); c.y = Sign(Mag(-1, 3)); };
        switch (c.mutSite) {
        case 1:                                                                                                                                     // el gate cambia tras la 1.a asociacion
            Quadrant();
            if (P(50)) { Idle(); c.mutA = 1u + R(255); c.mutB = P(50) ? 0u : 1u + R(100); }
            else { c.next = 1u + R(255); c.mutA = 0; c.mutB = 0; }
            break;
        case 2:                                                                                                                                     // los datos del jugador se sustituyen tras la 4.a asociacion
            Moving();
            if (P(50)) { c.x = Small(); c.y = Small(); c.xb = Sign(Mag(-1, 3)); c.yb = Sign(Mag(-1, 3)); }
            else { Quadrant(); c.xb = Small(); c.yb = Small(); }
            break;
        case 3: case 4: {                                                                                                                           // tras la 1.a AddAnimation: clump nuevo (3) o datos nuevos que no se releen (4)
            Moving();
            Quadrant();
            c.pres[0] = c.pres[1] = c.pres[2] = c.pres[3] = 0;
            if (P(15)) c.pres[R(4)] = 1;
            c.mutA = c.mutSite == 3 ? (Rand(n, nk()) | 0x00400000u) : General();
            c.mutB = General();
            break;
        }
        default:                                                                                                                                    // datos del jugador cambiados tras la 4.a asociacion
            Moving();
            if (P(50)) { c.x = Small(); c.y = Small(); c.mutA = Sign(Mag(-1, 3)); c.mutB = Sign(Mag(-1, 3)); }
            else { Quadrant(); c.mutA = Small(); c.mutB = Small(); }
            break;
        }
        break;
    }
    case 14: {                                                                                                                                      // las 16 combinaciones de asociaciones existentes, en movimiento
        Moving();
        const u32 pat = (n / 16u) & 15u;
        for (u32 i = 0; i < 4; ++i) c.pres[i] = (pat >> i) & 1u;
        c.x = General(); c.y = General();
        break;
    }
    default: {
        if (P(50)) Moving(); else if (P(30)) Idle();
        c.x = P(80) ? General() : Special(n, nk());
        c.y = P(80) ? General() : Special(n, nk());
        if (P(30)) { c.x = Small(); c.y = Small(); }
        if (P(25)) {
            c.mutSite = 1u + R(5);
            c.mutA = c.mutSite == 3 ? (Rand(n, nk()) | 0x00400000u) : (c.mutSite == 1 ? R(256) : General());
            c.mutB = c.mutSite == 1 ? R(256) : General();
        }
        break;
    }
    }
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("clump", c.clump).U("x", c.x).U("y", c.y).U("xb", c.xb).U("yb", c.yb)
     .A("pres", std::vector<u32>(c.pres, c.pres + 4)).A("amt", std::vector<u32>(c.amt, c.amt + 4)).A("dlt", std::vector<u32>(c.dlt, c.dlt + 4))
     .A("newamt", std::vector<u32>(c.newamt, c.newamt + 4)).A("newdlt", std::vector<u32>(c.newdlt, c.newdlt + 4))
     .U("next", c.next).U("combo", c.combo).U("last", c.last).U("mutsite", c.mutSite).U("muta", c.mutA).U("mutb", c.mutB);
    j.U("gate", c.gate);
    return j.End();
}
}

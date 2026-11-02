#pragma once
// Fixtures del banco Fn85G (lote 85G): una funcion, 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskSimpleFight::ChooseAttackPlayer                   0x624710  thiscall (tarea en ECX, ped), `ret 4`; devuelve AX (int16)   (hook CTaskSimpleFight / ChooseAttackPlayer, tipo S)
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

namespace F85GFix {
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
constexpr unsigned kCombos = 17;        // banderas de los combos 0..16

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kTaskWin = 0x28, kPedWin = 0x740, kTgtWin = 0x470, kIntWin = 0x170, kSMatWin = 0x40, kOthWin = 0x20, kOMatWin = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kTaskArena = Arena(kTaskWin), kPedArena = Arena(kPedWin), kTgtArena = Arena(kTgtWin), kIntArena = Arena(kIntWin), kSMatArena = Arena(kSMatWin), kOthArena = Arena(kOthWin), kOMatArena = Arena(kOMatWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 85007u); }
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
    u32 combo{}, next{}, move{}, last{}, chain{};   // bytes de la tarea: +0x24, +0x26, +0x25, +0x27, +0x15
    u32 animptr{};                                  // dword +0x1C (animacion de golpe): solo se mira si es nulo
    u32 tgt{}, ttype{}, tflags{};                   // objetivo (+0x18): presente, byte +0x36 y dword +0x46C
    u32 flags[kCombos]{};                           // banderas (byte +0x84 de la fila) de los combos 0..16
    u32 p72e{}, mstate{}, rot{}, aim{};             // ped: byte +0x72E, estado de movimiento +0x534, rumbo +0x558 y +0x55C inicial
    u32 smat{};                                     // el ped tiene matriz (+0x14)
    u32 spos[3]{};                                  // su posicion
    u32 opres[kOthers]{}, oalive[kOthers]{}, omat[kOthers]{};
    u32 opos[kOthers * 3]{};
    u32 reach{};                                    // valor que devuelve el falso de 0x61D6F0 (AL = 0 o 1; el resto es basura)
    u32 mutSite{}, mutA{}, mutB{}, mutC{}, mutD{}, mutPos[3]{};   // 1: en el falso de 0x61D6F0 cambian (siguiente, ultimo, movimiento, animacion); 2: id. cambian (tipo y banderas del objetivo);
                                                    // 3: en 0x5DEC00 cambian la bandera del combo y el byte +0x72E; 4: en la llamada mutA & 15 a 0x5E0170 cambia la posicion del ped consultado;
                                                    // 5: id. cambia el rumbo del ped; 6: id. cambia el elemento mutC & 15 de la lista
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
    auto Sign = [&](u32 bits) { return P(50) ? (bits | 0x80000000u) : (bits & 0x7FFFFFFFu); };
    auto Junk = [&](u32 al) { return (Rand(n, nk()) & ~0xFFu) | al; };
    // --- datos aleatorios base
    c.combo = P(80) ? R(17) : Pk({0u, 1u, 3u, 4u, 5u, 6u, 7u, 8u, 16u});
    c.next = R(256);
    c.move = P(70) ? R(8) : R(256);
    c.last = P(50) ? R(256) : c.next;
    c.chain = P(60) ? R(5) : R(256);
    c.animptr = P(60) ? 1u + R(0xFFFFFFFEu) : 0u;
    c.tgt = P(60) ? 1u : 0u;
    c.ttype = P(50) ? 3u + 8u * R(32) : R(256);
    c.tflags = P(40) ? 0x4000000u | (Rand(n, nk()) & 0xFBFFFFFFu) : (Rand(n, nk()) & ~0x4000000u);
    for (u32 i = 0; i < kCombos; ++i) c.flags[i] = R(256);
    c.p72e = P(50) ? R(256) : Pk({0u, 0xFFu});
    c.mstate = P(30) ? Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 100u}) : R(10);
    c.rot = P(15) ? Special(n, nk()) : Sign(B((0.0f + float(R(7000)) / 1000.0f)));
    c.aim = Rand(n, nk());
    c.smat = P(60) ? 1u : 0u;
    const float base = P(25) ? 0.0f : float(int(R(20001)) - 10000) / 10.0f;
    c.spos[0] = B(base + float(int(R(2001)) - 1000) / 1000.0f); c.spos[1] = B(base * 0.5f + float(int(R(2001)) - 1000) / 1000.0f); c.spos[2] = B(float(int(R(401)) - 200) / 10.0f);
    c.reach = Junk(P(50) ? 1u : 0u);
    auto Others = [&](u32 presPct, u32 alivePct, u32 maxRadius1000, u32 yScale) {
        for (u32 i = 0; i < kOthers; ++i) {
            c.opres[i] = P(presPct) ? 1u : 0u;
            c.oalive[i] = Junk(P(alivePct) ? 1u : 0u);
            c.omat[i] = P(50) ? 1u : 0u;
            const float radius = float(R(maxRadius1000 + 1)) / 1000.0f;
            const float angle = float(R(6284)) / 1000.0f;
            const float dz = float(int(R(1001)) - 500) / 1000.0f * float(yScale);
            c.opos[i * 3 + 0] = B(F(c.spos[0]) + radius * std::cos(angle));
            c.opos[i * 3 + 1] = B(F(c.spos[1]) + radius * std::sin(angle));
            c.opos[i * 3 + 2] = B(F(c.spos[2]) + dz);
        }
    };
    auto StartPlus = [&] {                  // el ataque empieza en 1: comando siguiente fuera de 0xB..0xE (con signo) o combo < 4
        if (P(55)) { c.next = P(50) ? R(0xB) : (0xF + R(0xF1)); c.combo = R(17); }
        else { c.next = 0xB + R(4); c.combo = R(4); }
    };
    auto StartMinus = [&] { c.next = 0xB + R(4); c.combo = 4 + R(13); };
    auto NotMove34 = [&] { do { c.move = P(70) ? R(8) : R(256); } while (c.move == 3 || c.move == 4); };
    auto Move34 = [&] { c.move = P(50) ? 3u : 4u; };
    auto Anim = [&] { c.animptr = 1u + R(0xFFFFFFFEu); };
    auto NoAnim = [&] { c.animptr = 0; };
    auto Reach = [&](bool v) { c.reach = Junk(v ? 1u : 0u); };
    // --- por familia
    switch (f) {
    case 0:                                                                                                                                         // el ataque empieza en 1
        StartPlus();
        if (P(70)) c.tgt = 1;
        if (!c.tgt) Others(70, 80, 2300, 1);
        break;
    case 1:                                                                                                                                         // con animacion y objetivo al alcance: -1
        StartMinus(); Anim(); NotMove34(); Reach(true);
        if (P(35)) c.tgt = 0;
        if (!c.tgt) Others(70, 80, 2300, 1);
        break;
    case 2:                                                                                                                                         // sin alcance, siguiente != ultimo, contador <= limite
        StartMinus(); Anim(); NotMove34(); Reach(false);
        if (c.last == c.next) c.last = (c.next + 1 + R(255)) & 0xFF;
        c.chain = P(30) ? Pk({0u, 1u, 2u, 0x80u, 0xFFu, 0xF0u}) : (P(50) ? R(3) : 0x80u + R(128));
        if (P(70)) c.tgt = 1;
        if (!c.tgt) Others(70, 80, 2300, 1);
        break;
    case 3:                                                                                                                                         // sin alcance, siguiente == ultimo
        StartMinus(); Anim(); NotMove34(); Reach(false);
        c.last = c.next;
        if (P(70)) c.tgt = 1;
        if (!c.tgt) Others(70, 80, 2300, 1);
        break;
    case 4:                                                                                                                                         // sin alcance, contador mayor que el limite
        StartMinus(); Anim(); NotMove34(); Reach(false);
        c.chain = P(20) ? Pk({3u, 4u, 0x7Fu}) : 3u + R(125);
        if (P(40)) c.last = c.next;
        if (P(70)) c.tgt = 1;
        if (!c.tgt) Others(70, 80, 2300, 1);
        break;
    case 5:                                                                                                                                         // sin animacion o con movimiento 3/4 y el ped corre
        StartMinus();
        if (P(50)) { NoAnim(); if (P(50)) Move34(); else NotMove34(); } else { Anim(); Move34(); }
        c.mstate = Pk({5u, 6u, 7u, 100u, 0x7FFFFFFFu, 5u + R(5)});
        Reach(P(50));
        if (P(70)) c.tgt = 1;
        if (!c.tgt) Others(70, 80, 2300, 1);
        break;
    case 6:                                                                                                                                         // sin animacion o con movimiento 3/4, no corre, sin alcance
        StartMinus();
        if (P(50)) { NoAnim(); if (P(50)) Move34(); else NotMove34(); } else { Anim(); Move34(); }
        c.mstate = Pk({0u, 1u, 2u, 3u, 4u, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFFFEu, R(5)});
        Reach(false);
        if (P(70)) c.tgt = 1;
        if (!c.tgt) Others(70, 80, 2300, 1);
        break;
    case 7:                                                                                                                                         // sin animacion, no corre, objetivo al alcance: 3 (combo a 4 segun banderas y objetivo)
        StartMinus(); NoAnim(); if (P(50)) Move34(); else NotMove34();
        c.mstate = Pk({0u, 1u, 2u, 3u, 4u, 0x80000000u, R(5)});
        Reach(true);
        c.tgt = P(75) ? 1u : 0u;
        if (c.tgt) { if (P(50)) { c.ttype = 3u + 8u * R(32); c.tflags = 0x4000000u | (Rand(n, nk()) & 0xFBFFFFFFu); } }
        if (P(50)) c.flags[c.combo] |= 4u;
        break;
    case 8:                                                                                                                                         // con animacion y movimiento 3/4, objetivo al alcance
        StartMinus(); Anim(); Move34();
        c.mstate = Pk({0u, 1u, 2u, 3u, 4u, 0x80000000u, R(5)});
        Reach(true);
        if (P(50)) c.last = c.next; else if (c.last == c.next) c.last = (c.next + 1 + R(255)) & 0xFF;
        c.tgt = P(75) ? 1u : 0u;
        if (c.tgt && P(50)) { c.ttype = 3u + 8u * R(32); c.tflags = 0x4000000u | (Rand(n, nk()) & 0xFBFFFFFFu); }
        if (P(50)) c.flags[c.combo] |= 4u;
        break;
    case 9:                                                                                                                                         // sin objetivo: peds vivos a menos de 2.0 de varios rumbos
        StartPlus(); c.tgt = 0;
        Others(75, 85, 1900, 1);
        break;
    case 10:                                                                                                                                        // distancias en el borde de 2.0 y rumbos empatados
        StartPlus(); c.tgt = 0;
        Others(85, 90, 2000, 1);
        for (u32 i = 0; i < kOthers; ++i) {
            if (P(40)) {
                const float r2 = P(50) ? 2.0f : F(NextUp(B(2.0f), int(R(9)) - 4));
                c.opos[i * 3 + 0] = B(F(c.spos[0]) + (P(50) ? r2 : -r2)); c.opos[i * 3 + 1] = c.spos[1]; c.opos[i * 3 + 2] = c.spos[2];
                if (P(40)) std::swap(c.opos[i * 3 + 0], c.opos[i * 3 + 1]);
            }
            if (P(25) && i > 0) { c.opos[i * 3 + 0] = c.opos[(i - 1) * 3 + 0]; c.opos[i * 3 + 1] = c.opos[(i - 1) * 3 + 1]; c.opos[i * 3 + 2] = c.opos[(i - 1) * 3 + 2]; }
        }
        break;
    case 11:                                                                                                                                        // valores especiales en posiciones y rumbo
        StartPlus(); c.tgt = 0;
        Others(85, 90, 1500, 1);
        for (u32 i = 0; i < kOthers; ++i) {
            if (P(35)) { const u32 k = R(3); c.opos[i * 3 + k] = Special(n, nk()); }
            if (P(10)) { c.opos[i * 3 + 0] = Pk({0u, 0x80000000u}); c.opos[i * 3 + 1] = Pk({0u, 0x80000000u}); c.opos[i * 3 + 2] = c.spos[2]; c.spos[0] = Pk({0u, 0x80000000u}); }
        }
        if (P(40)) c.rot = Special(n, nk());
        if (P(15)) { c.spos[0] = Special(n, nk()); }
        break;
    case 12:                                                                                                                                        // muertos, nulos y ninguno cercano
        StartPlus(); c.tgt = 0;
        Others(P(50) ? 20 : 60, P(50) ? 10 : 50, P(50) ? 2300 : 1900, 1);
        if (P(25)) for (u32 i = 0; i < kOthers; ++i) c.oalive[i] = Junk(0);
        if (P(20)) for (u32 i = 0; i < kOthers; ++i) c.opres[i] = 0;
        if (P(20)) for (u32 i = 0; i < kOthers; ++i) c.opos[i * 3] = B(F(c.spos[0]) + 3.0f + float(R(100)) / 10.0f);
        break;
    case 13: {                                                                                                                                      // mutaciones de una lectura
        c.mutSite = 1u + R(6);
        c.mutA = R(256); c.mutB = Rand(n, nk()); c.mutC = R(256); c.mutD = P(50) ? 1u + R(0xFFFFFFFEu) : 0u;
        for (u32 k = 0; k < 3; ++k) c.mutPos[k] = B(F(c.spos[k]) + float(int(R(3001)) - 1500) / 1000.0f);
        switch (c.mutSite) {
        case 1: StartMinus(); Anim(); NotMove34(); Reach(P(50)); if (c.last == c.next) c.last = (c.next + 1 + R(255)) & 0xFF; c.chain = R(3); if (P(70)) c.tgt = 1; break;
        case 2: StartMinus(); NoAnim(); NotMove34(); c.mstate = R(5); Reach(true); c.tgt = 1; c.ttype = P(50) ? 3u + 8u * R(32) : R(256); c.flags[c.combo] |= 4u;
                c.mutA = P(60) ? 3u + 8u * R(32) : R(256); c.mutB = P(60) ? (0x4000000u | (Rand(n, nk()) & 0xFBFFFFFFu)) : (Rand(n, nk()) & ~0x4000000u); break;
        case 3: StartMinus(); Anim(); NotMove34(); Reach(P(50)); c.last = P(50) ? c.next : (c.next + 1 + R(255)) & 0xFF; break;
        default: StartPlus(); c.tgt = 0; Others(85, 90, 1500, 1); c.mutA = R(16); c.mutC = R(16);
                 if (c.mutSite == 6) c.mutB = P(30) ? 1u : (R(16) << 1);
                 break;
        }
        break;
    }
    case 14:                                                                                                                                        // posicion en la matriz o compacta, del ped y de los demas
        StartPlus(); c.tgt = 0;
        Others(80, 85, 1900, 1);
        c.smat = R(2);
        for (u32 i = 0; i < kOthers; ++i) c.omat[i] = P(50) ? 1u : 0u;
        break;
    default: {
        if (P(40)) StartPlus(); else StartMinus();
        if (P(50)) Anim(); else NoAnim();
        if (P(50)) NotMove34(); else Move34();
        c.tgt = P(55) ? 1u : 0u;
        Reach(P(50));
        if (!c.tgt) Others(70, 80, 2300, 1);
        if (P(10)) { c.mutSite = 1u + R(6); c.mutA = R(256); c.mutB = Rand(n, nk()); c.mutC = R(256); c.mutD = P(50) ? 1u + R(0xFFFFFFFEu) : 0u;
                     for (u32 k = 0; k < 3; ++k) c.mutPos[k] = B(F(c.spos[k]) + float(int(R(3001)) - 1500) / 1000.0f);
                     if (c.mutSite >= 4) { c.mutA = R(16); c.mutC = R(16); if (c.mutSite == 6) c.mutB = P(30) ? 1u : (R(16) << 1); } }
        break;
    }
    }
    if (c.mutSite == 5) c.mutB = Sign(B(float(R(7000)) / 1000.0f));   // rumbo nuevo: un float sin cargas de NaN
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("combo", c.combo).U("next", c.next).U("move", c.move).U("last", c.last).U("chain", c.chain).U("animptr", c.animptr);
    j.U("tgt", c.tgt).U("ttype", c.ttype).U("tflags", c.tflags).A("flags", std::vector<u32>(c.flags, c.flags + kCombos));
    j.U("p72e", c.p72e).U("mstate", c.mstate).U("rot", c.rot).U("aim", c.aim).U("smat", c.smat).A("spos", std::vector<u32>(c.spos, c.spos + 3));
    j.A("opres", std::vector<u32>(c.opres, c.opres + kOthers)).A("oalive", std::vector<u32>(c.oalive, c.oalive + kOthers)).A("omat", std::vector<u32>(c.omat, c.omat + kOthers));
    j.A("opos", std::vector<u32>(c.opos, c.opos + kOthers * 3)).U("reach", c.reach);
    j.U("mutsite", c.mutSite).U("muta", c.mutA).U("mutb", c.mutB).U("mutc", c.mutC).U("mutd", c.mutD).A("mutpos", std::vector<u32>(c.mutPos, c.mutPos + 3));
    j.U("gate", c.gate);
    return j.End();
}
}

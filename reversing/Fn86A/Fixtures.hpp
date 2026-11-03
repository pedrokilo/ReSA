#pragma once
// Fixtures del banco Fn86A (lote 86A): una funcion, 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCam::ProcessArrestCamCheese                           0x512EF0  thiscall (camara en ECX, from, initialise), `ret 8`; devuelve AL   (hook CCam / ProcessArrestCamCheese, tipo S)
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

namespace F86AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;
constexpr unsigned kBones = 16;         // matrices de la jerarquia del ped

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kCamWin = 0x200, kEntWin = 0x40, kMatWin = 0x40, kPedWin = 0x40, kBoneWin = kBones * 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kCamArena = Arena(kCamWin), kEntArena = Arena(kEntWin), kMatArena = Arena(kMatWin), kPedArena = Arena(kPedWin), kBoneArena = Arena(kBoneWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 86008u); }
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
// Entidades que pueden ser el objetivo de la camara (0xB6F980): 0 = el objetivo inicial, 1 = el segundo (al que cambian las mutaciones 1 y 11).
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 init{};                                     // dword de pila del argumento `initialise` (el byte bajo es el bool; el resto es basura)
    u32 fromNull{};                                 // `from` nulo
    u32 rnd{};                                      // rand() del EXE (0..32767)
    u32 etype[2]{};                                 // byte +0x36 de las entidades 0 y 1
    u32 eflags[2]{};                                // dword +0x1C
    u32 emat[2]{};                                  // tienen matriz (+0x14)
    u32 epos[6]{}, ealt[6]{};                       // posicion en la ubicacion activa y en la otra (matriz +0x30 o compacta +4)
    u32 ms{}, f1{}, f2{}, st{}, b20{};              // tiempo (0xB7CB84), 0xB6EC60, 0xB70024, 0xB7001C y byte de 0xB70020
    u32 voff[3]{}, vzero[3]{};                      // 0xB70004..0xB7000C y 0xB70010..0xB70018 antes de la llamada
    u32 csrc[3]{}, cfov{}, cfront[3]{}, cup[3]{};   // camara: source (+0x19C), FOV (+0xB4), front (+0x190), up (+0x1B4) antes de la llamada
    u32 clump{}, hier{}, idx{}, bone[3]{};          // clump del ped (+0x18), jerarquia, indice de hueso (0..15) y posicion del hueso elegido
    u32 n1[3]{}, n2[3]{}, c1[3]{}, c2[3]{};         // lo que escriben los falsos: Normalise 1 y 2, CrossProduct 1 y 2
    u32 los1{}, los2{};                             // AL de las dos llamadas a 0x56A490 (con basura)
    u32 mutSite{}, mutVal{}, mutVal2{};
    u32 gate{};
};

// ------------------------------------------------------------------------------------------------ generador
inline void UnitVec(u32 c, u32 k, u32* out) {
    auto r = [&](u32 j) { return double(int(Rand(c, k + j) % 20001) - 10000) / 10000.0; };
    double x = r(1), y = r(2), z = r(3);
    double m = std::sqrt(x * x + y * y + z * z);
    if (m < 0.05) { x = 1.0; y = 0.0; z = 0.0; m = 1.0; }
    out[0] = B(float(x / m)); out[1] = B(float(y / m)); out[2] = B(float(z / m));
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
    auto Coord = [&] { return B(Fl(-3000.0f, 3000.0f)); };
    auto Geo = [&](u32 pct) { return f == 15 && P(pct) ? Special(n, nk()) : Coord(); };      // familia 15: valores especiales en la geometria
    // --- entidades
    for (u32 e = 0; e < 2; ++e) {
        const bool ped = e == 0 ? (f == 5 ? false : P(94)) : P(70);
        c.etype[e] = ped ? ((R(32) << 3) | 3u) : ((R(32) << 3) | Pk({0u, 1u, 2u, 4u, 5u, 6u, 7u}));
        c.eflags[e] = Rand(n, nk());
        c.emat[e] = P(60);
        for (u32 k = 0; k < 3; ++k) { c.epos[e * 3 + k] = Geo(15); c.ealt[e * 3 + k] = Coord(); }
    }
    // --- estado de la camara
    for (u32 k = 0; k < 3; ++k) { c.csrc[k] = Geo(12); c.cfront[k] = Rand(n, nk()); c.cup[k] = Rand(n, nk()); }
    c.cfov = P(70) ? B(Fl(10.0f, 120.0f)) : Rand(n, nk());
    c.clump = Rand(n, nk()); c.hier = Rand(n, nk());
    c.idx = P(45) ? 0u : R(kBones);
    for (u32 k = 0; k < 3; ++k) c.bone[k] = Geo(15);
    for (u32 k = 0; k < 3; ++k) { c.voff[k] = P(85) ? B(Fl(-2.0f, 2.0f)) : Special(n, nk()); c.vzero[k] = P(70) ? B(Fl(-50.0f, 50.0f)) : Rand(n, nk()); }
    // --- indicadores estaticos
    c.st = (f == 13 ? R(4) : (P(70) ? 3u : R(4))) | (R(0x4000u) << 2);
    c.b20 = P(45) ? 0u : R(256);
    // --- rand() y lineas de vista
    c.rnd = P(75) ? R(21200) : R(32768);
    c.los1 = Junk(P(90) ? 1u : 0u); c.los2 = Junk(P(85) ? 1u : 0u);
    // --- tiempo: modo 0 t < 0, 1 t en [0, 1), 2 t == 1 exacto, 3 t > 1
    auto Time = [&](int mode, bool negMs) {
        if (mode == 2 && !negMs) {
            c.ms = 100000u + R(4000000u);
            const float t2 = float(R(5000));
            c.f2 = B(t2);
            c.f1 = B(float(i32(c.ms) - 1000 - i32(t2)));
            return;
        }
        c.ms = negMs ? 0x80000000u + R(0x7FFFFF00u) : (P(70) ? 100000u + R(15000000u) : 100000u + R(0x7FFE0000u));
        const double d = mode == 0 ? -(1.0 + double(R(5000))) : (mode == 1 ? double(R(1000000)) / 1000.0 : (mode == 2 ? 1000.0 : 1001.0 + double(R(200000))));
        const float t2 = P(60) ? 0.0f : float(R(3000));
        c.f2 = B(t2);
        c.f1 = B(float(double(c.ms) - d - double(t2)));
    };
    switch (f) {
    case 7: Time(0, false); break;
    case 8: Time(1, false); break;
    case 9: Time(2, false); break;
    case 10: Time(3, false); break;
    case 12: Time(int(R(4)), true); break;
    case 11:
        Time(int(R(4)), false);
        if (P(50)) c.f1 = Special(n, nk());
        if (P(50)) c.f2 = Special(n, nk());
        if (P(20)) c.ms = R(3);
        break;
    default: Time(int(R(4)), false); break;
    }
    // --- falsos
    UnitVec(n, nk(), c.n1); UnitVec(n, nk(), c.n2);
    for (u32 k = 0; k < 3; ++k) { c.c1[k] = Geo(25); c.c2[k] = Geo(25); }
    if (f == 15 && P(30)) { for (u32 k = 0; k < 3; ++k) { c.n1[k] = Special(n, nk()); c.n2[k] = Special(n, nk()); } }
    // --- familias
    c.init = Junk(P(55) ? 1u : 0u);
    c.fromNull = P(2) ? 1u : 0u;
    switch (f) {
    case 0: c.init = Junk(0); break;
    case 1: c.init = Junk(1); c.rnd = R(21200); c.los1 = Junk(1); c.los2 = Junk(1); c.fromNull = 0; c.etype[0] = (R(32) << 3) | 3u; break;
    case 2: c.init = Junk(1); c.rnd = P(80) ? 21300u + R(11468u) : Pk({21297u, 21298u, 21299u, 21300u, 21301u, 21302u}); break;
    case 3: c.init = Junk(1); c.rnd = R(21200); c.los1 = Junk(0); c.fromNull = 0; c.etype[0] = (R(32) << 3) | 3u; break;
    case 4: c.init = Junk(1); c.rnd = R(21200); c.los1 = Junk(1); c.los2 = Junk(0); c.fromNull = 0; c.etype[0] = (R(32) << 3) | 3u; break;
    case 6: c.fromNull = 1; break;
    case 14: {
        c.mutSite = 1 + R(11);
        c.etype[1] = P(80) ? ((R(32) << 3) | 3u) : c.etype[1];
        c.fromNull = 0;
        if (c.mutSite <= 9) { c.init = Junk(1); c.rnd = R(21200); }
        if (c.mutSite == 11) { c.init = Junk(1); c.rnd = R(21200); c.los1 = Junk(1); }
        switch (c.mutSite) {
        case 2: c.mutVal = P(25) ? 0x80000000u + R(1000u) : 100000u + R(5000000u); break;
        case 3: c.mutVal = B(Fl(0.0f, 5000000.0f)); break;
        case 4: c.mutVal = B(Fl(0.0f, 3000.0f)); break;
        case 5: c.mutVal = Rand(n, nk()); break;
        case 6: c.mutVal = R(4) | (R(1000u) << 2); break;
        case 7: c.mutVal = B(Fl(-5.0f, 5.0f)); break;
        case 9: c.mutVal = R(256); break;
        case 10: c.mutVal = Coord(); break;
        default: break;
        }
        break;
    }
    default: break;
    }
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    auto V = [](const u32* p, std::size_t k) { return std::vector<u32>(p, p + k); };
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("init", c.init).U("fromnull", c.fromNull).U("rnd", c.rnd);
    j.A("etype", V(c.etype, 2)).A("eflags", V(c.eflags, 2)).A("emat", V(c.emat, 2)).A("epos", V(c.epos, 6)).A("ealt", V(c.ealt, 6));
    j.U("ms", c.ms).U("f1", c.f1).U("f2", c.f2).U("st", c.st).U("b20", c.b20).A("voff", V(c.voff, 3)).A("vzero", V(c.vzero, 3));
    j.A("csrc", V(c.csrc, 3)).U("cfov", c.cfov).A("cfront", V(c.cfront, 3)).A("cup", V(c.cup, 3));
    j.U("clump", c.clump).U("hier", c.hier).U("idx", c.idx).A("bone", V(c.bone, 3));
    j.A("n1", V(c.n1, 3)).A("n2", V(c.n2, 3)).A("c1", V(c.c1, 3)).A("c2", V(c.c2, 3)).U("los1", c.los1).U("los2", c.los2);
    j.U("mutsite", c.mutSite).U("mutval", c.mutVal).U("mutval2", c.mutVal2).U("gate", c.gate);
    return j.End();
}
}

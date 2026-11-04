#pragma once
// Fixtures del banco Fn89A (lote 89A): una funcion, 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCamera::IsItTimeForNewCamera                          0x51D770  thiscall (ECX = camara; camSequence y startTime en pila), `ret 8`; devuelve AL   (hook CCamera / IsItTimeForNewCamera, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Secuencia = n % 32 (0..29 los treinta casos del despacho, 30 negativa, 31 mayor de 29). Familia = (n / 32) % 16 (32 casos por secuencia y familia).
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

namespace F89AFix {
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
constexpr std::size_t kCamWin = 0xE00, kEntWin = 0x40, kVehWin = 0x600, kMatWin = 0x40, kSpdWin = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kCamArena = Arena(kCamWin), kEntArena = Arena(kEntWin), kVehArena = Arena(kVehWin), kMatArena = Arena(kMatWin), kSpdArena = Arena(kSpdWin);
// Desplazamiento (respecto al objeto de la arena) del vector de velocidad del jugador
constexpr std::size_t kSpdOff = 0x10;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 89001u); }
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
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 seq{}, start{}, now{};                      // camSequence (int32), startTime (int32), CTimer::m_snTimeInMilliseconds (0xB7CB84)
    u32 stick{};                                    // RightStickX del pad 0 (int16 en 16 bits)
    u32 alert{}, step{}, gate{};                    // bytes iniciales de 0x8CCDF4, 0x8CC471 y 0xB6F059
    u32 active{};                                   // m_nActiveCam (+0x59)
    u32 hasVeh{}, vehType{}, vehModel{}, tgtModel{};
    u32 tmat{};                                     // el objetivo de la camara tiene matriz (+0x14)
    u32 tpos[3]{}, talt[3]{}, csrc[3]{};            // posicion del objetivo (activa y la otra) y origen de la camara activa
    u32 fixedSrc[3]{}, pcoords[3]{}, speed[3]{};    // origen fijo (+0x86C), coordenadas y velocidad del jugador
    u32 los{}, onscreen{}, dw{};                    // resultados de los falsos
    u32 i138{}, i31{};                              // valores iniciales de +0x138 (float) y +0x31 (byte) de la camara
    u32 mutSite{}, mutVal{};
};

// ------------------------------------------------------------------------------------------------ generador
inline constexpr std::array<double, 12> kThresholds{1.0, 2.0, 3.0, 4.5, 29.0, 36.0, 38.0, 40.0, 44.0, 48.0, 50.0, 57.0};

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.ecx = Rand(n, 3); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    const u32 sc = n % 32, f = (n / 32) % 16;
    c.family = f;
    u32 kk = 100;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    auto Fl = [&](float lo, float hi) { return lo + (hi - lo) * (float(R(100001)) / 100000.0f); };
    auto Coord = [&] { return B(Fl(-3000.0f, 3000.0f)); };

    // --- secuencia
    if (sc < 30) c.seq = sc;
    else if (sc == 30) { const u32 k = R(8); c.seq = k == 0 ? 0x80000000u : (k == 1 ? 0xFFFFFFFFu : (k == 2 ? 0xFFFFFFE2u : 0x80000000u + R(0x7FFFFFFFu))); }
    else { const u32 k = R(8); c.seq = k == 0 ? 30u : (k == 1 ? 31u : (k == 2 ? 0x7FFFFFFFu : (k == 3 ? 0xFFu : 30u + R(1000)))); }

    // --- tiempo: ahora = inicio + transcurrido (en 32 bits); el transcurrido depende de la familia
    c.start = P(30) ? Rand(n, nk()) : (P(50) ? 0xFFFFFF00u + R(0x100) : R(2000000));
    if (P(10)) c.start = 0x7FFFFF00u + R(0x200);
    i32 delta;
    auto Small = [&] { const u32 k = R(10); return k < 4 ? i32(R(1200)) : (k < 6 ? i32(1200 + R(2200)) : i32(R(15000))); };
    switch (f) {
    case 0: delta = i32(R(1200)); break;
    case 1: { const i32 t[] = {1000, 2000, 3000, 5000}; delta = t[R(4)] + i32(R(9)) - 4; break; }
    case 2: delta = 15000 + i32(R(21)) - 10; break;
    case 3: delta = 20000 + i32(R(21)) - 10; break;
    case 4: delta = i32(R(40000)); break;
    case 7: delta = -i32(R(30000)); break;
    case 12: delta = i32(Rand(n, nk())); break;
    default: delta = P(88) ? Small() : (P(50) ? i32(R(40000)) : i32(Rand(n, nk()))); break;
    }
    c.now = c.start + u32(delta);

    // --- palo derecho, aviso y paso
    const i32 sticks[] = {0, 32, -32, 33, -33, 96, -96, 97, -97, 127, -128, 1, -1, 64, -64, 100, -100, 32767, -32768, 255};
    if (f == 5 || f == 6) c.stick = u32(u16(i16(P(30) ? sticks[R(20)] : i32(R(256)) - 128)));
    else if (P(75)) c.stick = u32(u16(i16(i32(R(65)) - 32)));          // |palo| <= 32 casi siempre: arma el aviso
    else c.stick = u32(u16(i16(P(40) ? sticks[R(20)] : i32(R(256)) - 128)));
    c.alert = P(f == 5 ? 60 : 50) ? 1u : (P(15) ? 0xA5u : 0u);
    c.step = P(50) ? 1u : 0xFFu;
    c.gate = P(50) ? Rand(n, nk()) & 0xFF : 1u;

    // --- camara y entidades
    c.active = R(4);
    c.hasVeh = P(f == 9 ? 50 : 70);
    const u32 types[] = {0, 1, 2, 3, 4, 5, 5, 5, 6, 7, 8};
    c.vehType = f == 9 ? (P(60) ? 5u : types[R(11)]) : (P(25) ? 5u : types[R(11)]);
    c.vehModel = P(15) ? 0x1B0u : (P(30) ? R(0x10000) : 0x1B0u + R(40) - 20);
    c.tgtModel = P(40) ? 0x1CCu : (P(30) ? R(0x10000) : 0x1CCu + R(7) - 3);
    c.tmat = P(60);
    for (u32 k = 0; k < 3; ++k) { c.tpos[k] = Coord(); c.talt[k] = Coord(); c.csrc[k] = Coord(); c.fixedSrc[k] = Coord(); }
    if (P(8)) for (u32 k = 0; k < 3; ++k) c.csrc[k] = c.tpos[k];

    // --- distancia del jugador al origen fijo: alrededor de un umbral de los casos 1, 2, 3, 5 y 15..19
    const double T = kThresholds[R(12)];
    const bool nearT = P(f == 8 ? 90 : 55);
    double r;
    if (P(8) || (f == 11 && P(30))) r = 0.0;
    else if (nearT) r = T * (1.0 + double(int(R(2001)) - 1000) / (P(50) ? 100000.0 : 3000000.0));
    else r = double(R(70000)) / 1000.0;
    {
        const double ang = double(R(62832)) / 10000.0;
        const double z = (sc == 18 || P(30)) ? double(int(R(2001)) - 1000) / 1000.0 : 0.0;   // el caso 18 conserva la z
        const double rr = std::sqrt(std::max(0.0, 1.0 - z * z));
        const double dx = rr * std::cos(ang), dy = rr * std::sin(ang);
        c.pcoords[0] = B(float(double(F(c.fixedSrc[0])) + r * dx));
        c.pcoords[1] = B(float(double(F(c.fixedSrc[1])) + r * dy));
        c.pcoords[2] = B(float(double(F(c.fixedSrc[2])) + r * z));
        // velocidad: alineada o contraria al desplazamiento, perpendicular, nula o aleatoria
        const u32 sk = R(8);
        const double mag = double(R(3000)) / 100.0;
        if (sk <= 2) { const double sgn = sk == 0 ? 1.0 : -1.0; c.speed[0] = B(float(sgn * mag * dx)); c.speed[1] = B(float(sgn * mag * dy)); c.speed[2] = B(float(double(int(R(2001)) - 1000) / 100.0)); }
        else if (sk == 3) { c.speed[0] = B(float(-mag * dy)); c.speed[1] = B(float(mag * dx)); c.speed[2] = B(Fl(-10.0f, 10.0f)); }   // dot ~ 0
        else if (sk == 4) { c.speed[0] = 0; c.speed[1] = 0; c.speed[2] = P(50) ? 0u : B(Fl(-5.0f, 5.0f)); }
        else { for (u32 k = 0; k < 3; ++k) c.speed[k] = B(Fl(-30.0f, 30.0f)); }
    }
    if (f == 15 || (f == 11 && P(30))) {
        for (u32 k = 0; k < 3; ++k) {
            if (P(20)) c.pcoords[k] = Special(n, nk());
            if (P(15)) c.fixedSrc[k] = Special(n, nk());
            if (P(20)) c.speed[k] = Special(n, nk());
            if (P(10)) c.csrc[k] = Special(n, nk());
            if (P(10)) c.tpos[k] = Special(n, nk());
        }
    }

    // --- resultados de los falsos
    c.i138 = P(30) ? Special(n, nk()) : Coord();
    c.i31 = P(50) ? 1u : R(256);
    c.los = P(f == 10 ? 50 : 82);
    c.onscreen = P(50);
    c.dw = P(50);
    if (f == 14) { c.onscreen = R(2); c.dw = R(2); }
    // --- mutaciones: una escritura dentro de un falso (el origen fijo o el jugador, que el EXE relee)
    if ((f == 13 && P(85)) || P(5)) { c.mutSite = 1 + R(4); c.mutVal = P(30) ? Special(n, nk()) : Coord(); }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    auto V = [](const u32* p, std::size_t k) { return std::vector<u32>(p, p + k); };
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("seq", c.seq).U("start", c.start).U("now", c.now).U("stick", c.stick).U("alert", c.alert).U("step", c.step).U("gate", c.gate).U("active", c.active);
    j.U("hasveh", c.hasVeh).U("vehtype", c.vehType).U("vehmodel", c.vehModel).U("tgtmodel", c.tgtModel).U("tmat", c.tmat);
    j.A("tpos", V(c.tpos, 3)).A("talt", V(c.talt, 3)).A("csrc", V(c.csrc, 3)).A("fixed", V(c.fixedSrc, 3)).A("pcoords", V(c.pcoords, 3)).A("speed", V(c.speed, 3));
    j.U("los", c.los).U("onscreen", c.onscreen).U("dw", c.dw).U("i138", c.i138).U("i31", c.i31).U("mutsite", c.mutSite).U("mutval", c.mutVal);
    return j.End();
}
}

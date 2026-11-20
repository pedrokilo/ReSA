#pragma once
// Fixtures del banco FnPW1: CVehicle::ProcessWheel (EXE 0x6D6C00), 18432 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CVehicle::ProcessWheel  0x6D6C00  thiscall, ECX = this, 12 dwords (wheelFwd*, wheelRight*, wheelContactSpeed*, wheelContactPoint*, wheelsOnGround, thrust, brake, adhesion,
//                                       wheelId (byte), wheelSpeed*, wheelState*, wheelStatus (word)), `ret 0x30`, void (hook CVehicle / ProcessWheel, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) y las familias 16 y 17 (1024 casos cada una).
// Entradas que este banco anade a las del banco anterior (CVehD): brake NaN (+ y -), +-inf, 0, -0; adhesion 0, negativa, NaN, minuscula; thrust NaN; velocidades de contacto y vectores de rueda
// con componentes desnormales (1e-40..1e-30) y casi 0; ruedas en el suelo 0..4; estado de rueda de entrada NORMAL/SPINNING/SKIDDING/FIXED; las tres estaticas 0xC1CDAC/AD/AE en sus dos valores;
// tipos de vehiculo (coche, moto, avion), RCBANDIT; GasPedal 0 y no 0; estado del vehiculo STATUS_PLAYER/REMOTE_CONTROLLED/otros; wheelStatus normal y pinchada; multiplicador antihundimiento.
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

namespace PW1Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;    // familias 0..15
constexpr unsigned kExtraFamilies = 4;    // familias 16..19, 1024 casos cada una
constexpr unsigned kCases = kBaseCases + kExtraFamilies * 1024;
constexpr unsigned kFamilies = 16 + kExtraFamilies;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x990, kHandWin = 0x100, kArgWin = 0x40, kStateWin = 0x10, kSpeedWin = 0x10;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kHandArena = Arena(kHandWin), kArgArena = Arena(kArgWin), kStateArena = Arena(kStateWin), kSpeedArena = Arena(kSpeedWin);

constexpr u32 kTimeStep = 0xB7CB5C;                       // CTimer::ms_fTimeStep
constexpr u32 kWheelFriction = 0xC2B9CC;                  // gHandlingDataMgr.fWheelFriction
constexpr u32 kFlagsBase = 0xC1CDAC, kFlagsBytes = 8;     // estaticas de la rueda (0xC1CDAC..0xC1CDB3); esta funcion usa 0xC1CDAC (ya patinaba), 0xC1CDAD (empuje) y 0xC1CDAE (freno)
constexpr u32 kFlagAlready = 0xC1CDAC, kFlagDrive = 0xC1CDAD, kFlagBrake = 0xC1CDAE;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 0x50571u); }
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
// NaN: el indefinido de la x87 (negativo) y el positivo. Los dos patrones solo se mezclan en brake, thrust, adhesion (que solo se comparan o se multiplican por un finito): con dos NaN distintos
// en una operacion aritmetica la carga que sale depende del orden de operandos que elija el compilador.
inline constexpr std::array<u32, 4> kNan{0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u};
inline constexpr std::array<u32, 2> kNanBoth{0x7FC00000u, 0xFFC00000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
inline constexpr std::array<u32, 14> kUnit{0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x40000000u, 0xC0000000u, 0x3F7FFFFFu, 0xBF7FFFFFu, 0x3F800001u, 0xBF800001u, 0x3DCCCCCDu, 0x3F99999Au, 0x3FA66666u, 0x41200000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi] (normales), signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
// magnitud 10^e (e entre lo y hi, con decimales), signo aleatorio: 1e-40 (desnormal) .. 1e-30
inline u32 DecadeFloat(u32 c, u32 k, double lo, double hi) {
    const double e = lo + double(Rand(c, k) % 10001) / 10000.0 * (hi - lo);
    const double v = std::pow(10.0, e);
    return B(float(Rand(c, k + 1) % 2 ? v : -v));
}
inline u32 Ulps(u32 bits, int k) {
    const u32 sign = bits & 0x80000000u;
    i32 mag = i32(bits & 0x7FFFFFFFu) + k;
    if (mag < 0) mag = 0;
    if (mag > 0x7F7FFFFF) mag = 0x7F7FFFFF;
    return sign | u32(mag);
}
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Un float: modo 0 moderado, 1 pequeno, 2 grande, 3 especial (cero, desnormal, inf, NaN), 4 unitario, 5 mezcla
inline u32 FloatOf(u32 c, u32 k, u32 mode) {
    const u32 r = Rand(c, k) % 32;
    switch (mode) {
    case 0: return r == 0 ? B(0.0f) : RandFloat(c, k + 2, -4, 6);
    case 1: return r < 3 ? Pick(kTiny, c, k + 1) : RandFloat(c, k + 2, -80, -20);
    case 2: return r < 3 ? Pick(kBig, c, k + 1) : RandFloat(c, k + 2, 20, 120);
    case 3:
        if (r < 8) return Pick(kTiny, c, k + 1);
        if (r < 13) return Pick(kInf, c, k + 1);
        if (r < 21) return Pick(kNan, c, k + 1);
        if (r < 24) return Pick(kBig, c, k + 1);
        return RandFloat(c, k + 2, -20, 30);
    case 4: return r < 4 ? RandFloat(c, k + 2, -2, 2) : Pick(kUnit, c, k + 1);
    default: {
        const u32 m = Rand(c, k + 3) % 5;
        return FloatOf(c, k + 5, m == 4 ? 3u : m);
    }
    }
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    // argumentos
    u32 fwd[3]{}, right[3]{}, cs[3]{}, cp[3]{};
    u32 wheels{}, thrust{}, brake{}, adh{}, wheelId{}, wstate{}, status{};
    // vehiculo
    u32 gas{}, vtype{}, subtype{}, model{}, mass{}, turn{}, hmass{}, tl{}, sadm{}, vstat{};   // vstat = byte 0x36 (tipo en los 3 bits bajos, estado en los 5 altos)
    // globales
    u32 ts{}, fric{}, af{}, b1{}, b2{}, randSeed{};
};

inline void Split3(u32 c, u32 k, double len, double out[3]) {
    double a = double(Rand(c, k) % 2001) / 1000.0 - 1.0;
    double b = double(Rand(c, k + 1) % 2001) / 1000.0 - 1.0;
    double d = double(Rand(c, k + 2) % 2001) / 1000.0 - 1.0;
    double m = a * a + b * b + d * d;
    if (m < 1e-6) { a = 1.0; b = 0.0; d = 0.0; m = 1.0; }
    const double s = len / std::sqrt(m);
    out[0] = a * s; out[1] = b * s; out[2] = d * s;
}

inline u32 StatusByte(u32 n, u32 k, u32 status) { return (status << 3) | (Rand(n, k) & 7u); }

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    auto fl = [&](u32 k, u32 mode = 0u) { return FloatOf(n, k, mode); };
    auto vec3 = [&](u32* dst, u32 k, double len) { double d[3]; Split3(n, k, len, d); for (int i = 0; i < 3; ++i) dst[i] = B(float(d[i])); };
    auto logmag = [&](u32 k, double lo, double hi) { return std::pow(10.0, std::log10(lo) + double(Rand(n, k) % 2001) / 2000.0 * (std::log10(hi) - std::log10(lo))); };
    // ---- base: caso plausible
    vec3(c.fwd, 10, 1.0); vec3(c.right, 14, 1.0);
    vec3(c.cs, 26, logmag(30, 0.001, 2.0));
    for (int i = 0; i < 3; ++i) c.cp[i] = B(float(double(Rand(n, 31 + i) % 4001) / 1000.0 - 2.0));
    c.wheels = Rand(n, 35) % 12 == 0 ? Rand(n, 36) % 9u : (Rand(n, 37) % 2 ? 4u : 1u + Rand(n, 38) % 4u);
    c.thrust = Rand(n, 39) % 3 == 0 ? B(0.0f) : B(float(int(Rand(n, 40) % 2001) - 1000) / 1000.0f);
    c.brake = Rand(n, 41) % 5 < 3 ? B(0.0f) : B(float(Rand(n, 42) % 2001) / 1000.0f);
    c.adh = B(float(Rand(n, 43) % 2001) / 1000.0f);
    c.wheelId = Rand(n, 46) % 5 == 0 ? Rand(n, 47) : Rand(n, 48) % 4;
    c.wstate = Rand(n, 49) % 3 == 0 ? Rand(n, 50) % 4 : 0u;
    c.status = Rand(n, 55) % 5 == 0 ? 1u : (Rand(n, 56) % 8 == 0 ? Rand(n, 57) : 0u);
    c.gas = Rand(n, 59) % 2 == 0 ? B(float(int(Rand(n, 60) % 41) - 20) / 4000.0f) : B(float(int(Rand(n, 61) % 2001) - 1000) / 1000.0f);
    {
        static constexpr std::array<u32, 10> vt{0u, 0u, 0u, 9u, 0u, 2u, 0u, 11u, 3u, 10u};
        c.vtype = Rand(n, 62) % 10 == 0 ? Rand(n, 63) : vt[Rand(n, 64) % vt.size()];
        c.subtype = Rand(n, 65) % 10 == 0 ? Rand(n, 66) : (Rand(n, 67) % 6 == 0 ? 4u : vt[Rand(n, 68) % vt.size()]);
    }
    c.model = Rand(n, 69) % 8 == 0 ? 0x1B9u : Rand(n, 70) & 0xFFFFu;
    c.mass = B(float(20 + Rand(n, 71) % 2000));
    c.turn = B(float(10 + Rand(n, 72) % 3000));
    c.hmass = Rand(n, 73) % 3 == 0 ? B(float(20 + Rand(n, 74) % 1000)) : c.mass;
    c.tl = Rand(n, 75) % 8 == 0 ? B(1.0f) : B(float(Rand(n, 76) % 1501) / 1000.0f);
    c.sadm = Rand(n, 77) % 3 == 0 ? B(0.0f) : B(float(Rand(n, 78) % 2001) / 1000.0f);
    {
        const u32 w = Rand(n, 79) % 6;   // estado del vehiculo (0 = STATUS_PLAYER, 8 = STATUS_REMOTE_CONTROLLED)
        c.vstat = StatusByte(n, 80, w < 2 ? 0u : w < 3 ? 8u : Rand(n, 81) % 32);
    }
    c.ts = B(0.5f + float(Rand(n, 82) % 3000) / 1000.0f);
    c.fric = B(0.01f + float(Rand(n, 83) % 3000) / 1000.0f);
    c.af = Rand(n, 84) % 3 == 0 ? (Rand(n, 85) % 8 == 0 ? (Rand(n, 86) % 255u) + 1u : 1u) : 0u;
    c.b1 = Rand(n, 87) % 2; c.b2 = Rand(n, 88) % 2;
    c.randSeed = Rand(n, 89) % 12 == 0 ? Pick(kStackSeeds, n, 90) : Rand(n, 91);
    // ---- por familia
    auto brakeOff = [&] { c.brake = Rand(n, 108) % 4 == 0 ? 0x80000000u : 0u; };
    auto thrustOn = [&] { if (F(c.thrust) == 0.0f || F(c.thrust) != F(c.thrust)) c.thrust = B(float(1 + Rand(n, 110) % 1000) / 1000.0f * (Rand(n, 111) % 2 ? 1.0f : -1.0f)); };
    auto speedsFor = [&](double rsMag, double fsMag) {   // contacto: cs = fwd * fs + right * rs (aprox.)
        double fvec[3], rvec[3];
        for (int i = 0; i < 3; ++i) { fvec[i] = double(F(c.fwd[i])); rvec[i] = double(F(c.right[i])); }
        const double fs = (Rand(n, 112) % 2 ? 1.0 : -1.0) * fsMag, rs = (Rand(n, 113) % 2 ? 1.0 : -1.0) * rsMag;
        for (int i = 0; i < 3; ++i) c.cs[i] = B(float(fvec[i] * fs + rvec[i] * rs));
    };
    auto statusPick = [&](u32 k) {   // 0 = jugador, 8 = control remoto, otros
        const u32 w = Rand(n, k) % 4;
        c.vstat = StatusByte(n, k + 1, w == 0 ? 0u : w == 1 ? 8u : Rand(n, k + 2) % 32);
    };
    switch (f) {
    case 0:   // marcha con empuje, sin freno: lateral limitado a +-adhesion
        brakeOff(); thrustOn(); c.status = 0;
        speedsFor(logmag(122, 0.001, 2.0), logmag(123, 0.001, 2.0));
        if (Rand(n, 124) % 4 == 0) c.adh = B(float(1 + Rand(n, 125) % 300) / 1000.0f);
        break;
    case 1:   // freno aplicado: la fuerza hacia delante se frena hasta el freno, y rueda fija
        c.brake = B(float(logmag(124, 0.01, 3.0)));
        if (Rand(n, 125) % 3 == 0) c.adh = B(F(c.brake) * (0.5f + float(Rand(n, 126) % 1500) / 1000.0f));
        speedsFor(logmag(127, 0.001, 1.0), logmag(128, 0.0005, 2.0));
        if (Rand(n, 129) % 5 == 0) c.status = 0;
        break;
    case 2:   // sin freno ni empuje: freno pasivo segun tipo (moto, avion, coche, masa ligera, RC Bandit) y GasPedal 0 / no 0
        brakeOff(); c.thrust = B(Rand(n, 130) % 2 ? 0.0f : -0.0f);
        c.gas = Rand(n, 131) % 6 == 0 ? fl(132, 4u) : (Rand(n, 133) % 5 == 0 ? B(0.0f) : B(float(int(Rand(n, 134) % 41) - 20) / 3000.0f));
        speedsFor(logmag(135, 0.0005, 1.0), logmag(136, 0.0003, 0.5));
        if (Rand(n, 137) % 3 == 0) c.cs[0] = c.cs[1] = c.cs[2] = 0u;
        {
            const u32 t = Rand(n, 138) % 4;   // 0 coche, 1 moto, 2 avion, 3 coche con masa ligera / RC Bandit
            c.vtype = t == 1 ? 9u : 0u; c.subtype = t == 2 ? 4u : 0u;
            if (t == 3) { c.hmass = B(float(100 + Rand(n, 139) % 800)); if (Rand(n, 140) % 2) c.model = 0x1B9u; }
        }
        if (Rand(n, 141) % 5 == 0) c.model = 0x1B9u;
        if (Rand(n, 142) % 4 == 0) c.hmass = Ulps(B(500.0f), int(Rand(n, 143) % 5) - 2);
        break;
    case 3:   // brake NaN (+ y -), +-inf, 0, -0 y minusculos
        {
            static constexpr std::array<u32, 12> bs{0x7FC00000u, 0xFFC00000u, 0x7F800000u, 0xFF800000u, 0u, 0x80000000u, 1u, 0x80000001u, 0x00400000u, 0x7FC00000u, 0xFFC00000u, 0x3F800000u};
            c.brake = bs[Rand(n, 144) % bs.size()];
            speedsFor(logmag(145, 0.001, 2.0), logmag(146, 0.001, 2.0));
            if (Rand(n, 147) % 3 == 0) { c.thrust = Pick(kTiny, n, 148); }
            if (Rand(n, 149) % 4 == 0) c.adh = Pick(kNanBoth, n, 150);
        }
        break;
    case 4:   // adhesion 0, negativa, NaN, muy pequena (con freno o sin el)
        {
            static constexpr std::array<u32, 12> as{0u, 0x80000000u, 0xBF800000u, 0xBE4CCCCDu, 0x7FC00000u, 0xFFC00000u, 1u, 0x00800000u, 0x80000001u, 0x3727C5ACu, 0x7F800000u, 0xFF800000u};
            c.adh = as[Rand(n, 151) % as.size()];
            if (Rand(n, 152) % 4 == 0) c.adh = DecadeFloat(n, 153, -45.0, -30.0);
            speedsFor(logmag(155, 0.001, 2.0), logmag(156, 0.001, 2.0));
            if (Rand(n, 157) % 2) { brakeOff(); thrustOn(); } else if (Rand(n, 158) % 2) { brakeOff(); c.thrust = B(0.0f); }
            else c.brake = B(float(logmag(159, 0.001, 3.0)));
        }
        break;
    case 5:   // thrust NaN (+ y -), +-inf, 0, -0 y minusculos
        {
            static constexpr std::array<u32, 10> ts{0x7FC00000u, 0xFFC00000u, 0x7F800000u, 0xFF800000u, 0u, 0x80000000u, 1u, 0x80000001u, 0x7FC00000u, 0xFFC00000u};
            c.thrust = ts[Rand(n, 160) % ts.size()];
            brakeOff();
            speedsFor(logmag(161, 0.001, 2.0), logmag(162, 0.001, 2.0));
        }
        break;
    case 6:   // rueda pinchada: ruido con rand() del EXE, tope a 0.3 sobre la velocidad hacia delante
        c.status = 1u;
        {
            static constexpr float lim[]{0.0f, 0.05f, 0.299f, 0.3f, 0.301f, 0.5f, -0.2f, 1.0f};
            const double fs = double(lim[Rand(n, 163) % std::size(lim)]);
            speedsFor(logmag(164, 0.0005, 1.0), std::fabs(fs));
            if (fs < 0) for (int i = 0; i < 3; ++i) c.cs[i] = B(float(-double(F(c.fwd[i])) * 0.2 + double(F(c.right[i])) * 0.05));
        }
        if (Rand(n, 165) % 3 == 0) { brakeOff(); thrustOn(); }
        break;
    case 7:   // contactSpeed y vectores de rueda con desnormales (1e-40..1e-30) y casi 0
        {
            const u32 w = Rand(n, 166) % 6;
            auto dn = [&](u32 k) { return DecadeFloat(n, k, -40.0, -30.0); };
            if (w == 0 || w == 3) for (int i = 0; i < 3; ++i) c.cs[i] = dn(170 + 2 * i);
            if (w == 1 || w == 3) for (int i = 0; i < 3; ++i) { c.fwd[i] = dn(180 + 2 * i); if (Rand(n, 177) % 2) c.right[i] = dn(190 + 2 * i); }
            if (w == 2 || w == 4) { for (int i = 0; i < 3; ++i) c.right[i] = dn(200 + 2 * i); if (Rand(n, 178) % 2) for (int i = 0; i < 3; ++i) c.cs[i] = dn(210 + 2 * i); }
            if (w == 5) { speedsFor(logmag(220, 1e-38, 1e-30), logmag(221, 1e-38, 1e-30)); for (int i = 0; i < 3; ++i) if (Rand(n, 222 + i) % 3 == 0) c.fwd[i] = dn(230 + i); }
            if (Rand(n, 226) % 3 == 0) { brakeOff(); thrustOn(); } else if (Rand(n, 227) % 3 == 0) { brakeOff(); c.thrust = B(0.0f); }
            if (Rand(n, 228) % 4 == 0) c.status = 1u;
            if (Rand(n, 229) % 6 == 0) c.adh = dn(240);
            if (Rand(n, 238) % 5 == 0) c.sadm = dn(239);
        }
        break;
    case 8:   // circulo de agarre superado: estado SPINNING / SKIDDING, wheelId 0/2/otros, estado del vehiculo, GasPedal, estatica "ya patinaba"
        c.adh = B(float(1 + Rand(n, 245) % 500) / 1000.0f);
        speedsFor(logmag(246, 0.01, 3.0), logmag(247, 0.01, 3.0));
        if (Rand(n, 248) % 2) { brakeOff(); thrustOn(); } else if (Rand(n, 249) % 2) { brakeOff(); c.thrust = B(0.0f); }
        c.wheelId = Rand(n, 250) % 3 == 0 ? Rand(n, 251) : Rand(n, 252) % 4;
        statusPick(253);
        c.gas = Rand(n, 256) % 4 == 0 ? B(0.0f) : B(float(int(Rand(n, 257) % 2001) - 1000) / 1000.0f);
        c.tl = Rand(n, 258) % 4 == 0 ? B(1.0f) : B(float(Rand(n, 259) % 1500) / 1000.0f);
        if (Rand(n, 260) % 3 == 0) c.af = 1u;
        {
            static constexpr float t[]{0.15f, 0.149f, 0.151f, 0.1f, 0.4f};
            if (Rand(n, 261) % 4 == 0) speedsFor(logmag(262, 0.01, 3.0), double(t[Rand(n, 263) % std::size(t)]));
        }
        break;
    case 9:   // ruedas en el suelo 0..4 (y valores raros)
        {
            static constexpr std::array<u32, 14> w{0u, 1u, 2u, 3u, 4u, 0u, 1u, 2u, 3u, 4u, 0xFFFFFFFFu, 0x80000000u, 0x01000001u, 8u};
            c.wheels = w[Rand(n, 264) % w.size()];
            speedsFor(logmag(265, 0.001, 2.0), logmag(266, 0.001, 2.0));
            if (Rand(n, 267) % 2) { brakeOff(); if (Rand(n, 268) % 2) thrustOn(); else c.thrust = B(0.0f); }
            if (Rand(n, 269) % 5 == 0) c.status = 1u;
        }
        break;
    case 10:   // estado de rueda de entrada NORMAL/SPINNING/SKIDDING/FIXED y estatica "ya patinaba" en sus dos valores
        c.wstate = Rand(n, 270) % 4;
        c.af = Rand(n, 271) % 2;
        c.tl = Rand(n, 272) % 4 == 0 ? B(1.0f) : B(float(Rand(n, 273) % 1500) / 1000.0f);
        statusPick(274);
        c.gas = Rand(n, 277) % 4 == 0 ? B(0.0f) : B(float(int(Rand(n, 278) % 2001) - 1000) / 1000.0f);
        speedsFor(logmag(279, 0.005, 2.0), logmag(280, 0.005, 2.0));
        if (Rand(n, 281) % 2) { brakeOff(); thrustOn(); }
        break;
    case 11:   // las tres estaticas (AC/AD/AE) en sus dos valores y combinaciones freno / empuje / ninguno
        c.af = Rand(n, 282) % 2; c.b1 = Rand(n, 283) % 2; c.b2 = Rand(n, 284) % 2;
        switch (Rand(n, 285) % 3) { case 0: brakeOff(); thrustOn(); break; case 1: c.brake = B(float(logmag(286, 0.01, 2.0))); break; default: brakeOff(); c.thrust = B(0.0f); break; }
        speedsFor(logmag(287, 0.001, 2.0), logmag(288, 0.001, 2.0));
        statusPick(289);
        c.wstate = Rand(n, 292) % 4 == 0 ? 1u + Rand(n, 293) % 3 : 0u;
        break;
    case 12:   // multiplicador antihundimiento (>0, 0, <0, NaN, inf, minusculo) con freno, empuje y sin ninguno: fuerza de giro separada
        {
            static constexpr std::array<u32, 10> sd{0x3F800000u, 0x3DCCCCCDu, 0x40000000u, 0u, 0x80000000u, 0xBF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800000u, 0x00000001u};
            c.sadm = Rand(n, 294) % 3 == 0 ? sd[Rand(n, 295) % sd.size()] : B(float(1 + Rand(n, 296) % 3000) / 1000.0f);
            switch (Rand(n, 297) % 3) { case 0: brakeOff(); thrustOn(); break; case 1: c.brake = B(float(logmag(298, 0.01, 2.0))); if (Rand(n, 299) % 2) c.adh = B(F(c.brake) * 2.0f); break; default: brakeOff(); c.thrust = B(0.0f); break; }
            speedsFor(logmag(300, 0.001, 2.0), logmag(301, 0.001, 2.0));
        }
        break;
    case 13:   // modulo de la fuerza total o de la de giro nulo, minusculo, infinito o NaN: ramas de normalizacion
        {
            const u32 w = Rand(n, 302) % 6;
            if (w == 0) { brakeOff(); thrustOn(); c.thrust = B(Rand(n, 303) % 2 ? 1e-30f : -1e-30f); for (int i = 0; i < 3; ++i) c.cs[i] = 0u; }
            else if (w == 1) { brakeOff(); thrustOn(); c.thrust = Pick(kTiny, n, 304); if (F(c.thrust) == 0.0f) c.thrust = 1u; for (int i = 0; i < 3; ++i) c.cs[i] = 0u; }
            else if (w == 2) { brakeOff(); thrustOn(); c.thrust = Pick(kBig, n, 305); for (int i = 0; i < 3; ++i) c.cs[i] = 0u; }
            else if (w == 3) { speedsFor(logmag(306, 0.001, 2.0), logmag(307, 0.001, 2.0)); c.fwd[Rand(n, 308) % 3] = Pick(kNan, n, 309); }
            else if (w == 4) { speedsFor(logmag(310, 0.001, 2.0), logmag(311, 0.001, 2.0)); c.right[Rand(n, 312) % 3] = Pick(kInf, n, 313); }
            else { brakeOff(); thrustOn(); speedsFor(logmag(314, 0.001, 2.0), logmag(315, 0.001, 2.0)); c.fwd[Rand(n, 316) % 3] = Pick(kBig, n, 317); }
            if (Rand(n, 318) % 3 == 0) c.sadm = B(float(1 + Rand(n, 319) % 2000) / 1000.0f);
            if (Rand(n, 320) % 3 == 0) c.wheels = 1u + Rand(n, 321) % 4;
        }
        break;
    case 14:   // aleatorio general
        speedsFor(logmag(322, 0.0005, 3.0), logmag(323, 0.0005, 3.0));
        if (Rand(n, 324) % 4 == 0) { brakeOff(); c.thrust = B(0.0f); }
        statusPick(325);
        break;
    case 15:   // aleatorio con valores de pila y estados fuera de rango
        {
            static constexpr u32 sv[]{0u, 1u, 2u, 3u, 4u, 5u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x12345678u, 0xFFFFFFFEu, 0x100u};
            c.wstate = sv[Rand(n, 328) % std::size(sv)];
            c.wheelId = Rand(n, 329) % 3 == 0 ? Pick(kStackSeeds, n, 330) : c.wheelId;
            c.vtype = Rand(n, 331); c.subtype = Rand(n, 332) % 2 ? Rand(n, 333) : c.subtype;
            c.af = Rand(n, 334) % 2; c.vstat = Rand(n, 335) & 0xFFu;
            c.status = Rand(n, 336) % 3 == 0 ? Rand(n, 337) : c.status;
            speedsFor(logmag(338, 0.0005, 2.0), logmag(339, 0.0005, 2.0));
        }
        break;
    case 16:   // NaN e infinitos repartidos
        {
            u32* t[] = {&c.thrust, &c.brake, &c.adh, &c.gas, &c.ts, &c.fric, &c.mass, &c.hmass, &c.tl, &c.turn, &c.sadm, &c.cs[0], &c.cs[1], &c.cs[2], &c.fwd[0], &c.fwd[2], &c.right[0], &c.right[1], &c.cp[0], &c.cp[1], &c.cp[2]};
            speedsFor(logmag(340, 0.001, 1.5), logmag(341, 0.001, 1.5));
            for (int k = 0; k < 21; ++k) if (Rand(n, 350 + k) % 8 == 0) *t[k] = Rand(n, 380 + k) % 2 ? Pick(kNan, n, 410 + k) : Pick(kInf, n, 440 + k);
            if (Rand(n, 470) % 2) { c.brake = Rand(n, 471) % 2 ? Pick(kNanBoth, n, 472) : c.brake; }
            if (Rand(n, 473) % 4 == 0) c.thrust = Pick(kNanBoth, n, 474);
        }
        break;
    case 18: case 19:   // umbrales con vecinos (+-3 ulps): ejes alineados (fs = cs.x, rs = cs.y exactos), una rueda, paso de tiempo 1: cada comparacion de la funcion queda en su borde
        {
            c.ts = B(1.0f);
            for (int i = 0; i < 3; ++i) { c.fwd[i] = B(i == 0 ? 1.0f : 0.0f); c.right[i] = B(i == 1 ? 1.0f : 0.0f); c.cs[i] = 0u; }
            c.wheels = 1u; c.wstate = 0u; c.af = 0u; c.status = 0u; c.sadm = Rand(n, 602) % 3 == 0 ? B(0.0f) : B(float(1 + Rand(n, 603) % 2000) / 1000.0f);
            c.gas = B(Rand(n, 604) % 2 ? 0.0f : 0.5f); c.vtype = 0u; c.subtype = 0u; c.model = 0u; c.tl = B(float(500 + Rand(n, 605) % 1000) / 1000.0f);
            const int k = int(Rand(n, 606) % 7) - 3;
            const float sg = Rand(n, 607) % 2 ? 1.0f : -1.0f;
            const float adhE = 0.02f + float(Rand(n, 608) % 1000) / 1000.0f;   // agarre efectivo (paso de tiempo 1)
            c.adh = B(adhE);
            auto setFs = [&](float fs) { c.cs[0] = B(fs); };
            auto setRs = [&](float rs) { c.cs[1] = B(rs); };
            if (f == 18) {
                switch (Rand(n, 609) % 6) {
                case 0:   // rueda fija: |fs| junto a 0.005 con el freno por encima del agarre
                    c.brake = B(adhE * 4.0f + 0.5f); c.thrust = 0u; setFs(sg * F(Ulps(B(0.005f), k))); setRs(sg * float(Rand(n, 610) % 100) / 1000.0f);
                    break;
                case 1:   // freno pasivo solo con |gas| < 0.01
                    c.brake = 0u; c.thrust = 0u; c.gas = Ulps(B(0.01f), k) | (Rand(n, 611) % 2 ? 0x80000000u : 0u); c.adh = B(1.0f); setFs(sg * (0.01f + float(Rand(n, 612) % 1000) / 1000.0f));
                    break;
                case 2:   // masa 500 del coche con el acelerador a cero
                    c.brake = 0u; c.thrust = 0u; c.gas = 0u; c.hmass = Ulps(B(500.0f), k); c.adh = B(1.0f); setFs(sg * (0.05f + float(Rand(n, 613) % 1000) / 1000.0f));
                    if (Rand(n, 614) % 3 == 0) c.model = 0x1B9u;
                    break;
                case 3:   // fs junto a 0.15 con empuje, circulo superado y rueda 0 o 2 (limite de traccion doble)
                    c.brake = 0u; c.thrust = B(sg * (0.3f + float(Rand(n, 615) % 500) / 1000.0f) * adhE); c.wheelId = Rand(n, 616) % 2 ? 2u : 0u; c.adh = B(adhE);
                    setFs(sg * F(Ulps(B(0.15f), k))); setRs(-0.95f * adhE);
                    break;
                case 4:   // limite de traccion * agarre contra |empuje| (SPINNING / SKIDDING)
                    c.brake = 0u; c.wheelId = 1u; c.thrust = Ulps(B(0.3f * adhE), k) | (sg < 0 ? 0x80000000u : 0u); setFs(0.05f); setRs(-0.99f * adhE);
                    break;
                default:  // rueda pinchada con fs junto al limite de 0.3
                    c.status = 1u; setFs(sg * F(Ulps(B(0.3f), k))); setRs(sg * (0.01f + float(Rand(n, 617) % 500) / 1000.0f));
                    if (Rand(n, 618) % 2) { c.brake = 0u; c.thrust = B(0.5f); }
                    break;
                }
            } else {
                switch (Rand(n, 619) % 4) {
                case 0:   // freno contra agarre (b <= adh recorta, b > adh deja la rueda fija)
                    c.brake = Ulps(B(adhE), k); c.thrust = 0u; setFs(sg * (0.01f + float(Rand(n, 620) % 2000) / 1000.0f)); setRs(float(Rand(n, 621) % 100) / 1000.0f);
                    break;
                case 1:   // lateral contra agarre con empuje (recorte a +-agarre)
                    c.brake = 0u; c.thrust = B(sg * 0.01f); setRs(-sg * F(Ulps(B(adhE), k))); setFs(float(Rand(n, 622) % 100) / 1000.0f);
                    break;
                case 2:   // fuerza hacia delante contra el freno (acotacion a +-freno)
                    {
                        const float b = 0.02f + float(Rand(n, 623) % 500) / 1000.0f;
                        c.brake = B(b); c.adh = B(1.0f); c.thrust = 0u;
                        setFs(-sg * F(Ulps(B(b), k)));    // fuerza = -fs / 1
                    }
                    break;
                default:  // energia contra agarre^2 (3-4-5)
                    c.brake = 0u; c.thrust = B(sg * 0.6f * adhE); setRs(-F(Ulps(B(0.8f * adhE), k))); setFs(float(Rand(n, 624) % 100) / 1000.0f);
                    break;
                }
            }
            if (Rand(n, 625) % 4 == 0) c.vstat = StatusByte(n, 626, 0u);
        }
        break;
    default:   // 17: desnormales y bordes repartidos
        {
            u32* t[] = {&c.thrust, &c.brake, &c.adh, &c.gas, &c.cs[0], &c.cs[1], &c.cs[2], &c.fwd[0], &c.fwd[1], &c.right[0], &c.right[2], &c.cp[0], &c.sadm, &c.ts, &c.hmass};
            speedsFor(logmag(480, 0.001, 1.0), logmag(481, 0.001, 1.0));
            for (int k = 0; k < 15; ++k) {
                const u32 w = Rand(n, 490 + k) % 8;
                if (w == 0) *t[k] = Pick(kTiny, n, 520 + k); else if (w == 1) *t[k] = (Rand(n, 550 + k) & 0x807FFFFFu) | 1u; else if (w == 2) *t[k] = DecadeFloat(n, 580 + k, -40.0, -30.0);
            }
        }
        break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.A("fwd", c.fwd, 3).A("right", c.right, 3).A("cs", c.cs, 3).A("cp", c.cp, 3);
    j.U("wheels", c.wheels).U("thrust", c.thrust).U("brake", c.brake).U("adh", c.adh).U("wheel_id", c.wheelId).U("wstate", c.wstate).U("status", c.status);
    j.U("gas", c.gas).U("vtype", c.vtype).U("subtype", c.subtype).U("model", c.model).U("mass", c.mass).U("turn", c.turn).U("hmass", c.hmass).U("tl", c.tl).U("sadm", c.sadm).U("vstat", c.vstat);
    j.U("ts", c.ts).U("fric", c.fric).U("af", c.af).U("b1", c.b1).U("b2", c.b2).U("rand_seed", c.randSeed);
    return j.End();
}
}

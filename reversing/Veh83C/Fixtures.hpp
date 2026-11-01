#pragma once
// Fixtures del banco Veh83C (lote 83C): CVehicle::AddSingleWheelParticles (EXE 0x6DE880), 18432 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CVehicle::AddSingleWheelParticles  0x6DE880  thiscall, ECX = this, 11 dwords (wheelState, wheelStatus, a2, a3, colPoint*, pos*, a6, idx, skidType, bloodState*, flags), `ret 0x2C`, devuelve false
//                                                  (hook CVehicle / AddSingleWheelParticles, tipo S)
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

namespace V83CFix {
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
constexpr std::size_t kVehWin = 0x990, kMatWin = 0x48, kCamWin = 0x40, kSysWin = 0x20, kColWin = 0x30, kVecWin = 0x10, kByteWin = 0x10, kPlWin = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kMatArena = Arena(kMatWin), kCamArena = Arena(kCamWin), kSysArena = Arena(kSysWin), kColArena = Arena(kColWin),
                      kVecArena = Arena(kVecWin), kByteArena = Arena(kByteWin), kPlArena = Arena(kPlWin);

constexpr u32 kCamPlacement = 0xB6F02C, kCamMatrixPtr = 0xB6F03C, kCamBase = 0xB6F028, kCamBytes = 0x20;   // TheCamera: m_placement (+4) y m_matrix (+0x14)
constexpr u32 kFxBase = 0xA9AE00, kFxBytes = 0x48;                                                         // g_fx: punteros a sistemas (+0 .. +0x44)
constexpr u32 kSlotSmoke = 0xA9AE24;                                                                       // g_fx.m_SmokeII3expand
constexpr u32 kFrameCounter = 0xB7CB4C, kTimeStep = 0xB7CB5C;                                              // CTimer::m_FrameCounter y CTimer::ms_fTimeStep

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 83093u); }
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
inline constexpr std::array<u32, 14> kUnit{0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x40000000u, 0xC0000000u, 0x3F7FFFFFu, 0xBF7FFFFFu, 0x3F800001u, 0xBF800001u, 0x3DCCCCCDu, 0x3F99999Au, 0x3FA66666u, 0x41200000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
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
constexpr std::array<u32, 12> kMatOff{0x00, 0x04, 0x08, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28, 0x30, 0x34, 0x38};

struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 ws{}, wstatus{}, a2{}, a3{}, a6{}, idx{}, skid{}, flags{}, blood{}, surfA{}, surfB{};
    u32 pos[3]{};
    u32 hasMat{}, cam[3]{}, camOther[3]{};
    u32 mat[12]{};
    u32 vel[3]{};
    u32 gas{}, bright{}, model{}, subtype{}, offscreen{}, touchWater{}, alwaysSkid{};
    u32 pvKind{}, ppKind{}, isWater{}, friction{}, waterRet{}, waterLevel{}, dirtRet{}, width{};
    u32 frame{}, timestep{}, randSeed{}, swapSlot{};
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
inline u32 RetBool(u32 n, u32 k, bool v) {   // bool devuelto en AL; el resto de EAX es basura
    return (Rand(n, k) % 3 == 0 ? (Rand(n, k + 1) & 0xFFFFFF00u) : 0u) | (v ? (1u + (Rand(n, k + 2) % 3 == 0 ? Rand(n, k + 3) % 255u : 0u)) : 0u);
}

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    auto fl = [&](u32 k, u32 mode = 0u) { return FloatOf(n, k, mode); };
    // ---- base: caso plausible
    c.hasMat = Rand(n, 3) % 5 < 3 ? 0u : 1u;
    c.ws = Rand(n, 4) % 4;
    c.wstatus = Rand(n, 5) % 6 == 0 ? 1u : (Rand(n, 6) % 8 == 0 ? Rand(n, 8) % 4u : 0u);
    c.a2 = Rand(n, 9) % 12 == 0 ? RandFloat(n, 10, 0, 3) & 0x7FFFFFFFu : B(float(Rand(n, 11) % 1000) / 1000.0f);   // casi siempre < 1
    {
        static constexpr std::array<float, 16> speeds{0.0f, 0.02f, 0.03f, 0.031f, 0.1f, 0.101f, 0.15f, 0.2f, 0.5f, 0.99f, 1.0f, 1.5f, 3.0f, 10.0f, 0.04f, 0.05f};
        c.a3 = Rand(n, 12) % 3 == 0 ? B(speeds[Rand(n, 13) % speeds.size()]) : B(float(Rand(n, 14) % 4000) / 1000.0f);
    }
    c.a6 = Rand(n, 15);
    c.idx = Rand(n, 16) % 10 == 0 ? Rand(n, 17) % 8u : Rand(n, 18) % 4u;
    c.skid = Rand(n, 19) % 4 == 0 ? Rand(n, 20) : Rand(n, 21) % 4u;
    c.flags = Rand(n, 22) % 6 == 0 ? Rand(n, 23) : Rand(n, 24) % 8u;
    c.blood = Rand(n, 25) % 3 == 0 ? (Rand(n, 26) % 2 ? 1u : (Rand(n, 27) & 0xFFu)) : 0u;
    c.surfA = Rand(n, 28) & 0xFFu; c.surfB = Rand(n, 29) & 0xFFu;
    c.gas = Rand(n, 30) % 8 == 0 ? fl(31, 4u) : B(float(int(Rand(n, 32) % 2001) - 1000) / 1000.0f);
    c.bright = Rand(n, 33) % 3 == 0 ? B(1.2f) : fl(34, 4u);
    c.model = Rand(n, 35) % 5 == 0 ? 0x1D7u : Rand(n, 36) & 0xFFFFu;
    {
        static constexpr std::array<u32, 10> sts{0u, 1u, 2u, 3u, 4u, 9u, 10u, 11u, 9u, 10u};
        c.subtype = Rand(n, 37) % 8 == 0 ? Rand(n, 38) : sts[Rand(n, 39) % sts.size()];
    }
    c.offscreen = Rand(n, 40) % 20 == 0; c.touchWater = Rand(n, 41) % 4 == 0; c.alwaysSkid = Rand(n, 42) % 4 == 0;
    c.pvKind = Rand(n, 43) % 3 == 0 ? 0u : 1u;
    c.ppKind = Rand(n, 44) % 4 == 0 ? 0u : 1u;
    c.isWater = RetBool(n, 45, Rand(n, 48) % 8 == 0);
    c.friction = Rand(n, 49) % 3 == 0 ? 1u : (Rand(n, 50) % 6 == 0 ? Rand(n, 51) : Rand(n, 52) % 3u);
    c.waterRet = RetBool(n, 53, Rand(n, 56) % 2 != 0);
    c.dirtRet = Rand(n, 57) % 3 == 0 ? 0u : 1u;
    c.width = Rand(n, 58) % 4 == 0 ? fl(59, 4u) : B(0.25f);
    c.frame = Rand(n, 60); c.timestep = B(0.5f + float(Rand(n, 61) % 3000) / 1000.0f);
    c.randSeed = Rand(n, 62) % 12 == 0 ? Pick(kStackSeeds, n, 63) : Rand(n, 64);
    c.swapSlot = Rand(n, 65) % 3 == 0;
    for (int i = 0; i < 12; ++i) c.mat[i] = fl(70 + i, 4u);
    for (int i = 0; i < 3; ++i) c.vel[i] = Rand(n, 90 + i) % 4 == 0 ? B(float(int(Rand(n, 93 + i) % 400) - 200) / 1000.0f) : fl(96 + i, 4u);
    for (int i = 0; i < 3; ++i) c.camOther[i] = fl(100 + i, 0u);
    // ---- colocacion respecto de la camara: el vehiculo esta a `dist` metros (posicion de la matriz, +0x30)
    auto camNormal = [&](int hi) { for (int i = 0; i < 3; ++i) c.cam[i] = RandFloat(n, 110 + i, -4, hi); };
    auto place = [&](double dist, double jitter = 0.0) {
        camNormal(8);
        double d[3];
        Split3(n, 114, dist * (1.0 + jitter * (double(Rand(n, 117) % 2001) - 1000.0) * 1e-5), d);
        for (int i = 0; i < 3; ++i) c.mat[9 + i] = B(float(double(F(c.cam[i])) - d[i]));
        if (jitter != 0.0 && Rand(n, 118) % 2 == 0) { const u32 a = Rand(n, 119) % 3; c.mat[9 + a] = Ulps(c.mat[9 + a], int(Rand(n, 120) % 5) - 2); }
    };
    // la suma (byte bajo del modelo + contador) con los bits bajos a cero para que pase la compuerta de cadencia
    auto passGate = [&](u32 mask, u32 pct) {
        if (Rand(n, 121) % 100 < pct) c.frame = (Rand(n, 122) & ~mask) - (c.model & 0xFFu);
    };
    auto nearVeh = [&] { place(double(Rand(n, 123) % 7000) / 1000.0); passGate(3u, 70); };
    auto placePos = [&] {   // posicion de la rueda: cerca del vehiculo
        for (int i = 0; i < 3; ++i) c.pos[i] = B(float(double(F(c.mat[9 + i])) + (double(Rand(n, 124 + i) % 4001) - 2000.0) / 1000.0));
    };
    auto level = [&] {   // nivel del agua alrededor de la z de la rueda (bordes con ulps)
        const u32 z = c.pos[2];
        switch (Rand(n, 128) % 6) {
        case 0: c.waterLevel = z; break;
        case 1: c.waterLevel = Ulps(z, 1); break;
        case 2: c.waterLevel = Ulps(z, -1); break;
        case 3: c.waterLevel = fl(129, 0u); break;
        case 4: c.waterLevel = Pick(kNan, n, 130); break;
        default: c.waterLevel = B(F(z) + float(int(Rand(n, 131) % 2001) - 1000) / 500.0f); break;
        }
    };
    // el limite de particulas lo da el EAX de _ftol2(|v * paso| * escala): se acota para no iterar mil millones de veces
    auto speedFor = [&](double maxMag) {
        double d[3];
        const double mag = std::pow(10.0, -2.0 + double(Rand(n, 135) % 2001) / 2000.0 * (std::log10(maxMag) + 2.0));
        Split3(n, 136, mag, d);
        for (int i = 0; i < 3; ++i) c.vel[i] = B(float(d[i]));
    };
    nearVeh(); placePos(); level();
    switch (f) {
    case 0: c.ws = 0; break;
    case 1: c.ws = 1; c.wstatus = Rand(n, 140) % 5 == 0 ? 1u : 0u; break;
    case 2: c.ws = 2; break;
    case 3: c.ws = 3; break;
    case 4:   // chispas: rueda pinchada, friccion 1, velocidad y giro
        c.wstatus = 1; c.friction = 1u; c.ws = Rand(n, 141) % 2 ? 1u : Rand(n, 142) % 4;
        if (Rand(n, 143) % 3 == 0) c.a3 = B(float(Rand(n, 144) % 250) / 1000.0f);
        if (Rand(n, 145) % 6 == 0) c.gas = fl(146, 4u);
        break;
    case 5: {   // compuertas de distancia: alrededor de 625, 400 y 64 (cuadrados 25, 20 y 8) y vehiculo del jugador
        static constexpr double dists[]{25.0, 20.0, 8.0, 24.0, 19.0, 9.0, 26.0, 21.0, 7.0};
        place(dists[Rand(n, 147) % std::size(dists)], 1.0);
        placePos();
        passGate(Rand(n, 148) % 2 ? 3u : 1u, 70);
        if (Rand(n, 149) % 3 == 0) c.alwaysSkid = 1;
        if (Rand(n, 150) % 6 == 0) c.offscreen = 1;
        if (Rand(n, 151) % 5 == 0) place(double(Rand(n, 152) % 90) + 25.0);
        break;
    }
    case 6:   // a2: alrededor de 1.0
        switch (Rand(n, 153) % 6) {
        case 0: c.a2 = B(1.0f); break;
        case 1: c.a2 = Ulps(B(1.0f), int(Rand(n, 154) % 5) - 2); break;
        case 2: c.a2 = Pick(kNan, n, 155); break;
        case 3: c.a2 = fl(156, 3u); break;
        case 4: c.a2 = Pick(kInf, n, 157); break;
        default: c.a2 = fl(158, 0u); break;
        }
        break;
    case 7:   // agua: superficie, tocando agua, nivel
        c.touchWater = 1; c.isWater = RetBool(n, 160, Rand(n, 161) % 4 == 0); c.waterRet = RetBool(n, 162, Rand(n, 163) % 4 != 0);
        c.ws = Rand(n, 164) % 4;
        break;
    case 8: {   // umbrales de la velocidad: 0.03, 0.04, 0.1, 0.15, 0.2, 1.0 (con vecinos)
        static constexpr float v[]{0.03f, 0.04f, 0.1f, 0.15f, 0.2f, 1.0f};
        c.a3 = Ulps(B(v[Rand(n, 165) % std::size(v)]), int(Rand(n, 166) % 5) - 2);
        if (Rand(n, 167) % 2 == 0) {   // el modulo de la velocidad del vehiculo cerca de 0.04 / 0.15 (plano y 3D)
            double d[3];
            Split3(n, 168, (Rand(n, 171) % 2 ? 0.04 : 0.15) * (1.0 + (double(Rand(n, 172) % 2001) - 1000.0) * 1e-5), d);
            for (int i = 0; i < 3; ++i) c.vel[i] = B(float(d[i]));
            if (Rand(n, 173) % 2) c.vel[2] = B(0.0f);
        }
        c.alwaysSkid = Rand(n, 174) % 2; c.idx = Rand(n, 175) % 4;
        break;
    }
    case 9:   // quad (0x1D7) con ruedas delanteras y traseras
        c.model = 0x1D7u; c.idx = Rand(n, 176) % 6; c.wstatus = Rand(n, 177) % 8 == 0 ? 1u : 0u; c.touchWater = 0; c.isWater = 0u;
        if (Rand(n, 178) % 5 == 0) c.model = Rand(n, 179) % 2 ? 0x1D6u : 0x1D7u | 0x10000u;
        break;
    case 10:   // subtipos 9, 2 y 10 (tamano, vida) con particulas
        {
            static constexpr u32 sts[]{9u, 2u, 10u, 0u, 3u, 9u, 2u, 10u};
            c.subtype = sts[Rand(n, 180) % std::size(sts)];
            c.ws = Rand(n, 181) % 4; c.a3 = B(0.2f + float(Rand(n, 182) % 2000) / 1000.0f); c.dirtRet = 1;
        }
        break;
    case 11:   // trayectoria por omision (estado 0): sangre, barro, rueda delantera/trasera, bAlwaysSkidMarks
        c.ws = Rand(n, 183) % 6 == 0 ? 4u + Rand(n, 184) % 5 : 0u; c.alwaysSkid = Rand(n, 185) % 2; c.flags = Rand(n, 186) % 8u; c.blood = Rand(n, 187) % 3 == 0 ? 1u : 0u;
        c.idx = Rand(n, 188) % 4; c.wstatus = Rand(n, 189) % 10 == 0 ? 1u : 0u;
        break;
    case 12:   // flotantes especiales (NaN, infinitos, desnormales, enormes) en entradas y matriz
        {
            u32* t[] = {&c.a2, &c.a3, &c.gas, &c.bright, &c.vel[0], &c.vel[1], &c.vel[2], &c.pos[0], &c.pos[1], &c.pos[2], &c.width, &c.waterLevel, &c.mat[3], &c.mat[4], &c.mat[0], &c.timestep, &c.cam[0], &c.mat[9]};
            for (int k = 0; k < 18; ++k) if (Rand(n, 190 + k) % 5 == 0) *t[k] = fl(210 + k, 3u);
            if (Rand(n, 230) % 2) { c.a2 = B(0.5f); }
        }
        c.ws = Rand(n, 231) % 4; passGate(3u, 80);
        break;
    case 13:   // muchas particulas (hasta unos cientos): velocidades grandes y paso de tiempo largo
        c.ws = Rand(n, 232) % 2 ? 2u : 3u; c.a3 = B(0.04f + float(Rand(n, 233) % 3000) / 1000.0f); c.dirtRet = 1; c.flags &= ~4u;
        speedFor(Rand(n, 234) % 3 == 0 ? 300.0 : 60.0);
        c.timestep = Rand(n, 235) % 3 == 0 ? B(float(Rand(n, 236) % 6000) / 1000.0f) : c.timestep;
        passGate(3u, 90);
        break;
    case 14:   // aleatorio general (todas las ramas)
        c.ws = Rand(n, 237) % 5; passGate(3u, 60);
        if (Rand(n, 238) % 4 == 0) c.a3 = fl(239, 4u);
        break;
    case 15:   // aleatorio con valores de pila y estados fuera de rango
        {
            static constexpr u32 wsv[]{0u, 1u, 2u, 3u, 4u, 5u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x12345678u, 0xFFFFFFFEu, 0x100u};
            c.ws = wsv[Rand(n, 240) % std::size(wsv)];
            c.wstatus = Rand(n, 241) % 3 == 0 ? Rand(n, 242) : c.wstatus;
            c.idx = Rand(n, 243) % 3 == 0 ? Pick(kStackSeeds, n, 244) : c.idx;
            c.flags = Rand(n, 245); c.skid = Rand(n, 246);
            c.subtype = Rand(n, 247);
            passGate(3u, 60);
        }
        break;
    case 16:   // NaN e infinitos repartidos
        {
            u32* t[] = {&c.a2, &c.a3, &c.gas, &c.vel[0], &c.vel[1], &c.vel[2], &c.pos[0], &c.pos[1], &c.pos[2], &c.mat[3], &c.mat[4], &c.mat[0], &c.mat[9], &c.mat[10], &c.mat[11], &c.cam[0], &c.cam[1], &c.cam[2], &c.waterLevel, &c.timestep};
            for (int k = 0; k < 20; ++k) if (Rand(n, 250 + k) % 6 == 0) *t[k] = Rand(n, 270 + k) % 2 ? Pick(kNan, n, 290 + k) : Pick(kInf, n, 310 + k);
            c.ws = Rand(n, 330) % 4; passGate(3u, 70);
            if (Rand(n, 331) % 3) c.a2 = B(0.5f);
        }
        break;
    default:   // 17: desnormales y bordes
        {
            u32* t[] = {&c.a2, &c.a3, &c.gas, &c.vel[0], &c.vel[1], &c.vel[2], &c.pos[0], &c.pos[1], &c.pos[2], &c.mat[3], &c.mat[4], &c.mat[0], &c.timestep, &c.waterLevel};
            for (int k = 0; k < 14; ++k) {
                const u32 w = Rand(n, 340 + k) % 4;
                if (w == 0) *t[k] = Pick(kTiny, n, 360 + k); else if (w == 1) *t[k] = (Rand(n, 380 + k) & 0x807FFFFFu) | 1u;
            }
            c.ws = Rand(n, 400) % 4; passGate(3u, 70);
            if (Rand(n, 401) % 3) c.a2 = B(0.5f);
        }
        break;
    }
    // Acotacion de las particulas de rastro (estados 2 y 3) y de la rueda en giro: n = max(1, (int)(|v * paso| * escala)); se estima en double y se reduce si pasa de 3000
    {
        const double ts = double(F(c.timestep)), vx = double(F(c.vel[0])) * ts, vy = double(F(c.vel[1])) * ts, vz = double(F(c.vel[2])) * ts;
        const double est = std::sqrt(vz * vz + vy * vy + vx * vx) * 3.0;
        if (std::isfinite(est) && est >= 3000.0 && est < 9.0e18) {
            double e[3];
            Split3(n, 420, double(Rand(n, 423) % 900) / (std::fabs(ts) == 0.0 ? 1.0 : std::fabs(ts)), e);
            for (int i = 0; i < 3; ++i) c.vel[i] = B(float(e[i]));
        }
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("ws", c.ws).U("wstatus", c.wstatus).U("a2", c.a2).U("a3", c.a3).U("a6", c.a6).U("idx", c.idx).U("skid", c.skid).U("flags", c.flags).U("blood", c.blood).U("surf_a", c.surfA).U("surf_b", c.surfB);
    j.A("pos", c.pos, 3).U("has_mat", c.hasMat).A("cam", c.cam, 3).A("cam_other", c.camOther, 3).A("mat", c.mat, 12).A("vel", c.vel, 3);
    j.U("gas", c.gas).U("bright", c.bright).U("model", c.model).U("subtype", c.subtype).U("offscreen", c.offscreen).U("touch_water", c.touchWater).U("always_skid", c.alwaysSkid);
    j.U("pv_kind", c.pvKind).U("pp_kind", c.ppKind).U("is_water", c.isWater).U("friction", c.friction).U("water_ret", c.waterRet).U("water_level", c.waterLevel).U("dirt_ret", c.dirtRet).U("width", c.width);
    j.U("frame", c.frame).U("timestep", c.timestep).U("rand_seed", c.randSeed).U("swap_slot", c.swapSlot);
    return j.End();
}
}

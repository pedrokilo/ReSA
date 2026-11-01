#pragma once
// Fixtures del banco Veh83D (lote 83D): CVehicle::ProcessBikeWheel (EXE 0x6D73B0), 18432 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CVehicle::ProcessBikeWheel  0x6D73B0  thiscall, ECX = this, 14 dwords (wheelFwd*, wheelRight*, wheelContactSpeed*, wheelContactPoint*, wheelsOnGround, thrust, brake, adhesion (8o), destabTraction (9o),
//                                           wheelId (byte), wheelSpeed*, wheelState*, special, wheelStatus (word)), `ret 0x38`, void (hook CVehicle / ProcessBikeWheel, tipo S)
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

namespace V83DFix {
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
constexpr std::size_t kVehWin = 0x990, kMatWin = 0x48, kHandWin = 0x100, kArgWin = 0x40, kStateWin = 0x10, kSpeedWin = 0x10;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kMatArena = Arena(kMatWin), kHandArena = Arena(kHandWin), kArgArena = Arena(kArgWin), kStateArena = Arena(kStateWin), kSpeedArena = Arena(kSpeedWin);

constexpr u32 kTimeStep = 0xB7CB5C;                       // CTimer::ms_fTimeStep
constexpr u32 kWheelFriction = 0xC2B9CC;                  // gHandlingDataMgr.fWheelFriction
constexpr u32 kFlagsBase = 0xC1CDAC, kFlagsBytes = 8;     // estaticas de la rueda (0xC1CDAC..0xC1CDB3); la funcion usa 0xC1CDAF (ya patinaba), 0xC1CDB0 (marcha atras), 0xC1CDB1 (empuje) y 0xC1CDB2 (freno)
constexpr u32 kFlagAlready = 0xC1CDAF, kFlagReverse = 0xC1CDB0, kFlagDrive = 0xC1CDB1, kFlagBrake = 0xC1CDB2;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 83094u); }
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
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    // argumentos
    u32 fwd[3]{}, right[3]{}, cs[3]{}, cp[3]{};
    u32 wheels{}, thrust{}, brake{}, adh{}, destab{}, wheelId{}, wstate{}, special{}, status{}, a11{};
    // vehiculo
    u32 gas{}, vtype{}, subtype{}, model{}, mass{}, turn{}, hmass{}, tl{};
    u32 mr[3]{}, mf[3]{};        // matriz del vehiculo: derecha (+0) y adelante (+0x10)
    // globales
    u32 ts{}, fric{}, af{}, b0{}, b1{}, b2{}, randSeed{};
    // salidas de los colaboradores falseados
    u32 nrm[3]{}, crs[3]{};
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
    vec3(c.mr, 18, 1.0); vec3(c.mf, 22, 1.0);
    vec3(c.cs, 26, logmag(30, 0.001, 2.0));
    for (int i = 0; i < 3; ++i) c.cp[i] = B(float(double(Rand(n, 31 + i) % 4001) / 1000.0 - 2.0));
    c.wheels = Rand(n, 35) % 20 == 0 ? Rand(n, 36) % 9u : (Rand(n, 37) % 2 ? 2u : 1u + Rand(n, 38) % 4u);
    c.thrust = Rand(n, 39) % 3 == 0 ? B(0.0f) : B(float(int(Rand(n, 40) % 2001) - 1000) / 1000.0f);
    c.brake = Rand(n, 41) % 5 < 3 ? B(0.0f) : B(float(Rand(n, 42) % 2001) / 1000.0f);
    c.adh = B(float(Rand(n, 43) % 2001) / 1000.0f);
    c.destab = Rand(n, 44) % 3 == 0 ? B(1.0f) : B(float(Rand(n, 45) % 1501) / 1000.0f);
    c.wheelId = Rand(n, 46) % 5 == 0 ? Rand(n, 47) : Rand(n, 48) % 2;
    c.wstate = Rand(n, 49) % 4 == 0 ? Rand(n, 50) % 4 : 0u;
    c.special = Rand(n, 51) % 10 < 6 ? 0u : (Rand(n, 52) % 4 == 0 ? Rand(n, 53) % 8 : 2u + Rand(n, 54) % 2);
    c.status = Rand(n, 55) % 5 == 0 ? 1u : (Rand(n, 56) % 8 == 0 ? Rand(n, 57) : 0u);
    c.a11 = Rand(n, 58);
    c.gas = Rand(n, 59) % 2 == 0 ? B(float(int(Rand(n, 60) % 41) - 20) / 4000.0f) : B(float(int(Rand(n, 61) % 2001) - 1000) / 1000.0f);
    {
        static constexpr std::array<u32, 10> vt{9u, 9u, 9u, 10u, 0u, 2u, 9u, 11u, 3u, 10u};
        c.vtype = Rand(n, 62) % 10 == 0 ? Rand(n, 63) : vt[Rand(n, 64) % vt.size()];
        c.subtype = Rand(n, 65) % 10 == 0 ? Rand(n, 66) : vt[Rand(n, 67) % vt.size()];
    }
    c.model = Rand(n, 68) % 8 == 0 ? 0x1B9u : Rand(n, 69) & 0xFFFFu;
    c.mass = B(float(20 + Rand(n, 70) % 2000));
    c.turn = B(float(10 + Rand(n, 71) % 3000));
    c.hmass = Rand(n, 72) % 3 == 0 ? B(float(20 + Rand(n, 73) % 1000)) : c.mass;
    c.tl = Rand(n, 74) % 8 == 0 ? B(1.0f) : B(float(Rand(n, 75) % 1501) / 1000.0f);
    c.ts = B(0.5f + float(Rand(n, 76) % 3000) / 1000.0f);
    c.fric = B(0.01f + float(Rand(n, 77) % 3000) / 1000.0f);
    c.af = Rand(n, 78) % 3 == 0; c.b0 = Rand(n, 79) % 2; c.b1 = Rand(n, 80) % 2; c.b2 = Rand(n, 81) % 2;
    c.randSeed = Rand(n, 82) % 12 == 0 ? Pick(kStackSeeds, n, 83) : Rand(n, 84);
    if (Rand(n, 85) % 12 == 0) {
        for (int i = 0; i < 3; ++i) c.nrm[i] = fl(86 + i, 3u);
    } else {
        vec3(c.nrm, 90, 1.0);
    }
    if (Rand(n, 94) % 12 == 0) {
        for (int i = 0; i < 3; ++i) c.crs[i] = fl(95 + i, 3u);
    } else {
        vec3(c.crs, 98, Rand(n, 101) % 6 == 0 ? 0.0 : logmag(102, 0.01, 4.0));
    }
    // ---- por familia
    auto brakeOff = [&] { c.brake = B(0.0f); };
    auto thrustOn = [&] { if (F(c.thrust) == 0.0f) c.thrust = B(float(1 + Rand(n, 110) % 1000) / 1000.0f * (Rand(n, 111) % 2 ? 1.0f : -1.0f)); };
    auto speedsFor = [&](double rsMag, double fsMag) {   // contacto: fuerza hacia delante ~fsMag y lateral ~rsMag (aprox.: cs = fwd * fs + right * rs)
        double fvec[3], rvec[3];
        for (int i = 0; i < 3; ++i) { fvec[i] = double(F(c.fwd[i])); rvec[i] = double(F(c.right[i])); }
        const double fs = (Rand(n, 112) % 2 ? 1.0 : -1.0) * fsMag, rs = (Rand(n, 113) % 2 ? 1.0 : -1.0) * rsMag;
        for (int i = 0; i < 3; ++i) c.cs[i] = B(float(fvec[i] * fs + rvec[i] * rs));
    };
    switch (f) {
    case 0:   // marcha con empuje, sin freno: lateral limitado a +-adhesion
        brakeOff(); thrustOn(); c.special = Rand(n, 120) % 6 == 0 ? Rand(n, 121) % 4 : 0u; c.status = 0;
        speedsFor(logmag(122, 0.001, 2.0), logmag(123, 0.001, 2.0));
        break;
    case 1:   // freno aplicado: la fuerza hacia delante se frena hasta el freno, y rueda fija
        c.brake = B(float(logmag(124, 0.01, 3.0)));
        if (Rand(n, 125) % 3 == 0) c.adh = B(F(c.brake) * (0.5f + float(Rand(n, 126) % 1500) / 1000.0f));
        speedsFor(logmag(127, 0.001, 1.0), logmag(128, 0.0005, 2.0));
        if (Rand(n, 129) % 5 == 0) c.status = 0;
        break;
    case 2:   // sin freno ni empuje: freno pasivo segun tipo (BMX, moto, otros, masa ligera, RC Bandit)
        brakeOff(); c.thrust = B(Rand(n, 130) % 2 ? 0.0f : -0.0f);
        c.gas = Rand(n, 131) % 6 == 0 ? fl(132, 4u) : B(float(int(Rand(n, 133) % 41) - 20) / 3000.0f);
        speedsFor(logmag(134, 0.0005, 1.0), logmag(135, 0.0003, 0.5));
        if (Rand(n, 136) % 3 == 0) c.cs[0] = c.cs[1] = c.cs[2] = 0u;
        c.subtype = (Rand(n, 137) % 3 == 0) ? 10u : (Rand(n, 138) % 3 == 0 ? 9u : c.subtype);
        c.vtype = Rand(n, 139) % 3 == 0 ? 9u : c.vtype;
        if (Rand(n, 140) % 4 == 0) c.hmass = B(float(300 + Rand(n, 141) % 400));
        if (Rand(n, 142) % 5 == 0) c.model = 0x1B9u;
        if (c.subtype == 10u && Rand(n, 143) % 2) c.adh = B(float(5 + Rand(n, 144) % 500) / 1000.0f);
        break;
    case 3:   // special 2 y 3 (sin velocidad lateral) y vecinos
        c.special = Rand(n, 145) % 4 == 0 ? Rand(n, 146) % 6 : 2u + Rand(n, 147) % 2;
        speedsFor(logmag(148, 0.001, 2.0), logmag(149, 0.001, 2.0));
        break;
    case 4:   // rueda pinchada: ruido aleatorio con rand() del EXE, tope a 0.12 sobre la velocidad hacia delante
        c.status = 1u; c.special = Rand(n, 150) % 8 == 0 ? 2u : 0u;
        {
            static constexpr float lim[]{0.0f, 0.05f, 0.119f, 0.12f, 0.121f, 0.5f, -0.2f, 1.0f};
            const double fs = double(lim[Rand(n, 151) % std::size(lim)]);
            speedsFor(logmag(152, 0.0005, 1.0), std::fabs(fs));
            if (fs < 0) for (int i = 0; i < 3; ++i) c.cs[i] = B(float(-double(F(c.fwd[i])) * 0.2 + double(F(c.right[i])) * 0.05));
        }
        break;
    case 5:   // circulo de agarre superado: velocidades grandes frente al agarre
        c.adh = B(float(1 + Rand(n, 153) % 500) / 1000.0f);
        speedsFor(logmag(154, 0.01, 3.0), logmag(155, 0.01, 3.0));
        if (Rand(n, 156) % 2) { c.brake = B(0.0f); thrustOn(); } else if (Rand(n, 157) % 2) { brakeOff(); c.thrust = B(0.0f); }
        break;
    case 6:   // dentro del circulo con destab < 1
        c.adh = B(float(300 + Rand(n, 158) % 3000) / 1000.0f);
        c.destab = B(float(Rand(n, 159) % 1000) / 1000.0f);
        speedsFor(logmag(160, 0.0005, 0.6), logmag(161, 0.0005, 0.6));
        if (Rand(n, 162) % 2) { brakeOff(); c.thrust = B(float(int(Rand(n, 163) % 201) - 100) / 1000.0f); }
        break;
    case 7:   // estado previo distinto de normal / bandera "ya patinaba": agarre por la perdida de traccion
        c.wstate = 1u + Rand(n, 164) % 3;
        c.af = Rand(n, 165) % 2;
        c.tl = Rand(n, 166) % 4 == 0 ? B(1.0f) : B(float(Rand(n, 167) % 1500) / 1000.0f);
        speedsFor(logmag(168, 0.005, 2.0), logmag(169, 0.005, 2.0));
        break;
    case 8:   // rueda 1 con freno o marcha atras (se salta la primera fuerza de giro) y numeros de rueda variados
        c.wheelId = Rand(n, 170) % 3 == 0 ? (1u | (Rand(n, 171) & 0xFFFFFF00u)) : 1u;
        if (Rand(n, 172) % 2) { c.brake = B(float(1 + Rand(n, 173) % 2000) / 1000.0f); }
        else { brakeOff(); c.thrust = B(-float(1 + Rand(n, 174) % 1000) / 1000.0f); }
        speedsFor(logmag(175, 0.001, 1.0), logmag(176, 0.001, 1.0));
        break;
    case 9:   // sin fuerza: velocidades nulas o diminutas
        {
            const u32 w = Rand(n, 177) % 4;
            if (w == 0) { c.cs[0] = c.cs[1] = c.cs[2] = 0u; }
            else if (w == 1) { for (int i = 0; i < 3; ++i) c.cs[i] = Pick(kTiny, n, 178 + i); }
            else if (w == 2) { speedsFor(0.0, logmag(181, 0.0001, 1.0)); }
            else { speedsFor(logmag(182, 0.0001, 1.0), 0.0); }
            if (Rand(n, 183) % 2) { brakeOff(); c.thrust = B(0.0f); }
            if (Rand(n, 184) % 4 == 0) c.fwd[0] = c.fwd[1] = c.fwd[2] = 0u;
            if (Rand(n, 185) % 4 == 0) c.right[0] = c.right[1] = c.right[2] = 0u;
        }
        break;
    case 10:   // umbrales con vecinos (0.12, 0.005, 0.1, 0.01, 0.05, 500, 1.0)
        {
            static constexpr float fsv[]{0.12f, 0.005f, 0.1f, 0.0f};
            const float t = fsv[Rand(n, 186) % std::size(fsv)];
            const u32 tb = Ulps(B(t), int(Rand(n, 187) % 5) - 2);
            speedsFor(logmag(188, 0.001, 0.5), double(F(tb)));
            switch (Rand(n, 189) % 6) {
            case 0: c.gas = Ulps(B(0.01f), int(Rand(n, 190) % 5) - 2) | (Rand(n, 191) % 2 ? 0x80000000u : 0u); brakeOff(); c.thrust = B(0.0f); break;
            case 1: c.destab = Ulps(B(1.0f), int(Rand(n, 192) % 5) - 2); break;
            case 2: c.hmass = Ulps(B(500.0f), int(Rand(n, 193) % 5) - 2); brakeOff(); c.thrust = B(0.0f); c.subtype = 0u; c.vtype = 0u; break;
            case 3: c.subtype = 10u; brakeOff(); c.thrust = B(0.0f); c.gas = B(0.0f); c.adh = B(float(5 + Rand(n, 194) % 500) / 1000.0f); break;
            case 4: c.special = Rand(n, 195) % 6; break;
            default: c.wheels = Pick(std::array<u32, 6>{0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0x01000001u}, n, 196); break;
            }
        }
        break;
    case 11:   // valores especiales (NaN, infinitos, desnormales, enormes) repartidos
        {
            u32* t[] = {&c.thrust, &c.brake, &c.adh, &c.destab, &c.gas, &c.ts, &c.fric, &c.mass, &c.hmass, &c.tl, &c.turn, &c.cs[0], &c.cs[1], &c.cs[2], &c.fwd[0], &c.right[1], &c.cp[0], &c.cp[2], &c.mr[0], &c.mf[1]};
            for (int k = 0; k < 20; ++k) if (Rand(n, 200 + k) % 6 == 0) *t[k] = fl(220 + k, 3u);
            speedsFor(logmag(240, 0.001, 1.0), logmag(241, 0.001, 1.0));
            if (Rand(n, 242) % 2) for (int k = 0; k < 20; ++k) if (Rand(n, 243 + k) % 9 == 0) *t[k] = fl(263 + k, 3u);
        }
        break;
    case 12:   // numero de ruedas en el suelo (fild exacto): 0, negativo, muy grande
        {
            static constexpr std::array<u32, 12> w{0u, 0xFFFFFFFFu, 3u, 8u, 0x7FFFFFFFu, 0x80000000u, 0x01000000u, 0x01000001u, 0xFF000000u, 100000u, 0x12345678u, 16777217u};
            c.wheels = w[Rand(n, 283) % w.size()];
            speedsFor(logmag(284, 0.001, 2.0), logmag(285, 0.001, 2.0));
        }
        break;
    case 13:   // bytes altos basura en wheelId y wheelStatus y valores raros en special/wheelState
        c.wheelId = (Rand(n, 286) & 0xFFFFFF00u) | (Rand(n, 287) % 3);
        c.status = (Rand(n, 288) & 0xFFFF0000u) | (Rand(n, 289) % 4 == 0 ? 1u : Rand(n, 290) % 3);
        c.special = Rand(n, 291) % 2 ? Rand(n, 292) : (Rand(n, 293) % 6);
        c.wstate = Rand(n, 294) % 3 == 0 ? Rand(n, 295) : c.wstate;
        speedsFor(logmag(296, 0.001, 1.5), logmag(297, 0.001, 1.5));
        break;
    case 14:   // aleatorio general
        speedsFor(logmag(298, 0.0005, 3.0), logmag(299, 0.0005, 3.0));
        if (Rand(n, 300) % 4 == 0) { brakeOff(); c.thrust = B(0.0f); }
        break;
    case 15:   // aleatorio con valores de pila y estados fuera de rango
        {
            static constexpr u32 sv[]{0u, 1u, 2u, 3u, 4u, 5u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x12345678u, 0xFFFFFFFEu, 0x100u};
            c.wstate = sv[Rand(n, 301) % std::size(sv)];
            c.special = sv[Rand(n, 302) % std::size(sv)];
            c.wheelId = Rand(n, 303) % 3 == 0 ? Pick(kStackSeeds, n, 304) : c.wheelId;
            c.vtype = Rand(n, 305); c.subtype = Rand(n, 306) % 2 ? Rand(n, 307) : c.subtype;
            c.af = Rand(n, 308) % 2;
            speedsFor(logmag(309, 0.0005, 2.0), logmag(310, 0.0005, 2.0));
        }
        break;
    case 16:   // NaN e infinitos repartidos
        {
            u32* t[] = {&c.thrust, &c.brake, &c.adh, &c.destab, &c.gas, &c.ts, &c.fric, &c.mass, &c.hmass, &c.tl, &c.turn, &c.cs[0], &c.cs[1], &c.cs[2], &c.fwd[0], &c.fwd[2], &c.right[0], &c.right[1], &c.cp[0], &c.cp[1], &c.cp[2], &c.mr[0], &c.mr[2], &c.mf[0], &c.mf[1], &c.nrm[0], &c.nrm[2], &c.crs[1], &c.crs[2]};
            speedsFor(logmag(320, 0.001, 1.5), logmag(321, 0.001, 1.5));
            for (int k = 0; k < 29; ++k) if (Rand(n, 330 + k) % 10 == 0) *t[k] = Rand(n, 360 + k) % 2 ? Pick(kNan, n, 390 + k) : Pick(kInf, n, 420 + k);
        }
        break;
    default:   // 17: desnormales y bordes
        {
            u32* t[] = {&c.thrust, &c.brake, &c.adh, &c.destab, &c.gas, &c.cs[0], &c.cs[1], &c.cs[2], &c.fwd[0], &c.fwd[1], &c.right[0], &c.right[2], &c.cp[0], &c.mr[1], &c.mf[2], &c.nrm[0], &c.nrm[1], &c.crs[0], &c.crs[2]};
            speedsFor(logmag(450, 0.001, 1.0), logmag(451, 0.001, 1.0));
            for (int k = 0; k < 19; ++k) {
                const u32 w = Rand(n, 460 + k) % 7;
                if (w == 0) *t[k] = Pick(kTiny, n, 490 + k); else if (w == 1) *t[k] = (Rand(n, 520 + k) & 0x807FFFFFu) | 1u;
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
    j.U("wheels", c.wheels).U("thrust", c.thrust).U("brake", c.brake).U("adh", c.adh).U("destab", c.destab).U("wheel_id", c.wheelId).U("wstate", c.wstate).U("special", c.special).U("status", c.status).U("a11", c.a11);
    j.U("gas", c.gas).U("vtype", c.vtype).U("subtype", c.subtype).U("model", c.model).U("mass", c.mass).U("turn", c.turn).U("hmass", c.hmass).U("tl", c.tl);
    j.A("mr", c.mr, 3).A("mf", c.mf, 3);
    j.U("ts", c.ts).U("fric", c.fric).U("af", c.af).U("b0", c.b0).U("b1", c.b1).U("b2", c.b2).U("rand_seed", c.randSeed);
    j.A("nrm", c.nrm, 3).A("crs", c.crs, 3);
    return j.End();
}
}

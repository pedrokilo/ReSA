#pragma once
// Fixtures del banco Veh80C (lote 80C): CVehicle::FireFixedMachineGuns (EXE 0x73DF00), 18432 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CVehicle::FireFixedMachineGuns  0x73DF00  thiscall, ECX = this (vehiculo), sin argumentos, `ret` simple, void   (hook CVehicle / FireFixedMachineGuns, tipo S)
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

namespace V80Fix {
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

// Arenas: guarda + ventana + guarda (la ventana es el objeto). La del vehiculo mide sizeof(CVehicle) = 0x5A0 (LiveComparison.hpp lo comprueba con static_assert).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x5A0, kMatWin = 0x48, kPedWin = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kMatArena = Arena(kMatWin), kPedArena = Arena(kPedWin);

constexpr u32 kTimeAddr = 0xB7CB84;   // CTimer::m_snTimeInMilliseconds

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 80080u); }
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
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ valores notables
// NaN: canonicos (quiet con signo +/-), NO canonicos (carga distinta, sNaN con y sin signo, carga maxima, carga minima) e infinitos
// Un unico patron de NaN (el indefinido de la x87, que producen inf-inf y 0*inf): con dos NaN distintos en una operacion la carga que sale depende del orden de operandos que elija el compilador
inline constexpr std::array<u32, 12> kNan{0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};   // +-0, desnormales y el menor normal
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
inline constexpr std::array<u32, 14> kUnit{0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x40000000u, 0xC0000000u, 0x3F7FFFFFu, 0xBF7FFFFFu, 0x3F800001u, 0xBF800001u, 0x3DCCCCCDu, 0x3F99999Au, 0x3FA66666u, 0x41200000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
// Float con exponente insesgado e (-126..127) y mantisa/signo aleatorios; Denorm: desnormal con signo y mantisa aleatorios (nunca cero)
inline u32 ExpFloat(u32 c, u32 k, int e) { const u32 r = Rand(c, k); return ((r >> 31) << 31) | (u32(e + 127) << 23) | (Rand(c, k + 500) & 0x7FFFFFu); }
inline u32 Denorm(u32 c, u32 k) { const u32 r = Rand(c, k); return ((r >> 31) << 31) | ((Rand(c, k + 500) & 0x7FFFFFu) | 1u); }
inline u32 Sgn(u32 bits, u32 neg) { return (bits & 0x7FFFFFFFu) | (neg << 31); }
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Un float: modo 0 moderado, 1 pequeno (productos que caen en el rango desnormal), 2 grande (desbordamiento), 3 especial (cero, desnormal, inf, NaN), 4 unitario, 5 mezcla de todo
inline u32 FloatOf(u32 c, u32 k, u32 mode) {
    const u32 r = Rand(c, k) % 32;
    switch (mode) {
    case 0: return r == 0 ? B(0.0f) : RandFloat(c, k + 2, -4, 11);
    case 1: return r < 3 ? Pick(kTiny, c, k + 1) : RandFloat(c, k + 2, -80, -20);
    case 2: return r < 3 ? Pick(kBig, c, k + 1) : RandFloat(c, k + 2, 20, 120);
    case 3:
        if (r < 8) return Pick(kTiny, c, k + 1);
        if (r < 13) return Pick(kInf, c, k + 1);
        if (r < 21) return Pick(kNan, c, k + 1);
        if (r < 24) return Pick(kBig, c, k + 1);
        return RandFloat(c, k + 2, -20, 100);
    case 4: return r < 4 ? RandFloat(c, k + 2, -2, 2) : Pick(kUnit, c, k + 1);
    default: {
        const u32 m = Rand(c, k + 3) % 5;
        return FloatOf(c, k + 5, m == 4 ? 3u : m);
    }
    }
}



// ------------------------------------------------------------------------------------------------ el caso
// Codigos de puntero (conductor): 0 nulo, 1 peaton A (arena), 2 peaton B (arena), 3 el propio vehiculo, 4..6 basura cruda (nunca se desreferencia)
constexpr u32 kGarbageBase = 0xA0000010u;
constexpr unsigned kEvents = 13;       // por canon: multiplicacion, 3 rand, DoTankDoomAiming, FireOneInstantHitRound; al final ReportWeaponEvent
constexpr u32 kNoMutation = 99;        // mutAt fuera de rango = sin mutacion
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 now{};             // CTimer::m_snTimeInMilliseconds
    u32 gun{};             // +0x4D4 (m_nGunFiringTime)
    u32 ammo{};            // +0x4AB (m_nAmmoInClip): solo el byte bajo
    u32 hasMat{};          // el vehiculo tiene matriz (si no, +0x14 = 0): solo valido cuando no se dispara
    u32 topX{}, topY{}, topZ{};   // matriz +0x10, +0x14 (y +0x18, que la funcion no lee)
    u32 driver{};          // codigo del conductor (+0x460)
    u32 res[2][3]{};       // resultados de CMatrix * CVector del primer y segundo canon
    u32 rnd[6]{};          // retornos de rand() (3 por canon); el EXE solo usa el byte bajo
    u32 doomMode{};        // bit 0: DoTankDoomAiming reescribe el extremo, bit 1: reescribe el inicio
    u32 dt[6]{};           // valores que escribe: inicio xyz y extremo xyz
    u32 mutAt{}, mutKind{}, mutVal{};   // evento tras el cual un falso muta el vehiculo (ver LiveComparison.hpp) y el valor
    u32 gate{};            // informativo
};

inline bool Fires(const C& c) { return c.now > c.gun + 150u; }   // jbe sin signo, con desbordamiento

inline u32 PtrCode(u32 n, u32 k) {
    const u32 r = Rand(n, k) % 10;
    return r < 3 ? 0u : r < 5 ? 1u : r < 7 ? 2u : r == 7 ? 3u : 4u + Rand(n, k + 1) % 3u;
}
// Un entero de rand(): el EXE usa solo el byte bajo (and 0xFF); lo demas es basura
inline u32 RndRet(u32 n, u32 k, int mode) {
    static constexpr std::array<u32, 8> edge{0u, 1u, 0x7Fu, 0x80u, 0x81u, 0xFEu, 0xFFu, 0x40u};
    const u32 hi = Rand(n, k + 2) % 3 == 0 ? 0u : (Rand(n, k + 3) & 0xFFFFFF00u);
    const u32 lo = mode == 1 ? Pick(edge, n, k) : (Rand(n, k + 1) & 0xFFu);
    return hi | lo;
}
// float(int * k) tal como lo calcula la x87 a 24 bits (producto exacto en double y un unico redondeo a float)
inline u32 Jit(int j, u32 kbits) { return B(float(double(j) * double(F(kbits)))); }
constexpr u32 kK15 = 0x3C75C28Fu, kK02 = 0x3CA3D70Au;   // 0.015f y 0.02f

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    static constexpr std::array<u32, 7> guns0{0u, 1u, 0xFFFFFF6Au, 0xFFFFFF6Bu, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u};
    c.gun = Rand(n, 91) % 3 == 0 ? Pick(guns0, n, 92) : Rand(n, 93) % 2 ? Rand(n, 94) % 100000u : Rand(n, 95);
    {
        const u32 lim = c.gun + 150u;   // uint32 con desbordamiento
        switch (Rand(n, 96) % 10) {
        case 0: c.now = lim; break;
        case 1: c.now = lim + 1u; break;
        case 2: c.now = lim - 1u; break;
        case 3: c.now = lim + 2u; break;
        case 4: c.now = lim + Rand(n, 97) % 5000u; break;
        case 5: c.now = lim - Rand(n, 97) % 5000u; break;
        case 6: c.now = lim + 1u + Rand(n, 97) % 100000u; break;
        case 7: c.now = lim + 1u + Rand(n, 97); break;
        default: c.now = Rand(n, 98); break;
        }
    }
    auto forceFire = [&] { if (!Fires(c)) c.now = c.gun + 151u + (Rand(n, 99) % 3 == 0 ? Rand(n, 100) % 100000u : 0u); };
    static constexpr std::array<u32, 8> ammo0{1u, 1u, 0u, 2u, 255u, 20u, 21u, 128u};
    c.ammo = Rand(n, 101) % 3 == 0 ? Pick(ammo0, n, 102) : (Rand(n, 103) & 0xFFu);
    c.hasMat = 1;
    c.topX = FloatOf(n, 110, Rand(n, 113) % 8 == 0 ? 5u : 4u);
    c.topY = FloatOf(n, 111, Rand(n, 114) % 8 == 0 ? 5u : 4u);
    c.topZ = FloatOf(n, 112, 0u);
    c.driver = PtrCode(n, 120);
    for (int g = 0; g < 2; ++g)
        for (int i = 0; i < 3; ++i) c.res[g][i] = FloatOf(n, 130 + 3 * g + i, Rand(n, 140 + 3 * g + i) % 8 == 0 ? 5u : 0u);
    for (int i = 0; i < 6; ++i) c.rnd[i] = RndRet(n, 150 + 6 * i, Rand(n, 190 + i) % 3 == 0 ? 1 : 0);
    c.doomMode = Rand(n, 200) % 4 == 0 ? 1u + Rand(n, 201) % 3u : 0u;
    for (int i = 0; i < 6; ++i) c.dt[i] = FloatOf(n, 210 + i, 0u);
    c.mutAt = Rand(n, 220) % 3 == 0 ? Rand(n, 221) % kEvents : kNoMutation;
    c.mutKind = c.mutAt == kNoMutation ? 0u : 1u + Rand(n, 222) % 5u;
    c.mutVal = Rand(n, 223) % 4 == 0 ? Pick(kStackSeeds, n, 224) : Rand(n, 225);
    // la mayoria de los casos dispara; la matriz nula solo tiene sentido si no se dispara (el EXE lee la matriz +0x10 sin comprobar nulo)
    if (f != 1 && f != 14 && f != 15) forceFire();
    auto topOf = [&](u32 mode) { c.topX = FloatOf(n, 230, mode); c.topY = FloatOf(n, 231, mode); };
    switch (f) {
    case 0: break;                       // moderado
    case 1:                              // sin disparo: ahora <= gun + 150 (incluido el borde y el desbordamiento)
        if (Fires(c)) c.now = c.gun + 150u - (Rand(n, 232) % 4 == 0 ? Rand(n, 233) % 5000u : 0u);
        c.hasMat = Rand(n, 234) % 2;
        break;
    case 2: {                            // borde del intervalo: -1, 0 (no dispara), +1, +2 (dispara)
        const u32 w = Rand(n, 235) % 5;
        c.now = c.gun + 150u + (w == 0 ? 0xFFFFFFFFu : w == 1 ? 0u : w == 2 ? 1u : w == 3 ? 2u : 1u);
        c.hasMat = Fires(c) ? 1u : Rand(n, 236) % 2;
        break;
    }
    case 3: {                            // la longitud horizontal alrededor del minimo 0.1: ceros, menor, igual, mayor
        static constexpr std::array<u32, 14> lenEdge{0x00000000u, 0x80000000u, 0x3DCCCCCDu, 0xBDCCCCCDu, 0x3DCCCCCCu, 0x3DCCCCCEu, 0x3D8F5C29u, 0x3D8F5C2Au, 0x3E4CCCCDu, 0x3B03126Fu, 0x3D6B2DC3u, 0xBD6B2DC3u, 0x3D6B2DC4u, 0x3D6B2DC2u};
        switch (Rand(n, 237) % 4) {
        case 0: c.topX = Pick(lenEdge, n, 238); c.topY = Pick(lenEdge, n, 239); break;
        case 1: c.topX = Pick(lenEdge, n, 238); c.topY = Pick(kTiny, n, 240); break;
        case 2: c.topX = Pick(kTiny, n, 238); c.topY = Pick(lenEdge, n, 239); break;
        default: c.topX = Sgn(RandFloat(n, 238, -6, -3), Rand(n, 241) % 2); c.topY = Sgn(RandFloat(n, 239, -6, -3), Rand(n, 242) % 2); break;
        }
        if (Rand(n, 246) % 5 == 0) { c.topX = Rand(n, 247) % 2 ? 0x80000000u : 0u; c.topY = Rand(n, 248) % 2 ? 0x80000000u : 0u; }   // vector horizontal nulo (con y sin signo)
        break;
    }
    case 4: topOf(3u); break;            // especiales: NaN, infinitos, ceros, desnormales, enormes
    case 5: topOf(Rand(n, 243) % 2 ? 1u : 2u); break;   // minimos y desbordamientos de los cuadrados
    case 6:                              // resultados de la multiplicacion especiales
        for (int g = 0; g < 2; ++g) for (int i = 0; i < 3; ++i) c.res[g][i] = FloatOf(n, 250 + 3 * g + i, 3u);
        break;
    case 7:                              // resultados enormes o diminutos: desbordamiento y cancelaciones
        for (int g = 0; g < 2; ++g) for (int i = 0; i < 3; ++i) c.res[g][i] = FloatOf(n, 250 + 3 * g + i, Rand(n, 260 + 3 * g + i) % 2 ? 1u : 2u);
        break;
    case 8: {                            // cancelaciones exactas: direccion (+-1, 0) o (0, +-1) da +-60; los resultados son su opuesto y el opuesto del ruido
        static constexpr std::array<u32, 4> dirs{0x3F800000u, 0xBF800000u, 0x3F800000u, 0xBF800000u};
        static constexpr std::array<u32, 2> zeros{0u, 0x80000000u};
        const u32 w = Rand(n, 244) % 4;
        c.topX = w < 2 ? dirs[w] : Pick(zeros, n, 245);
        c.topY = w >= 2 ? dirs[w] : Pick(zeros, n, 246);
        const u32 nxc = w == 0 ? 0x42700000u : w == 1 ? 0xC2700000u : 0u, nyc = w == 2 ? 0x42700000u : w == 3 ? 0xC2700000u : 0u;
        for (int g = 0; g < 2; ++g) {
            const int jx = int(c.rnd[3 * g + 1] & 0xFFu) - 0x80, jy = int(c.rnd[3 * g] & 0xFFu) - 0x80, jz = int(c.rnd[3 * g + 2] & 0xFFu) - 0x80;
            const u32 pick = Rand(n, 247 + g) % 6;
            c.res[g][0] = pick == 0 ? (nxc ^ 0x80000000u) : pick == 1 ? Jit(jx, kK15) ^ 0x80000000u : pick == 2 ? 0x80000000u : FloatOf(n, 250 + g, 0u);
            c.res[g][1] = pick == 0 ? (nyc ^ 0x80000000u) : pick == 1 ? Jit(jy, kK15) ^ 0x80000000u : pick == 3 ? 0x80000000u : FloatOf(n, 253 + g, 0u);
            c.res[g][2] = pick == 0 || pick == 1 ? (Jit(jz, kK02) ^ 0x80000000u) : pick == 4 ? 0x80000000u : pick == 5 ? 0u : FloatOf(n, 256 + g, 0u);
        }
        break;
    }
    case 9:                              // enteros de rand en los bordes y basura en los bits altos
        for (int i = 0; i < 6; ++i) c.rnd[i] = RndRet(n, 150 + 6 * i, 1);
        break;
    case 10:                             // cargador: 1 (se recarga), 0 (da la vuelta a 255), 2, 255; la hora se vuelve a leer al recargar
        c.ammo = Pick(ammo0, n, 262);
        if (Rand(n, 263) % 2) { c.mutAt = Rand(n, 264) % 2 ? kEvents - 1 : Rand(n, 265) % kEvents; c.mutKind = Rand(n, 266) % 2 ? 5u : (Rand(n, 267) % 2 ? 3u : 4u); }
        break;
    case 11:                             // barrido de mutaciones: cada evento x cada tipo (matriz, conductor, cargador, hora del arma, reloj)
        c.mutAt = (n / 16) % kEvents;
        c.mutKind = 1u + (n / 16 / kEvents) % 5u;
        c.driver = PtrCode(n, 268);
        break;
    case 12:                             // DoTankDoomAiming reescribe el inicio y/o el extremo con valores especiales
        c.doomMode = 1u + Rand(n, 269) % 3u;
        for (int i = 0; i < 6; ++i) c.dt[i] = FloatOf(n, 270 + i, Rand(n, 276 + i) % 3 == 0 ? 3u : 0u);
        break;
    case 13:                             // conductor y matriz alternativa
        c.driver = PtrCode(n, 277);
        if (Rand(n, 278) % 2) { c.mutKind = 1u + Rand(n, 279) % 2u; c.mutAt = Rand(n, 280) % kEvents; }
        break;
    case 14: case 15:                    // todo aleatorio, con y sin disparo
        if (Rand(n, 281) % 4 == 0) { topOf(5u); for (int g = 0; g < 2; ++g) for (int i = 0; i < 3; ++i) c.res[g][i] = FloatOf(n, 285 + 3 * g + i, 5u); }
        if (Rand(n, 282) % 2) forceFire();
        break;
    case 16:                             // NaN e infinitos en varios sitios a la vez (un solo patron de NaN)
        topOf(0u);
        for (int g = 0; g < 2; ++g) for (int i = 0; i < 3; ++i) if (Rand(n, 290 + 3 * g + i) % 3 == 0) c.res[g][i] = Rand(n, 296 + g) % 2 ? Pick(kNan, n, 298 + 3 * g + i) : Pick(kInf, n, 298 + 3 * g + i);
        if (Rand(n, 299) % 2) c.topX = Rand(n, 300) % 2 ? Pick(kNan, n, 301) : Pick(kInf, n, 301);
        if (Rand(n, 302) % 2) c.topY = Rand(n, 303) % 2 ? Pick(kNan, n, 304) : Pick(kInf, n, 304);
        break;
    default:                             // 17: ceros con signo, desnormales y sumas en el limite de la cancelacion
        for (int i = 0; i < 2; ++i) {
            const u32 w = Rand(n, 310 + i) % 3;
            (i ? c.topY : c.topX) = w == 0 ? Pick(kTiny, n, 312 + i) : w == 1 ? Denorm(n, 314 + i) : Sgn(ExpFloat(n, 316 + i, -126 + int(Rand(n, 318 + i) % 12)), Rand(n, 320 + i) % 2);
        }
        for (int g = 0; g < 2; ++g) for (int i = 0; i < 3; ++i) {
            const u32 w = Rand(n, 330 + 3 * g + i) % 4;
            c.res[g][i] = w == 0 ? Pick(kTiny, n, 340 + 3 * g + i) : w == 1 ? Denorm(n, 346 + 3 * g + i) : w == 2 ? Sgn(ExpFloat(n, 352 + 3 * g + i, -126 + int(Rand(n, 358 + 3 * g + i) % 12)), Rand(n, 364 + i) % 2) : FloatOf(n, 370 + 3 * g + i, 0u);
        }
        break;
    }
    if (Fires(c)) c.hasMat = 1;
    c.gate = Fires(c) ? 2u : 1u;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("now", c.now).U("gun", c.gun).U("ammo", c.ammo).U("has_mat", c.hasMat).U("top_x", c.topX).U("top_y", c.topY).U("top_z", c.topZ).U("driver", c.driver);
    for (int g = 0; g < 2; ++g) for (int i = 0; i < 3; ++i) j.U((std::string("res") + char('0' + g) + "xyz"[i]).c_str(), c.res[g][i]);
    for (int i = 0; i < 6; ++i) j.U((std::string("rnd") + char('0' + i)).c_str(), c.rnd[i]);
    j.U("doom_mode", c.doomMode);
    for (int i = 0; i < 6; ++i) j.U((std::string("dt") + char('0' + i)).c_str(), c.dt[i]);
    j.U("mut_at", c.mutAt).U("mut_kind", c.mutKind).U("mut_val", c.mutVal).U("gate", c.gate);
    return j.End();
}
}

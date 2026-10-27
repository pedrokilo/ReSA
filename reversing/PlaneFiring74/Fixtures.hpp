#pragma once
// Fixtures del banco PlaneFiring74 (lote 74): CVehicle::GetPlaneWeaponFiringStatus (EXE 0x6E3440), 18432 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CVehicle::GetPlaneWeaponFiringStatus  0x6E3440  thiscall, ECX = this (vehiculo), (bool& estado, eOrdnanceType& tipo), `ret 8`, EAX = objetivo   (hook CVehicle / GetPlaneWeaponFiringStatus, tipo S)
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

namespace Pf74Fix {
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

// Arenas: guarda + ventana + guarda (la ventana es el objeto). La del vehiculo mide sizeof(CPlane) = 0xA04 (LiveComparison.hpp lo comprueba con static_assert).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0xA04, kTgtWin = 0x40, kOthWin = 0x40, kMatWin = 0x48, kOutWin = 0x20;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kTgtArena = Arena(kTgtWin), kOthArena = Arena(kOthWin), kMatArena = Arena(kMatWin), kOutArena = Arena(kOutWin);
// Dentro de la ventana de salida: el bool (un byte) en +0x04 y el tipo de ordenanza (dword) en +0x10; el resto son vecinos que no deben cambiar
constexpr std::size_t kOutStatus = 0x04, kOutOrdnance = 0x10;

constexpr u32 kTimeAddr = 0xB7CB84;   // CTimer::m_snTimeInMilliseconds
constexpr u32 kPads0 = 0xB73458;      // CPad::Pads[0]
constexpr u32 kFlagA = 0xC1CC00;      // byte global leido para el RC Baron (CVehicle::bDisableRemoteDetonation)
constexpr u32 kFlagB = 0xB6EC2E;      // byte global leido para el RC Baron (raton en tercera persona)
constexpr u32 kCross = 0xC8A838;      // gCrossHair[2] (0x2C cada una)
constexpr u32 kCrossBytes = 0x58;
constexpr u8 kCrossFill = 0xA5;       // relleno conocido de la mira antes de cada lado (distinto de todo lo que escribe la funcion: 0xFF, 0x00, 1.0f)

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 74074u); }
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
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFA00000u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00001u, 0xFFBFFFFFu};
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
// Codigos de puntero (entidad objetivo del bloqueo y retornos de ScanAndMarkTargetForHeatSeekingMissile): 0 nulo, 1 entidad objetivo (arena), 2 otra entidad (arena), 3 el propio vehiculo, 4..6 basura cruda (nunca se desreferencia)
constexpr u32 kGarbageBase = 0xA0000010u;
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 model{};           // word en +0x22 (solo 16 bits)
    u32 slot{};            // retorno de CWorld::FindPlayerSlotWithVehiclePointer (cualquier dword: el pad es Pads[slot])
    u32 gf[2]{}, jd[2]{};  // retornos 1.o y 2.o de CPad::GetCarGunFired / CPad::CarGunJustDown (el EXE compara solo AX; los 16 bits altos son basura)
    u32 et{}, tg{};        // retornos de CPad::GetEnterTargeting / CPad::GetTarget (el EXE compara solo AL; el resto es basura)
    u32 locked{};          // retorno de CWeaponEffects::IsLockedOn (AL)
    u32 scan[2]{};         // codigos de los retornos 1.o y 2.o de ScanAndMarkTargetForHeatSeekingMissile
    u32 now{};             // CTimer::m_snTimeInMilliseconds
    u32 gun{};             // +0x4D4 (m_nGunFiringTime)
    u32 lockStart{};       // +0x9DC
    u32 lockTarget{};      // +0x9E0 (codigo)
    u32 flagA{}, flagB{};  // bytes globales 0xC1CC00 y 0xB6EC2E
    u32 hasMat{};          // el vehiculo tiene matriz (si no, +0x14 = 0): solo valido cuando no hay bengala
    u32 fwd[3]{}, up[3]{}, mpos[3]{}, ppos[3]{};
    u32 projRet{};         // EAX del falso de AddProjectile
    u32 stInit{}, ordInit{};   // valores previos del bool (byte) y del tipo de ordenanza (dword)
    u32 gate{};            // informativo
};

// Tabla del EXE (0x6E3804): 0 Hunter y 576, 1 Seasparrow/RC Baron/Maverick/Police Maverick/Cargobob, 2 Rustler, 3 Hydra, 4 resto (indice sin signo > 0x97 o entrada 4)
inline int KindOf(u32 model) {
    const std::int32_t s = std::int32_t(std::int16_t(std::uint16_t(model)));
    const u32 slot = u32(s - 425);
    if (slot > 0x97u) return 4;
    switch (s) {
    case 425: case 576: return 0;
    case 447: case 464: case 487: case 497: case 548: return 1;
    case 476: return 2;
    case 520: return 3;
    default: return 4;
    }
}
inline bool FlareFires(const C& c) { return KindOf(c.model) == 3 && std::uint16_t(c.jd[0]) == 1 && c.now > c.gun + 2000u; }

inline u32 Code(u32 n, u32 k) {
    const u32 r = Rand(n, k) % 10;
    return r < 3 ? 0u : r < 5 ? 1u : r < 7 ? 2u : r == 7 ? 3u : 4u + Rand(n, k + 1) % 3u;
}
// Retorno de un metodo del pad que devuelve int16: el EXE mira solo AX; los 16 bits altos de EAX son basura
inline u32 PadRet(u32 n, u32 k, u32 w0 = 25, u32 w1 = 30, u32 w2 = 30) {
    static constexpr std::array<u32, 8> other{3u, 0xFFFFu, 0x8000u, 0xFFFEu, 0x7FFFu, 0x0100u, 0x0102u, 0x0201u};
    const u32 r = Rand(n, k) % 100;
    u32 lo;
    if (r < w0) lo = 0u;
    else if (r < w0 + w1) lo = 1u;
    else if (r < w0 + w1 + w2) lo = 2u;
    else lo = Pick(other, n, k + 1);
    const u32 hi = Rand(n, k + 2) % 3 == 0 ? 0u : (Rand(n, k + 3) & 0xFFFF0000u);
    return hi | lo;
}
// Retorno de un metodo que devuelve bool: AL decide; el resto de EAX es basura
inline u32 BoolRet(u32 n, u32 k, u32 pctTrue = 50) {
    const u32 hi = Rand(n, k + 2) % 3 == 0 ? 0u : (Rand(n, k + 3) & 0xFFFFFF00u);
    const u32 r = Rand(n, k) % 100;
    const u32 lo = r < pctTrue ? (Rand(n, k + 1) % 4 == 0 ? 1u + Rand(n, k + 4) % 255u : 1u) : 0u;
    return hi | lo;
}
inline u32 PickModel(u32 n, u32 k, int kind) {
    static constexpr std::array<u32, 9> hunter{425u, 576u, 425u, 576u, 425u, 576u, 425u, 576u, 425u};
    static constexpr std::array<u32, 9> guns{447u, 464u, 487u, 497u, 548u, 464u, 464u, 447u, 548u};
    static constexpr std::array<u32, 12> none{424u, 426u, 446u, 448u, 463u, 465u, 475u, 477u, 486u, 488u, 496u, 498u};
    static constexpr std::array<u32, 12> none2{519u, 521u, 547u, 549u, 575u, 577u, 0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, 0xFFFEu};
    switch (kind) {
    case 0: return Pick(hunter, n, k);
    case 1: return Pick(guns, n, k);
    case 2: return 476u;
    case 3: return 520u;
    default: return Rand(n, k + 7) % 3 == 0 ? Pick(none2, n, k) : Rand(n, k + 8) % 4 == 0 ? Rand(n, k + 9) & 0xFFFFu : Pick(none, n, k);
    }
}

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    // modelo: reparto por defecto; las familias lo fuerzan
    {
        const u32 r = Rand(n, 10) % 20;
        const int kind = r < 3 ? 0 : r < 7 ? 1 : r < 9 ? 2 : r < 15 ? 3 : 4;
        c.model = PickModel(n, 11, kind);
    }
    switch (f) {
    case 0: c.model = PickModel(n, 11, 0); break;
    case 1: c.model = PickModel(n, 11, 1); break;
    case 2: c.model = 464u; break;
    case 3: c.model = 476u; break;
    case 4: case 5: case 6: case 7: case 10: case 11: case 13: case 16: case 17: c.model = 520u; break;
    case 9: c.model = PickModel(n, 11, 4); if (Rand(n, 12) % 5 == 0) c.model = PickModel(n, 13, int(Rand(n, 14) % 4)); break;
    default: break;
    }
    c.slot = Rand(n, 15) % 3 == 0 ? Rand(n, 16) % 2 : Rand(n, 17) % 2 ? Rand(n, 18) % 4 : (Rand(n, 19) % 2 ? Rand(n, 20) : Pick(kStackSeeds, n, 21));
    if (f == 12) c.slot = Rand(n, 22) % 2 ? (Rand(n, 23) % 8) : Rand(n, 24);
    c.gf[0] = PadRet(n, 30); c.gf[1] = PadRet(n, 36);
    c.jd[0] = PadRet(n, 42); c.jd[1] = PadRet(n, 48);
    c.et = BoolRet(n, 54); c.tg = BoolRet(n, 60); c.locked = BoolRet(n, 66, 60);
    c.scan[0] = Code(n, 72); c.scan[1] = Code(n, 74);
    c.flagA = Rand(n, 80) % 3 == 0 ? 0u : Rand(n, 81) % 3 == 0 ? (Rand(n, 82) & 0xFFu) : 1u;
    c.flagB = Rand(n, 83) % 3 == 0 ? 0u : Rand(n, 84) % 3 == 0 ? (Rand(n, 85) & 0xFFu) : 1u;
    c.projRet = Rand(n, 86);
    c.stInit = Rand(n, 87) & 0xFFu; c.ordInit = Rand(n, 88) % 4 == 0 ? Pick(kStackSeeds, n, 89) : Rand(n, 90);
    // tiempos
    static constexpr std::array<u32, 6> guns0{0u, 1u, 1999u, 2000u, 0xFFFFFFFFu, 0xFFFFF830u};
    c.gun = Rand(n, 91) % 3 == 0 ? Pick(guns0, n, 92) : Rand(n, 93) % 2 ? Rand(n, 94) % 100000u : Rand(n, 95);
    {
        const u32 lim = c.gun + 2000u;   // uint32 con desbordamiento, como el EXE
        switch (Rand(n, 96) % 10) {
        case 0: c.now = lim; break;
        case 1: c.now = lim + 1u; break;
        case 2: c.now = lim - 1u; break;
        case 3: c.now = lim + 2u; break;
        case 4: c.now = lim + Rand(n, 97) % 5000u; break;
        case 5: c.now = lim - Rand(n, 97) % 5000u; break;
        case 6: c.now = 0u; break;
        case 7: c.now = 0xFFFFFFFFu; break;
        default: c.now = Rand(n, 98); break;
        }
    }
    {
        switch (Rand(n, 99) % 12) {
        case 0: case 1: case 2: c.lockStart = 0u; break;
        case 3: c.lockStart = c.now - 1500u; break;
        case 4: c.lockStart = c.now - 1501u; break;
        case 5: c.lockStart = c.now - 1499u; break;
        case 6: c.lockStart = c.now; break;
        case 7: c.lockStart = c.now + 1u + Rand(n, 100) % 3000u; break;     // en el futuro: la resta con desbordamiento es enorme
        case 8: c.lockStart = c.now - Rand(n, 100) % 20000u; break;
        case 9: c.lockStart = 1u; break;
        case 10: c.lockStart = 0xFFFFFFFFu; break;
        default: c.lockStart = Rand(n, 101); break;
        }
    }
    c.lockTarget = Code(n, 102);
    if (Rand(n, 103) % 3 == 0) c.scan[0] = c.lockTarget;   // el objetivo se mantiene
    if (Rand(n, 104) % 3 == 0) c.scan[1] = c.lockTarget;
    c.hasMat = 1;
    // matriz y posicion: floats moderados por defecto
    for (int i = 0; i < 3; ++i) {
        c.fwd[i] = FloatOf(n, 110 + i, Rand(n, 113 + i) % 8 == 0 ? 5u : 0u);
        c.up[i] = FloatOf(n, 116 + i, Rand(n, 119 + i) % 8 == 0 ? 5u : 0u);
        c.mpos[i] = FloatOf(n, 122 + i, Rand(n, 125 + i) % 8 == 0 ? 5u : 0u);
        c.ppos[i] = FloatOf(n, 128 + i, 0u);
    }
    auto flarePath = [&] {   // fuerza el camino de la bengala: Hydra, 1.o CarGunJustDown == 1 (AX) y hora pasado el intervalo
        c.model = 520u;
        c.jd[0] = (c.jd[0] & 0xFFFF0000u) | 1u;
        c.now = c.gun + 2001u + (Rand(n, 140) % 4 == 0 ? Rand(n, 141) % 5000u : 0u);
    };
    auto allFloats = [&](u32 mode, u32 salt) {
        for (int i = 0; i < 3; ++i) { c.fwd[i] = FloatOf(n, 150 + i + salt, mode); c.up[i] = FloatOf(n, 153 + i + salt, mode); c.mpos[i] = FloatOf(n, 156 + i + salt, mode); }
    };
    switch (f) {
    case 0: case 1: case 2: case 3:   // Hunter, helicopteros ligeros, RC Baron y Rustler: combinaciones de los retornos del pad
        if (Rand(n, 142) % 3 == 0) c.gf[1] = PadRet(n, 143, 30, 40, 20);
        if (f == 2) { c.flagA = Rand(n, 144) % 2 ? 1u : (Rand(n, 145) % 2 ? 0u : 0x80u); c.flagB = Rand(n, 146) % 2 ? 1u : (Rand(n, 147) % 2 ? 0u : 0xFFu); c.gf[0] = PadRet(n, 148, 20, 30, 40); c.gf[1] = PadRet(n, 149, 20, 60, 10); }
        break;
    case 4:   // bengala con floats moderados
        flarePath();
        break;
    case 5: {  // borde del intervalo de bengalas: ahora = gun + 2000 (no), +2001 (si), con desbordamiento, 1.er CarGunJustDown == 1
        c.model = 520u; c.jd[0] = (c.jd[0] & 0xFFFF0000u) | 1u;
        const u32 w = Rand(n, 140) % 6;
        c.now = c.gun + 2000u + (w == 0 ? 0u : w == 1 ? 1u : w == 2 ? 0xFFFFFFFFu : w == 3 ? 2u : w == 4 ? 0u : 1u);
        break;
    }
    case 6:   // L1 (2.o CarGunJustDown == 2): rama del misil guiado o sin guia
        c.jd[1] = (c.jd[1] & 0xFFFF0000u) | 2u;
        if (Rand(n, 140) % 3 != 0) c.locked = (c.locked & ~0xFFu) | 1u;
        if (Rand(n, 141) % 4 != 0 && c.lockStart == 0u) c.lockStart = c.now - Rand(n, 142) % 4000u;
        if (Rand(n, 143) % 2) c.scan[0] = c.lockTarget;
        break;
    case 7:   // sin L1: apuntar (GetEnterTargeting / GetTarget / nada) y la temporizacion de la mira
        c.jd[1] = (c.jd[1] & 0xFFFF0000u) | (Rand(n, 140) % 5 == 0 ? 3u : (Rand(n, 141) % 2 ? 0u : 1u));
        if (Rand(n, 142) % 2) c.tg = (c.tg & ~0xFFu) | 1u;
        if (Rand(n, 143) % 8 == 0) c.et = c.et & ~0xFFu;   // sin apuntar de entrada: llega a GetTarget
        if (Rand(n, 144) % 5 < 2) {
            c.tg = (c.tg & ~0xFFu) | 1u; c.et = c.et & ~0xFFu; c.jd[1] = (c.jd[1] & 0xFFFF0000u) | 1u;   // objetivo mantenido con la fijacion en el borde de los 1500 ms (1499, 1500, 1501), con y sin desbordamiento
            c.lockTarget = 1u + Rand(n, 145) % 3u;
            c.scan[0] = c.lockTarget;
            const u32 d = Rand(n, 146) % 3 == 0 ? 1499u : Rand(n, 146) % 2 ? 1500u : 1501u;
            c.lockStart = c.now - d;
            if (c.lockStart == 0u) c.lockStart = c.now - d - 1u;
        }
        break;
    case 8: break;   // Hydra con todo aleatorio
    case 9: break;   // modelos fuera de tabla y de frontera
    case 10:  // bengala con valores especiales (NaN no canonicos, infinitos, desnormales, +-0)
        flarePath(); allFloats(3u, 0u);
        break;
    case 11:  // bengala con magnitudes extremas: desnormales de productos y desbordamientos
        flarePath(); allFloats(Rand(n, 144) % 2 ? 1u : 2u, 0u);
        for (int i = 0; i < 3; ++i) if (Rand(n, 160 + i) % 3 == 0) c.mpos[i] = FloatOf(n, 163 + i, 3u);
        break;
    case 12: break;   // pad por ranura: cualquier dword
    case 13:  // Hydra sin matriz (solo si no hay bengala)
        c.hasMat = 0;
        if (FlareFires(c)) c.now = c.gun + 2000u - Rand(n, 144) % 3u;
        break;
    case 14: case 15:  // todo aleatorio, con y sin camino de bengala
        if (Rand(n, 145) % 2) flarePath();
        if (Rand(n, 146) % 4 == 0) allFloats(5u, 0u);
        break;
    case 16: {  // bengala con DOS NaN en la misma componente (forward/up, up/pos, forward/pos) y con cargas, signos y senal distintos
        flarePath();
        for (int i = 0; i < 3; ++i) {
            c.fwd[i] = FloatOf(n, 170 + i, 0u); c.up[i] = FloatOf(n, 173 + i, 0u); c.mpos[i] = FloatOf(n, 176 + i, 0u);
            if (Rand(n, 180 + i) % 3 != 0) {
                const u32 w = Rand(n, 183 + i) % 4;
                const u32 a = Pick(kNan, n, 186 + i), b = Pick(kNan, n, 189 + i);
                const bool same = Rand(n, 192 + i) % 4 == 0;   // misma carga, distinto signo
                if (w == 0) { c.fwd[i] = a; c.up[i] = same ? (a ^ 0x80000000u) : b; }
                else if (w == 1) { c.up[i] = a; c.mpos[i] = same ? (a ^ 0x80000000u) : b; }
                else if (w == 2) { c.fwd[i] = a; c.mpos[i] = same ? (a ^ 0x80000000u) : b; }
                else { c.fwd[i] = a; c.up[i] = b; c.mpos[i] = Pick(kNan, n, 195 + i); }
            }
        }
        break;
    }
    default: {  // 17: ceros firmados, desnormales que se cancelan y sumas en el limite de la cancelacion
        flarePath();
        for (int i = 0; i < 3; ++i) {
            const u32 w = Rand(n, 170 + i) % 5;
            if (w == 0) { c.fwd[i] = Pick(kTiny, n, 173 + i); c.up[i] = Pick(kTiny, n, 176 + i); c.mpos[i] = Pick(kTiny, n, 179 + i); }
            else if (w == 1) { c.fwd[i] = Denorm(n, 173 + i); c.up[i] = Sgn(Denorm(n, 176 + i), Rand(n, 177 + i) % 2); c.mpos[i] = Denorm(n, 179 + i); }
            else if (w == 2) {   // cancelacion exacta de t1 y t2: arriba = t1 (t2 = -arriba = -t1), de modo que t2 + t1 = +0 con t1 distinto de cero; la posicion decide el signo del cero de salida
                c.fwd[i] = FloatOf(n, 173 + i, 4u);
                const volatile float t1 = F(c.fwd[i]) * -2.5f;
                c.up[i] = B(t1);
                c.mpos[i] = Rand(n, 177 + i) % 3 == 0 ? (Rand(n, 178 + i) % 2 ? 0x80000000u : 0u) : B(-F(c.up[i])) ^ (Rand(n, 179 + i) % 2 ? 0u : 0x80000000u);
            }
            else if (w == 3) {   // ceros: adelante y arriba +0 dan t1 = t2 = -0 (s = -0) y con posicion -0 la salida es -0
                const bool negSum = Rand(n, 173 + i) % 2 != 0;
                c.fwd[i] = negSum ? 0u : B(0.0f) | (Rand(n, 174 + i) % 2 ? 0x80000000u : 0u);
                c.up[i] = negSum ? 0u : B(0.0f) | (Rand(n, 176 + i) % 2 ? 0x80000000u : 0u);
                c.mpos[i] = Rand(n, 179 + i) % 3 != 0 ? 0x80000000u : B(0.0f) | (Rand(n, 180 + i) % 2 ? 0x80000000u : 0u);
            }
            else { c.fwd[i] = ExpFloat(n, 173 + i, -126 + int(Rand(n, 176 + i) % 12)); c.up[i] = ExpFloat(n, 179 + i, -126 + int(Rand(n, 182 + i) % 12)); c.mpos[i] = ExpFloat(n, 185 + i, -126 + int(Rand(n, 188 + i) % 12)); }
        }
        break;
    }
    }
    // la matriz nula solo vale si no hay bengala (el EXE lee matriz + 0x10 sin comprobar nulo)
    if (FlareFires(c)) c.hasMat = 1;
    if (KindOf(c.model) == 3 && std::uint16_t(c.jd[0]) == 1 && c.now == c.gun + 2000u) c.hasMat = 1;   // borde exacto: matriz valida, para que la mutacion `>=` se vea como diferencia de resultado y no como caida
    c.gate = FlareFires(c) ? 2u : 1u;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("model", c.model).U("slot", c.slot).U("gf0", c.gf[0]).U("gf1", c.gf[1]).U("jd0", c.jd[0]).U("jd1", c.jd[1]);
    j.U("et", c.et).U("tg", c.tg).U("locked", c.locked).U("scan0", c.scan[0]).U("scan1", c.scan[1]);
    j.U("now", c.now).U("gun", c.gun).U("lock_start", c.lockStart).U("lock_target", c.lockTarget).U("flag_a", c.flagA).U("flag_b", c.flagB).U("has_mat", c.hasMat);
    j.U("fwd_x", c.fwd[0]).U("fwd_y", c.fwd[1]).U("fwd_z", c.fwd[2]).U("up_x", c.up[0]).U("up_y", c.up[1]).U("up_z", c.up[2]);
    j.U("mpos_x", c.mpos[0]).U("mpos_y", c.mpos[1]).U("mpos_z", c.mpos[2]).U("ppos_x", c.ppos[0]).U("ppos_y", c.ppos[1]).U("ppos_z", c.ppos[2]);
    j.U("proj_ret", c.projRet).U("st_init", c.stInit).U("ord_init", c.ordInit).U("gate", c.gate);
    return j.End();
}
}

#pragma once
// Fixtures del banco Veh83B (lote 83B): CVehicle::InflictDamage (EXE 0x6D7C90), 18432 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CVehicle::InflictDamage  0x6D7C90  thiscall, ECX = this, (CEntity* damager, eWeaponType weapon, float intensity, CVector coords por valor) = 6 dwords, `ret 0x18`, void   (hook CVehicle / InflictDamage, tipo S)
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

namespace V83Fix {
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
constexpr std::size_t kVehWin = 0x990, kPedWin = 0x700, kSmallWin = 0x100, kMatWin = 0x48, kMiWin = 0x80;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kPedArena = Arena(kPedWin), kSmallArena = Arena(kSmallWin), kMatArena = Arena(kMatWin), kMiArena = Arena(kMiWin);
constexpr std::size_t kIntelOff = 0x600;     // CPedIntelligence dentro de la arena del peaton (m_eventGroup en +0x68)

constexpr u32 kTimeAddr = 0xB7CB5C;   // CTimer::ms_fTimeStep

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 83083u); }
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
// Arenas (indices de etiqueta): 0 vehiculo (this), 1 atacante D (peaton), 2 conductor, 3..6 pasajeros p0..p3 (peatones), 7 X (entidad que no es peaton),
// 8 Vd (vehiculo del atacante), 9 PL (peaton "jugador" de FindPlayerPed), 10 modelo (CVehicleModelInfo), 11 estructura del modelo, 12 matriz del vehiculo, 13 Vp (vehiculo del jugador).
constexpr unsigned kArVeh = 0, kArD = 1, kArDrv = 2, kArP0 = 3, kArX = 7, kArVd = 8, kArPL = 9, kArMi = 10, kArVs = 11, kArMat = 12, kArVp = 13;
// Modelos que el banco instala en ms_modelInfoPtrs (el EXE indexa con el word con signo de +0x22)
constexpr std::array<u32, 8> kModels{0x20Bu, 400u, 401u, 402u, 500u, 0x1A0u, 600u, 0u};
constexpr u32 kModelTable = 0xA9B0C8;
constexpr u32 kPlayers = 0xB7CD98, kPlayerInFocus = 0xB7CD74;
constexpr u32 kExplosion = 0x33;

struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 dmgKind{};                      // 0 nulo, 1 D, 2 X (no peaton), 3 conductor, 4 p0
    u32 weapon{}, intensity{}, coord[3]{};
    u32 status{}, vtype{}, vsub{}, model{};   // estado (5 bits), dwords +0x590 y +0x594, indice de kModels del word +0x22
    u32 lawEnf{}, tyresDont{}, isRC{}, petrol{};
    u32 hasDriver{}, pass[8]{}, maxPass{};     // pass[i]: 0 nulo, 1..4 -> p0..p3
    u32 health{};
    u32 canRet{}, canFire{};
    u32 progress{};
    u32 fpPed{}, fpVeh{};               // FindPlayerPed: 0 nulo, 1 D, 2 conductor, 3 p0, 4 PL;  FindPlayerVehicle: 0 nulo, 1 vehiculo, 2 X, 3 Vp
    u32 isPlayerRet{}, appearance{}, tyreRet{};
    u32 randSeed{};
    u32 dmgType{}, dmgHasVeh{}, vdSub{}, drvPedType{};   // byte +0x36 de D (tipo en los 3 bits bajos), D.m_pVehicle != nulo, +0x594 de Vd, m_nPedType del conductor
    u32 inFocus{}, havoc0{}, chase0{};
    u32 dummy[3]{}, mat[12]{};
    u32 gate{};                         // informativo
};

constexpr std::array<u32, 12> kMatOff{0x00, 0x04, 0x08, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28, 0x30, 0x34, 0x38};

inline u32 RetOk(u32 n, u32 k) { const u32 lo = 1u + Rand(n, k) % 255u; return (Rand(n, k + 1) % 3 == 0 ? 0u : (Rand(n, k + 2) & 0xFFFFFF00u)) | lo; }
inline u32 RetFail(u32 n, u32 k) { return Rand(n, k) % 3 == 0 ? 0u : (Rand(n, k + 2) & 0xFFFFFF00u); }
// Armas del banco: las de la tabla del EXE (22..52), las especiales y basura
inline u32 PickWeapon(u32 n, u32 k) {
    static constexpr std::array<u32, 12> special{0x33u, 0x10u, 0x13u, 0x14u, 24u, 22u, 52u, 53u, 21u, 0u, 0xFFFFFFFFu, 30u};
    const u32 r = Rand(n, k) % 8;
    if (r < 3) return special[Rand(n, k + 1) % special.size()];
    if (r < 7) return 20u + Rand(n, k + 2) % 36u;
    return Rand(n, k + 3);
}
inline u32 Dmg(u32 n, u32 k) {   // 0 nulo, 1 D, 2 X, 3 conductor, 4 p0
    static constexpr std::array<u32, 8> w{1, 1, 1, 1, 3, 4, 2, 0};
    return w[Rand(n, k) % w.size()];
}

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    auto fl = [&](u32 k, u32 mode = 0u) { return FloatOf(n, k, mode); };
    auto dmgFloat = [&](u32 k) {   // intensidades y vidas con valores de borde
        static constexpr std::array<float, 16> e{0.f, 1.f, 10.f, 10.001f, 9.999f, 250.f, 249.999f, 250.001f, 100.f, 500.f, 1000.f, 1100.f, 1100.1f, 2000.f, 5.f, 50.f};
        const u32 r = Rand(n, k) % 4;
        if (r == 0) return B(e[Rand(n, k + 1) % e.size()]);
        if (r == 1) return B(float(Rand(n, k + 2) % 20000) / 10.0f);
        if (r == 2) return B(float(Rand(n, k + 3) % 300) * 0.5f);
        return fl(k + 4, 4u);
    };
    c.dmgKind = Dmg(n, 10);
    c.weapon = PickWeapon(n, 11);
    c.intensity = dmgFloat(12);
    for (int i = 0; i < 3; ++i) c.coord[i] = fl(14 + i, Rand(n, 17 + i) % 3 == 0 ? 4u : 0u);
    c.status = Rand(n, 20) % 3 == 0 ? Rand(n, 21) % 32u : Rand(n, 22) % 6u;
    {
        static constexpr std::array<u32, 8> vt{0u, 0u, 0u, 9u, 9u, 5u, 3u, 11u};
        c.vtype = vt[Rand(n, 23) % vt.size()];
        static constexpr std::array<u32, 8> vs{0u, 0u, 3u, 4u, 9u, 3u, 4u, 6u};
        c.vsub = vs[Rand(n, 24) % vs.size()];
    }
    c.model = Rand(n, 25) % 4 == 0 ? 0u : Rand(n, 26) % kModels.size();
    c.lawEnf = Rand(n, 27) % 3 == 0; c.tyresDont = Rand(n, 28) % 4 == 0; c.isRC = Rand(n, 29) % 4 == 0; c.petrol = Rand(n, 30) % 2;
    c.hasDriver = Rand(n, 31) % 3 != 0;
    for (int i = 0; i < 8; ++i) c.pass[i] = Rand(n, 32 + i) % 3 == 0 ? 1u + Rand(n, 40 + i) % 4u : 0u;
    c.maxPass = Rand(n, 48) % 4 == 0 ? Rand(n, 49) % 9u : 1u + Rand(n, 50) % 4u;
    c.health = dmgFloat(51);
    c.canRet = Rand(n, 56) % 8 == 0 ? RetFail(n, 57) : RetOk(n, 57);
    c.canFire = Rand(n, 58) % 2 ? 1u : (Rand(n, 59) % 3 == 0 ? Rand(n, 60) & 0xFFu : 0u);
    {
        static constexpr std::array<float, 8> p{99.99f, 100.f, 100.01f, 0.f, 50.f, 150.f, 100.f, 99.f};
        c.progress = Rand(n, 61) % 3 == 0 ? B(p[Rand(n, 62) % p.size()]) : fl(63, 4u);
    }
    c.fpPed = Rand(n, 64) % 4 == 0 ? Rand(n, 65) % 5u : (Rand(n, 66) % 2 ? 1u : 0u);
    c.fpVeh = Rand(n, 67) % 4 == 0 ? Rand(n, 68) % 4u : 0u;
    c.isPlayerRet = Rand(n, 69) % 3 == 0 ? RetOk(n, 70) : RetFail(n, 71);
    c.appearance = Rand(n, 72) % 2 ? 1u : (Rand(n, 73) % 3 == 0 ? Rand(n, 74) : Rand(n, 75) % 6u);
    c.tyreRet = Rand(n, 76) % 3 == 0 ? Rand(n, 77) : Rand(n, 78) % 4u;
    c.randSeed = Rand(n, 79) % 5 == 0 ? Pick(kStackSeeds, n, 80) : Rand(n, 81);
    c.dmgType = Rand(n, 82) % 8 == 0 ? Rand(n, 83) % 8u : 3u;
    c.dmgHasVeh = Rand(n, 84) % 3 == 0; c.vdSub = Rand(n, 85) % 3 == 0 ? 9u : Rand(n, 86) % 12u;
    c.drvPedType = Rand(n, 87) % 3 == 0 ? 6u : Rand(n, 88) % 10u;
    c.inFocus = Rand(n, 89) % 4; c.havoc0 = Rand(n, 90); c.chase0 = Rand(n, 91) % 3 == 0 ? fl(92, 3u) : fl(93, 0u);
    for (int i = 0; i < 3; ++i) c.dummy[i] = Rand(n, 94 + i) % 4 == 0 ? 0u : fl(97 + i, 4u);
    for (int i = 0; i < 12; ++i) c.mat[i] = fl(100 + i, 4u);
    auto setDmgPed = [&] { c.dmgKind = Rand(n, 120) % 6 == 0 ? 3u : 1u; c.dmgType = 3u; };
    switch (f) {
    case 0:                                                      // CanVehicleBeDamaged devuelve falso (basura en los bits altos)
        c.canRet = RetFail(n, 121); break;
    case 1:                                                      // estado de jugador: progreso frente a 100
        c.canRet = RetOk(n, 122); c.status = Rand(n, 123) % 4 == 0 ? Rand(n, 124) % 32u : 0u;
        break;
    case 2:                                                      // el jugador (o su vehiculo) hace dano: caos, estadistica y rand
        c.canRet = RetOk(n, 125); c.intensity = Rand(n, 126) % 5 == 0 ? dmgFloat(127) : B(10.0f + float(1 + Rand(n, 128) % 500));
        c.status = Rand(n, 129) % 3 == 0 ? 5u : Rand(n, 130) % 5u;
        if (Rand(n, 131) % 2) { c.dmgKind = 1; c.fpPed = 1; } else { c.dmgKind = 2; c.fpPed = Rand(n, 132) % 2 ? 4u : 0u; c.fpVeh = 2; }
        break;
    case 3:                                                      // rueda reventada de un coche
        c.canRet = RetOk(n, 133); setDmgPed(); c.vtype = 0; c.tyresDont = Rand(n, 134) % 6 == 0; c.isPlayerRet = RetFail(n, 135);
        c.weapon = 22u + Rand(n, 136) % 31u; c.dmgHasVeh = 0; c.appearance = Rand(n, 137) % 4 ? 1u : Rand(n, 138) % 5u; c.intensity = dmgFloat(139);
        break;
    case 4:                                                      // rueda reventada de una moto
        c.canRet = RetOk(n, 140); setDmgPed(); c.vtype = 9; c.tyresDont = Rand(n, 141) % 12 == 0; c.isPlayerRet = RetFail(n, 142);
        c.weapon = Rand(n, 146) % 2 ? 24u : 22u + Rand(n, 143) % 31u; c.dmgHasVeh = Rand(n, 144) % 6 == 0; c.vdSub = Rand(n, 145) % 2 ? 9u : 0u;
        break;
    case 5:                                                      // casos especiales de la rueda: atacante en moto, moto de policia, jugador
        c.canRet = RetOk(n, 146); setDmgPed(); c.vtype = Rand(n, 147) % 3 ? 0u : 9u; c.weapon = Rand(n, 148) % 3 == 0 ? 24u : 22u + Rand(n, 149) % 31u;
        c.dmgHasVeh = Rand(n, 150) % 2; c.vdSub = Rand(n, 151) % 2 ? 9u : 3u; c.hasDriver = Rand(n, 152) % 4 != 0; c.drvPedType = Rand(n, 153) % 2 ? 6u : 2u;
        c.model = Rand(n, 154) % 2 ? 0u : Rand(n, 155) % kModels.size(); c.isPlayerRet = Rand(n, 156) % 5 == 0 ? RetOk(n, 157) : RetFail(n, 158);
        c.tyresDont = Rand(n, 159) % 5 == 0;
        break;
    case 6:                                                      // la rueda se rompe solo si el coche tiene aspecto de automovil
        c.canRet = RetOk(n, 160); setDmgPed(); c.vtype = 0; c.weapon = 24u; c.tyresDont = 0; c.isPlayerRet = RetFail(n, 161); c.dmgHasVeh = 0;
        c.appearance = Rand(n, 162) % 2 ? 1u : Rand(n, 163) % 7u; c.hasDriver = 0;
        break;
    case 7:                                                      // deposito de gasolina
        c.canRet = RetOk(n, 164); setDmgPed(); c.petrol = 1; c.canFire = 1u | (Rand(n, 165) % 3 == 0 ? Rand(n, 166) & 0xFFFFFF00u : 0u); c.isPlayerRet = RetOk(n, 167);
        c.vtype = Rand(n, 168) % 2 ? 0u : 9u;
        if (Rand(n, 169) % 8 == 0) { c.dummy[0] = c.dummy[1] = c.dummy[2] = 0u; }
        if (Rand(n, 170) % 4 != 0) {   // coordenadas cerca del deposito en el mundo (matriz * dummy, orden de sumas del EXE) para que el borde 0.25 se ejerza
            const double dx = F(c.dummy[0]), dy = F(c.dummy[1]), dz = F(c.dummy[2]);
            auto m = [&](int i) { return double(F(c.mat[i])); };
            const double w[3]{((m(6) * dz + m(3) * dy) + m(0) * dx) + m(9), ((m(7) * dz + m(1) * dx) + m(4) * dy) + m(10), ((m(8) * dz + m(2) * dx) + m(5) * dy) + m(11)};
            for (int i = 0; i < 3; ++i) c.coord[i] = B(float(w[i] + (Rand(n, 171 + i) % 2 ? 1.0 : -1.0) * double(Rand(n, 174 + i) % 150) * 0.001));
        }
        c.health = Rand(n, 177) % 2 ? B(1000.f) : dmgFloat(178); c.intensity = dmgFloat(179);
        break;
    case 8:                                                      // helicopteros y aviones: 0.4 de dano salvo RC y armas especiales
        c.canRet = RetOk(n, 180); c.vsub = Rand(n, 181) % 2 ? 3u : 4u; c.isRC = Rand(n, 182) % 3 == 0; c.vtype = Rand(n, 183) % 2 ? 5u : 0u;
        {
            static constexpr std::array<u32, 8> wp{0x33u, 0x10u, 0x13u, 0x14u, 0x12u, 0x15u, 0x34u, 0x32u};
            if (Rand(n, 184) % 2) c.weapon = wp[Rand(n, 185) % wp.size()];
        }
        c.health = B(1000.f); c.intensity = Rand(n, 186) % 2 ? dmgFloat(187) : B(float(11 + Rand(n, 188) % 400));
        break;
    case 9:                                                      // dano no letal: eventos para el conductor y los pasajeros
        c.canRet = RetOk(n, 189); setDmgPed(); c.health = B(1000.f); c.intensity = B(float(1 + Rand(n, 190) % 200));
        c.status = Rand(n, 191) % 2 ? 0u : Rand(n, 192) % 5u; c.vtype = 5u; c.vsub = 6u;
        break;
    case 10:                                                     // la vida cruza 250 hacia abajo (automovil): motor
        c.canRet = RetOk(n, 193); setDmgPed(); c.vtype = Rand(n, 194) % 4 ? 0u : 5u; c.vsub = 6u;
        {
            const float oldH = Rand(n, 195) % 4 == 0 ? 250.f : float(200 + Rand(n, 196) % 200);
            c.health = B(oldH); c.intensity = B(Rand(n, 197) % 3 == 0 ? oldH - 250.f + float(Rand(n, 198) % 3) * 0.5f : float(Rand(n, 199) % 150));
        }
        break;
    case 11:                                                     // letal: vida <= dano. Crimen, explosion o BlowUpCar
        c.canRet = RetOk(n, 200); c.health = B(float(10 + Rand(n, 201) % 400)); c.intensity = Rand(n, 202) % 4 == 0 ? c.health : B(F(c.health) + float(Rand(n, 203) % 300));
        c.fpPed = Rand(n, 204) % 3 ? 1u : Rand(n, 205) % 5u; setDmgPed(); c.vsub = Rand(n, 206) % 3 == 0 ? 3u : (Rand(n, 207) % 2 ? 4u : 0u);
        c.weapon = Rand(n, 208) % 3 == 0 ? kExplosion : PickWeapon(n, 209);
        break;
    case 12:                                                     // vida <= 0 o NaN: no se toca la vida; solo la ley
        c.canRet = RetOk(n, 210); c.health = Rand(n, 211) % 3 == 0 ? fl(212, 3u) : (Rand(n, 213) % 2 ? B(0.f) : B(-float(Rand(n, 214) % 100)));
        c.lawEnf = Rand(n, 215) % 2; c.fpPed = Rand(n, 216) % 2 ? 1u : Rand(n, 217) % 5u; c.dmgKind = Rand(n, 218) % 3 ? 1u : Dmg(n, 219);
        break;
    case 13:                                                     // flotantes especiales en las entradas
        c.canRet = RetOk(n, 220); c.intensity = fl(221, Rand(n, 222) % 2 ? 3u : 5u); c.health = fl(223, Rand(n, 224) % 2 ? 3u : 5u); c.progress = fl(225, 3u);
        for (int i = 0; i < 3; ++i) c.coord[i] = fl(226 + i, 3u);
        c.status = Rand(n, 229) % 3 == 0 ? 0u : c.status; c.petrol = 1; c.canFire = 1; c.isPlayerRet = RetOk(n, 230); setDmgPed();
        for (int i = 0; i < 3; ++i) if (Rand(n, 231 + i) % 3 == 0) c.dummy[i] = fl(234 + i, 3u);
        for (int i = 0; i < 12; ++i) if (Rand(n, 240 + i) % 4 == 0) c.mat[i] = fl(260 + i, 3u);
        break;
    case 14: case 15:                                            // aleatorio
        if (Rand(n, 280) % 2) c.canRet = RetOk(n, 281);
        if (Rand(n, 282) % 3 == 0) c.fpPed = 1;
        break;
    case 16:                                                     // NaN e infinitos repartidos
        c.canRet = RetOk(n, 283); setDmgPed();
        {
            u32* t[] = {&c.intensity, &c.health, &c.progress, &c.coord[0], &c.coord[1], &c.coord[2], &c.dummy[0], &c.dummy[1], &c.mat[0], &c.mat[9], &c.mat[10], &c.mat[11], &c.chase0};
            for (int k = 0; k < 13; ++k) if (Rand(n, 290 + k) % 3 == 0) *t[k] = Rand(n, 310 + k) % 2 ? Pick(kNan, n, 320 + k) : Pick(kInf, n, 340 + k);
        }
        c.petrol = Rand(n, 360) % 2; c.canFire = 1; c.isPlayerRet = RetOk(n, 361);
        break;
    default:                                                     // 17: desnormales y bordes de 0.25, 250 y 10
        c.canRet = RetOk(n, 362); setDmgPed();
        {
            u32* t[] = {&c.intensity, &c.health, &c.progress, &c.coord[0], &c.coord[1], &c.coord[2], &c.dummy[0], &c.dummy[1], &c.dummy[2], &c.mat[9], &c.mat[10], &c.mat[11], &c.mat[0]};
            for (int k = 0; k < 13; ++k) {
                const u32 w = Rand(n, 370 + k) % 5;
                if (w == 0) *t[k] = Pick(kTiny, n, 390 + k); else if (w == 1) *t[k] = Denorm(n, 410 + k); else if (w == 2) *t[k] = Sgn(ExpFloat(n, 430 + k, -126 + int(Rand(n, 450 + k) % 12)), Rand(n, 470 + k) % 2);
            }
        }
        c.petrol = Rand(n, 490) % 2; c.canFire = 1; c.isPlayerRet = RetOk(n, 491);
        if (Rand(n, 492) % 3 == 0) c.vsub = Rand(n, 493) % 2 ? 3u : 4u;
        break;
    }
    c.gate = (c.canRet & 0xFF) ? 1u : 0u;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("dmg_kind", c.dmgKind).U("weapon", c.weapon).U("intensity", c.intensity).A("coord", c.coord, 3);
    j.U("status", c.status).U("vtype", c.vtype).U("vsub", c.vsub).U("model", c.model);
    j.U("law_enf", c.lawEnf).U("tyres_dont", c.tyresDont).U("is_rc", c.isRC).U("petrol", c.petrol);
    j.U("has_driver", c.hasDriver).A("pass", c.pass, 8).U("max_pass", c.maxPass).U("health", c.health);
    j.U("can_ret", c.canRet).U("can_fire", c.canFire).U("progress", c.progress).U("fp_ped", c.fpPed).U("fp_veh", c.fpVeh);
    j.U("is_player_ret", c.isPlayerRet).U("appearance", c.appearance).U("tyre_ret", c.tyreRet).U("rand_seed", c.randSeed);
    j.U("dmg_type", c.dmgType).U("dmg_has_veh", c.dmgHasVeh).U("vd_sub", c.vdSub).U("drv_ped_type", c.drvPedType);
    j.U("in_focus", c.inFocus).U("havoc0", c.havoc0).U("chase0", c.chase0).A("dummy", c.dummy, 3).A("mat", c.mat, 12).U("gate", c.gate);
    return j.End();
}
}

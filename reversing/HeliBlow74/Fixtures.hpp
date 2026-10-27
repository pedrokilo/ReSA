#pragma once
// Fixtures del banco HeliBlow74 (lote 74): CHeli::BlowUpCar (EXE 0x6C6D30, virtual, ranura 41 de la vtable 0x871680), 21504 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CHeli::BlowUpCar  0x6C6D30  thiscall virtual, ECX = this (helicoptero), (CEntity* causante, bool ocultar), `ret 8`   (hook CHeli / BlowUpCar, tipo V)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) para n < 16384 y 1024 casos por cada familia 16..20 despues.
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
#include <utility>
#include <vector>

namespace Hb74Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;    // familias 0..15
constexpr unsigned kExtraFamilies = 5;    // familias 16..20
constexpr unsigned kCases = kBaseCases + kExtraFamilies * 1024;
constexpr unsigned kFamilies = 16 + kExtraFamilies;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto). El helicoptero es un CHeli completo (LiveComparison.hpp lo comprueba con static_assert).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kHeliWin = 0xA18, kMatWin = 0x48, kEntWin = 0x40, kAtomWin = 0x20;
constexpr std::size_t kArena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 kTimeAddr = 0xB7CB84;      // CTimer::m_snTimeInMilliseconds
constexpr u32 kFocusAddr = 0xB7CD74;     // CWorld::PlayerInFocus
constexpr u32 kPlayersAddr = 0xB7CD98;   // CWorld::Players (2 x 0x190)
constexpr u32 kNewsAddr = 0xB7CB8C;      // CWanted::UseNewsHeliInAdditionToPolice
constexpr u32 kAmbAddr = 0x9690A8;       // CCarCtrl::NumAmbulancesOnDuty
constexpr u32 kTruckAddr = 0x9690AC;     // CCarCtrl::NumFireTrucksOnDuty

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 74276u); }
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
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 hide{};          // segundo argumento (dword; el EXE no lo lee)
    u32 dmgKind{};       // causante: 0 nulo, 1 el ped del jugador, 2 el vehiculo del jugador, 3 otra entidad, 4 puntero crudo, 5 el propio helicoptero
    u32 dmgRaw{};
    u32 fppKind{}, fpvKind{};   // retorno de FindPlayerPed / FindPlayerVehicle: 0 la arena del jugador, 1 nulo, 2 otra arena, 3 crudo
    u32 fppRaw{}, fpvRaw{};
    u32 rnd{};           // retorno de rand()
    u32 flags428{};      // dword +0x428 (vehicleFlags bajos: bit 1 ambulancia, bit 2 camion, bit 4 motor, bit 6 luces)
    u32 flags42A{};      // byte +0x42A (bit 5 = bCanBeDamaged)
    u32 flags42D{};      // byte +0x42D (bit 7 = bSirenOrAlarm)
    u32 status{};        // byte +0x36 (tipo en 0..2, estado en 3..7)
    u32 mission{};       // byte +0x3BA
    u32 model{};         // word +0x22
    u32 phys{};          // dword +0x40
    u32 ent1c{};         // dword +0x1C
    u32 b4a8{};          // byte +0x4A8
    u32 b868{};          // byte +0x868
    u32 matPresent{};    // el helicoptero tiene matriz (posicion en +0x30) o no (posicion compacta en +4)
    u32 pos[3]{};        // posicion
    u32 atomic{};        // la devolucion de la llamada de RwFrameForAllObjects: 0 sin atomico, 1 con atomico
    u32 time{};          // CTimer::m_snTimeInMilliseconds
    u32 focus{};         // CWorld::PlayerInFocus (0 o 1)
    u32 havoc[2]{};      // CPlayerInfo::m_nHavocCaused de cada jugador
    u32 chase[2]{};      // CPlayerInfo::m_fCurrentChaseValue de cada jugador
    u32 news{};          // CWanted::UseNewsHeliInAdditionToPolice
    u32 ambulances{}, trucks{};
    u32 gate{};
};

inline float Unit(u32 c, u32 k) { return float(int(Rand(c, k) % 2001u) - 1000) / 1000.0f; }   // [-1, 1]

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.hide = Rand(n, 8);
    c.dmgKind = Rand(n, 10) % 6; c.dmgRaw = (Rand(n, 11) & 0xFFFFFu) | 1u;   // por debajo de 0x400000: nunca cae en una arena
    c.fppKind = Rand(n, 12) % 8 == 0 ? 1 + Rand(n, 13) % 3 : 0; c.fpvKind = Rand(n, 14) % 8 == 0 ? 1 + Rand(n, 15) % 3 : 0;
    c.fppRaw = (Rand(n, 16) & 0xFFFFFu) | 1u; c.fpvRaw = (Rand(n, 17) & 0xFFFFFu) | 1u;
    c.rnd = Rand(n, 18) % 6 == 0 ? Rand(n, 19) & 0x7FFFFFFFu : Rand(n, 19) % 32768u;
    c.flags428 = Rand(n, 20);
    c.flags42A = Rand(n, 21) % 5 == 0 ? (Rand(n, 22) & ~0x20u) : (Rand(n, 22) | 0x20u);
    c.flags42D = Rand(n, 23);
    c.status = Rand(n, 24) % 3 == 0 ? (Rand(n, 25) & 7u) : ((Rand(n, 25) & 7u) | ((Rand(n, 26) % 12u) << 3));
    c.mission = Rand(n, 27) % 4 == 0 ? 0x3Au : Rand(n, 28) & 0xFFu;
    static constexpr u32 models[]{0x1E8u, 0x1F1u, 0x1D1u, 0x1F5u, 0x1D2u, 0x1F4u, 0u, 0xFFFFu, 0x11D1u, 0x81F5u, 0x1E9u, 0x1E7u};
    c.model = Rand(n, 29) % 4 == 0 ? Rand(n, 30) & 0xFFFFu : models[Rand(n, 31) % std::size(models)];
    c.phys = Rand(n, 32); c.ent1c = Rand(n, 33); c.b4a8 = Rand(n, 34) & 0xFFu; c.b868 = Rand(n, 35) & 0xFFu;
    c.matPresent = Rand(n, 36) % 4 != 0;
    for (int i = 0; i < 3; ++i) c.pos[i] = Rand(n, 37 + i) % 5 == 0 ? FloatOf(n, 40 + i, 3u) : FloatOf(n, 40 + i, 0u);
    c.atomic = Rand(n, 43) % 2;
    c.time = Rand(n, 44) % 6 == 0 ? Pick(kStackSeeds, n, 45) : Rand(n, 46);
    c.focus = Rand(n, 47) % 2;
    for (int i = 0; i < 2; ++i) { c.havoc[i] = Rand(n, 48 + i) % 4 == 0 ? 0xFFFFFFF0u + Rand(n, 50 + i) % 16u : Rand(n, 52 + i); c.chase[i] = FloatOf(n, 54 + i, Rand(n, 56 + i) % 4 == 0 ? 3u : 0u); }
    c.news = Rand(n, 58) % 2; c.ambulances = Rand(n, 59) % 4 == 0 ? 0u : Rand(n, 60) % 5u; c.trucks = Rand(n, 61) % 4 == 0 ? 0u : Rand(n, 62) % 5u;
    c.gate = 1;
    auto Main = [&] { c.flags42A |= 0x20u; c.status &= 7u; if (Rand(n, 70) % 2) c.status = 0u; c.mission = 0x3Au; };   // llega a la parte principal
    switch (c.family) {
    case 0:   // llega a la parte principal con todo aleatorio
        Main();
        break;
    case 1:   // bCanBeDamaged: solo el bit 5 del byte +0x42A
        c.flags42A = Rand(n, 100) & 0xFFu;
        if (Rand(n, 101) % 3 == 0) c.flags42A = 0x20u << (Rand(n, 102) % 3);
        break;
    case 2: {  // estado distinto de jugador: mision, modelo RC y mision de estrellarse
        c.flags42A |= 0x20u;
        c.status = ((1u + Rand(n, 100) % 31u) << 3) | (Rand(n, 101) & 7u);
        const u32 t = Rand(n, 102) % 4;
        c.mission = t == 0 ? 0x3Au : t == 1 ? 0x39u : t == 2 ? 0x3Bu : Rand(n, 103) & 0xFFu;
        if (Rand(n, 104) % 3 == 0) c.model = Rand(n, 105) % 2 ? 0x1D1u : 0x1F5u;
        break;
    }
    case 3:   // modelos: noticias (0x1E8), policia, RC y vecinos; el compare es de 16 bits
        Main();
        c.model = models[n / 16 % std::size(models)];
        if (Rand(n, 100) % 4 == 0) c.model = Rand(n, 101) & 0xFFFFu;
        break;
    case 4:   // causante y retornos de FindPlayerPed / FindPlayerVehicle
        Main();
        c.dmgKind = Rand(n, 100) % 6; c.fppKind = Rand(n, 101) % 4; c.fpvKind = Rand(n, 102) % 4;
        if (Rand(n, 103) % 4 == 0) { c.dmgKind = 0; c.fppKind = 1; }   // nulo == nulo
        break;
    case 5:   // la rama de estado de jugador (borra colision y velocidades) frente a la de otros estados con mision 0x3A
        c.flags42A |= 0x20u; c.mission = 0x3Au;
        c.status = Rand(n, 100) % 2 ? (Rand(n, 101) & 7u) : (((1u + Rand(n, 102) % 31u) << 3) | (Rand(n, 101) & 7u));
        c.ent1c = Rand(n, 103) % 3 == 0 ? 0xFFFFFFFFu : c.ent1c;
        break;
    case 6:   // la rama no RC: atomico presente o ausente, nodo del chasis aleatorio
        Main();
        c.model = Rand(n, 100) % 4 == 0 ? (Rand(n, 101) & 0xFFFFu) : models[Rand(n, 102) % 4 + 1] ;
        if (c.model == 0x1D1u || c.model == 0x1F5u) c.model = 0x1F1u;
        c.atomic = n / 16 % 2;
        break;
    case 7:   // banderas del vehiculo: ambulancia, camion, motor, luces y contadores
        Main();
        c.flags428 = Rand(n, 100) % 3 == 0 ? 0xFFFFFFFFu : Rand(n, 101);
        c.ambulances = Rand(n, 102) % 3 == 0 ? 0u : Rand(n, 103) % 3u; c.trucks = Rand(n, 104) % 3 == 0 ? 0u : Rand(n, 105) % 3u;
        c.flags42D = Rand(n, 106) % 3 == 0 ? 0xFFu : Rand(n, 107) & 0xFFu;
        c.b868 = Rand(n, 108) % 3 == 0 ? 0xFFu : Rand(n, 109) & 0xFFu;
        break;
    case 8: {  // posicion con especiales (NaN no canonicos, infinitos, desnormales, ceros con signo), con y sin matriz
        Main();
        for (int i = 0; i < 3; ++i) c.pos[i] = FloatOf(n, 100 + i, 3u);
        c.matPresent = n / 16 % 2;
        break;
    }
    case 9:   // tiempo y marca de destruccion
        Main();
        c.time = Rand(n, 100) % 2 ? Pick(kStackSeeds, n, 101) : Rand(n, 102);
        break;
    case 10:   // mezcla de todo
        break;
    case 11:   // modelos RC con banderas y bombas aleatorias
        Main();
        c.model = Rand(n, 100) % 2 ? 0x1D1u : 0x1F5u;
        c.b4a8 = Rand(n, 101) & 0xFFu;
        break;
    case 12:   // modelo de noticias con la bandera ya apagada / encendida
        Main();
        c.model = 0x1E8u; c.news = Rand(n, 100) % 2;
        break;
    case 13:   // byte +0x4A8: bombas (bits 0..2) y luces forzadas (bits 3..4) con el resto de bits
        Main();
        c.b4a8 = (n / 16) % 256u;
        break;
    case 14:   // bits de +0x42D (sirena) y +0x868
        Main();
        c.flags42D = (n / 16) % 256u; c.b868 = (n / 16 / 4) % 256u;
        break;
    case 15:   // basura en los bytes altos de las banderas (el EXE solo toca bits sueltos)
        Main();
        c.flags428 = Rand(n, 100) | 0x00020006u; c.phys = 0xFFFFFFFFu; c.ent1c = 0xFFFFFFFFu;
        break;
    case 16:   // jugador causante: contadores del jugador con valores extremos del flotante y PlayerInFocus 0 y 1
        Main();
        c.dmgKind = 1 + Rand(n, 100) % 2; c.fppKind = c.dmgKind == 1 ? 0 : 1 + Rand(n, 107) % 2; c.fpvKind = c.dmgKind == 2 ? 0 : 1 + Rand(n, 108) % 2;
        for (int i = 0; i < 2; ++i) { c.chase[i] = Pick(kNan, n, 101 + i); if (Rand(n, 103 + i) % 2) c.chase[i] = Pick(kTiny, n, 105 + i); }
        if (Rand(n, 109) % 3 == 0) { c.chase[c.focus] = Pick(kInf, n, 110); }
        if (Rand(n, 111) % 3 == 0) c.chase[c.focus] = 0xC1200000u;   // -10.0f + 10.0f = +0
        break;
    case 17:   // rand(): valores extremos, incluso negativos
        Main();
        c.dmgKind = 1; c.fppKind = 0;
        c.rnd = Pick(kStackSeeds, n, 100);
        if (Rand(n, 101) % 2) c.rnd = Rand(n, 102);
        break;
    case 18:   // contadores de ambulancias y camiones en el limite (0 da la vuelta) y banderas activas
        Main();
        c.flags428 |= 6u;
        c.ambulances = Rand(n, 100) % 2 ? 0u : 1u; c.trucks = Rand(n, 101) % 2 ? 0u : 1u;
        break;
    case 19:   // el segundo argumento y todo el relleno aleatorios; mision de estrellarse con estado de jugador
        c.hide = Rand(n, 100); c.flags42A |= 0x20u; c.status = Rand(n, 101) & 7u; c.mission = Rand(n, 102) & 0xFFu;
        break;
    default:   // 20: mezcla de los extremos
        Main();
        c.dmgKind = Rand(n, 100) % 6; c.rnd = Rand(n, 101); c.chase[c.focus] = FloatOf(n, 102, 3u);
        c.pos[Rand(n, 103) % 3] = FloatOf(n, 104, 3u);
        c.time = Rand(n, 105);
        break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack).U("hide", c.hide);
    j.U("dmg_kind", c.dmgKind).U("dmg_raw", c.dmgRaw).U("fpp_kind", c.fppKind).U("fpv_kind", c.fpvKind).U("fpp_raw", c.fppRaw).U("fpv_raw", c.fpvRaw).U("rnd", c.rnd);
    j.U("flags428", c.flags428).U("flags42a", c.flags42A).U("flags42d", c.flags42D).U("status", c.status).U("mission", c.mission).U("model", c.model).U("phys", c.phys).U("ent1c", c.ent1c);
    j.U("b4a8", c.b4a8).U("b868", c.b868).U("mat_present", c.matPresent).U("pos_x", c.pos[0]).U("pos_y", c.pos[1]).U("pos_z", c.pos[2]).U("atomic", c.atomic).U("time", c.time).U("focus", c.focus);
    j.U("havoc0", c.havoc[0]).U("havoc1", c.havoc[1]).U("chase0", c.chase[0]).U("chase1", c.chase[1]).U("news", c.news).U("ambulances", c.ambulances).U("trucks", c.trucks).U("gate", c.gate);
    return j.End();
}
}

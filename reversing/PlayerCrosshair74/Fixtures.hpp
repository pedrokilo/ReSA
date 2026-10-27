#pragma once
// Fixtures del banco HeatFire73 (lote 73): CVehicle::FireHeatSeakingMissile (EXE 0x6E05C0), 16384 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CVehicle::FireHeatSeakingMissile  0x6E05C0  thiscall, ECX = this (vehiculo), (CEntity* objetivo, eOrdnanceType tipo, bool arg2), `ret 0Ch`   (hook CVehicle / FireHeatSeakingMissile, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia).
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

namespace Pc74Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kFamilies = 20;
constexpr unsigned kCases = kFamilies * 1024;    // 20480 pares
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kInfoWin = 0x190, kPadWin = 0x40, kPedWin = 0x480, kMatWin = 0x48;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kInfoArena = Arena(kInfoWin), kPadArena = Arena(kPadWin), kPedArena = Arena(kPedWin), kMatArena = Arena(kMatWin);
constexpr std::size_t kCrossOff = 0x158;   // CPlayerInfo::m_nCrosshairActivated (LiveComparison.hpp lo comprueba con offsetof)

constexpr u32 kTimeAddr = 0xB7CB84;   // CTimer::m_snTimeInMilliseconds
constexpr u32 kStepAddr = 0xB7CB5C;   // CTimer::ms_fTimeStep
constexpr u32 kInvertAddr = 0xB73402; // CPad::bInvertLook4Pad
constexpr u32 kAspectAddr = 0xC3EFA4; // CDraw::ms_fAspectRatio
constexpr u32 kCamBase = 0xB6F028;    // TheCamera
constexpr u32 kCamMatrix = 0xB6F99C;  // TheCamera.m_mCameraMatrix
constexpr u32 kPlayers = 0xB7CD98;    // CWorld::Players

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
// Solo patrones de bits. Los getters del mando devuelven un DWORD cuyo ax es el valor (el EXE hace movsx / test ax,ax); la mitad alta es basura.
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 player{};        // indice de jugador (0 o 1)
    u32 active{};        // DWORD m_nCrosshairActivated (el EXE solo mira el byte bajo)
    u32 tx{}, ty{};      // m_vecCrosshairTarget (+4, +8)
    u32 ts{};            // CTimer::ms_fTimeStep
    u32 inv{};           // byte CPad::bInvertLook4Pad
    u32 lr{}, ud{}, gun{};   // retornos de GetSteeringLeftRight / GetSteeringUpDown / GetCarGunFired
    u32 fov1{}, fov2{}, asp{};
    u32 mat[12]{};       // TheCamera.m_mCameraMatrix: derecha, delante, arriba, posicion
    u32 hasCamMat{};     // TheCamera.m_matrix != nullptr
    u32 cpos[3]{};       // posicion de la matriz a la que apunta m_matrix (+0x30)
    u32 ppos[3]{};       // posicion compacta de TheCamera (+4)
    u32 flags{};         // DWORD +0x470 del ped
    u32 fireXor{};       // lo que el falso de FireInstantHit invierte en ese DWORD
    u32 fireRet{};
};

inline u32 Steer16(u32 n, u32 k) {
    static constexpr std::array<u32, 12> sp{0u, 1u, 0xFFFFu, 0x7FFFu, 0x8000u, 0x8001u, 2u, 0xFFFEu, 100u, 0xFF9Cu, 0x4000u, 0xC000u};
    const u32 lo = Rand(n, k) % 3 == 0 ? sp[Rand(n, k + 1) % sp.size()] : (Rand(n, k + 2) & 0xFFFFu);
    return Rand(n, k + 3) % 2 ? lo : ((Rand(n, k + 4) & 0xFFFF0000u) | lo);
}
inline u32 StepOf(u32 n, u32 k, u32 mode) {   // paso del temporizador
    switch (mode) {
    case 0: return Rand(n, k) % 4 == 0 ? B(1.0f) : RandFloat(n, k + 1, -6, 2);
    case 1: return Pick(kUnit, n, k + 1);
    default: return FloatOf(n, k + 1, 3u);
    }
}
// Valor de la mira: moderado, de borde (+-0.9f y vecinos), o especial
inline u32 CrossOf(u32 n, u32 k, u32 mode) {
    static constexpr std::array<u32, 14> edge{0x3F666666u, 0x3F666667u, 0x3F666665u, 0xBF666666u, 0xBF666667u, 0xBF666665u, 0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x3F666668u, 0xBF666664u};
    switch (mode) {
    case 0: return RandFloat(n, k, -8, 0);
    case 1: return Pick(edge, n, k);
    case 2: return FloatOf(n, k, 3u);
    default: return Rand(n, k + 9) % 4 == 0 ? Pick(edge, n, k) : RandFloat(n, k, -10, 1);
    }
}
// Un angulo/FOV: evita |fov * 0.0087| >= 2^63 (fptan fuera de rango deja la pila x87 del propio EXE desbalanceada)
inline u32 FovOf(u32 n, u32 k, u32 mode) {
    static constexpr std::array<u32, 14> sp{0u, 0x80000000u, 0x42B40000u, 0x43340000u, 0x3F800000u, 0xBF800000u, 1u, 0x80000001u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0x7F800000u, 0xFF800000u, 0x43B40000u};
    switch (mode) {
    case 0: return RandFloat(n, k, 3, 7);
    case 1: return Pick(sp, n, k);
    case 2: return RandFloat(n, k, -30, 30);
    default: return Rand(n, k + 9) % 3 == 0 ? Pick(sp, n, k) : RandFloat(n, k, 2, 8);
    }
}
inline u32 AspOf(u32 n, u32 k, u32 mode) {
    static constexpr std::array<u32, 12> sp{0u, 0x80000000u, 0x3F800000u, 0x3FE38E39u, 0x40000000u, 0x7FC00000u, 0x7F800000u, 0xFF800000u, 1u, 0xBF800000u, 0x7F800001u, 0x7F7FFFFFu};
    switch (mode) {
    case 0: return Rand(n, k) % 3 == 0 ? B(4.0f / 3.0f) : RandFloat(n, k + 1, -1, 2);
    case 1: return Pick(sp, n, k);
    default: return Rand(n, k + 9) % 3 == 0 ? Pick(sp, n, k) : RandFloat(n, k, -1, 2);
    }
}

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n % kFamilies;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.player = Rand(n, 3) % 2;
    c.active = (Rand(n, 4) & 0xFFFFFF00u) | (Rand(n, 5) % 3 == 0 ? (1u + Rand(n, 6) % 255u) : 1u);
    c.tx = CrossOf(n, 10, 0); c.ty = CrossOf(n, 11, 0);
    c.ts = StepOf(n, 12, 0);
    c.inv = Rand(n, 13) % 2 ? 1u : Rand(n, 14) % 2 ? 0u : (Rand(n, 15) & 0xFFu);
    c.lr = Steer16(n, 20); c.ud = Steer16(n, 30);
    c.gun = (Rand(n, 40) % 2 ? 0u : (Rand(n, 41) & 0xFFFF0000u)) | (Rand(n, 42) % 3 ? (1u + Rand(n, 43) % 2u) : 0u);   // mitad sin disparo
    c.fov1 = FovOf(n, 50, 0); c.fov2 = Rand(n, 51) % 3 == 0 ? c.fov1 : FovOf(n, 52, 0);
    c.asp = AspOf(n, 53, 0);
    for (int i = 0; i < 12; ++i) c.mat[i] = RandFloat(n, 60 + i, -6, 6);
    c.hasCamMat = Rand(n, 80) % 3 != 0;
    for (int i = 0; i < 3; ++i) { c.cpos[i] = RandFloat(n, 81 + i, -4, 10); c.ppos[i] = RandFloat(n, 84 + i, -4, 10); }
    c.flags = Rand(n, 90);
    c.fireXor = Rand(n, 91) % 2 ? 0u : (Rand(n, 92) % 2 ? 0x02000000u : Rand(n, 93));
    c.fireRet = Rand(n, 94);
    const auto fire = [&] { c.gun = ((Rand(n, 100) & 0xFFFF0000u) * (Rand(n, 101) % 2)) | (1u + Rand(n, 102) % 2u); };
    const auto nofire = [&] { c.gun = Rand(n, 103) % 2 ? 0u : (Rand(n, 104) & 0xFFFF0000u); };
    const auto matAll = [&](u32 mode) { for (int i = 0; i < 12; ++i) c.mat[i] = FloatOf(n, 110 + i, mode); };
    switch (c.family) {
    case 0:  // inactiva: el byte bajo de m_nCrosshairActivated es cero (basura arriba); no debe pasar nada
        c.active = Rand(n, 120) & 0xFFFFFF00u; if (Rand(n, 121) % 4 == 0) c.active = 0;
        break;
    case 1:  // activa, sin disparo: paso y recorte moderados
        nofire();
        break;
    case 2:  // sin disparo: palancas extremas y pasos especiales
        nofire(); c.ts = StepOf(n, 120, Rand(n, 121) % 3); c.tx = CrossOf(n, 122, 0); c.ty = CrossOf(n, 123, 3);
        break;
    case 3:  // sin disparo: bordes del recorte y valores especiales de la mira (palanca a cero)
        nofire(); c.tx = CrossOf(n, 120, Rand(n, 121) % 3); c.ty = CrossOf(n, 122, Rand(n, 123) % 3);
        if (Rand(n, 124) % 2) { c.lr = Rand(n, 125) & 0xFFFF0000u; c.ud = Rand(n, 126) & 0xFFFF0000u; }
        break;
    case 4:  // sin disparo: byte de inversion (0, 1, otros) con palanca vertical
        nofire(); c.inv = Rand(n, 120) % 4 == 0 ? 0u : Rand(n, 121) % 2 ? 1u : (Rand(n, 122) & 0xFFu); c.ts = Rand(n, 123) % 2 ? B(1.0f) : StepOf(n, 124, 0);
        break;
    case 5:  // disparo normal
        fire(); c.fov1 = FovOf(n, 120, 0); c.fov2 = Rand(n, 121) % 2 ? c.fov1 : FovOf(n, 122, 0);
        break;
    case 6:  // disparo: FOV especiales
        fire(); c.fov1 = FovOf(n, 120, Rand(n, 121) % 2 ? 1u : 3u); c.fov2 = FovOf(n, 122, Rand(n, 123) % 2 ? 1u : 3u);
        break;
    case 7:  // disparo: aspecto especial
        fire(); c.asp = AspOf(n, 120, Rand(n, 121) % 4 ? 1u : 2u);
        break;
    case 8:  // disparo: matriz de la camara especial
        fire(); matAll(Rand(n, 120) % 2 ? 3u : 5u);
        break;
    case 9:  // disparo: mira especial (tras recortar solo sobreviven los valores en rango y NaN) y palanca a cero
        fire(); c.tx = CrossOf(n, 120, 2); c.ty = CrossOf(n, 121, 2); c.lr = Rand(n, 122) & 0xFFFF0000u; c.ud = Rand(n, 123) & 0xFFFF0000u;
        break;
    case 10: {  // disparo: banderas del ped y lo que invierte FireInstantHit (bit 25 y otros)
        fire(); c.flags = Rand(n, 120) % 3 == 0 ? 0u : Rand(n, 120) % 3 == 1 ? 0xFFFFFFFFu : Rand(n, 121);
        static constexpr std::array<u32, 8> xs{0u, 0x02000000u, 0xFFFFFFFFu, 0x01000000u, 0x04000000u, 0x00000001u, 0xFDFFFFFFu, 0x12345678u};
        c.fireXor = Pick(xs, n, 122);
        break;
    }
    case 11:  // disparo: posicion de la camara (matriz o compacta) con valores especiales
        fire(); c.hasCamMat = Rand(n, 120) % 2;
        for (int i = 0; i < 3; ++i) { c.cpos[i] = FloatOf(n, 121 + i, 3u); c.ppos[i] = FloatOf(n, 124 + i, 3u); c.mat[9 + i] = FloatOf(n, 127 + i, Rand(n, 130 + i) % 2 ? 3u : 0u); }
        break;
    case 12:  // disparo: mezcla de especiales en todo
        fire(); c.fov1 = FovOf(n, 120, 3); c.fov2 = FovOf(n, 121, 3); c.asp = AspOf(n, 122, 2); matAll(5u); c.tx = CrossOf(n, 123, 3); c.ty = CrossOf(n, 124, 3); c.ts = StepOf(n, 125, 2);
        for (int i = 0; i < 3; ++i) { c.cpos[i] = FloatOf(n, 130 + i, 5u); c.ppos[i] = FloatOf(n, 133 + i, 5u); }
        break;
    case 13:  // disparo: magnitudes diminutas (productos desnormales / de exponente extendido)
        fire(); matAll(1u); c.tx = RandFloat(n, 120, -60, -20); c.ty = RandFloat(n, 121, -60, -20); c.lr = Rand(n, 122) & 0xFFFF0000u; c.ud = Rand(n, 123) & 0xFFFF0000u; c.asp = AspOf(n, 124, 0);
        for (int i = 0; i < 3; ++i) { c.cpos[i] = FloatOf(n, 130 + i, 1u); c.ppos[i] = FloatOf(n, 133 + i, 1u); c.mat[9 + i] = FloatOf(n, 136 + i, 1u); }
        break;
    case 14:  // disparo: magnitudes grandes (desbordamientos)
        fire(); matAll(2u); c.asp = Rand(n, 120) % 2 ? B(1.0e-20f) : AspOf(n, 121, 0); c.fov1 = RandFloat(n, 122, 3, 8);
        for (int i = 0; i < 3; ++i) { c.cpos[i] = FloatOf(n, 130 + i, 2u); c.ppos[i] = FloatOf(n, 133 + i, 2u); }
        break;
    case 15: {  // el valor de AX de GetCarGunFired: 1, 2, 0x8000, 0xFFFF, y cero con la mitad alta distinta de cero
        static constexpr std::array<u32, 10> gs{1u, 2u, 0x8000u, 0xFFFFu, 0x10000u, 0xFFFF0000u, 0xDEAD0001u, 0xBEEF0000u, 0x12348000u, 0u};
        c.gun = Pick(gs, n, 120);
        break;
    }
    case 16:  // jugador 1 y 0 con registros y pila sembrados distintos; disparo
        fire(); c.player = Rand(n, 120) % 2;
        break;
    case 17:  // ceros con signo: la mira en cero, matriz con ceros y menos ceros, fov2 distinto de fov1 (cancelaciones exactas)
        fire(); c.tx = Rand(n, 120) % 2 ? 0u : 0x80000000u; c.ty = Rand(n, 121) % 2 ? 0u : 0x80000000u; c.lr = Rand(n, 122) & 0xFFFF0000u; c.ud = Rand(n, 123) & 0xFFFF0000u;
        for (int i = 0; i < 12; ++i) c.mat[i] = Rand(n, 124 + i) % 3 == 0 ? (Rand(n, 140 + i) % 2 ? 0u : 0x80000000u) : RandFloat(n, 150 + i, -3, 3);
        c.fov2 = FovOf(n, 170, 0);
        break;
    case 18: {  // sin disparo: la palanca lleva la mira justo sobre el borde del recorte (paso 1.0, palanca +-1/+-3000)
        static constexpr std::array<u32, 6> ed{0x3F666666u, 0x3F666665u, 0xBF666666u, 0xBF666665u, 0x3F666664u, 0xBF666667u};
        static constexpr std::array<u32, 6> st{1u, 0xFFFFu, 3000u, 0xF448u, 2u, 0xFFFEu};
        nofire(); c.ts = B(1.0f); c.tx = Pick(ed, n, 120); c.ty = Pick(ed, n, 121); c.lr = Pick(st, n, 122); c.ud = Pick(st, n, 123);
        break;
    }
    default:  // 19: disparo con indice de jugador 1, banderas con el bit 25 puesto y quitado por el falso
        fire(); c.player = 1; c.flags = Rand(n, 120) | (Rand(n, 121) % 2 ? 0x02000000u : 0u); c.fireXor = Rand(n, 122) % 2 ? 0x02000000u : 0u;
        break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("player", c.player).U("active", c.active).U("tx", c.tx).U("ty", c.ty).U("ts", c.ts).U("inv", c.inv).U("lr", c.lr).U("ud", c.ud).U("gun", c.gun);
    j.U("fov1", c.fov1).U("fov2", c.fov2).U("asp", c.asp).U("has_cam_mat", c.hasCamMat).U("flags", c.flags).U("fire_xor", c.fireXor).U("fire_ret", c.fireRet);
    static const char* const nm[12]{"rx", "ry", "rz", "fx", "fy", "fz", "ux", "uy", "uz", "px", "py", "pz"};
    for (int i = 0; i < 12; ++i) j.U(nm[i], c.mat[i]);
    j.U("cpos_x", c.cpos[0]).U("cpos_y", c.cpos[1]).U("cpos_z", c.cpos[2]).U("ppos_x", c.ppos[0]).U("ppos_y", c.ppos[1]).U("ppos_z", c.ppos[2]);
    return j.End();
}
}

#pragma once
// Fixtures del banco HeliFly75 (lote 75): CHeli::ProcessFlyingCarStuff (EXE 0x6C4E60, virtual, ranura 68 de la vtable 0x871680), 21504 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CHeli::ProcessFlyingCarStuff  0x6C4E60  thiscall virtual, ECX = this (helicoptero), sin argumentos, void   (hook CHeli / ProcessFlyingCarStuff, tipo V)
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

namespace Hf75Fix {
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
constexpr std::size_t kHeliWin = 0xA18, kMatWin = 0x48, kFrameWin = 0x60, kAtomWin = 0x60, kHandWin = 0x100;
constexpr std::size_t kArena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 kStepAddr = 0xB7CB5C;      // CTimer::ms_fTimeStep
constexpr u32 kCamMatPtrAddr = 0xB6F03C; // TheCamera.m_matrix (CPlaceable +0x14)
constexpr u32 kCamPosAddr = 0xB6F02C;    // TheCamera.m_placement.m_vPosn (CPlaceable +4)

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 75276u); }
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
inline u32 Denorm(u32 c, u32 k) { const u32 r = Rand(c, k); return ((r >> 31) << 31) | ((Rand(c, k + 500) & 0x7FFFFFu) | 1u); }
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
inline float Unit(u32 c, u32 k) { return float(int(Rand(c, k) % 2001u) - 1000) / 1000.0f; }   // [-1, 1]
// Un float cercano a un umbral (el umbral y sus vecinos de 1 y 2 ulp, con y sin signo) o un especial
inline u32 Near(u32 c, u32 k, float threshold) {
    const u32 b = B(threshold);
    switch (Rand(c, k) % 9) {
    case 0: return b;
    case 1: return b + 1;
    case 2: return b - 1;
    case 3: return b + 2;
    case 4: return b - 2;
    case 5: return b | 0x80000000u;
    case 6: return (b + 1) | 0x80000000u;
    case 7: return Pick(kTiny, c, k + 1);
    default: return B(threshold * (0.5f + float(Rand(c, k + 2) % 1000u) / 500.0f));
    }
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 status{};        // byte +0x36 (tipo en 0..2, estado en 3..7)
    u32 model{};         // word +0x22
    u32 flags40{};       // dword +0x40
    u32 b428{};          // byte +0x428 (bit 4 = motor)
    u32 b42c{};          // byte +0x42C (bit 7)
    u32 b960{};          // byte +0x960
    u32 bB8{};           // byte +0xB8
    u32 handFlags{};     // dword en +0xCC del manejo (bit 25 = 0x2000000)
    u32 rotor{};         // float +0x84C
    u32 ctl[4]{};        // floats +0x98C, +0x990, +0x994, +0x998
    u32 vel[3]{};        // floats +0x44, +0x48, +0x4C
    u32 prop{}, wheelRot{};   // floats +0x958 y +0x82C
    u32 nodePresent{};   // m_aCarNodes[12] (+0x678)
    u32 frame[16]{};     // matriz de modelado del nodo (RwMatrix, 16 palabras)
    u32 matPresent{};    // el helicoptero tiene matriz (posicion en +0x30) o no (posicion compacta en +4)
    u32 pos[3]{};        // posicion del helicoptero
    u32 mat[12]{};       // CMatrix del helicoptero: right (0..2), forward (4..6), up (8..10) en palabras; (3, 7, 11 son relleno)
    u32 atomic{};        // el resultado de RwFrameForAllObjects: 0 sin atomico, 1 con atomico
    u32 atomFlags{};     // byte +0x4C del atomico (bit 1 = esfera sucia)
    u32 radius{};        // float +0x28 del atomico
    u32 radiusAfter{};   // el valor que deja _rpAtomicResyncInterpolatedSphere
    u32 camMat{};        // la camara tiene matriz (posicion en +0x30) o no (posicion compacta en +4)
    u32 camPos[3]{};     // posicion de la camara
    u32 step{};          // CTimer::ms_fTimeStep
    u32 gate{};
};

inline u32 Sq(float x) { return B(x * x); }

// Direccion unitaria aleatoria (en doble) y su version en float
inline void UnitVec(u32 c, u32 k, double out[3]) {
    double x = Unit(c, k), y = Unit(c, k + 1), z = Unit(c, k + 2);
    double m = std::sqrt(x * x + y * y + z * z);
    if (m < 1e-3) { x = 0.0; y = 0.0; z = 1.0; m = 1.0; }
    out[0] = x / m; out[1] = y / m; out[2] = z / m;
}

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    // ---- estado: jugador (0), remoto (8), destruido (3) o cualquier otro
    static constexpr u32 statuses[]{0u, 8u, 3u, 0u, 8u, 3u, 1u, 2u, 4u, 5u, 6u, 7u, 9u, 10u, 11u, 12u, 31u};
    c.status = (statuses[Rand(n, 10) % std::size(statuses)] << 3) | (Rand(n, 11) & 7u);
    static constexpr u32 models[]{0x1D1u, 0x1F5u, 0x1D5u, 0x1BFu, 0x1A9u, 0x1A1u, 0x1E7u, 0x1E8u, 0x1F1u, 0x1D2u, 0x1F4u, 0u, 0xFFFFu, 0x11D1u, 0x81F5u};
    c.model = Rand(n, 12) % 5 == 0 ? Rand(n, 13) & 0xFFFFu : models[Rand(n, 14) % std::size(models)];
    c.flags40 = Rand(n, 15);
    if (Rand(n, 16) % 2) c.flags40 &= ~0x100u;
    if (Rand(n, 17) % 2) c.flags40 |= 0x8000000u; else c.flags40 &= ~0x8000000u;
    c.b428 = Rand(n, 18) & 0xFFu; c.b42c = Rand(n, 19) & 0xFFu; c.b960 = Rand(n, 20) % 3 == 0 ? Rand(n, 21) & 0xFFu : Rand(n, 21) % 7u; c.bB8 = Rand(n, 22) & 0xFFu;
    c.handFlags = Rand(n, 23) % 3 == 0 ? Rand(n, 24) & ~0x2000000u : Rand(n, 24) | 0x2000000u;
    // ---- rotor: umbrales del EXE 0.22, 0.15, 0.015, 0.0075, 0.154, 0.0044 y 0
    static constexpr float thr[]{0.22f, 0.15f, 0.015f, 0.0075f, 0.154f, 0.0044f};
    switch (Rand(n, 25) % 8) {
    case 0: c.rotor = Near(n, 26, thr[Rand(n, 27) % 6]); break;
    case 1: c.rotor = B(float(Rand(n, 28) % 3000u) / 10000.0f); break;                // 0 .. 0.3
    case 2: c.rotor = B(float(Rand(n, 28) % 400u) / 10000.0f); break;                 // 0 .. 0.04
    case 3: c.rotor = B(0.0f); break;
    case 4: c.rotor = FloatOf(n, 29, Rand(n, 30) % 4 == 0 ? 3u : 1u); break;
    case 5: c.rotor = B(-float(Rand(n, 28) % 3000u) / 10000.0f); break;
    case 6: c.rotor = B(0.154f * float(Rand(n, 28) % 1000u) / 1000.0f); break;
    default: c.rotor = B(0.22f * float(Rand(n, 28) % 1000u) / 1000.0f); break;
    }
    for (int i = 0; i < 4; ++i) c.ctl[i] = Rand(n, 31 + i) % 6 == 0 ? FloatOf(n, 35 + i, 3u) : FloatOf(n, 35 + i, 0u);
    static constexpr float velThr = 0.02f;
    for (int i = 0; i < 3; ++i) c.vel[i] = Rand(n, 40 + i) % 4 == 0 ? Near(n, 43 + i, velThr) : B(Unit(n, 46 + i) * 0.05f);
    c.step = Rand(n, 50) % 6 == 0 ? FloatOf(n, 51, 4u) : B(0.5f + float(Rand(n, 52) % 3000u) / 1000.0f);
    // ---- nodo, atomico y radio
    c.nodePresent = Rand(n, 53) % 6 != 0;
    for (int i = 0; i < 16; ++i) c.frame[i] = B(Unit(n, 54 + i) * 10.0f);
    c.atomic = Rand(n, 70) % 5 != 0;
    c.atomFlags = Rand(n, 71) & 0xFFu;
    c.radius = Rand(n, 72) % 3 == 0 ? Near(n, 73, 0.1f) : B(float(Rand(n, 74) % 3000u) / 1000.0f);
    c.radiusAfter = Rand(n, 75) % 3 == 0 ? Near(n, 76, 0.1f) : B(float(Rand(n, 77) % 3000u) / 1000.0f);
    // ---- posiciones y matrices
    c.matPresent = Rand(n, 78) % 4 != 0;
    double pos[3]; for (int i = 0; i < 3; ++i) { c.pos[i] = B(Unit(n, 79 + i) * 200.0f); pos[i] = F(c.pos[i]); }
    c.camMat = Rand(n, 82) % 2;
    const double radius = 1.0 + (Rand(n, 83) % 4 == 0 ? double(Rand(n, 84) % 40u) : 19.0 + double(Rand(n, 84) % 200u) / 100.0);   // alrededor de 20 (dist2 = 400)
    double dir[3]; UnitVec(n, 85, dir);
    for (int i = 0; i < 3; ++i) c.camPos[i] = B(float(pos[i] + dir[i] * radius));
    // matriz del helicoptero
    switch (Rand(n, 88) % 4) {
    case 0: case 1: {   // identidad con una perturbacion pequena
        const float eps = Rand(n, 89) % 2 ? 0.0f : 0.01f;
        const float id[12]{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};
        for (int i = 0; i < 12; ++i) c.mat[i] = B(id[i] + eps * Unit(n, 90 + i));
        break;
    }
    case 2:
        for (int i = 0; i < 12; ++i) c.mat[i] = B(Unit(n, 90 + i));
        break;
    default:
        for (int i = 0; i < 12; ++i) c.mat[i] = B(Unit(n, 90 + i) * 2.0f);
        break;
    }
    // vector derecho del nodo: alineado con la direccion hacia la camara, o aleatorio
    double axis[3];
    if (Rand(n, 105) % 2) {   // r = M * axis ~ d  (con la identidad, axis ~ d); con signo
        const double sg = Rand(n, 106) % 2 ? 1.0 : -1.0;
        const double noise = Rand(n, 107) % 3 == 0 ? 0.2 : 0.01;
        for (int i = 0; i < 3; ++i) axis[i] = -dir[i] * sg + noise * Unit(n, 108 + i);   // d = camara - helicoptero = +dir*radius; la direccion normalizada es dir; se usa -dir y el signo
    } else {
        UnitVec(n, 110, axis);
    }
    if (Rand(n, 113) % 2) for (int i = 0; i < 3; ++i) axis[i] = -axis[i];
    c.frame[0] = B(float(axis[0])); c.frame[1] = B(float(axis[1])); c.frame[2] = B(float(axis[2]));
    // diferencia de rotacion: alrededor de 0.5236
    const float diff = Rand(n, 114) % 3 == 0 ? 0.5236f * (0.9f + float(Rand(n, 115) % 200u) / 1000.0f) : float(Rand(n, 115) % 4000u) / 1000.0f;
    c.wheelRot = B(Unit(n, 116) * 3.0f);
    c.prop = B(F(c.wheelRot) + (Rand(n, 117) % 2 ? diff : -diff));
    c.gate = 1;

    auto NoMatrixSafe = [&] {   // sin matriz de helicoptero no se debe llegar a Multiply3x3 (el EXE desreferenciaria el puntero nulo): la rotacion de las aspas no cambia
        if (!c.matPresent) c.prop = c.wheelRot;
    };
    auto Alive = [&] {   // estados y velocidades que llegan a las ramas principales
        static constexpr u32 st[]{0u, 8u, 3u};
        c.status = (st[Rand(n, 120) % 3] << 3) | (Rand(n, 121) & 7u);
    };
    switch (c.family) {
    case 0:   // estados distintos de jugador/remoto/destruido: bandera del manejo, motor y freno del rotor (f < rotor, f >= rotor)
        c.status = ((1u + Rand(n, 130) % 31u) << 3) | (Rand(n, 131) & 7u);
        if ((c.status >> 3) == 8u || (c.status >> 3) == 3u) c.status = (1u << 3) | (c.status & 7u);
        c.rotor = Rand(n, 132) % 3 == 0 ? Near(n, 133, 0.0005499999970197678f * F(c.step)) : B(float(Rand(n, 134) % 4000u) / 10000.0f);
        break;
    case 1:   // jugador: acelerar el rotor y las llamadas a FlyingControl
        Alive();
        c.rotor = Rand(n, 130) % 2 ? Near(n, 131, Rand(n, 132) % 2 ? 0.22f : 0.15f) : B(float(Rand(n, 133) % 3000u) / 10000.0f);
        break;
    case 2:   // el cuerpo del rotor: nodo, atomico, radio y escala por modelo
        Alive();
        c.rotor = B(0.015f + float(Rand(n, 130) % 3000u) / 10000.0f);
        c.nodePresent = 1;
        c.model = models[Rand(n, 131) % 8];
        break;
    case 3:   // viento: estado 0/3/8 y rotor junto a 0.0075 y 0.015
        Alive();
        c.rotor = Rand(n, 130) % 2 ? Near(n, 131, 0.0075f) : Near(n, 132, 0.015f);
        c.nodePresent = 1; c.atomic = Rand(n, 133) % 2;
        break;
    case 4:   // sonido de aspas: rotor entre 0.0044 y 0.154, camara, distancia y diferencia de rotacion
        c.rotor = B(0.0044f + float(Rand(n, 130) % 1500u) / 10000.0f);
        c.model = models[2 + Rand(n, 131) % 9];
        c.nodePresent = 1;
        break;
    case 5:   // la alineacion llega al evento de audio
        c.rotor = B(0.0044f + float(Rand(n, 130) % 1500u) / 10000.0f);
        c.model = models[2 + Rand(n, 131) % 9];
        c.nodePresent = 1; c.matPresent = 1;
        c.prop = B(F(c.wheelRot) + 1.0f);
        break;
    case 6:   // matriz del helicoptero y vector del nodo con especiales
        c.rotor = B(0.0044f + float(Rand(n, 130) % 1500u) / 10000.0f);
        c.model = models[2 + Rand(n, 131) % 9];
        c.nodePresent = 1; c.matPresent = 1; c.prop = B(F(c.wheelRot) + 1.0f);
        for (int i = 0; i < 12; ++i) if (Rand(n, 140 + i) % 4 == 0) c.mat[i] = FloatOf(n, 160 + i, 3u);
        for (int i = 0; i < 3; ++i) if (Rand(n, 175 + i) % 3 == 0) c.frame[i] = FloatOf(n, 178 + i, 3u);
        break;
    case 7:   // posiciones con especiales: helicoptero y camara, con y sin matriz
        c.rotor = B(0.0044f + float(Rand(n, 130) % 1500u) / 10000.0f);
        c.model = models[2 + Rand(n, 131) % 9];
        c.nodePresent = 1;
        for (int i = 0; i < 3; ++i) { if (Rand(n, 140 + i) % 3 == 0) c.pos[i] = FloatOf(n, 143 + i, 3u); if (Rand(n, 147 + i) % 3 == 0) c.camPos[i] = FloatOf(n, 150 + i, 3u); }
        break;
    case 8:   // rotor y paso con especiales en todos los estados
        c.rotor = FloatOf(n, 130, 3u);
        c.step = FloatOf(n, 131, Rand(n, 132) % 2 ? 3u : 1u);
        break;
    case 9:   // freno del rotor: f cerca del rotor, resultado desnormal
        c.status = ((1u + Rand(n, 130) % 2u) << 3) | (Rand(n, 131) & 7u);
        c.handFlags |= 0x2000000u;
        c.step = Rand(n, 132) % 2 ? Denorm(n, 133) : B(float(Rand(n, 134) % 1000u) / 1000.0f);
        c.rotor = Rand(n, 135) % 2 ? Denorm(n, 136) : B(0.0005499999970197678f * F(c.step) * (0.5f + float(Rand(n, 137) % 1000u) / 500.0f));
        break;
    case 10:   // mezcla de todo
        break;
    case 11:   // argumentos de FlyingControl con especiales y velocidades en el umbral
        Alive();
        c.rotor = B(0.15f + float(Rand(n, 130) % 1000u) / 5000.0f);
        for (int i = 0; i < 4; ++i) c.ctl[i] = FloatOf(n, 131 + i, 5u);
        for (int i = 0; i < 3; ++i) c.vel[i] = Rand(n, 140 + i) % 2 ? FloatOf(n, 143 + i, 3u) : Near(n, 146 + i, velThr);
        c.b42c &= 0x7Fu;
        c.b960 = 4u + Rand(n, 150) % 3u;
        break;
    case 12:   // rotor exactamente en los umbrales
        c.rotor = Near(n, 130, thr[(n / 16) % 6]);
        if (Rand(n, 131) % 2) Alive();
        break;
    case 13:   // radio del atomico: umbral 0.1 y especiales, banderas del atomico
        Alive();
        c.rotor = B(0.015f + float(Rand(n, 130) % 3000u) / 10000.0f);
        c.nodePresent = 1; c.atomic = 1;
        c.radius = Rand(n, 131) % 2 ? FloatOf(n, 132, 3u) : Near(n, 133, 0.1f);
        c.radiusAfter = Rand(n, 134) % 2 ? FloatOf(n, 135, 3u) : Near(n, 136, 0.1f);
        c.atomFlags = Rand(n, 137) % 2 ? 2u : Rand(n, 138) & 0xFFu;
        break;
    case 14:   // distancia al cuadrado junto a 400 y diferencia de rotacion junto a 0.5236
        c.rotor = B(0.0044f + float(Rand(n, 130) % 1500u) / 10000.0f);
        c.model = models[2 + Rand(n, 131) % 9];
        c.nodePresent = 1; c.matPresent = 1;
        {
            double d2[3]; UnitVec(n, 132, d2);
            const double r = 20.0 * (1.0 + (double(int(Rand(n, 135) % 2001u) - 1000)) * 2e-7);
            for (int i = 0; i < 3; ++i) c.camPos[i] = B(float(double(F(c.pos[i])) + d2[i] * r));
            const float w = F(c.wheelRot);
            c.prop = Rand(n, 136) % 2 ? Near(n, 137, 0.5236f + w) : B(w + F(Near(n, 138, 0.5236f)));
        }
        break;
    case 15:   // basura en los bits sin uso de las banderas y modelos vecinos
        c.flags40 = Rand(n, 130) | (Rand(n, 131) % 2 ? 0x8000000u : 0u);
        c.b428 = 0xFFu; c.b42c = Rand(n, 132) & 0xFFu; c.b960 = Rand(n, 133) & 0xFFu;
        c.handFlags = Rand(n, 134) % 2 ? 0xFFFFFFFFu : 0xFDFFFFFFu;
        c.model = (Rand(n, 135) % 2 ? 0x1D1u : 0x1BFu) + (Rand(n, 136) % 3) - 1u;
        break;
    case 16:   // modelos RC con y sin el bit 8 de +0x40: suma al rotor
        Alive();
        c.model = Rand(n, 130) % 2 ? 0x1D1u : 0x1F5u;
        c.rotor = Near(n, 131, 0.22f);
        break;
    case 17:   // banderas del manejo (bit 25) y bits vecinos
        c.status = ((1u + Rand(n, 130) % 2u) << 3) | (Rand(n, 131) & 7u);
        c.handFlags = (Rand(n, 132) % 2 ? 0x2000000u : 0u) | (Rand(n, 133) % 2 ? Rand(n, 134) & ~0x2000000u : 0u);
        break;
    case 18:   // camara sin matriz (posicion compacta) y con matriz
        c.rotor = B(0.0044f + float(Rand(n, 130) % 1500u) / 10000.0f);
        c.nodePresent = 1; c.camMat = (n / 16) % 2;
        break;
    case 19:   // helicoptero sin matriz (posicion compacta): sin llegar a Multiply3x3
        c.matPresent = 0;
        c.rotor = Rand(n, 130) % 2 ? B(0.0044f + float(Rand(n, 131) % 1500u) / 10000.0f) : B(0.015f + float(Rand(n, 131) % 3000u) / 10000.0f);
        c.nodePresent = 1;
        break;
    default:   // 20: mezcla de los extremos
        Alive();
        c.rotor = FloatOf(n, 130, 5u); c.step = FloatOf(n, 131, 5u);
        for (int i = 0; i < 3; ++i) { c.pos[i] = FloatOf(n, 132 + i, 5u); c.camPos[i] = FloatOf(n, 135 + i, 5u); }
        break;
    }
    NoMatrixSafe();
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("status", c.status).U("model", c.model).U("flags40", c.flags40).U("b428", c.b428).U("b42c", c.b42c).U("b960", c.b960).U("bb8", c.bB8).U("hand_flags", c.handFlags);
    j.U("rotor", c.rotor).U("ctl0", c.ctl[0]).U("ctl1", c.ctl[1]).U("ctl2", c.ctl[2]).U("ctl3", c.ctl[3]).U("vel0", c.vel[0]).U("vel1", c.vel[1]).U("vel2", c.vel[2]);
    j.U("prop", c.prop).U("wheel_rot", c.wheelRot).U("node_present", c.nodePresent);
    for (int i = 0; i < 16; ++i) j.U(("frame" + std::to_string(i)).c_str(), c.frame[i]);
    j.U("mat_present", c.matPresent).U("pos0", c.pos[0]).U("pos1", c.pos[1]).U("pos2", c.pos[2]);
    for (int i = 0; i < 12; ++i) j.U(("mat" + std::to_string(i)).c_str(), c.mat[i]);
    j.U("atomic", c.atomic).U("atom_flags", c.atomFlags).U("radius", c.radius).U("radius_after", c.radiusAfter);
    j.U("cam_mat", c.camMat).U("cam0", c.camPos[0]).U("cam1", c.camPos[1]).U("cam2", c.camPos[2]).U("step", c.step).U("gate", c.gate);
    return j.End();
}
}

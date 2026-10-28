#pragma once
// Fixtures del banco CarConvoy75 (lote 75): CCarCtrl::CreateConvoy (EXE 0x42C740), 21504 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::CreateConvoy  0x42C740  cdecl (CVehicle* plantilla, int32 tipo de ped), el llamador limpia 8 bytes, resultado en AL   (hook CCarCtrl / CreateConvoy, tipo S)
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

namespace Cv75Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;    // familias 0..15
constexpr unsigned kExtraFamilies = 5;    // familias 16..20
constexpr unsigned kCases = kBaseCases + kExtraFamilies * 1024;
constexpr unsigned kFamilies = 16 + kExtraFamilies;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto). Los vehiculos son una ventana de 0x5A0 bytes (el EXE toca hasta +0x594); los peds, 0x4A0 (hasta +0x47C).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x5A0, kPedWin = 0x4A0, kIntelWin = 0x100, kMatWin = 0x48, kTaskWin = 0x40, kInfoWin = 0x40;
constexpr std::size_t kArena(std::size_t win) { return kGuard + win + kGuard; }
constexpr unsigned kModels = 1101;        // indices de modelo con region propia (tabla de carga 0x8E4CD0 y tabla de modelos 0xA9B0C8)

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 75002u); }
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
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 rnd{};            // retorno de rand() del EXE (0x821B1E): solo cuentan los 16 bits bajos
    u32 ped_type{};       // int32 (segundo argumento)
    u32 tmodel{};         // word +0x22 de la plantilla (indice en la tabla de modelos 0..1100)
    u32 radius{};         // float en +0x24 de la esfera de colision del modelo de la plantilla
    u32 cruise{};         // byte +0x3D0 de la plantilla
    u32 speed{};          // float +0x3C8 de la plantilla (m_speed)
    u32 fwd[3]{};         // fila +0x10 de la matriz de la plantilla (la plantilla SIEMPRE tiene matriz: el EXE la lee sin comprobar el nulo)
    u32 pos[3]{};         // posicion de la matriz de la plantilla (+0x30)
    u32 null_mask{};      // bit i: GetNewVehicleDependingOnCarModel devuelve nulo en la vuelta i
    u32 hit[6]{}, hitz[6]{};   // retorno y z del golpe de las llamadas sucesivas a ProcessVerticalLine (dos por vuelta que llega hasta alli)
    u32 height[3]{};      // retornos sucesivos de GetHeightAboveRoad
    u32 col[3]{};         // retornos sucesivos del contador de FindObjectsKindaColliding (word)
    u32 cmat[3]{};        // el coche de la vuelta i tiene matriz
    u32 ctype[3]{};       // int32 +0x590 del coche de la vuelta i
    u32 ctask_null[3]{};  // CTask::operator new devuelve nulo en la vuelta i
    u32 garbage{};
};

inline float Unit(u32 c, u32 k) { return float(int(Rand(c, k) % 2001u) - 1000) / 1000.0f; }   // [-1, 1]
inline u32 Nudge(u32 bits, int ulps) { return u32(i32(bits) + ulps); }
inline u32 Spec(u32 n, u32 k) { return FloatOf(n, k, 3u); }
// Un booleano en un dword: basura en los bytes altos con probabilidad 1/2 (los llamadores y los falsos leen AL)
inline u32 Bool(u32 n, u32 k, bool v) { return (Rand(n, k) % 2 ? (Rand(n, k + 1) & 0xFFFFFF00u) : 0u) | (v ? 1u : 0u); }

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.ecx = SeedReg(n, 3); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.rnd = Rand(n, 10) % 4 == 0 ? Rand(n, 11) : Rand(n, 11) % 65536u;
    c.ped_type = Rand(n, 14) % 4 == 0 ? Rand(n, 15) : u32(int(Rand(n, 15) % 45u) - 4);
    c.tmodel = Rand(n, 18) % 2 ? Rand(n, 19) % kModels : Pick(std::array<u32, 6>{596u, 597u, 598u, 427u, 490u, 0u}, n, 20);
    c.radius = B(1.0f + float(Rand(n, 21) % 500u) / 100.0f);
    c.cruise = Rand(n, 22) % 256;
    c.speed = Rand(n, 23) % 6 == 0 ? B(Unit(n, 24) * 3.0f) : B(float(Rand(n, 25) % 3000u) / 100.0f);
    for (int i = 0; i < 3; ++i) c.fwd[i] = B(Unit(n, 30 + i));
    c.pos[0] = B(Unit(n, 33) * 3000.0f); c.pos[1] = B(Unit(n, 34) * 3000.0f); c.pos[2] = B(Unit(n, 35) * 100.0f);
    c.null_mask = Rand(n, 36) % 8 == 0 ? Rand(n, 37) % 8 : 0u;
    const float pz = F(c.pos[2]);
    for (int i = 0; i < 6; ++i) {
        c.hit[i] = Bool(n, 40 + 4 * i, Rand(n, 41 + 4 * i) % 8 != 0);
        c.hitz[i] = B(pz + Unit(n, 42 + 4 * i) * 30.0f);
    }
    for (int i = 0; i < 3; ++i) c.height[i] = B(0.5f + float(Rand(n, 70 + i) % 300u) / 100.0f);
    for (int i = 0; i < 3; ++i) c.col[i] = Rand(n, 74 + i) % 6 == 0 ? (Rand(n, 80 + i) % 5 + 1) | (Rand(n, 83 + i) % 2 ? 0u : (Rand(n, 86 + i) & 0xFFFF0000u)) : (Rand(n, 89 + i) % 3 == 0 ? (Rand(n, 92 + i) & 0xFFFF0000u) : 0u);
    for (int i = 0; i < 3; ++i) { c.cmat[i] = Rand(n, 95 + i) % 4 != 0; c.ctype[i] = Rand(n, 98 + i) % 4 == 0 ? 9u : Rand(n, 101 + i) % 12; c.ctask_null[i] = Rand(n, 104 + i) % 10 == 0; }
    c.garbage = Rand(n, 108);
    switch (c.family) {
    case 0:   // el caso base (todo aleatorio)
        break;
    case 1: {  // sorteo de la tercera pieza: r * 100 / 32768 > 50 <=> r >= 16712
        static constexpr u32 low[]{16710u, 16711u, 16712u, 16713u, 0u, 1u, 65535u, 32768u, 16384u, 16385u};
        c.rnd = (Rand(n, 120) % 2 ? Rand(n, 121) & 0xFFFF0000u : 0u) | low[Rand(n, 122) % std::size(low)];
        break;
    }
    case 2:   // reservas nulas en todas las combinaciones de vueltas
        c.null_mask = (n / 16) % 8;
        c.rnd = (Rand(n, 120) % 2 ? 16712u : 0u);
        break;
    case 3: {  // golpes: todas las combinaciones por llamada, con distancias relativas a la z de la posicion (cerca, lejos, igual, simetrico)
        const float z = F(c.pos[2]);
        for (int i = 0; i < 6; i += 2) {
            c.hit[i] = Bool(n, 120 + i, Rand(n, 130 + i) % 2 == 0); c.hit[i + 1] = Bool(n, 140 + i, Rand(n, 150 + i) % 2 == 0);
            const float d = Unit(n, 160 + i) * 20.0f;
            c.hitz[i] = B(z + d);
            switch (Rand(n, 170 + i) % 5) {
            case 0: c.hitz[i + 1] = B(z - d); break;
            case 1: c.hitz[i + 1] = c.hitz[i]; break;
            case 2: c.hitz[i + 1] = B(z + d * 0.5f); break;
            case 3: c.hitz[i + 1] = B(z + d * 2.0f); break;
            default: c.hitz[i + 1] = Nudge(B(z - d), int(Rand(n, 180 + i) % 3u) - 1); break;
            }
        }
        break;
    }
    case 4: {  // golpes especiales y la frontera de 1e9
        static constexpr u32 sp[]{0x4E6E6B28u, 0x4E6E6B29u, 0x4E6E6B27u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0x7F800001u, 0xFFC12345u, 0u, 0x80000000u, 1u, 0x7F7FFFFFu, 0xFF7FFFFFu};
        for (int i = 0; i < 6; ++i) { c.hit[i] = Bool(n, 120 + i, Rand(n, 130 + i) % 4 != 0); if (Rand(n, 140 + i) % 2) c.hitz[i] = sp[Rand(n, 150 + i) % std::size(sp)]; }
        if (Rand(n, 160) % 3 == 0) c.pos[2] = Spec(n, 161);
        break;
    }
    case 5: {  // alturas especiales (NaN de todas clases, infinitos, desnormales) frente a suelos finitos o NaN
        static constexpr u32 nn[]{0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFA00000u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00001u, 0xFFBFFFFFu, 0x7F800002u};
        for (int i = 0; i < 3; ++i) c.height[i] = Rand(n, 120 + i) % 3 == 0 ? Spec(n, 123 + i) : nn[Rand(n, 126 + i) % std::size(nn)];
        for (int i = 0; i < 6; ++i) { c.hit[i] = Bool(n, 130 + i, Rand(n, 136 + i) % 6 != 0); if (Rand(n, 142 + i) % 2) c.hitz[i] = nn[Rand(n, 148 + i) % std::size(nn)]; }
        break;
    }
    case 6:   // contadores de FindObjectsKindaColliding: basura en la mitad alta del dword, valores en la baja
        for (int i = 0; i < 3; ++i) c.col[i] = Rand(n, 120 + i) % 2 ? (Rand(n, 123 + i) & 0xFFFF0000u) | (Rand(n, 126 + i) % 2 ? 0u : Rand(n, 129 + i) % 3 + 1) : (Rand(n, 132 + i) % 2 ? 0u : 0x10000u);
        break;
    case 7:   // adelante especial (NaN no canonicos, infinitos, desnormales, ceros con signo)
        for (int i = 0; i < 3; ++i) if (Rand(n, 120 + i) % 2 || i == 0) c.fwd[i] = Spec(n, 123 + i);
        break;
    case 8:   // posicion especial de la plantilla
        for (int i = 0; i < 3; ++i) if (Rand(n, 120 + i) % 2 || i == 2) c.pos[i] = Spec(n, 123 + i);
        break;
    case 9:   // radio especial (infinito, NaN, enorme, desnormal, negativo): T = float(2 r + 2.5)
        c.radius = Spec(n, 120);
        if (Rand(n, 121) % 3 == 0) c.radius = Pick(kBig, n, 122);
        break;
    case 10:  // exponentes extremos del adelante y la posicion: los productos desbordan o caen al rango desnormal del float
        for (int i = 0; i < 3; ++i) c.fwd[i] = Rand(n, 120 + i) % 3 == 0 ? Denorm(n, 123 + i) : ExpFloat(n, 126 + i, Rand(n, 129 + i) % 2 ? int(Rand(n, 132 + i) % 40u) - 126 : int(Rand(n, 135 + i) % 30u) + 100);
        for (int i = 0; i < 3; ++i) c.pos[i] = ExpFloat(n, 138 + i, int(Rand(n, 141 + i) % 60u) - 30);
        c.radius = Rand(n, 144) % 2 ? ExpFloat(n, 145, int(Rand(n, 146) % 40u) + 80) : c.radius;
        break;
    case 11:  // velocidad de crucero (los 256 valores del byte) y velocidad de la plantilla alrededor de los indices 0, 1 y 2
        c.cruise = (n / 16) % 256;
        c.speed = Rand(n, 120) % 3 == 0 ? B(float(int(Rand(n, 121) % 5u) - 1) + Unit(n, 122) * 0.001f) : B(float(int(Rand(n, 123) % 6u)));
        if (Rand(n, 124) % 8 == 0) c.speed = Spec(n, 125);
        break;
    case 12:  // tipo de vehiculo y matriz de cada coche
        for (int i = 0; i < 3; ++i) { c.ctype[i] = Rand(n, 120 + i) % 3 == 0 ? Rand(n, 123 + i) : Rand(n, 126 + i) % 14; c.cmat[i] = Rand(n, 129 + i) % 2; }
        c.rnd = 16712u + Rand(n, 132) % 2;
        for (int i = 0; i < 6; ++i) { c.hit[i] = Bool(n, 133 + i, true); c.hitz[i] = B(F(c.pos[2]) + Unit(n, 139 + i) * 5.0f); }
        for (int i = 0; i < 3; ++i) c.col[i] = 0;
        c.null_mask = 0;
        break;
    case 13:  // tareas nulas y relleno: todo se crea
        for (int i = 0; i < 3; ++i) c.ctask_null[i] = Rand(n, 120 + i) % 2;
        c.rnd = 16712u;
        for (int i = 0; i < 6; ++i) { c.hit[i] = Bool(n, 133 + i, true); c.hitz[i] = B(F(c.pos[2]) + Unit(n, 139 + i) * 5.0f); }
        for (int i = 0; i < 3; ++i) c.col[i] = 0;
        c.null_mask = 0;
        break;
    case 14:  // mezcla de rangos en todo
        for (int i = 0; i < 3; ++i) { c.fwd[i] = FloatOf(n, 120 + i, 5u); c.pos[i] = FloatOf(n, 123 + i, 5u); c.height[i] = FloatOf(n, 126 + i, 5u); }
        for (int i = 0; i < 6; ++i) c.hitz[i] = FloatOf(n, 130 + i, 5u);
        c.radius = FloatOf(n, 140, 5u); c.speed = FloatOf(n, 141, 5u);
        break;
    case 15:  // tres coches: todo true y cerca (recorre las tres vueltas completas)
        c.rnd = 16712u + Rand(n, 120) % 100; c.null_mask = 0;
        for (int i = 0; i < 6; ++i) { c.hit[i] = Bool(n, 121 + i, true); c.hitz[i] = B(F(c.pos[2]) + Unit(n, 127 + i) * 5.0f); }
        for (int i = 0; i < 3; ++i) c.col[i] = 0;
        break;
    case 16:  // NaN no canonicos / de señal en el adelante y la posicion a la vez (reglas de NaN de la x87 con dos NaN)
        for (int i = 0; i < 3; ++i) { c.fwd[i] = Rand(n, 120 + i) % 2 ? Pick(kNan, n, 123 + i) : c.fwd[i]; c.pos[i] = Rand(n, 126 + i) % 2 ? Pick(kNan, n, 129 + i) : c.pos[i]; }
        if (Rand(n, 132) % 2) c.radius = Pick(kNan, n, 133);
        break;
    case 17:  // el producto k * adelante * T desborda: radio grande y adelante cerca de FLT_MAX
        c.radius = Rand(n, 120) % 2 ? Pick(kBig, n, 121) : ExpFloat(n, 122, 100 + int(Rand(n, 123) % 27u));
        for (int i = 0; i < 3; ++i) c.fwd[i] = Rand(n, 124 + i) % 2 ? Pick(kBig, n, 127 + i) : ExpFloat(n, 130 + i, 90 + int(Rand(n, 133 + i) % 37u));
        for (int i = 0; i < 3; ++i) c.pos[i] = Rand(n, 136 + i) % 2 ? Pick(kBig, n, 139 + i) : c.pos[i];
        break;
    case 18:  // velocidad de la plantilla NaN / infinita / diminuta y crucero extremo
        c.speed = Rand(n, 120) % 2 ? Pick(kNan, n, 121) : Rand(n, 122) % 2 ? Pick(kInf, n, 123) : Denorm(n, 124);
        c.cruise = Rand(n, 125) % 2 ? 255u : Rand(n, 126) % 3;
        break;
    case 19:  // todo con valores especiales
        for (int i = 0; i < 3; ++i) { c.fwd[i] = Spec(n, 120 + i); c.pos[i] = Spec(n, 123 + i); c.height[i] = Spec(n, 126 + i); }
        for (int i = 0; i < 6; ++i) c.hitz[i] = Spec(n, 130 + i);
        c.radius = Spec(n, 140); c.speed = Spec(n, 141);
        break;
    default:  // 20: registros de entrada con basura y tipo de ped extremo
        c.eax = Rand(n, 120); c.ecx = Rand(n, 121); c.edx = Rand(n, 122);
        c.ped_type = Pick(std::array<u32, 6>{0x80000000u, 0x7FFFFFFFu, u32(-1), 0u, 14u, 23u}, n, 123);
        c.rnd = Rand(n, 124) % 2 ? 16712u : Rand(n, 125);
        break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("rnd", c.rnd).U("ped_type", c.ped_type).U("tmodel", c.tmodel).U("radius", c.radius).U("cruise", c.cruise).U("speed", c.speed);
    j.U("fwd_x", c.fwd[0]).U("fwd_y", c.fwd[1]).U("fwd_z", c.fwd[2]).U("pos_x", c.pos[0]).U("pos_y", c.pos[1]).U("pos_z", c.pos[2]).U("null_mask", c.null_mask);
    for (int i = 0; i < 6; ++i) { j.U(("hit" + std::to_string(i)).c_str(), c.hit[i]).U(("hitz" + std::to_string(i)).c_str(), c.hitz[i]); }
    for (int i = 0; i < 3; ++i) {
        j.U(("height" + std::to_string(i)).c_str(), c.height[i]).U(("col" + std::to_string(i)).c_str(), c.col[i]).U(("cmat" + std::to_string(i)).c_str(), c.cmat[i]);
        j.U(("ctype" + std::to_string(i)).c_str(), c.ctype[i]).U(("ctask_null" + std::to_string(i)).c_str(), c.ctask_null[i]);
    }
    j.U("garbage", c.garbage);
    return j.End();
}
}

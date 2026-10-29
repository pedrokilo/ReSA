#pragma once
// Fixtures del banco Veh81A (lote 81A): CVehicle::UpdateTrailerLink (EXE 0x6DFC50), 18432 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CVehicle::UpdateTrailerLink  0x6DFC50  thiscall, ECX = this (tractor), dos bool (dwords en la pila), `ret 8`, void   (hook CVehicle / UpdateTrailerLink, tipo S)
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

namespace V81Fix {
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

// Arenas: guarda + ventana + guarda (la ventana es el objeto). Tractor: un remolque completo (el EXE lee hasta +0x9EC); remolcado: hasta +0x86C (CAutomobile).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kSelfWin = 0xA20, kTowWin = 0x890, kMatWin = 0x48;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kSelfArena = Arena(kSelfWin), kTowArena = Arena(kTowWin), kMatArena = Arena(kMatWin);

constexpr u32 kStepAddr = 0xB7CB5C;     // CTimer::ms_fTimeStep
constexpr u32 kHoistAddr = 0x8D313C;    // word: limite del brazo de la grua

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 81081u); }
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
    template <std::size_t N> Js& A(const char* k, const u32 (&v)[N]) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < N; ++i) { if (i) s += ','; s += std::to_string(v[i]); }
        s += ']'; return *this;
    }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ valores notables
// NaN: un unico patron (el indefinido de la x87, que producen inf-inf y 0*inf): con dos NaN distintos en una operacion la carga que sale depende del orden de operandos que elija el compilador
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
inline u32 ExpFloat(u32 c, u32 k, int e) { const u32 r = Rand(c, k); return ((r >> 31) << 31) | (u32(e + 127) << 23) | (Rand(c, k + 500) & 0x7FFFFFu); }
inline u32 Denorm(u32 c, u32 k) { const u32 r = Rand(c, k); return ((r >> 31) << 31) | ((Rand(c, k + 500) & 0x7FFFFFu) | 1u); }
inline u32 Sgn(u32 bits, u32 neg) { return (bits & 0x7FFFFFFFu) | (neg << 31); }
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Un float: modo 0 moderado, 1 pequeno, 2 grande (desbordamiento), 3 especial (cero, desnormal, inf, NaN), 4 unitario, 5 mezcla de todo
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
// Valor "realista": con 1/32 de ceros, si no exponente en [lo, hi]
inline u32 Real(u32 c, u32 k, int lo, int hi) { return Rand(c, k) % 32 == 0 ? B(0.0f) : RandFloat(c, k + 2, lo, hi); }
// float(a + b) con un solo redondeo (a y b son floats)
inline u32 Add(u32 a, u32 b) { return B(float(double(F(a)) + double(F(b)))); }
// Avanza un patron de bits n unidades (para los bordes de un umbral)
inline u32 Ulps(u32 bits, int n) { return u32(i32(bits) + n); }

// ------------------------------------------------------------------------------------------------ el caso
constexpr unsigned kEvents = 5;        // hitch, barra, velocidad 1, velocidad 2, fuerza (o la ruptura en cualquier punto)
constexpr u32 kNoMutation = 99;        // mutAt fuera de rango = sin mutacion
// mutKind: 1 remolque -> B (+0x4C4), 2 matriz del tractor -> alternativa, 3 matriz del remolque A -> alternativa (matriz U), 4 paso global, 5 masa, 6 masa de giro, 7 centro x, 8 subtipo (+0x594),
//          9 dword +0x9EC, 10 brazo del remolque A (+0x86C), 11 modelo del remolque A, 12 limite 0x8D313C
constexpr u32 kMutKinds = 12;
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 arg0{}, arg1{};          // dwords de la pila: el EXE lee solo el byte bajo
    u32 statusByte{};            // byte +0x36
    u32 hasTrailer{};            // +0x4C4 no nulo
    u32 hitchRet{}, barRet{};    // EAX de los virtuales +0xEC y +0xF0 (el EXE lee AL)
    u32 hitch[3]{}, bar[3]{};    // lo que escriben
    u32 ts{};                    // CTimer::ms_fTimeStep
    u32 selfM[12]{}, selfPos[3]{};     // matriz del tractor (right, forward, up) y posicion
    u32 towM[12]{}, towPos[3]{};       // matriz del remolque A
    u32 altSelfM[12]{};          // matriz alternativa del tractor
    u32 altTowM[12]{}, altTowPos[3]{}; // matriz U (alternativa de A y matriz de B)
    u32 mass{}, turn{}, centre[3]{};
    u32 subtype{}, w9ec{};
    u32 model{}, angle{}, hoist{};     // remolque A: modelo (word +0x22), brazo (word +0x86C); limite global (word 0x8D313C)
    u32 modelB{}, angleB{};      // remolque B
    u32 spd[2][3]{};             // resultados de GetSpeed (tractor, remolque)
    u32 mutAt{}, mutKind{}, mutVal{};
    u32 gate{};                  // informativo
};

inline bool Low(u32 dword) { return (dword & 0xFFu) != 0; }

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    // bool de la pila: byte bajo 0/1 y basura en los bytes altos
    auto boolArg = [&](bool v, u32 k) { return (Rand(n, k) % 3 == 0 ? 0u : (Rand(n, k + 1) & 0xFFFFFF00u)) | (v ? 1u : 0u); };
    const u32 combo = Rand(n, 10) % 4;
    c.arg0 = boolArg(combo & 1, 11);
    c.arg1 = boolArg(combo & 2, 13);
    if (Rand(n, 15) % 3 == 0) { c.arg0 = boolArg(false, 11); c.arg1 = boolArg(true, 13); }   // la rama de amortiguacion es la mas larga: mas peso
    c.statusByte = ((10u + Rand(n, 16) % 2u) << 3) | (Rand(n, 17) & 7u);
    c.hasTrailer = 1;
    auto ret = [&](bool v, u32 k) { const u32 hi = Rand(n, k) % 3 == 0 ? 0u : (Rand(n, k + 1) & 0xFFFFFF00u); return hi | (v ? (1u + Rand(n, k + 2) % 255u) : 0u); };
    c.hitchRet = ret(true, 20); c.barRet = ret(true, 24);
    // posiciones: el enganche del tractor y la barra del remolque quedan cerca (la distancia entra en el limite 1 * max(paso, 0.7))
    c.ts = Rand(n, 28) % 2 ? B(1.0f) : RandFloat(n, 29, -1, 1);
    for (int i = 0; i < 3; ++i) c.hitch[i] = Real(n, 30 + i, -3, 5);
    for (int i = 0; i < 3; ++i) c.bar[i] = Add(c.hitch[i], RandFloat(n, 34 + i, -8, -2));
    // matrices: forward/up casi iguales en el tractor y el remolque (los ejes no pasan del umbral de ruptura)
    for (int i = 0; i < 12; ++i) c.selfM[i] = (i & 3) == 3 ? Rand(n, 40 + i) : RandFloat(n, 40 + i, -2, 0);
    for (int i = 0; i < 12; ++i) c.towM[i] = (i & 3) == 3 ? Rand(n, 56 + i) : ((i >> 2) == 0 ? RandFloat(n, 56 + i, -2, 0) : Add(c.selfM[i], RandFloat(n, 56 + i, -12, -4)));
    for (int i = 0; i < 12; ++i) c.altSelfM[i] = (i & 3) == 3 ? Rand(n, 72 + i) : ((i >> 2) == 0 ? RandFloat(n, 72 + i, -2, 0) : Add(c.selfM[i], RandFloat(n, 72 + i, -12, -4)));
    for (int i = 0; i < 12; ++i) c.altTowM[i] = (i & 3) == 3 ? Rand(n, 88 + i) : ((i >> 2) == 0 ? RandFloat(n, 88 + i, -2, 0) : Add(c.selfM[i], RandFloat(n, 88 + i, -12, -4)));
    for (int i = 0; i < 3; ++i) { c.selfPos[i] = Real(n, 104 + i, -3, 6); c.towPos[i] = Real(n, 108 + i, -3, 6); c.altTowPos[i] = Real(n, 112 + i, -3, 6); }
    c.mass = Rand(n, 116) % 8 == 0 ? RandFloat(n, 117, 0, 14) : B(1500.0f + float(Rand(n, 118) % 2000u));
    c.turn = Rand(n, 119) % 8 == 0 ? RandFloat(n, 120, 0, 14) : B(2000.0f + float(Rand(n, 121) % 4000u));
    for (int i = 0; i < 3; ++i) c.centre[i] = Real(n, 124 + i, -4, 2);
    c.subtype = Rand(n, 128) % 2 ? 11u : Rand(n, 129) % 12u;
    c.w9ec = Rand(n, 130) % 3 ? 0xC47A0000u : Rand(n, 131);
    {
        static constexpr std::array<u32, 6> models{0x20Du, 0x213u, 0x20Cu, 0x214u, 0x1E0u, 0x0u};
        c.model = Rand(n, 132) % 2 ? Pick(models, n, 133) : Rand(n, 134) % 0x10000u;
        c.modelB = Rand(n, 135) % 2 ? Pick(models, n, 136) : Rand(n, 137) % 0x10000u;
        static constexpr std::array<u32, 8> hoists{20000u, 100u, 99u, 0u, 1u, 0xFFFFu, 0x8000u, 0x7FFFu};
        c.hoist = Rand(n, 138) % 2 ? 20000u : Pick(hoists, n, 139);
        const i32 limit = i32(c.hoist) - 100;
        const i32 pick = limit + i32(Rand(n, 140) % 7) - 3;
        c.angle = Rand(n, 141) % 2 ? u32(pick) & 0xFFFFu : Rand(n, 142) % 0x10000u;
        c.angleB = Rand(n, 143) % 2 ? u32(pick) & 0xFFFFu : Rand(n, 144) % 0x10000u;
    }
    for (int g = 0; g < 2; ++g) for (int i = 0; i < 3; ++i) c.spd[g][i] = Real(n, 150 + 3 * g + i, -5, 3);
    c.mutAt = Rand(n, 160) % 3 == 0 ? Rand(n, 161) % kEvents : kNoMutation;
    c.mutKind = c.mutAt == kNoMutation ? 0u : 1u + Rand(n, 162) % kMutKinds;

    // La mayoria de los casos supera todas las comprobaciones y llega a la fuerza: modelo que no es grua ni tractor, o brazo por debajo del limite
    auto passModel = [&] {
        if (c.model == 0x20Du || c.model == 0x213u) {
            if (i32(c.angle & 0xFFFFu) > i32(c.hoist & 0xFFFFu) - 100) c.angle = ((c.hoist & 0xFFFFu) - 101u) & 0xFFFFu;
            if (i32(c.angle & 0xFFFFu) > i32(c.hoist & 0xFFFFu) - 100) c.model = 0x1E0u;
        }
    };
    if (f != 6 && f != 12) passModel();
    auto freeDamp = [&] { c.arg0 = boolArg(false, 11); c.arg1 = boolArg(true, 13); };
    switch (f) {
    case 0: break;                       // flujo normal
    case 1:                              // las cuatro combinaciones de bool
        c.arg0 = boolArg(Rand(n, 170) & 1, 171); c.arg1 = boolArg(Rand(n, 172) & 1, 173);
        break;
    case 2: {                            // borde de la distancia: bar = hitch + (a, 0, 0) con a alrededor de max(paso, 0.7)
        static constexpr std::array<u32, 9> steps{0x3F800000u, 0x3F000000u, 0x3F333333u, 0x3F333332u, 0x3F333334u, 0x00000000u, 0xBF800000u, 0x40000000u, 0x3FA00000u};
        c.ts = Rand(n, 174) % 4 == 0 ? RandFloat(n, 175, -1, 2) : Pick(steps, n, 176);
        const float t = !(F(c.ts) > 0.7f) ? 0.7f : F(c.ts);
        const u32 axis = Rand(n, 177) % 3;
        const u32 a = Ulps(B(t), int(Rand(n, 178) % 9) - 4);
        const bool zero = Rand(n, 179) % 2;
        for (int i = 0; i < 3; ++i) c.hitch[i] = zero ? 0u : Real(n, 180 + i, -3, 5);
        for (int i = 0; i < 3; ++i) c.bar[i] = zero ? (u32(i) == axis ? a : 0u) : Add(c.hitch[i], u32(i) == axis ? a : 0u);
        if (zero && Rand(n, 183) % 2) c.bar[axis] |= 0x80000000u;   // el signo no cambia la distancia
        break;
    }
    case 3: {                            // borde del eje delantero: dot < -0.3f (la suma z, y, x de productos)
        static constexpr std::array<u32, 7> edges{0xBE99999Au, 0xBE999999u, 0xBE99999Bu, 0xBE19999Au, 0xBE99999Cu, 0xBE999998u, 0xBF000000u};
        for (int i = 4; i < 7; ++i) { c.selfM[i] = 0u; c.towM[i] = 0u; }
        const u32 axis = Rand(n, 190) % 3;
        const u32 mode = Rand(n, 191) % 3;
        if (mode == 0) {                 // un eje: s = (1,0,0), t = (e,0,0)
            c.selfM[4 + axis] = B(1.0f);
            c.towM[4 + axis] = Pick(edges, n, 192);
        } else if (mode == 1) {          // dos ejes: s = (1,1,0), t = (e/2, e/2, 0)
            const u32 half = Ulps(Pick(edges, n, 192), -(1 << 23));
            const u32 a2 = (axis + 1) % 3;
            c.selfM[4 + axis] = B(1.0f); c.selfM[4 + a2] = B(1.0f);
            c.towM[4 + axis] = half; c.towM[4 + a2] = half;
        } else {                         // escala: s = (2,0,0), t = (e/2,0,0)
            c.selfM[4 + axis] = B(2.0f);
            c.towM[4 + axis] = Ulps(Pick(edges, n, 192), -(1 << 23));
        }
        if (mode != 1 && Rand(n, 193) % 3 == 0) c.towM[4 + (axis + 1) % 3] = Sgn(RandFloat(n, 194, -3, 3), Rand(n, 195) % 2);   // se suma un termino 0 * x
        break;
    }
    case 4: {                            // borde del eje superior: dot < 0 con el eje delantero alineado
        for (int i = 4; i < 7; ++i) { c.selfM[i] = i == 4 ? B(1.0f) : 0u; c.towM[i] = i == 4 ? B(1.0f) : 0u; }
        static constexpr std::array<u32, 12> ups{0u, 0x80000000u, 1u, 0x80000001u, 0x00800000u, 0x80800000u, 0x007FFFFFu, 0x807FFFFFu, 0x1E3CE508u, 0x9E3CE508u, 0x3F800000u, 0xBF800000u};
        for (int i = 8; i < 11; ++i) { c.selfM[i] = 0u; c.towM[i] = 0u; }
        const u32 axis = Rand(n, 196) % 3;
        const u32 mode = Rand(n, 197) % 4;
        if (mode == 3) {                 // todos los productos son +-0: la suma es -0 solo si los tres terminos son -0
            for (int i = 8; i < 11; ++i) { c.selfM[i] = RandFloat(n, 720 + i, -2, 1) & 0x7FFFFFFFu; c.towM[i] = Rand(n, 730 + i) % 4 ? 0x80000000u : 0u; }
        }
        else if (mode == 0) { c.selfM[8 + axis] = B(1.0f); c.towM[8 + axis] = Pick(ups, n, 198); }
        else if (mode == 1) {            // cancelacion: s = (1,1,0), t = (a, -a, 0)
            const u32 a2 = (axis + 1) % 3, a = Pick(ups, n, 198);
            c.selfM[8 + axis] = B(1.0f); c.selfM[8 + a2] = B(1.0f);
            c.towM[8 + axis] = a; c.towM[8 + a2] = a ^ 0x80000000u;
            if (Rand(n, 199) % 2) c.towM[8 + a2] = Ulps(c.towM[8 + a2], int(Rand(n, 200) % 3) - 1);
        } else { c.selfM[8 + axis] = B(2.0f); c.towM[8 + axis] = Pick(ups, n, 198); }
        break;
    }
    case 5: {                            // salidas anticipadas: sin remolque, estado distinto de 10 y 11, virtuales que fallan
        const u32 w = Rand(n, 201) % 8;
        if (w == 0) c.hasTrailer = 0;
        else if (w <= 2) c.statusByte = ((Rand(n, 202) % 32u) << 3) | (Rand(n, 203) & 7u);
        else if (w == 3) c.hitchRet = ret(false, 204);
        else if (w == 4) c.barRet = ret(false, 208);
        else if (w == 5) { c.hitchRet = ret(false, 204); c.barRet = ret(false, 208); }
        else if (w == 6) { c.hitchRet = Rand(n, 212) % 2 ? 0x100u : 0xFFFFFF00u; }
        else { c.barRet = Rand(n, 213) % 2 ? 0x100u : 0xDEADBE00u; c.statusByte = (10u + Rand(n, 214) % 2u) << 3; }
        break;
    }
    case 6: {                            // modelo grua / tractor y brazo contra el limite (comparacion con signo de dwords, limite negativo si el word es < 100)
        static constexpr std::array<u32, 6> models{0x20Du, 0x213u, 0x20Du, 0x213u, 0x20Cu, 0x214u};
        c.model = Pick(models, n, 215);
        static constexpr std::array<u32, 10> hoists{20000u, 100u, 99u, 0u, 1u, 0xFFFFu, 0x8000u, 0x7FFFu, 101u, 50u};
        c.hoist = Pick(hoists, n, 216);
        const i32 limit = i32(c.hoist) - 100;
        c.angle = u32(limit + i32(Rand(n, 217) % 5) - 2) & 0xFFFFu;
        if (Rand(n, 218) % 5 == 0) c.angle = Pick(kStackSeeds, n, 219) & 0xFFFFu;
        break;
    }
    case 7: {                            // subtipo 11 y el dword +0x9EC contra -1000.0f
        static constexpr std::array<u32, 8> subs{11u, 11u, 11u, 0u, 10u, 12u, 0x8000000Bu, 0xFFFFFFFFu};
        static constexpr std::array<u32, 8> w9{0xC47A0000u, 0xC47A0000u, 0xC47A0001u, 0x447A0000u, 0xC47AFFFFu, 0u, 0xFFFFFFFFu, 0xC4790000u};
        c.subtype = Pick(subs, n, 220);
        c.w9ec = Pick(w9, n, 221);
        freeDamp();
        if (Rand(n, 222) % 2) { c.arg0 = boolArg(Rand(n, 223) & 1, 224); c.arg1 = boolArg(Rand(n, 225) & 1, 226); }
        break;
    }
    case 8: {                            // amortiguacion: paso especial (0, <1, 1, >1, enorme, inf, NaN, negativo) y diferencias de posicion
        static constexpr std::array<u32, 14> steps{0x3F800000u, 0x3F7FFFFFu, 0x3F800001u, 0x3F000000u, 0u, 0x80000000u, 0xBF800000u, 0x40000000u, 0x41200000u, 0x7F800000u, 0xFF800000u, 0xFFC00000u, 0x7F7FFFFFu, 0x00000001u};
        c.ts = Rand(n, 227) % 4 == 0 ? RandFloat(n, 228, -3, 4) : Pick(steps, n, 229);
        freeDamp();
        if (Rand(n, 230) % 3 == 0) for (int i = 0; i < 3; ++i) c.bar[i] = Add(c.hitch[i], FloatOf(n, 231 + i, Rand(n, 234 + i) % 4 == 0 ? 1u : 0u));
        if (Rand(n, 750) % 5 == 0) for (int i = 0; i < 3; ++i) c.bar[i] = c.hitch[i];   // diferencia de posiciones nula
        break;
    }
    case 9:                              // enganche y barra especiales (NaN, infinitos, ceros, desnormales, enormes)
        for (int i = 0; i < 3; ++i) { if (Rand(n, 240 + i) % 2) c.hitch[i] = FloatOf(n, 243 + i, 3u); if (Rand(n, 246 + i) % 2) c.bar[i] = FloatOf(n, 249 + i, 3u); }
        if (Rand(n, 252) % 2) freeDamp();
        break;
    case 10:                             // matrices y centro de masa especiales
        for (int i = 0; i < 12; ++i) if ((i & 3) != 3) {
            if (Rand(n, 253 + i) % 5 == 0) c.selfM[i] = FloatOf(n, 266 + i, 3u);
            if (Rand(n, 279 + i) % 5 == 0) c.towM[i] = FloatOf(n, 292 + i, 3u);
        }
        for (int i = 0; i < 3; ++i) if (Rand(n, 305 + i) % 3 == 0) c.centre[i] = FloatOf(n, 308 + i, 5u);
        if (Rand(n, 311) % 2) freeDamp();
        break;
    case 11:                             // masas especiales (0, negativa, inf, NaN, desnormal, enorme) y centro de masa
        for (u32 pick = 0; pick < 2; ++pick) {
            u32& m = pick ? c.turn : c.mass;
            const u32 w = Rand(n, 312 + pick) % 6;
            m = w == 0 ? Pick(kTiny, n, 314 + pick) : w == 1 ? Pick(kInf, n, 316 + pick) : w == 2 ? Pick(kNan, n, 318 + pick) : w == 3 ? Pick(kBig, n, 320 + pick) : w == 4 ? Sgn(RandFloat(n, 322 + pick, 0, 12), 1u) : FloatOf(n, 324 + pick, 5u);
        }
        for (int i = 0; i < 3; ++i) if (Rand(n, 326 + i) % 3 == 0) c.centre[i] = FloatOf(n, 329 + i, 5u);
        if (Rand(n, 332) % 2) freeDamp();
        break;
    case 12: {                           // barrido de mutaciones: cada evento x cada tipo, con un flujo que llega al final (amortiguacion, subtipo 11, grua)
        const u32 idx = (n / 16) % (kEvents * kMutKinds);
        c.mutAt = idx % kEvents;
        c.mutKind = 1u + idx / kEvents;
        freeDamp();
        c.subtype = Rand(n, 333) % 2 ? 11u : 0u;
        c.w9ec = Rand(n, 334) % 4 ? 0xC47A0000u : Rand(n, 335);
        c.model = Rand(n, 336) % 2 ? 0x20Du : 0x213u; c.modelB = Rand(n, 337) % 2 ? 0x20Du : 0x213u;
        c.hoist = 20000u;
        c.angle = Rand(n, 338) % 2 ? 19899u : 19901u; c.angleB = Rand(n, 339) % 2 ? 19899u : 19901u;
        if (Rand(n, 340) % 2) c.angle = 0u;
        break;
    }
    case 13:                             // velocidades especiales
        for (int g = 0; g < 2; ++g) for (int i = 0; i < 3; ++i) c.spd[g][i] = FloatOf(n, 355 + 3 * g + i, Rand(n, 361 + 3 * g + i) % 4 == 0 ? 3u : 5u);
        switch (Rand(n, 740) % 4) {
        case 0: for (int i = 0; i < 3; ++i) c.spd[1][i] = c.spd[0][i]; break;   // velocidad relativa nula (Normalise: longitud 0)
        case 1:                                                                  // fuerza desbordada: masas enormes y velocidad relativa grande
            c.mass = RandFloat(n, 741, 60, 100) & 0x7FFFFFFFu; c.turn = RandFloat(n, 742, 60, 100) & 0x7FFFFFFFu;
            for (int g = 0; g < 2; ++g) for (int i = 0; i < 3; ++i) c.spd[g][i] = RandFloat(n, 743 + 3 * g + i, 60, 100);
            break;
        default: break;
        }
        if (Rand(n, 367) % 2) freeDamp();
        break;
    case 14: case 15:                    // todo aleatorio, con y sin salidas anticipadas
        if (Rand(n, 368) % 4 == 0) { for (int i = 0; i < 3; ++i) { c.hitch[i] = FloatOf(n, 369 + i, 5u); c.bar[i] = FloatOf(n, 372 + i, 5u); } }
        if (Rand(n, 375) % 4 == 0) for (int i = 0; i < 12; ++i) if ((i & 3) != 3) { c.selfM[i] = FloatOf(n, 376 + i, 5u); c.towM[i] = FloatOf(n, 388 + i, 5u); }
        if (Rand(n, 400) % 4 == 0) { c.mass = FloatOf(n, 401, 5u); c.turn = FloatOf(n, 402, 5u); for (int i = 0; i < 3; ++i) c.centre[i] = FloatOf(n, 403 + i, 5u); }
        if (Rand(n, 406) % 4 == 0) c.ts = FloatOf(n, 407, 5u);
        if (Rand(n, 408) % 6 == 0) c.hasTrailer = 0;
        if (Rand(n, 409) % 6 == 0) c.statusByte = ((Rand(n, 410) % 32u) << 3) | (Rand(n, 411) & 7u);
        if (Rand(n, 412) % 6 == 0) c.hitchRet = ret(false, 413);
        if (Rand(n, 417) % 6 == 0) c.barRet = ret(false, 418);
        if (Rand(n, 421) % 2) freeDamp();
        break;
    case 16:                             // NaN e infinitos en varios sitios a la vez (un solo patron de NaN)
        for (int i = 0; i < 3; ++i) {
            if (Rand(n, 430 + i) % 4 == 0) c.hitch[i] = Rand(n, 433 + i) % 2 ? Pick(kNan, n, 436 + i) : Pick(kInf, n, 436 + i);
            if (Rand(n, 439 + i) % 4 == 0) c.bar[i] = Rand(n, 442 + i) % 2 ? Pick(kNan, n, 445 + i) : Pick(kInf, n, 445 + i);
            if (Rand(n, 448 + i) % 4 == 0) c.centre[i] = Rand(n, 451 + i) % 2 ? Pick(kNan, n, 454 + i) : Pick(kInf, n, 454 + i);
            for (int g = 0; g < 2; ++g) if (Rand(n, 457 + 3 * g + i) % 4 == 0) c.spd[g][i] = Rand(n, 463 + 3 * g + i) % 2 ? Pick(kNan, n, 469 + 3 * g + i) : Pick(kInf, n, 469 + 3 * g + i);
        }
        for (int i = 0; i < 12; ++i) if ((i & 3) != 3) {
            if (Rand(n, 475 + i) % 6 == 0) c.selfM[i] = Rand(n, 487 + i) % 2 ? Pick(kNan, n, 499 + i) : Pick(kInf, n, 499 + i);
            if (Rand(n, 511 + i) % 6 == 0) c.towM[i] = Rand(n, 523 + i) % 2 ? Pick(kNan, n, 535 + i) : Pick(kInf, n, 535 + i);
        }
        if (Rand(n, 547) % 3 == 0) c.ts = Rand(n, 548) % 2 ? Pick(kNan, n, 549) : Pick(kInf, n, 549);
        if (Rand(n, 550) % 5 == 0) c.mass = Pick(kInf, n, 551);
        if (Rand(n, 552) % 5 == 0) c.turn = Pick(kNan, n, 553);
        if (Rand(n, 554) % 2) freeDamp();
        break;
    default:                             // 17: ceros con signo, desnormales y sumas en el limite de la cancelacion
        for (int i = 0; i < 3; ++i) {
            const u32 w = Rand(n, 560 + i) % 3;
            c.hitch[i] = w == 0 ? Pick(kTiny, n, 563 + i) : w == 1 ? Denorm(n, 566 + i) : Sgn(ExpFloat(n, 569 + i, -126 + int(Rand(n, 572 + i) % 12)), Rand(n, 575 + i) % 2);
            const u32 v = Rand(n, 578 + i) % 3;
            c.bar[i] = v == 0 ? Pick(kTiny, n, 581 + i) : v == 1 ? Denorm(n, 584 + i) : Sgn(ExpFloat(n, 587 + i, -126 + int(Rand(n, 590 + i) % 12)), Rand(n, 593 + i) % 2);
            const u32 u = Rand(n, 596 + i) % 4;
            c.centre[i] = u == 0 ? Pick(kTiny, n, 599 + i) : u == 1 ? Denorm(n, 602 + i) : u == 2 ? Sgn(ExpFloat(n, 605 + i, -126 + int(Rand(n, 608 + i) % 12)), Rand(n, 611 + i) % 2) : c.centre[i];
            for (int g = 0; g < 2; ++g) c.spd[g][i] = Rand(n, 614 + 3 * g + i) % 3 == 0 ? Pick(kTiny, n, 620 + 3 * g + i) : Rand(n, 626 + 3 * g + i) % 2 ? Denorm(n, 632 + 3 * g + i) : c.spd[g][i];
        }
        for (int i = 0; i < 12; ++i) if ((i & 3) != 3 && Rand(n, 638 + i) % 4 == 0) c.towM[i] = Rand(n, 650 + i) % 2 ? Pick(kTiny, n, 662 + i) : Denorm(n, 674 + i);
        if (Rand(n, 686) % 3 == 0) c.ts = Pick(kTiny, n, 687);
        if (Rand(n, 688) % 3 == 0) c.mass = Pick(kTiny, n, 689);
        if (Rand(n, 690) % 2) freeDamp();
        break;
    }
    // valor de la mutacion segun el tipo (los tipos 1..3 no usan valor)
    switch (c.mutKind) {
    case 4: c.mutVal = Rand(n, 700) % 2 ? B(1.0f) : FloatOf(n, 701, 5u); break;
    case 5: case 6: c.mutVal = Rand(n, 702) % 3 == 0 ? FloatOf(n, 703, 5u) : B(1000.0f + float(Rand(n, 704) % 3000u)); break;
    case 7: c.mutVal = FloatOf(n, 705, 5u); break;
    case 8: c.mutVal = Rand(n, 706) % 2 ? 11u : Rand(n, 707) % 12u; break;
    case 9: c.mutVal = Rand(n, 708) % 2 ? 0xC47A0000u : Rand(n, 709); break;
    case 10: c.mutVal = Rand(n, 710) % 2 ? Rand(n, 711) % 0x10000u : 19899u + Rand(n, 712) % 3u; break;
    case 11: c.mutVal = Rand(n, 713) % 2 ? 0x20Du : Rand(n, 714) % 0x10000u; break;
    case 12: c.mutVal = Rand(n, 715) % 2 ? Rand(n, 716) % 0x10000u : 20000u - 100u + Rand(n, 717) % 5u; break;
    default: c.mutVal = 0u; break;
    }
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("arg0", c.arg0).U("arg1", c.arg1).U("status_byte", c.statusByte).U("has_trailer", c.hasTrailer).U("hitch_ret", c.hitchRet).U("bar_ret", c.barRet);
    j.A("hitch", c.hitch).A("bar", c.bar).U("ts", c.ts);
    j.A("self_m", c.selfM).A("self_pos", c.selfPos).A("tow_m", c.towM).A("tow_pos", c.towPos).A("alt_self_m", c.altSelfM).A("alt_tow_m", c.altTowM).A("alt_tow_pos", c.altTowPos);
    j.U("mass", c.mass).U("turn", c.turn).A("centre", c.centre).U("subtype", c.subtype).U("w9ec", c.w9ec);
    j.U("model", c.model).U("angle", c.angle).U("hoist", c.hoist).U("model_b", c.modelB).U("angle_b", c.angleB);
    j.A("spd0", c.spd[0]).A("spd1", c.spd[1]);
    j.U("mut_at", c.mutAt).U("mut_kind", c.mutKind).U("mut_val", c.mutVal).U("gate", c.gate);
    return j.End();
}
}

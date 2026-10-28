#pragma once
// Fixtures del banco FxBlood79 (lote 79A): Fx_c::AddBlood del EXE, 16384 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 Fx_c::AddBlood 0x49EB00  thiscall, ECX = this (SI se usa: contador de manchas en this+0x50), (const CVector& pos, const CVector& direccion, int32 cantidad, float luz), `ret 10h`, void   (hook Fx_c / AddBlood, tipo S)
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

namespace Fb79Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;
using i16 = std::int16_t;

constexpr unsigned kCases = 16384;        // por funcion: 16 familias de 1024
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;
enum Fn : u32 { kBlood = 0 };

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 79079u); }
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
    Js& A(const char* k, const std::vector<u32>& v) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < v.size(); ++i) { if (i) s += ','; s += std::to_string(v[i]); }
        s += ']'; return *this;
    }
    std::string End() const { return s + "}"; }
};
inline std::vector<u32> V3(const u32* p) { return std::vector<u32>(p, p + 3); }

// ------------------------------------------------------------------------------------------------ valores notables (NaN incluidos los no canonicos y los senalizantes)
inline constexpr std::array<u32, 28> kSpecial{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x41C80000u, 0xC1C80000u, 0x40000000u, 0x3E800000u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F7FFFFFu, 0x3F800001u, 0x34000000u, 0x33800000u, 0x4B800000u, 0x7F000000u,
    0x7E800000u, 0x5F800000u, 0x1F800000u, 0x20000000u, 0x441C4000u, 0x441C4001u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
// Cargas utiles distintas dos a dos (la regla x87 de dos NaN usa la de mayor mantisa; con cargas iguales no se comparan)
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0xFFC00002u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFEu, 0x7F800001u, 0x7FA00000u, 0xFFA00003u, 0xFF800004u};
inline constexpr std::array<u32, 8> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u};
inline constexpr std::array<u32, 12> kDenorm{1u, 0x80000001u, 2u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x001FFFFFu, 0x801FFFFFu, 0x00200000u, 0x00200001u, 0x80200001u};
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 Neg(u32 b) { return b ^ 0x80000000u; }
// Una magnitud especial para un float de entrada: NaN, infinito, ceros, desnormales, muy grandes, normales
inline u32 MixComponent(u32 c, u32 k) {
    switch (Rand(c, k) % 8) {
    case 0: return Pick(kNan, c, k + 100);
    case 1: return Pick(kInf, c, k + 100);
    case 2: return Rand(c, k + 100) & 0x80000000u;
    case 3: return Rand(c, k + 100) & 0x807FFFFFu;
    case 4: return RandFloat(c, k + 100, 60, 127);
    case 5: return kSpecial[Rand(c, k + 100) % kSpecial.size()];
    case 6: return RandFloat(c, k + 100, -3, 6);
    default: return RandFloat(c, k + 100, -20, -3);
    }
}
// Mueve un float finito `k` unidades en la ultima posicion, conservando el signo.
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

// ------------------------------------------------------------------------------------------------ arenas y memoria global
constexpr u32 kGuard = 0x40;
constexpr u32 kVecWindow = 0x10, kVecArena = kGuard + kVecWindow + kGuard;                  // 0x90   un vector (posicion / direccion)
constexpr u32 kMatWindow = 0x40, kMatArena = kGuard + kMatWindow + kGuard;                  // 0xC0   matriz de la camara (posicion en +0x30)
constexpr u32 kSysWindow = 0x20, kSysArena = kGuard + kSysWindow + kGuard;                  // 0xA0   sistemas de particulas falsos
constexpr u32 kThisWindow = 0x70, kThisArena = kGuard + kThisWindow + kGuard;               // 0xF0   objeto `this` (Fx_c, 0x70 bytes): el contador esta en +0x50
static_assert(kVecArena == 0x90 && kMatArena == 0xC0 && kSysArena == 0xA0 && kThisArena == 0xF0, "arenas");
constexpr u32 kCounterOffset = 0x50;                                                       // Fx_c::m_Randomizer
constexpr u32 kCamPlacement = 0xB6F02C, kCamMatrixPtr = 0xB6F03C, kCamBase = 0xB6F028, kCamBytes = 0x20;   // TheCamera: m_placement (+4) y m_matrix (+0x14)
constexpr u32 kFxBase = 0xA9AE00, kFxBytes = 0x70;                                         // g_fx completo (0x70 bytes): sistemas (+0 .. +0x44) y contador (+0x50)
constexpr u32 kSlotBlood = 0xA9AE00;                                                       // g_fx.m_Blood
constexpr u32 kBloodTex = 0xC40400;                                                        // gpBloodPoolTex

// ------------------------------------------------------------------------------------------------ el caso
// rand() es el REAL del CRT del EXE (0x821B1E). El estado inicial de cada lado es `randSeed`. randMode: 0 = estado cualquiera; 1 = el sorteo numero `randK` vale un borde de kEdgeOut.
struct C {
    u32 fn{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 thisKind{};    // 0 arena, 1 g_fx real
    u32 hasMat{};      // la camara tiene matriz (posicion +0x30) o solo posicion compacta (+4)
    u32 amount{};      // int32 con signo
    u32 light{};       // luz (cuarto argumento)
    u32 bloodOn{};     // lo que devuelve CLocalisation::Blood()
    u32 counter0{};    // Fx_c::m_Randomizer (this+0x50) al empezar
    u32 tex{};         // gpBloodPoolTex (0xC40400)
    u32 randMode{}, randSeed{}, randK{};
    u32 swapSlot{};    // el falso de AddParticle cambia el sistema entre dos entre llamadas (el EXE lo relee en cada vuelta)
    u32 cam[3]{}, other[3]{}, pos[3]{};
    u32 dir[3]{};      // vector direccion (arena)
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
inline u32 SmallAmount(u32 n, u32 k, u32 hi) { return 1 + Rand(n, k) % hi; }
inline u32 NegAmount(u32 n, u32 k) { static constexpr u32 v[]{0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFFFFFFFEu, 0xFFFFFF9Cu, 0xFFFF0000u, 0xC0000000u, 0u, 0u}; return v[Rand(n, k) % std::size(v)]; }

// Congruencia lineal del CRT del EXE y su inversa (A es impar: inversa modulo 2^32 por Newton)
constexpr u32 kLcgA = 0x343FDu, kLcgC = 0x269EC3u;
constexpr u32 InvOdd(u32 a) { u32 x = a; for (int i = 0; i < 5; ++i) x *= 2u - a * x; return x; }
constexpr u32 kLcgInv = InvOdd(kLcgA);
static_assert(kLcgA * kLcgInv == 1u, "inversa de 0x343FD modulo 2^32");
constexpr u32 LcgNext(u32 s) { return s * kLcgA + kLcgC; }
constexpr u32 LcgBack(u32 s) { return (s - kLcgC) * kLcgInv; }
// Estado inicial tal que el sorteo numero k (0 = el primero) devuelve `out`; low/top16 completan los 17 bits que el sorteo no ve (bits 0..15 y 31)
inline u32 SeedForDraw(u32 k, u32 out, u32 low16, u32 bit31) {
    u32 s = (out << 16) | (low16 & 0xFFFFu) | ((bit31 & 1u) << 31);
    for (u32 i = 0; i <= k; ++i) s = LcgBack(s);
    return s;
}
inline constexpr std::array<u32, 13> kEdgeOut{0u, 1u, 2u, 4999u, 5000u, 9999u, 10000u, 16383u, 16384u, 19999u, 29999u, 32766u, 32767u};
// Sorteos que consume AddBlood: 4 por particula (tamano, x, y, z), despues 2 (x e y de la mancha) y, segun el contador, 1 mas (duracion)
inline u32 DrawBudget(const C& f) {
    const i32 amount = static_cast<i32>(f.amount);
    if (amount > 3) return 12;
    return 4 * (amount > 0 ? u32(amount) : 0u) + 3;
}
inline void ApplyRand(C& f, u32 c) {
    if (f.randMode == 1) {
        const u32 budget = DrawBudget(f);
        f.randK = Rand(c, 17) % budget;
        f.randSeed = SeedForDraw(f.randK, kEdgeOut[Rand(c, 21) % kEdgeOut.size()], Rand(c, 22), Rand(c, 23));
    } else {
        f.randMode = 0;
        static constexpr u32 sp[]{0u, 1u, 0x80000000u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x269EC3u};
        f.randSeed = Rand(c, 19) % 12 == 0 ? sp[Rand(c, 20) % std::size(sp)] : Rand(c, 24);
    }
}
inline void Vec3(u32 c, u32 k, u32* out, double lo, double hi) {   // modulo log-uniforme entre lo y hi
    double d[3];
    const double mag = std::pow(10.0, std::log10(lo) + double(Rand(c, k) % 2001) / 2000.0 * (std::log10(hi) - std::log10(lo)));
    Split3(c, k + 1, mag, d);
    for (int i = 0; i < 3; ++i) out[i] = B(float(d[i]));
}
// Contador de manchas: un tercio de los casos fija el valor posterior al incremento en 5 (mancha grande) o en 2 (pequena) modulo 8, otro un valor cualquiera y el resto bordes
inline u32 CounterFor(u32 c, u32 k) {
    static constexpr u32 edge[]{0u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 4u, 1u, 0xFFFFFFFCu, 0xFFFFFFF9u, 7u, 0x0FFFFFFFu};
    switch (Rand(c, k) % 6) {
    case 0: case 1: return (Rand(c, k + 1) & ~7u) | 4u;        // despues del incremento: 5
    case 2: return (Rand(c, k + 1) & ~7u) | 1u;                // despues: 2
    case 3: return Rand(c, k + 1);
    case 4: return edge[Rand(c, k + 2) % std::size(edge)];
    default: return Rand(c, k + 1) & 0xFFu;
    }
}

inline C MakeC(u32 fn, unsigned n) {
    C f{};
    f.fn = fn;
    f.family = n % kFamilies;
    const u32 c = n * 8u + 0x79003u + fn * 0x1000193u;
    f.seed = Rand(c, 6) | 1u;
    f.eax = SeedReg(c, 7); f.ecx = SeedReg(c, 8); f.edx = SeedReg(c, 9); f.stack = StackSeed(Rand(c, 11) % 12);
    f.thisKind = Rand(c, 3) % 4 == 0 ? 1u : 0u;
    f.hasMat = Rand(c, 1) % 5 < 3 ? 0u : 1u;
    f.amount = SmallAmount(c, 12, 6);
    f.light = Rand(c, 13) % 3 == 0 ? B(1.2f) : Rand(c, 14) % 2 ? RandFloat(c, 15, -4, 4) : B(0.0f);
    f.bloodOn = Rand(c, 5) % 16 == 0 ? 0u : 1u;
    f.counter0 = CounterFor(c, 25);
    f.tex = Rand(c, 26) % 4 == 0 ? (Rand(c, 27) % 3 == 0 ? 0u : 0xFFFFFFFFu) : Rand(c, 28);
    f.randMode = Rand(c, 16) % 8 == 0 ? 1u : 0u;
    f.swapSlot = Rand(c, 18) % 3 == 0 ? 1u : 0u;
    for (int i = 0; i < 3; ++i) f.other[i] = MixComponent(c, 20 + i);
    Vec3(c, 30, f.dir, 0.05, 6.0);
    auto camNormal = [&](int hi) { for (int i = 0; i < 3; ++i) f.cam[i] = RandFloat(c, 61 + i, -4, hi); };
    auto fromDelta = [&](const double d[3]) { for (int i = 0; i < 3; ++i) f.pos[i] = B(float(double(F(f.cam[i])) - d[i])); };
    auto inRange = [&](double hi) { double d[3]; camNormal(11); Split3(c, 70, double(Rand(c, 73) % 2000) / 2000.0 * hi, d); fromDelta(d); };
    double d[3];
    switch (f.family) {
    case 0: inRange(10.0); break;                                                                                                           // cerca
    case 1: inRange(24.9); break;                                                                                                           // casi en el limite
    case 2: {                                                                                                                               // alrededor de 25 (625)
        camNormal(8); Split3(c, 70, 25.0 * (1.0 + (double(Rand(c, 73) % 2001) - 1000.0) * 1e-6), d); fromDelta(d);
        f.pos[Rand(c, 74) % 3] = Ulps(f.pos[Rand(c, 74) % 3], int(Rand(c, 75) % 7) - 3); break;
    }
    case 3: {                                                                                                                               // sumas exactas 625 y vecinas
        static constexpr int quads[][3] = {{25, 0, 0}, {7, 24, 0}, {15, 20, 0}, {24, 7, 0}, {20, 15, 0}, {0, 0, 25}, {12, 16, 15}, {9, 12, 20}, {24, 0, 0}, {26, 0, 0}, {0, 25, 0}, {16, 15, 12}};
        const auto& q = quads[Rand(c, 70) % std::size(quads)];
        const int perm[3][3] = {{0, 1, 2}, {1, 2, 0}, {2, 0, 1}};
        const auto& p = perm[Rand(c, 71) % 3];
        for (int i = 0; i < 3; ++i) {
            const double v = double(q[p[i]]) * ((Rand(c, 72 + i) & 1) ? -1.0 : 1.0);
            f.cam[i] = B(float(double(int(Rand(c, 80 + i) % 2001) - 1000)));
            f.pos[i] = B(float(double(F(f.cam[i])) - v));
        }
        break;
    }
    case 4: camNormal(11); for (int i = 0; i < 3; ++i) f.pos[i] = RandFloat(c, 90 + i, 5, 40); break;                                          // lejos
    case 5: inRange(20.0);                                                                                                                  // NaN (cualquier carga): no sale
            for (int j = 0; j < 1 + int(Rand(c, 56) % 2); ++j) { if (Rand(c, 57 + j) & 1) f.cam[Rand(c, 60 + j) % 3] = Pick(kNan, c, 62 + j); else f.pos[Rand(c, 60 + j) % 3] = Pick(kNan, c, 62 + j); }
            break;
    case 6: inRange(20.0);                                                                                                                  // infinitos
            for (int j = 0; j < 1 + int(Rand(c, 56) % 3); ++j) {
                const u32 inf = Pick(kInf, c, 62 + j); const unsigned axis = Rand(c, 60 + j) % 3; const u32 mode = Rand(c, 66 + j) % 4;
                if (mode == 0) f.cam[axis] = inf; else if (mode == 1) f.pos[axis] = inf; else if (mode == 2) { f.cam[axis] = inf; f.pos[axis] = inf; } else { f.cam[axis] = inf; f.pos[axis] = Neg(inf); }
            }
            break;
    case 7: for (int i = 0; i < 3; ++i) { f.cam[i] = Pick(kTiny, c, 61 + i); f.pos[i] = Pick(kTiny, c, 90 + i); }                           // ceros y desnormales
            if (Rand(c, 106) % 2) for (int i = 0; i < 3; ++i) f.dir[i] = Pick(kDenorm, c, 107 + i);                                          // con direccion desnormal: direccion * 0.5 + posicion desnormal solo se redondea una vez (registro x87)
            break;
    case 8: inRange(20.0); break;                                                                                                           // cantidad extrema / especial (ver abajo)
    case 9: inRange(20.0);                                                                                                                  // direccion especial
            for (int i = 0; i < 3; ++i) f.dir[i] = Rand(c, 100 + i) % 3 == 0 ? f.dir[i] : MixComponent(c, 103 + i);
            if (Rand(c, 106) % 3 == 0) {
                for (int i = 0; i < 3; ++i) f.dir[i] = Pick(kDenorm, c, 107 + i);
                if (Rand(c, 110) % 2) for (int i = 0; i < 3; ++i) { f.cam[i] = Pick(kTiny, c, 111 + i); f.pos[i] = Pick(kTiny, c, 114 + i); }      // y camara/posicion desnormales
            }
            break;
    case 10: inRange(20.0);                                                                                                                 // luz especial
             f.light = Rand(c, 99) % 4 == 0 ? Pick(kNan, c, 100) : Rand(c, 99) % 4 == 1 ? Pick(kInf, c, 101) : Rand(c, 99) % 4 == 2 ? Pick(kTiny, c, 102) : MixComponent(c, 103);
             break;
    case 11: inRange(20.0); f.randMode = 1; f.amount = Rand(c, 138) % 3 ? SmallAmount(c, 12, 4) : NegAmount(c, 139); break;                    // sorteos forzados a los bordes (incluye los de la mancha)
    case 12: inRange(20.0);                                                                                                                 // contador (manchas) y textura
             f.counter0 = Rand(c, 150) % 2 ? ((Rand(c, 151) & ~7u) | (Rand(c, 152) % 2 ? 4u : 1u)) : CounterFor(c, 153);
             f.amount = Rand(c, 154) % 3 ? SmallAmount(c, 12, 3) : NegAmount(c, 155);
             break;
    case 13: inRange(20.0);                                                                                                                 // sin sangre y cambio de sistema
             f.bloodOn = Rand(c, 160) % 2; f.swapSlot = Rand(c, 135) % 2; f.amount = SmallAmount(c, 12, 8);
             break;
    case 14: {                                                                                                                              // coordenadas enormes o minusculas con la camara cerca; direccion pequena o grande
             const bool big = Rand(c, 170) % 2;
             for (int i = 0; i < 3; ++i) f.cam[i] = RandFloat(c, 61 + i, big ? 60 : -40, big ? 120 : -10);
             Split3(c, 70, double(Rand(c, 73) % 1000) / 1000.0 * 24.0, d);
             for (int i = 0; i < 3; ++i) f.pos[i] = Rand(c, 171 + i) % 3 == 0 ? f.cam[i] : B(float(double(F(f.cam[i])) - d[i]));
             if (Rand(c, 175) % 2) for (int i = 0; i < 3; ++i) f.dir[i] = RandFloat(c, 176 + i, -40, 120);
             break;
    }
    default:                                                                                                                                // 15: todo mezclado
        camNormal(7);
        for (int i = 0; i < 3; ++i) {
            f.pos[i] = Rand(c, 140 + i) % 4 == 0 ? MixComponent(c, 143 + i) : B(float(double(F(f.cam[i])) + (double(Rand(c, 150 + i) % 6001) - 3000.0) / 100.0));
            if (Rand(c, 153 + i) % 7 == 0) f.cam[i] = MixComponent(c, 156 + i);
            if (Rand(c, 159 + i) % 6 == 0) f.dir[i] = MixComponent(c, 162 + i);
        }
        if (Rand(c, 171) % 4 == 0) f.light = MixComponent(c, 172);
        f.randMode = Rand(c, 16) % 4 == 0 ? 1u : 0u;
        break;
    }
    // Cantidad: el reparto depende de la familia (acotada: AddBlood hace cuatro sorteos por vuelta)
    if (f.family == 8) {
        f.amount = Rand(c, 12) % 3 == 0 ? NegAmount(c, 12) : (Rand(c, 12) % 3 == 1 ? SmallAmount(c, 13, 3) : (Rand(c, 14) % 2 ? 5u + Rand(c, 13) % 20u : 100u + Rand(c, 13) % 300u));
    } else if (f.family == 1 || f.family == 3 || f.family == 15) {
        f.amount = Rand(c, 12) % 8 == 0 ? NegAmount(c, 14) : SmallAmount(c, 12, 6);
    }
    ApplyRand(f, c);
    return f;
}
inline std::string JsonC(const C& c) {
    Js j; j.U("fn", c.fn).U("family", c.family).U("seed", c.seed).U("this_kind", c.thisKind).U("has_mat", c.hasMat).U("amount", c.amount).U("light", c.light)
     .U("blood_on", c.bloodOn).U("counter0", c.counter0).U("tex", c.tex)
     .U("rand_mode", c.randMode).U("rand_seed", c.randSeed).U("rand_k", c.randK).U("swap_slot", c.swapSlot)
     .A("cam", V3(c.cam)).A("other", V3(c.other)).A("pos", V3(c.pos)).A("dir", V3(c.dir))
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}
}

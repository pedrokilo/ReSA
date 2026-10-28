#pragma once
// Fixtures del banco FxImpact78 (lote 78A): dos funciones de Fx_c del EXE, 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 Fx_c::AddSparks       0x49F040  thiscall, ECX = this (no se usa), (const CVector& origen, const CVector& direccion, float fuerza, int32 cantidad, CVector transversal por valor, eSparkType tipo (byte), float dispersion, float vida), `ret 28h`, void   (hook Fx_c / AddSparks, tipo S)
//   1 Fx_c::AddBulletImpact 0x49F3D0  thiscall, `this` no se usa, (const CVector& pos, const CVector& direccion, int32 superficie, int32 cantidad, float luz), `ret 14h`, void   (hook Fx_c / AddBulletImpact, tipo S)
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

namespace Fi78Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;
using i16 = std::int16_t;

constexpr unsigned kCases = 16384;        // por funcion: 16 familias de 1024
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 2;
enum Fn : u32 { kSparks = 0, kBullet = 1 };

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 78078u); }
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
constexpr u32 kVecWindow = 0x10, kVecArena = kGuard + kVecWindow + kGuard;                  // 0x90   un vector (origen / direccion)
constexpr u32 kMatWindow = 0x40, kMatArena = kGuard + kMatWindow + kGuard;                  // 0xC0   matriz de la camara (posicion en +0x30)
constexpr u32 kSysWindow = 0x20, kSysArena = kGuard + kSysWindow + kGuard;                  // 0xA0   sistemas de particulas falsos
constexpr u32 kThisWindow = 0x70, kThisArena = kGuard + kThisWindow + kGuard;               // 0xF0   objeto `this` (no se usa)
static_assert(kVecArena == 0x90 && kMatArena == 0xC0 && kSysArena == 0xA0 && kThisArena == 0xF0, "arenas");
constexpr u32 kCamPlacement = 0xB6F02C, kCamMatrixPtr = 0xB6F03C, kCamBase = 0xB6F028, kCamBytes = 0x20;   // TheCamera: m_placement (+4) y m_matrix (+0x14)
constexpr u32 kFxBase = 0xA9AE00, kFxBytes = 0x48;                                         // g_fx: punteros a sistemas (+0 .. +0x44)
constexpr u32 kSlotSand = 0xA9AE18, kSlotSmoke = 0xA9AE24, kSlotSpark = 0xA9AE28, kSlotSpark2 = 0xA9AE2C;
constexpr u32 kFrameCounter = 0xB7CB4C, kTimeStep = 0xB7CB5C;                              // CTimer::m_FrameCounter y CTimer::ms_fTimeStep
constexpr u32 kSurfaceInfos = 0xB79538;                                                    // g_surfaceInfos (ECX de GetBulletFx)

// ------------------------------------------------------------------------------------------------ el caso
// rand() es el REAL del CRT del EXE (0x821B1E). El estado inicial de cada lado es `randSeed`. randMode: 0 = estado cualquiera; 1 = el sorteo numero `randK` vale un borde de kEdgeOut.
struct C {
    u32 fn{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 thisKind{};    // 0 arena, 1 g_fx real, 2 puntero basura, 3 nulo
    u32 hasMat{};      // la camara tiene matriz (posicion +0x30) o solo posicion compacta (+4)
    u32 amount{};      // int32 con signo
    u32 light{};       // AddBulletImpact: arg4 (luz)
    u32 randMode{}, randSeed{}, randK{};
    u32 swapSlot{};    // el falso de AddParticle cambia el sistema entre dos entre llamadas (el EXE lo relee en cada vuelta)
    u32 cam[3]{}, other[3]{}, pos[3]{};
    u32 dir[3]{};      // vector direccion (arena)
    u32 force{}, spread{}, life{}, across[3]{};
    u32 stype{};       // dword de pila del argumento eSparkType (byte bajo = valor; el resto basura)
    u32 frame{};       // CTimer::m_FrameCounter
    u32 timestep{};    // CTimer::ms_fTimeStep
    u32 kind{};        // AddBulletImpact: lo que devuelve SurfaceInfos_c::GetBulletFx
    u32 surface{};     // AddBulletImpact: id de superficie que se pasa (dword)
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
inline u32 EdgeAmount(u32 n, u32 k) { static constexpr u32 v[]{6u, 7u, 8u, 9u, 0x7FFFFFFFu, 0x7FFFFFFEu, 0x01000001u, 0x01000000u, 0x00FFFFFFu, 0x02000003u, 0x40000003u, 15u, 16u, 1u, 2u, 3u}; return v[Rand(n, k) % std::size(v)]; }

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
inline u32 DrawBudget(const C& f) {
    if (f.fn != kSparks) return 1;
    const i32 amount = static_cast<i32>(f.amount);
    return 3 * (amount > 0 ? (amount < 3 ? u32(amount) : 3u) : 1u);
}
inline void ApplyRand(C& f, u32 c) {
    if (f.randMode == 1 && f.fn == kSparks) {
        const u32 budget = DrawBudget(f);
        f.randK = Rand(c, 17) % budget;
        f.randSeed = SeedForDraw(f.randK, kEdgeOut[Rand(c, 21) % kEdgeOut.size()], Rand(c, 22), Rand(c, 23));
    } else {
        f.randMode = 0;
        static constexpr u32 sp[]{0u, 1u, 0x80000000u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x269EC3u};
        f.randSeed = Rand(c, 19) % 12 == 0 ? sp[Rand(c, 20) % std::size(sp)] : Rand(c, 24);
    }
}
inline u32 StepFloat(u32 c, u32 k) { static constexpr float v[]{1.0f, 0.5f, 2.0f, 0.2f, 3.0f, 1.5f, 0.75f, 0.01f, 0.0f, 4.0f}; return Rand(c, k) % 3 == 0 ? RandFloat(c, k + 1, -4, 2) & 0x7FFFFFFFu : B(v[Rand(c, k + 2) % std::size(v)]); }
inline void Vec3(u32 c, u32 k, u32* out, double lo, double hi) {   // modulo log-uniforme entre lo y hi
    double d[3];
    const double mag = std::pow(10.0, std::log10(lo) + double(Rand(c, k) % 2001) / 2000.0 * (std::log10(hi) - std::log10(lo)));
    Split3(c, k + 1, mag, d);
    for (int i = 0; i < 3; ++i) out[i] = B(float(d[i]));
}

inline C MakeC(u32 fn, unsigned n) {
    C f{};
    f.fn = fn;
    f.family = n % kFamilies;
    const u32 c = n * 8u + 0x78003u + fn * 0x1000193u;
    f.seed = Rand(c, 6) | 1u;
    f.eax = SeedReg(c, 7); f.ecx = SeedReg(c, 8); f.edx = SeedReg(c, 9); f.stack = StackSeed(Rand(c, 11) % 12);
    f.thisKind = Rand(c, 3) % 4;
    f.hasMat = Rand(c, 1) % 5 < 3 ? 0u : 1u;
    f.amount = SmallAmount(c, 12, 6);
    f.light = Rand(c, 13) % 3 == 0 ? B(1.2f) : Rand(c, 14) % 2 ? RandFloat(c, 15, -4, 4) : B(0.0f);
    f.randMode = Rand(c, 16) % 8 == 0 ? 1u : 0u;
    f.swapSlot = Rand(c, 18) % 3 == 0 ? 1u : 0u;
    for (int i = 0; i < 3; ++i) f.other[i] = MixComponent(c, 20 + i);
    Vec3(c, 30, f.dir, 0.05, 6.0);
    f.force = Rand(c, 34) % 4 == 0 ? B(3.0f) : RandFloat(c, 35, -3, 3);
    f.spread = Rand(c, 36) % 4 == 0 ? B(0.4f) : Rand(c, 37) % 4 == 0 ? B(0.0f) : RandFloat(c, 38, -6, 1) & 0x7FFFFFFFu;
    f.life = Rand(c, 39) % 4 == 0 ? B(1.0f) : RandFloat(c, 40, -5, 2) & 0x7FFFFFFFu;
    Vec3(c, 41, f.across, 0.01, 4.0);
    if (Rand(c, 44) % 4 == 0) for (int i = 0; i < 3; ++i) f.across[i] = 0u;
    f.stype = Rand(c, 45) % 4 == 0 ? 0u : 1u;
    if (Rand(c, 46) % 4 == 0) f.stype |= (Rand(c, 47) & 0xFFFFFF00u);
    f.frame = Rand(c, 43);
    f.timestep = StepFloat(c, 48);
    f.kind = 1 + Rand(c, 49) % 4;
    f.surface = Rand(c, 50) % 8 == 0 ? Rand(c, 51) : Rand(c, 51) % 180;
    auto camNormal = [&](int hi) { for (int i = 0; i < 3; ++i) f.cam[i] = RandFloat(c, 61 + i, -4, hi); };
    auto fromDelta = [&](const double d[3]) { for (int i = 0; i < 3; ++i) f.pos[i] = B(float(double(F(f.cam[i])) - d[i])); };
    auto inRange = [&](double hi) { double d[3]; camNormal(11); Split3(c, 70, double(Rand(c, 73) % 2000) / 2000.0 * hi, d); fromDelta(d); };
    // Para que la compuerta de los fotogramas pares (a mas de 15) deje pasar a buena parte de los casos
    auto frameEven = [&](u32 pct) { if (Rand(c, 65) % 100 < pct) f.frame &= ~1u; else if (Rand(c, 66) % 3 == 0) f.frame |= 1u; };
    double d[3];
    switch (f.family) {
    case 0: inRange(14.9); break;                                                                                                           // a menos de 15: sin compuerta de fotograma
    case 1: inRange(149.0); frameEven(70); break;                                                                                           // entre 15 y 150
    case 2: {                                                                                                                               // alrededor de 15 (225)
        camNormal(8); Split3(c, 70, 15.0 * (1.0 + (double(Rand(c, 73) % 2001) - 1000.0) * 1e-6), d); fromDelta(d);
        f.pos[Rand(c, 74) % 3] = Ulps(f.pos[Rand(c, 74) % 3], int(Rand(c, 75) % 7) - 3); frameEven(50); break;
    }
    case 3: {                                                                                                                               // alrededor de 150 (22500)
        camNormal(8); Split3(c, 70, 150.0 * (1.0 + (double(Rand(c, 73) % 2001) - 1000.0) * 1e-6), d); fromDelta(d);
        f.pos[Rand(c, 74) % 3] = Ulps(f.pos[Rand(c, 74) % 3], int(Rand(c, 75) % 7) - 3); frameEven(70); break;
    }
    case 4: {                                                                                                                               // sumas exactas 225 y 22500
        static constexpr int quads[][3] = {{15, 0, 0}, {0, 9, 12}, {9, 12, 0}, {150, 0, 0}, {90, 120, 0}, {0, 0, 150}, {0, 120, 90}, {12, 0, 9}, {10, 10, 5}, {100, 100, 50}, {14, 0, 0}, {151, 0, 0}};
        const auto& q = quads[Rand(c, 70) % std::size(quads)];
        const int perm[3][3] = {{0, 1, 2}, {1, 2, 0}, {2, 0, 1}};
        const auto& p = perm[Rand(c, 71) % 3];
        for (int i = 0; i < 3; ++i) {
            const double v = double(q[p[i]]) * ((Rand(c, 72 + i) & 1) ? -1.0 : 1.0);
            f.cam[i] = B(float(double(int(Rand(c, 80 + i) % 2001) - 1000)));
            f.pos[i] = B(float(double(F(f.cam[i])) - v));
        }
        frameEven(50); break;
    }
    case 5: camNormal(11); for (int i = 0; i < 3; ++i) f.pos[i] = RandFloat(c, 90 + i, 8, 40); break;                                          // lejos
    case 6: inRange(100.0);                                                                                                                 // NaN (cualquier carga): no sale
            for (int j = 0; j < 1 + int(Rand(c, 56) % 2); ++j) { if (Rand(c, 57 + j) & 1) f.cam[Rand(c, 60 + j) % 3] = Pick(kNan, c, 62 + j); else f.pos[Rand(c, 60 + j) % 3] = Pick(kNan, c, 62 + j); }
            break;
    case 7: inRange(100.0);                                                                                                                 // infinitos
            for (int j = 0; j < 1 + int(Rand(c, 56) % 3); ++j) {
                const u32 inf = Pick(kInf, c, 62 + j); const unsigned axis = Rand(c, 60 + j) % 3; const u32 mode = Rand(c, 66 + j) % 4;
                if (mode == 0) f.cam[axis] = inf; else if (mode == 1) f.pos[axis] = inf; else if (mode == 2) { f.cam[axis] = inf; f.pos[axis] = inf; } else { f.cam[axis] = inf; f.pos[axis] = Neg(inf); }
            }
            break;
    case 8: for (int i = 0; i < 3; ++i) { f.cam[i] = Pick(kTiny, c, 61 + i); f.pos[i] = Pick(kTiny, c, 90 + i); } frameEven(70); break;           // ceros y desnormales
    case 9: inRange(100.0); frameEven(70); break;                                                                                           // cantidad extrema / especial (ver abajo)
    case 10: inRange(100.0); frameEven(80);                                                                                                 // direccion especial
             for (int i = 0; i < 3; ++i) f.dir[i] = Rand(c, 100 + i) % 3 == 0 ? f.dir[i] : MixComponent(c, 103 + i);
             if (Rand(c, 106) % 3 == 0) for (int i = 0; i < 3; ++i) f.dir[i] = Pick(kDenorm, c, 107 + i);
             break;
    case 11: inRange(100.0); frameEven(80);                                                                                                 // luz especial / dispersion, fuerza y vida especiales
             f.light = Rand(c, 99) % 4 == 0 ? Pick(kNan, c, 100) : Rand(c, 99) % 4 == 1 ? Pick(kInf, c, 101) : Rand(c, 99) % 4 == 2 ? Pick(kTiny, c, 102) : MixComponent(c, 103);
             if (Rand(c, 110) % 2) f.spread = MixComponent(c, 111);
             if (Rand(c, 112) % 2) f.force = MixComponent(c, 113);
             if (Rand(c, 114) % 2) f.life = MixComponent(c, 115);
             if (Rand(c, 116) % 4 == 0) { f.spread = RandFloat(c, 117, 125, 127) & 0x7FFFFFFFu; f.force = RandFloat(c, 118, -4, -1); }   // rango 2*dispersion fuera del rango float y fuerza pequena: vz queda finito en el registro x87
             else if (Rand(c, 119) % 3 == 0) { f.spread = Rand(c, 120) & 0x007FFFFFu; f.force = RandFloat(c, 121, 0, 3) & 0x7FFFFFFFu; f.dir[2] = Pick(kTiny, c, 122); }   // dispersion desnormal y fuerza grande: vz es desnormal como float pero no en el registro x87
             break;
    case 12: inRange(100.0); frameEven(80);                                                                                                 // transversal y paso de tiempo especiales
             for (int i = 0; i < 3; ++i) if (Rand(c, 120 + i) % 2) f.across[i] = MixComponent(c, 123 + i);
             f.timestep = Rand(c, 126) % 4 == 0 ? Pick(kNan, c, 127) : Rand(c, 126) % 4 == 1 ? Pick(kInf, c, 128) : Rand(c, 126) % 4 == 2 ? Pick(kDenorm, c, 129) : MixComponent(c, 130);
             break;
    case 13: inRange(100.0); frameEven(80);                                                                                                 // tipo de chispa (byte), cambio de sistema, superficie basura
             f.stype = Rand(c, 131) % 3 == 0 ? Rand(c, 132) : (Rand(c, 131) % 3 == 1 ? Rand(c, 133) % 4 : (Rand(c, 134) & 0xFFFFFF00u));
             f.swapSlot = Rand(c, 135) % 2; f.surface = Rand(c, 136);
             break;
    case 14: inRange(100.0); frameEven(80); f.randMode = Rand(c, 16) % 2; f.amount = Rand(c, 138) % 3 ? SmallAmount(c, 12, 12) : 1u; break;     // sorteos en los bordes
    default:                                                                                                                                // 15: todo mezclado
        camNormal(7);
        for (int i = 0; i < 3; ++i) {
            f.pos[i] = Rand(c, 140 + i) % 4 == 0 ? MixComponent(c, 143 + i) : B(float(double(F(f.cam[i])) + (double(Rand(c, 150 + i) % 6001) - 3000.0) / 100.0));
            if (Rand(c, 153 + i) % 7 == 0) f.cam[i] = MixComponent(c, 156 + i);
            if (Rand(c, 159 + i) % 6 == 0) f.dir[i] = MixComponent(c, 162 + i);
            if (Rand(c, 165 + i) % 6 == 0) f.across[i] = MixComponent(c, 168 + i);
        }
        if (Rand(c, 171) % 4 == 0) f.light = MixComponent(c, 172);
        if (Rand(c, 173) % 4 == 0) f.spread = MixComponent(c, 174);
        if (Rand(c, 175) % 4 == 0) f.life = MixComponent(c, 176);
        if (Rand(c, 177) % 4 == 0) f.force = MixComponent(c, 178);
        if (Rand(c, 179) % 4 == 0) f.timestep = MixComponent(c, 180);
        f.randMode = Rand(c, 16) % 4 == 0 ? 1u : 0u; frameEven(50);
        break;
    }
    // Cantidad: el reparto depende de la funcion y de la familia
    if (f.family == 9) {
        f.amount = fn == kSparks ? (Rand(c, 12) % 3 == 0 ? NegAmount(c, 12) : (Rand(c, 12) % 3 == 1 ? SmallAmount(c, 13, 3) : (Rand(c, 14) % 2 ? 5u + Rand(c, 13) % 20u : 100u + Rand(c, 13) % 300u)))
                                  : (Rand(c, 12) % 2 ? EdgeAmount(c, 14) : NegAmount(c, 15));
    } else if (fn == kBullet) {
        f.amount = Rand(c, 12) % 5 == 0 ? NegAmount(c, 14) : Rand(c, 12) % 5 == 1 ? EdgeAmount(c, 15) : Rand(c, 16) % 3 ? SmallAmount(c, 17, 14) : Rand(c, 17);
    } else if (f.family == 1 || f.family == 3 || f.family == 15) {
        f.amount = Rand(c, 12) % 8 == 0 ? NegAmount(c, 14) : SmallAmount(c, 12, 6);
    }
    if (fn == kBullet) {
        // tipo de impacto (lo que devuelve GetBulletFx): las cuatro primeras familias fijan el tipo 1, 2, 4 y 3; la 13, la mitad de las veces tipos que no hacen nada; el resto, mezcla
        static constexpr u32 other[]{0u, 5u, 6u, 7u, 255u, 0x80000000u, 0xFFFFFFFFu, 0x100u, 0x101u, 0x102u, 0x103u, 0x104u};
        switch (f.family) {
        case 0: f.kind = 1; break;
        case 1: f.kind = 2; break;
        case 2: f.kind = 4; break;
        case 3: f.kind = 3; break;
        case 13: f.kind = Rand(c, 193) % 2 ? other[Rand(c, 190) % std::size(other)] : 1 + Rand(c, 49) % 4; break;
        default: f.kind = 1 + Rand(c, 49) % 4; if (Rand(c, 191) % 24 == 0) f.kind = other[Rand(c, 192) % std::size(other)]; break;
        }
    }
    ApplyRand(f, c);
    return f;
}
inline std::string JsonC(const C& c) {
    Js j; j.U("fn", c.fn).U("family", c.family).U("seed", c.seed).U("this_kind", c.thisKind).U("has_mat", c.hasMat).U("amount", c.amount).U("light", c.light)
     .U("rand_mode", c.randMode).U("rand_seed", c.randSeed).U("rand_k", c.randK).U("swap_slot", c.swapSlot)
     .A("cam", V3(c.cam)).A("other", V3(c.other)).A("pos", V3(c.pos)).A("dir", V3(c.dir)).U("force", c.force).U("spread", c.spread).U("life", c.life).A("across", V3(c.across))
     .U("stype", c.stype).U("frame", c.frame).U("timestep", c.timestep).U("kind", c.kind).U("surface", c.surface)
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}
}

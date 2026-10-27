#pragma once
// Fixtures del banco FxWood73 (lote 73): Fx_c::AddWood (EXE 0x49EE10), 16384 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 Fx_c::AddWood  0x49EE10  thiscall, ECX = this (ignorado), (const CVector& pos, const CVector& direction, int32 amount, float lightMult), `ret 10h`, void   (hook Fx_c / AddWood, tipo S)
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

namespace Fw73Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;   // familias 0..15 (1024 cada una)
constexpr unsigned kHugeCases = 28;      // familia 16 (2026-10-27): 65535 (8), 65536 (8), 100000 (8) y 1000000 (4) vueltas; se mide el tiempo por vuelta
constexpr unsigned kCases = kBaseCases + kHugeCases;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 73073u); }
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
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0xFFC00001u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7F800001u, 0x7FA00000u, 0xFFA00000u, 0xFF800001u};
inline constexpr std::array<u32, 8> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u};
// Desnormales (y cercanos al limite normal) para la direccion: *4 puede dejarlos desnormales, y el desplazamiento aleatorio exacto 0 (resto 5000) los conserva
inline constexpr std::array<u32, 12> kDenorm{1u, 0x80000001u, 2u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x001FFFFFu, 0x801FFFFFu, 0x00200000u, 0x00200001u, 0x80200001u};
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 Neg(u32 b) { return b ^ 0x80000000u; }
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
constexpr u32 kVecWindow = 0x10, kVecArena = kGuard + kVecWindow + kGuard;                  // 0x90   un vector (pos, direccion)
constexpr u32 kMatWindow = 0x40, kMatArena = kGuard + kMatWindow + kGuard;                  // 0xC0   matriz de la camara (posicion en +0x30)
constexpr u32 kSysWindow = 0x20, kSysArena = kGuard + kSysWindow + kGuard;                  // 0xA0   sistemas de particulas falsos (g_fx.m_Blood)
constexpr u32 kEntWindow = 0x40, kEntArena = kGuard + kEntWindow + kGuard;                  // 0xC0   objeto `this` (ignorado)
static_assert(kVecArena == 0x90 && kMatArena == 0xC0 && kSysArena == 0xA0 && kEntArena == 0xC0, "arenas");
constexpr u32 kCamPlacement = 0xB6F02C, kCamMatrixPtr = 0xB6F03C, kCamBase = 0xB6F028, kCamBytes = 0x20;   // TheCamera: m_placement (+4) y m_matrix (+0x14)
constexpr u32 kBloodSlot = 0xA9AE00;                                                       // g_fx.m_Blood (el sistema que recibe las particulas)

// ------------------------------------------------------------------------------------------------ el caso
// rand() es el REAL del CRT del EXE (0x821B1E: seed = seed * 0x343FD + 0x269EC3, devuelve (seed >> 16) & 0x7FFF; la semilla esta en getptd() + 0x14). El estado inicial de cada lado es `randSeed`.
// Modos: 0 = estado inicial cualquiera (a veces 0, 1, 0x80000000, 0xFFFFFFFF); 1 = el sorteo numero `randK` vale un borde (0, 1, 4999, 5000, 9999, 10000, 19999, 20000, 29999, 30000, 32767);
// 2 = los sorteos `randK` y `randK`+1 (desplazamientos consecutivos de la velocidad, x/y o y/z de una vuelta) valen 5000, 15000 o 25000 (resto 5000: desplazamiento exactamente 0);
// 3 = solo el sorteo `randK` (un desplazamiento de la velocidad) vale 5000, 15000 o 25000. El estado inicial se obtiene invirtiendo la congruencia lineal (exacto, sin bucles de busqueda en el caso 1 y 3).
struct C {
    u32 family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 thisKind{};    // 0 arena, 1 g_fx real, 2 puntero basura, 3 nulo
    u32 hasMat{};      // la camara tiene matriz (posicion +0x30) o solo posicion compacta (+4)
    u32 amount{};      // int32 con signo
    u32 light{};       // lightMult (patron de bits)
    u32 randMode{}, randSeed{}, randK{};   // randSeed = estado INICIAL del generador del CRT del EXE (hebra +0x14); randK = indice del sorteo forzado (modos 1..3)
    u32 swapSlot{};    // el falso de AddParticle cambia g_fx.m_Blood entre dos sistemas tras cada llamada (el EXE lo relee en cada vuelta)
    u32 cam[3]{}, other[3]{}, pos[3]{}, dir[3]{};
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
constexpr u32 LcgOut(u32 s) { return (s >> 16) & 0x7FFFu; }
// Estado inicial tal que el sorteo numero k (0 = el primero) devuelve `out`; low/top16 completan los 17 bits que el sorteo no ve (bits 0..15 y 31)
inline u32 SeedForDraw(u32 k, u32 out, u32 low16, u32 bit31) {
    u32 s = (out << 16) | (low16 & 0xFFFFu) | ((bit31 & 1u) << 31);
    for (u32 i = 0; i <= k; ++i) s = LcgBack(s);
    return s;
}
// Estados s tales que out(s) y out(next(s)) valen 5000/15000/25000 (36 aprox.): una sola busqueda de 393216 candidatos
inline const std::vector<u32>& ZeroPairStates() {
    static const std::vector<u32> v = [] {
        std::vector<u32> r;
        for (u32 hi : {5000u, 15000u, 25000u}) for (u32 b = 0; b < 2; ++b) for (u32 low = 0; low < 65536; ++low) {
            const u32 s = (hi << 16) | (b << 31) | low;
            const u32 o = LcgOut(LcgNext(s));
            if (o == 5000u || o == 15000u || o == 25000u) r.push_back(s);
        }
        return r;
    }();
    return v;
}
inline constexpr std::array<u32, 11> kEdgeOut{0u, 1u, 4999u, 5000u, 9999u, 10000u, 19999u, 20000u, 29999u, 30000u, 32767u};
// Fija randSeed/randK segun randMode (despues de decidir `amount`)
inline void ApplyRand(C& f, u32 c) {
    const i32 amount = static_cast<i32>(f.amount);
    const u32 iters = amount > 0 ? static_cast<u32>(amount) : 1u;
    const u32 maxIter = iters < 3u ? iters : 3u;   // los sorteos forzados caen en las primeras vueltas
    if (amount <= 0 && f.randMode != 0) f.randMode = 0;
    switch (f.randMode) {
    case 1: {
        f.randK = Rand(c, 17) % (7u * maxIter);
        f.randSeed = SeedForDraw(f.randK, kEdgeOut[Rand(c, 21) % kEdgeOut.size()], Rand(c, 22), Rand(c, 23));
        break;
    }
    case 2: {
        const auto& v = ZeroPairStates();
        f.randK = (Rand(c, 17) % 2 ? 4u : 5u) + 7u * (Rand(c, 18) % maxIter);
        u32 s = v[Rand(c, 21) % v.size()];
        for (u32 i = 0; i <= f.randK; ++i) s = LcgBack(s);
        f.randSeed = s;
        break;
    }
    case 3: {
        f.randK = 4u + Rand(c, 17) % 3u + 7u * (Rand(c, 18) % maxIter);
        f.randSeed = SeedForDraw(f.randK, 5000u + 10000u * (Rand(c, 21) % 3), Rand(c, 22), Rand(c, 23));
        break;
    }
    default: {
        f.randMode = 0;
        static constexpr u32 sp[]{0u, 1u, 0x80000000u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x269EC3u};
        f.randSeed = Rand(c, 19) % 12 == 0 ? sp[Rand(c, 20) % std::size(sp)] : Rand(c, 24);
        break;
    }
    }
}

inline u32 BigAmount(u32 c) {
    static constexpr u32 mid[]{31u, 64u, 100u, 255u, 256u};
    const u32 r = Rand(c, 12) % 100;
    if (r < 40) return 13u + Rand(c, 13) % 18u;
    if (r < 65) return mid[Rand(c, 13) % std::size(mid)];
    if (r < 90) return 1000u;
    if (r < 98) return 4096u;
    return 10000u;
}
inline C MakeHuge(unsigned n) {
    static constexpr u32 am[]{65535u, 65536u, 100000u, 1000000u};
    const u32 j = n - kBaseCases;
    C f{};
    f.family = 16;
    const u32 c = n * 8u + 0x73003u;
    f.seed = Rand(c, 6) | 1u;
    f.eax = SeedReg(c, 7); f.ecx = SeedReg(c, 8); f.edx = SeedReg(c, 9); f.stack = StackSeed(Rand(c, 11) % 12);
    f.thisKind = Rand(c, 3) % 4;
    f.hasMat = Rand(c, 1) % 2;
    f.amount = am[j < 8 ? 0 : j < 16 ? 1 : j < 24 ? 2 : 3];
    f.light = B(1.2f);
    f.swapSlot = j % 2;
    f.randMode = j % 4 == 3 ? 3u : 0u;
    for (int i = 0; i < 3; ++i) { f.other[i] = MixComponent(c, 20 + i); f.dir[i] = RandFloat(c, 36 + i, -6, 4); f.cam[i] = RandFloat(c, 41 + i, -4, 11); }
    double d[3];
    Split3(c, 50, double(Rand(c, 53) % 2000) / 100.0, d);
    for (int i = 0; i < 3; ++i) f.pos[i] = B(float(double(F(f.cam[i])) - d[i]));
    ApplyRand(f, c);
    return f;
}
inline C MakeC(unsigned n) {
    if (n >= kBaseCases) return MakeHuge(n);
    C f{};
    f.family = n % kFamilies;
    const u32 c = n * 8u + 0x73003u;
    f.seed = Rand(c, 6) | 1u;
    f.eax = SeedReg(c, 7); f.ecx = SeedReg(c, 8); f.edx = SeedReg(c, 9); f.stack = StackSeed(Rand(c, 11) % 12);
    f.thisKind = Rand(c, 3) % 4;
    f.hasMat = Rand(c, 1) % 5 < 3 ? 0u : 1u;
    f.amount = SmallAmount(c, 12, 4);
    f.light = Rand(c, 13) % 3 == 0 ? B(1.2f) : Rand(c, 14) % 2 ? RandFloat(c, 15, -4, 4) : B(0.0f);
    f.randMode = Rand(c, 16) % 8 == 0 ? 1u : 0u;
    f.swapSlot = Rand(c, 18) % 3 == 0 ? 1u : 0u;
    for (int i = 0; i < 3; ++i) {
        f.other[i] = MixComponent(c, 20 + i);
        f.dir[i] = Rand(c, 30 + i) % 4 == 0 ? MixComponent(c, 33 + i) : RandFloat(c, 36 + i, -6, 4);
    }
    auto camNormal = [&](int hi) { for (int i = 0; i < 3; ++i) f.cam[i] = RandFloat(c, 41 + i, -4, hi); };
    auto fromDelta = [&](const double d[3]) { for (int i = 0; i < 3; ++i) f.pos[i] = B(float(double(F(f.cam[i])) - d[i])); };
    double d[3];
    switch (f.family) {
    case 0: camNormal(11); Split3(c, 50, double(Rand(c, 53) % 2000) / 100.0, d); fromDelta(d); break;                                      // dentro de rango, amount 1..4
    case 1: camNormal(11); Split3(c, 50, double(Rand(c, 53) % 2000) / 100.0, d); fromDelta(d); f.amount = 5 + Rand(c, 12) % 8; f.randMode = Rand(c, 16) % 6 == 0 ? 1u : 0u; break;   // 5..12 vueltas
    case 2: camNormal(8); Split3(c, 50, 25.0 * (1.0 + (double(Rand(c, 53) % 2001) - 1000.0) * 1e-6), d); fromDelta(d);                     // alrededor de 625
            f.pos[Rand(c, 54) % 3] = Ulps(f.pos[Rand(c, 54) % 3], int(Rand(c, 55) % 7) - 3); f.amount = SmallAmount(c, 12, 3); break;
    case 3: {                                                                                                                               // suma exactamente 625
        static constexpr int quads[][3] = {{25, 0, 0}, {0, 7, 24}, {0, 15, 20}, {12, 15, 16}, {7, 24, 0}, {15, 20, 0}, {20, 0, 15}, {24, 0, 7}, {0, 20, 15}, {16, 12, 15}};
        const auto& q = quads[Rand(c, 50) % std::size(quads)];
        const int perm[3][3] = {{0, 1, 2}, {1, 2, 0}, {2, 0, 1}};
        const auto& p = perm[Rand(c, 51) % 3];
        for (int i = 0; i < 3; ++i) {
            const double v = double(q[p[i]]) * ((Rand(c, 52 + i) & 1) ? -1.0 : 1.0);
            f.cam[i] = B(float(double(int(Rand(c, 60 + i) % 2001) - 1000)));
            f.pos[i] = B(float(double(F(f.cam[i])) - v));
        }
        if (Rand(c, 56) % 3 == 0) f.pos[Rand(c, 57) % 3] = Ulps(f.pos[Rand(c, 57) % 3], Rand(c, 58) % 2 ? 1 : -1);
        f.amount = SmallAmount(c, 12, 3);
        break;
    }
    case 4: camNormal(11); for (int i = 0; i < 3; ++i) f.pos[i] = RandFloat(c, 70 + i, 10, 40); f.amount = SmallAmount(c, 12, 6); break;       // lejos
    case 5: camNormal(8); Split3(c, 50, 10.0, d); fromDelta(d);                                                                             // NaN (cualquier carga)
            for (int j = 0; j < 1 + int(Rand(c, 56) % 2); ++j) { if (Rand(c, 57 + j) & 1) f.cam[Rand(c, 60 + j) % 3] = Pick(kNan, c, 62 + j); else f.pos[Rand(c, 60 + j) % 3] = Pick(kNan, c, 62 + j); }
            break;
    case 6: camNormal(8); Split3(c, 50, 10.0, d); fromDelta(d);                                                                             // infinitos
            for (int j = 0; j < 1 + int(Rand(c, 56) % 3); ++j) {
                const u32 inf = Pick(kInf, c, 62 + j); const unsigned axis = Rand(c, 60 + j) % 3; const u32 mode = Rand(c, 66 + j) % 4;
                if (mode == 0) f.cam[axis] = inf; else if (mode == 1) f.pos[axis] = inf; else if (mode == 2) { f.cam[axis] = inf; f.pos[axis] = inf; } else { f.cam[axis] = inf; f.pos[axis] = Neg(inf); }
            }
            break;
    case 7: for (int i = 0; i < 3; ++i) { f.cam[i] = Pick(kTiny, c, 41 + i); f.pos[i] = Pick(kTiny, c, 70 + i); }                           // ceros y desnormales: en rango
            for (int i = 0; i < 3; ++i) if (Rand(c, 80 + i) % 3 == 0) f.dir[i] = Pick(kTiny, c, 83 + i);
            break;
    case 8: camNormal(11); Split3(c, 50, double(Rand(c, 53) % 2000) / 100.0, d); fromDelta(d); f.amount = NegAmount(c, 12); break;           // amount <= 0: ni un solo sorteo
    case 9: camNormal(11); Split3(c, 50, double(Rand(c, 53) % 2000) / 100.0, d); fromDelta(d); f.amount = SmallAmount(c, 12, 3);           // direccion especial
            for (int i = 0; i < 3; ++i) f.dir[i] = Rand(c, 90 + i) % 5 == 0 ? RandFloat(c, 93 + i, -6, 4) : MixComponent(c, 96 + i);
            break;
    case 10: camNormal(11); Split3(c, 50, double(Rand(c, 53) % 2000) / 100.0, d); fromDelta(d); f.amount = SmallAmount(c, 12, 4);          // desnormales de direccion y desplazamientos exactamente 0
            for (int i = 0; i < 3; ++i) f.dir[i] = Rand(c, 90 + i) % 6 == 0 ? Pick(kSpecial, c, 93 + i) : Pick(kDenorm, c, 96 + i);
            f.randMode = Rand(c, 16) % 2 ? 2u : 3u;
            break;
    case 11: camNormal(11); Split3(c, 50, double(Rand(c, 53) % 2000) / 100.0, d); fromDelta(d); f.amount = SmallAmount(c, 12, 3);          // lightMult especial
            f.light = Rand(c, 99) % 4 == 0 ? Pick(kNan, c, 100) : Rand(c, 99) % 4 == 1 ? Pick(kInf, c, 101) : Rand(c, 99) % 4 == 2 ? Pick(kTiny, c, 102) : MixComponent(c, 103);
            break;
    case 12: camNormal(11); Split3(c, 50, double(Rand(c, 53) % 2000) / 100.0, d); fromDelta(d); f.amount = SmallAmount(c, 12, 4);          // rand: bordes y enteros de 32 bits (restos con signo)
            f.randMode = 1u;
            break;
    case 13: camNormal(11); Split3(c, 50, double(Rand(c, 53) % 2000) / 100.0, d); fromDelta(d); f.amount = BigAmount(c);         // muchas vueltas (13..30, 31, 64, 100, 255, 256, 1000, 4096, 10000)
            f.randMode = Rand(c, 16) % 4 == 0 ? 1u : 0u;
            break;
    case 14: camNormal(11); Split3(c, 50, double(Rand(c, 53) % 3000) / 100.0, d); fromDelta(d);                                            // mezcla de rangos y generadores
            f.amount = Rand(c, 12) % 6 == 0 ? NegAmount(c, 14) : SmallAmount(c, 12, 8); f.randMode = Rand(c, 16) % 4;
            for (int i = 0; i < 3; ++i) if (Rand(c, 85 + i) % 5 == 0) f.dir[i] = Pick(kDenorm, c, 88 + i);
            break;
    default:                                                                                                                                // 15: todo mezclado
        camNormal(7);
        for (int i = 0; i < 3; ++i) {
            f.pos[i] = Rand(c, 70 + i) % 4 == 0 ? MixComponent(c, 75 + i) : B(float(double(F(f.cam[i])) + (double(Rand(c, 80 + i) % 6001) - 3000.0) / 100.0));
            if (Rand(c, 85 + i) % 7 == 0) f.cam[i] = MixComponent(c, 88 + i);
            if (Rand(c, 91 + i) % 4 == 0) f.dir[i] = Rand(c, 94 + i) % 2 ? Pick(kDenorm, c, 97 + i) : MixComponent(c, 98 + i);
        }
        f.amount = Rand(c, 12) % 8 == 0 ? NegAmount(c, 14) : SmallAmount(c, 12, 6);
        f.randMode = Rand(c, 16) % 4;
        if (Rand(c, 19) % 4 == 0) f.light = MixComponent(c, 104);
        break;
    }
    ApplyRand(f, c);
    return f;
}
inline std::string JsonC(const C& c) {
    Js j; j.U("family", c.family).U("seed", c.seed).U("this_kind", c.thisKind).U("has_mat", c.hasMat).U("amount", c.amount).U("light", c.light).U("rand_mode", c.randMode).U("rand_seed", c.randSeed).U("rand_k", c.randK).U("swap_slot", c.swapSlot)
     .A("cam", V3(c.cam)).A("other", V3(c.other)).A("pos", V3(c.pos)).A("dir", V3(c.dir)).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}
}

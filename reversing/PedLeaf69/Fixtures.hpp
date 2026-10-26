#pragma once
// Fixtures del banco PedLeaf69 (lote 69, cuatro hojas sin llamadores en el EXE): 4096 casos por funcion. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / Cases.hpp / LiveComparison.hpp y fixture_dump.cpp.
//
//   0 CPedIntelligence::IsInHearingRange             0x600C00  thiscall, ECX = this, (const CVector*), `ret 4`, AL = rango^2 > (dz^2 + dx^2) + dy^2   (hook CPedIntelligence / IsInHearingRange)
//   1 CPedIntelligence::HasInterestingEntites        0x602080  thiscall, ECX = this, sin argumentos, `ret`, AL = alguna de las 3 entidades (+0x288) no nula   (hook CPedIntelligence / HasInterestingEntites)
//   2 CPedIntelligence::RemoveAllInterestingEntities 0x602320  thiscall, ECX = this, sin argumentos, `ret`, void; colaborador CEntity::CleanUpOldReference 0x571A00   (hook CPedIntelligence / RemoveAllInterestingEntities)
//   3 CPedStats::FindIndexWithPedStat                0x608940  cdecl, (void*), EAX = indice de la entrada de 0x34 bytes de ms_apPedStats (0xC0BBEC) igual al puntero, o 0   (hook CPedStats / FindIndexWithPedStat)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (256 casos por familia y funcion).
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

namespace Pl69Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 4;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline u32 FBits(float f) { return std::bit_cast<u32>(f); }
inline float BitsF(u32 b) { return std::bit_cast<float>(b); }
inline float F(u32 b) { return std::bit_cast<float>(b); }
inline u32 B(float f) { return std::bit_cast<u32>(f); }

// ------------------------------------------------------------------------------------------------ JSON minimo (mismas filas `input` en el juego y en cualquier espejo)
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

// ------------------------------------------------------------------------------------------------ valores notables
inline constexpr std::array<u32, 28> kSpecial{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x41C80000u, 0xC1C80000u, 0x40000000u, 0x3E800000u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F7FFFFFu, 0x3F800001u, 0x34000000u, 0x33800000u, 0x4B800000u, 0x7F000000u,
    0x7E800000u, 0x5F800000u, 0x1F800000u, 0x20000000u, 0x441C4000u, 0x441C4001u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0xFFC00001u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7F800001u, 0x7FA00000u, 0xFFA00000u, 0xFF800001u};
inline constexpr std::array<u32, 6> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu};
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 Neg(u32 b) { return b ^ 0x80000000u; }
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
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
// Mueve un float finito `k` unidades en la ultima posicion (orden de los bits de magnitud), conservando el signo.
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
// Resultado de bool de un colaborador: falso = AL 0 con basura en los bits altos, o AL != 0 con basura en los bits altos
inline u32 BoolRet(u32 n, u32 k, bool wantTrue) {
    static constexpr u32 als[]{1u, 2u, 0x80u, 0xFFu, 0x7Fu, 0x81u};
    if (!wantTrue) return Rand(n, k + 1) % 2 ? 0u : (Rand(n, k + 2) & 0xFFFFFF00u);
    return (Rand(n, k + 3) % 2 ? 0u : (Rand(n, k + 4) & 0xFFFFFF00u)) | als[Rand(n, k + 5) % std::size(als)];
}
inline float Bf(double d) { return float(d); }
inline u32 Fb(double d) { return B(float(d)); }

// ------------------------------------------------------------------------------------------------ arenas (suma a mano: guarda + ventana + guarda)
constexpr u32 kGuard = 0x40;
constexpr u32 kIntelWindow = 0x2C0, kIntelArena = kGuard + kIntelWindow + kGuard;               // 0x40 + 0x2C0 + 0x40 = 0x340  CPedIntelligence (m_pPed +0, rango oido +0xBC, entidades +0x288..+0x293)
constexpr u32 kEntWindow = 0x40, kEntArena = kGuard + kEntWindow + kGuard;                      // 0x40 + 0x40 + 0x40 = 0xC0    entidad (matriz en +0x14, posicion en +4)
constexpr u32 kMatWindow = 0x40, kMatArena = kGuard + kMatWindow + kGuard;                      // 0x40 + 0x40 + 0x40 = 0xC0    matriz (posicion en +0x30)
constexpr u32 kVecWindow = 0x10, kVecArena = kGuard + kVecWindow + kGuard;                      // 0x40 + 0x10 + 0x40 = 0x90    un vector
constexpr u32 kStatsWindow = 44 * 0x34, kStatsArena = kGuard + kStatsWindow + kGuard;           // 0x40 + 0x8F0 + 0x40 = 0x970  44 entradas de CPedStat (43 reales + 1 mas alla)
static_assert(kIntelArena == 0x340 && kEntArena == 0xC0 && kMatArena == 0xC0 && kVecArena == 0x90 && kStatsWindow == 0x8F0 && kStatsArena == 0x970, "arenas");
// Memoria global del juego que el banco toca (se guarda al empezar y se restaura tras cada lado)
constexpr u32 kPedStatsPtr = 0xC0BBEC;                                                         // CPedStats::ms_apPedStats
constexpr u32 kIntelRange = 0xBC, kIntelEntities = 0x288, kStatStride = 0x34, kStatCount = 43;

// ================================================================================================ 0: IsInHearingRange
struct C0 {
    u32 family{}, hasMat{}, range{}, seed{};
    u32 ped[3]{}, other[3]{}, posn[3]{};
    u32 eax{}, edx{}, stack{};
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
// Rango cerca de la frontera del orden del EXE: se prueban candidatos float(sqrt(suma)) +-2 ulps de las seis ordenaciones de la suma (aritmetica float: los exponentes de estas familias
// no desbordan) y se elige uno donde el EXE ((dz^2 + dx^2) + dy^2) y alguna otra ordenacion den resultados distintos; si no hay, el de la suma del EXE movido unas ulps.
inline void SolveRange(C0& f, u32 c, int ulpSpan) {
    const float dx = float(double(BitsF(f.posn[0])) - double(BitsF(f.ped[0])));
    const float dy = float(double(BitsF(f.posn[1])) - double(BitsF(f.ped[1])));
    const float dz = float(double(BitsF(f.posn[2])) - double(BitsF(f.ped[2])));
    volatile float vx = dx * dx, vy = dy * dy, vz = dz * dz;
    const float x2 = vx, y2 = vy, z2 = vz;
    volatile float t0 = z2 + x2, t1 = z2 + y2, t2 = x2 + y2, t3 = y2 + z2, t4 = x2 + z2, t5 = y2 + x2;
    volatile float s0 = float(t0) + y2, s1 = float(t1) + x2, s2 = float(t2) + z2, s3 = float(t3) + x2, s4 = float(t4) + y2, s5 = float(t5) + z2;
    const float sums[6]{s0, s1, s2, s3, s4, s5};
    const float exe = sums[0];
    for (u32 k = 0; k < (Rand(c, 57) % 2 ? 6u : 0u); ++k) {
        const float root = std::sqrt(sums[(k + Rand(c, 56)) % 6]);
        for (int u = -2; u <= 2; ++u) {
            const u32 rb = Ulps(FBits(root), u);
            volatile float lim = BitsF(rb) * BitsF(rb);
            const float l = lim;
            for (int j = 1; j < 6; ++j)
                if ((l > exe) != (l > sums[j])) { f.range = rb; return; }
        }
    }
    const double ex = double(dx), ey = double(dy), ez = double(dz);
    f.range = Ulps(FBits(float(std::sqrt((ez * ez + ex * ex) + ey * ey))), int(Rand(c, 55) % u32(2 * ulpSpan + 1)) - ulpSpan);
}
inline C0 MakeC0(u32 n) {
    C0 f{};
    f.family = n % kFamilies;
    const u32 c = n * 8u + 0x69000u;
    f.hasMat = Rand(c, 1) % 2;
    f.seed = Rand(c, 2) | 1u;
    f.eax = SeedReg(c, 7); f.edx = SeedReg(c, 9); f.stack = StackSeed(Rand(c, 11) % 12);
    for (int i = 0; i < 3; ++i) f.other[i] = MixComponent(c, 20 + i);
    auto pedNormal = [&](int hi) { for (int i = 0; i < 3; ++i) f.ped[i] = RandFloat(c, 41 + i, -4, hi); };
    auto fromDelta = [&](const double d[3]) { for (int i = 0; i < 3; ++i) f.posn[i] = FBits(float(double(BitsF(f.ped[i])) + d[i])); };
    double d[3];
    switch (f.family) {
    case 0: { pedNormal(11); const double len = double(Rand(c, 53) % 2000) / 100.0; Split3(c, 50, len, d); fromDelta(d);
              f.range = FBits(float(len + 0.01 + double(Rand(c, 55) % 3000) / 100.0)); break; }                                            // dentro de rango
    case 1: pedNormal(8); Split3(c, 50, 1.0 + double(Rand(c, 53) % 2000) / 100.0, d); fromDelta(d); SolveRange(f, c, 3); break;            // frontera
    case 2: {                                                                                                                              // suma exactamente 625
        static constexpr int quads[][3] = {{25, 0, 0}, {0, 7, 24}, {0, 15, 20}, {12, 15, 16}, {7, 24, 0}, {15, 20, 0}, {20, 0, 15}, {24, 0, 7}, {0, 20, 15}, {16, 12, 15}};
        const auto& q = quads[Rand(c, 50) % std::size(quads)];
        const int perm[3][3] = {{0, 1, 2}, {1, 2, 0}, {2, 0, 1}};
        const auto& p = perm[Rand(c, 51) % 3];
        for (int i = 0; i < 3; ++i) {
            const double v = double(q[p[i]]) * ((Rand(c, 52 + i) & 1) ? -1.0 : 1.0);
            f.ped[i] = FBits(float(double(int(Rand(c, 60 + i) % 2001) - 1000)));
            f.posn[i] = FBits(float(double(BitsF(f.ped[i])) - v));
        }
        f.range = FBits(25.0f);
        if (Rand(c, 56) % 3 == 0) f.range = Ulps(f.range, Rand(c, 58) % 2 ? 1 : -1);
        break;
    }
    case 3: { pedNormal(11); const double len = 20.0 + double(Rand(c, 53) % 2000) / 10.0; Split3(c, 50, len, d); fromDelta(d);
              f.range = FBits(float(len / (1.5 + double(Rand(c, 55) % 100) / 10.0))); break; }                                            // lejos
    case 4: for (int i = 0; i < 3; ++i) { f.ped[i] = RandFloat(c, 41 + i, 60, 127); f.posn[i] = Rand(c, 70 + i) % 3 == 0 ? Neg(f.ped[i]) : RandFloat(c, 70 + i, 60, 127); }
            f.range = Rand(c, 5) % 2 ? RandFloat(c, 5, 60, 127) : RandFloat(c, 5, -4, 10); break;                                         // enormes (los cuadrados desbordan float, no la x87)
    case 5: { pedNormal(8); Split3(c, 50, 10.0, d); fromDelta(d); f.range = FBits(15.0f);                                                  // NaN
              for (int j = 0; j < 1 + int(Rand(c, 56) % 2); ++j) { if (Rand(c, 57 + j) & 1) f.ped[Rand(c, 60 + j) % 3] = Pick(kNan, c, 62 + j); else f.posn[Rand(c, 60 + j) % 3] = Pick(kNan, c, 62 + j); }
              if (Rand(c, 59) % 5 == 0) f.range = Pick(kNan, c, 64);
              break; }
    case 6: { pedNormal(8); Split3(c, 50, 10.0, d); fromDelta(d); f.range = FBits(15.0f);                                                  // infinitos
              for (int j = 0; j < 1 + int(Rand(c, 56) % 3); ++j) {
                  const u32 inf = Pick(kInf, c, 62 + j); const unsigned axis = Rand(c, 60 + j) % 3; const u32 mode = Rand(c, 66 + j) % 4;
                  if (mode == 0) f.ped[axis] = inf; else if (mode == 1) f.posn[axis] = inf; else if (mode == 2) { f.ped[axis] = inf; f.posn[axis] = inf; } else { f.ped[axis] = inf; f.posn[axis] = Neg(inf); }
              }
              if (Rand(c, 59) % 4 == 0) f.range = Pick(kInf, c, 64);
              break; }
    case 7: for (int i = 0; i < 3; ++i) { f.ped[i] = Pick(kTiny, c, 41 + i); f.posn[i] = Pick(kTiny, c, 70 + i); }                           // ceros y subnormales
            f.range = Rand(c, 5) % 2 ? Pick(kTiny, c, 5) : Rand(c, 6) & 0x00FFFFFFu; break;
    case 8: pedNormal(2); for (int i = 0; i < 3; ++i) f.posn[i] = Rand(c, 70 + i) % 3 == 0 ? f.ped[i] : Ulps(f.ped[i], int(Rand(c, 73 + i) % 9) - 4);   // diferencias diminutas
            f.range = RandFloat(c, 5, -26, -18); break;
    case 9: pedNormal(5); Split3(c, 50, double(Rand(c, 53) % 500) / 100.0, d); fromDelta(d);                                               // rango especial
            f.range = Rand(c, 5) % 3 == 0 ? kSpecial[Rand(c, 6) % kSpecial.size()] : Rand(c, 5) % 3 == 1 ? Pick(kNan, c, 7) : Neg(RandFloat(c, 8, -3, 5)); break;
    case 10: {                                                                                                                              // un solo eje cerca del limite
        pedNormal(9); for (int i = 0; i < 3; ++i) f.posn[i] = f.ped[i];
        const double r = 1.0 + double(Rand(c, 52) % 5000) / 100.0; f.range = FBits(float(r));
        const unsigned axis = Rand(c, 50) % 3;
        const double v = r * (1.0 + (double(Rand(c, 53) % 2001) - 1000.0) * 1e-7) * ((Rand(c, 54) & 1) ? -1.0 : 1.0);
        f.posn[axis] = FBits(float(double(BitsF(f.ped[axis])) + v)); f.posn[axis] = Ulps(f.posn[axis], int(Rand(c, 55) % 5) - 2);
        break;
    }
    case 11: {                                                                                                                              // orden de la suma: componentes de magnitudes muy distintas
        for (int i = 0; i < 3; ++i) f.ped[i] = RandFloat(c, 41 + i, -2, 3);
        static constexpr int lo[3] = {0, -6, -10}, hi[3] = {6, 3, 5};
        const unsigned perm = Rand(c, 49) % 3;
        for (int i = 0; i < 3; ++i) { const unsigned j = (i + perm) % 3; d[i] = double(BitsF(RandFloat(c, 50 + i, lo[j], hi[j]))); }
        fromDelta(d); SolveRange(f, c, 3); break;
    }
    case 12: {                                                                                                                              // idem con posiciones grandes (la resta se redondea)
        for (int i = 0; i < 3; ++i) f.ped[i] = RandFloat(c, 41 + i, 5, 12);
        for (int i = 0; i < 3; ++i) d[i] = double(BitsF(RandFloat(c, 50 + i, -2 + i, 4)));
        fromDelta(d); SolveRange(f, c, 2); break;
    }
    case 13: pedNormal(7);                                                                                                                  // mezcla de componentes especiales en las coordenadas
        for (int i = 0; i < 3; ++i) {
            f.posn[i] = Rand(c, 70 + i) % 4 == 0 ? MixComponent(c, 75 + i) : FBits(float(double(BitsF(f.ped[i])) + (double(Rand(c, 80 + i) % 6001) - 3000.0) / 100.0));
            if (Rand(c, 85 + i) % 7 == 0) f.ped[i] = MixComponent(c, 88 + i);
        }
        f.range = FBits(float(double(Rand(c, 5) % 6000) / 100.0)); break;
    case 14: pedNormal(7); Split3(c, 50, double(Rand(c, 53) % 3000) / 100.0, d); fromDelta(d); f.range = MixComponent(c, 5); break;       // rango cualquiera
    default: pedNormal(8); for (int i = 0; i < 3; ++i) f.posn[i] = RandFloat(c, 70 + i, -4, 8); f.range = RandFloat(c, 5, -3, 6); break;     // todo al azar
    }
    return f;
}
inline std::string JsonC0(const C0& c) {
    Js j; j.U("family", c.family).U("has_mat", c.hasMat).U("range", c.range).U("seed", c.seed).A("ped", V3(c.ped)).A("other", V3(c.other)).A("posn", V3(c.posn))
     .U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 1 y 2: HasInterestingEntites / RemoveAllInterestingEntities (misma forma de entrada)
// kind: 0 = nulo, 1/2/3 = entidad de la arena entA/entB/entC, 4 = valor bruto `raw` (no nulo)
struct CS {
    u32 family{}, seed{};
    u32 kind[3]{}, raw[3]{};
    u32 before{}, after{};      // palabra vecina (+0x284 / +0x294): 0 = a cero, 1 = ruido de la arena, 2 = valor no nulo fijo
    u32 eax{}, edx{}, stack{};
};
inline u32 RawSlot(u32 c, u32 k) {
    static constexpr u32 sp[]{1u, 0x100u, 0x10000u, 0x01000000u, 0x80000000u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0xFFFFFF00u, 0x000000FFu, 0xDEAD0000u};
    return Rand(c, k) % 3 == 0 ? sp[Rand(c, k + 1) % std::size(sp)] : (Rand(c, k + 2) | 1u) | 0x00010000u;
}
inline CS MakeCS(u32 n, u32 base) {
    CS f{};
    f.family = n % kFamilies;
    const u32 c = n * 8u + base;
    f.seed = Rand(c, 2) | 1u;
    f.eax = SeedReg(c, 7); f.edx = SeedReg(c, 9); f.stack = StackSeed(Rand(c, 11) % 12);
    f.before = Rand(c, 3) % 3; f.after = Rand(c, 4) % 3;
    auto setSlot = [&](int i) { f.kind[i] = 1 + Rand(c, 20 + i) % 4; f.raw[i] = f.kind[i] == 4 ? RawSlot(c, 30 + 3 * i) : 0u; };
    switch (f.family) {
    case 0: break;                                                       // todo nulo
    case 1: setSlot(0); break;
    case 2: setSlot(1); break;
    case 3: setSlot(2); break;
    case 4: setSlot(0); setSlot(1); break;
    case 5: setSlot(0); setSlot(2); break;
    case 6: setSlot(1); setSlot(2); break;
    case 7: setSlot(0); setSlot(1); setSlot(2); break;
    case 8: f.before = 2; f.after = 2; break;                            // solo los vecinos no nulos
    case 9: { const int i = int(Rand(c, 40) % 3); f.kind[i] = 4; f.raw[i] = RawSlot(c, 41); if (Rand(c, 44) % 2) { f.before = 2; f.after = 2; } break; }   // valores brutos con bytes bajos nulos
    case 10: { const u32 k = 1 + Rand(c, 40) % 3; for (int i = 0; i < 3; ++i) f.kind[i] = k; break; }              // la misma entidad en las tres ranuras
    default: for (int i = 0; i < 3; ++i) if (Rand(c, 50 + i) % 5 < 3) setSlot(i); break;                          // 11..15: al azar
    }
    return f;
}
inline std::string JsonCS(const CS& c) {
    Js j; j.U("family", c.family).U("seed", c.seed).A("kind", V3(c.kind)).A("raw", V3(c.raw)).U("before", c.before).U("after", c.after).U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}
inline CS MakeC1(u32 n) { return MakeCS(n, 0x79000u); }
inline CS MakeC2(u32 n) { return MakeCS(n, 0x89000u); }
inline std::string JsonC1(const CS& c) { return JsonCS(c); }
inline std::string JsonC2(const CS& c) { return JsonCS(c); }

// ================================================================================================ 3: FindIndexWithPedStat
// base_kind: 0 = ms_apPedStats apunta a la arena, 1 = nulo, 2 = valor bruto `base_raw`; arg_kind: 0 = base + delta (delta con signo, modulo 2^32), 1 = valor bruto `arg_raw`
// (con base en la arena, el valor bruto es < 0x10000 o >= 0xF0000000: nunca cae sobre la arena).
struct C3 {
    u32 family{}, baseKind{}, baseRaw{}, argKind{}, delta{}, argRaw{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline C3 MakeC3(u32 n) {
    C3 f{};
    f.family = n % kFamilies;
    const u32 c = n * 8u + 0x99000u;
    f.seed = Rand(c, 2) | 1u;
    f.eax = SeedReg(c, 7); f.ecx = SeedReg(c, 8); f.edx = SeedReg(c, 9); f.stack = StackSeed(Rand(c, 11) % 12);
    const u32 idx = Rand(c, 20) % kStatCount;
    const u32 stride = kStatStride;
    auto rawFar = [&](u32 k) { const u32 r = Rand(c, k); return r % 2 ? 0xF0000000u | (r >> 4) : r & 0xFFFFu; };
    switch (f.family) {
    case 0: f.delta = idx * stride; break;                                             // encontrada
    case 1: f.delta = 0; break;                                                        // la primera
    case 2: f.delta = 42 * stride; break;                                              // la ultima
    case 3: f.delta = 43 * stride; break;                                              // justo despues: 0
    case 4: f.delta = (44 + Rand(c, 21) % 60) * stride; break;                         // mas alla
    case 5: f.delta = idx * stride + 1 + Rand(c, 22) % (stride - 1); break;            // dentro de una entrada, desalineado
    case 6: f.delta = u32(-i32(1 + Rand(c, 23) % 60) * i32(stride)) + (Rand(c, 24) % 2 ? 0u : u32(Rand(c, 25) % 4)); break;   // antes de la tabla
    case 7: f.argKind = 1; f.argRaw = 0; break;                                        // puntero nulo con tabla en la arena
    case 8: f.argKind = 1; f.argRaw = rawFar(26); break;                               // valor bruto lejano
    case 9: f.baseKind = 1; f.delta = Rand(c, 27) % 46 * stride; break;                // tabla nula, argumento k * 0x34
    case 10: f.baseKind = 1; f.delta = Rand(c, 27) % 4 ? 0u : Rand(c, 28) % 2000; break;   // tabla nula, argumento 0 o pequeno
    case 11: f.baseKind = 2; f.baseRaw = Rand(c, 29) % 3 == 0 ? 0xFFFFFF00u - (Rand(c, 30) % 0x400u) : Rand(c, 29) & ~3u; f.delta = Rand(c, 27) % 43 * stride; break;   // tabla bruta (con vuelta de 2^32)
    case 12: f.baseKind = 2; f.baseRaw = Rand(c, 29) & ~3u; f.delta = (43 + Rand(c, 27) % 5) * stride; break;
    case 13: f.delta = idx * stride + 4 * (1 + Rand(c, 22) % 12); break;               // desalineado a palabra
    case 14: f.argKind = 1; f.argRaw = Rand(c, 31) % 2 ? 0xFFFFFFFFu : 0x80000000u; break;
    default: f.delta = Rand(c, 32) % 2 ? idx * stride : idx * stride + (Rand(c, 33) % 3 - 1u); break;   // 15: la entrada o la palabra pegada
    }
    return f;
}
inline std::string JsonC3(const C3& c) {
    Js j; j.U("family", c.family).U("base_kind", c.baseKind).U("base_raw", c.baseRaw).U("arg_kind", c.argKind).U("delta", c.delta).U("arg_raw", c.argRaw).U("seed", c.seed)
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}
}

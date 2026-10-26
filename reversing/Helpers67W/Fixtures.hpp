#pragma once
// Fixtures del banco Helpers67W: CWaterLevel::RenderSeaBedSegment (0x6E6870), cdecl (int32 x, int32 y, float minX, float maxX, float minY, float maxY), `ret` del llamador (retira 24 bytes), void.
// Anade al bufer temporal de agua (0xC4B950 contador de vertices, 0xC4B954 contador de indices, 0xC4B958 indices, 0xC4D958 vertices de 36 bytes) un cuadrilatero: cuatro vertices y seis indices.
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp y fixture_dump.cpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco.
// Reparto: 16384 casos; familia = n % 16 (1024 casos por familia).
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace H67Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }
inline u32 FBits(float f) { return std::bit_cast<u32>(f); }
inline float BitsF(u32 b) { return std::bit_cast<float>(b); }
// Relleno previo de la region: 0 y 5 ruido, 1 ceros, 2 0xFF, 3 0xCD, 4 0xA5
inline void FillRegion(u8* region, u32 size, u32 mode, u32 seed) {
    switch (mode) {
    case 1: std::memset(region, 0x00, size); break;
    case 2: std::memset(region, 0xFF, size); break;
    case 3: std::memset(region, 0xCD, size); break;
    case 4: std::memset(region, 0xA5, size); break;
    default: FillWords(region, size, seed); break;
    }
}

// ------------------------------------------------------------------------------------------------ valores notables
inline constexpr std::array<u32, 30> kSpecial{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x41C80000u, 0xC1C80000u, 0x40000000u, 0x3E800000u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F7FFFFFu, 0x3F800001u, 0x34000000u, 0x33800000u, 0x4B800000u, 0x7F000000u,
    0x7E800000u, 0x5F800000u, 0x1F800000u, 0x20000000u, 0x3D23D70Au, 0x3F75C28Fu, 0x441C4000u, 0x441C4001u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
// Solo NaN silenciosos, con carga distinta de cero (la funcion nunca combina dos NaN: el otro operando es siempre finito).
inline constexpr std::array<u32, 8> kNan{0x7FC00001u, 0xFFC00002u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFEu, 0x7FC00007u, 0xFFC0F00Du};
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {
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
inline u32 Calm(u32 c, u32 k, int lo = -6, int hi = 7) { return RandFloat(c, k, lo, hi); }
// Fraccion de bloque (como las de RenderWater): positiva, en [2^-8, 1).
inline u32 Fraction01(u32 c, u32 k) { return RandFloat(c, k, -8, -1) & 0x7FFFFFFFu; }
// float(i) como bits (redondeo al mas cercano), para construir cancelaciones exactas.
inline u32 FloatBitsOfInt(i32 v) { return FBits(static_cast<float>(v)); }

// ------------------------------------------------------------------------------------------------ region del bufer temporal
// Region que el banco guarda, rellena por caso y restaura: [0xC4B950 contador de vertices][0xC4B954 contador de indices][0xC4B958 indices: 4096 x u16][0xC4D958 vertices: 2048 x 36 bytes].
constexpr u32 kRegionAddress = 0xC4B950u;
constexpr u32 kIdxOff = 8;
constexpr u32 kVtxOff = kIdxOff + 4096u * 2u;
constexpr u32 kVertexBytes = 36;
constexpr u32 kRegionBytes = kVtxOff + 2048u * kVertexBytes;
static_assert(kVtxOff == 0x2008 && kRegionBytes == 0x14008, "region 0xC4B950..0xC5F958");
constexpr u32 kMaxVertexCounter = 2044;   // n + 4 <= 2048 (TOTAL_TEMP_BUFFER_3DVERTICES)
constexpr u32 kMaxIndexCounter = 4090;    // m + 6 <= 4096 (TOTAL_TEMP_BUFFER_INDICES)
constexpr u32 kGuardBytes = 0x40;         // bytes vigilados a cada lado de la region

struct Case {
    u32 n, family;
    i32 x, y;
    u32 minX, maxX, minY, maxY;   // bits de float
    u32 vtx, idx;                 // contadores previos
    u32 mode, seedRegion;
    u32 eax, ecx, edx, stack;
};
// Segmentos de RenderWater: (minX, maxX, minY, maxY) = (0,1,0,1), (0,0.04,0,1), (0.96,1,0,1), (0,1,0,0.04), (0,1,0.96,1)
inline constexpr u32 kRealSegments[5][4]{
    {0x00000000u, 0x3F800000u, 0x00000000u, 0x3F800000u},
    {0x00000000u, 0x3D23D70Au, 0x00000000u, 0x3F800000u},
    {0x3F75C28Fu, 0x3F800000u, 0x00000000u, 0x3F800000u},
    {0x00000000u, 0x3F800000u, 0x00000000u, 0x3D23D70Au},
    {0x00000000u, 0x3F800000u, 0x3F75C28Fu, 0x3F800000u}};
inline constexpr std::array<i32, 14> kBigInts{i32(0x80000000u), 0x7FFFFFFF, 0x7FFFFFFE, i32(0x80000001u), 16777215, 16777216, 16777217, 16777218, 16777219, -16777217, -16777219, 33554433, 33554435, 1 << 30};
inline constexpr std::array<u32, 12> kTieFloats{0x3F000000u, 0x3F800000u, 0x3FC00000u, 0x40000000u, 0x40200000u, 0x40400000u, 0xBF000000u, 0xBF800000u, 0xBFC00000u, 0xC0000000u, 0x3E800000u, 0x3F400000u};
inline i32 SmallInt(u32 c, u32 k, int lo, int hi) { return lo + int(Rand(c, k) % u32(hi - lo + 1)); }

inline Case MakeCase(unsigned n) {
    Case f{};
    const u32 c = n * 8u + 0x670001u;
    f.n = n; f.family = n % kFamilies;
    const u32 fam = f.family;
    f.x = SmallInt(c, 1, -40, 40);
    f.y = SmallInt(c, 2, -40, 40);
    f.minX = Fraction01(c, 3); f.maxX = Fraction01(c, 4); f.minY = Fraction01(c, 5); f.maxY = Fraction01(c, 6);
    auto setAll = [&](auto gen) { f.minX = gen(10); f.maxX = gen(11); f.minY = gen(12); f.maxY = gen(13); };
    auto oneOf = [&](u32 value, u32 slot) {   // sustituye uno de los cuatro flotantes por `value`
        u32* p[4]{&f.minX, &f.maxX, &f.minY, &f.maxY};
        *p[slot % 4] = value;
    };
    switch (fam) {
    case 0: {   // parametros reales de RenderWater: bloques fuera del mundo y segmentos de borde
        f.x = SmallInt(c, 1, -30, 40); f.y = SmallInt(c, 2, -30, 40);
        if (Rand(c, 7) % 3 == 0) { f.x = SmallInt(c, 1, 0, 11); f.y = SmallInt(c, 2, 0, 11); }
        const auto& s = kRealSegments[Rand(c, 8) % 5];
        f.minX = s[0]; f.maxX = s[1]; f.minY = s[2]; f.maxY = s[3];
        break;
    }
    case 1: f.x = SmallInt(c, 1, -100, 100); f.y = SmallInt(c, 2, -100, 100); break;
    case 2: setAll([&](u32 k) { return Calm(c, k, -6, 3); }); break;
    case 3:
        f.x = Rand(c, 7) % 2 ? kBigInts[Rand(c, 8) % kBigInts.size()] : i32(Rand(c, 9));
        f.y = Rand(c, 10) % 2 ? kBigInts[Rand(c, 11) % kBigInts.size()] : i32(Rand(c, 12));
        if (Rand(c, 13) % 3 == 0) setAll([&](u32 k) { return Calm(c, k, -6, 3); });
        break;
    case 4:
        f.x = (Rand(c, 8) % 2 ? 1 : -1) * (16777210 + SmallInt(c, 7, 0, 20));
        f.y = (Rand(c, 10) % 2 ? 1 : -1) * (16777210 + SmallInt(c, 11, 0, 20));
        setAll([&](u32 k) { return Rand(c, k + 20) % 3 ? Pick(kTieFloats, c, k + 30) : Calm(c, k, -2, 2); });
        break;
    case 5: setAll([&](u32 k) { return kSpecial[Rand(c, k) % kSpecial.size()]; }); break;
    case 6: oneOf(Pick(kNan, c, 14), Rand(c, 15)); if (Rand(c, 16) % 3 == 0) oneOf(Pick(kNan, c, 17), Rand(c, 18)); break;
    case 7: oneOf(Pick(kInf, c, 14), Rand(c, 15)); if (Rand(c, 16) % 3 == 0) oneOf(Pick(kInf, c, 17), Rand(c, 18)); break;
    case 8: setAll([&](u32 k) { return Rand(c, k) % 2 ? (Rand(c, k + 20) & 0x80000000u) : (Rand(c, k + 21) & 0x807FFFFFu); }); break;
    case 9: setAll([&](u32 k) { return RandFloat(c, k, 60, 127); }); break;
    case 10: setAll([&](u32 k) { return RandFloat(c, k, -127, -20); }); break;
    case 11: {   // cancelaciones: x + minX == 0 exacto o a 1 ulp, y + minY, y/o los maximos
        f.x = SmallInt(c, 1, -2000, 2000); f.y = SmallInt(c, 2, -2000, 2000);
        const int k1 = SmallInt(c, 20, -1, 1), k2 = SmallInt(c, 21, -1, 1), k3 = SmallInt(c, 22, -1, 1), k4 = SmallInt(c, 23, -1, 1);
        f.minX = Ulps(FloatBitsOfInt(-f.x), k1); f.maxX = Ulps(FloatBitsOfInt(-f.x), k2);
        f.minY = Ulps(FloatBitsOfInt(-f.y), k3); f.maxY = Ulps(FloatBitsOfInt(-f.y), k4);
        if (Rand(c, 24) % 4 == 0) { f.x = 0; f.minX = 0x80000000u; }
        if (Rand(c, 25) % 4 == 0) { f.y = 0; f.maxY = 0x80000000u; }
        break;
    }
    case 12:
        break;   // contadores en los extremos (mas abajo)
    case 13:
        setAll([&](u32 k) { return MixComponent(c, k); });
        f.x = Rand(c, 7) % 4 ? f.x : i32(Rand(c, 8));
        f.y = Rand(c, 9) % 4 ? f.y : i32(Rand(c, 10));
        break;
    case 14: {
        const u32 v = Rand(c, 7) % 3 ? Fraction01(c, 8) : MixComponent(c, 9);
        f.minX = f.maxX = v; f.minY = f.maxY = Rand(c, 10) % 2 ? v : Fraction01(c, 11);
        if (Rand(c, 12) % 2) f.y = f.x;
        if (Rand(c, 13) % 5 == 0) f.x = f.y = 0;
        break;
    }
    default:   // 15: empates de redondeo: enteros grandes mas fracciones exactas
        f.x = (1 << 24) + SmallInt(c, 7, 0, 4000) * (Rand(c, 8) % 2 ? 1 : 3);
        f.y = (Rand(c, 9) % 2 ? 1 : -1) * ((1 << 24) + SmallInt(c, 10, 0, 4000));
        setAll([&](u32 k) { return Pick(kTieFloats, c, k + 40); });
        break;
    }
    // contadores previos
    static constexpr u32 vtxEdge[]{0, 1, 2, 3, 4, 5, 2040, 2041, 2042, 2043, 2044, 1022, 1023, 1024, 2000};
    static constexpr u32 idxEdge[]{0, 1, 2, 3, 5, 6, 4084, 4085, 4086, 4088, 4089, 4090, 2047, 2048, 4000};
    f.vtx = Rand(c, 30) % 4 == 0 ? Rand(c, 31) % (kMaxVertexCounter + 1) : (Rand(c, 32) % 2 ? Rand(c, 33) % 64 : Rand(c, 34) % (kMaxVertexCounter + 1));
    f.idx = Rand(c, 35) % 4 == 0 ? Rand(c, 36) % (kMaxIndexCounter + 1) : (Rand(c, 37) % 2 ? Rand(c, 38) % 96 : Rand(c, 39) % (kMaxIndexCounter + 1));
    if (fam == 12 || Rand(c, 40) % 16 == 0) {
        f.vtx = vtxEdge[Rand(c, 41) % std::size(vtxEdge)];
        f.idx = idxEdge[Rand(c, 42) % std::size(idxEdge)];
    }
    f.mode = Rand(c, 50) % 6; f.seedRegion = Rand(c, 51);
    f.eax = Rand(c, 52) % 8 == 0 ? 0u : Rand(c, 53);
    f.ecx = Rand(c, 54) % 8 == 0 ? 0xFFFFFFFFu : Rand(c, 55);
    f.edx = Rand(c, 56) % 8 == 0 ? 0u : Rand(c, 57);
    f.stack = StackSeed(Rand(c, 58) % 16 < 8 ? Rand(c, 59) % 8 : n);
    return f;
}
// Imagen previa de la region para el caso: relleno por modo y contadores.
inline void BuildRegionRef(const Case& f, u8* ref) {
    FillRegion(ref, kRegionBytes, f.mode, f.seedRegion);
    std::memcpy(ref, &f.vtx, 4);
    std::memcpy(ref + 4, &f.idx, 4);
}
}  // namespace H67Fix

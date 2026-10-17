#pragma once
// Fixtures deterministas del banco Dependencies22A (solo C++ estandar; fixture_oracle.py repite los parametros escalares de cada caso).
#include <array>
#include <bit>
#include <cstdint>
namespace Dependencies22AFix {
using u32 = std::uint32_t;
constexpr unsigned kGenerateCases = 2048, kMultiplierCases = 1536, kCacheCases = 2048, kFogCases = 2048;
constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
// 0, -0, subnormales, minimo normal, FLT_MAX, +-inf, NaN con cargas y signos distintos (silenciosos y senalizantes), +-1e30, 1e-30, +-1, 0.5, 2, 0x7FFFFFFF.
inline constexpr std::array<u32, 32> kStress{
    0x00000000, 0x80000000, 0x00000001, 0x807FFFFF, 0x00400000, 0x80400000, 0x00800000, 0x7F7FFFFF, 0xFF7FFFFF, 0x7F800000, 0xFF800000,
    0x7FC00000, 0xFFC00000, 0x7FC00001, 0x7F800001, 0xFF800001, 0x7FC12345, 0xFFC12345, 0x7FC54321, 0xFFC54321, 0x7149F2CA, 0xF149F2CA,
    0x0DA24260, 0x8DA24260, 0x3F800000, 0xBF800000, 0x3F000000, 0x40000000, 0x7FFFFFFF, 0xFFFFFFFF, 0x7F7FFFFE, 0x5F000000};
// Valor "normal": multiplo de 1/16 en [-range, range] (exacto en float); con probabilidad perMille/1000 un valor de estres.
constexpr u32 FloatBits(int sixteenths) { return std::bit_cast<u32>(static_cast<float>(sixteenths) * 0.0625f); }
constexpr u32 Value(u32 c, u32 k, int rangeSixteenths, u32 perMille) {
    if (Rand(c, k) % 1000 < perMille) return kStress[Rand(c, k + 7000) % 32];
    return FloatBits(static_cast<int>(Rand(c, k + 3000) % (2 * rangeSixteenths + 1)) - rangeSixteenths);
}
// Mascara de campos con estres segun el caso: bit0 pos, bit1 dir, bit2 radio, bit3 color, bit4 punto/camara, bit5 total/foggyness.
inline constexpr std::array<u32, 16> kStressMasks{0, 0, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x09, 0x06, 0x3F, 0x3F, 0x0C, 0x30, 0x03, 0x15};
constexpr u32 PerMille(u32 c, unsigned bit) { return (kStressMasks[c % 16] >> bit) & 1 ? 120 : 0; }

struct LightFx { u32 pos[3], dir[3], radius, color[3], entity; unsigned type, fog, shadows; };
inline constexpr std::array<unsigned, 16> kTypes{0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 5, 0, 1, 2, 5};
inline constexpr std::array<unsigned, 12> kCounts{1, 1, 2, 3, 4, 5, 8, 16, 31, 32, 0, 0xFFFFFFFFu};
constexpr unsigned LightCount(u32 c) { return kCounts[Rand(c, 1) % 12]; }
// centro de las luces (en dieciseisavos): (1600, 3200, 480); la luz i cae a +-25 unidades.
constexpr LightFx Light(u32 c, unsigned i) {
    const u32 k = 100 + i * 32;
    LightFx l{};
    for (unsigned j = 0; j < 3; ++j) {
        const int centre = j == 0 ? 1600 : j == 1 ? 3200 : 480;
        l.pos[j] = Rand(c, k + j) % 1000 < PerMille(c, 0) ? kStress[Rand(c, k + 50 + j) % 32] : FloatBits(centre + static_cast<int>(Rand(c, k + 10 + j) % 801) - 400);
        l.dir[j] = Value(c, k + 20 + j, 16, PerMille(c, 1));
        l.color[j] = Value(c, k + 24 + j, 1020, PerMille(c, 3)) ;
    }
    l.radius = Rand(c, k + 30) % 1000 < PerMille(c, 2) ? kStress[Rand(c, k + 60) % 32] : FloatBits(16 + static_cast<int>(Rand(c, k + 31) % 700));
    l.entity = Rand(c, k + 40) % 3 == 0 ? 0x1000 : (Rand(c, k + 41) % 2 ? 0x2000 : 0);
    l.type = kTypes[Rand(c, k + 42) % 16]; l.fog = Rand(c, k + 43) % 3; l.shadows = Rand(c, k + 44) % 2;
    return l;
}
// Punto de consulta (dieciseisavos): (1600, 3200, 480) +- 400 con estres segun la mascara.
constexpr u32 PointCoord(u32 c, unsigned j) {
    const int centre = j == 0 ? 1600 : j == 1 ? 3200 : 480;
    return Rand(c, 900 + j) % 1000 < PerMille(c, 4) ? kStress[Rand(c, 910 + j) % 32] : FloatBits(centre + static_cast<int>(Rand(c, 920 + j) % 801) - 400);
}
constexpr u32 TotalInit(u32 c) { return Value(c, 930, 48, PerMille(c, 5) + 40); }
constexpr bool UseTotal(u32 c) { return Rand(c, 931) % 4 != 0; }
constexpr u32 EntityArg(u32 c) { return Rand(c, 932) % 2 ? 0x1000 : 0x3000; }

// ProcessVerticalLineUsingCache: contenido de la cache, indice de escritura, punto y comportamiento del falso.
struct CacheFx { u32 entry[32][3], result[32], point[3]; unsigned next; bool pvlOk; u32 pvlZ; int hit; };
constexpr CacheFx Cache(u32 c) {
    CacheFx f{};
    for (unsigned j = 0; j < 3; ++j) f.point[j] = Value(c, 10 + j, 1600, 60 + PerMille(c, 4));
    f.hit = Rand(c, 5) % 3 == 0 ? static_cast<int>(Rand(c, 6) % 32) : -1;
    for (unsigned i = 0; i < 32; ++i) {
        for (unsigned j = 0; j < 3; ++j) f.entry[i][j] = Rand(c, 100 + i * 4 + j) % 40 == 0 ? f.point[j] : Value(c, 100 + i * 4 + j, 1600, 60);
        f.result[i] = Value(c, 300 + i, 800, 60);
    }
    if (f.hit >= 0) for (unsigned j = 0; j < 3; ++j) f.entry[f.hit][j] = Rand(c, 7) % 8 == 0 && j == 0 && f.point[0] == 0 ? 0x80000000u : f.point[j];
    f.next = Rand(c, 8) % 32; f.pvlOk = Rand(c, 9) % 4 != 0; f.pvlZ = Value(c, 11, 800, 120);
    return f;
}

// RenderFogEffect: luces cerca de la camara (dieciseisavos), globales y comportamiento de los falsos (porcentajes /10).
// Escenarios dirigidos: la mayoria de los casos usa luces con niebla 1 o 2 y tipos 0/1/3/4 (los unicos que dibujan), la camara a menos de 8 unidades, cache y suelo casi siempre con exito
// y CalcScreenCoors aceptando; 1 de cada 16 casos anade estres (mascara kStressMasks) y 1 de cada 40 es una cinematica.
inline constexpr std::array<unsigned, 8> kFogTypes{0, 0, 1, 1, 1, 3, 4, 0};
constexpr bool FogStress(u32 c) { return Rand(c, 11) % 16 == 0; }
constexpr LightFx FogLight(u32 c, unsigned i) {
    LightFx l = Light(FogStress(c) ? c : (c & ~0xFu), i);   // sin estres: mascara 0 (c % 16 == 0) para pos/dir/radio/color
    const u32 k = 100 + i * 32;
    l.type = kFogTypes[Rand(c, k + 70) % 8]; l.fog = 1 + Rand(c, k + 71) % 2;
    if (!FogStress(c)) {
        // direccion casi unitaria en el plano y color visible
        const int ang = static_cast<int>(Rand(c, k + 72) % 16);
        constexpr int cs[16]{16, 15, 11, 6, 0, -6, -11, -15, -16, -15, -11, -6, 0, 6, 11, 15}, sn[16]{0, 6, 11, 15, 16, 15, 11, 6, 0, -6, -11, -15, -16, -15, -11, -6};
        l.dir[0] = FloatBits(cs[ang]); l.dir[1] = FloatBits(sn[ang]); l.dir[2] = FloatBits(static_cast<int>(Rand(c, k + 73) % 5) - 2);
        for (unsigned j = 0; j < 3; ++j) l.color[j] = FloatBits(160 + static_cast<int>(Rand(c, k + 74 + j) % 4000));
        for (unsigned j = 0; j < 3; ++j) l.pos[j] = FloatBits((j == 0 ? 1600 : j == 1 ? 3200 : 480) + static_cast<int>(Rand(c, k + 10 + j) % 161) - 80);
    }
    return l;
}
struct FogFx { unsigned seed, count, pCalc, pPvl, pCache, cutscene, cameraMatrix; u32 foggy, timer, camera[3]; };
constexpr FogFx Fog(u32 c) {
    FogFx f{};
    f.seed = c * 7 + 1; f.count = (Rand(c, 1) % 64 == 0) ? Rand(c, 2) % 5 : 3 + Rand(c, 2) % 2;
    const bool st = FogStress(c);
    f.pCalc = st ? 3 + Rand(c, 3) % 8 : 10; f.pPvl = st ? 3 + Rand(c, 4) % 8 : 9 + Rand(c, 4) % 2; f.pCache = st ? 3 + Rand(c, 5) % 8 : 9 + Rand(c, 5) % 2;
    f.cutscene = Rand(c, 6) % 48 == 0; f.cameraMatrix = Rand(c, 7) % 2;
    f.foggy = Rand(c, 8) % 32 == 0 ? 0u : (st ? Value(c, 9, 16, 60 + PerMille(c, 5)) : FloatBits(1 + static_cast<int>(Rand(c, 9) % 16)));
    f.timer = Mix(c + 77);
    for (unsigned j = 0; j < 3; ++j) f.camera[j] = st && Rand(c, 20 + j) % 1000 < 30 + PerMille(c, 4) ? kStress[Rand(c, 30 + j) % 32] : FloatBits((j == 0 ? 1600 : j == 1 ? 3200 : 480) + static_cast<int>(Rand(c, 40 + j) % 129) - 64);
    return f;
}
}  // namespace Dependencies22AFix

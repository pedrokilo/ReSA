#pragma once
// Fixtures deterministas del banco Dependencies22C. Solo C++ estandar (sin dependencias del juego): fixture_oracle.py repite exactamente estas
// formulas y tablas (la comprobacion cruzada esta descrita en README.md). Todos los valores float se dan como patrones de bits.
#include <array>
#include <cstdint>
#include <cstring>
namespace Dependencies22CFix {
using u32 = std::uint32_t;
constexpr unsigned kWrapCases = 2048, kCreateCases = 4096;

// Mezcla (finalizador de murmur3 de 32 bits) y generador por (caso, clave).
constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline float F(u32 bits) { float f; std::memcpy(&f, &bits, 4); return f; }
inline u32 B(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }

// ------------------------------------------------------------------ CCam::WrapAngle (0x509BE0)
// El EXE no termina con |angulo| >= 2^26 aprox. (angulo - 2pi == angulo en float) ni con +-inf: esas entradas quedan FUERA del dominio de las fixtures
// (se comprueba en fixture_oracle.py y en run_local.py que |angulo| <= kWrapLimit y que es finito o NaN). Los NaN no entran en ningun bucle.
constexpr float kWrapLimit = 16777216.0f;
inline constexpr std::array<u32, 72> kAngle{
    0x00000000, 0x80000000, 0x3F800000, 0xBF800000, 0x40400000, 0xC0400000, 0x40490FDA, 0x40490FDB, 0x40490FDC, 0xC0490FDA, 0xC0490FDB, 0xC0490FDC,
    0x40C90FDA, 0x40C90FDB, 0x40C90FDC, 0xC0C90FDA, 0xC0C90FDB, 0xC0C90FDC, 0x4116CBE3, 0x4116CBE4, 0x4116CBE5, 0xC116CBE4, 0x41490FDB, 0xC1490FDB,
    0x42C80000, 0xC2C80000, 0x447A0000, 0xC47A0000, 0x4640E400, 0xC640E400, 0x49742400, 0xC9742400, 0x4A989680, 0xCA989680, 0x4B000000, 0xCB000000,
    0x4B800000, 0xCB800000, 0x4B7FFFFF, 0x4B000001, 0x00000001, 0x007FFFFF, 0x80000001, 0x807FFFFF, 0x00800000, 0x80800000, 0x3FC90FDB, 0xBFC90FDB,
    0x3F000000, 0xBF000000, 0x3D800000, 0x358637BD, 0x7FC00000, 0xFFC00000, 0x7FC01234, 0xFFC00001, 0x7F800001, 0xFF800123, 0x7FFFFFFF, 0xFFFFFFFF,
    0x4546F000, 0xC546F000, 0x41D59FBE, 0xC0E80000, 0x40A00000, 0xC0A00000, 0x412AAAAB, 0xC12AAAAB, 0x4796E7C2, 0xC796E7C2, 0x4B189680, 0xCB189680};
struct WrapFx {
    u32 angle;      // patron de bits del valor inicial
    unsigned kind;  // 0: tabla; 1: aleatorio de magnitud media; 2: multiplo de 2pi +- delta; 3: cerca de +-pi
    unsigned slot;  // desplazamiento del float en el arena (0..3 palabras)
    unsigned fill;  // relleno del arena (0..3)
    unsigned edx;   // semilla de EDX antes de la llamada
};
inline WrapFx Wrap(unsigned c) {
    WrapFx f{};
    f.slot = Rand(c, 9) % 4; f.fill = Rand(c, 10) % 4; f.edx = Rand(c, 11) | 1u;
    if (c < kAngle.size() * 4) {              // las 288 primeras: toda la tabla x 4 (misma entrada con otros arenas)
        f.kind = 0; f.angle = kAngle[c % kAngle.size()];
    } else if (c % 4 == 0) {                   // magnitud media: exponente 2^-8 .. 2^22
        f.kind = 1;
        const u32 e = 119 + Rand(c, 1) % 31, m = Rand(c, 2) & 0x7FFFFF, s = Rand(c, 3) & 1;
        f.angle = (s << 31) | (e << 23) | m;
    } else if (c % 4 == 1) {                   // k * 2pi +- 1 ulp/delta pequeno, k en [-40, 40]
        f.kind = 2;
        const int k = int(Rand(c, 4) % 81) - 40;
        const float base = float(k) * F(0x40C90FDB);
        const int d = int(Rand(c, 5) % 7) - 3;
        f.angle = B(base) + u32(d);
    } else if (c % 4 == 2) {                   // +-pi +- pocos ulp y +-2pi/pi mas multiplos
        f.kind = 3;
        const u32 sign = (Rand(c, 6) & 1) << 31;
        const u32 center = (Rand(c, 7) & 1) ? 0x40490FDBu : 0x40C90FDBu;
        const int d = int(Rand(c, 8) % 9) - 4;
        f.angle = sign | (center + u32(d));
    } else {                                   // entero pequeno o fraccion
        f.kind = 1;
        const float v = float(int(Rand(c, 12) % 4001) - 2000) / float(1 + Rand(c, 13) % 16);
        f.angle = B(v);
    }
    return f;
}

// ------------------------------------------------------------------ FxEmitter_c::CreateParticles (0x4A41E0)
// Cada caso: distancia de camara, (rate, lodStart, lodEnd), contador de emision, intensidad previa, tiempo, delta, viento/lluvia y sus limites, matriz padre y createLocal.
inline constexpr std::array<u32, 16> kDist{
    0x00000000, 0x3F800000, 0x41200000, 0x42C80000, 0x7149F2CA, 0xC0A00000, 0x7FC01234, 0x7F800001, 0x7F800000, 0xFF800000, 0x000003E8, 0x80000000, 0x7F7FFFFF, 0x40600000, 0x40E00000, 0x41FFEB85};
// rate (uint16), lodStart (uint16), lodEnd (uint16)
inline constexpr std::array<std::array<u32, 3>, 16> kLod{{
    {1000, 0, 0}, {1000, 0, 6400}, {65535, 640, 3200}, {1, 64, 64}, {0, 0, 64}, {500, 128, 64}, {65535, 0, 65535}, {3000, 32, 96},
    {20000, 6400, 12800}, {255, 1, 2}, {100, 0, 1}, {65535, 65535, 65535}, {777, 640, 641}, {1000, 1000, 2000}, {40000, 100, 6400}, {8, 3200, 3201}}};
inline constexpr std::array<u32, 16> kCount{
    0x00000000, 0x3F800000, 0x3F000000, 0x41200000, 0x42C80000, 0xBF800000, 0x7FC00001, 0x7F800002, 0x7F800000, 0xFF800000, 0x0DA24260, 0x00000003, 0x7F7FFFFF, 0x7149F2CA, 0x80000000, 0x3CA3D70A};
inline constexpr std::array<u32, 16> kInit{
    0x00000000, 0x3E800000, 0x3F000000, 0x3F7FBE77, 0x3F800000, 0x40000000, 0x40B00000, 0x7FC00002, 0xBF800000, 0x80000000, 0x7F800000, 0x7149F2CA, 0x000116C2, 0x38D1B717, 0x42C80000, 0x7F7FFFFF};
inline constexpr std::array<u32, 8> kWeatherValue{0x00000000, 0x3F000000, 0x3F800000, 0x40000000, 0xBF800000, 0x7FC00003, 0x7F800000, 0x000116C2};
// (minWind, maxWind, minRain, maxRain)
inline constexpr std::array<std::array<u32, 4>, 16> kWeatherLimits{{
    {0x00000000, 0x40000000, 0x00000000, 0x40000000}, {0x00000000, 0x3ECCCCCD, 0x00000000, 0x40000000}, {0x3F19999A, 0x40000000, 0x00000000, 0x40000000},
    {0x00000000, 0x40000000, 0x00000000, 0x3ECCCCCD}, {0x00000000, 0x40000000, 0x3F19999A, 0x40000000}, {0x7FC00000, 0x40000000, 0x00000000, 0x40000000},
    {0x00000000, 0xFFC00001, 0x00000000, 0x40000000}, {0x00000000, 0x40000000, 0x7FC01234, 0x40000000}, {0x00000000, 0x40000000, 0x00000000, 0x7F800002},
    {0xFF800000, 0x7F800000, 0xFF800000, 0x7F800000}, {0x7F800000, 0xFF800000, 0x00000000, 0x40000000}, {0x00000000, 0x40000000, 0x7F800000, 0xFF800000},
    {0x000116C2, 0x40000000, 0x000116C2, 0x40000000}, {0xBF800000, 0x40400000, 0xBF800000, 0x40400000}, {0x40000000, 0x40000000, 0x40000000, 0x40000000},
    {0x00000000, 0x40000000, 0x00000000, 0x40000000}}};
inline constexpr std::array<u32, 8> kDelta{0x00000000, 0x3CA3D70A, 0x3F000000, 0x3F800000, 0x42053333, 0xBF800000, 0x7FC00004, 0x7149F2CA};
inline constexpr std::array<u32, 6> kBenignCount{0x3F800000, 0x40000000, 0x40400000, 0x40A00000, 0x40F00000, 0x41000000};
inline constexpr std::array<u32, 6> kBenignInit{0x00000000, 0x3F000000, 0x3F800000, 0x3FC00000, 0x40000000, 0x00000001};

struct CreateFx {
    u32 dist, rate, lodStart, lodEnd, count, init, wind, rain, minWind, maxWind, minRain, maxRain, delta, time;
    unsigned parent;       // 1: el sistema tiene matriz padre
    unsigned createLocal;  // bit 1 de las banderas del sistema
    unsigned flagsOther;   // resto de bits de las banderas (bit 0 y bits 2..7)
    unsigned emissionFill; // semilla del resto de la estructura EmissionInfo_t que escribe el colaborador
    bool benign;           // el caso recorre el bucle de particulas (suma entre 1 y 16 con el tiempo en rango)
    bool guarded;          // el caso se forzo a salir por el viento para evitar un bucle de miles de millones de particulas
};
// Prediccion de la suma en doble (solo para decidir si el bucle de particulas seria desmesurado; no es un oraculo de valores).
inline double PredictedSum(const CreateFx& f) {
    const double rate = double(f.rate) * double(0.001f);
    const double lodStart = double(f.lodStart) * 0.015625, lodEnd = double(f.lodEnd) * 0.015625, dist = F(f.dist);
    double vis;
    if (lodStart > dist) vis = 1.0; else if (lodEnd < dist) vis = 0.0; else vis = 1.0 - (dist - lodStart) / (lodEnd - lodStart);
    return double(F(f.count)) * vis * rate + double(F(f.init));
}
inline CreateFx Create(unsigned c) {
    CreateFx f{};
    f.benign = c % 4 == 0;
    f.time = Rand(c, 20) & 0x3FFFFFFF;
    f.parent = Rand(c, 5) % 2; f.createLocal = Rand(c, 6) % 2; f.flagsOther = Rand(c, 21) & 0xFD; f.emissionFill = Rand(c, 22);
    if (f.benign) {
        // dist en el tramo cercano, rate 1000 (1.0), lod 640..6400 (10..100), viento y lluvia dentro de [0, 2]; suma = count * 1 * 1 + init
        f.dist = kDist[1 + Rand(c, 1) % 3]; f.rate = 1000; f.lodStart = 640; f.lodEnd = 6400;
        f.count = kBenignCount[Rand(c, 2) % kBenignCount.size()]; f.init = kBenignInit[Rand(c, 3) % kBenignInit.size()];
        f.wind = kWeatherValue[1 + Rand(c, 4) % 3]; f.rain = kWeatherValue[1 + Rand(c, 7) % 3];
        const auto& w = kWeatherLimits[0]; f.minWind = w[0]; f.maxWind = w[1]; f.minRain = w[2]; f.maxRain = w[3];
        f.delta = kDelta[1 + Rand(c, 8) % 4];
        return f;
    }
    f.dist = kDist[c % 16];
    const auto& lod = kLod[c / 16 % 16]; f.rate = lod[0]; f.lodStart = lod[1]; f.lodEnd = lod[2];
    f.count = kCount[c / 256 % 16]; f.init = kInit[Rand(c, 1) % 16];
    f.wind = kWeatherValue[Rand(c, 3) % 8]; f.rain = kWeatherValue[Rand(c, 4) % 8];
    const auto& w = kWeatherLimits[Rand(c, 2) % 16]; f.minWind = w[0]; f.maxWind = w[1]; f.minRain = w[2]; f.maxRain = w[3];
    f.delta = kDelta[Rand(c, 7) % 8];
    // Evita bucles desmesurados: si la suma prevista esta entre 16 y 1e19 (por encima de 2^63, _ftol2 da el entero indefinido, EAX = 0 y no hay bucle) el caso sale por el
    // viento (viento 0.5 < minimo +inf).
    const double s = PredictedSum(f);
    if (s == s && s >= 16.0 && s < 1.0e19) {
        f.guarded = true; f.wind = 0x3F000000; f.minWind = 0x7F800000;
    }
    return f;
}
}  // namespace Dependencies22CFix

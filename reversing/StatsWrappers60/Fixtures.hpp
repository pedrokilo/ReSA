#pragma once
// Fixtures del banco StatsWrappers60: CStats::FindLeastFavoriteRadioStation (0x559010), BuildStatLine (0x559230), UpdateRespectStat (0x55BC50) y ConstructStatLine (0x55A780).
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp y el arnes offline. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco, que lleva
// todo lo necesario; las fixtures solo hacen falta para regenerar los casos.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace StatsWrappers60Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCasesFind = 4096;
constexpr unsigned kCasesBuild = 6144;
constexpr unsigned kCasesRespect = 6144;
constexpr unsigned kCasesConstruct = 8192;

// ------------------------------------------------------------------------------------------------ aleatorios
constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline u64 Fnv64(const u8* p, std::size_t n) {
    u64 h = 0xCBF29CE484222325ull;
    for (std::size_t i = 0; i < n; ++i) { h ^= p[i]; h *= 0x100000001B3ull; }
    return h;
}
inline u32 Fnv32(const char* s) {   // FNV-1a de 32 bits de una cadena terminada en NUL (sin el NUL)
    u32 h = 2166136261u;
    for (; *s; ++s) { h ^= static_cast<u8>(*s); h *= 16777619u; }
    return h;
}
// Generador con estado (SplitMix32 sobre Mix); cada caso crea el suyo con una semilla distinta por funcion.
struct Rng {
    u32 state;
    explicit Rng(u32 seed) : state(Mix(seed ^ 0xA5A5A5A5u)) {}
    u32 Next() { state += 0x9E3779B9u; return Mix(state); }
    u32 Below(u32 n) { return n ? Next() % n : 0; }
    bool Chance(u32 num, u32 den) { return Below(den) < num; }
    template<class T, std::size_t N> T Pick(const std::array<T, N>& a) { return a[Below(static_cast<u32>(N))]; }
    template<class T> T Pick(const std::vector<T>& a) { return a[Below(static_cast<u32>(a.size()))]; }
    i32 Range(i32 lo, i32 hi) { return lo + static_cast<i32>(Below(static_cast<u32>(hi - lo + 1))); }   // inclusivo
};

inline float BitsToFloat(u32 b) { float f; std::memcpy(&f, &b, 4); return f; }
inline u32 FloatToBits(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }

// Valores notables de float (bits): ceros, unos, umbrales de los textos, extremos, denormales, infinitos, qNaN y sNaN con carga, enteros grandes.
inline constexpr std::array<u32, 48> kFloatPool{
    0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0x40000000u, 0x40490FDBu, 0x42C80000u, 0x447A0000u, 0x461C4000u, 0x3DCCCCCDu, 0x3C23D70Au,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x00800000u, 0x80800000u, 0x00000001u, 0x007FFFFFu, 0x807FFFFFu, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u,
    0x7FA00000u, 0xFFA00001u, 0x7FFFFFFFu, 0x4B000000u, 0x4B800000u, 0x4F000000u, 0x5F000000u, 0x5F800000u, 0xCF000000u, 0x4EFFFFFFu, 0x42FE0000u, 0x437F0000u,
    0x3F7FFFFFu, 0x3F800001u, 0x42C7FFFFu, 0x42C80001u, 0x40000001u, 0x3FFFFFFFu, 0x41200000u, 0x41F00000u, 0x42480000u, 0xC2C60000u, 0x3E4CCCCDu, 0x3D4CCCCDu};
inline u32 PickFloat(Rng& r) {
    switch (r.Below(10)) {
    case 0: case 1: case 2: case 3: return r.Pick(kFloatPool);
    case 4: case 5: return FloatToBits(static_cast<float>(r.Below(2001)));                                  // enteros 0..2000
    case 6: return FloatToBits(static_cast<float>(r.Range(-500, 500)) / static_cast<float>(1 + r.Below(100)));    // fracciones
    case 7: return (r.Next() & 0x807FFFFFu) | ((100u + r.Below(60)) << 23);                                   // rango normal
    case 8: return r.Next();                                                                                   // bits aleatorios
    default: return FloatToBits(static_cast<float>(r.Below(100000)) * 0.01f);                                 // dos decimales
    }
}
// Un float "razonable": sin NaN ni infinito (los valores de las estadisticas normales).
inline u32 PickSaneFloat(Rng& r) {
    switch (r.Below(6)) {
    case 0: return 0;
    case 1: return FloatToBits(static_cast<float>(r.Below(1001)));
    case 2: return FloatToBits(static_cast<float>(r.Below(100000)) * 0.01f);
    case 3: return FloatToBits(static_cast<float>(r.Below(30)) * 0.5f);
    case 4: return FloatToBits(static_cast<float>(r.Below(2000000)) + 0.5f);
    default: return FloatToBits(1.0f / static_cast<float>(1 + r.Below(1000)));
    }
}
inline constexpr std::array<u32, 40> kIntPool{
    0u, 1u, 0xFFFFFFFFu, 2u, 9u, 10u, 11u, 59u, 60u, 61u, 99u, 100u, 101u, 119u, 120u, 255u, 256u, 0x7FFFu, 0x8000u, 0xFFFFu, 65536u, 0xFFFFFFFEu, 0x00FFFFFFu,
    0x01000000u, 0x01000001u, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, 1000000u, 16777217u, 33554433u, 3599u, 3600u, 3601u, 86399u, 600000u, 6000000u, 59999u, 60000u, 60001u};
inline u32 PickInt(Rng& r) {
    switch (r.Below(8)) {
    case 0: case 1: case 2: return r.Pick(kIntPool);
    case 3: case 4: return r.Below(300);
    case 5: return r.Below(200000);
    case 6: return static_cast<u32>(-static_cast<i32>(r.Below(300)));
    default: return r.Next();
    }
}
// Un entero "pequeno y positivo" (contadores, tiempos en ms hasta unas horas).
inline u32 PickPositiveInt(Rng& r) {
    switch (r.Below(6)) {
    case 0: return 0;
    case 1: return 1 + r.Below(20);
    case 2: return r.Below(100000);
    case 3: return r.Below(30000000);
    case 4: return 59000 + r.Below(3000);
    default: return r.Below(400000000);
    }
}
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }

// Semillas de pila y registros comunes a todas las funciones.
struct Abi { u32 eax{}, ecx{}, edx{}, stack{}; };
inline Abi MakeAbi(unsigned n, u32 salt) {
    Abi a;
    a.eax = Rand(n, salt + 1); a.ecx = Rand(n, salt + 2); a.edx = Rand(n, salt + 3); a.stack = StackSeed(Rand(n, salt + 4) % 24);
    return a;
}

// ------------------------------------------------------------------------------------------------ direcciones del juego (compartidas con Core.hpp y el oraculo)
constexpr u32 kAddrGString2 = 0xB71510, kAddrGString = 0xB71670, kAddrGxt2 = 0xC1AED8, kAddrGxt1 = 0xC1B100;
constexpr u32 kLenGString = 352, kLenGxt = 552;
constexpr u32 kStatWordsBase = 0xB78E20, kNumStatWords = 344;     // enteros por identificador: [id*4 + 0xB78E20]
constexpr u32 kStatFloatsBase = 0xB79380, kNumStatFloats = 82;
constexpr u32 kTableDefault = 0x8CDE08, kTableBytes = 78;         // tabla de lineas de arg1 > 6 (12 entradas + terminador)

// ================================================================================================ FindLeastFavoriteRadioStation
struct FxFind {
    unsigned n{}, family{};
    std::array<i32, 14> list{};
    Abi abi{};
};
inline FxFind MakeFind(unsigned n) {
    FxFind f; f.n = n; f.family = n % 16;
    Rng r(n * 7919u + 11u);
    f.abi = MakeAbi(n, 100);
    auto rnd = [&] { return static_cast<i32>(r.Next()); };
    auto lowv = [&] { return r.Range(-6, 6); };
    for (auto& v : f.list) v = rnd();
    const i32 base = r.Chance(1, 2) ? static_cast<i32>(r.Below(1000000)) : rnd() >> 8;
    const unsigned m = 1 + r.Below(12);                       // posicion del minimo en 1..12
    switch (f.family) {
    case 0: break;                                              // aleatorio
    case 1: for (auto& v : f.list) v = lowv(); break;
    case 2: { const i32 v = r.Chance(1, 2) ? lowv() : rnd(); for (auto& x : f.list) x = v; break; }
    case 3: for (unsigned i = 0; i < 14; ++i) f.list[i] = base + static_cast<i32>(i) * (1 + static_cast<i32>(r.Below(5))); break;   // creciente
    case 4: for (unsigned i = 0; i < 14; ++i) f.list[i] = base - static_cast<i32>(i) * (1 + static_cast<i32>(r.Below(5))); break;   // decreciente
    case 5: for (unsigned i = 0; i < 14; ++i) f.list[i] = base + 5 + static_cast<i32>(r.Below(1000)); f.list[m] = base; break;     // minimo unico
    case 6: {                                                    // empate del minimo en dos posiciones (gana la primera)
        for (unsigned i = 0; i < 14; ++i) f.list[i] = base + 3 + static_cast<i32>(r.Below(50));
        unsigned a = 1 + r.Below(12), b = 1 + r.Below(12);
        f.list[a] = base; f.list[b] = base; break;
    }
    case 7:                                                      // el minimo esta solo en 0 o 13 (fuera del rango recorrido)
        for (unsigned i = 0; i < 14; ++i) f.list[i] = base + 1 + static_cast<i32>(r.Below(40));
        f.list[r.Chance(1, 2) ? 0 : 13] = base - 1 - static_cast<i32>(r.Below(1000)); break;
    case 8: {                                                    // extremos
        static constexpr i32 e[] = { INT32_MIN, INT32_MAX, -1, 0, 1, INT32_MIN + 1, INT32_MAX - 1 };
        for (auto& v : f.list) v = e[r.Below(7)];
        break;
    }
    case 9: for (unsigned i = 0; i < 14; ++i) f.list[i] = base; f.list[1 + r.Below(12)] = base + 1; break;      // casi todo igual (el primero gana)
    case 10: for (auto& v : f.list) v = r.Chance(3, 4) ? 0 : rnd(); break;
    case 11: for (auto& v : f.list) v = rnd() | static_cast<i32>(0x80000000u); break;                                   // todos negativos (alto bit)
    case 12: for (unsigned i = 0; i < 14; ++i) f.list[i] = base + 1 + static_cast<i32>(r.Below(30)); f.list[12] = base; f.list[7] = base; break;
    case 13: for (unsigned i = 0; i < 14; ++i) f.list[i] = base + 1 + static_cast<i32>(r.Below(30)); f.list[12] = base; break;
    case 14: for (unsigned i = 0; i < 14; ++i) f.list[i] = base + 1 + static_cast<i32>(r.Below(30)); f.list[1] = base; break;
    default: for (unsigned i = 0; i < 14; ++i) f.list[i] = base + static_cast<i32>(i) * 3; f.list[m] = base - 7; break;       // una caida
    }
    return f;
}

// ================================================================================================ BuildStatLine
struct FxBuild {
    unsigned n{}, group{};
    bool lineNull{}, p1Null{}, p2Null{};
    std::string line;
    u32 v1{}, v2{};
    i32 metrics{}, type{};
    bool metric{};          // resultado del falso de CLocalisation::Metric
    u32 pat{};              // patron de relleno inicial de los cuatro buffers de texto
    Abi abi{};
};
inline std::string PickKey(Rng& r) {
    static constexpr const char* fixed[] = { "STAT000", "STAT143", "FEST_OO", "ST_MILE", "ST_SECS", "STAT001", "RADIO", "", "A", "ST_LAB1", "STWE001", "WS_STD", "x~y~z", "~r~Hola", "STAT337" };
    if (r.Chance(1, 3)) return fixed[r.Below(15)];
    const unsigned len = 1 + (r.Chance(1, 8) ? r.Below(100) : r.Below(8));
    std::string s;
    for (unsigned i = 0; i < len; ++i) {
        const u32 k = r.Below(40);
        s += k < 26 ? static_cast<char>('A' + k) : (k < 36 ? static_cast<char>('0' + (k - 26)) : (k == 36 ? '_' : (k == 37 ? '~' : (k == 38 ? ' ' : 'z'))));
    }
    return s;
}
inline i32 PickMetricsAny(Rng& r) {
    switch (r.Below(6)) {
    case 0: return -1;
    case 1: return 10 + static_cast<i32>(r.Below(4));
    case 2: return static_cast<i32>(r.Next());
    case 3: return -static_cast<i32>(r.Below(5));
    default: return static_cast<i32>(r.Below(10));
    }
}
inline FxBuild MakeBuild(unsigned n) {
    FxBuild f; f.n = n; f.group = n % 24;
    Rng r(n * 104729u + 5u);
    f.abi = MakeAbi(n, 200);
    f.line = PickKey(r);
    f.metric = r.Chance(1, 2);
    f.pat = r.Next();
    f.type = r.Chance(1, 2) ? 0 : static_cast<i32>(r.Below(4)) + (r.Chance(1, 3) ? 1 : 0);
    if (f.type == 1) f.type = 2;                              // type == 1 solo en los grupos que lo piden
    f.metrics = static_cast<i32>(r.Below(10));
    f.p1Null = f.p2Null = false;
    const bool floatish = r.Chance(1, 2);
    auto val = [&](bool asFloat) { return asFloat ? (r.Chance(1, 6) ? PickSaneFloat(r) : PickFloat(r)) : PickInt(r); };
    const unsigned g = f.group;
    auto setValues = [&](i32 metrics, bool two) {
        const bool fl = metrics == 1 || metrics == 2 || metrics == 3 || metrics == 6 || metrics == 7 || metrics == 8;
        f.v1 = val(fl); f.v2 = val(two ? fl : floatish);
    };
    if (g <= 2) {                                           // type == 1: horas:minutos (los dos punteros validos)
        f.type = 1;
        f.v1 = PickInt(r); f.v2 = g == 1 ? (r.Chance(1, 2) ? r.Below(25) : PickInt(r)) : PickInt(r);
        if (g == 1 && r.Chance(1, 2)) f.v2 = static_cast<u32>(9 + static_cast<i32>(r.Below(3)));
        f.metrics = g == 2 ? PickMetricsAny(r) : static_cast<i32>(r.Below(10));
    } else if (g <= 5) {                                    // dos valores: metricas 0, 1 y 3
        static constexpr i32 ms[3] = { 0, 1, 3 };
        f.metrics = ms[g - 3];
        setValues(f.metrics, true);
    } else if (g == 6) {                                    // dos valores con otras metricas (no escribe nada)
        do { f.metrics = PickMetricsAny(r); } while (f.metrics == 0 || f.metrics == 1 || f.metrics == 3);
        setValues(f.metrics, true);
        f.p1Null = r.Chance(1, 3);
    } else if (g == 7) {                                    // dos valores con el primer puntero nulo si no se usa
        do { f.metrics = PickMetricsAny(r); } while (f.metrics == 0 || f.metrics == 1 || f.metrics == 3);
        setValues(f.metrics, true);
        f.p1Null = true;
    } else if (g <= 17) {                                   // un valor: metricas 0..9
        f.metrics = static_cast<i32>(g - 8);
        f.p2Null = true;
        setValues(f.metrics, false);
    } else if (g == 18) {                                   // un valor con metricas fuera de rango
        f.p2Null = true;
        do { f.metrics = PickMetricsAny(r); } while (f.metrics >= 0 && f.metrics <= 9);
        setValues(f.metrics, false);
    } else if (g == 19) {                                   // un valor con el puntero nulo
        f.p2Null = true; f.p1Null = true; f.metrics = PickMetricsAny(r);
        setValues(f.metrics, false);
    } else if (g == 20) {                                   // sin linea
        f.lineNull = true;
        f.type = r.Chance(1, 2) ? 0 : 3;
        f.p2Null = r.Chance(1, 2); f.p1Null = r.Chance(1, 2); f.metrics = PickMetricsAny(r);
        setValues(f.metrics, true);
    } else if (g == 21) {                                   // type distinto de 1 (pero parecido)
        static constexpr i32 ts[] = { 0, 2, -1, 0x100, 0x10001, INT32_MIN, INT32_MAX, 3, 255, 257 };
        f.type = ts[r.Below(10)];
        f.p2Null = r.Chance(1, 2); f.metrics = static_cast<i32>(r.Below(10));
        setValues(f.metrics, true);
    } else if (g == 22) {                                   // todo valido, dos valores, metricas 0..9
        f.metrics = static_cast<i32>(r.Below(10)); setValues(f.metrics, true);
    } else {                                                // 23: aleatorio valido
        f.metrics = r.Chance(2, 3) ? static_cast<i32>(r.Below(10)) : PickMetricsAny(r);
        f.p2Null = r.Chance(1, 2); setValues(f.metrics, true);
    }
    // Reglas de seguridad: el EXE desreferencia sin comprobar.
    if (f.type == 1) { f.p1Null = false; f.p2Null = false; }
    if (!f.p2Null && f.type != 1 && (f.metrics == 0 || f.metrics == 1 || f.metrics == 3)) f.p1Null = false;
    return f;
}

// ================================================================================================ UpdateRespectStat
struct FxResp {
    unsigned n{}, family{};
    u32 arg0{};               // argumento completo (el EXE lee solo el byte bajo)
    u32 respect{}, girl{}, w1{}, w2{}, territory{};   // floats (bits): 0xB79490, 0xB79484, 0xB793DC, 0xB79488, 0x96AB9C
    i32 missions{}, total{};  // 0xB791A0, 0xB791B0
    std::array<i32, 2> money{};
    u32 focus{};              // CWorld::PlayerInFocus (0 o 1)
    u32 cheat{};              // byte 0x96917F
    u32 low{}, high{};        // 0x8CDEC4, 0x8CDEC8 (bits)
    u32 prevTotal{};          // 0xB79480 antes de la llamada
    i32 tagged{};             // lo que devuelve el falso de GetPercentageTagged
    bool threshold{};         // lo que devuelve el falso de CheckForThreshold
    Abi abi{};
};
inline u32 ShownNear(Rng& r, float d) {
    switch (r.Below(24)) {
    case 0: return FloatToBits(-99.0f);
    case 1: return FloatToBits(0.0f);
    case 2: return FloatToBits(1.0f);
    case 3: return FloatToBits(1.99f);
    case 4: return FloatToBits(2.0f);
    case 5: return FloatToBits(2.01f);
    case 6: return FloatToBits(d - 40.5f);
    case 7: return FloatToBits(d - 39.5f);
    case 8: return FloatToBits(d - 1.0f);
    case 9: return FloatToBits(d);
    case 10: return FloatToBits(d + 1.0f);
    case 11: return FloatToBits(d + 39.5f);
    case 12: return FloatToBits(d + 40.5f);
    case 13: return FloatToBits(1000.0f);
    case 14: return 0x7FC00000u;
    case 15: return r.Chance(1, 2) ? 0x7F800000u : 0xFF800000u;
    case 16: return FloatToBits(static_cast<float>(static_cast<i32>(d)) + 0.25f);       // misma parte entera que d
    case 17: return FloatToBits(static_cast<float>(static_cast<i32>(d)) + 0.75f);
    case 18: return FloatToBits(d * 0.5f);
    case 19: return FloatToBits(d * 2.0f + 3.0f);
    case 20: return FloatToBits(1.5f);
    case 21: return FloatToBits(-1.0f);
    case 22: return PickFloat(r);
    default: return FloatToBits(d + static_cast<float>(r.Range(-100, 100)) * 0.37f);
    }
}
inline FxResp MakeResp(unsigned n) {
    FxResp f; f.n = n; f.family = n % 16;
    Rng r(n * 15485863u + 3u);
    f.abi = MakeAbi(n, 300);
    f.arg0 = f.family == 0 ? (r.Chance(1, 4) ? 1u : (r.Next() | 1u)) : (r.Chance(1, 6) ? (r.Next() & 0xFFFFFF00u) : 0u);   // distinto de cero solo en la familia 0; el resto, byte bajo 0
    f.focus = r.Below(2);
    f.cheat = r.Chance(1, 5) ? (r.Chance(1, 2) ? 1u : (1u + r.Below(255))) : 0u;
    f.prevTotal = PickFloat(r);
    f.threshold = r.Chance(1, 2);
    // El resto: respeto objetivo d y estadisticas que lo producen aproximadamente (respect * 0.4 domina; lo demas a cero o pequeno).
    static constexpr float targets[] = { 0.0f, 0.4f, 1.0f, 1.5f, 1.99f, 2.0f, 2.01f, 3.0f, 10.0f, 39.0f, 41.0f, 80.0f, 100.0f, 250.0f, 500.0f, 999.0f, 1000.0f, 1001.0f, 1500.0f, 1e6f, 12345.678f };
    float d = targets[r.Below(21)];
    f.respect = FloatToBits(d / 0.4f);
    f.girl = f.w1 = f.w2 = f.territory = 0;
    f.missions = 0; f.total = r.Chance(1, 2) ? 0 : static_cast<i32>(r.Below(5));
    f.money = { 0, 0 };
    f.tagged = 0;
    switch (f.family) {
    case 0: case 1: break;                                                   // objetivo puro (la 0 solo reinicia con arg0 != 0)
    case 2: f.tagged = static_cast<i32>(r.Below(30)); d += static_cast<float>(f.tagged) * 0.5f; break;
    case 3: f.missions = r.Range(0, 100); f.total = r.Range(-3, 120); break;
    case 4: f.girl = FloatToBits(static_cast<float>(r.Below(100))); f.w1 = FloatToBits(static_cast<float>(r.Below(1000))); f.w2 = FloatToBits(static_cast<float>(r.Below(1000))); break;
    case 5: f.territory = FloatToBits(static_cast<float>(r.Below(1001)) * 0.1f); break;
    case 6: f.money = { r.Range(0, 20000000), r.Range(0, 20000000) }; break;
    case 7:                                                                   // todo plausible
        f.respect = PickSaneFloat(r); f.missions = r.Range(0, 100); f.total = r.Range(0, 100); f.girl = PickSaneFloat(r); f.w1 = PickSaneFloat(r); f.w2 = PickSaneFloat(r);
        f.territory = FloatToBits(static_cast<float>(r.Below(1001)) * 0.1f); f.money = { r.Range(0, 100000000), r.Range(0, 100000000) }; f.tagged = static_cast<i32>(r.Below(101)); break;
    case 8:                                                                   // valores notables en cada campo
        f.respect = PickFloat(r); f.girl = PickFloat(r); f.w1 = PickFloat(r); f.w2 = PickFloat(r); f.territory = PickFloat(r);
        f.missions = static_cast<i32>(PickInt(r)); f.total = static_cast<i32>(PickInt(r)); f.money = { static_cast<i32>(PickInt(r)), static_cast<i32>(PickInt(r)) };
        f.tagged = static_cast<i32>(PickInt(r)); break;
    case 9:                                                                   // negativos y extremos del dinero y del contador de etiquetas
        f.money = { static_cast<i32>(PickInt(r)), static_cast<i32>(PickInt(r)) }; f.tagged = static_cast<i32>(PickInt(r)); break;
    case 10: f.respect = r.Next(); f.girl = r.Next(); f.w1 = r.Next(); f.w2 = r.Next(); f.territory = r.Next();
        f.missions = static_cast<i32>(r.Next()); f.total = static_cast<i32>(r.Next()); f.money = { static_cast<i32>(r.Next()), static_cast<i32>(r.Next()) }; f.tagged = static_cast<i32>(r.Next()); break;
    case 11: f.cheat = 1 + r.Below(255); f.respect = PickFloat(r); break;
    case 12: f.respect = FloatToBits(-d / 0.4f - 5.0f); f.territory = FloatToBits(-static_cast<float>(r.Below(100))); break;    // suma negativa
    case 13: f.total = r.Range(-5, 1); f.missions = static_cast<i32>(PickInt(r)); f.respect = PickSaneFloat(r); break;
    case 14: f.territory = FloatToBits(0.2f + static_cast<float>(r.Below(2)) * 0.0000001f * static_cast<float>(r.Below(10))); f.money = { r.Range(8000000, 12000000), r.Range(8000000, 12000000) }; break;
    default: f.respect = PickFloat(r); f.w1 = PickFloat(r); f.tagged = r.Range(-3, 3); break;
    }
    switch (r.Below(5)) {
    case 0:   // baja: el ultimo minimo mostrado esta por encima y el maximo es >= 2
        f.low = FloatToBits(d + static_cast<float>(r.Below(4)) * 0.5f + (r.Chance(1, 3) ? 40.5f : 0.0f));
        f.high = FloatToBits((d > 2.0f ? d : 2.0f) + static_cast<float>(r.Below(3)) * 1.5f);
        break;
    case 1:   // sube: el minimo mostrado esta por debajo
        f.low = FloatToBits(d - 0.25f - static_cast<float>(r.Below(5)) * 10.0f);
        f.high = ShownNear(r, d);
        break;
    default:
        f.low = ShownNear(r, d);
        f.high = ShownNear(r, d);
        break;
    }
    if (r.Chance(1, 6)) f.high = f.low;
    if (r.Chance(1, 8)) { f.low = FloatToBits(-99.0f); }
    if (r.Chance(1, 8)) { f.high = FloatToBits(-99.0f); }
    return f;
}

// ================================================================================================ ConstructStatLine
struct FxCons {
    unsigned n{}, family{};
    i32 arg0{};
    u32 arg1{};                       // argumento completo (el EXE lee el byte bajo)
    bool synth{};                     // tabla sintetica en 0x8CDE08 (arg1 > 6)
    std::array<u8, kTableBytes> tab{};     // contenido de la tabla sintetica (entradas, terminador y relleno)
    std::array<u32, kNumStatWords> words{};    // [0xB78E20 + id*4], id 0..343
    std::array<u32, kNumStatFloats> fl{};      // [0xB79380 + id*4], id 0..81
    u32 a9a89c{}, a9ad70{}, a9ad74{}, timer{};
    std::array<u32, 6> gang{};        // 0x96AB2C..0x96AB40 (etiquetas 331..333 y valores 331..333)
    u32 territory{};
    bool german{}, metric{}, populate{};
    u32 most{}, least{};              // lo que devuelven los falsos de FindMost / FindLeast (0..15)
    u32 pedSeed{};                    // semilla del relleno de la arena del jugador falso
    i32 pedSlot{};                    // byte con signo de +0x718
    u32 pedType{};                    // dword en +slot*0x1C+0x5A0
    u32 pedArmor{};                   // float (bits) en +0x548
    std::array<u32, 10> ws{};         // EAX que devuelve GetWeaponSkill para los tipos 22..31
    u32 mins{}, secs{};               // xor / suma de los falsos de ConvertToMins / ConvertToSecs
    u32 progress{};                   // float (bits) de GetPercentageProgress
    u32 pat{};
    Abi abi{};
};
// Estadisticas con significado: (id, formato, siempre, especial) como las de las tablas reales.
struct StatSample { i16 id; u8 fmt, always, special; };
inline constexpr std::array<StatSample, 40> kSampleEntries{ {
    {21,10,1,0},{22,10,1,0},{64,10,1,0},{164,0,1,1},{81,10,0,0},{330,0,1,1},{170,0,1,0},{135,0,1,0},{184,0,0,0},{200,0,1,0},
    {53,3,0,0},{13,3,1,0},{334,3,0,1},{328,0,1,1},{329,0,1,1},{128,0,1,0},{125,3,1,0},{44,9,0,0},{3,7,0,0},{10,7,1,0},
    {17,9,1,0},{139,7,1,0},{141,4,1,0},{46,5,0,0},{169,0,1,1},{173,0,1,1},{143,0,1,1},{144,0,1,1},{145,0,1,1},{213,0,0,1},
    {231,0,1,1},{241,0,1,1},{243,0,1,1},{252,0,0,1},{323,0,1,1},{326,0,1,1},{331,0,1,1},{337,0,1,1},{0,0,1,1},{320,0,1,1} } };
inline constexpr std::array<i16, 40> kSpecialIds{ 0, 143, 144, 145, 164, 169, 173, 174, 175, 213, 231, 241, 243, 244, 251, 252, 253, 254, 255, 256, 257, 258, 319, 320, 321, 322, 323, 324, 325,
    326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 337 };
// Milisegundos de vuelo ([0xB790C4], [0xB790D4]): minutos totales (ms * 1/60000 truncado) en los tramos de la escalera de 0x55B0A6, incluidos los negativos.
inline u32 PickFlightMs(Rng& r) {
    i32 minutes;
    switch (r.Below(10)) {
    case 0: minutes = r.Range(-3, 8); break;
    case 1: minutes = r.Range(0, 59); break;                 // menos de una hora
    case 2: minutes = r.Range(60, 400); break;
    case 3: minutes = r.Range(-6500, -1); break;             // negativos (el resto con signo da los tramos de r < 0)
    case 4: minutes = r.Range(-130, 130) * 60 + r.Range(-2, 2); break;
    case 5: minutes = r.Range(1, 110) * 60 + r.Range(-2, 33); break;
    case 6: minutes = r.Range(0, 6600); break;
    case 7: return PickInt(r);
    case 8: return PickPositiveInt(r);
    default: minutes = r.Range(-80, 80) * 60 + r.Range(-59, 59); break;
    }
    return static_cast<u32>(minutes * 60000 + (minutes >= 0 ? static_cast<i32>(r.Below(60000)) : -static_cast<i32>(r.Below(60000))));
}
inline FxCons MakeCons(unsigned n) {
    FxCons f; f.n = n; f.family = n % 16;
    Rng r(n * 32452843u + 9u);
    f.abi = MakeAbi(n, 400);
    f.pat = r.Next(); f.pedSeed = r.Next();
    f.german = r.Chance(1, 6); f.metric = r.Chance(1, 2); f.populate = r.Chance(1, 4);
    f.most = r.Below(16); f.least = r.Below(16);
    f.mins = r.Next(); f.secs = r.Next(); f.progress = r.Chance(1, 4) ? PickFloat(r) : PickSaneFloat(r);
    // perfil de las estadisticas: probabilidad de valor positivo
    const unsigned posNum = r.Pick(std::array<unsigned, 5>{ 1, 3, 5, 8, 10 });
    for (unsigned i = 0; i < kNumStatWords; ++i) {
        u32 v;
        if (!r.Chance(posNum, 10)) v = r.Chance(1, 3) ? static_cast<u32>(-static_cast<i32>(r.Below(5))) : 0u;
        else v = r.Chance(1, 6) ? PickInt(r) : PickPositiveInt(r) % 5000000u;
        f.words[i] = v;
    }
    for (unsigned i = 0; i < kNumStatFloats; ++i) {
        u32 v;
        if (!r.Chance(posNum, 10)) v = r.Chance(1, 2) ? 0u : PickFloat(r);
        else v = r.Chance(1, 5) ? PickFloat(r) : PickSaneFloat(r);
        f.fl[i] = v;
    }
    // Campos con significado para los manejadores especiales
    auto setWord = [&](u32 address, u32 v) { f.words[(address - kStatWordsBase) / 4] = v; };
    if (r.Chance(1, 2)) setWord(0xB7905C, r.Below(12));                          // mejor acrobacia insensata (1..8 + otros)
    for (u32 a : { 0xB79060u, 0xB79064u, 0xB790D8u, 0xB790DCu, 0xB79174u, 0xB791BCu, 0xB791C0u, 0xB791E4u, 0xB791E8u, 0xB791ECu, 0xB791F0u }) if (r.Chance(2, 3)) setWord(a, r.Below(60));
    for (u32 a : { 0xB790C4u, 0xB790D4u }) setWord(a, PickFlightMs(r));
    for (u32 i = 7; i < 21; ++i) setWord(0xB78E90 + i * 4, r.Chance(1, 4) ? 0u : (r.Chance(1, 10) ? r.Next() : r.Below(500)));   // PedsKilledOfThisType[7..20]
    f.a9a89c = PickInt(r); f.a9ad70 = PickInt(r); f.a9ad74 = PickInt(r);
    f.timer = r.Chance(1, 5) ? PickInt(r) : r.Below(400000000);
    for (auto& g : f.gang) g = r.Chance(1, 5) ? static_cast<u32>(-static_cast<i32>(1 + r.Below(3))) : r.Below(8);
    f.territory = PickFloat(r);
    // Jugador falso
    f.pedSlot = r.Chance(1, 2) ? r.Range(0, 12) : r.Range(-128, 127);
    switch (r.Below(8)) {
    case 0: f.pedType = 0x20; break;
    case 1: f.pedType = 0x15 + r.Below(13); break;                               // 0x15..0x21 (bordes)
    case 2: f.pedType = 0x16 + r.Below(11); break;
    case 3: f.pedType = (r.Next() & 0xFFFF0000u) | (0x16 + r.Below(11)); break;  // la palabra baja valida y el resto no
    case 4: f.pedType = (r.Next() & 0xFFFF0000u) | 0x20; break;
    case 5: f.pedType = r.Next(); break;
    default: f.pedType = 0x16 + r.Below(11); break;
    }
    f.pedArmor = PickFloat(r);
    for (auto& s : f.ws) { const u32 skill = r.Chance(3, 4) ? r.Below(4) : r.Next() & 0xFF; s = (r.Next() & 0xFFFFFF00u) | skill; }
    // tabla y argumentos
    const unsigned mode = r.Below(100);
    unsigned tableIndex;
    if (mode < 45) { tableIndex = r.Below(7); f.arg1 = tableIndex; f.synth = false; }
    else if (mode < 53) { tableIndex = 7; f.arg1 = 7 + r.Below(249); f.synth = false; }       // tabla por defecto real
    else { tableIndex = 7; f.arg1 = r.Chance(1, 2) ? 7 : 7 + r.Below(249); f.synth = true; }
    if (r.Chance(1, 4)) f.arg1 |= (r.Next() & 0xFFFFFF00u);                      // el EXE lee solo el byte bajo
    // numero de lineas esperado de manera aproximada: arg0 pequeno casi siempre
    const unsigned span = tableIndex == 6 ? 90 : (tableIndex == 3 ? 25 : (tableIndex == 5 ? 55 : 16));
    switch (r.Below(10)) {
    case 0: f.arg0 = -1 - static_cast<i32>(r.Below(3)); break;
    case 1: f.arg0 = static_cast<i32>(r.Below(4)); break;
    case 2: f.arg0 = 100 + static_cast<i32>(r.Below(5)); break;
    default: f.arg0 = static_cast<i32>(r.Below(span)); break;
    }
    if (f.synth) {
        f.arg0 = r.Chance(1, 8) ? -1 - static_cast<i32>(r.Below(2)) : static_cast<i32>(r.Below(14));
        const unsigned count = 1 + r.Below(12);
        for (unsigned i = 0; i < kTableBytes; ++i) f.tab[i] = static_cast<u8>(r.Next());            // relleno basura tras el terminador
        for (unsigned i = 0; i <= 12; ++i) {
            u8* e = f.tab.data() + i * 6;
            if (i == count) { e[0] = 0x9D; e[1] = 0xFF; continue; }                                  // -99 (el resto es relleno)
            if (i > count) { if (e[0] == 0x9D && e[1] == 0xFF) e[0] = 0; continue; }
            i16 id; u8 fmt, always, special;
            const unsigned kind = r.Below(10);
            if (kind < 5) {                                                                         // entrada con significado
                const StatSample s = kSampleEntries[r.Below(40)];
                id = s.id; fmt = s.fmt; always = s.always; special = s.special;
                if (r.Chance(1, 5)) always = static_cast<u8>(r.Chance(1, 2) ? 0 : 1);
            } else if (kind < 8) {                                                                  // formato cualquiera sobre una estadistica de las tablas
                id = r.Chance(1, 2) ? static_cast<i16>(r.Below(82)) : static_cast<i16>(82 + r.Below(262));
                fmt = static_cast<u8>(r.Chance(3, 4) ? r.Below(11) : r.Below(256));
                always = static_cast<u8>(r.Below(3) == 0 ? 0 : (r.Chance(1, 4) ? 1 + r.Below(255) : 1));
                special = 0;
            } else {                                                                                // identificadores especiales con indicador cualquiera
                id = r.Chance(2, 3) ? kSpecialIds[r.Below(40)] : static_cast<i16>(r.Below(344));
                fmt = static_cast<u8>(r.Below(11));
                always = static_cast<u8>(r.Below(3) == 0 ? 0 : 1);
                special = static_cast<u8>(r.Chance(7, 8) ? 1 : 1 + r.Below(255));
            }
            if (id == -99) id = 98;
            e[0] = static_cast<u8>(id & 0xFF); e[1] = static_cast<u8>((id >> 8) & 0xFF); e[2] = fmt; e[3] = always; e[4] = special; e[5] = static_cast<u8>(r.Next());
        }
    }
    return f;
}
}

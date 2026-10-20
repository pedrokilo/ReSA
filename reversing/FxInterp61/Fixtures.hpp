#pragma once
// Fixtures del banco FxInterp61: las cuatro GetVal(float* outValues, float delta) de FxInterpInfo{Float,U255,32,255}_c (0x4A8470, 0x4A8800, 0x4A89C0, 0x4A8B80; thiscall,
// ECX = this, 2 argumentos en pila, `ret 8`, sin valor de retorno). Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y fixture_dump.cpp.
// El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco: `arena` completa (hex, con los punteros como desplazamientos), `delta` y la funcion.
//
// Reparto: por funcion 4096 casos, familia = n % 16 (256 casos por familia). Las funciones comparten el generador; solo cambia como se rellenan las claves
// (Float: 12 floats por fila; U255/32/255: 24 enteros de 16 bits por fila).
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace FxInterp61Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFunctions = 4;
constexpr unsigned kFamilies = 16;
enum Func : unsigned { kFloat = 0, kU255 = 1, k32 = 2, k255 = 3 };

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

// ------------------------------------------------------------------------------------------------ disposicion de la arena (UNA sola, offsets fijos)
//   0x000..0x040 guarda | 0x040..0x050 objeto (vptr +0, m_bLooped +4, m_nNumKeys +5, m_nCount +6, m_pTimes +8, m_Keys +0xC) | 0x050..0x090 relleno
//   0x090..0x0B0 tiempos (16 x uint16; hay lecturas en indices -9 y > 15) | 0x0B0..0x100 relleno
//   0x100..0x300 8 filas de 0x40 bytes: 0x10 de relleno + 0x30 de claves (12 floats o 24 enteros de 16 bits) | 0x300..0x600 relleno (lecturas con numKeys grande)
//   0x600..0x640 guarda previa de la salida | 0x640..0x660 salida (8 floats) | 0x660..0x6C0 guarda | 0x6C0..0x6E0 punteros a las 8 filas de claves (fuera del alcance de las lecturas
//   con indices grandes: ningun puntero se lee como dato) | 0x6E0..0x700 guarda
// El resto de la arena se rellena con palabras aleatorias o patrones: la funcion solo puede cambiar los floats de salida [0, m_nCount).
constexpr u32 kArena = 0x700;
constexpr u32 kObj = 0x40, kObjBytes = 0x10;
constexpr u32 kTimes = 0x90, kTimesCap = 16;
constexpr u32 kPtrs = 0x6C0, kPtrCap = 8;
constexpr u32 kRow0 = 0x100, kRowStride = 0x40, kRowData = 0x10, kRowBytes = 0x30, kRows = 8;
constexpr u32 kOut = 0x640, kOutCap = 8;
constexpr u32 kFakeVptr = 0x0085A970u;      // valor cualquiera reconocible (la funcion no lo lee)
static_assert(kObj + kObjBytes <= kTimes && kTimes + kTimesCap * 2 <= kRow0 && 0x660 <= kPtrs && kPtrs + kPtrCap * 4 <= kArena && kRow0 + kRows * kRowStride <= 0x300 && 0x300 < 0x600 && 0x600 < kOut && kOut + kOutCap * 4 <= 0x660 && 0x660 < kArena);
static_assert(kRow0 + kRowStride * (kRows - 1) + kRowData + 4 * 127 < 0x600, "lecturas con numKeys hasta 127 dentro de la arena y antes de la salida");

// ------------------------------------------------------------------------------------------------ valores notables (floats)
inline constexpr std::array<u32, 28> kSpecial{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x3F3504F3u, 0xBF3504F3u, 0x40000000u, 0x3E800000u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F7FFFFFu, 0x3F800001u, 0x34000000u, 0x33800000u, 0x4B800000u, 0x7F000000u,
    0x7E800000u, 0x5F800000u, 0x1F800000u, 0x20000000u, 0x3FC00000u, 0xC0400000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
// NaN silenciosos con cargas utiles distintas (los de significandos iguales y signo opuesto dan un empate que el oraculo no resuelve: solo se exige EXE == DLL).
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0xFFC00001u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00002u, 0x7FE00000u, 0xFFE00000u, 0x7FC7FFFEu};
// NaN señalizadores: solo en rutas de copia bit a bit (la copia no los toca) y como delta (fld los silencia).
inline constexpr std::array<u32, 6> kSNan{0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFBFFFFFu, 0x7F812345u, 0xFFA54321u};
// deltas notables (floats): extremos del plegado y de la conversion a entero
inline constexpr std::array<u32, 24> kBigDelta{0x4E6E6B28u /*1e9*/, 0xCE6E6B28u, 0x4F000000u /*2^31*/, 0xCF000000u, 0x4F800000u /*2^32*/, 0x4F2A05F2u /*2.2e9*/, 0x5D000000u /*2^63*/, 0x5CFFFFFFu, 0xDD000000u,
    0x5F000000u /*2^63..*/, 0x5E800000u, 0x5E7FFFFFu /*<2^62*/, 0x60AD78ECu /*1e20*/, 0x7149F2CAu /*1e30*/, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x7E800000u, 0x4B7FFFFFu /*2^24-1*/, 0x4B800000u, 0x4B800001u,
    0x4CBEBC20u /*1e8*/, 0xCCBEBC20u, 0x4D800000u /*2^32*/, 0x5C800000u};

// float con exponente sin sesgo en [lo, hi], mantisa aleatoria y signo aleatorio
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
inline u32 PosFloat(u32 c, u32 k, int lo, int hi) { return RandFloat(c, k, lo, hi) & 0x7FFFFFFFu; }
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline float AsFloat(u32 b) { float f; std::memcpy(&f, &b, 4); return f; }
inline u32 AsBits(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }

struct F {
    u32 func;
    u32 family;
    u32 looped;               // byte m_bLooped (0..255)
    i32 numKeys;              // m_nNumKeys (int8)
    u32 count16;              // m_nCount (16 bits; la funcion usa solo el byte bajo con signo)
    u32 delta;                // bits del float delta
    u16 times[kTimesCap];     // se copian a la arena en kTimes
    u8 keys[kRows][kRowBytes];  // 8 filas de claves
    u32 fillStyle;            // 0 aleatorio, 1 0x00, 2 0xFF, 3 patron 0xC3
    u32 fillSeed;
    u32 eax, edx, stack;
};

// Contenido de una fila de claves: 12 floats (Float) o 24 enteros de 16 bits (el resto), con distinto estilo
inline void FillKeys(F& f, u32 c, unsigned style) {
    for (u32 r = 0; r < kRows; ++r) {
        for (u32 w = 0; w < kRowBytes / 4; ++w) {
            u32 v;
            const u32 k = 200 + r * 16 + w;
            if (f.func == kFloat) {
                switch (style) {
                case 0: v = RandFloat(c, k, -6, 4); break;                                   // tipicos
                case 1: v = Pick(kSpecial, c, k); break;                                     // valores simples y limites
                case 2: v = RandFloat(c, k, -126, 127); break;                               // todo el rango (incl. cancelaciones que desbordan float en el x87 extendido)
                case 3: v = PosFloat(c, k, -2, 1); break;                                    // 0.25..4, positivos
                case 4: v = Rand(c, k) & 0x807FFFFFu; break;                                   // subnormales con signo (y ceros)
                case 5: v = (Rand(c, k) & 3) == 0 ? Pick(kInf, c, k + 50) : RandFloat(c, k, -6, 4); break;  // algun infinito
                case 6: v = (Rand(c, k) & 3) == 0 ? Pick(kNan, c, k + 50) : RandFloat(c, k, -6, 4); break;  // algun NaN silencioso
                case 7: v = (Rand(c, k) & 3) == 0 ? Pick(kSNan, c, k + 50) : RandFloat(c, k, -6, 4); break;  // algun NaN senalizador
                case 8: v = (0x7F7FFFFFu - (Rand(c, k) & 3)) ^ ((Rand(c, k + 2) & 1) << 31); break;          // +-FLT_MAX (o casi)
                default: v = Rand(c, k); if ((v & 0x7F800000u) == 0x7F800000u && (v & 0x7FFFFFu)) v |= 0x400000u; break;   // bits crudos, NaN silenciados
                }
            } else {
                u32 a, b;
                auto one = [&](u32 kk) -> u32 {
                    switch (style) {
                    case 0: return Rand(c, kk) & 0xFFFFu;                                       // cualquier valor de 16 bits
                    case 1: { static const u32 e[] = {0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, 0x8001u, 0xFFFEu, 0x00FFu, 0x0100u, 0x4000u}; return e[Rand(c, kk) % 10]; }
                    case 2: return (Rand(c, kk) & 0xFFu);                                       // pequenos
                    case 3: return 0x8000u | (Rand(c, kk) & 0x7FFFu);                           // negativos / mitad alta
                    default: return (Rand(c, kk) & 0x7FFFu);                                    // positivos
                    }
                };
                a = one(k * 2); b = one(k * 2 + 1);
                v = a | (b << 16);
            }
            std::memcpy(&f.keys[r][w * 4], &v, 4);
        }
    }
}

inline void Times(F& f, u32 c, unsigned n, u32 step, u32 first) {      // tiempos crecientes (en 1/256)
    u32 t = first;
    for (unsigned i = 0; i < kTimesCap; ++i) {
        f.times[i] = u16(t > 65535u ? 65535u : t);
        t += 1 + Rand(c, 300 + i) % step;
        (void)n;
    }
}
inline float LastTime(const F& f) { return f.numKeys >= 1 && f.numKeys <= int(kTimesCap) ? float(f.times[f.numKeys - 1]) * 0.00390625f : 1.0f; }
inline u32 Frac(u32 c, u32 k, float lo, float hi) {                       // float aleatorio en [lo, hi)
    const float u = float(Rand(c, k) >> 8) / 16777216.0f;
    return AsBits(lo + (hi - lo) * u);
}

inline F MakeF(unsigned func, unsigned n) {
    F f{};
    const u32 c = n + func * 0x10001u;           // semilla distinta por funcion
    f.func = func;
    f.family = n % kFamilies;
    f.eax = Rand(c, 23); f.edx = Rand(c, 24); f.stack = StackSeed(n);
    f.fillStyle = (n / 16) % 4 == 0 ? 0 : (n / 16) % 4 == 1 ? 0 : (n / 16) % 4 == 2 ? 3 : 1;   // la mitad aleatoria, el resto patron o ceros (todos con ceros/patron se ven mas lecturas "vacias")
    f.fillSeed = Rand(c, 25);
    f.looped = (Rand(c, 26) & 1) ? 1u : 0u;
    f.numKeys = i32(2 + Rand(c, 27) % 7);
    f.count16 = 1 + Rand(c, 28) % 6;
    const unsigned keyStyle = f.func == kFloat ? (n / 16) % 4 : (n / 16) % 5;
    FillKeys(f, c, keyStyle);
    Times(f, c, 16, 600, Rand(c, 29) % 300);
    const float last = LastTime(f);
    f.delta = Frac(c, 30, 0.0f, last);
    switch (f.family) {
    case 0:                                                                  // una sola clave
        f.numKeys = 1; f.count16 = Rand(c, 31) % 9; f.delta = (Rand(c, 32) & 1) ? RandFloat(c, 33, -10, 10) : Pick(kSpecial, c, 34);
        if (f.func == kFloat && (Rand(c, 35) & 3) == 0) { FillKeys(f, c, 7); FillKeys(f, c, 7); }
        break;
    case 1:                                                                  // sin bucle, tiempo dentro del rango
        f.looped = 0; f.numKeys = i32(2 + Rand(c, 31) % 11); Times(f, c, 16, 120, Rand(c, 29) % 100); f.delta = Frac(c, 30, 0.0f, LastTime(f));
        break;
    case 2: {                                                                // sin bucle: sobre los tiempos, antes del primero y mas alla del ultimo
        f.looped = 0; f.numKeys = i32(2 + Rand(c, 31) % 11); Times(f, c, 16, 200, Rand(c, 29) % 100);
        const u32 j = Rand(c, 32) % u32(f.numKeys);
        switch (Rand(c, 33) % 6) {
        case 0: f.delta = AsBits(float(f.times[j]) * 0.00390625f); break;
        case 1: f.delta = AsBits(float(f.times[f.numKeys - 1]) * 0.00390625f); break;
        case 2: f.delta = AsBits(float(f.times[0]) * 0.00390625f - 0.001f * float(1 + Rand(c, 34) % 50)); break;
        case 3: f.delta = AsBits(LastTime(f) + float(Rand(c, 34) % 100) * 0.37f); break;
        case 4: f.delta = RandFloat(c, 34, -8, 2); break;
        default: f.delta = Frac(c, 34, -2.0f, LastTime(f) + 2.0f); break;
        }
        break;
    }
    case 3:                                                                  // con bucle
        f.looped = (Rand(c, 31) % 8) ? 1u : u32(Rand(c, 32));                 // algun valor no canonico del byte (!=0 y !=1)
        f.numKeys = i32(2 + Rand(c, 33) % 11); Times(f, c, 16, 150, Rand(c, 29) % 200);
        f.delta = Frac(c, 30, -3.0f * LastTime(f), 5.0f * LastTime(f) + 1.0f);
        break;
    case 4: {                                                                // con bucle y cocientes extremos / total diminuto o cero
        f.looped = 1; f.numKeys = i32(2 + Rand(c, 31) % 5);
        const u32 mode = Rand(c, 32) % 4;
        Times(f, c, 16, mode == 0 ? 3 : 400, mode == 1 ? 0 : Rand(c, 29) % 50);
        if (mode == 1) for (int i = 0; i < f.numKeys; ++i) f.times[i] = 0;    // total = 0: ratio inf/NaN
        if (mode == 2) f.times[f.numKeys - 1] = u16(1 + Rand(c, 36) % 3);     // total diminuto (1/256..3/256)
        f.delta = (Rand(c, 33) % 4) ? Pick(kBigDelta, c, 34) : RandFloat(c, 34, 20, 100);
        break;
    }
    case 5: {                                                                // delta especial (NaN, inf, ceros, subnormales)
        f.looped = Rand(c, 31) & 1; f.numKeys = i32(2 + Rand(c, 32) % 5); Times(f, c, 16, 300, Rand(c, 29) % 100);
        switch (Rand(c, 33) % 6) {
        case 0: f.delta = Pick(kNan, c, 34); break;
        case 1: f.delta = Pick(kSNan, c, 34); break;
        case 2: f.delta = Pick(kInf, c, 34); break;
        case 3: f.delta = (Rand(c, 34) & 1) ? 0x80000000u : 0u; break;
        case 4: f.delta = (Rand(c, 34) & 0x807FFFFFu); break;
        default: f.delta = ((Rand(c, 34) & 1) << 31) | 0x00800000u | (Rand(c, 35) & 1); break;
        }
        break;
    }
    case 6: {                                                                // tiempos desordenados / iguales / extremos
        f.numKeys = i32(2 + Rand(c, 31) % 7);
        const u32 mode = Rand(c, 32) % 5;
        for (u32 i = 0; i < kTimesCap; ++i) f.times[i] = mode == 0 ? u16(Rand(c, 300 + i)) : mode == 1 ? u16(Rand(c, 29) % 300) : mode == 2 ? u16(Rand(c, 300 + i) % 4) : mode == 3 ? u16(0xFFFF - Rand(c, 300 + i) % 3) : u16(i * 256);
        if (mode == 4 && (Rand(c, 33) & 1)) f.times[Rand(c, 34) % u32(f.numKeys)] = u16(Rand(c, 35));
        f.delta = (Rand(c, 36) % 3) ? Frac(c, 30, -1.0f, 260.0f) : RandFloat(c, 30, -3, 12);
        break;
    }
    case 7: {                                                                // numKeys 0, negativo o grande (lee fuera de las tablas, dentro de la arena)
        const u32 m = Rand(c, 31) % 3;
        f.numKeys = m == 0 ? 0 : m == 1 ? -i32(1 + Rand(c, 32) % 8) : i32(13 + Rand(c, 32) % 115);
        f.delta = (Rand(c, 33) & 1) ? Frac(c, 30, -1.0f, 100.0f) : RandFloat(c, 30, -3, 14);
        f.count16 = Rand(c, 34) % 5;
        break;
    }
    case 8: {                                                                // m_nCount: solo cuenta el byte bajo con signo
        static const u32 high[] = {0x0100u, 0x0200u, 0x7F00u, 0x8000u, 0xFF00u, 0x1200u, 0x0000u, 0xA500u};
        const u32 lowKind = Rand(c, 31) % 6;
        const u32 low = lowKind < 3 ? Rand(c, 32) % 9 : lowKind == 3 ? 0x80u + Rand(c, 32) % 128 : lowKind == 4 ? 0xFFu : 0u;     // 0..8, negativos (byte), -1, 0
        f.count16 = high[Rand(c, 33) % 8] | low;
        f.numKeys = i32(1 + Rand(c, 34) % 6); Times(f, c, 16, 200, Rand(c, 29) % 100); f.delta = Frac(c, 30, 0.0f, LastTime(f));
        if (Rand(c, 35) % 5 == 0) f.numKeys = 1;
        break;
    }
    case 9:                                                                  // claves extremas: (clave - anterior) desborda float pero no el x87 extendido
        f.looped = 0; f.numKeys = i32(2 + Rand(c, 31) % 3);
        if (f.func == kFloat) FillKeys(f, c, (Rand(c, 32) & 1) ? 8 : 2);
        else FillKeys(f, c, 1 + Rand(c, 32) % 2);
        Times(f, c, 16, 40, Rand(c, 29) % 10); f.delta = Frac(c, 30, 0.0f, LastTime(f));
        if (Rand(c, 33) % 3 == 0) f.times[1] = u16(f.times[0] + 1);
        break;
    case 10:                                                                 // claves con infinitos / NaN (Float) o enteros extremos (el resto)
        f.looped = Rand(c, 31) & 1; f.numKeys = i32(1 + Rand(c, 32) % 5); Times(f, c, 16, 100, Rand(c, 29) % 50);
        if (f.func == kFloat) FillKeys(f, c, 5 + Rand(c, 33) % 3);
        else FillKeys(f, c, 1);
        f.delta = (Rand(c, 34) % 4) ? Frac(c, 30, -1.0f, LastTime(f) * 1.5f + 1.0f) : AsBits(LastTime(f) + 1.0f);
        break;
    case 11: {                                                               // delta a pocos ulps de un tiempo de clave
        f.looped = (Rand(c, 31) % 3) == 0; f.numKeys = i32(2 + Rand(c, 32) % 8); Times(f, c, 16, 130, Rand(c, 29) % 100);
        const u32 j = Rand(c, 33) % u32(f.numKeys);
        const u32 base = AsBits(float(f.times[j]) * 0.00390625f);
        const i32 d = i32(Rand(c, 34) % 7) - 3;
        f.delta = base + u32(d);
        if (base == 0 && d < 0) f.delta = 0x80000000u | u32(-d);
        break;
    }
    case 12: {                                                               // datos tipicos de efectos: tiempos 0, 1.0, 2.0.. y claves pequenas
        f.numKeys = i32(2 + Rand(c, 31) % 4);
        for (u32 i = 0; i < kTimesCap; ++i) f.times[i] = u16(i * 256 + ((Rand(c, 32) & 3) == 0 ? Rand(c, 33 + i) % 256 : 0));
        FillKeys(f, c, f.func == kFloat ? 3 : 2);
        f.delta = (Rand(c, 34) & 1) ? Frac(c, 30, 0.0f, float(f.numKeys) + 1.0f) : AsBits(float(Rand(c, 35) % 10) * 0.25f);
        break;
    }
    default: {                                                               // 13..15: mezcla
        f.looped = (Rand(c, 31) % 4) == 0 ? u32(Rand(c, 32)) : (Rand(c, 32) & 1);
        const u32 m = Rand(c, 33) % 8;
        f.numKeys = m == 0 ? 1 : m == 1 ? 0 : m == 2 ? -i32(1 + Rand(c, 34) % 8) : i32(2 + Rand(c, 34) % 11);
        f.count16 = Rand(c, 35) % 12 < 9 ? Rand(c, 35) % 9 : (Rand(c, 36) & 0xFF00u) | (Rand(c, 35) % 9);
        Times(f, c, 16, 1 + Rand(c, 37) % 700, Rand(c, 29) % 400);
        if (Rand(c, 38) % 6 == 0) for (u32 i = 0; i < kTimesCap; ++i) f.times[i] = u16(Rand(c, 300 + i));
        FillKeys(f, c, f.func == kFloat ? Rand(c, 39) % 10 : Rand(c, 39) % 5);
        const u32 d = Rand(c, 40) % 10;
        f.delta = d < 4 ? Frac(c, 30, -2.0f, LastTime(f) * 2.0f + 2.0f) : d < 5 ? Pick(kBigDelta, c, 41) : d < 6 ? Pick(kNan, c, 41) : d < 7 ? Pick(kInf, c, 41) : d < 8 ? Pick(kSpecial, c, 41) : d < 9 ? RandFloat(c, 41, -30, 30) : Rand(c, 41);
        if ((f.delta & 0x7F800000u) == 0x7F800000u && (f.delta & 0x7FFFFFu) && !(f.delta & 0x400000u) && (Rand(c, 42) & 1)) f.delta |= 0x400000u;   // la mitad de los sNaN se silencian (los demas quedan)
        break;
    }
    }
    // las filas con numKeys <= 0 o > 12 de las tablas siguen siendo lecturas validas dentro de la arena
    return f;
}

// Construye la arena del caso. `base` = direccion real de la arena (punteros absolutos) o 0 (punteros = desplazamientos, forma que se registra).
inline void BuildArena(const F& f, u8* arena, u32 base) {
    switch (f.fillStyle) {
    case 1: std::memset(arena, 0x00, kArena); break;
    case 2: std::memset(arena, 0xFF, kArena); break;
    case 3: std::memset(arena, 0xC3, kArena); break;
    default: FillWords(arena, kArena, f.fillSeed); break;
    }
    u8* obj = arena + kObj;
    const u32 vptr = kFakeVptr;
    std::memcpy(obj, &vptr, 4);
    obj[4] = u8(f.looped);
    obj[5] = u8(f.numKeys);
    const u16 count = u16(f.count16);
    std::memcpy(obj + 6, &count, 2);
    const u32 times = base + kTimes, keys = base + kPtrs;
    std::memcpy(obj + 8, &times, 4);
    std::memcpy(obj + 12, &keys, 4);
    std::memcpy(arena + kTimes, f.times, sizeof f.times);
    for (u32 r = 0; r < kRows; ++r) {
        const u32 row = base + kRow0 + r * kRowStride + kRowData;
        std::memcpy(arena + kPtrs + r * 4, &row, 4);
        std::memcpy(arena + kRow0 + r * kRowStride + kRowData, f.keys[r], kRowBytes);
    }
}
}

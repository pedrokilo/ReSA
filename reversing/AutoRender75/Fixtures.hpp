#pragma once
// Fixtures del banco AutoRender75 (lote 75): 0x6A2B10 CAutomobile::Render (thiscall, sin argumentos, virtual). Solo C++ estandar (sin tipos del juego).
// MakeFR(n) devuelve TODO lo que necesita el caso y JsonFR(f) escribe la fila `input` que consume el oraculo (Python): el oraculo no repite ninguna constante de arena.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace AutoRender75Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;
using i64 = std::int64_t;

constexpr unsigned kCases = 16384;   // pares
constexpr unsigned kFunctions = 1;
constexpr unsigned kTotal = kCases * kFunctions;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline u64 Fnv64(const u8* p, std::size_t n) {
    u64 h = 0xCBF29CE484222325ull;
    for (std::size_t i = 0; i < n; ++i) { h ^= p[i]; h *= 0x100000001B3ull; }
    return h;
}
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }

// ------------------------------------------------------------------------------------------------ direcciones del juego (EXE canonico)
constexpr u32 kCarBytes = 0x988;                   // sizeof(CAutomobile)
constexpr u32 kTimerAddress = 0xB7CB84;            // CTimer::m_snTimeInMilliseconds
constexpr u32 kFrameCounterAddress = 0xB7CB4C;     // CTimer::m_FrameCounter
constexpr u32 kCheatInvisible = 0x96914B;          // CCheat::m_aCheatsActive[27]
constexpr u32 kCheatCarsOnWater = 0x969152;        // CCheat::m_aCheatsActive[34]
constexpr u32 kModelTable = 0xA9B0C8;              // CModelInfo::ms_modelInfoPtrs
constexpr u32 kFxSand = 0xA9AE18, kFxSmokeHuge = 0xA9AE20, kFxWaterSplash = 0xA9AE38;   // g_fx.m_Sand / m_SmokeHuge / m_WaterSplash
constexpr u32 kEngineInstance = 0xC97B24;          // RwEngineInstance (puntero a RwGlobals); +0x20 fpRenderStateSet, +0x24 fpRenderStateGet
// ThePaths (0x96F050): m_pPathNodes +0x804, m_pNaviNodes +0x924, m_pNodeLinks +0xA44, m_pNaviLinks +0xDA4 (64 entradas): una sola region 0x96F854..0x96FEF4
constexpr u32 kPathsAddress = 0x96F854, kPathsBytes = 0x6A0;
constexpr u32 kPathNodesOffset = 0x0, kNaviNodesOffset = 0x120, kNodeLinksOffset = 0x240, kNaviLinksOffset = 0x5A0;
constexpr unsigned kAreas = 72;
// Ventana de las arenas de ThePaths por area de la paleta (Core.hpp las usa; el oraculo solo necesita el valor del veneno y del byte +0xC fuera de la paleta)
constexpr unsigned kLinkLo = 16, kLinkTotal = 96;   // enlaces/navi: indices -16..79 (los 0..47 son explicitos, el resto es veneno)
constexpr unsigned kClArena = 1024;                 // CCarPathLink por area (un indice de 10 bits nunca sale de la arena)
constexpr u16 kPoisonLink = 0xA5A5;                 // valor de los u16 de enlaces/navi fuera de 0..47
constexpr u8 kBeyondCl = 0x00;                      // byte +0xC de los CCarPathLink con indice >= 16

// ------------------------------------------------------------------------------------------------ valores notables (floats)
inline float BitsToFloat(u32 b) { float f; std::memcpy(&f, &b, 4); return f; }
inline u32 FloatToBits(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
// Sin denormales ni valores tan pequenos que el x87 (exponente extendido) y el SSE resolverian distinto; NaN solo +qNaN.
inline constexpr std::array<u32, 20> kSpecialFloats{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x40490FDBu, 0xC0490FDBu, 0x3FC90FDBu, 0xBFC90FDBu,
    0x3DCCCCCDu, 0xBDCCCCCDu, 0x40000000u, 0xC0000000u, 0x3F333333u, 0x3A83126Fu, 0x358637BDu, 0x41200000u, 0x3F19999Au, 0x3E99999Au};
inline constexpr std::array<u32, 6> kNonFinite{0x7FC00000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0x7F800000u, 0xFF800000u};
// float uniforme en [lo, hi]
inline u32 RandF(u32 c, u32 k, float lo, float hi) {
    const float t = float(Rand(c, k) % 100001u) / 100000.0f;
    return FloatToBits(lo + (hi - lo) * t);
}
// 1/8 valores notables, el resto uniforme; `forbidZero` evita +-0 (para divisores)
inline u32 MixedF(u32 c, u32 k, float lo, float hi, bool forbidZero = false) {
    const u32 r = Rand(c, k + 5000u) % 64u;
    if (r < 8u) {
        u32 b = kSpecialFloats[Rand(c, k + 6000u) % kSpecialFloats.size()];
        if (forbidZero && (b & 0x7FFFFFFFu) == 0u) b = 0x3F000000u;
        return b;
    }
    return RandF(c, k, lo, hi);
}

// ------------------------------------------------------------------------------------------------ texto JSON minimo (la fila `input`)
inline void PutU(std::string& s, u64 v) { s += std::to_string(v); }
inline void PutI(std::string& s, i64 v) { s += std::to_string(v); }
template<class T> inline void PutArrU(std::string& s, const T* p, std::size_t n) {
    s += '[';
    for (std::size_t i = 0; i < n; ++i) { if (i) s += ','; PutU(s, u64(p[i])); }
    s += ']';
}
template<class T> inline void PutArrI(std::string& s, const T* p, std::size_t n) {
    s += '[';
    for (std::size_t i = 0; i < n; ++i) { if (i) s += ','; PutI(s, i64(p[i])); }
    s += ']';
}
inline void Key(std::string& s, const char* k) { s += '"'; s += k; s += "\":"; }
inline void KU(std::string& s, const char* k, u64 v) { Key(s, k); PutU(s, v); s += ','; }
inline void KI(std::string& s, const char* k, i64 v) { Key(s, k); PutI(s, v); s += ','; }

// =============================================================================================== 0x6A2B10 Render
struct FR {
    u32 family, headSeed, eax, edx, stack, time, old4d0;
    i32 model, getVal;
    u8 cheat, rc, lights, appearance, getWrites, node3Null, node6Null;
    u32 tracks[8], line0, spring0;
    u32 lines[12][6];       // CColLine: inicio xyz, fin xyz (se usan las 4..11)
    u32 fpos[6][3];         // posicion del modelling de los nodos 2..7
};

constexpr unsigned kFamilies = 16;

// ---- generadores de valores
inline u32 SignBit(u32 c, u32 k) { return (Rand(c, k) & 1u) << 31; }
// desnormales y exponentes muy pequenos, signo aleatorio
inline u32 TinyF(u32 c, u32 k) {
    const u32 s = SignBit(c, k + 1);
    switch (Rand(c, k) % 8u) {
    case 0: return s | 1u;
    case 1: return s | 0x007FFFFFu;
    case 2: return s | 0x00800000u;
    case 3: return s | 0x00400000u;
    case 5: return s | ((1u + Rand(c, k + 3) % 30u) << 23) | (Rand(c, k + 4) & 0x7FFFFFu);          // exponentes -126..-97
    case 6: return s | ((60u + Rand(c, k + 3) % 40u) << 23) | (Rand(c, k + 4) & 0x7FFFFFu);         // exponentes -67..-28
    default: return s | (Rand(c, k + 2) % 0x7FFFFFu + 1u);                                           // desnormal aleatorio
    }
}
// magnitudes grandes (exponentes 100..253)
inline u32 BigF(u32 c, u32 k) { return SignBit(c, k + 1) | ((100u + Rand(c, k) % 154u) << 23) | (Rand(c, k + 2) & 0x7FFFFFu); }
// patron crudo finito (cualquier exponente salvo 255)
inline u32 RawFinite(u32 c, u32 k) { u32 b = Rand(c, k); if ((b & 0x7F800000u) == 0x7F800000u) b &= ~0x00800000u; return b; }
inline u32 SignedZero(u32 c, u32 k) { return SignBit(c, k); }
inline u32 NoncanonNaN(u32 c, u32 k) {
    const u32 s = SignBit(c, k + 1);
    u32 payload = Rand(c, k + 2) & 0x7FFFFFu;
    switch (Rand(c, k + 3) % 4u) {
    case 0: payload = 0x400000u; break;                       // +-qNaN canonico
    case 1: payload |= 0x400000u; break;                      // silencioso con carga
    default: payload &= 0x3FFFFFu; break;                     // de senal (se silencia al cargar con fld)
    }
    if (payload == 0u) payload = 1u;                          // nunca infinito
    return s | 0x7F800000u | payload;
}
inline u32 InfF(u32 c, u32 k) { return SignBit(c, k) | 0x7F800000u; }

inline FR MakeFR(unsigned n) {
    FR f{};
    const u32 c = n + 0x750000u;
    const unsigned fam = n % kFamilies;
    f.family = fam;
    f.headSeed = Rand(c, 1); f.eax = Rand(c, 2); f.edx = Rand(c, 3); f.stack = StackSeed(n);
    const bool rhino = !(fam == 12 || fam == 13 || fam == 14);
    if (rhino) f.model = 432;
    else {
        const u32 rm = Rand(c, 4) % 16;
        f.model = rm < 3 ? 441 : rm < 6 ? -2 : rm < 8 ? 539 : rm < 9 ? 431 : rm < 10 ? 433 : rm < 11 ? 0 : rm < 12 ? 0x7FFF : i32(400 + Rand(c, 5) % 212);
        if (f.model == 432) f.model = 431;   // el Rhino lo cubren las otras familias
    }
    f.cheat = u8((fam == 1 || fam == 14) ? 1u : (fam == 0 || fam == 13 || fam == 12) ? Rand(c, 6) % 2u : (Rand(c, 6) % 5u == 0u));
    f.rc = (Rand(c, 7) % 5 == 0) ? u8(0x80 | (Rand(c, 8) & 0x7F)) : u8(Rand(c, 8) & 0x7F);
    f.lights = u8(Rand(c, 9));
    f.appearance = u8(Rand(c, 10) % 6);
    f.getWrites = (Rand(c, 11) % 4) != 0;
    constexpr i32 kGet[6]{0, 1, 2, -1, 0x7FFFFFFF, 7};
    f.getVal = (Rand(c, 12) % 3) ? kGet[Rand(c, 13) % 6] : i32(Rand(c, 14));
    f.node3Null = Rand(c, 15) % 3 == 0;
    f.node6Null = Rand(c, 16) % 3 == 0;
    switch (n % 4) {
    case 0: f.time = Rand(c, 17) % 100000u; break;
    case 1: f.time = 0xFFFFFFFFu - Rand(c, 17) % 4000u; break;       // alrededor del desbordamiento de +3000
    case 2: f.time = Rand(c, 17); break;
    default: f.time = 0xFFFFF448u + Rand(c, 17) % 3u; break;         // 0xFFFFF448 + 3000 == 0 (mod 2^32)
    }
    f.old4d0 = Rand(c, 18);
    // base finita comun
    for (unsigned i = 0; i < 8; ++i) f.tracks[i] = MixedF(c, 300 + i, -1.0f, 2.0f);
    f.line0 = MixedF(c, 330, 0.25f, 1.5f, true);
    f.spring0 = MixedF(c, 331, 0.0f, 1.0f);
    for (unsigned l = 0; l < 12; ++l) for (unsigned k = 0; k < 6; ++k) f.lines[l][k] = RandF(c, 400 + l * 8 + k, -3.0f, 3.0f);
    for (unsigned i = 0; i < 6; ++i) for (unsigned k = 0; k < 3; ++k) f.fpos[i][k] = RandF(c, 600 + i * 4 + k, -5.0f, 5.0f);
    // un unico origen no finito por caso (dos NaN de origen distinto exigirian la regla de dos operandos de la x87; ver README)
    const auto placeSource = [&](u32 value) {
        const u32 where = Rand(c, 800) % 5u;
        const u32 idx = Rand(c, 801), k = Rand(c, 802);
        switch (where) {
        case 0: f.tracks[idx % 8] = value; break;
        case 1: f.line0 = value; break;
        case 2: f.spring0 = value; break;
        case 3: f.lines[4 + idx % 8][k % 6] = value; break;
        default: f.fpos[(idx & 1) ? 5 : 2][k % 3] = value; break;     // nodo 7 (indice 5) o nodo 4 (indice 2)
        }
    };
    switch (fam) {
    case 2:   // lineas diminutas (desnormales y exponentes muy pequenos)
        for (unsigned l = 4; l < 12; ++l) for (unsigned k = 0; k < 6; ++k) if (Rand(c, 900 + l * 8 + k) % 4u != 0) f.lines[l][k] = TinyF(c, 1000 + l * 8 + k);
        for (unsigned i = 0; i < 6; ++i) for (unsigned k = 0; k < 3; ++k) if (Rand(c, 1200 + i * 4 + k) % 3u == 0) f.fpos[i][k] = TinyF(c, 1300 + i * 4 + k);
        break;
    case 3:   // muelle = linea: ratio 0 exacto, v = pista (diminuta o con signo)
        f.spring0 = f.line0;
        for (unsigned i = 0; i < 8; ++i) f.tracks[i] = Rand(c, 1400 + i) % 4u ? TinyF(c, 1410 + i) : SignedZero(c, 1420 + i);
        for (unsigned l = 4; l < 12; ++l) for (unsigned k = 0; k < 6; ++k) if (Rand(c, 900 + l * 8 + k) % 2u) f.lines[l][k] = TinyF(c, 1000 + l * 8 + k);
        break;
    case 4: {  // muelle 0: ratio 1 exacto, pistas alrededor de 1 (v 0, +-1ulp, negativa)
        f.spring0 = (Rand(c, 1500) & 3u) ? 0u : 0x80000000u;
        constexpr u32 kNear[8]{0x3F800000u, 0x3F800001u, 0x3F7FFFFFu, 0x3F800002u, 0x3F7FFFFEu, 0x3F800004u, 0x3F7FFFFCu, 0x3F800010u};
        for (unsigned i = 0; i < 8; ++i) f.tracks[i] = kNear[Rand(c, 1510 + i) % 8u];
        for (unsigned l = 4; l < 12; ++l) for (unsigned k = 0; k < 6; ++k) if (Rand(c, 900 + l * 8 + k) % 2u) f.lines[l][k] = TinyF(c, 1000 + l * 8 + k);
        break;
    }
    case 5: {  // linea0 cero / diminuta / grande y muelle cualquiera: ratio infinito, enorme o indefinido
        const u32 r = Rand(c, 1600) % 6u;
        f.line0 = r == 0 ? 0u : r == 1 ? 0x80000000u : r < 4 ? TinyF(c, 1601) : BigF(c, 1601);
        const u32 s = Rand(c, 1610) % 5u;
        f.spring0 = s == 0 ? 0u : s == 1 ? 0x80000000u : s == 2 ? TinyF(c, 1611) : s == 3 ? BigF(c, 1611) : RandF(c, 1611, 0.0f, 1.0f);
        break;
    }
    case 6: placeSource(InfF(c, 1700)); break;
    case 7: placeSource(NoncanonNaN(c, 1710)); break;
    case 8: {  // ceros con signo
        constexpr u32 k4[4]{0u, 0x80000000u, 0x3F800000u, 0xBF800000u};
        for (unsigned i = 0; i < 8; ++i) f.tracks[i] = k4[Rand(c, 1800 + i) % 4u];
        f.spring0 = SignedZero(c, 1810);
        for (unsigned l = 4; l < 12; ++l) for (unsigned k = 0; k < 6; ++k) if (Rand(c, 900 + l * 8 + k) % 2u) f.lines[l][k] = SignedZero(c, 1820 + l * 8 + k);
        for (unsigned i = 0; i < 6; ++i) for (unsigned k = 0; k < 3; ++k) if (Rand(c, 1200 + i * 4 + k) % 2u) f.fpos[i][k] = SignedZero(c, 1900 + i * 4 + k);
        break;
    }
    case 9:   // posiciones de nodo diminutas o grandes, lineas grandes
        for (unsigned i = 0; i < 6; ++i) for (unsigned k = 0; k < 3; ++k) f.fpos[i][k] = (Rand(c, 1200 + i * 4 + k) & 1u) ? TinyF(c, 2000 + i * 4 + k) : BigF(c, 2100 + i * 4 + k);
        for (unsigned l = 4; l < 12; ++l) for (unsigned k = 0; k < 6; ++k) if (Rand(c, 900 + l * 8 + k) % 3u == 0) f.lines[l][k] = BigF(c, 2200 + l * 8 + k);
        break;
    case 10: {  // desbordamiento con cancelacion: u*inicio.z fuera del rango float en el registro, v*fin.z guardado finito
        for (unsigned i = 0; i < 8; ++i) {
            const float v0 = BitsToFloat(RandF(c, 2300 + i, 5.0e4f, 2.0e5f));
            f.tracks[i] = FloatToBits(v0);
            const float s2 = float(4.2e38 / double(v0)) * (0.85f + 0.3f * float(Rand(c, 2310 + i) % 1000u) / 1000.0f);
            const bool neg = Rand(c, 2320 + i) & 1u;
            f.lines[4 + i][2] = FloatToBits(neg ? -s2 : s2);
            f.lines[4 + i][5] = FloatToBits((neg ? -s2 : s2) * (0.7f + 0.2f * float(Rand(c, 2330 + i) % 1000u) / 1000.0f));
        }
        f.spring0 = f.line0;                  // ratio 0
        break;
    }
    case 11:  // palabras crudas finitas en todos los floats
        for (unsigned i = 0; i < 8; ++i) f.tracks[i] = RawFinite(c, 2400 + i);
        f.line0 = RawFinite(c, 2410); f.spring0 = RawFinite(c, 2411);
        for (unsigned l = 4; l < 12; ++l) for (unsigned k = 0; k < 6; ++k) f.lines[l][k] = RawFinite(c, 2420 + l * 8 + k);
        for (unsigned i = 0; i < 6; ++i) for (unsigned k = 0; k < 3; ++k) f.fpos[i][k] = RawFinite(c, 2500 + i * 4 + k);
        break;
    case 15:  // cancelacion exacta: v = 0 y el nodo ya esta en el inicio de la primera linea de su grupo
        f.spring0 = f.line0;
        for (unsigned i = 0; i < 8; ++i) if (i == 0 || i == 4) f.tracks[i] = SignedZero(c, 2600 + i);
        for (unsigned k = 0; k < 3; ++k) { f.fpos[5][k] = f.lines[4][k]; f.fpos[2][k] = f.lines[8][k]; }
        break;
    case 12: case 13: case 14:   // no Rhino: las zonas del Rhino contienen basura (incluidos NaN y no finitos): no deben leerse
        for (unsigned i = 0; i < 8; ++i) f.tracks[i] = Rand(c, 2700 + i);
        f.line0 = Rand(c, 2710); f.spring0 = Rand(c, 2711);
        break;
    default: break;
    }
    return f;
}

inline std::string JsonFR(const FR& f) {
    std::string s = "{";
    KU(s, "family", f.family); KI(s, "model", f.model); KU(s, "cheat", f.cheat); KU(s, "rc", f.rc); KU(s, "lights", f.lights); KU(s, "appearance", f.appearance);
    KU(s, "get_writes", f.getWrites); KI(s, "get_val", f.getVal); KU(s, "node3_null", f.node3Null); KU(s, "node6_null", f.node6Null);
    KU(s, "time", f.time); KU(s, "old4d0", f.old4d0);
    Key(s, "tracks"); PutArrU(s, f.tracks, 8); s += ',';
    KU(s, "line0", f.line0); KU(s, "spring0", f.spring0);
    Key(s, "lines"); s += '[';
    for (unsigned l = 0; l < 12; ++l) { if (l) s += ','; PutArrU(s, f.lines[l], 6); }
    s += "],";
    Key(s, "fpos"); s += '[';
    for (unsigned i = 0; i < 6; ++i) { if (i) s += ','; PutArrU(s, f.fpos[i], 3); }
    s += "],";
    KU(s, "head_seed", f.headSeed); KU(s, "eax", f.eax); KU(s, "edx", f.edx); KU(s, "stack", f.stack);
    s.back() = '}';
    return s;
}
}

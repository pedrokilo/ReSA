#pragma once
// Fixtures del banco AutomobileWrappers60: tres funciones de CAutomobile que eran envolturas al EXE y hoy estan en C++:
//   0x44D520 CAutomobile::HasCarStoppedBecauseOfLight (thiscall, sin argumentos, bool en AL)
//   0x6A2B10 CAutomobile::Render                       (thiscall, sin argumentos, virtual)
//   0x6AA290 CAutomobile::UpdateWheelMatrix            (thiscall, int32 nodeIndex + int32 flags en pila, `ret 8`)
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp y el volcador dump_fixtures.cpp (compilado con cl para la autoprueba cruzada con fixture_oracle.py).
// Cada MakeXX(n) devuelve TODO lo que necesita el caso y JsonXX(f) escribe la fila `input` que consume el oraculo (Python): el oraculo no repite ninguna constante de arena;
// los tamanos de arena (guardas y ventanas) solo viven en Core.hpp.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace AutomobileWrappers60Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;
using i64 = std::int64_t;

constexpr unsigned kCases = 4096;   // pares por funcion
constexpr unsigned kFunctions = 3;
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

// =============================================================================================== 0x44D520 HasCarStoppedBecauseOfLight
// Paleta de 6 areas de ThePaths con contenido explicito. La arena de cada area (guardas y tamanos) esta en Core.hpp.
constexpr unsigned kPal = 6;       // areas de la paleta
constexpr unsigned kNodes = 6;     // nodos por area (indices 0..5)
constexpr unsigned kLinks = 48;    // entradas explicitas de enlaces por area (fuera de 0..47 = veneno)
constexpr unsigned kCl = 16;       // CCarPathLink por area
struct PalArea {
    u8 loaded;
    u16 id;                 // area (0..63)
    i16 base[kNodes];       // CPathNode+0x10 (m_wBaseLinkId)
    u8 numRaw[kNodes];      // CPathNode+0x18 (bits 0-3 = numero de enlaces)
    u16 la[kLinks], ln[kLinks];   // CNodeAddress del enlace (area, nodo)
    u16 navi[kLinks];       // CCarPathLinkAddress crudo (area<<10 | indice)
    u8 cl[kCl];             // byte +0xC de cada CCarPathLink
};
struct FS {
    u32 kind, headSeed, eax, edx, stack;
    u8 status;              // byte +0x36 completo
    u16 cur[2], start[2], end[2];   // {area, nodo}
    PalArea pal[kPal];
};

inline FS MakeFS(unsigned n) {
    FS f{};
    const u32 c = n + 0x100000u;
    f.kind = (n / 8) % 8;
    f.headSeed = Rand(c, 1); f.eax = Rand(c, 2); f.edx = Rand(c, 3); f.stack = StackSeed(n);
    const u32 b = Rand(c, 4) % 64;
    const u16 unloadedId = u16((b + 11u * kPal) % 64u);   // no esta en la paleta (11 es coprimo con 64)
    for (unsigned p = 0; p < kPal; ++p) {
        PalArea& a = f.pal[p];
        a.id = u16((b + 11u * p) % 64u);
        a.loaded = (Rand(c, 10 + p) % 5) != 0;
    }
    for (unsigned p = 0; p < kPal; ++p) {
        PalArea& a = f.pal[p];
        for (unsigned k = 0; k < kNodes; ++k) {
            const u32 r = Rand(c, 100 + p * 16 + k) % 16;
            a.base[k] = r < 12 ? i16(Rand(c, 120 + p * 16 + k) % 24) : r == 12 ? i16(-1) : r == 13 ? i16(-3) : r == 14 ? i16(40) : i16(32);
            const u32 q = Rand(c, 140 + p * 16 + k);
            const u32 num = (q % 16) == 0 ? 8 + (q >> 8) % 8 : (q >> 4) % 8;
            a.numRaw[k] = u8(((Rand(c, 160 + p * 16 + k) % 16) << 4) | num);
        }
        for (unsigned j = 0; j < kLinks; ++j) {
            const u32 r = Rand(c, 300 + p * 64 + j) % 8;
            const u32 q = Rand(c, 700 + p * 64 + j);
            a.la[j] = r == 3 ? u16(q % 64) : r == 4 ? u16(0xFFFF) : f.pal[q % kPal].id;
            a.ln[j] = u16((q >> 8) % 8);
            const u32 s = Rand(c, 1100 + p * 64 + j);
            const u32 t = s % 16;
            a.navi[j] = t == 0 ? u16(0xFFFF) : t == 1 ? u16(((s >> 8) % 64) << 10 | ((s >> 16) % kCl)) : u16((f.pal[(s >> 8) % kPal].id << 10) | ((s >> 16) % kCl));
        }
        for (unsigned j = 0; j < kCl; ++j) {
            const u32 s = Rand(c, 1500 + p * 16 + j);
            const u32 t = s % 4;
            a.cl[j] = t == 0 ? u8(0) : t == 1 ? u8((s >> 8) % 4) : t == 2 ? u8(((s >> 8) % 4) | ((s >> 12) & 0xFC)) : u8(s >> 8);
        }
    }
    const auto loadedPal = [&](u32 k) -> unsigned {
        for (unsigned t = 0; t < kPal; ++t) {
            const unsigned idx = (Rand(c, k) + t) % kPal;
            if (f.pal[idx].loaded) return idx;
        }
        f.pal[0].loaded = 1;
        return 0;
    };
    const auto palIndexOfId = [&](u16 id) -> int {
        for (unsigned p = 0; p < kPal; ++p) if (f.pal[p].id == id) return int(p);
        return -1;
    };
    // Fuerza que el nodo `node` del area de paleta `pi` tenga un enlace igual a (areaT, nodeT) cuyo enlace de carretera apunte a un area cargada con semaforo variable.
    const auto force = [&](unsigned pi, unsigned node, u32 salt, u16 areaT, u16 nodeT) {
        PalArea& a = f.pal[pi];
        a.base[node] = i16(Rand(c, salt) % 24);
        const u32 num = 1 + Rand(c, salt + 1) % 5;
        a.numRaw[node] = u8((a.numRaw[node] & 0xF0) | num);
        const unsigned idx = unsigned(a.base[node]) + Rand(c, salt + 2) % num;
        a.la[idx] = areaT;
        a.ln[idx] = nodeT;
        const unsigned pt = loadedPal(salt + 3);
        const unsigned k = Rand(c, salt + 4) % kCl;
        a.navi[idx] = u16((f.pal[pt].id << 10) | k);
        f.pal[pt].cl[k] = (Rand(c, salt + 5) % 3) ? u8(1 + Rand(c, salt + 6) % 3) : u8((Rand(c, salt + 7) % 4) << 2);   // casi siempre semaforo activo (bits 0-1)
    };

    const u32 mode = (n / 64) % 8;
    // estado: SIMPLE (2) o PHYSICS (3) salvo en el modo 3
    u32 status;
    if (mode == 3) { status = Rand(c, 12) % 30; if (status >= 2) status += 2; }   // 0, 1, 4..31
    else status = 2 + Rand(c, 12) % 2;
    f.status = u8((status << 3) | (Rand(c, 13) % 8));

    const unsigned pc = loadedPal(11);
    const unsigned curNode = Rand(c, 14) % kNodes;
    f.cur[0] = mode == 2 ? unloadedId : f.pal[pc].id;
    f.cur[1] = u16(curNode);
    // inicio de la ruta
    const unsigned ps = loadedPal(15);
    f.start[0] = f.pal[ps].id;
    f.start[1] = u16(Rand(c, 16) % kNodes);
    f.end[0] = f.pal[loadedPal(17)].id;
    f.end[1] = u16(Rand(c, 18) % kNodes);
    const bool forceStart = mode == 0 || mode == 1 || (mode == 7 && (Rand(c, 19) % 2));
    if (mode == 1) f.start[0] = unloadedId;
    if (forceStart && mode != 2) force(pc, curNode, 200, f.start[0], f.start[1]);
    if (mode == 4) f.end[0] = 0xFFFF;
    if (mode == 6) f.end[0] = unloadedId;
    const bool forceEnd = mode == 5 || (mode == 7 && (Rand(c, 20) % 2));
    if (forceEnd) {
        const int pe = palIndexOfId(f.end[0]);
        if (pe >= 0) force(unsigned(pe), f.end[1], 220, f.cur[0], f.cur[1]);
    }
    return f;
}

inline std::string JsonFS(const FS& f) {
    std::string s = "{";
    KU(s, "kind", f.kind); KU(s, "status", f.status); KU(s, "poison_link", kPoisonLink); KU(s, "beyond_cl", kBeyondCl);
    Key(s, "cur"); PutArrU(s, f.cur, 2); s += ',';
    Key(s, "start"); PutArrU(s, f.start, 2); s += ',';
    Key(s, "end"); PutArrU(s, f.end, 2); s += ',';
    KU(s, "head_seed", f.headSeed); KU(s, "eax", f.eax); KU(s, "edx", f.edx); KU(s, "stack", f.stack);
    Key(s, "pal"); s += '[';
    for (unsigned p = 0; p < kPal; ++p) {
        const PalArea& a = f.pal[p];
        if (p) s += ',';
        s += '{';
        KU(s, "id", a.id); KU(s, "loaded", a.loaded);
        Key(s, "base"); PutArrI(s, a.base, kNodes); s += ',';
        Key(s, "num"); PutArrU(s, a.numRaw, kNodes); s += ',';
        Key(s, "la"); PutArrU(s, a.la, kLinks); s += ',';
        Key(s, "ln"); PutArrU(s, a.ln, kLinks); s += ',';
        Key(s, "navi"); PutArrU(s, a.navi, kLinks); s += ',';
        Key(s, "cl"); PutArrU(s, a.cl, kCl);
        s += '}';
    }
    s += "]}";
    return s;
}

// =============================================================================================== 0x6A2B10 Render
struct FR {
    u32 kind, headSeed, eax, edx, stack, time, old4d0;
    i32 model, getVal;
    u8 cheat, rc, lights, appearance, getWrites, node3Null, node6Null;
    u32 tracks[8], line0, spring0;
    u32 lines[12][6];       // CColLine: inicio xyz, fin xyz (se usan las 4..11)
    u32 fpos[6][3];         // posicion del modelling de los nodos 2..7
};

inline FR MakeFR(unsigned n) {
    FR f{};
    const u32 c = n + 0x200000u;
    f.kind = (n / 8) % 8;
    f.headSeed = Rand(c, 1); f.eax = Rand(c, 2); f.edx = Rand(c, 3); f.stack = StackSeed(n);
    const u32 rm = Rand(c, 4) % 16;
    f.model = rm < 5 ? 432 : rm == 5 ? 441 : rm == 6 ? -2 : rm == 7 ? 539 : i32(400 + Rand(c, 5) % 212);
    f.cheat = Rand(c, 6) % 2;
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
    for (unsigned i = 0; i < 8; ++i) f.tracks[i] = MixedF(c, 300 + i, -1.0f, 2.0f);
    f.line0 = MixedF(c, 330, 0.25f, 1.5f, true);
    f.spring0 = MixedF(c, 331, 0.0f, 1.0f);
    for (unsigned l = 0; l < 12; ++l) for (unsigned k = 0; k < 6; ++k) f.lines[l][k] = RandF(c, 400 + l * 8 + k, -3.0f, 3.0f);
    for (unsigned i = 0; i < 6; ++i) for (unsigned k = 0; k < 3; ++k) f.fpos[i][k] = RandF(c, 600 + i * 4 + k, -5.0f, 5.0f);
    // como mucho un valor no finito por caso, solo en las orugas (un unico NaN/inf no se combina con otro NaN de distinto origen)
    if (Rand(c, 50) % 32 == 0) f.tracks[Rand(c, 51) % 8] = kNonFinite[Rand(c, 52) % kNonFinite.size()];
    return f;
}

inline std::string JsonFR(const FR& f) {
    std::string s = "{";
    KU(s, "kind", f.kind); KI(s, "model", f.model); KU(s, "cheat", f.cheat); KU(s, "rc", f.rc); KU(s, "lights", f.lights); KU(s, "appearance", f.appearance);
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

// =============================================================================================== 0x6AA290 UpdateWheelMatrix
struct FW {
    u32 kind, headSeed, eax, edx, stack;
    i32 node, flags, model, subtype, miWheelModel;
    u32 steer, steer2, hflags, mflags, vflags, wheelScale, frontH, frameCounter;
    u8 autoFlags, cheat, hasFrame;
    u8 wstat[4], surf[4];
    u32 wpos[4], rot[4], comp[4], move[3], right[3], cp[4][3], miFront, miRear, mat[16];
};

inline FW MakeFW(unsigned n) {
    FW f{};
    const u32 c = n + 0x300000u;
    f.kind = (n / 8) % 8;
    f.headSeed = Rand(c, 1); f.eax = Rand(c, 2); f.edx = Rand(c, 3); f.stack = StackSeed(n);
    const u32 rn = Rand(c, 4) % 32;
    constexpr i32 kBadNodes[5]{0, 1, 8, 9, -1};
    f.node = rn < 27 ? i32(2 + Rand(c, 5) % 6) : kBadNodes[rn - 27];
    f.hasFrame = (Rand(c, 6) % 25) != 0;
    f.flags = (Rand(c, 7) % 8 == 0) ? i32(Rand(c, 8)) : i32(Rand(c, 8) % 16);
    const u32 rm = Rand(c, 9) % 16;
    f.model = rm < 2 ? 532 : rm < 4 ? 513 : rm < 6 ? 571 : rm < 9 ? 539 : i32(400 + Rand(c, 10) % 100);
    const u32 rs = Rand(c, 11) % 16;
    f.subtype = rs < 8 ? 0 : rs < 10 ? 1 : rs < 12 ? 2 : rs < 14 ? 3 : rs == 14 ? 4 + i32(Rand(c, 12) % 4) : 9;
    f.hflags = 0;
    if (Rand(c, 13) % 6 == 0) f.hflags |= 0x20;
    if (Rand(c, 14) % 6 == 0) f.hflags |= 0x40;
    if (Rand(c, 15) % 12 == 0) f.hflags |= 0x20000;
    if (Rand(c, 16) % 3 == 0) f.hflags |= 1u << (8 + Rand(c, 17) % 4);
    if (Rand(c, 18) % 3 == 0) f.hflags |= 1u << (12 + Rand(c, 19) % 4);
    if (Rand(c, 20) % 8 == 0) f.hflags |= Rand(c, 21) & 0x1F00FF9Fu;
    constexpr u32 kModelBits[6]{0x10000u, 0x20000u, 0x80000u, 0x100000u, 0x200000u, 0x800000u};
    f.mflags = 0;
    for (unsigned i = 0; i < 6; ++i) if (Rand(c, 30 + i) % ((i == 1 || i == 4) ? 9 : 4) == 0) f.mflags |= kModelBits[i];   // los ejes macizos (0x20000 / 0x200000) son menos frecuentes
    if (Rand(c, 36) % 8 == 0) f.mflags |= Rand(c, 37) & 0x0F00FFFFu;
    f.vflags = Rand(c, 38);
    if (Rand(c, 39) % 6 == 0) f.vflags |= 0x40000000u; else f.vflags &= ~0x40000000u;
    f.autoFlags = u8(Rand(c, 40));
    if (Rand(c, 41) % 8 == 0) f.autoFlags |= 0x80; else f.autoFlags &= 0x7F;
    f.cheat = Rand(c, 42) % 4 == 0;
    f.frameCounter = Rand(c, 43);
    f.steer = MixedF(c, 50, -0.7f, 0.7f);
    f.steer2 = MixedF(c, 51, -0.7f, 0.7f);
    for (unsigned i = 0; i < 4; ++i) {
        f.wpos[i] = MixedF(c, 60 + i, -1.0f, 1.0f);
        f.rot[i] = MixedF(c, 70 + i, -7.0f, 7.0f);
        f.comp[i] = (Rand(c, 80 + i) % 4) ? RandF(c, 84 + i, 0.0f, 0.999f) : RandF(c, 84 + i, 1.0f, 1.3f);
        const u32 w = Rand(c, 90 + i) % 16;
        f.wstat[i] = u8(w < 9 ? 0 : w < 13 ? 1 : 2 + w % 3);
        { const u32 sv = Rand(c, 94 + i); const u32 sk = Rand(c, 98 + i) % 3; f.surf[i] = u8(sk == 0 ? ((sv & 0xFC) | 1) : sk == 1 ? ((sv & 0xFC) | 2) : sv); }   // agua (id & 3 == 1), arena (== 2) o cualquiera
        for (unsigned k = 0; k < 3; ++k) f.cp[i][k] = RandF(c, 100 + i * 4 + k, -5.0f, 5.0f);
    }
    f.wheelScale = (Rand(c, 120) % 8) ? RandF(c, 121, 0.5f, 1.5f) : kSpecialFloats[2 + Rand(c, 122) % 3];   // 1,0 / -1,0 / 0,5: nunca 0
    if (f.wheelScale == 0xBF800000u) f.wheelScale = 0x3F800000u;
    f.frontH = MixedF(c, 123, 0.0f, 1.0f);
    for (unsigned k = 0; k < 3; ++k) { f.move[k] = RandF(c, 130 + k, -2.0f, 2.0f); f.right[k] = RandF(c, 134 + k, -1.0f, 1.0f); }
    f.miFront = RandF(c, 140, 0.3f, 1.2f);
    f.miRear = (Rand(c, 141) % 4 == 0) ? f.miFront : RandF(c, 142, 0.3f, 1.2f);
    f.miWheelModel = (Rand(c, 143) % 2) ? -1 : i32(1 + Rand(c, 144) % 20);
    for (unsigned i = 0; i < 16; ++i) f.mat[i] = RandF(c, 150 + i, -3.0f, 3.0f);
    f.mat[3] = Rand(c, 170); f.mat[7] = Rand(c, 171); f.mat[11] = Rand(c, 172); f.mat[15] = Rand(c, 173);   // flags/relleno del RwMatrix
    // como mucho un valor no finito por caso: steer, steer2, rot o wpos
    if (Rand(c, 180) % 40 == 0) {
        const u32 slot = Rand(c, 181) % 10;
        const u32 nf = kNonFinite[Rand(c, 182) % kNonFinite.size()];
        if (slot == 0) f.steer = nf; else if (slot == 1) f.steer2 = nf; else if (slot < 6) f.rot[slot - 2] = nf; else f.wpos[slot - 6] = nf;
    }
    return f;
}

inline std::string JsonFW(const FW& f) {
    std::string s = "{";
    KU(s, "kind", f.kind); KI(s, "node", f.node); KI(s, "flags", f.flags); KI(s, "model", f.model); KI(s, "subtype", f.subtype); KI(s, "mi_wheel_model", f.miWheelModel);
    KU(s, "has_frame", f.hasFrame); KU(s, "steer", f.steer); KU(s, "steer2", f.steer2); KU(s, "hflags", f.hflags); KU(s, "mflags", f.mflags); KU(s, "vflags", f.vflags);
    KU(s, "wheel_scale", f.wheelScale); KU(s, "front_h", f.frontH); KU(s, "frame_counter", f.frameCounter); KU(s, "auto_flags", f.autoFlags); KU(s, "cheat", f.cheat);
    Key(s, "wstat"); PutArrU(s, f.wstat, 4); s += ',';
    Key(s, "surf"); PutArrU(s, f.surf, 4); s += ',';
    Key(s, "wpos"); PutArrU(s, f.wpos, 4); s += ',';
    Key(s, "rot"); PutArrU(s, f.rot, 4); s += ',';
    Key(s, "comp"); PutArrU(s, f.comp, 4); s += ',';
    Key(s, "move"); PutArrU(s, f.move, 3); s += ',';
    Key(s, "right"); PutArrU(s, f.right, 3); s += ',';
    Key(s, "cp"); s += '[';
    for (unsigned i = 0; i < 4; ++i) { if (i) s += ','; PutArrU(s, f.cp[i], 3); }
    s += "],";
    KU(s, "mi_front", f.miFront); KU(s, "mi_rear", f.miRear);
    Key(s, "mat"); PutArrU(s, f.mat, 16); s += ',';
    KU(s, "head_seed", f.headSeed); KU(s, "eax", f.eax); KU(s, "edx", f.edx); KU(s, "stack", f.stack);
    s.back() = '}';
    return s;
}
}

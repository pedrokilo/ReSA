#pragma once
// Fixtures del banco PresetLoad74 (lote 74): RsLoadPresetViews (EXE 0x619D60), 20480 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   RsLoadPresetViews  0x619D60  cdecl sin argumentos, devuelve NumPresetViews != 0 en EAX (hook global Rs / RsLoadPresetViews, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 20 (1024 casos por familia).
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

namespace Pl74Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kFamilies = 20;
constexpr unsigned kCases = kFamilies * 1024;
constexpr unsigned kFunctions = 1;
constexpr unsigned kMaxLines = 5;
constexpr unsigned kLineMax = 224;     // bytes de una linea (sin el cero final)
constexpr unsigned kNameMax = 40;      // el EXE reserva 0x40 bytes para el nombre

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kHeapWin = 0x800, kPathWin = 0x20, kFileWin = 0x20;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kHeapArena = Arena(kHeapWin), kPathArena = Arena(kPathWin), kFileArena = Arena(kFileWin);

constexpr u32 kPresetViews = 0xC1707C, kNumViews = 0xC17080, kCurView = 0x8D2E30;   // globales de preset_view.cpp
constexpr u32 kEngineInstance = 0xC97B24;                                           // RwEngineInstance (puntero)
constexpr u32 kFileFopen = 0xC4 + 4, kFileFclose = 0xC4 + 8, kFileFgets = 0xC4 + 0x14;       // RwFileFunctions
constexpr u32 kStrSprintf = 0xF0, kStrStrcpy = 0xF8, kStrStrlen = 0x120, kStrSscanf = 0x130; // RwStringFunctions
constexpr u32 kTableOff = 0xC4, kTableBytes = 0x134 - 0xC4 + 4;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 74076u); }
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

struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& H(const char* k, const u8* p, std::size_t n) {   // bytes en hexadecimal
        Key(k); s += '"'; constexpr char d[] = "0123456789abcdef";
        for (std::size_t i = 0; i < n; ++i) { s += d[p[i] >> 4]; s += d[p[i] & 15]; }
        s += '"'; return *this;
    }
    std::string End() const { return s + "}"; }
};

// valores notables de float
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFA00000u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00001u, 0xFFBFFFFFu};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Un float de un fichero de vistas: moderado, cero, especial
inline u32 ViewFloat(u32 c, u32 k, u32 mode) {
    const u32 r = Rand(c, k) % 32;
    if (mode == 0) return r == 0 ? B(0.0f) : RandFloat(c, k + 2, -4, 12);
    if (r < 8) return Pick(kTiny, c, k + 1);
    if (r < 12) return Pick(kInf, c, k + 1);
    if (r < 20) return Pick(kNan, c, k + 1);
    if (r < 23) return Pick(kBig, c, k + 1);
    return RandFloat(c, k + 2, -20, 100);
}

// ------------------------------------------------------------------------------------------------ el caso
struct Line {
    u32 len{};                         // bytes antes del cero final (sin ceros dentro)
    std::array<u8, kLineMax + 1> b{};
    i32 ret{};                         // retorno de sscanf: -1, 0..6 (la linea se ignora), 7 (sin nombre) u 8 (con nombre)
    u32 f[7]{};
    u32 nameLen{};
    std::array<u8, kNameMax + 1> name{};
};
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 numBefore{}, listBefore{}, curBefore{};   // NumPresetViews, PresetViews, CurrentPresetView antes de la llamada
    u32 pathNull{};      // psPathnameCreate devuelve nulo
    u32 fopenOk{};       // fopen devuelve un fichero
    u32 nLines{};
    u32 failAt{};        // el N-esimo Malloc (1-based) devuelve nulo; 0 = ninguno
    Line line[kMaxLines]{};
};

inline void GenLine(const C& c, Line& l, unsigned i, u32 mode) {
    const u32 n = c.n, k = 200 + 40 * i;
    u32 len;
    switch (Rand(n, k) % 6) {
    case 0: len = 0; break;
    case 1: len = 1 + Rand(n, k + 1) % 8; break;
    case 2: len = 20 + Rand(n, k + 1) % 80; break;
    default: len = 40 + Rand(n, k + 1) % (kLineMax - 40); break;
    }
    l.len = len;
    for (u32 p = 0; p < len; ++p) {
        const u32 r = Rand(n, k + 2 + p);
        u8 ch;
        switch (mode) {
        case 0: ch = u8(0x20 + r % 95); break;                                  // solo imprimibles
        case 1: ch = u8(1 + r % 255); break;                                    // cualquier byte distinto de cero
        case 2: ch = u8((r % 4 == 0) ? 1 + (r >> 8) % 31 : 0x20 + (r >> 8) % 95); break;   // imprimibles con controles
        case 3: ch = u8(0x80 + r % 128); break;                                 // bytes altos
        case 4: ch = u8(r % 3 == 0 ? "\n\r\t \x7F\x1B"[(r >> 8) % 6] : 0x30 + (r >> 8) % 10); break;
        default: ch = u8(1 + r % 255); break;
        }
        if (ch == 0) ch = 1;
        l.b[p] = ch;
    }
    if (Rand(n, k + 300) % 3 == 0 && len) l.b[len - 1] = '\n';
    l.b[len] = 0;
    const u32 rr = Rand(n, k + 301) % 16;
    l.ret = rr < 7 ? 7 : rr < 12 ? 8 : rr == 12 ? -1 : i32(Rand(n, k + 302) % 7);
    const u32 fm = Rand(n, k + 303) % 4 == 0 ? 1u : 0u;
    for (unsigned j = 0; j < 7; ++j) l.f[j] = ViewFloat(n, k + 310 + j, fm);
    l.nameLen = Rand(n, k + 320) % 5 == 0 ? 0 : 1 + Rand(n, k + 321) % kNameMax;
    for (u32 p = 0; p < l.nameLen; ++p) l.name[p] = u8(0x21 + Rand(n, k + 322 + p) % 94);
    l.name[l.nameLen] = 0;
}

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n % kFamilies;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.ecx = SeedReg(n, 4); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.numBefore = Rand(n, 10) % 3 == 0 ? 0u : Rand(n, 11) % 5 == 0 ? Rand(n, 12) : 1u + Rand(n, 13) % 6;
    c.listBefore = Rand(n, 14) % 3 == 0 ? 0u : Rand(n, 15) | 1u;
    c.curBefore = Rand(n, 16) % 3 == 0 ? 0xFFFFFFFFu : Rand(n, 17);
    c.pathNull = Rand(n, 18) % 8 == 0;
    c.fopenOk = Rand(n, 19) % 6 != 0;
    c.nLines = 1 + Rand(n, 20) % kMaxLines;
    c.failAt = Rand(n, 21) % 8 == 0 ? 1 + Rand(n, 22) % 6 : 0;
    const u32 mode = Rand(n, 23) % 3 == 0 ? Rand(n, 24) % 5 : 0u;
    for (unsigned i = 0; i < kMaxLines; ++i) GenLine(c, c.line[i], i, mode);
    switch (c.family) {
    case 0: c.fopenOk = 0; break;                                           // fopen falla
    case 1: c.fopenOk = 1; c.pathNull = 0; c.failAt = 0; c.nLines = 0; break;   // fichero vacio
    case 2: c.fopenOk = 1; c.failAt = 0; for (auto& l : c.line) l.ret = 7; break;   // todas sin nombre
    case 3: c.fopenOk = 1; c.failAt = 0; for (auto& l : c.line) l.ret = 8; break;   // todas con nombre
    case 4: c.fopenOk = 1; c.failAt = 0; for (auto& l : c.line) l.ret = i32(Rand(n, 90) % 7) - 1; break;   // ninguna valida
    case 5: c.fopenOk = 1; c.failAt = 1; for (auto& l : c.line) l.ret = Rand(n, 91) % 2 ? 7 : 8; break;   // falla el primer Malloc (nodo)
    case 6: c.fopenOk = 1; c.failAt = 2; for (auto& l : c.line) l.ret = Rand(n, 92) % 2 ? 7 : 8; break;   // falla el segundo (descripcion): Free del nodo
    case 7: c.fopenOk = 1; c.failAt = 3 + Rand(n, 93) % 6; for (auto& l : c.line) l.ret = Rand(n, 94) % 2 ? 7 : 8; break;   // falla mas tarde
    case 8: c.fopenOk = 1; c.failAt = 0; for (unsigned i = 0; i < kMaxLines; ++i) GenLine(c, c.line[i], i, 0); break;   // solo imprimibles
    case 9: c.fopenOk = 1; c.failAt = 0; for (unsigned i = 0; i < kMaxLines; ++i) GenLine(c, c.line[i], i, 1); break;   // cualquier byte
    case 10: c.fopenOk = 1; c.failAt = 0; for (unsigned i = 0; i < kMaxLines; ++i) GenLine(c, c.line[i], i, 3); break;  // bytes altos
    case 11: c.fopenOk = 1; c.failAt = 0; for (unsigned i = 0; i < kMaxLines; ++i) GenLine(c, c.line[i], i, 4); break;  // controles y espacios
    case 12: c.fopenOk = 1; c.failAt = 0; for (auto& l : c.line) { l.len = 0; l.b[0] = 0; } break;                       // lineas vacias
    case 13: c.fopenOk = 1; c.failAt = 0; for (auto& l : c.line) { for (unsigned j = 0; j < 7; ++j) l.f[j] = ViewFloat(n, 400 + j, 1); } break;   // floats especiales
    case 14: c.fopenOk = 1; c.failAt = 0; c.pathNull = 1; break;                                                       // ruta nula
    case 15: c.numBefore = 0; c.listBefore = 0; c.fopenOk = 1; c.failAt = 0; break;                                    // sin vistas previas
    case 16: c.fopenOk = 1; c.failAt = 0; c.numBefore = Rand(n, 95) % 2 ? 0xFFFFFFFFu : 0x80000000u; break;            // contador previo extremo
    case 17: c.fopenOk = 1; c.failAt = 0; for (auto& l : c.line) { l.nameLen = kNameMax; for (u32 p = 0; p < kNameMax; ++p) l.name[p] = u8(0x21 + Rand(n, 500 + p) % 94); l.ret = 8; } break;   // nombres de 40 caracteres
    case 18: c.fopenOk = 1; c.failAt = 0; for (auto& l : c.line) { l.len = kLineMax; for (u32 p = 0; p < kLineMax; ++p) l.b[p] = u8(0x20 + Rand(n, 600 + p) % 95); l.b[kLineMax] = 0; } break;   // lineas largas
    default: break;                                                                                                     // 19: aleatorio
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("num_before", c.numBefore).U("list_before", c.listBefore).U("cur_before", c.curBefore).U("path_null", c.pathNull).U("fopen_ok", c.fopenOk).U("n_lines", c.nLines).U("fail_at", c.failAt);
    static const char* const lk[kMaxLines]{"line0", "line1", "line2", "line3", "line4"};
    static const char* const rk[kMaxLines]{"ret0", "ret1", "ret2", "ret3", "ret4"};
    static const char* const fk[kMaxLines]{"floats0", "floats1", "floats2", "floats3", "floats4"};
    static const char* const nk[kMaxLines]{"name0", "name1", "name2", "name3", "name4"};
    for (unsigned i = 0; i < kMaxLines; ++i) {
        j.H(lk[i], c.line[i].b.data(), c.line[i].len);
        j.U(rk[i], u32(c.line[i].ret));
        u8 fb[28];
        std::memcpy(fb, c.line[i].f, 28);
        j.H(fk[i], fb, 28);
        j.H(nk[i], c.line[i].name.data(), c.line[i].nameLen);
    }
    return j.End();
}
}

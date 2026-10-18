#pragma once
// Fixtures del banco ModifySpeed27: CAutoPilot::ModifySpeed(float target) en 0x41B980 (thiscall, 1 arg float en pila, `ret 4`, sin valor de retorno).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco,
// que lleva todo lo necesario; Mix/Rand/FillWords solo hacen falta para regenerar las fixtures.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace ModifySpeed27Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4096;

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

// ------------------------------------------------------------------------------------------------ disposicion de memoria
constexpr u32 kApOffset = 0x40;                 // CAutoPilot dentro de su arena
constexpr u32 kApBytes = 0x98;                  // sizeof(CAutoPilot)
constexpr u32 kApArena = kApOffset + kApBytes + 0x40;
constexpr u32 kLinkBytes = 14;                  // sizeof(CCarPathLink)
constexpr u32 kNaviArena = 1024 * kLinkBytes + 128;   // 1024 enlaces por area
constexpr u32 kTimerAddress = 0xB7CB84;         // CTimer::m_snTimeInMilliseconds (uint32)
// ThePaths (0x96F050): m_pPathNodes[72] en +0x804 (0x96F854) y m_pNaviNodes[72] en +0x924 (0x96F974): una sola region contigua.
constexpr u32 kPathNodesAddress = 0x96F854, kNaviNodesAddress = 0x96F974, kPathsBytes = 0x240, kAreas = 72;

// ------------------------------------------------------------------------------------------------ valores notables
inline u32 Quiet(u32 b) {   // nunca sNaN: el x87 los silencia al cargar y la DLL los copiaria crudos
    if ((b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) && !(b & 0x400000u)) b |= 0x400000u;
    return b;
}
inline float BitsToFloat(u32 b) { float f; std::memcpy(&f, &b, 4); return f; }
inline u32 FloatToBits(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }

// target: 0, -0, 0,01f y vecinos, negativos, NaN (+/-), inf, minimo denormal, tipicos, FLT_MAX
inline constexpr std::array<u32, 24> kTargets{0x00000000u, 0x80000000u, 0x3C23D70Au, 0x3C23D70Bu, 0x3C23D709u, 0xBF800000u, 0x7FC00000u, 0xFFC00000u,
    0x7F800000u, 0xFF800000u, 0x00000001u, 0x3F800000u, 0x447A0000u, 0x40A00000u, 0x42C80000u, 0x7F7FFFFFu, 0x3DCCCCCDu, 0x41200000u, 0x41F00000u, 0x7FC00001u,
    0x3C23D70Cu, 0xC2C80000u, 0x00800000u, 0x42480000u};
// devolucion de CCurves::CalcSpeedScaleFactor: tipicos, 0, negativos, enormes, minusculos, NaN/inf
inline constexpr std::array<u32, 24> kScaleRets{0x3F800000u, 0x00000000u, 0x80000000u, 0xBF800000u, 0x40000000u, 0x3F000000u, 0x3DCCCCCDu, 0x40400000u, 0x41200000u,
    0x447A0000u, 0x4E6E6B28u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x00800000u, 0x1E3CE508u, 0x7FC00000u, 0x7F800000u, 0xFF800000u, 0x5F000000u, 0x5F800000u, 0xDF000000u,
    0x4B000000u, 0x4B800001u, 0xC0A00000u};
inline constexpr std::array<i32, 20> kSpeedScales{1000, 1, 2, -1, -2, 0x7FFFFFFF, i32(0x80000000u), 100, 10000, 50000, 2000, -1000, 3, 0x40000000, 0x10000,
    1 << 24, (1 << 24) + 1, 7, 13, 999};
inline constexpr std::array<i8, 12> kSmth{0, 1, 2, 3, -1, -2, 4, 7, 127, -128, 5, 6};
inline constexpr std::array<i8, 10> kLanes{0, 1, 2, 3, -1, 4, 5, 127, -128, -3};
inline constexpr std::array<i8, 13> kDirs{0, 1, -1, 100, -100, 127, -128, 50, -50, 25, 99, -99, 10};
inline constexpr std::array<u8, 10> kWidths{0, 255, 16, 128, 1, 127, 64, 32, 200, 8};
inline constexpr std::array<i16, 9> kPos{0, 32767, -32768, 1, -1, 8000, -8000, 4096, -4096};

struct F {
    u32 kind, target, now, fieldC, scaleRet, headSeed, eax, edx, stack;
    i32 speedScale;
    u16 cur, next;
    u32 pres;                 // 0 = area actual sin cargar, 1 = actual cargada y siguiente sin cargar, 2 = ambas cargadas
    i8 smthCurr, smthNext, lane, nextLane;
    u8 linkA[kLinkBytes], linkB[kLinkBytes];
};

// Enlace con flags (+0xB) en las tres ramas de 0x44DB00 / OneWayLaneOffset:
//   0 = opp==0 (ancho = 0,5 - same*0,5), 1 = opp!=0 y same==0, 2 = opp!=0 y same!=0 (ancho = byte +0xA * 0,011574074 + 0,5); 3..6 = variantes y bytes extremos.
inline void MakeLink(u8* link, u32 c, u32 salt, u32 branch) {
    FillWords(link, kLinkBytes, Rand(c, salt));
    const i16 px = (Rand(c, salt + 1) % 3) ? i16(Rand(c, salt + 2)) : kPos[Rand(c, salt + 3) % kPos.size()];
    const i16 py = (Rand(c, salt + 4) % 3) ? i16(Rand(c, salt + 5)) : kPos[Rand(c, salt + 6) % kPos.size()];
    std::memcpy(link, &px, 2); std::memcpy(link + 2, &py, 2);
    link[8] = u8((Rand(c, salt + 7) % 3) ? kDirs[Rand(c, salt + 8) % kDirs.size()] : i8(Rand(c, salt + 9)));
    link[9] = u8((Rand(c, salt + 10) % 3) ? kDirs[Rand(c, salt + 11) % kDirs.size()] : i8(Rand(c, salt + 12)));
    link[0xA] = (Rand(c, salt + 13) % 2) ? kWidths[Rand(c, salt + 14) % kWidths.size()] : u8(Rand(c, salt + 15));
    const u32 hi = Rand(c, salt + 16) & 0xC0u;   // bits 6-7 libres
    const u32 opp = 1 + Rand(c, salt + 17) % 7, same = 1 + Rand(c, salt + 18) % 7;
    switch (branch) {
    case 0: link[0xB] = u8(hi | ((Rand(c, salt + 19) % 8) << 3)); break;                  // opp == 0 (same 0..7)
    case 1: link[0xB] = u8(hi | opp); break;                                               // opp != 0, same == 0
    case 2: link[0xB] = u8(hi | (same << 3) | opp); break;                                 // opp != 0, same != 0
    case 3: link[0xB] = u8(hi | (same << 3)); break;                                       // opp == 0, same != 0
    case 4: link[0xB] = 0; break;
    case 5: link[0xB] = 0xFF; break;
    default: link[0xB] = u8(Rand(c, salt + 20)); break;
    }
}

inline F MakeF(unsigned n) {
    F f{};
    const u32 c = n;
    f.kind = (n / 8) % 8;      // familia de tiempos: ver abajo
    f.target = (n % 3) == 0 ? kTargets[(n / 3 + Rand(c, 1)) % kTargets.size()] : (n % 5) == 0 ? Quiet(Rand(c, 2)) : FloatToBits(float(Rand(c, 3) % 30000) / 100.0f);
    f.pres = (n % 8) == 0 ? 0u : (n % 8) == 1 ? 1u : 2u;
    f.scaleRet = (n % 4) == 0 ? kScaleRets[Rand(c, 4) % kScaleRets.size()] : (n % 4) == 1 ? FloatToBits(float(i32(Rand(c, 5) % 400000) - 100000) / 10000.0f)
                                                                           : FloatToBits(float(Rand(c, 6) % 4000) / 1000.0f);
    f.speedScale = (Rand(c, 7) % 4) ? kSpeedScales[Rand(c, 8) % kSpeedScales.size()] : i32(Rand(c, 9) | 1u);   // nunca 0 (division por cero)
    f.smthCurr = (Rand(c, 10) % 4) ? kSmth[Rand(c, 11) % kSmth.size()] : i8(Rand(c, 12));
    f.smthNext = (Rand(c, 13) % 4) ? kSmth[Rand(c, 14) % kSmth.size()] : i8(Rand(c, 15));
    f.lane = (Rand(c, 16) % 4) ? kLanes[Rand(c, 17) % kLanes.size()] : i8(Rand(c, 18));
    f.nextLane = (Rand(c, 19) % 4) ? kLanes[Rand(c, 20) % kLanes.size()] : i8(Rand(c, 21));
    f.headSeed = Rand(c, 22); f.eax = Rand(c, 23); f.edx = Rand(c, 24); f.stack = StackSeed(n);
    // ---- tiempos
    const u32 r1 = Rand(c, 30), r2 = Rand(c, 31);
    switch (f.kind) {
    case 0: f.now = r1 % 100000u; f.fieldC = f.now - (r2 % (f.now + 1)); break;                       // diferencia >= 0 pequena
    case 1: f.now = r1 % 100000u; f.fieldC = f.now + 1 + r2 % 50000u; break;                           // diferencia negativa (32 bits)
    case 2: f.now = 0x80000000u | r1; f.fieldC = f.now - (r2 % 100000u); break;                        // ahora >= 2^31 (negativo con signo)
    case 3: f.now = 0x80000000u | r1; f.fieldC = r2 % 1000u; break;                                    // diferencia enorme (puede ser negativa con signo)
    case 4: f.now = r1; f.fieldC = r2; break;
    case 5: f.now = r1; f.fieldC = r1; break;                                                          // diferencia 0
    case 6: {                                                                                          // empates a 24 bits (ver README: doble redondeo de la fuente)
        const u32 e = 1 + Rand(c, 32) % 8, mant = 0x800000u + (Rand(c, 33) & 0x7FFFFFu);
        f.now = (mant << e) | (1u << (e - 1)); f.fieldC = f.now - 1; f.pres = 2;
        f.target = 0x447A0000u;   // 1000,0: 1000/1000 = 1
        constexpr std::array<u32, 6> rets{0x3F800000u, 0x40000000u, 0x40400000u, 0xBF800000u, 0xC0400000u, 0x3F000000u};
        f.scaleRet = rets[Rand(c, 34) % rets.size()];
        f.speedScale = i32(0x40000000u + (Rand(c, 35) & 0x3FFFFFFFu));
        break;
    }
    default: f.now = 0x80000000u | r1; f.fieldC = 0x80000000u | r2; break;
    }
    // ---- direcciones (area << 10 | indice)
    const u32 amode = (n / 64) % 4;
    u32 areaA = Rand(c, 40) % 64, areaB = Rand(c, 41) % 64, idxA = Rand(c, 42) % 1024, idxB = Rand(c, 43) % 1024;
    if (amode == 1) { areaB = areaA; if (idxB == idxA) idxB = (idxA + 1) & 1023; }
    else if (amode == 2) { areaB = areaA; idxB = idxA; }
    else if (amode == 3) { areaA = (Rand(c, 44) & 1) ? 63 : 0; idxA = (Rand(c, 45) & 1) ? 1023 : 0; areaB = (Rand(c, 46) & 1) ? 63 : 0; idxB = (Rand(c, 47) & 1) ? 1023 : 0; }
    else if (areaB == areaA) areaB = (areaA + 1 + Rand(c, 48) % 62) & 63;
    if (f.pres == 1 && areaB == areaA) areaB = (areaA + 1) & 63;   // "siguiente sin cargar" exige otra area
    f.cur = u16((areaA << 10) | idxA); f.next = u16((areaB << 10) | idxB);
    MakeLink(f.linkA, c, 100, Rand(c, 50) % 8);
    if (f.cur == f.next) std::memcpy(f.linkB, f.linkA, kLinkBytes); else MakeLink(f.linkB, c, 200, Rand(c, 51) % 8);
    return f;
}
}

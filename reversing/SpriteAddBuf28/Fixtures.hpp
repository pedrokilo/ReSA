#pragma once
// Fixtures del banco SpriteAddBuf28: CSprite2d::AddToBuffer(posn, color, u1, v1, u2, v2, u3, v3, u4, v4) en 0x728200
// (cdecl: 10 dwords en pila, `ret` sin limpiar; sin valor de retorno). Solo C++ estandar: las incluye LiveComparison.hpp.
// Todo se deriva de Rand(caso, clave) (funciones puras): fixture_oracle.py/make_f() es un espejo EXACTO (misma aritmetica) para calibrar la cobertura y probar el oraculo.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace SpriteAddBuf28Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4096;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline u64 Fnv64(const u8* p, std::size_t n) {
    u64 h = 0xCBF29CE484222325ull;
    for (std::size_t i = 0; i < n; ++i) { h ^= p[i]; h *= 0x100000001B3ull; }
    return h;
}
inline u32 Crc32(const u8* p, std::size_t n) {   // CRC-32 estandar (igual que zlib.crc32)
    u32 crc = 0xFFFFFFFFu;
    for (std::size_t i = 0; i < n; ++i) {
        crc ^= p[i];
        for (int k = 0; k < 8; ++k) crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
    }
    return ~crc;
}
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }

// ------------------------------------------------------------------------------------------------ disposicion de memoria (EXE)
constexpr u32 kVertexBufferAddress = 0xC5F958;     // aRadiosityVertexBuffer: 1532 vertices de 28 bytes
constexpr u32 kVertexBytes = 28, kVertexCount = 1532, kVertexBufferBytes = kVertexBytes * kVertexCount;   // 42896
constexpr u32 kIndexBufferAddress = 0xC4B958;      // aTempBufferIndices: 4096 uint16
constexpr u32 kIndexCount = 4096, kIndexBufferBytes = kIndexCount * 2;                                    // 8192
constexpr u32 kNextIndexAddress = 0xC80458;        // CSprite2d::nextBufferIndex (int32)
constexpr u32 kNextVertexAddress = 0xC8045C;       // CSprite2d::nextBufferVertex (int32)
constexpr u32 kWindowBytes = 4 * kVertexBytes;     // 112: los 4 vertices que escribe 0x727A00
constexpr u32 kGuardBytes = 32;                    // vigilancia a cada lado del bufer de indices
constexpr u32 kArgsArena = 128, kRectOffset = 32, kColorOffset = 56;   // arena de argumentos: rect (16 bytes) y color (4 bytes) con guardas

// ------------------------------------------------------------------------------------------------ valores notables
inline u32 Quiet(u32 b) {   // nunca sNaN en los casos "normales"
    if ((b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) && !(b & 0x400000u)) b |= 0x400000u;
    return b;
}
inline u32 FloatToBits(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
inline constexpr std::array<u32, 24> kSpecialFloats{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x00000001u, 0x80000001u, 0x00800000u, 0x3F800001u, 0x447A0000u, 0x44800000u, 0xC4800000u, 0x3C23D70Au, 0x4B000000u, 0x7FC00001u,
    0x42C80000u, 0x43000000u, 0x3FC00000u};
inline constexpr std::array<u32, 4> kSignalingNaNs{0x7F800001u, 0x7FA00000u, 0xFF800001u, 0xFFBFFFFFu};
inline constexpr std::array<u32, 12> kColors{0x00000000u, 0xFFFFFFFFu, 0xFF000000u, 0x000000FFu, 0x01020304u, 0x80808080u, 0xFF00FF00u, 0x00FF00FFu, 0xFEFEFEFEu, 0x7F7F7F7Fu,
    0x0000FF00u, 0x00FF0000u};
// nextBufferVertex "raros" (la funcion lo usa tal cual como indice/aritmetica de 32 bits): negativos, truncamientos a 16 bits, enormes.
inline constexpr std::array<i32, 24> kWeirdVertices{-1, -2, -3, -4, -5, -1000, i32(0x80000000u), i32(0x80000001u), 0xFFFC, 0xFFFD, 0xFFFE, 0xFFFF, 0x10000, 0x10001, 0x10003,
    100000, 0x7FFFFFFB, 0x7FFFFFF0, 0x12345678, -0x12345678, 65532, 70000, 0x40000000, -65536};

struct F {
    u32 kind;
    i32 vertex, index;       // nextBufferVertex / nextBufferIndex ANTES de la llamada
    u32 rect[4];             // bits (left, bottom, right, top) tal como estan en memoria
    u32 color;               // dword del CRGBA (r, g, b, a = bytes 0..3)
    u32 uv[8];               // u1, v1, u2, v2, u3, v3, u4, v4 (bits)
    u32 vseed, iseed;        // patrones previos de los bufers: byte[i] = (i*7 + vseed) & 255 / (i*13 + iseed) & 255
    u32 eax, ecx, edx, stack;
};

inline u32 PickFloat(u32 c, u32 slot) {
    const u32 r = Rand(c, 200 + slot * 4) % 16;
    if (r < 6) return kSpecialFloats[Rand(c, 201 + slot * 4) % kSpecialFloats.size()];
    if (r < 8) return Quiet(Rand(c, 202 + slot * 4));
    return FloatToBits(float(i32(Rand(c, 203 + slot * 4) % 40000u) - 10000) / 16.0f);
}

inline F MakeF(unsigned n) {
    F f{};
    const u32 c = n;
    f.kind = n % 32;
    const u32 r1 = Rand(c, 1), r2 = Rand(c, 2);
    switch (f.kind) {
    case 0: f.vertex = 0; f.index = 0; break;                                                                 // bufer vacio
    case 1: f.vertex = 1528; f.index = i32(r2 % 4000u); break;                                                 // 1528+4 = 1532: no lleno por vertices
    case 2: f.vertex = 1529; f.index = i32(r2 % 4000u); break;                                                 // 1533: lleno por vertices (ventana fuera del bufer)
    case 3: f.vertex = 1529 + i32(r1 % 7u); f.index = i32(r2 % 4085u); break;                                  // lleno por vertices (1529..1535)
    case 4: f.vertex = i32(r1 % 1500u); f.index = 4084; break;                                                 // 4084+6 = 4090: no lleno por indices
    case 5: f.vertex = i32(r1 % 1500u); f.index = 4085; break;                                                 // 4091: lleno por indices
    case 6: f.vertex = i32(r1 % 1500u); f.index = 4086 + i32(r2 % 5u); break;                                  // 4086..4090
    case 7: f.vertex = 1529 + i32(r1 % 7u); f.index = 4085 + i32(r2 % 6u); break;                              // llenos los dos
    case 8: f.vertex = i32(r1 % 1529u); f.index = 0; break;
    case 9: f.vertex = kWeirdVertices[r1 % kWeirdVertices.size()]; f.index = i32(r2 % 4085u); break;          // vertices raros, indices no llenos
    case 10: f.vertex = kWeirdVertices[r1 % kWeirdVertices.size()]; f.index = 4085 + i32(r2 % 6u); break;     // raros + lleno por indices (Render con contadores <= 0 no reinicia)
    case 11: f.vertex = 1527; f.index = 4084; break;
    case 12: f.vertex = 4 * i32(r1 % 382u); f.index = 6 * i32(r2 % 681u); break;                               // alineados como en el juego (cuatro a cuatro, seis a seis)
    case 13: f.vertex = 1524 + i32(r1 % 5u); f.index = 4078 + i32(r2 % 7u); break;                             // justo en el limite de los dos
    default: f.vertex = i32(Rand(c, 3) % 1536u); f.index = i32(Rand(c, 4) % 4091u); break;                       // aleatorio (los indices nunca rebasan 4090: la funcion escribe 6 desde ahi)
    }
    // ---- floats (posn y uv): 12 ranuras
    for (u32 k = 0; k < 4; ++k) f.rect[k] = PickFloat(c, k);
    for (u32 k = 0; k < 8; ++k) f.uv[k] = PickFloat(c, 4 + k);
    if (n % 64 == 5) {   // familia de NaN senalizantes (bits crudos; ver README)
        const u32 slot = Rand(c, 60) % 12;
        const u32 v = kSignalingNaNs[Rand(c, 61) % kSignalingNaNs.size()];
        if (slot < 4) f.rect[slot] = v; else f.uv[slot - 4] = v;
    }
    f.color = (n % 4 == 0) ? kColors[Rand(c, 70) % kColors.size()] : Rand(c, 71);
    f.vseed = Rand(c, 80) & 0xFF; f.iseed = Rand(c, 81) & 0xFF;
    f.eax = Rand(c, 90); f.ecx = Rand(c, 91); f.edx = Rand(c, 92); f.stack = StackSeed(n);
    return f;
}
}

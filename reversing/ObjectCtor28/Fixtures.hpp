#pragma once
// Fixtures del banco ObjectCtor28: CObject::CObject(int32 modelId, bool bCreate) en 0x5A1D70 (thiscall, ECX = this, 2 argumentos en pila, `ret 8`, devuelve this en EAX).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco.
//
// El constructor del EXE hace, en este orden (desensamblado 0x5A1D70..0x5A1DDE):
//   1. prologo SEH (fs:[0]); CPhysical::CPhysical(this) (0x542260)                                  -> colaborador SUSTITUIDO (falso identico en ambos lados)
//   2. [this+0] = 0x866F60 (vtable de CObject); [this+0x170] = 0 (m_pDummyObject)
//   3. si el BYTE BAJO del argumento bCreate != 0: CEntity::SetModelIndex(this, modelId) (0x532AE0)  -> colaborador SUSTITUIDO
//      si no:                                       CEntity::SetModelIndexNoCreate(this, modelId) (0x533700) -> colaborador SUSTITUIDO
//   4. CObject::Init(this) (0x59F840)                                                                 -> colaborador SUSTITUIDO
//   5. epilogo SEH; EAX = this; ret 8.
// El constructor NO escribe nada mas (en particular no escribe m_nObjectType +0x13C: eso es del constructor por defecto 0x5A1D10).
// Los falsos (ver Core.hpp) dejan el objeto en un estado deterministico y registran una traza (this relativo, argumentos, vptr observado, +0x170): asi el banco comprueba el ORDEN de las
// llamadas, los argumentos, la seleccion Create/NoCreate por el byte bajo, el vptr y los dos unicos stores propios (vptr, +0x170), y que nada mas del objeto ni de las guardas cambie.
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace ObjectCtor28Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 6144;

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

// ------------------------------------------------------------------------------------------------ disposicion de memoria
constexpr u32 kObjBytes = 0x17C;                 // sizeof(CObject)
constexpr u32 kPhysBytes = 0x138;                // sizeof(CPhysical): lo que escribe el falso del constructor base (+ vptr marcador en +0)
constexpr u32 kInitAt = 0x138;                   // el falso de Init escribe ruido en los dwords de indice impar de [0x138, 0x17C) (el +0x170 es el indice 14: no lo toca)
constexpr u32 kDummyAt = 0x170;                  // m_pDummyObject
constexpr u32 kModelAt = 0x22;                   // m_nModelIndex (word): lo escriben los falsos SetModelIndex/NoCreate
constexpr u32 kMarkAt = 0x24;                    // word: marcador del falso (0x5E01 = SetModelIndex, 0x5E02 = SetModelIndexNoCreate)
constexpr u32 kGuard = 0x40;                     // guarda antes y despues del objeto
constexpr u32 kArena = kGuard + 0x10 + kObjBytes + kGuard;
constexpr u32 kObjOffsets[4]{0x40, 0x44, 0x48, 0x4C};
constexpr u32 kBaseVptrMarker = 0xBA5E0000u;     // lo que deja el falso del constructor base en +0
constexpr u32 kVtLabel = 0xC0B1EC70u;            // etiqueta con la que se sustituye el vptr propio de cada lado (0x866F60 en el EXE, la vtable de la DLL en el candidato)

// ------------------------------------------------------------------------------------------------ valores notables
// Argumento modelId (dword): los llamadores del EXE usan movzx/movsx de 16 bits o un dword de memoria; el banco recorre todo el dword.
inline constexpr u32 kSpecialModels[]{
    0u, 1u, 2u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0xFFFFu, 0x10000u, 0x7FFFu, 0x8000u, 0xFFFF8000u, 0xFFFF7FFFu, 19999u, 20000u, 0xFFFFFF00u, 0x00010000u, 0xDEADBEEFu,
    0xFFFF0000u, 0x0000FF00u, 0x000000FFu, 0x100u, 0x80u, 0x7Fu, 0x8000FFFFu, 0xFFFFFFFEu, 0x80000001u, 0x7FFFFFFEu, 18631u, 1337u, 0x00FFFFFFu, 0x01000000u,
};
// Argumento bCreate (dword en la pila): el EXE solo mira el BYTE BAJO (`mov cl,[esp+1Ch]; cmp cl,al`); los llamadores reales empujan 0 o 1.
inline constexpr u32 kCreateNonZeroLow[]{0x02u, 0x80u, 0xFFu, 0x7Fu, 0x01010101u, 0xAB0000CDu, 0xFFFFFF01u, 0x00000100u | 0x01u, 0x12345601u, 0x81u};
inline constexpr u32 kCreateZeroLow[]{0x100u, 0x01000000u, 0xFFFFFF00u, 0xAB000000u, 0x00010000u, 0x7FFFFF00u, 0x80000000u, 0x00FF0000u, 0x12345600u, 0xDEADBE00u};

struct F {
    u32 n, modelKind, createKind, mode, objOff, seedArena, seedP, seedI;
    u32 modelId, create;          // modelId (dword) y bCreate (dword completo de la pila)
    u32 eax, edx, stack;
    u8 pre[kObjBytes];            // contenido previo del objeto (el resto de la arena es la guarda)
};

// Mezcla del contenido previo de la arena: 0 ruido, 1 ceros, 2 0xFF, 3 0xCD, 4 0xA5, 5 ruido (otra semilla)
inline void FillArena(u8* arena, u32 mode, u32 seed) {
    switch (mode) {
    case 1: std::memset(arena, 0x00, kArena); break;
    case 2: std::memset(arena, 0xFF, kArena); break;
    case 3: std::memset(arena, 0xCD, kArena); break;
    case 4: std::memset(arena, 0xA5, kArena); break;
    default: FillWords(arena, kArena, seed); break;
    }
}

inline F Make(unsigned n) {
    F f{};
    f.n = n;
    f.modelKind = Rand(n, 1) % 8;
    switch (f.modelKind) {
    case 0: f.modelId = Rand(n, 11) % 41u; break;                                                                  // ids pequenos
    case 1: case 2: f.modelId = Rand(n, 11) % 20000u; break;                                                       // ids reales (0..19999)
    case 3: case 4: f.modelId = kSpecialModels[Rand(n, 11) % std::size(kSpecialModels)]; break;                    // extremos
    case 5: f.modelId = Rand(n, 11); break;                                                                        // dword aleatorio
    case 6: f.modelId = u32(-i32(1 + Rand(n, 11) % 40000u)); break;                                                // negativos pequenos (movsx)
    default: f.modelId = Rand(n, 11) & 0xFFFFu; break;                                                             // 16 bits (movzx)
    }
    f.createKind = Rand(n, 2) % 10 < 4 ? 0 : Rand(n, 2) % 10 < 8 ? 1u : Rand(n, 2) % 10 == 8 ? 2u : 3u;            // 0: 0 | 1: 1 | 2: byte bajo != 0 no canonico | 3: byte bajo == 0 con el resto != 0
    switch (f.createKind) {
    case 0: f.create = 0; break;
    case 1: f.create = 1; break;
    case 2: f.create = kCreateNonZeroLow[Rand(n, 12) % std::size(kCreateNonZeroLow)]; break;
    default: f.create = kCreateZeroLow[Rand(n, 12) % std::size(kCreateZeroLow)]; break;
    }
    f.mode = Rand(n, 3) % 6;
    f.objOff = kObjOffsets[Rand(n, 4) % 4];
    f.seedArena = Rand(n, 5) | 1u;
    f.seedP = Rand(n, 6);
    f.seedI = Rand(n, 7);
    f.eax = Rand(n, 8) % 5 == 0 ? 0u : Rand(n, 9);
    f.edx = Rand(n, 10) % 5 == 0 ? 0xFFFFFFFFu : Rand(n, 13);
    f.stack = StackSeed(Rand(n, 14) % 12);
    std::array<u8, kArena> arena{};
    FillArena(arena.data(), f.mode, f.seedArena);
    std::memcpy(f.pre, arena.data() + f.objOff, kObjBytes);
    return f;
}
}

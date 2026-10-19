#pragma once
// Fixtures del banco PartnerChatAbort30: CTaskComplexPartnerChat::MakeAbortable (0x682C60, thiscall con 3 argumentos de pila, `ret 0Ch`).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y tambien un programa autonomo de calibracion (fixture_mirror.py).
//
// La funcion llama a m_pSubTask->MakeAbortable (vtable +0x18, argumentos ped / prioridad / evento reenviados tal cual) y, si devuelve AL != 0, comprueba los BYTES
// m_conversationEnabled (+0x74) y m_pedConversationLoaded (+0x76): si ambos son distintos de 0 llama a CAEPedSpeechAudioEntity::ReleasePedConversation (0x4E52A0) y
// pone +0x76 a 0. Devuelve 1 (AL) si la subtarea fue abortable y 0 si no (sin tocar nada). No comprueba nulo en la subtarea (no se prueba).
// Cada caso fija: AL (y los 24 bits altos de EAX) del retorno de la subtarea falsa, los dos bytes de `this`, y los tres argumentos.
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace PartnerChatAbort30Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4608;   // 16 familias x 288

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

// ------------------------------------------------------------------------------------------------ disposicion de memoria (guarda | objeto | guarda)
constexpr u32 kThisBytes = 0x78;                       // sizeof(CTaskComplexPartnerChat)
constexpr u32 kThisOff = 0x40, kThisArena = 0x100;     // 0x40 + 0x78 = 0xB8 <= 0x100: guarda delantera 0x40, trasera 0x48
constexpr u32 kSubAt = 8, kConvAt = 0x74, kLoadedAt = 0x76;
constexpr u32 kSubBytes = 0x10;                        // objeto falso de subtarea: la vtable en +0
constexpr u32 kSubOff = 0x20, kSubArena = 0x80;        // 0x20 + 0x10 = 0x30 <= 0x80
constexpr u32 kSlots = 16;                             // entradas de la vtable falsa (todas apuntan a falsos distintos; solo la 6 es la correcta)
constexpr u32 kMakeAbortableSlot = 6;                  // [vtable + 0x18]

inline constexpr u8 kByteEdges[]{0, 1, 2, 3, 0x7F, 0x80, 0x81, 0xFE, 0xFF};
inline constexpr u32 kWordEdges[]{0u, 1u, 2u, 3u, 4u, 0x10u, 0x14u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu};

struct C {
    u32 family, res, resHigh, conv, loaded, ped, prio, event, eax, edx, stack;
};

inline u32 NonZeroByte(u32 n, u32 k) {
    const u32 r = Rand(n, k);
    if (r % 3 == 0) return kByteEdges[1 + (r >> 8) % (std::size(kByteEdges) - 1)];   // un borde distinto de 0
    const u32 b = (r >> 8) & 0xFFu;
    return b ? b : 0x5Au;
}
inline u32 AnyByte(u32 n, u32 k) {
    const u32 r = Rand(n, k);
    return r % 4 == 0 ? u32(kByteEdges[(r >> 8) % std::size(kByteEdges)]) : ((r >> 8) & 0xFFu);
}
inline u32 AnyWord(u32 n, u32 k) {
    const u32 r = Rand(n, k);
    return r % 3 == 0 ? kWordEdges[(r >> 8) % std::size(kWordEdges)] : Rand(n, k + 500);
}

// Familias (n % 16, 288 casos cada una):
//  0 subtarea no abortable (AL=0), bytes aleatorios   1 abortable, conv != 0 y cargada != 0 (libera)   2 abortable, conv != 0, cargada = 0
//  3 abortable, conv = 0, cargada != 0                 4 abortable, conv = 0, cargada = 0               5 AL no canonico (2, 0x7F, 0x80, 0xFF...) y bytes no canonicos
//  6 AL = 1, bytes no canonicos distintos de 0         7 AL = 0 con conv y cargada != 0 (NO libera)     8..15 combinaciones aleatorias
inline C MakeC(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.family = f;
    const u32 pick = Rand(n, 3) % 8;
    c.res = pick < 3 ? 0u : pick < 5 ? 1u : pick == 5 ? u32(kByteEdges[2 + Rand(n, 4) % (std::size(kByteEdges) - 2)]) : NonZeroByte(n, 5);
    c.conv = AnyByte(n, 6);
    c.loaded = AnyByte(n, 7);
    switch (f) {
    case 0: c.res = 0; break;
    case 1: c.res = 1; c.conv = Rand(n, 8) % 2 ? 1u : NonZeroByte(n, 9); c.loaded = Rand(n, 10) % 2 ? 1u : NonZeroByte(n, 11); break;
    case 2: c.res = 1; c.conv = NonZeroByte(n, 9); c.loaded = 0; break;
    case 3: c.res = 1; c.conv = 0; c.loaded = NonZeroByte(n, 11); break;
    case 4: c.res = 1; c.conv = 0; c.loaded = 0; break;
    case 5: c.res = NonZeroByte(n, 12); if (c.res == 1) c.res = 0xFF; c.conv = NonZeroByte(n, 9); c.loaded = NonZeroByte(n, 11); break;
    case 6: c.res = 1; c.conv = NonZeroByte(n, 9); c.loaded = NonZeroByte(n, 11); if (c.conv == 1) c.conv = 0x80; if (c.loaded == 1) c.loaded = 0x02; break;
    case 7: c.res = 0; c.conv = NonZeroByte(n, 9); c.loaded = NonZeroByte(n, 11); break;
    default: break;
    }
    c.resHigh = Rand(n, 13) & 0xFFFFFFu;      // 24 bits altos de EAX en el retorno de la subtarea (solo AL cuenta)
    c.ped = AnyWord(n, 14);
    c.prio = AnyWord(n, 15);
    c.event = AnyWord(n, 16);
    c.eax = Rand(n, 20);
    c.edx = Rand(n, 21);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    return c;
}
}  // namespace PartnerChatAbort30Fix

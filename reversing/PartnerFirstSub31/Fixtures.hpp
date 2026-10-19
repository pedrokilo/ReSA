#pragma once
// Fixtures del banco PartnerFirstSub31: CTaskComplexPartner::CreateFirstSubTask (0x681F20, thiscall con 1 argumento de pila `ped`, `ret 4`, marco SEH propio).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y tambien un programa autonomo de calibracion (fixture_mirror.py).
//
// La funcion lee de `this`: el BYTE m_leadSpeaker (+0x58), el dword m_taskId (+0x34), m_partner (+0x38) y, SOLO si lead != 0 y m_taskId == 0x4B4 (TASK_COMPLEX_PARTNER_CHAT), los bytes
// +0x74 (m_conversationEnabled), +0x75 (field_75) y +0x76 (m_pedConversationLoaded) de CTaskComplexPartnerChat. Orden:
//   si conv != 0: CAEPedSpeechAudioEntity::RequestPedConversation(ped, partner) (0x4E50E0, cdecl); AL != 0 -> +0x76 = 1; si no, +0x75 != 0 -> +0x74 = 0; si no -> devuelve NULO (sin mas llamadas)
//   ped->StopPlayingHandSignal() (0x5E0480) ; CTask::operator new(0x20) (0x61A5A0) ; nulo -> devuelve nulo ; CTaskSimpleStandStill(50, false, false, 8.0f) (0x62F310) ; devuelve el objeto.
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace PartnerFirstSub31Fix {
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
constexpr u32 kThisOff = 0x40, kThisArena = 0x100;     // 0x40 + 0x78 + 0x48 = 0x100
constexpr u32 kTaskIdAt = 0x34, kPartnerAt = 0x38, kLeadAt = 0x58, kConvAt = 0x74, kF75At = 0x75, kLoadedAt = 0x76;
constexpr u32 kObjBytes = 0x20;                        // sizeof(CTaskSimpleStandStill): el objeto que reserva la funcion (falso de operator new)
constexpr u32 kObjOff = 0x20, kObjArena = 0x80;        // 0x20 + 0x20 + 0x40 = 0x80
constexpr u32 kChatType = 0x4B4;                       // TASK_COMPLEX_PARTNER_CHAT

inline constexpr u8 kByteEdges[]{0, 1, 2, 3, 0x7F, 0x80, 0x81, 0xFE, 0xFF};
inline constexpr u32 kWordEdges[]{0u, 1u, 2u, 3u, 4u, 0x10u, 0x14u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu};
inline constexpr u32 kTypeNear[]{0x4B3u, 0x4B5u, 0x4B4u ^ 0x100u, 0x4B4u ^ 0x10000u, 0x4B4u ^ 0x80000000u, 0u, 0xFFFFFFFFu, 0xF4u, 0x398u, 0x4C2u, 0x4B4u ^ 1u};

struct C {
    u32 family, lead, taskId, conv, f75, loaded, req, reqHigh, allocNull, fill, ped, partner, eax, edx, stack;
};

inline u32 NonZeroByte(u32 n, u32 k) {
    const u32 r = Rand(n, k);
    if (r % 3 == 0) return kByteEdges[1 + (r >> 8) % (std::size(kByteEdges) - 1)];
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

// Familias (n % 16, 288 casos cada una); lead = m_leadSpeaker (byte entero), chat = lead != 0 y m_taskId == 0x4B4:
//  0 lead=0 y tipo chat (no pregunta)           1 lead != 0, tipo distinto (cerca de 0x4B4, un bit)   2 lead != 0, chat, conv = 0              3 chat, conv != 0, RequestPedConversation verdadero
//  4 chat, conv != 0, falso y field_75 != 0     5 chat, conv != 0, falso y field_75 = 0 (nulo, sin mas llamadas)   6 como 3 con retorno AL no canonico y bytes no canonicos
//  7 como 4 con bytes no canonicos              8 chat con agotamiento del pool (operator new nulo) y ramas al azar   9 lead no canonico (0x80, 0xFF...) con chat
// 10 loaded ya a 1 / ya a 0 (escritura redundante)   11 conv != 0 con +0x74 ya 0 tras field_75 (conv no canonico)   12..15 combinaciones aleatorias
inline C MakeC(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.family = f;
    c.lead = Rand(n, 3) % 3 == 0 ? NonZeroByte(n, 4) : (Rand(n, 5) % 2 ? 1u : 0u);
    c.taskId = Rand(n, 6) % 3 == 0 ? kTypeNear[Rand(n, 7) % std::size(kTypeNear)] : kChatType;
    c.conv = Rand(n, 8) % 2 ? NonZeroByte(n, 9) : 0u;
    c.f75 = AnyByte(n, 10);
    c.loaded = AnyByte(n, 11);
    c.req = Rand(n, 12) % 2 ? 0u : (Rand(n, 13) % 2 ? 1u : AnyByte(n, 14));
    c.reqHigh = Rand(n, 15) & 0xFFFFFFu;
    c.allocNull = Rand(n, 16) % 8 == 0;
    const bool chatCase = f == 2 || (f >= 3 && f <= 11);
    if (chatCase) { if (!c.lead) c.lead = Rand(n, 17) % 2 ? 1u : NonZeroByte(n, 18); c.taskId = kChatType; }
    switch (f) {
    case 0: c.lead = 0; c.taskId = kChatType; c.conv = NonZeroByte(n, 9); c.req = 0; c.f75 = 0; break;
    case 1: if (!c.lead) c.lead = 1; c.taskId = kTypeNear[Rand(n, 7) % std::size(kTypeNear)]; if (c.taskId == kChatType) c.taskId = 0x4B3u; c.conv = NonZeroByte(n, 9); break;
    case 2: c.conv = 0; break;
    case 3: c.conv = Rand(n, 19) % 2 ? 1u : NonZeroByte(n, 9); c.req = Rand(n, 20) % 2 ? 1u : NonZeroByte(n, 21); break;
    case 4: c.conv = Rand(n, 19) % 2 ? 1u : NonZeroByte(n, 9); c.req = 0; c.f75 = Rand(n, 20) % 2 ? 1u : NonZeroByte(n, 21); break;
    case 5: c.conv = Rand(n, 19) % 2 ? 1u : NonZeroByte(n, 9); c.req = 0; c.f75 = 0; break;
    case 6: c.conv = NonZeroByte(n, 9); if (c.conv == 1) c.conv = 0x80; c.req = NonZeroByte(n, 21); if (c.req == 1) c.req = 0xFF; break;
    case 7: c.conv = NonZeroByte(n, 9); if (c.conv == 1) c.conv = 0x80; c.req = 0; c.f75 = NonZeroByte(n, 21); if (c.f75 == 1) c.f75 = 0x02; break;
    case 8: c.allocNull = Rand(n, 22) % 2; break;
    case 9: c.lead = u32(kByteEdges[2 + Rand(n, 23) % (std::size(kByteEdges) - 2)]); break;
    case 10: c.loaded = Rand(n, 24) % 2 ? 1u : 0u; c.req = Rand(n, 25) % 3 ? NonZeroByte(n, 21) : 0u; break;
    case 11: c.conv = NonZeroByte(n, 9); c.req = 0; c.f75 = NonZeroByte(n, 21); break;
    default: break;
    }
    c.fill = (Rand(n, 26) & 0xFFu) | 1u;   // relleno de la arena del objeto reservado (distinto de 0, para ver los bytes que la funcion no toca)
    c.ped = AnyWord(n, 27);
    c.partner = AnyWord(n, 28);
    c.eax = Rand(n, 29);
    c.edx = Rand(n, 30);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    return c;
}
}  // namespace PartnerFirstSub31Fix

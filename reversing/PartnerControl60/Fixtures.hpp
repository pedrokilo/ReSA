#pragma once
// Fixtures del banco PartnerControl60: CTaskComplexPartner::ControlSubTask (0x6840D0, thiscall con 1 argumento (ped), `ret 4`, devuelve CTask* en EAX).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y tambien un programa autonomo de calibracion (fixture_mirror.py).
//
// Resumen del desensamblado (0x6840D0..0x68418D): ESI = this, EDI = ped.
//   - si el BYTE con signo m_partnerState (+0x59) <= 1 y m_partner (+0x38) != 0 y el BYTE m_taskCompleted (+0x5C) == 0: no se aborta (684126).
//   - si no, y m_partner != 0: t = CTaskManager::FindActiveTaskByType(&partner->m_pIntelligence->m_TaskMgr (= [partner+0x47C]+4), m_taskId) (0x681740); si t != 0 y t->m_partner (+0x38) == ped
//     y m_taskCompleted == 0: no se aborta (684126).
//   - en cualquier otro caso: m_pSubTask->MakeAbortable(ped, 1, 0) (vtable +0x18); si AL != 0 devuelve 0.
//   - 684126: m_pSubTask->GetTaskType() (vtable +0x10) == 0x387: ++WORD [this+0x70] y, si el word con signo > 150 (0x96), MakeAbortable(ped, 1, 0) otra vez; AL != 0 -> devuelve 0.
//   - ped->DropEntityThatThisPedIsHolding(true) (0x5E0360); this->StreamRequiredAnims() (vtable +0x2C); IKChainManager.IsLooking(ped) (0x6181A0, g_ikChainMan = 0xC15448);
//     si AL != 0: AbortLookAt(ped, 500) (0x618280). Devuelve m_pSubTask.
// Colaboradores sustituidos por falsos identicos en ambos lados: 0x681740, 0x5E0360, 0x6181A0, 0x618280; las llamadas virtuales (GetTaskType, MakeAbortable de la subtarea y
// StreamRequiredAnims de `this`) van por vtables falsas de arena con la misma ranura en el EXE y en la DLL.
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace PartnerControl60Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

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

constexpr unsigned kCases = 4096;   // 16 familias x 256

// ------------------------------------------------------------------------------------------------ disposicion de memoria (guarda | objeto | guarda)
constexpr u32 kThisBytes = 0x74;                                 // sizeof(CTaskComplexPartnerDeal): el word +0x70 pertenece a las clases derivadas
constexpr u32 kThisOff = 0x40, kThisArena = 0x100;               // 0x40 + 0x74 + 0x4C = 0x100
constexpr u32 kSubAtThis = 8, kTaskIdAt = 0x34, kPartnerAt = 0x38, kStateAt = 0x59, kCompletedAt = 0x5C, kCounterAt = 0x70;
constexpr u32 kSubOff = 0x20, kSubBytes = 0x30, kSubArena = 0x80;      // subtarea falsa: 0x20 + 0x30 + 0x30 = 0x80
constexpr u32 kPedOff = 0x10, kPedBytes = 0x20, kPedArena = 0x40;      // ped opaco (solo su direccion): 0x10 + 0x20 + 0x10 = 0x40
constexpr u32 kPartOff = 0x40, kPartBytes = 0x500, kPartArena = 0x580; // partner falso (solo se lee [+0x47C]): 0x40 + 0x500 + 0x40 = 0x580
constexpr u32 kIntelAtPartner = 0x47C;
constexpr u32 kIntelOff = 0x20, kIntelBytes = 0x40, kIntelArena = 0x80;   // inteligencia falsa (solo su direccion): 0x20 + 0x40 + 0x20 = 0x80
constexpr u32 kOtherOff = 0x20, kOtherBytes = 0x60, kOtherArena = 0xA0;   // tarea equivalente del partner (solo se lee [+0x38]): 0x20 + 0x60 + 0x20 = 0xA0
constexpr u32 kOtherPartnerAt = 0x38;
constexpr u32 kIkGlobal = 0xC15448;
constexpr u32 kGoToType = 0x387;
constexpr u32 kMaxEvents = 12;
constexpr u32 kVtSlots = 16;

// ------------------------------------------------------------------------------------------------ caso
struct C {
    u32 family, state, partnerNull, completed, found, otherIsPed, abort1, abort2, subType, counter, looking, taskId;
    u32 eax, edx, stack;
};

inline constexpr u8 kLowStates[]{1, 0, 0x80, 0xFF, 0x81, 0xFE, 1, 0};
inline constexpr u8 kHighStates[]{2, 3, 4, 5, 6, 7, 0x7F, 0x10, 0x40, 2, 5, 6};
inline constexpr u8 kAnyStates[]{0, 1, 2, 3, 4, 5, 6, 7, 0x7F, 0x80, 0x81, 0xFF, 0x10};
inline constexpr u8 kCompleted[]{1, 2, 0x80, 0xFF, 0x10, 1};
inline constexpr u32 kCounters[]{0u, 1u, 148u, 149u, 150u, 151u, 152u, 0x7FFEu, 0x7FFFu, 0x8000u, 0xFFFFu, 0xFFFEu, 0x00FFu, 0x0100u, 300u};
inline constexpr u32 kSubTypes[]{0x386u, 0x388u, 0x1387u, 0x80000387u, 0u, 0xFFFFFFFFu, 0x10387u, 0x287u, 0x3870u, 0x38Fu};
inline constexpr u32 kAl[]{0u, 1u, 2u, 0x80u, 0xFFu, 0x7Fu, 0x10u, 1u};

inline u32 Al(u32 n, u32 k, bool nonzero) {   // EAX de un bool devuelto: AL (cero o no) y, la mitad de las veces, basura en los 24 bits altos
    u32 al = nonzero ? 1u + Rand(n, k) % 255u : 0u;
    if (nonzero && Rand(n, k + 1) % 3 == 0) al = kAl[Rand(n, k + 2) % std::size(kAl)] | 1u;   // no canonico pero distinto de cero
    const u32 hi = Rand(n, k + 3) % 2 ? (Rand(n, k + 4) & 0xFFFFFF00u) : 0u;
    return hi | al;
}

// Familias (n % 16, 256 casos cada una):
//  0 estado <= 1 con signo (1, 0, 0x80, 0xFF...), partner, completed = 0: sin Find ni abortar                                1 estado <= 1, partner, completed != 0: Find y MakeAbortable (partner sincronizado)
//  2 estado > 1, partner, Find devuelve la tarea con partner == ped, completed = 0: sin abortar                              3 estado > 1, Find devuelve tarea con otro partner, MakeAbortable devuelve cierto (-> 0)
//  4 estado > 1, Find devuelve tarea con otro partner, MakeAbortable devuelve falso: sigue                                  5 Find devuelve nulo: MakeAbortable (resultado al azar)
//  6 sin partner (m_partner == 0): MakeAbortable                                                                              7 estado > 1, sincronizado pero completed != 0: MakeAbortable
//  8 subtarea 0x387, sin abortar, contador en el borde de 150 y MakeAbortable devuelve cierto                                 9 subtarea 0x387, contador en el borde, MakeAbortable devuelve falso
//  10 tipos de subtarea cercanos a 0x387 (no entran)                                                                          11 IsLooking con AL variado: AbortLookAt(ped, 500) o no
//  12..15 mezcla al azar de todos los parametros
inline C MakeC(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.family = f;
    c.taskId = Rand(n, 3) % 4 == 0 ? 0x4B4u : Rand(n, 4);
    c.eax = Rand(n, 70);
    c.edx = Rand(n, 71);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    c.subType = Rand(n, 5) % 3 == 0 ? kGoToType : Rand(n, 6) % 2 ? kSubTypes[Rand(n, 7) % std::size(kSubTypes)] : Rand(n, 8);
    c.counter = Rand(n, 9) % 4 ? kCounters[Rand(n, 10) % std::size(kCounters)] : (Rand(n, 11) & 0xFFFFu);
    c.abort1 = Al(n, 12, Rand(n, 14) % 2);
    c.abort2 = Al(n, 15, Rand(n, 17) % 2);
    c.looking = Al(n, 18, Rand(n, 20) % 2);
    c.found = Rand(n, 21) % 4 != 0;
    c.otherIsPed = Rand(n, 22) % 2;
    c.completed = Rand(n, 23) % 3 == 0 ? u32(kCompleted[Rand(n, 24) % std::size(kCompleted)]) : 0u;
    c.partnerNull = 0;
    c.state = kAnyStates[Rand(n, 25) % std::size(kAnyStates)];
    const u32 low = kLowStates[Rand(n, 26) % std::size(kLowStates)], high = kHighStates[Rand(n, 27) % std::size(kHighStates)];
    const u32 nz = kCompleted[Rand(n, 28) % std::size(kCompleted)];
    switch (f) {
    case 0: c.state = low; c.completed = 0; break;
    case 1: c.state = low; c.completed = nz; c.found = 1; c.otherIsPed = 1; break;
    case 2: c.state = high; c.completed = 0; c.found = 1; c.otherIsPed = 1; break;
    case 3:
        if (Rand(n, 29) % 3) { c.state = high; c.completed = Rand(n, 30) % 2 ? 0u : nz; } else { c.state = low; c.completed = nz; }
        c.found = 1; c.otherIsPed = 0; c.abort1 = Al(n, 12, true);
        break;
    case 4: c.state = high; c.completed = 0; c.found = 1; c.otherIsPed = 0; c.abort1 = Al(n, 12, false); break;
    case 5: c.state = Rand(n, 29) % 2 ? high : u32(kAnyStates[Rand(n, 31) % std::size(kAnyStates)]); c.found = 0; if (std::int8_t(c.state) <= 1 && c.completed == 0) c.completed = nz; break;
    case 6: c.partnerNull = 1; c.state = kAnyStates[Rand(n, 31) % std::size(kAnyStates)]; c.found = Rand(n, 32) % 2; break;
    case 7: c.state = high; c.completed = nz; c.found = 1; c.otherIsPed = 1; break;
    case 8: c.state = low; c.completed = 0; c.subType = kGoToType; c.counter = kCounters[Rand(n, 10) % std::size(kCounters)]; c.abort2 = Al(n, 15, true); break;
    case 9: c.state = high; c.completed = 0; c.found = 1; c.otherIsPed = 1; c.subType = kGoToType; c.counter = kCounters[Rand(n, 10) % std::size(kCounters)]; c.abort2 = Al(n, 15, false); break;
    case 10: c.state = low; c.completed = 0; c.subType = kSubTypes[Rand(n, 7) % std::size(kSubTypes)]; break;
    case 11: c.state = high; c.completed = 0; c.found = 1; c.otherIsPed = 1; if (c.subType == kGoToType) c.subType = 0x388; break;
    default:
        c.partnerNull = Rand(n, 33) % 8 == 0;
        break;
    }
    return c;
}
}  // namespace PartnerControl60Fix

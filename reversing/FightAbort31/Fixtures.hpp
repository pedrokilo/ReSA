#pragma once
// Fixtures del banco FightAbort31: CTaskSimpleFight::MakeAbortable (0x6239F0, thiscall con 3 argumentos de pila, `ret 0Ch`, hook VIRTUAL ranura 6 de la vtable 0x86D684).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y tambien un programa autonomo de calibracion (fixture_mirror.py).
//
// La funcion (ver fixture_oracle.py para el desensamblado): prioridad != 1 y != 2 -> m_nNextCommand = 1 y devuelve 0; con urgente (1) o inmediata (2): si hay evento y su tipo (llamada
// virtual +8, SE PIDE DOS VECES) es <= 0x1F (con signo) o == 0x3C devuelve 0 sin tocar nada mas; despues suelta m_pAnim (+0x1C: delta -1000.0 si es inmediata, SetDeleteCallback(0x4D1390, 0)),
// suelta m_pIdleAnim (+0x20: SetDeleteCallback y, si peso > 0 y delta >= 0, BlendAnimation(clump, grupo, 3, 16.0 / 1000.0)), pone a cero el movimiento de pelea del jugador (+0xC/+0x10 de los datos,
// y llama a 0x61C9B0) y marca m_bIsFinished (+8) = 1; devuelve 1.
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace FightAbort31Fix {
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
// Cada arena = guarda delantera (off) + ventana del objeto (bytes) + guarda trasera; la suma esta escrita a mano y comprobada por fixture_mirror.py contra el oraculo.
constexpr u32 kThisBytes = 0x28;                        // sizeof(CTaskSimpleFight)
constexpr u32 kThisOff = 0x40, kThisArena = 0x100;      // 0x40 + 0x28 + 0x98
constexpr u32 kAnimBytes = 0x3C;                        // CAnimBlendAssociation (hasta m_pCallbackData + 4)
constexpr u32 kAnimOff = 0x20, kAnimArena = 0xA0;       // 0x20 + 0x3C + 0x44
constexpr u32 kPedBytes = 0x4D8;                        // ventana de CPed que lee la funcion (hasta m_nAnimGroup +0x4D4 inclusive)
constexpr u32 kPedOff = 0x40, kPedArena = 0x558;        // 0x40 + 0x4D8 + 0x40
constexpr u32 kPdBytes = 0x20;                          // ventana de CPlayerPedData (m_vecFightMovement en +0xC)
constexpr u32 kPdOff = 0x20, kPdArena = 0x80;           // 0x20 + 0x20 + 0x40
constexpr u32 kEvBytes = 0x10;                          // CEvent falso: vtable en +0
constexpr u32 kEvOff = 0x20, kEvArena = 0x80;           // 0x20 + 0x10 + 0x50
constexpr u32 kSlots = 16;                              // vtable falsa del evento (solo la ranura 2 es la correcta)
constexpr u32 kGetEventPrioritySlot = 2;                    // [vtable + 8]

// Desplazamientos dentro de los objetos
constexpr u32 kFinishedAt = 8, kAnimAt = 0x1C, kIdleAt = 0x20, kNextAt = 0x26;         // CTaskSimpleFight
constexpr u32 kAnimWeightAt = 0x18, kAnimDeltaAt = 0x1C;                              // CAnimBlendAssociation
constexpr u32 kPedClumpAt = 0x18, kPedPdAt = 0x480, kPedGroupAt = 0x4D4;              // CPed
constexpr u32 kPdFightAt = 0xC;                                                       // CPlayerPedData::m_vecFightMovement (x en +0xC, y en +0x10)

inline constexpr u32 kFloatEdges[]{0x00000000u, 0x80000000u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F800000u, 0xBF800000u, 0x3A83126Fu, 0xBA83126Fu,
                                   0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FFFFFFFu, 0x42C80000u, 0xC0800000u, 0xC47A0000u};
inline constexpr u32 kWordEdges[]{0u, 1u, 2u, 3u, 4u, 0x10u, 0x14u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu};
inline constexpr u32 kTypeEdges[]{0u, 1u, 0x1Eu, 0x1Fu, 0x20u, 0x21u, 0x3Bu, 0x3Cu, 0x3Du, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFFE0u, 0x100u, 0x80000020u};

struct C {
    u32 family, prio, hasEvent, evType, hasAnim, hasIdle, pedKind, animDelta, idleWeight, idleDelta, pdX, pdY, finished0, next0, ret, eax, edx, stack;
};

inline u32 Edge(u32 n, u32 k, const u32* tab, std::size_t count) { return tab[Rand(n, k) % count]; }
inline u32 AnyFloat(u32 n, u32 k) {
    const u32 r = Rand(n, k);
    return r % 3 == 0 ? Edge(n, k + 700, kFloatEdges, std::size(kFloatEdges)) : r % 3 == 1 ? Rand(n, k + 500) : (Rand(n, k + 500) & 0x807FFFFFu) | ((Rand(n, k + 600) % 20 + 118) << 23);   // borde / aleatorio / exponente moderado
}
inline u32 PosFloat(u32 n, u32 k) { return 0x3F800000u + (Rand(n, k) & 0x00FFFFFFu) % 0x1000000u; }       // positivo normal (>= 1.0)
inline u32 NegFloat(u32 n, u32 k) { return 0xBF800000u + (Rand(n, k) & 0x007FFFFFu); }                    // negativo normal (<= -1.0)
inline u32 AnyWord(u32 n, u32 k) {
    const u32 r = Rand(n, k);
    return r % 3 == 0 ? Edge(n, k + 400, kWordEdges, std::size(kWordEdges)) : Rand(n, k + 500);
}
inline float AsFloat(u32 bits) { float x; std::memcpy(&x, &bits, 4); return x; }

// Familias (n % 16, 288 casos cada una):
//  0 prioridad ocioso (0, 3 y otros) -> next = 1, devuelve 0   1 evento con prioridad <= 0x1F (con signo) -> 0      2 evento con prioridad 0x3C -> 0
//  3 urgente sin evento, ambas animaciones       4 inmediata sin evento, ambas animaciones    5 urgente con evento aceptable, jugador
//  6 inmediata con evento aceptable, jugador     7 solo m_pAnim                                8 solo m_pIdleAnim con peso > 0 y delta >= 0 (se mezcla)
//  9 idle que NO se mezcla (peso <= 0 / NaN / delta < 0)   10 ped nulo (solo sin mezcla)      11 jugador sin animaciones     12 no jugador con todo
// 13 prioridades de borde (3, 0x80000001, 0xFFFFFFFF)     14..15 combinaciones aleatorias
inline C MakeC(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.family = f;
    c.prio = Rand(n, 3) % 2 ? 1u : 2u;
    c.hasEvent = Rand(n, 4) % 3 == 0;
    c.evType = Rand(n, 5) % 3 == 0 ? Edge(n, 6, kTypeEdges, std::size(kTypeEdges)) : Rand(n, 7) % 0x80u;
    c.hasAnim = Rand(n, 8) % 2;
    c.hasIdle = Rand(n, 9) % 2;
    c.pedKind = 1 + Rand(n, 10) % 2;                  // 1 = no jugador, 2 = jugador (0 = ped nulo, solo familia 10)
    c.animDelta = AnyFloat(n, 11);
    const u32 wk = Rand(n, 12) % 4, dk = Rand(n, 13) % 4;
    c.idleWeight = wk == 0 ? PosFloat(n, 14) : wk == 1 ? AnyFloat(n, 15) : wk == 2 ? NegFloat(n, 16) : Edge(n, 17, kFloatEdges, std::size(kFloatEdges));
    c.idleDelta = dk == 0 ? PosFloat(n, 18) : dk == 1 ? AnyFloat(n, 19) : dk == 2 ? 0u : Edge(n, 20, kFloatEdges, std::size(kFloatEdges));
    c.pdX = Rand(n, 21);
    c.pdY = Rand(n, 22);
    c.finished0 = Rand(n, 23) % 4 == 0 ? 1u : Rand(n, 24) & 0xFFu;
    c.next0 = Rand(n, 25) % 4 == 0 ? Edge(n, 26, kWordEdges, std::size(kWordEdges)) & 0xFFu : Rand(n, 27) & 0xFFu;
    c.ret = Rand(n, 28);                              // EAX con que salen los falsos que devuelven valor (el booleano de IsPlayer se toma de pedKind)
    c.eax = Rand(n, 29);
    c.edx = Rand(n, 30);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    switch (f) {
    case 0: c.prio = Rand(n, 31) % 2 ? 0u : (Rand(n, 32) % 2 ? 3u : AnyWord(n, 33)); if (c.prio == 1 || c.prio == 2) c.prio = 0; break;
    case 1: c.prio = 1 + Rand(n, 3) % 2; c.hasEvent = 1; c.evType = Rand(n, 34) % 3 == 0 ? Edge(n, 35, kTypeEdges, std::size(kTypeEdges)) & 0x1Fu : (Rand(n, 36) % 2 ? 0x1Fu : 0x80000000u | (Rand(n, 37) & 0x7FFFFFFFu)); break;
    case 2: c.prio = 1 + Rand(n, 3) % 2; c.hasEvent = 1; c.evType = 0x3C; break;
    case 3: c.prio = 1; c.hasEvent = 0; c.hasAnim = 1; c.hasIdle = 1; break;
    case 4: c.prio = 2; c.hasEvent = 0; c.hasAnim = 1; c.hasIdle = 1; break;
    case 5: c.prio = 1; c.hasEvent = 1; c.evType = 0x20 + Rand(n, 38) % 0x1B; c.pedKind = 2; break;
    case 6: c.prio = 2; c.hasEvent = 1; c.evType = 0x3D + Rand(n, 39) % 0x40; c.pedKind = 2; break;
    case 7: c.hasAnim = 1; c.hasIdle = 0; break;
    case 8: c.hasIdle = 1; c.idleWeight = PosFloat(n, 14); c.idleDelta = Rand(n, 40) % 2 ? PosFloat(n, 18) : 0u; break;
    case 9: c.hasIdle = 1; c.idleWeight = Rand(n, 41) % 2 ? Edge(n, 42, kFloatEdges, std::size(kFloatEdges)) : NegFloat(n, 16);
            if (Rand(n, 43) % 2) { c.idleWeight = PosFloat(n, 14); c.idleDelta = NegFloat(n, 44); } break;
    case 10: c.pedKind = 0; c.hasIdle = Rand(n, 45) % 2; break;
    case 11: c.hasAnim = 0; c.hasIdle = 0; c.pedKind = 2; break;
    case 12: c.pedKind = 1; c.hasAnim = 1; c.hasIdle = 1; break;
    case 13: c.prio = Rand(n, 47) % 3 == 0 ? 3u : Rand(n, 47) % 3 == 1 ? 0x80000001u : 0xFFFFFFFFu; break;
    default: break;
    }
    if (c.pedKind == 0 && c.hasIdle && AsFloat(c.idleWeight) > 0.0f && AsFloat(c.idleDelta) >= 0.0f) c.idleWeight = 0u;   // ped nulo: la mezcla del reposo desreferenciaria el ped (el EXE se detiene)
    return c;
}
}  // namespace FightAbort31Fix

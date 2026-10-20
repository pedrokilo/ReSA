#pragma once
// Fixtures del banco PartnerNext60: CTaskComplexPartner::CreateNextSubTask (0x683AD0, thiscall con 1 argumento (ped), `ret 4`, devuelve CTask* en EAX; marco SEH propio).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y tambien un programa autonomo de calibracion (fixture_mirror.py).
//
// Resumen del desensamblado (0x683AD0..0x6840BE): ESI = this, EBP = ped.
//   partner = [this+0x38]; sin partner -> 0. t = Find(&partner->intel->taskMgr, [this+0x34]) (0x681740); sin t o t->m_partner != ped -> 0.
//   despues se despacha por el BYTE m_partnerState (+0x59): 6 (secuencia), 5 (acercarse / andar), 4, 3, 2, 1; cualquier otro valor -> 0.
// Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en el EXE y en la DLL): 0x681740 (Find), 0x53CDC0 (GetNodeHeadingFromVector), 0x61A5A0 (CTask::operator new),
// 0x681AF0 (CTaskManager::SetTask) y 0x681FE0 (sin hook, solo en el EXE; la DLL lo llama por su direccion). Los constructores (StandStill 0x62F310, Wander 0x48E4F0, GoToPointAndStandStill 0x668120,
// TurnToFace 0x66B890) son reales en ambos lados y construyen en arenas que devuelve el falso de operator new; la funcion GetPartnerSequence (vtable +0x34 de `this`), GetTaskType (+0x10) de la
// subtarea y de la tarea por defecto y GetWanderType (+0x2C) van por vtables falsas de arena.
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace PartnerNext60Fix {
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
constexpr u32 kThisBytes = 0x74;                                  // sizeof(CTaskComplexPartnerDeal): el word +0x70 pertenece a las clases derivadas
constexpr u32 kThisOff = 0x40, kThisArena = 0x100;                // 0x40 + 0x74 + 0x4C = 0x100
constexpr u32 kSubAtThis = 8, kTaskIdAt = 0x34, kPartnerAt = 0x38, kPointAt = 0x40, kTargetAt = 0x4C, kLeadAt = 0x58, kStateAt = 0x59, kFirstAt = 0x5A, kUpdAt = 0x5B, kFaceAt = 0x5D, kStreamedAt = 0x6E, kCounterAt = 0x70;
constexpr u32 kPedOff = 0x40, kPedBytes = 0x4E8, kPedArena = 0x568;   // ped y partner falsos: 0x40 + 0x4E8 + 0x40 = 0x568
constexpr u32 kPlacementAt = 4, kHeadingAt = 0x10, kMatrixAt = 0x14, kIntelAt = 0x47C, kShiftAt = 0x4D8;
constexpr u32 kMatOff = 0x20, kMatBytes = 0x48, kMatArena = 0x88;       // matrices falsas: 0x20 + 0x48 + 0x20 = 0x88
constexpr u32 kIntelOff = 0x20, kIntelBytes = 0x40, kIntelArena = 0x80;  // inteligencias falsas: 0x20 + 0x40 + 0x20 = 0x80
constexpr u32 kDefAtIntel = 0x14;                                       // [intel+0x14] = m_TaskMgr.m_aPrimaryTasks[TASK_PRIMARY_DEFAULT]
constexpr u32 kTaskOff = 0x20, kTaskBytes = 0x30, kTaskArena = 0x80;     // subtarea / tarea por defecto falsas: 0x20 + 0x30 + 0x30 = 0x80
constexpr u32 kOtherOff = 0x20, kOtherBytes = 0x60, kOtherArena = 0xA0;  // tarea equivalente del partner: 0x20 + 0x60 + 0x20 = 0xA0
constexpr u32 kOtherPartnerAt = 0x38, kOtherStateAt = 0x59, kOtherFirstAt = 0x5A, kOtherPointAt = 0x40;
constexpr u32 kAllocOff = 0x20, kAllocBytes = 0x40, kAllocArena = 0x80;  // objetos que "reserva" el falso de operator new: 0x20 + 0x40 + 0x20 = 0x80
constexpr u32 kSeqOff = 0x10, kSeqBytes = 0x20, kSeqArena = 0x40;        // secuencia falsa (solo su direccion): 0x10 + 0x20 + 0x10 = 0x40
constexpr u32 kTimeAddr = 0xB7CB84;
constexpr u32 kMaxEvents = 16;
constexpr u32 kVtSlots = 16;
constexpr u32 kSizeStand = 0x20, kSizeWander = 0x38, kSizeGoTo = 0x28, kSizeTurn = 0x28;

// ------------------------------------------------------------------------------------------------ caso
struct C {
    u32 family, state, taskId, partnerNull, found, otherIsPed, otherState, otherFirst, upd, streamed, face, first, lead;
    u32 subType, counter, point[3], target[3], calc[3];
    u32 defKind, defType, wanderType;
    u32 pedMat, ped[2], pedAlt[2], part[2], partAlt[2], heading, fwd[2], right[2], pedFwdOther[2];
    u32 headingRet, alloc0Null, alloc1Null, timeMs, seqRet, fill0, fill1;
    u32 eax, edx, stack;
};

inline constexpr u8 kByteEdges[]{2, 3, 0x7F, 0x80, 0x81, 0xFE, 0xFF, 0x10};
inline constexpr u32 kOneF = 0x3F800000u, kZeroF = 0u, kNegZeroF = 0x80000000u, kNanF = 0x7FC00000u, kInfF = 0x7F800000u, kNegInfF = 0xFF800000u;
inline constexpr u32 kSpecialF[]{kZeroF, kNegZeroF, kOneF, 0xBF800000u, 0x3F7D70A4u, 0x3F7D70A5u, 0x3F7D70A3u, 0x3F8147AEu, 0x3F8147AFu, 0x3F8147ADu, 0x3CA3D70Au, 0x3CA3D70Bu, 0x3CA3D709u, 0x40000000u, 0x358637BDu};
inline constexpr u32 kStatesAll[]{0, 1, 2, 3, 4, 5, 6, 7, 8, 0x80, 0x81, 0xFF, 0x7F, 0x10};
inline constexpr u32 kSubTypes[]{0x387u, 0x398u, 0xCBu, 0x386u, 0x388u, 0x397u, 0x399u, 0xCAu, 0xCCu, 0x80000387u, 0x10398u, 0x1CBu, 0u, 0xFFFFFFFFu};
inline constexpr u32 kHeadings[]{0x00000000u, 0x80000000u, 0x40490FDBu, 0xC0490FDBu, 0x3FC90FDBu, 0x40C90FDBu, 0x3F800000u, 0x41200000u, 0x42C80000u, 0x4B189680u, 0xC2480000u, 0x3A83126Fu, 0x5F000001u, 0x7F000000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u};

inline constexpr u32 kTimes[]{0u, 0xFFFFFFFFu, 0xFFFF0000u, 0xFFFE795Fu, 0xFFFE7960u, 100000u};
inline constexpr u32 kDeadStates[]{0u, 7u, 8u, 0x80u, 0xFFu, 0x7Fu, 0x10u, 0x86u, 0x105u};
inline constexpr u32 kRefDist[]{0x3F7D70A4u, 0x3F7D70A5u, 0x3F7D70A3u, 0x3F8147AEu, 0x3F8147AFu, 0x3F8147ADu, 0x3F800000u, 0x3F7FFFFFu, 0x3F800001u};
inline constexpr u32 kRefDelta[]{0xBCF5C28Fu, 0xBCA3D70Au, 0xBCA3D709u, 0xBCA3D70Bu, 0x00000000u, 0x3CA3D709u, 0x3CA3D70Au, 0x3CA3D70Bu, 0x3CF5C28Fu};   // -0.03 .. +0.03 alrededor de +-0.02
inline u32 Pick(u32 n, u32 k, const u32* a, std::size_t m) { return a[Rand(n, k) % m]; }
// Coordenada flotante "normal" (bits): signo y escala al azar, de 0.01 a 1e9 (los cuadrados no desbordan ni se vuelven denormales en float)
inline u32 NormalF(u32 n, u32 k) {
    static const float scales[]{0.01f, 0.5f, 1.0f, 3.0f, 10.0f, 100.0f, 1000.0f, 1.0e5f, 1.0e7f, 1.0e9f};
    const float s = scales[Rand(n, k) % std::size(scales)];
    const float u = float(Rand(n, k + 1) % 2000001u) / 1000000.0f - 1.0f;   // [-1, 1]
    const float v = u * s;
    u32 b;
    std::memcpy(&b, &v, 4);
    return b;
}
inline u32 FloatBits(float v) { u32 b; std::memcpy(&b, &v, 4); return b; }

// Familias (n % 16, 256 casos cada una):
//  0 sin partner                              1 Find devuelve nulo                          2 la tarea del partner apunta a otro peaton
//  3 estado 6 (secuencia): streamed / contador / retorno de GetPartnerSequence
//  4 estado 5, sin "mirar al partner" (+0x5D == 0), estado del partner < 5 (con signo): solo StandStill
//  5 estado 5, sin mirar al partner, estado del partner >= 5: tarea por defecto, forward (matriz o rumbo), Wander + SetTask, estado = 6
//  6 estado 5, mirar al partner, +0x5A == 1: estado del partner == 6 -> estado = 6
//  7 estado 5, mirar al partner, +0x5A != 1: geometria, distancia alrededor de 0.99 / 1.01 (llegada)
//  8 estado 5, mirar al partner, +0x5A != 1: geometria normal (varias escalas)
//  9 estado 5, mirar al partner, +0x5A != 1: geometria especial (coincidentes, 0, NaN, inf, cerca de 0.02)
//  10 estado 4                                 11 estado 3                                    12 estado 2                                  13 estado 1
//  14 estados sin rama (0, 7, 8, 0x80, 0xFF...)    15 mezcla al azar de todos los parametros
inline C MakeC(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.family = f;
    c.taskId = Rand(n, 3) % 4 == 0 ? 0x4B4u : Rand(n, 4);
    c.eax = Rand(n, 70);
    c.edx = Rand(n, 71);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    c.partnerNull = 0;
    c.found = 1;
    c.otherIsPed = 1;
    c.state = Pick(n, 5, kStatesAll, std::size(kStatesAll));
    c.otherState = Pick(n, 6, kStatesAll, std::size(kStatesAll));
    c.otherFirst = Rand(n, 7) % 3 == 0 ? 0xFFu : Rand(n, 8) % 3 == 0 ? 1u : (Rand(n, 9) & 0xFFu);
    c.upd = Rand(n, 10) % 3 == 0 ? Pick(n, 11, kStatesAll, std::size(kStatesAll)) : (Rand(n, 12) & 0xFFu);
    c.streamed = Rand(n, 13) % 3 == 0 ? 1u : Rand(n, 14) % 3 == 0 ? 0u : Rand(n, 15) % 2 ? u32(kByteEdges[Rand(n, 16) % std::size(kByteEdges)]) : (1u + Rand(n, 17) % 255u);
    c.face = Rand(n, 18) % 3 == 0 ? 0u : Rand(n, 19) % 2 ? 1u : u32(kByteEdges[Rand(n, 20) % std::size(kByteEdges)]);
    c.first = Rand(n, 21) % 3 == 0 ? 0xFFu : Rand(n, 22) % 3 == 0 ? 1u : Rand(n, 23) % 2 ? 0u : (Rand(n, 24) & 0xFFu);
    c.lead = Rand(n, 25) % 3 == 0 ? 0u : Rand(n, 26) % 2 ? 1u : u32(kByteEdges[Rand(n, 27) % std::size(kByteEdges)]);
    c.subType = Pick(n, 28, kSubTypes, std::size(kSubTypes));
    c.counter = Rand(n, 29) & 0xFFFFu;
    for (u32 k = 0; k < 3; ++k) {
        c.point[k] = Rand(n, 30) % 4 == 0 ? Pick(n, 31 + k, kSpecialF, std::size(kSpecialF)) : NormalF(n, 40 + 2 * k);
        c.target[k] = NormalF(n, 50 + 2 * k);
        c.calc[k] = NormalF(n, 60 + 2 * k);
    }
    if (Rand(n, 34) % 3 == 0) { c.point[0] = kZeroF; c.point[1] = Rand(n, 35) % 2 ? kZeroF : kNegZeroF; }
    c.defKind = Rand(n, 80) % 4;   // 0 nula, 1 tipo distinto de 0x390, 2 Wander no estandar, 3 Wander estandar
    c.defType = c.defKind == 0 ? 0u : c.defKind == 1 ? (Rand(n, 81) % 2 ? 0x391u : Rand(n, 82)) : 0x390u;
    c.wanderType = c.defKind == 2 ? (Rand(n, 83) % 2 ? 1u : Rand(n, 84) | 1u) : 0u;
    c.pedMat = Rand(n, 85) % 2;
    for (u32 k = 0; k < 2; ++k) {
        c.ped[k] = NormalF(n, 90 + 2 * k);
        c.pedAlt[k] = NormalF(n, 94 + 2 * k);
        c.part[k] = NormalF(n, 98 + 2 * k);
        c.partAlt[k] = NormalF(n, 102 + 2 * k);
        c.fwd[k] = NormalF(n, 106 + 2 * k);
        c.right[k] = NormalF(n, 110 + 2 * k);
        c.pedFwdOther[k] = NormalF(n, 114 + 2 * k);
    }
    c.heading = Rand(n, 86) % 4 ? Pick(n, 87, kHeadings, std::size(kHeadings)) : NormalF(n, 88);
    if ((c.heading & 0x7FFFFFFFu) > 0x4B189680u && (c.heading & 0x7FFFFFFFu) < 0x5F000000u) c.heading = 0x41200000u;   // entre 1e7 y 2^63: el oraculo no modela la reduccion de rango del x87
    c.headingRet = Rand(n, 120);
    c.alloc0Null = Rand(n, 121) % 6 == 0;
    c.alloc1Null = Rand(n, 122) % 6 == 0;
    c.timeMs = Rand(n, 123) % 5 == 0 ? Pick(n, 124, kTimes, std::size(kTimes)) : Rand(n, 125);
    c.seqRet = Rand(n, 126) % 3 == 0 ? 0u : Rand(n, 127) % 2 ? 1u : Rand(n, 128);   // 0 nulo, 1 = la secuencia de arena, otro = valor opaco
    c.fill0 = 1u + Rand(n, 129) % 255u;
    c.fill1 = 1u + Rand(n, 130) % 255u;
    // escenario por familia
    switch (f) {
    case 0: c.partnerNull = 1; break;
    case 1: c.found = 0; break;
    case 2: c.otherIsPed = 0; break;
    case 3: c.state = 6; break;
    case 4: c.state = 5; c.face = 0; c.otherState = Rand(n, 131) % 2 ? (Rand(n, 132) % 5) : (0x80u + Rand(n, 133) % 128u); break;   // 0..4 o negativo con signo
    case 5: c.state = 5; c.face = 0; c.otherState = Rand(n, 131) % 3 ? (5u + Rand(n, 132) % 3) : 5u; c.defKind = Rand(n, 134) % 8 == 0 ? Rand(n, 135) % 3 : 3; c.defType = c.defKind == 0 ? 0u : c.defKind == 1 ? 0x391u : 0x390u; c.wanderType = c.defKind == 2 ? 1u : 0u; break;
    case 6: c.state = 5; c.face = c.face ? c.face : 1u; c.first = 1; c.otherState = Rand(n, 131) % 2 ? 6u : Pick(n, 132, kStatesAll, std::size(kStatesAll)); break;
    case 7: case 8: case 9: c.state = 5; c.face = c.face ? c.face : 1u; if (c.first == 1) c.first = Rand(n, 136) % 2 ? 0xFFu : 0u; c.pedMat = 1; break;
    case 10: c.state = 4; if (Rand(n, 151) % 2) c.otherState = 4u + Rand(n, 152) % 2u; break;
    case 11: c.state = 3; { const u32 r = Rand(n, 153) % 10; if (r < 4) c.subType = 0x387u; else if (r < 8) c.subType = 0x398u; } break;
    case 12: c.state = 2; if (Rand(n, 154) % 8) c.subType = 0xCBu; break;
    case 13: c.state = 1; if (Rand(n, 155) % 8) c.subType = 0xCBu; break;
    case 14: c.state = Pick(n, 5, kDeadStates, std::size(kDeadStates)); break;
    default: c.state = Rand(n, 137) % 8; c.partnerNull = Rand(n, 138) % 10 == 0; c.found = Rand(n, 139) % 8 != 0; c.otherIsPed = Rand(n, 140) % 8 != 0; break;
    }
    if ((f >= 7 && f <= 9) || (f == 15 && c.state == 5 && c.face != 0 && c.first != 1)) {
        c.pedMat = 1;
        if (f == 7) {   // distancia al partner alrededor de 0.99 / 1.01: partner en (0, 0) o desplazado, ped a esa distancia en x o en y
            const u32 r = Pick(n, 141, kRefDist, std::size(kRefDist));
            const float rf = [&] { float v; std::memcpy(&v, &r, 4); return v; }();
            const float qx = Rand(n, 143) % 2 ? 0.0f : float(Rand(n, 144) % 64u), qy = Rand(n, 145) % 2 ? 0.0f : float(Rand(n, 146) % 64u);
            const bool alongY = Rand(n, 142) % 3 == 1;
            c.part[0] = FloatBits(qx); c.part[1] = FloatBits(qy);
            c.ped[0] = FloatBits(alongY ? qx : qx + rf); c.ped[1] = FloatBits(alongY ? qy + rf : qy);
        } else if (f == 9) {
            const u32 kind = Rand(n, 147) % 8;
            switch (kind) {
            case 0: c.ped[0] = c.part[0]; c.ped[1] = c.part[1]; break;                                  // coincidentes: suma 0, x = 1
            case 1: c.part[0] = kZeroF; c.part[1] = kZeroF; c.ped[0] = kZeroF; c.ped[1] = kNegZeroF; break;
            case 2: c.ped[0] = kNanF; break;                                                              // NaN (un solo tipo de NaN)
            case 3: c.ped[1] = kInfF; break;                                                              // inf - finito = inf; inf * 0 = NaN
            case 4: c.ped[0] = kNegInfF; c.ped[1] = kInfF; break;
            case 5: case 6: {   // partner en (0, 0) y ped a (1 + d, 0): la correccion |d| queda a ambos lados de 0.02
                const u32 d = Pick(n, 148, kRefDelta, std::size(kRefDelta));
                float df; std::memcpy(&df, &d, 4);
                c.part[0] = kZeroF; c.part[1] = kZeroF; c.ped[0] = FloatBits(1.0f + df); c.ped[1] = kZeroF; break;
            }
            default: c.ped[0] = Rand(n, 149) % 2 ? kZeroF : kNegZeroF; c.ped[1] = kZeroF; c.part[0] = FloatBits(1.0e9f); c.part[1] = FloatBits(-1.0e9f); break;
            }
        }
        if (f == 9 && Rand(n, 150) % 6 == 0) c.right[0] = kNanF;   // matriz del ped con NaN en right.x
    }
    // invariante de las fixtures: la rama geometrica (estado de byte 5, cara != 0, primer != 1) lee la matriz del ped sin comprobar nulo (EXE 0x683DC7: mov edx,[ecx] con ecx = ped->m_matrix); el estado 0x105 de la familia 14 tiene el byte bajo 5
    if ((c.state & 0xFFu) == 5 && (c.face & 0xFFu) != 0 && (c.first & 0xFFu) != 1) c.pedMat = 1;
    // invariante de las fixtures: si se va a construir el Wander, la primera reserva no es nula (el EXE escribiria en nulo+0x34)
    if (c.state == 5 && !c.face && std::int8_t(c.otherState) >= 5 && c.defKind == 3 && c.found && c.otherIsPed && !c.partnerNull) c.alloc0Null = 0;
    return c;
}
}  // namespace PartnerNext60Fix

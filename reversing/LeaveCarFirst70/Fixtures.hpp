#pragma once
// Fixtures del banco LeaveCarFirst70 (lote 70): CTaskComplexLeaveCar::CreateFirstSubTask (0x641FC0, virtual, vtable 0x86E828 ranura 9, thiscall con 1 argumento (ped), `ret 4`,
// devuelve CTask* en EAX; marco SEH propio), 16384 casos = 16 familias x 1024. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp / fixture_dump.cpp.
//
// Resumen del desensamblado (0x641FC0..0x6421AC), ESI = ped, EBX = this, [this+0xC] = vehiculo objetivo, [this+0x14] = retardo:
//   A = ([ped+0x530] == 0x3F (arrestado)) || ([ped+0x474] & 0x40 (bIsBeingArrested)); si A y CPed::IsPlayer (0x5DF8F0, solo AL): CTask::operator new(0x18) (0x61A5A0) y, si no es nulo, CTaskSimplePause(-1) (0x48E750).
//   Si no (A falso, o A y no es jugador): si ([ped+0x46C] & 0x100) == 0 (bInVehicle): printf("CTaskComplexLeaveCar - ped not in car") (0x821982 del EXE), operator new(0x18) y pausa(-1).
//   En coche: si [vehiculo+0x460] (conductor) == ped: grupo = CPedGroups::GetPedsGroup(ped) (0x5F7E80); si hay grupo y grupo->membresia(+8).IsLeader(ped) (0x5F69C0, solo AL): CEvent::operator new(0x14) (0x4B5620),
//   si no es nulo CEventLeaderExitedCarAsDriver (0x4B8300), CEventGroupEvent local (ped, evento) (0x4ADFD0: RegisterReference 0x571B70 del ped), grupo->inteligencia(+0x30).AddEvent (0x5F7470) y destructor (0x4AE070:
//   CleanUpOldReference 0x571A00 del ped y borrado virtual del evento -> CEvent::operator delete 0x4B5630). Despues, IsPlayer: si no es jugador ped->SetRadioStation (0x5DFD90); si lo es y [ped+0x58C] (m_pVehicle) no es nulo,
//   PlayerAboutToExitVehicleAsDriver (0x4F5BA0) sobre m_pVehicle+0x138.
//   Final: [vehiculo+0x590] == 5 (barco): operator new(0x14) y CTaskComplexLeaveBoat(vehiculo, 0) (0x63BB60); si no operator new(0x70) y CTaskSimpleCarDriveTimed(vehiculo, retardo) (0x5FF940). Nulo del operator new -> 0.
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <string>

namespace Leave70Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 70001u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

// ------------------------------------------------------------------------------------------------ disposicion de memoria (guarda | objeto | guarda)
constexpr u32 kGuard = 0x40;
constexpr u32 kTaskBytes = 0x34, kTaskArena = kGuard + kTaskBytes + kGuard;      // 0xB4   CTaskComplexLeaveCar falso (lee +0xC y +0x14)
constexpr u32 kPedBytes = 0x5A0, kPedArena = kGuard + kPedBytes + kGuard;        // 0x620  ped falso (lee +0x46C, +0x474, +0x530, +0x58C)
constexpr u32 kVehBytes = 0x5A0, kVehArena = kGuard + kVehBytes + kGuard;        // 0x620  vehiculo falso (lee +0x460 y +0x590; +0x138 solo como direccion)
constexpr u32 kGrpBytes = 0x40, kGrpArena = kGuard + kGrpBytes + kGuard;         // 0xC0   grupo falso (+8 membresia, +0x30 inteligencia: solo direcciones)
constexpr u32 kEvBytes = 0x20, kEvArena = kGuard + kEvBytes + kGuard;            // 0xA0   evento que "reserva" el falso de CEvent::operator new (0x14 bytes)
constexpr u32 kAllocBytes = 0x70, kAllocArena = kGuard + kAllocBytes + kGuard;   // 0xF0   tarea que "reserva" el falso de CTask::operator new (hasta 0x70 bytes)
static_assert(kTaskArena == 0xB4 && kPedArena == 0x620 && kVehArena == 0x620 && kGrpArena == 0xC0 && kEvArena == 0xA0 && kAllocArena == 0xF0, "arenas");
// campos
constexpr u32 kTaskVehAt = 0xC, kTaskDelayAt = 0x14;
constexpr u32 kPedFlagsAt = 0x46C, kPedFlags2At = 0x474, kPedStateAt = 0x530, kPedVehAt = 0x58C;
constexpr u32 kVehDriverAt = 0x460, kVehTypeAt = 0x590, kVehAudioAt = 0x138;
constexpr u32 kGrpMemberAt = 8, kGrpIntelAt = 0x30;
constexpr u32 kMaxEvents = 24;
constexpr u32 kPlSeq = 4;   // respuestas independientes de IsPlayer por llamada
constexpr u32 kStateArrested = 0x3F, kTypeBoat = 5;

struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& A(const char* k, const u32* v, unsigned n) { Key(k); s += '['; for (unsigned i = 0; i < n; ++i) { if (i) s += ','; s += std::to_string(v[i]); } s += ']'; return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ caso
// driverMode: 0 = el ped, 1 = nulo, 2 = el otro vehiculo, 3 = ped + 4, 4 = el grupo.   pedVehMode (m_pVehicle del ped): 0 nulo, 1 vehiculo objetivo, 2 otro vehiculo, 3 el propio ped.
struct C {
    u32 family, state, flagsA, flagsB, pedVehMode, driverMode, vtype, hasGroup, delay, evNull, taskNull, leaderRet;
    u32 pl[kPlSeq];
    u32 fill, evFill, eax, edx, stack;
};

inline constexpr u32 kStates[]{0u, 1u, 2u, 0x3Eu, 0x40u, 0x13Fu, 0x8000003Fu, 0xFFFFFFFFu, 0x3F000000u, 0x1000003Fu, 0x37u, 0x34u, 0x35u, 0x3Du, 0x3Cu, 0x7Fu, 0xBFu, 0x80000000u};
inline constexpr u32 kTypes[]{0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 0x105u, 0x80000005u, 0xFFFFFFFFu, 0x5000000u, 0x500u, 0x4u, 0x6u, 0xFFFFFFFBu};
inline constexpr u32 kDelays[]{0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, 1000u, 0xFFFFu, 0x10000u, 0x12345678u};
template <class A> inline u32 Pick(const A& a, u32 n, u32 k) { return a[Rand(n, k) % std::size(a)]; }

// Palabra de retorno de un colaborador que devuelve bool: solo cuenta AL (ceros en AL con basura en el resto; AL no nulo con basura)
inline u32 BoolWord(u32 n, u32 k, bool yes) {
    const u32 hiMode = Rand(n, k) % 3;
    const u32 hi = hiMode == 0 ? 0u : (Rand(n, k + 1) & 0xFFFFFF00u);
    const u32 lo = yes ? 1u + Rand(n, k + 2) % 255u : 0u;
    return hi | lo;
}

// Familias (n % 16, 1024 casos cada una):
//  0 arrestado por estado + jugador -> pausa                   1 arrestado por bandera (estado distinto) + jugador -> pausa
//  2 arrestado pero no jugador (sigue)                          3 no arrestado y fuera del coche (printf + pausa)
//  4 en coche, el conductor no es el ped                        5 conductor = ped, sin grupo
//  6 conductor = ped, con grupo, no es lider                    7 lider, evento reservado, jugador (audio con m_pVehicle variable)
//  8 lider, operator new del evento nulo                        9 lider, no es jugador (SetRadioStation)
// 10 lider, barco y operator new de tarea nulo                 11 lider, no barco y operator new de tarea nulo
// 12 tipos de vehiculo alrededor de 5 (32 bits completos)      13 retardos extremos (no barco)
// 14 estados alrededor de 0x3F y banderas (A con basura)       15 mezcla al azar
inline C MakeC(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.family = f;
    c.eax = Rand(n, 70);
    c.edx = Rand(n, 71);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    c.fill = 1u + Rand(n, 97) % 255u;
    c.evFill = 1u + Rand(n, 98) % 255u;
    // ped
    c.state = Rand(n, 1) % 3 == 0 ? kStateArrested : (Rand(n, 2) % 2 ? Pick(kStates, n, 3) : Rand(n, 4));
    c.flagsA = Rand(n, 5);
    c.flagsB = Rand(n, 6);
    const bool inVeh = Rand(n, 7) % 4 != 0;
    c.flagsA = (c.flagsA & ~0x100u) | (inVeh ? 0x100u : 0u);
    const bool arrBit = Rand(n, 8) % 3 == 0;
    c.flagsB = (c.flagsB & ~0x40u) | (arrBit ? 0x40u : 0u);
    c.pedVehMode = Rand(n, 9) % 4;
    c.driverMode = Rand(n, 10) % 3 == 0 ? Rand(n, 11) % 5 : 0u;
    c.vtype = Rand(n, 12) % 3 == 0 ? kTypeBoat : Pick(kTypes, n, 13);
    c.hasGroup = Rand(n, 14) % 4 != 0;
    c.delay = Rand(n, 15) % 3 == 0 ? Pick(kDelays, n, 16) : Rand(n, 17);
    c.evNull = Rand(n, 18) % 10 == 0;
    c.taskNull = Rand(n, 19) % 10 == 0;
    c.leaderRet = BoolWord(n, 20, Rand(n, 23) % 2);
    for (u32 k = 0; k < kPlSeq; ++k) c.pl[k] = BoolWord(n, 30 + 4 * k, Rand(n, 26 + k) % 2);

    const auto isPlayer = [&](u32 k, bool yes) { c.pl[k] = BoolWord(n, 40 + 4 * k, yes); };
    const auto makeA = [&](bool viaState) {   // A verdadero por estado (0x3F) o solo por la bandera
        if (viaState) { c.state = kStateArrested; c.flagsB = (c.flagsB & ~0x40u) | (Rand(n, 50) % 2 ? 0x40u : 0u); }
        else { c.state = (Rand(n, 51) % 2 ? Pick(kStates, n, 52) : Rand(n, 53)); if (c.state == kStateArrested) c.state = 0x3Eu; c.flagsB |= 0x40u; }
    };
    const auto notA = [&] {
        if (c.state == kStateArrested) c.state = Rand(n, 54) % 2 ? 0x3Eu : 0x40u;
        c.flagsB &= ~0x40u;
    };
    const auto inCar = [&](bool yes) { c.flagsA = (c.flagsA & ~0x100u) | (yes ? 0x100u : 0u); };
    const auto driver = [&](bool ped) { c.driverMode = ped ? 0u : 1u + Rand(n, 55) % 4; };
    const auto notBoat = [&] { if (c.vtype == kTypeBoat) c.vtype = Pick(kTypes, n, 56); if (c.vtype == kTypeBoat) c.vtype = 4u; };
    const auto leaderPath = [&] {   // A falso o no jugador, en coche, conductor ped, grupo y lider
        notA(); inCar(true); driver(true); c.hasGroup = 1; c.leaderRet = BoolWord(n, 60, true);
    };
    switch (f) {
    case 0: makeA(true); isPlayer(0, true); break;
    case 1: makeA(false); isPlayer(0, true); break;
    case 2: if (Rand(n, 57) % 2) makeA(true); else makeA(false); isPlayer(0, false); break;
    case 3: notA(); inCar(false); break;
    case 4: notA(); inCar(true); driver(false); break;
    case 5: notA(); inCar(true); driver(true); c.hasGroup = 0; break;
    case 6: notA(); inCar(true); driver(true); c.hasGroup = 1; c.leaderRet = BoolWord(n, 61, false); break;
    case 7: leaderPath(); c.evNull = 0; isPlayer(0, true); isPlayer(1, true); isPlayer(2, true); break;
    case 8: leaderPath(); c.evNull = 1; break;
    case 9: leaderPath(); c.evNull = 0; isPlayer(0, false); isPlayer(1, false); isPlayer(2, false); break;
    case 10: leaderPath(); c.vtype = kTypeBoat; c.taskNull = 1; break;
    case 11: leaderPath(); notBoat(); c.taskNull = 1; break;
    case 12: notA(); inCar(true); if (Rand(n, 58) % 3) { driver(Rand(n, 59) % 2); } c.vtype = Pick(kTypes, n, 62); break;
    case 13: notA(); inCar(true); notBoat(); c.delay = Pick(kDelays, n, 63); c.taskNull = 0; break;
    case 14: if (Rand(n, 64) % 2) { c.state = kStateArrested ^ (1u << (Rand(n, 65) % 32)); if (Rand(n, 66) % 4 == 0) c.state = kStateArrested; } c.flagsB = Rand(n, 67); break;
    default: break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("family", c.family).U("state", c.state).U("flags_a", c.flagsA).U("flags_b", c.flagsB).U("ped_veh_mode", c.pedVehMode).U("driver_mode", c.driverMode).U("vtype", c.vtype)
     .U("has_group", c.hasGroup).U("delay", c.delay).U("ev_null", c.evNull).U("task_null", c.taskNull).U("leader_ret", c.leaderRet).A("pl", c.pl, kPlSeq)
     .U("fill", c.fill).U("ev_fill", c.evFill).U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}
}  // namespace Leave70Fix

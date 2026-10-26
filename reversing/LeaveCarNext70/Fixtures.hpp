#pragma once
// Fixtures del banco LeaveCarNext70 (lote 70): CTaskComplexLeaveCar::CreateNextSubTask (virtual, vtable 0x86E828 slot 8, EXE 0x6419F0), 16384 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskComplexLeaveCar::CreateNextSubTask  0x6419F0  thiscall, ECX = this (tarea), (CPed*), `ret 4`, EAX = tarea nueva o nulo   (hook CTaskComplexLeaveCar / CreateNextSubTask, tipo V)
//
// El tipo de la subtarea actual decide; los colaboradores se sustituyen por falsos que registran sus argumentos y devuelven valores del caso (por orden de llamada).
// Reparto: familia = n % 16 (1024 casos por familia).
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

namespace Lc70Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kTaskWin = 0x34, kSubWin = 0x10, kPedWin = 0x5A0, kVehWin = 0x680, kHandWin = 0xE0, kLineWin = 0x1C, kOthWin = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kTaskArena = Arena(kTaskWin), kSubArena = Arena(kSubWin), kPedArena = Arena(kPedWin), kVehArena = Arena(kVehWin), kHandArena = Arena(kHandWin),
                      kLineOldArena = Arena(kLineWin), kLineNewArena = Arena(kLineWin), kOthArena = Arena(kOthWin);

// Tipos de subtarea que distingue el EXE
constexpr u32 kPause = 0xCA, kGetUp = 0xCE, kDie = 0xD4, kLeaveBoat = 0x2CC, kDriveTimed = 0x2CF, kCloseDoor = 0x326, kWaitSlow = 0x329, kWaitDoor = 0x32A, kGetOut = 0x32D,
              kJumpOut = 0x32E, kForceOut = 0x32F, kSetOut = 0x330, kBeDragged = 0x337;
inline constexpr std::array<u32, 11> kTypesNull{0u, 1u, 0xC9u, 0xCBu, 0xCDu, 0xCFu, 0xD3u, 0xD5u, 0x2CBu, 0x2CDu, 0x2CEu};
inline constexpr std::array<u32, 19> kTypesNull2{0x2D0u, 0x325u, 0x327u, 0x328u, 0x32Bu, 0x32Cu, 0x331u, 0x332u, 0x333u, 0x334u, 0x335u, 0x336u, 0x338u, 0x339u, 0x516u, 0x1000u, 0x80000000u, 0xFFFFFFFFu, 0x7FFFFFFFu};
inline constexpr std::array<u32, 13> kTypesMain{kPause, kGetUp, kDie, kLeaveBoat, kDriveTimed, kCloseDoor, kWaitSlow, kWaitDoor, kGetOut, kJumpOut, kForceOut, kSetOut, kBeDragged};

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 70070u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
inline bool Chance(u32 n, u32 k, u32 num, u32 den) { return Rand(n, k) % den < num; }   // num/den

// ------------------------------------------------------------------------------------------------ JSON minimo (mismas filas `input` en el juego y en cualquier espejo)
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 subType{};
    // tarea
    u32 door{}, delay{}, sensible{}, force{}, die{}, inAir{}, vehNull{};
    u32 lineKind{};      // 0 = utilidad existente (arena), 1 = nulo, 2 = puntero crudo
    u32 lineRaw{};
    u32 flagsSet{}, numIn{}, dieAnim{}, dieBlend{}, dieSpeed{};
    // peaton
    u32 inVeh{}, drowning{}, pedVehKind{};   // pedVehKind: 0 = vehiculo de la tarea, 1 = otro objeto, 2 = nulo
    // vehiculo
    u32 vType{}, vSub{}, lock{}, drvKind{};  // drvKind: 0 = nulo, 1 = el peaton, 2 = otro objeto
    u32 pass[3]{};                           // 0 = nulo, 1 = no nulo
    u32 inFlags{}, outFlags{}, numGetting{}, status{}, modelFlags{}, nodes{};   // nodes: mascara de los 12 nodos no nulos
    // respuestas de los falsos
    u32 canStep{}, passengerRet{}, driverRet{}, playerBits{}, roomBits{}, hasDoorBits{}, statusSeq[3]{};
    u32 doorFlagRet{}, targetDoorRet{}, createNull{}, allocNull{};
};

inline constexpr std::array<u32, 14> kDoorsAny{0u, 1u, 2u, 7u, 8u, 9u, 10u, 11u, 12u, 0xFFFFFFFFu, 0x80000008u, 0x100u, 3u, 5u};
inline u32 DoorMain(u32 n, u32 k) {   // puertas 8..11 con peso, a veces otras
    const u32 r = Rand(n, k) % 10;
    return r < 8 ? 8u + Rand(n, k + 1) % 4 : Pick(kDoorsAny, n, k + 2);
}
inline u32 TypeMain(u32 n, u32 k) { return Pick(kTypesMain, n, k); }
inline u32 VehTypeAny(u32 n, u32 k) {
    static constexpr u32 v[]{0u, 9u, 1u, 2u, 3u, 5u, 10u, 11u, 0xFFFFFFFFu, 0x80000000u, 4u};
    return Rand(n, k) % 3 == 0 ? Rand(n, k + 1) : v[Rand(n, k + 2) % std::size(v)];
}

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n % kFamilies;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.subType = TypeMain(n, 10);
    // tarea
    c.door = Rand(n, 11) % 3 == 0 ? 0u : DoorMain(n, 12);
    c.delay = Rand(n, 13);
    c.sensible = Rand(n, 14) % 2; c.force = Rand(n, 15) % 2; c.die = Rand(n, 16) % 2; c.inAir = Rand(n, 17) % 2;
    c.vehNull = 0;
    c.lineKind = 0; c.lineRaw = Rand(n, 18) | 1u;
    c.flagsSet = Rand(n, 19) % 4 == 0 ? Rand(n, 20) % 2 * 0xFFu : Rand(n, 21) & 0xFFu;
    c.numIn = Rand(n, 22) % 4 == 0 ? Rand(n, 23) % 2 * 0xFFu : Rand(n, 24) & 0xFFu;
    c.dieAnim = Rand(n, 25); c.dieBlend = Rand(n, 26); c.dieSpeed = Rand(n, 27);
    // peaton
    c.inVeh = Rand(n, 30) % 4 != 0; c.drowning = Rand(n, 31) % 2; c.pedVehKind = Rand(n, 32) % 3;
    // vehiculo
    c.vType = Rand(n, 40) % 2 ? 0u : VehTypeAny(n, 41);
    c.vSub = Rand(n, 42) % 3 == 0 ? 2u : Rand(n, 43) % 2 ? 0u : VehTypeAny(n, 44);
    c.lock = Rand(n, 45) % 3 == 0 ? 1u : Rand(n, 46) % 3 == 0 ? 5u : Rand(n, 47) % 10;
    c.drvKind = Rand(n, 48) % 3;
    for (int i = 0; i < 3; ++i) c.pass[i] = Rand(n, 50 + i) % 2;
    static constexpr u32 masks[]{0u, 1u, 2u, 4u, 8u, 3u, 5u, 12u, 15u, 0xFFu, 0x10u, 0x80u, 0xF0u};
    c.inFlags = Rand(n, 53) % 4 == 0 ? Rand(n, 54) & 0xFFu : masks[Rand(n, 55) % std::size(masks)];
    c.outFlags = Rand(n, 56) % 4 == 0 ? Rand(n, 57) & 0xFFu : masks[Rand(n, 58) % std::size(masks)];
    c.numGetting = Rand(n, 59) & 0xFFu; c.status = Rand(n, 60) & 0xFFu;
    c.modelFlags = Rand(n, 61); c.nodes = Rand(n, 62) & 0xFFFu;
    // respuestas de los falsos
    c.canStep = Rand(n, 70) % 2; c.passengerRet = Rand(n, 71) % 2; c.driverRet = Rand(n, 72) % 2; c.playerBits = Rand(n, 73); c.roomBits = Rand(n, 74); c.hasDoorBits = Rand(n, 75);
    for (int i = 0; i < 3; ++i) { const u32 r = Rand(n, 76 + i) % 6; c.statusSeq[i] = r < 2 ? 0u : r < 4 ? 2u : r == 4 ? 1u : Rand(n, 80 + i) % 5; }
    c.doorFlagRet = Rand(n, 85) % 4 == 0 ? Pick(kStackSeeds, n, 86) : Rand(n, 87);
    c.targetDoorRet = Rand(n, 88) % 2 ? 8u + Rand(n, 89) % 4 : Pick(kDoorsAny, n, 90);
    c.createNull = Rand(n, 91) % 6 == 0; c.allocNull = 0;

    switch (c.family) {
    case 0:   // tipos ajenos: el EXE devuelve nulo sin crear nada
        c.subType = Rand(n, 100) % 4 == 0 ? Rand(n, 101) : Rand(n, 102) % 2 ? Pick(kTypesNull, n, 103) : Pick(kTypesNull2, n, 104);
        if (Rand(n, 105) % 8 == 0) c.subType = u32(kTypesMain[Rand(n, 106) % kTypesMain.size()] + (Rand(n, 107) % 2 ? 0x10000u : 0xFFFF0000u));   // desbordes alrededor de los tipos conocidos
        break;
    case 1:   // tipos sencillos: solo CreateSubTask(tipo)
        c.subType = Pick(std::array<u32, 7>{kPause, kGetUp, kDie, kLeaveBoat, kWaitDoor, kForceOut, kSetOut}, n, 110);
        break;
    case 2:   // 0x2CF con puertas 8..11 y banderas
        c.subType = kDriveTimed; c.inVeh = Rand(n, 111) % 8 != 0;
        c.door = Rand(n, 112) % 3 == 0 ? 0u : DoorMain(n, 113);
        if (c.pedVehKind == 2 || c.pedVehKind == 1) c.pedVehKind = Rand(n, 114) % 2;
        break;
    case 3:   // 0x329: el vehiculo no deja bajar con calma (salto)
        c.subType = kWaitSlow; c.canStep = 0; c.door = DoorMain(n, 115); break;
    case 4:   // 0x329: hay sitio por la puerta elegida
        c.subType = kWaitSlow; c.canStep = 1; c.roomBits &= ~1u; c.roomBits |= 1u; c.door = DoorMain(n, 116); break;
    case 5:   // 0x329: sin sitio, puertas 8 y 9
        c.subType = kWaitSlow; c.canStep = 1; c.roomBits &= ~1u; c.door = 8u + Rand(n, 117) % 2; break;
    case 6:   // 0x329: sin sitio, puertas 10 y 11
        c.subType = kWaitSlow; c.canStep = 1; c.roomBits &= ~1u; c.door = 10u + Rand(n, 118) % 2; break;
    case 7:   // 0x329: sin sitio, puertas raras
        c.subType = kWaitSlow; c.canStep = 1; c.roomBits &= ~1u; c.door = Pick(kDoorsAny, n, 119); break;
    case 8:   // 0x32D
        c.subType = kGetOut; c.lineKind = 0; c.door = DoorMain(n, 120); c.vType = Rand(n, 121) % 4 ? 0u : VehTypeAny(n, 122); break;
    case 9:   // 0x32D con el vehiculo automovil y la puerta cerrable
        c.subType = kGetOut; c.lineKind = 0; c.vType = 0; c.door = DoorMain(n, 123); c.lock = Rand(n, 124) % 2 ? 1u : Rand(n, 125) % 8; break;
    case 10:  // 0x32E
        c.subType = kJumpOut; c.vehNull = Rand(n, 126) % 12 == 0; c.vType = Rand(n, 127) % 3 ? 0u : VehTypeAny(n, 128);
        c.vSub = Rand(n, 129) % 4 ? 0u : 2u; c.die = Rand(n, 130) % 4 == 0; break;
    case 11:  // 0x32E sin muerte, para llegar a la limpieza de banderas
        c.subType = kJumpOut; c.die = 0; c.vehNull = 0; c.vType = 0; c.vSub = 0; c.inAir = Rand(n, 131) % 4 == 0; c.drowning = Rand(n, 132) % 4 == 0; break;
    case 12:  // 0x337 y 0x330
        c.subType = Rand(n, 133) % 2 ? kBeDragged : kSetOut; c.vehNull = Rand(n, 134) % 8 == 0; c.lock = Rand(n, 135) % 2 ? 5u : Rand(n, 136) % 8; break;
    case 13:  // 0x329 con todo aleatorio
        c.subType = kWaitSlow; c.canStep = Rand(n, 137) % 6 != 0; break;
    default:  // 14 y 15: mezcla
        c.subType = Rand(n, 138) % 5 == 0 ? Pick(kTypesNull, n, 139) : TypeMain(n, 140);
        if (c.family == 15) {
            c.allocNull = Rand(n, 141) % 3 == 0; c.createNull = Rand(n, 142) % 2; c.lineKind = Rand(n, 143) % 3;
        }
        break;
    }
    // La utilidad de alineacion solo puede ser nula o cruda si la rama no la desreferencia (0x32D la lee)
    if (c.family != 15 && c.subType == kWaitSlow) c.allocNull = Rand(n, 150) % 7 == 0;
    // CDamageManager::GetDoorStatus_Component/SetDoorStatus_Component de la DLL reciben la puerta como uint8 (tComponent) y el EXE como dword: en 0x32D la puerta se limita a 0..255 (el juego solo usa 0..11)
    if (c.subType == kGetOut) c.door &= 0xFFu;
    if (c.subType == kGetOut) c.lineKind = 0;
    else if (c.family != 15) c.lineKind = Rand(n, 151) % 3;
    // El EXE desreferencia el vehiculo de la tarea salvo en 0x32E y 0x337
    if (c.subType != kJumpOut && c.subType != kBeDragged) c.vehNull = 0;
    // 0x2CF: con la puerta 0 la funcion calcula la puerta; el peaton debe poder ser pasajero/conductor
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack).U("sub_type", c.subType);
    j.U("door", c.door).U("delay", c.delay).U("sensible", c.sensible).U("force", c.force).U("die", c.die).U("in_air", c.inAir).U("veh_null", c.vehNull);
    j.U("line_kind", c.lineKind).U("line_raw", c.lineRaw).U("flags_set", c.flagsSet).U("num_in", c.numIn).U("die_anim", c.dieAnim).U("die_blend", c.dieBlend).U("die_speed", c.dieSpeed);
    j.U("in_veh", c.inVeh).U("drowning", c.drowning).U("ped_veh_kind", c.pedVehKind);
    j.U("v_type", c.vType).U("v_sub", c.vSub).U("lock", c.lock).U("drv_kind", c.drvKind).U("pass0", c.pass[0]).U("pass1", c.pass[1]).U("pass2", c.pass[2]);
    j.U("in_flags", c.inFlags).U("out_flags", c.outFlags).U("num_getting", c.numGetting).U("status", c.status).U("model_flags", c.modelFlags).U("nodes", c.nodes);
    j.U("can_step", c.canStep).U("passenger_ret", c.passengerRet).U("driver_ret", c.driverRet).U("player_bits", c.playerBits).U("room_bits", c.roomBits).U("has_door_bits", c.hasDoorBits);
    j.U("status0", c.statusSeq[0]).U("status1", c.statusSeq[1]).U("status2", c.statusSeq[2]);
    j.U("door_flag_ret", c.doorFlagRet).U("target_door_ret", c.targetDoorRet).U("create_null", c.createNull).U("alloc_null", c.allocNull);
    return j.End();
}
}

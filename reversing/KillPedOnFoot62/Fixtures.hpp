#pragma once
// Fixtures del banco KillPedOnFoot62: CTaskComplexKillPedOnFoot::{ControlSubTask 0x626260, CreateFirstSubTask 0x62B490, CreateNextSubTask 0x62B150, CreateSubTask 0x625E70, MakeAbortable 0x625E40}.
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y tambien un programa autonomo de calibracion (fixture_mirror.py).
//
// Todas las funciones son thiscall con `this` en ECX (ESI en el EXE). Resumen del desensamblado:
//   MakeAbortable 0x625E40 (ret 0xC): ped->[+0x474] &= ~0x400000; sin subtarea -> true; si no, subtarea->MakeAbortable(ped, prioridad, evento) (vtable +0x18) y devuelve su AL.
//   CreateSubTask 0x625E70 (ret 8): despacha por taskId (ver fixture_oracle.py).
//   CreateNextSubTask 0x62B150 (ret 4), CreateFirstSubTask 0x62B490 (ret 4), ControlSubTask 0x626260 (ret 4): ver fixture_oracle.py.
// El mundo del banco es UNA sola arena con regiones separadas por guardas de 0x20 bytes (una guarda inicial y una tras cada region; la region se redondea a 16).
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace KillPedOnFootFix {
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

// ------------------------------------------------------------------------------------------------ rutas y casos
enum Route : u32 { kControl = 0, kFirst = 1, kNext = 2, kSub = 3, kAbort = 4, kRoutes = 5 };
inline constexpr std::array<u32, kRoutes> kRouteCases{2048, 768, 768, 1024, 512};
constexpr unsigned kCases = 2048 + 768 + 768 + 1024 + 512;   // 5120
inline constexpr u32 RouteStart(u32 r) { u32 s = 0; for (u32 i = 0; i < r; ++i) s += kRouteCases[i]; return s; }

// ------------------------------------------------------------------------------------------------ disposicion de la arena (guarda 0x20 | region redondeada a 16 | ...)
enum Region : u32 { R_THIS, R_PED, R_TGT, R_PMAT, R_TMAT, R_PVEH, R_TVEH, R_INTEL, R_WP, R_PD, R_WT, R_CP, R_CT, R_GRP, R_SUB, R_ALLOC, R_OTHER, R_LAB, kRegions };
inline constexpr std::array<u32, kRegions> kRegionBytes{
    0x38,    // R_THIS  CTaskComplexKillPedOnFoot (sizeof 0x38)
    0x7A0,   // R_PED   CPed (0x79C) + byte +0x79C del CCopPed (m_bDontPursuit)
    0x7A0,   // R_TGT   igual
    0x48,    // R_PMAT  CMatrix del ped
    0x48,    // R_TMAT  CMatrix del objetivo
    0x5A0,   // R_PVEH  CVehicle del ped
    0x5A0,   // R_TVEH  CVehicle del objetivo
    0x80,    // R_INTEL CPedIntelligence parcial (+0x68 grupo de eventos)
    0x30,    // R_WP    CWanted devuelto por FindPlayerWanted
    0x40,    // R_PD    CPlayerPedData del objetivo (+0 CWanted*, +0x34 banderas)
    0x30,    // R_WT    CWanted del objetivo
    0x40,    // R_CP    entidad de contacto del ped (+0x2F area)
    0x40,    // R_CT    entidad de contacto del objetivo
    0x40,    // R_GRP   CPedGroup (solo direccion; GetLeader recibe +8)
    0x40,    // R_SUB   subtarea falsa (vtable, bytes +0x10 / +0x30 / +0x33 / +0x35)
    0x70,    // R_ALLOC objeto que reserva el falso de operator new (la mayor tarea mide 0x70)
    0x40,    // R_OTHER direccion que devuelve a veces FindPlayerPed
    0x40,    // R_LAB   etiquetas de punteros que solo se comparan (m_standingOnEntity, evento)
};
constexpr u32 kGuard = 0x20;
constexpr u32 Round16(u32 v) { return (v + 15u) & ~15u; }
constexpr u32 RegionOff(u32 r) { u32 o = kGuard; for (u32 i = 0; i < r; ++i) o += Round16(kRegionBytes[i]) + kGuard; return o; }
constexpr u32 kArena = RegionOff(kRegions);   // fin de la ultima guarda
// Suma a mano (guarda inicial 0x20 + por region: redondeo a 16 + guarda 0x20):
//   THIS 0x40+0x20, PED 0x7A0+0x20, TGT 0x7A0+0x20, PMAT 0x50+0x20 (0x48 -> 0x50), TMAT 0x50+0x20, PVEH 0x5A0+0x20, TVEH 0x5A0+0x20, INTEL 0x80+0x20, WP 0x30+0x20, PD 0x40+0x20, WT 0x30+0x20,
//   CP 0x40+0x20, CT 0x40+0x20, GRP 0x40+0x20, SUB 0x40+0x20, ALLOC 0x70+0x20, OTHER 0x40+0x20, LAB 0x40+0x20  (THIS 0x38 -> 0x40)
static_assert(Round16(0x38) == 0x40 && Round16(0x48) == 0x50 && Round16(0x70) == 0x70);

// Desplazamientos del EXE (desensamblado)
constexpr u32 kSubAt = 8, kFlagsAt = 0xC, kTargetAt = 0x10, kPedFlagsAt = 0x14, kDelayAt = 0x18, kChanceAt = 0x1C, kCompAt = 0x20, kStartAt = 0x24, kTimeAt = 0x28, kTmStartAt = 0x2C, kTmIntervalAt = 0x30, kTmStartedAt = 0x34, kTmStoppedAt = 0x35;
constexpr u32 kPosAt = 4, kMatrixAt = 0x14, kMatPosAt = 0x30, kF46CAt = 0x46C, kF474At = 0x474, kF478At = 0x478, kIntelAt = 0x47C, kPDataAt = 0x480, kCreatedAt = 0x484, kHealthAt = 0x540, kStandAt = 0x568, kContactAt = 0x584,
              kVehAt = 0x58C, kTypeAt = 0x598, kWeaponsAt = 0x5A0, kWeaponSize = 0x1C, kAmmoInWeapon = 0xC, kSlotAt = 0x718, kCopAt = 0x79C, kRefsAt = 0x24, kAreaAt = 0x2F;
constexpr u32 kVelAt = 0x44, kVTypeAt = 0x590, kVSubAt = 0x594;
constexpr u32 kWChanceAt = 0x1C, kWBitsAt = 0x1E, kPdWantedAt = 0, kPdFlagsAt = 0x34;
constexpr u32 kTimeAddr = 0xB7CB84;
constexpr u32 kMaxEvents = 24;
constexpr u32 kVtSlots = 16;

// ------------------------------------------------------------------------------------------------ constantes de tareas
constexpr u32 T_NONE = 0xC8, T_PAUSE = 0xCA, T_STAND = 0xCB, T_DRAG = 0x2BF, T_LEAVECAR = 0x2C0, T_DRIVE = 0x2CF, T_MELEE = 0x3E9, T_ARMED = 0x3EA, T_DESTROY = 0x3EB, T_SIGNAL = 0x4BA, T_FINISHED = 0x516;

// ------------------------------------------------------------------------------------------------ caso
struct C {
    u32 route, family, idx;
    // this
    u32 tflags, tgtNull, pedFlagsT, delayT, chanceT, compT, startT, timeT, tmStart, tmInterval, tmStarted, tmStopped, timeMs;
    u32 subNull, subType, subB10, subB30, subB33, subB35;
    // ped
    u32 pType, pPos[3], pAlt[3], pMat, pF46C, pF474, pF478, pCreated, pStand, pContact, pVeh, pCop, pSlot, pAmmo[13];
    // objetivo
    u32 tType, tPos[3], tAlt[3], tMat, tHealth, tF46C, tStand, tContact, tVeh, tPdFlags, twChance, twBits;
    // wanted del jugador (FindPlayerWanted)
    u32 wpChance, wpBits;
    // vehiculo del objetivo
    u32 vx, vy, vType, vSub;
    // codigos de area de las entidades de contacto
    u32 cpCode, ctCode;
    // respuestas de los falsos (un bit por llamada sucesiva) y basura en los bits altos
    u32 mMelee, mDriver, mPassenger, mCanOpen, mRoom, mHealthy, mNoPolice, mAbort, mStillProc, garbage;
    u32 selPlayer, selGroup, selLeader, door, randVal;
    // argumentos
    u32 prio, evt, taskId;
    u32 allocNull, fillAlloc;
    u32 eax, edx, stack;
};

inline constexpr u32 kOneF = 0x3F800000u, kZeroF = 0u, kNegZeroF = 0x80000000u, kNanF = 0x7FC00000u, kInfF = 0x7F800000u, kNegInfF = 0xFF800000u;
inline constexpr u32 kHealths[]{0x41200000u, 0u, 0x80000000u, 0xBF800000u, 0x3F800000u, 0x00000001u, 0x80000001u, 0x7FC00000u, 0xFFC00000u, 0x7F800000u, 0xFF800000u, 0x42C80000u, 0x00800000u, 0xBF000000u};
// Velocidades alrededor de 0.1f (0x3DCCCCCD) y casos especiales
inline constexpr u32 kSpeedX[]{0u, 0x3DCCCCCCu, 0x3DCCCCCDu, 0x3DCCCCCEu, 0x3DCCCCCBu, 0x3DCCCCCFu, 0x3D8F5C29u, 0xBDCCCCCDu, 0x3F800000u, 0x00000001u, 0x7FC00000u, 0x7F800000u, 0x80000000u, 0x3E4CCCCDu, 0x358637BDu, 0x3D6147AEu};
inline constexpr u32 kSpeedY[]{0u, 0x3DCCCCCDu, 0x3D8F5C29u, 0x80000000u, 0x00000001u, 0x7FC00000u, 0xBDCCCCCDu, 0x3D6147AEu, 0x3DCCCCCCu, 0x3F000000u, 0xFF800000u, 0x3E800000u};
// Deltas para la distancia 5.0f (sqrt((dz*dz + dy*dy) + dx*dx) < 5)
inline constexpr u32 kDeltaX[]{0x40A00000u, 0x409FFFFFu, 0x40A00001u, 0x40800000u, 0u, 0x3F800000u, 0x7FC00000u, 0x7F800000u, 0x80000000u, 0x4B189680u, 0x00000001u, 0xC0A00000u, 0x40466666u};
inline constexpr u32 kDeltaY[]{0u, 0x40400000u, 0x3F800000u, 0x80000000u, 0x7FC00000u, 0x409FFFFFu, 0x00000001u, 0xC0800000u, 0x40000000u};
inline constexpr u32 kPedTypes[]{0, 1, 2, 3, 4, 5, 6, 6, 7, 8, 10, 16, 17, 18, 20, 23, 24, 31, 32, 0xFF, 0x80000000u, 6, 7, 1, 0};
inline constexpr u32 kSubTypesAll[]{T_NONE, T_PAUSE, T_STAND, T_DRAG, T_LEAVECAR, T_DRIVE, T_MELEE, T_ARMED, T_DESTROY, T_SIGNAL, T_FINISHED, 0xC9u, 0xCCu, 0x3E8u, 0x3ECu, 0x4B9u, 0u, 0xFFFFFFFFu, 0x2C1u, 0x2BEu, 0x2CEu, 0x2D0u};
inline constexpr u32 kTaskIds[]{T_NONE, T_PAUSE, T_STAND, T_DRAG, T_LEAVECAR, T_DRIVE, T_MELEE, T_ARMED, T_DESTROY, T_SIGNAL, T_FINISHED, 0xC9u, 0xCCu, 0x3E8u, 0x3ECu, 0x4B9u, 0x4BBu, 0u, 0xFFFFFFFFu, 0x2C1u, 0x2BEu, 0x2CEu, 0x2D0u, 0x517u, 0x515u};
inline constexpr u32 kTimes[]{0u, 0xFFFFFFFFu, 0xFFFF0000u, 0x7FFFFFFFu, 0x80000000u, 100000u, 1u, 2000u};
inline constexpr u32 kRands[]{0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, 0x4000u, 0x2AAAu, 0x7FFEu};

inline u32 Pick(u32 n, u32 k, const u32* a, std::size_t m) { return a[Rand(n, k) % m]; }
inline bool Pct(u32 n, u32 k, u32 pct) { return Rand(n, k) % 100u < pct; }
inline u32 FloatBits(float v) { u32 b; std::memcpy(&b, &v, 4); return b; }
inline float BitsFloat(u32 b) { float v; std::memcpy(&v, &b, 4); return v; }
inline u32 NormalF(u32 n, u32 k) {
    static const float scales[]{0.01f, 0.5f, 1.0f, 3.0f, 10.0f, 100.0f, 1000.0f, 1.0e5f, 1.0e7f, 1.0e9f};
    const float s = scales[Rand(n, k) % std::size(scales)];
    const float u = float(Rand(n, k + 1) % 2000001u) / 1000000.0f - 1.0f;
    return FloatBits(u * s);
}

// Bandas de ped / objetivo (bits del EXE)
constexpr u32 kBitInVeh = 1u << 8, kBitStay = 1u << 21, kBitDontDrag = 1u << 29, kBitNoIK = 1u << 22, kBitSignal = 1u << 12;

inline C MakeC(u32 n) {
    C c{};
    u32 r = 0;
    while (r + 1 < kRoutes && n >= RouteStart(r + 1)) ++r;
    const u32 i = n - RouteStart(r);
    c.route = r;
    c.idx = i;
    const u32 f = i % 16;
    c.family = f;
    c.eax = Rand(n, 70);
    c.edx = Rand(n, 71);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    c.garbage = Rand(n, 72);
    // --- this
    c.tflags = Rand(n, 100) & 0xFFu;                     // bits 0..6 de banderas y el bit 7 sin uso
    c.tgtNull = Pct(n, 101, 6) ? 1u : 0u;
    c.pedFlagsT = Rand(n, 102);
    c.delayT = Rand(n, 103) % 3 ? Rand(n, 104) % 5000u : Rand(n, 105);
    c.chanceT = Rand(n, 106) % 3 ? Rand(n, 107) % 101u : Rand(n, 108);
    c.compT = Rand(n, 109) % 3 ? Rand(n, 110) % 4u : (Rand(n, 111) & 0xFFu);
    c.timeMs = Rand(n, 112) % 5 == 0 ? Pick(n, 113, kTimes, std::size(kTimes)) : (Rand(n, 114) % 3 ? 100000u + Rand(n, 115) % 100000u : Rand(n, 116));
    c.startT = Rand(n, 117) % 3 == 0 ? c.timeMs - Rand(n, 118) % 5000u : Rand(n, 119);
    c.timeT = Rand(n, 120) % 4 == 0 ? u32(-1) : Rand(n, 121) % 3 == 0 ? 0u : Rand(n, 122) % 3 == 0 ? Rand(n, 123) % 5000u : Rand(n, 124);
    c.tmStart = Rand(n, 125) % 2 ? c.timeMs - Rand(n, 126) % 4000u : Rand(n, 127);
    c.tmInterval = Rand(n, 128) % 3 == 0 ? 2000u : Rand(n, 129) % 3 == 0 ? u32(-1) : Rand(n, 130) % 5000u;
    c.tmStarted = Rand(n, 131) % 3 == 0 ? 0u : Rand(n, 132) % 8 == 0 ? (Rand(n, 133) & 0xFFu) : 1u;
    c.tmStopped = Rand(n, 134) % 5 == 0 ? 1u : Rand(n, 135) % 9 == 0 ? (Rand(n, 136) & 0xFFu) : 0u;
    // --- subtarea
    c.subNull = 0;
    c.subType = Rand(n, 137) % 5 == 0 ? Rand(n, 138) : Pick(n, 139, kSubTypesAll, std::size(kSubTypesAll));
    c.subB10 = Rand(n, 140) & 0xFFu;
    c.subB30 = Rand(n, 141) & 0xFFu;
    c.subB33 = Rand(n, 142) & 0xFFu;
    c.subB35 = Rand(n, 143) & 0xFFu;
    // --- ped
    c.pType = Pct(n, 144, 30) ? 6u : Pick(n, 145, kPedTypes, std::size(kPedTypes));
    const u32 base[3]{NormalF(n, 146), NormalF(n, 148), NormalF(n, 150)};
    for (u32 k = 0; k < 3; ++k) {
        c.pPos[k] = base[k];
        c.pAlt[k] = NormalF(n, 152 + 2 * k);
    }
    c.pMat = Rand(n, 158) % 2;
    c.pF46C = Rand(n, 159);
    c.pF474 = Rand(n, 160);
    c.pF478 = Rand(n, 161);
    c.pCreated = Rand(n, 162) % 3 == 0 ? 2u : (Rand(n, 163) % 3 == 0 ? Rand(n, 164) & 0xFFu : (Rand(n, 165) % 3));
    c.pStand = Rand(n, 166) % 3;
    c.pContact = Rand(n, 167) % 4;
    c.pVeh = Pct(n, 168, 80) ? 1u : 0u;
    c.pCop = Pct(n, 169, 55) ? 1u : (Rand(n, 170) % 4 == 0 ? (Rand(n, 171) & 0xFFu) : 0u);
    c.pSlot = Rand(n, 172) % 13;
    for (u32 k = 0; k < 13; ++k) {
        const u32 q = Rand(n, 180 + k) % 6;
        c.pAmmo[k] = q == 0 ? 0u : q == 1 ? 1u + Rand(n, 200 + k) % 500u : q == 2 ? 0x80000000u + Rand(n, 220 + k) % 3u : q == 3 ? u32(-1) : q == 4 ? Rand(n, 240 + k) : 0u;
    }
    // --- objetivo
    c.tType = Pct(n, 260, 35) ? (Rand(n, 261) % 2) : Pick(n, 262, kPedTypes, std::size(kPedTypes));
    c.tPos[0] = Pct(n, 263, 60) ? FloatBits(BitsFloat(base[0]) + BitsFloat(Pick(n, 264, kDeltaX, std::size(kDeltaX)))) : NormalF(n, 265);
    c.tPos[1] = Pct(n, 266, 60) ? FloatBits(BitsFloat(base[1]) + BitsFloat(Pick(n, 267, kDeltaY, std::size(kDeltaY)))) : NormalF(n, 268);
    c.tPos[2] = Pct(n, 269, 60) ? base[2] : NormalF(n, 270);
    for (u32 k = 0; k < 3; ++k) c.tAlt[k] = NormalF(n, 272 + 2 * k);
    c.tMat = Rand(n, 280) % 2;
    c.tHealth = Pct(n, 281, 55) ? 0x41200000u : Pick(n, 282, kHealths, std::size(kHealths));
    c.tF46C = Rand(n, 283);
    c.tStand = Rand(n, 284) % 3;
    c.tContact = Rand(n, 285) % 4;
    c.tVeh = Pct(n, 286, 85) ? 1u : 0u;
    c.tPdFlags = Rand(n, 287) % 2 ? (Rand(n, 288) | 1u) & 0xFFu : (Rand(n, 289) & 0xFEu);
    c.twChance = Rand(n, 290) % 3 == 0 ? 0u : Rand(n, 291) % 4 == 0 ? 0xFFFFu : (Rand(n, 292) & 0xFFFFu);
    c.twBits = Pct(n, 293, 45) ? 4u | (Rand(n, 294) & 0xFBu) : (Rand(n, 295) & 0xFBu);
    c.wpChance = Rand(n, 296) % 3 == 0 ? 0u : Rand(n, 297) % 4 == 0 ? 0xFFFFu : (Rand(n, 298) & 0xFFFFu);
    c.wpBits = Pct(n, 299, 40) ? 4u | (Rand(n, 300) & 0xFBu) : (Rand(n, 301) & 0xFBu);
    // --- vehiculo del objetivo
    c.vx = Pct(n, 302, 70) ? Pick(n, 303, kSpeedX, std::size(kSpeedX)) : NormalF(n, 304);
    c.vy = Pct(n, 306, 70) ? Pick(n, 307, kSpeedY, std::size(kSpeedY)) : NormalF(n, 308);
    c.vType = Rand(n, 310) % 3 == 0 ? Pick(n, 311, std::array<u32, 8>{0, 5, 9, 1, 2, 10, 3, 4}.data(), 8) : Rand(n, 312) % 12;
    c.vSub = Rand(n, 313) % 3 == 0 ? (3u + Rand(n, 314) % 2u) : (Rand(n, 315) % 4 == 0 ? Rand(n, 316) : Rand(n, 317) % 12);
    c.cpCode = Rand(n, 320) & 0xFFu;
    c.ctCode = Rand(n, 321) % 2 ? c.cpCode : (Rand(n, 322) & 0xFFu);
    // --- falsos
    c.mMelee = Rand(n, 330); c.mDriver = Rand(n, 331); c.mPassenger = Rand(n, 332); c.mCanOpen = Rand(n, 333); c.mRoom = Rand(n, 334); c.mHealthy = Rand(n, 335);
    c.mNoPolice = Rand(n, 336); c.mAbort = Rand(n, 337); c.mStillProc = Rand(n, 338);
    c.selPlayer = Rand(n, 339); c.selGroup = Rand(n, 340); c.selLeader = Rand(n, 341); c.door = Rand(n, 342) % 3 ? Rand(n, 343) % 8 : Rand(n, 344);
    c.randVal = Rand(n, 345) % 3 == 0 ? Pick(n, 346, kRands, std::size(kRands)) : (Rand(n, 347) & 0x7FFFu);
    c.prio = Rand(n, 350) % 3 == 0 ? Rand(n, 351) : Rand(n, 352) % 3;
    c.evt = Rand(n, 353) % 3 == 0 ? 0u : 1u;
    c.taskId = Pick(n, 354, kTaskIds, std::size(kTaskIds));
    c.allocNull = Pct(n, 355, 7) ? 1u : 0u;
    c.fillAlloc = 1u + Rand(n, 356) % 255u;

    // ---------------------------------------------------------------- escenarios por ruta y familia
    const auto Common = [&](bool objetivoJugador) {
        if (objetivoJugador) c.tType = Rand(n, 400) % 2;
        c.tPdFlags |= 0u;
    };
    const auto Veh = [&](bool inVeh, u32 pctOk) {
        // objetivo en vehiculo con vehiculo valido (pctOk % de las veces) y velocidad pequena
        if (inVeh) c.tF46C |= kBitInVeh; else c.tF46C &= ~kBitInVeh;
        if (inVeh) c.tVeh = Pct(n, 401, pctOk) ? 1u : 0u;
    };
    const auto Favor = [&](u32 pct) {   // escalon de vehiculo favorable: el objetivo va en un coche normal, quieto, con todas las comprobaciones a favor y el temporizador vencido (con bordes exactos)
        if (!Pct(n, 600, pct)) return;
        c.vType = Pick(n, 601, std::array<u32, 4>{0, 1, 2, 10}.data(), 4);
        c.vSub = Pct(n, 615, 8) ? 3u + Rand(n, 616) % 2u : Rand(n, 602) % 3;
        c.vx = Pick(n, 603, std::array<u32, 6>{0u, 0x3D6147AEu, 0x3D8F5C29u, 0x80000000u, 0x3DCCCCCDu, 0x358637BDu}.data(), 6);
        c.vy = Pick(n, 604, std::array<u32, 5>{0u, 0x3D6147AEu, 0x80000000u, 0x3DCCCCCCu, 0x00000001u}.data(), 5);
        c.pF46C &= ~kBitStay;
        c.tF46C = (c.tF46C | kBitInVeh) & ~kBitDontDrag;
        if (Pct(n, 609, 12)) c.tF46C |= kBitDontDrag;
        c.tVeh = 1;
        c.mDriver |= 1u;
        c.mPassenger |= 1u;
        c.mCanOpen |= 1u;
        c.mHealthy |= 1u;
        if (Pct(n, 610, 50)) c.mRoom |= 1u; else c.mRoom &= ~1u;
        if (Pct(n, 606, 70)) {
            c.tmStarted = 1;
            c.tmStopped = Pct(n, 607, 20) ? 1u : 0u;
            c.tmStart = c.timeMs - 3000u;
            c.tmInterval = Pct(n, 611, 25) ? (Pct(n, 612, 50) ? 3000u : (Pct(n, 613, 50) ? 3001u : 2999u)) : 2000u;
            if (c.tmStopped) c.tmInterval = Pct(n, 614, 50) ? 0u : 2000u;
        }
    };
    const auto ShiftDist = [&](u32 k) {   // fuerza la distancia al borde de 5.0f con el ped y el objetivo en las posiciones elegidas
        c.tPos[0] = FloatBits(BitsFloat(base[0]) + BitsFloat(Pick(n, k, kDeltaX, std::size(kDeltaX))));
        c.tPos[1] = FloatBits(BitsFloat(base[1]) + BitsFloat(Pick(n, k + 1, kDeltaY, std::size(kDeltaY))));
        c.tPos[2] = base[2];
    };
    switch (r) {
    case kAbort:
        c.subNull = f % 4 == 0 ? 1u : 0u;
        c.pF474 = Rand(n, 410) % 2 ? (c.pF474 | kBitNoIK) : (c.pF474 & ~kBitNoIK);
        c.tgtNull = Rand(n, 411) % 2;
        break;
    case kSub: {
        // 16 familias de tareas; el resto al azar
        static const u32 order[16]{T_NONE, T_PAUSE, T_PAUSE, T_STAND, T_DRAG, T_LEAVECAR, T_DRIVE, T_MELEE, T_ARMED, T_ARMED, T_DESTROY, T_SIGNAL, T_SIGNAL, T_FINISHED, 0u, 0u};
        c.taskId = order[f];
        if (f >= 14) c.taskId = Rand(n, 414) % 2 ? Pick(n, 415, kTaskIds, std::size(kTaskIds)) : Rand(n, 416);
        if (c.taskId == T_DESTROY || c.taskId == T_DRAG || c.taskId == T_MELEE || c.taskId == T_ARMED || c.taskId == T_SIGNAL) c.tgtNull = Pct(n, 417, 5) && c.taskId != T_DESTROY ? 1u : 0u;
        if (c.taskId == T_ARMED) c.allocNull = 0;   // el EXE escribiria en nulo+0x35
        if (c.taskId == T_PAUSE) { c.tflags = (c.tflags & ~0x60u) | (Rand(n, 418) % 4) * 0x20u; }
        if (f == 12) { c.selPlayer = (c.selPlayer & ~3u) | 1u; }
        if (c.taskId == T_DESTROY && !c.tgtNull) c.tVeh = Pct(n, 419, 85) ? 1u : 0u;
        break;
    }
    case kNext: {
        c.subNull = 0;
        c.tgtNull = Pct(n, 420, 4) ? 1u : 0u;
        static const u32 subs[16]{T_PAUSE, T_PAUSE, T_PAUSE, T_DRAG, T_DRAG, T_LEAVECAR, T_LEAVECAR, T_LEAVECAR, T_LEAVECAR, T_DRIVE, T_MELEE, T_ARMED, T_DESTROY, T_STAND, T_NONE, T_SIGNAL};
        c.subType = f == 15 && Rand(n, 421) % 2 ? Pick(n, 422, kSubTypesAll, std::size(kSubTypesAll)) : subs[f];
        if (f <= 2) {   // pausa: policia / espera a que el jugador este a salvo
            c.pType = Pct(n, 423, 60) ? 6u : c.pType;
            c.pCop = Pct(n, 424, 70) ? 1u : 0u;
            if (f == 2) c.tflags |= 0x60u; else if (f == 1) c.tflags = (c.tflags & ~0x60u) | (Rand(n, 425) % 4) * 0x20u;
            if (Pct(n, 426, 50)) c.tflags |= 0x60u;
        }
        if (f >= 5 && f <= 8) {   // salir del coche: objetivo jugador, pistola, distancia 5
            Common(Pct(n, 427, 70));
            if (Pct(n, 428, 75)) c.tPdFlags |= 1u;
            if (Pct(n, 429, 25)) c.pF46C |= kBitInVeh; else c.pF46C &= ~kBitInVeh;
            if (Pct(n, 430, 70)) ShiftDist(431);
            c.pType = Pct(n, 433, 50) ? Pick(n, 434, std::array<u32, 6>{7, 8, 10, 16, 6, 6}.data(), 6) : c.pType;
            if (Pct(n, 435, 60)) c.pCreated = Rand(n, 436) % 2 ? 0u : 1u;
            c.pStand = 1 + Rand(n, 437) % 2; c.tStand = Rand(n, 438) % 3;
        }
        break;
    }
    case kFirst: {
        c.tgtNull = Pct(n, 440, 5) ? 1u : 0u;
        if (f >= 2) Common(Pct(n, 441, 70));
        if (Pct(n, 442, 75)) c.tPdFlags |= 1u;
        if (Pct(n, 443, 12)) c.pF46C |= kBitInVeh; else c.pF46C &= ~kBitInVeh;
        if (f % 4 != 0) {   // objetivo en un vehiculo
            Veh(true, 90);
            if (Pct(n, 444, 70)) { c.vx = Pick(n, 445, kSpeedX, std::size(kSpeedX)); c.vy = Pick(n, 446, kSpeedY, std::size(kSpeedY)); }
            c.vSub = Pct(n, 447, 12) ? 3u + Rand(n, 448) % 2u : Rand(n, 449) % 3u;
            c.vType = Pct(n, 450, 25) ? 9u : Pct(n, 451, 20) ? 5u : Rand(n, 452) % 3u;
            c.pF46C &= ~kBitStay;
            if (Pct(n, 453, 6)) c.pF46C |= kBitStay;
            if (Pct(n, 454, 25)) c.tF46C |= kBitDontDrag; else c.tF46C &= ~kBitDontDrag;
        } else {
            Veh(false, 0);
        }
        if (f % 4 != 0) Favor(50);
        if (Pct(n, 455, 60)) ShiftDist(456);
        c.pStand = 1 + Rand(n, 458) % 2; c.tStand = Rand(n, 459) % 3;
        c.pType = Pct(n, 460, 50) ? Pick(n, 461, std::array<u32, 6>{7, 8, 10, 16, 6, 6}.data(), 6) : c.pType;
        break;
    }
    case kControl: {
        c.subNull = 0;
        c.tgtNull = f == 0 ? 1u : (Pct(n, 470, 2) ? 1u : 0u);
        if (f >= 1) c.timeT = Pct(n, 471, 85) ? u32(-1) : c.timeT;       // sin tiempo limite salvo en la familia 2
        const bool toPlayer = Pct(n, 472, 65);
        if (f >= 1) Common(toPlayer);
        switch (f) {
        case 1:   // subtarea de pausa
            c.subType = T_PAUSE;
            c.tflags = (c.tflags & ~0x62u) | (Rand(n, 473) % 4) * 0x20u;
            break;
        case 2:   // tiempo agotado
            c.timeT = Pct(n, 474, 80) ? 100u + Rand(n, 475) % 5000u : c.timeT;
            c.startT = c.timeMs - (Rand(n, 476) % 3 == 0 ? Rand(n, 477) % 100u : (c.timeT + (Rand(n, 478) % 3) - 1));
            c.tflags &= ~2u;
            break;
        case 3:   // objetivo nuevo
            c.tflags |= 2u;
            break;
        case 4:   // objetivo muerto
            c.tHealth = Pct(n, 479, 80) ? Pick(n, 480, std::array<u32, 6>{0u, 0x80000000u, 0xBF800000u, 0x80000001u, 0xFF800000u, 0xBF000000u}.data(), 6) : c.tHealth;
            c.tflags &= ~(2u | 8u);
            c.subType = Pct(n, 481, 25) ? T_SIGNAL : c.subType;
            c.pF478 = Pct(n, 482, 60) ? (c.pF478 | kBitSignal) : (c.pF478 & ~kBitSignal);
            break;
        case 5:   // objetivo vivo jugador: a salvo / no
            c.tflags &= ~(2u | 8u);
            c.tHealth = Pct(n, 483, 80) ? 0x41200000u : c.tHealth;
            if (toPlayer) { c.tType = Rand(n, 484) % 2; c.pCop = Pct(n, 485, 60) ? 1u : 0u; c.pType = Pct(n, 486, 70) ? 6u : c.pType; }
            c.subType = Pct(n, 487, 40) ? T_PAUSE : Pick(n, 488, std::array<u32, 5>{T_MELEE, T_ARMED, T_DRAG, T_DESTROY, T_STAND}.data(), 5);
            c.tflags = Pct(n, 489, 50) ? (c.tflags | 0x60u) : (c.tflags & ~0x60u);
            break;
        case 6:   // melee
        case 7:   // armada
        case 8:   // arrastrar
        case 9:   // destruir
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15: {
            static const u32 subs[16]{0, 0, 0, 0, 0, 0, T_MELEE, T_ARMED, T_DRAG, T_DESTROY, T_MELEE, T_ARMED, T_DRAG, T_DESTROY, 0, 0};
            c.tflags &= ~(2u | 8u);
            c.tHealth = Pct(n, 490, 88) ? Pick(n, 491, std::array<u32, 4>{0x41200000u, 0x3F800000u, 0x42C80000u, 0x00800000u}.data(), 4) : c.tHealth;
            if (f <= 13 && f != 4) c.subType = subs[f];
            if (f == 14) c.subType = Pick(n, 492, std::array<u32, 6>{T_NONE, T_STAND, T_LEAVECAR, T_DRIVE, T_SIGNAL, 0xFFu}.data(), 6);
            if (f == 15) c.subType = Rand(n, 493) % 2 ? Pick(n, 494, kSubTypesAll, std::size(kSubTypesAll)) : c.subType;
            Veh(Pct(n, 495, 75), 92);
            if (Pct(n, 496, 70)) { c.vx = Pick(n, 497, kSpeedX, std::size(kSpeedX)); c.vy = Pick(n, 498, kSpeedY, std::size(kSpeedY)); }
            c.vSub = Pct(n, 499, 10) ? 3u + Rand(n, 500) % 2u : Rand(n, 501) % 3u;
            c.vType = Pct(n, 502, 12) ? 9u : Pct(n, 503, 10) ? 5u : Rand(n, 504) % 3u;
            c.pF46C &= ~kBitStay;
            if (Pct(n, 505, 6)) c.pF46C |= kBitStay;
            if (Pct(n, 506, 20)) c.tF46C |= kBitDontDrag; else c.tF46C &= ~kBitDontDrag;
            // mascara de respuestas favorables a la rama larga
            if (Pct(n, 507, 70)) { c.mDriver |= 1u; c.mCanOpen |= 1u; c.mHealthy |= 1u; }
            if (f == 7 || f == 11) {   // armada: municion
                c.pF46C = c.pF46C;
            }
            break;
        }
        default: break;
        }
        if (f >= 6 && f <= 13) Favor(65);
        // codigos de area distintos / iguales y contactos
        if (Pct(n, 510, 40)) { c.pContact = 1 + Rand(n, 511) % 3; c.tContact = 1 + Rand(n, 512) % 3; }
        if (f == 10 + 0 && Pct(n, 513, 50)) { c.pContact = 1; c.tContact = 2; }
        break;
    }
    }
    // el objetivo en la posicion del ped si lo pide (distancia 0)
    if (Pct(n, 520, 3)) { c.tPos[0] = c.pPos[0]; c.tPos[1] = c.pPos[1]; c.tPos[2] = c.pPos[2]; }
    // Fixtures que el EXE desreferencia sin comprobar nulo (daria un AV): objetivo en vehiculo con melee/destruir => vehiculo valido; DestroyCar siempre con objetivo; Armed siempre con reserva
    if (r == kControl && (c.tF46C & kBitInVeh) && !c.tVeh && (c.subType == T_MELEE || c.subType == T_DESTROY)) c.tVeh = 1;
    if (r == kSub && c.taskId == T_DESTROY) c.tgtNull = 0;
    if (r != kSub || c.taskId == T_ARMED) c.allocNull = 0;   // el EXE escribe en nulo+0x35 si la reserva de CTaskComplexKillPedOnFootArmed falla; el pool agotado solo se prueba en CreateSubTask
    return c;
}
}  // namespace KillPedOnFootFix

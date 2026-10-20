#pragma once
// Fixtures del banco CollisionScan61. Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp.
//   CCollisionEventScanner::ScanForCollisionEvents (0x604500, thiscall, 2 argumentos (peaton, grupo de eventos), `ret 8`, void)
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace CollisionScan61Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
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
constexpr u32 F(float f) { return std::bit_cast<u32>(f); }
inline float FromBits(u32 v) { return std::bit_cast<float>(v); }
inline float RandF(u32 n, u32 k, float lo, float hi) { return lo + (hi - lo) * float(Rand(n, k) & 0xFFFFFF) * (1.0f / 16777216.0f); }

constexpr u32 kMaxEvents = 64;
constexpr unsigned kCasesColl = 4096;
constexpr u32 kFnColl = 1, kEnabled = kFnColl;   // 16 familias x 256

// Arenas (guarda | objeto | guarda; suma a mano):
constexpr u32 kCsOff = 0x20, kCsBytes = 0x1, kCsArena = 0x41;          // CCollisionEventScanner (this): 0x20 + 0x1 + 0x20
constexpr u32 kVOff = 0x40, kVBytes = 0x5A0, kVArena = 0x620;           // peaton victima: 0x40 + 0x5A0 + 0x40
constexpr u32 kVmOff = 0x20, kVmBytes = 0x48, kVmArena = 0x88;          // matriz de la victima: 0x20 + 0x48 + 0x20
constexpr u32 kIOff = 0x40, kIBytes = 0x100, kIArena = 0x180;           // inteligencia de la victima: 0x40 + 0x100 + 0x40
constexpr u32 kTOff = 0x20, kTBytes = 0x20, kTArena = 0x60;             // tarea (GoTo): 0x20 + 0x20 + 0x20
constexpr u32 kDOff = 0x40, kDBytes = 0x5A0, kDArena = 0x620;           // entidad que dano (edificio / vehiculo / peaton / objeto): 0x40 + 0x5A0 + 0x40
constexpr u32 kDmOff = 0x20, kDmBytes = 0x48, kDmArena = 0x88;          // matriz de la entidad que dano: 0x20 + 0x48 + 0x20
constexpr u32 kDiOff = 0x40, kDiBytes = 0x100, kDiArena = 0x180;        // inteligencia del peaton que dano: 0x40 + 0x100 + 0x40
constexpr u32 kColOff = 0x20, kColBytes = 0x18, kColArena = 0x58;       // modelo de colision (caja): 0x20 + 0x18 + 0x20
constexpr u32 kPdOff = 0x20, kPdBytes = 0x20, kPdArena = 0x60;          // datos de jugador: 0x20 + 0x20 + 0x20
constexpr u32 kClOff = 0x20, kClBytes = 0x20, kClArena = 0x60;          // ropa (solo su direccion): 0x20 + 0x20 + 0x20
constexpr u32 kAOff = 0x20, kABytes = 0x40, kAArena = 0x80;             // entidad a la que esta sujeto el peaton: 0x20 + 0x40 + 0x20
constexpr u32 kGOff = 0x20, kGBytes = 0x20, kGArena = 0x60;             // grupo de eventos (solo su direccion): 0x20 + 0x20 + 0x20
// desplazamientos del EXE
constexpr u32 kEntPosAt = 0x4, kEntMatAt = 0x14, kEntFlagsAt = 0x1C, kEntTypeAt = 0x36, kPhysFlagsAt = 0x40, kPhysSpeedAt = 0x44, kPhysTurnZAt = 0x58, kPhysMassAt = 0x8C,
              kPhysDmgIntAt = 0xD8, kPhysDmgEntAt = 0xDC, kPhysImpactAt = 0xE0, kPhysPosnAt = 0xEC, kPhysPieceAt = 0xF8, kPhysAttachedAt = 0xFC, kPhysIgnoredAt = 0x128,
              kPedFlagsAt = 0x46C, kPedIntelAt = 0x47C, kPedPlayerAt = 0x480, kPedAcquaintanceAt = 0x4E0, kPedShiftAt = 0x550, kPedStandingOnAt = 0x568, kPedContactAt = 0x584, kPedTypeAt = 0x598,
              kVehTowingAt = 0x4C4, kVehSubtypeAt = 0x594, kMatUp = 0x20, kMatPos = 0x30;
constexpr u32 kTimeGlobal = 0xB7CB84, kLastNoiseA = 0xC0B1B0, kLastNoiseB = 0xC0B1B4;

struct C {
    u32 family;
    u32 hitByCar;                                    // byte m_bAlreadyHitByCar (crudo)
    u32 dmgBits, dmgNull, kind, typeNoise;           // intensidad (+D8), danador nulo, tipo (3 bits) y bits altos de +36
    u32 e0[3], ec[3], piece;                         // +E0, +EC, +F8 (word)
    u32 attachKind, attachType;                      // sujeto a: 0 nulo, 1 entidad A
    u32 flags46C, entFlags, standingOn, massBits, shiftX, shiftY, hasPlayer;
    u32 pedMatNull, pos[3], place[3], vmRows[9];
    u32 pedType, contactKind;
    u32 taskNull, isGoto, taskMove;
    u32 dFlags1C, dPhys40, dSpeed[3], dTurnZ, dSubtype, dTow, dAttKind, dPedType, dDamNull;
    u32 dVm[9], dVpos[3], dVplace[3];
    u32 boxMin[3], boxMax[3], headingBits, limitedBits;
    u32 isPlayerV, isPlayerD, hitSide, affects, bala, localDir, acq[2], pedFlag[2], move[2];
    u32 now, lastA, lastB;
    u32 eax, edx, stack;
};

inline constexpr u32 kSpecialBits[]{0x7FC00000u, 0xFFC00000u, 0x7F800000u, 0xFF800000u, 0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0x7FC12345u};
inline u32 SpecialBits(u32 n, u32 k) { return kSpecialBits[Rand(n, k) % std::size(kSpecialBits)]; }
inline u32 RawBool(u32 n, u32 k, bool value) {   // EAX de un bool devuelto: AL (cero o no) y, a veces, basura en los 24 bits altos
    u32 al = value ? (Rand(n, k) % 3 == 0 ? (Rand(n, k + 1) | 1u) & 0xFFu : 1u) : 0u;
    if (value && al == 0) al = 1;
    const u32 hi = Rand(n, k + 2) % 3 == 0 ? (Rand(n, k + 3) & 0xFFFFFF00u) : 0u;
    return hi | al;
}
inline void RotAxes(u32 n, u32 k, float out[9]) {
    const float yaw = RandF(n, k, -3.14159f, 3.14159f), pitch = RandF(n, k + 1, -1.2f, 1.2f), roll = RandF(n, k + 2, -1.2f, 1.2f);
    const float cy = std::cos(yaw), sy = std::sin(yaw), cp = std::cos(pitch), sp = std::sin(pitch), cr = std::cos(roll), sr = std::sin(roll);
    out[0] = cy * cr + sy * sp * sr; out[1] = sy * cr - cy * sp * sr; out[2] = -cp * sr;
    out[3] = -sy * cp;               out[4] = cy * cp;                out[5] = sp;
    out[6] = cy * sr - sy * sp * cr; out[7] = sy * sr + cy * sp * cr; out[8] = cp * cr;
}

// Familias (n % 16, 256 casos cada una):
//  0  salidas rapidas: danador nulo / intensidad 0, -0, NaN, tipos 0 y 5..7 (sin rama), byte de estado crudo        1  edificio (y, a veces, el ruido de pasamontanas)
//  2  objeto: umbrales 1 / 2 de la intensidad, estatico, contacto, sujeto                                           3  objeto: producto escalar, masa, danio por caida (GenerateDamageEvent)
//  4  peaton danado por peaton con la victima jugadora                                                              5  peaton danado por el jugador (ira, odio)
//  6  peaton contra peaton, sin jugadores; el otro peaton con su propio danador                                     7  vehiculo rapido, victima que no es jugadora (sujeta o no)
//  8  vehiculo rapido, victima jugadora (ruta aritmetica completa)                                                    9  vehiculo lento (<= 0.0025 o NaN): sujeta / jugadora / evento
//  10 tren y atropello del peaton quieto                                                                            11 danador de tipo vehiculo con m_bAlreadyHitByCar puesto, y tipos sin rama
//  12 ruido de pasamontanas: edificio u objeto, temporizadores (y entidad fisica)                                 13 ruido de pasamontanas con objeto fisico (bit 20h, giro 0.04, temporizador B)   14..15 mezcla al azar
inline C MakeC(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.family = f;
    const auto R = [&](u32 k) { return Rand(n, k); };
    c.eax = R(70); c.edx = R(71);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    constexpr u32 kBoolBytes[]{0, 1, 2, 0x80, 0xFF, 0x7F, 0x10};
    c.hitByCar = R(2) % 4 ? 0u : kBoolBytes[R(3) % std::size(kBoolBytes)];
    c.dmgBits = F(RandF(n, 4, 0.05f, 60.0f));
    c.dmgNull = 0;
    c.kind = 1 + R(5) % 4;
    c.typeNoise = R(6) & 0xF8u;
    for (int i = 0; i < 3; ++i) c.e0[i] = F(RandF(n, 7 + i, -4.0f, 4.0f));
    for (int i = 0; i < 3; ++i) c.ec[i] = F(RandF(n, 10 + i, -300.0f, 300.0f));
    c.piece = R(13) % 3 ? R(14) % 20 : R(15) & 0xFFFFu;
    c.attachKind = 0; c.attachType = R(16) & 0xFFu;
    c.flags46C = R(17) & ~0x1u; c.entFlags = R(18) & ~0x10u; c.standingOn = 0;
    c.massBits = F(RandF(n, 19, 1.0f, 2000.0f)); c.shiftX = F(RandF(n, 20, -1.0f, 1.0f)); c.shiftY = F(RandF(n, 21, -1.0f, 1.0f));
    c.hasPlayer = R(22) % 2;
    c.pedMatNull = R(23) % 4 == 0;
    for (int i = 0; i < 3; ++i) { c.pos[i] = F(RandF(n, 24 + i, -300.0f, 300.0f)); c.place[i] = R(27 + i); }
    for (int i = 0; i < 9; ++i) c.vmRows[i] = F(RandF(n, 30 + i, -1.0f, 1.0f));
    c.pedType = R(40) % 40; c.contactKind = R(41) % 4;
    c.taskNull = R(42) % 3 == 0; c.isGoto = R(43) % 3 ? RawBool(n, 44, true) : 0u; c.taskMove = R(48) % 4 ? R(49) % 10 : R(50);
    c.dFlags1C = R(51) & ~0x40004u; c.dPhys40 = R(52) & ~0x24u; c.dTurnZ = F(RandF(n, 53, -1.0f, 1.0f));
    for (int i = 0; i < 3; ++i) c.dSpeed[i] = F(RandF(n, 54 + i, -1.0f, 1.0f));
    c.dSubtype = R(57) % 12; c.dTow = R(58) % 2 ? 0u : R(59) | 1u; c.dAttKind = R(60) % 4; c.dPedType = R(61) % 40; c.dDamNull = R(62) % 2;
    {
        float ax[9]; RotAxes(n, 63, ax);
        for (int i = 0; i < 9; ++i) c.dVm[i] = F(ax[i]);
        for (int i = 0; i < 3; ++i) { c.dVpos[i] = F(RandF(n, 66 + i, -300.0f, 300.0f)); c.dVplace[i] = R(69 + i); }
    }
    for (int i = 0; i < 3; ++i) { c.boxMin[i] = F(RandF(n, 80 + i, -4.0f, -0.5f)); c.boxMax[i] = F(RandF(n, 83 + i, 0.5f, 4.0f)); }
    c.headingBits = F(RandF(n, 86, -3.2f, 3.2f)); c.limitedBits = F(RandF(n, 87, -3.2f, 3.2f));
    c.isPlayerV = RawBool(n, 88, R(92) % 2); c.isPlayerD = RawBool(n, 93, R(97) % 2);
    c.hitSide = R(98) % 4 ? R(99) % 8 : R(100); c.affects = RawBool(n, 101, R(105) % 2); c.bala = RawBool(n, 106, R(110) % 2);
    c.localDir = R(111) % 4 ? R(112) % 8 : R(113);
    c.acq[0] = R(114) % 3 ? 1u << (R(115) % 8) : R(116); c.acq[1] = R(117) % 3 ? 1u << (R(118) % 8) : R(119);
    c.pedFlag[0] = R(120) % 3 ? 1u << (R(121) % 8) : R(122); c.pedFlag[1] = R(123) % 3 ? 1u << (R(124) % 8) : R(125);
    c.move[0] = R(126) % 4 ? R(127) % 10 : R(128); c.move[1] = R(129) % 4 ? R(130) % 10 : R(131);
    c.now = R(132) % 3 ? 100000u + R(133) % 100000u : R(134);
    c.lastA = R(135) % 2 ? c.now - 1000u - (R(136) % 3) + 1u : (R(137) % 3 ? c.now - R(138) % 5000u : R(139));
    c.lastB = R(140) % 2 ? c.now - 2000u - (R(141) % 3) + 1u : (R(142) % 3 ? c.now - R(143) % 5000u : R(144));
    if (R(145) % 4 == 0) { c.lastA = 0; c.lastB = 0; }

    const auto pickSpeed = [&](u32 k, u32 cls) {   // cuadrado de la velocidad: 0 -> <= 1e-4 ; 1 -> (1e-4, 0.0025] ; 2 -> (0.0025, 0.01] ; 3 -> > 0.01 ; 4 -> alrededor de 0.0025 ; 5 -> especiales
        float len = 0.0f;
        switch (cls) {
        case 0: len = RandF(n, k, 0.0f, 0.0099f); break;
        case 1: len = RandF(n, k, 0.0105f, 0.0495f); break;
        case 2: len = RandF(n, k, 0.0505f, 0.0995f); break;
        case 3: len = RandF(n, k, 0.11f, 2.5f); break;
        case 4: len = std::sqrt(0.0025f * (1.0f + float(int(R(k + 1) % 7) - 3) * 1e-6f)); break;
        default: len = 0.3f; break;
        }
        const float a1 = RandF(n, k + 2, 0.0f, 6.2831853f), a2 = RandF(n, k + 3, 0.0f, 3.1415926f);
        c.dSpeed[0] = F(len * std::sin(a2) * std::cos(a1)); c.dSpeed[1] = F(len * std::sin(a2) * std::sin(a1)); c.dSpeed[2] = F(len * std::cos(a2));
        if (cls == 4 && R(k + 4) % 2) { c.dSpeed[0] = F(len); c.dSpeed[1] = 0; c.dSpeed[2] = 0; }
        if (cls == 5) { const u32 w = R(k + 5) % 3; c.dSpeed[w] = SpecialBits(n, k + 6); }
    };
    switch (f) {
    case 0: {
        const u32 w = R(150) % 8;
        c.dmgNull = w < 2; c.kind = R(151) % 8;
        if (w == 2) c.dmgBits = 0; else if (w == 3) c.dmgBits = 0x80000000u; else if (w == 4) c.dmgBits = 0x7FC00000u; else if (w == 5) c.dmgBits = 0xFFC00000u;
        else if (w == 6) c.dmgBits = F(1.0e-30f);
        if (c.kind == 2) { pickSpeed(152, R(156) % 4); }
        c.hitByCar = kBoolBytes[R(157) % std::size(kBoolBytes)];
        break;
    }
    case 1: {
        c.kind = 1; c.hasPlayer = R(150) % 2;
        if (R(151) % 3 == 0) c.dmgBits = F(RandF(n, 152, 0.0f, 8.0f));
        c.piece = R(153) & 0xFFFFu;
        break;
    }
    case 2: {
        c.kind = 4;
        const u32 w = R(150) % 6;
        c.flags46C |= (R(151) % 4) ? 1u : 0u;
        c.hasPlayer = R(152) % 2;
        const float lim = c.hasPlayer ? 2.0f : 1.0f;
        if (w == 0) c.dmgBits = F(lim); else if (w == 1) c.dmgBits = F(std::nextafter(lim, 10.0f)); else if (w == 2) c.dmgBits = F(std::nextafter(lim, 0.0f));
        else if (w == 3) c.dmgBits = F(RandF(n, 153, 0.0f, 5.0f));
        c.dFlags1C |= R(154) % 5 == 0 ? 4u : (R(155) % 6 == 0 ? 0x40000u : 0u);
        c.contactKind = R(156) % 4;
        c.dAttKind = R(157) % 4;
        c.massBits = F(RandF(n, 158, 0.0f, 3.0f)); c.shiftX = F(0.0f); c.shiftY = F(0.0f);   // producto escalar nulo: el danio queda como esta
        break;
    }
    case 3: {
        c.kind = 4; c.flags46C |= 1u; c.hasPlayer = R(150) % 2;
        const u32 w = R(151) % 8;
        c.massBits = F(RandF(n, 152, 0.0f, 2000.0f)); c.dmgBits = F(RandF(n, 153, 0.0f, 10.0f));
        if (w == 0) c.dmgBits = F(RandF(n, 154, 0.0f, 1.0e5f)); else if (w == 1) c.dmgBits = F(1.0e18f * float(1 + R(155) % 20)); else if (w == 2) c.dmgBits = SpecialBits(n, 156);
        else if (w == 3) c.dmgBits = F(RandF(n, 157, -5.0f, 5.0f));
        else if (w == 4) c.dmgBits = F(RandF(n, 158, 2147483000.0f, 2.2e9f));
        for (int i = 0; i < 2; ++i) c.e0[i] = F(RandF(n, 160 + i, -4.0f, 4.0f));
        c.shiftX = F(RandF(n, 162, -1.0f, 1.0f)); c.shiftY = F(RandF(n, 163, -1.0f, 1.0f));
        if (R(164) % 5 == 0) c.shiftX = SpecialBits(n, 165);
        c.contactKind = R(166) % 5 == 0 ? 1u : 0u;
        c.dAttKind = R(167) % 4;
        c.dFlags1C |= R(168) % 8 == 0 ? 4u : 0u;
        c.localDir = R(169) % 3 ? R(170) % 8 : R(171);
        break;
    }
    case 4: {
        c.kind = 3; c.isPlayerV = RawBool(n, 150, true); c.isPlayerD = RawBool(n, 155, R(159) % 2);
        c.dDamNull = R(160) % 3 != 0;
        c.hasPlayer = R(161) % 2;
        break;
    }
    case 5: {
        c.kind = 3; c.isPlayerV = 0; c.isPlayerD = RawBool(n, 150, true);
        c.dDamNull = R(160) % 3 != 0;
        c.acq[0] = R(162) % 2 ? c.pedFlag[0] | R(163) : c.pedFlag[0] & ~R(163); c.acq[1] = R(164) % 2 ? c.pedFlag[1] | R(165) : c.pedFlag[1] & ~R(165);
        break;
    }
    case 6: {
        c.kind = 3; c.isPlayerV = 0; c.isPlayerD = 0;
        c.dDamNull = R(160) % 3 != 0;
        if (R(161) % 4 == 0) c.isPlayerD = RawBool(n, 162, true);
        if (R(166) % 4 == 0) c.isPlayerV = RawBool(n, 167, true);
        break;
    }
    case 7: {
        c.kind = 2; c.hitByCar = R(150) % 5 == 0 ? 1u : 0u;
        pickSpeed(151, R(155) % 2 ? 3u : (R(156) % 2 ? 2u : 4u));
        c.isPlayerV = 0;
        c.attachKind = R(157) % 2; c.attachType = R(162) % 2 ? (2u | (R(158) & 0xF8u)) : (R(158) & 0xFFu);
        c.flags46C |= (R(159) % 2) ? 1u : 0u;
        c.entFlags = R(160) % 2 ? c.entFlags | 0x10u : c.entFlags;
        c.massBits = F(RandF(n, 161, 1.0f, 2000.0f));
        break;
    }
    case 8: {
        c.kind = 2; c.hitByCar = 0;
        c.isPlayerV = RawBool(n, 150, true);
        pickSpeed(151, R(155) % 8 == 0 ? 5u : (R(156) % 3 ? 3u : 2u));
        c.flags46C |= (R(157) % 2) ? 1u : 0u;
        c.dmgBits = F(RandF(n, 158, 0.0f, 40.0f));
        const float bmin[3]{FromBits(c.boxMin[0]), FromBits(c.boxMin[1]), FromBits(c.boxMin[2])}, bmax[3]{FromBits(c.boxMax[0]), FromBits(c.boxMax[1]), FromBits(c.boxMax[2])};
        const float box = std::atan2(bmax[0] - bmin[0], bmax[1] - bmin[1]);
        const u32 w = R(159) % 10;
        float rel;
        switch (w) {
        case 0: rel = box * (1.0f + float(int(R(160) % 5) - 2) * 1e-6f); break;
        case 1: rel = -box * (1.0f + float(int(R(160) % 5) - 2) * 1e-6f); break;
        case 2: rel = (3.1415927f - box) * (1.0f + float(int(R(160) % 5) - 2) * 1e-6f); break;
        case 3: rel = -(3.1415927f - box) * (1.0f + float(int(R(160) % 5) - 2) * 1e-6f); break;
        case 4: rel = 0.0f; break;
        case 5: rel = RandF(n, 161, 0.0f, 3.1415927f); break;
        case 6: rel = RandF(n, 162, -3.1415927f, 0.0f); break;
        case 7: rel = box * RandF(n, 163, 0.0f, 3.2f / box); break;
        default: rel = RandF(n, 164, -3.2f, 3.2f); break;
        }
        c.limitedBits = F(rel);
        if (R(165) % 12 == 0) c.limitedBits = SpecialBits(n, 166);
        if (R(167) % 12 == 0) c.headingBits = SpecialBits(n, 168);
        // posicion de la victima cerca del vehiculo
        for (int i = 0; i < 3; ++i) c.pos[i] = F(FromBits(c.dVpos[i]) + RandF(n, 170 + i, -6.0f, 6.0f));
        break;
    }
    case 9: {
        c.kind = 2; c.hitByCar = 0;
        pickSpeed(151, R(155) % 8 == 0 ? 5u : (R(156) % 2 ? 4u : (R(157) % 2 ? 0u : 1u)));
        c.attachKind = R(158) % 2; c.attachType = R(166) % 2 ? (2u | (R(159) & 0xF8u)) : (R(159) & 0xFFu);
        c.isPlayerV = RawBool(n, 160, R(164) % 2);
        c.flags46C |= (R(165) % 2) ? 1u : 0u;
        break;
    }
    case 10: {
        c.kind = 2; c.hitByCar = 0;
        c.dSubtype = R(150) % 3 ? 6u : R(151) % 12; c.dPhys40 |= R(152) % 3 ? 4u : 0u;
        c.entFlags |= R(153) % 4 ? 0x10u : 0u; c.flags46C |= R(154) % 4 ? 1u : 0u; c.standingOn = R(155) % 4 == 0 ? R(156) | 1u : 0u;
        const float tiny[]{0.0f, 0.00005f, 0.0001f, 0.0001001f, 0.00009f, 0.001f};
        const float len = R(157) % 3 ? tiny[R(158) % std::size(tiny)] : RandF(n, 159, 0.0f, 0.02f);
        c.dSpeed[0] = F(len); c.dSpeed[1] = 0; c.dSpeed[2] = 0;
        if (R(160) % 6 == 0) c.dSpeed[0] = SpecialBits(n, 161);
        c.isPlayerV = RawBool(n, 162, R(166) % 2);
        c.attachKind = R(167) % 2; c.attachType = R(169) % 3 ? (2u | (R(168) & 0xF8u)) : (R(168) & 0xFFu);
        break;
    }
    case 11: {
        c.kind = R(150) % 3 ? 2u : 3u + R(151) % 4;
        if (R(152) % 3 == 0) c.kind = R(153) % 8;
        c.hitByCar = R(154) % 2 ? kBoolBytes[R(155) % std::size(kBoolBytes)] : 1u;
        pickSpeed(156, R(160) % 4);
        c.isPlayerV = RawBool(n, 161, R(165) % 2);
        break;
    }
    case 12: {
        c.kind = R(150) % 2 ? 1u : 4u; c.hasPlayer = R(151) % 5 != 0;
        c.dPhys40 = (c.dPhys40 & ~0x20u) | (R(152) % 2 ? 0x20u : 0u);
        c.dTurnZ = R(153) % 3 ? F(RandF(n, 154, -0.1f, 0.1f)) : (R(155) % 2 ? F(0.04f) : F(std::nextafter(0.04f, 1.0f)));
        if (R(156) % 10 == 0) c.dTurnZ = SpecialBits(n, 157);
        c.dmgBits = R(158) % 3 ? F(RandF(n, 159, 0.5f, 5.0f)) : (R(160) % 2 ? F(1.0f) : (R(161) % 2 ? F(3.0f) : F(std::nextafter(3.0f, 9.0f))));
        c.bala = RawBool(n, 162, R(166) % 4 != 0);
        c.flags46C &= ~1u;   // para el objeto: sin producto escalar ni golpe fuerte
        break;
    }
    case 13: {
        c.kind = R(150) % 5 ? 4u : 1u; c.hasPlayer = R(151) % 8 != 0;
        c.dPhys40 = (c.dPhys40 & ~0x20u) | (R(152) % 5 ? 0x20u : 0u);
        c.dTurnZ = R(153) % 3 ? F(RandF(n, 154, 0.03f, 0.2f) * (R(155) % 2 ? 1.0f : -1.0f)) : (R(156) % 2 ? F(0.04f) : F(std::nextafter(0.04f, 1.0f)));
        if (R(157) % 12 == 0) c.dTurnZ = SpecialBits(n, 158);
        c.dmgBits = R(159) % 2 ? F(RandF(n, 160, 0.5f, 5.0f)) : (R(161) % 2 ? F(1.0f) : (R(162) % 2 ? F(3.0f) : F(std::nextafter(3.0f, 9.0f))));
        c.bala = RawBool(n, 163, R(167) % 6 != 0);
        c.flags46C &= ~1u;
        c.lastB = R(168) % 2 ? c.now - 2001u - (R(169) % 2) : (R(170) % 2 ? c.now - 2000u : c.now - R(171) % 4000u);
        c.lastA = R(172) % 2 ? c.now - 1001u - (R(173) % 2) : (R(174) % 2 ? c.now - 1000u : c.now - R(175) % 2000u);
        break;
    }
    default: {
        c.kind = R(150) % 3 == 0 ? 1 + R(151) % 4 : (R(152) % 2 ? 2u : 3u + R(153) % 2);
        if (R(154) % 10 == 0) c.kind = R(155) % 8;
        pickSpeed(156, R(160) % 6);
        c.isPlayerV = RawBool(n, 161, R(165) % 3 == 0); c.isPlayerD = RawBool(n, 166, R(170) % 3 == 0);
        c.flags46C |= R(171) % 2; c.dDamNull = R(172) % 2;
        c.attachKind = R(173) % 3 == 0; c.attachType = R(179) % 3 ? (2u | (R(174) & 0xF8u)) : (R(174) & 0xFFu);
        c.entFlags |= (R(175) % 2) ? 0x10u : 0u;
        const float bmin[3]{FromBits(c.boxMin[0]), FromBits(c.boxMin[1]), FromBits(c.boxMin[2])}, bmax[3]{FromBits(c.boxMax[0]), FromBits(c.boxMax[1]), FromBits(c.boxMax[2])};
        const float box = std::atan2(bmax[0] - bmin[0], bmax[1] - bmin[1]);
        if (R(176) % 3 == 0) c.limitedBits = F(box * RandF(n, 177, 0.5f, 1.5f));
        if (R(178) % 3 == 0) for (int i = 0; i < 3; ++i) c.pos[i] = F(FromBits(c.dVpos[i]) + RandF(n, 179 + i, -6.0f, 6.0f));
        break;
    }
    }
    // Con la matriz de la victima nula, la comprobacion contra el adelante del peaton (0x6048BE) leeria [0x10] sin matriz: el cuadrado de la velocidad se limita a (0.0025, 0.01]
    if (c.kind == 2 && c.pedMatNull) pickSpeed(151, 2);
    return c;
}

// Constantes de las arenas que usa el banco: se vuelcan en la fila `begin` y run_local.py las compara con las del oraculo (misma suma a mano en ambos lados).
inline const auto& ArenaTable() {
    static const std::vector<std::pair<const char*, u32>> t{
        {"kCsOff", kCsOff}, {"kCsBytes", kCsBytes}, {"kCsArena", kCsArena}, {"kVOff", kVOff}, {"kVBytes", kVBytes}, {"kVArena", kVArena}, {"kVmOff", kVmOff}, {"kVmBytes", kVmBytes}, {"kVmArena", kVmArena},
        {"kIOff", kIOff}, {"kIBytes", kIBytes}, {"kIArena", kIArena}, {"kTOff", kTOff}, {"kTBytes", kTBytes}, {"kTArena", kTArena}, {"kDOff", kDOff}, {"kDBytes", kDBytes}, {"kDArena", kDArena},
        {"kDmOff", kDmOff}, {"kDmBytes", kDmBytes}, {"kDmArena", kDmArena}, {"kDiOff", kDiOff}, {"kDiBytes", kDiBytes}, {"kDiArena", kDiArena}, {"kColOff", kColOff}, {"kColBytes", kColBytes}, {"kColArena", kColArena},
        {"kPdOff", kPdOff}, {"kPdBytes", kPdBytes}, {"kPdArena", kPdArena}, {"kClOff", kClOff}, {"kClBytes", kClBytes}, {"kClArena", kClArena}, {"kAOff", kAOff}, {"kABytes", kABytes}, {"kAArena", kAArena},
        {"kGOff", kGOff}, {"kGBytes", kGBytes}, {"kGArena", kGArena},
    };
    return t;
}
}  // namespace CollisionScan61Fix

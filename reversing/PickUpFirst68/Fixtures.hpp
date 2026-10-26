#pragma once
// Fixtures del banco PickUpFirst68 (lote 68): CTaskComplexGoPickUpEntity::CreateFirstSubTask (0x693610, virtual, vtable 0x870B98 ranura 9, thiscall con 1 argumento (ped), `ret 4`,
// devuelve CTask* en EAX; marco SEH propio), 16384 casos = 16 familias x 1024. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
// Resumen del desensamblado (0x693610..0x6939CB): ESI = this, EDI = entidad = [this+0xC]; sin entidad -> 0 (no escribe nada).
//   [this+0x28] = CTimer::m_snTimeInMilliseconds (0xB7CB84). d = ped - entidad (posicion de la matriz +0x30 si la hay, si no del placement +4), guardado en floats (dz, dy, dx).
//   caja = ms_modelInfoPtrs[movsx([ent+0x22])]->[+0x14] (min +0, max +0xC); [this+0x10..0x18] = (-0.2f, -min.y - 0.2f, -min.z).
//   2*max.y < max.x -> SetHeading(atan2(-dx, dy)); si no 2*max.x < max.y -> SetHeading(atan2(-dx, dy) - pi/2 (0x858FE4)); si no GetMatrix y, si up.z < 0.9f (0x858C20), SetHeading(GetHeading()).
//   SetHeading en linea: con matriz CMatrix::SetRotateZOnly (0x59B020), sin matriz [ent+0x10] = rumbo. Objetos (tipo 4 en [ent+0x36]&7): velocidades [ent+0x44] y [ent+0x50] = 0 y SetIsStatic(true) (vtable +0x10).
//   Con [ent+0x18] (objeto RW) no nulo: CMatrix::UpdateRwMatrix (0x59AD70, con matriz) o CSimpleTransform::UpdateRwMatrix (0x54EF40) sobre [[ent+0x18]+4]+0x10.
//   Producto escalar derecha = (dz*r.z + dy*r.y) + dx*r.x, frente = (dz*f.z + dy*f.y) + dx*f.x (la matriz se pide antes de cada uno: AllocateMatrix 0x54F560 + UpdateMatrix 0x54F1B0 si falta).
//   derecha > |frente| -> pickup = (0.4f + max.x, 0); -|frente| > derecha -> (min.x - 0.4f, 0); si no (0, frente > 0 ? 0.4f + max.y : min.y - 0.4f); pickup.z = min.z + 1.0f.
//   punto = matriz * pickup (0x59C890), CTask::operator new(0x28) (0x61A5A0) y, si no es nulo, CTaskComplexGoToPointAndStandStill(4, punto, 0.2f, 0, false, true) (0x668120).
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace PickUp68Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline u32 FloatBits(float v) { u32 b; std::memcpy(&b, &v, 4); return b; }
inline float BitsFloat(u32 b) { float v; std::memcpy(&v, &b, 4); return v; }

// ------------------------------------------------------------------------------------------------ disposicion de memoria (guarda | objeto | guarda)
constexpr u32 kGuard = 0x40;
constexpr u32 kTaskBytes = 0x34, kTaskArena = kGuard + kTaskBytes + kGuard;                        // 0x40 + 0x34 + 0x40 = 0xB4   CTaskComplexGoPickUpEntity
constexpr u32 kEntBytes = 0x60, kEntArena = kGuard + kEntBytes + kGuard;                            // 0x40 + 0x60 + 0x40 = 0xE0   entidad (lee hasta +0x36, escribe +0x44..+0x5C)
constexpr u32 kPedBytes = 0x20, kPedArena = kGuard + kPedBytes + kGuard;                            // 0x40 + 0x20 + 0x40 = 0xA0   ped (solo se lee +4..+0x14)
constexpr u32 kG2 = 0x20;
constexpr u32 kMatBytes = 0x48, kMatArena = kG2 + kMatBytes + kG2;                                  // 0x20 + 0x48 + 0x20 = 0x88   matrices
constexpr u32 kMiBytes = 0x20, kMiArena = kG2 + kMiBytes + kG2;                                     // 0x20 + 0x20 + 0x20 = 0x60   CBaseModelInfo falso ([+0x14] = CColModel*)
constexpr u32 kColBytes = 0x18, kColArena = kG2 + kColBytes + kG2;                                  // 0x20 + 0x18 + 0x20 = 0x58   CColModel falso (caja en +0..+0x17)
constexpr u32 kRwBytes = 0x10, kRwArena = kG2 + kRwBytes + kG2;                                     // 0x20 + 0x10 + 0x20 = 0x50   objeto RW falso ([+4] = marco)
constexpr u32 kFrameBytes = 0x80, kFrameArena = kG2 + kFrameBytes + kG2;                            // 0x20 + 0x80 + 0x20 = 0xC0   marco RW falso (solo su direccion)
constexpr u32 kAllocBytes = 0x28, kAllocArena = kG2 + kAllocBytes + kG2;                            // 0x20 + 0x28 + 0x20 = 0x68   objeto que "reserva" el falso de operator new
static_assert(kTaskArena == 0xB4 && kEntArena == 0xE0 && kPedArena == 0xA0 && kMatArena == 0x88 && kMiArena == 0x60 && kColArena == 0x58 && kRwArena == 0x50 && kFrameArena == 0xC0 && kAllocArena == 0x68, "arenas");
// campos de la tarea
constexpr u32 kEntityAt = 0xC, kPosAt = 0x10, kPickupAt = 0x1C, kTimeFieldAt = 0x28;
// campos de la entidad / peaton
constexpr u32 kPlacementAt = 4, kHeadingAt = 0x10, kMatrixAt = 0x14, kRwAt = 0x18, kModelAt = 0x22, kFlagsAt = 0x36, kMoveAt = 0x44, kTurnAt = 0x50;
constexpr u32 kColAtMi = 0x14, kParentAtRw = 4;
constexpr u32 kTimeAddr = 0xB7CB84;
constexpr u32 kModelPtrs = 0xA9B0C8, kModelCount = 20000;
constexpr u32 kMaxEvents = 24;
constexpr u32 kVtSlots = 16;
constexpr u32 kSizeGoTo = 0x28;
// Valores centinela de los campos que la funcion escribe (para que el banco vea exactamente que bytes cambian)
constexpr u32 kSentPos = 0xA5A5A5A5u, kSentPickup = 0x5A5A5A5Au, kSentTime = 0xC3C3C3C3u, kSentHeading = 0x69696969u, kSentSpeed = 0x96969696u;

// ------------------------------------------------------------------------------------------------ caso
struct C {
    u32 family, entNull, entMat, flags, modelIdx, rw, pedMat;
    u32 ent[3], entAlt[3], ped[3], pedAlt[3];     // posicion activa (matriz o placement) y la de la otra fuente
    u32 box[6];                                   // min xyz, max xyz
    u32 right[3], fwd[3], up[3], aRight[3], aFwd[3], aUp[3];   // matriz propia de la entidad / matriz que reserva el falso de AllocateMatrix
    u32 target[3];
    u32 timeMs, newNull, fill;
    u32 eax, edx, stack;
};

inline constexpr u32 kNanF = 0x7FC00000u, kInfF = 0x7F800000u, kNegInfF = 0xFF800000u, kZeroF = 0u, kNegZeroF = 0x80000000u;
inline constexpr u32 kSpecialF[]{kZeroF, kNegZeroF, 0x3F800000u, 0xBF800000u, 0x3F666666u, 0x3F666665u, 0x3F666667u, 0x3ECCCCCDu, 0x3E4CCCCDu, 0x40000000u, 0x358637BDu, 0x3F000000u, 0xBF000000u};
inline constexpr u32 kHeadings[]{0x00000000u, 0x80000000u, 0x40490FDBu, 0xC0490FDBu, 0x3FC90FDBu, 0x40C90FDBu, 0x3F800000u, 0x41200000u, 0x42C80000u, 0xC2480000u, 0x3A83126Fu, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0x4B189680u};
inline constexpr u32 kTimes[]{0u, 0xFFFFFFFFu, 0xFFFF0000u, 0xFFFE795Fu, 0xFFFE7960u, 100000u, 0x80000000u};
inline constexpr u32 kModels[]{0u, 1u, 2u, 3u, 7u, 100u, 400u, 1000u, 4000u, 9999u, 19999u, 0x3FFFu, 0x1234u, 0x3FFFu / 2u};
template <class A> inline u32 Pick(const A& a, u32 n, u32 k) { return a[Rand(n, k) % std::size(a)]; }
// Coordenada "normal": signo y escala al azar de 0.01 a 1e5
inline u32 NormalF(u32 n, u32 k) {
    static const float scales[]{0.01f, 0.5f, 1.0f, 3.0f, 10.0f, 100.0f, 1000.0f, 1.0e5f};
    const float s = scales[Rand(n, k) % std::size(scales)];
    const float u = float(Rand(n, k + 1) % 2000001u) / 1000000.0f - 1.0f;   // [-1, 1]
    return FloatBits(u * s);
}
inline u32 UnitF(u32 n, u32 k) { return FloatBits(float(Rand(n, k) % 2000001u) / 1000000.0f - 1.0f); }   // [-1, 1]
inline u32 PosF(u32 n, u32 k, float lo, float hi) { return FloatBits(lo + (hi - lo) * float(Rand(n, k) % 1000001u) / 1000000.0f); }

// Familias (n % 16, 1024 casos cada una):
//  0 sin entidad                               1 rumbo (2*max.y < max.x), con matriz           2 igual, sin matriz (placement)
//  3 rumbo - pi/2 (2*max.x < max.y), con matriz  4 igual, sin matriz                           5 up.z < 0.9 con matriz (GetHeading + SetHeading)
//  6 up.z < 0.9 sin matriz (se reserva)          7 up.z >= 0.9 con matriz                        8 up.z >= 0.9 sin matriz
//  9 bordes de la caja (2*max.y = max.x y vecinos, ceros, NaN, inf)    10 ped en la direccion derecha de la entidad (derecha > |frente|)
//  11 ped en la direccion contraria (-|frente| > derecha)              12 ped delante / detras (rama final, signo del frente, ceros con signo)
//  13 NaN / inf / enormes / minimos en matrices y posiciones            14 operator new devuelve nulo          15 mezcla al azar de todos los parametros
inline C MakeC(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.family = f;
    c.eax = Rand(n, 70);
    c.edx = Rand(n, 71);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    c.entNull = f == 0;
    c.entMat = Rand(n, 1) % 2;
    c.pedMat = Rand(n, 2) % 2;
    c.rw = Rand(n, 3) % 4 != 0;
    // byte de banderas [ent+0x36]: tipo en los 3 bits bajos (4 = objeto), bits altos al azar
    c.flags = Rand(n, 4) % 2 ? (4u | (Rand(n, 5) & 0xF8u)) : (Rand(n, 6) & 0xFFu);
    c.modelIdx = Rand(n, 7) % 3 == 0 ? Pick(kModels, n, 8) : Rand(n, 9) % kModelCount;
    for (u32 k = 0; k < 3; ++k) {
        c.ent[k] = NormalF(n, 10 + 2 * k);
        c.entAlt[k] = NormalF(n, 16 + 2 * k);
        c.pedAlt[k] = NormalF(n, 28 + 2 * k);
        c.right[k] = UnitF(n, 34 + k);
        c.fwd[k] = UnitF(n, 38 + k);
        c.up[k] = UnitF(n, 42 + k);
        c.aRight[k] = UnitF(n, 46 + k);
        c.aFwd[k] = UnitF(n, 50 + k);
        c.aUp[k] = UnitF(n, 54 + k);
        c.target[k] = Rand(n, 58 + k) % 6 == 0 ? Pick(kSpecialF, n, 62 + k) : NormalF(n, 66 + 2 * k);
    }
    // caja: min negativo, max positivo; relacion max.x / max.y al azar en [0.3, 3.3] salvo que la familia la fuerce
    {
        const float my = 0.1f + 20.0f * float(Rand(n, 80) % 1000u) / 1000.0f;
        const float ratio = 0.3f + 3.0f * float(Rand(n, 81) % 1000u) / 1000.0f;
        c.box[3] = FloatBits(my * ratio);
        c.box[4] = FloatBits(my);
        c.box[5] = PosF(n, 82, 0.1f, 5.0f);
        c.box[0] = FloatBits(-(0.1f + 20.0f * float(Rand(n, 83) % 1000u) / 1000.0f));
        c.box[1] = FloatBits(-(0.1f + 20.0f * float(Rand(n, 84) % 1000u) / 1000.0f));
        c.box[2] = FloatBits(-(0.1f + 5.0f * float(Rand(n, 85) % 1000u) / 1000.0f));
        if (Rand(n, 86) % 12 == 0) c.box[Rand(n, 87) % 3] = Pick(kSpecialF, n, 88);   // minimos especiales
    }
    c.timeMs = Rand(n, 93) % 5 == 0 ? Pick(kTimes, n, 94) : Rand(n, 95);
    c.newNull = Rand(n, 96) % 10 == 0;
    c.fill = 1u + Rand(n, 97) % 255u;
    // posicion del ped: la de la entidad mas un desplazamiento (a veces cero)
    {
        const u32 r = Rand(n, 100) % 10;
        for (u32 k = 0; k < 3; ++k) {
            const float base = BitsFloat(c.ent[k]);
            const float off = r == 0 ? 0.0f : BitsFloat(NormalF(n, 101 + 2 * k)) * 0.1f;
            c.ped[k] = FloatBits(base + off);
        }
        if (Rand(n, 108) % 8 == 0) { c.ped[0] = c.ent[0]; c.ped[1] = c.ent[1]; c.ped[2] = c.ent[2]; }   // coincidentes: d = 0
        if (Rand(n, 109) % 16 == 0) { c.ped[Rand(n, 110) % 3] = Pick(kSpecialF, n, 111); }
    }
    const auto setRatio = [&](float ratio) { c.box[3] = FloatBits(BitsFloat(c.box[4]) * ratio); };   // max.x = ratio * max.y
    const float u01 = float(Rand(n, 121) % 1000u) / 1000.0f;
    const u32 upMode = Rand(n, 120) % 10;
    switch (f) {
    case 1: case 2: setRatio(2.01f + 3.0f * u01); c.entMat = f == 1; break;
    case 3: case 4: setRatio(0.49f - 0.19f * u01); c.entMat = f == 3; break;
    case 5: case 6: case 7: case 8: {
        setRatio(0.55f + 1.4f * u01);   // ni A ni B
        c.entMat = (f == 5 || f == 7);
        const bool low = f == 5 || f == 6;
        u32* up = c.entMat ? c.up : c.aUp;   // la matriz que se lee: la propia (con matriz) o la reservada (sin matriz)
        up[2] = low ? (upMode < 6 ? PosF(n, 122, -1.0f, 0.8999f) : upMode == 6 ? 0x3F666665u : upMode == 7 ? 0xBF800000u : upMode == 8 ? 0x80000000u : 0x00000000u)
                    : (upMode < 5 ? PosF(n, 122, 0.9f, 1.0f) : upMode == 5 ? 0x3F666666u : upMode == 6 ? 0x3F800000u : upMode == 7 ? kNanF : upMode == 8 ? kInfF : 0x3F666667u);
        break;
    }
    case 9: {
        const u32 kind = Rand(n, 123) % 12;
        const float my = BitsFloat(c.box[4]);
        switch (kind) {
        case 0: c.box[3] = FloatBits(my + my); break;                                  // 2*max.y == max.x (no es menor)
        case 1: c.box[3] = FloatBits(my + my) + 1u; break;
        case 2: c.box[3] = FloatBits(my + my) - 1u; break;
        case 3: c.box[4] = FloatBits(BitsFloat(c.box[3]) + BitsFloat(c.box[3])); break;  // 2*max.x == max.y
        case 4: c.box[4] = FloatBits(BitsFloat(c.box[3]) * 2.0f) + 1u; break;
        case 5: c.box[4] = FloatBits(BitsFloat(c.box[3]) * 2.0f) - 1u; break;
        case 6: c.box[3] = kZeroF; c.box[4] = Rand(n, 124) % 2 ? kZeroF : kNegZeroF; break;
        case 7: c.box[3] = kNanF; break;
        case 8: c.box[4] = kNanF; break;
        case 9: c.box[3] = kInfF; break;
        case 10: c.box[4] = kInfF; c.box[3] = Rand(n, 125) % 2 ? kInfF : kNegInfF; break;
        default: c.box[3] = kZeroF; c.box[4] = FloatBits(0.5f); break;
        }
        break;
    }
    case 10: case 11: case 12: {
        // el ped se coloca respecto a la entidad a lo largo de una combinacion de los ejes de la matriz que se lee
        setRatio(0.55f + 1.4f * u01);   // rama de la matriz
        if (f == 12 && Rand(n, 126) % 3 == 0) {
            for (u32 k = 0; k < 3; ++k) c.ped[k] = c.ent[k];
            if (Rand(n, 127) % 2) c.ped[Rand(n, 128) % 3] = Rand(n, 129) % 2 ? kZeroF : kNegZeroF;
        } else {
            const float s = std::fabs(BitsFloat(NormalF(n, 130))) * 5.0f + 1.0f;
            const u32* r = c.entMat ? c.right : c.aRight;
            const u32* fw = c.entMat ? c.fwd : c.aFwd;
            const float a = f == 10 ? 3.0f : f == 11 ? -3.0f : 0.1f * float(int(Rand(n, 131) % 21) - 10);
            const float b = f == 12 ? 3.0f * (Rand(n, 132) % 2 ? 1.0f : -1.0f) : 0.2f * float(int(Rand(n, 133) % 11) - 5);
            for (u32 k = 0; k < 3; ++k) c.ped[k] = FloatBits(BitsFloat(c.ent[k]) + (a * BitsFloat(r[k]) + b * BitsFloat(fw[k])) * s);
        }
        break;
    }
    case 13: {
        const u32 kind = Rand(n, 135) % 18;
        u32* m0 = c.entMat ? c.right : c.aRight;
        u32* m1 = c.entMat ? c.fwd : c.aFwd;
        switch (kind) {
        case 0: m0[Rand(n, 136) % 3] = kNanF; break;
        case 1: m1[Rand(n, 137) % 3] = kNanF; break;
        case 2: c.ped[Rand(n, 138) % 3] = kNanF; break;
        case 3: { const u32 k = Rand(n, 139) % 3; c.ent[k] = kNanF; c.entAlt[k] = kNanF; } break;
        case 4: m0[Rand(n, 140) % 3] = Rand(n, 141) % 2 ? kInfF : kNegInfF; break;
        case 5: m1[Rand(n, 142) % 3] = Rand(n, 143) % 2 ? kInfF : kNegInfF; break;
        case 6: c.ped[Rand(n, 144) % 3] = Rand(n, 145) % 2 ? kInfF : kNegInfF; break;
        case 7: for (u32 k = 0; k < 3; ++k) { m0[k] = FloatBits(1.0e30f); m1[k] = FloatBits(-1.0e30f); } break;
        case 8: for (u32 k = 0; k < 3; ++k) { c.ped[k] = FloatBits(3.0e38f); c.ent[k] = FloatBits(-3.0e38f); c.entAlt[k] = c.ent[k]; } break;
        case 9: for (u32 k = 0; k < 3; ++k) { m0[k] = FloatBits(1.0e-20f); m1[k] = FloatBits(1.0e-20f); } break;      // productos diminutos
        case 10: for (u32 k = 0; k < 3; ++k) { m0[k] = 0x00800000u; m1[k] = 0x80800000u; } break;                   // minimos normales
        case 11: for (u32 k = 0; k < 3; ++k) { m0[k] = 0x007FFFFFu; m1[k] = 0x807FFFFFu; } break;                   // subnormales
        case 12: for (u32 k = 0; k < 3; ++k) { m0[k] = kZeroF; m1[k] = Rand(n, 146) % 2 ? kZeroF : kNegZeroF; } break;
        case 13: for (u32 k = 0; k < 3; ++k) c.ped[k] = FloatBits(BitsFloat(c.ent[k]) + 1.0e-30f); break;
        default: {   // 14..17: posiciones diminutas (2^-27..2^-7) y matrices con exponentes de 2^-126 a 2^-97: los productos caen en el rango subnormal y por debajo del menor float
            const auto tiny = [&](u32 k, u32 lo, u32 hi) { return ((Rand(n, k) & 1u) << 31) | ((lo + Rand(n, k + 1) % (hi - lo + 1u)) << 23) | (Rand(n, k + 2) & 0x7FFFFFu); };
            for (u32 k = 0; k < 3; ++k) {
                c.ent[k] = tiny(160 + 3 * k, 100, 120);
                c.ped[k] = kind >= 16 ? c.ent[k] + 1u + Rand(n, 175 + k) % 64u : tiny(170 + 3 * k, 100, 120);   // 16 y 17: a pocos ulp de la entidad (deltas de 2^-35..2^-50)
                c.entAlt[k] = c.ent[k];
                m0[k] = tiny(180 + 3 * k, 1, kind == 17 ? 20 : 40);
                m1[k] = tiny(190 + 3 * k, 1, kind == 16 ? 12 : 40);
            }
            break;
        }
        }
        setRatio(0.55f + 1.4f * u01);
        break;
    }
    case 14: c.newNull = 1; break;
    default: break;
    }
    c.pedAlt[0] = Rand(n, 150) % 2 ? c.pedAlt[0] : c.ped[0] ^ 0x1u;   // la fuente no activa lleva un valor distinto
    return c;
}
}  // namespace PickUp68Fix

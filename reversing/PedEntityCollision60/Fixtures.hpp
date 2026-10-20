#pragma once
// Fixtures del banco PedEntityCollision60: CPed::ProcessEntityCollision (0x5E2530), hook VIRTUAL, thiscall, 2 argumentos de pila (CEntity*, CColPoint*), `ret 8`, devuelve EAX = numero de puntos.
// Solo C++ estandar (sin tipos del juego): las incluyen LiveComparison.hpp/Core.hpp y dump_fixtures.cpp (autoprueba del generador y de los tamanos de arena).
//
// La funcion es enorme (4.342 bytes, SEH, ~20 colaboradores). Aqui el EXE original (hook apagado) es el oraculo: se ejecutan los mismos bytes con la misma memoria falsa que la DLL (hook encendido).
// Cada objeto que la funcion toca (ped, entidad, matrices, colmodel/datos de colision del ped, puntos de contacto, tarea de arma, ...) es una ARENA con guardas rellenas de basura determinista
// (guarda | ventana | guarda); los punteros entre objetos se resuelven al montar el caso. Las salidas de ProcessColModels, IsSteepSlope, etc. las dicta el caso a traves de falsos identicos en ambos lados.
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace PedEntityCollision60Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kFamilies = 24;
constexpr unsigned kPerFamily = 256;
constexpr unsigned kCases = kFamilies * kPerFamily;   // 6144

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
inline u32 F2U(float f) { return std::bit_cast<u32>(f); }
inline float U2F(u32 u) { return std::bit_cast<float>(u); }

// ------------------------------------------------------------------------------------------------ disposicion de memoria (guarda | objeto | guarda)
// La suma esta escrita a mano y la comprueba dump_fixtures.cpp (static_assert) y Core.hpp.
constexpr u32 kPedBytes = 0x79C, kPedOff = 0x40, kPedArena = 0x81C;          // 0x40 + 0x79C + 0x40 (CPed completo)
constexpr u32 kEntBytes = 0x598, kEntOff = 0x20, kEntArena = 0x5D8;          // 0x20 + 0x598 + 0x20 (entidad: hasta +0x594)
constexpr u32 kMatBytes = 0x48, kMatOff = 0x20, kMatArena = 0x88;            // 0x20 + 0x48 + 0x20 (matriz del ped y de la entidad)
constexpr u32 kCpCount = 33;                                                 // 32 puntos + el extra que la funcion escribe en el indice numCPs
constexpr u32 kCpBytes = kCpCount * 0x2C, kCpOff = 0x40, kCpArena = 0x62C;   // 0x40 + 0x5AC + 0x40 (puntos de contacto)
constexpr u32 kMiBytes = 0x18, kMiOff = 0x20, kMiArena = 0x58;               // 0x20 + 0x18 + 0x20 (CBaseModelInfo falso: +0x14 colmodel)
constexpr u32 kCmBytes = 0x30, kCmOff = 0x20, kCmArena = 0x70;               // 0x20 + 0x30 + 0x20 (colmodel del ped / copia del temporal 0x968E20 / colmodel de la entidad)
constexpr u32 kCdBytes = 0x18, kCdOff = 0x20, kCdArena = 0x58;               // 0x20 + 0x18 + 0x20 (CCollisionData: lineas, esferas)
constexpr u32 kLnBytes = 0x40, kLnOff = 0x20, kLnArena = 0x80;               // 0x20 + 0x40 + 0x20 (dos CColLine de 0x20)
constexpr u32 kSpBytes = 0x50, kSpOff = 0x20, kSpArena = 0x90;               // 0x20 + 0x50 + 0x20 (cuatro CColSphere de 0x14)
constexpr u32 kTaskBytes = 0x34, kTaskOff = 0x20, kTaskArena = 0x74;         // 0x20 + 0x34 + 0x20 (CTaskSimpleUseGun: +0x30 = arma)
constexpr u32 kWiBytes = 0x1C, kWiOff = 0x20, kWiArena = 0x5C;               // 0x20 + 0x1C + 0x20 (CWeaponInfo: +0x18 = flags)
constexpr u32 kTokBytes = 0x10, kTokOff = 0x10, kTokArena = 0x30;            // 0x10 + 0x10 + 0x10 (objetos opacos: inteligencia, clump, asociacion de animacion)

// globales que el banco fija por caso y restaura
constexpr u32 kGTimestep = 0xB7CB5C, kGTimeMs = 0xB7CB84, kGMix = 0x8D12C0, kGBright = 0x8D21E4;
constexpr u32 kGModelInfo = 0xA9B0C8;                     // ms_modelInfoPtrs
constexpr u32 kGTempCm = 0x968E20;                        // CTempColModels (colmodel del ped para bTestForBlockedPositions): 0x30 bytes; +0x2C = datos
constexpr u32 kGCamActive = 0xB6F081;                     // TheCamera.m_nActiveCam
constexpr u32 kGCamMode = 0xB6F1A8, kGCamFront = 0xB6F32C, kGCamStride = 0x238;   // modo (word) y front.x/front.y de cada CCam
constexpr u32 kGSurface = 0xB79538;                       // g_surfaceInfos
constexpr u32 kModelSlots = 4;                            // indices de modelo 0..3

// ------------------------------------------------------------------------------------------------ etiquetas
enum : u32 {
    L_NULL = 0, L_PED, L_ENT, L_MATPED, L_MATENT, L_CP, L_MI, L_PCM, L_TCM, L_ECM, L_CD, L_LN, L_SP, L_TASK, L_WI, L_INTEL, L_CLUMP, L_ASSOC,
    L_COUNT
};
constexpr u32 kTokBase = 0xFD000000u;
constexpr u32 Tok(u32 label) { return kTokBase | label; }
constexpr u32 kAnyPtr = 0x00123450u;      // puntero no nulo que no es ninguno de los objetos (solo se compara)

// ------------------------------------------------------------------------------------------------ caso
struct Case {
    // --- ped
    u32 modelIdx{};
    u32 pedF1C{}, pedPhys{}, ped46C{}, ped470{}, ped474{}, ped478{};
    u32 pedType{}, pedState{};
    u32 attachedSel{}, ignoredSel{}, vehSel{};          // 0 nulo, 1 entidad, 2 otro puntero
    u32 f588{}, speed[3]{}, bright{}, pedPos[3]{}, pedHeading{};
    u32 aimSel{};                                       // 0 GetTaskUseGun nulo, 1 tarea sin arma, 2 tarea con arma
    u32 winfoFlags{};
    u32 camActive{}, camMode[3]{}, camFront[6]{};
    // --- entidad
    u32 entF1C{}, entType{}, entPhys{}, entVType{}, entSub{}, entSpeed[3]{}, entBright{}, entMatNull{}, entMatPos[3]{};
    u32 entPlHeading{}, entPlPos[3]{};
    // --- esferas / datos de colision del ped
    u32 sph[3][5]{};                                    // tres esferas: x y z radio (+ superficie)
    u32 line[2][8]{};                                   // dos lineas: (valores previos; la funcion reescribe z)
    u32 useTempCm{};                                    // derivado: bTestForBlockedPositions && usesCollision (solo informativo)
    // --- salidas de los falsos
    u32 n1{}, n2{};
    u32 touch0{}, touch1{}, w0{}, w1{};                 // maxTouch que escribe la 1a llamada (w = 1 si lo escribe)
    u32 cpSeed{}, cpPedZ{};                             // semilla de los puntos y z de referencia
    u32 lpz[2]{};                                       // z de los puntos de linea (bits)
    u32 steepSeed{}, softSeed{}, assocSel{}, localDir{}, affects[2]{}, hdgBits{}, junk{}, resp[4]{};
    // --- globales
    u32 timestep{}, timeMs{}, gMix{}, gBright{};
    // --- ABI
    u32 eax{}, edx{}, stack{};
    unsigned family{}, index{};
};

// valores float especiales para el caos
inline constexpr std::array<u32, 20> kSpecialF{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0x40000000u, 0x3727C5ACu, 0xB727C5ACu, 0x7149F2CAu, 0xF149F2CAu,
                                               0x7F7FFFFFu, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC12345u, 0x00000001u, 0x00400000u, 0x80000001u, 0x3DCCCCCDu, 0xBE19999Au};

inline float Fq(u32 r, int lo, int hi, float step) { return float(int(r % u32(hi - lo + 1)) + lo) * step; }   // valores exactos (multiplos de step)

// -------- puntos de contacto (0x2C bytes): se generan a demanda, sin guardarlos
// set 0/1: puntos del primer/segundo ProcessColModels (idx = indice); set 2/3: puntos de linea 0/1 (idx 0)
inline u32 NormalZ(u32 r, u32 mode) {
    // z de la normal: cerca de los umbrales -0.867 (0xBF5DF3B6) y -0.99 (0xBF7D70A4)
    static constexpr std::array<u32, 14> t{0xBF800000u, 0xBF7D70A4u, 0xBF7D70A5u, 0xBF7D70A3u, 0xBF7FF972u, 0xBF5DF3B6u, 0xBF5DF3B7u, 0xBF5DF3B5u, 0xBF000000u, 0x00000000u, 0x3F000000u, 0x3F3504F3u, 0x3F800000u, 0xBF59999Au};
    if (mode == 1) return kSpecialF[r % kSpecialF.size()];
    return t[r % t.size()];
}
inline void MakeCp(const Case& c, unsigned set, unsigned idx, u8* out) {
    const u32 s = c.cpSeed + set * 7919u + idx * 104729u;
    auto R = [&](u32 k) { return Rand(s, k); };
    std::memset(out, 0, 0x2C);
    const float pz = U2F(c.cpPedZ);
    const bool chaos = (R(1) % 23) == 0;
    // punto: z relativa a la z del ped (para activar los umbrales de +1.0 / +3.0)
    u32 zbits;
    if (set >= 2) zbits = c.lpz[set - 2];
    else zbits = F2U(pz + Fq(R(2), -8, 8, 0.25f));
    u32 px = F2U(Fq(R(3), -400, 400, 0.25f)), py = F2U(Fq(R(4), -400, 400, 0.25f));
    if (chaos) { px = kSpecialF[R(5) % kSpecialF.size()]; zbits = kSpecialF[R(6) % kSpecialF.size()]; }
    // normal: a menudo casi unitaria
    u32 nx, ny, nz;
    switch (R(7) % 9) {
    case 0: nx = F2U(0.f); ny = F2U(0.f); nz = F2U(1.f); break;
    case 1: nx = F2U(0.f); ny = F2U(0.f); nz = F2U(-1.f); break;
    case 2: nx = F2U(1.f); ny = F2U(0.f); nz = F2U(0.f); break;
    case 3: nx = F2U(-0.6f); ny = F2U(0.8f); nz = F2U(0.f); break;
    case 4: nx = F2U(0.1f); ny = F2U(-0.1f); nz = NormalZ(R(8), 0); break;
    case 5: nx = F2U(0.f); ny = F2U(0.f); nz = NormalZ(R(8), 0); break;
    case 6: nx = F2U(Fq(R(9), -4, 4, 0.25f)); ny = F2U(Fq(R(10), -4, 4, 0.25f)); nz = NormalZ(R(8), 0); break;
    case 7: nx = kSpecialF[R(9) % kSpecialF.size()]; ny = kSpecialF[R(10) % kSpecialF.size()]; nz = NormalZ(R(8), chaos ? 1 : 0); break;
    default: nx = R(9); ny = R(10); nz = R(11); break;
    }
    if (set >= 2 && (R(12) & 1)) nz = NormalZ(R(8), 0);
    const u32 surfA = R(13) & 0xFF, surfB = (R(14) % 5 == 0) ? (R(15) & 0xFF) : (R(14) % 64);
    // pieza A: la funcion distingue 2, >2, 4, 5, 6, 8
    static constexpr std::array<u32, 12> pieces{0, 1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 0xFF};
    const u32 pieceA = pieces[R(16) % pieces.size()];
    std::memcpy(out + 0x00, &px, 4); std::memcpy(out + 0x04, &py, 4); std::memcpy(out + 0x08, &zbits, 4);
    const u32 f0c = R(17), f1c = R(18);
    std::memcpy(out + 0x0C, &f0c, 4);
    std::memcpy(out + 0x10, &nx, 4); std::memcpy(out + 0x14, &ny, 4); std::memcpy(out + 0x18, &nz, 4);
    std::memcpy(out + 0x1C, &f1c, 4);
    out[0x20] = u8(surfA); out[0x21] = u8(pieceA); out[0x22] = u8(R(19)); out[0x23] = u8(surfB); out[0x24] = u8(R(20)); out[0x25] = u8(R(21));
    out[0x26] = u8(R(22)); out[0x27] = u8(R(23));
    const u32 depth = F2U(Fq(R(24), 0, 64, 0.0625f));
    std::memcpy(out + 0x28, &depth, 4);
}

// -------- generador
inline Case MakeCase(unsigned n) {
    Case c{};
    c.family = n % kFamilies;
    c.index = n / kFamilies;
    const unsigned fam = c.family;
    auto R = [&](u32 k) { return Rand(n, k); };
    auto P = [&](u32 k, u32 pct) { return (R(k) % 100) < pct; };         // verdadero con probabilidad pct %
    auto special = [&](u32 k) { return kSpecialF[R(k) % kSpecialF.size()]; };

    // ----- perfiles por familia (porcentajes)
    u32 pTest = 12, pUses = 92, pPlayer = 30, pLines = 85, pCeil = 25, pWasStanding = 20, pStanding = 30, pTouch0 = 60, pTouch1 = 25;
    u32 pStuck = 15, pHead = 15, pWall = 12, pFall = 50, pBoat = 10, pEntNullMat = 15, pAim = 30, pStatic = 30, pNegz = 15, pTrain = 12, pChaos = 3, pPhys0 = 12, pPhys8 = 40;
    u32 pFilterPieces = 40, pExtra = 12, pAnim = 50, pAffects = 80, pSteep = 30, pSoft = 40, pPedEnt = 10;
    switch (fam) {
    case 0: pTouch0 = 95; pStanding = 5; pWasStanding = 5; pLines = 100; pTest = 0; break;
    case 1: pTouch0 = 95; pStanding = 90; pLines = 100; pTest = 0; break;
    case 2: pTouch0 = 95; pPlayer = 90; pStanding = 10; pLines = 100; pTest = 0; break;
    case 3: pTouch0 = 85; pTouch1 = 90; pCeil = 100; pLines = 100; pTest = 0; break;
    case 4: pTouch0 = 95; pHead = 100; pLines = 100; pStanding = 30; pTest = 0; break;
    case 5: pTouch0 = 95; pWall = 90; pLines = 100; pTest = 0; break;
    case 6: pTouch0 = 95; pWasStanding = 0; pFall = 95; pLines = 100; pStanding = 10; pTest = 0; break;
    case 7: pTouch0 = 95; pWasStanding = 0; pFall = 5; pAnim = 95; pLines = 100; pTest = 0; break;
    case 8: pTouch0 = 95; pLines = 100; pPlayer = 70; pWasStanding = 5; pTest = 0; break;
    case 9: pTouch0 = 8; pCeil = 80; pTouch1 = 80; pLines = 100; pStanding = 60; pTest = 0; break;
    case 10: pLines = 5; pTest = 90; pFilterPieces = 95; break;
    case 11: pTest = 100; pPlayer = 95; pAim = 95; pUses = 100; break;
    case 12: pTest = 100; pLines = 20; pFilterPieces = 100; pPedEnt = 30; break;
    case 13: pTest = 30; pTrain = 90; pLines = 40; break;
    case 14: pStatic = 95; pWasStanding = 95; pPlayer = 70; pLines = 30; break;
    case 15: pHead = 70; pTouch0 = 90; pLines = 100; pExtra = 80; pTest = 0; break;
    case 16: pLines = 40; pStatic = 60; break;
    case 17: pUses = 25; pPhys0 = 30; break;
    case 18: pChaos = 45; break;
    case 19: pBoat = 90; pTouch0 = 85; pStanding = 90; pLines = 100; pTest = 0; break;
    case 20: pStuck = 70; pLines = 90; pEntNullMat = 70; break;
    case 21: pPhys0 = 60; pLines = 60; pPedEnt = 40; break;
    case 22: pWasStanding = 60; pStanding = 60; pStatic = 70; break;
    default: break;
    }
    const bool chaos = P(1, pChaos);

    // ----- ped
    c.modelIdx = R(2) % kModelSlots;
    u32 f1C = R(3);
    f1C = P(4, pUses) ? (f1C | 1u) : (f1C & ~1u);
    f1C = P(5, pStuck) ? (f1C | 0x10u) : (f1C & ~0x10u);
    f1C = P(6, 50) ? (f1C | 2u) : (f1C & ~2u);
    f1C |= 0x20000000u & R(7);
    c.pedF1C = f1C;
    u32 phys = R(8) & ~0x19000u & ~0x10000u;
    if (P(9, pPhys0)) phys |= (R(10) % 3 == 0) ? 0x10000u : (R(10) % 3 == 1 ? 0x9000u : 0x1000u);
    if (P(11, 15)) phys |= 0x8u;
    c.pedPhys = phys;
    u32 f46C = R(12), f470 = R(13), f474 = R(14), f478 = R(15);
    f46C = P(16, pStanding) ? (f46C | 1u) : (f46C & ~1u);
    f46C = P(17, pWasStanding) ? (f46C | 2u) : (f46C & ~2u);
    f470 = P(18, pHead) ? (f470 | (1u << 21)) : (f470 & ~(1u << 21));
    f470 = P(19, 25) ? (f470 | (1u << 27)) : (f470 & ~(1u << 27));
    f470 = P(20, 40) ? (f470 | 0x82u) : (f470 & ~0x82u);          // bOnBoat (1), bTryingToReachDryLand (7) previos
    f470 = P(21, 30) ? (f470 | 0x20u) : (f470 & ~0x20u);
    f474 = P(22, pTest) ? (f474 | 0x8000u) : (f474 & ~0x8000u);
    f478 = P(23, pCeil) ? (f478 | 0x100u) : (f478 & ~0x100u);
    c.ped46C = f46C; c.ped470 = f470; c.ped474 = f474; c.ped478 = f478;
    c.useTempCm = ((f474 & 0x8000u) && (f1C & 1u)) ? 1 : 0;
    c.pedType = P(24, pPlayer) ? (R(25) & 1) : (R(25) % 24);
    if (P(26, 4)) c.pedType = 0xFFFFFFFFu;
    c.pedState = P(27, 25) ? 1u : (R(28) % 60);
    c.attachedSel = P(29, 8) ? 2u : 0u;
    c.ignoredSel = P(30, 15) ? (R(31) % 2 ? 1u : 2u) : 0u;
    c.vehSel = P(32, 15) ? (R(33) % 2 ? 1u : 2u) : 0u;
    // 0x588: sin suelo (99999.992), o un valor relativo a la z del ped, o especial
    const float pedZ = Fq(R(34), -2048, 2048, 0.125f);
    c.pedPos[0] = F2U(Fq(R(35), -400, 400, 0.25f)); c.pedPos[1] = F2U(Fq(R(36), -400, 400, 0.25f)); c.pedPos[2] = F2U(pedZ);
    c.cpPedZ = F2U(pedZ);
    switch (R(37) % 6) {
    case 0: case 1: case 2: c.f588 = 0x47C34FFFu; break;
    case 3: c.f588 = F2U(pedZ + Fq(R(38), -16, 16, 0.25f)); break;
    case 4: c.f588 = F2U(pedZ + Fq(R(38), -4, 8, 0.125f)); break;
    default: c.f588 = special(38); break;
    }
    // velocidad: a menudo pequena; a veces rapida (dano) o hacia abajo
    {
        const u32 m = R(39) % 8;
        float vx = 0, vy = 0, vz = 0;
        if (m == 0) { vx = 0; vy = 0; vz = 0; }
        else if (m == 1) { vx = Fq(R(40), -16, 16, 0.03125f); vy = Fq(R(41), -16, 16, 0.03125f); vz = Fq(R(42), -16, 4, 0.03125f); }
        else if (m == 2) { vx = Fq(R(40), -4, 4, 0.0625f); vy = Fq(R(41), -4, 4, 0.0625f); vz = Fq(R(42), -48, 8, 0.0125f); }
        else if (m == 3) { vx = Fq(R(40), -2, 2, 0.01f); vy = Fq(R(41), -2, 2, 0.01f); vz = Fq(R(42), -40, 40, 0.0125f); }
        else if (m == 4) { vx = Fq(R(40), -1, 1, 0.005f); vy = Fq(R(41), -1, 1, 0.005f); vz = -0.3f * float(R(42) % 4); }
        else if (m == 5) { vx = Fq(R(40), -400, 400, 0.001f); vy = Fq(R(41), -400, 400, 0.001f); vz = Fq(R(42), -1000, 1000, 0.001f); }
        else if (m == 6) { vx = 0.1f; vy = (R(40) & 1) ? 0.01f : -0.01f; vz = (R(41) & 1) ? 0.01f : -0.01f; }
        else { vx = 0.33f; vy = 0.f; vz = (R(40) & 1) ? -0.25f : -0.375f; }
        c.speed[0] = F2U(vx); c.speed[1] = F2U(vy); c.speed[2] = F2U(vz);
        if (chaos) { c.speed[R(43) % 3] = special(44); }
    }
    c.bright = F2U(Fq(R(45), 0, 64, 0.0625f));
    if (chaos && P(46, 50)) c.bright = special(47);
    c.pedHeading = F2U(Fq(R(48), -50, 50, 0.125f));
    // ----- tarea de arma / camara
    c.aimSel = P(49, pAim) ? (P(50, 85) ? 2u : 1u) : 0u;
    c.winfoFlags = P(51, 70) ? (R(52) | 2u) : (R(52) & ~2u);
    c.camActive = R(53) % 3;
    for (int i = 0; i < 3; ++i) c.camMode[i] = P(54 + i, 70) ? (0x35u | (R(60 + i) & 0xFFFF0000u)) : (R(57 + i) & 0xFFFFu);
    for (int i = 0; i < 6; ++i) c.camFront[i] = P(63 + i, 85) ? F2U(Fq(R(70 + i), -8, 8, 0.25f)) : special(76 + i);
    // ----- entidad
    {
        const u32 tsel = R(80) % 100;
        u32 type;
        if (tsel < 20) type = 1; else if (tsel < 45) type = 2; else if (tsel < 45 + pPedEnt) type = 3; else if (tsel < 80) type = 4; else if (tsel < 88) type = 5; else type = (R(81) % 3 == 0) ? 0 : (R(81) % 3 == 1 ? 6 : 7);
        c.entType = type | (R(82) & 0xF8u);
    }
    u32 ef = R(83);
    ef = P(84, pStatic) ? (ef | 4u) : (ef & ~4u);
    ef = P(85, 15) ? (ef | 0x40000u) : (ef & ~0x40000u);
    ef = P(86, 50) ? (ef | 1u) : (ef & ~1u);
    c.entF1C = ef;
    c.entPhys = (R(87) & ~0xCu) | (P(88, 40) ? 4u : 0u) | (P(89, pPhys8) ? 8u : 0u);
    c.entVType = P(90, pBoat) ? 5u : (R(91) % 9);
    c.entSub = P(92, pTrain) ? 6u : (R(93) % 9);
    c.entSpeed[0] = F2U(Fq(R(94), -16, 16, 0.0625f)); c.entSpeed[1] = F2U(Fq(R(95), -16, 16, 0.0625f)); c.entSpeed[2] = F2U(Fq(R(96), -16, 16, 0.0625f));
    c.entBright = F2U(Fq(R(97), 0, 64, 0.125f));
    c.entMatNull = P(98, pEntNullMat) ? 1u : 0u;
    c.entMatPos[0] = F2U(Fq(R(99), -400, 400, 0.25f)); c.entMatPos[1] = F2U(Fq(R(100), -400, 400, 0.25f)); c.entMatPos[2] = F2U(pedZ + Fq(R(101), -16, 16, 0.25f));
    c.entPlHeading = F2U(0.f);   // UpdateMatrix real: rumbo 0 para que seno/coseno (x87/SSE) sean exactos
    c.entPlPos[0] = c.entMatPos[0]; c.entPlPos[1] = c.entMatPos[1]; c.entPlPos[2] = c.entMatPos[2];
    // ----- esferas y lineas del colmodel del ped (k10 = r2 + z2; esfera 0: z - r)
    for (int i = 0; i < 3; ++i) {
        c.sph[i][0] = F2U(Fq(R(110 + i * 5), -4, 4, 0.125f)); c.sph[i][1] = F2U(Fq(R(111 + i * 5), -4, 4, 0.125f));
        c.sph[i][2] = F2U(Fq(R(112 + i * 5), -12, 12, 0.125f)); c.sph[i][3] = F2U(Fq(R(113 + i * 5), 0, 12, 0.125f)); c.sph[i][4] = R(114 + i * 5);
        if (chaos && P(130 + i, 30)) { c.sph[i][2 + (R(133 + i) & 1)] = special(136 + i); }
    }
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 8; ++j) c.line[i][j] = P(140 + i * 8 + j, 50) ? F2U(Fq(R(160 + i * 8 + j), -8, 8, 0.25f)) : special(180 + i * 8 + j);
    // ----- salidas de ProcessColModels
    auto count = [&](u32 k, u32 zeroPct) -> u32 {
        const u32 q = R(k) % 100;
        if (q < zeroPct) return 0;
        if (q < zeroPct + 40) return 1 + R(k + 1) % 4;
        if (q < zeroPct + 55) return 4 + R(k + 1) % 8;
        if (q < zeroPct + 70) return 28 + R(k + 1) % 5;
        return 1 + R(k + 1) % 32;
    };
    c.n1 = count(200, P(202, 20) ? 100 : 12);
    c.n2 = count(203, 25);
    // la z del punto de linea: relativa a la z del ped (umbrales z + 1.0 y z + 3.0); delta exactos
    static constexpr std::array<float, 14> deltas{-4.f, -2.0f, -1.5f, -1.0f, -0.5f, -0.25f, -0.125f, 0.f, 0.125f, 0.25f, 0.5f, 1.0f, 2.0f, 8.f};
    {
        const float base = pedZ - 1.0f;    // lp.z + 1.0 == pos.z cuando delta == 0
        c.lpz[0] = F2U(base + deltas[R(210) % deltas.size()]);
        c.lpz[1] = F2U(U2F(c.f588 == 0x47C34FFFu ? F2U(pedZ + 2.0f) : c.f588) + deltas[R(211) % deltas.size()]);
        if (c.family == 3 || c.family == 9) c.lpz[1] = F2U(pedZ + Fq(R(212), -12, 16, 0.25f));
        if (chaos) { c.lpz[R(213) & 1] = special(214); }
    }
    static constexpr std::array<u32, 11> touches{0x00000000u, 0x3F000000u, 0x3F7FFFFFu, 0x3F800000u, 0x3F800001u, 0x3FC00000u, 0xBDCCCCCDu, 0x7FC00000u, 0x7F800000u, 0x00000001u, 0x3E4CCCCDu};
    c.touch0 = P(220, pTouch0) ? ((R(221) % 4 == 0) ? touches[R(222) % touches.size()] : F2U(0.125f * float(1 + R(222) % 7))) : 0x3F800000u;
    c.touch1 = P(223, pTouch1) ? ((R(224) % 4 == 0) ? touches[R(225) % touches.size()] : F2U(0.125f * float(1 + R(225) % 7))) : 0x3F800000u;
    c.w0 = c.touch0 != 0x3F800000u || P(226, 30);
    c.w1 = c.touch1 != 0x3F800000u || P(227, 30);
    c.cpSeed = R(230);
    c.steepSeed = P(231, pSteep) ? R(232) | 1u : 0u;
    c.softSeed = P(233, pSoft) ? R(234) | 1u : 0u;
    c.assocSel = P(235, pAnim) ? 1u : 0u;
    c.localDir = (R(236) & ~0xFFu) | (R(237) % 4);
    c.affects[0] = P(238, pAffects) ? 1 : 0; c.affects[1] = P(239, pAffects) ? 1 : 0;
    c.hdgBits = P(240, 85) ? F2U(Fq(R(241), -63, 63, 0.0625f)) : special(242);
    c.junk = R(243);
    for (int i = 0; i < 4; ++i) c.resp[i] = R(244 + i);
    // ----- globales
    {
        static constexpr std::array<u32, 12> ts{0x3F800000u, 0x3F800000u, 0x3F800000u, 0x3E4CCCCDu, 0x3F000000u, 0x40000000u, 0x41200000u, 0x00000000u, 0x80000000u, 0x7FC00000u, 0x7F800000u, 0x3C23D70Au};
        c.timestep = ts[R(250) % ts.size()];
        c.timeMs = R(251);
        c.gMix = P(252, 60) ? F2U(Fq(R(253), 0, 16, 0.0625f)) : special(254);
        c.gBright = P(255, 60) ? F2U(Fq(R(256), 0, 16, 0.0625f)) : special(257);
    }
    c.eax = R(260); c.edx = R(261); c.stack = StackSeed(n);
    return c;
}
} // namespace PedEntityCollision60Fix

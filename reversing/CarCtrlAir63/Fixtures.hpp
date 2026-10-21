#pragma once
// Fixtures del banco CarCtrlAir63 (lote 63, CCarCtrl parte A: helicopteros, aviones, lanchas y combate aereo). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp /
// LiveComparison.hpp y el programa autonomo de calibracion fixture_mirror.cpp.
//
// Cada caso (funcion, n) es DETERMINISTA: el generador rellena IMAGENES completas de los objetos (el vehiculo bajo prueba A, el objetivo T, el vehiculo del jugador P, tres matrices)
// con basura y luego fija los campos que la funcion lee. Los colaboradores son falsos identicos en ambos lados (EXE con hook apagado / DLL con solo ese hook encendido) que registran
// sus argumentos EN ORDEN y devuelven lo que dicta el caso (tablas por orden de llamada).
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

namespace A63Fix {
using i16 = std::int16_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFamilies = 12;

// ------------------------------------------------------------------------------------------------ funciones del banco (indices estables; ver kActive)
enum Fn : u32 {
    kHeliFlyDir = 0,        // GetAIHeliToFlyInDirection            0x42A730
    kDogAgainstPlayer,      // GetAIPlaneToDoDogFightAgainstPlayer  0x42F370
    kHeliCrash,             // SteerAIHeliToCrashAndBurn            0x4238E0
    kPlaneCrash,            // SteerAIPlaneToCrashAndBurn           0x423880
    kPlaneTowards,          // SteerAIPlaneTowardsTargetCoors       0x423790
    kPlaneFollow,           // SteerAIPlaneToFollowEntity           0x4237F0
    kHeliTowards,           // SteerAIHeliTowardsTargetCoors        0x42A630
    kHeliAway,              // SteerAIHeliFlyingAwayFromPlayer      0x42ACB0
    kHeliLand,              // SteerAIHeliToLand                    0x42AD30
    kHeliPolice,            // SteerAIHeliAsPoliceHeli              0x42AAD0
    kTestFire,              // TestWhetherToFirePlaneGuns           0x429520   (vehicle, entity)
    kDogMoves,              // TriggerDogFightMoves                 0x429300   (vehicle1, vehicle2)
    kPlaneAttack,           // GetAIPlaneToAttackPlayer             0x429780
    kPlaneDog,              // GetAIPlaneToDoDogFight               0x429890
    kHeliFollow,            // SteerAIHeliToFollowEntity            0x42A750
    kHeliView,              // SteerAIHeliToKeepEntityInView        0x42AEB0
    kHeliAttack,            // GetAIHeliToAttackPlayer              0x42F3C0
    kBoatHead,              // SteerAIBoatWithPhysicsHeadingForTarget 0x428BE0 (vehicle, float, float, float*, float*, float*)
    kBoatAttack,            // SteerAIBoatWithPhysicsAttackingPlayer  0x428DE0 (vehicle, float*, float*, float*, bool*)
    kBoatCircle,            // SteerAIBoatWithPhysicsCirclingPlayer   0x429090 (vehicle, float*, float*, float*, bool*)
    kRockets,               // FireHeliRocketsAtTarget                0x42B270 (vehicle, entity)
    kFixed,                 // FlyAIHeliToTarget_FixedOrientation      0x423940 (heli, float, CVector)
    kPlaneDir,              // FlyAIPlaneInCertainDirection            0x423000 (plane)
    kHeliDir,               // FlyAIHeliInCertainDirection             0x429A70 (heli, float, float, bool)
    kFnCount
};

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline u32 Bits(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
inline float Flt(u32 v) { float f; std::memcpy(&f, &v, 4); return f; }

// ------------------------------------------------------------------------------------------------ disposicion
constexpr u32 kObjBytes = 0xB00, kObjWords = kObjBytes / 4;   // CHeli mide 0xA18
constexpr u32 kMatWords = 24;
constexpr u32 kPedWords = 0x600 / 4;
constexpr u32 kGuardWords = 4;
// campos de la entidad (bytes)
constexpr u32 kOffPos = 0x04, kOffMatrix = 0x14, kOffRSeed = 0x20, kOffSpeed = 0x44, kOffHealth = 0x4C0, kOffCompress = 0x7D4, kOffForced = 0x954;
constexpr u32 kOffCarFlags = 0x3DB, kOffFollowMult = 0x3DA, kOffTargetDist = 0x3D8, kOffCreation = 0x430, kOffEntInfo = 0x36, kOffIntel = 0x47C;
constexpr u32 kOffCruise = 0x3D0, kOffModel = 0x22, kOffMaxAlt9A8 = 0x9A8, kOffAlt9AC = 0x9AC, kOffMinAlt9B0 = 0x9B0;
constexpr u32 kOffMission = 0x3BA, kOffTemp = 0x3BB, kOffTempTime = 0x3BC, kOffDest = 0x3EC, kOffTarget = 0x41C, kOffDogFlag = 0x425, kOffFlags42C = 0x42C, kOffWeapon = 0x513;
constexpr u32 kOffSkid = 0x98C, kOffUpDown = 0x990, kOffLeftRight = 0x994, kOffBrake = 0x998;   // CHeli
constexpr u32 kOffMaxAlt = 0x9A8, kOffMinAlt = 0x9B0, kOffHeading = 0x9B4, kOffA14 = 0xA14;
constexpr u32 kGarbageSeed = 0x6A09E667u;
inline u32 Garbage(u32 n, u32 k) {
    u32 v = 0xC0000000u | (Rand(n + kGarbageSeed, k) & 0x3FFFFFFFu);
    if ((v & 0x7F800000u) == 0x7F800000u) v |= 0x400000u;   // nunca un SNaN
    return v == 0xFFFFFFFFu ? 0xC0000000u : v;
}

struct Spec {
    u32 fn{}, family{}, n{}, seed{};
    u32 regs[4]{};                                  // semilla de pila, EAX, ECX, EDX de entrada
    u32 a[kObjWords]{}, t[kObjWords]{}, p[kObjWords]{};
    u32 mat[3][kMatWords]{};
    u32 hasMat[3]{1, 1, 1};                         // m_matrix != 0 de A, T y P
    u32 tgtKind{1};                                 // valor de A+0x41C: 0 nulo, 1 T, 2 P, 3 ped, 4 A
    u32 atan[8]{};                                  // GetATanOfXY: retorno por orden de llamada (bits de float)
    u32 fpv[8]{};                                   // FindPlayerVehicle: 0 nulo, 1 P, 2 T, 3 A
    u32 pc[8][3]{};                                 // FindPlayerCoors: vector por orden de llamada
    u32 fpe[4]{};                                   // FindPlayerEntity: 0 nulo, 1 T, 2 P, 3 A
    u32 task{};                                     // CTaskManager::GetActiveTask: 0 nulo, 1 tarea falsa
    u32 taskType{};                                 // GetTaskType de la tarea falsa
    u32 rnd[8]{};                                   // rand() del EXE (0x821B1E): retorno por orden de llamada
    u32 norm[4][3]{};                               // CVector::Normalise (0x59C910): vector que deja el falso, por orden de llamada
    u32 app[4]{};                                   // CVehicle::GetVehicleAppearance: retorno por orden de llamada
    u32 speedV[3]{};                                // vector que devuelve FindPlayerSpeed
    u32 arg2Kind{1};                                // segundo argumento de (vehiculo, entidad): 0 nulo, 1 T, 2 P, 3 A
    u32 timeMs{};                                   // CTimer::m_snTimeInMilliseconds
    u32 fargs[2]{};                                 // argumentos float por valor (HeadingForTarget)
    u32 prevMs{};                                   // CTimer::m_snPreviousTimeInMilliseconds
    u32 dline[2]{};                                 // CCollision::DistToMathematicalLine: retorno por orden de llamada (bits de float)
    u32 dv[3]{};                                    // argumento CVector por valor
    u32 los[8][3]{};                                // CWorld::ProcessLineOfSight: punto de choque por orden de llamada
    u32 losHit[8]{};                                // ... y si hay choque
    u32 probe[32]{};                                // funcion 0x422F80 del EXE: retorno (float) por orden de llamada
    u32 stale{};                                    // valor del local sin inicializar del EXE ([esp+0x10]) y de CcAirPlaneLimit
    u32 stepBits{};                                 // CTimer::ms_fTimeStep
    u32 key[4]{};                                   // campos clave del caso (solo para el registro)
};

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_) : n(n_) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
};

// Solo NaN silenciosos: la x87 resuelve un SNaN en memoria frente a un QNaN en ST(0) distinto de un QNaN frente a QNaN y depende del codigo exacto que emita el compilador;
// ningun calculo de coma flotante del juego produce un SNaN.
constexpr u32 kQNaN = 0x7FC00000u, kSNaN = 0x7FC00001u, kInf = 0x7F800000u, kNegInf = 0xFF800000u;
inline float NextUp(float f) { return std::nextafterf(f, std::numeric_limits<float>::infinity()); }
inline float NextDown(float f) { return std::nextafterf(f, -std::numeric_limits<float>::infinity()); }

// valor "raro" de un campo float: NaN (silencioso y senalizador), infinitos, -0.0, subnormales, enormes
inline u32 Odd(Gen& g) {
    switch (g.P(12)) {
    case 0: return kQNaN;
    case 1: return kSNaN;
    case 2: return kInf;
    case 3: return kNegInf;
    case 4: return 0x80000000u;
    case 5: return 1u + g.P(100);                      // subnormal
    case 6: return 0x80000001u + g.P(100);             // subnormal negativo
    case 7: return 0x7F000000u + g.P(0x7FFFFFu);       // enorme (~1e38)
    case 8: return 0xFF000000u + g.P(0x7FFFFFu);       // enorme negativo
    case 9: return 0xFFC00000u;                        // NaN negativo
    case 10: return 0x00800000u + g.P(1000);           // minimo normal
    default: return 0u;
    }
}
// umbral +- k ulp (k en -2..2), a veces negado
inline u32 NearThr(Gen& g, float thr) {
    float v = g.C(15) ? -thr : thr;
    const int k = int(g.P(5)) - 2;
    for (int i = 0; i < std::abs(k); ++i) v = k > 0 ? NextUp(v) : NextDown(v);
    return Bits(v);
}
// float "normal" en [-range, range]: enteros, cuartos o mantisa aleatoria
inline u32 Moderate(Gen& g, float range) {
    switch (g.P(3)) {
    case 0: return Bits(float(int(g.P(u32(2 * range) + 1)) - int(range)));
    case 1: return Bits(float(int(g.P(u32(8 * range) + 1)) - int(4 * range)) * 0.25f);
    default: return Bits((float(g.P(2000001)) / 1000000.0f - 1.0f) * range);
    }
}
// coordenada con el estilo de la familia
inline u32 Coord(Gen& g, u32 fam) {
    switch (fam) {
    case 0: return Moderate(g, 2000.0f);
    case 1: return Bits(float(int(g.P(4001)) - 2000) / 7.0f);
    case 2: return Bits(float(int(g.P(121)) - 60));
    case 3: return Bits(float(int(g.P(5)) - 2));
    case 4: return g.C(30) ? (g.C(50) ? kQNaN : kSNaN) : Moderate(g, 100.0f);
    case 5: return g.C(35) ? (g.C(50) ? kInf : kNegInf) : Moderate(g, 100.0f);
    case 6: return Bits((g.C(50) ? 1.0f : -1.0f) * std::ldexp(1.0f + float(g.P(1000)) / 1000.0f, int(g.P(66)) + 60));   // ~1e18 .. 4e37
    case 7: return g.C(50) ? (1u + g.P(0x7FFFFF)) : (0x80000001u + g.P(0x7FFFFF));   // subnormales
    case 8: return g.C(50) ? 0x80000000u : (g.C(50) ? 0u : Moderate(g, 3.0f));
    case 9: return Bits(float(int(g.P(8)) * 1000000 + int(g.P(4)) - 2) + 16777216.0f);  // cerca de 2^24: la resta pierde bits
    case 10: return g.C(30) ? Odd(g) : Moderate(g, 5000.0f);
    default: return g.R();                                                                   // bits aleatorios
    }
}

inline void FillGarbage(Spec& s) {
    for (u32 i = 0; i < kObjWords; ++i) { s.a[i] = Garbage(s.n + s.fn * 0x20000u, i); s.t[i] = Garbage(s.n + s.fn * 0x20000u + 77u, i); s.p[i] = Garbage(s.n + s.fn * 0x20000u + 99u, i); }
    for (u32 m = 0; m < 3; ++m) for (u32 i = 0; i < kMatWords; ++i) s.mat[m][i] = Garbage(s.n + s.fn * 0x20000u + 123u + m, i);
    for (u32 i = 0; i < 8; ++i) s.atan[i] = Garbage(s.n + s.fn * 0x20000u + 5u, i);
}
inline u32& AW(Spec& s, u32 off) { return s.a[off / 4]; }
inline u8& AB(Spec& s, u32 off) { return reinterpret_cast<u8*>(s.a)[off]; }
inline u32& TW(Spec& s, u32 off) { return s.t[off / 4]; }
inline u32& PW(Spec& s, u32 off) { return s.p[off / 4]; }

// posicion de la entidad e (0 = A, 1 = T, 2 = P): la real va donde la lea el EXE (la matriz si hay, m_placement si no); el otro sitio lleva un senuelo
inline void SetPos(Spec& s, u32 who, const u32 v[3], Gen& g) {
    u32* img = who == 0 ? s.a : who == 1 ? s.t : s.p;
    for (int i = 0; i < 3; ++i) {
        const u32 decoy = g.C(50) ? Bits(1.0e6f + float(i)) : Garbage(s.n, 400 + u32(i) + who * 4);
        img[kOffPos / 4 + i] = s.hasMat[who] ? decoy : v[i];
        s.mat[who][12 + i] = s.hasMat[who] ? v[i] : decoy;
    }
}
inline void RandomAtan(Gen& g, Spec& s) {
    for (int i = 0; i < 8; ++i) {
        switch (g.P(10)) {
        case 0: s.atan[i] = Odd(g); break;
        case 1: s.atan[i] = Bits(3.14159274f); break;
        case 2: s.atan[i] = Bits(-3.14159274f); break;
        case 3: s.atan[i] = 0u; break;
        default: s.atan[i] = Bits((float(g.P(2000001)) / 1000000.0f - 1.0f) * 3.2f); break;
        }
    }
}
inline void RandomPc(Gen& g, Spec& s, u32 fam) {
    for (int c = 0; c < 8; ++c) for (int i = 0; i < 3; ++i) s.pc[c][i] = Coord(g, fam);
}

// destino a distancia horizontal exacta ~len de la posicion (triples pitagoricos x escala), con posibles perturbaciones de 1 ulp
inline void Triple(Gen& g, float len, float& ox, float& oy) {
    static const float t[][2]{{3, 4}, {4, 3}, {5, 12}, {12, 5}, {8, 15}, {15, 8}, {7, 24}, {24, 7}, {20, 21}, {21, 20}, {1, 0}, {0, 1}, {0, 0}};
    const auto& e = t[g.P(13)];
    const float norm = std::sqrt(e[0] * e[0] + e[1] * e[1]);
    const float scale = norm == 0.0f ? 0.0f : len / norm;
    ox = e[0] * scale * (g.C(50) ? 1.0f : -1.0f);
    oy = e[1] * scale * (g.C(50) ? 1.0f : -1.0f);
}

inline Spec MakeSpec(u32 fn, u32 n) {
    Spec s;
    s.fn = fn; s.n = n;
    Gen g(n + fn * 0x20000u);
    s.family = n % kFamilies;
    const u32 fam = s.family;
    FillGarbage(s);
    s.regs[0] = StackSeed(n);
    s.regs[1] = g.C(30) ? Garbage(n, 1) : g.R();
    s.regs[2] = g.C(30) ? Garbage(n, 2) : g.R();
    s.regs[3] = g.C(30) ? Garbage(n, 3) : g.R();
    s.seed = g.R();
    for (u32 i = 0; i < 3; ++i) s.hasMat[i] = g.C(70) ? 1u : 0u;
    s.tgtKind = g.C(5) ? 2u : 1u;
    for (int i = 0; i < 8; ++i) s.fpv[i] = g.C(60) ? 1u : (g.C(40) ? 0u : (g.C(50) ? 2u : 3u));
    RandomAtan(g, s);
    RandomPc(g, s, g.C(60) ? fam : 0u);
    for (int i = 0; i < 4; ++i) s.fpe[i] = g.C(55) ? 2u : (g.C(40) ? 0u : (g.C(50) ? 1u : 3u));
    s.task = g.C(70) ? 1u : 0u;
    s.taskType = g.C(50) ? 0x390u : g.P(0x500);
    s.timeMs = g.C(10) ? (g.C(50) ? 0xFFFFFFF0u : 0u) : g.R();
    for (int i = 0; i < 8; ++i) s.rnd[i] = g.C(10) ? 0x7FFFu : (g.C(10) ? 0u : (g.R() & 0x7FFFu));
    for (int c = 0; c < 4; ++c) for (int i = 0; i < 3; ++i) s.norm[c][i] = g.C(8) ? Odd(g) : Bits((float(g.P(2000001)) / 1000000.0f - 1.0f));
    for (int i = 0; i < 4; ++i) s.app[i] = g.C(40) ? 5u : g.P(8);
    for (int i = 0; i < 3; ++i) s.speedV[i] = g.C(10) ? Odd(g) : Moderate(g, 120.0f);
    AW(s, kOffRSeed) = (AW(s, kOffRSeed) & 0xFFFF0000u) | (g.R() & 0xFFFFu);   // word +0x20 (m_nRandomSeed)
    u32 pos[3], dst[3];
    for (int i = 0; i < 3; ++i) { pos[i] = Coord(g, fam); dst[i] = Coord(g, g.C(70) ? fam : 0u); }
    switch (fn) {
    case kHeliFlyDir:
        AW(s, kOffHeading) = g.C(15) ? Odd(g) : Coord(g, fam);
        s.key[0] = AW(s, kOffHeading);
        break;
    case kDogAgainstPlayer:
        s.key[0] = s.fpv[0]; s.key[1] = s.fpv[1];
        break;
    case kHeliCrash:
        AW(s, kOffA14) = g.C(30) ? Odd(g) : Coord(g, fam);
        s.key[0] = AW(s, kOffA14); s.key[1] = AW(s, kOffRSeed) & 1u;
        break;
    case kPlaneCrash:
        s.key[1] = AW(s, kOffRSeed) & 1u;
        break;
    case kPlaneTowards:
    case kHeliTowards: {
        if (fn == kHeliTowards) {
            const u32 w = g.P(8);
            AW(s, kOffForced) = w == 0 ? kQNaN : w == 1 ? 0u : w == 2 ? 0x80000000u : w == 3 ? Bits(-1.0f) : w == 4 ? Bits(-0.0f) : w == 5 ? kSNaN : w == 6 ? Bits(float(g.P(7)) * 0.5f) : Odd(g);
            s.key[0] = AW(s, kOffForced);
        }
        // destino a distancias de umbral respecto a la posicion
        if (g.C(40) && fam < 4) {
            float ox, oy;
            Triple(g, g.C(50) ? 10.0f : 50.0f, ox, oy);
            pos[0] = Bits(float(int(g.P(41)) - 20)); pos[1] = Bits(float(int(g.P(41)) - 20));
            dst[0] = Bits(Flt(pos[0]) + ox); dst[1] = Bits(Flt(pos[1]) + oy);
            if (g.C(30)) dst[g.P(2)] = Bits(g.C(50) ? NextUp(Flt(dst[g.P(2)])) : NextDown(Flt(dst[g.P(2)])));
        }
        SetPos(s, 0, pos, g);
        AW(s, kOffDest) = dst[0]; AW(s, kOffDest + 4) = dst[1]; AW(s, kOffDest + 8) = dst[2];
        s.key[1] = pos[0]; s.key[2] = dst[0];
        break;
    }
    case kPlaneFollow: {
        SetPos(s, 0, pos, g);
        SetPos(s, 1, dst, g);
        s.key[1] = pos[0]; s.key[2] = dst[0];
        break;
    }
    case kHeliAway: {
        SetPos(s, 0, pos, g);
        s.key[1] = pos[0];
        break;
    }
    case kHeliLand: {
        if (g.C(45) && fam < 5) {
            float ox, oy;
            Triple(g, g.C(70) ? 10.0f : 9.99f, ox, oy);
            pos[0] = Bits(float(int(g.P(41)) - 20)); pos[1] = Bits(float(int(g.P(41)) - 20));
            dst[0] = Bits(Flt(pos[0]) + ox); dst[1] = Bits(Flt(pos[1]) + oy);
            if (g.C(30)) dst[g.P(2)] = Bits(g.C(50) ? NextUp(Flt(dst[g.P(2)])) : NextDown(Flt(dst[g.P(2)])));
        } else if (g.C(50)) {
            dst[0] = Bits(Flt(pos[0]) + float(int(g.P(9)) - 4) * 0.5f); dst[1] = Bits(Flt(pos[1]) + float(int(g.P(9)) - 4) * 0.5f);
        }
        SetPos(s, 0, pos, g);
        AW(s, kOffDest) = dst[0]; AW(s, kOffDest + 4) = dst[1]; AW(s, kOffDest + 8) = dst[2];
        // velocidad horizontal alrededor de 0.05
        static const float sp[]{0.0f, 0.01f, 0.03f, 0.035f, 0.0353f, 0.05f, 0.04999999f, 0.06f, 0.0f, 0.001f, 0.0499f, 0.0501f, 1.0f};
        const u32 mode = g.P(6);
        for (int i = 0; i < 2; ++i) AW(s, kOffSpeed + 4 * u32(i)) = mode < 3 ? Bits(sp[g.P(13)] * 0.70710678f * (g.C(50) ? 1.0f : -1.0f)) : (mode == 3 ? Odd(g) : Moderate(g, 0.1f));
        if (g.C(15)) AW(s, kOffSpeed + 4 * g.P(2)) = NearThr(g, 0.05f);
        for (int i = 0; i < 4; ++i) {
            const u32 w = g.P(8);
            AW(s, kOffCompress + 4 * u32(i)) = w < 3 ? Bits(1.0f) : w == 3 ? NearThr(g, 1.0f) : w == 4 ? Bits(0.0f) : w == 5 ? Odd(g) : w == 6 ? Bits(NextDown(1.0f)) : Bits(NextUp(1.0f));
        }
        s.key[1] = pos[0]; s.key[2] = dst[0]; s.key[3] = AW(s, kOffSpeed);
        break;
    }
    case kHeliPolice: {
        if (g.C(40) && fam < 5) {
            float ox, oy;
            Triple(g, 50.0f, ox, oy);
            pos[0] = Bits(float(int(g.P(41)) - 20)); pos[1] = Bits(float(int(g.P(41)) - 20));
            dst[0] = Bits(Flt(pos[0]) + ox); dst[1] = Bits(Flt(pos[1]) + oy);
            if (g.C(30)) dst[g.P(2)] = Bits(g.C(50) ? NextUp(Flt(dst[g.P(2)])) : NextDown(Flt(dst[g.P(2)])));
        }
        static const float zs[]{0.0f, 5.9f, 6.0f, 6.1f, 24.9f, 25.0f, 25.1f, 100.0f, -3.0f, 6.0000005f, 5.9999995f, 25.000002f, 24.999998f};
        if (g.C(55)) dst[2] = g.C(25) ? NearThr(g, g.C(50) ? 6.0f : 25.0f) : Bits(zs[g.P(13)]);
        SetPos(s, 0, pos, g);
        SetPos(s, 1, dst, g);
        const u32 w = g.P(8);
        AW(s, kOffHealth) = w < 2 ? NearThr(g, 230.0f) : w == 2 ? Bits(1000.0f) : w == 3 ? Odd(g) : w == 4 ? Bits(0.0f) : w == 5 ? Bits(229.99998f) : w == 6 ? Bits(230.0f) : Bits(230.00002f);
        s.key[1] = pos[0]; s.key[2] = dst[0]; s.key[3] = AW(s, kOffHealth);
        break;
    }
    case kTestFire: {
        // arma: 0 / 1 / otra; objetivo nulo / T / P / A; posiciones a distancia 150 (triples exactos) y direccion que deja el falso de Normalise con dot en 0.8 +- ulp
        const u32 w = g.P(10);
        AB(s, kOffWeapon) = u8(w < 4 ? 0u : w < 7 ? 1u : g.C(50) ? 2u + g.P(6) : g.P(256));
        AB(s, kOffFlags42C) = u8(g.R());
        const u32 tk = g.P(10);
        s.arg2Kind = tk == 0 ? 0u : tk < 7 ? 1u : tk < 9 ? 2u : 3u;
        const bool early = AB(s, kOffWeapon) > 1 || s.arg2Kind == 0;
        if (!early) s.hasMat[0] = 1u;                      // el EXE usa m_matrix del vehiculo sin comprobar nulo
        static const float tr[][3]{{50, 100, 100}, {100, 50, 100}, {100, 100, 50}, {90, 120, 0}, {120, 90, 0}, {0, 90, 120}, {0, 120, 90}, {90, 0, 120}, {120, 0, 90}, {0, 0, 150}, {150, 0, 0}, {0, 150, 0}};
        if (g.C(80)) {
            const auto& e = tr[g.P(12)];
            for (int i = 0; i < 3; ++i) { pos[i] = Bits(float(int(g.P(41)) - 20)); dst[i] = Bits(Flt(pos[i]) + e[i] * (g.C(50) ? 1.0f : -1.0f)); }
            if (g.C(35)) { const u32 c = g.P(3); dst[c] = Bits(g.C(50) ? NextUp(Flt(dst[c])) : NextDown(Flt(dst[c]))); }
        }
        SetPos(s, 0, pos, g);
        SetPos(s, 1, dst, g);
        SetPos(s, 2, dst, g);
        const u32 axis = g.P(3);
        u32 fwd[3];
        for (int i = 0; i < 3; ++i) fwd[i] = Moderate(g, 1.0f);
        if (g.C(50)) {
            for (int c = 0; c < 4; ++c) for (int i = 0; i < 3; ++i) s.norm[c][i] = Bits(i == int(axis) ? (g.C(85) ? 1.0f : -1.0f) : (g.C(70) ? 0.0f : (g.C(50) ? -0.0f : 0.5f)));
            fwd[axis] = NearThr(g, 0.8f);
        } else if (g.C(30)) {
            fwd[g.P(3)] = Odd(g);
        }
        for (int i = 0; i < 3; ++i) s.mat[0][4 + i] = fwd[i];
        s.key[0] = AB(s, kOffWeapon); s.key[1] = s.arg2Kind; s.key[2] = pos[0]; s.key[3] = dst[0];
        break;
    }
    case kDogMoves: {
        AB(s, kOffTemp) = u8(g.C(60) ? 0u : (g.C(50) ? 1u + g.P(24) : g.P(256)));
        s.hasMat[1] = 1u;                                   // vehiculo2: el EXE usa su m_matrix sin comprobar nulo
        static const float tr[][3]{{42, 56, 0}, {56, 42, 0}, {70, 0, 0}, {0, 70, 0}, {0, 0, 0}, {0, 0, 14}, {0, 0, 15}, {42, 56, 14}};
        if (g.C(85)) {
            const auto& e = tr[g.P(8)];
            for (int i = 0; i < 3; ++i) { pos[i] = Bits(float(int(g.P(41)) - 20)); dst[i] = Bits(Flt(pos[i]) + e[i] * (g.C(50) ? 1.0f : -1.0f)); }
            if (g.C(35)) { const u32 c = g.P(3); dst[c] = Bits(g.C(50) ? NextUp(Flt(dst[c])) : NextDown(Flt(dst[c]))); }
        }
        if (g.C(20)) { pos[2] = Bits(Flt(dst[2]) + (g.C(50) ? 1.0f : -1.0f) * (g.C(50) ? 15.0f : NextUp(15.0f))); }
        SetPos(s, 0, pos, g);
        SetPos(s, 1, dst, g);
        s.rnd[0] = g.C(70) ? ((g.R() & ~0xFFu) | (12u + g.P(4))) : (g.R() & 0x7FFFu);
        const u32 axis = g.P(3);
        u32 fwd[3];
        for (int i = 0; i < 3; ++i) fwd[i] = Moderate(g, 1.0f);
        if (g.C(55)) {
            for (int c = 0; c < 4; ++c) for (int i = 0; i < 3; ++i) s.norm[c][i] = Bits(i == int(axis) ? 1.0f : 0.0f);
            const u32 w = g.P(6);
            fwd[axis] = w < 2 ? NearThr(g, 0.7f) : w < 4 ? 0u : w == 4 ? 0x80000000u : (g.C(50) ? kQNaN : Bits(NextUp(0.0f)));
        }
        for (int i = 0; i < 3; ++i) s.mat[1][4 + i] = fwd[i];
        s.key[0] = AB(s, kOffTemp); s.key[1] = s.rnd[0]; s.key[2] = pos[0]; s.key[3] = dst[0];
        break;
    }
    case kPlaneAttack: {
        SetPos(s, 0, pos, g);
        for (int i = 0; i < 3; ++i) s.speedV[i] = g.C(8) ? Odd(g) : Moderate(g, 120.0f);
        s.key[0] = s.fpv[0]; s.key[1] = s.app[0]; s.key[2] = pos[0]; s.key[3] = s.pc[0][0];
        break;
    }
    case kPlaneDog: {
        const u32 w = g.P(8);
        AB(s, kOffDogFlag) = u8(w < 3 ? 0u : w < 6 ? 1u : (g.C(50) ? 0x81u : g.P(256)));
        s.tgtKind = g.C(8) ? 2u : 1u;
        if (AB(s, kOffDogFlag) == 1 && fam < 6 && g.C(60)) {
            float ox, oy;
            Triple(g, 50.0f, ox, oy);
            pos[0] = Bits(float(int(g.P(41)) - 20)); pos[1] = Bits(float(int(g.P(41)) - 20));
            dst[0] = Bits(Flt(pos[0]) + ox); dst[1] = Bits(Flt(pos[1]) + oy);
            if (g.C(35)) { const u32 c = g.P(2); dst[c] = Bits(g.C(50) ? NextUp(Flt(dst[c])) : NextDown(Flt(dst[c]))); }
        }
        SetPos(s, 0, pos, g);
        u32 tpos[3];
        for (int i = 0; i < 3; ++i) tpos[i] = Coord(g, g.C(70) ? fam : 0u);
        SetPos(s, s.tgtKind == 2 ? 2u : 1u, tpos, g);
        AW(s, kOffDest) = dst[0]; AW(s, kOffDest + 4) = dst[1]; AW(s, kOffDest + 8) = dst[2];
        for (u32 i = 0; i < 3; ++i) { const u32 sp = g.C(8) ? Odd(g) : Moderate(g, 120.0f); (s.tgtKind == 2 ? PW(s, kOffSpeed + 4 * i) : TW(s, kOffSpeed + 4 * i)) = sp; }
        s.rnd[0] = g.C(45) ? ((g.R() & ~0x3FFu) | 500u) : (g.R() & 0x7FFFu);
        s.key[0] = AB(s, kOffDogFlag); s.key[1] = s.rnd[0]; s.key[2] = pos[0]; s.key[3] = dst[0];
        break;
    }
    case kHeliFollow: {
        AB(s, kOffFollowMult) = u8(g.C(35) ? 0u : (g.C(60) ? 1u + g.P(5) : g.P(256)));
        AB(s, kOffCarFlags) = u8(g.C(50) ? 0xA0u : g.R());
        AB(s, kOffMission) = u8(g.C(40) ? 0x29u : (g.C(30) ? 0x2Au : g.R()));
        AW(s, kOffCreation) = g.C(50) ? s.timeMs - 50000u + (g.P(5) - 2u) : g.R();
        {
            const u32 w = g.P(8);
            AW(s, kOffForced) = w == 0 ? kQNaN : w == 1 ? 0u : w == 2 ? 0x80000000u : w == 3 ? Bits(-1.0f) : w == 4 ? kSNaN : w == 5 ? Bits(float(g.P(7)) * 0.5f) : w == 6 ? Bits(-0.0f) : Odd(g);
            s.key[0] = AW(s, kOffForced);
        }
        {
            const u32 w = g.P(8);
            AW(s, kOffHealth) = w < 3 ? NearThr(g, 300.0f) : w == 3 ? Odd(g) : Moderate(g, 1000.0f);
        }
        // objetivo: tipo (bits 0..2 de +0x36) y estado (bits 3..7)
        const u32 type = g.C(35) ? 2u : (g.C(55) ? 3u : g.P(8));
        const u32 status = g.C(50) ? (g.C(50) ? 2u : 3u) : g.P(32);
        (s.tgtKind == 2 ? PW(s, kOffEntInfo) : TW(s, kOffEntInfo)) = 0;
        reinterpret_cast<u8*>(s.t)[kOffEntInfo] = u8(type | (status << 3));
        if (s.tgtKind == 2) { reinterpret_cast<u8*>(s.p)[kOffEntInfo] = u8(type | (status << 3)); }
        if (g.C(45) && fam < 6) {
            float ox, oy;
            Triple(g, g.C(50) ? 50.0f : 25.0f, ox, oy);
            pos[0] = Bits(float(int(g.P(41)) - 20)); pos[1] = Bits(float(int(g.P(41)) - 20));
            dst[0] = Bits(Flt(pos[0]) + ox); dst[1] = Bits(Flt(pos[1]) + oy);
            if (g.C(35)) { const u32 c = g.P(2); dst[c] = Bits(g.C(50) ? NextUp(Flt(dst[c])) : NextDown(Flt(dst[c]))); }
        }
        if (g.C(40)) { static const float zs[]{5.9f, 6.0f, 6.1f, 24.9f, 25.0f, 25.1f, 0.0f, -4.0f, 6.0000005f, 25.000002f}; dst[2] = Bits(zs[g.P(10)]); }
        SetPos(s, 0, pos, g);
        SetPos(s, s.tgtKind == 2 ? 2u : 1u, dst, g);
        for (int i = 0; i < 3; ++i) s.mat[s.tgtKind == 2 ? 2 : 1][4 + i] = g.C(10) ? Odd(g) : Moderate(g, 1.0f);
        s.key[1] = pos[0]; s.key[2] = dst[0]; s.key[3] = AB(s, kOffFollowMult);
        break;
    }
    case kHeliView: {
        s.hasMat[0] = 1u;                                    // el EXE usa m_matrix del helicoptero sin comprobar nulo
        const u32 tdv = g.C(60) ? (2u + g.P(30)) : (g.C(50) ? g.P(256) : 10u * (1u + g.P(6)));
        AB(s, kOffTargetDist) = u8(tdv);
        // el bucle de normalizacion del angulo no termina con ángulos enormes o infinitos: el falso de GetATanOfXY solo devuelve valores finitos y moderados (o NaN)
        for (int i = 0; i < 8; ++i) { const float v = Flt(s.atan[i]); if (!(std::fabs(v) < 1000.0f) && v == v) s.atan[i] = Bits((float(g.P(2000001)) / 1000000.0f - 1.0f) * 3.2f); }
        if (g.C(45) && fam < 6) {
            static const float f[]{2.0f, 1.0f, 0.5f, 1.5f};
            float ox, oy;
            Triple(g, f[g.P(4)] * float(tdv), ox, oy);
            pos[0] = Bits(float(int(g.P(41)) - 20)); pos[1] = Bits(float(int(g.P(41)) - 20));
            dst[0] = Bits(Flt(pos[0]) + ox); dst[1] = Bits(Flt(pos[1]) + oy);
            if (g.C(35)) { const u32 c = g.P(2); dst[c] = Bits(g.C(50) ? NextUp(Flt(dst[c])) : NextDown(Flt(dst[c]))); }
        }
        SetPos(s, 0, pos, g);
        SetPos(s, s.tgtKind == 2 ? 2u : 1u, dst, g);
        for (u32 i = 0; i < 3; ++i) AW(s, kOffSpeed + 4 * i) = g.C(8) ? Odd(g) : Moderate(g, 2.0f);
        for (int i = 0; i < 3; ++i) { s.mat[0][i] = g.C(8) ? Odd(g) : Moderate(g, 1.0f); s.mat[0][4 + i] = g.C(8) ? Odd(g) : Moderate(g, 1.0f); }
        AW(s, kOffMaxAlt) = g.R();
        s.rnd[0] = g.C(10) ? 0x7FFFu : (g.R() & 0x7FFFu);
        s.key[0] = tdv; s.key[1] = pos[0]; s.key[2] = dst[0]; s.key[3] = AW(s, kOffSpeed);
        break;
    }
    case kHeliAttack: {
        AB(s, kOffMission) = u8(g.C(35) ? 0x17u : (g.C(35) ? 0x38u : g.R()));
        if (g.C(50) && fam < 6) {
            static const float tr[][2]{{9, 12}, {12, 9}, {0, 15}, {15, 0}, {10.8f, 14.4f}, {0, 18}, {18, 0}, {10.8f, 14.4f}, {14.4f, 10.8f}};
            const auto& e = tr[g.P(9)];
            pos[0] = Bits(float(int(g.P(41)) - 20)); pos[1] = Bits(float(int(g.P(41)) - 20));
            const float ox = e[0] * (g.C(50) ? 1.0f : -1.0f), oy = e[1] * (g.C(50) ? 1.0f : -1.0f);
            const u32 xv = Bits(Flt(pos[0]) + ox), yv = Bits(Flt(pos[1]) + oy);
            s.pc[2][0] = xv; s.pc[3][0] = xv; s.pc[4][1] = yv; s.pc[5][1] = yv;
            if (g.C(30)) { const u32 c = g.P(4); u32& w = c < 2 ? s.pc[2 + c][0] : s.pc[2 + c][1]; w = Bits(g.C(50) ? NextUp(Flt(w)) : NextDown(Flt(w))); }
        }
        SetPos(s, 0, pos, g);
        s.key[0] = AB(s, kOffMission); s.key[1] = pos[0]; s.key[2] = s.pc[2][0]; s.key[3] = s.pc[5][1];
        break;
    }
    case kBoatHead:
    case kBoatAttack:
    case kBoatCircle: {
        s.hasMat[0] = 1u;                                    // el EXE usa m_matrix de la lancha sin comprobar nulo
        const u32 cruise = g.C(70) ? (1u + g.P(40)) : (g.C(40) ? 0u : g.P(256));
        AB(s, kOffCruise) = u8(cruise);
        // velocidad horizontal alrededor de la que hace err = cruise - |v| * 60 igual a 0, -5 o 0.25 * cruise
        static const float errs[]{0.0f, 1.0e-6f, -1.0e-6f, -5.0f, -5.0000005f, -4.9999995f, 5.0f, 0.25f, 10.0f, -20.0f};
        const float e = errs[g.P(10)] + (g.C(30) ? 0.25f * float(cruise) : 0.0f);
        const float sp = (float(cruise) - e) / 60.0f;
        const float ang = float(g.P(628)) / 100.0f;
        if (g.C(65)) { AW(s, kOffSpeed) = Bits(sp * std::cos(ang)); AW(s, kOffSpeed + 4) = Bits(sp * std::sin(ang)); }
        else { AW(s, kOffSpeed) = g.C(10) ? Odd(g) : Moderate(g, 6.0f); AW(s, kOffSpeed + 4) = g.C(10) ? Odd(g) : Moderate(g, 6.0f); }
        AW(s, kOffSpeed + 8) = g.R();
        // eje adelante: casi siempre unitario, a veces nulo / con -0.0 / NaN / infinito
        {
            const u32 w = g.P(10);
            for (int i = 0; i < 2; ++i) s.mat[0][4 + i] = w < 6 ? Moderate(g, 1.0f) : w == 6 ? 0u : w == 7 ? 0x80000000u : Odd(g);
            if (w == 8) s.mat[0][4 + g.P(2)] = Bits(1.0e-30f);
        }
        // el bucle de normalizacion del angulo no termina con angulos enormes: el falso de GetATanOfXY solo devuelve valores finitos y moderados (o NaN)
        for (int i = 0; i < 8; ++i) { const float v = Flt(s.atan[i]); if (!(std::fabs(v) < 1000.0f) && v == v) s.atan[i] = Bits((float(g.P(2000001)) / 1000000.0f - 1.0f) * 3.2f); }
        SetPos(s, 0, pos, g);
        AW(s, kOffRSeed) = (AW(s, kOffRSeed) & 0xFFFF0000u) | (g.R() & 0xFFFFu);
        if (fn == kBoatHead) {
            s.fargs[0] = Coord(g, g.C(70) ? fam : 0u);
            s.fargs[1] = Coord(g, g.C(70) ? fam : 0u);
            if (g.C(40)) { const float d = (g.C(50) ? 1.0f : -1.0f) * (g.C(50) ? 0.5f : 0.4999999f); s.atan[1] = Bits(Flt(s.atan[0]) - d); }
        } else {
            for (int c = 0; c < 4; ++c) for (int i = 0; i < 3; ++i) s.pc[c][i] = Coord(g, g.C(60) ? fam : 0u);
            if (fn == kBoatAttack) {
                AW(s, kOffModel - 2) = (AW(s, kOffModel - 2) & 0x0000FFFFu) | ((g.C(60) ? 430u : (g.R() & 0xFFFFu)) << 16);
                static const float tr[][3]{{24, 32, 0}, {32, 24, 0}, {0, 0, 40}, {40, 0, 0}, {0, 40, 0}, {12, 16, 0}, {0, 20, 0}};
                if (g.C(45) && fam < 6) {
                    const auto& e2 = tr[g.P(7)];
                    pos[0] = Bits(float(int(g.P(41)) - 20)); pos[1] = Bits(float(int(g.P(41)) - 20)); pos[2] = Bits(float(int(g.P(41)) - 20));
                    SetPos(s, 0, pos, g);
                    for (int i = 0; i < 3; ++i) s.pc[0][i] = Bits(Flt(pos[i]) + e2[i] * (g.C(50) ? 1.0f : -1.0f));
                    if (g.C(35)) { const u32 c = g.P(3); s.pc[0][c] = Bits(g.C(50) ? NextUp(Flt(s.pc[0][c])) : NextDown(Flt(s.pc[0][c]))); }
                }
                if (g.C(40)) { const float d = g.C(50) ? 0.15f : 0.0f; const u32 w = g.P(3); s.atan[1] = Bits(Flt(s.atan[0]) - (w == 0 ? d : w == 1 ? NextUp(d) : NextDown(d))); }
            }
            for (int i = 0; i < 3; ++i) s.speedV[i] = g.C(10) ? Odd(g) : Moderate(g, 30.0f);
            for (int i = 0; i < 4; ++i) for (int j = 0; j < 3; ++j) s.norm[i][j] = g.C(10) ? Odd(g) : Bits((float(g.P(2000001)) / 1000000.0f - 1.0f));
        }
        s.key[0] = cruise; s.key[1] = AW(s, kOffSpeed); s.key[2] = s.mat[0][4]; s.key[3] = AW(s, kOffRSeed);
        break;
    }
    case kRockets: {
        AB(s, kOffWeapon) = u8(g.C(35) ? 0u : (g.C(55) ? 4u : g.P(256)));
        s.hasMat[0] = 1u;                                    // el EXE usa m_matrix del lanzador sin comprobar nulo
        s.arg2Kind = g.C(85) ? 1u : 2u;
        const auto bucket = [](u32 t) { return u32((std::uint64_t(t) * 0x10624DD3ull) >> 32) >> 4; };
        {
            static const u32 deltas[]{0u, 1u, 100u, 249u, 250u, 251u, 499u, 500u, 750u, 1000u, 4000u, 0xFFFFFFFFu};
            const u32 d = deltas[g.P(12)];
            s.prevMs = s.timeMs - d;
            if (g.C(30)) s.prevMs = g.R();
            if (g.C(60) && bucket(s.prevMs) == bucket(s.timeMs)) s.prevMs = s.timeMs - 250u * (1u + g.P(3));   // casi siempre cambia de cubo
        }
        static const float tr[][3]{{48, 64, 0}, {64, 48, 0}, {0, 0, 80}, {80, 0, 0}, {0, 80, 0}, {16, 32, 0}, {0, 0, 0}};
        if (g.C(70)) {
            const auto& e = tr[g.P(7)];
            for (int i = 0; i < 3; ++i) { pos[i] = Bits(float(int(g.P(41)) - 20)); dst[i] = Bits(Flt(pos[i]) - e[i] * (g.C(50) ? 1.0f : -1.0f)); }
            if (g.C(35)) { const u32 c = g.P(3); dst[c] = Bits(g.C(50) ? NextUp(Flt(dst[c])) : NextDown(Flt(dst[c]))); }
        }
        SetPos(s, 0, pos, g);
        SetPos(s, 1, dst, g);
        SetPos(s, 2, dst, g);
        for (int i = 0; i < 3; ++i) { s.mat[0][i] = g.C(8) ? Odd(g) : Moderate(g, 1.0f); s.mat[0][4 + i] = g.C(8) ? Odd(g) : Moderate(g, 1.0f); }
        for (int i = 0; i < 2; ++i) {
            const u32 w = g.P(8);
            s.dline[i] = w < 3 ? NearThr(g, 7.0f) : w == 3 ? Odd(g) : w == 4 ? Bits(0.0f) : Bits(float(g.P(20)));
        }
        s.key[0] = AB(s, kOffWeapon); s.key[1] = s.prevMs; s.key[2] = pos[0]; s.key[3] = dst[0];
        break;
    }
    case kFixed: {
        s.hasMat[0] = 1u;                                    // el EXE usa m_matrix del helicoptero sin comprobar nulo
        // reloj: en el 65 % de los casos el bloque periodico (cada 500 ms con la semilla del helicoptero) se ejecuta: (previo + semilla) % 500 > (actual + semilla) % 500
        {
            const u32 seed16 = g.R() & 0xFFFFu;
            AW(s, kOffRSeed) = (AW(s, kOffRSeed) & 0xFFFF0000u) | seed16;
            if (g.C(65)) {
                const u32 rc = (s.timeMs + seed16) % 500u;
                if (rc < 499u) {
                    const u32 rp = rc + 1u + g.P(499u - rc);
                    s.prevMs = s.timeMs + (rp - rc);
                } else {
                    s.prevMs = s.timeMs;
                }
            } else {
                s.prevMs = g.R();
            }
        }
        {
            const u32 w = g.P(10);
            s.fargs[0] = w < 6 ? Bits((float(g.P(2000001)) / 1000000.0f - 1.0f) * 3.2f) : w == 6 ? 0u : w == 7 ? 0x80000000u : w == 8 ? kQNaN : Bits(float(int(g.P(41)) - 20) * 0.5f);
        }
        // el bucle de normalizacion del angulo no termina con angulos enormes: la orientacion y el rumbo (falso de GetATanOfXY) son finitos y moderados (o NaN)
        for (int i = 0; i < 8; ++i) { const float v = Flt(s.atan[i]); if (!(std::fabs(v) < 1000.0f) && v == v) s.atan[i] = Bits((float(g.P(2000001)) / 1000000.0f - 1.0f) * 3.2f); }
        for (int i = 0; i < 3; ++i) { s.mat[0][i] = g.C(8) ? Odd(g) : Moderate(g, 1.0f); s.mat[0][4 + i] = g.C(8) ? Odd(g) : Moderate(g, 1.0f); }
        SetPos(s, 0, pos, g);
        for (u32 i = 0; i < 3; ++i) AW(s, kOffSpeed + 4 * i) = g.C(10) ? Odd(g) : (g.C(35) ? Bits(0.0f) : Moderate(g, 1.0f));
        // destino cerca de la posicion (errores del orden de los umbrales 5.0)
        for (int i = 0; i < 3; ++i) s.dv[i] = g.C(8) ? Odd(g) : (g.C(60) ? Bits(Flt(pos[i]) + float(int(g.P(21)) - 10) * (g.C(30) ? 0.5f : 1.0f)) : Coord(g, fam));
        AW(s, kOffMaxAlt9A8) = g.C(10) ? Odd(g) : Moderate(g, 200.0f);
        AW(s, kOffAlt9AC) = g.C(10) ? Odd(g) : Moderate(g, 200.0f);
        AW(s, kOffMinAlt9B0) = g.C(10) ? Odd(g) : Moderate(g, 20.0f);
        for (int c = 0; c < 8; ++c) { s.losHit[c] = g.C(60) ? 1u : 0u; for (int i = 0; i < 3; ++i) s.los[c][i] = g.C(8) ? Odd(g) : Moderate(g, 200.0f); }
        if (g.C(30)) s.los[0][2] = Bits(Flt(AW(s, kOffAlt9AC)) - Flt(AW(s, kOffMinAlt9B0)));    // choque justo en el borde de la comparacion
        for (int c = 0; c < 4; ++c) for (int i = 0; i < 3; ++i) s.norm[c][i] = g.C(8) ? Odd(g) : Bits((float(g.P(2000001)) / 1000000.0f - 1.0f));
        s.rnd[0] = g.C(10) ? 0x7FFFu : (g.R() & 0x7FFFu);
        s.key[0] = s.fargs[0]; s.key[1] = s.prevMs; s.key[2] = pos[0]; s.key[3] = s.dv[0];
        break;
    }
    case kPlaneDir: {
        s.hasMat[0] = 1u;                                    // el EXE usa m_matrix del avion sin comprobar nulo
        auto fin = [&](float range) { return g.C(8) ? (g.C(50) ? kQNaN : Bits(0.0f)) : Bits((float(g.P(2000001)) / 1000000.0f - 1.0f) * range); };
        // reloj: en el 55 % de los casos se ejecuta el bloque periodico: (previo + semilla) % 1000 > (actual + semilla) % 1000
        {
            const u32 seed16 = g.R() & 0xFFFFu;
            AW(s, kOffRSeed) = (AW(s, kOffRSeed) & 0xFFFF0000u) | seed16;
            if (g.C(55)) {
                const u32 rc = (s.timeMs + seed16) % 1000u;
                if (rc < 999u) { const u32 rp = rc + 1u + g.P(999u - rc); s.prevMs = s.timeMs + (rp - rc); } else { s.prevMs = s.timeMs; }
            } else {
                s.prevMs = g.R();
            }
        }
        // el bucle de normalizacion del angulo no termina con angulos enormes: rumbos finitos y moderados (o NaN)
        for (int i = 0; i < 8; ++i) { const float v = Flt(s.atan[i]); if (!(std::fabs(v) < 1000.0f) && v == v) s.atan[i] = Bits((float(g.P(2000001)) / 1000000.0f - 1.0f) * 3.2f); }
        for (int i = 0; i < 3; ++i) { s.mat[0][i] = g.C(8) ? Odd(g) : Moderate(g, 1.0f); s.mat[0][4 + i] = g.C(8) ? Odd(g) : Moderate(g, 1.0f); s.mat[0][8 + i] = g.C(8) ? Odd(g) : Moderate(g, 1.0f); }
        if (g.C(70)) s.mat[0][6] = Bits((float(g.P(2000001)) / 1000000.0f - 1.0f));   // fwd.z en [-1, 1] para el asin
        SetPos(s, 0, pos, g);
        for (u32 i = 0; i < 3; ++i) AW(s, kOffSpeed + 4 * i) = g.C(8) ? Odd(g) : (g.C(25) ? Bits(0.0f) : Moderate(g, 1.5f));
        // compresiones de ruedas
        for (u32 i = 0; i < 4; ++i) { const u32 w = g.P(6); AW(s, 0x7D4 + 4 * i) = w < 3 ? Bits(1.0f) : w == 3 ? Bits(0.5f) : w == 4 ? Bits(NextDown(1.0f)) : Odd(g); }
        {
            static const u32 deltas[]{3999u, 4000u, 4001u, 0u, 1u, 100000u, 0xFFFFFFFFu, 2000u, 6000u};
            AW(s, 0x9C0) = g.C(60) ? s.timeMs - deltas[g.P(9)] : g.R();
        }
        AW(s, 0x9CC) = g.C(50) ? Bits(1.0f) : (g.C(50) ? Bits(0.0f) : Moderate(g, 2.0f));
        AB(s, 0x42C) = u8(g.R());
        {
            const u32 w = g.P(8);
            AB(s, kOffTemp) = u8(w == 0 ? 0x0Fu : w == 1 ? 0x10u : w == 2 ? 0x11u : w == 3 ? 0x12u : w == 4 ? 0u : g.P(256));
        }
        AW(s, 0x9A4) = fin(3.2f); AW(s, 0x9A8) = fin(300.0f); AW(s, 0x9AC) = fin(300.0f); AW(s, 0x9B0) = fin(30.0f); AW(s, 0x9B4) = fin(3.2f);
        AW(s, 0x9B8) = fin(3.2f); AW(s, 0x9BC) = fin(1.6f); AW(s, 0x98C) = fin(2.0f); AW(s, 0x99C) = fin(3.2f); AW(s, 0x998) = fin(2.0f); AW(s, 0x988) = g.R(); AW(s, 0x990) = g.R(); AW(s, 0x994) = g.R();
        s.stale = g.C(10) ? (g.C(50) ? kQNaN : 0u) : Bits((float(g.P(2000001)) / 1000000.0f - 1.0f) * (g.C(50) ? 1.0f : 40.0f));
        {
            static const float steps[]{1.0f, 0.5f, 2.0f, 0.0166f, 5.0f, 100.0f, 0.0f, 1.0000001f, 0.9999999f, 3.0f};
            s.stepBits = g.C(5) ? kQNaN : Bits(steps[g.P(10)]);
        }
        for (int i = 0; i < 32; ++i) { const u32 w = g.P(8); s.probe[i] = w < 2 ? NearThr(g, 150.0f) : w == 2 ? Odd(g) : w == 3 ? Bits(float(g.P(400))) : Bits(float(g.P(150))); }
        for (int i = 0; i < 4; ++i) s.rnd[i] = g.R() & 0x7FFFu;
        s.key[0] = s.prevMs; s.key[1] = AB(s, kOffTemp); s.key[2] = AW(s, 0x9C0); s.key[3] = s.stale;
        break;
    }
    case kHeliDir: {
        s.hasMat[0] = 1u;                                    // el EXE usa m_matrix del helicoptero sin comprobar nulo
        {
            const u32 w = g.P(10);
            AB(s, kOffMission) = u8(w < 3 ? 0x29u : w < 5 ? 0x30u : g.R());
        }
        AB(s, 0x3DF) = u8(g.C(70) ? g.P(80) : g.R());
        AB(s, 0x3D9) = u8(g.C(35) ? 0u : (g.C(50) ? g.P(5) : (g.C(50) ? 256u - g.P(5) : g.R())));
        AB(s, kOffCruise) = u8(g.C(70) ? (1u + g.P(40)) : g.R());
        AB(s, 0x42C) = u8(g.R());
        AB(s, 0x9B8) = u8(g.C(45) ? 0u : (g.C(60) ? 1u : g.R()));
        {
            const u32 seed16 = g.R() & 0xFFFFu;
            AW(s, kOffRSeed) = (AW(s, kOffRSeed) & 0xFFFF0000u) | seed16;
            if (g.C(60)) {
                const u32 rc = (s.timeMs + seed16) % 500u;
                if (rc < 499u) { const u32 rp = rc + 1u + g.P(499u - rc); s.prevMs = s.timeMs + (rp - rc); } else { s.prevMs = s.timeMs; }
            } else {
                s.prevMs = g.R();
            }
        }
        // el bucle de normalizacion del angulo no termina con angulos enormes: rumbos finitos y moderados (o NaN)
        for (int i = 0; i < 8; ++i) { const float v = Flt(s.atan[i]); if (!(std::fabs(v) < 1000.0f) && v == v) s.atan[i] = Bits((float(g.P(2000001)) / 1000000.0f - 1.0f) * 3.2f); }
        {
            const u32 w = g.P(10);
            s.fargs[0] = w < 6 ? Bits((float(g.P(2000001)) / 1000000.0f - 1.0f) * 3.2f) : w == 6 ? 0u : w == 7 ? 0x80000000u : w == 8 ? kQNaN : Bits(float(int(g.P(41)) - 20) * 0.5f);
        }
        {
            const float b = float(AB(s, 0x3DF));
            const u32 w = g.P(12);
            s.fargs[1] = w < 2 ? NearThr(g, 60.0f) : w < 4 ? NearThr(g, b) : w == 4 ? kQNaN : w == 5 ? Bits(0.0f) : w == 6 ? Bits(b + 0.5f) : w == 7 ? Bits(1000.0f) : Bits(float(g.P(100)));
        }
        {
            static const u32 a4s[]{0u, 1u, 0x100u, 0x101u, 2u, 0xFFu, 1u, 1u};
            s.dv[0] = a4s[g.P(8)];
        }
        for (int i = 0; i < 3; ++i) { s.mat[0][i] = g.C(8) ? Odd(g) : Moderate(g, 1.0f); s.mat[0][4 + i] = g.C(8) ? Odd(g) : Moderate(g, 1.0f); }
        SetPos(s, 0, pos, g);
        {
            static const float sp[]{0.0f, 0.005f, 0.007f, 0.00707f, 0.01f, 0.0099999f, 0.02f, 0.1f, 1.0f};
            for (u32 i = 0; i < 3; ++i) AW(s, kOffSpeed + 4 * i) = g.C(8) ? Odd(g) : (g.C(50) ? Bits(sp[g.P(9)] * (g.C(50) ? 1.0f : -1.0f)) : Moderate(g, 1.5f));
        }
        for (int i = 0; i < 3; ++i) AW(s, kOffDest + 4 * u32(i)) = g.C(8) ? Odd(g) : Bits(Flt(pos[i]) + float(int(g.P(101)) - 50) * (g.C(30) ? 0.5f : 1.0f));
        AW(s, 0x9A8) = g.C(8) ? Odd(g) : Moderate(g, 300.0f);
        AW(s, 0x9AC) = g.C(8) ? Odd(g) : Moderate(g, 300.0f);
        AW(s, 0x9B0) = g.C(8) ? Odd(g) : Moderate(g, 30.0f);
        AW(s, 0x990) = g.C(10) ? Odd(g) : (g.C(40) ? Bits(-float(g.P(100)) / 100.0f) : Moderate(g, 1.5f));
        AW(s, 0x994) = g.R(); AW(s, 0x998) = g.C(10) ? Odd(g) : Moderate(g, 1.5f); AW(s, 0x98C) = g.R();
        {
            static const float steps[]{1.0f, 0.5f, 2.0f, 0.0166f, 5.0f, 100.0f, 0.0f, 1.0000001f, 3.0f, -1.0f};
            s.stepBits = g.C(5) ? kQNaN : Bits(steps[g.P(10)]);
        }
        for (int c = 0; c < 8; ++c) { s.losHit[c] = g.C(60) ? 1u : 0u; for (int i = 0; i < 3; ++i) s.los[c][i] = g.C(8) ? Odd(g) : Bits(Flt(pos[i]) + float(int(g.P(41)) - 20)); }
        for (int c = 0; c < 4; ++c) for (int i = 0; i < 3; ++i) s.norm[c][i] = g.C(8) ? Odd(g) : Bits((float(g.P(2000001)) / 1000000.0f - 1.0f));
        s.rnd[0] = g.C(10) ? 0x7FFFu : (g.R() & 0x7FFFu);
        s.key[0] = s.fargs[0]; s.key[1] = s.fargs[1]; s.key[2] = s.dv[0]; s.key[3] = AB(s, kOffMission);
        break;
    }
    default: break;
    }
    return s;
}

inline std::string U(u32 v) { return std::to_string(v); }
inline std::string Arr(const u32* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline u32 HashWords(const u32* w, std::size_t n) { u32 h = 2166136261u; for (std::size_t i = 0; i < n; ++i) { h ^= w[i]; h *= 16777619u; } return h; }
inline std::string JsonSpec(const Spec& s) {
    u32 h = HashWords(s.a, kObjWords);
    h = h * 31u + HashWords(s.t, kObjWords);
    h = h * 31u + HashWords(&s.mat[0][0], 3 * kMatWords);
    return "{\"fn\":" + U(s.fn) + ",\"n\":" + U(s.n) + ",\"fam\":" + U(s.family) + ",\"regs\":" + Arr(s.regs, 4) + ",\"has_mat\":" + Arr(s.hasMat, 3) + ",\"tgt\":" + U(s.tgtKind) + ",\"fpv\":" + Arr(s.fpv, 8) + ",\"arg2\":" + U(s.arg2Kind) + ",\"rnd\":" + Arr(s.rnd, 8) + ",\"time\":" + U(s.timeMs) + ",\"fpe\":" + Arr(s.fpe, 4) + ",\"task\":" + U(s.task) + ",\"ttype\":" + U(s.taskType) + ",\"fargs\":" + Arr(s.fargs, 2) + ",\"prev\":" + U(s.prevMs) + ",\"dline\":" + Arr(s.dline, 2) + ",\"dv\":" + Arr(s.dv, 3) + ",\"stale\":" + U(s.stale) + ",\"step\":" + U(s.stepBits) +
           ",\"atan\":" + Arr(s.atan, 8) + ",\"key\":" + Arr(s.key, 4) + ",\"img\":" + U(h) + "}";
}
}  // namespace A63Fix

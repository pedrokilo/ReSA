#pragma once
// Fixtures del banco Helpers66A (lote 66, cinco funciones que seguian como envolturas o llamadas por direccion): 4096 casos por funcion. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / Cases.hpp / LiveComparison.hpp.
//
//   0 CPlayerPed::PlayerWantsToAttack            0x60CC50  thiscall, ECX = this, sin argumentos, `ret`, EAX indefinido (nadie lo lee)        (hook CPlayerPed / PlayerWantsToAttack)
//   1 CPlayerPed::EvaluateNeighbouringTarget     0x60D1C0  thiscall, (CEntity*, CEntity**, float*, float maxDist, float comp, bool), `ret 18h`, void   (hook CPlayerPed / EvaluateNeighbouringTarget)
//   2 CTaskSimpleGangDriveBy::HasClearLineOfSight 0x621B10 thiscall, (CPed*, const CVector*), `ret 8`, AL = linea de vision libre                  (hook CTaskSimpleGangDriveBy / HasClearLineOfSight)
//   3 Fx_c::AddPunchImpact                       0x49F670  thiscall, ECX = this (ignorado), (const CVector*, const CVector*, int), `ret 0Ch`, void  (hook Fx_c / AddPunchImpact)
//   4 RsSavePresetView                           0x619FA0  cdecl, (RwCamera*), AL/EAX = vistas cargadas                                       (hook Rs / RsSavePresetView)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (256 casos por familia y funcion).
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

namespace Hp66Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 5;

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
inline u32 FBits(float f) { return std::bit_cast<u32>(f); }
inline float BitsF(u32 b) { return std::bit_cast<float>(b); }
inline float F(u32 b) { return std::bit_cast<float>(b); }
inline u32 B(float f) { return std::bit_cast<u32>(f); }

// ------------------------------------------------------------------------------------------------ JSON minimo (mismas filas `input` en el juego y en cualquier espejo)
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& A(const char* k, const std::vector<u32>& v) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < v.size(); ++i) { if (i) s += ','; s += std::to_string(v[i]); }
        s += ']'; return *this;
    }
    std::string End() const { return s + "}"; }
};
inline std::vector<u32> V3(const u32* p) { return std::vector<u32>(p, p + 3); }

// ------------------------------------------------------------------------------------------------ valores notables
inline constexpr std::array<u32, 28> kSpecial{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x41C80000u, 0xC1C80000u, 0x40000000u, 0x3E800000u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F7FFFFFu, 0x3F800001u, 0x34000000u, 0x33800000u, 0x4B800000u, 0x7F000000u,
    0x7E800000u, 0x5F800000u, 0x1F800000u, 0x20000000u, 0x441C4000u, 0x441C4001u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0xFFC00001u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7F800001u, 0x7FA00000u, 0xFFA00000u, 0xFF800001u};
inline constexpr std::array<u32, 6> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu};
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 Neg(u32 b) { return b ^ 0x80000000u; }
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
inline u32 MixComponent(u32 c, u32 k) {
    switch (Rand(c, k) % 8) {
    case 0: return Pick(kNan, c, k + 100);
    case 1: return Pick(kInf, c, k + 100);
    case 2: return Rand(c, k + 100) & 0x80000000u;
    case 3: return Rand(c, k + 100) & 0x807FFFFFu;
    case 4: return RandFloat(c, k + 100, 60, 127);
    case 5: return kSpecial[Rand(c, k + 100) % kSpecial.size()];
    case 6: return RandFloat(c, k + 100, -3, 6);
    default: return RandFloat(c, k + 100, -20, -3);
    }
}
// Mueve un float finito `k` unidades en la ultima posicion (orden de los bits de magnitud), conservando el signo.
inline u32 Ulps(u32 bits, int k) {
    const u32 sign = bits & 0x80000000u;
    i32 mag = i32(bits & 0x7FFFFFFFu) + k;
    if (mag < 0) mag = 0;
    if (mag > 0x7F7FFFFF) mag = 0x7F7FFFFF;
    return sign | u32(mag);
}
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Resultado de bool de un colaborador: falso = AL 0 con basura en los bits altos, o AL != 0 con basura en los bits altos
inline u32 BoolRet(u32 n, u32 k, bool wantTrue) {
    static constexpr u32 als[]{1u, 2u, 0x80u, 0xFFu, 0x7Fu, 0x81u};
    if (!wantTrue) return Rand(n, k + 1) % 2 ? 0u : (Rand(n, k + 2) & 0xFFFFFF00u);
    return (Rand(n, k + 3) % 2 ? 0u : (Rand(n, k + 4) & 0xFFFFFF00u)) | als[Rand(n, k + 5) % std::size(als)];
}
inline float Bf(double d) { return float(d); }
inline u32 Fb(double d) { return B(float(d)); }

// ------------------------------------------------------------------------------------------------ arenas (suma a mano: guarda + ventana + guarda)
constexpr u32 kGuard = 0x40;
constexpr u32 kEntWindow = 0x40, kEntArena = kGuard + kEntWindow + kGuard;                  // 0x40 + 0x40 + 0x40 = 0xC0   entidad (tipo en +0x36, matriz en +0x14, posicion en +4)
constexpr u32 kMatWindow = 0x40, kMatArena = kGuard + kMatWindow + kGuard;                  // 0x40 + 0x40 + 0x40 = 0xC0   matriz (posicion en +0x30)
constexpr u32 kVecWindow = 0x10, kVecArena = kGuard + kVecWindow + kGuard;                  // 0x40 + 0x10 + 0x40 = 0x90   un vector / un puntero de salida
constexpr u32 kPedWindow = 0x7A0, kPedArena = kGuard + kPedWindow + kGuard;                 // 0x40 + 0x7A0 + 0x40 = 0x820 ped (hasta +0x79C + 4)
constexpr u32 kPlayerWindow = 0x40, kPlayerArena = kGuard + kPlayerWindow + kGuard;         // 0x40 + 0x40 + 0x40 = 0xC0   datos del jugador (grupo en +0x38)
constexpr u32 kTaskWindow = 0x44, kTaskArena = kGuard + kTaskWindow + kGuard;               // 0x40 + 0x44 + 0x40 = 0xC4   CTaskSimpleGangDriveBy
constexpr u32 kCamWindow = 0x88, kCamArena = kGuard + kCamWindow + kGuard;                  // 0x40 + 0x88 + 0x40 = 0x108  RwCamera (near +0x80, far +0x84)
constexpr u32 kSmokeWindow = 0x20, kSmokeArena = kGuard + kSmokeWindow + kGuard;            // 0x40 + 0x20 + 0x40 = 0xA0   sistema de humo falso (m_SmokeII3expand)
static_assert(kEntArena == 0xC0 && kMatArena == 0xC0 && kVecArena == 0x90 && kPedArena == 0x820 && kPlayerArena == 0xC0 && kTaskArena == 0xC4 && kCamArena == 0x108 && kSmokeArena == 0xA0, "arenas");
// Memoria global del juego que el banco toca (se guarda al empezar y se restaura tras cada lado)
constexpr u32 kGroupsBase = 0xC09920, kGroupStride = 0x2D4, kGroups = 8;                 // CPedGroups::ms_groups (8 x 0x2D4 = 0x16A0)
constexpr u32 kGroupsBytes = kGroups * kGroupStride;                                      // 0x16A0
constexpr u32 kCamPlacement = 0xB6F02C, kCamMatrixPtr = 0xB6F03C, kCamBytes = 0x20;       // TheCamera: m_placement (+4) y m_matrix (+0x14); region 0xB6F028..0xB6F048
constexpr u32 kCamBase = 0xB6F028;
constexpr u32 kMouseFlag = 0xB6EC2E;                                                      // CCamera::m_bUseMouse3rdPerson (byte; region de 4)
constexpr u32 kTimerMs = 0xB7CB84, kIgnoreEntity = 0xB7CD68;                              // CTimer::m_snTimeInMilliseconds, CWorld::pIgnoreEntity
constexpr u32 kSmokeSlot = 0xA9AE24;                                                      // g_fx.m_SmokeII3expand
constexpr u32 kKbUsed = 0xC1705C, kKbHandler = 0xC17060, kNumViews = 0xC17080;           // RsGlobal.keyboard.used / .inputEventHandler, NumPresetViews
constexpr u32 kEngineInstance = 0xC97B24;                                                 // RwEngineInstance (RwGlobals*)
constexpr u32 kFileTableOff = 0xC4, kTableBytes = 0x7C;                                   // RwGlobals+0xC4: fileFuncs[11] (0x2C), stringFuncs (+0xF0 sprintf, +0x120 strlen) hasta +0x140

// ================================================================================================ 0: PlayerWantsToAttack
struct C0 {
    u32 family{}, group{}, flagByte{}, camMouse{}, count{}, tKind{}, tType{}, mKind{}, mType{}, isTagRet{}, canRet{}, findKind{}, findRaw{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline u32 EntType(u32 c, u32 k, int what) {   // byte +0x36 de la entidad: bits bajos = tipo (3 = ped, 4 = objeto, 2 = vehiculo), bits altos de basura
    static constexpr u32 others[]{0, 1, 2, 5, 6, 7};
    u32 type = what == 0 ? 3u : what == 1 ? 4u : what == 2 ? 2u : others[Rand(c, k) % std::size(others)];
    return (Rand(c, k + 1) & 0xF8u) | type;
}
inline C0 MakeC0(u32 n) {
    C0 c; const u32 f = n % 16; c.family = f;
    c.group = Rand(n, 1) % 8;
    static constexpr u32 flags[]{1, 2, 0x7F, 0x80, 0xFF, 1, 1};
    c.flagByte = flags[Rand(n, 2) % std::size(flags)];
    static constexpr u32 mouse[]{1, 1, 2, 0x80, 0xFF};
    c.camMouse = mouse[Rand(n, 3) % std::size(mouse)];
    static constexpr u32 goodCounts[]{1, 2, 3, 8, 0x7FFFFFFF, 0x10000000, 5};
    static constexpr u32 badCounts[]{0, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFFFEu, 0x80000001u, 0};
    c.count = goodCounts[Rand(n, 4) % std::size(goodCounts)];
    c.tKind = 1; c.mKind = 0;
    c.tType = EntType(n, 5, 0); c.mType = EntType(n, 7, 0);
    c.isTagRet = BoolRet(n, 10, false); c.canRet = BoolRet(n, 20, false);
    c.findKind = 1 + Rand(n, 30) % 2; c.findRaw = Rand(n, 31) | 0x10000u;
    switch (f) {
    case 0: c.count = badCounts[Rand(n, 4) % std::size(badCounts)]; break;                                         // sin miembros: sale
    case 1: c.flagByte = 0; break;                                                                                 // m_bMembersEnterLeadersVehicle = 0: sale
    case 2: c.tType = EntType(n, 5, 0); break;                                                                      // objetivo ped: ataque directo
    case 3: c.tType = EntType(n, 5, 3); c.isTagRet = BoolRet(n, 10, true); break;                                   // no ped, IsTag: sale
    case 4: c.tType = EntType(n, 5, 1); c.canRet = BoolRet(n, 20, true); break;                                     // objeto que se puede apuntar: sale
    case 5: c.tType = EntType(n, 5, 1); break;                                                                      // objeto no apuntable: busca ped
    case 6: c.tType = EntType(n, 5, 2); break;                                                                      // vehiculo: busca ped
    case 7: c.tKind = 0; c.mKind = 1; c.mType = EntType(n, 7, 0); break;                                            // sin objetivo + raton: ped del raton
    case 8: c.tKind = 0; c.mKind = 1; c.mType = EntType(n, 7, 1 + Rand(n, 8) % 3); c.isTagRet = BoolRet(n, 10, Rand(n, 9) % 3 == 0); c.canRet = BoolRet(n, 20, Rand(n, 21) % 2 == 0); break;
    case 9: c.tKind = 0; c.mKind = 1; c.camMouse = 0; break;                                                        // sin raton: el objetivo del raton no cuenta
    case 10: c.findKind = 0; c.tKind = Rand(n, 11) % 2; break;                                                      // FindPedToAttack devuelve nulo
    case 11: c.mKind = 1; c.camMouse = 1; c.tType = EntType(n, 5, 0); c.mType = EntType(n, 7, 1); break;            // objetivo y raton: gana el objetivo
    default:                                                                                                         // 12..15: mezclas
        c.tKind = Rand(n, 12) % 3 != 0; c.mKind = Rand(n, 13) % 2;
        c.tType = EntType(n, 5, int(Rand(n, 14) % 4)); c.mType = EntType(n, 7, int(Rand(n, 15) % 4));
        if (Rand(n, 16) % 4 == 0) c.camMouse = 0;
        if (Rand(n, 17) % 5 == 0) c.count = badCounts[Rand(n, 4) % std::size(badCounts)];
        if (Rand(n, 18) % 6 == 0) c.flagByte = 0;
        c.isTagRet = BoolRet(n, 10, Rand(n, 19) % 3 == 0); c.canRet = BoolRet(n, 20, Rand(n, 22) % 2 == 0);
        if (Rand(n, 23) % 5 == 0) c.findKind = 0;
        break;
    }
    c.seed = Rand(n, 40);
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC0(const C0& c) {
    Js j; j.U("family", c.family).U("group", c.group).U("flag_byte", c.flagByte).U("cam_mouse", c.camMouse).U("count", c.count).U("t_kind", c.tKind).U("t_type", c.tType)
     .U("m_kind", c.mKind).U("m_type", c.mType).U("is_tag_ret", c.isTagRet).U("can_ret", c.canRet).U("find_kind", c.findKind).U("find_raw", c.findRaw).U("seed", c.seed)
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 1: EvaluateNeighbouringTarget
struct C1 {
    u32 family{}, selfMat{}, tgtMat{}, camMat{};
    u32 self[3]{}, tgt[3]{}, cam[3]{};                 // posiciones USADAS (en el modo con matriz: matriz+0x30)
    u32 selfOther[3]{}, tgtOther[3]{}, camOther[3]{};  // posiciones NO usadas
    u32 maxDist{}, comp{}, flag{}, outPrio{}, outTargetRaw{}, atanRet{}, brokenRet{}, slot{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline C1 MakeC1(u32 n) {
    C1 c; const u32 f = n % 16; c.family = f;
    c.selfMat = Rand(n, 1) % 3 == 0; c.tgtMat = Rand(n, 2) % 3 == 0; c.camMat = Rand(n, 3) % 3 == 0;
    for (u32 i = 0; i < 3; ++i) {
        c.selfOther[i] = Rand(n, 10 + i); c.tgtOther[i] = Rand(n, 13 + i); c.camOther[i] = Rand(n, 16 + i);
        c.self[i] = FBits(float(double(i32(Rand(n, 20 + i) % 4001u) - 2000) / 16.0));
        c.cam[i] = FBits(float(double(i32(Rand(n, 23 + i) % 4001u) - 2000) / 16.0));
    }
    // desplazamiento entre jugador y objetivo
    double d[3];
    static constexpr int triples[][3]{{3, 4, 0}, {0, 3, 4}, {4, 0, 3}, {2, 3, 6}, {1, 2, 2}, {12, 4, 3}, {0, 0, 5}, {5, 0, 0}};
    const auto& t = triples[Rand(n, 30) % std::size(triples)];
    const double scale = (Rand(n, 31) % 3 == 0) ? 1.0 : double(1 + Rand(n, 32) % 4) / 4.0;
    for (int i = 0; i < 3; ++i) d[i] = double(t[i]) * scale * ((Rand(n, 33 + i) & 1) ? -1.0 : 1.0);
    for (u32 i = 0; i < 3; ++i) c.tgt[i] = FBits(float(double(BitsF(c.self[i])) + d[i]));
    const double dist = std::sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
    c.maxDist = FBits(float(dist * (1.0 + double(i32(Rand(n, 40) % 2001u) - 1000) * 1e-4)) + (Rand(n, 41) % 4 == 0 ? 0.0f : 20.0f));   // casi siempre dentro del rango
    // angulo deseado (rumbo menos compensacion) por familia
    double A = 0.0;
    auto frac = [&](u32 k, double lo, double hi) { return lo + (hi - lo) * double(Rand(n, k) % 100001u) / 100000.0; };
    c.flag = Rand(n, 50) % 2;
    c.brokenRet = BoolRet(n, 52, Rand(n, 51) % 12 == 0);
    switch (f) {
    case 0: c.maxDist = FBits(float(dist * 0.5)); A = frac(60, -0.8, 0.8); break;                                           // fuera de rango
    case 1: c.maxDist = Pick(kNan, n, 61); A = frac(60, -0.8, 0.8); break;                                                  // maxDist NaN
    case 2: c.brokenRet = BoolRet(n, 52, true); A = frac(60, -0.8, 0.8); break;                                             // DoesTargetHaveToBeBroken
    case 3: A = frac(60, 3.2, 3.2 + 6.28318 * double(1 + Rand(n, 62) % 6)); break;                                         // mayor que pi: bucle de restar 2 pi
    case 4: A = -frac(60, 3.2, 3.2 + 6.28318 * double(1 + Rand(n, 62) % 6)); break;                                        // menor que -pi
    case 5: A = (Rand(n, 63) % 2 ? 1.0 : -1.0) * frac(60, 0.88, 3.1); break;                                               // fuera de 50 grados
    case 6: A = frac(60, 0.01, 0.87); c.flag = 1; break;                                                                    // derecha: prioridad -angulo
    case 7: A = frac(60, 0.01, 0.87); c.flag = 0; break;                                                                    // izquierda con angulo > 0: -100000
    case 8: A = -frac(60, 0.01, 0.87); c.flag = 0; break;                                                                   // izquierda: prioridad angulo
    case 9: A = -frac(60, 0.01, 0.87); c.flag = 1; break;                                                                   // derecha con angulo < 0: -100000
    case 10: {                                                                                                               // fronteras exactas
        static constexpr double edges[]{0.0, -0.0, 0.8726646304130554, -0.8726646304130554, 3.1415927410125732, -3.1415927410125732, 6.2831854820251465, -6.2831854820251465, 0.87266457, 0.87266469, 3.1415925, 3.1415930};
        A = edges[Rand(n, 64) % std::size(edges)];
        break;
    }
    case 11: A = 0.4; break;                                                                                                 // NaN en rumbo o compensacion (abajo)
    case 12: A = frac(60, -0.87, 0.87); break;                                                                               // prioridad inicial cerca de la nueva
    case 13: {                                                                                                               // posiciones diminutas: x87 (exponente extendido) contra SSE
        for (u32 i = 0; i < 3; ++i) { c.self[i] = Pick(kTiny, n, 70 + i); c.tgt[i] = Pick(kTiny, n, 73 + i); }
        c.maxDist = Pick(kTiny, n, 76);
        if (Rand(n, 77) % 3 == 0) c.maxDist = FBits(1e-30f * float(1 + Rand(n, 78) % 9));
        A = frac(60, -0.8, 0.8);
        break;
    }
    case 14: {                                                                                                               // enormes: los cuadrados desbordan float pero no la x87
        for (u32 i = 0; i < 3; ++i) { c.self[i] = RandFloat(n, 80 + i, 60, 126); c.tgt[i] = Rand(n, 83 + i) % 2 ? Neg(c.self[i]) : RandFloat(n, 86 + i, 60, 126); }
        c.maxDist = Rand(n, 89) % 2 ? 0x7F800000u : FBits(3.0e38f);
        A = frac(60, -0.8, 0.8);
        break;
    }
    default: {                                                                                                               // 15: mezcla
        for (u32 i = 0; i < 3; ++i) {
            if (Rand(n, 90 + i) % 6 == 0) c.tgt[i] = MixComponent(n, 93 + i);
            if (Rand(n, 96 + i) % 8 == 0) c.self[i] = MixComponent(n, 99 + i);
            if (Rand(n, 102 + i) % 8 == 0) c.cam[i] = MixComponent(n, 105 + i);
        }
        if (Rand(n, 108) % 4 == 0) c.maxDist = MixComponent(n, 109);
        A = frac(60, -2.0, 2.0);
        break;
    }
    }
    // compensacion y valor devuelto por GetATanOfXY (atanRet - comp = A)
    u32 comp;
    switch (Rand(n, 120) % 8) {
    case 0: comp = 0u; break;
    case 1: comp = FBits(float(frac(121, -7.0, 7.0))); break;
    case 2: comp = FBits(3.1415927f); break;
    case 3: comp = FBits(6.2831855f); break;
    case 4: comp = Pick(kTiny, n, 122); break;
    case 5: comp = FBits(float(frac(123, -200.0, 200.0))); break;
    default: comp = FBits(float(frac(124, -3.0, 3.0))); break;
    }
    c.comp = comp;
    c.atanRet = FBits(float(A + double(BitsF(comp))));
    if (f == 10 && Rand(n, 125) % 2) c.atanRet = FBits(float(A)), c.comp = 0u;
    if (f == 11) { if (Rand(n, 126) % 2) c.atanRet = Pick(kNan, n, 127); else c.comp = Pick(kNan, n, 128); }
    // el bucle de normalizacion no acaba con |angulo| infinito ni enorme: fuera de la familia 11 el rumbo es siempre finito
    // prioridad inicial de salida
    static constexpr u32 prios[]{0xC7C35000u, 0xC8435000u, 0xFF7FFFFFu, 0xFF800000u, 0x7F800000u, 0x00000000u, 0x80000000u, 0xBF800000u, 0x3F800000u};
    c.outPrio = prios[Rand(n, 130) % std::size(prios)];
    if (f == 12) {
        const double p = c.flag ? (A > 0.0 ? -A : -100000.0) : (A < 0.0 ? A : -100000.0);
        const u32 pb = FBits(float(p));
        c.outPrio = Rand(n, 131) % 6 == 0 ? Pick(kNan, n, 132) : Ulps(pb, int(Rand(n, 133) % 5) - 2);
    }
    // el valor de `flag` es un dword del llamador: el EXE lee solo el byte bajo
    if (Rand(n, 140) % 4 == 0) c.flag = c.flag ? Pick(std::array<u32, 5>{0x101u, 0xFFFFFF01u, 0x2u, 0x80u, 0xFFu}, n, 141) : Pick(std::array<u32, 5>{0x100u, 0xFFFFFF00u, 0x0u, 0x7FFFFF00u, 0x80000000u}, n, 141);
    c.outTargetRaw = Rand(n, 142);
    c.slot = Rand(n, 143) % 3 == 0 ? u32(i32(Rand(n, 144) % 256u) - 128) & 0xFFu : Rand(n, 145) % 13u;
    c.seed = Rand(n, 150);
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC1(const C1& c) {
    Js j; j.U("family", c.family).U("self_mat", c.selfMat).U("tgt_mat", c.tgtMat).U("cam_mat", c.camMat).A("self", V3(c.self)).A("tgt", V3(c.tgt)).A("cam", V3(c.cam))
     .A("self_other", V3(c.selfOther)).A("tgt_other", V3(c.tgtOther)).A("cam_other", V3(c.camOther)).U("max_dist", c.maxDist).U("comp", c.comp).U("flag", c.flag).U("out_prio", c.outPrio)
     .U("out_target_raw", c.outTargetRaw).U("atan_ret", c.atanRet).U("broken_ret", c.brokenRet).U("slot", c.slot).U("seed", c.seed)
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 2: HasClearLineOfSight
struct C2 {
    u32 family{}, hasMat{}, timer{}, losTime{}, blocked{}, tKind{}, tRaw{}, losRet{}, randRet{}, ignoreInit{}, seed{};
    u32 pos[3]{}, other[3]{}, aim[3]{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline C2 MakeC2(u32 n) {
    C2 c; const u32 f = n % 16; c.family = f;
    c.hasMat = Rand(n, 1) % 3 == 0;
    for (u32 i = 0; i < 3; ++i) {
        c.pos[i] = FBits(float(double(i32(Rand(n, 10 + i) % 4001u) - 2000) / 8.0));
        c.other[i] = Rand(n, 13 + i);
        c.aim[i] = Rand(n, 16 + i) % 5 == 0 ? Rand(n, 19 + i) : FBits(float(double(i32(Rand(n, 22 + i) % 4001u) - 2000) / 8.0));
    }
    // tiempo: familias 0..3 sin comprobacion (timer <= losTime), 4..15 con comprobacion
    static constexpr u32 times[]{0u, 1u, 1000u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFFF0u, 123456u, 0x7FFFFFF0u, 5000000u};
    const u32 t0 = times[Rand(n, 30) % std::size(times)];
    c.timer = Rand(n, 31) % 3 == 0 ? Rand(n, 32) : t0;
    const u32 delta = 1 + Rand(n, 33) % 3000u;
    switch (f) {
    case 0: c.losTime = c.timer; break;                                    // igual: no comprueba (jbe)
    case 1: c.losTime = c.timer + (1 + Rand(n, 34) % 100u); break;         // timer menor (con vuelta de contador)
    case 2: c.losTime = 0xFFFFFFFFu; c.timer = Rand(n, 35) % 2 ? 0x7FFFFFFFu : 0x80000000u; break;
    case 3: c.losTime = c.timer + 0x80000000u; break;                      // solo el signo difiere: comparacion SIN signo
    default: c.losTime = c.timer - delta; break;                           // comprueba
    }
    if (f == 4) c.losTime = 0x7FFFFFFFu, c.timer = 0x80000000u;            // con signo seria menor: sin signo es mayor
    if (f == 5) c.losTime = c.timer - 1u;                                   // un milisegundo por encima
    c.blocked = (Rand(n, 36) % 2) ? 0u : (Rand(n, 37) % 2 ? 1u : Pick(std::array<u32, 4>{2u, 0x80u, 0xFFu, 0x10u}, n, 38));
    c.tKind = Rand(n, 39) % 4 == 0 ? 0u : (Rand(n, 39) % 4 == 1 ? 2u : 1u);                         // 0 nulo, 1 entidad de la arena, 2 puntero bruto
    if (f == 6) c.tKind = 0;                                                                         // sin objetivo: no hay linea, solo se reprograma
    if (f >= 7 && f <= 9) c.tKind = 1;
    c.tRaw = Rand(n, 40) | 0x10000u;
    c.losRet = BoolRet(n, 41, Rand(n, 47) % 2);
    if (f == 7) c.losRet = BoolRet(n, 41, true);
    if (f == 8) c.losRet = BoolRet(n, 41, false);
    c.randRet = Rand(n, 48) % 3 == 0 ? Pick(std::array<u32, 8>{0u, 0xFFFFu, 0x10000u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0x7FFF8000u, 0x8000u}, n, 49) : Rand(n, 50);
    // posiciones notables
    if (f == 10) { for (u32 i = 0; i < 3; ++i) if (Rand(n, 60 + i) % 2) c.pos[i] = Pick(kNan, n, 63 + i); }             // NaN (con senalizantes) en la posicion del ped
    if (f == 11) { c.pos[1] = Pick(std::array<u32, 4>{0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFBFFFFFu}, n, 66); }   // y senalizante: la x87 lo silencia
    if (f == 12) { c.pos[2] = Pick(std::array<u32, 4>{0x7F800001u, 0xFF800001u, 0x7FA00000u, 0x7FC00007u}, n, 67); }   // z senalizante
    if (f == 13) { for (u32 i = 0; i < 3; ++i) c.pos[i] = Pick(kTiny, n, 70 + i); }                                      // ceros y subnormales (z + 0.5)
    if (f == 14) { for (u32 i = 0; i < 3; ++i) c.pos[i] = Rand(n, 73 + i) % 3 == 0 ? Pick(kInf, n, 76 + i) : RandFloat(n, 79 + i, 60, 127); }
    if (f == 15) { for (u32 i = 0; i < 3; ++i) c.pos[i] = MixComponent(n, 83 + i); for (u32 i = 0; i < 3; ++i) if (Rand(n, 86 + i) % 3 == 0) c.aim[i] = MixComponent(n, 89 + i); }
    c.ignoreInit = Rand(n, 95) % 2 ? 0u : (Rand(n, 96) | 0x1000u);
    c.seed = Rand(n, 97);
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC2(const C2& c) {
    Js j; j.U("family", c.family).U("has_mat", c.hasMat).U("timer", c.timer).U("los_time", c.losTime).U("blocked", c.blocked).U("t_kind", c.tKind).U("t_raw", c.tRaw).U("los_ret", c.losRet)
     .U("rand_ret", c.randRet).U("ignore_init", c.ignoreInit).U("seed", c.seed).A("pos", V3(c.pos)).A("other", V3(c.other)).A("aim", V3(c.aim))
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 3: AddPunchImpact
struct C3 {
    u32 family{}, camMode{}, thisKind{}, num{}, seed{};
    u32 cam[3]{}, other[3]{}, pos[3]{}, vel[3]{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline void Split3(u32 c, u32 k, double len, double out[3]) {
    double a = double(Rand(c, k) % 2001) / 1000.0 - 1.0;
    double b = double(Rand(c, k + 1) % 2001) / 1000.0 - 1.0;
    double d = double(Rand(c, k + 2) % 2001) / 1000.0 - 1.0;
    double m = a * a + b * b + d * d;
    if (m < 1e-6) { a = 1.0; b = 0.0; d = 0.0; m = 1.0; }
    const double s = len / std::sqrt(m);
    out[0] = a * s; out[1] = b * s; out[2] = d * s;
}
inline C3 MakeC3(u32 n) {
    C3 f{};
    f.family = n % kFamilies;
    const u32 c = n * 8u + 0x30003u;
    f.camMode = Rand(c, 1) % 5 < 3 ? 0u : 1u;
    f.thisKind = Rand(c, 3) % 4;
    f.num = SeedReg(c, 4);
    f.seed = Rand(c, 6) | 1u;
    f.eax = SeedReg(c, 7); f.ecx = SeedReg(c, 8); f.edx = SeedReg(c, 9); f.stack = StackSeed(Rand(c, 11) % 12);
    for (int i = 0; i < 3; ++i) { f.other[i] = MixComponent(c, 20 + i); f.vel[i] = Rand(c, 30 + i) % 4 == 0 ? MixComponent(c, 33 + i) : RandFloat(c, 36 + i, -6, 4); }
    auto camNormal = [&](int hi) { for (int i = 0; i < 3; ++i) f.cam[i] = RandFloat(c, 41 + i, -4, hi); };
    auto fromDelta = [&](const double d[3]) { for (int i = 0; i < 3; ++i) f.pos[i] = FBits(float(double(BitsF(f.cam[i])) - d[i])); };
    double d[3];
    switch (f.family) {
    case 0: camNormal(11); Split3(c, 50, double(Rand(c, 53) % 2000) / 100.0, d); fromDelta(d); break;                                      // dentro de rango
    case 1: camNormal(8); Split3(c, 50, 25.0 * (1.0 + (double(Rand(c, 53) % 2001) - 1000.0) * 1e-6), d); fromDelta(d);
            f.pos[Rand(c, 54) % 3] = Ulps(f.pos[Rand(c, 54) % 3], int(Rand(c, 55) % 7) - 3); break;                                         // alrededor de 625
    case 2: {                                                                                                                               // suma exactamente 625
        static constexpr int quads[][3] = {{25, 0, 0}, {0, 7, 24}, {0, 15, 20}, {12, 15, 16}, {7, 24, 0}, {15, 20, 0}, {20, 0, 15}, {24, 0, 7}, {0, 20, 15}, {16, 12, 15}};
        const auto& q = quads[Rand(c, 50) % std::size(quads)];
        const int perm[3][3] = {{0, 1, 2}, {1, 2, 0}, {2, 0, 1}};
        const auto& p = perm[Rand(c, 51) % 3];
        for (int i = 0; i < 3; ++i) {
            const double v = double(q[p[i]]) * ((Rand(c, 52 + i) & 1) ? -1.0 : 1.0);
            f.cam[i] = FBits(float(double(int(Rand(c, 60 + i) % 2001) - 1000)));
            f.pos[i] = FBits(float(double(BitsF(f.cam[i])) - v));
        }
        if (Rand(c, 56) % 3 == 0) f.pos[Rand(c, 57) % 3] = Ulps(f.pos[Rand(c, 57) % 3], Rand(c, 58) % 2 ? 1 : -1);
        break;
    }
    case 3: camNormal(11); for (int i = 0; i < 3; ++i) f.pos[i] = RandFloat(c, 70 + i, 10, 40); break;                                      // lejos
    case 4: for (int i = 0; i < 3; ++i) { f.cam[i] = RandFloat(c, 41 + i, 60, 127); f.pos[i] = Rand(c, 70 + i) % 3 == 0 ? Neg(f.cam[i]) : RandFloat(c, 70 + i, 60, 127); } break;   // enormes
    case 5: camNormal(8); Split3(c, 50, 10.0, d); fromDelta(d);                                                                             // NaN
            for (int j = 0; j < 1 + int(Rand(c, 56) % 2); ++j) { if (Rand(c, 57 + j) & 1) f.cam[Rand(c, 60 + j) % 3] = Pick(kNan, c, 62 + j); else f.pos[Rand(c, 60 + j) % 3] = Pick(kNan, c, 62 + j); }
            break;
    case 6: camNormal(8); Split3(c, 50, 10.0, d); fromDelta(d);                                                                             // infinitos
            for (int j = 0; j < 1 + int(Rand(c, 56) % 3); ++j) {
                const u32 inf = Pick(kInf, c, 62 + j); const unsigned axis = Rand(c, 60 + j) % 3; const u32 mode = Rand(c, 66 + j) % 4;
                if (mode == 0) f.cam[axis] = inf; else if (mode == 1) f.pos[axis] = inf; else if (mode == 2) { f.cam[axis] = inf; f.pos[axis] = inf; } else { f.cam[axis] = inf; f.pos[axis] = Neg(inf); }
            }
            break;
    case 7: for (int i = 0; i < 3; ++i) { f.cam[i] = Pick(kTiny, c, 41 + i); f.pos[i] = Pick(kTiny, c, 70 + i); } break;                       // ceros y subnormales
    case 8: camNormal(2); for (int i = 0; i < 3; ++i) f.pos[i] = Rand(c, 70 + i) % 3 == 0 ? f.cam[i] : Ulps(f.cam[i], int(Rand(c, 73 + i) % 9) - 4); break;   // diferencias diminutas
    case 9: for (int i = 0; i < 3; ++i) { f.cam[i] = RandFloat(c, 41 + i, 14, 30); f.pos[i] = Ulps(f.cam[i], int(Rand(c, 73 + i) % 41) - 20); } break;
    case 10: {                                                                                                                              // un solo eje cerca del limite
        camNormal(9); for (int i = 0; i < 3; ++i) f.pos[i] = f.cam[i];
        const unsigned axis = Rand(c, 50) % 3;
        const double v = 25.0 * (1.0 + (double(Rand(c, 53) % 2001) - 1000.0) * 1e-7) * ((Rand(c, 54) & 1) ? -1.0 : 1.0);
        f.pos[axis] = FBits(float(double(BitsF(f.cam[axis])) - v)); f.pos[axis] = Ulps(f.pos[axis], int(Rand(c, 55) % 5) - 2);
        break;
    }
    default:                                                                                                                                // 11..15: mezclas
        camNormal(7);
        for (int i = 0; i < 3; ++i) {
            f.pos[i] = Rand(c, 70 + i) % 4 == 0 ? MixComponent(c, 75 + i) : FBits(float(double(BitsF(f.cam[i])) + (double(Rand(c, 80 + i) % 6001) - 3000.0) / 100.0));
            if (Rand(c, 85 + i) % 7 == 0) f.cam[i] = MixComponent(c, 88 + i);
        }
        break;
    }
    return f;
}
inline std::string JsonC3(const C3& c) {
    Js j; j.U("family", c.family).U("cam_mode", c.camMode).U("this_kind", c.thisKind).U("num", c.num).U("seed", c.seed).A("cam", V3(c.cam)).A("other", V3(c.other)).A("pos", V3(c.pos)).A("vel", V3(c.vel))
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 4: RsSavePresetView
struct C4 {
    u32 family{}, frameKind{}, nearPl{}, farPl{}, fopenKind{}, fputsRet{}, loadRet{}, numBefore{}, numAfter{}, createKind{}, kbUsed{}, kbRet{}, normScale{}, closeRet{}, seed{};
    u32 pos[3]{}, at[3]{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline C4 MakeC4(u32 n) {
    C4 c; const u32 f = n % 16; c.family = f;
    c.frameKind = Rand(n, 1) % 4 == 0;
    c.nearPl = Rand(n, 2) % 4 == 0 ? MixComponent(n, 3) : FBits(float(0.1 + double(Rand(n, 4) % 1000) / 100.0));
    c.farPl = Rand(n, 5) % 4 == 0 ? MixComponent(n, 6) : FBits(float(100.0 + double(Rand(n, 7) % 100000) / 10.0));
    c.fopenKind = f == 0 ? 0u : (Rand(n, 8) % 2 ? 1u : 2u);                   // 0 = nulo (fallo); 1 = ficha pequena; 2 = puntero alto
    c.fputsRet = f == 1 ? Pick(std::array<u32, 4>{0u, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFFFEu}, n, 9) : Pick(std::array<u32, 5>{1u, 7u, 0x7FFFFFFFu, 100u, 2u}, n, 9);
    c.loadRet = f == 2 ? 0u : (Rand(n, 10) % 5 == 0 ? 0u : 1u);
    c.numBefore = Rand(n, 11) % 3 == 0 ? Rand(n, 12) : Rand(n, 13) % 12;
    c.numAfter = Rand(n, 14) % 4 == 0 ? Pick(std::array<u32, 6>{0u, 0x80000000u, 0xFFFFFFFFu, 1u, 0x7FFFFFFFu, 2u}, n, 15) : 1 + Rand(n, 16) % 20;
    c.createKind = f == 3 ? 1u : (Rand(n, 17) % 16 == 0 ? 1u : 0u);
    c.kbUsed = Rand(n, 18) % 3 == 0 ? 0u : Pick(std::array<u32, 6>{1u, 0x100u, 0xFFFF0000u, 0xFFFFFFFFu, 2u, 0x80000000u}, n, 19);
    c.kbRet = Rand(n, 20);
    c.normScale = Rand(n, 21) % 3 == 0 ? Pick(std::array<u32, 8>{0x3F800000u, 0x3F400000u, 0x40000000u, 0x40400000u, 0xBF800000u, 0x3F000000u, 0x00000000u, 0x3F800001u}, n, 22) : FBits(float(0.5 + double(Rand(n, 23) % 2500) / 1000.0));
    c.closeRet = Rand(n, 24);
    // matriz: at (indices 8..10), posicion (12..14)
    for (u32 i = 0; i < 3; ++i) c.pos[i] = FBits(float(double(i32(Rand(n, 30 + i) % 8001u) - 4000) / 16.0));
    {   // vector de vista: unitario aleatorio o especial
        double a = double(Rand(n, 40) % 2001) / 1000.0 - 1.0, b = double(Rand(n, 41) % 2001) / 1000.0 - 1.0, d = double(Rand(n, 42) % 2001) / 1000.0 - 1.0;
        double m = std::sqrt(a * a + b * b + d * d); if (m < 1e-3) { a = 0; b = 1; d = 0; m = 1; }
        c.at[0] = FBits(float(a / m)); c.at[1] = FBits(float(b / m)); c.at[2] = FBits(float(d / m));
    }
    switch (f) {
    case 4: c.at[1] = FBits(1.0f + float(Rand(n, 43) % 4) * 1.1920929e-07f); break;                                          // at.y > 1 por redondeo: recorte
    case 5: c.at[1] = FBits(-1.0f - float(Rand(n, 43) % 4) * 1.1920929e-07f); break;                                         // at.y < -1
    case 6: c.at[0] = 0u; c.at[1] = FBits(1.0f); c.at[2] = 0u; break;                                                        // mirando hacia arriba: t = 0
    case 7: c.at[0] = 0u; c.at[1] = FBits(-1.0f); c.at[2] = 0u; break;
    case 8: c.at[0] = Pick(kNan, n, 44); break;                                                                              // NaN en at
    case 9: c.at[Rand(n, 45) % 3] = Pick(kInf, n, 46); break;                                                                // infinito en at
    case 10: for (u32 i = 0; i < 3; ++i) c.at[i] = Pick(kTiny, n, 47 + i); break;                                            // ceros y subnormales (0 * x)
    case 11: for (u32 i = 0; i < 3; ++i) c.at[i] = FBits(float(double(i32(Rand(n, 50 + i) % 6001u) - 3000) / 1000.0)); break; // fuera del rango [-1, 1]
    case 12: c.at[0] = FBits(1.0f); c.at[1] = 0u; c.at[2] = 0u; break;                                                       // mirando a +x
    case 13: c.at[0] = 0u; c.at[1] = 0u; c.at[2] = FBits(Rand(n, 53) % 2 ? 1.0f : -1.0f); break;                              // +-z
    case 14: for (u32 i = 0; i < 3; ++i) c.pos[i] = MixComponent(n, 54 + i); break;                                          // posicion con NaN/inf/subnormales (solo se copia)
    case 15: for (u32 i = 0; i < 3; ++i) if (Rand(n, 57 + i) % 3 == 0) c.at[i] = MixComponent(n, 60 + i); break;
    default: break;
    }
    c.seed = Rand(n, 90);
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC4(const C4& c) {
    Js j; j.U("family", c.family).U("frame_kind", c.frameKind).U("near", c.nearPl).U("far", c.farPl).U("fopen_kind", c.fopenKind).U("fputs_ret", c.fputsRet).U("load_ret", c.loadRet)
     .U("num_before", c.numBefore).U("num_after", c.numAfter).U("create_kind", c.createKind).U("kb_used", c.kbUsed).U("kb_ret", c.kbRet).U("norm_scale", c.normScale).U("close_ret", c.closeRet)
     .U("seed", c.seed).A("pos", V3(c.pos)).A("at", V3(c.at)).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}
}

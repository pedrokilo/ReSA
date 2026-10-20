#pragma once
// Fixtures del banco FollowFootsteps62: CTaskComplexFollowPedFootsteps::{ControlSubTask 0x695090, CreateSubTask 0x695E40, CreateNextSubTask 0x694EE0, CreateFirstSubTask 0x695000, MakeAbortable 0x694ED0}.
// Solo C++ estandar (sin tipos del juego): las incluyen LiveComparison.hpp/Core.hpp y tambien un programa autonomo de calibracion (fixture_mirror.py).
//
// Funciones (ids): 0 ControlSubTask, 1 CreateSubTask, 2 CreateNextSubTask, 3 CreateFirstSubTask, 4 MakeAbortable. 4096 casos por funcion.
// Resumen del desensamblado (detalle completo en la cabecera de fixture_oracle.py): this = ECX (ESI), argumentos de pila; MakeAbortable salta a la ranura 6 (+0x18) de la subtarea.
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace FollowFootsteps62Fix {
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
inline u32 Fb(float f) { return std::bit_cast<u32>(f); }
inline float Bf(u32 u) { return std::bit_cast<float>(u); }

constexpr unsigned kCasesPer = 4096;
constexpr unsigned kFns = 5;
enum : u32 { FnControl = 0, FnCreateSub = 1, FnNext = 2, FnFirst = 3, FnAbortable = 4 };

// ------------------------------------------------------------------------------------------------ disposicion de memoria (guarda | objeto | guarda)
constexpr u32 kThisOff = 0x20, kThisBytes = 0x20, kThisArena = 0x60;           // 0x20 + 0x20 + 0x20
constexpr u32 kSubAtThis = 8, kTargetAt = 0xC, kCreateTimerAt = 0x10, kLosTimerAt = 0x14, kRouteAt = 0x18, kMoveAt = 0x1C, kFlagsAt = 0x1E;
constexpr u32 kSubOff = 0x20, kSubBytes = 0x30, kSubArena = 0x70;               // 0x20 + 0x30 + 0x20 (subtarea falsa; GoToPoint: +8 = m_moveState)
constexpr u32 kSubMoveAt = 8;
constexpr u32 kPedOff = 0x40, kPedBytes = 0x480, kPedArena = 0x500;             // 0x40 + 0x480 + 0x40 (peaton y objetivo)
constexpr u32 kPlaceAt = 4, kMatAt = 0x14, kPedFlagsAt = 0x46C, kIntelAt = 0x47C;
constexpr u32 kMatOff = 0x10, kMatBytes = 0x48, kMatArena = 0x70;               // 0x10 + 0x48 + 0x18
constexpr u32 kMatPosAt = 0x30;
constexpr u32 kIntelOff = 0x20, kIntelBytes = 0x278, kIntelArena = 0x2B8;       // 0x20 + 0x278 + 0x20
constexpr u32 kCounterAt = 0x274, kMgrAt = 4;
constexpr u32 kRouteOff = 0x20, kRouteBytes = 0x64, kRouteArena = 0xA8;         // 0x20 + 0x64 + 0x24
constexpr u32 kGoOff = 0x10, kGoBytes = 0x30, kGoArena = 0x50;                  // 0x10 + 0x30 + 0x10 (tarea devuelta por GetSimplestActiveTask)
constexpr u32 kAllocOff = 0x20, kAllocBytes = 0x50, kAllocArena = 0x90;         // 0x20 + 0x50 + 0x20 (tarea que "reserva" operator new; la mayor mide 0x4C)
constexpr u32 kMaxEvents = 20;
constexpr u32 kVtSlots = 16;
constexpr u32 kTimerGlobal = 0xB7CB84;

// ------------------------------------------------------------------------------------------------ caso
struct C {
    u32 fn, family;
    u32 eax, edx, stack;
    // objeto this
    u32 tgtNull, routeNull, moveState, flagsByte, createTimerDelta, losTimerDelta, now;
    // peaton y objetivo
    u32 tgtMat, pedMat, tgtInVeh, tgtNoise, counter;
    u32 tgt[3], tgtAlt[3], ped[3], pedAlt[3];
    // ruta
    u32 count, ent[24];
    // falsos
    u32 subType, aliveRet, losRet, simplestNull, isGoToRet, abortRet, createKind, createVal, newNull;
    // CreateSubTask / MakeAbortable
    u32 type, prio, evPtr, pedArg;
};

// ------------------------------------------------------------------------------------------------ valores
inline float Coord(u32 n, u32 k) {
    static constexpr float nice[]{0.f, 1.f, -1.f, 100.5f, -250.25f, 1234.5f, 3000.f, -3000.f, 0.125f, -0.5f, 17.f, -42.75f};
    if (Rand(n, k) % 4 == 0) return nice[Rand(n, k + 1) % std::size(nice)];
    return float(i32(Rand(n, k + 2) % 2000001u) - 1000000) / 200.f;   // +-5000 con paso 0.005
}
inline u32 Special(u32 n, u32 k) {   // NaN canonico (positivo), +-inf, grandes exactos, pequenos
    static constexpr u32 sp[]{0x7FC00000u, 0x7F800000u, 0xFF800000u, 0x4E6E6B28u /*1e9*/, 0xCE6E6B28u, 0x4CBEBC20u /*1e8*/, 0x2EDBE6FFu /*1e-10*/};
    return sp[Rand(n, k) % std::size(sp)];
}
inline void SetV(u32* dst, double x, double y, double z) { dst[0] = Fb(float(x)); dst[1] = Fb(float(y)); dst[2] = Fb(float(z)); }
inline void RandV(u32* dst, u32 n, u32 k, float scale = 1.f) { for (u32 i = 0; i < 3; ++i) dst[i] = Fb(Coord(n, k + i * 3) * scale); }
inline void AddV(u32* out, const u32* p, const u32* d) { for (int i = 0; i < 3; ++i) out[i] = Fb(Bf(p[i]) + Bf(d[i])); }   // objetivo = peaton + delta (en float)

inline constexpr u32 kTypes[]{0x384u, 0xCBu, 0x1F4u, 0x38Bu, 0x516u, 0xC8u, 0u, 0x385u, 0x383u, 0xCAu, 0xCCu, 0x1F3u, 0x1F5u, 0x38Au, 0x38Cu, 0x517u, 0x515u,
                              0x80000384u, 0x1384u, 0x100000CBu, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x200u, 0x39Eu, 0x387u, 0x800u};
inline constexpr u32 kAl[]{0u, 1u, 2u, 0x80u, 0xFFu, 0x7Fu, 0x10u, 1u};
inline u32 Al(u32 n, u32 k, bool nonzero) {   // EAX de un bool devuelto: AL (cero o no) y, la mitad de las veces, basura en los 24 bits altos
    u32 al = nonzero ? 1u + Rand(n, k) % 255u : 0u;
    if (nonzero && Rand(n, k + 1) % 3 == 0) al = kAl[Rand(n, k + 2) % std::size(kAl)] | 1u;
    const u32 hi = Rand(n, k + 3) % 2 ? (Rand(n, k + 4) & 0xFFFFFF00u) : 0u;
    return hi | al;
}
inline u32 AlEven(u32 n, u32 k) {   // AL distinto de cero con el bit 0 a cero (2, 0x80...): el EXE guarda AL & 1
    static constexpr u32 even[]{2u, 0x80u, 0x10u, 0xFEu, 0x40u};
    const u32 hi = Rand(n, k + 3) % 2 ? (Rand(n, k + 4) & 0xFFFFFF00u) : 0u;
    return hi | even[Rand(n, k) % std::size(even)];
}
inline float ULP(float f, int steps) { for (int i = 0; i < std::abs(steps); ++i) f = std::nextafter(f, steps > 0 ? INFINITY : -INFINITY); return f; }
inline u32 PickType(u32 n, u32 k) { return kTypes[Rand(n, k) % std::size(kTypes)]; }

// Ruta alrededor del objetivo T: last = T - vlen*dirV; prev = last - wlen*dirW (angulo theta respecto de dirV); el resto cercano a T; fuera del contador, basura.
inline void MakeRoute(C& c, u32 n, bool edgeZero) {
    static constexpr double vlen[]{0.05, 0.3, 0.3499, 0.35, 0.3501, 0.36, 0.5, 2.0, 10.0, 300.0};
    static constexpr double wlen[]{0.0, 1e-3, 0.5, 3.0, 40.0, 1.0};
    static constexpr double theta[]{0, 5, 10, 18, 18.1949, 18.2, 20, 45, 90, 135, 180, 270, 17.9};
    static constexpr double phi[]{0, 30, 45, 90, 180, 210, 300};
    static constexpr double psi[]{0, 0, 0, 30, -45, 90};
    constexpr double kRad = 3.14159265358979323846 / 180.0;
    const double lv = vlen[Rand(n, 200) % std::size(vlen)], lw = wlen[Rand(n, 201) % std::size(wlen)];
    const double th = theta[Rand(n, 202) % std::size(theta)] * kRad;
    const double ph = (Rand(n, 203) % 4 ? phi[Rand(n, 204) % std::size(phi)] : double(Rand(n, 205) % 360)) * kRad;
    const double ps = psi[Rand(n, 206) % std::size(psi)] * kRad;
    const double T[3]{double(Bf(c.tgt[0])), double(Bf(c.tgt[1])), double(Bf(c.tgt[2]))};
    const double dv[3]{std::cos(ph) * std::cos(ps), std::sin(ph) * std::cos(ps), std::sin(ps)};
    double dw[3]{std::cos(ph + th) * std::cos(ps), std::sin(ph + th) * std::cos(ps), std::sin(ps)};
    if (Rand(n, 207) % 5 == 0) { dw[0] = dv[0]; dw[1] = dv[1]; dw[2] = dv[2]; }
    double last[3], prev[3];
    for (int i = 0; i < 3; ++i) { last[i] = T[i] - lv * dv[i]; prev[i] = last[i] - lw * dw[i]; }
    if (edgeZero) { last[0] = -double(0.35f); last[1] = 0; last[2] = 0; }
    for (u32 i = 0; i < 24; ++i) c.ent[i] = Fb(Coord(n, 300 + i));
    const u32 cnt = c.count;
    for (u32 i = 0; i + 2 < cnt; ++i) for (int j = 0; j < 3; ++j) c.ent[3 * i + j] = Fb(float(T[j]) + Coord(n, 400 + 3 * i + j) * 0.01f);
    if (cnt >= 1) for (int j = 0; j < 3; ++j) c.ent[3 * (cnt - 1) + j] = Fb(float(last[j]));
    if (cnt >= 2) for (int j = 0; j < 3; ++j) c.ent[3 * (cnt - 2) + j] = Fb(float(prev[j]));
    if (Rand(n, 208) % 40 == 0 && cnt >= 1) c.ent[3 * (cnt - 1) + Rand(n, 209) % 3] = Rand(n, 211) % 2 ? 0x7F800000u : 0x7FC00000u;   // inf/NaN en el ultimo punto
}

inline void SetPositions(C& c, u32 n) {
    RandV(c.pedAlt, n, 600); RandV(c.tgtAlt, n, 610);
    c.tgtMat = Rand(n, 620) % 2; c.pedMat = Rand(n, 621) % 2;
    RandV(c.ped, n, 630);
    if (Rand(n, 640) % 3 == 0) { c.ped[0] = c.ped[1] = c.ped[2] = 0u; }   // con el peaton en el origen el delta es exacto
}

// Delta D = objetivo - peaton segun el regimen: 0 |D| < 1.4; 1 borde de 1.4; 2 rango normal (1.45..7.85); 3 borde de 8; 4 lejos; 5 2D < 1 con |dz| > 2; 6 bordes de esa condicion; 7 diminuto; 8 |D|^2 alrededor de 1.4
inline void ControlD(C& c, u32 n, u32 regime) {
    u32 d[3]{};
    const u32 ax = Rand(n, 700) % 3;
    switch (regime) {
    case 0: { const double r = 1.4 * (Rand(n, 701) % 1000) / 1000.0, a = (Rand(n, 702) % 628) / 100.0; SetV(d, r * std::cos(a), r * std::sin(a) * 0.7, r * std::sin(a) * 0.7); } break;
    case 1: { float v = ULP(1.4f, int(Rand(n, 703) % 5) - 2); d[ax] = Fb(Rand(n, 704) % 2 ? v : -v); } break;
    case 2: { const double r = 1.45 + 6.4 * (Rand(n, 705) % 1000) / 1000.0, a = (Rand(n, 706) % 628) / 100.0, b = (Rand(n, 707) % 314) / 100.0 - 1.57;
              SetV(d, r * std::cos(a) * std::cos(b), r * std::sin(a) * std::cos(b), r * std::sin(b)); } break;
    case 3: { float v = ULP(8.f, int(Rand(n, 708) % 5) - 2); d[ax] = Fb(Rand(n, 709) % 2 ? v : -v); } break;
    case 4: { const double r = 8.2 + 100.0 * (Rand(n, 710) % 1000) / 1000.0, a = (Rand(n, 711) % 628) / 100.0, b = (Rand(n, 712) % 314) / 100.0 - 1.57;
              SetV(d, r * std::cos(a) * std::cos(b), r * std::sin(a) * std::cos(b), r * std::sin(b)); } break;
    case 5: { const double h = 0.05 + 0.9 * (Rand(n, 713) % 1000) / 1000.0, a = (Rand(n, 714) % 628) / 100.0; double z = 2.0 + 5.8 * (Rand(n, 715) % 1000) / 1000.0;
              SetV(d, h * std::cos(a), h * std::sin(a), Rand(n, 716) % 2 ? z : -z); } break;
    case 6: { static constexpr float xs[]{1.0f, 0.99999994f, 1.0000001f, 0.5f, 0.6f, 0.0f};
              static constexpr float zs[]{2.0f, 2.0000002f, 1.9999999f, 2.5f, 3.0f, -2.0f, -2.0000002f};
              SetV(d, xs[Rand(n, 717) % std::size(xs)], 0.0, zs[Rand(n, 718) % std::size(zs)]); if (Rand(n, 719) % 2) std::swap(d[0], d[1]); } break;
    case 8: { const double r = std::sqrt(1.4) * (1.0 + double(int(Rand(n, 740) % 9) - 4) * 1.5e-7), a = (Rand(n, 741) % 628) / 100.0, b = (Rand(n, 742) % 314) / 100.0 - 1.57;
              SetV(d, r * std::cos(a) * std::cos(b), r * std::sin(a) * std::cos(b), r * std::sin(b)); } break;   // |D|^2 alrededor de 1.4 (CreateNext/First comparan la suma con 1.4)
    default: RandV(d, n, 720, 0.002f); break;
    }
    AddV(c.tgt, c.ped, d);
}

// ------------------------------------------------------------------------------------------------ ControlSubTask
// Familias (n % 16, 256 casos cada una):
//  0 sin objetivo / sin ruta / objetivo muerto -> FINISHED   1 subtarea SEEK -> devuelve la subtarea      2 objetivo en vehiculo -> SEEK   3 contador de la inteligencia en el borde de 30 -> HIT_HEAD
//  4 temporizador de linea de vision (500) y AL del resultado  5 muy cerca (|D|^2 < 1.96) y borde exacto   6 lejos (> 64) y borde exacto    7 2D < 1 con |dz| > 2 y bordes
//  8 rango normal, indicador alzado: ruta vaciada, UpdatePoint    9 rango normal, indicador bajado: temporizador de 166, ruta llena y STAND con > 1 puntos
//  10 paso > 0.1225 con >= 2 puntos: coseno 0.95   11 acumulacion de distancias y umbral de 12.25; GoToPoint recibe el estado   12 ruta vacia   13..15 mezcla
inline C MakeControl(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.fn = FnControl; c.family = f;
    c.eax = Rand(n, 70); c.edx = Rand(n, 71); c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    static constexpr u32 nows[]{100000u, 1000u, 0xFFFFFF00u, 5u, 0x7FFFFFFFu, 0x80000000u, 123456789u};
    c.now = nows[Rand(n, 1) % std::size(nows)];
    SetPositions(c, n);
    static constexpr u32 lDelta[]{0u, 1u, 499u, 500u, 501u, 502u, 100000u, 0xFFFFFFFFu, 0xFFFFFE0Cu /*now+500*/, 250u, 10000u};
    static constexpr u32 cDelta[]{0u, 1u, 165u, 166u, 167u, 332u, 1000u, 0xFFFFFFFFu, 0xFFFFFF5Au /*now+166*/, 100u, 100000u};
    c.losTimerDelta = lDelta[Rand(n, 2) % std::size(lDelta)];
    c.createTimerDelta = cDelta[Rand(n, 3) % std::size(cDelta)];
    c.moveState = Rand(n, 4) % 3 ? (Rand(n, 5) % 8) : (Rand(n, 6) & 0xFFFFu);
    c.flagsByte = (Rand(n, 7) & 0xFEu) | (Rand(n, 8) % 2);
    c.tgtInVeh = 0; c.tgtNoise = Rand(n, 9);
    static constexpr u32 ctrs[]{0u, 1u, 29u, 30u, 31u, 32u, 100u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, 0xFFFFFFE2u};
    c.counter = Rand(n, 10) % 3 ? 0u : ctrs[Rand(n, 11) % std::size(ctrs)];
    c.subType = Rand(n, 12) % 3 == 0 ? (Rand(n, 13) % 2 ? 0xCBu : 0x384u) : PickType(n, 14);
    c.aliveRet = Al(n, 15, true);
    c.losRet = Al(n, 18, Rand(n, 20) % 2);
    c.simplestNull = Rand(n, 21) % 4 == 0;
    c.isGoToRet = Al(n, 22, Rand(n, 24) % 2);
    c.abortRet = Al(n, 25, Rand(n, 27) % 2);
    c.createKind = Rand(n, 28) % 4; c.createVal = 0xC0DE0000u + Rand(n, 29) % 0x10000u;
    c.count = Rand(n, 30) % 9;
    ControlD(c, n, 2 + (Rand(n, 31) % 5 == 0 ? Rand(n, 32) % 5 : 0));
    const auto notSeek = [&](u32 repl) { if (c.subType == 0x38Bu) c.subType = repl; };
    switch (f) {
    case 0: {
        const u32 w = Rand(n, 40) % 4;
        if (w == 0) c.tgtNull = 1; else if (w == 1) c.routeNull = 1; else if (w == 2) c.aliveRet = Al(n, 15, false); else { c.tgtNull = Rand(n, 41) % 2; c.routeNull = 1; }
        break; }
    case 1: c.subType = 0x38Bu; break;
    case 2: c.tgtInVeh = 1; notSeek(0xCBu); break;
    case 3: c.counter = ctrs[Rand(n, 50) % std::size(ctrs)]; if (Rand(n, 51) % 2) c.counter = 28u + Rand(n, 52) % 5; notSeek(0xCBu); break;
    case 4: c.losTimerDelta = lDelta[Rand(n, 53) % std::size(lDelta)]; notSeek(0xCBu); c.losRet = Rand(n, 54) % 3 == 0 ? AlEven(n, 55) : Al(n, 56, Rand(n, 57) % 2);
            ControlD(c, n, Rand(n, 58) % 7 == 0 ? 0 : 2); break;
    case 5: ControlD(c, n, Rand(n, 59) % 2 ? 1 : 0); notSeek(0xCBu); if (Rand(n, 60) % 2) c.subType = 0xCBu; break;
    case 6: ControlD(c, n, Rand(n, 61) % 2 ? 3 : 4); notSeek(0xCBu); break;
    case 7: ControlD(c, n, Rand(n, 62) % 2 ? 5 : 6); notSeek(0xCBu); break;
    case 8: ControlD(c, n, 2); c.flagsByte |= 1u; c.losTimerDelta = lDelta[Rand(n, 63) % std::size(lDelta)]; c.losRet = Al(n, 64, true); notSeek(0x384u); break;
    case 9: ControlD(c, n, 2); c.flagsByte &= ~1u; c.losTimerDelta = 100u; c.createTimerDelta = cDelta[Rand(n, 66) % std::size(cDelta)];
            notSeek(0xCBu); if (Rand(n, 67) % 2) c.subType = 0xCBu; break;
    case 10: ControlD(c, n, 2); c.flagsByte &= ~1u; c.losTimerDelta = 100u; c.createTimerDelta = Rand(n, 68) % 8 == 0 ? 165u : 1000u + Rand(n, 69) % 1000u;
             c.count = Rand(n, 72) % 3 ? 2 + Rand(n, 74) % 6 : 1 + Rand(n, 73) % 7; notSeek(0xCBu); break;
    case 11: ControlD(c, n, 2); c.flagsByte &= ~1u; c.losTimerDelta = 100u; c.createTimerDelta = 1000u + Rand(n, 75) % 1000u; c.count = Rand(n, 76) % 8;
             c.subType = Rand(n, 77) % 2 ? 0x384u : PickType(n, 78); notSeek(0x384u); break;
    case 12: c.count = 0; ControlD(c, n, 2); notSeek(0xCBu); if (Rand(n, 79) % 2) c.flagsByte &= ~1u; break;
    default:
        c.tgtInVeh = Rand(n, 80) % 8 == 0; c.tgtNull = Rand(n, 81) % 24 == 0; c.routeNull = Rand(n, 82) % 24 == 0; if (Rand(n, 83) % 24 == 0) c.aliveRet = Al(n, 15, false);
        ControlD(c, n, Rand(n, 84) % 7);
        if (Rand(n, 85) % 20 == 0) c.tgt[Rand(n, 86) % 3] = Special(n, 87);
        if (Rand(n, 88) % 30 == 0) c.ped[Rand(n, 89) % 3] = Special(n, 90);
        if (Rand(n, 92) % 3 == 0) notSeek(0xCBu);
        break;
    }
    const bool zeroEdge = f == 10 && Rand(n, 91) % 4 == 0;
    if (zeroEdge) { SetV(c.ped, 3.0, 0.0, 0.0); c.tgt[0] = c.tgt[1] = c.tgt[2] = 0u; }   // T = 0 (peaton en (3,0,0): rango normal) y last = -0.35f: v.x == 0.35f exacto
    MakeRoute(c, n, zeroEdge);
    return c;
}

// ------------------------------------------------------------------------------------------------ CreateSubTask
inline C MakeCreateSub(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.fn = FnCreateSub; c.family = f;
    c.eax = Rand(n, 70); c.edx = Rand(n, 71); c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    c.now = 100000u; SetPositions(c, n); ControlD(c, n, 2);
    c.moveState = Rand(n, 4) % 3 ? (Rand(n, 5) % 8) : (Rand(n, 6) & 0xFFFFu);
    c.flagsByte = Rand(n, 7) & 0xFFu; c.tgtNoise = Rand(n, 9); c.tgtInVeh = Rand(n, 10) % 2;
    c.type = PickType(n, 11);
    c.count = Rand(n, 30) % 9;
    c.newNull = Rand(n, 12) % 8 == 0;
    switch (f) {
    case 0: c.tgtNull = 1; break;
    case 1: c.routeNull = 1; break;
    case 2: case 3: case 4: c.type = 0x384u; c.newNull = Rand(n, 13) % 6 == 0; c.count = f == 2 ? 1 + Rand(n, 14) % 8 : (f == 3 ? 8 : Rand(n, 15) % 9); break;
    case 5: case 6: c.type = 0xCBu; break;
    case 7: case 8: c.type = 0x1F4u; break;
    case 9: case 10: case 11: c.type = 0x38Bu; c.tgtInVeh = f == 9 ? 1 : (f == 10 ? 0 : Rand(n, 16) % 2); break;
    case 12: c.type = 0x516u; break;
    default: break;
    }
    if (f >= 2 && Rand(n, 17) % 40 == 0) c.newNull = 1;
    MakeRoute(c, n, false);
    return c;
}

// ------------------------------------------------------------------------------------------------ CreateNextSubTask / CreateFirstSubTask
inline C MakeNext(u32 n, bool first) {
    C c{};
    const u32 f = n % 16;
    c.fn = first ? FnFirst : FnNext; c.family = f;
    c.eax = Rand(n, 70); c.edx = Rand(n, 71); c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    c.now = 100000u; SetPositions(c, n);
    c.subType = PickType(n, 14);
    c.count = Rand(n, 30) % 9;
    c.routeNull = Rand(n, 31) % 6 == 0;
    c.createKind = Rand(n, 28) % 4; c.createVal = 0xC0DE0000u + Rand(n, 29) % 0x10000u;
    c.tgtNoise = Rand(n, 9);
    static constexpr u32 regimes[]{2, 2, 8, 8, 0, 7, 1, 3, 4, 8, 8};
    ControlD(c, n, regimes[Rand(n, 32) % std::size(regimes)]);
    static constexpr u32 nxt[]{200u, 0xCBu, 0x384u, 0x1F4u, 0x38Bu, 0x516u, 0u};
    switch (f) {
    case 0: c.tgtNull = 1; break;
    case 1: c.subType = 200u; break;
    case 2: c.subType = 0xCBu; c.routeNull = 0; c.count = 1 + Rand(n, 34) % 8; break;
    case 3: c.subType = 0x384u; c.routeNull = 0; c.count = 1 + Rand(n, 34) % 8; break;
    case 4: c.subType = Rand(n, 35) % 2 ? 0xCBu : 0x384u; c.count = 0; c.routeNull = Rand(n, 36) % 2; break;
    case 5: c.subType = 0x1F4u; break;
    case 6: c.subType = 0x38Bu; ControlD(c, n, Rand(n, 37) % 2 ? 8 : 0); break;
    case 7: c.subType = 0x38Bu; ControlD(c, n, 8); break;
    case 8: c.subType = 0x38Bu; ControlD(c, n, Rand(n, 37) % 2 ? 1 : 8); break;
    case 9: c.subType = 0x516u; break;
    case 10: c.subType = nxt[Rand(n, 39) % std::size(nxt)]; break;
    default: break;
    }
    if (first && f == 6) ControlD(c, n, 8);
    if (Rand(n, 92) % 24 == 0) c.tgt[Rand(n, 93) % 3] = Special(n, 94);
    if (Rand(n, 95) % 40 == 0) c.ped[Rand(n, 96) % 3] = Special(n, 97);
    MakeRoute(c, n, false);
    return c;
}

// ------------------------------------------------------------------------------------------------ MakeAbortable
inline C MakeAbortable(u32 n) {
    C c{};
    c.fn = FnAbortable; c.family = n % 16;
    c.eax = Rand(n, 70); c.edx = Rand(n, 71); c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    c.now = 100000u; SetPositions(c, n);
    c.abortRet = Rand(n, 25) % 5 == 0 ? 0u : Al(n, 26, true);
    if (Rand(n, 27) % 3 == 0) c.abortRet = Al(n, 28, false);
    static constexpr u32 prios[]{0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0x80000000u};
    c.prio = Rand(n, 29) % 2 ? prios[Rand(n, 30) % std::size(prios)] : Rand(n, 31);
    c.evPtr = Rand(n, 32) % 3 == 0 ? 0u : Rand(n, 33);
    c.pedArg = Rand(n, 34) % 4 == 0 ? 0u : Rand(n, 35);
    return c;
}

inline C MakeC(u32 fn, u32 n) {
    switch (fn) {
    case FnControl: return MakeControl(n);
    case FnCreateSub: return MakeCreateSub(n);
    case FnNext: return MakeNext(n, false);
    case FnFirst: return MakeNext(n, true);
    default: return MakeAbortable(n);
    }
}
}  // namespace FollowFootsteps62Fix

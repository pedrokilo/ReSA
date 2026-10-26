#pragma once
// Fixtures del banco LeaveCarAbort70 (lote 70): CTaskComplexLeaveCar::MakeAbortable (0x641100), 16384 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskComplexLeaveCar::MakeAbortable  0x641100  thiscall, ECX = this (tarea), (CPed*, int32 prioridad, const CEvent*), `ret 0xC`, AL = resultado
//     (hook CTaskComplexLeaveCar / MakeAbortable, tipo V: vtable 0x86E828, ranura 6)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia).
// GetTaskType de la subtarea y GetEventType del evento se llaman varias veces: el falso devuelve st[i] / et[i] segun el NUMERO DE LLAMADA (indice i, saturado en 5),
// de modo que una llamada de mas o de menos en la DLL cambia la ruta o la traza.
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

namespace Lc70Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

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
inline float F(u32 b) { return std::bit_cast<float>(b); }
inline u32 B(float f) { return std::bit_cast<u32>(f); }

struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    std::string End() const { return s + "}"; }
};

// Sin NaN senalizantes (el x87 los silencia al copiar): solo silenciosos
inline constexpr std::array<u32, 6> kNan{0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0xFFC00001u, 0x7FC12345u, 0xFFC54321u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 8> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
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
// Valor de un byte: canonico o notable (banderas con signo)
inline u32 ByteOf(u32 n, u32 k, std::initializer_list<u32> special) {
    const u32 r = Rand(n, k) % 8;
    if (r < 5) return special.begin()[Rand(n, k + 1) % special.size()] & 0xFFu;
    return Rand(n, k + 2) & 0xFFu;
}
// float adyacente (en bits ordenados) a v, k pasos (k puede ser negativo)
inline u32 Step(u32 v, int k) {
    if (((v >> 23) & 0xFFu) == 0xFFu) return v;                       // NaN/inf: sin cambio
    i32 key = (v >> 31) ? -i32(v & 0x7FFFFFFFu) : i32(v);              // clave ordenada: los negativos van al reves
    key += k;
    return key < 0 ? 0x80000000u | u32(-key) : u32(key);
}

// Constantes del EXE (leidas de 0x641100): umbral de velocidad del avion 0x858CC4 = 0.2f (0x3E4CCCCD), umbral de caida en el agua 0x858EE4 = -0.3f (0xBE99999A)
inline constexpr u32 kSpeedLimit = 0x3E4CCCCDu, kFallLimit = 0xBE99999Au;
// Tipos de tarea (eTaskType) y de evento (eEventType) que compara la funcion
constexpr u32 kTDie = 0xD4, kTWait = 0x329, kTGetUp = 0xCE, kTJump = 0x32E, kTDrive = 0x2CF;
constexpr u32 kEDamage = 9, kEAir = 0x21, kEWater = 0x42;
constexpr u32 kParachute = 0x2E;

// ------------------------------------------------------------------------------------------------ arenas (guarda + ventana + guarda; el ped lleva un prefijo mayor:
// el indice de arma con signo lee hasta -128 * 0x1C bytes antes del arreglo de armas, que empieza en +0x5A0)
constexpr u32 kGuard = 0x40;
constexpr u32 kTaskWindow = 0x34, kTaskArena = kGuard + kTaskWindow + kGuard;               // 0x40 + 0x34 + 0x40 = 0xB4     CTaskComplexLeaveCar
constexpr u32 kSubWindow = 0x40, kSubArena = kGuard + kSubWindow + kGuard;                  // 0x40 + 0x40 + 0x40 = 0xC0     subtarea
constexpr u32 kVehWindow = 0x600, kVehArena = kGuard + kVehWindow + kGuard;                 // 0x40 + 0x600 + 0x40 = 0x680   vehiculo falso (+0x44 velocidad, +0x485 contador, +0x594 subtipo)
constexpr u32 kPedPrefix = 0x900, kPedWindow = 0x2000, kPedArena = kPedPrefix + kPedWindow + kGuard;   // 0x900 + 0x2000 + 0x40 = 0x2940  ped falso
constexpr u32 kMatWindow = 0x48, kMatArena = kGuard + kMatWindow + kGuard;                  // 0x40 + 0x48 + 0x40 = 0xC8     matriz falsa (posicion en +0x30)
constexpr u32 kEvWindow = 0x50, kEvArena = kGuard + kEvWindow + kGuard;                     // 0x40 + 0x50 + 0x40 = 0xD0     evento falso (+0xC, +0x40)
static_assert(kTaskArena == 0xB4 && kSubArena == 0xC0 && kVehArena == 0x680 && kPedArena == 0x2940 && kMatArena == 0xC8 && kEvArena == 0xD0, "arenas");

// ================================================================================================ 0: MakeAbortable
struct C {
    u32 family{}, vehNull{}, prio{}, subNull{}, evNull{};
    u32 st0{}, st1{}, st2{}, st3{}, st4{}, st5{};     // GetTaskType de la subtarea por numero de llamada
    u32 et0{}, et1{}, et2{}, et3{}, et4{}, et5{};     // GetEventType del evento por numero de llamada
    u32 killed{}, grp{}, abortRet{}, pedFlags{};
    u32 door{}, sens{}, dfs{}, ngs{}, inAir{}, vng{}, vsub{}, wheels{}, mag{}, vz{}, slot{}, wtype{}, hasMat{}, clump{}, isPlayer{}, pvl{}, cdoor{};
    u32 px{}, py{}, pz{}, mx{}, my{}, mz{};
    u32 eax{}, edx{}, stack{}, seed{};
    u32 St(unsigned i) const { const u32 a[6]{st0, st1, st2, st3, st4, st5}; return a[i < 5 ? i : 5]; }
    u32 Et(unsigned i) const { const u32 a[6]{et0, et1, et2, et3, et4, et5}; return a[i < 5 ? i : 5]; }
};
inline void SetSt(C& c, u32 v) { c.st0 = c.st1 = c.st2 = c.st3 = c.st4 = c.st5 = v; }
inline void SetEt(C& c, u32 v) { c.et0 = c.et1 = c.et2 = c.et3 = c.et4 = c.et5 = v; }
// Tipos / eventos que ninguna comparacion de la funcion acepta
inline constexpr std::array<u32, 14> kOtherTypes{0u, 1u, 0xD3u, 0xD5u, 0x328u, 0x32Au, 0xCFu, 0x32Du, 0x32Fu, 0x2CEu, 0x2D0u, 0xFFFFFFFFu, 0x100u, 0x80000000u};
inline constexpr std::array<u32, 12> kOtherEvents{0u, 1u, 8u, 0xAu, 0x20u, 0x22u, 0x41u, 0x43u, 0xFFFFFFFFu, 0x80000000u, 0x100u, 0x7Fu};
inline u32 PickHigh(u32 base, u32 n, u32 k) {   // el valor tal cual o con bits altos distintos (el EXE compara los 32 bits)
    switch (Rand(n, k) % 12) { case 0: return base | 0x10000u; case 1: return base | 0x80000000u; case 2: return base | 0x100u; default: return base; }
}
inline u32 RandType(u32 n, u32 k) {
    switch (Rand(n, k) % 8) {
    case 0: return kTDie; case 1: return kTWait; case 2: return kTGetUp; case 3: return kTJump; case 4: return kTDrive;
    default: return Pick(kOtherTypes, n, k + 1);
    }
}
inline u32 RandEvent(u32 n, u32 k) {
    switch (Rand(n, k) % 8) {
    case 0: case 1: return kEDamage; case 2: case 3: return kEAir; case 4: return kEWater;
    default: return Pick(kOtherEvents, n, k + 1);
    }
}
inline bool IsKnownType(u32 v) { return v == kTDie || v == kTWait || v == kTGetUp || v == kTJump || v == kTDrive; }
inline C MakeC(u32 n) {
    C c; const u32 f = n % 16; c.family = f;
    // ---- base aleatoria
    c.vehNull = Rand(n, 1) % 20 == 0;
    c.prio = Rand(n, 2) % 8 == 0 ? Pick(std::array<u32, 8>{0u, 3u, 4u, 0x100u, 0x80000000u, 0xFFFFFFFFu, 0x80000001u, 0x10001u}, n, 3) : 1 + Rand(n, 4) % 2;
    c.subNull = 0;
    c.evNull = Rand(n, 5) % 6 == 0;
    SetSt(c, RandType(n, 6));
    { u32* t = &c.st0; const u32 base = t[0]; for (unsigned i = 1; i < 6; ++i) t[i] = Rand(n, 10 + i) % 6 == 0 ? RandType(n, 20 + i) : base; }
    SetEt(c, RandEvent(n, 7));
    { u32* t = &c.et0; const u32 base = t[0]; for (unsigned i = 1; i < 6; ++i) t[i] = Rand(n, 30 + i) % 5 == 0 ? RandEvent(n, 40 + i) : base; }
    c.killed = ByteOf(n, 50, {0u, 1u, 1u, 2u, 0x80u, 0xFFu});
    c.grp = ByteOf(n, 51, {0u, 1u, 1u, 2u, 0x80u, 0xFFu});
    c.abortRet = BoolRet(n, 52, Rand(n, 53) % 2);
    c.pedFlags = (Rand(n, 54) & ~0x100u) | (Rand(n, 55) % 2 ? 0x100u : 0u);
    c.door = Rand(n, 56) % 3 == 0 ? 0u : Pick(std::array<u32, 12>{8u, 9u, 10u, 11u, 18u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x100u, 12u, 0x10000008u}, n, 57);
    c.sens = ByteOf(n, 58, {0u, 1u, 1u, 2u, 0x80u, 0xFFu});
    c.dfs = ByteOf(n, 59, {0u, 1u, 2u, 4u, 8u, 0x80u, 0xFFu, 0x0Fu});
    c.ngs = ByteOf(n, 60, {0u, 1u, 1u, 2u, 3u, 0x80u, 0xFFu});
    c.inAir = ByteOf(n, 61, {0u, 1u, 2u, 0x80u, 0xFFu});
    c.vng = ByteOf(n, 62, {0u, 1u, 2u, 3u, 0x80u, 0xFFu, 4u});
    c.vsub = Rand(n, 63) % 3 == 0 ? 4u : Pick(std::array<u32, 12>{0u, 1u, 2u, 3u, 5u, 6u, 8u, 0xFFFFFFFFu, 0x100u, 0x80000004u, 4u, 7u}, n, 64);
    c.wheels = Rand(n, 65) % 3 == 0 ? 0u : Pick(std::array<u32, 8>{1u, 4u, 0x100u, 0x80000000u, 0xFFFFFFFFu, 2u, 0x10000u, 0x01000000u}, n, 66);
    c.mag = Rand(n, 67) % 3 == 0 ? Step(kSpeedLimit, int(Rand(n, 68) % 9) - 4) : Rand(n, 69) % 4 == 0 ? Pick(kNan, n, 70) : Rand(n, 71) % 6 == 0 ? Pick(kTiny, n, 72) : Rand(n, 73) % 6 == 0 ? Pick(kInf, n, 74) : RandFloat(n, 75, -8, 4);
    c.vz = Rand(n, 76) % 3 == 0 ? Step(kFallLimit, int(Rand(n, 77) % 9) - 4) : Rand(n, 78) % 6 == 0 ? Pick(kNan, n, 79) : Rand(n, 80) % 6 == 0 ? Pick(kTiny, n, 81) : Rand(n, 82) % 8 == 0 ? Pick(kInf, n, 83) : RandFloat(n, 84, -8, 4);
    c.slot = Rand(n, 85) % 3 == 0 ? Pick(std::array<u32, 12>{0u, 1u, 12u, 13u, 0x7Fu, 0x80u, 0xFFu, 0xF5u, 0xF4u, 0xFEu, 3u, 0x8Au}, n, 86) : Rand(n, 87) % 13;
    if (i8(c.slot) == -11) c.slot = 0xF4;   // -11 * 0x1C + 0x5A0 = 0x46C: solaparia con las banderas del ped
    c.wtype = Rand(n, 88) % 3 ? kParachute : (Rand(n, 89) % 2 ? Pick(std::array<u32, 8>{0u, 1u, 0x2Du, 0x2Fu, 0x1000002Eu, 0x8000002Eu, 0xFFFFFFFFu, 0x12Eu}, n, 90) : Rand(n, 91) % 47);
    c.hasMat = Rand(n, 92) % 2;
    c.clump = SeedReg(n, 93);
    c.isPlayer = BoolRet(n, 94, Rand(n, 95) % 2);
    c.pvl = BoolRet(n, 96, Rand(n, 97) % 4 == 0);
    c.cdoor = Pick(std::array<u32, 10>{0xFFFFFFFFu, 8u, 9u, 10u, 11u, 0u, 18u, 0x7FFFFFFFu, 0x80000000u, 1u}, n, 98);
    c.px = RandFloat(n, 99, -3, 12); c.py = RandFloat(n, 100, -3, 12); c.pz = RandFloat(n, 101, -3, 12);
    c.mx = RandFloat(n, 102, -3, 12); c.my = RandFloat(n, 103, -3, 12); c.mz = RandFloat(n, 104, -3, 12);

    // ---- familias: fuerzan una ruta distinta de la funcion (el resto de campos siguen aleatorios)
    auto notKnown = [&](u32 k) { const u32 v = Pick(kOtherTypes, n, k); return IsKnownType(v) ? 0x77u : v; };
    auto prioUrgent = [&] { c.prio = 1; c.vehNull = 0; c.subNull = 0; };
    // la subtarea es de salto (0x32E) en la 4a llamada, sin pasar por 0xD4, 0x329 ni 0xCE antes (las 3 primeras llamadas devuelven tipos ajenos)
    auto jumpTypes = [&] {
        c.st0 = notKnown(110); c.st1 = notKnown(111); c.st2 = notKnown(112); c.st3 = c.st4 = c.st5 = kTJump;
        if (Rand(n, 113) % 8 == 0) c.st3 = PickHigh(kTJump, n, 114);
    };
    auto jumpEvent = [&](u32 a0, u32 a1, u32 a2, u32 a3, u32 a4) { c.et0 = a0; c.et1 = a1; c.et2 = a2; c.et3 = a3; c.et4 = c.et5 = a4; c.evNull = 0; };
    auto parachute = [&](bool yes) { c.wtype = yes ? kParachute : Pick(std::array<u32, 7>{0u, 1u, 0x2Du, 0x2Fu, 0x1000002Eu, 0x8000002Eu, 0x12Eu}, n, 120); };
    switch (f) {
    case 0: c.vehNull = 1; if (Rand(n, 121) % 4 == 0) c.subNull = 1; break;                                                     // sin vehiculo: devuelve true
    case 1: c.vehNull = 0; c.prio = Pick(std::array<u32, 12>{0u, 3u, 4u, 0x100u, 0x80000000u, 0xFFFFFFFFu, 0x80000001u, 0x10001u, 0x10002u, 0x10000u, 0x20000000u, 5u}, n, 122);
            c.subNull = Rand(n, 123) % 2; break;                                                                                // prioridad ni 1 ni 2: false
    case 2: c.vehNull = 0; c.prio = 2; c.subNull = 0; c.pedFlags &= ~0x100u; break;                                            // inmediata, ped fuera del coche
    case 3: c.vehNull = 0; c.prio = 2; c.subNull = 0; c.pedFlags |= 0x100u; if (c.door == 0) c.door = Pick(std::array<u32, 6>{8u, 9u, 10u, 11u, 0xFFFFFFFFu, 18u}, n, 126); break;   // inmediata, en el coche, puerta fijada
    case 4: c.vehNull = 0; c.prio = 2; c.subNull = 0; c.pedFlags |= 0x100u; c.door = 0; break;                                  // inmediata, en el coche, puerta por calcular
    case 5: prioUrgent(); c.st0 = kTDie; c.st1 = Rand(n, 128) % 3 ? kTDrive : notKnown(129); c.st2 = c.st3 = c.st4 = c.st5 = c.st1; break;   // 0xD4 y luego 0x2CF: false
    case 6: prioUrgent(); c.st0 = kTDie; c.st1 = notKnown(131); c.st2 = c.st3 = c.st4 = c.st5 = c.st1; break;                 // 0xD4: aborta la subtarea y suelta el vehiculo si acepta
    case 7: prioUrgent(); c.st0 = notKnown(132); c.st1 = kTWait; c.st2 = Rand(n, 133) % 2 ? kTGetUp : notKnown(134);           // 0x329 con eventos de dano / que mataron
            c.et0 = Rand(n, 135) % 4 ? kEDamage : RandEvent(n, 136); c.et1 = Rand(n, 137) % 3 ? kEDamage : RandEvent(n, 138);
            c.killed = Rand(n, 139) % 2 ? ByteOf(n, 140, {1u, 2u, 0x80u, 0xFFu}) : ByteOf(n, 141, {0u, 0u, 1u}); c.grp = Rand(n, 142) % 2 ? ByteOf(n, 143, {1u, 2u, 0x80u, 0xFFu}) : ByteOf(n, 144, {0u, 0u, 1u});
            if (Rand(n, 145) % 6 == 0) c.evNull = 1; break;
    case 8: prioUrgent(); c.st0 = notKnown(146); c.st1 = Rand(n, 147) % 3 ? notKnown(148) : kTWait; c.st2 = kTGetUp; c.st3 = c.st4 = c.st5 = Rand(n, 149) % 2 ? kTGetUp : kTJump;   // 0xCE con eventos de dano / que mataron
            c.et0 = Rand(n, 150) % 4 ? kEDamage : RandEvent(n, 151); c.et1 = Rand(n, 152) % 4 ? kEDamage : RandEvent(n, 153); c.et2 = Rand(n, 154) % 4 ? kEDamage : RandEvent(n, 155);
            c.killed = Rand(n, 156) % 2 ? ByteOf(n, 157, {1u, 2u, 0x80u, 0xFFu}) : ByteOf(n, 158, {0u, 0u, 1u}); c.grp = Rand(n, 159) % 2 ? ByteOf(n, 160, {1u, 2u, 0x80u, 0xFFu}) : ByteOf(n, 161, {0u, 0u, 1u});
            if (Rand(n, 162) % 6 == 0) c.evNull = 1; break;
    case 9: prioUrgent(); jumpTypes();                                                                                          // salto sin evento de aire/agua: cae al final (0x2CF) o false
            jumpEvent(Pick(kOtherEvents, n, 164), Pick(kOtherEvents, n, 165), kEDamage, 0u, 0u);
            if (Rand(n, 163) % 4 == 0) c.evNull = 1;
            if (Rand(n, 166) % 3 == 0) { c.st4 = c.st5 = kTDrive; }
            if (Rand(n, 167) % 2) { c.st3 = kTJump; c.st4 = kTDrive; c.st5 = kTDrive; }
            break;
    case 10: prioUrgent(); jumpTypes(); parachute(true); c.abortRet = BoolRet(n, 168, true);                                    // salto en el aire con paracaidas y sin ruedas: avion rapido o jugador sobre el suelo
             jumpEvent(kEAir, kEAir, kEAir, kEAir, kEAir); if (Rand(n, 169) % 3 == 0) c.et1 = kEWater; if (Rand(n, 170) % 3 == 0) c.et3 = kEWater;
             c.wheels = Rand(n, 171) % 8 ? 0u : c.wheels; c.vsub = Rand(n, 172) % 2 ? 4u : c.vsub; c.mag = Rand(n, 173) % 2 ? Step(kSpeedLimit, int(Rand(n, 174) % 9) - 4) : c.mag;
             c.isPlayer = BoolRet(n, 175, Rand(n, 176) % 2); c.pvl = BoolRet(n, 177, Rand(n, 178) % 3 == 0); break;
    case 11: prioUrgent(); jumpTypes(); parachute(true); c.abortRet = BoolRet(n, 179, true);                                    // lo mismo con ped que no es jugador / golpe del rayo vertical
             jumpEvent(kEAir, kEAir, kEAir, kEAir, kEAir); c.wheels = 0; c.vsub = Rand(n, 180) % 3 ? Pick(std::array<u32, 5>{0u, 1u, 3u, 5u, 0x80000004u}, n, 181) : 4u;
             c.isPlayer = BoolRet(n, 182, Rand(n, 183) % 2); c.pvl = BoolRet(n, 184, Rand(n, 185) % 2); break;
    case 12: prioUrgent(); jumpTypes(); c.abortRet = BoolRet(n, 186, true);                                                     // agua, o aire sin paracaidas / con ruedas: velocidad z contra -0.3f
             parachute(Rand(n, 187) % 2); jumpEvent(Rand(n, 188) % 2 ? kEWater : kEAir, kEWater, Rand(n, 189) % 2 ? kEWater : kEAir, kEWater, kEWater);
             if (Rand(n, 190) % 2) { c.wheels = Pick(std::array<u32, 5>{1u, 4u, 0x100u, 0x80000000u, 0xFFFFFFFFu}, n, 191); }
             c.vz = Rand(n, 192) % 2 ? Step(kFallLimit, int(Rand(n, 193) % 11) - 5) : c.vz; break;
    case 13: prioUrgent(); jumpTypes(); c.abortRet = BoolRet(n, 194, false);                                                    // salto cuya subtarea se niega a abortar: false sin mas
             jumpEvent(Rand(n, 195) % 2 ? kEAir : kEWater, Rand(n, 196) % 2 ? kEWater : kEAir, kEAir, kEAir, kEAir); break;
    case 14: prioUrgent(); c.st0 = notKnown(197); c.st1 = notKnown(198); c.st2 = notKnown(199); c.st3 = Rand(n, 200) % 2 ? notKnown(201) : kTJump;   // resto: 0x2CF en la llamada final
             c.st4 = Rand(n, 202) % 2 ? kTDrive : notKnown(203); c.st5 = Rand(n, 204) % 2 ? kTDrive : notKnown(205);
             if (Rand(n, 206) % 4 == 0) c.evNull = 1; break;
    default: {                                                                                                                  // 15: mezcla con NaN, infinitos, subnormales y valores extremos
        c.prio = Rand(n, 207) % 3 ? 1 : 2; c.vehNull = Rand(n, 208) % 12 == 0; c.subNull = 0;
        if (Rand(n, 209) % 2) { jumpTypes(); jumpEvent(RandEvent(n, 210), RandEvent(n, 211), RandEvent(n, 212), RandEvent(n, 213), RandEvent(n, 214)); if (Rand(n, 215) % 2) { c.et0 = kEAir; } }
        parachute(Rand(n, 216) % 3); c.mag = Rand(n, 217) % 2 ? Pick(kNan, n, 218) : Pick(kInf, n, 219); c.vz = Rand(n, 220) % 2 ? Pick(kNan, n, 221) : Pick(kTiny, n, 222);
        c.vsub = 4; c.wheels = 0;
        break;
    }
    }
    // el EXE no comprueba la subtarea: solo hay casos sin subtarea cuando la funcion no la toca (sin vehiculo o prioridad ajena)
    if (!(c.vehNull || (c.prio != 1 && c.prio != 2))) c.subNull = 0;
    c.eax = SeedReg(n, 250); c.edx = SeedReg(n, 256); c.stack = StackSeed(n); c.seed = Rand(n, 299);
    return c;
}
inline std::string JsonC(const C& c) {
    Js j; j.U("family", c.family).U("veh_null", c.vehNull).U("prio", c.prio).U("sub_null", c.subNull).U("ev_null", c.evNull)
     .U("st0", c.st0).U("st1", c.st1).U("st2", c.st2).U("st3", c.st3).U("st4", c.st4).U("st5", c.st5)
     .U("et0", c.et0).U("et1", c.et1).U("et2", c.et2).U("et3", c.et3).U("et4", c.et4).U("et5", c.et5)
     .U("killed", c.killed).U("grp", c.grp).U("abort_ret", c.abortRet).U("ped_flags", c.pedFlags)
     .U("door", c.door).U("sens", c.sens).U("dfs", c.dfs).U("ngs", c.ngs).U("in_air", c.inAir).U("vng", c.vng).U("vsub", c.vsub).U("wheels", c.wheels)
     .U("mag", c.mag).U("vz", c.vz).U("slot", c.slot).U("wtype", c.wtype).U("has_mat", c.hasMat).U("clump", c.clump).U("is_player", c.isPlayer).U("pvl", c.pvl).U("cdoor", c.cdoor)
     .U("px", c.px).U("py", c.py).U("pz", c.pz).U("mx", c.mx).U("my", c.my).U("mz", c.mz)
     .U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}
}

#pragma once
// Fixtures del banco Fn85C (lote 85C): cuatro funciones, 16384 casos cada una (65536 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CGarage::IsAnyCarBlockingDoor                          0x4494F0  thiscall (garaje en ECX), `ret`; devuelve AL                                  (hook CGarage / IsAnyCarBlockingDoor, tipo S)
//   1 CGarage::StoreAndRemoveCarsForThisImpoundingGarage     0x449A50  thiscall (garaje en ECX, coches guardados, plazas maximas), `ret 8`; sin retorno (hook CGarage / StoreAndRemoveCarsForThisImpoundingGarage, tipo S)
//   2 CTaskSimpleFight::BeHitWhileBlocking                   0x61C650  thiscall (tarea de la victima en ECX, victima, creador, combo, movimiento), `ret 10h`; devuelve AL
//                                                                      (hook CTaskSimpleFight / BeHitWhileBlocking, tipo S)
//   3 CTaskSimpleFight::ChooseAttackAI                       0x624A40  thiscall (tarea en ECX, ped), `ret 4`; devuelve AX                            (hook CTaskSimpleFight / ChooseAttackAI, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Familia = n % 16 (1024 casos por familia).
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

namespace F85CFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kPoolSlots = 8, kSlotStride = 0xA18, kPoolWin = kPoolSlots * kSlotStride, kMapWin = 16, kPoolStructWin = 0x20, kMatPoolWin = kPoolSlots * 0x48;
constexpr std::size_t kGarWin = 0xD8, kCarsWin = 4 * 0x40, kVtWin = 0x10;
constexpr std::size_t kColWin = kPoolSlots * 0x30, kDataWin = kPoolSlots * 0x10, kMaxSph = 4, kSphWin = kPoolSlots * kMaxSph * 0x14;
constexpr std::size_t kTaskWin = 0x28, kAnimWin = 0x40, kHierWin = 0x20, kPedLiteWin = 0x20, kMatWin = 0x48, kPedWin = 0x740, kTargetWin = 0x560;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kGarArena = Arena(kGarWin), kPoolArena = Arena(kPoolWin), kMapArena = Arena(kMapWin), kPoolStructArena = Arena(kPoolStructWin), kMatPoolArena = Arena(kMatPoolWin);
constexpr std::size_t kColArena = Arena(kColWin), kDataArena = Arena(kDataWin), kSphArena = Arena(kSphWin), kCarsArena = Arena(kCarsWin), kVtArena = Arena(kVtWin);
constexpr std::size_t kTaskArena = Arena(kTaskWin), kAnimArena = Arena(kAnimWin), kHierArena = Arena(kHierWin), kPedLiteArena = Arena(kPedLiteWin), kMatArena = Arena(kMatWin);
constexpr std::size_t kPedArena = Arena(kPedWin), kTargetArena = Arena(kTargetWin);

constexpr u32 kPoolAddr = 0xB74494;          // CPools::ms_pVehiclePool
constexpr u32 kPlayerFocusAddr = 0xB7CD74;   // CWorld::PlayerInFocus
constexpr u32 kPlayerInfoBase = 0xB7CD98;    // CWorld::Players
constexpr u32 kMeleeFlagsBase = 0xC17154;    // CTaskSimpleFight::m_aComboData[0].m_wFlags - 4 * 0x88 (el EXE indexa (combo - 4) * 0x88)

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 85003u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline u32 B(float f) { return std::bit_cast<u32>(f); }
inline float F(u32 b) { return std::bit_cast<float>(b); }

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

inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Un float "especial": cero, desnormal, infinito, NaN (un unico patron), enorme
inline u32 Special(u32 c, u32 k) {
    const u32 r = Rand(c, k) % 32;
    if (r < 10) return Pick(kTiny, c, k + 1);
    if (r < 16) return Pick(kInf, c, k + 1);
    if (r < 24) return 0xFFC00000u;
    if (r < 28) return Pick(kBig, c, k + 1);
    return B(float(int(Rand(c, k + 3) % 2001) - 1000) / 7.0f);
}
inline u32 NextUp(u32 bits, int steps) {   // desplaza un float normal `steps` unidades de ultimo lugar (signo respetado)
    const bool neg = (bits >> 31) != 0;
    u32 mag = bits & 0x7FFFFFFFu;
    mag = u32(i32(mag) + (neg ? -steps : steps));
    return (bits & 0x80000000u) | (mag & 0x7FFFFFFFu);
}

// ------------------------------------------------------------------------------------------------ el caso (los campos de cada funcion son independientes)
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    // pool de vehiculos (0 y 1)
    u32 psize{};                                  // tamano del pool (0..8)
    u32 pmap[kPoolSlots]{};                       // byte de ranuras: bit 7 = libre
    u32 pmat[kPoolSlots]{};                       // 1 = el vehiculo tiene matriz (posicion en +0x30 de la matriz); 0 = posicion en +4
    u32 ppos[kPoolSlots][3]{};                    // posicion del vehiculo (patrones de 32 bits)
    // 0 IsAnyCarBlockingDoor
    u32 nsph[kPoolSlots]{};                       // palabra +0 de la CCollisionData de cada vehiculo (int16 con signo; la mitad alta es basura)
    u32 sphR[kPoolSlots][kMaxSph]{};              // radio de cada esfera
    u32 tmask{};                                  // resultado de IsEntityTouching3D en la llamada k (bit k)
    u32 rmask{};                                  // resultado de IsPointInsideGarage(radio) en la llamada k (bit k)
    u32 reOn{}, reK{}, reVal{};                   // tras la llamada k a IsPointInsideGarage(radio) se reescribe el contador de esferas del ultimo vehiculo consultado
    // 1 StoreAndRemoveCarsForThisImpoundingGarage
    u32 maxSlot{};                                // i32
    u32 cmodel[4]{};                              // palabra +0x12 inicial de cada coche guardado
    u32 imask{};                                  // resultado de IsPointInsideGarage(punto) en la llamada k (bit k)
    u32 createdBy[kPoolSlots]{};                  // byte +0x4A4 de cada vehiculo
    u32 esmask{};                                 // resultado de EntityHasASphereWayOutsideGarage en la llamada k (bit k)
    u32 pinfo{};                                  // CPlayerInfo del jugador en foco (lo fija CaseRun)
    // 2 BeHitWhileBlocking
    u32 lastCmd{}, animNull{}, animFlags{}, total{}, cur{}, hierNull{};
    u32 vpos[3]{}, cpos[3]{}, cHasMat{}, fwd[3]{};
    u32 combo{}, move{}, comboHi{}, moveHi{}, cset{};
    // 3 ChooseAttackAI
    u32 tflags{}, pbyte{}, randv{}, tkind{}, ttype{}, tpd{}, health{}, wlevel{}, reach{};
    u32 gate{};
};

inline constexpr std::array<u32, 12> kComboSets{0xFFu, 0u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 12u, 15u, 16u};

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.ecx = Rand(n, 3); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    u32 kk = 100 + fn * 50;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    auto fillPool = [&](bool positions) {
        c.psize = f < 2 ? R(3) : (f == 2 ? 0u : (f >= 12 ? 8u : 1u + R(8)));
        for (u32 s = 0; s < kPoolSlots; ++s) {
            c.pmap[s] = P(30) ? (0x80u | R(0x80)) : R(0x80);
            c.pmat[s] = P(60) ? 1u : 0u;
            if (positions) for (u32 k = 0; k < 3; ++k) c.ppos[s][k] = P(85) ? B(float(int(R(20001)) - 10000) / 16.0f) : Special(n, nk());
        }
    };
    switch (fn) {
    case 0: {   // IsAnyCarBlockingDoor
        fillPool(false);
        for (u32 s = 0; s < kPoolSlots; ++s) {
            const u32 w = R(100);
            const u32 count = w < 12 ? 0u : (w < 30 ? 1u : (w < 55 ? 2u : (w < 75 ? 3u : (w < 92 ? 4u : 0xFFFFu))));   // 0xFFFF = -1 como int16
            c.nsph[s] = (count == 0xFFFFu ? (P(50) ? 0xFFFFu : 0x8000u + R(0x7FFF)) : count) | (R(0x10000) << 16);
            for (u32 j = 0; j < kMaxSph; ++j) c.sphR[s][j] = P(90) ? B(float(R(2000)) / 100.0f) : Special(n, nk());
        }
        c.tmask = Rand(n, nk());
        c.rmask = Rand(n, nk());
        if (f == 3) { for (u32 s = 0; s < kPoolSlots; ++s) c.pmap[s] &= 0x7Fu; c.tmask = 0xFFFFFFFFu; c.rmask = 0xFFFFFFFFu; }                    // todo dentro: nadie bloquea
        if (f == 4) { for (u32 s = 0; s < kPoolSlots; ++s) c.pmap[s] |= 0x80u; }                                                                 // todo libre
        if (f == 5) { for (u32 s = 0; s < kPoolSlots; ++s) c.pmap[s] &= 0x7Fu; c.tmask = 0u; }                                                   // nada toca
        if (f == 6) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmap[s] &= 0x7Fu; c.nsph[s] = (c.nsph[s] & 0xFFFF0000u) | 1u + R(4); } c.tmask = 0xFFFFFFFFu; c.rmask = 0xFFFFFFFFu; c.rmask &= ~(1u << R(6)); }   // una esfera fuera
        if (f == 7) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmap[s] &= 0x7Fu; c.nsph[s] = (c.nsph[s] & 0xFFFF0000u) | (R(2) ? 0u : (R(2) ? 0xFFFFu : 0x8000u)); } c.tmask = 0xFFFFFFFFu; }   // sin esferas
        if (f == 8) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmat[s] = 0; c.pmap[s] &= 0x7Fu; } }
        if (f == 9) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmat[s] = 1; c.pmap[s] &= 0x7Fu; } }
        if (f == 10) { c.reOn = 1; c.reK = R(6); c.reVal = P(50) ? (R(2) ? 0u : 0xFFFFu) : 1u + R(4); for (u32 s = 0; s < kPoolSlots; ++s) { c.pmap[s] &= 0x7Fu; c.nsph[s] = (c.nsph[s] & 0xFFFF0000u) | 2u + R(3); } c.tmask = 0xFFFFFFFFu; c.rmask = 0xFFFFFFFFu; }   // reescritura del contador en vuelo
        if (f == 11) { c.reOn = 1; c.reK = R(3); c.reVal = 1u + R(4); c.tmask = 0xFFFFFFFFu; c.rmask = 0xFFFFFFFFu; for (u32 s = 0; s < kPoolSlots; ++s) { c.pmap[s] &= 0x7Fu; c.nsph[s] = (c.nsph[s] & 0xFFFF0000u) | 1u + R(2); } }
        if (f == 12) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmap[s] &= 0x7Fu; c.nsph[s] = (c.nsph[s] & 0xFFFF0000u) | 4u; } c.tmask = 0xFFFFFFFFu; c.rmask = 0xFFFFFFFFu; c.rmask &= ~(1u << (24 + R(8))); }   // el bloqueo esta muy al final
        break;
    }
    case 1: {   // StoreAndRemoveCarsForThisImpoundingGarage
        fillPool(true);
        const u32 w = R(100);
        c.maxSlot = w < 30 ? 3u : (w < 45 ? 4u : (w < 55 ? 0u : (w < 65 ? 1u + R(2) : (w < 75 ? 5u + R(4) : (w < 85 ? u32(-1 - int(R(3))) : (w < 92 ? 0x7FFFFFFFu : 0x80000000u))))));
        for (u32 k = 0; k < 4; ++k) c.cmodel[k] = P(30) ? 0u : (P(50) ? 1u + R(0x100) : R(0x10000));
        c.imask = Rand(n, nk());
        for (u32 s = 0; s < kPoolSlots; ++s) c.createdBy[s] = P(70) ? 1u + R(4) : R(256);
        c.esmask = Rand(n, nk());
        if (f == 3) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmap[s] &= 0x7Fu; c.createdBy[s] = 1u; } c.imask = 0xFFFFFFFFu; c.esmask = 0u; c.maxSlot = 3; }                   // todo dentro y guardable
        if (f == 4) { for (u32 s = 0; s < kPoolSlots; ++s) c.pmap[s] |= 0x80u; }                                                                                                    // todo libre
        if (f == 5) { for (u32 s = 0; s < kPoolSlots; ++s) c.pmap[s] &= 0x7Fu; c.imask = 0u; }                                                                                      // todo fuera
        if (f == 6) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmap[s] &= 0x7Fu; c.createdBy[s] = 2u; } c.imask = 0xFFFFFFFFu; }                                                    // todos de mision
        if (f == 7) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmap[s] &= 0x7Fu; c.createdBy[s] = 1u + R(4); } c.imask = 0xFFFFFFFFu; c.esmask = 0xFFFFFFFFu; }                     // todos con esfera fuera
        if (f == 8) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmap[s] &= 0x7Fu; c.createdBy[s] = 1u + R(4); } c.imask = 0xFFFFFFFFu; c.esmask = 0u; c.maxSlot = 4; }               // llena las cuatro plazas
        if (f == 9) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmap[s] &= 0x7Fu; c.createdBy[s] = 1u + R(4); } c.imask = 0xFFFFFFFFu; c.esmask = 0u; c.maxSlot = u32(-int(R(4))); }   // maximo no positivo
        if (f == 10) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmap[s] &= 0x7Fu; c.createdBy[s] = 1u + R(4); } c.imask = 0xFFFFFFFFu; c.esmask = 0u; c.maxSlot = 0x7FFFFFFFu; }   // mas que plazas
        if (f == 11) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmat[s] = 0; c.pmap[s] &= 0x7Fu; } }
        if (f == 12) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmat[s] = 1; c.pmap[s] &= 0x7Fu; } }
        for (u32 k = 0; k < 4; ++k) if (f == 13) c.cmodel[k] = 1u + R(0xFF);
        if (f == 14) { for (u32 k = 0; k < 4; ++k) c.cmodel[k] = 0u; }
        break;
    }
    case 2: {   // BeHitWhileBlocking
        c.lastCmd = P(85) ? 2u : R(256);
        c.animNull = P(8) ? 1u : 0u;
        c.animFlags = P(80) ? R(2) * 0x10u : R(256);
        c.hierNull = 0u;
        const u32 w = R(10);
        c.total = w < 6 ? B(float(R(3000)) / 100.0f + 1.0f) : (w < 8 ? Special(n, nk()) : B(float(R(100)) / 10.0f));
        c.cur = w < 6 ? B(float(R(2000)) / 100.0f) : (R(3) ? Special(n, nk()) : B(float(R(100)) / 10.0f));
        if (f == 15 || (f >= 8 && f < 11)) { c.cur = c.total; if (f == 15 && R(2)) c.cur = NextUp(c.total, int(R(3)) - 1); }
        for (u32 k = 0; k < 3; ++k) {
            c.vpos[k] = P(90) ? B(float(int(R(2001)) - 1000) / 8.0f) : Special(n, nk());
            c.cpos[k] = P(90) ? B(float(int(R(2001)) - 1000) / 8.0f) : Special(n, nk());
            c.fwd[k] = P(80) ? B(float(int(R(2001)) - 1000) / 1000.0f) : Special(n, nk());
        }
        if (f < 4) {   // direccion de avance unitaria alineada con el vector creador - victima: producto escalar alto
            c.fwd[0] = B(1.0f); c.fwd[1] = 0u; c.fwd[2] = 0u;
            c.vpos[0] = B(float(int(R(200))) / 4.0f); c.vpos[1] = c.cpos[1]; c.vpos[2] = c.cpos[2];
            c.cpos[0] = B(F(c.vpos[0]) + float(R(2) ? 1 + int(R(30)) : -1 - int(R(30))) * 0.25f);
            c.lastCmd = 2u; c.animNull = 0u; c.animFlags &= ~1u; c.total = B(10.0f); c.cur = B(float(R(90)) / 10.0f);
        }
        if (f == 4) { c.fwd[0] = B(1.0f); c.fwd[1] = 0u; c.fwd[2] = 0u; c.vpos[0] = 0u; c.vpos[1] = c.cpos[1]; c.vpos[2] = c.cpos[2]; c.cpos[0] = B(0.3f + float(int(R(3)) - 1) * 0.0000001f); c.lastCmd = 2u; c.animNull = 0u; c.animFlags &= ~1u; c.total = B(10.0f); c.cur = 0u; }   // justo en 0.3
        if (f == 5) { for (u32 k = 0; k < 3; ++k) c.cpos[k] = c.vpos[k]; c.lastCmd = 2u; c.animNull = 0u; c.animFlags &= ~1u; c.total = B(10.0f); c.cur = 0u; }       // mismo punto: producto 0 < 0.3
        c.cHasMat = P(60) ? 1u : 0u;
        const u32 cm = R(100);
        c.combo = cm < 50 ? 7u + R(7) : (cm < 80 ? R(7) : R(256));
        c.move = P(50) ? R(3) : R(256);
        if (f >= 8 && f < 11) { c.combo = f == 8 ? 8u + R(5) : (f == 9 ? 7u : 9u); }
        c.comboHi = P(50) ? 0u : R(0x1000000);
        c.moveHi = P(50) ? 0u : R(0x1000000);
        c.cset = P(75) ? 7u + R(7) : kComboSets[R(12)];
        if (f >= 12 && f < 15) { c.lastCmd = 2u; c.animNull = 0u; c.animFlags &= ~1u; c.total = B(10.0f); c.cur = B(float(R(90)) / 10.0f); }
        if (f == 12) { c.fwd[0] = B(1.0f); c.fwd[1] = 0u; c.fwd[2] = 0u; c.cpos[0] = B(F(c.vpos[0]) + 1.0f); c.cpos[1] = c.vpos[1]; c.cpos[2] = c.vpos[2]; c.combo = 8u + R(5); c.cset = 5u + R(10); }   // casi seguro bloquea: se prueba el combo activo
        if (f == 13) { c.fwd[0] = B(1.0f); c.fwd[1] = 0u; c.fwd[2] = 0u; c.cpos[0] = B(F(c.vpos[0]) + 1.0f); c.cpos[1] = c.vpos[1]; c.cpos[2] = c.vpos[2]; c.combo = 7u; c.move = R(3); }
        if (f == 14) { c.fwd[0] = B(1.0f); c.fwd[1] = 0u; c.fwd[2] = 0u; c.cpos[0] = B(F(c.vpos[0]) + 1.0f); c.cpos[1] = c.vpos[1]; c.cpos[2] = c.vpos[2]; c.combo = 9u; c.cset = 6u + R(8); }
        break;
    }
    default: {  // 3 ChooseAttackAI
        c.cset = P(80) ? kComboSets[R(12)] : (R(2) ? 17u : 1u + R(3));
        c.tflags = P(60) ? 0x47u & R(256) | (P(70) ? 2u : 0u) : R(256);
        c.pbyte = P(50) ? 0xFFu : R(256);
        const u32 w = R(100);
        c.randv = w < 30 ? 26214u + R(5) - 2u : (w < 45 ? 16384u + R(5) - 2u : (w < 55 ? R(32768) : (w < 90 ? 20000u + R(12768) : (w < 94 ? 0u : (w < 97 ? 32767u : R(0x7FFFFFFFu))))));
        c.tkind = P(15) ? 0u : (P(15) ? 1u : 2u);   // 0 sin objetivo, 1 objetivo que no es un ped, 2 ped
        c.ttype = R(256);
        if ((c.ttype & 7u) == 3u) c.ttype ^= 1u;      // un objetivo que no es un ped
        if (c.tkind == 2) c.ttype = 3u | ((R(2) ? 0u : R(32)) << 3);
        c.tpd = P(75) ? 1u : 0u;
        c.health = P(60) ? B(float(R(400)) / 10.0f) : (P(60) ? B(20.0f + float(int(R(3)) - 1) * 0.0001f) : Special(n, nk()));
        c.wlevel = P(60) ? 1u + R(6) : (P(50) ? 0u : (P(50) ? u32(-1 - int(R(5))) : Rand(n, nk())));
        c.reach = Rand(n, nk());
        if (f < 4) { c.randv = 26220u + R(6540); c.tflags = 0x43u | (P(50) ? 0x40u : 0u) | 2u; c.tkind = 2; c.ttype = 3u; c.tpd = 1u; c.wlevel = 1u + R(6); c.health = B(20.5f + float(R(100))); }          // camino A completo
        if (f == 4) { c.randv = 26220u + R(6540); c.tflags = 0x42u; }                                                                                                                                   // A sin bandera 0x40
        if (f == 5) { c.randv = 26220u + R(6540); c.tflags = 0x42u | 0x40u; c.tkind = R(2) ? 0u : 1u; }                                                                                                    // A sin objetivo o no ped
        if (f == 6) { c.randv = 26220u + R(6540); c.tflags = 0x42u | 0x40u; c.tkind = 2; c.ttype = 3u; c.tpd = R(2); c.wlevel = R(2) ? 0u : 3u; }                                                     // A sin datos de jugador o sin busqueda
        if (f == 7) { c.randv = 26220u + R(6540); c.tflags = 0x42u | 0x40u; c.tkind = 2; c.ttype = 3u; c.tpd = 1u; c.wlevel = 2u; c.health = R(2) ? B(20.0f) : (R(2) ? B(5.0f) : 0xFFC00000u); }           // A con salud no mayor que 20
        if (f == 8) { c.randv = R(26200); c.tflags = 0x05u | (R(2) ? 2u : 0u); }                                                                                                                   // B: alcance y bandera 4
        if (f == 9) { c.randv = 16380u + R(10); c.tflags = 0x01u; c.reach = 0u; }                                                                                                                 // borde de 0.5 con bandera 1
        if (f == 10) { c.randv = 26200u + R(30); c.tflags = 0x03u | 0x04u; }                                                                                                                         // borde de 0.8
        if (f == 11) { c.cset = 5u + R(3); c.tflags = R(256); c.pbyte = R(256); }                                                                                                                 // combo con mascara del ped
        if (f == 12) { c.cset = R(2) ? 5u : 7u; c.tflags = 0x47u; c.pbyte = R(2) ? 0u : 0x42u; c.randv = 20000u + R(12768); c.reach = 0xFFFFFFFFu; }
        if (f == 13) { c.cset = 4u + R(2) * 4u; c.tflags = 0x47u; c.pbyte = 0u; }                                                                                                                   // sin mascara (4, 8)
        if (f == 14) { c.randv = 0x7FFFFFFFu - R(3); }                                                                                                                                              // tirada enorme
        break;
    }
    }
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    switch (c.fn) {
    case 0: case 1: {
        std::vector<u32> pos;
        for (u32 s = 0; s < kPoolSlots; ++s) for (u32 k = 0; k < 3; ++k) pos.push_back(c.ppos[s][k]);
        j.U("psize", c.psize).A("pmap", std::vector<u32>(c.pmap, c.pmap + kPoolSlots)).A("pmat", std::vector<u32>(c.pmat, c.pmat + kPoolSlots));
        if (c.fn == 0) {
            std::vector<u32> radius;
            for (u32 s = 0; s < kPoolSlots; ++s) for (u32 k = 0; k < kMaxSph; ++k) radius.push_back(c.sphR[s][k]);
            j.A("nsph", std::vector<u32>(c.nsph, c.nsph + kPoolSlots)).A("sphr", radius).U("tmask", c.tmask).U("rmask", c.rmask).U("reon", c.reOn).U("rek", c.reK).U("reval", c.reVal);
        } else {
            j.A("ppos", pos).U("maxslot", c.maxSlot).A("cmodel", std::vector<u32>(c.cmodel, c.cmodel + 4)).U("imask", c.imask).A("createdby", std::vector<u32>(c.createdBy, c.createdBy + kPoolSlots))
             .U("esmask", c.esmask).U("pinfo", c.pinfo);
        }
        break;
    }
    case 2:
        j.U("lastcmd", c.lastCmd).U("animnull", c.animNull).U("animflags", c.animFlags).U("total", c.total).U("cur", c.cur).A("vpos", std::vector<u32>(c.vpos, c.vpos + 3)).A("cpos", std::vector<u32>(c.cpos, c.cpos + 3))
         .U("chasmat", c.cHasMat).A("fwd", std::vector<u32>(c.fwd, c.fwd + 3)).U("combo", c.combo).U("move", c.move).U("combohi", c.comboHi).U("movehi", c.moveHi).U("cset", c.cset);
        break;
    default:
        j.U("cset", c.cset).U("tflags", c.tflags).U("pbyte", c.pbyte).U("randv", c.randv).U("tkind", c.tkind).U("ttype", c.ttype).U("tpd", c.tpd).U("health", c.health).U("wlevel", c.wlevel).U("reach", c.reach);
        break;
    }
    j.U("gate", c.gate);
    return j.End();
}
}

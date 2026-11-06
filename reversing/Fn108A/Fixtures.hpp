#pragma once
// Fixtures del banco Fn108A (lote 108A): 17 funciones x 16384 casos (278528 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CMBlur::MotionBlurRender       0x71D700  cdecl (7 argumentos), `ret`; void           (NOP)
//   1 CMBlur::MotionBlurOpen         0x71D710  cdecl (1 argumento); devuelve true
//   2 CMBlur::MotionBlurClose        0x71D720  cdecl; devuelve true
//   3 CMBlur::SetDrunkBlur           0x71D730  cdecl (float); escribe CMBlur::Drunkness (0xC73C58)
//   4 CMBlur::ClearDrunkBlur         0x71D780  cdecl; CTimer::ms_fTimeScale (0xB7CB64) = 1.0 y Drunkness = 0
//   5 CRubbish::SetVisibility        0x7204C0  cdecl (bool como dword); NOP
//   6 CHeli::SpecialHeliPreRender    0x6C4750  sin argumentos; NOP
//   7 CPlaneBanners::Init            0x717820  sin argumentos; NOP
//   8 CTaskSimpleInAir::ApplyRollAndPitch  0x678E80  thiscall (ped), `ret 4`; NOP
//   9 CTaskSimpleGoToPointFine::SetMoveRatio  0x65F3A0  thiscall (float), `ret 4`; this+0x20 = valor
//  10 CVehicleModelInfo::GetWheelSize  0x6A06F0  thiscall (bool), `ret 4`; devuelve float en ST0
//  11 CTaskManager::GetLastTaskOf(CTask*)  0x681970  cdecl; sigue GetSubTask (ranura 2) hasta la ultima tarea
//  12 CTaskManager::GetLastTaskOf(ePrimaryTasks)  0x681A00  thiscall (indice), `ret 4`
//  13 CTaskSimpleDuck::StopFireGun  0x61C420  thiscall; devuelve bool
//  14 CTaskSimpleDuck::AbortBecauseOfOtherDuck  0x692340  thiscall (ped), `ret 4`; void
//  15 SayJacked  0x6497A0  cdecl (jacked, vehiculo, retraso); devuelve bool
//  16 SayJacking  0x6497F0  cdecl (jacker, jacked, vehiculo, retraso); devuelve bool
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
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

namespace F108AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 17;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kTaskWin = 0x60, kPedWin = 0x5A0, kVehWin = 0x180, kAnimWin = 0x30, kChainWin = 0x20;
constexpr unsigned kChainN = 16;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 108001u); }
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
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }



// ------------------------------------------------------------------------------------------------ el caso
// Cada funcion interpreta `a` como se documenta en su generador; todos los valores viajan como u32 (los floats, como su patron de bits).
// a[40] = valor inicial de CMBlur::Drunkness (0xC73C58) y a[41] = el de CTimer::ms_fTimeScale (0xB7CB64) en TODAS las funciones (las dos globales que toca el lote).
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    std::array<u32, 48> a{};
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 Pk(std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); }
    float Fl(float lo, float hi) { return lo + (hi - lo) * (float(R(100001)) / 100000.0f); }
    u32 W() { return Rand(n, nk()); }
};

// Un patron de float "de juego": normales, ceros con signo, desnormales, infinitos, NaN silenciosos y senalizantes con cargas distintas, enormes
inline u32 AnyFloat(Gen& g) {
    const u32 r = g.R(40);
    if (r < 8) return B(g.Fl(-2.0f, 2.0f));
    if (r < 12) return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u});
    if (r < 18) return g.Pk({1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x00800000u, 0x80800000u});
    if (r < 22) return g.Pk({0x7F800000u, 0xFF800000u});
    if (r < 28) return g.Pk({0xFFC00000u, 0x7FC00000u, 0x7F800001u, 0xFF800001u, 0x7FBFFFFFu, 0xFFBFFFFFu, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC12345u, 0xFF812345u});
    if (r < 32) return g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u});
    return g.W();
}
inline u32 NextUp(u32 bits, int steps) {   // desplaza un float normal `steps` unidades de ultimo lugar (signo respetado)
    const bool neg = (bits >> 31) != 0;
    u32 mag = bits & 0x7FFFFFFFu;
    mag = u32(i32(mag) + (neg ? -steps : steps));
    return (bits & 0x80000000u) | (mag & 0x7FFFFFFFu);
}
inline u32 PtrKind(Gen& g) { return g.P(80) ? 0u : g.R(4); }   // 0 arena, 1 nulo, 2 y 3 valores crudos

// valores iniciales de las dos globales que toca el lote
inline void Globals(C& c, Gen& g) {
    c.a[40] = g.P(60) ? AnyFloat(g) : g.W();
    c.a[41] = g.P(60) ? AnyFloat(g) : g.W();
}

// ---- 0: MotionBlurRender: 7 argumentos
inline void Make0(C& c, unsigned n) {
    Gen g(n);
    for (u32 k = 0; k < 7; ++k) c.a[k] = c.family == 0 ? 0u : (c.family == 1 ? 0xFFFFFFFFu : (c.family == 2 ? AnyFloat(g) : g.W()));
}
// ---- 1: MotionBlurOpen: 1 argumento (camara)
inline void Make1(C& c, unsigned n) {
    Gen g(n);
    c.a[0] = c.family == 0 ? 0u : (c.family == 1 ? 0xFFFFFFFFu : (c.family == 2 ? 1u : g.W()));
}
// ---- 2: MotionBlurClose, 4: ClearDrunkBlur, 6: SpecialHeliPreRender, 7: Init: sin argumentos
inline void MakeNone(C&, unsigned) {}
// ---- 3: SetDrunkBlur: a[0] = argumento
inline void Make3(C& c, unsigned n) {
    Gen g(n);
    switch (c.family) {
    case 0: c.a[0] = B(g.Fl(0.0001f, 0.9999f)); break;                                                                 // estrictamente entre 0 y 1
    case 1: c.a[0] = g.Pk({0u, 0x80000000u}); break;                                                                    // ceros con signo
    case 2: c.a[0] = g.P(40) ? 0x3F800000u : NextUp(0x3F800000u, int(g.R(7)) - 3); break;                               // alrededor de 1.0
    case 3: c.a[0] = B(g.Fl(-100.0f, -0.0001f)); break;                                                                 // negativos
    case 4: c.a[0] = B(g.Fl(1.0001f, 1000.0f)); break;                                                                  // mayores que 1
    case 5: c.a[0] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F800000u, 0xFF800000u, 0x7F000000u, 0xFF000000u}); break;       // enormes e infinitos
    case 6: c.a[0] = g.Pk({0xFFC00000u, 0x7FC00000u, 0x7F800001u, 0xFF800001u, 0x7FBFFFFFu, 0xFFBFFFFFu, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC12345u, 0xFF812345u}); break;   // NaN
    case 7: c.a[0] = g.Pk({1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80000100u}); break;   // desnormales
    case 8: c.a[0] = NextUp(0x00800000u, int(g.R(5)) - 1); if (g.P(30)) c.a[0] = NextUp(0x80800000u, int(g.R(5)) - 1); break; // alrededor del minimo normal
    case 9: c.a[0] = 0x3F7FFFFFu - g.R(6); break;                                                                       // justo por debajo de 1
    default: c.a[0] = g.P(35) ? B(g.Fl(-0.5f, 1.5f)) : AnyFloat(g); break;
    }
}
// ---- 5: SetVisibility: un dword (bool con basura en los bytes altos)
inline void Make5(C& c, unsigned n) {
    Gen g(n);
    c.a[0] = c.family == 0 ? 0u : (c.family == 1 ? 1u : (c.family == 2 ? 0x100u : (c.family == 3 ? 0xFFFFFF01u : g.W())));
}
// ---- 8: ApplyRollAndPitch: a[0] = el ped (0 arena, 1 nulo, 2 y 3 valores crudos)
inline void Make8(C& c, unsigned n) { Gen g(n); c.a[0] = c.family < 6 ? c.family % 4 : PtrKind(g); }
// ---- 9: SetMoveRatio: a[0] = valor, a[1] = dword inicial de this+0x20
inline void Make9(C& c, unsigned n) {
    Gen g(n);
    switch (c.family) {
    case 0: c.a[0] = B(g.Fl(0.0f, 4.0f)); break;
    case 1: c.a[0] = g.Pk({0u, 0x80000000u, 0x3F800000u, 0x40000000u, 0x3F000000u}); break;
    case 2: c.a[0] = g.Pk({0xFFC00000u, 0x7FC00000u, 0x7F800001u, 0xFF800001u, 0x7F800000u, 0xFF800000u}); break;
    case 3: c.a[0] = g.Pk({1u, 0x80000001u, 0x007FFFFFu, 0x00400000u}); break;
    default: c.a[0] = AnyFloat(g); break;
    }
    c.a[1] = g.P(15) ? c.a[0] : (g.P(15) ? (c.a[0] ^ (1u << g.R(32))) : g.W());
}
// ---- 10: GetWheelSize: a[0] = argumento (dword; solo cuenta el byte bajo), a[1] = tamano delantero (+0x40), a[2] = trasero (+0x44)
inline void Make10(C& c, unsigned n) {
    Gen g(n);
    switch (c.family) {
    case 0: c.a[0] = 0u; break;
    case 1: c.a[0] = 1u; break;
    case 2: c.a[0] = 0x100u + (g.P(50) ? 0u : (g.W() & 0xFFFFFF00u)); break;      // byte bajo cero con basura arriba: trasera
    case 3: c.a[0] = g.Pk({2u, 0x7Fu, 0x80u, 0xFFu, 0x81u, 0xFFFFFFFFu, 0x01010101u}); break;
    default: c.a[0] = g.P(35) ? g.R(2) : g.W(); break;
    }
    c.a[1] = (c.family < 8 && c.family != 4) ? B(g.Fl(0.1f, 1.5f)) : AnyFloat(g);
    c.a[2] = (c.family < 8 && c.family != 4) ? B(g.Fl(0.1f, 1.5f)) : AnyFloat(g);
}
// ---- 11: GetLastTaskOf(CTask*): a[0] = longitud de la cadena (0 = se pasa nulo, 1..6)
inline void Make11(C& c, unsigned n) {
    Gen g(n);
    c.a[0] = c.family < 7 ? c.family : g.R(7);
}
// ---- 12: GetLastTaskOf(indice): a[0] = indice 0..4, a[1..5] = longitud de la cadena de cada hueco primario (0..3)
inline void Make12(C& c, unsigned n) {
    Gen g(n);
    c.a[0] = c.family < 10 ? c.family % 5 : g.R(5);
    for (u32 k = 0; k < 5; ++k) c.a[1 + k] = g.R(4);
    if (c.family >= 5 && c.family < 10) c.a[1 + c.a[0]] = 1 + g.R(3);       // el hueco pedido siempre ocupado
    if (c.family == 10) c.a[1 + c.a[0]] = 0;                                  // el hueco pedido vacio
}
// ---- 13: StopFireGun: a[0] = dword en +0x0C (contador de silbidos +0x0E en los 16 bits altos), a[1] = bytes +0x18..+0x1B (byte +0x19 = abortando), a[2] = +0x1C (x de la orden de movimiento),
//      a[3] = animacion (0 arena, 1 nula), a[4] = mezcla de la animacion (+0x18)
inline void Make13(C& c, unsigned n) {
    Gen g(n);
    const u32 counters[]{0u, 1u, 5u, 0x7FFFu, 0x8000u, 0xFFFFu, 0xFFFEu};
    const u32 cnt = g.P(60) ? counters[g.R(7)] : (g.W() & 0xFFFFu);
    c.a[0] = (g.W() & 0xFFFFu) | (cnt << 16);
    const u32 ab = g.Pk({0u, 1u, 2u, 0x80u, 0xFFu});
    c.a[1] = (g.W() & 0xFFFF00FFu) | (ab << 8);
    c.a[2] = 0u; c.a[3] = 0u; c.a[4] = 0x3F800000u;
    const u32 blendOk = g.Pk({0x3F800000u, 0x3F800001u, 0x40000000u, B(g.Fl(1.0f, 5.0f)), 0x7F800000u, 0x7F7FFFFFu});
    switch (c.family) {
    case 0: c.a[2] = B(g.Fl(-5.0f, 5.0f)); if (!c.a[2]) c.a[2] = 1; break;                                 // movimiento distinto de cero
    case 1: c.a[2] = g.Pk({0xFFC00000u, 0x7FC00000u, 0x7F800001u, 0xFF800001u, 0x7F800000u, 0xFF800000u}); break;   // NaN e infinitos
    case 2: c.a[2] = g.Pk({0u, 0x80000000u}); c.a[3] = 1; break;                                          // sin animacion
    case 3: c.a[2] = g.Pk({0u, 0x80000000u}); c.a[4] = B(g.Fl(0.0f, 0.9999f)); break;                       // mezcla menor que 1
    case 4: c.a[2] = g.Pk({0u, 0x80000000u}); c.a[4] = g.Pk({0xFFC00000u, 0x7FC00000u, 0x7F800001u, 0xFF800001u}); break;   // mezcla NaN
    case 5: c.a[2] = g.Pk({0u, 0x80000000u}); c.a[4] = blendOk; break;                                      // mezcla completa: decide el resto
    case 6: c.a[2] = g.Pk({0u, 0x80000000u}); c.a[4] = blendOk; c.a[1] = (c.a[1] & 0xFFFF00FFu) | (g.Pk({1u, 2u, 0xFFu}) << 8); break;   // abortando
    case 7: c.a[2] = g.Pk({0u, 0x80000000u}); c.a[4] = blendOk; c.a[1] &= 0xFFFF00FFu; c.a[0] = (c.a[0] & 0xFFFFu) | (g.Pk({1u, 2u, 0x7FFFu}) << 16); break;   // contador positivo
    case 8: c.a[2] = g.Pk({0u, 0x80000000u}); c.a[4] = blendOk; c.a[1] &= 0xFFFF00FFu; c.a[0] = (c.a[0] & 0xFFFFu) | (g.Pk({0u, 0x8000u, 0xFFFFu, 0xFFFEu}) << 16); break;   // contador cero o negativo: falso
    case 9: c.a[2] = g.Pk({0u, 0x80000000u}); c.a[4] = NextUp(0x3F800000u, int(g.R(5)) - 2); break;         // alrededor de 1.0
    case 10: c.a[2] = g.Pk({0u, 0x80000000u}); c.a[4] = g.Pk({1u, 0x80000001u, 0x007FFFFFu, 0x80000000u, 0u, 0x00800000u}); break;   // diminutas y cero
    case 11: c.a[2] = g.Pk({1u, 0x80000001u, 0x00800000u, 0x007FFFFFu}); break;                             // movimiento diminuto (desnormal)
    case 12: c.a[2] = g.Pk({0u, 0x80000000u}); c.a[4] = g.Pk({0xFF800000u, 0x7F800000u, 0xBF800000u, 0xFF7FFFFFu}); break;   // mezcla infinita o negativa
    default: c.a[2] = g.P(40) ? g.Pk({0u, 0x80000000u}) : AnyFloat(g); c.a[3] = g.P(15); c.a[4] = g.P(50) ? blendOk : AnyFloat(g); break;
    }
}
// ---- 14: AbortBecauseOfOtherDuck: a[0] = animacion de agachado (+0x10; 0 arena, 1 nula), a[1] = animacion de movimiento (+0x14), a[2] = bytes +0x18..+0x1B, a[3] = delta de mezcla inicial de la animacion de movimiento (+0x1C),
//      a[4] = ped (0 arena, 1 nulo, 2 y 3 crudos), a[5] = delta de mezcla inicial de la animacion de agachado (+0x1C)
inline void Make14(C& c, unsigned n) {
    Gen g(n);
    c.a[0] = c.family & 1 ? 1u : 0u;
    c.a[1] = c.family & 2 ? 1u : 0u;
    if (c.family >= 4) { c.a[0] = g.P(75) ? 0u : 1u; c.a[1] = g.P(75) ? 0u : 1u; }
    c.a[2] = g.W(); c.a[3] = g.P(30) ? 0xC1000000u : g.W(); c.a[4] = PtrKind(g); c.a[5] = g.W();
}
// ---- 15: SayJacked: a[0] = valor que devuelve GetVehicleTypeForAudio (el tipo en los 16 bits bajos, basura arriba), a[1] = retraso, a[2] = lo que devuelve CPed::Say
// ---- 16: SayJacking: ademas a[3] = lo que devuelve IsPedFemaleForAudio
inline u32 TypeWord(Gen& g, u32 family) {
    const u32 type = family < 12 ? (family % 3) : g.Pk({0u, 1u, 2u, 0u, 1u, 2u, 3u, 0xFFFFu, 0x7FFFu, 0x8000u, 0x0100u});
    const u32 junk = g.P(30) ? (g.W() & 0xFFFFu) : 0u;
    return type | (junk << 16);
}
inline u32 SayResult(Gen& g) { return g.Pk({0xFFFFu, 0xFFFFu, 0u, 1u, 0x7FFFu, 0xFFFEu, 0x1FFFFu, 0xABCD0000u, 0xFFFF0000u, 0xFFFFFFFFu, 0x8000u, g.W()}); }
inline u32 Delay(Gen& g) { return g.Pk({0u, 0u, 1u, 100u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.W()}); }
inline void Make15(C& c, unsigned n) { Gen g(n); c.a[0] = TypeWord(g, c.family); c.a[1] = Delay(g); c.a[2] = SayResult(g); }
inline void Make16(C& c, unsigned n) {
    Gen g(n);
    c.a[0] = TypeWord(g, c.family); c.a[1] = Delay(g); c.a[2] = SayResult(g);
    c.a[3] = g.P(50) ? g.R(2) : (g.P(50) ? g.W() : (g.W() & 0xFFFFFF00u));     // solo cuenta el byte bajo
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    { Gen g(n, 700); Globals(c, g); }
    switch (fn) {
    case 0: Make0(c, n); break;
    case 1: Make1(c, n); break;
    case 3: Make3(c, n); break;
    case 5: Make5(c, n); break;
    case 8: Make8(c, n); break;
    case 9: Make9(c, n); break;
    case 10: Make10(c, n); break;
    case 11: Make11(c, n); break;
    case 12: Make12(c, n); break;
    case 13: Make13(c, n); break;
    case 14: Make14(c, n); break;
    case 15: Make15(c, n); break;
    case 16: Make16(c, n); break;
    default: MakeNone(c, n); break;     // 2, 4, 6, 7
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    return j.End();
}
}

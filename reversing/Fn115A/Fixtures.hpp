#pragma once
// Fixtures del banco Fn115A (lote 115A): 16384 casos por funcion (20 x 16384 = 327680 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
// Nueve cuerpos no virtuales de destructor (kind 0) y once constructores (kind 1) del EXE de tareas y eventos (ver kSpecs):
//   kind 0 destructor (thiscall, ECX = objeto, `ret`): vtable propia, CEntity::CleanUpOldReference (0x571A00) de la referencia no nula (si la hay), y la cola del destructor base (0x4AC010 / 0x61A3A0 / 0x61A3D0 / 0x632C10 + 0x61A3D0).
//   kind 1 constructor (thiscall, ECX = objeto, argumentos en la pila, `ret N`, EAX = this): constructor base (0x61A390 / 0x61A3B0 / 0x6321F0 / 0x69C240), campos, vtable propia y, en CTaskSimpleArrestPed, RegisterReference.
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

namespace F115AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 20;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kObjWin = 0x80, kEntWin = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 115001u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

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

// ------------------------------------------------------------------------------------------------ las funciones
// types: un caracter por argumento de pila del constructor: 'P' puntero a entidad (modo 0 arena A / B segun el orden, 1 nulo, 2 crudo), 'D' dword, 'B' dword cuyo byte bajo se usa.
// refOff: destructor, desplazamiento de la referencia que se libera (0xFFFF = ninguna). vtable: vtable propia del EXE (para la vtable inicial del destructor).
struct Spec {
    const char* name;
    std::uintptr_t entry;
    u32 kind;                      // 0 destructor, 1 constructor
    u32 vtable;
    u32 refOff;
    const char* types;
};
inline constexpr std::array<Spec, kFunctions> kSpecs{{
    {"CEventDontJoinPlayerGroup::Destructor", 0x609140, 0, 0x86D0E0, 0xC, ""},
    {"CEventPassObject::Destructor", 0x65DCF0, 0, 0x86F8B8, 0xC, ""},
    {"CTaskComplexKillPedOnFoot::Destructor", 0x620EF0, 0, 0x86D894, 0x10, ""},
    {"CTaskComplexSequence::Destructor", 0x6389F0, 0, 0x86E200, 0xFFFF, ""},
    {"CTaskSimpleCarSetPedInAsDriver::Destructor", 0x647170, 0, 0x86EE28, 0x10, ""},
    {"CTaskSimpleCarSetPedOut::Destructor", 0x647950, 0, 0x86EEB8, 0x8, ""},
    {"CTaskSimpleGoTo::Destructor", 0x667D50, 0, 0x86FD1C, 0xFFFF, ""},
    {"CTaskComplexWander::Destructor", 0x669D20, 0, 0x86FE84, 0xFFFF, ""},
    {"CTaskComplexJump::Destructor", 0x67A060, 0, 0x870570, 0xFFFF, ""},
    {"CTaskSimpleAnim::Constructor", 0x61A6C0, 1, 0x86D504, 0xFFFF, "D"},
    {"CTaskSimplePlayHandSignalAnim::Constructor", 0x61AE50, 1, 0x86D5B8, 0xFFFF, "DDBB"},
    {"CTaskSimpleDrown::Constructor", 0x62FEC0, 1, 0x86DE30, 0xFFFF, ""},
    {"CTaskComplexAttractorPartnerWait::Constructor", 0x633250, 1, 0x86E284, 0xFFFF, "BD"},
    {"CTaskComplexUseGoggles::Constructor", 0x634EF0, 1, 0x86E4A8, 0xFFFF, ""},
    {"CTaskComplexUseEffectRunning::Constructor", 0x636780, 1, 0x86E594, 0xFFFF, "PP"},
    {"CTaskComplexUseEffectSprinting::Constructor", 0x636860, 1, 0x86E5C0, 0xFFFF, "PP"},
    {"CTaskComplexStuckInAir::Constructor", 0x67BA40, 1, 0x870608, 0xFFFF, ""},
    {"CTaskSimpleArrestPed::Constructor", 0x68B620, 1, 0x870984, 0xFFFF, "P"},
    {"CTaskAllocatorPlayerCommandAttack::Constructor", 0x69C480, 1, 0x870E78, 0xFFFF, "PDD"},
    {"CTaskSimpleFacial::Constructor", 0x690C70, 1, 0x870AB8, 0xFFFF, "DD"},
}};
inline constexpr u32 kPreVtable = 0xDEADBEEFu;   // vtable previa de los constructores (el objeto parte de basura)
inline constexpr std::size_t kPlayerFn = 18;     // CTaskAllocatorPlayerCommandAttack::Constructor: a[8] = modo del jugador que devuelve el falso de FindPlayerPed

// ------------------------------------------------------------------------------------------------ el caso
// Todos los valores viajan como u32 (los floats, como su patron de bits).
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
    u32 W() { return Rand(n, nk()); }
};

// ------------------------------------------------------------------------------------------------ kind 0: destructores
//   a[0] = referencia (0 arena de entidad A, 1 nula, 2 cruda 0x00BADC00), a[2] = vtable inicial (dword)
inline void MakeDtor(C& c, unsigned n, const Spec& sp) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(70) ? 0u : g.R(3);
    c.a[2] = g.P(50) ? sp.vtable : g.W();
    switch (f) {
    case 0: c.a[0] = 0; break;
    case 1: c.a[0] = 1; break;
    case 2: c.a[0] = 2; break;
    case 3: c.a[0] = 0; c.a[2] = sp.vtable; break;
    case 4: c.a[0] = 1; c.a[2] = sp.vtable; break;
    case 5: c.a[2] = 0x0085AB38u; break;
    case 6: c.a[2] = 0x0086D48Cu; break;
    case 7: c.a[2] = 0u; break;
    case 8: c.a[2] = 0xFFFFFFFFu; c.a[0] = g.R(3); break;
    case 9: c.a[2] = sp.vtable; break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ constructores
//   a[0..3] = argumentos de pila en bruto: los punteros llevan un modo (0 arena de entidad A / B segun el orden, 1 nulo, 2 crudo 0x00BADC00 / 0x00BADC40); los demas, su valor.
//   a[8] = modo del jugador (CTaskAllocatorPlayerCommandAttack): 0 arena de entidad B, 1 nulo, 2 crudo.
inline void MakeCtor(C& c, unsigned n, const Spec& sp) {
    const u32 f = c.family;
    Gen g(n);
    u32 pm[4]{};
    for (u32 i = 0; i < 4; ++i) pm[i] = g.P(70) ? 0u : g.R(3);
    switch (f) {
    case 0: pm[0] = pm[1] = pm[2] = 0; break;
    case 1: pm[0] = pm[1] = pm[2] = 1; break;
    case 2: pm[0] = 1; pm[1] = 0; break;
    case 3: pm[0] = 0; pm[1] = 1; break;
    case 4: pm[0] = pm[1] = pm[2] = 2; break;
    case 5: pm[0] = 2; pm[1] = 0; break;
    case 6: pm[0] = 0; pm[1] = 2; break;
    default: break;
    }
    u32 val[4]{};
    for (u32 i = 0; i < 4; ++i) val[i] = g.W();
    switch (f) {
    case 7: for (auto& v : val) v = 0; break;
    case 8: for (auto& v : val) v = 1; break;
    case 9: for (auto& v : val) v = 2; break;
    case 10: for (auto& v : val) v = 0x100u; break;
    case 11: for (auto& v : val) v = 0xFFFFFF01u; break;
    case 12: for (auto& v : val) v = 0x80u; break;
    case 13: for (auto& v : val) v = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0xFFu, 0xFF00u, 0x00FF0000u}); break;
    case 14: for (auto& v : val) v = g.P(50) ? g.R(4) : g.W(); break;
    default: break;
    }
    u32 nargs = 0;
    for (const char* t = sp.types; *t; ++t) ++nargs;
    for (u32 i = 0; i < nargs; ++i) c.a[i] = sp.types[i] == 'P' ? pm[i] : val[i];
    c.a[8] = pm[3];
    if (c.fn == kPlayerFn && f >= 7 && f <= 9) c.a[8] = f - 7;
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    const Spec& sp = kSpecs[fn];
    if (sp.kind == 0) MakeDtor(c, n, sp); else MakeCtor(c, n, sp);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    return j.End();
}
}

#pragma once
// Fixtures del banco Fn119A (lote 119A): 16384 casos por funcion (17 x 16384 = 278528 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
// Diez eventos (CEventPedEnteredMyVehicle, CEventRevived, CEventChatPartner, CEventSexyPed, CEventSexyVehicle, CEventInWater, CEventFireNearby: constructor y cuerpo del destructor; CEventInAir,
// CEventHitByWaterCannon, CEventPotentialWalkIntoFire: solo el cuerpo del destructor, que unicamente deja la vtable de CEvent):
//   kind 1 constructor del EXE (thiscall, ECX = evento, argumentos en la pila, `ret` / `ret 4` / `ret 8` / `ret 0Ch`, EAX = this): escribe los campos de CEvent (y de CEventEditableResponse si lo es) en linea, la vtable propia,
//         los argumentos en sus campos y registra con CEntity::RegisterReference (0x571B70) las referencias no nulas en el orden del EXE.
//   kind 0 cuerpo no virtual del destructor (thiscall, `ret`): (si libera referencias) vtable propia, CEntity::CleanUpOldReference (0x571A00) de las referencias no nulas, vtable de CEvent (0x85AB38).
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

namespace F119AFix {
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
constexpr std::size_t kEventWin = 0x40, kEntWin = 0x40, kVecWin = 0x20;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 119001u); }
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

// ------------------------------------------------------------------------------------------------ las funciones
// Argumento de pila de un constructor: kind 0 ninguno, 1 puntero (modo a[i]: 0 arena, 1 nulo, 2 crudo) que se guarda en `off`, 2 byte bajo del dword a[i] guardado en `off`, 3 dword a[i] guardado en `off`,
// 4 puntero a un CVector (arena propia, 12 bytes en a[8..10]) cuyos tres dwords se copian a `off`, 5 float (patron de bits en a[i], con NaN, denormales e infinitos) guardado en `off`.
struct Arg { u32 kind; u32 off; };
struct Spec {
    const char* name;
    std::uintptr_t entry;
    u32 kind;                      // 0 destructor, 1 constructor
    u32 vtable;                    // vtable propia del EXE
    bool editable;                 // el constructor escribe tambien los campos de CEventEditableResponse
    std::array<Arg, 3> args;       // constructor: argumentos en el orden de la pila
    u32 nargs;
    std::array<u32, 2> reg;        // constructor: indices de argumento registrados con RegisterReference, en el orden del EXE (0xFF = ninguno)
    u32 refs;                      // destructor: numero de referencias; off = desplazamientos en el orden de liberacion (refs 0: solo deja la vtable de CEvent)
    std::array<u32, 2> off;
};
inline constexpr std::array<Spec, kFunctions> kSpecs{{
    {"CEventPedEnteredMyVehicle::Constructor", 0x4AEAC0, 1, 0x85AFE8, true, {{{1, 0x14}, {1, 0x18}, {3, 0x1C}}}, 3, {0, 1}, 0, {0, 0}},
    {"CEventPedEnteredMyVehicle::Destructor", 0x4AEB90, 0, 0x85AFE8, true, {}, 0, {0xFF, 0xFF}, 2, {0x14, 0x18}},
    {"CEventRevived::Constructor", 0x4AEC50, 1, 0x85B030, false, {}, 0, {0xFF, 0xFF}, 0, {0, 0}},
    {"CEventRevived::Destructor", 0x4AECA0, 0, 0x85B030, false, {}, 0, {0xFF, 0xFF}, 0, {0, 0}},
    {"CEventChatPartner::Constructor", 0x4AECD0, 1, 0x85B070, false, {{{2, 0x0C}, {1, 0x10}}}, 2, {1, 0xFF}, 0, {0, 0}},
    {"CEventChatPartner::Destructor", 0x4AED60, 0, 0x85B070, false, {}, 0, {0xFF, 0xFF}, 1, {0x10, 0}},
    {"CEventSexyPed::Constructor", 0x4AEDF0, 1, 0x85B0B0, true, {{{1, 0x14}}}, 1, {0, 0xFF}, 0, {0, 0}},
    {"CEventSexyPed::Destructor", 0x4AEEA0, 0, 0x85B0B0, true, {}, 0, {0xFF, 0xFF}, 1, {0x14, 0}},
    {"CEventSexyVehicle::Constructor", 0x4AF010, 1, 0x85B0F8, false, {{{1, 0x0C}}}, 1, {0, 0xFF}, 0, {0, 0}},
    {"CEventSexyVehicle::Destructor", 0x4AF0A0, 0, 0x85B0F8, false, {}, 0, {0xFF, 0xFF}, 1, {0x0C, 0}},
    {"CEventInWater::Constructor", 0x4B1370, 1, 0x85B4C8, false, {{{5, 0x0C}}}, 1, {0xFF, 0xFF}, 0, {0, 0}},
    {"CEventInWater::Destructor", 0x4B13C0, 0, 0x85B4C8, false, {}, 0, {0xFF, 0xFF}, 0, {0, 0}},
    {"CEventFireNearby::Constructor", 0x4B1F10, 1, 0x85B6E8, true, {{{4, 0x14}}}, 1, {0xFF, 0xFF}, 0, {0, 0}},
    {"CEventFireNearby::Destructor", 0x4B1F80, 0, 0x85B6E8, true, {}, 0, {0xFF, 0xFF}, 0, {0, 0}},
    {"CEventInAir::Destructor", 0x4B0D00, 0, 0x85B3C0, false, {}, 0, {0xFF, 0xFF}, 0, {0, 0}},
    {"CEventHitByWaterCannon::Destructor", 0x4B1320, 0, 0x85B488, false, {}, 0, {0xFF, 0xFF}, 0, {0, 0}},
    {"CEventPotentialWalkIntoFire::Destructor", 0x4B1F00, 0, 0x85B6A0, true, {}, 0, {0xFF, 0xFF}, 0, {0, 0}},
}};
inline constexpr u32 kPreVtable = 0xDEADBEEFu;   // vtable previa de los constructores (el objeto parte de basura)

// ------------------------------------------------------------------------------------------------ el caso
// Cada funcion interpreta `a` como se documenta en su generador; todos los valores viajan como u32 (los floats, como su patron de bits).
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

inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 8> kNaNs{0xFFC00000u, 0x7FC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFBFFFFFu, 0x7FC12345u, 0xFFFFFFFFu};

// ------------------------------------------------------------------------------------------------ kind 0: destructores con referencias
//   a[0] = primera referencia (0 arena de entidad A, 1 nula, 2 cruda 0x00BADC00), a[1] = segunda (0 arena B, 1 nula, 2 cruda 0x00BADC40), a[2] = vtable inicial (dword)
inline void MakeDtor(C& c, unsigned n, const Spec& sp) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(70) ? 0u : g.R(3); c.a[1] = g.P(70) ? 0u : g.R(3);
    c.a[2] = g.P(50) ? sp.vtable : g.W();
    switch (f) {
    case 0: c.a[0] = 0; c.a[1] = 0; break;
    case 1: c.a[0] = 1; c.a[1] = 1; break;
    case 2: c.a[0] = 0; c.a[1] = 1; break;
    case 3: c.a[0] = 1; c.a[1] = 0; break;
    case 4: c.a[0] = 2; c.a[1] = 2; break;
    case 5: c.a[0] = 2; c.a[1] = 0; break;
    case 6: c.a[0] = 0; c.a[1] = 2; break;
    case 7: c.a[2] = 0x0085AB38u; break;
    case 8: c.a[2] = 0u; break;
    case 9: c.a[2] = 0xFFFFFFFFu; c.a[0] = g.R(3); break;
    case 10: c.a[2] = sp.vtable; break;
    case 11: c.a[0] = 1; c.a[1] = 2; break;
    default: break;
    }
}

// Palabra de 32 bits con aspecto de float: aleatoria, NaN (silenciosos y senalizantes), diminutos (denormales y ceros con signo), infinitos o floats corrientes; la clase depende de la familia y de la posicion.
inline u32 FloatWord(Gen& g, u32 f, u32 pos) {
    switch ((f + pos) % 8) {
    case 0: return g.W();
    case 1: return kNaNs[g.R(u32(kNaNs.size()))];
    case 2: return kTiny[g.R(u32(kTiny.size()))];
    case 3: return g.Pk({0x7F800000u, 0xFF800000u, 0x00000000u, 0x80000000u});
    case 4: return B(g.Fl(-1000.0f, 1000.0f));
    case 5: return g.P(50) ? kNaNs[g.R(u32(kNaNs.size()))] : g.W();
    case 6: return g.P(50) ? kTiny[g.R(u32(kTiny.size()))] : B(g.Fl(-2.0f, 2.0f));
    default: return g.P(20) ? 0x7F7FFFFFu : g.W();
    }
}

// ------------------------------------------------------------------------------------------------ constructores
//   a[0..2] = argumentos de pila en bruto: los punteros llevan un modo (0 arena de entidad A / B segun el orden, 1 nulo, 2 crudo 0x00BADC00 / 0x00BADC40); los bytes y dwords, su valor
inline void MakeCtor(C& c, unsigned n, const Spec& sp) {
    const u32 f = c.family;
    Gen g(n);
    u32 pm[3]{};
    for (u32 i = 0; i < 3; ++i) pm[i] = g.P(70) ? 0u : g.R(3);
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
    u32 val[3]{};
    for (u32 i = 0; i < 3; ++i) val[i] = g.W();
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
    for (u32 i = 0; i < sp.nargs; ++i) c.a[i] = sp.args[i].kind == 1 ? pm[i] : (sp.args[i].kind == 4 ? 0u : val[i]);
    for (u32 i = 0; i < sp.nargs; ++i)
        if (sp.args[i].kind == 5) c.a[i] = FloatWord(g, f, i);
        else if (sp.args[i].kind == 4) for (u32 k = 0; k < 3; ++k) c.a[8 + k] = FloatWord(g, f, k);
    // la vtable previa y el relleno previo (padding) son parte de la imagen: el EXE y la DLL no deben tocarlos
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

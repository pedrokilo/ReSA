#pragma once
// Fixtures del banco Fn107A (lote 107A): 16384 casos por funcion (25 x 16384 = 409600 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0..21 destructores de eventos (cuerpo no virtual del EXE, thiscall, `ret`): ECX = evento de hasta 0x40 bytes. Cada uno deja su vtable, libera con CEntity::CleanUpOldReference (0x571A00) las referencias que
//         no sean nulas (una o dos, en el orden del EXE) y deja la vtable de CEvent (0x85AB38). CEventCarUpsideDown (12) llama a CEntity::RegisterReference (0x571B70) en lugar de liberar (asi es el EXE).
//   22, 23 destructores triviales `mov [ecx], 0x85AB38; ret` de CEvent (0x4AC010) y CEventEditableResponse (0x4AC480): EAX, ECX y EDX deben salir intactos.
//   24    RoundVectorToDecimals (0x50A120, cdecl (float*, int)): RoundToDecimals (0x50A0A0, falseada) sobre x, y, z; cada resultado se guarda como float.
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

namespace F107AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 25;
constexpr unsigned kFirstTrivial = 22, kRoundVector = 24;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kEventWin = 0x40, kEntWin = 0x40, kVecWin = 0x10;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 107001u); }
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
// kind 0: destructor con referencias (vtable propia, desplazamientos de los punteros en el orden de liberacion; registerRef solo en CEventCarUpsideDown); kind 1: trivial; kind 2: RoundVectorToDecimals.
struct Spec {
    const char* name;
    std::uintptr_t entry;
    u32 kind;
    u32 vtable;
    u32 refs;
    std::array<u32, 2> off;
    bool registerRef;
};
inline constexpr std::array<Spec, kFunctions> kSpecs{{
    {"CEventAcquaintancePed::Destructor", 0x4AF890, 0, 0x85B250, 1, {0x14, 0}, false},
    {"CEventVehicleDamage::Destructor", 0x4B1990, 0, 0x85B588, 2, {0x14, 0x18}, false},
    {"CEventAreaCodes::Destructor", 0x4B2210, 0, 0x85B7C0, 1, {0x0C, 0}, false},
    {"CEventGunShot::Destructor", 0x4AC720, 0, 0x85ABE0, 1, {0x14, 0}, false},
    {"CEventPedCollisionWithPed::Destructor", 0x4ACA70, 0, 0x85AC68, 1, {0x14, 0}, false},
    {"CEventCopCarBeingStolen::Destructor", 0x4B17F0, 0, 0x85B548, 2, {0x0C, 0x10}, false},
    {"CEventVehicleOnFire::Destructor", 0x4B1170, 0, 0x85B440, 1, {0x14, 0}, false},
    {"CEventAttractor::Destructor", 0x4AF400, 0, 0x85B1B8, 1, {0x18, 0}, false},
    {"CEventVehicleToSteal::Destructor", 0x4AF700, 0, 0x85B210, 1, {0x0C, 0}, false},
    {"CEventVehicleDied::Destructor", 0x4B0DA0, 0, 0x85B400, 1, {0x0C, 0}, false},
    {"CEventSeenPanickedPed::Destructor", 0x4B2130, 0, 0x85B778, 1, {0x14, 0}, false},
    {"CEventPlayerCommandToGroup::Destructor", 0x4B2470, 0, 0x85B800, 1, {0x18, 0}, false},
    {"CEventCarUpsideDown::Destructor", 0x4B1D50, 0, 0x85B660, 1, {0x0C, 0}, true},
    {"CEventVehicleCollision::Destructor", 0x4AC920, 0, 0x85AC28, 1, {0x14, 0}, false},
    {"CEventObjectCollision::Destructor", 0x4ACDD0, 0, 0x85ACA8, 1, {0x14, 0}, false},
    {"CEventBuildingCollision::Destructor", 0x4ACFF0, 0, 0x85ACE8, 1, {0x14, 0}, false},
    {"CEventPotentialWalkIntoVehicle::Destructor", 0x4AE3C0, 0, 0x85AED0, 1, {0x14, 0}, false},
    {"CEventPotentialWalkIntoObject::Destructor", 0x4AE670, 0, 0x85AF18, 1, {0x14, 0}, false},
    {"CEventStuckInAir::Destructor", 0x4B1520, 0, 0x85B508, 1, {0x0C, 0}, false},
    {"CEventDanger::Destructor", 0x4B26A0, 0, 0x85B848, 1, {0x14, 0}, false},
    {"CEventPotentialWalkIntoPed::Destructor", 0x4AE7A0, 0, 0x85AF60, 1, {0x20, 0}, false},
    {"CEventVehicleHitAndRun::Destructor", 0x4AEA50, 0, 0x85AFA8, 2, {0x10, 0x0C}, false},
    {"CEvent::Destructor", 0x4AC010, 1, 0x85AB38, 0, {0, 0}, false},
    {"CEventEditableResponse::Destructor", 0x4AC480, 1, 0x85AB38, 0, {0, 0}, false},
    {"Common::RoundVectorToDecimals", 0x50A120, 2, 0, 0, {0, 0}, false},
}};

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

// ------------------------------------------------------------------------------------------------ kind 1: destructores triviales
//   a[0] = vtable inicial (dword)
inline void MakeTrivial(C& c, unsigned n, const Spec& sp) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(50) ? sp.vtable : g.W();
    if (f == 1) c.a[0] = 0u;
    if (f == 2) c.a[0] = 0x0085AB80u;
    if (f == 3) c.a[0] = 0xFFFFFFFFu;
    if (f == 4) c.a[0] = 0x0085AB38u;
}

// ------------------------------------------------------------------------------------------------ kind 2: RoundVectorToDecimals (0x50A120)
//   a[0..2] = x, y, z (bits float), a[3] = digitos (int con signo)
inline u32 WideE(Gen& g, int e) {
    const double ex = double(int(g.R(u32(2 * e + 1))) - e);
    const double m = 1.0 + double(g.R(1000000)) / 1000000.0;
    return B(float((g.P(50) ? -1.0 : 1.0) * m * std::pow(10.0, ex)));
}
inline void MakeRound(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    for (int i = 0; i < 3; ++i) c.a[i] = B(g.Fl(-1000.0f, 1000.0f));
    c.a[3] = 4;
    switch (f) {
    case 0: c.a[3] = g.R(5); break;
    case 1: for (int i = 0; i < 3; ++i) c.a[i] = B(-g.Fl(0.0f, 1000.0f)); c.a[3] = g.R(5); break;
    case 2: for (int i = 0; i < 3; ++i) c.a[i] = g.Pk({0u, 0x80000000u, B(1.0f), B(-1.0f), B(0.5f), B(-0.5f)}); c.a[3] = g.R(5); break;
    case 3: for (int i = 0; i < 3; ++i) c.a[i] = B(g.Fl(-100000.0f, 100000.0f)); c.a[3] = u32(-int(1 + g.R(4))); break;
    case 4: for (int i = 0; i < 3; ++i) c.a[i] = g.P(50) ? kNaNs[g.R(8)] : B(g.Fl(-10.0f, 10.0f)); c.a[3] = g.R(5); break;
    case 5: for (int i = 0; i < 3; ++i) c.a[i] = g.Pk({0x7F800000u, 0xFF800000u, B(2.5f), 0xFFC00000u}); c.a[3] = g.R(5); break;
    case 6: for (int i = 0; i < 3; ++i) c.a[i] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, B(1.0e30f), B(-3.0e38f), B(1.0e20f)}); c.a[3] = u32(int(g.R(25)) - 10); break;
    case 7: for (int i = 0; i < 3; ++i) c.a[i] = kTiny[g.R(12)]; c.a[3] = g.R(8); break;
    case 8: c.a[3] = g.Pk({300u, u32(-300), 307u, 308u, 309u, u32(-307), u32(-308), u32(-323), 1000u, u32(-1000), 0x7FFFFFFFu, 0x80000000u, 0x7FFFFFFEu, 0xFFFFFFFFu, 0x80000001u}); break;
    case 9: { const u32 v = B(g.Fl(-500.0f, 500.0f)); c.a[0] = c.a[1] = c.a[2] = v; c.a[3] = g.R(5); break; }
    case 10: for (int i = 0; i < 3; ++i) c.a[i] = g.W(); c.a[3] = g.W(); break;
    case 11: for (int i = 0; i < 3; ++i) c.a[i] = B(g.Fl(-3000.0f, 3000.0f)); c.a[3] = 4; break;       // vectores de camara redondeados a 4 decimales
    case 12: for (int i = 0; i < 3; ++i) c.a[i] = WideE(g, 30); c.a[3] = u32(int(g.R(81)) - 40); break;
    case 13: for (int i = 0; i < 3; ++i) c.a[i] = B(float(int(g.R(2001)) - 1000) * 0.1f); c.a[3] = g.R(4); break;
    case 14: for (int i = 0; i < 3; ++i) c.a[i] = g.P(40) ? kNaNs[g.R(8)] : (g.P(50) ? kTiny[g.R(12)] : B(g.Fl(-50.0f, 50.0f))); c.a[3] = g.P(60) ? g.R(5) : g.W(); break;
    case 15: for (int i = 0; i < 3; ++i) c.a[i] = g.P(70) ? B(g.Fl(-2000.0f, 2000.0f)) : g.W(); c.a[3] = g.P(70) ? g.R(5) : g.W(); break;
    default: break;
    }
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    const Spec& sp = kSpecs[fn];
    switch (sp.kind) {
    case 0: MakeDtor(c, n, sp); break;
    case 1: MakeTrivial(c, n, sp); break;
    default: MakeRound(c, n); break;
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

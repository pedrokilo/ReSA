#pragma once
// Fixtures del banco Fn120A (lote 120A): 16384 casos por funcion (16384 pares por funcion). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
// Cinco metodos virtuales diminutos del EXE (cada direccion aparece exactamente una vez en una vtable; todas son thiscall con ECX = this, que ninguna lee):
//   0 CAEPedlessSpeechAudioEntity::AddScriptSayEvent  0x4E6080  `ret 14h` (3 B; cinco argumentos de pila que limpia el callee; no hace nada)   vtable 0x85F330 ranura 1
//   1 CTaskComplexGangJoinRespond::GetTaskType        0x661710  `mov eax,4C3h; ret` (6 B)  = 1219 TASK_COMPLEX_GANG_JOIN_RESPOND               vtable 0x86FB5C ranura 4
//   2 CTaskComplexKillPedOnFoot::GetTaskType          0x620EE0  `mov eax,3E8h; ret` (6 B)  = 1000 TASK_COMPLEX_KILL_PED_ON_FOOT               vtable 0x86D894 ranura 4
//   3 CTaskComplexPartner::GetTaskType                0x681F10  `mov eax,100h; ret` (6 B)  =  256 TASK_COMPLEX_PARTNER                        vtable 0x870664 ranura 4
//   4 CTaskComplexPartnerDeal::GetTaskType            0x681DF0  `mov eax,4B2h; ret` (6 B)  = 1202 TASK_COMPLEX_PARTNER_DEAL                   vtable 0x870754 ranura 4
// (el listado de pendientes atribuia 0x681DF0 a CTaskManager; la vtable que la contiene es la de CTaskComplexPartnerDeal)
//
// Cada caso se ejecuta por la entrada del EXE (via=0) o por la ranura de la vtable del EXE (via=1: el EXE construye los objetos con su vtable y llama por ella).
// Familia = n % 16: tipo de `this` = familia % 4 (0 arena alineado, 1 arena desalineado, 2 nulo o crudo, 3 arena alineado con la vtable del EXE en +0) y relleno = familia / 4.
#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

namespace F120AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 5;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kObjWin = 0x100, kAWin = 0x40;   // arena del objeto (this) y arena secundaria centinela (no la toca nadie)
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// vtables del EXE (direccion, ranura) de cada funcion
inline constexpr std::array<u32, kFunctions> kVtable{0x85F330u, 0x86FB5Cu, 0x86D894u, 0x870664u, 0x870754u};
inline constexpr std::array<u32, kFunctions> kSlot{1u, 4u, 4u, 4u, 4u};
inline constexpr std::array<u32, kFunctions> kTaskType{0u, 1219u, 1000u, 256u, 1202u};

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 100001u); }
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

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    std::array<u32, 16> a{};
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

inline constexpr std::array<u32, 10> kRawPtrs{1u, 0xFFFFFFFFu, 0x80000000u, 0xCDCDCDCDu, 0x00C0FFEEu, 0x7FFFFFFFu, 0x00010000u, 0xFDFDFDFDu, 0x00000004u, 0x0BADF00Du};
inline u32 RawPtr(Gen& g) { return g.P(60) ? kRawPtrs[g.R(10)] : g.W() | 1u; }
// Un valor de bool de 32 bits tal como lo apila el llamador (solo cuenta el byte bajo para la funcion; el resto lo ignora)
inline u32 BoolWord(Gen& g) { return g.Pk({0u, 1u, 0u, 1u, 2u, 0x80u, 0xFFu, 0x100u, 0x101u, 0xFFFFFFFFu, 0xFFFFFF00u, 0x01010100u}); }
inline u32 EventWord(Gen& g) { return g.P(35) ? g.R(0x40) : g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x100u, 0x1000u, 0xFFFFu, g.W()}); }

// ------------------------------------------------------------------------------------------------ generador (las cinco funciones comparten forma)
//   a[0] = tipo de this (0 arena, 1 nulo, 2 crudo), a[1] = valor crudo, a[2] = desplazamiento de this en la arena (0..0x3F), a[3] = via (0 entrada del EXE, 1 ranura de la vtable del EXE),
//   a[4] = relleno de la arena (0 aleatorio, 1 ceros, 2 unos, 3 0xCD), a[5] = vtable del EXE escrita en +0 de this (1) o no (0),
//   a[6..10] = argumentos de AddScriptSayEvent (audioEvent, scriptID, overrideSilence, isForceAudible, isFrontEnd); en las GetTaskType se apilan 0 argumentos y quedan sin uso.
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[3] = (n >> 4) & 1;
    c.a[4] = f / 4;
    switch (kind) {
    case 0: c.a[0] = 0; c.a[2] = 4 * g.R(16); c.a[5] = 0; break;
    case 1: c.a[0] = 0; c.a[2] = (g.R(15) * 4) + 1 + g.R(3); c.a[5] = 0; if (c.a[2] >= 0x40) c.a[2] -= 0x20; break;
    case 2: c.a[0] = g.P(35) ? 1u : 2u; c.a[2] = 0; c.a[5] = 0; break;
    default: c.a[0] = 0; c.a[2] = 4 * g.R(16); c.a[5] = 1; break;
    }
    c.a[1] = RawPtr(g);
    c.a[6] = EventWord(g);
    c.a[7] = EventWord(g);
    c.a[8] = BoolWord(g);
    c.a[9] = BoolWord(g);
    c.a[10] = BoolWord(g);
    // dobla la frecuencia de las tres banderas distintas de cero en las familias pares y de ninguna en las impares de `kind` 1 (cubre las ramas de las mutaciones del banco)
    if ((n / 32) % 4 == 1) { c.a[8] = 1; c.a[9] = 1; c.a[10] = 1; }
    if ((n / 32) % 4 == 2) { c.a[8] = 0; c.a[9] = 0; c.a[10] = 0; }
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    MakeAll(c, n);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    return j.End();
}
}

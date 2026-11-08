#pragma once
// Fixtures del banco Fn121A (lote 121A): 16384 casos por funcion (16384 pares por funcion). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
// Seis funciones diminutas del EXE (thiscall, ECX = this; solo escriben la vtable en [this] y vuelven; sin argumentos de pila, sin colaboradores):
//   0 CTaskAllocator::~CTaskAllocator (cuerpo)                      0x69BB40  `mov [ecx],870E30h; ret` (7 B)            vtable 0x870E30 (CTaskAllocator)
//   1 CEntitySeekPosCalculator::~CEntitySeekPosCalculator (cuerpo)  0x46A610  `mov [ecx],859DC4h; ret` (7 B)            vtable 0x859DC4 (una ranura: la pura)
//   2 CPedGroupDefaultTaskAllocatorFollowLimited::ctor              0x5F6480  `mov eax,ecx; mov [eax],86C764h; ret` (9 B)  vtable 0x86C764, GetType = 1
//   3 CPedGroupDefaultTaskAllocatorStandStill::ctor                 0x5F64B0  `mov eax,ecx; mov [eax],86C76Ch; ret` (9 B)  vtable 0x86C76C, GetType = 2
//   4 CPedGroupDefaultTaskAllocatorChat::ctor                       0x5F64E0  `mov eax,ecx; mov [eax],86C774h; ret` (9 B)  vtable 0x86C774, GetType = 3
//   5 CPedGroupDefaultTaskAllocatorSitInLeaderCar::ctor             0x5F6540  `mov eax,ecx; mov [eax],86C784h; ret` (9 B)  vtable 0x86C784, GetType = 4
// (el listado de pendientes las atribuia por cercania a CFormation, CTaskSimpleSetCharDecisionMaker, FollowAnyMeans, FollowLimited, Random y StandStill: las clases reales salen de la vtable que escribe cada una)
//
// Cada caso se ejecuta por la entrada del EXE. `this` apunta a una arena propia (con guardas de 0x40) en un desplazamiento 0..0xFF (alineado o no, incluido el borde donde la escritura de 4 bytes entra en la guarda).
// El objeto parte de una imagen previa conocida: relleno de la arena (familia / 4) y un dword previo en [this] (a[6]) que el oraculo usa para predecir los bytes que cambian.
// Familia = n % 16: tipo = familia % 4 (0 alineado, 1 desalineado, 2 al borde de la ventana, 3 alineado con dword previo "interesante") y relleno = familia / 4.
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

namespace F121AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 6;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kObjWin = 0x100, kAWin = 0x40;   // arena del objeto (this) y arena secundaria centinela (no la toca nadie)
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// vtable que escribe cada funcion (la del EXE) y si devuelve `this` en EAX (constructores)
inline constexpr std::array<u32, kFunctions> kVtable{0x870E30u, 0x859DC4u, 0x86C764u, 0x86C76Cu, 0x86C774u, 0x86C784u};
inline constexpr std::array<u32, kFunctions> kReturnsThis{0u, 0u, 1u, 1u, 1u, 1u};

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

// ------------------------------------------------------------------------------------------------ generador (las seis funciones comparten forma)
//   a[0] = desplazamiento de this en la arena (0..0xFF), a[1] = tipo (familia % 4), a[2] = relleno de la arena (0 aleatorio, 1 ceros, 2 unos, 3 0xCD), a[3] = via (0: entrada del EXE; reservado),
//   a[6] = dword previo en [this] (imagen previa del objeto).
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[3] = 0;
    switch (kind) {
    case 0: c.a[0] = 4 * g.R(0x40); break;
    case 1: c.a[0] = 4 * g.R(0x3F) + 1 + g.R(3); break;
    case 2: c.a[0] = 0xF8 + g.R(8); break;                                                  // 0xF8..0xFF: desde 0xFD la escritura de 4 bytes entra en la guarda posterior
    default: c.a[0] = 4 * g.R(0x40); break;
    }
    // dword previo: aleatorio / ceros / unos / 0xDEADBEEF / la vtable de la propia funcion / la de otra funcion / la vtable con un byte distinto / un byte igual
    const u32 own = kVtable[c.fn], other = kVtable[(c.fn + 1 + g.R(kFunctions - 1)) % kFunctions];
    switch (kind == 3 ? g.R(9) : g.Pk({0u, 0u, 1u, 2u, 3u, 3u, 4u, 5u, 6u, 7u})) {
    case 0: c.a[6] = g.W(); break;
    case 1: c.a[6] = 0u; break;
    case 2: c.a[6] = 0xFFFFFFFFu; break;
    case 3: c.a[6] = 0xDEADBEEFu; break;
    case 4: c.a[6] = own; break;
    case 5: c.a[6] = other; break;
    case 6: c.a[6] = own ^ (0xFFu << (8 * g.R(4))); break;
    case 7: c.a[6] = own ^ (0xFFFFu << (8 * g.R(3))); break;
    default: c.a[6] = (g.W() & ~(0xFFu << (8 * g.R(4)))) | (own & 0xFFu); break;
    }
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

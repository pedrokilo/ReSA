#pragma once
// Fixtures del banco Fn125A (lote 125A): 16384 casos por funcion (16384 pares por funcion). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
// Trece destructores virtuales con borrado de 30 bytes del EXE (ranura 0 de la vtable de cada clase). Las clases del listado de pendientes se atribuian por cercania: las reales salen del
// desensamblado, de la vtable que escribe cada cuerpo y de la ranura 0 (ver README.md y fixture_oracle.py). Todas tienen la misma forma:
//     push esi; mov esi,ecx; call <cuerpo>; test byte [esp+8],1; je +; push esi; call <operator delete de la clase>; add esp,4; mov eax,esi; pop esi; ret 4
//    idx  direccion  clase (real)                              cuerpo del destructor        operator delete            vtable de la clase
//     0   0x6F1120   CBoat                                      0x6F00F0 (hook)              0x6E2D90 CVehicle (sin hook) 0x8721A0
//     1   0x5B1DE0   CCutsceneObject                            0x5B1A80 (sin hook)          0x5A1F20 CObject (sin hook)  0x868A60
//     2   0x59EB50   CDummyObject                               0x59EAB0 (sin hook)          0x5326F0 CDummy (sin hook)   0x866E78
//     3   0x6C7F90   CMonsterTruck                              0x6C7D10 (sin hook)          0x6E2D90 CVehicle (sin hook) 0x8717D8
//     4   0x4994C0   CTaskComplexBeCop                          0x4994E0 (hook)              0x61A5B0 CTask (hook)        0x85A428
//     5   0x638AC0   CTaskComplexUsePairedAttractor             0x6331E0 (hook)              0x61A5B0                     0x86E258   (el listado decia CTaskComplexBeInGroup)
//     6   0x5F66E0   CTaskComplexBeStill                        0x5F6700 (hook)              0x61A5B0                     0x86C7BC
//     7   0x4B89D0   CTaskComplexCarDriveMissionFleeScene       0x4B89F0 (hook)              0x61A5B0                     0x85B95C
//     8   0x4B8AB0   CTaskComplexCarDriveMissionKillPed         0x4B8AD0 (hook)              0x61A5B0                     0x85B994
//     9   0x46A6C0   CTaskComplexClimb                          0x46A6E0 (hook)              0x61A5B0                     0x859DCC
//    10   0x6289D0   CTaskComplexDestroyCarMelee                0x621DA0 (hook)              0x61A5B0                     0x86D994   (el listado decia CTaskComplexDestroyCar)
//    11   0x628C60   CTaskComplexDestroyCarArmed                0x622010 (hook)              0x61A5B0                     0x86D9C4   (el listado decia CTaskComplexDestroyCarMelee)
//    12   0x629060   CTaskComplexRoadRage                       0x622110 (hook)              0x61A5B0                     0x86D9F0   (el listado decia CTaskComplexDestroyCarArmed)
//
// Cada caso se ejecuta por la entrada del EXE. `this` apunta a una arena propia (guardas de 0x40) en un desplazamiento alineado, desalineado o al borde de la ventana (la escritura entra en la guarda).
// Familia = n % 16: tipo = familia % 4 (0 alineado, 1 desalineado, 2 al borde de la ventana, 3 alineado con datos "interesantes") y relleno = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F125AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 13;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kObjWin = 0x1000;   // arena del objeto (this)
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Extremo de lo que cada funcion lee o escribe a partir de this (bytes): solo el dword de la vtable ([this]); los falsos del cuerpo y del borrado lo leen y el del cuerpo lo escribe
constexpr u32 kSpanBytes = 4;
// Vtable de cada clase (la que el cuerpo real del destructor deja en [this] antes de seguir con la base)
inline constexpr std::array<u32, kFunctions> kVtable{0x8721A0u, 0x868A60u, 0x866E78u, 0x8717D8u, 0x85A428u, 0x86E258u, 0x86C7BCu, 0x85B95Cu, 0x85B994u, 0x859DCCu, 0x86D994u, 0x86D9C4u, 0x86D9F0u};

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
    u32 eax{}, ecx{}, edx{}, stack{};
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

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de this en la arena, a[1] = tipo (familia % 4), a[2] = relleno de la arena (0 aleatorio, 1 ceros, 2 unos, 3 0xCD), a[3] = 0 (reservado), a[4] = ventana de la arena del objeto,
//   a[5] = extremo de lo que toca la funcion a partir de this (bytes), a[6] = dword previo en [this] (la vtable que tenia el objeto al entrar), a[7] = banderas (argumento de pila),
//   a[8] = dword que el falso del cuerpo del destructor deja en [this] (la vtable de la base), a[9] = EAX que devuelven los falsos del cuerpo y del borrado (no es this).
inline u32 Interesting(Gen& g, u32 own, u32 other) {
    // aleatorio / ceros / unos / 0xDEADBEEF / la propia / la de otra clase / la propia con un byte distinto / con dos bytes distintos / un byte igual
    switch (g.Pk({0u, 0u, 1u, 2u, 3u, 3u, 4u, 5u, 6u, 7u})) {
    case 0: return g.W();
    case 1: return 0u;
    case 2: return 0xFFFFFFFFu;
    case 3: return 0xDEADBEEFu;
    case 4: return own;
    case 5: return other;
    case 6: return own ^ (0xFFu << (8 * g.R(4)));
    case 7: return own ^ (0xFFFFu << (8 * g.R(3)));
    default: return (g.W() & ~(0xFFu << (8 * g.R(4)))) | (own & 0xFFu);
    }
}

inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    const u32 fn = c.fn;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[3] = 0;
    c.a[4] = u32(kObjWin);
    c.a[5] = kSpanBytes;
    const u32 win = u32(kObjWin), span = kSpanBytes;
    switch (kind) {
    case 0: c.a[0] = 4 * g.R((win - span) / 4 + 1); break;
    case 1: c.a[0] = 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3); break;
    case 2: c.a[0] = win - span - 4 + g.R(8); break;                           // desde 4 bytes antes del borde hasta 3 bytes dentro de la guarda posterior
    default: c.a[0] = 4 * g.R((win - span) / 4 + 1); break;
    }
    const u32 own = kVtable[fn], other = kVtable[(fn + 1 + g.R(kFunctions - 1)) % kFunctions];
    c.a[6] = Interesting(g, own, other);
    switch (g.R(10)) {
    case 0: c.a[7] = 0; break;
    case 1: c.a[7] = 1; break;
    case 2: c.a[7] = g.Pk({2u, 4u, 0x80u, 0xFEu, 0x100u, 0x10000u, 0x80000000u, 0xFFFFFF00u, 0xFFFFFE00u}); break;
    case 3: c.a[7] = g.Pk({3u, 5u, 0x81u, 0xFFu, 0x101u, 0x10001u, 0x80000001u, 0xFFFFFF01u, 0xFFFFFF03u, 0x12345679u}); break;
    case 4: c.a[7] = g.W() & ~1u; break;
    case 5: c.a[7] = g.W() | 1u; break;
    case 6: c.a[7] = 0xFFFFFFFFu; break;
    default: c.a[7] = g.W(); break;
    }
    // vtable que deja el cuerpo: la de la base (otra clase), la propia, la previa (no cambia nada), cero, unos o aleatoria
    switch (g.R(8)) {
    case 0: c.a[8] = other; break;
    case 1: c.a[8] = own; break;
    case 2: c.a[8] = c.a[6]; break;
    case 3: c.a[8] = 0; break;
    case 4: c.a[8] = 0xFFFFFFFFu; break;
    case 5: c.a[8] = c.a[6] ^ (0xFFu << (8 * g.R(4))); break;
    default: c.a[8] = g.W(); break;
    }
    c.a[9] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W(), 0x00BADC00u + g.R(256)});
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.ecx = Rand(n, 3) | 1u; c.stack = StackSeed(n);
    MakeAll(c, n);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    return j.End();
}
}

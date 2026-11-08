#pragma once
// Fixtures del banco Fn127A (lote 127A): 16384 casos por funcion (16384 pares por funcion). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
// Veintidos destructores virtuales con borrado de 30 bytes del EXE (ranura 0 de la vtable de cada clase). Las clases del listado de pendientes se atribuian por cercania: las reales salen del
// desensamblado, de la vtable que escribe cada cuerpo y de la ranura 0 (ver README.md y fixture_oracle.py). Todas tienen la misma forma:
//     push esi; mov esi,ecx; call <cuerpo>; test byte [esp+8],1; je +; push esi; call <operator delete de la clase>; add esp,4; mov eax,esi; pop esi; ret 4
//    idx  direccion  clase (real)                              cuerpo del destructor        operator delete   vtable de la clase
//   0   0x421250   CTaskComplexLeaveAnyCar                          0x421270 (hook)   0x61A5B0 CTask (hook) 0x858EB0
//   1   0x6424F0   CTaskComplexScreamInCarThenLeave                 0x63C2C0 (hook)   0x61A5B0 CTask (hook) 0x86E8D8
//   2   0x642370   CTaskComplexLeaveCarAndFlee                      0x63C030 (hook)   0x61A5B0 CTask (hook) 0x86E8AC
//   3   0x640E40   CTaskComplexEnterBoatAsDriver                    0x63B650 (hook)   0x61A5B0 CTask (hook) 0x86E7D0
//   4   0x6577D0   CTaskComplexEvasiveCower                         0x6554E0 (hook)   0x61A5B0 CTask (hook) 0x86F3BC
//   5   0x68DAB0   CTaskComplexKillCriminal                         0x68BF30 (hook)   0x61A5B0 CTask (hook) 0x870A00
//   6   0x643760   CTaskSimpleCarSetTempAction                      0x63D730 (hook)   0x61A5B0 CTask (hook) 0x86EA6C
//   7   0x6637A0   CTaskGangHassleVehicle                           0x65FB60 (hook)   0x61A5B0 CTask (hook) 0x86F9D4
//   8   0x6390A0   CTaskComplexUseScriptedAttractor                 0x633360 (hook)   0x61A5B0 CTask (hook) 0x86E2DC
//   9   0x4B8820   CTaskComplexUseClosestFreeScriptedAttractorRun   0x4B8840 (hook)   0x61A5B0 CTask (hook) 0x85B904
//  10   0x4B88E0   CTaskComplexUseClosestFreeScriptedAttractorSprint 0x4B8900 (hook)   0x61A5B0 CTask (hook) 0x85B930
//  11   0x639190   CTaskComplexOnFire                               0x6333C0 (hook)   0x61A5B0 CTask (hook) 0x86E308
//  12   0x6980F0   CTaskComplexExtinguishFires                      0x6975F0 (hook)   0x61A5B0 CTask (hook) 0x870DD0
//  13   0x656E50   CTaskComplexWalkRoundBuildingAttempt             0x654950 (hook)   0x61A5B0 CTask (hook) 0x86F334
//  14   0x676330   CTaskInteriorBeInOffice                          0x675260 (hook)   0x61A5B0 CTask (hook) 0x870218
//  15   0x676860   CTaskInteriorUseInfo                             0x675A90 (hook)   0x61A5B0 CTask (hook) 0x8702E8
//  16   0x677760   CTaskInteriorSitAtDesk                           0x676080 (hook)   0x61A5B0 CTask (hook) 0x87035C
//  17   0x676620   CTaskInteriorGoToInfo                            0x675810 (hook)   0x61A5B0 CTask (hook) 0x8702BC
//  18   0x6761E0   CTaskInteriorBeInHouse                           0x675000 (hook)   0x61A5B0 CTask (hook) 0x8701E8
//  19   0x64CC40   CTaskSimpleCarFallOut                            0x648DE0 (hook)   0x61A5B0 CTask (hook) 0x86EFD0
//  20   0x64ACE0   CTaskSimpleCarOpenDoorFromOutside                0x645EE0 (hook)   0x61A5B0 CTask (hook) 0x86EC9C
//  21   0x64B000   CTaskSimpleCarCloseDoorFromOutside               0x646570 (hook)   0x61A5B0 CTask (hook) 0x86ED2C
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

namespace F127AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 22;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kObjWin = 0x1000;   // arena del objeto (this)
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Extremo de lo que cada funcion lee o escribe a partir de this (bytes): solo el dword de la vtable ([this]); los falsos del cuerpo y del borrado lo leen y el del cuerpo lo escribe
constexpr u32 kSpanBytes = 4;
// Vtable de cada clase (la que el cuerpo real del destructor deja en [this] antes de seguir con la base)
inline constexpr std::array<u32, kFunctions> kVtable{0x858EB0u, 0x86E8D8u, 0x86E8ACu, 0x86E7D0u, 0x86F3BCu, 0x870A00u, 0x86EA6Cu, 0x86F9D4u, 0x86E2DCu, 0x85B904u, 0x85B930u, 0x86E308u, 0x870DD0u, 0x86F334u, 0x870218u, 0x8702E8u, 0x87035Cu, 0x8702BCu, 0x8701E8u, 0x86EFD0u, 0x86EC9Cu, 0x86ED2Cu};

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

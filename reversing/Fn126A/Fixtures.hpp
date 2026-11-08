#pragma once
// Fixtures del banco Fn126A (lote 126A): 16384 casos por funcion (16384 pares por funcion). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
// Trece destructores virtuales con borrado de 30 bytes del EXE (ranura 0 de la vtable de cada clase). Las clases del listado de pendientes se atribuian por cercania: las reales salen del
// desensamblado, de la vtable que escribe cada cuerpo y de la ranura 0 (ver README.md y fixture_oracle.py). Todas tienen la misma forma:
//     push esi; mov esi,ecx; call <cuerpo>; test byte [esp+8],1; je +; push esi; call <operator delete de la clase>; add esp,4; mov eax,esi; pop esi; ret 4
//    idx  direccion  clase (real)                              cuerpo del destructor        operator delete   vtable de la clase
//    0   0x640590   CTaskComplexEnterCarAsDriverTimed        0x63AE00 (hook)   0x61A5B0 CTask (hook) 0x86E720
//    1   0x640F10   CTaskComplexStealCar                     0x63B850 (hook)   0x61A5B0 CTask (hook) 0x86E7FC
//    2   0x640640   CTaskComplexEnterCarAsPassengerTimed     0x63B0C0 (hook)   0x61A5B0 CTask (hook) 0x86E74C
//    3   0x640710   CTaskComplexEnterCarAsPassengerWait      0x63B3C0 (hook)   0x61A5B0 CTask (hook) 0x86E778
//    4   0x640BC0   CTaskComplexLeaveCarAsPassengerWait      0x63B4A0 (hook)   0x61A5B0 CTask (hook) 0x86E7A4
//    5   0x6560C0   CTaskComplexHitPedWithCar                0x653A30 (hook)   0x61A5B0 CTask (hook) 0x86F294
//    6   0x697820   CTaskComplexInvestigateDeadPed           0x696E80 (hook)   0x61A5B0 CTask (hook) 0x870D3C
//    7   0x6629F0   CTaskComplexGangFollower                 0x65EBB0 (hook)   0x61A5B0 CTask (hook) 0x86F938
//    8   0x64ABE0   CTaskSimpleCarAlign                      0x645BE0 (hook)   0x61A5B0 CTask (hook) 0x86EC78
//    9   0x697B50   CTaskComplexReactToGunAimedAt            0x697230 (hook)   0x61A5B0 CTask (hook) 0x870D78
//   10   0x68F9F0   CTaskComplexCopInCar                     0x68C8C0 (hook)   0x61A5B0 CTask (hook) 0x870A2C
//   11   0x6296B0   CTaskSimpleStealthKill                   0x622680 (sin hook)   0x61A5B0 CTask (hook) 0x86DA74
//   12   0x626FA0   CTaskComplexKillPedOnFootArmed           0x621250 (hook)   0x61A5B0 CTask (hook) 0x86D918
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

namespace F126AFix {
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
inline constexpr std::array<u32, kFunctions> kVtable{0x86E720u, 0x86E7FCu, 0x86E74Cu, 0x86E778u, 0x86E7A4u, 0x86F294u, 0x870D3Cu, 0x86F938u, 0x86EC78u, 0x870D78u, 0x870A2Cu, 0x86DA74u, 0x86D918u};

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

#pragma once
// Fixtures del banco Fn188A (lote 188A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (sin hook antes del lote; las clases reales salen del desensamblado y de las vtables, ver README.md y fixture_oracle.py). 13 reconstruidas.
//   idx  direccion  funcion real                                                                forma
//    0   0x5FEF70   CEventScriptedAttractor cuerpo del destructor                              thiscall() `ret`: vtable 86C9F0h; jmp 4AF400
//    1   0x5FEF80   CEventScriptedAttractor::GetEventType (V)                                  thiscall() `ret`: mov eax,30h
//    2   0x5FEF90   CEventScriptedAttractor::CloneEditable (V)                                 thiscall() `ret`: new(20h) 4B5620; si no es nulo: ctor 4AF350(+14h, +18h, 0); vtable 86C9F0h; eax = memoria
//    3   0x5FEFF0   CEventScriptedAttractor destructor con borrado                             thiscall(banderas) `ret 4`: 5FEF70; delete 4B5630 si bit 0; eax = this
//    4   0x5FF030   CEventAcquaintancePedRespect cuerpo del destructor                         thiscall() `ret`: vtable 86CA38h; jmp 4AF890
//    5   0x5FF050   CEventAcquaintancePedRespect::CloneEditable (V)                            thiscall() `ret`: new(18h) 4B5620; si no es nulo: ctor 4AF820(+14h); vtable 86CA38h; eax = memoria
//    6   0x5FF0B0   CEventAcquaintancePedRespect destructor con borrado                        thiscall(banderas) `ret 4`: 5FF030; delete 4B5630 si bit 0; eax = this
//    7   0x5FF0F0   CEventAcquaintancePedLike cuerpo del destructor                            thiscall() `ret`: vtable 86CA80h; jmp 4AF890
//    8   0x5FF110   CEventAcquaintancePedLike::CloneEditable (V)                               thiscall() `ret`: new(18h) 4B5620; si no es nulo: ctor 4AF820(+14h); vtable 86CA80h; eax = memoria
//    9   0x5FF170   CEventAcquaintancePedLike destructor con borrado                           thiscall(banderas) `ret 4`: 5FF0F0; delete 4B5630 si bit 0; eax = this
//   10   0x5FF1B0   CEventAcquaintancePedDislike cuerpo del destructor                         thiscall() `ret`: vtable 86CAC8h; jmp 4AF890
//   11   0x5FF1D0   CEventAcquaintancePedDislike::CloneEditable (V)                            thiscall() `ret`: new(18h) 4B5620; si no es nulo: ctor 4AF820(+14h); vtable 86CAC8h; eax = memoria
//   12   0x5FF230   CEventAcquaintancePedDislike destructor con borrado                        thiscall(banderas) `ret 4`: 5FF1B0; delete 4B5630 si bit 0; eax = this
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A de 0x5000 bytes, B, C y D de 0x1000, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
// Familia = n % 16: tipo = familia % 4 (posicion del objeto: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F188AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 13;
constexpr unsigned kArenas = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;    // ventana de las arenas B, C y D
constexpr std::size_t kWinA = 0x1000;   // ventana de la arena A (igual que las demas: ningun objeto del lote pasa de 0x20 bytes)
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

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
// Relleno de una region: 0 aleatorio (semilla), 1 ceros, 2 unos, 3 0xCD
inline void FillKind(u8* dst, std::size_t bytes, u32 kind, u32 seed) {
    if (kind == 0) FillWords(dst, bytes, seed);
    else std::memset(dst, kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), bytes);
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
    Js& H(const char* k, const std::vector<u8>& v) {
        constexpr char d[] = "0123456789abcdef";
        Key(k); s += '"';
        for (u8 b : v) { s += d[b >> 4]; s += d[b & 15]; }
        s += '"'; return *this;
    }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
constexpr unsigned kSlots = 64;       // dwords de parametros del caso; a[60..63] son las semillas de las arenas
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, kSlots> a{};
    std::vector<u8> s;          // datos en bytes (sin uso en este lote)
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

// Desplazamiento de un objeto de `span` bytes en la ventana de la arena A segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWinA);
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B, C o D), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
// Un puntero crudo no nulo
inline u32 RawNz(Gen& g) { u32 v; do { v = Raw(g); } while (v == 0); return v; }

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}

// Clases de funcion (forma)
enum Kind : u32 { kBodyJmp, kConstType, kEvClone, kSdd };
inline constexpr std::array<Kind, kFunctions> kKind{kBodyJmp, kConstType, kEvClone, kSdd, kBodyJmp, kEvClone, kSdd, kBodyJmp, kEvClone, kSdd, kBodyJmp, kEvClone, kSdd};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x5FEF70, 0x5FEF80, 0x5FEF90, 0x5FEFF0, 0x5FF030, 0x5FF050, 0x5FF0B0, 0x5FF0F0, 0x5FF110, 0x5FF170, 0x5FF1B0, 0x5FF1D0, 0x5FF230};
// Bytes que cada funcion toca a partir de su puntero `this` (CloneEditable lee +14h y, la de CEventScriptedAttractor, +18h)
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 0x20, 4, 4, 0x18, 4, 4, 0x18, 4, 4, 0x18, 4};

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[60..63] = semillas de las arenas A, B, C y D
//   kConstType: nada mas
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que el falso deja en [this], a[6] = banderas (pila)
//   kBodyJmp: a[4] = EAX del falso del colaborador, a[5] = dword que deja en [this]
//   kEvClone: a[4] = valor de +14h (crudo), a[5] = valor de +18h (crudo), a[6] = memoria de new (0 nula, 1 en B), a[7] = desplazamiento en B, a[8] = EAX del falso del constructor, a[9] = dword que deja en [mem]
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[60] = c.seed * 13u + 50u; c.a[61] = c.a[60] + 1u; c.a[62] = c.a[60] + 2u; c.a[63] = c.a[60] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x85B1B8u, 0x85AB38u, 0x85B250u, 0x86C9F0u, 0x86CA38u, 0x86CA80u, 0x86CAC8u});
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kConstType:
        break;
    case kSdd:
        a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W()});
        a[5] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 1u});
        a[6] = FlagsDword(g);
        break;
    case kBodyJmp:
        a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W()});
        a[5] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 1u});
        break;
    case kEvClone:
        a[4] = Raw(g); a[5] = Raw(g);
        a[6] = g.Pk({0u, 1u, 1u, 1u});
        a[7] = AuxOffset(g, 0x40);
        a[8] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W()});
        a[9] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu});
        break;
    }
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
    j.H("s", c.s);
    return j.End();
}
}

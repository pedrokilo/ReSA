#pragma once
// Fixtures del banco Fn123A (lote 123A): 16384 casos de una funcion (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
// Una funcion del EXE (thiscall, ECX = this, sin argumentos de pila, `ret`, marco SEH que solo desenrolla):
//   0 CTaskComplexPartner::~CTaskComplexPartner (cuerpo no virtual)   0x683A40  (marco SEH 0x8468B8)
//       vtable 0x870664; si [this+0x38] != 0 -> CEntity::CleanUpOldReference 0x571A00 (ECX = el companero, push &this->m_partner);
//       si el byte [this+0x6E] != 0: si el byte [this+0x5E] difiere del byte de 0x858B54 (`repe cmps`, ECX = 1; ese byte vale 0) -> 0x4D3990 (GetAnimationBlockIndex por nombre, cdecl) y
//       0x4D3FD0 (RemoveAnimBlockRef, cdecl) con su resultado; byte [this+0x6E] = 0; al final -> CTaskComplex::Destructor 0x61A3D0 (ECX = this).
//
// Cada caso se ejecuta por la entrada del EXE. `this` apunta a una arena propia (con guardas de 0x40) en un desplazamiento alineado, desalineado o al borde de la ventana (el byte +0x6E entra en la guarda).
// El objeto parte de una imagen previa conocida que el oraculo usa para predecir los bytes que cambian.
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

namespace F123AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kObjWin = 0x180, kAWin = 0x40;   // arena del objeto (this) y arena secundaria (companeros apuntados dentro de ella)
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kSpan = 0x70;                    // sizeof(CTaskComplexPartner)

constexpr u32 kGlobalAddr = 0xB5D4A0u;    // ms_aAnimBlocks: nadie debe tocarlo (RemoveAnimBlockRef esta falseado en ambos lados)
constexpr std::size_t kGlobalBytes = 0x40;
constexpr u32 kVtable = 0x870664u;        // vtable de CTaskComplexPartner en el EXE
inline constexpr std::array<u32, 8> kOtherVtables{0x870754u, 0x870800u, 0x86C0A8u, 0x86EAACu, 0x870ADCu, 0x86FD74u, 0x86DD50u, 0x86D894u};

// Desplazamientos de los campos de CTaskComplexPartner
constexpr std::size_t kOffPartner = 0x38, kOffName = 0x5E, kOffFlag = 0x6E;
constexpr std::size_t kNameLen = 16;

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
//   a[0] = desplazamiento de this en la arena, a[1] = tipo (familia % 4), a[2] = relleno de la arena (0 aleatorio, 1 ceros, 2 unos, 3 0xCD),
//   a[3] = companero [this+0x38] como etiqueta de arena (0x10000000 + (arena << 20) + desplazamiento desde el inicio de la arena) o valor crudo (nunca en 0x1xxxxxxx),
//   a[4] = byte [this+0x6E] (m_requiredAnimsStreamedIn, el EXE lo prueba entero), a[5] = dword previo en [this] (vtable previa),
//   a[6..9] = los 16 bytes del nombre del bloque en [this+0x5E] (el primer byte decide la rama del `repe cmps`), a[10] = lo que devuelve GetAnimationBlockIndex (falso),
//   a[11] = ventana de la arena del objeto, a[12] = extremo del objeto (0x70).
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
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[11] = u32(kObjWin);
    c.a[12] = u32(kSpan);
    switch (kind) {
    case 0: c.a[0] = 4 * g.R(0x40); break;
    case 1: c.a[0] = 4 * g.R(0x3F) + 1 + g.R(3); break;
    case 2: c.a[0] = u32(kObjWin) - u32(kSpan) - 4 + g.R(8); break;           // el byte +0x6E queda desde 5 bytes antes del borde hasta 1 byte dentro de la guarda
    default: c.a[0] = 4 * g.R(0x40); break;
    }
    // companero
    const u32 pk = g.R(100);
    if (pk < 25) c.a[3] = 0;
    else if (pk < 48) c.a[3] = 0x10100000u + g.R(u32(Arena(kAWin)));                       // dentro de la arena secundaria
    else if (pk < 58) c.a[3] = 0x10000000u + u32(kGuard) + c.a[0];                           // el propio objeto
    else if (pk < 68) c.a[3] = 0x10000000u + g.R(u32(Arena(kObjWin)));                       // otro punto de la arena del objeto
    else if (pk < 85) c.a[3] = g.Pk({1u, 2u, 0x7F000000u, 0x7F001234u, 0xFFFFFFFFu, 0x80000000u, 0x00400000u, 0x00858B54u, 0x00B5D4A0u});
    else { u32 w = g.W(); if ((w >> 28) == 1) w ^= 0x10000000u; c.a[3] = w; }
    // byte de bandera
    const u32 fk = g.R(100);
    c.a[4] = fk < 35 ? 0u : fk < 55 ? 1u : fk < 60 ? 0x80u : fk < 65 ? 0xFFu : fk < 70 ? 2u : (g.W() & 0xFFu);
    // nombre del bloque: 16 bytes; con probabilidad 35% el primer byte es cero (basura detras); si no, nonulo (incluidos 0x80 y 0xFF)
    for (int i = 0; i < 4; ++i) c.a[6 + i] = g.W();
    const u32 nk = g.R(100);
    u32 first = nk < 35 ? 0u : nk < 40 ? 0x80u : nk < 45 ? 0xFFu : nk < 50 ? 1u : (g.W() & 0xFFu);
    if (nk >= 35 && first == 0) first = 0x67;
    if (g.P(20)) { c.a[7] = 0; c.a[8] = 0; c.a[9] = 0; }                                   // resto del nombre vacio
    c.a[6] = (c.a[6] & ~0xFFu) | first;
    // lo que devuelve GetAnimationBlockIndex
    c.a[10] = g.Pk({0u, 0xFFFFFFFFu, 1u, 0x7FFFFFFFu, 0x80000000u, 0x42u, g.W(), g.R(0x80)});
    // vtable previa
    c.a[5] = Interesting(g, kVtable, kOtherVtables[g.R(8)]);
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

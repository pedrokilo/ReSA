#pragma once
// Fixtures del banco Fn218A (lote 218A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (CON hook del lote 218A, sin validar hasta este banco). 32 funciones (plantilla: Fn216A):
//   0..9    cuerpos de destructor de FxInfo{SpriteRect,HeatHaze,Trail,Flat,Dir,AnimTexture,ColourRange,SelfLit,ColourBright,Smoke}_c (0x4A76C0 0x4A7730 0x4A77A0 0x4A7810 0x4A7880 0x4A78F0 0x4A7960 0x4A79D0 0x4A7A40 0x4A7AB0):
//           thiscall() `ret`, SEH propio: destructor del interp de +8 (REAL: 0x4A8B70, 0x4A87F0, 0x4A8460 o 0x4A89B0: vtable 0x85A968) y [this] = 0x85A7DC
//   10..18  destructores con borrado de FxInfo{HeatHaze,Trail,Flat,Dir,AnimTexture,ColourRange,SelfLit,ColourBright,Smoke}_c (0x4A7710 0x4A7780 0x4A77F0 0x4A7860 0x4A78D0 0x4A7940 0x4A79B0 0x4A7A20 0x4A7A90): thiscall(banderas) `ret 4`:
//           cuerpo (falso) y operator delete 0x8214BD (falso) de this si el bit 0 del BYTE de banderas; eax = this
//   19..22  constructores de FxInterpInfo{Float,U255,32,255}_c (0x4A8440 0x4A87D0 0x4A8990 0x4A8B50): thiscall() `ret`: byte +4 = 0, dword +8 = 0, [this] = vtable del interp, dword +0xC = 0; eax = this (ecx = 0)
//   23..26  cuerpos de destructor de esos cuatro interp (0x4A8460 0x4A87F0 0x4A89B0 0x4A8B70): thiscall() `ret`: [this] = 0x85A968
//   27..30  destructores con borrado de esos cuatro interp (0x4A8D30 0x4A8D50 0x4A8D70 0x4A8D90): como 10..18 (cuerpo 0x4A8460 ... falso)
//   31      ListItem_c::Constructor (0x4A8DB0): thiscall() `ret`: dword +0 = 0, dword +4 = 0; eax = this
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
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

namespace F218AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 32;
constexpr unsigned kArenas = 4;
constexpr unsigned kSlots = 96;         // dwords de entrada por caso

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
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

// Desplazamiento de un objeto de `span` bytes en la ventana segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWin);
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B, C o D), sin invadir la guarda
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
inline u32 RawNZ(Gen& g) { const u32 v = Raw(g); return v ? v : 0x00C00000u + g.R(0x1000); }

// Un float "interesante" (bits): ceros con signo, unos, bordes, denormales, enormes, infinitos, NaN tranquilos y de senalizacion, aleatorios y valores de color
inline u32 FloatBits(Gen& g, bool allowNaN) {
    const u32 r = g.W();
    const u32 w = g.R(0x10000);
    const u32 pick = g.R(24);
    switch (pick) {
    case 0: return 0u;
    case 1: return 0x80000000u;
    case 2: case 3: return 0x3F800000u;                         // 1.0
    case 4: return 0x3F000000u;                                 // 0.5
    case 5: return 0x40000000u;                                 // 2.0
    case 6: return 0x437F0000u;                                 // 255.0
    case 7: return 0x3B808081u;                                 // 1/255
    case 8: case 9: return r & 0x007FFFFFu;                     // denormal positivo
    case 10: return (r & 0x007FFFFFu) | 0x80000000u;            // denormal negativo
    case 11: return 0x7F7FFFFFu - (r & 0xFFu);                  // casi el mayor float
    case 12: return 0xFF7FFFFFu - (r & 0xFFu);
    case 13: return g.P(50) ? 0x7F800000u : 0xFF800000u;        // infinito
    case 14: return allowNaN ? (0x7FC00000u | (r & 0x003FFFFFu) | (g.P(50) ? 0x80000000u : 0u)) : r;   // NaN tranquilo
    case 15: return allowNaN ? (0x7F800000u | ((r & 0x003FFFFFu) ? (r & 0x003FFFFFu) : 1u) | (g.P(50) ? 0x80000000u : 0u)) : r;   // NaN de senalizacion
    case 16: case 17: return 0x3F800000u + ((r & 0xFFFFu) << 7) - 0x00200000u;   // cerca de 1
    case 18: case 19: return 0x3F000000u + ((r & 0x7FFFFFu));   // [0.5, 1)
    case 20: return 0x43000000u + ((r & 0x7FFFFFu) >> 1);       // 128..
    case 21: return (r & 0x00FFFFFFu) | 0x30000000u;            // pequeno
    default: return r;                                           // cualquier patron
    }
    (void)w;
}
inline bool IsNaNBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x007FFFFFu) != 0; }

// Clases de funcion (forma)
enum Kind : u32 { kBody, kSdd, kICtor, kIBody, kLi };
inline constexpr unsigned kNFxBody = 10, kNFxSdd = 9, kNICtor = 4, kNIBody = 4, kNISdd = 4;
inline constexpr unsigned kFxBody0 = 0, kFxSdd0 = 10, kICtor0 = 19, kIBody0 = 23, kISdd0 = 27, kLi0 = 31;
inline constexpr std::array<Kind, kFunctions> kKind = [] {
    std::array<Kind, kFunctions> a{};
    for (unsigned i = 0; i < kFunctions; ++i)
        a[i] = i < 10 ? kBody : i < 19 ? kSdd : i < 23 ? kICtor : i < 27 ? kIBody : i < 31 ? kSdd : kLi;
    return a;
}();
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{
    0x4A76C0, 0x4A7730, 0x4A77A0, 0x4A7810, 0x4A7880, 0x4A78F0, 0x4A7960, 0x4A79D0, 0x4A7A40, 0x4A7AB0,
    0x4A7710, 0x4A7780, 0x4A77F0, 0x4A7860, 0x4A78D0, 0x4A7940, 0x4A79B0, 0x4A7A20, 0x4A7A90,
    0x4A8440, 0x4A87D0, 0x4A8990, 0x4A8B50,
    0x4A8460, 0x4A87F0, 0x4A89B0, 0x4A8B70,
    0x4A8D30, 0x4A8D50, 0x4A8D70, 0x4A8D90,
    0x4A8DB0};
// Cuerpos de destructor de los FxInfo (diez, indice 0..9) y de los interp (cuatro): colaboradores (falsos) de los destructores con borrado
inline constexpr std::array<u32, kNFxBody> kFxBodyAddr{0x4A76C0, 0x4A7730, 0x4A77A0, 0x4A7810, 0x4A7880, 0x4A78F0, 0x4A7960, 0x4A79D0, 0x4A7A40, 0x4A7AB0};
inline constexpr std::array<u32, kNIBody> kIBodyAddr{0x4A8460, 0x4A87F0, 0x4A89B0, 0x4A8B70};
// Destructor del interp (REAL) que llama el cuerpo de cada FxInfo
inline constexpr std::array<u32, kNFxBody> kDtorInterp{0x4A8B70, 0x4A8B70, 0x4A87F0, 0x4A8460, 0x4A8460, 0x4A89B0, 0x4A87F0, 0x4A87F0, 0x4A87F0, 0x4A87F0};
// Vtable del interp que deja su constructor (Float, U255, 32, 255)
inline constexpr std::array<u32, kNICtor> kICtorVt{0x85A970, 0x85A978, 0x85A980, 0x85A988};
// Cuerpo (colaborador falso) de cada destructor con borrado (indice 10..18 y 27..30)
inline constexpr u32 SddBody(unsigned fn) { return fn < kISdd0 ? kFxBodyAddr[fn - kFxSdd0 + 1] : kIBodyAddr[fn - kISdd0]; }
// Vtables esperadas del EXE en +0 y +8 tras la funcion (0 = no se comprueba)
inline constexpr u32 kBaseVt = 0x85A7DC, kInterpBaseVt = 0x85A968;
inline constexpr u32 ExpVt0(unsigned fn) { return kKind[fn] == kBody ? kBaseVt : kKind[fn] == kICtor ? kICtorVt[fn - kICtor0] : kKind[fn] == kIBody ? kInterpBaseVt : 0; }
inline constexpr u32 ExpVt8(unsigned fn) { return kKind[fn] == kBody ? kInterpBaseVt : 0; }
inline constexpr u32 kSpanBytes = 0x18;      // bytes del objeto que se preparan (hasta +0x17)
inline constexpr u32 kVtMark = 0xA51A51A5u;  // marca de "vtable" (la de la DLL o la esperada del EXE) en los eventos de los falsos

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento del objeto en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4)
//   a[4..9] = seis dwords del objeto (+0, +4, +8, +0xC, +0x10, +0x14) previos a la llamada, aplicados solo si a[40+i] != 0 (si no, el relleno de la arena)
//   kSdd: a[13] = banderas (dword de pila); a[14] = EAX que devuelve el falso del cuerpo
//   a[92..95] = semillas de las arenas A, B, C y D
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    auto& a = c.a;
    a[1] = kind;
    a[2] = f / 4;
    a[92] = c.seed * 13u + 50u; a[93] = a[92] + 1u; a[94] = a[92] + 2u; a[95] = a[92] + 3u;
    a[0] = Offset(g, kind, kSpanBytes);
    const u32 vt0 = ExpVt0(c.fn) ? ExpVt0(c.fn) : kBaseVt;
    const u32 vt8 = ExpVt8(c.fn) ? ExpVt8(c.fn) : kInterpBaseVt;
    for (u32 i = 0; i < 6; ++i) {
        a[40 + i] = g.P(55) ? 1u : 0u;
        a[4 + i] = g.Pk({0u, 0u, 0xFFFFFFFFu, g.W(), g.W(), vt0, vt8, kBaseVt, kInterpBaseVt, 0xDEADBEEFu, 0x0000FFFFu, 0xFFFF0000u, 0x20202020u, 0x00FF00FFu, 0x80808080u});
    }
    if (kKind[c.fn] == kSdd) {
        a[13] = g.Pk({0u, 1u, 1u, 1u, 2u, 3u, 0xFEu, 0xFFu, 0xFFFFFFFFu, 0x100u, 0x101u, 0x80000001u, g.W(), g.W() | 1u, g.W() & ~1u});
        a[14] = g.Pk({0u, 1u, 0xDEADBEEFu, g.W()});
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

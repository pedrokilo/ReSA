#pragma once
// Fixtures del banco Fn217A (lote 217A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (sin validar antes del banco; las clases salen del desensamblado, ver README.md y fixture_oracle.py). 31 entradas (0x4A6D30, cuerpo del destructor de FxInfoEmPos_c, tambien figura en el
// informe del 216A y lo valida el banco Fn216A: no entra aqui).
//   idx    forma
//   0-14   cuerpos de destructor (67 B, marco SEH) de FxInfoEmWeather_c, EmRotation, Noise, Force, Friction, AttractPt, AttractLine, GroundCollide, Wind, Jitter, RotSpeed, Float, Underwater, Colour, Size:
//          thiscall() `ret`; destructor del miembro +8 (0x4A8460 / 0x4A89B0 / 0x4A87F0: solo deja la vtable 0x85A968) y despues [this] = 0x85A7DC
//   15-30  destructores con borrado (27 B) de las mismas clases y de FxInfoSpriteRect_c: thiscall(banderas) `ret 4`: cuerpo (entrada + 0x20) y, con el bit 0 del byte, operator delete 0x8214BD; devuelve this
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

namespace F217AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 31;
constexpr unsigned kArenas = 4;
constexpr unsigned kSlots = 128;        // dwords de entrada por caso

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
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, kSlots> a{};
    std::array<u32, 4> sd{};    // semillas de las arenas A..D
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    bool Pm(u32 permille) { return Rand(n, nk()) % 1000 < permille; }
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
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B, C o D), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
// La pila del hilo del banco (cerca de 0x00EB0000) se etiqueta aparte (Tag): un puntero crudo que caiga ahi se confundiria con un objeto de la pila, asi que ninguno se genera en esa zona
inline u32 AvoidStack(u32 v) { return (v >= 0x00800000u && v < 0x02000000u) ? (v ^ 0x40000000u) : v; }
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return AvoidStack(g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu}));
}
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
// Un float uniforme en [lo, hi] como bits (la aritmetica en double es determinista; el oraculo solo lee los bits)
inline u32 UniformBits(Gen& g, double lo, double hi) {
    const double u = double(g.R(1u << 24)) / 16777216.0;
    const float v = float(lo + u * (hi - lo));
    return std::bit_cast<u32>(v);
}
// Mezcla: la mitad uniforme en [lo, hi], una cuarta parte de valores especiales de la lista y otra de FloatBits
inline u32 Mixed(Gen& g, double lo, double hi, std::initializer_list<u32> specials) {
    const u32 r = g.R(8);
    if (r < 4) return UniformBits(g, lo, hi);
    if (r < 6) return g.Pk(specials);
    return FloatBits(g);
}


// Clases de funcion (forma)
enum Kind : u32 { kBody, kDel };
constexpr unsigned kBodies = 15;
inline constexpr std::array<Kind, kFunctions> kKind{
    kBody, kBody, kBody, kBody, kBody, kBody, kBody, kBody, kBody, kBody, kBody, kBody, kBody, kBody, kBody,
    kDel, kDel, kDel, kDel, kDel, kDel, kDel, kDel, kDel, kDel, kDel, kDel, kDel, kDel, kDel, kDel};
// Entrada de cada funcion en el EXE (cuerpos y destructores con borrado, en el mismo orden de clases; el ultimo borrado es FxInfoSpriteRect_c)
inline constexpr std::array<u32, kFunctions> kEntry{
    0x4A6DA0, 0x4A6E10, 0x4A6E80, 0x4A6EF0, 0x4A6F60, 0x4A6FD0, 0x4A7040, 0x4A70B0, 0x4A73B0, 0x4A7420, 0x4A7490, 0x4A7500, 0x4A7570, 0x4A75E0, 0x4A7650,
    0x4A6D80, 0x4A6DF0, 0x4A6E60, 0x4A6ED0, 0x4A6F40, 0x4A6FB0, 0x4A7020, 0x4A7090, 0x4A7390, 0x4A7400, 0x4A7470, 0x4A74E0, 0x4A7550, 0x4A75C0, 0x4A7630, 0x4A76A0};
// Destructor del miembro +8 que llama el cuerpo del EXE (solo los cuerpos): Float 0x4A8460, 32 0x4A89B0, U255 0x4A87F0
inline constexpr std::array<u32, kBodies> kInterpDtor{
    0x4A8460, 0x4A8460, 0x4A89B0, 0x4A89B0, 0x4A89B0, 0x4A89B0, 0x4A89B0, 0x4A89B0, 0x4A89B0, 0x4A89B0, 0x4A8460, 0x4A8460, 0x4A87F0, 0x4A87F0, 0x4A89B0};
constexpr u32 kSpan = 0x18;

// ------------------------------------------------------------------------------------------------ disposicion de las entradas (a[])
//   a[0] = desplazamiento del objeto en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[4] = dword previo en [this+8],
//   a[5..8] = dwords previos en +4, +0xC, +0x10, +0x14 (los destructores no los tocan), a[9] = banderas (kDel; dword con ruido; solo cuenta el bit 0 del byte bajo)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.sd[0] = c.seed * 13u + 50u; c.sd[1] = c.sd[0] + 1u; c.sd[2] = c.sd[0] + 2u; c.sd[3] = c.sd[0] + 3u;
    auto& a = c.a;
    a[0] = Offset(g, kind, kSpan);
    a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x85A7DCu, 0x85A968u, 0x887D5Cu, 0x870114u});
    a[4] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x85A7DCu, 0x85A968u, 0x85A980u});
    for (u32 i = 5; i <= 8; ++i) a[i] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), g.W(), 0x1004u, 0x85A7DCu, 0x85A968u});
    if (kKind[c.fn] == kDel)
        a[9] = g.Pk({0u, 1u, 1u, 2u, 3u, 0xFEu, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0x80000001u, g.W(), g.W()});
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
    std::size_t used = c.a.size();
    while (used > 0 && c.a[used - 1] == 0) --used;
    j.A("a", std::vector<u32>(c.a.begin(), c.a.begin() + used));
    j.A("sd", std::vector<u32>(c.sd.begin(), c.sd.end()));
    return j.End();
}
}

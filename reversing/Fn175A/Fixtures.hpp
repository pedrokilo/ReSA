#pragma once
// Fixtures del banco Fn175A (lote 175A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado y de los llamadores, ver README.md y fixture_oracle.py). 4 asignadas: ninguna tenia hook. Los nombres del listado eran
// erroneos: 0x552520 es el cuerpo del destructor de CQuadTreeNode (no CPtrNodeDoubleLinkPool), 0x66D440 y 0x66D4B0 son el constructor y la copia de CPatrolRoute (no CTaskComplexInvestigateDisturbance) y 0x674930 es el
// constructor de CTaskComplexFollowPatrolRoute (vtable 0x870114; no CTaskComplexWanderProstitute).
//   idx  direccion  nombre real                                                              forma
//    0   0x552520   CQuadTreeNode<void*> cuerpo del destructor (marco SEH)                    thiscall() `ret`: por cada hijo no nulo (+0x14..+0x20): 0x552520(hijo) y 0x552BC0(pool [0xB745BC], hijo); al final Flush 0x552400 de la lista (+0x10)
//    1   0x66D440   CPatrolRoute::Constructor                                                 thiscall() `ret`: +0 = 0, vector constructor 0x821D30 (0x46AE30 por entrada) y 16 bytes a cero; devuelve this
//    2   0x66D4B0   CPatrolRoute::Assign                                                      thiscall(origen) `ret 4`: copia el contador, los vectores (+0x144) y los dos nombres C de cada entrada
//    3   0x674930   CTaskComplexFollowPatrolRoute constructor (vtable 0x870114, marco SEH)    thiscall(a, ruta, b, c, d) `ret 14h`: base 0x61A3B0, campos, new 0x41B810 (0x1A4), ctor 0x66D440 y copia 0x66D4B0
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
// Familia = n % 16: tipo = familia % 4 (posicion de `this`: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F175AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 4;

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
    std::array<u32, 48> a{};
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

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}

// Clases de funcion (forma)
enum Kind : u32 { kQuad, kPrCtor, kPrAssign, kFpr };
inline constexpr std::array<Kind, kFunctions> kKind{kQuad, kPrCtor, kPrAssign, kFpr};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x552520, 0x66D440, 0x66D4B0, 0x674930};
// Bytes que cada funcion toca a partir de su puntero `this` (kSpan[0] = 0x24: el cuerpo del destructor solo lee +0x10..+0x23; el nodo entero ocupa 0x28)
inline constexpr std::array<u32, kFunctions> kSpan{0x24, 0x138, 0x200, 0x30};
inline constexpr u32 kSrcLen = 0x200;                        // bytes de la ruta origen de Assign que el banco prepara (hasta 12 entradas)
inline constexpr u32 kQuadSlot = 0x50;                       // separacion de los nodos de los hijos en las arenas B y C
inline constexpr u32 kQuadBase = 0x40;                       // desplazamiento del primer nodo de hijo
inline constexpr u32 kPoolGlobal = 0xB745BC;                 // CQuadTreeNode::ms_pQuadTreeNodePool (global del juego; el falso del 0x552BC0 anota su valor)

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global
//   kQuad: a[0] = raiz (en A; el nodo cabe entero en la ventana: nunca invade la guarda); a[3] = dword que hay en la cabeza de la lista (+0x10); a[4..7] = hijos de la raiz (0 nulo; 1..8 nodo B de la ranura e-1; 9..16 nodo C de la
//          ranura e-9); a[8] = desplazamiento de la rejilla de nodos (0..3); a[16 + 4*s + j] = hijo j del nodo B de la ranura s (0 nulo o 9..16 = nodo C); los nodos C no tienen hijos. Ranura k: desplazamiento kQuadBase + a[8] + 0x50*k.
//   kPrCtor: a[3] = dword previo en el contador (+0), no se lee
//   kPrAssign: a[3] = contador de la ruta origen (dword; 0, negativos, 1..8 y 9..12); a[4] = semilla de la imagen de la ruta origen; a[5] = 1 si el primer nombre de cada entrada puede medir hasta 0x2F (si no, hasta 0x17);
//          a[17] = arena de la ruta origen (0 A, 1 B, 2 C) y a[18] = su desplazamiento (si es la misma ruta que el destino, la misma arena y desplazamiento); a[19] = modo (informativo: 0 en B, 1 en C, 2 la misma ruta, 3 en A antes del destino)
//   kFpr: a[3] = dword previo en +0; a[4] = dword previo en +0x20 (banderas); a[5] = a (argumento 1); a[6] = modo de la ruta origen (0 en B a[7], 1 en C a[8], 2 cruda a[9], 3 nula); a[10] = b; a[11] = c; a[16] = d; a[17] = 0 la reserva falla / 1 devuelve
//          un nodo en B (a[18]); a[19] = modo del EAX del constructor falso (0 el nodo, 1 un puntero a C a[20], 2 nulo, 3 crudo a[21]); a[22], a[23], a[24] = dwords que deja el constructor base falso en +0, +4 y +8; a[25] = dword que deja
//          el constructor falso en el nodo
inline u32 TreeOffset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWin);
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4) + 1 + g.R(3);
    case 2: return win - span - g.R(4);
    default: return g.P(50) ? g.R(0x20) : win - span - 4 * g.R(8);
    }
}

inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    switch (kKind[c.fn]) {
    case kQuad: {
        c.a[0] = TreeOffset(g, kind, kSpan[c.fn]);
        c.a[3] = Raw(g);
        const u32 shape = g.R(12);   // 0 sin hijos; 1 un hijo; 2..4 varios; resto aleatorio con muchos hijos
        const auto child = [&](bool allowB) -> u32 {
            if (!g.P(shape == 0 ? 0 : (shape == 1 ? 25 : (shape < 5 ? 55 : 85)))) return 0u;
            return allowB && g.P(55) ? 1u + g.R(8) : 9u + g.R(8);
        };
        for (unsigned j = 0; j < 4; ++j) c.a[4 + j] = child(true);
        if (shape == 1) { c.a[4 + g.R(4)] = 1u + g.R(8); }
        if (g.P(8) && c.a[4] != 0) c.a[5] = c.a[4];   // el mismo nodo como dos hijos
        c.a[8] = g.R(4);
        const bool deep = shape >= 5 || g.P(35);
        for (unsigned s = 0; s < 8; ++s)
            for (unsigned j = 0; j < 4; ++j) c.a[16 + 4 * s + j] = deep && g.P(60) ? 9u + g.R(8) : 0u;
        break;
    }
    case kPrCtor:
        c.a[0] = Offset(g, kind, kSpan[c.fn]);
        c.a[3] = Raw(g);
        break;
    case kPrAssign: {
        c.a[0] = Offset(g, kind, kSpan[c.fn]);
        const u32 m = g.R(100);
        c.a[3] = m < 6 ? 0u : (m < 12 ? g.Pk({0xFFFFFFFFu, 0x80000000u, 0xFFFFFF00u, 0xFFFFFFF8u, 0x80000001u}) : (m < 80 ? 1u + g.R(8) : (m < 92 ? 9u + g.R(4) : 8u)));
        c.a[4] = g.W();
        c.a[5] = g.P(35) ? 1u : 0u;
        u32 mode = g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 3u, 3u});
        if (mode == 3 && c.a[0] < kSrcLen) mode = 0;
        c.a[19] = mode;
        switch (mode) {
        case 1: c.a[17] = 2; c.a[18] = Offset(g, g.R(4), kSrcLen); break;
        case 2: c.a[17] = 0; c.a[18] = c.a[0]; break;
        case 3: c.a[17] = 0; c.a[18] = g.R(c.a[0] - kSrcLen + 1); break;
        default: c.a[17] = 1; c.a[18] = Offset(g, g.R(4), kSrcLen); break;
        }
        break;
    }
    case kFpr:
        c.a[0] = Offset(g, kind, kSpan[c.fn]);
        c.a[3] = Raw(g);
        c.a[4] = g.Pk({0u, 0xFFFFFFFFu, 1u, 2u, 3u, 0xFFFFFFFDu, 0xFFFFFFFEu, g.W(), g.W(), g.W(), 0x80000000u});
        c.a[5] = g.Pk({0u, 1u, 0xFFFFu, 0x10000u, 0xFFFF8000u, 0x7FFFu, 0x12345678u, 0xDEADBEEFu, g.W(), g.W(), g.R(8), 0xFFFFFFFFu});
        c.a[6] = g.Pk({0u, 0u, 1u, 2u, 3u});
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[8] = Offset(g, g.R(4), 4);
        c.a[9] = Raw(g);
        c.a[10] = g.Pk({0u, 1u, 0xFFFFu, 0x10000u, 0xFFFF8000u, 0x7FFFu, 0x87654321u, g.W(), g.W(), 0xFFFFFFFFu, g.R(8)});
        c.a[11] = g.Pk({0u, 0x3F000000u, 0x40A00000u, 0x7FC00000u, 0xFF800000u, 0x80000000u, g.W(), g.W(), g.W(), 0xFFFFFFFFu, 1u});
        c.a[16] = g.Pk({0u, 0x3F000000u, 0x40A00000u, 0x7FA00000u, 0x7F800000u, 0x80000000u, g.W(), g.W(), g.W(), 0xFFFFFFFFu, 2u});
        c.a[17] = g.P(85) ? 1u : 0u;
        c.a[18] = Offset(g, g.R(4), 0x10);
        c.a[19] = g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 3u});
        c.a[20] = Offset(g, g.R(4), 4);
        c.a[21] = Raw(g);
        c.a[22] = g.Pk({0u, 0x86D48Cu, 0x86D4CCu, 0xFFFFFFFFu, g.W(), g.W()});
        c.a[23] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W()});
        c.a[24] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W()});
        c.a[25] = g.Pk({0u, 0x86D4CCu, 0xFFFFFFFFu, g.W(), g.W()});
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

#pragma once
// Fixtures del banco Fn131A (lote 131A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado se atribuian por cercania: las reales salen del desensamblado, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre real                                   forma
//   0   0x5DE920   CAEPedAudioEntity::Destructor                  thiscall() `ret`, marco SEH: vtable 0x86C2A8, destructor de m_sTwinLoopSoundEntity (+0xA8, 0x4F2B80) en linea, vtable base 0x859988, ~CAESound sobre m_tempSound
//   1   0x563130   CSector::Destructor                            thiscall() `ret`, marco SEH: Flush de la lista doble (+4, 0x552470) y de la simple (+0, 0x552400)
//   2   0x552400   CPtrNodeSingleLinkPool::Flush (plantilla)      thiscall() `ret`: vacia una lista simple de punteros liberando los nodos en el pool simple (0xB74484)
//   3   0x552470   CPtrNodeDoubleLinkPool::Flush (plantilla)      thiscall() `ret`: vacia una lista doble de punteros liberando los nodos en el pool doble (0xB74488)
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40) y, donde hace falta, memoria global del juego (los dos punteros de pool 0xB74484 / 0xB74488 apuntan a pools
// falsos de la arena C; se guardan y se restauran tras cada lado).
// Familia = n % 16: tipo = familia % 4 (ver cada funcion) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F131AFix {
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

constexpr u32 kSpanPed = 0x15C;        // CAEPedAudioEntity completo
constexpr u32 kSpanList = 8;           // cabeza simple (+0) y cabeza doble (+4) de CSector

// Reparto de la arena C en las funciones de listas: estados del pool simple [0x40, 0x6C0), estados del pool doble [0x740, 0xDC0), pools falsos (0x10 bytes) en 0xE00 y 0xE40 y blancos de los
// punteros "anterior" de los nodos dobles (escrituras de [prev + 4]) en [0xE80, 0xF70)
constexpr u32 kStatesS = 0x40, kStatesSEnd = 0x6C0, kStatesD = 0x740, kStatesDEnd = 0xDC0, kPoolS = 0xE00, kPoolD = 0xE40, kScratch = 0xE80, kScratchEnd = 0xF70;

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
    std::array<u32, 16> a{};
    std::vector<u8> s;          // datos en bytes (desplazamientos de nodos y especificaciones de "anterior")
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

// Especificacion de un puntero de sonido (CAEPedAudioEntity): bits 28..31 = tipo (0 nulo, 1 arena B, 2 arena C, 3 arena A, 4 crudo fuera de las arenas), el resto el desplazamiento o el valor
inline u32 PtrSpec(Gen& g, u32 selfOff) {
    switch (g.R(10)) {
    case 0: case 1: case 2: return 0u;
    case 3: case 4: return 0x10000000u | (4 * g.R(0x3F0));
    case 5: case 6: return 0x20000000u | (4 * g.R(0x3F0));
    case 7: return 0x30000000u | (4 * g.R(0x3F0));
    case 8: return 0x30000000u | (selfOff + 4 * g.R(kSpanPed / 4));   // apunta dentro del propio objeto
    default: return 0x40000000u | (0x00400000u + 4 * g.R(0x100000));
    }
}

// Posiciones de nodos (desplazamiento del primer byte) distintas entre si (separacion minima de 16) y fuera de [avoidLo, avoidHi) (celda de cabeza en la misma arena)
inline std::vector<int> NodeOffsets(Gen& g, u32 kind, u32 count, int avoidLo, int avoidHi) {
    const int win = int(kWin);
    std::vector<int> out;
    for (u32 i = 0; i < count; ++i) {
        for (int attempt = 0; attempt < 40; ++attempt) {
            int p;
            switch (kind) {
            case 0: p = int(4 * g.R((win - 16) / 4)); break;
            case 1: p = int(4 * g.R((win - 16) / 4)) + 1 + int(g.R(3)); break;
            case 2: p = g.P(40) ? (g.P(50) ? win - 12 + int(g.R(0x20)) : -int(g.R(0x28))) : int(4 * g.R((win - 16) / 4)); break;
            default: p = g.P(50) ? int(4 * g.R(8)) : win - 16 - int(4 * g.R(8)); break;
            }
            bool ok = p + 16 <= avoidLo || p >= avoidHi;
            for (int q : out) ok = ok && (p + 16 <= q || q + 16 <= p);
            if (ok) { out.push_back(p); break; }
        }
    }
    return out;
}

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento principal (objeto o celda de cabeza en la arena A), a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global
//   Funcion 0: a[3] = WORD +0x88 de la entidad gemela, a[4], a[5] = especificaciones de los punteros +0xA0 y +0xA4 (PtrSpec), a[6] = EAX que devuelven los falsos.
//   Funciones 1..3: a[3] = nodos simples (en A), a[4] = nodos dobles (en B), a[5] = delta K del almacenamiento del pool simple (almacenamiento = A - K), a[6] = idem doble (B - K), a[7] = desplazamiento de
//     los estados simples en C, a[8] = idem dobles, a[9], a[10] = +0xC inicial de los pools simple y doble, a[11] = modo de los punteros "anterior" de los nodos dobles (0 consistentes, 1 cabeza con
//     anterior, 2 y 3 aleatorios); s = desplazamientos int16 de los nodos simples, de los dobles y especificaciones u16 de "anterior" de los dobles (0 nulo, 1 consistente, >= 2 desplazamiento en C del blanco).
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    if (c.fn == 0) {
        c.a[0] = Offset(g, kind, kSpanPed);
        switch (g.R(8)) {
        case 0: case 1: c.a[3] = 0u; break;
        case 2: c.a[3] = 1u; break;
        case 3: c.a[3] = g.Pk({0x100u, 0x8000u, 0xFF00u, 0x0200u, 0x4000u}); break;     // palabra distinta de cero con el byte bajo a cero
        case 4: c.a[3] = 0xFFFFu; break;
        default: c.a[3] = 1u + g.R(0xFFFF); break;
        }
        c.a[4] = PtrSpec(g, c.a[0]);
        c.a[5] = g.P(15) ? c.a[4] : PtrSpec(g, c.a[0]);
        c.a[6] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), 0x00C0FFEEu});
        return;
    }
    const bool wantS = c.fn != 3, wantD = c.fn != 2;
    const u32 headSpan = c.fn == 1 ? kSpanList : 4u;
    c.a[0] = Offset(g, kind, headSpan);
    const auto count = [&](bool want) { const u32 v = g.Pk({0u, 1u, 1u, 2u, 2u, 3u, 4u, 5u, 6u, 8u, 12u, 20u, 40u}); return want ? v : 0u; };
    const u32 ns0 = count(wantS), nd0 = count(wantD);
    const auto sOffs = NodeOffsets(g, kind, ns0, int(c.a[0]), int(c.a[0] + headSpan));   // los nodos simples comparten la arena A con la cabeza
    const auto dOffs = NodeOffsets(g, kind, nd0, -0x1000, -0x1000);
    const u32 ns = u32(sOffs.size()), nd = u32(dOffs.size());
    c.a[3] = ns; c.a[4] = nd;
    const auto delta = [&]() { return u32(g.P(25) ? int(g.Pk({0u, 1u, 7u, 8u, 12u, 0x40u, 0xFFFFFFFFu, 0xFFFFFFF9u, 0xFFFFFFF8u, 0xFFFFFFF4u, 5u, 0xFFFFFFFBu, 0x100u, 0xFFFFFF00u})) : int(g.R(0x601)) - 0x300); };
    c.a[5] = delta(); c.a[6] = delta();
    // indices de estados resultantes: extremos para colocar el vector de estados en C
    const auto span = [&](const std::vector<int>& offs, int k, bool single, int& lo, int& hi) {
        lo = 1 << 30; hi = -(1 << 30);
        for (int p : offs) {
            const int diff = p + k;
            const int idx = single ? (diff >> 3) : diff / 12;
            lo = std::min(lo, idx); hi = std::max(hi, idx);
        }
        if (offs.empty()) lo = hi = 0;
    };
    int loS, hiS, loD, hiD;
    span(sOffs, int(c.a[5]), true, loS, hiS);
    span(dOffs, int(c.a[6]), false, loD, hiD);
    const auto place = [&](int lo, int hi, u32 zoneLo, u32 zoneHi) {
        const int minOff = int(zoneLo) - lo, maxOff = int(zoneHi) - 1 - hi;
        return u32(minOff + int(g.R(u32(maxOff - minOff + 1))));
    };
    c.a[7] = place(loS, hiS, kStatesS, kStatesSEnd);
    c.a[8] = place(loD, hiD, kStatesD, kStatesDEnd);
    const auto firstFree = [&](int lo, int hi) {
        switch (g.R(9)) {
        case 0: return 0x7FFFFFFFu;
        case 1: return 0x80000000u;
        case 2: return 0xFFFFFFFFu;
        case 3: return u32(hi + 1);
        case 4: return u32(lo - 1);
        case 5: return u32(lo + int(g.R(u32(hi - lo + 1))));
        case 6: return 0u;
        case 7: return u32(lo);
        default: return g.W();
        }
    };
    c.a[9] = firstFree(loS, hiS); c.a[10] = firstFree(loD, hiD);
    c.a[11] = g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 3u});
    std::vector<u8> s;
    const auto put16 = [&](u32 v) { s.push_back(u8(v)); s.push_back(u8(v >> 8)); };
    for (int p : sOffs) put16(u32(p) & 0xFFFFu);
    for (int p : dOffs) put16(u32(p) & 0xFFFFu);
    for (u32 i = 0; i < nd; ++i) {
        u32 spec = 1;   // consistente
        const u32 scratch = kScratch + 4 * g.R((kScratchEnd - kScratch) / 4) + (g.P(30) ? 1 + g.R(3) : 0);
        switch (c.a[11]) {
        case 0: spec = 1; break;
        case 1: spec = i == 0 ? scratch : 1; break;
        case 2: spec = g.Pk({0u, 1u, scratch, scratch}); break;
        default: spec = i == 0 ? 0u : g.Pk({0u, 1u, scratch}); break;
        }
        put16(spec);
    }
    c.s = s;
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

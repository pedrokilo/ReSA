#pragma once
// Fixtures del banco Fn93A (lote 93A): 16384 casos por funcion (16384 pares por funcion). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarPathLink::OneWayLaneOffset                0x44DB00  thiscall (ECX = enlace de 14 bytes), sin argumentos, `ret`; float en ST0       (hook S)
//   1 CPathFind::RemoveBadStartNode                 0x44E4F0  thiscall (ECX = this; posicion, nodos y contador en pila), `ret 14h`; void     (hook S)
//   2 CPathFind::StoreInteriorNodeLink              0x450FB0  cdecl (area A, nodo A, area B, nodo B, ranura, &contador), sin this; void      (hook S)
//   3 CPathFind::CalcDistToAnyConnectingLinks       0x44F190  thiscall (ECX = this; nodo y posicion en pila), `ret 10h`; float en ST0        (hook S)
//   4 CPathFind::FindNodeClosestInRegion            0x44F2C0  thiscall (ECX = this; 10 argumentos en pila), `ret 2Ch`; void                  (hook S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Familia = n % 16 (1024 casos por familia).
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

namespace F93AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 5;
constexpr unsigned kBanks = 3;          // bancos de nodos (arenas de 12 nodos de 0x1C bytes)
constexpr unsigned kBankNodes = 12;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kLinkWin = 0x10;                 // F0: enlace de 14 bytes
constexpr std::size_t kBankWin = 0x1C * kBankNodes;    // banco de nodos
constexpr std::size_t kThis1Win = 0x940;               // F1: CPathFind hasta +0x924 (m_pPathNodes)
constexpr std::size_t kNodesWin = 0x40;                // F1: 16 CNodeAddress
constexpr std::size_t kCountWin = 0x10;                // F1: contador (dword)
constexpr std::size_t kLinks2Win = 0xA0;               // F2: 40 CNodeAddress
constexpr std::size_t kBytes2Win = 0x30;               // F2: 40 bytes (longitudes / intersecciones)
constexpr std::size_t kCtrWin = 0x10;                  // F2: contador de enlaces
constexpr std::size_t kThis3Win = 0xB70;               // F3: CPathFind hasta +0xB64
constexpr std::size_t kNodeWin = 0x40;                 // F3: nodo consultado
constexpr std::size_t kLinkArrWin = 0xA0;              // F3: m_pNodeLinks del area del nodo (40 CNodeAddress)
constexpr std::size_t kThis4Win = 0x1200;              // F4: CPathFind hasta +0x11E4
constexpr std::size_t kOutWin = 0x10;                  // F4: CNodeAddress de salida y distancia
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 93001u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline u32 B(float f) { return std::bit_cast<u32>(f); }
inline float F(u32 b) { return std::bit_cast<float>(b); }

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
    // F0: bytes +0xA (ancho) y +0xB (carriles) del enlace
    u32 width{}, lanes{};
    // posiciones int16 crudas de los bancos de nodos (x, y, z por nodo; se escriben en +8, +0xA, +0xC), compartidas por F1..F4
    i32 np[kBanks][kBankNodes][3]{};
    // F1: contador (dword; cuenta la palabra baja), posicion, direcciones de la ruta (area, nodo) de hasta 16 entradas y carga de las areas de los dos primeros nodos
    u32 cnt{};
    u32 pos[3]{};
    u32 areaOf[16]{}, idOf[16]{};
    u32 loaded0{}, loaded1{}, bank0{}, bank1{};
    // F2: argumentos (area A, nodo A, area B, nodo B, ranura), contador, bancos de las tres areas, byte inicial de banderas del nodo A en el area de la ranura
    u32 a1{}, a2{}, a3{}, a4{}, a5{}, ctr{}, bankA{}, bankB{}, bankF{}, flagInit{};
    // F3: numero de enlaces (byte +0x18 completo), base de enlaces, area del nodo, enlaces (area, nodo), resultados del falso DistToLineSqr
    u32 nflags{}, base{}, narea{};
    u32 lkArea[16]{}, lkNode[16]{};
    u32 loadedArea[2]{}, areaBank[2]{};         // F3/F4: las dos areas del banco (cargadas o no) y el banco de cada una
    u32 dist[16]{};
    // F4: area, tipo, banderas (dwords con basura), contadores, distancia de salida, banderas de los nodos y resultados del falso CalcDist
    u32 area4{}, ntype{}, flLow{}, flUnk{}, flBoat{}, flUnused{}, numNodes{}, numVeh{}, outDist{}, outInit{};
    u32 fl18[kBankNodes]{}, fl19[kBankNodes]{};
};

// ------------------------------------------------------------------------------------------------ generadores
struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 Pk(std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); }
    float Fl(float lo, float hi) { return lo + (hi - lo) * (float(R(100001)) / 100000.0f); }
    u32 W() { return Rand(n, nk()); }
    i32 Sr(i32 lo, i32 hi) { return lo + i32(R(u32(hi - lo + 1))); }
};

inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
// Un float "especial": cero, desnormal, infinito, NaN (un unico patron), enorme
inline u32 Special(Gen& g) {
    const u32 r = g.R(32);
    if (r < 10) return kTiny[g.R(12)];
    if (r < 16) return kInf[g.R(4)];
    if (r < 24) return 0xFFC00000u;
    if (r < 28) return kBig[g.R(6)];
    return B(float(int(g.R(2001)) - 1000) / 7.0f);
}
inline u32 NextUp(u32 bits, int steps) {   // desplaza un float normal `steps` unidades de ultimo lugar (signo respetado)
    const bool neg = (bits >> 31) != 0;
    u32 mag = bits & 0x7FFFFFFFu;
    mag = u32(i32(mag) + (neg ? -steps : steps));
    return (bits & 0x80000000u) | (mag & 0x7FFFFFFFu);
}
inline i32 Clamp16(i32 v) { return v < -32768 ? -32768 : (v > 32767 ? 32767 : v); }
inline float Raw(i32 r) { return float(r) * 0.125f; }   // posicion int16 / 8 (exacta)

// Rellena las posiciones de los bancos: mismas coordenadas en una ventana pequena o grandes
inline void FillNodes(C& c, Gen& g, bool wide) {
    const i32 span = wide ? 32767 : 400;
    for (auto& bank : c.np)
        for (auto& node : bank)
            for (auto& v : node) v = g.Sr(-span, span);
}

// ------------------------------------------------------------------------------------------------ funcion 0: CCarPathLink::OneWayLaneOffset
inline void MakeFn0(C& c, unsigned n) {
    Gen g(n);
    const u32 f = c.family;
    u32 opp = g.R(8), same = g.R(8), hi = g.R(4) << 6, width = g.R(256);
    switch (f) {
    case 0: opp = g.R(8); same = g.R(8); break;
    case 1: opp = 0; same = g.R(8); break;
    case 2: opp = 0; same = 0; break;
    case 3: opp = 1 + g.R(7); same = 0; break;
    case 4: opp = 1 + g.R(7); same = 1 + g.R(7); break;
    case 5: opp = 1 + g.R(7); same = 1 + g.R(7); width = 0; break;
    case 6: opp = 1 + g.R(7); same = 1 + g.R(7); width = 255; break;
    case 7: opp = 1 + g.R(7); same = 1 + g.R(7); width = g.Pk({1u, 2u, 127u, 128u, 129u, 254u, 16u, 100u}); break;
    case 8: opp = 1 + (n / 16) % 7; same = 1 + (n / 112) % 7; width = (n / 784) & 255u; break;
    case 9: hi = 0xC0; break;
    case 10: opp = 1 + g.R(7); same = 1 + g.R(7); width = g.R(256); break;
    case 11: opp = 0; same = 7; break;
    case 12: opp = 7; same = 0; break;
    case 13: opp = 7; same = 7; hi = 0xC0; break;
    case 14: opp = 0; same = 1 + g.R(7); width = g.R(256); break;
    case 15: break;
    default: break;
    }
    c.lanes = opp | (same << 3) | hi;
    c.width = width;
}

// ------------------------------------------------------------------------------------------------ funcion 1: CPathFind::RemoveBadStartNode
inline double Dot1(const C& c) {   // (y0 - py) * (y1 - py) + (x1 - px) * (x0 - px) en double (solo para elegir casos)
    const double x0 = Raw(c.np[c.bank0][c.idOf[0] & 7][0]), y0 = Raw(c.np[c.bank0][c.idOf[0] & 7][1]);
    const double x1 = Raw(c.np[c.bank1][c.idOf[1] & 7][0]), y1 = Raw(c.np[c.bank1][c.idOf[1] & 7][1]);
    const double px = F(c.pos[0]), py = F(c.pos[1]);
    return (y0 - py) * (y1 - py) + (x1 - px) * (x0 - px);
}
inline void MakeFn1(C& c, unsigned n) {
    Gen g(n);
    const u32 f = c.family;
    static constexpr std::array<u32, 8> kAreas{0u, 5u, 17u, 40u, 63u, 64u, 71u, 1u};
    FillNodes(c, g, f == 0 && g.P(30));
    c.cnt = 2 + g.R(14);
    for (u32 k = 0; k < 16; ++k) { c.areaOf[k] = kAreas[g.R(8)]; c.idOf[k] = g.R(8); }
    c.loaded0 = c.loaded1 = 1;
    c.bank0 = g.R(kBanks); c.bank1 = g.R(kBanks);
    for (u32 k = 0; k < 3; ++k) c.pos[k] = B(g.Fl(-60.0f, 60.0f));
    // la ruta empieza con las dos areas pedidas; si son la misma area comparten banco y estado de carga
    auto share = [&] { if (c.areaOf[0] == c.areaOf[1]) { c.bank1 = c.bank0; c.loaded1 = c.loaded0; } };
    // elige pos para que el producto tenga el signo pedido (-1, 0 o +1); hasta 60 intentos
    auto aim = [&](int sign) {
        for (int t = 0; t < 60; ++t) {
            for (u32 k = 0; k < 3; ++k) c.pos[k] = B(g.Fl(-60.0f, 60.0f));
            const double d = Dot1(c);
            if (sign < 0 ? d < -1.0 : (sign > 0 ? d > 1.0 : false)) return;
        }
    };
    auto zeroDot = [&] {   // pos sobre el primer nodo en x e y: los dos terminos valen cero
        const u32 variant = g.R(4);
        const float x0 = Raw(c.np[c.bank0][c.idOf[0] & 7][0]), y0 = Raw(c.np[c.bank0][c.idOf[0] & 7][1]);
        const float x1 = Raw(c.np[c.bank1][c.idOf[1] & 7][0]), y1 = Raw(c.np[c.bank1][c.idOf[1] & 7][1]);
        if (variant == 0) { c.pos[0] = B(x0); c.pos[1] = B(y0); }
        else if (variant == 1) { c.pos[0] = B(x1); c.pos[1] = B(y1); }
        else if (variant == 2) { c.pos[0] = B(x0); c.pos[1] = B(y1); }
        else { c.pos[0] = B(x1); c.pos[1] = B(y0); }
    };
    switch (f) {
    case 0:
        c.cnt = g.Pk({2u, 3u, 5u, 15u, g.R(16)});
        break;
    case 1:   // contador menor que 2 (palabra baja con signo); la palabra alta es basura
        c.cnt = (g.Pk({0u, 1u, 0xFFFFu, 0x8000u, 0xFFFEu, 0xFFFBu, 0xFFFFu - g.R(30)}) & 0xFFFFu) | (g.P(50) ? (g.W() & 0xFFFF0000u) : 0u);
        aim(-1);
        break;
    case 2: c.loaded0 = 0; share(); aim(-1); break;
    case 3: c.loaded1 = 0; if (c.areaOf[0] == c.areaOf[1]) c.loaded0 = 0; aim(-1); break;
    case 4: share(); aim(-1); break;
    case 5: share(); aim(+1); break;
    case 6: share(); zeroDot(); break;
    case 7: {   // producto casi nulo, con los dos signos: pos en el primer nodo desplazado unos ulps
        share(); zeroDot();
        const int steps = int(g.R(9)) - 4;
        const u32 axis = g.R(2);
        c.pos[axis] = NextUp(c.pos[axis] == 0 ? 0x3F800000u : c.pos[axis], steps == 0 ? 1 : steps);
        if (g.P(30)) c.pos[axis] = B(F(c.pos[axis]) + (g.P(50) ? 1e-3f : -1e-3f));
        break;
    }
    case 8:
        share();
        for (u32 k = 0; k < 3; ++k) if (g.P(45)) c.pos[k] = Special(g);
        break;
    case 9: {   // posiciones de nodo cero y pos denormal: los productos desbordan hacia cero por debajo
        share();
        for (u32 k = 0; k < 3; ++k) c.pos[k] = g.Pk({1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00000100u, 0x80000100u, 0x00400000u, 0x80400000u});
        const bool zeroNodes = g.P(80);
        if (zeroNodes) { for (u32 b = 0; b < kBanks; ++b) for (u32 i = 0; i < kBankNodes; ++i) { c.np[b][i][0] = c.np[b][i][1] = 0; } }
        else { c.np[c.bank0][c.idOf[0] & 7][0] = 0; c.np[c.bank1][c.idOf[1] & 7][1] = 0; }
        break;
    }
    case 10: c.cnt = 2; share(); aim(-1); break;
    case 11: c.cnt = 15; share(); aim(-1); break;
    case 12:   // el mismo nodo dos veces: el producto es una suma de cuadrados (nunca negativa)
        c.areaOf[1] = c.areaOf[0]; c.idOf[1] = c.idOf[0]; share();
        if (g.P(30)) { c.areaOf[1] = c.areaOf[0]; c.idOf[1] = (c.idOf[0] + 1) & 7; }
        break;
    case 13:   // misma area con bancos y cargas compartidos; la ruta completa de la misma area
        for (u32 k = 0; k < 16; ++k) c.areaOf[k] = c.areaOf[0];
        share(); aim(g.P(70) ? -1 : +1);
        break;
    case 14:   // contador con basura arriba (solo cuenta la palabra baja) y retorno
        c.cnt = (2 + g.R(14)) | (g.W() & 0xFFFF0000u);
        share(); aim(-1);
        break;
    case 15: share(); aim(g.P(80) ? -1 : +1); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 2: CPathFind::StoreInteriorNodeLink
inline void MakeFn2(C& c, unsigned n) {
    Gen g(n);
    const u32 f = c.family;
    const bool wide = f == 7 || (f == 0 && g.P(30)) || f == 6;
    FillNodes(c, g, wide);
    c.a5 = g.R(8);
    const u32 idx5 = 0x40 + c.a5;
    c.a1 = g.Pk({idx5, idx5, 0x40u + g.R(8), g.R(64)});
    c.a3 = g.Pk({idx5, c.a1, 0x40u + g.R(8), g.R(64)});
    c.a2 = g.R(8);
    c.a4 = g.R(8);
    c.ctr = g.R(32);
    c.bankA = g.R(kBanks); c.bankB = g.R(kBanks); c.bankF = g.R(kBanks);
    c.flagInit = g.W() & 0xFFu;
    // posicion del nodo A (area a1, indice con signo a2) y del nodo B (area a3, a4): las ponemos a distancia elegida
    const auto setDelta = [&](i32 rx, i32 ry, i32 rz) {
        // el nodo A real es (banco de a1, int8(a2)); el nodo B es (banco de a3, a4). int8 negativo cae en la guarda: se omite ahi
        const i32 ia = i32(std::int8_t(c.a2 & 0xFF));
        if (ia < 0) return;
        const i32 ax = c.np[c.bankA][ia][0], ay = c.np[c.bankA][ia][1], az = c.np[c.bankA][ia][2];
        auto& b = c.np[c.bankB][c.a4];
        b[0] = Clamp16(ax + rx); b[1] = Clamp16(ay + ry); b[2] = Clamp16(az + rz);
    };
    switch (f) {
    case 0: break;
    case 1: setDelta(0, 0, 0); break;                                                    // d = 0
    case 2: setDelta(g.Sr(-7, 7), g.Sr(-7, 7), g.Sr(-7, 7)); break;                      // d < 1
    case 3: setDelta(g.Sr(-600, 600), g.Sr(-600, 600), g.Sr(-600, 600)); break;
    case 4: {   // distancia entera exacta (ternas pitagoricas en unidades de 8 crudas = 1 m)
        static constexpr i32 t[6][3] = {{3, 4, 0}, {5, 12, 0}, {8, 15, 0}, {2, 3, 6}, {1, 4, 8}, {6, 17, 6}};
        const auto& tr = t[g.R(6)];
        const i32 m = 1 + g.R(12);
        setDelta(tr[0] * 8 * m * (g.P(50) ? 1 : -1), tr[1] * 8 * m * (g.P(50) ? 1 : -1), tr[2] * 8 * m * (g.P(50) ? 1 : -1));
        break;
    }
    case 5: {   // en torno de enteros y de 1.0: un paso crudo mas o menos
        const i32 k = g.Pk({1u, 2u, 3u, 7u, 16u, 100u});
        setDelta(8 * k + g.Sr(-2, 2), 0, 0);
        if (g.P(40)) setDelta(0, 8 * k + g.Sr(-2, 2), g.Sr(0, 1));
        break;
    }
    case 6: {   // en torno de 255
        const i32 e = g.Sr(-4, 4);
        setDelta(2040 + e, 0, 0);
        if (g.P(30)) setDelta(g.Sr(0, 1) * 3, 2040 + e, 0);
        if (g.P(20)) setDelta(1200 + g.Sr(-5, 5), 1500 + g.Sr(-5, 5), 1100 + g.Sr(-5, 5));
        break;
    }
    case 7: setDelta(g.Sr(-30000, 30000), g.Sr(-30000, 30000), g.Sr(-30000, 30000)); break;
    case 8: c.flagInit = (c.flagInit & 0xF0u) | g.Pk({15u, 0u, 14u, 1u, 7u}); c.ctr = g.R(40); setDelta(g.Sr(-3000, 3000), g.Sr(-3000, 3000), g.Sr(-300, 300)); break;
    case 9:   // indice de nodo A con basura sobre el byte bajo (movsx byte) y valores negativos
        c.a2 = g.Pk({0xFFFFFFFFu, 0xFEu, 0x100u + g.R(8), 0xFFFFFF00u + g.R(8), 0x7FFFFF00u + g.R(8), 0x80000000u + g.R(8), g.R(8), 0xFFFFFFFEu});
        c.a2 &= ~0u;
        setDelta(g.Sr(-2000, 2000), g.Sr(-2000, 2000), g.Sr(-200, 200));
        break;
    case 10: c.a3 = c.a1; c.bankB = c.bankA; setDelta(g.Sr(-2000, 2000), g.Sr(-2000, 2000), g.Sr(-200, 200)); break;
    case 11: c.a3 = c.a1; c.bankB = c.bankA; c.a4 = c.a2 & 7; c.a1 = idx5; c.a3 = idx5; c.bankA = c.bankB = c.bankF; break;   // el mismo nodo (d = 0), mismo banco que las banderas
    case 12: c.a1 = idx5; c.bankA = c.bankF; setDelta(g.Sr(-4000, 4000), g.Sr(-4000, 4000), g.Sr(-400, 400)); break;
    case 13: c.ctr = g.Pk({0u, 1u, 31u, 39u, 16u}); setDelta(g.Sr(-4000, 4000), g.Sr(-4000, 4000), g.Sr(-400, 400)); break;
    case 14: setDelta(g.Pk({0u, 4080u, 2040u, 1u}), g.Pk({0u, 1u, 7u, 2040u}), g.Pk({0u, 1u, 3u, 2040u})); break;
    case 15: setDelta(g.Sr(-3000, 3000), g.Sr(-3000, 3000), g.Sr(-300, 300)); break;
    default: break;
    }
    // consistencia de bancos: la misma ranura global de area apunta a un solo banco
    if (c.a3 == c.a1) c.bankB = c.bankA;
    if (idx5 == c.a1) c.bankF = c.bankA;
    else if (idx5 == c.a3) c.bankF = c.bankB;
}

// ------------------------------------------------------------------------------------------------ funcion 3: CPathFind::CalcDistToAnyConnectingLinks
inline void MakeDistList(C& c, Gen& g, u32 mode) {
    for (u32 k = 0; k < 16; ++k) {
        switch (mode) {
        case 0: c.dist[k] = B(g.Fl(0.0f, 5000.0f)); break;
        case 1: c.dist[k] = B(1000.0f * float(16 - k)); break;                             // descendente: cada uno es el nuevo minimo
        case 2: c.dist[k] = B(1.0f + float(k) * 3.0f); break;                              // ascendente: el primero se queda
        case 3: c.dist[k] = Special(g); break;
        case 4: c.dist[k] = g.Pk({B(1000000.0f), B(1000000.0f), B(7.0f), B(7.0f), 0u, 0x80000000u, 0xFFC00000u}); break;   // empates y NaN
        case 5: c.dist[k] = g.Pk({B(1.0e6f), 0x7F800000u, B(2.0e6f), 0x7F7FFFFFu, B(1000001.0f), B(1000000.0f)}); break;      // nunca menores que 1e6 (salvo empate)
        default: c.dist[k] = B(g.Fl(0.0f, 100.0f)); break;
        }
    }
}
inline void MakeFn3(C& c, unsigned n) {
    Gen g(n);
    const u32 f = c.family;
    FillNodes(c, g, g.P(20));
    static constexpr std::array<u32, 8> kAreas{0u, 5u, 17u, 40u, 63u, 64u, 71u, 1u};
    c.narea = kAreas[g.R(8)];
    c.loadedArea[0] = c.loadedArea[1] = 1;
    c.areaBank[0] = g.R(kBanks); c.areaBank[1] = g.R(kBanks);
    c.nflags = (g.W() & 0xF0u) | (1 + g.R(15));
    c.base = u32(g.Sr(0, 12)) & 0xFFFFu;
    for (u32 k = 0; k < 16; ++k) { c.lkArea[k] = g.P(50) ? 0u : 1u; c.lkNode[k] = g.R(8); }   // indice 0/1 = una de las dos areas del banco
    for (u32 k = 0; k < 3; ++k) c.pos[k] = B(g.Fl(-100.0f, 100.0f));
    MakeDistList(c, g, 0);
    switch (f) {
    case 0: break;
    case 1: c.nflags = (c.nflags & 0xF0u); break;                                         // sin enlaces: 1000.0
    case 2: c.loadedArea[0] = c.loadedArea[1] = 0; break;                                  // ningun area destino cargada
    case 3: c.loadedArea[g.R(2)] = 0; break;
    case 4: MakeDistList(c, g, 4); break;
    case 5: MakeDistList(c, g, 1); c.nflags = (c.nflags & 0xF0u) | (4 + g.R(12)); break;
    case 6: MakeDistList(c, g, 2); break;
    case 7: MakeDistList(c, g, 3); break;
    case 8: c.nflags = (c.nflags & 0xF0u) | 15; break;
    case 9: c.base = u32(g.Sr(-4, 0)) & 0xFFFFu; break;                                    // base negativa (movsx)
    case 10: c.nflags = g.W() & 0xFFu; if (!(c.nflags & 0xF)) c.nflags |= 1 + g.R(15); break;   // basura en los bits altos del byte de banderas
    case 11: MakeDistList(c, g, 5); break;
    case 12: for (u32 k = 0; k < 16; ++k) c.dist[k] = B(1000000.0f); break;
    case 13: c.nflags = (c.nflags & 0xF0u) | 1; break;
    case 14: for (u32 k = 0; k < 16; ++k) c.lkArea[k] = k & 1; break;
    case 15: break;
    default: break;
    }
    if (f == 8 || f == 9) { for (u32 k = 0; k < 3; ++k) if (g.P(15)) c.pos[k] = Special(g); }
}

// ------------------------------------------------------------------------------------------------ funcion 4: CPathFind::FindNodeClosestInRegion
inline float FloatScore(const C& c, u32 node) {   // aproximacion para elegir distancias de salida en las fronteras (solo normales)
    const float x = Raw(c.np[c.areaBank[0]][node][0]), y = Raw(c.np[c.areaBank[0]][node][1]), z = Raw(c.np[c.areaBank[0]][node][2]);
    const float s = (std::fabs(z - F(c.pos[2])) * 3.0f + std::fabs(y - F(c.pos[1]))) + std::fabs(x - F(c.pos[0]));
    return s * 0.3f;
}
inline void MakeFn4(C& c, unsigned n) {
    Gen g(n);
    const u32 f = c.family;
    static constexpr std::array<u32, 8> kAreas{0u, 5u, 17u, 40u, 63u, 64u, 71u, 1u};
    FillNodes(c, g, false);
    c.area4 = kAreas[g.R(8)];
    c.areaBank[0] = g.R(kBanks);
    c.loadedArea[0] = 1;
    c.ntype = g.R(2) | (g.P(40) ? (g.W() & 0xFFFFFF00u) : 0u);
    c.flLow = g.P(50) ? 1 : 0; c.flUnk = g.P(50) ? 1 : 0; c.flBoat = g.P(80) ? 0 : 1; c.flUnused = g.W();
    c.numNodes = 1 + g.R(12); c.numVeh = g.R(c.numNodes + 1);
    for (u32 k = 0; k < kBankNodes; ++k) {
        c.fl18[k] = (g.W() & 0x70u) | (g.P(15) ? 0x80u : 0u) | g.R(16);
        c.fl19[k] = g.P(25) ? 2u | (g.W() & 0xFDu) : (g.W() & 0xFDu);
        c.dist[k] = B(g.Fl(0.0f, 60.0f));
    }
    for (u32 k = 0; k < 3; ++k) c.pos[k] = B(g.Fl(-40.0f, 40.0f));
    c.outDist = B(g.P(50) ? 1000000.0f : g.Fl(1.0f, 300.0f));
    c.outInit = g.W();
    const auto pickTypeFlags = [&] { c.flLow = 0; c.flUnk = 0; c.flBoat = 0; for (u32 k = 0; k < kBankNodes; ++k) { c.fl18[k] &= 0x4Fu; c.fl19[k] &= 0xFDu; } };
    switch (f) {
    case 0: break;
    case 1: c.loadedArea[0] = 0; break;
    case 2: c.ntype = g.Pk({0u, 0x100u, 0xABCDEF00u}); pickTypeFlags(); break;
    case 3: c.ntype = g.Pk({1u, 0x101u, 0xABCDEF01u}); pickTypeFlags(); break;
    case 4: c.ntype = g.Pk({2u, 3u, 0x80u, 0xFFu, 0x102u, 0xFFFFFFFFu}); break;
    case 5: c.flLow = g.Pk({1u, 0x100u, 0xFFFFFF00u, 0xFFu, 2u}); c.ntype = g.R(2); break;
    case 6: c.flUnk = g.Pk({1u, 0x100u, 0xFFFFFF00u, 0xFFu, 2u}); c.ntype = g.R(2); break;
    case 7: c.flBoat = g.Pk({0u, 1u, 2u, 0x80u, 0x100u, 0x101u, 0xFFu, 0xFFFFFF01u}); c.ntype = g.R(2); for (u32 k = 0; k < kBankNodes; ++k) if (g.P(50)) c.fl18[k] |= 0x80u; break;
    case 8: c.outDist = g.Pk({0u, 1u, 0x80000000u, 0x3F800000u}); break;                 // nada mejora
    case 9: c.outDist = B(1.0e6f); pickTypeFlags(); c.ntype = g.R(2); break;               // todo pasa la primera comparacion
    case 10: for (u32 k = 0; k < kBankNodes; ++k) c.dist[k] = g.P(50) ? Special(g) : B(g.Fl(0.0f, 5.0f)); pickTypeFlags(); c.ntype = g.R(2); c.outDist = B(g.P(50) ? 1000000.0f : 50.0f); break;
    case 11: {   // fronteras: la distancia de salida igual a una puntuacion (+-ulps)
        pickTypeFlags(); c.ntype = g.R(2); c.numNodes = 1 + g.R(12); c.numVeh = c.ntype == 0 ? c.numNodes : 0;
        const u32 node = c.ntype == 0 ? g.R(c.numVeh) : g.R(c.numNodes);
        c.outDist = NextUp(B(FloatScore(c, node)), int(g.R(5)) - 2);
        for (u32 k = 0; k < kBankNodes; ++k) c.dist[k] = g.Pk({0u, 0x80000000u, B(0.25f), B(1.0f)});
        break;
    }
    case 12: for (u32 k = 0; k < 3; ++k) if (g.P(50)) c.pos[k] = Special(g); pickTypeFlags(); c.ntype = g.R(2); break;
    case 13: c.numVeh = g.Pk({0u, c.numNodes, c.numNodes + 1 > 12 ? 12u : c.numNodes + 1, g.R(13)}); c.numNodes = g.Pk({c.numNodes, 0u, 12u}); if (c.numVeh > 12) c.numVeh = 12; break;
    case 14: {   // distancias de enlace que van bajando: muchos nodos mejoran seguidos
        pickTypeFlags(); c.ntype = 0; c.numNodes = 12; c.numVeh = 12; c.outDist = B(1.0e6f);
        for (u32 k = 0; k < kBankNodes; ++k) c.dist[k] = B(float(12 - k) * 0.5f);
        break;
    }
    case 15: pickTypeFlags(); c.ntype = g.R(2); break;
    default: break;
    }
    if (c.numVeh > c.numNodes && f != 13) c.numVeh = c.numNodes;
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    switch (fn) {
    case 0: MakeFn0(c, n); break;
    case 1: MakeFn1(c, n); break;
    case 2: MakeFn2(c, n); break;
    case 3: MakeFn3(c, n); break;
    default: MakeFn4(c, n); break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    auto V = [](const u32* p, std::size_t k) { return std::vector<u32>(p, p + k); };
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    auto Np = [&](u32 banks, u32 nodes) {
        std::vector<u32> v;
        for (u32 b = 0; b < banks; ++b) for (u32 i = 0; i < nodes; ++i) for (u32 k = 0; k < 3; ++k) v.push_back(u32(c.np[b][i][k]));
        return v;
    };
    switch (c.fn) {
    case 0: j.U("width", c.width).U("lanes", c.lanes); break;
    case 1:
        j.U("cnt", c.cnt).A("pos", V(c.pos, 3)).A("areaof", V(c.areaOf, 16)).A("idof", V(c.idOf, 16)).U("loaded0", c.loaded0).U("loaded1", c.loaded1).U("bank0", c.bank0).U("bank1", c.bank1);
        j.A("np", Np(kBanks, 8));
        break;
    case 2:
        j.U("a1", c.a1).U("a2", c.a2).U("a3", c.a3).U("a4", c.a4).U("a5", c.a5).U("ctr", c.ctr).U("bankA", c.bankA).U("bankB", c.bankB).U("bankF", c.bankF).U("flaginit", c.flagInit);
        j.A("np", Np(kBanks, 8));
        break;
    case 3:
        j.U("nflags", c.nflags).U("base", c.base).U("narea", c.narea).A("lkarea", V(c.lkArea, 16)).A("lknode", V(c.lkNode, 16)).A("loaded", V(c.loadedArea, 2)).A("areabank", V(c.areaBank, 2));
        j.A("dist", V(c.dist, 16)).A("pos", V(c.pos, 3)).A("np", Np(kBanks, 8));
        break;
    default:
        j.U("area", c.area4).U("ntype", c.ntype).U("fllow", c.flLow).U("flunk", c.flUnk).U("flboat", c.flBoat).U("flunused", c.flUnused).U("numnodes", c.numNodes).U("numveh", c.numVeh);
        j.U("outdist", c.outDist).U("outinit", c.outInit).U("loaded", c.loadedArea[0]).U("bank", c.areaBank[0]).A("fl18", V(c.fl18, kBankNodes)).A("fl19", V(c.fl19, kBankNodes));
        j.A("dist", V(c.dist, kBankNodes)).A("pos", V(c.pos, 3)).A("np", Np(kBanks, kBankNodes));
        break;
    }
    return j.End();
}
}

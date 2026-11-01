#pragma once
// Fixtures del banco Car84B (lote 84B): CCarCtrl::PickNextNodeToChaseCar (EXE 0x426EF0), 18432 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::PickNextNodeToChaseCar  0x426EF0  cdecl, cuatro argumentos (vehiculo, x, y, z del destino), `ret` simple, devuelve bool   (hook CCarCtrl / PickNextNodeToChaseCar, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) y las familias 16 y 17 (1024 casos cada una).
// Mundo de caminos de juguete: 3 ranuras de area (id de area distinto en 0..63, cargada o no), 4 nodos por area, 16 enlaces de nodo y de navegacion por area y 8 enlaces de carretera (14 bytes) por area.
// Reglas de seguridad (el EXE no comprueba nada de esto): el area del nodo siguiente esta cargada y el nodo es < 4; los identificadores de enlace de carretera son < 8; ningun nodo tiene mas enlaces de los que
// caben tras su base (base + n <= 16); si la ruta tiene 1 o 2 nodos distintos del siguiente, estan en la tabla de enlaces del nodo siguiente (el EXE los busca sin limite de indices).
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace C84BFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;    // familias 0..15
constexpr unsigned kExtraFamilies = 2;    // familias 16..17, 1024 casos cada una
constexpr unsigned kCases = kBaseCases + kExtraFamilies * 1024;
constexpr unsigned kFamilies = 16 + kExtraFamilies;
constexpr unsigned kFunctions = 1;

constexpr u32 kSlots = 3, kNodesPer = 4, kLinksPer = 16, kNaviPer = 16, kNavNodesPer = 8, kNodeBytes = 0x1C, kLinkBytes = 14;
// Arenas: guarda + ventana + guarda. La del vehiculo mide sizeof(CVehicle) = 0x5A0 (LiveComparison.hpp lo comprueba con static_assert).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x5A0;
constexpr std::size_t kMatWin = 0x80;      // la matriz del vehiculo: solo se lee la posicion (+0x30, +0x34)
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kNodeArena = Arena(kSlots * kNodesPer * kNodeBytes), kNlArena = Arena(kSlots * kLinksPer * 4), kNaviArena = Arena(kSlots * kNaviPer * 2),
                      kNnArena = Arena(kSlots * kNavNodesPer * kLinkBytes), kMatArena = Arena(kMatWin);

constexpr u32 kTimeAddr = 0xB7CB84;       // CTimer::m_snTimeInMilliseconds (la funcion no lo lee: el banco comprueba que no cambia)
constexpr u32 kZoneAddr = 0xC81314;       // zona del clima (word): 0 y 4 activan la rama de persecucion directa
constexpr u32 kForbiddenAddr = 0x8A5F44;  // direccion de nodo invalida del EXE: nodo prohibido de DoPathSearch

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 84002u); }
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

// ------------------------------------------------------------------------------------------------ JSON minimo
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

// ------------------------------------------------------------------------------------------------ valores notables
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Un float "especial": cero, desnormal, infinito, NaN (un unico patron), enorme
inline u32 Special(u32 c, u32 k) {
    const u32 r = Rand(c, k) % 32;
    if (r < 10) return Pick(kTiny, c, k + 1);
    if (r < 16) return Pick(kInf, c, k + 1);
    if (r < 24) return 0xFFC00000u;
    if (r < 28) return Pick(kBig, c, k + 1);
    return B(float(int(Rand(c, k + 3) % 2001) - 1000) / 7.0f);
}

// Valor flotante de la mutacion 15..17 (el oraculo repite la tabla): tabla de especiales o (v % 2001 - 1000) / 8
inline u32 MutFloat(u32 v) {
    static constexpr std::array<u32, 8> t{0u, 0x80000000u, 1u, 0x7F800000u, 0xFF800000u, 0xFFC00000u, 0x7F7FFFFFu, 0x3F800000u};
    return (v & 3u) == 0 ? t[(v >> 2) % 8u] : B(float(int(v % 2001u) - 1000) / 8.0f);
}

// ------------------------------------------------------------------------------------------------ el caso
constexpr u32 kNoMutation = 99;
constexpr u32 kKinds = 20;
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 now{}, rseed{};
    u32 st[3]{};
    // mundo de caminos
    u32 areas[kSlots]{}, loaded[kSlots]{};
    u32 node[kSlots * kNodesPer][3]{};     // [x | y << 16, base | b18 << 16 | b19 << 24, b1A]
    u32 nlink[kSlots][kLinksPer]{};         // direcciones de nodo (area | nodo << 16)
    u32 navi[kSlots][kNaviPer]{};           // direcciones de enlace de carretera (id | area << 10)
    u32 lk[kSlots * kNavNodesPer][3]{};     // [x | y << 16, otroArea | otroNodo << 16, dx | dy << 8 | ancho << 16 | banderas << 24]
    // vehiculo y piloto
    u32 seed16{}, cur{}, start{}, end0{}, fieldC{}, scale{}, curLink{}, nextLink{}, prevLink{};
    u32 sm4{}, sm5{}, sm6{}, lane7{}, lane8{}, mission{}, f3db{}, f50{}, model{}, subtype{};
    // destino, zona del clima, posicion del vehiculo (matriz o transformacion simple)
    u32 dest[3]{}, zone{}, hasMat{}, mat[3]{}, place[3]{};
    // salida del falso de DoPathSearch
    u32 cnt{}, ps0{}, ps1{}, psDist{};
    u32 mutAt{}, mutKind{}, mutVal{};
    u32 gate{};
};

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = Rand(n, 1); c.ecx = Rand(n, 3); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    u32 kk = 100;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    auto Coord = [&]() -> u32 {   // int16 como u16
        const u32 w = R(100);
        if (w < 3) return 0x7FFFu;
        if (w < 6) return 0x8000u;
        if (w < 9) return 0u;
        return u32(i16(int(R(12001)) - 6000)) & 0xFFFFu;
    };
    // Coordenada del mundo (metros): del orden de las de los nodos (coordenadas / 8) o un especial
    auto World = [&](bool allowSpecial) -> u32 {
        const u32 w = R(100);
        if (allowSpecial && w < 7) return Special(n, nk());
        if (w < 14) return B(float(int(R(12001)) - 6000) * 0.125f);
        return B(float(int(R(12001)) - 6000) * 0.125f + float(R(8)) * 0.125f * 0.5f);
    };

    // ---- reloj, rand() del EXE y estaticas
    c.now = 1000u + R(2000000u);
    c.rseed = Rand(n, nk());
    c.st[0] = Rand(n, nk()); c.st[1] = Rand(n, nk()); c.st[2] = Rand(n, nk());
    // ---- areas
    c.areas[0] = R(64);
    do { c.areas[1] = R(64); } while (c.areas[1] == c.areas[0]);
    do { c.areas[2] = R(64); } while (c.areas[2] == c.areas[0] || c.areas[2] == c.areas[1]);
    for (u32 s = 0; s < kSlots; ++s) c.loaded[s] = P(90) ? 1u : 0u;
    auto Unloaded = [&]() -> u32 {
        for (;;) {
            const u32 a = R(64);
            if (a != c.areas[0] && a != c.areas[1] && a != c.areas[2]) return a;
        }
    };
    auto Addr = [&](bool allowBad) -> u32 {
        const u32 s = R(kSlots), j = R(kNodesPer);
        u32 area = c.areas[s];
        if (allowBad && P(6)) area = Unloaded();
        return area | (j << 16);
    };
    auto LinkWord = [&](bool allowBad) -> u32 {
        const u32 s = R(kSlots), id = R(kNavNodesPer);
        u32 area = c.areas[s];
        if (allowBad && P(7)) area = Unloaded();
        return id | (area << 10);
    };
    // ---- vehiculo y piloto
    c.seed16 = P(45) ? 0u : (1u + R(65535));
    c.cur = Addr(true);
    c.start = Addr(false);
    c.end0 = Rand(n, nk());
    c.nextLink = LinkWord(true);
    c.curLink = R(65536); c.prevLink = R(65536);
    c.fieldC = P(15) ? Rand(n, nk()) : R(100000); c.scale = P(10) ? 0xFFFFFFFFu : R(6000);
    c.sm4 = R(256); c.sm5 = P(80) ? (R(2) ? 1u : 0xFFu) : R(256); c.sm6 = P(80) ? (R(2) ? 1u : 0xFFu) : R(256);
    c.lane7 = P(70) ? R(5) : R(256); c.lane8 = P(70) ? R(5) : R(256);
    c.mission = R(256);
    if (P(35)) { static constexpr u32 kList[8]{2, 4, 0x0F, 0x34, 0x11, 0x36, 0x2B, 0x3D}; c.mission = kList[R(8)]; }
    c.f3db = P(25) ? 0x08u : P(25) ? 0x10u : R(256);
    c.f50 = P(40) ? 1u : R(256);
    c.model = P(30) ? 0x21Bu : R(65536);
    c.subtype = P(15) ? 10u : (P(30) ? R(16) : Rand(n, nk()));
    // ---- destino, zona y posicion
    c.dest[0] = World(true); c.dest[1] = World(true); c.dest[2] = World(true);
    c.zone = P(30) ? 0u : P(30) ? 4u : P(50) ? R(10) : R(65536);
    c.hasMat = P(60) ? 1u : 0u;
    for (u32 k = 0; k < 3; ++k) { c.mat[k] = World(true); c.place[k] = World(true); }
    // ---- nodos
    auto KindByte = [&]() -> u32 { return R(256); };
    for (u32 i = 0; i < kSlots * kNodesPer; ++i) {
        const u32 x = Coord(), y = Coord();
        const u32 base = R(7);
        const u32 count = P(4) ? 0u : 1u + R(8);
        const u32 b18 = count | (P(45) ? 0x20u : 0u) | (P(30) ? 0x80u : 0u) | (P(30) ? 0x10u : 0u);
        c.node[i][0] = x | (y << 16);
        c.node[i][1] = base | (b18 << 16) | (R(256) << 24);
        c.node[i][2] = KindByte();
    }
    for (u32 s = 0; s < kSlots; ++s) {
        for (u32 k = 0; k < kLinksPer; ++k) c.nlink[s][k] = P(22) ? c.cur : P(18) ? c.start : Addr(true);
        for (u32 k = 0; k < kNaviPer; ++k) c.navi[s][k] = LinkWord(true);
    }
    auto Flags = [&]() -> u32 { return P(30) ? ((1u + R(7)) | ((1u + R(7)) << 3)) : R(256); };
    for (u32 i = 0; i < kSlots * kNavNodesPer; ++i) {
        auto Dir = [&]() -> u32 { const u32 d = R(10); return d == 0 ? 0u : d == 1 ? 100u : d == 2 ? u32(u8(-100)) : d == 3 ? 127u : d == 4 ? 0x80u : u32(u8(int(R(201)) - 100)); };
        const u32 x = Coord(), y = Coord();
        u32 oa, on;
        if (P(40)) { oa = c.start & 0xFFFFu; on = c.start >> 16; }
        else if (P(15)) { oa = c.cur & 0xFFFFu; on = c.cur >> 16; }
        else if (P(60)) { const u32 a = Addr(true); oa = a & 0xFFFFu; on = a >> 16; }
        else { oa = R(65536); on = R(65536); }
        c.lk[i][0] = x | (y << 16);
        c.lk[i][1] = oa | (on << 16);
        c.lk[i][2] = Dir() | (Dir() << 8) | (R(256) << 16) | (Flags() << 24);
    }
    // ---- salida del falso de DoPathSearch
    {
        const u32 w = R(100);
        c.cnt = w < 14 ? 0u : w < 36 ? 1u : w < 62 ? 2u : w < 84 ? 3u + R(6) : w < 92 ? 0xFFFFu : P(50) ? 0x7FFFu : 0x8000u;
    }
    c.ps0 = Addr(true); c.ps1 = Addr(true);
    c.psDist = P(12) ? Special(n, nk()) : B(float(R(1000000)) * 0.1f);
    c.mutAt = P(30) ? 0u : kNoMutation;
    c.mutKind = c.mutAt == kNoMutation ? 0u : 1u + R(kKinds);
    c.mutVal = P(25) ? Pick(kStackSeeds, n, nk()) : Rand(n, nk());

    // ---- sesgos por familia
    auto AllLoaded = [&] { for (u32 s = 0; s < kSlots; ++s) c.loaded[s] = 1; };
    auto ValidEntry = [&] {   // actual, siguiente y enlace siguiente con el area cargada
        AllLoaded();
        c.cur = c.areas[R(kSlots)] | (R(kNodesPer) << 16);
        c.start = c.areas[R(kSlots)] | (R(kNodesPer) << 16);
        c.nextLink = R(kNavNodesPer) | (c.areas[R(kSlots)] << 10);
    };
    auto AllLinksValid = [&] {   // todos los candidatos y enlaces con el area cargada
        for (u32 s = 0; s < kSlots; ++s) {
            for (u32 k = 0; k < kLinksPer; ++k) c.nlink[s][k] = c.areas[R(kSlots)] | (R(kNodesPer) << 16);
            for (u32 k = 0; k < kNaviPer; ++k) c.navi[s][k] = R(kNavNodesPer) | (c.areas[R(kSlots)] << 10);
        }
    };
    auto Count = [&](u32 lo, u32 hi) { for (u32 i = 0; i < kSlots * kNodesPer; ++i) { u32 b18 = (c.node[i][1] >> 16) & 0xF0u; b18 |= lo + R(hi - lo + 1); c.node[i][1] = (c.node[i][1] & 0xFF00FFFFu) | (b18 << 16); } };
    auto FiniteWorld = [&] { for (u32 k = 0; k < 3; ++k) { c.dest[k] = World(false); c.mat[k] = World(false); c.place[k] = World(false); } };
    auto Zone04 = [&] { c.zone = P(50) ? 0u : 4u; };
    auto ZoneOther = [&] { c.zone = 1u + R(3); if (P(30)) c.zone = 5u + R(65530); };
    auto PathCount = [&](u32 v) { c.cnt = v; };
    switch (f) {
    case 0: break;
    case 1:   // zona 0/4 y ninguna ruta: sale con true
        ValidEntry(); Zone04(); PathCount(0);
        break;
    case 2:   // zona 0/4, hay ruta y la linea recta es mucho mas corta: sale con true
        ValidEntry(); Zone04(); FiniteWorld(); PathCount(1 + R(8));
        c.psDist = P(80) ? B(1.0e6f + float(R(1000000))) : B(1.0e30f);
        break;
    case 3:   // zona 0/4, hay ruta pero no es mucho mas larga: sigue
        ValidEntry(); AllLinksValid(); Zone04(); FiniteWorld(); PathCount(P(50) ? 1u + R(2) : 3u + R(6));
        c.psDist = P(30) ? Special(n, nk()) : B(float(R(1000)) * 0.1f);
        Count(1, 8);
        break;
    case 4:   // una o dos paradas: el primer nodo de la ruta es un vecino del siguiente
        ValidEntry(); AllLinksValid(); FiniteWorld(); Count(1, 8);
        PathCount(1 + R(2)); if (P(60)) ZoneOther();
        c.ps0 = c.areas[R(kSlots)] | (R(kNodesPer) << 16);
        break;
    case 5:   // el primer nodo es el siguiente y el segundo un vecino
        ValidEntry(); AllLinksValid(); FiniteWorld(); Count(1, 8);
        PathCount(2); if (P(60)) ZoneOther();
        c.ps0 = c.start; c.ps1 = c.areas[R(kSlots)] | (R(kNodesPer) << 16);
        break;
    case 6:   // igualdades: el primero (y el segundo) son el siguiente, o la cuenta no es 1 ni 2: ruta general
        ValidEntry(); AllLinksValid(); FiniteWorld(); Count(1, 8);
        PathCount(P(50) ? 1u + R(2) : (P(50) ? 3u + R(5) : 0u)); if (P(60)) ZoneOther();
        c.ps0 = c.start; c.ps1 = P(70) ? c.start : c.areas[R(kSlots)] | (R(kNodesPer) << 16);
        break;
    case 7:   // ruta general: candidatos validos, destino y posicion finitos
        ValidEntry(); AllLinksValid(); FiniteWorld(); Count(2, 8);
        PathCount(P(50) ? 0u : 3u + R(6)); ZoneOther();
        for (u32 s = 0; s < kSlots; ++s) for (u32 k = 0; k < kLinksPer; ++k) if (P(25)) c.nlink[s][k] = c.cur;
        break;
    case 8:   // ruta general con candidatos saltados (el actual, areas descargadas), pocos enlaces o ninguno
        ValidEntry(); FiniteWorld(); PathCount(P(50) ? 0u : 3u + R(6)); ZoneOther();
        switch (R(4)) {
        case 0: Count(0, 1); break;
        case 1: Count(1, 3); for (u32 s = 0; s < kSlots; ++s) for (u32 k = 0; k < kLinksPer; ++k) c.nlink[s][k] = c.cur; break;
        case 2: Count(2, 8); for (u32 s = 0; s < kSlots; ++s) for (u32 k = 0; k < kLinksPer; ++k) c.nlink[s][k] = P(50) ? Unloaded() : c.areas[R(kSlots)] | (R(kNodesPer) << 16); break;
        default: Count(1, 8); AllLinksValid(); break;
        }
        break;
    case 9:   // signo del tramo, carriles, misiones de la lista, contador y distancia entre enlaces
        ValidEntry(); AllLinksValid(); FiniteWorld(); Count(1, 8); PathCount(P(50) ? 0u : 3u + R(6)); ZoneOther();
        c.f50 = P(70) ? 1u : R(4); c.lane8 = R(5); c.lane7 = R(5);
        if (P(30)) for (u32 i = 0; i < kSlots * kNavNodesPer; ++i) c.lk[i][0] = (c.lk[i][0] & 0xFFFF0000u) | (u32(i16(int(R(40)) - 20)) & 0xFFFFu);
        c.f3db = P(30) ? 0x08u : P(30) ? 0x10u : R(8);
        break;
    case 10: case 11: case 12:   // barrido de mutaciones: cada tipo en casos que recorren varias etapas
        ValidEntry(); AllLinksValid(); FiniteWorld(); Count(2, 8);
        PathCount(P(25) ? 1u + R(2) : (P(50) ? 3u + R(5) : 0u)); if (P(70)) ZoneOther(); else Zone04();
        c.mutAt = 0;
        if (f == 10) c.mutKind = 1u + (n / 16) % 10u;
        else if (f == 11) c.mutKind = 11u + (n / 16) % 10u;
        else c.mutKind = 1u + R(kKinds);
        if (c.zone == 0 || c.zone == 4) c.psDist = B(float(R(100)) * 0.1f);
        break;
    case 13:  // rumbos con empates: destino sobre la direccion de un enlace, ejes y diagonales
        ValidEntry(); AllLinksValid(); FiniteWorld(); Count(2, 8); PathCount(P(50) ? 0u : 3u + R(6)); ZoneOther();
        {
            // posicion del vehiculo = nodo siguiente + desplazamiento; destino = vehiculo + multiplo de la direccion hacia un candidato
            const u32 s = [&] { for (u32 q = 0; q < kSlots; ++q) if (c.areas[q] == (c.start & 0xFFFFu)) return q; return 0u; }();
            const u32 sn = s * kNodesPer + (c.start >> 16);
            const float px = float(i16(c.node[sn][0] & 0xFFFFu)) * 0.125f, py = float(i16(c.node[sn][0] >> 16)) * 0.125f;
            const float ox = float(R(9)) - 4.0f, oy = float(R(9)) - 4.0f;
            c.place[0] = B(px + ox); c.place[1] = B(py + oy); c.mat[0] = c.place[0]; c.mat[1] = c.place[1];
            const u32 w = R(6);
            float dx = 0.0f, dy = 0.0f;
            switch (w) {
            case 0: dx = float(R(40)) + 1.0f; break;
            case 1: dy = float(R(40)) + 1.0f; break;
            case 2: dx = -(float(R(40)) + 1.0f); break;
            case 3: dy = -(float(R(40)) + 1.0f); break;
            case 4: { const float k = float(R(30)) + 1.0f; dx = P(50) ? k : -k; dy = P(50) ? k : -k; break; }
            default: { dx = float(int(R(81)) - 40); dy = float(int(R(81)) - 40); break; }
            }
            c.dest[0] = B(px + ox + dx); c.dest[1] = B(py + oy + dy);
        }
        break;
    case 14: case 15: break;   // todo aleatorio
    case 16:  // especiales (NaN, infinitos, desnormales, enormes) en destino, posicion y distancia de la ruta
        ValidEntry(); AllLinksValid(); Count(1, 8);
        for (u32 k = 0; k < 3; ++k) { if (P(50)) c.dest[k] = Special(n, nk()); if (P(40)) c.mat[k] = Special(n, nk()); if (P(40)) c.place[k] = Special(n, nk()); }
        c.psDist = Special(n, nk());
        PathCount(P(30) ? 1u + R(2) : 3u + R(6)); if (P(50)) Zone04(); else ZoneOther();
        break;
    default:  // 17: hasta 15 enlaces por nodo y bases altas
        ValidEntry(); AllLinksValid(); FiniteWorld(); Count(1, 15); PathCount(P(50) ? 0u : 3u + R(6)); ZoneOther();
        c.lane8 = R(7); c.lane7 = R(7);
        break;
    }
    // ---- reglas de seguridad: ningun nodo tiene mas enlaces de los que caben tras su base (base + n <= 16)
    for (u32 i = 0; i < kSlots * kNodesPer; ++i) {
        const u32 base = c.node[i][1] & 0xFFFFu;
        u32 b18 = (c.node[i][1] >> 16) & 0xFFu;
        if ((b18 & 0xFu) > 16u - base) b18 = (b18 & 0xF0u) | (16u - base);
        c.node[i][1] = (c.node[i][1] & 0xFF00FFFFu) | (b18 << 16);
    }
    // ---- reglas de seguridad: el nodo siguiente tiene el area cargada y la ranura cargada
    {
        u32 slot = 0;
        for (u32 s = 0; s < kSlots; ++s) if (c.areas[s] == (c.start & 0xFFFFu)) slot = s;
        c.loaded[slot] = 1;
        // ---- la ruta de 1 o 2 nodos tiene sus nodos en la tabla de enlaces del nodo siguiente (el EXE los busca sin limite)
        const u32 base = c.node[slot * kNodesPer + (c.start >> 16)][1] & 0xFFFFu;
        const u32 count = c.cnt & 0xFFFFu;
        if (count == 1 || count == 2) {
            if (c.ps0 != c.start) c.ps0 = c.nlink[slot][base + R(16u - base)];
            if (c.ps0 == c.start && count == 2 && c.ps1 != c.start) c.ps1 = c.nlink[slot][base + R(16u - base)];
        }
        // la mutacion de la base o del numero de enlaces del nodo siguiente no debe dejar una busqueda sin resultado
        if ((c.mutKind == 18 || c.mutKind == 19) && (count == 1 || count == 2)) c.cnt = 3;
    }
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack).U("now", c.now).U("rseed", c.rseed);
    j.A("st", std::vector<u32>(c.st, c.st + 3));
    j.A("areas", std::vector<u32>(c.areas, c.areas + kSlots)).A("loaded", std::vector<u32>(c.loaded, c.loaded + kSlots));
    {
        std::vector<u32> v;
        for (u32 i = 0; i < kSlots * kNodesPer; ++i) for (u32 k = 0; k < 3; ++k) v.push_back(c.node[i][k]);
        j.A("node", v);
    }
    {
        std::vector<u32> v;
        for (u32 s = 0; s < kSlots; ++s) for (u32 k = 0; k < kLinksPer; ++k) v.push_back(c.nlink[s][k]);
        j.A("nlink", v);
    }
    {
        std::vector<u32> v;
        for (u32 s = 0; s < kSlots; ++s) for (u32 k = 0; k < kNaviPer; ++k) v.push_back(c.navi[s][k]);
        j.A("navi", v);
    }
    {
        std::vector<u32> v;
        for (u32 i = 0; i < kSlots * kNavNodesPer; ++i) for (u32 k = 0; k < 3; ++k) v.push_back(c.lk[i][k]);
        j.A("lk", v);
    }
    j.U("seed16", c.seed16).U("cur", c.cur).U("start", c.start).U("end0", c.end0).U("field_c", c.fieldC).U("scale", c.scale).U("cur_link", c.curLink).U("next_link", c.nextLink).U("prev_link", c.prevLink);
    j.U("sm4", c.sm4).U("sm5", c.sm5).U("sm6", c.sm6).U("lane7", c.lane7).U("lane8", c.lane8).U("mission", c.mission).U("f3db", c.f3db).U("f50", c.f50).U("model", c.model).U("subtype", c.subtype);
    j.A("dest", std::vector<u32>(c.dest, c.dest + 3)).U("zone", c.zone).U("has_mat", c.hasMat).A("mat", std::vector<u32>(c.mat, c.mat + 3)).A("place", std::vector<u32>(c.place, c.place + 3));
    j.U("cnt", c.cnt).U("ps0", c.ps0).U("ps1", c.ps1).U("ps_dist", c.psDist);
    j.U("mut_at", c.mutAt).U("mut_kind", c.mutKind).U("mut_val", c.mutVal).U("gate", c.gate);
    return j.End();
}
}

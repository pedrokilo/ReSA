#pragma once
// Fixtures del banco Car84E (lote 84E): CCarCtrl::GenerateCarCreationCoors2 (EXE 0x424210), 18432 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::GenerateCarCreationCoors2  0x424210  cdecl, 15 dwords de pila, `ret` simple, devuelve bool en AL   (hook CCarCtrl / GenerateCarCreationCoors2, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) y las familias 16 y 17 (1024 casos cada una).
// Mundo de caminos de juguete: 3 ranuras de area (id de area distinto en 0..63; las ranuras 0 y 1 siempre cargadas, la 2 casi siempre), 4 nodos por area, 16 enlaces de nodo y de navegacion por area y 8 enlaces
// de carretera (14 bytes) por area. Los cuatro nodos guardados (estaticas 0x969104, 0x969100, 0x9690FC y 0x9690F8), el ultimo punto de refresco (0x9690EC) y el reloj de la proxima actualizacion (0x9690E8) tambien
// son entradas del caso.
// Reglas de seguridad (el EXE no comprueba nada de esto): todo nodo cargado tiene entre 1 y 8 enlaces (el EXE divide por el numero de enlaces) y una base de enlaces <= 7; las direcciones de la tabla de nodos y
// los nodos que devuelven los falsos tienen area de una ranura (o 0xFFFF) y numero de nodo < 4; el area propia de cada nodo (+0x12) es la de una ranura cargada; los enlaces de navegacion tienen identificador < 8.
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

namespace C84EFix {
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
// Arenas: guarda + ventana + guarda
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kOutWin = 0x20;      // salidas: origen (+0, 12 bytes), nodo 1 (+0x10), nodo 2 (+0x14), fraccion (+0x18)
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kNodeArena = Arena(kSlots * kNodesPer * kNodeBytes), kNlArena = Arena(kSlots * kLinksPer * 4), kNaviArena = Arena(kSlots * kNaviPer * 2),
                      kNnArena = Arena(kSlots * kNavNodesPer * kLinkBytes), kOutArena = Arena(kOutWin);

constexpr u32 kTimeAddr = 0xB7CB84;       // CTimer::m_snTimeInMilliseconds
constexpr u32 kZoneAddr = 0xC81314;       // zona del clima (word): la funcion no la lee; el banco comprueba que no cambia
constexpr u32 kGStringLen = 96;           // bytes de gString (0xB71670) que se rellenan y comparan

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 84005u); }
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

// Valor flotante de la mutacion (el oraculo repite la tabla): tabla de especiales o (v % 2001 - 1000) / 8
inline u32 MutFloat(u32 v) {
    static constexpr std::array<u32, 8> t{0u, 0x80000000u, 1u, 0x7F800000u, 0xFF800000u, 0xFFC00000u, 0x7F7FFFFFu, 0x3F800000u};
    return (v & 3u) == 0 ? t[(v >> 2) % 8u] : B(float(int(v % 2001u) - 1000) / 8.0f);
}
// rand() del CRT del EXE (0x821B1E): semilla * 0x343FD + 0x269EC3, resultado (semilla >> 16) & 0x7FFF
inline u32 Lcg(u32& state) { state = state * 0x343FDu + 0x269EC3u; return (state >> 16) & 0x7FFFu; }

// ------------------------------------------------------------------------------------------------ el caso
constexpr u32 kNoMutation = 99;
constexpr u32 kKinds = 7;
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 now{}, rseed{};
    // estaticas: guarda de inicializacion (0x969108), proxima actualizacion (0x9690E8), ultimo punto (0x9690EC..F4), nodos guardados A (0x969104), B (0x969100), C (0x9690FC), D (0x9690F8),
    // las tres de la secuencia (0x969078, 0x969074, 0x969070), la bandera 0xC0FCB5
    u32 guard{}, nextTime{}, last[3]{}, sA{}, sB{}, sC{}, sD{}, seq[3]{}, busy{};
    // argumentos: posicion, (radius, arg3) = direccion x, y, arg4 = limite, arg5, arg6, arg7, arg12, arg13
    u32 pos[3]{}, rad{}, a3{}, a4{}, f5{}, a6{}, a7{}, a12{}, a13{};
    // mundo de caminos
    u32 areas[kSlots]{}, loaded[kSlots]{};
    u32 node[kSlots * kNodesPer][3]{};     // [x | y << 16, z | base << 16, area propia | b18 << 16 | b19 << 24]
    u32 nlink[kSlots][kLinksPer]{};         // direcciones de nodo (area | nodo << 16)
    u32 navi[kSlots][kNaviPer]{};           // direcciones de enlace de carretera (id | area << 10)
    u32 lk[kSlots * kNavNodesPer][2]{};     // [x | y << 16, banderas (+0xB)]
    // salidas de los falsos: dos llamadas a Find2NodesForCarCreation (A, B, C, D), el nodo mas cercano, la visibilidad de las esferas, el resultado de ThisNodeWillLeadIntoADeadEnd
    u32 f2[4]{}, closest{}, vis[4]{}, dead[2]{};
    u32 mutEv{}, mutKind{}, mutVal{};
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
    auto Coord = [&](bool wide) -> u32 {   // int16 como u16
        const u32 w = R(100);
        if (w < 3) return 0x7FFFu;
        if (w < 6) return 0x8000u;
        if (w < 8) return 0u;
        return u32(i16(int(R(wide ? 6001 : 4801)) - (wide ? 3000 : 2400))) & 0xFFFFu;
    };
    // Coordenada del mundo (metros): del orden de las de los nodos (coordenadas / 8) o un especial
    auto World = [&](bool allowSpecial) -> u32 {
        if (allowSpecial && P(5)) return Special(n, nk());
        return B(float(R(6001)) / 10.0f - 300.0f);
    };
    auto Dist = [&](u32 lo, u32 span, bool allowSpecial) -> u32 {
        if (allowSpecial && P(5)) return Special(n, nk());
        return B(float(lo) + float(R(span * 8)) / 8.0f);
    };

    // ---- reloj, rand() del EXE
    c.now = P(8) ? 0xFFFFFF00u + R(256) : 100000u + R(2000000u);
    c.rseed = Rand(n, nk());
    // ---- areas
    c.areas[0] = R(64);
    do { c.areas[1] = R(64); } while (c.areas[1] == c.areas[0]);
    do { c.areas[2] = R(64); } while (c.areas[2] == c.areas[0] || c.areas[2] == c.areas[1]);
    c.loaded[0] = 1; c.loaded[1] = 1; c.loaded[2] = P(85) ? 1u : 0u;
    auto LoadedSlot = [&]() -> u32 { for (;;) { const u32 s = R(kSlots); if (c.loaded[s]) return s; } };
    auto AddrLoaded = [&]() -> u32 { return c.areas[LoadedSlot()] | (R(kNodesPer) << 16); };
    auto AddrAny = [&]() -> u32 { return c.areas[R(kSlots)] | (R(kNodesPer) << 16); };
    auto AddrOrNone = [&]() -> u32 { const u32 w = R(100); return w < 14 ? (0xFFFFu | (R(kNodesPer) << 16)) : w < 80 ? AddrLoaded() : AddrAny(); };
    auto LinkWord = [&]() -> u32 { return R(kNavNodesPer) | (c.areas[R(kSlots)] << 10); };

    // ---- estaticas
    c.guard = P(45) ? 0x1Fu : R(32);
    if (P(20)) c.guard |= Rand(n, nk()) << 5;
    {
        const u32 w = R(100);   // distancia al ultimo punto: <= 10 (sin refresco por distancia) o mayor
        const u32 px = World(false), py = World(false), pz = World(false);
        c.pos[0] = px; c.pos[1] = py; c.pos[2] = pz;
        if (w < 45) {
            c.last[0] = B(F(px) + float(int(R(15)) - 7) * 0.5f); c.last[1] = B(F(py) + float(int(R(15)) - 7) * 0.5f);   // |delta| <= 7 en cada eje: longitud <= 9.9
        } else {
            c.last[0] = B(F(px) + float(int(R(401)) - 200) * 0.5f); c.last[1] = B(F(py) + float(int(R(401)) - 200) * 0.5f);
        }
        c.last[2] = World(false);
        if (P(5)) c.last[0] = Special(n, nk());
        if (P(5)) c.last[1] = Special(n, nk());
        if (P(4)) c.pos[0] = Special(n, nk());
        if (P(4)) c.pos[1] = Special(n, nk());
        if (P(3)) c.pos[2] = Special(n, nk());
    }
    c.nextTime = P(40) ? c.now - 1u - R(5000) : c.now + R(5000);
    c.sA = AddrOrNone(); c.sB = AddrOrNone(); c.sC = AddrOrNone(); c.sD = AddrOrNone();
    c.seq[0] = Rand(n, nk()); c.seq[1] = Rand(n, nk()); c.seq[2] = Rand(n, nk());
    c.busy = R(256);
    // ---- argumentos
    auto Unit = [&]() -> u32 { return P(4) ? Special(n, nk()) : B(float(int(R(201)) - 100) / 100.0f); };
    c.rad = Unit(); c.a3 = Unit(); c.a4 = P(4) ? Special(n, nk()) : B(float(int(R(201)) - 100) / 100.0f);
    c.f5 = R(2);
    c.a6 = Dist(20, 200, true); c.a7 = Dist(10, 250, true);
    c.a12 = R(2); c.a13 = P(30) ? 1u : 0u;
    // ---- nodos
    for (u32 i = 0; i < kSlots * kNodesPer; ++i) {
        const u32 x = Coord(false), y = Coord(false);
        const u32 z = P(5) ? (P(50) ? 0x7FFFu : 0x8000u) : (u32(i16(int(R(241)) - 120)) & 0xFFFFu);
        const u32 base = R(8);
        const u32 f18 = (1u + R(8)) | (P(25) ? 0x10u : 0u) | (P(15) ? 0x20u : 0u) | (P(30) ? 0x40u : 0u) | (P(30) ? 0x80u : 0u);
        const u32 f19 = P(40) ? (0x20u | R(16) | (P(30) ? 0x10u : 0u) | (P(30) ? 0x80u : 0u)) : R(256);
        c.node[i][0] = x | (y << 16);
        c.node[i][1] = z | (base << 16);
        c.node[i][2] = c.areas[LoadedSlot()] | (f18 << 16) | (f19 << 24);
    }
    for (u32 s = 0; s < kSlots; ++s) {
        for (u32 k = 0; k < kLinksPer; ++k) c.nlink[s][k] = AddrAny();
        for (u32 k = 0; k < kNaviPer; ++k) c.navi[s][k] = LinkWord();
    }
    for (u32 i = 0; i < kSlots * kNavNodesPer; ++i) {
        const u32 x = Coord(false), y = Coord(false);
        const u32 flags = P(30) ? ((1u + R(7)) | ((1u + R(7)) << 3)) : R(256);
        c.lk[i][0] = x | (y << 16);
        c.lk[i][1] = flags;
    }
    // ---- salidas de los falsos
    for (u32 k = 0; k < 4; ++k) { c.f2[k] = AddrOrNone(); c.vis[k] = R(2); }
    c.closest = AddrOrNone();
    c.dead[0] = R(2); c.dead[1] = R(2);
    c.mutEv = P(30) ? R(6) : kNoMutation;
    c.mutKind = c.mutEv == kNoMutation ? 0u : 1u + R(kKinds);
    c.mutVal = P(25) ? Pick(kStackSeeds, n, nk()) : Rand(n, nk());

    // ---- sesgos por familia
    // Semilla de rand() cuyo primer valor tiene (valor & 0xF) == 4 (probabilidad 1/16): el nodo de partida sale de FindNodeClosestToCoors
    auto SeedClosest = [&]() {
        for (u32 k = 0; k < 400; ++k) {
            u32 s = Rand(n, 900 + k);
            u32 t = s;
            if ((Lcg(t) & 0xFu) == 4) { c.rseed = s; return; }
        }
    };
    // Coordenada int16 (x 0.125) a menos de 100 m de la posicion dada (eje 0 = x, 1 = y); si la posicion no es finita o es enorme, una cualquiera
    auto Near = [&](int axis, int span = 800) -> u32 {
        const float p = F(c.pos[axis]);
        if (!(std::fabs(p) < 1000.0f)) return Coord(false);
        return u32(i16(int(p * 8.0f) + int(R(u32(2 * span + 1))) - span)) & 0xFFFFu;
    };
    auto NodesNear = [&] { for (u32 i = 0; i < kSlots * kNodesPer; ++i) { const u32 x = Near(0), y = Near(1); c.node[i][0] = x | (y << 16); } };
    auto NoRefresh = [&] {
        c.nextTime = c.now + 1u + R(5000);
        c.last[0] = B(F(c.pos[0]) + 1.0f); c.last[1] = B(F(c.pos[1]) - 1.0f);
        if (!(c.nextTime > c.now)) c.nextTime = c.now;   // sin desbordamiento
    };
    auto AllLoadedStatics = [&] { c.sA = AddrLoaded(); c.sB = AddrLoaded(); c.sC = AddrLoaded(); c.sD = AddrLoaded(); for (u32 k = 0; k < 4; ++k) c.f2[k] = AddrLoaded(); };
    switch (f) {
    case 0: break;
    case 1:   // sin refresco: se usan las estaticas del caso
        NoRefresh();
        break;
    case 2:   // refresco por distancia (y a veces por reloj)
        c.last[0] = B(F(c.pos[0]) + 20.0f + float(R(100))); c.last[1] = B(F(c.pos[1]));
        c.nextTime = c.now + 1u + R(4000);
        if (c.nextTime < c.now) c.nextTime = c.now;
        break;
    case 3:   // refresco solo por reloj
        c.last[0] = B(F(c.pos[0]) + 1.0f); c.last[1] = B(F(c.pos[1]) + 2.0f);
        c.nextTime = c.now - 1u - R(5000);
        if (c.nextTime > c.now) c.nextTime = 0u;
        break;
    case 4:   // bloque de la bandera: primer nodo cargado, tramo ancho o estrecho, cerca o lejos
        c.nextTime = c.now - 1u - R(3000);
        if (c.nextTime > c.now) c.nextTime = 0u;
        AllLoadedStatics();
        for (u32 i = 0; i < kSlots * kNodesPer; ++i) c.node[i][2] = (c.node[i][2] & 0x00FFFFFFu) | ((P(50) ? (R(256) & ~0x30u) : (0x20u | R(16))) << 24);
        for (u32 i = 0; i < kSlots * kNavNodesPer; ++i) {
            if (P(85)) { const u32 x = Near(0, 360), y = Near(1, 360); c.lk[i][0] = x | (y << 16); }
            else c.lk[i][0] = (u32(i16(int(R(4001)) - 2000)) & 0xFFFFu) | (u32(i16(int(R(4001)) - 2000)) << 16);
            c.lk[i][1] = P(15) ? (R(2) ? 1u : (R(2) ? 0x10u : 0x08u)) : R(256);
        }
        break;
    case 5:   // el nodo de partida sale de FindNodeClosestToCoors
        SeedClosest(); c.a13 = 0;
        break;
    case 6:   // primer rand() % 16 == 4 pero arg13 = true: se consume otro rand()
        SeedClosest(); c.a13 = 1;
        break;
    case 7:   // saltos por banderas 0x20 con arg12 = true
        c.a12 = 1;
        for (u32 i = 0; i < kSlots * kNodesPer; ++i) if (P(60)) c.node[i][2] |= 0x20u << 16;
        break;
    case 8:   // direccion y limite del producto escalar con ambos sentidos de arg5; puntos aceptados con frecuencia
        c.a12 = P(30) ? 1u : 0u;
        NodesNear();
        for (u32 i = 0; i < kSlots * kNodesPer; ++i) c.node[i][2] &= ~(0x20u << 16);
        c.a6 = Dist(30, 80, false); c.a7 = Dist(10, 60, false);
        break;
    case 9:   // nodos sin salida con y sin arg12
        for (u32 i = 0; i < kSlots * kNodesPer; ++i) { c.node[i][2] &= ~(0x20u << 16); if (P(70)) c.node[i][2] |= 0x10u << 16; }
        NodesNear();
        c.a6 = Dist(30, 80, false); c.a7 = Dist(10, 60, false);
        for (u32 k = 0; k < 4; ++k) c.vis[k] = P(50) ? 1u : 0u;
        break;
    case 10: case 11: case 12:   // barrido de mutaciones
        NodesNear();
        c.mutEv = (f == 10) ? ((n / 16) % 4) : (f == 11) ? 2u + ((n / 16) % 4) : R(8);
        c.mutKind = 1u + ((n / 64) + f) % kKinds;
        for (u32 i = 0; i < kSlots * kNodesPer; ++i) c.node[i][2] &= ~(0x20u << 16);
        c.a6 = Dist(30, 80, false); c.a7 = Dist(10, 60, false);
        break;
    case 13:  // pendiente: z grande
        NodesNear();
        for (u32 i = 0; i < kSlots * kNodesPer; ++i) {
            c.node[i][1] = (u32(i16(int(R(3201)) - 1600)) & 0xFFFFu) | (c.node[i][1] & 0xFFFF0000u);
            c.node[i][2] &= ~(0x20u << 16);
        }
        c.a6 = Dist(30, 80, false); c.a7 = Dist(10, 60, false);
        break;
    case 14:  // nodos lejanos con saltos: se supera el recorrido de 230 m
        c.a12 = 1;
        for (u32 i = 0; i < kSlots * kNodesPer; ++i) {
            c.node[i][2] |= 0x20u << 16;
            c.node[i][0] = ((i & 1u) ? 0x0900u : 0xF700u) | (u32(i16((i & 2u) ? 1500 : -1500)) << 16);
        }
        break;
    case 15: break;   // todo aleatorio
    case 16:  // especiales (NaN, infinitos, desnormales, enormes) en posicion, ultimo punto y argumentos
        for (u32 k = 0; k < 3; ++k) { if (P(40)) c.pos[k] = Special(n, nk()); if (P(40)) c.last[k] = Special(n, nk()); }
        if (P(50)) c.a6 = Special(n, nk());
        if (P(50)) c.a7 = Special(n, nk());
        if (P(40)) c.a4 = Special(n, nk());
        if (P(40)) c.rad = Special(n, nk());
        if (P(40)) c.a3 = Special(n, nk());
        break;
    default:  // 17: dos nodos guardados invalidos o de area sin cargar, areas ajenas
        c.loaded[2] = 0;
        c.sA = AddrAny(); c.sB = AddrAny(); c.sC = AddrAny(); c.sD = AddrAny();
        for (u32 k = 0; k < 4; ++k) c.f2[k] = P(30) ? (0xFFFFu | (R(kNodesPer) << 16)) : AddrAny();
        c.closest = P(30) ? (0xFFFFu | (R(kNodesPer) << 16)) : AddrAny();
        for (u32 s = 0; s < kSlots; ++s) for (u32 k = 0; k < kNaviPer; ++k) if (P(30)) c.navi[s][k] = R(kNavNodesPer) | (R(64) << 10);
        break;
    }
    // ---- reglas de seguridad: las areas propias de los nodos y de los enlaces de navegacion son validas; un caso con ranura 2 sin cargar no deja ningun nodo propio apuntando a ella
    for (u32 i = 0; i < kSlots * kNodesPer; ++i) {
        const u32 own = c.node[i][2] & 0xFFFFu;
        bool ok = false;
        for (u32 s = 0; s < kSlots; ++s) if (c.loaded[s] && c.areas[s] == own) ok = true;
        if (!ok) c.node[i][2] = (c.node[i][2] & 0xFFFF0000u) | c.areas[LoadedSlot()];
    }
    // el numero de enlaces de cada nodo (nibble bajo de +0x18) no puede ser 0
    for (u32 i = 0; i < kSlots * kNodesPer; ++i) {
        u32 f18 = (c.node[i][2] >> 16) & 0xFFu;
        if ((f18 & 0xFu) == 0) { f18 |= 1u; c.node[i][2] = (c.node[i][2] & 0xFF00FFFFu) | (f18 << 16); }
    }
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack).U("now", c.now).U("rseed", c.rseed);
    j.U("guard", c.guard).U("next_time", c.nextTime).A("last", std::vector<u32>(c.last, c.last + 3));
    j.U("sa", c.sA).U("sb", c.sB).U("sc", c.sC).U("sd", c.sD).A("seq", std::vector<u32>(c.seq, c.seq + 3)).U("busy", c.busy);
    j.A("pos", std::vector<u32>(c.pos, c.pos + 3)).U("rad", c.rad).U("a3", c.a3).U("a4", c.a4).U("f5", c.f5).U("a6", c.a6).U("a7", c.a7).U("a12", c.a12).U("a13", c.a13);
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
        for (u32 i = 0; i < kSlots * kNavNodesPer; ++i) for (u32 k = 0; k < 2; ++k) v.push_back(c.lk[i][k]);
        j.A("lk", v);
    }
    j.A("f2", std::vector<u32>(c.f2, c.f2 + 4)).U("closest", c.closest).A("vis", std::vector<u32>(c.vis, c.vis + 4)).A("dead", std::vector<u32>(c.dead, c.dead + 2));
    j.U("mut_ev", c.mutEv).U("mut_kind", c.mutKind).U("mut_val", c.mutVal).U("gate", c.gate);
    return j.End();
}
}

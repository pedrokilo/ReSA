#pragma once
// Fixtures del banco Fn607B (COLAB-607A): 16384 casos de CADA una de tres funciones del EXE que 0x607A90 (CPedAcquaintanceScanner::ScanEntities) llama. Solo C++ estandar (sin tipos del juego): las incluyen
// Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
//  0 = 0x603AF0 CPedAcquaintanceScanner::CanJoinRiotAgainst(ped, otro): cdecl, dos punteros de pila (`ret`); devuelve AL (bool). Colaboradores con hook (falsos identicos en el EXE y en el cuerpo de la DLL):
//      CPed::IsPlayer 0x5DF8F0, CPedIntelligence::Respects 0x601C90, CPedGroups::GetPedsGroup 0x5F7E80 y CPedGroupMembership::GetLeader 0x5F69A0; IsPedTypeGang 0x5FE9C0 corre REAL en el EXE (hook apagado)
//      y expandido en linea en la DLL (funcion pura del tipo).
//  1 = 0x6042B0 CDecisionMakerTypes::HasResponseToAnyEvent(ped, eventos, contador): thiscall (ECX = el CDecisionMakerTypes), tres dwords de pila (`ret 0Ch`); devuelve AL (bool). Sin colaboradores.
//  2 = 0x607560 CPedAcquaintanceScanner::FindAcquaintance(ped, prioridad, actual, candidato, salidaA, salidaB): thiscall (ECX = el escaner), seis dwords de pila (`ret 18h`); devuelve EAX (int). Colaboradores:
//      con hook (falsos identicos en el EXE y en el cuerpo de la DLL): CPedIntelligence::CanSeeEntityWithLights 0x605550 (devuelve float en ST0), CAcquaintance::GetAcquaintances 0x608970, CPedType::GetPedFlag 0x608830,
//      CGameLogic::LaRiotsActiveHere 0x441C10 y 0x603AF0 (CanJoinRiotAgainst, con hook desde este caso); sin hook ni funcion propia: 0x606BA0 (falso solo en la direccion del EXE, la DLL la llama por esa direccion).
//      Globales del juego que lee: CTimer::m_snTimeInMilliseconds 0xB7CB84 y los dos enteros 0x8D235C / 0x8D2360 (se fijan por caso y se restauran).
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A: el ped; B: el otro ped / candidato; C: escaner, inteligencia, lista de eventos, grupos y celdas de salida; D: el CDecisionMakerTypes de la
// funcion 1) con guardas de 0x40. Familia = n % 16: tipo = familia % 4 (colocacion de los objetos: 0 alineado, 1 desalineado, 2 al final de su zona, 3 al principio) y relleno de las arenas = familia / 4
// (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F607BFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 3;
constexpr u32 kEntryAddresses[kFunctions]{0x603AF0, 0x6042B0, 0x607560};

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWinA = 0x1000, kWinB = 0x1000, kWinC = 0x1000, kWinD = 0x18000;   // ventana de cada arena (la D solo la usa la funcion 1)
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Zonas de la arena C: escaner +0x000, inteligencia +0x100, lista de eventos +0x200, celda de salidaA +0x400, de salidaB +0x500, grupo +0x600, lider +0x700
constexpr u32 kZoneScanner = 0x000, kZoneIntel = 0x100, kZoneList = 0x200, kZoneOutA = 0x400, kZoneOutB = 0x500, kZoneGroup = 0x600, kZoneLeader = 0x700;
constexpr u32 kPedSpan = 0x5A0;         // bytes del objeto peaton que el banco rellena (hasta +0x59C)
constexpr u32 kIntelSpan = 0xB8;        // la inteligencia: hasta +0xB4 (tipo de decision maker)
constexpr u32 kMaxEvents = 24;          // entradas de la lista de eventos
constexpr u32 kRowBytes = 60;           // bytes de una decision
constexpr u32 kMakerBytes = 41 * kRowBytes;   // bytes de un CDecisionMaker
constexpr u32 kDmtSpan = 0xDE90;        // bytes del CDecisionMakerTypes
constexpr u32 kEventTable = 0xC034;     // m_EventIndices dentro del CDecisionMakerTypes
constexpr u32 kRandomMaker = 0xC1B4, kMissionMaker = 0xCB50, kPlayerMaker = 0xD4EC;

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
    std::array<u32, 40> a{};
    std::vector<u32> w;         // datos de longitud variable (ver el generador de cada funcion)
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

// Desplazamiento de un objeto de `span` bytes dentro de una zona [lo, lo + zone) segun el tipo: 0 alineado, 1 desalineado, 2 al final de la zona, 3 al principio
inline u32 Place(Gen& g, u32 kind, u32 lo, u32 zone, u32 span) {
    const u32 room = zone > span + 8 ? zone - span - 8 : 0;
    switch (kind) {
    case 0: return lo + 4 * g.R(room / 4 + 1);
    case 1: return lo + 4 * g.R(room / 4 + 1) + 1 + g.R(3);
    case 2: return lo + room - 4 * g.R(2);
    default: return lo + 4 * g.R(2);
    }
}

inline bool IsNan(u32 w) { return (w & 0x7F800000u) == 0x7F800000u && (w & 0x007FFFFFu) != 0; }
inline u32 Bits(float f) { return std::bit_cast<u32>(f); }
// Un bool del EXE: el valor en el bit 0 y basura en los bits 8..31 (los llamadores solo miran AL)
inline u32 BoolWord(Gen& g, u32 pct) { const bool v = g.P(pct); return (v ? 1u : 0u) | (g.P(50) ? (g.W() & 0xFFFFFF00u) : 0u); }
// Un tipo de peaton: policia (6), medico (18), bombero (19), jugadores (0, 1), bandas (7..16), cualquiera 0..31 o extremos
inline u32 PedType(Gen& g, bool copBias) {
    const u32 k = g.R(100);
    if (k < (copBias ? 20u : 12u)) return 6u;
    if (k < 20) return g.Pk({18u, 19u});
    if (k < 26) return g.Pk({0u, 1u});
    if (k < 56) return 7u + g.R(10);
    if (k < 92) return g.R(32);
    return g.Pk({0xFFFFFFFFu, 32u, 33u, 0x80000000u, 0x7FFFFFFFu, 6u, 0xFFFFFFF9u, g.W()});
}
inline u32 VisibilityBits(Gen& g) {
    const u32 k = g.R(100);
    if (k < 22) return Bits(float(int(g.R(2000000)) + 1) / 1000.0f);                                    // positiva normal
    if (k < 40) return Bits(-float(int(g.R(2000000)) + 1) / 1000.0f);                                   // negativa normal
    if (k < 52) return 0u;                                                                              // +0
    if (k < 62) return 0x80000000u;                                                                     // -0
    if (k < 70) return 0x7FC00000u | (g.W() & 0x003FFFFFu);                                             // NaN silencioso
    if (k < 76) return 0xFF800001u | (g.W() & 0x007FFFFFu);                                             // NaN senalizante con signo
    if (k < 82) return g.Pk({0x7F800000u, 0xFF800000u});                                                // infinitos
    if (k < 90) return g.Pk({0x00000001u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00800000u, 0x80800000u});   // denormales y minimos
    if (k < 94) return g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x3F800000u, 0xBF800000u});
    return g.W();
}

// ------------------------------------------------------------------------------------------------ generadores
// Indices comunes: a[0] tipo de colocacion (familia % 4); a[1] relleno (familia / 4); a[2..5] semillas de las arenas A, B, C, D.
//
// FUNCION 0 (CanJoinRiotAgainst):
//   a[6] ped en A; a[7] otro en B; a[8] tipo del ped (+0x598); a[9] tipo del otro; a[10] byte de +0x484 del ped (creado por); a[11] modo del puntero de inteligencia (+0x47C del ped: 0 = la inteligencia
//   de la arena C en a[12]; 1 = el valor crudo a[13]); a[12] inteligencia en C; a[13] valor crudo; a[14] objeto grupo en C (el que devuelve GetPedsGroup); a[15] objeto lider en C; a[16] valor crudo (no nulo)
//   del lider. w[0..3] IsPlayer (bool por llamada), w[4..6] GetLeader (codigo por llamada: 0 nulo, 1 el ped, 2 el otro, 3 el lider de C, 4 el valor crudo), w[7] Respects (bool), w[8] GetPedsGroup (1 =
//   el grupo de C, otro = nulo), w[9..10] reservados (IsPedTypeGang corre real: sin secuencia).
constexpr u32 kW0 = 11;
// FUNCION 1 (HasResponseToAnyEvent):
//   a[6] ped en A; a[7] inteligencia en C (+0xB4 = tipo de decision maker, ped +0x47C apunta a ella); a[8] lista de eventos en C; a[9] el CDecisionMakerTypes en D; a[10] tipo de decision maker (+0xB4);
//   a[11] byte de +0x484 del ped (creado por); a[12] contador (pila); a[13] numero de eventos escritos (entradas de la lista y de la tabla de indices: min(max(contador, 0), 24)).
//   w[6*j + 0..5], j < 24: evento, indice de decision, desplazamiento de la fila en D, clase de escritura (0 seis 200; 1 seis 200 salvo la posicion; 2 sin escribir), posicion, valor.
constexpr u32 kW1 = 6 * kMaxEvents;
// FUNCION 2 (FindAcquaintance):
//   a[6] ped en A; a[7] candidato en B; a[8] escaner en C; a[9] celda salidaA en C; a[10] celda salidaB en C; a[11] inteligencia en C (ped +0x47C apunta a ella); a[12] prioridad; a[13] actual (acumulador);
//   a[14] tipo del candidato; a[15..19] acquaintances del ped (+0x4E0..); a[20] CTimer::m_snTimeInMilliseconds; a[21] int de 0x8D235C; a[22] int de 0x8D2360; a[23] valor previo de salidaA; a[24] valor previo
//   de salidaB; a[25] inicio del temporizador del escaner (+0); a[26] intervalo (+4); a[27] byte de +8 | byte de +9 << 8.
//   w[0] bits de la visibilidad (0x605550); w[1..5] LaRiotsActiveHere (bool por llamada); w[6..10] CanJoinRiotAgainst; w[11..15] 0x606BA0.
constexpr u32 kW2 = 16;

inline void CommonHeader(C& c, Gen& g) {
    const u32 f = c.family;
    c.a[0] = f % 4;
    c.a[1] = f / 4;
    c.a[2] = c.seed * 13u + 50u; c.a[3] = c.a[2] + 1u; c.a[4] = c.a[2] + 2u; c.a[5] = c.a[2] + 3u;
    (void)g;
}

inline void MakeFn0(C& c, Gen& g) {
    const u32 kind = c.a[0];
    c.a[6] = Place(g, kind, 0, u32(kWinA), kPedSpan);
    c.a[7] = Place(g, kind, 0, u32(kWinB), kPedSpan);
    c.a[8] = PedType(g, false);
    c.a[9] = g.P(30) ? c.a[8] : PedType(g, false);
    {
        const u32 k = g.R(100);
        c.a[10] = k < 20 ? 2u : (k < 40 ? 1u : (k < 55 ? 0u : (k < 65 ? 3u : g.R(256))));
    }
    c.a[11] = g.P(85) ? 0u : 1u;
    c.a[12] = Place(g, kind, kZoneIntel, 0x100, kIntelSpan);
    c.a[13] = g.Pk({0x12345678u, 0xDEADBEEFu, 1u, 0x80000000u, g.W() | 1u});
    c.a[14] = Place(g, kind, kZoneGroup, 0x100, 0x10);
    c.a[15] = Place(g, kind, kZoneLeader, 0x100, 0x10);
    c.a[16] = g.Pk({0xCAFEBABEu, 0x7FFFFFFCu, 4u, g.W() | 1u});
    c.w.assign(kW0, 0);
    const u32 pPlayer = g.Pk({10u, 30u, 60u});
    for (u32 i = 0; i < 4; ++i) c.w[i] = BoolWord(g, pPlayer);
    for (u32 i = 0; i < 3; ++i) c.w[4 + i] = g.P(35) ? 0u : g.R(5);
    c.w[7] = BoolWord(g, g.Pk({20u, 50u, 80u}));
    c.w[8] = g.P(60) ? 1u : (g.P(50) ? 0u : (g.W() & ~1u));
    c.w[9] = g.W(); c.w[10] = g.W();
}

inline u32 MakerBase(i32 dm, u32 created) {   // desplazamiento del CDecisionMaker dentro del CDecisionMakerTypes
    if (dm == -2) return kPlayerMaker;
    if (dm == -1) return created == 2 ? kMissionMaker : kRandomMaker;
    return 4u + u32(dm) * kMakerBytes;
}

inline void MakeFn1(C& c, Gen& g) {
    const u32 kind = c.a[0];
    c.a[6] = Place(g, kind, 0, u32(kWinA), kPedSpan);
    c.a[7] = Place(g, kind, kZoneIntel, 0x100, kIntelSpan);
    c.a[8] = Place(g, kind, kZoneList, 0x100, 4 * kMaxEvents);
    c.a[9] = Place(g, kind, 0, 0x2000 + kDmtSpan, kDmtSpan);
    i32 dm;
    {
        const u32 k = g.R(100);
        dm = k < 12 ? -2 : (k < 30 ? -1 : (k < 78 ? i32(g.R(20)) : (k < 92 ? 20 + i32(g.R(3)) : i32(g.R(20)))));
    }
    c.a[10] = u32(dm);
    c.a[11] = g.P(40) ? 2u : g.Pk({0u, 1u, 3u, 0xFFu, g.R(256)});
    u32 count;
    {
        const u32 k = g.R(100);
        if (k < 8) count = 0;
        else if (k < 16) count = u32(-int(1 + g.R(5)));
        else if (k < 18) count = 0x80000000u;
        else if (k < 66) count = 1 + g.R(8);
        else if (k < 90) count = 9 + g.R(8);
        else count = 17 + g.R(8);
    }
    c.a[12] = count;
    const i32 sc = i32(count);
    const u32 n = sc > 0 ? std::min<u32>(u32(sc), kMaxEvents) : 0;
    c.a[13] = n;
    c.w.assign(kW1, 0);
    // las filas solo se escriben con decision makers 0..19, -1 y -2 (los de indice mayor caen sobre la tabla de indices y las propias filas: se leen sin escribir)
    const bool writable = dm < 20;
    for (u32 j = 0; j < n; ++j) {
        u32* r = &c.w[6 * j];
        const u32 ek = g.R(100);
        r[0] = ek < 72 ? g.R(96) : (ek < 86 ? 96 + g.R(40) : (ek < 94 ? u32(-int(1 + g.R(20))) : g.R(96)));
        if (j > 0 && g.P(15)) r[0] = c.w[6 * (j - 1)];
        u32 idx = g.P(92) ? g.R(41) : 41 + g.R(20);
        if (dm == 19 && idx > 40) idx = 40;                  // la fila 41 del maker 19 empieza en la tabla de indices
        r[1] = idx;
        r[2] = c.a[9] + MakerBase(dm, c.a[11] & 0xFFu) + idx * kRowBytes;
        const u32 k = g.R(100);
        r[3] = !writable ? 2u : (k < 42 ? 0u : (k < 80 ? 1u : 2u));
        r[4] = g.R(6);
        u32 v = g.Pk({0u, 199u, 201u, 0xFFFFFFFFu, 1u, 0x64u, 0xC9u, g.R(400)});
        if (v == 200) v = 201;
        r[5] = v;
    }
}

inline void MakeFn2(C& c, Gen& g) {
    const u32 kind = c.a[0];
    c.a[6] = Place(g, kind, 0, u32(kWinA), kPedSpan);
    c.a[7] = Place(g, kind, 0, u32(kWinB), kPedSpan);
    c.a[8] = Place(g, kind, kZoneScanner, 0x100, 0x10);
    c.a[9] = Place(g, kind, kZoneOutA, 0x100, 4);
    c.a[10] = Place(g, kind, kZoneOutB, 0x100, 4);
    c.a[11] = Place(g, kind, kZoneIntel, 0x100, kIntelSpan);
    {
        const u32 k = g.R(100);
        c.a[12] = k < 52 ? 0xFFFFFFFFu : (k < 84 ? g.R(5) : g.Pk({5u, 6u, 0xFFFFFFFEu, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFCu, g.W()}));
    }
    {
        const u32 k = g.R(100);
        c.a[13] = k < 30 ? 0xFFFFFFFFu : (k < 82 ? g.R(5) : g.Pk({5u, 6u, 7u, 0xFFFFFFFEu, 0x80000000u, g.W()}));
    }
    c.a[14] = PedType(g, true);
    for (unsigned k = 0; k < 5; ++k) {
        const u32 m = g.R(100);
        u32 v;
        if (m < 22) v = 0;
        else if (m < 27) v = 0xFFFFFFFFu;
        else if (m < 42) v = g.W();
        else {
            v = 0;
            const u32 many = 1 + g.R(3);
            for (u32 q = 0; q < many; ++q) {
                const u32 t = g.P(60) ? c.a[14] : g.R(32);
                if (i32(t) < 32) v |= 1u << (t & 31);
            }
        }
        c.a[15 + k] = v;
    }
    c.a[20] = g.P(20) ? g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu}) : g.W();
    c.a[21] = g.P(30) ? 3000u : g.Pk({0u, 1u, 200u, 0xFFFFFFFFu, 0x80000000u, g.R(100000), g.W()});
    c.a[22] = g.P(30) ? 200u : g.Pk({0u, 1u, 3000u, 0xFFFFFFFFu, 0x80000000u, g.R(100000), g.W()});
    c.a[23] = g.W(); c.a[24] = g.W();
    c.a[25] = g.W(); c.a[26] = g.W();
    c.a[27] = (g.P(50) ? 1u : g.R(256)) | (g.R(256) << 8);
    c.w.assign(kW2, 0);
    c.w[0] = VisibilityBits(g);
    const u32 pRiot = g.Pk({10u, 40u, 70u, 100u}), p603 = g.Pk({20u, 50u, 80u}), p606 = g.Pk({20u, 50u, 80u, 100u});
    for (u32 i = 0; i < 5; ++i) c.w[1 + i] = BoolWord(g, pRiot);
    for (u32 i = 0; i < 5; ++i) c.w[6 + i] = BoolWord(g, p603);
    for (u32 i = 0; i < 5; ++i) c.w[11 + i] = BoolWord(g, p606);
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.ecx = Rand(n, 3) | 1u; c.stack = StackSeed(n);
    Gen g(n + fn * 0x10000u, 100);
    CommonHeader(c, g);
    if (fn == 0) MakeFn0(c, g);
    else if (fn == 1) MakeFn1(c, g);
    else MakeFn2(c, g);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    j.A("w", c.w);
    return j.End();
}
}

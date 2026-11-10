#pragma once
// Fixtures del banco Fn607A (CASO-607A90): 16384 casos de UNA funcion del EXE. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// 0x607A90 = CPedAcquaintanceScanner::ScanEntities (escaneo de entidades): thiscall con seis argumentos de pila, `ret 18h`:
//   ECX = el escaner; pila: ped, prioridad, CEntity** entidades, contador, int* salidaA, int* salidaB. EAX = numero de candidatos que pasan el filtro (0 si ninguno).
//   Pone *salidaA = 0; para cada entidad (indice < contador) no nula y viva (0x5E0170): delta = posicion(candidato) - posicion(ped) (x87, guardada como float), 0x59C910 la normaliza, y compara
//   (z*fz + y*fy) + x*fx (fwd = matriz del ped +0x10..+0x18, leida DESPUES de normalizar) con el float de 0xC0B034: sigue si es mayor, o si el ped o el candidato tienen el bit 0x100 de +0x46C.
//   Luego decide por prioridad / acquaintances (0x608970, 0x608830), disturbios (0x441C10) y los colaboradores sin hook 0x603AF0 / 0x6042B0 (via 0x4684F0) y apunta el candidato en una tabla local
//   de 16. Al final, por cada candidato de la tabla (mientras el acumulador sea != 4): 0x5F1C40 y 0x607560 (que escribe salidaA/salidaB y fija el temporizador del escaner).
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A: el ped; B: hasta 16 candidatos; C: escaner, instancia falsa, tabla de entidades y celdas de salida; D: matrices) con guardas de 0x40,
// y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL). Familia = n % 16: tipo = familia % 4 (colocacion de los objetos: 0 alineado, 1 desalineado,
// 2 al final de su zona, 3 al principio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F607AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;
constexpr u32 kEntryAddress = 0x607A90;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWinA = 0x1000, kWinB = 0x8000, kWinC = 0x1000, kWinD = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Zonas de la arena C (escaner +0x000, instancia falsa +0x100, tabla de entidades +0x200, celda de salidaA +0x400, de salidaB +0x500) y tamanos
constexpr u32 kZoneScanner = 0x000, kZoneInstance = 0x100, kZoneEntities = 0x200, kZoneOutA = 0x400, kZoneOutB = 0x500;
constexpr u32 kPedSpan = 0x5A0;         // bytes del objeto peaton que el banco rellena (hasta +0x59C)
constexpr u32 kCandSlot = 0x600;        // separacion de los candidatos en B
constexpr u32 kMatSlot = 0x80;          // separacion de las matrices en D
constexpr u32 kMaxSlots = 24;           // entradas de la tabla de entidades
constexpr u32 kMaxCands = 16;           // objetos candidatos
constexpr u32 kThresholdAddress = 0xC0B034;   // float: umbral del producto escalar (global del juego)

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
    std::vector<u32> w;         // datos de longitud variable (ver el generador)
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
inline float AsFloat(u32 w) { return std::bit_cast<float>(w); }
inline u32 Bits(float f) { return std::bit_cast<u32>(f); }
// Un float de 32 bits en [-1, 1] con seis decimales
inline u32 UnitComp(Gen& g) { return Bits((float(int(g.R(2000001)) - 1000000)) / 1000000.0f); }
// Un float "interesante": ceros con signo, +-1, infinitos, extremos, denormales y aleatorios (puede ser NaN solo si `nan`)
inline u32 SpecialBits(Gen& g, bool nan) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = 0x7F800001u | (g.W() & 0x007FFFFFu), w4 = 0xFF800001u | (g.W() & 0x007FFFFFu), w5 = g.W() & 0x007FFFFFu;
    for (;;) {
        const u32 v = g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FBFFFFFu, 0x00000001u, 0x007FFFFFu,
                            0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x00800000u, 0x80800000u, 0x80000001u, w1, w2, w3, w4, w5});
        if (nan || !IsNan(v)) return v;
    }
}
// Componente de un vector (adelante del ped, salida de la normalizacion): unitaria casi siempre, a veces extrema (los productos se salen del rango de float; la x87 tiene exponente ampliado)
inline u32 VecComp(Gen& g) {
    const u32 k = g.R(100);
    if (k < 62) return UnitComp(g);
    if (k < 70) return g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u, 0x7F6FFFFFu});
    if (k < 78) return g.Pk({0x00000001u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00800000u, 0x80800000u, 0x00200000u});
    if (k < 90) return SpecialBits(g, true);
    return g.W();
}
// Posicion normal (casi siempre) o extrema; nunca NaN (las NaN se reparten aparte)
inline u32 PosComp(Gen& g) {
    const u32 k = g.R(100);
    if (k < 72) return Bits((float(int(g.R(24001)) - 12000)) / 4.0f);
    if (k < 82) return g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u, 0x7F6FFFFFu, 0xFF6FFFFFu});
    if (k < 90) return g.Pk({0x00000001u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00800000u, 0x80800000u, 0u, 0x80000000u});
    if (k < 93) return g.Pk({0x7F800000u, 0xFF800000u});
    for (;;) { const u32 v = g.W(); if (!IsNan(v)) return v; }
}
inline u32 NanBits(Gen& g) {   // un NaN cualquiera (silencioso o senalizante, con signo)
    const u32 payload = g.W() & 0x007FFFFFu;
    return (g.P(50) ? 0x80000000u : 0u) | 0x7F800000u | (payload ? payload : 0x00400000u);
}
// Umbral del producto escalar
inline u32 ThresholdBits(Gen& g) {
    const u32 k = g.R(100);
    if (k < 55) return Bits((float(int(g.R(2400001)) - 1200000)) / 1000000.0f);
    if (k < 70) return g.Pk({0u, 0x80000000u, 0x3F000000u, 0xBF000000u, 0x3F800000u, 0xBF800000u, 0x3F7FFFFFu, 0xBF7FFFFFu, 0x3F7D70A4u});
    return SpecialBits(g, true);
}

// ------------------------------------------------------------------------------------------------ generador
//   a: ver el cuadro de indices abajo. w: matriz del ped (fwd, pos), matriz alternativa (fwd, pos), candidatos (10 dwords), tabla de entidades y las secuencias de los falsos.
//   a[0] peaton en A; a[1] tipo (familia % 4); a[2] relleno (familia / 4); a[3] prioridad; a[4] contador; a[5] umbral (bits); a[6] tabla de entidades en C; a[7] celda salidaA en C; a[8] celda salidaB en C;
//   a[9] escaner en C; a[10] instancia falsa de 0x4684F0 en C; a[11] valor previo de salidaA; a[12..15] semillas de A, B, C, D; a[16] banderas del ped (+0x46C); a[17] tipo del ped (+0x598);
//   a[18..22] acquaintances (+0x4E0..); a[23] candidatos nc; a[24] entradas de la tabla ns; a[25] modo de escritura lateral de la normalizacion (0 ninguno, 1 reescribe fwd.x de la matriz actual,
//   2 cambia el puntero de matriz del ped a la alternativa); a[26] numero de la llamada a la normalizacion que la hace; a[27] valor de fwd.x para el modo 1; a[28] sal de los bits altos de los bool;
//   a[29] matriz del ped en D; a[30] matriz alternativa en D; a[31] valor previo de salidaB; a[32] estado (+0x530) del ped.
//   w[0..5] fwd y pos de la matriz del ped; w[6..11] fwd y pos de la alternativa; w[12 + 10*j + 0..9] candidato j: objeto en B, pos x, y, z, tipo de posicion (0 en +4 del objeto; 1 en la matriz),
//   matriz en D, estado (+0x530), banderas (+0x46C), tipo (+0x598), reservado; despues ns dwords de la tabla (indice de candidato, nc = el propio ped, 0xFFFFFFFF = nulo), y las secuencias:
//   normalizacion (3 dwords por llamada), disturbios (1 por llamada, 2 por candidato), 0x603AF0 (idem), 0x6042B0 (1 por candidato), 0x5F1C40 (1 por entrada de la tabla), 0x607560 (3 dwords por entrada:
//   retorno, valor que escribe en salidaA, valor que escribe en salidaB). Los bool (disturbios, 0x603AF0, 0x6042B0, 0x5F1C40) llevan el valor en el bit 0 y basura en los bits 8..31.
constexpr u32 kHdr = 12;     // dwords de w antes de los candidatos
constexpr u32 kCandWords = 10;
inline u32 SeqBase(u32 nc, u32 ns) { return kHdr + kCandWords * nc + ns; }
inline u32 SeqNorm(u32 nc, u32 ns) { return SeqBase(nc, ns); }
inline u32 SeqRiot(u32 nc, u32 ns) { return SeqNorm(nc, ns) + 3 * ns; }
inline u32 Seq603(u32 nc, u32 ns) { return SeqRiot(nc, ns) + 2 * ns; }
inline u32 Seq6042(u32 nc, u32 ns) { return Seq603(nc, ns) + 2 * ns; }
inline u32 SeqCant(u32 nc, u32 ns) { return Seq6042(nc, ns) + ns; }
inline u32 Seq607(u32 nc, u32 ns) { return SeqCant(nc, ns) + ns; }
inline u32 SeqEnd(u32 nc, u32 ns) { return Seq607(nc, ns) + 3 * ns; }

inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    c.a[0] = Place(g, kind, 0, u32(kWinA), kPedSpan);
    // contador y entidades
    const bool full = g.P(7);   // escenario de tabla llena: 16 candidatos distintos, vivos, de tipo 6, prioridad -1 y umbral -4 (todos pasan el filtro)
    u32 count;
    if (full) {
        count = 16 + g.R(9);
    } else {
        const u32 k = g.R(100);
        if (k < 36) count = 16;
        else if (k < 66) count = g.R(17);
        else if (k < 76) count = 17 + g.R(8);
        else if (k < 82) count = 0;
        else if (k < 88) count = u32(-int(1 + g.R(5)));
        else if (k < 90) count = g.Pk({0x80000000u, 0xFFFFFFFFu});
        else count = 1 + g.R(3);
    }
    c.a[4] = count;
    const i32 signedCount = i32(count);
    const u32 ns = signedCount > 0 ? std::min<u32>(u32(signedCount), kMaxSlots) : 0;
    const u32 nc = full ? kMaxCands : (ns == 0 ? 0 : 1 + g.R(std::min<u32>(ns, kMaxCands)));
    c.a[23] = nc; c.a[24] = ns;
    // prioridad
    if (full) {
        c.a[3] = 0xFFFFFFFFu;
    } else {
        const u32 k = g.R(100);
        if (k < 55) c.a[3] = 0xFFFFFFFFu;
        else if (k < 85) c.a[3] = g.R(5);
        else c.a[3] = g.Pk({5u, 6u, 0xFFFFFFFEu, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFCu, g.W()});
    }
    c.a[5] = full ? 0xC0800000u : ThresholdBits(g);
    c.a[6] = Place(g, kind, kZoneEntities, 0x200, 4 * kMaxSlots);
    c.a[7] = Place(g, kind, kZoneOutA, 0x100, 4);
    c.a[8] = Place(g, kind, kZoneOutB, 0x100, 4);
    c.a[9] = Place(g, kind, kZoneScanner, 0x100, 0x10);
    c.a[10] = Place(g, kind, kZoneInstance, 0x100, 0x10);
    c.a[11] = g.W(); c.a[31] = g.W();
    // ped
    c.a[16] = (g.P(25) ? 0x100u : 0u) | (g.W() & ~0x100u) * (g.P(50) ? 1u : 0u) | (g.P(15) ? 0x100u : 0u);
    {
        const u32 k = g.R(100);
        c.a[17] = k < 20 ? 6u : (k < 90 ? g.R(32) : g.Pk({0xFFFFFFFFu, 32u, 33u, 0x80000000u, 0x7FFFFFFFu, g.W()}));
    }
    c.a[32] = g.P(15) ? g.Pk({0x36u, 0x37u}) : g.W();
    c.a[25] = g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 2u});
    c.a[26] = g.R(ns + 1);
    c.a[27] = VecComp(g);
    c.a[28] = g.W();
    c.a[29] = Place(g, kind, 0, kMatSlot, 0x3C);
    c.a[30] = kMatSlot + Place(g, kind, 0, kMatSlot, 0x3C);
    // matrices del ped y la alternativa: fwd (x, y, z) y pos (x, y, z)
    c.w.assign(SeqEnd(nc, ns), 0);
    for (unsigned i = 0; i < 12; ++i) c.w[i] = (i % 6 < 3) ? (full ? UnitComp(g) : VecComp(g)) : PosComp(g);
    // reparto de NaN en las posiciones: por componente, 0 = ninguna; 1 = los candidatos pueden llevar NaN (el ped y la alternativa no); 2 = el ped o la alternativa lleva NaN (los candidatos no)
    u32 nanMode[3];
    for (auto& m : nanMode) m = full ? 0u : g.Pk({0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 1u, 2u});
    for (unsigned comp = 0; comp < 3; ++comp) {
        if (nanMode[comp] == 2) {
            c.w[3 + comp] = NanBits(g);
            if (g.P(50)) c.w[9 + comp] = NanBits(g);
        }
    }
    // candidatos
    std::array<u32, kMaxCands> cType{};
    for (u32 j = 0; j < nc; ++j) {
        u32* r = &c.w[kHdr + kCandWords * j];
        r[0] = Place(g, kind, j * kCandSlot, kCandSlot, kPedSpan);
        for (unsigned comp = 0; comp < 3; ++comp) r[1 + comp] = (nanMode[comp] == 1 && g.P(25)) ? NanBits(g) : PosComp(g);
        if (g.P(25) && g.P(50)) {   // posiciones cercanas al ped (delta pequena)
            for (unsigned comp = 0; comp < 3; ++comp) if (nanMode[comp] != 1 && !IsNan(c.w[3 + comp]) && g.P(70)) r[1 + comp] = Bits(AsFloat(c.w[3 + comp]) + float(int(g.R(41)) - 20) * 0.25f);
        }
        r[4] = g.P(72) ? 1u : 0u;
        r[5] = (2 + j) * kMatSlot + Place(g, kind, 0, kMatSlot, 0x3C);
        r[6] = full ? g.Pk({0u, 1u, 0x35u, 0x38u, 0xFFFFFFFFu}) : (g.P(18) ? g.Pk({0x36u, 0x37u}) : (g.P(40) ? g.Pk({0u, 1u, 0x35u, 0x38u, 0xFFFFFFFFu}) : g.W()));
        r[7] = (g.P(25) ? 0x100u : 0u) | ((g.P(50) ? g.W() : 0u) & ~0x100u);
        const u32 k = g.R(100);
        r[8] = (full || k < 24) ? 6u : (k < 90 ? g.R(32) : g.Pk({0xFFFFFFFFu, 32u, 33u, 0x80000000u, 0x7FFFFFFFu, g.W()}));
        cType[j] = r[8];
    }
    // acquaintances del ped: 0, todo unos, aleatorio o la union de las banderas de algunos tipos de los candidatos (para que acierten)
    for (unsigned k = 0; k < 5; ++k) {
        const u32 m = g.R(100);
        u32 v;
        if (m < 25) v = 0;
        else if (m < 30) v = 0xFFFFFFFFu;
        else if (m < 45) v = g.W();
        else {
            v = 0;
            const u32 many = 1 + g.R(3);
            for (u32 q = 0; q < many; ++q) {
                const u32 t = nc ? cType[g.R(nc)] : 0u;
                if (i32(t) < 32) v |= 1u << (t & 31);
            }
        }
        c.a[18 + k] = v;
    }
    // tabla de entidades
    for (u32 i = 0; i < ns; ++i) {
        u32 e;
        const u32 k = g.R(100);
        if (i >= 16) e = 0xFFFFFFFFu;                       // mas de 16 entradas: las sobrantes son nulas (la tabla local del EXE guarda 16)
        else if (full) e = i;
        else if (k < 12) e = 0xFFFFFFFFu;
        else if (k < 16) e = nc;                            // el propio ped
        else e = g.R(nc);
        c.w[kHdr + kCandWords * nc + i] = e;
    }
    // secuencias
    auto boolWord = [&](u32 pct) { return (g.P(pct) ? 1u : 0u) | (g.P(50) ? (g.W() & 0xFFFFFF00u) : 0u); };
    for (u32 i = 0; i < ns; ++i) {
        u32* p = &c.w[SeqNorm(nc, ns) + 3 * i];
        const bool special = g.P(25);
        for (unsigned comp = 0; comp < 3; ++comp) p[comp] = (special && !full) ? VecComp(g) : UnitComp(g);
    }
    const u32 pRiot = g.Pk({10u, 40u, 70u, 100u}), p603 = g.Pk({20u, 50u, 80u}), p6042 = g.Pk({20u, 50u, 80u}), pCant = g.Pk({30u, 70u, 100u});
    for (u32 i = 0; i < 2 * ns; ++i) c.w[SeqRiot(nc, ns) + i] = boolWord(pRiot);
    for (u32 i = 0; i < 2 * ns; ++i) c.w[Seq603(nc, ns) + i] = boolWord(p603);
    for (u32 i = 0; i < ns; ++i) c.w[Seq6042(nc, ns) + i] = boolWord(p6042);
    for (u32 i = 0; i < ns; ++i) c.w[SeqCant(nc, ns) + i] = boolWord(pCant);
    for (u32 i = 0; i < ns; ++i) {
        u32* p = &c.w[Seq607(nc, ns) + 3 * i];
        const u32 k = g.R(100);
        p[0] = k < 22 ? 4u : (k < 45 ? 0xFFFFFFFFu : (k < 85 ? g.R(5) : g.W()));
        p[1] = g.W(); p[2] = g.W();
    }
    // escenario de empate: el vector adelante del ped (y el de la matriz alternativa) es un eje unitario y la normalizacion devuelve siempre el mismo valor en ese eje; el umbral es ese valor, de modo que el producto
    // escalar es EXACTAMENTE igual al umbral (distingue `>` de `>=`); sin la escritura lateral que reescribe fwd.x
    if (!full && g.P(8)) {
        const u32 axis = g.R(3);
        const u32 t = g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, UnitComp(g), UnitComp(g), UnitComp(g)});
        for (unsigned base : {0u, 6u})
            for (unsigned comp = 0; comp < 3; ++comp) c.w[base + comp] = comp == axis ? 0x3F800000u : (g.P(50) ? 0u : 0x80000000u);
        c.a[5] = t;
        if (c.a[25] == 1) c.a[25] = 0;
        for (u32 i = 0; i < ns; ++i) {
            u32* p = &c.w[SeqNorm(nc, ns) + 3 * i];
            for (unsigned comp = 0; comp < 3; ++comp) p[comp] = comp == axis ? t : UnitComp(g);
        }
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
    j.A("w", c.w);
    return j.End();
}
}

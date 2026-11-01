#pragma once
// Fixtures del banco Car84A (lote 84A): CCarCtrl::PickNextNodeRandomly (EXE 0x42DE80), 18432 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::PickNextNodeRandomly  0x42DE80  cdecl, un argumento (vehiculo), `ret` simple, void   (hook CCarCtrl / PickNextNodeRandomly, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) y las familias 16 y 17 (1024 casos cada una).
// Mundo de caminos de juguete: 3 ranuras de area (id de area distinto en 0..63, cargada o no), 4 nodos por area, 16 enlaces de nodo y de navegacion por area y 8 enlaces de carretera (14 bytes) por area.
// Regla de seguridad: el desplazamiento de la secuencia (lo fija el falso de InitSequence) es siempre >= 0, asi que los indices son >= 0 y siempre caen dentro de las tablas de la ranura.
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

namespace C84AFix {
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
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kNodeArena = Arena(kSlots * kNodesPer * kNodeBytes), kNlArena = Arena(kSlots * kLinksPer * 4), kNaviArena = Arena(kSlots * kNaviPer * 2),
                      kNnArena = Arena(kSlots * kNavNodesPer * kLinkBytes);

constexpr u32 kTimeAddr = 0xB7CB84;       // CTimer::m_snTimeInMilliseconds

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 84001u); }
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

// Valor flotante de la mutacion 9 (el oraculo repite la tabla): tabla de especiales o (v % 2001 - 1000) / 8
inline u32 MutFloat(u32 v) {
    static constexpr std::array<u32, 8> t{0u, 0x80000000u, 1u, 0x7F800000u, 0xFF800000u, 0xFFC00000u, 0x7F7FFFFFu, 0x3F800000u};
    return (v & 3u) == 0 ? t[(v >> 2) % 8u] : B(float(int(v % 2001u) - 1000) / 8.0f);
}

// ------------------------------------------------------------------------------------------------ el caso
constexpr u32 kNoMutation = 99;
constexpr u32 kKinds = 20;
constexpr u32 kSeq = 16;       // longitud de las secuencias de retornos de los falsos (el indice se satura en el ultimo)
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 now{}, rseed{};
    u32 seqOff{}, seqOther{}, st[3]{};
    // mundo de caminos
    u32 areas[kSlots]{}, loaded[kSlots]{};
    u32 node[kSlots * kNodesPer][3]{};     // [x | y << 16, base | b18 << 16 | b19 << 24, b1A]
    u32 nlink[kSlots][kLinksPer]{};         // direcciones de nodo (area | nodo << 16)
    u32 navi[kSlots][kNaviPer]{};           // direcciones de enlace de carretera (id | area << 10)
    u32 lk[kSlots * kNavNodesPer][3]{};     // [x | y << 16, otroArea | otroNodo << 16, dx | dy << 8 | ancho << 16 | banderas << 24]
    // vehiculo y piloto
    u32 seed16{}, cur{}, start{}, end0{}, fieldC{}, scale{}, curLink{}, nextLink{}, prevLink{}, t3ac{}, t3b0{};
    u32 sm4{}, sm5{}, sm6{}, lane7{}, lane8{}, style{}, mission{}, tempact{}, tempTime{}, speed{}, f3db{}, f50{}, s36{}, b8{}, created{};
    // retornos crudos de los falsos
    u32 rtnt{}, fpd[kSeq]{}, fpdOut[kSeq]{}, ian[kSeq]{}, csf{};
    u32 mutAt{}, mutKind{}, mutVal{};
    u32 gate{};
};

inline u32 StatusOf(const C& c) { return (c.s36 & 0xFFu) >> 3; }

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
    auto Ret = [&](bool b) {   // retorno crudo de un bool: AL decide; los bits altos son basura (la DLL no puede probar EAX completo)
        u32 hi = Rand(n, nk()) & 0xFFFFFF00u;
        if (Rand(n, nk()) % 3 == 0) hi = 0;
        u32 lo = b ? (Rand(n, nk()) % 2 ? 1u : 1u + Rand(n, nk()) % 255u) : 0u;
        return hi | lo;
    };
    auto Coord = [&]() -> u32 {   // int16 como u16
        const u32 w = R(100);
        if (w < 3) return 0x7FFFu;
        if (w < 6) return 0x8000u;
        if (w < 9) return 0u;
        return u32(i16(int(R(12001)) - 6000)) & 0xFFFFu;
    };

    // ---- reloj, rand() del EXE y secuencia
    {
        const u32 w = R(8);
        c.now = w == 0 ? 0u : w == 1 ? 0xFFFFFFFFu : w == 2 ? 0xFFFFF000u : 1000u + R(2000000u);
    }
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
    c.start = Addr(true);
    c.end0 = Rand(n, nk());
    c.nextLink = LinkWord(true);
    c.curLink = R(65536); c.prevLink = R(65536);
    c.fieldC = P(15) ? Rand(n, nk()) : R(100000); c.scale = P(10) ? 0xFFFFFFFFu : R(6000);
    c.t3ac = Rand(n, nk()); c.t3b0 = Rand(n, nk());
    c.sm4 = R(256); c.sm5 = P(80) ? (R(2) ? 1u : 0xFFu) : R(256); c.sm6 = P(80) ? (R(2) ? 1u : 0xFFu) : R(256);
    c.lane7 = P(70) ? R(5) : R(256); c.lane8 = P(70) ? R(5) : R(256);
    c.style = R(256); c.mission = R(256); c.tempact = R(256); c.tempTime = Rand(n, nk());
    c.speed = P(78) ? B(5.0f + float(R(116)) + float(R(8)) / 8.0f) : Special(n, nk());
    c.f3db = P(25) ? 0x08u : P(25) ? 0x10u : R(256);
    c.f50 = P(30) ? 1u : R(256);
    c.s36 = P(60) ? ((2u << 3) | R(8)) : P(35) ? ((3u << 3) | R(8)) : R(256);
    c.b8 = R(256);
    c.created = P(30) ? 2u : R(256);
    // ---- nodos
    auto KindByte = [&]() -> u32 {
        const u32 w = R(100);
        const u32 kind = w < 20 ? 0u : w < 32 ? 1u : w < 44 ? 2u : w < 56 ? 9u : w < 68 ? 10u : R(16);
        return (kind << 4) | R(16);
    };
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
        const u32 w = R(10);
        auto Dir = [&]() -> u32 { const u32 d = R(10); return d == 0 ? 0u : d == 1 ? 100u : d == 2 ? u32(u8(-100)) : d == 3 ? 127u : d == 4 ? 0x80u : u32(u8(int(R(201)) - 100)); };
        (void)w;
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
    // ---- retornos de los falsos
    c.rtnt = Ret(P(15));
    for (u32 k = 0; k < kSeq; ++k) {
        const u32 m = R(8);
        c.fpd[k] = (Rand(n, nk()) & 0xFFFFFF00u) | (P(60) ? (1u << R(3)) : m);
        if (Rand(n, nk()) % 3 == 0) c.fpd[k] &= 0xFFu;
        c.fpdOut[k] = P(25) ? 1u : 0u;
        c.ian[k] = Ret(P(55));
    }
    c.csf = P(85) ? B(0.5f + float(R(4000)) / 10.0f) : Special(n, nk());
    c.seqOff = P(70) ? R(8) : R(0x7FFFFF00u);
    c.seqOther = P(50) ? R(2) : P(30) ? R(256) : 0u;
    c.mutAt = P(30) ? R(16) : kNoMutation;
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
    auto Passable = [&] {   // enlaces con carriles de los dos lados y sin bandera de un solo sentido
        for (u32 i = 0; i < kSlots * kNavNodesPer; ++i) c.lk[i][2] = (c.lk[i][2] & 0x00FFFFFFu) | (((1u + R(7)) | ((1u + R(7)) << 3)) << 24);
    };
    auto AllLinksValid = [&] {   // todos los candidatos y enlaces con el area cargada
        for (u32 s = 0; s < kSlots; ++s) {
            for (u32 k = 0; k < kLinksPer; ++k) c.nlink[s][k] = c.areas[R(kSlots)] | (R(kNodesPer) << 16);
            for (u32 k = 0; k < kNaviPer; ++k) c.navi[s][k] = R(kNavNodesPer) | (c.areas[R(kSlots)] << 10);
        }
    };
    auto NoBit20 = [&](bool value) { for (u32 i = 0; i < kSlots * kNodesPer; ++i) { u32 b18 = (c.node[i][1] >> 16) & 0xFFu; b18 = value ? (b18 | 0x20u) : (b18 & ~0x20u); c.node[i][1] = (c.node[i][1] & 0xFF00FFFFu) | (b18 << 16); } };
    auto Count = [&](u32 lo, u32 hi) { for (u32 i = 0; i < kSlots * kNodesPer; ++i) { u32 b18 = (c.node[i][1] >> 16) & 0xF0u; b18 |= lo + R(hi - lo + 1); c.node[i][1] = (c.node[i][1] & 0xFF00FFFFu) | (b18 << 16); } };
    auto Kind = [&](u32 kind) { for (u32 i = 0; i < kSlots * kNodesPer; ++i) c.node[i][2] = (kind << 4) | R(16); };
    switch (f) {
    case 0: break;
    case 1:   // primer barrido: todo pasa
        ValidEntry(); AllLinksValid(); Passable(); Count(1, 8);
        for (u32 k = 0; k < kSeq; ++k) { c.ian[k] = Ret(true); c.fpd[k] = (c.fpd[k] & 0xFFFFFF00u) | 7u; c.fpdOut[k] = 0; }
        c.s36 = (R(2) ? 2u : 3u) << 3 | R(8);
        break;
    case 2:   // el primer barrido no encuentra: IsThisAnAppropriateNode siempre falso o direccion sin mascara -> segundo barrido
        ValidEntry(); AllLinksValid(); Passable(); Count(1, 8);
        if (R(2)) for (u32 k = 0; k < kSeq; ++k) c.ian[k] = Ret(false);
        else for (u32 k = 0; k < kSeq; ++k) { c.ian[k] = Ret(true); c.fpd[k] &= 0xFFFFFF00u; }
        NoBit20(R(2) != 0);
        break;
    case 3:   // el segundo barrido tampoco (bit 0x20 en candidato y anterior): tercer barrido
        ValidEntry(); AllLinksValid(); Passable(); Count(1, 8);
        for (u32 k = 0; k < kSeq; ++k) c.ian[k] = Ret(false);
        NoBit20(true);
        break;
    case 4:   // ninguno sirve: candidatos iguales al anterior, no cargados, malos
        ValidEntry(); Count(1, 8);
        for (u32 k = 0; k < kSeq; ++k) c.ian[k] = Ret(P(30));
        switch (R(4)) {
        case 0: for (u32 s = 0; s < kSlots; ++s) for (u32 k = 0; k < kLinksPer; ++k) c.nlink[s][k] = c.cur; break;
        case 1: for (u32 s = 0; s < kSlots; ++s) for (u32 k = 0; k < kLinksPer; ++k) c.nlink[s][k] = Unloaded(); break;
        case 2: AllLinksValid(); for (u32 i = 0; i < kSlots * kNavNodesPer; ++i) c.lk[i][2] &= 0x00FFFFFFu; break;
        default: AllLinksValid(); for (u32 s = 0; s < kSlots; ++s) for (u32 k = 0; k < kNaviPer; ++k) c.navi[s][k] = R(kNavNodesPer) | (Unloaded() << 10); break;
        }
        break;
    case 5:   // tipos del nodo elegido 1, 2 y 10 (y los demas)
        ValidEntry(); AllLinksValid(); Passable(); Count(1, 8);
        Kind(R(3) == 0 ? 10u : R(2) ? 1u : 2u);
        for (u32 k = 0; k < kSeq; ++k) { c.ian[k] = Ret(true); c.fpd[k] = (c.fpd[k] & 0xFFFFFF00u) | 7u; c.fpdOut[k] = 0; }
        if (P(40)) c.s36 = (2u << 3) | R(8);
        break;
    case 6:   // tipo 9 del nodo actual (el siguiente de la entrada) y vehiculo de mision o no
        ValidEntry(); AllLinksValid(); Passable(); Count(1, 8);
        for (u32 i = 0; i < kSlots * kNodesPer; ++i) c.node[i][2] = ((P(70) ? 9u : R(16)) << 4) | R(16);
        c.created = P(40) ? 2u : R(256);
        if (P(60)) c.s36 = (2u << 3) | R(8);
        for (u32 k = 0; k < kSeq; ++k) { c.ian[k] = Ret(P(80)); c.fpd[k] = (c.fpd[k] & 0xFFFFFF00u) | 7u; }
        break;
    case 7:   // parte x87 (estado 2): enlaces, carriles y anchos variados
        ValidEntry(); AllLinksValid(); Count(1, 8);
        c.s36 = (2u << 3) | R(8);
        for (u32 k = 0; k < kSeq; ++k) { c.ian[k] = Ret(P(80)); c.fpd[k] = (c.fpd[k] & 0xFFFFFF00u) | 7u; c.fpdOut[k] = 0; }
        c.lane7 = R(6); c.lane8 = R(6); c.f3db &= ~0x18u;
        c.speed = B(1.0f + float(R(200)));
        break;
    case 8:   // sin estado 2 (vuelve antes de la parte x87), entradas descargadas
        ValidEntry(); AllLinksValid();
        c.s36 = ((P(50) ? 3u : R(32)) << 3) | R(8);
        if (c.s36 >> 3 == 2) c.s36 = (3u << 3) | (c.s36 & 7u);
        if (P(25)) { u32 s = R(kSlots); c.loaded[s] = 0; }
        if (P(25)) c.cur = Unloaded();
        if (P(15)) c.start = Unloaded();
        if (P(15)) c.nextLink = R(kNavNodesPer) | (Unloaded() << 10);
        break;
    case 9:   // velocidad y factor con especiales
        ValidEntry(); AllLinksValid(); Passable(); Count(1, 8);
        c.s36 = (2u << 3) | R(8);
        for (u32 k = 0; k < kSeq; ++k) { c.ian[k] = Ret(true); c.fpd[k] = (c.fpd[k] & 0xFFFFFF00u) | 7u; c.fpdOut[k] = 0; }
        c.speed = P(60) ? Special(n, nk()) : B(float(R(9)) * 0.001f);
        c.csf = P(60) ? Special(n, nk()) : B(float(R(1000000)) * 0.01f);
        break;
    case 10: case 11: case 12:   // barrido de mutaciones: cada evento x cada tipo en casos que recorren varias etapas
        ValidEntry(); AllLinksValid(); Count(2, 8);
        c.s36 = (2u << 3) | R(8);
        for (u32 k = 0; k < kSeq; ++k) { c.ian[k] = Ret(P(60)); c.fpd[k] = (c.fpd[k] & 0xFFFFFF00u) | (P(70) ? 7u : 0u); c.fpdOut[k] = P(10) ? 1u : 0u; }
        if (P(50)) Passable();
        if (f == 10) { c.mutAt = (n / 16) % 12; c.mutKind = 1u + (n / 16 / 12) % 10u; }
        else if (f == 11) { c.mutAt = (n / 16) % 12; c.mutKind = 11u + (n / 16 / 12) % 10u; }
        else { c.mutAt = R(14); c.mutKind = 1u + R(kKinds); }
        break;
    case 13:  // cambio de carril al azar: contador a 1, mucha distancia, secuencias de rand() del EXE
        ValidEntry(); AllLinksValid(); Passable(); Count(1, 8);
        c.s36 = (R(4) == 0 ? 3u : 2u) << 3 | R(8);
        c.f50 = P(70) ? 1u : R(4); c.f3db &= ~0x18u;
        for (u32 k = 0; k < kSeq; ++k) { c.ian[k] = Ret(true); c.fpd[k] = (c.fpd[k] & 0xFFFFFF00u) | 7u; c.fpdOut[k] = 0; }
        c.lane8 = R(5);
        if (P(30)) for (u32 i = 0; i < kSlots * kNodesPer; ++i) c.node[i][0] = (c.node[i][0] & 0xFFFF0000u) | (u32(i16(int(R(40)) - 20)) & 0xFFFFu);
        break;
    case 14: case 15: break;   // todo aleatorio
    case 16:  // especiales de la velocidad y del factor, fieldC cerca del desbordamiento, escala maxima
        ValidEntry(); AllLinksValid(); Passable(); Count(1, 8);
        c.s36 = (2u << 3) | R(8);
        c.speed = Special(n, nk()); c.csf = Special(n, nk());
        c.fieldC = P(50) ? 0x7FFFFFF0u + R(32) : Rand(n, nk()); c.scale = P(50) ? 0xFFFFFFFFu - R(16) : Rand(n, nk());
        for (u32 k = 0; k < kSeq; ++k) { c.ian[k] = Ret(P(85)); c.fpd[k] = (c.fpd[k] & 0xFFFFFF00u) | 7u; c.fpdOut[k] = 0; }
        break;
    default:  // 17: hasta 15 enlaces por nodo y desplazamientos de secuencia grandes
        ValidEntry(); AllLinksValid(); Count(1, 15);
        c.seqOff = P(60) ? R(0x7FFFFF00u) : R(16); c.seqOther = R(2);
        for (u32 k = 0; k < kSeq; ++k) { c.ian[k] = Ret(P(35)); c.fpd[k] = (c.fpd[k] & 0xFFFFFF00u) | (P(80) ? 7u : 0u); c.fpdOut[k] = P(15) ? 1u : 0u; }
        c.s36 = (P(70) ? 2u : 3u) << 3 | R(8);
        if (P(50)) NoBit20(false);
        break;
    }
    // ---- reglas de seguridad: ningun nodo tiene mas enlaces de los que caben tras su base (base + n <= 16), y siempre hay una ranura cargada
    for (u32 i = 0; i < kSlots * kNodesPer; ++i) {
        const u32 base = c.node[i][1] & 0xFFFFu;
        u32 b18 = (c.node[i][1] >> 16) & 0xFFu;
        if ((b18 & 0xFu) > 16u - base) b18 = (b18 & 0xF0u) | (16u - base);
        c.node[i][1] = (c.node[i][1] & 0xFF00FFFFu) | (b18 << 16);
    }
    // ---- reglas de seguridad: siempre hay una ranura cargada con el actual
    {
        bool any = false;
        for (u32 s = 0; s < kSlots; ++s) any |= c.loaded[s] != 0;
        if (!any) c.loaded[R(kSlots)] = 1;
    }
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack).U("now", c.now).U("rseed", c.rseed);
    j.U("seq_off", c.seqOff).U("seq_other", c.seqOther).A("st", std::vector<u32>(c.st, c.st + 3));
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
    j.U("t3ac", c.t3ac).U("t3b0", c.t3b0).U("sm4", c.sm4).U("sm5", c.sm5).U("sm6", c.sm6).U("lane7", c.lane7).U("lane8", c.lane8).U("style", c.style).U("mission", c.mission).U("tempact", c.tempact);
    j.U("temp_time", c.tempTime).U("speed", c.speed).U("f3db", c.f3db).U("f50", c.f50).U("s36", c.s36).U("b8", c.b8).U("created", c.created);
    j.U("rtnt", c.rtnt).A("fpd", std::vector<u32>(c.fpd, c.fpd + kSeq)).A("fpd_out", std::vector<u32>(c.fpdOut, c.fpdOut + kSeq)).A("ian", std::vector<u32>(c.ian, c.ian + kSeq)).U("csf", c.csf);
    j.U("mut_at", c.mutAt).U("mut_kind", c.mutKind).U("mut_val", c.mutVal).U("gate", c.gate);
    return j.End();
}
}

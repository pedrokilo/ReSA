#pragma once
// Fixtures del banco Car84F (lote 84F): CCarCtrl::GenerateOneEmergencyServicesCar (EXE 0x42B7D0), 18432 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::GenerateOneEmergencyServicesCar  0x42B7D0  cdecl (modelo, posicion x, y, z), `ret` del llamador, devuelve el vehiculo en EAX   (hook CCarCtrl / GenerateOneEmergencyServicesCar, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) y las familias 16 y 17 (1024 casos cada una).
// Mundo de caminos de juguete: 3 ranuras de area (id de area distinto en 0..71, siempre cargadas) con 4 nodos cada una (altura +0xC y banderas +0x18 de los nodos son entradas del caso).
// Los falsos de GenerateCarCreationCoors2 (hasta 5 llamadas), FindObjectsKindaColliding, ProcessVerticalLine (2 llamadas) y GetDistanceFromCentreOfMassToBaseOfModel devuelven lo que dice el caso.
// Reglas de seguridad (el EXE no comprueba nada de esto): las direcciones de nodo que devuelve un falso con resultado verdadero son de una ranura (area) y un nodo < 4; la reserva de
// operator new nunca es nula; el vehiculo creado siempre tiene matriz.
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

namespace C84FFix {
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

constexpr u32 kSlots = 3, kNodesPer = 4, kNodeBytes = 0x1C, kCalls = 5;
// Arenas: guarda + ventana + guarda
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x988;      // CAutomobile completo
constexpr std::size_t kMatWin = 0x48;       // CMatrix
constexpr std::size_t kPlayerWin = 0x10;    // vector del jugador
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kNodeArena = Arena(kSlots * kNodesPer * kNodeBytes), kVehArena = Arena(kVehWin), kMatArena = Arena(kMatWin), kPlayerArena = Arena(kPlayerWin);

constexpr u32 kPifAddr = 0xB7CD74;        // CWorld::PlayerInFocus (byte)
constexpr u32 kAmbAddr = 0x9690A8;        // NumAmbulancesOnDuty
constexpr u32 kFireAddr = 0x9690AC;       // NumFireTrucksOnDuty

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 84006u); }
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

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 model{}, posn[3]{}, pif{}, player[3]{};
    u32 amb{}, fire{};                       // contadores iniciales
    // llamadas a GenerateCarCreationCoors2: resultado, punto de salida, nodo 1 y 2 como (area | nodo << 16), fraccion
    u32 gret[kCalls]{}, gorg[kCalls][3]{}, gn1[kCalls]{}, gn2[kCalls]{}, gfrac[kCalls]{};
    u32 cnt[kCalls]{};                       // contador de FindObjectsKindaColliding (int16)
    u32 phit[2]{}, pz[2]{}, pxy[2][2]{};     // ProcessVerticalLine: golpe, z, x e y que escribe
    u32 dist{};                              // GetDistanceFromCentreOfMassToBaseOfModel
    u32 areas[kSlots]{};
    u32 nodeZ[kSlots * kNodesPer]{}, nodeFlag[kSlots * kNodesPer]{};
    u32 gate{};
};

inline constexpr std::array<u32, 8> kHandled{407u, 416u, 427u, 523u, 596u, 597u, 598u, 599u};

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
    auto World = [&](bool allowSpecial) -> u32 {
        if (allowSpecial && P(6)) return Special(n, nk());
        return B(float(R(6001)) / 10.0f - 300.0f);
    };
    // areas distintas en 0..71
    c.areas[0] = R(72);
    do { c.areas[1] = R(72); } while (c.areas[1] == c.areas[0]);
    do { c.areas[2] = R(72); } while (c.areas[2] == c.areas[0] || c.areas[2] == c.areas[1]);
    auto ValidAddr = [&]() -> u32 { return c.areas[R(kSlots)] | (R(kNodesPer) << 16); };
    auto Junk = [&]() -> u32 { return Rand(n, nk()); };
    c.model = P(75) ? kHandled[R(8)] : (P(50) ? Pick(std::array<u32, 12>{0u, 1u, 406u, 408u, 415u, 417u, 426u, 428u, 490u, 522u, 595u, 600u}, n, nk()) : (P(50) ? Rand(n, nk()) : 380u + R(240)));
    for (u32 k = 0; k < 3; ++k) { c.posn[k] = World(true); c.player[k] = World(true); }
    c.pif = R(256);
    c.amb = P(20) ? 0xFFFFFFFFu : Rand(n, nk()) % 100u;
    c.fire = P(20) ? 0xFFFFFFFFu : Rand(n, nk()) % 100u;
    for (u32 k = 0; k < kCalls; ++k) {
        c.gret[k] = R(2);
        for (u32 j = 0; j < 3; ++j) c.gorg[k][j] = World(true);
        c.gn1[k] = c.gret[k] ? ValidAddr() : (P(50) ? ValidAddr() : Junk());
        c.gn2[k] = c.gret[k] ? ValidAddr() : (P(50) ? ValidAddr() : Junk());
        c.gfrac[k] = P(75) ? B(float(R(1001)) / 1000.0f) : (P(50) ? B(float(int(R(401)) - 200) / 100.0f) : Special(n, nk()));
        c.cnt[k] = P(60) ? 0u : (P(40) ? 1u + R(3) : (P(50) ? 0x100u : (P(50) ? 0xFFFFu : 0x8000u)));
    }
    for (u32 k = 0; k < 2; ++k) {
        c.phit[k] = P(70) ? 1u : 0u;
        c.pz[k] = P(80) ? B(float(int(R(8001)) - 2000) / 8.0f) : (P(30) ? B(1.0e9f) : Special(n, nk()));
        c.pxy[k][0] = Junk(); c.pxy[k][1] = Junk();
    }
    c.dist = P(80) ? B(float(R(401)) / 100.0f) : Special(n, nk());
    for (u32 i = 0; i < kSlots * kNodesPer; ++i) {
        c.nodeZ[i] = P(6) ? (P(50) ? 0x7FFFu : 0x8000u) : (u32(i16(int(R(2001)) - 1000)) & 0xFFFFu);
        c.nodeFlag[i] = P(70) ? R(0x80) : R(256);   // bit 7: nodo de agua
    }

    // ---- sesgos por familia
    auto NodeIdx = [&](u32 addr) -> u32 {   // indice plano (ranura * 4 + nodo) de una direccion (area | nodo << 16) valida
        for (u32 sl = 0; sl < kSlots; ++sl) if (c.areas[sl] == (addr & 0xFFFFu)) return sl * kNodesPer + ((addr >> 16) & 0xFFFFu);
        return 0u;
    };
    auto Good = [&](u32 k) {   // intento k: acierta (resultado verdadero, nodo sin agua, nada encima)
        c.gret[k] = 1; c.cnt[k] = 0;
        c.gn1[k] = ValidAddr();
        c.nodeFlag[NodeIdx(c.gn1[k])] &= 0x7Fu;
    };
    auto HitBoth = [&] { c.phit[0] = 1; c.phit[1] = 1; };
    auto NiceWorld = [&] {
        for (u32 k = 0; k < 3; ++k) c.posn[k] = B(float(R(6001)) / 10.0f - 300.0f);
        for (u32 k = 0; k < kCalls; ++k) for (u32 j = 0; j < 3; ++j) c.gorg[k][j] = B(float(R(6001)) / 10.0f - 300.0f);
        for (u32 k = 0; k < kCalls; ++k) c.gfrac[k] = B(float(R(1001)) / 1000.0f);
        for (u32 k = 0; k < 2; ++k) c.pz[k] = B(float(int(R(8001)) - 2000) / 8.0f);
        c.dist = B(float(R(401)) / 100.0f);
    };
    const u32 first = R(kCalls);   // intento que acierta en las familias de busqueda
    switch (f) {
    case 0: break;
    case 1: case 2: case 3:   // acierta en el primer intento, modelos de bomberos / ambulancia / policia, todo normal
        NiceWorld(); Good(0); HitBoth();
        c.model = f == 1 ? 407u : f == 2 ? 416u : kHandled[2 + R(6)];
        break;
    case 4:   // modelos sin ocupantes
        NiceWorld(); Good(0); HitBoth();
        c.model = P(50) ? Pick(std::array<u32, 8>{0u, 406u, 408u, 426u, 490u, 522u, 600u, 0xFFFFFFFFu}, n, nk()) : (380u + R(240));
        break;
    case 5:   // acierta en el intento `first` (los anteriores fallan de varias formas)
        NiceWorld();
        for (u32 k = 0; k < first; ++k) {
            const u32 w = R(3);
            c.gret[k] = w == 0 ? 0u : 1u;
            c.cnt[k] = 0;
            c.gn1[k] = ValidAddr(); c.gn2[k] = ValidAddr();
            if (w == 1) c.nodeFlag[NodeIdx(c.gn1[k])] |= 0x80u;
            if (w == 2) c.cnt[k] = 1u + R(3);
        }
        Good(first); HitBoth();
        break;
    case 6:   // acierta justo en el quinto intento (cuenta como fallo) o en el cuarto
        NiceWorld();
        for (u32 k = 0; k < 4; ++k) c.gret[k] = 0;
        if (P(50)) { Good(4); } else { Good(3); c.gret[4] = 0; }
        HitBoth();
        break;
    case 7:   // todos los intentos fallan
        NiceWorld();
        for (u32 k = 0; k < kCalls; ++k) { if (P(50)) c.gret[k] = 0; else c.cnt[k] = 1u + R(3); }
        for (u32 k = 0; k < kCalls; ++k) if (c.gret[k] && c.cnt[k] == 0) c.cnt[k] = 1u;
        break;
    case 8:   // nodos de agua y bandera 0x80 en distintos nodos
        NiceWorld(); Good(0); HitBoth();
        for (u32 i = 0; i < kSlots * kNodesPer; ++i) c.nodeFlag[i] = P(50) ? (c.nodeFlag[i] | 0x80u) : (c.nodeFlag[i] & 0x7Fu);
        break;
    case 9:   // contadores int16 distintos de cero con byte bajo cero
        NiceWorld(); Good(0); HitBoth();
        c.cnt[0] = Pick(std::array<u32, 6>{0u, 0x100u, 0x8000u, 0xFFFFu, 0x10000u, 0x20000u}, n, nk());
        if (P(50)) { Good(1); HitBoth(); }
        break;
    case 10:  // sin suelo: ningun golpe, o un golpe con z exactamente 1e9
        NiceWorld(); Good(0);
        c.phit[0] = P(60) ? 0u : 1u; c.phit[1] = P(60) ? 0u : 1u;
        if (c.phit[0]) c.pz[0] = P(70) ? B(1.0e9f) : c.pz[0];
        if (c.phit[1]) c.pz[1] = P(70) ? B(1.0e9f) : c.pz[1];
        break;
    case 11:  // valores especiales en el suelo
        NiceWorld(); Good(0); HitBoth();
        if (P(70)) c.pz[0] = Special(n, nk());
        if (P(70)) c.pz[1] = Special(n, nk());
        if (P(30)) c.phit[0] = 0;
        break;
    case 12:  // empates tras el redondeo a 24 bits de la comparacion de distancias al suelo: z del punto = z' (z1' = z2' = nodeZ / 8, t = 0) con nodeZ impar, golpes de ~2^21 que difieren en 2 unidades de 1/8
        NiceWorld(); Good(0); HitBoth();
        {
            const i32 nz = 1 + 2 * i32(R(4000));
            c.gn1[0] = c.areas[0]; c.gn2[0] = c.areas[0];
            c.nodeZ[0] = u32(nz) & 0xFFFFu; c.nodeFlag[0] &= 0x7Fu;
            c.gfrac[0] = 0u;
            long long xu = (1ll << 24) + 2ll * R(1u << 23);
            if (P(50)) xu += 2;
            const long long x2u = xu - 2ll * nz + (P(50) ? 2ll : -2ll);
            c.pz[0] = B(float(xu) * 0.125f);
            c.pz[1] = B(-float(x2u) * 0.125f);
            if (P(25)) { c.pz[0] = B(-float(xu) * 0.125f); c.pz[1] = B(float(x2u) * 0.125f); }
        }
        break;
    case 13:  // direccion: posicion igual al punto, diferencias minimas o enormes, NaN
        NiceWorld(); Good(0); HitBoth();
        {
            const u32 w = R(8);
            const float ox = F(c.gorg[0][0]), oy = F(c.gorg[0][1]);
            if (w == 0) { c.posn[0] = c.gorg[0][0]; c.posn[1] = c.gorg[0][1]; }
            else if (w == 1) { c.posn[0] = B(ox + 1.0e-20f); c.posn[1] = c.gorg[0][1]; }
            else if (w == 2) { c.posn[0] = c.gorg[0][0]; c.posn[1] = B(oy + 1.0e-30f); }
            else if (w == 3) { c.posn[0] = B(ox + 3.0e19f); c.posn[1] = B(oy - 2.0e19f); }
            else if (w == 4) { c.posn[0] = Special(n, nk()); }
            else if (w == 5) { c.posn[1] = Special(n, nk()); c.posn[0] = c.gorg[0][0]; }
            else if (w == 6) { c.posn[0] = B(ox + 1.0e-40f); c.posn[1] = B(oy); }
            else { c.posn[0] = B(ox + 1.0e30f); c.posn[1] = B(oy + 1.0e30f); }
        }
        break;
    case 14:  // interpolacion de la altura: fracciones especiales, nodos con alturas extremas
        NiceWorld(); Good(0); HitBoth();
        c.gfrac[0] = P(50) ? Special(n, nk()) : B(float(int(R(401)) - 200) / 50.0f);
        for (u32 i = 0; i < kSlots * kNodesPer; ++i) if (P(40)) c.nodeZ[i] = P(50) ? 0x7FFFu : 0x8000u;
        break;
    case 15:  // distancia al centro de masa especial
        NiceWorld(); Good(0); HitBoth();
        c.dist = P(70) ? Special(n, nk()) : B(float(int(R(201)) - 100) / 4.0f);
        if (P(30)) c.pz[0] = Special(n, nk());
        break;
    case 16:  // jugador y modelo: bytes del jugador en foco y especiales
        Good(R(2)); HitBoth();
        for (u32 k = 0; k < 3; ++k) { if (P(50)) c.player[k] = Special(n, nk()); if (P(30)) c.posn[k] = Special(n, nk()); }
        c.pif = Pick(std::array<u32, 6>{0u, 1u, 0x7Fu, 0x80u, 0xFFu, 3u}, n, nk());
        c.model = P(50) ? kHandled[R(8)] : Rand(n, nk());
        break;
    default:  // 17: combinacion aleatoria con acierto temprano y especiales en todo
        Good(R(3));
        if (P(60)) HitBoth();
        for (u32 k = 0; k < 3; ++k) if (P(25)) c.gorg[0][k] = Special(n, nk());
        break;
    }
    // regla de seguridad: un falso que devuelve verdadero deja siempre dos nodos de una ranura (el EXE los usa sin comprobar)
    for (u32 k = 0; k < kCalls; ++k) {
        if (!(c.gret[k] & 1u)) continue;
        for (u32* addr : {&c.gn1[k], &c.gn2[k]}) {
            bool ok = ((*addr >> 16) & 0xFFFFu) < kNodesPer;
            bool inSlot = false;
            for (u32 sl = 0; sl < kSlots; ++sl) if (c.areas[sl] == (*addr & 0xFFFFu)) inSlot = true;
            if (!ok || !inSlot) *addr = ValidAddr();
        }
    }
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("model", c.model).A("posn", std::vector<u32>(c.posn, c.posn + 3)).U("pif", c.pif).A("player", std::vector<u32>(c.player, c.player + 3)).U("amb", c.amb).U("fire", c.fire);
    j.A("gret", std::vector<u32>(c.gret, c.gret + kCalls));
    {
        std::vector<u32> v;
        for (u32 k = 0; k < kCalls; ++k) for (u32 q = 0; q < 3; ++q) v.push_back(c.gorg[k][q]);
        j.A("gorg", v);
    }
    j.A("gn1", std::vector<u32>(c.gn1, c.gn1 + kCalls)).A("gn2", std::vector<u32>(c.gn2, c.gn2 + kCalls)).A("gfrac", std::vector<u32>(c.gfrac, c.gfrac + kCalls));
    j.A("cnt", std::vector<u32>(c.cnt, c.cnt + kCalls)).A("phit", std::vector<u32>(c.phit, c.phit + 2)).A("pz", std::vector<u32>(c.pz, c.pz + 2));
    {
        std::vector<u32> v;
        for (u32 k = 0; k < 2; ++k) { v.push_back(c.pxy[k][0]); v.push_back(c.pxy[k][1]); }
        j.A("pxy", v);
    }
    j.U("dist", c.dist).A("areas", std::vector<u32>(c.areas, c.areas + kSlots));
    j.A("node_z", std::vector<u32>(c.nodeZ, c.nodeZ + kSlots * kNodesPer)).A("node_flag", std::vector<u32>(c.nodeFlag, c.nodeFlag + kSlots * kNodesPer));
    j.U("gate", c.gate);
    return j.End();
}
}

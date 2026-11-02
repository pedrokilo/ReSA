#pragma once
// Fixtures del banco Car84D (lote 84D): CCarCtrl::DragCarToPoint (EXE 0x42EC90), 18432 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::DragCarToPoint  0x42EC90  cdecl, dos argumentos (vehiculo, destino), `ret` simple, void   (hook CCarCtrl / DragCarToPoint, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) y las familias 16 y 17 (1024 casos cada una).
// El vehiculo es un CVehicle completo (0x5A0) con vtabla falsa (solo la entrada 0xD4, GetHeightAboveRoad, es valida), su matriz (arena), su informacion de modelo y caja de colision (arenas enlazadas desde
// CModelInfo::ms_modelInfoPtrs, restauradas) y dos entidades de suelo. Reglas de seguridad (el EXE no comprueba nada de esto): el vehiculo siempre tiene matriz; los suelos y las alturas son finitos y de
// modulo moderado (o NaN) cuando medio largo es finito, de modo que el cabeceo atan(dz / medio) no se acerca a pi/2 (alli la x87 y la DLL difieren en el cos por la precision del angulo); medio largo vale 0, NaN,
// un valor normal o uno enorme, nunca un valor pequeno distinto de cero.
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

namespace C84DFix {
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

// Arenas: guarda + ventana + guarda (la ventana es el objeto). La del vehiculo mide sizeof(CVehicle) = 0x5A0 (LiveComparison.hpp lo comprueba con static_assert).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x5A0, kMatWin = 0x48, kMiWin = 0x20, kColWin = 0x30, kEntWin = 0x40, kTgtWin = 0x10;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kMatArena = Arena(kMatWin), kMiArena = Arena(kMiWin), kColArena = Arena(kColWin), kEntArena = Arena(kEntWin), kTgtArena = Arena(kTgtWin);

constexpr u32 kModelBase = 0x400;                 // indice de modelo de las casillas del banco (4 casillas consecutivas de CModelInfo::ms_modelInfoPtrs)
constexpr u32 kModelPtrs = 0xA9B0C8;              // CModelInfo::ms_modelInfoPtrs

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 84004u); }
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
    Js& A(const char* k, const u32* v, int count) { Key(k); s += '['; for (int i = 0; i < count; ++i) { if (i) s += ','; s += std::to_string(v[i]); } s += ']'; return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ valores notables
// Un unico patron de NaN (el indefinido de la x87): con dos NaN distintos en una operacion la carga que sale depende del orden de operandos que elija el compilador
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
constexpr u32 kNan = 0xFFC00000u;
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Un float "especial": cero, desnormal, infinito, NaN (un unico patron), enorme
inline u32 Special(u32 c, u32 k) {
    const u32 r = Rand(c, k) % 32;
    if (r < 10) return Pick(kTiny, c, k + 1);
    if (r < 16) return Pick(kInf, c, k + 1);
    if (r < 24) return kNan;
    if (r < 28) return Pick(kBig, c, k + 1);
    return B(float(int(Rand(c, k + 3) % 2001) - 1000) / 7.0f);
}
inline bool IsNan(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
inline bool IsInf(u32 b) { return (b & 0x7FFFFFFFu) == 0x7F800000u; }

// Mutaciones (tras el evento numero mutAt el falso muta el estado; el oraculo repite la tabla):
//  1 posicion.z de la matriz actual   2 banderas del vehiculo (+0x1C)   3 suelo delantero guardado (+0x4E4)   4 suelo trasero guardado (+0x4E8)   5 la matriz del vehiculo pasa a la otra matriz (arena B)
//  6 byte +0x3E1   7 banderas de la entidad A   8 banderas de la entidad B   9 posicion.x y .y de la matriz actual
constexpr u32 kNoMutation = 99;
constexpr u32 kKinds = 9;

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 mslot{};                                 // casilla de modelo (0..3): indice de modelo = kModelBase + mslot
    u32 colMin{}, colMax{};                      // caja de colision: min.y y max.y
    u32 cnt{};                                   // byte +0x3E1
    u32 vflags{}, eflags[2]{};                   // banderas del vehiculo (+0x1C) y de las dos entidades de suelo
    u32 g4e4{}, g4e8{}, steer0{}, entp0{};       // suelo delantero y trasero guardados, volante, entidad de suelo guardada (+0x48C)
    u32 mr[3]{}, mf[3]{}, mu[3]{}, mp[3]{};      // matriz del vehiculo: derecha, adelante, arriba, posicion
    u32 br[3]{}, bf[3]{}, bu[3]{}, bp[3]{};      // la otra matriz (mutacion 5)
    u32 place[3]{};                              // transformacion simple (+4): la funcion no la usa con matriz
    u32 tgt[3]{};                                // destino (solo x e y se leen)
    // salidas de los falsos
    u32 sRet{}, sCp[3]{};                        // ProcessLineSphere
    u32 vRet[2]{}, vZ[2]{};                      // IsStoredPolyStillValidVerticalLine (delantero, trasero): retorno y z del punto
    u32 pRet[4]{}, pZ[4]{}, pEnt[4]{};           // ProcessVerticalLine (llamadas 1..4 en orden): retorno, z del punto, entidad (0 = A, 1 = B)
    u32 crs[2][3]{};                             // CrossProduct (dos llamadas)
    u32 height{};                                // GetHeightAboveRoad
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
    // Distancia en metros, multiplo de 1/8 en [-r, r]
    auto Meters = [&](int r) -> u32 { return B(float(int(R(u32(16 * r + 1))) - 8 * r) / 8.0f); };
    // Componente de un vector director: multiplo de 1/16 en [-1, 1]
    auto Dir = [&]() -> u32 { return B(float(int(R(33)) - 16) / 16.0f); };
    auto Ground = [&]() -> u32 { return P(4) ? kNan : Meters(400); };
    auto Sp = [&] { return Special(n, nk()); };

    c.mslot = R(4);
    // caja de colision: medio largo = (max - min) * 0.95f
    {
        const u32 w = R(100);
        if (w < 4) { c.colMin = c.colMax = Meters(3); }                                         // medio largo 0
        else if (w < 6) { c.colMin = kNan; c.colMax = Meters(3); }
        else if (w < 8) { c.colMin = B(-3.0e38f); c.colMax = B(3.0e38f); }                      // enorme (se desborda al guardarlo)
        else if (w < 12) { c.colMin = B(1.0f + float(R(24)) / 8.0f); c.colMax = B(-1.0f - float(R(24)) / 8.0f); }   // negativo
        else { c.colMin = B(-float(8 + R(40)) / 8.0f); c.colMax = B(float(8 + R(40)) / 8.0f); }
    }
    c.cnt = P(60) ? R(24) : R(256);
    c.vflags = Rand(n, nk()); c.eflags[0] = Rand(n, nk()); c.eflags[1] = Rand(n, nk());
    c.g4e4 = Ground(); c.g4e8 = Ground(); c.steer0 = Rand(n, nk()); c.entp0 = P(30) ? 0u : Rand(n, nk());
    for (u32 k = 0; k < 3; ++k) { c.mp[k] = Meters(3000); c.bp[k] = Meters(3000); c.place[k] = Meters(3000); c.tgt[k] = Meters(3000); }
    c.mp[2] = Meters(200); c.bp[2] = Meters(200);
    if (P(40)) { c.tgt[0] = B(F(c.mp[0]) + float(int(R(81)) - 40) / 8.0f); c.tgt[1] = B(F(c.mp[1]) + float(int(R(81)) - 40) / 8.0f); }   // destino cerca del vehiculo
    for (u32 k = 0; k < 3; ++k) { c.mr[k] = Dir(); c.mf[k] = Dir(); c.mu[k] = Dir(); c.br[k] = Dir(); c.bf[k] = Dir(); c.bu[k] = Dir(); }
    if (P(70)) { c.mf[2] = B(float(int(R(9)) - 4) / 16.0f); c.bf[2] = B(float(int(R(9)) - 4) / 16.0f); }
    // salidas de los falsos
    c.sRet = P(45) ? 1u : 0u;
    c.sCp[0] = P(60) ? B(F(c.tgt[0]) + float(int(R(41)) - 20) / 8.0f) : Meters(3000);
    c.sCp[1] = P(60) ? B(F(c.tgt[1]) + float(int(R(41)) - 20) / 8.0f) : Meters(3000);
    c.sCp[2] = Meters(100);
    for (u32 k = 0; k < 2; ++k) { c.vRet[k] = P(35) ? 1u : 0u; c.vZ[k] = Ground(); }
    for (u32 k = 0; k < 4; ++k) { c.pRet[k] = P(50) ? 1u : 0u; c.pZ[k] = Ground(); c.pEnt[k] = R(2); }
    for (u32 k = 0; k < 2; ++k) for (u32 j = 0; j < 3; ++j) c.crs[k][j] = B(float(int(R(401)) - 200) / 64.0f);
    {   // z de la segunda CrossProduct: el volante es (z * -10) acotado a [-0.5, 0.5]
        const u32 w = R(100);
        c.crs[1][2] = w < 40 ? B(float(int(R(101)) - 50) / 1024.0f) : w < 50 ? B(0.05f) : w < 60 ? B(-0.05f) : w < 66 ? kNan : w < 72 ? Pick(kInf, n, nk()) : w < 76 ? Pick(kTiny, n, nk()) : B(float(int(R(401)) - 200) / 64.0f);
    }
    c.height = P(10) ? Sp() : Meters(3);
    c.mutAt = P(30) ? R(4) : kNoMutation;
    c.mutKind = c.mutAt == kNoMutation ? 0u : 1u + R(kKinds);
    c.mutVal = P(25) ? Pick(kStackSeeds, n, nk()) : Rand(n, nk());

    // ---- sesgos por familia
    auto FrontNone = [&] { c.vRet[0] = 0; c.pRet[0] = 0; c.pRet[1] = 0; };
    switch (f) {
    case 0: break;
    case 1: c.sRet = 1; break;                                     // la recta corta la esfera
    case 2: c.sRet = 0; break;                                     // no corta: P se acerca al destino
    case 3: c.vRet[0] = 1; c.vRet[1] = 1; break;                   // los dos suelos en cache
    case 4: c.vRet[0] = 0; c.pRet[0] = 1; c.vRet[1] = 0; c.pRet[1] = 1; break;     // delantero con la primera lectura; el trasero con la segunda llamada global (indice 1)
    case 5: c.vRet[0] = 0; c.pRet[0] = 0; c.pRet[1] = 1; c.vRet[1] = 0; c.pRet[2] = 1; break;   // delantero con la segunda lectura
    case 6: FrontNone(); c.vRet[1] = P(50) ? 1u : 0u; break;       // delantero sin suelo (conserva el de antes)
    case 7: c.vRet[0] = P(50) ? 1u : 0u; c.vRet[1] = 0; c.pRet[2] = 0; c.pRet[3] = 0; c.pRet[1] = 0; c.pRet[0] = 0; break;   // trasero sin suelo
    case 8:    // byte +0x3E1 en torno al limite (16) y con signo
        { const u32 w = R(8); c.cnt = w == 0 ? 16u : w == 1 ? 17u : w == 2 ? 0x80u : w == 3 ? 0xFFu : w == 4 ? 0x7Fu : w == 5 ? 15u : R(256); }
        c.pRet[0] = 1; c.pRet[1] = P(50) ? 1u : 0u;
        break;
    case 9:    // banderas: con acierto de ProcessVerticalLine se copian los bits 30 y 31 de la entidad
        c.vRet[0] = 0; c.vRet[1] = 0; c.pRet[0] = 1; c.pRet[1] = 1; c.pRet[2] = 1; c.pRet[3] = 1;
        c.eflags[0] = u32(R(4)) << 30 | (Rand(n, nk()) & 0x3FFFFFFFu); c.eflags[1] = u32(R(4)) << 30 | (Rand(n, nk()) & 0x3FFFFFFFu);
        c.vflags = u32(R(4)) << 30 | (Rand(n, nk()) & 0x3FFFFFFFu);
        break;
    case 10: case 11: case 12:   // barrido de mutaciones
        c.mutAt = R(4); c.mutKind = f == 10 ? 1u + (n / 16) % 5u : f == 11 ? 6u + (n / 16) % 4u : 1u + R(kKinds);
        if (P(50)) c.sRet = 1;
        break;
    case 13:   // el retorno de GetHeightAboveRoad y las alturas especiales
        c.height = P(50) ? Sp() : Meters(3);
        break;
    case 14:   // el volante: la z de la segunda CrossProduct cubre los cuatro tramos
        { const u32 w = R(5); c.crs[1][2] = w == 0 ? B(float(int(R(101)) - 50) / 512.0f) : w == 1 ? B(0.06f + float(R(100)) / 100.0f) : w == 2 ? B(-0.06f - float(R(100)) / 100.0f) : w == 3 ? kNan : Pick(kInf, n, nk()); }
        break;
    case 15: break;            // todo aleatorio
    case 16:   // especiales (NaN, infinitos, desnormales, enormes) en posiciones, vectores, destino y salidas
        {
            u32* t[] = {&c.mp[0], &c.mp[1], &c.mp[2], &c.mf[0], &c.mf[1], &c.mf[2], &c.mr[0], &c.tgt[0], &c.tgt[1], &c.sCp[0], &c.sCp[1], &c.height, &c.crs[0][0], &c.crs[0][1], &c.crs[1][0], &c.crs[1][1], &c.g4e4, &c.g4e8, &c.vZ[0], &c.pZ[0], &c.pZ[2]};
            for (int k = 0; k < 21; ++k) if (P(10)) *t[k] = Sp();
        }
        break;
    default:   // 17: geometria degenerada (destino sobre P, vectores nulos, cajas de colision nulas)
        { const u32 w = R(6);
          if (w == 0) { c.tgt[0] = c.mp[0]; c.tgt[1] = c.mp[1]; for (u32 k = 0; k < 3; ++k) c.mf[k] = 0; }
          else if (w == 1) { c.colMin = c.colMax = 0u; }
          else if (w == 2) { c.mf[0] = 0; c.mf[1] = 0; c.mf[2] = B(1.0f); }
          else if (w == 3) { c.tgt[0] = B(F(c.mp[0]) - 0.125f * float(R(3))); c.tgt[1] = c.mp[1]; }
          else if (w == 4) { for (u32 k = 0; k < 3; ++k) c.mr[k] = 0; c.colMin = B(-1.0f); c.colMax = B(1.0f); }
          else { c.mp[0] = c.tgt[0]; c.mp[1] = c.tgt[1]; }
        }
        break;
    }
    // ---- reglas de seguridad. Medio largo = (max.y - min.y) * 0.95f. El cabeceo atan(dz / medio) no debe acercarse a +-pi/2 (alli el cos de la x87 y el de la DLL difieren por la precision del angulo, hasta en el
    // signo): con medio largo distinto de cero y de NaN los suelos son NaN o de modulo <= 1e4 (medio largo >= 0.5 en modulo; los menores se sustituyen por 0); con medio largo 0 todos los suelos son iguales
    // (dz = 0 y 0 * inf = NaN) o NaN. Con medio largo NaN no se restringe nada. Ningun valor flotante lleva un NaN con carga distinta del indefinido.
    {
        auto HalfOf = [&](double& half, bool& nan) { const double h = double(F(c.colMax)) - double(F(c.colMin)); nan = h != h; half = std::fabs(h * 0.95); };
        double half; bool nanH;
        HalfOf(half, nanH);
        if (!nanH && half != 0.0 && half < 0.5) { c.colMax = c.colMin; HalfOf(half, nanH); }
        const bool mutGround = c.mutAt != kNoMutation && (c.mutKind == 3 || c.mutKind == 4);
        if (!nanH && half == 0.0) {
            const u32 g = P(5) ? kNan : Meters(400);
            c.g4e4 = g; c.g4e8 = g;
            for (u32 k = 0; k < 2; ++k) c.vZ[k] = g;
            for (u32 k = 0; k < 4; ++k) c.pZ[k] = g;
            if (mutGround) c.mutVal = g;
        } else if (!nanH) {
            auto Bound = [&](u32& v) { if (IsInf(v) || (!IsNan(v) && std::fabs(F(v)) > 1.0e4f)) v = Meters(400); };
            Bound(c.g4e4); Bound(c.g4e8);
            for (u32 k = 0; k < 2; ++k) Bound(c.vZ[k]);
            for (u32 k = 0; k < 4; ++k) Bound(c.pZ[k]);
            if (mutGround) c.mutVal = P(10) ? kNan : Meters(400);
        }
        auto Clean = [](u32& v) { if (IsNan(v)) v = kNan; };
        if (c.mutAt != kNoMutation && (c.mutKind == 1 || c.mutKind == 3 || c.mutKind == 4 || c.mutKind == 9)) Clean(c.mutVal);
        for (u32 k = 0; k < 3; ++k) { Clean(c.mp[k]); Clean(c.bp[k]); Clean(c.mr[k]); Clean(c.mf[k]); Clean(c.mu[k]); Clean(c.br[k]); Clean(c.bf[k]); Clean(c.bu[k]); Clean(c.tgt[k]); Clean(c.place[k]); Clean(c.sCp[k]); }
        for (u32 k = 0; k < 2; ++k) for (u32 j = 0; j < 3; ++j) Clean(c.crs[k][j]);
        Clean(c.height); Clean(c.colMin); Clean(c.colMax); Clean(c.g4e4); Clean(c.g4e8);
        for (u32 k = 0; k < 2; ++k) Clean(c.vZ[k]);
        for (u32 k = 0; k < 4; ++k) Clean(c.pZ[k]);
        // la entidad de suelo guardada: 0 o una direccion que no esta en ninguna arena ni en la pila
        if (c.entp0 != 0) c.entp0 |= 0xE0000000u;
    }
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("mslot", c.mslot).U("col_min", c.colMin).U("col_max", c.colMax).U("cnt", c.cnt).U("vflags", c.vflags);
    j.A("eflags", c.eflags, 2).U("g4e4", c.g4e4).U("g4e8", c.g4e8).U("steer0", c.steer0).U("entp0", c.entp0);
    j.A("mr", c.mr, 3).A("mf", c.mf, 3).A("mu", c.mu, 3).A("mp", c.mp, 3).A("br", c.br, 3).A("bf", c.bf, 3).A("bu", c.bu, 3).A("bp", c.bp, 3).A("place", c.place, 3).A("tgt", c.tgt, 3);
    j.U("s_ret", c.sRet).A("s_cp", c.sCp, 3).A("v_ret", c.vRet, 2).A("v_z", c.vZ, 2).A("p_ret", c.pRet, 4).A("p_z", c.pZ, 4).A("p_ent", c.pEnt, 4);
    j.A("crs0", c.crs[0], 3).A("crs1", c.crs[1], 3).U("height", c.height);
    j.U("mut_at", c.mutAt).U("mut_kind", c.mutKind).U("mut_val", c.mutVal).U("gate", c.gate);
    return j.End();
}
}

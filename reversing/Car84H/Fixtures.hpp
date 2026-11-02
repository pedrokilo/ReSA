#pragma once
// Fixtures del banco Car84H (lote 84H): CCarCtrl::SteerAICarWithPhysicsFollowPath (EXE 0x434900), 18432 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::SteerAICarWithPhysicsFollowPath  0x434900  cdecl, cinco argumentos (vehiculo y cuatro punteros a salidas), `ret` simple, void   (hook CCarCtrl / SteerAICarWithPhysicsFollowPath, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) y las familias 16 y 17 (1024 casos cada una).
// El vehiculo es una arena de 0x9A0 bytes con su matriz (siempre presente: el EXE la usa sin comprobar nulo), un entidad objetivo (arena con su matriz opcional), el mundo de caminos (tres areas con
// ocho tramos de carretera de 14 bytes cada una enlazadas desde ThePaths), una arena de salidas (giro, acelerador, freno y freno de mano) y colaboradores falsos. Reglas de seguridad (el EXE no comprueba
// nada de esto): todo tramo al que lee el EXE pertenece a un area cargada (las direcciones de area descargada solo aparecen donde el EXE las comprueba antes de leer), los identificadores de tramo son < 8,
// el objetivo existe siempre, ninguna mutacion lo anula, y ningun valor flotante lleva un NaN con carga distinta del indefinido.
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

namespace C84HFix {
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
constexpr unsigned kSlots = 3, kLinks = 8, kLinkBytes = 14, kSlotWords = 28;   // 8 * 14 = 112 bytes = 28 palabras por area

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x9A0, kMatWin = 0x48, kOutWin = 0x10, kTgtWin = 0x20, kLinkWin = kLinks * kLinkBytes, kPathWin = 0x10;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kMatArena = Arena(kMatWin), kOutArena = Arena(kOutWin), kTgtArena = Arena(kTgtWin), kTMatArena = Arena(kMatWin), kLinkArena = Arena(kLinkWin), kPathArena = Arena(kPathWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 84008u); }
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
//  1 mision (+0x3BA)   2 accion temporal (+0x3BB)   3 estilo de conduccion (+0x3B9)   4 velocidad de crucero (+0x3D0)   5 CTimer::m_snTimeInMilliseconds   6 velocidad.x (+0x44)
//  7 adelante.x de la matriz   8 tramo actual (+0x3A4)   9 tramo siguiente (+0x3A6)   10 carril actual (+0x3B7)   11 subtipo (+0x594)   12 posicion.x de la matriz   13 destino.x (+0x3EC)
//  14 multiplicador actual (+0x3B5)   15 temporizador (+0x818)
constexpr u32 kNoMutation = 99;
constexpr u32 kKinds = 15;

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 cur{}, next{};                           // +0x3A4, +0x3A6: direcciones de tramo (id | area << 10)
    u32 s5{}, s6{}, l7{}, l8{};                  // +0x3B5, +0x3B6 (multiplicadores) y +0x3B7, +0x3B8 (carriles): bytes
    u32 style{}, mission{}, tact{}, ttime{};     // +0x3B9, +0x3BA, +0x3BB (bytes), +0x3BC
    u32 cruise{}, subtype{}, timer{};            // +0x3D0 (byte), +0x594, +0x818 (float)
    u32 dest[3]{};                               // +0x3EC
    u32 speed[3]{};                              // +0x44
    u32 mr[3]{}, mf[3]{}, mu[3]{}, mp[3]{};      // matriz del vehiculo: derecha, adelante, arriba, posicion
    u32 tmat{}, tpos[3]{}, tmp[3]{};             // objetivo: con matriz (posicion en la matriz) o sin ella (posicion en +4)
    u32 now{};                                   // CTimer::m_snTimeInMilliseconds
    u32 areas[kSlots]{}, unl[2]{};               // numeros de area cargados y descargados
    u32 links[kSlots][kSlotWords]{};             // 112 bytes por area
    // salidas de los falsos
    u32 stopRet{}, pickRet{}, light{}, bridge{};
    u32 pickMut{}, pickCur{}, pickNext{}, pickS5{}, pickS6{}, pickL7{}, pickL8{}, pickMission{};
    u32 hf[3]{}, hfHb{}, player[3]{};
    u32 weave{}, maxSteer{}, maxTraffic{}, mult[2]{};
    u32 o0[4]{};                                 // salidas antes de la llamada (giro, acelerador, freno y byte de freno de mano)
    u32 mutAt{}, mutKind{}, mutVal{};
    u32 gate{};
};

// byte b de un tramo (slot s, id k)
inline u8 LinkByte(const C& c, u32 s, u32 k, u32 off) {
    const u32 pos = k * kLinkBytes + off;
    return u8(c.links[s][pos / 4] >> (8 * (pos % 4)));
}
inline void SetLinkByte(C& c, u32 s, u32 k, u32 off, u32 v) {
    const u32 pos = k * kLinkBytes + off;
    u32& w = c.links[s][pos / 4];
    w = (w & ~(0xFFu << (8 * (pos % 4)))) | ((v & 0xFFu) << (8 * (pos % 4)));
}
inline void SetLink16(C& c, u32 s, u32 k, u32 off, int v) { SetLinkByte(c, s, k, off, u32(v)); SetLinkByte(c, s, k, off + 1, u32(v) >> 8); }

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = Rand(n, 1); c.ecx = Rand(n, 3); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    const u32 sweep = n < kBaseCases ? n / 16 : (n - kBaseCases) % 1024;
    u32 kk = 100;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    auto Meters = [&](int r) -> u32 { return B(float(int(R(u32(16 * r + 1))) - 8 * r) / 8.0f); };    // multiplo de 1/8 en [-r, r]
    auto Dir = [&]() -> u32 { return B(float(int(R(33)) - 16) / 16.0f); };                            // multiplo de 1/16 en [-1, 1]
    auto Sp = [&] { return Special(n, nk()); };
    auto Small = [&]() -> u32 { return u32(int(R(9)) - 4) & 0xFFu; };                                 // int8 en [-4, 4]
    auto Ang = [&]() -> u32 { return B(float(int(R(401)) - 200) / 32.0f); };                          // angulo en [-6.25, 6.25]

    // ---- areas (tres cargadas y dos descargadas, distintas)
    {
        u32 used[5];
        for (u32 s = 0; s < 5; ++s) {
            u32 a;
            bool dup;
            do {
                a = R(64);
                dup = false;
                for (u32 t = 0; t < s; ++t) dup |= used[t] == a;
            } while (dup);
            used[s] = a;
        }
        for (u32 s = 0; s < kSlots; ++s) c.areas[s] = used[s];
        c.unl[0] = used[3]; c.unl[1] = used[4];
    }
    auto Word = [&](u32 s, u32 id) -> u32 { return (c.areas[s] << 10) | id; };
    auto LoadedWord = [&]() -> u32 { return Word(R(kSlots), R(kLinks)); };

    // ---- vehiculo y matriz
    for (u32 k = 0; k < 3; ++k) {
        c.mp[k] = k < 2 ? Meters(3000) : Meters(100);
        c.mr[k] = Dir(); c.mf[k] = Dir(); c.mu[k] = Dir();
        c.speed[k] = P(10) ? Sp() : B(float(int(R(193)) - 96) / 32.0f);
        c.dest[k] = Meters(3000);
        c.tpos[k] = Meters(3000); c.tmp[k] = Meters(3000);
        c.player[k] = Meters(3000);
    }
    c.tmat = P(40) ? 1u : 0u;
    c.cur = LoadedWord(); c.next = P(40) ? c.cur : LoadedWord();
    c.s5 = Small(); c.s6 = Small(); c.l7 = Small(); c.l8 = Small();
    c.style = P(70) ? R(7) : R(256);
    c.mission = P(30) ? (P(50) ? 8u : 12u) : R(256);
    c.tact = P(70) ? 0u : R(256);
    c.cruise = P(15) ? R(12) : R(256);
    c.subtype = P(25) ? 10u : R(20);
    c.timer = P(50) ? B(float(int(R(41)) - 20) / 8.0f) : Rand(n, nk());
    c.ttime = Rand(n, nk());
    c.now = P(8) ? R(4) : P(8) ? 0xFFFFFFFFu - R(4) : Rand(n, nk());

    // ---- tramos: todos cerca del vehiculo (uno cerca de cada umbral de distancia)
    auto Offset = [&](int meters8) { return float(meters8) / 8.0f; };
    auto PlaceLink = [&](u32 s, u32 k, float cx, float cy, float radius) {
        const float ox = Offset(int(R(u32(16 * radius) + 1)) - int(8 * radius));
        const float oy = Offset(int(R(u32(16 * radius) + 1)) - int(8 * radius));
        auto q = [](float v) { int r = int(std::floor(v * 8.0f + 0.5f)); return std::clamp(r, -32768, 32767); };
        SetLink16(c, s, k, 0, q(cx + ox));
        SetLink16(c, s, k, 2, q(cy + oy));
    };
    {
        const float vx = F(c.mp[0]), vy = F(c.mp[1]);
        for (u32 s = 0; s < kSlots; ++s) {
            for (u32 k = 0; k < kLinks; ++k) {
                for (u32 b = 0; b < kLinkBytes; ++b) SetLinkByte(c, s, k, b, R(256));
                PlaceLink(s, k, vx, vy, P(30) ? 12.0f : 70.0f);
                SetLinkByte(c, s, k, 8, P(80) ? u32(int(R(201)) - 100) : R(256));
                SetLinkByte(c, s, k, 9, P(80) ? u32(int(R(201)) - 100) : R(256));
                SetLinkByte(c, s, k, 0xB, P(30) ? (R(8) | (R(8) << 3)) : R(256));
            }
        }
    }
    // ---- falsos
    c.stopRet = P(4) ? 1u : 0u;
    c.pickRet = P(60) ? 1u : 0u;
    c.light = P(15) ? 1u : 0u; c.bridge = P(8) ? 1u : 0u;
    c.pickMut = P(60) ? 1u : 0u;
    auto AnyWord = [&]() -> u32 {   // un area cualquiera: si esta cargada el id es < 8 (el EXE lee ese tramo)
        const u32 area = R(64);
        const bool loaded = area == c.areas[0] || area == c.areas[1] || area == c.areas[2];
        return (area << 10) | (loaded ? R(kLinks) : R(1024));
    };
    c.pickCur = P(70) ? LoadedWord() : AnyWord();
    c.pickNext = P(70) ? LoadedWord() : AnyWord();
    c.pickS5 = Small(); c.pickS6 = Small(); c.pickL7 = Small(); c.pickL8 = Small();
    {
        static constexpr u32 missions[] = {2, 4, 8, 12, 15, 17, 0x2B, 0x41, 0x42, 0x43, 0x44, 3, 5, 9, 0x10, 0x40, 0x45, 1, 0, 0xFE, 0xFF, 0x2A, 0x2C};
        c.pickMission = P(75) ? missions[R(std::size(missions))] : R(256);
    }
    for (u32 k = 0; k < 3; ++k) c.hf[k] = P(70) ? B(float(int(R(33)) - 16) / 8.0f) : Sp();
    c.hfHb = P(50) ? 1u : 0u;
    c.weave = P(70) ? Ang() : Sp();
    c.maxSteer = P(70) ? B(float(int(R(41)) - 20) / 32.0f) : Sp();
    c.maxTraffic = P(70) ? B(float(int(R(81)) - 8) / 2.0f) : Sp();
    for (u32 k = 0; k < 2; ++k) c.mult[k] = P(70) ? B(float(int(R(25))) / 16.0f) : Sp();
    for (u32 k = 0; k < 4; ++k) c.o0[k] = Rand(n, nk());
    c.mutAt = P(25) ? R(5) : kNoMutation;
    c.mutKind = c.mutAt == kNoMutation ? 0u : 1u + R(kKinds);
    c.mutVal = P(25) ? Pick(kStackSeeds, n, nk()) : Rand(n, nk());

    // ---- sesgos por familia
    // Coloca el vehiculo a `dist` metros del primer tramo (en un angulo aleatorio): los desplazamientos de carril desvian el punto 1 unos metros
    auto VehicleAt = [&](float dist, bool alongSegment) {
        const u32 s = (c.cur >> 10) == c.areas[0] ? 0u : (c.cur >> 10) == c.areas[1] ? 1u : 2u;
        const u32 k = c.cur & 0x3FFu;
        const float lx = float(i16(u16(LinkByte(c, s, k, 0) | (LinkByte(c, s, k, 1) << 8)))) / 8.0f;
        const float ly = float(i16(u16(LinkByte(c, s, k, 2) | (LinkByte(c, s, k, 3) << 8)))) / 8.0f;
        float ax = float(int(R(201)) - 100), ay = float(int(R(201)) - 100);
        if (alongSegment) {   // hacia el segundo tramo
            const u32 s2 = (c.next >> 10) == c.areas[0] ? 0u : (c.next >> 10) == c.areas[1] ? 1u : 2u;
            const u32 k2 = c.next & 0x3FFu;
            const float nx = float(i16(u16(LinkByte(c, s2, k2, 0) | (LinkByte(c, s2, k2, 1) << 8)))) / 8.0f;
            const float ny = float(i16(u16(LinkByte(c, s2, k2, 2) | (LinkByte(c, s2, k2, 3) << 8)))) / 8.0f;
            ax = nx - lx; ay = ny - ly;
        }
        const float len = std::sqrt(ax * ax + ay * ay);
        if (len < 0.001f) { ax = 1.0f; ay = 0.0f; }
        const float inv = len < 0.001f ? 1.0f : 1.0f / len;
        c.mp[0] = B(std::floor((lx + ax * inv * dist) * 8.0f + 0.5f) / 8.0f);
        c.mp[1] = B(std::floor((ly + ay * inv * dist) * 8.0f + 0.5f) / 8.0f);
    };
    auto PutSegmentNear = [&] {   // el segundo tramo a 20..60 m del primero (otro area o el mismo)
        const u32 s = (c.cur >> 10) == c.areas[0] ? 0u : (c.cur >> 10) == c.areas[1] ? 1u : 2u;
        const u32 s2 = (c.next >> 10) == c.areas[0] ? 0u : (c.next >> 10) == c.areas[1] ? 1u : 2u;
        const u32 k = c.cur & 0x3FFu, k2 = c.next & 0x3FFu;
        const int x = i16(u16(LinkByte(c, s, k, 0) | (LinkByte(c, s, k, 1) << 8)));
        const int y = i16(u16(LinkByte(c, s, k, 2) | (LinkByte(c, s, k, 3) << 8)));
        const int dx = int(R(641)) - 320, dy = int(R(641)) - 320;
        SetLink16(c, s2, k2, 0, std::clamp(x + dx, -32768, 32767));
        SetLink16(c, s2, k2, 2, std::clamp(y + dy, -32768, 32767));
    };
    auto Distance = [&]() -> float {
        static constexpr float d[] = {0.5f, 2.0f, 4.0f, 4.9f, 5.0f, 5.1f, 6.0f, 7.0f, 7.9f, 8.0f, 8.1f, 12.0f, 25.0f, 39.0f, 40.0f, 41.0f, 55.0f, 90.0f};
        return d[R(std::size(d))];
    };
    switch (f) {
    case 0: break;
    case 1:    // StopCarIfNodesAreInvalid verdadero y areas descargadas
        c.stopRet = P(50) ? 1u : 0u;
        if (P(40)) c.cur = (c.unl[0] << 10) | R(1024);
        if (P(40)) c.next = (c.unl[1] << 10) | R(1024);
        break;
    case 2:    // cerca del primer tramo: elige el siguiente nodo
        c.stopRet = 0; VehicleAt(P(60) ? 2.0f : 4.0f, false);
        break;
    case 3:    // 5..8 m (con producto positivo elige el siguiente)
        c.stopRet = 0; VehicleAt(P(70) ? 6.5f : 7.5f, P(60));
        break;
    case 4:    // alineado: razon > 0.7
        c.stopRet = 0; PutSegmentNear(); VehicleAt(8.0f + float(R(20)), true);
        break;
    case 5:    // desalineado y tramos distintos: salta al punto objetivo
        c.stopRet = 0; c.next = (c.cur ^ 1u) & ~0u; if ((c.next >> 10) != c.areas[0] && (c.next >> 10) != c.areas[1] && (c.next >> 10) != c.areas[2]) c.next = LoadedWord();
        if (c.next == c.cur) c.next = Word(0, (c.cur + 1) & 7u) == c.cur ? Word(0, (c.cur + 2) & 7u) : Word(0, (c.cur + 1) & 7u);
        VehicleAt(P(60) ? 12.0f : Distance(), false);
        break;
    case 6:    // eleccion del siguiente nodo con misiones del conmutador
        c.stopRet = 0; c.pickRet = 1; VehicleAt(3.0f, false);
        break;
    case 7:    // eleccion fallida o mision por defecto: se recalcula el primer punto
        c.stopRet = 0; VehicleAt(P(70) ? 3.0f : 6.0f, false);
        c.pickRet = P(50) ? 0u : 1u; c.pickMut = P(80) ? 1u : 0u;
        if (c.pickRet) c.pickMission = P(50) ? 0x40u : R(256);
        break;
    case 8:    // tras la eleccion, tramos descargados
        c.stopRet = 0; VehicleAt(3.0f, false); c.pickMut = 1;
        c.pickRet = P(50) ? 0u : 1u; if (c.pickRet) c.pickMission = 0x40u;
        if (P(50)) c.pickCur = (c.unl[0] << 10) | R(1024); else c.pickNext = (c.unl[1] << 10) | R(1024);
        break;
    case 9:    // lejos (> 40 m)
        c.stopRet = 0; VehicleAt(P(50) ? 39.9f : 41.0f + float(R(60)), false); c.next = P(70) ? c.cur : c.next;
        break;
    case 10:   // estilo de conduccion y velocidad de crucero
        c.stopRet = 0; VehicleAt(P(60) ? 3.0f : 20.0f, false); c.style = R(9) < 7 ? R(7) : R(256);
        c.cruise = P(30) ? R(14) : R(256);
        break;
    case 11:   // rumbo con correccion (estilos 2, 5, 6) y normalizacion
        c.stopRet = 0; VehicleAt(P(50) ? 3.0f : 30.0f, false); { static constexpr u32 st[] = {2, 5, 6, 0, 1, 3, 4}; c.style = st[R(7)]; }
        c.weave = P(70) ? B(float(int(R(401)) - 200) / 8.0f) : c.weave;
        break;
    case 12:   // semaforo y puente
        c.stopRet = 0; VehicleAt(P(50) ? 3.0f : 30.0f, false); c.light = P(60) ? 1u : 0u; c.bridge = P(40) ? 1u : 0u;
        c.style = P(70) ? R(7) : R(256);
        break;
    case 13:   // velocidad deseada, gas y freno
        c.stopRet = 0; VehicleAt(P(50) ? 3.0f : 20.0f, false); c.cruise = P(25) ? R(4) : R(256);
        c.mult[0] = P(50) ? B(float(int(R(17))) / 16.0f) : c.mult[0]; c.mult[1] = P(50) ? B(float(int(R(17))) / 16.0f) : c.mult[1];
        if (P(40)) { c.mf[0] = B(1.0f); c.mf[1] = 0; c.mf[2] = 0; c.speed[0] = B(float(int(R(41)) - 10) / 64.0f); c.speed[1] = c.speed[2] = 0; }
        break;
    case 14:   // motos
        c.stopRet = 0; VehicleAt(P(50) ? 3.0f : 20.0f, false); c.subtype = P(80) ? 10u : R(20);
        c.timer = P(60) ? B(float(int(R(21)) - 10) / 8.0f) : B(0.0f); if (P(15)) c.timer = 0x80000000u;
        c.cruise = P(60) ? 12u + R(244) : c.cruise;
        break;
    case 15:   // misiones 8 y 12 sin accion temporal: cerca del destino
        c.stopRet = 0; VehicleAt(P(60) ? 3.0f : 20.0f, false);
        c.mission = P(50) ? 8u : 12u; c.tact = P(80) ? 0u : R(256); c.pickMut = P(60) ? 0u : 1u; c.pickMission = P(50) ? 0x40u : c.mission;
        {
            const bool flat = P(75);   // adelante vertical (longitud horizontal 0) y posicion casi a ras del suelo: |posicion . adelante| / distancia < 0.2
            if (flat) { c.mf[0] = 0; c.mf[1] = 0; c.mf[2] = P(50) ? B(1.0f) : B(-0.5f); c.mp[2] = Meters(1); }
            const float s = F(c.mp[0]), t = F(c.mp[1]);
            const float ox = float(int(R(33)) - 16) / 4.0f, oy = float(int(R(33)) - 16) / 4.0f, oz = float(int(R(9)) - 4) / 2.0f;
            c.dest[0] = B(s + ox); c.dest[1] = B(t + oy); c.dest[2] = B(F(c.mp[2]) + oz);
            if (!flat) { c.mf[0] = B(float(int(R(33)) - 16) / 16.0f); c.mf[1] = B(float(int(R(33)) - 16) / 16.0f); c.mf[2] = 0; }
        }
        break;
    case 16:   // especiales (NaN, infinitos, desnormales, enormes)
        {
            u32* t[] = {&c.speed[0], &c.speed[1], &c.speed[2], &c.mf[0], &c.mf[1], &c.mf[2], &c.mp[2], &c.dest[0], &c.dest[1], &c.dest[2], &c.timer, &c.weave, &c.maxSteer, &c.maxTraffic, &c.mult[0], &c.mult[1], &c.hf[0], &c.hf[1]};
            for (u32* p : t) if (P(12)) *p = Sp();
        }
        c.stopRet = 0;
        break;
    default:   // 17: barrido de mutaciones
        c.stopRet = 0; VehicleAt(Distance(), P(40));
        c.mutAt = R(5); c.mutKind = 1u + sweep % kKinds;
        break;
    }

    // ---- reglas de seguridad
    {
        // mutaciones de tramo: siempre a un tramo cargado
        c.mutVal = (c.mutKind == 8 || c.mutKind == 9) ? LoadedWord() : c.mutVal;
        if (c.mutKind == 10 || c.mutKind == 14) c.mutVal = Small();
        if (c.mutKind == 11) c.mutVal = P(50) ? 10u : R(20);
        if (c.mutKind == 12 || c.mutKind == 13) c.mutVal = Meters(3000);
        if (c.mutKind == 7) c.mutVal = Dir();
    }
    // un NaN con carga distinta del indefinido no se usa (la carga que sale depende del orden de operandos)
    auto Clean = [](u32& v) { if (IsNan(v)) v = kNan; };
    for (u32 k = 0; k < 3; ++k) { Clean(c.speed[k]); Clean(c.mp[k]); Clean(c.mr[k]); Clean(c.mf[k]); Clean(c.mu[k]); Clean(c.dest[k]); Clean(c.tpos[k]); Clean(c.tmp[k]); Clean(c.player[k]); Clean(c.hf[k]); }
    Clean(c.timer); Clean(c.weave); Clean(c.maxSteer); Clean(c.maxTraffic); Clean(c.mult[0]); Clean(c.mult[1]);
    if (c.mutAt != kNoMutation && (c.mutKind == 6 || c.mutKind == 7 || c.mutKind == 12 || c.mutKind == 13 || c.mutKind == 15)) Clean(c.mutVal);
    // el rumbo corregido se normaliza con un bucle de +-2 pi: un valor infinito o enorme no terminaria nunca (ni en el EXE ni en la DLL)
    if (IsInf(c.weave) || (!IsNan(c.weave) && std::fabs(F(c.weave)) > 2000.0f)) c.weave = Ang();
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("cur", c.cur).U("next", c.next).U("s5", c.s5).U("s6", c.s6).U("l7", c.l7).U("l8", c.l8).U("style", c.style).U("mission", c.mission).U("tact", c.tact).U("ttime", c.ttime);
    j.U("cruise", c.cruise).U("subtype", c.subtype).U("timer", c.timer).A("dest", c.dest, 3).A("speed", c.speed, 3);
    j.A("mr", c.mr, 3).A("mf", c.mf, 3).A("mu", c.mu, 3).A("mp", c.mp, 3).U("tmat", c.tmat).A("tpos", c.tpos, 3).A("tmp", c.tmp, 3).U("now", c.now);
    j.A("areas", c.areas, kSlots).A("unl", c.unl, 2);
    for (u32 s = 0; s < kSlots; ++s) { const std::string key = "links" + std::to_string(s); j.A(key.c_str(), c.links[s], kSlotWords); }
    j.U("stop_ret", c.stopRet).U("pick_ret", c.pickRet).U("light", c.light).U("bridge", c.bridge);
    j.U("pick_mut", c.pickMut).U("pick_cur", c.pickCur).U("pick_next", c.pickNext).U("pick_s5", c.pickS5).U("pick_s6", c.pickS6).U("pick_l7", c.pickL7).U("pick_l8", c.pickL8).U("pick_mission", c.pickMission);
    j.A("hf", c.hf, 3).U("hf_hb", c.hfHb).A("player", c.player, 3);
    j.U("weave", c.weave).U("max_steer", c.maxSteer).U("max_traffic", c.maxTraffic).A("mult", c.mult, 2).A("o0", c.o0, 4);
    j.U("mut_at", c.mutAt).U("mut_kind", c.mutKind).U("mut_val", c.mutVal).U("gate", c.gate);
    return j.End();
}
}

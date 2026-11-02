#pragma once
// Fixtures del banco Car84J (lote 84J): CCarCtrl::SteerAICarWithPhysicsFollowPath_Racing (EXE 0x435830), 18432 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::SteerAICarWithPhysicsFollowPath_Racing  0x435830  cdecl, cinco argumentos (vehiculo y cuatro punteros a salidas), `ret` simple, void   (hook CCarCtrl / SteerAICarWithPhysicsFollowPath_Racing, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) y las familias 16 y 17 (1024 casos cada una).
// El vehiculo es una arena de 0x9A0 bytes con su matriz (siempre presente: el EXE la usa sin comprobar nulo), una arena de salidas (giro, acelerador, freno y freno de mano) y el mundo de caminos: tres areas
// cargadas (tramos de carretera de 14 bytes, nodos de 0x1C bytes con su primer enlace, enlaces de nodo y enlaces de navegacion) enlazadas desde ThePaths. Reglas de seguridad (el EXE no comprueba nada de esto):
// todo tramo que lee el EXE pertenece a un area cargada con id < 8; el nodo actual tras el avance (el siguiente de antes) esta en un area cargada con id < 8 y su tabla de enlaces contiene, entre sus 16
// primeras entradas, TODOS los nodos que pueden quedar como siguiente (la busqueda del EXE no tiene limite); los enlaces de navegacion de esas entradas son tramos cargados; los nodos invalidos (area 0xFFFF)
// solo forman un sufijo de la ruta; ningun valor flotante lleva un NaN con carga distinta del indefinido; el rumbo corregido es finito y menor de 2000 radianes (el bucle de normalizacion no terminaria).
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

namespace C84JFix {
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
constexpr unsigned kNodes = 8, kNodeBytes = 0x1C, kNL = 40, kPool = 16;       // nodos por area, bytes por nodo, entradas de enlaces por area, entradas del grupo de candidatos

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x9A0, kMatWin = 0x48, kOutWin = 0x10, kLinkWin = kLinks * kLinkBytes, kNodeWin = kSlots * kNodes * kNodeBytes, kNlWin = kSlots * kNL * 4, kNaviWin = kSlots * kNL * 2;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kMatArena = Arena(kMatWin), kOutArena = Arena(kOutWin), kLinkArena = Arena(kLinkWin), kNodeArena = Arena(kNodeWin), kNlArena = Arena(kNlWin), kNaviArena = Arena(kNaviWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 84010u); }
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
//  1 velocidad.x (+0x44)   2 adelante.x de la matriz   3 posicion.x de la matriz   4 tramo actual (+0x3A4)   5 tramo siguiente (+0x3A6)   6 multiplicador actual (+0x3B5)   7 multiplicador siguiente (+0x3B6)
//  8 velocidad de crucero (+0x3D0)   9 subtipo (+0x594)   10 temporizador (+0x818)   11 contador de la ruta (+0x418)   12 primer nodo de la ruta (+0x3F8)   13 nodo siguiente (+0x394)
//  14 destino.x (+0x3EC)   15 modelo (+0x22)   16 banderas (+0x3DB)   17 carril siguiente (+0x3B8)
constexpr u32 kNoMutation = 99;
constexpr u32 kKinds = 17;

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 cur{}, next{}, prevL{};                  // +0x3A4, +0x3A6, +0x3A8: direcciones de tramo (id | area << 10)
    u32 s4{}, s5{}, s6{}, l7{}, l8{};            // +0x3B4..+0x3B8: bytes (multiplicadores y carriles)
    u32 mission{}, cruise{}, subtype{}, timer{}; // +0x3BA, +0x3D0 (bytes), +0x594, +0x818 (float)
    u32 model{}, f3db{};                         // +0x22 (word), +0x3DB (byte)
    u32 curN{}, nextN{}, prevN{};                // +0x390, +0x394, +0x398: direcciones de nodo (area | id << 16)
    u32 fieldC{}, scale{};                       // +0x39C, +0x3A0
    u32 pn[8]{}, pcount{};                       // ruta (+0x3F8) y su contador (+0x418, int16)
    u32 dest[3]{};                               // +0x3EC
    u32 speed[3]{};                              // +0x44
    u32 mr[3]{}, mf[3]{}, mu[3]{}, mp[3]{};      // matriz del vehiculo: derecha, adelante, arriba, posicion
    u32 areas[kSlots]{}, unl[2]{};               // numeros de area cargados y descargados
    u32 links[kSlots][kSlotWords]{};             // tramos: 112 bytes por area
    u32 nodeBase[kSlots]{};                      // primer enlace de TODOS los nodos del area (int16 en +0x10 del nodo)
    u32 nl[kSlots][kNL]{};                       // enlaces de nodo (direcciones de nodo)
    u32 navi[kSlots][kNL]{};                     // enlaces de navegacion (direcciones de tramo, 16 bits)
    // salidas de los falsos
    u32 stopRet{};
    u32 pathCnt{}, ps[8]{};                      // DoPathSearch: contador y nodos que escribe (solo los primeros `pathCnt`, como el original)
    u32 inv{};                                   // primer nodo invalido de la ruta (8: ninguno); no viaja al JSON
    u32 fl[4]{};                                 // FindLinkBetweenNodes: direccion de tramo (16 bits) de cada llamada
    u32 bendRet[4]{}, bendHead[4]{}, bendFac[4]{}, bendW[4]{};   // DealWithBend_Racing por llamada: devuelve, rumbo, factor y que salidas escribe (bit 0 rumbo, bit 1 factor)
    u32 clipW[6]{}, clipV[6]{};                  // ClipTargetOrientationToLink por llamada: si escribe el rumbo y su valor
    u32 perc{}, weave{}, bridge{};
    u32 hf[3]{}, hfHb{};
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
    auto Sign = [&]() -> u32 { return P(50) ? 1u : 0xFFu; };                                          // +1 / -1 (los multiplicadores de sentido)
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
    auto LoadedNode = [&]() -> u32 { return c.areas[R(kSlots)] | (R(kNodes) << 16); };
    auto AnyNode = [&]() -> u32 { return R(64) | (R(20) << 16); };

    // ---- vehiculo y matriz
    for (u32 k = 0; k < 3; ++k) {
        c.mp[k] = k < 2 ? Meters(3000) : Meters(100);
        c.mr[k] = Dir(); c.mf[k] = Dir(); c.mu[k] = Dir();
        c.speed[k] = P(10) ? Sp() : B(float(int(R(193)) - 96) / 32.0f);
        c.dest[k] = Meters(3000);
    }
    c.cur = LoadedWord(); c.next = P(40) ? c.cur : LoadedWord(); c.prevL = Rand(n, nk()) & 0xFFFFu;
    c.s4 = Sign(); c.s5 = Sign(); c.s6 = Sign(); c.l7 = Small(); c.l8 = Small();
    c.mission = R(256);
    c.cruise = P(15) ? R(12) : R(256);
    c.subtype = P(25) ? 10u : R(20);
    c.timer = P(50) ? B(float(int(R(41)) - 20) / 8.0f) : Rand(n, nk());
    c.model = P(15) ? 0x21Bu : R(0x10000);
    c.f3db = Rand(n, nk()) & 0xFFu;
    c.curN = LoadedNode(); c.nextN = LoadedNode(); c.prevN = AnyNode();
    c.fieldC = Rand(n, nk()); c.scale = Rand(n, nk());

    // ---- ruta (+0x3F8) y resultado de DoPathSearch: los invalidos (area 0xFFFF) solo forman un sufijo de las cuatro primeras posiciones
    {
        // Los nodos invalidos (area 0xFFFF) forman un sufijo comun de la ruta propia y del resultado de la busqueda: desde `inv` todos lo son (la ruta se desplaza al quitar nodos y se mezcla con la busqueda)
        const u32 inv = P(10) ? R(4) : 8u;
        auto Route = [&](u32* v) {
            for (u32 i = 0; i < 8; ++i) v[i] = i >= inv ? (0xFFFFu | (R(20) << 16)) : P(60) ? LoadedNode() : AnyNode();
        };
        Route(c.pn);
        Route(c.ps);
        c.inv = inv;
        c.pcount = P(60) ? 4u + R(5) : u32(int(R(7)) - 3);     // >= 4: no hay busqueda; en otro caso la hay
        c.pathCnt = P(15) ? 0u : P(10) ? 1u : P(15) ? 2u : R(9);
    }

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
            }
        }
    }
    // ---- falsos
    c.stopRet = P(4) ? 1u : 0u;
    for (u32 k = 0; k < 4; ++k) c.fl[k] = P(25) ? 0xFFFFu : LoadedWord();
    for (u32 k = 0; k < 4; ++k) {
        c.bendRet[k] = P(8) ? 1u : 0u;
        c.bendHead[k] = P(70) ? Ang() : Sp();
        c.bendFac[k] = P(70) ? B(float(int(R(17))) / 16.0f) : Sp();
        c.bendW[k] = R(4);
    }
    for (u32 k = 0; k < 6; ++k) {
        c.clipW[k] = P(75) ? 1u : 0u;
        c.clipV[k] = P(70) ? Ang() : Sp();
    }
    c.perc = P(70) ? B(float(int(R(33))) / 16.0f) : Sp();
    c.weave = P(70) ? Ang() : Sp();
    c.bridge = P(8) ? 1u : 0u;
    for (u32 k = 0; k < 3; ++k) c.hf[k] = P(70) ? B(float(int(R(33)) - 16) / 8.0f) : Sp();
    c.hfHb = P(50) ? 1u : 0u;
    for (u32 k = 0; k < 4; ++k) c.o0[k] = Rand(n, nk());
    c.mutAt = P(25) ? R(7) : kNoMutation;
    c.mutKind = c.mutAt == kNoMutation ? 0u : 1u + R(kKinds);
    c.mutVal = P(25) ? Pick(kStackSeeds, n, nk()) : Rand(n, nk());

    // ---- sesgos por familia
    // Coloca el vehiculo a `dist` metros del primer tramo (en un angulo aleatorio): los tramos no tienen desplazamiento de carril
    auto SlotOf = [&](u32 word) -> u32 { return (word >> 10) == c.areas[0] ? 0u : (word >> 10) == c.areas[1] ? 1u : 2u; };
    auto LinkXY = [&](u32 word, float& x, float& y) {
        const u32 s = SlotOf(word), k = word & 0x3FFu;
        x = float(i16(u16(LinkByte(c, s, k, 0) | (LinkByte(c, s, k, 1) << 8)))) / 8.0f;
        y = float(i16(u16(LinkByte(c, s, k, 2) | (LinkByte(c, s, k, 3) << 8)))) / 8.0f;
    };
    auto VehicleAt = [&](float dist, bool alongSegment) {
        float lx, ly;
        LinkXY(c.cur, lx, ly);
        float ax = float(int(R(201)) - 100), ay = float(int(R(201)) - 100);
        if (alongSegment) {   // hacia el segundo tramo
            float nx, ny;
            LinkXY(c.next, nx, ny);
            ax = nx - lx; ay = ny - ly;
        }
        const float len = std::sqrt(ax * ax + ay * ay);
        if (len < 0.001f) { ax = 1.0f; ay = 0.0f; }
        const float inv = len < 0.001f ? 1.0f : 1.0f / len;
        c.mp[0] = B(std::floor((lx + ax * inv * dist) * 8.0f + 0.5f) / 8.0f);
        c.mp[1] = B(std::floor((ly + ay * inv * dist) * 8.0f + 0.5f) / 8.0f);
    };
    auto PutSegmentNear = [&] {   // el segundo tramo a 20..60 m del primero (otro area o el mismo)
        const u32 s = SlotOf(c.cur), s2 = SlotOf(c.next);
        const u32 k = c.cur & 0x3FFu, k2 = c.next & 0x3FFu;
        const int x = i16(u16(LinkByte(c, s, k, 0) | (LinkByte(c, s, k, 1) << 8)));
        const int y = i16(u16(LinkByte(c, s, k, 2) | (LinkByte(c, s, k, 3) << 8)));
        const int dx = int(R(641)) - 320, dy = int(R(641)) - 320;
        SetLink16(c, s2, k2, 0, std::clamp(x + dx, -32768, 32767));
        SetLink16(c, s2, k2, 2, std::clamp(y + dy, -32768, 32767));
    };
    auto Distance = [&]() -> float {
        static constexpr float d[] = {0.5f, 2.0f, 4.0f, 7.0f, 7.9f, 8.0f, 8.1f, 10.0f, 11.9f, 12.0f, 12.1f, 20.0f, 40.0f, 90.0f};
        return d[R(std::size(d))];
    };
    // Con el vehiculo casi quieto y de frente (velocidad 0) la distancia a lo largo del avance es cero
    switch (f) {
    case 0: break;
    case 1:    // StopCarIfNodesAreInvalid verdadero y areas descargadas
        c.stopRet = P(50) ? 1u : 0u;
        if (P(40)) c.cur = (c.unl[0] << 10) | R(1024);
        if (P(40)) c.next = (c.unl[1] << 10) | R(1024);
        break;
    case 2:    // cerca (< 8 m) con la ruta ya llena: avanza sin buscar
        c.stopRet = 0; VehicleAt(P(60) ? 2.0f : 7.0f, false); c.pcount = 4u + R(5);
        break;
    case 3:    // cerca con la ruta corta: busca (y sin ruta va al destino con mision 9)
        c.stopRet = 0; VehicleAt(P(60) ? 2.0f : 7.0f, false); c.pcount = u32(int(R(7)) - 3);
        c.pathCnt = P(35) ? 0u : P(30) ? 1u : P(30) ? 2u : R(9);
        break;
    case 4:    // 8..12 m con producto positivo
        c.stopRet = 0; VehicleAt(P(70) ? 10.0f : 11.5f, P(70));
        break;
    case 5:    // alineado: razon > 0.7
        c.stopRet = 0; PutSegmentNear(); VehicleAt(12.0f + float(R(20)), true);
        break;
    case 6:    // desalineado y tramos distintos: no avanza
        c.stopRet = 0; if (c.next == c.cur) c.next = Word(0, ((c.cur & 7u) + 1u) & 7u) == c.cur ? Word(1, 0) : Word(0, ((c.cur & 7u) + 1u) & 7u);
        VehicleAt(P(60) ? 15.0f : Distance(), false);
        break;
    case 7:    // tramos iguales: avanza aunque este lejos
        c.stopRet = 0; c.next = c.cur; VehicleAt(P(60) ? 30.0f : Distance(), false);
        break;
    case 8:    // curva: DealWithBend_Racing devuelve verdadero en alguna posicion
        c.stopRet = 0; VehicleAt(P(60) ? 15.0f : Distance(), false);
        {
            const u32 at = R(4);
            for (u32 k = 0; k < 4; ++k) c.bendRet[k] = k == at ? 1u : P(10) ? 1u : 0u;
        }
        break;
    case 9:    // sin curva: recortes de rumbo con enlaces de adelanto
        c.stopRet = 0; VehicleAt(P(60) ? 3.0f : Distance(), false);
        for (u32 k = 0; k < 4; ++k) { c.bendRet[k] = 0; c.fl[k] = P(15) ? 0xFFFFu : LoadedWord(); }
        c.pcount = 4u + R(5);
        break;
    case 10:   // puente y velocidad de crucero
        c.stopRet = 0; VehicleAt(P(50) ? 3.0f : 20.0f, false); c.bridge = P(50) ? 1u : 0u; c.cruise = P(30) ? R(14) : R(256);
        if (P(50)) for (u32 k = 0; k < 4; ++k) c.bendFac[k] = B(float(int(R(17))) / 16.0f);
        break;
    case 11:   // correccion del rumbo y normalizacion
        c.stopRet = 0; VehicleAt(P(50) ? 3.0f : 30.0f, false); c.weave = P(70) ? B(float(int(R(401)) - 200) / 8.0f) : c.weave;
        break;
    case 12:   // limite de giro segun el modulo de la velocidad
        c.stopRet = 0; VehicleAt(P(50) ? 3.0f : 20.0f, false);
        {
            const float mag = P(50) ? float(R(30)) / 32.0f : float(R(96)) / 32.0f;
            c.speed[0] = B(mag); c.speed[1] = c.speed[2] = 0;
            if (P(30)) { c.speed[0] = B(float(int(R(41)) - 20) / 32.0f); c.speed[1] = B(float(int(R(41)) - 20) / 32.0f); c.speed[2] = P(50) ? 0u : B(float(int(R(21)) - 10) / 32.0f); }
        }
        c.weave = P(80) ? B(float(int(R(41)) - 20) / 20.0f) : c.weave;
        break;
    case 13:   // velocidad deseada, gas y freno
        c.stopRet = 0; VehicleAt(P(50) ? 3.0f : 20.0f, false); c.cruise = P(25) ? R(4) : R(256);
        for (u32 k = 0; k < 4; ++k) c.bendFac[k] = P(50) ? B(float(int(R(17))) / 16.0f) : c.bendFac[k];
        c.bridge = P(10) ? 1u : 0u;
        if (P(50)) { c.mf[0] = B(1.0f); c.mf[1] = 0; c.mf[2] = 0; c.speed[0] = B(float(int(R(121)) - 10) / 8.0f); c.speed[1] = c.speed[2] = 0; }
        if (P(40)) c.weave = B(float(int(R(41)) - 20) / 20.0f);
        break;
    case 14:   // motos
        c.stopRet = 0; VehicleAt(P(50) ? 3.0f : 20.0f, false); c.subtype = P(80) ? 10u : R(20);
        c.timer = P(60) ? B(float(int(R(21)) - 10) / 8.0f) : B(0.0f); if (P(15)) c.timer = 0x80000000u;
        c.cruise = P(60) ? 12u + R(244) : c.cruise;
        if (P(50)) { c.mf[0] = B(1.0f); c.mf[1] = 0; c.mf[2] = 0; c.speed[0] = B(float(int(R(41))) / 16.0f); c.speed[1] = c.speed[2] = 0; }
        break;
    case 15:   // ruta con la busqueda y las tablas de enlaces (varias eliminaciones)
        c.stopRet = 0; VehicleAt(P(60) ? 3.0f : 20.0f, false);
        c.pcount = u32(int(R(4)) - 1);
        c.pathCnt = 1u + R(8);
        if (P(60)) c.ps[0] = c.nextN;                      // el primer nodo es el siguiente: se quita dos veces
        if (P(30)) c.pn[0] = c.nextN;
        break;
    case 16:   // especiales (NaN, infinitos, desnormales, enormes)
        {
            u32* t[] = {&c.speed[0], &c.speed[1], &c.speed[2], &c.mf[0], &c.mf[1], &c.mf[2], &c.mp[2], &c.dest[0], &c.dest[1], &c.dest[2], &c.timer, &c.weave, &c.perc, &c.hf[0], &c.hf[1],
                        &c.bendHead[0], &c.bendFac[0], &c.bendFac[1], &c.clipV[0], &c.clipV[1]};
            for (u32* p : t) if (P(12)) *p = Sp();
        }
        c.stopRet = 0;
        break;
    default:   // 17: barrido de mutaciones
        c.stopRet = 0; VehicleAt(Distance(), P(40));
        c.mutAt = R(7); c.mutKind = 1u + sweep % kKinds;
        break;
    }

    // adelante horizontal nulo (la longitud horizontal 0 deja la X normalizada en 1)
    if (P(4)) { c.mf[0] = P(50) ? 0u : 0x80000000u; c.mf[1] = P(50) ? 0u : 0x80000000u; }
    // ---- mundo de caminos: el nodo actual tras el avance (el siguiente de antes) y los candidatos a siguiente
    // Todas las areas comparten el grupo de candidatos: los 16 nodos que pueden quedar como siguiente (ruta propia, resultado de la busqueda y los valores de mutacion)
    {
        u32 pool[kPool];
        for (u32 i = 0; i < 8; ++i) { pool[i] = c.pn[i]; pool[8 + i] = c.ps[i]; }
        for (u32 s = 0; s < kSlots; ++s) {
            c.nodeBase[s] = R(9);
            for (u32 j = 0; j < kNL; ++j) { c.nl[s][j] = Rand(n, nk()) | 0x00010000u; c.navi[s][j] = P(25) ? 0xFFFFu : Word(R(kSlots), R(kLinks)); }
            for (u32 t = 0; t < kPool; ++t) { c.nl[s][c.nodeBase[s] + t] = pool[t]; c.navi[s][c.nodeBase[s] + t] = Word(R(kSlots), R(kLinks)); }
        }
    }
    // El nodo siguiente (+0x394) debe estar cargado con id < 8 (el avance lo lee); lo mismo con la mutacion que lo cambia
    {
        auto Ok = [&](u32 node) { const u32 a = node & 0xFFFFu; return (node >> 16) < kNodes && (a == c.areas[0] || a == c.areas[1] || a == c.areas[2]); };
        if (!Ok(c.nextN)) c.nextN = LoadedNode();
        if (c.mutKind == 13) c.mutVal = LoadedNode();
    }

    // ---- reglas de seguridad
    {
        if (c.mutKind == 4 || c.mutKind == 5) c.mutVal = LoadedWord();
        if (c.mutKind == 6 || c.mutKind == 7 || c.mutKind == 17) c.mutVal = Small();
        if (c.mutKind == 9) c.mutVal = P(50) ? 10u : R(20);
        if (c.mutKind == 11) c.mutVal = u32(int(R(12)) - 3);
        if (c.mutKind == 12) c.mutVal = c.ps[c.inv == 0 ? 0u : R(c.inv < 8 ? c.inv : 8u)];
        if (c.mutKind == 14 || c.mutKind == 3 || c.mutKind == 1) c.mutVal = Meters(3000);
        if (c.mutKind == 2) c.mutVal = Dir();
        if (c.mutKind == 15) c.mutVal = P(40) ? 0x21Bu : R(0x10000);
    }
    // un NaN con carga distinta del indefinido no se usa (la carga que sale depende del orden de operandos)
    auto Clean = [](u32& v) { if (IsNan(v)) v = kNan; };
    for (u32 k = 0; k < 3; ++k) { Clean(c.speed[k]); Clean(c.mp[k]); Clean(c.mr[k]); Clean(c.mf[k]); Clean(c.mu[k]); Clean(c.dest[k]); Clean(c.hf[k]); }
    Clean(c.timer); Clean(c.weave); Clean(c.perc);
    for (u32 k = 0; k < 4; ++k) { Clean(c.bendHead[k]); Clean(c.bendFac[k]); }
    for (u32 k = 0; k < 6; ++k) Clean(c.clipV[k]);
    if (c.mutAt != kNoMutation && (c.mutKind == 1 || c.mutKind == 2 || c.mutKind == 3 || c.mutKind == 10 || c.mutKind == 14)) Clean(c.mutVal);
    // el rumbo corregido se normaliza con un bucle de +-2 pi: un valor infinito o enorme no terminaria nunca (ni en el EXE ni en la DLL)
    if (IsInf(c.weave) || (!IsNan(c.weave) && std::fabs(F(c.weave)) > 2000.0f)) c.weave = Ang();
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("cur", c.cur).U("next", c.next).U("prev_l", c.prevL).U("s4", c.s4).U("s5", c.s5).U("s6", c.s6).U("l7", c.l7).U("l8", c.l8).U("mission", c.mission);
    j.U("cruise", c.cruise).U("subtype", c.subtype).U("timer", c.timer).U("model", c.model).U("f3db", c.f3db);
    j.U("cur_n", c.curN).U("next_n", c.nextN).U("prev_n", c.prevN).U("field_c", c.fieldC).U("scale", c.scale);
    j.A("pn", c.pn, 8).U("pcount", c.pcount).A("dest", c.dest, 3).A("speed", c.speed, 3);
    j.A("mr", c.mr, 3).A("mf", c.mf, 3).A("mu", c.mu, 3).A("mp", c.mp, 3);
    j.A("areas", c.areas, kSlots).A("unl", c.unl, 2);
    for (u32 s = 0; s < kSlots; ++s) { const std::string key = "links" + std::to_string(s); j.A(key.c_str(), c.links[s], kSlotWords); }
    j.A("node_base", c.nodeBase, kSlots);
    for (u32 s = 0; s < kSlots; ++s) { const std::string key = "nl" + std::to_string(s); j.A(key.c_str(), c.nl[s], kNL); }
    for (u32 s = 0; s < kSlots; ++s) { const std::string key = "navi" + std::to_string(s); j.A(key.c_str(), c.navi[s], kNL); }
    j.U("stop_ret", c.stopRet).U("path_cnt", c.pathCnt).A("ps", c.ps, 8).A("fl", c.fl, 4);
    j.A("bend_ret", c.bendRet, 4).A("bend_head", c.bendHead, 4).A("bend_fac", c.bendFac, 4).A("bend_w", c.bendW, 4).A("clip_w", c.clipW, 6).A("clip_v", c.clipV, 6);
    j.U("perc", c.perc).U("weave", c.weave).U("bridge", c.bridge).A("hf", c.hf, 3).U("hf_hb", c.hfHb).A("o0", c.o0, 4);
    j.U("mut_at", c.mutAt).U("mut_kind", c.mutKind).U("mut_val", c.mutVal).U("gate", c.gate);
    return j.End();
}
}

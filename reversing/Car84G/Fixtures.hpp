#pragma once
// Fixtures del banco Car84G (lote 84G): CCarCtrl::SteerAICarWithPhysics (EXE 0x437C20), 18432 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::SteerAICarWithPhysics  0x437C20  cdecl, un argumento (vehiculo), `ret` simple, void   (hook CCarCtrl / SteerAICarWithPhysics, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) y las familias 16 y 17 (1024 casos cada una).
// El vehiculo es una arena de 0x9A0 bytes (el EXE escribe hasta +0x998 en el Vortex) con su matriz (arena aparte, o sin matriz: la posicion y el rumbo salen de la transformacion simple +4 / +0x10), las
// regiones del juego que lee (CVehicleRecording::bUseCarAI, CTimer) y colaboradores falsos. Reglas de seguridad (el EXE no comprueba nada de esto): el vehiculo solo carece de matriz cuando la funcion no la usa
// (rama de espera con bIsStopped, o accion TEMPACT_BOOST_USE_STEERING_ANGLE sin bIsParked) y ninguna mutacion cambia la mision, las banderas de movimiento o la accion temporal en ese caso; el rumbo de la
// transformacion simple es de modulo moderado (seno y coseno de la x87 frente al ayudante de la DLL); ningun valor flotante lleva un NaN con carga distinta del indefinido.
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

namespace C84GFix {
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

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x9A0, kMatWin = 0x48;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kMatArena = Arena(kMatWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 84007u); }
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
//  1 mision (+0x3BA)   2 banderas de movimiento (+0x3DC)   3 accion temporal (+0x3BB)   4 tiempo de la accion (+0x3BC)   5 entidad objetivo (+0x41C)   6 CTimer::m_snTimeInMilliseconds
//  7 velocidad.x (+0x44)   8 adelante.x de la matriz   9 volante (+0x494)   10 banderas del vehiculo (+0x428)   11 indice de modelo (+0x22)   12 nodo actual (+0x390)   13 nodo inicial (+0x394)
//  14 CTimer::ms_fTimeStep
constexpr u32 kNoMutation = 99;
constexpr u32 kKinds = 14;

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 rec{}, useAi{};                          // id con signo de la grabacion (+0x424, byte) y el byte de CVehicleRecording::bUseCarAI[id]
    u32 mission{}, tact{}, ttime{}, tstart{}, tswitch{};   // +0x3BA, +0x3BB (bytes), +0x3BC, +0x3AC, +0x3B0
    u32 node[3]{};                               // +0x390 actual, +0x394 inicial, +0x398 final
    u32 mov{}, target{}, obst{}, vflags{};       // +0x3DC (byte), +0x41C, +0x420, +0x428
    u32 steer0{}, gas0{}, brk0{}, model{};       // +0x494, +0x49C, +0x4A0, +0x22 (word)
    u32 speed[3]{};                              // +0x44
    u32 matNull{};
    u32 mr[3]{}, mf[3]{}, mu[3]{}, mp[3]{};      // matriz: derecha, adelante, arriba, posicion
    u32 place[3]{}, heading{};                   // transformacion simple (+4, +0x10)
    u32 now{}, ts{};                             // CTimer::m_snTimeInMilliseconds, CTimer::ms_fTimeStep
    // salidas de los falsos
    u32 loaded{};                                // AreNodesLoadedForArea
    u32 om[3]{}, omHb{};                         // SteerAICarWithPhysics_OnlyMission: giro, acelerador, freno, freno de mano
    u32 findMax{};                               // FindMaxSteerAngle
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
    const u32 sweep = n < kBaseCases ? n / 16 : (n - kBaseCases) % 1024;
    u32 kk = 100;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    auto Meters = [&](int r) -> u32 { return B(float(int(R(u32(16 * r + 1))) - 8 * r) / 8.0f); };    // multiplo de 1/8 en [-r, r]
    auto Dir = [&]() -> u32 { return B(float(int(R(33)) - 16) / 16.0f); };                            // multiplo de 1/16 en [-1, 1]
    auto Sp = [&] { return Special(n, nk()); };
    auto Out = [&]() -> u32 { return P(70) ? B(float(int(R(33)) - 16) / 8.0f) : Sp(); };             // salida de un falso

    // ---- valores base
    c.rec = P(75) ? u32(0x80 + R(128)) : R(128);
    c.useAi = 1;
    {   // mision: tabla de clases (15..55) y fuera de rango
        const u32 w = R(100);
        c.mission = w < 55 ? 15 + R(41) : w < 85 ? R(70) : w < 92 ? 0x80 + R(128) : 56 + R(72);
    }
    c.target = P(50) ? 0u : (0xE0000000u | (Rand(n, nk()) & 0x1FFFFFFFu));
    {
        const u32 w = R(100);
        c.tact = w < 80 ? R(25) : w < 90 ? 25 + R(103) : 0x80 + R(128);
    }
    c.mov = (P(8) ? 1u : 0u) | (P(15) ? 2u : 0u) | (P(50) ? u32(R(64)) << 2 : 0u);
    c.obst = P(50) ? 0u : Rand(n, nk());
    c.vflags = Rand(n, nk());
    c.tstart = Rand(n, nk()); c.tswitch = Rand(n, nk());
    for (u32 k = 0; k < 3; ++k) c.node[k] = Rand(n, nk());
    c.steer0 = P(60) ? B(float(int(R(65)) - 32) / 32.0f) : Sp();
    c.gas0 = Rand(n, nk()); c.brk0 = Rand(n, nk());
    c.model = P(25) ? 0x21Bu : P(10) ? 0x21Au + 2 * R(2) : R(0x10000);
    // tiempos: now y tiempo de la accion (con envolvimiento)
    c.now = P(8) ? R(4) : P(8) ? 0xFFFFFFFFu - R(4) : Rand(n, nk());
    {
        static constexpr i32 offs[] = {0, 1, -1, 1250, 1249, 1251, 2000, -2000, 5000, -5000, 100000, -3, 3, 1300, 1200};
        const u32 w = R(100);
        c.ttime = w < 12 ? Rand(n, nk()) : c.now + u32(offs[R(std::size(offs))]);
    }
    c.ts = P(10) ? Sp() : B(float(int(R(160)) + 1) / 64.0f);
    for (u32 k = 0; k < 3; ++k) {
        c.speed[k] = P(10) ? Sp() : B(float(int(R(193)) - 96) / 32.0f);
        c.mp[k] = Meters(3000); c.place[k] = Meters(3000);
        c.mr[k] = Dir(); c.mf[k] = Dir(); c.mu[k] = Dir();
    }
    c.heading = B(float(int(R(257)) - 128) / 16.0f);
    c.matNull = 0;
    c.loaded = P(50) ? 1u : 0u;
    for (u32 k = 0; k < 3; ++k) c.om[k] = Out();
    c.omHb = P(50) ? 1u : 0u;
    c.findMax = P(70) ? B(float(int(R(41)) - 20) / 32.0f) : Sp();
    c.mutAt = P(25) ? R(4) : kNoMutation;
    c.mutKind = c.mutAt == kNoMutation ? 0u : 1u + R(kKinds);
    c.mutVal = P(25) ? Pick(kStackSeeds, n, nk()) : Rand(n, nk());

    // ---- sesgos por familia
    auto Axis = [&](float threshold) {   // adelante = (1, 0, 0) y velocidad = (k, 0, 0): el producto escalar es exactamente k (umbral exacto, +-1 ulp, cero, negativos)
        c.mf[0] = B(1.0f); c.mf[1] = 0; c.mf[2] = 0;
        const u32 t = B(threshold);
        const u32 w = R(12);
        c.speed[0] = w == 0 ? t : w == 1 ? t + 1 : w == 2 ? t - 1 : w == 3 ? 0u : w == 4 ? 0x80000000u : w == 5 ? B(-threshold) : w == 6 ? B(threshold * 2.0f) : w == 7 ? B(threshold * 0.5f) : w == 8 ? kNan : w == 9 ? 0x7F800000u : B(float(int(R(41)) - 20) / 64.0f);
        c.speed[1] = c.speed[2] = P(70) ? 0u : B(float(int(R(9)) - 4) / 64.0f);
    };
    const u32 now0 = c.now;
    switch (f) {
    case 0: break;
    case 1:    // grabacion: con id >= 0 solo actua si bUseCarAI[id] != 0
        c.rec = P(50) ? R(128) : u32(0x80 + R(128));
        c.useAi = P(50) ? 1u : 0u;
        c.mov &= ~3u;
        break;
    case 2:    // tabla de clases de la mision (mision - 15) con y sin objetivo, y mision fuera de rango
        c.mission = sweep % 3 == 0 ? u32(R(256)) : u32(15 + (sweep % 41));
        c.target = sweep % 2 ? 0u : c.target | 1u;
        c.tact = P(70) ? 0u : 2u;
        c.mov &= ~3u;
        c.mutAt = P(30) ? 0u : kNoMutation; c.mutKind = c.mutAt == kNoMutation ? 0u : 1u + R(5);
        break;
    case 3:    // bIsStopped: espera a los nodos de la zona (con y sin matriz)
        c.mov |= 1u;
        c.loaded = P(50) ? 1u : 0u;
        c.matNull = P(40) ? 1u : 0u;
        break;
    case 4:    // bIsParked: se invierte la ruta (mision distinta de 0 y de 11)
        c.mov = (c.mov & ~1u) | 2u;
        { const u32 w = R(6); c.mission = w == 0 ? 0u : w == 1 ? 11u : P(50) ? 15 + R(41) : R(70); }
        c.target = P(70) ? c.target | 1u : c.target;
        break;
    case 5:    // WAIT (1) y BRAKE (24)
        c.mov &= ~3u; c.tact = P(50) ? 1u : 24u;
        break;
    case 6:    // REVERSE (3): producto escalar velocidad . adelante contra 0.04f
        c.mov &= ~3u; c.tact = 3;
        if (P(60)) Axis(0.04f);
        break;
    case 7:    // REVERSE_LEFT (13) y REVERSE_RIGHT (14)
        c.mov &= ~3u; c.tact = P(50) ? 13u : 14u;
        break;
    case 8:    // curvas con freno de mano, giro y hacia delante (4..9)
        c.mov &= ~3u; c.tact = 4 + R(6);
        break;
    case 9:    // esquivar (10, 11, 20, 21): el borde tiempo - 1250 y el tiempo
        c.mov &= ~3u; { static constexpr u32 t[] = {10, 11, 20, 21}; c.tact = t[R(4)]; }
        break;
    case 10:   // choque frontal (19): el giro se acerca a +-0.5
        c.mov &= ~3u; c.tact = 19;
        {
            const u32 w = R(16);
            static constexpr float v[] = {0.0f, -0.0f, 0.45f, 0.4999f, 0.5f, 0.51f, -0.45f, -0.4999f, -0.5f, -0.51f, 0.001f, -0.001f, 0.48f, -0.48f, 2.0f};
            c.steer0 = w < std::size(v) ? B(v[w]) : P(50) ? kNan : Pick(kInf, n, nk());
            if (P(40)) c.ts = B(float(int(R(8)) + 1) / 32.0f);
        }
        break;
    case 11:   // REVERSE_STRAIGHT (22): producto escalar contra 0.1f
        c.mov &= ~3u; c.tact = 22;
        if (P(60)) Axis(0.1f);
        break;
    case 12:   // BOOST_USE_STEERING_ANGLE (23) con y sin matriz
        c.mov &= ~3u; c.tact = 23;
        c.matNull = P(35) ? 1u : 0u;
        break;
    case 13:   // acciones de avion (15..18) y la rama por defecto (0, 2, 12, fuera de rango)
        c.mov &= ~3u;
        { const u32 w = R(10); c.tact = w < 4 ? 15 + R(4) : w == 4 ? 0u : w == 5 ? 2u : w == 6 ? 12u : w == 7 ? 25 + R(103) : w == 8 ? u32(0x80 + R(128)) : 24u + 2 * R(2); }
        break;
    case 14:   // barrido de mutaciones tras un evento
        c.mutAt = R(4); c.mutKind = 1u + sweep % kKinds;
        c.tact = 1 + R(24);
        break;
    case 15:   // Vortex, banderas del vehiculo y salidas
        c.model = P(60) ? 0x21Bu : P(50) ? 0x21Au : 0x21Cu;
        c.mov &= ~3u;
        c.tact = P(60) ? 0u : R(25);
        break;
    case 16:   // especiales (NaN, infinitos, desnormales, enormes)
        {
            u32* t[] = {&c.speed[0], &c.speed[1], &c.speed[2], &c.mf[0], &c.mf[1], &c.mf[2], &c.mp[0], &c.mp[1], &c.mp[2], &c.steer0, &c.ts, &c.om[0], &c.om[1], &c.om[2], &c.findMax, &c.heading, &c.place[0], &c.place[1]};
            for (u32* p : t) if (P(12)) *p = Sp();
        }
        c.mov &= ~3u;
        c.tact = P(70) ? 1 + R(24) : c.tact;
        break;
    default:   // 17: tiempos con envolvimiento y degenerados
        c.mov &= ~3u;
        c.now = P(50) ? R(3000) : 0xFFFFFFFFu - R(3000);
        c.ttime = c.now + u32(P(50) ? i32(R(5000)) - 2500 : i32(R(3000)) - 1500);
        c.tact = P(80) ? 1 + R(24) : c.tact;
        if (P(30)) c.ts = 0u;
        break;
    }
    (void)now0;

    // ---- reglas de seguridad
    // Sin matriz solo en la rama de espera (bIsStopped) o con la accion 23 sin bIsParked, y sin mutaciones que cambien la mision, las banderas de movimiento o la accion temporal
    {
        const bool mutUnsafe = c.mutAt != kNoMutation && (c.mutKind == 1 || c.mutKind == 2 || c.mutKind == 3);
        const bool allowed = !mutUnsafe && ((c.mov & 1) || (c.tact == 23 && !(c.mov & 2)));
        if (!allowed) c.matNull = 0;
    }
    // un NaN con carga distinta del indefinido no se usa (la carga que sale depende del orden de operandos)
    auto Clean = [](u32& v) { if (IsNan(v)) v = kNan; };
    for (u32 k = 0; k < 3; ++k) { Clean(c.speed[k]); Clean(c.mp[k]); Clean(c.place[k]); Clean(c.mr[k]); Clean(c.mf[k]); Clean(c.mu[k]); Clean(c.om[k]); }
    Clean(c.steer0); Clean(c.ts); Clean(c.findMax); Clean(c.heading);
    if (c.mutAt != kNoMutation && (c.mutKind == 7 || c.mutKind == 8 || c.mutKind == 9 || c.mutKind == 14)) Clean(c.mutVal);
    // el rumbo de la transformacion simple: modulo moderado (seno y coseno)
    if (c.matNull && (IsInf(c.heading) || IsNan(c.heading) || std::fabs(F(c.heading)) > 100.0f)) c.heading = B(float(int(Rand(n, 77) % 257) - 128) / 16.0f);
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("rec", c.rec).U("use_ai", c.useAi).U("mission", c.mission).U("tact", c.tact).U("ttime", c.ttime).U("tstart", c.tstart).U("tswitch", c.tswitch);
    j.A("node", c.node, 3).U("mov", c.mov).U("target", c.target).U("obst", c.obst).U("vflags", c.vflags);
    j.U("steer0", c.steer0).U("gas0", c.gas0).U("brk0", c.brk0).U("model", c.model).A("speed", c.speed, 3).U("mat_null", c.matNull);
    j.A("mr", c.mr, 3).A("mf", c.mf, 3).A("mu", c.mu, 3).A("mp", c.mp, 3).A("place", c.place, 3).U("heading", c.heading).U("now", c.now).U("ts", c.ts);
    j.U("loaded", c.loaded).A("om", c.om, 3).U("om_hb", c.omHb).U("find_max", c.findMax);
    j.U("mut_at", c.mutAt).U("mut_kind", c.mutKind).U("mut_val", c.mutVal).U("gate", c.gate);
    return j.End();
}
}

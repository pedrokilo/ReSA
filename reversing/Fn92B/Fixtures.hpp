#pragma once
// Fixtures del banco Fn92B (lote 92B): 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CObjectPotentialCollisionScanner::ScanForObjectPotentialCollisionEvents   0x606890  thiscall (ECX = escaner de 0xC bytes; ped en pila), `ret 4`; void   (hook S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Familia = n % 16 (1024 casos por familia).
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

namespace F92BFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kScanWin = 0x10, kPedWin = 0x5A0, kIntelWin = 0x80, kMatWin = 0x40, kTaskWin = 0x40, kObjWin = 0x150, kModelWin = 0x40, kColWin = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 92003u); }
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
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }

// Indices de modelo del objeto (word +0x22): el EXE lo extiende con signo (movsx) y lee la tabla de punteros 0xA9B0C8; la ranura elegida apunta a una arena durante cada lado
inline constexpr std::array<u32, 8> kModelIdx{19999u, 19998u, 19997u, 19996u, 0xFFFCu, 0xFFF8u, 0xFFE0u, 0xFFF0u};
inline constexpr u32 kModelTable = 0xA9B0C8u;
inline u32 ModelSlot(u32 sel) { return kModelTable + 4u * u32(i32(i16(u16(kModelIdx[sel % 8])))); }


// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 gate{};
    // --- temporizador (imagen inicial del escaner: inicio, intervalo, arrancado (+8) y detenido (+9) con basura en el resto) y hora del juego
    u32 scan[4]{};
    u32 now{};
    // --- el ped: matriz o placement (posicion en la ubicacion activa y en la otra), tipo de ped (+0x598; 0 y 1 son el jugador)
    u32 pmat{}, ppos[3]{}, palt[3]{}, ptype{};
    // --- tareas: la mas simple en la primera consulta (estado de movimiento +8) y en la segunda (punto de destino +0xC) y sus tipos (ranura 4 de la vtabla, +0x20)
    u32 t1{}, t1type{}, t1move{}, t2{}, t2type{}, ttgt[3]{};
    // --- el objeto que devuelve el escaner (+0x4C): presente, banderas +0x140, dword +0x1C, indice de modelo (selector de kModelIdx), matriz o placement
    u32 oPresent{}, oflags{}, o1c{}, omodel{}, ohasmat{}, opos[3]{}, oalt[3]{};
    // --- el falso de GetBoundCentre (centro), el radio de la esfera delimitadora del modelo (+0x24 de la colision), los de linea de vision (resultado, basura, distancia que escribe el primero)
    u32 cz[3]{}, radius{};
    u32 los1{}, los1junk{}, dist{}, los2{}, los2junk{};
};

// ------------------------------------------------------------------------------------------------ generadores
struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 Pk(std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); }
    float Fl(float lo, float hi) { return lo + (hi - lo) * (float(R(100001)) / 100000.0f); }
    u32 W() { return Rand(n, nk()); }
};

inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
// Un float "especial": cero, desnormal, infinito, NaN (un unico patron), enorme
inline u32 Special(Gen& g) {
    const u32 r = g.R(32);
    if (r < 10) return kTiny[g.R(12)];
    if (r < 16) return kInf[g.R(4)];
    if (r < 24) return 0xFFC00000u;
    if (r < 28) return kBig[g.R(6)];
    return B(float(int(g.R(2001)) - 1000) / 7.0f);
}
inline u32 NextUp(u32 bits, int steps) {   // desplaza un float normal `steps` unidades de ultimo lugar (signo respetado)
    const bool neg = (bits >> 31) != 0;
    u32 mag = bits & 0x7FFFFFFFu;
    mag = u32(i32(mag) + (neg ? -steps : steps));
    return (bits & 0x80000000u) | (mag & 0x7FFFFFFFu);
}

// Temporizador: expired = vencido (arrancado, hora >= inicio + intervalo) con probabilidad alta; si no, estados variados (sin arrancar, detenido, basura en los booleanos, desbordes)
inline void MakeTimer(C& c, Gen& g, bool expired) {
    u32 start = g.W() % 4000000u, interval = g.R(3000), started = 1, stopped = 0;
    u32 now = start + interval + g.R(2000);
    if (!expired || g.P(8)) {
        started = g.P(70) ? 1 : (g.P(50) ? 0 : g.R(256));
        stopped = g.P(75) ? 0 : (g.P(50) ? 1 : g.R(256));
        if (g.P(15)) start = 0xFFFFFFFFu - g.R(3000);
        interval = g.P(60) ? g.R(3000) : (g.P(50) ? g.W() : 0xFFFFFFFFu - g.R(1000));
        switch (g.R(5)) {
        case 0: now = start + interval + g.R(4000); break;
        case 1: now = start + interval; break;
        case 2: now = start + interval - 1; break;
        case 3: now = g.W(); break;
        default: now = start + g.R(4000); break;
        }
    }
    c.scan[0] = start; c.scan[1] = interval;
    c.scan[2] = (started & 0xFFu) | ((stopped & 0xFFu) << 8) | (g.W() << 16);
    c.scan[3] = g.W();
    c.now = now;
}

// ------------------------------------------------------------------------------------------------ geometria comun
inline void PutPos(u32* out, double x, double y, double z) { out[0] = B(float(x)); out[1] = B(float(y)); out[2] = B(float(z)); }
inline double ToD(u32 b) { return double(F(b)); }

inline void MakePed(C& c, Gen& g, bool alwaysMatrix) {
    c.pmat = alwaysMatrix ? 1 : g.P(60);
    for (u32 k = 0; k < 3; ++k) { c.ppos[k] = B(g.Fl(-3000.0f, 3000.0f)); c.palt[k] = B(g.Fl(-3000.0f, 3000.0f)); }
}


// ------------------------------------------------------------------------------------------------ funcion 0: CObjectPotentialCollisionScanner::ScanForObjectPotentialCollisionEvents
// Prepara el camino "bueno" (peaton que no es jugador, con tareas de ir a un punto en movimiento, objeto presente con colision, cerca y con la z solapada)
inline void Good0(C& c, Gen& g, u32 goType1, u32 goType2) {
    c.ptype = 2 + g.R(5);
    c.t1 = c.t2 = 1; c.t1type = goType1; c.t2type = goType2; c.t1move = 2 + g.R(3);
    c.oPresent = 1; c.o1c |= 1u; c.oflags &= ~0x401u;
}
inline void MakeObj0(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    MakeTimer(c, g, f != 0 && f != 1);
    MakePed(c, g, false);
    const double px = ToD(c.ppos[0]), py = ToD(c.ppos[1]), pz = ToD(c.ppos[2]);
    const float  pzf = F(c.ppos[2]);
    c.ptype = g.P(80) ? g.Pk({2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 20u}) : g.Pk({0u, 1u, 0u, 1u, 0xFFFFFFFFu, 0x100u});
    auto goType = [&] { return g.P(50) ? 900u : 926u; };
    auto anyType = [&] { return g.Pk({514u, 100u, 901u, 0u, 925u, 927u, 899u, 0xFFFFFFFFu, 1000u, g.W()}); };
    c.t1 = g.P(90); c.t1type = g.P(80) ? goType() : anyType(); c.t1move = g.P(85) ? g.Pk({2u, 3u, 4u, 5u, 6u, 7u, 0u, 8u, 0xFFFFFFFFu, g.W()}) : 1u;
    c.t2 = g.P(92); c.t2type = g.P(88) ? goType() : anyType();
    for (u32 k = 0; k < 3; ++k) c.ttgt[k] = B(g.Fl(-3000.0f, 3000.0f));
    c.oPresent = g.P(92);
    c.o1c = g.W() | 1u;
    if (g.P(6)) c.o1c &= ~1u;
    c.oflags = g.W() & ~0x401u;
    if (g.P(6)) c.oflags |= 1u;
    if (g.P(6)) c.oflags |= 0x400u;
    c.omodel = g.R(8);
    c.ohasmat = g.P(60);
    // distancia al objeto: casi siempre a menos de 7.5 m (clase 0), si no entre 7.5 y 20 (clase 1) o mas lejos (clase 2)
    const u32 dcls = g.P(88) ? 0u : (g.P(60) ? 1u : 2u);
    const double dist = dcls == 0 ? double(g.R(750)) / 100.0 : (dcls == 1 ? 7.5 + double(g.R(1250)) / 100.0 : 20.0 + double(g.R(300000)) / 100.0);
    const double dz0 = double(int(g.R(4001)) - 2000) / 1000.0;
    const double hz = dist * dist - dz0 * dz0;
    const double hd = hz > 0 ? std::sqrt(hz) : 0.0;
    const double ang = double(g.R(62832)) / 10000.0;
    PutPos(c.opos, px + hd * std::cos(ang), py + hd * std::sin(ang), pz - dz0);
    for (u32 k = 0; k < 3; ++k) c.oalt[k] = B(g.Fl(-3000.0f, 3000.0f));
    c.radius = B(g.Fl(0.0f, 6.0f));
    const float rzf = F(c.radius);
    // centro del objeto (lo devuelve el falso de GetBoundCentre): con la z solapada con la del ped casi siempre (|z - zc| <= radius + 1)
    const float zoff = g.Fl(-1.0f, 1.0f);
    const bool zok = g.P(75);
    const float zc = zok ? pzf + zoff * (rzf + 1.0f) : pzf + (zoff < 0 ? -1.0f : 1.0f) * (rzf + 1.0f + g.Fl(0.0f, 5.0f));
    PutPos(c.cz, px + double(g.R(200)) / 100.0 - 1.0, py + double(g.R(200)) / 100.0 - 1.0, double(zc));
    c.los1 = g.P(20) ? 1u : 0u;
    c.los1junk = g.W();
    c.dist = B(g.P(75) ? g.Fl(0.51f, 12.0f) : g.Fl(-1.0f, 0.5f));
    c.los2 = g.P(15) ? 1u : 0u;
    c.los2junk = g.W();
    auto nearPos = [&](double span) { PutPos(c.opos, px + double(g.R(u32(span * 100))) / 100.0, py + double(g.R(u32(span * 100))) / 100.0, pz); };
    switch (f) {
    case 2:  // el jugador (o tipos cercanos a el): no escanea; con el estado de movimiento y la segunda consulta a favor
        c.ptype = g.Pk({0u, 1u, 0u, 1u, 2u, 0x100u, 0x101u, 0xFFFFFFFFu});
        c.t1 = 1; c.t1type = goType(); c.t1move = g.Pk({2u, 3u, 4u, 1u}); c.t2 = 1; c.t2type = goType();
        break;
    case 3:  // estados de movimiento y tareas: la mas simple no es de ir a un punto, sin tarea, movimiento 1 (quieto) o la segunda consulta distinta
        c.ptype = 2 + g.R(5);
        switch (g.R(6)) {
        case 0: c.t1 = 0; break;
        case 1: c.t1 = 1; c.t1type = g.Pk({100u, 514u, 901u, 925u, 927u, 0u}); break;
        case 2: c.t1 = 1; c.t1type = goType(); c.t1move = 1; break;
        case 3: c.t1 = 1; c.t1type = goType(); c.t1move = g.Pk({2u, 3u}); c.t2 = 0; break;
        case 4: c.t1 = 1; c.t1type = goType(); c.t1move = g.Pk({2u, 3u}); c.t2 = 1; c.t2type = g.Pk({100u, 514u, 901u, 925u, 927u, 0u, 0xFFFFFFFFu}); break;
        default: c.t1 = 1; c.t1type = goType(); c.t1move = g.Pk({0u, 2u, 7u, 0x80000000u}); c.t2 = 1; c.t2type = goType(); break;
        }
        break;
    case 4:  // sin objeto o con banderas que lo descartan (recogible, roto, sin colision) y banderas con basura
        Good0(c, g, goType(), goType());
        switch (g.R(6)) {
        case 0: c.oPresent = 0; break;
        case 1: c.oflags |= 1u; break;
        case 2: c.oflags |= 0x400u; break;
        case 3: c.o1c &= ~1u; break;
        case 4: c.oflags = g.W(); c.o1c = g.W(); break;
        default: c.oflags |= g.Pk({0x2u, 0x100u, 0x200u, 0x800u, 0xFFFFFBFEu}); c.oflags &= ~0x401u; break;
        }
        break;
    case 5:  // distancia al objeto en el limite de 56.25 (eje exacto 7.5 con ulps)
        Good0(c, g, goType(), goType());
        {
            const double d = ToD(NextUp(B(7.5f), int(g.R(9)) - 4));
            const u32 pick = g.R(4);
            if (pick == 3) PutPos(c.opos, px + d / std::sqrt(3.0), py + d / std::sqrt(3.0), pz + d / std::sqrt(3.0));
            else PutPos(c.opos, px + (pick == 0 ? d : 0.0), py + (pick == 1 ? d : 0.0), pz + (pick == 2 ? d : 0.0));
        }
        break;
    case 6:  // la z del peaton solapa la del objeto; con linea de vision verdadera y falsa
        Good0(c, g, goType(), goType());
        PutPos(c.opos, px + double(g.R(500)) / 100.0, py + double(g.R(500)) / 100.0, pz + double(int(g.R(2001)) - 1000) / 1000.0);
        break;
    case 7:  // la z del peaton no solapa: por debajo o por encima
        Good0(c, g, goType(), goType());
        nearPos(5.0);
        PutPos(c.cz, px, py, double(pzf) + (g.P(50) ? 1.0 : -1.0) * (double(rzf) + 1.0 + double(g.R(100000)) / 1000.0 + (g.P(50) ? 0.0 : 0.001)));
        break;
    case 8:  // limite z - 1.0 = radius + centro (ulps)
        Good0(c, g, goType(), goType());
        nearPos(5.0);
        { const float tz = (pzf - 1.0f) - rzf; c.cz[2] = NextUp(B(tz), int(g.R(9)) - 4); }
        break;
    case 9:  // limite (z + 1.0) = centro - radius (ulps)
        Good0(c, g, goType(), goType());
        nearPos(5.0);
        { const float tz = (pzf + 1.0f) + rzf; c.cz[2] = NextUp(B(tz), int(g.R(9)) - 4); }
        break;
    case 10:  // distancia que escribe la linea de vision: limite de 0.5 (ulps), cero, negativa, NaN, infinito
        Good0(c, g, goType(), goType());
        nearPos(5.0);
        PutPos(c.cz, px, py, double(pzf));
        c.los1 = 0;
        c.dist = g.P(70) ? NextUp(B(0.5f), int(g.R(9)) - 4) : g.Pk({0u, 0x80000000u, 0xFFC00000u, 0x7F800000u, 0xFF800000u, 1u, B(0.499f), B(0.501f), B(1.0f)});
        c.los2 = g.P(30);
        break;
    case 11:  // lineas de vision: todas las combinaciones con basura en los bits altos
        Good0(c, g, goType(), goType());
        nearPos(5.0);
        PutPos(c.cz, px, py, double(pzf));
        c.los1 = g.P(30); c.los2 = g.P(50);
        c.los1junk = g.P(50) ? g.W() : 0u; c.los2junk = g.P(50) ? g.W() : 0u;
        c.dist = B(g.Fl(0.6f, 10.0f));
        break;
    case 12:  // floats especiales en las posiciones, el centro, la radio y la distancia
        Good0(c, g, goType(), goType());
        for (u32 k = 0; k < 3; ++k) {
            if (g.P(14)) c.opos[k] = Special(g);
            if (g.P(10)) c.ppos[k] = Special(g);
            if (g.P(14)) c.cz[k] = Special(g);
            if (g.P(10)) c.ttgt[k] = Special(g);
        }
        if (g.P(25)) c.radius = Special(g);
        if (g.P(25)) c.dist = Special(g);
        if (g.P(50)) nearPos(5.0);
        c.los1 = g.P(20); c.los2 = g.P(15);
        break;
    case 13:  // indices de modelo con signo (movsx) y radio de la esfera delimitadora del modelo
        Good0(c, g, goType(), goType());
        c.omodel = 4 + g.R(4);
        PutPos(c.opos, px + double(g.R(500)) / 100.0, py + double(g.R(500)) / 100.0, pz + double(int(g.R(2001)) - 1000) / 1000.0);
        c.radius = B(g.Fl(0.0f, 4.0f));
        c.los1 = 0; c.dist = B(g.Fl(0.6f, 10.0f)); c.los2 = 0;
        break;
    case 14:  // desplazamientos pequenos entre el objeto y el peaton, posiciones iguales
        Good0(c, g, goType(), goType());
        PutPos(c.opos, px + double(int(g.R(2001)) - 1000) / 1000.0, py + double(int(g.R(2001)) - 1000) / 1000.0, pz + double(int(g.R(2001)) - 1000) / 1000.0);
        if (g.P(15)) { c.opos[0] = c.ppos[0]; c.opos[1] = c.ppos[1]; c.opos[2] = c.ppos[2]; }
        c.los1 = 0; c.dist = B(g.Fl(0.6f, 10.0f)); c.los2 = 0;
        break;
    case 15:  // caso tipico con exito (el evento): ninguna linea de vision libre y distancia mayor que 0.5
        Good0(c, g, goType(), goType());
        PutPos(c.opos, px + double(g.R(500)) / 100.0, py + double(g.R(500)) / 100.0, pz + double(int(g.R(2001)) - 1000) / 1000.0);
        PutPos(c.cz, px, py, double(pzf) + double(int(g.R(2001)) - 1000) / 1000.0);
        c.radius = B(g.Fl(0.5f, 4.0f));
        c.los1 = 0; c.dist = B(g.Fl(0.6f, 10.0f)); c.los2 = 0;
        break;
    default: break;
    }
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    c.gate = 1;
    MakeObj0(c, n);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    auto V = [](const u32* p, std::size_t k) { return std::vector<u32>(p, p + k); };
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack).U("gate", c.gate);
    j.A("scan", V(c.scan, 4)).U("now", c.now);
    j.U("pmat", c.pmat).A("ppos", V(c.ppos, 3)).A("palt", V(c.palt, 3)).U("ptype", c.ptype);
    j.U("t1", c.t1).U("t1type", c.t1type).U("t1move", c.t1move).U("t2", c.t2).U("t2type", c.t2type).A("ttgt", V(c.ttgt, 3));
    j.U("opresent", c.oPresent).U("oflags", c.oflags).U("o1c", c.o1c).U("omodel", c.omodel).U("ohasmat", c.ohasmat).A("opos", V(c.opos, 3)).A("oalt", V(c.oalt, 3));
    j.A("cz", V(c.cz, 3)).U("radius", c.radius).U("los1", c.los1).U("los1junk", c.los1junk).U("dist", c.dist).U("los2", c.los2).U("los2junk", c.los2junk);
    return j.End();
}
}

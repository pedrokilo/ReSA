#pragma once
// Fixtures del banco Fn92A (lote 92A): 16384 casos por funcion (16384 pares por funcion). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CPedAcquaintanceScanner::ScanForPedAcquaintances   0x607D80  thiscall (ECX = escaner; ped, entidades y numero en pila), `ret 0Ch`; void   (hook S)
//   1 CNearbyFireScanner::ScanForNearbyFires              0x603E70  thiscall (ECX = escaner; ped en pila), `ret 4`; void                       (hook S)
//   2 CSexyPedScanner::ScanForSexyPeds                    0x603BF0  thiscall (ECX = escaner; ped, entidades y numero en pila), `ret 0Ch`; void   (hook S)
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

namespace F92AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 3;
constexpr unsigned kEnts = 16;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kScanWin = 0x10, kPedWin = 0x5A0, kEntsWin = 0x40, kIntelWin = 0x80, kMatWin = 0x40, kTaskWin = 0x40, kFireWin = 0x28, kStatWin = 0x30;
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

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 gate{};
    // --- temporizador de las tres funciones (imagen inicial del escaner: inicio, intervalo, arrancado (+8) y detenido (+9) con basura en el resto) y hora del juego
    u32 scan[4]{};
    u32 now{};
    // --- funcion 0
    u32 gint{};             // dword del EXE 0x8D2358 (intervalo, 500 de serie)
    u32 allowed{};          // eax que devuelve 0x603A30 (solo cuenta AL)
    u32 count{};            // tercer argumento (tambien funcion 2)
    // --- funciones 1 y 2: el ped
    u32 pmat{}, ppos[3]{}, palt[3]{};   // el ped tiene matriz; posicion en la ubicacion activa (matriz +0x30 o placement +4) y en la otra
    // --- funcion 1: fuego y tareas
    u32 fire{}, fpos[3]{}, fsize{};
    u32 t1{}, t1type{}, t1move{}, t2{}, t2type{}, act{}, acttype{};
    // --- funcion 2: ped (matriz hacia delante, creado por, banderas +0x46C, datos de jugador, atractivo) y hasta 16 entidades
    u32 fwd[3]{}, created{}, pflags{}, pdata{}, psex{};
    u32 ePresent[kEnts]{}, eType[kEnts]{}, eSex[kEnts]{}, eFlags[kEnts]{}, eHasMat[kEnts]{}, ePos[kEnts][3]{}, eAlt[kEnts][3]{};
    u32 los{};              // bit i: resultado de la i-esima llamada a la linea de vision
    u32 losJunk{};          // basura de los bits altos del valor que devuelve la linea de vision
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

// ------------------------------------------------------------------------------------------------ funcion 0
inline void MakeScan0(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    u32 start = g.W() % 4000000u, interval = 500;
    u32 started = 1, stopped = 0;
    u32 now = start + g.R(2000);
    u32 gint = 500;
    c.allowed = g.W();
    c.count = g.Pk({16u, 16u, 16u, 0u, 1u, 15u, 17u, 0xFFFFFFFFu, 0x80000000u, g.R(100)});
    switch (f) {
    case 0:   // aleatorio
        started = g.P(70) ? 1 : (g.P(50) ? 0 : g.R(256)); stopped = g.P(75) ? 0 : (g.P(50) ? 1 : g.R(256));
        start = g.W(); interval = g.P(50) ? g.R(3000) : g.W(); now = g.P(60) ? start + g.R(4000) : g.W();
        gint = g.P(70) ? 500 : g.W();
        break;
    case 1:   // sin arrancar (se arranca con el intervalo del EXE y la hora de este fotograma)
        started = 0; stopped = g.P(20); now = g.W(); start = g.W(); interval = g.W();
        gint = g.P(50) ? 500 : g.R(100000);
        break;
    case 2:   // arrancado y sin vencer
        interval = 100 + g.R(3000); now = start + g.R(interval);
        break;
    case 3:   // vencido, 0x603A30 devuelve verdadero (AL distinto de cero, con basura arriba)
        interval = g.R(3000); now = start + interval + g.R(3000);
        c.allowed = g.Pk({1u, 0x100u + 1u, 0xFFFFFF01u, 0x80u, 0xFFu, 2u});
        break;
    case 4:   // vencido y 0x603A30 devuelve falso (AL cero, con basura arriba)
        interval = g.R(3000); now = start + interval + g.R(3000);
        c.allowed = g.Pk({0u, 0x100u, 0xFFFFFF00u, 0xDEADBE00u, 0x1234AB00u});
        break;
    case 5:   // detenido (se reinicia el inicio)
        stopped = g.Pk({1u, 1u, 2u, 0x80u, 0xFFu, 0x100u}); started = g.P(80) ? 1 : 0;
        interval = g.R(3000); now = start + g.R(6000);
        break;
    case 6:   // arrancado = 0 con detenido != 0
        started = 0; stopped = g.Pk({1u, 2u, 0xFFu}); now = g.W();
        break;
    case 7:   // justo en el limite: hora == inicio + intervalo
        interval = g.R(5000); now = start + interval;
        break;
    case 8:   // un milisegundo antes del limite
        interval = 1 + g.R(5000); now = start + interval - 1;
        break;
    case 9:   // inicio + intervalo desborda 2^32
        start = 0xFFFFFFFFu - g.R(3000); interval = g.R(6000); now = g.P(50) ? start + g.R(6000) : g.R(6000);
        break;
    case 10:  // intervalo negativo (suma sin signo) y valor negativo en la tabla del EXE
        interval = g.Pk({0xFFFFFFFFu, 0x80000000u, 0xFFFFFE0Cu, 0xFFFFFFF6u, g.W() | 0x80000000u});
        now = g.P(50) ? start + interval + g.R(3000) : start + g.R(3000);
        gint = g.P(60) ? 500 : (g.W() | 0x80000000u);
        break;
    case 11:  // booleanos con basura (se leen como bytes)
        started = g.Pk({2u, 0x80u, 0xFFu, 0x100u, 0x7Fu, 0x101u}); stopped = g.Pk({0u, 0u, 0x100u, 2u, 0x80u, 0xFFu});
        interval = g.R(3000); now = start + interval + g.R(2) * 4000u;
        break;
    case 12:  // la hora es anterior al inicio (sin signo)
        interval = g.R(3000); now = start - 1 - g.R(100000);
        break;
    case 13:  // 0x603A30 devuelve basura (AL cero o distinto de cero)
        interval = g.R(3000); now = start + interval + g.R(3000);
        c.allowed = g.Pk({0x0000FF00u, 0xFFFFFF01u, 0x00000001u, 0xABCD0080u, g.W()});
        break;
    case 14:  // valores de la tabla del EXE
        interval = g.R(3000); now = start + interval + g.R(3000);
        c.allowed = 1; gint = g.Pk({0u, 1u, 500u, 3000u, 0x7FFFFFFFu, 0xFFFFFFFFu});
        break;
    case 15:  // registros sembrados (ABI): vencido con 0x603A30 verdadero
        interval = g.R(3000); now = start + interval + g.R(3000); c.allowed = 1;
        break;
    }
    c.scan[0] = start; c.scan[1] = interval;
    c.scan[2] = (started & 0xFFu) | ((stopped & 0xFFu) << 8) | (g.W() << 16);
    c.scan[3] = g.W();
    c.now = now; c.gint = gint;
}

// ------------------------------------------------------------------------------------------------ geometria comun
inline void PutPos(u32* out, double x, double y, double z) { out[0] = B(float(x)); out[1] = B(float(y)); out[2] = B(float(z)); }
inline double ToD(u32 b) { return double(F(b)); }

inline void MakePed(C& c, Gen& g, bool alwaysMatrix) {
    c.pmat = alwaysMatrix ? 1 : g.P(60);
    for (u32 k = 0; k < 3; ++k) { c.ppos[k] = B(g.Fl(-3000.0f, 3000.0f)); c.palt[k] = B(g.Fl(-3000.0f, 3000.0f)); }
}

// ------------------------------------------------------------------------------------------------ funcion 1: CNearbyFireScanner::ScanForNearbyFires
inline void MakeFire1(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    MakeTimer(c, g, f != 0 && f != 1);
    MakePed(c, g, false);
    // clase de distancia: 0 sin fuego, 1 a menos de 4 m, 2 entre 4 y 20 m, 3 mas lejos, 4 limite de 16, 5 limite de 400
    u32 fc = f == 0 ? g.R(6) : (f == 2 ? 0 : (f == 3 ? 2 : (f == 10 ? 4 + g.R(2) : (f == 1 ? g.Pk({1u, 1u, 2u, 3u}) : 1))));
    if (f == 8 || f == 12 || f == 13 || f == 14 || f == 15) fc = g.P(85) ? 1 : g.Pk({2u, 3u, 0u});
    if (f == 9) fc = 1;
    c.fire = fc != 0;
    double dist = fc == 1 ? g.R(400) / 100.0 : (fc == 2 ? 4.0 + g.R(1600) / 100.0 : (fc == 3 ? 20.0 + g.R(300000) / 100.0 : 0.0));
    double dz = double(int(g.R(6001)) - 3000) / 1000.0;
    if (fc == 1 || fc == 2) dz = double(int(g.R(3601)) - 1800) / 1000.0;
    double hz = dist * dist - dz * dz;
    double hd = hz > 0 ? std::sqrt(hz) : 0.0;
    const double ang = double(g.R(62832)) / 10000.0;
    const double px = ToD(c.ppos[0]), py = ToD(c.ppos[1]), pz = ToD(c.ppos[2]);
    PutPos(c.fpos, px + hd * std::cos(ang), py + hd * std::sin(ang), pz + dz);
    if (fc == 4 || fc == 5) {
        // limite de 16 o de 400: dx = 4 o 20 con ulps, dy = dz = 0 (la suma es exactamente el cuadrado del eje)
        const u32 axis = B(fc == 4 ? 4.0f : 20.0f);
        const u32 dxb = NextUp(axis, int(g.R(9)) - 4);
        const double dd = ToD(dxb);
        c.fpos[0] = B(float(px + dd)); c.fpos[1] = c.ppos[1]; c.fpos[2] = c.ppos[2];
        if (g.P(40)) { c.fpos[1] = B(float(py + dd)); c.fpos[0] = c.ppos[0]; }
        if (g.P(30)) { PutPos(c.fpos, px, py, pz + ToD(NextUp(B(2.0f), int(g.R(9)) - 4))); }
    }
    if (f == 9) {
        // desnivel justo en 2.0 (con ulps), cerca en el plano
        const double dd = ToD(NextUp(B(2.0f), int(g.R(9)) - 4)) * (g.P(50) ? 1.0 : -1.0);
        PutPos(c.fpos, px + g.R(300) / 100.0, py + g.R(300) / 100.0, pz + dd);
    }
    c.fsize = g.Pk({B(0.5f), B(0.99f), B(1.0f), B(1.5f), B(2.0f), B(2.5f), B(5.0f), 0xFFC00000u, B(-1.0f), 0u, B(g.Fl(0.0f, 4.0f)), 0x3F7FFFFFu, 0x3FFFFFFFu});
    if (f == 12) c.fsize = g.Pk({B(0.0f), B(0.999f), B(1.0f), B(1.0001f), B(1.999f), B(2.0f), B(2.001f), 0xFFC00000u, 0x7F800000u, 0xFF800000u, 0x80000000u, 0x00000001u});
    // tareas: la mas simple (primera y segunda consulta), la activa y los estados de movimiento
    auto goType = [&] { return g.P(50) ? 900u : 926u; };
    auto anyType = [&] { return g.Pk({514u, 100u, 901u, 0u, 925u, 927u, 899u, 0xFFFFFFFFu, 1000u, g.W()}); };
    c.t1 = g.P(88); c.t1type = g.P(75) ? goType() : anyType(); c.t1move = g.Pk({1u, 2u, 3u, 4u, 5u, 6u, 7u, 0u, 8u, 0xFFFFFFFFu, g.W()});
    c.t2 = g.P(88); c.t2type = g.P(80) ? goType() : anyType();
    c.act = g.P(88); c.acttype = g.P(15) ? 514u : (g.P(60) ? goType() : anyType());
    switch (f) {
    case 4: case 14: case 15: c.t1 = c.t2 = 1; c.act = 1; c.t1type = goType(); c.t2type = goType(); c.acttype = g.Pk({900u, 100u, 515u, 513u}); break;
    case 5: c.act = 1; c.acttype = 514; c.t1 = c.t2 = 1; c.t1type = goType(); c.t2type = goType(); break;
    case 6: c.act = 0; c.t1 = c.t2 = 1; c.t1type = goType(); c.t2type = goType(); break;
    case 7: c.t1 = 0; c.t2 = 1; c.t2type = goType(); c.act = 1; c.acttype = g.Pk({900u, 100u}); break;
    case 8: c.t1type = g.Pk({900u, 926u, 901u, 925u, 0u, 514u}); c.t2type = g.Pk({900u, 926u, 901u, 925u, 0u, 514u}); c.t1 = c.t2 = 1; c.act = 1; c.acttype = 100; break;
    case 13: c.t1 = 1; c.t2 = g.P(50); c.t1type = goType(); c.t2type = g.P(50) ? anyType() : goType(); c.act = 1; c.acttype = 100; if (g.P(30)) { c.t1type = anyType(); c.t2type = goType(); } break;
    default: break;
    }
    if (f == 11) {
        for (u32 k = 0; k < 3; ++k) {
            if (g.P(15)) c.fpos[k] = Special(g);
            if (g.P(15)) c.ppos[k] = Special(g);
        }
        c.fire = 1; c.t1 = c.t2 = c.act = 1; c.t1type = goType(); c.t2type = goType(); c.acttype = 100;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 2: CSexyPedScanner::ScanForSexyPeds
inline void MakeSexy2(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    MakeTimer(c, g, f != 0 && f != 1);
    MakePed(c, g, true);
    // ped
    const double px = ToD(c.ppos[0]), py = ToD(c.ppos[1]), pz = ToD(c.ppos[2]);
    {
        const double a = double(g.R(62832)) / 10000.0, z = double(int(g.R(2001)) - 1000) / 1000.0, r = std::sqrt(std::max(0.0, 1.0 - z * z));
        PutPos(c.fwd, r * std::cos(a), r * std::sin(a), z);
    }
    c.created = g.Pk({0u, 1u, 2u, 2u, 3u, 0xFFu, 0x82u, 4u});
    c.pflags = g.W();
    if (g.P(80)) c.pflags &= ~0x100u;
    c.pdata = g.P(40);
    c.psex = g.Pk({0u, 0x7Fu, 0x80u, 0xFFu, 1u, 0x40u, 0xC0u, g.R(256)});
    c.count = g.Pk({16u, 16u, 16u, 16u, 8u, 1u, 0u, 3u, 15u, g.R(17)});
    for (u32 i = 0; i < kEnts; ++i) {
        c.ePresent[i] = g.P(88);
        c.eType[i] = g.P(80) ? 5u : g.Pk({0u, 1u, 4u, 6u, 15u, 0xFFFFFFFFu, g.W()});
        c.eSex[i] = g.P(60) ? ((c.psex + 1 + g.R(100)) & 0xFFu) : g.R(256);
        c.eFlags[i] = g.W();
        if (g.P(85)) c.eFlags[i] &= ~0x100u;
        c.eHasMat[i] = g.P(60);
        const double ang = double(g.R(62832)) / 10000.0, z = double(int(g.R(2001)) - 1000) / 1000.0;
        const double dist = g.P(75) ? double(g.R(9500)) / 100.0 : 95.0 + double(g.R(500000)) / 100.0;
        const double r = std::sqrt(std::max(0.0, 1.0 - z * z));
        PutPos(c.ePos[i], px + dist * r * std::cos(ang), py + dist * r * std::sin(ang), pz + dist * z);
        for (u32 k = 0; k < 3; ++k) c.eAlt[i][k] = B(g.Fl(-3000.0f, 3000.0f));
    }
    c.los = g.W() | g.W(); c.losJunk = g.W();
    if (g.P(50)) c.los = 0xFFFFFFFFu;
    switch (f) {
    case 2:   // en vehiculo y no de mision (sale) o de mision (sigue)
        c.pflags |= 0x100u; c.created = g.Pk({0u, 1u, 2u, 3u, 2u, 0xFFu});
        break;
    case 3:   // sin candidatos (cuenta 0 o todos nulos o tipo distinto)
        if (g.P(50)) c.count = g.Pk({0u, 0xFFFFFFFFu, 0x80000000u});
        else for (u32 i = 0; i < kEnts; ++i) c.ePresent[i] = g.P(20);
        break;
    case 4:   // candidatos con tipo distinto, no mas atractivos o en vehiculo
        for (u32 i = 0; i < kEnts; ++i) {
            switch (g.R(4)) {
            case 0: c.eType[i] = g.Pk({4u, 6u, 0u, 15u}); break;
            case 1: c.eSex[i] = (c.psex - g.R(3)) & 0xFFu; break;
            case 2: c.eFlags[i] |= 0x100u; break;
            default: break;
            }
        }
        break;
    case 5:   // distancia al cuadrado justo en 10000 (eje exacto con ulps)
        for (u32 i = 0; i < kEnts; ++i) if (g.P(60)) {
            const u32 axis = NextUp(B(100.0f), int(g.R(9)) - 4);
            const double d = ToD(axis);
            const u32 pick = g.R(3);
            c.ePos[i][0] = B(float(px + (pick == 0 ? d : 0.0))); c.ePos[i][1] = B(float(py + (pick == 1 ? d : 0.0))); c.ePos[i][2] = B(float(pz + (pick == 2 ? d : 0.0)));
            c.eType[i] = 5; c.eSex[i] = (c.psex + 1 + g.R(100)) & 0xFFu; c.eFlags[i] &= ~0x100u;
        }
        break;
    case 6:   // producto con el eje cerca de cero: entidad perpendicular al frente o detras
        for (u32 i = 0; i < kEnts; ++i) if (g.P(60)) {
            const double dist = 1.0 + double(g.R(8000)) / 100.0;
            const double fx = ToD(c.fwd[0]), fy = ToD(c.fwd[1]), fz = ToD(c.fwd[2]);
            double ox = -fy, oy = fx, oz = 0.0;
            const double n2 = std::sqrt(ox * ox + oy * oy);
            if (n2 > 1e-6) { ox /= n2; oy /= n2; } else { ox = 1.0; oy = 0.0; }
            const double s = g.P(50) ? 1.0 : -1.0, mix = g.P(50) ? 0.0 : double(int(g.R(2001)) - 1000) * 1e-6;
            PutPos(c.ePos[i], px + dist * (s * ox + mix * fx), py + dist * (s * oy + mix * fy), pz + dist * (mix * fz) + (g.P(30) ? double(int(g.R(2001)) - 1000) / 1000.0 : 0.0));
            c.eType[i] = 5; c.eSex[i] = (c.psex + 1 + g.R(100)) & 0xFFu; c.eFlags[i] &= ~0x100u;
        }
        break;
    case 7:   // linea de vision: algunas verdaderas, otras falsas (varios candidatos)
        c.los = g.W() | (g.P(50) ? g.W() : 0u);
        for (u32 i = 0; i < kEnts; ++i) { c.eType[i] = 5; c.eSex[i] = (c.psex + 1 + g.R(100)) & 0xFFu; c.eFlags[i] &= ~0x100u; c.ePresent[i] = 1; }
        c.count = 16;
        break;
    case 8:   // datos de jugador (se anota el evento interesante de los candidatos a menos de 100 m)
        c.pdata = g.P(70);
        for (u32 i = 0; i < kEnts; ++i) if (g.P(70)) { c.eType[i] = 5; c.eSex[i] = (c.psex + 1 + g.R(100)) & 0xFFu; c.eFlags[i] &= ~0x100u; }
        break;
    case 9:   // varios candidatos: el mas cercano antes o despues
        for (u32 i = 0; i < kEnts; ++i) { c.eType[i] = 5; c.eSex[i] = (c.psex + 1 + g.R(100)) & 0xFFu; c.eFlags[i] &= ~0x100u; c.ePresent[i] = g.P(95); }
        c.count = g.Pk({16u, 16u, 5u, 10u});
        c.los = 0xFFFFFFFFu;
        break;
    case 10:  // floats especiales en las posiciones
        for (u32 i = 0; i < kEnts; ++i) {
            c.eType[i] = 5; c.eSex[i] = (c.psex + 1 + g.R(100)) & 0xFFu; c.eFlags[i] &= ~0x100u;
            for (u32 k = 0; k < 3; ++k) if (g.P(12)) c.ePos[i][k] = Special(g);
        }
        for (u32 k = 0; k < 3; ++k) { if (g.P(10)) c.ppos[k] = Special(g); if (g.P(8)) c.fwd[k] = Special(g); }
        break;
    case 11:  // atractivo con signo: limites 127/-128 y desigualdad estricta
        c.psex = g.Pk({0x7Fu, 0x80u, 0xFFu, 0u, 0x7Eu, 0x81u});
        for (u32 i = 0; i < kEnts; ++i) { c.eType[i] = 5; c.eFlags[i] &= ~0x100u; c.eSex[i] = g.Pk({0x7Fu, 0x80u, 0xFFu, 0u, 0x7Eu, 0x81u, 1u, c.psex, (c.psex + 1) & 0xFFu, (c.psex - 1) & 0xFFu}); }
        break;
    case 12:  // creado por: 2 (mision) salta la comprobacion del vehiculo
        c.created = g.Pk({2u, 2u, 0u, 1u, 3u, 0x102u, 0xFFu});
        c.pflags |= g.P(60) ? 0x100u : 0u;
        for (u32 i = 0; i < kEnts; ++i) { c.eType[i] = 5; c.eSex[i] = (c.psex + 1 + g.R(100)) & 0xFFu; c.eFlags[i] &= ~0x100u; }
        break;
    case 13:  // banderas con el bit 8 entre basura
        c.pflags = g.W() & ~0x100u; if (g.P(30)) c.pflags |= 0x100u;
        for (u32 i = 0; i < kEnts; ++i) { c.eType[i] = 5; c.eSex[i] = (c.psex + 1 + g.R(100)) & 0xFFu; c.eFlags[i] = g.W(); if (g.P(70)) c.eFlags[i] &= ~0x100u; }
        break;
    case 14:  // numero de entidades y huecos nulos
        for (u32 i = 0; i < kEnts; ++i) { c.eType[i] = 5; c.eSex[i] = (c.psex + 1 + g.R(100)) & 0xFFu; c.eFlags[i] &= ~0x100u; c.ePresent[i] = g.P(70); }
        break;
    case 15:  // caso tipico con exito
        c.created = 2; c.pflags &= ~0x100u; c.count = 16; c.los = 0xFFFFFFFFu;
        for (u32 i = 0; i < kEnts; ++i) { c.eType[i] = 5; c.eSex[i] = (c.psex + 1 + g.R(100)) & 0xFFu; c.eFlags[i] &= ~0x100u; c.ePresent[i] = g.P(90); }
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
    if (fn == 0) MakeScan0(c, n);
    else if (fn == 1) MakeFire1(c, n);
    else MakeSexy2(c, n);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    auto V = [](const u32* p, std::size_t k) { return std::vector<u32>(p, p + k); };
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack).U("gate", c.gate);
    j.A("scan", V(c.scan, 4)).U("now", c.now);
    if (c.fn == 0) j.U("gint", c.gint).U("allowed", c.allowed).U("count", c.count);
    if (c.fn >= 1) j.U("pmat", c.pmat).A("ppos", V(c.ppos, 3)).A("palt", V(c.palt, 3));
    if (c.fn == 1) {
        j.U("fire", c.fire).A("fpos", V(c.fpos, 3)).U("fsize", c.fsize).U("t1", c.t1).U("t1type", c.t1type).U("t1move", c.t1move).U("t2", c.t2).U("t2type", c.t2type).U("act", c.act).U("acttype", c.acttype);
    }
    if (c.fn == 2) {
        j.U("count", c.count).A("fwd", V(c.fwd, 3)).U("created", c.created).U("pflags", c.pflags).U("pdata", c.pdata).U("psex", c.psex);
        j.A("epresent", V(c.ePresent, kEnts)).A("etype", V(c.eType, kEnts)).A("esex", V(c.eSex, kEnts)).A("eflags", V(c.eFlags, kEnts)).A("ehasmat", V(c.eHasMat, kEnts));
        j.A("epos", V(&c.ePos[0][0], 3 * kEnts)).A("ealt", V(&c.eAlt[0][0], 3 * kEnts)).U("los", c.los).U("losjunk", c.losJunk);
    }
    return j.End();
}
}

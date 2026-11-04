#pragma once
// Fixtures del banco Fn92C (lote 92C): 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CAttractorScanner::ScanForAttractors   0x6060A0  thiscall (ECX = escaner de 0x90 bytes; ped en pila), `ret 4`; void   (hook S)
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

namespace F92CFix {
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
constexpr std::size_t kScanWin = 0x90, kPedWin = 0x5A0, kIntelWin = 0x80, kMatWin = 0x40, kTaskWin = 0x40, kEffWin = 0x40, kEntWin = 0x40;
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

constexpr unsigned kEffects = 64;     // CScripted2dEffects::NUM_SCRIPTED_2D_EFFECTS

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 gate{};
    // --- imagen inicial del escaner: dword +0 (el byte bajo es la bandera), temporizador (inicio +4, intervalo +8, arrancado/detenido +0xC con basura), hora del juego,
    //     efecto en uso (+0x10: 0 nulo, 1 el efecto que devuelve el falso, 2 otro puntero) y dword +0x14 (0 cero, 1 la entidad, 2 otro valor)
    u32 s0{}, scan[4]{}, now{}, inuse{}, f14{}, arr[30]{};   // arr = las tres tablas de 10 dwords (+0x18..+0x90)
    // --- el ped: matriz o placement (posicion en la activa y en la otra), banderas +0x46C, indice de modelo (word +0x22, con signo) y tipo de ped (+0x598)
    u32 pmat{}, ppos[3]{}, palt[3]{}, pflags{}, pmodel{}, ptype{};
    // --- tarea activa (presente, tipo en +0x20 de la ranura 4 de la vtabla)
    u32 act{}, acttype{};
    // --- efectos de script: bytes de CScripted2dEffects::ms_activated, y por cada uno activado su lista de usuarios (cuatro modelos y dword +0x20 con la bandera en el byte bajo);
    //     ulmask = resultado (AL) de la i-esima llamada a 0x5FE960
    u32 actv[kEffects]{}, ut[kEffects][4]{}, uf[kEffects]{}, ulmask{}, uljunk{};
    // --- resultados de los falsos: 0x600180 (efecto: 0 nulo, 1 arena; entidad: 0 nula, 1 arena, 2 otro valor), byte de tipo del efecto (+0x34, con basura arriba), 0x603570 (0 el ped, 1 nulo, 2 otro),
    //     valor devuelto por CEventGroup::Add
    u32 resEffect{}, resEntity{}, efType{}, user{}, addRet{};
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



// ------------------------------------------------------------------------------------------------ funcion 0: CAttractorScanner::ScanForAttractors
inline u32 SExt16(u32 v) { return u32(i32(i16(u16(v)))); }
// Efectos activados: hasta `maxCount` entradas con su lista de usuarios
inline void Actives(C& c, Gen& g, u32 maxCount, u32 listPct) {
    const u32 count = g.R(maxCount + 1);
    for (u32 k = 0; k < count; ++k) {
        const u32 idx = g.R(kEffects);
        c.actv[idx] = g.P(85) ? 1u : g.Pk({2u, 0x80u, 0xFFu, 0x7Fu, 0x10u});
        c.uf[idx] = (g.P(listPct) ? g.Pk({1u, 1u, 2u, 0x80u, 0xFFu}) : 0u) | (g.P(50) ? g.W() & 0xFFFFFF00u : 0u);
        for (u32 j = 0; j < 4; ++j) c.ut[idx][j] = g.P(40) ? 0xFFFFFFFFu : (g.P(30) ? SExt16(c.pmodel) : g.R(20000));
    }
}
inline void MakeAttr0(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    MakeTimer(c, g, f != 0 && f != 1 && f != 5);
    MakePed(c, g, false);
    const double px = ToD(c.ppos[0]), py = ToD(c.ppos[1]);
    c.s0 = (g.P(92) ? g.Pk({1u, 1u, 2u, 0x80u, 0xFFu, 0x7Fu}) : 0u) | (g.P(50) ? g.W() & 0xFFFFFF00u : 0u);
    c.inuse = g.R(3); c.f14 = g.R(3);
    for (u32 k = 0; k < 30; ++k) c.arr[k] = g.P(25) ? g.Pk({0u, 0x41C80000u, 0x43610000u, 0xFFFFFFFFu}) : g.W();
    c.pflags = g.W();
    if (g.P(88)) c.pflags &= ~0x100u;
    c.pmodel = g.P(60) ? g.R(20000) : g.Pk({0xFFFFu, 0x8000u, 0x7FFFu, 0u, 1u, 0xFFFF0000u | g.R(20000), g.W()});
    c.ptype = g.P(80) ? g.R(30) : g.Pk({0xFFFFFFFFu, 0x100u, g.W()});
    c.act = g.P(85); c.acttype = g.P(12) ? 233u : g.Pk({232u, 234u, 0u, 100u, 0xE9u + 0x100u, 0xFFFFFFFFu, g.W()});
    Actives(c, g, 6, 55);
    c.ulmask = g.W(); c.uljunk = g.W();
    c.resEffect = g.P(82); c.resEntity = g.Pk({1u, 1u, 1u, 0u, 2u});
    c.efType = g.P(35) ? 7u : g.Pk({0u, 1u, 3u, 5u, 6u, 8u, 9u, 0xFFu, 0x80u, 0x107u, 0xFFFFFF07u}) ;
    if (g.P(40)) c.efType |= (g.W() & 0xFFFFFF00u);
    c.user = g.Pk({0u, 0u, 0u, 1u, 2u});
    c.addRet = g.P(60) ? g.Pk({1u, 0x12345678u, 0xFFFFFFFFu}) : 0u;
    switch (f) {
    case 2:   // escaner apagado (byte +0 cero, con basura arriba) o encendido con basura
        c.s0 = g.P(50) ? (g.W() & 0xFFFFFF00u) : g.Pk({1u, 0x101u, 0xFFFFFF01u, 0x80u, 0xFFu, 2u});
        break;
    case 3:   // en vehiculo (bit 8 de +0x46C) con el resto de bits en basura
        c.s0 = 1; c.pflags = g.W(); if (g.P(80)) c.pflags |= 0x100u; else c.pflags &= ~0x100u;
        break;
    case 4:   // tarea activa: 233 (usar efecto) aplaza el escaneo; otros tipos o sin tarea siguen
        c.s0 = 1; c.pflags &= ~0x100u; c.act = g.P(88); c.acttype = g.P(55) ? 233u : g.Pk({232u, 234u, 0u, 100u, 0x1E9u, 0xFFFFFFE9u, 0xE9u});
        break;
    case 5:   // temporizador sin vencer o justo en el limite (hora == inicio + intervalo)
        c.s0 = 1; c.pflags &= ~0x100u; c.act = 0;
        break;
    case 6:   // posicion del ped en el limite entre celdas: x = 50 * m +- 15 con ulps (la celda cambia al cruzar)
        c.s0 = 1; c.pflags &= ~0x100u; c.act = 0;
        for (u32 k = 0; k < 2; ++k) {
            const double m = double(int(g.R(121)) - 60);
            const double base = 50.0 * m + (g.P(50) ? 15.0 : -15.0);
            const u32 b = NextUp(B(float(base)), int(g.R(9)) - 4);
            if (g.P(85)) c.ppos[k] = b;
        }
        c.resEffect = 0;
        break;
    case 7:   // posiciones extremas: fuera del mapa, en el borde, NaN e infinitos, grandes (la conversion a 64 bits envuelve)
        c.s0 = 1; c.pflags &= ~0x100u; c.act = 0;
        for (u32 k = 0; k < 3; ++k) {
            if (g.P(55)) c.ppos[k] = g.Pk({0x7F800000u, 0xFF800000u, 0xFFC00000u, B(3000.0f), B(-3000.0f), B(2985.0f), B(-2985.0f), B(2999.9f), B(-3014.9f), B(1.0e10f), B(-1.0e10f), B(1.0e18f), B(-1.0e18f),
                                           B(9.3e18f), B(-9.3e18f), B(2.0e19f), 0x7F7FFFFFu, 0xFF7FFFFFu, 0u, 0x80000000u, 1u, B(float(double(g.R(2u)) * 4294967296.0 * 50.0 + double(g.R(120)) * 50.0))});
        }
        c.resEffect = 0;
        break;
    case 8:   // efectos activados con y sin lista de usuarios: modelo en la lista, tipo de ped admitido o no
        c.s0 = 1; c.pflags &= ~0x100u; c.act = 0;
        Actives(c, g, 16, 70);
        break;
    case 9:   // modelo con signo (0xFFFF = -1 como el valor por defecto de las listas) y bytes de activado con basura
        c.s0 = 1; c.pflags &= ~0x100u; c.act = 0;
        c.pmodel = g.Pk({0xFFFFu, 0xFFFE0001u, 0x8000u, 0xFFFFu, 0x7FFFu, 0x1FFFFu});
        for (u32 i = 0; i < kEffects; ++i) c.actv[i] = 0;
        Actives(c, g, 10, 80);
        for (u32 i = 0; i < kEffects; ++i) if (c.actv[i]) for (u32 j = 0; j < 4; ++j) if (g.P(35)) c.ut[i][j] = g.P(50) ? SExt16(c.pmodel) : (SExt16(c.pmodel) & 0xFFFFu);
        break;
    case 10:  // efecto en uso: nulo, el mismo y la misma entidad (sale), mismo efecto con otra entidad, otro efecto
        c.s0 = 1; c.pflags &= ~0x100u; c.act = 0; c.resEffect = g.P(90);
        c.inuse = g.R(3); c.f14 = g.R(3); c.resEntity = g.R(3);
        if (g.P(40)) { c.inuse = 1; c.f14 = c.resEntity; }
        break;
    case 11:  // tipo 7 (script): 0x603570 devuelve el ped, nulo u otro; Add devuelve cero o no
        c.s0 = 1; c.pflags &= ~0x100u; c.act = 0; c.resEffect = 1; c.efType = g.P(85) ? 7u : g.Pk({6u, 8u, 0x107u});
        if (g.P(30)) c.efType |= (g.W() & 0xFFFFFF00u);
        c.inuse = g.P(70) ? 0u : g.R(3); c.f14 = g.R(3);
        break;
    case 12:  // el resto de tipos (todos los bytes): CEventAttractor
        c.s0 = 1; c.pflags &= ~0x100u; c.act = 0; c.resEffect = 1; c.efType = g.R(256) | (g.P(40) ? (g.W() & 0xFFFFFF00u) : 0u);
        c.inuse = 0; c.f14 = g.R(3);
        break;
    case 13:  // valor que devuelve Add: cero, un puntero, basura
        c.s0 = 1; c.pflags &= ~0x100u; c.act = 0; c.resEffect = 1; c.efType = g.P(50) ? 7u : g.R(10); c.user = 0;
        c.addRet = g.Pk({0u, 1u, 0x80000000u, 0xFFFFFFFFu, 0x100u, g.W(), 0u});
        c.inuse = 0;
        break;
    case 14:  // posicion con floats especiales con el temporizador vencido
        c.s0 = 1; c.pflags &= ~0x100u; c.act = 0;
        for (u32 k = 0; k < 3; ++k) if (g.P(35)) c.ppos[k] = Special(g);
        c.resEffect = g.P(50);
        break;
    case 15:  // caso tipico con exito: efecto distinto del que esta en uso, 0x603570 devuelve el ped o no es de script, Add devuelve distinto de cero
        c.s0 = 1; c.pflags &= ~0x100u; c.act = g.P(30); c.acttype = 100; c.resEffect = 1; c.resEntity = g.Pk({1u, 1u, 2u});
        c.efType = g.P(40) ? 7u : g.R(7); c.user = 0; c.addRet = 1; c.inuse = g.P(50) ? 0u : 2u; c.f14 = g.R(3);
        break;
    default: break;
    }
    (void)px; (void)py;
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    c.gate = 1;
    MakeAttr0(c, n);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    auto V = [](const u32* p, std::size_t k) { return std::vector<u32>(p, p + k); };
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack).U("gate", c.gate);
    j.U("s0", c.s0).A("scan", V(c.scan, 4)).U("now", c.now).U("inuse", c.inuse).U("f14", c.f14).A("arr", V(c.arr, 30));
    j.U("pmat", c.pmat).A("ppos", V(c.ppos, 3)).A("palt", V(c.palt, 3)).U("pflags", c.pflags).U("pmodel", c.pmodel).U("ptype", c.ptype);
    j.U("act", c.act).U("acttype", c.acttype);
    j.A("actv", V(c.actv, kEffects));
    // solo las listas de usuarios de los efectos activados (el resto no se lee): [indice, cuatro modelos, dword +0x20]
    std::vector<u32> ul;
    for (u32 i = 0; i < kEffects; ++i) if (c.actv[i] & 0xFFu) { ul.push_back(i); for (u32 k = 0; k < 4; ++k) ul.push_back(c.ut[i][k]); ul.push_back(c.uf[i]); }
    j.A("ul", ul).U("ulmask", c.ulmask).U("uljunk", c.uljunk);
    j.U("reseffect", c.resEffect).U("resentity", c.resEntity).U("eftype", c.efType).U("user", c.user).U("addret", c.addRet);
    return j.End();
}
}

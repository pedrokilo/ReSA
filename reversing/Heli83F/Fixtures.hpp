#pragma once
// Fixtures del banco Heli83F (lote 83F): CHeli::ProcessControl (EXE 0x6C7050), 18432 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CHeli::ProcessControl  0x6C7050  thiscall virtual, ECX = this, sin argumentos, `ret`, void (hook CHeli / ProcessControl, tipo V: RH_ScopedVMTInstall)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) y las familias 16 y 17 (1024 casos cada una).
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

namespace H83FFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;    // familias 0..15
constexpr unsigned kExtraFamilies = 2;    // familias 16..17, 1024 casos cada una
constexpr unsigned kCases = kBaseCases + kExtraFamilies * 1024;
constexpr unsigned kFamilies = 16 + kExtraFamilies;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0xA20, kMatWin = 0x48, kDrvWin = 0x5A0, kPvWin = 0x5A0, kEntWin = 0x60, kPlayerWin = 0x490, kDataWin = 0x10, kWantedWin = 0x40, kFxWin = 0x10;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kMatArena = Arena(kMatWin), kDrvArena = Arena(kDrvWin), kPvArena = Arena(kPvWin), kEntArena = Arena(kEntWin), kPlayerArena = Arena(kPlayerWin),
                      kDataArena = Arena(kDataWin), kWantedArena = Arena(kWantedWin), kFxArena = Arena(kFxWin);

constexpr u32 kTimeStep = 0xB7CB5C;                       // CTimer::ms_fTimeStep
constexpr u32 kNowMs = 0xB7CB84, kPrevMs = 0xB7CB78;      // CTimer::m_snTimeInMilliseconds y m_snPreviousTimeInMilliseconds
constexpr u32 kEventsFlag = 0xC0B184;                     // byte: bit 0 permite el aviso de eventos de interes del final
constexpr u32 kPadsBase = 0xB73458, kPadSize = 0x134, kPadsBytes = 2 * 0x134;
constexpr u32 kAudioEngine = 0xB6BC90, kInterestingEvents = 0xC0B058;

// Palabras del helicoptero que la funcion escribe y el banco lee y compara (la inicial de cada una la fija el caso)
inline constexpr std::array<u32, 22> kOffs{0x504, 0x580, 0x984, 0x9C0, 0x9C4, 0x9C8, 0x9CC, 0x9D0, 0x9D4, 0x9D8, 0x9DC, 0x9E0, 0x9E4, 0x9E8, 0x9EC, 0x9F0, 0x9F4, 0x9F8, 0x9FC, 0xA00, 0xA04, 0xA08};

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 83095u); }
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
inline constexpr std::array<u32, 4> kNan{0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
inline constexpr std::array<u32, 14> kUnit{0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x40000000u, 0xC0000000u, 0x3F7FFFFFu, 0xBF7FFFFFu, 0x3F800001u, 0xBF800001u, 0x3DCCCCCDu, 0x3F99999Au, 0x3FA66666u, 0x41200000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
inline u32 Ulps(u32 bits, int k) {
    const u32 sign = bits & 0x80000000u;
    i32 mag = i32(bits & 0x7FFFFFFFu) + k;
    if (mag < 0) mag = 0;
    if (mag > 0x7F7FFFFF) mag = 0x7F7FFFFF;
    return sign | u32(mag);
}
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Un float: modo 0 moderado, 1 pequeno, 2 grande, 3 especial (cero, desnormal, inf, NaN), 4 unitario, 5 mezcla
inline u32 FloatOf(u32 c, u32 k, u32 mode) {
    const u32 r = Rand(c, k) % 32;
    switch (mode) {
    case 0: return r == 0 ? B(0.0f) : RandFloat(c, k + 2, -4, 6);
    case 1: return r < 3 ? Pick(kTiny, c, k + 1) : RandFloat(c, k + 2, -80, -20);
    case 2: return r < 3 ? Pick(kBig, c, k + 1) : RandFloat(c, k + 2, 20, 120);
    case 3:
        if (r < 8) return Pick(kTiny, c, k + 1);
        if (r < 13) return Pick(kInf, c, k + 1);
        if (r < 21) return Pick(kNan, c, k + 1);
        if (r < 24) return Pick(kBig, c, k + 1);
        return RandFloat(c, k + 2, -20, 30);
    case 4: return r < 4 ? RandFloat(c, k + 2, -2, 2) : Pick(kUnit, c, k + 1);
    default: {
        const u32 m = Rand(c, k + 3) % 5;
        return FloatOf(c, k + 5, m == 4 ? 3u : m);
    }
    }
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{}, rseed{};          // registros sembrados en la entrada y estado inicial del rand() del EXE
    // EAX que devuelven los falsos booleanos (AL cuenta; el resto es basura una de cada cuatro veces)
    u32 horn{}, norain{}, nopolice{}, los{};     // CPad::HornJustDown, CCullZones::PlayerNoRain, CCullZones::NoPolice, CWorld::GetIsLineOfSightClear
    u32 pvKind{}, pvSub{}, peKind{}, apKind{};   // FindPlayerVehicle (0 nulo, 1 arena) y su +0x594; FindPlayerEntity y objetivo del piloto automatico (0 nulo, 1 entidad, 2 CPlayerPed, 3 vehiculo)
    // helicoptero
    u32 fl428{}, fl40{}, mission{}, status{}, model{}, hf988{}, drvKind{}, drvType{}, fire{}, matNull{}, heading{};
    u32 hpos[3]{}, mpos[3]{}, mr[3]{}, mf[3]{}, mu[3]{}, ms[3]{};
    u32 iw[22]{};                                // palabras iniciales de kOffs (iw[1] no se usa: el puntero 0x580 lo fija `fire`)
    u32 nsw{}, swt[4]{}, a11{};                  // +0x9B9, +0x9BA..0x9BD y +0xA11 (bytes)
    // objetivo (el mismo contenido en las arenas de entidad, CPlayerPed y vehiculo del jugador)
    u32 tpos[3]{}, tmatNull{}, tmpos[3]{}, tspd[3]{};
    // globales
    u32 now{}, prev{}, ts{}, ev184{};
    u32 wlevel{}, wflags{};                      // CWanted +0x2C y +0x1E
    // salidas de los falsos (por orden de llamada; las llamadas de mas repiten la ultima)
    u32 nrm[3]{}, sw[8][3]{}, mp[5][3]{}, m3o[4][3]{};
};

inline void Split3(u32 c, u32 k, double len, double out[3]) {
    double a = double(Rand(c, k) % 2001) / 1000.0 - 1.0;
    double b = double(Rand(c, k + 1) % 2001) / 1000.0 - 1.0;
    double d = double(Rand(c, k + 2) % 2001) / 1000.0 - 1.0;
    double m = a * a + b * b + d * d;
    if (m < 1e-6) { a = 1.0; b = 0.0; d = 0.0; m = 1.0; }
    const double s = len / std::sqrt(m);
    out[0] = a * s; out[1] = b * s; out[2] = d * s;
}

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    const u32 f = c.family;
    auto fl = [&](u32 k, u32 mode = 0u) { return FloatOf(n, k, mode); };
    auto vec3 = [&](u32* dst, u32 k, double len) { double d[3]; Split3(n, k, len, d); for (int i = 0; i < 3; ++i) dst[i] = B(float(d[i])); };
    auto logmag = [&](u32 k, double lo, double hi) { return std::pow(10.0, std::log10(lo) + double(Rand(n, k) % 2001) / 2000.0 * (std::log10(hi) - std::log10(lo))); };
    auto sgn = [&](u32 k) { return Rand(n, k) % 2 ? 1.0 : -1.0; };
    auto pct = [&](u32 k, u32 p) { return Rand(n, k) % 100 < p; };
    auto uni = [&](u32 k, double lo, double hi) { return lo + (hi - lo) * double(Rand(n, k) % 100001) / 100000.0; };
    auto fv = [](double d) { return B(float(d)); };
    // booleano (AL): 0, 1 o un byte cualquiera; el resto de EAX es basura una de cada cuatro veces
    auto flag = [&](u32 k, u32 on) -> u32 {
        u32 v = pct(k, on) ? (pct(k + 1, 80) ? 1u : 1u + Rand(n, k + 2) % 255u) : 0u;
        if (pct(k + 3, 25)) v |= Rand(n, k + 4) & 0xFFFFFF00u;
        return v;
    };
    auto pos = [&] { return c.matNull ? c.hpos : c.mpos; };   // posicion del helicoptero (matriz o placement)
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n); c.rseed = Rand(n, 3);
    c.horn = flag(5, 10); c.norain = flag(6, 6); c.nopolice = flag(7, 20); c.los = flag(8, 55);
    c.pvKind = pct(10, 60) ? 1u : 0u;
    {
        static constexpr std::array<u32, 6> subs{0u, 1u, 2u, 5u, 6u, 0xFFFFFFFFu};
        c.pvSub = pct(11, 22) ? (pct(12, 50) ? 3u : 4u) : subs[Rand(n, 13) % subs.size()];
    }
    c.peKind = pct(14, 10) ? 0u : 1u + Rand(n, 15) % 3u;
    c.apKind = pct(16, 20) ? 0u : 1u + Rand(n, 17) % 3u;
    // ---- helicoptero
    c.fl428 = Rand(n, 20) & 0xFFu;
    if (pct(21, 70)) c.fl428 |= 0x10u; else c.fl428 &= ~0x10u;
    c.fl40 = Rand(n, 22) & ~0x20000100u;
    {
        const u32 r = Rand(n, 23) % 100;
        if (r < 50) c.mission = 0x28u;
        else if (r < 65) c.mission = 0x27u;
        else { c.mission = Rand(n, 24) & 0xFFu; if (c.mission == 0x28u || c.mission == 0x27u) c.mission = 0x25u; }
    }
    c.status = pct(25, 55) ? Rand(n, 26) % 8u : (Rand(n, 26) & 0xFFu);
    c.model = pct(27, 35) ? 0x1F1u : (Rand(n, 28) & 0xFFFFu);
    if (c.model == 0x1F1u && !pct(27, 35)) c.model = 0x1F2u;
    c.hf988 = Rand(n, 29) & 0xFFu;
    if (pct(30, 60)) c.hf988 |= 2u; else c.hf988 &= ~2u;
    c.drvKind = pct(31, 65) ? 1u : 0u;
    c.drvType = pct(32, 35) ? 1u : (pct(33, 40) ? 0u : Rand(n, 34) % 20u);
    c.fire = pct(35, 45) ? 1u : 0u;
    c.matNull = 0u;
    c.heading = fl(36, 0u);
    for (int i = 0; i < 3; ++i) { c.hpos[i] = fv(sgn(40 + i) * logmag(43 + i, 0.5, 3000.0)); c.mpos[i] = fv(sgn(50 + i) * logmag(53 + i, 0.5, 3000.0)); }
    vec3(c.mr, 60, 1.0); vec3(c.mf, 64, 1.0); vec3(c.mu, 68, 1.0);
    if (pct(72, 80)) c.mu[2] = fv(0.5 + double(Rand(n, 73) % 500) / 1000.0);
    vec3(c.ms, 74, logmag(77, 0.001, 1.0));
    // ---- tiempos y estado inicial de los campos escritos
    c.now = 200000u + Rand(n, 80) % 300000000u;
    if (pct(81, 4)) c.now = 0xFFFFF000u + Rand(n, 82) % 0x1000u;
    c.prev = pct(83, 85) ? c.now - Rand(n, 84) % 200u : c.now + Rand(n, 84) % 50u;
    u32 delta = pct(85, 70) ? Rand(n, 86) % 1001u : 1001u + Rand(n, 87) % 5000u;
    c.ts = fv(0.5 + double(Rand(n, 88) % 3000) / 1000.0);
    c.ev184 = (Rand(n, 89) & 0xFEu) | (pct(90, 50) ? 1u : 0u);
    c.wlevel = pct(91, 95) ? Rand(n, 92) % 7u : (pct(93, 50) ? 7u + Rand(n, 94) % 100u : Rand(n, 94));
    c.wflags = pct(95, 75) ? (Rand(n, 96) & 0xF8u) : (Rand(n, 96) & 0xFFu);
    c.nsw = pct(97, 50) ? 0u : 1u + Rand(n, 98) % 4u;
    for (int i = 0; i < 4; ++i) c.swt[i] = pct(100 + i, 45) ? 0u : (pct(104 + i, 6) ? 1u + Rand(n, 108 + i) % 255u : 1u + Rand(n, 112 + i) % 3u);
    c.a11 = pct(116, 40) ? 1u : (pct(117, 4) ? 2u + Rand(n, 118) % 254u : 0u);
    for (int i = 0; i < 3; ++i) {
        c.tpos[i] = fv(sgn(120 + i) * logmag(123 + i, 0.5, 400.0));
        c.tmpos[i] = fv(sgn(126 + i) * logmag(129 + i, 0.5, 400.0));
        c.tspd[i] = fv(sgn(132 + i) * logmag(135 + i, 0.001, 2.0));
    }
    c.tmatNull = pct(138, 25) ? 1u : 0u;
    vec3(c.nrm, 140, 1.0);
    for (int k = 0; k < 8; ++k) vec3(c.sw[k], 150 + 4 * k, logmag(182 + k, 0.5, 12.0));
    for (int k = 0; k < 5; ++k) vec3(c.mp[k], 200 + 4 * k, logmag(222 + k, 1.0, 60.0));
    for (int k = 0; k < 4; ++k) vec3(c.m3o[k], 230 + 4 * k, logmag(250 + k, 0.01, 2.0));
    for (int i = 0; i < 22; ++i) c.iw[i] = Rand(n, 260 + i);
    c.iw[2] = fl(283, 4u);
    c.iw[15] = c.now - delta;
    c.iw[20] = c.now - Rand(n, 284) % 9000u;
    c.iw[21] = c.now + Rand(n, 285) % 20000u - 5000u;
    c.iw[19] = fl(286, 4u);
    // ---- el historial de 6 muestras (X: iw[3..8], Y: iw[9..14]): alrededor de un punto a distancia D del helicoptero; el objetivo a distancia E de ese punto
    auto place = [&](double D, double E) {
        const double px = F(pos()[0]), py = F(pos()[1]);
        const double a = double(Rand(n, 290) % 6283) / 1000.0, b = double(Rand(n, 291) % 6283) / 1000.0;
        const double ix = px + D * std::cos(a), iy = py + D * std::sin(a);
        for (int i = 0; i < 6; ++i) { c.iw[3 + i] = fv(ix); c.iw[9 + i] = fv(iy); }
        double ax = ix + E * std::cos(b), ay = iy + E * std::sin(b);
        u32* t = c.tmatNull ? c.tpos : c.tmpos;
        t[0] = fv(ax); t[1] = fv(ay);
    };
    auto placeRandom = [&] {
        const u32 r = Rand(n, 292) % 100;
        const double D = r < 20 ? uni(293, 60.0, 400.0) : r < 45 ? uni(293, 40.0, 60.0) : uni(293, 0.0, 40.0);
        const double E = pct(294, 65) ? uni(295, 0.0, 7.0) : uni(295, 7.0, 40.0);
        place(D, E);
    };
    placeRandom();
    auto nearOk = [&] { place(pct(296, 15) ? 40.0 + 2.0 * uni(297, 0.0, 1.0) : uni(297, 0.0, 38.0), pct(298, 85) ? uni(299, 0.0, 6.9) : uni(299, 6.9, 30.0)); };
    // retardo del nivel de busqueda 0..6 (999999, 999999, 999999, 10000, 5000, 3500, 2000)
    auto delayOf = [&](u32 lvl) -> i32 { static constexpr i32 d[7]{999999, 999999, 999999, 10000, 5000, 3500, 2000}; return lvl <= 6 ? d[lvl] : 5000; };
    auto chase = [&] {   // mision 0x28 con el jugador fuera de un helicoptero o avion y todo despejado
        c.mission = 0x28u; c.fl40 &= ~0x20000100u; c.norain = 0u;
        c.pvKind = pct(300, 45) ? 0u : 1u;
        if (c.pvKind) c.pvSub = pct(301, 50) ? 0u : (pct(302, 50) ? 1u : 5u);
        if (c.peKind == 0u && pct(303, 85)) c.peKind = 1u + Rand(n, 304) % 3u;
    };
    // fija las marcas de tiempo para disparar (o no) con el retardo del nivel actual
    auto dueSetup = [&](i32 delay, i32 slack) {
        c.iw[20] = c.now - u32(delay) - u32(slack);   // 0xA04
        c.iw[0] = c.now - 1u - Rand(n, 305) % 500u;   // 0x504 ya pasado
    };
    switch (f) {
    case 0:   // bloqueado: bandera de entidad o zona sin lluvia; el resto aleatorio
        if (pct(310, 50)) c.fl40 |= 0x20000000u; else c.norain = 1u + (pct(311, 20) ? Rand(n, 312) % 255u : 0u);
        if (pct(313, 30)) c.fl40 |= 0x100u;
        break;
    case 1:   // persecucion del jugador (mision 0x28): camino principal con el foco encendido
        chase(); nearOk();
        c.wlevel = Rand(n, 314) % 7u; c.wflags &= ~7u;
        if (pct(315, 70)) c.nopolice = 0u;
        if (pct(316, 70)) { delta = Rand(n, 317) % 1001u; c.iw[15] = c.now - delta; }
        break;
    case 2:   // el jugador va en helicoptero o avion: la persecucion no arranca; alternativas por modelo o piloto automatico
        c.mission = 0x28u; c.fl40 &= ~0x20000100u; c.norain = 0u; c.pvKind = 1u; c.pvSub = pct(320, 50) ? 3u : 4u;
        if (pct(321, 50)) { c.status = Rand(n, 322) % 8u; c.model = 0x1F1u; c.a11 = 1u; nearOk(); }
        break;
    case 3:   // mision 0x27 con objetivo del piloto automatico
        c.mission = 0x27u; c.fl40 &= ~0x20000100u; c.norain = 0u;
        if (pct(323, 75)) { c.apKind = 1u + Rand(n, 324) % 3u; c.hf988 |= 2u; } else if (pct(325, 50)) c.apKind = 0u; else c.hf988 &= ~2u;
        if (pct(326, 80)) nearOk();
        if (pct(327, 15)) c.fl40 |= 0x100u;
        break;
    case 4:   // foco de busqueda fijo sin objetivo (modelo 0x1F1, estado 0, bandera A11)
        c.mission = pct(328, 80) ? 0x20u + Rand(n, 329) % 8u : c.mission; if (c.mission == 0x27u || c.mission == 0x28u) c.mission = 0x26u;
        c.status = Rand(n, 330) % 8u; c.model = pct(331, 85) ? 0x1F1u : 0x1F0u; c.a11 = pct(332, 80) ? 1u : 0u; c.fl40 &= ~0x20000100u; c.norain = 0u;
        if (pct(333, 12)) { c.matNull = 1u; c.heading = pct(334, 50) ? 0u : 0x80000000u; }
        if (pct(335, 75)) place(pct(336, 15) ? 40.0 + 2.0 * uni(337, 0.0, 1.0) : uni(337, 0.0, 38.0), 0.0);
        break;
    case 5:   // motor, particulas, bocina y conductor
        c.fl428 = pct(338, 70) ? (c.fl428 & ~0x10u) : (c.fl428 | 0x10u); c.fire = pct(339, 70) ? 1u : 0u; c.horn = pct(340, 60) ? flag(341, 90) : 0u;
        c.drvKind = pct(342, 70) ? 1u : 0u; c.drvType = pct(343, 40) ? 1u : Rand(n, 344) % 4u;
        if (pct(345, 50)) { chase(); nearOk(); }
        c.a11 = pct(346, 50) ? 1u : 0u;
        break;
    case 6:   // historial de 1 s: retrasos alrededor de 1000 ms
        {
            static constexpr std::array<u32, 14> d{1000u, 1001u, 1002u, 1999u, 2000u, 2001u, 2002u, 2999u, 3000u, 3001u, 5000u, 6000u, 999u, 0u};
            delta = pct(350, 55) ? d[Rand(n, 351) % d.size()] : (pct(352, 70) ? 1001u + Rand(n, 353) % 100000u : (pct(354, 60) ? 100000u + Rand(n, 355) % 5000000u : (pct(356, 40) ? 0x7FFFFFFFu : (0x80000000u + Rand(n, 357) % 0x1000u))));
            if (pct(358, 50)) chase(); else { c.mission = 0x27u; c.apKind = 1u + Rand(n, 359) % 3u; c.hf988 |= 2u; c.fl40 &= ~0x20000100u; c.norain = 0u; }
            c.fl40 &= ~0x20000100u; c.norain = 0u;
            c.iw[15] = c.now - delta;
            c.ms[0] = fv(sgn(360) * logmag(361, 0.001, 3.0)); c.ms[1] = fv(sgn(362) * logmag(363, 0.001, 3.0)); c.tspd[0] = fv(sgn(364) * logmag(365, 0.001, 3.0)); c.tspd[1] = fv(sgn(366) * logmag(367, 0.001, 3.0));
            if (pct(368, 30)) c.peKind = 0u;
        }
        break;
    case 7:   // umbrales de distancia 40 y 60
        {
            static constexpr std::array<double, 10> d{39.9, 40.0, 40.0001, 41.0, 59.99, 60.0, 60.01, 100.0, 0.0, 30.0};
            chase();
            const double D = pct(370, 60) ? d[Rand(n, 371) % d.size()] + double(int(Rand(n, 372) % 5) - 2) * 1e-5 : uni(373, 0.0, 70.0);
            place(D, pct(374, 80) ? uni(375, 0.0, 6.9) : uni(375, 0.0, 30.0));
            if (pct(376, 30)) { c.mission = 0x27u; c.apKind = 1u + Rand(n, 377) % 3u; c.hf988 |= 2u; }
            delta = Rand(n, 378) % 1001u; c.iw[15] = c.now - delta;
        }
        break;
    case 8:   // potencia 0.9 (distancia 42) y radio 7 del punto de mira
        {
            chase();
            const double D = pct(380, 50) ? 42.0 + double(int(Rand(n, 381) % 9) - 4) * 1e-5 : uni(382, 38.0, 46.0);
            const double E = pct(383, 55) ? 7.0 + double(int(Rand(n, 384) % 9) - 4) * 1e-6 : uni(385, 5.0, 9.0);
            place(D, E);
            delta = Rand(n, 386) % 1001u; c.iw[15] = c.now - delta;
            c.iw[21] = pct(387, 50) ? c.now - 1u - Rand(n, 388) % 3u : (pct(389, 50) ? c.now + 0u : c.now + 1u + Rand(n, 390) % 3u);   // 0xA08 junto a la hora actual
            if (pct(391, 15)) c.now = 0xFFFFFFF0u + Rand(n, 392) % 16u;
        }
        break;
    case 9:   // tabla de niveles de busqueda, mitad de retardo, objetivo = jugador y avisos de la policia
        chase(); nearOk();
        c.wlevel = pct(393, 70) ? Rand(n, 394) % 7u : (pct(395, 60) ? 7u + Rand(n, 396) % 6u : Rand(n, 396));
        c.nopolice = pct(397, 50) ? flag(398, 100) : 0u;
        c.peKind = pct(399, 55) ? 2u : 1u + Rand(n, 400) % 3u;
        c.wflags = pct(401, 40) ? (Rand(n, 402) & 0xFFu) : (Rand(n, 402) & 0xF8u);
        if (pct(403, 50)) { const i32 dl = delayOf(c.wlevel); dueSetup(c.nopolice & 0xFF ? dl / 2 : dl, 5); }
        break;
    case 10:   // momento del disparo: retardo, hora anterior y linea de vision
        chase(); nearOk();
        c.peKind = 2u; c.wflags &= ~7u;
        {
            c.wlevel = Rand(n, 404) % 7u; c.nopolice = pct(405, 25) ? 1u : 0u;
            i32 dl = delayOf(c.wlevel); if (c.nopolice & 0xFF) dl /= 2;
            static constexpr std::array<int, 9> sl{-3, -2, -1, 0, 1, 2, 3, 100, -100};
            const int s = sl[Rand(n, 406) % sl.size()];
            c.iw[20] = c.now - u32(dl) - u32(s);
            const u32 due = c.iw[20] + u32(dl);
            static constexpr std::array<int, 7> pv{-2, -1, 0, 1, 2, 5, 400};
            c.prev = pct(407, 60) ? due + u32(pv[Rand(n, 408) % pv.size()]) : c.now - Rand(n, 409) % 100u;
            c.iw[0] = pct(410, 50) ? c.now - Rand(n, 411) % 50u : c.now + u32(int(Rand(n, 412) % 5) - 2);
            c.los = flag(413, 50);
        }
        break;
    case 11:   // el disparo: dispersion, normalizacion y tirada del 15 %
        chase(); nearOk();
        c.peKind = 2u; c.wflags &= ~7u; c.wlevel = 6u; c.nopolice = 0u;
        dueSetup(2000, 3 + int(Rand(n, 414) % 3000));
        c.los = pct(415, 80) ? 1u : 0u;
        if (c.los == 0u) { c.los = 0u; c.iw[20] = c.now - 2000u - 3u - Rand(n, 416) % 3000u; }
        if (pct(417, 18)) { for (int i = 0; i < 3; ++i) c.nrm[i] = fl(418 + i, 3u); }
        if (pct(421, 15)) { for (int k = 0; k < 3; ++k) c.mp[0][k] = fl(422 + k, 3u); }
        if (pct(425, 10)) c.mp[0][Rand(n, 426) % 3] = Pick(kTiny, n, 427);
        c.prev = c.now - Rand(n, 428) % 100u;
        break;
    case 12:   // cuerdas de los SWAT
        {
            c.mission = pct(430, 80) ? 0x28u : c.mission;
            c.fl40 &= ~0x20000100u; c.norain = 0u;
            c.nsw = pct(431, 70) ? 1u + Rand(n, 432) % 4u : 0u;
            for (int i = 0; i < 4; ++i) { static constexpr std::array<u32, 6> st{0u, 1u, 2u, 3u, 255u, 1u}; c.swt[i] = pct(433 + i, 25) ? 0u : st[Rand(n, 437 + i) % st.size()]; }
            if (pct(441, 5)) { c.matNull = 1u; c.heading = pct(442, 50) ? 0u : 0x80000000u; }
            if (pct(443, 50)) { chase(); nearOk(); }
        }
        break;
    case 13:   // aviso de eventos de interes (0xC0B184 bit 0) y su tirada
        c.ev184 = pct(444, 85) ? (c.ev184 | 1u) : (c.ev184 & ~1u);
        c.ts = pct(445, 55) ? fv(uni(446, 20.0, 700.0)) : fv(uni(447, 0.5, 4.0));
        if (pct(448, 60)) { chase(); nearOk(); }
        break;
    case 14:   // valores especiales repartidos (NaN, infinitos, desnormales, enormes)
        {
            if (pct(450, 60)) { chase(); nearOk(); }
            u32* t[] = {&c.ts, &c.heading, &c.hpos[0], &c.hpos[1], &c.mpos[0], &c.mpos[1], &c.mpos[2], &c.mr[0], &c.mf[1], &c.mu[0], &c.mu[2], &c.ms[0], &c.ms[1], &c.tpos[0], &c.tpos[1], &c.tmpos[0], &c.tmpos[1], &c.tspd[0], &c.tspd[1],
                        &c.nrm[0], &c.nrm[2], &c.sw[0][0], &c.sw[1][1], &c.mp[0][0], &c.mp[0][2], &c.m3o[0][1], &c.iw[3], &c.iw[4], &c.iw[5], &c.iw[9], &c.iw[10], &c.iw[11], &c.iw[2], &c.iw[19]};
            for (int k = 0; k < 34; ++k) if (pct(460 + k, 9)) *t[k] = fl(500 + k, 3u);
            if (c.matNull) c.heading = pct(540, 50) ? 0u : 0x80000000u;
        }
        break;
    case 15:   // aleatorio general
        if (pct(541, 55)) { chase(); if (pct(542, 70)) nearOk(); }
        else if (pct(543, 40)) { c.mission = 0x27u; c.apKind = 1u + Rand(n, 544) % 3u; c.hf988 |= 2u; c.fl40 &= ~0x20000100u; c.norain = 0u; nearOk(); }
        if (pct(545, 4)) { c.matNull = 1u; c.heading = pct(546, 50) ? 0u : 0x80000000u; }
        break;
    case 16:   // NaN e infinitos repartidos
        {
            if (pct(550, 75)) { chase(); nearOk(); c.peKind = pct(551, 60) ? 2u : c.peKind; c.wflags &= ~7u; dueSetup(delayOf(c.wlevel), 3 + int(Rand(n, 552) % 300)); }
            u32* t[] = {&c.ts, &c.hpos[0], &c.hpos[1], &c.mpos[0], &c.mpos[1], &c.mpos[2], &c.mr[0], &c.mf[1], &c.mu[0], &c.mu[2], &c.ms[0], &c.ms[1], &c.tpos[0], &c.tpos[1], &c.tmpos[0], &c.tmpos[1], &c.tspd[0], &c.tspd[1],
                        &c.nrm[0], &c.nrm[1], &c.nrm[2], &c.sw[0][0], &c.sw[1][1], &c.sw[2][2], &c.mp[0][0], &c.mp[0][1], &c.mp[0][2], &c.mp[1][0], &c.m3o[0][1], &c.m3o[1][0], &c.iw[3], &c.iw[4], &c.iw[5], &c.iw[9], &c.iw[10], &c.iw[11], &c.iw[2], &c.iw[19]};
            for (int k = 0; k < 38; ++k) if (pct(560 + k, 8)) *t[k] = pct(600 + k, 50) ? Pick(kNan, n, 640 + k) : Pick(kInf, n, 680 + k);
        }
        break;
    default:   // 17: desnormales y bordes
        {
            if (pct(720, 70)) { chase(); nearOk(); c.peKind = pct(721, 60) ? 2u : c.peKind; c.wflags &= ~7u; dueSetup(delayOf(c.wlevel), 3 + int(Rand(n, 722) % 300)); }
            u32* t[] = {&c.ts, &c.hpos[0], &c.mpos[0], &c.mpos[1], &c.mu[2], &c.ms[0], &c.ms[1], &c.tpos[0], &c.tmpos[0], &c.tmpos[1], &c.tspd[0], &c.nrm[0], &c.nrm[1], &c.sw[0][0], &c.mp[0][0], &c.mp[0][1], &c.m3o[0][0],
                        &c.iw[3], &c.iw[4], &c.iw[5], &c.iw[9], &c.iw[10], &c.iw[11], &c.iw[2], &c.iw[19]};
            for (int k = 0; k < 25; ++k) {
                const u32 w = Rand(n, 730 + k) % 7;
                if (w == 0) *t[k] = Pick(kTiny, n, 760 + k); else if (w == 1) *t[k] = (Rand(n, 790 + k) & 0x807FFFFFu) | 1u;
            }
        }
        break;
    }
    // el rumbo solo importa sin matriz y la funcion solo lo soporta en +-0 (sin y cos exactos); la matriz nula se vuelve a fijar tras los valores especiales
    if (c.matNull && c.heading != 0u && c.heading != 0x80000000u) c.heading = 0u;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack).U("rseed", c.rseed);
    j.U("horn", c.horn).U("norain", c.norain).U("nopolice", c.nopolice).U("los", c.los);
    j.U("pv_kind", c.pvKind).U("pv_sub", c.pvSub).U("pe_kind", c.peKind).U("ap_kind", c.apKind);
    j.U("fl428", c.fl428).U("fl40", c.fl40).U("mission", c.mission).U("status", c.status).U("model", c.model).U("hf988", c.hf988).U("drv_kind", c.drvKind).U("drv_type", c.drvType).U("fire", c.fire).U("mat_null", c.matNull).U("heading", c.heading);
    j.A("hpos", c.hpos, 3).A("mpos", c.mpos, 3).A("mr", c.mr, 3).A("mf", c.mf, 3).A("mu", c.mu, 3).A("ms", c.ms, 3).A("iw", c.iw, 22);
    j.U("nsw", c.nsw).A("swt", c.swt, 4).U("a11", c.a11);
    j.A("tpos", c.tpos, 3).U("tmat_null", c.tmatNull).A("tmpos", c.tmpos, 3).A("tspd", c.tspd, 3);
    j.U("now", c.now).U("prev", c.prev).U("ts", c.ts).U("ev184", c.ev184).U("wlevel", c.wlevel).U("wflags", c.wflags);
    j.A("nrm", c.nrm, 3).A("sw", &c.sw[0][0], 24).A("mp", &c.mp[0][0], 15).A("m3o", &c.m3o[0][0], 12);
    return j.End();
}
}

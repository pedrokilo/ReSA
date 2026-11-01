#pragma once
// Fixtures del banco Heli83E (lote 83E): CHeli::ProcessControlInputs (EXE 0x6C4830), 18432 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CHeli::ProcessControlInputs  0x6C4830  thiscall, ECX = this, 1 dword (numero de mando; la funcion usa solo el byte bajo), `ret 4`, void (hook CHeli / ProcessControlInputs, tipo V: RH_ScopedVMTInstall)
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

namespace H83EFix {
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
constexpr std::size_t kVehWin = 0xA20, kMatWin = 0x48, kFlyWin = 0x60, kPlayerWin = 0x20;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kMatArena = Arena(kMatWin), kFlyArena = Arena(kFlyWin), kPlayerArena = Arena(kPlayerWin);

constexpr u32 kTimeStep = 0xB7CB5C;                       // CTimer::ms_fTimeStep
constexpr u32 kUseMouse3rd = 0xB6EC2E;                    // CCamera::m_bUseMouse3rdPerson (byte)
constexpr u32 kMouseFly = 0xC1CC03;                       // CVehicle::m_bEnableMouseFlying (byte)
constexpr u32 kLastControl = 0xC1CC04;                    // CVehicle::m_nLastControlInput (dword)
constexpr u32 kMouseX = 0xB73424, kMouseY = 0xB73428;     // CPad::NewMouseControllerState.m_AmountMoved
constexpr u32 kPadsBase = 0xB73458, kPadSize = 0x134, kPadsBytes = 2 * 0x134;

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
    u32 eax{}, edx{}, stack{};
    u32 arg{};                                   // dword del argumento: byte bajo = mando (0 o 1), el resto basura
    // EAX que devuelven los falsos del mando (CPad::GetAccelerate, GetBrake, GetSteeringLeftRight, GetSteeringUpDown: AX; GetLookRight, GetLookLeft, GetHorn: AL)
    u32 acc{}, brk{}, lr{}, ud{}, lookR{}, lookL{}, horn{};
    // globales
    u32 m3{}, fly{}, mode{}, mx{}, my{}, ts{};
    u32 vml{}, dpc{}, vmlOther{}, dpcOther{};    // palabras del mando: +0x2C (m_bVehicleMouseLook) y +0x10E (DisablePlayerControls), del mando usado y del otro
    // vehiculo
    u32 skid0{}, ud0{}, lr0{}, acc0{}, steer0{}, gas0{}, brk0{}, flags{}, health{};
    u32 ms[3]{};                                 // m_vecMoveSpeed
    u32 mr[3]{}, mf[3]{}, mu[3]{};               // matriz: derecha (+0), adelante (+0x10), arriba (+0x20)
    u32 pitch{}, roll{};                         // CFlyingHandlingData +0x24 y +0x1C
    // salidas de los falsos de salida (Normalise y CrossProduct se llaman dos veces como maximo)
    u32 nrm[2][3]{}, crs[2][3]{};
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
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    auto fl = [&](u32 k, u32 mode = 0u) { return FloatOf(n, k, mode); };
    auto vec3 = [&](u32* dst, u32 k, double len) { double d[3]; Split3(n, k, len, d); for (int i = 0; i < 3; ++i) dst[i] = B(float(d[i])); };
    auto logmag = [&](u32 k, double lo, double hi) { return std::pow(10.0, std::log10(lo) + double(Rand(n, k) % 2001) / 2000.0 * (std::log10(hi) - std::log10(lo))); };
    auto sgn = [&](u32 k) { return Rand(n, k) % 2 ? 1.0 : -1.0; };
    auto pct = [&](u32 k, u32 p) { return Rand(n, k) % 100 < p; };
    // eje del mando (AX): casi siempre un entero pequeno; la mitad alta de EAX es basura una de cada cuatro veces
    auto axis = [&](u32 k, u32 range) -> u32 {
        const u32 r = Rand(n, k) % 10;
        u32 v;
        if (r < 2) v = 0;
        else if (r < 8) v = u32(i32(Rand(n, k + 1) % (2 * range + 1)) - i32(range));
        else v = Rand(n, k + 2) & 0xFFFFu;
        v &= 0xFFFFu;
        if (pct(k + 3, 25)) v |= Rand(n, k + 4) & 0xFFFF0000u;
        else if ((v & 0x8000u) && pct(k + 5, 50)) v |= 0xFFFF0000u;
        return v;
    };
    // booleano (AL): 0, 1 o un byte cualquiera; el resto de EAX es basura una de cada cuatro veces
    auto flag = [&](u32 k, u32 on) -> u32 {
        u32 v = pct(k, on) ? (pct(k + 1, 80) ? 1u : 1u + Rand(n, k + 2) % 255u) : 0u;
        if (pct(k + 3, 25)) v |= Rand(n, k + 4) & 0xFFFFFF00u;
        return v;
    };
    // ---- base: caso plausible
    c.arg = (Rand(n, 10) % 2) | (pct(11, 30) ? (Rand(n, 12) & 0xFFFFFF00u) : 0u);
    c.acc = axis(13, 255); c.brk = axis(17, 255); c.lr = axis(21, 128); c.ud = axis(25, 128);
    c.lookR = flag(29, 20); c.lookL = flag(34, 20); c.horn = flag(39, 25);
    c.m3 = pct(44, 70) ? 1u : 0u; c.fly = pct(45, 80) ? 1u : 0u;
    {
        static constexpr std::array<u32, 6> md{2u, 2u, 2u, 0u, 1u, 3u};
        c.mode = pct(46, 8) ? Rand(n, 47) : md[Rand(n, 48) % md.size()];
    }
    c.mx = pct(49, 45) ? (pct(50, 30) ? 0x80000000u : 0u) : B(float(sgn(51) * logmag(52, 0.001, 60.0)));
    c.my = pct(53, 45) ? (pct(54, 30) ? 0x80000000u : 0u) : B(float(sgn(55) * logmag(56, 0.001, 60.0)));
    c.ts = B(0.5f + float(Rand(n, 57) % 3000) / 1000.0f);
    c.vml = pct(58, 35) ? (pct(59, 70) ? 1u : Rand(n, 60) & 0xFFFFu) : 0u;
    c.dpc = pct(61, 15) ? (pct(62, 60) ? 1u : 1u + Rand(n, 63) % 0xFFFFu) : 0u;
    c.vmlOther = Rand(n, 64) & 0xFFFFu; c.dpcOther = Rand(n, 65) & 0xFFFFu;
    auto steer = [&](u32 k) {
        const u32 r = Rand(n, k) % 10;
        if (r < 3) return pct(k + 1, 50) ? 0u : 0x80000000u;
        if (r < 5) return B(float(sgn(k + 2) * (0.499 + double(Rand(n, k + 3) % 2001) / 1e6)));
        return B(float(sgn(k + 4) * logmag(k + 5, 0.001, 3.0)));
    };
    c.skid0 = fl(66, 4u); c.ud0 = steer(67); c.lr0 = steer(73); c.acc0 = fl(79, 0u);
    c.steer0 = fl(80, 0u); c.gas0 = fl(81, 0u); c.brk0 = fl(82, 0u);
    c.flags = Rand(n, 83) & 0xFFu;
    c.health = pct(84, 20) ? B(float(Rand(n, 85) % 251)) : B(float(Rand(n, 86) % 1100));
    vec3(c.ms, 87, pct(90, 15) ? logmag(91, 0.01, 0.4) : logmag(92, 0.0005, 2.0));
    vec3(c.mr, 93, 1.0); vec3(c.mf, 97, 1.0); vec3(c.mu, 101, 1.0);
    if (pct(105, 85)) c.mu[2] = B(float(0.05 + double(Rand(n, 106) % 950) / 1000.0));   // arriba.z positivo casi siempre
    c.pitch = B(float(double(Rand(n, 107) % 4001) / 1000.0)); c.roll = B(float(double(Rand(n, 108) % 4001) / 1000.0));
    for (int k = 0; k < 2; ++k) {
        if (pct(110 + 20 * k, 10)) { for (int i = 0; i < 3; ++i) c.nrm[k][i] = fl(111 + 20 * k + i, 3u); }
        else vec3(c.nrm[k], 114 + 20 * k, 1.0);
        if (pct(117 + 20 * k, 10)) { for (int i = 0; i < 3; ++i) c.crs[k][i] = fl(118 + 20 * k + i, 3u); }
        else vec3(c.crs[k], 121 + 20 * k, pct(124 + 20 * k, 8) ? 0.0 : logmag(125 + 20 * k, 0.1, 2.0));
    }
    // ---- por familia
    auto cheat = [&] { c.m3 = 1u; c.fly = 1u; };
    auto noMouse = [&] { c.mx = pct(150, 50) ? 0u : 0x80000000u; c.my = pct(151, 50) ? 0u : 0x80000000u; };
    auto padZero = [&] { c.lr = pct(152, 30) ? (Rand(n, 153) & 0xFFFF0000u) : 0u; c.ud = pct(154, 30) ? (Rand(n, 155) & 0xFFFF0000u) : 0u; };
    auto padNonZero = [&] {
        const u32 w = Rand(n, 156) % 3;
        if (w != 1) c.lr = (c.lr & 0xFFFF0000u) | (1u + Rand(n, 157) % 0xFFFEu);
        if (w != 0) c.ud = (c.ud & 0xFFFF0000u) | (1u + Rand(n, 158) % 0xFFFEu);
    };
    auto steerNonZero = [&] {
        const u32 w = Rand(n, 160) % 3;
        if (w != 1) c.lr0 = B(float(sgn(161) * logmag(162, 1e-6, 3.0)));
        if (w != 0) c.ud0 = B(float(sgn(164) * logmag(165, 1e-6, 3.0)));
    };
    switch (f) {
    case 0:   // sin raton (o sin la bandera de vuelo): el teclado/mando fija los ejes
        if (pct(170, 50)) c.m3 = 0u; else c.fly = 0u;
        break;
    case 1:   // raton movido: rama del raton con m_bVehicleMouseLook 0 o distinto
        cheat();
        if (pct(171, 40)) { c.mx = 0u; if (F(c.my) == 0.0f) c.my = B(float(sgn(172) * logmag(173, 0.01, 40.0))); }
        else if (pct(174, 20)) c.my = 0u;
        if (F(c.mx) == 0.0f && F(c.my) == 0.0f) c.mx = B(float(sgn(175) * logmag(176, 0.01, 40.0)));
        c.mode = pct(177, 50) ? 2u : c.mode;
        break;
    case 2:   // sin raton, ejes fijados distintos de cero, modo raton y mando en reposo: sigue en modo raton
        cheat(); noMouse(); c.mode = 2u; padZero(); steerNonZero();
        break;
    case 3:   // igual pero el mando se mueve: vuelve al teclado
        cheat(); noMouse(); c.mode = 2u; padNonZero(); steerNonZero();
        break;
    case 4:   // modo raton, mando en reposo y ejes nulos: no cambia nada
        cheat(); noMouse(); c.mode = 2u; padZero();
        c.lr0 = pct(178, 50) ? 0u : 0x80000000u; c.ud0 = pct(179, 50) ? 0u : 0x80000000u;
        if (pct(180, 15)) c.lr0 = Pick(kNan, n, 181);
        if (pct(182, 15)) c.ud0 = Pick(kNan, n, 183);
        break;
    case 5:   // modo distinto de raton con la bandera de vuelo: teclado
        cheat(); noMouse();
        {
            static constexpr std::array<u32, 8> md{0u, 1u, 3u, 4u, 0xFFFFFFFFu, 0x80000000u, 0x100u, 0x00000002u};
            c.mode = md[Rand(n, 184) % md.size()];
            if (c.mode == 2u && pct(185, 70)) c.mode = 3u;
        }
        if (pct(186, 50)) steerNonZero();
        break;
    case 6:   // decaimiento del raton: ejes junto a 0.5 y pasos de tiempo variados
        cheat(); c.mode = 2u;
        if (pct(187, 60)) { c.mx = 0u; c.my = 0u; padZero(); steerNonZero(); }
        {
            static constexpr float lim[]{0.5f, 0.49999997f, 0.50000006f, 0.4f, 0.6f, 0.0f, 0.25f};
            auto edge = [&](u32 k) { return B(float(sgn(k)) * F(Ulps(B(lim[Rand(n, k + 1) % std::size(lim)]), int(Rand(n, k + 2) % 5) - 2) & 0x7FFFFFFFu)); };
            c.lr0 = edge(188); c.ud0 = edge(191);
        }
        c.ts = pct(194, 25) ? Pick(std::array<u32, 8>{0u, 0x80000000u, 0x3F800000u, 0x40A00000u, 0xBF800000u, 0x3C23D70Au, 0x41200000u, 0x00000001u}, n, 195) : c.ts;
        break;
    case 7:   // bocina con el helicoptero boca arriba: nivelar
        c.horn = 1u + (pct(197, 20) ? Rand(n, 198) % 255u : 0u);
        c.mu[2] = B(float(0.01 + double(Rand(n, 199) % 990) / 1000.0));
        c.pitch = B(float(double(Rand(n, 200) % 8001) / 1000.0)); c.roll = B(float(double(Rand(n, 201) % 8001) / 1000.0));
        if (pct(202, 40)) vec3(c.ms, 203, logmag(206, 0.1, 5.0));
        break;
    case 8:   // bocina con arriba.z en el borde (0, -0, minimo, negativo, NaN) o sin bocina
        {
            static constexpr std::array<u32, 9> up{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0xFFC00000u, 0xBF800000u, 0x3F800000u, 0x00800000u};
            c.horn = pct(207, 85) ? 1u : 0u;
            c.mu[2] = pct(208, 70) ? Pick(up, n, 209) : Ulps(Pick(std::array<u32, 3>{0u, 1u, 0x3F800000u}, n, 210), int(Rand(n, 211) % 5) - 2);
        }
        break;
    case 9:   // acotado de los ejes: valores del mando grandes y ejes de raton junto a +-1; productos de nivelacion junto a +-2
        if (pct(213, 50)) { c.lr = (c.lr & 0xFFFF0000u) | (Rand(n, 214) & 0xFFFFu); c.ud = (c.ud & 0xFFFF0000u) | (Rand(n, 215) & 0xFFFFu); }
        else {
            cheat(); c.mode = 2u; noMouse(); padZero();
            static constexpr float lim[]{1.0f, 1.5f, 0.99999994f, 1.0000001f, 2.0f};
            auto edge = [&](u32 k) { return B(float(sgn(k)) * F(Ulps(B(lim[Rand(n, k + 1) % std::size(lim)]), int(Rand(n, k + 2) % 5) - 2) & 0x7FFFFFFFu)); };
            c.lr0 = edge(216); c.ud0 = edge(219);
        }
        if (pct(221, 60)) {
            // nivelacion con producto ~ +-2: el vector normalizado (0, 1, 0) por la velocidad y el factor de estabilidad
            c.mu[2] = B(0.5f);
            const double target = Rand(n, 222) % 2 ? 2.0 : -2.0;
            const double s = target * (1.0 + double(int(Rand(n, 223) % 7) - 3) * 1e-7);
            c.nrm[0][0] = 0u; c.nrm[0][1] = B(1.0f); c.nrm[0][2] = 0u;
            c.nrm[1][0] = 0u; c.nrm[1][1] = B(1.0f); c.nrm[1][2] = 0u;
            c.ms[0] = 0u; c.ms[1] = B(float(s / 2.0)); c.ms[2] = 0u;
            c.pitch = B(2.0f); c.roll = B(2.0f);
            c.horn = 1u;
        }
        break;
    case 10:   // DisablePlayerControls distinto de cero: despejar la zona y limitar la velocidad a 0.28
        c.dpc = pct(224, 85) ? (pct(225, 60) ? 1u : 1u + Rand(n, 226) % 0xFFFFu) : 0u;
        {
            const double len = pct(228, 30) ? 0.28 : logmag(229, 0.01, 2.0);
            vec3(c.ms, 230, len);
            if (pct(233, 25)) c.ms[Rand(n, 234) % 3] = B(float(sgn(235) * logmag(236, 1e-3, 4.0)));
            if (pct(237, 10)) { c.ms[0] = c.ms[1] = c.ms[2] = Pick(kTiny, n, 238); }
        }
        break;
    case 11:   // salud en el umbral de 250
        {
            static constexpr std::array<u32, 8> h{0x437A0000u, 0x437A0001u, 0x437A0000u - 1u, 0u, 0x80000000u, 0xFFC00000u, 0x7F800000u, 0xFF800000u};
            c.health = pct(239, 70) ? Ulps(0x437A0000u, int(Rand(n, 240) % 5) - 2) : Pick(h, n, 241);
            if (pct(242, 40)) c.lookL = 0u;
        }
        break;
    case 12:   // mirar a un lado: bytes del mando
        c.lookR = flag(243, 50); c.lookL = flag(248, 50); c.horn = flag(253, 20);
        if (pct(258, 30)) c.lookR = (Rand(n, 259) & 0xFFFFFF00u);
        if (pct(260, 30)) c.lookL = (Rand(n, 261) & 0xFFFFFF00u);
        break;
    case 13:   // el mando devuelve basura en la mitad alta y valores extremos de 16 bits
        {
            static constexpr std::array<u32, 8> ext{0x7FFFu, 0x8000u, 0xFFFFu, 0x8001u, 0x7FFEu, 0u, 1u, 0x0080u};
            u32* t[] = {&c.acc, &c.brk, &c.lr, &c.ud};
            for (int k = 0; k < 4; ++k) {
                if (pct(262 + k, 60)) *t[k] = ext[Rand(n, 270 + k) % ext.size()];
                *t[k] = (*t[k] & 0xFFFFu) | (Rand(n, 280 + k) & 0xFFFF0000u);
            }
            if (pct(290, 50)) { c.lookR = (Rand(n, 291) & 0xFFFFFF00u) | (Rand(n, 292) % 4); c.lookL = (Rand(n, 293) & 0xFFFFFF00u) | (Rand(n, 294) % 3); c.horn = (Rand(n, 295) & 0xFFFFFF00u) | (Rand(n, 296) % 3); }
        }
        break;
    case 14:   // valores especiales repartidos (NaN, infinitos, desnormales, enormes)
        {
            u32* t[] = {&c.mx, &c.my, &c.ts, &c.lr0, &c.ud0, &c.skid0, &c.acc0, &c.steer0, &c.gas0, &c.brk0, &c.health, &c.ms[0], &c.ms[1], &c.ms[2], &c.mr[0], &c.mf[1], &c.mu[2], &c.mu[0], &c.pitch, &c.roll, &c.nrm[0][0], &c.nrm[0][2], &c.nrm[1][1], &c.crs[0][1], &c.crs[1][0]};
            for (int k = 0; k < 25; ++k) if (pct(300 + k, 14)) *t[k] = fl(330 + k, 3u);
            if (pct(360, 50)) cheat();
            if (pct(361, 40)) c.horn = 1u;
            if (pct(362, 25)) c.dpc = 1u;
        }
        break;
    case 15:   // aleatorio general: mezcla de ramas
        if (pct(363, 50)) cheat();
        if (pct(364, 30)) c.horn = 1u;
        if (pct(365, 25)) c.dpc = 1u + Rand(n, 366) % 0xFFFFu;
        if (pct(367, 40)) { c.mx = 0u; c.my = 0u; }
        break;
    case 16:   // NaN e infinitos repartidos
        {
            u32* t[] = {&c.mx, &c.my, &c.ts, &c.lr0, &c.ud0, &c.skid0, &c.health, &c.ms[0], &c.ms[1], &c.ms[2], &c.mr[0], &c.mr[2], &c.mf[0], &c.mf[1], &c.mu[0], &c.mu[2], &c.pitch, &c.roll, &c.nrm[0][0], &c.nrm[0][1], &c.nrm[0][2], &c.nrm[1][0], &c.nrm[1][1], &c.nrm[1][2], &c.crs[0][0], &c.crs[0][1], &c.crs[1][1], &c.crs[1][2]};
            if (pct(368, 70)) cheat();
            if (pct(369, 50)) c.horn = 1u;
            if (pct(370, 30)) c.dpc = 1u;
            for (int k = 0; k < 28; ++k) if (pct(380 + k, 10)) *t[k] = pct(410 + k, 50) ? Pick(kNan, n, 440 + k) : Pick(kInf, n, 470 + k);
        }
        break;
    default:   // 17: desnormales y bordes
        {
            u32* t[] = {&c.mx, &c.my, &c.ts, &c.lr0, &c.ud0, &c.skid0, &c.health, &c.ms[0], &c.ms[1], &c.ms[2], &c.mu[2], &c.pitch, &c.roll, &c.nrm[0][0], &c.nrm[0][1], &c.nrm[1][0], &c.nrm[1][2], &c.crs[0][0], &c.crs[1][1]};
            if (pct(500, 60)) cheat();
            if (pct(501, 50)) c.horn = 1u;
            if (pct(502, 30)) c.dpc = 1u;
            for (int k = 0; k < 19; ++k) {
                const u32 w = Rand(n, 510 + k) % 7;
                if (w == 0) *t[k] = Pick(kTiny, n, 540 + k); else if (w == 1) *t[k] = (Rand(n, 570 + k) & 0x807FFFFFu) | 1u;
            }
        }
        break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack).U("arg", c.arg);
    j.U("acc", c.acc).U("brk", c.brk).U("lr", c.lr).U("ud", c.ud).U("look_r", c.lookR).U("look_l", c.lookL).U("horn", c.horn);
    j.U("m3", c.m3).U("fly", c.fly).U("mode", c.mode).U("mx", c.mx).U("my", c.my).U("ts", c.ts);
    j.U("vml", c.vml).U("dpc", c.dpc).U("vml_other", c.vmlOther).U("dpc_other", c.dpcOther);
    j.U("skid0", c.skid0).U("ud0", c.ud0).U("lr0", c.lr0).U("acc0", c.acc0).U("steer0", c.steer0).U("gas0", c.gas0).U("brk0", c.brk0).U("flags", c.flags).U("health", c.health);
    j.A("ms", c.ms, 3).A("mr", c.mr, 3).A("mf", c.mf, 3).A("mu", c.mu, 3).U("pitch", c.pitch).U("roll", c.roll);
    j.A("nrm0", c.nrm[0], 3).A("nrm1", c.nrm[1], 3).A("crs0", c.crs[0], 3).A("crs1", c.crs[1], 3);
    return j.End();
}
}

#pragma once
// Fixtures del banco Car81C (lote 81C): CCarCtrl::PossiblyRemoveVehicle (EXE 0x424F80), 18432 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::PossiblyRemoveVehicle  0x424F80  cdecl, un argumento (vehiculo), `ret` simple, void   (hook CCarCtrl / PossiblyRemoveVehicle, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) y las familias 16 y 17 (1024 casos cada una).
// Regla de seguridad (el EXE lee el centro del jugador SIN inicializar si no entra en la etapa 1): si el vehiculo no entra en la etapa 1 (bloqueado, CanBeDeleted falso o grua), su estado nunca
// es 2, 3 ni 5 (ni se muta a ellos), de modo que las etapas 2 y 3 no llegan a usar el centro. El estado 2 exige matriz (el EXE lee la matriz sin comprobar nulo).
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

namespace C81Fix {
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

// Arenas: guarda + ventana + guarda (la ventana es el objeto). La del vehiculo mide sizeof(CVehicle) = 0x5A0 (LiveComparison.hpp lo comprueba con static_assert).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x5A0, kMatWin = 0x48, kCenWin = 0x10, kClumpWin = 0x20;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kMatArena = Arena(kMatWin), kCenArena = Arena(kCenWin), kClumpArena = Arena(kClumpWin);

constexpr u32 kTimeAddr = 0xB7CB84;       // CTimer::m_snTimeInMilliseconds
constexpr u32 kCamActiveAddr = 0xB6F081;  // TheCamera.m_nActiveCam
constexpr u32 kCamsBase = 0xB6F19C;       // TheCamera.m_aCams
constexpr u32 kCamStride = 0x238;
constexpr u32 kCamMultAddr = 0xB6F11C, kCamLowAddr = 0xB6F9B4, kFocusAddr = 0xB7CD74;
constexpr u32 kGarbageBase = 0xA0000010u;
constexpr u32 kK170 = 0x432A0000u, kInv170 = 0x3BC0C0C1u;   // 170.0f y 0x858F94 (el patron real se lee del EXE; aqui solo orienta las fixtures)

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 81031u); }
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
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ valores notables
inline constexpr std::array<u32, 4> kNan{0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u};   // un unico patron de NaN (el indefinido de la x87)
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
// Un float "especial": modo 0 normal moderado, 1 pequeno, 2 grande, 3 especial (cero, desnormal, inf, NaN)
inline u32 Special(u32 c, u32 k, u32 mode) {
    const u32 r = Rand(c, k) % 32;
    switch (mode) {
    case 0: return RandFloat(c, k + 2, -4, 11);
    case 1: return r < 4 ? Pick(kTiny, c, k + 1) : RandFloat(c, k + 2, -80, -20);
    case 2: return r < 4 ? Pick(kBig, c, k + 1) : RandFloat(c, k + 2, 20, 120);
    default:
        if (r < 9) return Pick(kTiny, c, k + 1);
        if (r < 15) return Pick(kInf, c, k + 1);
        if (r < 24) return Pick(kNan, c, k + 1);
        if (r < 28) return Pick(kBig, c, k + 1);
        return RandFloat(c, k + 2, -20, 100);
    }
}
inline u32 Ulps(u32 bits, int d) { return u32(bits + u32(d)); }   // desplazamiento en ulps (d > 0: magnitud mayor; d < 0: menor)

// ------------------------------------------------------------------------------------------------ el caso
constexpr unsigned kEvents = 40;       // cota superior de eventos de un caso (el barrido recorre los primeros 24)
constexpr u32 kNoMutation = 99;        // mutAt fuera de rango = sin mutacion
constexpr u32 kKinds = 27;
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 now{};
    // vehiculo
    u32 b485{}, f428{}, f42A{}, f42B{}, f42C{}, s36{}, entFlags{}, model{}, created{}, range{}, till{}, blown{}, switched{}, style{}, mission{}, tempact{}, vtype{}, target{}, clump{}, fakePhys{}, hasMat{};
    u32 vp[3]{}, mp[3]{}, upz{}, bp[3]{}, bupz{}, cen[3]{};
    // globales
    u32 camMult{}, camLow{}, activeCam{}, cam[9]{}, focus{};
    // retornos crudos de los colaboradores
    u32 rcbd[2]{}, rcrane{}, ralpha{}, ron[4]{}, rlook{}, rgar[3]{}, rint[2]{}, rlight{}, rbridge{};
    u32 mutAt{}, mutKind{}, mutVal{};
    u32 entered{}, gate{};
};

inline bool Entered(const C& c) { return !((c.f428 >> 3) & 1u) && (c.rcbd[0] & 0xFFu) != 0 && (c.rcrane & 0xFFu) == 0; }
inline u32 StatusOf(const C& c) { return (c.s36 & 0xFFu) >> 3; }

// float aproximado (solo orienta las fixtures: la verdad la da el oraculo)
inline float Approx(const C& c, bool& nearRange) {
    const bool on0 = (c.ron[0] & 0xFFu) != 0;
    const u32 ac = c.activeCam % 3;
    nearRange = !on0 && !c.cam[ac * 3 + 1] && !c.cam[ac * 3 + 2] && !c.cam[ac * 3] && c.rlook != 0 && (c.created & 0xFFu) != 3 && (c.model & 0xFFFFu) != 0x1A0 && (c.model & 0xFFFFu) != 0x197
        && !(c.f428 & 1u) && !((c.f42B >> 1) & 1u) && c.now >= c.till && !((c.f42C >> 6) & 1u);
    float v = nearRange ? 45.0f : F(c.camMult) * 170.0f;
    if (F(c.camLow) < -0.9f) v = 70.0f;
    const float r = float(std::int16_t(c.range & 0xFFFFu));
    const float m = r > 170.0f ? r : 170.0f;
    return (m * v) * F(kInv170);
}

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
    auto Ret = [&](bool b) {   // retorno crudo de un bool: AL decide; los bits altos son basura (la DLL no puede probar EAX completo)
        u32 hi = Rand(n, nk()) & 0xFFFFFF00u;
        if (Rand(n, nk()) % 3 == 0) hi = 0;
        u32 lo = b ? (Rand(n, nk()) % 2 ? 1u : 1u + Rand(n, nk()) % 255u) : 0u;
        return hi | lo;
    };
    auto Coord = [&] { return B(float(int(R(6001)) - 3000) + float(R(1000)) / 1000.0f); };

    // ---- reloj y estado de entrada
    {
        const u32 w = R(12);
        c.now = w == 0 ? 0u : w == 1 ? 1u : w == 2 ? 0xFFFFFFFFu : w == 3 ? 70000u : 1000u + R(2000000u);
    }
    c.b485 = P(4) ? 1u + R(255) : 0u;
    c.f428 = R(256) & ~0x09u;     // sin bit 0 (ley) ni bit 3 (bloqueado) por defecto
    c.f42A = R(256) & ~0x04u;     // sin desvanecido
    c.f42B = R(256) & ~0x0Au;     // sin convoy ni aparcado en parking
    c.f42C = R(256) & ~0x40u;
    c.target = P(15) ? kGarbageBase + 0x40u : 0u;
    c.entFlags = Rand(n, nk()) & ~0x10u;
    c.model = P(80) ? R(0x10000u) : R(3) == 0 ? 0x1A0u : R(2) ? 0x197u : 0x19Fu + R(3);
    if ((c.model == 0x1A0u || c.model == 0x197u) && P(70)) c.model ^= 0x2000u;
    c.created = P(70) ? R(3) : R(2) ? 3u : 4u + R(250);
    c.range = P(50) ? 0u : R(2) ? R(171) : R(600);
    if (P(8)) c.range = 0x10000u - R(400);   // negativo como int16 (movsx)
    c.till = P(40) ? 0u : c.now - R(100000);
    c.blown = P(20) ? 0u : c.now - R(200000);
    c.switched = c.now - (P(60) ? 5001u + R(50000) : R(6000));
    c.style = P(40) ? 0u : P(40) ? 4u : R(7);
    c.mission = P(10) ? 0x3Bu : R(70);
    c.tempact = P(10) ? 12u : R(25);
    c.vtype = P(55) ? 0u : P(25) ? 3u + R(2) : R(12);
    if (P(3)) c.vtype = R(2) ? 0xFFFFFFFFu : 0x80000003u;
    c.clump = P(60) ? 1u : P(50) ? 0u : 2u;
    c.fakePhys = R(256);
    c.hasMat = P(85) ? 1u : 0u;
    c.s36 = (P(30) ? 2u : P(35) ? 3u : P(40) ? 5u : R(12)) << 3 | R(8);
    for (int i = 0; i < 3; ++i) c.vp[i] = Coord();
    for (int i = 0; i < 3; ++i) c.mp[i] = Coord();
    for (int i = 0; i < 3; ++i) c.bp[i] = Coord();
    for (int i = 0; i < 3; ++i) c.cen[i] = Coord();
    {
        static constexpr std::array<u32, 12> upzs{0x3F800000u, 0xBF800000u, 0x3F3D70A4u, 0xBF3D70A4u, 0x3F3D70A3u, 0x3F3D70A5u, 0u, 0x80000000u, 0x3E800000u, 0xBE800000u, 0x3F000000u, 0xBF000000u};
        c.upz = P(60) ? Pick(upzs, n, nk()) : B(float(int(R(2001)) - 1000) / 1000.0f);
        c.bupz = P(50) ? Pick(upzs, n, nk()) : B(float(int(R(2001)) - 1000) / 1000.0f);
    }
    {
        static constexpr std::array<u32, 12> mults{0x3F800000u, 0x3E800000u, 0x3F000000u, 0x40000000u, 0u, 0x3E87878Au, 0x3E878788u, 0x3E878789u, 0x3F4CCCCDu, 0x3FC00000u, 0x3F333333u, 0x3DCCCCCDu};
        c.camMult = P(70) ? Pick(mults, n, nk()) : B(float(R(4000)) / 1000.0f);
        static constexpr std::array<u32, 8> lows{0xBF666666u, 0xBF666667u, 0xBF666665u, 0u, 0xBF800000u, 0x3F800000u, 0xBF333333u, 0xBFC00000u};
        c.camLow = P(55) ? Pick(lows, n, nk()) : B(float(int(R(3001)) - 1500) / 1000.0f);
    }
    c.activeCam = R(3);
    for (int i = 0; i < 9; ++i) c.cam[i] = P(8) ? (R(2) ? 1u : 1u + R(255)) : 0u;
    c.focus = R(256);
    c.rcbd[0] = Ret(!P(9)); c.rcbd[1] = Ret(!P(9));
    c.rcrane = Ret(P(10));
    c.ralpha = P(45) ? (R(3) == 0 ? (Rand(n, nk()) & 0xFFFFFF00u) | 0u : 0u) : (R(2) ? 1u + R(255) : Rand(n, nk()));
    if (c.ralpha == 0 && P(15)) c.ralpha = 0x100u;   // EAX distinto de cero con AL = 0: GetClumpAlpha se prueba entero
    for (int i = 0; i < 4; ++i) c.ron[i] = Ret(P(30));
    c.rlook = P(10) ? 0u : (R(2) ? 1u + R(5) : Rand(n, nk()) | 1u);
    for (int i = 0; i < 3; ++i) c.rgar[i] = Ret(P(10));
    for (int i = 0; i < 2; ++i) c.rint[i] = Ret(P(10));
    c.rlight = Ret(P(12)); c.rbridge = Ret(P(12));
    c.mutAt = P(30) ? R(24) : kNoMutation;
    c.mutKind = c.mutAt == kNoMutation ? 0u : 1u + R(kKinds);
    c.mutVal = P(25) ? Pick(kStackSeeds, n, nk()) : Rand(n, nk());

    // ---- sesgos por familia
    // posicion activa del vehiculo (la de la matriz si hay; si no la simple) a distancia horizontal D del centro, en una direccion elegida
    auto place = [&](float distance, int zmode) {
        static constexpr float dirs[][3] = {{1, 0, 0}, {0, 1, 0}, {0.6f, 0.8f, 0}, {0.8f, 0.6f, 0}, {-0.6f, 0.8f, 0}, {0.28f, -0.96f, 0}, {0, 0, 1}, {0.36f, 0.48f, 0.8f}, {-0.48f, 0.36f, 0.8f}, {0.6f, 0, 0.8f}};
        const auto& d = dirs[R(10)];
        const float sx = R(2) ? 1.0f : -1.0f, sy = R(2) ? 1.0f : -1.0f;
        const float a = distance * d[0] * sx, b = distance * d[1] * sy, z = distance * d[2] * (R(2) ? 1.0f : -1.0f);
        u32 x = B(F(c.cen[0]) + a), y = B(F(c.cen[1]) + b), zz = B(F(c.cen[2]) + (zmode ? z : 0.0f));
        const int ex = int(R(7)) - 3, ey = int(R(7)) - 3;
        if (R(3) == 0) x = Ulps(x, ex);
        if (R(3) == 0) y = Ulps(y, ey);
        u32* dst = c.hasMat ? c.mp : c.vp;
        dst[0] = x; dst[1] = y; dst[2] = zz;
    };
    auto enter = [&] {   // entra en la etapa 1: no bloqueado, borrable, sin grua, y no sale antes
        c.b485 = 0; c.f428 &= ~0x08u; c.f42B &= ~0x08u;
        c.rcbd[0] = Ret(true); c.rcrane = Ret(false);
    };
    auto statusAt = [&](u32 s) { c.s36 = s << 3 | R(8); };
    auto edgeRange = [&] {   // posicion a distancia ~ limite de la etapa 1 (alrededor de float(rango) con ulps)
        bool nr;
        const float t = Approx(c, nr);
        place((t == t && t < 1e6f && t > 0.0f) ? t : 45.0f, 0);
    };
    switch (f) {
    case 0: break;
    case 1:   // lejos y elimina: pasa todas las puertas de la etapa 1
        enter(); c.f42A &= ~0x04u; c.rgar[0] = Ret(false); c.rint[0] = Ret(false); c.ron[1] = Ret(false); c.mission = R(0x3Bu);
        if (P(70)) for (int i = 0; i < 9; ++i) c.cam[i] = 0;
        if (P(70)) { c.range = R(171); c.camLow = B(0.5f); }
        { bool nr; const float t = Approx(c, nr); place(t * (1.0f + float(R(1000)) / 500.0f) + 1.0f, 0); }
        break;
    case 2:   // lejos pero no elimina directamente: mision 59, garaje, interesante, desvanecido
        enter(); c.f42A &= ~0x04u;
        { bool nr; const float t = Approx(c, nr); place(t * 1.5f + 5.0f, 0); }
        switch (R(5)) {
        case 0: c.mission = 0x3Bu; break;
        case 1: c.rgar[0] = Ret(true); break;
        case 2: c.rint[0] = Ret(true); break;
        case 3: c.ron[1] = Ret(true); break;
        default: c.ron[1] = Ret(false); break;
        }
        break;
    case 3:   // rango: condiciones de la camara y valores 45 / 70 / multiplicador, con la distancia en el borde
        enter(); c.f42A &= ~0x04u;
        c.ron[0] = Ret(P(30));
        if (P(80)) { c.cam[c.activeCam * 3] = 0; c.cam[c.activeCam * 3 + 1] = 0; c.cam[c.activeCam * 3 + 2] = 0; }
        if (P(80)) { c.created = 1; c.model = 0x100u; c.f428 &= ~1u; c.f42B &= ~2u; c.f42C &= ~0x40u; c.till = 0; c.rlook = 1; }
        c.rgar[0] = Ret(false); c.rint[0] = Ret(false); c.ron[1] = Ret(false); c.mission = 0;
        edgeRange();
        break;
    case 4:   // puertas de salida: ocupantes, convoy, bloqueado, no borrable, grua, desvanecido; no entra en la etapa 1 -> estados seguros
        switch (R(8)) {
        case 0: c.b485 = 1u + R(255); break;
        case 1: c.f42B |= 0x08u; c.target = kGarbageBase + 0x40u; break;
        case 2: c.f42B |= 0x08u; c.target = 0; break;
        case 3: c.f428 |= 0x08u; break;
        case 4: c.rcbd[0] = Ret(false); break;
        case 5: c.rcrane = Ret(true); break;
        case 6: enter(); c.f42A |= 0x04u; break;
        default: enter(); c.f42A |= 0x04u; c.ralpha = R(2) ? 0u : 1u + R(1000); break;
        }
        break;
    case 5:   // volcado y atascado: estados 2, 3, 5; tipo helicoptero/avion; bit 4 de las banderas; z del eje up en el borde
        enter(); c.f42A &= ~0x04u; c.rgar[0] = Ret(false); c.rint[0] = Ret(false);
        { bool nr; const float t = Approx(c, nr); place(t * (0.2f + float(R(700)) / 1000.0f), 0); }
        statusAt(R(3) == 0 ? 2u : R(2) ? 3u : 5u);
        if (StatusOf(c) == 2) c.hasMat = 1;
        c.vtype = R(3) == 0 ? 0u : 3u + R(2);
        c.entFlags |= R(2) ? 0x10u : 0u;
        break;
    case 6:   // etapa 2: coche parado sin verse a mas de 22 m (distancia en el borde)
        enter(); c.f42A &= ~0x04u; c.rgar[0] = Ret(false); c.rint[0] = Ret(false);
        statusAt(R(2) ? 2u : 3u); c.style = R(2) ? 0u : 4u; c.vtype = R(4) == 0 ? 3u : 0u; c.tempact = R(10) == 0 ? 12u : 0u;
        c.till = 0; c.switched = c.now - (R(4) == 0 ? R(6000) : 5001u + R(100));
        c.ron[2] = Ret(false);
        if (c.now == 0) c.now = 70000u;
        c.switched = c.now - (R(4) == 0 ? R(6000) : 5001u + R(100));
        place(R(4) == 0 ? 10.0f + float(R(20)) : 22.0f, 0);
        c.rint[1] = Ret(P(10)); c.rcbd[1] = Ret(!P(10)); c.rlight = Ret(P(10)); c.rbridge = Ret(P(10)); c.rgar[1] = Ret(P(10));
        { bool nr; const float t = Approx(c, nr); if (t < 22.0f) c.range = 600; }   // que la etapa 1 no elimine antes
        if (StatusOf(c) == 2) c.hasMat = 1;
        if (P(60)) { c.mutAt = R(10); c.mutKind = 1u + R(kKinds); }
        break;
    case 7:   // etapa 3: restos destruidos; magnitud 3D en el borde de 6.5 m
        enter(); c.f42A &= ~0x04u; c.rgar[0] = Ret(false); c.rint[0] = Ret(false);
        statusAt(5u); c.vtype = 0u; c.till = P(50) ? 0u : c.now - R(100000);
        c.blown = c.now - (R(3) == 0 ? 59990u + R(20) : 60000u + R(100000));
        if (P(15)) c.blown = 0;
        c.ron[3] = Ret(P(10)); c.rgar[2] = Ret(P(15)); c.ron[2] = Ret(P(60));
        c.switched = c.now - R(5000);
        place(6.5f, 1);
        c.range = 600;
        if (P(60)) { c.mutAt = R(10); c.mutKind = 1u + R(kKinds); }
        break;
    case 8:   // especiales x87 en posiciones, centro, multiplicador de la camara, limite bajo y z del eje up
        enter(); c.f42A &= ~0x04u;
        for (int i = 0; i < 3; ++i) { if (P(35)) c.mp[i] = Special(n, nk(), 3); if (P(35)) c.vp[i] = Special(n, nk(), 3); if (P(30)) c.cen[i] = Special(n, nk(), 3); }
        if (P(35)) c.camMult = Special(n, nk(), 3);
        if (P(25)) c.camLow = Special(n, nk(), 3);
        if (P(40)) c.upz = Special(n, nk(), 3);
        if (P(30)) { c.mp[R(2)] = Special(n, nk(), R(2) ? 1u : 2u); c.cen[R(2)] = Special(n, nk(), R(2) ? 1u : 2u); }
        c.rgar[0] = Ret(P(30)); c.rint[0] = Ret(P(30));
        if (P(50)) statusAt(R(2) ? 2u : (R(2) ? 5u : 3u));
        if (StatusOf(c) == 2) c.hasMat = 1;
        if (P(30)) c.range = R(2) ? 0u : 600;
        break;
    case 9:   // sin matriz: la posicion simple; matrices alternativas
        c.hasMat = P(70) ? 0u : 1u;
        if (!c.hasMat && StatusOf(c) == 2) statusAt(3u);
        if (P(70)) enter();
        edgeRange();
        break;
    case 10: case 11: case 12:   // barrido de mutaciones: cada evento x cada tipo en casos que entran y recorren varias etapas
        enter(); c.f42A &= ~0x04u;
        c.rgar[0] = Ret(P(30)); c.rint[0] = Ret(P(30)); c.ron[1] = Ret(P(40)); c.mission = P(80) ? 0u : 0x3Bu;
        c.hasMat = P(90) ? 1u : 0u;
        switch (R(5)) {
        case 0: { bool nr; const float t = Approx(c, nr); place(t * (0.9f + float(R(300)) / 1000.0f), 0); break; }
        case 1: place(22.0f + float(R(100)) / 10.0f, 0); break;
        case 2: place(6.5f + float(R(100)) / 10.0f, 1); break;
        case 3:   // cadena completa de la etapa 2 (trazas largas)
            statusAt(R(2) ? 2u : 3u); c.style = R(2) ? 0u : 4u; c.vtype = 0u; c.tempact = 0u; c.till = 0u; c.switched = c.now - 5001u - R(100); if (c.now == 0) c.now = 70000u;
            c.switched = c.now - 5001u - R(100); c.ron[2] = Ret(false); c.rint[1] = Ret(false); c.rcbd[1] = Ret(true); c.rlight = Ret(false); c.rbridge = Ret(false); c.rgar[1] = Ret(false);
            c.range = 600; place(22.5f + float(R(200)) / 10.0f, 0);
            break;
        default:  // cadena completa de la etapa 3
            statusAt(5u); c.vtype = 0u; c.till = 0u; c.blown = c.now - 60001u - R(1000); c.ron[3] = Ret(false); c.rgar[2] = Ret(false); c.range = 600; place(6.6f + float(R(100)) / 10.0f, 1);
            break;
        }
        if (c.mutKind != 7 && StatusOf(c) == 2) c.hasMat = P(95) ? 1u : c.hasMat;
        if (R(5) == 0) { statusAt(R(4) == 0 ? 2u : R(3) == 0 ? 5u : 3u); c.style = R(2) ? 0u : 4u; c.vtype = R(5) == 0 ? 3u + R(2) : 0u; c.tempact = 0; }
        c.till = P(70) ? 0u : c.till; c.switched = c.now - 5001u - R(100); c.blown = c.now - 60001u - R(1000);
        if (f == 10) { c.mutAt = (n / 16) % 10; c.mutKind = 1u + (n / 16 / 10) % 13u; }
        else if (f == 11) { c.mutAt = (n / 16) % 10; c.mutKind = 14u + (n / 16 / 10) % 14u; if (c.mutKind > kKinds) c.mutKind = 1u + R(kKinds); }
        else { c.mutAt = R(10); c.mutKind = 1u + R(kKinds); }
        break;
    case 13:  // bordes de reloj: till, switched, blown con envoltura
        enter(); c.f42A &= ~0x04u;
        { const u32 w = R(6); c.now = w == 0 ? 0u : w == 1 ? 4294967295u : w == 2 ? 5000u : w == 3 ? 60000u : 65000u + R(1000); }
        { const u32 w = R(6); c.switched = w == 0 ? c.now - 5000u : w == 1 ? c.now - 5001u : w == 2 ? c.now - 4999u : w == 3 ? c.now + 1u : w == 4 ? 0u : c.now - R(10000); }
        { const u32 w = R(7); c.blown = w == 0 ? c.now - 60000u : w == 1 ? c.now - 60001u : w == 2 ? c.now - 59999u : w == 3 ? 1u : w == 4 ? 0xFFFFFFFFu : w == 5 ? 60000u : c.now - R(100000); }
        { const u32 w = R(5); c.till = w == 0 ? c.now : w == 1 ? c.now + 1u : w == 2 ? c.now - 1u : w == 3 ? 0u : 1u; }
        statusAt(R(2) ? 5u : (R(2) ? 2u : 3u)); c.style = R(2) ? 0u : 4u; c.rgar[0] = Ret(false); c.rint[0] = Ret(false);
        if (StatusOf(c) == 2) c.hasMat = 1;
        place(P(50) ? 25.0f + float(R(20)) : 6.6f + float(R(10)) / 5.0f, 1);
        c.range = 600;
        break;
    case 14: case 15: break;   // todo aleatorio
    case 16:  // NaN e infinitos en varios sitios a la vez, ranges 45 / 70
        enter(); c.f42A &= ~0x04u;
        for (int i = 0; i < 3; ++i) { if (P(25)) c.mp[i] = Special(n, nk(), 3); if (P(25)) c.cen[i] = Special(n, nk(), 3); if (P(15)) c.vp[i] = Special(n, nk(), 3); }
        if (P(25)) c.camMult = Special(n, nk(), 3);
        if (P(25)) c.camLow = Special(n, nk(), 3);
        if (P(30)) c.upz = Special(n, nk(), 3);
        c.rgar[0] = Ret(P(30)); c.rint[0] = Ret(P(30));
        statusAt(R(3) == 0 ? 5u : R(2) ? 2u : 3u); if (StatusOf(c) == 2) c.hasMat = 1;
        c.blown = c.now - 60001u - R(100); c.switched = c.now - 5001u - R(100); c.till = 0;
        break;
    default:  // 17: barrido de ulps de los tres umbrales con cada parte del calculo
        enter(); c.f42A &= ~0x04u;
        c.rgar[0] = Ret(false); c.rint[0] = Ret(false); c.ron[1] = Ret(false); c.mission = 0;
        c.switched = c.now - 5001u - R(100); c.till = 0; c.blown = c.now - 60001u - R(1000);
        switch (R(3)) {
        case 0: edgeRange(); break;
        case 1: statusAt(R(2) ? 2u : 3u); c.style = 0; c.vtype = 0; c.tempact = 0; c.ron[2] = Ret(false); c.rgar[1] = Ret(false); c.rint[1] = Ret(false); c.rcbd[1] = Ret(true); c.rlight = Ret(false); c.rbridge = Ret(false); c.range = 600; place(22.0f, 0); break;
        default: statusAt(5u); c.vtype = 0; c.ron[3] = Ret(false); c.rgar[2] = Ret(false); c.range = 600; place(6.5f, 1); break;
        }
        if (StatusOf(c) == 2) c.hasMat = 1;
        break;
    }
    // ---- reglas de seguridad
    c.entered = Entered(c) ? 1u : 0u;
    if (!c.entered) {
        if (StatusOf(c) == 2 || StatusOf(c) == 3 || StatusOf(c) == 5) c.s36 = (R(2) ? 4u : 1u) << 3 | (c.s36 & 7u);
        if (c.mutKind == 7) c.mutKind = 3u;
    }
    if (!c.hasMat) {
        if (StatusOf(c) == 2) c.s36 = 3u << 3 | (c.s36 & 7u);
        if (c.mutKind == 7) c.mutKind = 3u;
    }
    if (c.mutKind == 7) c.mutVal = (c.mutVal & ~0xF8u) | ((c.mutVal % 12u) << 3);
    c.gate = c.entered ? 2u : 1u;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack).U("now", c.now);
    j.U("b485", c.b485).U("f428", c.f428).U("f42A", c.f42A).U("f42B", c.f42B).U("f42C", c.f42C).U("s36", c.s36).U("ent_flags", c.entFlags).U("model", c.model).U("created", c.created).U("range", c.range);
    j.U("till", c.till).U("blown", c.blown).U("switched", c.switched).U("style", c.style).U("mission", c.mission).U("tempact", c.tempact).U("vtype", c.vtype).U("target", c.target).U("clump", c.clump);
    j.U("fake_phys", c.fakePhys).U("has_mat", c.hasMat);
    for (int i = 0; i < 3; ++i) j.U((std::string("vp") + char('0' + i)).c_str(), c.vp[i]);
    for (int i = 0; i < 3; ++i) j.U((std::string("mp") + char('0' + i)).c_str(), c.mp[i]);
    j.U("upz", c.upz);
    for (int i = 0; i < 3; ++i) j.U((std::string("bp") + char('0' + i)).c_str(), c.bp[i]);
    j.U("bupz", c.bupz);
    for (int i = 0; i < 3; ++i) j.U((std::string("cen") + char('0' + i)).c_str(), c.cen[i]);
    j.U("cam_mult", c.camMult).U("cam_low", c.camLow).U("active_cam", c.activeCam).U("focus", c.focus);
    for (int i = 0; i < 9; ++i) j.U((std::string("cam") + char('0' + i)).c_str(), c.cam[i]);
    j.U("rcbd0", c.rcbd[0]).U("rcbd1", c.rcbd[1]).U("rcrane", c.rcrane).U("ralpha", c.ralpha);
    for (int i = 0; i < 4; ++i) j.U((std::string("ron") + char('0' + i)).c_str(), c.ron[i]);
    j.U("rlook", c.rlook);
    for (int i = 0; i < 3; ++i) j.U((std::string("rgar") + char('0' + i)).c_str(), c.rgar[i]);
    j.U("rint0", c.rint[0]).U("rint1", c.rint[1]).U("rlight", c.rlight).U("rbridge", c.rbridge);
    j.U("mut_at", c.mutAt).U("mut_kind", c.mutKind).U("mut_val", c.mutVal).U("entered", c.entered).U("gate", c.gate);
    return j.End();
}
}

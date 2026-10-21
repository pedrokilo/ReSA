#pragma once
// Fixtures del banco CarCtrlSteer63 (lote 63, CCarCtrl parte B1): 3 funciones x 4096 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp
// y un programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
//   CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget_Stop  0x428990  cdecl (vehicle, x, y, a4, a5, float* o6, float* o7, float* o8, bool* o9), 9 argumentos, void
//   CCarCtrl::SteerAICarWithPhysicsHeadingForTarget          0x433280  cdecl (vehicle, target, x, y, float* steer, float* accel, float* brake, bool* handbrake), 8 argumentos, void
//   CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget       0x4335E0  cdecl (vehicle, unused, a3, a4, a5, a6, float* steer, float* accel, float* brake, bool* handbrake), 10 argumentos, void
//
// Cada caso es una descripcion SEMANTICA (la leen el banco y fixture_oracle.py): vehiculo (posicion, matriz, velocidad, estilo, mision, ...), argumentos, tablas de resultados de los
// colaboradores sustituidos (por orden de llamada) y estado real que lee la funcion (CTimer::ms_fTimeStep).
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

namespace CS63Fix {
using i16 = std::int16_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i32 = std::int32_t;

constexpr unsigned kFns = 3;
constexpr unsigned kPerFn = 4096;
constexpr unsigned kCases = kFns * kPerFn;
constexpr unsigned kFamilies = 8;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 54321u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline u32 Bits(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
inline float Flt(u32 v) { float f; std::memcpy(&f, &v, 4); return f; }
inline u64 DBits(double d) { u64 v; std::memcpy(&v, &d, 8); return v; }
inline double DFlt(u64 v) { double d; std::memcpy(&d, &v, 8); return d; }

// ------------------------------------------------------------------------------------------------ disposicion (bytes del vehiculo del EXE)
constexpr u32 kVehSize = 0x900, kMatWords = 24, kGuardWords = 4, kPedSize = 0x500, kIntelWords = 8, kTaskWords = 4, kVtWords = 8, kOutWords = 8;
constexpr u32 kVPos = 0x04, kVMat = 0x14, kVSpeed = 0x44, kVStyle = 0x3B9, kVMission = 0x3BA, kVCruise = 0x3D0, kVFlags = 0x428, kVDriver = 0x460, kVCops = 0x4DC, kVSub = 0x594, kVTimer = 0x818;
constexpr u32 kPedIntel = 0x47C;
constexpr u32 kMTop = 0x10, kMPos = 0x30;
constexpr u32 kTimeStepAddr = 0xB7CB5C;
constexpr u32 kGarbageSeed = 0x3C6EF372u;
inline u32 Garbage(u32 n, u32 k) {
    u32 v = 0xC0000000u | (Rand(n + kGarbageSeed, k) & 0x3FFFFFFFu);
    return v == 0xFFFFFFFFu ? 0xC0000000u : v;
}

enum Fn : u32 { kStop = 0, kHeading = 1, kBlock = 2 };

// tipos de colaborador (indice en los eventos)
enum : u32 { kFpv = 1, kGetActive, kUpsideDown, kTell, kTaskType, kAtan, kWeave, kMaxSteer, kSpeedMul, kHeadingFake, kKinds };

struct Spec {
    u32 fn{}, family{}, regs[4]{};
    u32 hasMat{1}, hasDriver{}, intelNull{}, taskKind{}, target{};
    u32 vpos[3]{}, mpos[3]{}, mtop[3]{}, msp[3]{};
    u32 style{}, mission{}, cruise{}, flags428{}, cops{}, sub594{}, f818{};
    u32 in[4]{};              // argumentos float (bits): Stop (x, y, a4, a5); Heading (x, y); Block (a3, a4, a5, a6)
    u32 unusedArg{};
    u32 pvMask{}, upMask{};
    u32 pv[3]{};              // velocidad del vehiculo del jugador
    u32 step{};
    u64 atan[3]{};            // resultados (double) de 0x53CC70 por orden de llamada
    u32 weave{}, maxSteer{}, speedMul{};
    u32 hOut[4]{};            // HeadingForTarget falso: steer, accel, brake y byte del freno de mano
    u32 outPre[4]{};
};

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_) : n(n_) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
};
constexpr u32 kQNaN = 0x7FC00000u, kInf = 0x7F800000u, kNegInf = 0xFF800000u, kNegZero = 0x80000000u;

template <std::size_t N>
inline u32 PickF(Gen& g, const float (&v)[N]) { return Bits(v[g.P(u32(N))]); }
inline float Grid(Gen& g, int span, float step) { return float(int(g.P(u32(2 * span + 1))) - span) * step; }
inline u32 AnyNum(Gen& g) {   // numero cualquiera de uso general
    switch (g.P(10)) {
    case 0: return 0u;
    case 1: return kNegZero;
    case 2: return kQNaN;
    case 3: return Bits(Grid(g, 400, 0.25f));
    case 4: return Bits(Grid(g, 100, 0.01f));
    case 5: return Bits(float(g.P(2000)) * 0.001f - 1.0f);
    default: return Bits(Grid(g, 1000, 0.125f));
    }
}
inline u32 Sprinkle(Gen& g, u32 normal) {   // casi siempre `normal`; a veces NaN / -0.0
    if (!g.C(5)) return normal;
    static const u32 odd[]{kQNaN, kNegZero, 0u};
    return odd[g.P(3)];
}
inline u64 PickAtan(Gen& g) {   // rumbo (double con mantisa completa): casi siempre en [-pi, pi]; a veces fuera (bucles de normalizacion), nunca infinito ni enorme
    switch (g.P(12)) {
    case 0: return DBits(0.0);
    case 1: return DBits(3.14159265358979);
    case 2: return DBits(-3.14159265358979);
    case 3: return DBits(double(g.P(2000001)) / 1000000.0 * 12.0 - 12.0);
    case 4: return DBits(double(g.P(2000001)) / 1000000.0 * 40.0 - 40.0);
    case 5: return DBits(std::numeric_limits<double>::quiet_NaN());
    case 6: return DBits(double(Flt(Bits(float(g.P(2000001)) / 1000000.0f * 6.0f - 6.0f))));
    default: {
        const double a = (double(g.R() >> 8) / double(1u << 24)) * 6.283185307179586 - 3.141592653589793;
        return DBits(a);
    }
    }
}

inline void GenVehicle(Gen& g, Spec& s) {
    for (int i = 0; i < 3; ++i) {
        s.vpos[i] = g.C(60) ? Bits(Grid(g, 400, 0.25f)) : Garbage(g.n, 10 + i);
        s.mpos[i] = g.C(60) ? Bits(Grid(g, 400, 0.25f)) : Garbage(g.n, 20 + i);
    }
    s.hasMat = g.C(75) ? 1u : 0u;
    s.style = g.P(8);
    s.mission = g.R() & 0xFF;
    s.cruise = g.C(60) ? g.P(61) : (g.R() & 0xFF);
    s.flags428 = g.R() & 0xFF;
    s.cops = g.R() & 0xFFFF;
    s.sub594 = g.C(50) ? 10u : (g.C(50) ? g.P(12) : g.R());
    {
        static const float t[]{0.0f, -0.0f, 1.0f, -1.0f, 5.0f, 10.0f, -5.0f};
        s.f818 = g.C(10) ? kQNaN : PickF(g, t);
    }
    s.unusedArg = g.C(50) ? 0u : g.R();
    for (int i = 0; i < 4; ++i) s.outPre[i] = Garbage(g.n, 70 + i);
}

// posicion efectiva del vehiculo (la de la matriz si la tiene; si no, la del placement)
inline void EffPos(const Spec& s, float& x, float& y) { x = Flt(s.hasMat ? s.mpos[0] : s.vpos[0]); y = Flt(s.hasMat ? s.mpos[1] : s.vpos[1]); }

inline Spec MakeSpec(u32 n) {
    Spec s;
    Gen g(n);
    s.fn = n / kPerFn;
    s.family = n % kFamilies;
    s.regs[0] = StackSeed(n);
    s.regs[1] = g.C(30) ? Garbage(n, 1) : g.R();
    s.regs[2] = g.C(30) ? Garbage(n, 2) : g.R();
    s.regs[3] = g.C(30) ? Garbage(n, 3) : g.R();
    GenVehicle(g, s);
    static const float stepT[]{0.0f, 0.5f, 1.0f, 1.5f, 2.0f, 3.0f, 10.0f, 0.0001f, 1.0e10f, -1.0f, 1.0e30f, 0.25f, 0.9f, 1.1f};
    s.step = g.C(8) ? kQNaN : (g.C(40) ? PickF(g, stepT) : Bits(float(g.P(4001)) * 0.001f));
    for (int i = 0; i < 3; ++i) s.atan[i] = PickAtan(g);
    {
        static const float w[]{0.0f, 0.5f, -0.5f, 1.0f, 3.0f, -3.0f, 2.0f, 0.1f};
        s.weave = g.C(8) ? kQNaN : (g.C(50) ? PickF(g, w) : Bits(float(g.P(12001)) * 0.001f - 6.0f));
    }
    {
        static const float m[]{0.2f, 0.7f, 0.9f, 0.5f, 0.0f, 0.3f, 0.35f, 0.6f, 0.1f, -0.2f};
        s.maxSteer = g.C(6) ? kQNaN : (g.C(70) ? PickF(g, m) : Bits(float(g.P(1001)) * 0.001f));
    }
    {
        static const float m[]{0.0f, 1.0f, 0.5f, 2.0f, 1.5f, -1.0f, 2.5f, 0.25f, 0.1f, 3.0f, 4.0f};
        s.speedMul = g.C(6) ? kQNaN : (g.C(60) ? PickF(g, m) : Bits(float(g.P(4001)) * 0.001f - 1.0f));
    }
    {
        static const float h[]{0.0f, 0.5f, -0.5f, 1.0f, 0.3f, -0.1f, 0.7f};
        for (int i = 0; i < 3; ++i) s.hOut[i] = g.C(6) ? kQNaN : (g.C(70) ? PickF(g, h) : Bits(float(g.P(2001)) * 0.001f - 1.0f));
        s.hOut[3] = g.C(50) ? (g.R() & 0xFF) : (g.C(50) ? 1u : 0u);
    }
    float px{}, py{};
    EffPos(s, px, py);
    switch (s.fn) {
    case kStop: {
        // distancia al punto (6,8) = 10 exacto, justo por debajo / por encima, 0, lejos...
        static const float offs[][2]{{0, 0}, {3, 4}, {6, 8}, {6, 8.001f}, {6, 7.999f}, {-6, -8}, {8, 6}, {-8, 6}, {10, 0}, {0, -10}, {9.99f, 0}, {10.01f, 0}, {5, 5}, {7, 7}, {7.07f, 7.07f}, {30, 40}, {0.5f, 0.25f}, {-2, 9.5f}, {9, 4}, {1, 1}};
        const auto& o = offs[g.P(sizeof(offs) / sizeof(offs[0]))];
        if (g.C(85)) { s.in[0] = Bits(px + o[0]); s.in[1] = Bits(py + o[1]); } else { s.in[0] = AnyNum(g); s.in[1] = AnyNum(g); }
        static const float a45[][2]{{0, 0}, {0.01f, 0.01f}, {0.0141f, 0.0141f}, {0.0142f, 0.0142f}, {0.02f, 0}, {0, 0.02f}, {0.0199f, 0}, {0.0201f, 0}, {0.03f, 0.03f}, {-0.01f, 0.01f}, {0.001f, 0.002f}, {0.2f, 0.2f}};
        const auto& a = a45[g.P(sizeof(a45) / sizeof(a45[0]))];
        s.in[2] = g.C(8) ? Sprinkle(g, Bits(a[0])) : Bits(a[0]);
        s.in[3] = g.C(8) ? Sprinkle(g, Bits(a[1])) : Bits(a[1]);
        if (g.C(10)) { s.in[2] = AnyNum(g); s.in[3] = AnyNum(g); }
        static const u32 missions[]{0x12u, 5u, 6u, 0u, 0x14u};
        s.mission = g.C(55) ? 0x13u : (g.C(60) ? missions[g.P(5)] : (g.R() & 0xFF));
        static const float sp[]{0.0f, 0.005f, 0.00707f, 0.01f, 0.0099f, 0.011f, -0.007f, 0.0071f, 0.02f, 0.0001f, -0.0099f};
        s.msp[0] = g.C(6) ? kQNaN : PickF(g, sp);
        s.msp[1] = g.C(6) ? kQNaN : PickF(g, sp);
        s.msp[2] = Bits(Grid(g, 100, 0.01f));
        s.hasDriver = g.C(78) ? 1u : 0u;
        s.intelNull = g.C(8) ? 1u : 0u;
        s.taskKind = g.P(3);
        s.flags428 = g.C(60) ? (s.flags428 | 1u) : (s.flags428 & ~1u);
        static const u32 copsT[]{0, 1, 100, 2483, 2490, 2499, 2500, 2501, 2510, 65535, 65530, 2480};
        if (g.C(60)) s.cops = copsT[g.P(sizeof(copsT) / sizeof(copsT[0]))];
        s.pvMask = g.C(15) ? 0u : (g.C(15) ? ~0u : g.R());
        {   // llamadas a FindPlayerVehicle por orden: [prueba, velocidad]? [prueba, objetivo de IsUpsideDown, velocidad]?; las que se desreferencian (velocidad) deben ser no nulas
            u32 k = 0;
            const bool first = (s.pvMask >> k) & 1u;
            ++k;
            if (first) { s.pvMask |= 1u << k; ++k; }
            const bool second = (s.pvMask >> k) & 1u;
            ++k;
            if (second) { ++k; s.pvMask |= 1u << k; }
        }
        s.upMask = g.R();
        static const float pvs[]{0.0f, 0.02f, 0.0288f, 0.0289f, 0.03f, 0.05f, 0.1f, -0.02f, 0.04f};
        for (int i = 0; i < 3; ++i) s.pv[i] = g.C(5) ? kQNaN : PickF(g, pvs);
        break;
    }
    case kHeading: {
        s.hasMat = 1;   // el EXE desreferencia la matriz sin comprobarla
        s.target = g.C(50) ? 1u : 0u;
        static const float off[][2]{{0, 0}, {10, 0}, {0, 10}, {-5, 5}, {100, 100}, {1, -1}, {0.5f, 0.5f}, {-20, 3}, {3, -20}, {1000, 0}};
        const auto& o = off[g.P(sizeof(off) / sizeof(off[0]))];
        if (g.C(90)) { s.in[0] = Bits(px + o[0]); s.in[1] = Bits(py + o[1]); } else { s.in[0] = AnyNum(g); s.in[1] = AnyNum(g); }
        // direccion de avance: unitaria en general; a veces nula, pequena, grande o NaN
        switch (g.P(10)) {
        case 0: s.mtop[0] = 0; s.mtop[1] = 0; break;
        case 1: s.mtop[0] = kNegZero; s.mtop[1] = 0; break;
        case 2: s.mtop[0] = 0; s.mtop[1] = Bits(Grid(g, 20, 0.05f)); break;
        case 3: s.mtop[0] = Bits(Grid(g, 20, 0.05f)); s.mtop[1] = 0; break;
        case 4: s.mtop[0] = Bits(float(g.P(2001)) * 0.001f - 1.0f); s.mtop[1] = Bits(float(g.P(2001)) * 0.001f - 1.0f); break;
        case 5: s.mtop[0] = Bits(float(g.P(2001)) - 1000.0f); s.mtop[1] = Bits(float(g.P(2001)) - 1000.0f); break;
        case 6: s.mtop[0] = Bits(float(g.P(2001)) * 1.0e-6f - 1.0e-3f); s.mtop[1] = Bits(float(g.P(2001)) * 1.0e-6f - 1.0e-3f); break;
        case 7: s.mtop[0] = kQNaN; s.mtop[1] = Bits(Grid(g, 20, 0.05f)); break;
        default: {
            const double a = double(g.P(62832)) * 0.0001;
            s.mtop[0] = Bits(float(std::cos(a))); s.mtop[1] = Bits(float(std::sin(a)));
            break;
        }
        }
        s.mtop[2] = AnyNum(g);
        // velocidad: |v|*60 alrededor de 25 (|v| ~ 0.4167) y de 0.3
        static const float sp[]{0.0f, 0.1f, 0.2f, 0.25f, 0.3f, 0.31f, 0.4f, 0.41f, 0.42f, 0.5f, -0.2f, -0.4f, 0.17f, 0.0001f, 1.0f};
        for (int i = 0; i < 3; ++i) s.msp[i] = g.C(5) ? kQNaN : (g.C(70) ? PickF(g, sp) : Bits(float(g.P(1601)) * 0.001f - 0.8f));
        break;
    }
    default: {   // kBlock
        static const float off[][2]{{0, 0}, {3, 4}, {-3, 4}, {5, 0}, {4.9f, 0}, {5.1f, 0}, {0, -5}, {10, 10}, {30, -30}, {1, 1}, {-2.5f, 2.5f}};
        const auto& o = off[g.P(sizeof(off) / sizeof(off[0]))];
        // (arg3, arg4) = punto; (arg5, arg6) = velocidad del objetivo, que se proyecta 60 fotogramas
        if (g.C(80)) { s.in[0] = Bits(px + o[0]); s.in[1] = Bits(py + o[1]); } else { s.in[0] = AnyNum(g); s.in[1] = AnyNum(g); }
        static const float d[]{0.0f, 0.01f, -0.01f, 0.05f, 0.09f, 0.1f, 0.13f, 0.1301f, 0.2f, 0.5f, -0.3f, 1.0f, 0.0919f, 0.0921f, -0.13f, 0.001f};
        s.in[2] = g.C(5) ? Sprinkle(g, kQNaN) : (g.C(85) ? PickF(g, d) : Bits(float(g.P(2001)) * 0.001f - 1.0f));
        s.in[3] = g.C(5) ? Sprinkle(g, kQNaN) : (g.C(85) ? PickF(g, d) : Bits(float(g.P(2001)) * 0.001f - 1.0f));
        if (g.C(3)) s.in[2] = kInf;
        if (g.C(3)) s.in[3] = kNegInf;
        s.mission = g.C(45) ? 0x12u : (g.C(50) ? 6u : (g.R() & 0xFF));
        break;
    }
    }
    return s;
}

inline std::string U(u64 v) { return std::to_string(v); }
inline std::string Arr(const u32* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string Arr64(const u64* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string JsonSpec(const Spec& s) {
    return "{\"fn\":" + U(s.fn) + ",\"fam\":" + U(s.family) + ",\"regs\":" + Arr(s.regs, 4) + ",\"mat\":" + U(s.hasMat) + ",\"driver\":" + U(s.hasDriver) + ",\"intel_null\":" + U(s.intelNull) +
           ",\"task\":" + U(s.taskKind) + ",\"vpos\":" + Arr(s.vpos, 3) + ",\"mpos\":" + Arr(s.mpos, 3) + ",\"mtop\":" + Arr(s.mtop, 3) + ",\"msp\":" + Arr(s.msp, 3) + ",\"style\":" + U(s.style) +
           ",\"mission\":" + U(s.mission) + ",\"cruise\":" + U(s.cruise) + ",\"flags\":" + U(s.flags428) + ",\"cops\":" + U(s.cops) + ",\"sub\":" + U(s.sub594) + ",\"f818\":" + U(s.f818) +
           ",\"in\":" + Arr(s.in, 4) + ",\"unused\":" + U(s.unusedArg) + ",\"pvmask\":" + U(s.pvMask) + ",\"upmask\":" + U(s.upMask) + ",\"pv\":" + Arr(s.pv, 3) + ",\"step\":" + U(s.step) +
           ",\"atan\":" + Arr64(s.atan, 3) + ",\"weave\":" + U(s.weave) + ",\"maxsteer\":" + U(s.maxSteer) + ",\"speedmul\":" + U(s.speedMul) + ",\"hout\":" + Arr(s.hOut, 4) +
           ",\"outpre\":" + Arr(s.outPre, 4) + ",\"target\":" + U(s.target) + "}";
}
}  // namespace CS63Fix

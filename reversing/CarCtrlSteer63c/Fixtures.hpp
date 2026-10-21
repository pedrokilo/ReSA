#pragma once
// Fixtures del banco CarCtrlSteer63c (lote 63, CCarCtrl parte B1, tanda 3): SteerAICarTowardsPointInEscort (0x4336D0), 4096 casos. Solo C++ estandar (sin tipos del juego).
//
//   CCarCtrl::SteerAICarTowardsPointInEscort  0x4336D0  cdecl (CVehicle* escorted/leader?, CVehicle* v2, float x, float y, float* steer, float* accel, float* brake, bool* handbrake), 8 argumentos, void
//   (en el EXE el primero es el vehiculo que conduce y el segundo el vehiculo escoltado)
//
// Cada caso es una descripcion SEMANTICA (la leen el banco y fixture_oracle.py): dos vehiculos con matriz completa (el EXE llama a 0x59C890 de verdad: matriz x vector), velocidades,
// resultados de los colaboradores sustituidos por orden de llamada (GetATanOfXY devuelve un double sin redondear).
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

namespace CS63CFix {
using i16 = std::int16_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i32 = std::int32_t;

constexpr unsigned kFns = 1;
constexpr unsigned kPerFn = 4096;
constexpr unsigned kCases = kFns * kPerFn;
constexpr unsigned kFamilies = 8;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 777u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline u32 Bits(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
inline float Flt(u32 v) { float f; std::memcpy(&f, &v, 4); return f; }
inline u64 DBits(double d) { u64 v; std::memcpy(&v, &d, 8); return v; }
inline double DFlt(u64 v) { double d; std::memcpy(&d, &v, 8); return d; }

// ------------------------------------------------------------------------------------------------ disposicion (bytes del vehiculo del EXE)
constexpr u32 kVehSize = 0x900, kMatWords = 16, kGuardWords = 4, kOutWords = 8;
constexpr u32 kVMat = 0x14, kVSpeed = 0x44;
constexpr u32 kGarbageSeed = 0x2B992DDFu;
inline u32 Garbage(u32 n, u32 k) {
    u32 v = 0xC0000000u | (Rand(n + kGarbageSeed, k) & 0x3FFFFFFFu);
    return v == 0xFFFFFFFFu ? 0xC0000000u : v;
}

// tipos de colaborador (indice en los eventos)
enum : u32 { kAtan = 1, kWeave, kMaxSteer, kKinds };

struct Spec {
    u32 family{}, regs[4]{};
    u32 m1[kMatWords]{}, m2[kMatWords]{};   // matrices de los dos vehiculos (right, top, at, pos; 4 palabras cada una)
    u32 sp1[3]{}, sp2[3]{};
    u32 in[2]{};
    u64 atan[3]{};
    u32 weave{}, maxSteer{};
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
inline u64 PickAtan(Gen& g) {
    switch (g.P(12)) {
    case 0: return DBits(0.0);
    case 1: return DBits(3.14159265358979);
    case 2: return DBits(-3.14159265358979);
    case 3: return DBits(double(g.P(2000001)) / 1000000.0 * 12.0 - 12.0);
    case 4: return DBits(double(g.P(2000001)) / 1000000.0 * 40.0 - 40.0);
    case 5: return DBits(std::numeric_limits<double>::quiet_NaN());
    case 6: return DBits(double(Flt(Bits(float(g.P(2000001)) / 1000000.0f * 6.0f - 6.0f))));
    default: return DBits((double(g.R() >> 8) / double(1u << 24)) * 6.283185307179586 - 3.141592653589793);
    }
}

// matriz de rotacion alrededor de Z (right = (cos, sin, 0), top = (-sin, cos, 0), at = (0, 0, 1)) con pequenas perturbaciones o valores especiales
inline void GenMatrix(Gen& g, u32* m, double angle, float px, float py, float pz) {
    const float c = float(std::cos(angle)), s = float(std::sin(angle));
    const float v[16]{c, s, 0.0f, 0.0f, -s, c, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, px, py, pz, 0.0f};
    for (int i = 0; i < 16; ++i) m[i] = Bits(v[i]);
    if (g.C(25)) { for (int i : {0, 1, 2, 4, 5, 6, 8, 9, 10}) if (g.C(40)) m[i] = Bits(Flt(m[i]) + Grid(g, 4, 0.05f)); }
    if (g.C(3)) m[g.P(3) + 4 * g.P(3)] = kQNaN;
    for (int i : {3, 7, 11, 15}) m[i] = Garbage(g.n, 300 + u32(i));
}

inline Spec MakeSpec(u32 n) {
    Spec s;
    Gen g(n);
    s.family = n % kFamilies;
    s.regs[0] = StackSeed(n);
    s.regs[1] = g.C(30) ? Garbage(n, 1) : g.R();
    s.regs[2] = g.C(30) ? Garbage(n, 2) : g.R();
    s.regs[3] = g.C(30) ? Garbage(n, 3) : g.R();
    for (int i = 0; i < 4; ++i) s.outPre[i] = Garbage(n, 70 + i);
    const double a2 = double(g.P(62832)) * 0.0001, a1 = double(g.P(62832)) * 0.0001;
    const float p2x = Grid(g, 400, 0.25f), p2y = Grid(g, 400, 0.25f), p2z = Grid(g, 40, 0.5f);
    GenMatrix(g, s.m2, a2, p2x, p2y, p2z);
    // velocidades
    static const float spd[]{0.0f, 0.05f, 0.1f, 0.2f, 0.3f, 0.4f, 0.41f, 0.42f, 0.5f, 0.7f, 1.0f, -0.1f, -0.3f};
    for (int i = 0; i < 3; ++i) {
        s.sp2[i] = g.C(4) ? kQNaN : (g.C(60) ? PickF(g, spd) : Bits(float(g.P(1601)) * 0.001f - 0.8f));
        s.sp1[i] = g.C(4) ? kQNaN : (g.C(60) ? PickF(g, spd) : Bits(float(g.P(1601)) * 0.001f - 0.8f));
    }
    // punto local respecto al vehiculo 2
    s.in[0] = g.C(3) ? kQNaN : Bits(Grid(g, 40, 0.5f));
    s.in[1] = g.C(3) ? kQNaN : Bits(Grid(g, 40, 0.5f));
    // posicion del objetivo (aprox. en doble) y del vehiculo 1 a una distancia/angulo dados
    const double lx = double(Flt(s.in[0])), ly = double(Flt(s.in[1]));
    double c2 = std::cos(a2), s2 = std::sin(a2);
    double tx = c2 * lx - s2 * ly + double(p2x) + double(Flt(s.sp2[0]));
    double ty = s2 * lx + c2 * ly + double(p2y) + double(Flt(s.sp2[1]));
    if (tx != tx) tx = 0.0;
    if (ty != ty) ty = 0.0;
    static const float dist[]{0.0f, 1.0f, 5.0f, 10.0f, 14.9f, 15.0f, 15.1f, 20.0f, 30.0f, 100.0f, 0.5f, 3.0f, 12.0f, 7.0f};
    static const float rel[]{0.0f, 0.05f, 0.5f, 1.0f, 1.5f, 1.6f, 2.0f, 3.0f, 3.14159f, 4.0f, 5.0f, 6.0f};   // angulo del objetivo respecto al avance del vehiculo 1
    const float d = g.C(75) ? dist[g.P(sizeof(dist) / sizeof(dist[0]))] : float(g.P(3001)) * 0.01f;
    const double phi = a1 + double(g.C(80) ? rel[g.P(sizeof(rel) / sizeof(rel[0]))] : float(g.P(6284)) * 0.001f);
    const float p1x = float(tx - d * std::cos(phi)), p1y = float(ty - d * std::sin(phi));
    GenMatrix(g, s.m1, a1, p1x, p1y, Grid(g, 40, 0.5f));
    // direccion de avance del vehiculo 1: especial a veces (nula, -0, NaN)
    switch (g.P(12)) {
    case 0: s.m1[4] = 0; s.m1[5] = 0; break;
    case 1: s.m1[4] = kNegZero; s.m1[5] = 0; break;
    case 2: s.m1[4] = Bits(Grid(g, 20, 0.05f)); s.m1[5] = 0; break;
    case 3: s.m1[4] = 0; s.m1[5] = Bits(Grid(g, 20, 0.05f)); break;
    case 4: s.m1[4] = kQNaN; break;
    default: break;
    }
    for (int i = 0; i < 3; ++i) s.atan[i] = PickAtan(g);
    {
        static const float w[]{0.0f, 0.5f, -0.5f, 1.0f, 3.0f, -3.0f, 2.0f, 0.1f};
        s.weave = g.C(8) ? kQNaN : (g.C(50) ? PickF(g, w) : Bits(float(g.P(12001)) * 0.001f - 6.0f));
    }
    {
        static const float m[]{0.2f, 0.7f, 0.9f, 0.5f, 0.0f, 0.3f, 0.35f, 0.6f, 0.1f, -0.2f};
        s.maxSteer = g.C(6) ? kQNaN : (g.C(70) ? PickF(g, m) : Bits(float(g.P(1001)) * 0.001f));
    }
    return s;
}

inline std::string U(u64 v) { return std::to_string(v); }
inline std::string Arr(const u32* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string Arr64(const u64* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string JsonSpec(const Spec& s) {
    return "{\"fam\":" + U(s.family) + ",\"regs\":" + Arr(s.regs, 4) + ",\"m1\":" + Arr(s.m1, kMatWords) + ",\"m2\":" + Arr(s.m2, kMatWords) + ",\"sp1\":" + Arr(s.sp1, 3) + ",\"sp2\":" + Arr(s.sp2, 3) +
           ",\"in\":" + Arr(s.in, 2) + ",\"atan\":" + Arr64(s.atan, 3) + ",\"weave\":" + U(s.weave) + ",\"maxsteer\":" + U(s.maxSteer) + ",\"outpre\":" + Arr(s.outPre, 4) + "}";
}
}  // namespace CS63CFix

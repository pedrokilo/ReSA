#pragma once
// Fixtures del banco CarCtrlSteer63d (lote 63, CCarCtrl parte B1, tanda 4): SteerAICarBlockingPlayerForwardAndBack (0x422B20), 4096 casos. Solo C++ estandar (sin tipos del juego).
//
//   CCarCtrl::SteerAICarBlockingPlayerForwardAndBack  0x422B20  cdecl (CVehicle* vehicle, float* steer, float* accel, float* brake, bool* handbrake), 5 argumentos, void
//
// Cada caso es una descripcion SEMANTICA (la leen el banco y fixture_oracle.py): vehiculo con matriz completa y velocidad, entidad del jugador (con o sin matriz), velocidad y coordenadas
// que devuelven los colaboradores sustituidos (FindPlayerSpeed/Entity/Coors, AllocateMatrix, UpdateMatrix).
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

namespace CS63DFix {
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
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 4242u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline u32 Bits(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
inline float Flt(u32 v) { float f; std::memcpy(&f, &v, 4); return f; }

// ------------------------------------------------------------------------------------------------ disposicion (bytes del EXE)
constexpr u32 kVehSize = 0x900, kEntSize = 0x40, kMatWords = 16, kGuardWords = 4, kOutWords = 8;
constexpr u32 kVMat = 0x14, kVSpeed = 0x44;
constexpr u32 kEPlacement = 0x04, kEMat = 0x14;
constexpr u32 kGarbageSeed = 0x1D2B7E15u;
inline u32 Garbage(u32 n, u32 k) {
    u32 v = 0xC0000000u | (Rand(n + kGarbageSeed, k) & 0x3FFFFFFFu);
    return v == 0xFFFFFFFFu ? 0xC0000000u : v;
}

// tipos de colaborador (indice en los eventos)
enum : u32 { kSpeed = 1, kEntity, kAlloc, kUpdate, kCoors, kKinds };

struct Spec {
    u32 family{}, regs[4]{};
    u32 m1[kMatWords]{};           // matriz del vehiculo (right, top, at, pos; 4 palabras cada una)
    u32 sp1[3]{};                  // velocidad del vehiculo (+0x44)
    u32 playerHasMat{1};
    u32 mp[kMatWords]{};           // matriz del jugador (cuando existe)
    u32 mpAlloc[kMatWords]{};      // matriz que deja AllocateMatrix (cuando no existia)
    u32 ps[3]{};                   // CVector que devuelve FindPlayerSpeed
    u32 pc[2][3]{};                // CVector que devuelve FindPlayerCoors en la 1.a y 2.a llamada
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

inline void GenMatrix(Gen& g, u32* m, double angle, float px, float py, float pz) {
    const float c = float(std::cos(angle)), s = float(std::sin(angle));
    const float v[16]{c, s, 0.0f, 0.0f, -s, c, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, px, py, pz, 0.0f};
    for (int i = 0; i < 16; ++i) m[i] = Bits(v[i]);
    if (g.C(20)) { for (int i : {0, 1, 4, 5}) if (g.C(40)) m[i] = Bits(Flt(m[i]) + Grid(g, 4, 0.05f)); }
    if (g.C(2)) m[g.P(2) + 4 * g.P(2)] = kQNaN;
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
    static const double axes[]{0.0, 1.5707963267948966, 3.141592653589793, 4.71238898038469};
    const double a1 = g.C(25) ? axes[g.P(4)] : double(g.P(62832)) * 0.0001;
    const double a2 = g.C(25) ? axes[g.P(4)] : double(g.P(62832)) * 0.0001;
    const float vx = Grid(g, 200, 0.5f), vy = Grid(g, 200, 0.5f), vz = Grid(g, 20, 0.5f);
    GenMatrix(g, s.m1, a1, vx, vy, vz);
    GenMatrix(g, s.mp, a2, Grid(g, 200, 0.5f), Grid(g, 200, 0.5f), Grid(g, 20, 0.5f));
    GenMatrix(g, s.mpAlloc, double(g.P(62832)) * 0.0001, 0.0f, 0.0f, 0.0f);
    s.playerHasMat = g.C(80) ? 1u : 0u;
    // el vehiculo puede tener la direccion nula (normalizacion de longitud 0)
    switch (g.P(14)) {
    case 0: s.m1[0] = 0; s.m1[1] = 0; break;
    case 1: s.m1[4] = 0; s.m1[5] = 0; break;
    case 2: s.m1[0] = kNegZero; s.m1[1] = 0; break;
    case 3: s.m1[4] = Bits(Grid(g, 20, 0.05f)); s.m1[5] = 0; break;
    default: break;
    }
    // velocidades
    static const float spd[]{0.0f, 0.05f, 0.1f, 0.2f, 0.3f, 0.5f, 0.8f, -0.05f, -0.2f, -0.5f};
    for (int i = 0; i < 3; ++i) {
        s.sp1[i] = g.C(3) ? kQNaN : (g.C(60) ? PickF(g, spd) : Bits(float(g.P(1601)) * 0.001f - 0.8f));
        s.ps[i] = g.C(3) ? kQNaN : (g.C(60) ? PickF(g, spd) : Bits(float(g.P(1601)) * 0.001f - 0.8f));
    }
    if (g.C(10)) { s.ps[0] = s.ps[1] = 0; s.mp[4] = s.mp[5] = s.mpAlloc[4] = s.mpAlloc[5] = 0; }   // jugador parado: C == 0 sustituye el divisor
    // coordenadas del jugador respecto al vehiculo: sobre sus ejes y a distintas distancias
    for (int k = 0; k < 2; ++k) {
        const float dx = Grid(g, 40, 0.5f), dy = Grid(g, 40, 0.5f), dz = Grid(g, 6, 0.5f);
        s.pc[k][0] = g.C(2) ? kQNaN : Bits(vx + dx);
        s.pc[k][1] = Bits(vy + dy);
        s.pc[k][2] = Bits(vz + dz);
        if (k == 1 && g.C(70)) for (int i = 0; i < 3; ++i) s.pc[1][i] = s.pc[0][i];
    }
    return s;
}

inline std::string U(u64 v) { return std::to_string(v); }
inline std::string Arr(const u32* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string JsonSpec(const Spec& s) {
    return "{\"fam\":" + U(s.family) + ",\"regs\":" + Arr(s.regs, 4) + ",\"m1\":" + Arr(s.m1, kMatWords) + ",\"sp1\":" + Arr(s.sp1, 3) + ",\"pmat\":" + U(s.playerHasMat) + ",\"mp\":" + Arr(s.mp, kMatWords) +
           ",\"mpalloc\":" + Arr(s.mpAlloc, kMatWords) + ",\"ps\":" + Arr(s.ps, 3) + ",\"pc0\":" + Arr(s.pc[0], 3) + ",\"pc1\":" + Arr(s.pc[1], 3) + ",\"outpre\":" + Arr(s.outPre, 4) + "}";
}
}  // namespace CS63DFix

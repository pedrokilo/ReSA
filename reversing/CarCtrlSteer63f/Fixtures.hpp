#pragma once
// Fixtures del banco CarCtrlSteer63f (lote 63, CCarCtrl parte B1, tanda 6): DealWithBend_Racing (0x428040), 4096 casos. Solo C++ estandar (sin tipos del juego).
//
//   CCarCtrl::DealWithBend_Racing  0x428040  cdecl (CVehicle* vehicle, CCarPathLinkAddress l1, l2, l3, l4, char a6, a7, a8, a9, float a10, float* o11, float* o12, float* o13, float* o14, CVector* pos),
//   15 argumentos de una palabra, devuelve bool en AL.
//
// Cada caso es una descripcion SEMANTICA (la leen el banco y fixture_oracle.py): dos arrays de 8 tramos de camino (CCarPathLink de 0xE bytes) en la arena, cuyas direcciones de area se cargan en
// CPathFind (m_pPathNodes 0x96F854, m_pNaviNodes 0x96F974), vehiculo con matriz, argumentos y resultados de GetATanOfXY (0x53CC70) por orden de llamada. 0x44DB00 (desplazamiento de carril) y 0x4226F0
// (interseccion de dos rectas) corren de verdad en el EXE y en la DLL.
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

namespace CS63FFix {
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
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 8675309u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline u32 Bits(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
inline float Flt(u32 v) { float f; std::memcpy(&f, &v, 4); return f; }
inline u64 DBits(double d) { u64 v; std::memcpy(&v, &d, 8); return v; }
inline double DFlt(u64 v) { double d; std::memcpy(&d, &v, 8); return d; }

// ------------------------------------------------------------------------------------------------ disposicion (bytes del EXE)
constexpr u32 kVehSize = 0x900, kMatWords = 16, kGuardWords = 4, kOutWords = 8, kLinkSize = 0xE, kLinksPerArea = 8, kLinkBytes = kLinksPerArea * kLinkSize, kLinkWords = (kLinkBytes + 3) / 4;
constexpr u32 kVMat = 0x14, kVPos = 0x04, kVSub = 0x594;
constexpr u32 kAreaA = 7, kAreaB = 20;                      // areas de 6 bits de las direcciones de tramo
constexpr u32 kPathNodesAddr = 0x96F854, kNaviNodesAddr = 0x96F974, kTableWords = 48;
constexpr u32 kGarbageSeed = 0x5851F42Du;
inline u32 Garbage(u32 n, u32 k) {
    u32 v = 0xC0000000u | (Rand(n + kGarbageSeed, k) & 0x3FFFFFFFu);
    return v == 0xFFFFFFFFu ? 0xC0000000u : v;
}

// tipos de colaborador (indice en los eventos)
enum : u32 { kAtan = 1, kKinds };

struct Spec {
    u32 family{}, regs[4]{};
    u32 sub594{};
    u32 mpos[3]{}, mtop[3]{};          // matriz del vehiculo (siempre presente)
    u32 addr[4]{};                     // palabra de pila de cada direccion de tramo (16 bits bajos = direccion, altos = basura)
    u32 loaded[2]{1, 1};               // area A / B con nodos de camino cargados (puntero no nulo)
    u8 links[2][kLinkWords * 4]{};     // 2 arrays de 8 tramos de 0xE bytes
    u32 args[4]{};                     // a6..a9 (byte bajo valido, resto basura)
    u32 arg10{};
    u64 atan[3]{};                     // resultados (double) de 0x53CC70 por orden de llamada
    u32 outPre[8]{};
};

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_) : n(n_) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
};
constexpr u32 kQNaN = 0x7FC00000u;

inline float Grid(Gen& g, int span, float step) { return float(int(g.P(u32(2 * span + 1))) - span) * step; }
template <std::size_t N>
inline u32 PickF(Gen& g, const float (&v)[N]) { return Bits(v[g.P(u32(N))]); }
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
inline void PutI16(u8* p, i16 v) { std::memcpy(p, &v, 2); }

inline Spec MakeSpec(u32 n) {
    Spec s;
    Gen g(n);
    s.family = n % kFamilies;
    s.regs[0] = StackSeed(n);
    s.regs[1] = g.C(30) ? Garbage(n, 1) : g.R();
    s.regs[2] = g.C(30) ? Garbage(n, 2) : g.R();
    s.regs[3] = g.C(30) ? Garbage(n, 3) : g.R();
    for (int i = 0; i < 8; ++i) s.outPre[i] = Garbage(n, 70 + i);
    s.sub594 = g.C(35) ? 10u : g.P(12);
    // vehiculo y trayecto
    const float vx = Grid(g, 200, 0.5f), vy = Grid(g, 200, 0.5f), vz = Grid(g, 20, 0.5f);
    s.mpos[0] = Bits(vx); s.mpos[1] = Bits(vy); s.mpos[2] = Bits(vz);
    const double theta = double(g.P(62832)) * 0.0001;
    static const double rel[]{0.0, 0.1, 0.3, 0.5, 0.7, 1.0, 1.1, 1.6, 2.5, 3.0, 3.1416, 4.0, 5.5};
    const double headV = theta + rel[g.P(sizeof(rel) / sizeof(rel[0]))];
    s.mtop[0] = Bits(float(-std::sin(headV))); s.mtop[1] = Bits(float(std::cos(headV))); s.mtop[2] = Bits(Grid(g, 10, 0.05f));
    if (g.C(3)) s.mtop[0] = kQNaN;
    static const float dist0[]{0.0f, 1.0f, 2.0f, 5.0f, 8.0f, 10.0f, 12.0f, 15.0f, 20.0f, 30.0f, 40.0f, 59.0f, 60.0f, 61.0f, 70.0f};
    const float startDist = g.C(85) ? dist0[g.P(sizeof(dist0) / sizeof(dist0[0]))] : float(g.P(601)) * 0.1f;
    const float sx0 = vx + startDist * float(std::cos(theta + 1.2)), sy0 = vy + startDist * float(std::sin(theta + 1.2));
    static const float spacing[]{2.0f, 5.0f, 8.0f, 9.9f, 10.0f, 10.1f, 15.0f, 25.0f};
    for (int a = 0; a < 2; ++a) {
        const float step = spacing[g.P(sizeof(spacing) / sizeof(spacing[0]))];
        const double phi0 = theta + (g.C(15) ? 0.05 : 0.0);
        for (u32 j = 0; j < kLinksPerArea; ++j) {
            u8* l = s.links[a] + j * kLinkSize;
            for (u32 b = 0; b < kLinkSize; ++b) l[b] = u8(g.R());                   // basura en los bytes no usados
            const double phi = phi0 + (g.C(10) ? double(g.P(300)) * 0.01 - 1.5 : 0.0) + (a ? 0.15 : 0.0);
            const float px = sx0 + float(a) * 6.0f + step * float(j) * float(std::cos(theta)) + Grid(g, 4, 0.25f);
            const float py = sy0 + step * float(j) * float(std::sin(theta)) + Grid(g, 4, 0.25f);
            PutI16(l + 0, i16(std::lround(px * 8.0f)));
            PutI16(l + 2, i16(std::lround(py * 8.0f)));
            l[8] = u8(i32(std::lround(100.0 * std::cos(phi))));
            l[9] = u8(i32(std::lround(100.0 * std::sin(phi))));
            if (g.C(8)) { l[8] = 0; l[9] = 0; }
            l[0xA] = u8(g.R());
            const u32 opp = g.C(30) ? 0u : g.P(6), same = g.C(30) ? 0u : g.P(6);
            l[0xB] = u8(opp | (same << 3) | (g.R() & 0xC0u));
        }
    }
    // direcciones de los tramos: del array A o B; a veces invalidas (0xFFFF)
    for (int i = 0; i < 4; ++i) {
        u32 raw;
        if (g.C(i == 0 ? 3 : (i == 1 ? 4 : 25))) raw = 0xFFFFu;
        else raw = ((g.C(70) ? kAreaA : kAreaB) << 10) | g.P(kLinksPerArea);
        s.addr[i] = (g.R() & 0xFFFF0000u) | raw;
    }
    if (g.C(55)) {   // cadena ordenada de tramos consecutivos del mismo array
        const u32 area = g.C(60) ? kAreaA : kAreaB;
        const u32 first = g.P(4);
        for (int i = 0; i < 4; ++i) if ((s.addr[i] & 0xFFFFu) != 0xFFFFu) s.addr[i] = (s.addr[i] & 0xFFFF0000u) | (area << 10) | (first + u32(i));
    }
    s.loaded[0] = g.C(95) ? 1u : 0u;
    s.loaded[1] = g.C(95) ? 1u : 0u;
    // argumentos
    static const int mult[]{1, -1, 1, -1, 2, 100, 0, -100, 5};
    const int baseMult = mult[g.P(sizeof(mult) / sizeof(mult[0]))];
    for (int i = 0; i < 4; ++i) {
        const int m = g.C(55) ? baseMult : (g.C(85) ? mult[g.P(sizeof(mult) / sizeof(mult[0]))] : int(g.R() & 0xFF) - 128);
        s.args[i] = (g.R() & 0xFFFFFF00u) | (u32(m) & 0xFFu);
    }
    {
        static const float a10[]{0.0f, 10.0f, 20.0f, 30.0f, 45.0f, 60.0f, 100.0f, 5.0f, -3.0f, 1.0f, 6.0f};
        s.arg10 = g.C(5) ? kQNaN : (g.C(75) ? PickF(g, a10) : Bits(float(g.P(1201)) * 0.1f - 20.0f));
    }
    for (int i = 0; i < 3; ++i) s.atan[i] = PickAtan(g);
    return s;
}

inline std::string U(u64 v) { return std::to_string(v); }
inline std::string Arr(const u32* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string Arr64(const u64* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string JsonSpec(const Spec& s) {
    std::string links = "[";
    for (int a = 0; a < 2; ++a) {
        if (a) links += ',';
        links += '[';
        for (u32 b = 0; b < kLinkBytes; ++b) { if (b) links += ','; links += U(s.links[a][b]); }
        links += ']';
    }
    links += ']';
    return "{\"fam\":" + U(s.family) + ",\"regs\":" + Arr(s.regs, 4) + ",\"sub\":" + U(s.sub594) + ",\"mpos\":" + Arr(s.mpos, 3) + ",\"mtop\":" + Arr(s.mtop, 3) + ",\"addr\":" + Arr(s.addr, 4) +
           ",\"loaded\":" + Arr(s.loaded, 2) + ",\"links\":" + links + ",\"args\":" + Arr(s.args, 4) + ",\"arg10\":" + U(s.arg10) + ",\"atan\":" + Arr64(s.atan, 3) + ",\"outpre\":" + Arr(s.outPre, 8) + "}";
}
}  // namespace CS63FFix

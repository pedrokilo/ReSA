#pragma once
// Fixtures del banco Garage63 (lote 63): nueve funciones de CGarage, 4096 casos cada una. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y
// un programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
//   0 CGarage::InitDoorsAtStart                  0x447600  thiscall, void
//   1 CGarage::PlayerArrestedOrDied              0x4486C0  thiscall, void (ECX = this y EDX de entrada deben quedar intactos: lo exige el bucle de 0x449E60)
//   2 CGarage::IsEntityEntirelyInside3D          0x448BE0  thiscall (entity, float radius) ret 8, AL
//   3 CGarage::IsEntityEntirelyOutside           0x448D30  thiscall (entity, float radius) ret 8, AL
//   4 CGarage::IsStaticPlayerCarEntirelyInside   0x44A830  thiscall, AL
//   5 CGarage::IsGarageEmpty                     0x44A9C0  thiscall, AL
//   6 CGarage::RemoveCarsBlockingDoorNotInside   0x449690  thiscall, void
//   7 CGarage::TidyUpGarage                      0x449C50  thiscall, void
//   8 CGarage::TidyUpGarageClose                 0x449D10  thiscall, void
//
// Cada caso es una descripcion SEMANTICA (la leen el banco y fixture_oracle.py). Todos los colaboradores (entidad, matriz, esferas, transformacion, pool, mundo, jugador, destructor
// virtual...) son falsos identicos en ambos lados que registran sus argumentos EN ORDEN y devuelven lo que dicta el caso.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

namespace G63Fix {
using i16 = std::int16_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFns = 9;
constexpr u32 kInit = 0, kArrested = 1, kInside3D = 2, kOutside = 3, kStatic = 4, kEmpty = 5, kRemove = 6, kTidy = 7, kTidyClose = 8;
constexpr unsigned kFamilies = 12;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline u32 Bits(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
inline float Flt(u32 v) { float f; std::memcpy(&f, &v, 4); return f; }

// ------------------------------------------------------------------------------------------------ disposicion
constexpr u32 kGarageSize = 0xD8;
constexpr u32 kVehSize = 0xA18, kMaxVeh = 8;
constexpr u32 kMatWords = 24, kCmWords = 12, kCdWords = 4, kMaxSph = 8, kSphWords = 5;
constexpr u32 kGuardWords = 4;
constexpr u32 kPedWords = 0x500 / 4, kIntelWords = 8;
constexpr u32 kPoolAddr = 0xB74494;                 // ds:[0xB74494] = CVehiclePool*
// campos de la entidad (bytes)
constexpr u32 kOffPos = 4, kOffMatrix = 0x14, kOffB36 = 0x36, kOffSpeed = 0x44, kOffFlags428 = 0x428, kOffVType = 0x590;
// campos del CGarage (bytes)
constexpr u32 kGPosZ = 0x08, kGTopZ = 0x1C, kGLeft = 0x28, kGRight = 0x2C, kGFront = 0x30, kGBack = 0x34, kGDoorPos = 0x38, kGTime = 0x3C, kGType = 0x4C, kGState = 0x4D, kGFlags = 0x4E;
constexpr u32 kGarbageSeed = 0x6A09E667u;
inline u32 Garbage(u32 n, u32 k) {
    u32 v = 0xC0000000u | (Rand(n + kGarbageSeed, k) & 0x3FFFFFFFu);
    return v == 0xFFFFFFFFu ? 0xC0000000u : v;
}

// ------------------------------------------------------------------------------------------------ especificacion
struct Veh {
    u32 freeSlot{1};          // ranura libre (bit 7 del estado)
    u32 type{0};              // m_nVehicleType (+0x590)
    u32 b36{0};               // byte +0x36: tipo de entidad (bits 0..2) y estado (bits 3..7)
    u32 hasMat{1};            // m_matrix != 0
    u32 upz{0x3F800000u};     // z del eje "up" (+0x28 de la matriz)
    u32 pos[3]{};             // posicion de m_placement (+4)
    u32 mpos[3]{};            // posicion de la matriz (+0x30)
    u32 speed[3]{};           // m_vecMoveSpeed (+0x44)
    u32 locked{0};            // byte +0x428, bit 3
    u32 hasCol{1};            // el CColModel tiene m_pColData
    u32 nsph{0};              // m_nNumSpheres (int16 en los 16 bits bajos)
    u32 sphR[kMaxSph]{};      // radios de las esferas
};
struct Spec {
    u32 fn{}, family{};
    u32 regs[4]{};            // semilla de pila, EAX, ECX, EDX de entrada
    u32 gtype{}, gstate{}, gflags{}, gdoor{}, gtime{};
    u32 gb[6]{};              // izquierda, derecha, frente, fondo, z, topZ (bits de float)
    u32 radius{};
    u32 nveh{4};
    Veh veh[kMaxVeh]{};
    u32 touchMask{}, pointMask{}, radMask{}, delMask{}, inMask{}, seed{};
    u32 reK{0xFFFFFFFFu}, reKind{}, reIdx{}, reVal{};
    u32 pv{1}, pvNullAt{0xFFu}, task{};
    u32 cnt{}, listIdx[16]{};
};

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_) : n(n_) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
};

constexpr u32 kQNaN = 0x7FC00000u, kInf = 0x7F800000u, kNegInf = 0xFF800000u;
inline float NextUp(float f) { return std::nextafterf(f, std::numeric_limits<float>::infinity()); }
inline float NextDown(float f) { return std::nextafterf(f, -std::numeric_limits<float>::infinity()); }
// valor "raro" (para campos que solo se comparan): NaN, infinitos, -0.0, subnormal, enorme
inline u32 Odd(Gen& g) {
    switch (g.P(8)) {
    case 0: return kQNaN;
    case 1: return kInf;
    case 2: return kNegInf;
    case 3: return 0x80000000u;
    case 4: return 1u + g.P(100);                      // subnormal
    case 5: return 0x7F000000u + g.P(0x7FFFFFu);       // enorme (~1e38)
    case 6: return 0xFF000000u + g.P(0x7FFFFFu);       // enorme negativo
    default: return 0u;
    }
}
inline u32 Small(Gen& g, int lo, int hi) { return Bits(float(int(lo + int(g.P(u32(hi - lo + 1)))))); }
inline u32 Quarter(Gen& g, int lo, int hi) { return Bits(float(int(lo * 4 + int(g.P(u32((hi - lo) * 4 + 1))))) * 0.25f); }

inline void FillSpheres(Gen& g, Veh& v, u32 n) {
    for (u32 i = 0; i < kMaxSph; ++i) v.sphR[i] = g.C(10) ? Odd(g) : (g.C(30) ? Bits(0.0f) : Quarter(g, 0, 4));
    v.nsph = n;
}

// geometria: garaje [left,right] x [front,back] x [posz,topz], radio y la posicion de la entidad veh[0] respecto a ellos
inline void GenGeom(Gen& g, Spec& s, u32 fam) {
    const float left = float(int(g.P(40)) - 20), width = float(1 + g.P(30)), front = float(int(g.P(40)) - 20), depth = float(1 + g.P(30)), z0 = float(int(g.P(20)) - 10), h = float(1 + g.P(10));
    s.gb[0] = Bits(left); s.gb[1] = Bits(left + width); s.gb[2] = Bits(front); s.gb[3] = Bits(front + depth); s.gb[4] = Bits(z0); s.gb[5] = Bits(z0 + h);
    const float radii[]{0.0f, 0.25f, 0.5f, 1.0f, 2.0f, 4.0f, 8.0f, 100.0f};
    const float r = radii[g.P(8)];
    s.radius = Bits(r);
    const float lim[6]{left - r, left + width + r, front - r, front + depth + r, z0 - r, z0 + h + r};   // limites ampliados (exactos: enteros y cuartos)
    float p[3]{left + width * 0.5f, front + depth * 0.5f, z0 + h * 0.5f};
    Veh& v = s.veh[0];
    switch (fam) {
    case 0: break;                                                                      // dentro
    case 1: { const u32 w = g.P(6); p[w / 2] = lim[w]; } break;                         // justo en un limite ampliado
    case 2: { const u32 w = g.P(6); p[w / 2] = g.C(50) ? NextUp(lim[w]) : NextDown(lim[w]); } break;
    case 3: { const u32 w = g.P(3); p[w] = g.C(50) ? lim[w * 2] - 1.0f - float(g.P(50)) : lim[w * 2 + 1] + 1.0f + float(g.P(50)); } break;
    case 4: { const u32 w = g.P(3); p[w] = Flt(kQNaN); if (g.C(50)) s.gb[g.P(6)] = kQNaN; if (g.C(30)) s.radius = kQNaN; } break;
    case 5: { if (g.C(50)) s.radius = g.C(50) ? kInf : kNegInf; if (g.C(50)) s.gb[g.P(6)] = g.C(50) ? kInf : kNegInf; if (g.C(50)) p[g.P(3)] = Flt(g.C(50) ? kInf : kNegInf); } break;
    case 6: { s.radius = g.C(50) ? 0x7F000000u : 0x00000005u; s.gb[g.P(6)] = g.C(50) ? 0x7F000000u : 0x00000003u; p[g.P(3)] = Flt(Odd(g)); } break;
    case 7: break;                                                                      // cuenta de esferas rara (mas abajo)
    case 8: break;                                                                      // matriz / sin matriz con posiciones distintas
    case 9: break;                                                                      // reentrada
    case 10: { p[g.P(3)] = Flt(Odd(g)); } break;
    default: { const u32 w = g.P(6); p[w / 2] = lim[w] + (g.C(50) ? 0.0f : float(int(g.P(5)) - 2) * 0.25f); } break;
    }
    // sitio de la posicion: la matriz (+0x30) o m_placement (+4); el otro lleva un senuelo
    v.hasMat = g.C(55) ? 1u : 0u;
    for (int i = 0; i < 3; ++i) {
        const u32 real = Bits(p[i]), decoy = g.C(50) ? Bits(lim[i * 2] - 3.0f) : Garbage(g.n, 50 + i);
        v.pos[i] = v.hasMat ? decoy : real;
        v.mpos[i] = v.hasMat ? real : decoy;
    }
    if (fam == 8) v.hasMat = g.C(50) ? 1u : 0u, std::swap(v.pos[0], v.mpos[0]);   // siempre coherente con hasMat? no: el banco usa la ficha real del caso (pos segun hasMat)
    // esferas y colision
    v.hasCol = fam == 7 && g.C(30) ? 0u : 1u;
    u32 n = g.P(5);
    if (fam == 7) { const u32 w = g.P(6); n = w == 0 ? 0u : w == 1 ? 0xFFFFu : w == 2 ? 0x8000u : w == 3 ? 0x8001u : w == 4 ? 1u : 8u; }
    FillSpheres(g, v, n);
    s.pointMask = g.R(); s.radMask = g.R();
    if (fam != 7 && g.C(40)) { s.pointMask = ~0u; s.radMask = ~0u; }
    if (fam != 7 && g.C(15)) { s.pointMask = 0; s.radMask = 0; }
    s.seed = g.R();
    if (fam == 9 && v.nsph >= 1 && v.nsph <= 8) { s.reK = g.P(v.nsph); s.reKind = 1 + g.P(2); s.reIdx = 0; s.reVal = g.C(30) ? 0u : (g.C(30) ? 0xFFFFu : g.P(9)); }
}

inline void GenVeh(Gen& g, Veh& v, u32 fam) {
    v.freeSlot = g.C(fam == 0 ? 5 : 22) ? 1u : 0u;
    v.type = g.C(35) ? 0u : g.C(40) ? 9u : g.P(14);
    const u32 st = g.C(40) ? 5u : g.P(8);
    v.b36 = (st << 3) | g.P(8);
    v.hasMat = g.C(80) ? 1u : 0u;
    const u32 pickUp = g.P(6);
    v.upz = pickUp == 0 ? Bits(0.5f) : pickUp == 1 ? Bits(0.4999999f) : pickUp == 2 ? kQNaN : pickUp == 3 ? Bits(-1.0f) : pickUp == 4 ? Bits(1.0f) : Bits(float(g.P(100)) / 100.0f);
    for (int i = 0; i < 3; ++i) { v.pos[i] = Bits(float(int(g.P(60)) - 30)); v.mpos[i] = Bits(float(int(g.P(60)) - 30)); }
    v.locked = g.C(25) ? 1u : 0u;
    v.hasCol = 1;
    FillSpheres(g, v, g.C(15) ? 0u : (g.C(5) ? 0xFFFFu : 1u + g.P(4)));
}

constexpr u32 kPoolSizes[]{1, 2, 3, 4, 5, 6, 7, 8};

inline Spec MakeSpec(u32 fn, u32 n) {
    Spec s;
    s.fn = fn;
    Gen g(n + fn * 0x20000u);
    s.family = n % kFamilies;
    const u32 fam = s.family;
    s.regs[0] = StackSeed(n);
    s.regs[1] = g.C(30) ? Garbage(n, 1) : g.R();
    s.regs[2] = g.C(30) ? Garbage(n, 2) : g.R();
    s.regs[3] = g.C(30) ? Garbage(n, 3) : g.R();
    s.seed = g.R();
    // el garaje: tipo / estado / banderas / puerta
    {
        static constexpr u32 types[]{0, 1, 2, 3, 4, 5, 6, 11, 12, 13, 14, 15, 16, 20, 45, 46, 47, 100, 255};
        s.gtype = g.C(70) ? types[g.P(sizeof(types) / sizeof(types[0]))] : (g.C(70) ? g.P(48) : g.P(256));
        s.gstate = g.C(80) ? g.P(7) : g.P(256);
        s.gflags = g.C(60) ? 0xFFu : g.P(256);
        s.gdoor = g.C(40) ? Bits(float(g.P(3))) : (g.C(30) ? kQNaN : g.R());
        s.gtime = g.R();
    }
    s.pointMask = g.R(); s.radMask = g.R(); s.touchMask = g.R(); s.delMask = g.R(); s.inMask = g.R();
    switch (fn) {
    case kInit:
    case kArrested:
        break;
    case kInside3D:
    case kOutside:
        GenGeom(g, s, fam);
        break;
    case kStatic: {
        GenGeom(g, s, fam);
        Veh& v = s.veh[0];
        v.type = g.C(40) ? 0u : g.C(50) ? 9u : g.P(14);
        s.pv = g.C(88) ? 1u : 0u;
        s.pvNullAt = g.C(10) ? 0u : 0xFFu;
        s.task = g.C(20) ? 0x1234u : 0u;
        // velocidad: componentes pequenas, en el limite 0.01f, negativas, -0.0, NaN, grandes
        static const float vs[]{0.0f, 0.001f, 0.005f, 0.009f, 0.01f, 0.0100001f, -0.01f, -0.0100001f, 0.02f, -0.0f, 0.00577f, 0.0058f, 0.0057f, 1.0e-30f, 1.0e-20f, 0.0f};
        for (int i = 0; i < 3; ++i) v.speed[i] = g.C(8) ? Odd(g) : Bits(vs[g.P(16)]);
        if (g.C(40)) { for (int i = 0; i < 3; ++i) v.speed[i] = Bits(0.0f); }
        break;
    }
    case kEmpty: {
        GenGeom(g, s, fam);
        s.cnt = g.C(15) ? (g.C(50) ? 0u : (g.C(50) ? 0xFFFFu : 0x8000u)) : 1u + g.P(16);
        for (u32 i = 0; i < 16; ++i) s.listIdx[i] = g.P(kMaxVeh);
        break;
    }
    default: {
        GenGeom(g, s, fam);
        s.nveh = kPoolSizes[g.C(70) ? 2 + g.P(6) : g.P(8)];
        for (u32 i = 0; i < kMaxVeh; ++i) GenVeh(g, s.veh[i], fam);
        if (fn == kTidy) for (u32 i = 0; i < kMaxVeh; ++i) if ((s.veh[i].b36 & 0xF8u) != 0x28u) s.veh[i].hasMat = 1u;   // el EXE desreferencia la matriz sin comprobarla
        if (fn == kTidyClose || fn == kTidy) {
            // reentrada: cambia el contador de esferas de un vehiculo en la k-esima transformacion
            if (fam == 9) { s.reK = g.P(4); s.reKind = 1 + g.P(2); s.reIdx = g.P(s.nveh); s.reVal = g.C(30) ? 0u : (g.C(30) ? 0xFFFFu : g.P(9)); }
        }
        break;
    }
    }
    return s;
}

inline std::string U(u32 v) { return std::to_string(v); }
inline std::string Arr(const u32* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string JsonVeh(const Veh& v) {
    return "{\"free\":" + U(v.freeSlot) + ",\"type\":" + U(v.type) + ",\"b36\":" + U(v.b36) + ",\"mat\":" + U(v.hasMat) + ",\"upz\":" + U(v.upz) + ",\"pos\":" + Arr(v.pos, 3) + ",\"mpos\":" + Arr(v.mpos, 3) +
           ",\"speed\":" + Arr(v.speed, 3) + ",\"locked\":" + U(v.locked) + ",\"col\":" + U(v.hasCol) + ",\"nsph\":" + U(v.nsph) + ",\"sphr\":" + Arr(v.sphR, kMaxSph) + "}";
}
inline std::string JsonSpec(const Spec& s) {
    std::string o = "{\"fn\":" + U(s.fn) + ",\"fam\":" + U(s.family) + ",\"regs\":" + Arr(s.regs, 4) + ",\"g\":" + Arr(&s.gtype, 5) + ",\"gb\":" + Arr(s.gb, 6) + ",\"radius\":" + U(s.radius) +
                    ",\"nveh\":" + U(s.nveh) + ",\"masks\":[" + U(s.touchMask) + "," + U(s.pointMask) + "," + U(s.radMask) + "," + U(s.delMask) + "," + U(s.inMask) + "],\"seed\":" + U(s.seed) +
                    ",\"re\":[" + U(s.reK) + "," + U(s.reKind) + "," + U(s.reIdx) + "," + U(s.reVal) + "],\"pv\":[" + U(s.pv) + "," + U(s.pvNullAt) + "," + U(s.task) + "],\"cnt\":" + U(s.cnt) + ",\"list\":" +
                    Arr(s.listIdx, 16) + ",\"veh\":[";
    const u32 nv = s.fn == kInit || s.fn == kArrested ? 0u : (s.fn >= kRemove ? kMaxVeh : (s.fn == kEmpty ? kMaxVeh : 1u));
    for (u32 i = 0; i < nv; ++i) { if (i) o += ','; o += JsonVeh(s.veh[i]); }
    return o + "]}";
}
}  // namespace G63Fix

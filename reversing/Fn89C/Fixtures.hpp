#pragma once
// Fixtures del banco Fn89C (lote 89C): una funcion, 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskSimpleGangDriveBy::FireGun                    0x627CC0  thiscall (ECX = tarea; ped en pila), `ret 4`; void   (hook CTaskSimpleGangDriveBy / FireGun, tipo S)
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

namespace F89CFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;
constexpr unsigned kModelBase = 19980;  // indices de modelo usados: kModelBase .. kModelBase + kModels - 1 (entradas libres de ms_modelInfoPtrs)
constexpr unsigned kModels = 12;
constexpr unsigned kSlots = 13;         // ranuras de arma del ped

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kTaskWin = 0x48, kPedWin = 0x7C0, kPdWin = 0x40, kWiWin = 0x40, kEntWin = 0x40, kMatWin = 0x40, kVehWin = 0x60, kMiWin = 0x20, kColWin = 0x20;
constexpr std::size_t kClumpWin = 0x20, kBoneWin = 0x20, kHierWin = 0x20, kMaWin = 0x40 * 10, kAssocWin = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kMaObjOff = 0x80;   // la matriz 0 del array esta 2 matrices por encima del inicio del objeto (indices -2 .. 7)

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 89003u); }
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

inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Un float "especial": cero, desnormal, infinito, NaN (un unico patron), enorme
inline u32 Special(u32 c, u32 k) {
    const u32 r = Rand(c, k) % 32;
    if (r < 10) return Pick(kTiny, c, k + 1);
    if (r < 16) return Pick(kInf, c, k + 1);
    if (r < 24) return 0xFFC00000u;
    if (r < 28) return Pick(kBig, c, k + 1);
    return B(float(int(Rand(c, k + 3) % 2001) - 1000) / 7.0f);
}
inline u32 NextUp(u32 bits, int steps) {   // desplaza un float normal `steps` unidades de ultimo lugar (signo respetado)
    const bool neg = (bits >> 31) != 0;
    u32 mag = bits & 0x7FFFFFFFu;
    mag = u32(i32(mag) + (neg ? -steps : steps));
    return (bits & 0x80000000u) | (mag & 0x7FFFFFFFu);
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 ptype{};                                    // CPed::m_nPedType (+0x598): 0 y 1 son jugador
    u32 cam{}, cmode[3]{}, csrc[9]{};               // camara activa (byte 0xB6F081), modo (word) y origen (+0x190) de las tres camaras
    u32 pmat{}, ppos[3]{}, palt[3]{};               // el ped tiene matriz (+0x14); posicion en la ubicacion activa y en la otra (matriz +0x30 o compacta +4)
    u32 slot{}, ikinit{}, atkinit{};                // ranura activa (byte +0x718), +0x528 (banderas del IK) y datos del jugador +0x2C al empezar
    u32 fireoff[3]{}, xform[3]{}, bonetag{}, boneidx{};   // desplazamiento de disparo; salida de RwV3dTransformPoints; BoneTag; indice que devuelve RpHAnimIDGetIndex
    u32 vmodel{}, vmodel2{}, vmat[16]{}, boxA[6]{}, boxB[6]{};   // vehiculo: indice de modelo (0..kModels-1), modelo de la mutacion, matriz, cajas (pares/impares)
    u32 fsrc[3]{}, ftgt[3]{};                       // salidas de Find3rdPersonCamTargetVector
    u32 ent{}, emat[16]{}, coords[3]{}, assoc{}, animid{}, fdir{};   // tarea: entidad objetivo (con matriz), su matriz, coordenadas, asociacion de animacion, su AnimId, m_nFakeShootDirn
    u32 mutSite{}, mutVal{};
    u32 gate{};
};

// ------------------------------------------------------------------------------------------------ generador
inline void UnitXY(u32 c, u32 k, double z, u32* out) {   // vector unitario con la z indicada (|z| <= 1)
    const double ang = double(Rand(c, k) % 62832) / 10000.0;
    const double r = std::sqrt(std::max(0.0, 1.0 - z * z));
    out[0] = B(float(r * std::cos(ang))); out[1] = B(float(r * std::sin(ang))); out[2] = B(float(z));
}
inline constexpr std::array<u32, 12> kAnimIds{0xE7u, 0xECu, 0xEBu, 0xE8u, 0xEAu, 0xEEu, 0xE9u, 0xEDu, 0x00E6u, 0x00EFu, 0u, 0xFFFFu};

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    u32 kk = 100;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    auto Pk = [&](std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); };
    auto Fl = [&](float lo, float hi) { return lo + (hi - lo) * (float(R(100001)) / 100000.0f); };
    auto Unit = [&](u32* o) { UnitXY(n, nk(), double(int(R(2001)) - 1000) / 1000.0, o); };
    // --- jugador o IA y camara
    bool player;
    switch (f) {
    case 1: case 2: case 3: player = true; break;
    case 4: case 5: case 6: player = false; break;
    case 0: player = P(40); break;
    default: player = P(35); break;
    }
    c.ptype = player ? R(2) : (2 + R(30));
    c.cam = R(3);
    for (u32 k = 0; k < 3; ++k) {
        c.cmode[k] = Pk({0x37u, 0x31u, 0x37u, 0x31u, R(0x40), 0x2u, 0x30u, 0x32u, 0x36u, 0x38u, 0u, 0x1FFu});
        for (u32 j = 0; j < 3; ++j) c.csrc[k * 3 + j] = B(Fl(-3000.0f, 3000.0f));
    }
    {
        u32& m = c.cmode[c.cam];
        if (f == 1 || f == 7 || f == 8) m = 0x37;
        else if (f == 2) m = 0x31;
        else if (f == 3) m = Pk({0u, 1u, 0x30u, 0x32u, 0x36u, 0x38u, 0x2Fu, 0x3Fu, R(0x30), 0x137u});
        else if (player && (f == 0 || f == 15 || f >= 9)) m = Pk({0x37u, 0x37u, 0x31u, 0x31u, R(0x40)});
    }
    // --- ped
    c.pmat = P(60);
    for (u32 k = 0; k < 3; ++k) { c.ppos[k] = B(Fl(-3000.0f, 3000.0f)); c.palt[k] = B(Fl(-3000.0f, 3000.0f)); }
    c.slot = R(kSlots);
    c.ikinit = Rand(n, nk()); c.atkinit = Rand(n, nk());
    c.bonetag = Rand(n, nk()); c.boneidx = Pk({0u, 1u, 2u, 3u, 5u, 7u, u32(-1), u32(-2), 6u});
    // --- vehiculo
    c.vmodel = R(kModels); c.vmodel2 = (c.vmodel + 1 + 2 * R(kModels / 2)) % kModels;
    {
        u32 right[3], fwd[3], up[3];
        Unit(right); Unit(fwd); Unit(up);
        if (f == 14) { right[0] = B(1.0f); right[1] = 0; right[2] = 0; fwd[0] = B(1.0f); fwd[1] = 0; fwd[2] = 0; }
        for (u32 k = 0; k < 3; ++k) { c.vmat[k] = right[k]; c.vmat[4 + k] = fwd[k]; c.vmat[8 + k] = up[k]; c.vmat[12 + k] = B(Fl(-3000.0f, 3000.0f)); }
        c.vmat[3] = Rand(n, nk()); c.vmat[7] = Rand(n, nk()); c.vmat[11] = Rand(n, nk()); c.vmat[15] = Rand(n, nk());
    }
    for (u32 k = 0; k < 3; ++k) {
        c.boxA[k] = B(-(0.5f + Fl(0.0f, 3.0f))); c.boxA[3 + k] = B(0.5f + Fl(0.0f, 3.0f));
        c.boxB[k] = B(-(0.5f + Fl(0.0f, 3.0f))); c.boxB[3 + k] = B(0.5f + Fl(0.0f, 3.0f));
    }
    // --- geometria del disparo: origen (salida de la transformacion) cerca del vehiculo, objetivo a distancia aleatoria
    const double vx = double(F(c.vmat[12])), vy = double(F(c.vmat[13])), vz = double(F(c.vmat[14]));
    for (u32 k = 0; k < 3; ++k) c.fireoff[k] = B(Fl(-1.0f, 1.0f));
    {
        const double a = double(R(6001)) / 1000.0 - 3.0, b = double(R(6001)) / 1000.0 - 3.0, h = double(R(3001)) / 1000.0 - 1.0;
        const double rx = double(F(c.vmat[0])), ry = double(F(c.vmat[1])), rz = double(F(c.vmat[2])), fx = double(F(c.vmat[4])), fy = double(F(c.vmat[5])), fz = double(F(c.vmat[6]));
        c.xform[0] = B(float(vx + a * rx + b * fx)); c.xform[1] = B(float(vy + a * ry + b * fy)); c.xform[2] = B(float(vz + a * rz + b * fz + h));
    }
    u32 dirv[3];
    Unit(dirv);
    const double dist = P(8) ? 0.0 : (P(50) ? double(R(5000)) / 100.0 : double(R(300000)) / 100.0);
    for (u32 k = 0; k < 3; ++k) { c.ftgt[k] = B(float(double(F(c.xform[k])) + double(F(dirv[k])) * dist)); c.fsrc[k] = B(Fl(-3000.0f, 3000.0f)); }
    // --- tarea: entidad, matriz, coordenadas, animacion
    c.ent = f == 5 ? 0 : (f == 4 ? 1 : P(65));
    {
        u32 right[3], fwd[3], up[3];
        Unit(right); Unit(fwd); Unit(up);
        for (u32 k = 0; k < 3; ++k) { c.emat[k] = right[k]; c.emat[4 + k] = fwd[k]; c.emat[8 + k] = up[k]; }
        c.emat[12] = B(float(vx + double(Fl(-20.0f, 20.0f)))); c.emat[13] = B(float(vy + double(Fl(-20.0f, 20.0f)))); c.emat[14] = B(float(vz + double(Fl(-20.0f, 20.0f))));
        c.emat[3] = Rand(n, nk()); c.emat[7] = Rand(n, nk()); c.emat[11] = Rand(n, nk()); c.emat[15] = Rand(n, nk());
    }
    if (c.ent) { for (u32 k = 0; k < 3; ++k) c.coords[k] = B(Fl(-30.0f, 30.0f)); }
    else { for (u32 k = 0; k < 3; ++k) c.coords[k] = c.ftgt[k]; }
    if (f == 6) {
        // coordenadas nulas: todas cero (con -0), una distinta de cero, o NaN
        const u32 m = R(6);
        for (u32 k = 0; k < 3; ++k) c.coords[k] = Pk({0u, 0x80000000u});
        if (m == 1) c.coords[R(3)] = Pk({1u, 0x80000001u, 0x3F800000u, 0xBF800000u});
        else if (m == 2) c.coords[R(3)] = 0xFFC00000u;
        else if (m == 3) c.coords[R(3)] = Pk({0x7F800000u, 0xFF800000u, 0x00800000u});
        else if (m == 4) c.coords[0] = B(Fl(-30.0f, 30.0f));
    }
    c.assoc = P(70);
    c.animid = (f >= 7 && f <= 10) ? (c.assoc ? kAnimIds[(f - 7) * 2 + R(2)] : Rand(n, nk())) : kAnimIds[R(12)];
    c.fdir = Pk({0u, 1u, 2u, 3u, 4u, 0xFFu, 0x81u, 1u, 2u, 3u, 0x101u});
    if (f == 7) { c.fdir = Pk({0u, 1u, 3u, 4u}); if (!c.assoc) c.fdir = 1; }
    // --- caso concreto por familia
    if (f == 14) {
        // umbral 0.1f del producto con el eje: eje = (1,0,0); la IA sin entidad usa las coordenadas como punto del mundo, con el origen del disparo en el cero: direccion = coords normalizada
        const u32 side = R(4);
        c.assoc = 1; c.animid = side == 0 ? 0xE7u : side == 1 ? 0xEBu : side == 2 ? 0xEAu : 0xE9u;
        const double sx = (side == 0 || side == 2) ? -1.0 : 1.0;
        const double scale = P(50) ? 1.0 : double(R(100000) + 1) / 100.0;
        const double yz = std::sqrt(1.0 - 0.01);
        const double ang = double(R(62832)) / 10000.0;
        u32 bits = B(float(sx * 0.1 * scale));
        bits = NextUp(bits, int(R(9)) - 4);
        c.coords[0] = bits; c.coords[1] = B(float(yz * std::cos(ang) * scale)); c.coords[2] = B(float(yz * std::sin(ang) * scale));
        c.ent = 0;
        c.xform[0] = 0; c.xform[1] = 0; c.xform[2] = 0;
        c.vmat[12] = B(Fl(-1.0f, 1.0f)); c.vmat[13] = B(Fl(-1.0f, 1.0f)); c.vmat[14] = B(Fl(-1.0f, 1.0f));
        c.ptype = 2 + R(30);
    }
    if (f == 13) {
        // origen = objetivo (vector nulo) o casi, o coordenadas con extremos
        c.ent = 0; c.ptype = 2 + R(30);
        for (u32 k = 0; k < 3; ++k) c.coords[k] = P(70) ? c.xform[k] : Special(n, nk());
        if (P(30)) for (u32 k = 0; k < 3; ++k) c.coords[k] = B(float(double(F(c.xform[k])) + double(R(3)) * 1e-30));
    }
    if (f == 12) {
        for (u32 k = 0; k < 3; ++k) {
            if (P(8)) c.xform[k] = Special(n, nk());
            if (P(8)) c.coords[k] = Special(n, nk());
            if (P(8)) c.ftgt[k] = Special(n, nk());
            if (P(6)) c.fsrc[k] = Special(n, nk());
            if (P(6)) c.vmat[12 + k] = Special(n, nk());
            if (P(6)) c.ppos[k] = Special(n, nk());
            if (P(6)) c.csrc[c.cam * 3 + k] = Special(n, nk());
        }
        for (u32 k = 0; k < 16; ++k) { if (P(5) && k % 4 != 3) c.vmat[k] = Special(n, nk()); if (P(5) && k % 4 != 3) c.emat[k] = Special(n, nk()); }
        for (u32 k = 0; k < 6; ++k) { if (P(8)) c.boxA[k] = Special(n, nk()); if (P(8)) c.boxB[k] = Special(n, nk()); }
    }
    // --- mutaciones: una escritura en memoria dentro de un falso
    if (f == 11) {
        c.mutSite = 1 + R(4);
        c.mutVal = P(30) ? Special(n, nk()) : B(Fl(-30.0f, 30.0f));
        if (P(30)) c.mutVal = Pk({0u, 0x80000000u});
    }
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    auto V = [](const u32* p, std::size_t k) { return std::vector<u32>(p, p + k); };
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("ptype", c.ptype).U("cam", c.cam).A("cmode", V(c.cmode, 3)).A("csrc", V(c.csrc, 9));
    j.U("pmat", c.pmat).A("ppos", V(c.ppos, 3)).A("palt", V(c.palt, 3)).U("slot", c.slot).U("ikinit", c.ikinit).U("atkinit", c.atkinit);
    j.A("fireoff", V(c.fireoff, 3)).A("xform", V(c.xform, 3)).U("bonetag", c.bonetag).U("boneidx", c.boneidx);
    j.U("vmodel", c.vmodel).U("vmodel2", c.vmodel2).A("vmat", V(c.vmat, 16)).A("boxa", V(c.boxA, 6)).A("boxb", V(c.boxB, 6));
    j.A("fsrc", V(c.fsrc, 3)).A("ftgt", V(c.ftgt, 3));
    j.U("ent", c.ent).A("emat", V(c.emat, 16)).A("coords", V(c.coords, 3)).U("assoc", c.assoc).U("animid", c.animid).U("fdir", c.fdir);
    j.U("mutsite", c.mutSite).U("mutval", c.mutVal).U("gate", c.gate);
    return j.End();
}
}

#pragma once
// Fixtures del banco CarCtrlSteer63e (lote 63, CCarCtrl parte B1, tanda 5): SteerAICarParkParallel (0x433BA0) y SteerAICarParkPerpendicular (0x433EA0), 2 x 4096 casos.
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y un programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
//   CCarCtrl::SteerAICarParkParallel       0x433BA0  cdecl (CVehicle* vehicle, float* steer, float* accel, float* brake, bool* handbrake), 5 argumentos, void  (con marco SEH del compilador)
//   CCarCtrl::SteerAICarParkPerpendicular  0x433EA0  igual
//
// Cada caso es una descripcion SEMANTICA (la leen el banco y fixture_oracle.py): vehiculo (nodos, mision, banderas, ocupantes), nodos de camino reales en la arena (CPathNode de 0x1C bytes
// con la posicion int16 en +8/+0xA/+0xC; el EXE y la DLL leen la misma memoria), pool de tareas y resultados de los colaboradores sustituidos.
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

namespace CS63EFix {
using i16 = std::int16_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i32 = std::int32_t;

constexpr unsigned kFns = 2;
constexpr unsigned kPerFn = 4096;
constexpr unsigned kCases = kFns * kPerFn;
constexpr unsigned kFamilies = 8;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 31337u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline u32 Bits(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
inline float Flt(u32 v) { float f; std::memcpy(&f, &v, 4); return f; }

// ------------------------------------------------------------------------------------------------ disposicion (bytes del EXE)
constexpr u32 kVehSize = 0x900, kMatWords = 16, kPedSize = 0x500, kIntelWords = 8, kNodeSize = 0x1C, kNodesPerArea = 4, kGuardWords = 4, kOutWords = 8, kNumPeds = 9, kTaskWords = 5, kNumTasks = 9;
constexpr u32 kVMat = 0x14, kVPos = 0x04, kVMission = 0x3BA, kVCruise = 0x3D0, kVMove = 0x3DC, kVFlags = 0x428, kVCurrent = 0x390, kVStart = 0x394, kVDriver = 0x460, kVPass = 0x464;
constexpr u32 kPedIntel = 0x47C;
constexpr u32 kPathNodesAddr = 0x96F854, kPathNodesWords = 48;     // CPathFind::m_pPathNodes (puntero por area)
constexpr u32 kGarbageSeed = 0x6F4A7C15u;
inline u32 Garbage(u32 n, u32 k) {
    u32 v = 0xC0000000u | (Rand(n + kGarbageSeed, k) & 0x3FFFFFFFu);
    return v == 0xFFFFFFFFu ? 0xC0000000u : v;
}

enum Fn : u32 { kParallel = 0, kPerpendicular = 1 };
// tipos de colaborador (indice en los eventos)
enum : u32 { kNew = 1, kSetTask, kHeadingFake, kKinds };

struct Spec {
    u32 fn{}, family{}, regs[4]{};
    u32 hasMat{1};
    u32 vpos[3]{}, mpos[3]{};
    u32 startAddr{}, curAddr{};              // CNodeAddress (area en los 16 bits bajos, indice en los altos)
    u32 areaLoaded[2]{1, 1};                 // 0: area del nodo inicial, 1: area del nodo actual (puntero nulo si 0)
    u32 sameArea{};                          // el nodo actual usa el mismo array que el inicial
    i16 node[2][kNodesPerArea][3]{};         // posicion int16 de los nodos de cada array
    u32 mission{}, cruise{}, flags428{}, flags3DC{};
    u32 hasDriver{}, passMask{}, intelNull{};
    u32 newMask{};                           // bit k: la k-esima reserva de CTask::operator new devuelve un bloque (0 = pool agotado)
    u32 hOut[4]{};                           // HeadingForTarget falso: steer, accel, brake y byte del freno de mano
    u32 outPre[4]{};
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

inline Spec MakeSpec(u32 n) {
    Spec s;
    Gen g(n);
    s.fn = n / kPerFn;
    s.family = n % kFamilies;
    s.regs[0] = StackSeed(n);
    s.regs[1] = g.C(30) ? Garbage(n, 1) : g.R();
    s.regs[2] = g.C(30) ? Garbage(n, 2) : g.R();
    s.regs[3] = g.C(30) ? Garbage(n, 3) : g.R();
    for (int i = 0; i < 4; ++i) s.outPre[i] = Garbage(n, 70 + i);
    for (int i = 0; i < 3; ++i) { s.vpos[i] = Garbage(n, 10 + i); s.mpos[i] = Garbage(n, 20 + i); }
    s.hasMat = g.C(75) ? 1u : 0u;

    // nodos: dos arrays de 4 nodos; coordenadas int16 (x0.125) con rectas y agrupadas
    const int span = g.C(70) ? 800 : 6000;
    for (int a = 0; a < 2; ++a)
        for (u32 i = 0; i < kNodesPerArea; ++i)
            for (int c = 0; c < 3; ++c) s.node[a][i][c] = i16(int(g.P(u32(2 * span + 1))) - span);
    if (g.C(25)) { for (int a = 0; a < 2; ++a) for (u32 i = 1; i < kNodesPerArea; ++i) { s.node[a][i][0] = s.node[a][0][0]; s.node[a][i][1] = s.node[a][0][1]; } }   // nodos alineados / coincidentes (direccion nula)
    const u32 area0 = g.P(48), area1 = g.C(40) ? area0 : g.P(48);
    s.sameArea = area0 == area1 ? 1u : 0u;
    const u32 idx0 = g.P(kNodesPerArea), idx1 = s.sameArea && g.C(15) ? idx0 : g.P(kNodesPerArea);
    s.startAddr = area0 | (idx0 << 16);
    s.curAddr = area1 | (idx1 << 16);
    s.areaLoaded[0] = g.C(94) ? 1u : 0u;
    s.areaLoaded[1] = g.C(94) ? 1u : 0u;
    if (s.sameArea) s.areaLoaded[1] = s.areaLoaded[0];

    // vehiculo: posicion cerca del punto de aparcado (grid) o aleatoria
    const u32 sa = s.sameArea ? 0u : 0u;
    (void)sa;
    const float nx0 = float(s.node[0][idx0][0]) * 0.125f, ny0 = float(s.node[0][idx0][1]) * 0.125f;
    static const float offs[]{0.0f, 0.5f, 1.0f, 1.9f, 2.0f, 2.1f, 3.9f, 4.0f, 4.1f, 5.9f, 6.0f, 6.1f, 10.0f, 30.0f};
    const bool nearTarget = g.C(55);   // vehiculo a menos de ~1.2 m del nodo inicial: alcanza la rama de aparcado (distancia < 2)
    float px = nx0 + (nearTarget ? Grid(g, 6, 0.2f) : (g.C(50) ? 1.0f : -1.0f) * offs[g.P(sizeof(offs) / sizeof(offs[0]))]);
    float py = ny0 + (nearTarget ? Grid(g, 6, 0.2f) : (g.C(50) ? 1.0f : -1.0f) * offs[g.P(sizeof(offs) / sizeof(offs[0]))]);
    if (g.C(15)) { px = Grid(g, 400, 0.5f); py = Grid(g, 400, 0.5f); }
    const float pz = Grid(g, 40, 0.5f);
    if (s.hasMat) { s.mpos[0] = Bits(px); s.mpos[1] = Bits(py); s.mpos[2] = Bits(pz); }
    else { s.vpos[0] = Bits(px); s.vpos[1] = Bits(py); s.vpos[2] = Bits(pz); }

    // mision: la de entrada de cada funcion en el 45 % de los casos
    static const u32 others[]{0x0B, 0x32, 0x31, 0x2D, 0x2E, 0x00, 0x14, 0x13};
    if (s.fn == kParallel) s.mission = g.C(45) ? 0x2Eu : (g.C(70) ? others[g.P(8)] : (g.R() & 0xFF));
    else s.mission = g.C(45) ? 0x2Du : (g.C(70) ? others[g.P(8)] : (g.R() & 0xFF));
    s.cruise = g.C(60) ? g.P(12) : (g.R() & 0xFF);
    s.flags428 = g.R() & 0xFF;
    s.flags3DC = g.R() & 0xFF;
    s.hasDriver = g.C(70) ? 1u : 0u;
    s.passMask = g.C(30) ? 0u : (g.R() & 0xFF);
    s.intelNull = 0;
    s.newMask = g.C(60) ? 0x1FFu : (g.C(50) ? 0u : (g.R() & 0x1FFu));
    {
        static const float h[]{0.0f, 0.5f, -0.5f, 1.0f, 0.3f, -0.1f, 0.7f};
        for (int i = 0; i < 3; ++i) s.hOut[i] = g.C(6) ? kQNaN : (g.C(70) ? PickF(g, h) : Bits(float(g.P(2001)) * 0.001f - 1.0f));
        s.hOut[3] = g.C(50) ? (g.R() & 0xFF) : (g.C(50) ? 1u : 0u);
    }
    return s;
}

inline std::string U(u64 v) { return std::to_string(v); }
inline std::string Arr(const u32* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string JsonSpec(const Spec& s) {
    std::string nodes = "[";
    for (int a = 0; a < 2; ++a) {
        if (a) nodes += ',';
        nodes += '[';
        for (u32 i = 0; i < kNodesPerArea; ++i) { if (i) nodes += ','; nodes += "[" + std::to_string(s.node[a][i][0]) + "," + std::to_string(s.node[a][i][1]) + "," + std::to_string(s.node[a][i][2]) + "]"; }
        nodes += ']';
    }
    nodes += ']';
    return "{\"fn\":" + U(s.fn) + ",\"fam\":" + U(s.family) + ",\"regs\":" + Arr(s.regs, 4) + ",\"mat\":" + U(s.hasMat) + ",\"vpos\":" + Arr(s.vpos, 3) + ",\"mpos\":" + Arr(s.mpos, 3) + ",\"start\":" + U(s.startAddr) +
           ",\"cur\":" + U(s.curAddr) + ",\"loaded\":" + Arr(s.areaLoaded, 2) + ",\"same\":" + U(s.sameArea) + ",\"nodes\":" + nodes + ",\"mission\":" + U(s.mission) + ",\"cruise\":" + U(s.cruise) +
           ",\"flags428\":" + U(s.flags428) + ",\"flags3dc\":" + U(s.flags3DC) + ",\"driver\":" + U(s.hasDriver) + ",\"pass\":" + U(s.passMask) + ",\"newmask\":" + U(s.newMask) + ",\"hout\":" + Arr(s.hOut, 4) +
           ",\"outpre\":" + Arr(s.outPre, 4) + "}";
}
}  // namespace CS63EFix

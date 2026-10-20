#pragma once
// Fixtures del banco GarageUpdate63 (lote 63b): CGarage::Update (0x44AA50), 16384 casos en 16 familias. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp
// y un programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
//   CGarage::Update  0x44AA50  thiscall (int32 garageId), ret 4, void. Ningun llamador del EXE lee EAX/ECX/EDX (0x44C910 solo usa ESI/EDI, preservados).
//
// Cada caso es una descripcion SEMANTICA (la leen el banco y fixture_oracle.py): garaje (tipo, estado, banderas, limites), estado global real que lee la funcion (reloj, contador de
// fotogramas, dinero, mando, banderas de CGarages), jugador/vehiculos falsos y el comportamiento de los ~45 colaboradores sustituidos (mascaras de resultado por numero de llamada).
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

namespace GU63Fix {
using i16 = std::int16_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFns = 1;
constexpr unsigned kFamilies = 16;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline u32 Bits(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
inline float Flt(u32 v) { float f; std::memcpy(&f, &v, 4); return f; }

// ------------------------------------------------------------------------------------------------ disposicion
constexpr u32 kGarageSize = 0xD8, kVehSize = 0xA18, kMaxVeh = 8, kMatWords = 24, kGuardWords = 4, kPedWords = 0x600 / 4, kWantedWords = 0x40 / 4, kVtWords = 64;
// campos del CGarage (bytes)
constexpr u32 kGPosZ = 0x08, kGTopZ = 0x1C, kGLeft = 0x28, kGRight = 0x2C, kGFront = 0x30, kGBack = 0x34, kGDoorPos = 0x38, kGTime = 0x3C, kGTarget = 0x40, kGType = 0x4C, kGState = 0x4D, kGFlags = 0x4E;
// estado global real del EXE que Update lee/escribe
constexpr u32 kTimerAddr = 0xB7CB84, kFrameAddr = 0xB7CB4C, kFocusAddr = 0xB7CD74, kMoneyBase = 0xB7CE50, kPlayerStride = 0x190;
constexpr u32 kNoResprays = 0x96C008, kRespraysFree = 0x96C009, kBombsFree = 0x96C00A, kLastGarage = 0x96BFDC, kLastHelp = 0x96BFE0, kCamOutside = 0x96BFE4, kCamIn = 0xB6F844, kCamOut = 0xB6F848, kBombShop = 0x8E6940;
constexpr u32 kPadW = 0xB73458 + 0x10A, kPadB = 0xB73458 + 0x10E;
constexpr u32 kGarbageSeed = 0x6A09E667u;
inline u32 Garbage(u32 n, u32 k) {
    u32 v = 0xC0000000u | (Rand(n + kGarbageSeed, k) & 0x3FFFFFFFu);
    return v == 0xFFFFFFFFu ? 0xC0000000u : v;
}

// kinds de colaborador (indice en las mascaras y en los eventos)
enum : u32 {
    kFpv = 1, kFpp, kFpc, kFpw, kIn3d, kOut, kStatic, kEmpty, kTouch, kRemoveCars, kStoreHide, kSlideOpen, kSlideClosed, kIsOutside, kBlocking, kCountCenter, kOtherPed, kOtherCar, kThrow, kCalcDist,
    kRestoreHide, kNeatly, kRestoreImp, kStoreImp, kFindDoors, kMsg, kFindSafe, kCountHide, kSprayable, kDetonator, kCallOff, kIncStat, kGetStat, kSetStat, kAudio, kReqModel, kHelp, kTextGet,
    kClearWanted, kGetRemap, kSetRemap, kAppearance, kDestroy, kChoose, kAddParticle, kRandF, kFix, kRand, kKinds
};

struct Veh {
    u32 model{0}, hasMat{1}, pos[3]{}, mpos[3]{}, f42E{}, f42F{}, col[4]{}, f4A8{}, f4B0{}, health{}, f4CC{}, t590{}, t594{}, f7BC{}, f868{}, f8E4{};
    u32 mright[3]{}, mup[3]{};     // palabras 0..2 y 8..10 de la matriz (los ejes que se niegan al dar la vuelta al coche)
};
struct Spec {
    u32 family{}, regs[4]{};
    u32 gtype{}, gstate{}, gflags{}, gdoor{}, gtime{}, gb[6]{}, target{0xFF}, garageId{};
    u32 timer{}, frame{}, focus{}, money{};
    u32 noResprays{}, respraysFree{}, bombsFree{}, lastGarage{}, lastHelp{}, camOutside{}, camIn{}, camOut{}, bombShop{};
    u32 padW{}, padB{};
    u32 pv{1}, pvIdx{0}, pedNull{0}, pedFlags{}, pedVeh{0xFF};
    u32 wByte{}, wDword{};
    u32 coord[8][3]{};
    Veh veh[kMaxVeh]{};
    u32 mask[kKinds]{};
    u32 dist[6]{}, appear[4]{}, remap[2]{}, countCenter[3]{}, countHide[2]{}, randf[16]{}, stat{}, colors[4]{}, doors[2]{0xFF, 0xFF}, seed{};
};

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_) : n(n_) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
};
constexpr u32 kQNaN = 0x7FC00000u, kInf = 0x7F800000u, kNegInf = 0xFF800000u;

// distancias / umbrales que miran las ramas (floats)
inline u32 PickDist(Gen& g) {
    static const float v[]{0.0f, 1.0f, 15.99f, 16.0f, 16.01f, 63.99f, 64.0f, 64.01f, 12.2f, 12.25f, 12.3f, 99.9f, 100.0f, 224.9f, 225.0f, 225.1f, 288.9f, 289.0f, 289.1f, 3599.0f, 3600.0f, 3601.0f,
                           4224.0f, 4225.0f, 4226.0f, 4899.0f, 4900.0f, 4901.0f, 0.5f, -1.0f, 1.0e30f};
    if (g.C(5)) return kQNaN;
    return Bits(v[g.P(sizeof(v) / sizeof(v[0]))]);
}
inline u32 PickCoordZ(Gen& g) {
    static const float v[]{0.0f, 10.0f, 949.9f, 950.0f, 950.1f, 1000.0f, -5.0f};
    if (g.C(4)) return kQNaN;
    return Bits(v[g.P(sizeof(v) / sizeof(v[0]))]);
}
inline u32 Sprinkle(Gen& g, u32 normal) {   // casi siempre `normal`; a veces NaN / infinito / -0.0
    if (!g.C(6)) return normal;
    static const u32 odd[]{kQNaN, kInf, kNegInf, 0x80000000u, 0x00000001u, 0x7F000000u};
    return odd[g.P(6)];
}

inline void GenVeh(Gen& g, Veh& v, float cx, float cy, float cz) {
    static const u32 models[]{0, 0x1A7, 0x23B, 5, 0};
    v.model = models[g.P(5)];
    v.hasMat = g.C(70) ? 1u : 0u;
    const float ox = float(int(g.P(40)) - 20), oy = float(int(g.P(40)) - 20);
    const u32 pos[3]{Bits(cx + ox), Bits(cy + oy), Bits(cz)};
    for (int i = 0; i < 3; ++i) { v.pos[i] = g.C(50) ? pos[i] : Garbage(g.n, 40 + i); v.mpos[i] = g.C(50) ? pos[i] : Garbage(g.n, 50 + i); }
    v.f42E = g.R() & 0xFF; v.f42F = g.R() & 0xFF;
    for (int i = 0; i < 4; ++i) v.col[i] = g.R() & 0xFF;
    v.f4A8 = g.C(70) ? (g.R() & 0xF8) : g.R() & 0xFF;
    v.f4B0 = g.R();
    static const float hp[]{1000.0f, 1000.5f, 969.9f, 970.0f, 970.1f, 500.0f, 0.0f, 1500.0f, -10.0f};
    v.health = g.C(5) ? kQNaN : Bits(hp[g.P(9)]);
    v.f4CC = g.R();
    v.t590 = g.C(50) ? 0u : (g.C(50) ? 9u : g.P(12));
    v.t594 = g.C(40) ? 0xAu : (g.C(30) ? 9u : g.P(12));
    v.f7BC = g.R(); v.f868 = g.C(50) ? (g.R() & 0xFD) : g.R() & 0xFF; v.f8E4 = g.R();
    for (int i = 0; i < 3; ++i) { v.mright[i] = Bits(float(int(g.P(21)) - 10) * 0.5f); v.mup[i] = Bits(float(int(g.P(21)) - 10) * 0.5f); }
    if (g.C(50)) v.mup[2] = Bits(-0.5f - float(g.P(3)));   // z del eje "up" negativa: el coche se da la vuelta
}

constexpr u32 kTypesMisc[]{11, 15, 19, 23, 43};
constexpr u32 kTypesSpray[]{5};
constexpr u32 kTypesBomb[]{2, 3, 4};
constexpr u32 kTypesSafe[]{16, 17, 18, 24, 25, 26, 27, 28, 29, 30, 31, 32, 39, 40, 41, 42, 44, 45};
constexpr u32 kTypesImpound[]{33, 34, 35};
constexpr u32 kTypesTuning[]{36, 37, 38};

inline Spec MakeSpec(u32 n) {
    Spec s;
    Gen g(n);
    s.family = n % kFamilies;
    const u32 fam = s.family;
    s.regs[0] = StackSeed(n);
    s.regs[1] = g.C(30) ? Garbage(n, 1) : g.R();
    s.regs[2] = g.C(30) ? Garbage(n, 2) : g.R();
    s.regs[3] = g.C(30) ? Garbage(n, 3) : g.R();
    s.seed = g.R();
    // garaje
    const float left = float(int(g.P(60)) - 30), width = float(2 + g.P(20)), front = float(int(g.P(60)) - 30), depth = float(2 + g.P(20)), z0 = float(int(g.P(20)) - 10), h = float(2 + g.P(8));
    s.gb[0] = Bits(left); s.gb[1] = Bits(left + width); s.gb[2] = Bits(front); s.gb[3] = Bits(front + depth); s.gb[4] = Bits(z0); s.gb[5] = Bits(z0 + h);
    if (g.C(4)) s.gb[g.P(6)] = Sprinkle(g, s.gb[0]);
    const float cx = left + width * 0.5f, cy = front + depth * 0.5f;
    // tipo y estado segun la familia
    auto pick = [&](const u32* a, size_t cnt) { return a[g.P(u32(cnt))]; };
    switch (fam) {
    case 0:  s.gtype = g.C(60) ? g.P(52) : (g.C(50) ? 13u : 255u); s.gstate = g.P(8); break;                         // contabilidad y tipos arbitrarios
    case 1:  s.gtype = 5; s.gstate = g.C(85) ? g.P(5) : g.P(8); break;
    case 2:  s.gtype = 5; s.gstate = g.C(70) ? 0u : g.P(5); break;
    case 3:  s.gtype = pick(kTypesBomb, 3); s.gstate = g.C(85) ? g.P(5) : g.P(8); break;
    case 4:  s.gtype = 1; s.gstate = g.C(85) ? g.P(4) : g.P(8); break;
    case 5:  s.gtype = pick(kTypesMisc, 5); s.gstate = g.P(5); break;
    case 6:  s.gtype = g.C(50) ? 14u : 21u; s.gstate = g.P(7); break;
    case 7:  s.gtype = pick(kTypesSafe, sizeof(kTypesSafe) / sizeof(kTypesSafe[0])); s.gstate = g.C(85) ? g.P(4) : g.P(6); break;
    case 8:  s.gtype = g.C(50) ? 20u : 23u; s.gstate = g.P(5); break;
    case 9:  s.gtype = pick(kTypesImpound, 3); s.gstate = g.P(5); break;
    case 10: s.gtype = g.C(70) ? pick(kTypesTuning, 3) : 43u; s.gstate = g.P(5); break;
    default: s.gtype = g.C(80) ? g.P(47) : g.P(256); s.gstate = g.C(80) ? g.P(6) : g.P(256); break;
    }
    s.gflags = g.C(30) ? (g.R() & 0xFF) : (g.C(50) ? 0u : (g.C(50) ? 0x40u : 0x80u));
    if (fam != 0 && g.C(80)) s.gflags &= 0xDD;                       // sin 0x20 ni 0x02 casi siempre (0x02+estado 0 sale pronto)
    if (fam == 0 && g.C(40)) s.gflags |= 0x28;
    static const float doors[]{0.0f, 0.4f, 0.4001f, 0.39f, 0.5f, 0.5001f, 1.0f, 0.2f};
    s.gdoor = g.C(10) ? kQNaN : Bits(doors[g.P(8)]);
    s.timer = g.C(50) ? 100000u + g.P(100000) : g.R();
    s.gtime = g.C(50) ? s.timer - 1 + g.P(3) : (g.C(50) ? s.timer + g.P(5000) : g.R());   // el reloj frente a m_nTimeToOpen (unsigned): menor / igual / mayor
    if (g.C(25)) s.gtime = s.timer - 1 - g.P(3000);
    s.frame = g.C(60) ? (g.R() & ~0x1Fu) : g.R();
    s.focus = 0;
    static const int32_t money[]{0, 50, 99, 100, 101, 499, 500, 501, 100000, -50, 1};
    s.money = u32(money[g.P(11)]);
    s.noResprays = g.C(15) ? 1u : 0u; s.respraysFree = g.C(20) ? 1u : 0u; s.bombsFree = g.C(20) ? 1u : 0u;
    s.garageId = g.P(4);
    s.lastGarage = g.C(30) ? s.garageId : g.P(5); s.lastHelp = g.C(60) ? s.timer - 18000 - g.P(10) + 5 : s.timer - g.P(30000);
    s.camOutside = g.P(2); s.camIn = 0; s.camOut = 0; s.bombShop = g.C(50) ? 1u : g.P(3);
    s.padW = g.C(30) ? g.P(5) : (g.R() & 0xFFFF); s.padB = g.R() & 0xFF;
    // jugador
    s.pv = g.C(85) ? 1u : 0u; s.pvIdx = g.P(3);
    s.pedNull = g.C(6) ? 1u : 0u;
    s.pedFlags = g.C(50) ? (g.R() | 0x100u) : (g.R() & ~0x100u);
    s.pedVeh = g.C(60) ? 1u + g.P(2) : (g.C(50) ? 0xFFu : 3u);
    s.wByte = g.R() & 0xFF; s.wDword = g.C(50) ? 0u : g.R();
    // objetivo del garaje: el vehiculo del jugador, otro o ninguno
    s.target = g.C(30) ? 0xFFu : (g.C(60) ? s.pvIdx : (s.pvIdx + 1 + g.P(2)) % 3);
    for (u32 i = 0; i < kMaxVeh; ++i) GenVeh(g, s.veh[i], cx, cy, 0.0f);
    // coordenadas del jugador respecto al centro del garaje
    static const float offs[]{0.0f, 2.0f, 5.0f, 12.0f, 15.9f, 29.9f, 30.0f, 30.1f, 40.0f, 100.0f};
    for (int k = 0; k < 8; ++k) {
        const float ox = (g.C(50) ? 1.0f : -1.0f) * offs[g.P(10)], oy = (g.C(50) ? 1.0f : -1.0f) * offs[g.P(10)];
        s.coord[k][0] = g.C(4) ? Sprinkle(g, 0) : Bits(cx + (k < 2 ? ox : ox * 0.5f));
        s.coord[k][1] = g.C(4) ? Sprinkle(g, 0) : Bits(cy + oy);
        s.coord[k][2] = PickCoordZ(g);
    }
    if (g.C(30)) { for (int k = 0; k < 8; ++k) s.coord[k][2] = Bits(float(int(g.P(30))) - 10.0f); }   // z mayormente por debajo de 950 y a veces cerca de topZ / z del garaje
    if (g.C(25)) { s.coord[2][2] = Bits(z0 + h - 1.0f - float(g.P(3))); s.coord[3][2] = Bits(z0 + float(g.P(3)) * 0.5f); }
    // mascaras de resultado de los colaboradores
    for (u32 k = 1; k < kKinds; ++k) {
        const u32 r = g.R();
        s.mask[k] = g.C(15) ? ~0u : (g.C(15) ? 0u : r);
    }
    for (int i = 0; i < 6; ++i) s.dist[i] = PickDist(g);
    for (int i = 0; i < 4; ++i) s.appear[i] = g.C(25) ? 3u : (g.C(25) ? 5u : g.P(8));
    for (int i = 0; i < 2; ++i) s.remap[i] = g.C(60) ? 0xFFFFFFFFu : g.P(4);
    for (int i = 0; i < 3; ++i) s.countCenter[i] = g.P(7);
    for (int i = 0; i < 2; ++i) s.countHide[i] = g.P(7);
    for (int i = 0; i < 16; ++i) s.randf[i] = Bits(float(int(g.P(2000)) - 1000) * 0.003f);
    s.stat = g.C(5) ? kQNaN : Bits(float(g.P(5)));
    for (int i = 0; i < 4; ++i) s.colors[i] = g.P(256);
    s.doors[0] = g.C(25) ? 0xFFu : g.P(kMaxVeh); s.doors[1] = g.C(25) ? 0xFFu : g.P(kMaxVeh);
    // ajustes por familia que acercan la ejecucion a las ramas profundas
    if (s.gtype == 5 && s.gstate == 0) s.veh[s.pvIdx].hasMat = 1;   // el EXE desreferencia la matriz del vehiculo del jugador sin comprobarla
    if (fam == 2 || fam == 1) { s.noResprays = g.C(5) ? 1u : 0u; s.pv = 1; s.pedNull = 0; }
    if (fam == 2) { s.veh[s.pvIdx].t590 = g.C(60) ? 0u : (g.C(60) ? 9u : 2u); s.gflags &= ~0x80u; if (g.C(40)) s.gflags |= 0x80u; s.veh[s.pvIdx].model = 0; }
    if (fam == 7 && g.C(50)) { static const float closeD[]{5.0f, 12.0f, 12.3f, 50.0f, 99.0f}; for (int i = 0; i < 6; ++i) s.dist[i] = Bits(closeD[g.P(5)]); for (int i = 0; i < 2; ++i) s.countHide[i] = 4 + g.P(3); s.mask[kBlocking] = 0; }
    if (fam == 7 && g.C(60)) {   // las puertas del refugio junto al jugador (distancia < 5)
        for (int d = 0; d < 2; ++d) if (s.doors[d] < kMaxVeh && s.doors[d] != s.pvIdx) { Veh& dv = s.veh[s.doors[d]]; const Veh& pvv = s.veh[s.pvIdx]; dv.hasMat = pvv.hasMat; for (int i = 0; i < 3; ++i) { dv.pos[i] = pvv.pos[i]; dv.mpos[i] = pvv.mpos[i]; } }
        s.lastHelp = s.timer - 18001 - g.P(100);
    }
    if (fam == 7 && g.C(70)) { s.pv = 1; s.pedNull = 0; for (int k = 0; k < 4; ++k) s.coord[k][2] = Bits(float(g.P(10))); }
    if (!s.pv) { s.mask[kStatic] = 0; s.mask[kOtherCar] = 0; }   // sin vehiculo del jugador el EXE no llega a estas ramas con el vehiculo (desreferencia sin comprobar)
    return s;
}

inline std::string U(u32 v) { return std::to_string(v); }
inline std::string Arr(const u32* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string JsonVeh(const Veh& v) {
    return "{\"model\":" + U(v.model) + ",\"mat\":" + U(v.hasMat) + ",\"pos\":" + Arr(v.pos, 3) + ",\"mpos\":" + Arr(v.mpos, 3) + ",\"f42E\":" + U(v.f42E) + ",\"f42F\":" + U(v.f42F) + ",\"col\":" + Arr(v.col, 4) +
           ",\"f4A8\":" + U(v.f4A8) + ",\"f4B0\":" + U(v.f4B0) + ",\"hp\":" + U(v.health) + ",\"f4CC\":" + U(v.f4CC) + ",\"t590\":" + U(v.t590) + ",\"t594\":" + U(v.t594) + ",\"f7BC\":" + U(v.f7BC) +
           ",\"f868\":" + U(v.f868) + ",\"f8E4\":" + U(v.f8E4) + ",\"mright\":" + Arr(v.mright, 3) + ",\"mup\":" + Arr(v.mup, 3) + "}";
}
inline std::string JsonSpec(const Spec& s) {
    std::string o = "{\"fam\":" + U(s.family) + ",\"regs\":" + Arr(s.regs, 4) + ",\"g\":[" + U(s.gtype) + "," + U(s.gstate) + "," + U(s.gflags) + "," + U(s.gdoor) + "," + U(s.gtime) + "],\"gb\":" + Arr(s.gb, 6) +
                    ",\"target\":" + U(s.target) + ",\"gid\":" + U(s.garageId) + ",\"timer\":" + U(s.timer) + ",\"frame\":" + U(s.frame) + ",\"focus\":" + U(s.focus) + ",\"money\":" + U(s.money) +
                    ",\"glob\":[" + U(s.noResprays) + "," + U(s.respraysFree) + "," + U(s.bombsFree) + "," + U(s.lastGarage) + "," + U(s.lastHelp) + "," + U(s.camOutside) + "," + U(s.camIn) + "," + U(s.camOut) + "," + U(s.bombShop) +
                    "],\"pad\":[" + U(s.padW) + "," + U(s.padB) + "],\"pv\":[" + U(s.pv) + "," + U(s.pvIdx) + "," + U(s.pedNull) + "," + U(s.pedFlags) + "," + U(s.pedVeh) + "],\"wanted\":[" + U(s.wByte) + "," + U(s.wDword) + "],\"coord\":[";
    for (int k = 0; k < 8; ++k) { if (k) o += ','; o += Arr(s.coord[k], 3); }
    o += "],\"veh\":[";
    for (u32 i = 0; i < kMaxVeh; ++i) { if (i) o += ','; o += JsonVeh(s.veh[i]); }
    o += "],\"mask\":" + Arr(s.mask, kKinds) + ",\"dist\":" + Arr(s.dist, 6) + ",\"appear\":" + Arr(s.appear, 4) + ",\"remap\":" + Arr(s.remap, 2) + ",\"cc\":" + Arr(s.countCenter, 3) + ",\"ch\":" + Arr(s.countHide, 2) +
         ",\"randf\":" + Arr(s.randf, 16) + ",\"stat\":" + U(s.stat) + ",\"colors\":" + Arr(s.colors, 4) + ",\"doors\":" + Arr(s.doors, 2) + ",\"seed\":" + U(s.seed) + "}";
    return o;
}
}  // namespace GU63Fix

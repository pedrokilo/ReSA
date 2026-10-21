#pragma once
// Fixtures del banco CarCtrlWeave63B (lote 63, CCarCtrl parte B2, tanda B): las seis funciones de recorrido de listas de sector, 4096 casos cada una. Solo C++ estandar.
//
//   0 CCarCtrl::SlowCarDownForCarsSectorList     0x432420  cdecl (list*, CVehicle*, float x4, float*, float)
//   1 CCarCtrl::SlowCarDownForObjectsSectorList  0x42D4F0  cdecl (list*, CVehicle*, float x4, float*, float)
//   2 CCarCtrl::SlowCarDownForPedsSectorList     0x425440  cdecl (list*, CVehicle*, float x4, float*, float)
//   3 CCarCtrl::WeaveThroughCarsSectorList       0x42D680  cdecl (list*, CVehicle*, CPhysical*, float x4, float*, float*)
//   4 CCarCtrl::WeaveThroughObjectsSectorList    0x42D950  cdecl (list*, CVehicle*, float x4, float*, float*)
//   5 CCarCtrl::WeaveThroughPedsSectorList       0x42D7E0  cdecl (list*, CVehicle*, CPhysical*, float x4, float*, float*)
//
// Cada caso es una descripcion SEMANTICA. Los nodos de la lista, las entidades, las matrices, los col models, los pedestres con su inteligencia y el jugador (ds:[0xB7CD74], remoto) son
// objetos del banco; los colaboradores con efectos (GetBoundCentre, DistAlongLine2D, las funciones SlowCarDownFor*/WeaveFor*, GetMatrix, FindPlayerVehicle/Ped, el registro de referencias, los
// eventos de peaton) son falsos identicos en ambos lados que registran sus argumentos EN ORDEN.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

namespace CwBFix {
using i16 = std::int16_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFns = 6;
constexpr u32 kSlowCars = 0, kSlowObjs = 1, kSlowPeds = 2, kWeaveCars = 3, kWeaveObjs = 4, kWeavePeds = 5;
constexpr unsigned kFamilies = 12;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline u32 Bits(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
inline float Flt(u32 v) { float f; std::memcpy(&f, &v, 4); return f; }

// ------------------------------------------------------------------------------------------------ disposicion
constexpr u32 kEnts = 7;                    // E0 = vehiculo; E1..E6 = entidades de la lista / fisico
constexpr u32 kMaxNodes = 6;
constexpr u32 kEntWords = 0x600 / 4;
constexpr u32 kMatWords = 0x50 / 4;
constexpr u32 kCmWords = 0x30 / 4;
constexpr u32 kMiWords = 0x40 / 4;
constexpr u32 kIntelWords = 0x100 / 4;      // inteligencia del peaton (el grupo de eventos esta en +0x68)
constexpr u32 kGuardWords = 4;
constexpr u32 kModelBase = 0x4C00;
// campos de la entidad (bytes)
constexpr u32 kOffPos = 4, kOffMatrix = 0x14, kOffModel = 0x22, kOffFlags = 0x1C, kOffScan = 0x2C, kOffB36 = 0x36, kOffSpeed = 0x44, kOffTimeSw = 0x3B0, kOffStyle = 0x3B9, kOffTempAct = 0x3BB,
                kOffTempTime = 0x3BC, kOffCtrl = 0x3DB, kOff42B = 0x42B, kOffDriver = 0x460, kOffIntel = 0x47C, kOff514 = 0x514, kOff584 = 0x584, kOffFC = 0xFC, kOffVType = 0x590;
constexpr u32 kTimerAddr = 0xB7CB84, kScanAddr = 0xB7CD78, kFocusAddr = 0xB7CD74, kPlayersAddr = 0xB7CD98, kPlayerStride = 0x190, kRemoteOff = 0xD8;
constexpr u32 kGarbageSeed = 0x6A09E667u;
inline u32 Garbage(u32 n, u32 k) {
    u32 v = 0xC0000000u | (Rand(n + kGarbageSeed, k) & 0x3FFFFFFFu);
    return v == 0xFFFFFFFFu ? 0xC0000000u : v;
}

// ------------------------------------------------------------------------------------------------ especificacion
struct Ent {
    u32 hasMat{1};
    u32 modelSel{3};         // 0..2: MI_ROADWORKBARRIER1, MI_ROADBLOCKFUCKEDCAR1, MI_ROADBLOCKFUCKEDCAR2 (indices reales); 3: modelo propio del banco
    u32 pos[3]{}, mpos[3]{};
    u32 right[3]{}, fwd[3]{}, up[3]{};
    u32 speed[3]{};
    u32 centre[3]{};         // resultado del GetBoundCentre falso
    u32 box[6]{};
    u32 b36{2};
    u32 flags{1};            // byte +0x1C (bit 0 = usa colision)
    u32 scan{0};             // codigo de exploracion (u16)
    u32 vtype{0}, style{0}, ctrl{0}, tempAct{0}, tempTime{0}, d514{0};
    u32 driver{0};           // 0 nulo, 1..kEnts = entidad i-1
    u32 b42B{0};
    u32 d584{0}, dFC{0};     // 0 nulo, 1 = vehiculo, 2 = otra entidad, 3 = basura
};
struct Spec {
    u32 fn{}, family{};
    u32 regs[4]{};
    Ent e[kEnts]{};
    u32 nodes{0};            // numero de nodos de la lista
    u32 item[kMaxNodes]{};   // entidad (0..kEnts-1) de cada nodo
    u32 physical{1};         // entidad que hace de CPhysical* (funciones 3 y 5)
    u32 xmin{}, ymin{}, xmax{}, ymax{};
    u32 out3{}, out4{};      // *arg7 / *arg8 iniciales
    u32 cruise{};            // ultimo argumento float de las funciones Slow*
    u32 scanNow{100};        // CWorld::ms_nCurrentScanCode
    u32 now{};               // reloj
    u32 dist[kMaxNodes]{};   // resultado de cada DistAlongLine2D falso
    u32 player{0};           // FindPlayerVehicle: 0 nulo, 1..kEnts
    u32 playerPed{0};        // FindPlayerPed: 0 nulo, 1..kEnts
    u32 remote{0};           // Players[focus].m_pRemoteVehicle: 0 nulo, 1..kEnts
    u32 isPlayerMask{};      // CPed::IsPlayer del falso (bit por llamada)
    u32 seed{};
};

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_) : n(n_) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
};

constexpr u32 kQNaN = 0x7FC00000u;
inline float NextUp(float f) { return std::nextafterf(f, std::numeric_limits<float>::infinity()); }
inline float NextDown(float f) { return std::nextafterf(f, -std::numeric_limits<float>::infinity()); }
inline u32 Cont(Gen& g, float lo, float hi) { return Bits(lo + (hi - lo) * (float(g.R() >> 8) / 16777216.0f)); }
inline u32 Quarter(Gen& g, int lo, int hi) { return Bits(float(lo * 4 + int(g.P(u32((hi - lo) * 4 + 1)))) * 0.25f); }
inline float F(u32 v) { return Flt(v); }

inline void Dir2(Gen& g, bool grid, u32& x, u32& y) {
    if (grid) {
        static const float s = 0.70710678f;
        static const float dx[8]{1.0f, s, 0.0f, -s, -1.0f, -s, 0.0f, s};
        static const float dy[8]{0.0f, s, 1.0f, s, 0.0f, -s, -1.0f, -s};
        const u32 k = g.P(8);
        x = Bits(dx[k]); y = Bits(dy[k]);
    } else {
        const float a = 6.2831853f * (float(g.R() >> 8) / 16777216.0f);
        x = Bits(std::cos(a)); y = Bits(std::sin(a));
    }
}
inline void SetHasMat(Ent& o, u32 v) {
    if (o.hasMat != v) { for (int i = 0; i < 3; ++i) std::swap(o.pos[i], o.mpos[i]); }
    o.hasMat = v;
}
inline void SetPos(Ent& o, float x, float y, float z) {
    u32* p = o.hasMat ? o.mpos : o.pos;
    p[0] = Bits(x); p[1] = Bits(y); p[2] = Bits(z);
}
inline void GetPos(const Ent& o, float& x, float& y, float& z) {
    const u32* p = o.hasMat ? o.mpos : o.pos;
    x = Flt(p[0]); y = Flt(p[1]); z = Flt(p[2]);
}

inline void FillEnt(Gen& g, Ent& o, bool grid, u32 mode, u32 idx) {
    o.hasMat = g.C(85) ? 1u : 0u;
    o.modelSel = 3;
    for (int i = 0; i < 3; ++i) {
        const u32 decoy = g.C(50) ? Bits(50.0f + float(g.P(30))) : Garbage(g.n, 60 + i + 3 * idx);
        o.pos[i] = decoy;
        o.mpos[i] = decoy;
    }
    u32 cx, sy;
    Dir2(g, grid, cx, sy);
    const float c = Flt(cx), s = Flt(sy);
    o.right[0] = Bits(c); o.right[1] = Bits(s); o.right[2] = Bits(0.0f);
    o.fwd[0] = Bits(-s); o.fwd[1] = Bits(c); o.fwd[2] = Bits(0.0f);
    o.up[0] = Bits(0.0f); o.up[1] = Bits(0.0f); o.up[2] = Bits(1.0f);
    if (mode == 1) { o.fwd[2] = Cont(g, -0.4f, 0.4f); o.right[2] = Cont(g, -0.4f, 0.4f); }
    if (mode == 2) o.up[2] = Bits(g.C(40) ? 0.9f : (g.C(50) ? NextUp(0.9f) : NextDown(0.9f)));
    if (mode == 3) o.up[2] = Cont(g, -1.0f, 1.0f);
    for (int i = 0; i < 3; ++i) o.speed[i] = grid ? Quarter(g, -2, 2) : Cont(g, -0.5f, 0.5f);
    for (int i = 0; i < 3; ++i) { o.box[i] = Quarter(g, -3, -1); o.box[3 + i] = Quarter(g, 1, 3); }
    o.b36 = 2;
    o.flags = g.C(85) ? 1u : (g.C(50) ? 0u : g.P(256) & ~1u);
    o.scan = g.C(15) ? 100u : 7u;   // 100 = el codigo de exploracion actual (ya visitada)
    o.d514 = g.C(60) ? 0u : g.R();
    o.b42B = g.C(45) ? 8u : (g.C(50) ? 0u : g.P(256));
    o.d584 = g.P(4); o.dFC = g.P(4);
    o.driver = g.C(60) ? 0u : 1u + g.P(kEnts);
}

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
    const bool grid = fam == 1 || fam == 2;
    for (u32 i = 0; i < kEnts; ++i) FillEnt(g, s.e[i], grid, fam == 7 ? 1u : (fam == 8 ? 2u + g.P(2) : 0u), i);
    Ent& V = s.e[0];
    SetHasMat(V, 1);                                // el EXE usa la matriz del vehiculo sin comprobar nulo
    V.scan = 7;
    V.flags = 1;
    const float vx = grid ? F(Quarter(g, -10, 10)) : F(Cont(g, -10.0f, 10.0f)), vy = grid ? F(Quarter(g, -10, 10)) : F(Cont(g, -10.0f, 10.0f)), vz = F(Quarter(g, -3, 3));
    SetPos(V, vx, vy, vz);
    V.b36 = g.C(70) ? 0x10u | 2u : g.P(256);       // estado en los bits altos (la exploracion mira ==3<<3 y ==2<<3)
    static const u32 types[]{0, 3, 4, 6, 9, 1, 2, 5, 7, 8, 10};
    V.vtype = g.C(60) ? types[g.P(11)] : g.P(12);
    static const u32 styles[]{0, 1, 4, 6, 2, 3, 5, 7};
    V.style = g.C(70) ? styles[g.P(4)] : styles[g.P(8)];
    V.ctrl = g.P(256);
    V.tempAct = g.P(256);
    V.tempTime = g.R();
    V.d514 = g.C(55) ? 1u : 0u;
    V.driver = g.C(55) ? 0u : 2u + g.P(5);          // una entidad de la lista hace de conductor (E2..E6)
    // rectangulo alrededor del vehiculo
    const float radii[]{4.0f, 10.0f, 20.0f, 0.0f, 40.0f};
    const float r = radii[g.P(5)];
    s.xmin = Bits(vx - r); s.xmax = Bits(vx + r); s.ymin = Bits(vy - r); s.ymax = Bits(vy + r);
    if (g.C(10)) std::swap(s.xmin, s.xmax);
    s.out3 = g.C(20) ? Cont(g, 0.0f, 6.2831853f) : Cont(g, 0.0f, 15.0f);
    s.out4 = g.C(20) ? Cont(g, 0.0f, 6.2831853f) : Cont(g, 0.0f, 15.0f);
    s.cruise = Bits(g.C(20) ? 0.0f : 5.0f + float(g.P(40)));
    s.scanNow = 100u;
    s.now = g.R() >> 4;
    s.nodes = g.C(8) ? 0u : 1u + g.P(kMaxNodes);
    for (u32 k = 0; k < kMaxNodes; ++k) s.item[k] = 1 + g.P(kEnts - 1);
    if (fn == kSlowCars && g.C(20)) s.item[g.P(kMaxNodes)] = 0;       // el propio vehiculo en la lista
    if (g.C(20)) s.item[g.P(kMaxNodes)] = s.item[g.P(kMaxNodes)];      // nodos con la misma entidad (codigo de exploracion)
    s.physical = 1 + g.P(kEnts - 1);
    if (g.C(30)) s.physical = s.item[g.P(kMaxNodes)];
    s.player = g.C(40) ? 0u : (g.C(50) ? 1u : 1u + g.P(kEnts));
    s.playerPed = g.C(40) ? 0u : 1u + g.P(kEnts);
    if (g.C(30)) s.playerPed = s.item[g.P(kMaxNodes)] + 1;
    s.remote = g.C(40) ? 0u : (g.C(50) ? 1u : 1u + g.P(kEnts));
    s.isPlayerMask = g.R();
    for (u32 k = 0; k < kMaxNodes; ++k) s.dist[k] = g.C(8) ? kQNaN : Cont(g, -3.0f, 25.0f);

    // entidades de la lista: modelos, tipos, posiciones dentro / fuera del rectangulo y del rango en z
    for (u32 i = 1; i < kEnts; ++i) {
        Ent& o = s.e[i];
        const float lo = std::min(F(s.xmin), F(s.xmax)), hi = std::max(F(s.xmin), F(s.xmax));
        const float ylo = F(s.ymin), yhi = F(s.ymax);
        float px = lo + (hi - lo) * (float(g.R() >> 8) / 16777216.0f), py = ylo + (yhi - ylo) * (float(g.R() >> 8) / 16777216.0f);
        float pz = vz + (g.C(75) ? F(Cont(g, -2.0f, 2.0f)) : F(Cont(g, -14.0f, 14.0f)));
        if (g.C(10)) px = g.C(50) ? lo : hi;
        if (g.C(10)) py = g.C(50) ? ylo : yhi;
        if (grid) { px = std::floor(px * 4.0f) * 0.25f; py = std::floor(py * 4.0f) * 0.25f; pz = std::floor(pz * 4.0f) * 0.25f; }
        SetPos(o, px, py, pz);
        o.centre[0] = Bits(px + (g.C(30) ? 0.0f : F(Cont(g, -1.0f, 1.0f))));
        o.centre[1] = Bits(py + (g.C(30) ? 0.0f : F(Cont(g, -1.0f, 1.0f))));
        o.centre[2] = Bits(pz + (g.C(40) ? 0.0f : F(Cont(g, -1.0f, 1.0f))));
        if (fn == kSlowObjs || fn == kWeaveObjs) o.modelSel = g.C(60) ? g.P(3) : 3u;
        o.b36 = (fn == kSlowPeds || fn == kWeavePeds) ? (g.C(80) ? 3u : g.P(8)) : (g.C(90) ? 2u : g.P(8));
        // distancia en y a la que el resultado de DistAlongLine2D coloca el z esperado: z del centro ~ dist*fwd.z + vz
        if (g.C(50)) o.centre[2] = Bits(F(o.fwd[2]) * 0.0f + vz + F(Cont(g, -3.0f, 3.0f)));
    }
    if (fn == kSlowPeds && g.C(70)) {
        // peatones delante del vehiculo, dentro del rectangulo y de la ventana de frenado; el vehiculo avanza y su estilo de conduccion es 0, 1, 4 o 6
        const float fx = F(V.fwd[0]), fy = F(V.fwd[1]), rx = F(V.right[0]), ry = F(V.right[1]);
        const float halfLen = F(V.box[4]);
        for (u32 i = 1; i < kEnts; ++i) {
            const float u = F(Cont(g, 0.0f, 14.0f)), w = F(Cont(g, -2.0f, 2.0f));
            SetPos(s.e[i], vx + fx * (halfLen + u) + rx * w, vy + fy * (halfLen + u) + ry * w, vz + F(Cont(g, -1.0f, 1.0f)));
            s.e[i].flags = 1;
            s.e[i].scan = 7;
        }
        s.xmin = Bits(vx - 40.0f); s.xmax = Bits(vx + 40.0f); s.ymin = Bits(vy - 40.0f); s.ymax = Bits(vy + 40.0f);
        static const u32 pedStyles[]{0, 1, 4, 6};
        V.style = pedStyles[g.P(4)];
        const float sp = F(Cont(g, 0.02f, 0.6f)) * (g.C(85) ? 1.0f : -1.0f);
        V.speed[0] = Bits(fx * sp); V.speed[1] = Bits(fy * sp); V.speed[2] = Bits(0.0f);
        s.out3 = Cont(g, 0.0f, 20.0f);
    }
    if (fam >= 4 && fam <= 6) {   // un valor raro
        u32* slots[]{&V.fwd[0], &V.fwd[1], &V.right[0], &V.right[1], &V.speed[0], &V.speed[1], &V.speed[2], &s.out3, &s.out4, &s.cruise, &s.dist[0], &s.dist[1], &s.e[1].centre[0], &s.e[2].centre[1],
                     &s.e[1].mpos[2], &s.e[2].mpos[0], &s.e[1].pos[2], &s.xmin, &s.ymax};
        u32& slot = *slots[g.P(sizeof(slots) / sizeof(slots[0]))];
        slot = fam == 4 ? kQNaN : (fam == 5 ? 0x80000000u : Bits(0.0f));
    }
    return s;
}

inline std::string U(u32 v) { return std::to_string(v); }
inline std::string Arr(const u32* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string JsonEnt(const Ent& v) {
    return "{\"mat\":" + U(v.hasMat) + ",\"msel\":" + U(v.modelSel) + ",\"pos\":" + Arr(v.pos, 3) + ",\"mpos\":" + Arr(v.mpos, 3) + ",\"right\":" + Arr(v.right, 3) + ",\"fwd\":" + Arr(v.fwd, 3) +
           ",\"up\":" + Arr(v.up, 3) + ",\"speed\":" + Arr(v.speed, 3) + ",\"centre\":" + Arr(v.centre, 3) + ",\"box\":" + Arr(v.box, 6) + ",\"b36\":" + U(v.b36) + ",\"flags\":" + U(v.flags) + ",\"scan\":" + U(v.scan) +
           ",\"veh\":[" + U(v.vtype) + "," + U(v.style) + "," + U(v.ctrl) + "," + U(v.tempAct) + "," + U(v.tempTime) + "," + U(v.d514) + "," + U(v.driver) + "," + U(v.b42B) + "," + U(v.d584) + "," + U(v.dFC) + "]}";
}
inline std::string JsonSpec(const Spec& s) {
    std::string o = "{\"fn\":" + U(s.fn) + ",\"fam\":" + U(s.family) + ",\"regs\":" + Arr(s.regs, 4) + ",\"nodes\":" + U(s.nodes) + ",\"item\":" + Arr(s.item, kMaxNodes) + ",\"physical\":" + U(s.physical) +
                    ",\"rect\":[" + U(s.xmin) + "," + U(s.ymin) + "," + U(s.xmax) + "," + U(s.ymax) + "],\"out\":[" + U(s.out3) + "," + U(s.out4) + "],\"cruise\":" + U(s.cruise) + ",\"scan_now\":" + U(s.scanNow) +
                    ",\"now\":" + U(s.now) + ",\"dist\":" + Arr(s.dist, kMaxNodes) + ",\"player\":" + U(s.player) + ",\"player_ped\":" + U(s.playerPed) + ",\"remote\":" + U(s.remote) + ",\"is_player\":" + U(s.isPlayerMask) +
                    ",\"seed\":" + U(s.seed) + ",\"ent\":[";
    for (u32 i = 0; i < kEnts; ++i) { if (i) o += ','; o += JsonEnt(s.e[i]); }
    return o + "]}";
}
}  // namespace CwBFix

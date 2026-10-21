#pragma once
// Fixtures del banco Remote64 (lote 64): CRemote::GivePlayerRemoteControlledCar (0x45AB10, 8192 casos) y CRemote::TakeRemoteControlledCarFromPlayer (0x45AE80, 4096 casos).
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y un programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
//   CRemote::GivePlayerRemoteControlledCar           0x45AB10  cdecl (x, y, z, rotacion, modelo), cinco dwords, void
//   CRemote::TakeRemoteControlledCarFromPlayer       0x45AE80  cdecl (bool), un dword, void
//
// Cada caso es una descripcion SEMANTICA (la leen el banco y fixture_oracle.py): argumentos, comportamiento de los colaboradores sustituidos, estado real del EXE que las funciones leen o
// escriben (jugador en foco, reloj, puntero del pool de vehiculos y dos CPlayerInfo) y los vehiculos falsos de la arena (memoria rellenada por una formula conocida por el oraculo).
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

namespace R64Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCasesGive = 8192, kCasesTake = 4096;
constexpr unsigned kFns = 2;
constexpr u32 kGive = 0, kTake = 1;
constexpr unsigned kFamilies = 8;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline u32 Bits(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
inline float Flt(u32 v) { float f; std::memcpy(&f, &v, 4); return f; }

// ------------------------------------------------------------------------------------------------ disposicion
constexpr u32 kVehSize = 0xA18, kVehWords = kVehSize / 4, kMaxVeh = 6, kMatWords = 24, kGuardWords = 4, kPoolWords = 8, kStatesBytes = 48, kStatesBase = 24;
constexpr u32 kNone = 0xFF;   // "sin vehiculo" en los indices
// estado real del EXE
constexpr u32 kFocusAddr = 0xB7CD74, kTimerAddr = 0xB7CB84, kPoolPtrAddr = 0xB74494, kPlayers = 0xB7CD98, kPlayerStride = 0x190;
constexpr u32 kRemoteOff = 0xB0, kFlagDD = 0xDD, kFlagDE = 0xDE, kFlagDF = 0xDF, kTimeOff = 0xE0;
constexpr u32 kCamera = 0xB6F028, kMissionCleanup = 0xA90850;
constexpr u32 kGarbageSeed = 0x6A09E667u;
inline u32 Garbage(u32 n, u32 k) {
    u32 v = 0xC0000000u | (Rand(n + kGarbageSeed, k) & 0x3FFFFFFFu);
    return v == 0xFFFFFFFFu ? 0xC0000000u : v;
}
// contenido inicial de la arena (lo conocen C++ y el oraculo)
inline u32 VehJunk(u32 n, u32 i, u32 w) { return Rand(n ^ 0x51EDu, i * 0x1000u + w); }
inline u32 MatJunk(u32 n, u32 i, u32 w) { return Rand(n ^ 0x3A7u, 0x100u * i + w + 77u); }

// kinds de colaborador (indice en los eventos)
enum : u32 { kIsHeli = 1, kIsPlane, kNew, kCtorHeli, kCtorPlane, kCtorAuto, kGround, kDist, kRotZ, kJoin, kAdd, kFpv, kRegRef, kTakeCtl, kZoom, kSetCreated, kRemove, kKinds };

struct Mut { u32 kind{}, focus{}, pl{}, idx{kNone}, off{}; };   // reentrada de un falso: 1 = cambia el jugador en foco, 2 = cambia el vehiculo remoto de un jugador, 3 = ambos
struct Spec {
    u32 fn{}, family{}, regs[4]{};
    u32 pos[3]{}, rot{}, model{}, barg{};
    u32 heli{}, plane{}, newIdx{0}, retIdx{kNone}, ground{}, dist{}, gflag{};
    u32 hasMat[kMaxVeh]{};
    u32 j428{}, j42D{}, pv{}, pvIdx{0};
    u32 focus{}, timer{};
    u32 slot[2][2]{{kNone, 0}, {kNone, 0}};          // vehiculo remoto de cada jugador (indice, desplazamiento)
    u32 fl[2][4]{};                                  // palabras iniciales 0xDD, 0xDE, 0xDF (bytes) y 0xE0 de cada jugador
    u32 stor[2]{0, 0};                               // almacenamiento del pool (indice de vehiculo, desplazamiento)
    u32 states[kStatesBytes]{};                      // estados de los huecos del pool falso
    u32 pokes[4][3]{};                               // (indice de vehiculo, desplazamiento en bytes, valor) aplicados tras rellenar la arena; indice kNone = sin efecto
    Mut mA, mB;                                      // reentradas de SetVehicleCreatedBy / RemoveEntityFromList
};

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_) : n(n_) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
};
constexpr u32 kQNaN = 0x7FC00000u, kSNaN = 0x7F800001u, kInf = 0x7F800000u, kNegInf = 0xFF800000u;

inline u32 PickCoord(Gen& g, bool allowOdd) {
    if (allowOdd && g.C(14)) {
        static const u32 odd[]{kQNaN, kSNaN, kInf, kNegInf, 0x80000000u, 0u, 0x00000001u, 0xFFC00001u, 0x7F7FFFFFu};
        return odd[g.P(9)];
    }
    if (g.C(30)) return Bits(float(int(g.P(6000)) - 3000));
    return Bits(float(int(g.P(600000)) - 300000) * 0.01f);
}
// valores finitos y sin subnormales: la suma de dos de ellos se redondea a float igual en la x87 de 24 bits y en SSE
inline u32 PickFiniteBits(Gen& g) {
    static const float v[]{0.0f, -0.0f, 1.5f, -1.5f, 2.25f, 123.456f, -77.125f, 0.0009765625f, 1000.0f, 340000.5f, -0.75f, 0.3f, 7.0f, 15.5f, -3.0e5f, 12345.678f};
    return Bits(v[g.P(16)]);
}

inline void PickSlots(Gen& g, Spec& s, bool needArena) {
    for (int p = 0; p < 2; ++p) {
        if (!needArena && g.C(30)) { s.slot[p][0] = kNone; s.slot[p][1] = 0; }
        else { s.slot[p][0] = g.P(kMaxVeh); s.slot[p][1] = g.C(50) ? 0u : g.P(0x500); }
        s.fl[p][0] = g.R() & 0xFF; s.fl[p][1] = g.R() & 0xFF; s.fl[p][2] = g.R() & 0xFF; s.fl[p][3] = g.C(30) ? 0u : g.R();
    }
}

inline Spec MakeSpec(u32 fn, u32 n) {
    Spec s;
    s.fn = fn;
    Gen g(n + fn * 0x20000u + 1u);
    s.family = n % kFamilies;
    const u32 fam = s.family;
    s.regs[0] = StackSeed(n);
    s.regs[1] = g.C(30) ? Garbage(n, 1) : g.R();
    s.regs[2] = g.C(30) ? Garbage(n, 2) : g.R();
    s.regs[3] = g.C(30) ? Garbage(n, 3) : g.R();
    s.focus = g.P(2);
    s.timer = g.C(30) ? g.P(1000) : g.R();
    for (u32 i = 0; i < kMaxVeh; ++i) s.hasMat[i] = g.C(70) ? 1u : 0u;
    // pool falso: almacenamiento dentro de la arena y estados de hueco conocidos
    s.stor[0] = g.P(kMaxVeh); s.stor[1] = g.C(50) ? 0u : g.P(0x400);
    for (u32 i = 0; i < kStatesBytes; ++i) s.states[i] = g.R() & 0xFF;
    for (auto& pk : s.pokes) pk[0] = kNone;
    if (fn == kGive) {
        s.pos[0] = PickCoord(g, true); s.pos[1] = PickCoord(g, true); s.pos[2] = PickCoord(g, true);
        s.rot = g.C(80) ? Bits(float(int(g.P(6283)) - 3141) * 0.001f) : PickCoord(g, true);
        const u32 low = g.C(35) ? 0x8000u + g.P(0x8000u) : g.P(0x8000u);
        s.model = low | (g.C(50) ? (g.R() & 0xFFFF0000u) : 0u);
        const u32 type = fam % 3;   // 0 automovil, 1 helicoptero, 2 avion
        s.heli = type == 1; s.plane = type == 2;
        s.newIdx = g.P(kMaxVeh);
        s.retIdx = (fam >= 3 && fam < 6) ? g.P(kMaxVeh) : kNone;   // el constructor devuelve otro vehiculo
        if (s.retIdx == s.newIdx) s.retIdx = kNone;
        s.ground = PickFiniteBits(g); s.dist = PickFiniteBits(g); s.gflag = g.R() & 1u;
        s.j428 = g.C(50) ? 0u : (g.R() & 0xFF); s.j42D = g.C(50) ? 0u : (g.R() & 0xFF);
        const u32 subject = s.retIdx == kNone ? s.newIdx : s.retIdx;
        s.pv = g.C(75) ? 1u : 0u; s.pvIdx = (fam == 7) ? subject : g.P(kMaxVeh);
        s.hasMat[subject] = fam == 6 ? 0u : (g.C(85) ? 1u : 0u);   // fam 6: sin matriz (la posicion va al placement)
        PickSlots(g, s, false);
        s.barg = 0;
    } else {
        s.barg = (g.R() & 0xFFFFFF00u) | (g.R() & 1u);
        PickSlots(g, s, true);
        // el vehiculo remoto del jugador en foco y su CreatedBy (+0x4A4)
        const u32 f = s.focus;
        const bool created = fam != 1;
        if (fam == 3) s.slot[f][1] = 1 + g.P(0x4FF);   // vehiculo sin alinear
        u32 value = 2;
        if (!created) { value = g.C(50) ? 0u : 1u + g.P(255); if (value == 2) value = 3; }
        s.pokes[0][0] = s.slot[f][0]; s.pokes[0][1] = s.slot[f][1] + 0x4A4; s.pokes[0][2] = value;
        if (fam == 2) { s.stor[0] = kMaxVeh - 1; s.stor[1] = 0; }   // almacenamiento tras el vehiculo: indice negativo
        auto mutation = [&](Mut& m, bool focusToo, bool remoteToo) {
            m.kind = (focusToo ? 1u : 0u) | (remoteToo ? 2u : 0u);
            m.focus = g.P(2); m.pl = g.P(2); m.idx = g.P(kMaxVeh); m.off = g.C(50) ? 0u : g.P(0x500);
        };
        if (fam == 4) mutation(s.mA, true, false);
        if (fam == 5) mutation(s.mA, false, true);
        if (fam == 6) mutation(s.mB, true, false);
        if (fam == 7) { const bool a = g.C(50), b = g.C(60), c = g.C(50), d = g.C(60); mutation(s.mA, a, b || !a); mutation(s.mB, c, d || !c); }
    }
    return s;
}

inline std::string U(u32 v) { return std::to_string(v); }
inline std::string Arr(const u32* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string JsonMut(const Mut& m) { const u32 a[5]{m.kind, m.focus, m.pl, m.idx, m.off}; return Arr(a, 5); }
inline std::string JsonSpec(const Spec& s) {
    std::string o = "{\"fn\":" + U(s.fn) + ",\"fam\":" + U(s.family) + ",\"regs\":" + Arr(s.regs, 4) + ",\"pos\":" + Arr(s.pos, 3) + ",\"rot\":" + U(s.rot) + ",\"model\":" + U(s.model) + ",\"barg\":" + U(s.barg) +
                    ",\"heli\":" + U(s.heli) + ",\"plane\":" + U(s.plane) + ",\"new\":" + U(s.newIdx) + ",\"ret\":" + U(s.retIdx) + ",\"ground\":" + U(s.ground) + ",\"dist\":" + U(s.dist) + ",\"gflag\":" + U(s.gflag) +
                    ",\"mat\":" + Arr(s.hasMat, kMaxVeh) + ",\"j\":[" + U(s.j428) + "," + U(s.j42D) + "],\"pv\":[" + U(s.pv) + "," + U(s.pvIdx) + "],\"focus\":" + U(s.focus) + ",\"timer\":" + U(s.timer) +
                    ",\"slots\":[" + Arr(s.slot[0], 2) + "," + Arr(s.slot[1], 2) + "],\"fl\":[" + Arr(s.fl[0], 4) + "," + Arr(s.fl[1], 4) + "],\"stor\":" + Arr(s.stor, 2) + ",\"states\":" + Arr(s.states, kStatesBytes) + ",\"pokes\":[";
    for (int k = 0; k < 4; ++k) { if (k) o += ','; o += Arr(s.pokes[k], 3); }
    o += "],\"mA\":" + JsonMut(s.mA) + ",\"mB\":" + JsonMut(s.mB) + "}";
    return o;
}
}  // namespace R64Fix

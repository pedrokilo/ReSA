#pragma once
// Fixtures del banco CarCtrlWeave63C (lote 63, CCarCtrl parte B2, tanda C): CCarCtrl::UpdateCarOnRails (0x436540), 4096 casos. Solo C++ estandar.
//
//   0 CCarCtrl::UpdateCarOnRails   0x436540   cdecl (CVehicle*), void
//
// Cada caso es una descripcion SEMANTICA. El vehiculo, los tramos de carretera (dos "areas" de enlaces de la tabla ThePaths.m_pNaviNodes sustituidas por memoria del banco durante cada
// llamada), el reloj y las funciones con efectos (0x422590, 0x432B10 que avanza el tramo, SlowCarOnRailsDownForTrafficAndLights, CAutoPilot::ModifySpeed, CCurves::CalcCurvePoint y 0x42EC90) son
// objetos o falsos identicos en ambos lados que registran sus argumentos EN ORDEN. Se ejecutan de verdad el Normalise del EXE (0x59C910) y CCarPathLink::OneWayLaneOffset (0x44DB00 / inline).
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

namespace CwCFix {
using i16 = std::int16_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFns = 1;
constexpr unsigned kFamilies = 12;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline u32 Bits(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
inline float Flt(u32 v) { float f; std::memcpy(&f, &v, 4); return f; }

// ------------------------------------------------------------------------------------------------ disposicion
constexpr u32 kEntWords = 0x600 / 4;
constexpr u32 kLinks = 8;                   // 2 areas x 4 enlaces
constexpr u32 kLinkBytes = 0xE;
constexpr u32 kLinkWords = 32;              // 8 enlaces de 0xE bytes caben en 32 palabras (con relleno)
constexpr u32 kGuardWords = 4;
// campos del vehiculo (bytes)
constexpr u32 kOffSpeed = 0x44, kOffSeed = 0x20, kOffB36 = 0x36, kOffFieldC = 0x39C, kOffScale = 0x3A0, kOffCur = 0x3A4, kOffNext = 0x3A6, kOffLaneCurr = 0x3B5, kOffLaneNext = 0x3B6, kOffLaneCur = 0x3B7,
                kOffLaneNx = 0x3B8, kOffTempAct = 0x3BB, kOffTempTime = 0x3BC, kOffMove = 0x3DC, kOffSub = 0x594, kOffStart = 0x3AC, kOffSwitched = 0x3B0;
constexpr u32 kTimerAddr = 0xB7CB84, kNaviAddr = 0x96F974;
constexpr u32 kGarbageSeed = 0x6A09E667u;
inline u32 Garbage(u32 n, u32 k) {
    u32 v = 0xC0000000u | (Rand(n + kGarbageSeed, k) & 0x3FFFFFFFu);
    return v == 0xFFFFFFFFu ? 0xC0000000u : v;
}

// ------------------------------------------------------------------------------------------------ especificacion
struct Link {
    u32 x{}, y{};            // int16 en los 16 bits bajos (posicion * 8)
    u32 dx{}, dy{};          // int8 (direccion * 100)
    u32 width{}, lanes{};    // bytes +0xA y +0xB
};
struct Spec {
    u32 fn{}, family{};
    u32 regs[4]{};
    Link link[kLinks]{};
    u32 curSel{}, nextSel{};              // enlace actual / siguiente (0..7: area = sel >> 2, indice = sel & 3)
    u32 advCur{}, advNext{}, advField{};  // lo que deja 0x432B10 (falso): enlaces y field_C; advanceKind 0 = no toca nada
    u32 advKind{};
    u32 moveFlags{}, tempAct{}, tempTime{}, fieldC{}, scale{}, b36{2};
    u32 laneCurr{}, laneNext{}, laneCur{}, laneNx{};   // bytes con signo
    u32 sub{0}, seed{};
    u32 speed[3]{};
    u32 now{};
    u32 curvePos[2]{}, curveSpeed[3]{};   // resultado de CalcCurvePoint (falso)
};

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_) : n(n_) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
};

constexpr u32 kQNaN = 0x7FC00000u;
inline u32 Cont(Gen& g, float lo, float hi) { return Bits(lo + (hi - lo) * (float(g.R() >> 8) / 16777216.0f)); }

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
    s.seed = g.R() & 0xFFFFu;
    for (u32 i = 0; i < kLinks; ++i) {
        Link& l = s.link[i];
        l.x = u32(i16(int(g.P(8000)) - 4000)) & 0xFFFFu;
        l.y = u32(i16(int(g.P(8000)) - 4000)) & 0xFFFFu;
        static const int dirs[]{-100, -71, 0, 71, 100};
        l.dx = u32(u8(i16(dirs[g.P(5)])));
        l.dy = u32(u8(i16(dirs[g.P(5)])));
        if (g.C(10)) { l.dx = g.P(256); l.dy = g.P(256); }
        // +0xB: bits 0-2 carriles opuestos, bits 3-5 mismo sentido; +0xA ancho
        const u32 opp = g.C(30) ? 0u : g.P(8), same = g.C(30) ? 0u : g.P(8);
        l.lanes = opp | (same << 3) | (g.P(4) << 6);
        l.width = g.C(30) ? 16u : g.P(256);
    }
    s.curSel = g.P(kLinks);
    s.nextSel = g.C(15) ? s.curSel : g.P(kLinks);
    s.advCur = g.P(kLinks); s.advNext = g.P(kLinks);
    s.advKind = g.C(60) ? 0u : (g.C(60) ? 1u : 2u);       // 1: avanza a otros enlaces y rehace field_C; 2: solo field_C
    s.now = g.C(30) ? g.R() : (g.R() >> 4);
    // movimiento / accion temporal
    s.moveFlags = g.C(12) ? 1u | (g.P(256) & ~1u) : (g.P(256) & ~1u);
    static const u32 acts[]{1, 24, 12, 0, 2, 3, 5, 9, 25, 255, 128};
    s.tempAct = g.C(70) ? 0u : acts[g.P(11)];
    if (fam == 3) s.tempAct = 1; if (fam == 4) s.tempAct = 24; if (fam == 5) s.tempAct = 12;
    // tiempo de accion respecto a now: iguales, +-1, muy grandes (envolvente)
    const u32 tdiff[]{0u, 1u, 0xFFFFFFFFu, 5000u, 0x80000000u};
    s.tempTime = s.now + tdiff[g.P(5)];
    if (g.C(30)) s.tempTime = g.R();
    // field_C / escala: now - field_C entre 0 y 2*escala, a veces negativos (envolvente), a veces el limite exacto
    s.scale = g.C(8) ? 0u : (g.C(5) ? g.R() : 100u + g.P(20000));
    const u32 elapsed = g.C(60) ? g.P(s.scale ? s.scale + (s.scale >> 1) : 5) : (g.C(50) ? s.scale : g.R());
    s.fieldC = s.now - elapsed;
    if (g.C(8)) s.fieldC = s.now + 1u + g.P(1000);               // elapsed negativo (+2^32)
    s.b36 = g.C(15) ? (0x18u | g.P(8)) : (g.C(60) ? 0x10u | 2u : g.P(256));
    auto lane = [&](void) { return g.C(70) ? u32(u8(i16(int(g.P(5)) - 2))) : g.P(256); };
    s.laneCurr = lane(); s.laneNext = lane(); s.laneCur = lane(); s.laneNx = lane();
    s.sub = g.C(20) ? 10u : g.P(12);
    for (int i = 0; i < 3; ++i) s.speed[i] = g.C(40) ? Bits(0.0f) : Cont(g, -1.0f, 1.0f);
    s.curvePos[0] = Cont(g, -1000.0f, 1000.0f); s.curvePos[1] = Cont(g, -1000.0f, 1000.0f);
    for (int i = 0; i < 3; ++i) s.curveSpeed[i] = g.C(10) ? kQNaN : Cont(g, -300.0f, 300.0f);
    if (fam >= 6 && fam <= 8) {
        // valores raros en lo que entra por float
        u32* slots[]{&s.curveSpeed[0], &s.curveSpeed[1], &s.curveSpeed[2], &s.curvePos[0], &s.curvePos[1], &s.speed[0]};
        u32& slot = *slots[g.P(6)];
        slot = fam == 6 ? kQNaN : (fam == 7 ? 0x7F800000u : 0x80000000u);
    }
    // la mayor parte de los casos recorren el camino principal
    if (fam >= 9 && g.C(70)) { s.moveFlags &= ~1u; s.tempAct = 0; s.b36 = 0x10u | 2u; s.scale = 1000u + g.P(5000); s.fieldC = s.now - g.P(s.scale + s.scale / 2); }
    return s;
}

inline std::string U(u32 v) { return std::to_string(v); }
inline std::string Arr(const u32* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string JsonSpec(const Spec& s) {
    std::string o = "{\"fn\":" + U(s.fn) + ",\"fam\":" + U(s.family) + ",\"regs\":" + Arr(s.regs, 4) + ",\"cur\":" + U(s.curSel) + ",\"next\":" + U(s.nextSel) + ",\"adv\":[" + U(s.advKind) + "," + U(s.advCur) + "," + U(s.advNext) +
                    "],\"move\":" + U(s.moveFlags) + ",\"temp\":[" + U(s.tempAct) + "," + U(s.tempTime) + "],\"fieldc\":" + U(s.fieldC) + ",\"scale\":" + U(s.scale) + ",\"b36\":" + U(s.b36) + ",\"lanes\":[" + U(s.laneCurr) + "," +
                    U(s.laneNext) + "," + U(s.laneCur) + "," + U(s.laneNx) + "],\"sub\":" + U(s.sub) + ",\"seed\":" + U(s.seed) + ",\"speed\":" + Arr(s.speed, 3) + ",\"now\":" + U(s.now) + ",\"cpos\":" + Arr(s.curvePos, 2) +
                    ",\"cspeed\":" + Arr(s.curveSpeed, 3) + ",\"link\":[";
    for (u32 i = 0; i < kLinks; ++i) { const Link& l = s.link[i]; if (i) o += ','; o += "[" + U(l.x) + "," + U(l.y) + "," + U(l.dx) + "," + U(l.dy) + "," + U(l.width) + "," + U(l.lanes) + "]"; }
    return o + "]}";
}
}  // namespace CwCFix

#pragma once
// Fixtures del banco CarCtrlWeave63A (lote 63, CCarCtrl parte B2, tanda A): cinco funciones "hoja", 4096 casos cada una. Solo C++ estandar (sin tipos del juego): las incluyen
// Core.hpp / LiveComparison.hpp y un programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
//   0 CCarCtrl::TestCollisionBetween2MovingRects                      0x425B30  cdecl (v1, v2, float, float, CVector*, CVector*) -> ST0
//   1 CCarCtrl::TestCollisionBetween2MovingRects_OnlyFrontBumper      0x425F70  cdecl (v1, v2, float, float, CVector*, CVector*) -> ST0
//   2 CCarCtrl::SlowCarDownForOtherCar                                0x42D0E0  cdecl (CEntity*, CVehicle*, float*, float), void
//   3 CCarCtrl::WeaveForOtherCar                                      0x426350  cdecl (CEntity*, CVehicle*, float*, float*), void
//   4 CCarCtrl::WeaveForObject                                        0x426BC0  cdecl (CEntity*, CVehicle*, float*, float*), void
//
// Cada caso es una descripcion SEMANTICA (la leen el banco y fixture_oracle.py). Las entidades, matrices, col models e info de modelo son objetos del banco; los colaboradores con
// efectos o dependientes de globales (jugador, matriz, referencias, atan2, SwaysInWind, las dos pruebas de colision, la prueba de barrido 0x421A50) son falsos identicos en ambos lados que
// registran sus argumentos EN ORDEN; los colaboradores matematicos puros (0x59C890, 0x59C790) se ejecutan de verdad.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

namespace CwAFix {
using i16 = std::int16_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFns = 5;
constexpr u32 kTc = 0, kFb = 1, kSlow = 2, kWoc = 3, kWfo = 4;
constexpr unsigned kFamilies = 12;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline u32 Bits(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
inline float Flt(u32 v) { float f; std::memcpy(&f, &v, 4); return f; }

// ------------------------------------------------------------------------------------------------ disposicion
constexpr u32 kObjs = 4;
constexpr u32 kEntWords = 0x600 / 4;       // entidad (vehiculo/objeto/peaton) de 0x600 bytes
constexpr u32 kMatWords = 0x50 / 4;        // CMatrix (derecha 0, adelante 0x10, arriba 0x20, posicion 0x30)
constexpr u32 kCmWords = 0x30 / 4;         // col model: caja min (0..8) y max (0xC..0x14)
constexpr u32 kMiWords = 0x40 / 4;         // info de modelo: +0x14 = col model
constexpr u32 kGuardWords = 4;
constexpr u32 kModelBase = 0x4C00;         // primer indice de modelo propio buscado (el banco busca kObjs entradas nulas consecutivas a partir de aqui)
// campos de la entidad (bytes)
constexpr u32 kOffPos = 4, kOffHeading = 0x10, kOffMatrix = 0x14, kOffModel = 0x22, kOffFlags = 0x1C, kOffB36 = 0x36, kOffSpeed = 0x44, kOffTimeSw = 0x3B0, kOffStyle = 0x3B9, kOffMission = 0x3BA,
                kOffTempAct = 0x3BB, kOffTempTime = 0x3BC, kOffCtrl = 0x3DB, kOffTarget = 0x41C, kOffObstr = 0x420, kOff42B = 0x42B, kOff46C = 0x46C, kOff58C = 0x58C;
constexpr u32 kTimerAddr = 0xB7CB84;
constexpr u32 kGarbageSeed = 0x6A09E667u;
inline u32 Garbage(u32 n, u32 k) {
    u32 v = 0xC0000000u | (Rand(n + kGarbageSeed, k) & 0x3FFFFFFFu);
    return v == 0xFFFFFFFFu ? 0xC0000000u : v;
}

// ------------------------------------------------------------------------------------------------ especificacion
struct Obj {
    u32 hasMat{1};           // m_matrix != 0
    u32 modelSel{5};         // 0..4: MI_TRAFFICLIGHTS, SINGLESTREETLIGHTS1/2/3, DOUBLESTREETLIGHTS (indices reales del juego); 5: modelo propio del banco
    u32 pos[3]{};            // posicion de m_placement (+4)
    u32 mpos[3]{};           // posicion de la matriz (+0x30)
    u32 right[3]{}, fwd[3]{}, up[3]{};
    u32 speed[3]{};          // m_vecMoveSpeed (+0x44)
    u32 b36{2};              // byte +0x36: tipo de entidad (bits 0..2) y estado (bits 3..7)
    u32 timeSw{};            // +0x3B0
    u32 style{}, mission{}, ctrl{}, tempAct{}, tempTime{};
    u32 target{0};           // +0x41C: 0 nulo, 1..4 = objeto i-1, 5 = basura
    u32 b42B{}, d46C{}, d58C{};   // d58C: 0 nulo, 1..4 objeto i-1, 5 basura
    u32 box[6]{};            // col model: min xyz, max xyz
    u32 sways{};             // resultado (dword) de SwaysInWind para el modelo propio
};
struct Spec {
    u32 fn{}, family{};
    u32 regs[4]{};           // semilla de pila, EAX, ECX, EDX de entrada
    Obj o[kObjs]{};
    u32 ia{0}, ib{1};        // objetos que hacen de primer y segundo argumento
    u32 a3{}, a4{};          // argumentos float (funciones 0 y 1)
    u32 vecA[3]{}, vecB[3]{};
    u32 out3{}, out4{};      // valores iniciales de *arg3 / *arg4 (funciones 2..4)
    u32 tcF{}, tcB{};        // falsos de las pruebas de colision (SlowCarDownForOtherCar): OnlyFrontBumper / TestCollision
    u32 now{};               // reloj (0xB7CB84)
    u32 player{0};           // FindPlayerVehicle: 0 nulo, 1..4 objeto i-1
    u32 sweepMask{};         // WeaveForOtherCar: resultado de cada llamada a 0x421A50 (bit k = llamada k)
    u32 atanBits{};          // WeaveForObject: resultado de GetATanOfXY
    u32 seed{};
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
// valor "raro": NaN, infinitos, -0.0, enorme (nunca subnormal: el juego corre con MXCSR FTZ y el x87 del EXE los conserva)
inline u32 Odd(Gen& g) {
    switch (g.P(6)) {
    case 0: return kQNaN;
    case 1: return kInf;
    case 2: return kNegInf;
    case 3: return 0x80000000u;
    case 4: return 0x50000000u + g.P(0x7FFFFFu);       // grande (~1e10: sus cuadrados caben en un float; el x87 del EXE tiene exponente extendido y la DLL no)
    default: return 0xD0000000u + g.P(0x7FFFFFu);      // grande negativo
    }
}
inline u32 Cont(Gen& g, float lo, float hi) { return Bits(lo + (hi - lo) * (float(g.R() >> 8) / 16777216.0f)); }
inline u32 Quarter(Gen& g, int lo, int hi) { return Bits(float(lo * 4 + int(g.P(u32((hi - lo) * 4 + 1)))) * 0.25f); }
inline float F(u32 v) { return Flt(v); }

// direccion 2D: rejilla (8 direcciones exactas) o continua
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

// matriz de un objeto: giro de rumbo (adelante = (-sin, cos, 0), derecha = (cos, sin, 0)), a veces inclinada
inline void FillMatrix(Gen& g, Obj& o, bool grid, u32 mode) {
    u32 cx, sy;
    Dir2(g, grid, cx, sy);
    const float c = Flt(cx), s = Flt(sy);
    o.right[0] = Bits(c); o.right[1] = Bits(s); o.right[2] = Bits(0.0f);
    o.fwd[0] = Bits(-s); o.fwd[1] = Bits(c); o.fwd[2] = Bits(0.0f);
    o.up[0] = Bits(0.0f); o.up[1] = Bits(0.0f); o.up[2] = Bits(1.0f);
    switch (mode) {
    case 1: o.fwd[2] = Cont(g, -0.5f, 0.5f); o.right[2] = Cont(g, -0.5f, 0.5f); break;                           // inclinada
    case 2: for (int i = 0; i < 3; ++i) { o.fwd[i] = Bits(0.0f); } break;                                          // adelante nulo
    case 3: o.fwd[g.P(3)] = Odd(g); break;
    case 4: for (int i = 0; i < 3; ++i) { o.fwd[i] = Cont(g, -3.0f, 3.0f); o.right[i] = Cont(g, -3.0f, 3.0f); } break;     // sin normalizar
    case 5: o.up[2] = Bits(g.C(50) ? 0.9f : NextDown(0.9f)); break;
    default: break;
    }
}

inline void FillBox(Gen& g, Obj& o, u32 mode) {
    // caja del col model: min < 0 < max en cuartos
    for (int i = 0; i < 3; ++i) {
        o.box[i] = Quarter(g, -3, -1);
        o.box[3 + i] = Quarter(g, 1, 3);
    }
    switch (mode) {
    case 1: for (int i = 0; i < 6; ++i) o.box[i] = Cont(g, -4.0f, 4.0f); break;
    case 2: for (int i = 0; i < 6; ++i) o.box[i] = Bits(0.0f); break;
    case 3: for (int i = 0; i < 3; ++i) std::swap(o.box[i], o.box[3 + i]); break;     // invertida
    case 4: o.box[g.P(6)] = Odd(g); break;
    default: break;
    }
}

// cambia m_matrix nulo/no nulo conservando la posicion real en el sitio que corresponde
inline void SetHasMat(Obj& o, u32 v) {
    if (o.hasMat != v) { for (int i = 0; i < 3; ++i) std::swap(o.pos[i], o.mpos[i]); }
    o.hasMat = v;
}

inline void FillObj(Gen& g, Obj& o, u32 fam, float posRange, bool grid) {
    o.hasMat = g.C(80) ? 1u : 0u;
    o.modelSel = 5;
    for (int i = 0; i < 3; ++i) {
        const u32 real = grid ? Quarter(g, int(-posRange), int(posRange)) : Cont(g, -posRange, posRange);
        const u32 decoy = g.C(50) ? Bits(50.0f + float(g.P(30))) : Garbage(g.n, 60 + i);
        o.pos[i] = o.hasMat ? decoy : real;
        o.mpos[i] = o.hasMat ? real : decoy;
    }
    FillMatrix(g, o, grid, fam == 7 ? 1u : (fam == 8 ? g.P(6) : 0u));   // modo 3 (valor raro en adelante) solo en la familia 8
    FillBox(g, o, fam == 9 ? g.P(5) : 0u);
    for (int i = 0; i < 3; ++i) o.speed[i] = grid ? Quarter(g, -2, 2) : Cont(g, -0.5f, 0.5f);
}

inline void SpecialOdd(Gen& g, Spec& s, u32 fam) {
    const u32 keep3 = s.out3, keep4 = s.out4;
    // inyeccion de valores raros en un campo flotante cualquiera (familias 4..6)
    u32* slots[]{&s.a3, &s.a4, &s.vecA[0], &s.vecA[1], &s.vecB[0], &s.vecB[1], &s.o[0].mpos[0], &s.o[0].mpos[1], &s.o[1].mpos[0], &s.o[1].mpos[1], &s.o[0].box[3], &s.o[1].box[3],
                 &s.o[0].box[4], &s.o[1].box[4], &s.o[0].box[0], &s.o[1].box[1], &s.o[0].speed[0], &s.o[0].fwd[0], &s.o[1].fwd[1], &s.out3, &s.out4, &s.o[0].pos[0], &s.o[1].pos[1]};
    constexpr u32 count = sizeof(slots) / sizeof(slots[0]);
    const u32 times = 1;   // un solo valor raro: dos NaN de signo distinto en una operacion se propagan distinto en x87 (EXE) y SSE (DLL)
    for (u32 t = 0; t < times; ++t) {
        u32& slot = *slots[g.P(count)];
        if (s.fn == kWfo && fam != 4) { slot = g.C(50) ? kQNaN : 0x80000000u; continue; }   // infinitos o enormes darian un margen infinito: los bucles de angulos no terminarian
        switch (fam) {
        case 4: slot = kQNaN; break;
        case 5: slot = g.C(50) ? kInf : kNegInf; break;
        default: slot = g.C(50) ? Bits(0.0f) : (g.C(50) ? 0x80000000u : Odd(g)); break;
        }
    }
    if (s.fn == kWfo) { s.out3 = keep3; s.out4 = keep4; }   // los bucles de normalizacion de angulos no terminan con infinitos ni valores enormes (ni en el EXE ni en la DLL)
}

constexpr u32 kPlayerChoices = 6;
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
    const bool grid = fam == 1 || fam == 2 || fam == 3;     // rejilla: empates exactos
    const float range = fam == 10 ? 40.0f : (fam == 11 ? 3.0f : 12.0f);
    for (u32 i = 0; i < kObjs; ++i) FillObj(g, s.o[i], fam, range, grid);
    s.ia = g.P(kObjs);
    s.ib = (s.ia + 1 + g.P(kObjs - 1)) % kObjs;
    s.now = g.C(30) ? 0u : (g.C(30) ? 0xFFFFFFF0u : g.R() >> 4);
    s.out3 = Cont(g, 0.0f, 6.2831853f);
    s.out4 = Cont(g, 0.0f, 6.2831853f);

    // velocidad relativa y direcciones (funciones 0 y 1)
    const float speeds[]{0.5f, 2.0f, 8.0f, 30.0f};
    const float scale = speeds[g.P(4)];
    Dir2(g, grid, s.vecA[0], s.vecA[1]);
    Dir2(g, grid, s.vecB[0], s.vecB[1]);
    s.vecA[2] = Bits(0.0f); s.vecB[2] = Bits(0.0f);
    if (g.C(15)) { s.vecA[0] = Cont(g, -3.0f, 3.0f); s.vecA[1] = Cont(g, -3.0f, 3.0f); }
    if (g.C(15)) { s.vecB[0] = Cont(g, -3.0f, 3.0f); s.vecB[1] = Cont(g, -3.0f, 3.0f); }
    s.a3 = grid ? Quarter(g, int(-scale), int(scale)) : Cont(g, -scale, scale);
    s.a4 = grid ? Quarter(g, int(-scale), int(scale)) : Cont(g, -scale, scale);
    if (fam == 0 && g.C(30)) { s.a3 = Bits(0.0f); }
    if (fam == 0 && g.C(30)) { s.a4 = Bits(0.0f); }
    if (fam == 0 && g.C(10)) { s.a3 = 0x80000000u; s.a4 = 0x80000000u; }
    if (fam == 3 && g.C(40)) { s.a3 = Bits(0.0f); s.a4 = Bits(0.0f); }

    Obj& A = s.o[s.ia];   // primer vehiculo / entidad
    Obj& B = s.o[s.ib];   // segundo vehiculo / vehiculo
    switch (fn) {
    case kTc:
    case kFb:
        // los dos vehiculos son o[0] y o[1] (los argumentos 1 y 2); ia/ib no se usan
        s.ia = 0; s.ib = 1;
        // los dos cercanos para que haya cortes
        if (fam != 10 && g.C(80)) {
            for (int i = 0; i < 2; ++i) {
                const u32 slot = s.o[1].hasMat ? 0u : 1u;
                const u32 v = grid ? Quarter(g, -6, 6) : Cont(g, -6.0f, 6.0f);
                (slot == 0 ? s.o[1].mpos : s.o[1].pos)[i] = v;
            }
        }
        if (fam >= 4 && fam <= 6) SpecialOdd(g, s, fam);
        break;
    case kSlow: {
        SetHasMat(B, 1);      // car2: la matriz se usa sin comprobar nulo
        if (g.C(25)) SetHasMat(A, 0);
        A.b36 = g.C(75) ? 2u : g.P(8);
        B.b36 = g.C(30) ? (0x10u | g.P(8)) : g.P(256);
        const float arg4 = g.C(10) ? 0.0f : 5.0f + float(g.P(40));   // los valores raros de arg4 entran por SpecialOdd (dos NaN de signo distinto se propagan distinto en x87 y SSE)
        s.a4 = Bits(arg4);
        s.out3 = g.C(25) ? Bits(1.0f) : (g.C(60) ? Cont(g, 0.0f, arg4 > 0 ? arg4 : 1.0f) : Odd(g));
        // car1 delante del recorrido de car2 (lado >= 0) o detras
        const float rx = F(B.fwd[0]), ry = F(B.fwd[1]);
        const float dist = float(g.P(12));
        const float sgn = g.C(75) ? 1.0f : -1.0f;
        const u32 pb0 = B.hasMat ? B.mpos[0] : B.pos[0], pb1 = B.hasMat ? B.mpos[1] : B.pos[1];
        const float bx = F(pb0), by = F(pb1);
        const u32 nx = Bits(bx + sgn * dist * (std::isfinite(rx) ? rx : 1.0f) + float(int(g.P(5)) - 2) * 0.25f);
        const u32 ny = Bits(by + sgn * dist * (std::isfinite(ry) ? ry : 1.0f) + float(int(g.P(5)) - 2) * 0.25f);
        (A.hasMat ? A.mpos : A.pos)[0] = nx;
        (A.hasMat ? A.mpos : A.pos)[1] = ny;
        A.speed[0] = g.C(10) ? Bits(0.0f) : Cont(g, -0.4f, 0.4f);
        A.speed[1] = g.C(10) ? Bits(0.0f) : Cont(g, -0.4f, 0.4f);
        if (g.C(50)) {   // car1 casi opuesto a car2
            for (int i = 0; i < 3; ++i) A.fwd[i] = Bits(-F(B.fwd[i]) * (g.C(80) ? 1.0f : 0.6f));
        }
        if (g.C(10)) { static const u32 modes[]{0, 1, 2, 4, 5}; FillMatrix(g, B, grid, modes[g.P(5)]); }
        // tiempos devueltos por las pruebas de colision, relativos a 1 / arg4
        const float rcp = 1.0f / arg4;
        const float cands[]{-1.0f, -0.0f, 0.0f, 0.001f, rcp * 0.5f, rcp, NextUp(rcp), NextDown(rcp), 2.0f * rcp, 3.0f * rcp, NextUp(3.0f * rcp), NextDown(3.0f * rcp), 4.0f * rcp, 0.45f, 0.5f,
                            NextUp(0.5f), NextDown(0.5f), 1.4999f, 1.5f, NextDown(1.5f), 1.6f, 5.0f, 0.2f, 0.2f + 3.0f * rcp, 0.26f, 0.7f, 1.0f, 0.1f, 0.3f, 0.4f};
        constexpr u32 nc = sizeof(cands) / sizeof(cands[0]);
        auto pick = [&] {
            const u32 r = g.P(100);
            if (r < 4) return kQNaN;
            if (r < 8) return kInf;
            if (r < 12) return kNegInf;
            return Bits(cands[g.P(nc)]);
        };
        s.tcF = pick();
        s.tcB = g.C(35) ? s.tcF : pick();
        // temporizadores: now - timeSw alrededor de 15000
        const u32 diffs[]{14999u, 15000u, 15001u, 0u, 1u, 100000u, 0xFFFFFFFFu, 0x80000000u, 15000u + 1u};
        A.timeSw = s.now - diffs[g.P(9)];
        B.timeSw = s.now - diffs[g.P(9)];
        if (g.C(30)) { A.timeSw = s.now - 20000u; B.timeSw = s.now - 20000u; }
        s.player = g.C(40) ? 0u : 1u + g.P(kObjs);
        if (g.C(25)) s.player = s.ia + 1;
        B.style = g.P(8); B.ctrl = g.P(256);
        if (fam >= 4 && fam <= 6) SpecialOdd(g, s, fam);
        break;
    }
    case kWoc: {
        SetHasMat(A, 1); SetHasMat(B, 1);   // el EXE usa las dos matrices sin comprobar nulo
        static const u32 missions[]{3, 16, 0x35, 0x37, 0, 1, 0x1D, 0x1E, 0x20, 0x1F, 0x1B, 0x1C, 0x10, 0x36, 0x7F, 0xFF};
        B.mission = g.C(60) ? missions[g.P(4)] : missions[g.P(16)];
        A.mission = g.C(45) ? missions[6 + g.P(6)] : missions[g.P(16)];
        const u32 tsel = g.P(8);
        B.target = tsel < 3 ? s.ia + 1 : (tsel < 5 ? 0u : (tsel < 7 ? 1u + g.P(kObjs) : 5u));
        A.target = g.C(40) ? s.ib + 1 : (g.C(50) ? 0u : 1u + g.P(kObjs));
        A.b36 = g.C(50) ? 2u : (g.C(60) ? 3u : g.P(8));
        A.b42B = g.C(50) ? 8u : g.P(256);
        A.d46C = g.C(50) ? 0x100u : g.R();
        A.d58C = g.C(40) ? B.target : g.P(6);
        s.player = g.C(35) ? s.ia + 1 : (g.C(30) ? 0u : 1u + g.P(kObjs));
        // posiciones relativas con signo variable y empates en la rejilla
        s.out3 = g.C(10) ? Bits(0.0f) : Cont(g, 0.0f, 6.2831853f);
        s.out4 = g.C(10) ? Bits(0.0f) : Cont(g, 0.0f, 6.2831853f);
        if (g.C(10)) s.out3 = Cont(g, -7.0f, 0.2f);
        if (g.C(10)) s.out4 = Cont(g, 6.0f, 9.0f);
        s.sweepMask = g.C(15) ? 0xFFFFFFFFu : (g.C(15) ? 0u : g.R());
        B.speed[0] = g.C(20) ? Bits(0.0f) : Cont(g, -0.6f, 0.6f);
        B.speed[1] = g.C(20) ? Bits(0.0f) : Cont(g, -0.6f, 0.6f);
        if (fam >= 4 && fam <= 6) SpecialOdd(g, s, fam);
        break;
    }
    default: {   // kWfo
        // modelo del primer objeto: uno de los cinco especiales o propio (con o sin SwaysInWind)
        const u32 m = g.P(100);
        A.modelSel = m < 45 ? m / 9 : 5u;
        A.sways = g.C(60) ? (g.C(50) ? 1u : 0xA5A5A501u) : (g.C(50) ? 0u : 0xA5A5A500u);
        SetHasMat(A, g.C(30) ? 0u : 1u);
        // el angulo (atan) en distintos rangos: normal, negativo, mayor que 2 pi, multiplos de 2 pi, nulo
        const float pi2 = 6.2831853f;
        const u32 ak = g.P(10);
        float atanV = ak < 3 ? F(Cont(g, 0.0f, pi2)) : (ak < 5 ? F(Cont(g, -pi2, 0.0f)) : (ak < 7 ? F(Cont(g, pi2, 3.0f * pi2)) : (ak == 7 ? -pi2 * float(1 + g.P(20)) : (ak == 8 ? pi2 * float(1 + g.P(20)) : 0.0f))));
        s.atanBits = Bits(atanV);
        if (g.C(5)) s.atanBits = kQNaN;
        s.out3 = g.C(15) ? Cont(g, -50.0f, 50.0f) : Cont(g, -4.0f, 4.0f);
        s.out4 = g.C(15) ? Cont(g, -50.0f, 50.0f) : Cont(g, -4.0f, 4.0f);
        if (g.C(10)) s.out3 = s.atanBits;
        if (g.C(10)) s.out4 = s.atanBits;
        // el objeto cerca del vehiculo para tener distancias razonables (el arco depende de halfWidth / distance)
        if (g.C(80)) {
            const u32 slot = B.hasMat;
            const float ox = F(A.hasMat ? A.mpos[0] : A.pos[0]), oy = F(A.hasMat ? A.mpos[1] : A.pos[1]);
            (slot ? B.mpos : B.pos)[0] = Bits(ox + float(int(g.P(9)) - 4));
            (slot ? B.mpos : B.pos)[1] = Bits(oy + float(int(g.P(9)) - 4) + 0.5f);   // nunca a distancia cero: margen infinito = bucle de angulos infinito
        }
        if (fam >= 4 && fam <= 6) SpecialOdd(g, s, fam);
        break;
    }
    }
    if (fn == kWfo) {
        // los bucles de normalizacion de angulos no terminan (ni en el EXE ni en la DLL) con margen infinito o enorme: infinitos y enormes pasan a NaN, que no entra en los bucles
        auto tame = [](u32& v) { const float x = Flt(v); if (std::isinf(x) || std::fabs(x) > 1.0e6f) v = kQNaN; };
        for (Obj& ob : s.o) {
            for (int i = 0; i < 3; ++i) { tame(ob.pos[i]); tame(ob.mpos[i]); tame(ob.right[i]); tame(ob.fwd[i]); }
            for (int i = 0; i < 6; ++i) tame(ob.box[i]);
        }
    }
    return s;
}

inline std::string U(u32 v) { return std::to_string(v); }
inline std::string Arr(const u32* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string JsonObj(const Obj& v) {
    return "{\"mat\":" + U(v.hasMat) + ",\"msel\":" + U(v.modelSel) + ",\"pos\":" + Arr(v.pos, 3) + ",\"mpos\":" + Arr(v.mpos, 3) + ",\"right\":" + Arr(v.right, 3) + ",\"fwd\":" + Arr(v.fwd, 3) +
           ",\"up\":" + Arr(v.up, 3) + ",\"speed\":" + Arr(v.speed, 3) + ",\"b36\":" + U(v.b36) + ",\"tsw\":" + U(v.timeSw) + ",\"ap\":[" + U(v.style) + "," + U(v.mission) + "," + U(v.ctrl) + "," + U(v.tempAct) + "," +
           U(v.tempTime) + "],\"target\":" + U(v.target) + ",\"raw\":[" + U(v.b42B) + "," + U(v.d46C) + "," + U(v.d58C) + "],\"box\":" + Arr(v.box, 6) + ",\"sways\":" + U(v.sways) + "}";
}
inline std::string JsonSpec(const Spec& s) {
    std::string o = "{\"fn\":" + U(s.fn) + ",\"fam\":" + U(s.family) + ",\"regs\":" + Arr(s.regs, 4) + ",\"ia\":" + U(s.ia) + ",\"ib\":" + U(s.ib) + ",\"a\":[" + U(s.a3) + "," + U(s.a4) + "],\"vecA\":" + Arr(s.vecA, 3) +
                    ",\"vecB\":" + Arr(s.vecB, 3) + ",\"out\":[" + U(s.out3) + "," + U(s.out4) + "],\"tc\":[" + U(s.tcF) + "," + U(s.tcB) + "],\"now\":" + U(s.now) + ",\"player\":" + U(s.player) + ",\"sweep\":" + U(s.sweepMask) +
                    ",\"atan\":" + U(s.atanBits) + ",\"seed\":" + U(s.seed) + ",\"obj\":[";
    for (u32 i = 0; i < kObjs; ++i) { if (i) o += ','; o += JsonObj(s.o[i]); }
    return o + "]}";
}
}  // namespace CwAFix

#pragma once
// Fixtures del banco CarWeave75 (lote 75): CCarCtrl::FindAngleToWeaveThroughTraffic (EXE 0x4325C0), 18432 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::FindAngleToWeaveThroughTraffic  0x4325C0  cdecl, `ret` (CVehicle*, CPhysical*, float arg3, float arg4, float arg5) -> float en ST0   (hook CCarCtrl / FindAngleToWeaveThroughTraffic, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n / 1024 (18 familias de 1024 casos).
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace Cw75Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kFamilies = 18;
constexpr unsigned kCases = kFamilies * 1024;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x3D0, kMatWin = 0x48, kPhysWin = 0x20;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kMatArena = Arena(kMatWin), kPhysArena = Arena(kPhysWin);

constexpr u32 kScanCodeAddr = 0xB7CD78;        // CWorld::ms_nCurrentScanCode (uint16)
constexpr u32 kRepeatSectorsAddr = 0xB992B8;   // CWorld::ms_aRepeatSectors[16][16] (12 bytes cada una)

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 75075u); }
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

// ------------------------------------------------------------------------------------------------ JSON minimo
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& Raw(const char* k, const std::string& v) { Key(k); s += v; return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ valores notables
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFA00000u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00001u, 0xFFBFFFFFu};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
inline constexpr u32 kPiBits = 0x40490FDBu, kTwoPiBits = 0x40C90FDBu, kNegPiBits = 0xC0490FDBu, kHalfPiBits = 0x3FC90FDBu, kTolBits = 0x3DA3D70Au;
inline constexpr u32 kInitBits = 0xC61C3F9Au;   // valor inicial de la comparacion del bucle externo
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// float uniforme en [lo, hi] (resolucion de 2^-20 del intervalo)
inline u32 Uni(u32 c, u32 k, float lo, float hi) { return B(lo + (hi - lo) * (float(Rand(c, k) & 0xFFFFFu) / 1048575.0f)); }
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}

// ------------------------------------------------------------------------------------------------ el caso
constexpr unsigned kMaxWrites = 6;
// En la llamada `call` (indice global desde 0, contando las tres funciones de barrido) el falso escribe: mode bit0 = *arg8 (slot50), bit1 = *arg9 (slot40)
struct Wr {u32 call{}, mode{}, v50{}, v40{};};
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 hasMat{};
    u32 mpos[2]{}, ppos[2]{};   // posicion de la matriz (+0x30, +0x34) y compacta (+4, +8)
    u32 vx{}, vy{};             // velocidad (+0x44, +0x48)
    u32 scale{};                // arg5
    u32 arg3{}, arg4{};         // rumbo al objetivo y rumbo del vehiculo
    u32 style{};              // byte +0x3B9
    u32 scan{};                 // CWorld::ms_nCurrentScanCode inicial
    u32 phys{};                 // 0 = nulo, 1 = arena, 2 = valor crudo
    u32 nWrites{};
    Wr w[kMaxWrites]{};
};

// Un valor moderado en [-lim, lim]
inline u32 Mod(u32 c, u32 k, float lim = 8.0f) { return Uni(c, k, -lim, lim); }

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n / 1024;
    const u32 m = n % 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.hasMat = Rand(n, 29) % 5 != 0;
    for (int i = 0; i < 2; ++i) { c.mpos[i] = Uni(n, 40 + i, -2900.0f, 2900.0f); c.ppos[i] = Uni(n, 44 + i, -2900.0f, 2900.0f); }
    c.vx = Uni(n, 48, -3.0f, 3.0f); c.vy = Uni(n, 49, -3.0f, 3.0f);
    c.scale = Uni(n, 50, 0.1f, 8.0f);
    c.arg3 = Uni(n, 51, -7.0f, 7.0f); c.arg4 = Uni(n, 52, -7.0f, 7.0f);
    c.style = Rand(n, 53) % 4 == 0 ? 6u : Rand(n, 54) & 0xFFu;
    c.scan = Rand(n, 56) % 3 == 0 ? Rand(n, 57) & 0xFFFFu : Rand(n, 58) % 100;
    c.phys = Rand(n, 59) % 5 == 0 ? 0u : Rand(n, 60) % 7 == 0 ? 2u : 1u;
    c.nWrites = 0;
    auto write = [&](u32 call, u32 mode, u32 v50, u32 v40) { if (c.nWrites < kMaxWrites) c.w[c.nWrites++] = {call, mode, v50, v40}; };
    // `count` escrituras en llamadas aleatorias con valores moderados (la de mayor indice siempre es completa, para que el valor final sea moderado)
    auto randomWrites = [&](u32 maxCall, u32 count) {
        for (u32 i = 0; i < count; ++i) write(Rand(n, 200 + i) % maxCall, i + 1 == count ? 3u : 1 + Rand(n, 210 + i) % 3, Mod(n, 220 + i), Mod(n, 230 + i));
        u32 best = 0;
        for (u32 i = 1; i < c.nWrites; ++i) if (c.w[i].call >= c.w[best].call) best = i;
        c.w[best].mode = 3;
    };
    switch (c.family) {
    case 0:   // sin escrituras: un barrido, los dos valores de salida quedan en arg3
        break;
    case 1:   // una escritura de ambos en la primera llamada: dos barridos
        write(0, 3, Mod(n, 220), Mod(n, 230));
        break;
    case 2:   // escrituras repartidas (varios barridos en rangos con varias celdas)
        c.scale = Uni(n, 50, 1.0f, 12.0f);
        randomWrites(40, 1 + Rand(n, 61) % 5);
        break;
    case 3: { // eleccion final: valores de los barridos fijados para recorrer las ramas (A, |a-b| < 0.08, a < b, a >= b)
        c.arg3 = Uni(n, 51, -3.0f, 3.0f); c.arg4 = Uni(n, 52, -3.0f, 3.0f);
        const u32 s50 = Uni(n, 220, -7.0f, 7.0f);
        u32 s40 = Uni(n, 230, -7.0f, 7.0f);
        if (m % 4 == 0) s40 = B(F(s50) + F(Uni(n, 231, -0.12f, 0.12f)));   // cerca: |a-b| alrededor de 0.08
        else if (m % 4 == 1) s40 = s50;
        write(Rand(n, 232) % 3, 3, s50, s40);
        break;
    }
    case 4:   // rumbos y barridos grandes: bucles de normalizacion de varias vueltas
        c.arg3 = Mod(n, 51, 100.0f); c.arg4 = Mod(n, 52, 100.0f);
        write(Rand(n, 232) % 3, 3, Mod(n, 220, 100.0f), Mod(n, 230, 100.0f));
        break;
    case 5: { // fronteras: +-pi, pi/2, tolerancia 0.08 y +-2 ulp
        static constexpr std::array<u32, 6> edges{kPiBits, kNegPiBits, kHalfPiBits, kTolBits, 0u, 0x80000000u};
        auto jig = [&](u32 v, u32 k) { const u32 r = Rand(n, k) % 5; const u32 j = r == 0 ? v : r == 1 ? v + 1 : r == 2 ? v - 1 : r == 3 ? v + 2 : v - 2; return (j & 0x7F800000u) == 0x7F800000u ? v : j; };   // nunca NaN ni infinito
        c.arg3 = Rand(n, 92) % 2 ? jig(Pick(edges, n, 90), 93) : Mod(n, 51);
        c.arg4 = Rand(n, 94) % 2 ? jig(Pick(edges, n, 91), 95) : Mod(n, 52);
        if (Rand(n, 96) % 3 == 0) c.arg4 = B(F(c.arg3) + F(jig(Pick(edges, n, 97), 98)));
        write(Rand(n, 232) % 3, 3, B(F(c.arg3) + F(jig(Pick(edges, n, 99), 100))), B(F(c.arg3) + F(jig(Pick(edges, n, 101), 102))));
        break;
    }
    case 6: { // cotas del sector: posiciones en los bordes (pos * 0.02 + 60 cerca de un entero, 0 y 119) y escalas grandes (se saturan)
        const u32 mode = m % 8;
        for (int i = 0; i < 2; ++i) {
            float p;
            switch (mode) {
            case 0: p = -3000.0f + float(Rand(n, 70 + i) % 6000); break;
            case 1: p = float(int(Rand(n, 70 + i) % 120) - 60) * 50.0f; break;      // justo en un borde de sector
            case 2: p = -3010.0f + float(Rand(n, 70 + i) % 40); break;               // fuera por abajo
            case 3: p = 2970.0f + float(Rand(n, 70 + i) % 100); break;               // cerca/pasado el borde alto
            case 4: p = float(int(Rand(n, 70 + i) % 120) - 60) * 50.0f + (Rand(n, 74 + i) % 2 ? 1e-3f : -1e-3f); break;
            default: p = 3200.0f * (float(Rand(n, 72 + i) % 2000) / 1000.0f - 1.0f); break;
            }
            c.mpos[i] = B(p);
            c.ppos[i] = B(p);
        }
        c.hasMat = Rand(n, 76) % 2;
        c.scale = Rand(n, 77) % 3 == 0 ? Uni(n, 78, 8.0f, 40.0f) : Uni(n, 78, 0.05f, 3.0f);
        write(Rand(n, 232) % 8, 3, Mod(n, 220), Mod(n, 230));
        break;
    }
    case 7:   // rangos enormes: casi toda la rejilla de 120 x 120 celdas (pocas por el coste de llamadas)
        c.mpos[0] = c.ppos[0] = Uni(n, 40, -300.0f, 300.0f); c.mpos[1] = c.ppos[1] = Uni(n, 41, -300.0f, 300.0f);
        if (m % 32 == 0) { c.scale = Uni(n, 50, 150.0f, 400.0f); write(Rand(n, 232) % 5000, 3, Mod(n, 220), Mod(n, 230)); write(Rand(n, 233) % 20000, 3, Mod(n, 221), Mod(n, 231)); }
        else { c.scale = Uni(n, 50, 12.0f, 40.0f); write(Rand(n, 232) % 300, 3, Mod(n, 220), Mod(n, 230)); }
        break;
    case 8: { // velocidad: cero, grande, especial (NaN, infinito, desnormal), componentes distintas
        static constexpr std::array<u32, 8> pick{0u, 0x80000000u, 1u, 0x3F800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0x40000000u};
        switch (m % 6) {
        case 0: c.vx = Pick(kTiny, n, 80); c.vy = Pick(kTiny, n, 81); break;
        case 1: c.vx = Pick(kNan, n, 80); c.vy = Uni(n, 82, -3.0f, 3.0f); break;
        case 2: c.vx = Uni(n, 82, -3.0f, 3.0f); c.vy = Pick(kInf, n, 81); break;
        case 3: c.vx = Uni(n, 83, -1.0f, 1.0f); c.vy = Uni(n, 84, -1.0f, 1.0f); break;
        case 4: c.vx = Pick(pick, n, 85); c.vy = Pick(pick, n, 86); break;
        default: c.vx = Uni(n, 87, -0.7f, 0.7f); c.vy = Uni(n, 88, -0.7f, 0.7f); break;   // |v| cerca de 0.4: t cerca de 2.0
        }
        write(Rand(n, 232) % 4, 3, Mod(n, 220), Mod(n, 230));
        break;
    }
    case 9: { // escala especial (NaN no canonicos, infinito, desnormal, +-0, negativa, enorme): los limites salen NaN/inf y las celdas se saturan a 0
        static constexpr std::array<u32, 6> sc{0u, 0x80000000u, 0xBF800000u, 0xC0000000u, 0x7F7FFFFFu, 0x3F800000u};
        const u32 r = Rand(n, 80) % 5;
        c.scale = r == 0 ? Pick(kNan, n, 81) : r == 1 ? Pick(kInf, n, 82) : r == 2 ? Pick(kTiny, n, 83) : r == 3 ? Pick(sc, n, 84) : Uni(n, 85, -3.0f, 0.0f);
        write(Rand(n, 232) % 2, 3, Mod(n, 220), Mod(n, 230));
        break;
    }
    case 10: { // posicion especial (NaN, infinito, desnormal, ceros): NaN de dos operandos (posicion y radio)
        for (int i = 0; i < 2; ++i) {
            const u32 r = Rand(n, 80 + i) % 4;
            const u32 v = r == 0 ? Pick(kNan, n, 82 + i) : r == 1 ? Pick(kInf, n, 84 + i) : r == 2 ? Pick(kTiny, n, 86 + i) : Pick(kBig, n, 88 + i);
            c.mpos[i] = v;
            c.ppos[i] = Rand(n, 90 + i) % 2 ? v : c.ppos[i];
        }
        if (Rand(n, 92) % 3 == 0) c.scale = Pick(kNan, n, 93);
        write(Rand(n, 232) % 2, 3, Mod(n, 220), Mod(n, 230));
        break;
    }
    case 11:  // sin matriz: posicion compacta (+4)
        c.hasMat = 0;
        write(Rand(n, 232) % 4, 3, Mod(n, 220), Mod(n, 230));
        if (Rand(n, 233) % 2) write(Rand(n, 234) % 8, 3, Mod(n, 221), Mod(n, 231));
        break;
    case 12:  // codigo de escaneo en los bordes (0xFFFF llama a ClearScanCodes; 0xFFFE; 0)
        c.scan = Pick(std::array<u32, 8>{0xFFFFu, 0xFFFEu, 0xFFFFu, 0u, 1u, 0x7FFFu, 0x8000u, 0xFFFDu}, n, 90);
        write(Rand(n, 232) % 4, 3, Mod(n, 220), Mod(n, 230));
        break;
    case 13:  // estilo 6 (sin llamada a los peatones) y los demas valores del byte
        c.style = m % 2 ? 6u : Rand(n, 90) & 0xFFu;
        randomWrites(30, 1 + Rand(n, 61) % 4);
        break;
    case 14:  // arg3 = -10000.4 exacto (0xC61C3F9A): el bucle externo no entra; arg3 de miles con bucles largos
        c.arg3 = m % 2 ? kInitBits : B(-9000.0f - float(Rand(n, 90) % 3000));
        c.arg4 = Rand(n, 91) % 2 ? Mod(n, 52) : B(F(c.arg3) + F(Mod(n, 92, 3.0f)));
        if (m % 4 == 3) write(0, 3, Mod(n, 220, 20000.0f), Mod(n, 230, 20000.0f));
        break;
    case 15: { // rumbos NaN/desnormales/ceros con signo (con escritura de ambos en la primera llamada y celdas siempre)
        const u32 r = Rand(n, 90) % 4;
        const u32 sp = r == 0 ? Pick(kNan, n, 91) : r == 1 ? Pick(kTiny, n, 92) : r == 2 ? 0u : 0x80000000u;
        if (Rand(n, 93) % 2) c.arg3 = sp; else c.arg4 = sp;
        if (Rand(n, 94) % 5 == 0) { c.arg3 = Pick(kNan, n, 95); c.arg4 = Pick(kNan, n, 96); }
        c.scale = Uni(n, 50, 0.5f, 3.0f);
        write(0, 3, Mod(n, 220), Mod(n, 230));
        break;
    }
    case 16: { // valores de barrido especiales en el medio (NaN, infinito, desnormal) que se sobrescriben antes de salir
        const u32 sp1 = Rand(n, 90) % 3 == 0 ? Pick(kNan, n, 91) : Rand(n, 92) % 2 ? Pick(kTiny, n, 93) : Pick(kInf, n, 94);
        const u32 sp2 = Rand(n, 95) % 3 == 0 ? Pick(kNan, n, 96) : Rand(n, 97) % 2 ? Pick(kTiny, n, 98) : Pick(kBig, n, 99);
        c.scale = Uni(n, 50, 1.0f, 6.0f);
        write(0, 3, sp1, sp2);
        write(1 + Rand(n, 232) % 2, 1 + Rand(n, 233) % 3, Mod(n, 220), Mod(n, 230));
        write(3 + Rand(n, 234) % 12, 3, Mod(n, 221), Mod(n, 231));
        break;
    }
    default:  // 17: mezcla (rangos y rumbos moderados, escrituras variadas)
        c.scale = Uni(n, 50, 0.3f, 10.0f);
        c.arg3 = Mod(n, 51, 20.0f); c.arg4 = Mod(n, 52, 20.0f);
        randomWrites(60, 1 + Rand(n, 61) % 5);
        if (Rand(n, 62) % 6 == 0) c.vx = Pick(kTiny, n, 63);
        if (Rand(n, 64) % 8 == 0) c.phys = 0;
        break;
    }
    // Seguridad contra bucles sin fin (tambien en el EXE): los dos valores de salida deben quedar finitos al salir (si no, la comparacion NaN != NaN del bucle externo no termina),
    // y los rumbos no pueden ser infinitos (los bucles de normalizacion no terminan). Un NaN en arg3 solo se admite con una escritura final de ambos en la primera llamada (siempre hay celdas).
    auto finite = [](u32 b) { return (b & 0x7F800000u) != 0x7F800000u; };
    if (!finite(c.arg3) && (c.arg3 & 0x7FFFFFu) == 0) c.arg3 = 0;
    if (!finite(c.arg4) && (c.arg4 & 0x7FFFFFu) == 0) c.arg4 = 0;
    for (int slot = 0; slot < 2; ++slot) {
        int last = -1;
        for (u32 i = 0; i < c.nWrites; ++i)
            if ((c.w[i].mode >> slot) & 1u && (last < 0 || c.w[i].call >= c.w[last].call)) last = int(i);
        if (last >= 0) {
            u32& v = slot == 0 ? c.w[last].v50 : c.w[last].v40;
            if (!finite(v)) v = Mod(n, 240 + slot);
        } else if (!finite(c.arg3)) {
            write(0, 3, Mod(n, 242), Mod(n, 243));
        }
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack).U("has_mat", c.hasMat);
    j.U("mpos_x", c.mpos[0]).U("mpos_y", c.mpos[1]).U("ppos_x", c.ppos[0]).U("ppos_y", c.ppos[1]).U("vx", c.vx).U("vy", c.vy).U("scale", c.scale);
    j.U("arg3", c.arg3).U("arg4", c.arg4).U("style", c.style).U("scan", c.scan).U("phys", c.phys).U("n_writes", c.nWrites);
    std::string w = "[";
    for (u32 i = 0; i < c.nWrites; ++i) {
        if (i) w += ',';
        w += '[' + std::to_string(c.w[i].call) + ',' + std::to_string(c.w[i].mode) + ',' + std::to_string(c.w[i].v50) + ',' + std::to_string(c.w[i].v40) + ']';
    }
    j.Raw("writes", w + "]");
    return j.End();
}
}

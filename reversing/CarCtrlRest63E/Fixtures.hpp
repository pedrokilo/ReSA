#pragma once
// Fixtures del banco CarCtrlRest63E (lote 63, CCarCtrl parte C, tanda 2): 4096 casos por funcion. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / Cases.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::SetUpDriverAndPassengersForVehicle        0x4217C0  cdecl (CVehicle*, int, int, bool, bool, int), void
//   1 CCarCtrl::SetCoordsOfScriptCar                      0x4342A0  cdecl (CVehicle*, float, float, float, uint8, uint8), void
//   2 CCarCtrl::ScanForPedDanger                          0x42CE40  cdecl (CVehicle*), void
//   3 CCarCtrl::FindMaximumSpeedForThisCarInTraffic       0x434400  cdecl (CVehicle*), float en ST0
//
// Todos los floats viajan como patrones de 32 bits sin signo. Politica de especiales: a lo sumo UN campo NaN con carga util propia por caso.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace Cc63eFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4096;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

// ------------------------------------------------------------------------------------------------ JSON minimo (mismas filas `input` en el juego y en el espejo)
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

// ------------------------------------------------------------------------------------------------ floats
constexpr u32 NanBits(u32 slot, bool signaling) {
    const u32 sign = (slot & 1u) << 31;
    return signaling ? (sign | 0x7F800000u | (0x2000u + slot * 0x41u)) : (sign | 0x7FC00000u | (0x100u + slot * 0x37u));
}
inline constexpr std::array<u32, 20> kFloatEdges{
    0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3DCCCCCDu, 0xBDCCCCCDu, 0x3D4CCCCDu, 0x00000001u, 0x80000001u, 0x007FFFFFu,
    0x807FFFFFu, 0x00800000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x3F000000u, 0x40000000u, 0x3E000000u, 0x45000000u, 0x00400000u, 0x80400000u};
inline u32 OrdinaryFloat(u32 n, u32 k, int minExp, int maxExp) {   // exponentes sesgados (127 = 1.0)
    const u32 sign = Rand(n, k) & 1u;
    const u32 exp = u32(minExp) + Rand(n, k + 1) % u32(maxExp - minExp + 1);
    return (sign << 31) | (exp << 23) | (Rand(n, k + 2) & 0x7FFFFFu);
}
inline u32 TinyFloat(u32 n, u32 k) {   // denormal o casi
    const u32 sign = Rand(n, k) & 1u;
    const u32 exp = Rand(n, k + 1) % 3 == 0 ? Rand(n, k + 3) % 6u : 0u;
    return (sign << 31) | (exp << 23) | (Rand(n, k + 2) & 0x7FFFFFu);
}
inline u32 HugeFloat(u32 n, u32 k) {
    const u32 sign = Rand(n, k) & 1u;
    return (sign << 31) | ((200u + Rand(n, k + 1) % 55u) << 23) | (Rand(n, k + 2) & 0x7FFFFFu);   // hasta 0x7F7FFFFF
}
inline u32 InfBits(u32 n, u32 k) { return Rand(n, k) & 1u ? 0xFF800000u : 0x7F800000u; }
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
inline bool IsInfBits(u32 b) { return (b & 0x7FFFFFFFu) == 0x7F800000u; }
inline float F(u32 b) { float f; std::memcpy(&f, &b, 4); return f; }
inline u32 B(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
// Valor de relleno para los registros de entrada (EAX/ECX/EDX)
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Resultado de bool de un colaborador: falso = 0 con basura en los bits altos, o AL != 0 con basura en los bits altos
inline u32 BoolRet(u32 n, u32 k, bool wantTrue) {
    static constexpr u32 als[]{1u, 2u, 0x80u, 0xFFu, 0x7Fu, 0x81u};
    if (!wantTrue) return Rand(n, k + 1) % 2 ? 0u : (Rand(n, k + 2) & 0xFFFFFF00u);
    return (Rand(n, k + 3) % 2 ? 0u : (Rand(n, k + 4) & 0xFFFFFF00u)) | als[Rand(n, k + 5) % std::size(als)];
}



// ------------------------------------------------------------------------------------------------ constantes
constexpr u32 kModels = 600;                      // modelos 0..599 con byte de carga (+0 de cada 20 bytes de 0x8E4CD0)
constexpr u32 kGroups = 10, kGroupWords = 23;     // 10 grupos de 23 words desde 0xC0F074
constexpr u32 kRandSeq = 96;
constexpr u32 kCatGuard = 4;                      // categoria de salida: 1 palabra con guardas de 4 palabras a cada lado
constexpr u32 kCatArenaWords = kCatGuard + 1 + kCatGuard;   // 4 + 1 + 4 = 9 palabras

// ================================================================================================ 0: ChooseModel
struct E0 {
    u32 family{}, dens[4]{}, flagA{}, flagB{}, dealerModel{}, forced{}, dontGang{}, dontCops{}, strength[11]{};
    u32 table[kGroups][kGroupWords]{}, loaded[kModels]{}, rnd[kRandSeq]{};
    u32 gangWarRet{}, policeRet{}, pickRet{}, gangCarRet{}, rangeRet{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline u32 SmallModel(u32 n, u32 k) { return Rand(n, k) % kModels; }
inline E0 MakeE0(u32 n) {
    E0 c; const u32 f = n % 16; c.family = f;
    auto ord = [&](u32 k, int lo, int hi) { return OrdinaryFloat(n, k, u32(lo), u32(hi)) & 0x7FFFFFFFu; };
    for (u32 i = 0; i < 4; ++i) c.dens[i] = Rand(n, 1 + i) % 7 == 0 ? 0u : B(float(Rand(n, 10 + i) % 40u) + float(Rand(n, 20 + i) % 8u) * 0.125f);
    switch (f) {
    case 1: for (u32 i = 0; i < 4; ++i) c.dens[i] = 0; break;                                        // total 0
    case 2: c.dens[Rand(n, 30) % 4] = NanBits(Rand(n, 31) % 6, Rand(n, 32) % 2 == 0); break;
    case 3: c.dens[Rand(n, 30) % 4] = InfBits(n, 31); break;
    case 4: c.dens[Rand(n, 30) % 4] = B(-float(Rand(n, 31) % 30u) - 1.0f); break;                      // negativos
    case 5: for (u32 i = 0; i < 4; ++i) c.dens[i] = ord(40 + i, 100, 135); break;                    // pequenos (total < 1 -> truncado a 0)
    case 6: for (u32 i = 0; i < 4; ++i) c.dens[i] = ord(40 + i, 140, 160); break;                    // enormes
    case 7: for (u32 i = 0; i < 4; ++i) c.dens[i] = B(float(Rand(n, 40 + i) % 4u) * 0.25f); break;   // fracciones: el total se trunca
    case 8: c.dens[0] = B(1.0f); c.dens[1] = B(2.0f); c.dens[2] = B(3.0f); c.dens[3] = B(4.0f); break;
    default: break;
    }
    c.flagA = Rand(n, 50) % 3 == 0 ? (Rand(n, 51) & 0xFFu) : 0u;
    c.flagB = Rand(n, 52) % 4 == 0 ? (Rand(n, 53) % 2 ? 1u : Rand(n, 53) & 0xFFu) : 0u;
    c.dealerModel = Rand(n, 54) % 6 == 0 ? (Rand(n, 55) & 0xFFFFu) % kModels : SmallModel(n, 56);
    c.forced = Rand(n, 57) % 3 == 0 ? 0xFFFFFFFFu : (Rand(n, 58) % 4 == 0 ? Rand(n, 59) : SmallModel(n, 60));
    c.dontGang = Rand(n, 61) % 6 == 0 ? (Rand(n, 62) & 0xFFu) | 1u : 0u;
    c.dontCops = Rand(n, 63) % 6 == 0 ? (Rand(n, 64) & 0xFFu) | 1u : 0u;
    for (u32 i = 0; i < 11; ++i) c.strength[i] = Rand(n, 70 + i) % 3 == 0 ? 0u : Rand(n, 90 + i) % 40u;
    if (f == 9) for (u32 i = 0; i < 10; ++i) c.strength[i] = 0;                                       // suma 0 -> -1
    if (f == 10) { for (u32 i = 0; i < 10; ++i) c.strength[i] = 0; c.strength[Rand(n, 100) % 10] = 1 + Rand(n, 101) % 255u; }
    for (u32 g = 0; g < kGroups; ++g) {
        for (u32 w = 0; w < kGroupWords; ++w) {
            const bool sparse = f == 11;
            c.table[g][w] = (sparse ? Rand(n, 200 + g * 32 + w) % 8 != 0 : Rand(n, 200 + g * 32 + w) % 4 == 0) ? 2000u : SmallModel(n, 600 + g * 32 + w);
        }
        c.table[g][Rand(n, 900 + g) % 12] = SmallModel(n, 910 + g);    // el rand del EXE solo alcanza los indices 0..11 (rand < 32768): siempre hay una entrada valida alcanzable
    }
    const u32 loadPct = f == 12 ? 0u : f == 13 ? 100u : 40u;
    for (u32 m = 0; m < kModels; ++m) {
        if (Rand(n, 2000 + m) % 100 < loadPct) c.loaded[m] = 1;
        else { u32 v = Rand(n, 3000 + m) & 0xFFu; if (v == 1) v = 0; c.loaded[m] = v; }
    }
    // sorteos del rand del EXE (0..32767): el 1.o (si flagA) es el del control del 1%; el siguiente es el que reparte la categoria; despues los de las rutas de pandillas
    for (u32 i = 0; i < kRandSeq; ++i) c.rnd[i] = Rand(n, 4000 + i) % 32768u;
    for (u32 k = 0; k < 12; ++k) c.rnd[64 + k] = k * 2850u + 5u;   // cubren todos los indices 0..11 del sorteo de modelo de pandilla
    // fuerza la categoria: calcula los acumulados con doble y elige el rand que reparte para caer en la categoria deseada (con algunos bordes)
    {
        const double d0 = F(c.dens[0]), d1 = F(c.dens[1]), d2 = F(c.dens[2]), d3 = F(c.dens[3]);
        const double sum = ((d0 + d1) + d2) + d3;
        const double tt = std::floor(sum);
        const u32 cat = Rand(n, 5000) % 5;   // 0 dealers, 1 pandillas, 2 policia, 3 otros, 4 borde aleatorio
        double target = 0.5;
        if (tt > 0.0) {
            const double q3 = d3 / tt, q2 = (d2 + d3) / tt, q1 = (d1 + d2 + d3) / tt;
            switch (cat) {
            case 0: target = q3 * 0.5; break;
            case 1: target = (q3 + q2) * 0.5; break;
            case 2: target = (q2 + q1) * 0.5; break;
            case 3: target = (q1 + 1.0) * 0.5; break;
            default: { static constexpr double e[]{0.0, 1e-6, -1e-6}; target = (Rand(n, 5001) % 2 ? q3 : Rand(n, 5001) % 3 ? q2 : q1) + e[Rand(n, 5002) % 3]; break; }
            }
        }
        long r2 = long(target * 32767.0);
        if (r2 < 0) r2 = 0;
        if (r2 > 32767) r2 = 32767;
        const u32 at = (c.flagA != 0) ? 1u : 0u;
        c.rnd[at] = u32(r2);
        if (c.flagA != 0) c.rnd[0] = Rand(n, 5003) % 3 == 0 ? Rand(n, 5004) % 655u : Rand(n, 5004) % 32768u;   // < 656 pasa el control
    }
    c.gangWarRet = BoolRet(n, 6000, Rand(n, 6003) % 3 == 0);
    static constexpr i32 modelRets[]{-1, -2, 0, 1, 399, 0x7FFF, -32768, 12345};
    c.policeRet = u32(modelRets[Rand(n, 6010) % std::size(modelRets)]);
    c.pickRet = u32(modelRets[Rand(n, 6011) % std::size(modelRets)]);
    c.gangCarRet = u32(modelRets[Rand(n, 6012) % std::size(modelRets)]);
    c.rangeRet = Rand(n, 6013) % 10u;
    c.eax = SeedReg(n, 7000); c.ecx = SeedReg(n, 7003); c.edx = SeedReg(n, 7006); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonE0(const E0& c) {
    std::vector<u32> table;
    for (u32 g = 0; g < kGroups; ++g) for (u32 w = 0; w < kGroupWords; ++w) table.push_back(c.table[g][w]);
    Js j; j.U("family", c.family).A("dens", {c.dens[0], c.dens[1], c.dens[2], c.dens[3]}).U("flag_a", c.flagA).U("flag_b", c.flagB).U("dealer_model", c.dealerModel).U("forced", c.forced)
     .U("dont_gang", c.dontGang).U("dont_cops", c.dontCops).A("strength", std::vector<u32>(c.strength, c.strength + 11)).A("table", table)
     .A("loaded", std::vector<u32>(c.loaded, c.loaded + kModels)).A("rnd", std::vector<u32>(c.rnd, c.rnd + kRandSeq))
     .U("gang_war_ret", c.gangWarRet).U("police_ret", c.policeRet).U("pick_ret", c.pickRet).U("gang_car_ret", c.gangCarRet).U("range_ret", c.rangeRet)
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}
}

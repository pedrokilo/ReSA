#pragma once
// Fixtures del banco CarCtrlRest63B (lote 63, CCarCtrl parte C, tanda 2): 4096 casos por funcion. Solo C++ estandar (sin tipos del juego):
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

namespace Cc63bFix {
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

// ------------------------------------------------------------------------------------------------ arenas (suma a mano: guarda + ventana + guarda)
constexpr u32 kGuard = 0x40;
// Vehiculo: ventana de 0x600 bytes con guardas de 0x40 a cada lado. 0x40 + 0x600 + 0x40 = 0x680.
constexpr u32 kVehWindow = 0x600, kVehArena = kGuard + kVehWindow + kGuard;
// Matriz falsa (solo +0x30 = posicion): ventana de 0x40, guardas de 0x20. 0x20 + 0x40 + 0x20 = 0x80.
constexpr u32 kMatGuard = 0x20, kMatWindow = 0x40, kMatArena = kMatGuard + kMatWindow + kMatGuard;
// CModelInfo::ms_modelInfoPtrs: 20000 punteros.
constexpr u32 kModelInfoSlots = 20000;
// Pool falso de SetCoordsOfScriptCar: almacenamiento (palabra 0), tabla de estados (palabra 1). Tabla: 16 bytes antes del indice 0 y 144 despues. 16 + 144 = 160.
constexpr u32 kFlagsBefore = 16, kFlagsAfter = 144, kFlagsArena = kFlagsBefore + kFlagsAfter;
constexpr int kPoolMinIndex = -8, kPoolMaxIndex = 100;
constexpr u32 kVtableSlots = 48;
constexpr u32 kPoolBytes = 16;

// ------------------------------------------------------------------------------------------------ coordenadas del mundo
// Modos: 0 tipica, 1 en el borde de un sector tras sumar/restar el radio, 2 borde bajo, 3 borde alto, 4 lejana, 5 enorme, 6 infinita, 7 NaN, 8 diminuta, 9 especiales, 10 enteros pequenos
inline u32 WorldCoord(u32 n, u32 k, u32 mode, float range) {
    switch (mode) {
    case 0: return B(float(i32(Rand(n, k) % 6000u) - 3000) + float(Rand(n, k + 1) % 1000u) / 1000.0f);
    case 1: {
        static constexpr float d[]{0.0f, 14.0f, -14.0f, 11.0f, -11.0f, 44.0f, -44.0f, 0.5f, -0.5f};
        const float base = 50.0f * float(Rand(n, k) % 121u) - 3000.0f;   // sectores 0..120
        float v = base + (Rand(n, k + 1) % 3 == 0 ? d[Rand(n, k + 2) % std::size(d)] : (Rand(n, k + 2) % 2 ? range : -range));
        switch (Rand(n, k + 3) % 5) {
        case 1: v = std::nextafter(v, 1e30f); break;
        case 2: v = std::nextafter(v, -1e30f); break;
        case 3: v += 0.001f; break;
        case 4: v -= 0.001f; break;
        default: break;
        }
        return B(v); }
    case 2: return B(-2900.0f - float(Rand(n, k) % 400u) + float(Rand(n, k + 1) % 100u) / 100.0f);
    case 3: return B(2900.0f + float(Rand(n, k) % 400u) + float(Rand(n, k + 1) % 100u) / 100.0f);
    case 4: return B((Rand(n, k) % 2 ? 1.0f : -1.0f) * (1e4f + float(Rand(n, k + 1) % 1000000u)));
    case 5: return HugeFloat(n, k);
    case 6: return InfBits(n, k);
    case 7: return NanBits(Rand(n, k) % 6, Rand(n, k + 1) % 3 == 0);
    case 8: return TinyFloat(n, k);
    case 9: return kFloatEdges[Rand(n, k) % kFloatEdges.size()];
    default: return B(float(i32(Rand(n, k) % 41u) - 20));
    }
}

// ================================================================================================ 0: SetUpDriverAndPassengersForVehicle
constexpr u32 kPassTokens = 16, kRandSeq = 64;
struct C0 {
    u32 family{}, pedType{}, minPass{}, b4{}, b5{}, passNum{}, model{}, maxPass{}, driver{}, isCarRet{}, animBlock{}, animFile{}, drvRet{};
    u32 passTok[kPassTokens]{}, rnd[kRandSeq]{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline C0 MakeC0(u32 n) {
    C0 c; const u32 f = n % 16; c.family = f;
    static constexpr i32 gangEdges[]{13, 14, 15, 22, 23, 24, 0, -1, 7, 100, 0x7FFFFFFF, i32(0x80000000u)};
    switch (f % 8) {
    case 0: case 1: case 2: c.pedType = u32(14 + i32(Rand(n, 1) % 10u)); break;
    case 3: c.pedType = u32(gangEdges[Rand(n, 1) % std::size(gangEdges)]); break;
    case 4: c.pedType = u32(i32(Rand(n, 1) % 14u)); break;
    case 5: c.pedType = u32(24 + i32(Rand(n, 1) % 20u)); break;
    case 6: c.pedType = Rand(n, 1); break;
    default: c.pedType = Rand(n, 1) % 2 ? u32(14 + i32(Rand(n, 2) % 10u)) : u32(gangEdges[Rand(n, 2) % std::size(gangEdges)]); break;
    }
    c.maxPass = Rand(n, 3) % 12 == 0 ? Rand(n, 4) % 13u : Rand(n, 4) % 9u;
    c.minPass = u32(i32(Rand(n, 5) % 12u) - 3);
    if (f == 9) c.minPass = c.maxPass + Rand(n, 6) % 3u;              // minimo >= limite
    if (f == 10) c.minPass = c.maxPass > 0 ? c.maxPass - 1 : 0;      // un solo intento
    c.passNum = Rand(n, 7) % 10 == 0 ? u32(99) : Rand(n, 7) % 7 == 0 ? u32(i32(Rand(n, 8) % 4u) - 3) : Rand(n, 8) % 11u;
    c.b4 = Rand(n, 9) % 5 == 0 ? ((Rand(n, 10) & 0xFFFFFF00u) | (Rand(n, 11) % 2)) : Rand(n, 11) % 2;
    c.b5 = Rand(n, 12) % 5 == 0 ? ((Rand(n, 13) & 0xFFFFFF00u) | (Rand(n, 14) % 2)) : Rand(n, 14) % 2;
    const bool isCar = Rand(n, 15) % 3 != 0;
    c.isCarRet = BoolRet(n, 16, isCar);
    c.model = isCar ? Rand(n, 20) % kModelInfoSlots : (Rand(n, 21) & 0xFFFFu);
    c.animBlock = Rand(n, 22) % 3 == 0 ? Rand(n, 23) : (Rand(n, 23) % 20u);
    c.animFile = Rand(n, 24) % 2 ? c.animBlock : (Rand(n, 25) % 3 == 0 ? Rand(n, 26) : (Rand(n, 26) % 20u));
    c.driver = Rand(n, 27) % 6 == 0 ? 0u : 0x00B00000u + (Rand(n, 28) & 0xFFFF0u);
    c.drvRet = Rand(n, 29);
    for (u32 i = 0; i < kPassTokens; ++i) c.passTok[i] = Rand(n, 100 + i) % 5 == 0 ? 0u : 0x00C00000u + (Rand(n, 120 + i) & 0xFFFF0u);
    static constexpr u32 edges[]{0, 1, 4094, 4095, 4096, 4097, 0x3FFD, 0x3FFE, 0x3FFF, 0x4000, 0x4001, 32766, 32767};
    const u32 mode = f % 4;
    for (u32 i = 0; i < kRandSeq; ++i) {
        u32 v = Rand(n, 200 + i) % 32768u;
        if (mode == 0) v = Rand(n, 200 + i) % 6000u;                    // casi siempre exito
        else if (mode == 1) v = 4096u + Rand(n, 200 + i) % 28672u;       // nunca exito (pero a veces < 0x3FFF)
        if (Rand(n, 300 + i) % 4 == 0) v = edges[Rand(n, 400 + i) % std::size(edges)];
        c.rnd[i] = v;
    }
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC0(const C0& c) {
    Js j; j.U("family", c.family).U("ped_type", c.pedType).U("min_pass", c.minPass).U("b4", c.b4).U("b5", c.b5).U("pass_num", c.passNum).U("model", c.model).U("max_pass", c.maxPass)
     .U("driver", c.driver).U("is_car_ret", c.isCarRet).U("anim_block", c.animBlock).U("anim_file", c.animFile).U("drv_ret", c.drvRet);
    j.A("pass_tok", std::vector<u32>(c.passTok, c.passTok + kPassTokens)).A("rnd", std::vector<u32>(c.rnd, c.rnd + kRandSeq));
    j.U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 1: SetCoordsOfScriptCar
struct C1 {
    u32 family{}, pos[3]{}, arg5{}, arg6{}, vtype{}, tempAction{}, poolIndex{}, poolRem{}, groundRet{}, distRet{}, flagsSeed{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline C1 MakeC1(u32 n) {
    C1 c; const u32 f = n % 16; c.family = f;
    auto coord = [&](u32 k, u32 mode) { return WorldCoord(n, k, mode, 14.0f); };
    c.pos[0] = coord(1, 0); c.pos[1] = coord(5, 0);
    // z: alrededor de -100.0f (0xC2C80000) y especiales
    static constexpr u32 zs[]{0xC2C80000u, 0xC2C7FFFFu, 0xC2C80001u, 0xC2C00000u, 0xC3000000u, 0x00000000u, 0x80000000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00001u, 0x7FA00000u, 0x3F800000u, 0x42C80000u};
    c.pos[2] = Rand(n, 9) % 3 == 0 ? zs[Rand(n, 10) % std::size(zs)] : (Rand(n, 10) % 2 ? OrdinaryFloat(n, 11, 120, 140) : B(-100.0f - float(Rand(n, 11) % 500u) / 4.0f));
    switch (f) {
    case 1: c.pos[0] = coord(20, 5); break;
    case 2: c.pos[1] = coord(20, 6); break;
    case 3: c.pos[0] = coord(20, 7); c.pos[2] = Rand(n, 21) % 2 ? c.pos[2] : zs[Rand(n, 22) % std::size(zs)]; break;
    case 4: c.pos[2] = zs[Rand(n, 20) % std::size(zs)]; break;
    case 5: c.pos[2] = TinyFloat(n, 20); break;
    default: break;
    }
    static constexpr u32 b[]{0u, 1u, 2u, 0x80u, 0xFFu, 0x100u, 0xFFFFFF00u, 0x01010101u};
    c.arg5 = Rand(n, 30) % 3 == 0 ? b[Rand(n, 31) % std::size(b)] : Rand(n, 31) % 2;
    c.arg6 = Rand(n, 32) % 3 == 0 ? b[Rand(n, 33) % std::size(b)] : Rand(n, 33) % 2;
    static constexpr u32 types[]{0, 0, 0, 5, 5, 11, 9, 9, 1, 2, 3, 4, 6, 7, 8, 10, 12, 0xFFFFFFFFu, 0x100u, 0x80000000u, 0x80000005u};
    c.vtype = types[Rand(n, 34) % std::size(types)];
    if (f == 6) c.vtype = 5;
    if (f == 7) c.vtype = 0;
    if (f == 8) c.vtype = 9;
    if (f == 9) c.vtype = 11;
    c.tempAction = Rand(n, 35) & 0xFFu;
    c.poolIndex = u32(kPoolMinIndex + i32(Rand(n, 36) % u32(kPoolMaxIndex - kPoolMinIndex + 1)));
    c.poolRem = Rand(n, 37) % 5 == 0 ? (Rand(n, 38) % 2 ? 0u : 0xA17u) : Rand(n, 38) % 0xA18u;
    c.groundRet = Rand(n, 39) % 4 == 0 ? zs[Rand(n, 40) % std::size(zs)] : OrdinaryFloat(n, 41, 100, 140);
    c.distRet = Rand(n, 42) % 5 == 0 ? (Rand(n, 43) % 2 ? TinyFloat(n, 44) : zs[Rand(n, 45) % std::size(zs)]) : OrdinaryFloat(n, 46, 110, 135);
    c.flagsSeed = Rand(n, 47);
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC1(const C1& c) {
    Js j; j.U("family", c.family).A("pos", {c.pos[0], c.pos[1], c.pos[2]}).U("arg5", c.arg5).U("arg6", c.arg6).U("vtype", c.vtype).U("temp_action", c.tempAction)
     .U("pool_index", c.poolIndex).U("pool_rem", c.poolRem).U("ground_ret", c.groundRet).U("dist_ret", c.distRet).U("flags_seed", c.flagsSeed)
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 2 y 3: ScanForPedDanger / FindMaximumSpeedForThisCarInTraffic
constexpr u32 kSpeedWrites = 24;
struct CS {
    u32 family{}, matMode{}, pos[2]{}, decoy[2]{}, style{}, cruise{}, mult{}, scanCode{}, playerMode{}, honkByte{}, flags429{};
    u32 speedWrites[kSpeedWrites]{}, mutateAt{}, mutCruise{}, mutMult{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
// ranges: 14.0f (FindMaximumSpeed) o 44.0f / 11.0f (ScanForPedDanger, segun sea el vehiculo del jugador)
inline CS MakeCS(u32 n, bool findMax) {
    CS c; const u32 f = n % 16; c.family = f;
    static constexpr u32 modeX[16]{0, 1, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 0, 10};
    static constexpr u32 modeY[16]{0, 1, 0, 1, 0, 3, 0, 5, 0, 7, 8, 9, 0, 1, 0, 10};
    c.playerMode = Rand(n, 1) % 3;   // 0: otro vehiculo (puntero distinto), 1: el propio vehiculo, 2: nulo
    const float range = findMax ? 14.0f : (c.playerMode == 1 ? 44.0f : 11.0f);
    u32 mx = modeX[f], my = modeY[f];
    if (f == 15) { mx = Rand(n, 2) % 11; my = Rand(n, 3) % 11; }
    c.pos[0] = WorldCoord(n, 10, mx, range); c.pos[1] = WorldCoord(n, 20, my, range);
    if (f == 10 || f == 11) { c.pos[Rand(n, 4) % 2] = WorldCoord(n, 30, 0, range); }   // solo un eje especial
    c.decoy[0] = OrdinaryFloat(n, 40, 120, 135); c.decoy[1] = OrdinaryFloat(n, 44, 120, 135);
    c.matMode = Rand(n, 5) % 2;
    c.style = Rand(n, 6) % 4 == 0 ? Rand(n, 7) & 0xFFu : Rand(n, 7) % 8;
    c.cruise = Rand(n, 8) % 5 == 0 ? (Rand(n, 9) % 2 ? 255u : 0u) : Rand(n, 9) % 120u;
    c.mult = Rand(n, 12) % 4 == 0 ? 0x3F800000u : OrdinaryFloat(n, 13, 120, 132) & 0x7FFFFFFFu;
    if (f == 14) {
        static constexpr u32 ms[]{0u, 0x80000000u, 1u, 0x007FFFFFu, 0x00800000u, 0x7F800000u, 0x7FC00000u, 0xFFC00123u, 0x7FA00000u, 0x7F7FFFFFu, 0x3F800000u, 0xBF800000u};
        c.mult = ms[Rand(n, 14) % std::size(ms)];
        if (Rand(n, 15) % 2) c.mult = TinyFloat(n, 16);
    }
    c.scanCode = Rand(n, 17) % 2 ? Rand(n, 18) % 3000u : Rand(n, 18) % 65536u;
    if (f == 12 || f == 5) { static constexpr u32 sc[]{0xFFFFu, 0xFFFEu, 0u, 1u, 0x8000u, 0xFFFDu}; c.scanCode = sc[Rand(n, 19) % std::size(sc)]; }
    c.honkByte = Rand(n, 21) & 0xFFu;
    c.flags429 = Rand(n, 22) & 0xFFu;
    // escrituras del falso sobre la velocidad por orden de llamada (0 = sin cambio)
    for (u32 i = 0; i < kSpeedWrites; ++i) {
        u32 v = 0;
        const u32 r = Rand(n, 100 + i) % 10;
        if (f >= 12 || f == 15) {
            if (r < 3) v = OrdinaryFloat(n, 130 + i, 120, 140) & 0x7FFFFFFFu;
            else if (r == 3) v = TinyFloat(n, 160 + i) & 0x7FFFFFFFu;
            else if (r == 4) v = NanBits(Rand(n, 190 + i) % 6, false);
        }
        c.speedWrites[i] = v;
    }
    c.mutateAt = (f == 15 || f == 13) ? Rand(n, 230) % 8 : 99u;
    c.mutCruise = Rand(n, 231) % 256u;
    c.mutMult = Rand(n, 232) % 4 == 0 ? TinyFloat(n, 233) : OrdinaryFloat(n, 234, 120, 135) & 0x7FFFFFFFu;
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonCS(const CS& c) {
    Js j; j.U("family", c.family).U("mat_mode", c.matMode).A("pos", {c.pos[0], c.pos[1]}).A("decoy", {c.decoy[0], c.decoy[1]}).U("style", c.style).U("cruise", c.cruise).U("mult", c.mult)
     .U("scan_code", c.scanCode).U("player_mode", c.playerMode).U("honk_byte", c.honkByte).U("flags429", c.flags429)
     .A("speed_writes", std::vector<u32>(c.speedWrites, c.speedWrites + kSpeedWrites)).U("mutate_at", c.mutateAt).U("mut_cruise", c.mutCruise).U("mut_mult", c.mutMult)
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}
}

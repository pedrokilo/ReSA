#pragma once
// Fixtures del banco Player82B (lote 82B): CPlayerPed::Compute3rdPersonMouseTarget (EXE 0x60B650, thiscall, ECX = jugador, un argumento bool de pila, `ret 4`, void).
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CPlayerPed::Compute3rdPersonMouseTarget  0x60B650  (bool meleeWeapon)   (hook CPlayerPed / Compute3rdPersonMouseTarget, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: n % 16 = familia (1024 casos por familia, 16384 casos).
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

namespace Pm82BFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kPedWin = 0x800, kMatWin = 0x80, kIntelWin = 0x100, kTaskWin = 0x20, kGroupWin = 0x80, kInfoWin = 0x40, kEvWin = 0x20;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kPedArena = Arena(kPedWin), kMatArena = Arena(kMatWin), kIntelArena = Arena(kIntelWin), kTaskArena = Arena(kTaskWin), kGroupArena = Arena(kGroupWin),
                      kInfoArena = Arena(kInfoWin), kEvArena = Arena(kEvWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 82102u); }
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
    std::string End() const { return s + "}"; }
};

inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}

// Valores especiales de float (0, -0, desnormales, uno, maximos, infinitos, NaN con carga util)
inline u32 SpecialF(u32 n, u32 k) {
    static constexpr u32 sp[]{0x00000000u, 0x80000000u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00800000u, 0x3F800000u, 0xBF800000u, 0x7F7FFFFFu, 0xFF7FFFFFu,
                              0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x3F000000u, 0x42C80000u, 0x41200000u, 0x42700000u};
    return sp[Rand(n, k) % std::size(sp)];
}
// Coordenada de mundo: casi siempre -3000..3000 (con dos decimales)
inline u32 Coord(u32 n, u32 k) { return B(float(i32(Rand(n, k) % 600001) - 300000) / 100.0f); }
// Componente de un vector frontal aproximadamente unitario
inline u32 Unit(u32 n, u32 k) { return B(float(i32(Rand(n, k) % 2000001) - 1000000) / 1000000.0f); }
// Float normal con exponente aleatorio (-30..30) y signo y mantisa aleatorios
inline u32 Wide(u32 n, u32 k) {
    const u32 r = Rand(n, k), e = 127u + (Rand(n, k + 1) % 61u) - 30u;
    return (r & 0x80000000u) | (e << 23) | (Rand(n, k + 2) & 0x7FFFFFu);
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 useMouse{};                          // byte 0xB6EC2E (cualquier valor: el EXE hace `test al,al`)
    u32 meleeArg{};                          // dword de pila (el EXE lee solo el byte bajo)
    u32 slot{};                              // byte +0x718 (movsx)
    u32 wType{}, skillRet{};
    u32 range0{}, fire1{}, flags2{};         // campos leidos de las tres estructuras CWeaponInfo (+4, +0, +0x18) por las llamadas 0, 1 y 2
    u32 info0Fire{}, info1Range{}, info1Flags{}, info2Fire{}, info2Range{};   // relleno de las otras (no se leen)
    u32 pHasM{};
    u32 pPl[3]{}, pM[3]{};                   // posicion del jugador en m_placement (+4) y en la matriz (+0x30)
    u32 camIdx{};                            // byte 0xB6F081
    u32 camSrc[3][3]{}, camFront[3][3]{};    // por camara: m_vecSource y m_vecFront
    u32 f3Src[3]{}, f3Tgt[3]{};              // salida de Find3rdPersonCamTargetVector
    u32 losRet{}, losWrites{}, hitMode{};    // retorno completo del trazado, si escribe el golpe con retorno falso, y a quien golpea
    u32 aliveRet{};                          // IsAlive (bits altos basura)
    u32 oldMode{};                           // objetivo anterior: 0 nulo, 1 el golpeado, 2 otro peaton, 3 otro mas
    u32 f7a0{}, timer{};
    u32 seeRet{};
    u32 taskMask{}, taskRet[3]{};            // tareas presentes (bit i = ranura i) y su GetTaskType
    u32 groupMode{}, sameRet{}, newNull{};
    u32 evFill{};                            // relleno de la arena del evento reservado
    u32 deadPeds{}, carTyres{}, ignoreOld{};
};

inline void Pos(u32* dst, u32 n, u32 k) { for (unsigned i = 0; i < 3; ++i) dst[i] = Coord(n, k + 3 * i); }

inline u32 TaskTypeOf(u32 n, u32 k) {
    static constexpr u32 sp[]{0x259u, 0x258u, 0x25Au, 0u, 0xFFFFFFFFu, 0x80000259u, 0x159u, 0x2590u};
    return Rand(n, k) % 3 == 0 ? 0x259u : sp[Rand(n, k + 1) % std::size(sp)];
}

inline C BaseDefaults(unsigned n) {
    C c;
    c.n = n; c.family = n % 16; c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.useMouse = Rand(n, 3) % 8 ? (Rand(n, 4) % 4 ? 1u : ((Rand(n, 5) | 1u) & 0xFF)) : 0u;
    c.meleeArg = (Rand(n, 6) & 0xFFFFFF00u) | (Rand(n, 8) % 2 ? 1u : (Rand(n, 9) % 2 ? 0u : (Rand(n, 10) % 255 + 1)));
    c.slot = Rand(n, 11) % 4 ? Rand(n, 12) % 13 : (u32(i32(Rand(n, 13) % 41) - 20) & 0xFF);
    c.wType = Rand(n, 14) % 3 == 0 ? 0x17u : (Rand(n, 15) % 2 ? Rand(n, 16) % 47 : Rand(n, 17));
    c.skillRet = Rand(n, 19) % 4;   // GetWeaponSkill del EXE devuelve un entero 0..3 en EAX (los bits altos basura no son dominio)
    c.range0 = Rand(n, 21) % 6 ? B(float(Rand(n, 22) % 20001) / 100.0f) : SpecialF(n, 23);
    c.fire1 = Rand(n, 24) % 5 ? 1u + Rand(n, 25) % 5 : (Rand(n, 26) % 2 ? 0u : Rand(n, 27));
    c.flags2 = Rand(n, 28) % 4 ? (Rand(n, 29) | 1u) : (Rand(n, 30) & ~1u);
    c.info0Fire = Rand(n, 31); c.info1Range = Rand(n, 32); c.info1Flags = Rand(n, 33); c.info2Fire = Rand(n, 34); c.info2Range = Rand(n, 35);
    c.pHasM = Rand(n, 36) % 4 != 0;
    Pos(c.pPl, n, 40); Pos(c.pM, n, 50);
    c.camIdx = Rand(n, 60) % 3;
    for (unsigned k = 0; k < 3; ++k) for (unsigned i = 0; i < 3; ++i) c.camFront[k][i] = Unit(n, 80 + 3 * k + i);
    // la fuente de la camara a 0..30 m del jugador (el producto escalar cambia de signo con frecuencia)
    for (unsigned k = 0; k < 3; ++k) {
        const u32* pp = (k % 2 ? c.pM : c.pPl);
        for (unsigned i = 0; i < 3; ++i) c.camSrc[k][i] = B(F(pp[i]) + float(i32(Rand(n, 90 + 3 * k + i) % 6001) - 3000) / 100.0f);
    }
    for (unsigned i = 0; i < 3; ++i) { c.f3Src[i] = Coord(n, 100 + i); c.f3Tgt[i] = Coord(n, 103 + i); }
    c.losRet = Rand(n, 110) % 5 ? ((Rand(n, 111) & 0xFFFFFF00u) | 1u) : (Rand(n, 112) & 0xFFFFFF00u);
    if (Rand(n, 113) % 8 == 0) c.losRet = Rand(n, 114);
    c.losWrites = Rand(n, 115) % 2;
    c.hitMode = Rand(n, 116) % 8 ? 1u : Rand(n, 117) % 4;
    c.aliveRet = Rand(n, 120) % 5 ? ((Rand(n, 121) & 0xFFFFFF00u) | 1u) : (Rand(n, 122) & 0xFFFFFF00u);
    c.oldMode = Rand(n, 123) % 4;
    c.timer = Rand(n, 124);
    c.f7a0 = Rand(n, 125) % 2 ? c.timer + (Rand(n, 126) % 5000) : c.timer - (Rand(n, 127) % 5000);
    c.seeRet = Rand(n, 128) % 4 ? ((Rand(n, 129) & 0xFFFFFF00u) | 1u) : (Rand(n, 130) & 0xFFFFFF00u);
    c.taskMask = Rand(n, 131) % 3 ? 0u : (Rand(n, 132) % 8);
    for (unsigned i = 0; i < 3; ++i) c.taskRet[i] = TaskTypeOf(n, 133 + 2 * i);
    c.groupMode = Rand(n, 140) % 2; c.sameRet = Rand(n, 141) % 4 ? (Rand(n, 142) & 0xFFFFFF00u) : (Rand(n, 143) | 1u);
    c.newNull = Rand(n, 144) % 16 == 0;
    c.evFill = Rand(n, 145) & 0xFF;
    c.deadPeds = Rand(n, 146) & 0xFF; c.carTyres = Rand(n, 147) & 0xFF; c.ignoreOld = Rand(n, 148);
    return c;
}

inline void Chain(C& c, bool full) {   // el trazado golpea a un peaton vivo (y, con full, todo el recorrido llega al final)
    c.useMouse = 1; c.hitMode = 1; c.losRet = (c.losRet & ~0xFFu) | 1u; c.aliveRet = (c.aliveRet & ~0xFFu) | 1u;
    if (full) { c.fire1 = 1 + c.fire1 % 5; c.flags2 |= 1u; c.seeRet = (c.seeRet & ~0xFFu) | 1u; c.taskMask = 0; }
}

inline C MakeC(unsigned n) {
    C c = BaseDefaults(n);
    switch (c.family) {
    case 0:   // flujo completo hasta el final: arma de fuego con bit 0, a la vista, casi nunca con tarea pendiente
        Chain(c, true); c.taskMask = Rand(n, 201) % 4 == 0 ? Rand(n, 202) % 8 : 0u; c.slot = Rand(n, 203) % 13; break;
    case 1:   // raton apagado (o con un valor distinto de 0/1 que cuenta como encendido): caduca el objetivo anterior
        c.useMouse = Rand(n, 204) % 3 ? 0u : (Rand(n, 205) & 0xFF); c.oldMode = 1 + Rand(n, 206) % 3; break;
    case 2:   // ruta del vector de apuntado de la camara
        c.useMouse = 1; c.meleeArg = (c.meleeArg & 0xFFFFFF00u) | (Rand(n, 207) % 2 ? 1u : 1u + Rand(n, 208) % 255); break;
    case 3:   // ruta calculada con producto escalar negativo frecuente
        c.useMouse = 1; c.meleeArg &= 0xFFFFFF00u;
        for (unsigned k = 0; k < 3; ++k) {
            const u32* pp = c.pHasM ? c.pM : c.pPl;
            const float d[3]{float(i32(Rand(n, 210 + k) % 6001) - 3000) / 100.0f, float(i32(Rand(n, 213 + k) % 6001) - 3000) / 100.0f, float(i32(Rand(n, 216 + k) % 6001) - 3000) / 100.0f};
            for (unsigned i = 0; i < 3; ++i) { c.camSrc[k][i] = B(F(pp[i]) + d[i]); c.camFront[k][i] = B(-d[i] / 40.0f); }
        }
        break;
    case 4:   // borde del signo del producto escalar: 0, -0, minimos
        c.useMouse = 1; c.meleeArg &= 0xFFFFFF00u;
        for (unsigned k = 0; k < 3; ++k) {
            const u32* pp = c.pHasM ? c.pM : c.pPl;
            const unsigned mode = Rand(n, 220 + k) % 6;
            if (mode == 0) { for (unsigned i = 0; i < 3; ++i) c.camSrc[k][i] = pp[i]; }
            else if (mode == 1) { for (unsigned i = 0; i < 3; ++i) c.camFront[k][i] = (Rand(n, 223 + 3 * k + i) % 2) ? 0u : 0x80000000u; }
            else if (mode == 2) { c.camFront[k][0] = B(1.0f); c.camFront[k][1] = 0u; c.camFront[k][2] = 0x80000000u; c.camSrc[k][0] = B(F(pp[0]) - 0.0000001f); c.camSrc[k][1] = pp[1]; c.camSrc[k][2] = pp[2]; }
            else if (mode == 3) { c.camFront[k][0] = B(1.0f); c.camFront[k][1] = 0u; c.camFront[k][2] = 0u; c.camSrc[k][0] = B(F(pp[0]) + (Rand(n, 232 + k) % 2 ? 1e-30f : -1e-30f)); c.camSrc[k][1] = pp[1]; c.camSrc[k][2] = pp[2]; }
            else if (mode == 4) { for (unsigned i = 0; i < 3; ++i) c.camSrc[k][i] = B(F(pp[i]) + float(i32(Rand(n, 235 + 3 * k + i) % 5) - 2)); }
        }
        break;
    case 5:   // trazado: negativo, golpe nulo / el propio jugador / otro peaton, muerto
        c.useMouse = 1; c.hitMode = Rand(n, 244) % 4; c.losWrites = Rand(n, 245) % 2; break;
    case 6:   // gestion del objetivo anterior (arma cuerpo a cuerpo: termina justo despues)
        Chain(c, false); c.oldMode = Rand(n, 246) % 4; c.fire1 = 0; break;
    case 7:   // cadena de armas: cuerpo a cuerpo, bit 0 de las banderas, ranuras con signo, tipo 0x17
        Chain(c, false);
        c.fire1 = Rand(n, 247) % 3 == 0 ? 0u : 1u + Rand(n, 248) % 5; c.flags2 = Rand(n, 249) % 2 ? (Rand(n, 250) | 1u) : (Rand(n, 251) & ~1u);
        c.slot = Rand(n, 252) % 2 ? (u32(i32(Rand(n, 253) % 41) - 20) & 0xFF) : Rand(n, 254) % 21;
        c.wType = Rand(n, 255) % 2 ? 0x17u : Rand(n, 256) % 47; break;
    case 8:   // a la vista y tareas pendientes
        Chain(c, true);
        c.seeRet = Rand(n, 257) % 6 ? ((c.seeRet & ~0xFFu) | 1u) : (c.seeRet & ~0xFFu); c.taskMask = Rand(n, 258) % 8;
        for (unsigned i = 0; i < 3; ++i) c.taskRet[i] = Rand(n, 259 + i) % 2 ? 0x259u : TaskTypeOf(n, 262 + i);
        break;
    case 9:   // grupos
        Chain(c, true); c.groupMode = Rand(n, 265) % 3 != 0; c.newNull = Rand(n, 266) % 5 == 0; c.oldMode = Rand(n, 267) % 4; break;
    case 10: {   // no finitos y especiales en posiciones, camaras y alcance
        c.useMouse = 1; c.meleeArg &= 0xFFFFFF00u;
        u32* all[]{c.pPl, c.pM, c.camSrc[0], c.camSrc[1], c.camSrc[2], c.camFront[0], c.camFront[1], c.camFront[2]};
        for (unsigned v = 0; v < 8; ++v) for (unsigned i = 0; i < 3; ++i) if (Rand(n, 270 + 3 * v + i) % 5 == 0) all[v][i] = SpecialF(n, 300 + 3 * v + i);
        if (Rand(n, 330) % 3 == 0) c.range0 = SpecialF(n, 331);
        break; }
    case 11: {   // exponentes variados (redondeo a 24 bits y cancelaciones)
        c.useMouse = 1; c.meleeArg &= 0xFFFFFF00u;
        u32* all[]{c.pPl, c.pM, c.camSrc[0], c.camSrc[1], c.camSrc[2], c.camFront[0], c.camFront[1], c.camFront[2]};
        for (unsigned v = 0; v < 8; ++v) for (unsigned i = 0; i < 3; ++i) if (Rand(n, 340 + 3 * v + i) % 2 == 0) all[v][i] = Wide(n, 370 + 4 * (3 * v + i));
        c.range0 = Wide(n, 470);
        break; }
    case 12:   // jugador con y sin matriz y ruta melee
        c.useMouse = 1; c.pHasM = Rand(n, 475) % 2; c.meleeArg = (c.meleeArg & 0xFFFFFF00u) | (Rand(n, 476) % 2); break;
    case 13:   // camara activa y valores distintos por camara
        c.useMouse = 1; c.meleeArg &= 0xFFFFFF00u; c.camIdx = Rand(n, 477) % 3; break;
    case 14: {  // temporizador y caducidad
        c.useMouse = Rand(n, 478) % 2; c.hitMode = 1; c.losRet = Rand(n, 479) % 2 ? (c.losRet & ~0xFFu) : (c.losRet | 1u);
        c.oldMode = 1 + Rand(n, 480) % 3;
        static constexpr u32 ts[]{0u, 1u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFC17u, 0xFFFFFC18u, 999u, 1000u, 1001u};
        c.timer = ts[Rand(n, 481) % std::size(ts)];
        static constexpr i32 ds[]{-2, -1, 0, 1, 2, 999, 1000, 1001, -1000};
        c.f7a0 = c.timer + u32(ds[Rand(n, 482) % std::size(ds)]);
        if (Rand(n, 483) % 6 == 0) c.f7a0 = Rand(n, 484);
        break; }
    default: {  // 15: mezcla con especiales poco frecuentes
        c.useMouse = Rand(n, 485) % 6 != 0;
        u32* all[]{c.pPl, c.pM, c.camSrc[0], c.camSrc[1], c.camSrc[2], c.camFront[0], c.camFront[1], c.camFront[2]};
        for (unsigned v = 0; v < 8; ++v) for (unsigned i = 0; i < 3; ++i) if (Rand(n, 490 + 3 * v + i) % 40 == 0) all[v][i] = SpecialF(n, 520 + 3 * v + i);
        break; }
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("use_mouse", c.useMouse).U("melee_arg", c.meleeArg).U("slot", c.slot).U("w_type", c.wType).U("skill_ret", c.skillRet);
    j.U("range0", c.range0).U("fire1", c.fire1).U("flags2", c.flags2).U("p_has_m", c.pHasM).U("cam_idx", c.camIdx);
    for (unsigned i = 0; i < 3; ++i) {
        const std::string s = std::to_string(i);
        j.U(("p_pl" + s).c_str(), c.pPl[i]).U(("p_m" + s).c_str(), c.pM[i]).U(("f3_src" + s).c_str(), c.f3Src[i]).U(("f3_tgt" + s).c_str(), c.f3Tgt[i]);
        for (unsigned k = 0; k < 3; ++k) {
            const std::string t = s + "_" + std::to_string(k);
            j.U(("cam_src" + t).c_str(), c.camSrc[k][i]).U(("cam_front" + t).c_str(), c.camFront[k][i]);
        }
        j.U(("task_ret" + s).c_str(), c.taskRet[i]);
    }
    j.U("los_ret", c.losRet).U("los_writes", c.losWrites).U("hit_mode", c.hitMode).U("alive_ret", c.aliveRet).U("old_mode", c.oldMode).U("f7a0", c.f7a0).U("timer", c.timer);
    j.U("see_ret", c.seeRet).U("task_mask", c.taskMask).U("group_mode", c.groupMode).U("same_ret", c.sameRet).U("new_null", c.newNull).U("ev_fill", c.evFill);
    j.U("dead_peds", c.deadPeds).U("car_tyres", c.carTyres).U("ignore_old", c.ignoreOld);
    return j.End();
}
}

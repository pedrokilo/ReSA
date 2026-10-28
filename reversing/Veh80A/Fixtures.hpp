#pragma once
// Fixtures del banco Veh80A (lote 80A): CVehicle::DoDriveByShootings del EXE (0x741FD0), 20480 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CVehicle::DoDriveByShootings 0x741FD0  thiscall, ECX = this (vehiculo), sin argumentos, `ret`, void   (hook CVehicle / DoDriveByShootings, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 20 (1024 casos por familia).
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

namespace Ve80Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;
using i16 = std::int16_t;

constexpr unsigned kCases = 20480;        // por funcion: 20 familias de 1024
constexpr unsigned kFamilies = 20;
constexpr unsigned kFunctions = 1;
enum Fn : u32 { kDriveBy = 0 };

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 80080u); }
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
    Js& A(const char* k, const std::vector<u32>& v) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < v.size(); ++i) { if (i) s += ','; s += std::to_string(v[i]); }
        s += ']'; return *this;
    }
    Js& Raw(const char* k, const std::string& raw) { Key(k); s += raw; return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ arenas y memoria global
constexpr u32 kGuard = 0x40;
constexpr u32 kThisWindow = 0x600,  kThisArena = kGuard + kThisWindow + kGuard;               // vehiculo: vptr +0, banderas +0x428..0x42B, conductor +0x460, subtipo +0x594
constexpr u32 kDrvObj = 0x1000, kDrvArena = 0x2800;                                            // peaton: el objeto esta en +0x1000 de la arena; la ranura de arma (-128..127) cae siempre dentro
constexpr u32 kInfoWindow = 0x70,   kInfoArena = kGuard + kInfoWindow + kGuard;               // CWeaponInfo
constexpr u32 kPInfoWindow = 0x190, kPInfoArena = kGuard + kPInfoWindow + kGuard;             // CPlayerInfo
constexpr u32 kRideWindow = 0x1C,   kRideArena = kGuard + kRideWindow + kGuard;               // CRideAnimData
constexpr u32 kAssocWindow = 0x40,  kAssocArena = kGuard + kAssocWindow + kGuard;             // CAnimBlendAssociation
constexpr u32 kPadWindow = 0x10,    kPadArena = kGuard + kPadWindow + kGuard;                 // CPad (opaco)
constexpr u32 kNumArenas = 12;
// indices de arena (orden de las etiquetas): 0 this, 1 conductor, 2 info A, 3 info B, 4 info del jugador, 5 ride A, 6 ride B, 7..10 asociaciones, 11 mando
constexpr u32 kAT = 0, kAD = 1, kAIA = 2, kAIB = 3, kAP = 4, kARA = 5, kARB = 6, kAS0 = 7, kAPad = 11;
constexpr u32 kCamActive = 0xB6F081, kCamFlag = 0xB6F057, kCamArray = 0xB6F19C, kCamSize = 0x238, kTimer = 0xB7CB84;
constexpr u32 kCamRegion1 = 0xB6F050, kCamRegion1Bytes = 0x40, kCamRegion2Bytes = 3 * kCamSize;
constexpr u32 kWeaponBase = 0x5A0, kWeaponSize = 0x1C;
static_assert(kDrvObj + kWeaponBase + 127 * kWeaponSize + 0x14 <= kDrvArena && kDrvObj + kWeaponBase >= 128 * kWeaponSize, "la ranura (-128..127) cae siempre dentro de la arena del conductor");

// ------------------------------------------------------------------------------------------------ paletas
inline constexpr std::array<u32, 28> kFlt{0x00000000u, 0x80000000u, 0x3F800000u, 0x3F7D70A4u, 0x3F7D70A3u, 0x3F7D70A5u, 0xBF800000u, 0xC1000000u, 0x41000000u, 0x7FC00000u,
    0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0x7F800000u, 0xFF800000u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x3F000000u, 0x3F7FFFFFu, 0x3F800001u,
    0x3DCCCCCDu, 0x34000000u, 0xBF7D70A4u, 0x7FC12345u, 0xFFC54321u};
inline constexpr std::array<u32, 12> kSmall{0u, 1u, 2u, 3u, 4u, 5u, 0x31u, 0x80u, 0xFFu, 0x100u, 0x10001u, 0x30031u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 AnyU32(u32 c, u32 k) {
    switch (Rand(c, k) % 4) {
    case 0: return Pick(kFlt, c, k + 100);
    case 1: return Pick(kSmall, c, k + 100);
    default: return Rand(c, k + 100);
    }
}
inline u32 Garbage(u32 c, u32 k) { return Rand(c, k) & 0xFFFFFF00u; }       // basura en los 24 bits altos de EAX
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}

// ------------------------------------------------------------------------------------------------ el caso
struct Mut { u32 after{}, target{}, value{}; };           // tras el evento `after` (dentro del falso, antes de volver) se escribe `value` en el objetivo
struct Assoc { u32 flags{}, amount{}, delta{}; };
constexpr u32 kMutTargets = 19;
struct C {
    u32 fn{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 vsub{}, vflags{};                    // subtipo (+0x594) y byte +0x429 del vehiculo
    u32 has_driver{}, pinfo_null{}, can_drive_by{}, pad_null{};
    u32 slot{}, wtype{}, wstate{}, clip0{}, total{}, nextshot{};
    u32 info1_slot{}, info2_clip{}, info_map{};   // info_map: 0 -> (A,B), 1 -> (A,A), 2 -> (B,A), 3 -> (B,B)
    u32 infoa_clip{}, infob_slot{};          // el otro campo de cada informacion
    u32 gun_ret{}, ll_ret{}, lr_ret{};       // EAX completo que devuelven GetCarGunFired / GetLookLeft / GetLookRight (el EXE lee AX / AL)
    u32 ride1{}, ride2{}, ride_a_group{}, ride_b_group{};   // 0 nulo, 1 A, 2 B
    u32 cam_active{}, cam_flag{};
    u32 cam_mode[3]{}, cam_ll[3]{}, cam_lr[3]{};
    u32 now{};
    u32 get_sel[3]{}, blend_sel{};           // 0 nulo, 1..4 asociacion
    Assoc assoc[4]{};
    u32 clump{};
    u32 nmut{};
    Mut mut[2]{};
};

inline u32 BmxOrOther(u32 c, u32 k) {
    switch (Rand(c, k) % 8) {
    case 0: case 1: case 2: return 10u;                    // BMX
    case 3: case 4: return 0u;                              // automovil
    case 5: return 9u;                                      // moto
    case 6: return Rand(c, k + 1) % 21u;
    default: return Pick(std::array<u32, 6>{11u, 0x10000Au, 0x0A00u, 0xFFFFFFF6u, 0x8000000Au, 0x1Au}, c, k + 1);
    }
}

// Estado base: el camino completo de disparo (todo lo que hace falta para llegar a FireFromCar) con variantes aleatorias de lo que no cambia el camino
inline void Base(C& f, u32 c) {
    auto R = [&](u32 k) { return Rand(c, k); };
    f.vsub = (R(30) % 3 == 0) ? 10u : 0u;
    f.vflags = R(31) & 0xFFu;
    f.has_driver = 1; f.pinfo_null = 0; f.can_drive_by = 1; f.pad_null = 0;
    f.slot = R(32) % 13;
    f.wtype = R(33) % 47;
    f.wstate = R(34) % 8;
    f.clip0 = R(35) % 200;
    f.total = 1 + R(36) % 60;
    f.now = R(37);
    f.nextshot = f.now - 1u - R(38) % 2000u;
    f.info1_slot = 4; f.info2_clip = 1 + R(39) % 60; f.info_map = R(40) % 4;
    f.infoa_clip = R(41) & 0xFFFFu; f.infob_slot = R(42);
    f.gun_ret = 1u + (R(43) & 0xFFFF0000u);                 // AX = 1, basura arriba
    f.ll_ret = 1u | Garbage(c, 44); f.lr_ret = 1u | Garbage(c, 45);
    f.ride1 = (f.vsub == 10) ? 1u + R(46) % 2 : 0u; f.ride2 = 1u + R(47) % 2;
    f.ride_a_group = R(48) % 20; f.ride_b_group = R(49) % 20;
    f.cam_active = R(50) % 3; f.cam_flag = 0;
    for (int i = 0; i < 3; ++i) { f.cam_mode[i] = 2u + R(51 + i) % 40; f.cam_ll[i] = 1; f.cam_lr[i] = R(54 + i) % 2; }
    f.get_sel[0] = 1 + R(57) % 4; f.get_sel[1] = 1 + R(58) % 4; f.get_sel[2] = 1 + R(59) % 4; f.blend_sel = 1 + R(60) % 4;
    for (int i = 0; i < 4; ++i) {
        f.assoc[i].flags = (R(61 + i) & 0xFFFEu);           // bit 0 limpio
        f.assoc[i].amount = B(0.995f + float(R(65 + i) % 100) / 100000.0f);
        f.assoc[i].delta = B(float(R(69 + i) % 50) / 10.0f);
    }
    f.clump = R(73);
    f.nmut = 0;
}

// Cada "botón" rompe una condicion del camino de disparo; `pct` es la probabilidad de aplicarlo en el caso
inline void Perturb(C& f, u32 c, u32 pct, u32 only) {
    auto R = [&](u32 k) { return Rand(c, k); };
    auto on = [&](u32 knob, u32 k) { return (only == 99 || only == knob) && R(k) % 100 < (only == knob ? 70u : pct); };
    if (on(0, 200)) f.has_driver = 0;
    if (on(1, 201)) f.pinfo_null = 1;
    if (on(2, 202)) f.can_drive_by = Pick(std::array<u32, 6>{0u, 2u, 0x80u, 0xFFu, 0x100u, 0x101u}, c, 203);
    if (on(3, 204)) f.pad_null = 1;
    if (on(4, 205)) f.slot = Pick(std::array<u32, 14>{0u, 1u, 4u, 12u, 7u, 14u, 50u, 100u, 127u, 128u, 129u, 200u, 244u, 255u}, c, 206);
    if (on(5, 207)) f.wtype = Rand(c, 208) % 4 == 0 ? Rand(c, 209) : Rand(c, 210) % 60;
    if (on(6, 211)) f.info1_slot = Pick(std::array<u32, 9>{0u, 3u, 5u, 0xFFFFFFFCu, 0x80000004u, 0x100u, 0x14u, 0x4000000u, 0xFFFFFFFFu}, c, 212);
    if (on(7, 213)) f.vsub = BmxOrOther(c, 214);
    if (on(8, 215)) f.vflags = R(216) & 0xFFu;
    if (on(9, 217)) f.gun_ret = Pick(std::array<u32, 10>{0u, 1u, 2u, 0xFFFFu, 0x8000u, 0x100u, 0x10000u, 0xFFFF0000u, 0x12340000u, 0xFFFFFFFFu}, c, 218) + (Rand(c, 219) & 1 ? Rand(c, 220) & 0xFFFF0000u : 0u);
    if (on(10, 221)) { f.ride1 = R(222) % 3; f.ride2 = 1 + R(223) % 2; f.ride_a_group = Rand(c, 224) % 3 ? R(225) % 20 : R(226); f.ride_b_group = Rand(c, 227) % 3 ? R(228) % 20 : R(229); }
    if (on(11, 230)) {
        f.cam_active = Rand(c, 231) % 12 == 0 ? 3u + R(232) % 38 : R(233) % 3;
        f.cam_flag = Pick(std::array<u32, 5>{0u, 1u, 2u, 0x80u, 0xFFu}, c, 234);
        for (int i = 0; i < 3; ++i) f.cam_mode[i] = Rand(c, 235 + i) % 3 == 0 ? Pick(std::array<u32, 7>{1u, 0x31u, 0x10001u, 0x30031u, 0x10031u, 0u, 0x2u}, c, 238 + i) : 2u + Rand(c, 241 + i) % 60u;
    }
    if (on(12, 245)) {
        f.ll_ret = Pick(std::array<u32, 6>{0u, 1u, 2u, 0x80u, 0xFFu, 0x100u}, c, 246) | (Rand(c, 247) & 1 ? Garbage(c, 248) : 0u);
        f.lr_ret = Pick(std::array<u32, 6>{0u, 1u, 2u, 0x80u, 0xFFu, 0x100u}, c, 249) | (Rand(c, 250) & 1 ? Garbage(c, 251) : 0u);
        for (int i = 0; i < 3; ++i) {
            f.cam_ll[i] = Pick(std::array<u32, 6>{0u, 1u, 2u, 0x80u, 0xFFu, 0x100u}, c, 252 + i);
            f.cam_lr[i] = Pick(std::array<u32, 6>{0u, 1u, 2u, 0x80u, 0xFFu, 0x100u}, c, 255 + i);
        }
    }
    if (on(13, 258)) {
        const u32 clip = f.info2_clip & 0xFFFFu;
        switch (Rand(c, 259) % 12) {
        case 0: f.total = 0; break;
        case 1: f.total = 1; break;
        case 2: f.total = clip; break;
        case 3: f.total = clip + 1; break;
        case 4: f.total = clip - 1; break;
        case 5: f.total = 0x80000000u; break;
        case 6: f.total = 0xFFFFFFFFu; break;
        case 7: f.total = 0x7FFFFFFFu; break;
        case 8: f.total = 0xFFFFu; break;
        case 9: f.total = Rand(c, 260); break;
        case 10: f.total = u32(i32(0) - i32(1 + Rand(c, 261) % 100)); break;
        default: f.total = 25001u; break;
        }
        f.clip0 = Pick(std::array<u32, 6>{0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 17u}, c, 262);
    }
    if (on(14, 263)) f.info2_clip = Pick(std::array<u32, 12>{0u, 1u, 2u, 17u, 0x7FFFu, 0x8000u, 0xFFFFu, 0xFFFEu, 0x100u, 0x7FFEu, 0x8001u, 1000u}, c, 264) | (Rand(c, 265) & 1 ? Rand(c, 266) & 0xFFFF0000u : 0u);
    if (on(15, 267)) {
        switch (Rand(c, 268) % 10) {
        case 0: f.nextshot = f.now; break;
        case 1: f.nextshot = f.now + 1; break;
        case 2: f.nextshot = f.now - 1; break;
        case 3: f.nextshot = 0; f.now = 0; break;
        case 4: f.nextshot = 0xFFFFFFFFu; f.now = 0; break;
        case 5: f.nextshot = 0; f.now = 0xFFFFFFFFu; break;
        case 6: f.nextshot = 0x80000000u; f.now = 0x7FFFFFFFu; break;
        case 7: f.nextshot = 0x7FFFFFFFu; f.now = 0x80000000u; break;
        case 8: f.nextshot = Rand(c, 269); break;
        default: f.nextshot = f.now + 0x46u; break;
        }
    }
    if (on(16, 270)) {
        for (int i = 0; i < 3; ++i) f.get_sel[i] = Rand(c, 271 + i) % 3 == 0 ? 0u : 1u + Rand(c, 274 + i) % 4;
        f.blend_sel = Rand(c, 277) % 3 == 0 ? 0u : 1u + Rand(c, 278) % 4;
    }
    if (on(17, 279)) for (int i = 0; i < 4; ++i) f.assoc[i].flags = Rand(c, 280 + i) % 3 == 0 ? Rand(c, 284 + i) & 0xFFFFu : Pick(std::array<u32, 6>{0u, 1u, 0x1u, 0x10u, 0xFFFEu, 0xFFFFu}, c, 288 + i);
    if (on(18, 292)) for (int i = 0; i < 4; ++i) f.assoc[i].amount = Rand(c, 293 + i) % 4 == 0 ? Rand(c, 297 + i) : Pick(kFlt, c, 301 + i);
    if (on(19, 305)) for (int i = 0; i < 4; ++i) f.assoc[i].delta = Rand(c, 306 + i) % 4 == 0 ? Rand(c, 310 + i) : Pick(kFlt, c, 314 + i);
    if (on(20, 318)) f.clump = Pick(std::array<u32, 5>{0u, 0xFFFFFFFFu, 0x80000000u, 0x1u, Rand(c, 319)}, c, 320);
    if (on(21, 321)) f.info_map = R(322) % 4;
}

inline void AddMuts(C& f, u32 c, u32 n, u32 maxAfter) {
    f.nmut = n > 2 ? 2 : n;
    for (u32 i = 0; i < f.nmut; ++i) {
        f.mut[i].after = Rand(c, 400 + i) % maxAfter;
        f.mut[i].target = Rand(c, 402 + i) % kMutTargets;
        f.mut[i].value = AnyU32(c, 404 + i);
        // La ranura activa se lee tras el segundo evento: antes de el, cambiarla cambiaria de arma (el contenido de las otras ranuras no esta definido por el caso)
        if (f.mut[i].target == 5 && f.mut[i].after < 2) f.mut[i].after += 2;
    }
}

inline C MakeC(u32 fn, unsigned n) {
    C f{};
    f.fn = fn;
    f.family = n % kFamilies;
    const u32 c = n * 8u + 0x80003u + fn * 0x1000193u;
    f.seed = Rand(c, 6) | 1u;
    f.eax = SeedReg(c, 7); f.ecx = SeedReg(c, 8); f.edx = SeedReg(c, 9); f.stack = StackSeed(Rand(c, 11) % 12);
    Base(f, c);
    switch (f.family) {
    case 0: Perturb(f, c, 12, 99); break;                          // camino de disparo con pocas rupturas
    case 1: Perturb(f, c, 0, Rand(c, 5) % 5 < 3 ? (Rand(c, 4) % 4) : 6u); break;   // salidas anticipadas (conductor, informacion, mando, subtipo del arma)
    case 2: Perturb(f, c, 0, Rand(c, 4) % 2 ? 7 : 9); break;     // subtipo del vehiculo y retorno del disparo
    case 3: Perturb(f, c, 0, 10); break;                           // datos de animacion de montar
    case 4: Perturb(f, c, 0, 11); break;                           // camara
    case 5: Perturb(f, c, 0, 12);                                  // mirar a un lado (a menudo sin mirar a la izquierda, para que salgan derecha y fija)
            for (int i = 0; i < 3; ++i) if (Rand(c, 350 + i) % 2) f.cam_ll[i] = 0;
            if (Rand(c, 353) % 2) f.ll_ret = Garbage(c, 354);
            break;
    case 6: Perturb(f, c, 0, Rand(c, 4) % 2 ? 13 : 14);            // municion y cargador (a menudo sin disparo: recarga y retirada de las animaciones)
            if (Rand(c, 355) % 3 != 0) { f.gun_ret = Rand(c, 356) & 0xFFFF0000u; f.ll_ret = Garbage(c, 357); f.lr_ret = Garbage(c, 358); for (int i = 0; i < 3; ++i) { f.cam_ll[i] = 0; f.cam_lr[i] = 0; } }
            break;
    case 7: Perturb(f, c, 0, Rand(c, 4) % 2 ? 16 : 19); break;    // existencia de la animacion y delta
    case 8: Perturb(f, c, 0, Rand(c, 4) % 2 ? 17 : 18); break;    // banderas e intensidad de la mezcla
    case 9: Perturb(f, c, 0, 15); break;                           // reloj y proximo disparo
    case 10: break;                                                // camino completo de disparo sin rupturas
    case 11: Perturb(f, c, 8, 99); AddMuts(f, c, 1 + Rand(c, 5) % 2, 5); break;      // mutaciones tras los primeros eventos
    case 12: Perturb(f, c, 8, 99); AddMuts(f, c, 1 + Rand(c, 5) % 2, 13); break;     // mutaciones tras cualquier evento
    case 13: Perturb(f, c, 0, Rand(c, 4) % 2 ? 4 : 5); break;     // ranura y tipo del arma
    case 14: Perturb(f, c, 20, 99); break;                         // registros y pila sucios
    case 15: Perturb(f, c, 0, Rand(c, 4) % 2 ? 18 : 19); for (int i = 0; i < 4; ++i) { if (Rand(c, 330 + i) % 2) f.assoc[i].delta = Pick(kFlt, c, 334 + i); if (Rand(c, 338 + i) % 2) f.assoc[i].amount = Pick(kFlt, c, 342 + i); } break;   // flotantes especiales
    case 16: Perturb(f, c, 0, 8); if (Rand(c, 4) % 2) f.ride1 = 0;                // banderas del vehiculo / vehiculo bajo
             if (Rand(c, 359) % 3 == 0) { f.ll_ret = Garbage(c, 360); f.lr_ret = Garbage(c, 361); for (int i = 0; i < 3; ++i) { f.cam_ll[i] = 0; f.cam_lr[i] = Rand(c, 362 + i) % 2; } }
             break;
    case 17: f.vsub = 10; f.ride1 = 1 + Rand(c, 5) % 2; Perturb(f, c, 15, 99);          // bici con datos de montar (a menudo sin mirar a un lado: animacion fija)
             if (Rand(c, 365) % 2) { f.ll_ret = Garbage(c, 366); f.lr_ret = Garbage(c, 367); for (int i = 0; i < 3; ++i) { f.cam_ll[i] = 0; f.cam_lr[i] = 0; } }
             break;
    case 18: Perturb(f, c, 35, 99); if (Rand(c, 5) % 3 == 0) AddMuts(f, c, 1 + Rand(c, 6) % 2, 13); break;      // mezcla
    default: Perturb(f, c, 55, 99); if (Rand(c, 5) % 3 == 0) AddMuts(f, c, 1 + Rand(c, 6) % 2, 13); break;      // mezcla densa
    }
    // Las mutaciones tambien aparecen (con poca probabilidad) en las demas familias
    if (f.nmut == 0 && f.family != 10 && Rand(c, 7) % 10 == 0) AddMuts(f, c, 1, 13);
    return f;
}
inline std::string JsonC(const C& c) {
    Js j; j.U("fn", c.fn).U("family", c.family).U("seed", c.seed)
     .U("vsub", c.vsub).U("vflags", c.vflags).U("has_driver", c.has_driver).U("pinfo_null", c.pinfo_null).U("can_drive_by", c.can_drive_by).U("pad_null", c.pad_null)
     .U("slot", c.slot).U("wtype", c.wtype).U("wstate", c.wstate).U("clip0", c.clip0).U("total", c.total).U("nextshot", c.nextshot)
     .U("info1_slot", c.info1_slot).U("info2_clip", c.info2_clip).U("info_map", c.info_map).U("infoa_clip", c.infoa_clip).U("infob_slot", c.infob_slot)
     .U("gun_ret", c.gun_ret).U("ll_ret", c.ll_ret).U("lr_ret", c.lr_ret)
     .U("ride1", c.ride1).U("ride2", c.ride2).U("ride_a_group", c.ride_a_group).U("ride_b_group", c.ride_b_group)
     .U("cam_active", c.cam_active).U("cam_flag", c.cam_flag)
     .A("cam_mode", {c.cam_mode[0], c.cam_mode[1], c.cam_mode[2]}).A("cam_ll", {c.cam_ll[0], c.cam_ll[1], c.cam_ll[2]}).A("cam_lr", {c.cam_lr[0], c.cam_lr[1], c.cam_lr[2]})
     .U("now", c.now).A("get_sel", {c.get_sel[0], c.get_sel[1], c.get_sel[2]}).U("blend_sel", c.blend_sel)
     .A("assoc", {c.assoc[0].flags, c.assoc[0].amount, c.assoc[0].delta, c.assoc[1].flags, c.assoc[1].amount, c.assoc[1].delta,
                  c.assoc[2].flags, c.assoc[2].amount, c.assoc[2].delta, c.assoc[3].flags, c.assoc[3].amount, c.assoc[3].delta})
     .U("clump", c.clump).U("nmut", c.nmut).A("mut", {c.mut[0].after, c.mut[0].target, c.mut[0].value, c.mut[1].after, c.mut[1].target, c.mut[1].value})
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}
}

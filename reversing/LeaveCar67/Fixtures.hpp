#pragma once
// Fixtures del banco LeaveCar67 (lote 67): CTaskSimpleGangDriveBy::SetupAnim (helper de animacion sin simbolo, 0x627B20), 16384 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskSimpleGangDriveBy::SetupAnim  0x627B20  thiscall, ECX = this (tarea), (CPed*), `ret 4`, AL = hay animacion con peso de mezcla > 0.9 y delta >= 0   (hook CTaskSimpleGangDriveBy / SetupAnim)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia).
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

namespace Lc67Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

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
inline u32 FBits(float f) { return std::bit_cast<u32>(f); }
inline float F(u32 b) { return std::bit_cast<float>(b); }
inline u32 B(float f) { return std::bit_cast<u32>(f); }

// ------------------------------------------------------------------------------------------------ JSON minimo (mismas filas `input` en el juego y en cualquier espejo)
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ valores notables
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0xFFC00001u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7F800001u, 0x7FA00000u, 0xFFA00000u, 0xFF800001u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 8> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u};
// Alrededor del umbral 0.9f (0x3F666666, 0x858C20) del peso de mezcla
inline constexpr std::array<u32, 14> kAmp{0x3F666666u, 0x3F666665u, 0x3F666667u, 0x3F800000u, 0x3F000000u, 0x00000000u, 0x80000000u, 0x00000001u, 0x007FFFFFu, 0x3F333333u, 0x40000000u, 0xBF800000u, 0x3F7FFFFFu, 0x3F666660u};
// Alrededor del umbral 0.0f (0x858B50) del delta de mezcla
inline constexpr std::array<u32, 14> kDel{0x00000000u, 0x80000000u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x3F800000u, 0xBF800000u, 0x3727C5ACu, 0xB727C5ACu, 0x40800000u, 0xC0800000u, 0x00800000u, 0x80800000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Resultado de bool de un colaborador: falso = AL 0 con basura en los bits altos, o AL != 0 con basura en los bits altos
inline u32 BoolRet(u32 n, u32 k, bool wantTrue) {
    static constexpr u32 als[]{1u, 2u, 0x80u, 0xFFu, 0x7Fu, 0x81u};
    if (!wantTrue) return Rand(n, k + 1) % 2 ? 0u : (Rand(n, k + 2) & 0xFFFFFF00u);
    return (Rand(n, k + 3) % 2 ? 0u : (Rand(n, k + 4) & 0xFFFFFF00u)) | als[Rand(n, k + 5) % std::size(als)];
}
// Valor de un byte: canonico o notable (bytes de banderas/ordenes con signo)
inline u32 ByteOf(u32 n, u32 k, std::initializer_list<u32> special) {
    const u32 r = Rand(n, k) % 8;
    if (r < 5) return special.begin()[Rand(n, k + 1) % special.size()] & 0xFFu;
    return Rand(n, k + 2) & 0xFFu;
}

// ------------------------------------------------------------------------------------------------ arenas (suma a mano: guarda + ventana + guarda)
constexpr u32 kGuard = 0x40;
constexpr u32 kTaskWindow = 0x44, kTaskArena = kGuard + kTaskWindow + kGuard;               // 0x40 + 0x44 + 0x40 = 0xC4    CTaskSimpleGangDriveBy
constexpr u32 kPedWindow = 0x40, kPedArena = kGuard + kPedWindow + kGuard;                  // 0x40 + 0x40 + 0x40 = 0xC0    ped falso (solo se lee +0x18, el clump)
constexpr u32 kAssocWindow = 0x40, kAssocArena = kGuard + kAssocWindow + kGuard;            // 0x40 + 0x40 + 0x40 = 0xC0    CAnimBlendAssociation (peso +0x18, delta +0x1C, id +0x2C, banderas +0x2E)
constexpr u32 kWeaponWindow = 0x40, kWeaponArena = kGuard + kWeaponWindow + kGuard;         // 0x40 + 0x40 + 0x40 = 0xC0    CWeaponInfo (cargador en +0x20)
constexpr u32 kBlockWindow = 0x20, kBlockArena = kGuard + kBlockWindow + kGuard;            // 0x40 + 0x20 + 0x40 = 0xA0    CAnimBlock (IsLoaded en +0x10)
constexpr u32 kGroupCount = 256, kGroupStride = 0x14;
constexpr u32 kGroupsWindow = kGroupCount * kGroupStride, kGroupsArena = kGuard + kGroupsWindow + kGuard;   // 0x40 + 0x1400 + 0x40 = 0x1480  ms_aAnimAssocGroups falso
static_assert(kTaskArena == 0xC4 && kPedArena == 0xC0 && kAssocArena == 0xC0 && kWeaponArena == 0xC0 && kBlockArena == 0xA0 && kGroupsArena == 0x1480, "arenas");
// Memoria global del juego que el banco toca (se guarda al empezar y se restaura tras cada lado)
constexpr u32 kGroupsPtr = 0xB4EA34;                                                       // CAnimManager::ms_aAnimAssocGroups (puntero)
constexpr u32 kBlocksBase = 0xB5D4A0;                                                      // CAnimManager::ms_aAnimBlocks
constexpr u32 kModelBase = 0x63E7;                                                         // IFPToModelId(0) = 25575
constexpr u32 kCbDefault = 0x4D1390, kCbFinish = 0x621BE0;                                 // callbacks que la funcion instala
constexpr u32 kMagicTime = 0x8D2E7C;                                                       // global float que se pasa a SetCurrentTime

// ================================================================================================ 0: SetupAnim
struct C {
    u32 family{}, group{}, viaGroup{}, blk{}, loaded{}, refd{}, reqId{}, hasAssoc{}, aId{}, aFlags{}, aAmp{}, aDel{}, bId{}, bFlags{}, bAmp{}, bDel{};
    u32 next{}, last{}, burst{}, timer{}, inRange{}, wKind{}, clip{}, clump{}, isPlayerRet{}, randRet{}, seed{};
    u32 eax{}, edx{}, stack{};
};
inline u32 Sext16(u32 v) { return u32(i32(i16(v & 0xFFFFu))); }
inline C MakeC(u32 n) {
    C c; const u32 f = n % 16; c.family = f;
    // grupo: casi siempre cualquiera de los 256 del arreglo falso; algunos son los que usa el tiroteo (72..75)
    c.group = Rand(n, 1) % 5 == 0 ? Pick(std::array<u32, 8>{72u, 73u, 74u, 75u, 0u, 255u, 1u, 128u}, n, 2) : Rand(n, 3) % kGroupCount;
    c.viaGroup = Rand(n, 4) % 2;                                         // 1: la entrada del grupo trae el bloque; 0: entrada nula (se busca por nombre)
    c.blk = Rand(n, 5) % 100 < 70 ? 0u : (Rand(n, 6) % 2 ? 1u : 2u);       // 0 bloque de la arena, 1 bloque del EXE con IsLoaded == 1, 2 bloque del EXE con IsLoaded != 1
    c.loaded = Rand(n, 7) % 100 < 60 ? 1u : Pick(std::array<u32, 6>{0u, 2u, 0x80u, 0xFFu, 3u, 0x101u}, n, 8) & 0xFFu;   // IsLoaded del bloque de la arena (solo importa el byte)
    c.refd = ByteOf(n, 9, {0u, 0u, 1u, 2u, 0x80u, 0xFFu});
    // animacion requerida y animacion actual
    const u32 mode = Rand(n, 10) % 10;                                     // relacion entre el id de la asociacion actual y el requerido
    c.reqId = Rand(n, 11) % 8 == 0 ? 0xBFu : (Rand(n, 12) % 4 == 0 ? Rand(n, 13) : (Rand(n, 14) % 2 ? Rand(n, 15) % 0xC0u : Sext16(Rand(n, 16))));
    c.hasAssoc = Rand(n, 17) % 100 < 60;
    c.aId = mode < 4 ? c.reqId & 0xFFFFu : (mode < 6 ? (c.reqId + 0x10000u) & 0xFFFFu : Rand(n, 18) & 0xFFFFu);
    if (mode == 6) c.reqId = Sext16(c.reqId), c.aId = c.reqId & 0xFFFFu;                      // igual con signo en 16 bits
    if (mode == 7) c.reqId = (c.reqId & 0xFFFFu) | 0x10000u, c.aId = c.reqId & 0xFFFFu;       // bajos iguales, dword distinto: comparacion de 32 bits tras movsx
    c.aFlags = Rand(n, 19) % 3 == 0 ? Rand(n, 20) & 0xFFFFu : (Rand(n, 21) % 2 ? 1u : 0u) | (Rand(n, 22) & 0xFFFEu);
    c.bFlags = Rand(n, 23) % 3 == 0 ? Rand(n, 24) & 0xFFFFu : (Rand(n, 25) % 2 ? 1u : 0u) | (Rand(n, 26) & 0xFFFEu);
    c.bId = Rand(n, 27) & 0xFFFFu;
    auto amp = [&](u32 k) { const u32 r = Rand(n, k) % 10; return r < 5 ? Pick(kAmp, n, k + 1) : r < 8 ? RandFloat(n, k + 2, -4, 2) : r == 8 ? Pick(kNan, n, k + 3) : Pick(kInf, n, k + 4); };
    auto del = [&](u32 k) { const u32 r = Rand(n, k) % 10; return r < 5 ? Pick(kDel, n, k + 1) : r < 8 ? RandFloat(n, k + 2, -20, 4) : r == 8 ? Pick(kNan, n, k + 3) : Pick(kTiny, n, k + 4); };
    c.aAmp = amp(30); c.aDel = del(40); c.bAmp = amp(50); c.bDel = del(60);
    // ordenes: bytes con signo (char)
    c.next = ByteOf(n, 70, {0u, 1u, 2u, 3u, 3u, 4u, 5u, 6u, 7u, 0x7Fu, 0x80u, 0xFFu});
    c.last = ByteOf(n, 74, {0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 0x7Fu, 0x80u, 0xFFu, 0xFEu});
    c.burst = ByteOf(n, 78, {0u, 1u, 2u, 3u, 0x7Fu, 0x80u, 0xFFu, 0xFFu, 1u});
    c.timer = Rand(n, 82) % 10 < 6 ? Pick(std::array<u32, 12>{0u, 1u, 0xFFFFu, 0x7FFFu, 0x8000u, 100u, 2u, 0xFFFEu, 0x8001u, 200u, 0xFF00u, 0x00FFu}, n, 83) : Rand(n, 84) & 0xFFFFu;
    c.inRange = ByteOf(n, 85, {0u, 1u, 1u, 1u, 2u, 0x80u, 0xFFu});
    c.wKind = 1;
    c.clip = (Rand(n, 86) % 10 < 7 ? Pick(std::array<u32, 14>{0u, 1u, 2u, 3u, 4u, 30u, 0x7FFFu, 0x8000u, 0xFFFFu, 0xFFFEu, 100u, 0x7FFEu, 0x8001u, 17u}, n, 87) : Rand(n, 88) & 0xFFFFu)
           | (Rand(n, 89) % 3 == 0 ? 0u : Rand(n, 90) % 3 == 0 ? 0xFFFF0000u : (Rand(n, 91) & 0xFFFF0000u));
    c.clump = Rand(n, 92) % 6 == 0 ? 0u : (Rand(n, 93) | 0x10000u);
    c.isPlayerRet = BoolRet(n, 94, Rand(n, 95) % 2);
    c.randRet = Rand(n, 96) % 3 == 0 ? Pick(std::array<u32, 10>{0u, 0xFFFFu, 0x10000u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0x7FFF8000u, 0x8000u, 0x7FFFu, 0x4000u}, n, 97) : Rand(n, 98);
    c.seed = Rand(n, 99);

    // ---- familias: fuerzan una ruta distinta de la funcion (el resto de campos siguen aleatorios)
    auto signedLeZero = [&](u32 k) { return Pick(std::array<u32, 8>{0u, 0xFFFFu, 0x8000u, 0x8001u, 0xFFF0u, 0xFF00u, 0xFFFEu, 0x9000u}, n, k); };
    auto positive = [&](u32 k) { return Pick(std::array<u32, 8>{1u, 2u, 100u, 0x7FFFu, 0x7FFEu, 0x1234u, 5u, 0x4000u}, n, k); };
    auto loadedOk = [&] { c.blk = 0; c.loaded = 1; };
    switch (f) {
    case 0: c.viaGroup = 1; c.blk = 0; c.loaded = Pick(std::array<u32, 6>{0u, 2u, 0x80u, 0xFFu, 3u, 0x7Fu}, n, 100); break;       // bloque de la arena no cargado (por el grupo)
    case 1: c.viaGroup = 0; c.blk = 0; c.loaded = Pick(std::array<u32, 6>{0u, 2u, 0x80u, 0xFFu, 3u, 0x7Fu}, n, 100); break;       // no cargado (buscado por nombre)
    case 2: c.blk = 2; break;                                                                                                       // bloque del EXE con IsLoaded != 1, indice negativo
    case 3: loadedOk(); c.refd = 0; break;                                                                                         // cargado, sin referencia: la anade
    case 4: loadedOk(); c.refd = Pick(std::array<u32, 5>{1u, 2u, 0x80u, 0xFFu, 0x10u}, n, 101); break;                               // cargado, ya referenciado
    case 5: loadedOk(); c.reqId = 0xBFu; c.hasAssoc = 0; break;                                                                     // sin animacion definida y sin asociacion
    case 6: loadedOk(); c.reqId = 0xBFu; c.hasAssoc = 1; break;                                                                     // sin animacion definida pero con asociacion
    case 7: loadedOk(); c.hasAssoc = 0; c.reqId = Rand(n, 102) % 2 ? Rand(n, 103) % 0xBF : c.reqId; if (c.reqId == 0xBF) c.reqId = 0x10; c.timer = signedLeZero(104); c.inRange = 1 + Rand(n, 105) % 255; break;   // sin asociacion: crea
    case 8: loadedOk(); c.hasAssoc = 0; if (c.reqId == 0xBF) c.reqId = 0x11; if (Rand(n, 106) % 2) c.timer = positive(107); else c.inRange = 0; break;                                                        // sin asociacion: no crea
    case 9: loadedOk(); c.hasAssoc = 1; c.reqId = Sext16(Rand(n, 108) % 2 ? Rand(n, 109) : Rand(n, 109) % 0xC0); if (c.reqId == 0xBF) c.reqId = 0x12; c.aId = c.reqId & 0xFFFFu; break;                  // misma animacion: no reemplaza
    case 10: loadedOk(); c.hasAssoc = 1; if (c.reqId == 0xBF) c.reqId = 0x13; c.aId = (c.reqId + 1u + Rand(n, 110) % 0xFFFEu) & 0xFFFFu; if (c.aId == (c.reqId & 0xFFFFu)) c.aId ^= 0x100u;
             c.next = 1 + Rand(n, 111) % 3; c.last = Pick(std::array<u32, 8>{0u, 1u, 2u, 3u, 0x80u, 0xFFu, 0xFEu, 0x7Fu}, n, 112); if (c.last == 0x7Fu) c.last = 3; break;   // con asociacion distinta: reemplaza
    case 11: loadedOk(); c.hasAssoc = 1; if (c.reqId == 0xBF) c.reqId = 0x14; c.aId = (c.reqId + 1u + Rand(n, 113) % 0xFFFEu) & 0xFFFFu; if (c.aId == (c.reqId & 0xFFFFu)) c.aId ^= 0x100u;
             if (Rand(n, 114) % 2) c.next = Pick(std::array<u32, 8>{0u, 4u, 5u, 7u, 0x80u, 0xFFu, 0x7Fu, 0xF3u}, n, 115); else { c.next = 1 + Rand(n, 116) % 3; c.last = Pick(std::array<u32, 5>{4u, 5u, 7u, 0x7Fu, 6u}, n, 117); } break;   // distinta pero no reemplaza
    case 12: loadedOk(); if (c.reqId == 0xBF) c.reqId = 0x15; c.next = 3; c.burst = Pick(std::array<u32, 8>{0u, 1u, 0x80u, 0xFFu, 0xFEu, 0x81u, 2u, 0x7Fu}, n, 118);
             if (c.hasAssoc) { c.aId = (c.reqId + 1u + Rand(n, 119) % 0xFFFEu) & 0xFFFFu; if (c.aId == (c.reqId & 0xFFFFu)) c.aId ^= 0x100u; c.last = Pick(std::array<u32, 6>{0u, 1u, 2u, 3u, 0x80u, 0xFFu}, n, 120); }
             else { c.timer = signedLeZero(121); c.inRange = 1 + Rand(n, 122) % 255; } break;                                                                                            // reemplaza con orden 3: rafaga aleatoria
    case 13: loadedOk(); if (c.reqId == 0xBF) c.reqId = 0x16; if (Rand(n, 123) % 2) { c.bAmp = Pick(kAmp, n, 124); c.bDel = Pick(kDel, n, 125); c.hasAssoc = 0; c.timer = signedLeZero(126); c.inRange = 1; }
             else { c.aAmp = Pick(kAmp, n, 124); c.aDel = Pick(kDel, n, 125); c.hasAssoc = 1; c.aId = c.reqId & 0xFFFFu; c.reqId = Sext16(c.reqId); c.aId = c.reqId & 0xFFFFu; } break;   // umbrales del resultado
    case 14: loadedOk(); if (c.reqId == 0xBF) c.reqId = 0x17; c.aAmp = Rand(n, 127) % 2 ? Pick(kNan, n, 128) : Pick(kInf, n, 129); c.bAmp = Rand(n, 130) % 2 ? Pick(kNan, n, 131) : Pick(kInf, n, 132);
             c.aDel = Pick(kTiny, n, 133); c.bDel = Rand(n, 134) % 2 ? Pick(kTiny, n, 135) : Pick(kNan, n, 136);
             if (Rand(n, 137) % 3 == 0) { c.aAmp = Pick(kAmp, n, 138); c.bAmp = Pick(kAmp, n, 139); } break;                                                                       // NaN, infinitos y subnormales
    default: break;                                                                                                                                                                      // 15: mezcla
    }
    // el arma solo se lee si se sortea la rafaga: nulo si no se llega (orden distinta de 3 o rafaga > 1 con signo)
    const bool burstReads = (c.next & 0xFFu) == 3u && i8(c.burst & 0xFFu) <= 1;
    c.wKind = (!burstReads && Rand(n, 140) % 3 == 0) ? 0u : 1u;
    c.eax = SeedReg(n, 70); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC(const C& c) {
    Js j; j.U("family", c.family).U("group", c.group).U("via_group", c.viaGroup).U("blk", c.blk).U("loaded", c.loaded).U("refd", c.refd).U("req_id", c.reqId).U("has_assoc", c.hasAssoc)
     .U("a_id", c.aId).U("a_flags", c.aFlags).U("a_amp", c.aAmp).U("a_del", c.aDel).U("b_id", c.bId).U("b_flags", c.bFlags).U("b_amp", c.bAmp).U("b_del", c.bDel)
     .U("next", c.next).U("last", c.last).U("burst", c.burst).U("timer", c.timer).U("in_range", c.inRange).U("w_kind", c.wKind).U("clip", c.clip).U("clump", c.clump)
     .U("is_player_ret", c.isPlayerRet).U("rand_ret", c.randRet).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}
}

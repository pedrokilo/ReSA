#pragma once
// Fixtures del banco PlayerAnim80D (lote 80D): CPlayerPed::ProcessAnimGroups (EXE 0x6098F0, thiscall, ECX = jugador, sin argumentos, `ret`, void).
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CPlayerPed::ProcessAnimGroups  0x6098F0  ()   (hook CPlayerPed / ProcessAnimGroups, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: n < 16384: familia = n % 16 (1024 casos por familia); despues las familias 16..19 (ver MakeExtra).
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

namespace Pa80Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;   // familias 0..15 (n % 16), 1024 casos cada una
constexpr unsigned kFamHeading = 1024;   // familia 16: rumbo (+0x50) con NaN, infinitos, desnormales, +-0, los limites exactos y sus vecinos
constexpr unsigned kFamModel = 1024;     // familia 17: tipo de modelo del arma (AL + basura) y tipo de arma de 0 a 0x30
constexpr unsigned kFamMut = 1024;       // familia 18: mutaciones entre llamadas: 15 sitios x 31 tipos (barrido) + aleatorias
constexpr unsigned kFamSlot = 1024;      // familia 19: ranura de arma con signo (-20..20, byte) x tipo de pelea x grupos
constexpr unsigned kCases = kBaseCases + kFamHeading + kFamModel + kFamMut + kFamSlot;
constexpr unsigned kFamilies = 20;
constexpr unsigned kFunctions = 1;
constexpr unsigned kGroups = 24;         // grupos de animacion del arreglo falso (0..23)
constexpr unsigned kBlocks = 8;          // bloques reales de ms_aAnimBlocks que usa el banco (0..7)
constexpr unsigned kSlots = 41;          // ranuras -20..20 (se leen con signo, sin comprobar el rango)
constexpr unsigned kSites = 15;

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kPlayerWin = 0x800, kDataWin = 0x100, kSmallWin = 0x100, kGroupsWin = kGroups * 20;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kPlayerArena = Arena(kPlayerWin), kDataArena = Arena(kDataWin), kSmallArena = Arena(kSmallWin), kGroupsArena = Arena(kGroupsWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 80404u); }
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
// Valor devuelto en AL por un colaborador bool: AL (0 o no 0) y basura en el resto de EAX (a veces AL = 0 con basura arriba)
inline u32 BoolRet(u32 n, u32 k, bool al) {
    const u32 hi = Rand(n, k) % 3 == 0 ? 0u : Rand(n, k + 1) & 0xFFFFFF00u;
    const u32 lo = al ? (Rand(n, k + 2) % 3 == 0 ? 1u + Rand(n, k + 3) % 255u : 1u) : 0u;
    return hi | lo;
}
// Rumbos (+0x50) de los datos del jugador: dentro y fuera de +-0.8726646 (0x3F5F66F3 / 0xBF5F66F3)
inline u32 Heading(u32 c, u32 k) {
    static constexpr u32 sp[]{0x00000000u, 0x80000000u, 0x3F5F66F3u, 0xBF5F66F3u, 0x3F5F66F2u, 0x3F5F66F4u, 0xBF5F66F2u, 0xBF5F66F4u, 0x3F800000u, 0xBF800000u,
                              0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FBFFFFFu, 0x00000001u, 0x80000001u, 0x007FFFFFu,
                              0x807FFFFFu, 0x3F000000u, 0xBF000000u, 0x40490FDBu, 0xC0490FDBu, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x3F5F66F0u, 0xBF5F66F0u, 0x3F5F66F8u, 0xBF5F66F8u};
    const u32 r = Rand(c, k) % 10;
    if (r < 4) return sp[Rand(c, k + 1) % std::size(sp)];
    if (r == 4) return 0x7F800000u | (Rand(c, k + 2) & 0x807FFFFFu);   // NaN/inf con carga util y signo arbitrarios
    if (r < 7) return B(float(i32(Rand(c, k + 3) % 6000) - 3000) / 1000.0f);   // -3..3 rad
    return B(float(i32(Rand(c, k + 4) % 1600) - 800) / 1000.0f);               // -0.8..0.8: casi siempre dentro
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 hdg{};                                     // rumbo (+0x50 de los datos), patron de 32 bits
    u32 camIdx{};                                  // byte 0xB6F081 (camara activa)
    u32 mouse{}, strafe{};                         // retorno de Using3rdPersonMouseCam y de CanStrafeOrMouseControl (AL + basura)
    u32 animGroup{};                               // +0x4D4 inicial
    u32 wobj{}, clumpRet{}, mtype{}, wtype{};      // +0x4F4 presente; GetClumpModelInfo devuelve (0 = nulo, 1 = modelo); GetModelType (AL + basura); +0x24 del modelo
    u32 jet{}, bal{}, defGroup{};                  // GetTaskJetPack (puntero), GetIsWearingBalaclava (AL + basura), GetDefaultPlayerMotionGroup
    u32 pedType{}, midx{}, animType{};             // +0x598, +0x22 (palabra), +0x24 del modelo de peaton
    u32 slot{}, skill{};                           // +0x718 (byte), retorno de GetWeaponSkill
    u32 wt[kSlots]{};                              // tipos de arma de las ranuras -20..20 (+0x5A0 + ranura*0x1C)
    u32 fire{}, baseCombo{}, style{}, pflags{};    // CWeaponInfo +0 (disparo) y +0x6E (combo base, byte), +0x72D (estilo, byte), dword +0x46C
    u32 f48{}, f4C{}, f48b{}, f4Cb{};              // datos del jugador +0x48 y +0x4C (grupos de animacion referenciados) y los de los otros datos (tras una mutacion de puntero)
    u32 row[12]{};                                 // primer campo (grupo) de cada fila de la tabla de combos 0xC170D0
    u32 blk[kGroups]{};                            // bloque de cada grupo: 0 = nulo, 1 + k = bloque real k
    u32 loaded[kBlocks]{};                         // byte IsLoaded (+0x10) de los bloques reales 0..7
    u32 gb{};                                      // bloque que devuelve GetAnimationBlock (0..7)
    u32 mutSite{}, mutKind{}, mutA{}, mutB{}, mutG{}, mutBlk{}, mutAG{};
};

inline u32 GroupVal(u32 n, u32 k) { return Rand(n, k) % kGroups; }
inline u32 RefVal(u32 n, u32 k) {   // grupo referenciado actual: 0 (ninguno) o un grupo
    const u32 r = Rand(n, k) % 4;
    return r == 0 ? 0u : GroupVal(n, k + 1);
}
inline u32 WeaponTypeVal(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 0x19u, 0x1Bu, 0x1Eu, 0x1Fu, 0x21u, 0x22u, 0x23u, 0x24u, 0x25u, 0x26u, 0x36u, 0x39u, 0xFFFFFFFFu, 0x80000000u};
    const u32 r = Rand(n, k) % 3;
    return r == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2) % 0x30;
}
inline u32 AnimGroupVal(u32 n, u32 k) {
    const u32 r = Rand(n, k) % 6;
    if (r < 2) return 0x36;
    if (r == 2) return 0x37 + Rand(n, k + 1) % 0x20;
    if (r == 3) return Rand(n, k + 2);
    return Rand(n, k + 3) % 0x80;
}
inline u32 DefGroupVal(u32 n, u32 k) {
    static constexpr u32 sp[]{0x36u, 0x37u, 0x35u, 0x38u, 0x40u, 0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x36u + 6u};
    return Rand(n, k) % 2 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2) % 0x60;
}

inline C BaseDefaults(unsigned n) {
    C c;
    c.n = n; c.family = n % 16; c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.hdg = Heading(n, 10);
    c.camIdx = Rand(n, 14) % 8 == 0 ? Rand(n, 15) & 0xFFu : Rand(n, 16) % 3;
    c.mouse = BoolRet(n, 17, Rand(n, 18) % 2); c.strafe = BoolRet(n, 21, Rand(n, 22) % 2);
    c.animGroup = AnimGroupVal(n, 23);
    c.wobj = Rand(n, 28) % 4 != 0; c.clumpRet = Rand(n, 29) % 5 != 0;
    c.mtype = Rand(n, 30) % 3 ? (4u | (Rand(n, 31) % 2 ? 0u : Rand(n, 32) & 0xFFFFFF00u)) : (Rand(n, 33) % 2 ? Rand(n, 34) & 0xFFu : Rand(n, 35));
    c.wtype = WeaponTypeVal(n, 36);
    c.jet = Rand(n, 40) % 8 == 0 ? (Rand(n, 41) % 2 ? Rand(n, 42) | 1u : Rand(n, 43) << 8) : 0u;
    c.bal = BoolRet(n, 44, Rand(n, 45) % 4 == 0);
    c.defGroup = DefGroupVal(n, 46);
    c.pedType = Rand(n, 50) % 3 == 0 ? 1u : Rand(n, 51) % 3 ? 0u : Rand(n, 52) % 40;
    if (Rand(n, 53) % 8 == 0) c.pedType = 0x100u | (Rand(n, 54) & 1u);   // 0x101 / 0x100: solo el dword igual a 1 cuenta
    c.midx = Rand(n, 55) % 8;
    c.animType = Rand(n, 56) % 3 == 0 ? 0x36u : (Rand(n, 57) % 2 ? Rand(n, 58) % 0x80 : Rand(n, 59));
    c.slot = Rand(n, 60) % 4 == 0 ? u32(Rand(n, 61) % 41) - 20u : Rand(n, 62) % 13;
    c.slot &= 0xFFu;
    c.skill = Rand(n, 63) % 4;   // GetWeaponSkill devuelve un eWeaponSkill pequeno (0..3)
    for (unsigned i = 0; i < kSlots; ++i) c.wt[i] = WeaponTypeVal(n, 70 + i);
    c.fire = Rand(n, 120) % 3 == 0 ? 1 + Rand(n, 121) % 8 : 0u;
    c.baseCombo = Rand(n, 122) % 3 == 0 ? 4u : 4u + Rand(n, 123) % 12;
    c.style = Rand(n, 124) % 3 == 0 ? 4u : 4u + Rand(n, 125) % 12;
    c.pflags = Rand(n, 126); if (Rand(n, 127) % 4) c.pflags &= ~0x100u; else c.pflags |= 0x100u;
    c.f48 = RefVal(n, 130); c.f4C = RefVal(n, 133); c.f48b = RefVal(n, 136); c.f4Cb = RefVal(n, 138);
    for (unsigned i = 0; i < 12; ++i) c.row[i] = GroupVal(n, 140 + i);
    for (unsigned g = 0; g < kGroups; ++g) c.blk[g] = Rand(n, 160 + g) % 4 == 0 ? 0u : 1u + Rand(n, 190 + g) % kBlocks;
    static constexpr u32 ld[]{0u, 1u, 1u, 1u, 2u, 0xFFu, 0x80u, 3u};
    for (unsigned k = 0; k < kBlocks; ++k) c.loaded[k] = Rand(n, 220 + k) % 3 ? 1u : ld[Rand(n, 230 + k) % std::size(ld)];
    c.gb = Rand(n, 240) % kBlocks;
    return c;
}

inline void Outside(C& c, u32 n, u32 k) {   // rumbo que obliga a mirar la camara
    static constexpr u32 sp[]{0x3F5F66F3u, 0xBF5F66F3u, 0xBF800000u, 0x3F800000u, 0xC0000000u, 0x40000000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xBF5F66F2u, 0x3F5F66F4u};
    c.hdg = sp[Rand(n, k) % std::size(sp)];
}
inline void Inside(C& c, u32 n, u32 k) {    // rumbo que NO mira la camara
    static constexpr u32 sp[]{0x00000000u, 0x80000000u, 0x3F000000u, 0xBF000000u, 0x3F5F66F2u, 0xBF5F66F4u, 0x00000001u, 0x80000001u};
    c.hdg = sp[Rand(n, k) % std::size(sp)];
}
inline void CamStrafe(C& c, u32 n, u32 k) { c.mouse = BoolRet(n, k, true); c.strafe = BoolRet(n, k + 4, true); }

inline C MakeExtra(unsigned n) {
    C c = BaseDefaults(n);
    const unsigned j = n - kBaseCases;
    if (j < kFamHeading) {   // familia 16
        c.family = 16;
        c.hdg = Heading(n, 300); if (Rand(n, 301) % 2) c.hdg = Heading(n, 302);
        c.mouse = BoolRet(n, 303, Rand(n, 304) % 4 != 0); c.strafe = BoolRet(n, 305, Rand(n, 306) % 4 != 0);
        c.animGroup = Rand(n, 307) % 2 ? 0x36u : AnimGroupVal(n, 308);
        return c;
    }
    if (j < kFamHeading + kFamModel) {   // familia 17
        c.family = 17;
        const unsigned m = j - kFamHeading;
        Inside(c, n, 310);
        c.wobj = 1; c.clumpRet = 1; c.jet = 0;
        c.wtype = m % 0x30; if (Rand(n, 311) % 8 == 0) c.wtype = WeaponTypeVal(n, 312);
        static constexpr u32 mt[]{4u, 4u, 4u, 0x104u, 0xFFFFFF04u, 0x00000004u, 0x5u, 0x3u, 0u, 0x40u, 0x400u, 0x80000004u, 0x12345604u, 0xFFu, 0x6u, 0x7u};
        c.mtype = mt[(m / 3) % std::size(mt)];
        return c;
    }
    if (j < kFamHeading + kFamModel + kFamMut) {   // familia 18
        c.family = 18;
        const unsigned m = j - kFamHeading - kFamModel;
        // 15 sitios x 31 tipos = 465 combinaciones, dos pasadas y el resto aleatorio
        const unsigned combo = m < 930 ? m % 465 : Rand(n, 320) % 465;
        c.mutSite = 1 + combo % kSites; c.mutKind = 1 + (combo / kSites) % 31;
        c.mutA = GroupVal(n, 321); c.mutB = GroupVal(n, 322); c.mutG = GroupVal(n, 323);
        c.mutBlk = Rand(n, 324) % 3 == 0 ? 0u : 1u + Rand(n, 325) % kBlocks; c.mutAG = AnimGroupVal(n, 326);
        // rutas largas para que lleguen a todos los sitios: camara + extrangulacion + arma de la lista + pelea con grupos nuevos
        if (m % 2) { Outside(c, n, 327); CamStrafe(c, n, 328); c.strafe = BoolRet(n, 329, Rand(n, 330) % 2); }
        c.wobj = 1; c.clumpRet = 1; c.mtype = 4; c.jet = 0;
        c.wtype = Rand(n, 331) % 3 ? 0x30u : 5u;
        c.bal = BoolRet(n, 332, Rand(n, 333) % 2);
        c.fire = 0; c.pflags &= ~0x100u; c.baseCombo = 5u + Rand(n, 334) % 11; c.style = 5u + Rand(n, 335) % 11;
        c.f48 = Rand(n, 336) % 2 ? 0u : GroupVal(n, 337); c.f4C = Rand(n, 338) % 2 ? 0u : GroupVal(n, 339);
        return c;
    }
    c.family = 19;   // familia 19
    const unsigned m = j - kFamHeading - kFamModel - kFamMut;
    Inside(c, n, 340);
    c.wobj = 0; c.jet = 0;
    c.bal = 0;
    static constexpr u32 slots[]{0u, 1u, 2u, 3u, 4u, 5u, 12u, 0xFFu, 0xFEu, 0xFDu, 0xF0u, 0xECu, 14u, 20u, 19u, 0xECu};   // 0xEC = -20
    c.slot = slots[m % std::size(slots)];
    if (Rand(n, 341) % 2) c.slot = u32(i32(Rand(n, 342) % 41) - 20) & 0xFFu;
    c.fire = Rand(n, 343) % 2 ? 0u : 1u + Rand(n, 344) % 8;
    c.pflags = (Rand(n, 345) & ~0x100u) | (Rand(n, 346) % 4 == 0 ? 0x100u : 0u);
    return c;
}

inline C MakeC0(unsigned n) {
    if (n >= kBaseCases) return MakeExtra(n);
    C c = BaseDefaults(n);
    auto melee = [&] { c.wobj = 0; c.jet = 0; c.bal = 0; Inside(c, n, 400); c.fire = 0; c.pflags &= ~0x100u; };
    switch (c.family) {
    case 0: break;
    case 1: Outside(c, n, 401); CamStrafe(c, n, 402); c.animGroup = 0x36; break;
    case 2: Outside(c, n, 403); CamStrafe(c, n, 404); if (c.animGroup == 0x36) c.animGroup = 0x37 + Rand(n, 405) % 0x20; break;
    case 3: Outside(c, n, 406); c.mouse = BoolRet(n, 407, true); c.strafe = BoolRet(n, 408, false); break;
    case 4: Inside(c, n, 409); break;
    case 5: Inside(c, n, 410); c.wobj = 1; c.clumpRet = 1; c.mtype = 4; c.wtype = 0x23 + Rand(n, 411) % 2; break;
    case 6: Inside(c, n, 412); c.jet = Rand(n, 413) % 2 ? Rand(n, 414) | 1u : Rand(n, 415) << 8 | 0x100u; c.wobj = Rand(n, 416) % 2; break;
    case 7: { Inside(c, n, 417); c.wobj = 1; c.clumpRet = 1; c.mtype = 4; c.jet = 0;
              static constexpr u32 wl[]{5, 6, 7, 9, 0x25, 0x26, 0x1F, 0x1E, 0x1B, 0x19, 0x22, 0x21, 0x23, 0x24, 8, 0x20, 0x1A, 0x18, 0, 4};
              c.wtype = wl[Rand(n, 418) % std::size(wl)]; break; }
    case 8: Inside(c, n, 419); c.jet = 0; c.wobj = Rand(n, 420) % 2; c.pedType = Rand(n, 421) % 2 ? 1u : Rand(n, 422) % 5; break;
    case 9: Inside(c, n, 423); c.jet = 0; c.wobj = 0; c.bal = BoolRet(n, 424, false); c.animGroup = Rand(n, 425) % 3 ? 0x36u : AnimGroupVal(n, 426);
            c.pedType = 1; c.animType = Rand(n, 427) % 2 ? 0x36u : Rand(n, 428) % 0x80; break;
    case 10: Inside(c, n, 429); c.wobj = 0; c.jet = 0; c.bal = 0; c.fire = Rand(n, 430) % 2 ? 1u + Rand(n, 431) % 8 : 0u; c.pflags = (c.pflags & ~0x100u) | (c.fire == 0 ? 0x100u : (Rand(n, 432) % 2 ? 0x100u : 0u)); break;
    case 11: melee(); c.baseCombo = 4; c.style = 4; c.f48 = RefVal(n, 433); c.f4C = RefVal(n, 434); break;
    case 12: melee(); c.style = 4; c.baseCombo = 5u + Rand(n, 435) % 11; c.f4C = RefVal(n, 436);
             if (Rand(n, 437) % 3 == 0) c.f48 = c.row[c.baseCombo - 4]; break;
    case 13: melee(); c.baseCombo = 4; c.style = 5u + Rand(n, 438) % 11; c.f48 = RefVal(n, 439);
             if (Rand(n, 440) % 3 == 0) c.f4C = c.row[c.style - 4]; break;
    case 14: melee(); c.baseCombo = 5u + Rand(n, 441) % 11; c.style = 5u + Rand(n, 442) % 11;
             if (Rand(n, 443) % 4 == 0) c.f48 = c.row[c.baseCombo - 4]; if (Rand(n, 444) % 4 == 0) c.f4C = c.row[c.style - 4];
             if (Rand(n, 445) % 4 == 0) { for (auto& l : c.loaded) l = Rand(n, 446) % 2 ? 0u : 1u; } break;
    default:  // 15: mutaciones
        c.mutSite = 1 + Rand(n, 450) % kSites; c.mutKind = 1 + Rand(n, 451) % 31;
        c.mutA = GroupVal(n, 452); c.mutB = GroupVal(n, 453); c.mutG = GroupVal(n, 454);
        c.mutBlk = Rand(n, 455) % 3 == 0 ? 0u : 1u + Rand(n, 456) % kBlocks; c.mutAG = AnimGroupVal(n, 457);
        if (Rand(n, 458) % 2) { Outside(c, n, 459); CamStrafe(c, n, 460); }
        if (Rand(n, 461) % 2) { c.wobj = 1; c.clumpRet = 1; c.mtype = 4; c.jet = 0; }
        if (Rand(n, 462) % 2) { c.fire = 0; c.pflags &= ~0x100u; }
        break;
    }
    return c;
}

// La ranura de arma -11 (+0x5A0 - 11 * 0x1C = +0x46C) coincide con las banderas del peaton: el fixture iguala su tipo a las banderas
inline C MakeC(unsigned n) { C c = MakeC0(n); c.wt[9] = c.pflags; return c; }

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("hdg", c.hdg).U("cam_idx", c.camIdx).U("mouse", c.mouse).U("strafe", c.strafe).U("anim_group", c.animGroup);
    j.U("wobj", c.wobj).U("clump_ret", c.clumpRet).U("mtype", c.mtype).U("wtype", c.wtype).U("jet", c.jet).U("bal", c.bal).U("def_group", c.defGroup);
    j.U("ped_type", c.pedType).U("midx", c.midx).U("anim_type", c.animType).U("slot", c.slot).U("skill", c.skill);
    for (unsigned i = 0; i < kSlots; ++i) { const std::string a = "wt" + std::to_string(i); j.U(a.c_str(), c.wt[i]); }
    j.U("fire", c.fire).U("base_combo", c.baseCombo).U("style", c.style).U("pflags", c.pflags).U("f48", c.f48).U("f4c", c.f4C).U("f48b", c.f48b).U("f4cb", c.f4Cb);
    for (unsigned i = 0; i < 12; ++i) { const std::string a = "row" + std::to_string(i); j.U(a.c_str(), c.row[i]); }
    for (unsigned g = 0; g < kGroups; ++g) { const std::string a = "blk" + std::to_string(g); j.U(a.c_str(), c.blk[g]); }
    for (unsigned k = 0; k < kBlocks; ++k) { const std::string a = "loaded" + std::to_string(k); j.U(a.c_str(), c.loaded[k]); }
    j.U("gb", c.gb);
    j.U("mut_site", c.mutSite).U("mut_kind", c.mutKind).U("mut_a", c.mutA).U("mut_b", c.mutB).U("mut_g", c.mutG).U("mut_blk", c.mutBlk).U("mut_ag", c.mutAG);
    return j.End();
}
}

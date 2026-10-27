#pragma once
// Fixtures del banco FightHitObj72 (lote 72): CTaskSimpleFight::FightHitObj (EXE 0x61D400, thiscall, ECX = tarea, 6 argumentos en pila, `ret 0x18`), 16384 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskSimpleFight::FightHitObj  0x61D400  (CPed* creador, CObject* objeto, CVector* posn1, CVector* posn2, uint32 componente, uint32 superficie)   (hook CTaskSimpleFight / FightHitObj, tipo S)
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

namespace Fo72Fix {
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
constexpr std::size_t kTaskWin = 0x30, kPedWin = 0x800, kObjWin = 0x1A0, kInfoWin = 0x30, kMatWin = 0x48, kVecWin = 0x10;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kTaskArena = Arena(kTaskWin), kPedArena = Arena(kPedWin), kObjArena = Arena(kObjWin), kInfoArena = Arena(kInfoWin), kMatArena = Arena(kMatWin), kVecArena = Arena(kVecWin);

constexpr u32 kMaxMult = 0x42C7CCCDu;   // 0x86D6B0 = 99.9f
constexpr u32 kChainsaw = 9;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 72072u); }
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
inline bool IsNan(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }

// ------------------------------------------------------------------------------------------------ JSON minimo
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ valores notables (NaN solo canonicos: la x87 y SSE eligen distinto operando entre dos NaN con carga util)
inline constexpr std::array<u32, 2> kNan{0x7FC00000u, 0xFFC00000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 8> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
inline constexpr std::array<u32, 12> kSurfaces{0u, 1u, 2u, 3u, 4u, 0x7Fu, 0x80u, 0xFFu, 0x20u, 0x41u, 0x15u, 0xA5u};
inline constexpr std::array<u32, 12> kWeaponTypes{0u, 1u, 2u, 8u, 9u, 9u, 9u, 0x16u, 0x2Eu, 0xFFu, 0x12345678u, 0x80000009u};
inline constexpr std::array<u32, 8> kEffectsHigh{0xC8u, 0xC9u, 0xFFu, 0xC8u, 0x80u, 0xCCu, 0xFEu, 0xC8u};
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
// Un float "de mundo": mayormente moderado, a veces especial
inline u32 World(u32 c, u32 k, bool special) {
    const u32 r = Rand(c, k) % 16;
    if (special) {
        if (r < 3) return Pick(kTiny, c, k + 1);
        if (r < 5) return Pick(kInf, c, k + 1);
        if (r < 7) return Pick(kBig, c, k + 1);
        if (r < 9) return Pick(kNan, c, k + 1);
        if (r < 10) return B(1.0f);
        if (r < 11) return B(-1.0f);
        return RandFloat(c, k + 2, -20, 100);
    }
    if (r == 0) return B(0.0f);
    return RandFloat(c, k + 2, -4, 11);
}
// Un float cerca de `base` (+-k ulp) para los umbrales
inline u32 Near(u32 base, u32 c, u32 k) {
    static constexpr int d[]{0, 1, -1, 2, -2, 3, -3, 0, 1, -1, 16, -16};
    const int off = d[Rand(c, k) % std::size(d)];
    if (F(base) != F(base) || std::isinf(F(base))) return base;
    return (base & 0x80000000u) ? u32(i32(base) - off) : u32(i32(base) + off);
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 combo{}, move{}, slot{}, weaponType{}, surface{}, component{};
    u32 damage{};
    u32 effect{}, physFlags{}, flags{}, mult{}, uproot{}, objMat{};
    u32 mutFlags{}, flagsAfter{}, mutWeapon{}, mutNormal{}, mutPosn1{};
    u32 p1[3]{}, n2[3]{}, pos[3]{}, mutN[3]{}, mutP[3]{};
};

// Ya se cumplen (o no) las tres puertas de la fuerza / del bloque de SetIsStatic?
inline bool MultOk(u32 mult) { return F(mult) < 99.9f; }   // NaN da false

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n % kFamilies;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    const bool special = (n / 16) % 4 == 3;
    // combate: valores reales con algunos desbordes cortos de la tabla (el EXE no acota el indice)
    c.combo = u32(i32(Rand(n, 10) % 24) - 8);
    c.move = u32(i32(Rand(n, 11) % 49) - 8);
    if (Rand(n, 12) % 3 != 0) { c.combo = Rand(n, 13) % 9; c.move = Rand(n, 14) % 0x22; }
    // arma activa: ranura real 0..12 casi siempre; fuera de rango (el EXE no acota el movsx) en algunos casos
    c.slot = u32(i32(Rand(n, 15) % 13));
    if (Rand(n, 16) % 8 == 0) c.slot = u32(i32(Rand(n, 17) % 51) - 30);
    c.weaponType = Rand(n, 18) % 3 == 0 ? kChainsaw : Pick(kWeaponTypes, n, 19);
    c.surface = Pick(kSurfaces, n, 20);
    if (Rand(n, 21) % 4 == 0) c.surface = Rand(n, 22) % 256;   // el unico llamador (FightStrike) pasa un uint8
    c.component = Rand(n, 23) % 4 == 0 ? Rand(n, 24) : Rand(n, 25) % 32;
    // danio devuelto por FightStrikeDamage
    c.damage = Rand(n, 26) % 8 == 0 ? Pick(kTiny, n, 27) : RandFloat(n, 28, -8, 8);
    // objeto: por defecto cumple las tres puertas y no tiene las banderas 4 / 0x40000
    c.effect = Rand(n, 30) % 0xC8;
    c.physFlags = Rand(n, 31) & ~0x84u;
    c.flags = Rand(n, 32) & ~(0x4u | 0x40000u);
    c.mult = Rand(n, 33) % 2 ? B(float(Rand(n, 34) % 1000) * 0.1f - 20.0f) : RandFloat(n, 35, -6, 6);
    if (!MultOk(c.mult)) c.mult = B(F(c.mult) * 0.001f);
    c.uproot = Rand(n, 36) % 3 == 0 ? Pick(kTiny, n, 37) : RandFloat(n, 38, -10, 8);
    c.objMat = Rand(n, 39) % 2;
    for (int i = 0; i < 3; ++i) {
        c.p1[i] = World(n, 40 + i, special);
        c.n2[i] = World(n, 50 + i, special);
        c.pos[i] = World(n, 60 + i, special);
        c.mutN[i] = World(n, 70 + i, false);
        c.mutP[i] = World(n, 80 + i, false);
    }
    const u32 inner = Rand(n, 90) % 2;   // 1: el objeto entra en el bloque de SetIsStatic (banderas 4 o 0x40000)
    auto setInner = [&] { const u32 r = Rand(n, 91) % 3; c.flags |= r == 0 ? 0x40000u : r == 1 ? 0x40004u : 4u; };
    // umbral de arranque <= 0: cero, cero negativo, negativos normales o desnormales
    auto lowUproot = [&] {
        const u32 r = Rand(n, 93) % 4;
        c.uproot = r == 0 ? 0u : r == 1 ? 0x80000000u : r == 2 ? (0x80000000u | (Rand(n, 95) & 0x007FFFFFu)) : (RandFloat(n, 96, -5, 6) | 0x80000000u);
    };
    switch (c.family) {
    case 0: c.effect = Pick(kEffectsHigh, n, 100); if (inner) setInner(); break;                  // efecto de danio >= 0xC8
    case 1: c.physFlags |= 4u; if (inner) setInner(); break;                                         // bit 2 de las banderas fisicas
    case 2: {   // multiplicador >= 99.9 (o NaN / infinito)
        const u32 r = Rand(n, 101) % 8;
        c.mult = r == 0 ? kMaxMult : r == 1 ? Near(kMaxMult, n, 102) : r == 2 ? Pick(kNan, n, 103) : r == 3 ? Pick(kInf, n, 104) : r == 4 ? Pick(kBig, n, 105) : r == 5 ? B(F(kMaxMult) + float(Rand(n, 106) % 1000)) : Near(kMaxMult, n, 107);
        if (MultOk(c.mult)) c.mult = B(F(kMaxMult) + 1.0f);
        if (inner) setInner();
        break; }
    case 3: setInner(); lowUproot(); break;                                                          // SetIsStatic + AddToMovingList
    case 4: {   // umbral de arranque no cumplido: el bloque se salta pero la fuerza sigue
        setInner();
        const u32 r = Rand(n, 110) % 6;
        c.uproot = r == 0 ? Pick(kNan, n, 111) : r == 1 ? Pick(kInf, n, 112) & 0x7FFFFFFFu : r == 2 ? 1u : r == 3 ? B(0.5f) : r == 4 ? 0x007FFFFFu : (RandFloat(n, 113, -3, 6) & 0x7FFFFFFFu) | 1u;
        break; }
    case 5: c.objMat = 1; break;                                                                     // fuerza, posicion en la matriz
    case 6: c.objMat = 0; break;                                                                     // fuerza, posicion compacta
    case 7: c.physFlags |= 0x80u; c.objMat = Rand(n, 114) % 2; break;                               // factor -0.1
    case 8: {   // fuerza con diferencias desnormales y normales cero
        c.physFlags = (c.physFlags & ~0x80u) | ((Rand(n, 115) % 2) ? 0x80u : 0u);
        static constexpr u32 pairs[][2]{{0x00800000u, 0x007FFFFFu}, {0x007FFFFFu, 0x00800000u}, {0x00400000u, 0u}, {0u, 0x80000001u}, {3u, 1u}, {0x80800000u, 0x807FFFFFu}, {0x3F800000u, 0x3F800000u}, {0x00000001u, 0x80000000u}};
        const u32 r = Rand(n, 116) % 6;
        for (int i = 0; i < 3; ++i) if (Rand(n, 117 + i) % 2 || r < 2) { const u32* p = pairs[Rand(n, 120 + i) % std::size(pairs)]; c.p1[i] = p[0]; c.pos[i] = p[1]; }
        for (int i = 0; i < 3; ++i) if (Rand(n, 123 + i) % 2) c.n2[i] = Pick(kTiny, n, 126 + i);
        break; }
    case 9: {   // las banderas cambian dentro de SetIsStatic / AddToMovingList: se relee antes de decidir la fuerza
        setInner(); lowUproot();
        c.mutFlags = 1 + Rand(n, 130) % 2;
        c.flagsAfter = Rand(n, 131) % 2 ? (c.flags & ~(0x4u | 0x40000u)) : (c.flags | (Rand(n, 132) % 2 ? 4u : 0x40000u));
        break; }
    case 10: c.damage = Rand(n, 133) % 3 == 0 ? Pick(kNan, n, 134) : Rand(n, 135) % 3 == 0 ? Pick(kInf, n, 136) : Rand(n, 137) % 2 ? Pick(kBig, n, 138) : Pick(kTiny, n, 139); break;   // danio especial
    case 11: c.weaponType = kChainsaw; c.mutWeapon = Rand(n, 140) % 2 ? 1 : 0; break;               // motosierra (con relectura)
    case 12: c.mutWeapon = 1 + Rand(n, 141) % 2; c.slot = u32(i32(Rand(n, 142) % 51) - 30); c.weaponType = Rand(n, 143) % 2 ? kChainsaw : Pick(kWeaponTypes, n, 144); break;   // el arma cambia en ObjectDamage; ranuras fuera de rango
    case 13: c.mutNormal = 1; c.mutPosn1 = Rand(n, 145) % 2; break;                                  // FightStrikeDamage modifica las posiciones
    default:    // 14 y 15: mezcla de todo
        c.mutNormal = Rand(n, 146) % 4 == 0; c.mutPosn1 = Rand(n, 147) % 4 == 0; c.mutWeapon = Rand(n, 148) % 3;
        if (inner) setInner();
        if (inner && Rand(n, 149) % 2) lowUproot();
        if (Rand(n, 150) % 2) c.physFlags |= 0x80u;
        if (Rand(n, 151) % 3 == 0) { c.mutFlags = 1 + Rand(n, 152) % 2; c.flagsAfter = Rand(n, 153) & ~(0x4u | 0x40000u); if (Rand(n, 154) % 2) c.flagsAfter |= 0x40000u; }
        if (c.family == 15) {
            if (Rand(n, 155) % 3 == 0) c.effect = 0xC0 + Rand(n, 156) % 16;
            if (Rand(n, 157) % 6 == 0) c.physFlags |= 4u;
            if (Rand(n, 158) % 5 == 0) c.mult = Near(kMaxMult, n, 159);
        }
        break;
    }
    // Una resta con dos NaN depende del operando que elija cada unidad: se evita (tambien con la mutacion de posn1)
    for (int i = 0; i < 3; ++i) {
        if (IsNan(c.p1[i]) && IsNan(c.pos[i])) c.pos[i] = B(1.5f);
        if (IsNan(c.mutP[i]) && IsNan(c.pos[i])) c.mutP[i] = B(2.5f);
    }
    if (c.mutFlags == 0) c.flagsAfter = c.flags;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("combo", c.combo).U("move", c.move).U("slot", c.slot).U("weapon_type", c.weaponType).U("surface", c.surface).U("component", c.component).U("damage", c.damage);
    j.U("effect", c.effect).U("phys_flags", c.physFlags).U("flags", c.flags).U("mult", c.mult).U("uproot", c.uproot).U("obj_mat", c.objMat);
    j.U("mut_flags", c.mutFlags).U("flags_after", c.flagsAfter).U("mut_weapon", c.mutWeapon).U("mut_normal", c.mutNormal).U("mut_posn1", c.mutPosn1);
    j.U("p1_x", c.p1[0]).U("p1_y", c.p1[1]).U("p1_z", c.p1[2]).U("n2_x", c.n2[0]).U("n2_y", c.n2[1]).U("n2_z", c.n2[2]).U("pos_x", c.pos[0]).U("pos_y", c.pos[1]).U("pos_z", c.pos[2]);
    j.U("mutn_x", c.mutN[0]).U("mutn_y", c.mutN[1]).U("mutn_z", c.mutN[2]).U("mutp_x", c.mutP[0]).U("mutp_y", c.mutP[1]).U("mutp_z", c.mutP[2]);
    return j.End();
}
}

#pragma once
// Fixtures del banco FightHitPed75 (lote 75): CTaskSimpleFight::FightHitPed (EXE 0x61CBA0, thiscall, ECX = tarea, 5 argumentos en pila, `ret 0x14`, EAX = victima o 0). Solo C++ estandar
// (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskSimpleFight::FightHitPed  0x61CBA0  (CPed* creador, CPed* victima, CVector* posn1, CVector* posn2, int16 componente)   (hook CTaskSimpleFight / FightHitPed, tipo S)
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

namespace Fp75Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;   // familias 0..15 (n % 16), 1024 casos cada una
constexpr unsigned kFamNan2 = 1024;      // familia 16: dos NaN no canonicos en la misma resta (atacante - victima)
constexpr unsigned kFamNan1 = 1024;      // familia 17: un NaN no canonico en cada operacion (resta, 1.5f, normalizacion, brillo, vida, danio)
constexpr unsigned kComboLo = 8, kComboN = 24, kMoveLo = 8, kMoveN = 49;   // combo -8..15, movimiento -8..40
constexpr unsigned kFamGrid = kComboN * kMoveN * 2;   // familia 18: barrido exhaustivo del indice de la tabla de sonidos con lado 0 y con lado distinto de 0
constexpr unsigned kFamEdge = 1024;      // familia 19: bordes de _ftol2 (danio, vida), del sorteo contra la probabilidad y del retardo
constexpr unsigned kCases = kBaseCases + kFamNan2 + kFamNan1 + kFamGrid + kFamEdge;
constexpr unsigned kFamilies = 20;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kTaskWin = 0x30, kPedWin = 0x800, kMatWin = 0x48, kVecWin = 0x10, kIntelWin = 0x20, kBlockerWin = 0x30, kActiveWin = 0x10, kGroupWin = 0x20;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kTaskArena = Arena(kTaskWin), kPedArena = Arena(kPedWin), kMatArena = Arena(kMatWin), kVecArena = Arena(kVecWin), kIntelArena = Arena(kIntelWin),
                      kBlockerArena = Arena(kBlockerWin), kActiveArena = Arena(kActiveWin), kGroupArena = Arena(kGroupWin);

constexpr u32 kChainsaw = 9;

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
inline bool IsNan(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }

// ------------------------------------------------------------------------------------------------ JSON minimo
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ valores notables
inline constexpr std::array<u32, 2> kNan{0x7FC00000u, 0xFFC00000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 8> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
inline constexpr std::array<u32, 12> kWeaponTypes{0u, 1u, 2u, 8u, 9u, 9u, 9u, 0x16u, 0x2Eu, 0xFFu, 0x12345678u, 0x80000009u};
inline constexpr std::array<u32, 10> kTaskTypes{0xCDu, 0xCDu, 0xCCu, 0xCEu, 0u, 0xFFFFFFFFu, 0x1CDu, 0xCD00u, 0x80000000u, 1u};
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
// Un float "de mundo": mayormente moderado, a veces especial (NaN solo canonicos)
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
// NaN con signo, carga util y tipo (QNaN/SNaN) arbitrarios; `kind`: 0 QNaN, 1 SNaN, otro = aleatorio.
inline u32 NanBits(u32 c, u32 k, u32 kind) {
    const u32 r = Rand(c, k), s = Rand(c, k + 1);
    u32 payload;
    switch (s % 6) {
    case 0: payload = 0; break;
    case 1: payload = 1 + (r % 0xFF); break;
    case 2: payload = 0x3FFFFFu; break;
    case 3: payload = 0x200000u | (r & 0xFFu); break;
    case 4: payload = 0x1FFFFFu; break;
    default: payload = (r >> 3) & 0x3FFFFFu; break;
    }
    const bool quiet = kind == 0 ? true : kind == 1 ? false : ((r >> 30) & 1) != 0;
    if (!quiet && payload == 0) payload = 1;   // un sNaN exige carga util distinta de cero
    return ((r >> 31) << 31) | 0x7F800000u | (quiet ? 0x400000u : 0u) | payload;
}
// r16 (valor de rand() & 0xFFFF) cuyo tirada floor(r16 * 100 / 32768) vale exactamente `roll`
inline u32 R16ForRoll(u32 roll) { return (roll * 32768u + 99u) / 100u; }

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 combo{}, move{};                                          // bytes con signo de la tarea (+0x24, +0x25), extendidos con ceros
    u32 victimPlayer{}, taskType{}, hasFighting{}, blocked{}, blockedHi{};
    u32 creatorPlayer{}, slot{}, weaponType{}, mutWeapon{};
    u32 damage{}, side{}, gdeOk{}, gdeHi{}, component{};
    u32 health{}, brightness{}, gdeHealthMut{}, gdeHealth{};
    u32 alive1{}, alive2{}, aliveHi{};
    u32 randValue{};
    u32 normMode{}, normOut[3]{};
    u32 creatorMat{}, victimMat{};
    u32 cpos[3]{}, vpos[3]{}, p1[3]{}, p2[3]{}, mutP1{}, mutP[3]{};
    u32 mutSite{}, mutCombo{}, mutMove{};
    u32 tailRoll{};                                               // 1: la tirada se fija con R16ForRoll(rollTarget)
};

inline bool RealCombo(u32 c) { return c >= 4 && c <= 15; }

inline C BaseDefaults(unsigned n) {
    C c;
    c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    const bool special = (n / 16) % 4 == 3;
    // combate: valores reales casi siempre; a veces desbordes cortos de la tabla (el EXE no acota el indice)
    c.combo = u32(i32(Rand(n, 10) % 24) - 8);
    c.move = u32(i32(Rand(n, 11) % 49) - 8);
    if (Rand(n, 12) % 3 != 0) { c.combo = 4 + Rand(n, 13) % 12; c.move = Rand(n, 14) % 5; }
    c.victimPlayer = Rand(n, 20) % 8 == 0;
    c.taskType = Rand(n, 21) % 2 ? 0xCDu : Rand(n, 22) % 4 == 0 ? Pick(kTaskTypes, n, 23) : 0xCCu + Rand(n, 24) % 3;
    c.hasFighting = Rand(n, 25) % 3 == 0;
    c.blocked = Rand(n, 26) % 2;
    c.blockedHi = Rand(n, 27) % 2 ? 0u : Rand(n, 28) << 8;
    c.creatorPlayer = Rand(n, 29) % 3 == 0;
    c.slot = u32(i32(Rand(n, 30) % 13));
    if (Rand(n, 31) % 8 == 0) c.slot = u32(i32(Rand(n, 32) % 51) - 30);
    c.weaponType = Rand(n, 33) % 4 == 0 ? kChainsaw : Pick(kWeaponTypes, n, 34);
    c.damage = Rand(n, 35) % 6 == 0 ? RandFloat(n, 36, -3, 40) : B(float(Rand(n, 37) % 300) + (Rand(n, 38) % 3 == 0 ? 0.75f : 0.0f));
    c.side = Rand(n, 39) % 3 == 0 ? 0u : Rand(n, 40) % 4 == 0 ? Rand(n, 41) : 1 + Rand(n, 42) % 7;
    c.gdeOk = Rand(n, 43) % 2;
    c.gdeHi = Rand(n, 44) % 2 ? 0u : Rand(n, 45) << 8;
    c.component = Rand(n, 46) % 4 == 0 ? Rand(n, 47) : Rand(n, 48) % 32;
    c.health = Rand(n, 49) % 4 == 0 ? RandFloat(n, 50, -4, 9) : B(float(Rand(n, 51) % 120) - 5.0f + (Rand(n, 52) % 2 ? 0.5f : 0.0f));
    c.brightness = Rand(n, 53) % 3 == 0 ? Rand(n, 54) : B(float(Rand(n, 55) % 100) * 0.01f);
    c.gdeHealthMut = Rand(n, 56) % 4 == 0;
    c.gdeHealth = Rand(n, 57) % 2 ? RandFloat(n, 58, -4, 9) : B(float(Rand(n, 59) % 120) - 5.0f);
    c.alive1 = Rand(n, 60) % 2; c.alive2 = Rand(n, 61) % 5 == 0 ? !c.alive1 : c.alive1;
    c.aliveHi = Rand(n, 62) % 2 ? 0u : Rand(n, 63) << 8;
    c.randValue = Rand(n, 64);
    c.normMode = Rand(n, 65) % 3 == 0;
    for (int i = 0; i < 3; ++i) {
        c.normOut[i] = World(n, 66 + i, false);
        c.cpos[i] = World(n, 70 + i, special);
        c.vpos[i] = World(n, 74 + i, special);
        c.p1[i] = World(n, 78 + i, false);
        c.p2[i] = World(n, 82 + i, false);
        c.mutP[i] = World(n, 86 + i, false);
    }
    c.creatorMat = Rand(n, 90) % 2; c.victimMat = Rand(n, 91) % 2;
    c.mutP1 = Rand(n, 92) % 8 == 0;
    c.mutWeapon = Rand(n, 93) % 8 == 0 ? 1 + Rand(n, 94) % 2 : 0;
    // por defecto, camino normal: la victima no es el jugador y no pelea, la tirada deja pasar o no al azar
    c.victimPlayer = c.victimPlayer && Rand(n, 95) % 2;
    return c;
}

// Dos NaN en el mismo operando de la x87 (fsub): atacante - victima por eje.
inline void TwoNanPair(u32 n, int i, u32& a, u32& b) {
    switch (Rand(n, 201 + i) % 10) {
    case 0: a = NanBits(n, 210 + 4 * i, 0); b = NanBits(n, 211 + 4 * i, 0); break;
    case 1: a = NanBits(n, 210 + 4 * i, 1); b = NanBits(n, 211 + 4 * i, 1); break;
    case 2: a = NanBits(n, 210 + 4 * i, 2); b = NanBits(n, 211 + 4 * i, 2); break;
    case 3: a = NanBits(n, 210 + 4 * i, 2); b = a ^ 0x80000000u; break;
    case 4: a = NanBits(n, 210 + 4 * i, 2); b = a; break;
    case 5: b = NanBits(n, 210 + 4 * i, 0); a = (b & 0xFFBFFFFFu); if ((a & 0x3FFFFFu) == 0) a |= 1u; break;
    case 6: a = NanBits(n, 210 + 4 * i, 0); b = (a & 0xFFBFFFFFu); if ((b & 0x3FFFFFu) == 0) b |= 1u; break;
    case 7: a = 0x7FC00000u | (Rand(n, 214 + i) & 0x3FFFFFu); b = 0x7FC00000u | (Rand(n, 215 + i) & 0x3FFFFFu); if (Rand(n, 216 + i) % 2) std::swap(a, b); break;
    case 8: a = 0xFFC00000u | (Rand(n, 217 + i) & 0x3FFFFFu); b = 0x7FC00000u | (Rand(n, 218 + i) & 0x3FFFFFu); break;
    default: a = NanBits(n, 210 + 4 * i, 2); b = RandFloat(n, 219 + i, -4, 11); if (Rand(n, 220 + i) % 2) std::swap(a, b); break;
    }
}

// Deja el caso en el camino que llega a la sangre: golpe normal, vida/probabilidad que dejan pasar la tirada (chance > roll).
inline void ForceBlood(C& c) {
    c.victimPlayer = 0; c.hasFighting = 0;
    c.combo = 8 + c.n % 5;                                         // combos 8..12: probabilidad 100
    c.randValue = R16ForRoll(Rand(c.n, 230) % 100);
}

inline C MakeExtra(unsigned n) {
    C c = BaseDefaults(n);
    const unsigned j = n - kBaseCases;
    if (j < kFamNan2) {   // familia 16: dos NaN en la resta de la direccion de la sangre (posicion del atacante - posicion de la victima)
        c.family = 16;
        ForceBlood(c);
        if (Rand(n, 231) % 3 == 0) { c.combo = Rand(n, 232) % 2 ? 4u + Rand(n, 233) % 4 : 13u + Rand(n, 234) % 3; c.health = B(0.0f); c.randValue = R16ForRoll(Rand(n, 235) % 90); }
        for (int i = 0; i < 3; ++i) {
            u32 a, b;
            TwoNanPair(n, i, a, b);
            c.cpos[i] = a; c.vpos[i] = b;
        }
        c.normMode = Rand(n, 236) % 2;
        return c;
    }
    if (j < kFamNan2 + kFamNan1) {   // familia 17: un NaN no canonico/sNaN en cada operacion de la ruta
        c.family = 17;
        const u32 r = Rand(n, 240) % 8;
        if (r != 0 && r != 6) ForceBlood(c);
        auto some = [&](u32* v, u32 k) { for (int i = 0; i < 3; ++i) if (Rand(n, k + i) % 2) v[i] = NanBits(n, k + 4 + i, 2); v[Rand(n, k + 8) % 3] = NanBits(n, k + 9, 2); };
        switch (r) {
        case 0: c.damage = NanBits(n, 241, 2); break;                                                         // _ftol2(NaN)
        case 1: some(c.cpos, 250); break;                                                                      // resta: solo el atacante
        case 2: some(c.vpos, 260); break;                                                                      // resta: solo la victima
        case 3: some(c.normOut, 270); c.normMode = 1; c.combo = 8 + n % 5; c.alive1 = c.alive2 = 1; break;     // normalizacion con NaN y *1.5f
        case 4: c.brightness = NanBits(n, 280, 2); break;                                                      // brillo (sNaN/QNaN no canonicos) hasta AddBlood
        case 5: c.health = NanBits(n, 281, 2); c.combo = 4 + n % 4; c.randValue = R16ForRoll(Rand(n, 282) % 120); break;   // vida NaN: probabilidad 0
        case 6: c.health = Rand(n, 283) % 2 ? NanBits(n, 284, 2) : B(Rand(n, 285) % 2 ? INFINITY : -INFINITY); c.combo = 4 + n % 4; break;
        default: some(c.p1, 290); some(c.p2, 300); c.mutP1 = 1; some(c.mutP, 310); break;                      // posn1 con NaN (se copia en bruto)
        }
        return c;
    }
    if (j < kFamNan2 + kFamNan1 + kFamGrid) {   // familia 18: cada indice de la tabla de sonidos (combo -8..15 x movimiento -8..40), con lado 0 y con lado distinto de 0
        c.family = 18;
        const unsigned g = (j - kFamNan2 - kFamNan1) % (kComboN * kMoveN);
        const bool nonZero = (j - kFamNan2 - kFamNan1) >= kComboN * kMoveN;
        c.combo = u32(i32(g / kMoveN) - i32(kComboLo));
        c.move = u32(i32(g % kMoveN) - i32(kMoveLo));
        c.victimPlayer = 0; c.hasFighting = 0;
        c.side = nonZero ? 1 + Rand(n, 320) % 7 : 0;
        c.mutSite = 0;
        return c;
    }
    c.family = 19;   // familia 19: bordes de _ftol2, del sorteo y del retardo
    const unsigned e = j - kFamNan2 - kFamNan1 - kFamGrid;
    c.victimPlayer = 0; c.hasFighting = 0;
    switch (e % 4) {
    case 0: {   // danio en los bordes de int32/int64 y fracciones
        static constexpr u32 edge[]{0x4F000000u, 0x4EFFFFFFu, 0xCF000000u, 0xCEFFFFFFu, 0x4F800000u, 0x5F000000u, 0x5EFFFFFFu, 0xDF000000u, 0xDEFFFFFFu, 0x7F7FFFFFu, 0x3F7FFFFFu, 0xBF7FFFFFu,
                                    0x3F800000u, 0xBF800000u, 0x477FFF00u, 0x4B7FFFFFu, 0x4B800000u, 0xCB800000u, 0x00000001u, 0x80000001u, 0x7F800000u, 0xFF800000u, 0x5F800000u, 0x4F800001u};
        c.damage = edge[(e / 4) % std::size(edge)];
        break; }
    case 1: {   // vida en los bordes de 100 - vida
        static constexpr u32 hv[]{0x42C80000u, 0x42C7FFFFu, 0x42C80001u, 0x00000000u, 0x80000000u, 0x3F800000u, 0xC2C80000u, 0x42C60000u, 0x7F800000u, 0xFF800000u, 0xCF000000u, 0xCF800000u, 0x4F000000u, 0x33800000u, 0x3F7FFFFFu, 0x42C7FFFEu};
        c.health = hv[(e / 4) % std::size(hv)];
        c.combo = 4 + (e / 64) % 4; c.move = (e / 8) % 4;
        c.randValue = R16ForRoll(Rand(n, 330) % 105);
        break; }
    case 2: {   // sorteo contra la probabilidad: tirada = probabilidad - 1, probabilidad, probabilidad + 1
        c.combo = 8 + (e / 4) % 5; c.randValue = R16ForRoll(99 + (e / 20) % 3 - 1);
        if ((e / 4) % 5 == 4) { c.combo = 4; c.move = 4; c.randValue = R16ForRoll(0); }                  // probabilidad -1: nunca
        if ((e / 4) % 7 == 3) { c.combo = 4 + (e / 4) % 3; c.move = (e / 4) % 4; c.health = B(float(Rand(n, 331) % 101)); c.randValue = R16ForRoll(u32(std::max(0.0, 100.0 - double(F(c.health)))) + (e / 20) % 3 - 1); }
        c.randValue |= (Rand(n, 332) & 0xFFFF0000u);
        break; }
    default: {  // retardo del combo 7, movimiento 1: la tabla es fija; cubre la rama del retardo con todas las variaciones de lado y de arma
        c.combo = 7; c.move = 1; c.side = 0;
        c.weaponType = Pick(kWeaponTypes, n, 333);
        break; }
    }
    return c;
}

inline C MakeC(unsigned n) {
    if (n >= kBaseCases) return MakeExtra(n);
    C c = BaseDefaults(n);
    auto setSite = [&] { c.mutSite = 1 + Rand(n, 100) % 7; c.mutCombo = Rand(n, 101) % 3 ? 4 + Rand(n, 102) % 12 : u32(i32(Rand(n, 103) % 24) - 8); c.mutMove = Rand(n, 104) % 3 ? Rand(n, 105) % 5 : u32(i32(Rand(n, 106) % 49) - 8); };
    switch (c.family) {
    case 0:   // la victima es el jugador: la tarea activa 0xCD sale, otras siguen
        c.victimPlayer = 1; c.hasFighting = Rand(n, 110) % 2;
        break;
    case 1:   // la victima pelea y lo bloquea: solo suena el golpe bloqueado
        c.victimPlayer = Rand(n, 111) % 6 == 0; if (c.victimPlayer) c.taskType = 0xCC;
        c.hasFighting = 1; c.blocked = 1;
        if (Rand(n, 112) % 3 == 0) c.combo = 5;
        if (Rand(n, 113) % 3 == 0) { setSite(); c.mutSite = 7 + 0 * c.mutSite; }
        if (Rand(n, 114) % 4 == 0) c.mutSite = 3;
        break;
    case 2:   // la victima pelea y no lo bloquea: sigue el golpe normal
        c.victimPlayer = 0; c.hasFighting = 1; c.blocked = 0;
        if (Rand(n, 115) % 3 == 0) setSite();
        break;
    case 3:   // lado 0: combos 5 (movimientos 0..2 y fuera), 7 (movimiento 1 y otros) y los demas
        c.victimPlayer = 0; c.hasFighting = 0; c.side = 0;
        c.combo = Rand(n, 116) % 3 == 0 ? 5u : Rand(n, 117) % 3 == 0 ? 7u : c.combo;
        if (c.combo == 5 || c.combo == 7) c.move = Rand(n, 118) % 4 == 0 ? u32(i32(Rand(n, 119) % 9) - 4) : Rand(n, 120) % 4;
        if (Rand(n, 121) % 4 == 0) setSite();
        break;
    case 4:   // lado distinto de 0: combo 5 con movimientos 0..2 y fuera
        c.victimPlayer = 0; c.hasFighting = 0; c.side = 1 + Rand(n, 122) % 7;
        c.combo = Rand(n, 123) % 2 ? 5u : c.combo;
        if (c.combo == 5) c.move = Rand(n, 124) % 4 == 0 ? u32(i32(Rand(n, 125) % 9) - 4) : Rand(n, 126) % 4;
        if (Rand(n, 127) % 4 == 0) setSite();
        break;
    case 5:   // el lado se prueba entero (dword): bajo byte 0 pero distinto de 0, negativo, enormes
        c.victimPlayer = 0; c.hasFighting = 0;
        c.side = Pick(std::array<u32, 12>{0u, 0x100u, 0x10000u, 0x80000000u, 0xFFFFFFFFu, 1u, 7u, 0x7FFFFFFFu, 0x1000000u, 0xFF00u, 0x80u, 0xFFu}, n, 128);
        break;
    case 6:   // arma: motosierra con relectura tras GenerateDamageEvent, ranuras fuera de rango, jugador (Say)
        c.victimPlayer = 0; c.hasFighting = 0;
        c.mutWeapon = Rand(n, 129) % 3; c.creatorPlayer = Rand(n, 130) % 2;
        c.weaponType = Rand(n, 131) % 2 ? kChainsaw : Pick(kWeaponTypes, n, 132);
        if (Rand(n, 133) % 2) c.slot = u32(i32(Rand(n, 134) % 51) - 30);
        break;
    case 7:   // danio: enteros, negativos, enormes, NaN, infinitos, desnormales
        c.victimPlayer = 0; c.hasFighting = 0;
        c.damage = Rand(n, 135) % 5 == 0 ? Pick(kNan, n, 136) : Rand(n, 137) % 5 == 0 ? Pick(kInf, n, 138) : Rand(n, 139) % 4 == 0 ? Pick(kBig, n, 140) : Rand(n, 141) % 4 == 0 ? Pick(kTiny, n, 142) : RandFloat(n, 143, -10, 40);
        break;
    case 8:   // probabilidad de sangre: combos 8..12, (4,4) y la vida; la tirada en torno a la probabilidad
        c.victimPlayer = 0; c.hasFighting = 0;
        switch (Rand(n, 144) % 4) {
        case 0: c.combo = 8 + Rand(n, 145) % 5; c.randValue = R16ForRoll(98 + Rand(n, 146) % 5); break;
        case 1: c.combo = 4; c.move = 4; c.randValue = R16ForRoll(Rand(n, 147) % 200); break;
        case 2: c.combo = 4 + Rand(n, 148) % 12; if (c.combo == 4) c.move = Rand(n, 149) % 4; c.health = B(float(Rand(n, 150) % 130) - 10.0f); c.randValue = R16ForRoll(Rand(n, 151) % 140); break;
        default: c.health = Rand(n, 152) % 2 ? Pick(kTiny, n, 153) : Pick(kBig, n, 154); c.randValue = R16ForRoll(Rand(n, 155) % 200); break;
        }
        c.randValue |= Rand(n, 156) & 0xFFFF0000u;
        break;
    case 9:   // sangre: vivo/muerto, combos 8..12 (escala 1.5f), normalizacion que cambia el vector, posiciones con valores especiales (sin dos NaN)
        ForceBlood(c);
        if (Rand(n, 157) % 3 == 0) c.combo = 4 + Rand(n, 158) % 4;
        c.randValue = R16ForRoll(Rand(n, 159) % 60);
        break;
    case 10:  // sangre con posiciones especiales de mundo (sin dos NaN en un eje)
        ForceBlood(c);
        for (int i = 0; i < 3; ++i) { c.cpos[i] = World(n, 160 + i, true); c.vpos[i] = World(n, 164 + i, true); if (IsNan(c.cpos[i]) && IsNan(c.vpos[i])) c.vpos[i] = B(1.25f); }
        break;
    case 11:  // brillo y posn1 en bruto (incluida la mutacion de posn1 dentro de rand)
        ForceBlood(c);
        c.brightness = Rand(n, 168) % 2 ? Rand(n, 169) : B(float(Rand(n, 170) % 200) * 0.005f);
        c.mutP1 = Rand(n, 171) % 2;
        break;
    case 12:  // mutaciones de combo/movimiento, vida y arma entre llamadas
        setSite(); c.victimPlayer = 0; if (Rand(n, 172) % 4 == 0) { c.hasFighting = 1; c.blocked = Rand(n, 173) % 2; }
        c.gdeHealthMut = Rand(n, 174) % 2;
        if (Rand(n, 175) % 2) ForceBlood(c);
        break;
    case 13:  // resultado de GenerateDamageEvent (solo AL) y valor devuelto
        c.victimPlayer = 0; c.hasFighting = 0; c.gdeOk = Rand(n, 176) % 2;
        if (Rand(n, 177) % 2) ForceBlood(c);
        break;
    default:  // 14, 15: mezcla de todo
        if (Rand(n, 178) % 4 == 0) setSite();
        if (Rand(n, 179) % 3 == 0) ForceBlood(c);
        if (c.family == 15 && Rand(n, 180) % 4 == 0) { c.combo = 5; c.move = Rand(n, 181) % 3; }
        break;
    }
    // Una resta con dos NaN depende del operando que elija cada unidad: solo las familias 16 y 17 la cubren a proposito
    if (c.family < 16)
        for (int i = 0; i < 3; ++i)
            if (IsNan(c.cpos[i]) && IsNan(c.vpos[i])) c.vpos[i] = B(2.5f);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("combo", c.combo).U("move", c.move).U("victim_player", c.victimPlayer).U("task_type", c.taskType).U("has_fighting", c.hasFighting).U("blocked", c.blocked).U("blocked_hi", c.blockedHi);
    j.U("creator_player", c.creatorPlayer).U("slot", c.slot).U("weapon_type", c.weaponType).U("mut_weapon", c.mutWeapon);
    j.U("damage", c.damage).U("side", c.side).U("gde_ok", c.gdeOk).U("gde_hi", c.gdeHi).U("component", c.component);
    j.U("health", c.health).U("brightness", c.brightness).U("gde_health_mut", c.gdeHealthMut).U("gde_health", c.gdeHealth);
    j.U("alive1", c.alive1).U("alive2", c.alive2).U("alive_hi", c.aliveHi).U("rand_value", c.randValue).U("norm_mode", c.normMode);
    j.U("norm_x", c.normOut[0]).U("norm_y", c.normOut[1]).U("norm_z", c.normOut[2]);
    j.U("creator_mat", c.creatorMat).U("victim_mat", c.victimMat);
    j.U("cpos_x", c.cpos[0]).U("cpos_y", c.cpos[1]).U("cpos_z", c.cpos[2]).U("vpos_x", c.vpos[0]).U("vpos_y", c.vpos[1]).U("vpos_z", c.vpos[2]);
    j.U("p1_x", c.p1[0]).U("p1_y", c.p1[1]).U("p1_z", c.p1[2]).U("p2_x", c.p2[0]).U("p2_y", c.p2[1]).U("p2_z", c.p2[2]);
    j.U("mut_p1", c.mutP1).U("mutp_x", c.mutP[0]).U("mutp_y", c.mutP[1]).U("mutp_z", c.mutP[2]);
    j.U("mut_site", c.mutSite).U("mut_combo", c.mutCombo).U("mut_move", c.mutMove);
    return j.End();
}
}

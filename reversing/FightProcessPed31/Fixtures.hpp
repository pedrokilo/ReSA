#pragma once
// Fixtures del banco FightProcessPed31: CTaskSimpleFight::ProcessPed (0x629920, thiscall con 1 argumento de pila (ped), `ret 4`, hook VIRTUAL ranura 7 de la vtable 0x86D684).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y tambien un programa autonomo de calibracion (fixture_mirror.py).
//
// La funcion (ver fixture_oracle.py para el desensamblado) mira la tarea (+8..+0x27), dos asociaciones de animacion, el ped (posicion, matriz, velocidad, arma activa, estilo de pelea, grupo de animacion,
// estado de movimiento), la tabla de combos 0xC170D0 (filas de 0x88 bytes), los vectores de nivel 0xC177D0, el temporizador 0xB7CB5C, la camara (0xB6EC2E, 0xB6F32C), la tabla de grupos de animacion
// (puntero 0xB4EA34, 0x14 bytes por grupo) y llama a once colaboradores ya en C++ y a seis del EXE: todos se SUSTITUYEN por el mismo falso en ambos lados.
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace FightProcessPed31Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4608;   // 16 familias x 288

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

// ------------------------------------------------------------------------------------------------ disposicion de memoria (guarda | objeto | guarda)
// Cada arena = guarda delantera (off) + ventana del objeto (bytes) + guarda trasera; la suma esta escrita a mano y comprobada por fixture_mirror.py contra el oraculo.
constexpr u32 kThisBytes = 0x28, kThisOff = 0x40, kThisArena = 0x100;           // 0x40 + 0x28 + 0x98
constexpr u32 kAnimBytes = 0x3C, kAnimOff = 0x20, kAnimArena = 0xA0;            // 0x20 + 0x3C + 0x44 (anim de golpe, reposo y reposo nuevo)
constexpr u32 kHierBytes = 0x14, kHierOff = 0x20, kHierArena = 0x70;            // 0x20 + 0x14 + 0x3C (tiempo total en +0x10)
constexpr u32 kPedBytes = 0x730, kPedOff = 0x40, kPedArena = 0x7B0;             // 0x40 + 0x730 + 0x40 (hasta m_nFightingStyle +0x72D)
constexpr u32 kMatBytes = 0x48, kMatOff = 0x20, kMatArena = 0xA0;               // 0x20 + 0x48 + 0x38 (matriz del ped y del objetivo)
constexpr u32 kTgtBytes = 0x18, kTgtOff = 0x20, kTgtArena = 0x80;               // 0x20 + 0x18 + 0x48 (objetivo: posicion +4, puntero a matriz +0x14)
constexpr u32 kBlocksBytes = 0x60, kBlocksOff = 0x20, kBlocksArena = 0xC0;      // 0x20 + 0x60 + 0x40: tres CAnimBlock de 0x20
constexpr u32 kGroupsBytes = 0x50, kGroupsOff = 0x20, kGroupsArena = 0x110;     // 0x20 + 0x50 + 0xA0: cuatro CAnimBlendAssocGroup de 0x14
constexpr u32 kWinfoBytes = 0x70, kWinfoOff = 0x20, kWinfoArena = 0xD0;         // 0x20 + 0x70 + 0x40: CWeaponInfo falso (m_nBaseCombo en +0x6E)
constexpr u32 kPadBytes = 0x10, kPadOff = 0x20, kPadArena = 0x70;               // 0x20 + 0x10 + 0x40: CPad falso (solo se usa su direccion)
constexpr u32 kSlots = 16;                                                       // no se usa vtable (se mantiene para el contrato del banco)
constexpr u32 kProcessPedSlot = 7;                                               // [vtable + 0x1C]
// region de la tabla de combos / niveles (como FightMeleeData31): 0x40 + 0x830 + 0x40
constexpr u32 kRegionBase = 0xC17090u, kRegion = 0x8B0, kTableAt = 0xC170D0u, kLevelsAt = 0xC177D0u;
constexpr u32 kRowBytes = 0x88;

// Desplazamientos en los objetos
constexpr u32 kPedPos = 4, kPedMatrix = 0x14, kPedClump = 0x18, kPedSpeed = 0x44, kPedGroup = 0x4D4, kPedMoveState = 0x534, kPedSwim = 0x538, kPedAim = 0x55C, kPedWeapons = 0x5A0, kPedSlot = 0x718, kPedStyle = 0x72D;
constexpr u32 kWeaponStride = 0x1C;
constexpr u32 kAnimHier = 0x14, kAnimWeight = 0x18, kAnimDelta = 0x1C, kAnimTime = 0x20, kAnimStep = 0x28, kAnimId = 0x2C, kAnimFlags = 0x2E;
constexpr u32 kHierTotal = 0x10;
constexpr u32 kAimInit = 0xFFFFFFFFu;

inline constexpr u32 kFloatEdges[]{0x00000000u, 0x80000000u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F800000u, 0xBF800000u, 0x3A83126Fu, 0xBA83126Fu, 0x7F7FFFFFu, 0xFF7FFFFFu,
                                   0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x3F666666u, 0x3F666667u, 0x3F666665u, 0x3ECCCCCDu, 0x3ECCCCCCu, 0x3ECCCCCEu, 0xC0800000u, 0xC0800001u, 0xC07FFFFFu, 0x3C23D70Au};
inline constexpr u32 kNextEdges[]{0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 0xAu, 0xBu, 0xCu, 0xDu, 0xEu, 0xFu, 0x10u, 0x11u, 0x7Fu, 0x80u, 0xFFu};
inline constexpr u32 kLastEdges[]{0u, 1u, 2u, 3u, 4u, 8u, 9u, 0xAu, 0xBu, 0xFu, 0x10u, 0x11u, 0x12u, 0xFFu};

struct Wr { u32 addr, value; };

struct C {
    u32 family = 0;
    // tarea
    u32 finished = 0, inControl = 0, animsRef = 0, reqGroup = 0x21, idlePeriod = 0, idleCounter = 0, combo = 0, move = 0, next = 0, last = 0;
    u32 hasTarget = 0, hasAnim = 0, hasIdle = 0, hasHier = 1;
    // asociaciones
    u32 animW = 0, animD = 0, animTime = 0, animStep = 0, animFlags = 0, animId = 0, hierTotal = 0, idleW = 0, idleD = 0;
    // ped
    u32 isPlayer = 0, moveState = 0, slot = 0, wtype = 0, style = 4, group = 0, clump = 0, hasMatrix = 1, swim0 = 0;
    std::array<u32, 12> mat{};       // right(3), forward(3), up(3), pos(3)
    std::array<u32, 3> pedPos{}, speed{};
    u32 tgtHasMatrix = 0;
    std::array<u32, 3> tgtPos{}, tgtMatPos{};
    u32 camFlag = 0, getTarget = 0;
    std::array<u32, 3> camFront{};
    u32 timestep = 0;
    u32 baseCombo = 4;
    u32 rAvail = 0, rAttack = 0;
    std::array<u32, 4> groupBlock{};     // 0 = bloque nulo (se pide por nombre), 1..3 = bloque k del arena
    std::array<u32, 3> blockLoaded{};
    u32 nameBlock = 1;
    u32 tableSeed = 0;
    std::vector<Wr> writes;
    u32 eax = 0, edx = 0, stack = 0;
};

// ------------------------------------------------------------------------------------------------ generador
struct Gen {
    u32 n, k = 100;
    u32 R() { return Rand(n, k++); }
    u32 Pick(u32 m) { return R() % m; }
    bool Chance(u32 num, u32 den) { return Pick(den) < num; }
    u32 EdgeF() { return kFloatEdges[Pick(u32(std::size(kFloatEdges)))]; }
    // k * 2^e exacto (|k| < 2^24)
    static u32 Fx(i32 kk, int e) { return std::bit_cast<u32>(std::ldexp(float(kk), e)); }
    // float normal con exponente sesgado en [lo, hi]
    u32 Norm(u32 lo, u32 hi, bool neg = false) { return (neg ? 0x80000000u : 0u) | ((lo + Pick(hi - lo + 1)) << 23) | (R() & 0x7FFFFFu); }
    u32 Time() { return Norm(0x76, 0x80); }                      // ~0.01 .. 2
    u32 Small() { return Norm(0x70, 0x78); }
    u32 OneOf(std::initializer_list<u32> l) { auto it = l.begin(); std::advance(it, Pick(u32(l.size()))); return *it; }
};

inline i32 S8(u32 v) { return i32(std::int8_t(v & 0xFFu)); }
inline float BitsToFloat(u32 b) { float f; std::memcpy(&f, &b, 4); return f; }

struct RowSpec {
    u32 group, hit[5], chain[5], block1, block2, level[5], sound[5], flagsWord;
};

// Familias (n % 16, 288 casos cada una); todas parten del mismo estado aleatorio valido y fuerzan lo que las caracteriza:
//  0 finalizada con reposo   1 finalizada sin reposo     2 sin control (siguiente < 1 / anim 0x2F)    3 contador de espera (paso de tiempo de borde, vueltas del u16)
//  4 sin reposo con ultimo comando de fin    5 crea el reposo (arma, estilo, filas con/sin bandera, grupos, bloques)     6 igual con ped en movimiento / no jugador
//  7 sin anim de golpe (comandos, IsPlayer, FindTarget, ChooseAttack)     8 anim con combo < 4 (ultimo 8/0xA/3)    9 anim con combo >= 4 y ultimo 2 (parar, desvanecer, soltar)
// 10 anim de golpe: cruce del instante del golpe (sonido, nivel, FightStrike)    11 anim de golpe: cadena (siguiente 0xB..0xE, movimientos 0/1/3/4)
// 12 orientacion (objetivo con/sin matriz, camara, pad)     13 bordes de coma flotante en pesos, tiempos y paso     14..15 mezcla aleatoria
inline C MakeC(u32 n) {
    Gen g{n};
    C c;
    const u32 f = n % 16;
    c.family = f;
    c.tableSeed = Rand(n, 70);
    c.eax = Rand(n, 20);
    c.edx = Rand(n, 21);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);

    // ---- estado aleatorio valido
    c.finished = g.Chance(1, 10) ? (g.Chance(1, 2) ? 1u : 1 + g.Pick(255)) : 0u;
    c.inControl = g.Chance(1, 2) ? 0u : (g.Chance(2, 3) ? 1u : 1 + g.Pick(255));
    c.reqGroup = g.Chance(1, 3) ? 0x21u : g.Pick(4);
    c.animsRef = g.Chance(1, 2) ? 0u : (g.Chance(2, 3) ? 1u : 1 + g.Pick(255));
    c.idlePeriod = g.Chance(1, 4) ? g.OneOf({0, 1, 100, 0x7FFF, 0xFFFE, 0xFFFF}) : g.R() & 0xFFFFu;
    c.idleCounter = g.Chance(1, 4) ? g.OneOf({0, 1, 99, 100, 101, 0x7FFF, 0x8000, 0xFFFE, 0xFFFF}) : g.R() & 0xFFFFu;
    c.combo = g.Chance(1, 4) ? g.Pick(4) : (g.Chance(1, 6) ? 0xFFu - g.Pick(4) : 4 + g.Pick(12));
    c.move = g.Pick(5);
    c.next = g.Chance(1, 2) ? g.OneOf({0, 1, 2, 3, 4, 5, 6, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x7F, 0x80, 0xFF}) : g.R() & 0xFFu;
    c.last = g.Chance(1, 2) ? g.OneOf({0, 1, 2, 3, 4, 5, 8, 9, 0xA, 0xB, 0xF, 0x10, 0x11, 0x12, 0xFF}) : g.R() & 0xFFu;
    c.hasTarget = g.Chance(1, 3);
    c.hasAnim = g.Chance(1, 2);
    c.hasIdle = g.Chance(3, 5);
    c.isPlayer = g.Chance(1, 2);
    c.moveState = g.Chance(7, 10) ? g.Pick(5) : (g.Chance(2, 3) ? 5 + g.Pick(4) : 0xFFFFFFFFu - g.Pick(3));
    c.slot = g.Pick(13);
    c.wtype = g.Chance(1, 8) ? 9u : (g.Chance(1, 4) ? 0u : g.Pick(46));
    c.style = 4 + g.Pick(12);
    c.baseCombo = g.Chance(1, 3) ? 4u : 4 + g.Pick(12);
    c.group = g.R();
    c.clump = g.R();
    c.swim0 = g.Chance(1, 4) ? 1u : g.R();
    c.hasMatrix = 1;
    for (int i = 0; i < 9; ++i) c.mat[i] = g.Norm(0x7A, 0x7F, g.Chance(1, 3));
    for (int i = 0; i < 3; ++i) c.mat[9 + i] = g.Norm(0x78, 0x85, g.Chance(1, 2));
    for (int i = 0; i < 3; ++i) {
        c.pedPos[i] = g.Norm(0x78, 0x85, g.Chance(1, 2));
        c.speed[i] = g.Norm(0x70, 0x7E, g.Chance(1, 2));
        c.tgtPos[i] = g.Norm(0x78, 0x85, g.Chance(1, 2));
        c.tgtMatPos[i] = g.Norm(0x78, 0x85, g.Chance(1, 2));
    }
    c.tgtHasMatrix = g.Chance(1, 2);
    c.camFlag = g.Chance(2, 3) ? 1u : (g.Chance(1, 2) ? 0u : 1 + g.Pick(255));
    c.getTarget = g.Chance(2, 3) ? 1u : (g.Chance(1, 2) ? 0u : 1 + g.Pick(255));
    for (int i = 0; i < 3; ++i) c.camFront[i] = g.Chance(1, 8) ? g.OneOf({0u, 0x80000000u, 0x3F800000u, 0xBF800000u}) : g.Norm(0x78, 0x7F, g.Chance(1, 2));
    c.timestep = g.Chance(1, 5) ? g.OneOf({0u, 0x80000000u, 0x3F800000u, 0x41D00000u, 0x7F800000u, 0x7FC00000u, 0xBF800000u, 0x5F000000u, 0x7F7FFFFFu, 0x3C23D70Au}) : g.Norm(0x78, 0x82);
    c.rAvail = g.R();
    c.rAttack = g.Chance(1, 3) ? (g.R() & 0xFFFF0000u) | 3u : (g.Chance(1, 2) ? g.R() : (g.R() & 0xFFFF0000u) | g.Pick(8));
    for (int i = 0; i < 4; ++i) c.groupBlock[i] = g.Pick(4);
    for (int i = 0; i < 3; ++i) c.blockLoaded[i] = g.Chance(1, 2) ? 0u : (g.Chance(1, 2) ? 1u : 1 + g.Pick(255));
    c.nameBlock = 1 + g.Pick(3);

    // ---- asociaciones
    c.animW = g.Chance(2, 3) ? g.Norm(0x7E, 0x7F) : g.EdgeF();
    c.animD = g.Chance(1, 2) ? g.Norm(0x70, 0x7E) : (g.Chance(1, 2) ? 0u : g.EdgeF());
    c.animTime = g.Time();
    c.animStep = g.Small();
    c.animFlags = g.Chance(1, 2) ? 1u : (g.R() & 0xFFFFu);
    c.animId = g.Chance(1, 8) ? 0x2Fu : g.R() & 0xFFFFu;
    c.hierTotal = g.Chance(1, 8) ? g.EdgeF() : g.Norm(0x7F, 0x82);
    c.idleW = g.Chance(1, 2) ? g.Norm(0x7A, 0x7F) : g.EdgeF();
    c.idleD = g.Chance(1, 2) ? g.Norm(0x70, 0x7E) : (g.Chance(1, 2) ? 0u : g.EdgeF());

    // ---- filas de la tabla de combos (combos 4..15 = filas 0..11)
    std::array<RowSpec, 12> rows{};
    for (auto& r : rows) {
        r.group = g.Chance(1, 4) ? 0x21u : g.Pick(4);
        for (int m = 0; m < 5; ++m) {
            r.hit[m] = g.Time();
            r.chain[m] = g.Chance(1, 2) ? g.Time() : r.hit[m] + 1 + g.Pick(40);
            r.level[m] = g.Chance(3, 10) ? 7u : g.Pick(7);
            r.sound[m] = g.Chance(1, 2) ? 0x3Cu + g.Pick(9) : g.R() & 0xFFFFu;
        }
        r.block1 = g.Time();
        r.block2 = g.Chance(1, 2) ? g.Time() : r.block1 + 1 + g.Pick(40);
        r.flagsWord = (g.Chance(2, 3) ? 0x0400u : 0u) | (g.R() & 0xFBFFu & (g.Chance(1, 2) ? 0xFFFFu : 0u));
    }

    // ---- ayudantes de familia
    auto Row = [&]() -> RowSpec& { return rows[(c.combo >= 4 && c.combo <= 15 ? c.combo : 4) - 4]; };
    auto WeightOk = [&]() { c.animW = g.Chance(7, 8) ? 0x3F666667u + g.Pick(0x199990u) : g.OneOf({0x3F666666u, 0x3F666665u, 0x3F800000u}); };    // > 0.9 (o justo en el borde)
    auto DeltaOk = [&]() { c.animD = g.Chance(1, 4) ? 0u : (g.Chance(1, 8) ? 0x80000000u : g.Norm(0x70, 0x7E)); };
    auto FoldCombo = [&](u32 lo, u32 hi) { c.combo = lo + g.Pick(hi - lo + 1); };
    auto HitCross = [&](u32 mode) {
        if (c.combo < 4 || c.combo > 15) return;
        const RowSpec& r = Row();
        const u32 hit = r.hit[c.move], chain = r.chain[c.move];
        switch (mode) {
        case 0: c.animTime = hit + 1 + g.Pick(64); c.animStep = g.Norm(0x70, 0x78); break;                          // cruza: t > golpe y t - paso < golpe
        case 1: c.animTime = hit; c.animStep = g.Small(); break;                                                      // t == golpe: no cruza
        case 2: c.animTime = hit + 1 + g.Pick(64); c.animStep = 1 + g.Pick(3); break;                                 // paso diminuto: t - paso >= golpe
        case 3: { const i32 kk = 1 + i32(g.Pick(1000)), mm = 1 + i32(g.Pick(1000)); c.animTime = Gen::Fx(kk + mm, -12); c.animStep = Gen::Fx(mm, -12); break; }   // t - paso == k * 2^-12 exacto (el golpe se ajusta)
        case 4: c.animTime = chain + g.Pick(40); c.animStep = g.Chance(1, 2) ? g.Small() : 1 + g.Pick(3); break;     // t >= cadena
        case 5: c.animTime = chain - 1 - g.Pick(40); c.animStep = g.Small(); break;                                   // t < cadena
        default: break;
        }
    };

    switch (f) {
    case 0: c.finished = g.Chance(1, 2) ? 1u : 1 + g.Pick(255); c.hasIdle = 1;
            if (g.Chance(1, 2)) { c.idleW = g.Norm(0x7A, 0x7F); c.idleD = g.Chance(1, 2) ? 0u : g.Norm(0x70, 0x7E); } break;
    case 1: c.finished = g.Chance(1, 2) ? 1u : 1 + g.Pick(255); c.hasIdle = 0; break;
    case 2: c.finished = 0; c.inControl = 0; c.next = g.Chance(1, 2) ? g.OneOf({0u, 0x80u, 0xFFu, 0xF0u, 0x81u}) : 1 + g.Pick(255); if (g.Chance(1, 2)) { c.hasAnim = 1; c.animId = g.Chance(2, 3) ? 0x2Fu : g.R() & 0xFFFFu; } break;
    case 3: c.finished = 0;
            c.timestep = g.Chance(1, 2) ? g.OneOf({0u, 0x80000000u, 0x3F800000u, 0x41D00000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xBF800000u, 0x5F000000u, 0x7F7FFFFFu, 0x3C23D70Au, 0x47800000u, 0x43480000u}) : g.Norm(0x70, 0x90, g.Chance(1, 8));
            c.idleCounter = g.Chance(1, 2) ? 0xFFFFu - g.Pick(40) : g.R() & 0xFFFFu; if (g.Chance(1, 2)) { c.inControl = 1; c.combo = 1 + g.Pick(15); } break;
    case 4: c.finished = 0; c.inControl = 1; c.hasIdle = 0; c.last = g.OneOf({1u, 0xFu, 0x10u, 0x11u}); break;
    case 5: case 6:
            c.finished = 0; c.inControl = 1; c.hasIdle = 0; c.hasAnim = 0; c.last = g.OneOf({0u, 2u, 3u, 4u, 8u, 9u, 0xAu, 0x12u, 0xFFu});
            c.moveState = f == 5 ? g.Pick(5) : 5 + g.Pick(4); c.isPlayer = f == 5 ? g.Chance(1, 2) : g.Chance(1, 4);
            {   // la fila elegida: arma -> combo base (o estilo); se relaciona con el grupo exigido y los bloques
                const u32 combo = c.baseCombo == 4 ? c.style : c.baseCombo;
                RowSpec& r = rows[combo - 4];
                const u32 mode = g.Pick(6);
                if (mode == 0) r.group = 0x21;
                else if (mode == 1) { if (c.reqGroup == 0x21) c.reqGroup = g.Pick(4); r.group = c.reqGroup; c.animsRef = 1 + g.Pick(2); }   // mismo grupo y referenciado
                else if (mode == 2) { if (c.reqGroup == 0x21) c.reqGroup = g.Pick(4); r.group = c.reqGroup; c.animsRef = 0; }               // mismo grupo sin referencia
                else if (mode == 3) { r.group = (c.reqGroup + 1 + g.Pick(3)) & 3; if (c.reqGroup == 0x21) c.animsRef = 0; }       // grupo distinto
                if (g.Chance(1, 5)) r.flagsWord &= ~0x0400u; else r.flagsWord |= 0x0400u;
                c.nameBlock = 1 + g.Pick(3);
            }
            break;
    case 7: c.finished = 0; c.inControl = 1; c.hasIdle = g.Chance(5, 6); c.hasAnim = 0; c.last = g.Chance(1, 4) ? 0x10u : g.OneOf({0u, 2u, 3u, 4u, 5u, 8u, 9u, 0xAu, 0xFFu});
            c.next = g.Chance(1, 2) ? g.OneOf({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 0xBu, 0x80u}) : g.R() & 0xFFu; c.combo = g.Chance(1, 2) ? 0u : g.R() & 0xFFu;
            if (c.combo >= 4 && c.combo <= 0xF0) c.combo = 4 + g.Pick(12);
            c.idleCounter = g.Chance(1, 2) ? c.idlePeriod + 1 + g.Pick(3) : c.idleCounter; c.idleCounter &= 0xFFFF; c.reqGroup = g.Chance(1, 2) ? 0x21u : c.reqGroup; if (c.reqGroup == 0x21) c.animsRef = 0; break;
    case 8: c.finished = 0; c.inControl = 1; c.hasIdle = 1; c.hasAnim = 1; c.combo = g.Chance(1, 2) ? g.Pick(4) : 0xFFu - g.Pick(4); c.last = g.OneOf({8u, 0xAu, 3u, 3u, 8u, 0xAu, 2u, 0u});
            c.next = g.Chance(1, 2) ? 3u : g.R() & 0xFFu;
            {
                const i32 kk = 1000 + i32(g.Pick(2000));
                c.hierTotal = Gen::Fx(5 * kk, -10);
                c.animTime = Gen::Fx(2 * kk + i32(g.Pick(5)) - 2, -10);              // t / total alrededor de 0.4
                if (g.Chance(1, 8)) c.animTime = g.EdgeF();
                c.animD = g.OneOf({0xC0800000u, 0xC07FFFFFu, 0xC0800001u, 0u, 0x3F800000u, 0xBF800000u, 0x7FC00000u, 0xC0000000u});
            }
            break;
    case 9: c.finished = 0; c.inControl = 1; c.hasIdle = 1; c.hasAnim = 1; FoldCombo(4, 15); c.last = 2; c.next = g.Chance(3, 5) ? 2u : g.OneOf({0u, 1u, 0xBu, 0xCu, 0xFFu, 0x10u, 3u});
            c.animFlags = g.Chance(2, 3) ? (g.R() & 0xFFFFu) | 1u : (g.R() & 0xFFFEu);
            {
                RowSpec& r = Row();
                const u32 mode = g.Pick(4);
                if (mode == 0) { c.animTime = r.block1 - 1 - g.Pick(30); c.animStep = g.Small(); }                                     // dentro del bloqueo de golpe
                else if (mode == 1) { c.animTime = r.block2 - 1 - g.Pick(30); c.animStep = g.Small(); if (c.animTime < r.block1) c.animTime = r.block1; }   // dentro de la cadena
                else if (mode == 2) { c.animTime = g.Time(); c.animStep = g.Small(); }
                else { c.animTime = r.block1 + g.Pick(8); c.animStep = 1 + g.Pick(3); }
                if (g.Chance(1, 4)) { c.animW = g.Norm(0x7A, 0x7F); c.animD = g.Chance(1, 2) ? 0u : g.Norm(0x70, 0x7E); }
            }
            break;
    case 10: c.finished = 0; c.inControl = 1; c.hasIdle = 1; c.hasAnim = 1; FoldCombo(4, 15); c.last = g.OneOf({0u, 1u, 3u, 4u, 8u, 0xBu}); c.move = g.Pick(5);
            if (g.Chance(1, 4)) { c.combo = 5; c.move = g.Pick(3); }
            WeightOk(); DeltaOk(); HitCross(g.Chance(2, 3) ? 0 : g.Pick(4)); c.wtype = g.Chance(1, 6) ? 9u : c.wtype;
            c.next = g.Chance(1, 2) ? g.OneOf({0xBu, 0xCu, 0xEu, 0xFu}) : c.next; break;
    case 11: c.finished = 0; c.inControl = 1; c.hasIdle = 1; c.hasAnim = 1; FoldCombo(4, 15); if (g.Chance(1, 2)) c.combo = 0xC; c.last = g.OneOf({0u, 1u, 3u, 4u, 8u, 0xBu}); c.isPlayer = g.Chance(3, 5);
            c.move = g.OneOf({0u, 1u, 3u, 3u, 4u, 4u, 4u, 2u}); c.next = g.Chance(3, 4) ? 0xBu + g.Pick(4) : g.R() & 0xFFu; WeightOk(); DeltaOk(); HitCross(g.Chance(3, 4) ? 4 : g.Pick(6));
            if (g.Chance(1, 3)) c.rAttack = (c.rAttack & 0xFFFF0000u) | 3u; break;
    case 12: c.finished = 0; c.inControl = 1; c.hasIdle = 1; c.hasAnim = 0; c.next = 0; c.combo = 0; c.last = g.Pick(3); c.hasTarget = g.Chance(2, 3); c.hasMatrix = g.Chance(1, 2); c.reqGroup = 0x21; c.animsRef = 0;
            if (g.Chance(1, 4)) { for (int i = 0; i < 3; ++i) { c.tgtPos[i] = c.pedPos[i]; c.tgtMatPos[i] = c.mat[9 + i]; } }
            break;
    case 13: c.finished = 0; c.inControl = 1; c.hasIdle = 1; c.hasAnim = 1; FoldCombo(4, 15); c.last = g.OneOf({0u, 2u, 3u, 8u});
            for (u32* p : {&c.animW, &c.animD, &c.animTime, &c.animStep, &c.hierTotal, &c.idleW, &c.idleD}) if (g.Chance(1, 2)) *p = g.EdgeF();
            if (g.Chance(1, 3)) c.combo = g.Pick(4);
            break;
    default: break;
    }

    // ---- ajustes de validez (el EXE desreferenciaria algo que el banco no modela)
    if (c.hasAnim) c.hasMatrix = 1;                                                              // el golpe usa la matriz del ped
    const bool needHier = c.hasAnim && S8(c.combo) < 4 && (c.last == 8 || c.last == 0xA);
    c.hasHier = needHier ? 1u : (g.Chance(7, 8) ? 1u : 0u);
    if (c.reqGroup == 0x21) c.animsRef = 0;
    if (c.hasAnim) c.move %= 5;
    if (c.combo >= 4 && c.combo <= 15 && c.hasAnim && c.family != 13) {}                           // las filas siempre existen para 4..15
    if (c.combo > 15 && c.combo < 0xF0) c.combo = 4 + (c.combo % 12);

    // ---- escrituras en la region (filas y vectores de nivel)
    for (u32 i = 0; i < 12; ++i) {
        const RowSpec& r = rows[i];
        const u32 a = kTableAt + i * kRowBytes;
        c.writes.push_back({a, r.group});
        for (u32 m = 0; m < 5; ++m) {
            c.writes.push_back({a + 0x08 + 4 * m, r.hit[m]});
            c.writes.push_back({a + 0x1C + 4 * m, r.chain[m]});
            c.writes.push_back({a + 0x5C + 4 * m, r.sound[m]});
        }
        c.writes.push_back({a + 0x48, r.block1});
        c.writes.push_back({a + 0x4C, r.block2});
        c.writes.push_back({a + 0x50, r.level[0] | (r.level[1] << 8) | (r.level[2] << 16) | (r.level[3] << 24)});
        c.writes.push_back({a + 0x54, r.level[4] | (g.R() & 0xFFFFFF00u)});
        c.writes.push_back({a + 0x84, r.flagsWord});
    }
    for (u32 v = 0; v < 7; ++v)
        for (u32 k = 0; k < 3; ++k) c.writes.push_back({kLevelsAt + 12 * v + 4 * k, g.Norm(0x78, 0x82, g.Chance(1, 3))});
    return c;
}
}  // namespace FightProcessPed31Fix

#pragma once
// Fixtures del banco DecisionPedGroup27: CDecisionMakerTypes::MakeDecision Ped (0x606E70, 11 argumentos, ret 2Ch) y Group (0x606F80, 8 argumentos, ret 20h).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco.
//
// 2560 casos por funcion = 2048 del banco original (DependenciesDecisionPed7: mismas formulas de reparto por `scenario`) + 512 casos de rand (familias 1..8).
//   Casos 0..2047 (familia 0): idénticos en reparto al banco original: selector de decision maker dmIds[(s/8)%6] (-2,-1,0,1,7,19) y el otro selector el siguiente; eventId=(s/8)%96,
//     eventIndex=(s/4)%41, sourceType=s%4, inVehicle=(s/4)%2; avoid1=1001, avoid2=1002 si s&64, avoid3=1003 si s&128, seek=1004 si s&256; Ped: tipo ped484 = 2 si s&16 (si no 1),
//     useGroup = s&32; Group: byte 2D0 = 1 si s&16. Las seis opciones de la decision seleccionada: tarea 1000+n (o -1 si s&512), probabilidades 1+(n+r+s)%99 (o 0 si s&1024),
//     bools (s+n)&1 y ((s/2)+n)&1.
//   Casos 2048..2559 (familias 1..8, 64 cada una; k = (indice-2048)%8): la decision seleccionada se construye con opciones validas/filtradas y la tirada de rand deseada
//     se coloca eligiendo el estado inicial del RNG del EXE (SeedForRoll, inversion del LCG):
//       1 umbral: tirada = acumulado/total*32767 +-2 (2..6 opciones)       5 seek HALLADO entre las opciones validas (no se consume rand)
//       2 tirada 32767 con totales «malos» 41/47/55/61 (roll 1.0f sin acierto)   6 seek NO hallado (inexistente, filtrado por avoid1 o por probabilidad 0) + tirada aleatoria
//       3 tirada 32767 con otros totales (incl. 1530 = seis opciones a 255)   7 tirada aleatoria con 1..6 opciones y probabilidades aleatorias
//       4 tirada 0                                                           8 seis opciones 1/255 con umbral +-2
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace DecisionPedGroup27Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 2048;
constexpr unsigned kRandCases = 512;
constexpr unsigned kCasesPerFunction = kBaseCases + kRandCases;   // 2560
constexpr unsigned kChoiceBytes = 60;                              // sizeof(CDecision)

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

// ------------------------------------------------------------------------------------------------ RNG del EXE (rand 0x821B1E, srand 0x821B11)
constexpr u32 NextState(u32 x) { return x * 0x343FDu + 0x269EC3u; }
constexpr u32 InverseOf343FD() { const u32 a = 0x343FDu; u32 x = a; for (int i = 0; i < 6; ++i) x *= 2u - a * x; return x; }
constexpr u32 RandOutput(u32 state) { return (NextState(state) >> 16) & 0x7FFFu; }          // primera salida de rand() desde `state`
// estado inicial cuya primera salida de rand() es `r`; `junk` rellena el bit 31 y los 16 bits bajos del estado siguiente
constexpr u32 SeedForRoll(u32 r, u32 junk) {
    const u32 next = ((junk & 1u) << 31) | ((r & 0x7FFFu) << 16) | ((junk >> 8) & 0xFFFFu);
    return (next - 0x269EC3u) * InverseOf343FD();
}
static_assert(RandOutput(SeedForRoll(32767u, 0x12345678u)) == 32767u && RandOutput(SeedForRoll(0u, 0xFFFFFFFFu)) == 0u && RandOutput(SeedForRoll(12345u, 7u)) == 12345u);

// ------------------------------------------------------------------------------------------------ casos
inline constexpr std::array<i32, 6> kDmIds{-2, -1, 0, 1, 7, 19};
inline constexpr std::array<u32, 4> kMissTotals{41, 47, 55, 61};
inline constexpr std::array<u32, 34> kTotals{41, 47, 55, 61, 82, 83, 94, 97, 107, 109, 6, 7, 10, 11, 12, 13, 14, 15, 18, 20, 22, 23, 24, 25, 100, 255, 256, 300, 510, 1000, 1275, 1529, 1530, 3};

struct Case {
    u32 func{};            // 0 = Ped (0x606E70), 1 = Group (0x606F80)
    u32 index{};           // 0..2559 dentro de la funcion
    u32 family{};          // 0 = banco original; 1..8 = casos de rand
    i32 dmB4{}, dmB8{};    // Ped: intel+0xB4 / +0xB8 (selector sin grupo / con grupo)
    u32 useGroup{};        // Ped: argumento bUseInGroupDecisionMaker (0/1)
    i32 groupId{};         // Group: group+0x2C8
    u32 pedType{};         // Ped: byte ped+0x484 (1 o 2)
    u32 missionFlag{};     // Group: byte group+0x2D0 (0 o 1)
    u32 eventId{}, eventIndex{}, sourceType{}, inVehicle{};
    i32 avoid[3]{}, seek{};
    u32 fillSeed{};        // relleno aleatorio del decision maker completo (antes de escribir indices y decision)
    u32 randState{};       // estado del RNG del EXE antes de la llamada (srand 0x821B11)
    u32 selectedOffset{};  // desplazamiento de la decision seleccionada dentro del decision maker
    u8 choice[kChoiceBytes]{};
};

// Selector efectivo y desplazamiento de la decision que lee el EXE (desensamblado 0x606E70 / 0x606F80).
inline i32 EffectiveDm(const Case& c) { return c.useGroup ? c.dmB8 : c.dmB4; }
inline u32 SelectedOffset(const Case& c) {
    if (c.func == 0) {
        const i32 dm = EffectiveDm(c);
        if (dm == -2) return 0xD4ECu + c.eventIndex * 60u;
        if (dm == -1) return (c.pedType == 2 ? 0xCB50u : 0xC1B4u) + c.eventIndex * 60u;
        return 4u + (u32(dm) * 41u + c.eventIndex) * 60u;
    }
    if (c.groupId == -1) return (c.missionFlag == 1 ? 0xE824u : 0xDE88u) + c.eventIndex * 60u;
    return 4u + (u32(c.groupId) * 41u + c.eventIndex) * 60u;
}

inline void PutChoice(u8* out, const i32 (&tasks)[6], const u8 (&probs)[6][4], const u8 (&bools)[6][2]) {
    for (int n = 0; n < 6; ++n) {
        std::memcpy(out + n * 4, &tasks[n], 4);
        for (int r = 0; r < 4; ++r) out[24 + n * 4 + r] = probs[n][r];
        out[48 + n * 2] = bools[n][0];
        out[49 + n * 2] = bools[n][1];
    }
}

// Reparte `total` en `count` valores 1..255 (requiere count <= total <= 255*count).
inline void Distribute(u32 total, u32 count, u32 c, u32 salt, u32* vals) {
    u32 rem = total - count;
    for (u32 q = 0; q < count; ++q) vals[q] = 1;
    for (u32 q = 0; q < count && rem; ++q) {
        u32 add = rem < 254 ? rem : 254;
        if (q + 1 < count) add = Rand(c, salt + q) % (add + 1);
        vals[q] += add; rem -= add;
    }
    for (u32 q = 0; q < count && rem; ++q) { u32 add = 255 - vals[q]; if (add > rem) add = rem; vals[q] += add; rem -= add; }
}

// Casos 0..2047: reparto identico al del banco original.
inline void BuildBase(Case& f) {
    const u32 s = f.index;
    const i32 dm = kDmIds[(s / 8) % 6], otherDm = kDmIds[((s / 8) + 1) % 6];
    f.eventId = (s / 8) % 96; f.eventIndex = (s / 4) % 41;
    f.sourceType = s % 4; f.inVehicle = (s / 4) % 2;
    f.avoid[0] = 1001; f.avoid[1] = (s & 64) ? 1002 : -1; f.avoid[2] = (s & 128) ? 1003 : -1; f.seek = (s & 256) ? 1004 : -1;
    if (f.func == 0) {
        f.dmB4 = dm; f.dmB8 = otherDm; f.pedType = (s & 16) ? 2 : 1; f.useGroup = (s & 32) ? 1 : 0;
    } else {
        f.groupId = dm == -2 ? -1 : dm; f.missionFlag = (s & 16) ? 1 : 0;
    }
    f.fillSeed = s * 2654435761u + 0x12893211u;
    f.randState = Rand(s, 1);
    i32 tasks[6]; u8 probs[6][4]{}; u8 bools[6][2]{};
    for (int n = 0; n < 6; ++n) {
        tasks[n] = (s & 512) ? -1 : 1000 + n;
        for (int r = 0; r < 4; ++r) probs[n][r] = (s & 1024) ? u8(0) : u8(1 + (n + r + s) % 99);
        bools[n][0] = u8((s + n) & 1); bools[n][1] = u8(((s / 2) + n) & 1);
    }
    PutChoice(f.choice, tasks, probs, bools);
}

// Casos 2048..2559: opciones validas/filtradas controladas y tirada colocada con SeedForRoll.
inline void BuildRand(Case& f) {
    const u32 c = f.index, r = c - kBaseCases, fam = r % 8;   // fam 0..7 -> familia 1..8
    const u32 s = c;                                          // las formulas de enrutado son las del banco original
    const i32 dm = kDmIds[(s / 8) % 6], otherDm = kDmIds[((s / 8) + 1) % 6];
    f.eventId = (s / 8) % 96; f.eventIndex = (s / 4) % 41;
    f.sourceType = s % 4; f.inVehicle = (s / 4) % 2;
    if (f.func == 0) { f.dmB4 = dm; f.dmB8 = otherDm; f.pedType = (s & 16) ? 2 : 1; f.useGroup = (s & 32) ? 1 : 0; }
    else { f.groupId = dm == -2 ? -1 : dm; f.missionFlag = (s & 16) ? 1 : 0; }
    f.avoid[0] = 1001; f.avoid[1] = (Rand(c, 290) & 1) ? 1002 : -1; f.avoid[2] = (Rand(c, 291) & 1) ? 1003 : -1; f.seek = -1;
    f.fillSeed = c * 2654435761u + 0x12893211u;
    const u32 st = f.sourceType, veh = f.inVehicle & 1u;

    int perm[6]{0, 1, 2, 3, 4, 5};
    for (int i = 5; i > 0; --i) { const int j = int(Rand(c, 300 + u32(i)) % u32(i + 1)); const int t = perm[i]; perm[i] = perm[j]; perm[j] = t; }
    u32 want = 1 + Rand(c, 301) % 6;
    if (fam == 0) want = 2 + Rand(c, 301) % 5;
    if (fam == 7) want = 6;
    u32 vals[6]{};
    switch (fam) {
    case 0: for (u32 q = 0; q < want; ++q) vals[q] = 1 + Rand(c, 310 + q) % 60; break;
    case 1: case 2: {
        u32 total = 0;
        for (u32 a = 0; a < u32(kTotals.size()) + 1 && !total; ++a) {
            const u32 cand = fam == 1 ? kMissTotals[(Rand(c, 311) + a) % kMissTotals.size()] : kTotals[(Rand(c, 312) + a) % kTotals.size()];
            if (cand >= want && cand <= 255u * want) total = cand;
        }
        if (!total) total = want;
        Distribute(total, want, c, 320, vals);
        break; }
    case 7: for (u32 q = 0; q < want; ++q) vals[q] = (Rand(c, 310 + q) & 1) ? 255u : 1 + Rand(c, 316 + q) % 3; break;
    default:
        for (u32 q = 0; q < want; ++q) {
            switch (Rand(c, 305 + q) % 4) { case 0: vals[q] = 1; break; case 1: vals[q] = 255; break; default: vals[q] = 1 + Rand(c, 313 + q) % 255; break; }
        }
        break;
    }

    i32 tasks[6]; u8 probs[6][4]; u8 bools[6][2]; u8 validProb[6]{}; bool valid[6]{}; int filteredTaskSlot = -1;
    for (int i = 0; i < 6; ++i) {
        tasks[i] = 3000 + i * 13;
        for (int j = 0; j < 4; ++j) probs[i][j] = u8(Rand(c, 330 + u32(i) * 4 + u32(j)));
        for (int j = 0; j < 2; ++j) bools[i][j] = u8(Rand(c, 360 + u32(i) * 2 + u32(j)) & 1);
    }
    for (u32 q = 0; q < 6; ++q) {
        const int i = perm[q];
        if (q < want) {
            tasks[i] = 2000 + i * 7; probs[i][st] = u8(vals[q]); bools[i][veh] = 1; valid[i] = true; validProb[i] = u8(vals[q]);
        } else {
            const u32 p = 1 + Rand(c, 370 + q) % 255;
            switch (Rand(c, 380 + q) % 4) {
            case 0: tasks[i] = 2000 + i * 7; probs[i][st] = 0; bools[i][veh] = 1; if (filteredTaskSlot < 0) filteredTaskSlot = i; break;   // probabilidad 0
            case 1: tasks[i] = -1; probs[i][st] = u8(p); bools[i][veh] = 1; break;                                                          // tarea -1
            case 2: tasks[i] = f.avoid[0]; probs[i][st] = u8(p); bools[i][veh] = 1; break;                                                   // tarea = avoid1
            default: tasks[i] = 2000 + i * 7; probs[i][st] = u8(p); bools[i][veh] = 0; break;                                                // bool del tipo de vehiculo a 0
            }
        }
    }

    // acumulados en el orden de las ranuras (el EXE reune las opciones validas con i ascendente)
    u32 total = 0; for (int i = 0; i < 6; ++i) if (valid[i]) total += validProb[i];
    u32 cumAt[6]{}; int nValid = 0; u32 run = 0; int firstValid = -1;
    for (int i = 0; i < 6; ++i) if (valid[i]) { run += validProb[i]; cumAt[nValid++] = run; if (firstValid < 0) firstValid = i; }
    auto thresholdRoll = [&](u32 salt) {
        const u32 k = Rand(c, salt) % u32(nValid);
        const i32 base = i32((u64(cumAt[k]) * 32767u + total / 2) / total) + i32(Rand(c, salt + 1) % 5) - 2;
        return base < 0 ? 0u : base > 32767 ? 32767u : u32(base);
    };
    u32 roll;
    switch (fam) {
    case 0: case 7: roll = thresholdRoll(400); break;
    case 1: case 2: roll = 32767; break;
    case 3: roll = 0; break;
    case 4: roll = Rand(c, 402) & 0x7FFFu; {   // seek hallado: una de las tareas validas
        int pick = int(Rand(c, 403) % u32(nValid)), seen = 0;
        for (int i = 0; i < 6; ++i) if (valid[i]) { if (seen++ == pick) { f.seek = tasks[i]; break; } }
        } break;
    case 5: roll = Rand(c, 404) & 0x7FFFu; {   // seek no hallado
        switch (Rand(c, 405) % 3) { case 0: f.seek = 7777; break; case 1: f.seek = f.avoid[0]; break; default: f.seek = filteredTaskSlot >= 0 ? tasks[filteredTaskSlot] : 7777; break; }
        } break;
    default: roll = Rand(c, 406) & 0x7FFFu; break;
    }
    f.randState = SeedForRoll(roll, Rand(c, 407));
    PutChoice(f.choice, tasks, probs, bools);
}

inline Case MakeCase(u32 func, u32 index) {
    Case f{};
    f.func = func; f.index = index;
    f.family = index < kBaseCases ? 0u : 1u + (index - kBaseCases) % 8u;
    if (index < kBaseCases) BuildBase(f); else BuildRand(f);
    f.selectedOffset = SelectedOffset(f);
    return f;
}
}

#pragma once
// Fixtures del banco DecisionMakeDecision27: CDecision::MakeDecision en 0x6040D0
// (thiscall, 8 argumentos en pila: sourceType, inVehicle, avoid1, avoid2, avoid3, seek, int16& taskSlot, int16& facialSlot; `ret 0x20`).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco.
//
// Objeto CDecision (0x3C bytes): m_Tasks int32[6] en +0x00, m_Probs uint8[6][4] en +0x18, m_Bools bool[6][2] en +0x30.
// La tirada del EXE es la primera salida de rand() (0x821B1E, LCG 0x343FD/0x269EC3, (estado>>16)&0x7FFF). Para colocar la tirada en un valor concreto
// (0, 32767, umbrales) se calcula la semilla que la produce: se invierte el LCG (SeedForRoll). srand del EXE = 0x821B11.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace DecisionMakeDecision27Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 6144;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline u64 Fnv64(const u8* p, std::size_t n) {
    u64 h = 0xCBF29CE484222325ull;
    for (std::size_t i = 0; i < n; ++i) { h ^= p[i]; h *= 0x100000001B3ull; }
    return h;
}
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }

// ------------------------------------------------------------------------------------------------ RNG del EXE
constexpr u32 NextState(u32 x) { return x * 0x343FDu + 0x269EC3u; }
constexpr u32 InverseOf343FD() { const u32 a = 0x343FDu; u32 x = a; for (int i = 0; i < 6; ++i) x *= 2u - a * x; return x; }
constexpr u32 RandOutput(u32 state) { return (NextState(state) >> 16) & 0x7FFFu; }          // primera salida de rand() desde `state`
// semilla (estado antes de la llamada) cuya primera salida es `r`; `junk` rellena el bit 31 y los 16 bits bajos del estado siguiente
constexpr u32 SeedForRoll(u32 r, u32 junk) {
    const u32 next = ((junk & 1u) << 31) | ((r & 0x7FFFu) << 16) | ((junk >> 8) & 0xFFFFu);
    return (next - 0x269EC3u) * InverseOf343FD();
}
static_assert(RandOutput(SeedForRoll(32767u, 0x12345678u)) == 32767u && RandOutput(SeedForRoll(0u, 0xFFFFFFFFu)) == 0u && RandOutput(SeedForRoll(12345u, 7u)) == 12345u);

// ------------------------------------------------------------------------------------------------ disposicion de memoria
constexpr u32 kObjOffset = 0x40;                 // CDecision dentro de su arena
constexpr u32 kObjBytes = 0x3C;                  // sizeof(CDecision)
constexpr u32 kObjArena = kObjOffset + kObjBytes + 0x40;
constexpr u32 kSlotsBytes = 32;                  // arena de salidas: taskSlot en +8, facialSlot en +20 (int16), el resto es guarda
constexpr u32 kTaskSlotAt = 8, kFacialSlotAt = 20;

// ------------------------------------------------------------------------------------------------ valores notables
inline constexpr std::array<i32, 12> kSpecialTasks{200, 0x100C8, 0xFFFF, 0x8000, 0x7FFFFFFF, i32(0x80000000u), -2, 1000000, 0, 1, 0x10000, 199};
inline constexpr std::array<i32, 17> kOutOfRangeSources{4, 5, 6, 7, 8, 11, 12, 17, 23, -1, -2, -3, -4, -5, -8, -12, -24};   // [-24, 23]: la lectura de probs queda dentro del objeto
inline constexpr std::array<u32, 10> kVehicleRaw{0u, 1u, 2u, 0x100u, 0x80u, 0xFFu, 0x01000000u, 0xFFFFFF00u, 0xFFFFFFFFu, 0xFFFFFF01u};   // el EXE solo mira el byte bajo
// totales (suma de probabilidades validas) para los que 1/total*total no da 1.0f en el x87 de 24 bits (tirada 32767 = roll 1.0f sin acierto) y otros
inline constexpr std::array<u32, 4> kMissTotals{41, 47, 55, 61};
inline constexpr std::array<u32, 34> kTotals{41, 47, 55, 61, 82, 83, 94, 97, 107, 109, 6, 7, 10, 11, 12, 13, 14, 15, 18, 20, 22, 23, 24, 25, 100, 255, 256, 300, 510, 1000, 1275, 1529, 1530, 3};

struct F {
    u32 kind, seed, eax, edx, stack, inVeh;
    i32 st, avoid[3], seek;
    i16 slot0, face0;
    u8 obj[kObjBytes];
};

inline i32 PickTask(u32 c, u32 k, bool special) {
    if (special || Rand(c, k) % 4 == 0) return kSpecialTasks[Rand(c, k + 1) % kSpecialTasks.size()];
    return i32(Rand(c, k + 2) % 1500u);
}
inline u8 AnyProb(u32 c, u32 k) {
    switch (Rand(c, k) % 5) { case 0: return 0; case 1: return 1; case 2: return 255; case 3: return 0; default: return u8(Rand(c, k + 1)); }
}
inline u8 ValidProb(u32 c, u32 k, u32 mode) {
    switch (mode) {
    case 0: return 1;
    case 1: return 255;
    case 2: return u8(1 + Rand(c, k) % 255);
    case 3: return (Rand(c, k) & 1) ? 255 : 1;
    default: return u8(1 + Rand(c, k) % 5);
    }
}
inline u8 TrueByte(u32 c, u32 k, bool raw) {   // m_Bools son bool (0/1); el EXE solo comprueba != 0: algun caso usa bytes no canonicos
    if (!raw) return 1;
    constexpr std::array<u8, 4> v{1, 2, 0x80, 0xFF};
    return v[Rand(c, k) % 4];
}

// Replica en el generador (no es el oraculo) el filtro de opciones para colocar umbrales de tirada.
struct Collected { i32 tasks[6]; u8 probs[6]; int count; };
inline Collected Collect(const F& f) {
    Collected o{}; for (auto& t : o.tasks) t = 200;
    const bool veh = (f.inVeh & 0xFFu) != 0;
    for (int i = 0; i < 6; ++i) {
        const u8 p = f.obj[0x18 + i * 4 + f.st];
        if (p == 0) continue;
        i32 t; std::memcpy(&t, f.obj + i * 4, 4);
        if (t == -1 || t == f.avoid[0] || t == f.avoid[1] || t == f.avoid[2]) continue;
        if (!f.obj[0x30 + i * 2 + (veh ? 1 : 0)]) continue;
        o.tasks[o.count] = t; o.probs[o.count] = p; ++o.count;
    }
    return o;
}

// kind (n % 16): 0 vacio (count 0); 1..6 count exacto; 7 totales especiales con tirada 32767/32766/umbral; 8 umbrales intermedios (+-2 alrededor del acumulado);
//   9 seis opciones a 255 (total 1530); 10 mezcla 0/1/255; 11 variantes de seek; 12 tareas iguales a avoid1/2/3; 13 bools por vehiculo y byte bajo de inVehicle;
//   14 sourceType fuera de rango; 15 tareas extremas (-1, INT_MIN, 0xFFFF, 200, ...).
inline F MakeF(unsigned n) {
    F f{};
    const u32 c = n;
    f.kind = n % 16;
    f.eax = Rand(c, 23); f.edx = Rand(c, 24); f.stack = StackSeed(n);
    f.slot0 = i16(Rand(c, 25)); f.face0 = i16(Rand(c, 26));
    const bool outOfRange = f.kind == 14;
    f.st = outOfRange ? kOutOfRangeSources[Rand(c, 3) % kOutOfRangeSources.size()] : i32(Rand(c, 2) % 4);
    f.inVeh = f.kind == 13 ? kVehicleRaw[Rand(c, 4) % kVehicleRaw.size()] : (Rand(c, 4) & 1);
    const bool veh = (f.inVeh & 0xFFu) != 0;
    const bool rawBools = f.kind == 13 || (Rand(c, 5) % 16) == 0;
    const bool special = f.kind == 15;

    u32 want;
    switch (f.kind) {
    case 0: want = 0; break;
    case 1: case 2: case 3: case 4: case 5: case 6: want = f.kind; break;
    case 7: want = 1 + Rand(c, 6) % 6; break;
    case 8: want = 2 + Rand(c, 6) % 5; break;
    case 9: want = 6; break;
    default: want = Rand(c, 6) % 7; break;
    }
    const u32 pmode = f.kind == 9 ? 1 : f.kind == 10 ? 3 : Rand(c, 7) % 5;

    // ---- objeto base: todo aleatorio
    for (int i = 0; i < 6; ++i) {
        i32 t = (Rand(c, 10 + i) % 8 == 0) ? -1 : PickTask(c, 20 + i * 4, special);
        std::memcpy(f.obj + i * 4, &t, 4);
        for (int j = 0; j < 4; ++j) f.obj[0x18 + i * 4 + j] = AnyProb(c, 40 + i * 4 + j);
        for (int j = 0; j < 2; ++j) f.obj[0x30 + i * 2 + j] = (Rand(c, 80 + i * 2 + j) & 1) ? TrueByte(c, 90 + i * 2 + j, rawBools) : 0;
    }
    f.avoid[0] = 0x7000; f.avoid[1] = 0x7001; f.avoid[2] = 0x7002;
    bool avSet[3]{};
    if (!outOfRange) {
        int perm[6]{0, 1, 2, 3, 4, 5};
        for (int i = 5; i > 0; --i) { const int j = int(Rand(c, 100 + i) % u32(i + 1)); const int t = perm[i]; perm[i] = perm[j]; perm[j] = t; }
        u32 prevTask = 0; bool havePrev = false;
        u32 probOf[6]{};
        for (u32 q = 0; q < 6; ++q) {
            const int i = perm[q];
            if (q < want) {   // opcion valida
                i32 t = (havePrev && Rand(c, 110 + q) % 6 == 0) ? i32(prevTask) : PickTask(c, 120 + q * 4, special);
                std::memcpy(f.obj + i * 4, &t, 4); prevTask = u32(t); havePrev = true;
                probOf[i] = ValidProb(c, 130 + q, pmode);
                f.obj[0x30 + i * 2 + (veh ? 1 : 0)] = TrueByte(c, 140 + q, rawBools);
            } else {          // opcion filtrada por una razon
                u32 reason = Rand(c, 150 + q) % 4;
                if (f.kind == 12 && (Rand(c, 160 + q) & 1)) reason = 2;
                const u32 p = 1 + Rand(c, 170 + q) % 255;
                switch (reason) {
                case 0: probOf[i] = 0; f.obj[0x30 + i * 2 + (veh ? 1 : 0)] = 1; break;
                case 1: probOf[i] = p; { i32 t = -1; std::memcpy(f.obj + i * 4, &t, 4); } f.obj[0x30 + i * 2 + (veh ? 1 : 0)] = 1; break;
                case 2: {
                    probOf[i] = p; f.obj[0x30 + i * 2 + (veh ? 1 : 0)] = 1;
                    const u32 k = Rand(c, 180 + q) % 3;
                    i32 t;
                    if (avSet[k]) t = f.avoid[k]; else { t = PickTask(c, 190 + q * 4, special); f.avoid[k] = t; avSet[k] = true; }
                    std::memcpy(f.obj + i * 4, &t, 4);
                    break; }
                default:
                    probOf[i] = p; { i32 t = PickTask(c, 200 + q * 4, special); std::memcpy(f.obj + i * 4, &t, 4); } f.obj[0x30 + i * 2 + (veh ? 1 : 0)] = 0; break;
                }
            }
        }
        // kind 7: ajustar los valores validos para que su suma sea un total especial
        if (f.kind == 7 && want > 0) {
            u32 T = 0;
            const bool miss = Rand(c, 210) & 1;
            for (u32 a = 0; a < kTotals.size() + 1; ++a) {
                const u32 cand = miss ? kMissTotals[(Rand(c, 211) + a) % kMissTotals.size()] : kTotals[(Rand(c, 212) + a) % kTotals.size()];
                if (cand >= want && cand <= 255u * want) { T = cand; break; }
            }
            if (!T) T = want;
            u32 vals[6]{}; u32 rem = T - want;
            for (u32 q = 0; q < want; ++q) vals[q] = 1;
            for (u32 q = 0; q < want && rem; ++q) { u32 add = rem < 254 ? rem : 254; if (q + 1 < want) add = Rand(c, 220 + q) % (add + 1); vals[q] += add; rem -= add; }
            for (u32 q = 0; q < want && rem; ++q) { u32 add = 255 - vals[q]; if (add > rem) add = rem; vals[q] += add; rem -= add; }
            for (u32 q = 0; q < want; ++q) probOf[perm[q]] = vals[q];
        }
        for (int i = 0; i < 6; ++i) f.obj[0x18 + i * 4 + f.st] = u8(probOf[i]);
        for (int k = 0; k < 3; ++k) if (!avSet[k]) { const u32 r = Rand(c, 230 + k); f.avoid[k] = r % 6 == 0 ? -1 : r % 11 == 0 ? 200 : 0x7000 + k; }
    } else {
        for (int k = 0; k < 3; ++k) f.avoid[k] = Rand(c, 230 + k) % 3 == 0 ? PickTask(c, 240 + k * 4, true) : 0x7000 + k;
    }

    // ---- seek
    const Collected col = Collect(f);
    u32 smode = f.kind == 11 ? (n / 16) % 6 : Rand(c, 250) % 10;
    if (f.kind != 11) { static constexpr u32 map[10]{0, 0, 0, 0, 1, 1, 2, 3, 4, 5}; smode = map[smode]; }
    if (f.kind == 7 || f.kind == 8) smode = 0;
    switch (smode) {
    case 0: f.seek = -1; break;
    case 1: f.seek = col.count > 0 ? col.tasks[Rand(c, 251) % u32(col.count)] : 5; break;
    case 2: f.seek = 0x6000 + i32(Rand(c, 252) % 1000); break;
    case 3: { const u32 w = Rand(c, 253) % 7; if (w < 6) std::memcpy(&f.seek, f.obj + w * 4, 4); else f.seek = f.avoid[Rand(c, 254) % 3]; } break;
    case 4: f.seek = 200; break;
    default: f.seek = 0x100C8; break;
    }

    // ---- tirada (primera salida de rand): valor concreto -> semilla
    u32 r;
    u32 rmode = f.kind == 7 ? (Rand(c, 260) % 10 < 6 ? 2 : Rand(c, 261) % 3 == 0 ? 3 : 4) : f.kind == 8 ? 4 : Rand(c, 262) % 8;
    switch (rmode) {
    case 1: r = 0; break;
    case 2: r = 32767; break;
    case 3: r = 32766; break;
    case 7: r = 1; break;
    case 4: {   // umbral: tirada ~ acumulado/total * 32767, +-2
        if (col.count > 0) {
            u32 total = 0; for (int i = 0; i < col.count; ++i) total += col.probs[i];
            const int k = int(Rand(c, 263) % u32(col.count)); u32 cum = 0; for (int i = 0; i <= k; ++i) cum += col.probs[i];
            const i32 rb = i32((u64(cum) * 32767u + total / 2) / total) + i32(Rand(c, 264) % 5) - 2;
            r = rb < 0 ? 0u : rb > 32767 ? 32767u : u32(rb);
        } else r = Rand(c, 265) & 0x7FFFu;
        break; }
    default: r = Rand(c, 266) & 0x7FFFu; break;
    }
    f.seed = SeedForRoll(r, Rand(c, 267));
    return f;
}
}

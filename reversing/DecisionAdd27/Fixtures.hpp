#pragma once
// Fixtures del banco DecisionAdd27: CDecision::Add (0x600600) y CDecisionMakerTypes::AddEventResponse (0x6044C0).
//   Add:              thiscall, 3 argumentos en pila (taskId, const float* chances[4], const int32* flags[2]); `ret 0Ch`.
//   AddEventResponse: thiscall, 5 argumentos en pila (decisionMakerIndex, eventType, taskId, float* chances, int32* flags); `ret 14h`.
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco.
//
// Objeto CDecision (0x3C bytes): m_Tasks int32[6] en +0x00, m_Probs uint8[6][4] en +0x18, m_Bools bool[6][2] en +0x30.
// Una ranura i esta LIBRE si tarea == -1, las 4 probabilidades == 0 (comparacion SIN signo: 0x80/0xFF la ocupan) y los 2 bytes de bool == 0 (cualquier byte != 0 la ocupa).
// Add rellena la PRIMERA libre: tarea, cada probabilidad = byte bajo de _ftol2(chances[j]) (0x821B40: truncado a int64; NaN/inf/|x|>=2^63 -> indefinido 0x8000000000000000, byte 0)
// y cada bool = (flags[k] != 0) como dword.
//
// AddEventResponse trabaja sobre un objeto CDecisionMakerTypes sintetico en una arena controlada (kMem bytes, `this` en kThis):
//   P = this + 0xC034 + eventType*4                          (m_EventIndices[eventType], aritmetica de 32 bits)
//   W = this + 4 + (u32[P] + decisionMakerIndex*41) * 0x3C   (aritmetica de 32 bits, SIN comprobar limites)  -> CDecision::Add(W, ...)
// Como 0x3C = 4*15 y 41 es impar, sumar k*2^30 a eventType, a decisionMakerIndex o al indice leido NO cambia P ni W (la multiplicacion da la vuelta en 32 bits):
// el generador usa esa propiedad, y tambien resuelve indices enormes que caen en una posicion arbitraria de la arena (modo 6). Todo valor generado
// cae SIEMPRE dentro de la arena (el generador descarta lo demas), asi que una fuente que no diera la vuelta en 32 bits escribiria fuera de la ventana y lo delataria `outside_ok`.
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace DecisionAdd27Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCasesAdd = 6144;
constexpr unsigned kCasesAer = 4608;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }

// ------------------------------------------------------------------------------------------------ disposicion de memoria
constexpr u32 kObjOffset = 0x40;                 // CDecision dentro de su arena (Add)
constexpr u32 kObjBytes = 0x3C;                  // sizeof(CDecision)
constexpr u32 kObjArena = kObjOffset + kObjBytes + 0x40;
constexpr u32 kArgsArena = 0x80;                 // arena de argumentos: chances (16 bytes) en +0x20, flags (8 bytes) en +0x58; el resto es guarda y se compara entero
constexpr u32 kChancesAt = 0x20, kFlagsAt = 0x58;
constexpr u32 kMem = 0x18000;                    // arena de AddEventResponse
constexpr u32 kThis = 0x3000;                    // `this` sintetico dentro de la arena
constexpr u32 kEventTable = 0xC034;              // m_EventIndices (EVENT_TOTAL_NUM_EVENTS = 96 entradas)
constexpr u32 kMakerDecisions = 41;              // CDecision::MAX_NUM_DECISIONS
constexpr u32 kDecisionStride = 0x3C;

constexpr u32 InverseOf15() { u32 x = 15; for (int i = 0; i < 6; ++i) x *= 2u - 15u * x; return x; }
static_assert(15u * InverseOf15() == 1u);
constexpr u32 WindowOf(u32 dm, u32 idx) { return kThis + 4u + (idx + dm * kMakerDecisions) * kDecisionStride; }   // W (32 bits, da la vuelta)
constexpr u32 TableOf(u32 ev) { return kThis + kEventTable + ev * 4u; }                                          // P
static_assert(WindowOf(0, 0) == kThis + 4u && WindowOf(1, 0) == kThis + 4u + 41u * 0x3Cu && WindowOf(0x40000000u, 0x40000000u) == kThis + 4u);
static_assert(TableOf(0x40000000u) == kThis + kEventTable && TableOf(0xFFFFFFFFu) == kThis + kEventTable - 4u);

// ------------------------------------------------------------------------------------------------ valores notables
constexpr u32 Fb(float f) { return std::bit_cast<u32>(f); }
// Bits de float para chances[j]. Nunca hay NaN senalizadores (el x87 los silencia al cargarlos). El oraculo trunca como el _ftol2 del EXE y se queda con el byte bajo.
inline constexpr u32 kSpecialChances[]{
    Fb(0.0f), 0x80000000u /* -0.0 */, Fb(0.5f), 0x3F7FFFFFu /* 0.99999994 */, Fb(1.0f), 0x3F800001u /* 1.0000001 */, Fb(1.5f), Fb(2.0f), Fb(12.0f), Fb(100.0f),
    Fb(127.0f), Fb(127.9f), Fb(128.0f), Fb(128.5f), Fb(200.0f), 0x437EFFFFu /* 254.99998 */, Fb(254.5f), Fb(255.0f), Fb(255.5f), Fb(255.9f),
    0x437FFFFFu /* 255.99998 */, Fb(256.0f), Fb(256.5f), Fb(257.0f), Fb(300.0f), Fb(511.0f), Fb(511.5f), Fb(512.0f), Fb(1000.0f), Fb(4095.9f),
    Fb(65535.0f), Fb(65535.5f), Fb(65536.0f), Fb(65543.0f) /* 65536+7 */, Fb(65791.0f) /* 65536+255 */, Fb(65792.0f), Fb(131200.0f) /* 131072+128 */,
    Fb(16777216.0f), 0x4B800001u /* 2^24+2 */, Fb(1073741952.0f) /* 2^30+128 */, Fb(2147483520.0f) /* 2^31-128 */, Fb(2147483648.0f), 0x4F000001u /* 2^31+256 */,
    Fb(4294967296.0f), Fb(4294967808.0f), 0x5EFFFFFFu /* mayor que cabe en int64 */, 0x5F000000u /* 2^63 */, 0x5F000001u, 0x5F800000u /* 2^64 */,
    Fb(1.0e19f), Fb(1.0e30f), 0x7F7FFFFFu /* FLT_MAX */, 0x7F800000u /* +inf */, 0x7FC00000u /* NaN */, 0x7FC12345u /* NaN con carga */, 0x7FFFFFFFu /* NaN */,
    Fb(-0.25f), Fb(-0.5f), 0xBF7FFFFFu /* -0.99999994 */, Fb(-1.0f), Fb(-1.5f), Fb(-2.0f), Fb(-100.0f), Fb(-127.5f), Fb(-128.0f), Fb(-254.5f),
    Fb(-255.0f), Fb(-255.9f), Fb(-256.0f), Fb(-257.0f), Fb(-300.0f), Fb(-65536.0f), Fb(-65537.0f), Fb(-16777216.0f),
    0xDEFFFFFFu /* menor que cabe en int64 */, 0xDF000000u /* -2^63 */, 0xDF000001u, 0xFF800000u /* -inf */, 0xFFC00000u /* NaN negativo */, 0xFFFFFFFFu /* NaN */,
    0xFF7FFFFFu /* -FLT_MAX */, 0x00000001u /* denormal */, 0x007FFFFFu /* denormal */, 0x80000001u /* denormal negativo */, Fb(1.0e-30f),
};
inline constexpr std::size_t kSpecialChanceCount = std::size(kSpecialChances);
inline constexpr u32 kBoundaryChances[]{   // frontera del byte: 255.x, 256.x, 65536+x
    Fb(254.0f), Fb(255.0f), Fb(255.5f), Fb(255.9f), 0x437FFFFFu, Fb(256.0f), Fb(256.5f), Fb(257.0f), Fb(257.9f), Fb(511.9f), Fb(512.0f), Fb(65535.9f), Fb(65536.0f), Fb(65536.5f),
    Fb(65537.0f), Fb(65538.9f), Fb(65543.0f), Fb(65791.0f), Fb(65791.9f), Fb(65792.0f), Fb(65793.0f), Fb(131071.0f), Fb(131072.0f), Fb(131073.0f), Fb(16777217.0f) /* 2^24 (no cabe 2^24+1) */,
};
inline constexpr i32 kFlagValues[]{0, 0, 0, 1, 1, -1, 2, 0x100, 0x80, 0xFF, 0x10000, 0x01000000, i32(0x80000000u), 0x7FFFFFFF, i32(0xFFFFFF00u), 0x00FFFF00, i32(0x80000001u), 0x7F};
inline constexpr i32 kTaskExtremes[]{-1, -1, 0, 1, i32(0x80000000u), 0x7FFFFFFF, 200, 199, 0xFFFF, 0x10000, 0x100C8, -2, 1000000, 0x80, 0xFF, 0x8000, -1000, 5};
inline constexpr i32 kOutDm[]{-4, -3, -2, -1, 20, 21, 22};
inline constexpr i32 kOutIdx[]{41, 42, 50, 60, 80, -1, -2, -5, -20, -41, -60, -100, 100, 200, 500, -500};
inline constexpr i32 kWrapK[]{-2, -1, 1, 2, 3};

struct F {
    u32 kind, eax, edx, stack, freeMask;
    i32 task;
    u32 chances[4];
    i32 flags[2];
    u8 obj[kObjBytes];
};

inline i32 PickTask(u32 c, u32 k, bool special) {
    if (special || Rand(c, k) % 4 == 0) return kTaskExtremes[Rand(c, k + 1) % std::size(kTaskExtremes)];
    return i32(Rand(c, k + 2) % 1500u);
}
inline i32 NotMinus1(u32 c, u32 k) { const i32 t = PickTask(c, k, false); return t == -1 ? 0 : t; }
inline u8 TrueByte(u32 c, u32 k, bool raw) {   // los bools son 0/1 en el juego; el EXE solo comprueba != 0 (bytes no canonicos: 2, 0x80, 0xFF)
    if (!raw) return 1;
    constexpr u8 v[4]{1, 2, 0x80, 0xFF};
    return v[Rand(c, k) % 4];
}
inline u8 NzProb(u32 c, u32 k, bool high) {
    constexpr u8 hi[6]{0x80, 0xFF, 0x81, 0xFE, 0xC0, 0x80};
    constexpr u8 any[6]{1, 2, 0x7F, 0x80, 0xFF, 0x40};
    if (high) return hi[Rand(c, k) % 6];
    return Rand(c, k) % 3 == 0 ? any[Rand(c, k + 1) % 6] : u8(1 + Rand(c, k + 2) % 255);
}
inline void PutTask(u8* o, int i, i32 t) { std::memcpy(o + i * 4, &t, 4); }
inline void SetFree(u8* o, int i) { PutTask(o, i, -1); std::memset(o + 0x18 + i * 4, 0, 4); std::memset(o + 0x30 + i * 2, 0, 2); }
// Ranura OCUPADA. Estilos: 0 solo tarea != -1 (cercana a libre); 1 solo UNA probabilidad != 0; 2 solo UN bool != 0; 3 tarea + basura; 4 tarea -1 y varias probabilidades;
// 5 tarea -1, probabilidades 0 y los dos bools; 6 todo lleno.
inline void SetOccupied(u8* o, int i, u32 c, u32 k, u32 style, bool high, bool raw) {
    i32 t = -1; u8 p[4]{}; u8 b[2]{};
    switch (style) {
    case 0: t = NotMinus1(c, k); break;
    case 1: p[Rand(c, k + 1) % 4] = NzProb(c, k + 2, high); break;
    case 2: b[Rand(c, k + 1) % 2] = TrueByte(c, k + 2, raw); break;
    case 3:
        t = NotMinus1(c, k);
        for (u32 j = 0; j < 4; ++j) p[j] = Rand(c, k + 3 + j) % 2 ? NzProb(c, k + 7 + j, false) : u8(0);
        for (u32 j = 0; j < 2; ++j) b[j] = Rand(c, k + 11 + j) % 2 ? TrueByte(c, k + 13 + j, raw) : u8(0);
        break;
    case 4:
        for (u32 j = 0; j < 4; ++j) p[j] = Rand(c, k + 3 + j) % 2 ? NzProb(c, k + 7 + j, high) : u8(0);
        p[Rand(c, k + 15) % 4] = NzProb(c, k + 16, high);
        for (u32 j = 0; j < 2; ++j) b[j] = Rand(c, k + 11 + j) % 2 ? TrueByte(c, k + 13 + j, raw) : u8(0);
        break;
    case 5: b[0] = TrueByte(c, k + 1, raw); b[1] = TrueByte(c, k + 2, raw); break;
    default:
        t = NotMinus1(c, k);
        for (auto& v : p) v = 255;
        b[0] = b[1] = 1;
        break;
    }
    PutTask(o, i, t);
    std::memcpy(o + 0x18 + i * 4, p, 4);
    std::memcpy(o + 0x30 + i * 2, b, 2);
}
inline u32 RandomMask(u32 c, u32 count) {   // subconjunto de `count` ranuras de 6
    int perm[6]{0, 1, 2, 3, 4, 5};
    for (int i = 5; i > 0; --i) { const int j = int(Rand(c, 600 + i) % u32(i + 1)); const int t = perm[i]; perm[i] = perm[j]; perm[j] = t; }
    u32 m = 0;
    for (u32 q = 0; q < count && q < 6; ++q) m |= 1u << perm[q];
    return m;
}

// kind (0..15): 0 ninguna libre; 1..6 exactamente k libres en posiciones aleatorias; 7 las seis libres; 8 una libre en la posicion `pos % 6` y las demas cercanas a libre (UN solo motivo);
//   9 barrido de chances especiales con ranura libre; 10 barrido de flags; 11 tareas extremas; 12 ranuras anteriores ocupadas por probabilidades 0x80/0xFF y libres desde `pos % 6`;
//   13 bytes de bool no canonicos; 14 solo la ranura 5 libre (las anteriores cercanas a libre); 15 todo aleatorio (0..6 libres).
inline u32 MakeImage(u32 c, u32 kind, u32 pos, u8* o) {
    u32 mask = 0;
    switch (kind) {
    case 0: mask = 0; break;
    case 1: case 2: case 3: case 4: case 5: case 6: mask = RandomMask(c, kind); break;
    case 7: mask = 0x3F; break;
    case 8: mask = 1u << (pos % 6); break;
    case 12: mask = (0x3Fu << (pos % 6)) & 0x3Fu; break;
    case 14: mask = 1u << 5; break;
    case 9: case 10: case 11: case 13: mask = RandomMask(c, 1 + Rand(c, 501) % 6); break;
    default: mask = Rand(c, 500) % 10 == 0 ? 0u : RandomMask(c, 1 + Rand(c, 501) % 6); break;
    }
    const bool high = kind == 12;
    const bool raw = kind == 13 || Rand(c, 610) % 16 == 0;
    for (int i = 0; i < 6; ++i) {
        if (mask >> i & 1) { SetFree(o, i); continue; }
        u32 style;
        if (kind == 8 || kind == 14) style = Rand(c, 620 + i) % 3;
        else if (kind == 12) style = (Rand(c, 620 + i) & 1) ? 1u : 4u;
        else if (kind == 13) style = (Rand(c, 620 + i) & 1) ? 2u : 5u;
        else style = Rand(c, 620 + i) % 7;
        SetOccupied(o, i, c, 640 + i * 24, style, high, raw);
    }
    return mask;
}

inline u32 Typical(u32 c, u32 k) {
    switch (Rand(c, k) % 6) {
    case 0: return Fb(float(Rand(c, k + 1) % 256));
    case 1: return Fb(float(Rand(c, k + 1) % 256) + float(Rand(c, k + 2) % 16) / 16.0f);
    case 2: return Fb(0.0f);
    case 3: return Fb(255.0f);
    case 4: return Fb(128.0f);
    default: return Fb(float(1 + Rand(c, k + 1) % 100));
    }
}
inline void MakeChances(u32 c, u32 kind, u32 pos, u32 out[4]) {
    const u32 N = u32(kSpecialChanceCount);
    if (kind == 9) {   // barrido: cada especial aparece varias veces en cada componente
        for (u32 j = 0; j < 4; ++j) out[j] = kSpecialChances[(pos + j * 29u) % N];
        return;
    }
    const u32 mode = Rand(c, 300) % 8;
    switch (mode) {
    case 0: for (u32 j = 0; j < 4; ++j) out[j] = Typical(c, 310 + j * 4); break;
    case 1: for (u32 j = 0; j < 4; ++j) out[j] = Fb(float(Rand(c, 320 + j * 2) % 4000) / 16.0f); break;
    case 2: for (u32 j = 0; j < 4; ++j) out[j] = kSpecialChances[Rand(c, 330 + j) % N]; break;
    case 3: for (u32 j = 0; j < 4; ++j) out[j] = Typical(c, 310 + j * 4); out[Rand(c, 340) % 4] = kSpecialChances[Rand(c, 341) % N]; break;
    case 4: { const u32 s = kSpecialChances[Rand(c, 342) % N]; for (u32 j = 0; j < 4; ++j) out[j] = s; } break;
    case 5: { constexpr float v[7]{0.0f, 1.0f, 127.0f, 128.0f, 129.0f, 254.0f, 255.0f}; for (u32 j = 0; j < 4; ++j) out[j] = Fb(v[Rand(c, 343 + j) % 7]); } break;
    case 6: for (u32 j = 0; j < 4; ++j) out[j] = Rand(c, 347 + j) % 3 == 0 ? Typical(c, 310 + j * 4) : kBoundaryChances[Rand(c, 351 + j) % std::size(kBoundaryChances)]; break;
    default: for (u32 j = 0; j < 4; ++j) out[j] = Fb(-(float(Rand(c, 355 + j) % 600) + float(Rand(c, 360 + j) % 16) / 16.0f)); break;
    }
}
inline void MakeFlags(u32 c, u32 kind, u32 pos, i32 out[2]) {
    constexpr u32 M = u32(std::size(kFlagValues));
    if (kind == 10) {
        out[0] = kFlagValues[pos % M];
        out[1] = kFlagValues[(pos * 7u + pos / M + 3u) % M];
        return;
    }
    for (u32 j = 0; j < 2; ++j) {
        const u32 r = Rand(c, 370 + j) % 8;
        out[j] = r < 3 ? 0 : r < 5 ? 1 : kFlagValues[Rand(c, 372 + j) % M];
    }
}
inline void Fill(F& f, u32 c, u32 kind, u32 pos) {
    f.kind = kind;
    f.eax = Rand(c, 23); f.edx = Rand(c, 24);
    f.freeMask = MakeImage(c, kind, pos, f.obj);
    f.task = kind == 11 ? kTaskExtremes[pos % std::size(kTaskExtremes)] : PickTask(c, 380, false);
    MakeChances(c, kind, pos, f.chances);
    MakeFlags(c, kind, pos, f.flags);
}

// ------------------------------------------------------------------------------------------------ Add: n -> caso
inline F MakeF(unsigned n) {
    F f{};
    Fill(f, n, n % 16, n / 16);
    f.stack = StackSeed(n);
    return f;
}

// ------------------------------------------------------------------------------------------------ AddEventResponse: n -> caso
// modo de direccion (n % 8): 0 normal (dm 0..19, indice 0..40, eventType 0..95); 1 dm fuera de 0..19 (-4..-1, 20..22); 2 indice leido fuera de 0..40;
//   3 dm + k*2^30; 4 indice leido + k*2^30; 5 eventType + k*2^30; 6 ventana W arbitraria (indice enorme resuelto para que W caiga donde se quiere); 7 posicion P de la tabla
//   en cualquier lugar de la arena (eventType negativo o grande, quizas + k*2^30).
// familia de ranuras (n / 8 % 16): las mismas `kind` de Add para el contenido de la CDecision destino.
struct G {
    F f;
    u32 mode;
    i32 dm, evType;
    u32 evIndex, thisOff, tableOff, windowOff, arenaSeed;
};
inline bool WindowOK(u32 w) { return w >= 0x40 && w <= kMem - kObjBytes - 0x40; }
inline bool TableOK(u32 p, u32 w) { return p >= 0x40 && p <= kMem - 0x44 && (p + 4 <= w || p >= w + kObjBytes); }
inline G MakeG(unsigned n) {
    G g{};
    const u32 c = n + 0x10000u;
    g.mode = n % 8;
    const u32 kind = (n / 8) % 16, pos = n / 128;
    Fill(g.f, c, kind, pos);
    g.f.stack = StackSeed(n);
    g.thisOff = kThis;
    g.arenaSeed = Rand(c, 700);
    bool found = false;
    i32 dm = 1; u32 idx = 2, ev = 3;
    for (u32 attempt = 0; attempt < 64 && !found; ++attempt) {
        const u32 a = attempt * 32;
        const u32 d0 = Rand(c, 710 + a) % 20, i0 = Rand(c, 711 + a) % 41, e0 = Rand(c, 712 + a) % 96;
        const u32 k = u32(kWrapK[Rand(c, 713 + a) % std::size(kWrapK)]);
        i32 d = i32(d0); u32 i = i0, e = e0;
        switch (g.mode) {
        case 1: d = kOutDm[Rand(c, 714 + a) % std::size(kOutDm)]; break;
        case 2: i = u32(kOutIdx[Rand(c, 715 + a) % std::size(kOutIdx)]); break;
        case 3: d = i32(d0 + (k << 30)); if (Rand(c, 716 + a) % 3 == 0) d = i32(u32(kOutDm[Rand(c, 717 + a) % std::size(kOutDm)]) + (k << 30)); break;
        case 4: i = i0 + (k << 30); if (Rand(c, 716 + a) % 3 == 0) i = u32(kOutIdx[Rand(c, 717 + a) % std::size(kOutIdx)]) + (k << 30); break;
        case 5: e = e0 + (k << 30); break;
        case 6: {
            const u32 w = 0x40 + 4u * (Rand(c, 720 + a) % ((kMem - kObjBytes - 0x80) / 4));
            const u32 which = Rand(c, 721 + a) % 3;
            d = which == 0 ? i32(d0) : which == 1 ? kOutDm[Rand(c, 722 + a) % std::size(kOutDm)] : i32(d0 + (k << 30));
            const u32 delta = w - kThis - 4u;                                     // multiplo de 4
            const u32 s = ((delta >> 2) * InverseOf15()) & 0x3FFFFFFFu;           // 15*s == delta/4 (mod 2^30)  =>  60*s == delta (mod 2^32)
            i = s - u32(d) * kMakerDecisions + (Rand(c, 723 + a) % 4u << 30);
            break; }
        case 7: {
            const u32 p = 0x40 + 4u * (Rand(c, 724 + a) % ((kMem - 0x100) / 4));
            e = u32((i32(p) - i32(kThis + kEventTable)) / 4);
            if (Rand(c, 725 + a) % 2) e += k << 30;
            break; }
        default: break;
        }
        const u32 w = WindowOf(u32(d), i), p = TableOf(e);
        if (WindowOK(w) && TableOK(p, w)) { dm = d; idx = i; ev = e; found = true; }
    }
    if (!found) { dm = 1; idx = 2; ev = 3; }   // respaldo (no deberia ocurrir con frecuencia)
    g.dm = dm; g.evType = i32(ev); g.evIndex = idx;
    g.windowOff = WindowOf(u32(dm), idx);
    g.tableOff = TableOf(ev);
    return g;
}
}

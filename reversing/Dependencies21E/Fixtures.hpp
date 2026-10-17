#pragma once
// Fixtures deterministas del banco Dependencies21E (sin dependencias del juego: se compilan igual en el banco y en un arnes aislado).
// fixtures.py repite exactamente estas formulas (LCG de 32 bits, tablas y reparto por indice); run_local.py exige que cada fixture registrado coincida con el suyo.
// Dominio: solo valores que el EXE puede producir o que estresan el aritmetico (NaN con carga distinta y senalizantes, +-inf, +-0, subnormales, +-FLT_MAX, +-1e30,
// enteros extremos); la eleccion de camara de EnterCar solo vale 0..3 (el EXE nunca guarda otra) y el coche siempre tiene matriz (con matriz nula el EXE se cae).
#include <cstdint>
#include <cstring>

namespace Dependencies21EFixtures {
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;

inline float AsFloat(u32 bits) { float f; std::memcpy(&f, &bits, 4); return f; }
inline u32 AsBits(float f) { u32 u; std::memcpy(&u, &f, 4); return u; }

// LCG de Numerical Recipes; la semilla se mezcla con el indice del caso. El orden de las llamadas lo fija cada funcion (siempre sentencias separadas).
struct Lcg {
    u32 state;
    explicit Lcg(u32 seed) : state(seed * 2654435761u + 12345u) { next(); next(); }
    u32 next() { state = state * 1664525u + 1013904223u; return state; }
    u32 below(u32 n) { return (next() >> 8) % n; }
};

inline constexpr unsigned kCases1 = 1536;   // 0x610DB0: 24 pares de limites x 16 valores del RNG x 4 estados x87 iniciales
inline constexpr unsigned kCases2 = 1536;   // 0x72D6D0: 256 barridos de un campo + 256 combinaciones de extremos + 1024 mezclas
inline constexpr unsigned kCases3 = 3072;   // 0x63A380: 1536 perfiles de flujo, dos veces (reales / adversarios)

// Estados iniciales de la x87 (FSW: TOP, C0..C3 y PE pegajoso); la FCW es la del juego en el momento del banco.
inline constexpr std::uint16_t kFpStatus[4] = {0x0000, 0x4700 | (3 << 11), 0x0100 | 0x0020 | (5 << 11), 0x0400 | (7 << 11)};

// ---------------- 0x610DB0 ----------------
struct Fx1 { i32 lower, upper; u32 random; u32 fp; };
inline constexpr i32 kLimits[24][2] = {
    {1, 5}, {1, 5}, {0, 1}, {1, 2}, {5, 1}, {-5, 5}, {0, 0}, {7, 7}, {0, 65536}, {0, 32768}, {-32768, 32767}, {INT32_MIN, INT32_MAX},
    {INT32_MAX, INT32_MIN}, {0, INT32_MAX}, {INT32_MIN, 0}, {1, 100000}, {0x7FFFFFF0, 0x7FFFFFFF}, {-1, -2}, {0x40000000, -0x40000000}, {123456789, -987654321},
    {2, 4}, {10, 20}, {1, 6}, {3, 3}};
inline constexpr u32 kRandoms[16] = {0u, 1u, 2u, 3u, 0x3FFFu, 0x4000u, 0x7FFEu, 0x7FFFu, 0x8000u, 0xFFFFu, 0x12345u, 0xFFFF0001u, 0x8001u, 0x5555u, 0x2AAAu, 0x7FFFu};
inline Fx1 MakeFx1(u32 i) { return {kLimits[i % 24][0], kLimits[i % 24][1], kRandoms[(i / 24) % 16], (i / 384) % 4}; }

// ---------------- 0x72D6D0 ----------------
struct Fx2 { i16 zone[8]; u32 fp; };
inline constexpr i16 kInt16Pool[32] = {0, 1, -1, 2, -2, 255, -255, 256, -256, 1000, -1000, 12345, -12345, 16383, -16383, 16384, -16384, 32766, -32766, 32767, -32767, -32768, 32765, -32765,
                                       -3, 3, 100, -100, 5000, -5000, 30000, -30000};
inline Fx2 MakeFx2(u32 i) {
    Fx2 f{};
    f.fp = i % 4;
    if (i < 256) {
        static constexpr i16 base[8] = {100, -200, 300, -400, 500, -600, -700, 800};
        for (int k = 0; k < 8; ++k) f.zone[k] = base[k];
        f.zone[i / 32] = kInt16Pool[i % 32];
    } else if (i < 512) {
        const u32 m = i - 256;
        for (int k = 0; k < 8; ++k) f.zone[k] = ((m >> k) & 1u) ? i16(32767) : i16(-32768);
    } else {
        Lcg r(0x22220000u + i);
        for (int k = 0; k < 8; ++k) {
            const u32 pick = r.below(4);
            if (pick != 0) {
                f.zone[k] = kInt16Pool[r.below(32)];
            } else {
                f.zone[k] = i16(std::uint16_t(r.next() >> 8));
            }
        }
    }
    return f;
}

// ---------------- 0x63A380 ----------------
inline constexpr u32 kStress[32] = {
    0x00000000u, 0x80000000u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00800000u, 0x80800000u, 0x3F800000u, 0xBF800000u, 0x7F7FFFFFu, 0xFF7FFFFFu,
    0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7FC01234u, 0xFFC54321u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFBFFFFFu, 0x7149F2CAu, 0xF149F2CAu,
    0x0DA24260u, 0x8DA24260u, 0x40490FDBu, 0xC0490FDBu, 0x3FC90FDBu, 0xBFC90FDBu, 0x40C90FDBu, 0xC0C90FDBu};
// {up.x, up.y, right.x, right.y} = {-sin a, cos a, cos a, sin a} para a = k * pi / 8
inline constexpr u32 kAxes[16][4] = {
    {0x00000000u, 0x3F800000u, 0x3F800000u, 0x00000000u}, {0xBEC3EF15u, 0x3F6C835Eu, 0x3F6C835Eu, 0x3EC3EF15u}, {0xBF3504F3u, 0x3F3504F3u, 0x3F3504F3u, 0x3F3504F3u},
    {0xBF6C835Eu, 0x3EC3EF15u, 0x3EC3EF15u, 0x3F6C835Eu}, {0xBF800000u, 0x248D3132u, 0x248D3132u, 0x3F800000u}, {0xBF6C835Eu, 0xBEC3EF15u, 0xBEC3EF15u, 0x3F6C835Eu},
    {0xBF3504F3u, 0xBF3504F3u, 0xBF3504F3u, 0x3F3504F3u}, {0xBEC3EF15u, 0xBF6C835Eu, 0xBF6C835Eu, 0x3EC3EF15u}, {0xA50D3132u, 0xBF800000u, 0xBF800000u, 0x250D3132u},
    {0x3EC3EF15u, 0xBF6C835Eu, 0xBF6C835Eu, 0xBEC3EF15u}, {0x3F3504F3u, 0xBF3504F3u, 0xBF3504F3u, 0xBF3504F3u}, {0x3F6C835Eu, 0xBEC3EF15u, 0xBEC3EF15u, 0xBF6C835Eu},
    {0x3F800000u, 0xA553C9CAu, 0xA553C9CAu, 0xBF800000u}, {0x3F6C835Eu, 0x3EC3EF15u, 0x3EC3EF15u, 0xBF6C835Eu}, {0x3F3504F3u, 0x3F3504F3u, 0x3F3504F3u, 0xBF3504F3u},
    {0x3EC3EF15u, 0x3F6C835Eu, 0x3F6C835Eu, 0xBEC3EF15u}};
// vertical cerca del limite del EXE (0x8D2ECC = 0xBE32B8C1)
inline constexpr u32 kVertical[8] = {0xBE32B8C1u, 0xBE32B8C0u, 0xBE32B8C2u, 0xBE2E147Bu, 0xBE3851ECu, 0x00000000u, 0xBF000000u, 0x3F000000u};
inline constexpr u32 kRealLimit = 0xBE32B8C1u;
inline constexpr i32 kSubKinds[10] = {802, 804, 820, 807, 808, 802, 804, 820, 807, 808};
inline constexpr i32 kCarSubtypes[4] = {0, 9, 5, 4};
inline constexpr i16 kSticks[8][4] = {{0, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 1, 1}, {21, 0, 0, 0}, {0, -32768, 1, 0}, {20, -20, 1, 0}, {32767, 5, 0, 1}};   // aimLR, aimUD, acelerar, frenar
inline constexpr u32 kPedTypes[3] = {0u, 1u, 7u};

// Valor real en [-scale, scale): (u * 2 - 1) * scale, con u = (siguiente >> 8) / 2^24; todos los pasos exactos salvo el producto final (un solo redondeo).
inline u32 RealBits(Lcg& r, float scale) {
    const u32 n = r.next();
    const float u = float(n >> 8) / 16777216.f;
    return AsBits((u * 2.f - 1.f) * scale);
}
// mode 0: 88 % reales, 12 % de la tabla de estres; mode 1 (adversario): 40 % tabla, 10 % NaN con carga, 10 % senalizante, 20 % subnormal, 20 % reales.
inline u32 PickBits(Lcg& r, float scale, u32 mode) {
    if (mode == 0) {
        const u32 pick = r.below(100);
        if (pick < 12) return kStress[r.below(32)];
        return RealBits(r, scale);
    }
    const u32 kind = r.below(10);
    if (kind < 4) return kStress[r.below(32)];
    if (kind == 4) {
        const u32 payload = r.next() & 0x3FFFFFu;
        const u32 sign = r.next() & 0x80000000u;
        return 0x7FC00000u | payload | sign;
    }
    if (kind == 5) {
        const u32 payload = (r.next() & 0x3FFFFFu) | 1u;
        const u32 sign = r.next() & 0x80000000u;
        return 0x7F800000u | payload | sign;
    }
    if (kind < 8) {
        const u32 mantissa = r.next() & 0x7FFFFFu;
        const u32 sign = r.next() & 0x80000000u;
        return mantissa | sign;
    }
    return RealBits(r, scale);
}

struct Fx3 {
    u32 gate, sticks, choice0, pedProfile, pass;
    bool carNull, subNull, pedMatrix;
    i32 carSubtype, subKind;
    u32 pedType;
    i16 aimLR, aimUD, accel, brake;
    u32 up[2], right[3], carPos[3], pedPos[3];
    u32 distance, minY, horizontal, vertical, alpha, beta, limit;
    u32 fp;
};
inline Fx3 MakeFx3(u32 i) {
    Fx3 f{};
    const u32 flow = i % 1536u;
    f.pass = i / 1536u;
    f.gate = flow % 16u;
    f.sticks = (flow / 16u) % 8u;
    f.choice0 = (flow / 128u) % 4u;
    f.pedProfile = (flow / 512u) % 3u;
    f.fp = i % 4u;
    f.carNull = f.gate == 0;
    f.carSubtype = f.gate == 1 ? 3 : kCarSubtypes[(flow / 3u) % 4u];
    f.subNull = f.gate == 2;
    f.subKind = f.gate == 3 ? 701 : f.gate == 4 ? 704 : f.gate == 5 ? 9999 : f.gate >= 6 ? kSubKinds[f.gate - 6] : 802;
    f.pedType = kPedTypes[f.pedProfile];
    f.pedMatrix = ((i >> 3) & 1u) != 0;
    f.aimLR = kSticks[f.sticks][0];
    f.aimUD = kSticks[f.sticks][1];
    f.accel = kSticks[f.sticks][2] ? i16(0x100) : i16(0);
    f.brake = kSticks[f.sticks][3] ? i16(-1) : i16(0);
    Lcg r(0x33330000u + i);
    const u32 mode = f.pass;
    const u32 axis = r.below(16);
    f.up[0] = kAxes[axis][0]; f.up[1] = kAxes[axis][1]; f.right[0] = kAxes[axis][2]; f.right[1] = kAxes[axis][3];
    f.right[2] = PickBits(r, 0.2f, mode);
    if (mode == 1) {
        for (int k = 0; k < 2; ++k) {
            const u32 swap = r.below(2);
            const u32 bits = PickBits(r, 1.f, 1);
            if (swap) f.up[k] = bits;
        }
        for (int k = 0; k < 2; ++k) {
            const u32 swap = r.below(2);
            const u32 bits = PickBits(r, 1.f, 1);
            if (swap) f.right[k] = bits;
        }
    }
    const float posScale = mode == 0 ? 50.f : 500.f;
    for (int k = 0; k < 3; ++k) f.carPos[k] = PickBits(r, posScale, mode);
    for (int k = 0; k < 3; ++k) f.pedPos[k] = PickBits(r, posScale, mode);
    {
        const u32 bits = PickBits(r, 8.f, mode);
        f.distance = mode == 0 ? (bits & 0x7FFFFFFFu) : bits;
    }
    {
        const u32 bits = PickBits(r, 4.f, mode);
        f.minY = mode == 0 ? (bits | 0x80000000u) : bits;
    }
    f.horizontal = PickBits(r, 3.2f, mode);
    {
        const u32 closeTo = r.below(100);
        const u32 index = r.below(8);
        const u32 bits = PickBits(r, 0.6f, mode);
        f.vertical = closeTo < 45 ? kVertical[index] : bits;
    }
    f.alpha = PickBits(r, 1.5f, mode);
    f.beta = PickBits(r, 1.5f, mode);
    {
        const u32 kind = r.below(4);
        const u32 bits = PickBits(r, 0.3f, 1);
        f.limit = (mode == 0 || kind < 2) ? kRealLimit : bits;
    }
    return f;
}
}  // namespace Dependencies21EFixtures

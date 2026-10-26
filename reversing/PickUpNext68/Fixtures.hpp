#pragma once
// Fixtures del banco PickUpNext68 (lote 68): CTaskComplexGoPickUpEntity::CreateNextSubTask (virtual, vtable 0x870B98 slot 8, EXE 0x691AE0), 16384 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskComplexGoPickUpEntity::CreateNextSubTask  0x691AE0  thiscall, ECX = this (tarea), (CPed*), `ret 4`, EAX = tarea nueva o nulo   (hook CTaskComplexGoPickUpEntity / CreateNextSubTask, tipo V)
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

namespace Pu68Fix {
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
constexpr std::size_t kTaskWin = 0x34, kSubWin = 0x10, kEntWin = 0x40, kPedWin = 0x488, kIntelWin = 0x30, kMatWin = 0x48, kColWin = 0x30, kSecWin = 0x20;
constexpr std::size_t kAllocAWin = 0x18, kAllocBWin = 0x4C, kAllocCWin = 0x3C;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kTaskArena = Arena(kTaskWin), kSubArena = Arena(kSubWin), kEntArena = Arena(kEntWin), kPedArena = Arena(kPedWin), kIntelArena = Arena(kIntelWin), kMatArena = Arena(kMatWin),
                      kColArena = Arena(kColWin), kSecArena = Arena(kSecWin), kAllocAArena = Arena(kAllocAWin), kAllocBArena = Arena(kAllocBWin), kAllocCArena = Arena(kAllocCWin);

// Tipos de tarea que decide el EXE: 0x134 TASK_SIMPLE_PICKUP_ENTITY, 0x386 TASK_SIMPLE_ACHIEVE_HEADING, 0x387 TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL
constexpr u32 kTypePickUp = 0x134, kTypeHeading = 0x386, kTypeGoTo = 0x387;
constexpr u32 kGroupPick = 0x51;
constexpr u32 kTimeAddr = 0xB7CB84;   // CTimer::m_snTimeInMilliseconds

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 68068u); }
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

// ------------------------------------------------------------------------------------------------ JSON minimo (mismas filas `input` en el juego y en cualquier espejo)
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
inline constexpr std::array<u32, 14> kTypesOther{0u, 1u, 0x133u, 0x135u, 0x136u, 0x385u, 0x388u, 0x1387u, 0x10134u, 0x80000000u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x252u, 0x486u};
inline constexpr std::array<u32, 14> kGroupsOther{0u, 0x50u, 0x52u, 0x53u, 0x151u, 0x80000051u, 0xFFFFFFFFu, 0x41u, 0x71u, 0x1u, 0x7FFFFFFFu, 0x100u, 0x5Fu, 0x49u};
inline constexpr std::array<u32, 3> kTypesMain{kTypePickUp, kTypeHeading, kTypeGoTo};
inline constexpr std::array<u32, 12> kTimes{0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 1000u, 123456u, 0x12345678u, 0xFFFFFF00u, 0xDEADBEEFu, 60000u, 0x01000000u};
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
    // El orden de los patrones de bits crece con la magnitud: para negativos el desplazamiento se invierte
    return (base & 0x80000000u) ? u32(i32(base) - off) : u32(i32(base) + off);
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 subType{}, entNull{}, group{}, time{};
    u32 secNull{}, secKind{}, secRaw{};   // GetTaskSecondary(4): nulo / tarea falsa / puntero crudo
    u32 allocNull{};
    u32 pedMat{}, entMat{};
    u32 ped[3]{}, ent[3]{}, colZ{}, vec[3]{}, pick[3]{};
};

// mode 0: top > ped.z (rama 0x53); 1: top <= ped.z < top + 0.55 (rama 0x52); 2: ped.z >= top + 0.55 (sin cambio)
inline void ThresholdZ(C& c, int mode) {
    const unsigned n = c.n;
    const float top = F(c.colZ) + F(c.ent[2]);
    const float lim = F(0x3F0CCCCDu) + top;
    const u32 pick = Rand(n, 140) % 4;
    if (mode == 0) c.ped[2] = pick == 0 ? Near(B(top), n, 141) : B(top - 1.0f - float(Rand(n, 142) % 1000) * 0.01f);
    else if (mode == 1) c.ped[2] = pick == 0 ? Near(B(top), n, 141) : pick == 1 ? Near(B(lim), n, 143) : B(top + 0.25f * float(Rand(n, 144) % 2) + 0.01f * float(Rand(n, 145) % 20));
    else c.ped[2] = pick == 0 ? Near(B(lim), n, 143) : B(lim + 0.01f + float(Rand(n, 146) % 1000) * 0.01f);
}

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n % kFamilies;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.time = Rand(n, 20) % 3 == 0 ? Pick(kTimes, n, 21) : Rand(n, 22);
    c.group = Rand(n, 23) % 2 ? Pick(kGroupsOther, n, 24) : Rand(n, 25);
    if (c.group == kGroupPick) c.group = 0x52;
    c.secNull = 1; c.secKind = 0; c.secRaw = 0;
    c.allocNull = 0;
    c.pedMat = Rand(n, 30) % 2; c.entMat = Rand(n, 31) % 2;
    const bool special = (n / 16) % 4 == 3;
    for (int i = 0; i < 3; ++i) {
        c.ped[i] = World(n, 40 + i, special);
        c.ent[i] = World(n, 50 + i, special);
        c.vec[i] = Rand(n, 60 + i) % 8 == 0 ? Pick(kTiny, n, 70 + i) : RandFloat(n, 80 + i, -8, 12);
        c.pick[i] = Rand(n, 90 + i) % 8 == 0 ? Pick(kTiny, n, 100 + i) : RandFloat(n, 110 + i, -8, 12);
    }
    c.colZ = World(n, 120, special);
    // Un NaN a la vez en cada resta (dx, dy): con dos NaN la x87 y SSE eligen operando distinto
    for (int i = 0; i < 2; ++i) if (F(c.ped[i]) != F(c.ped[i]) && F(c.ent[i]) != F(c.ent[i])) c.ent[i] = B(1.5f);
    if (F(c.colZ) != F(c.colZ) && F(c.ent[2]) != F(c.ent[2])) c.ent[2] = B(2.5f);
    switch (c.family) {
    case 0:   // sin entidad: el EXE devuelve nulo antes de mirar la subtarea
        c.entNull = 1; c.subType = Rand(n, 130) % 2 ? Pick(kTypesOther, n, 131) : Pick(kTypesMain, n, 132);
        break;
    case 1: c.subType = Pick(kTypesOther, n, 130); if (Rand(n, 133) % 4 == 0) c.subType = Rand(n, 134); break;
    case 2: c.subType = kTypePickUp; c.secNull = 0; c.secKind = Rand(n, 135) % 3 == 0 ? 1 : 0; c.secRaw = (Rand(n, 136) | 1u); break;
    case 3: c.subType = kTypePickUp; break;
    case 4: c.subType = kTypePickUp; c.allocNull = 1; break;
    case 5: c.subType = kTypeGoTo; break;
    case 6: {   // los valores especiales los decide (n / 16) % 4 == 3; ademas, diferencias desnormales (la x87 las conserva, SSE con FTZ las vacia)
        c.subType = kTypeGoTo;
        static constexpr u32 pairs[][2]{{0x00800000u, 0x007FFFFFu}, {0x007FFFFFu, 0x00800000u}, {0x00400000u, 0u}, {0u, 0x80000001u}, {3u, 1u}, {0x80800000u, 0x807FFFFFu}, {0x3F800000u, 0x3F800000u}, {0x00000001u, 0x80000000u}};
        const u32 r = Rand(n, 200) % 8;
        auto setPair = [&](int axis) { const u32* p = pairs[Rand(n, 201 + u32(axis)) % std::size(pairs)]; c.ent[axis] = p[0]; c.ped[axis] = p[1]; };
        if (r < 2) setPair(0);
        else if (r < 4) setPair(1);
        else if (r < 6) { setPair(0); setPair(1); }
        break; }
    case 7: c.subType = kTypeGoTo; c.allocNull = 1; break;
    case 8: c.subType = kTypeHeading; break;   // grupo distinto de 0x51
    case 9: c.subType = kTypeHeading; c.group = kGroupPick; ThresholdZ(c, 0); break;
    case 10: c.subType = kTypeHeading; c.group = kGroupPick; ThresholdZ(c, 1); break;
    case 11: c.subType = kTypeHeading; c.group = kGroupPick; ThresholdZ(c, 2); break;
    case 12: {
        c.subType = kTypeHeading; c.group = kGroupPick;
        // Valores especiales: sumas que desbordan el float (la x87 sigue finita), infinitos y NaN
        const u32 r = Rand(n, 150) % 8;
        if (r == 0) { c.colZ = B(3.0e38f); c.ent[2] = B(3.0e38f); }
        else if (r == 1) { c.colZ = B(-3.0e38f); c.ent[2] = B(-3.0e38f); }
        else if (r == 2) { c.colZ = Pick(kInf, n, 151); c.ent[2] = Pick(kBig, n, 152); }
        else if (r == 3) { c.colZ = Pick(kNan, n, 153); c.ent[2] = B(1.0f); }
        else if (r == 4) { c.ent[2] = Pick(kNan, n, 154); c.colZ = B(-2.0f); }
        else if (r == 5) { c.colZ = Pick(kBig, n, 155); c.ent[2] = Pick(kBig, n, 156); }
        else if (r == 6) { c.colZ = Pick(kTiny, n, 164); c.ent[2] = Pick(kTiny, n, 165); c.ped[2] = Pick(kTiny, n, 166); }
        else { c.colZ = 0x007FFFFFu; c.ent[2] = 0x00000001u + (Rand(n, 167) % 3); c.ped[2] = Pick(kTiny, n, 168); }   // suma desnormal frente a cero o desnormal
        const u32 q = r >= 6 ? 3 : Rand(n, 157) % 4;
        if (q == 0) c.ped[2] = Rand(n, 158) % 2 ? Pick(kInf, n, 159) : Pick(kBig, n, 160);
        else if (q == 1) ThresholdZ(c, int(Rand(n, 162) % 3));
        else if (q == 2) c.ped[2] = Pick(kNan, n, 163);
        break; }
    case 13:
        c.subType = kTypeHeading; c.group = Rand(n, 24) % 3 == 0 ? (c.group) : kGroupPick; ThresholdZ(c, int(Rand(n, 162) % 3));
        for (int i = 0; i < 3; ++i) if (Rand(n, 170 + i) % 3 == 0) c.pick[i] = Rand(n, 173 + i) % 2 ? Pick(kNan, n, 176 + i) : Rand(n, 180 + i) % 2 ? Pick(kInf, n, 184 + i) : Pick(kBig, n, 188 + i);
        break;
    default: {   // 14 y 15: mezcla
        const u32 r = Rand(n, 130) % 5;
        c.subType = r == 0 ? kTypePickUp : r == 1 ? kTypeHeading : r == 2 ? kTypeGoTo : Pick(kTypesOther, n, 131);
        if (c.subType == kTypePickUp) { c.secNull = Rand(n, 190) % 2; c.secKind = Rand(n, 191) % 2; c.secRaw = Rand(n, 192) | 1u; c.allocNull = Rand(n, 193) % 3 == 0; }
        if (c.subType == kTypeGoTo) c.allocNull = Rand(n, 194) % 6 == 0;
        if (c.subType == kTypeHeading) { if (Rand(n, 195) % 2) c.group = kGroupPick; ThresholdZ(c, int(Rand(n, 196) % 3)); }
        c.entNull = Rand(n, 197) % 16 == 0;
        break; }
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("sub_type", c.subType).U("ent_null", c.entNull).U("group", c.group).U("time", c.time);
    j.U("sec_null", c.secNull).U("sec_kind", c.secKind).U("sec_raw", c.secRaw).U("alloc_null", c.allocNull).U("ped_mat", c.pedMat).U("ent_mat", c.entMat);
    j.U("ped_x", c.ped[0]).U("ped_y", c.ped[1]).U("ped_z", c.ped[2]).U("ent_x", c.ent[0]).U("ent_y", c.ent[1]).U("ent_z", c.ent[2]).U("col_z", c.colZ);
    j.U("vec_x", c.vec[0]).U("vec_y", c.vec[1]).U("vec_z", c.vec[2]).U("pick_x", c.pick[0]).U("pick_y", c.pick[1]).U("pick_z", c.pick[2]);
    return j.End();
}
}

#pragma once
// Fixtures del banco PedWrappers60: tres funciones virtuales de CPed (hooks Virtual, dispatch 0 en hooks.csv):
//   CPed::ProcessControl                    0x5E8CD0  thiscall, sin argumentos, `ret`        (6144 casos)
//   CPed::SpecialEntityPreCollisionStuff    0x5E3C30  thiscall, 6 argumentos de pila, `ret 18h` (4096 casos)
//   CPed::SpecialEntityCalcCollisionSteps   0x5E3E90  thiscall, 2 argumentos de pila, `ret 8`    (4096 casos)
// Solo C++ estandar (sin tipos del juego): las incluyen LiveComparison.hpp/Core.hpp y un programa autonomo de calibracion (fixture_mirror.py).
//
// Cada objeto que las funciones tocan (el ped, sus datos de jugador, la entidad bajo los pies, el vehiculo, las matrices, ...) es memoria FALSA del banco con la disposicion
// del juego (mismos desplazamientos) y se rellena con basura determinista; sobre ella se escriben "pokes" (desplazamiento, tamano, valor) con los campos que la funcion lee o escribe.
// Los punteros entre objetos son simbolicos (Tok(etiqueta)): el banco los sustituye por la direccion real de la arena y el oraculo (fixture_oracle.py) los compara tal cual.
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <initializer_list>
#include <iterator>
#include <vector>

namespace PedWrappers60Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCasesPC = 6144;   // 16 familias x 384
constexpr unsigned kCasesPS = 4096;   // 16 familias x 256
constexpr unsigned kCasesCS = 4096;   // 16 familias x 256

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
constexpr u32 kPedBytes = 0x79C, kPedOff = 0x40, kPedArena = 0x81C;          // 0x40 + 0x79C + 0x40 (CPed completo)
constexpr u32 kPdataBytes = 0xAC, kPdataOff = 0x20, kPdataArena = 0xEC;      // 0x20 + 0xAC + 0x20 (CPlayerPedData; dos arenas: la del ped y la del otro jugador)
constexpr u32 kPpedBytes = 0x488, kPpedOff = 0x20, kPpedArena = 0x4C8;       // 0x20 + 0x488 + 0x20 (otro ped: solo +0x480)
constexpr u32 kEntBytes = 0x598, kEntOff = 0x20, kEntArena = 0x5D8;          // 0x20 + 0x598 + 0x20 (entidad bajo los pies / colPhysical: hasta +0x594)
constexpr u32 kVehBytes = 0x28, kVehOff = 0x20, kVehArena = 0x68;            // 0x20 + 0x28 + 0x20 (vehiculo del ped: +0x14 matriz, +0x22 modelo)
constexpr u32 kMatBytes = 0x48, kMatOff = 0x20, kMatArena = 0x88;            // 0x20 + 0x48 + 0x20 (tres matrices: ped, otro, camara)
constexpr u32 kMinfoBytes = 0x18, kMinfoOff = 0x20, kMinfoArena = 0x58;      // 0x20 + 0x18 + 0x20 (dos CBaseModelInfo falsos y un CObjectData falso)
constexpr u32 kTokBytes = 0x10, kTokOff = 0x10, kTokArena = 0x30;            // 0x10 + 0x10 + 0x10 (objetos opacos: fx, jerarquia, clump, inteligencia, objeto del arma, colmodels, textura)
constexpr u32 kMatArrBytes = 0x40, kMatArrOff = 0x20, kMatArrArena = 0x80;   // 0x20 + 0x40 + 0x20 (matriz base del array de huesos)
constexpr u32 kBoolsBytes = 8, kBoolsOff = 8, kBoolsArena = 0x18;            // 8 + 8 + 8 (cuatro referencias a bool: solo se escribe 1 byte cada una)
constexpr u32 kVtblBytes = 0x68, kVtblOff = 0x10, kVtblArena = 0x88;         // 0x10 + 0x68 + 0x10 (26 punteros de la vtable falsa del ped)
constexpr u32 kObjinfoUproot = 0x14;
constexpr u32 kSlotMoveAnim = 23;                                            // ranura de SetMoveAnim (vtable + 0x5C)

// globales que ProcessControl lee y el banco fija por caso (y restaura)
constexpr u32 kGFrame = 0xB7CB4C, kGTimestep = 0xB7CB5C, kGArea = 0xB72914, kGForce = 0xB7CD6E, kGCamMat = 0xB6F03C, kGCamPos = 0xB6F02C, kGBloodTex = 0xC40400, kGModelInfo = 0xA9B0C8;
constexpr u32 kColPoints = 0xC092A8, kGroups = 0xC09928, kGroupSize = 0x2D4, kFxMgr = 0xA9AE80;

// ------------------------------------------------------------------------------------------------ etiquetas y punteros simbolicos
enum : u32 {
    L_NULL = 0, L_PED, L_PDATA, L_PDATA2, L_PPED, L_ENT, L_VEH, L_MATPED, L_MATOTHER, L_MATCAM, L_MI0, L_MI1, L_OBJINFO, L_FX, L_HIER, L_CLUMP, L_INTEL, L_WOBJ, L_CM0, L_CM1, L_TEX, L_MATARR, L_VTBL, L_BOOLS,
    L_COUNT
};
constexpr u32 kTokBase = 0xFD000000u;
constexpr u32 Tok(u32 label) { return kTokBase | label; }
constexpr bool IsTok(u32 v) { return (v & 0xFF000000u) == kTokBase && (v & 0xFFFFFFu) < L_COUNT; }
constexpr u32 kAnyPtr = 0x00123450u;      // puntero no nulo que no es ninguno de los objetos (solo se compara)

struct Poke { u32 off, size, value; bool ptr; };
struct Obj {
    std::vector<Poke> p;
    void W(u32 off, u32 size, u32 v, bool ptr = false) {
        for (auto& q : p) if (q.off == off && q.size == size) { q.value = v; q.ptr = ptr; return; }
        p.push_back({off, size, v, ptr});
    }
    void W32(u32 off, u32 v) { W(off, 4, v); }
    void W16(u32 off, u32 v) { W(off, 2, v & 0xFFFFu); }
    void W8(u32 off, u32 v) { W(off, 1, v & 0xFFu); }
    void P(u32 off, u32 v) { W(off, 4, v, true); }
    u32 Get(u32 off, u32 size) const { for (const auto& q : p) if (q.off == off && q.size == size) return q.value; return 0; }
};

// ------------------------------------------------------------------------------------------------ generador
inline constexpr u32 kFloatEdges[]{0x00000000u, 0x80000000u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F800000u, 0xBF800000u, 0x3A83126Fu, 0xBA83126Fu, 0x7F7FFFFFu, 0xFF7FFFFFu,
                                   0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x3C23D70Au, 0x3C23D70Bu, 0x3C23D709u, 0x3DB851ECu, 0x3DB851EDu, 0x3DB851EBu, 0x43AF0000u, 0x43AEFFFFu, 0x43AF0001u,
                                   0x3E800000u, 0x3E800001u, 0x3E7FFFFFu, 0x447A0000u, 0x461C4000u};
struct Gen {
    u32 n, k = 100;
    u32 R() { return Rand(n, k++); }
    u32 Pick(u32 m) { return R() % m; }
    bool Chance(u32 num, u32 den) { return Pick(den) < num; }
    u32 EdgeF() { return kFloatEdges[Pick(u32(std::size(kFloatEdges)))]; }
    // float normal con exponente sesgado en [lo, hi]
    u32 Norm(u32 lo, u32 hi, bool neg = false) { return (neg ? 0x80000000u : 0u) | ((lo + Pick(hi - lo + 1)) << 23) | (R() & 0x7FFFFFu); }
    u32 OneOf(std::initializer_list<u32> l) { auto it = l.begin(); std::advance(it, Pick(u32(l.size()))); return *it; }
};
inline u32 FBits(float f) { return std::bit_cast<u32>(f); }
inline float BitsF(u32 b) { return std::bit_cast<float>(b); }

// ================================================================================================ CPed::SpecialEntityCalcCollisionSteps (0x5E3E90)
// Lee: +0xFC (adjunto), +0x480 (datos de jugador), +0x44..+0x4C (velocidad), +0x568 (entidad bajo los pies), +0x9C (elasticidad: la duplica sin datos de jugador) y el paso 0xB7CB5C.
// Familias (n % 16, 256 casos cada una):
//  0 adjunto (devuelve 1)   1 sin datos de jugador, velocidad nula   2 sin datos, umbral 0.09 (v*paso*paso)   3 sin datos, pasos 1..40 (ceil(d*5))
//  4 sin datos, bordes enteros de ceil   5 jugador de pie sobre una entidad (ceil(d*6.6666665), minimo 4)   6 igual en el borde de 4   7 jugador sin entidad (ceil(d*3.3333333), minimo 2)
//  8 igual en el borde de 2   9 pasos de tiempo de borde (0, -0, negativo, NaN, inf, FLT_MAX, denormal)   10 velocidades de borde (NaN, inf, FLT_MAX, denormales)
// 11 resultados enormes (el _ftol2 desborda)   12 elasticidad de borde   13..15 mezcla aleatoria
struct KCase {
    u32 family = 0;
    Obj ped;
    u32 timestep = 0;
    std::array<u32, 2> bools{};           // valores iniciales de las dos referencias a bool (no se leen ni se escriben)
    u32 eax = 0, edx = 0, stack = 0;
};

inline KCase MakeK(u32 n) {
    Gen g{n};
    KCase c;
    const u32 f = n % 16;
    c.family = f;
    c.eax = Rand(n, 20);
    c.edx = Rand(n, 21);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    c.bools = {Rand(n, 22) & 0xFFu, Rand(n, 23) & 0xFFu};
    u32 attached = 0, pdata = 0, standing = 0, elast = g.Norm(0x78, 0x84, g.Chance(1, 4));
    u32 speed[3]{};
    u32 ts = g.Norm(0x76, 0x7F);
    const bool hasPdata = g.Chance(1, 2);
    if (hasPdata) pdata = Tok(L_PDATA);
    if (g.Chance(1, 2)) standing = Tok(L_ENT);
    for (int i = 0; i < 3; ++i) speed[i] = g.Norm(0x70, 0x7F, g.Chance(1, 2));
    auto Vec = [&](float x, float y, float z) { speed[0] = FBits(x); speed[1] = FBits(y); speed[2] = FBits(z); };
    // x tal que |x| * ts * mult ~ objetivo; velocidad en un solo eje (los otros dos a cero)
    auto OnAxis = [&](double target, double mult, u32 delta) {
        const float t = BitsF(ts);
        double x = t != 0.0f && std::isfinite(t) ? target / (double(t) * mult) : 1.0;
        u32 b = FBits(float(x));
        b += delta - 2;      // delta 0..4 => -2..+2 ulp
        speed[0] = b; speed[1] = 0; speed[2] = 0;
        if (g.Chance(1, 3)) { const u32 s = speed[0]; speed[0] = 0; speed[g.Pick(2) + 1] = s; }
        if (g.Chance(1, 4)) speed[0] |= 0x80000000u;
    };
    switch (f) {
    case 0: attached = g.Chance(1, 2) ? Tok(L_ENT) : kAnyPtr; break;
    case 1: Vec(0.f, 0.f, 0.f); if (g.Chance(1, 2)) speed[g.Pick(3)] = 0x80000000u; pdata = 0; break;
    case 2: pdata = 0; ts = g.Norm(0x7A, 0x80); OnAxis(0.3 /* sqrt(0.09) */, 1.0, g.Pick(5)); break;
    case 3: pdata = 0; ts = g.Norm(0x76, 0x80); OnAxis(double(1 + g.Pick(40)) - 0.5 + (g.Chance(1, 2) ? 1.0 : 0.0), 5.0, g.Pick(5)); break;
    case 4: pdata = 0; ts = g.Chance(1, 2) ? FBits(1.0f) : g.Norm(0x7C, 0x80); OnAxis(double(1 + g.Pick(40)), 5.0, g.Pick(5)); break;
    case 5: pdata = Tok(L_PDATA); standing = Tok(L_ENT); ts = g.Norm(0x7A, 0x80); OnAxis(double(1 + g.Pick(40)) * (g.Chance(1, 2) ? 1.0 : 0.9), 6.6666665, g.Pick(5)); break;
    case 6: pdata = Tok(L_PDATA); standing = Tok(L_ENT); ts = g.Norm(0x7A, 0x80); OnAxis(g.Chance(1, 2) ? 4.0 : 3.0, 6.6666665, g.Pick(5)); break;
    case 7: pdata = Tok(L_PDATA); standing = 0; ts = g.Norm(0x7A, 0x80); OnAxis(double(1 + g.Pick(40)) * (g.Chance(1, 2) ? 1.0 : 0.9), 3.3333333, g.Pick(5)); break;
    case 8: pdata = Tok(L_PDATA); standing = 0; ts = g.Norm(0x7A, 0x80); OnAxis(g.Chance(1, 2) ? 2.0 : 1.0, 3.3333333, g.Pick(5)); break;
    case 9: ts = g.EdgeF(); if (g.Chance(1, 2)) Vec(BitsF(g.Norm(0x78, 0x7F)), 0.f, 0.f); break;
    case 10: speed[0] = g.EdgeF(); speed[1] = g.Chance(1, 2) ? g.EdgeF() : 0u; speed[2] = g.Chance(1, 2) ? g.EdgeF() : 0u; if (g.Chance(1, 2)) ts = FBits(1.0f); break;
    case 11: ts = g.Norm(0xF0, 0xFE); speed[0] = g.Norm(0xA0, 0xFE); if (g.Chance(1, 2)) speed[1] = g.Norm(0xA0, 0xFE); if (g.Chance(1, 4)) pdata = 0; break;
    case 12: elast = g.EdgeF(); pdata = g.Chance(2, 3) ? 0u : Tok(L_PDATA); ts = g.Norm(0x7A, 0x80); OnAxis(double(1 + g.Pick(20)), 5.0, g.Pick(5)); break;
    default: break;
    }
    if (f != 0 && f != 1 && f != 9 && f != 10 && f != 11 && f != 12 && g.Chance(1, 16)) attached = kAnyPtr;
    if (f >= 13 && f <= 15) {   // mezcla: cualquier combinacion
        pdata = g.Chance(1, 2) ? Tok(L_PDATA) : 0u;
        standing = g.Chance(1, 2) ? Tok(L_ENT) : 0u;
        ts = g.Chance(1, 8) ? g.EdgeF() : g.Norm(0x74, 0x82, g.Chance(1, 16));
        if (g.Chance(1, 8)) for (int i = 0; i < 3; ++i) speed[i] = g.EdgeF();
        else for (int i = 0; i < 3; ++i) speed[i] = g.Chance(1, 3) ? 0u : g.Norm(0x70, 0x82, g.Chance(1, 2));
        if (g.Chance(1, 6)) attached = kAnyPtr;
    }
    c.timestep = ts;
    c.ped.P(0xFC, attached);
    c.ped.P(0x480, pdata);
    c.ped.P(0x568, standing);
    for (int i = 0; i < 3; ++i) c.ped.W32(0x44 + 4 * i, speed[i]);
    c.ped.W32(0x9C, elast);
    return c;
}

// ================================================================================================ CPed::SpecialEntityPreCollisionStuff (0x5E3C30)
// ped (this): +0x1C bandera de atascado (bit 4), +0x40 banderas fisicas, +0x470 (bit 4 y bit 31), +0x47C inteligencia, +0x58C vehiculo, +0x598 tipo, +0xFC adjunto, +0x128 ignorado, +0x14 matriz (z en +0x38);
// colPhysical (arena de entidad): +0x14 matriz (arriba z en +0x28, posicion z en +0x38), +0x1C, +0x22 modelo (u16), +0x36 tipo (3 bits), +0x40, +0x44..+0x4C velocidad, +0xFC, +0x128,
// +0x140 banderas de objeto (bit 8 = farola), +0x160 CObjectData (+0x14 limite de arranque). Falso: CPedIntelligence::GetTaskClimb (0x601180).
// Familias (n % 16, 256 casos cada una):
//  0 vehiculo con el peaton derribado de la moto   1 entidad ignorada (en un sentido u otro) +/- bKnockedUpIntoAir y bKnockedOffBike   2 adjuntos (a la otra, de la otra, ambos)
//  3 otra con bDisableMoveForce (colision/31, ignorar atasco, atascos)   4 otra con masa infinita o DisableZ   5 objeto: farola caida   6 objeto: modelo 0x156
//  7 objeto: limite de arranque y velocidad   8 objeto: atascado   9 no objeto: modelos 0x1B9/0x234/0x252   10 no objeto: atascado   11 cola: jugadores y nadar
// 12 bordes de coma flotante (z, up.z, velocidades)   13..15 mezcla aleatoria
struct SCase {
    u32 family = 0;
    Obj ped, ent, objinfo, matPed, matEnt;
    u32 ignoreArg = 0;                    // lo que se empuja como bIgnoreStuckCheck (dword completo: el EXE solo mira el byte bajo)
    std::array<u32, 4> bools{};           // valores iniciales de las cuatro referencias a bool
    u32 rClimb = 0;                       // lo que devuelve el falso de GetTaskClimb
    u32 eax = 0, edx = 0, stack = 0;
};

inline SCase MakeS(u32 n) {
    Gen g{n};
    SCase c;
    const u32 f = n % 16;
    c.family = f;
    c.eax = Rand(n, 20);
    c.edx = Rand(n, 21);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    for (int i = 0; i < 4; ++i) c.bools[i] = Rand(n, 22 + i) & 0xFFu;
    c.rClimb = g.Chance(1, 2) ? 0u : Tok(L_INTEL);
    auto PtrChoice = [&](u32 selfTok, u32 otherTok) -> u32 {
        switch (g.Pick(5)) { case 0: return selfTok; case 1: return otherTok; case 2: return kAnyPtr; default: return 0u; }
    };
    // ---- ped
    u32 entFlags = g.R() & ~0x10u;                                     // bIsStuck desactivado por defecto
    if (g.Chance(1, 4)) entFlags |= 0x10u;
    u32 pedPhys = g.R() & 0x00FFFFFFu & ~0x1000u;                      // sin la bandera 0x1000 por defecto
    if (g.Chance(1, 4)) pedPhys |= 0x1000u;
    u32 flags470 = g.R() & ~0x80000010u;
    if (g.Chance(1, 4)) flags470 |= 0x10u;
    if (g.Chance(1, 4)) flags470 |= 0x80000000u;
    u32 pedType = g.Chance(1, 2) ? g.Pick(2) : 2 + g.Pick(20);
    u32 pedAttached = g.Chance(3, 4) ? 0u : PtrChoice(Tok(L_ENT), kAnyPtr);
    u32 pedIgnored = g.Chance(3, 4) ? 0u : PtrChoice(Tok(L_ENT), kAnyPtr);
    u32 pedVeh = g.Chance(1, 2) ? 0u : (g.Chance(1, 2) ? Tok(L_ENT) : kAnyPtr);
    // ---- colPhysical
    u32 entType = g.OneOf({1u, 2u, 3u, 4u, 5u, 0u, 7u});
    u32 entPhys = (g.R() & 0x0FFFFFFFu) & ~(0x20u | 0x4u | 0x40u | 0x80u | 0x40000000u);   // sin los bits que deciden ramas
    u32 entModel = g.Chance(1, 4) ? g.OneOf({0x156u, 0x1B9u, 0x234u, 0x252u, 0x155u, 0x157u, 0u, 0xFFFFu}) : (g.R() & 0xFFFFu);
    u32 entFlags1C = g.R() & ~0x10u;
    if (g.Chance(1, 4)) entFlags1C |= 0x10u;
    u32 entAttached = g.Chance(3, 4) ? 0u : PtrChoice(Tok(L_PED), kAnyPtr);
    u32 entIgnored = g.Chance(3, 4) ? 0u : PtrChoice(Tok(L_PED), kAnyPtr);
    u32 objFlags = g.R() & ~0x100u;
    if (g.Chance(1, 4)) objFlags |= 0x100u;
    u32 uproot = g.Chance(1, 2) ? g.Norm(0x78, 0x82) : g.EdgeF();
    u32 spd[3];
    for (int i = 0; i < 3; ++i) spd[i] = g.Chance(1, 2) ? g.Norm(0x70, 0x7B, g.Chance(1, 2)) : g.Norm(0x7B, 0x7E, g.Chance(1, 2));
    u32 upZ = g.Chance(1, 2) ? g.Norm(0x7C, 0x7F, g.Chance(1, 4)) : g.OneOf({0x3F28F5C3u, 0x3F28F5C2u, 0x3F28F5C4u, 0u, 0x80000000u, 0x7FC00000u});    // 0.66f +/- ulp
    u32 entZ = g.Norm(0x78, 0x85, g.Chance(1, 2)), pedZ = g.Norm(0x78, 0x85, g.Chance(1, 2));
    c.ignoreArg = g.Chance(1, 2) ? 0u : (g.Chance(1, 2) ? 1u : g.OneOf({0x100u, 0xFFFFFF00u, 0x01010101u, 2u, 0x80u}));

    switch (f) {
    case 0: entType = 2; pedVeh = Tok(L_ENT); flags470 |= 0x80000000u; if (g.Chance(1, 4)) pedVeh = 0u; if (g.Chance(1, 4)) flags470 &= 0x7FFFFFFFu; break;
    case 1: entType = g.Pick(6); if (g.Chance(1, 2)) pedIgnored = Tok(L_ENT); else entIgnored = Tok(L_PED);
            if (g.Chance(1, 2)) flags470 |= 0x10u; else flags470 &= ~0x10u; if (g.Chance(1, 2)) flags470 |= 0x80000000u; else flags470 &= 0x7FFFFFFFu; break;
    case 2: switch (g.Pick(4)) { case 0: pedAttached = Tok(L_ENT); break; case 1: entAttached = Tok(L_PED); break; case 2: pedAttached = kAnyPtr; entAttached = kAnyPtr; break; default: pedAttached = Tok(L_ENT); entAttached = Tok(L_PED); break; }
            pedIgnored = 0; entIgnored = 0; break;
    case 3: entPhys |= 0x20u; if (g.Chance(1, 3)) entPhys |= 0x4u; if (g.Chance(1, 4)) entPhys |= 0x40000000u; pedAttached = 0; entAttached = 0; pedIgnored = 0; entIgnored = 0;
            if (g.Chance(1, 2)) entFlags |= 0x10u; else entFlags &= ~0x10u; break;
    case 4: entPhys |= g.OneOf({0x40u, 0x80u, 0xC0u}); pedAttached = 0; entAttached = 0; pedIgnored = 0; entIgnored = 0; break;
    case 5: entType = 4; objFlags |= 0x100u; pedAttached = 0; entAttached = 0; pedIgnored = 0; entIgnored = 0; if (g.Chance(1, 2)) upZ = g.Norm(0x70, 0x7E); break;
    case 6: entType = 4; objFlags &= ~0x100u; entModel = g.Chance(2, 3) ? 0x156u : g.OneOf({0x155u, 0x157u}); pedAttached = 0; entAttached = 0; pedIgnored = 0; entIgnored = 0;
            if (g.Chance(1, 4)) pedZ = entZ; break;
    case 7: entType = 4; objFlags &= ~0x100u; entModel = g.Chance(2, 3) ? 0x100u : 0x156u; pedAttached = 0; entAttached = 0; pedIgnored = 0; entIgnored = 0;
            if (entModel == 0x156u) { pedZ = g.Norm(0x78, 0x85); entZ = pedZ; if (g.Chance(1, 2)) entZ = FBits(BitsF(pedZ) + 1.0f); }
            if (g.Chance(1, 2)) entPhys |= 0x4u; for (int i = 0; i < 3; ++i) spd[i] = g.Chance(1, 2) ? g.Norm(0x70, 0x75, g.Chance(1, 2)) : g.OneOf({0x3A83126Fu, 0xBA83126Fu, 0x3A83126Eu, 0x3A831270u, 0u, 0x7FC00000u}); break;
    case 8: entType = 4; objFlags &= ~0x100u; entModel = 0x100u; pedAttached = 0; entAttached = 0; pedIgnored = 0; entIgnored = 0; uproot = g.Chance(1, 2) ? 0u : 0x80000000u;
            if (g.Chance(1, 2)) entPhys &= ~0x4u; for (int i = 0; i < 3; ++i) spd[i] = g.Norm(0x7B, 0x7E);
            if (g.Chance(1, 2)) entFlags1C |= 0x10u; else entFlags1C &= ~0x10u; break;
    case 9: entType = g.OneOf({1u, 2u, 3u, 5u, 6u}); entModel = g.OneOf({0x1B9u, 0x234u, 0x252u, 0x1B8u, 0x1BAu, 0x233u, 0x235u, 0x251u, 0x253u}); pedAttached = 0; entAttached = 0; pedIgnored = 0; entIgnored = 0; break;
    case 10: entType = g.OneOf({1u, 2u, 3u, 5u, 6u}); entModel = 0x400u + g.Pick(8); pedAttached = 0; entAttached = 0; pedIgnored = 0; entIgnored = 0;
            if (g.Chance(1, 2)) entFlags1C |= 0x10u; else entFlags1C &= ~0x10u; break;
    case 11: pedType = g.Chance(1, 2) ? g.Pick(2) : 2 + g.Pick(8); pedAttached = 0; entAttached = 0; pedIgnored = 0; entIgnored = 0; entType = g.OneOf({1u, 3u, 5u});
            if (g.Chance(1, 2)) entPhys |= 0x20u | 0x4u; break;
    case 12: for (int i = 0; i < 3; ++i) if (g.Chance(1, 2)) spd[i] = g.EdgeF();
            upZ = g.EdgeF(); entZ = g.EdgeF(); pedZ = g.Chance(1, 2) ? entZ : g.EdgeF(); uproot = g.EdgeF(); entType = 4; entModel = g.Chance(1, 2) ? 0x156u : 0x100u; objFlags |= g.Chance(1, 2) ? 0x100u : 0u;
            pedAttached = 0; entAttached = 0; pedIgnored = 0; entIgnored = 0; if (g.Chance(1, 2)) entPhys |= 0x4u; break;
    default: break;
    }
    if (f >= 13) {   // mezcla: tipos y banderas decisivas aleatorias
        if (g.Chance(1, 2)) entPhys |= g.OneOf({0x20u, 0x4u, 0x40u, 0x80u, 0x40000000u, 0x24u, 0x60u, 0xC0u});
        entType = g.OneOf({1u, 2u, 3u, 4u, 4u, 4u, 5u});
        if (g.Chance(1, 3)) entModel = g.OneOf({0x156u, 0x1B9u, 0x234u, 0x252u});
    }
    c.ped.P(0x14, Tok(L_MATPED));
    c.ped.W32(0x1C, entFlags);
    c.ped.W32(0x40, pedPhys);
    c.ped.P(0xFC, pedAttached);
    c.ped.P(0x128, pedIgnored);
    c.ped.W32(0x470, flags470);
    c.ped.P(0x47C, Tok(L_INTEL));
    c.ped.P(0x58C, pedVeh);
    c.ped.W32(0x598, pedType);
    c.ent.P(0x14, Tok(L_MATOTHER));
    c.ent.W32(0x1C, entFlags1C);
    c.ent.W16(0x22, entModel);
    c.ent.W8(0x36, entType | (g.R() & 0xF8u));            // el EXE solo mira los 3 bits bajos
    c.ent.W32(0x40, entPhys);
    for (int i = 0; i < 3; ++i) c.ent.W32(0x44 + 4 * i, spd[i]);
    c.ent.P(0xFC, entAttached);
    c.ent.P(0x128, entIgnored);
    c.ent.W32(0x140, objFlags);
    c.ent.P(0x160, Tok(L_OBJINFO));
    c.objinfo.W32(kObjinfoUproot, uproot);
    c.matPed.W32(0x38, pedZ);
    c.matEnt.W32(0x28, upZ);
    c.matEnt.W32(0x38, entZ);
    return c;
}

// ================================================================================================ CPed::ProcessControl (0x5E8CD0)
// El desensamblado (0x5E8CD0..0x5E9530) esta descrito bloque a bloque en fixture_oracle.py. Los colaboradores (ver Core.hpp) son falsos identicos en ambos lados; la rand del EXE (0x821B1E)
// es la real y se siembra con srand del EXE (0x821B11). Los falsos que reciben el ped pueden "mutar" un dword del ped (xor) para exigir las lecturas frescas del EXE.
//
// Familias (n % 16, 384 casos cada una):
//  0 llama de molotov: crea la FX (jugador, visible, con objeto del arma)   1 llama de molotov: destruye (no visible/no se renderiza/nadando/sin objeto) o mantiene   2 sin molotov: tipo no jugador, otra arma
//  3 salidas tempranas: replay, ola de ataque (area, z > 950)   4 PruneReferences y transparencia (fotograma + semilla, fade, alfa en los bordes)
//  5 derribado de la moto (bKnockedOffBike): condiciones, velocidad < 0.01, vehiculo/ProcessColModels   6 cabeza atascada (z + 1.5 < field_588)   7 limpieza de banderas y fogonazos (paso de tiempo de borde)
//  8 peaton de mision: FindPlayerPed/IsMember   9 humedad del jugador   10 de pie sobre una entidad segura/tren (pospone y sale)   11 velocidad vertical y omitir la fisica
// 12 gotas de sangre (Blood, fotograma, camara, rand)   13 en vehiculo, UpdatePedCount, Say y motosierra   14 mezcla aleatoria   15 mezcla aleatoria con mutaciones
enum PStub : u32 { S_PRUNE = 0, S_BUOY, S_PFIRST, S_SKIP, S_PHYSPC, S_DELW, S_FOOT, S_PPROC, S_NEWVEL, S_UPDPOS, S_MOVEANIM, S_COUNT };
struct Mut { u32 stub, off, mask; };
struct PCase {
    u32 family = 0;
    Obj ped, pdata, pdata2, pped, ent, veh, matPed, matVeh, matCam, clump;
    u32 frame = 0, timestep = 0, area = 0, force = 0, camMat = 0;   // 0xB7CB4C, 0xB7CB5C, 0xB72914, 0xB7CD6E, [0xB6F03C]
    std::array<u32, 3> camPos{};
    u32 miPedIdx = 19000, miVehIdx = 19001, miPedCol = 0, miVehCol = 1;   // indices en la tabla de modelos (0xA9B0C8) y colmodel (0 = CM0, 1 = CM1) de cada uno
    u32 rSwim = 0, rAlpha = 0, rCol = 0, rFindPlayer = 0, rIsMember = 0, rBlood = 0, rFx = 0, rBone = 0, rJunk = 0;
    std::array<u32, 3> plCoors{};
    std::vector<Mut> muts;
    u32 seed = 1;
    u32 bloodTex = 0x00C0FFEEu;
    u32 eax = 0, edx = 0, stack = 0;
};
// dwords del ped completamente especificados (se pueden mutar con xor sin tocar bytes sin especificar)
inline constexpr u32 kMutOffsets[]{0x40, 0x44, 0x48, 0x4C, 0x46C, 0x470, 0x474, 0x478, 0x4D8, 0x4DC, 0x504, 0x508, 0x530, 0x534, 0x588, 0x598, 0x72C};

inline PCase MakeP(u32 n) {
    Gen g{n};
    PCase c;
    const u32 f = n % 16;
    c.family = f;
    c.eax = Rand(n, 20);
    c.edx = Rand(n, 21);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    c.seed = Rand(n, 24) | 1u;
    c.rJunk = Rand(n, 25) & 0xFFFFFF00u;
    // ---- estado base valido: todos los campos que la funcion puede leer o escribir en cualquier camino
    u32 pedType = g.Chance(1, 2) ? g.Pick(2) : 2 + g.Pick(22);
    u32 slot = g.Pick(13);
    u32 wtype = g.Chance(1, 4) ? g.OneOf({0x12u, 9u}) : g.Pick(47);
    u32 wfx = 0;
    u32 f46C = g.R(), f470 = g.R(), f474 = g.R(), f478 = g.R();
    // por defecto: sin las banderas que cortan la funcion o fuerzan caminos raros; cada familia las enciende
    f478 &= ~(0x200000u | 0x100u | 0x400u | 0x4000u | 0x100000u);   // bUsedForReplay, bit 8, bDruggedUp, bits 14 y 20
    f474 &= ~(0x1000u | 0x2u);                                      // bPartOfAttackWave, bDontRender
    f470 &= ~(0x80000000u | 0x200000u | 0x80000u | 0x8u);          // bKnockedOffBike, bHeadStuckInCollision, bIsDrowning, bFadeOut
    f46C &= ~(0x40000u | 0x100u);                                  // bPedIsBleeding, bInVehicle
    if (g.Chance(1, 2)) f46C |= 1u; else f46C &= ~1u;              // bIsStanding
    u32 phys = g.R() & ~0x100u;                                     // sin PHYSICAL_SUBMERGED_IN_WATER por defecto
    u32 entFlags = (g.R() & ~0xC0u) | 0x80u;                       // visible, no pospuesto
    u32 seedw = g.R() & 0xFFFFu;
    u32 pmodel = 19000 + g.Pick(8);
    u32 weaponObj = g.Chance(3, 4) ? Tok(L_WOBJ) : 0u;
    u32 pdataTok = g.Chance(2, 3) ? Tok(L_PDATA) : 0u;
    u32 stand = g.Chance(1, 3) ? Tok(L_ENT) : 0u;
    u32 veh = g.Chance(1, 2) ? Tok(L_VEH) : 0u;
    u32 matPedTok = Tok(L_MATPED);
    u32 speed[3], shift[2];
    for (int i = 0; i < 3; ++i) speed[i] = g.Chance(1, 3) ? 0u : g.Norm(0x70, 0x7F, g.Chance(1, 2));
    for (int i = 0; i < 2; ++i) shift[i] = g.Chance(2, 3) ? 0u : g.Norm(0x70, 0x7F, g.Chance(1, 2));
    u32 f588 = g.Chance(1, 2) ? 0x47C34FFFu : g.Norm(0x78, 0x90, g.Chance(1, 4));
    u32 gAlpha1 = g.Chance(1, 2) ? 0u : g.R() & 0xFFFFu, gProg1 = g.R() & 0xFFFFu, gAlpha2 = g.Chance(1, 2) ? 0u : g.R() & 0xFFFFu, gProg2 = g.R() & 0xFFFFu;
    u32 f72F = g.Chance(1, 2) ? 0u : g.Pick(256);
    u32 pedState = g.Chance(1, 4) ? g.OneOf({0u, 0x10u, 0x29u, 0x37u, 0x36u}) : g.Pick(60);
    u32 moveState = g.Chance(1, 2) ? g.Pick(2) : g.Pick(8);
    u32 createdBy = g.Chance(1, 4) ? 2u : g.Pick(4);
    u32 ts = g.Chance(1, 6) ? g.EdgeF() : g.Norm(0x76, 0x80);
    u32 area = g.Chance(7, 8) ? 0u : 1 + g.Pick(20);
    c.frame = g.R();
    c.rAlpha = g.Chance(3, 4) ? g.Pick(300) : g.OneOf({0xFFFFFFF0u, 0u, 0xFFu, 0xF8u, 0xF0u, 0x100u, 7u, 8u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu});
    c.rSwim = g.Chance(1, 4) ? Tok(L_INTEL) : 0u;
    c.rCol = g.Chance(1, 2) ? 0u : (g.Chance(1, 2) ? 1u : g.R());
    c.rFindPlayer = g.Pick(3);
    c.rIsMember = g.Chance(1, 2);
    c.rBlood = g.Chance(3, 4);
    c.rFx = g.Chance(3, 4);
    c.rBone = g.Chance(1, 2) ? g.Pick(32) : g.R();
    c.plCoors = {g.Norm(0x70, 0x85, g.Chance(1, 2)), g.Norm(0x70, 0x85, g.Chance(1, 2)), g.Chance(2, 3) ? g.Norm(0x70, 0x88, g.Chance(1, 2)) : g.OneOf({0x446D6000u, 0x446D6001u, 0x446D5FFFu, 0x7FC00000u, 0x7F800000u, 0xFF800000u, 0u, 0x80000000u})};   // 950.0f +/- ulp
    c.camMat = g.Chance(1, 2) ? Tok(L_MATCAM) : 0u;
    for (int i = 0; i < 3; ++i) c.camPos[i] = g.Norm(0x78, 0x85, g.Chance(1, 2));
    c.miPedIdx = 19000 + g.Pick(8);
    c.miVehIdx = g.Chance(1, 4) ? c.miPedIdx : 19000 + g.Pick(8);
    c.miPedCol = g.Pick(2);
    c.miVehCol = g.Pick(2);
    pmodel = c.miPedIdx;
    u32 vmodel = c.miVehIdx;
    u32 groundFlags = (g.R() & ~0x20u) | (g.Chance(1, 2) ? 0x20u : 0u);
    u32 groundType = g.Chance(1, 2) ? 2u : g.Pick(8);
    u32 groundSub = g.Chance(1, 3) ? 6u : g.Pick(12);
    u32 plGroup = g.Pick(8);
    u32 wet = g.Chance(1, 3) ? g.Pick(256) : g.OneOf({0u, 1u, 99u, 100u, 101u, 127u, 128u, 129u, 255u, 254u});
    u32 cover = g.Chance(1, 3) ? g.Pick(256) : g.OneOf({0u, 0x31u, 0x32u, 0x33u, 0x64u, 0xFFu, 0x80u});
    u32 pedZ = g.Norm(0x78, 0x85, g.Chance(1, 2));
    bool hasMatrix = true;
    bool mutate = false;

    switch (f) {
    case 0: pedType = g.Pick(2); wtype = 0x12; wfx = 0; weaponObj = Tok(L_WOBJ); f474 &= ~0x2u; entFlags |= 0x80u; phys &= ~0x100u;
            if (g.Chance(1, 5)) { phys |= 0x100u; } c.rSwim = g.Chance(1, 4) ? Tok(L_INTEL) : 0u; break;
    case 1: pedType = g.Pick(2); wtype = 0x12; wfx = Tok(L_FX);
            switch (g.Pick(5)) {
            case 0: f474 |= 0x2u; break;
            case 1: entFlags &= ~0x80u; break;
            case 2: phys |= 0x100u; c.rSwim = Tok(L_INTEL); break;
            case 3: weaponObj = 0u; break;
            default: break;   // se mantiene
            }
            break;
    case 2: pedType = 2 + g.Pick(22); if (g.Chance(1, 2)) wtype = 0x12; wfx = g.Chance(1, 2) ? Tok(L_FX) : 0u; if (g.Chance(1, 3)) { pedType = g.Pick(2); wtype = g.Pick(0x12); } break;
    case 3: switch (g.Pick(4)) {
            case 0: f478 |= 0x200000u; break;
            case 1: f474 |= 0x1000u; area = 1 + g.Pick(20); break;
            case 2: f474 |= 0x1000u; area = 0; break;
            default: f474 |= 0x1000u; area = 0; c.plCoors[2] = g.OneOf({0x446D6000u, 0x446D6001u, 0x446D5FFFu, 0x7FC00000u, 0x7F800000u, 0xFF800000u, 0x44800000u}); break;
            }
            pedType = g.Chance(1, 2) ? g.Pick(2) : 2 + g.Pick(8); break;
    case 4: pedType = 2 + g.Pick(8); if (g.Chance(1, 2)) f470 |= 0x8u;
            c.frame = (g.R() & ~0x1Fu) | ((0x20u - (seedw & 0x1Fu)) & 0x1Fu); if (g.Chance(1, 3)) c.frame = g.R();
            if (g.Chance(1, 3)) c.rAlpha = g.OneOf({0xF8u, 0xF0u, 0xEFu, 0xFFu, 0xFEu, 8u, 7u, 0u, 1u, 0x100u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFF8u, 0xFFFFFFFFu}); break;
    case 5: pedType = 2 + g.Pick(8); f470 |= 0x80000000u; f588 = g.Chance(5, 6) ? 0x47C34FFFu : 0x47C34FFEu + g.Pick(4);
            if (g.Chance(1, 2)) f46C |= 1u; else { f46C &= ~1u; if (g.Chance(3, 4)) f470 |= 0x80000u; }
            if (g.Chance(1, 8)) f470 |= 0x200000u; if (g.Chance(1, 8)) stand = Tok(L_ENT); else stand = 0u;
            switch (g.Pick(6)) {
            case 0: for (int i = 0; i < 3; ++i) speed[i] = 0u; break;
            case 1: speed[0] = g.OneOf({0x3DCCCCCDu, 0x3DCCCCCCu, 0x3DCCCCCEu, 0x3CA3D70Au, 0x3C23D70Au, 0xBDCCCCCDu}); speed[1] = 0; speed[2] = 0; break;    // alrededor de 0.1 (= sqrt(0.01))
            case 2: speed[0] = g.Norm(0x73, 0x7B, g.Chance(1, 2)); speed[1] = g.Norm(0x73, 0x7B, g.Chance(1, 2)); speed[2] = g.Norm(0x73, 0x7B, g.Chance(1, 2)); break;
            case 3: speed[0] = g.EdgeF(); break;
            default: break;
            }
            break;
    case 6: pedType = 2 + g.Pick(8); f470 |= 0x200000u; f478 |= 0x100u; pedZ = g.Norm(0x78, 0x85, g.Chance(1, 2)); f588 = g.Chance(1, 2) ? g.EdgeF() : g.Norm(0x78, 0x8A, g.Chance(1, 4));
            if (g.Chance(1, 3)) hasMatrix = false; if (g.Chance(1, 3)) f478 &= ~0x100u; if (g.Chance(1, 3)) f470 &= ~0x200000u; break;
    case 7: pedType = 2 + g.Pick(8); ts = g.Chance(1, 2) ? g.EdgeF() : g.Norm(0x70, 0x90, g.Chance(1, 8));
            gAlpha1 = g.Chance(1, 4) ? g.OneOf({0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, 0x100u}) : g.R() & 0xFFFFu; gProg1 = g.Chance(1, 4) ? g.OneOf({0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, 0xFFFEu}) : g.R() & 0xFFFFu;
            gAlpha2 = g.Chance(1, 4) ? g.OneOf({0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, 0x100u}) : g.R() & 0xFFFFu; gProg2 = g.Chance(1, 4) ? g.OneOf({0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, 0xFFFEu}) : g.R() & 0xFFFFu;
            if (g.Chance(1, 3)) { gAlpha1 = 1 + g.Pick(2000); gProg1 = 1 + g.Pick(30); gAlpha2 = 1 + g.Pick(2000); gProg2 = 1 + g.Pick(30); ts = g.Norm(0x7A, 0x7F); }
            break;
    case 8: pedType = 2 + g.Pick(8); createdBy = g.Chance(3, 4) ? 2u : g.Pick(4); f478 |= g.Chance(1, 2) ? 0x100u : 0u; c.rFindPlayer = g.Pick(3); plGroup = g.Pick(8); break;
    case 9: pedType = 2 + g.Pick(8); pdataTok = g.Chance(5, 6) ? Tok(L_PDATA) : 0u; phys |= g.Chance(2, 3) ? 0x100u : 0u;
            if (g.Chance(3, 4)) c.muts.push_back({S_BUOY, 0x40, 0x100u});      // el EXE borra la bandera antes de ProcessBuoyancy; el real la repone: el falso lo imita con un xor
            break;
    case 10: pedType = 2 + g.Pick(8); f46C |= g.Chance(5, 6) ? 1u : 0u; stand = g.Chance(5, 6) ? Tok(L_ENT) : 0u; c.force = g.Chance(1, 6) ? g.Pick(256) : 0u; groundFlags = (groundFlags & ~0x20u) | (g.Chance(1, 2) ? 0x20u : 0u);
            groundType = g.Chance(2, 3) ? 2u : g.Pick(8); groundSub = g.Chance(1, 2) ? 6u : g.Pick(12); break;
    case 11:
            if (g.Chance(2, 3)) {   // todas las condiciones de omitir la fisica verdaderas salvo (a lo sumo) una
                pedType = 2 + g.Pick(8); f46C |= 1u; f46C &= ~0x200u; stand = 0u; moveState = g.Pick(2); pedState = g.Pick(0x29); shift[0] = shift[1] = g.Chance(1, 4) ? 0x80000000u : 0u;
                for (int i = 0; i < 3; ++i) speed[i] = g.Chance(1, 4) ? 0x80000000u : 0u;
                ts = g.Norm(0x76, 0x80); f46C &= ~0x1u; f46C |= 1u;
                switch (g.Pick(14)) {
                case 0: f46C &= ~1u; break;
                case 1: speed[0] = g.Norm(0x70, 0x7F, g.Chance(1, 2)); break;
                case 2: speed[1] = g.Chance(1, 2) ? 0x7FC00000u : g.Norm(0x20, 0x7F); break;
                case 3: speed[2] = g.OneOf({0x00000001u, 0x80000001u, 0x3E800000u, 0xBE800000u, 0x7F800000u}); break;
                case 4: moveState = 2 + g.Pick(6); break;
                case 5: shift[0] = g.Norm(0x70, 0x7F, g.Chance(1, 2)); break;
                case 6: shift[1] = g.OneOf({0x00000001u, 0x7FC00000u, 0x3F800000u}); break;
                case 7: pedState = 0x29; break;
                case 8: f46C |= 0x200u; break;
                case 9: stand = Tok(L_ENT); groundFlags &= ~0x20u; groundType = g.OneOf({1u, 3u, 4u, 5u, 0u, 7u}); break;
                case 10: pedType = g.Pick(2); break;
                default: break;       // todas verdaderas
                }
                break;
            }
            pedType = g.Chance(1, 4) ? g.Pick(2) : 2 + g.Pick(8); f46C |= 1u; if (g.Chance(1, 2)) f46C &= ~1u; stand = g.Chance(1, 5) ? Tok(L_ENT) : 0u; if (stand) { groundFlags &= ~0x20u; groundType = g.OneOf({1u, 3u, 4u, 5u, 0u, 7u}); }
            for (int i = 0; i < 3; ++i) speed[i] = g.Chance(2, 3) ? 0u : (g.Chance(1, 4) ? g.OneOf({0x80000000u, 0x3E800000u, 0x3E800001u, 0x3E7FFFFFu, 0x7FC00000u, 0xBE800000u}) : g.Norm(0x70, 0x80, g.Chance(1, 2)));
            if (g.Chance(1, 4)) { speed[2] = g.OneOf({0x3E800000u, 0x3E800001u, 0x3E7FFFFFu, 0x3F800000u, 0x7F800000u}); }
            ts = g.Chance(1, 4) ? g.EdgeF() : g.Norm(0x76, 0x80);
            moveState = g.Chance(2, 3) ? g.Pick(2) : g.Pick(8); shift[0] = g.Chance(2, 3) ? 0u : shift[0]; shift[1] = g.Chance(2, 3) ? 0u : shift[1]; pedState = g.Chance(1, 4) ? 0x29u : g.Chance(1, 4) ? 0x37u : g.Pick(60);
            f46C = (f46C & ~0x200u) | (g.Chance(1, 5) ? 0x200u : 0u); break;
    case 12: pedType = 2 + g.Pick(8); f46C |= g.Chance(1, 2) ? 0x40000u : 0u; f72F = g.Chance(1, 2) ? 0u : 1 + g.Pick(255); f46C &= ~0x100u; c.rBlood = g.Chance(5, 6);
            c.frame = (g.R() & ~3u) | (g.Chance(1, 5) ? 1 + g.Pick(3) : 0u);
            if (g.Chance(1, 6)) hasMatrix = false;
            for (int i = 0; i < 3; ++i) { c.camPos[i] = g.Norm(0x78, 0x85, g.Chance(1, 2)); if (g.Chance(1, 2)) c.camPos[i] = FBits(float(i) * 3.0f + 1.0f); }
            break;
    case 13: pedType = 2 + g.Pick(8); f46C |= g.Chance(1, 2) ? 0x100u : 0u; veh = g.Chance(3, 4) ? Tok(L_VEH) : 0u;
            c.frame = (g.R() & ~0x3FFFu) | ((0x4000u - seedw) & 0x3FFFu); if (g.Chance(1, 3)) c.frame = g.R(); f478 |= g.Chance(1, 2) ? 0x400u : 0u; wtype = g.Chance(1, 2) ? 9u : wtype;
            pedState = g.Chance(1, 3) ? 0x10u : pedState; phys |= g.Chance(1, 3) ? 0x100u : 0u; c.rSwim = g.Chance(1, 3) ? Tok(L_INTEL) : 0u; break;
    default: break;
    }
    if (f == 14 || f == 15) {   // mezcla: se activan banderas de corte con probabilidad baja
        if (g.Chance(1, 20)) f478 |= 0x200000u;
        if (g.Chance(1, 8)) f474 |= 0x1000u;
        if (g.Chance(1, 8)) f470 |= 0x80000000u;
        if (g.Chance(1, 8)) f470 |= 0x200000u;
        if (g.Chance(1, 8)) f478 |= 0x100u;
        if (g.Chance(1, 8)) f46C |= 0x40000u;
        if (g.Chance(1, 8)) f46C |= 0x100u;
        if (g.Chance(1, 8)) f478 |= 0x400u;
        if (g.Chance(1, 8)) phys |= 0x100u;
        if (g.Chance(1, 8)) f470 |= 0x8u;
        if (g.Chance(1, 6)) wtype = g.OneOf({0x12u, 9u});
        if (g.Chance(1, 5)) { pedType = g.Pick(2); }
        if (g.Chance(1, 8)) f474 |= 0x2u;
        if (g.Chance(1, 8)) entFlags &= ~0x80u;
        if (f == 15) mutate = true;
    }
    // ---- validez: lo que el EXE desreferenciaria sin comprobar (el banco no lo modela)
    if (createdBy == 2 && c.rFindPlayer == 0) c.rFindPlayer = 1 + g.Pick(2);      // FindPlayerPed nulo: el EXE desreferencia el resultado
    if (f != 6 && f != 12) hasMatrix = true;                                        // sin matriz solo cuando la gota de sangre no puede desreferenciarla

    // ---- ped
    const u32 woff = 0x5A0 + 0x1C * slot;
    Obj& p = c.ped;
    p.P(0x00, Tok(L_VTBL));
    p.P(0x14, hasMatrix ? matPedTok : 0u);
    p.P(0x18, Tok(L_CLUMP));
    p.W32(0x1C, entFlags);
    p.W16(0x20, seedw);
    p.W16(0x22, pmodel);
    for (int i = 0; i < 3; ++i) p.W32(0x04 + 4 * i, g.Norm(0x78, 0x85, g.Chance(1, 2)));   // posicion de la colocacion (se usa sin matriz); z en +0x0C
    p.W32(0x0C, pedZ);
    p.W32(0x40, phys);
    for (int i = 0; i < 3; ++i) p.W32(0x44 + 4 * i, speed[i]);
    p.P(0x128, g.Chance(1, 2) ? 0u : kAnyPtr);
    p.W32(0x46C, f46C);
    p.W32(0x470, f470);
    p.W32(0x474, f474);
    p.W32(0x478, f478);
    p.P(0x47C, Tok(L_INTEL));
    p.P(0x480, pdataTok);
    p.W8(0x484, createdBy);
    p.W32(0x4D8, shift[0]);
    p.W32(0x4DC, shift[1]);
    p.P(0x4F4, weaponObj);
    p.W16(0x504, gAlpha1);
    p.W16(0x506, gProg1);
    p.W16(0x508, gAlpha2);
    p.W16(0x50A, gProg2);
    p.W32(0x530, pedState);
    p.W32(0x534, moveState);
    p.P(0x568, stand);
    p.W32(0x588, f588);
    p.P(0x58C, veh);
    p.W32(0x598, pedType);
    p.W8(0x718, slot);
    p.W32(woff, wtype);
    p.P(woff + 0x18, wfx);
    p.W32(0x72C, (g.R() & 0x00FFFFFFu) | (f72F << 24));        // +0x72C..+0x72E basura valida, +0x72F = field_72F
    // ---- otros objetos
    c.pdata.W32(0x38, plGroup);
    c.pdata.W8(0x8C, wet);
    c.pdata.W8(0x8E, cover);
    c.pdata2.W32(0x38, plGroup);
    c.pped.P(0x480, Tok(L_PDATA2));
    c.ent.W32(0x1C, groundFlags);
    c.ent.W8(0x36, groundType | (g.R() & 0xF8u));
    c.ent.W32(0x594, groundSub);
    c.veh.P(0x14, Tok(L_MATOTHER));
    c.veh.W16(0x22, vmodel);
    c.matPed.W32(0x30, g.Norm(0x78, 0x85, g.Chance(1, 2)));
    c.matPed.W32(0x34, g.Norm(0x78, 0x85, g.Chance(1, 2)));
    c.matPed.W32(0x38, pedZ);
    c.matCam.W32(0x30, c.camPos[0]);
    c.matCam.W32(0x34, c.camPos[1]);
    c.matCam.W32(0x38, c.camPos[2]);
    c.clump.W8(0x00, 2);        // rpCLUMP: GetRpClump() comprueba el tipo del objeto en Debug
    c.timestep = ts;
    c.area = area;
    // posicion de la camara respecto del ped (familia 12: cerca o lejos de la gota); sin matriz el ped usa la posicion de la colocacion y la camara se deja lejos
    if (f == 12 || !hasMatrix) {
        const bool isNear = hasMatrix && g.Chance(2, 3);
        for (int i = 0; i < 3; ++i) {
            const float base = hasMatrix ? BitsF(c.matPed.Get(0x30 + 4 * i, 4)) : BitsF(p.Get(0x04 + 4 * i, 4));
            c.camPos[i] = isNear ? FBits(base + float(int(g.Pick(60)) - 30) * (g.Chance(1, 4) ? 1.7f : 0.2f)) : FBits(base + 70.0f);
        }
        c.matCam.W32(0x30, c.camPos[0]); c.matCam.W32(0x34, c.camPos[1]); c.matCam.W32(0x38, c.camPos[2]);
        if (!hasMatrix) c.rBlood = 1;
    }
    // ---- mutaciones (solo en la familia 15 y a veces en la 14)
    if (mutate || (f == 14 && g.Chance(1, 4))) {
        const u32 count = 1 + g.Pick(3);
        for (u32 i = 0; i < count; ++i) {
            Mut m{g.Pick(S_COUNT), kMutOffsets[g.Pick(u32(std::size(kMutOffsets)))], 0};
            u32 mask = 1u << g.Pick(32);
            if (g.Chance(1, 3)) mask |= 1u << g.Pick(32);
            if (m.off == 0x504 || m.off == 0x508) mask &= 0x7FFF7FFFu;
            if (m.off == 0x598) mask = 1u << g.Pick(2);
            if (m.off == 0x530) mask = 1u << g.Pick(6);
            if (m.off == 0x534) mask = 1u << g.Pick(3);
            m.mask = mask;
            c.muts.push_back(m);
        }
    }
    return c;
}

// ================================================================================================ tabla de modelos y serializacion de la fila `input` (mismo texto en el banco y en fixture_mirror)
struct MiEntry { u32 idx, mi, cm; };
inline std::vector<MiEntry> MiTable(const PCase& c) {
    std::vector<MiEntry> t;
    t.push_back({c.miPedIdx, L_MI0, c.miPedCol ? L_CM1 : L_CM0});
    if (c.miVehIdx != c.miPedIdx) t.push_back({c.miVehIdx, L_MI1, c.miVehCol ? L_CM1 : L_CM0});
    return t;
}
inline void Put(std::string& s, const char* fmt, u32 a = 0, u32 b = 0, u32 c = 0, u32 d = 0) {
    char buf[160];
    std::snprintf(buf, sizeof buf, fmt, a, b, c, d);
    s += buf;
}
inline void PutObj(std::string& s, const char* key, const Obj& o) {
    s += '"'; s += key; s += "\":[";
    for (std::size_t i = 0; i < o.p.size(); ++i) {
        if (i) s += ',';
        Put(s, "[%u,%u,%u,%u]", o.p[i].off, o.p[i].size, o.p[i].value, o.p[i].ptr ? 1u : 0u);
    }
    s += "],";
}
inline void PutArr(std::string& s, const char* key, const u32* v, int n) {
    s += '"'; s += key; s += "\":[";
    for (int i = 0; i < n; ++i) Put(s, i ? ",%u" : "%u", v[i]);
    s += "],";
}
inline std::string JsonK(const KCase& c) {
    std::string s = "{";
    Put(s, "\"family\":%u,\"timestep\":%u,", c.family, c.timestep);
    PutArr(s, "bools", c.bools.data(), 2);
    PutObj(s, "ped", c.ped);
    Put(s, "\"eax\":%u,\"edx\":%u,\"stack_seed\":%u}", c.eax, c.edx, c.stack);
    return s;
}
inline std::string JsonS(const SCase& c) {
    std::string s = "{";
    Put(s, "\"family\":%u,\"ignore_arg\":%u,\"r_climb\":%u,", c.family, c.ignoreArg, c.rClimb);
    PutArr(s, "bools", c.bools.data(), 4);
    PutObj(s, "ped", c.ped);
    PutObj(s, "ent", c.ent);
    PutObj(s, "objinfo", c.objinfo);
    PutObj(s, "mat_ped", c.matPed);
    PutObj(s, "mat_ent", c.matEnt);
    Put(s, "\"eax\":%u,\"edx\":%u,\"stack_seed\":%u}", c.eax, c.edx, c.stack);
    return s;
}
inline std::string JsonP(const PCase& c) {
    std::string s = "{";
    Put(s, "\"family\":%u,\"frame\":%u,\"timestep\":%u,\"area\":%u,", c.family, c.frame, c.timestep, c.area);
    Put(s, "\"force\":%u,\"cam_mat\":%u,", c.force, c.camMat);
    PutArr(s, "cam_pos", c.camPos.data(), 3);
    s += "\"mi\":[";
    const auto mi = MiTable(c);
    for (std::size_t i = 0; i < mi.size(); ++i) Put(s, i ? ",[%u,%u,%u]" : "[%u,%u,%u]", mi[i].idx, mi[i].mi, mi[i].cm);
    s += "],";
    Put(s, "\"r_swim\":%u,\"r_alpha\":%u,\"r_col\":%u,\"r_find_player\":%u,", c.rSwim, c.rAlpha, c.rCol, c.rFindPlayer);
    Put(s, "\"r_is_member\":%u,\"r_blood\":%u,\"r_fx\":%u,\"r_bone\":%u,", c.rIsMember, c.rBlood, c.rFx, c.rBone);
    Put(s, "\"r_junk\":%u,\"seed\":%u,\"blood_tex\":%u,", c.rJunk, c.seed, c.bloodTex);
    PutArr(s, "pl_coors", c.plCoors.data(), 3);
    s += "\"muts\":[";
    for (std::size_t i = 0; i < c.muts.size(); ++i) Put(s, i ? ",[%u,%u,%u]" : "[%u,%u,%u]", c.muts[i].stub, c.muts[i].off, c.muts[i].mask);
    s += "],";
    PutObj(s, "ped", c.ped);
    PutObj(s, "pdata", c.pdata);
    PutObj(s, "pdata2", c.pdata2);
    PutObj(s, "pped", c.pped);
    PutObj(s, "ent", c.ent);
    PutObj(s, "veh", c.veh);
    PutObj(s, "mat_ped", c.matPed);
    PutObj(s, "mat_veh", c.matVeh);
    PutObj(s, "mat_cam", c.matCam);
    PutObj(s, "clump", c.clump);
    Put(s, "\"eax\":%u,\"edx\":%u,\"stack_seed\":%u}", c.eax, c.edx, c.stack);
    return s;
}
}  // namespace PedWrappers60Fix

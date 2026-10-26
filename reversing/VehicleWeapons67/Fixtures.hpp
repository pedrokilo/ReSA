#pragma once
// Fixtures del banco VehicleWeapons67 (lote 67): CVehicle::ProcessWeapons (0x6E3950), thiscall sin argumentos, `ret` simple. Solo C++ estandar: las incluyen Core.hpp / LiveComparison.hpp.
//
// El caso es un vector `v` de 40 palabras (ver abajo) mas los valores iniciales de EAX/EDX/pila (ECX = this). El vehiculo falso (0xA40 bytes: llega hasta +0xA0C del helicoptero),
// el objeto RenderWare (clump) con su marco padre, la lista de fogonazos y los demas objetos viven en una arena propia con guardas; los colaboradores se sustituyen por falsos que
// registran sus argumentos en orden. Un "codigo de puntero" se resuelve con Ptr(): 0 nulo, 1 otra entidad de la arena, 2 el propio vehiculo, 3 el marco, 4 el clump, 5..12 los
// ocho FxSystem_c falsos, y cualquier valor >= 0xA0000000 se usa tal cual (basura: nunca se desreferencia).
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

namespace V67Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFns = 1;
constexpr u32 kMaxEvents = 40;
constexpr u32 kVWords = 40;
constexpr u32 kEvArgs = 4;
constexpr u32 kFamilies = 16;

// Disposicion de la arena (bytes desde su base): 0x40 guarda + 0xA40 vehiculo + 0x20 clump + 0x80 marco + 0x40 lista de fogonazos + 0x40 fogonazos falsos + 0x40 otra entidad + 0x40 guarda.
constexpr u32 oG0 = 0x000, oVeh = 0x040, oClump = 0xA80, oFrame = 0xAA0, oFxArr = 0xB20, oFx = 0xB60, oOther = 0xBA0, oG1 = 0xBE0;
constexpr u32 kArenaSize = 0xC20;
constexpr u32 kArenaWords = kArenaSize / 4;
static_assert(oVeh == oG0 + 0x40 && oClump == oVeh + 0xA40 && oFrame == oClump + 0x20 && oFxArr == oFrame + 0x80 && oFx == oFxArr + 0x40 && oOther == oFx + 0x40 &&
              oG1 == oOther + 0x40 && kArenaSize == oG1 + 0x40, "arena");
// Campos del vehiculo (desplazamientos del EXE, verificados con el desensamblado de 0x6E3950)
constexpr u32 kVClump = 0x18, kVModel = 0x22, kVStatus = 0x36, kVPhysFlags = 0x40, kVMission = 0x3BA, kVAiTarget = 0x41C, kVFlags42C = 0x42C, kVWeapon = 0x513, kVSubType = 0x594,
              kVPlaneFx = 0x9D4, kVPlaneFlag = 0x9DC, kVHeliFx = 0xA0C;
constexpr u64 kLabel = 0x100000000ull;   // marca de "puntero a la arena": kLabel + desplazamiento

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }

struct Spec {
    u32 family{};
    std::array<u32, kVWords> v{};
    u32 stack{}, eax{}, edx{};
};

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_) : n(n_) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
};

// v[]: 0 subtipo (+0x594); 1 banderas fisicas (+0x40); 2 estado (5 bits, byte +0x36 >> 3); 3 tres bits bajos de +0x36; 4 banderas +0x42C (bit 4 = bFireGun); 5 modelo (+0x22);
// 6 arma en uso (+0x513); 7 mision (+0x3BA); 8 objetivo del piloto automatico (+0x41C, codigo); 9 +0x9DC del avion; 10 lista de fogonazos del helicoptero (+0xA0C: 0 nula, 1 la lista);
// 11 lista del avion (+0x9D4); 12 clump (+0x18: 0 nulo, 1 el clump); 13 padre del clump (codigo); 14..21 los ocho fogonazos (codigos); 22, 23 retornos 1.o y 2.o de FindPlayerVehicle (codigos);
// 24 GetPlaneWeaponFiringStatus escribe el byte; 25 el byte; 26 escribe el tipo; 27 el tipo; 28 retorno (codigo); 29 SelectPlaneWeapon escribe el arma; 30 el arma; 31 GetPlaneNumGuns.
constexpr u32 kSub = 0, kPhys = 1, kStat = 2, kStLow = 3, kF42C = 4, kModel = 5, kWeapon = 6, kMission = 7, kAiTarget = 8, kPlaneFlag = 9, kHeliFx = 10, kPlaneFx = 11, kClump = 12,
              kParent = 13, kFxEntries = 14, kFpv1 = 22, kFpv2 = 23, kStWrite = 24, kStByte = 25, kOrdWrite = 26, kOrdVal = 27, kStRet = 28, kSelWrite = 29, kSelVal = 30, kNumGuns = 31;

inline u32 Garbage(Gen& g) { return 0xA0000000u | (g.R() & 0x0FFFFFF0u); }
// codigo de puntero: pesos (nulo, otra, vehiculo, marco, clump, fogonazo, basura)
inline u32 PtrCode(Gen& g, u32 wNull, u32 wOther, u32 wThis, u32 wFrame, u32 wFx, u32 wGarbage) {
    const u32 total = wNull + wOther + wThis + wFrame + wFx + wGarbage;
    u32 r = g.P(total);
    if (r < wNull) return 0u;
    r -= wNull;
    if (r < wOther) return 1u;
    r -= wOther;
    if (r < wThis) return 2u;
    r -= wThis;
    if (r < wFrame) return 3u;
    r -= wFrame;
    if (r < wFx) return 5u + g.P(8);
    return Garbage(g);
}

inline Spec MakeSpec(u32 n) {
    Spec s;
    Gen g(n + 0x31000u);
    const u32 fam = n % kFamilies;
    s.family = fam;
    s.stack = StackSeed(n);
    s.eax = g.R();
    s.edx = g.R();
    // subtipo: 4 avion, 3 helicoptero, resto sin lista de fogonazos
    u32 sub;
    switch (g.P(10)) {
    case 0: case 1: case 2: case 3: sub = 4; break;
    case 4: case 5: case 6: sub = 3; break;
    case 7: sub = g.C(50) ? 8u : (g.C(50) ? 0u : 5u + g.P(6)); break;
    case 8: sub = 0xFFFFFFFFu; break;
    default: sub = g.R(); break;
    }
    switch (fam) {
    case 0: case 1: case 2: case 5: case 13: sub = 4; break;
    case 3: case 4: sub = 3; break;
    case 6: if (sub == 3 || sub == 4) sub = g.C(50) ? 0u : 1u + g.P(2); break;
    default: break;
    }
    s.v[kSub] = sub;
    // estado: 0 jugador, 8 mando a distancia (camino del jugador); el resto, camino de la IA
    u32 st;
    switch (g.P(10)) {
    case 0: case 1: case 2: case 3: st = 0; break;
    case 4: case 5: st = 8; break;
    default: st = g.C(50) ? 1u + g.P(7) : 9u + g.P(23); break;
    }
    switch (fam) {
    case 3: case 10: case 11: case 12: case 0: case 1: case 2: st = 0; break;
    case 4: st = 8; break;
    case 7: case 8: case 9: if (st == 0 || st == 8) st = 1u + g.P(7); break;
    default: break;
    }
    s.v[kStat] = st;
    s.v[kStLow] = g.P(8);
    // banderas fisicas: bit 29 (destruido) en ~8 %; las demas aleatorias
    u32 phys = g.R();
    phys = g.C(fam == 5 ? 100 : 8) ? (phys | 0x20000000u) : (phys & ~0x20000000u);
    s.v[kPhys] = phys;
    u32 f42 = g.R();
    f42 = (fam == 7 || fam == 8) ? (f42 | 0x10u) : fam == 9 ? (f42 & ~0x10u) : (g.C(50) ? (f42 | 0x10u) : (f42 & ~0x10u));
    s.v[kF42C] = f42;
    u32 model;
    switch (g.P(10)) {
    case 0: case 1: case 2: case 3: model = 0x208; break;
    case 4: model = 0x207; break;
    case 5: model = 0x209; break;
    default: model = g.R() & 0xFFFFu; break;
    }
    if (fam == 8 && g.C(80)) model = 0x208;
    s.v[kModel] = model;
    u32 weapon;
    switch (g.P(12)) {
    case 0: case 1: case 2: weapon = 1; break;
    case 3: case 4: weapon = 2; break;
    case 5: case 6: case 7: weapon = 3; break;
    case 8: case 9: weapon = 4; break;
    case 10: weapon = g.C(50) ? 0u : 5u; break;
    default: weapon = g.R() & 0xFFu; break;
    }
    switch (fam) {
    case 3: case 7: weapon = 1; break;
    case 4: case 12: weapon = 2; break;
    case 8: case 10: weapon = 3; break;
    case 11: weapon = 4; break;
    case 13: weapon = g.C(50) ? 0u : (g.C(50) ? 5u : (g.R() & 0xFFu)); break;
    default: break;
    }
    s.v[kWeapon] = weapon;
    u32 mission;
    switch (g.P(10)) {
    case 0: case 1: mission = 0x23; break;
    case 2: case 3: mission = 0x3B; break;
    case 4: case 5: mission = 0x3F; break;
    case 6: { static const u32 adj[] = {0x22, 0x24, 0x3A, 0x3C, 0x3E, 0x40, 0xA3, 0xBB, 0xBF, 0x00, 0xFF}; mission = adj[g.P(11)]; break; }
    default: mission = g.R() & 0xFFu; break;
    }
    s.v[kMission] = mission;
    s.v[kAiTarget] = PtrCode(g, 30, 30, 5, 0, 0, 35);
    s.v[kPlaneFlag] = g.C(60) ? 0u : (g.C(50) ? 1u : g.R());
    if (fam == 0) s.v[kPlaneFlag] = 0;
    else if (fam == 1) s.v[kPlaneFlag] = 1u + g.P(0xFFFFFFFEu);
    s.v[kHeliFx] = g.C(75) ? 1u : 0u;
    s.v[kPlaneFx] = g.C(75) ? 1u : 0u;
    s.v[kClump] = g.C(80) ? 1u : 0u;
    s.v[kParent] = PtrCode(g, 15, 0, 0, 60, 0, 25);
    for (u32 i = 0; i < 8; ++i) s.v[kFxEntries + i] = PtrCode(g, 35, 0, 0, 0, 50, 15);
    // 1.o retorno de FindPlayerVehicle (comparado con `this` en la rama del avion): el propio vehiculo en las familias que ejercen la mira
    s.v[kFpv1] = (fam == 0 || fam == 1 || fam == 5 || fam == 14) ? (g.C(85) ? 2u : PtrCode(g, 50, 30, 0, 0, 0, 20)) : PtrCode(g, 30, 25, 30, 0, 0, 15);
    s.v[kFpv2] = PtrCode(g, 25, 25, 30, 0, 0, 20);
    s.v[kStWrite] = g.C(75) ? 1u : 0u;
    s.v[kStByte] = g.C(80) ? 1u : 0u;
    s.v[kOrdWrite] = g.C(85) ? 1u : 0u;
    u32 ord;
    switch (g.P(10)) {
    case 0: ord = 0; break;
    case 1: case 2: case 3: ord = 1; break;
    case 4: case 5: ord = 2; break;
    case 6: ord = 3; break;
    case 7: ord = 0xFFFFFFFFu; break;
    case 8: ord = 0x80000000u; break;
    default: ord = g.R(); break;
    }
    s.v[kOrdVal] = ord;
    s.v[kStRet] = PtrCode(g, 30, 30, 5, 0, 0, 35);
    s.v[kSelWrite] = g.C(40) ? 1u : 0u;
    s.v[kSelVal] = g.C(70) ? g.P(6) : (g.R() & 0xFFu);
    {
        const u32 r = g.P(100);
        i32 ng;
        if (r < 15) ng = 0;
        else if (r < 22) ng = -1 - i32(g.P(3));
        else if (r < 34) ng = 1;
        else if (r < 44) ng = 2;
        else if (r < 52) ng = 6;
        else ng = 3 + i32(g.P(6));   // 3..8
        if (fam == 14 && ng <= 0) ng = 1 + i32(g.P(8));
        s.v[kNumGuns] = u32(ng);
    }
    return s;
}

// Relleno inicial de la arena: palabras con los bits 31 y 30 puestos (nunca coinciden con una direccion de la arena); los campos de cada caso se escriben encima.
inline u32 FillWord(u32 n, u32 i) { return 0xC0000000u | (Rand(n + 0x777u, 1000u + i) >> 2); }

inline std::string U(u32 v) { return std::to_string(v); }
inline std::string JsonSpec(const Spec& s) {
    std::string o = "{\"fam\":" + U(s.family) + ",\"v\":[";
    for (u32 i = 0; i < kVWords; ++i) o += (i ? "," : "") + U(s.v[i]);
    o += "],\"regs\":[" + U(s.stack) + "," + U(s.eax) + "," + U(s.edx) + "]}";
    return o;
}
}  // namespace V67Fix

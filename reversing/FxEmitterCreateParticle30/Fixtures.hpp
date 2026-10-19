#pragma once
// Fixtures deterministas, arenas y colaboradores sustituidos (stubs) del banco FxEmitterCreateParticle30 (FxEmitter_c::CreateParticle, EXE 0x4A2580). Solo C++ estandar y
// sin tipos del juego: se incluye tanto en el banco dentro del juego (Core.hpp) como en el arnes aislado de offline/ (que ejecuta los bytes reales del EXE reubicados frente
// al codigo C++). fixture_oracle.py repite exactamente las formulas y tablas de este archivo (la comprobacion cruzada esta descrita en README.md).
// Todos los valores float se dan como patrones de bits.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace CpFix {
using u32 = std::uint32_t;
using u8 = std::uint8_t;
constexpr unsigned kCases = 4096;

// Mezcla (finalizador de murmur3 de 32 bits) y generador por (caso, clave).
constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline float F(u32 bits) { float f; std::memcpy(&f, &bits, 4); return f; }
inline u32 B(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }

// ------------------------------------------------------------------------------------------------------------------------------ arena
// Disposicion del arena (todo del banco; ningun objeto del motor FX ni del juego se toca). Sumas: cada hueco es de tamano fijo y los desplazamientos son los acumulados.
//   emisor   FxEmitter_c        0x000..0x040   (0x14 usados: vptr +0, m_PrimBP +4, m_System +8, m_bEnabled +0xC, m_fEmissionIntensity +0x10)
//   prim     FxPrimBP_c         0x040..0x0C0   (0x40 usados; TList_c m_Particles en +0x20: head +0x20, tail +0x24, count +0x28)
//   sistema  FxSystem_c         0x0C0..0x140   (0x74 usados; banderas +0x62 (bit 0x20 = m_MustCreateParticles), m_VelAdd +0x68)
//   emision  EmissionInfo_t     0x140..0x1C0   (0x68 usados)
//   mult     FxPrtMult_c        0x1C0..0x1E0   (0x1C usados)
//   wld      RwMatrix           0x1E0..0x220   (0x40)
//   vel      CVector impuesto   0x220..0x230   (0xC usados)
//   particula A (FxEmitterPrt_c)  0x230..0x270 (0x3C usados) y B 0x270..0x2B0
//   elemento previo de la lista  0x2B0..0x2F0  (0x3C usados; solo si listMode == 1)
// Total 0x2F0. La memoria real va con 16 bytes de guarda a cada lado (kGuard).
constexpr std::size_t kEmitter = 0x000, kPrim = 0x040, kSystem = 0x0C0, kEmission = 0x140, kMult = 0x1C0, kWld = 0x1E0, kVel = 0x220, kPartA = 0x230, kPartB = 0x270, kItem = 0x2B0, kArena = 0x2F0;
static_assert(kPrim == kEmitter + 0x40 && kSystem == kPrim + 0x80 && kEmission == kSystem + 0x80 && kMult == kEmission + 0x80 && kWld == kMult + 0x20 && kVel == kWld + 0x40 && kPartA == kVel + 0x10 &&
              kPartB == kPartA + 0x40 && kItem == kPartB + 0x40 && kArena == kItem + 0x40, "arena");
constexpr std::size_t kGuard = 16;
constexpr unsigned kEmissionWords = 26;
constexpr u32 kVtable = 0x0085A7A4;

// ------------------------------------------------------------------------------------------------------------------------------ generacion de valores
inline constexpr std::array<u32, 40> kSpecial{
    0x00000000, 0x80000000, 0x00000001, 0x80000001, 0x007FFFFF, 0x00800000, 0x3F800000, 0xBF800000, 0x7F800000, 0xFF800000, 0x7FC00000, 0x7FC01234, 0xFFC00001, 0x7FFFFFFF, 0x7F7FFFFF, 0xFF7FFFFF,
    0x3F000000, 0x40000000, 0x41200000, 0x42C80000, 0x3A83126F, 0x437F0000, 0x477FFF00, 0x4B000000, 0x3F7FFFFF, 0x3F800001, 0x3C23D70A, 0x3E800000, 0x437F8000, 0x42C7FFFF, 0x7F000000, 0x00FFFFFF,
    0x3FC90FDB, 0x40490FDB, 0x40C90FDB, 0xC0490FDB, 0x42B40000, 0x43B40000, 0x7149F2CA, 0x0DA24260};
// Margenes de exponente sesgado para valores "naturales" por clase de campo.
enum Kind : unsigned { kUnit = 0, kAngle = 1, kLife = 2, kColor = 3, kScale = 4, kPos = 5 };
struct ExpRange { unsigned lo, hi; };
inline constexpr std::array<ExpRange, 6> kRanges{{{118, 130}, {126, 137}, {120, 130}, {115, 127}, {112, 127}, {120, 133}}};

// Familia del caso (n % 8): 0-3 y 7 "benignas" (pocos especiales), 4 especiales, 5 extremos de exponente, 6 mezcla de especiales y extremos.
inline unsigned Family(unsigned c) { return c % 8; }
inline u32 Val(unsigned c, unsigned key, Kind kind) {
    const unsigned fam = Family(c);
    const unsigned dice = Rand(c, key + 1000) % 100;
    unsigned special = 3, extreme = 2;
    if (fam == 4) { special = 40; extreme = 5; }
    else if (fam == 5) { special = 5; extreme = 40; }
    else if (fam == 6) { special = 20; extreme = 20; }
    else if (fam == 7) { special = 10; extreme = 5; }
    const u32 r = Rand(c, key);
    if (dice < special) return kSpecial[r % kSpecial.size()];
    const u32 mant = r & 0x7FFFFF, sign = (r >> 31) & 1;
    if (dice < special + extreme) {
        const u32 e = (Rand(c, key + 2000) & 1) ? 1 + (Rand(c, key + 3000) % 24) : 231 + (Rand(c, key + 3000) % 24);
        return (sign << 31) | (e << 23) | mant;
    }
    const ExpRange rg = kRanges[kind];
    const u32 e = rg.lo + Rand(c, key + 2000) % (rg.hi - rg.lo + 1);
    // los colores, las escalas y las vidas son casi siempre positivos
    const u32 s = (kind == kColor || kind == kScale || kind == kLife) ? ((r >> 29) % 8 == 0 ? 1u : 0u) : sign;
    return (s << 31) | (e << 23) | mant;
}

struct CpFx {
    unsigned family;
    u32 emission[kEmissionWords];  // EmissionInfo_t: 0 count, 1 radius, 2-4 sizeMin, 5-7 sizeMax, 8 speed, 9 speedBias, 10-12 direction, 13 angleMin, 14 angleMax, 15 life, 16 lifeBias, 17-19 pos, 20 rotMin, 21 rotMax
    u32 mult[7];                   // FxPrtMult_c: red, green, blue, alpha, size, rot, life
    u32 wld[16];                   // RwMatrix de entrada (right, flags, up, pad, at, pad, pos, pad)
    u32 getRw[12];                 // lo que escribe el stub de GetRWMatrix (right, up, at, pos; sin flags ni rellenos)
    u32 velOverride[3];
    u32 velAdd[3];
    u32 timeSince, brightness;
    unsigned hasVel;       // 1: velOverride != nullptr
    unsigned createLocal;  // argumento createLocal (0 o 1)
    unsigned mustCreate;   // bit m_MustCreateParticles de las banderas del sistema
    unsigned poolMode;     // 0: GetParticle da la particula A; 1: primero null y luego B; 2: siempre null
    unsigned listMode;     // 0: lista vacia; 1: ya contiene un elemento
    unsigned flagsOther;   // resto de bits de las banderas del sistema (+0x62)
    unsigned radiusKind, dirKind;
    u32 seed;              // srand del EXE (0x821B11) antes de la llamada
    unsigned fill;         // relleno del arena (0..3)
};

// Radios: caja (|r| < 0.001 o NaN), esfera con radio > 0 (cuarta tirada) y esfera con radio < 0 (sin ella); limites exactos de 0.001f.
inline u32 RadiusValue(unsigned c, unsigned kind) {
    switch (kind) {
    case 0: return 0x00000000;
    case 1: return (Rand(c, 60) & 1) ? 0x3A03126F : 0xBA03126F;      // +-0.0005
    case 2: return 0x3A83126F;                                        // 0.001f (>= umbral: esfera)
    case 3: return 0x3A83126E;                                        // 0.001f - 1 ulp (caja)
    case 4: return Val(c, 61, kScale) & 0x7FFFFFFF;                   // positivo cualquiera
    case 5: return Val(c, 62, kScale) | 0x80000000u;                  // negativo cualquiera
    case 6: return kSpecial[Rand(c, 63) % kSpecial.size()];           // NaN, inf, desnormales, enormes...
    default: return 0xBA83126F;                                       // -0.001f (esfera con radio negativo)
    }
}
inline u32 DirXValue(unsigned c, unsigned kind) {
    switch (kind) {
    case 0: return Val(c, 70, kUnit);                                                          // normal (casi siempre <= 10)
    case 1: return 0x41200001 + (Rand(c, 71) % 64);                                            // > 10 (usa la posicion de la particula)
    case 2: return 0x41200000;                                                                 // == 10 (normaliza)
    case 3: return 0x7FC00000 | (Rand(c, 72) & 0x3FFFFF);                                      // NaN (normaliza)
    case 4: return (Rand(c, 73) & 1) ? 0x7F800000 : 0xFF800000;                                // +-inf
    default: return 0x41200000 + (Rand(c, 74) % 5000);                                         // algo mayor que 10
    }
}

inline CpFx Make(unsigned c) {
    CpFx f{};
    f.family = Family(c);
    const Kind kinds[kEmissionWords] = {kUnit, kScale, kPos, kPos, kPos, kPos, kPos, kPos, kUnit, kUnit, kUnit, kUnit, kUnit, kAngle, kAngle, kLife, kUnit, kPos, kPos, kPos, kAngle, kAngle, kUnit, kUnit, kUnit, kUnit};
    for (unsigned i = 0; i < kEmissionWords; ++i) f.emission[i] = Val(c, 100 + i, kinds[i]);
    f.radiusKind = Rand(c, 50) % 8;
    f.dirKind = Rand(c, 51) % 6;
    f.emission[1] = RadiusValue(c, f.radiusKind);
    f.emission[10] = DirXValue(c, f.dirKind);
    for (unsigned i = 0; i < 7; ++i) f.mult[i] = Val(c, 200 + i, i == 4 || i == 5 ? kScale : (i == 6 ? kLife : kColor));
    for (unsigned i = 0; i < 16; ++i) f.wld[i] = Val(c, 300 + i, kUnit);
    for (unsigned i = 0; i < 12; ++i) f.getRw[i] = Val(c, 400 + i, kUnit);
    for (unsigned i = 0; i < 3; ++i) { f.velOverride[i] = Val(c, 500 + i, kUnit); f.velAdd[i] = Val(c, 510 + i, kUnit); }
    f.timeSince = Val(c, 520, kLife);
    f.brightness = Val(c, 521, kColor);
    f.hasVel = (Rand(c, 530) % 4) == 0;
    f.createLocal = (Rand(c, 531) % 3) == 0;
    f.mustCreate = Rand(c, 532) & 1;
    f.poolMode = (Rand(c, 533) % 10) < 7 ? 0 : (Rand(c, 534) & 1 ? 1 : 2);
    f.listMode = Rand(c, 535) & 1;
    f.flagsOther = Rand(c, 536) & 0xDF;
    f.fill = Rand(c, 537) % 4;
    // Semilla del rand() del EXE: fija 4 de cada 16 casos en los extremos de `% 10000` (0 y 9999) para la primera tirada; el resto, aleatoria.
    u32 seed = Rand(c, 540);
    if (c % 16 == 3 || c % 16 == 11) {
        const u32 want = (c % 16 == 3) ? 0u : 9999u;
        for (unsigned i = 0; i < 100000; ++i) {                      // busqueda determinista de una semilla cuya primera tirada da el resto deseado
            const u32 next = seed * 0x343FDu + 0x269EC3u;
            if (((next >> 16) & 0x7FFFu) % 10000u == want) break;
            seed = Mix(seed + i + 1);
        }
    }
    f.seed = seed;
    return f;
}

// ------------------------------------------------------------------------------------------------------------------------------ arena
inline void Put32(u8* base, std::size_t at, u32 v) { std::memcpy(base + at, &v, 4); }
inline u32 Get32(const u8* base, std::size_t at) { u32 v; std::memcpy(&v, base + at, 4); return v; }
inline u32 Ptr(const u8* base, std::size_t at) { return static_cast<u32>(reinterpret_cast<std::uintptr_t>(base + at)); }
inline void BuildArena(u8* base, const CpFx& f) {
    constexpr u8 fills[4] = {0x5C, 0xC3, 0xA5, 0x00};
    std::memset(base, fills[f.fill], kArena);
    Put32(base, kEmitter + 0, kVtable); Put32(base, kEmitter + 4, Ptr(base, kPrim)); Put32(base, kEmitter + 8, Ptr(base, kSystem));
    base[kEmitter + 0xC] = 1; Put32(base, kEmitter + 0x10, 0x3F000000);
    base[kSystem + 0x62] = static_cast<u8>(f.flagsOther | (f.mustCreate ? 0x20u : 0u));
    for (unsigned i = 0; i < 3; ++i) Put32(base, kSystem + 0x68 + 4 * i, f.velAdd[i]);
    for (unsigned i = 0; i < kEmissionWords; ++i) Put32(base, kEmission + 4 * i, f.emission[i]);
    for (unsigned i = 0; i < 7; ++i) Put32(base, kMult + 4 * i, f.mult[i]);
    for (unsigned i = 0; i < 16; ++i) Put32(base, kWld + 4 * i, f.wld[i]);
    for (unsigned i = 0; i < 3; ++i) Put32(base, kVel + 4 * i, f.velOverride[i]);
    if (f.listMode == 1) {                                                    // lista con un elemento previo: head = tail = elemento, cuenta 1, enlaces nulos
        Put32(base, kPrim + 0x20, Ptr(base, kItem)); Put32(base, kPrim + 0x24, Ptr(base, kItem)); Put32(base, kPrim + 0x28, 1);
        Put32(base, kItem + 0, 0); Put32(base, kItem + 4, 0);
    } else {
        Put32(base, kPrim + 0x20, 0); Put32(base, kPrim + 0x24, 0); Put32(base, kPrim + 0x28, 0);
    }
}

// ------------------------------------------------------------------------------------------------------------------------------ colaboradores sustituidos (trazados)
// Se instalan en ambos lados (EXE y DLL) con los mismos cuerpos. Los punteros se anotan como desplazamientos en el arena (0xFFFFFFFE si son locales de pila, 0xFFFFFFFF si nulos).
inline std::vector<u32> trace;
inline u8* traceBase{};
inline unsigned poolCalls{}, poolMode{};
inline u32 getRwWords[12]{};
inline u32 Off(const void* p) {
    if (!p) return 0xFFFFFFFFu;
    const auto* b = static_cast<const u8*>(p);
    return (b >= traceBase && b < traceBase + kArena) ? static_cast<u32>(b - traceBase) : 0xFFFFFFFEu;
}
inline u32 Fnv(const void* p, std::size_t n) { u32 h = 2166136261u; for (std::size_t i = 0; i < n; ++i) { h ^= static_cast<const u8*>(p)[i]; h *= 16777619u; } return h; }
inline u32 Bits(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
// Mitad exacta de un float por bits: infinitos y NaN se conservan (NaN silenciado), exponente >= 2 baja una unidad, el resto (cero, desnormal y exponente 1) queda en cero con su signo.
inline u32 HalfBits(u32 v) {
    const u32 e = (v >> 23) & 0xFF;
    if (e == 0xFF) return (v & 0x7FFFFF) ? (v | 0x400000u) : v;
    if (e >= 2) return v - 0x00800000u;
    return v & 0x80000000u;
}
inline void HalfVec(void* out, const void* in) {
    u32 w[3]; std::memcpy(w, in, 12);
    for (u32& x : w) x = HalfBits(x);
    std::memcpy(out, w, 12);
}
// GetParticle(0x4A93C0, thiscall, 1 argumento de pila, ret 4): guion segun poolMode; llamada n=0: A / null / null, n=1: A / B / null (tras FreeUpParticle).
inline void* __fastcall StubGetParticle(void*, void*, int primType) {
    trace.insert(trace.end(), {1u, static_cast<u32>(primType) & 0xFFu});
    const unsigned n = poolCalls++;
    int slot = -1;
    if (poolMode == 0) slot = n == 0 ? 0 : -1;
    else if (poolMode == 1) slot = n == 1 ? 1 : -1;
    return slot == 0 ? traceBase + kPartA : (slot == 1 ? traceBase + kPartB : nullptr);
}
// FreeUpParticle (0x4A9400, thiscall, sin argumentos)
inline void __fastcall StubFreeUpParticle(void*, void*) { trace.push_back(2u); }
// FxPrimBP_c::GetRWMatrix (0x4A9DC0, thiscall, 1 argumento, ret 4): escribe right, up, at y pos (12 floats; flags y rellenos intactos)
inline void __fastcall StubGetRWMatrix(void* self, void*, void* m) {
    trace.insert(trace.end(), {3u, Off(self), Off(m)});
    auto* w = static_cast<u32*>(m);
    for (unsigned i = 0; i < 3; ++i) { w[i] = getRwWords[i]; w[4 + i] = getRwWords[3 + i]; w[8 + i] = getRwWords[6 + i]; w[12 + i] = getRwWords[9 + i]; }
}
// RwV3dNormalize (0x7ED9B0, cdecl (out, in), devuelve la longitud en ST0)
inline float __cdecl StubNormalize(void* out, const void* in) {
    trace.insert(trace.end(), {4u, Off(out), Off(in), Fnv(in, 12), static_cast<u32>(out == in)});
    HalfVec(out, in);
    return 1.0f;
}
// RwV3dTransformVectors (0x7EDDF0, cdecl (out, in, n, matrix), devuelve out)
inline void* __cdecl StubTransform(void* out, const void* in, int n, const void* m) {
    trace.insert(trace.end(), {5u, Off(out), Off(in), static_cast<u32>(n), Off(m), Fnv(in, 12), Fnv(m, 0x40)});
    HalfVec(out, in);
    return out;
}
// RotateVecIntoVec (0x4A1660, cdecl (res, vec, align))
inline void __cdecl StubRotate(void* res, const void* vec, const void* align) {
    trace.insert(trace.end(), {6u, Off(res), Off(vec), Off(align), Fnv(vec, 12), Fnv(align, 12)});
    HalfVec(res, vec);
}
// FxEmitterBP_c::UpdateParticle (0x4A21D0, thiscall, 2 argumentos, ret 8): solo anota la particula tal como queda
inline bool __fastcall StubUpdateParticle(void* self, void*, float timeSince, void* prt) {
    trace.insert(trace.end(), {7u, Off(self), Bits(timeSince), Off(prt), Fnv(prt, 0x3C)});
    return false;
}
}  // namespace CpFix

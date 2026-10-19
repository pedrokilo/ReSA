#pragma once
// Fixtures del banco RTShadowSetup28: CRealTimeShadow::SetupForThisEntity (0x706520, thiscall, 1 argumento CPhysical* owner, `ret 4`, devuelve bool en AL).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp (y fixture_dump.cpp para calibrar/validar sin el juego).
// Todo valor float se construye por BITS (exponente/mantisa/signo), nunca con aritmetica float: el resultado no depende de las opciones de compilacion.
//
// 5120 casos = 8 grupos x 640. Grupo = n % 8; familia = (n / 8) % 20 (32 repeticiones de cada familia por grupo).
//   g0 owner->m_pRwObject == nullptr (el EXE devuelve false tras guardar el dueno)
//   g1 objeto con tipo 0                      g2 objeto con tipo 3..255 (Destroy + false)
//   g3 atomo con el bit 1 de +0x4C a cero      g4 atomo con el bit 1 de +0x4C a uno (resincroniza la esfera)
//   g5 clump                                   g6 atomo con +0x4C aleatorio          g7 clump (misma familia de esferas que g6)
// La familia elige la esfera (4 dwords en objeto+0x1C): 0 normal, 1 ceros, 2 radio negativo, 3 radio +-0, 4/5 radio +-inf, 6 radio qNaN, 7 radio cerca del desbordamiento,
// 8 radio denormal (SOLO los vetados: x87 24 bits y SSE dan lo mismo), 9 radios normales minimos, 10 centro NaN/inf (incl. sNaN: se copia en bruto), 11 centro denormal/-0,
// 12 cuatro dwords aleatorios (radio sin sNaN), 13 radio potencia de dos, 14 radio con mantisa llena, 15 radio exponentes medios, 16 centros enormes, 17 radio enorme,
// 18 radio 1.0, 19 mezcla de las anteriores.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace RTShadowFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kGroups = 8;
constexpr unsigned kPerGroup = 640;
constexpr unsigned kCases = kGroups * kPerGroup;   // 5120

// ------------------------------------------------------------------------------------------------ disposicion de memoria
constexpr u32 kGuard = 0x40;
constexpr u32 kShadowAt = kGuard, kShadowBytes = 0x4C;                          // CRealTimeShadow (sizeof == 0x4C)
constexpr u32 kShadowArena = kGuard + 0x50 + kGuard;
constexpr u32 kOwnerAt = kGuard, kOwnerBytes = 0x100;                           // CPhysical falso: solo se lee +0x18
constexpr u32 kOwnerArena = kGuard + kOwnerBytes + kGuard;
constexpr u32 kObjAt = kGuard, kObjBytes = 0x80;                                // RpAtomic / RpClump falso (tipo en +0, padre en +4, esfera en +0x1C, banderas en +0x4C)
constexpr u32 kObjArena = kGuard + kObjBytes + kGuard;
constexpr u32 kFrameAt = kGuard, kFrameBytes = 0x60, kNumFrames = 3;            // RwFrame falso: matriz de modelado en +0x10, posicion en +0x10+0x30
constexpr u32 kFrameArena = kGuard + kNumFrames * kFrameBytes + kGuard;
constexpr u32 kBogusParent = 0x00C0FFE0u;                                       // padre invalido: la funcion solo calcula padre+0x10 y el falso no lo desreferencia
constexpr u32 kMulBits = 0x3F8CCCCDu;                                           // 1,1f del EXE (0x858F14)

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 0x5A17u); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline constexpr std::array<u32, 8> kRegSeeds{0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, 0xDEADBEEFu, 0x00706520u, 0x000000FFu};

struct Gen {
    u32 c{}, k{};
    explicit Gen(u32 seed) : c(seed) {}
    u32 operator()(u32 m) { return m ? Rand(c, k++) % m : (++k, 0u); }
    u32 Raw() { return Rand(c, k++); }
    bool One(u32 den) { return (*this)(den) == 0; }
};

// Radios denormales vetados (el oraculo comprueba que x87 a 24 bits y SSE de precision simple coinciden): magnitudes de bits.
inline constexpr std::array<u32, 8> kDenormalOk{0x00000001u, 0x00000002u, 0x00000003u, 0x00400000u, 0x007FFFFFu, 0x007FFFFEu, 0x00200000u, 0x00100001u};
// Radios cerca del desbordamiento (x1,1 -> inf o FLT_MAX): FLT_MAX, vecinos y el limite FLT_MAX/1,1.
inline constexpr std::array<u32, 10> kNearOverflow{0x7F7FFFFFu, 0x7F7FFFFEu, 0x7F68BA2Du, 0x7F68BA2Eu, 0x7F68BA2Cu, 0x7F000000u, 0x7F6A0000u, 0x7F7FFFFDu, 0x7F68BA30u, 0x7F400000u};
inline constexpr std::array<u32, 12> kSpecialBits{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F8CCCCDu, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0x00800000u, 0x80800000u};

inline u32 Float(Gen& g, u32 expLo, u32 expHi, bool anySign) {                // exponente en [expLo, expHi], mantisa aleatoria
    const u32 e = expLo + g(expHi - expLo + 1);
    return ((anySign && g.One(2)) ? 0x80000000u : 0u) | (e << 23) | (g.Raw() & 0x7FFFFFu);
}
inline u32 NoSNan(u32 b) { return ((b >> 23 & 0xFF) == 0xFF && (b & 0x7FFFFF) != 0) ? (b | 0x400000u) : b; }   // sNaN -> qNaN (solo el radio)

struct Fx {
    unsigned n{};
    int group{}, family{};
    bool objPresent{};
    std::array<u8, kObjBytes> object{};         // bytes 4..7 (padre) a cero: el banco escribe el puntero real
    u32 parentLabel{};                          // 0 nulo, 1..3 marco de la arena, 4 invalido
    std::array<u32, 12> framePos{};             // posicion de cada marco (3 dwords)
    std::array<u8, kShadowBytes> pre{};
    u32 ownerSeed{}, guardSeed{};
    u32 eax{}, edx{}, stack{};
    u32 sphere[4]{};
};

inline void PutU32(u8* p, u32 v) { std::memcpy(p, &v, 4); }

inline void MakeSphere(Gen& g, int family, u32 s[4]) {
    auto normalXYZ = [&] { for (int i = 0; i < 3; ++i) s[i] = Float(g, 100, 140, true); };
    switch (family) {
    case 0:  normalXYZ(); s[3] = Float(g, 110, 140, false); break;
    case 1:  s[0] = s[1] = s[2] = s[3] = 0; break;
    case 2:  normalXYZ(); s[3] = 0x80000000u | Float(g, 100, 140, false); break;
    case 3:  normalXYZ(); s[3] = g.One(2) ? 0u : 0x80000000u; break;
    case 4:  normalXYZ(); s[3] = 0x7F800000u; break;
    case 5:  normalXYZ(); s[3] = 0xFF800000u; break;
    case 6:  normalXYZ(); s[3] = (g.One(2) ? 0x80000000u : 0u) | 0x7FC00000u | (g.Raw() & 0x3FFFFFu); break;
    case 7:  normalXYZ(); s[3] = kNearOverflow[g(u32(kNearOverflow.size()))] | (g.One(4) ? 0x80000000u : 0u); break;
    case 8:  normalXYZ(); s[3] = kDenormalOk[g(u32(kDenormalOk.size()))] | (g.One(2) ? 0x80000000u : 0u); break;
    case 9:  normalXYZ(); s[3] = Float(g, 1, 3, true); break;
    case 10: {
        for (int i = 0; i < 3; ++i) {
            const u32 pick = g(5);
            s[i] = pick == 0 ? 0x7FC00000u : pick == 1 ? 0x7F800000u : pick == 2 ? 0xFF800000u : pick == 3 ? (0x7F800000u | (g.Raw() & 0x7FFFFFu) | 1u) : Float(g, 100, 140, true);
        }
        s[3] = Float(g, 110, 140, false);
        break;
    }
    case 11: {
        for (int i = 0; i < 3; ++i) { const u32 pick = g(3); s[i] = pick == 0 ? 0x80000000u : pick == 1 ? ((g.Raw() & 0x7FFFFFu) | (g.One(2) ? 0x80000000u : 0u)) : 0u; }
        s[3] = Float(g, 110, 140, false);
        break;
    }
    case 12: for (int i = 0; i < 3; ++i) s[i] = g.Raw(); s[3] = NoSNan(g.Raw()); break;
    case 13: normalXYZ(); s[3] = ((1u + g(253)) << 23) | (g.One(4) ? 0x80000000u : 0u); break;
    case 14: normalXYZ(); s[3] = ((g.One(4) ? 0x80000000u : 0u) | ((1u + g(253)) << 23) | 0x7FFFFFu); break;
    case 15: normalXYZ(); s[3] = Float(g, 100, 160, true); break;
    case 16: for (int i = 0; i < 3; ++i) s[i] = Float(g, 200, 254, true); s[3] = Float(g, 110, 140, false); break;
    case 17: normalXYZ(); s[3] = Float(g, 150, 253, g.One(8)); break;
    case 18: normalXYZ(); s[3] = 0x3F800000u; break;
    default: MakeSphere(g, int(g(19)), s); break;
    }
}

inline Fx Make(unsigned n) {
    Fx f;
    f.n = n;
    f.group = int(n % kGroups);
    f.family = int((n / kGroups) % 20);
    Gen g(n * 2654435761u + 0x1234u);
    f.objPresent = f.group != 0;
    FillWords(f.object.data(), f.object.size(), n * 40503u + 7u);
    const u32 t = g(8);
    // tipo
    u8 type = 1;
    switch (f.group) {
    case 0: type = u8(g.Raw()); break;                   // sin objeto: no se mira
    case 1: type = 0; break;
    case 2: {
        static constexpr u8 kT[] = {3, 4, 5, 7, 8, 15, 16, 0x7F, 0x80, 0xFE, 0xFF, 0x81, 0x41, 0x0A};
        type = (t < 4 && g.One(2)) ? u8(3 + g(253)) : kT[g(u32(sizeof kT))];
        break;
    }
    case 3: case 4: case 6: type = 1; break;
    default: type = 2; break;
    }
    f.object[0] = type;
    PutU32(f.object.data() + 4, 0);
    MakeSphere(g, f.family, f.sphere);
    u32 flags = g.Raw();
    switch (f.group) {
    case 3: flags &= ~2u; if (f.family % 4 == 0) flags = 0; else if (f.family % 4 == 1) flags = 0xFFFFFFFDu; else if (f.family % 4 == 3) flags &= 0xFFFFFF00u; break;
    case 4: flags |= 2u;  if (f.family % 4 == 0) flags = 2; else if (f.family % 4 == 1) flags = 0xFFFFFFFFu; else if (f.family % 4 == 3) flags = (flags & 0xFFFFFF00u) | 2u; break;
    default: break;
    }
    PutU32(f.object.data() + 0x4C, flags);
    // La esfera deseada es la que llega a la sombra: los falsos de resincronizacion (atomo con el bit 1) y de RpClumpGetBoundingSphere (clump) la mezclan con XOR,
    // asi que se guarda ya mezclada para que los valores especiales (NaN, inf, denormales...) lleguen intactos a la multiplicacion por 1,1f.
    for (int i = 0; i < 4; ++i) {
        u32 w = f.sphere[i];
        if (type == 1 && (flags & 2u)) w ^= 0x13579BDFu + u32(i) * 0x01010101u;
        else if (type == 2) w ^= 0x2468ACE0u + u32(i) * 0x00FF00FFu;
        PutU32(f.object.data() + 0x1C + 4 * i, w);
    }
    // padre
    const u32 p = g(10);
    f.parentLabel = p == 0 ? 0u : p == 1 ? 4u : 1u + g(kNumFrames);
    for (auto& v : f.framePos) v = g.One(8) ? Float(g, 0, 255, true) : Float(g, 100, 140, true);
    // sombra previa (los bytes que la funcion no escribe deben conservarse)
    switch (f.family % 5) {
    case 0: FillWords(f.pre.data(), f.pre.size(), n * 2246822519u + 3u); break;
    case 1: f.pre.fill(0x00); break;
    case 2: f.pre.fill(0xFF); break;
    case 3: f.pre.fill(u8(0x11 + 0x13 * (n % 13))); break;
    default: for (std::size_t i = 0; i < f.pre.size(); ++i) f.pre[i] = u8(i * 7 + n); break;
    }
    f.ownerSeed = n * 668265263u + 11u;
    f.guardSeed = n * 374761393u + 17u;
    f.eax = g.One(2) ? kRegSeeds[g(8)] : g.Raw();
    f.edx = g.One(2) ? kRegSeeds[g(8)] : g.Raw();
    f.stack = kStackSeeds[g(8)];
    return f;
}
}  // namespace RTShadowFix

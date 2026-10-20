#pragma once
// Fixtures del banco WaterLevel62 (lote 62): cinco funciones de CWaterLevel, 4096 casos cada una. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp y un programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
//   CWaterLevel::FillQuadsAndTrianglesList       0x6E7B30  cdecl, void, sin argumentos
//   CWaterLevel::GetWaterDepth                   0x6EA960  cdecl (const CVector*, float*, float*, float*), bool en AL
//   CWaterLevel::RenderHighDetailWaterRectangle  0x6EB810  cdecl (minX, maxX, Y1, Y2, CRenPar x4 por valor), void
//   CWaterLevel::RenderHighDetailWaterTriangle   0x6EDDC0  cdecl (X1, Y1, P1, X2, Y2, P2, X3, Y3, P3), void
//   CWaterLevel::TestLineAgainstWater            0x6E61B0  cdecl (CVector from, CVector to, CVector* out) por valor, bool en AL
//
// Todos los floats viajan como patrones de 32 bits sin signo. Politica de especiales (para que el oraculo no dependa de la regla de x87 con dos NaN):
// a lo sumo UN campo NaN con carga util propia por caso.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace Water62Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4096;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

// ------------------------------------------------------------------------------------------------ JSON minimo (mismas filas `input` en el juego y en el espejo)
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& A(const char* k, const std::vector<u32>& v) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < v.size(); ++i) { if (i) s += ','; s += std::to_string(v[i]); }
        s += ']'; return *this;
    }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ floats
// NaN con carga util propia por casilla (todas con significando distinto y nunca igual al indefinido 0xFFC00000): qNaN o sNaN.
constexpr u32 NanBits(u32 slot, bool signaling) {
    const u32 sign = (slot & 1u) << 31;
    return signaling ? (sign | 0x7F800000u | (0x2000u + slot * 0x41u)) : (sign | 0x7FC00000u | (0x100u + slot * 0x37u));
}
inline constexpr std::array<u32, 20> kFloatEdges{
    0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3DCCCCCDu, 0xBDCCCCCDu, 0x3D4CCCCDu, 0x00000001u, 0x80000001u, 0x007FFFFFu,
    0x807FFFFFu, 0x00800000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x3F000000u, 0x40000000u, 0x3E000000u, 0x45000000u, 0x00400000u, 0x80400000u};
inline u32 OrdinaryFloat(u32 n, u32 k, int minExp, int maxExp) {   // exponentes sesgados (127 = 1.0)
    const u32 sign = Rand(n, k) & 1u;
    const u32 exp = u32(minExp) + Rand(n, k + 1) % u32(maxExp - minExp + 1);
    return (sign << 31) | (exp << 23) | (Rand(n, k + 2) & 0x7FFFFFu);
}
inline u32 TinyFloat(u32 n, u32 k) {   // denormal o casi
    const u32 sign = Rand(n, k) & 1u;
    const u32 exp = Rand(n, k + 1) % 3 == 0 ? Rand(n, k + 3) % 6u : 0u;
    return (sign << 31) | (exp << 23) | (Rand(n, k + 2) & 0x7FFFFFu);
}
inline u32 HugeFloat(u32 n, u32 k) {
    const u32 sign = Rand(n, k) & 1u;
    return (sign << 31) | ((200u + Rand(n, k + 1) % 55u) << 23) | (Rand(n, k + 2) & 0x7FFFFFu);   // hasta 0x7F7FFFFF
}
inline u32 InfBits(u32 n, u32 k) { return Rand(n, k) & 1u ? 0xFF800000u : 0x7F800000u; }
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
inline bool IsInfBits(u32 b) { return (b & 0x7FFFFFFFu) == 0x7F800000u; }
// Cualquier patron de 32 bits razonable (para funciones que solo copian los floats)
inline u32 AnyFloat(u32 n, u32 k, u32 slot) {
    const u32 sel = Rand(n, k) % 100;
    if (sel < 45) return OrdinaryFloat(n, k + 10, 100, 150);
    if (sel < 65) return kFloatEdges[Rand(n, k + 11) % kFloatEdges.size()];
    if (sel < 72) return TinyFloat(n, k + 12);
    if (sel < 78) return HugeFloat(n, k + 13);
    if (sel < 84) return InfBits(n, k + 14);
    if (sel < 92) return NanBits(slot + Rand(n, k + 15) % 5u, false);
    if (sel < 96) return NanBits(slot + Rand(n, k + 15) % 5u, true);
    return Rand(n, k + 16);
}
inline float F(u32 b) { float f; std::memcpy(&f, &b, 4); return f; }
inline u32 B(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }

// ------------------------------------------------------------------------------------------------ atlas de agua sintetico (entradas de FillQuadsAndTrianglesList y TestLineAgainstWater)
// m_aVertices (1021 x 20 bytes: x, y int16 + z, bigWaves, smallWaves float + flowX, flowY + 2 de relleno), WaterQuads (301 x 10 bytes: 4 indices + banderas),
// WaterTriangles (6 x 8 bytes: 3 indices + banderas), m_BlockPolyInfo (144 palabras) y m_PolyCombos (700 palabras). Tamanos del juego:
constexpr u32 kVertexBytes = 20, kVertexCount = 1021, kQuadBytes = 10, kQuadCount = 301, kTriBytes = 8, kTriCount = 6, kBlockWords = 144, kComboWords = 700;
constexpr u32 kMaxPool = 96, kMaxQuads = 40;
// Coordenada de vertice: modo 0 aleatoria, 1 junto a los bordes de bloque (-3000 + 500 j +- 2), 2 extremos de int16 y cercanias de los bordes del mundo.
inline i32 Coord(u32 n, u32 k, u32 mode) {
    if (mode == 0) return i32(Rand(n, k) % 7001u) - 3500;
    if (mode == 1) return -3000 + 500 * i32(Rand(n, k) % 13u) + i32(Rand(n, k + 1) % 5u) - 2;
    static constexpr i32 ext[]{-32768, -32767, -3001, -3000, -2999, 0, 2999, 3000, 3001, 32766, 32767, -1, 1, 499, 500, -500};
    return ext[Rand(n, k) % std::size(ext)];
}

// ================================================================================================ 0: FillQuadsAndTrianglesList
struct A0 {
    u32 family{};
    std::vector<u32> vx, vy;                 // vertices (int16 como u32 de 16 bits)
    std::vector<u32> quads;                  // 5 palabras por cuadrilatero: 4 indices + banderas
    std::vector<u32> tris;                   // 4 palabras por triangulo: 3 indices + banderas
    u32 vm1x{}, vm1y{};                      // el vertice -1 (indice 0xFFFF extendido con signo)
    u32 numQ{}, numT{};                      // contadores tal como estan en memoria (u32 sin tratar)
    u32 trigKind{}, trigAt{}, trigVal{};     // el falso cambia NumWaterQuads (1) o NumWaterTriangles (2) en la llamada trigAt (1..)
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline A0 MakeA0(u32 n) {
    A0 c;
    const u32 f = n % 16;
    c.family = f;
    const u32 pool = 6 + Rand(n, 1) % 40u;
    const u32 modeSel = f % 4;
    for (u32 v = 0; v < pool; ++v) {
        const u32 mode = modeSel == 0 ? 0u : modeSel == 1 ? 1u : modeSel == 2 ? 2u : Rand(n, 100 + v) % 3u;
        c.vx.push_back(u32(Coord(n, 200 + 4 * v, Rand(n, 300 + v) % 4 == 0 ? Rand(n, 400 + v) % 3u : mode)) & 0xFFFFu);
        c.vy.push_back(u32(Coord(n, 600 + 4 * v, Rand(n, 700 + v) % 4 == 0 ? Rand(n, 800 + v) % 3u : mode)) & 0xFFFFu);
    }
    const u32 q = f < 4 ? Rand(n, 2) % 4u : f < 8 ? 1 + Rand(n, 2) % 8u : f < 12 ? 5 + Rand(n, 2) % 20u : (Rand(n, 3) % 4 == 0 ? 0u : Rand(n, 2) % 25u);
    const u32 t = Rand(n, 4) % 8 == 0 ? 0u : Rand(n, 5) % 7u;
    const bool useMinus1 = f % 8 == 7;
    auto index = [&](u32 k) -> u32 { return useMinus1 && Rand(n, k) % 5 == 0 ? 0xFFFFu : Rand(n, k + 1) % pool; };
    for (u32 i = 0; i < q; ++i) {
        for (u32 j = 0; j < 4; ++j) c.quads.push_back(index(1000 + 16 * i + 3 * j));
        c.quads.push_back(Rand(n, 1900 + i) & 0xFFFFu);
    }
    for (u32 i = 0; i < t; ++i) {
        for (u32 j = 0; j < 3; ++j) c.tris.push_back(index(2000 + 16 * i + 3 * j));
        c.tris.push_back(Rand(n, 2900 + i) & 0xFFFFu);
    }
    c.vm1x = u32(Coord(n, 3000, Rand(n, 3001) % 3u)) & 0xFFFFu;
    c.vm1y = u32(Coord(n, 3002, Rand(n, 3003) % 3u)) & 0xFFFFu;
    c.numQ = q;
    c.numT = t;
    if (f == 3) c.numQ = 0;
    if (f == 6 && q) c.numQ = 0x80000000u | (Rand(n, 3010) & 0x7FFFFFFFu);
    if (f == 5) c.numT = 0;
    if (f == 9 && t) c.numT = 0x80000000u | (Rand(n, 3011) & 0x7FFFFFFFu);
    if (f % 4 == 2 && Rand(n, 3020) % 4 != 0) {
        c.trigKind = 1 + Rand(n, 3021) % 2;
        c.trigAt = 1 + Rand(n, 3022) % 6;
        if (c.trigKind == 1) {
            const u32 pick = Rand(n, 3023) % 5;
            c.trigVal = pick == 0 ? 0u : pick == 1 ? 0xFFFFFFFFu : pick == 2 ? q : pick == 3 ? q / 2 : (q ? Rand(n, 3024) % (q + 1) : 0u);
        } else {
            const u32 pick = Rand(n, 3025) % 4;
            c.trigVal = pick == 0 ? 0u : pick == 1 ? 0x80000000u : t ? Rand(n, 3026) % (t + 1) : 0u;
        }
    }
    c.eax = Rand(n, 70); c.ecx = Rand(n, 73); c.edx = Rand(n, 71); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonA0(const A0& c) {
    Js j;
    j.U("family", c.family).A("vx", c.vx).A("vy", c.vy).A("quads", c.quads).A("tris", c.tris).U("vm1x", c.vm1x).U("vm1y", c.vm1y)
     .U("num_q", c.numQ).U("num_t", c.numT).U("trig_kind", c.trigKind).U("trig_at", c.trigAt).U("trig_val", c.trigVal)
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}

// ================================================================================================ 1: GetWaterDepth
// Arena (guarda | objeto | guarda): 0x40 + 0x40 + 0x40 = 0xC0. En el objeto: vecPos (3 floats) en +0x00..+0x0B, salidas por defecto en +0x10 (profundidad), +0x14 (nivel), +0x18 (suelo).
constexpr u32 kDepthObj = 0x40, kDepthObjBytes = 0x40, kDepthArena = 0xC0;
constexpr u32 kNullOut = 0xFFFFFFFFu;
struct B1 {
    u32 family{}, mode{}, retA{}, retG{}, writeLevel{};
    u32 pos[3]{};
    u32 level{}, ground{};
    u32 out[3]{};      // desplazamientos dentro del objeto de profundidad, nivel y suelo (kNullOut = nulo)
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline B1 MakeB1(u32 n) {
    B1 c;
    const u32 f = n % 16;
    c.family = f;
    c.mode = (n / 16) % 8;   // disposicion de las salidas
    for (u32 i = 0; i < 3; ++i) c.pos[i] = AnyFloat(n, 10 + 8 * i, i);
    // retorno del colaborador: AL = 0 (con bytes altos) o distinto de cero no canonico
    auto retVal = [&](u32 k, u32 falseOneIn) {
        const bool isFalse = Rand(n, k) % falseOneIn == 0;
        static constexpr u32 als[]{1u, 2u, 0x80u, 0xFFu, 0x7Fu, 0x81u};
        if (isFalse) return Rand(n, k + 1) % 2 ? 0u : (Rand(n, k + 2) & 0xFFFFFF00u);
        return (Rand(n, k + 3) % 2 ? 0u : (Rand(n, k + 4) & 0xFFFFFF00u)) | als[Rand(n, k + 5) % std::size(als)];
    };
    c.retA = retVal(30, 4);
    c.retG = retVal(40, 3);
    c.writeLevel = Rand(n, 50) % 16 == 0 ? 0u : 1u;
    // valores de nivel y suelo: a lo sumo un NaN
    const u32 m = (f + Rand(n, 51)) % 12;
    c.level = OrdinaryFloat(n, 60, 100, 150);
    c.ground = OrdinaryFloat(n, 64, 100, 150);
    switch (m) {
    case 4: c.level = kFloatEdges[Rand(n, 68) % kFloatEdges.size()]; break;
    case 5: c.ground = kFloatEdges[Rand(n, 69) % kFloatEdges.size()]; break;
    case 6: c.level = kFloatEdges[Rand(n, 68) % kFloatEdges.size()]; c.ground = kFloatEdges[Rand(n, 69) % kFloatEdges.size()]; break;
    case 7: c.level = TinyFloat(n, 70); c.ground = Rand(n, 72) % 2 ? TinyFloat(n, 73) : OrdinaryFloat(n, 74, 1, 20); break;
    case 8: c.level = HugeFloat(n, 75); c.ground = Rand(n, 77) % 2 ? HugeFloat(n, 78) : OrdinaryFloat(n, 79, 100, 150); break;
    case 9: if (Rand(n, 80) % 2) c.level = InfBits(n, 81); else c.ground = InfBits(n, 82); if (Rand(n, 83) % 3 == 0) { c.level = InfBits(n, 84); c.ground = InfBits(n, 85); } break;
    case 10: if (Rand(n, 86) % 2) c.level = NanBits(Rand(n, 87) % 6, Rand(n, 88) % 3 == 0); else c.ground = NanBits(Rand(n, 89) % 6, Rand(n, 90) % 3 == 0); break;
    case 11: c.level = Rand(n, 91) % 2 ? 0x80000000u : 0u; c.ground = Rand(n, 92) % 2 ? 0x80000000u : 0u; break;
    default: break;
    }
    // salidas: 0 distintas, 1 patron de nulos, 2 profundidad = nivel, 3 nivel = suelo, 4 profundidad = suelo, 5 las tres iguales, 6 dentro de vecPos, 7 mezcla
    static constexpr u32 std3[3]{0x10u, 0x14u, 0x18u};
    for (u32 i = 0; i < 3; ++i) c.out[i] = std3[i];
    switch (c.mode) {
    case 1: { const u32 mask = Rand(n, 93) % 8; for (u32 i = 0; i < 3; ++i) if (!(mask >> i & 1)) c.out[i] = kNullOut; break; }
    case 2: c.out[1] = c.out[0]; break;
    case 3: c.out[2] = c.out[1]; break;
    case 4: c.out[2] = c.out[0]; break;
    case 5: c.out[1] = c.out[0]; c.out[2] = c.out[0]; break;
    case 6: for (u32 i = 0; i < 3; ++i) c.out[i] = 4u * (Rand(n, 94 + i) % 3); break;
    case 7: for (u32 i = 0; i < 3; ++i) { const u32 r = Rand(n, 97 + i) % 8; c.out[i] = r == 7 ? kNullOut : r < 3 ? std3[r] : r < 6 ? 4u * (r - 3) : 0x1Cu; } break;
    default: break;
    }
    c.eax = Rand(n, 70); c.ecx = Rand(n, 73); c.edx = Rand(n, 71); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonB1(const B1& c) {
    Js j;
    j.U("family", c.family).U("mode", c.mode).U("ret_a", c.retA).U("ret_g", c.retG).U("write_level", c.writeLevel)
     .A("pos", {c.pos[0], c.pos[1], c.pos[2]}).U("level", c.level).U("ground", c.ground).A("out", {c.out[0], c.out[1], c.out[2]})
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}

// ================================================================================================ 2 y 3: RenderHighDetailWaterRectangle / Triangle
// CRenPar: z, bigWaves, smallWaves (floats) + flowX, flowY (int8) + 2 bytes de relleno (no se comparan). En JSON: 4 palabras por CRenPar: z, big, small, flowX | flowY << 8.
struct Ren { u32 z{}, big{}, sm{}, flow{}; };
constexpr u32 kMirrorEdges[]{0u, 1u, 2u, 0x80u, 0xFFu};
// Ruta de la funcion rectangulo segun los enteros (0 pequeno -> dos capas, 1 parte en X, 2 parte en Y), como el EXE (envoltura de 32 bits).
inline int RectPath(i32 minX, i32 maxX, i32 y1, i32 y2) {
    const i32 minY = y1 < y2 ? y1 : y2, maxY = y1 < y2 ? y2 : y1;
    const i32 hx = i32(u32(maxX) - u32(minX)) / 2, hy = i32(u32(maxY) - u32(minY)) / 2;
    const i32 twice = i32(u32(hy) * u32(hx) * 2u), tri = i32(u32(hy + 1) * u32(hx + 1));
    if (i32(u32(twice) * 3u) < 0x1000 && tri < 0x800) return 0;
    return hx > hy ? 1 : 2;
}
inline int TriPath(i32 x1, i32 x2) {
    const i32 hw = i32(u32(x2) - u32(x1)) / 2;
    const i32 sq = i32(u32(hw) * u32(hw));
    const i32 n = i32(u32(hw) + 1u);
    const i32 tri = n >= 1 ? i32(u32((std::uint64_t(n) * (std::uint64_t(n) + 1u)) / 2u)) : 0;
    return (i32(u32(sq) * 3u) < 0x1000 && tri < 0x800) ? 0 : 1;
}
struct C2 {
    u32 family{}, path{}, mirror{}, ret1{}, ret2{};
    i32 ints[4]{};   // minX, maxX, Y1, Y2
    Ren p[4]{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
// Cuatro valores de CRenPar: ordinarios (el rango estrecho evita los denormales que el lerp en SSE vaciaria) o cualquiera, a lo sumo un NaN
inline void FillRen(u32 n, Ren* p, u32 count, bool narrow, u32 base) {
    const u32 nanAt = Rand(n, base) % 24 == 0 ? Rand(n, base + 1) % (count * 3) : 0xFFFFu;
    for (u32 i = 0; i < count; ++i) {
        u32* w[3]{&p[i].z, &p[i].big, &p[i].sm};
        for (u32 k = 0; k < 3; ++k) {
            if (narrow) {
                *w[k] = Rand(n, base + 10 + 4 * (i * 3 + k)) % 6 == 0 ? (Rand(n, base + 11 + 4 * (i * 3 + k)) % 2 ? 0x00000000u : 0x80000000u) : OrdinaryFloat(n, base + 12 + 4 * (i * 3 + k), 100, 150);
            } else {
                u32 v = AnyFloat(n, base + 12 + 4 * (i * 3 + k), i * 3 + k);
                if (IsNanBits(v) || IsInfBits(v)) v = OrdinaryFloat(n, base + 13 + 4 * (i * 3 + k), 110, 140);
                if (Rand(n, base + 14 + 4 * (i * 3 + k)) % 16 == 0) v = InfBits(n, base + 15 + 4 * (i * 3 + k));
                *w[k] = v;
            }
        }
        if (nanAt / 3 == i && !narrow) *w[nanAt % 3] = NanBits(i * 3 + nanAt % 3, Rand(n, base + 2) % 3 == 0);
        p[i].flow = (Rand(n, base + 3 + i) & 0xFFFFu);
    }
}
inline void PickCallResults(u32 n, u32 base, u32& mirror, u32& ret1, u32& ret2) {
    auto retVal = [&](u32 k, u32 falseOneIn) {
        const bool isFalse = Rand(n, k) % falseOneIn == 0;
        static constexpr u32 als[]{1u, 2u, 0x80u, 0xFFu, 0x7Fu, 0x81u};
        if (isFalse) return Rand(n, k + 1) % 2 ? 0u : (Rand(n, k + 2) & 0xFFFFFF00u);
        return (Rand(n, k + 3) % 2 ? 0u : (Rand(n, k + 4) & 0xFFFFFF00u)) | als[Rand(n, k + 5) % std::size(als)];
    };
    mirror = Rand(n, base) % 3 == 0 ? 0u : kMirrorEdges[Rand(n, base + 1) % std::size(kMirrorEdges)];
    ret1 = retVal(base + 10, 5);
    ret2 = retVal(base + 20, 3);
}
inline C2 MakeC2(u32 n) {
    C2 c;
    const u32 f = n % 16;
    c.family = f;
    PickCallResults(n, 400, c.mirror, c.ret1, c.ret2);
    if (f == 12) c.ret1 = Rand(n, 410) % 2 ? 0u : (Rand(n, 411) & 0xFFFFFF00u);   // invisible: primera llamada falsa y espejo / segunda tambien
    if (f == 12 && (Rand(n, 412) % 2 || c.mirror == 0)) c.mirror = 0, c.ret2 = 0;
    i32 hx = 0, hy = 0;
    const u32 kind = f < 4 ? 0 : f < 6 ? 1 : f < 9 ? 2 : f < 12 ? 3 : f == 12 ? Rand(n, 1) % 4 : 4;
    switch (kind) {
    case 0: hx = Rand(n, 2) % 41; hy = Rand(n, 3) % (4096 / (6 * (hx > 0 ? hx : 1)) + 1); while ((hx + 1) * (hy + 1) >= 2048 || 6 * hx * hy >= 4096) --hy; if (hy < 0) hy = 0; break;
    case 1: {   // junto a los umbrales 6 hx hy = 0x1000 y (hx + 1)(hy + 1) = 0x800
        if (Rand(n, 2) % 2) { hx = 1 + Rand(n, 3) % 60; const i32 base = 4096 / (6 * hx); hy = base + i32(Rand(n, 4) % 3) - 1; if (hy < 0) hy = 0; }
        else { hx = Rand(n, 3) % 60; const i32 base = 2048 / (hx + 1); hy = base + i32(Rand(n, 4) % 3) - 1; if (hy < 0) hy = 0; }
        break;
    }
    case 2: hx = 40 + Rand(n, 2) % 400; hy = Rand(n, 3) % 41; if (Rand(n, 4) % 4 == 0) hy = hx - Rand(n, 5) % 3; break;   // ancho: parte en X (tambien hx == hy + 1)
    case 3: hy = 40 + Rand(n, 2) % 400; hx = Rand(n, 3) % 41; if (Rand(n, 4) % 4 == 0) hx = hy + i32(Rand(n, 5) % 2); break;   // alto: parte en Y (tambien hx == hy)
    default: hx = Rand(n, 2) % 300; hy = Rand(n, 3) % 300; break;
    }
    const i32 dx = 2 * hx + i32(Rand(n, 6) % 2), dy = 2 * hy + i32(Rand(n, 7) % 2);
    i32 minX = Coord(n, 8, Rand(n, 9) % 2) * (Rand(n, 10) % 5 == 0 ? 8 : 1), minY = Coord(n, 11, Rand(n, 12) % 2) * (Rand(n, 13) % 5 == 0 ? 8 : 1);
    i32 maxX = minX + dx, y1 = minY, y2 = minY + dy;
    if (Rand(n, 14) % 2) std::swap(y1, y2);
    if (f == 13) {   // ancho / alto negativos o cero (maxX < minX, Y1 > Y2 ya cubierto)
        const u32 s = Rand(n, 15) % 3;
        if (s == 0) maxX = minX - dx; else if (s == 1) maxX = minX; else { maxX = minX - 1; }
        if (Rand(n, 16) % 3 == 0) y2 = y1;
    }
    if (f == 14) {   // extremos de 32 bits (envoltura)
        static constexpr i32 e32[]{INT32_MIN, INT32_MIN + 1, INT32_MAX, INT32_MAX - 1, 0, -1, 1, 0x40000000, -0x40000000, 0x7FFFFFFE, -0x7FFFFFFF};
        i32* t[4]{&minX, &maxX, &y1, &y2};
        const u32 cnt = 1 + Rand(n, 17) % 3;
        for (u32 k = 0; k < cnt; ++k) *t[Rand(n, 18 + k) % 4] = e32[Rand(n, 22 + k) % std::size(e32)];
    }
    c.ints[0] = minX; c.ints[1] = maxX; c.ints[2] = y1; c.ints[3] = y2;
    c.path = u32(RectPath(minX, maxX, y1, y2));
    const bool visible = (c.ret1 & 0xFFu) || ((c.mirror & 0xFFu) && (c.ret2 & 0xFFu));
    const bool narrow = visible && c.path == 2;
    FillRen(n, c.p, 4, narrow, 500);
    c.eax = Rand(n, 70); c.ecx = Rand(n, 73); c.edx = Rand(n, 71); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC2(const C2& c) {
    Js j;
    std::vector<u32> p;
    for (const Ren& r : c.p) { p.push_back(r.z); p.push_back(r.big); p.push_back(r.sm); p.push_back(r.flow); }
    j.U("family", c.family).U("path", c.path).U("mirror", c.mirror).U("ret1", c.ret1).U("ret2", c.ret2)
     .A("ints", {u32(c.ints[0]), u32(c.ints[1]), u32(c.ints[2]), u32(c.ints[3])}).A("p", p)
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}
struct C3 {
    u32 family{}, path{}, mirror{}, ret1{}, ret2{};
    i32 ints[6]{};   // X1, Y1, X2, Y2, X3, Y3
    Ren p[3]{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline C3 MakeC3(u32 n) {
    C3 c;
    const u32 f = n % 16;
    c.family = f;
    PickCallResults(n, 400, c.mirror, c.ret1, c.ret2);
    if (f == 12) { c.ret1 = Rand(n, 410) % 2 ? 0u : (Rand(n, 411) & 0xFFFFFF00u); if (Rand(n, 412) % 2 || c.mirror == 0) c.mirror = 0, c.ret2 = 0; }
    const u32 kind = f < 4 ? 0 : f < 6 ? 1 : f < 12 ? 2 : f == 12 ? Rand(n, 1) % 3 : 3;
    i32 hw = 0;
    switch (kind) {
    case 0: hw = Rand(n, 2) % 24; break;                       // pequeno: 3 hw^2 < 0x1000 y T < 0x800
    case 1: hw = Rand(n, 2) % 2 ? 36 + i32(Rand(n, 3) % 3) : 60 + i32(Rand(n, 3) % 8); break;   // umbrales (3 hw^2 = 0x1000 en hw ~ 36.99; T = 0x800 en hw ~ 62)
    case 2: hw = 40 + Rand(n, 2) % 30000; break;                // grande
    default: hw = Rand(n, 2) % 400; break;
    }
    i32 dx = 2 * hw + i32(Rand(n, 4) % 2);
    i32 x1 = Coord(n, 5, Rand(n, 6) % 2) * (Rand(n, 7) % 5 == 0 ? 8 : 1);
    i32 x2 = x1 + dx;
    if (f == 13) { const u32 s = Rand(n, 8) % 3; x2 = s == 0 ? x1 - dx : s == 1 ? x1 : x1 - 1; }
    if (f == 14) {
        static constexpr i32 e32[]{INT32_MIN, INT32_MIN + 1, INT32_MAX, INT32_MAX - 1, 0, -1, 1, 0x40000000, -0x40000000, 0x7FFFFFFE, -0x7FFFFFFF};
        if (Rand(n, 9) % 2) x1 = e32[Rand(n, 10) % std::size(e32)]; else x2 = e32[Rand(n, 10) % std::size(e32)];
        if (Rand(n, 11) % 3 == 0) x2 = e32[Rand(n, 12) % std::size(e32)];
    }
    if (f == 15 && Rand(n, 13) % 64 == 0) { x1 = -(1 << 28) + i32(Rand(n, 14) % 1000); x2 = x1 + (1 << 29); }   // mitad de 2^28: la funcion siempre queda invisible (ver mas abajo)
    const i32 y1 = Coord(n, 15, Rand(n, 16) % 3), y2 = Coord(n, 17, Rand(n, 18) % 3), y3 = Coord(n, 19, Rand(n, 20) % 3);
    i32 x3 = Coord(n, 21, Rand(n, 22) % 3);
    if (f == 14 && Rand(n, 23) % 3 == 0) x3 = INT32_MAX - i32(Rand(n, 24) % 4);
    c.ints[0] = x1; c.ints[1] = y1; c.ints[2] = x2; c.ints[3] = y2; c.ints[4] = x3; c.ints[5] = y3;
    c.path = u32(TriPath(x1, x2));
    FillRen(n, c.p, 3, false, 500);
    // el EXE suma 1 + 2 + .. + (mitad + 1) con un bucle: solo se ejecuta si la esfera es visible, asi que las mitades absurdas (> 1e6) se dejan invisibles
    if (std::abs(std::int64_t(i32(u32(x2) - u32(x1)) / 2)) > 1000000) { c.ret1 = 0; c.mirror = 0; c.ret2 = 0; }
    c.eax = Rand(n, 70); c.ecx = Rand(n, 73); c.edx = Rand(n, 71); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC3(const C3& c) {
    Js j;
    std::vector<u32> p;
    for (const Ren& r : c.p) { p.push_back(r.z); p.push_back(r.big); p.push_back(r.sm); p.push_back(r.flow); }
    j.U("family", c.family).U("path", c.path).U("mirror", c.mirror).U("ret1", c.ret1).U("ret2", c.ret2)
     .A("ints", {u32(c.ints[0]), u32(c.ints[1]), u32(c.ints[2]), u32(c.ints[3]), u32(c.ints[4]), u32(c.ints[5])}).A("p", p)
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}

// ================================================================================================ 4: TestLineAgainstWater
// Arena de la salida (guarda | 3 floats + relleno | guarda): 0x40 + 0x10 + 0x30 = 0x80.
constexpr u32 kLineObj = 0x40, kLineObjBytes = 0x10, kLineArena = 0x80;
struct E4 {
    u32 family{};
    u32 from[3]{}, to[3]{};
    u32 outInit[3]{};                      // contenido inicial de la salida
    std::vector<u32> vx, vy;               // vertices (int16 de 16 bits)
    std::vector<u32> quads;                // 5 palabras por cuadrilatero: 4 indices + banderas
    std::vector<u32> tris;                 // 4 palabras por triangulo
    u32 vm1x{}, vm1y{};
    std::vector<u32> blockIdx, blockVal;   // bloques no vacios (indice 0..143) y su palabra
    std::vector<u32> comboVal;             // m_PolyCombos[0..n) (el resto, ceros)
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline u32 Info(u32 type, u32 id) { return (type << 14) | (id & 0x3FFFu); }
// Indice de bloque como el EXE: trunc(v * 0.002f + 6.0f) en doble (solo para colocar poligonos; el banco compara contra el EXE)
inline i32 BlockOf(double v) { const double x = v * double(0.002f) + 6.0; return x != x || x > 1e9 || x < -1e9 ? 0 : i32(x); }
inline E4 MakeE4(u32 n) {
    E4 c;
    const u32 f = n % 16;
    c.family = f;
    // linea: 0..7 cruza z = 0 en el mundo; 8..9 fuera del mundo; 10 sin cruce; 11 especiales; 12..15 mezclas
    double x0, y0, x1, y1, z0, z1;
    const u32 sel = f;
    auto world = [&](u32 k) { return double(i32(Rand(n, k) % 6001u) - 3000); };
    if (sel == 8 || sel == 9) {   // fuera del mundo
        const double base = (Rand(n, 1) % 2 ? 3000.0 : -3500.0) + double(Rand(n, 2) % 400);
        x0 = base; x1 = base + double(i32(Rand(n, 3) % 700) - 350);
        y0 = world(4); y1 = y0 + double(i32(Rand(n, 5) % 900) - 450);
        if (Rand(n, 6) % 2) { std::swap(x0, y0); std::swap(x1, y1); }
    } else {
        x0 = world(1); y0 = world(2);
        const double span = Rand(n, 3) % 4 == 0 ? 2500.0 : 700.0;
        x1 = x0 + double(i32(Rand(n, 4) % u32(2 * span + 1)) - i32(span)); y1 = y0 + double(i32(Rand(n, 5) % u32(2 * span + 1)) - i32(span));
        if (Rand(n, 6) % 5 == 0) y1 = y0;
        if (Rand(n, 7) % 5 == 0) x1 = x0;
    }
    z0 = double(i32(Rand(n, 8) % 61u) + 1); z1 = -double(i32(Rand(n, 9) % 61u) + 1);
    if (Rand(n, 10) % 4 == 0) z1 = -z0;              // cruce exactamente en el medio
    if (Rand(n, 11) % 2) std::swap(z0, z1);
    if (sel == 10) { if (Rand(n, 12) % 2) z1 = z0 + double(Rand(n, 13) % 20); else { z0 = -z0; z1 = z0 - double(Rand(n, 14) % 20); } }   // sin cruce
    // coordenadas como float (con alguna fraccion)
    u32 fr[6];
    double* v[6]{&x0, &y0, &z0, &x1, &y1, &z1};
    for (int i = 0; i < 6; ++i) {
        double val = *v[i];
        if (Rand(n, 20 + i) % 3 == 0) val += double(Rand(n, 30 + i) % 8) / 8.0;
        fr[i] = B(float(val));
    }
    c.from[0] = fr[0]; c.from[1] = fr[1]; c.from[2] = fr[2]; c.to[0] = fr[3]; c.to[1] = fr[4]; c.to[2] = fr[5];
    if (f == 15 && Rand(n, 300) % 2) {   // x tan lejos que el indice de bloque (32 bits bajos) multiplicado por 12 envuelve a la fila 0 de la cuadricula (ix = j * 2^30)
        const u32 j = 1 + Rand(n, 301) % 3;
        const double target = double(j) * 1073741824.0;
        const float x = float((target - 6.0) / double(0.002f));
        for (i32 k = -200; k <= 200; ++k) {
            const float cand = F(B(x) + u32(k));
            volatile float p = cand * 0.002f;
            volatile float q = p + 6.0f;
            const double qd = q;
            if (u32(std::int64_t(qd)) == u32(std::uint64_t(target))) { c.from[0] = B(cand); c.to[0] = B(cand); break; }
        }
    }
    if (sel == 11) {   // especiales: a lo sumo UN NaN, o infinitos
        u32* all[6]{&c.from[0], &c.from[1], &c.from[2], &c.to[0], &c.to[1], &c.to[2]};
        const u32 k = Rand(n, 40) % 6;
        const u32 what = Rand(n, 41) % 8;
        if (what < 3) *all[k] = NanBits(k, what == 0);
        else if (what < 5) *all[k] = InfBits(n, 42);
        else if (what == 5) *all[k] = kFloatEdges[Rand(n, 43) % kFloatEdges.size()];
        else if (what == 6) { *all[k] = HugeFloat(n, 44); }
        else { *all[k] = TinyFloat(n, 45); }
        if (Rand(n, 46) % 5 == 0) { const u32 k2 = (k + 1 + Rand(n, 47) % 5) % 6; if (!IsNanBits(*all[k])) *all[k2] = InfBits(n, 48); }
    }
    if (f == 12 && Rand(n, 49) % 6 == 0) { c.from[2] = Rand(n, 50) % 2 ? 0u : 0x80000000u; }   // z = +-0
    if (f == 13 && Rand(n, 51) % 6 == 0) { c.to[2] = c.from[2]; }                                 // z iguales
    for (int i = 0; i < 3; ++i) c.outInit[i] = Rand(n, 52 + i);
    // vertices, cuadrilateros y bloques alrededor del cruce
    const double fz0 = F(c.from[2]), fz1 = F(c.to[2]);
    double cx = x0, cy = y0;
    if (fz0 != fz1 && fz0 == fz0 && fz1 == fz1) { const double s = std::abs(fz0) / (std::abs(fz0) + std::abs(fz1)); cx = x0 + (x1 - x0) * s; cy = y0 + (y1 - y0) * s; }
    const bool hasVm1 = Rand(n, 60) % 8 == 0;
    auto addVertex = [&](i32 x, i32 y) { c.vx.push_back(u32(x) & 0xFFFFu); c.vy.push_back(u32(y) & 0xFFFFu); return u32(c.vx.size() - 1); };
    auto addQuad = [&](i32 qx, i32 w, i32 qy, i32 h) {
        const u32 a = addVertex(qx, qy), b = addVertex(qx + w, qy), d = addVertex(qx, qy + h), e = addVertex(qx + w, qy + h);
        c.quads.push_back(a); c.quads.push_back(b); c.quads.push_back(d); c.quads.push_back(e);
        c.quads.push_back(Rand(n, 70 + u32(c.quads.size())) & 0xFFFFu);
        return u32(c.quads.size() / 5 - 1);
    };
    auto addTri = [&]() {
        const u32 a = addVertex(Coord(n, 80 + u32(c.tris.size()), 0), Coord(n, 81 + u32(c.tris.size()), 0)), b = addVertex(Coord(n, 82 + u32(c.tris.size()), 0), Coord(n, 83 + u32(c.tris.size()), 0)), d = addVertex(Coord(n, 84 + u32(c.tris.size()), 0), Coord(n, 85 + u32(c.tris.size()), 0));
        c.tris.push_back(a); c.tris.push_back(b); c.tris.push_back(d); c.tris.push_back(Rand(n, 86 + u32(c.tris.size())) & 0xFFFFu);
        return u32(c.tris.size() / 4 - 1);
    };
    if (c.tris.empty()) { const u32 nt = Rand(n, 61) % 3; for (u32 i = 0; i < nt; ++i) addTri(); }
    // un poligono que contiene (o roza) el cruce
    auto crossQuad = [&]() {
        const i32 fx = i32(std::floor(cx)), fy = i32(std::floor(cy));
        const u32 mode = Rand(n, 90 + u32(c.quads.size())) % 8;
        i32 qx = fx - i32(Rand(n, 91 + u32(c.quads.size())) % 200), w = 200 + i32(Rand(n, 92 + u32(c.quads.size())) % 200);
        i32 qy = fy - i32(Rand(n, 93 + u32(c.quads.size())) % 200), h = 200 + i32(Rand(n, 94 + u32(c.quads.size())) % 200);
        switch (mode) {
        case 0: qx = fx; break;                       // borde izquierdo en el cruce
        case 1: w = fx - qx; break;                   // borde derecho en floor(cruce)
        case 2: w = fx - qx + 1; break;               // borde derecho en floor + 1
        case 3: qy = fy; break;
        case 4: h = fy - qy; break;
        case 5: h = fy - qy + 1; break;
        case 6: qx = fx + 1 + i32(Rand(n, 95 + u32(c.quads.size())) % 3); break;   // se queda a la derecha: no contiene el cruce
        default: break;
        }
        if (Rand(n, 96 + u32(c.quads.size())) % 16 == 0) w = -w;                   // ancho negativo
        return addQuad(qx, w, qy, h);
    };
    auto decoyQuad = [&]() { return addQuad(Coord(n, 97 + u32(c.quads.size()), Rand(n, 98) % 2), i32(Rand(n, 99 + u32(c.quads.size())) % 700), Coord(n, 100 + u32(c.quads.size()), Rand(n, 101) % 2), i32(Rand(n, 102 + u32(c.quads.size())) % 700)); };
    // bloques recorridos por la caja de la linea (maximo 6 x 6)
    const double minx = std::min(x0, x1), maxx = std::max(x0, x1), miny = std::min(y0, y1), maxy = std::max(y0, y1);
    const i32 bx0 = BlockOf(minx), bx1 = BlockOf(maxx), by0 = BlockOf(miny), by1 = BlockOf(maxy);
    u32 combosUsed = 0;
    const bool useCombos = Rand(n, 110) % 2 == 0;
    for (i32 bx = bx0; bx <= bx1 && bx <= bx0 + 5; ++bx) {
        for (i32 by = by0; by <= by1 && by <= by0 + 5; ++by) {
            if (bx < 0 || bx >= 12 || by < 0 || by >= 12) continue;
            const u32 idx = u32(bx * 12 + by);
            const u32 r = Rand(n, 120 + u32(idx)) % 12;
            const bool atCross = BlockOf(cx) == bx && BlockOf(cy) == by;
            if (c.quads.size() / 5 + 3 >= kMaxQuads) break;
            if (r < 3 && !atCross) continue;                                   // vacio
            if (r < 5) { c.blockIdx.push_back(idx); c.blockVal.push_back(Info(1, atCross ? crossQuad() : decoyQuad())); }   // cuadrilatero unico
            else if (r < 6 && !c.tris.empty()) { c.blockIdx.push_back(idx); c.blockVal.push_back(Info(2, Rand(n, 130 + idx) % u32(c.tris.size() / 4))); }   // triangulo unico (no se prueba)
            else if (useCombos || atCross) {                                    // combinacion
                const u32 start = combosUsed;
                const u32 len = Rand(n, 140 + idx) % 4;
                bool placedCross = false;
                for (u32 k = 0; k < len; ++k) {
                    const u32 kind = Rand(n, 150 + idx + k) % 8;
                    if (kind < 5) { const bool cross = atCross && !placedCross && Rand(n, 160 + idx + k) % 2; placedCross |= cross; c.comboVal.push_back(Info(1, cross ? crossQuad() : decoyQuad())); }
                    else if (kind < 7 && !c.tris.empty()) c.comboVal.push_back(Info(2, Rand(n, 170 + idx + k) % u32(c.tris.size() / 4)));
                    else c.comboVal.push_back(Info(3, Rand(n, 180 + idx + k) & 0x3FFFu));
                }
                if (atCross && !placedCross && Rand(n, 190) % 3 != 0) c.comboVal.push_back(Info(1, crossQuad()));
                c.comboVal.push_back(Info(0, Rand(n, 191 + idx) % 3 == 0 ? (Rand(n, 192 + idx) & 0x3FFFu) : 0u));   // terminador (tipo 0, id cualquiera)
                combosUsed = u32(c.comboVal.size());
                if (c.comboVal.size() > 600) break;
                c.blockIdx.push_back(idx); c.blockVal.push_back(Info(3, start));
            }
        }
    }
    // algun bloque lejos de la linea (no se recorre) con basura, y bloque con combinacion vacia
    if (Rand(n, 200) % 4 == 0 && c.quads.size() >= 5) { c.blockIdx.push_back(143u); c.blockVal.push_back(Info(1, 0u)); }
    if (hasVm1) { c.vm1x = u32(Coord(n, 210, Rand(n, 211) % 3)) & 0xFFFFu; c.vm1y = u32(Coord(n, 212, Rand(n, 213) % 3)) & 0xFFFFu; }
    else { c.vm1x = 0; c.vm1y = 0; }
    if (hasVm1 && !c.quads.empty()) { c.quads[0] = 0xFFFFu; c.quads[1] = c.quads[1]; }   // el primer cuadrilatero usa el vertice -1 como vertice 0
    c.eax = Rand(n, 70); c.ecx = Rand(n, 73); c.edx = Rand(n, 71); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonE4(const E4& c) {
    Js j;
    j.U("family", c.family).A("from", {c.from[0], c.from[1], c.from[2]}).A("to", {c.to[0], c.to[1], c.to[2]}).A("out_init", {c.outInit[0], c.outInit[1], c.outInit[2]})
     .A("vx", c.vx).A("vy", c.vy).A("quads", c.quads).A("tris", c.tris).U("vm1x", c.vm1x).U("vm1y", c.vm1y)
     .A("block_idx", c.blockIdx).A("block_val", c.blockVal).A("combos", c.comboVal)
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}
}  // namespace Water62Fix

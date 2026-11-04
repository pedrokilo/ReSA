#pragma once
// Fixtures del banco Fn91A (lote 91A): cuatro funciones de CWaterLevel, 16384 casos cada una (65536 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y fixture_dump.cpp.
//
//   0 CWaterLevel::AddToQuadsAndTrianglesList                   0x6E5750  cdecl (blockX, blockY, index, type), `ret` del llamador; void       (hook CWaterLevel, tipo S)
//   1 CWaterLevel::GetGroundLevel                               0x6EA8A0  cdecl (const CVector* pos, float* outZ, uint8* outColData, float range); devuelve AL
//   2 CWaterLevel::RenderHighDetailWaterRectangle_OneLayer      0x6E91D0  cdecl (minX, maxX, Y1, Y2, CRenPar P1..P4 por valor (16 bytes), layer, twiceArea, triangular, halfX, halfY); void
//   3 CWaterLevel::RenderHighDetailWaterTriangle_OneLayer       0x6E8780  cdecl (X1, Y1, P1, X2, Y2, P2, X3, Y3, P3, layer, halfSquared, triangular, halfW); void
//
// Todos los floats viajan como patrones de 32 bits sin signo. Familia = n % 16 (1024 casos por familia). Los NaN son siempre 0xFFC00000 (el NaN indefinido de la x87): ninguna funcion mezcla dos NaN distintos.
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

namespace F91AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 4;

// Regiones de memoria del juego que tocan las funciones (se guardan al empezar, se rellenan por caso y se restauran tras cada lado).
constexpr u32 kBufAddr = 0xC4B950, kBufBytes = 0x14008, kIdxOff = 8, kVtxOff = 0x2008, kVertexBytes = 36;   // contadores (vertices, indices) + 4096 indices + 2048 vertices de 36 bytes
constexpr u32 kColAddr = 0xC1F960, kColBytes = 0x1808, kColB = 8, kColG = 0x808, kColR = 0x1008;              // contador de colores + tablas B, G, R de 0x800 bytes
constexpr u32 kComboAddr = 0xC215F0, kComboBytes = 0x6A0, kComboList = 8, kComboBlocks = 0x580;              // contador + m_PolyCombos (0xC215F8) + bloques 12 x 12 (0xC21B70)
constexpr u32 kGlobAddr = 0xC21168, kGlobBytes = 0x20;                                                      // 0xC21168 color triangulo, 0xC2116C WaterColor, 0xC21170..0xC21184 desplazamientos
constexpr u32 kNormAddr = 0xC278D4, kNormBytes = 0x10;                                                      // normal de las olas (3 floats) y byte 0xC278E0 (depuracion)
constexpr u32 kDetailAddr = 0x8D37D0, kAlphaAddr = 0x8D3808, kAlphaBytes = 0x0C, kK3834Addr = 0x8D3834;      // distancia de detalle, alfas de capa, escala de la capa 2
constexpr u32 kCamPosAddr = 0xB6F02C, kCamMatAddr = 0xB6F03C;                                                // posicion del placement de la camara y puntero a su matriz
constexpr u32 kMaxVertices = 2048, kMaxIndices = 4096, kMaxColors = 2048;
constexpr std::size_t kGuard = 0x40;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 91001u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
// Relleno previo de una region: 0 y 5 ruido, 1 ceros, 2 0xFF, 3 0xCD, 4 0xA5
inline void FillRegion(u8* region, u32 size, u32 mode, u32 seed) {
    switch (mode) {
    case 1: std::memset(region, 0x00, size); break;
    case 2: std::memset(region, 0xFF, size); break;
    case 3: std::memset(region, 0xCD, size); break;
    case 4: std::memset(region, 0xA5, size); break;
    default: FillWords(region, size, seed); break;
    }
}
inline u32 B(float f) { return std::bit_cast<u32>(f); }
inline float F(u32 b) { return std::bit_cast<float>(b); }

struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& I(const char* k, i32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& A(const char* k, const std::vector<u32>& v) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < v.size(); ++i) { if (i) s += ','; s += std::to_string(v[i]); }
        s += ']'; return *this;
    }
    std::string End() const { return s + "}"; }
};

inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Un float "especial": cero, desnormal, infinito, NaN (un unico patron), enorme, o un valor normal
inline u32 Special(u32 c, u32 k) {
    const u32 r = Rand(c, k) % 32;
    if (r < 10) return Pick(kTiny, c, k + 1);
    if (r < 16) return Pick(kInf, c, k + 1);
    if (r < 24) return 0xFFC00000u;
    if (r < 28) return Pick(kBig, c, k + 1);
    return B(float(int(Rand(c, k + 3) % 2001) - 1000) / 7.0f);
}
// Float con signo aleatorio, exponente uniforme en [lo, hi] y mantisa aleatoria
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
inline i32 RI(u32 c, u32 k, i32 lo, i32 hi) { return lo + i32(Rand(c, k) % u32(hi - lo + 1)); }
inline u32 Cap(u32 r, u32 limit) { return r % (limit + 1); }

struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 mode{}, rseed{};                                  // relleno de las regiones
    // 0: AddToQuadsAndTrianglesList
    i32 qa{}, qb{}, qi{};
    u32 qtype{}, qcount{}, qold{};
    // 1: GetGroundLevel
    u32 gpos[3]{}, grange{}, gcolnull{}, ghit{}, gz{}, gsurf{}, gpiece{}, gretHi{};
    // 2 y 3: rectangulo (g = minX, maxX, Y1, Y2) y triangulo (g = X1, Y1, X2, Y2, X3, Y3)
    i32 g[6]{};
    u32 p[4][4]{};                                        // CRenPar: z, grandes, pequenas, dword de banderas (flowX, flowY y relleno)
    i32 layer{}, junkA{}, junkB{}, hx{}, hy{};
    u32 shift[6]{};                                       // 0xC21170, 0xC21174, 0xC21178, 0xC2117C, 0xC21180, 0xC21184
    u32 wcol{}, alpha[3]{}, k3834{}, flag{};
    i32 detail{48};
    u32 useMat{}, camP[3]{}, camM[3]{};
    u32 preVtx{}, preIdx{}, preCi{}, vtx0{}, idx0{}, ci0{}, wseed{};
    i32 mutCall{-1};
    u32 mutDv{}, mutDi{}, mutDc{};
};

// Numero de vertices que escribe una llamada (rejilla del rectangulo; filas del triangulo) y margenes de las zonas observadas.
inline u32 Vertices(unsigned fn, i32 hx, i32 hy) {
    if (fn == 2) return (hx >= 0 && hy >= 0) ? u32(hx + 1) * u32(hy + 1) : 0u;
    if (fn == 3) return hx >= 0 ? u32(hx + 1) * u32(hx + 2) / 2u : 0u;
    return 0;
}
inline u32 SpanVertices(unsigned fn, i32 hx, i32 hy) { return Vertices(fn, hx, hy) + 8; }
inline u32 SpanIndices(unsigned fn, i32 hx, i32 hy) { return 6 * Vertices(fn, hx, hy) + 3 * u32(hx > 0 ? hx : 0) + 16 + 6; }
inline u32 SpanColors(unsigned fn, i32 hx, i32 hy) { return Vertices(fn, hx, hy) + 8; }

inline void CommonAbi(C& c) {
    c.eax = Rand(c.seed, 1) ; c.ecx = Rand(c.seed, 2); c.edx = Rand(c.seed, 3); c.stack = StackSeed(c.n);
    c.mode = Rand(c.seed, 4) % 6; c.rseed = Rand(c.seed, 5);
}

inline C MakeQuad(unsigned n) {
    C c; c.fn = 0; c.n = n; c.family = n % 16; c.seed = Mix(n * 0x2545F491u + 0x1000u);
    CommonAbi(c);
    auto R = [&](u32 k) { return Rand(c.seed, 100 + k); };
    c.qa = i32(R(1) % 12); c.qb = i32(R(2) % 12);
    c.qtype = 1 + R(3) % 2;
    c.qcount = R(4) % 696;
    c.qi = i32(R(7) % 700);
    const u32 oldType = R(5) % 4;
    c.qold = (oldType << 14) | (R(6) % 0x4000);
    auto setOld = [&](u32 t) { c.qold = (t << 14) | (R(6) % 0x4000); };
    switch (c.family) {
    case 1: setOld(0); break;
    case 2: setOld(1); break;
    case 3: setOld(2); break;
    case 4: setOld(3); break;
    case 5: c.qtype = Pick(std::array<u32, 8>{0u, 3u, 0x10001u, 0xFFFFFFFFu, 0x8000u, 0x4000u, 0x20000u, 0xFFFFu}, c.seed, 10); break;
    case 6: c.qi = i32(Pick(std::array<u32, 8>{0xFFFFFFFFu, 0u, 0x3FFFu, 0x4000u, 0xFFFFu, 0x10000u, 0x80000000u, 0x7FFFFFFFu}, c.seed, 11)); break;
    case 7: c.qcount = Pick(std::array<u32, 6>{0u, 1u, 2u, 3u, 694u, 695u}, c.seed, 12); break;
    case 8: { const u32 s = R(8) % 4; c.qa = (s & 1) ? 11 : 0; c.qb = (s & 2) ? 11 : 0; if (R(9) % 2) std::swap(c.qa, c.qb); break; }
    case 9: setOld(3); c.qcount = R(10) % 3; break;
    case 10: setOld(1 + R(11) % 2); c.qcount = 693 + R(12) % 3; break;
    case 11: c.qold = R(13) & 0xFFFFu; break;
    case 12: setOld(3); c.qtype = 1 + R(14) % 2; break;
    case 13: setOld(1 + R(15) % 2); c.qtype = 1 + R(16) % 2; break;
    case 14: setOld(0); c.qtype = Pick(std::array<u32, 4>{0u, 1u, 2u, 3u}, c.seed, 17); break;
    default: break;
    }
    return c;
}

inline C MakeGround(unsigned n) {
    C c; c.fn = 1; c.n = n; c.family = n % 16; c.seed = Mix(n * 0x2545F491u + 0x2000u);
    CommonAbi(c);
    auto R = [&](u32 k) { return Rand(c.seed, 100 + k); };
    for (u32 k = 0; k < 2; ++k) c.gpos[k] = B(float(int(R(k) % 6001) - 3000) / 4.0f);
    c.gpos[2] = B(float(int(R(2) % 801) - 400) / 4.0f);
    c.grange = B(30.0f);
    c.ghit = R(3) % 4 != 0;
    c.gcolnull = R(4) % 3 == 0;
    c.gz = B(float(int(R(5) % 4001) - 2000) / 16.0f);
    c.gsurf = R(6) & 0xFF; c.gpiece = R(7) & 0xFF;
    c.gretHi = R(8) & 0xFFFFFF00u;
    switch (c.family) {
    case 1: c.grange = Pick(std::array<u32, 8>{B(0.0f), B(-30.0f), B(1e-3f), B(100.0f), B(1e6f), B(0.5f), B(-0.0f), B(30.000004f)}, c.seed, 20); break;
    case 2: c.gpos[2] = Pick(std::array<u32, 4>{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0x7E800000u}, c.seed, 21); c.grange = Pick(std::array<u32, 4>{0x7F7FFFFFu, 0x7F000000u, 0xFF7FFFFFu, B(30.0f)}, c.seed, 22); break;
    case 3: c.gpos[2] = Pick(kTiny, c.seed, 23); c.grange = Pick(kTiny, c.seed, 24); break;
    case 4: c.ghit = 0; break;
    case 5: c.ghit = 1; c.gcolnull = 1; break;
    case 6: c.gpos[R(25) % 3] = Special(c.seed, 26); break;
    case 7: c.grange = Special(c.seed, 27); break;
    case 8: c.ghit = 1; c.gz = Special(c.seed, 28); break;
    case 9: c.gpos[0] = 0x80000000u; c.gpos[1] = 0x80000000u; c.gpos[2] = 0x80000000u; c.grange = Pick(std::array<u32, 2>{0x80000000u, 0u}, c.seed, 29); break;
    case 10: c.ghit = 1; c.gcolnull = 0; c.gsurf = 0xFF; c.gpiece = 0xFF; break;
    case 11: for (u32 k = 0; k < 3; ++k) c.gpos[k] = RandFloat(c.seed, 30 + k, -30, 30); c.grange = RandFloat(c.seed, 33, -30, 30); break;
    case 12: c.ghit = 1; c.gz = RandFloat(c.seed, 34, -30, 30); break;
    default: break;
    }
    return c;
}

inline C MakeWater(unsigned fn, unsigned n) {
    C c; c.fn = fn; c.n = n; c.family = n % 16; c.seed = Mix(n * 0x2545F491u + (fn == 2 ? 0x3000u : 0x4000u));
    CommonAbi(c);
    const bool rect = fn == 2;
    const u32 fam = c.family;
    auto R = [&](u32 k) { return Rand(c.seed, 100 + k); };
    // ---- tamano de la rejilla
    auto smallDim = [&](u32 k) { return 1 + i32(R(k) % 4); };
    auto mediumDim = [&](u32 k) { return 5 + i32(R(k) % 8); };
    c.hx = smallDim(1); c.hy = smallDim(2);
    if (fam == 2 || fam == 3) { c.hx = mediumDim(1); c.hy = mediumDim(2); }
    if (fam == 4 || fam == 9 || fam == 10 || fam == 11 || fam == 12 || fam == 13 || fam == 14 || fam == 15) { if (R(3) % 3 == 0) { c.hx = mediumDim(1); c.hy = mediumDim(2); } }
    if (fam == 7) { const u32 s = R(3) % 3; if (s != 1) c.hx = 1; if (s != 0) c.hy = 1; }
    if (fam == 6) { if (R(3) % 3 != 1) c.hx = -(1 + i32(R(4) % 3)); if (R(3) % 3 != 0) c.hy = -(1 + i32(R(5) % 3)); }
    if (R(6) % 64 == 0) { c.hx = 13 + i32(R(1) % 8); c.hy = 13 + i32(R(2) % 8); }
    if (!rect) { c.hy = c.hx; }
    // ---- geometria
    i32 base = RI(c.seed, 110, -3000, 3000);
    for (u32 k = 0; k < 6; ++k) c.g[k] = 0;
    if (rect) {
        c.g[0] = base; c.g[1] = base + 1 + RI(c.seed, 111, 0, 400);
        c.g[2] = RI(c.seed, 112, -3000, 3000); c.g[3] = c.g[2] + 1 + RI(c.seed, 113, 0, 400);
        if (R(14) % 8 == 0) std::swap(c.g[0], c.g[1]);
        if (R(15) % 8 == 0) std::swap(c.g[2], c.g[3]);
    } else {
        c.g[0] = base; c.g[1] = RI(c.seed, 112, -3000, 3000);
        c.g[2] = base + 1 + RI(c.seed, 111, 0, 400); c.g[3] = c.g[1] + RI(c.seed, 113, -100, 400);
        c.g[4] = (R(16) % 3 == 0) ? c.g[0] : base + RI(c.seed, 117, -100, 300); c.g[5] = c.g[1] + 1 + RI(c.seed, 118, 0, 400);
        if (R(16) % 3 == 1) c.g[4] = c.g[0];
    }
    if (fam == 8) {
        const std::array<i32, 10> big{0x7FFFFFFF, i32(0x80000000), 0x40000000, -0x40000000, 0x01000001, -0x01000001, 0x3FFFFFFF, 0x00FFFFFF, 0x7FFFFF00, -0x7FFFFF00};
        for (u32 k = 0; k < (rect ? 4u : 6u); ++k) c.g[k] = (R(20 + k) % 3 == 0) ? big[R(30 + k) % 10] : c.g[k] + big[R(40 + k) % 10] / 4;
    }
    // La division de la funcion lanza #DE con cociente fuera de rango (INT_MIN / -1): se evita.
    auto diffMin = [&](i32 a, i32 b) { return std::uint32_t(a) - std::uint32_t(b) == 0x80000000u; };
    if (c.hx == -1 && (rect ? (diffMin(c.g[1], c.g[0]) || diffMin(c.g[0], c.g[1])) : (diffMin(c.g[2], c.g[0]) || diffMin(c.g[0], c.g[2]) || diffMin(c.g[4], c.g[1])))) c.hx = -2;
    if (rect && c.hy == -1 && (diffMin(c.g[3], c.g[2]) || diffMin(c.g[2], c.g[3]))) c.hy = -2;
    if (!rect && c.hx == -1 && (diffMin(c.g[5], c.g[1]) || diffMin(c.g[5], c.g[3]) || diffMin(c.g[1], c.g[5]) || diffMin(c.g[1], c.g[3]))) c.hx = -2;
    // ---- esquinas (CRenPar): z, grandes, pequenas, banderas
    const u32 nP = rect ? 4 : 3;
    for (u32 k = 0; k < nP; ++k) {
        c.p[k][0] = B(float(int(R(50 + k) % 4001) - 2000) / 1000.0f);
        c.p[k][1] = B(float(int(R(54 + k) % 1001)) / 1000.0f);
        c.p[k][2] = B(float(int(R(58 + k) % 1001)) / 1000.0f);
        c.p[k][3] = R(62 + k);
    }
    if (fam == 10) for (u32 k = 0; k < nP; ++k) for (u32 j = 0; j < 3; ++j) c.p[k][j] = RandFloat(c.seed, 200 + 4 * k + j, -30, 30);
    if (fam == 11 || (fam == 15 && R(70) % 4 == 0)) c.p[R(71) % nP][R(72) % 3] = Special(c.seed, 73);
    // ---- capa
    c.layer = (R(80) % 2) ? 1 : 0;
    if (fam == 0) c.layer = 0;
    if (fam == 1) c.layer = 1;
    if (fam == 5) c.layer = rect ? Pick(std::array<u32, 6>{2u, 3u, 0xFFFFFFFFu, 5u, 0x80000000u, 100u}, c.seed, 81) : Pick(std::array<u32, 5>{3u, 4u, 0xFFFFFFFFu, 7u, 0x80000000u}, c.seed, 81);
    c.junkA = i32(R(82)); c.junkB = i32(R(83));
    // ---- globales
    for (u32 k = 0; k < 6; ++k) c.shift[k] = B(float(R(90 + k) % 100000) / 100000.0f);
    if (fam == 12) for (u32 k = 0; k < 6; ++k) c.shift[k] = (R(96 + k) % 3 == 0) ? Special(c.seed, 102 + k) : RandFloat(c.seed, 108 + k, -10, 40);
    c.wcol = R(120);
    c.alpha[0] = 0xA0; c.alpha[1] = 0x5A; c.alpha[2] = 0xFF;
    if (fam == 13 || fam == 15) for (u32 k = 0; k < 3; ++k) c.alpha[k] = R(121 + k);
    c.k3834 = B(0.1f);
    if (fam == 13 || fam == 15) c.k3834 = RandFloat(c.seed, 124, -4, 3);
    c.flag = 0;
    if (rect && (fam == 13 || (fam == 15 && R(125) % 4 == 0))) c.flag = 1 + R(126) % 255;
    c.detail = 48;
    if (fam == 14) c.detail = i32(Pick(std::array<u32, 10>{0u, 1u, 0xFFFFFFD0u, 1000u, 0x7FFFFFFFu, 0x80000000u, 2u, 24u, 36u, 47u}, c.seed, 127));
    // ---- camara
    c.useMat = R(130) % 2;
    const i32 cx0 = rect ? c.g[0] : c.g[0], cy0 = rect ? c.g[2] : c.g[1];
    auto camVec = [&](u32* v, u32 k) {
        v[0] = B(float(cx0) + float(int(R(k) % 201) - 100) / 2.0f); v[1] = B(float(cy0) + float(int(R(k + 1) % 201) - 100) / 2.0f); v[2] = B(float(int(R(k + 2) % 201) - 100));
    };
    camVec(c.camP, 140); camVec(c.camM, 150);
    if (fam == 9) {
        // la distancia al vertice base queda alrededor de los umbrales 0.75 y 1.0 (36 y 48 con la distancia de detalle 48)
        const std::array<float, 14> dist{35.0f, 35.99f, 36.0f, 36.01f, 37.0f, 40.0f, 47.0f, 47.99f, 48.0f, 48.01f, 49.0f, 60.0f, 0.0f, 1.0f};
        const float d = dist[R(160) % dist.size()];
        const u32 axis = R(161) % 3;
        const float sgn = (R(162) % 2) ? -1.0f : 1.0f;
        u32* v = (c.useMat ? c.camM : c.camP);
        v[0] = B(float(cx0) + (axis != 1 ? sgn * d : 0.0f)); v[1] = B(float(cy0) + (axis != 0 ? (axis == 2 ? 0.0f : sgn * d) : 0.0f));
    }
    if (fam == 11) { u32* v = (c.useMat ? c.camM : c.camP); v[R(163) % 3] = Special(c.seed, 164); }
    if (fam == 15 && R(165) % 5 == 0) { u32* v = (c.useMat ? c.camM : c.camP); v[R(166) % 2] = Special(c.seed, 167); }
    // ---- contadores y mutaciones del falso de las olas
    const u32 K = Vertices(fn, c.hx, c.hy);
    const u32 sv = SpanVertices(fn, c.hx, c.hy), si = SpanIndices(fn, c.hx, c.hy), sc = SpanColors(fn, c.hx, c.hy);
    c.preVtx = R(170); c.preIdx = R(171); c.preCi = R(172);
    c.vtx0 = Cap(R(173), kMaxVertices - 1 - 16 - sv); c.idx0 = Cap(R(174), kMaxIndices - 1 - 16 - si); c.ci0 = Cap(R(175), kMaxColors - 1 - 16 - sc);
    if (fam == 15 || fam == 13) { if (R(176) % 3 == 0) { c.vtx0 = kMaxVertices - 1 - 16 - sv; c.idx0 = kMaxIndices - 1 - 16 - si; c.ci0 = kMaxColors - 1 - 16 - sc; } }
    if (R(177) % 5 == 0) { c.vtx0 = 0; c.idx0 = 0; c.ci0 = 0; }
    (void)K;
    c.wseed = R(180);
    c.mutCall = -1;
    if (fam == 13 || fam == 15 || fam == 4) {
        if (R(181) % 3 == 0) { c.mutCall = i32(R(182) % 3); c.mutDv = R(183) % 4; c.mutDi = R(184) % 4; c.mutDc = R(185) % 4; }
    }
    return c;
}

inline C MakeC(unsigned fn, unsigned n) {
    switch (fn) {
    case 0: return MakeQuad(n);
    case 1: return MakeGround(n);
    default: return MakeWater(fn, n);
    }
}

// Salidas del falso de las olas para la llamada k (contador en orden de ejecucion): altura (campo 0), multiplicador de color (1), brillo (2) y normal (3..5); todos float.
inline u32 WaveOut(const C& c, u32 k, u32 field) {
    const u32 r = Rand(c.wseed, 5000 + k * 16 + field);
    if (field == 0) {
        if (r % 11 == 0) { const std::array<u32, 7> t{0xFFC00000u, 0u, 0x80000000u, 0x7F800000u, 0xFF800000u, 1u, 0x7F7FFFFFu}; return t[(r >> 8) % 7]; }
        return B(float(int(Rand(c.wseed, 7000 + k) % 4001) - 2000) / 500.0f);
    }
    if (field == 1) {
        if (r % 16 < 8) { const std::array<u32, 8> t{0xFFC00000u, 0u, 0xBF800000u, 0x7149F2CAu, 0xF149F2CAu, 0x7F800000u, 0x43960000u, 0x3B449BA6u}; return t[r % 16]; }
        return B(float(int(Rand(c.wseed, 8000 + k) % 3001)) / 1000.0f);
    }
    return B(float(int(r % 2001) - 1000) / 250.0f);
}

inline std::string JsonC(const C& c) {
    Js j;
    auto V = [](const u32* p, std::size_t k) { return std::vector<u32>(p, p + k); };
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack).U("mode", c.mode).U("rseed", c.rseed);
    if (c.fn == 0) {
        j.I("qa", c.qa).I("qb", c.qb).I("qi", c.qi).U("qtype", c.qtype).U("qcount", c.qcount).U("qold", c.qold);
    } else if (c.fn == 1) {
        j.A("gpos", V(c.gpos, 3)).U("grange", c.grange).U("gcolnull", c.gcolnull).U("ghit", c.ghit).U("gz", c.gz).U("gsurf", c.gsurf).U("gpiece", c.gpiece).U("gret_hi", c.gretHi);
    } else {
        std::vector<u32> g; for (int k = 0; k < 6; ++k) g.push_back(u32(c.g[k]));
        std::vector<u32> p; for (int k = 0; k < 4; ++k) for (int m = 0; m < 4; ++m) p.push_back(c.p[k][m]);
        j.A("g", g).A("p", p).I("layer", c.layer).I("junk_a", c.junkA).I("junk_b", c.junkB).I("hx", c.hx).I("hy", c.hy);
        j.A("shift", V(c.shift, 6)).U("wcol", c.wcol).A("alpha", V(c.alpha, 3)).U("k3834", c.k3834).U("flag", c.flag).I("detail", c.detail);
        j.U("use_mat", c.useMat).A("cam_p", V(c.camP, 3)).A("cam_m", V(c.camM, 3));
        j.U("pre_vtx", c.preVtx).U("pre_idx", c.preIdx).U("pre_ci", c.preCi).U("vtx0", c.vtx0).U("idx0", c.idx0).U("ci0", c.ci0).U("wseed", c.wseed);
        j.I("mut_call", c.mutCall).U("mut_dv", c.mutDv).U("mut_di", c.mutDi).U("mut_dc", c.mutDc);
    }
    return j.End();
}
}

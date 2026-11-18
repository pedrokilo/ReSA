#pragma once
// Fixtures del banco Fn228A (lote 228A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (CON hook del lote 228A, sin validar hasta este banco). 22 funciones del modulo RpWorld de RenderWare (bageomet.c y bamateri.c: morph targets, geometrias y materiales):
//   idx  direccion   funcion                                                              convencion
//    0   0x74C200   RpMorphTargetCalcBoundingSphere(morph, esfera)                       cdecl(2) -> morph (x87 de 24 bits)
//    1   0x74C310   RpGeometryAddMorphTargets(geometry, n)                                cdecl(2) -> int
//    2   0x74C690   RpGeometryTriangleSetVertexIndices(geometry, tri, v1, v2, v3)         cdecl(5) -> geometry
//    3   0x74C6C0   RpGeometryTriangleSetMaterial(geometry, tri, material)                cdecl(3) -> geometry o 0
//    4   0x74C7D0   RpGeometryLock(geometry, modo)                                        cdecl(2) -> geometry
//    5   0x74C800   RpGeometryUnlock(geometry)                                            cdecl(1) -> geometry o 0
//    6   0x74CA90   RpGeometryCreate(numVert, numTri, formato)                            cdecl(3) -> geometry o 0
//    7   0x74CCB0   RpGeometryAddRef(geometry)                                            cdecl(1) -> geometry
//    8   0x74CCC0   RpGeometryDestroy(geometry)                                           cdecl(1) -> 1
//    9   0x74CD70   RpGeometryRegisterPlugin(5 argumentos)                                cdecl(5) -> resultado
//   10   0x74CDA0   RpGeometryRegisterPluginStream(4 argumentos)                          cdecl(4) -> resultado
//   11   0x74D190   RpGeometryStreamRead(flujo)                                           cdecl(1) -> geometry o 0
//   12   0x74D6D0   _rpGeometryDestroyUnreferenced(geometry)                              cdecl(1) -> 1
//   13   0x74D7D0   _rpMaterialRightsStreamRead(flujo, longitud, objeto, desp, tam)       cdecl(5) -> flujo o 0
//   14   0x74D810   _rpMaterialRightsStreamWrite(flujo, longitud, material, desp, tam)    cdecl(5) -> flujo o 0
//   15   0x74D870   _rpMaterialSetDefaultSurfaceProperties(props)                        cdecl(1) -> void
//   16   0x74D990   RpMaterialCreate()                                                    cdecl(0) -> material o 0
//   17   0x74DA20   RpMaterialDestroy(material)                                            cdecl(1) -> 1
//   18   0x74DBC0   RpMaterialSetTexture(material, textura)                                cdecl(2) -> material
//   19   0x74DBF0   RpMaterialRegisterPlugin(5 argumentos)                                cdecl(5) -> resultado
//   20   0x74DC20   RpMaterialRegisterPluginStream(4 argumentos)                          cdecl(4) -> resultado
//   21   0x74DD30   RpMaterialStreamRead(flujo)                                           cdecl(1) -> material o 0
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, ventana de 0x2000, guardas de 0x40) y TODOS los colaboradores falseados salvo los que corren reales (ver LiveComparison.hpp).
// Los objetos del caso (geometrias, morph targets, triangulos, materiales, texturas, tuberias, listas) se escriben con "pokes" sobre un relleno pseudoaleatorio de las arenas (lo no escrito es basura que la funcion
// no debe leer); el oraculo Python repite exactamente los pokes. Familia = n % 16: tipo = familia % 4 (posicion del objeto principal: 0 alineado, 1 desalineado, 2 al borde de la ventana, 3 al inicio) y relleno =
// familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
// Los falsos devuelven resultados de COLAS por tipo de colaborador (a[kQ + 16 * cola + i]), consumidas en orden de llamada, y los datos del flujo salen de a[kData + i]; el oraculo consume lo mismo.
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

namespace F228AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 22;
constexpr unsigned kArenas = 4;
constexpr unsigned kSlots = 640;        // dwords de entrada por caso

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x2000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 100001u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
// Relleno de una region: 0 aleatorio (semilla), 1 ceros, 2 unos, 3 0xCD
inline void FillKind(u8* dst, std::size_t bytes, u32 kind, u32 seed) {
    if (kind == 0) FillWords(dst, bytes, seed);
    else std::memset(dst, kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), bytes);
}

// Poke: {espacio, desplazamiento, tamano, clase de valor, v1, v2}. espacio 0..3 = arena (desplazamiento respecto a la ventana), 4 = direccion absoluta del juego (desplazamiento = direccion).
// clase 0: valor = v1; 1: puntero a (arena v1, desplazamiento v2); 2: puntero a (arena v1, desplazamiento v2) menos la direccion de RwEngineInstance (para el desplazamiento del modulo de clumps).
using Poke = std::array<u32, 6>;

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
    Js& H(const char* k, const std::vector<u8>& v) {
        constexpr char d[] = "0123456789abcdef";
        Key(k); s += '"';
        for (u8 b : v) { s += d[b >> 4]; s += d[b & 15]; }
        s += '"'; return *this;
    }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, kSlots> a{};
    std::vector<Poke> pokes;
    std::vector<u8> s;          // datos en bytes (sin uso en este lote)
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 Pk(std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); }
    u32 W() { return Rand(n, nk()); }
};

inline u32 AvoidStack(u32 v) { return (v >= 0x00800000u && v < 0x02000000u) ? (v ^ 0x40000000u) : v; }
// Un dword "interesante" para punteros crudos y datos (nunca direcciones de las arenas ni de la pila del hilo del banco)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return AvoidStack(g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu}));
}
inline u32 RawNZ(Gen& g) { const u32 v = Raw(g); return v ? v : 0x00400123u; }

// ------------------------------------------------------------------------------------------------ direcciones y distribucion de los slots de `a`
inline constexpr u32 kEntryAddr[kFunctions]{0x74C200, 0x74C310, 0x74C690, 0x74C6C0, 0x74C7D0, 0x74C800, 0x74CA90, 0x74CCB0, 0x74CCC0, 0x74CD70, 0x74CDA0, 0x74D190, 0x74D6D0, 0x74D7D0, 0x74D810, 0x74D870, 0x74D990, 0x74DA20, 0x74DBC0, 0x74DBF0, 0x74DC20, 0x74DD30};
enum Fn : u32 { kCalcSphere, kAddMT, kTriSetVI, kTriSetMat, kLock, kUnlock, kGeoCreate, kAddRef, kGeoDestroy, kGeoRegPlugin, kGeoRegPluginStream, kGeoStreamRead, kGeoDestroyUnref, kRightsRead, kRightsWrite,
                kSetDefProps, kMatCreate, kMatDestroy, kMatSetTexture, kMatRegPlugin, kMatRegPluginStream, kMatStreamRead };
// Globales del juego que el banco toca: R0 = 0x8D628C (4: sizeOfStruct del registro de geometrias), R1 = 0x8D62BC (12: propiedades de superficie por defecto), R2 = 0xC924E8 (4: datos extra de derechos),
// R3 = 0xC92510 (8: id de derechos y desplazamiento de la lista libre de materiales), R4 = marca del banco (1)
inline constexpr u32 kGeoSizeAt = 0x8D628C, kDefPropsAt = 0x8D62BC, kRightsExtraAt = 0xC924E8, kRightsIdAt = 0xC92510, kModOffAt = 0xC92514, kBenchMarker = 0xA9B0B0, kEngineAt = 0xC97B24;
inline constexpr u32 kMarkerValue = 0x5A;
inline constexpr std::array<u32, 5> kRegAt{0x8D628Cu, 0x8D62BCu, 0xC924E8u, 0xC92510u, 0xA9B0B0u};
inline constexpr std::array<u32, 5> kRegBytes{4u, 12u, 4u, 8u, 1u};

// Parametros generales (a[...]) y colas. Cola k = a[kQ + 16 k + (i & 15)]. Datos del flujo: a[kData + i] (palabras consumidas en orden por RwStreamRead y RwStreamReadReal).
inline constexpr u32 kRdChunksRet = 26, kRightsId = 27, kRightsExtra = 28, kErrDelta = 30, kRegRet = 31;
inline constexpr u32 kBBox = 32;                                           // 6 floats que devuelve RwBBoxCalculate: sup x, y, z, inf x, y, z
inline constexpr u32 kSwapAddr = 40, kSwapVal = 41;                        // RwStreamWriteInt32: tras la primera llamada escribe kSwapVal en kSwapAddr (fichas; 0 = nada)
inline constexpr u32 kInitRefOn = 42, kInitRefVal = 43;                    // _rwPluginRegistryInitObject del registro de materiales: si kInitRefOn, deja refCount = kInitRefVal
inline constexpr u32 kArgs = 64;                                           // argumentos: 3 dwords por argumento (clase, v1, v2), hasta 6
inline constexpr u32 kQ = 128;
inline constexpr u32 kData = 460, kDataWords = 160;
enum QueueId : u32 { qAlloc, qMalloc, qRealloc, qFindOk, qFindLen, qFindVer, qReadOk, qRealOk, qRi32Ok, qRi32Val, qGeoCreate, qAddMT, qMatList, qUnlock, qTex, qMesh, qOpt, qAppend, qInit, qWriteOk, kQueues };

// ------------------------------------------------------------------------------------------------ constructor de escenas
struct Obj { u32 ar{}, off{}; };
struct Bld {
    C& c; Gen& g;
    u32 kind, mis;
    u32 cursor[4], limit[4];
    bool first{true};
    Bld(C& c_, Gen& g_, u32 kind_) : c(c_), g(g_), kind(kind_), mis(kind_ == 1 ? 1 + g_.R(3) : 0u) {
        for (int i = 0; i < 4; ++i) { cursor[i] = 0x10 + 4 * g.R(16); limit[i] = u32(kWin); }
    }
    Obj Alloc(u32 size) {
        const u32 sz = (size + 3) & ~3u;
        u32 ar = g.R(4);
        if (first) {
            first = false;
            if (kind == 2) { const u32 off = u32(kWin) - sz - 4 * g.R(3) - 4; limit[ar] = off - 8; return {ar, off}; }
            if (kind == 3) { const u32 off = 4 * g.R(4); cursor[ar] = off + sz + 8; return {ar, off}; }
        }
        for (int t = 0; t < 4; ++t, ar = (ar + 1) & 3u) {
            const u32 off = cursor[ar] + mis;
            if (off + sz + 8 <= limit[ar]) { cursor[ar] = off - mis + sz + 4 + 4 * g.R(8); return {ar, off}; }
        }
        std::abort();
    }
    void W(const Obj& o, u32 rel, u32 size, u32 v) { c.pokes.push_back({o.ar, o.off + rel, size, 0u, v, 0u}); }
    void Wabs(u32 addr, u32 size, u32 v) { c.pokes.push_back({4u, addr, size, 0u, v, 0u}); }
    void P(const Obj& o, u32 rel, const Obj& t, u32 trel = 0) { c.pokes.push_back({o.ar, o.off + rel, 4u, 1u, t.ar, t.off + trel}); }
    void WabsP(u32 addr, const Obj& t, u32 trel, u32 vk) { c.pokes.push_back({4u, addr, 4u, vk, t.ar, t.off + trel}); }
    // Ficha de argumento i: clase 1 puntero (arena, desplazamiento) o clase 0 crudo
    void ArgP(u32 i, const Obj& o, u32 rel = 0) { c.a[kArgs + 3 * i] = 1; c.a[kArgs + 3 * i + 1] = o.ar; c.a[kArgs + 3 * i + 2] = o.off + rel; }
    void ArgR(u32 i, u32 v) { c.a[kArgs + 3 * i] = 0; c.a[kArgs + 3 * i + 1] = v; c.a[kArgs + 3 * i + 2] = 0; }
    // Ficha (token) de un puntero para las colas: la misma forma que la etiqueta de los eventos
    static u32 Tok(const Obj& o, u32 rel = 0) { return 0x10000000u + (o.ar << 20) + u32(kGuard) + o.off + rel; }
    void Q(u32 q, u32 i, u32 v) { c.a[kQ + 16 * q + (i & 15)] = v; }
    void D(u32 i, u32 v) { if (i < kDataWords) c.a[kData + i] = v; }
};

constexpr u32 kGeoSize = 0x68, kMorphSize = 0x1C, kMatSize = 0x1C, kTexSize = 0x58, kPipeSize = 0x34;

// Bits de un float de aspecto "interesante" (sabor 0: rango normal; 1: un NaN 0x7FC00000; 2: infinitos; 3: magnitudes enormes)
inline u32 FloatBits(Gen& g, u32 flavor) {
    const u32 sign = g.P(35) ? 0x80000000u : 0u;
    if (flavor == 1 && g.P(12)) return 0x7FC00000u;
    if (flavor == 2 && g.P(18)) return sign | 0x7F800000u;
    if (g.P(8)) return sign;                                       // +-0
    if (flavor == 3 && g.P(60)) return sign | (u32(100 + g.R(28)) << 23) | (g.W() & 0x7FFFFFu);     // exponente 100..127 (hasta ~3.4e38)
    if (g.P(25)) return sign | std::bit_cast<u32>(float(1 + g.R(64)) * (g.P(50) ? 1.0f : 0.25f));
    const u32 e = 110 + g.R(34);                                    // exponentes 110..143 en sesgo: ~1e-4 .. 1e4
    return sign | (std::min(e, 150u) << 23) | (g.W() & 0x7FFFFFu);
}
inline float AsFloat(u32 b) { return std::bit_cast<float>(b); }

// ------------------------------------------------------------------------------------------------ generador
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    auto& a = c.a;
    a[1] = kind;
    a[2] = f / 4;
    a[90] = c.seed * 7u + 11u;
    a[92] = c.seed * 13u + 50u; a[93] = a[92] + 1u; a[94] = a[92] + 2u; a[95] = a[92] + 3u;
    Bld b(c, g, kind);
    // resultados de los colaboradores que no son colas
    a[kRdChunksRet] = g.P(88) ? 1u : (g.P(50) ? 0u : g.Pk({2u, 0xFFFFFFFFu, 0x100u}));
    a[kRightsId] = g.P(55) ? 0u : g.Pk({1u, 2u, 0x1Fu, 0xFFFFFFFFu, g.W()});
    a[kRightsExtra] = g.Pk({0u, 1u, g.W(), 0xDEADBEEFu});
    a[kErrDelta] = g.P(80) ? 0u : g.Pk({1u, 0xFFFFFFFFu, g.W()});
    a[kRegRet] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W(), 0x80000000u});
    const u32 fi = g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 2u, 3u});      // intensidad de fallos de los falsos en las funciones con flujo (0 = nunca)
    const u32 failPct = fi == 0 ? 0u : (fi == 1 ? 3u : (fi == 2 ? 8u : 22u));
    const u32 okPct = 100u - failPct;
    auto Fail = [&] { return g.P(failPct); };
    auto Ver = [&](bool bad) -> u32 {
        if (!bad) return g.Pk({0x34000u, 0x34000u, 0x36003u, 0x35000u, 0x34001u, 0x36002u, 0x34000u + g.R(0x2004)});
        return g.Pk({0x33FFFu, 0x36004u, 0u, 0xFFFFFFFFu, 0x80000000u, g.R(0x34000), 0x36004u + g.R(0x1000)});
    };
    // desplazamiento de la lista libre de materiales: RwEngineInstance + [0xC92514] cae en un bloque de la arena cuyo primer dword es el puntero de la lista
    auto MatFreeList = [&] {
        Obj ml = b.Alloc(8);
        b.W(ml, 0, 4, RawNZ(g)); b.W(ml, 4, 4, RawNZ(g));
        b.WabsP(kModOffAt, ml, 0, 2);
    };
    // un RpMaterial con textura, tuberia y raster del grupo dado
    auto RandDword = [&] { return g.W(); };
    switch (c.fn) {
    case kCalcSphere: {
        const u32 nv = g.Pk({1u, 1u, 2u, 3u, 4u, 5u, 6u, 8u, 0u});
        Obj G = b.Alloc(kGeoSize), M = b.Alloc(kMorphSize), V = b.Alloc(nv * 12 + 4), S = b.Alloc(16);
        b.W(G, 0x14, 4, nv); b.P(M, 0, G); b.P(M, 0x14, V);
        const u32 flavor = g.Pk({0u, 0u, 0u, 0u, 1u, 2u, 3u});
        float lo[3]{}, hi[3]{};
        bool seen[3]{};
        std::array<u32, 3> mn{}, mx{};
        for (u32 i = 0; i < nv * 3; ++i) {
            const u32 bits = FloatBits(g, flavor);
            b.W(V, 4 * i, 4, bits);
            const float v = AsFloat(bits);
            const u32 ax = i % 3;
            if (v == v) {   // el maximo y el minimo ignoran NaN
                if (!seen[ax] || v > hi[ax]) { hi[ax] = v; mx[ax] = bits; }
                if (!seen[ax] || v < lo[ax]) { lo[ax] = v; mn[ax] = bits; }
                seen[ax] = true;
            }
        }
        const bool consistent = nv > 0 && g.P(75);
        for (u32 ax = 0; ax < 3; ++ax) {
            a[kBBox + ax] = consistent ? mx[ax] : FloatBits(g, flavor == 1 ? 0u : flavor);
            a[kBBox + 3 + ax] = consistent ? mn[ax] : FloatBits(g, flavor == 1 ? 0u : flavor);
        }
        b.ArgP(0, M); b.ArgP(1, S);
        break;
    }
    case kAddMT: {
        const bool bigCase = g.P(10);
        const u32 flags = g.Pk({0u, 0x10u, 0x01000000u, 0x01000010u, 0x1Cu, 0x0F000010u, 0x0F000000u, g.W()});
        const bool native = (flags & 0x01000000u) != 0, normals = (flags & 0x10u) != 0;
        u32 nvu = g.Pk({0u, 1u, 1u, 2u, 3u, 5u, 8u});
        if (bigCase) nvu = g.Pk({0xFFFFFFFFu, 0xFFFFFFFDu, 0x7FFFFFFFu, 0x10000000u, 0x20000000u, 0x80000000u});
        const i32 nOld = i32(g.Pk({0u, 0u, 1u, 2u, 3u}));
        const bool hasOld = nOld > 0 ? g.P(92) : g.P(15);
        const i32 mt = i32(g.Pk({1u, 1u, 2u, 3u, 0u, 0xFFFFFFFFu}));
        const u32 size = native ? 0x1Cu : 0x1Cu + u32(i32(nvu)) * 12u * (normals ? 2u : 1u);
        const u32 total = (nOld + mt) > 0 ? u32(nOld + mt) * size : 0u;
        const u32 need = std::max<u32>(total, u32(nOld) * size + u32(std::max(mt, 0)) * 0x1C + 8u) + 0x40u;
        Obj G = b.Alloc(kGeoSize);
        const bool fits = !bigCase || native;
        const u32 kindAlloc = g.R(100);
        Obj oldB = fits ? b.Alloc(need) : b.Alloc(0x40);
        Obj newB = fits ? b.Alloc(need) : b.Alloc(0x40);
        b.W(G, 8, 4, flags); b.W(G, 0x14, 4, nvu); b.W(G, 0x18, 4, u32(nOld));
        if (hasOld) b.P(G, 0x5C, oldB); else b.W(G, 0x5C, 4, 0);
        const bool fail = (!fits && !native) ? true : (kindAlloc < 10);
        const u32 result = fail ? 0u : (kindAlloc < 55 && hasOld ? Bld::Tok(oldB) : Bld::Tok(newB));
        b.Q(qMalloc, 0, result); b.Q(qRealloc, 0, result);
        b.ArgP(0, G); b.ArgR(1, u32(mt));
        break;
    }
    case kTriSetVI: {
        Obj G = b.Alloc(kGeoSize), T = b.Alloc(8);
        b.ArgP(0, G); b.ArgP(1, T); b.ArgR(2, g.W()); b.ArgR(3, g.W()); b.ArgR(4, g.W());
        break;
    }
    case kTriSetMat: {
        const i32 nM = i32(g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 1u, 2u, 3u, 0xFFFFFFFFu, 0xFFFFFFFDu}));
        Obj G = b.Alloc(kGeoSize), T = b.Alloc(8), arr = b.Alloc(32);
        std::vector<u32> pool; for (int i = 0; i < 6; ++i) pool.push_back(g.P(20) ? 0u : RawNZ(g));
        for (u32 i = 0; i < 8; ++i) b.W(arr, 4 * i, 4, pool[g.R(6)]);
        b.W(G, 0x24, 4, u32(nM));
        if (nM > 0 || g.P(90)) b.P(G, 0x20, arr); else b.W(G, 0x20, 4, RawNZ(g));
        u32 mat;
        const u32 pk = g.R(100);
        if (pk < 12) mat = 0u;
        else if (pk < 65) { mat = 0; for (u32 i = 0; i < 8; ++i) { mat = pool[g.R(6)]; if (mat) break; } if (!mat) mat = RawNZ(g); }
        else mat = RawNZ(g);
        // el material buscado, a veces el ultimo o el primero de la lista (si esta)
        b.Q(qAppend, 0, g.Pk({0xFFFFFFFFu, 0u, 1u, 2u, 5u, 0xFFFFu, 0x10000u, 0x7FFFFFFFu, 0x12345u, 0xFFFFFFFEu, 0x80000000u}));
        b.ArgP(0, G); b.ArgP(1, T); b.ArgR(2, mat);
        break;
    }
    case kLock: {
        Obj G = b.Alloc(kGeoSize);
        b.W(G, 0xC, 2, g.W() & 0xFFFFu);
        b.W(G, 0x54, 4, g.P(55) ? 0u : RawNZ(g));
        b.ArgP(0, G); b.ArgR(1, g.Pk({0u, 1u, 2u, 3u, 0xFFFu, 0xFFFFFFFFu, g.W(), 0x10000u, 0x10001u, 0xFFFEu}));
        break;
    }
    case kUnlock: {
        const i32 nT = i32(g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 2u, 3u, 0xFFFFFFFFu}));
        const i32 nM = i32(g.Pk({0u, 1u, 2u, 3u, 4u, 2u, 3u, 0xFFFFFFFFu}));
        Obj G = b.Alloc(kGeoSize);
        const u32 flags = g.Pk({0u, 1u, 0u, 1u, 4u, 5u, 0x14u, g.W()});
        b.W(G, 8, 4, flags); b.W(G, 0x10, 4, u32(nT)); b.W(G, 0x24, 4, u32(nM));
        const bool hasMesh = g.P(12);
        b.W(G, 0x54, 4, hasMesh ? RawNZ(g) : 0u);
        const u32 cntM = nM > 0 ? u32(nM) : 0u, cntT = nT > 0 ? u32(nT) : 0u;
        Obj tris = b.Alloc(std::max(cntT, 1u) * 8), mats = b.Alloc(std::max(cntM, 1u) * 4);
        std::vector<Obj> tex; for (int i = 0; i < 3; ++i) tex.push_back(b.Alloc(kTexSize));
        const u32 rasterPool[4]{0u, RawNZ(g), RawNZ(g), RawNZ(g)};
        const u32 pipePool[4]{0u, RawNZ(g), RawNZ(g), RawNZ(g)};
        for (u32 i = 0; i < 3; ++i) b.W(tex[i], 0, 4, rasterPool[g.R(4)]);
        for (u32 i = 0; i < cntM; ++i) {
            Obj m = b.Alloc(kMatSize);
            const u32 tk = g.R(4);
            if (tk == 0) b.W(m, 0, 4, 0); else b.P(m, 0, tex[tk - 1]);
            b.W(m, 8, 4, pipePool[g.R(4)]);
            b.P(mats, 4 * i, m);
        }
        if (cntT > 0 || g.P(85)) b.P(G, 0x2C, tris); else b.W(G, 0x2C, 4, RawNZ(g));
        b.P(G, 0x20, mats);
        for (u32 i = 0; i < cntT; ++i) {
            b.W(tris, 8 * i, 2, g.W() & 0xFFFFu); b.W(tris, 8 * i + 2, 2, g.W() & 0xFFFFu); b.W(tris, 8 * i + 4, 2, g.W() & 0xFFFFu);
            b.W(tris, 8 * i + 6, 2, cntM ? g.R(cntM) : (g.W() & 0xFFFFu));
        }
        const bool needLists = nM > 0;
        for (u32 i = 0; i < 3; ++i) b.Q(qMalloc, i, (needLists && cntT == 0 && g.P(30)) ? 0u : (needLists ? Bld::Tok(b.Alloc(cntM * 4 + 8)) : 0u));
        b.Q(qMesh, 0, g.P(failPct ? 10u : 4u) ? 0u : Bld::Tok(b.Alloc(8)));
        b.Q(qOpt, 0, g.P(failPct ? 15u : 8u) ? 0u : Bld::Tok(b.Alloc(8)));
        b.ArgP(0, G);
        break;
    }
    case kGeoCreate: {
        const u32 numVert = g.Pk({0u, 1u, 2u, 3u, 5u, 7u, 1u, 2u, 0x10000u, 0xFFFFu, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x1000u, 0x80000000u});
        const u32 numTri = g.Pk({0u, 1u, 2u, 3u, 5u, 1u, 2u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x1000u});
        const u32 sets = g.Pk({0u, 0u, 1u, 1u, 2u, 3u, 4u, 8u, 9u, 12u});
        u32 fmt = g.Pk({0u, 0x4u, 0x8u, 0x10u, 0x14u, 0x1Cu, 0x80u, 0x84u, 0x7Bu, 0xFFu, 0x01000000u, 0x01000004u, 0x0F00000Cu, g.W() & 0x0F0000FFu});
        if (sets) fmt |= sets << 16;
        if (g.P(5)) fmt |= (g.W() & 0xF0u) << 8;       // bits intermedios del formato (se ignoran salvo el byte 2)
        const u32 S = g.Pk({0x60u, 0x68u, 0x70u, 0x90u, 0x60u + 4 * g.R(16), 0x60u + g.R(0x40)});
        b.Wabs(kGeoSizeAt, 4, S);
        // tamano del bloque (independiente del banco: solo para dimensionar el bloque devuelto por el falso)
        u32 sz = S;
        u32 nsets = (fmt >> 16) & 0xFFu;
        if (!nsets) nsets = (std::int8_t(fmt & 0xFF) < 0) ? 2u : ((fmt >> 2) & 1u);
        const u32 eff = (fmt & 0x7Bu) | (nsets == 1 ? 4u : (nsets > 1 ? 0x80u : 0u));
        if (!(fmt & 0x01000000u)) { if (eff & 8) sz += numVert * 4; sz += nsets * numVert * 8; sz += numTri * 8; }
        const bool fits = i32(numVert) >= 0 && i32(numTri) >= 0 && numVert < 0x1000 && numTri < 0x1000 && sz <= 0x1800;
        const bool noTri = i32(numTri) <= 0 || numTri == 0;
        Obj blk = b.Alloc(fits ? std::max(sz, 0x70u) + 0x20u : std::max(S, 0x70u) + 0x20u);
        Obj mb = b.Alloc(0x40);
        const bool okAlloc = !g.P(failPct ? 14u : 6u);
        const bool useBlk = okAlloc && (fits || (i32(numVert) >= 0 && i32(numVert) < 0x10000 && numTri == 0));
        b.Q(qMalloc, 0, useBlk ? Bld::Tok(blk) : 0u);
        b.Q(qMalloc, 1, g.P(failPct ? 15u : 7u) ? 0u : Bld::Tok(mb));
        b.Q(qRealloc, 0, g.P(30) ? 0u : Bld::Tok(mb));
        b.Q(qInit, 0, g.P(failPct ? 10u : 5u) ? 0u : 1u);
        (void)noTri;
        b.ArgR(0, numVert); b.ArgR(1, numTri); b.ArgR(2, fmt);
        break;
    }
    case kAddRef: {
        Obj G = b.Alloc(kGeoSize);
        b.W(G, 0xE, 2, g.Pk({0u, 1u, 2u, 0x7FFFu, 0xFFFFu, 0x8000u, g.W() & 0xFFFFu}));
        b.ArgP(0, G);
        break;
    }
    case kGeoDestroy: case kGeoDestroyUnref: {
        Obj G = b.Alloc(kGeoSize);
        b.W(G, 0xE, 2, g.Pk({1u, 1u, 1u, 2u, 3u, 0u, 0xFFFFu, 0x7FFFu, 0x8000u, 0x8001u, 0x10u, g.W() & 0xFFFFu}));
        b.W(G, 0xC, 2, g.W() & 0xFFFFu);
        b.W(G, 0x54, 4, g.P(50) ? 0u : RawNZ(g));
        b.W(G, 0x58, 4, g.P(50) ? 0u : RawNZ(g));
        if (g.P(60)) b.P(G, 0x5C, b.Alloc(0x20)); else b.W(G, 0x5C, 4, 0);
        b.ArgP(0, G);
        break;
    }
    case kGeoRegPlugin: case kMatRegPlugin: {
        for (u32 i = 0; i < 5; ++i) b.ArgR(i, Raw(g));
        break;
    }
    case kGeoRegPluginStream: case kMatRegPluginStream: {
        for (u32 i = 0; i < 4; ++i) b.ArgR(i, Raw(g));
        break;
    }
    case kGeoStreamRead: {
        // flujo (crudo); geometria G que da RpGeometryCreate (falso): sus campos concuerdan, casi siempre, con lo que trae el flujo
        const u32 nV = g.Pk({0u, 1u, 2u, 3u, 4u, 2u, 3u});
        const u32 nT = g.Pk({0u, 1u, 2u, 3u, 4u, 2u});
        const u32 nMT = g.Pk({1u, 1u, 2u, 3u, 1u, 2u, 0u});
        const bool native = g.P(10);
        const u32 sets = g.Pk({0u, 1u, 1u, 2u, 0u, 2u, 3u});
        const bool prelit = g.P(45);
        const bool prelitFmt = g.P(90) ? prelit : !prelit;
        Obj G = b.Alloc(kGeoSize);
        const u32 gflags = (native ? 0x01000000u : 0u) | (prelit ? 8u : 0u) | (g.P(50) ? 0x10u : 0u) | (g.P(30) ? 1u : 0u);
        b.W(G, 8, 4, gflags);
        b.W(G, 0xE, 2, g.Pk({1u, 1u, 1u, 1u, 2u, 3u, 0u, 0xFFFFu}));
        b.W(G, 0xC, 2, g.W() & 0xFFFFu);
        b.W(G, 0x10, 4, nT); b.W(G, 0x14, 4, nV); b.W(G, 0x18, 4, nMT); b.W(G, 0x1C, 4, sets);
        b.W(G, 0x54, 4, g.P(70) ? 0u : RawNZ(g));
        b.W(G, 0x58, 4, g.P(55) ? 0u : RawNZ(g));
        // datos de vertices
        Obj pre = b.Alloc(std::max(nV, 1u) * 4), tris = b.Alloc(std::max(nT, 1u) * 8);
        b.P(G, 0x30, pre); b.P(G, 0x2C, tris);
        std::vector<Obj> tc;
        for (u32 s = 0; s < 4; ++s) { Obj t = b.Alloc(std::max(nV, 1u) * 8); tc.push_back(t); b.P(G, 0x34 + 4 * s, t); }
        Obj mts = b.Alloc(std::max(nMT, 1u) * kMorphSize);
        b.P(G, 0x5C, mts);
        std::vector<Obj> vb;
        for (u32 i = 0; i < std::max(nMT, 1u); ++i) {
            Obj v = b.Alloc(std::max(nV, 1u) * 24 + 4);
            vb.push_back(v);
            b.P(mts, i * kMorphSize + 0x14, v);
            b.P(mts, i * kMorphSize + 0x18, v, std::max(nV, 1u) * 12);
        }
        // datos del flujo
        const u32 ver0 = Ver(Fail());
        b.Q(qFindOk, 0, Fail() ? 0u : 1u); b.Q(qFindVer, 0, ver0);
        const bool props = ver0 == 0x34000u;
        u32 d = 0;
        const u32 fmtWord = (native ? 0x01000000u : 0u) | (prelitFmt ? 8u : 0u) | (sets << 16) | (g.P(40) ? 0x10u : 0u) | (g.P(25) ? 0x4u : 0u);
        b.D(d++, fmtWord); b.D(d++, nT); b.D(d++, nV); b.D(d++, g.P(80) ? nMT : g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu}));
        if (props) { b.D(d++, g.W()); b.D(d++, g.W()); b.D(d++, g.W()); }
        if (!native && nV != 0) {
            if (prelitFmt) for (u32 i = 0; i < nV; ++i) b.D(d++, g.W());
            for (u32 s = 0; s < sets; ++s) for (u32 i = 0; i < nV * 2; ++i) b.D(d++, g.W());
            for (u32 i = 0; i < nT * 2; ++i) b.D(d++, g.W());
        }
        for (u32 m = 0; m < nMT; ++m) {
            for (u32 i = 0; i < 4; ++i) b.D(d++, g.W());
            const u32 hv = g.Pk({0u, 1u, 1u, 1u, 2u}), hn = g.Pk({0u, 1u, 1u, 0u, 0xFFFFFFFFu});
            b.D(d++, hv); b.D(d++, hn);
            const u32 words = hv ? (hn ? nV * 6 : nV * 3) : (hn ? nV * 3 : 0u);
            for (u32 i = 0; i < words; ++i) b.D(d++, g.W());
        }
        b.Q(qFindLen, 0, 0u);
        for (u32 i = 0; i < 16; ++i) { b.Q(qReadOk, i, Fail() ? 0u : 1u); b.Q(qRealOk, i, Fail() ? 0u : 1u); }
        u32 ver1 = Ver(Fail());
        if (!props && ver1 == 0x34000u) ver1 = 0x34001u;      // con version 0x34000 en la lista de materiales las propiedades leidas antes (solo si la del STRUCT era 0x34000) irian sin inicializar
        b.Q(qFindOk, 1, Fail() ? 0u : 1u); b.Q(qFindVer, 1, ver1);
        b.Q(qFindLen, 1, 0u);
        b.Q(qGeoCreate, 0, g.P(failPct ? 5u : 0u) ? 0u : Bld::Tok(G));
        b.Q(qAddMT, 0, Fail() ? 0xFFFFFFFFu : g.Pk({0u, 1u, 2u}));
        b.Q(qMatList, 0, Fail() ? 0u : Bld::Tok(G, 0x20));
        b.Q(qUnlock, 0, Fail() ? 0u : Bld::Tok(G));
        b.ArgR(0, Raw(g));
        break;
    }
    case kRightsRead: {
        b.ArgR(0, RawNZ(g)); b.ArgR(1, g.Pk({8u, 8u, 4u, 0u, 16u, 0xFFFFFFFFu, g.W(), 7u, 9u})); b.ArgR(2, Raw(g)); b.ArgR(3, Raw(g)); b.ArgR(4, Raw(g));
        for (u32 i = 0; i < 4; ++i) { b.Q(qRi32Ok, i, g.P(75) ? 1u : 0u); b.Q(qRi32Val, i, g.Pk({0u, 1u, 0x1Fu, 0xFFFFFFFFu, g.W()})); }
        break;
    }
    case kRightsWrite: {
        Obj mat = b.Alloc(kMatSize), p1 = b.Alloc(kPipeSize), p2 = b.Alloc(kPipeSize);
        b.P(mat, 8, p1);
        if (g.P(35)) { a[kSwapAddr] = Bld::Tok(mat, 8); a[kSwapVal] = Bld::Tok(p2); }
        b.ArgR(0, RawNZ(g)); b.ArgR(1, g.Pk({8u, 8u, 4u, 0u, g.W()})); b.ArgP(2, mat); b.ArgR(3, Raw(g)); b.ArgR(4, Raw(g));
        for (u32 i = 0; i < 4; ++i) b.Q(qWriteOk, i, g.P(75) ? 1u : 0u);
        break;
    }
    case kSetDefProps: {
        if (g.P(25)) b.ArgR(0, 0u);
        else { Obj p = b.Alloc(12); b.ArgP(0, p); }
        break;
    }
    case kMatCreate: {
        b.Q(qAlloc, 0, g.P(15) ? 0u : Bld::Tok(b.Alloc(kMatSize)));
        break;
    }
    case kMatDestroy: {
        Obj m = b.Alloc(kMatSize);
        b.W(m, 0x18, 2, g.Pk({1u, 1u, 1u, 2u, 0u, 3u, 0xFFFFu, 0x8000u, g.W() & 0xFFFFu}));
        if (g.P(60)) b.P(m, 0, b.Alloc(kTexSize)); else b.W(m, 0, 4, g.P(50) ? 0u : RawNZ(g));
        b.ArgP(0, m);
        break;
    }
    case kMatSetTexture: {
        Obj m = b.Alloc(kMatSize), t1 = b.Alloc(kTexSize), t2 = b.Alloc(kTexSize);
        b.W(t1, 0x54, 4, g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, g.W()})); b.W(t2, 0x54, 4, g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, g.W()}));
        const u32 oldk = g.R(100);
        if (oldk < 45) b.P(m, 0, t1); else if (oldk < 70) b.P(m, 0, t2); else if (oldk < 85) b.W(m, 0, 4, 0); else b.W(m, 0, 4, RawNZ(g));
        b.ArgP(0, m);
        const u32 nk = g.R(100);
        if (nk < 40) b.ArgP(1, t1); else if (nk < 80) b.ArgP(1, t2); else b.ArgR(1, 0u);
        break;
    }
    case kMatStreamRead: {
        Obj mat = b.Alloc(kMatSize);
        const u32 len = g.Pk({0x1Cu, 0x1Cu, 0x1Cu, 0x1Cu, 0x1Cu, 0x18u, 0x10u, 0x0Cu, 4u, 0u, 0x1Au});
        const u32 ver0 = Ver(Fail());
        b.Q(qFindOk, 0, Fail() ? 0u : 1u); b.Q(qFindLen, 0, len); b.Q(qFindVer, 0, ver0);
        b.Q(qFindOk, 1, Fail() ? 0u : 1u); b.Q(qFindLen, 1, 0u); b.Q(qFindVer, 1, Ver(Fail()));
        b.Q(qReadOk, 0, Fail() ? 0u : 1u);
        b.D(0, g.W()); b.D(1, g.W()); b.D(2, g.W());
        b.D(3, g.Pk({0u, 1u, 1u, 2u, 0xFFFFFFFFu, g.W(), 0u}));
        b.D(4, g.W()); b.D(5, g.W()); b.D(6, g.W());
        b.Q(qAlloc, 0, g.P(failPct ? 8u : 3u) ? 0u : Bld::Tok(mat));
        b.Q(qTex, 0, g.P(30) ? 0u : Bld::Tok(b.Alloc(kTexSize)));
        MatFreeList();
        if (g.P(15)) { a[kInitRefOn] = 1; a[kInitRefVal] = g.Pk({2u, 0u, 0xFFFFu, 3u, 0x8000u}); }
        b.ArgR(0, Raw(g));
        break;
    }
    }
    // funciones que necesitan la lista libre de materiales
    if (c.fn == kMatCreate || c.fn == kMatDestroy) MatFreeList();
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.ecx = Rand(n, 3) | 1u; c.stack = StackSeed(n);
    MakeAll(c, n);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    std::size_t last = c.a.size();   // los ceros finales de `a` no se vuelcan (el oraculo los repone): reduce el tamano del informe
    while (last > 0 && c.a[last - 1] == 0) --last;
    j.A("a", std::vector<u32>(c.a.begin(), c.a.begin() + last));
    std::vector<u32> flat;
    for (const auto& p : c.pokes) flat.insert(flat.end(), p.begin(), p.end());
    j.A("p", flat);
    j.H("s", c.s);
    return j.End();
}
}

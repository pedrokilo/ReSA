#pragma once
// Fixtures del banco Fn227A (lote 227A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (CON hook del lote 227A, sin validar hasta este banco). 15 funciones del modulo RpWorld de RenderWare (RpClump y registros de plugins de RpAtomic/RpClump):
//   idx  direccion   funcion                                                          convencion
//    0   0x749F70   RpClumpClone(clump)                                              cdecl(1) -> clump o 0
//    1   0x74A1C0   ClumpLightDestroyCallBack(luz, dato)                             cdecl(2) -> luz
//    2   0x74A290   RpClumpCreate()                                                  cdecl(0) -> clump o 0
//    3   0x74A310   RpClumpDestroy(clump)                                            cdecl(1) -> 1
//    4   0x74A490   RpClumpAddAtomic(clump, atomico)                                 cdecl(2) -> clump
//    5   0x74A4C0   RpClumpRemoveAtomic(clump, atomico)                              cdecl(2) -> clump
//    6   0x74A520   RpClumpRemoveLight(clump, luz)                                   cdecl(2) -> clump
//    7   0x74AE50   _rpGeometryListDeinitialize(lista)                               cdecl(1) -> lista
//    8   0x74B060   ClumpAtomicStreamRead(flujo, listaMarcos, listaGeometrias)       cdecl(3) -> atomico o 0
//    9   0x74B420   RpClumpStreamRead(flujo)                                         cdecl(1) -> clump o 0
//   10   0x74BCC0   ClumpAtomicDestroyCallBack(atomico, dato)                        cdecl(2) -> atomico
//   11   0x74BDA0   RpAtomicRegisterPlugin(5 argumentos)                             cdecl(5) -> resultado
//   12   0x74BDD0   RpClumpRegisterPlugin(5 argumentos)                              cdecl(5) -> resultado
//   13   0x74BE00   RpAtomicRegisterPluginStream(4 argumentos)                       cdecl(4) -> resultado
//   14   0x74BE70   RpClumpRegisterPluginStream(4 argumentos)                        cdecl(4) -> resultado
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, ventana de 0x2000, guardas de 0x40) y TODOS los colaboradores falseados salvo los que corren reales (ver LiveComparison.hpp).
// Los objetos del caso (clumps, atomicos, luces, camaras, geometrias, marcos, listas) se escriben con "pokes" sobre un relleno pseudoaleatorio de las arenas (lo no escrito es basura que la funcion no debe leer);
// el oraculo Python repite exactamente los pokes. Familia = n % 16: tipo = familia % 4 (posicion del objeto principal: 0 alineado, 1 desalineado, 2 al borde de la ventana, 3 al inicio) y relleno = familia / 4
// (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
// Los falsos devuelven resultados de COLAS por tipo de colaborador (a[kQ + 16 * cola + i]), consumidas en orden de llamada; el oraculo consume las mismas colas.
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

namespace F227AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 15;
constexpr unsigned kArenas = 4;
constexpr unsigned kSlots = 384;        // dwords de entrada por caso

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
inline constexpr u32 kEntryAddr[kFunctions]{0x749F70, 0x74A1C0, 0x74A290, 0x74A310, 0x74A490, 0x74A4C0, 0x74A520, 0x74AE50, 0x74B060, 0x74B420, 0x74BCC0, 0x74BDA0, 0x74BDD0, 0x74BE00, 0x74BE70};
enum Fn : u32 { kClone, kLightCb, kCreate, kDestroy, kAddAtomic, kRemoveAtomic, kRemoveLight, kGeoDeinit, kAtomicRead, kStreamRead, kAtomicCb, kRegAtomic, kRegClump, kRegAtomicStream, kRegClumpStream };
// Globales del juego que el banco toca
inline constexpr u32 kRightsExtraAt = 0xC92480, kRightsIdAt = 0xC924A8, kModOffAt = 0xC924AC, kCamOffAt = 0xC924D8, kLightOffAt = 0xC924DC, kBenchMarker = 0xA9B0B0, kEngineAt = 0xC97B24;
inline constexpr u32 kMarkerValue = 0x5A;
// Regiones de memoria global vigiladas: R0 = 0xC92480 (4), R1 = 0xC924A8 (8: id de derechos y desplazamiento del modulo), R2 = 0xC924D8 (8: desplazamientos de camara y luz), R3 = marca del banco (1)
inline constexpr std::array<u32, 4> kRegAt{0xC92480u, 0xC924A8u, 0xC924D8u, 0xA9B0B0u};
inline constexpr std::array<u32, 4> kRegBytes{4u, 8u, 8u, 1u};

// Parametros generales (a[...]) y colas. Cola k = a[kQ + 16 k + (i & 15)].
inline constexpr u32 kOffLight = 16, kOffCam = 17;                         // valores de 0xC924DC y 0xC924D8
inline constexpr u32 kFrameListPtr = 24, kFrameListNum = 25;
inline constexpr u32 kRdChunksRet = 26, kRightsId = 27, kRightsExtra = 28;    // ReadDataChunks: resultado y valores que deja en los globales de derechos
inline constexpr u32 kCloneFrame = 29, kErrDelta = 30, kRegRet = 31;
inline constexpr u32 kArgs = 64;                                           // argumentos: 3 dwords por argumento (clase, v1, v2), hasta 6
inline constexpr u32 kQ = 128;
enum QueueId : u32 { qAlloc, qMalloc, qFindOk, qFindLen, qFindVer, qReadCnt, qReadData, qRi32Ok, qRi32Val, qAtom, qGeo, qLight, qCam, qWorld, qClump, qFrameOk, kQueues };

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
    // Anillo de enlaces {next, prev}: cabeza en (head, rel); cada elemento = (objeto, desplazamiento del enlace). Cabeza y elementos quedan circulares
    void Ring(const Obj& head, u32 rel, const std::vector<std::pair<Obj, u32>>& el) {
        const u32 n = u32(el.size());
        if (n == 0) { P(head, rel, head, rel); P(head, rel + 4, head, rel); return; }
        P(head, rel, el[0].first, el[0].second); P(head, rel + 4, el[n - 1].first, el[n - 1].second);
        for (u32 i = 0; i < n; ++i) {
            if (i + 1 < n) P(el[i].first, el[i].second, el[i + 1].first, el[i + 1].second); else P(el[i].first, el[i].second, head, rel);
            if (i > 0) P(el[i].first, el[i].second + 4, el[i - 1].first, el[i - 1].second); else P(el[i].first, el[i].second + 4, head, rel);
        }
    }
};

// Desplazamiento del enlace de plugin dentro de una luz o camara (valores de 0xC924DC / 0xC924D8)
inline u32 PlugOff(Gen& g, u32 salt) { return g.Pk({0x20u, 0x24u, 0x28u, 0x30u, 0x40u, 0x44u, 0x58u, 0x70u, 0x30u + 4 * g.R(0x18), 0x21u + g.R(0x30), 0x10u + salt * 4u}); }

constexpr u32 kClumpSize = 0x38, kAtomSize = 0x58, kGeoSize = 0x68, kMorphSize = 0x28, kFrameSize = 0xB0;

struct Scene {
    Bld& b; Gen& g;
    u32 loff, coff;
    Obj NewClump() { return b.Alloc(kClumpSize); }
    Obj NewAtomic() { return b.Alloc(kAtomSize); }
    Obj NewLight() { return b.Alloc(loff + 0x14); }
    Obj NewCamera() { return b.Alloc(coff + 0x14); }
    Obj NewFrame() { return b.Alloc(kFrameSize); }
    Obj NewGeometry() {
        const Obj geo = b.Alloc(kGeoSize), morph = b.Alloc(kMorphSize);
        b.P(geo, 0x5C, morph);
        return geo;
    }
};

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
    Scene sc{b, g, PlugOff(g, 0), PlugOff(g, 1)};
    const u32 loff = sc.loff, coff = sc.coff;
    a[kOffLight] = loff; a[kOffCam] = coff;
    b.Wabs(kLightOffAt, 4, loff);
    b.Wabs(kCamOffAt, 4, coff);
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
        if (!bad) return g.Pk({0x34000u, 0x36003u, 0x35000u, 0x34001u, 0x36002u, 0x34000u + g.R(0x2004)});
        return g.Pk({0x33FFFu, 0x36004u, 0u, 0xFFFFFFFFu, 0x80000000u, g.R(0x34000), 0x36004u + g.R(0x1000)});
    };
    switch (c.fn) {
    case kClone: {
        Obj S = sc.NewClump(), N = sc.NewClump();
        const u32 nS = g.Pk({0u, 1u, 1u, 2u, 2u, 3u});
        std::vector<std::pair<Obj, u32>> srcAtoms;
        b.W(S, 0x28, 4, RawNZ(g));                                            // callback del origen
        b.W(S, 4, 4, g.P(12) ? 0u : RawNZ(g));                                // marco raiz del origen (crudo: solo se pasa a los falsos)
        for (u32 i = 0; i < nS; ++i) {
            Obj at = sc.NewAtomic();
            srcAtoms.push_back({at, 0x40});
            b.W(at, 0x3C, 4, 0);
            if (g.P(failPct ? 15 : 7)) b.W(at, 4, 4, 0);
            else { Obj fr = sc.NewFrame(); b.W(fr, 0xA0, 4, g.P(10) ? 0u : RawNZ(g)); b.P(at, 4, fr); }
            Obj cl = sc.NewAtomic();
            b.W(cl, 0x14, 4, g.P(50) ? 0u : RawNZ(g));                         // repEntry del clon (para la salida sin marco)
            b.Q(qAtom, i, g.P(failPct ? (failPct > 3 ? 25u : 10u) : 0u) ? 0u : Bld::Tok(cl));
        }
        b.Ring(S, 8, srcAtoms);
        // el clump nuevo (resultado de RpClumpCreate falso): puede traer atomicos, luces y camaras ya enlazados
        std::vector<std::pair<Obj, u32>> pre, lights, cams;
        const u32 nPre = g.P(30) ? g.R(3) : 0u, nL = g.Pk({0u, 0u, 1u, 2u}), nC = g.Pk({0u, 0u, 1u});
        for (u32 i = 0; i < nPre; ++i) { Obj at = sc.NewAtomic(); b.W(at, 0x3C, 4, 0); pre.push_back({at, 0x40}); }
        for (u32 i = 0; i < nL; ++i) { Obj l = sc.NewLight(); lights.push_back({l, loff + 4}); b.P(l, loff, N); }
        for (u32 i = 0; i < nC; ++i) { Obj cm = sc.NewCamera(); cams.push_back({cm, coff + 4}); b.P(cm, coff, N); }
        b.Ring(N, 8, pre); b.Ring(N, 0x10, lights); b.Ring(N, 0x18, cams);
        b.W(N, 4, 4, 0);                                                      // sin marco hasta que se asigne
        b.W(N, 0x28, 4, RawNZ(g));
        b.Q(qClump, 0, g.P(failPct ? 6u : 0u) ? 0u : Bld::Tok(N));
        { Obj cf = sc.NewFrame(); a[kCloneFrame] = g.P(failPct ? 25 : 0) ? 0u : Bld::Tok(cf); }
        b.ArgP(0, S);
        break;
    }
    case kLightCb: {
        Obj cl = sc.NewClump();
        const u32 nL = 1 + g.R(3);
        std::vector<Obj> ls; std::vector<std::pair<Obj, u32>> el;
        for (u32 i = 0; i < nL; ++i) { Obj l = sc.NewLight(); ls.push_back(l); el.push_back({l, loff + 4}); b.P(l, loff, cl); }
        b.Ring(cl, 0x10, el);
        b.ArgP(0, ls[g.R(nL)]); b.ArgR(1, Raw(g));
        break;
    }
    case kCreate: {
        b.Q(qAlloc, 0, g.P(14) ? 0u : Bld::Tok(sc.NewClump()));
        break;
    }
    case kDestroy: {
        Obj cl = sc.NewClump();
        const u32 nA = g.Pk({0u, 1u, 1u, 2u, 3u}), nL = g.Pk({0u, 1u, 1u, 2u, 3u}), nC = g.Pk({0u, 1u, 1u, 2u, 3u});
        std::vector<std::pair<Obj, u32>> at, lt, cm;
        for (u32 i = 0; i < nA; ++i) {
            Obj o = sc.NewAtomic(); at.push_back({o, 0x40});
            b.W(o, 0x14, 4, g.P(50) ? 0u : RawNZ(g));
            if (g.P(70)) b.P(o, 0x18, sc.NewGeometry()); else b.W(o, 0x18, 4, 0);
            if (g.P(70)) b.P(o, 4, sc.NewFrame()); else b.W(o, 4, 4, 0);
            b.Q(qWorld, i, g.Pk({0u, 1u, 1u, 0xFFFFFFFFu, 0x100u, 0x80000000u}));
        }
        for (u32 i = 0; i < nL; ++i) { Obj o = sc.NewLight(); lt.push_back({o, loff + 4}); b.P(o, loff, cl); }
        for (u32 i = 0; i < nC; ++i) { Obj o = sc.NewCamera(); cm.push_back({o, coff + 4}); b.P(o, coff, cl); }
        b.Ring(cl, 8, at); b.Ring(cl, 0x10, lt); b.Ring(cl, 0x18, cm);
        if (g.P(75)) b.P(cl, 4, sc.NewFrame()); else b.W(cl, 4, 4, 0);
        b.ArgP(0, cl);
        break;
    }
    case kAddAtomic: case kRemoveAtomic: {
        Obj cl = sc.NewClump();
        const u32 nA = c.fn == kAddAtomic ? g.Pk({0u, 0u, 1u, 2u, 3u}) : 1 + g.R(3);
        std::vector<std::pair<Obj, u32>> at; std::vector<Obj> os;
        for (u32 i = 0; i < nA; ++i) { Obj o = sc.NewAtomic(); at.push_back({o, 0x40}); os.push_back(o); b.P(o, 0x3C, cl); }
        b.Ring(cl, 8, at);
        if (c.fn == kAddAtomic) { Obj nw = sc.NewAtomic(); b.W(nw, 0x3C, 4, g.P(50) ? 0u : Raw(g)); b.ArgP(0, cl); b.ArgP(1, nw); }
        else { b.ArgP(0, cl); b.ArgP(1, os[g.R(nA)]); }
        break;
    }
    case kRemoveLight: {
        Obj cl = sc.NewClump();
        const u32 nL = 1 + g.R(3);
        std::vector<Obj> ls; std::vector<std::pair<Obj, u32>> el;
        for (u32 i = 0; i < nL; ++i) { Obj l = sc.NewLight(); ls.push_back(l); el.push_back({l, loff + 4}); b.P(l, loff, cl); }
        b.Ring(cl, 0x10, el);
        b.ArgP(0, cl); b.ArgP(1, ls[g.R(nL)]);
        break;
    }
    case kGeoDeinit: {
        Obj list = b.Alloc(8);
        const u32 num = g.Pk({0u, 0u, 1u, 2u, 3u, 5u, 0xFFFFFFFFu, 0xFFFFFFFDu, 0x80000000u});
        if (i32(num) > 0 || g.P(60)) {
            Obj vec = b.Alloc(0x40);
            for (u32 i = 0; i < 8; ++i) b.P(vec, 4 * i, sc.NewGeometry());
            b.P(list, 0, vec);
        } else b.W(list, 0, 4, 0);
        b.W(list, 4, 4, num);
        b.ArgP(0, list);
        break;
    }
    case kAtomicCb: {
        Obj at = sc.NewAtomic();
        b.W(at, 0x14, 4, g.P(50) ? 0u : RawNZ(g));
        if (g.P(70)) b.P(at, 0x18, sc.NewGeometry()); else b.W(at, 0x18, 4, 0);
        if (g.P(70)) b.P(at, 4, sc.NewFrame()); else b.W(at, 4, 4, 0);
        b.Q(qWorld, 0, g.Pk({0u, 1u, 1u, 0xFFFFFFFFu, 0x100u, 0x80000000u}));
        b.ArgP(0, at); b.ArgR(1, Raw(g));
        break;
    }
    case kAtomicRead: {
        // flujo, lista de marcos {puntero, numero}, lista de geometrias {puntero, numero}
        Obj fl = b.Alloc(8), gl = b.Alloc(8), frames = b.Alloc(0x20), geoms = b.Alloc(0x20);
        const u32 nFr = g.Pk({0u, 1u, 3u, 5u, 1u, 3u}), nGe = g.Pk({0u, 0u, 2u, 3u, 1u});
        for (u32 i = 0; i < 8; ++i) { if (g.P(85)) b.P(frames, 4 * i, sc.NewFrame()); else b.W(frames, 4 * i, 4, RawNZ(g)); }
        b.P(fl, 0, frames); b.W(fl, 4, 4, nFr);
        std::vector<Obj> geos;
        for (u32 i = 0; i < 8; ++i) {
            if (g.P(80)) { Obj ge = sc.NewGeometry(); geos.push_back(ge); b.P(geoms, 4 * i, ge); b.W(ge, 0xE, 2, g.Pk({0u, 1u, 0x7FFFu, 0xFFFFu, g.W() & 0xFFFFu})); }
            else b.W(geoms, 4 * i, 4, 0);
        }
        b.P(gl, 0, geoms); b.W(gl, 4, 4, nGe);
        Obj at = sc.NewAtomic();
        // el atomico que da RpAtomicCreate (falso): geometria, marco, repEntry y banderas a la entrada
        if (g.R(10) < 3 && !geos.empty()) b.P(at, 0x18, geos[0]); else b.W(at, 0x18, 4, 0);
        if (g.P(60)) b.P(at, 4, sc.NewFrame()); else b.W(at, 4, 4, 0);
        b.W(at, 0x14, 4, g.P(50) ? 0u : RawNZ(g));
        // datos del flujo: {indice de marco, indice de geometria, banderas, sin uso}; indices en 0..7
        b.Q(qReadData, 0, g.R(8)); b.Q(qReadData, 1, g.R(8)); b.Q(qReadData, 2, g.W() & 0xFFu); b.Q(qReadData, 3, g.W());
        const u32 len = g.Pk({16u, 16u, 16u, 12u, 8u, 4u, 0u, 15u, 16u});
        b.Q(qFindOk, 0, Fail() ? 0u : 1u); b.Q(qFindLen, 0, len); b.Q(qFindVer, 0, Ver(Fail()));
        b.Q(qReadCnt, 0, g.P(okPct) ? len : len ^ 1u);
        b.Q(qAtom, 0, g.P(failPct ? 5u : 0u) ? 0u : Bld::Tok(at));
        // lectura de la geometria desde el flujo (solo con lista de geometrias vacia)
        b.Q(qFindOk, 1, Fail() ? 0u : 1u); b.Q(qFindLen, 1, 0); b.Q(qFindVer, 1, Ver(Fail()));
        { Obj ge = sc.NewGeometry(); b.W(ge, 0xE, 2, g.Pk({0u, 1u, 2u, 0x7FFFu, 0xFFFFu, g.W() & 0xFFFFu})); b.Q(qGeo, 0, Fail() ? 0u : (g.P(30) && !geos.empty() ? Bld::Tok(geos[0]) : Bld::Tok(ge))); }
        b.ArgR(0, Raw(g)); b.ArgP(1, fl); b.ArgP(2, gl);
        break;
    }
    case kStreamRead: {
        // clump nuevo (falso de RpClumpCreate), lista de marcos, geometrias, atomicos, luces y camaras segun las colas
        Obj N = sc.NewClump();
        std::vector<std::pair<Obj, u32>> pre;
        const u32 nPre = g.P(15) ? 1u : 0u;
        for (u32 i = 0; i < nPre; ++i) { Obj at = sc.NewAtomic(); b.W(at, 0x3C, 4, 0); pre.push_back({at, 0x40}); }
        b.Ring(N, 8, pre);
        b.Ring(N, 0x10, {}); b.Ring(N, 0x18, {});
        b.W(N, 4, 4, 0); b.W(N, 0x28, 4, RawNZ(g));
        b.Q(qClump, 0, g.P(failPct ? 5u : 0u) ? 0u : Bld::Tok(N));
        Obj frames = b.Alloc(0x20);
        for (u32 i = 0; i < 8; ++i) { if (g.P(85)) b.P(frames, 4 * i, sc.NewFrame()); else b.W(frames, 4 * i, 4, RawNZ(g)); }
        a[kFrameListPtr] = Bld::Tok(frames); a[kFrameListNum] = g.Pk({0u, 1u, 3u, 5u, 8u});
        const u32 cnt = g.Pk({0u, 1u, 2u, 3u, 1u, 2u, 0xFFFFFFFFu, 0xFFFFFFFBu});
        const u32 nAt = g.Pk({0u, 1u, 2u, 2u, 1u, 0xFFFFFFFFu, 0x80000000u});
        const u32 nLi = g.Pk({0u, 0u, 1u, 2u, 1u, 0xFFFFFFFFu});
        const u32 nCa = g.Pk({0u, 0u, 1u, 2u, 1u, 0xFFFFFFFEu});
        b.Q(qReadData, 0, nAt); b.Q(qReadData, 1, nLi); b.Q(qReadData, 2, nCa); b.Q(qReadData, 3, cnt);
        for (u32 i = 0; i < 16; ++i) {
            b.Q(qFindOk, i, Fail() ? 0u : 1u);
            b.Q(qFindLen, i, g.Pk({12u, 4u, 0u, 16u}));
            b.Q(qFindVer, i, Ver(Fail()));
        }
        b.Q(qReadCnt, 0, g.P(okPct) ? 12u : g.R(12));
        b.Q(qReadCnt, 1, g.P(okPct) ? 4u : g.R(4));
        b.Q(qFrameOk, 0, g.P(okPct) ? 1u : 0u);
        b.Q(qMalloc, 0, g.P(okPct) ? Bld::Tok(b.Alloc(0x40)) : 0u);
        for (u32 i = 0; i < 4; ++i) { Obj ge = sc.NewGeometry(); b.W(ge, 0xE, 2, g.Pk({0u, 1u, 0x7FFFu, 0xFFFFu})); b.Q(qGeo, i, g.P(okPct) ? Bld::Tok(ge) : 0u); }
        for (u32 i = 0; i < 4; ++i) { Obj at = sc.NewAtomic(); b.W(at, 0x3C, 4, 0); b.Q(qAtom, i, g.P(okPct) ? Bld::Tok(at) : 0u); }
        for (u32 i = 0; i < 4; ++i) { Obj l = sc.NewLight(); b.Q(qLight, i, g.P(okPct) ? Bld::Tok(l) : 0u); }
        for (u32 i = 0; i < 4; ++i) { Obj cm = sc.NewCamera(); b.Q(qCam, i, g.P(okPct) ? Bld::Tok(cm) : 0u); }
        for (u32 i = 0; i < 8; ++i) { b.Q(qRi32Ok, i, g.P(okPct) ? 1u : 0u); b.Q(qRi32Val, i, g.R(8)); }
        b.ArgR(0, Raw(g));
        break;
    }
    case kRegAtomic: case kRegClump: case kRegAtomicStream: case kRegClumpStream: {
        const u32 na = (c.fn == kRegAtomic || c.fn == kRegClump) ? 5u : 4u;
        for (u32 i = 0; i < na; ++i) b.ArgR(i, Raw(g));
        break;
    }
    }
    // desplazamiento del modulo de clumps: RwEngineInstance + valor cae en un bloque de la arena con las listas libres (atomicos +0, clumps +4)
    {
        Obj ml = b.Alloc(8);
        b.W(ml, 0, 4, RawNZ(g)); b.W(ml, 4, 4, RawNZ(g));
        b.WabsP(kModOffAt, ml, 0, 2);
    }
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

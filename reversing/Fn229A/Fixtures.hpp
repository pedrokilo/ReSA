#pragma once
// Fixtures del banco Fn229A (lote 229A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (CON hook del lote 229A, sin validar hasta este banco). 23 funciones del modulo RpWorld de RenderWare (lista de materiales bamatlst, geometrias dinamicas y tuberias de D3D9, sectores de BSP y RpWorld):
//   idx  direccion   funcion                                                              convencion
//    0   0x74E010   RpMaterialStreamGetSize(material)                                     cdecl(1) -> u32
//    1   0x74E150   _rpMaterialListDeinitialize(lista)                                    cdecl(1) -> lista
//    2   0x74E350   _rpMaterialListAppendMaterial(lista, material)                        cdecl(2) -> indice o -1
//    3   0x74E450   _rpMaterialListStreamGetSize(lista)                                   cdecl(1) -> u32
//    4   0x74E600   _rpMaterialListStreamRead(flujo, lista)                               cdecl(2) -> lista o 0
//    5   0x74E970   _rpD3D9AddDynamicGeometry(geometria)                                  cdecl(1) -> void
//    6   0x74E9E0   _rpD3D9RemoveDynamicGeometry(geometria)                               cdecl(1) -> void
//    7   0x74EA40   _rpD3D9PluginAttach()                                                 cdecl(0) -> 1/0
//    8   0x74EAB0   _rpD3D9MaterialPipelineOpen()                                         cdecl(0) -> 1
//    9   0x74EAC0   _rpD3D9MaterialPipelineClose()                                        cdecl(0) -> void
//   10   0x74EAD0   _rpD3D9WorldSectorPipelineOpen()                                      cdecl(0) -> 1/0
//   11   0x74EB40   _rpD3D9WorldSectorPipelineClose()                                     cdecl(0) -> void
//   12   0x74EB80   _rpD3D9AtomicPipelineOpen()                                           cdecl(0) -> 1/0
//   13   0x74EC00   _rpD3D9AtomicPipelineClose()                                          cdecl(0) -> void
//   14   0x74ECA0   _rpWorldSectorDeinstanceAll(sector)                                   cdecl(1) -> void
//   15   0x74ED50   _rpWorldSectorDestroyRecurse(sector)                                  cdecl(1) -> void
//   16   0x74EEC0   _rpSectorDefaultRenderCallBack(sector)                                cdecl(1) -> sector o 0
//   17   0x74F210   RpWorldUnlock(mundo)                                                  cdecl(1) -> mundo o 0
//   18   0x74F4E0   RpWorldSectorGetWorld(sector)                                         cdecl(1) -> mundo o 0
//   19   0x74F610   RpWorldDestroy(mundo)                                                 cdecl(1) -> 1
//   20   0x74F760   RpWorldCreate(caja)                                                   cdecl(1) -> mundo o 0
//   21   0x74FBC0   RpWorldForAllMaterials(mundo, callback, dato)                         cdecl(3) -> mundo
//   22   0x74FC70   RpWorldForAllWorldSectors(mundo, callback, dato)                      cdecl(3) -> mundo
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, ventana de 0x2000, guardas de 0x40) y TODOS los colaboradores falseados salvo los que corren reales (ver LiveComparison.hpp).
// Los objetos del caso (listas de materiales, arboles de sectores, mundos, tuberias, nodos) se escriben con "pokes" sobre un relleno pseudoaleatorio de las arenas (lo no escrito es basura que la funcion no debe
// leer); el oraculo Python repite exactamente los pokes. Familia = n % 16: tipo = familia % 4 (posicion del objeto principal: 0 alineado, 1 desalineado, 2 al borde de la ventana, 3 al inicio) y relleno =
// familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
// Los falsos devuelven resultados de COLAS por tipo de colaborador (a[kQ + 16 * cola + i]), consumidas en orden de llamada; el oraculo consume lo mismo.
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

namespace F229AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 23;
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

// Poke: {espacio, desplazamiento, tamano, clase de valor, v1, v2}. espacio 0..3 = arena (desplazamiento respecto a la ventana), 4 = direccion absoluta del juego (desplazamiento = direccion),
// 5 = direccion relativa a RwEngineInstance (desplazamiento = distancia).
// clase 0: valor = v1; 1: puntero a (arena v1, desplazamiento v2); 2: puntero a (arena v1, desplazamiento v2) menos la direccion de RwEngineInstance (desplazamiento de modulo).
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
inline constexpr u32 kEntryAddr[kFunctions]{0x74E010, 0x74E150, 0x74E350, 0x74E450, 0x74E600, 0x74E970, 0x74E9E0, 0x74EA40, 0x74EAB0, 0x74EAC0, 0x74EAD0, 0x74EB40, 0x74EB80, 0x74EC00, 0x74ECA0, 0x74ED50,
                                            0x74EEC0, 0x74F210, 0x74F4E0, 0x74F610, 0x74F760, 0x74FBC0, 0x74FC70};
enum Fn : u32 { kMatStrSize, kMatListDeinit, kMatListAppend, kMatListStrSize, kMatListStrRead, kDynAdd, kDynRemove, kPluginAttach, kMatPipeOpen, kMatPipeClose, kSectPipeOpen, kSectPipeClose,
                kAtomPipeOpen, kAtomPipeClose, kDeinstAll, kDestroyRec, kDefRender, kWorldUnlock, kSectGetWorld, kWorldDestroy, kWorldCreate, kForAllMat, kForAllSect };
// Globales del juego que el banco toca: R0 = 0xC9251C (12: callback anterior de restauracion, cabeza y lista libre de las geometrias dinamicas), R1 = 0xC9254C (4: desplazamiento del modulo de mundos),
// R2 = 0xC9BC60 (4: desplazamiento del modulo de tuberias), R3 = 0x8D62D0 (4: sizeOfStruct del registro de mundos), R4 = 0x8D736C (4: del registro de sectores), R5 = marca del banco (1),
// R6 = RwEngineInstance + 4 (4: mundo actual), R7 = RwEngineInstance + 0x130 (0x20: funciones de memoria del motor, falsos)
inline constexpr u32 kDynPrevCbAt = 0xC9251C, kDynHeadAt = 0xC92520, kDynPoolAt = 0xC92524, kWorldModAt = 0xC9254C, kPipeModAt = 0xC9BC60, kWorldRegAt = 0x8D62D0, kSectorRegAt = 0x8D736C,
                     kBenchMarker = 0xA9B0B0, kEngineAt = 0xC97B24;
inline constexpr u32 kMarkerValue = 0x5A;
inline constexpr std::array<u32, 6> kRegAt{0xC9251Cu, 0xC9254Cu, 0xC9BC60u, 0x8D62D0u, 0x8D736Cu, 0xA9B0B0u};
inline constexpr std::array<u32, 6> kRegBytes{12u, 4u, 4u, 4u, 4u, 1u};

// Parametros generales (a[...]) y colas. Cola k = a[kQ + 16 k + (i & 15)].
inline constexpr u32 kErrDelta = 30;
inline constexpr u32 kArgs = 64;                                           // argumentos: 3 dwords por argumento (clase, v1, v2), hasta 6
inline constexpr u32 kQ = 96;
enum QueueId : u32 { qAlloc, qMalloc, qRealloc, qFindOk, qFindLen, qFindVer, qRi32Ok, qRi32Val, qMatRead, qMatSize, qTexSize, qRegSize, qMesh, qOpt, qFlCreate, qPipeCreate, qLock, qNodeDef, qAddFrag,
                     qPipeUnlock, qBool0, qBool1, qCbVal, qExec, qWUnlock, qCb, qInit, kQueues };
static_assert(kQ + 16 * kQueues <= kSlots);

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
    void WengP(u32 rel, const Obj& t, u32 trel = 0) { c.pokes.push_back({5u, rel, 4u, 1u, t.ar, t.off + trel}); }
    // Ficha de argumento i: clase 1 puntero (arena, desplazamiento), clase 0 crudo, clase 2 callback del banco (v1 = 0 material, 1 sector)
    void ArgP(u32 i, const Obj& o, u32 rel = 0) { c.a[kArgs + 3 * i] = 1; c.a[kArgs + 3 * i + 1] = o.ar; c.a[kArgs + 3 * i + 2] = o.off + rel; }
    void ArgR(u32 i, u32 v) { c.a[kArgs + 3 * i] = 0; c.a[kArgs + 3 * i + 1] = v; c.a[kArgs + 3 * i + 2] = 0; }
    void ArgCb(u32 i, u32 which) { c.a[kArgs + 3 * i] = 2; c.a[kArgs + 3 * i + 1] = which; c.a[kArgs + 3 * i + 2] = 0; }
    // Ficha (token) de un puntero para las colas: la misma forma que la etiqueta de los eventos
    static u32 Tok(const Obj& o, u32 rel = 0) { return 0x10000000u + (o.ar << 20) + u32(kGuard) + o.off + rel; }
    void Q(u32 q, u32 i, u32 v) { c.a[kQ + 16 * q + (i & 15)] = v; }
};

constexpr u32 kWorldSize = 0x70, kSectorSize = 0x88, kPlaneSize = 0x18, kMatSize = 0x1C, kTexSize = 0x58, kPipeSize = 0x34, kEntrySize = 0x10;

// Arbol de sectores: planos (tipo 0, 4, 8, 1...) con dos hijos, sectores con atomos (-1) y de construccion (-2). modo 0: campos de liberacion; 1: solo tipo y malla (RpWorldUnlock completa el resto); 2: solo tipo y malla.
struct TreeBuilder {
    Bld& b; Gen& g;
    u32 mode; bool allowNull, allowBuild;
    int budget;
    std::vector<Obj> leaves;
    TreeBuilder(Bld& b_, Gen& g_, u32 mode_, bool allowNull_, bool allowBuild_, int budget_) : b(b_), g(g_), mode(mode_), allowNull(allowNull_), allowBuild(allowBuild_), budget(budget_) {}
    // lista circular de enlaces de 8 bytes cuya cabeza esta en el sector (rel): count nodos propios
    void Links(const Obj& s, u32 rel, u32 count) {
        std::vector<Obj> nodes;
        for (u32 i = 0; i < count; ++i) nodes.push_back(b.Alloc(8));
        if (nodes.empty()) { b.P(s, rel, s, rel); b.P(s, rel + 4, s, rel); return; }
        b.P(s, rel, nodes[0]); b.P(s, rel + 4, nodes.back());
        for (u32 i = 0; i < count; ++i) {
            if (i + 1 < count) b.P(nodes[i], 0, nodes[i + 1]); else b.P(nodes[i], 0, s, rel);
            if (i) b.P(nodes[i], 4, nodes[i - 1]); else b.P(nodes[i], 4, s, rel);
        }
    }
    void PtrField(const Obj& s, u32 rel, u32 nullPct) {
        if (g.P(nullPct)) b.W(s, rel, 4, 0);
        else if (g.P(30)) b.P(s, rel, b.Alloc(8));
        else b.W(s, rel, 4, RawNZ(g));
    }
    Obj Leaf() {
        Obj s = b.Alloc(kSectorSize);
        b.W(s, 0, 4, 0xFFFFFFFFu);
        if (mode == 0) {
            PtrField(s, 4, 35); PtrField(s, 8, 35); PtrField(s, 0xC, 40);
            for (u32 i = 0; i < 8; ++i) PtrField(s, 0x10 + 4 * i, 55);
            PtrField(s, 0x30, 45); PtrField(s, 0x34, 50); PtrField(s, 0x78, 45);
            Links(s, 0x38, g.Pk({0u, 0u, 1u, 2u, 3u}));
            Links(s, 0x40, g.Pk({0u, 0u, 1u, 2u, 3u}));
        } else if (mode == 1) {
            b.W(s, 0x78, 4, g.P(18) ? RawNZ(g) : 0u);
        } else {
            b.W(s, 0x78, 4, g.P(45) ? RawNZ(g) : 0u);
        }
        leaves.push_back(s);
        return s;
    }
    Obj Make(u32 depth) {
        if (budget <= 1 || depth == 0 || g.P(35)) {
            --budget;
            if (allowBuild && g.P(15)) { Obj s = b.Alloc(0x10); b.W(s, 0, 4, 0xFFFFFFFEu); return s; }
            return Leaf();
        }
        --budget;
        Obj p = b.Alloc(kPlaneSize);
        b.W(p, 0, 4, g.Pk({0u, 4u, 8u, 0u, 4u, 8u, 1u, 2u, 0x7FFFFFFFu}));
        b.W(p, 4, 4, g.W());
        const bool nl = allowNull && g.P(15), nr = allowNull && g.P(20);
        if (nl) b.W(p, 8, 4, 0); else { const Obj l = Make(depth - 1); b.P(p, 8, l); }
        if (nr) b.W(p, 0xC, 4, 0); else { const Obj r = Make(depth - 1); b.P(p, 0xC, r); }
        return p;
    }
};

// ------------------------------------------------------------------------------------------------ generador
inline u32 SizeVal(Gen& g) { return g.Pk({0u, 1u, 0x28u, 0x1Cu, 0x34u, 0xFFFFFFFFu, 0xFFFFFFCCu, 0x7FFFFFFFu, 0x80000000u, g.W(), g.R(0x400)}); }
inline u32 Ver(Gen& g, bool bad) {
    if (!bad) return g.Pk({0x34000u, 0x34000u, 0x36003u, 0x35000u, 0x34001u, 0x36002u, 0x34000u + g.R(0x2004)});
    return g.Pk({0x33FFFu, 0x36004u, 0u, 0xFFFFFFFFu, 0x80000000u, g.R(0x34000), 0x36004u + g.R(0x1000)});
}

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
    a[kErrDelta] = g.P(80) ? 0u : g.Pk({1u, 0xFFFFFFFFu, g.W()});
    const u32 fi = g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 2u, 3u});      // intensidad de fallos de los falsos (0 = nunca)
    const u32 failPct = fi == 0 ? 0u : (fi == 1 ? 3u : (fi == 2 ? 8u : 22u));
    auto Fail = [&] { return g.P(failPct); };
    auto Material = [&](u32 refCountKind) {
        Obj m = b.Alloc(kMatSize);
        b.W(m, 0x18, 2, refCountKind ? g.Pk({0u, 1u, 2u, 0xFFFFu, 0x7FFFu, 0x8000u, g.W() & 0xFFFFu}) : 1u);
        return m;
    };
    // modulo de mundos: [0] lista libre de entradas, [4] cabeza de la lista de mundos (next, prev); entradas {mundo, tamano, next, prev}
    auto WorldModule = [&](u32 count, const std::vector<Obj>& worlds, const std::vector<u32>* sizes) {
        Obj wm = b.Alloc(12);
        b.WabsP(kWorldModAt, wm, 0, 2);
        b.W(wm, 0, 4, g.P(60) ? Bld::Tok(b.Alloc(8)) : RawNZ(g));
        std::vector<Obj> entries;
        for (u32 i = 0; i < count; ++i) entries.push_back(b.Alloc(kEntrySize));
        if (count == 0) { b.P(wm, 4, wm, 4); b.P(wm, 8, wm, 4); }
        else {
            b.P(wm, 4, entries[0], 8); b.P(wm, 8, entries.back(), 8);
            for (u32 i = 0; i < count; ++i) {
                if (i + 1 < count) b.P(entries[i], 8, entries[i + 1], 8); else b.P(entries[i], 8, wm, 4);
                if (i) b.P(entries[i], 0xC, entries[i - 1], 8); else b.P(entries[i], 0xC, wm, 4);
                if (i < worlds.size()) b.P(entries[i], 0, worlds[i]); else b.W(entries[i], 0, 4, RawNZ(g));
                const u32 sz = sizes && i < sizes->size() ? (*sizes)[i] : g.Pk({0x70u, 0x100u, 0x1000u, g.W()});
                b.W(entries[i], 4, 4, sz);
            }
        }
        return wm;
    };
    auto PipeModule = [&](u32 slot40, u32 slot54, u32 slot58) {
        Obj pm = b.Alloc(0x60);
        b.WabsP(kPipeModAt, pm, 0, 2);
        auto S = [&](u32 rel, u32 mode) {
            if (mode == 0) b.W(pm, rel, 4, 0);
            else if (mode == 1) { const Obj p = b.Alloc(kPipeSize); b.P(pm, rel, p); }
            else b.W(pm, rel, 4, RawNZ(g));
        };
        S(0x40, slot40); S(0x54, slot54); S(0x58, slot58);
        return pm;
    };
    // cadena de geometrias dinamicas: nodos {geometria, siguiente}
    auto DynList = [&](u32 count, std::vector<u32>* geoTokens) {
        std::vector<Obj> nodes;
        for (u32 i = 0; i < count; ++i) nodes.push_back(b.Alloc(8));
        if (count == 0) b.Wabs(kDynHeadAt, 4, 0); else b.WabsP(kDynHeadAt, nodes[0], 0, 1);
        for (u32 i = 0; i < count; ++i) {
            const u32 geo = RawNZ(g) | 4u;
            b.W(nodes[i], 0, 4, geo);
            if (geoTokens) geoTokens->push_back(geo);
            if (i + 1 < count) b.P(nodes[i], 4, nodes[i + 1]); else b.W(nodes[i], 4, 4, 0);
        }
    };
    auto PoolPtr = [&] {
        if (g.P(35)) b.Wabs(kDynPoolAt, 4, 0);
        else if (g.P(40)) { const Obj p = b.Alloc(8); b.WabsP(kDynPoolAt, p, 0, 1); }
        else b.Wabs(kDynPoolAt, 4, RawNZ(g));
    };
    switch (c.fn) {
    case kMatStrSize: {
        Obj M = Material(0);
        if (g.P(35)) b.W(M, 0, 4, 0); else if (g.P(65)) { const Obj t = b.Alloc(kTexSize); b.P(M, 0, t); } else b.W(M, 0, 4, RawNZ(g));
        b.Q(qTexSize, 0, SizeVal(g)); b.Q(qRegSize, 0, SizeVal(g));
        b.ArgP(0, M);
        break;
    }
    case kMatListDeinit: {
        Obj L = b.Alloc(12), V = b.Alloc(0x28);
        const u32 nn = g.Pk({0u, 1u, 1u, 2u, 3u, 4u, 5u, 8u, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFFFEu, 7u});
        const bool hasV = g.P(85);
        if (hasV) b.P(L, 0, V); else b.W(L, 0, 4, 0);
        b.W(L, 4, 4, nn); b.W(L, 8, 4, g.Pk({0u, 1u, 0x14u, g.W(), 0xFFFFFFFFu}));
        std::vector<Obj> pool; for (int i = 0; i < 3; ++i) pool.push_back(Material(1));
        for (u32 i = 0; i < 8; ++i) {
            const u32 k = g.R(100);
            if (k < 60) b.P(V, 4 * i, pool[g.R(3)]); else if (k < 75) b.W(V, 4 * i, 4, 0); else b.W(V, 4 * i, 4, RawNZ(g));
        }
        b.ArgP(0, L);
        break;
    }
    case kMatListAppend: {
        Obj L = b.Alloc(12), mat = Material(1), V = b.Alloc(0x40), blk = b.Alloc(0x60);
        const bool hole = g.P(45);
        i32 nm, space;
        if (hole) { nm = i32(g.Pk({0u, 0u, 1u, 2u, 3u, 5u, 0xFFFFFFFFu})); space = nm + 1 + i32(g.R(3)); if (g.P(8)) space = i32(g.Pk({0x7FFFFFFFu, 0x14u})); }
        else { nm = i32(g.Pk({0u, 1u, 2u, 3u, 5u, 8u})); space = g.P(15) ? i32(g.Pk({0xFFFFFFECu, 0x80000000u, 0xFFFFFFFFu})) : nm - i32(g.R(3)); if (g.P(10)) space = nm; }
        if (hole || g.P(60)) b.P(L, 0, V); else b.W(L, 0, 4, 0);
        b.W(L, 4, 4, u32(nm)); b.W(L, 8, 4, u32(space));
        const u32 res = g.P(failPct ? 25u : 10u) ? 0u : Bld::Tok(blk);
        b.Q(qMalloc, 0, res); b.Q(qRealloc, 0, res);
        b.ArgP(0, L); b.ArgP(1, mat);
        break;
    }
    case kMatListStrSize: {
        Obj L = b.Alloc(12), V = b.Alloc(0x28);
        const u32 nn = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 8u, 3u, 4u, 0xFFFFFFFFu, 0x80000000u});
        std::vector<u32> pool; std::vector<Obj> mats;
        for (int i = 0; i < 4; ++i) { mats.push_back(Material(0)); pool.push_back(g.R(100) < 15 ? 0u : (g.P(15) ? RawNZ(g) : 1u)); }
        const u32 np = 1 + g.R(4);
        if (i32(nn) > 0 || g.P(70)) b.P(L, 0, V); else b.W(L, 0, 4, RawNZ(g));
        b.W(L, 4, 4, nn); b.W(L, 8, 4, g.W());
        for (u32 i = 0; i < 8; ++i) {
            const u32 k = g.R(np);
            if (pool[k] == 1u) b.P(V, 4 * i, mats[k]); else b.W(V, 4 * i, 4, pool[k]);
        }
        for (u32 i = 0; i < 16; ++i) b.Q(qMatSize, i, SizeVal(g));
        b.ArgP(0, L);
        break;
    }
    case kMatListStrRead: {
        const i32 N = i32(g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 3u, 2u, 0xFFFFFFFFu, 0xFFFFFFFEu}));
        Obj L = b.Alloc(12);
        std::vector<Obj> mats; for (int i = 0; i < 4; ++i) mats.push_back(Material(1));
        Obj B = b.Alloc(u32(std::max(N, 1)) * 4 + 0x20), I = b.Alloc(0x48);
        for (i32 i = 0; i < std::max(N, 1); ++i) b.P(B, 4 * u32(i), mats[g.R(4)]);
        b.Q(qMalloc, 0, N > 0 ? (g.P(failPct ? 14u : 5u) ? 0u : Bld::Tok(B)) : Bld::Tok(I));
        b.Q(qMalloc, 1, Bld::Tok(I)); b.Q(qMalloc, 2, Bld::Tok(I));
        b.Q(qFindOk, 0, Fail() ? 0u : 1u); b.Q(qFindVer, 0, Ver(g, Fail()));
        b.Q(qRi32Ok, 0, Fail() ? 0u : 1u); b.Q(qRi32Ok, 1, Fail() ? 0u : 1u);
        b.Q(qRi32Val, 0, u32(N));
        for (u32 i = 0; i < 12; ++i) {
            const u32 k = g.R(100);
            b.Q(qRi32Val, 1 + i, k < 60 ? g.Pk({0xFFFFFFFFu, 0x80000000u, 0xFFFFFFFEu}) : (N > 0 ? g.R(u32(N)) : 0u));
        }
        for (u32 i = 0; i < 12; ++i) {
            b.Q(qFindOk, 1 + i, Fail() ? 0u : 1u); b.Q(qFindVer, 1 + i, Ver(g, Fail()));
            b.Q(qMatRead, i, Fail() ? 0u : Bld::Tok(mats[g.R(4)]));
        }
        b.ArgR(0, RawNZ(g)); b.ArgP(1, L);
        break;
    }
    case kDynAdd: {
        std::vector<u32> toks;
        DynList(g.Pk({0u, 0u, 1u, 2u, 3u}), &toks);
        PoolPtr();
        u32 geo = RawNZ(g) | 4u;
        if (!toks.empty() && g.P(45)) geo = toks[g.R(u32(toks.size()))];
        b.Q(qAlloc, 0, Bld::Tok(b.Alloc(8)));
        b.Q(qFlCreate, 0, g.P(15) ? 0u : (g.P(60) ? Bld::Tok(b.Alloc(8)) : RawNZ(g)));
        b.ArgR(0, geo);
        break;
    }
    case kDynRemove: {
        std::vector<u32> toks;
        DynList(g.Pk({0u, 1u, 1u, 2u, 3u, 4u}), &toks);
        PoolPtr();
        u32 geo = RawNZ(g) | 4u;
        if (!toks.empty() && g.P(75)) geo = toks[g.R(u32(toks.size()))];
        b.ArgR(0, geo);
        break;
    }
    case kPluginAttach: {
        b.Q(qBool0, 0, g.Pk({0u, 1u, 1u, 1u, 0xFFFFFFFFu, 0x100u, 0x80000000u, g.W()}));
        b.Q(qBool1, 0, g.Pk({0u, 1u, 1u, 1u, 0xFFFFFFFFu, 0x100u, 0x80000000u, g.W()}));
        b.Q(qCbVal, 0, g.Pk({0u, 0x74EA80u, RawNZ(g), g.W()}));
        break;
    }
    case kMatPipeOpen: case kMatPipeClose: break;
    case kSectPipeOpen: case kAtomPipeOpen: {
        PipeModule(g.P(30) ? 0u : 1u, g.Pk({0u, 1u, 2u}), g.Pk({0u, 1u, 2u}));
        const bool ok1 = !g.P(failPct ? 10u : 4u);
        Obj pipe = b.Alloc(kPipeSize);
        b.Q(qPipeCreate, 0, ok1 ? Bld::Tok(pipe) : 0u);
        b.Q(qLock, 0, g.P(failPct ? 14u : 5u) ? 0u : (g.P(60) ? Bld::Tok(b.Alloc(8)) : RawNZ(g)));
        b.Q(qNodeDef, 0, g.P(50) ? Bld::Tok(b.Alloc(8)) : RawNZ(g));
        b.Q(qAddFrag, 0, g.P(60) ? Bld::Tok(b.Alloc(8)) : RawNZ(g));
        b.Q(qPipeUnlock, 0, g.P(failPct ? 15u : 5u) ? 0u : g.Pk({1u, 1u, 0xFFFFFFFFu, 0x100u, g.W() | 1u}));
        b.Q(qBool0, 0, g.P(failPct ? 20u : 8u) ? 0u : g.Pk({1u, 1u, 0xFFFFFFFFu, 0x100u, 0x80000000u}));
        break;
    }
    case kSectPipeClose: case kAtomPipeClose: {
        const u32 s54 = g.Pk({0u, 1u, 2u}), s58 = g.Pk({0u, 1u, 2u});
        PipeModule(0u, s54, s58);
        if (c.fn == kAtomPipeClose) {
            DynList(g.Pk({0u, 1u, 2u, 3u, 4u}), nullptr);
            PoolPtr();
        }
        break;
    }
    case kDeinstAll: case kDestroyRec: {
        TreeBuilder t(b, g, 0, true, true, 1 + int(g.R(8)));
        Obj root = t.Make(1 + g.R(5));
        b.ArgP(0, root);
        break;
    }
    case kDefRender: {
        Obj s = b.Alloc(kSectorSize), world = b.Alloc(kWorldSize);
        const u32 nt = g.Pk({0u, 0u, 1u, 2u, 0x8000u, 0xFFFFu, 0x100u, g.W() & 0xFFFFu, 0x7FFFu});
        b.W(s, 0x84, 2, nt);
        const u32 sp = g.R(100);
        if (sp < 40) { const Obj p = b.Alloc(kPipeSize); b.P(s, 0x7C, p); } else if (sp < 55) b.W(s, 0x7C, 4, RawNZ(g)); else b.W(s, 0x7C, 4, 0);
        const u32 wp = g.R(100);
        if (wp < 40) { const Obj p = b.Alloc(kPipeSize); b.P(world, 0x6C, p); } else if (wp < 55) b.W(world, 0x6C, 4, RawNZ(g)); else b.W(world, 0x6C, 4, 0);
        b.WengP(4, world);
        PipeModule(g.P(30) ? 0u : (g.P(65) ? 1u : 2u), g.Pk({0u, 1u, 2u}), g.Pk({0u, 1u, 2u}));
        b.Q(qExec, 0, g.Pk({0u, 1u, 1u, 1u, 0xFFFFFFFFu, 0x100u, 0x80000000u, 0x10000u, g.W()}));
        b.ArgP(0, s);
        break;
    }
    case kWorldUnlock: {
        Obj world = b.Alloc(kWorldSize);
        TreeBuilder t(b, g, 1, false, false, 1 + int(g.R(5)));
        Obj root = t.Make(1 + g.R(4));
        const bool noRoot = g.P(6);
        if (noRoot) b.W(world, 0x1C, 4, 0); else b.P(world, 0x1C, root);
        const u32 nM = g.Pk({0u, 1u, 2u, 3u, 4u, 2u, 3u, 0u, 0xFFFFFFFFu});
        b.W(world, 8, 4, g.Pk({0u, 1u, 0u, 1u, 3u, g.W()}));
        const u32 nEntries = 8;
        Obj V = b.Alloc(nEntries * 4 + 0x20);
        b.P(world, 0x10, V);
        b.W(world, 0x14, 4, nM);
        std::vector<Obj> tex; for (int i = 0; i < 3; ++i) tex.push_back(b.Alloc(kTexSize));
        const u32 rasterPool[4]{0u, RawNZ(g), RawNZ(g), RawNZ(g)};
        const u32 pipePool[4]{0u, RawNZ(g), RawNZ(g), RawNZ(g)};
        for (u32 i = 0; i < 3; ++i) b.W(tex[i], 0, 4, rasterPool[g.R(4)]);
        for (u32 i = 0; i < nEntries; ++i) {
            Obj m = b.Alloc(kMatSize);
            const u32 tk = g.R(4);
            if (tk == 0) b.W(m, 0, 4, 0); else b.P(m, 0, tex[tk - 1]);
            b.W(m, 8, 4, pipePool[g.R(4)]);
            b.P(V, 4 * i, m);
        }
        // colas de los colaboradores: mallas (una por hoja visitada con malla nula), tres vectores temporales por hoja, optimizacion
        for (u32 i = 0; i < 12; ++i) b.Q(qMalloc, i, Bld::Tok(b.Alloc(0x40)));
        for (u32 i = 0; i < 4; ++i) b.Q(qMesh, i, g.P(failPct ? 10u : 4u) ? 0u : Bld::Tok(b.Alloc(8)));
        for (u32 i = 0; i < 4; ++i) b.Q(qOpt, i, g.P(failPct ? 15u : 8u) ? 0u : Bld::Tok(b.Alloc(8)));
        // hojas: indice absoluto de material = ventana + matIndex (ventana de 16 bits SIN signo); siempre dentro de las 8 entradas de V
        const bool bigFirst = g.P(12) && !t.leaves.empty();
        for (u32 k = 0; k < t.leaves.size(); ++k) {
            const Obj& s = t.leaves[k];
            if (bigFirst && k == 0) {
                // numTriangles >= 0x8000 con malla nula y BuildMesh nulo: el EXE pasa el valor sin signo y devuelve 0
                b.W(s, 0x78, 4, 0);
                b.W(s, 0x80, 2, g.W() & 0xFFFFu);
                b.W(s, 0x84, 2, g.Pk({0x8000u, 0xFFFFu, 0x8001u, 0xC000u, 0x9000u | (g.W() & 0xFFFu)}));
                b.W(s, 4, 4, RawNZ(g));
                b.Q(qMesh, 0, 0u);
                continue;
            }
            const u32 nt = g.Pk({0u, 1u, 2u, 3u, 4u, 2u, 3u});
            if (g.P(5)) {   // malla ya construida: se salta la hoja aunque numTriangles sea enorme y la ventana desmedida
                b.W(s, 0x78, 4, RawNZ(g)); b.W(s, 0x80, 2, g.W() & 0xFFFFu); b.W(s, 0x84, 2, g.W() & 0xFFFFu); b.W(s, 4, 4, RawNZ(g));
                continue;
            }
            const u32 w = g.Pk({0u, 1u, 2u, 0u});
            b.W(s, 0x80, 2, w);
            b.W(s, 0x84, 2, nt);
            Obj tris = b.Alloc(std::max(nt, 1u) * 8);
            if (nt > 0 || g.P(85)) b.P(s, 4, tris); else b.W(s, 4, 4, RawNZ(g));
            for (u32 i = 0; i < nt; ++i) {
                b.W(tris, 8 * i, 2, g.W() & 0xFFFFu); b.W(tris, 8 * i + 2, 2, g.W() & 0xFFFFu); b.W(tris, 8 * i + 4, 2, g.W() & 0xFFFFu);
                b.W(tris, 8 * i + 6, 2, g.R(5));
            }
        }
        b.ArgP(0, world);
        break;
    }
    case kSectGetWorld: {
        const u32 count = g.Pk({0u, 1u, 1u, 2u, 3u});
        std::vector<Obj> worlds; std::vector<u32> sizes; std::vector<std::vector<Obj>> leafSets;
        for (u32 i = 0; i < count; ++i) {
            worlds.push_back(b.Alloc(kWorldSize));
            const bool single = g.P(45);
            b.W(worlds[i], 3, 1, single ? (g.P(70) ? 1u : 0x81u) : (g.P(70) ? 0u : 0x80u));
            sizes.push_back(g.Pk({0x70u, 0x100u, 0x400u, 0u, 0xFFFFFFF0u, g.W()}));
            std::vector<Obj> leaves;
            if (!single) {
                TreeBuilder t(b, g, 2, false, false, 1 + int(g.R(5)));
                Obj root = t.Make(1 + g.R(3));
                b.P(worlds[i], 0x1C, root);
                leaves = t.leaves;
            } else b.W(worlds[i], 0x1C, 4, RawNZ(g));
            leafSets.push_back(leaves);
        }
        WorldModule(count, worlds, &sizes);
        const u32 pk = g.R(100);
        if (pk < 35 && count > 0) {
            const u32 w = g.R(count);
            if (!leafSets[w].empty()) b.ArgP(0, leafSets[w][g.R(u32(leafSets[w].size()))]);
            else b.ArgP(0, worlds[w], g.Pk({0u, 4u, 0x20u, 0x6Cu, 0x70u, 0xFFFFFFFCu, 0x100u, 0x3FCu}));
        } else if (pk < 70 && count > 0) b.ArgP(0, worlds[g.R(count)], g.Pk({0u, 8u, 0x10u, 0x6Fu, 0x70u, 0x74u, 0xFFFFFFFCu, 0x100u, 0x3FFu, 0x400u}));
        else b.ArgR(0, Raw(g));
        break;
    }
    case kWorldDestroy: {
        Obj world = b.Alloc(kWorldSize);
        const bool single = g.P(40);
        b.W(world, 3, 1, single ? g.Pk({1u, 0x81u, 3u}) : g.Pk({0u, 0x80u, 2u}));
        const bool noRoot = g.P(10);
        if (noRoot) b.W(world, 0x1C, 4, 0);
        else {
            TreeBuilder t(b, g, 2, false, false, 1 + int(g.R(5)));
            Obj root = t.Make(1 + g.R(3));
            b.P(world, 0x1C, root);
        }
        const u32 count = g.Pk({0u, 1u, 1u, 2u, 3u});
        std::vector<Obj> worlds; const u32 mine = g.R(count + 1);
        for (u32 i = 0; i < count; ++i) worlds.push_back(i == mine ? world : b.Alloc(kWorldSize));
        WorldModule(count, worlds, nullptr);
        b.ArgP(0, world);
        break;
    }
    case kWorldCreate: {
        Obj box = b.Alloc(24);
        for (u32 i = 0; i < 6; ++i) b.W(box, 4 * i, 4, g.Pk({0x7F800001u, 0x7FA00000u, 0xFFC00000u, 0x7FC00000u, 0x80000000u, 0x3F800000u, g.W(), g.W(), g.W(), 0xFF800000u}));
        Obj wblk = b.Alloc(0x90), sblk = b.Alloc(0x90);
        b.Wabs(kWorldRegAt, 4, g.Pk({0x70u, 0x78u, 0x100u, 0x80u, g.W(), 0u, 0x90u}));
        b.Wabs(kSectorRegAt, 4, g.Pk({0x88u, 0x90u, 0x100u, 0x8Cu, g.W(), 0u, 0x4u}));
        b.Q(qMalloc, 0, g.P(failPct ? 14u : 5u) ? 0u : Bld::Tok(wblk));
        b.Q(qMalloc, 1, g.P(failPct ? 14u : 6u) ? 0u : Bld::Tok(sblk));
        b.Q(qInit, 0, g.P(50) ? 1u : 0u);
        b.Q(qAlloc, 0, g.P(failPct ? 14u : 8u) ? 0u : Bld::Tok(b.Alloc(kEntrySize)));
        b.Q(qWUnlock, 0, g.P(failPct ? 30u : 20u) ? 0u : g.Pk({1u, 0xFFFFFFFFu, 0x100u, 0x80000000u, g.W() | 1u}));
        std::vector<Obj> worlds; const u32 count = g.Pk({0u, 0u, 1u, 2u});
        for (u32 i = 0; i < count; ++i) worlds.push_back(b.Alloc(kWorldSize));
        WorldModule(count, worlds, nullptr);
        b.ArgP(0, box);
        break;
    }
    case kForAllMat: {
        Obj world = b.Alloc(kWorldSize), V = b.Alloc(0x28);
        const u32 nn = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 8u, 3u, 4u, 0xFFFFFFFFu, 0x80000000u});
        if (i32(nn) > 0 || g.P(60)) b.P(world, 0x10, V); else b.W(world, 0x10, 4, RawNZ(g));
        b.W(world, 0x14, 4, nn);
        for (u32 i = 0; i < 8; ++i) { if (g.P(70)) { const Obj m = Material(0); b.P(V, 4 * i, m); } else b.W(V, 4 * i, 4, g.P(30) ? 0u : RawNZ(g)); }
        const u32 cont = g.Pk({90u, 70u, 40u});
        for (u32 i = 0; i < 16; ++i) b.Q(qCb, i, g.P(cont) ? g.Pk({1u, 1u, 0xFFFFFFFFu, 0x100u, 0x80000000u, g.W() | 1u}) : 0u);
        b.ArgP(0, world); b.ArgCb(1, 0); b.ArgR(2, Raw(g));
        break;
    }
    case kForAllSect: {
        Obj world = b.Alloc(kWorldSize);
        TreeBuilder t(b, g, 2, false, false, 1 + int(g.R(7)));
        Obj root = t.Make(1 + g.R(4));
        b.P(world, 0x1C, root);
        const u32 cont = g.Pk({95u, 80u, 50u});
        for (u32 i = 0; i < 16; ++i) b.Q(qCb, i, g.P(cont) ? g.Pk({1u, 1u, 0xFFFFFFFFu, 0x100u, 0x80000000u, g.W() | 1u}) : 0u);
        b.ArgP(0, world); b.ArgCb(1, 1); b.ArgR(2, Raw(g));
        break;
    }
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

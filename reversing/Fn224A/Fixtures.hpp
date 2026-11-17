#pragma once
// Fixtures del banco Fn224A (lote 224A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (14 entradas, todas cdecl con hook y validated=0 antes del banco; controlador de rasters D3D9 de RenderWare incrustado en el EXE, ver README.md y fixture_oracle.py).
//   idx  direccion  nombre                                   forma
//    0   0x4CB530   RasterListPush(raster)                   alta de un nodo (raster, cabeza) en la lista [0xB4E9FC]; el nodo sale de RwFreeListAlloc por RwEngineInstance+0x144
//    1   0x4CB5E0   RasterListDestroy()                      vacia la lista con RwEngineInstance+0x148 y destruye la lista libre (0x801B80)
//    2   0x4CB640   RasterListReleaseResources()             recorre la lista y suelta los recursos COM de los rasters raiz segun el tipo (tabla 0x4CB6E0)
//    3   0x4CBB00   RasterDestroy(dispositivo, raster, ind)  destruccion del raster D3D9 (etapas 0x7FDE50/0x7FDCD0, 0x4CB560 y 0x4CB730 reales del EXE, 0x730B70, COM)
//    4   0x4CBD80   CheckDepthStencilSurfaceFormat(fmt)      CheckDeviceFormat (uso 2, tipo 1) y CheckDepthStencilMatch (adaptFmt, adaptFmt, fmt)
//    5   0x4CBDE0   CheckTextureFormat(fmt)                  CheckDeviceFormat (uso 0, tipo 3)
//    6   0x4CBE20   CheckRenderTargetTextureFormat(fmt)      CheckDeviceFormat (1, 3) y CheckDepthStencilMatch (adaptFmt, fmt, [0xC9C068])
//    7   0x4CBE80   CheckDepthStencilTextureFormat(fmt)      CheckDeviceFormat (2, 3) y CheckDepthStencilMatch (adaptFmt, adaptFmt, fmt)
//    8   0x4CBF20   CheckRenderTargetCubeFormat(fmt)         CheckDeviceFormat (1, 5) y CheckDepthStencilMatch (adaptFmt, fmt, [0xC9C068])
//    9   0x4CBF80   CheckTextureFormatUsage400(fmt)          sin la bandera 0x40000000 de [0xC9BF0C] devuelve 0; si no CheckDeviceFormat (0x400, 3) == 0
//   10   0x4CBFD0   CheckTextureFormatUsage401(fmt)          idem con uso 0x401
//   11   0x4CC020   CheckCubeFormatUsage400(fmt)             idem con uso 0x400 y tipo 5
//   12   0x4CC150   RasterListInit()                         [0xB4EA00] = RwFreeListCreate(8, 0x7F, 4, 0x30411) (0x801980)
//   13   0x4CC170   RasterListShutdown()                     0x4CB5E0, contadores a cero, libera los datos de la pila de indices (+0x138) y destruye la lista de paletas (0x801B80)
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL, o solo en la direccion
// del EXE cuando la DLL los llama por ella). Las funciones de RwEngineInstance (+0x134 malloc, +0x138 free, +0x13C realloc, +0x144 RwFreeListAlloc, +0x148 RwFreeListFree) se llaman por puntero: el banco
// apunta [0xC97B24] a una estructura falsa. Familia = n % 16: tipo = familia % 4 (posicion del objeto: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno = familia / 4.
//
// Globales del EXE que el banco fija y vigila (cada una es una region de 4 bytes; indice g = orden ascendente de direccion), a[48 + g] = su valor inicial:
//   g: 0 0x8E2428 tipo de dispositivo | 1 0xB4E9E0 desplazamiento de la extension | 2 0xB4E9E8 indices libres de paleta | 3 0xB4E9EC capacidad | 4 0xB4E9F0 siguiente indice | 5 0xB4E9F4 datos de la pila
//      6 0xB4E9F8 lista libre de paletas | 7 0xB4E9FC cabeza de la lista de rasters | 8 0xB4EA00 lista libre de nodos | 9 0xC97B24 RwEngineInstance | 10 0xC97C20 IDirect3D9* | 11 0xC97C24 adaptador
//      12 0xC97C2C superficie de profundidad por defecto | 13 0xC9BEF0 formato del adaptador | 14 0xC9BF0C banderas de capacidades | 15 0xC9C068 formato de profundidad
//
// Especificaciones de puntero: PS(arena, desplazamiento) = ((0xA + arena) << 28) | desplazamiento (el banco la resuelve con la direccion real de la arena, que anota en cada caso); un valor
// con otro nibble alto es un valor crudo (nunca se desreferencia salvo donde el caso lo dice).
//
// Efecto lateral generico de los falsos ("poke"): a[40] = id del evento del falso que lo dispara (0 = ninguno), a[41] = que llamada (0, 1, ...) de ese id, a[42] = tipo de destino (0 global a[43], 1 arena a[43] en
// el desplazamiento a[44], 2 puntero-argumento de la llamada mas a[44]), a[45] = modo del valor (0 crudo a[46], 1 especificacion de puntero a[46]); escribe 4 bytes tras anotar el evento (comprueba el orden de
// las relecturas: cabeza de la lista, pila de paletas, formato del adaptador, vtable...).
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

namespace F224AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 14;
constexpr unsigned kArenas = 4;
constexpr unsigned kWords = 128;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
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
    std::array<u32, kWords> a{};
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

// Desplazamiento de un objeto de `span` bytes en la ventana segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWin);
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar, sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

// Especificacion de puntero a una arena (0 A, 1 B, 2 C, 3 D)
constexpr u32 PS(u32 arena, u32 off) { return ((0xAu + arena) << 28) | off; }
constexpr bool IsPS(u32 v) { const u32 n = v >> 28; return n >= 0xAu && n <= 0xDu; }

// Un dword "interesante" para valores crudos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas ni de la pila del hilo del banco)
inline u32 AvoidStack(u32 v) { return (v >= 0x00800000u && v < 0x02000000u) ? (v ^ 0x40000000u) : v; }
inline u32 NoPS(u32 v) { return IsPS(v) ? (v ^ 0x50000000u) : v; }
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return NoPS(AvoidStack(g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu})));
}
// Un valor crudo distinto de cero
inline u32 RawNZ(Gen& g) { const u32 v = Raw(g); return v ? v : 0xDEADBEEFu; }
inline u32 FormatVal(Gen& g) {
    const u32 w = g.W(), r = g.R(0x100);
    return g.Pk({0x45u, 0x46u, 0x47u, 0x50u, 0x51u, 0x4Du, 0x4Eu, 0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, r, w, 0x15u, 0x16u, 0x32u});
}
inline u32 HrVal(Gen& g) {
    const u32 w = g.W(), r = g.R(8);
    return g.Pk({0u, 0u, 0u, 1u, 2u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x80000000u, 0x8876086Cu, 0x88760868u, 0x80004005u, w, r, 0xFFFFFFFEu});
}

// Clases de funcion (forma)
enum Kind : u32 { kPush, kLDestroy, kRelease, kRDestroy, kCheck, kBump, kInit, kShut };
inline constexpr std::array<Kind, kFunctions> kKind{kPush, kLDestroy, kRelease, kRDestroy, kCheck, kCheck, kCheck, kCheck, kCheck, kBump, kBump, kBump, kInit, kShut};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4CB530, 0x4CB5E0, 0x4CB640, 0x4CBB00, 0x4CBD80, 0x4CBDE0, 0x4CBE20, 0x4CBE80, 0x4CBF20, 0x4CBF80, 0x4CBFD0, 0x4CC020, 0x4CC150, 0x4CC170};
// Bytes que cada funcion toca a partir del objeto principal en la arena A
inline constexpr std::array<u32, kFunctions> kSpan{8, 8, 8, 0xE0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
// Direccion de cada global (g)
inline constexpr std::array<u32, 16> kGlobal{0x8E2428, 0xB4E9E0, 0xB4E9E8, 0xB4E9EC, 0xB4E9F0, 0xB4E9F4, 0xB4E9F8, 0xB4E9FC, 0xB4EA00, 0xC97B24, 0xC97C20, 0xC97C24, 0xC97C2C, 0xC9BEF0, 0xC9BF0C, 0xC9C068};
// Globales (g) que usa cada funcion, en orden ascendente (regiones vigiladas)
inline std::vector<u32> GlobalsOf(u32 fn) {
    switch (kKind[fn]) {
    case kPush: case kLDestroy: return {7, 8, 9};
    case kRelease: return {1, 7, 12};
    case kRDestroy: return {1, 2, 3, 5, 6, 7, 8, 9, 12};
    case kCheck: case kBump: return {0, 10, 11, 13, 14, 15};
    case kInit: return {8};
    default: return {2, 3, 4, 5, 6, 9};
    }
}

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento del objeto principal en A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[36..39] = semillas de las arenas A, B, C y D
//   kPush (0): a[4] = el raster (argumento); el nodo nuevo esta en A en a[0]; a[48+7] = cabeza, a[48+8] = lista libre
//   kLDestroy (1): a[4] = nodos L (0..6) en B desde a[5] con separacion a[6]; a[7] = 1: la cabeza esta a cero aunque haya nodos; a[48+8] = lista libre (la cabeza sale de los nodos)
//   kRelease (2): a[4] = L (0..5) rasters; nodos en B desde a[5] con separacion a[6]; rasters en A desde a[7] con separacion a[8]; a[48+1] = desplazamiento de la extension (E); vtables falsas en C en a[10] y a[11];
//         objetos COM en D desde a[12] (recurso i en a[12] + 0x20*i, objeto del tipo 2 en +0x10); a[48+12] = superficie por defecto; por raster i (p = 64 + 8*i): [p] modo del padre (0 el mismo, 1 crudo
//         a[13], 2 raster + 4), [p+1] cType, [p+2] cFlags, [p+3] modo del recurso (0 nulo, 1 objeto, 2 la superficie por defecto), [p+4] D3DFORMAT (+0x18), [p+5] objeto de +0x1C (0 nulo, 1 objeto),
//         [p+6] vtable del recurso (0 / 1), [p+7] vtable del objeto de +0x1C
//   kRDestroy (3): el raster esta en A en a[0], E = a[48+1]; a[4] = argumento 1 (dispositivo, sin uso), a[5] = modo del padre (0 el mismo, 1 crudo a[27], 2 raster + 4), a[6] = cType, a[7] = cFlags,
//         a[8] = textura de +0 (0 nula, 1 objeto COM en D a[20], 2 la superficie por defecto), a[9] = paleta de +4 (0 nula, 1 bloque en C a[10] con indice a[11] en +0x400), a[12] = D3DFORMAT (+0x18),
//         a[13] = mascara de etapas cuyo raster actual es este (8 bits), a[24] = valor del falso de etapa si no coincide, a[14] = nodos L (0..4) en B desde a[15] con separacion a[16]; a[17] = posicion del raster
//         en la lista (>= L: ausente); a[18] = semilla de los demas rasters de la lista; a[19] = vtable falsa en C; a[21] = datos de la pila de indices en D; a[22], a[23] = lo que devuelven malloc y realloc (en D);
//         a[25], a[26] = segundo bloque de paleta en C (desplazamiento, indice); a[28] = tercer argumento (indicadores, sin uso); a[48+2..8] pila de indices y listas; a[48+12] superficie por defecto
//   kCheck (4..8), kBump (9..11): el objeto IDirect3D9 esta en A en a[0] (vtable 1 en C a[7]); un segundo objeto en D a[9] (vtable 2 en C a[8]); a[4] = formato (argumento), a[5], a[6] = HRESULT de la
//         primera y la segunda consulta; a[48+g] = globales (0 tipo, 10 objeto, 11 adaptador, 13 formato del adaptador, 14 banderas, 15 formato de profundidad)
//   kInit (12): a[4] = lo que devuelve RwFreeListCreate; a[48+8] = lista libre
//   kShut (13): a[48+2..6] = contadores, siguiente indice, datos y lista de paletas
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[36] = c.seed * 13u + 50u; c.a[37] = c.a[36] + 1u; c.a[38] = c.a[36] + 2u; c.a[39] = c.a[36] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu});
    auto& a = c.a;
    // valores iniciales de las globales (los usan solo las funciones que las vigilan)
    for (u32 k = 0; k < 16; ++k) a[48 + k] = Raw(g);
    const auto Aligned = [&](u32 span, u32 low) { return low + 4u * g.R((u32(kWin) - low - span) / 4u); };
    switch (kKind[c.fn]) {
    case kPush: {
        a[4] = g.P(25) ? PS(g.R(4), AuxOffset(g, 8)) : Raw(g);
        a[48 + 7] = g.Pk({0u, PS(1, AuxOffset(g, 8)), PS(0, a[0]), Raw(g), Raw(g), PS(2, AuxOffset(g, 8))});
        a[48 + 8] = Raw(g);
        if (g.P(40)) {
            a[40] = 0xE144; a[41] = 0; a[42] = 0; a[43] = 7; a[45] = g.P(50) ? 1u : 0u;
            a[46] = a[45] ? g.Pk({PS(1, AuxOffset(g, 8)), PS(0, a[0])}) : Raw(g);
        }
        break;
    }
    case kLDestroy: {
        const u32 L = g.Pk({0u, 1u, 1u, 2u, 3u, 4u, 5u, 6u});
        a[4] = L; a[5] = Aligned(0x100, 0x40); a[6] = 4u * (2u + g.R(8)); a[7] = g.P(10) ? 1u : 0u;
        a[48 + 8] = g.P(70) ? RawNZ(g) : 0u;
        if (L && g.P(40)) {
            a[40] = 0xE148; a[41] = g.R(L + 1);
            const u32 m = g.R(3);
            if (m == 0) { a[42] = 0; a[43] = 8; a[45] = 0; a[46] = g.P(50) ? 0u : RawNZ(g); }
            else if (m == 1 && a[41] + 2 < L) {   // el siguiente nodo apunta a un posterior o a nada
                const u32 j = a[41] + 1, k = j + 1 + g.R(L - j - 1);
                a[42] = 1; a[43] = 1; a[44] = a[5] + a[6] * j + 4; a[45] = 1; a[46] = g.P(30) ? 0u : PS(1, a[5] + a[6] * k);
            } else { a[42] = 2; a[44] = g.P(50) ? 0u : 4u; a[45] = 0; a[46] = Raw(g); }
        }
        break;
    }
    case kRelease: {
        const u32 L = g.Pk({0u, 1u, 1u, 2u, 3u, 4u, 5u});
        a[4] = L; a[5] = Aligned(0x100, 0x40); a[6] = 4u * (2u + g.R(8));
        a[48 + 1] = 0x24u + 4u * g.R(0x25) + (g.P(15) ? 1u + g.R(3) : 0u);
        a[7] = 0x40u + 4u * g.R(0x40) + (kind == 1 ? 1u + g.R(3) : 0u); a[8] = a[48 + 1] + 0x28u + 4u * g.R(0x40);   // los rasters no se solapan con la extension del anterior
        a[10] = 0x40u + 4u * g.R(0x80); a[11] = a[10] + 0x80u + 4u * g.R(0x80);
        a[12] = 0x40u + 4u * g.R(0x100); a[13] = RawNZ(g);
        a[48 + 12] = g.Pk({0u, PS(3, 0xE00), PS(3, 0xE00)});
        for (u32 i = 0; i < 6; ++i) {
            const u32 p = 64 + 8 * i;
            a[p] = g.P(75) ? 0u : g.Pk({1u, 2u});
            a[p + 1] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 0u, 1u, 4u, 5u, 6u, 7u, 0xFFu, 0x20u, g.R(256)});
            a[p + 2] = g.R(256);
            a[p + 3] = g.Pk({0u, 1u, 1u, 1u, 2u});
            a[p + 4] = FormatVal(g);
            a[p + 5] = g.Pk({0u, 1u, 1u});
            a[p + 6] = g.R(2); a[p + 7] = g.R(2);
        }
        if (L && g.P(45)) {
            a[40] = g.Pk({0xC008u, 0xC108u}); a[41] = g.R(L + 1);
            const u32 m = g.Pk({0u, 1u, 3u});
            const u32 j = g.R(L);
            if (m == 0) { a[42] = 1; a[43] = 0; a[44] = a[7] + a[8] * j; a[45] = g.P(50) ? 1u : 0u; a[46] = a[45] ? PS(0, a[7] + a[8] * j) : RawNZ(g); }
            else if (m == 1) { a[42] = 1; a[43] = 0; a[44] = a[7] + a[8] * j + 0x20; a[45] = 0; a[46] = g.W(); }
            else if (m == 2) { a[42] = 0; a[43] = 1; a[45] = 0; a[46] = 0x24u + 4u * g.R(0x25); }
            else { a[42] = 0; a[43] = 12; a[45] = 0; a[46] = Raw(g); }
        }
        break;
    }
    case kRDestroy: {
        a[48 + 1] = 0x24u + 4u * g.R(0x25) + (g.P(15) ? 1u + g.R(3) : 0u);
        a[4] = Raw(g);
        a[5] = g.P(80) ? 0u : g.Pk({1u, 2u});
        a[6] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 0u, 1u, 2u, 4u, 5u, 6u, 7u, 0xFFu, g.R(256)});
        a[7] = g.R(256);
        a[8] = g.Pk({0u, 1u, 1u, 1u, 2u});
        a[9] = g.P(55) ? 1u : 0u;
        a[10] = 0x100u + 4u * g.R(0x2A0); a[11] = g.Pk({0u, 1u, 0xFFFFu, 0x10000u, 0x1234u, g.W(), 0xFFFFFFFFu, g.R(0x400)});
        a[12] = FormatVal(g);
        a[13] = g.Pk({0u, 1u, 2u, 4u, 0x80u, 0xFFu, g.R(256), g.R(256)});
        a[14] = g.Pk({0u, 1u, 2u, 3u, 4u});
        a[15] = Aligned(0x100, 0x40); a[16] = 4u * (3u + g.R(6));
        a[17] = g.R(a[14] + 2);
        a[18] = g.W();
        a[19] = 0x40u + 4u * g.R(0x40);
        a[20] = 0x40u + 4u * g.R(0x40);
        a[21] = 0x200u + 4u * g.R(0x80);
        a[22] = 0x600u + 4u * g.R(0x80); a[23] = 0xA80u + 4u * g.R(0x30);
        a[24] = g.P(50) ? 0u : RawNZ(g);
        a[25] = 0x100u + 4u * g.R(0x2A0); a[26] = g.Pk({0u, 3u, 0x7FFFu, 0x20000u, g.W()});
        a[27] = RawNZ(g);
        a[28] = Raw(g);
        // pila de indices de paleta: g 2 = cuenta, 3 = capacidad, 4 = siguiente, 5 = datos
        const u32 sc = g.R(6);
        if (sc == 0) { a[48 + 2] = g.R(0xF0); a[48 + 3] = a[48 + 2] + 1u + g.R(0x10); a[48 + 5] = PS(3, a[21]); }
        else if (sc == 1) { a[48 + 2] = 1u + g.R(0xF0); a[48 + 3] = a[48 + 2]; a[48 + 5] = PS(3, a[21]); }
        else if (sc == 2) { a[48 + 2] = 0; a[48 + 3] = 0; a[48 + 5] = 0; }
        else if (sc == 3) { a[48 + 2] = 0; a[48 + 3] = 0; a[48 + 5] = PS(3, a[21]); }
        else if (sc == 4) { a[48 + 2] = 2u + g.R(0xEE); a[48 + 3] = g.R(a[48 + 2]); a[48 + 5] = PS(3, a[21]); }
        else { a[48 + 2] = g.R(0xF0); a[48 + 3] = a[48 + 2]; a[48 + 5] = 0; }
        a[48 + 4] = Raw(g);
        a[48 + 6] = g.P(90) ? RawNZ(g) : 0u;
        a[48 + 8] = RawNZ(g);
        a[48 + 12] = g.Pk({0u, PS(3, 0xE00), PS(3, 0xE00)});
        if (g.P(55)) {
            a[40] = g.Pk({0x7FDE50u, 0x7FDCD0u, 0xE148u, 0xE148u, 0xE134u, 0xE13Cu, 0x730B70u, 0xC008u, 0xC108u});
            a[41] = g.R(3);
            const u32 m = g.R(8);
            if (m == 0) { a[42] = 1; a[43] = 0; a[44] = a[0]; a[45] = g.P(50) ? 1u : 0u; a[46] = a[45] ? PS(0, a[0]) : RawNZ(g); }
            else if (m == 1) { a[42] = 1; a[43] = 0; a[44] = a[0] + 0x20; a[45] = 0; a[46] = g.Pk({g.W(), 0u, 2u, 5u, 6u, 0x80u, 0x8000u}); }
            else if (m == 2) { a[42] = 1; a[43] = 0; a[44] = a[0] + a[48 + 1] + 4; a[45] = 1; a[46] = PS(2, a[25]); }   // otro bloque de paleta valido (con cero el EXE lee [0 + 0x400])
            else if (m == 3) { a[42] = 0; a[43] = a[48 + 5] ? g.Pk({2u, 3u}) : 6u; a[45] = 0; a[46] = a[43] == 6u ? RawNZ(g) : g.R(0x80); }   // sin datos no se baja la cuenta bajo la capacidad (escribiria en 0)
            else if (m == 4) { a[42] = 0; a[43] = g.Pk({6u, 8u}); a[45] = 0; a[46] = g.P(30) ? 0u : RawNZ(g); }
            else if (m == 5) { a[42] = 0; a[43] = 12; a[45] = g.P(50) ? 1u : 0u; a[46] = a[45] ? PS(3, 0xE00) : g.Pk({0u, RawNZ(g)}); }
            else if (m == 6) { a[42] = 0; a[43] = 7; a[45] = 0; a[46] = 0; if (a[14] > 0 && g.P(50)) { a[45] = 1; a[46] = PS(1, a[15]); } }
            else { a[42] = 0; a[43] = 1; a[45] = 0; a[46] = 0x24u + 4u * g.R(0x25); }
        }
        break;
    }
    case kCheck:
    case kBump: {
        a[4] = g.Pk({FormatVal(g), FormatVal(g), g.R(0x80), 0x15u, 0x16u, 0x32u, 0x1Au});
        a[5] = HrVal(g); a[6] = HrVal(g);
        a[7] = 0x40u + 4u * g.R(0x100); a[8] = 0x600u + 4u * g.R(0x100); a[9] = 0x40u + 4u * g.R(0x100);
        a[48 + 0] = g.Pk({1u, 2u, 3u, 4u, 0u, g.R(8), g.W()});
        a[48 + 10] = PS(0, a[0]);
        a[48 + 11] = g.Pk({0u, 1u, 2u, g.R(8), g.W()});
        a[48 + 13] = g.Pk({0x16u, 0x17u, 0x15u, 0x20u, g.R(0x80), g.W()});
        a[48 + 14] = g.P(50) ? (g.W() | 0x40000000u) : (g.W() & ~0x40000000u);
        a[48 + 15] = g.Pk({0x4Bu, 0x4Du, 0x4Eu, g.R(0x80), g.W()});
        if (g.P(50)) {
            a[40] = 0xC00A; a[41] = 0;
            const u32 m = g.R(8);
            if (m == 0 || m == 1) { a[42] = 0; a[43] = 10; a[45] = 1; a[46] = PS(3, a[9]); }
            else if (m == 2) { a[42] = 0; a[43] = 11; a[45] = 0; a[46] = Raw(g); }
            else if (m == 3) { a[42] = 0; a[43] = 0; a[45] = 0; a[46] = Raw(g); }
            else if (m == 4) { a[42] = 0; a[43] = 13; a[45] = 0; a[46] = Raw(g); }
            else if (m == 5) { a[42] = 0; a[43] = 15; a[45] = 0; a[46] = Raw(g); }
            else if (m == 6) { a[42] = 0; a[43] = 14; a[45] = 0; a[46] = g.W(); }
            else { a[42] = 1; a[43] = 0; a[44] = a[0]; a[45] = 1; a[46] = PS(2, a[8]); }
        }
        break;
    }
    case kInit:
        a[4] = g.Pk({0u, RawNZ(g), RawNZ(g), PS(3, AuxOffset(g, 8)), 1u});
        a[48 + 8] = Raw(g);
        break;
    case kShut: {
        a[48 + 2] = Raw(g); a[48 + 3] = Raw(g); a[48 + 4] = Raw(g);
        a[48 + 5] = g.Pk({0u, RawNZ(g), RawNZ(g), PS(3, AuxOffset(g, 8))});
        a[48 + 6] = g.Pk({0u, RawNZ(g), RawNZ(g)});
        if (g.P(45)) {
            a[40] = g.Pk({0x4CB5E0u, 0xE138u}); a[41] = 0;
            a[42] = 0; a[43] = g.Pk({5u, 6u, 2u, 3u, 4u}); a[45] = 0; a[46] = g.P(40) ? 0u : RawNZ(g);
        }
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
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    j.H("s", c.s);
    return j.End();
}
}

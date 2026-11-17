#pragma once
// Fixtures del banco Fn212A (lote 212A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (CON hook del lote 212A, sin validar hasta este banco). 32 funciones (plantilla: Fn189A para el constructor de CPool y los falsos de new[]/delete; CBDNRenderCB30 para los falsos de RenderWare).
//   idx  direccion  nombre                                                              forma
//    0   0x5D94B0   CCustomCarEnvMapPipeline PoolScalarDeletingDestructorB              thiscall(banderas) `ret 4`: tamano +8 > 0 (con signo): con propietario (+0x10) operator delete 0x8214BD de +0 y +4; +0,+4,+8,+0xC = 0; bit 0 de [esp+4]: delete de this; eax = this
//    1   0x5D9510   CCustomCarEnvMapPipeline PoolScalarDeletingDestructorC              igual (otra copia)
//    2   0x5DA640   RxD3D9InstanceDataRender                                            cdecl(header, mesh): 0x7F9FF0(0), 0x7F9FB0([mesh+0x10]); indexado ([header+8] != 0): 0x7FA320 (6 args) o 0x7FA360 (3 args)
//    3   0x5DA6A0   RxD3D9InstanceDataRenderLighting                                    cdecl(header, mesh, flags, tex): textura y etapa 0 (0x7FDE70, 0x7FC340 x6 o x4) y el dibujo de 0x5DA640 en linea
//    4   0x5DA790   SetEnvMapMaterial                                                   cdecl(surf, color, flags, spec, power): material D3D9 global 0x8D12D8; x87 a 24 bits; estados 0x7FC2D0; 0x7FC430; eax = 1
//    5   0x5DAAC0   SetDefaultSurfaceProperties                                         cdecl(): 0x7FC4D0(0x8D131C, 0x8D1328, 0x20)
//   6..18  0x550180 0x550250 0x550320 0x5503F0 0x5504C0 0x550570 0x550640 0x5506F0 0x5507C0 0x550890 0x550960 0x550A30 0x550B00
//                   constructores de CPool (PtrNodeSingleLink, PtrNodeDoubleLink, EntryInfoNode, Ped, Vehicle, Building, Object, Dummy, ColModel, Task, Event, PointRoute, PatrolRoute):
//                   thiscall(capacidad, nombre) `ret 8`: operator new[] 0x821195 x2 (capacidad * tamano de elemento de 32 bits; capacidad); +0, +4, +0x10 = 1, +8, +0xC = -1; estados = 0x80; eax = this
//  19..31  0x5501E0 0x5502B0 0x550380 0x550450 0x550520 0x5505D0 0x5506A0 0x550750 0x550820 0x5508F0 0x5509C0 0x550A90 0x550B60
//                   cuerpos de destructor de CPool: thiscall() `ret`: tamano +8 > 0: con propietario delete[] 0x8213AE de +0 y +4; +0,+4,+8,+0xC = 0
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
// Familia = n % 16: tipo = familia % 4 (posicion del objeto: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F212AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 32;
constexpr unsigned kArenas = 4;
constexpr unsigned kSlots = 96;         // dwords de entrada por caso

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
    std::array<u32, kSlots> a{};
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
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B, C o D), sin invadir la guarda
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
inline u32 RawNZ(Gen& g) { const u32 v = Raw(g); return v ? v : 0x00C00000u + g.R(0x1000); }

// Un float "interesante" (bits): ceros con signo, unos, bordes, denormales, enormes, infinitos, NaN tranquilos y de senalizacion, aleatorios y valores de color
inline u32 FloatBits(Gen& g, bool allowNaN) {
    const u32 r = g.W();
    const u32 w = g.R(0x10000);
    const u32 pick = g.R(24);
    switch (pick) {
    case 0: return 0u;
    case 1: return 0x80000000u;
    case 2: case 3: return 0x3F800000u;                         // 1.0
    case 4: return 0x3F000000u;                                 // 0.5
    case 5: return 0x40000000u;                                 // 2.0
    case 6: return 0x437F0000u;                                 // 255.0
    case 7: return 0x3B808081u;                                 // 1/255
    case 8: case 9: return r & 0x007FFFFFu;                     // denormal positivo
    case 10: return (r & 0x007FFFFFu) | 0x80000000u;            // denormal negativo
    case 11: return 0x7F7FFFFFu - (r & 0xFFu);                  // casi el mayor float
    case 12: return 0xFF7FFFFFu - (r & 0xFFu);
    case 13: return g.P(50) ? 0x7F800000u : 0xFF800000u;        // infinito
    case 14: return allowNaN ? (0x7FC00000u | (r & 0x003FFFFFu) | (g.P(50) ? 0x80000000u : 0u)) : r;   // NaN tranquilo
    case 15: return allowNaN ? (0x7F800000u | ((r & 0x003FFFFFu) ? (r & 0x003FFFFFu) : 1u) | (g.P(50) ? 0x80000000u : 0u)) : r;   // NaN de senalizacion
    case 16: case 17: return 0x3F800000u + ((r & 0xFFFFu) << 7) - 0x00200000u;   // cerca de 1
    case 18: case 19: return 0x3F000000u + ((r & 0x7FFFFFu));   // [0.5, 1)
    case 20: return 0x43000000u + ((r & 0x7FFFFFu) >> 1);       // 128..
    case 21: return (r & 0x00FFFFFFu) | 0x30000000u;            // pequeno
    default: return r;                                           // cualquier patron
    }
    (void)w;
}
inline bool IsNaNBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x007FFFFFu) != 0; }

// Clases de funcion (forma)
enum Kind : u32 { kScalarDel, kRender, kRenderLit, kEnvMat, kDefSurf, kPoolCtor, kDtorBody };
inline constexpr std::array<Kind, kFunctions> kKind{kScalarDel, kScalarDel, kRender, kRenderLit, kEnvMat, kDefSurf,
    kPoolCtor, kPoolCtor, kPoolCtor, kPoolCtor, kPoolCtor, kPoolCtor, kPoolCtor, kPoolCtor, kPoolCtor, kPoolCtor, kPoolCtor, kPoolCtor, kPoolCtor,
    kDtorBody, kDtorBody, kDtorBody, kDtorBody, kDtorBody, kDtorBody, kDtorBody, kDtorBody, kDtorBody, kDtorBody, kDtorBody, kDtorBody, kDtorBody};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x5D94B0, 0x5D9510, 0x5DA640, 0x5DA6A0, 0x5DA790, 0x5DAAC0,
    0x550180, 0x550250, 0x550320, 0x5503F0, 0x5504C0, 0x550570, 0x550640, 0x5506F0, 0x5507C0, 0x550890, 0x550960, 0x550A30, 0x550B00,
    0x5501E0, 0x5502B0, 0x550380, 0x550450, 0x550520, 0x5505D0, 0x5506A0, 0x550750, 0x550820, 0x5508F0, 0x5509C0, 0x550A90, 0x550B60};
// Tamano del elemento de cada constructor de CPool (indices 6..18), leido del EXE: lea [edi*8], lea [edi+edi*2]*4, lea [edi+edi*4]*4, imul 7C4h, imul 0A18h, imul 38h, imul 19Ch, imul 38h, lea [edi+edi*2]*16, shl 7, imul 44h, imul 64h, imul 1A4h
inline constexpr std::array<u32, 13> kElem{8u, 12u, 0x14u, 0x7C4u, 0xA18u, 0x38u, 0x19Cu, 0x38u, 0x30u, 0x80u, 0x44u, 0x64u, 0x1A4u};
// Bytes que cada funcion toca a partir del puntero primario de la arena A
inline constexpr std::array<u32, kFunctions> kSpan{0x14, 0x14, 0x10, 0x10, 4, 4,
    0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14,
    0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14};

// Memoria global del juego tocada
inline constexpr u32 kMarker = 0xA9B0B0;                 // marca 0x5A de la prueba negativa (funciones sin otra observable global)
inline constexpr u32 kMaterial = 0x8D12D8, kMaterialBytes = 0x44;   // D3DMATERIAL9 global de SetEnvMapMaterial
inline constexpr u32 kAmbientSat = 0x8E2418, kAmbientSatBytes = 0x10;   // AmbientSaturated (RwRGBAReal)

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento del objeto primario en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[92..95] = semillas de las arenas A, B, C y D
//   kScalarDel / kDtorBody: a[4] = modo del puntero +0 (0 crudo, 1 arena B, 2 nulo), a[5] = crudo, a[9] = desplazamiento en B; a[6] = modo del puntero +4 (0 crudo, 1 arena C, 2 nulo), a[7] = crudo, a[11] = desplazamiento en C;
//         a[8] = tamano (+8), a[10] = banderas (dword de pila; solo kScalarDel), a[12] = byte de propiedad (+0x10), a[13] = ultimo libre (+0xC)
//   kPoolCtor: a[4] = capacidad, a[5] = desplazamiento del almacenamiento devuelto (B), a[6] = desplazamiento de los estados devueltos (C), a[7] = nombre crudo, a[8], a[9] = retornos crudos si capacidad <= 0
//   kRender / kRenderLit: la cabecera (0x10 bytes) en A (a[0]), la malla (0x24 bytes) en B (a[5]); a[6] = [header+8] (buffer de indices, crudo), a[7] = [header+0xC] (tipo primitivo), a[8] = [mesh+0x10], a[9..12] = [mesh+0x14], +0x18, +0x1C, +0x20;
//         a[13] = modo de mutacion de los falsos (bit 0: el falso de SetPixelShader reescribe a[14..17] en [header+8], [header+0xC], [mesh+0x10], [mesh+0x14]; bit 1: el de SetVertexShader reescribe a[18..21]
//         en [mesh+0x18], [mesh+0x1C], [mesh+0x20], [header+8]); kRenderLit: a[22] = banderas (dword de pila), a[23] = textura (cruda, o puntero a la arena D si a[24] == 1), a[25] = desplazamiento en D
//   kEnvMat: el color (dword) en A (a[0], a[3] = el color); las propiedades de superficie (12 bytes: ambiente, especular, difuso) en B (a[5]); a[6..8] = bits de ambiente, especular y difuso; a[9] = banderas; a[10] = bits de brillo;
//         a[11] = bits de potencia; a[12..14] = AmbientSaturated r, g, b (bits); a[15] = AmbientSaturated a; a[16..32] = los 17 dwords previos del material global
//   kDefSurf: sin entradas (solo la marca 0x5A)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    auto& a = c.a;
    a[1] = kind;
    a[2] = f / 4;
    a[92] = c.seed * 13u + 50u; a[93] = a[92] + 1u; a[94] = a[92] + 2u; a[95] = a[92] + 3u;
    a[0] = Offset(g, kind, kSpan[c.fn]);
    a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu});
    switch (kKind[c.fn]) {
    case kScalarDel: case kDtorBody:
        a[4] = g.Pk({0u, 1u, 1u, 2u}); a[5] = Raw(g); a[9] = AuxOffset(g, 4);
        a[6] = g.Pk({0u, 1u, 1u, 2u}); a[7] = Raw(g); a[11] = AuxOffset(g, 4);
        a[8] = g.Pk({0u, 1u, 2u, 5u, 5u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFFFEu, g.R(100), g.R(100), g.R(100), g.W(), g.W()});
        a[10] = g.Pk({0u, 1u, 1u, 2u, 3u, 0xFEu, 0xFFFFFFFFu, 0x100u, 0x101u, g.W(), g.W() | 1u});
        a[12] = g.Pk({0u, 1u, 1u, 1u, 0xFFu, 0x80u, 2u, g.W() & 0xFFu});
        a[13] = Raw(g);
        break;
    case kPoolCtor: {
        const i32 cap = i32(g.Pk({0u, 1u, 2u, 3u, 5u, 16u, 50u, 50u, 50u, 63u, 64u, 100u, 255u, 300u, g.R(0x300), g.R(0x300), 0xFFFFFFFFu, 0xFFFFFFFBu, 0x80000000u, 0xF0000000u, 0x80000001u}));
        a[4] = u32(cap);
        a[5] = AuxOffset(g, 0x20);
        a[6] = g.R(4) + 4 * g.R((u32(kWin) - 0x310) / 4);
        a[7] = Raw(g);
        a[8] = Raw(g); a[9] = Raw(g);
        break;
    }
    case kRender: case kRenderLit: {
        a[5] = AuxOffset(g, 0x24);
        a[6] = g.Pk({0u, 0u, 1u, 0xFFFFFFFFu, g.W(), RawNZ(g)});
        a[7] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, g.W(), g.R(8)});
        a[8] = Raw(g);
        for (u32 i = 9; i <= 12; ++i) a[i] = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, g.R(65536), g.R(65536), g.W(), g.W()});
        a[13] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});
        for (u32 i = 14; i <= 21; ++i) a[i] = g.P(25) ? 0u : (i == 14 || i == 21 ? (g.P(50) ? 0u : RawNZ(g)) : g.W());
        if (kKind[c.fn] == kRenderLit) {
            a[22] = g.Pk({0u, 4u, 0x80u, 0x84u, 8u, 0x100u, 0x184u, 0x104u, 0x180u, 0x04u | (g.W() << 8), 0x80u | (g.W() << 8), 0x84u | (g.W() << 8), g.W() & ~0x84u, g.W() | 4u, g.W() | 0x80u, g.W()});
            a[24] = g.Pk({0u, 1u, 1u});
            a[25] = AuxOffset(g, 4);
            a[23] = a[24] == 1 ? 0u : Raw(g);
        }
        break;
    }
    case kEnvMat: {
        a[5] = AuxOffset(g, 12);
        const bool nanSat = g.P(8);
        const u32 ambient = FloatBits(g, true), spec = FloatBits(g, true), diffuse = FloatBits(g, true);
        a[6] = g.P(10) ? 0x3F800000u : ambient;
        a[7] = spec;
        a[8] = diffuse;
        a[9] = g.Pk({0u, 8u, 0x40u, 0x48u, 0x40u, 0x48u, 0x40u, 0x48u, 0xC8u, 0x41u, 0x49u, 0x60u, 0x68u, 0x100u | 0x40u, 0x100u | 0x48u, g.W(), g.W() | 0x40u, g.W() | 0x48u});
        a[10] = FloatBits(g, true);
        a[11] = FloatBits(g, true);
        for (u32 i = 12; i <= 14; ++i) a[i] = FloatBits(g, nanSat);
        a[15] = FloatBits(g, true);
        // la x87 del juego no mezcla dos NaN en un producto de este banco (el oraculo no modela la eleccion entre dos NaN): ambiente o ambiente saturado, no ambos
        if (IsNaNBits(a[6])) for (u32 i = 12; i <= 14; ++i) if (IsNaNBits(a[i])) a[i] = 0x3F000000u + g.R(0x400000);
        for (u32 i = 16; i <= 32; ++i) a[i] = g.W();
        // el color: 0xFFFFFFFF con frecuencia (rama sin modular aunque el indicador este puesto), canales 0/1/255
        const u32 pick = g.R(10);
        const u32 ch0 = g.Pk({0u, 1u, 255u, 128u, g.R(256), g.R(256)}), ch1 = g.Pk({0u, 1u, 255u, g.R(256), g.R(256)}), ch2 = g.Pk({0u, 255u, g.R(256), g.R(256)}), ch3 = g.Pk({0u, 1u, 255u, 255u, g.R(256), g.R(256)});
        a[3] = pick < 2 ? 0xFFFFFFFFu : (pick < 4 ? g.W() : (ch0 | (ch1 << 8) | (ch2 << 16) | (ch3 << 24)));
        break;
    }
    case kDefSurf:
        break;
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

#pragma once
// Fixtures del banco Fn223A (lote 223A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (CON hook del lote 223A, sin validar hasta este banco). 22 funciones (plantillas: Fn221A / Fn173A / Fn171A para el esqueleto del banco; las clases del listado salen del desensamblado, ver PendienteValidar/223A.md):
//   idx  direccion   funcion                                                              convencion
//   0   0x542080   CEntryInfoList::Destructor (salto a Flush 0x536E10)                     thiscall sin pila
//   1   0x465040   CPool<CObject>::GetAt(ref)                                              thiscall(ref) `ret 4`
//   2   0x465070   CPool<CObject>::GetRef(objeto)                                          thiscall(objeto) `ret 4`
//   3   0x4650A0   CDecisionMaker::CDecisionMaker (41 CDecision ctor + 41 SetDefault)       thiscall sin pila
//   4   0x4654B0   CMissionCleanup::RemoveEntityFromList(handle, tipo)                     thiscall(handle, tipo) `ret 8`
//   5   0x46A7E0   CTheScripts::RemoveFromSuppressedCarModels(modelo)                      cdecl(modelo)
//   6   0x46B260   CScriptsForBrains::CScriptsForBrains (llama a Init 0x46A8C0)            thiscall sin pila
//   7   0x474870   CCamera::VectorTrackRunning (+0xC60)  x87                               thiscall sin pila, EAX 0/1
//   8   0x4748A0   CCamera::VectorMoveRunning (+0xCC0)   x87                               thiscall sin pila, EAX 0/1
//   9   0x4748D0   CTaskSimpleSetCharIgnoreWeaponRangeFlag destructor con borrado         thiscall(banderas) `ret 4`
//  10   0x486AE0   rutina vacia cdecl de 6 argumentos                                      cdecl(6)
//  11   0x493360   CTheScripts::MoveSearchLightBetweenTwoPoints  x87                       cdecl(8)
//  12   0x493420   CTheScripts::MoveSearchLightToEntity                                    cdecl(3)
//  13   0x493480   CTheScripts::MoveSearchLightToPointAndStop                               cdecl(5)
//  14   0x4C57A0   CPedModelInfo constructor (cuerpo)                                      thiscall sin pila
//  15   0x4C9990   destruccion de recursos de RxD3D9ResEntryHeader                         cdecl(cabecera)
//  16   0x4C9AB0   _rwD3D9RasterPluginAttach                                               cdecl()
//  17   0x4C9EA0   _rwD3D9TextureHasAlpha                                                  cdecl(textura)
//  18   0x4C9F70   _rwD3D9RasterIsCubeRaster                                               cdecl(raster)
//  19   0x4C9F90   _rwD3D9RasterLock                                                       cdecl(pixeles, raster, modo)
//  20   0x4CA290   _rwD3D9RasterUnlock                                                     cdecl(sin uso, raster)
//  21   0x4CA540   desbloqueo de la paleta de un raster D3D9                               cdecl(sin uso, raster)
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL, o solo en la direccion del EXE
// cuando la DLL los llama por ella). Familia = n % 16: tipo = familia % 4 (posicion del objeto: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4
// (0 aleatorio, 1 ceros, 2 unos, 3 0xCD). Las funciones de D3D9 llaman por la tabla virtual a objetos COM falsos del banco (dispositivo, texturas, superficies) que anotan cada llamada.
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

namespace F223AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 22;
constexpr unsigned kArenas = 4;
constexpr unsigned kSlots = 128;        // dwords de entrada por caso

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
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B, C o D), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
// La pila del hilo del banco (cerca de 0x00EB0000) se etiqueta aparte (Tag): un puntero crudo que caiga ahi se confundiria con un objeto de la pila, asi que ninguno se genera en esa zona
inline u32 AvoidStack(u32 v) { return (v >= 0x00800000u && v < 0x02000000u) ? (v ^ 0x40000000u) : v; }
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return AvoidStack(g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu}));
}
// Un float (como bits) "interesante": normales, ceros, infinitos, NaN, denormales
inline u32 FloatBits(Gen& g) {
    const u32 w = g.W();
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0x7F800001u, 0x00000001u, 0x007FFFFFu, 0x42C80000u, w, w, w & 0xBFFFFFFFu, (w & 0x807FFFFFu) | 0x3F000000u});
}

// Clases de funcion (forma)
enum Kind : u32 { kEntryDtor, kPoolGetAt, kPoolGetRef, kDecision, kMissionRm, kSuppressed, kBrains, kCamera, kSdd, kBoxStub, kSlBetween, kSlEntity, kSlPoint, kPedMI, kHeader, kAttach, kTexAlpha, kIsCube, kLock, kUnlock, kPalette };
inline constexpr std::array<Kind, kFunctions> kKind{kEntryDtor, kPoolGetAt, kPoolGetRef, kDecision, kMissionRm, kSuppressed, kBrains, kCamera, kCamera, kSdd, kBoxStub, kSlBetween, kSlEntity, kSlPoint,
                                                    kPedMI, kHeader, kAttach, kTexAlpha, kIsCube, kLock, kUnlock, kPalette};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x542080, 0x465040, 0x465070, 0x4650A0, 0x4654B0, 0x46A7E0, 0x46B260, 0x474870, 0x4748A0, 0x4748D0, 0x486AE0, 0x493360, 0x493420, 0x493480,
                                                    0x4C57A0, 0x4C9990, 0x4C9AB0, 0x4C9EA0, 0x4C9F70, 0x4C9F90, 0x4CA290, 0x4CA540};
// Bytes del objeto principal que cada funcion toca a partir de su puntero (en la arena A)
inline constexpr u32 SpanOf(Kind k) {
    switch (k) {
    case kPoolGetAt: case kPoolGetRef: return 8u;
    case kDecision: return 0x9A0u;
    case kMissionRm: return 0x25Cu;
    case kCamera: return 0xCC8u;
    case kPedMI: return 0x38u;
    case kHeader: return 0x58u;
    default: return 4u;
    }
}
// Regiones de memoria global del juego que el banco toca (se guardan al empezar y se restauran tras cada lado)
inline constexpr u32 kFmtAt = 0xB4E7E0, kFmtBytes = 0x204;            // tabla de formatos (0xB4E7E0, 0x80 dwords) y desplazamiento de la extension de raster D3D9 (0xB4E9E0)
inline constexpr u32 kExtOffsetAt = 0xB4E9E0;
inline constexpr u32 kDevAt = 0xC97C28, kDevBytes = 12;               // dispositivo D3D9 (0xC97C28) y superficie de dibujo (0xC97C30)
inline constexpr u32 kSuppAt = 0xA44940, kSuppBytes = 0xA0;           // CTheScripts::SuppressedVehicleModels (40 enteros)
inline constexpr u32 kLightsAt = 0xA94D68, kLightBytes = 8 * 0x7C;    // ScriptSearchLightArray
inline constexpr u32 kTimerAt = 0xB7CB84;                             // CTimer::m_snTimeInMilliseconds
inline constexpr u32 kPoolsAt = 0xB74490, kPoolsBytes = 0x10;         // punteros de los pools de peatones (0xB74490), vehiculos (0xB74494) y objetos (0xB7449C)
inline constexpr u32 kBenchMarker = 0xA9B0B0;
inline constexpr u32 kMarkerValue = 0x5A;                             // marca que solo pone el banco (el juego real usa 0 o 1): la prueba negativa condiciona sus mutaciones a ella
inline constexpr std::array<u32, 7> kRegAt{kFmtAt, kDevAt, kSuppAt, kLightsAt, kTimerAt, kPoolsAt, kBenchMarker};
inline constexpr std::array<u32, 7> kRegBytes{kFmtBytes, kDevBytes, kSuppBytes, kLightBytes, 4u, kPoolsBytes, 1u};
// Direcciones del EXE de los colaboradores que anotan eventos
inline constexpr u32 kEvFlush = 0x536E10, kEvDecCtor = 0x6040C0, kEvDecSet = 0x600530, kEvMoving = 0x542800, kEvBrainsInit = 0x46A8C0, kEvTaskDtor = 0x61A3A0, kEvTaskDelete = 0x61A5B0;
inline constexpr u32 kEvThingIdx = 0x4839A0, kEvCleanUp = 0x571A00, kEvRegRef = 0x571B70, kEvBaseModel = 0x4C4A60;
inline constexpr u32 kEvDynGeo = 0x74E9E0, kEvIndexBuf = 0x730D30, kEvDecl = 0x7FAC10, kEvVtxDestroy = 0x7F56A0, kEvDynVtx = 0x7F5AE0, kEvRegPlugin = 0x7FB0B0, kEvMipmaps = 0x7F3CD0;
// Ids de los objetos COM falsos (los eventos anotan el id del objeto y no su direccion)
enum ComId : u32 { kComDevice, kComTexture, kComCube, kComRenderSurface, kComSwapSurface, kComLevel0, kComLevel1, kComLevel2, kComLevel3, kComOffscreen0, kComOffscreen1, kComCount };

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento del objeto principal en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[90] = semilla de las regiones globales,
//   a[92..95] = semillas de las arenas A, B, C y D
//   kEntryDtor: a[4] = EAX del falso de Flush, a[5] = dword que deja en [this]
//   kPoolGetAt: [objeto principal] = {objetos, banderas}; a[4] = ranura (con signo, 24 bits), a[5] = byte de banderas de la ranura, a[6] = desplazamiento en B del byte de banderas, a[7] = objetos (crudo),
//         a[8] = 0 objetos crudo a[7] / 1 puntero a la arena C + a[9], a[10] = 1 el byte bajo de la referencia coincide con el de banderas / 0 no (a[11] = byte bajo en ese caso)
//   kPoolGetRef: a[4] = ranura, a[5] = byte de banderas, a[6] = desplazamiento en B, a[7] = resto (0..411), a[8] = 0 objeto crudo a[9] / 1 puntero a C + a[9]
//   kDecision: a[4] / a[5] = bases de los dwords que dejan los falsos del constructor (en [this]) y de SetDefault (en [this + 4]) xor el numero de llamada
//   kMissionRm: a[4] = handle, a[5] = tipo (dword; solo cuenta el byte bajo), a[6] = ranura, a[7] = byte de banderas, a[8] = desplazamiento de la entidad en D, a[9] = desplazamiento de las banderas en C,
//         a[10] = dword de banderas de la entidad (+0x1C), a[11] = contador inicial, a[12..14] = desplazamientos en B de las estructuras de pool de vehiculos, peatones y objetos, a[15] = semilla de las 75 entradas;
//         s = las 75 entradas (8 bytes: tipo, 3 bytes de relleno, handle)
//   kSuppressed: a[4] = modelo, a[8 + i] = SuppressedVehicleModels[i] (40 enteros)
//   kBrains: a[4] = dword que el falso de Init deja en [this]
//   kCamera: a[4] = tiempo (entero con signo en 0xB7CB84), a[5] = fin de seguimiento (+0xC60, bits float), a[6] = fin de movimiento (+0xCC0)
//   kSdd: a[4] = banderas (pila), a[5] = dword que deja el falso de CTask::Destructor en [this]
//   kBoxStub: a[4..9] = argumentos
//   kSlBetween/kSlEntity/kSlPoint: a[4] = referencia, a[5] = indice que devuelve el falso (con signo), a[6] = referencia previa (+0x48), a[7] y a[8] = destino (+0x14, +0x18; bits float);
//         kSlBetween a[9..15] = x1 y1 z1 x2 y2 z2 velocidad; kSlEntity a[9] = entidad, a[10] = velocidad; kSlPoint a[9..12] = x y z velocidad
//   kPedMI: a[4] = dword que el falso del constructor base deja en [this], a[5] = dword que deja en [this + 0x34]
//   kHeader: a[4] = 0 sin objeto asociado / 1 objeto en B + a[5], a[6] = tipo (byte), a[7] = dword +0x18 del objeto, a[8] = bufer de indices, a[9] = declaracion, a[10] = cuenta (+0x54);
//         a[11 + 6 i ..] = flujo i: bufer, desplazamiento, paso, banderas (16 bits), managed, libre
//   kAttach: a[4] = lo que devuelve el falso de RwRasterRegisterPlugin
//   kTexAlpha: a[4] = 0 raster nulo / 1 raster en B + a[5], a[6] = desplazamiento de la extension, a[7] = byte alfa (+8), a[8] = byte +9
//   kIsCube: a[5] = desplazamiento del raster en B, a[6] = desplazamiento de la extension, a[7] = byte +9
//   kLock/kUnlock/kPalette: cadena de rasters R0 (A + a[0]), R1 (B + a[5]), R2 (C + a[6]), R3 (D + a[7]) con profundidad a[4] (0..3, el ultimo es su propio padre); a[8] = desplazamiento de la extension;
//         a[9] = cpPixels, a[10] = cType, a[11] = privateFlags, a[12] = cFormat, a[13] = ancho, a[14] = alto, a[15] = nOffsetX, a[16] = nOffsetY, a[17] = ancho original, a[18] = alto original, a[19] = paso,
//         a[20] = modo de bloqueo, a[21] = byte +9 de la extension del raster superior, a[22] = cadena de intercambio del superior, a[23] = byte +9 de R0, a[24] = byte +0xA de R0, a[25] = nivel (+0xB) de R0,
//         a[26] = superficie bloqueada de R0 (0..5 = ids 5..10), a[27] = HRESULT de GetSurfaceLevel, a[28] = superficie que devuelve (0..3), a[29] = HRESULT de LockRect, a[30] = paso del bloqueo, a[31] = bits del bloqueo,
//         a[32] = HRESULT de CreateOffscreenPlainSurface, a[33] = superficie que crea (0..1), a[34..41] = D3DSURFACE_DESC, a[42] = desplazamiento en C de *pixeles, a[43] = desplazamiento en C de la paleta,
//         a[44] = numero de paleta ([paleta + 0x400]), a[45] = paleta de la extension de R0 (0 nula / 1 en C), a[46] = paleta de R0 (+8), a[47] = primer argumento (sin uso)
inline u32 NiceFloat(Gen& g) {
    if (g.P(55)) { const float f = float(int(g.R(2001)) - 1000) / float(1u << g.R(4)); u32 b; std::memcpy(&b, &f, 4); return b; }
    return FloatBits(g);
}
inline u32 Hr(Gen& g) { return g.Pk({0u, 0u, 0u, 0u, 1u, 0x80004005u, 0xFFFFFFFFu, 0x8876086Cu, 0x7FFFFFFFu, 0x80000000u, g.W(), g.R(100)}); }
inline u32 Dim(Gen& g) { return g.Pk({0u, 1u, 2u, 3u, 7u, 16u, 64u, 255u, 256u, 1024u, 0xFFFFFFFFu, 0x80000000u, g.R(4096), g.R(4096), g.R(4096), g.W()}); }
inline u32 FloatOf(i32 v) { const float f = float(v); u32 b; std::memcpy(&b, &f, 4); return b; }
inline void MakeRaster(Gen& g, C& c, Kind k, u32 kind) {
    auto& a = c.a;
    const u32 X = g.Pk({0x30u, 0x34u, 0x38u, 0x40u, 0x44u, 0x50u, 0x7Cu, 0x100u, 0x30u + g.R(0x300), 0x30u + 4 * g.R(0x80), 0x32u + g.R(0x100)});
    a[8] = X;
    a[0] = Offset(g, kind, X + 0x28);
    a[4] = g.Pk({0u, 1u, 1u, 2u, 3u});
    a[5] = AuxOffset(g, X + 0x28); a[6] = AuxOffset(g, X + 0x28); a[7] = AuxOffset(g, X + 0x28);
    const u32 t = g.Pk({0u, 0u, 4u, 4u, 2u, 2u, 5u, 5u, 1u, 3u, 6u, 7u, g.R(8)});
    a[10] = g.P(25) ? ((g.W() & 0xF8u) | t) : t;
    a[11] = g.Pk({0u, 4u, 0x10u, 0x14u, 2u, 6u, 0x1Cu, 0xFFu, 0x18u, g.W() & 0xFFu, g.W() & 0xFFu, 0xE7u, 0xF9u});
    a[12] = g.Pk({0u, 0x10u, 0x10u, 0xFFu, 0x20u, g.W() & 0xFFu, g.W() & 0xFFu});
    a[13] = Dim(g); a[14] = Dim(g);
    a[15] = g.Pk({0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, g.W() & 0xFFFFu, g.R(256)}); a[16] = g.Pk({0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, g.W() & 0xFFFFu, g.R(256)});
    a[17] = Dim(g); a[18] = Dim(g);
    a[19] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.R(8192)});
    if (k == kLock) {
        a[9] = g.P(78) ? 0u : Raw(g);
        const u32 mip = g.Pk({0u, 0u, 1u, 2u, 3u, 4u, 5u, 0xFFu, 31u, 32u, 33u, g.R(256), g.R(8)});
        a[20] = (mip << 8) | g.Pk({0u, 1u, 2u, 3u, 3u, 1u}) | (g.P(15) ? (g.W() & 0xFFFF00FCu) : 0u) | (g.P(10) ? (g.W() & 0xFFFF0000u) : 0u);
        a[20] = (a[20] & 0xFFFF00FFu) | (mip << 8);   // el byte 1 es el nivel de mip
    } else {
        a[9] = g.P(88) ? (Raw(g) | 4u) : 0u;
        a[20] = g.W();
    }
    a[21] = g.Pk({0u, 0u, 0u, 0x10u, 0x01u, 0x11u, 0x25u, 0x0Fu, g.W() & 0xFFu, g.W() & 0xFFu});
    a[22] = g.Pk({0u, 0u, 1u, Raw(g), 0xFFFFFFFFu});
    a[23] = g.W() & 0xFFu; a[24] = g.Pk({0u, 0u, 1u, 0x10u, 0x0Fu, 0xF0u, g.W() & 0xFFu}); a[25] = g.Pk({0u, 0u, 0u, 1u, 2u, 0xFFu, g.W() & 0xFFu});
    a[26] = g.R(6);
    a[27] = Hr(g); a[28] = g.R(4); a[29] = Hr(g); a[30] = g.Pk({0u, 4u, 64u, 256u, 0xFFFFFFFFu, g.W(), g.R(8192)}); a[31] = g.Pk({0u, Raw(g), g.W(), 0x10000000u});
    a[32] = Hr(g); a[33] = g.R(2);
    for (u32 i = 0; i < 8; ++i) a[34 + i] = (i == 6 || i == 7) ? Dim(g) : g.Pk({0u, 1u, 21u, 22u, 23u, 0x32315644u, g.W(), g.R(4)});
    a[42] = AuxOffset(g, 4); a[43] = AuxOffset(g, 0x404);
    a[44] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.R(256)});
    a[45] = g.P(80) ? 1u : 0u; a[46] = g.Pk({0u, 0u, Raw(g)}); a[47] = Raw(g);
}
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    auto& a = c.a;
    const Kind k = kKind[c.fn];
    a[1] = kind;
    a[2] = f / 4;
    a[90] = c.seed * 7u + 11u;
    a[92] = c.seed * 13u + 50u; a[93] = a[92] + 1u; a[94] = a[92] + 2u; a[95] = a[92] + 3u;
    a[0] = Offset(g, kind, SpanOf(k));
    a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu});
    switch (k) {
    case kEntryDtor:
        a[4] = Raw(g); a[5] = g.Pk({0u, g.W(), 0xA5A5A5A5u});
        break;
    case kPoolGetAt: {
        const u32 slot = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x7FFFFFu, 0xFF800000u, g.R(100), g.R(0x1000), u32(-i32(g.R(0x1000))), u32(i32(g.R(0x1000000)) - 0x800000)});
        a[4] = slot;
        a[5] = g.Pk({0u, 1u, 0x80u, 0xFFu, 0x7Fu, g.W() & 0xFFu, g.W() & 0xFFu});
        a[6] = AuxOffset(g, 4);
        a[7] = Raw(g);
        a[8] = g.P(35) ? 1u : 0u; a[9] = AuxOffset(g, 4);
        a[10] = g.P(60) ? 1u : 0u; a[11] = g.W() & 0xFFu;
        break;
    }
    case kPoolGetRef: {
        a[4] = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x7FFFu, 0xFFFF8000u, g.R(100), g.R(0x1000), u32(-i32(g.R(0x1000))), u32(i32(g.R(0x100000)) - 0x80000)});
        a[5] = g.Pk({0u, 1u, 0x80u, 0xFFu, 0x7Fu, g.W() & 0xFFu, g.W() & 0xFFu});
        a[6] = AuxOffset(g, 4);
        a[7] = g.Pk({0u, 1u, 411u, 410u, 206u, g.R(412), g.R(412), g.R(412)});
        a[8] = g.P(35) ? 1u : 0u; a[9] = a[8] ? AuxOffset(g, 4) : Raw(g);
        break;
    }
    case kDecision:
        a[4] = g.W(); a[5] = g.W();
        break;
    case kMissionRm: {
        const u32 type = g.Pk({1u, 1u, 2u, 2u, 3u, 3u, 0u, 4u, 5u, 0xFFu, 0x101u, 0x102u, 0x103u, 0x203u, g.W() & 0xFFu, g.W()});
        a[5] = type;
        const u32 low = type & 0xFFu;
        const u32 slot = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, 0xFFFFFFFEu, g.R(100), g.R(0x7FFF), u32(-i32(g.R(0x1000)))});
        a[6] = slot;
        a[7] = g.Pk({0u, 1u, 0x80u, 0xFFu, g.W() & 0xFFu, g.W() & 0xFFu});
        const u32 matchFlag = g.P(75) ? 1u : 0u;
        const u32 hlow = matchFlag ? a[7] : ((a[7] + 1u + g.R(255)) & 0xFFu);
        const bool poolType = low >= 1 && low <= 3;   // solo los tipos con pool buscan la entidad: su handle lleva siempre una ranura coherente con el banco
        a[4] = (!poolType && g.P(40)) ? g.Pk({0u, 0xFFFFFFFFu, g.W()}) : ((slot << 8) | hlow);
        a[8] = AuxOffset(g, 0x20); a[9] = AuxOffset(g, 4);
        a[10] = g.Pk({0u, 0x40000u, 0x40000u, 0x40000u, 4u, 0x40004u, 0x40000u | g.W(), g.W(), 0xFFFFFFFFu, 0x3FFFFu, 0xFFFBFFFFu});
        a[11] = g.Pk({0u, 1u, 0xFFu, 75u, g.W() & 0xFFu, g.W() & 0xFFu});
        a[12] = AuxOffset(g, 8) & ~7u; a[13] = ((a[12] + 0x100u) % 0xF00u) & ~7u; a[14] = ((a[12] + 0x200u) % 0xF00u) & ~7u;
        a[15] = g.W();
        const u32 pm = g.Pk({3u, 10u, 30u, 60u});
        c.s.resize(0x258);
        for (u32 i = 0; i < 75; ++i) {
            u32 t, h;
            const u32 r = Rand(a[15], i) % 100;
            if (r < pm) { t = low; h = a[4]; }
            else if (r < pm + 12) { t = low; h = Rand(a[15], 200 + i); if ((Rand(a[15], 800 + i) % 100) < 30) h = a[4] ^ 0x100u; }
            else if (r < pm + 24) { t = Rand(a[15], 300 + i) % 6; if (t == low) t = (t + 1) % 6; h = a[4]; }
            else { t = Rand(a[15], 400 + i) % 7; h = Rand(a[15], 500 + i); if ((Rand(a[15], 600 + i) & 3) == 0) h = 0; }
            const u32 pad = Rand(a[15], 700 + i) & 0xFFFFFF00u;
            const u32 w0 = (t & 0xFFu) | pad;
            std::memcpy(c.s.data() + 8 * i, &w0, 4); std::memcpy(c.s.data() + 8 * i + 4, &h, 4);
        }
        break;
    }
    case kSuppressed: {
        for (u32 i = 0; i < 40; ++i) a[8 + i] = g.Pk({0xFFFFFFFFu, 0xFFFFFFFFu, g.R(20), g.R(700), 0xFFFFFFFEu, g.W(), 0x80000000u, g.R(5)});
        const u32 pick = a[8 + g.R(40)];
        a[4] = g.Pk({pick, pick, pick, 0xFFFFFFFFu, g.R(700), 0u, g.W(), 0x80000000u, a[8], a[47]});
        break;
    }
    case kBrains:
        a[4] = g.Pk({0u, g.W(), 0xC3C3C3C3u});
        break;
    case kCamera: {
        const i32 t = i32(g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, 0x1000000u, 0x1000001u, 0xFFFFFFC0u, g.R(100000), g.R(0x7FFFFFFF), u32(-i32(g.R(100000))), g.W(), 0xFEFFFFFFu, 0xFF000001u}));
        a[4] = u32(t);
        auto pickEnd = [&]() -> u32 {
            const double exact = t < 0 ? double(t) + 4294967296.0 : double(t);
            const float nearest = float(exact);
            u32 b; std::memcpy(&b, &nearest, 4);
            switch (g.R(14)) {
            case 0: case 1: case 2: return b;
            case 3: case 4: return b + 1u;
            case 5: case 6: return b - 1u;
            case 7: return b + 2u;
            case 8: return b - 2u;
            case 9: return FloatOf(t);
            case 10: return g.Pk({0u, 0x80000000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0x7F800001u, 0xFFC00000u, 0x4F800000u});
            case 11: return g.W();
            case 12: return FloatBits(g);
            default: return FloatOf(t + i32(g.R(5)) - 2);
            }
        };
        a[5] = pickEnd(); a[6] = pickEnd();
        break;
    }
    case kSdd:
        a[4] = g.Pk({0u, 1u, 1u, 3u, 0xFFFFFFFEu, 0x100u, 0x101u, g.W(), (g.W() & 0xFFFFFF00u) | 1u, g.W() & 0xFFFFFF00u});
        a[5] = g.Pk({0u, g.W(), 0x859EBCu});
        break;
    case kBoxStub:
        for (u32 i = 0; i < 6; ++i) a[4 + i] = g.P(50) ? FloatBits(g) : Raw(g);
        break;
    case kSlBetween: case kSlEntity: case kSlPoint: {
        a[4] = Raw(g);
        a[5] = g.P(28) ? g.Pk({0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000000u, g.W() | 0x80000000u}) : g.R(8);
        a[6] = g.Pk({0u, 0u, Raw(g)});
        a[7] = NiceFloat(g); a[8] = NiceFloat(g);
        if (k == kSlBetween) {
            for (u32 i = 0; i < 7; ++i) a[9 + i] = NiceFloat(g);
            if (g.P(12)) { a[12] = a[9]; a[13] = a[10]; }                    // los dos puntos coinciden (x2,y2 = x1,y1)
            else if (g.P(10)) { a[12] = a[10]; a[13] = a[9]; a[9] = a[7] ^ (g.P(50) ? 0u : 0x80000000u); a[10] = a[8]; }   // distancias simetricas
            if (g.P(25)) { a[9] = a[7] + g.R(3); a[10] = a[8] + g.R(3); }
        } else if (k == kSlEntity) {
            a[9] = g.P(18) ? 0u : Raw(g); a[10] = g.P(50) ? NiceFloat(g) : Raw(g);
        } else {
            for (u32 i = 0; i < 4; ++i) a[9 + i] = g.P(50) ? NiceFloat(g) : Raw(g);
        }
        break;
    }
    case kPedMI:
        a[4] = g.Pk({0u, g.W(), 0x85BB9Cu}); a[5] = g.Pk({0u, g.W(), 0xFFFFFFFFu});
        break;
    case kHeader: {
        a[4] = g.P(85) ? 1u : 0u;
        a[5] = AuxOffset(g, 0x20);
        a[6] = g.Pk({0u, 1u, 8u, 8u, 1u, 0xFFu, 2u, 3u, 0x80u, g.W() & 0xFFu});
        a[7] = Raw(g); a[8] = g.P(40) ? 0u : Raw(g); a[9] = g.P(40) ? 0u : Raw(g); a[10] = g.Pk({0u, 1u, 3u, 0x10000u, 0xFFFFFFFFu, g.W(), g.R(100)});
        for (u32 i = 0; i < 2; ++i) {
            const u32 b = 11 + 6 * i;
            a[b] = g.P(25) ? 0u : Raw(g);
            a[b + 1] = g.Pk({0u, 0u, 4u, 0x100u, 0xFFFFFFFFu, g.W(), g.R(1000)});
            a[b + 2] = g.Pk({0u, 12u, 16u, 32u, 0x40u, g.W(), g.R(64)});
            a[b + 3] = g.Pk({0u, 0xFFFFu, g.W() & 0xFFFFu, 1u});
            a[b + 4] = g.Pk({0u, 0u, 1u, 0xFFu, g.W() & 0xFFu});
            a[b + 5] = g.Pk({0u, 0u, 1u, 0xFFu, g.W() & 0xFFu});
        }
        break;
    }
    case kAttach:
        a[4] = g.Pk({0u, 1u, 0x2Cu, 0x40u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000000u, 0x7FFFFFFFu, g.R(0x200), g.W(), 0x100u, u32(-i32(g.R(0x100)))});
        break;
    case kTexAlpha: {
        a[4] = g.P(15) ? 0u : 1u;
        const u32 X = g.Pk({0x30u, 0x34u, 0x38u, 0x40u, 0x50u, 0x7Cu, 0x100u, 0x30u + g.R(0x300), 0x31u + g.R(0x300)});
        a[6] = X;
        a[0] = Offset(g, kind, 4);
        a[5] = Offset(g, g.R(4), X + 0x10);
        a[7] = g.Pk({0u, 1u, 0xFFu, 0x80u, g.W() & 0xFFu, g.W() & 0xFFu}); a[8] = g.W() & 0xFFu;
        break;
    }
    case kIsCube: {
        const u32 X = g.Pk({0x30u, 0x34u, 0x38u, 0x40u, 0x50u, 0x7Cu, 0x100u, 0x30u + g.R(0x300), 0x31u + g.R(0x300)});
        a[6] = X;
        a[5] = Offset(g, g.R(4), X + 0x10);
        a[7] = g.Pk({0u, 1u, 0xFu, 0x10u, 0x1Fu, 0xF0u, 0xFFu, g.W() & 0xFFu, g.W() & 0xFFu});
        break;
    }
    case kLock: case kUnlock: case kPalette:
        MakeRaster(g, c, k, kind);
        if (k == kPalette && (a[11] & 0x10u)) a[45] = 1u;   // con el bit 4 de privateFlags el EXE lee [paleta + 0x400]: la paleta de la extension no puede ser nula
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

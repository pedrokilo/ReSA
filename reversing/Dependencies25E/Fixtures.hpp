#pragma once
// Fixtures deterministas y construccion de los arenas del banco Dependencies25E (sin dependencias del juego).
// fixture_oracle.py repite exactamente estas formulas y tablas (comprobacion cruzada descrita en README.md).
// Un arena es un bloque propio del banco: [16 bytes de guarda 0xA5][interior][16 bytes de guarda 0xA5]. Las direcciones que contiene el interior
// son absolutas (direccion real del arena); no se toca ningun objeto del motor RenderWare ni del juego.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
namespace Dependencies25EFix {
using u32 = std::uint32_t;
inline constexpr unsigned kCases = 1024;   // pares por funcion
inline constexpr unsigned kGuard = 16;
inline constexpr std::uint8_t kGuardByte = 0xA5;
inline constexpr u32 kEmptyString = 0x884F6A;   // cadena vacia del EXE que _rwStringStreamGetSize usa para una cadena nula

// Mezcla (finalizador de murmur3 de 32 bits) y generador por (caso, clave).
constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }

// Palabras de estres: 0, pequenos, limites de 8/16/32 bits con signo, patrones, direcciones del EXE usadas por las funciones.
inline constexpr std::array<u32, 32> kWords{
    0x00000000, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000007, 0x00000008, 0x0000007F,
    0x00000080, 0x000000FF, 0x00000100, 0x0000FFFF, 0x00010000, 0x3FFFFFFF, 0x40000000, 0x7FFFFFFF,
    0x80000000, 0x80000001, 0xC0000000, 0xFFFF0000, 0xFFFFFFFC, 0xFFFFFFFD, 0xFFFFFFFE, 0xFFFFFFFF,
    0x55555555, 0xAAAAAAAA, 0xDEADBEEF, 0x12345678, 0x01010101, 0x00884F6A, 0x00C9A6B0, 0x0000FFF0};
// 75 % de las veces una palabra de la tabla, 25 % una palabra aleatoria.
constexpr u32 Word(u32 c, u32 k) { return (Rand(c, k) & 3u) == 0 ? Rand(c, k + 1000u) : kWords[Rand(c, k + 2000u) % 32u]; }
// Semillas de EAX/ECX/EDX antes de la llamada (cambian por caso; solo se registran).
constexpr u32 Seed(u32 c, u32 reg) { return Rand(c, 9000u + reg); }

inline void Put32(std::uint8_t* base, std::size_t at, u32 v) { std::memcpy(base + at, &v, 4); }
inline u32 Get32(const std::uint8_t* base, std::size_t at) { u32 v; std::memcpy(&v, base + at, 4); return v; }
inline u32 Ptr(const std::uint8_t* base, std::size_t at) { return static_cast<u32>(reinterpret_cast<std::uintptr_t>(base + at)); }
// Rellena el interior con palabras aleatorias del caso (los campos que la funcion no debe leer quedan con basura).
inline void Junk(std::uint8_t* interior, std::size_t size, u32 c) { for (std::size_t i = 0; i < size; i += 4) Put32(interior, i, Rand(c, 7000u + static_cast<u32>(i / 4))); }

// ------------------------------------------------------------------ RxHeapGetGlobalHeap (0x8052F0)
// El valor que se escribe temporalmente en la variable del EXE 0xC9A6B0.
constexpr u32 RxHeapValue(u32 c) { return Word(c, 1); }

// ------------------------------------------------------------------ _rwFrameListDeinitialize (0x807720)
// rwFrameList {frames(+0), numFrames(+4)} en el interior +16; interior de 48 bytes.
struct FrameListFx { u32 frames, numFrames; };
constexpr FrameListFx FrameList(u32 c) {
    FrameListFx f{Word(c, 1), Word(c, 2)};
    if (Rand(c, 3) % 8 == 0) f.numFrames = 0;
    if (Rand(c, 4) % 8 == 1) f.frames = 0;
    return f;
}
inline constexpr std::size_t kFrameListInterior = 48, kFrameListObject = 16;
inline void BuildFrameList(std::uint8_t* base, u32 c) {
    Junk(base, kFrameListInterior, c);
    const auto f = FrameList(c);
    Put32(base, kFrameListObject + 0, f.frames); Put32(base, kFrameListObject + 4, f.numFrames);
}

// ------------------------------------------------------------------ _rwPluginRegistryGetPluginOffset (0x808470)
// Registro RwPluginRegistry (24 bytes) en +16: sizeOfStruct, origSizeOfStruct, maxSizeOfStruct, staticAlloc, firstRegEntry(+0x10), lastRegEntry(+0x14).
// Ocho ranuras de RwPluginRegEntry (0x40 bytes) desde +48: offset(+0), size(+4), pluginID(+8), ..., nextRegEntry(+0x30), prevRegEntry(+0x34), parent(+0x38).
inline constexpr std::array<u32, 6> kPluginIds{0x00000001, 0x00010007, 0xFFFFFFFF, 0x80000000, 0x00000020, 0x00000000};
inline constexpr std::size_t kPluginInterior = 48 + 8 * 0x40, kPluginRegistry = 16, kPluginSlot0 = 48, kPluginSlotSize = 0x40;
struct PluginFx { unsigned n; unsigned slot[8]; u32 id[8], offset[8]; u32 query; unsigned mode; };
constexpr unsigned PluginSlot(u32 c, unsigned j) { return (j * 3u + Rand(c, 11) % 8u) % 8u; }   // 3 es coprimo con 8: permutacion de las ranuras
constexpr PluginFx Plugin(u32 c) {
    PluginFx f{Rand(c, 10) % 9u, {}, {}, {}, 0, Rand(c, 50) % 4u};
    for (unsigned j = 0; j < 8; ++j) {
        f.slot[j] = PluginSlot(c, j);
        f.id[j] = kPluginIds[Rand(c, 40 + j) % 6u];
        f.offset[j] = Word(c, 30 + j);
    }
    switch (f.mode) {
        case 0: f.query = f.n ? f.id[Rand(c, 51) % f.n] : 0x12345678u; break;          // el id de una entrada de la lista
        case 1: f.query = 0x12345678u ^ (Rand(c, 52) & 0xFFFFu); break;                 // ausente (los ids de la tabla no tienen esos bits)
        case 2: f.query = kPluginIds[Rand(c, 53) % 6u]; break;                          // id de la tabla (presente o no)
        default: f.query = Word(c, 54); break;
    }
    return f;
}
inline void BuildPlugin(std::uint8_t* base, u32 c) {
    Junk(base, kPluginInterior, c);
    const auto f = Plugin(c);
    for (unsigned j = 0; j < f.n; ++j) {
        const std::size_t entry = kPluginSlot0 + kPluginSlotSize * f.slot[j];
        Put32(base, entry + 0, f.offset[j]); Put32(base, entry + 8, f.id[j]);
        Put32(base, entry + 0x30, j + 1 < f.n ? Ptr(base, kPluginSlot0 + kPluginSlotSize * f.slot[j + 1]) : 0u);
    }
    Put32(base, kPluginRegistry + 0x10, f.n ? Ptr(base, kPluginSlot0 + kPluginSlotSize * f.slot[0]) : 0u);
}

// ------------------------------------------------------------------ RwSList (0x809400, 0x8094B0, 0x809510, 0x809530, 0x809540)
// RwSList {listElements(+0), numElementsFilled(+4), numElementsAlloced(+8), entrySize(+0xC)} en el interior +16; interior de 48 bytes.
struct SListFx { u32 elements, filled, alloced, entrySize, arg; };
constexpr SListFx SList(u32 c) {
    SListFx f{Word(c, 1), Word(c, 2), Word(c, 3), Word(c, 4), Word(c, 5)};
    if (Rand(c, 6) % 4 == 0) { f.entrySize = Rand(c, 7) % 256u; f.filled = Rand(c, 8) % 1024u; f.arg = Rand(c, 9) % 1024u; }   // valores plausibles
    return f;
}
inline constexpr std::size_t kSListInterior = 48, kSListObject = 16;
inline void BuildSList(std::uint8_t* base, u32 c) {
    Junk(base, kSListInterior, c);
    const auto f = SList(c);
    Put32(base, kSListObject + 0, f.elements); Put32(base, kSListObject + 4, f.filled); Put32(base, kSListObject + 8, f.alloced); Put32(base, kSListObject + 0xC, f.entrySize);
}

// ------------------------------------------------------------------ _rwStringClose (0x80A440)
// Sin argumentos ni estado: interior de 32 bytes con basura que la funcion no debe tocar.
inline constexpr std::size_t kStringCloseInterior = 32;
inline void BuildStringClose(std::uint8_t* base, u32 c) { Junk(base, kStringCloseInterior, c); }

// ------------------------------------------------------------------ _rwStringStreamGetSize (0x80A470)
// Interior de 64 bytes con basura; la cadena es el interior +16 (o nula en 1 de cada 8 casos). rwstrlen del motor se sustituye por un doble que devuelve `length`.
struct StringFx { bool null; u32 length; };
constexpr StringFx String(u32 c) {
    StringFx f{Rand(c, 1) % 8u == 0, Word(c, 2)};
    if (Rand(c, 3) % 4 == 0) f.length = Rand(c, 4) % 100u;
    return f;
}
inline constexpr std::size_t kStringInterior = 64, kStringObject = 16;
inline void BuildString(std::uint8_t* base, u32 c) { Junk(base, kStringInterior, c); }

// ------------------------------------------------------------------ RwD3D9RasterIsCompressed (0x820C90)
// Interior de 64 bytes; el byte de banderas esta en el interior +32 y vale (caso & 255); el desplazamiento de la extension (variable 0xB4E9E0) es `delta`
// y el raster es la direccion del byte menos 0xA menos delta (con envoltura de 32 bits). Los bytes vecinos son basura.
struct RasterFx { u32 flags, delta; };
constexpr RasterFx Raster(u32 c) {
    RasterFx f{c & 255u, 0};
    switch ((c >> 8) & 3u) { case 0: f.delta = 0; break; case 1: f.delta = 0x30; break; case 2: f.delta = 0xFFFFFFB0u; break; default: f.delta = Rand(c, 61); break; }
    return f;
}
inline constexpr std::size_t kRasterInterior = 64, kRasterFlag = 32;
inline void BuildRaster(std::uint8_t* base, u32 c) { Junk(base, kRasterInterior, c); base[kRasterFlag] = static_cast<std::uint8_t>(Raster(c).flags); }

// ------------------------------------------------------------------ CrtAtexit (0x821D1E)
// Interior de 16 bytes con basura. func es una palabra cualquiera (no se llama); _onexit (0x821CE6) se sustituye por un doble que devuelve `result`.
struct AtexitFx { u32 func, result; };
constexpr AtexitFx Atexit(u32 c) {
    AtexitFx f{Word(c, 1), 0};
    switch (Rand(c, 2) % 4u) { case 0: f.result = 0; break; case 1: f.result = f.func; break; default: f.result = Word(c, 3); break; }
    return f;
}
inline constexpr std::size_t kAtexitInterior = 16;
inline void BuildAtexit(std::uint8_t* base, u32 c) { Junk(base, kAtexitInterior, c); }

// ------------------------------------------------------------------ CrtLockFile2 (0x823FD1) / CrtUnlockFile2 (0x824023)
// Casos 0..639: indice 0..19 (cerrojo de tabla 16 + indice del CRT del EXE); 640..1023: indices >= 20 (seccion critica propia del banco en stream + 0x20).
inline constexpr std::array<u32, 20> kLockIndices{20, 21, 22, 23, 24, 31, 32, 33, 63, 64, 100, 127, 128, 255, 256, 1000, 65535, 65536, 0x7FFFFFFE, 0x7FFFFFFF};
struct LockFx { u32 index; bool real; u32 stream; };
constexpr LockFx Lock(u32 c) {
    LockFx f{};
    f.real = c < 640u;
    f.index = f.real ? c % 20u : kLockIndices[(c - 640u) % 20u];
    f.stream = Word(c, 1);   // solo se usa si no es real; en ese caso el banco lo sustituye por &seccion - 0x20
    return f;
}

// ------------------------------------------------------------------ CrtMalloc (0x824257)
// Casos 0..639: tamanos que el montón del EXE satisface; 640..1023: tamanos que no puede satisfacer (>= 0xC0000000), con _newmode variable.
inline constexpr std::array<u32, 40> kMallocSizes{
    0, 1, 2, 3, 7, 8, 9, 15, 16, 17, 24, 31, 32, 33, 63, 64, 65, 127, 128, 129,
    255, 256, 257, 511, 512, 1000, 1016, 1017, 1024, 1025, 2047, 2048, 4095, 4096, 4097, 8192, 65535, 65536, 65537, 1048576};
inline constexpr std::array<u32, 8> kMallocFailSizes{0xC0000000, 0xF0000000, 0xFFFF0000, 0xFFFFF000, 0xFFFFFFDF, 0xFFFFFFE0, 0xFFFFFFE1, 0xFFFFFFFF};
inline constexpr std::array<u32, 6> kNewModes{0, 1, 2, 0x80000000, 0xFFFFFFFF, 0x100};
struct MallocFx { bool success; u32 size, newmode; };
constexpr MallocFx Malloc(u32 c) {
    MallocFx f{c < 640u, 0, 0};
    f.size = f.success ? kMallocSizes[Rand(c, 1) % 40u] : kMallocFailSizes[Rand(c, 2) % 8u];
    f.newmode = kNewModes[Rand(c, 3) % 6u];
    return f;
}

// ------------------------------------------------------------------ CrtErrno (0x82B181)
// El valor que se escribe temporalmente en errno del hilo (_getptd()->_terrno).
constexpr u32 ErrnoValue(u32 c) { return Word(c, 1); }

// Tamano total de un arena (guardas incluidas) para un interior dado.
constexpr std::size_t ArenaBytes(std::size_t interior) { return interior + 2 * kGuard; }
}

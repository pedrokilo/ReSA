#pragma once
// Fixtures deterministas del banco Dependencies25C. Solo C++ estandar (sin dependencias del juego): fixture_oracle.py lee las tablas de este archivo y repite
// exactamente las formulas de Make() (la comprobacion cruzada esta descrita en README.md). Los valores float se dan como patrones de bits.
#include <array>
#include <cstdint>
namespace Dependencies25CFix {
using u32 = std::uint32_t;
using u64 = std::uint64_t;

// Funciones del banco (indice estable; el orden es el de las filas de g25c-hooks.csv.txt).
enum Fn : unsigned {
    MeshHeaderCreate, BuildMeshDefaultTriStrip, NodeDefWorldSector, SectorRegisterPlugin, SectorRegisterPluginStream, HAnimFrameGetHierarchy, SkinDeinitialize,
    UVCustomDataSize, UVKeyFrameSize, AnimGetNumNodes, InterpolatorDestroy, DictSchemaDestruct, DictSchemaGetCurrent, StreamWriteInt32, CameraSetViewOffset,
    CameraRegisterPlugin, FrameDirty, MatrixOrthoNormalize, EngineRegisterPlugin, EngineGetPluginOffset, kFnCount
};
inline constexpr std::array<unsigned, kFnCount> kCases{1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1536, 2048, 1024, 1024, 1024, 1024, 2304, 1024, 1024, 1024, 1024, 1024};
inline constexpr unsigned kTotalPairs = 1024 * 17 + 1536 + 2048 + 2304;   // = 23296

// Mezcla (finalizador de murmur3 de 32 bits) y generador por (caso, clave).
constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 0x2C1B3C6Du); }

// Palabras de 32 bits con valores limite (enteros extremos, patrones float NaN/inf/subnormal, constantes de RenderWare).
inline constexpr std::array<u32, 32> kW32{
    0x00000000, 0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040,
    0x0000007F, 0x00000080, 0x000000FF, 0x00000100, 0x0000FFFF, 0x00010000, 0x7FFFFFFF, 0x80000000,
    0x80000001, 0xFFFFFFFF, 0xFFFFFFFE, 0xDEADBEEF, 0x12345678, 0x9E3779B1, 0x55555555, 0xAAAAAAAA,
    0x7F800000, 0x7FC00000, 0xFF800001, 0x007FFFFF, 0x3F800000, 0xBF800000, 0x00030502, 0x0000A5A5};
// Valores que devuelve un colaborador falso (EAX): nulo, exito, error, signo, desbordes, constantes.
inline constexpr std::array<u32, 16> kRet{
    0x00000000, 0x00000001, 0xFFFFFFFF, 0x80000000, 0x7FFFFFFF, 0x00000002, 0x00000040, 0x00000004,
    0xDEADBEEF, 0x00000100, 0x0BADF00D, 0xFFFFFFFE, 0x80000001, 0x0000FFFF, 0x00010000, 0xA5A5A5A5};
// Desplazamientos (x, y) de RwCameraSetViewOffset como patrones de bits: +-0, subnormales, normales, +-1, +-FLT_MAX, +-inf, NaN con distinta carga y senalizantes, +-1e30, 1e-30.
inline constexpr std::array<u32, 24> kFloat{
    0x00000000, 0x80000000, 0x00000001, 0x807FFFFF, 0x00800000, 0x3F800000, 0xBF800000, 0x3F000000,
    0x7F7FFFFF, 0xFF7FFFFF, 0x7F800000, 0xFF800000, 0x7FC01234, 0xFFC00001, 0x7F800001, 0xFF800123,
    0x7FFFFFFF, 0xFFFFFFFF, 0x7149F2CA, 0xF149F2CA, 0x0DA24260, 0x40490FDB, 0x42C80000, 0x007FFFFE};
// Pasos (animKeyFrameSize) de RtAnimAnimationGetNumNodes: multiplos de 4; el 0 solo se usa con cero nodos.
inline constexpr std::array<u32, 12> kStep{0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 48, 64};

// Posiciones fijas del arena (4096 bytes; las dos guardas de 16 bytes quedan fuera). Los campos de cada estructura se escriben en los desplazamientos reales del EXE.
inline constexpr unsigned kArena = 0x1000;
inline constexpr unsigned kEngineBlock = 0x100;    // RwGlobals falso (rwmalloc en +0x134, rwfree en +0x138)
inline constexpr unsigned kHAnimFrame0 = 0x040, kHAnimTarget0 = 0x200;
inline constexpr unsigned kStructBase = 0x300;     // anim UV, esquema de diccionario
inline constexpr unsigned kCameraBase = 0x200, kOffsetBase = 0x500, kParentBase = 0x700;
inline constexpr unsigned kFrameBase = 0x200, kRootOther0 = 0x800, kRootOther1 = 0x900, kFrameAlt = 0x600;
inline constexpr unsigned kMeshBase = 0x400;

enum Arg { kRaw = 0, kPtr = 1 };
struct Spec { unsigned kind; u32 v; };   // kRaw: valor de 32 bits; kPtr: desplazamiento dentro del arena

struct Fx {
    unsigned fn, c;
    u32 seed[3];      // EAX/ECX/EDX de entrada
    u32 prime;        // semilla de los residuos de los ocho registros x87
    u32 ret;          // valor que devuelven los colaboradores falsos
    unsigned n;       // argumentos de pila
    Spec a[6];
    u32 x[6];         // parametros propios de cada funcion (ver Make)
};

// Palabra "especial": en c % 4 == 0 siempre aleatoria (argumentos distintos entre si, detectan permutaciones); si no, mitad tabla, mitad aleatoria.
constexpr u32 W(u32 c, u32 k) { return c % 4 == 0 ? Rand(c, 10 + k) : (Rand(c, 20 + k) & 1 ? kW32[Rand(c, 30 + k) % 32] : Rand(c, 40 + k)); }
constexpr u32 RetOf(u32 c) { return c % 4 == 0 ? Rand(c, 5) : kRet[Rand(c, 6) % 16]; }

constexpr Fx Make(unsigned fn, unsigned c) {
    Fx f{};
    f.fn = fn; f.c = c;
    f.seed[0] = Rand(c, 1); f.seed[1] = Rand(c, 2); f.seed[2] = Rand(c, 3); f.prime = Rand(c, 4); f.ret = RetOf(c);
    switch (fn) {
    case MeshHeaderCreate:             // _rpMeshHeaderCreate(size)
        f.n = 1; f.a[0] = {kRaw, W(c, 0)}; break;
    case BuildMeshDefaultTriStrip:     // (buildMesh, data)
        f.n = 2; f.a[0] = c % 3 == 0 ? Spec{kPtr, kMeshBase + 4 * (Rand(c, 50) % 64)} : Spec{kRaw, W(c, 0)}; f.a[1] = {kRaw, W(c, 1)}; break;
    case NodeDefWorldSector:           // ()
        f.n = 0; break;
    case SectorRegisterPlugin: case CameraRegisterPlugin:   // (size, pluginID, ctor, dtor, copy)
        f.n = 5; for (unsigned i = 0; i < 5; ++i) f.a[i] = {kRaw, W(c, i)}; break;
    case SectorRegisterPluginStream:   // (pluginID, read, write, getSize)
        f.n = 4; for (unsigned i = 0; i < 4; ++i) f.a[i] = {kRaw, W(c, i)}; break;
    case HAnimFrameGetHierarchy: {     // (frame): x0 = desplazamiento del destino, x1 = palabra que contiene, x2 = tipo de marco (0 en el arena, 1 puntero crudo, 2 nulo)
        f.n = 1; f.x[0] = kHAnimTarget0 + 4 * (Rand(c, 60) % 256); f.x[1] = W(c, 7); f.x[2] = Rand(c, 61) % 3;
        f.a[0] = f.x[2] == 0 ? Spec{kPtr, kHAnimFrame0 + 4 * (Rand(c, 62) % 32)} : f.x[2] == 1 ? Spec{kRaw, Rand(c, 63)} : Spec{kRaw, 0};
        break; }
    case SkinDeinitialize: case UVCustomDataSize: case InterpolatorDestroy:
        f.n = 1; f.a[0] = {kRaw, W(c, 0)}; break;
    case UVKeyFrameSize:               // (anim): x0 = numFrames, x1 = customData, x2 = modo (5: colaborador real, el resto: falso que devuelve `ret`)
        f.n = 1; f.a[0] = {kPtr, kStructBase + 4 * (Rand(c, 50) % 32)}; f.x[0] = W(c, 1); f.x[1] = W(c, 2); f.x[2] = c % 6; break;
    case AnimGetNumNodes: {            // (anim): x0 = paso, x1 = nodos esperados, x2 = variante de la disposicion
        const unsigned m = (c / 12) % 13; const unsigned s = m == 0 ? kStep[c % 12] : kStep[1 + c % 11];
        f.n = 1; f.x[0] = s; f.x[1] = m; f.x[2] = c / 156;
        f.a[0] = {kPtr, 0x40 + 0x40 * (f.x[2] % 2)}; break; }
    case DictSchemaDestruct: case DictSchemaGetCurrent:   // (schema): x0 = palabra en +0x10 (destruct) o +0x14 (getcurrent)
        f.n = 1; f.a[0] = {kPtr, kStructBase + 4 * (Rand(c, 50) % 32)}; f.x[0] = W(c, 1); break;
    case StreamWriteInt32:             // (stream, ints, numBytes)
        f.n = 3; for (unsigned i = 0; i < 3; ++i) f.a[i] = {kRaw, W(c, i)}; break;
    case CameraSetViewOffset:          // (camera, offset): x0 = X, x1 = Y (patrones), x2 = solape (0 estructura aparte, 1 = &viewOffset, 2 = &viewOffset.y, 3 = &viewOffset - 4), x3 = padre (0 nulo, 1 en el arena, 2 puntero crudo)
        f.n = 2; f.x[0] = kFloat[c % 24]; f.x[1] = kFloat[(c / 24) % 24]; f.x[2] = (c / 576) % 4; f.x[3] = (c + c / 24 + c / 576) % 3;
        f.a[0] = {kPtr, kCameraBase};
        f.a[1] = f.x[2] == 0 ? Spec{kPtr, kOffsetBase} : f.x[2] == 1 ? Spec{kPtr, kCameraBase + 0x78} : f.x[2] == 2 ? Spec{kPtr, kCameraBase + 0x7C} : Spec{kPtr, kCameraBase + 0x74};
        break;
    case FrameDirty:                   // (frame): x0 = indicador privado de la raiz, x1 = tipo de raiz (0 el propio marco, 1 otro en 0x800, 2 otro en 0x900, 3 el propio marco en 0x600)
        f.n = 1; f.x[0] = c % 256; f.x[1] = (c / 256) % 4;
        f.a[0] = {kPtr, f.x[1] == 3 ? kFrameAlt : kFrameBase}; break;
    case MatrixOrthoNormalize:         // (out, in)
        f.n = 2; f.a[0] = c % 3 == 0 ? Spec{kPtr, kMeshBase + 16 * (Rand(c, 50) % 32)} : Spec{kRaw, W(c, 0)}; f.a[1] = c % 5 == 0 ? Spec{kPtr, kMeshBase + 16 * (Rand(c, 51) % 32)} : Spec{kRaw, W(c, 1)}; break;
    case EngineRegisterPlugin:         // (size, pluginID, init, term)
        f.n = 4; for (unsigned i = 0; i < 4; ++i) f.a[i] = {kRaw, W(c, i)}; break;
    case EngineGetPluginOffset:        // (pluginID)
        f.n = 1; f.a[0] = {kRaw, W(c, 0)}; break;
    }
    return f;
}
}  // namespace Dependencies25CFix

#pragma once
// Nucleo comun del banco Dependencies25B (sin dependencias del juego). Lo incluyen LiveComparison.hpp (dentro del juego, EXE con hook apagado frente a DLL con hook
// encendido) y offline/main.cpp (bytes reales del EXE en un proceso hijo frente al C++ de los fuentes). Exige que `json` (nlohmann) y <windows.h> ya esten declarados.
//
// Veinte funciones de biblioteca (RenderWare SDK / controlador D3D9 de RenderWare), 1024 pares cada una = 20480 pares:
//   RwCore::RwD3D9IndexBufferCreate 0x4C9970 | RpAnisot::{GetMaxSupported 0x748F20, TextureSet 0x748F30, TextureGet 0x748F50}
//   RpWorld::{RpClumpGetNumAtomics 0x7498E0, RpAtomicSetStreamAlwaysCallBack 0x74BE30, RpAtomicSetStreamRightsCallBack 0x74BE50, RpAtomicGetPluginOffset 0x74BEC0,
//             RpGeometryTriangleGetMaterial 0x74C760, _rpMaterialListInitialize 0x74E1B0, _rpMaterialListGetMaterial 0x74E2B0, _rpMaterialListFindMaterialIndex 0x74E420,
//             RpWorldRegisterPlugin 0x74FCD0, RpWorldRegisterPluginStream 0x74FD00, RpAtomicGetWorld 0x751050, RpLightRegisterPlugin 0x751D60,
//             _rpD3D9VertexDeclarationGetSize 0x7522E0, RpD3D9GeometryGetUsageFlags 0x7588D0, RpD3D9WorldSectorSetUsageFlags 0x7588E0, RpD3D9WorldSectorGetUsageFlags 0x758900}
// Ninguna ejecuta x87. Los objetos son un arena propio del banco (con 16 bytes de guarda a cada lado); los desplazamientos de plugin globales del EXE (0xC92478, 0xC925A8,
// 0xC926E8, 0xC926EC) se fijan por caso y se restauran tras cada lado (compuerta globals_restored). Los colaboradores de RenderWare se sustituyen en AMBOS lados por el mismo
// falso trazado (parche en la direccion del EXE y en la entrada del envoltorio de la DLL).
#include <array>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace Dependencies25BFix {
using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 1024;
constexpr unsigned kArena = 0x180, kGuard = 16;
constexpr u8 kGuardByte = 0xA5;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 25011u); }

enum Fid : unsigned {
    IndexBuf, MaxSupported, AnisoSet, AnisoGet, ClumpAtoms, AtomAlways, AtomRights, AtomPlugOff, TriMat, MatInit,
    MatGet, MatFind, WorldReg, WorldRegStream, AtomWorld, LightReg, VdeclSize, GeoUsage, SecSet, SecGet, kFunctions
};
// Colaboradores: 0 ninguno | 1 RwD3D9GetCaps 0x7FAD20 | 2 AddPlgnStrmAlwaysCB 0x808920 | 3 AddPlgnStrmRightsCB 0x808950 | 4 GetPluginOffset 0x808470 | 5 AddPlugin 0x8084A0 |
//               6 AddPluginStream 0x8088E0 | 7 D3DResourceSystem::CreateIndexBuffer 0x7306A0
constexpr unsigned kCollabKinds = 8;
constexpr std::array<std::uintptr_t, kCollabKinds> kCollabExe{0, 0x7FAD20, 0x808920, 0x808950, 0x808470, 0x8084A0, 0x8088E0, 0x7306A0};
struct Fn { const char* name; const char* cls; const char* fn; std::uintptr_t entry; unsigned collab; std::uintptr_t glob; unsigned nargs; bool hasRet; u32 retMask; };
inline constexpr std::array<Fn, kFunctions> kFns{{
    {"RwCore::RwD3D9IndexBufferCreate", "RwCore", "RwD3D9IndexBufferCreate", 0x4C9970, 7, 0, 2, true, 0xFFFFFFFFu},
    {"RpAnisot::RpAnisotGetMaxSupportedMaxAnisotropy", "RpAnisot", "RpAnisotGetMaxSupportedMaxAnisotropy", 0x748F20, 1, 0, 0, true, 0xFFu},
    {"RpAnisot::RpAnisotTextureSetMaxAnisotropy", "RpAnisot", "RpAnisotTextureSetMaxAnisotropy", 0x748F30, 0, 0xC92478, 2, true, 0xFFFFFFFFu},
    {"RpAnisot::RpAnisotTextureGetMaxAnisotropy", "RpAnisot", "RpAnisotTextureGetMaxAnisotropy", 0x748F50, 0, 0xC92478, 1, true, 0xFFu},
    {"RpWorld::RpClumpGetNumAtomics", "RpWorld", "RpClumpGetNumAtomics", 0x7498E0, 0, 0, 1, true, 0xFFFFFFFFu},
    {"RpWorld::RpAtomicSetStreamAlwaysCallBack", "RpWorld", "RpAtomicSetStreamAlwaysCallBack", 0x74BE30, 2, 0, 2, true, 0xFFFFFFFFu},
    {"RpWorld::RpAtomicSetStreamRightsCallBack", "RpWorld", "RpAtomicSetStreamRightsCallBack", 0x74BE50, 3, 0, 2, true, 0xFFFFFFFFu},
    {"RpWorld::RpAtomicGetPluginOffset", "RpWorld", "RpAtomicGetPluginOffset", 0x74BEC0, 4, 0, 1, true, 0xFFFFFFFFu},
    {"RpWorld::RpGeometryTriangleGetMaterial", "RpWorld", "RpGeometryTriangleGetMaterial", 0x74C760, 0, 0, 2, true, 0xFFFFFFFFu},
    {"RpWorld::_rpMaterialListInitialize", "RpWorld", "_rpMaterialListInitialize", 0x74E1B0, 0, 0, 1, true, 0xFFFFFFFFu},
    {"RpWorld::_rpMaterialListGetMaterial", "RpWorld", "_rpMaterialListGetMaterial", 0x74E2B0, 0, 0, 2, true, 0xFFFFFFFFu},
    {"RpWorld::_rpMaterialListFindMaterialIndex", "RpWorld", "_rpMaterialListFindMaterialIndex", 0x74E420, 0, 0, 2, true, 0xFFFFFFFFu},
    {"RpWorld::RpWorldRegisterPlugin", "RpWorld", "RpWorldRegisterPlugin", 0x74FCD0, 5, 0, 5, true, 0xFFFFFFFFu},
    {"RpWorld::RpWorldRegisterPluginStream", "RpWorld", "RpWorldRegisterPluginStream", 0x74FD00, 6, 0, 4, true, 0xFFFFFFFFu},
    {"RpWorld::RpAtomicGetWorld", "RpWorld", "RpAtomicGetWorld", 0x751050, 0, 0xC925A8, 1, true, 0xFFFFFFFFu},
    {"RpWorld::RpLightRegisterPlugin", "RpWorld", "RpLightRegisterPlugin", 0x751D60, 5, 0, 5, true, 0xFFFFFFFFu},
    {"RpWorld::_rpD3D9VertexDeclarationGetSize", "RpWorld", "_rpD3D9VertexDeclarationGetSize", 0x7522E0, 0, 0, 1, true, 0xFFFFFFFFu},
    {"RpWorld::RpD3D9GeometryGetUsageFlags", "RpWorld", "RpD3D9GeometryGetUsageFlags", 0x7588D0, 0, 0xC926E8, 1, true, 0xFFFFFFFFu},
    {"RpWorld::RpD3D9WorldSectorSetUsageFlags", "RpWorld", "RpD3D9WorldSectorSetUsageFlags", 0x7588E0, 0, 0xC926EC, 2, false, 0u},
    {"RpWorld::RpD3D9WorldSectorGetUsageFlags", "RpWorld", "RpD3D9WorldSectorGetUsageFlags", 0x758900, 0, 0xC926EC, 1, true, 0xFFFFFFFFu},
}};

// ------------------------------------------------------------------ pools
// Valores de 32 bits genericos (identificadores de plugin, tamanos, punteros a callbacks falsos, bytes de anisotropia con relleno).
inline constexpr std::array<u32, 32> kPool{
    0x00000000, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x0000007F, 0x00000080, 0x000000FF,
    0x00000100, 0x00007FFF, 0x00008000, 0x0000FFFF, 0x00010000, 0x7FFFFFFF, 0x80000000, 0x80000001,
    0xFFFFFFFE, 0xFFFFFFFF, 0xDEADBEEF, 0x00401000, 0x007498E0, 0x00C97B24, 0x12345678, 0xCAFEBABE,
    0xA5A5A5A5, 0x5A5A5A5A, 0x01010101, 0xFFFF0000, 0x0000FFFF, 0x80808080, 0x7F7F7F7F, 0x13579BDF};
// Retornos de los colaboradores (HRESULT y enteros extremos).
inline constexpr std::array<u32, 16> kRet{
    0x00000000, 0x00000001, 0x00000002, 0xFFFFFFFF, 0xFFFFFFFE, 0x80000000, 0x7FFFFFFF, 0x80000001,
    0x8876086C, 0x8007000E, 0x00000100, 0x0000FFFF, 0xFFFF0000, 0x7FFFFFFE, 0xDEADBEEF, 0x00000005};
// Desplazamientos de plugin (se impone el valor al global del EXE; el objeto se coloca en `arena + t - desplazamiento`).
inline constexpr std::array<u32, 16> kOff{
    0x00000000, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000008, 0x00000010, 0x00000024,
    0x00000040, 0x0000007C, 0x00000080, 0x00000100, 0xFFFFFFFC, 0xFFFFFFF0, 0x80000000, 0x7FFFFFFF};
// Bytes de anisotropia (parte baja del argumento RwInt8; los 24 bits altos van con basura).
inline constexpr std::array<u8, 32> kByte{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x08, 0x10, 0x7F, 0x80, 0x81, 0xFF, 0xFE, 0x55, 0xAA, 0x0F, 0xF0,
    0x40, 0x20, 0x11, 0x22, 0x33, 0x44, 0x66, 0x77, 0x88, 0x99, 0xBB, 0xCC, 0xDD, 0xEE, 0x5A, 0xA5};
// Indices de material / tipos: enteros extremos y envolturas de 32 bits (el EXE multiplica por 4 con vuelta).
inline constexpr std::array<u32, 32> kIdx{
    0x00000000, 0x00000001, 0x00000002, 0x00000003, 0x00000005, 0x00000007, 0x0000000F, 0x00000010,
    0x00000040, 0x00000100, 0x0000FFFF, 0x00010000, 0x7FFFFFFF, 0x80000000, 0x80000001, 0xFFFFFFFF,
    0xFFFFFFFE, 0xFFFFFFF0, 0xFFFF0000, 0x20000000, 0x40000000, 0x3FFFFFFF, 0xC0000000, 0xBFFFFFFF,
    0x12345678, 0x87654321, 0x55555555, 0xAAAAAAAA, 0x0000FFFE, 0x00FFFFFF, 0xFF000000, 0x1FFFFFFF};
// numMaterials para FindMaterialIndex.
inline constexpr std::array<u32, 16> kCount{
    0, 1, 2, 3, 4, 5, 7, 8, 11, 13, 16, 0xFFFFFFFFu, 0xFFFFFFF9u, 0x80000000u, 0x80000001u, 0xFFFFFFFBu};
// matIndex de RpTriangle (64): 0..47 y 16 especiales (incluye 0xFFFF = sin material).
inline constexpr u32 TriId(unsigned i) {
    constexpr std::array<u32, 16> special{0x40, 0x7F, 0x80, 0xFF, 0x100, 0x7FFF, 0x8000, 0xFFFE, 0xFFFF, 0xFFFF, 0x1234, 0xAAAA, 0x5555, 0xC000, 0x4000, 0xFF00};
    return i < 48 ? i : special[i - 48];
}
// Tipos de declaracion de vertices (1024): 0..63, rango valido de .rdata (0x858000..0x8A3FFC tras 4*t con vuelta de 32 bits), negativos, envolturas y bordes. Solo se leen direcciones de .rdata
// (inmutables y fuera de las entradas parcheadas por los hooks).
inline constexpr u32 VdeclType(unsigned c) {
    if (c < 64) return c;
    if (c < 320) return Rand(c, 11) % 48000u;                       // 0x874EF8 + 4*t < 0x8A4000
    if (c < 448) return static_cast<u32>(-static_cast<i32>(Rand(c, 12) % 28000u));   // hasta 0x858000
    if (c < 704) return (Rand(c, 13) % 48000u) + 0x40000000u * (1u + (c & 1u));      // 4*t da la vuelta a 2^32
    if (c < 960) return (Rand(c, 14) % 17u) + 0x40000000u * (c & 3u) + ((c & 4u) ? 0x80000000u : 0u);
    constexpr std::array<u32, 64> edge{
        0x00000000u, 0x00000001u, 0x0000000Fu, 0x00000010u, 0x00000011u, 0x00000012u, 0x00000013u, 0x00000014u, 0x00000015u, 0x0000001Fu, 0x00000020u, 0x00000021u, 0x0000BB7Fu, 0x0000BB7Eu, 0x0000B798u, 0x00009C40u,
        0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFFF0u, 0xFFFF92A0u, 0xFFFF92A1u, 0xFFFFFF9Cu, 0xFFFFFFC0u, 0xFFFFFE00u, 0x40000000u, 0x40000001u, 0x40000010u, 0x4000000Fu, 0x4000BB7Fu, 0x80000000u, 0x80000010u, 0x80000011u,
        0xC0000000u, 0xC0000001u, 0xC0000010u, 0x3FFFFFFFu, 0x7FFFFFFFu, 0xBFFFFFFFu, 0xC0000000u, 0xC000BB7Fu, 0x00004000u, 0x00008000u, 0x0000A000u, 0x0000BB7Fu, 0x0000BB00u, 0x00001000u, 0x00000400u, 0x00000100u,
        0x00000080u, 0x0000007Fu, 0x000000FFu, 0x000003FFu, 0x00000FFFu, 0x00003FFFu, 0x40004000u, 0x80008000u, 0xC000A000u, 0xFFFFFFFDu, 0xFFFFFFFCu, 0xFFFFFFF8u, 0xFFFFFF01u, 0xFFFFFF00u, 0xFFFFFC18u, 0xFFFFD8F0u};
    return edge[c - 960];
}

inline void Put32(u8* b, std::size_t at, u32 v) { std::memcpy(b + at, &v, 4); }
inline u32 Get32(const u8* b, std::size_t at) { u32 v; std::memcpy(&v, b + at, 4); return v; }
inline u32 AbsAt(const u8* base, std::size_t at) { return static_cast<u32>(reinterpret_cast<std::uintptr_t>(base + at)); }

constexpr u32 JunkWord(unsigned style, unsigned c, unsigned i) {
    switch (style & 7u) {
    case 0: return Rand(c, 1000 + i);
    case 1: return 0xFFFFFFFFu;
    case 2: return 0xA5A5A5A5u;
    case 3: return 0x5A5A5A5Au;
    case 4: return 0x80000000u + i;
    case 5: return 0x01010101u * (i & 0xFFu);
    case 6: return Rand(c ^ 0x55555555u, 2000 + i);
    default: return 0xDEAD0000u + i;
    }
}
inline unsigned JunkStyle(unsigned c) { return Rand(c, 1) & 7u; }
inline void FillJunk(u8* base, unsigned c) { const unsigned s = JunkStyle(c); for (unsigned i = 0; i < kArena / 4; ++i) Put32(base, i * 4, JunkWord(s, c, i)); }

// ------------------------------------------------------------------ casos
struct Fx { u32 args[6]{}; unsigned nargs{}; bool hasGlob{}; u32 glob{}; u32 stubRet{}; u32 capsAt{}; };

// Disposicion de cada arena (desplazamientos desde `base`):
//  IndexBuf       puntero a puntero (0 / base+0x40 / base+0x41 / basura): ningun acceso a memoria.
//  MaxSupported   D3DCAPS9 falso en capsAt (el colaborador devuelve capsAt): MaxAnisotropy DWORD en +0x6C con vecinos distintos.
//  AnisoSet/Get   textura en base+t-desplazamiento; el byte esta en base+t.
//  ClumpAtoms     clump en +0x20 (atomicList.link en +0x28/+0x2C); nodos de lista de 8 bytes en +0x60+8*ranura (16 ranuras) ordenados por una permutacion.
//  TriMat         geometria en +0x20 (matList.materials en +0x40), triangulo en +0x80 (matIndex en +0x86), tabla de 16 etiquetas en +0x100.
//  MatInit        lista en base+off (12 bytes).
//  MatGet         lista en +0x20 {materials, num, space}; tabla de 16 etiquetas en +0x100.
//  MatFind        lista en +0x20; vector de 16 punteros en +0x80.
//  AtomWorld/GeoUsage/SecGet/SecSet  objeto en base+t-desplazamiento; el DWORD esta en base+t.
inline Fx Setup(Fid f, unsigned c, u8* base) {
    Fx x{};
    FillJunk(base, c);
    auto A = [&](std::size_t at) { return AbsAt(base, at); };
    const u32 t = 0x10u + Rand(c, 3) % 0x150u;
    switch (f) {
    case IndexBuf: {
        const u32 n = kPool[c % 16];
        const unsigned ppSel = (c / 16) % 4;
        const u32 pp = ppSel == 0 ? 0u : ppSel == 1 ? A(0x40) : ppSel == 2 ? A(0x41) : 0xDEADBEEFu;
        x.nargs = 2; x.args[0] = n; x.args[1] = pp; x.stubRet = kRet[(c / 64) % 16];
    } break;
    case MaxSupported: {
        const u8 v = static_cast<u8>(c % 256);
        const std::size_t at = std::array<std::size_t, 4>{0x10, 0x11, 0x37, 0x5C}[(c / 256) % 4];
        x.capsAt = A(at);
        base[at + 0x6B] = static_cast<u8>(v ^ 0x5A); base[at + 0x6C] = v; base[at + 0x6D] = static_cast<u8>(v + 1); base[at + 0x6E] = static_cast<u8>(~v); base[at + 0x6F] = 0x7F;
    } break;
    case AnisoSet: case AnisoGet: {
        const u32 off = kOff[c % 16];
        const bool nullTex = ((c / 16) % 2) != 0;
        const u32 byteSel = (c / 32) % 32;
        x.hasGlob = true; x.glob = off;
        const u32 tex = nullTex ? 0u : static_cast<u32>(A(t) - off);
        if (f == AnisoSet) { x.nargs = 2; x.args[0] = tex; x.args[1] = (Rand(c, 7) & 0xFFFFFF00u) | kByte[byteSel]; }
        else { x.nargs = 1; x.args[0] = tex; base[t] = kByte[byteSel]; base[t - 1] = static_cast<u8>(kByte[byteSel] ^ 0xFF); base[t + 1] = static_cast<u8>(kByte[byteSel] + 1); }
    } break;
    case ClumpAtoms: {
        const unsigned n = c % 13;
        // permutacion de las 16 ranuras (Fisher-Yates determinista)
        std::array<unsigned, 16> perm{}; for (unsigned i = 0; i < 16; ++i) perm[i] = i;
        for (unsigned i = 15; i > 0; --i) { const unsigned j = Rand(c, 100 + i) % (i + 1); const unsigned tmp = perm[i]; perm[i] = perm[j]; perm[j] = tmp; }
        const std::size_t header = 0x28;
        auto node = [&](unsigned k) { return static_cast<std::size_t>(0x60 + 8 * perm[k]); };
        Put32(base, header, n ? A(node(0)) : A(header)); Put32(base, header + 4, n ? A(node(n - 1)) : A(header));
        const bool badPrev = (Rand(c, 5) & 3u) == 0;
        for (unsigned k = 0; k < n; ++k) {
            Put32(base, node(k), k + 1 < n ? A(node(k + 1)) : A(header));
            Put32(base, node(k) + 4, badPrev ? Rand(c, 200 + k) : (k ? A(node(k - 1)) : A(header)));
        }
        x.nargs = 1; x.args[0] = A(0x20);
    } break;
    case AtomAlways: case AtomRights: {
        x.nargs = 2; x.args[0] = kPool[c % 32]; x.args[1] = kPool[(c / 32) % 32]; x.stubRet = kRet[Rand(c, 9) % 16];
    } break;
    case AtomPlugOff: {
        x.nargs = 1; x.args[0] = c < 512 ? kPool[c % 32] : kPool[c % 32] ^ Rand(c, 2); x.stubRet = kRet[(c / 32) % 16];
    } break;
    case TriMat: {
        const u32 id = TriId(c % 64);
        const unsigned slot = (c / 64) % 16;
        for (unsigned s = 0; s < 16; ++s) Put32(base, 0x100 + 4 * s, 0x4D415400u + (s << 8) + (c & 0xFFu));
        for (unsigned k = 0; k < 3; ++k) { const u32 w = Rand(c, 30 + k); std::memcpy(base + 0x80 + 2 * k, &w, 2); }
        const std::uint16_t id16 = static_cast<std::uint16_t>(id); std::memcpy(base + 0x86, &id16, 2);
        const u32 materials = id == 0xFFFFu ? ((slot & 1u) ? 0xDEADBEEFu : A(0x100)) : static_cast<u32>(A(0x100 + 4 * slot) - 4u * id);
        Put32(base, 0x40, materials);
        x.nargs = 2; x.args[0] = A(0x20); x.args[1] = A(0x80);
    } break;
    case MatInit: {
        const u32 off = 0x10u + (c % 64) * 5u;
        x.nargs = 1; x.args[0] = A(off);
    } break;
    case MatGet: {
        const u32 idx = kIdx[c % 32];
        const unsigned slot = (c / 32) % 16;
        for (unsigned s = 0; s < 16; ++s) Put32(base, 0x100 + 4 * s, 0x4D415400u + (s << 8) + (c & 0xFFu));
        Put32(base, 0x20, static_cast<u32>(A(0x100 + 4 * slot) - 4u * idx));
        x.nargs = 2; x.args[0] = A(0x20); x.args[1] = idx;
    } break;
    case MatFind: {
        const u32 n = kCount[c % 16];
        const unsigned mode = (c / 16) % 8;
        const unsigned variant = (c / 128) % 8;
        const bool positive = static_cast<i32>(n) > 0;
        for (unsigned i = 0; i < 16; ++i) {
            const u32 r = Rand(c, 300 + i) % 7u;
            Put32(base, 0x80 + 4 * i, r == 6u ? 0u : 0x4D4D0000u + r);
        }
        const unsigned last = positive ? n - 1 : 0;
        u32 material = 0;
        switch (mode) {
        case 0: material = Get32(base, 0x80 + 4 * (Rand(c, 40) % 16)); break;
        case 1: material = Get32(base, 0x80); break;
        case 2: material = positive ? Get32(base, 0x80 + 4 * last) : 0x4D4D0001u; break;
        case 3: material = 0x4D4DFFFFu; break;
        case 4: material = 0; break;
        case 5: material = 0x4D4D0000u + (Rand(c, 41) % 6u); break;
        case 6: material = Get32(base, 0x80 + 4 * (positive ? Rand(c, 42) % n : 0u)); break;
        default: material = A(0x44); break;
        }
        const u32 materials = positive ? A(0x80) : (variant % 3 == 0 ? 0xDEADBEEFu : variant % 3 == 1 ? A(0x80) : 0u);
        Put32(base, 0x20, materials); Put32(base, 0x24, n);
        x.nargs = 2; x.args[0] = A(0x20); x.args[1] = material;
    } break;
    case WorldReg: case LightReg: {
        x.nargs = 5; for (unsigned k = 0; k < 5; ++k) x.args[k] = kPool[Rand(c, 50 + k) % 32];
        x.stubRet = kRet[Rand(c, 9) % 16];
    } break;
    case WorldRegStream: {
        x.nargs = 4; for (unsigned k = 0; k < 4; ++k) x.args[k] = kPool[Rand(c, 60 + k) % 32];
        x.stubRet = kRet[Rand(c, 9) % 16];
    } break;
    case VdeclSize: {
        x.nargs = 1; x.args[0] = VdeclType(c);
    } break;
    case AtomWorld: case GeoUsage: case SecGet: {
        x.hasGlob = true; x.glob = kOff[c % 16];
        Put32(base, t, 0x57000000u + Rand(c, 8) % 0xFFFFFFu);
        x.nargs = 1; x.args[0] = static_cast<u32>(A(t) - x.glob);
    } break;
    case SecSet: {
        x.hasGlob = true; x.glob = kOff[c % 16];
        x.nargs = 2; x.args[0] = static_cast<u32>(A(t) - x.glob); x.args[1] = kPool[(c / 16) % 32] ^ (c >= 512 ? Rand(c, 4) : 0u);
    } break;
    default: break;
    }
    return x;
}

// ------------------------------------------------------------------ hash y utilidades
inline std::string Hex64(std::uint64_t v) { char b[17]; for (int i = 15; i >= 0; --i) { b[i] = "0123456789abcdef"[v & 15]; v >>= 4; } b[16] = 0; return b; }
inline std::string HexBytes(const u8* p, std::size_t n) { constexpr char d[] = "0123456789abcdef"; std::string s; s.reserve(n * 2); for (std::size_t i = 0; i < n; ++i) { s += d[p[i] >> 4]; s += d[p[i] & 15]; } return s; }
inline std::uint64_t Fnv64(const u8* p, std::size_t n) { std::uint64_t h = 0xCBF29CE484222325ull; for (std::size_t i = 0; i < n; ++i) { h ^= p[i]; h *= 0x100000001B3ull; } return h; }
inline std::uintptr_t ModuleOf(std::uintptr_t a) { MEMORY_BASIC_INFORMATION i{}; return VirtualQuery(reinterpret_cast<void*>(a), &i, sizeof(i)) == sizeof(i) ? std::uintptr_t(i.AllocationBase) : 0; }
inline std::uintptr_t Resolve(std::uintptr_t a) { for (unsigned i = 0; i < 16 && *reinterpret_cast<u8*>(a) == 0xE9; ++i) a += 5 + *reinterpret_cast<i32*>(a + 1); return a; }
inline std::string CodeHex(std::uintptr_t a, std::size_t n) { return HexBytes(reinterpret_cast<const u8*>(a), n); }

// Parche temporal de 5 bytes (JMP rel32) en una entrada de codigo; restaura al destruirse.
struct Patch {
    std::uintptr_t at; u8 saved[5]{};
    Patch(std::uintptr_t a, void* fn) : at(a) {
        DWORD old{}; VirtualProtect(reinterpret_cast<void*>(a), 5, PAGE_EXECUTE_READWRITE, &old);
        std::memcpy(saved, reinterpret_cast<void*>(a), 5);
        u8 jmp[5]{0xE9}; const u32 rel = static_cast<u32>(reinterpret_cast<std::uintptr_t>(fn) - a - 5); std::memcpy(jmp + 1, &rel, 4);
        std::memcpy(reinterpret_cast<void*>(a), jmp, 5);
        FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void*>(a), 5);
    }
    ~Patch() { std::memcpy(reinterpret_cast<void*>(at), saved, 5); FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void*>(at), 5); }
    Patch(const Patch&) = delete; Patch& operator=(const Patch&) = delete;
};
// Vigila bytes de codigo: deben volver a su contenido inicial tras cada llamada.
struct Watch {
    std::uintptr_t address; std::vector<u8> saved;
    Watch(std::uintptr_t a, std::size_t n) : address(a), saved(reinterpret_cast<u8*>(a), reinterpret_cast<u8*>(a) + n) {}
    bool Intact() const { return std::memcmp(saved.data(), reinterpret_cast<void*>(address), saved.size()) == 0; }
};

// ------------------------------------------------------------------ colaboradores falsos (los mismos en el EXE y en la DLL)
inline std::vector<u32> trace;
inline u32 stubRet{}, stubCaps{};
// tipo 1: RwD3D9GetCaps() -> puntero a D3DCAPS9 falso
inline u32 __cdecl StubCaps() { trace.push_back(1); return stubCaps; }
// tipos 2/3: AddPlgnStrm{Always,Rights}CB(reg, pluginID, cb)
inline u32 __cdecl StubAlways(u32 a, u32 b, u32 c) { trace.insert(trace.end(), {2u, a, b, c}); return stubRet; }
inline u32 __cdecl StubRights(u32 a, u32 b, u32 c) { trace.insert(trace.end(), {3u, a, b, c}); return stubRet; }
// tipo 4: GetPluginOffset(reg, pluginID)
inline u32 __cdecl StubGetOff(u32 a, u32 b) { trace.insert(trace.end(), {4u, a, b}); return stubRet; }
// tipo 5: AddPlugin(reg, size, pluginID, ctor, dtor, copy)
inline u32 __cdecl StubAdd(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f) { trace.insert(trace.end(), {5u, a, b, c, d, e, f}); return stubRet; }
// tipo 6: AddPluginStream(reg, pluginID, read, write, getSize)
inline u32 __cdecl StubStream(u32 a, u32 b, u32 c, u32 d, u32 e) { trace.insert(trace.end(), {6u, a, b, c, d, e}); return stubRet; }
// tipo 7: CreateIndexBuffer(numIndices, format, ppIndexBuffer)
inline u32 __cdecl StubIndex(u32 a, u32 b, u32 c) { trace.insert(trace.end(), {7u, a, b, c}); return stubRet; }
inline void* CollabStub(unsigned kind) {
    switch (kind) {
    case 1: return reinterpret_cast<void*>(&StubCaps);
    case 2: return reinterpret_cast<void*>(&StubAlways);
    case 3: return reinterpret_cast<void*>(&StubRights);
    case 4: return reinterpret_cast<void*>(&StubGetOff);
    case 5: return reinterpret_cast<void*>(&StubAdd);
    case 6: return reinterpret_cast<void*>(&StubStream);
    case 7: return reinterpret_cast<void*>(&StubIndex);
    default: return nullptr;
    }
}

// ------------------------------------------------------------------ estado x87 / MXCSR
inline void SaveFP(void* p) { __asm {mov eax,p} __asm {fnsave [eax]} __asm {frstor [eax]} }
inline void RestoreFP(void* p) { __asm {mov eax,p} __asm {frstor [eax]} }
inline u32 ReadMXCSR() { u32 v{}; __asm {stmxcsr v} return v; }
inline void SetMXCSR(u32 v) { __asm {ldmxcsr v} }
alignas(16) inline std::array<u8, 108> savedFP{};
inline u32 savedMXCSR{}; inline bool savedFPActive{};
inline void RestoreFloatingState() { if (savedFPActive) { RestoreFP(savedFP.data()); SetMXCSR(savedMXCSR); } }
inline bool FloatingStateRestored() { alignas(16) std::array<u8, 108> current{}; SaveFP(current.data()); return current == savedFP && ReadMXCSR() == savedMXCSR; }
// Deja en los ocho registros ST un residuo conocido y distinto: el FNSAVE posterior compara tambien el contenido de los registros vacios.
inline void PrimeX87() {
    static const double stale[8]{1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
    const double* p = stale;
    __asm {mov eax,p} __asm {fld qword ptr [eax]} __asm {fld qword ptr [eax+8]} __asm {fld qword ptr [eax+16]} __asm {fld qword ptr [eax+24]}
    __asm {fld qword ptr [eax+32]} __asm {fld qword ptr [eax+40]} __asm {fld qword ptr [eax+48]} __asm {fld qword ptr [eax+56]}
    __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)}
}
// true: la igualdad EXE/DLL exige los 108 bytes del FNSAVE, incluido el contenido crudo de los registros ST marcados como vacios. No se relaja.
inline constexpr bool kCompareDeadX87Registers = true;
inline u32 FpWord(const std::array<u8, 108>& fp, std::size_t at) { u32 v{}; std::memcpy(&v, fp.data() + at, 4); return v; }
inline bool FpRegisterIsDead(const std::array<u8, 108>& fp, unsigned st) { const u32 ftw = FpWord(fp, 8) & 0xFFFF, top = (FpWord(fp, 4) >> 11) & 7; return ((ftw >> (2 * ((top + st) & 7))) & 3) == 3; }
inline std::array<u8, 108> MaskDeadRegisters(std::array<u8, 108> fp) { for (unsigned st = 0; st < 8; ++st) if (FpRegisterIsDead(fp, st)) std::memset(fp.data() + 28 + 10 * st, 0, 10); return fp; }
inline json FpDifference(const std::array<u8, 108>& a, const std::array<u8, 108>& b) {
    json out = json::array();
    const struct { const char* name; std::size_t at, size; } fields[]{{"fcw", 0, 2}, {"fsw", 4, 2}, {"ftw", 8, 2}, {"fip", 12, 4}, {"fcs_fop", 16, 4}, {"fdp", 20, 4}, {"fds", 24, 4}};
    for (const auto& f : fields) if (std::memcmp(a.data() + f.at, b.data() + f.at, f.size)) out.push_back(f.name);
    for (unsigned st = 0; st < 8; ++st) if (std::memcmp(a.data() + 28 + 10 * st, b.data() + 28 + 10 * st, 10)) out.push_back("st" + std::to_string(st) + (FpRegisterIsDead(a, st) ? "(dead)" : "(valid)"));
    return out;
}

// ------------------------------------------------------------------ llamada cdecl con 0..6 argumentos (EAX/ECX/EDX con semillas distintas, capturados justo tras la llamada)
struct Reg { u32 eax, ecx, edx; bool stack, nonvolatile; };
__declspec(noinline) inline Reg GateCdecl(std::uintptr_t entry, const u32* args, unsigned n) {
    u32 before{}, after{}, ra{}, rc{}, rd{}, b0{}, s0{}, i0{}, p0{}, b1{}, s1{}, i1{}, p1{};
    const u32 bytes = n * 4;
    __asm {mov before,esp} __asm {mov b0,ebx} __asm {mov s0,esi} __asm {mov i0,edi} __asm {mov p0,ebp}
    __asm {
        mov ecx,args
        mov edx,n
    push_loop:
        test edx,edx
        jz push_done
        dec edx
        push dword ptr [ecx+edx*4]
        jmp push_loop
    push_done:
        mov eax,0A5123456h
        mov ecx,0B1234567h
        mov edx,0C0DE5921h
        call entry
        mov ra,eax
        mov rc,ecx
        mov rd,edx
        add esp,bytes
    }
    __asm {mov after,esp} __asm {mov b1,ebx} __asm {mov s1,esi} __asm {mov i1,edi} __asm {mov p1,ebp}
    return {ra, rc, rd, before == after, b0 == b1 && s0 == s1 && i0 == i1 && p0 == p1};
}

// ------------------------------------------------------------------ ejecucion de un lado
struct Env {
    std::uintptr_t asi{}, exe{};
    bool (*jumpsToDll)(std::uintptr_t){};
    std::array<std::uintptr_t, kCollabKinds> collabDll{};   // entradas de la DLL ya resueltas (sin thunks)
};
struct Run { Reg reg{}; std::array<u8, 108> fp{}; u32 mxcsr{}, ip{}; bool routeOK{}; };
struct Side { Run run; json out; };

alignas(16) inline std::array<u8, kArena + 2 * kGuard> arenaBytes{};

// `route.Invoke(candidate, f)` pone la ruta (hook apagado/encendido) y llama a f. `fx` lo rellena Setup.
template <class Route>
inline Side RunSide(Route& route, const Env& env, Fid f, unsigned c, bool candidate) {
    const Fn& fn = kFns[f];
    Side side{};
    // reconstruye el arena y el caso
    u8* base = arenaBytes.data() + kGuard;
    std::memset(arenaBytes.data(), kGuardByte, kGuard); std::memset(base + kArena, kGuardByte, kGuard);
    const Fx fx = Setup(f, c, base);
    trace.reserve(32); trace.clear(); stubRet = fx.stubRet; stubCaps = fx.capsAt;   // sin reservas de memoria dentro de la llamada
    u32 savedGlobal{}; bool globalsRestored = true;
    if (fx.hasGlob) { savedGlobal = *reinterpret_cast<u32*>(fn.glob); *reinterpret_cast<u32*>(fn.glob) = fx.glob; }
    Run run{};
    route.Invoke(candidate, [&] {
        run.routeOK = candidate ? env.jumpsToDll(fn.entry) && ModuleOf(Resolve(fn.entry)) == env.asi : !env.jumpsToDll(fn.entry);
        std::vector<std::unique_ptr<Patch>> patches;
        if (fn.collab) {
            patches.push_back(std::make_unique<Patch>(kCollabExe[fn.collab], CollabStub(fn.collab)));
            patches.push_back(std::make_unique<Patch>(env.collabDll[fn.collab], CollabStub(fn.collab)));
        }
        // el residuo x87 se planta justo antes de la llamada (despues de crear los parches): nada intermedio puede tocar los ocho registros ST ni FIP
        RestoreFloatingState(); PrimeX87(); _controlfp(_PC_24 | _RC_NEAR, _MCW_PC | _MCW_RC);
        run.reg = GateCdecl(fn.entry, fx.args, fx.nargs);
        SaveFP(run.fp.data()); run.mxcsr = ReadMXCSR();
        return 0;
    });
    std::memcpy(&run.ip, run.fp.data() + 12, 4);
    if (fx.hasGlob) { *reinterpret_cast<u32*>(fn.glob) = savedGlobal; globalsRestored = *reinterpret_cast<u32*>(fn.glob) == savedGlobal; }
    side.run = run;
    json out;
    out["ret"] = fn.hasRet ? json(run.reg.eax & fn.retMask) : json(nullptr);
    out["state"] = Hex64(Fnv64(arenaBytes.data(), arenaBytes.size()));
    out["trace"] = trace;
    // Ninguna ruta ejecuta x87: FIP es el del ultimo fstp de PrimeX87 (en el ASI) y debe coincidir en ambos lados.
    out["fp_ip_route_ok"] = ModuleOf(run.ip) == env.asi;
    const auto logged = kCompareDeadX87Registers ? run.fp : MaskDeadRegisters(run.fp);
    out["post_floating_state"] = HexBytes(logged.data(), 108); out["post_mxcsr"] = run.mxcsr;
    out["stack_restored"] = run.reg.stack; out["nonvolatile_preserved"] = run.reg.nonvolatile; out["route_ok"] = run.routeOK;
    out["globals_restored"] = globalsRestored;
    side.out = out;
    return side;
}

// Entrada de un caso (compartida por los dos lados; el oraculo de Python simula la funcion a partir de ella).
inline json CaseInput(Fid f, unsigned c) {
    u8* base = arenaBytes.data() + kGuard;
    std::memset(arenaBytes.data(), kGuardByte, kGuard); std::memset(base + kArena, kGuardByte, kGuard);
    const Fx fx = Setup(f, c, base);
    json in = {{"fid", static_cast<unsigned>(f)}, {"case", c}, {"base", static_cast<u32>(reinterpret_cast<std::uintptr_t>(base))}, {"nargs", fx.nargs},
               {"args", std::vector<u32>(fx.args, fx.args + fx.nargs)}, {"glob", fx.hasGlob ? json(fx.glob) : json(nullptr)}, {"stub_ret", fx.stubRet}, {"caps_at", fx.capsAt},
               {"init", HexBytes(arenaBytes.data(), arenaBytes.size())}};
    return in;
}
}  // namespace Dependencies25BFix

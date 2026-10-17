#pragma once
// Fixtures deterministas del banco Dependencies21G. Solo C++ estandar (sin dependencias del juego): fixture_oracle.py repite exactamente estas
// formulas y tablas (la comprobacion cruzada esta descrita en README.md). Todos los valores float se dan como patrones de bits.
#include <array>
#include <cstdint>
namespace Dependencies21GFix {
using u32 = std::uint32_t;
constexpr unsigned kSlerpCases = 4096, kFirstTextureCases = 1024, kReplaceCases = 1536;

// Mezcla (finalizador de murmur3 de 32 bits) y generador por (caso, clave).
constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }

// ------------------------------------------------------------------ CQuaternion::Slerp (0x59C300)
// theta (32): +-0, subnormales, minimo normal, entornos de pi/2, pi y 2pi, FLT_MAX, +-inf, NaN con carga distinta y senalizantes, +-1e30, 1e-30.
inline constexpr std::array<u32, 32> kTheta{
    0x00000000, 0x80000000, 0x00000001, 0x807FFFFF, 0x00800000, 0x3A83126F, 0x3F000000, 0x3F800000,
    0x3FC90FDA, 0x3FC90FDB, 0x3FC90FDC, 0x40000000, 0x40490FDA, 0x40490FDB, 0x40490FDC, 0x40C90FDB,
    0x42C80000, 0x7F7FFFFF, 0xFF7FFFFF, 0x7F800000, 0xFF800000, 0x7FC01234, 0xFFC00001, 0x7F800001,
    0xFF800123, 0xBF000000, 0xBFC90FDB, 0x7149F2CA, 0xF149F2CA, 0x0DA24260, 0x3FC00000, 0x40400000};
// t (16)
inline constexpr std::array<u32, 16> kT{
    0x00000000, 0x3F800000, 0x3F000000, 0x3E800000, 0xBF800000, 0x40000000, 0x7FC00001, 0x7F800002,
    0x7F800000, 0xFF800000, 0x80000000, 0x00000001, 0x7149F2CA, 0xF149F2CA, 0x7F7FFFFF, 0x3F7FFFFF};
// 1/sin(theta) (8)
inline constexpr std::array<u32, 8> kInv{0x3F800000, 0x3E800000, 0x00000000, 0x80000000, 0xFFC01234, 0x7F800000, 0x7149F2CA, 0x00000003};
// 16 pares (from, to): x, y, z, w
inline constexpr std::array<std::array<u32, 8>, 16> kQuat{{
    {0x00000000, 0x00000000, 0x00000000, 0x3F800000, 0x3F3504F3, 0x00000000, 0x00000000, 0x3F3504F3},
    {0x3DCCCCCD, 0xBE4CCCCD, 0x3E99999A, 0x3F666666, 0xBF000000, 0x3F000000, 0x3F000000, 0x3F000000},
    {0x7FC00001, 0x7FC00001, 0x7FC00001, 0x7FC00001, 0xFFC00001, 0xFFC00001, 0xFFC00001, 0xFFC00001},
    {0x7FC00001, 0x7FC00002, 0x7F800001, 0xFF800002, 0xFFC00001, 0x7FC00001, 0xFFC00002, 0x7FC00002},
    {0x00000001, 0x00000002, 0x007FFFFF, 0x80000001, 0x00400000, 0x80000002, 0x00000003, 0x00000004},
    {0x00000000, 0x80000000, 0x00000000, 0x80000000, 0x80000000, 0x00000000, 0x80000000, 0x00000000},
    {0x7F800000, 0xFF800000, 0x7F800000, 0xFF800000, 0xFF800000, 0x7F800000, 0x7F800000, 0xFF800000},
    {0x7F7FFFFF, 0xFF7FFFFF, 0x7F7FFFFF, 0xFF7FFFFF, 0xFF7FFFFF, 0x7F7FFFFF, 0x7F7FFFFF, 0xFF7FFFFF},
    {0x7149F2CA, 0xF149F2CA, 0x7149F2CA, 0xF149F2CA, 0xF149F2CA, 0x7149F2CA, 0x7149F2CA, 0xF149F2CA},
    {0x7FC00001, 0x00000001, 0x00000000, 0x3F800000, 0xFFC00001, 0x80000001, 0x80000000, 0x3F800000},
    {0x80000000, 0x7FFFFFFF, 0xFFFFFFFF, 0x00000001, 0xFFFFFFFF, 0x7FFFFFFF, 0x80000001, 0x7F800001},
    {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000},
    {0x3F000000, 0x3F000000, 0x3F000000, 0x3F000000, 0x3F000000, 0x3F000000, 0x3F000000, 0x3F000000},
    {0x0DA24260, 0x0DA24260, 0x0DA24260, 0x0DA24260, 0x7149F2CA, 0x7149F2CA, 0x7149F2CA, 0x7149F2CA},
    {0x7FC00003, 0x7FFFFFFF, 0xFFFFFFFF, 0x7FC00000, 0x7FC00001, 0xFFC00003, 0x7FC00000, 0xFFFFFFFF},
    {0x00000001, 0x7F7FFFFF, 0x80000001, 0xFF7FFFFF, 0x7F7FFFFF, 0x00000001, 0xFF7FFFFF, 0x80000001}}};

struct SlerpFx {
    u32 theta, t, inv;
    u32 from[4], to[4];
    unsigned quat;   // indice en kQuat
    unsigned alias;  // 0: destino propio; 1: destino = from; 2: destino = to
};
constexpr SlerpFx Slerp(unsigned c) {
    SlerpFx f{};
    f.theta = kTheta[c % 32]; f.t = kT[c / 32 % 16]; f.inv = kInv[c / 512 % 8];
    f.quat = (c * 5 + c / 32) % 16; f.alias = (c / 3 + c / 7) % 3;
    for (unsigned i = 0; i < 4; ++i) { f.from[i] = kQuat[f.quat][i]; f.to[i] = kQuat[f.quat][4 + i]; }
    return f;
}

// ------------------------------------------------------------------ GetFirstTexture (0x734940)
struct FirstTextureFx {
    unsigned count;      // texturas en la lista (0..8)
    unsigned pad;        // desplazamiento del diccionario en el arena (0..3 palabras)
    unsigned order;      // permutacion de las ranuras donde viven las texturas (0..5)
    unsigned junk;       // relleno del arena (0..4)
    unsigned prev;       // 1: los enlaces prev son coherentes; 0: basura
};
constexpr FirstTextureFx FirstTexture(unsigned c) {
    return {c % 9, c / 9 % 4, c / 36 % 6, c / 216 % 5, c / 7 % 2};
}
// ranura (0..8) de la textura i-esima segun el orden
constexpr unsigned TextureSlot(unsigned order, unsigned i) {
    constexpr unsigned mul[6] = {1, 8, 5, 2, 4, 7};   // 8 == -1 (mod 9)
    return (i * mul[order]) % 9;
}
constexpr u32 JunkWord(unsigned junk, unsigned index) {
    switch (junk) { case 0: return 0x00000000; case 1: return 0xFFFFFFFF; case 2: return 0xA5A5A5A5; case 3: return 0x5A5A5A5A; default: return 0xC3000000u | (index & 0xFFFFFFu); }
}

// ------------------------------------------------------------------ RpGeometryReplaceOldMaterialWithNewMaterial (0x734DE0)
inline constexpr std::array<u32, 8> kRefCounts{0x0001, 0x0002, 0x0003, 0x0005, 0x7FFF, 0xFFFF, 0x0000, 0x8000};
struct ReplaceFx {
    unsigned meshes;          // numMeshes (0..8)
    unsigned materials;       // numMaterials de la lista (0..6)
    unsigned meshMat[8];      // material (0..7 del pool) de cada malla
    unsigned listMat[6];      // material de cada entrada de la lista
    unsigned oldMat, newMat;  // indices del pool (new puede ser 0..7)
    u32 refCount[8];          // refCount de cada material del pool
    unsigned firstMeshOffset; // 0 o 0x30 (el EXE lo ignora)
    unsigned serial;          // serialNum de la cabecera
    bool absent;              // true: old no esta en la lista (FindIndex devuelve -1)
};
constexpr ReplaceFx Replace(unsigned c) {
    ReplaceFx f{};
    f.meshes = c % 9; f.materials = c / 9 % 7;
    f.oldMat = Rand(c, 1) % 6;
    const unsigned kind = Rand(c, 2) % 4;
    f.newMat = kind == 0 ? f.oldMat : kind == 1 ? 6 : kind == 2 ? 7 : Rand(c, 3) % 8;
    f.absent = Rand(c, 4) % 5 == 0;
    for (unsigned i = 0; i < 8; ++i) f.meshMat[i] = Rand(c, 20 + i) % 3 == 0 ? f.oldMat : Rand(c, 10 + i) % 8;
    for (unsigned j = 0; j < 6; ++j) {
        unsigned m = Rand(c, 30 + j) % 8;
        if (f.absent && m == f.oldMat) m = (m + 1) % 8;
        f.listMat[j] = m;
    }
    if (!f.absent && f.materials > 0) f.listMat[Rand(c, 5) % f.materials] = f.oldMat;
    for (unsigned k = 0; k < 8; ++k) f.refCount[k] = kRefCounts[Rand(c, 40 + k) % 8];
    f.firstMeshOffset = Rand(c, 6) % 4 == 0 ? 0x30 : 0;
    f.serial = Rand(c, 7) & 0xFFFF;
    return f;
}
}  // namespace Dependencies21GFix

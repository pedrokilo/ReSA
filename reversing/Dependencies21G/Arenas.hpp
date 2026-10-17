#pragma once
// Construccion de los arenas falsos del banco Dependencies21G (sin dependencias del juego; fixture_oracle.py repite el diseno byte a byte).
// Un arena es un bloque propio del banco: ningun objeto del motor RenderWare se toca y no se llama a nada que dependa de su estado interno.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Fixtures.hpp"
namespace Dependencies21GFix {
inline void Put32(std::uint8_t* base, std::size_t at, std::uint32_t v) { std::memcpy(base + at, &v, 4); }
inline std::uint32_t Get32(const std::uint8_t* base, std::size_t at) { std::uint32_t v; std::memcpy(&v, base + at, 4); return v; }
inline void Put16(std::uint8_t* base, std::size_t at, std::uint16_t v) { std::memcpy(base + at, &v, 2); }
inline std::uint32_t Ptr(const std::uint8_t* base, std::size_t at) { return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(base + at)); }

// ---- GetFirstTexture ----
constexpr std::size_t kTexArena = 0x600, kTexDict = 0x10, kTexSlot0 = 0x100, kTexSlotSize = 0x80;
constexpr std::size_t DictOffset(const FirstTextureFx& f) { return kTexDict + 4 * f.pad; }
constexpr std::size_t TextureOffset(unsigned slot) { return kTexSlot0 + kTexSlotSize * slot; }
// RwTexDictionary: object(8) | link{next,prev}(+8) | lInInstance(+0x10); RwTexture: raster(0) dict(4) lInDictionary{next,prev}(+8) name(+0x10) mask(+0x30) filter(+0x50) refCount(+0x54)
inline void BuildFirstTexture(std::uint8_t* base, const FirstTextureFx& f) {
    for (std::size_t i = 0; i < kTexArena; i += 4) Put32(base, i, JunkWord(f.junk, static_cast<unsigned>(i / 4)));
    const std::size_t dict = DictOffset(f), sentinel = dict + 8;
    Put32(base, dict + 0, 0x00000006);   // object: type=6, subtype=0, flags=0, privateFlags=0
    Put32(base, dict + 4, 0);            // parent
    for (unsigned s = 0; s < 9; ++s) {   // las nueve ranuras llevan una textura plausible; solo las primeras `count` se enlazan
        const std::size_t t = TextureOffset(s);
        Put32(base, t + 0, 0xCAFE0000u + s); Put32(base, t + 4, Ptr(base, dict));
        for (unsigned k = 0; k < 8; ++k) Put32(base, t + 0x10 + 4 * k, 0x41414100u + s * 16 + k);
        Put32(base, t + 0x50, 0x1106); Put32(base, t + 0x54, 1 + static_cast<int>(s));
    }
    std::size_t previous = sentinel;     // desplazamiento del enlace anterior
    for (unsigned i = 0; i < f.count; ++i) {
        const std::size_t link = TextureOffset(TextureSlot(f.order, i)) + 8;
        Put32(base, previous + 0, Ptr(base, link));       // previous.next = link
        Put32(base, link + 4, f.prev ? Ptr(base, previous) : 0xFEEDF00Du + i);   // link.prev
        previous = link;
    }
    Put32(base, previous + 0, Ptr(base, sentinel));       // ultimo.next = centinela (o centinela.next si esta vacia)
    Put32(base, sentinel + 4, f.prev ? Ptr(base, previous) : 0xDEC0DED0u);    // centinela.prev
    Put32(base, dict + 0x10, 0x0BADF00Du); Put32(base, dict + 0x14, 0x0BADF11Du);
}
// desplazamiento esperado del resultado (-1: nulo)
constexpr long ExpectedFirstTexture(const FirstTextureFx& f) { return f.count == 0 ? -1L : static_cast<long>(TextureOffset(TextureSlot(f.order, 0))); }

// ---- RpGeometryReplaceOldMaterialWithNewMaterial ----
constexpr std::size_t kRepArena = 0x240, kRepGeometry = 0x000, kRepList = 0x0A0, kRepHeader = 0x0C0, kRepMeshes = 0x0D0, kRepPool = 0x140, kRepMaterialSize = 0x20;
constexpr std::size_t PoolOffset(unsigned k) { return kRepPool + kRepMaterialSize * k; }
// RpGeometry: matList.{materials,numMaterials,space} en +0x20; mesh en +0x54. RpMeshHeader: flags, numMeshes(u16), serialNum(u16), totalIndices, firstMeshOffset (0x10 bytes); RpMesh: indices, numIndices, material.
// RpMaterial: texture(0) color(4) pipeline(8) surfaceProps(0xC..0x17) refCount(0x18, int16) pad(0x1A).
inline void BuildReplace(std::uint8_t* base, const ReplaceFx& f) {
    std::memset(base, 0xA5, kRepArena);
    std::memset(base + 0x090, 0xC3, 0x10);   // relleno anterior a la lista: aqui cae list[-1] cuando FindIndex devuelve -1
    std::memset(base + 0x0B8, 0xC3, 0x08);
    Put32(base, kRepGeometry + 0x20, Ptr(base, kRepList)); Put32(base, kRepGeometry + 0x24, f.materials); Put32(base, kRepGeometry + 0x28, 6);
    Put32(base, kRepGeometry + 0x54, Ptr(base, kRepHeader));
    for (unsigned j = 0; j < 6; ++j) Put32(base, kRepList + 4 * j, Ptr(base, PoolOffset(f.listMat[j])));
    Put32(base, kRepHeader + 0, 0x0000BEEF); Put16(base, kRepHeader + 4, static_cast<std::uint16_t>(f.meshes)); Put16(base, kRepHeader + 6, static_cast<std::uint16_t>(f.serial));
    Put32(base, kRepHeader + 8, 0x1234); Put32(base, kRepHeader + 12, f.firstMeshOffset);
    for (unsigned i = 0; i < 8; ++i) {
        const std::size_t m = kRepMeshes + 12 * i;
        Put32(base, m + 0, 0xDA000000u | i); Put32(base, m + 4, 0x10 + i); Put32(base, m + 8, Ptr(base, PoolOffset(f.meshMat[i])));
    }
    for (unsigned k = 0; k < 8; ++k) {
        const std::size_t p = PoolOffset(k);
        Put32(base, p + 0, 0x7E000000u | k); Put32(base, p + 4, 0xFF00FF00u | k); Put32(base, p + 8, 0);
        Put32(base, p + 0xC, 0x3F800000); Put32(base, p + 0x10, 0x3F000000); Put32(base, p + 0x14, 0x3E800000);
        Put16(base, p + 0x18, static_cast<std::uint16_t>(f.refCount[k])); Put16(base, p + 0x1A, 0x5AA5);
    }
}
}  // namespace Dependencies21GFix

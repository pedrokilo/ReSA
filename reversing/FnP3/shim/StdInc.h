#pragma once
// Cabecera minima para compilar D3D9RasterDriver.cpp dentro del banco P3 (sin el resto del proyecto).
#define NOMINMAX
#include <windows.h>
#include <stdint.h>
#include <stddef.h>
typedef int8_t int8; typedef int16_t int16; typedef int32_t int32; typedef int64_t int64;
typedef uint8_t uint8; typedef uint16_t uint16; typedef uint32_t uint32; typedef uint64_t uint64;
typedef int32_t RwInt32; typedef uint32_t RwUInt32; typedef uint8_t RwUInt8; typedef int16_t RwInt16;
template<typename T> inline T& StaticRef(uintptr_t address) { return *reinterpret_cast<T*>(address); }
struct RwRaster {
    RwRaster* parent; RwUInt8* cpPixels; RwUInt8* palette; RwInt32 width, height, depth, stride; RwInt16 nOffsetX, nOffsetY;
    RwUInt8 cType, cFlags, privateFlags, cFormat; RwUInt8* originalPixels; RwInt32 originalWidth, originalHeight, originalStride;
};
static_assert(sizeof(RwRaster) == 0x34);
struct RwFreeList;
RwFreeList* RwFreeListCreate(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwUInt32 hint);
void RwFreeListDestroy(RwFreeList* list);
#define RH_ScopedClass(x)
#define RH_ScopedCategoryGlobal()
#define RH_ScopedNamedGlobalInstall(a, b, c)
// GetClientRect: el EXE la llama por la IAT (0x858304) y el C++ por esta macro; las dos van al mismo falso del banco.
extern "C++" BOOL WINAPI BenchGetClientRect(HWND, LPRECT);
#define GetClientRect BenchGetClientRect

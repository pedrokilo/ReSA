#pragma once
// Cabecera minima para compilar D3D9RasterStandards.cpp dentro del banco FnD3DStd (sin el resto del proyecto).
#define NOMINMAX
#include <windows.h>
#include <d3d9.h>
#include <stdint.h>
#include <stddef.h>
typedef int8_t int8; typedef int16_t int16; typedef int32_t int32; typedef int64_t int64;
typedef uint8_t uint8; typedef uint16_t uint16; typedef uint32_t uint32; typedef uint64_t uint64;
typedef int32_t RwInt32; typedef uint32_t RwUInt32; typedef uint8_t RwUInt8; typedef int16_t RwInt16; typedef uint16_t RwUInt16; typedef int32_t RwBool; typedef char RwChar;
template<typename T> inline T& StaticRef(uintptr_t address) { return *reinterpret_cast<T*>(address); }
struct RwRaster {
    RwRaster* parent; RwUInt8* cpPixels; RwUInt8* palette; RwInt32 width, height, depth, stride; RwInt16 nOffsetX, nOffsetY;
    RwUInt8 cType, cFlags, privateFlags, cFormat; RwUInt8* originalPixels; RwInt32 originalWidth, originalHeight, originalStride;
};
static_assert(sizeof(RwRaster) == 0x34);
struct RwStream;
struct RwTexture {
    RwRaster* raster; void* dict; void* linkNext; void* linkPrev; RwChar name[32]; RwChar mask[32]; RwUInt32 filterAddressing; RwInt32 refCount;
};
static_assert(offsetof(RwTexture, name) == 0x10 && offsetof(RwTexture, mask) == 0x30 && offsetof(RwTexture, filterAddressing) == 0x50);
#define RH_ScopedClass(x)
#define RH_ScopedCategoryGlobal()
#define RH_ScopedInstall(a, b)
#define RH_ScopedNamedGlobalInstall(a, b, c)
// Funciones de RenderWare que llama el codigo bajo prueba (en el banco son falsos con traza; el EXE las tiene parcheadas hacia los mismos falsos).
RwUInt8* RwRasterLock(RwRaster*, RwUInt8 level, RwInt32 lockMode);
RwRaster* RwRasterUnlock(RwRaster*);
RwUInt8* RwRasterLockPalette(RwRaster*, RwInt32 lockMode);
RwRaster* RwRasterUnlockPalette(RwRaster*);
RwInt32 RwRasterGetNumLevels(RwRaster*);
RwRaster* RwRasterCreate(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags);
RwBool RwRasterDestroy(RwRaster*);
RwTexture* RwTextureCreate(RwRaster*);
RwTexture* RwTextureSetName(RwTexture*, const RwChar*);
RwTexture* RwTextureSetMaskName(RwTexture*, const RwChar*);
RwUInt32 RwStreamRead(RwStream*, void*, RwUInt32);
RwStream* RwStreamWrite(RwStream*, const void*, RwUInt32);
RwBool RwStreamFindChunk(RwStream*, RwUInt32 type, RwUInt32* lengthOut, RwUInt32* versionOut);
RwStream* _rwStreamWriteVersionedChunkHeader(RwStream*, RwInt32 type, RwInt32 size, RwUInt32 version, RwUInt32 buildNum);
void _rwD3D9SetDepthStencilSurface(void* surface);
RwBool RwD3D9SetRenderTarget(RwUInt32 index, RwRaster* raster);
void RwD3D9GetRenderState(RwUInt32 state, void* value);
void RwD3D9SetRenderState(RwUInt32 state, RwUInt32 value);
RwBool _rwD3D9RWGetRenderState(RwInt32 state, void* value);
RwBool _rwD3D9RenderStateSrcBlend(RwInt32 blend);
RwBool _rwD3D9RenderStateDestBlend(RwInt32 blend);
RwBool _rwD3D9RWSetRasterStage(RwRaster* raster, RwUInt32 stage);

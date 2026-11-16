/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "rw/rwcore.h"
#include "rw/rphanim.h"
#include "rw/rpuvanim.h"
#include "rw/rpskin.h"
#include "rw/rpmatfx.h"
#include "rw/skeleton.h"
#include "rw/rwplcore.h"
#include <type_traits>

#ifdef _DX9_SDK_INSTALLED
#include "d3d9.h"
#endif

static inline auto& RwInitialized = StaticRef<bool>(0xC920E8);
static inline auto& RwEngineInstance =  StaticRef<RwGlobals*>(0xC97B24);
static inline auto& RsGlobal =  StaticRef<RsGlobalType>(0xC17040);
static inline auto& geometryTKList =  StaticRef<RwPluginRegistry>(0x8D628C);
static inline auto& RpUVAnimDictSchema =  StaticRef<RtDictSchema>(0x8DED50);
static inline auto& AmbientSaturated = StaticRef<RwRGBAReal>(0x8E2418);

inline IDirect3DDevice9 *GetD3D9Device() {
    return *reinterpret_cast<IDirect3DDevice9 **>(0xC97C28);
}

#ifndef D3DMATRIX_DEFINED
struct _D3DMATRIX;
#endif
inline _D3DMATRIX *GetD3DViewTransform() {
    return reinterpret_cast<_D3DMATRIX *>(0xC9BC80);
}

inline _D3DMATRIX *GetD3DProjTransform() {
    return reinterpret_cast<_D3DMATRIX *>(0x8E2458);
}

#define RWRSTATE(a) (reinterpret_cast<void *>(a))
#define PSGLOBAL(var) (((psGlobalType *)(RsGlobal.ps))->var)

struct RwResEntrySA : RwResEntry {
    RxD3D9ResEntryHeader header;
    RxD3D9InstanceData meshData;
};


void RwCoreInjectHooks();
void RwPlCoreInjectHooks();
void RwCoreInjectHooks25d();   // grupo g25d (biblioteca RenderWare / driver D3D9)
void RwD3D9InjectHooksR1A();     // lote R1A (sin validar): RwD3D9GetCaps y _rwD3D9DeviceGetRestoreCallback
void RwD3D9InjectHooksR6A();     // lote R6A (sin validar): estados de render y etapas de textura del controlador D3D9 (0x7FD810..0x7FDE60)
void RwCoreInjectHooksR7A();       // lote R7A (sin validar): gestor de buferes de vertices D3D9 (0x7F5500..0x7F5A00) y lectura de texturas con mipmaps (0x7F4AE0)
void RwPlCoreInjectHooks25d();

template<typename T>
inline T RwStreamRead(RwStream* stream, size_t size = sizeof(T)) {
    T data;
    RwStreamRead(stream, &data, size);
    return data;
}

namespace RtAnim {
    void InjectHooks();
}

// Funciones de biblioteca de RenderWare (lote 25, grupo g25c).
void RwCoreFrameCameraInjectHooks();
void RwPlCoreEngineStreamInjectHooks();
void RpWorldInjectHooks();
void RpHAnimInjectHooks();
void RpSkinInjectHooks();
void RpUVAnimInjectHooks();
void RtDictInjectHooks();
void RtBMPInjectHooks();         // lote R3A (sin validar): RtBMPImageRead y su ayudante de filas
void RtSlerpInjectHooks();       // lote R2A (sin validar): RtQuatSetupSlerpCache
void RtQuatInjectHooks();        // lote R5A (sin validar): RtQuatConvertFromMatrix (y sus tres ramas), RtQuatRotate y RtQuatTransformVectors
void ZlibInflateInjectHooks();   // lote R5A (sin validar): inflate de zlib 1.1.4 enlazado en el EXE (0x7E9430..0x7EB5B0)
void D3DX9InjectHooks();         // lote R4A (sin validar): matematicas de D3DX9 (3DNow!/SSE) enlazadas en el EXE

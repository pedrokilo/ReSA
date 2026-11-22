// Nodo de Rx "nodeD3D9SubmitNoLight.csl" de Im3D de D3D9 (definicion 0x8E2978, ver _rwD3D9RxNodeDefinitionGetSubmitNoLight 0x80E9A0): su nodeBody (0x80E270) era un hueco de Ghidra alcanzable
// solo por el puntero de la tabla de nodeMethods; en el EXE independiente (fase 8) lo sustituia un shim que devolvia 1 sin dibujar. Reconstruido en C++ a partir del desensamblado.
#include "StdInc.h"

#include <cstring>
#include <d3d9.h>

#include "rpworld.h"

// Funciones del EXE sin cabecera propia (rwcore.cpp / rpworld.cpp).
RwBool _rwD3D9RenderStateVertexAlphaEnable(RwBool enable); // 0x7FE0A0 (rwcore.cpp)

namespace {
inline IDirect3DDevice9* Device() { return *reinterpret_cast<IDirect3DDevice9**>(0xC97C28); } // _RwD3DDevice

auto& Im3DPoolPtr           = StaticRef<rwIm3DPool*>(0xC9AB30);       // estado de Im3D (modulo + 0x38) fijado por el nodo ImmInstance (0x80E970)
auto& Im3DBaseIndex         = StaticRef<RwUInt32>(0xC9AB34);          // primer vertice del bloque reservado en el bufer dinamico
auto& Im3DVertexBuffer      = StaticRef<void*>(0xC9AB38);             // bufer dinamico de vertices bloqueado
auto& Im3DIndexPosition     = StaticRef<RwUInt32>(0xC9AB3C);          // posicion (en indices) del siguiente bloque en el bufer de indices
auto& Im3DIndexBuffer       = StaticRef<IDirect3DIndexBuffer9*>(0xC9AB40);
auto& Im3DDeclLit           = StaticRef<void*>(0xC9AB44);             // POSITION, NORMAL, COLOR, TEXCOORD0 (36 bytes, vertice de Im3D tal cual)
auto& Im3DDeclTextured      = StaticRef<void*>(0xC9AB48);             // POSITION, COLOR, TEXCOORD0 (24 bytes)
auto& Im3DDeclPlain         = StaticRef<void*>(0xC9AB4C);             // POSITION, COLOR (16 bytes)

auto& D3D9LastFVF               = StaticRef<RwUInt32>(0x8E2440);
auto& D3D9LastVertexDeclaration = StaticRef<void*>(0x8E2444);
auto& D3D9LastVertexShader      = StaticRef<void*>(0x8E2448);
auto& D3D9LastPixelShader       = StaticRef<void*>(0x8E244C);
auto& D3D9LastIndexBuffer       = StaticRef<void*>(0x8E2450);

// RwPrimitiveType -> D3DPRIMITIVETYPE (tabla del EXE, 0x88513C; el indice 6, lista de puntos, cae fuera de la tabla y da un valor sin sentido, como en el EXE).
inline RwUInt32 D3DPrimitiveTypeOf(RwPrimitiveType type) { return reinterpret_cast<const RwUInt32*>(0x88513C)[static_cast<RwUInt32>(type)]; }

// Primitivas dibujadas por D3D segun el tipo y el numero de elementos (indices o vertices). Con un tipo fuera de 2..6 el EXE usaba una variable local sin inicializar; aqui el numero de elementos.
inline RwUInt32 NumPrimitives(RwUInt32 d3dType, RwUInt32 count) {
    switch (d3dType) {
    case D3DPT_LINELIST:     return count / 2;
    case D3DPT_LINESTRIP:    return count - 1;
    case D3DPT_TRIANGLELIST: return count / 3;
    case D3DPT_TRIANGLESTRIP:
    case D3DPT_TRIANGLEFAN:  return count - 2;
    default:                 return count;
    }
}

// Copia un vertice de Im3D (36 bytes: posicion, normal, color en +0x18, uv en +0x1C) al formato reducido: posicion, color y, si tiene textura, uv.
inline RwUInt32* CopyVertex(RwUInt32* dst, const RwUInt8* src, bool textured) {
    const auto* s = reinterpret_cast<const RwUInt32*>(src);
    dst[0] = s[0];
    dst[1] = s[1];
    dst[2] = s[2];
    dst[3] = s[6];
    if (textured) {
        dst[4] = s[7];
        dst[5] = s[8];
        return dst + 6;
    }
    return dst + 4;
}

inline void SetVertexDeclarationCached(void* declaration) {
    if (D3D9LastVertexDeclaration != declaration) {
        D3D9LastVertexDeclaration = declaration;
        D3D9LastFVF = 0xFFFFFFFF;
        Device()->SetVertexDeclaration(static_cast<IDirect3DVertexDeclaration9*>(declaration));
    }
}

inline void SetStreamZero(void* buffer, RwUInt32 stride) { RwD3D9SetStreamSource(0, buffer, 0, stride); }
} // namespace

// 0x80E270 (fase 8 cuarta ronda, sin validar; unos 1.800 bytes con la tabla de saltos; nodeBody del nodo de Rx "nodeD3D9SubmitNoLight.csl" (definicion 0x8E2978); lo llama RxPipelineExecute en las tuberias de
// render de Im3D; cdecl, (nodo, parametros) -> TRUE, ninguno de los dos se usa: trabaja con el estado de Im3D guardado en 0xC9AB30 por 0x80E970): dibuja con D3D9 sin iluminacion los vertices de Im3D.
// Estados: alfa por vertice segun ALLOPAQUE (bit 2), transformacion de mundo = ltm, estados 0x89 = 0, 0x8F = 0 y 0x88 = 0 si NOCLIP (bit 4) o 1; etapa 0 con la textura de la etapa (modulada con el difuso) o sin
// ella (SELECTARG2 difuso); declaracion de vertices de 24 bytes (con UV, bit 1) o de 16, y sin vertex ni pixel shader. Sin vertices (numElements = 0) no hace nada. Con indices (>0) y primitivas pequenas
// (indices <= vertices y <= 8) copia solo los vertices indexados a un bufer dinamico y dibuja con DrawPrimitive; si no, copia todos los vertices, vuelca los indices al bufer de indices dinamico (0xC9AB40,
// posicion 0xC9AB3C; vuelta a 0 con DISCARD si no caben en 0xFFFF) y dibuja con DrawIndexedPrimitive. Sin indices dibuja con DrawPrimitive. Si algo falla (bufer, bloqueo o dibujo) recurre al camino de
// reserva: declaracion de 36 bytes (0xC9AB44), flujo 0 vacio y DrawIndexedPrimitiveUP / DrawPrimitiveUP con los vertices originales. Siempre devuelve TRUE.
RwBool _rxD3D9SubmitNoLightNodeBody(void* /*node*/, const void* /*params*/) {
    rwIm3DPool* const pool = Im3DPoolPtr;
    _rwIm3DPoolStash& stash = pool->stash;
    const RwUInt8* const verts = static_cast<const RwUInt8*>(pool->elements);
    const RwUInt32 flags = stash.flags;

    const bool textured = (flags & rwIM3D_VERTEXUV) != 0;
    void* const declaration = textured ? Im3DDeclTextured : Im3DDeclPlain;
    const RwUInt32 stride = textured ? 0x18u : 0x10u;

    _rwD3D9RenderStateVertexAlphaEnable((flags & rwIM3D_ALLOPAQUE) ? 0 : 1);
    RwD3D9SetTransformWorld(stash.ltm);
    RwD3D9SetRenderState(0x89, 0);
    RwD3D9SetRenderState(0x8F, 0);
    RwD3D9SetRenderState(0x88, (flags & rwIM3D_NOCLIP) ? 0 : 1);

    if (_rwD3D9RWGetRasterStage(0) == nullptr) {
        RwD3D9SetTextureStageState(0, 1, 3);
        RwD3D9SetTextureStageState(0, 3, 0);
        RwD3D9SetTextureStageState(0, 4, 3);
        RwD3D9SetTextureStageState(0, 6, 0);
    } else {
        RwD3D9SetTextureStageState(0, 1, 4);
        RwD3D9SetTextureStageState(0, 2, 2);
        RwD3D9SetTextureStageState(0, 3, 0);
        RwD3D9SetTextureStageState(0, 4, 4);
        RwD3D9SetTextureStageState(0, 5, 2);
        RwD3D9SetTextureStageState(0, 6, 0);
    }

    SetVertexDeclarationCached(declaration);
    if (D3D9LastVertexShader != nullptr) {
        D3D9LastVertexShader = nullptr;
        Device()->SetVertexShader(nullptr);
    }
    if (D3D9LastPixelShader != nullptr) {
        D3D9LastPixelShader = nullptr;
        Device()->SetPixelShader(nullptr);
    }

    const RwUInt32 d3dType = D3DPrimitiveTypeOf(stash.primType);
    const RwUInt32 numVerts = pool->numElements;
    if (numVerts == 0) {
        return TRUE;
    }

    const RwUInt16* const indices = stash.indices;
    const RwUInt32 numIndices = stash.numIndices;
    RwUInt32 numPrims = 0;
    bool needFallback = false;

    if (indices != nullptr && numIndices != 0) {
        numPrims = NumPrimitives(d3dType, numIndices);
        if (numIndices <= numVerts && numIndices <= 8) {
            // Primitiva pequena: solo los vertices que se usan, en el orden de los indices.
            RwUInt32* out = nullptr;
            if (RwD3D9DynamicVertexBufferLock(stride, numIndices, &Im3DVertexBuffer, reinterpret_cast<void**>(&out), &Im3DBaseIndex)) {
                for (RwUInt32 i = 0; i < numIndices; ++i) {
                    out = CopyVertex(out, verts + indices[i] * 0x24u, textured);
                }
                RwD3D9DynamicVertexBufferUnlock(Im3DVertexBuffer);
                SetStreamZero(Im3DVertexBuffer, stride);
                _rwD3D9RenderStateFlushCache();
                const HRESULT hr = Device()->DrawPrimitive(static_cast<D3DPRIMITIVETYPE>(d3dType), Im3DBaseIndex, numPrims);
                needFallback = hr < 0;
            } else {
                needFallback = true;
            }
        } else if (Im3DIndexBuffer != nullptr && numIndices <= 0xFFFF) {
            RwUInt32* out = nullptr;
            if (RwD3D9DynamicVertexBufferLock(stride, numVerts, &Im3DVertexBuffer, reinterpret_cast<void**>(&out), &Im3DBaseIndex)) {
                for (RwUInt32 i = 0; i < numVerts; ++i) {
                    out = CopyVertex(out, verts + i * 0x24u, textured);
                }
                RwD3D9DynamicVertexBufferUnlock(Im3DVertexBuffer);

                void* indexData = nullptr;
                HRESULT hr;
                if (Im3DIndexPosition + numIndices > 0xFFFF) {
                    Im3DIndexPosition = 0;
                    hr = Im3DIndexBuffer->Lock(0, numIndices * 2, &indexData, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);
                } else {
                    hr = Im3DIndexBuffer->Lock(Im3DIndexPosition * 2, numIndices * 2, &indexData, D3DLOCK_NOOVERWRITE | D3DLOCK_NOSYSLOCK);
                }
                if (hr >= 0) {
                    std::memcpy(indexData, indices, numIndices * 2);
                    Im3DIndexBuffer->Unlock();
                    SetStreamZero(Im3DVertexBuffer, stride);
                    if (D3D9LastIndexBuffer != Im3DIndexBuffer) {
                        D3D9LastIndexBuffer = Im3DIndexBuffer;
                        Device()->SetIndices(Im3DIndexBuffer);
                    }
                    _rwD3D9RenderStateFlushCache();
                    hr = Device()->DrawIndexedPrimitive(static_cast<D3DPRIMITIVETYPE>(d3dType), Im3DBaseIndex, 0, numVerts, Im3DIndexPosition, numPrims);
                    Im3DIndexPosition += numIndices;
                    needFallback = hr < 0;
                } else {
                    needFallback = true;
                }
            } else {
                needFallback = true;
            }
        } else {
            needFallback = true;
        }

        if (needFallback) {
            // Camino de reserva: vertices originales de 36 bytes desde memoria del usuario.
            SetVertexDeclarationCached(Im3DDeclLit);
            SetStreamZero(nullptr, 0);
            if (D3D9LastIndexBuffer != nullptr) {
                D3D9LastIndexBuffer = nullptr;
                Device()->SetIndices(nullptr);
            }
            _rwD3D9RenderStateFlushCache();
            Device()->DrawIndexedPrimitiveUP(static_cast<D3DPRIMITIVETYPE>(d3dType), 0, numVerts, numPrims, indices, D3DFMT_INDEX16, verts, 0x24);
        }
        return TRUE;
    }

    // Sin indices: vertices en el orden dado.
    numPrims = NumPrimitives(d3dType, numVerts);
    RwUInt32* out = nullptr;
    if (RwD3D9DynamicVertexBufferLock(stride, numVerts, &Im3DVertexBuffer, reinterpret_cast<void**>(&out), &Im3DBaseIndex)) {
        for (RwUInt32 i = 0; i < numVerts; ++i) {
            out = CopyVertex(out, verts + i * 0x24u, textured);
        }
        RwD3D9DynamicVertexBufferUnlock(Im3DVertexBuffer);
        SetStreamZero(Im3DVertexBuffer, stride);
        _rwD3D9RenderStateFlushCache();
        const HRESULT hr = Device()->DrawPrimitive(static_cast<D3DPRIMITIVETYPE>(d3dType), Im3DBaseIndex, numPrims);
        needFallback = hr < 0;
    } else {
        needFallback = true;
    }
    if (needFallback) {
        SetVertexDeclarationCached(Im3DDeclLit);
        SetStreamZero(nullptr, 0);
        _rwD3D9RenderStateFlushCache();
        Device()->DrawPrimitiveUP(static_cast<D3DPRIMITIVETYPE>(d3dType), numPrims, verts, 0x24);
    }
    return TRUE;
}

// Fase 8 cuarta ronda (sin validar): gancho del nodeBody de SubmitNoLight reconstruido en este archivo.
void RpWorldInjectHooksNoLight() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpWorld");

    RH_ScopedGlobalInstall(_rxD3D9SubmitNoLightNodeBody, 0x80E270);
}

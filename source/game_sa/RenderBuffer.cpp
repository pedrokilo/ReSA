/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RenderBuffer
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "RenderBuffer.hpp"

namespace RenderBuffer {
auto& s_IndicesToBeStored{ StaticRef<int32>(0xC40410) };
auto& s_VerticesToBeStored{ StaticRef<int32>(0xC40414) };

// Como el original: los contadores del búfer temporal (0xC4B950/0xC4B954) son dwords con signo
// (common.h los declara uint16; aquí se leen y escriben enteros).
auto& s_TempIdxStored{ StaticRef<int32>(0xC4B954) };
auto& s_TempVtxStored{ StaticRef<int32>(0xC4B950) };

const auto IdxBufferSize = (int32)std::size(aTempBufferIndices);
const auto VtxBufferSize = (int32)std::size(TempBufferVertices.m_3d);

void InjectHooks() {
    RH_ScopedNamespace(RenderBuffer);
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(StartStoring, 0x708290);
    RH_ScopedGlobalInstall(StopStoring, 0x7077A0);
    RH_ScopedGlobalInstall(RenderStuffInBuffer, 0x707800);
    RH_ScopedGlobalInstall(ClearRenderBuffer, 0x707790);
}

// 0x708290
void StartStoring(
    int32 nIndicesNeeded,
    int32 nVerticesNeeded,
    RwImVertexIndex*& outPtrFirstIndex,
    RwIm3DVertex*& outPtrFirstVertex
) {
    assert(nIndicesNeeded > 0 && nIndicesNeeded <= IdxBufferSize);
    assert(nVerticesNeeded > 0 && nVerticesNeeded <= VtxBufferSize);

    if (s_TempIdxStored + nIndicesNeeded >= IdxBufferSize) {
        RenderBuffer::RenderStuffInBuffer();
    }

    if (s_TempVtxStored + nVerticesNeeded >= VtxBufferSize) {
        RenderBuffer::RenderStuffInBuffer();
    }

    outPtrFirstIndex = &aTempBufferIndices[s_TempIdxStored];
    outPtrFirstVertex = &TempBufferVertices.m_3d[s_TempVtxStored];

    s_IndicesToBeStored = nIndicesNeeded;
    s_VerticesToBeStored = nVerticesNeeded;
}

// 0x7077A0
void StopStoring() {
    const int32 totalNumIdx = s_TempIdxStored + s_IndicesToBeStored;
    if (s_TempIdxStored >= totalNumIdx) { // Doesn't make a lot of sense, as it will only be true if `s_IndicesToBeStored` is negative (or 0), which isn't allowed.
        s_TempVtxStored += s_VerticesToBeStored;
        s_TempIdxStored  = totalNumIdx;
    } else {
        // Como el original: bucle hasta totalNumIdx exclusivo, sumando la palabra baja del contador de vértices
        const int32 baseVtx = s_TempVtxStored;
        for (int32 n = s_TempIdxStored; n < totalNumIdx; n++) {
            aTempBufferIndices[n] += (RxVertexIndex)baseVtx;
        }
        s_TempVtxStored = baseVtx + s_VerticesToBeStored;
        s_TempIdxStored = totalNumIdx;
    }
}

// NOTSA
void Render(RwPrimitiveType primType, RwMatrix* ltm, RwUInt32 /*RwIm3DTransformFlags*/ flags, bool isIndexed) {
    if (s_TempVtxStored) {
        if (RwIm3DTransform(TempBufferVertices.m_3d, s_TempVtxStored, ltm, flags)) {
            if (isIndexed) {
                assert(aTempBufferIndices);
                RwIm3DRenderIndexedPrimitive(primType, aTempBufferIndices, s_TempIdxStored);
            } else {
                RwIm3DRenderPrimitive(primType);
            }
            RwIm3DEnd();
        }
    }
    ClearRenderBuffer();
}

// 0x707800
void RenderStuffInBuffer() {
    Render(rwPRIMTYPETRILIST, nullptr, rwIM3D_VERTEXUV);
}

// 0x707790
void ClearRenderBuffer() {
    s_TempIdxStored  = 0;
    s_TempVtxStored = 0;
}

// 0x6E7680 - I know, `LittlTest()` is missing, but nobody cares.
void RenderIfDoesntFit(int32 nIdxNeeded, int32 nVtxNeeded) {
    if (s_TempIdxStored + nIdxNeeded > IdxBufferSize || s_TempVtxStored + nVtxNeeded > VtxBufferSize) {
        RenderStuffInBuffer();
    }
}

// notsa
RwIm3DVertex* PushVertex(CVector pos, CRGBA color) {
    const auto vtx = &TempBufferVertices.m_3d[s_TempVtxStored++];

    RwIm3DVertexSetPos(vtx, pos.x, pos.y, pos.z);
    RwIm3DVertexSetRGBA(vtx, color.r, color.g, color.b, color.a);

    return vtx;
}

// notsa
RwIm3DVertex* PushVertex(CVector pos, CVector2D uv, CRGBA color) {
    const auto vtx = PushVertex(pos, color);

    RwIm3DVertexSetU(vtx, uv.x);
    RwIm3DVertexSetV(vtx, uv.y);

    return vtx;
}

// notsa
void PushIndex(int32 idx, bool useCurrentVtxAsBase) {
    idx = useCurrentVtxAsBase
        ? (int32)s_TempVtxStored + idx
        : idx;
    assert(idx >= 0);
    aTempBufferIndices[s_TempIdxStored++] = idx;
}

// notsa
void PushIndices(std::initializer_list<int32> idxs, bool useCurrentVtxAsBase) {
    for (auto idx : idxs) {
        PushIndex(idx, useCurrentVtxAsBase);
    }
}

bool CanFitVertices(int32 nVtxNeeded) {
    return s_TempVtxStored + nVtxNeeded <= VtxBufferSize;
}

}; // namespace RenderBuffer 

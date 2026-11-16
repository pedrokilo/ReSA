/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RpSkin
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rpskin.h"
#include "X87Math.h"

namespace {
// Plugin RpSkin: id 0x116; las reservas de skin usan la pista 0x30116 (0x116 | rwMEMHINTDUR_EVENT).
constexpr RwUInt32 kSkinPluginID = 0x116;
constexpr RwUInt32 kSkinMemHint  = 0x30116;

// Variables globales del modulo en el EXE (se leen y escriben por direccion, sin copiarlas).
RwInt32& SkinEngineOffset()   { return *reinterpret_cast<RwInt32*>(0xC978A0); }
RwInt32& SkinAtomicOffset()   { return *reinterpret_cast<RwInt32*>(0xC978A4); }
RwInt32& SkinGeometryOffset() { return *reinterpret_cast<RwInt32*>(0xC978A8); }
RwFreeList*& SkinFreeList()   { return *reinterpret_cast<RwFreeList**>(0xC978B8); }
RxPipeline** SkinPipelines()  { return reinterpret_cast<RxPipeline**>(0xC978C4); } // 4 entradas, hasta 0xC978D4
constexpr int kNumSkinPipelines = 4;
RwUInt32& SkinFlagE0()  { return *reinterpret_cast<RwUInt32*>(0xC978E0); }
RwUInt32& SkinFlagE4()  { return *reinterpret_cast<RwUInt32*>(0xC978E4); }
RwUInt32& SkinLimitE8() { return *reinterpret_cast<RwUInt32*>(0xC978E8); }

RpSkin*& GeometrySkin(const RpGeometry* geometry) {
    return *reinterpret_cast<RpSkin**>(const_cast<RwUInt8*>(reinterpret_cast<const RwUInt8*>(geometry)) + SkinGeometryOffset());
}

// RWERROR((codigo)) del SDK con el pluginID del EXE: RwError { 0x116, _rwerror(codigo) } + RwErrorSet (0x808820). _rwerror (0x8088D0) se llama por direccion.
void RaiseSkinError(RwInt32 code) {
    RwError error;
    error.pluginID  = kSkinPluginID;
    error.errorCode = reinterpret_cast<RwInt32(__cdecl*)(RwInt32, ...)>(0x8088D0)(code);
    RwErrorSet(&error);
}

// Copia de un float por la x87 (fld/fstp de 32 bits): un NaN senalizador se vuelve silencioso (bit 22) y el resto de patrones se conserva.
RwUInt32 X87FloatCopyBits(RwUInt32 bits) {
    if ((bits & 0x7F800000u) == 0x7F800000u && (bits & 0x007FFFFFu) != 0) {
        bits |= 0x00400000u;
    }
    return bits;
}
} // namespace

static_assert(offsetof(RpAtomic, geometry) == 0x18 && offsetof(RpAtomic, pipeline) == 0x6C && offsetof(RpGeometry, numVertices) == 0x14);

void RpSkinInjectHooks() {
    RH_ScopedNamespaceName("RpSkin");
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(RpSkinPluginAttach, 0x7C6820);
    RH_ScopedGlobalInstall(_rpSkinAtomicStreamRightsCallBack, 0x7C6B70);
    RH_ScopedGlobalInstall(_rpSkinFindUsedBones, 0x7C70C0);
    RH_ScopedGlobalInstall(_rpSkinCreateData, 0x7C7190);
    RH_ScopedGlobalInstall(RpSkinAtomicSetHAnimHierarchy, 0x7C7520);
    RH_ScopedGlobalInstall(RpSkinAtomicGetHAnimHierarchy, 0x7C7540);
    RH_ScopedGlobalInstall(RpSkinGeometryGetSkin, 0x7C7550);
    RH_ScopedGlobalInstall(RpSkinGeometrySetSkin, 0x7C7560);
    RH_ScopedGlobalInstall(RpSkinCreate, 0x7C75B0);
    RH_ScopedGlobalInstall(RpSkinGetNumBones, 0x7C77E0);
    RH_ScopedGlobalInstall(RpSkinGetVertexBoneWeights, 0x7C77F0);
    RH_ScopedGlobalInstall(RpSkinGetVertexBoneIndices, 0x7C7800);
    RH_ScopedGlobalInstall(RpSkinGetSkinToBoneMatrices, 0x7C7810);
    RH_ScopedGlobalInstall(_rpSkinPipelinesDestroy, 0x7C86F0);
    RH_ScopedGlobalInstall(_rpSkinInitialize, 0x7C8740);
    RH_ScopedGlobalInstall(_rpSkinDeinitialize, 0x7C8820);
    RH_ScopedGlobalInstall(_rpSkinNativeGeometryGetSize, 0x7C8830);
    RH_ScopedGlobalInstall(_rpSkinNativeGeometryWrite, 0x7C8840);
    RH_ScopedGlobalInstall(_rpSkinNativeGeometryRead, 0x7C88B0);
    RH_ScopedGlobalInstall(_rpSkinPipelinesCreate, 0x7C8980);
    RH_ScopedGlobalInstall(_rpSkinPipelineAtomicAttach, 0x7C89B0);
    RH_ScopedGlobalInstall(_rpSkinD3D9AtomicLimitCheckOrOff, 0x7C89C0);
    RH_ScopedGlobalInstall(_rpSkinD3D9AtomicLimitCheck, 0x7C8A00);
    RH_ScopedGlobalInstall(_rpSkinD3D9CreateVertexBuffer, 0x7C9680);
    RH_ScopedGlobalInstall(_rpSkinSplitDataCreate, 0x7C8A70);
    RH_ScopedGlobalInstall(_rpSkinSplitDataDestroy, 0x7C8B10);
    RH_ScopedGlobalInstall(_rpSkinSplitDataGetStreamSize, 0x7C8D40);
    RH_ScopedGlobalInstall(_rpSkinGenericMatrixBlend, 0x7CB6A0); // lote R6A (sin validar)
}

void RpSkinSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7C67F0)(blockSize, numBlocksToPrealloc);
}

// 0x7C6820: registra el plugin 0x116 en el motor, en los atomics y en las geometrias. Constructor/destructor/copia y las rutinas de flujo que no estan
// reconstruidas se pasan por direccion fija (0x7C68E0, 0x7C69A0, 0x7C6A80, 0x7C6AA0, 0x7C6AC0, 0x7C72A0, 0x7C7500, 0x7C7510, 0x7C6AE0, 0x7C69F0, 0x7C6A10, 0x7C6A70,
// 0x7C6D20, 0x7C6C30, 0x7C6BE0). Solo se guardan los desplazamientos del motor (0xC978A0), del atomic (0xC978A4) y de la geometria (0xC978A8); el resto de resultados
// se ignora. Devuelve siempre TRUE.
RwBool RpSkinPluginAttach() {
    SkinEngineOffset() = RwEngineRegisterPlugin(0, kSkinPluginID,
        reinterpret_cast<RwPluginObjectConstructor>(0x7C68E0), reinterpret_cast<RwPluginObjectDestructor>(0x7C69A0));
    SkinAtomicOffset() = RpAtomicRegisterPlugin(4, kSkinPluginID,
        reinterpret_cast<RwPluginObjectConstructor>(0x7C6A80), reinterpret_cast<RwPluginObjectDestructor>(0x7C6AA0), reinterpret_cast<RwPluginObjectCopy>(0x7C6AC0));
    RpAtomicRegisterPluginStream(kSkinPluginID,
        reinterpret_cast<RwPluginDataChunkReadCallBack>(0x7C72A0), reinterpret_cast<RwPluginDataChunkWriteCallBack>(0x7C7500), reinterpret_cast<RwPluginDataChunkGetSizeCallBack>(0x7C7510));
    RpAtomicSetStreamAlwaysCallBack(kSkinPluginID, reinterpret_cast<RwPluginDataChunkAlwaysCallBack>(0x7C6AE0));
    RpAtomicSetStreamRightsCallBack(kSkinPluginID, reinterpret_cast<RwPluginDataChunkRightsCallBack>(_rpSkinAtomicStreamRightsCallBack));
    SkinGeometryOffset() = RpGeometryRegisterPlugin(4, kSkinPluginID,
        reinterpret_cast<RwPluginObjectConstructor>(0x7C69F0), reinterpret_cast<RwPluginObjectDestructor>(0x7C6A10), reinterpret_cast<RwPluginObjectCopy>(0x7C6A70));
    RpGeometryRegisterPluginStream(kSkinPluginID,
        reinterpret_cast<RwPluginDataChunkReadCallBack>(0x7C6D20), reinterpret_cast<RwPluginDataChunkWriteCallBack>(0x7C6C30), reinterpret_cast<RwPluginDataChunkGetSizeCallBack>(0x7C6BE0));
    return TRUE;
}

// 0x7C6B70: callback de derechos del flujo del atomic. Si el atomic tiene geometria con skin: tipo = extraData, salvo que el plugin de motor 0x120 no este registrado
// (offset -1) y el tipo sea 2, o (si no) el 0x12E no este registrado y el tipo sea 3: en ambos casos el tipo pasa a 1. Despues engancha el pipeline con
// _rpSkinPipelineAtomicAttach (0x7C89B0). Devuelve siempre TRUE.
RwBool _rpSkinAtomicStreamRightsCallBack(void* object, RwInt32 /*offsetInObject*/, RwInt32 /*sizeInObject*/, RwUInt32 extraData) {
    RpAtomic* const atomic = static_cast<RpAtomic*>(object);
    RpGeometry* const geometry = atomic->geometry;
    if (geometry != nullptr && GeometrySkin(geometry) != nullptr) {
        RwUInt32 type = extraData;
        if (RwEngineGetPluginOffset(0x120) == -1 && static_cast<RwInt32>(type) == 2) {
            type = 1;
        } else if (RwEngineGetPluginOffset(0x12E) == -1 && static_cast<RwInt32>(type) == 3) {
            type = 1;
        }
        _rpSkinPipelineAtomicAttach(atomic, type);
    }
    return TRUE;
}

// 0x7C70C0: recorre los vertices y sus pesos (comparados como dword != 0, no como float: -0.0f cuenta) y reune sin repetir, en `bones`, los indices de hueso usados
// (byte b del dword de indices, desplazamiento 8*b con el contador de bits en CL: &31). `*numBones` queda con la cantidad; maxNumWeightsPerVertex sale de skin+0x10.
void _rpSkinFindUsedBones(RpSkin* skin, const RwUInt32* vertexIndices, const RwMatrixWeights* vertexWeights, RwUInt8* bones, RwUInt32* numBones, RwUInt32 numVertices) {
    *numBones = 0;
    for (RwUInt32 v = 0; v < numVertices; ++v) {
        const RwUInt32* weight = reinterpret_cast<const RwUInt32*>(vertexWeights + v);
        RwUInt32 shift = 0;
        for (RwUInt32 b = 0; b < skin->maxNumWeightsPerVertex; ++b, ++weight, shift += 8) {
            if (*weight == 0) {
                continue;
            }
            const RwUInt32 count = *numBones;
            const RwUInt32 value = (vertexIndices[v] >> (shift & 31)) & 0xFF;
            RwUInt32 k = 0;
            while (k < count && value != bones[k]) {
                ++k;
            }
            if (k == count) {
                bones[count] = static_cast<RwUInt8>(value);
                *numBones = count + 1;
            }
        }
    }
}

// 0x7C7190: reserva el bloque unico de datos del skin (RwMalloc, pista 0x30116): numBones*64 + numVertices*20 + numUsedBones + 15 bytes
// a cero, y reparte huesos usados (+8), matrices (+0xC, alineadas a 16), indices (+0x14) y pesos (+0x18). Copia las fuentes no nulas en este orden: huesos usados,
// matrices (de la ultima a la primera), indices y pesos. Si la reserva falla deja skin+0x3C en NULL y devuelve FALSE (no libera el skin).
RwBool _rpSkinCreateData(RpSkin* skin, RwUInt32 numBones, RwUInt32 numUsedBones, RwUInt32 numVertices, const RwUInt8* usedBones, const RwMatrixWeights* weights, const RwUInt32* indices, const RwMatrix* matrices) {
    const RwUInt32 matricesBytes = numBones << 6;
    const RwUInt32 totalSize = matricesBytes + numVertices * 20 + numUsedBones + 0xF;
    RwUInt8* const block = static_cast<RwUInt8*>(RwMalloc(totalSize, kSkinMemHint));
    skin->allocation = block;
    if (block == nullptr) {
        return FALSE;
    }
    memset(block, 0, totalSize);
    skin->numBones     = numBones;
    skin->numUsedBones = numUsedBones;
    RwUInt8* const aligned = reinterpret_cast<RwUInt8*>((reinterpret_cast<uintptr_t>(block) + numUsedBones + 0xF) & ~static_cast<uintptr_t>(0xF));
    skin->skinToBoneMatrices = reinterpret_cast<RwMatrix*>(aligned);
    skin->vertexBoneIndices  = reinterpret_cast<RwUInt32*>(aligned + matricesBytes);
    skin->vertexBoneWeights  = reinterpret_cast<RwMatrixWeights*>(aligned + matricesBytes + numVertices * 4);
    skin->usedBonesArray     = block;
    if (usedBones != nullptr && numUsedBones != 0) {
        memcpy(block, usedBones, numUsedBones);
    }
    if (matrices != nullptr && skin->numBones != 0) {
        for (RwUInt32 i = skin->numBones; i-- > 0;) {
            memcpy(aligned + (static_cast<size_t>(i) << 6), reinterpret_cast<const RwUInt8*>(matrices) + (static_cast<size_t>(i) << 6), 0x40);
        }
    }
    if (indices != nullptr) {
        memcpy(skin->vertexBoneIndices, indices, numVertices * 4);
    }
    if (weights != nullptr) {
        memcpy(skin->vertexBoneWeights, weights, numVertices << 4);
    }
    return TRUE;
}

// 0x7C7520: guarda la jerarquia en el dato del plugin del atomic (desplazamiento 0xC978A4) y devuelve el atomic. Sin comprobaciones.
RpAtomic* RpSkinAtomicSetHAnimHierarchy(RpAtomic* atomic, RpHAnimHierarchy* hierarchy) {
    *reinterpret_cast<RpHAnimHierarchy**>(reinterpret_cast<RwUInt8*>(atomic) + SkinAtomicOffset()) = hierarchy;
    return atomic;
}

// 0x7C7540: lee la jerarquia del dato del plugin del atomic.
RpHAnimHierarchy* RpSkinAtomicGetHAnimHierarchy(const RpAtomic* atomic) {
    return *reinterpret_cast<RpHAnimHierarchy* const*>(reinterpret_cast<const RwUInt8*>(atomic) + SkinAtomicOffset());
}

// 0x7C7560: si el skin es el mismo, no hace nada. Si habia otro, llama a _rpSkinDeinitialize (0x7C8820); guarda el nuevo y, si no es NULL, llama a _rpSkinInitialize (0x7C8740):
// si esta devuelve NULL, devuelve NULL; si no, la geometria.
RpGeometry* RpSkinGeometrySetSkin(RpGeometry* geometry, RpSkin* skin) {
    RpSkin* const old = GeometrySkin(geometry);
    if (skin == old) {
        return geometry;
    }
    if (old != nullptr) {
        _rpSkinDeinitialize(geometry);
    }
    GeometrySkin(geometry) = skin;
    if (skin != nullptr) {
        if (_rpSkinInitialize(geometry) == nullptr) {
            return nullptr;
        }
    }
    return geometry;
}

// 0x7C7550: puntero al skin guardado en el dato del plugin de la geometria (desplazamiento 0xC978A8).
RpSkin* RpSkinGeometryGetSkin(RpGeometry* geometry) {
    return GeometrySkin(geometry);
}

// 0x7C75B0: crea un RpSkin. Toma la entrada de la lista libre 0xC978B8 (pista 0x30116, sin comprobar), la pone a cero (0x40 bytes), fija maxNumWeightsPerVertex a 1 y lo
// sube por los pesos (dword != 0) hasta 4; busca los huesos usados con _rpSkinFindUsedBones en un array local de 256, reserva numBones*64 + numVertices*20 + usados + 15 bytes
// (pista 0x30116: si falla libera el skin en la lista libre y devuelve NULL) y los reparte/copia como _rpSkinCreateData (mismo orden: huesos usados, matrices de la
// ultima a la primera, indices, pesos).
RpSkin* RpSkinCreate(RwUInt32 numVertices, RwUInt32 numBones, RwMatrixWeights* vertexWeights, RwUInt32* vertexIndices, RwMatrix* inverseMatrices) {
    RwUInt8 bones[256];
    RwUInt32 numUsed = 0;
    RpSkin* const skin = static_cast<RpSkin*>(RwFreeListAlloc(SkinFreeList(), kSkinMemHint));
    memset(skin, 0, sizeof(RpSkin));
    skin->maxNumWeightsPerVertex = 1;
    for (RwUInt32 v = 0; v < numVertices; ++v) {
        RwUInt32 w = skin->maxNumWeightsPerVertex;
        if (w >= 4) {
            continue;
        }
        const RwUInt32* weight = reinterpret_cast<const RwUInt32*>(vertexWeights) + (w + v * 4);
        bool finished = false;
        while (*weight != 0) {
            skin->maxNumWeightsPerVertex = skin->maxNumWeightsPerVertex + 1;
            if (skin->maxNumWeightsPerVertex == 4) {
                finished = true;
                break;
            }
            ++w;
            ++weight;
            if (!(w < 4)) {
                break;
            }
        }
        if (finished) {
            break;
        }
    }
    _rpSkinFindUsedBones(skin, vertexIndices, vertexWeights, bones, &numUsed, numVertices);
    const RwUInt32 matricesBytes = numBones << 6;
    const RwUInt32 totalSize = matricesBytes + numVertices * 20 + numUsed + 0xF;
    RwUInt8* const block = static_cast<RwUInt8*>(RwMalloc(totalSize, kSkinMemHint));
    skin->allocation = block;
    if (block == nullptr) {
        RwFreeListFree(SkinFreeList(), skin);
        return nullptr;
    }
    memset(block, 0, totalSize);
    skin->numBones     = numBones;
    skin->numUsedBones = numUsed;
    RwUInt8* const aligned = reinterpret_cast<RwUInt8*>((reinterpret_cast<uintptr_t>(block) + numUsed + 0xF) & ~static_cast<uintptr_t>(0xF));
    skin->skinToBoneMatrices = reinterpret_cast<RwMatrix*>(aligned);
    skin->vertexBoneIndices  = reinterpret_cast<RwUInt32*>(aligned + matricesBytes);
    skin->vertexBoneWeights  = reinterpret_cast<RwMatrixWeights*>(aligned + matricesBytes + numVertices * 4);
    skin->usedBonesArray     = block;
    if (numUsed != 0) {
        memcpy(block, bones, numUsed);
    }
    if (inverseMatrices != nullptr && skin->numBones != 0) {
        for (RwUInt32 i = skin->numBones; i-- > 0;) {
            memcpy(aligned + (static_cast<size_t>(i) << 6), reinterpret_cast<const RwUInt8*>(inverseMatrices) + (static_cast<size_t>(i) << 6), 0x40);
        }
    }
    if (vertexIndices != nullptr) {
        memcpy(skin->vertexBoneIndices, vertexIndices, numVertices * 4);
    }
    if (vertexWeights != nullptr) {
        memcpy(skin->vertexBoneWeights, vertexWeights, numVertices << 4);
    }
    return skin;
}

RpSkin* RpSkinDestroy(RpSkin* skin) {
    return ((RpSkin*(__cdecl *)(RpSkin*))0x7C77A0)(skin);
}

// 0x7C77E0: primer dword del skin.
RwUInt32 RpSkinGetNumBones(RpSkin* skin) {
    return skin->numBones;
}

// 0x7C77F0: skin+0x18.
/*const*/ RwMatrixWeights* RpSkinGetVertexBoneWeights(RpSkin* skin) {
    return skin->vertexBoneWeights;
}

// 0x7C7800: skin+0x14.
const RwUInt32* RpSkinGetVertexBoneIndices(RpSkin* skin) {
    return skin->vertexBoneIndices;
}

// 0x7C7810: skin+0x0C.
const RwMatrix* RpSkinGetSkinToBoneMatrices(RpSkin* skin) {
    return skin->skinToBoneMatrices;
}

RwBool RpSkinIsSplit(RpSkin* skin) {
    return ((RwBool(__cdecl *)(RpSkin*))0x7C7820)(skin);
}

RpAtomic* RpSkinAtomicSetType(RpAtomic* atomic, RpSkinType type) {
    return ((RpAtomic*(__cdecl *)(RpAtomic*, RpSkinType))0x7C7830)(atomic, type);
}

RpSkinType RpSkinAtomicGetType(RpAtomic* atomic) {
    return ((RpSkinType(__cdecl *)(RpAtomic*))0x7C7880)(atomic);
}

// 0x7C86F0: destruye (0x805820) los cuatro pipelines de skin guardados en 0xC978C4..0xC978D4 que no sean NULL y los pone a NULL. Devuelve TRUE.
RwBool _rpSkinPipelinesDestroy() {
    for (int i = 0; i < kNumSkinPipelines; ++i) {
        if (SkinPipelines()[i] != nullptr) {
            _rxPipelineDestroy(SkinPipelines()[i]);
            SkinPipelines()[i] = nullptr;
        }
    }
    return TRUE;
}

// 0x7C8740: por cada vertice cuyo primer peso (como entero con signo) sea < 0x3F800000 (1.0f), ordena sus cuatro pesos de mayor a menor por burbuja repetida hasta que una pasada
// no cambie nada, comparando los dwords como enteros con signo y permutando a la vez los cuatro bytes de indices de hueso (el intercambio del peso inferior pasa por la x87:
// un NaN senalizador sale silencioso). Despues ordena con qsort del EXE (0x8247E0, comparador 0x7C8720) los huesos usados. Con un skin NULL el EXE lo desreferencia (se conserva).
RpGeometry* _rpSkinInitialize(RpGeometry* geometry) {
    RpSkin* const skin = GeometrySkin(geometry);
    if (skin != nullptr) {
        RwInt32* weight = reinterpret_cast<RwInt32*>(skin->vertexBoneWeights);
        RwUInt8* index = reinterpret_cast<RwUInt8*>(skin->vertexBoneIndices);
        for (RwInt32 v = 0; v < geometry->numVertices; ++v, weight += 4, index += 4) {
            if (weight[0] >= 0x3F800000) {
                continue;
            }
            RwInt32 swapped;
            do {
                swapped = 0;
                if (!(weight[0] >= weight[1])) {
                    const RwUInt8 t = index[0];
                    index[0] = index[1];
                    index[1] = t;
                    const RwInt32 other = weight[1];
                    weight[1] = static_cast<RwInt32>(X87FloatCopyBits(static_cast<RwUInt32>(weight[0])));
                    weight[0] = other;
                    swapped = 1;
                }
                if (!(weight[1] >= weight[2])) {
                    const RwUInt8 t = index[1];
                    index[1] = index[2];
                    index[2] = t;
                    const RwInt32 other = weight[2];
                    weight[2] = static_cast<RwInt32>(X87FloatCopyBits(static_cast<RwUInt32>(weight[1])));
                    weight[1] = other;
                    swapped = 1;
                }
                if (!(weight[2] >= weight[3])) {
                    const RwUInt8 t = index[2];
                    index[2] = index[3];
                    index[3] = t;
                    const RwInt32 other = weight[3];
                    weight[3] = static_cast<RwInt32>(X87FloatCopyBits(static_cast<RwUInt32>(weight[2])));
                    weight[2] = other;
                    swapped = 1;
                }
            } while (swapped != 0);
        }
    }
    if (skin->usedBonesArray != nullptr) {
        ((void(__cdecl *)(void*, RwUInt32, RwUInt32, void*))0x8247E0)(skin->usedBonesArray, skin->numUsedBones, 1, reinterpret_cast<void*>(0x7C8720));
    }
    return geometry;
}

// 0x7C8820: `mov eax, [esp+4]; ret`: devuelve la geometria sin tocar nada.
RpGeometry* _rpSkinDeinitialize(RpGeometry* geometry) {
    return geometry;
}

// 0x7C8830: `xor eax, eax; ret`: tamano 0 del dato nativo de la geometria (la geometria es el unico argumento de pila).
RwInt32 _rpSkinNativeGeometryGetSize(const RpGeometry* /*geometry*/) {
    return 0;
}

// 0x7C8840: escribe el dato nativo del skin: cabecera de chunk versionada (struct = 1, tamano -12, version 0x36003, build 0xFFFF), el entero 9 y el primer dword del skin
// (numBones). Devuelve el flujo si todo fue bien, NULL si no.
RwStream* _rpSkinNativeGeometryWrite(RwStream* stream, const RpGeometry* geometry) {
    const RwInt32 marker = 9;
    if (_rwStreamWriteVersionedChunkHeader(stream, 1, -12, 0x36003, 0xFFFF) == nullptr) {
        return nullptr;
    }
    if (RwStreamWriteInt32(stream, &marker, 4) == nullptr) {
        return nullptr;
    }
    const RpSkin* const skin = GeometrySkin(geometry);
    return RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(skin), 4) != nullptr ? stream : nullptr;
}

// 0x7C88B0: lee el dato nativo: busca el chunk struct; version fuera de 0x34000..0x36003 -> RWERROR(0x80000004) con pluginID 0x116 y NULL; el entero siguiente debe ser 9;
// toma un RpSkin de la lista libre (pista 0x30116), lo pone a cero y lee numBones en su primer dword (si falla, NULL y el skin queda sin liberar, como en el EXE);
// RpSkinGeometrySetSkin(geometry, skin) y devuelve el flujo.
RwStream* _rpSkinNativeGeometryRead(RwStream* stream, RpGeometry* geometry) {
    RwUInt32 length;
    RwUInt32 version;
    if (!RwStreamFindChunk(stream, 1, &length, &version)) {
        return nullptr;
    }
    if (version < 0x34000 || version > 0x36003) {
        RaiseSkinError(static_cast<RwInt32>(0x80000004));
        return nullptr;
    }
    RwInt32 marker;
    if (RwStreamReadInt32(stream, &marker, 4) == nullptr) {
        return nullptr;
    }
    if (marker != 9) {
        return nullptr;
    }
    RpSkin* const skin = static_cast<RpSkin*>(RwFreeListAlloc(SkinFreeList(), kSkinMemHint));
    memset(skin, 0, sizeof(RpSkin));
    if (RwStreamReadInt32(stream, reinterpret_cast<RwInt32*>(skin), 4) == nullptr) {
        return nullptr;
    }
    RpSkinGeometrySetSkin(geometry, skin);
    return stream;
}

// 0x7C8980: si el bit 0 de `flags` (solo el byte bajo) esta puesto, crea el pipeline generico con 0x7CB2B0 y lo guarda en 0xC978C4 (aunque sea NULL; entonces devuelve FALSE).
// Sin el bit no toca nada. Devuelve TRUE por defecto.
RwBool _rpSkinPipelinesCreate(RwUInt32 flags) {
    RwBool result = TRUE;
    if ((flags & 1) != 0) {
        RxPipeline* const pipeline = ((RxPipeline*(__cdecl *)(void))0x7CB2B0)();
        if (pipeline == nullptr) {
            result = FALSE;
        }
        SkinPipelines()[0] = pipeline;
    }
    return result;
}

// 0x7C89B0: pone el pipeline del atomic (RpAtomic+0x6C) al pipeline generico 0xC978C4 y devuelve el atomic. El segundo argumento (tipo) que le pasan los llamadores no se lee.
RpAtomic* _rpSkinPipelineAtomicAttach(RpAtomic* atomic, RwUInt32 /*type*/) {
    atomic->pipeline = SkinPipelines()[0];
    return atomic;
}

// 0x7C89C0: TRUE si 0xC978E0 es 0; si no, FALSE si 0xC978E4 es 0 o el atomic no tiene geometria con skin; si no, (skin+0x1C <= [0xC978E8]) sin signo. Sin comprobar geometria NULL.
RwBool _rpSkinD3D9AtomicLimitCheckOrOff(RpAtomic* atomic) {
    if (SkinFlagE0() == 0) {
        return TRUE;
    }
    if (SkinFlagE4() == 0) {
        return FALSE;
    }
    const RpSkin* const skin = GeometrySkin(atomic->geometry);
    if (skin == nullptr) {
        return FALSE;
    }
    return skin->field1C <= SkinLimitE8() ? TRUE : FALSE;
}

// 0x7C8A00: igual que 0x7C89C0 pero FALSE cuando 0xC978E0 es 0.
RwBool _rpSkinD3D9AtomicLimitCheck(RpAtomic* atomic) {
    if (SkinFlagE0() == 0) {
        return FALSE;
    }
    if (SkinFlagE4() == 0) {
        return FALSE;
    }
    const RpSkin* const skin = GeometrySkin(atomic->geometry);
    if (skin == nullptr) {
        return FALSE;
    }
    return skin->field1C <= SkinLimitE8() ? TRUE : FALSE;
}

// 0x7C9680 (1810 bytes; el listado dudaba de la clase: es RpSkin, la llaman 0x7C7830 y 0x7C9031 de este modulo): callback de instanciado del pipeline D3D9 de skin
// (RxD3D9ResEntryHeader en el segundo argumento; el tercero es "reinstanciar"). cdecl, sin SEH. Pasos:
//  1. RwD3D9GetCaps (0x7FAD20), skin de la geometria, flags de la geometria y RpD3D9GeometryGetUsageFlags (0x7588D0).
//  2. Libera los dos flujos de vertices del header: gestionado -> 0x7F56A0(stride, totalNumVertex*stride, vb, offset); no gestionado con offset 0 y sin dynamicLock -> 0x7F5AE0(vb);
//     deja vertexBuffer/offset/stride/geometryFlags/managed/dynamicLock a 0.
//  3. Numero de flujos: 1 (flujo 0 gestionado) si (usage & 0xFFF) == 0 y (no hay skin, skin+0x20 != 0, no es reinstanciado o _rpSkinD3D9AtomicLimitCheckOrOff devuelve != 0); si no, 2
//     (flujo 0 no gestionado, flujo 1 gestionado) y _rpD3D9AddDynamicGeometry (0x74E970).
//  4. Arma los D3DVERTEXELEMENT9: con skin y skin+0x20 != 0 la disposicion con pesos/indices (posicion FLOAT3; normales FLOAT3 o DEC3N si D3DCAPS9.DeclTypes lo admite; tangentes con
//     usage & 0x10000; color D3DCOLOR si flags & 8; coordenadas de textura con _rpD3D9FindFormatV2d (0x755AA0) y _rpD3D9VertexDeclarationGetSize (0x7522E0); pesos e indices en el
//     ultimo flujo con el formato mas compacto que admitan DeclTypes y maxNumWeightsPerVertex); si no, la disposicion simple (FLOAT3, normales FLOAT3, color, FLOAT2). Cada elemento fija
//     su Offset al stride actual del flujo y lo incrementa; los geometryFlags del flujo reciben el bit del componente. Termina con D3DDECL_END y RwD3D9CreateVertexDeclaration (0x7FAA30).
//  5. Reduce el numero de flujos a los que tienen stride != 0 (minimo 1). Con un flujo gestionado: 0x7F5500(stride, size, &vb, &offset) (FALSE si falla) y baseIndex =
//     offset / stride + minVert por malla; si no, por flujo: gestionado con D3DCAPS9.DevCaps2 & 1 -> 0x7F5500; si no, con skin+0x20 != 0 (o sin DevCaps2 y mas de un flujo) buffer dinamico
//     0x7F5A00(size, &vb), si no dynamicLock = 1; managed y offset a 0. Con mas de un flujo: useOffsets = DevCaps2 & 1 y baseIndex = minVert por malla.
//     Los bucles por malla son do/while como en el EXE (con numMeshes == 0 el EXE no termina; no se protege). Devuelve TRUE.
RwBool _rpSkinD3D9CreateVertexBuffer(RpAtomic* atomic, RxD3D9ResEntryHeader* header, RwBool reinstance) {
    constexpr RwUInt32 kDeclTypeUByte4  = 0x01;
    constexpr RwUInt32 kDeclTypeUByte4N = 0x02;
    constexpr RwUInt32 kDeclTypeShort2N = 0x04;
    constexpr RwUInt32 kDeclTypeShort4N = 0x08;
    constexpr RwUInt32 kDeclTypeUShort2N = 0x10;
    constexpr RwUInt32 kDeclTypeUShort4N = 0x20;
    constexpr RwUInt32 kDeclTypeDec3N   = 0x80;
    constexpr RwUInt32 kDevCaps2StreamOffset = 1;
    struct VertexElement { RwUInt16 stream; RwUInt16 offset; RwUInt8 type; RwUInt8 method; RwUInt8 usage; RwUInt8 usageIndex; };
    static_assert(sizeof(VertexElement) == 8);

    const D3DCAPS9* const caps = static_cast<const D3DCAPS9*>(RwD3D9GetCaps());
    RpGeometry* const geometry = atomic->geometry;
    RpSkin* const skin = GeometrySkin(geometry);
    const RwUInt32 geometryFlags = geometry->flags;
    const RwUInt32 usage = static_cast<RwUInt32>(RpD3D9GeometryGetUsageFlags(geometry));
    const RwUInt32 numTexCoordSets = static_cast<RwUInt32>(geometry->numTexCoordSets);
    RxD3D9VertexStream* const streams = header->vertexStream;

    // 2. liberar los flujos anteriores.
    for (int s = 0; s < 2; ++s) {
        RxD3D9VertexStream& stream = streams[s];
        if (stream.vertexBuffer != nullptr) {
            const RwUInt32 offset = stream.offset;
            if (stream.managed != 0) {
                ((void(__cdecl *)(RwUInt32, RwUInt32, void*, RwUInt32))0x7F56A0)(stream.stride, header->totalNumVertex * stream.stride, stream.vertexBuffer, offset);
            } else if (offset == 0 && stream.dynamicLock == 0) {
                ((void(__cdecl *)(void*))0x7F5AE0)(stream.vertexBuffer);
            }
            stream.vertexBuffer = nullptr;
        }
        stream.offset        = 0;
        stream.stride        = 0;
        stream.geometryFlags = 0;
        stream.managed       = 0;
        stream.dynamicLock   = 0;
    }

    // 3. numero de flujos.
    RwUInt32 numStreams;
    if ((usage & 0xFFF) == 0 && (skin == nullptr || skin->field20 != 0 || reinstance == 0 || _rpSkinD3D9AtomicLimitCheckOrOff(atomic) != 0)) {
        numStreams = 1;
        streams[0].managed = 1;
    } else {
        streams[0].managed = 0;
        streams[1].managed = 1;
        _rpD3D9AddDynamicGeometry(geometry);
        numStreams = 2;
    }
    const RwUInt16 lastStream = static_cast<RwUInt16>(numStreams - 1);

    // 4. elementos de la declaracion.
    VertexElement elements[16];
    RwUInt32 n;
    const RwUInt8 flagsByte = static_cast<RwUInt8>(geometryFlags);
    const RwUInt32 declTypes = caps->DeclTypes;
    if (skin != nullptr && skin->field20 != 0) {
        RwUInt16 sIdx = (usage & 2) ? 0 : lastStream;
        elements[0] = { sIdx, static_cast<RwUInt16>(streams[sIdx].stride), 2, 0, 0, 0 };
        streams[sIdx].stride = 0xC;
        streams[sIdx].geometryFlags = 2;
        n = 1;
        if (flagsByte & 0x10) {
            sIdx = (usage & 4) ? 0 : lastStream;
            const bool dec3n = (declTypes & kDeclTypeDec3N) != 0;
            elements[1] = { sIdx, static_cast<RwUInt16>(streams[sIdx].stride), static_cast<RwUInt8>(dec3n ? 0x0E : 2), 0, 3, 0 };
            streams[sIdx].stride += dec3n ? 4 : 0xC;
            streams[sIdx].geometryFlags |= 4;
            n = 2;
        }
        if (usage & 0x10000) {
            sIdx = (usage & 0xFF2) ? 0 : lastStream;
            const bool dec3n = (declTypes & kDeclTypeDec3N) != 0;
            elements[n] = { sIdx, static_cast<RwUInt16>(streams[sIdx].stride), static_cast<RwUInt8>(dec3n ? 0x0E : 2), 0, 6, 0 };
            streams[sIdx].stride += dec3n ? 4 : 0xC;
            ++n;
        }
        if (flagsByte & 8) {
            sIdx = (usage & 8) ? 0 : lastStream;
            elements[n] = { sIdx, static_cast<RwUInt16>(streams[sIdx].stride), 4, 0, 0x0A, 0 };
            streams[sIdx].stride += 4;
            streams[sIdx].geometryFlags |= 8;
            ++n;
        }
        for (RwUInt32 t = 0; t < numTexCoordSets; ++t) {
            sIdx = (usage & (0x10u << t)) ? 0 : lastStream;
            elements[n].stream = sIdx;
            elements[n].offset = static_cast<RwUInt16>(streams[sIdx].stride);
            const RwUInt32 format = _rpD3D9FindFormatV2d(reinterpret_cast<const RwV2d*>(geometry->texCoords[t]), static_cast<RwInt32>(header->totalNumVertex));
            elements[n].type       = static_cast<RwUInt8>(format);
            elements[n].method     = 0;
            elements[n].usage      = 5;
            elements[n].usageIndex = static_cast<RwUInt8>(t);
            const RwUInt32 size = _rpD3D9VertexDeclarationGetSize(format & 0xFF);
            streams[sIdx].stride += size;
            streams[sIdx].geometryFlags |= static_cast<RwUInt16>(0x10u << t);
            ++n;
        }
        const RwUInt32 maxWeights = skin->maxNumWeightsPerVertex;
        if (maxWeights > 1) {
            RxD3D9VertexStream& stream = streams[lastStream];
            elements[n].stream = lastStream;
            elements[n].offset = static_cast<RwUInt16>(stream.stride);
            RwUInt8 type;
            RwUInt32 size;
            // Cadena de formatos de cuatro pesos (maxNumWeightsPerVertex >= 4, o == 3 sin DEC3N).
            auto chooseFour = [&]() {
                if (declTypes & kDeclTypeUShort4N) {
                    type = 0x0C; size = 8;
                } else if (declTypes & kDeclTypeShort4N) {
                    type = 0x0A; size = 8;
                } else if (declTypes & kDeclTypeUByte4N) {
                    type = 8; size = 4;
                } else {
                    type = 4; size = 4;
                }
            };
            if (maxWeights >= 4) {
                chooseFour();
            } else if (maxWeights >= 3) {
                if (declTypes & kDeclTypeDec3N) {
                    type = 0x0E; size = 4;
                } else {
                    chooseFour();
                }
            } else if (declTypes & kDeclTypeUShort2N) {
                type = 0x0B; size = 4;
            } else if (declTypes & kDeclTypeShort2N) {
                type = 9; size = 4;
            } else if (declTypes & kDeclTypeDec3N) {
                type = 0x0E; size = 4;
            } else if (declTypes & kDeclTypeUByte4N) {
                type = 8; size = 4;
            } else {
                type = 4; size = 4;
            }
            elements[n].type       = type;
            stream.stride          = stream.stride + size;
            elements[n].method     = 0;
            elements[n].usage      = 1;
            elements[n].usageIndex = 0;
            ++n;
        }
        {
            RxD3D9VertexStream& stream = streams[lastStream];
            elements[n].stream = lastStream;
            elements[n].offset = static_cast<RwUInt16>(stream.stride);
            RwUInt8 type;
            RwUInt32 size;
            if (maxWeights >= 3) {
                if (declTypes & kDeclTypeUByte4) {
                    type = 5; size = 4;
                } else {
                    type = 7; size = 8;
                }
            } else {
                type = 6; size = 4;
            }
            elements[n].type       = type;
            stream.stride          = stream.stride + size;
            elements[n].method     = 0;
            elements[n].usage      = 2;
            elements[n].usageIndex = 0;
            ++n;
        }
    } else {
        elements[0] = { 0, static_cast<RwUInt16>(streams[0].stride), 2, 0, 0, 0 };
        streams[0].stride = 0xC;
        streams[0].geometryFlags = 2;
        n = 1;
        if (flagsByte & 0x10) {
            streams[0].stride += 0xC;
            streams[0].geometryFlags |= 4;
            elements[1] = { 0, 0xC, 2, 0, 3, 0 };
            n = 2;
        }
        if (flagsByte & 8) {
            const RwUInt16 sIdx = (usage & 8) ? 0 : lastStream;
            elements[n] = { sIdx, static_cast<RwUInt16>(streams[sIdx].stride), 4, 0, 0x0A, 0 };
            streams[sIdx].stride += 4;
            streams[sIdx].geometryFlags |= 8;
            ++n;
        }
        for (RwUInt32 t = 0; t < numTexCoordSets; ++t) {
            const RwUInt16 sIdx = (usage & (0x10u << t)) ? 0 : lastStream;
            elements[n] = { sIdx, static_cast<RwUInt16>(streams[sIdx].stride), 1, 0, 5, static_cast<RwUInt8>(t) };
            streams[sIdx].stride += 8;
            streams[sIdx].geometryFlags |= static_cast<RwUInt16>(0x10u << t);
            ++n;
        }
    }
    elements[n] = { 0xFF, 0, 0x11, 0, 0, 0 };
    ((RwBool(__cdecl *)(const void*, void*))0x7FAA30)(elements, &header->vertexDeclaration);

    // 5. buffers de vertices.
    RwUInt32 usedStreams = numStreams;
    if (usedStreams > 1) {
        for (;;) {
            if (streams[usedStreams - 1].stride != 0) {
                break;
            }
            --usedStreams;
            if (!(usedStreams > 1)) {
                break;
            }
        }
    }
    auto* const instances = reinterpret_cast<RxD3D9InstanceData*>(header + 1);
    auto createManaged = [&](RxD3D9VertexStream& stream) -> bool {
        const RwUInt32 size = header->totalNumVertex * stream.stride;
        return ((RwBool(__cdecl *)(RwUInt32, RwUInt32, void*, RwUInt32*))0x7F5500)(stream.stride, size, &stream.vertexBuffer, &stream.offset) != 0;
    };
    bool singleStreamPath = false;
    if (usedStreams == 1 && streams[0].managed != 0) {
        if (!createManaged(streams[0])) {
            return FALSE;
        }
        singleStreamPath = true;
    } else {
        for (RwUInt32 i = 0; i < usedStreams; ++i) {
            RxD3D9VertexStream& stream = streams[i];
            const RwUInt32 size = header->totalNumVertex * stream.stride;
            if (stream.managed != 0 && (caps->DevCaps2 & kDevCaps2StreamOffset) != 0) {
                if (!createManaged(stream)) {
                    return FALSE;
                }
                continue;
            }
            bool createDynamic;
            if (skin->field20 != 0) {
                createDynamic = true;
            } else if ((caps->DevCaps2 & kDevCaps2StreamOffset) != 0) {
                createDynamic = false;
            } else {
                createDynamic = usedStreams != 1;
            }
            if (createDynamic) {
                if (!((RwBool(__cdecl *)(RwUInt32, void*))0x7F5A00)(size, &stream.vertexBuffer)) {
                    return FALSE;
                }
            } else {
                stream.dynamicLock = 1;
            }
            stream.managed = 0;
            stream.offset  = 0;
        }
        if (usedStreams > 1) {
            if ((caps->DevCaps2 & kDevCaps2StreamOffset) != 0) {
                header->useOffsets = 1;
            }
            RwUInt32 count = header->numMeshes;
            RxD3D9InstanceData* instance = instances;
            do {
                instance->baseIndex = instance->minVert;
                ++instance;
            } while (--count != 0);
            return TRUE;
        }
        singleStreamPath = true;
    }
    if (singleStreamPath) {
        RwUInt32 count = header->numMeshes;
        header->useOffsets = 0;
        RxD3D9InstanceData* instance = instances;
        do {
            instance->baseIndex = streams[0].offset / streams[0].stride + instance->minVert;
            ++instance;
        } while (--count != 0);
    }
    return TRUE;
}

RwUInt8* _rpSkinGetMeshBoneRemapIndices(RpSkin* skin) {
    return ((RwUInt8*(__cdecl *)(RpSkin*))0x7C8A40)(skin);
}

RwUInt8* _rpSkinGetMeshBoneRLECount(RpSkin* skin) {
    return ((RwUInt8*(__cdecl *)(RpSkin*))0x7C8A50)(skin);
}

RwUInt8* _rpSkinGetMeshBoneRLE(RpSkin* skin) {
    return ((RwUInt8*(__cdecl *)(RpSkin*))0x7C8A60)(skin);
}

// 0x7C8A70: libera los datos divididos anteriores (skin+0x30, RwFree del motor) y pone a 0 los campos +0x24..+0x38; reserva numMatrices + 2*(numMeshes+numRLE) bytes
// (pista 0x30116; si falla deja +0x30 en NULL y devuelve NULL), los pone a cero y fija boneLimit (+0x24), numMeshes (+0x28), numRLE (+0x2C), indices de remapeo (+0x30),
// cuentas RLE (+0x34 = base + numMatrices) y RLE (+0x38 = +0x34 + 2*numMeshes). Devuelve el skin.
RpSkin* _rpSkinSplitDataCreate(RpSkin* skin, RwUInt32 boneLimit, RwUInt32 numMatrices, RwUInt32 numMeshes, RwUInt32 numRLE) {
    if (skin->meshBoneRemapIndices != nullptr) {
        RwFree(skin->meshBoneRemapIndices);
    }
    skin->boneLimit            = 0;
    skin->numMeshes            = 0;
    skin->numRLE               = 0;
    skin->meshBoneRemapIndices = nullptr;
    skin->meshBoneRLECount     = nullptr;
    skin->meshBoneRLE          = nullptr;
    const RwUInt32 size = numMatrices + (numMeshes + numRLE) * 2;
    RwUInt8* const block = static_cast<RwUInt8*>(RwMalloc(size, kSkinMemHint));
    skin->meshBoneRemapIndices = block;
    if (block == nullptr) {
        return nullptr;
    }
    memset(block, 0, size);
    skin->boneLimit        = boneLimit;
    skin->numRLE           = numRLE;
    skin->numMeshes        = numMeshes;
    skin->meshBoneRLECount = block + numMatrices;
    skin->meshBoneRLE      = block + numMatrices + numMeshes * 2;
    return skin;
}

// 0x7C8B10: libera los datos divididos (skin+0x30, RwFree del motor) si existen, pone a 0 los campos +0x24..+0x38 y devuelve TRUE.
RwBool _rpSkinSplitDataDestroy(RpSkin* skin) {
    if (skin->meshBoneRemapIndices != nullptr) {
        RwFree(skin->meshBoneRemapIndices);
    }
    skin->boneLimit            = 0;
    skin->numMeshes            = 0;
    skin->numRLE               = 0;
    skin->meshBoneRemapIndices = nullptr;
    skin->meshBoneRLECount     = nullptr;
    skin->meshBoneRLE          = nullptr;
    return TRUE;
}

// 0x7C8D40: tamano del bloque de datos divididos en el flujo: 12 si no hay mallas; si las hay, 12 + numBones + 2*(numMeshes + numRLE).
RwUInt32 _rpSkinSplitDataGetStreamSize(const RpSkin* skin) {
    RwUInt32 size = 0xC;
    if (skin->numMeshes != 0) {
        size = skin->numBones + (skin->numRLE + skin->numMeshes) * 2 + 0xC;
    }
    return size;
}

// 0x7CB6A0 (lote R6A, sin validar; el listado decia RpUVAnim con confianza baja: sigue a RpSkin 0x7CB2B0 y es el mezclador generico de huesos por CPU, sin llamadores en el EXE; el nombre es
// el del SDK de RW). cdecl, 9 argumentos de pila, `ret`, 392 bytes: (numVertices, pesos 4 floats por vertice (16 bytes), indices 4 bytes por vertice (un byte por hueso), matrices de hueso (64 bytes),
// posiciones de salida (base, `outStride` bytes por vertice), posiciones de entrada (12 bytes por vertice), normales de salida (0 = no se calculan; mismo paso `outStride`), normales de entrada (12 bytes)).
// Recorre los vertices del ULTIMO al primero. Por vertice, hasta cuatro pesos: se corta en el primer peso cuya palabra como ENTERO con signo es <= 0 (no es una comparacion de float: -0.0 y los
// negativos cortan, un NaN positivo no); el hueso j es (indices >> (8 * j)) & 0xFF. El primer peso guarda el resultado y los demas lo suman. Las dos subrutinas del EXE 0x7CB828 (posicion, con la
// traslacion de la matriz) y 0x7CB8B0 (normal, sin traslacion) usan registros propios (edx/ebx/edi/eax/esi): son fragmentos locales y se reconstruyen aqui con SkinBlendPointCpp (X87Math.cpp).
void _rpSkinGenericMatrixBlend(RwInt32 numVertices, const RwMatrixWeights* matrixWeights, const RwUInt32* matrixIndices, const RwMatrix* matrixArray, RwUInt8* outVertices, const RwUInt8* inVertices,
                               RwUInt8* outNormals, const RwUInt8* inNormals, RwInt32 outStride) {
    RwInt32 index = numVertices;
    RwInt32 outOffset = outStride * numVertices - outStride;
    const RwInt32* weights = reinterpret_cast<const RwInt32*>(reinterpret_cast<const RwUInt8*>(matrixWeights) + 0x10 * numVertices - 0x10);
    const RwUInt32* indices = matrixIndices + numVertices - 1;
    const RwUInt8* inVertex = inVertices + 0x0C * numVertices - 0x0C;
    do {
        const RwInt32* weight = weights;
        for (RwUInt32 j = 0; j < 4; ++j, ++weight) {
            if (*weight <= 0) {
                break;
            }
            const RwMatrix* const bone = reinterpret_cast<const RwMatrix*>(reinterpret_cast<const RwUInt8*>(matrixArray) + (((*indices >> (8 * j)) & 0xFFu) << 6));
            const float w = *reinterpret_cast<const float*>(weight);
            SkinBlendPointCpp(reinterpret_cast<const float*>(bone), reinterpret_cast<const float*>(inVertex), w, reinterpret_cast<float*>(outVertices + outOffset), j == 0, true);
            if (outNormals != nullptr) {
                const RwUInt8* const inNormal = inVertex - inVertices + inNormals;
                SkinBlendPointCpp(reinterpret_cast<const float*>(bone), reinterpret_cast<const float*>(inNormal), w, reinterpret_cast<float*>(outNormals + outOffset), j == 0, false);
            }
        }
        weights -= 4;
        indices -= 1;
        inVertex -= 0x0C;
        outOffset -= outStride;
    } while (--index != 0);
}

RxNodeDefinition* RxNodeDefinitionGetD3D9SkinAtomicAllInOne() {
    return ((RxNodeDefinition*(__cdecl *)(void))0x7CB2A0)();
}

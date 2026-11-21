/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RpSkin
 * Reconstrucción: COMPLETA — 59/59 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/59 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <emmintrin.h>
#include <xmmintrin.h>

#include "rpskin.h"
#include "X87Math.h"
#include "Crt/CrtQsort.h"

// Callbacks del plugin 0x116 (lote S12A; cdecl). Se registran por su direccion propia en RpSkinPluginAttach y llevan hook en la direccion del EXE.
static void*     RpSkinEngineConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                                // 0x7C68E0
static void*     RpSkinEngineDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                                 // 0x7C69A0
static void*     RpSkinGeometryConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                              // 0x7C69F0
static void*     RpSkinGeometryDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                               // 0x7C6A10
static void*     RpSkinGeometryCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);           // 0x7C6A70
static void*     RpSkinAtomicConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                                // 0x7C6A80
static void*     RpSkinAtomicDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                                 // 0x7C6AA0
static void*     RpSkinAtomicCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);             // 0x7C6AC0
static RwBool    RpSkinAtomicStreamAlwaysCallBack(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                       // 0x7C6AE0
static RwInt32   RpSkinGeometryStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                      // 0x7C6BE0
static RwStream* RpSkinGeometryStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject); // 0x7C6C30
static RwStream* RpSkinGeometryStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);       // 0x7C6D20
static RwStream* RpSkinAtomicStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);         // 0x7C72A0
static RwStream* RpSkinAtomicStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);   // 0x7C7500
static RwInt32   RpSkinAtomicStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                        // 0x7C7510
static int __cdecl RpSkinUsedBoneCompare(const void* a, const void* b);                                                              // 0x7C8720

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
    error.errorCode = _rwerror(code);
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
    RH_ScopedGlobalInstall(_rpSkinSplitDataStreamWrite, 0x7C8B50); // lote R7A (sin validar)
    RH_ScopedGlobalInstall(_rpSkinSplitDataStreamRead, 0x7C8BE0); // lote R7A (sin validar)
    RH_ScopedGlobalInstall(_rpSkinD3D9InstanceVertexData, 0x7C90D0); // lote R7A (sin validar)
    RH_ScopedGlobalInstall(_rpSkinSSETransformPoints, 0x7CAAD0); // lote R7A (sin validar)
    RH_ScopedGlobalInstall(_rpSkinSSETransformPointsIndexed, 0x7CAB80); // lote R7A (sin validar)
    RH_ScopedGlobalInstall(_rpSkinD3D9CreateGenericPipeline, 0x7CB2B0); // lote R7A (sin validar)
    // Lote S12A (sin validar)
    RH_ScopedGlobalInstall(RpSkinSetFreeListCreateParams, 0x7C67F0);
    RH_ScopedGlobalInstall(RpSkinEngineConstructor, 0x7C68E0);
    RH_ScopedGlobalInstall(RpSkinEngineDestructor, 0x7C69A0);
    RH_ScopedGlobalInstall(RpSkinGeometryConstructor, 0x7C69F0);
    RH_ScopedGlobalInstall(RpSkinGeometryDestructor, 0x7C6A10);
    RH_ScopedGlobalInstall(RpSkinGeometryCopy, 0x7C6A70);
    RH_ScopedGlobalInstall(RpSkinAtomicConstructor, 0x7C6A80);
    RH_ScopedGlobalInstall(RpSkinAtomicDestructor, 0x7C6AA0);
    RH_ScopedGlobalInstall(RpSkinAtomicCopy, 0x7C6AC0);
    RH_ScopedGlobalInstall(RpSkinAtomicStreamAlwaysCallBack, 0x7C6AE0);
    RH_ScopedGlobalInstall(RpSkinGeometryStreamGetSize, 0x7C6BE0);
    RH_ScopedGlobalInstall(RpSkinGeometryStreamWrite, 0x7C6C30);
    RH_ScopedGlobalInstall(RpSkinGeometryStreamRead, 0x7C6D20);
    RH_ScopedGlobalInstall(RpSkinAtomicStreamRead, 0x7C72A0);
    RH_ScopedGlobalInstall(RpSkinAtomicStreamWrite, 0x7C7500);
    RH_ScopedGlobalInstall(RpSkinAtomicStreamGetSize, 0x7C7510);
    RH_ScopedGlobalInstall(RpSkinDestroy, 0x7C77A0);
    RH_ScopedGlobalInstall(RpSkinIsSplit, 0x7C7820);
    RH_ScopedGlobalInstall(RpSkinAtomicSetType, 0x7C7830);
    RH_ScopedGlobalInstall(RpSkinAtomicGetType, 0x7C7880);
    RH_ScopedGlobalInstall(RpSkinUsedBoneCompare, 0x7C8720);
    RH_ScopedGlobalInstall(_rpSkinGetMeshBoneRemapIndices, 0x7C8A40);
    RH_ScopedGlobalInstall(_rpSkinGetMeshBoneRLECount, 0x7C8A50);
    RH_ScopedGlobalInstall(_rpSkinGetMeshBoneRLE, 0x7C8A60);
    RH_ScopedGlobalInstall(RxNodeDefinitionGetD3D9SkinAtomicAllInOne, 0x7CB2A0);
}

// 0x7C67F0 (lote S12A, sin validar; 20 bytes): guarda el tamano de bloque de la lista libre de skins en 0x8DECE0 y el numero de bloques a reservar en 0x8DECE4.
void RpSkinSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    StaticRef<RwInt32>(0x8DECE0) = blockSize;
    StaticRef<RwInt32>(0x8DECE4) = numBlocksToPrealloc;
}

// Calcula maxNumWeightsPerVertex como 0x7C72A0 y 0x7C6D20 (rama de formato antiguo): parte de 1 y, por cada vertice, mientras el peso (comparado como dword != 0) en la posicion `max` no sea cero sube `max`
// y mira la siguiente (hasta 4; al llegar a 4 termina).
static void SkinComputeMaxWeights(RpSkin* skin, RwUInt32 numVertices) {
    const RwUInt32* const weights = reinterpret_cast<const RwUInt32*>(skin->vertexBoneWeights);
    skin->maxNumWeightsPerVertex = 1;
    for (RwUInt32 v = 0; v < numVertices; ++v) {
        RwUInt32 k = skin->maxNumWeightsPerVertex;
        if (k >= 4) {
            continue;
        }
        const RwUInt32* weight = weights + k + v * 4;
        while (*weight != 0) {
            ++skin->maxNumWeightsPerVertex;
            if (skin->maxNumWeightsPerVertex == 4) {
                return;
            }
            ++k;
            ++weight;
            if (k >= 4) {
                break;
            }
        }
    }
}

// Cambia el skin de la geometria (con _rpSkinDeinitialize del anterior si lo habia y _rpSkinInitialize del nuevo) salvo que ya sea ese (cola comun de 0x7C72A0 y 0x7C6D20).
static void SkinInstallOnGeometry(RpGeometry* geometry, RpSkin* skin) {
    RpSkin* const previous = GeometrySkin(geometry);
    if (skin == previous) {
        return;
    }
    if (previous != nullptr) {
        _rpSkinDeinitialize(geometry);
    }
    GeometrySkin(geometry) = skin;
    _rpSkinInitialize(geometry);
}

// 0x7C68E0 (lote S12A, sin validar; cdecl, 3 argumentos, 185 bytes): constructor del plugin de motor 0x116. Solo la primera vez (contador 0xC978C0 a 0): _rpSkinPipelinesCreate(tipos) con tipos = 1, 3 si existe el plugin
// de motor 0x120 y |4 si existe el 0x12E; lista libre de skins (RwFreeListCreateAndPreallocateSpace(0x40, [0x8DECE0], 4, [0x8DECE4], espacio propio 0xC97078, pista 0x40116)) en 0xC978B8; bloque de 0x400F bytes a cero
// (RwMalloc, pista 0x40116) en 0xC978B0 y su direccion alineada a 16 (+15 y &~15) en 0xC978AC. Siempre sube el contador. Devuelve el objeto.
// Si el RwMalloc fallase el EXE escribiria en NULL; aqui se omite la puesta a cero.
static void* RpSkinEngineConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    RwInt32& initCount = StaticRef<RwInt32>(0xC978C0);
    if (initCount == 0) {
        RwUInt32 types = 1;
        if (RwEngineGetPluginOffset(0x120) != -1) {
            types = 3;
        }
        if (RwEngineGetPluginOffset(0x12E) != -1) {
            types |= 4;
        }
        _rpSkinPipelinesCreate(types);
        SkinFreeList() = RwFreeListCreateAndPreallocateSpace(0x40, StaticRef<RwInt32>(0x8DECE0), 4, StaticRef<RwInt32>(0x8DECE4), reinterpret_cast<RwFreeList*>(0xC97078), 0x40116);
        void* const block = RwMalloc(0x400F, 0x40116);
        StaticRef<void*>(0xC978B0) = block;
        if (block != nullptr) {
            memset(block, 0, 0x400F);
        }
        StaticRef<uintptr_t>(0xC978AC) = (reinterpret_cast<uintptr_t>(StaticRef<void*>(0xC978B0)) + 0xF) & ~static_cast<uintptr_t>(0xF);
    }
    initCount = initCount + 1;
    return object;
}

// 0x7C69A0 (lote S12A, sin validar; cdecl, 3 argumentos, 76 bytes): destructor del plugin de motor: baja el contador 0xC978C0 y, si llega a 0, _rpSkinPipelinesDestroy, RwFreeListDestroy de la lista de skins (0xC978B8, que
// queda a NULL) y RwFree del bloque de 0xC978B0 (que queda a NULL). Devuelve el objeto.
static void* RpSkinEngineDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    RwInt32& initCount = StaticRef<RwInt32>(0xC978C0);
    initCount = initCount - 1;
    if (initCount == 0) {
        _rpSkinPipelinesDestroy();
        RwFreeListDestroy(SkinFreeList());
        void* const block = StaticRef<void*>(0xC978B0);
        SkinFreeList() = nullptr;
        RwFree(block);
        StaticRef<void*>(0xC978B0) = nullptr;
    }
    return object;
}

// 0x7C69F0 (lote S12A, sin validar; 18 bytes): constructor del plugin de geometria: skin = NULL en geometria + [0xC978A8]. Devuelve el objeto.
static void* RpSkinGeometryConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    GeometrySkin(static_cast<RpGeometry*>(object)) = nullptr;
    return object;
}

// 0x7C6A10 (lote S12A, sin validar; cdecl, 3 argumentos, 96 bytes): destructor del plugin de geometria. Si tiene skin: _rpSkinDeinitialize(geometria) (0x7C8820), RwFree del bloque de datos (skin+0x3C) si existe,
// _rpSkinSplitDataDestroy (0x7C8B10), devuelve el skin a la lista libre y deja el campo a NULL. Devuelve el objeto.
static void* RpSkinGeometryDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    RpGeometry* const geometry = static_cast<RpGeometry*>(object);
    if (RpSkin* const skin = GeometrySkin(geometry)) {
        _rpSkinDeinitialize(geometry);
        if (skin->allocation != nullptr) {
            RwFree(skin->allocation);
        }
        _rpSkinSplitDataDestroy(skin);
        RwFreeListFree(SkinFreeList(), skin);
        GeometrySkin(geometry) = nullptr;
    }
    return object;
}

// 0x7C6A70 (lote S12A, sin validar; 5 bytes): copia del plugin de geometria: no copia nada y devuelve el objeto destino.
static void* RpSkinGeometryCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(srcObject); UNUSED(offsetInObject); UNUSED(sizeInObject);
    return dstObject;
}

// 0x7C6A80 (lote S12A, sin validar; 19 bytes): constructor del plugin de atomic: dato (4 bytes) = 0 en atomic + [0xC978A4]. Devuelve el objeto.
static void* RpSkinAtomicConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    *reinterpret_cast<RwUInt32*>(reinterpret_cast<uintptr_t>(object) + static_cast<uint32>(SkinAtomicOffset())) = 0;
    return object;
}

// 0x7C6AA0 (lote S12A, sin validar; 24 bytes): destructor del plugin de atomic: pone a 0 el dato (si no lo era). Devuelve el objeto.
static void* RpSkinAtomicDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    *reinterpret_cast<RwUInt32*>(reinterpret_cast<uintptr_t>(object) + static_cast<uint32>(SkinAtomicOffset())) = 0;
    return object;
}

// 0x7C6AC0 (lote S12A, sin validar; 21 bytes): copia del plugin de atomic: el dato (dword) del destino = el del origen. Devuelve el destino.
static void* RpSkinAtomicCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    const RwUInt32 value = *reinterpret_cast<const RwUInt32*>(reinterpret_cast<uintptr_t>(srcObject) + static_cast<uint32>(SkinAtomicOffset()));
    *reinterpret_cast<RwUInt32*>(reinterpret_cast<uintptr_t>(dstObject) + static_cast<uint32>(SkinAtomicOffset())) = value;
    return dstObject;
}

// 0x7C6AE0 (lote S12A, sin validar; cdecl, 3 argumentos, 142 bytes): callback "siempre" del flujo del atomic. Tipo = 1, o 2 si el plugin de motor 0x120 esta registrado y el byte de su dato en el atomic
// (RpAtomicGetPluginOffset(0x120)) no es 0. Si el atomic tiene geometria con skin: tipo 2 sin plugin 0x120 -> 1 y, si no, tipo 3 sin plugin 0x12E -> 1; _rpSkinPipelineAtomicAttach(atomic, tipo) (0x7C89B0). Devuelve TRUE.
static RwBool RpSkinAtomicStreamAlwaysCallBack(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    RpAtomic* const atomic = static_cast<RpAtomic*>(object);
    RwUInt32 type = 1;
    if (RwEngineGetPluginOffset(0x120) != -1) {
        if (*(reinterpret_cast<const RwUInt8*>(atomic) + static_cast<uint32>(RpAtomicGetPluginOffset(0x120))) != 0) {
            type = 2;
        }
    }
    RpGeometry* const geometry = atomic->geometry;
    if (geometry != nullptr && GeometrySkin(geometry) != nullptr) {
        if (RwEngineGetPluginOffset(0x120) == -1 && type == 2) {
            type = 1;
        } else if (RwEngineGetPluginOffset(0x12E) == -1 && type == 3) {
            type = 1;
        }
        _rpSkinPipelineAtomicAttach(atomic, type);
    }
    return TRUE;
}

// 0x7C6BE0 (lote S12A, sin validar; cdecl, 3 argumentos, 75 bytes): tamano en flujo del skin de la geometria. Sin skin: 0. Geometria nativa (flags & 0x1000000): _rpSkinNativeGeometryGetSize (0x7C8830). Si no:
// numBones * 64 + numVertices * 20 + numUsedBones + 4 + _rpSkinSplitDataGetStreamSize(skin) (0x7C8D40).
static RwInt32 RpSkinGeometryStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    const RpGeometry* const geometry = static_cast<const RpGeometry*>(object);
    const RpSkin* const skin = GeometrySkin(geometry);
    if (skin == nullptr) {
        return 0;
    }
    if (geometry->flags & rpGEOMETRYNATIVE) {
        return static_cast<RwInt32>(_rpSkinNativeGeometryGetSize(geometry));
    }
    const RwUInt32 base = (skin->numBones << 6) + static_cast<RwUInt32>(geometry->numVertices) * 20u + skin->numUsedBones + 4u;
    return static_cast<RwInt32>(base + _rpSkinSplitDataGetStreamSize(skin));
}

// 0x7C6C30 (lote S12A, sin validar; cdecl, 5 argumentos, 240 bytes): escritura del skin de la geometria. Sin skin: devuelve el flujo sin escribir. Nativa: _rpSkinNativeGeometryWrite(flujo, geometria) (0 -> NULL).
// Si no: cabecera de 4 bytes con numBones (8 bits bajos) | numUsedBones (8 bits bajos) << 8 | maxNumWeightsPerVertex (8 bits bajos) << 16 (RwStreamWriteInt32), huesos usados (RwStreamWrite, numUsedBones bytes),
// indices (numVertices * 4, RwStreamWriteInt32), pesos (numVertices * 16, RwStreamWriteReal), matrices (numBones * 64, RwStreamWriteReal) y _rpSkinSplitDataStreamWrite(flujo, skin); cualquier fallo -> NULL.
static RwStream* RpSkinGeometryStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(binaryLength); UNUSED(offsetInObject); UNUSED(sizeInObject);
    const RpGeometry* const geometry = static_cast<const RpGeometry*>(object);
    const RpSkin* const skin = GeometrySkin(geometry);
    if (skin == nullptr) {
        return stream;
    }
    if (geometry->flags & rpGEOMETRYNATIVE) {
        return _rpSkinNativeGeometryWrite(stream, geometry) != 0 ? stream : nullptr;
    }
    const RwUInt32 numVertices = static_cast<RwUInt32>(geometry->numVertices);
    const RwUInt32 header = (skin->numBones & 0xFFu) | ((skin->numUsedBones & 0xFFu) << 8) | ((skin->maxNumWeightsPerVertex & 0xFFu) << 16);
    if (RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&header), 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamWrite(stream, skin->usedBonesArray, skin->numUsedBones) == nullptr) {
        return nullptr;
    }
    if (RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(skin->vertexBoneIndices), numVertices * 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamWriteReal(stream, reinterpret_cast<const RwReal*>(skin->vertexBoneWeights), numVertices << 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamWriteReal(stream, reinterpret_cast<const RwReal*>(skin->skinToBoneMatrices), skin->numBones << 6) == nullptr) {
        return nullptr;
    }
    return _rpSkinSplitDataStreamWrite(stream, skin) != 0 ? stream : nullptr;
}

// 0x7C6D20 (lote S12A, sin validar; cdecl, 5 argumentos, 926 bytes): lectura del skin de la geometria. Nativa: _rpSkinNativeGeometryRead(flujo, geometria) (0 -> NULL). Si no: cabecera (RwStreamReadInt32; fallo -> NULL) con
// numBones, numUsedBones y maxNumWeightsPerVertex (8 bits cada uno). Bloque de la lista libre 0xC978B8 (pista 0x30116) a cero (0x40 bytes) y _rpSkinCreateData(skin, numBones, numUsedBones, numVertices, buffer de 256 bytes, NULL, NULL, NULL)
// (fallo: devuelve el bloque a la lista y NULL). Formato antiguo (max 0): numUsedBones = numBones (si es 0, _rpSkinFindUsedBones sin datos), sin lectura de huesos usados; nuevo: RwStreamRead de numUsedBones bytes en usedBonesArray
// (cantidad distinta -> NULL). Despues indices (V*4) y pesos (V*16) (fallo -> NULL). Nuevo: maxNumWeightsPerVertex = max, matrices (numBones*64) y _rpSkinSplitDataStreamRead (fallo -> NULL). Antiguo: por matriz RwStreamSkip(4) +
// RwStreamReadReal(0x40) (fallo -> NULL), maxNumWeightsPerVertex y huesos usados recalculados. Si el skin no era ya el de la geometria: _rpSkinDeinitialize del anterior (si habia), se guarda y _rpSkinInitialize. Devuelve el flujo.
// El buffer de huesos usados del EXE es de pila sin inicializar (se copia y se vuelve a calcular); aqui va a cero. Con la lista libre agotada el EXE escribiria en NULL; aqui devuelve NULL.
static RwStream* RpSkinGeometryStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(binaryLength); UNUSED(offsetInObject); UNUSED(sizeInObject);
    RpGeometry* const geometry = static_cast<RpGeometry*>(object);
    if (geometry->flags & rpGEOMETRYNATIVE) {
        return _rpSkinNativeGeometryRead(stream, geometry) != 0 ? stream : nullptr;
    }
    RwUInt32 header;
    if (RwStreamReadInt32(stream, reinterpret_cast<RwInt32*>(&header), 4) == nullptr) {
        return nullptr;
    }
    const RwUInt32 numBones = header & 0xFFu;
    const RwUInt32 numUsedBones = (header >> 8) & 0xFFu;
    const RwUInt32 maxWeights = (header >> 16) & 0xFFu;
    const RwUInt32 numVertices = static_cast<RwUInt32>(geometry->numVertices);
    RpSkin* const skin = static_cast<RpSkin*>(RwFreeListAlloc(SkinFreeList(), kSkinMemHint));
    if (skin == nullptr) {
        return nullptr;
    }
    memset(skin, 0, 0x40);
    RwUInt8 bonesBuffer[0x100] = {};
    if (maxWeights == 0) {
        RwUInt32 usedBones = numBones;
        if (usedBones == 0) {
            _rpSkinFindUsedBones(skin, nullptr, nullptr, bonesBuffer, &usedBones, numVertices);
        }
        if (!_rpSkinCreateData(skin, numBones, usedBones, numVertices, bonesBuffer, nullptr, nullptr, nullptr)) {
            RwFreeListFree(SkinFreeList(), skin);
            return nullptr;
        }
    } else {
        if (!_rpSkinCreateData(skin, numBones, numUsedBones, numVertices, bonesBuffer, nullptr, nullptr, nullptr)) {
            RwFreeListFree(SkinFreeList(), skin);
            return nullptr;
        }
        if (RwStreamRead(stream, skin->usedBonesArray, numUsedBones) != numUsedBones) {
            return nullptr;
        }
    }
    if (RwStreamReadInt32(stream, reinterpret_cast<RwInt32*>(skin->vertexBoneIndices), numVertices * 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamReadReal(stream, reinterpret_cast<RwReal*>(skin->vertexBoneWeights), numVertices << 4) == nullptr) {
        return nullptr;
    }
    if (maxWeights != 0) {
        skin->maxNumWeightsPerVertex = maxWeights;
        if (RwStreamReadReal(stream, reinterpret_cast<RwReal*>(skin->skinToBoneMatrices), skin->numBones << 6) == nullptr) {
            return nullptr;
        }
        if (_rpSkinSplitDataStreamRead(stream, skin) == nullptr) {
            return nullptr;
        }
    } else {
        RwUInt32 offset = 0;
        for (RwUInt32 i = 0; i < skin->numBones; ++i, offset += 0x40) {
            if (RwStreamSkip(stream, 4) == nullptr) {
                return nullptr;
            }
            if (RwStreamReadReal(stream, reinterpret_cast<RwReal*>(reinterpret_cast<RwUInt8*>(skin->skinToBoneMatrices) + offset), 0x40) == nullptr) {
                return nullptr;
            }
        }
        SkinComputeMaxWeights(skin, numVertices);
        _rpSkinFindUsedBones(skin, skin->vertexBoneIndices, skin->vertexBoneWeights, skin->usedBonesArray, &skin->numUsedBones, numVertices);
    }
    SkinInstallOnGeometry(geometry, skin);
    return stream;
}

// 0x7C72A0 (lote S12A, sin validar; cdecl, 5 argumentos, 593 bytes): lectura del skin en el flujo del atomic (formato antiguo). Si la geometria del atomic ya tiene skin: RwStreamSkip(flujo, sizeInObject) (el EXE usa el
// quinto argumento) y devuelve el flujo (fallo -> NULL). Si no: numBones (RwStreamReadInt32; fallo -> NULL), bloque de la lista libre (pista 0x30116) a cero, si numBones es 0 _rpSkinFindUsedBones sin datos, _rpSkinCreateData(skin,
// numBones, numUsedBones = numBones o el resultado, numVertices, buffer de 256 bytes, NULL, NULL, NULL) (fallo: devuelve el bloque a la lista; el EXE sigue con el skin a NULL y falla despues, aqui devuelve NULL),
// RwStreamSkip(4), indices (V*4), pesos (V*16 reales), por matriz RwStreamSkip(12) + RwStreamReadReal(0x40) (fallos -> NULL), maxNumWeightsPerVertex y huesos usados recalculados y el skin pasa a la geometria
// (_rpSkinDeinitialize del anterior, _rpSkinInitialize). Devuelve el flujo.
static RwStream* RpSkinAtomicStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(binaryLength); UNUSED(offsetInObject);
    RpAtomic* const atomic = static_cast<RpAtomic*>(object);
    RpGeometry* const geometry = atomic->geometry;
    if (GeometrySkin(geometry) != nullptr) {
        return RwStreamSkip(stream, static_cast<RwUInt32>(sizeInObject)) != nullptr ? stream : nullptr;
    }
    RwInt32 count;
    if (RwStreamReadInt32(stream, &count, 4) == nullptr) {
        return nullptr;
    }
    const RwUInt32 numVertices = static_cast<RwUInt32>(geometry->numVertices);
    const RwUInt32 numBones = static_cast<RwUInt32>(count);
    RpSkin* const skin = static_cast<RpSkin*>(RwFreeListAlloc(SkinFreeList(), kSkinMemHint));
    if (skin == nullptr) {
        return nullptr;
    }
    memset(skin, 0, 0x40);
    RwUInt8 bonesBuffer[0x100] = {};
    RwUInt32 usedBones = numBones;
    if (usedBones == 0) {
        _rpSkinFindUsedBones(skin, nullptr, nullptr, bonesBuffer, &usedBones, numVertices);
    }
    if (!_rpSkinCreateData(skin, numBones, usedBones, numVertices, bonesBuffer, nullptr, nullptr, nullptr)) {
        RwFreeListFree(SkinFreeList(), skin);
        return nullptr;
    }
    if (RwStreamSkip(stream, 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamReadInt32(stream, reinterpret_cast<RwInt32*>(skin->vertexBoneIndices), numVertices * 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamReadReal(stream, reinterpret_cast<RwReal*>(skin->vertexBoneWeights), numVertices << 4) == nullptr) {
        return nullptr;
    }
    RwUInt32 offset = 0;
    for (RwUInt32 i = 0; i < skin->numBones; ++i, offset += 0x40) {
        if (RwStreamSkip(stream, 0xC) == nullptr) {
            return nullptr;
        }
        if (RwStreamReadReal(stream, reinterpret_cast<RwReal*>(reinterpret_cast<RwUInt8*>(skin->skinToBoneMatrices) + offset), 0x40) == nullptr) {
            return nullptr;
        }
    }
    SkinComputeMaxWeights(skin, numVertices);
    _rpSkinFindUsedBones(skin, skin->vertexBoneIndices, skin->vertexBoneWeights, skin->usedBonesArray, &skin->numUsedBones, numVertices);
    SkinInstallOnGeometry(geometry, skin);
    return stream;
}

// 0x7C7500 (lote S12A, sin validar; 5 bytes): escritura del flujo del atomic: no escribe nada y devuelve el flujo.
static RwStream* RpSkinAtomicStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(binaryLength); UNUSED(object); UNUSED(offsetInObject); UNUSED(sizeInObject);
    return stream;
}

// 0x7C7510 (lote S12A, sin validar; 3 bytes): tamano del flujo del atomic: 0.
static RwInt32 RpSkinAtomicStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(object); UNUSED(offsetInObject); UNUSED(sizeInObject);
    return 0;
}

// 0x7C8720 (lote S12A, sin validar; cdecl, 2 argumentos, 19 bytes): comparador de qsort de los huesos usados: diferencia de los bytes (sin signo) apuntados.
static int __cdecl RpSkinUsedBoneCompare(const void* a, const void* b) {
    return static_cast<int>(*static_cast<const RwUInt8*>(a)) - static_cast<int>(*static_cast<const RwUInt8*>(b));
}

// 0x7C6820: registra el plugin 0x116 en el motor, en los atomics y en las geometrias. Lote S12A: constructor/destructor/copia, rutinas de flujo y callback "siempre" son funciones propias
// (con hook en su direccion del EXE: 0x7C68E0, 0x7C69A0, 0x7C6A80, 0x7C6AA0, 0x7C6AC0, 0x7C72A0, 0x7C7500, 0x7C7510, 0x7C6AE0, 0x7C69F0, 0x7C6A10, 0x7C6A70, 0x7C6D20, 0x7C6C30, 0x7C6BE0). Solo se guardan los desplazamientos del motor (0xC978A0), del atomic (0xC978A4) y de la geometria (0xC978A8); el resto de resultados
// se ignora. Devuelve siempre TRUE.
RwBool RpSkinPluginAttach() {
    SkinEngineOffset() = RwEngineRegisterPlugin(0, kSkinPluginID, RpSkinEngineConstructor, RpSkinEngineDestructor);
    SkinAtomicOffset() = RpAtomicRegisterPlugin(4, kSkinPluginID, RpSkinAtomicConstructor, RpSkinAtomicDestructor, RpSkinAtomicCopy);
    RpAtomicRegisterPluginStream(kSkinPluginID, RpSkinAtomicStreamRead, RpSkinAtomicStreamWrite, RpSkinAtomicStreamGetSize);
    RpAtomicSetStreamAlwaysCallBack(kSkinPluginID, RpSkinAtomicStreamAlwaysCallBack);
    RpAtomicSetStreamRightsCallBack(kSkinPluginID, reinterpret_cast<RwPluginDataChunkRightsCallBack>(_rpSkinAtomicStreamRightsCallBack));
    SkinGeometryOffset() = RpGeometryRegisterPlugin(4, kSkinPluginID, RpSkinGeometryConstructor, RpSkinGeometryDestructor, RpSkinGeometryCopy);
    RpGeometryRegisterPluginStream(kSkinPluginID, RpSkinGeometryStreamRead, RpSkinGeometryStreamWrite, RpSkinGeometryStreamGetSize);
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

// 0x7C77A0 (lote S12A, sin validar; cdecl, 1 argumento, 60 bytes): libera el bloque de datos (skin+0x3C) si existe, _rpSkinSplitDataDestroy(skin) (0x7C8B10), devuelve el skin a la lista libre 0xC978B8 y devuelve NULL.
RpSkin* RpSkinDestroy(RpSkin* skin) {
    if (skin->allocation != nullptr) {
        RwFree(skin->allocation);
    }
    _rpSkinSplitDataDestroy(skin);
    RwFreeListFree(SkinFreeList(), skin);
    return nullptr;
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

// 0x7C7820 (lote S12A, sin validar; 14 bytes): TRUE si skin+0x28 (numero de mallas) no es cero (sin signo).
RwBool RpSkinIsSplit(RpSkin* skin) {
    return skin->numMeshes != 0 ? TRUE : FALSE;
}

// 0x7C7830 (lote S12A, sin validar; cdecl, 2 argumentos, 72 bytes): si el plugin de motor 0x120 no esta registrado y el tipo es 2, o (si no) el 0x12E no esta registrado y el tipo es 3, el tipo pasa a 1; despues
// _rpSkinPipelineAtomicAttach(atomic, tipo) (0x7C89B0) cuyo resultado se devuelve.
RpAtomic* RpSkinAtomicSetType(RpAtomic* atomic, RpSkinType type) {
    RwUInt32 skinType = static_cast<RwUInt32>(type);
    if (RwEngineGetPluginOffset(0x120) == -1 && skinType == 2) {
        skinType = 1;
    } else if (RwEngineGetPluginOffset(0x12E) == -1 && skinType == 3) {
        skinType = 1;
    }
    return _rpSkinPipelineAtomicAttach(atomic, skinType);
}

// 0x7C7880 (lote S12A, sin validar; 23 bytes): si el pipeline del atomic (atomic+0x6C) pertenece al plugin 0x116 (pluginId +0x2C) devuelve su dato (+0x30), el tipo; si no, 0.
RpSkinType RpSkinAtomicGetType(RpAtomic* atomic) {
    const RxPipeline* const pipeline = atomic->pipeline;
    if (pipeline->pluginId == kSkinPluginID) {
        return static_cast<RpSkinType>(pipeline->pluginData);
    }
    return static_cast<RpSkinType>(0);
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
        ExeCrt::Qsort(skin->usedBonesArray, skin->numUsedBones, 1, RpSkinUsedBoneCompare);
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
        RxPipeline* const pipeline = _rpSkinD3D9CreateGenericPipeline();
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
                RwD3D9DestroyVertexBuffer(stream.stride, header->totalNumVertex * stream.stride, stream.vertexBuffer, offset);
            } else if (offset == 0 && stream.dynamicLock == 0) {
                RwD3D9DynamicVertexBufferDestroy(stream.vertexBuffer);
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
        return RwD3D9CreateVertexBuffer(stream.stride, size, &stream.vertexBuffer, &stream.offset) != 0;
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
                if (!RwD3D9DynamicVertexBufferCreate(size, &stream.vertexBuffer)) {
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

// 0x7C8A40 (lote S12A, sin validar; 8 bytes): skin+0x30.
RwUInt8* _rpSkinGetMeshBoneRemapIndices(RpSkin* skin) {
    return skin->meshBoneRemapIndices;
}

// 0x7C8A50 (lote S12A, sin validar; 8 bytes): skin+0x34.
RwUInt8* _rpSkinGetMeshBoneRLECount(RpSkin* skin) {
    return skin->meshBoneRLECount;
}

// 0x7C8A60 (lote S12A, sin validar; 8 bytes): skin+0x38.
RwUInt8* _rpSkinGetMeshBoneRLE(RpSkin* skin) {
    return skin->meshBoneRLE;
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

// 0x7C8B50 (lote R7A, sin validar; el listado decia RpSkin con confianza alta y es correcto: la llama 0x7C6CF8, escritura del chunk de datos divididos del skin): (stream, skin). Escribe con RwStreamWriteInt32
// (4 bytes cada una) boneLimit (+0x24), numMeshes (+0x28) y numRLE (+0x2C); cada escritura se comprueba (NULL si devuelve NULL). Si numMeshes != 0 (sin signo) escribe con RwStreamWrite (0x7ECB30)
// los datos de remapeo (skin+0x30): numBones + 2*(numMeshes + numRLE) bytes; NULL si falla. Devuelve el flujo. El nombre es el del SDK de RW (inferido).
RwStream* _rpSkinSplitDataStreamWrite(RwStream* stream, const RpSkin* skin) {
    if (RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&skin->boneLimit), 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&skin->numMeshes), 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&skin->numRLE), 4) == nullptr) {
        return nullptr;
    }
    if (skin->numMeshes != 0) {
        if (RwStreamWrite(stream, skin->meshBoneRemapIndices, skin->numBones + (skin->numRLE + skin->numMeshes) * 2) == nullptr) {
            return nullptr;
        }
    }
    return stream;
}

// 0x7C8BE0 (lote R7A, sin validar; la llama 0x7C7071): (stream, skin). Lee tres enteros de 4 bytes con RwStreamReadInt32, en este orden: boneLimit, numMeshes y numRLE (NULL si falla alguna). Con numMeshes <= 0
// (CON signo) devuelve el flujo sin tocar el skin. Si no: toma numBones (skin+0), libera los datos divididos anteriores (skin+0x30, RwFree del motor) si existen, pone a 0 los seis campos +0x24..+0x38,
// reserva numBones + 2*(numMeshes + numRLE) bytes (pista 0x30116; si falla deja skin+0x30 en NULL y devuelve NULL), los pone a cero, rellena boneLimit, numMeshes, cuentas RLE (+0x34 = base + numBones),
// RLE (+0x38 = +0x34 + 2*numMeshes) y numRLE, y lee con RwStreamRead (0x7EC9D0) esos mismos bytes sobre el bloque. Si la lectura devuelve 0 el EXE llama a RwFree con `skin + 0x24` (un puntero INTERIOR
// del skin, no el bloque: fallo del original que se conserva) y devuelve NULL. Devuelve el flujo.
RwStream* _rpSkinSplitDataStreamRead(RwStream* stream, RpSkin* skin) {
    RwInt32 boneLimit;
    RwInt32 numMeshes;
    RwInt32 numRLE;
    if (RwStreamReadInt32(stream, &boneLimit, 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamReadInt32(stream, &numMeshes, 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamReadInt32(stream, &numRLE, 4) == nullptr) {
        return nullptr;
    }
    if (numMeshes <= 0) {
        return stream;
    }
    const RwUInt32 numBones = skin->numBones;
    if (skin->meshBoneRemapIndices != nullptr) {
        RwFree(skin->meshBoneRemapIndices);
    }
    skin->boneLimit            = 0;
    skin->numMeshes            = 0;
    skin->numRLE               = 0;
    skin->meshBoneRemapIndices = nullptr;
    skin->meshBoneRLECount     = nullptr;
    skin->meshBoneRLE          = nullptr;
    const RwUInt32 size = numBones + static_cast<RwUInt32>(numMeshes + numRLE) * 2;
    RwUInt8* const block = static_cast<RwUInt8*>(RwMalloc(size, kSkinMemHint));
    skin->meshBoneRemapIndices = block;
    if (block == nullptr) {
        return nullptr;
    }
    memset(block, 0, size);
    skin->boneLimit        = static_cast<RwUInt32>(boneLimit);
    skin->numMeshes        = static_cast<RwUInt32>(numMeshes);
    skin->meshBoneRLECount = block + numBones;
    skin->meshBoneRLE      = block + numBones + static_cast<RwUInt32>(numMeshes) * 2;
    skin->numRLE           = static_cast<RwUInt32>(numRLE);
    if (RwStreamRead(stream, skin->meshBoneRemapIndices, skin->numBones + static_cast<RwUInt32>(numMeshes + numRLE) * 2) == 0) {
        RwFree(reinterpret_cast<RwUInt8*>(skin) + 0x24);
        return nullptr;
    }
    return stream;
}

// 0x7C90D0 (lote R7A, sin validar; el listado decia RpSkin con confianza alta y es correcto: la llaman 0x7C7FF2 y 0x7C9031 del propio modulo, el segundo tras _rpSkinD3D9CreateVertexBuffer): rellena los
// buferes de vertices D3D9 de una geometria con skin (instanciado / reinstanciado). cdecl, 4 argumentos de pila (el EXE usa marco con `and esp, -8`); el valor de retorno de EAX es basura y ningun llamador
// lo lee. (atomic, cabecera RxD3D9ResEntryHeader (= RwResEntry + 0x18), jerarquia RpHAnimHierarchy (los llamadores la leen del plugin del atomic, atomic + [0xC978A4]; solo se usa su numNodes, +4), banderas de bloqueo: 0xFFF = todo).
//  1. RpD3D9GeometryGetUsageFlags (0x7588D0), skin de la geometria, numTexCoordSets; GetDeclaration de la declaracion del header (hasta 18 elementos en pila) y numero de flujos = max(Stream)+1
//     (0 si el primer elemento ya es D3DDECL_END).
//  2. Por flujo (el bucle se salta los flujos cuyas geometryFlags no estan en las banderas, salvo con 0xFFF): bufer no gestionado con offset 0 -> las banderas |= geometryFlags y Lock(0, 0, 0x2800);
//     si no Lock(offset, stride * totalNumVertex, 0x800). Punteros bloqueados por flujo (2 ranuras).
//  3. Posiciones (banderas & 2): _rpD3D9VertexDeclarationInstV3d con morphTarget->verts (+0x14), en el elemento (usage 0, indice 0).
//  4. Con skin, skin+0x20 != 0 y banderas == 0xFFF: pesos (maxNumWeightsPerVertex > 1; pone 1.0f,0,0,0 si el primero > 1.0f, comparacion ordenada x87) con _rpD3D9VertexDeclarationInstWeights, e
//     indices con _rpD3D9VertexDeclarationInstIndicesRemap y una tabla local de 255 bytes a cero: con skin+0x34 != 0 usa los remapeos skin+0x30 (el primero se pone a 0 si >= numNodes de la jerarquia, con signo;
//     entrada < 0xFF -> *3); si no, con numNodes <= limite [0xC978E8] i*3 por posicion, y si no usedBones[i] (del skin, skin->numUsedBones) -> i*3.
//  5. Normales (flags de la geometria & 0x10 y banderas & 4): InstV3d con morphTarget->normals (+0x18). Tangentes (usage & 0x10000 y banderas & 0xFF2): _rpD3D9VertexDeclarationInstTangent con verts,
//     texCoords[numTexCoordSets - 1] (el EXE lee geometria + 0x30 + 4 * numTexCoordSets) y el header. Coordenadas de textura (numTexCoordSets != 0, banderas & 0xFF0): por conjunto cuyo bit (0x10 << t)
//     esta puesto, InstV2d. Color (flags de la geometria & 8 y banderas & 8): por malla (do/while, con numMeshes == 0 no termina), _rpD3D9VertexDeclarationInstColor y guarda el resultado en vertexAlpha.
//  6. Unlock de los flujos bloqueados.
void _rpSkinD3D9InstanceVertexData(RpAtomic* atomic, RxD3D9ResEntryHeader* header, const RpHAnimHierarchy* hierarchy, RwUInt32 lockedFlags) {
    RpGeometry* const geometry = atomic->geometry;
    const RwUInt32 usage = static_cast<RwUInt32>(RpD3D9GeometryGetUsageFlags(geometry));
    const RpSkin* const skin = GeometrySkin(geometry);
    const RwUInt32 numTexCoordSets = static_cast<RwUInt32>(geometry->numTexCoordSets);

    D3DVERTEXELEMENT9 elements[18] = {};
    UINT numElements = 0;
    header->vertexDeclaration->GetDeclaration(elements, &numElements);

    RwUInt32 numStreams = 0;
    if (elements[0].Type != D3DDECLTYPE_UNUSED) {
        RwUInt32 maxStream = 0;
        const D3DVERTEXELEMENT9* element = elements;
        BYTE nextType;
        do {
            if (!(element->Stream < maxStream)) {
                maxStream = element->Stream + 1;
            }
            nextType = element[1].Type;
            ++element;
        } while (nextType != D3DDECLTYPE_UNUSED);
        numStreams = maxStream;
    }

    RwUInt8* locked[2] = { nullptr, nullptr };
    for (RwUInt32 i = 0; i < numStreams; ++i) {
        RxD3D9VertexStream& stream = header->vertexStream[i];
        if (lockedFlags != 0xFFF && (lockedFlags & stream.geometryFlags) == 0) {
            continue;
        }
        const RwUInt32 size = stream.stride * header->totalNumVertex;
        auto* const vertexBuffer = static_cast<IDirect3DVertexBuffer9*>(stream.vertexBuffer);
        if (stream.managed == 0 && stream.offset == 0) {
            lockedFlags |= stream.geometryFlags;
            vertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&locked[i]), 0x2800);
        } else {
            vertexBuffer->Lock(stream.offset, size, reinterpret_cast<void**>(&locked[i]), 0x800);
        }
    }

    // Busca el elemento (usage, indice) sin comprobar el final (como el EXE).
    auto findElement = [&elements](BYTE elementUsage, BYTE elementUsageIndex) -> const D3DVERTEXELEMENT9& {
        RwUInt32 k = 0;
        while (elements[k].Usage != elementUsage || elements[k].UsageIndex != elementUsageIndex) {
            ++k;
        }
        return elements[k];
    };
    const RwUInt32 totalNumVertex = header->totalNumVertex;
    RpMorphTarget* const morphTarget = geometry->morphTarget;

    if ((lockedFlags & 2) != 0) {
        const D3DVERTEXELEMENT9& element = findElement(D3DDECLUSAGE_POSITION, 0);
        _rpD3D9VertexDeclarationInstV3d(element.Type, locked[element.Stream] + element.Offset, morphTarget->verts, static_cast<RwInt32>(totalNumVertex), header->vertexStream[element.Stream].stride);
    }

    if (skin != nullptr && skin->field20 != 0 && lockedFlags == 0xFFF) {
        const RwUInt32 numUsedBones = skin->numUsedBones;
        if (skin->maxNumWeightsPerVertex > 1) {
            const D3DVERTEXELEMENT9& weightElement = findElement(D3DDECLUSAGE_BLENDWEIGHT, 0);
            RwMatrixWeights* const weights = skin->vertexBoneWeights;
            for (RwUInt32 v = 0; v < totalNumVertex; ++v) {
                float* const w = reinterpret_cast<float*>(weights + v);
                if (w[0] > 1.0f) {
                    *reinterpret_cast<RwUInt32*>(&w[0]) = 0x3F800000;
                    *reinterpret_cast<RwUInt32*>(&w[1]) = 0;
                    *reinterpret_cast<RwUInt32*>(&w[2]) = 0;
                    *reinterpret_cast<RwUInt32*>(&w[3]) = 0;
                }
            }
            _rpD3D9VertexDeclarationInstWeights(weightElement.Type, locked[weightElement.Stream] + weightElement.Offset, reinterpret_cast<const RwV4d*>(weights), static_cast<RwInt32>(totalNumVertex), header->vertexStream[weightElement.Stream].stride);
        }

        const D3DVERTEXELEMENT9& indexElement = findElement(D3DDECLUSAGE_BLENDINDICES, 0);
        RwUInt8 remap[255];
        memset(remap, 0, sizeof(remap));
        if (skin->meshBoneRLECount != nullptr) {
            RwUInt8* const remapSource = skin->meshBoneRemapIndices;
            if (static_cast<RwInt32>(remapSource[0]) >= static_cast<RwInt32>(hierarchy->numNodes)) {
                remapSource[0] = 0;
            }
            const RwUInt32 count = static_cast<RwUInt32>(hierarchy->numNodes);
            for (RwUInt32 i = 0; i < count; ++i) {
                if (remapSource[i] < 0xFF) {
                    remap[i] = static_cast<RwUInt8>(remapSource[i] * 3);
                }
            }
        } else {
            const RwUInt32 count = static_cast<RwUInt32>(hierarchy->numNodes);
            if (count <= SkinLimitE8()) {
                for (RwUInt32 i = 0; i < count; ++i) {
                    remap[i] = static_cast<RwUInt8>(i * 3);
                }
            } else {
                remap[0] = 0;
                for (RwUInt32 i = 0; i < numUsedBones; ++i) {
                    remap[skin->usedBonesArray[i]] = static_cast<RwUInt8>(i * 3);
                }
            }
        }
        _rpD3D9VertexDeclarationInstIndicesRemap(indexElement.Type, locked[indexElement.Stream] + indexElement.Offset, skin->vertexBoneIndices, remap, static_cast<RwInt32>(totalNumVertex), header->vertexStream[indexElement.Stream].stride);
    }

    if ((geometry->flags & 0x10) != 0 && (lockedFlags & 4) != 0) {
        const D3DVERTEXELEMENT9& element = findElement(D3DDECLUSAGE_NORMAL, 0);
        _rpD3D9VertexDeclarationInstV3d(element.Type, locked[element.Stream] + element.Offset, morphTarget->normals, static_cast<RwInt32>(totalNumVertex), header->vertexStream[element.Stream].stride);
    }

    if ((usage & 0x10000) != 0 && (lockedFlags & 0xFF2) != 0) {
        const D3DVERTEXELEMENT9& element = findElement(D3DDECLUSAGE_TANGENT, 0);
        const RwTexCoords* const tangents = *reinterpret_cast<RwTexCoords* const*>(reinterpret_cast<const RwUInt8*>(geometry) + 0x30 + 4 * numTexCoordSets);
        _rpD3D9VertexDeclarationInstTangent(element.Type, locked[element.Stream] + element.Offset, morphTarget->verts, tangents, header, header->vertexStream[element.Stream].stride);
    }

    if (numTexCoordSets != 0 && (lockedFlags & 0xFF0) != 0) {
        for (RwUInt32 t = 0; t < numTexCoordSets; ++t) {
            if ((lockedFlags & (0x10u << t)) == 0) {
                continue;
            }
            const D3DVERTEXELEMENT9& element = findElement(D3DDECLUSAGE_TEXCOORD, static_cast<BYTE>(t));
            _rpD3D9VertexDeclarationInstV2d(element.Type, locked[element.Stream] + element.Offset, reinterpret_cast<const RwV2d*>(geometry->texCoords[t]), static_cast<RwInt32>(totalNumVertex), header->vertexStream[element.Stream].stride);
        }
    }

    if ((geometry->flags & 8) != 0 && (lockedFlags & 8) != 0) {
        const D3DVERTEXELEMENT9& element = findElement(D3DDECLUSAGE_COLOR, 0);
        const RwUInt32 stride = header->vertexStream[element.Stream].stride;
        RwUInt32 count = header->numMeshes;
        RxD3D9InstanceData* instance = reinterpret_cast<RxD3D9InstanceData*>(header + 1);
        do {
            const RwUInt32 first = instance->minVert;
            instance->vertexAlpha = _rpD3D9VertexDeclarationInstColor(locked[element.Stream] + first * stride + element.Offset, geometry->preLitLum + first, static_cast<RwInt32>(instance->numVertices), stride);
            ++instance;
        } while (--count != 0);
    }

    for (RwUInt32 i = 0; i < numStreams; ++i) {
        if (locked[i] != nullptr) {
            static_cast<IDirect3DVertexBuffer9*>(header->vertexStream[i].vertexBuffer)->Unlock();
        }
    }
}

namespace {
// Escritura de un vector de 3 floats desde un registro SSE (los tres floats bajos).
inline void SkinStoreVec3(RwUInt8* destination, __m128 value) {
    alignas(16) float lanes[4];
    _mm_store_ps(lanes, value);
    float* const out = reinterpret_cast<float*>(destination);
    out[0] = lanes[0];
    out[1] = lanes[1];
    out[2] = lanes[2];
}
// Transformacion de un punto como el EXE: (z * fila2 [+ fila3]) + (y * fila1 + x * fila0), con mulps/addps de precision simple por separado y en ese orden.
inline __m128 SkinTransformPoint(const RwUInt8* source, __m128 row0, __m128 row1, __m128 row2, const __m128* row3) {
    const float* const p = reinterpret_cast<const float*>(source);
    __m128 x = _mm_load_ss(p);
    __m128 y = _mm_load_ss(p + 1);
    __m128 z = _mm_load_ss(p + 2);
    x = _mm_shuffle_ps(x, x, 0);
    y = _mm_shuffle_ps(y, y, 0);
    z = _mm_shuffle_ps(z, z, 0);
    x = _mm_mul_ps(x, row0);
    y = _mm_mul_ps(y, row1);
    z = _mm_mul_ps(z, row2);
    y = _mm_add_ps(y, x);
    if (row3 != nullptr) {
        z = _mm_add_ps(z, *row3);
    }
    return _mm_add_ps(z, y);
}
} // namespace

// 0x7CAAD0 (lote R7A, sin validar; el listado decia RpSkin con confianza alta: ayudante SSE del skin generico, la llama 0x7C7EE0; nombre inferido): transforma por UNA matriz alineada a 16 bytes. cdecl, 7
// argumentos de pila (numVertices, matriz, posiciones de salida, posiciones de entrada (12 bytes por vertice), normales de salida, normales de entrada (NULL = no se calculan), paso de salida en bytes). Por vertice (do/while
// con contador sin signo: con 0 el EXE da 2^32 vueltas): posicion = (z * fila2 + fila3) + (y * fila1 + x * fila0) con mulps/addps de precision simple en ese orden (movaps de la matriz: la cuarta lane
// de cada fila se ignora); escribe 3 floats y avanza el paso de salida; normal = (z * fila2) + (y * fila1 + x * fila0) sin traslacion y con el mismo paso.
void _rpSkinSSETransformPoints(RwUInt32 numVertices, const RwMatrix* matrix, RwUInt8* outVertices, const RwUInt8* inVertices, RwUInt8* outNormals, const RwUInt8* inNormals, RwInt32 outStride) {
    const float* const m = reinterpret_cast<const float*>(matrix);
    const __m128 row0 = _mm_load_ps(m);
    const __m128 row1 = _mm_load_ps(m + 4);
    const __m128 row2 = _mm_load_ps(m + 8);
    const __m128 row3 = _mm_load_ps(m + 12);
    do {
        SkinStoreVec3(outVertices, SkinTransformPoint(inVertices, row0, row1, row2, &row3));
        inVertices += 0xC;
        outVertices += outStride;
        if (inNormals != nullptr) {
            SkinStoreVec3(outNormals, SkinTransformPoint(inNormals, row0, row1, row2, nullptr));
            inNormals += 0xC;
            outNormals += outStride;
        }
    } while (--numVertices != 0);
}

// 0x7CAB80 (lote R7A, sin validar; ayudante SSE del skin generico, la llama 0x7C7F06; nombre inferido): igual que 0x7CAAD0 pero la matriz de cada vertice sale de un array por indice: el indice es el byte
// bajo del dword de `matrixIndices` (se avanza 4 bytes por vertice) y la matriz es matrixArray + indice * 64 (alineada a 16). cdecl, 8 argumentos de pila (numVertices, indices, matrices, posiciones de salida,
// posiciones de entrada, normales de salida, normales de entrada (NULL = no se calculan), paso de salida). Los indices avanzan aunque no haya normales; las normales usan la misma matriz
// que la posicion del mismo vertice.
void _rpSkinSSETransformPointsIndexed(RwUInt32 numVertices, const RwUInt32* matrixIndices, const RwMatrix* matrixArray, RwUInt8* outVertices, const RwUInt8* inVertices, RwUInt8* outNormals, const RwUInt8* inNormals, RwInt32 outStride) {
    do {
        const RwUInt32 index = *reinterpret_cast<const RwUInt8*>(matrixIndices);
        ++matrixIndices;
        const float* const m = reinterpret_cast<const float*>(reinterpret_cast<const RwUInt8*>(matrixArray) + (index << 6));
        const __m128 row0 = _mm_load_ps(m);
        const __m128 row1 = _mm_load_ps(m + 4);
        const __m128 row2 = _mm_load_ps(m + 8);
        const __m128 row3 = _mm_load_ps(m + 12);
        SkinStoreVec3(outVertices, SkinTransformPoint(inVertices, row0, row1, row2, &row3));
        inVertices += 0xC;
        outVertices += outStride;
        if (inNormals != nullptr) {
            SkinStoreVec3(outNormals, SkinTransformPoint(inNormals, row0, row1, row2, nullptr));
            inNormals += 0xC;
            outNormals += outStride;
        }
    } while (--numVertices != 0);
}

// 0x7CB2B0 (lote R7A, sin validar; el listado decia RpSkin con confianza alta y es correcto: la llama _rpSkinPipelinesCreate 0x7C898E; nombre inferido): crea el pipeline generico del skin D3D9.
// Lee D3DCAPS9 (0xC9BF00): 0xC978E0 = bit 16 de DevCaps (HWTRANSFORMANDLIGHT); con ese bit y VertexShaderVersion (+0xC4) y PixelShaderVersion (+0xCC), de 16 bits bajos, ambas >= 0x101: 0xC978E4 = 1,
// 0xC978EC = MaxVertexShaderConst (+0xC8) y 0xC978E8 = (MaxVertexShaderConst - 12) / 3 (sin signo, multiplicacion por 0xAAAAAAAB); si no, los tres a 0. Despues RxPipelineCreate (0x8057B0; NULL si
// falla), pluginId = 0x116, pluginData = 1, RxPipelineLock (0x806990; si devuelve NULL destruye el pipeline con 0x805820 y devuelve NULL), RxLockedPipeAddFragment(bloqueado, NULL, 0x8DED08, NULL)
// (nodo AllInOne del skin D3D9), RxLockedPipeUnlock sobre su resultado (sin mirarlo) y devuelve el pipeline.
RxPipeline* _rpSkinD3D9CreateGenericPipeline() {
    const D3DCAPS9* const caps = static_cast<const D3DCAPS9*>(RwD3D9GetCaps());
    const RwUInt32 hardwareTransformAndLight = (caps->DevCaps >> 16) & 1;
    SkinFlagE0() = hardwareTransformAndLight;
    if (hardwareTransformAndLight != 0 && (caps->VertexShaderVersion & 0xFFFF) >= 0x101 && (caps->PixelShaderVersion & 0xFFFF) >= 0x101) {
        SkinFlagE4() = 1;
        const RwUInt32 maxConstants = caps->MaxVertexShaderConst;
        *reinterpret_cast<RwUInt32*>(0xC978EC) = maxConstants;
        SkinLimitE8() = static_cast<RwUInt32>((static_cast<uint64>(static_cast<RwUInt32>(maxConstants - 12)) * 0xAAAAAAABull) >> 33);
    } else {
        SkinFlagE4() = 0;
        SkinLimitE8() = 0;
        *reinterpret_cast<RwUInt32*>(0xC978EC) = 0;
    }
    RxPipeline* const pipeline = RxPipelineCreate();
    if (pipeline == nullptr) {
        return nullptr;
    }
    pipeline->pluginId   = kSkinPluginID;
    pipeline->pluginData = 1;
    RxLockedPipe* const lockedPipe = RxPipelineLock(pipeline);
    if (lockedPipe == nullptr) {
        _rxPipelineDestroy(pipeline);
        return nullptr;
    }
    RxLockedPipeUnlock(RxLockedPipeAddFragment(lockedPipe, nullptr, RxNodeDefinitionGetD3D9SkinAtomicAllInOne(), nullptr));
    return pipeline;
}

// 0x7CB2A0 (lote S12A, sin validar; `mov eax, 0x8DED08; ret`): definicion del nodo de skin D3D9 (datos del EXE, no se copian).
RxNodeDefinition* RxNodeDefinitionGetD3D9SkinAtomicAllInOne() {
    return reinterpret_cast<RxNodeDefinition*>(0x8DED08);
}

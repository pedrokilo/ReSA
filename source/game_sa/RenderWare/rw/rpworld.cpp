/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RpWorld
 * Reconstrucción: COMPLETA — 25/25 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 25/25 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rpworld.h"
#include <rpdbgerr.h>

// Direcciones del EXE usadas por las funciones de este archivo.
namespace {
constexpr uintptr_t kWorldSectorPluginRegistry   = 0x8D736C; // RpWorldSector: lista de plugins
constexpr uintptr_t kD3D9WorldSectorAllInOneNode = 0x8D63B8; // RxNodeDefinition estatica de D3D9WorldSectorAllInOne
constexpr RwUInt32  kMeshHeaderMemoryHint        = 0x30502;  // rwMEMHINTDUR_GLOBAL | 0x0502
constexpr RwUInt32  kGeometryMemoryHint          = 0x3000F;  // RpGeometry y su bloque de morph targets
constexpr RwUInt32  kGeometryTempListHint        = 0x10006;  // lista temporal de texturas de RpGeometryUnlock
constexpr RwUInt32  kGeometryTempRasterHint      = 0x10503;  // listas temporales de rasters y tuberias de RpGeometryUnlock
constexpr RwInt32   kErrorBadVersion             = static_cast<RwInt32>(0x80000004);
constexpr RwInt32   kErrorNoMemory               = static_cast<RwInt32>(0x80000013);
constexpr RwUInt32  kStreamVersionMin            = 0x34000;
constexpr RwUInt32  kStreamVersionMax            = 0x36003;
}

// Registro de plugins de RpMaterial y estado global del modulo de materiales (.data del EXE).
static auto& s_materialTKList          = StaticRef<RwPluginRegistry>(0x8D62A4);
static auto& s_defaultSurfaceProps     = StaticRef<RwSurfaceProperties>(0x8D62BC); // ambiente, especular, difusa
static auto& s_materialGlobalsOffset   = StaticRef<RwInt32>(0xC92514);             // desplazamiento en RwEngineInstance de la lista libre de materiales
static auto& s_materialRightsId        = StaticRef<RwInt32>(0xC92510);             // lectura del plugin 0x1F (derechos de render): identificador
static auto& s_materialRightsExtraData = StaticRef<RwInt32>(0xC924E8);             // lectura del plugin 0x1F: datos extra

static_assert(offsetof(RwGlobals, memoryFuncs) + offsetof(RwMemoryFunctions, rwfree) == 0x138);
static_assert(offsetof(RwGlobals, memoryFuncs) + offsetof(RwMemoryFunctions, rwrealloc) == 0x13C);
static_assert(offsetof(RwGlobals, memoryAlloc) == 0x144 && offsetof(RwGlobals, memoryFree) == 0x148);
static_assert(offsetof(RpGeometry, flags) == 0x08 && offsetof(RpGeometry, lockedSinceLastInst) == 0x0C && offsetof(RpGeometry, refCount) == 0x0E);
static_assert(offsetof(RpGeometry, numTriangles) == 0x10 && offsetof(RpGeometry, numVertices) == 0x14 && offsetof(RpGeometry, numMorphTargets) == 0x18 && offsetof(RpGeometry, numTexCoordSets) == 0x1C);
static_assert(offsetof(RpGeometry, matList) == 0x20 && offsetof(RpGeometry, triangles) == 0x2C && offsetof(RpGeometry, preLitLum) == 0x30 && offsetof(RpGeometry, texCoords) == 0x34);
static_assert(offsetof(RpGeometry, mesh) == 0x54 && offsetof(RpGeometry, repEntry) == 0x58 && offsetof(RpGeometry, morphTarget) == 0x5C);
static_assert(sizeof(RpMorphTarget) == 0x1C && offsetof(RpMorphTarget, verts) == 0x14 && offsetof(RpMorphTarget, normals) == 0x18);
static_assert(sizeof(RpTriangle) == 8 && sizeof(RpGeometryChunkInfo) == 0x10 && sizeof(RpMaterialChunkInfo) == 0x1C);
static_assert(offsetof(RpMaterial, pipeline) == 0x08 && offsetof(RpMaterial, surfaceProps) == 0x0C && offsetof(RpMaterial, refCount) == 0x18);
static_assert(offsetof(RwTexture, refCount) == 0x54 && offsetof(RxPipeline, pluginId) == 0x2C && offsetof(RxPipeline, pluginData) == 0x30);

namespace {
// RWERROR((codigo [, argumento])) del SDK: RwError { pluginID = 2 (literal del EXE), errorCode = _rwerror(codigo [, argumento]) }; _rwerror (0x8088D0) devuelve su primer argumento y
// RwErrorSet (0x808820) guarda el error. Se llama a _rwerror por direccion para conservar la secuencia de colaboradores del EXE.
void RaiseRwError(RwInt32 code) {
    RwError error;
    error.pluginID  = 2;
    error.errorCode = reinterpret_cast<RwInt32(__cdecl*)(RwInt32, ...)>(0x8088D0)(code);
    RwErrorSet(&error);
}

void RaiseRwError(RwInt32 code, RwUInt32 argument) {
    RwError error;
    error.pluginID  = 2;
    error.errorCode = reinterpret_cast<RwInt32(__cdecl*)(RwInt32, ...)>(0x8088D0)(code, argument);
    RwErrorSet(&error);
}

// RwSqrt del SDK (0x7EDB30): raiz cuadrada por tabla de RenderWare; recibe el float por la pila y lo devuelve en ST0. No esta reconstruida: se llama por direccion fija.
RwReal RwSqrtFromExe(RwReal value) {
    return reinterpret_cast<RwReal(__cdecl*)(RwReal)>(0x7EDB30)(value);
}

// Lista libre de RpMaterial: puntero guardado en RwEngineInstance + globalsOffset del modulo de materiales (0xC92514).
RwFreeList*& MaterialFreeList() {
    return *reinterpret_cast<RwFreeList**>(reinterpret_cast<uint8_t*>(static_cast<RwGlobals*>(RwEngineInstance)) + s_materialGlobalsOffset);
}
}

void RpWorldInjectHooks() {
    RH_ScopedNamespaceName("RpWorld");
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(_rpMeshHeaderCreate, 0x758920);
    RH_ScopedGlobalInstall(RpBuildMeshGenerateDefaultTriStrip, 0x7591B0);
    RH_ScopedGlobalInstall(RxNodeDefinitionGetD3D9WorldSectorAllInOne, 0x75E9F0);
    RH_ScopedGlobalInstall(RpWorldSectorRegisterPlugin, 0x761C90);
    RH_ScopedGlobalInstall(RpWorldSectorRegisterPluginStream, 0x761CC0);
}

void _rwD3D9VSSetActiveWorldMatrix(const RwMatrix* worldMatrix) {
    ((void(__cdecl *)(const RwMatrix*))0x764650)(worldMatrix);
}

void _rwD3D9VSGetComposedTransformMatrix() {
    ((void(__cdecl *)(void))0x7646E0)();
}

void _rwD3D9VSGetWorldViewTransposedMatrix() {
    ((void(__cdecl *)(void))0x764730)();
}

void _rwD3D9VSGetWorldViewMatrix() {
    ((void(__cdecl *)(void))0x764760)();
}

void _rwD3D9VSGetInverseWorldMatrix() {
    ((void(__cdecl *)(void))0x7647B0)();
}

void _rwD3D9VSGetWorldMultiplyMatrix(RwMatrix* worldMultiplyMatrix, const RwMatrix* matrix) {
    ((void(__cdecl *)(RwMatrix*, const RwMatrix*))0x764920)(worldMultiplyMatrix, matrix);
}

void _rwD3D9VSGetWorldMultiplyTransposeMatrix(void* worldMultiplyMatrix, const RwMatrix* matrix) {
    ((void(__cdecl *)(void*, const RwMatrix*))0x764960)(worldMultiplyMatrix, matrix);
}

void _rwD3D9VSGetWorldViewMultiplyTransposeMatrix(void* worldViewMultiplyMatrix, const RwMatrix* matrix) {
    ((void(__cdecl *)(void*, const RwMatrix*))0x7649D0)(worldViewMultiplyMatrix, matrix);
}

void _rwD3D9VSGetWorldNormalizedMultiplyTransposeMatrix(void* worldMultiplyMatrix, const RwMatrix* matrix) {
    ((void(__cdecl *)(void*, const RwMatrix*))0x764A70)(worldMultiplyMatrix, matrix);
}

void _rwD3D9VSGetWorldNormalizedViewMultiplyTransposeMatrix(void* worldViewMultiplyMatrix, const RwMatrix* matrix) {
    ((void(__cdecl *)(void*, const RwMatrix*))0x764B50)(worldViewMultiplyMatrix, matrix);
}

void _rwD3D9VSGetWorldNormalizedTransposeMatrix() {
    ((void(__cdecl *)(void))0x764C60)();
}

void _rwD3D9VSGetProjectionTransposedMatrix() {
    ((void(__cdecl *)(void))0x764D20)();
}

void _rwD3D9VSGetNormalInLocalSpace(const RwV3d* normalWorldSpace, RwV3d* normalLocalSpace) {
    ((void(__cdecl *)(const RwV3d*, RwV3d*))0x764D30)(normalWorldSpace, normalLocalSpace);
}

void _rwD3D9VSGetPointInLocalSpace(const RwV3d* pointWorldSpace, RwV3d* pointLocalSpace) {
    ((void(__cdecl *)(const RwV3d*, RwV3d*))0x764E70)(pointWorldSpace, pointLocalSpace);
}

void _rwD3D9VSGetRadiusInLocalSpace(RwReal radiusWorldSpace, RwReal* radiusLocalSpace) {
    ((void(__cdecl *)(RwReal, RwReal*))0x764F60)(radiusWorldSpace, radiusLocalSpace);
}

void _rpD3D9VertexShaderCachePurge() {
    ((void(__cdecl *)(void))0x75EE60)();
}

void* _rpD3D9GetVertexShader(const _rpD3D9VertexShaderDescriptor* desc, _rpD3D9VertexShaderDispatchDescriptor* dispatch) {
    return ((void*(__cdecl *)(const _rpD3D9VertexShaderDescriptor*, _rpD3D9VertexShaderDispatchDescriptor*))0x75EED0)(desc, dispatch);
}

RwUInt32 _rpD3D9GetNumConstantsUsed(const _rpD3D9VertexShaderDescriptor* desc) {
    return ((RwUInt32(__cdecl *)(const _rpD3D9VertexShaderDescriptor*))0x75EDD0)(desc);
}

void _rxD3D9VertexShaderDefaultBeginCallBack(void* object, RwUInt32 type, _rpD3D9VertexShaderDescriptor* desc) {
    ((void(__cdecl *)(void*, RwUInt32, _rpD3D9VertexShaderDescriptor*))0x760DF0)(object, type, desc);
}

RwV4d* _rxD3D9VertexShaderDefaultLightingCallBack(void* object, RwUInt32 type, RwV4d* shaderConstantPtr, _rpD3D9VertexShaderDescriptor* desc) {
    return ((RwV4d*(__cdecl *)(void*, RwUInt32, RwV4d*, _rpD3D9VertexShaderDescriptor*))0x761170)(object, type, shaderConstantPtr, desc);
}

void* _rxD3D9VertexShaderDefaultGetMaterialShaderCallBack(const RpMaterial* material, _rpD3D9VertexShaderDescriptor* desc, _rpD3D9VertexShaderDispatchDescriptor* dispatch) {
    return ((void*(__cdecl *)(const RpMaterial*, _rpD3D9VertexShaderDescriptor*, _rpD3D9VertexShaderDispatchDescriptor*))0x761010)(material, desc, dispatch);
}

void _rxD3D9VertexShaderDefaultMeshRenderCallBack(RxD3D9ResEntryHeader* resEntryHeader, RxD3D9InstanceData* instancedMesh, const _rpD3D9VertexShaderDescriptor* desc, const _rpD3D9VertexShaderDispatchDescriptor* dispatch) {
    ((void(__cdecl *)(RxD3D9ResEntryHeader*, RxD3D9InstanceData*, const _rpD3D9VertexShaderDescriptor*, const _rpD3D9VertexShaderDispatchDescriptor*))0x761030)(resEntryHeader, instancedMesh, desc, dispatch);
}

void _rxD3D9VertexShaderDefaultEndCallBack(void* object, RwUInt32 type, _rpD3D9VertexShaderDescriptor* desc) {
    ((void(__cdecl *)(void*, RwUInt32, _rpD3D9VertexShaderDescriptor*))0x761000)(object, type, desc);
}

void _rpD3D9VertexShaderUpdateLightsColors(RwV4d* shaderConstantPtr, const _rpD3D9VertexShaderDescriptor* desc, RwReal ambientCoef, RwReal diffuseCoef) {
    ((void(__cdecl *)(RwV4d*, const _rpD3D9VertexShaderDescriptor*, RwReal, RwReal))0x761720)(shaderConstantPtr, desc, ambientCoef, diffuseCoef);
}

void _rpD3D9VertexShaderUpdateMaterialColor(const RwRGBA* color, const _rpD3D9VertexShaderDispatchDescriptor* dispatch) {
    ((void(__cdecl *)(const RwRGBA*, const _rpD3D9VertexShaderDispatchDescriptor*))0x761820)(color, dispatch);
}

void _rpD3D9VertexShaderUpdateFogData(const _rpD3D9VertexShaderDescriptor* desc, const _rpD3D9VertexShaderDispatchDescriptor* dispatch) {
    ((void(__cdecl *)(const _rpD3D9VertexShaderDescriptor*, const _rpD3D9VertexShaderDispatchDescriptor*))0x7618B0)(desc, dispatch);
}

void _rpD3D9VertexShaderUpdateMorphingCoef(RwReal morphingCoef, const _rpD3D9VertexShaderDispatchDescriptor* dispatch) {
    ((void(__cdecl *)(RwReal, const _rpD3D9VertexShaderDispatchDescriptor*))0x761950)(morphingCoef, dispatch);
}

void _rpD3DVertexShaderSetUVAnimMatrix(RwMatrix* matrix, const _rpD3D9VertexShaderDispatchDescriptor* dispatch) {
    ((void(__cdecl *)(RwMatrix*, const _rpD3D9VertexShaderDispatchDescriptor*))0x7619A0)(matrix, dispatch);
}

void _rpD3D9VertexShaderSetEnvMatrix(RwFrame* frame, const _rpD3D9VertexShaderDescriptor* desc, const _rpD3D9VertexShaderDispatchDescriptor* dispatch) {
    ((void(__cdecl *)(RwFrame*, const _rpD3D9VertexShaderDescriptor*, const _rpD3D9VertexShaderDispatchDescriptor*))0x761A10)(frame, desc, dispatch);
}

void _rpD3D9VertexShaderSetBumpMatrix(RwFrame* frame, RwReal factor, const _rpD3D9VertexShaderDispatchDescriptor* dispatch) {
    ((void(__cdecl *)(RwFrame*, RwReal, const _rpD3D9VertexShaderDispatchDescriptor*))0x761B70)(frame, factor, dispatch);
}

// 0x75E9F0: `mov eax, 0x8D63B8; ret`; devuelve la direccion de la definicion estatica del EXE, sin leer ni escribir nada.
RxNodeDefinition* RxNodeDefinitionGetD3D9WorldSectorAllInOne() {
    return reinterpret_cast<RxNodeDefinition*>(kD3D9WorldSectorAllInOneNode);
}

RxNodeDefinition* RxNodeDefinitionGetD3D9AtomicAllInOne() {
    return ((RxNodeDefinition*(__cdecl *)(void))0x7582E0)();
}

void RpMaterialSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x74D8C0)(blockSize, numBlocksToPrealloc);
}

// 0x74D990
// Pide un RpMaterial a la lista libre (pista 0x30007; solo comprueba nulo), pone refCount = 1, color blanco, sin textura ni tuberia, las propiedades de superficie por defecto
// (0x8D62BC..C4) y llama a _rwPluginRegistryInitObject sobre el registro de materiales (0x8D62A4). El campo pad (+0x1A) no se escribe.
RpMaterial* RpMaterialCreate() {
    RpMaterial* material = static_cast<RpMaterial*>(RwFreeListAlloc(MaterialFreeList(), 0x30007));
    if (!material) {
        return nullptr;
    }
    material->refCount = 1;
    material->color = { 0xFF, 0xFF, 0xFF, 0xFF };
    material->texture = nullptr;
    material->pipeline = nullptr;
    material->surfaceProps = s_defaultSurfaceProps;
    _rwPluginRegistryInitObject(&s_materialTKList, material);
    return material;
}

// 0x74DA20
// Con refCount == 1 (comparacion exacta de 16 bits): DeInitObject del registro, RwTextureDestroy de la textura si la hay, textura = nula y se devuelve el bloque a la lista libre; con
// cualquier otro valor solo se resta uno (un 0 pasa a -1). Devuelve siempre TRUE.
RwBool RpMaterialDestroy(RpMaterial* material) {
    const RwInt16 refCount = material->refCount;
    if (refCount == 1) {
        _rwPluginRegistryDeInitObject(&s_materialTKList, material);
        if (material->texture) {
            RwTextureDestroy(material->texture);
        }
        material->texture = nullptr;
        RwFreeListFree(MaterialFreeList(), material);
        return TRUE;
    }
    material->refCount = static_cast<RwInt16>(refCount - 1);
    return TRUE;
}

RpMaterial* RpMaterialClone(RpMaterial* material) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*))0x74DA80)(material);
}

// 0x74DBC0
// Suma una referencia a la textura nueva (si no es nula; refCount de 32 bits en +0x54) ANTES de soltar la anterior (RwTextureDestroy si la habia) y la guarda. Devuelve el material.
RpMaterial* RpMaterialSetTexture(RpMaterial* material, RwTexture* texture) {
    if (texture) {
        ++texture->refCount;
    }
    if (material->texture) {
        RwTextureDestroy(material->texture);
    }
    material->texture = texture;
    return material;
}

// 0x74DBF0: _rwPluginRegistryAddPlugin sobre el registro de materiales (0x8D62A4).
RwInt32 RpMaterialRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return _rwPluginRegistryAddPlugin(&s_materialTKList, size, pluginID, constructCB, destructCB, copyCB);
}

// 0x74DC20: _rwPluginRegistryAddPluginStream sobre el registro de materiales.
RwInt32 RpMaterialRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return _rwPluginRegistryAddPluginStream(&s_materialTKList, pluginID, readCB, writeCB, getSizeCB);
}

RwInt32 RpMaterialSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkAlwaysCallBack))0x74DC50)(pluginID, alwaysCB);
}

RwInt32 RpMaterialSetStreamRightsCallBack(RwUInt32 pluginID, RwPluginDataChunkRightsCallBack rightsCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkRightsCallBack))0x74DC70)(pluginID, rightsCB);
}

RwInt32 RpMaterialGetPluginOffset(RwUInt32 pluginID) {
    return ((RwInt32(__cdecl *)(RwUInt32))0x74DC90)(pluginID);
}

RwBool RpMaterialValidatePlugins(const RpMaterial* material) {
    return ((RwBool(__cdecl *)(const RpMaterial*))0x74DCB0)(material);
}

// 0x74DD30
// Lee un RpMaterial. Busca el chunk STRUCT (1) y comprueba la version (0x34000..0x36003, si no error 0x80000004); lee `longitud` bytes (la del chunk, sin limitar a 28) sobre un
// RpMaterialChunkInfo puesto a cero; pide el material a la lista libre y lo inicializa como RpMaterialCreate (color blanco, refCount 1, propiedades por defecto) y llama a InitObject ANTES de
// copiar color y propiedades del chunk. Si `textured`, busca el chunk TEXTURE (6) y llama a RwTextureStreamRead sin comprobar el resultado. Despues lee los datos de plugins (con las
// variables de derechos 0xC92510/0xC924E8 a cero) y, si el plugin de derechos dejo un id, invoca _rwPluginRegistryInvokeRights. Los fallos tras crear el material lo destruyen
// (refCount 1) o solo restan una referencia; devuelve nulo.
RpMaterial* RpMaterialStreamRead(RwStream* stream) {
    RwUInt32 version;
    RwUInt32 length;
    if (!RwStreamFindChunk(stream, rwID_STRUCT, &length, &version)) {
        return nullptr;
    }
    if (version < kStreamVersionMin || version > kStreamVersionMax) {
        RaiseRwError(kErrorBadVersion);
        return nullptr;
    }
    RpMaterialChunkInfo chunkInfo{};
    if (RwStreamRead(stream, &chunkInfo, length) != length) {
        return nullptr;
    }
    RpMaterial* material = static_cast<RpMaterial*>(RwFreeListAlloc(MaterialFreeList(), 0x30007));
    if (!material) {
        return nullptr;
    }
    material->refCount = 1;
    material->color = { 0xFF, 0xFF, 0xFF, 0xFF };
    material->texture = nullptr;
    material->pipeline = nullptr;
    material->surfaceProps = s_defaultSurfaceProps;
    _rwPluginRegistryInitObject(&s_materialTKList, material);
    material->color = chunkInfo.color;
    material->surfaceProps = chunkInfo.surfaceProps;
    material->texture = nullptr;

    // Cierre de fallo comun: con refCount == 1 se destruye el material; con otro valor solo se resta uno.
    const auto destroyOrRelease = [material]() {
        const RwInt16 refCount = material->refCount;
        if (refCount != 1) {
            material->refCount = static_cast<RwInt16>(refCount - 1);
            return;
        }
        _rwPluginRegistryDeInitObject(&s_materialTKList, material);
        if (material->texture) {
            RwTextureDestroy(material->texture);
        }
        material->texture = nullptr;
        RwFreeListFree(MaterialFreeList(), material);
    };

    if (chunkInfo.textured) {
        if (!RwStreamFindChunk(stream, rwID_TEXTURE, nullptr, &version)) {
            destroyOrRelease();
            return nullptr;
        }
        if (version < kStreamVersionMin || version > kStreamVersionMax) {
            destroyOrRelease();
            RaiseRwError(kErrorBadVersion);
            return nullptr;
        }
        material->texture = RwTextureStreamRead(stream);
    }
    s_materialRightsId = 0;
    s_materialRightsExtraData = 0;
    if (!_rwPluginRegistryReadDataChunks(&s_materialTKList, stream, material)) {
        destroyOrRelease();
        return nullptr;
    }
    if (s_materialRightsId) {
        _rwPluginRegistryInvokeRights(&s_materialTKList, s_materialRightsId, material, s_materialRightsExtraData);
    }
    return material;
}

// 0x74D870
// Fija las propiedades de superficie por defecto (0x8D62BC: ambiente, 0x8D62C0: especular, 0x8D62C4: difusa). Con nulo escribe 1.0 en los tres, en el orden ambiente, difusa, especular.
// Copia los dwords tal cual (sin pasar por el FPU).
void _rpMaterialSetDefaultSurfaceProperties(RwSurfaceProperties* surfProps) {
    auto* const target = reinterpret_cast<RwUInt32*>(&s_defaultSurfaceProps);
    if (!surfProps) {
        constexpr RwUInt32 kOne = 0x3F800000; // 1.0f
        target[0] = kOne;
        target[2] = kOne;
        target[1] = kOne;
        return;
    }
    const auto* const source = reinterpret_cast<const RwUInt32*>(surfProps);
    target[0] = source[0];
    target[1] = source[1];
    target[2] = source[2];
}

// 0x74D7D0
// Lectura del plugin 0x1F de RpMaterial (derechos de render): RwStreamReadInt32 de 4 bytes a 0xC92510 y, si la longitud del chunk es exactamente 8, otros 4 a 0xC924E8. Devuelve el flujo
// o nulo si falla una lectura. No usa `object`.
RwStream* _rpMaterialRightsStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    if (!RwStreamReadInt32(stream, &s_materialRightsId, 4)) {
        return nullptr;
    }
    if (binaryLength == 8 && !RwStreamReadInt32(stream, &s_materialRightsExtraData, 4)) {
        return nullptr;
    }
    return stream;
}

// 0x74D810
// Escritura del plugin 0x1F: escribe con RwStreamWriteInt32 (4 bytes cada uno) pipeline->pluginId (+0x2C) y pipeline->pluginData (+0x30), donde pipeline es el campo +8 del material (se
// relee en cada llamada). Devuelve el flujo, o nulo si falla cualquier escritura.
RwStream* _rpMaterialRightsStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    const auto* const material = static_cast<const RpMaterial*>(object);
    if (!RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&material->pipeline->pluginId), 4)) {
        return nullptr;
    }
    return RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&material->pipeline->pluginData), 4) ? stream : nullptr;
}

const RpMaterial* RpMaterialStreamWrite(const RpMaterial* material, RwStream* stream) {
    return ((const RpMaterial*(__cdecl *)(const RpMaterial*, RwStream*))0x74E050)(material, stream);
}

RpMaterialChunkInfo* _rpMaterialChunkInfoRead(RwStream* stream, RpMaterialChunkInfo* materialChunkInfo, RwInt32* bytesRead) {
    return ((RpMaterialChunkInfo*(__cdecl *)(RwStream*, RpMaterialChunkInfo*, RwInt32*))0x74DCC0)(stream, materialChunkInfo, bytesRead);
}

// Registros de plugins del SDK (RwPluginRegistry en .data del EXE) y desplazamientos de plugin que dejan registrados RpWorldPluginAttach y RpD3D9 (grupo g25b).
static auto& s_atomicTKList  = StaticRef<RwPluginRegistry>(0x8D624C);
static auto& s_worldTKList   = StaticRef<RwPluginRegistry>(0x8D62D0);
static auto& s_lightTKList   = StaticRef<RwPluginRegistry>(0x8D62F8);
static auto& s_atomicWorldOffset        = StaticRef<RwInt32>(0xC925A8); // escrito en 0x7500BC
static auto& s_geometryUsageFlagsOffset = StaticRef<RwInt32>(0xC926E8); // escrito en 0x75880E
static auto& s_sectorUsageFlagsOffset   = StaticRef<RwInt32>(0xC926EC); // escrito en 0x75883F

// Lote 226A (sin validar): funciones de este archivo definidas mas abajo; se declaran aqui para registrarlas.
static RwStream* AtomicRightsStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject); // 0x749120
static RwStream* AtomicRightsStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject); // 0x749160
static void      ClumpFreeListDestroyCallBack(void* clump, void* data); // 0x749520
static void      AtomicFreeListDestroyCallBack(void* atomic, void* data); // 0x7496A0
static RwBool    ClumpPluginAttach(); // 0x749830
static RpAtomic* AtomicSyncCallBack(RpAtomic* atomic); // 0x749D20

// Registro de los hooks del grupo g25b en rpworld.cpp (propio: el sufijo evita chocar con otros registradores de este archivo).
void RpWorldInjectHooks25b() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpWorld");

    // Lote 226A (sin validar)
    RH_ScopedGlobalInstall(AtomicRightsStreamRead, 0x749120);
    RH_ScopedGlobalInstall(AtomicRightsStreamWrite, 0x749160);
    RH_ScopedGlobalInstall(_rpAtomicResyncInterpolatedSphere, 0x7491F0);
    RH_ScopedGlobalInstall(RpAtomicGetWorldBoundingSphere, 0x749330);
    RH_ScopedGlobalInstall(ClumpFreeListDestroyCallBack, 0x749520);
    RH_ScopedGlobalInstall(AtomicFreeListDestroyCallBack, 0x7496A0);
    RH_ScopedGlobalInstall(ClumpPluginAttach, 0x749830);
    RH_ScopedGlobalInstall(RpClumpRender, 0x749B20);
    RH_ScopedGlobalInstall(RpClumpForAllCameras, 0x749BB0);
    RH_ScopedGlobalInstall(RpClumpForAllLights, 0x749C00);
    RH_ScopedGlobalInstall(RpAtomicCreate, 0x749C50);
    RH_ScopedGlobalInstall(AtomicSyncCallBack, 0x749D20);
    RH_ScopedGlobalInstall(RpAtomicSetGeometry, 0x749D40);
    RH_ScopedGlobalInstall(RpAtomicDestroy, 0x749DC0);
    RH_ScopedGlobalInstall(RpAtomicClone, 0x749E60);

    RH_ScopedGlobalInstall(AtomicDefaultRenderCallBack, 0x7491C0);
    RH_ScopedGlobalInstall(RpClumpGetNumAtomics, 0x7498E0);
    RH_ScopedGlobalInstall(RpClumpForAllAtomics, 0x749B70);
    RH_ScopedGlobalInstall(RpAtomicSetFrame, 0x74BF20);
    RH_ScopedGlobalInstall(RpGeometryForAllMaterials, 0x74C790);
    RH_ScopedGlobalInstall(RpAtomicSetStreamAlwaysCallBack, 0x74BE30);
    RH_ScopedGlobalInstall(RpAtomicSetStreamRightsCallBack, 0x74BE50);
    RH_ScopedGlobalInstall(RpAtomicGetPluginOffset, 0x74BEC0);
    RH_ScopedGlobalInstall(RpGeometryTriangleGetMaterial, 0x74C760);
    RH_ScopedGlobalInstall(_rpMaterialListInitialize, 0x74E1B0);
    RH_ScopedGlobalInstall(_rpMaterialListGetMaterial, 0x74E2B0);
    RH_ScopedGlobalInstall(_rpMaterialListFindMaterialIndex, 0x74E420);
    RH_ScopedGlobalInstall(RpWorldRegisterPlugin, 0x74FCD0);
    RH_ScopedGlobalInstall(RpWorldRegisterPluginStream, 0x74FD00);
    RH_ScopedGlobalInstall(RpAtomicGetWorld, 0x751050);
    RH_ScopedGlobalInstall(RpLightRegisterPlugin, 0x751D60);
    RH_ScopedGlobalInstall(_rpD3D9VertexDeclarationGetSize, 0x7522E0);
    RH_ScopedGlobalInstall(RpD3D9GeometryGetUsageFlags, 0x7588D0);
    RH_ScopedGlobalInstall(RpD3D9WorldSectorSetUsageFlags, 0x7588E0);
    RH_ScopedGlobalInstall(RpD3D9WorldSectorGetUsageFlags, 0x758900);
}

// Registro de los hooks del lote 228A (sin validar): geometrias y materiales del SDK de RenderWare (bageomet.c / bamateri.c).
void RpWorldInjectHooks228A() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpWorld");

    RH_ScopedGlobalInstall(RpMorphTargetCalcBoundingSphere, 0x74C200);
    RH_ScopedGlobalInstall(RpGeometryAddMorphTargets, 0x74C310);
    RH_ScopedGlobalInstall(RpGeometryTriangleSetVertexIndices, 0x74C690);
    RH_ScopedGlobalInstall(RpGeometryTriangleSetMaterial, 0x74C6C0);
    RH_ScopedGlobalInstall(RpGeometryLock, 0x74C7D0);
    RH_ScopedGlobalInstall(RpGeometryUnlock, 0x74C800);
    RH_ScopedGlobalInstall(RpGeometryCreate, 0x74CA90);
    RH_ScopedGlobalInstall(RpGeometryAddRef, 0x74CCB0);
    RH_ScopedGlobalInstall(RpGeometryDestroy, 0x74CCC0);
    RH_ScopedGlobalInstall(RpGeometryRegisterPlugin, 0x74CD70);
    RH_ScopedGlobalInstall(RpGeometryRegisterPluginStream, 0x74CDA0);
    RH_ScopedGlobalInstall(RpGeometryStreamRead, 0x74D190);
    RH_ScopedGlobalInstall(_rpGeometryDestroyUnreferenced, 0x74D6D0);
    RH_ScopedGlobalInstall(_rpMaterialRightsStreamRead, 0x74D7D0);
    RH_ScopedGlobalInstall(_rpMaterialRightsStreamWrite, 0x74D810);
    RH_ScopedGlobalInstall(_rpMaterialSetDefaultSurfaceProperties, 0x74D870);
    RH_ScopedGlobalInstall(RpMaterialCreate, 0x74D990);
    RH_ScopedGlobalInstall(RpMaterialDestroy, 0x74DA20);
    RH_ScopedGlobalInstall(RpMaterialSetTexture, 0x74DBC0);
    RH_ScopedGlobalInstall(RpMaterialRegisterPlugin, 0x74DBF0);
    RH_ScopedGlobalInstall(RpMaterialRegisterPluginStream, 0x74DC20);
    RH_ScopedGlobalInstall(RpMaterialStreamRead, 0x74DD30);
}

// 0x74E1B0
RpMaterialList* _rpMaterialListInitialize(RpMaterialList* matList) {
    matList->space        = 0;
    matList->materials    = nullptr;
    matList->numMaterials = 0;
    return matList;
}

RpMaterial** _rpMaterialListAlloc(RwUInt32 count) {
    return ((RpMaterial**(__cdecl *)(RwUInt32))0x74E1C0)(count);
}

// 0x74E2B0
RpMaterial* _rpMaterialListGetMaterial(const RpMaterialList* matList, RwInt32 matIndex) {
    return matList->materials[matIndex];
}

RpMaterialList* _rpMaterialListSetSize(RpMaterialList* matList, RwInt32 size) {
    return ((RpMaterialList*(__cdecl *)(RpMaterialList*, RwInt32))0x74E2C0)(matList, size);
}

RpMaterialList* _rpMaterialListCopy(RpMaterialList* matListOut, const RpMaterialList* matListIn) {
    return ((RpMaterialList*(__cdecl *)(RpMaterialList*, const RpMaterialList*))0x74E1F0)(matListOut, matListIn);
}

// 0x74E420
// Busca de atras hacia delante; sin coincidencia (o con numMaterials <= 0) devuelve numMaterials-1 como el EXE (dec eax, con vuelta en 32 bits).
RwInt32 _rpMaterialListFindMaterialIndex(const RpMaterialList* matList, const RpMaterial* material) {
    RwInt32 index = matList->numMaterials;
    if (index > 0) {
        RpMaterial* const* cursor = matList->materials + index;
        do {
            const RpMaterial* const current = *--cursor;
            --index;
            if (current == material) {
                return index;
            }
        } while (index > 0);
    }
    return static_cast<RwInt32>(static_cast<RwUInt32>(index) - 1u);
}

const RpMaterialList* _rpMaterialListStreamWrite(const RpMaterialList* matList, RwStream* stream) {
    return ((const RpMaterialList*(__cdecl *)(const RpMaterialList*, RwStream*))0x74E4B0)(matList, stream);
}

void* _rpMeshOpen(void* instance, RwInt32 offset, RwInt32 size) {
    return ((void*(__cdecl *)(void*, RwInt32, RwInt32))0x758970)(instance, offset, size);
}

void* _rpMeshClose(void* instance, RwInt32 offset, RwInt32 size) {
    return ((void*(__cdecl *)(void*, RwInt32, RwInt32))0x758940)(instance, offset, size);
}

RwInt16 _rpMeshGetNextSerialNumber(void) {
    return ((RwInt16(__cdecl *)(void))0x7590E0)();
}

RpBuildMesh* _rpBuildMeshCreate(RwUInt32 bufferSize) {
    return ((RpBuildMesh*(__cdecl *)(RwUInt32))0x758A90)(bufferSize);
}

RwBool _rpBuildMeshDestroy(RpBuildMesh* mesh) {
    return ((RwBool(__cdecl *)(RpBuildMesh*))0x758B80)(mesh);
}

RwBool _rpMeshDestroy(RpMeshHeader* mesh) {
    return ((RwBool(__cdecl *)(RpMeshHeader*))0x758BC0)(mesh);
}

RpBuildMesh* _rpBuildMeshAddTriangle(RpBuildMesh* mesh, RpMaterial* material, RwInt32 vert1, RwInt32 vert2, RwInt32 vert3, RwUInt16 matIndex, RwUInt16 textureIndex, RwUInt16 rasterIndex, RwUInt16 pipelineIndex) {
    return ((RpBuildMesh*(__cdecl *)(RpBuildMesh*, RpMaterial*, RwInt32, RwInt32, RwInt32, RwUInt16, RwUInt16, RwUInt16, RwUInt16))0x758C00)(mesh, material, vert1, vert2, vert3, matIndex, textureIndex, rasterIndex, pipelineIndex);
}

RpMeshHeader* _rpMeshHeaderForAllMeshes(RpMeshHeader* meshHeader, RpMeshCallBack fpCallBack, void* data) {
    return ((RpMeshHeader*(__cdecl *)(RpMeshHeader*, RpMeshCallBack, void*))0x758D30)(meshHeader, fpCallBack, data);
}

RwStream* _rpMeshWrite(const RpMeshHeader* meshHeader, const void* object, RwStream* stream, const RpMaterialList* matList) {
    return ((RwStream*(__cdecl *)(const RpMeshHeader*, const void*, RwStream*, const RpMaterialList*))0x758D70)(meshHeader, object, stream, matList);
}

RpMeshHeader* _rpMeshRead(RwStream* stream, const void* object, const RpMaterialList* matList) {
    return ((RpMeshHeader*(__cdecl *)(RwStream*, const void*, const RpMaterialList*))0x758EC0)(stream, object, matList);
}

RwInt32 _rpMeshSize(const RpMeshHeader* meshHeader, const void* object) {
    return ((RwInt32(__cdecl *)(const RpMeshHeader*, const void*))0x759090)(meshHeader, object);
}

void _rpMeshHeaderDestroy(RpMeshHeader* meshHeader) {
    ((void(__cdecl *)(RpMeshHeader*))0x758910)(meshHeader);
}

// 0x758920: `call [RwEngineInstance + 0x134](size, 0x30502)`, es decir RwMalloc del motor (memoryFuncs.rwmalloc) con la pista de memoria
// global de mallas. Lee el puntero del motor (0xC97B24) en cada llamada y devuelve tal cual lo que devuelva el asignador; no escribe nada.
static_assert(offsetof(RwGlobals, memoryFuncs) + offsetof(RwMemoryFunctions, rwmalloc) == 0x134);
RpMeshHeader* _rpMeshHeaderCreate(RwUInt32 size) {
    return static_cast<RpMeshHeader*>(RwMalloc(size, kMeshHeaderMemoryHint));
}

RpTriStripPolygon* RpTriStripPolygonFollowStrip(RpTriStripPolygon* curr, RpTriStripPolygon* prev) {
    return ((RpTriStripPolygon*(__cdecl *)(RpTriStripPolygon*, RpTriStripPolygon*))0x75C330)(curr, prev);
}

RwUInt32 RpTriStripDefaultCost(RpTriStripPolygon* startPolygon, RwUInt32 testFrame, void* data) {
    return ((RwUInt32(__cdecl *)(RpTriStripPolygon*, RwUInt32, void*))0x75B500)(startPolygon, testFrame, data);
}

RpTriStripMesh* RpTriStripMeshTunnel(RpTriStripMesh* mesh, void* data) {
    return ((RpTriStripMesh*(__cdecl *)(RpTriStripMesh*, void*))0x75B780)(mesh, data);
}

RpTriStripMesh* RpTriStripMeshQuick(RpTriStripMesh* mesh, void* data) {
    return ((RpTriStripMesh*(__cdecl *)(RpTriStripMesh*, void*))0x75BD80)(mesh, data);
}

RpMeshHeader* RpBuildMeshGenerateTriStrip(RpBuildMesh* buildMesh, void* data) {
    return ((RpMeshHeader*(__cdecl *)(RpBuildMesh*, void*))0x75C380)(buildMesh, data);
}

RpMeshHeader* RpBuildMeshGenerateTrivialTriStrip(RpBuildMesh* buildMesh, void* data) {
    return ((RpMeshHeader*(__cdecl *)(RpBuildMesh*, void*))0x759100)(buildMesh, data);
}

// 0x7591B0: llama a la rutina interna 0x7591D0 (todavia sin reconstruir) con (buildMesh, 0, 1) y devuelve su resultado; `data` no se usa.
// Se llama por direccion, una sola vez, igual que el EXE.
RpMeshHeader* RpBuildMeshGenerateDefaultTriStrip(RpBuildMesh* buildmesh, void* data) {
    return reinterpret_cast<RpMeshHeader*(__cdecl*)(RpBuildMesh*, RwInt32, RwInt32)>(0x7591D0)(buildmesh, 0, 1);
}

RpMeshHeader* RpBuildMeshGeneratePreprocessTriStrip(RpBuildMesh* buildmesh, void* data) {
    return ((RpMeshHeader*(__cdecl *)(RpBuildMesh*, void*))0x75A900)(buildmesh, data);
}

RpMeshHeader* RpBuildMeshGenerateExhaustiveTriStrip(RpBuildMesh* buildmesh, void* data) {
    return ((RpMeshHeader*(__cdecl *)(RpBuildMesh*, void*))0x75A940)(buildmesh, data);
}

RpMeshHeader* RpBuildMeshGenerateDefaultIgnoreWindingTriStrip(RpBuildMesh* buildmesh, void* data) {
    return ((RpMeshHeader*(__cdecl *)(RpBuildMesh*, void*))0x75A8E0)(buildmesh, data);
}

RpMeshHeader* RpBuildMeshGeneratePreprocessIgnoreWindingTriStrip(RpBuildMesh* buildmesh, void* data) {
    return ((RpMeshHeader*(__cdecl *)(RpBuildMesh*, void*))0x75A920)(buildmesh, data);
}

RpMeshHeader* RpBuildMeshGenerateExhaustiveIgnoreWindingTriStrip(RpBuildMesh* buildmesh, void* data) {
    return ((RpMeshHeader*(__cdecl *)(RpBuildMesh*, void*))0x75B4E0)(buildmesh, data);
}

RwBool RpMeshSetTriStripMethod(RpTriStripMeshCallBack callback, void* data) {
    return ((RwBool(__cdecl *)(RpTriStripMeshCallBack, void*))0x75D4C0)(callback, data);
}

RwBool RpMeshGetTriStripMethod(RpTriStripMeshCallBack* callback, void* data) {
    return ((RwBool(__cdecl *)(RpTriStripMeshCallBack*, void*))0x75D500)(callback, data);
}

RpMeshHeader* _rpTriListMeshGenerate(RpBuildMesh* buildMesh, void* data) {
    return ((RpMeshHeader*(__cdecl *)(RpBuildMesh*, void*))0x75D530)(buildMesh, data);
}

RpMeshHeader* _rpMeshOptimise(RpBuildMesh* buildmesh, RwUInt32 flags) {
    return ((RpMeshHeader*(__cdecl *)(RpBuildMesh*, RwUInt32))0x75D970)(buildmesh, flags);
}

RpGeometry* RpGeometryTransform(RpGeometry* geometry, const RwMatrix* matrix) {
    return ((RpGeometry*(__cdecl *)(RpGeometry*, const RwMatrix*))0x74BFE0)(geometry, matrix);
}

RpGeometry* RpGeometryCreateSpace(RwReal radius) {
    return ((RpGeometry*(__cdecl *)(RwReal))0x74C130)(radius);
}

// 0x74C200
// Esfera envolvente de un morph target. RwBBoxCalculate (0x808F60) da la caja de los vertices; el centro es (sup + inf) * 0.5 por eje (float de 24 bits en el EXE, igual que float aqui);
// el radio al cuadrado es el maximo de (dx*dx + dy*dy) + dz*dz sobre los vertices (la actualizacion es `>` estricto: con NaN no cambia); si es > 0 se sustituye por RwSqrt (0x7EDB30,
// por direccion) y el radio final es ese valor por 1.001 (constante 0x872490). Escribe centro x, y, z y radio en ese orden y devuelve el morph target.
const RpMorphTarget* RpMorphTargetCalcBoundingSphere(const RpMorphTarget* morphTarget, RwSphere* boundingSphere) {
    RwBBox bbox;
    RwReal radiusSq = 0.0f;
    const RwUInt32 numVertices = static_cast<RwUInt32>(morphTarget->parentGeom->numVertices);
    const RwV3d* const verts = morphTarget->verts;
    RwBBoxCalculate(&bbox, verts, static_cast<RwInt32>(numVertices));

    const RwReal sumX = bbox.sup.x + bbox.inf.x;
    const RwReal sumY = bbox.sup.y + bbox.inf.y;
    const RwReal sumZ = bbox.sup.z + bbox.inf.z;
    RwV3d center;
    center.x = sumX * 0.5f;
    center.y = sumY * 0.5f;
    center.z = sumZ * 0.5f; // el EXE guarda la suma de z (fstp) y la recarga antes de multiplicar; con 24 bits no cambia el valor

    if (numVertices != 0) {
        RwUInt32 remaining = numVertices;
        const RwV3d* vertex = verts;
        do {
            const RwReal dx = vertex->x - center.x;
            const RwReal dy = vertex->y - center.y;
            const RwReal dz = vertex->z - center.z;
            const RwReal distanceSq = (dx * dx + dy * dy) + dz * dz;
            if (distanceSq > radiusSq) {
                radiusSq = distanceSq;
            }
            ++vertex;
        } while (--remaining != 0);
    }
    if (radiusSq > 0.0f) {
        radiusSq = RwSqrtFromExe(radiusSq);
    }
    boundingSphere->center.x = center.x;
    boundingSphere->center.y = center.y;
    boundingSphere->center.z = center.z;
    boundingSphere->radius = radiusSq * 1.001f;
    return morphTarget;
}

// 0x74C310
// Reserva `mtcount` morph targets mas. Tamano de cada uno: 0x1C de cabecera + (numVertices * 12 de posiciones) + otro tanto de normales si el geometry tiene rpGEOMETRYNORMALS; con
// rpGEOMETRYNATIVE solo la cabecera. Un bloque nuevo se pide con RwMalloc y uno existente con RwRealloc (pista 0x3000F); si falla pone el error 0x80000013 (con el tamano) y devuelve -1.
// Tras un realloc, los datos de vertices existentes (bytes tras las cabeceras) se desplazan hacia arriba mtcount * 0x1C bytes, copiando hacia atras byte a byte. Despues reparte los punteros
// de verts/normales de todos los morph targets y deja los nuevos con esfera a cero y parentGeom = geometry. Devuelve numMorphTargets - mtcount.
RwInt32 RpGeometryAddMorphTargets(RpGeometry* geometry, RwInt32 mtcount) {
    const RwUInt32 geometryFlags = geometry->flags;
    RwInt32 morphTargetSize;
    if (geometryFlags & rpGEOMETRYNATIVE) {
        morphTargetSize = 0x1C;
    } else {
        const RwInt32 vertexBytes = geometry->numVertices * 12;
        morphTargetSize = vertexBytes + 0x1C;
        if (geometryFlags & rpGEOMETRYNORMALS) {
            morphTargetSize += vertexBytes;
        }
    }
    const RwInt32 totalSize = (geometry->numMorphTargets + mtcount) * morphTargetSize;
    RpMorphTarget* const oldBlock = geometry->morphTarget;
    RpMorphTarget* block;
    if (oldBlock) {
        block = static_cast<RpMorphTarget*>(RwRealloc(oldBlock, totalSize, kGeometryMemoryHint));
    } else {
        block = static_cast<RpMorphTarget*>(RwMalloc(totalSize, kGeometryMemoryHint));
    }
    if (!block) {
        RaiseRwError(kErrorNoMemory, static_cast<RwUInt32>(totalSize));
        return -1;
    }
    if (oldBlock) {
        const RwInt32 oldCount = geometry->numMorphTargets;
        RwInt32 dataBytes = (morphTargetSize - 0x1C) * oldCount;
        uint8_t* source = reinterpret_cast<uint8_t*>(block) + oldCount * morphTargetSize - 1;
        uint8_t* destination = source + mtcount * 0x1C;
        while (dataBytes != 0) {
            *destination = *source;
            --destination;
            --source;
            --dataBytes;
        }
    }

    const RwInt32 newCount = geometry->numMorphTargets + mtcount;
    geometry->numMorphTargets = newCount;
    geometry->morphTarget = block;
    uint8_t* dataCursor = reinterpret_cast<uint8_t*>(block) + newCount * 0x1C;
    if (newCount > 0) {
        RwInt32 index = 0;
        do {
            RpMorphTarget* const target = &geometry->morphTarget[index];
            target->verts = nullptr;
            target->normals = nullptr;
            if (!(geometry->flags & rpGEOMETRYNATIVE) && geometry->numVertices != 0) {
                target->verts = reinterpret_cast<RwV3d*>(dataCursor);
                dataCursor += geometry->numVertices * 12;
                if (geometry->flags & rpGEOMETRYNORMALS) {
                    target->normals = reinterpret_cast<RwV3d*>(dataCursor);
                    dataCursor += geometry->numVertices * 12;
                }
            }
            ++index;
        } while (index < geometry->numMorphTargets);
    }

    RwInt32 first = geometry->numMorphTargets - mtcount;
    if (first < geometry->numMorphTargets) {
        do {
            RpMorphTarget* const target = &geometry->morphTarget[first];
            target->boundingSphere.center.x = 0.0f;
            target->boundingSphere.center.y = 0.0f;
            target->boundingSphere.center.z = 0.0f;
            target->boundingSphere.radius = 0.0f;
            target->parentGeom = geometry;
            ++first;
        } while (first < geometry->numMorphTargets);
    }
    return geometry->numMorphTargets - mtcount;
}

RwInt32 RpGeometryAddMorphTarget(RpGeometry* geometry) {
    return ((RwInt32(__cdecl *)(RpGeometry*))0x74C4D0)(geometry);
}

RpGeometry* RpGeometryRemoveMorphTarget(RpGeometry* geometry, RwInt32 morphTarget) {
    return ((RpGeometry*(__cdecl *)(RpGeometry*, RwInt32))0x74C4E0)(geometry, morphTarget);
}

// 0x74C690: escribe los tres indices (16 bits) en el orden 0, 1, 2 y devuelve el geometry.
const RpGeometry* RpGeometryTriangleSetVertexIndices(const RpGeometry* geometry, RpTriangle* triangle, RwUInt16 vert1, RwUInt16 vert2, RwUInt16 vert3) {
    triangle->vertIndex[0] = vert1;
    triangle->vertIndex[1] = vert2;
    triangle->vertIndex[2] = vert3;
    return geometry;
}

// 0x74C6C0
// Con material nulo pone matIndex = 0xFFFF. Si no, busca el material en la lista (_rpMaterialListFindMaterialIndex) y, si no esta (< 0), lo anade (_rpMaterialListAppendMaterial, 0x74E350;
// si falla con < 0 devuelve nulo sin tocar el triangulo); matIndex recibe los 16 bits bajos del indice. Devuelve el geometry.
RpGeometry* RpGeometryTriangleSetMaterial(RpGeometry* geometry, RpTriangle* triangle, RpMaterial* material) {
    if (!material) {
        triangle->matIndex = 0xFFFF;
        return geometry;
    }
    RwInt32 index = _rpMaterialListFindMaterialIndex(&geometry->matList, material);
    if (index < 0) {
        index = _rpMaterialListAppendMaterial(&geometry->matList, material);
        if (index < 0) {
            return nullptr;
        }
    }
    triangle->matIndex = static_cast<RwUInt16>(index);
    return geometry;
}

const RpGeometry* RpGeometryTriangleGetVertexIndices(const RpGeometry* geometry, const RpTriangle* triangle, RwUInt16* vert1, RwUInt16* vert2, RwUInt16* vert3) {
    return ((const RpGeometry*(__cdecl *)(const RpGeometry*, const RpTriangle*, RwUInt16*, RwUInt16*, RwUInt16*))0x74C720)(geometry, triangle, vert1, vert2, vert3);
}

// 0x74C760
RpMaterial* RpGeometryTriangleGetMaterial(const RpGeometry* geometry, const RpTriangle* triangle) {
    const RwUInt16 matIndex = triangle->matIndex;
    if (matIndex == 0xFFFFu) {
        return nullptr;
    }
    return geometry->matList.materials[matIndex];
}

// 0x74C790
// Como el original: el numero de materiales se lee una vez; el arreglo de punteros se relee en cada vuelta (el callback puede cambiarlo); un callback que devuelve nulo corta el recorrido.
RpGeometry* RpGeometryForAllMaterials(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* data) {
    static_assert(offsetof(RpGeometry, matList) + offsetof(RpMaterialList, materials) == 0x20 && offsetof(RpGeometry, matList) + offsetof(RpMaterialList, numMaterials) == 0x24);
    const RwInt32 numMaterials = geometry->matList.numMaterials;
    for (RwInt32 i = 0; i < numMaterials; ++i) {
        if (!fpCallBack(geometry->matList.materials[i], data)) {
            break;
        }
    }
    return geometry;
}

// 0x74C7D0
// lockedSinceLastInst |= (16 bits bajos de lockMode). Si el modo incluye rpGEOMETRYLOCKPOLYGONS (bit 0) y hay malla (+0x54) la destruye (_rpMeshDestroy, 0x758BC0) y la pone a nulo.
RpGeometry* RpGeometryLock(RpGeometry* geometry, RwInt32 lockMode) {
    geometry->lockedSinceLastInst |= static_cast<RwUInt16>(lockMode);
    if (lockMode & rpGEOMETRYLOCKPOLYGONS) {
        if (geometry->mesh) {
            _rpMeshDestroy(geometry->mesh);
            geometry->mesh = nullptr;
        }
    }
    return geometry;
}

// 0x74C800
// Si el geometry ya tiene malla devuelve el geometry sin mas. Si no: _rpBuildMeshCreate(numTriangles) (nulo -> nulo); con materiales (> 0) pide tres listas de numMateriales punteros
// (texturas con pista 0x10006, rasters y tuberias con 0x10503; sin comprobar nulo) y por cada triangulo busca (indices de 16 bits, busqueda lineal, alta al final) la textura del material,
// su raster (nulo sin textura) y su tuberia, y llama a _rpBuildMeshAddTriangle (0x758C00) con (malla, material, v0, v1, v2, matIndex, indiceTextura, indiceRaster, indiceTuberia); libera las
// tres listas (texturas, rasters, tuberias). Luego _rpMeshOptimise(malla, rpGEOMETRYTRISTRIP ? 1 : 0): con exito guarda la malla en el geometry; si no, _rpBuildMeshDestroy y nulo.
RpGeometry* RpGeometryUnlock(RpGeometry* geometry) {
    RwUInt32 numTextures = 0;
    RwUInt32 numRasters = 0;
    RwUInt32 numPipelines = 0;
    if (geometry->mesh) {
        return geometry;
    }
    RpBuildMesh* const buildMesh = _rpBuildMeshCreate(geometry->numTriangles);
    if (!buildMesh) {
        return nullptr;
    }
    if (geometry->matList.numMaterials > 0) {
        const RwUInt32 listBytes = static_cast<RwUInt32>(geometry->matList.numMaterials) * 4;
        void** const textures = static_cast<void**>(RwMalloc(listBytes, kGeometryTempListHint));
        void** const rasters = static_cast<void**>(RwMalloc(listBytes, kGeometryTempRasterHint));
        void** const pipelines = static_cast<void**>(RwMalloc(listBytes, kGeometryTempRasterHint));
        if (geometry->numTriangles > 0) {
            RwInt32 triangleIndex = 0;
            do {
                const RpTriangle* const triangle = &geometry->triangles[triangleIndex];
                const RwUInt16 matIndex = triangle->matIndex;
                RpMaterial* const material = _rpMaterialListGetMaterial(&geometry->matList, matIndex);
                RwTexture* const texture = material->texture;

                RwUInt16 textureIndex = 0;
                if (static_cast<RwUInt16>(numTextures) != 0) {
                    do {
                        if (textures[textureIndex] == texture) {
                            break;
                        }
                        ++textureIndex;
                    } while (textureIndex < static_cast<RwUInt16>(numTextures));
                }
                if (textureIndex == static_cast<RwUInt16>(numTextures)) {
                    textures[textureIndex] = texture;
                    numTextures = numTextures + 1;
                }

                RwRaster* const raster = texture ? texture->raster : nullptr;
                RwUInt16 rasterIndex = 0;
                if (static_cast<RwUInt16>(numRasters) != 0) {
                    do {
                        if (rasters[rasterIndex] == raster) {
                            break;
                        }
                        ++rasterIndex;
                    } while (rasterIndex < static_cast<RwUInt16>(numRasters));
                }
                if (rasterIndex == static_cast<RwUInt16>(numRasters)) {
                    rasters[rasterIndex] = raster;
                    numRasters = numRasters + 1;
                }

                RxPipeline* const pipeline = material->pipeline;
                RwUInt16 pipelineIndex = 0;
                if (static_cast<RwUInt16>(numPipelines) != 0) {
                    do {
                        if (pipelines[pipelineIndex] == pipeline) {
                            break;
                        }
                        ++pipelineIndex;
                    } while (pipelineIndex < static_cast<RwUInt16>(numPipelines));
                }
                if (pipelineIndex == static_cast<RwUInt16>(numPipelines)) {
                    pipelines[pipelineIndex] = pipeline;
                    numPipelines = numPipelines + 1;
                }

                _rpBuildMeshAddTriangle(buildMesh, material, triangle->vertIndex[0], triangle->vertIndex[1], triangle->vertIndex[2], triangle->matIndex, textureIndex, rasterIndex, pipelineIndex);
                ++triangleIndex;
            } while (triangleIndex < geometry->numTriangles);
        }
        RwFree(textures);
        RwFree(rasters);
        RwFree(pipelines);
    }
    RpMeshHeader* const mesh = _rpMeshOptimise(buildMesh, (geometry->flags & rpGEOMETRYTRISTRIP) ? 1 : 0);
    if (mesh) {
        geometry->mesh = mesh;
        return geometry;
    }
    _rpBuildMeshDestroy(buildMesh);
    return nullptr;
}

const RpGeometry* RpGeometryForAllMeshes(const RpGeometry* geometry, RpMeshCallBack fpCallBack, void* data) {
    return ((const RpGeometry*(__cdecl *)(const RpGeometry*, RpMeshCallBack, void*))0x74CA60)(geometry, fpCallBack, data);
}

// 0x74CA90
// numVert < 0 o numTriangles < 0 devuelven nulo en silencio; numVert >= 0x10000 pone el error literal 6 (pluginID 2) y devuelve nulo. Numero de juegos de coordenadas de textura: el byte 2 del
// formato si no es cero; si no, 2 con el bit 7 del byte bajo, y si no el bit 2 (rpGEOMETRYTEXTURED). El formato efectivo es (byte bajo & 0x7B) | (1 juego: 0x04; mas de uno: 0x80). Tamano del
// bloque: sizeOfStruct del registro de plugins (0x8D628C) y, si no es nativo, + 4 por vertice con prelit, + 8 por vertice y juego, + 8 por triangulo (RwMalloc, pista 0x3000F; sin memoria o
// lista de materiales sin inicializar -> nulo, el bloque no se libera). Deja el objeto con tipo 8, refCount 1, flags = (formato & 0x0F000000) | formato efectivo, punteros de datos repartidos
// tras la estructura (prelit, coordenadas, triangulos con matIndex = 0xFFFF) y un morph target (RpGeometryAddMorphTargets(geometry, 1); si falla se libera todo y devuelve nulo).
RpGeometry* RpGeometryCreate(RwInt32 numVert, RwInt32 numTriangles, RwUInt32 format) {
    if (numVert < 0) {
        return nullptr;
    }
    if (numVert >= 0x10000) {
        RaiseRwError(6);
        return nullptr;
    }
    if (numTriangles < 0) {
        return nullptr;
    }
    RwUInt32 allocSize = static_cast<RwUInt32>(geometryTKList.sizeOfStruct);
    RwUInt32 numTexCoordSets;
    if (format & 0xFF0000) {
        numTexCoordSets = (format >> 16) & 0xFF;
    } else if (static_cast<RwInt8>(format) < 0) {
        numTexCoordSets = 2;
    } else {
        numTexCoordSets = (format >> 2) & 1;
    }
    RwUInt32 texturedFlag;
    if (numTexCoordSets == 1) {
        texturedFlag = rpGEOMETRYTEXTURED;
    } else {
        texturedFlag = (1u < numTexCoordSets) ? rpGEOMETRYTEXTURED2 : 0u;
    }
    const RwUInt32 effectiveFormat = ((format & 0xFF) & 0x7B) | texturedFlag;
    const RwUInt32 nativeFormat = format & rpGEOMETRYNATIVE;
    if (!nativeFormat) {
        if (effectiveFormat & rpGEOMETRYPRELIT) {
            allocSize += static_cast<RwUInt32>(numVert) * 4;
        }
        if (numTexCoordSets > 0) {
            allocSize += numTexCoordSets * static_cast<RwUInt32>(numVert) * 8;
        }
        allocSize += static_cast<RwUInt32>(numTriangles) * 8;
    }
    RpGeometry* const geometry = static_cast<RpGeometry*>(RwMalloc(allocSize, kGeometryMemoryHint));
    if (!geometry) {
        return nullptr;
    }
    if (!_rpMaterialListInitialize(&geometry->matList)) {
        return nullptr;
    }
    geometry->morphTarget = nullptr;
    geometry->numMorphTargets = 0;
    geometry->object.parent = nullptr;
    geometry->repEntry = nullptr;
    geometry->lockedSinceLastInst = 0;
    geometry->mesh = nullptr;
    geometry->object.type = 8;
    geometry->object.subType = 0;
    geometry->object.flags = 0;
    geometry->object.privateFlags = 0;
    geometry->refCount = 1;
    geometry->numTexCoordSets = static_cast<RwInt32>(numTexCoordSets);
    for (RwInt32 i = 0; i < rwMAXTEXTURECOORDS; ++i) {
        geometry->texCoords[i] = nullptr;
    }
    geometry->preLitLum = nullptr;
    geometry->triangles = nullptr;
    geometry->numTriangles = numTriangles;
    geometry->flags = (format & rpGEOMETRYNATIVEFLAGSMASK) | effectiveFormat;
    geometry->numVertices = numVert;
    if (!nativeFormat) {
        uint8_t* cursor = reinterpret_cast<uint8_t*>(geometry) + geometryTKList.sizeOfStruct;
        if ((effectiveFormat & rpGEOMETRYPRELIT) && numVert != 0) {
            geometry->preLitLum = reinterpret_cast<RwRGBA*>(cursor);
            cursor += static_cast<RwUInt32>(numVert) * 4;
        }
        if (numTexCoordSets > 0 && numVert != 0) {
            // Sin limite de 8 como el EXE (bucle por puntero sobre texCoords[0] y siguientes).
            void** slot = reinterpret_cast<void**>(&geometry->texCoords[0]);
            for (RwUInt32 n = numTexCoordSets; n != 0; --n) {
                *slot = cursor;
                cursor += static_cast<RwUInt32>(numVert) * 8;
                ++slot;
            }
        }
        if (numTriangles != 0) {
            geometry->triangles = reinterpret_cast<RpTriangle*>(cursor);
            for (RwInt32 i = 0; i < numTriangles; ++i) {
                geometry->triangles[i].matIndex = 0xFFFF;
            }
        }
    }
    if (RpGeometryAddMorphTargets(geometry, 1) < 0) {
        _rpMaterialListDeinitialize(&geometry->matList);
        RwFree(geometry);
        return nullptr;
    }
    _rwPluginRegistryInitObject(&geometryTKList, geometry);
    return geometry;
}

// 0x74CCB0: suma una referencia (refCount de 16 bits en +0x0E) y devuelve el geometry.
RpGeometry* RpGeometryAddRef(RpGeometry* geometry) {
    ++geometry->refCount;
    return geometry;
}

// 0x74CCC0
// Con refCount > 1 (con signo) solo resta uno y devuelve TRUE. Si no: libera la entrada de recursos (RwResourcesFreeResEntry, si hay), marca lockedSinceLastInst |= 0xFFF, destruye la malla
// (_rpMeshDestroy) si la hay, DeInitObject del registro de geometrias (0x8D628C), libera el bloque de morph targets, deinicializa la lista de materiales, resta uno al refCount y libera el
// geometry. Devuelve TRUE.
RwBool RpGeometryDestroy(RpGeometry* geometry) {
    const RwInt16 refCount = geometry->refCount;
    if (static_cast<RwInt32>(refCount) - 1 > 0) {
        geometry->refCount = static_cast<RwInt16>(refCount - 1);
        return TRUE;
    }
    if (geometry->repEntry) {
        RwResourcesFreeResEntry(geometry->repEntry);
    }
    RpMeshHeader* const mesh = geometry->mesh;
    const RwInt16 refCountAgain = geometry->refCount;
    geometry->lockedSinceLastInst |= rpGEOMETRYLOCKALL;
    geometry->refCount = refCountAgain;
    if (mesh) {
        _rpMeshDestroy(mesh);
        geometry->mesh = nullptr;
    }
    _rwPluginRegistryDeInitObject(&geometryTKList, geometry);
    if (geometry->morphTarget) {
        RwFree(geometry->morphTarget);
        geometry->morphTarget = nullptr;
    }
    _rpMaterialListDeinitialize(&geometry->matList);
    --geometry->refCount;
    RwFree(geometry);
    return TRUE;
}

// 0x74CD70: _rwPluginRegistryAddPlugin sobre el registro de geometrias (0x8D628C).
RwInt32 RpGeometryRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return _rwPluginRegistryAddPlugin(&geometryTKList, size, pluginID, constructCB, destructCB, copyCB);
}

// 0x74CDA0: _rwPluginRegistryAddPluginStream sobre el registro de geometrias.
RwInt32 RpGeometryRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return _rwPluginRegistryAddPluginStream(&geometryTKList, pluginID, readCB, writeCB, getSizeCB);
}

RwInt32 RpGeometrySetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkAlwaysCallBack))0x74CDD0)(pluginID, alwaysCB);
}

RwInt32 RpGeometryGetPluginOffset(RwUInt32 pluginID) {
    return ((RwInt32(__cdecl *)(RwUInt32))0x74CDF0)(pluginID);
}

RwBool RpGeometryValidatePlugins(const RpGeometry* geometry) {
    return ((RwBool(__cdecl *)(const RpGeometry*))0x74CE10)(geometry);
}

RwUInt32 RpGeometryStreamGetSize(const RpGeometry* geometry) {
    return ((RwUInt32(__cdecl *)(const RpGeometry*))0x74CE20)(geometry);
}

const RpGeometry* RpGeometryStreamWrite(const RpGeometry* geometry, RwStream* stream) {
    return ((const RpGeometry*(__cdecl *)(const RpGeometry*, RwStream*))0x74CED0)(geometry, stream);
}

// 0x74D6D0
// Cola de destruccion de un geometry ya sin referencias: suma uno al refCount, marca lockedSinceLastInst |= 0xFFF, destruye la malla (si la hay; el puntero se lee antes de sumar), DeInitObject
// del registro de geometrias, libera el bloque de morph targets, deinicializa la lista de materiales, resta uno y libera el geometry. Devuelve TRUE. No toca la entrada de recursos.
RwBool _rpGeometryDestroyUnreferenced(RpGeometry* geometry) {
    RpMeshHeader* const mesh = geometry->mesh;
    ++geometry->refCount;
    geometry->lockedSinceLastInst |= rpGEOMETRYLOCKALL;
    if (mesh) {
        _rpMeshDestroy(mesh);
        geometry->mesh = nullptr;
    }
    _rwPluginRegistryDeInitObject(&geometryTKList, geometry);
    if (geometry->morphTarget) {
        RwFree(geometry->morphTarget);
        geometry->morphTarget = nullptr;
    }
    _rpMaterialListDeinitialize(&geometry->matList);
    --geometry->refCount;
    RwFree(geometry);
    return TRUE;
}

namespace {
// Fallo de lectura de los datos del geometry (0x74D522 / 0x74D3AC / 0x74D600): con refCount > 1 (con signo) solo resta uno; si no, libera la entrada de recursos, RpGeometryLock(todo),
// DeInitObject, libera los morph targets, deinicializa la lista de materiales, resta uno y libera el geometry.
void GeometryStreamReadAbort(RpGeometry* geometry) {
    const RwInt16 refCount = geometry->refCount;
    if (static_cast<RwInt32>(refCount) - 1 > 0) {
        geometry->refCount = static_cast<RwInt16>(refCount - 1);
        return;
    }
    if (geometry->repEntry) {
        RwResourcesFreeResEntry(geometry->repEntry);
    }
    const RwInt16 refCountAgain = geometry->refCount;
    geometry->refCount = refCountAgain;
    RpGeometryLock(geometry, rpGEOMETRYLOCKALL);
    _rwPluginRegistryDeInitObject(&geometryTKList, geometry);
    if (geometry->morphTarget) {
        RwFree(geometry->morphTarget);
        geometry->morphTarget = nullptr;
    }
    _rpMaterialListDeinitialize(&geometry->matList);
    --geometry->refCount;
    RwFree(geometry);
}

// Fallo tras leer los datos (lista de materiales, plugins o RpGeometryUnlock; 0x74D5B4 / 0x74D5BE): con refCount > 1 solo resta uno; si no, libera la entrada de recursos, resta uno y
// llama a 0x74D6D0 (que lo suma otra vez antes de destruir).
void GeometryStreamReadRelease(RpGeometry* geometry) {
    const RwInt16 refCount = geometry->refCount;
    if (static_cast<RwInt32>(refCount) - 1 > 0) {
        geometry->refCount = static_cast<RwInt16>(refCount - 1);
        return;
    }
    if (geometry->repEntry) {
        RwResourcesFreeResEntry(geometry->repEntry);
    }
    --geometry->refCount;
    _rpGeometryDestroyUnreferenced(geometry);
}

// Cabecera por morph target del flujo: esfera (16 bytes) y dos indicadores de si siguen vertices y normales.
struct GeometryMorphTargetChunk {
    RwSphere sphere;
    RwInt32  hasVertices;
    RwInt32  hasNormals;
};
static_assert(sizeof(GeometryMorphTargetChunk) == 0x18);
}

// 0x74D190
// Lee un RpGeometry. Chunk STRUCT con version 0x34000..0x36003 (si no, error 0x80000004 y nulo); 16 bytes de RpGeometryChunkInfo (y, solo con version 0x34000, 12 bytes de propiedades de
// superficie; cualquier lectura corta devuelve nulo sin error). RpGeometryCreate(numVertices, numTriangles, formato) y, con mas de un morph target, RpGeometryAddMorphTargets(n - 1).
// Si no es nativo y tiene vertices: prelit (si el formato leido tiene el bit 8; 4 bytes por vertice), coordenadas (RwStreamReadReal, 8 bytes por vertice y juego) y triangulos (8 bytes cada
// uno, con las palabras reordenadas (w0, w1, w2, w3) -> (w1, w0, w3, w2)). Luego por cada morph target: 24 bytes de cabecera (esfera + hasVertices + hasNormals), y vertices y normales:
// con ambos se lee un solo bloque de numVertices * 24 bytes sobre `verts`, solo vertices numVertices * 12, solo normales numVertices * 12 sobre `normals`. Despues el chunk MATLIST (8; sin
// encontrarlo devuelve nulo sin liberar nada), con version 0x34000 las propiedades por defecto de materiales, _rpMaterialListStreamRead, los datos de plugins y RpGeometryUnlock.
RpGeometry* RpGeometryStreamRead(RwStream* stream) {
    RwUInt32 version;
    RpGeometryChunkInfo chunkInfo;
    RwSurfaceProperties surfaceProps;

    if (!RwStreamFindChunk(stream, rwID_STRUCT, nullptr, &version)) {
        return nullptr;
    }
    if (version < kStreamVersionMin || version > kStreamVersionMax) {
        RaiseRwError(kErrorBadVersion);
        return nullptr;
    }
    if (version < kStreamVersionMin + 1) {
        if (RwStreamRead(stream, &chunkInfo, 0x10) != 0x10) {
            return nullptr;
        }
        if (RwStreamRead(stream, &surfaceProps, 0xC) != 0xC) {
            return nullptr;
        }
    } else if (RwStreamRead(stream, &chunkInfo, 0x10) != 0x10) {
        return nullptr;
    }

    RpGeometry* const geometry = RpGeometryCreate(chunkInfo.numVertices, chunkInfo.numTriangles, chunkInfo.format);
    if (!geometry) {
        return nullptr;
    }
    if (chunkInfo.numMorphTargets > 1) {
        if (RpGeometryAddMorphTargets(geometry, chunkInfo.numMorphTargets - 1) < 0) {
            GeometryStreamReadAbort(geometry);
            return nullptr;
        }
    }

    if (!(geometry->flags & rpGEOMETRYNATIVE) && geometry->numVertices != 0) {
        if (chunkInfo.format & rpGEOMETRYPRELIT) {
            const RwUInt32 preLitBytes = static_cast<RwUInt32>(geometry->numVertices) * 4;
            if (RwStreamRead(stream, geometry->preLitLum, preLitBytes) != preLitBytes) {
                GeometryStreamReadAbort(geometry);
                return nullptr;
            }
        }
        if (geometry->numTexCoordSets > 0) {
            const RwUInt32 texCoordBytes = static_cast<RwUInt32>(geometry->numVertices) * 8;
            for (RwInt32 set = 0; set < geometry->numTexCoordSets; ++set) {
                if (!RwStreamReadReal(stream, reinterpret_cast<RwReal*>(geometry->texCoords[set]), texCoordBytes)) {
                    GeometryStreamReadAbort(geometry);
                    return nullptr;
                }
            }
        }
        if (geometry->numTriangles != 0) {
            const RwUInt32 triangleBytes = static_cast<RwUInt32>(geometry->numTriangles) * 8;
            if (RwStreamRead(stream, geometry->triangles, triangleBytes) != triangleBytes) {
                GeometryStreamReadAbort(geometry);
                return nullptr;
            }
            RwUInt16* word = reinterpret_cast<RwUInt16*>(geometry->triangles);
            RwUInt32 remaining = static_cast<RwUInt32>(geometry->numTriangles);
            do {
                const RwUInt16 first = word[0];
                word[0] = word[1];
                const RwUInt16 fourth = word[3];
                word[1] = first;
                const RwUInt16 third = word[2];
                word[2] = fourth;
                word[3] = third;
                word += 4;
            } while (--remaining != 0);
        }
    }

    if (geometry->numMorphTargets > 0) {
        RwInt32 index = 0;
        do {
            RpMorphTarget* const target = &geometry->morphTarget[index];
            GeometryMorphTargetChunk morphChunk;
            if (RwStreamRead(stream, &morphChunk, sizeof(morphChunk)) != sizeof(morphChunk)) {
                GeometryStreamReadAbort(geometry);
                return nullptr;
            }
            target->boundingSphere.center.x = morphChunk.sphere.center.x;
            target->boundingSphere.center.y = morphChunk.sphere.center.y;
            target->boundingSphere.center.z = morphChunk.sphere.center.z;
            target->boundingSphere.radius = morphChunk.sphere.radius;
            if (morphChunk.hasVertices) {
                const RwUInt32 vertexBytes = static_cast<RwUInt32>(geometry->numVertices) * 12;
                const RwUInt32 bytes = morphChunk.hasNormals ? vertexBytes * 2 : vertexBytes;
                if (!RwStreamReadReal(stream, reinterpret_cast<RwReal*>(target->verts), bytes)) {
                    GeometryStreamReadAbort(geometry);
                    return nullptr;
                }
            } else if (morphChunk.hasNormals) {
                const RwUInt32 vertexBytes = static_cast<RwUInt32>(geometry->numVertices) * 12;
                if (!RwStreamReadReal(stream, reinterpret_cast<RwReal*>(target->normals), vertexBytes)) {
                    GeometryStreamReadAbort(geometry);
                    return nullptr;
                }
            }
            ++index;
        } while (index < geometry->numMorphTargets);
    }

    if (!RwStreamFindChunk(stream, rwID_MATLIST, nullptr, &version)) {
        return nullptr;
    }
    if (version < kStreamVersionMin || version > kStreamVersionMax) {
        GeometryStreamReadAbort(geometry);
        RaiseRwError(kErrorBadVersion);
        return nullptr;
    }
    if (version < kStreamVersionMin + 1) {
        _rpMaterialSetDefaultSurfaceProperties(&surfaceProps);
    }
    if (!_rpMaterialListStreamRead(stream, &geometry->matList)) {
        GeometryStreamReadRelease(geometry);
        return nullptr;
    }
    if (version < kStreamVersionMin + 1) {
        _rpMaterialSetDefaultSurfaceProperties(nullptr);
    }
    if (!_rwPluginRegistryReadDataChunks(&geometryTKList, stream, geometry)) {
        GeometryStreamReadRelease(geometry);
        return nullptr;
    }
    if (!RpGeometryUnlock(geometry)) {
        GeometryStreamReadRelease(geometry);
        return nullptr;
    }
    return geometry;
}

RpGeometryChunkInfo* _rpGeometryChunkInfoRead(RwStream* stream, RpGeometryChunkInfo* geometryChunkInfo, RwInt32* bytesRead) {
    return ((RpGeometryChunkInfo*(__cdecl *)(RwStream*, RpGeometryChunkInfo*, RwInt32*))0x74D750)(stream, geometryChunkInfo, bytesRead);
}

RpWorldSector* RpWorldSectorRender(RpWorldSector* worldSector) {
    return ((RpWorldSector*(__cdecl *)(RpWorldSector*))0x761C50)(worldSector);
}

const RpWorldSector* RpWorldSectorForAllMeshes(const RpWorldSector* sector, RpMeshCallBack fpCallBack, void* data) {
    return ((const RpWorldSector*(__cdecl *)(const RpWorldSector*, RpMeshCallBack, void*))0x761C60)(sector, fpCallBack, data);
}

// 0x761C90: registra el plugin en la lista de plugins de RpWorldSector (0x8D736C) con _rwPluginRegistryAddPlugin (0x8084A0).
RwInt32 RpWorldSectorRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return _rwPluginRegistryAddPlugin(reinterpret_cast<RwPluginRegistry*>(kWorldSectorPluginRegistry), size, pluginID, constructCB, destructCB, copyCB);
}

// 0x761CC0: igual para los callbacks de flujo (_rwPluginRegistryAddPluginStream, 0x8088E0) sobre la lista de RpWorldSector.
RwInt32 RpWorldSectorRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return _rwPluginRegistryAddPluginStream(reinterpret_cast<RwPluginRegistry*>(kWorldSectorPluginRegistry), pluginID, readCB, writeCB, getSizeCB);
}

RwInt32 RpWorldSectorSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkAlwaysCallBack))0x761CF0)(pluginID, alwaysCB);
}

RwInt32 RpWorldSectorSetStreamRightsCallBack(RwUInt32 pluginID, RwPluginDataChunkRightsCallBack rightsCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkRightsCallBack))0x761D10)(pluginID, rightsCB);
}

RwInt32 RpWorldSectorGetPluginOffset(RwUInt32 pluginID) {
    return ((RwInt32(__cdecl *)(RwUInt32))0x761D30)(pluginID);
}

RwBool RpWorldSectorValidatePlugins(const RpWorldSector* sector) {
    return ((RwBool(__cdecl *)(const RpWorldSector*))0x761D50)(sector);
}

void RpLightSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x752250)(blockSize, numBlocksToPrealloc);
}

RpLight* RpLightCreate(RwInt32 type) {
    return ((RpLight*(__cdecl *)(RwInt32))0x752110)(type);
}

RwBool RpLightDestroy(RpLight* light) {
    return ((RwBool(__cdecl *)(RpLight*))0x7520D0)(light);
}

RpLight* RpLightSetRadius(RpLight* light, RwReal radius) {
    return ((RpLight*(__cdecl *)(RpLight*, RwReal))0x751A70)(light, radius);
}

RpLight* RpLightSetColor(RpLight* light, const RwRGBAReal* color) {
    return ((RpLight*(__cdecl *)(RpLight*, const RwRGBAReal*))0x751A90)(light, color);
}

RwReal RpLightGetConeAngle(const RpLight* light) {
    return ((RwReal(__cdecl *)(const RpLight*))0x751AE0)(light);
}

RpLight* RpLightSetConeAngle(RpLight* ight, RwReal angle) {
    return ((RpLight*(__cdecl *)(RpLight*, RwReal))0x751D20)(ight, angle);
}

RwUInt32 RpLightStreamGetSize(const RpLight* light) {
    return ((RwUInt32(__cdecl *)(const RpLight*))0x751E10)(light);
}

RpLight* RpLightStreamRead(RwStream* stream) {
    return ((RpLight*(__cdecl *)(RwStream*))0x751F00)(stream);
}

RpLightChunkInfo* _rpLightChunkInfoRead(RwStream* stream, RpLightChunkInfo* lightChunkInfo, RwInt32* bytesRead) {
    return ((RpLightChunkInfo*(__cdecl *)(RwStream*, RpLightChunkInfo*, RwInt32*))0x752060)(stream, lightChunkInfo, bytesRead);
}

// 0x751D60
RwInt32 RpLightRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return _rwPluginRegistryAddPlugin(&s_lightTKList, size, pluginID, constructCB, destructCB, copyCB);
}

RwInt32 RpLightRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkReadCallBack, RwPluginDataChunkWriteCallBack, RwPluginDataChunkGetSizeCallBack))0x751D90)(pluginID, readCB, writeCB, getSizeCB);
}

RwInt32 RpLightSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkAlwaysCallBack))0x751DC0)(pluginID, alwaysCB);
}

RwInt32 RpLightGetPluginOffset(RwUInt32 pluginID) {
    return ((RwInt32(__cdecl *)(RwUInt32))0x751DE0)(pluginID);
}

RwBool RpLightValidatePlugins(const RpLight* light) {
    return ((RwBool(__cdecl *)(const RpLight*))0x751E00)(light);
}

void RpD3D9LightSetAttenuationParams(RpLight* light, const RpD3D9AttenuationParams* params) {
    ((void(__cdecl *)(RpLight*, const RpD3D9AttenuationParams*))0x755D20)(light, params);
}

void RpD3D9LightGetAttenuationParams(const RpLight* light, RpD3D9AttenuationParams* params) {
    ((void(__cdecl *)(const RpLight*, RpD3D9AttenuationParams*))0x755D50)(light, params);
}

RwBool _rwD3D9LightsOpen(void) {
    return ((RwBool(__cdecl *)(void))0x755D80)();
}

RwBool _rwD3D9LightsGlobalEnable(RpLightFlag flags) {
    return ((RwBool(__cdecl *)(RpLightFlag))0x756070)(flags);
}

RwBool _rwD3D9LightDirectionalEnable(RpLight* light) {
    return ((RwBool(__cdecl *)(RpLight*))0x756260)(light);
}

RwBool _rwD3D9LightLocalEnable(RpLight* light) {
    return ((RwBool(__cdecl *)(RpLight*))0x756390)(light);
}

void _rwD3D9LightsEnable(RwBool enable, RwUInt32 type) {
    ((void(__cdecl *)(RwBool, RwUInt32))0x756600)(enable, type);
}

void _rwD3D9LightsClose(void) {
    ((void(__cdecl *)(void))0x755FE0)();
}

void RpAtomicSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x749720)(blockSize, numBlocksToPrealloc);
}

void RpClumpSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x749740)(blockSize, numBlocksToPrealloc);
}

// 0x7491C0
// Como el original: sin tuberia propia usa la por defecto del motor (campo +0x3C del bloque de plugin, `[0xC9BC60] + [0xC97B24] + 0x3C`) y ejecuta RxPipelineExecute (0x805710) con (tuberia, atomico, 1); devuelve el atomico si el resultado no es nulo.
RpAtomic* AtomicDefaultRenderCallBack(RpAtomic* atomic) {
    static_assert(offsetof(RpAtomic, pipeline) == 0x6C);
    RxPipeline* pipeline = atomic->pipeline;
    if (!pipeline) {
        pipeline = *reinterpret_cast<RxPipeline**>(StaticRef<uint8*>(0xC9BC60) + StaticRef<int32>(0xC97B24) + 0x3C);
    }
    return RxPipelineExecute(pipeline, atomic, TRUE) ? atomic : nullptr;
}

// Lote 226A (sin validar): estado compartido de las funciones de atomos y clumps de abajo.
static auto& s_clumpTKList             = StaticRef<RwPluginRegistry>(0x8D6264); // registro de plugins de RpClump
static auto& s_engineFreeListsOffset   = StaticRef<RwInt32>(0xC924AC);          // desplazamiento de las listas libres de atomos (+0) y clumps (+4) dentro de RwEngineInstance
static auto& s_cameraClumpLinkOffset   = StaticRef<RwInt32>(0xC924D8);          // plugin de RwCamera: {clump, enlace} (el enlace en +4); escrito en 0x749848
static auto& s_lightClumpLinkOffset    = StaticRef<RwInt32>(0xC924DC);          // plugin de RpLight: igual; escrito en 0x749873
static auto& s_rightsReadWord0         = StaticRef<RwInt32>(0xC924A8);          // destino de la primera lectura de AtomicRightsStreamRead
static auto& s_rightsReadWord1         = StaticRef<RwInt32>(0xC92480);          // destino de la segunda lectura

static_assert(offsetof(RwGlobals, memoryAlloc) == 0x144 && offsetof(RwGlobals, memoryFree) == 0x148);
static_assert(offsetof(RpAtomic, interpolator) == 0x4C && offsetof(RpAtomic, repEntry) == 0x14 && offsetof(RpAtomic, geometry) == 0x18);
static_assert(offsetof(RpAtomic, boundingSphere) == 0x1C && offsetof(RpAtomic, worldBoundingSphere) == 0x2C && offsetof(RpAtomic, clump) == 0x3C);
static_assert(offsetof(RpAtomic, renderCallBack) == 0x48 && offsetof(RpAtomic, renderFrame) == 0x60 && offsetof(RpAtomic, llWorldSectorsInAtomic) == 0x64);
static_assert(offsetof(RpGeometry, numMorphTargets) == 0x18 && offsetof(RpGeometry, morphTarget) == 0x5C && sizeof(RpMorphTarget) == 0x1C);
static_assert(offsetof(RpClump, atomicList) == 0x8 && offsetof(RpClump, lightList) == 0x10 && offsetof(RpClump, cameraList) == 0x18);
static_assert(offsetof(RxPipeline, pluginId) == 0x2C && offsetof(RxPipeline, pluginData) == 0x30);

// Lista libre de atomos (slot 0) o de clumps (slot 1) que cuelga de RwEngineInstance; el desplazamiento se lee en cada uso, como el EXE.
static RwFreeList* EngineFreeList(int32 slot) {
    return *reinterpret_cast<RwFreeList**>(reinterpret_cast<uint8*>(RwEngineInstance) + s_engineFreeListsOffset + slot * 4);
}

// 0x74CCB0 (RpGeometryAddRef: solo incrementa refCount): se llama por su direccion, como el EXE.
static RpGeometry* GeometryAddRef(RpGeometry* geometry) {
    return reinterpret_cast<RpGeometry*(__cdecl*)(RpGeometry*)>(0x74CCB0)(geometry);
}

// Parte de AtomicDestroyBody: suelta la geometria; si el atomo tiene marco y esta en un mundo, actualiza los objetos del marco.
static void AtomicReleaseGeometry(RpAtomic* atomic) {
    RpGeometryDestroy(atomic->geometry);
    RwFrame* const frame = static_cast<RwFrame*>(atomic->object.object.parent);
    atomic->geometry = nullptr;
    if (frame && RpAtomicGetWorld(atomic)) {
        RwFrameUpdateObjects(frame);
    }
}

// Cuerpo comun de RpAtomicDestroy (0x749DC0) y AtomicFreeListDestroyCallBack (0x7496A0) y del bucle de ClumpFreeListDestroyCallBack (0x749520), sin devolver nada:
// destruye los datos de plugin, la entrada de recursos y la geometria, suelta el marco y devuelve el atomo a su lista libre.
static void AtomicDestroyBody(RpAtomic* atomic) {
    _rwPluginRegistryDeInitObject(&s_atomicTKList, atomic);
    if (atomic->repEntry) {
        RwResourcesFreeResEntry(atomic->repEntry);
    }
    if (atomic->geometry) {
        AtomicReleaseGeometry(atomic);
    }
    _rwObjectHasFrameReleaseFrame(atomic);
    RwFreeListFree(EngineFreeList(0), atomic);
}

// 0x7491F0
// Recalcula la esfera envolvente del atomo interpolando las de dos morph targets (centro y radio) y limpia el bit 1 del interpolador; privateFlags |= 1.
// Como el EXE: x87 a 24 bits, es decir cada operacion se redondea a float; orden: s = posicion * recipTime; radio = (fin - inicio) * s + inicio; centro igual por componentes
// ((fin - inicio) se guarda como float, se multiplica por s, se suma al inicio). Sin geometria no hace nada. Con indices iguales o fuera de rango copia la esfera sin interpolar
// (la del target de inicio si los dos indices son validos con signo, si no la del target 0).
void _rpAtomicResyncInterpolatedSphere(RpAtomic* atomic) {
    RpGeometry* const geometry = atomic->geometry;
    if (!geometry) {
        return;
    }
    const int16 startIndex = atomic->interpolator.startMorphTarget;
    const int16 endIndex   = atomic->interpolator.endMorphTarget;
    const int32 numMorphTargets = geometry->numMorphTargets;
    if (startIndex != endIndex && startIndex < numMorphTargets && endIndex < numMorphTargets) {
        RpMorphTarget* const morphTargets = geometry->morphTarget;
        const RwSphere& start = morphTargets[startIndex].boundingSphere;
        const RwSphere& end   = morphTargets[endIndex].boundingSphere;
        const float scale = atomic->interpolator.position * atomic->interpolator.recipTime;
        atomic->boundingSphere.radius = (end.radius - start.radius) * scale + start.radius;
        atomic->boundingSphere.center.x = end.center.x - start.center.x;
        atomic->boundingSphere.center.y = end.center.y - start.center.y;
        atomic->boundingSphere.center.z = end.center.z - start.center.z;
        const float scaledX = atomic->boundingSphere.center.x * scale;
        atomic->boundingSphere.center.x = scaledX;
        const float scaledY = atomic->boundingSphere.center.y * scale;
        atomic->boundingSphere.center.y = scaledY;
        const float scaledZ = atomic->boundingSphere.center.z * scale;
        atomic->boundingSphere.center.z = scaledZ;
        atomic->boundingSphere.center.x = start.center.x + scaledX;
        atomic->boundingSphere.center.y = start.center.y + scaledY;
        atomic->boundingSphere.center.z = start.center.z + scaledZ;
    } else if (startIndex < numMorphTargets && endIndex < numMorphTargets) {
        atomic->boundingSphere = geometry->morphTarget[startIndex].boundingSphere;
    } else {
        atomic->boundingSphere = geometry->morphTarget[0].boundingSphere;
    }
    atomic->interpolator.flags &= ~rpINTERPOLATORDIRTYSPHERE;
    atomic->object.object.privateFlags |= rpATOMICPRIVATEWORLDBOUNDDIRTY;
}

// 0x749330
// Devuelve la esfera envolvente en coordenadas de mundo (+0x2C). Si el interpolador esta sucio la recalcula antes. Si el marco esta sucio (RwFrameDirty, bits 0 y 1 del marco raiz)
// o el atomo tiene privateFlags & 1: centro = LTM * centro local; con matriz ortonormal (flags & 3 == 3) el radio es el local; si no, radio local por la raiz del mayor de los tres
// cuadrados de longitud de los ejes (derecha, arriba, adelante). Cada longitud al cuadrado es (x*x + y*y) + z*z en float. El maximo conserva la semantica del EXE con fcom
// (NaN elige el segundo): m = (arriba < adelante o NaN) ? adelante : arriba; max = (derecha < m o NaN) ? m : derecha. privateFlags se relee tras _rwSqrt y se limpia el bit 0.
const RwSphere* RpAtomicGetWorldBoundingSphere(RpAtomic* atomic) {
    RwFrame* const frame = static_cast<RwFrame*>(atomic->object.object.parent);
    if (atomic->interpolator.flags & rpINTERPOLATORDIRTYSPHERE) {
        _rpAtomicResyncInterpolatedSphere(atomic);
    }
    if (RwFrameDirty(frame) || (atomic->object.object.privateFlags & rpATOMICPRIVATEWORLDBOUNDDIRTY)) {
        const RwMatrix* const ltm = RwFrameGetLTM(frame);
        RwV3dTransformPoint(&atomic->worldBoundingSphere.center, &atomic->boundingSphere.center, ltm);
        if ((ltm->flags & rwMATRIXTYPEMASK) == rwMATRIXTYPEORTHONORMAL) {
            atomic->worldBoundingSphere.radius = atomic->boundingSphere.radius;
            atomic->object.object.privateFlags &= ~rpATOMICPRIVATEWORLDBOUNDDIRTY;
        } else {
            const float rightSq = (ltm->right.x * ltm->right.x + ltm->right.y * ltm->right.y) + ltm->right.z * ltm->right.z;
            const float upSq    = (ltm->up.x * ltm->up.x + ltm->up.y * ltm->up.y) + ltm->up.z * ltm->up.z;
            const float atSq    = (ltm->at.x * ltm->at.x + ltm->at.y * ltm->at.y) + ltm->at.z * ltm->at.z;
            const float middle  = !(upSq >= atSq) ? atSq : upSq;
            const float maximum = !(rightSq >= middle) ? middle : rightSq;
            const float scaled  = _rwSqrt(maximum) * atomic->boundingSphere.radius;
            atomic->object.object.privateFlags &= ~rpATOMICPRIVATEWORLDBOUNDDIRTY;
            atomic->worldBoundingSphere.radius = scaled;
        }
    }
    return &atomic->worldBoundingSphere;
}

// 0x749120 (callback de lectura del plugin de derechos de render, id 0x1F, registrado con 0x74BE00): lee 4 bytes del flujo a 0xC924A8 y, si la longitud es 8, otros 4 a 0xC92480.
// Devuelve el flujo, o nulo si alguna lectura falla (RwStreamReadInt32 devuelve nulo). No usa object, offsetInObject ni sizeInObject.
static RwStream* AtomicRightsStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    if (!RwStreamReadInt32(stream, &s_rightsReadWord0, 4)) {
        return nullptr;
    }
    if (binaryLength == 8 && !RwStreamReadInt32(stream, &s_rightsReadWord1, 4)) {
        return nullptr;
    }
    return stream;
}

// 0x749160 (callback de escritura del mismo plugin): escribe pluginId (+0x2C) y pluginData (+0x30) de la tuberia del atomo (`object + 0x6C`, sin comprobar nulo) con RwStreamWriteInt32.
// Devuelve el flujo si la segunda escritura devuelve algo distinto de nulo (neg/sbb/and); si la primera falla devuelve nulo.
static RwStream* AtomicRightsStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    static_assert(offsetof(RpAtomic, pipeline) == 0x6C);
    const RxPipeline* const pipeline = static_cast<const RpAtomic*>(object)->pipeline;
    if (!RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&pipeline->pluginId), 4)) {
        return nullptr;
    }
    return RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&pipeline->pluginData), 4) ? stream : nullptr;
}

// 0x749520 (callback de RwFreeListForAllUsed sobre la lista de clumps, llamado desde 0x749480): destruye el clump que queda en la lista libre. Orden: DeInitObject de plugins del clump,
// cada atomo de la lista (el siguiente enlace se lee antes de destruir), cada luz y cada camara (se sacan de su lista por el enlace del plugin, que se pone a cero, y se destruyen;
// el desplazamiento del plugin se relee en cada vuelta y un puntero de objeto nulo corta), el marco del clump (RwFrameDestroyHierarchy) y por fin el clump a su lista libre.
static void ClumpFreeListDestroyCallBack(void* clumpPtr, void* data) {
    RpClump* const clump = static_cast<RpClump*>(clumpPtr);
    _rwPluginRegistryDeInitObject(&s_clumpTKList, clump);
    RwLLLink* const atomicEnd = &clump->atomicList.link;
    for (RwLLLink* link = atomicEnd->next; link != atomicEnd; ) {
        RwLLLink* const next = link->next;
        AtomicDestroyBody(reinterpret_cast<RpAtomic*>(reinterpret_cast<uint8*>(link) - offsetof(RpAtomic, inClumpLink)));
        link = next;
    }
    RwLLLink* const lightEnd = &clump->lightList.link;
    for (RwLLLink* link = lightEnd->next; link != lightEnd; ) {
        const RwInt32 offset = s_lightClumpLinkOffset;
        RwLLLink* const next = link->next;
        uint8* const light = reinterpret_cast<uint8*>(link) - offset - 4;
        uint32* const pluginData = reinterpret_cast<uint32*>(light + offset);
        RwLLLink* const pluginLink = reinterpret_cast<RwLLLink*>(pluginData + 1);
        pluginLink->prev->next = pluginLink->next;
        pluginLink->next->prev = pluginLink->prev;
        pluginData[2] = 0;
        pluginData[1] = 0;
        pluginData[0] = 0;
        RpLightDestroy(reinterpret_cast<RpLight*>(light));
        if (!light) {
            break;
        }
        link = next;
    }
    RwLLLink* const cameraEnd = &clump->cameraList.link;
    for (RwLLLink* link = cameraEnd->next; link != cameraEnd; ) {
        const RwInt32 offset = s_cameraClumpLinkOffset;
        RwLLLink* const next = link->next;
        uint8* const camera = reinterpret_cast<uint8*>(link) - offset - 4;
        uint32* const pluginData = reinterpret_cast<uint32*>(camera + offset);
        RwLLLink* const pluginLink = reinterpret_cast<RwLLLink*>(pluginData + 1);
        pluginLink->prev->next = pluginLink->next;
        pluginLink->next->prev = pluginLink->prev;
        pluginData[2] = 0;
        pluginData[1] = 0;
        pluginData[0] = 0;
        RwCameraDestroy(reinterpret_cast<RwCamera*>(camera));
        if (!camera) {
            break;
        }
        link = next;
    }
    if (clump->object.parent) {
        RwFrameDestroyHierarchy(static_cast<RwFrame*>(clump->object.parent));
    }
    RwFreeListFree(EngineFreeList(1), clump);
}

// 0x7496A0 (callback de RwFreeListForAllUsed sobre la lista de atomos): igual que RpAtomicDestroy sin valor de retorno.
static void AtomicFreeListDestroyCallBack(void* atomic, void* data) {
    AtomicDestroyBody(static_cast<RpAtomic*>(atomic));
}

// 0x749830 (llamada desde RpWorldPluginAttach, 0x74FE77): registra los plugins que enlazan camaras y luces a su clump (12 bytes cada uno, id 0x10): RwCameraRegisterPlugin con los
// constructores/destructores del EXE 0x749880/0x7498A0 y RpLightRegisterPlugin con 0x7498B0/0x7498D0 (se llaman por su direccion). El desplazamiento de camara se guarda en 0xC924D8
// aunque falle (si es negativo devuelve 0 sin registrar luces); el de luz se guarda siempre en 0xC924DC; devuelve 1 si no es negativo.
static RwBool ClumpPluginAttach() {
    s_cameraClumpLinkOffset = RwCameraRegisterPlugin(0xC, 0x10, reinterpret_cast<RwPluginObjectConstructor>(0x749880), reinterpret_cast<RwPluginObjectDestructor>(0x7498A0), nullptr);
    if (s_cameraClumpLinkOffset < 0) {
        return FALSE;
    }
    const RwInt32 lightOffset = RpLightRegisterPlugin(0xC, 0x10, reinterpret_cast<RwPluginObjectConstructor>(0x7498B0), reinterpret_cast<RwPluginObjectDestructor>(0x7498D0), nullptr);
    s_lightClumpLinkOffset = lightOffset;
    return lightOffset >= 0;
}

// RpClumpGtaCancelStream (0x72E700) vive en rwtexdict.cpp junto con el resto de la lectura de clumps de GTA.

// 0x749B70
// Como el original: el enlace siguiente se lee ANTES del callback (puede quitar el atomico actual); un callback que devuelve nulo corta; devuelve el clump.
RpClump* RpClumpForAllAtomics(RpClump* clump, RpAtomicCallBack callback, void* data) {
    static_assert(offsetof(RpAtomic, inClumpLink) == 0x40);
    const RwLLLink* const end = rwLinkListGetTerminator(&clump->atomicList);
    RwLLLink* link = rwLinkListGetFirstLLLink(&clump->atomicList);
    while (link != end) {
        RwLLLink* const next = rwLLLinkGetNext(link);
        if (!callback(rwLLLinkGetData(link, RpAtomic, inClumpLink), data)) {
            break;
        }
        link = next;
    }
    return clump;
}

// 0x749C00
// Recorre la lista de luces del clump; el objeto es el enlace menos el desplazamiento del plugin (0xC924DC, releido en cada vuelta) menos 4. El enlace siguiente se lee ANTES del
// callback; un callback que devuelve nulo corta; devuelve el clump.
RpClump* RpClumpForAllLights(RpClump* clump, RpLightCallBack callback, void* data) {
    const RwLLLink* const end = &clump->lightList.link;
    for (RwLLLink* link = clump->lightList.link.next; link != end; ) {
        const RwInt32 offset = s_lightClumpLinkOffset;
        RwLLLink* const next = link->next;
        if (!callback(reinterpret_cast<RpLight*>(reinterpret_cast<uint8*>(link) - offset - 4), data)) {
            break;
        }
        link = next;
    }
    return clump;
}

// 0x749BB0
// Igual que RpClumpForAllLights con la lista de camaras y el desplazamiento de 0xC924D8.
RpClump* RpClumpForAllCameras(RpClump* clump, RwCameraCallBack callback, void* data) {
    const RwLLLink* const end = &clump->cameraList.link;
    for (RwLLLink* link = clump->cameraList.link.next; link != end; ) {
        const RwInt32 offset = s_cameraClumpLinkOffset;
        RwLLLink* const next = link->next;
        if (!callback(reinterpret_cast<RwCamera*>(reinterpret_cast<uint8*>(link) - offset - 4), data)) {
            break;
        }
        link = next;
    }
    return clump;
}

// 0x74BF20
// Como el original: _rwObjectHasFrameSetFrame (0x804EF0) y luego se marca la esfera envolvente del mundo como sucia (byte privateFlags |= 1, releido tras la llamada).
RpAtomic* RpAtomicSetFrame(RpAtomic* atomic, RwFrame* frame) {
    _rwObjectHasFrameSetFrame(atomic, frame);
    atomic->object.object.privateFlags |= rpATOMICPRIVATEWORLDBOUNDDIRTY;
    return atomic;
}

// ---------------------------------------------------------------------------------------------------------------------
// Lote 227A (sin validar): RpClump (creacion, clonado, destruccion, listas de atomicos/luces, lectura de flujo) y los
// registros de plugins de RpAtomic/RpClump. Todo el codigo sigue el desensamblado del EXE (0x749F70..0x74BE70).
// ---------------------------------------------------------------------------------------------------------------------
namespace {

// Registro de plugins de RpClump y desplazamientos de plugin que dejan RpWorldPluginAttach (0x74FDxx) / clumpModule en el EXE.
static auto& s_clumpModuleOffset = StaticRef<RwInt32>(0xC924AC);  // clumpModule.globalsOffset (globales del modulo dentro de RwEngineInstance)
static auto& s_cameraClumpOffset = StaticRef<RwInt32>(0xC924D8);  // desplazamiento del enlace de clump dentro de RwCamera
static auto& s_lightClumpOffset  = StaticRef<RwInt32>(0xC924DC);  // desplazamiento del enlace de clump dentro de RpLight
static auto& s_atomicRightsPluginID  = StaticRef<RwUInt32>(0xC924A8); // plugin cuyos derechos se invocan tras leer un atomico
static auto& s_atomicRightsExtraData = StaticRef<RwUInt32>(0xC92480);

constexpr RwUInt32 kClumpAllocHint = 0x30010;
constexpr RwUInt32 kGeomListAllocHint = 0x3000F;

// Identificadores de chunk (rwID_*) y codigos de error que usa el EXE en esta zona.
constexpr RwUInt32 kChunkStruct = 0x01, kChunkCamera = 0x05, kChunkFrameList = 0x0E, kChunkGeometry = 0x0F;
constexpr RwUInt32 kChunkLight = 0x12, kChunkAtomic = 0x14, kChunkGeometryList = 0x1A;
constexpr RwInt32 kErrBadVersion = static_cast<RwInt32>(0x80000004);
constexpr RwInt32 kErrNoMem      = static_cast<RwInt32>(0x80000013);
constexpr RwInt32 kErrRead       = static_cast<RwInt32>(0x8000001A);
constexpr RwInt32 kErrCloneAtomicWithoutFrame = 5;
constexpr RwInt32 kErrorPluginID = 2; // rwPLUGIN_ID del modulo world en las estructuras RwError del EXE

// Funciones del EXE que se llaman por direccion fija (aun sin reconstruir en este lote).
RpClumpCallBack DefaultClumpCallBack() { return reinterpret_cast<RpClumpCallBack>(static_cast<uintptr_t>(0x74A260)); }       // devuelve su primer argumento
RwCameraCallBack ClumpCameraDestroyCallBack() { return reinterpret_cast<RwCameraCallBack>(static_cast<uintptr_t>(0x74A200)); } // quita la camara de la lista del clump y la destruye

// Globales del modulo de clump dentro de RwEngineInstance (+globalsOffset): listas libres de atomicos (+0) y de clumps (+4).
struct ClumpModuleGlobals {
    RwFreeList* atomicFreeList;
    RwFreeList* clumpFreeList;
};
inline ClumpModuleGlobals& ClumpGlobals() {
    return *reinterpret_cast<ClumpModuleGlobals*>(reinterpret_cast<uintptr_t>(RwEngineInstance) + static_cast<uintptr_t>(s_clumpModuleOffset));
}

// RwFreeListAlloc / RwFreeListFree del motor: [RwEngineInstance + 0x144] y [+ 0x148], cdecl, (lista, puntero).
static_assert(offsetof(RwGlobals, memoryAlloc) == 0x144);
static_assert(offsetof(RwGlobals, memoryFree) == 0x148);
inline void* FreeListAlloc(RwFreeList* freeList, RwUInt32 hint) {
    return RwEngineInstance->memoryAlloc(freeList, hint);
}
inline void FreeListFree(RwFreeList* freeList, void* memory) {
    RwEngineInstance->memoryFree(freeList, memory);
}


// Lista de geometrias que se carga junto con un clump (_rpGeometryList del SDK).
struct RpGeometryList {
    RpGeometry** geometries;
    RwInt32      numGeoms;
};

// Datos del enlace de clump que un plugin guarda dentro de una luz o una camara: {clump, enlace de lista}.
struct ClumpListEntry {
    RpClump* clump;
    RwLLLink link;
};
static_assert(offsetof(ClumpListEntry, link) == 4);
static_assert(offsetof(RpAtomic, inClumpLink) == 0x40);
static_assert(offsetof(RpAtomic, clump) == 0x3C);
static_assert(offsetof(RpAtomic, repEntry) == 0x14);
static_assert(offsetof(RpAtomic, geometry) == 0x18);
static_assert(offsetof(RpAtomic, boundingSphere) == 0x1C);
static_assert(offsetof(RpGeometry, morphTarget) == 0x5C);
static_assert(offsetof(RpMorphTarget, boundingSphere) == 4);
static_assert(offsetof(RpClump, atomicList) == 0x08);
static_assert(offsetof(RpClump, lightList) == 0x10);
static_assert(offsetof(RpClump, cameraList) == 0x18);
static_assert(offsetof(RpClump, callback) == 0x28);
static_assert(offsetof(RwFrame, root) == 0xA0);

inline ClumpListEntry* ListEntryOf(void* object, RwInt32 pluginOffset) {
    return reinterpret_cast<ClumpListEntry*>(reinterpret_cast<uintptr_t>(object) + static_cast<uintptr_t>(pluginOffset));
}
// Objeto (luz o camara) al que pertenece un enlace de la lista del clump: enlace - desplazamiento - 4.
inline void* ObjectOfListLink(RwLLLink* link, RwInt32 pluginOffset) {
    return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(link) - static_cast<uintptr_t>(pluginOffset) - offsetof(ClumpListEntry, link));
}

// Saca el enlace de su lista y pone a cero prev, next y clump (en ese orden), como el EXE.
inline void UnlinkListEntry(ClumpListEntry* entry) {
    RwLLLink* const prev = entry->link.prev;
    prev->next = entry->link.next;
    entry->link.next->prev = entry->link.prev;
    entry->link.prev = nullptr;
    entry->link.next = nullptr;
    entry->clump = nullptr;
}

// Inserta el enlace al principio de la lista (la lista se relee tras tocar el enlace, como en el EXE).
inline void AttachListEntry(ClumpListEntry* entry, RpClump* clump, RwLinkList* list) {
    RwLLLink* const head = &list->link;
    RwLLLink* const first = head->next;
    entry->link.prev = head;
    entry->link.next = first;
    head->next->prev = &entry->link;
    head->next = &entry->link;
    entry->clump = clump;
}

// Cuerpo comun de "destruir el almacenamiento de un atomico" (0x74BCC0, el bucle de 0x74A310 y las salidas de error de 0x74B060):
// destructores de plugins, entrada de recursos, geometria (con actualizacion del marco si esta en un mundo), marco y memoria.
void DestroyAtomicStorage(RpAtomic* atomic) {
    _rwPluginRegistryDeInitObject(&s_atomicTKList, atomic);
    if (atomic->repEntry) {
        RwResourcesFreeResEntry(atomic->repEntry);
    }
    if (atomic->geometry) {
        RpGeometryDestroy(atomic->geometry);
        RwFrame* const frame = static_cast<RwFrame*>(atomic->object.object.parent);
        atomic->geometry = nullptr;
        if (frame && RpAtomicGetWorld(atomic)) {
            RwFrameUpdateObjects(frame);
        }
    }
    _rwObjectHasFrameReleaseFrame(atomic);
    FreeListFree(ClumpGlobals().atomicFreeList, atomic);
}

// Bucle de RpAtomicDestroy sobre la lista de atomicos de un clump (copiado tal cual en las salidas de error de 0x749F70 y 0x74B420):
// lee el siguiente enlace antes de destruir; corta si el atomico (enlace - 0x40) es nulo o si no queda mas lista.
void DestroyClumpAtomicsWithRpAtomicDestroy(RpClump* clump) {
    RwLLLink* const end = &clump->atomicList.link;
    RwLLLink* link = end->next;
    if (link == end) {
        return;
    }
    for (;;) {
        RwLLLink* const next = link->next;
        RpAtomic* const atomic = rwLLLinkGetData(link, RpAtomic, inClumpLink);
        RpAtomicDestroy(atomic);
        if (!atomic) {
            break;
        }
        if (next == end) {
            break;
        }
        link = next;
    }
}

// Final comun de las salidas de error de 0x749F70: quita las camaras con el callback del EXE (0x74A200), destruye el marco
// raiz (si hay) y devuelve el clump a su lista libre.
void FreeClumpCamerasFrameAndMemory(RpClump* clump) {
    RpClumpForAllCameras(clump, ClumpCameraDestroyCallBack(), nullptr);
    if (clump->object.parent) {
        RwFrameDestroyHierarchy(static_cast<RwFrame*>(clump->object.parent));
    }
    FreeListFree(ClumpGlobals().clumpFreeList, clump);
}

// 0x74A1C0: callback de RpClumpForAllLights; quita el enlace de la luz de la lista del clump, lo pone a cero y destruye la luz.
RpLight* ClumpLightDestroyCallBack(RpLight* light, void* /*data*/) {
    UnlinkListEntry(ListEntryOf(light, s_lightClumpOffset));
    RpLightDestroy(light);
    return light;
}

// 0x74BCC0: callback de RpClumpForAllAtomics que libera el atomico (sin sacarlo de la lista del clump).
RpAtomic* ClumpAtomicDestroyCallBack(RpAtomic* atomic, void* /*data*/) {
    DestroyAtomicStorage(atomic);
    return atomic;
}

// 0x74AE50 (_rpGeometryListDeinitialize): destruye las geometrias de la lista y libera el vector con rwfree del motor.
RpGeometryList* _rpGeometryListDeinitialize(RpGeometryList* geomList) {
    for (RwInt32 i = 0; i < geomList->numGeoms; ++i) {
        RpGeometryDestroy(geomList->geometries[i]);
    }
    if (geomList->geometries) {
        RwFree(geomList->geometries);
        geomList->geometries = nullptr;
    }
    return geomList;
}

// Tras fallar una lectura del atomico: libera el atomico y notifica el error.
void AtomicReadFailure(RpAtomic* atomic, RwInt32 errorCode) {
    DestroyAtomicStorage(atomic);
    RaiseRwError(errorCode);
}

// Copia la esfera envolvente del primer morph target de la geometria al atomico (16 bytes, como cuatro dwords del EXE).
inline void CopyMorphTargetSphereToAtomic(RpAtomic* atomic, const RpGeometry* geometry) {
    std::memcpy(&atomic->boundingSphere, &geometry->morphTarget[0].boundingSphere, sizeof(RwSphere));
}

// Marca el marco del atomico como actualizable si el atomico esta en un mundo.
inline void UpdateAtomicFrameIfInWorld(RpAtomic* atomic) {
    RwFrame* const frame = static_cast<RwFrame*>(atomic->object.object.parent);
    if (frame && RpAtomicGetWorld(atomic)) {
        RwFrameUpdateObjects(frame);
    }
}

// 0x74B060: lee un atomico del flujo (chunk estructura) y resuelve su marco y su geometria con las listas ya leidas del clump;
// sin lista de geometrias lee la geometria del propio flujo. Cdecl (flujo, lista de marcos, lista de geometrias).
RpAtomic* ClumpAtomicStreamRead(RwStream* stream, rwFrameList* frameList, RpGeometryList* geomList) {
    RwUInt32 length;
    RwUInt32 version;
    if (!RwStreamFindChunk(stream, kChunkStruct, &length, &version)) {
        RaiseRwError(kErrRead);
        return nullptr;
    }
    if (version < kStreamVersionMin || version > kStreamVersionMax) {
        RaiseRwError(kErrBadVersion);
        return nullptr;
    }
    struct AtomicChunk {
        RwInt32 frameIndex;
        RwInt32 geomIndex;
        RwInt32 flags;
        RwInt32 unused;
    } chunk = {};
    if (RwStreamRead(stream, &chunk, length) != length) {
        RaiseRwError(kErrRead);
        return nullptr;
    }

    RpAtomic* const atomic = RpAtomicCreate();
    if (!atomic) {
        return nullptr;
    }
    atomic->object.object.flags = static_cast<RwUInt8>(chunk.flags);

    if (frameList->numFrames) {
        _rwObjectHasFrameSetFrame(atomic, frameList->frames[chunk.frameIndex]);
        atomic->object.object.privateFlags |= rpATOMICPRIVATEWORLDBOUNDDIRTY;
    }

    if (geomList->numGeoms) {
        RpGeometry* const geometry = geomList->geometries[chunk.geomIndex];
        if (geometry != atomic->geometry) {
            if (geometry) {
                ++geometry->refCount; // RpGeometryAddRef (0x74CCB0: inc word ptr [geometry+0xE])
            }
            if (atomic->geometry) {
                RpGeometryDestroy(atomic->geometry);
            }
            atomic->geometry = geometry;
            if (geometry) {
                CopyMorphTargetSphereToAtomic(atomic, geometry);
            }
            UpdateAtomicFrameIfInWorld(atomic);
        }
    } else {
        RwUInt32 geometryVersion;
        if (!RwStreamFindChunk(stream, kChunkGeometry, nullptr, &geometryVersion)) {
            AtomicReadFailure(atomic, kErrRead);
            return nullptr;
        }
        if (geometryVersion < kStreamVersionMin || geometryVersion > kStreamVersionMax) {
            AtomicReadFailure(atomic, kErrBadVersion);
            return nullptr;
        }
        RpGeometry* const geometry = RpGeometryStreamRead(stream);
        if (!geometry) {
            AtomicReadFailure(atomic, kErrRead);
            return nullptr;
        }
        if (geometry != atomic->geometry) {
            ++geometry->refCount; // RpGeometryAddRef
            if (atomic->geometry) {
                RpGeometryDestroy(atomic->geometry);
            }
            atomic->geometry = geometry;
            CopyMorphTargetSphereToAtomic(atomic, geometry);
            UpdateAtomicFrameIfInWorld(atomic);
        }
        RpGeometryDestroy(geometry); // suelta la referencia que dio la lectura
    }

    s_atomicRightsPluginID = 0;
    s_atomicRightsExtraData = 0;
    if (!_rwPluginRegistryReadDataChunks(&s_atomicTKList, stream, atomic)) {
        RaiseRwError(kErrRead); // el EXE no libera el atomico en este camino
        return nullptr;
    }
    if (s_atomicRightsPluginID) {
        _rwPluginRegistryInvokeRights(&s_atomicTKList, s_atomicRightsPluginID, atomic, s_atomicRightsExtraData);
    }
    return atomic;
}

// Salidas de error de 0x74B420. "Cola 1" (0x74B527): luces y camaras del clump, marco raiz, memoria del clump y error de lectura.
void ClumpReadFailTail(RpClump* clump) {
    RpClumpForAllLights(clump, ClumpLightDestroyCallBack, nullptr);
    RpClumpForAllCameras(clump, ClumpCameraDestroyCallBack(), nullptr);
    if (clump->object.parent) {
        RwFrameDestroyHierarchy(static_cast<RwFrame*>(clump->object.parent));
    }
    FreeListFree(ClumpGlobals().clumpFreeList, clump);
    RaiseRwError(kErrRead);
}

// Fallo antes de leer la lista de marcos (o con ella ya liberada): destructores de plugins del clump y atomicos con RpAtomicDestroy.
void ClumpReadAbortBeforeFrames(RpClump* clump) {
    _rwPluginRegistryDeInitObject(&s_clumpTKList, clump);
    DestroyClumpAtomicsWithRpAtomicDestroy(clump);
    ClumpReadFailTail(clump);
}

// Fallo con la lista de marcos ya leida y la de geometrias liberada o inexistente (0x74B7DE / 0x74BBF5): atomicos con el callback del EXE.
void ClumpReadAbortWithFrames(RpClump* clump, rwFrameList* frameList) {
    _rwFrameListDeinitialize(frameList);
    _rwPluginRegistryDeInitObject(&s_clumpTKList, clump);
    RpClumpForAllAtomics(clump, ClumpAtomicDestroyCallBack, nullptr);
    ClumpReadFailTail(clump);
}

// Fallo con la lista de geometrias ya creada: se libera antes de la salida comun.
void ClumpReadAbortWithGeometries(RpClump* clump, rwFrameList* frameList, RpGeometryList* geomList) {
    _rpGeometryListDeinitialize(geomList);
    ClumpReadAbortWithFrames(clump, frameList);
}

} // namespace

RpClump* RpClumpCreateSpace(const RwV3d* position, RwReal radius) {
    return ((RpClump*(__cdecl *)(const RwV3d*, RwReal))0x749970)(position, radius);
}

// 0x749B20
// Para cada atomo de la lista cuyo indicador `flags` tiene el bit 4 (rpATOMICRENDER): RwFrameGetLTM(marco) (resultado ignorado) y su callback de dibujo; si el callback devuelve nulo
// el resultado pasa a nulo (y se sigue con el resto). Devuelve el clump, o nulo si algun callback fallo.
RpClump* RpClumpRender(RpClump* clump) {
    RpClump* result = clump;
    const RwLLLink* const end = &clump->atomicList.link;
    for (RwLLLink* link = clump->atomicList.link.next; link != end; link = link->next) {
        RpAtomic* const atomic = reinterpret_cast<RpAtomic*>(reinterpret_cast<uint8*>(link) - offsetof(RpAtomic, inClumpLink));
        if (atomic->object.object.flags & rpATOMICRENDER) {
            RwFrameGetLTM(static_cast<RwFrame*>(atomic->object.object.parent));
            if (!atomic->renderCallBack(atomic)) {
                result = nullptr;
            }
        }
    }
    return result;
}

// 0x74A4C0: saca el atomico de la lista del clump (el enlace no se pone a cero) y anula atomic->clump; devuelve el clump.
RpClump* RpClumpRemoveAtomic(RpClump* clump, RpAtomic* atomic) {
    RwLLLink* const prev = atomic->inClumpLink.prev;
    prev->next = atomic->inClumpLink.next;
    atomic->inClumpLink.next->prev = atomic->inClumpLink.prev;
    atomic->clump = nullptr;
    return clump;
}

// 0x74A490: inserta el atomico al principio de la lista del clump y le asigna el clump; devuelve el clump.
RpClump* RpClumpAddAtomic(RpClump* clump, RpAtomic* atomic) {
    RwLLLink* const head = &clump->atomicList.link;
    RwLLLink* const first = head->next;
    atomic->inClumpLink.prev = head;
    atomic->inClumpLink.next = first;
    head->next->prev = &atomic->inClumpLink;
    head->next = &atomic->inClumpLink;
    atomic->clump = clump;
    return clump;
}

// 0x74A520: saca la luz de la lista del clump (enlace dentro de la luz, en el desplazamiento del plugin 0xC924DC) y lo pone a cero.
RpClump* RpClumpRemoveLight(RpClump* clump, RpLight* light) {
    UnlinkListEntry(ListEntryOf(light, s_lightClumpOffset));
    return clump;
}

RpClump* RpClumpAddLight(RpClump* clump, RpLight* light) {
    return ((RpClump*(__cdecl *)(RpClump*, RpLight*))0x74A4F0)(clump, light);
}

RpClump* RpClumpRemoveCamera(RpClump* clump, RwCamera* camera) {
    return ((RpClump*(__cdecl *)(RpClump*, RwCamera*))0x74A580)(clump, camera);
}

RpClump* RpClumpAddCamera(RpClump* clump, RwCamera* camera) {
    return ((RpClump*(__cdecl *)(RpClump*, RwCamera*))0x74A550)(clump, camera);
}

// 0x74A310: destruye el clump: destructores de plugins, atomicos (almacenamiento propio, sin sacarlos de la lista), luces y camaras
// (sacan su enlace, lo ponen a cero y se destruyen), marco raiz y memoria del clump. Devuelve 1.
RwBool RpClumpDestroy(RpClump* clump) {
    _rwPluginRegistryDeInitObject(&s_clumpTKList, clump);

    RwLLLink* const atomicEnd = &clump->atomicList.link;
    RwLLLink* link = atomicEnd->next;
    if (link != atomicEnd) {
        for (;;) {
            RwLLLink* const next = link->next;
            DestroyAtomicStorage(rwLLLinkGetData(link, RpAtomic, inClumpLink));
            if (next == atomicEnd) {
                break;
            }
            link = next;
        }
    }

    RwLLLink* const lightEnd = &clump->lightList.link;
    link = lightEnd->next;
    if (link != lightEnd) {
        for (;;) {
            RwLLLink* const next = link->next;
            const RwInt32 offset = s_lightClumpOffset;
            RpLight* const light = static_cast<RpLight*>(ObjectOfListLink(link, offset));
            UnlinkListEntry(ListEntryOf(light, offset));
            RpLightDestroy(light);
            if (!light) {
                break;
            }
            if (next == lightEnd) {
                break;
            }
            link = next;
        }
    }

    RwLLLink* const cameraEnd = &clump->cameraList.link;
    link = cameraEnd->next;
    if (link != cameraEnd) {
        for (;;) {
            RwLLLink* const next = link->next;
            const RwInt32 offset = s_cameraClumpOffset;
            RwCamera* const camera = static_cast<RwCamera*>(ObjectOfListLink(link, offset));
            UnlinkListEntry(ListEntryOf(camera, offset));
            RwCameraDestroy(camera);
            if (!camera) {
                break;
            }
            if (next == cameraEnd) {
                break;
            }
            link = next;
        }
    }

    if (clump->object.parent) {
        RwFrameDestroyHierarchy(static_cast<RwFrame*>(clump->object.parent));
    }
    FreeListFree(ClumpGlobals().clumpFreeList, clump);
    return TRUE;
}

// 0x74A290: reserva un clump de la lista libre del modulo (pista 0x30010) y lo deja vacio: listas de atomicos, luces y camaras
// vacias, tipo rpCLUMP (2), sin marco, callback por defecto (0x74A260) y plugins inicializados. Devuelve nulo si no hay memoria.
RpClump* RpClumpCreate(void) {
    RpClump* const clump = static_cast<RpClump*>(FreeListAlloc(ClumpGlobals().clumpFreeList, kClumpAllocHint));
    if (!clump) {
        return nullptr;
    }
    clump->atomicList.link.prev = &clump->atomicList.link;
    clump->atomicList.link.next = &clump->atomicList.link;
    clump->lightList.link.prev = &clump->lightList.link;
    clump->object.type = rpCLUMP;
    clump->lightList.link.next = &clump->lightList.link;
    clump->object.subType = 0;
    clump->object.flags = 0;
    clump->object.privateFlags = 0;
    clump->object.parent = nullptr;
    clump->cameraList.link.next = &clump->cameraList.link;
    clump->cameraList.link.prev = &clump->cameraList.link;
    clump->inWorldLink.prev = nullptr;
    clump->inWorldLink.next = nullptr;
    clump->callback = DefaultClumpCallBack();
    _rwPluginRegistryInitObject(&s_clumpTKList, clump);
    return clump;
}

// 0x749F70: clona el clump. Crea el clump, clona la jerarquia de marcos (_rwFrameCloneAndLinkClones deja en frame->root el clon),
// clona cada atomico (RpAtomicClone) asignandole el marco clon y lo anade; al final purga los enlaces temporales de clones
// (_rwFramePurgeClone) y copia callback y datos de plugins. Si algo falla destruye el clump nuevo y devuelve nulo.
RpClump* RpClumpClone(RpClump* clump) {
    RpClump* const newClump = RpClumpCreate();
    if (!newClump) {
        return nullptr;
    }

    newClump->object.parent = _rwFrameCloneAndLinkClones(static_cast<RwFrame*>(clump->object.parent));
    if (!newClump->object.parent) {
        // Sin marco clonado: se deshace el clump nuevo (las listas estan vacias, pero el EXE recorre igualmente atomicos y luces).
        _rwPluginRegistryDeInitObject(&s_clumpTKList, newClump);
        DestroyClumpAtomicsWithRpAtomicDestroy(newClump);

        RwLLLink* const lightEnd = &newClump->lightList.link;
        RwLLLink* link = lightEnd->next;
        if (link != lightEnd) {
            for (;;) {
                RwLLLink* const next = link->next;
                const RwInt32 offset = s_lightClumpOffset;
                RpLight* const light = static_cast<RpLight*>(ObjectOfListLink(link, offset));
                RpClump* const owner = ListEntryOf(light, offset)->clump;
                RpClumpRemoveLight(owner, light);
                RpLightDestroy(light);
                if (!light) {
                    break;
                }
                if (next == lightEnd) {
                    break;
                }
                link = next;
            }
        }
        FreeClumpCamerasFrameAndMemory(newClump);
        return nullptr;
    }

    bool success = true;
    RwLLLink* const atomicEnd = &clump->atomicList.link;
    RwLLLink* link = atomicEnd->next;
    if (link != atomicEnd) {
        for (;;) {
            RwLLLink* const next = link->next;
            RpAtomic* const sourceAtomic = rwLLLinkGetData(link, RpAtomic, inClumpLink);
            RpAtomic* const clonedAtomic = RpAtomicClone(sourceAtomic);
            if (!clonedAtomic) {
                success = false;
                break;
            }
            RwFrame* const sourceFrame = static_cast<RwFrame*>(sourceAtomic->object.object.parent);
            if (!sourceFrame) {
                // Atomico sin marco: el clon se libera y se notifica el error 5; el EXE deja la bandera de exito y corta el bucle.
                _rwPluginRegistryDeInitObject(&s_atomicTKList, clonedAtomic);
                if (clonedAtomic->repEntry) {
                    RwResourcesFreeResEntry(clonedAtomic->repEntry);
                }
                RpAtomicSetGeometry(clonedAtomic, nullptr, 0);
                _rwObjectHasFrameReleaseFrame(clonedAtomic);
                FreeListFree(ClumpGlobals().atomicFreeList, clonedAtomic);
                RaiseRwError(kErrCloneAtomicWithoutFrame);
                break;
            }
            _rwObjectHasFrameSetFrame(clonedAtomic, sourceFrame->root);
            clonedAtomic->object.object.privateFlags |= rpATOMICPRIVATEWORLDBOUNDDIRTY;
            RpClumpAddAtomic(newClump, clonedAtomic);
            if (next == atomicEnd) {
                break;
            }
            link = next;
        }
    }

    _rwFramePurgeClone(static_cast<RwFrame*>(clump->object.parent));

    if (success) {
        newClump->callback = clump->callback;
        _rwPluginRegistryCopyObject(&s_clumpTKList, newClump, clump);
        return newClump;
    }

    // Fallo al clonar un atomico: se deshace el clump nuevo (las luces con el callback 0x74A1C0, las camaras con 0x74A200).
    _rwPluginRegistryDeInitObject(&s_clumpTKList, newClump);
    DestroyClumpAtomicsWithRpAtomicDestroy(newClump);

    RwLLLink* const lightEnd = &newClump->lightList.link;
    link = lightEnd->next;
    if (link != lightEnd) {
        for (;;) {
            RwLLLink* const next = link->next;
            const RwInt32 offset = s_lightClumpOffset;
            RpLight* const light = static_cast<RpLight*>(ObjectOfListLink(link, offset));
            if (!ClumpLightDestroyCallBack(light, nullptr)) {
                break;
            }
            if (next == lightEnd) {
                break;
            }
            link = next;
        }
    }
    FreeClumpCamerasFrameAndMemory(newClump);
    return nullptr;
}

// 0x749DC0
// Destruye el atomo: DeInitObject de plugins, entrada de recursos, geometria (y, con marco en un mundo, RwFrameUpdateObjects), suelta el marco y devuelve el atomo a la lista libre. Devuelve 1.
RwBool RpAtomicDestroy(RpAtomic* atomic) {
    AtomicDestroyBody(atomic);
    return TRUE;
}

// 0x749E60
// Crea un atomo con RpAtomicCreate (nulo si falla) y copia type, subType, flags y privateFlags (byte a byte), pone parent a nulo, _rwObjectHasFrameSetFrame(clon, nulo), privateFlags |= 1,
// y, si el original tiene geometria (el clon recien creado no la tiene), la asigna con AddRef y copia la esfera del morph target 0 sin interpolar (sin mirar las banderas). Despues
// copia la esfera local (4 dwords), el callback de dibujo, la tuberia y el interpolador (5 dwords) y por ultimo los datos de plugin con _rwPluginRegistryCopyObject.
RpAtomic* RpAtomicClone(RpAtomic* atomic) {
    RpAtomic* const clone = RpAtomicCreate();
    if (!clone) {
        return nullptr;
    }
    clone->object.object.type         = atomic->object.object.type;
    clone->object.object.subType      = atomic->object.object.subType;
    clone->object.object.flags        = atomic->object.object.flags;
    clone->object.object.privateFlags = atomic->object.object.privateFlags;
    clone->object.object.parent       = nullptr;
    clone->object.object.flags        = atomic->object.object.flags;
    _rwObjectHasFrameSetFrame(clone, nullptr);
    clone->object.object.privateFlags |= rpATOMICPRIVATEWORLDBOUNDDIRTY;
    RpGeometry* const sourceGeometry = atomic->geometry;
    if (sourceGeometry != clone->geometry) {
        if (sourceGeometry) {
            GeometryAddRef(sourceGeometry);
        }
        if (clone->geometry) {
            RpGeometryDestroy(clone->geometry);
        }
        clone->geometry = sourceGeometry;
        if (sourceGeometry) {
            clone->boundingSphere = sourceGeometry->morphTarget[0].boundingSphere;
        }
        RwFrame* const frame = static_cast<RwFrame*>(clone->object.object.parent);
        if (frame && RpAtomicGetWorld(clone)) {
            RwFrameUpdateObjects(frame);
        }
    }
    clone->boundingSphere = atomic->boundingSphere;
    clone->renderCallBack = atomic->renderCallBack;
    clone->pipeline       = atomic->pipeline;
    clone->interpolator   = atomic->interpolator;
    _rwPluginRegistryCopyObject(&s_atomicTKList, clone, atomic);
    return clone;
}

// 0x749C50
// Reserva un atomo de la lista libre de atomos de RwEngineInstance (pista 0x30014; nulo si falla) y lo inicializa: type 1 (rpATOMIC), flags 5 (collision test | render), privateFlags 1,
// sync = 0x749D20, _rwObjectHasFrameSetFrame(atomo, nulo) y privateFlags |= 1, geometria y esferas a cero, callback de dibujo = AtomicDefaultRenderCallBack (0x7491C0), tiempo y recipTime
// 1.0f, indices y posicion a cero, flags del interpolador 3, sin clump ni enlace ni tuberia, lista de sectores vacia y por ultimo _rwPluginRegistryInitObject.
RpAtomic* RpAtomicCreate(void) {
    RpAtomic* const atomic = static_cast<RpAtomic*>(RwFreeListAlloc(EngineFreeList(0), 0x30014));
    if (!atomic) {
        return nullptr;
    }
    atomic->object.object.type = rpATOMIC;
    atomic->object.object.subType = 0;
    atomic->object.object.parent = nullptr;
    atomic->object.sync = reinterpret_cast<RwObjectHasFrameSyncFunction>(&AtomicSyncCallBack);
    atomic->repEntry = nullptr;
    atomic->object.object.flags = rpATOMICCOLLISIONTEST | rpATOMICRENDER;
    atomic->object.object.privateFlags = 1;
    _rwObjectHasFrameSetFrame(atomic, nullptr);
    atomic->object.object.privateFlags |= rpATOMICPRIVATEWORLDBOUNDDIRTY;
    atomic->geometry = nullptr;
    atomic->boundingSphere.radius = 0.0f;
    atomic->boundingSphere.center.x = 0.0f;
    atomic->boundingSphere.center.y = 0.0f;
    atomic->boundingSphere.center.z = 0.0f;
    atomic->worldBoundingSphere.radius = 0.0f;
    atomic->worldBoundingSphere.center.x = 0.0f;
    atomic->worldBoundingSphere.center.y = 0.0f;
    atomic->worldBoundingSphere.center.z = 0.0f;
    atomic->renderCallBack = AtomicDefaultRenderCallBack;
    atomic->interpolator.time = 1.0f;
    atomic->interpolator.recipTime = 1.0f;
    atomic->interpolator.startMorphTarget = 0;
    atomic->interpolator.endMorphTarget = 0;
    atomic->interpolator.position = 0.0f;
    atomic->interpolator.flags = rpINTERPOLATORDIRTYINSTANCE | rpINTERPOLATORDIRTYSPHERE;
    atomic->inClumpLink.prev = nullptr;
    atomic->inClumpLink.next = nullptr;
    atomic->clump = nullptr;
    atomic->pipeline = nullptr;
    atomic->llWorldSectorsInAtomic.link.next = &atomic->llWorldSectorsInAtomic.link;
    atomic->llWorldSectorsInAtomic.link.prev = &atomic->llWorldSectorsInAtomic.link;
    _rwPluginRegistryInitObject(&s_atomicTKList, atomic);
    return atomic;
}

// 0x749D20 (campo `sync` de los atomos): si el interpolador esta sucio de esfera (bit 2 de flags) la recalcula; privateFlags |= 1; devuelve el atomo.
static RpAtomic* AtomicSyncCallBack(RpAtomic* atomic) {
    if (atomic->interpolator.flags & rpINTERPOLATORDIRTYSPHERE) {
        _rpAtomicResyncInterpolatedSphere(atomic);
    }
    atomic->object.object.privateFlags |= rpATOMICPRIVATEWORLDBOUNDDIRTY;
    return atomic;
}

// 0x749D40
// Si la geometria cambia: AddRef de la nueva (si no es nula, 0x74CCB0), destruye la anterior (RpGeometryDestroy) y la asigna; si flags no tiene el bit 1 (rpATOMICSAMEBOUNDINGSPHERE)
// copia la esfera del morph target 0 de la nueva (si existe) y, con marco en un mundo, RwFrameUpdateObjects. Devuelve el atomo.
RpAtomic* RpAtomicSetGeometry(RpAtomic* atomic, RpGeometry* geometry, RwUInt32 flags) {
    if (geometry != atomic->geometry) {
        if (geometry) {
            GeometryAddRef(geometry);
        }
        if (atomic->geometry) {
            RpGeometryDestroy(atomic->geometry);
        }
        atomic->geometry = geometry;
        if (!(flags & rpATOMICSAMEBOUNDINGSPHERE)) {
            if (geometry) {
                atomic->boundingSphere = geometry->morphTarget[0].boundingSphere;
            }
            RwFrame* const frame = static_cast<RwFrame*>(atomic->object.object.parent);
            if (frame && RpAtomicGetWorld(atomic)) {
                RwFrameUpdateObjects(frame);
            }
        }
    }
    return atomic;
}

RpClump* RpClumpSetCallBack(RpClump* clump, RpClumpCallBack callback) {
    return ((RpClump*(__cdecl *)(RpClump*, RpClumpCallBack))0x74A240)(clump, callback);
}

RpClumpCallBack RpClumpGetCallBack(const RpClump* clump) {
    return ((RpClumpCallBack(__cdecl *)(const RpClump*))0x74A270)(clump);
}

// 0x7498E0
// Recorre la lista enlazada atomicList; el EXE corta tambien si el enlace es el desplazamiento de inClumpLink (atomico nulo).
RwInt32 RpClumpGetNumAtomics(RpClump* clump) {
    static_assert(offsetof(RpAtomic, inClumpLink) == 0x40);
    RwInt32 numAtomics = 0;
    const RwLLLink* const end = &clump->atomicList.link;
    for (const RwLLLink* link = end->next; link != end; link = link->next) {
        ++numAtomics;
        if (reinterpret_cast<uintptr_t>(link) == offsetof(RpAtomic, inClumpLink)) {
            break;
        }
    }
    return numAtomics;
}

RwInt32 RpClumpGetNumLights(RpClump* clump) {
    return ((RwInt32(__cdecl *)(RpClump*))0x749910)(clump);
}

RwInt32 RpClumpGetNumCameras(RpClump* clump) {
    return ((RwInt32(__cdecl *)(RpClump*))0x749940)(clump);
}

RpClump* RpLightGetClump(const RpLight* light) {
    return ((RpClump*(__cdecl *)(const RpLight*))0x749E40)(light);
}

RpClump* RwCameraGetClump(const RwCamera* camera) {
    return ((RpClump*(__cdecl *)(const RwCamera*))0x749E50)(camera);
}

RwUInt32 RpAtomicStreamGetSize(RpAtomic* atomic) {
    return ((RwUInt32(__cdecl *)(RpAtomic*))0x74A5B0)(atomic);
}

RpAtomic* RpAtomicStreamRead(RwStream* stream) {
    return ((RpAtomic*(__cdecl *)(RwStream*))0x74B030)(stream);
}

RpAtomic* RpAtomicStreamWrite(RpAtomic* atomic, RwStream* stream) {
    return ((RpAtomic*(__cdecl *)(RpAtomic*, RwStream*))0x74A850)(atomic, stream);
}

RwUInt32 RpClumpStreamGetSize(RpClump* clump) {
    return ((RwUInt32(__cdecl *)(RpClump*))0x74A5E0)(clump);
}

// 0x74B420: lee un clump del flujo: chunk estructura (version 0x34000..0x36003, 12 bytes con numAtomics/numLights/numCameras),
// lista de marcos, lista de geometrias, atomicos, luces y camaras (cada luz/camara con su indice de marco) y datos de plugins.
// Cada fallo deshace lo ya creado en el orden del EXE y deja el error de RenderWare (0x8000001A, o 0x80000004 por version).
RpClump* RpClumpStreamRead(RwStream* stream) {
    RwUInt32 length;
    RwUInt32 version;
    if (!RwStreamFindChunk(stream, kChunkStruct, &length, &version)) {
        RaiseRwError(kErrRead);
        return nullptr;
    }
    if (version < kStreamVersionMin || version > kStreamVersionMax) {
        RaiseRwError(kErrBadVersion);
        return nullptr;
    }
    RpClumpChunkInfo info;
    if (RwStreamRead(stream, &info, 12) != 12) {
        RaiseRwError(kErrRead);
        return nullptr;
    }

    RpClump* const clump = RpClumpCreate();
    if (!clump) {
        return nullptr;
    }

    // Lista de marcos.
    RwUInt32 chunkVersion;
    rwFrameList frameList;
    if (!RwStreamFindChunk(stream, kChunkFrameList, nullptr, &chunkVersion)) {
        ClumpReadAbortBeforeFrames(clump);
        return nullptr;
    }
    if (!_rwFrameListStreamRead(stream, &frameList)) {
        ClumpReadAbortBeforeFrames(clump);
        return nullptr;
    }
    clump->object.parent = frameList.frames[0];
    if (!RwStreamFindChunk(stream, kChunkGeometryList, nullptr, &chunkVersion)) {
        _rwFrameListDeinitialize(&frameList);
        ClumpReadAbortBeforeFrames(clump);
        return nullptr;
    }

    // Lista de geometrias (chunk estructura con el numero de geometrias y despues cada geometria).
    RpGeometryList geomList;
    RwUInt32 geomListLength;
    RwUInt32 geomListVersion;
    if (!RwStreamFindChunk(stream, kChunkStruct, &geomListLength, &geomListVersion)) {
        ClumpReadAbortWithFrames(clump, &frameList);
        return nullptr;
    }
    if (geomListVersion < kStreamVersionMin || geomListVersion > kStreamVersionMax) {
        RaiseRwError(kErrBadVersion);
        ClumpReadAbortWithFrames(clump, &frameList);
        return nullptr;
    }
    RwInt32 count;
    if (RwStreamRead(stream, &count, 4) != 4) {
        ClumpReadAbortWithFrames(clump, &frameList);
        return nullptr;
    }
    geomList.numGeoms = 0;
    if (count > 0) {
        geomList.geometries = static_cast<RpGeometry**>(RwMalloc(static_cast<size_t>(count) * sizeof(RpGeometry*), kGeomListAllocHint));
        if (!geomList.geometries) {
            RwUInt32 requested = static_cast<RwUInt32>(count) * sizeof(RpGeometry*);
            RwError error;
            error.pluginID = kErrorPluginID;
            error.errorCode = _rwerror(kErrNoMem, requested);
            RwErrorSet(&error);
            ClumpReadAbortWithFrames(clump, &frameList);
            return nullptr;
        }
    } else {
        geomList.geometries = nullptr;
    }
    for (RwInt32 i = 0; i < count; ++i) {
        RwUInt32 geometryVersion;
        if (!RwStreamFindChunk(stream, kChunkGeometry, nullptr, &geometryVersion)) {
            ClumpReadAbortWithGeometries(clump, &frameList, &geomList);
            return nullptr;
        }
        if (geometryVersion < kStreamVersionMin || geometryVersion > kStreamVersionMax) {
            ClumpReadAbortWithGeometries(clump, &frameList, &geomList);
            return nullptr;
        }
        geomList.geometries[i] = RpGeometryStreamRead(stream);
        if (!geomList.geometries[i]) {
            ClumpReadAbortWithGeometries(clump, &frameList, &geomList);
            return nullptr;
        }
        ++geomList.numGeoms;
    }

    // Atomicos.
    for (RwInt32 i = 0; i < info.numAtomics; ++i) {
        RpAtomic* atomic = nullptr;
        if (RwStreamFindChunk(stream, kChunkAtomic, nullptr, &chunkVersion)) {
            atomic = ClumpAtomicStreamRead(stream, &frameList, &geomList);
        }
        if (!atomic) {
            ClumpReadAbortWithGeometries(clump, &frameList, &geomList);
            return nullptr;
        }
        RpClumpAddAtomic(clump, atomic);
    }

    // Luces: chunk estructura con el indice de marco, chunk de luz y enlace en la lista del clump.
    for (RwInt32 i = 0; i < info.numLights; ++i) {
        RwInt32 frameIndex;
        RpLight* light = nullptr;
        if (RwStreamFindChunk(stream, kChunkStruct, nullptr, nullptr) &&
            RwStreamReadInt32(stream, &frameIndex, 4) &&
            RwStreamFindChunk(stream, kChunkLight, nullptr, nullptr)) {
            light = RpLightStreamRead(stream);
        }
        if (!light) {
            ClumpReadAbortWithGeometries(clump, &frameList, &geomList);
            return nullptr;
        }
        _rwObjectHasFrameSetFrame(light, frameList.frames[frameIndex]);
        AttachListEntry(ListEntryOf(light, s_lightClumpOffset), clump, &clump->lightList);
    }

    // Camaras: igual que las luces con la lista de camaras del clump.
    for (RwInt32 i = 0; i < info.numCameras; ++i) {
        RwInt32 frameIndex;
        RwCamera* camera = nullptr;
        if (RwStreamFindChunk(stream, kChunkStruct, nullptr, nullptr) &&
            RwStreamReadInt32(stream, &frameIndex, 4) &&
            RwStreamFindChunk(stream, kChunkCamera, nullptr, nullptr)) {
            camera = RwCameraStreamRead(stream);
        }
        if (!camera) {
            ClumpReadAbortWithGeometries(clump, &frameList, &geomList);
            return nullptr;
        }
        _rwObjectHasFrameSetFrame(camera, frameList.frames[frameIndex]);
        AttachListEntry(ListEntryOf(camera, s_cameraClumpOffset), clump, &clump->cameraList);
    }

    _rpGeometryListDeinitialize(&geomList);
    _rwFrameListDeinitialize(&frameList);
    if (!_rwPluginRegistryReadDataChunks(&s_clumpTKList, stream, clump)) {
        RpClumpDestroy(clump);
        RaiseRwError(kErrRead);
        return nullptr;
    }
    return clump;
}

RpClump* RpClumpStreamWrite(RpClump* clump, RwStream* stream) {
    return ((RpClump*(__cdecl *)(RpClump*, RwStream*))0x74AA10)(clump, stream);
}

RpClumpChunkInfo* _rpClumpChunkInfoRead(RwStream* stream, RpClumpChunkInfo* clumpChunkInfo, RwInt32* bytesRead) {
    return ((RpClumpChunkInfo*(__cdecl *)(RwStream*, RpClumpChunkInfo*, RwInt32*))0x74BD40)(stream, clumpChunkInfo, bytesRead);
}

// 0x74BDA0: registra el plugin en la lista de RpAtomic (0x8D624C) con _rwPluginRegistryAddPlugin.
RwInt32 RpAtomicRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return _rwPluginRegistryAddPlugin(&s_atomicTKList, size, pluginID, constructCB, destructCB, copyCB);
}

// 0x74BDD0: igual sobre la lista de RpClump (0x8D6264).
RwInt32 RpClumpRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return _rwPluginRegistryAddPlugin(&s_clumpTKList, size, pluginID, constructCB, destructCB, copyCB);
}

// 0x74BE00: callbacks de flujo (_rwPluginRegistryAddPluginStream) sobre la lista de RpAtomic.
RwInt32 RpAtomicRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return _rwPluginRegistryAddPluginStream(&s_atomicTKList, pluginID, readCB, writeCB, getSizeCB);
}

// 0x74BE30
RwInt32 RpAtomicSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return _rwPluginRegistryAddPlgnStrmlwysCB(&s_atomicTKList, pluginID, alwaysCB);
}

// 0x74BE50
RwInt32 RpAtomicSetStreamRightsCallBack(RwUInt32 pluginID, RwPluginDataChunkRightsCallBack rightsCB) {
    return _rwPluginRegistryAddPlgnStrmRightsCB(&s_atomicTKList, pluginID, rightsCB);
}

// 0x74BE70: igual sobre la lista de RpClump.
RwInt32 RpClumpRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return _rwPluginRegistryAddPluginStream(&s_clumpTKList, pluginID, readCB, writeCB, getSizeCB);
}

RwInt32 RpClumpSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkAlwaysCallBack))0x74BEA0)(pluginID, alwaysCB);
}

// 0x74BEC0
RwInt32 RpAtomicGetPluginOffset(RwUInt32 pluginID) {
    return _rwPluginRegistryGetPluginOffset(&s_atomicTKList, pluginID);
}

RwInt32 RpClumpGetPluginOffset(RwUInt32 pluginID) {
    return ((RwInt32(__cdecl *)(RwUInt32))0x74BEE0)(pluginID);
}

RwBool RpAtomicValidatePlugins(const RpAtomic* atomic) {
    return ((RwBool(__cdecl *)(const RpAtomic*))0x74BF00)(atomic);
}

RwBool RpClumpValidatePlugins(const RpClump* clump) {
    return ((RwBool(__cdecl *)(const RpClump*))0x74BF10)(clump);
}

RwBool RpAtomicInstance(RpAtomic* atomic) {
    return ((RwBool(__cdecl *)(RpAtomic*))0x74BF40)(atomic);
}

RwBool _rpWorldFindBBox(RpWorld* world, RwBBox* boundingBox) {
    return ((RwBool(__cdecl *)(RpWorld*, RwBBox*))0x74EFA0)(world, boundingBox);
}

RpWorld* _rpWorldSetupSectorBoundingBoxes(RpWorld* world) {
    return ((RpWorld*(__cdecl *)(RpWorld*))0x74F020)(world);
}

RwBool _rpWorldForAllGlobalLights(RpLightCallBack callBack, void* data) {
    return ((RwBool(__cdecl *)(RpLightCallBack, void*))0x74EF10)(callBack, data);
}

RpWorldSector* _rpWorldSectorForAllLocalLights(RpWorldSector* sector, RpLightCallBack callBack, void* data) {
    return ((RpWorldSector*(__cdecl *)(RpWorldSector*, RpLightCallBack, void*))0x74EF60)(sector, callBack, data);
}

RpWorld* RpWorldLock(RpWorld* world) {
    return ((RpWorld*(__cdecl *)(RpWorld*))0x74F1A0)(world);
}

void _rpWorldRegisterWorld(RpWorld* world, RwUInt32 memorySize) {
    ((void(__cdecl *)(RpWorld*, RwUInt32))0x74F0C0)(world, memorySize);
}

void _rpWorldUnregisterWorld(RpWorld* world) {
    ((void(__cdecl *)(RpWorld*))0x74F140)(world);
}

RwBool RpWorldPluginAttach() {
    return ((RwBool(__cdecl *)(void))0x74FDA0)();
}

RpWorld* RpWorldForAllClumps(RpWorld* world, RpClumpCallBack fpCallBack, void* data) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RpClumpCallBack, void*))0x74FB80)(world, fpCallBack, data);
}

RpWorld* RpWorldForAllLights(RpWorld* world, RpLightCallBack fpCallBack, void* data) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RpLightCallBack, void*))0x74FC00)(world, fpCallBack, data);
}

RpWorld* RpWorldRender(RpWorld* world) {
    return ((RpWorld*(__cdecl *)(RpWorld*))0x74F570)(world);
}

RpWorld* RpWorldSetSectorRenderCallBack(RpWorld* world, RpWorldSectorCallBackRender fpCallBack) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RpWorldSectorCallBackRender))0x74F730)(world, fpCallBack);
}

RpWorldSectorCallBackRender RpWorldGetSectorRenderCallBack(const RpWorld* world) {
    return ((RpWorldSectorCallBackRender(__cdecl *)(const RpWorld*))0x74F750)(world);
}

// 0x74FCD0
RwInt32 RpWorldRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return _rwPluginRegistryAddPlugin(&s_worldTKList, size, pluginID, constructCB, destructCB, copyCB);
}

// 0x74FD00
RwInt32 RpWorldRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return _rwPluginRegistryAddPluginStream(&s_worldTKList, pluginID, readCB, writeCB, getSizeCB);
}

RwInt32 RpWorldSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkAlwaysCallBack))0x74FD30)(pluginID, alwaysCB);
}

RwInt32 RpWorldSetStreamRightsCallBack(RwUInt32 pluginID, RwPluginDataChunkRightsCallBack rightsCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkRightsCallBack))0x74FD50)(pluginID, rightsCB);
}

RwInt32 RpWorldGetPluginOffset(RwUInt32 pluginID) {
    return ((RwInt32(__cdecl *)(RwUInt32))0x74FD70)(pluginID);
}

RwBool RpWorldValidatePlugins(RpWorld* world) {
    return ((RwBool(__cdecl *)(RpWorld*))0x74FD90)(world);
}

void RpD3D9GeometrySetUsageFlags(RpGeometry* geometry, RpD3D9GeometryUsageFlag flags) {
    ((void(__cdecl *)(RpGeometry*, RpD3D9GeometryUsageFlag))0x7588B0)(geometry, flags);
}

// 0x7588D0
RpD3D9GeometryUsageFlag RpD3D9GeometryGetUsageFlags(const RpGeometry* geometry) {
    return *reinterpret_cast<const RpD3D9GeometryUsageFlag*>(reinterpret_cast<uintptr_t>(geometry) + static_cast<uintptr_t>(s_geometryUsageFlagsOffset));
}

// 0x7588E0
void RpD3D9WorldSectorSetUsageFlags(RpWorldSector* sector, RpD3D9WorldSectorUsageFlag flags) {
    *reinterpret_cast<RpD3D9WorldSectorUsageFlag*>(reinterpret_cast<uintptr_t>(sector) + static_cast<uintptr_t>(s_sectorUsageFlagsOffset)) = flags;
}

// 0x758900
RpD3D9WorldSectorUsageFlag RpD3D9WorldSectorGetUsageFlags(const RpWorldSector* sector) {
    return *reinterpret_cast<const RpD3D9WorldSectorUsageFlag*>(reinterpret_cast<uintptr_t>(sector) + static_cast<uintptr_t>(s_sectorUsageFlagsOffset));
}

void RxD3D9AllInOneSetInstanceCallBack(RxPipelineNode* node, RxD3D9AllInOneInstanceCallBack callback) {
    ((void(__cdecl *)(RxPipelineNode*, RxD3D9AllInOneInstanceCallBack))0x757380)(node, callback);
}

RxD3D9AllInOneInstanceCallBack RxD3D9AllInOneGetInstanceCallBack(RxPipelineNode* node) {
    return ((RxD3D9AllInOneInstanceCallBack(__cdecl *)(RxPipelineNode*))0x757390)(node);
}

void RxD3D9AllInOneSetReinstanceCallBack(RxPipelineNode* node, RxD3D9AllInOneReinstanceCallBack callback) {
    ((void(__cdecl *)(RxPipelineNode*, RxD3D9AllInOneReinstanceCallBack))0x7573A0)(node, callback);
}

RxD3D9AllInOneReinstanceCallBack RxD3D9AllInOneGetReinstanceCallBack(RxPipelineNode* node) {
    return ((RxD3D9AllInOneReinstanceCallBack(__cdecl *)(RxPipelineNode*))0x7573B0)(node);
}

void RxD3D9AllInOneSetLightingCallBack(RxPipelineNode* node, RxD3D9AllInOneLightingCallBack callback) {
    ((void(__cdecl *)(RxPipelineNode*, RxD3D9AllInOneLightingCallBack))0x7573C0)(node, callback);
}

RxD3D9AllInOneLightingCallBack RxD3D9AllInOneGetLightingCallBack(RxPipelineNode* node) {
    return ((RxD3D9AllInOneLightingCallBack(__cdecl *)(RxPipelineNode*))0x7573D0)(node);
}

void RxD3D9AllInOneSetRenderCallBack(RxPipelineNode* node, RxD3D9AllInOneRenderCallBack callback) {
    ((void(__cdecl *)(RxPipelineNode*, RxD3D9AllInOneRenderCallBack))0x7573E0)(node, callback);
}

RxD3D9AllInOneRenderCallBack RxD3D9AllInOneGetRenderCallBack(RxPipelineNode* node) {
    return ((RxD3D9AllInOneRenderCallBack(__cdecl *)(RxPipelineNode*))0x7573F0)(node);
}

// 0x7522E0
// Tabla de 17 DWORD en .rdata (0x874EF8) con el tamano en bytes de cada D3DDECLTYPE (FLOAT1=4 ... FLOAT16_4=8); el EXE no comprueba el rango.
RwUInt32 _rpD3D9VertexDeclarationGetSize(RwUInt32 type) {
    const auto* const sizes = reinterpret_cast<const RwUInt32*>(0x874EF8);
    return sizes[type];
}

RwUInt32 _rpD3D9VertexDeclarationGetStride(const void* vertexDeclaration) {
    return ((RwUInt32(__cdecl *)(const void*))0x7522F0)(vertexDeclaration);
}

RwUInt32 _rpD3D9VertexDeclarationInstV3d(RwUInt32 type, RwUInt8* mem, const RwV3d* src, RwInt32 numVerts, RwUInt32 stride) {
    return ((RwUInt32(__cdecl *)(RwUInt32, RwUInt8*, const RwV3d*, RwInt32, RwUInt32))0x752AD0)(type, mem, src, numVerts, stride);
}

RwUInt32 _rpD3D9VertexDeclarationInstV3dComp(RwUInt32 type, RwUInt8* mem, const RpVertexNormal* src, RwInt32 numVerts, RwUInt32 stride) {
    return ((RwUInt32(__cdecl *)(RwUInt32, RwUInt8*, const RpVertexNormal*, RwInt32, RwUInt32))0x7531B0)(type, mem, src, numVerts, stride);
}

RwUInt32 _rpD3D9VertexDeclarationInstV3dMorph(RwUInt32 type, RwUInt8* mem, const RwV3d* src1, const RwV3d* src2, RwReal scale, RwInt32 numVerts, RwUInt32 stride) {
    return ((RwUInt32(__cdecl *)(RwUInt32, RwUInt8*, const RwV3d*, const RwV3d*, RwReal, RwInt32, RwUInt32))0x753B60)(type, mem, src1, src2, scale, numVerts, stride);
}

RwUInt32 _rpD3D9VertexDeclarationInstV2d(RwUInt32 type, RwUInt8* mem, const RwV2d* src, RwInt32 numVerts, RwUInt32 stride) {
    return ((RwUInt32(__cdecl *)(RwUInt32, RwUInt8*, const RwV2d*, RwInt32, RwUInt32))0x7544E0)(type, mem, src, numVerts, stride);
}

RwBool _rpD3D9VertexDeclarationInstColor(RwUInt8* mem, const RwRGBA* color, RwInt32 numVerts, RwUInt32 stride) {
    return ((RwBool(__cdecl *)(RwUInt8*, const RwRGBA*, RwInt32, RwUInt32))0x754AE0)(mem, color, numVerts, stride);
}

RwUInt32 _rpD3D9VertexDeclarationInstIndices(RwUInt32 type, RwUInt8* mem, const RwUInt32* indices, RwInt32 numVerts, RwUInt32 stride) {
    return ((RwUInt32(__cdecl *)(RwUInt32, RwUInt8*, const RwUInt32*, RwInt32, RwUInt32))0x754B40)(type, mem, indices, numVerts, stride);
}

RwUInt32 _rpD3D9VertexDeclarationInstIndicesRemap(RwUInt32 type, RwUInt8* mem, const RwUInt32* indices, const RwUInt8* remap, RwInt32 numVerts, RwUInt32 stride) {
    return ((RwUInt32(__cdecl *)(RwUInt32, RwUInt8*, const RwUInt32*, const RwUInt8*, RwInt32, RwUInt32))0x754C80)(type, mem, indices, remap, numVerts, stride);
}

RwUInt32 _rpD3D9VertexDeclarationInstWeights(RwUInt32 type, RwUInt8* mem, const RwV4d* src, RwInt32 numVerts, RwUInt32 stride) {
    return ((RwUInt32(__cdecl *)(RwUInt32, RwUInt8*, const RwV4d*, RwInt32, RwUInt32))0x752320)(type, mem, src, numVerts, stride);
}

RwUInt32 _rpD3D9VertexDeclarationInstTangent(RwUInt32 type, RwUInt8* mem, const RwV3d* pos, const RwTexCoords* texCoord, RxD3D9ResEntryHeader* meshHeader, RwUInt32 stride) {
    return ((RwUInt32(__cdecl *)(RwUInt32, RwUInt8*, const RwV3d*, const RwTexCoords*, RxD3D9ResEntryHeader*, RwUInt32))0x754E20)(type, mem, pos, texCoord, meshHeader, stride);
}

void _rpD3D9VertexDeclarationUnInstV3d(RwUInt32 type, RwV3d* dst, const RwUInt8* src) {
    ((void(__cdecl *)(RwUInt32, RwV3d*, const RwUInt8*))0x7551F0)(type, dst, src);
}

void _rpD3D9VertexDeclarationUnInstV2d(RwUInt32 type, RwV2d* dst, const RwUInt8* src) {
    ((void(__cdecl *)(RwUInt32, RwV2d*, const RwUInt8*))0x7555E0)(type, dst, src);
}

void _rpD3D9GetMinMaxValuesV3d(const RwV3d* src, RwInt32 numElements, RwV3d* min, RwV3d* max) {
    ((void(__cdecl *)(const RwV3d*, RwInt32, RwV3d*, RwV3d*))0x755830)(src, numElements, min, max);
}

void _rpD3D9GetMinMaxValuesV2d(const RwV2d* src, RwInt32 numElements, RwV2d* min, RwV2d* max) {
    ((void(__cdecl *)(const RwV2d*, RwInt32, RwV2d*, RwV2d*))0x7558F0)(src, numElements, min, max);
}

RwUInt32 _rpD3D9FindFormatV3d(const RwV3d* src, RwInt32 numElements) {
    return ((RwUInt32(__cdecl *)(const RwV3d*, RwInt32))0x755980)(src, numElements);
}

RwUInt32 _rpD3D9FindFormatV2d(const RwV2d* src, RwInt32 numElements) {
    return ((RwUInt32(__cdecl *)(const RwV2d*, RwInt32))0x755AA0)(src, numElements);
}

RxPipeline* RpWorldSetDefaultSectorPipeline(RxPipeline* pipeline) {
    return ((RxPipeline*(__cdecl *)(RxPipeline*))0x75E270)(pipeline);
}

RxPipeline* RpAtomicSetDefaultPipeline(RxPipeline* pipeline) {
    return ((RxPipeline*(__cdecl *)(RxPipeline*))0x75E2A0)(pipeline);
}

RxPipeline* RpMaterialSetDefaultPipeline(RxPipeline* pipeline) {
    return ((RxPipeline*(__cdecl *)(RxPipeline*))0x75E2D0)(pipeline);
}

const RpGeometry* RpGeometryIsCorrectlySorted(const RpGeometry* geometry, RwBool* result) {
    return ((const RpGeometry*(__cdecl *)(const RpGeometry*, RwBool*))0x75D9D0)(geometry, result);
}

RpGeometry* RpGeometrySortByMaterial(const RpGeometry* geometry, RpGeometrySortByMaterialCallBack callback) {
    return ((RpGeometry*(__cdecl *)(const RpGeometry*, RpGeometrySortByMaterialCallBack))0x75DAE0)(geometry, callback);
}

void RpTieSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x74FF60)(blockSize, numBlocksToPrealloc);
}

void RpLightTieSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x74FF80)(blockSize, numBlocksToPrealloc);
}

RpWorld* RpWorldRemoveCamera(RpWorld* world, RwCamera* camera) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RwCamera*))0x750F50)(world, camera);
}

RpWorld* RpWorldAddCamera(RpWorld* world, RwCamera* camera) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RwCamera*))0x750F20)(world, camera);
}

RpWorld* RwCameraGetWorld(const RwCamera* camera) {
    return ((RpWorld*(__cdecl *)(const RwCamera*))0x750F80)(camera);
}

RpWorld* RpWorldRemoveAtomic(RpWorld* world, RpAtomic* atomic) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RpAtomic*))0x750FC0)(world, atomic);
}

RpWorld* RpWorldAddAtomic(RpWorld* world, RpAtomic* atomic) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RpAtomic*))0x750F90)(world, atomic);
}

// 0x751050
RpWorld* RpAtomicGetWorld(const RpAtomic* atomic) {
    return *reinterpret_cast<RpWorld* const*>(reinterpret_cast<uintptr_t>(atomic) + static_cast<uintptr_t>(s_atomicWorldOffset));
}

RpWorld* RpWorldAddClump(RpWorld* world, RpClump* clump) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RpClump*))0x751300)(world, clump);
}

RpWorld* RpWorldRemoveClump(RpWorld* world, RpClump* clump) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RpClump*))0x751460)(world, clump);
}

RpWorld* RpClumpGetWorld(const RpClump* clump) {
    return ((RpWorld*(__cdecl *)(const RpClump*))0x7516B0)(clump);
}

RpWorld* RpWorldAddLight(RpWorld* world, RpLight* light) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RpLight*))0x751910)(world, light);
}

RpWorld* RpWorldRemoveLight(RpWorld* world, RpLight* light) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RpLight*))0x751960)(world, light);
}

RpWorld* RpLightGetWorld(const RpLight* light) {
    return ((RpWorld*(__cdecl *)(const RpLight*))0x7519E0)(light);
}

RwCamera* RwCameraForAllClumpsInFrustum(RwCamera* camera, void* data) {
    return ((RwCamera*(__cdecl *)(RwCamera*, void*))0x7516C0)(camera, data);
}

RwCamera* RwCameraForAllAtomicsInFrustum(RwCamera* camera, RpAtomicCallBack callback, void* data) {
    return ((RwCamera*(__cdecl *)(RwCamera*, RpAtomicCallBack, void*))0x7517F0)(camera, callback, data);
}

RwCamera* RwCameraForAllSectorsInFrustum(RwCamera* camera, RpWorldSectorCallBack callBack, void* data) {
    return ((RwCamera*(__cdecl *)(RwCamera*, RpWorldSectorCallBack, void*))0x751660)(camera, callBack, data);
}

RpLight* RpLightForAllWorldSectors(RpLight* light, RpWorldSectorCallBack callback, void* data) {
    return ((RpLight*(__cdecl *)(RpLight*, RpWorldSectorCallBack, void*))0x7519F0)(light, callback, data);
}

RpAtomic* RpAtomicForAllWorldSectors(RpAtomic* atomic, RpWorldSectorCallBack callback, void* data) {
    return ((RpAtomic*(__cdecl *)(RpAtomic*, RpWorldSectorCallBack, void*))0x751060)(atomic, callback, data);
}

RpWorldSector* RpWorldSectorForAllAtomics(RpWorldSector* sector, RpAtomicCallBack callback, void* data) {
    return ((RpWorldSector*(__cdecl *)(RpWorldSector*, RpAtomicCallBack, void*))0x7510A0)(sector, callback, data);
}

RpWorldSector* RpWorldSectorForAllCollisionAtomics(RpWorldSector* sector, RpAtomicCallBack callback, void* data) {
    return ((RpWorldSector*(__cdecl *)(RpWorldSector*, RpAtomicCallBack, void*))0x751140)(sector, callback, data);
}

RpWorldSector* RpWorldSectorForAllLights(RpWorldSector* sector, RpLightCallBack callback, void* data) {
    return ((RpWorldSector*(__cdecl *)(RpWorldSector*, RpLightCallBack, void*))0x7511E0)(sector, callback, data);
}

RwUInt32 RpWorldStreamGetSize(const RpWorld* world) {
    return ((RwUInt32(__cdecl *)(const RpWorld*))0x761EA0)(world);
}

RpWorld* RpWorldStreamRead(RwStream* stream) {
    return ((RpWorld*(__cdecl *)(RwStream*))0x762960)(stream);
}

const RpWorld* RpWorldStreamWrite(const RpWorld* world, RwStream* stream) {
    return ((const RpWorld*(__cdecl *)(const RpWorld*, RwStream*))0x762150)(world, stream);
}

RpWorldSectorChunkInfo* _rpWorldSectorChunkInfoRead(RwStream* stream, RpWorldSectorChunkInfo* worldSectorChunkInfo, RwInt32* bytesRead) {
    return ((RpWorldSectorChunkInfo*(__cdecl *)(RwStream*, RpWorldSectorChunkInfo*, RwInt32*))0x7635B0)(stream, worldSectorChunkInfo, bytesRead);
}

RpPlaneSectorChunkInfo* _rpPlaneSectorChunkInfoRead(RwStream* stream, RpPlaneSectorChunkInfo* planeSectorChunkInfo, RwInt32* bytesRead) {
    return ((RpPlaneSectorChunkInfo*(__cdecl *)(RwStream*, RpPlaneSectorChunkInfo*, RwInt32*))0x763620)(stream, planeSectorChunkInfo, bytesRead);
}

RpWorldChunkInfo* _rpWorldChunkInfoRead(RwStream* stream, RpWorldChunkInfo* worldChunkInfo, RwInt32* bytesRead) {
    return ((RpWorldChunkInfo*(__cdecl *)(RwStream*, RpWorldChunkInfo*, RwInt32*))0x763690)(stream, worldChunkInfo, bytesRead);
}


/****************************************************************************
_rwD3D9EnableClippingIfNeeded
Purpose:
On entry:
On exit :
*/
void
_rwD3D9EnableClippingIfNeeded(void *object,
    RwUInt32 type)
{
    RwCamera    *camera;
    RwBool      fullyInsideFrustum;

    RWFUNCTION(RWSTRING("_rwD3D9EnableClippingIfNeeded"));
    RWASSERT(object != NULL);

    camera = RwCameraGetCurrentCamera();
    RWASSERT(camera != NULL);

    if (type == rpATOMIC)
    {
        RpAtomic    *atomic;

        atomic = (RpAtomic *)object;

        fullyInsideFrustum =
            RwD3D9CameraIsSphereFullyInsideFrustum(camera,
                RpAtomicGetWorldBoundingSphere(atomic));
    }
    else
    {
        const RpWorldSector   *worldSector;

        RWASSERT(type == ((RwUInt8)rwSECTORATOMIC));

        worldSector = (const RpWorldSector *)object;

        fullyInsideFrustum =
            RwD3D9CameraIsBBoxFullyInsideFrustum(camera,
                RpWorldSectorGetTightBBox(worldSector));
    }

    RwD3D9SetRenderState(D3DRS_CLIPPING, (fullyInsideFrustum == FALSE));

    RWRETURNVOID();
}

// Registro de los hooks del lote 227A (RpClump y registros de plugins de RpAtomic/RpClump), sin validar.
void RpWorldInjectHooks227A() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpWorld");

    RH_ScopedGlobalInstall(RpClumpClone, 0x749F70);
    RH_ScopedGlobalInstall(ClumpLightDestroyCallBack, 0x74A1C0);
    RH_ScopedGlobalInstall(RpClumpCreate, 0x74A290);
    RH_ScopedGlobalInstall(RpClumpDestroy, 0x74A310);
    RH_ScopedGlobalInstall(RpClumpAddAtomic, 0x74A490);
    RH_ScopedGlobalInstall(RpClumpRemoveAtomic, 0x74A4C0);
    RH_ScopedGlobalInstall(RpClumpRemoveLight, 0x74A520);
    RH_ScopedGlobalInstall(_rpGeometryListDeinitialize, 0x74AE50);
    RH_ScopedGlobalInstall(ClumpAtomicStreamRead, 0x74B060);
    RH_ScopedGlobalInstall(RpClumpStreamRead, 0x74B420);
    RH_ScopedGlobalInstall(ClumpAtomicDestroyCallBack, 0x74BCC0);
    RH_ScopedGlobalInstall(RpAtomicRegisterPlugin, 0x74BDA0);
    RH_ScopedGlobalInstall(RpClumpRegisterPlugin, 0x74BDD0);
    RH_ScopedGlobalInstall(RpAtomicRegisterPluginStream, 0x74BE00);
    RH_ScopedGlobalInstall(RpClumpRegisterPluginStream, 0x74BE70);
}

// ---------------------------------------------------------------------------------------------------------------------
// Lote 229A (sin validar): lista de materiales (bamatlst), geometrias dinamicas de D3D9, tuberias por defecto del modulo de
// mundos, sectores de BSP (liberacion, desinstanciado, render por defecto) y el objeto RpWorld (crear, destruir, desbloquear,
// iteradores). Todo el codigo sigue el desensamblado del EXE (0x74E010..0x74FC70); no hay banco: el coordinador valida despues.
// Las funciones del EXE que no se reconstruyen aqui (ataduras 0x74FFA0/0x74FFF0, plugins 0x755B90/0x7587D0, 0x760CF0, el
// callback 0x74EA80) se llaman por direccion fija.
// ---------------------------------------------------------------------------------------------------------------------
namespace {

static auto& s_sectorTKList229     = StaticRef<RwPluginRegistry>(0x8D736C); // registro de plugins de RpWorldSector
static auto& s_dynamicGeometryHead = StaticRef<void*>(0xC92520);            // cabeza de la lista de geometrias dinamicas de D3D9
static auto& s_dynamicGeometryPool = StaticRef<RwFreeList*>(0xC92524);      // lista libre de sus nodos (8 bytes)
static auto& s_prevRestoreCallBack = StaticRef<rwD3D9DeviceRestoreCallBack>(0xC9251C); // callback de restauracion que habia antes de 0x74EA40
static auto& s_worldModuleOffset   = StaticRef<RwInt32>(0xC9254C);          // globalsOffset del modulo de mundos dentro de RwEngineInstance
static auto& s_pipelineModuleOffset = StaticRef<RwInt32>(0xC9BC60);         // globalsOffset del modulo de tuberias por defecto

constexpr RwUInt32 kChunkMaterial = 0x07;
constexpr RwUInt32 kMatListAllocHint = 0x1030008;
constexpr RwUInt32 kMatListIndexHint = 0x10501;
constexpr RwUInt32 kWorldAllocHint = 0x3000B;
constexpr RwUInt32 kWorldListEntryHint = 0x40507;
constexpr RwUInt32 kDynamicGeometryHint = 0x3050D;
constexpr uintptr_t kSectorDefaultRenderCallBackAddress = 0x74EEC0; // el EXE guarda ESTA direccion en RpWorld::renderCallBack
constexpr uintptr_t kD3D9RestoreCallBackAddress = 0x74EA80;         // callback de restauracion del dispositivo (sin reconstruir)

// Posiciones dentro de las globales del modulo de tuberias (RwEngineInstance + [0xC9BC60] + desplazamiento).
constexpr uint32 kPipelineSlotDefaultSector = 0x40;
constexpr uint32 kPipelineSlotPlatformAtomic = 0x54;
constexpr uint32 kPipelineSlotPlatformSector = 0x58;

static_assert(offsetof(RwGlobals, curWorld) == 4);
static_assert(offsetof(RpMaterial, refCount) == 0x18 && offsetof(RpMaterial, pipeline) == 8);
static_assert(offsetof(RpMaterialList, numMaterials) == 4 && offsetof(RpMaterialList, space) == 8);
static_assert(offsetof(RpWorld, flags) == 0x08 && offsetof(RpWorld, renderOrder) == 0x0C && offsetof(RpWorld, matList) == 0x10);
static_assert(offsetof(RpWorld, rootSector) == 0x1C && offsetof(RpWorld, numTexCoordSets) == 0x20 && offsetof(RpWorld, numClumpsInWorld) == 0x24);
static_assert(offsetof(RpWorld, currentClumpLink) == 0x28 && offsetof(RpWorld, clumpList) == 0x2C && offsetof(RpWorld, lightList) == 0x34);
static_assert(offsetof(RpWorld, directionalLightList) == 0x3C && offsetof(RpWorld, worldOrigin) == 0x44 && offsetof(RpWorld, boundingBox) == 0x50);
static_assert(offsetof(RpWorld, renderCallBack) == 0x68 && offsetof(RpWorld, pipeline) == 0x6C);
static_assert(offsetof(RpWorldSector, triangles) == 4 && offsetof(RpWorldSector, vertices) == 8 && offsetof(RpWorldSector, normals) == 0xC);
static_assert(offsetof(RpWorldSector, texCoords) == 0x10 && rwMAXTEXTURECOORDS == 8 && offsetof(RpWorldSector, preLitLum) == 0x30);
static_assert(offsetof(RpWorldSector, repEntry) == 0x34 && offsetof(RpWorldSector, collAtomicsInWorldSector) == 0x38 && offsetof(RpWorldSector, lightsInWorldSector) == 0x40);
static_assert(offsetof(RpWorldSector, boundingBox) == 0x48 && offsetof(RpWorldSector, tightBoundingBox) == 0x60 && offsetof(RpWorldSector, mesh) == 0x78);
static_assert(offsetof(RpWorldSector, pipeline) == 0x7C && offsetof(RpWorldSector, matListWindowBase) == 0x80 && offsetof(RpWorldSector, numVertices) == 0x82);
static_assert(offsetof(RpWorldSector, numTriangles) == 0x84);
static_assert(offsetof(RpPlaneSector, leftSubTree) == 8 && offsetof(RpPlaneSector, rightSubTree) == 0xC);
static_assert(sizeof(RpTriangle) == 8 && offsetof(RpTriangle, matIndex) == 6);
static_assert(offsetof(RwTexture, raster) == 0);

// Nodo de la lista de geometrias dinamicas de D3D9 (se enlaza por `next`).
struct DynamicGeometryNode {
    RpGeometry*          geometry;
    DynamicGeometryNode* next;
};
static_assert(sizeof(DynamicGeometryNode) == 8);

// Globales del modulo de mundos dentro de RwEngineInstance: lista libre de entradas (+0) y lista de mundos registrados (+4).
struct WorldModuleGlobals {
    RwFreeList* entryFreeList;
    RwLinkList  worldList;
};
static_assert(offsetof(WorldModuleGlobals, worldList) == 4);

// Entrada de la lista de mundos (16 bytes): {mundo, bytes de la asignacion unica, enlace}.
struct WorldListEntry {
    RpWorld*   world;
    RwUInt32   memorySize;
    RwLLLink   link;
};
static_assert(sizeof(WorldListEntry) == 0x10 && offsetof(WorldListEntry, link) == 8);

inline WorldModuleGlobals& WorldGlobals() {
    return *reinterpret_cast<WorldModuleGlobals*>(reinterpret_cast<uintptr_t>(RwEngineInstance) + static_cast<uintptr_t>(s_worldModuleOffset));
}
inline WorldListEntry* EntryOfWorldLink(RwLLLink* link) {
    return reinterpret_cast<WorldListEntry*>(reinterpret_cast<uintptr_t>(link) - offsetof(WorldListEntry, link));
}
inline RxPipeline*& PipelineModuleSlot(uint32 offset) {
    return *reinterpret_cast<RxPipeline**>(reinterpret_cast<uintptr_t>(RwEngineInstance) + static_cast<uintptr_t>(s_pipelineModuleOffset) + offset);
}

// Funciones del EXE llamadas por direccion fija (no se reconstruyen en este lote).
inline RwInt32 TieDestroyExe(RwLLLink* link)      { return reinterpret_cast<RwInt32(__cdecl*)(RwLLLink*)>(static_cast<uintptr_t>(0x74FFF0))(link); } // ata de atomo de un sector
inline RwInt32 LightTieDestroyExe(RwLLLink* link) { return reinterpret_cast<RwInt32(__cdecl*)(RwLLLink*)>(static_cast<uintptr_t>(0x74FFA0))(link); } // ata de luz de un sector
inline RwBool  D3D9LightPluginAttachExe()         { return reinterpret_cast<RwBool(__cdecl*)()>(static_cast<uintptr_t>(0x755B90))(); } // registra el plugin de luces (id 0x505)
inline RwBool  D3D9GeometryPluginAttachExe()      { return reinterpret_cast<RwBool(__cdecl*)()>(static_cast<uintptr_t>(0x7587D0))(); } // registra los plugins de geometria/sector (id 0x50F)
inline void    D3D9VertexShaderOpenExe()          { reinterpret_cast<void(__cdecl*)()>(static_cast<uintptr_t>(0x760CF0))(); }          // inicializa la cache de shaders de vertices

// Libera los recursos de instancia, las ataduras (atomos y luces) y los plugins de un sector con atomos (parte comun de 0x74ECA0 y 0x74ED50).
void WorldSectorReleaseInstance(RpWorldSector* worldSector) {
    if (worldSector->repEntry) {
        RwResourcesFreeResEntry(worldSector->repEntry);
    }
    RwLLLink* const atomicsEnd = &worldSector->collAtomicsInWorldSector.link;
    for (RwLLLink* link = atomicsEnd->next; link != atomicsEnd;) {
        RwLLLink* const current = link;
        link = link->next;
        TieDestroyExe(current);
    }
    RwLLLink* const lightsEnd = &worldSector->lightsInWorldSector.link;
    for (RwLLLink* link = lightsEnd->next; link != lightsEnd;) {
        RwLLLink* const current = link;
        link = link->next;
        LightTieDestroyExe(current);
    }
    _rwPluginRegistryDeInitObject(&s_sectorTKList229, worldSector);
}

// Quita el mundo de la lista de mundos del modulo y devuelve su entrada a la lista libre (la mitad de 0x74F140 que el EXE repite en linea).
void WorldListUnregister(RpWorld* world) {
    RwLLLink* const head = &WorldGlobals().worldList.link;
    RwLLLink* link = head->next;
    if (link == head) {
        return;
    }
    WorldListEntry* entry;
    for (;;) {
        entry = EntryOfWorldLink(link);
        if (entry->world == world) {
            break;
        }
        link = link->next;
        if (link == head) {
            return;
        }
    }
    entry->link.prev->next = entry->link.next;
    entry->link.next->prev = entry->link.prev;
    RwFreeListFree(WorldGlobals().entryFreeList, entry);
}

// Recorre el arbol de sectores (pila explicita) y destruye la malla de cada sector con atomos, dejando mesh = 0.
void WorldDestroySectorMeshes(RpWorld* world) {
    RpSector* sector = world->rootSector;
    RpSector* stack[64] = {};
    RwInt32 depth = 0;
    if (!sector) {
        return;
    }
    do {
        if (sector->type < 0) {
            RpWorldSector* const worldSector = reinterpret_cast<RpWorldSector*>(sector);
            if (worldSector->mesh) {
                _rpMeshDestroy(worldSector->mesh);
                worldSector->mesh = nullptr;
            }
            sector = stack[depth];
            --depth;
        } else {
            RpPlaneSector* const plane = reinterpret_cast<RpPlaneSector*>(sector);
            sector = plane->leftSubTree;
            ++depth;
            stack[depth] = plane->rightSubTree;
        }
    } while (depth >= 0);
}

// Cuerpo de RpWorldDestroy (0x74F610), que el EXE tambien incrusta en la salida de error de RpWorldCreate (0x74F9E2..).
void WorldDestroyBody(RpWorld* world) {
    WorldListUnregister(world);
    WorldDestroySectorMeshes(world);
    _rpMaterialListDeinitialize(&world->matList);
    RpSector* const root = world->rootSector;
    if (world->object.privateFlags & rpWORLDSINGLEMALLOC) {
        if (root) {
            _rpWorldSectorDeinstanceAll(root);
        }
    } else {
        if (root) {
            _rpWorldSectorDestroyRecurse(root);
        }
    }
    _rwPluginRegistryDeInitObject(&s_worldTKList, world);
    RwFree(world);
}

} // namespace

// 0x74E010
// Tamano de un material en un flujo: 0x28 sin textura, o tamano de la textura + 0x34; mas los datos de plugin y la cabecera de 12 bytes.
RwUInt32 RpMaterialStreamGetSize(const RpMaterial* material) {
    RwUInt32 size = 0x28;
    if (material->texture) {
        size = RwTextureStreamGetSize(material->texture) + 0x34;
    }
    return static_cast<RwUInt32>(_rwPluginRegistryGetSize(&s_materialTKList, material)) + size + 0xC;
}

// 0x74E150
// Destruye cada material (RpMaterialDestroy y lo pone a 0), libera el vector y deja materials/numMaterials/space a cero.
RpMaterialList* _rpMaterialListDeinitialize(RpMaterialList* matList) {
    RpMaterial** const materials = matList->materials;
    if (materials) {
        const RwInt32 numMaterials = matList->numMaterials;
        for (RwInt32 i = 0; i < numMaterials; ++i) {
            RpMaterialDestroy(materials[i]);
            materials[i] = nullptr;
        }
        RwFree(materials);
        matList->materials = nullptr;
    }
    matList->numMaterials = 0;
    matList->space = 0;
    return matList;
}

// 0x74E350
// Con hueco libre (space > numMaterials): guarda el material, refCount++ y devuelve el indice. Con la lista llena amplia 20 entradas (space*4 + 0x50 bytes,
// RwRealloc si ya habia vector, RwMalloc si no); sin memoria: RWERROR(NOMEM, bytes) y -1. Tras ampliar, space += 0x14 y guarda igual.
RwInt32 _rpMaterialListAppendMaterial(RpMaterialList* matList, RpMaterial* material) {
    if (matList->space > matList->numMaterials) {
        matList->materials[matList->numMaterials] = material;
        ++material->refCount;
        const RwInt32 count = matList->numMaterials + 1;
        matList->numMaterials = count;
        return count - 1;
    }
    const RwUInt32 newSize = static_cast<RwUInt32>(matList->space) * 4 + 0x50;
    RpMaterial** newMaterials;
    if (matList->materials) {
        newMaterials = static_cast<RpMaterial**>(RwRealloc(matList->materials, newSize, kMatListAllocHint));
    } else {
        newMaterials = static_cast<RpMaterial**>(RwMalloc(newSize, kMatListAllocHint));
    }
    if (!newMaterials) {
        RaiseRwError(kErrNoMem, newSize);
        return -1;
    }
    const RwInt32 newSpace = matList->space + 0x14;
    const RwInt32 index = matList->numMaterials;
    matList->materials = newMaterials;
    matList->space = newSpace;
    newMaterials[index] = material;
    ++material->refCount;
    const RwInt32 count = matList->numMaterials + 1;
    matList->numMaterials = count;
    return count - 1;
}

// 0x74E450
// 0x10 + 4*numMaterials, mas (tamano del material + 12) por cada material cuyo puntero no aparece antes en la lista (busqueda hacia atras desde i-1).
RwUInt32 _rpMaterialListStreamGetSize(const RpMaterialList* matList) {
    RwUInt32 size = static_cast<RwUInt32>(matList->numMaterials) * 4 + 0x10;
    for (RwInt32 i = 0; i < matList->numMaterials; ++i) {
        bool seenBefore = false;
        for (RwInt32 j = i - 1; j >= 0; --j) {
            if (matList->materials[j] == matList->materials[i]) {
                seenBefore = true;
                break;
            }
        }
        if (!seenBefore) {
            size = size + RpMaterialStreamGetSize(matList->materials[i]) + 0xC;
        }
    }
    return size;
}

// 0x74E600
// Lee la lista de materiales de un flujo. Chunk STRUCT con version 0x34000..0x36003 (si no, RWERROR 0x80000004 y 0); numero de materiales (RwStreamReadInt32);
// 0 materiales devuelve la lista vacia; con N>0 reserva N punteros (0x1030008; sin memoria: RWERROR(NOMEM, bytes), lista vacia y 0); luego lee N indices
// (vector temporal de N*4 bytes, 0x10501, sin comprobar si fallo la reserva como el EXE). Indice < 0: chunk MATERIAL (version comprobada), RpMaterialStreamRead;
// indice >= 0: reutiliza el material de ese puesto con refCount++. Cada uno se anade con _rpMaterialListAppendMaterial y luego RpMaterialDestroy (el resultado
// de anadir se ignora). Cualquier fallo libera el vector temporal, deshace la lista (el EXE lo repite en linea; aqui _rpMaterialListDeinitialize) y devuelve 0.
RpMaterialList* _rpMaterialListStreamRead(RwStream* stream, RpMaterialList* matList) {
    RwUInt32 length;
    RwUInt32 version;
    if (!RwStreamFindChunk(stream, kChunkStruct, &length, &version)) {
        return nullptr;
    }
    if (version < kStreamVersionMin || version > kStreamVersionMax) {
        RaiseRwError(kErrBadVersion);
        return nullptr;
    }
    RwInt32 numMaterials;
    if (!RwStreamReadInt32(stream, &numMaterials, 4)) {
        return nullptr;
    }
    matList->space = 0;
    matList->materials = nullptr;
    matList->numMaterials = 0;
    if (numMaterials == 0) {
        return matList;
    }
    if (numMaterials > 0) {
        const RwUInt32 size = static_cast<RwUInt32>(numMaterials) * 4;
        RpMaterial** const materials = static_cast<RpMaterial**>(RwMalloc(size, kMatListAllocHint));
        if (!materials) {
            RaiseRwError(kErrNoMem, size);
            _rpMaterialListDeinitialize(matList);
            return nullptr;
        }
        matList->materials = materials;
        matList->space = numMaterials;
    }
    const RwUInt32 indicesSize = static_cast<RwUInt32>(numMaterials) * 4;
    RwInt32* const indices = static_cast<RwInt32*>(RwMalloc(indicesSize, kMatListIndexHint));
    if (!RwStreamReadInt32(stream, indices, indicesSize)) {
        RwFree(indices);
        _rpMaterialListDeinitialize(matList);
        return nullptr;
    }
    for (RwInt32 i = 0; i < numMaterials; ++i) {
        RpMaterial* material;
        if (indices[i] < 0) {
            if (!RwStreamFindChunk(stream, kChunkMaterial, nullptr, &version)) {
                RwFree(indices);
                _rpMaterialListDeinitialize(matList);
                return nullptr;
            }
            if (version < kStreamVersionMin || version > kStreamVersionMax) {
                RaiseRwError(kErrBadVersion);
                RwFree(indices);
                _rpMaterialListDeinitialize(matList);
                return nullptr;
            }
            material = RpMaterialStreamRead(stream);
            if (!material) {
                RwFree(indices);
                _rpMaterialListDeinitialize(matList);
                return nullptr;
            }
        } else {
            material = matList->materials[indices[i]];
            ++material->refCount;
        }
        _rpMaterialListAppendMaterial(matList, material);
        RpMaterialDestroy(material);
    }
    RwFree(indices);
    return matList;
}

// 0x74E970
// Anade la geometria a la lista de geometrias dinamicas de D3D9 si no estaba (busqueda lineal); la lista libre de nodos (8 bytes, 0x80 por bloque, alineacion 4)
// se crea al primer uso. Sin comprobar el nodo devuelto, como el EXE. Inserta al principio.
void _rpD3D9AddDynamicGeometry(RpGeometry* geometry) {
    for (DynamicGeometryNode* node = static_cast<DynamicGeometryNode*>(s_dynamicGeometryHead); node; node = node->next) {
        if (node->geometry == geometry) {
            return;
        }
    }
    RwFreeList* freeList = s_dynamicGeometryPool;
    if (!freeList) {
        freeList = RwFreeListCreate(8, 0x80, 4, kDynamicGeometryHint);
        s_dynamicGeometryPool = freeList;
    }
    DynamicGeometryNode* const node = static_cast<DynamicGeometryNode*>(RwFreeListAlloc(freeList, kDynamicGeometryHint));
    node->geometry = geometry;
    node->next = static_cast<DynamicGeometryNode*>(s_dynamicGeometryHead);
    s_dynamicGeometryHead = node;
}

// 0x74E9E0
// Quita la geometria de la lista (si esta): desenlaza (la cabeza actualiza el global), pone geometry = 0 y devuelve el nodo a la lista libre.
void _rpD3D9RemoveDynamicGeometry(RpGeometry* geometry) {
    DynamicGeometryNode* const head = static_cast<DynamicGeometryNode*>(s_dynamicGeometryHead);
    DynamicGeometryNode* previous = nullptr;
    DynamicGeometryNode* node = head;
    if (!head) {
        return;
    }
    while (node->geometry != geometry) {
        previous = node;
        node = node->next;
        if (!node) {
            return;
        }
    }
    if (node == head) {
        s_dynamicGeometryHead = node->next;
    } else {
        previous->next = node->next;
    }
    node->geometry = nullptr;
    RwFreeListFree(s_dynamicGeometryPool, node);
}

// 0x74EA40 (nombre inferido: la llama RpWorldPluginAttach en 0x74FE81 tras registrar los plugins de mundo)
// Registra los plugins de D3D9 (luces 0x755B90 y, si fue bien, geometria 0x7587D0); guarda el callback de restauracion del dispositivo que habia y
// pone 0x74EA80 (que marca las geometrias dinamicas como sucias y encadena el anterior). Devuelve 1 si los dos registros fueron bien.
static RwBool _rpD3D9PluginAttach() {
    RwBool result = FALSE;
    if (D3D9LightPluginAttachExe() && D3D9GeometryPluginAttachExe()) {
        result = TRUE;
    }
    s_prevRestoreCallBack = _rwD3D9DeviceGetRestoreCallback();
    _rwD3D9DeviceSetRestoreCallback(reinterpret_cast<rwD3D9DeviceRestoreCallBack>(kD3D9RestoreCallBackAddress));
    return result;
}

// 0x74EAB0 (nombre inferido; primera etapa de apertura de tuberias, llamada desde 0x75E39D): sin trabajo, devuelve 1.
static RwBool _rpD3D9MaterialPipelineOpen() {
    return TRUE;
}

// 0x74EAC0 (nombre inferido; ultima etapa del cierre, 0x75E30A/0x75E3C9): vacia.
static void _rpD3D9MaterialPipelineClose() {
}

// 0x74EAD0 (nombre inferido; llamada desde 0x75E3AF)
// Crea la tuberia de sectores por defecto: RxPipelineCreate, pluginId = 2, bloquea, anade un fragmento con la definicion de nodo D3D9WorldSectorAllInOne
// (0x75E9F0), desbloquea; guarda la tuberia en el hueco 0x58 del modulo y la declara por defecto (RpWorldSetDefaultSectorPipeline). Fallo: destruye la tuberia y 0.
static RwBool _rpD3D9WorldSectorPipelineOpen() {
    RxPipeline* const pipeline = RxPipelineCreate();
    if (!pipeline) {
        return FALSE;
    }
    pipeline->pluginId = 2;
    RxLockedPipe* const locked = RxPipelineLock(pipeline);
    if (locked) {
        RxNodeDefinition* const nodeDefinition = RxNodeDefinitionGetD3D9WorldSectorAllInOne();
        RxLockedPipe* const withFragment = RxLockedPipeAddFragment(locked, nullptr, nodeDefinition, nullptr);
        if (RxLockedPipeUnlock(withFragment)) {
            PipelineModuleSlot(kPipelineSlotPlatformSector) = pipeline;
            RpWorldSetDefaultSectorPipeline(pipeline);
            return TRUE;
        }
    }
    _rxPipelineDestroy(pipeline);
    return FALSE;
}

// 0x74EB40 (nombre inferido; llamada desde 0x75E300 y 0x75E3BF)
// Pone la tuberia de sectores por defecto a NULL y destruye la del hueco 0x58 (que vuelve a 0).
static void _rpD3D9WorldSectorPipelineClose() {
    RpWorldSetDefaultSectorPipeline(nullptr);
    RxPipeline* const pipeline = PipelineModuleSlot(kPipelineSlotPlatformSector);
    if (pipeline) {
        _rxPipelineDestroy(pipeline);
        PipelineModuleSlot(kPipelineSlotPlatformSector) = nullptr;
    }
}

// 0x74EB80 (nombre inferido; llamada desde 0x75E3A6)
// Igual que la de sectores pero con D3D9AtomicAllInOne (0x7582E0), hueco 0x54 y RpAtomicSetDefaultPipeline; despues _rwD3D9LightsOpen (0x755D80): si falla,
// destruye la tuberia y devuelve 0 (el hueco y la tuberia por defecto quedan como estaban, igual que el EXE); si va bien llama a 0x760CF0 y devuelve 1.
static RwBool _rpD3D9AtomicPipelineOpen() {
    RxPipeline* const pipeline = RxPipelineCreate();
    if (!pipeline) {
        return FALSE;
    }
    pipeline->pluginId = 2;
    RxLockedPipe* const locked = RxPipelineLock(pipeline);
    if (locked) {
        RxNodeDefinition* const nodeDefinition = RxNodeDefinitionGetD3D9AtomicAllInOne();
        RxLockedPipe* const withFragment = RxLockedPipeAddFragment(locked, nullptr, nodeDefinition, nullptr);
        if (RxLockedPipeUnlock(withFragment)) {
            PipelineModuleSlot(kPipelineSlotPlatformAtomic) = pipeline;
            RpAtomicSetDefaultPipeline(pipeline);
            if (_rwD3D9LightsOpen()) {
                D3D9VertexShaderOpenExe();
                return TRUE;
            }
        }
    }
    _rxPipelineDestroy(pipeline);
    return FALSE;
}

// 0x74EC00 (nombre inferido; llamada desde 0x75E305 y 0x75E3C4)
// Cierre del modulo D3D9: devuelve cada nodo de la lista de geometrias dinamicas a su lista libre (geometry = 0; el siguiente se lee DESPUES de liberar, como
// el EXE), pone la cabeza a 0, destruye la lista libre, purga la cache de shaders (0x75EE60), _rwD3D9LightsClose (0x755FE0), pone la tuberia de atomos por
// defecto a NULL y destruye la del hueco 0x54.
static void _rpD3D9AtomicPipelineClose() {
    DynamicGeometryNode* node = static_cast<DynamicGeometryNode*>(s_dynamicGeometryHead);
    if (node) {
        do {
            node->geometry = nullptr;
            RwFreeListFree(s_dynamicGeometryPool, node);
            node = node->next;
        } while (node);
        s_dynamicGeometryHead = node;
    }
    if (s_dynamicGeometryPool) {
        RwFreeListDestroy(s_dynamicGeometryPool);
        s_dynamicGeometryPool = nullptr;
    }
    _rpD3D9VertexShaderCachePurge();
    _rwD3D9LightsClose();
    RpAtomicSetDefaultPipeline(nullptr);
    RxPipeline* const pipeline = PipelineModuleSlot(kPipelineSlotPlatformAtomic);
    if (pipeline) {
        _rxPipelineDestroy(pipeline);
        PipelineModuleSlot(kPipelineSlotPlatformAtomic) = nullptr;
    }
}

// 0x74ECA0
// Desinstancia todos los sectores del arbol (recursion por la izquierda, iteracion por la derecha). Sector de construccion (-2): nada. Sector con atomos (-1):
// libera la entrada de recursos, las ataduras de atomos y de luces y los plugins, y destruye la malla (mesh = 0).
void _rpWorldSectorDeinstanceAll(RpSector* sector) {
    if (sector->type == rwSECTORBUILD) {
        return;
    }
    for (;;) {
        if (sector->type == rwSECTORATOMIC) {
            RpWorldSector* const worldSector = reinterpret_cast<RpWorldSector*>(sector);
            WorldSectorReleaseInstance(worldSector);
            if (worldSector->mesh) {
                _rpMeshDestroy(worldSector->mesh);
                worldSector->mesh = nullptr;
            }
            return;
        }
        RpPlaneSector* const plane = reinterpret_cast<RpPlaneSector*>(sector);
        if (plane->leftSubTree) {
            _rpWorldSectorDeinstanceAll(plane->leftSubTree);
        }
        sector = plane->rightSubTree;
        if (!sector) {
            return;
        }
        if (sector->type == rwSECTORBUILD) {
            return;
        }
    }
}

// 0x74ED50
// Destruye el arbol: sector de construccion (-2): solo RwFree. Plano: izquierda, derecha (se lee tras la izquierda), pone los punteros a 0 y RwFree.
// Sector con atomos: libera instancia, ataduras y plugins; luego vertices, normales, luminancias, triangulos y los 8 juegos de coordenadas (cada uno si no es
// nulo, y a 0), la malla (mesh = 0) y el propio sector.
void _rpWorldSectorDestroyRecurse(RpSector* sector) {
    if (sector->type == rwSECTORBUILD) {
        RwFree(sector);
        return;
    }
    if (sector->type != rwSECTORATOMIC) {
        RpPlaneSector* const plane = reinterpret_cast<RpPlaneSector*>(sector);
        if (plane->leftSubTree) {
            _rpWorldSectorDestroyRecurse(plane->leftSubTree);
        }
        RpSector* const right = plane->rightSubTree;
        plane->leftSubTree = nullptr;
        if (right) {
            _rpWorldSectorDestroyRecurse(right);
        }
        plane->rightSubTree = nullptr;
        RwFree(plane);
        return;
    }
    RpWorldSector* const worldSector = reinterpret_cast<RpWorldSector*>(sector);
    WorldSectorReleaseInstance(worldSector);
    if (worldSector->vertices) {
        RwFree(worldSector->vertices);
        worldSector->vertices = nullptr;
    }
    if (worldSector->normals) {
        RwFree(worldSector->normals);
        worldSector->normals = nullptr;
    }
    if (worldSector->preLitLum) {
        RwFree(worldSector->preLitLum);
        worldSector->preLitLum = nullptr;
    }
    if (worldSector->triangles) {
        RwFree(worldSector->triangles);
        worldSector->triangles = nullptr;
    }
    for (RwInt32 i = 0; i < rwMAXTEXTURECOORDS; ++i) {
        if (worldSector->texCoords[i]) {
            RwFree(worldSector->texCoords[i]);
            worldSector->texCoords[i] = nullptr;
        }
    }
    if (worldSector->mesh) {
        _rpMeshDestroy(worldSector->mesh);
        worldSector->mesh = nullptr;
    }
    RwFree(worldSector);
}

// 0x74EEC0
// Sin triangulos devuelve el sector sin hacer nada. Tuberia: la del sector; si no, la del mundo actual (RwEngineInstance->curWorld->pipeline); si no, la
// de sectores por defecto del modulo (hueco 0x40). RxPipelineExecute(tuberia, sector, 1): devuelve el sector si el resultado es distinto de cero, si no 0.
RpWorldSector* _rpSectorDefaultRenderCallBack(RpWorldSector* sector) {
    if (sector->numTriangles == 0) {
        return sector;
    }
    RxPipeline* pipeline = sector->pipeline;
    if (!pipeline) {
        RpWorld* const world = static_cast<RpWorld*>(RwEngineInstance->curWorld);
        pipeline = world->pipeline;
        if (!pipeline) {
            pipeline = PipelineModuleSlot(kPipelineSlotDefaultSector);
        }
    }
    return RxPipelineExecute(pipeline, sector, TRUE) ? sector : nullptr;
}

// 0x74F210
// Recorre el arbol con pila explicita (planos: baja por la izquierda y apila la derecha). Cada sector con atomos SIN malla construye una: una RpBuildMesh con
// numTriangles; si el mundo tiene materiales, tres vectores temporales (texturas 0x10006, rasters 0x10507, tuberias 0x10507; numMateriales*4 bytes) con los
// valores unicos; por cada triangulo toma el material de la ventana (materiales + matListWindowBase)[matIndex], busca/anade su textura, su raster
// (texture->raster, 0 sin textura) y su tuberia, y llama a _rpBuildMeshAddTriangle (vertices, matIndex, indices de textura, raster y tuberia); libera los tres
// vectores y optimiza con _rpMeshOptimise(buildMesh, flags & 1). Sin malla resultante: destruye el RpBuildMesh y devuelve 0. Raiz nula: 0. Si todo va bien devuelve el mundo.
RpWorld* RpWorldUnlock(RpWorld* world) {
    RpSector* sector = world->rootSector;
    RpSector* stack[64] = {};
    RwInt32 depth = 0;
    if (!sector) {
        return nullptr;
    }
    for (;;) {
        if (sector->type < 0) {
            RpWorldSector* const worldSector = reinterpret_cast<RpWorldSector*>(sector);
            RpMaterial** const materials = world->matList.materials + worldSector->matListWindowBase;
            if (!worldSector->mesh) {
                RpBuildMesh* const buildMesh = _rpBuildMeshCreate(worldSector->numTriangles);
                if (!buildMesh) {
                    return nullptr;
                }
                if (world->matList.numMaterials != 0) {
                    const RwUInt32 arraySize = static_cast<RwUInt32>(world->matList.numMaterials) * 4;
                    RwTexture** const textures = static_cast<RwTexture**>(RwMalloc(arraySize, 0x10006));
                    RwRaster** const rasters = static_cast<RwRaster**>(RwMalloc(arraySize, 0x10507));
                    RxPipeline** const pipelines = static_cast<RxPipeline**>(RwMalloc(arraySize, 0x10507));
                    RwUInt32 numTextures = 0;
                    RwUInt32 numRasters = 0;
                    RwUInt32 numPipelines = 0;
                    for (RwInt32 i = 0; i < worldSector->numTriangles; ++i) {
                        const RpTriangle* const triangle = &worldSector->triangles[i];
                        RpMaterial* const material = materials[triangle->matIndex];

                        RwTexture* const texture = material->texture;
                        RwUInt16 textureIndex = 0;
                        if (static_cast<RwUInt16>(numTextures) > textureIndex) {
                            do {
                                if (textures[textureIndex] == texture) {
                                    break;
                                }
                                ++textureIndex;
                            } while (textureIndex < static_cast<RwUInt16>(numTextures));
                        }
                        if (textureIndex == static_cast<RwUInt16>(numTextures)) {
                            textures[textureIndex] = texture;
                            numTextures = numTextures + 1;
                        }

                        RwRaster* const raster = texture ? texture->raster : nullptr;
                        RwUInt16 rasterIndex = 0;
                        if (static_cast<RwUInt16>(numRasters) > rasterIndex) {
                            do {
                                if (rasters[rasterIndex] == raster) {
                                    break;
                                }
                                ++rasterIndex;
                            } while (rasterIndex < static_cast<RwUInt16>(numRasters));
                        }
                        if (rasterIndex == static_cast<RwUInt16>(numRasters)) {
                            rasters[rasterIndex] = raster;
                            numRasters = numRasters + 1;
                        }

                        RxPipeline* const pipeline = material->pipeline;
                        RwUInt16 pipelineIndex = 0;
                        if (static_cast<RwUInt16>(numPipelines) > pipelineIndex) {
                            do {
                                if (pipelines[pipelineIndex] == pipeline) {
                                    break;
                                }
                                ++pipelineIndex;
                            } while (pipelineIndex < static_cast<RwUInt16>(numPipelines));
                        }
                        if (pipelineIndex == static_cast<RwUInt16>(numPipelines)) {
                            pipelines[pipelineIndex] = pipeline;
                            numPipelines = numPipelines + 1;
                        }

                        _rpBuildMeshAddTriangle(buildMesh, material, triangle->vertIndex[0], triangle->vertIndex[1], triangle->vertIndex[2],
                                                triangle->matIndex, textureIndex, rasterIndex, pipelineIndex);
                    }
                    RwFree(textures);
                    RwFree(rasters);
                    RwFree(pipelines);
                }
                RpMeshHeader* const mesh = _rpMeshOptimise(buildMesh, (world->flags & rpWORLDTRISTRIP) ? 1 : 0);
                if (!mesh) {
                    _rpBuildMeshDestroy(buildMesh);
                    return nullptr;
                }
                worldSector->mesh = mesh;
            }
            sector = stack[depth];
            --depth;
        } else {
            RpPlaneSector* const plane = reinterpret_cast<RpPlaneSector*>(sector);
            sector = plane->leftSubTree;
            ++depth;
            stack[depth] = plane->rightSubTree;
        }
        if (depth < 0) {
            return world;
        }
    }
}

// 0x74F4E0
// Recorre la lista de mundos registrados (entrada = enlace - 8). Mundo de asignacion unica (privateFlags & 1): el sector pertenece si su direccion esta en
// [mundo, mundo + memorySize) (comparacion sin signo). Si no, recorre su arbol con pila explicita buscando el puntero exacto. Sin coincidencia: NULL.
RpWorld* RpWorldSectorGetWorld(const RpWorldSector* sector) {
    RwLLLink* const head = &WorldGlobals().worldList.link;
    RwLLLink* link = head->next;
    if (link == head) {
        return nullptr;
    }
    const uintptr_t sectorAddress = reinterpret_cast<uintptr_t>(sector);
    const RpSector* stack[65] = {};
    do {
        WorldListEntry* const entry = EntryOfWorldLink(link);
        RpWorld* const world = entry->world;
        if (world->object.privateFlags & rpWORLDSINGLEMALLOC) {
            const uintptr_t start = reinterpret_cast<uintptr_t>(world);
            if (sectorAddress >= start && sectorAddress < start + entry->memorySize) {
                return entry->world;
            }
        } else {
            const RpSector* current = world->rootSector;
            RwInt32 depth = 0;
            do {
                if (current->type < 0) {
                    if (reinterpret_cast<const RpSector*>(sector) == current) {
                        return entry->world;
                    }
                    current = stack[depth];
                    --depth;
                } else {
                    const RpPlaneSector* const plane = reinterpret_cast<const RpPlaneSector*>(current);
                    current = plane->leftSubTree;
                    ++depth;
                    stack[depth] = plane->rightSubTree;
                }
            } while (depth >= 0);
        }
        link = link->next;
    } while (link != head);
    return nullptr;
}

// 0x74F610
// Desregistra el mundo, destruye las mallas de sus sectores, deshace la lista de materiales y libera el arbol (desinstancia si fue de asignacion unica;
// destruye cada nodo si no), los plugins del mundo y el propio mundo. Devuelve 1.
RwBool RpWorldDestroy(RpWorld* world) {
    WorldDestroyBody(world);
    return TRUE;
}

// 0x74F760
// Reserva el mundo (tamano del registro de plugins 0x8D62D0, 0x3000B; sin memoria RWERROR(NOMEM, tamano) y 0), lo inicializa (tipo 7, lista de materiales
// vacia, flags 0, orden FRONT2BACK) y reserva el sector raiz (tamano del registro 0x8D736C; fallo: RWERROR con el argumento 4 como el EXE, libera el mundo y 0).
// El sector: tipo -1, listas vacias, cajas = *boundingBox. El mundo: origen 0, cajas = *boundingBox, listas vacias, renderCallBack = 0x74EEC0 (direccion del
// EXE), pipeline 0. Se registra en la lista de mundos (entrada 0x40507, sin comprobar el fallo), InitObject del mundo y del sector y RpWorldUnlock; si este
// falla se deshace todo como RpWorldDestroy y devuelve 0.
RpWorld* RpWorldCreate(RwBBox* boundingBox) {
    RpWorld* const world = static_cast<RpWorld*>(RwMalloc(s_worldTKList.sizeOfStruct, kWorldAllocHint));
    if (!world) {
        RaiseRwError(kErrNoMem, s_worldTKList.sizeOfStruct);
        return nullptr;
    }
    world->object.type = rpWORLD;
    world->object.subType = 0;
    world->object.flags = 0;
    world->object.privateFlags = 0;
    world->object.parent = nullptr;
    _rpMaterialListInitialize(&world->matList);
    world->flags = 0;
    world->renderOrder = rpWORLDRENDERFRONT2BACK;

    RpWorldSector* const sector = static_cast<RpWorldSector*>(RwMalloc(s_sectorTKList229.sizeOfStruct, kWorldAllocHint));
    if (!sector) {
        RaiseRwError(kErrNoMem, 4);
        RwFree(world);
        return nullptr;
    }
    RwLLLink* const atomicsHead = &sector->collAtomicsInWorldSector.link;
    atomicsHead->prev = atomicsHead;
    atomicsHead->next = atomicsHead;
    RwLLLink* const lightsHead = &sector->lightsInWorldSector.link;
    lightsHead->prev = lightsHead;
    sector->type = rwSECTORATOMIC;
    lightsHead->next = lightsHead;
    sector->repEntry = nullptr;
    sector->mesh = nullptr;
    sector->numVertices = 0;
    sector->numTriangles = 0;
    sector->vertices = nullptr;
    sector->triangles = nullptr;
    sector->normals = nullptr;
    for (RwInt32 i = 0; i < rwMAXTEXTURECOORDS; ++i) {
        sector->texCoords[i] = nullptr;
    }
    sector->preLitLum = nullptr;
    sector->boundingBox.inf = boundingBox->inf;
    sector->boundingBox.sup = boundingBox->sup;
    sector->tightBoundingBox.inf = boundingBox->inf;
    sector->tightBoundingBox.sup = boundingBox->sup;
    sector->pipeline = nullptr;

    world->rootSector = reinterpret_cast<RpSector*>(sector);
    world->numTexCoordSets = 0;
    world->worldOrigin.z = 0.0f;
    world->worldOrigin.y = 0.0f;
    world->worldOrigin.x = 0.0f;
    world->boundingBox.inf = boundingBox->inf;
    world->boundingBox.sup = boundingBox->sup;
    world->numClumpsInWorld = 0;
    RwLLLink* const clumpsHead = &world->clumpList.link;
    clumpsHead->prev = clumpsHead;
    world->currentClumpLink = clumpsHead;
    clumpsHead->next = clumpsHead;
    RwLLLink* const worldLightsHead = &world->lightList.link;
    worldLightsHead->prev = worldLightsHead;
    worldLightsHead->next = worldLightsHead;
    RwLLLink* const directionalHead = &world->directionalLightList.link;
    directionalHead->next = directionalHead;
    directionalHead->prev = directionalHead;
    world->renderCallBack = reinterpret_cast<RpWorldSectorCallBackRender>(kSectorDefaultRenderCallBackAddress);
    world->pipeline = nullptr;

    // _rpWorldRegisterWorld(world, tamano del registro) en linea.
    {
        const RwUInt32 memorySize = static_cast<RwUInt32>(s_worldTKList.sizeOfStruct);
        WorldListEntry* const entry = static_cast<WorldListEntry*>(RwFreeListAlloc(WorldGlobals().entryFreeList, kWorldListEntryHint));
        if (entry) {
            entry->world = world;
            entry->memorySize = memorySize;
            entry->link.next = WorldGlobals().worldList.link.next;
            entry->link.prev = &WorldGlobals().worldList.link;
            WorldGlobals().worldList.link.next->prev = &entry->link;
            WorldGlobals().worldList.link.next = &entry->link;
        }
    }

    _rwPluginRegistryInitObject(&s_worldTKList, world);
    _rwPluginRegistryInitObject(&s_sectorTKList229, sector);
    if (!RpWorldUnlock(world)) {
        WorldDestroyBody(world);
        return nullptr;
    }
    return world;
}

// 0x74FBC0
// Llama a fpCallBack(material, data) con cada material de la lista (el numero se lee una vez; el vector en cada vuelta) hasta que devuelva NULL. Devuelve el mundo.
RpWorld* RpWorldForAllMaterials(RpWorld* world, RpMaterialCallBack fpCallBack, void* data) {
    const RwInt32 numMaterials = world->matList.numMaterials;
    for (RwInt32 i = 0; i < numMaterials; ++i) {
        if (!fpCallBack(world->matList.materials[i], data)) {
            break;
        }
    }
    return world;
}

// 0x74FC70
// Recorre el arbol con pila explicita y llama a fpCallBack(sector, data) con cada sector con atomos, hasta que devuelva NULL. Sin comprobar la raiz. Devuelve el mundo.
RpWorld* RpWorldForAllWorldSectors(RpWorld* world, RpWorldSectorCallBack fpCallBack, void* data) {
    RpSector* sector = world->rootSector;
    RpSector* stack[64] = {};
    RwInt32 depth = 0;
    do {
        if (sector->type < 0) {
            if (!fpCallBack(reinterpret_cast<RpWorldSector*>(sector), data)) {
                break;
            }
            sector = stack[depth];
            --depth;
        } else {
            RpPlaneSector* const plane = reinterpret_cast<RpPlaneSector*>(sector);
            sector = plane->leftSubTree;
            ++depth;
            stack[depth] = plane->rightSubTree;
        }
    } while (depth >= 0);
    return world;
}

// Registro de los hooks del lote 229A, sin validar.
void RpWorldInjectHooks229A() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpWorld");

    RH_ScopedGlobalInstall(RpMaterialStreamGetSize, 0x74E010);
    RH_ScopedGlobalInstall(_rpMaterialListDeinitialize, 0x74E150);
    RH_ScopedGlobalInstall(_rpMaterialListAppendMaterial, 0x74E350);
    RH_ScopedGlobalInstall(_rpMaterialListStreamGetSize, 0x74E450);
    RH_ScopedGlobalInstall(_rpMaterialListStreamRead, 0x74E600);
    RH_ScopedGlobalInstall(_rpD3D9AddDynamicGeometry, 0x74E970);
    RH_ScopedGlobalInstall(_rpD3D9RemoveDynamicGeometry, 0x74E9E0);
    RH_ScopedGlobalInstall(_rpD3D9PluginAttach, 0x74EA40);
    RH_ScopedGlobalInstall(_rpD3D9MaterialPipelineOpen, 0x74EAB0);
    RH_ScopedGlobalInstall(_rpD3D9MaterialPipelineClose, 0x74EAC0);
    RH_ScopedGlobalInstall(_rpD3D9WorldSectorPipelineOpen, 0x74EAD0);
    RH_ScopedGlobalInstall(_rpD3D9WorldSectorPipelineClose, 0x74EB40);
    RH_ScopedGlobalInstall(_rpD3D9AtomicPipelineOpen, 0x74EB80);
    RH_ScopedGlobalInstall(_rpD3D9AtomicPipelineClose, 0x74EC00);
    RH_ScopedGlobalInstall(_rpWorldSectorDeinstanceAll, 0x74ECA0);
    RH_ScopedGlobalInstall(_rpWorldSectorDestroyRecurse, 0x74ED50);
    RH_ScopedGlobalInstall(_rpSectorDefaultRenderCallBack, 0x74EEC0);
    RH_ScopedGlobalInstall(RpWorldUnlock, 0x74F210);
    RH_ScopedGlobalInstall(RpWorldSectorGetWorld, 0x74F4E0);
    RH_ScopedGlobalInstall(RpWorldDestroy, 0x74F610);
    RH_ScopedGlobalInstall(RpWorldCreate, 0x74F760);
    RH_ScopedGlobalInstall(RpWorldForAllMaterials, 0x74FBC0);
    RH_ScopedGlobalInstall(RpWorldForAllWorldSectors, 0x74FC70);
}

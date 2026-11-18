/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RpWorld
 * Reconstrucción: COMPLETA — 197/197 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 25/197 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rpworld.h"
#include "X87Math.h"
#include <rpdbgerr.h>
#include <bit>
#include <cstring>
#include "X87Math.h"

// Direcciones del EXE usadas por las funciones de este archivo.
namespace {
constexpr uintptr_t kWorldSectorPluginRegistry   = 0x8D736C; // RpWorldSector: lista de plugins
constexpr uintptr_t kD3D9AtomicAllInOneNode      = 0x8D6338; // RxNodeDefinition estatica de D3D9AtomicAllInOne
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
    RH_ScopedGlobalInstall(_rpD3D9VertexDeclarationUnInstV3d, 0x7551F0); // lote R10A (sin validar)
}

void _rwD3D9VSGetWorldViewMatrix() {
    ((void(__cdecl *)(void))0x764760)();
}

void _rwD3D9VSGetWorldMultiplyMatrix(RwMatrix* worldMultiplyMatrix, const RwMatrix* matrix) {
    ((void(__cdecl *)(RwMatrix*, const RwMatrix*))0x764920)(worldMultiplyMatrix, matrix);
}

void _rwD3D9VSGetProjectionTransposedMatrix() {
    ((void(__cdecl *)(void))0x764D20)();
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


// 0x75E9F0: `mov eax, 0x8D63B8; ret`; devuelve la direccion de la definicion estatica del EXE, sin leer ni escribir nada.
RxNodeDefinition* RxNodeDefinitionGetD3D9WorldSectorAllInOne() {
    return reinterpret_cast<RxNodeDefinition*>(kD3D9WorldSectorAllInOneNode);
}

// 0x7582E0 (lote R18A, sin validar; sustituye la envoltura): `mov eax, 0x8D6338; ret`; devuelve la direccion de la definicion estatica del EXE, sin leer ni escribir nada.
RxNodeDefinition* RxNodeDefinitionGetD3D9AtomicAllInOne() {
    return reinterpret_cast<RxNodeDefinition*>(kD3D9AtomicAllInOneNode);
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

// Lote R16A (sin validar): desplazamientos de los datos de plugin 0x509 (mundo al que pertenece el objeto) y de los modulos de luz y de enlaces (ties).
static auto& s_cameraWorldOffset = StaticRef<RwInt32>(0xC925A4); // camara: datos de 0x1C bytes; el mundo va en +0x0C (escrito en 0x75009A)
static auto& s_clumpWorldOffset  = StaticRef<RwInt32>(0xC925AC); // clump: {mundo, marca de ties} (escrito en 0x7500CB)
static auto& s_lightWorldOffset  = StaticRef<RwInt32>(0xC925B0); // luz: el mundo en +0x00 (escrito en 0x7500FA)
static auto& s_lightModuleOffset = StaticRef<RwInt32>(0xC925D8); // RwEngineInstance + este valor = lista libre de luces

// Lote 230A (sin validar): nodos de enlace (tie) entre sectores del mundo y atomos/luces. Cada nodo es 0x18 bytes: un enlace {next, prev} en +0x00, un puntero en +0x08 (atomo o luz),
// un segundo enlace en +0x0C y un puntero en +0x14 (sector). Las listas libres cuelgan de RwEngineInstance + [0xC9259C] (datos del plugin de motor 0x509): RpTie en +0, RpLightTie en +4 y
// en +8 un contador de 16 bits (inicializado a 1 por el constructor 0x750280) que RpWorldAddClump copia a los datos de plugin del clump.
static auto& s_tieModuleOffset = StaticRef<RwInt32>(0xC9259C);
struct TieModuleGlobals {
    RwFreeList* tieFreeList;
    RwFreeList* lightTieFreeList;
    RwUInt32    tieFrame;
};
static TieModuleGlobals& TieGlobals() {
    return *reinterpret_cast<TieModuleGlobals*>(reinterpret_cast<uintptr_t>(RwEngineInstance) + static_cast<uintptr_t>(s_tieModuleOffset));
}

// Desenlaza primero el segundo enlace (+0x0C: anterior->siguiente = siguiente, siguiente->anterior = anterior) y despues el primero (+0x00), como el EXE.
static void TieUnlinkBoth(RwLLLink* first, RwLLLink* second) {
    rwLinkListRemoveLLLink(second);
    rwLinkListRemoveLLLink(first);
}

// Cuerpo comun del destructor del plugin de luz (0x750D30) y de RpWorldRemoveLight (0x751960): recorre WorldSectorsInLight (+0x2C) y, por cada enlace (que es el segundo enlace de un RpLightTie, +0x0C):
// lo quita de la lista (anterior->siguiente = siguiente), desenlaza el nodo completo y lo libera en la lista libre de +4. El siguiente se lee antes de tocar el nodo; el bucle acaba al volver al centinela.
static void LightDestroyAllTies(RpLight* light) {
    RwLLLink* const end = &light->WorldSectorsInLight.link;
    RwLLLink* link = end->next;
    while (link != end) {
        RwLLLink* const next = link->next;
        link->prev->next = next;
        RpLightTie* const tie = reinterpret_cast<RpLightTie*>(reinterpret_cast<uint8_t*>(link) - offsetof(RpLightTie, WorldSectorInLight));
        TieUnlinkBoth(&tie->lightInWorldSector, &tie->WorldSectorInLight);
        RwFreeListFree(TieGlobals().lightTieFreeList, tie);
        link = next;
    }
}

// Lista libre de RpLight: RwEngineInstance + [0xC925D8] (cdecl en 0x752110, 0x7520D0 y 0x752210).
static RwFreeList*& LightFreeList() {
    return *reinterpret_cast<RwFreeList**>(reinterpret_cast<uintptr_t>(RwEngineInstance) + static_cast<uintptr_t>(s_lightModuleOffset));
}

// Lote 226A (sin validar): funciones de este archivo definidas mas abajo; se declaran aqui para registrarlas.
static RwStream* AtomicRightsStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject); // 0x749120
static RwStream* AtomicRightsStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject); // 0x749160
static void      ClumpFreeListDestroyCallBack(void* clump, void* data); // 0x749520
static void      AtomicFreeListDestroyCallBack(void* atomic, void* data); // 0x7496A0
static RwBool    ClumpPluginAttach(); // 0x749830
static RwBool    _rpTieMeshPluginAttach(); // 0x750050 (lote R16A)
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

// ---------------------------------------------------------------------------------------------------------------------
// Lote R18A (sin validar): mallas de RpWorld (RpBuildMesh, RpMeshHeader). Estado global: lista libre de RpBuildMesh en 0xC926F0; desplazamiento del contador
// de serie de mallas dentro de RwEngineInstance en 0xC9B8C0.
// ---------------------------------------------------------------------------------------------------------------------
namespace {
constexpr RwUInt32 kBuildMeshFreeListHint = 0x30502;   // RpBuildMesh (lista libre) y RpMeshHeader leido
constexpr RwUInt32 kBuildMeshBufferHint   = 0x1030502; // triangulos del RpBuildMesh
constexpr RwUInt32 kMeshStreamChunk       = 0x100;     // indices por lectura/escritura de flujo
}
static auto& s_buildMeshFreeList = StaticRef<RwFreeList*>(0xC926F0);
static auto& s_meshSerialOffset  = StaticRef<RwUInt32>(0xC9B8C0);
static_assert(sizeof(RpBuildMeshTriangle) == 0x14 && offsetof(RpBuildMeshTriangle, material) == 0x8 && offsetof(RpBuildMeshTriangle, matIndex) == 0xC);
static_assert(offsetof(RpBuildMeshTriangle, textureIndex) == 0xE && offsetof(RpBuildMeshTriangle, rasterIndex) == 0x10 && offsetof(RpBuildMeshTriangle, pipelineIndex) == 0x12);
static_assert(sizeof(RpBuildMesh) == 0xC && sizeof(RpMesh) == 0xC && sizeof(RpMeshHeader) == 0x10 && offsetof(RpMeshHeader, firstMeshOffset) == 0xC);

namespace {
// El objeto de la malla es una geometria (tipo 8) o un sector (tipo 7). Con rpGEOMETRYNATIVE (0x1000000 en +0x8) los indices no viajan en el flujo.
inline bool MeshObjectIsKnownType(const void* object) {
    const RwUInt8 type = *static_cast<const RwUInt8*>(object);
    return type == 8 || type == 7;
}
inline bool MeshObjectIsNative(const void* object) {
    const RwUInt32 flags = *reinterpret_cast<const RwUInt32*>(static_cast<const uint8_t*>(object) + 8);
    return MeshObjectIsKnownType(object) && (flags & 0x1000000u) != 0;
}
inline bool MeshObjectStoresIndices(const void* object) {
    const RwUInt32 flags = *reinterpret_cast<const RwUInt32*>(static_cast<const uint8_t*>(object) + 8);
    return MeshObjectIsKnownType(object) && (flags & 0x1000000u) == 0;
}
}

// 0x758A90 (lote R18A, sin validar; sustituye la envoltura; cdecl, 1 argumento, 234 bytes): reserva un RpBuildMesh de la lista libre 0xC926F0 (pista 0x30502); si falla pone el error
// 0x80000013 con 0xC y devuelve NULL. numTriangles = 0; con bufferSize != 0 reserva bufferSize * 20 bytes (RwMalloc, pista 0x1030502): si falla devuelve el RpBuildMesh a la lista
// libre, error 0x80000013 con el tamano y NULL; si no triangleBufferSize = bufferSize. Con bufferSize == 0: meshTriangles = NULL y triangleBufferSize = 0.
RpBuildMesh* _rpBuildMeshCreate(RwUInt32 bufferSize) {
    RpBuildMesh* const mesh = static_cast<RpBuildMesh*>(RwFreeListAlloc(s_buildMeshFreeList, kBuildMeshFreeListHint));
    if (mesh == nullptr) {
        RaiseRwError(kErrorNoMemory, 0xC);
        return nullptr;
    }
    mesh->numTriangles = 0;
    if (bufferSize != 0) {
        const RwUInt32 bytes = bufferSize * 20;
        mesh->meshTriangles = static_cast<RpBuildMeshTriangle*>(RwMalloc(bytes, kBuildMeshBufferHint));
        if (mesh->meshTriangles == nullptr) {
            RwFreeListFree(s_buildMeshFreeList, mesh);
            RaiseRwError(kErrorNoMemory, bytes);
            return nullptr;
        }
        mesh->triangleBufferSize = bufferSize;
        return mesh;
    }
    mesh->meshTriangles      = nullptr;
    mesh->triangleBufferSize = 0;
    return mesh;
}

// 0x758B80 (lote R18A, sin validar; sustituye la envoltura; cdecl, 1 argumento, 64 bytes): libera meshTriangles con RwFree si no es NULL (y lo pone a NULL), devuelve el RpBuildMesh a la
// lista libre 0xC926F0 y devuelve TRUE.
RwBool _rpBuildMeshDestroy(RpBuildMesh* mesh) {
    if (mesh->meshTriangles != nullptr) {
        RwFree(mesh->meshTriangles);
        mesh->meshTriangles = nullptr;
    }
    RwFreeListFree(s_buildMeshFreeList, mesh);
    return TRUE;
}

// 0x758BC0 (lote R18A, sin validar; sustituye la envoltura; cdecl, 1 argumento, 58 bytes): libera la cabecera con RwFree salvo que sus cinco campos (flags, numMeshes, serialNum,
// totalIndicesInMesh, firstMeshOffset) sean todos cero. Devuelve TRUE.
RwBool _rpMeshDestroy(RpMeshHeader* mesh) {
    if (!(mesh->flags == 0 && mesh->numMeshes == 0 && mesh->serialNum == 0 && mesh->totalIndicesInMesh == 0 && mesh->firstMeshOffset == 0)) {
        RwFree(mesh);
    }
    return TRUE;
}

// 0x758C00 (lote R18A, sin validar; sustituye la envoltura; cdecl, 9 argumentos, 291 bytes): anade un triangulo al RpBuildMesh. Con numTriangles >= triangleBufferSize pide
// (numTriangles + 1) * 20 bytes: RwRealloc (si numTriangles != 0) o RwMalloc (pista 0x1030502); si falla error 0x80000013 con el tamano y NULL; si va bien meshTriangles = bloque y
// triangleBufferSize = numTriangles + 1 (crece de uno en uno). Escribe indices de vertice (16 bits), material, matIndex, textureIndex, rasterIndex y pipelineIndex (16 bits) en el
// triangulo numTriangles, incrementa numTriangles y devuelve la malla.
RpBuildMesh* _rpBuildMeshAddTriangle(RpBuildMesh* mesh, RpMaterial* material, RwInt32 vert1, RwInt32 vert2, RwInt32 vert3, RwUInt16 matIndex, RwUInt16 textureIndex, RwUInt16 rasterIndex, RwUInt16 pipelineIndex) {
    if (mesh->numTriangles >= mesh->triangleBufferSize) {
        const RwUInt32 bytes = (mesh->numTriangles + 1) * 20;
        void* block;
        if (mesh->numTriangles != 0) {
            block = RwRealloc(mesh->meshTriangles, bytes, kBuildMeshBufferHint);
        } else {
            block = RwMalloc(bytes, kBuildMeshBufferHint);
        }
        if (block == nullptr) {
            RaiseRwError(kErrorNoMemory, bytes);
            return nullptr;
        }
        mesh->meshTriangles      = static_cast<RpBuildMeshTriangle*>(block);
        mesh->triangleBufferSize = mesh->numTriangles + 1;
    }
    RpBuildMeshTriangle* const triangle = &mesh->meshTriangles[mesh->numTriangles];
    triangle->material      = material;
    triangle->vertIndex[0]  = static_cast<RwUInt16>(vert1);
    triangle->vertIndex[1]  = static_cast<RwUInt16>(vert2);
    triangle->vertIndex[2]  = static_cast<RwUInt16>(vert3);
    triangle->matIndex      = matIndex;
    triangle->textureIndex  = textureIndex;
    triangle->rasterIndex   = rasterIndex;
    triangle->pipelineIndex = pipelineIndex;
    mesh->numTriangles++;
    return mesh;
}

// 0x758D30 (lote R18A, sin validar; sustituye la envoltura; cdecl, 3 argumentos, 58 bytes): recorre las numMeshes mallas (RpMesh de 12 bytes que empiezan en cabecera + 0x10 +
// firstMeshOffset) llamando a callback(malla, cabecera, datos); se detiene si devuelve NULL. Devuelve la cabecera.
RpMeshHeader* _rpMeshHeaderForAllMeshes(RpMeshHeader* meshHeader, RpMeshCallBack fpCallBack, void* data) {
    RpMesh* mesh = reinterpret_cast<RpMesh*>(reinterpret_cast<uint8_t*>(meshHeader) + meshHeader->firstMeshOffset + 0x10);
    RwUInt32 remaining = meshHeader->numMeshes;
    while (remaining != 0) {
        --remaining;
        if (fpCallBack(mesh, meshHeader, data) == nullptr) {
            break;
        }
        ++mesh;
    }
    return meshHeader;
}

// 0x758D70 (lote R18A, sin validar; sustituye la envoltura; cdecl, 4 argumentos, 329 bytes): escribe la cabecera {flags, numMeshes (32 bits), totalIndicesInMesh} (12 bytes) con
// RwStreamWriteInt32 (0x7ED460) y, por malla (desde cabecera + 0x10, SIN sumar firstMeshOffset, como el EXE), {numIndices, indice del material en matList (negativo -> 0)} (8 bytes,
// _rpMaterialListFindMaterialIndex) y, si el objeto no es nativo (tipo 8 o 7 sin 0x1000000), los indices como dwords en bloques de 0x100. Cualquier escritura que devuelva NULL
// da NULL. Devuelve el flujo.
RwStream* _rpMeshWrite(const RpMeshHeader* meshHeader, const void* object, RwStream* stream, const RpMaterialList* matList) {
    struct { RwUInt32 flags; RwUInt32 numMeshes; RwUInt32 totalIndices; } header = { meshHeader->flags, meshHeader->numMeshes, meshHeader->totalIndicesInMesh };
    if (RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&header), 12) == nullptr) {
        return nullptr;
    }
    const RpMesh* mesh = reinterpret_cast<const RpMesh*>(reinterpret_cast<const uint8_t*>(meshHeader) + 0x10);
    RwUInt32 remaining = meshHeader->numMeshes;
    while (remaining != 0) {
        --remaining;
        struct { RwUInt32 numIndices; RwInt32 matIndex; } entry;
        entry.numIndices = mesh->numIndices;
        entry.matIndex   = _rpMaterialListFindMaterialIndex(matList, mesh->material);
        if (entry.matIndex < 0) {
            entry.matIndex = 0;
        }
        if (RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&entry), 8) == nullptr) {
            return nullptr;
        }
        if (MeshObjectStoresIndices(object)) {
            RwUInt32 left = mesh->numIndices;
            const RxVertexIndex* source = mesh->indices;
            while (left != 0) {
                RwInt32 chunk[kMeshStreamChunk];
                const RwUInt32 count = left < kMeshStreamChunk ? left : kMeshStreamChunk;
                for (RwUInt32 i = 0; i < count; ++i) {
                    chunk[i] = static_cast<RwInt32>(source[i]);
                }
                source += count;
                if (RwStreamWriteInt32(stream, chunk, count * 4) == nullptr) {
                    return nullptr;
                }
                left -= count;
            }
        }
        ++mesh;
    }
    return stream;
}

// 0x758EC0 (lote R18A, sin validar; sustituye la envoltura; cdecl, 3 argumentos, 462 bytes): lee {flags, numMeshes, totalIndicesInMesh} (RwStreamReadInt32 0x7ED540; NULL -> NULL).
// Reserva (numMeshes + 1) * 16 bytes (+ totalIndicesInMesh * 2 si el objeto no es nativo; RwMalloc, pista 0x30502; sin error si falla: devuelve NULL). Rellena la cabecera:
// flags, numMeshes (16 bits), serialNum = contador de serie global (RwEngineInstance + [0xC9B8C0], 16 bits, que luego se incrementa), totalIndicesInMesh y firstMeshOffset = 0. Por
// malla (a partir de cabecera + 0x10; los indices van tras las numMeshes mallas de 12 bytes) lee {numIndices, indice de material}: numIndices, material =
// _rpMaterialListGetMaterial(matList, indice), indices = cursor, y si el objeto no es nativo lee los indices como dwords en bloques de 0x100 y los guarda como 16 bits. Un fallo de
// lectura devuelve NULL (sin liberar la cabecera, como el EXE). Devuelve la cabecera.
RpMeshHeader* _rpMeshRead(RwStream* stream, const void* object, const RpMaterialList* matList) {
    struct { RwUInt32 flags; RwUInt32 numMeshes; RwUInt32 totalIndices; } info;
    if (RwStreamReadInt32(stream, reinterpret_cast<RwInt32*>(&info), 12) == nullptr) {
        return nullptr;
    }
    const bool withIndices = MeshObjectStoresIndices(object);
    RwUInt32 size = (info.numMeshes + 1) << 4;
    if (withIndices) {
        size = size + info.totalIndices * 2;
    }
    RpMeshHeader* const header = static_cast<RpMeshHeader*>(RwMalloc(size, kBuildMeshFreeListHint));
    if (header == nullptr) {
        return nullptr;
    }
    RpMesh* mesh = reinterpret_cast<RpMesh*>(reinterpret_cast<uint8_t*>(header) + 0x10);
    header->flags     = info.flags;
    header->numMeshes = static_cast<RwUInt16>(info.numMeshes);
    RxVertexIndex* indexCursor = reinterpret_cast<RxVertexIndex*>(mesh + info.numMeshes);
    RwUInt16& serial = *reinterpret_cast<RwUInt16*>(reinterpret_cast<uintptr_t>(static_cast<RwGlobals*>(RwEngineInstance)) + static_cast<uintptr_t>(s_meshSerialOffset));
    header->serialNum          = serial;
    header->totalIndicesInMesh = info.totalIndices;
    header->firstMeshOffset    = 0;
    serial = static_cast<RwUInt16>(serial + 1);
    RwUInt32 remaining = header->numMeshes;
    while (remaining != 0) {
        --remaining;
        struct { RwUInt32 numIndices; RwInt32 matIndex; } entry;
        if (RwStreamReadInt32(stream, reinterpret_cast<RwInt32*>(&entry), 8) == nullptr) {
            return nullptr;
        }
        mesh->numIndices = entry.numIndices;
        mesh->material   = _rpMaterialListGetMaterial(matList, entry.matIndex);
        mesh->indices    = indexCursor;
        if (withIndices) {
            RwUInt32 left = mesh->numIndices;
            while (left != 0) {
                RwInt32 chunk[kMeshStreamChunk];
                const RwUInt32 count = left < kMeshStreamChunk ? left : kMeshStreamChunk;
                if (RwStreamReadInt32(stream, chunk, count * 4) == nullptr) {
                    return nullptr;
                }
                left -= count;
                for (RwUInt32 i = 0; i < count; ++i) {
                    *indexCursor++ = static_cast<RxVertexIndex>(chunk[i]);
                }
            }
        }
        ++mesh;
    }
    return header;
}

// 0x759090 (lote R18A, sin validar; sustituye la envoltura; cdecl, 2 argumentos, 77 bytes): tamano en flujo de la malla. Objeto nativo (tipo 8 o 7 con 0x1000000 en +0x8):
// numMeshes * 8 + 12; si no (totalIndicesInMesh + numMeshes * 2) * 4 + 12.
RwInt32 _rpMeshSize(const RpMeshHeader* meshHeader, const void* object) {
    if (MeshObjectIsNative(object)) {
        return static_cast<RwInt32>(static_cast<RwUInt32>(meshHeader->numMeshes) * 8 + 0xC);
    }
    return static_cast<RwInt32>((meshHeader->totalIndicesInMesh + static_cast<RwUInt32>(meshHeader->numMeshes) * 2) * 4 + 0xC);
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

// ---------------------------------------------------------------------------------------------------------------------
// Lote R19A (sin validar): generador de tiras por defecto de RpBuildMesh (0x7591D0) y sus rutinas internas (0x759790, 0x75A090, 0x75A1B0, 0x75A250, 0x75A370).
// Estructuras (nombres inferidos): nodo de triangulo (0x24 bytes de lista libre), arista (0x10) y lista de indices de tira (0x10, lista libre propia en
// RwEngineInstance + [0xC9B8C0] + 4, creada y destruida por 0x7591D0).
// ---------------------------------------------------------------------------------------------------------------------
namespace {
constexpr RwUInt32 kStripListHint   = 0x10502;   // lista libre de listas de tiras, buffers temporales y mallas intermedias
constexpr RwUInt32 kStripNodeHint   = 0x30502;   // nodos, aristas, vector de nodos, resultados de 0x759790
constexpr RwUInt32 kStripEntriesHint = 0x1010502; // vector de mallas terminadas

static_assert(sizeof(RpBuildStripEdge) == 0x10 && offsetof(RpBuildStripEdge, tri1) == 4 && offsetof(RpBuildStripEdge, tri2) == 8 && offsetof(RpBuildStripEdge, next) == 0xC);
static_assert(offsetof(RpBuildStripNode, edges) == 4 && offsetof(RpBuildStripNode, next) == 0x10 && offsetof(RpBuildStripNode, prev) == 0x14);
static_assert(offsetof(RpBuildStripNode, used) == 0x18 && offsetof(RpBuildStripNode, trialUsed) == 0x1C && offsetof(RpBuildStripNode, numOpenNeighbours) == 0x20);
static_assert(sizeof(RpBuildStripList) == 0x10 && offsetof(RpBuildStripList, numIndices) == 4 && offsetof(RpBuildStripList, capacity) == 8 && offsetof(RpBuildStripList, next) == 0xC);

// Lista libre de RpBuildStripList: *(RwFreeList**)(RwEngineInstance + [0xC9B8C0] + 4). El EXE la lee en cada uso.
inline RwFreeList*& MeshStripFreeListSlot() {
    return *reinterpret_cast<RwFreeList**>(reinterpret_cast<uintptr_t>(static_cast<RwGlobals*>(RwEngineInstance)) + static_cast<uintptr_t>(s_meshSerialOffset) + 4);
}

// 1 si el nodo existe y no esta consumido (flag +0x1C en las pasadas de prueba, +0x18 en la definitiva).
inline RwInt32 StripNodeOpen(const RpBuildStripNode* node, bool trial) {
    return (node != nullptr && (trial ? node->trialUsed : node->used) == 0) ? 1 : 0;
}
inline RwInt32 StripEdgeOpen(const RpBuildStripEdge* edge, bool trial) {
    return StripNodeOpen(edge->tri1, trial) + StripNodeOpen(edge->tri2, trial);
}
// Quita un nodo de la lista de su cubo (heads[numOpenNeighbours]).
inline void StripNodeUnlink(RpBuildStripNode* node, RpBuildStripNode** heads) {
    RpBuildStripNode** const slot = &heads[node->numOpenNeighbours];
    if (*slot == node) {
        *slot = node->next;
        RpBuildStripNode* const head = heads[node->numOpenNeighbours];
        if (head != nullptr) {
            head->prev = nullptr;
        }
    } else {
        if (node->next != nullptr) {
            node->next->prev = node->prev;
        }
        if (node->prev != nullptr) {
            node->prev->next = node->next;
        }
    }
}
}

// 0x75A1B0 (lote R19A, sin validar; nombre inferido; cdecl, 6 argumentos (lista libre de aristas, cabeza de la lista de aristas, indice origen (16 bits), indice destino (16 bits),
// indice del triangulo, vector de nodos), 151 bytes; llamador 0x75A090): busca en la lista de aristas una arista INVERSA (vertB == origen y vertA == destino) que aun no tenga
// segundo triangulo; si la hay, incrementa el contador de vecinos abiertos (byte +0x20) de su primer triangulo y del triangulo actual y pone el actual como tri2. Si no, crea una
// arista (lista libre, pista 0x30502) con vertA = origen, vertB = destino, tri1 = nodo actual, tri2 = NULL, y la pone a la cabeza. Devuelve la arista.
RpBuildStripEdge* _rpBuildStripEdgeFind(RwFreeList* edgeFreeList, RpBuildStripEdge** edgeHead, RwUInt16 from, RwUInt16 to, RwUInt32 nodeIndex, RpBuildStripNode** nodes) {
    for (RpBuildStripEdge* edge = *edgeHead; edge != nullptr; edge = edge->next) {
        if (edge->vertB == from && edge->vertA == to && edge->tri2 == nullptr) {
            edge->tri1->numOpenNeighbours = static_cast<RwUInt8>(edge->tri1->numOpenNeighbours + 1);
            nodes[nodeIndex]->numOpenNeighbours = static_cast<RwUInt8>(nodes[nodeIndex]->numOpenNeighbours + 1);
            edge->tri2 = nodes[nodeIndex];
            return edge;
        }
    }
    RpBuildStripEdge* const edge = static_cast<RpBuildStripEdge*>(RwFreeListAlloc(edgeFreeList, kStripNodeHint));
    edge->vertA = from;
    edge->vertB = to;
    edge->tri2  = nullptr;
    edge->tri1  = nodes[nodeIndex];
    edge->next  = *edgeHead;
    *edgeHead   = edge;
    return edge;
}

// 0x75A090 (lote R19A, sin validar; nombre inferido; cdecl, 4 argumentos (numero de triangulos, RwFreeList[2] de salida, cabeza de aristas, triangulos), 273 bytes; llamador 0x759790):
// reserva el vector de n punteros a nodo (RwMalloc, pista 0x30502), la lista libre de nodos (0x24 bytes, n por bloque, alineacion 4) en freeLists[0] y la de aristas (0x10 bytes,
// n/2 + 1 por bloque) en freeLists[1] (ambas con pista 0x30502). Para cada triangulo crea un nodo (indice, prev/next/used = 0, contador de vecinos 0) y sus tres aristas dirigidas
// (v0->v1, v1->v2, v2->v0) con 0x75A1B0, guardandolas en +4, +8 y +0xC. Devuelve el vector de nodos.
RpBuildStripNode** _rpBuildStripNodesCreate(RwUInt32 numTriangles, RwFreeList** freeLists, RpBuildStripEdge** edgeHead, RpBuildMeshTriangle* triangles) {
    RpBuildStripNode** const nodes = static_cast<RpBuildStripNode**>(RwMalloc(numTriangles * 4, kStripNodeHint));
    freeLists[0] = RwFreeListCreate(0x24, static_cast<RwInt32>(numTriangles), 4, kStripNodeHint);
    freeLists[1] = RwFreeListCreate(0x10, static_cast<RwInt32>((numTriangles >> 1) + 1), 4, kStripNodeHint);
    for (RwUInt32 i = 0; i < numTriangles; ++i) {
        RpBuildStripNode* const node = static_cast<RpBuildStripNode*>(RwFreeListAlloc(freeLists[0], kStripNodeHint));
        nodes[i] = node;
        node->numOpenNeighbours = 0;
        node->index = i;
        node->prev  = nullptr;
        node->next  = nullptr;
        node->used  = 0;
        const RwUInt16* const v = triangles[i].vertIndex;
        node->edges[0] = _rpBuildStripEdgeFind(freeLists[1], edgeHead, v[0], v[1], i, nodes);
        node->edges[1] = _rpBuildStripEdgeFind(freeLists[1], edgeHead, v[1], v[2], i, nodes);
        node->edges[2] = _rpBuildStripEdgeFind(freeLists[1], edgeHead, v[2], v[0], i, nodes);
    }
    return nodes;
}

// 0x75A250 (lote R19A, sin validar; nombre inferido; cdecl, 3 argumentos (nodo, cubos, pasada), `ret`, 273 bytes; llamadores 0x759790 y 0x75A370): con pasada < 4 (prueba) solo marca
// el nodo en el flag de prueba (+0x1C). Con pasada >= 4 lo marca consumido (+0x18), lo saca de su cubo y, por cada una de sus tres aristas, toma el vecino sin consumir (tri1 si no es
// el propio nodo ni esta consumido; si no tri2 si no esta consumido), lo saca de su cubo, le resta uno al contador de vecinos abiertos (byte) y lo pone a la cabeza del cubo nuevo.
void _rpBuildStripNodeConsume(RpBuildStripNode* node, RpBuildStripNode** heads, RwInt32 pass) {
    if (pass < 4) {
        node->trialUsed = 1;
        return;
    }
    node->used = 1;
    StripNodeUnlink(node, heads);
    for (RwInt32 k = 0; k < 3; ++k) {
        const RpBuildStripEdge* const edge = node->edges[k];
        RpBuildStripNode* neighbour = edge->tri1;
        if (!(neighbour != nullptr && neighbour != node && neighbour->used == 0)) {
            neighbour = edge->tri2;
            if (neighbour == nullptr || neighbour->used != 0) {
                continue;
            }
        }
        StripNodeUnlink(neighbour, heads);
        neighbour->numOpenNeighbours = static_cast<RwUInt8>(neighbour->numOpenNeighbours - 1);
        RpBuildStripNode* const head = heads[neighbour->numOpenNeighbours];
        neighbour->next = head;
        if (head != nullptr) {
            head->prev = neighbour;
        }
        heads[neighbour->numOpenNeighbours] = neighbour;
        neighbour->prev = nullptr;
    }
}

// 0x75A370 (lote R19A, sin validar; nombre inferido; cdecl, 5 argumentos (tira, arista, cubos, triangulos (no se usa), pasada), 750 bytes; llamador 0x759790): alarga la tira a traves
// de la arista dada. Cruza al triangulo abierto de la arista (tri1 si esta abierto, si no tri2; sin abierto devuelve lo contado), lo consume (0x75A250) y cuenta; el vertice nuevo es el
// opuesto a la arista cruzada. La arista siguiente es la primera de las tres del triangulo que une (ultimo vertice de la tira, vertice nuevo) en cualquier orden. Con arista siguiente con
// triangulos abiertos anade el vertice y sigue por ella. Si no, busca otra arista del triangulo (distinta de la cruzada y de la siguiente; si no hay, la ultima candidata); si tiene
// abiertos y la tira tiene longitud par, anade un vertice duplicado (copia de indices[n-2]) mas el nuevo y sigue por ella; en cualquier otro caso anade el vertice nuevo y termina.
// Abierto = no consumido, flag +0x1C con pasada < 4 y +0x18 con pasada >= 4. Devuelve el numero de triangulos consumidos.
RwUInt32 _rpBuildStripExtend(RpBuildStripList* strip, RpBuildStripEdge* edge, RpBuildStripNode** heads, RpBuildMeshTriangle* triangles, RwInt32 pass) {
    (void)triangles;
    const bool trial = pass < 4;
    RwUInt32 count = 0;
    RpBuildStripEdge* candidate = nullptr;
    RpBuildStripEdge* current = edge;
    if (current == nullptr) {
        return 0;
    }
    for (;;) {
        RpBuildStripNode* tri = current->tri1;
        if (!(tri != nullptr && StripNodeOpen(tri, trial) == 1)) {
            tri = current->tri2;
            if (tri == nullptr) {
                return count;
            }
            if (StripNodeOpen(tri, trial) == 0) {
                return count;
            }
        }
        ++count;
        _rpBuildStripNodeConsume(tri, heads, pass);

        RwInt32 crossed = -1;
        if (current == tri->edges[0]) {
            crossed = 0;
        } else if (current == tri->edges[1]) {
            crossed = 1;
        } else if (current == tri->edges[2]) {
            crossed = 2;
        }
        const RpBuildStripEdge* const oppositeEdge = tri->edges[(crossed + 1) % 3];
        const RwUInt16 apex = (oppositeEdge->tri1 == tri) ? oppositeEdge->vertB : oppositeEdge->vertA;

        const RwUInt32 n = strip->numIndices;
        RwUInt16* const buffer = strip->indices;
        const RwUInt16 last = buffer[n - 1];
        const auto joins = [&](const RpBuildStripEdge* e) {
            return (e->vertA == last && e->vertB == apex) || (e->vertA == apex && e->vertB == last);
        };
        RpBuildStripEdge* next;
        if (joins(tri->edges[0])) {
            next = tri->edges[0];
        } else if (joins(tri->edges[1])) {
            next = tri->edges[1];
        } else if (joins(tri->edges[2])) {
            next = tri->edges[2];
        } else {
            next = nullptr;
        }

        if (next != nullptr && StripEdgeOpen(next, trial) > 0) {
            buffer[n] = apex;
            strip->numIndices = strip->numIndices + 1;
            current = next;
            continue;
        }

        RpBuildStripEdge* other;
        if (tri->edges[0] != current && tri->edges[0] != next) {
            other = tri->edges[0];
            candidate = other;
        } else if (tri->edges[1] != current && tri->edges[1] != next) {
            other = tri->edges[1];
            candidate = other;
        } else if (tri->edges[2] != current && tri->edges[2] != next) {
            other = tri->edges[2];
            candidate = other;
        } else {
            other = candidate;
        }

        if (other != nullptr && StripEdgeOpen(other, trial) > 0 && (n & 1) == 0) {
            buffer[n] = buffer[n - 2];
            strip->numIndices = strip->numIndices + 1;
            strip->indices[strip->numIndices] = apex;
            strip->numIndices = strip->numIndices + 1;
            current = other;
            continue;
        }

        strip->indices[strip->numIndices] = apex;
        strip->numIndices = strip->numIndices + 1;
        return count;
    }
}

// 0x759790 (lote R19A, sin validar; nombre inferido; cdecl, 4 argumentos (triangulos, cantidad, RpBuildStripList** de salida, probar rotaciones), 2302 bytes; llamador 0x7591D0): genera
// las tiras de un grupo de triangulos del mismo material. Crea nodos/aristas (0x75A090), reparte los nodos en cuatro cubos segun sus vecinos abiertos y reserva dos RpBuildStripList
// (tira y auxiliar, capacidad 2n + 2 indices, buffers de 4 * (2n + 2) bytes, pista 0x10502). Mientras queden triangulos: un nodo sin vecinos (cubo 0) va solo como tira de 3 indices; si no
// toma el primer nodo del primer cubo no vacio, elige la rotacion de salida r con el recuento de triangulos abiertos de sus tres aristas (cascada del EXE) y prueba pasadas: con
// probarRotaciones != 0 las rotaciones r, r+1, r+2 (mod 3) con flags de prueba y por ultimo la mejor; con 0 solo la definitiva (rotacion r). Cada pasada copia used a trialUsed, pone los
// tres indices rotados, consume el nodo y alarga por la segunda arista (0x75A370); si la primera arista aun tiene triangulos abiertos alarga tambien hacia atras con la lista auxiliar
// (invertida, con duplicado si es impar). La pasada definitiva guarda una RpBuildStripList nueva (pista 0x30502) a la cabeza de *outList con la parte hacia atras invertida seguida de la
// tira (copia de 4 bytes por indice, como el EXE). Al final libera aristas, nodos, listas libres y buffers. Devuelve TRUE.
RwBool _rpBuildStripListGenerate(RpBuildMeshTriangle* triangles, RwUInt32 numTriangles, RpBuildStripList** outList, RwInt32 tryRotations) {
    RpBuildStripEdge* edgeHead = nullptr;
    RwFreeList* freeLists[2] = { nullptr, nullptr };
    RpBuildStripNode* heads[4] = { nullptr, nullptr, nullptr, nullptr };
    RpBuildStripNode** const nodes = _rpBuildStripNodesCreate(numTriangles, freeLists, &edgeHead, triangles);

    for (RwUInt32 i = 0; i < numTriangles; ++i) {
        RpBuildStripNode* const node = nodes[i];
        node->next = heads[node->numOpenNeighbours];
        if (node->next != nullptr) {
            node->next->prev = node;
        }
        heads[nodes[i]->numOpenNeighbours] = nodes[i];
        nodes[i]->prev = nullptr;
    }

    RpBuildStripList* const strip = static_cast<RpBuildStripList*>(RwFreeListAlloc(MeshStripFreeListSlot(), kStripListHint));
    const RwUInt32 capacity = numTriangles * 2 + 2;
    strip->numIndices = 0;
    strip->capacity   = capacity;
    strip->indices    = static_cast<RwUInt16*>(RwMalloc(capacity * 4, kStripListHint));
    RpBuildStripList* const work = static_cast<RpBuildStripList*>(RwFreeListAlloc(MeshStripFreeListSlot(), kStripListHint));
    work->capacity   = capacity;
    work->numIndices = 0;
    work->indices    = static_cast<RwUInt16*>(RwMalloc(capacity * 4, kStripListHint));

    if (numTriangles != 0) {
        RwUInt32 processed = 0;
        do {
            RwInt32 mode = tryRotations != 0 ? 0 : 3;
            RwUInt32 bestTotal = 0;
            if (heads[0] != nullptr) {
                // Triangulo sin vecinos abiertos: tira de un solo triangulo.
                RpBuildStripNode* const node = heads[0];
                const RwUInt32 triIndex = node->index;
                RpBuildStripList* const single = static_cast<RpBuildStripList*>(RwFreeListAlloc(MeshStripFreeListSlot(), kStripNodeHint));
                single->next = *outList;
                *outList = single;
                single->numIndices = 3;
                single->capacity   = 3;
                single->indices    = static_cast<RwUInt16*>(RwMalloc(0xC, kStripNodeHint));
                const RpBuildMeshTriangle* const tri = &triangles[triIndex];
                single->indices[0] = tri->vertIndex[0];
                single->indices[1] = tri->vertIndex[1];
                single->indices[2] = tri->vertIndex[2];
                node->used      = 1;
                node->trialUsed = 1;
                RpBuildStripNode* const nextNode = node->next;
                heads[0] = nextNode;
                if (nextNode != nullptr) {
                    nextNode->prev = nullptr;
                }
                ++processed;
                continue;
            }

            RwInt32 bucket = 1;
            while (bucket < 3 && heads[bucket] == nullptr) {
                ++bucket;
            }
            RpBuildStripNode** const slot = &heads[bucket];
            RpBuildStripNode* const start = *slot;

            // Rotacion de partida r segun los triangulos abiertos de las aristas (cascada del EXE).
            RwInt32 rotation;
            {
                RpBuildStripEdge* const e0 = start->edges[0];
                RpBuildStripEdge* const e1 = start->edges[1];
                RpBuildStripEdge* const e2 = start->edges[2];
                if (StripEdgeOpen(e2, false) >= 2 && StripEdgeOpen(e1, false) >= 2) {
                    rotation = 1;
                } else if (StripEdgeOpen(e0, false) >= 2 && StripEdgeOpen(e2, false) >= 2) {
                    rotation = 2;
                } else if (StripEdgeOpen(e1, false) >= 2 && StripEdgeOpen(e0, false) >= 2) {
                    rotation = 0;
                } else {
                    const RwInt32 open0 = StripEdgeOpen(e0, false);
                    const RwInt32 open1 = StripEdgeOpen(e1, false);
                    const RwInt32 open2 = StripEdgeOpen(e2, false);
                    if (open0 > open1) {
                        rotation = (open0 > open2 ? 1 : 0) + 1;
                    } else {
                        rotation = open1 <= open2 ? 1 : 0;
                    }
                }
            }
            const RwUInt32 processedBefore = processed;
            RwInt32 bestRotation = rotation;
            const RwInt32 baseRotation = rotation;
            RpBuildStripEdge* firstEdge  = nullptr;
            RpBuildStripEdge* secondEdge = nullptr;

            for (;;) {
                for (RwUInt32 i = 0; i < numTriangles; ++i) {
                    nodes[i]->trialUsed = nodes[i]->used;
                }
                const RwInt32 pass = mode;
                ++mode;
                RwInt32 rot;
                switch (pass) {
                case 0: rot = static_cast<RwInt32>(static_cast<RwUInt32>(baseRotation) % 3); break;
                case 1: rot = static_cast<RwInt32>(static_cast<RwUInt32>(baseRotation + 1) % 3); break;
                case 2: rot = static_cast<RwInt32>(static_cast<RwUInt32>(baseRotation + 2) % 3); break;
                default: rot = bestRotation; break;
                }
                RpBuildStripNode* const node = *slot;
                switch (rot) {
                case 0: firstEdge = node->edges[0]; secondEdge = node->edges[1]; break;
                case 1: firstEdge = node->edges[1]; secondEdge = node->edges[2]; break;
                case 2: firstEdge = node->edges[2]; secondEdge = node->edges[0]; break;
                default: break;
                }
                const RpBuildMeshTriangle* const tri = &triangles[node->index];
                strip->indices[0] = tri->vertIndex[static_cast<RwUInt32>(rot) % 3];
                strip->indices[1] = tri->vertIndex[static_cast<RwUInt32>(rot + 1) % 3];
                strip->indices[2] = tri->vertIndex[static_cast<RwUInt32>(rot + 2) % 3];
                strip->numIndices = 3;
                _rpBuildStripNodeConsume(node, heads, mode);
                RwUInt32 total = _rpBuildStripExtend(strip, secondEdge, heads, triangles, mode) + processedBefore + 1;
                processed = total;

                const bool backward = StripEdgeOpen(firstEdge, mode < 4) > 0;
                if (backward) {
                    work->indices[0] = strip->indices[1];
                    work->indices[1] = strip->indices[0];
                    work->numIndices = 2;
                    total += _rpBuildStripExtend(work, firstEdge, heads, triangles, mode);
                    processed = total;
                    const RwUInt32 workCount = work->numIndices;
                    if (workCount & 1) {
                        work->indices[workCount] = work->indices[workCount - 2];
                        work->numIndices = work->numIndices + 1;
                    }
                }
                if (total > bestTotal) {
                    bestTotal    = total;
                    bestRotation = rot;
                }
                if (mode < 4) {
                    continue;
                }

                if (backward) {
                    RpBuildStripList* const joined = static_cast<RpBuildStripList*>(RwFreeListAlloc(MeshStripFreeListSlot(), kStripNodeHint));
                    joined->next = *outList;
                    *outList = joined;
                    joined->numIndices = 0;
                    joined->capacity   = strip->numIndices + work->numIndices - 2;
                    joined->indices    = static_cast<RwUInt16*>(RwMalloc(joined->capacity * 4, kStripNodeHint));
                    if (work->numIndices > 2) {
                        do {
                            joined->indices[joined->numIndices] = work->indices[work->numIndices - 1];
                            joined->numIndices = joined->numIndices + 1;
                            work->numIndices   = work->numIndices - 1;
                        } while (work->numIndices > 2);
                    }
                    std::memcpy(joined->indices + joined->numIndices, strip->indices, static_cast<size_t>(strip->numIndices) * 4);
                    joined->numIndices = joined->capacity;
                } else {
                    RpBuildStripList* const single = static_cast<RpBuildStripList*>(RwFreeListAlloc(MeshStripFreeListSlot(), kStripNodeHint));
                    single->next = *outList;
                    *outList = single;
                    single->capacity   = strip->numIndices;
                    single->numIndices = strip->numIndices;
                    single->indices    = static_cast<RwUInt16*>(RwMalloc(strip->numIndices * 4, kStripNodeHint));
                    std::memcpy(single->indices, strip->indices, static_cast<size_t>(strip->numIndices) * 4);
                }
                break;
            }
        } while (processed < numTriangles);
    }

    RwFree(work->indices);
    work->indices = nullptr;
    RwFreeListFree(MeshStripFreeListSlot(), work);
    RwFree(strip->indices);
    strip->indices = nullptr;
    RwFreeListFree(MeshStripFreeListSlot(), strip);

    if (edgeHead != nullptr) {
        RpBuildStripEdge* edge = edgeHead;
        RpBuildStripEdge* following;
        do {
            following = edge->next;
            RwFreeListFree(freeLists[1], edge);
            edge = following;
        } while (following != nullptr);
    }
    RwFreeListDestroy(freeLists[1]);
    freeLists[1] = nullptr;
    for (RwUInt32 i = 0; i < numTriangles; ++i) {
        RwFreeListFree(freeLists[0], nodes[i]);
        nodes[i] = nullptr;
    }
    RwFreeListDestroy(freeLists[0]);
    RwFree(nodes);
    return TRUE;
}

// 0x7591D0 (lote R19A, sin validar; nombre inferido; cdecl, 3 argumentos (RpBuildMesh, probar rotaciones, modo de union), 1134 bytes; llamador 0x7591B0 con (malla, 0, 1)): genera la
// RpMeshHeader de tiras de una RpBuildMesh. Hace un vector de punteros a los triangulos (RwMalloc numTriangulos * 4, pista 0x10502; NULL -> NULL) y lo ordena con el qsort del EXE
// (0x8247E0, no estable) y el comparador 0x759640 (por direccion fija). Cuenta los grupos de material consecutivos (puntero de material en +8), reserva el vector de mallas terminadas
// (grupos * 4, pista 0x1010502) y la tabla de grupos (12 bytes: 0, cantidad, material; grupos * 12, pista 0x10502), crea la lista libre de RpBuildStripList (0x10 bytes, n/10 + 5 por
// bloque, alineacion 4, pista 0x10502) y la deja en RwEngineInstance + [0xC9B8C0] + 4. Por grupo copia sus triangulos (20 bytes) a un vector propio, genera las tiras (0x759790),
// las une con 0x75A660 (por direccion fija) y convierte cada resultado en un bloque {indices, cantidad, material, indices inline} (RwMalloc cantidad * 2 + 12) guardado en el vector de
// mallas con contador de 16 bits. Destruye la lista libre y deja el puntero a 0. Crea la cabecera con _rpMeshHeaderCreate (16 + suma de (12 + 2 * cantidad)): flags 1, numMeshes de 16
// bits, serialNum = contador de serie global (que incrementa), totalIndicesInMesh, firstMeshOffset 0; escribe las RpMesh y copia los indices tras ellas. Libera los bloques, el vector de
// punteros, el de mallas y la tabla. Con 0 triangulos el EXE lee ptrs[0] sin inicializar (se conserva).
RpMeshHeader* _rpBuildMeshGenerateTriStrip(RpBuildMesh* buildMesh, RwInt32 tryRotations, RwInt32 joinMode) {
    struct GroupRecord { RwUInt32 unused; RwUInt32 count; RpMaterial* material; };
    static_assert(sizeof(GroupRecord) == 12);

    const RwUInt32 numTriangles = buildMesh->numTriangles;
    RpBuildMeshTriangle** const pointers = static_cast<RpBuildMeshTriangle**>(RwMalloc(numTriangles * 4, kStripListHint));
    if (pointers == nullptr) {
        return nullptr;
    }
    for (RwUInt32 i = 0; i < numTriangles; ++i) {
        pointers[i] = &buildMesh->meshTriangles[i];
    }
    using CompareFn = int(__cdecl*)(const void*, const void*);
    using QsortFn = void(__cdecl*)(void*, size_t, size_t, CompareFn);
    reinterpret_cast<QsortFn>(0x8247E0)(pointers, numTriangles, 4, reinterpret_cast<CompareFn>(0x759640));

    RwUInt32 numGroups = 1;
    if (buildMesh->numTriangles >= 2) {
        RpMaterial* current = pointers[0]->material;
        for (RwUInt32 i = 1; i < buildMesh->numTriangles; ++i) {
            RpMaterial* const material = pointers[i]->material;
            if (material != current) {
                current = material;
                ++numGroups;
            }
        }
    }

    RpMesh** const finished = static_cast<RpMesh**>(RwMalloc(numGroups * 4, kStripEntriesHint));
    GroupRecord* const groups = static_cast<GroupRecord*>(RwMalloc(numGroups * 12, kStripListHint));
    groups[0].material = pointers[0]->material;
    groups[0].unused   = 0;
    groups[0].count    = 0;
    numGroups = 1;
    if (buildMesh->numTriangles >= 2) {
        GroupRecord* record = &groups[0];
        for (RwUInt32 i = 0; i < buildMesh->numTriangles - 1; ++i) {
            RpMaterial* const nextMaterial = pointers[i + 1]->material;
            if (pointers[i]->material != nextMaterial) {
                record[1].material = nextMaterial;
                record[1].unused   = 0;
                record[1].count    = i + 1;
                record->count      = i - record->count + 1;
                ++numGroups;
                ++record;
            }
        }
    }
    GroupRecord& lastGroup = groups[numGroups - 1];
    lastGroup.count = buildMesh->numTriangles - lastGroup.count;

    MeshStripFreeListSlot() = RwFreeListCreate(0x10, static_cast<RwInt32>(buildMesh->numTriangles / 10 + 5), 4, kStripListHint);

    RpBuildStripList* list = nullptr;
    RwUInt32 numStrips = 0;
    RpBuildMeshTriangle** cursor = pointers;
    if (numGroups != 0) {
        RwUInt32 remainingGroups = numGroups;
        const GroupRecord* record = &groups[0];
        do {
            RpBuildMeshTriangle* const copy = static_cast<RpBuildMeshTriangle*>(RwMalloc(record->count * 20, kStripListHint));
            for (RwUInt32 i = 0; i < record->count; ++i) {
                std::memcpy(&copy[i], *cursor, sizeof(RpBuildMeshTriangle));
                ++cursor;
            }
            _rpBuildStripListGenerate(copy, record->count, &list, tryRotations);
            reinterpret_cast<void(__cdecl*)(RpBuildStripList**, RwInt32)>(0x75A660)(&list, joinMode);

            for (RpBuildStripList* strip = list; strip != nullptr; strip = strip->next) {
                // Bloque RpMesh {indices, cantidad, material} seguido de sus indices (12 + 2 * cantidad bytes).
                RpMesh* const block = static_cast<RpMesh*>(RwMalloc(strip->numIndices * 2 + 0xC, kStripListHint));
                block->material   = record->material;
                block->numIndices = strip->numIndices;
                block->indices    = reinterpret_cast<RxVertexIndex*>(block + 1);
                for (RwUInt32 i = 0; i < strip->numIndices; ++i) {
                    block->indices[i] = strip->indices[i];
                }
                finished[numStrips & 0xFFFF] = block;
                ++numStrips;
            }
            while (list != nullptr) {
                RpBuildStripList* const entry = list;
                list = entry->next;
                RwFree(entry->indices);
                entry->indices = nullptr;
                RwFreeListFree(MeshStripFreeListSlot(), entry);
            }
            RwFree(copy);
            ++record;
            --remainingGroups;
        } while (remainingGroups != 0);
    }
    RwFreeListDestroy(MeshStripFreeListSlot());
    MeshStripFreeListSlot() = nullptr;

    const RwUInt32 numMeshes = numStrips & 0xFFFF;
    RwUInt32 totalIndices = 0;
    RwUInt32 size = 0x10;
    for (RwUInt32 i = 0; i < numMeshes; ++i) {
        const RwUInt32 count = finished[i]->numIndices;
        totalIndices += count;
        size = size + count * 2 + 0xC;
    }
    RpMeshHeader* const header = _rpMeshHeaderCreate(size);
    header->flags     = 1;
    header->numMeshes = static_cast<RwUInt16>(numStrips);
    RwUInt16& serial = *reinterpret_cast<RwUInt16*>(reinterpret_cast<uintptr_t>(static_cast<RwGlobals*>(RwEngineInstance)) + static_cast<uintptr_t>(s_meshSerialOffset));
    header->firstMeshOffset    = 0;
    header->serialNum          = serial;
    header->totalIndicesInMesh = totalIndices;
    serial = static_cast<RwUInt16>(serial + 1);

    RpMesh* mesh = reinterpret_cast<RpMesh*>(reinterpret_cast<RwUInt8*>(header) + 0x10);
    RxVertexIndex* indexCursor = reinterpret_cast<RxVertexIndex*>(mesh + numMeshes);
    for (RwUInt32 i = 0; i < numMeshes; ++i) {
        const RpMesh* const block = finished[i];
        mesh->indices    = indexCursor;
        mesh->numIndices = block->numIndices;
        mesh->material   = block->material;
        std::memcpy(indexCursor, block->indices, static_cast<size_t>(mesh->numIndices) * 2);
        indexCursor += mesh->numIndices;
        RwFree(finished[i]);
        finished[i] = nullptr;
        ++mesh;
    }
    RwFree(pointers);
    RwFree(finished);
    RwFree(groups);
    return header;
}

// 0x7591B0: llama a _rpBuildMeshGenerateTriStrip (0x7591D0, lote R19A) con (buildMesh, 0, 1) y devuelve su resultado; `data` no se usa.
RpMeshHeader* RpBuildMeshGenerateDefaultTriStrip(RpBuildMesh* buildmesh, void* data) {
    return _rpBuildMeshGenerateTriStrip(buildmesh, 0, 1);
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

// 0x752110 (lote R16A, sin validar; sustituye la envoltura; cdecl, 1 argumento, 144 bytes): pide un RpLight a la lista libre (hint 0x30012; sin memoria devuelve NULL sin error) y lo deja asi:
// type 3, subType = tipo, flags 3, privateFlags 1 (color blanco), padre y radio a 0, color (1,1,1,1), sync = 0x7521A0 (por direccion), angulo menos-coseno 0, lista WorldSectorsInLight vacia (apunta
// a si misma), enlace inWorld a 0 y lightFrame = RwEngineInstance->lightFrame - 1 (16 bits); despues _rwPluginRegistryInitObject(registro de luces 0x8D62F8, luz).
RpLight* RpLightCreate(RwInt32 type) {
    RpLight* const light = static_cast<RpLight*>(RwFreeListAlloc(LightFreeList(), 0x30012));
    if (!light) {
        return nullptr;
    }
    light->object.object.type    = rpLIGHT;
    light->object.object.subType = static_cast<RwUInt8>(type);
    light->color.red   = 1.0f;
    light->color.green = 1.0f;
    light->color.blue  = 1.0f;
    light->color.alpha = 1.0f;
    light->object.object.flags  = 0;
    light->object.object.parent = nullptr;
    light->object.sync = reinterpret_cast<RwObjectHasFrameSyncFunction>(static_cast<uintptr_t>(0x7521A0));
    light->radius = 0.0f;
    light->minusCosAngle = 0.0f;
    light->object.object.privateFlags = 1;
    light->WorldSectorsInLight.link.next = &light->WorldSectorsInLight.link;
    light->WorldSectorsInLight.link.prev = &light->WorldSectorsInLight.link;
    light->inWorld.prev = nullptr;
    light->inWorld.next = nullptr;
    const RwUInt16 lightFrame = static_cast<RwGlobals*>(RwEngineInstance)->lightFrame;
    light->object.object.flags = 3;
    light->lightFrame = static_cast<RwUInt16>(lightFrame - 1);
    _rwPluginRegistryInitObject(&s_lightTKList, light);
    return light;
}

// 0x7520D0 (lote R16A, sin validar; sustituye la envoltura; cdecl, 1 argumento, 54 bytes): destruye los datos de plugin de la luz, suelta el marco (_rwObjectHasFrameReleaseFrame 0x804F40) y la devuelve
// a la lista libre (+0xC925D8). Devuelve siempre 1. 0x752210 es el mismo cuerpo sin valor de retorno (callback de la lista libre).
static void LightDestroyBody(RpLight* light) {
    _rwPluginRegistryDeInitObject(&s_lightTKList, light);
    _rwObjectHasFrameReleaseFrame(light);
    RwFreeListFree(LightFreeList(), light);
}

RwBool RpLightDestroy(RpLight* light) {
    LightDestroyBody(light);
    return TRUE;
}

// 0x751A70 (lote R16A, sin validar; sustituye la envoltura; cdecl, 2 argumentos): guarda el radio (+0x14) y, si la luz tiene marco, RwFrameUpdateObjects (0x7F0910) sobre el; devuelve la luz.
RpLight* RpLightSetRadius(RpLight* light, RwReal radius) {
    light->radius = radius;
    if (light->object.object.parent) {
        RwFrameUpdateObjects(static_cast<RwFrame*>(light->object.object.parent));
    }
    return light;
}

// Marca la luz como blanca (privateFlags = 1) si rojo == verde y rojo == azul (comparaciones x87 ordenadas: con NaN queda a 0), como los dos sitios del EXE (0x751A90 y 0x751F00).
static void LightUpdateWhiteFlag(RpLight* light) {
    light->object.object.privateFlags = (light->color.red == light->color.green && light->color.red == light->color.blue) ? 1 : 0;
}

// 0x751A90 (lote R16A, sin validar; sustituye la envoltura; cdecl, 2 argumentos, 77 bytes): copia el color (4 floats a +0x18) y recalcula el indicador de blanco (+3); devuelve la luz.
// Sin comprobar el alfa. Copia los cuatro dwords en orden (rojo, verde, azul, alfa) antes de comparar.
RpLight* RpLightSetColor(RpLight* light, const RwRGBAReal* color) {
    light->color.red   = color->red;
    light->color.green = color->green;
    light->color.blue  = color->blue;
    light->color.alpha = color->alpha;
    LightUpdateWhiteFlag(light);
    return light;
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

// 0x751F00 (lote R16A, sin validar; sustituye la envoltura; cdecl, 1 argumento, 338 bytes): lee una luz de un flujo. RwStreamFindChunk(flujo, 1 (struct), &longitud, &version); version fuera de
// 0x34000..0x36003 -> RWERROR 0x80000004 (plugin 2) y NULL. Lee `longitud` bytes en un RpLightChunkInfo de 24 bytes puesto a cero (el EXE no limita la longitud: con mas de 24 desborda la pila);
// si no se leen todos devuelve NULL. RpLightCreate(tipo = byte +0x16 de typeAndFlags; sin memoria NULL); radio, color, y el angulo: con version < 0x30300 (inalcanzable tras la comprobacion anterior)
// -sqrt(1 / (a * a + 1)) con RwSqrt (0x7EDB30) y x87 a 24 bits, si no el valor tal cual; indicador de blanco; flags = byte bajo de typeAndFlags. Por ultimo
// _rwPluginRegistryReadDataChunks(registro de luces, flujo, luz): resultado nulo -> NULL (la luz creada no se libera, como el EXE).
RpLight* RpLightStreamRead(RwStream* stream) {
    RwUInt32 length = 0;
    RwUInt32 version = 0;
    if (!RwStreamFindChunk(stream, rwID_STRUCT, &length, &version)) {
        return nullptr;
    }
    if (version < kStreamVersionMin || version > kStreamVersionMax) {
        RaiseRwError(kErrorBadVersion);
        return nullptr;
    }
    RpLightChunkInfo info = {};
    if (RwStreamRead(stream, &info, length) != length) {
        return nullptr;
    }
    RpLight* const light = RpLightCreate(static_cast<RwInt32>((info.typeAndFlags >> 16) & 0xFF));
    if (!light) {
        return nullptr;
    }
    light->radius      = info.radius;
    light->color.red   = info.red;
    light->color.green = info.green;
    light->color.blue  = info.blue;
    if (version < 0x30300) {
        const RwReal recip = 1.0f / (info.minusCosAngle * info.minusCosAngle + 1.0f);
        light->minusCosAngle = -RwSqrtFromExe(recip);
    } else {
        light->minusCosAngle = info.minusCosAngle;
    }
    LightUpdateWhiteFlag(light);
    light->object.object.flags = static_cast<RwUInt8>(info.typeAndFlags & 0xFF);
    if (!_rwPluginRegistryReadDataChunks(&s_lightTKList, stream, light)) {
        return nullptr;
    }
    return light;
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
        // x87 de 24 bits del EXE (denormales incluidos): ver AtomicInterpolateSphereCpp (X87Math.cpp).
        AtomicInterpolateSphereCpp(&atomic->boundingSphere.center.x, &start.center.x, &end.center.x, atomic->interpolator.position, atomic->interpolator.recipTime);
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
            // x87 de 24 bits del EXE: longitudes al cuadrado y maximo en AtomicMatrixMaxLengthSquaredCpp (X87Math.cpp); el producto final con el fmul del x87.
            const float maximum = AtomicMatrixMaxLengthSquaredCpp(&ltm->right.x);
            const float root    = _rwSqrt(maximum);
            const float scaled  = FloatTimesFloatCpp(root, atomic->boundingSphere.radius);
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
    // Como el EXE: el puntero a la tuberia se relee del atomo para cada escritura (0x749160 lee [objeto + 0x6C] dos veces).
    if (!RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&static_cast<const RpAtomic*>(object)->pipeline->pluginId), 4)) {
        return nullptr;
    }
    return RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&static_cast<const RpAtomic*>(object)->pipeline->pluginData), 4) ? stream : nullptr;
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
// (s_clumpTKList, 0x8D6264, ya esta definido arriba por el lote 226A: la segunda definicion de 227A hacia el simbolo ambiguo y no compilaba al unir ambos lotes; se usa la de 226A.)
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

// 0x74FDA0 (lote 230A, sin validar; cdecl sin argumentos; el listado decia Win): registra los ocho plugins de motor del modulo RpWorld con RwEngineRegisterPlugin (0x7F2BB0), en este orden y con las
// rutinas del EXE por direccion fija (tamano, id, constructor, destructor): (4, 0x501, 0x74D8E0, 0x74D950), (0x30, 0x502, 0x758970, 0x758940), (4, 0x503, 0x74BFA0, 0x74BFC0), (8, 0x504, 0x749760, 0x749480),
// (4, 0x505, 0x752270, 0x7521B0), (0, 0x50A, 0x761C30, 0x761C40), (0xC, 0x507, 0x74FEE0, 0x74FE90) y (0, 0x50B, 0x7637E0, 0x7637D0). Los resultados se acumulan con OR (el EXE no corta al primer fallo): si el
// OR es negativo devuelve 0; si no, 0x750050 (registro del plugin de sectores, id 0x509), ClumpPluginAttach (0x749830) y 0x74EA40, cada uno cortando con 0 si devuelve 0; el resultado final es (0x74EA40() != 0).
RwBool RpWorldPluginAttach() {
    const auto ctor = [](uintptr_t address) { return reinterpret_cast<RwPluginObjectConstructor>(address); };
    const auto dtor = [](uintptr_t address) { return reinterpret_cast<RwPluginObjectDestructor>(address); };
    RwInt32 status = RwEngineRegisterPlugin(4, 0x501, ctor(0x74D8E0), dtor(0x74D950));
    status |= RwEngineRegisterPlugin(0x30, 0x502, ctor(0x758970), dtor(0x758940));
    status |= RwEngineRegisterPlugin(4, 0x503, ctor(0x74BFA0), dtor(0x74BFC0));
    status |= RwEngineRegisterPlugin(8, 0x504, ctor(0x749760), dtor(0x749480));
    status |= RwEngineRegisterPlugin(4, 0x505, ctor(0x752270), dtor(0x7521B0));
    status |= RwEngineRegisterPlugin(0, 0x50A, ctor(0x761C30), dtor(0x761C40));
    status |= RwEngineRegisterPlugin(0xC, 0x507, ctor(0x74FEE0), dtor(0x74FE90));
    status |= RwEngineRegisterPlugin(0, 0x50B, ctor(0x7637E0), dtor(0x7637D0));
    if (status < 0) {
        return FALSE;
    }
    if (!_rpTieMeshPluginAttach()) {
        return FALSE;
    }
    if (!ClumpPluginAttach()) {
        return FALSE;
    }
    return reinterpret_cast<RwBool(__cdecl*)()>(0x74EA40)() != 0;
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

// 0x7588B0 (lote R18A, sin validar; sustituye la envoltura; cdecl, 2 argumentos, `ret`, 17 bytes): escribe el dword de flags en geometria + desplazamiento del plugin (0xC926E8), sin comprobar nada.
void RpD3D9GeometrySetUsageFlags(RpGeometry* geometry, RpD3D9GeometryUsageFlag flags) {
    *reinterpret_cast<RpD3D9GeometryUsageFlag*>(reinterpret_cast<uintptr_t>(geometry) + static_cast<uintptr_t>(s_geometryUsageFlagsOffset)) = flags;
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

void RxD3D9AllInOneSetLightingCallBack(RxPipelineNode* node, RxD3D9AllInOneLightingCallBack callback) {
    ((void(__cdecl *)(RxPipelineNode*, RxD3D9AllInOneLightingCallBack))0x7573C0)(node, callback);
}

RxD3D9AllInOneLightingCallBack RxD3D9AllInOneGetLightingCallBack(RxPipelineNode* node) {
    return ((RxD3D9AllInOneLightingCallBack(__cdecl *)(RxPipelineNode*))0x7573D0)(node);
}

// 0x7573E0 (lote R18A, sin validar; sustituye la envoltura; cdecl, 2 argumentos, `ret`, 15 bytes): privateData (+0x14 del nodo)->render (+0xC) = callback.
void RxD3D9AllInOneSetRenderCallBack(RxPipelineNode* node, RxD3D9AllInOneRenderCallBack callback) {
    *reinterpret_cast<RxD3D9AllInOneRenderCallBack*>(static_cast<uint8_t*>(node->privateData) + 0xC) = callback;
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

RwUInt32 _rpD3D9VertexDeclarationInstV3dComp(RwUInt32 type, RwUInt8* mem, const RpVertexNormal* src, RwInt32 numVerts, RwUInt32 stride) {
    return ((RwUInt32(__cdecl *)(RwUInt32, RwUInt8*, const RpVertexNormal*, RwInt32, RwUInt32))0x7531B0)(type, mem, src, numVerts, stride);
}

RwUInt32 _rpD3D9VertexDeclarationInstIndices(RwUInt32 type, RwUInt8* mem, const RwUInt32* indices, RwInt32 numVerts, RwUInt32 stride) {
    return ((RwUInt32(__cdecl *)(RwUInt32, RwUInt8*, const RwUInt32*, RwInt32, RwUInt32))0x754B40)(type, mem, indices, numVerts, stride);
}

namespace {
// fld dword + fistp dword con la palabra de control del proceso (redondeo al par mas cercano; fuera de rango o NaN = 0x80000000). Es el mismo helper que RwFastRealToUInt32.
inline RwInt32 WeightFistp(float value) {
    return static_cast<RwInt32>(RwPlCoreFastRealToUInt32Cpp(value));
}
inline void WeightStore16(RwUInt8* dst, RwUInt32 value) {
    const RwUInt16 v = static_cast<RwUInt16>(value);
    std::memcpy(dst, &v, sizeof(v));
}
inline void WeightStore32(RwUInt8* dst, RwUInt32 value) {
    std::memcpy(dst, &value, sizeof(value));
}
}

// 0x752320 (lote R16A, sin validar; sustituye la envoltura; cdecl, 5 argumentos (tipo, destino, origen RwV4d de 16 bytes, numero de vertices, paso), 1908 bytes; el listado la atribuia a RpWorld):
// instancia los cuatro pesos (x, y, z, w) de un vertice en el formato D3DDECLTYPE `type` (tabla de saltos 0x752A94, tipos 0..14). Devuelve SIEMPRE la tabla de tamanos 0x874EF8[type] (sin comprobar el
// rango, igual que _rpD3D9VertexDeclarationGetSize); con type > 14 o numVerts <= 0 (comparacion con signo) no escribe nada. Por cada vertice (origen += 16, destino += paso):
//  0 FLOAT1: x.  1 FLOAT2: x, y.  2 y 3: los CUATRO dwords (el EXE copia 16 bytes tambien en FLOAT3).
//  4 D3DCOLOR: si x >= 1.0 (la comparacion es "menor que" de la x87: con NaN tambien convierte) bytes {0, 0, 255, 0}; si no b[1] = fistp(y * 255), b[0] = fistp(z * 255), b[3] = fistp(w * 255),
//    b[2] = 255 - b[3] - b[1] - b[0] (modulo 256).
//  5 UBYTE4: cuatro bytes fistp(x), fistp(y), fistp(z), fistp(w) sin escalar.  6 SHORT2 y 7 SHORT4: palabras de 16 bits con fistp de x, y [, z, w].
//  8 UBYTE4N: si x >= 1.0 bytes {255, 0, 0, 0}; si no b[1] = fistp(y * 255), b[2] = fistp(z * 255), b[3] = fistp(w * 255), b[0] = 255 - b[2] - b[3] - b[1].
//  9 SHORT2N: v = fistp(x * 32767): {v, 0x7FFF - v}.  10 SHORT4N: v1..v3 = fistp(y, z, w * 32767), palabras {0x7FFF - v3 - v2 - v1, v1, v2, v3}.
//  11 USHORT2N y 12 USHORT4N: igual con 65535 y 0xFFFF (11 usa x; 12 usa y, z, w).
//  13 UDEC3: a = fistp(x), b = fistp(y), c = fistp(z) sin escalar: ((c & 0x3FF) << 20) | ((b & 0x3FF) << 10) | (a & 0x3FF).
//  14 DEC3N: si z < 0.0019569471 (0x3B004020, "menor que" ordenado) i = fistp(x * 511), valor = (0x7FC00 - ((i & 0x3FF) << 10)) | (i & 0x3FF); si no b = fistp(y * 511) & 0x3FF, c = fistp(z * 511) & 0x3FF,
//    valor = (((c << 10) | b) << 10) | (0x1FF - c - b) (el ultimo termino SIN enmascarar, puede ser negativo y ensucia los bits altos).
// Los productos se redondean a float antes de fistp (el EXE guarda con fstp y recarga). El EXE reutiliza los huecos de los argumentos como temporales; todo se lee a registros antes.
RwUInt32 _rpD3D9VertexDeclarationInstWeights(RwUInt32 type, RwUInt8* mem, const RwV4d* src, RwInt32 numVerts, RwUInt32 stride) {
    const auto* const sizes = reinterpret_cast<const RwUInt32*>(0x874EF8);
    if (type > 14 || numVerts <= 0) {
        return sizes[type];
    }
    const float* s = reinterpret_cast<const float*>(src);
    RwUInt8* d = mem;
    constexpr float kDec3SmallZ = std::bit_cast<float>(0x3B004020u);
    for (RwInt32 n = numVerts; n > 0; --n, s += 4, d += stride) {
        switch (type) {
        case 0:
            std::memcpy(d, s, 4);
            break;
        case 1:
            std::memcpy(d, s, 8);
            break;
        case 2:
        case 3:
            std::memcpy(d, s, 16);
            break;
        case 4:
            if (s[0] >= 1.0f) {
                d[0] = 0;
                d[1] = 0;
                d[2] = 0xFF;
                d[3] = 0;
            } else {
                d[1] = static_cast<RwUInt8>(WeightFistp(s[1] * 255.0f));
                d[0] = static_cast<RwUInt8>(WeightFistp(s[2] * 255.0f));
                const RwUInt8 w = static_cast<RwUInt8>(WeightFistp(s[3] * 255.0f));
                d[3] = w;
                d[2] = static_cast<RwUInt8>(0xFF - w - d[1] - d[0]);
            }
            break;
        case 5:
            d[0] = static_cast<RwUInt8>(WeightFistp(s[0]));
            d[1] = static_cast<RwUInt8>(WeightFistp(s[1]));
            d[2] = static_cast<RwUInt8>(WeightFistp(s[2]));
            d[3] = static_cast<RwUInt8>(WeightFistp(s[3]));
            break;
        case 6:
            WeightStore16(d + 0, WeightFistp(s[0]));
            WeightStore16(d + 2, WeightFistp(s[1]));
            break;
        case 7:
            WeightStore16(d + 0, WeightFistp(s[0]));
            WeightStore16(d + 2, WeightFistp(s[1]));
            WeightStore16(d + 4, WeightFistp(s[2]));
            WeightStore16(d + 6, WeightFistp(s[3]));
            break;
        case 8:
            if (s[0] >= 1.0f) {
                d[0] = 0xFF;
                d[1] = 0;
                d[2] = 0;
                d[3] = 0;
            } else {
                d[1] = static_cast<RwUInt8>(WeightFistp(s[1] * 255.0f));
                d[2] = static_cast<RwUInt8>(WeightFistp(s[2] * 255.0f));
                const RwUInt8 w = static_cast<RwUInt8>(WeightFistp(s[3] * 255.0f));
                d[3] = w;
                d[0] = static_cast<RwUInt8>(0xFF - d[2] - w - d[1]);
            }
            break;
        case 9: {
            const RwInt32 v = WeightFistp(s[0] * 32767.0f);
            WeightStore16(d + 0, v);
            WeightStore16(d + 2, 0x7FFF - v);
            break;
        }
        case 10: {
            const RwInt32 v1 = WeightFistp(s[1] * 32767.0f);
            const RwInt32 v2 = WeightFistp(s[2] * 32767.0f);
            const RwInt32 v3 = WeightFistp(s[3] * 32767.0f);
            WeightStore16(d + 0, 0x7FFF - v3 - v2 - v1);
            WeightStore16(d + 2, v1);
            WeightStore16(d + 4, v2);
            WeightStore16(d + 6, v3);
            break;
        }
        case 11: {
            const RwInt32 v = WeightFistp(s[0] * 65535.0f);
            WeightStore16(d + 0, v);
            WeightStore16(d + 2, 0xFFFF - v);
            break;
        }
        case 12: {
            const RwInt32 v1 = WeightFistp(s[1] * 65535.0f);
            const RwInt32 v2 = WeightFistp(s[2] * 65535.0f);
            const RwInt32 v3 = WeightFistp(s[3] * 65535.0f);
            WeightStore16(d + 0, 0xFFFF - v3 - v2 - v1);
            WeightStore16(d + 2, v1);
            WeightStore16(d + 4, v2);
            WeightStore16(d + 6, v3);
            break;
        }
        case 13: {
            const RwUInt32 a = static_cast<RwUInt32>(WeightFistp(s[0]));
            const RwUInt32 b = static_cast<RwUInt32>(WeightFistp(s[1]));
            const RwUInt32 c = static_cast<RwUInt32>(WeightFistp(s[2]));
            WeightStore32(d, ((((c & 0x3FF) << 10) | (b & 0x3FF)) << 10) | (a & 0x3FF));
            break;
        }
        case 14:
            if (s[2] < kDec3SmallZ) {
                const RwUInt32 i = static_cast<RwUInt32>(WeightFistp(s[0] * 511.0f)) & 0x3FF;
                WeightStore32(d, (0x7FC00u - (i << 10)) | i);
            } else {
                const RwUInt32 b = static_cast<RwUInt32>(WeightFistp(s[1] * 511.0f)) & 0x3FF;
                const RwUInt32 c = static_cast<RwUInt32>(WeightFistp(s[2] * 511.0f)) & 0x3FF;
                WeightStore32(d, ((((c << 10) | b) << 10)) | (0x1FFu - c - b));
            }
            break;
        }
    }
    return sizes[type];
}

// ---------------------------------------------------------------------------------------------------------------------
// Lote R17A (sin validar): instanciacion de vertices D3D9 (posicion, morph, texcoord, color, indices con remap, tangentes), lectura de
// texcoord, rangos y formato de texcoord. Todas las tablas de saltos (15 entradas, tipos D3DDECLTYPE 0..14) y constantes se leyeron del EXE.
// Las multiplicaciones y sumas son floats (la x87 del proceso trabaja a 24 bits); fistp = redondeo al par con la palabra de control del proceso
// (WeightFistp). Se compila con float_control(precise) para que /fp:fast no reasocie ni cambie 1/det * n por n / det.
// ---------------------------------------------------------------------------------------------------------------------
#pragma float_control(precise, on, push)
namespace {
inline const RwUInt32* DeclSizeTable() { return reinterpret_cast<const RwUInt32*>(0x874EF8); }
}

// 0x752AD0 (lote R17A, sin validar; sustituye la envoltura; cdecl, 5 argumentos (tipo D3DDECLTYPE, destino, origen RwV3d, numero de vertices, paso), 1700 bytes; el listado la atribuia a RpWorld):
// instancia posiciones (x, y, z) al formato `type` (tabla de saltos 0x753174, tipos 0..14; mayor -> no hace nada). Devuelve SIEMPRE 0x874EF8[type] (sin comprobar el rango). Con numVerts <= 0 (con signo) no escribe.
// Por vertice (origen += 12, destino += paso): 0 FLOAT1: x.  1 FLOAT2: x, y.  2 FLOAT3: x, y, z (con paso 12 el EXE copia numVerts * 12 bytes de golpe con rep movs).  3 FLOAT4: x, y, z, 1.0f.
// 4 D3DCOLOR: bytes {fistp(z * 255), fistp(y * 255), fistp(x * 255), 255}.  5 UBYTE4: {x, y, z, 1} con fistp sin escalar.  6 SHORT2: palabras {x, y}.  7 SHORT4: {x, y, z, 1}.
// 8 UBYTE4N: bytes {x * 255, y * 255, z * 255, 255}.  9 SHORT2N: {x * 32767, y * 32767}.  10 SHORT4N: {x, y, z (* 32767), 0x7FFF}.  11 USHORT2N: {x * 65535, y * 65535}.  12 USHORT4N: {x, y, z (* 65535), 0xFFFF}.
// 13 UDEC3: a = fistp(x), b = fistp(y), c = fistp(z): ((c & 0x3FF) << 20) | ((b & 0x3FF) << 10) | (a & 0x3FF).  14 DEC3N: igual con * 511.
// El producto se guarda como float antes del fistp (fstp + fld). Para type 2 con paso 12 y numVerts < 0 el EXE ejecutaria un rep movs gigante (se ignora aqui).
RwUInt32 _rpD3D9VertexDeclarationInstV3d(RwUInt32 type, RwUInt8* mem, const RwV3d* src, RwInt32 numVerts, RwUInt32 stride) {
    const RwUInt32* const sizes = DeclSizeTable();
    if (type > 14) {
        return sizes[type];
    }
    if (type == 2 && stride == 12) {
        if (numVerts > 0) {
            std::memcpy(mem, src, static_cast<size_t>(numVerts) * 12u);
        }
        return sizes[type];
    }
    if (numVerts <= 0) {
        return sizes[type];
    }
    const float* s = reinterpret_cast<const float*>(src);
    RwUInt8* d = mem;
    for (RwInt32 n = numVerts; n > 0; --n, s += 3, d += stride) {
        switch (type) {
        case 0:
            std::memcpy(d, s, 4);
            break;
        case 1:
            std::memcpy(d, s, 8);
            break;
        case 2:
            std::memcpy(d, s, 12);
            break;
        case 3:
            std::memcpy(d, s, 12);
            WeightStore32(d + 12, 0x3F800000u);
            break;
        case 4:
            d[2] = static_cast<RwUInt8>(WeightFistp(s[0] * 255.0f));
            d[1] = static_cast<RwUInt8>(WeightFistp(s[1] * 255.0f));
            d[0] = static_cast<RwUInt8>(WeightFistp(s[2] * 255.0f));
            d[3] = 0xFF;
            break;
        case 5:
            d[0] = static_cast<RwUInt8>(WeightFistp(s[0]));
            d[1] = static_cast<RwUInt8>(WeightFistp(s[1]));
            d[2] = static_cast<RwUInt8>(WeightFistp(s[2]));
            d[3] = 1;
            break;
        case 6:
            WeightStore16(d + 0, WeightFistp(s[0]));
            WeightStore16(d + 2, WeightFistp(s[1]));
            break;
        case 7:
            WeightStore16(d + 0, WeightFistp(s[0]));
            WeightStore16(d + 2, WeightFistp(s[1]));
            WeightStore16(d + 4, WeightFistp(s[2]));
            WeightStore16(d + 6, 1);
            break;
        case 8:
            d[0] = static_cast<RwUInt8>(WeightFistp(s[0] * 255.0f));
            d[1] = static_cast<RwUInt8>(WeightFistp(s[1] * 255.0f));
            d[2] = static_cast<RwUInt8>(WeightFistp(s[2] * 255.0f));
            d[3] = 0xFF;
            break;
        case 9:
            WeightStore16(d + 0, WeightFistp(s[0] * 32767.0f));
            WeightStore16(d + 2, WeightFistp(s[1] * 32767.0f));
            break;
        case 10:
            WeightStore16(d + 0, WeightFistp(s[0] * 32767.0f));
            WeightStore16(d + 2, WeightFistp(s[1] * 32767.0f));
            WeightStore16(d + 4, WeightFistp(s[2] * 32767.0f));
            WeightStore16(d + 6, 0x7FFF);
            break;
        case 11:
            WeightStore16(d + 0, WeightFistp(s[0] * 65535.0f));
            WeightStore16(d + 2, WeightFistp(s[1] * 65535.0f));
            break;
        case 12:
            WeightStore16(d + 0, WeightFistp(s[0] * 65535.0f));
            WeightStore16(d + 2, WeightFistp(s[1] * 65535.0f));
            WeightStore16(d + 4, WeightFistp(s[2] * 65535.0f));
            WeightStore16(d + 6, 0xFFFF);
            break;
        case 13: {
            const RwUInt32 a = static_cast<RwUInt32>(WeightFistp(s[0]));
            const RwUInt32 b = static_cast<RwUInt32>(WeightFistp(s[1]));
            const RwUInt32 c = static_cast<RwUInt32>(WeightFistp(s[2]));
            WeightStore32(d, ((((c & 0x3FF) << 10) | (b & 0x3FF)) << 10) | (a & 0x3FF));
            break;
        }
        case 14: {
            const RwUInt32 a = static_cast<RwUInt32>(WeightFistp(s[0] * 511.0f));
            const RwUInt32 b = static_cast<RwUInt32>(WeightFistp(s[1] * 511.0f));
            const RwUInt32 c = static_cast<RwUInt32>(WeightFistp(s[2] * 511.0f));
            WeightStore32(d, ((((c & 0x3FF) << 10) | (b & 0x3FF)) << 10) | (a & 0x3FF));
            break;
        }
        }
    }
    return sizes[type];
}

// 0x753B60 (lote R17A, sin validar; sustituye la envoltura; cdecl, 7 argumentos (tipo, destino, origen A, origen B, escala, numero de vertices, paso), 2370 bytes): igual que 0x752AD0 pero cada componente es la
// mezcla v = (B - A) * escala + A (cuatro operaciones float, la x87 redondea a 24 bits tras cada una) de dos RwV3d. Tipos 0..14 (tabla 0x7544A4; mayor o numVerts <= 0 -> nada; devuelve 0x874EF8[type]).
//  0..3: float x [, y [, z [, 1.0f]]] (3 FLOAT3 no, 2 = x, y, z; 3 = x, y, z, 1.0f).
//  4, 8, 9, 10, 11, 12, 14 (con escala): p = (1.0 - escala) * K y q = escala * K (K = 255, 255, 32767, 32767, 65535, 65535, 511; ambos redondeados a float al principio); cada componente
//  c = p * A.c + q * B.c (productos float y suma float) y despues fistp. 4 D3DCOLOR: bytes {c(z), c(y), c(x), 255}. 8 UBYTE4N: {x, y, z, 255}. 9 SHORT2N: {x, y}. 10 SHORT4N: {x, y, z, 0x7FFF}.
//  11 USHORT2N: {x, y}. 12 USHORT4N: {x, y, z, 0xFFFF}. 14 DEC3N: ((z & 0x3FF) << 20) | ((y & 0x3FF) << 10) | (x & 0x3FF).
//  5 UBYTE4 {x, y, z, 1}, 6 SHORT2 {x, y}, 7 SHORT4 {x, y, z, 1}, 13 UDEC3: la mezcla v = (B - A) * escala + A sin escalar, fistp y empaquetado como 0x752AD0.
RwUInt32 _rpD3D9VertexDeclarationInstV3dMorph(RwUInt32 type, RwUInt8* mem, const RwV3d* src1, const RwV3d* src2, RwReal scale, RwInt32 numVerts, RwUInt32 stride) {
    const RwUInt32* const sizes = DeclSizeTable();
    if (type > 14 || numVerts <= 0) {
        return sizes[type];
    }
    const float* a = reinterpret_cast<const float*>(src1);
    const float* b = reinterpret_cast<const float*>(src2);
    RwUInt8* d = mem;
    // Constantes de la mezcla ponderada (tipos 4, 8..12 y 14): p = (1 - escala) * K, q = escala * K.
    float p = 0.0f;
    float q = 0.0f;
    {
        float k = 0.0f;
        switch (type) {
        case 4:
        case 8:
            k = 255.0f;
            break;
        case 9:
        case 10:
            k = 32767.0f;
            break;
        case 11:
        case 12:
            k = 65535.0f;
            break;
        case 14:
            k = 511.0f;
            break;
        default:
            break;
        }
        if (k != 0.0f) {
            const float oneMinus = 1.0f - scale;
            p = oneMinus * k;
            q = scale * k;
        }
    }
    for (RwInt32 n = numVerts; n > 0; --n, a += 3, b += 3, d += stride) {
        // v = (B - A) * escala + A, componente por componente (productos float).
        auto mix = [&](int c) -> float {
            const float diff = b[c] - a[c];
            const float scaled = diff * scale;
            return scaled + a[c];
        };
        // c = p * A + q * B.
        auto weighted = [&](int c) -> float {
            const float pa = p * a[c];
            const float qb = q * b[c];
            return pa + qb;
        };
        switch (type) {
        case 0:
            WeightStore32(d, std::bit_cast<RwUInt32>(mix(0)));
            break;
        case 1:
            WeightStore32(d + 0, std::bit_cast<RwUInt32>(mix(0)));
            WeightStore32(d + 4, std::bit_cast<RwUInt32>(mix(1)));
            break;
        case 2:
            WeightStore32(d + 0, std::bit_cast<RwUInt32>(mix(0)));
            WeightStore32(d + 4, std::bit_cast<RwUInt32>(mix(1)));
            WeightStore32(d + 8, std::bit_cast<RwUInt32>(mix(2)));
            break;
        case 3:
            WeightStore32(d + 0, std::bit_cast<RwUInt32>(mix(0)));
            WeightStore32(d + 4, std::bit_cast<RwUInt32>(mix(1)));
            WeightStore32(d + 12, 0x3F800000u);
            WeightStore32(d + 8, std::bit_cast<RwUInt32>(mix(2)));
            break;
        case 4:
            d[2] = static_cast<RwUInt8>(WeightFistp(weighted(0)));
            d[1] = static_cast<RwUInt8>(WeightFistp(weighted(1)));
            d[0] = static_cast<RwUInt8>(WeightFistp(weighted(2)));
            d[3] = 0xFF;
            break;
        case 5:
            d[0] = static_cast<RwUInt8>(WeightFistp(mix(0)));
            d[1] = static_cast<RwUInt8>(WeightFistp(mix(1)));
            d[3] = 1;
            d[2] = static_cast<RwUInt8>(WeightFistp(mix(2)));
            break;
        case 6:
            WeightStore16(d + 0, WeightFistp(mix(0)));
            WeightStore16(d + 2, WeightFistp(mix(1)));
            break;
        case 7:
            WeightStore16(d + 0, WeightFistp(mix(0)));
            WeightStore16(d + 2, WeightFistp(mix(1)));
            WeightStore16(d + 6, 1);
            WeightStore16(d + 4, WeightFistp(mix(2)));
            break;
        case 8:
            d[0] = static_cast<RwUInt8>(WeightFistp(weighted(0)));
            d[1] = static_cast<RwUInt8>(WeightFistp(weighted(1)));
            d[3] = 0xFF;
            d[2] = static_cast<RwUInt8>(WeightFistp(weighted(2)));
            break;
        case 9:
            WeightStore16(d + 0, WeightFistp(weighted(0)));
            WeightStore16(d + 2, WeightFistp(weighted(1)));
            break;
        case 10:
            WeightStore16(d + 0, WeightFistp(weighted(0)));
            WeightStore16(d + 2, WeightFistp(weighted(1)));
            WeightStore16(d + 6, 0x7FFF);
            WeightStore16(d + 4, WeightFistp(weighted(2)));
            break;
        case 11:
            WeightStore16(d + 0, WeightFistp(weighted(0)));
            WeightStore16(d + 2, WeightFistp(weighted(1)));
            break;
        case 12:
            WeightStore16(d + 0, WeightFistp(weighted(0)));
            WeightStore16(d + 2, WeightFistp(weighted(1)));
            WeightStore16(d + 6, 0xFFFF);
            WeightStore16(d + 4, WeightFistp(weighted(2)));
            break;
        case 13: {
            const RwUInt32 x = static_cast<RwUInt32>(WeightFistp(mix(0)));
            const RwUInt32 y = static_cast<RwUInt32>(WeightFistp(mix(1)));
            const RwUInt32 z = static_cast<RwUInt32>(WeightFistp(mix(2)));
            WeightStore32(d, ((((z & 0x3FF) << 10) | (y & 0x3FF)) << 10) | (x & 0x3FF));
            break;
        }
        case 14: {
            const RwUInt32 x = static_cast<RwUInt32>(WeightFistp(weighted(0)));
            const RwUInt32 y = static_cast<RwUInt32>(WeightFistp(weighted(1)));
            const RwUInt32 z = static_cast<RwUInt32>(WeightFistp(weighted(2)));
            WeightStore32(d, ((((z & 0x3FF) << 10) | (y & 0x3FF)) << 10) | (x & 0x3FF));
            break;
        }
        }
    }
    return sizes[type];
}

// 0x7544E0 (lote R17A, sin validar; sustituye la envoltura; cdecl, 5 argumentos (tipo, destino, origen RwV2d, numero de vertices, paso), 1467 bytes): coordenadas de textura (u, v) al formato `type` (tabla 0x754A9C, tipos 0..14).
// Devuelve 0x874EF8[type]; type > 14 o numVerts <= 0 no escriben. 0 FLOAT1: u.  1 FLOAT2: u, v (con paso 8 el EXE copia numVerts * 8 bytes de golpe).  2 FLOAT3: u, v, 0.  3 FLOAT4: u, v, 0, 1.0f.
// 4 D3DCOLOR: bytes {0, fistp(v * 255), fistp(u * 255), 255}.  5 UBYTE4: {u, v, 0, 1} sin escalar.  6 SHORT2: {u, v}.  7 SHORT4: {u, v, 0, 1}.  8 UBYTE4N: {u * 255, v * 255, 0, 255}.
// 9 SHORT2N: {u, v} * 32767.  10 SHORT4N: {u * 32767, v * 32767, 0, 0x7FFF}.  11 USHORT2N: * 65535.  12 USHORT4N: {.., .., 0, 0xFFFF}.  13 UDEC3: ((v & 0x3FF) << 10) | (u & 0x3FF) con fistp sin escalar.  14 DEC3N: igual con * 511.
RwUInt32 _rpD3D9VertexDeclarationInstV2d(RwUInt32 type, RwUInt8* mem, const RwV2d* src, RwInt32 numVerts, RwUInt32 stride) {
    const RwUInt32* const sizes = DeclSizeTable();
    if (type > 14) {
        return sizes[type];
    }
    if (type == 1 && stride == 8) {
        if (numVerts > 0) {
            std::memcpy(mem, src, static_cast<size_t>(numVerts) * 8u);
        }
        return sizes[type];
    }
    if (numVerts <= 0) {
        return sizes[type];
    }
    const float* s = reinterpret_cast<const float*>(src);
    RwUInt8* d = mem;
    for (RwInt32 n = numVerts; n > 0; --n, s += 2, d += stride) {
        switch (type) {
        case 0:
            std::memcpy(d, s, 4);
            break;
        case 1:
            std::memcpy(d, s, 8);
            break;
        case 2:
            std::memcpy(d, s, 8);
            WeightStore32(d + 8, 0);
            break;
        case 3:
            std::memcpy(d, s, 8);
            WeightStore32(d + 8, 0);
            WeightStore32(d + 12, 0x3F800000u);
            break;
        case 4:
            d[2] = static_cast<RwUInt8>(WeightFistp(s[0] * 255.0f));
            d[0] = 0;
            d[1] = static_cast<RwUInt8>(WeightFistp(s[1] * 255.0f));
            d[3] = 0xFF;
            break;
        case 5:
            d[0] = static_cast<RwUInt8>(WeightFistp(s[0]));
            d[2] = 0;
            d[1] = static_cast<RwUInt8>(WeightFistp(s[1]));
            d[3] = 1;
            break;
        case 6:
            WeightStore16(d + 0, WeightFistp(s[0]));
            WeightStore16(d + 2, WeightFistp(s[1]));
            break;
        case 7:
            WeightStore16(d + 0, WeightFistp(s[0]));
            WeightStore16(d + 4, 0);
            WeightStore16(d + 2, WeightFistp(s[1]));
            WeightStore16(d + 6, 1);
            break;
        case 8:
            d[0] = static_cast<RwUInt8>(WeightFistp(s[0] * 255.0f));
            d[2] = 0;
            d[1] = static_cast<RwUInt8>(WeightFistp(s[1] * 255.0f));
            d[3] = 0xFF;
            break;
        case 9:
            WeightStore16(d + 0, WeightFistp(s[0] * 32767.0f));
            WeightStore16(d + 2, WeightFistp(s[1] * 32767.0f));
            break;
        case 10:
            WeightStore16(d + 0, WeightFistp(s[0] * 32767.0f));
            WeightStore16(d + 4, 0);
            WeightStore16(d + 2, WeightFistp(s[1] * 32767.0f));
            WeightStore16(d + 6, 0x7FFF);
            break;
        case 11:
            WeightStore16(d + 0, WeightFistp(s[0] * 65535.0f));
            WeightStore16(d + 2, WeightFistp(s[1] * 65535.0f));
            break;
        case 12:
            WeightStore16(d + 0, WeightFistp(s[0] * 65535.0f));
            WeightStore16(d + 4, 0);
            WeightStore16(d + 2, WeightFistp(s[1] * 65535.0f));
            WeightStore16(d + 6, 0xFFFF);
            break;
        case 13: {
            const RwUInt32 u = static_cast<RwUInt32>(WeightFistp(s[0]));
            const RwUInt32 v = static_cast<RwUInt32>(WeightFistp(s[1]));
            WeightStore32(d, ((v & 0x3FF) << 10) | (u & 0x3FF));
            break;
        }
        case 14: {
            const RwUInt32 u = static_cast<RwUInt32>(WeightFistp(s[0] * 511.0f));
            const RwUInt32 v = static_cast<RwUInt32>(WeightFistp(s[1] * 511.0f));
            WeightStore32(d, ((v & 0x3FF) << 10) | (u & 0x3FF));
            break;
        }
        }
    }
    return sizes[type];
}

// 0x754AE0 (lote R17A, sin validar; sustituye la envoltura; cdecl, 4 argumentos (destino, colores RwRGBA, numero de vertices, paso), `ret`, 85 bytes): convierte RwRGBA (r, g, b, a) a D3DCOLOR (a << 24 | r << 16 | g << 8 | b) por vertice
// (origen += 4, destino += paso). Devuelve TRUE si algun alfa es distinto de 255 (el AND de todos los alfas con 0xFF inicial no es 0xFF); con numVerts <= 0 devuelve FALSE y no escribe.
RwBool _rpD3D9VertexDeclarationInstColor(RwUInt8* mem, const RwRGBA* color, RwInt32 numVerts, RwUInt32 stride) {
    RwUInt8 alphaAnd = 0xFF;
    const RwUInt8* c = reinterpret_cast<const RwUInt8*>(color);
    RwUInt8* d = mem;
    for (RwInt32 n = numVerts; n > 0; --n, c += 4, d += stride) {
        const RwUInt32 value = (static_cast<RwUInt32>(c[3]) << 24) | (static_cast<RwUInt32>(c[0]) << 16) | (static_cast<RwUInt32>(c[1]) << 8) | static_cast<RwUInt32>(c[2]);
        WeightStore32(d, value);
        alphaAnd &= c[3];
    }
    return alphaAnd != 0xFF ? TRUE : FALSE;
}

// 0x754C80 (lote R17A, sin validar; sustituye la envoltura; cdecl, 6 argumentos (tipo, destino, indices de 32 bits, tabla de remapeo de bytes, numero de vertices, paso), `ret`, 386 bytes): escribe por vertice los cuatro bytes del
// indice i (little endian b0..b3) pasados por remap[]. Solo tipos 4..7 (tabla 0x754E04); cualquier otro (o numVerts <= 0, con signo) no escribe. Devuelve 0x874EF8[type] siempre.
// 4 D3DCOLOR: bytes {r[b2], r[b1], r[b0], r[b3]}.  5 UBYTE4: {r[b0], r[b1], r[b2], r[b3]}.  6 SHORT2: palabras {r[b0], r[b1]} (extendidas con ceros).  7 SHORT4: palabras {r[b0], r[b1], r[b2], r[b3]}.
RwUInt32 _rpD3D9VertexDeclarationInstIndicesRemap(RwUInt32 type, RwUInt8* mem, const RwUInt32* indices, const RwUInt8* remap, RwInt32 numVerts, RwUInt32 stride) {
    const RwUInt32* const sizes = DeclSizeTable();
    if (type - 4u > 3u || numVerts <= 0) {
        return sizes[type];
    }
    const RwUInt8* ib = reinterpret_cast<const RwUInt8*>(indices);
    RwUInt8* d = mem;
    for (RwInt32 n = numVerts; n > 0; --n, ib += 4, d += stride) {
        switch (type) {
        case 4:
            d[0] = remap[ib[2]];
            d[1] = remap[ib[1]];
            d[2] = remap[ib[0]];
            d[3] = remap[ib[3]];
            break;
        case 5:
            d[0] = remap[ib[0]];
            d[1] = remap[ib[1]];
            d[2] = remap[ib[2]];
            d[3] = remap[ib[3]];
            break;
        case 6:
            WeightStore16(d + 0, remap[ib[0]]);
            WeightStore16(d + 2, remap[ib[1]]);
            break;
        case 7:
            WeightStore16(d + 0, remap[ib[0]]);
            WeightStore16(d + 2, remap[ib[1]]);
            WeightStore16(d + 4, remap[ib[2]]);
            WeightStore16(d + 6, remap[ib[3]]);
            break;
        }
    }
    return sizes[type];
}

// 0x754E20 (lote R17A, sin validar; sustituye la envoltura; cdecl, 6 argumentos (tipo, destino, posiciones RwV3d, texcoords, cabecera de recursos D3D9, paso), `ret`, 973 bytes): calcula tangentes por vertice y las instancia con _rpD3D9VertexDeclarationInstV3d.
// 1) numVerts = cabecera->totalNumVertex (+0x3C); bufer de numVerts * 12 bytes con RwMalloc (pista 0x1050D, el EXE llama a [RwEngineInstance + 0x134]) puesto a cero. 2) Bloquea el bufer de indices (IDirect3DIndexBuffer9::Lock(0,
// totalNumIndex * 2, &ptr, 0)). 3) Por cada malla (numMeshes, instancias de 0x24 bytes desde cabecera + 0x40): indices = ptr + startIndex (palabras de 16 bits); cuenta = numIndex (primType 5 = tira: numIndex - 2); minVert se suma a cada indice.
// Lista de triangulos (primType 4): k += 3, (a, b, c) = (i[k], i[k+1], i[k+2]) y NO se descartan degenerados. Tira: k += 1, k par (a, b, c) = (i[k], i[k+1], i[k+2]), k impar (a, b, c) = (i[k+2], i[k+1], i[k]); se descarta el triangulo si a == b, a == c o b == c.
// 4) Por triangulo: e1 = P[b] - P[a], e2 = P[c] - P[a]; du1 = uv[b].u - uv[a].u, dv1, du2 = uv[c].u - uv[a].u, dv2; det = du1 * dv2 - du2 * dv1; si |det| > 9.99999996e-13 (0x874F44; comparacion ordenada, con NaN no entra): r = (1.0 / det) * (e2.c * dv1 - dv2 * e1.c)
// para cada componente c = x, y, z (cada operacion float; 1.0 / det redondeado a float) y tan[a].c -= r, tan[b].c -= r, tan[c].c -= r (en ese orden). 5) Desbloquea los indices. 6) Si numVerts > 0, normaliza cada tangente: s = (y * y + x * x) + z * z,
// i = _rwInvSqrt(s) (0x7EDB90), componentes *= i. 7) r = _rpD3D9VertexDeclarationInstV3d(tipo, destino, tangentes, numVerts, paso), RwFree(tangentes) (hueco +0x138) y devuelve r.
RwUInt32 _rpD3D9VertexDeclarationInstTangent(RwUInt32 type, RwUInt8* mem, const RwV3d* pos, const RwTexCoords* texCoord, RxD3D9ResEntryHeader* meshHeader, RwUInt32 stride) {
    static_assert(sizeof(RxD3D9InstanceData) == 0x24, "instancia D3D9");
    const RwInt32 numVerts = static_cast<RwInt32>(meshHeader->totalNumVertex);
    const RwUInt32 tanBytes = static_cast<RwUInt32>(numVerts) * 12u;
    float* const tangents = static_cast<float*>(RwMalloc(tanBytes, 0x1050D));
    std::memset(tangents, 0, tanBytes);

    RwUInt16* lockedIndices = nullptr;
    meshHeader->indexBuffer->Lock(0, meshHeader->totalNumIndex * 2, reinterpret_cast<void**>(&lockedIndices), 0);

    const float* const P = reinterpret_cast<const float*>(pos);
    const float* const UV = reinterpret_cast<const float*>(texCoord);
    const RxD3D9InstanceData* inst = reinterpret_cast<const RxD3D9InstanceData*>(reinterpret_cast<const RwUInt8*>(meshHeader) + 0x40);
    const bool isList = (static_cast<RwUInt32>(meshHeader->primType) == 4);
    const bool isStrip = (static_cast<RwUInt32>(meshHeader->primType) == 5);

    for (RwUInt32 mesh = meshHeader->numMeshes; mesh != 0; --mesh, ++inst) {
        const RwUInt16* const idx = lockedIndices + inst->startIndex;
        RwInt32 count = static_cast<RwInt32>(inst->numIndex);
        if (isStrip) {
            count -= 2;
        }
        const RwUInt32 minVert = inst->minVert;
        if (count <= 0) {
            continue;
        }
        RwInt32 k = 0;
        do {
            RwUInt32 a, b, c;
            if (isList) {
                a = static_cast<RwUInt32>(idx[k]) + minVert;
                b = static_cast<RwUInt32>(idx[k + 1]) + minVert;
                c = static_cast<RwUInt32>(idx[k + 2]) + minVert;
                k += 3;
            } else {
                if (k & 1) {
                    b = static_cast<RwUInt32>(idx[k + 1]) + minVert;
                    a = static_cast<RwUInt32>(idx[k + 2]) + minVert;
                    c = static_cast<RwUInt32>(idx[k]) + minVert;
                } else {
                    a = static_cast<RwUInt32>(idx[k]) + minVert;
                    b = static_cast<RwUInt32>(idx[k + 1]) + minVert;
                    c = static_cast<RwUInt32>(idx[k + 2]) + minVert;
                }
                k += 1;
                if (a == b || a == c || b == c) {
                    continue;
                }
            }
            const float* const pa = P + a * 3;
            const float* const pb = P + b * 3;
            const float* const pc = P + c * 3;
            const float* const ta = UV + a * 2;
            const float* const tb = UV + b * 2;
            const float* const tc = UV + c * 2;
            float* const na = tangents + a * 3;
            float* const nb = tangents + b * 3;
            float* const nc = tangents + c * 3;

            const float e1x = pb[0] - pa[0];
            const float du1 = tb[0] - ta[0];
            const float dv1 = tb[1] - ta[1];
            const float e2x = pc[0] - pa[0];
            const float du2 = tc[0] - ta[0];
            const float dv2 = tc[1] - ta[1];
            const float d1 = du1 * dv2;
            const float d2 = du2 * dv1;
            const float det = d1 - d2;
            const float absDet = std::fabs(det);
            constexpr float kEpsilon = std::bit_cast<float>(0x2B8CBCCCu);
            // x
            {
                const float t1 = e2x * dv1;
                const float t2 = dv2 * e1x;
                const float num = t1 - t2;
                if (absDet > kEpsilon) {
                    const float inv = 1.0f / det;
                    const float r = inv * num;
                    na[0] = na[0] - r;
                    nb[0] = nb[0] - r;
                    nc[0] = nc[0] - r;
                }
            }
            // y
            {
                const float e1y = pb[1] - pa[1];
                const float e2y = pc[1] - pa[1];
                const float t1 = e2y * dv1;
                const float t2 = dv2 * e1y;
                const float num = t1 - t2;
                if (absDet > kEpsilon) {
                    const float inv = 1.0f / det;
                    const float r = num * inv;
                    na[1] = na[1] - r;
                    nb[1] = nb[1] - r;
                    nc[1] = nc[1] - r;
                }
            }
            // z
            {
                const float e1z = pb[2] - pa[2];
                const float e2z = pc[2] - pa[2];
                const float t1 = e2z * dv1;
                const float t2 = dv2 * e1z;
                const float num = t1 - t2;
                if (absDet > kEpsilon) {
                    const float inv = 1.0f / det;
                    const float r = num * inv;
                    na[2] = na[2] - r;
                    nb[2] = nb[2] - r;
                    nc[2] = nc[2] - r;
                }
            }
        } while (k < count);
    }

    meshHeader->indexBuffer->Unlock();

    if (numVerts > 0) {
        float* t = tangents;
        for (RwInt32 n = numVerts; n > 0; --n, t += 3) {
            const float x = t[0];
            const float y = t[1];
            const float z = t[2];
            const float yy = y * y;
            const float xx = x * x;
            const float sum1 = yy + xx;
            const float zz = z * z;
            const float sum = sum1 + zz;
            const RwReal inv = _rwInvSqrt(sum);
            t[0] = inv * x;
            t[1] = inv * y;
            t[2] = inv * z;
        }
    }

    const RwUInt32 result = _rpD3D9VertexDeclarationInstV3d(type, mem, reinterpret_cast<const RwV3d*>(tangents), numVerts, stride);
    RwFree(tangents);
    return result;
}

// 0x7555E0 (lote R17A, sin validar; sustituye la envoltura; cdecl, 3 argumentos (tipo de elemento, destino RwV2d, origen), `ret`, 531 bytes; la usa _rpMatFXD3D9BumpMapInstance): lee un elemento de un vertice D3D9 como RwV2d (u, v).
// Tabla de saltos 0x7557F4 (tipos 0..14; mayor -> (0, 0)). 0 FLOAT1: (dword, 0).  1, 2 y 3: dos dwords tal cual.  4 D3DCOLOR: (byte2, byte1) * (1/255) [0x859A3C = 0x3B808081].  5 UBYTE4: (byte0, byte1) sin escala.
// 6 y 7 (SHORT2/SHORT4): (short0, short1) con signo, sin escala.  8 UBYTE4N: (byte0, byte1) * (1/255).  9 y 10 (SHORT2N/SHORT4N): (short0, short1) con signo * (1/32767) [0x858C7C = 0x38000100].
// 11 y 12 (USHORT2N/USHORT4N): igual con signo (movsx, tambien para los "sin signo") * (1/65535) [0x874F48 = 0x37800080].  13 UDEC3: (v & 0x3FF, (v >> 10) & 0x3FF) sin escala (fild de 64 bits).
// 14 DEC3N: campos de 10 bits con signo (bit 0x200 extiende el signo con 0xFFFFFC00) * (1/511) [0x874F40 = 0x3B004020]. Cada producto es fild + fmul + fstp (un redondeo a float).
void _rpD3D9VertexDeclarationUnInstV2d(RwUInt32 type, RwV2d* dst, const RwUInt8* src) {
    constexpr float k255   = std::bit_cast<float>(0x3B808081u);
    constexpr float k32767 = std::bit_cast<float>(0x38000100u);
    constexpr float k65535 = std::bit_cast<float>(0x37800080u);
    constexpr float k511   = std::bit_cast<float>(0x3B004020u);
    auto readS16 = [&](int off) -> RwInt32 {
        RwInt16 v;
        std::memcpy(&v, src + off, sizeof(v));
        return static_cast<RwInt32>(v);
    };
    switch (type) {
    case 0: {
        RwUInt32 v;
        std::memcpy(&v, src, 4);
        std::memcpy(&dst->x, &v, 4);
        dst->y = 0.0f;
        return;
    }
    case 1:
    case 2:
    case 3: {
        RwUInt32 v0, v1;
        std::memcpy(&v0, src, 4);
        std::memcpy(&v1, src + 4, 4);
        std::memcpy(&dst->x, &v0, 4);
        std::memcpy(&dst->y, &v1, 4);
        return;
    }
    case 4:
        dst->x = static_cast<float>(static_cast<RwInt32>(src[2])) * k255;
        dst->y = static_cast<float>(static_cast<RwInt32>(src[1])) * k255;
        return;
    case 5:
        dst->x = static_cast<float>(static_cast<RwInt32>(src[0]));
        dst->y = static_cast<float>(static_cast<RwInt32>(src[1]));
        return;
    case 6:
    case 7:
        dst->x = static_cast<float>(readS16(0));
        dst->y = static_cast<float>(readS16(2));
        return;
    case 8:
        dst->x = static_cast<float>(static_cast<RwInt32>(src[0])) * k255;
        dst->y = static_cast<float>(static_cast<RwInt32>(src[1])) * k255;
        return;
    case 9:
    case 10:
        dst->x = static_cast<float>(readS16(0)) * k32767;
        dst->y = static_cast<float>(readS16(2)) * k32767;
        return;
    case 11:
    case 12:
        dst->x = static_cast<float>(readS16(0)) * k65535;
        dst->y = static_cast<float>(readS16(2)) * k65535;
        return;
    case 13: {
        RwUInt32 v;
        std::memcpy(&v, src, 4);
        dst->x = static_cast<float>(static_cast<long long>(v & 0x3FFu));
        dst->y = static_cast<float>(static_cast<long long>((v >> 10) & 0x3FFu));
        return;
    }
    case 14: {
        RwInt32 v;
        std::memcpy(&v, src, 4);
        RwInt32 lo = v & 0x3FF;
        if (lo & 0x200) {
            lo |= static_cast<RwInt32>(0xFFFFFC00u);
        }
        RwInt32 hi = (v >> 10) & 0x3FF;
        if (hi & 0x200) {
            hi |= static_cast<RwInt32>(0xFFFFFC00u);
        }
        dst->x = static_cast<float>(lo) * k511;
        dst->y = static_cast<float>(hi) * k511;
        return;
    }
    default:
        dst->x = 0.0f;
        dst->y = 0.0f;
        return;
    }
}

// 0x7558F0 (lote R17A, sin validar; sustituye la envoltura; cdecl, 4 argumentos (origen RwV2d, numero de elementos, minimo, maximo), `ret`, 131 bytes): min = max = src[0]; si numElements > 1 recorre numElements - 1 elementos EMPEZANDO por src[0] (el
// puntero avanza al final de cada vuelta, asi que el ultimo elemento nunca se examina: es el comportamiento del EXE). Por componente: si src < min (ordenado) min = src; si no, si src > max (ordenado, fcomp + test ah,5) max = src. Con NaN no cambia nada.
void _rpD3D9GetMinMaxValuesV2d(const RwV2d* src, RwInt32 numElements, RwV2d* min, RwV2d* max) {
    min->x = src->x;
    min->y = src->y;
    max->x = src->x;
    max->y = src->y;
    if (numElements > 1) {
        const RwV2d* p = src;
        for (RwInt32 n = numElements - 1; n != 0; --n, ++p) {
            if (min->x > p->x) {      // min.x > p.x ordenado (con NaN el EXE salta a la rama del maximo; ahi tampoco cambia nada)
                min->x = p->x;
            } else if (max->x < p->x) {
                max->x = p->x;
            }
            if (min->y > p->y) {
                min->y = p->y;
            } else if (max->y < p->y) {
                max->y = p->y;
            }
        }
    }
}

// 0x755AA0 (lote R17A, sin validar; sustituye la envoltura; cdecl, 2 argumentos (origen RwV2d, numero de elementos), `ret`, 227 bytes): elige el formato D3DDECLTYPE de las coordenadas de textura. Calcula minimo y maximo con 0x7558F0 y mira las capacidades
// del dispositivo (DWORD en 0xC9BFEC = D3DCAPS9::DeclTypes: 0x4 SHORT2N, 0x10 USHORT2N, 0x80 DEC3N). Si max.x <= 1.0 y max.y <= 1.0 (ordenado; si no, FLOAT2 = 1): con min.x >= 0 y min.y >= 0 -> USHORT2N (11) si el bit 0x10, si no DEC3N (14) si el bit 0x80,
// si no D3DCOLOR (4). Si algun min < 0 (o NaN): si min.x < -1 o min.y < -1 (o NaN) -> 1; si no SHORT2N (9) con el bit 0x4, si no DEC3N (14) con el bit 0x80, si no 1.
RwUInt32 _rpD3D9FindFormatV2d(const RwV2d* src, RwInt32 numElements) {
    RwV2d mn;
    RwV2d mx;
    _rpD3D9GetMinMaxValuesV2d(src, numElements, &mn, &mx);
    if (!(mx.x <= 1.0f)) {
        return 1;
    }
    if (!(mx.y <= 1.0f)) {
        return 1;
    }
    const RwUInt32 declTypes = *reinterpret_cast<const volatile RwUInt32*>(0xC9BFEC);
    if (mn.x >= 0.0f && mn.y >= 0.0f) {
        if (declTypes & 0x10) {
            return 11;
        }
        return (declTypes & 0x80) ? 14 : 4;
    }
    if (!(mn.x >= -1.0f) || !(mn.y >= -1.0f)) {
        return 1;
    }
    if (declTypes & 0x4) {
        return 9;
    }
    return (declTypes & 0x80) ? 14 : 1;
}
#pragma float_control(pop)

namespace {
// Estado global de las luces de D3D9 (direcciones fijas del EXE; los nombres son inferidos).
// Tres arreglos asignados con RwMalloc (0xC926C4, 0xC926C8, 0xC926D4) con sus contadores/estado (0xC926B8/BC/C0, 0xC926CC/D0, 0xC926D8/DC).
static auto& s_lightsArrayA  = StaticRef<void*>(0xC926C4);
static auto& s_lightsArrayB  = StaticRef<void*>(0xC926C8);
static auto& s_lightsArrayC  = StaticRef<void*>(0xC926D4);
static auto& s_lightsStateB8 = StaticRef<RwUInt32>(0xC926B8);
static auto& s_lightsStateBC = StaticRef<RwUInt32>(0xC926BC);
static auto& s_lightsStateC0 = StaticRef<RwUInt32>(0xC926C0);
static auto& s_lightsStateCC = StaticRef<RwUInt32>(0xC926CC);
static auto& s_lightsStateD0 = StaticRef<RwUInt32>(0xC926D0);
static auto& s_lightsStateD8 = StaticRef<RwUInt32>(0xC926D8);
static auto& s_lightsStateDC = StaticRef<RwUInt32>(0xC926DC);
static auto& s_lightsMaxActive = StaticRef<RwUInt32>(0xC926B4);  // D3DCAPS9::MaxActiveLights, o 0xFFFFFFFF si es 0
static auto& s_d3d9LightPluginOffset = StaticRef<RwInt32>(0xC926B0); // desplazamiento del plugin 0x505 de RpLight

// Parte comun de _rwD3D9LightsOpen y _rwD3D9LightsClose: libera los tres arreglos (RwFree, hueco +0x138 de RwEngineInstance) y pone a cero sus contadores, en el orden del EXE.
inline void D3D9LightsFreeArrays() {
    if (s_lightsArrayA != nullptr) {
        RwFree(s_lightsArrayA);
        s_lightsArrayA = nullptr;
    }
    void* const b = s_lightsArrayB;
    s_lightsStateB8 = 0;
    s_lightsStateBC = 0;
    s_lightsStateC0 = 0;
    if (b != nullptr) {
        RwFree(b);
        s_lightsArrayB = nullptr;
    }
    void* const c = s_lightsArrayC;
    s_lightsStateCC = 0;
    s_lightsStateD0 = 0;
    if (c != nullptr) {
        RwFree(c);
        s_lightsArrayC = nullptr;
    }
    s_lightsStateD8 = 0;
    s_lightsStateDC = 0;
}

inline void StoreWord(uintptr_t address, RwUInt32 value) {
    *reinterpret_cast<RwUInt32*>(address) = value;
}

// 0x755B90 (lote R17A, sin validar; nombre inferido; cdecl sin argumentos, `ret`, 45 bytes; la llama _rpD3D9PluginAttach en 0x74EA41): registra el plugin 0x505 de RpLight (tamano 0x10, constructor 0x755BC0, destructor 0x755BF0, copia 0x755CE0,
// que se llaman por direccion fija) con RpLightRegisterPlugin (0x751D60); guarda el desplazamiento resultante en 0xC926B0 y devuelve TRUE si no es negativo.
RwBool D3D9LightPluginAttach() {
    const RwInt32 offset = RpLightRegisterPlugin(0x10, 0x505,
                                                 reinterpret_cast<RwPluginObjectConstructor>(0x755BC0),
                                                 reinterpret_cast<RwPluginObjectDestructor>(0x755BF0),
                                                 reinterpret_cast<RwPluginObjectCopy>(0x755CE0));
    s_d3d9LightPluginOffset = offset;
    return offset >= 0 ? TRUE : FALSE;
}

// Rango de los indices (no se reconstruye con nombre del SDK; nombre inferido). 0x756710 (lote R17A, sin validar; cdecl, 4 argumentos (indices de 16 bits, cantidad, salida del numero de vertices, salida del primer vertice), `ret`, 85 bytes; llamador 0x7C8E9A):
// min = menor indice y max = mayor indice (comparacion sin signo; min empieza en 0xFFFFFFFF, max en 0; con empate se actualiza igual); *numVertices = max - min + 1 y *minVert = min. Con cantidad 0 deja los dos en 0.
void _rpD3D9FindMinVertAndNumVertices(const RwUInt16* indices, RwUInt32 numIndices, RwUInt32* numVertices, RwUInt32* minVert) {
    if (numIndices == 0) {
        *numVertices = 0;
        *minVert = 0;
        return;
    }
    RwUInt32 mn = 0xFFFFFFFFu;
    RwUInt32 mx = 0;
    for (RwUInt32 n = numIndices; n != 0; --n, ++indices) {
        const RwUInt32 value = *indices;
        if (!(mn < value)) {
            mn = value;
        }
        if (!(mx > value)) {
            mx = value;
        }
    }
    *numVertices = mx - mn + 1;
    *minVert = mn;
}

// 0x756770 (lote R17A, sin validar; nombre inferido; cdecl, 2 argumentos (indices de 16 bits, cantidad de indices), 33 bytes; llamador 0x7C8F78): ordena los triangulos (3 indices de 16 bits, 6 bytes) con qsort del EXE (0x8247E0; no es estable,
// por eso se llama al del EXE) y el comparador 0x7567A0 (por direccion fija): qsort(indices, cantidad / 3, 6, 0x7567A0). La division entre 3 es la del EXE (multiplicar por 0xAAAAAAAB y desplazar 33 bits = division entera sin signo).
void _rpD3D9SortTriangles(RwUInt16* indices, RwUInt32 numIndices) {
    using CompareFn = int(__cdecl*)(const void*, const void*);
    using QsortFn = void(__cdecl*)(void*, size_t, size_t, CompareFn);
    const RwUInt32 numTriangles = static_cast<RwUInt32>((static_cast<unsigned long long>(numIndices) * 0xAAAAAAABull) >> 33);
    reinterpret_cast<QsortFn>(0x8247E0)(indices, numTriangles, 6, reinterpret_cast<CompareFn>(0x7567A0));
}

// 0x756830 (lote R17A, sin validar; nombre inferido; cdecl, 4 argumentos (destino, tira de indices de 16 bits, cantidad de indices, desplazamiento), `ret`, 296 bytes reales (el listado daba 159; termina en 0x756957); llamador 0x756BF0): convierte una tira
// de triangulos en una lista descartando los degenerados. Para i = 0 .. cantidad - 3 (contador SIN signo: cantidad < 3 daria una vuelta gigante, igual que el EXE): (s0, s1, s2) = tira[i..i+2]; si s0 == s1, s0 == s2 o s1 == s2 se salta (comparacion de 16 bits);
// si no escribe tres indices al destino (+3 en el contador de salida): i par (s0 - d, s1 - d, s2 - d), i impar (s2 - d, s1 - d, s0 - d) con d = desplazamiento (resta de 16 bits; con desplazamiento 0 no se resta pero el resultado es el mismo). Devuelve el numero de indices escritos.
RwUInt32 _rpD3D9TriStripToTriList(RwUInt16* dst, const RwUInt16* src, RwUInt32 numIndices, RwUInt32 offset) {
    const RwUInt32 numTris = numIndices - 2;
    RwUInt32 written = 0;
    if (numTris == 0) {
        return 0;
    }
    for (RwUInt32 i = 0; i < numTris; ++i, ++src) {
        const RwUInt16 s0 = src[0];
        const RwUInt16 s1 = src[1];
        const RwUInt16 s2 = src[2];
        if (s0 == s1 || s0 == s2 || s1 == s2) {
            continue;
        }
        const RwUInt16 o = static_cast<RwUInt16>(offset);
        if (i & 1) {
            dst[2] = static_cast<RwUInt16>(s0 - o);
            dst[1] = static_cast<RwUInt16>(s1 - o);
            dst[0] = static_cast<RwUInt16>(s2 - o);
        } else {
            dst[0] = static_cast<RwUInt16>(s0 - o);
            dst[1] = static_cast<RwUInt16>(s1 - o);
            dst[2] = static_cast<RwUInt16>(s2 - o);
        }
        dst += 3;
        written += 3;
    }
    return written;
}

// 0x756D90 (lote R17A, sin validar; nombre inferido; cdecl, 2 argumentos (objeto, tipo), 81 bytes; llamadores 0x5D6495, 0x5D77E5, 0x5D993F, 0x815C32): activa o desactiva el recorte de D3D9 (estado 0x88 = D3DRS_CLIPPING) segun si el objeto cae por completo
// dentro de la piramide de la camara actual (primer dword de RwEngineInstance). Con tipo 1 el objeto es un atomo: RwD3D9CameraIsSphereFullyInsideFrustum (0x7FAD30) con su esfera de mundo (RpAtomicGetWorldBoundingSphere 0x749330); con cualquier otro tipo es un sector
// y se usa la caja +0x60 con RwD3D9CameraIsBBoxFullyInsideFrustum (0x7FAD90). Despues RwD3D9SetRenderState(0x88, resultado == 0) como salto final (cola); no devuelve nada util.
void _rwD3D9SetClippingForObject(void* object, RwInt32 type) {
    const void* const camera = RwEngineInstance->curCamera;
    RwBool fullyInside;
    if (type == 1) {
        fullyInside = RwD3D9CameraIsSphereFullyInsideFrustum(camera, RpAtomicGetWorldBoundingSphere(static_cast<RpAtomic*>(object)));
    } else {
        fullyInside = RwD3D9CameraIsBBoxFullyInsideFrustum(camera, static_cast<const RwUInt8*>(object) + 0x60);
    }
    RwD3D9SetRenderState(0x88, fullyInside == 0 ? 1 : 0);
}

// Datos privados del nodo AllInOne de D3D9 (apuntados por RxPipelineNode::privateData, +0x14): callbacks de instanciacion y de reinstanciacion (0x757380..0x7573B0).
struct D3D9AllInOnePrivateCallbacks {
    RxD3D9AllInOneInstanceCallBack   instance;
    RxD3D9AllInOneReinstanceCallBack reinstance;
};
inline D3D9AllInOnePrivateCallbacks* AllInOneCallbacks(RxPipelineNode* node) {
    return static_cast<D3D9AllInOnePrivateCallbacks*>(node->privateData);
}
} // namespace

// 0x755D80 (lote R17A, sin validar; sustituye la envoltura; cdecl sin argumentos, 604 bytes): abre el estado de luces de D3D9. Libera los tres arreglos (como _rwD3D9LightsClose), guarda en 0xC926B4 D3DCAPS9::MaxActiveLights (+0xA0 de RwD3D9GetCaps
// 0x7FAD20; 0xFFFFFFFF si es 0) e inicializa las variables fijas: ambiente saturado 0x8E2418 = (0, 0, 0, 1.0f), 0xC92648 = 3, y los vectores/colores globales de 0xC925E4..0xC926AC a (0, 0, 0, 1.0f) o ceros segun el EXE (ver las escrituras en orden). Devuelve TRUE.
RwBool _rwD3D9LightsOpen(void) {
    D3D9LightsFreeArrays();
    const RwUInt32 maxActive = *reinterpret_cast<const RwUInt32*>(reinterpret_cast<const RwUInt8*>(RwD3D9GetCaps()) + 0xA0);
    s_lightsMaxActive = maxActive;
    if (maxActive == 0) {
        s_lightsMaxActive = 0xFFFFFFFFu;
    }
    constexpr RwUInt32 kOne = 0x3F800000u;
    StoreWord(0x8E2418, 0);
    StoreWord(0x8E241C, 0);
    StoreWord(0x8E2420, 0);
    StoreWord(0x8E2424, kOne);
    StoreWord(0xC92648, 3);
    StoreWord(0xC9264C, 0);
    StoreWord(0xC92650, 0);
    StoreWord(0xC92654, 0);
    StoreWord(0xC92658, kOne);
    StoreWord(0xC92688, 0);
    StoreWord(0xC9268C, 0);
    StoreWord(0xC92690, 0);
    StoreWord(0xC9265C, 0);
    StoreWord(0xC92660, 0);
    StoreWord(0xC92664, 0);
    StoreWord(0xC92668, kOne);
    StoreWord(0xC9266C, 0);
    StoreWord(0xC92670, 0);
    StoreWord(0xC92674, 0);
    StoreWord(0xC92678, kOne);
    StoreWord(0xC9267C, 0);
    StoreWord(0xC92680, 0);
    StoreWord(0xC92684, 0);
    StoreWord(0xC92694, 0);
    StoreWord(0xC92698, 0);
    StoreWord(0xC9269C, 0);
    StoreWord(0xC926A0, 0);
    StoreWord(0xC926A4, 0);
    StoreWord(0xC926A8, 0);
    StoreWord(0xC926AC, 0);
    StoreWord(0xC925E4, 0);
    StoreWord(0xC925E8, 0);
    StoreWord(0xC925EC, 0);
    StoreWord(0xC925F0, kOne);
    StoreWord(0xC925F4, 0);
    StoreWord(0xC925F8, 0);
    StoreWord(0xC925FC, 0);
    StoreWord(0xC92600, kOne);
    StoreWord(0xC92604, 0);
    StoreWord(0xC92608, 0);
    StoreWord(0xC9260C, 0);
    StoreWord(0xC92610, kOne);
    StoreWord(0xC92630, kOne);
    return TRUE;
}

// 0x755FE0 (lote R17A, sin validar; sustituye la envoltura; cdecl sin argumentos, 139 bytes): libera los tres arreglos de luces de D3D9 con RwFree y pone a cero sus punteros y contadores (0xC926C4, 0xC926C8 + 0xC926B8/BC/C0, 0xC926D4 + 0xC926CC/D0, 0xC926D8, 0xC926DC).
void _rwD3D9LightsClose(void) {
    D3D9LightsFreeArrays();
}

// 0x757380 (lote R17A, sin validar; sustituye la envoltura; cdecl, 2 argumentos, `ret`, 14 bytes): privateData->instance = callback (privateData en +0x14 del nodo).
void RxD3D9AllInOneSetInstanceCallBack(RxPipelineNode* node, RxD3D9AllInOneInstanceCallBack callback) {
    AllInOneCallbacks(node)->instance = callback;
}

// 0x757390 (lote R17A, sin validar; sustituye la envoltura; cdecl, 1 argumento, 10 bytes): devuelve privateData->instance.
RxD3D9AllInOneInstanceCallBack RxD3D9AllInOneGetInstanceCallBack(RxPipelineNode* node) {
    return AllInOneCallbacks(node)->instance;
}

// 0x7573A0 (lote R17A, sin validar; sustituye la envoltura; cdecl, 2 argumentos, 15 bytes): privateData->reinstance = callback.
void RxD3D9AllInOneSetReinstanceCallBack(RxPipelineNode* node, RxD3D9AllInOneReinstanceCallBack callback) {
    AllInOneCallbacks(node)->reinstance = callback;
}

// 0x7573B0 (lote R17A, sin validar; sustituye la envoltura; cdecl, 1 argumento, 11 bytes): devuelve privateData->reinstance.
RxD3D9AllInOneReinstanceCallBack RxD3D9AllInOneGetReinstanceCallBack(RxPipelineNode* node) {
    return AllInOneCallbacks(node)->reinstance;
}

// 0x7551F0 (lote R10A, sin validar; sustituye la envoltura; cdecl, 3 argumentos (tipo de elemento de la declaracion de vertices, destino RwV3d, origen), `ret`, 936 bytes; el listado la atribuia a RpMatFX por vecindad):
// lee un elemento de un vertice D3D9 y lo deja como RwV3d. Tipos (D3DDECLTYPE, tabla de saltos 0x755598; mayor que 14 -> (0,0,0)): 0 FLOAT1 = (dword, 0, 0); 1 FLOAT2 = (dword, dword, 0); 2 y 3 (FLOAT3 y FLOAT4) = tres dwords;
// 4 D3DCOLOR = (byte2, byte1, byte0) * (1/255) [0x859A3C = 0x3B808081]; 5 UBYTE4 = (byte0, byte1, byte2) sin escala; 6 SHORT2 = (short0, short1, 0); 7 SHORT4 = tres shorts; 8 UBYTE4N = (byte0, byte1, byte2) * (1/255);
// 9 SHORT2N = (short0, short1, 0) * (1/32767) [0x858C7C = 0x38000100]; 10 SHORT4N = tres shorts * (1/32767); 11 USHORT2N y 12 USHORT4N = como 9 y 10 pero con (1/65535) [0x874F48 = 0x37800080] y leyendo los words CON SIGNO (movsx) como el EXE;
// 13 UDEC3 = tres campos de 10 bits sin signo (bits 0, 10 y 20) sin escala; 14 DEC3N = los mismos campos con signo (extension del bit 9) * (1/511) [0x874F40 = 0x3B004020]. Las conversiones entero a float son `fild` + `fmul` + `fstp` en la x87 a 24 bits:
// el entero es exacto y el producto se redondea una sola vez a 24 bits, igual que la multiplicacion float de C++. Los casos 0..3 copian dwords (sin pasar por la x87). Escribe siempre los tres componentes.
void _rpD3D9VertexDeclarationUnInstV3d(RwUInt32 type, RwV3d* dst, const RwUInt8* src) {
    constexpr float kByteScale    = std::bit_cast<float>(0x3B808081u); // 0x859A3C
    constexpr float kShortScale   = std::bit_cast<float>(0x38000100u); // 0x858C7C
    constexpr float kUShortScale  = std::bit_cast<float>(0x37800080u); // 0x874F48
    constexpr float kDecScale     = std::bit_cast<float>(0x3B004020u); // 0x874F40
    const auto readDword = [src](size_t index) {
        RwUInt32 value;
        std::memcpy(&value, src + index * sizeof(RwUInt32), sizeof(value));
        return value;
    };
    const auto readShort = [src](size_t index) {
        RwInt16 value;
        std::memcpy(&value, src + index * sizeof(RwInt16), sizeof(value));
        return static_cast<RwInt32>(value);
    };
    auto* const out = reinterpret_cast<RwUInt32*>(dst);
    switch (type) {
    case 0:
        out[0] = readDword(0);
        out[1] = 0;
        out[2] = 0;
        return;
    case 1:
        out[0] = readDword(0);
        out[1] = readDword(1);
        out[2] = 0;
        return;
    case 2:
    case 3:
        out[0] = readDword(0);
        out[1] = readDword(1);
        out[2] = readDword(2);
        return;
    case 4:
        dst->x = static_cast<float>(static_cast<RwInt32>(src[2])) * kByteScale;
        dst->y = static_cast<float>(static_cast<RwInt32>(src[1])) * kByteScale;
        dst->z = static_cast<float>(static_cast<RwInt32>(src[0])) * kByteScale;
        return;
    case 5:
        dst->x = static_cast<float>(static_cast<RwInt32>(src[0]));
        dst->y = static_cast<float>(static_cast<RwInt32>(src[1]));
        dst->z = static_cast<float>(static_cast<RwInt32>(src[2]));
        return;
    case 6:
        dst->x = static_cast<float>(readShort(0));
        out[2] = 0;
        dst->y = static_cast<float>(readShort(1));
        return;
    case 7:
        dst->x = static_cast<float>(readShort(0));
        dst->y = static_cast<float>(readShort(1));
        dst->z = static_cast<float>(readShort(2));
        return;
    case 8:
        dst->x = static_cast<float>(static_cast<RwInt32>(src[0])) * kByteScale;
        dst->y = static_cast<float>(static_cast<RwInt32>(src[1])) * kByteScale;
        dst->z = static_cast<float>(static_cast<RwInt32>(src[2])) * kByteScale;
        return;
    case 9:
        dst->x = static_cast<float>(readShort(0)) * kShortScale;
        out[2] = 0;
        dst->y = static_cast<float>(readShort(1)) * kShortScale;
        return;
    case 10:
        dst->x = static_cast<float>(readShort(0)) * kShortScale;
        dst->y = static_cast<float>(readShort(1)) * kShortScale;
        dst->z = static_cast<float>(readShort(2)) * kShortScale;
        return;
    case 11:
        dst->x = static_cast<float>(readShort(0)) * kUShortScale;
        out[2] = 0;
        dst->y = static_cast<float>(readShort(1)) * kUShortScale;
        return;
    case 12:
        dst->x = static_cast<float>(readShort(0)) * kUShortScale;
        dst->y = static_cast<float>(readShort(1)) * kUShortScale;
        dst->z = static_cast<float>(readShort(2)) * kUShortScale;
        return;
    case 13: {
        const RwUInt32 packed = readDword(0);
        dst->x = static_cast<float>(packed & 0x3FFu);
        dst->y = static_cast<float>((packed >> 10) & 0x3FFu);
        dst->z = static_cast<float>((packed >> 20) & 0x3FFu);
        return;
    }
    case 14: {
        const RwUInt32 packed = readDword(0);
        const auto signExtend10 = [](RwUInt32 field) {
            return static_cast<RwInt32>((field & 0x200u) != 0 ? (field | 0xFFFFFC00u) : field);
        };
        const RwInt32 x = signExtend10(packed & 0x3FFu);
        const RwInt32 y = signExtend10((packed >> 10) & 0x3FFu);
        const RwInt32 z = signExtend10((packed >> 20) & 0x3FFu);
        dst->x = static_cast<float>(x) * kDecScale;
        dst->y = static_cast<float>(y) * kDecScale;
        dst->z = static_cast<float>(z) * kDecScale;
        return;
    }
    default:
        out[0] = 0;
        out[1] = 0;
        out[2] = 0;
        return;
    }
}

void _rpD3D9GetMinMaxValuesV3d(const RwV3d* src, RwInt32 numElements, RwV3d* min, RwV3d* max) {
    ((void(__cdecl *)(const RwV3d*, RwInt32, RwV3d*, RwV3d*))0x755830)(src, numElements, min, max);
}

RwUInt32 _rpD3D9FindFormatV3d(const RwV3d* src, RwInt32 numElements) {
    return ((RwUInt32(__cdecl *)(const RwV3d*, RwInt32))0x755980)(src, numElements);
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

// 0x750F50 (lote R16A, sin validar; sustituye la envoltura; cdecl, 2 argumentos, 33 bytes): los datos de plugin de la camara (camara + [0xC925A4]) guardan el mundo en +0x0C. Si es nulo devuelve NULL
// sin tocar nada; si no, pone a cero +0x0C y +0x08 y devuelve el mundo.
RpWorld* RpWorldRemoveCamera(RpWorld* world, RwCamera* camera) {
    uint8_t* const data = reinterpret_cast<uint8_t*>(camera) + s_cameraWorldOffset;
    RwUInt32& cameraWorld = *reinterpret_cast<RwUInt32*>(data + 0x0C);
    if (cameraWorld == 0) {
        return nullptr;
    }
    cameraWorld = 0;
    *reinterpret_cast<RwUInt32*>(data + 0x08) = 0;
    return world;
}

// 0x750F20 (lote R16A, sin validar; sustituye la envoltura; cdecl, 2 argumentos, 39 bytes): si la camara tiene marco (+4), RwFrameUpdateObjects (0x7F0910); despues guarda el mundo en los datos de
// plugin de la camara (+0x0C) y devuelve el mundo. No comprueba que la camara ya estuviera en otro mundo.
RpWorld* RpWorldAddCamera(RpWorld* world, RwCamera* camera) {
    uint8_t* const data = reinterpret_cast<uint8_t*>(camera) + s_cameraWorldOffset;
    if (camera->object.object.parent) {
        RwFrameUpdateObjects(static_cast<RwFrame*>(camera->object.object.parent));
    }
    *reinterpret_cast<RpWorld**>(data + 0x0C) = world;
    return world;
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

// 0x751300 (lote R16A, sin validar; sustituye la envoltura; cdecl, 2 argumentos, 143 bytes): anade un clump al mundo. Enlaza clump->inWorldLink (+0x20) al principio de world->clumpList (+0x2C) y
// suma 1 a numClumpsInWorld (+0x24); guarda el mundo en los datos de plugin del clump (clump + [0xC925AC], +0). Despues, con el mundo como dato: RpClumpForAllAtomics(0x751390),
// RpClumpForAllLights(0x7513D0) y RpClumpForAllCameras(0x751420) (callbacks por direccion, sin reconstruir). Si el clump tiene marco (padre, leido ANTES de los recorridos):
// RwMatrixOptimize(&frame->modelling (+0x10), NULL) (0x7F17E0) y RwFrameUpdateObjects(frame). Por ultimo copia el contador de ties (RwEngineInstance + [0xC9259C] + 8) a +4 de los datos del clump; devuelve el mundo.
RpWorld* RpWorldAddClump(RpWorld* world, RpClump* clump) {
    uint8_t* const data = reinterpret_cast<uint8_t*>(clump) + s_clumpWorldOffset;
    RwFrame* const frame = static_cast<RwFrame*>(clump->object.parent);
    RwLLLink* const link = &clump->inWorldLink;
    link->prev = &world->clumpList.link;
    link->next = world->clumpList.link.next;
    world->clumpList.link.next->prev = link;
    world->clumpList.link.next = link;
    ++world->numClumpsInWorld;
    *reinterpret_cast<RpWorld**>(data) = world;
    RpClumpForAllAtomics(clump, reinterpret_cast<RpAtomicCallBack>(static_cast<uintptr_t>(0x751390)), world);
    RpClumpForAllLights(clump, reinterpret_cast<RpLightCallBack>(static_cast<uintptr_t>(0x7513D0)), world);
    RpClumpForAllCameras(clump, reinterpret_cast<RwCameraCallBack>(static_cast<uintptr_t>(0x751420)), world);
    if (frame) {
        RwMatrixOptimize(&frame->modelling, nullptr);
        RwFrameUpdateObjects(frame);
    }
    *reinterpret_cast<RwUInt32*>(data + 4) = TieGlobals().tieFrame;
    return world;
}

RpWorld* RpWorldRemoveClump(RpWorld* world, RpClump* clump) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RpClump*))0x751460)(world, clump);
}

RpWorld* RpClumpGetWorld(const RpClump* clump) {
    return ((RpWorld*(__cdecl *)(const RpClump*))0x7516B0)(clump);
}

// 0x751910 (lote R16A, sin validar; sustituye la envoltura; cdecl, 2 argumentos, 71 bytes): guarda el mundo en los datos de plugin de la luz (luz + [0xC925B0], +0). subType < 0x80 (direccional y
// ambiental, sin posicion) va a world->directionalLightList (+0x3C) sin tocar el marco; el resto (puntual y foco) hace antes RwFrameUpdateObjects (0x7F0910) del marco si lo hay y va a world->lightList
// (+0x34). El enlace light->inWorld (+0x34) se pone al principio de la lista elegida (rwLinkListAddLLLink). Devuelve el mundo.
RpWorld* RpWorldAddLight(RpWorld* world, RpLight* light) {
    *reinterpret_cast<RpWorld**>(reinterpret_cast<uint8_t*>(light) + s_lightWorldOffset) = world;
    RwLinkList* list;
    if (light->object.object.subType < 0x80) {
        list = &world->directionalLightList;
    } else {
        if (light->object.object.parent) {
            RwFrameUpdateObjects(static_cast<RwFrame*>(light->object.object.parent));
        }
        list = &world->lightList;
    }
    rwLinkListAddLLLink(list, &light->inWorld);
    return world;
}

// 0x751960 (lote R16A, sin validar; sustituye la envoltura; cdecl, 2 argumentos, 119 bytes): pone a cero el mundo de los datos de plugin de la luz, destruye todos sus RpLightTie (LightDestroyAllTies,
// igual que el destructor de plugin 0x750D30) y quita light->inWorld (+0x34) de la lista del mundo. Devuelve el mundo.
RpWorld* RpWorldRemoveLight(RpWorld* world, RpLight* light) {
    *reinterpret_cast<RpWorld**>(reinterpret_cast<uint8_t*>(light) + s_lightWorldOffset) = nullptr;
    LightDestroyAllTies(light);
    rwLinkListRemoveLLLink(&light->inWorld);
    return world;
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

// 0x7519F0 (lote R16A, sin validar; sustituye la envoltura; cdecl, 3 argumentos, 114 bytes): subType < 0x80: si la luz esta en un mundo (datos de plugin +0), RpWorldForAllWorldSectors(mundo,
// callback, dato) y devuelve la luz si ese resultado no es NULL, si no NULL; sin mundo devuelve la luz. subType >= 0x80: recorre los enlaces de WorldSectorsInLight (+0x2C) leyendo el siguiente antes
// de llamar, callback(sector = enlace + 8, dato); un resultado NULL corta el bucle; devuelve siempre la luz.
RpLight* RpLightForAllWorldSectors(RpLight* light, RpWorldSectorCallBack callback, void* data) {
    if (light->object.object.subType < 0x80) {
        RpWorld* const world = *reinterpret_cast<RpWorld**>(reinterpret_cast<uint8_t*>(light) + s_lightWorldOffset);
        if (!world) {
            return light;
        }
        return RpWorldForAllWorldSectors(world, callback, data) ? light : nullptr;
    }
    RwLLLink* const end = &light->WorldSectorsInLight.link;
    RwLLLink* link = end->next;
    while (link != end) {
        RwLLLink* const next = link->next;
        if (!callback(*reinterpret_cast<RpWorldSector**>(reinterpret_cast<uint8_t*>(link) + 8), data)) {
            return light;
        }
        link = next;
    }
    return light;
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
// Registra los plugins de D3D9 (luces 0x755B90, ahora en C++, y si fue bien, geometria 0x7587D0); guarda el callback de restauracion del dispositivo que habia y
// pone 0x74EA80 (que marca las geometrias dinamicas como sucias y encadena el anterior). Devuelve 1 si los dos registros fueron bien.
static RwBool _rpD3D9PluginAttach() {
    RwBool result = FALSE;
    if (D3D9LightPluginAttach() && D3D9GeometryPluginAttach()) {
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
// vacia, flags 0, orden de render 2) y reserva el sector raiz (tamano del registro 0x8D736C; fallo: RWERROR con el argumento 4 como el EXE, libera el mundo y 0).
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
    world->renderOrder = static_cast<RpWorldRenderOrder>(2); // el EXE guarda 2 (el enumerador FRONT2BACK del encabezado vale 1: el banco Fn229A encontro la diferencia)

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

// Lote 230A (sin validar): destruccion de los nodos de enlace (tie); las estructuras y TieUnlinkBoth estan al principio del archivo.

// 0x74FFA0 (cdecl, un argumento, 68 bytes; el listado decia Win): destruccion de un RpLightTie. Desenlaza sin comprobar los punteros y libera el nodo en la lista libre de +4; devuelve 1.
static RwBool _rpLightTieDestroy(RpLightTie* tie) {
    TieUnlinkBoth(&tie->lightInWorldSector, &tie->WorldSectorInLight);
    FreeListFree(TieGlobals().lightTieFreeList, tie);
    return TRUE;
}

// 0x74FFF0 (cdecl, un argumento, 81 bytes; el listado decia Win): destruccion de un RpTie. Solo actua si el atomo (+0x08) Y el sector (+0x14) no son nulos (si falta uno no desenlaza ni libera); entonces
// desenlaza igual que _rpLightTieDestroy y libera el nodo en la lista libre de +0. Devuelve 1 siempre.
static RwBool _rpTieDestroy(RpTie* tie) {
    if (tie->apAtom && tie->worldSector) {
        TieUnlinkBoth(&tie->lAtomicInWorldSector, &tie->lWorldSectorInAtomic);
        FreeListFree(TieGlobals().tieFreeList, tie);
    }
    return TRUE;
}

// Registro de los hooks del lote 230A de RpWorld (sin validar).
void RpWorldInjectHooks230A() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpWorld");

    RH_ScopedGlobalInstall(RpWorldPluginAttach, 0x74FDA0);
    RH_ScopedGlobalInstall(_rpLightTieDestroy, 0x74FFA0);
    RH_ScopedGlobalInstall(_rpTieDestroy, 0x74FFF0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Lote R16A (sin validar): plugin 0x509 (mundo del objeto y ties), plugins de malla 0x50E / 0x510 de geometria y sector, destruccion de luces y registro de la lista libre.

// 0x750D30 (cdecl, 3 argumentos (objeto, desplazamiento, tamano), 89 bytes): destructor del plugin 0x509 de RpLight. Destruye todos los RpLightTie de la luz (LightDestroyAllTies: no toca el mundo
// guardado ni el enlace inWorld) y devuelve el objeto. Los otros dos argumentos no se leen.
static void* LightWorldPluginDestruct(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    (void)offsetInObject;
    (void)sizeInObject;
    LightDestroyAllTies(static_cast<RpLight*>(object));
    return object;
}

// 0x750DB0 (cdecl, 5 argumentos, 36 bytes): lectura del plugin de malla 0x50E de RpGeometry. geometry->mesh (+0x54) = _rpMeshRead(flujo, geometria, &geometry->matList (+0x20)); devuelve el flujo si la malla
// no es NULL y NULL si lo es (la malla se guarda en cualquier caso). No usa la longitud ni el desplazamiento.
static RwStream* GeometryMeshStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    (void)binaryLength;
    (void)offsetInObject;
    (void)sizeInObject;
    RpGeometry* const geometry = static_cast<RpGeometry*>(object);
    geometry->mesh = _rpMeshRead(stream, geometry, &geometry->matList);
    return geometry->mesh ? stream : nullptr;
}

// 0x750EA0 (cdecl, 5 argumentos, 35 bytes): escritura del plugin 0x50E de RpWorldSector. world = RpWorldSectorGetWorld(sector); devuelve _rpMeshWrite(sector->mesh (+0x78), world, flujo, &world->matList (+0x10)).
static RwStream* SectorMeshStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    (void)binaryLength;
    (void)offsetInObject;
    (void)sizeInObject;
    const RpWorldSector* const sector = static_cast<const RpWorldSector*>(object);
    RpWorld* const world = RpWorldSectorGetWorld(sector);
    return _rpMeshWrite(sector->mesh, world, stream, &world->matList);
}

// 0x750ED0 (cdecl, 5 argumentos, 42 bytes): lectura del plugin 0x50E de RpWorldSector. world = RpWorldSectorGetWorld(sector); sector->mesh (+0x78) = _rpMeshRead(flujo, mundo, &world->matList (+0x10));
// devuelve el flujo si la malla no es NULL y NULL si lo es.
static RwStream* SectorMeshStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    (void)binaryLength;
    (void)offsetInObject;
    (void)sizeInObject;
    RpWorldSector* const sector = static_cast<RpWorldSector*>(object);
    RpWorld* const world = RpWorldSectorGetWorld(sector);
    sector->mesh = _rpMeshRead(stream, world, &world->matList);
    return sector->mesh ? stream : nullptr;
}

// 0x750F00 (cdecl, 3 argumentos (objeto, desplazamiento, tamano), 26 bytes): tamano del plugin 0x50E de RpWorldSector = _rpMeshSize(sector->mesh (+0x78), RpWorldSectorGetWorld(sector)).
static RwInt32 SectorMeshStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    (void)offsetInObject;
    (void)sizeInObject;
    const RpWorldSector* const sector = static_cast<const RpWorldSector*>(object);
    return _rpMeshSize(sector->mesh, RpWorldSectorGetWorld(sector));
}

// 0x752210 (cdecl, 1 argumento util, 49 bytes): callback de la lista libre de luces (lo usa el destructor 0x7521B0 del plugin de motor 0x505): mismo cuerpo que RpLightDestroy sin valor de retorno.
static void LightFreeListDestroyCallBack(void* light, void* data) {
    (void)data;
    LightDestroyBody(static_cast<RpLight*>(light));
}

// 0x750050 (cdecl, sin argumentos, 549 bytes; el listado la atribuia a RpWorld): registro de los plugins 0x509, 0x50E, 0x510 y 0x1F del modulo; lo llama RpWorldPluginAttach (0x74FDA0). En este orden (cada
// resultado se acumula con OR; los desplazamientos se guardan en sus globales):
//  1. RwEngineRegisterPlugin(0xC, 0x509, ctor 0x750280 (listas libres de ties), dtor 0x750370).
//  2. RwCameraRegisterPlugin(0x1C, 0x509, 0x7503E0, 0x750760, copia 0x750710) -> 0xC925A4; RpAtomicRegisterPlugin(8, 0x509, 0x7507B0, 0x7509B0, 0x7509A0) -> 0xC925A8;
//     RpClumpRegisterPlugin(8, 0x509, 0x750A30, 0x750A90, 0x750A60) -> 0xC925AC; RpLightRegisterPlugin(8, 0x509, 0x750AA0, 0x750D30, 0x750CD0) -> 0xC925B0.
//  3. Para 0x50E y despues 0x510: RpGeometryRegisterPlugin(0, id, 0, 0, 0), RpWorldSectorRegisterPlugin(0, id, 0, 0, 0), flujo de geometria (id, lectura, escritura, tamano) y flujo de sector.
//     0x50E: geometria (0x750DB0, 0x750D90, 0x750DE0), sector (0x750ED0, 0x750EA0, 0x750F00); 0x510: geometria (0x750E20, 0x750E00, 0x750E40), sector (0x750E70, 0x750E50, 0x750E90).
//  4. Plugin 0x1F (derechos de render): RpAtomicRegisterPlugin(0, 0x1F, 0, 0, 0) y su flujo (0x749120, 0x749160, 0x7491A0); RpWorldRegisterPlugin y su flujo (0x761D60, 0x761DA0, 0x761DE0);
//     RpWorldSectorRegisterPlugin y su flujo (0x761E00, 0x761E40, 0x761E80); RpMaterialRegisterPlugin y su flujo (0x74D7D0, 0x74D810, 0x74D850).
// Si el OR es negativo devuelve 0; si no, devuelve (0x75E3E0() != 0).
static RwBool _rpTieMeshPluginAttach() {
    const auto ctor = [](uintptr_t address) { return reinterpret_cast<RwPluginObjectConstructor>(address); };
    const auto dtor = [](uintptr_t address) { return reinterpret_cast<RwPluginObjectDestructor>(address); };
    const auto copy = [](uintptr_t address) { return reinterpret_cast<RwPluginObjectCopy>(address); };
    const auto readCB = [](uintptr_t address) { return reinterpret_cast<RwPluginDataChunkReadCallBack>(address); };
    const auto writeCB = [](uintptr_t address) { return reinterpret_cast<RwPluginDataChunkWriteCallBack>(address); };
    const auto sizeCB = [](uintptr_t address) { return reinterpret_cast<RwPluginDataChunkGetSizeCallBack>(address); };

    RwInt32 status = RwEngineRegisterPlugin(0xC, 0x509, ctor(0x750280), dtor(0x750370));
    s_cameraWorldOffset = RwCameraRegisterPlugin(0x1C, 0x509, ctor(0x7503E0), dtor(0x750760), copy(0x750710));
    status |= s_cameraWorldOffset;
    s_atomicWorldOffset = RpAtomicRegisterPlugin(8, 0x509, ctor(0x7507B0), dtor(0x7509B0), copy(0x7509A0));
    status |= s_atomicWorldOffset;
    s_clumpWorldOffset = RpClumpRegisterPlugin(8, 0x509, ctor(0x750A30), dtor(0x750A90), copy(0x750A60));
    status |= s_clumpWorldOffset;
    s_lightWorldOffset = RpLightRegisterPlugin(8, 0x509, ctor(0x750AA0), LightWorldPluginDestruct, copy(0x750CD0));
    status |= s_lightWorldOffset;

    status |= RpGeometryRegisterPlugin(0, 0x50E, nullptr, nullptr, nullptr);
    status |= RpWorldSectorRegisterPlugin(0, 0x50E, nullptr, nullptr, nullptr);
    status |= RpGeometryRegisterPluginStream(0x50E, GeometryMeshStreamRead, writeCB(0x750D90), sizeCB(0x750DE0));
    status |= RpWorldSectorRegisterPluginStream(0x50E, SectorMeshStreamRead, SectorMeshStreamWrite, SectorMeshStreamGetSize);

    status |= RpGeometryRegisterPlugin(0, 0x510, nullptr, nullptr, nullptr);
    status |= RpWorldSectorRegisterPlugin(0, 0x510, nullptr, nullptr, nullptr);
    status |= RpGeometryRegisterPluginStream(0x510, readCB(0x750E20), writeCB(0x750E00), sizeCB(0x750E40));
    status |= RpWorldSectorRegisterPluginStream(0x510, readCB(0x750E70), writeCB(0x750E50), sizeCB(0x750E90));

    status |= RpAtomicRegisterPlugin(0, 0x1F, nullptr, nullptr, nullptr);
    status |= RpAtomicRegisterPluginStream(0x1F, readCB(0x749120), writeCB(0x749160), sizeCB(0x7491A0));
    status |= RpWorldRegisterPlugin(0, 0x1F, nullptr, nullptr, nullptr);
    status |= RpWorldRegisterPluginStream(0x1F, _rpWorldRightsStreamRead, _rpWorldRightsStreamWrite, sizeCB(0x761DE0));
    status |= RpWorldSectorRegisterPlugin(0, 0x1F, nullptr, nullptr, nullptr);
    status |= RpWorldSectorRegisterPluginStream(0x1F, _rpWorldSectorRightsStreamRead, _rpWorldSectorRightsStreamWrite, sizeCB(0x761E80));
    status |= RpMaterialRegisterPlugin(0, 0x1F, nullptr, nullptr, nullptr);
    status |= RpMaterialRegisterPluginStream(0x1F, readCB(0x74D7D0), writeCB(0x74D810), sizeCB(0x74D850));

    if (status < 0) {
        return FALSE;
    }
    return reinterpret_cast<RwBool(__cdecl*)()>(0x75E3E0)() != 0;
}

// Registro de los hooks del lote R16A de RpWorld (sin validar).
void RpWorldInjectHooksR16A() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpWorld");

    RH_ScopedGlobalInstall(_rpTieMeshPluginAttach, 0x750050);
    RH_ScopedGlobalInstall(LightWorldPluginDestruct, 0x750D30);
    RH_ScopedGlobalInstall(GeometryMeshStreamRead, 0x750DB0);
    RH_ScopedGlobalInstall(SectorMeshStreamWrite, 0x750EA0);
    RH_ScopedGlobalInstall(SectorMeshStreamRead, 0x750ED0);
    RH_ScopedGlobalInstall(SectorMeshStreamGetSize, 0x750F00);
    RH_ScopedGlobalInstall(RpWorldAddCamera, 0x750F20);
    RH_ScopedGlobalInstall(RpWorldRemoveCamera, 0x750F50);
    RH_ScopedGlobalInstall(RpWorldAddClump, 0x751300);
    RH_ScopedGlobalInstall(RpWorldAddLight, 0x751910);
    RH_ScopedGlobalInstall(RpWorldRemoveLight, 0x751960);
    RH_ScopedGlobalInstall(RpLightForAllWorldSectors, 0x7519F0);
    RH_ScopedGlobalInstall(RpLightSetRadius, 0x751A70);
    RH_ScopedGlobalInstall(RpLightSetColor, 0x751A90);
    RH_ScopedGlobalInstall(RpLightStreamRead, 0x751F00);
    RH_ScopedGlobalInstall(RpLightDestroy, 0x7520D0);
    RH_ScopedGlobalInstall(RpLightCreate, 0x752110);
    RH_ScopedGlobalInstall(LightFreeListDestroyCallBack, 0x752210);
    RH_ScopedGlobalInstall(_rpD3D9VertexDeclarationInstWeights, 0x752320);
}

// Registro de los hooks del lote R17A de RpWorld (sin validar): instanciacion y lectura de vertices D3D9, formato de texcoord, plugin de luces 0x505,
// estado de luces, utilidades de indices, recorte por piramide y accesores de callbacks del nodo AllInOne.
void RpWorldInjectHooksR17A() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpWorld");

    RH_ScopedGlobalInstall(_rpD3D9VertexDeclarationInstV3d, 0x752AD0);
    RH_ScopedGlobalInstall(_rpD3D9VertexDeclarationInstV3dMorph, 0x753B60);
    RH_ScopedGlobalInstall(_rpD3D9VertexDeclarationInstV2d, 0x7544E0);
    RH_ScopedGlobalInstall(_rpD3D9VertexDeclarationInstColor, 0x754AE0);
    RH_ScopedGlobalInstall(_rpD3D9VertexDeclarationInstIndicesRemap, 0x754C80);
    RH_ScopedGlobalInstall(_rpD3D9VertexDeclarationInstTangent, 0x754E20);
    RH_ScopedGlobalInstall(_rpD3D9VertexDeclarationUnInstV2d, 0x7555E0);
    RH_ScopedGlobalInstall(_rpD3D9GetMinMaxValuesV2d, 0x7558F0);
    RH_ScopedGlobalInstall(_rpD3D9FindFormatV2d, 0x755AA0);
    RH_ScopedGlobalInstall(D3D9LightPluginAttach, 0x755B90);
    RH_ScopedGlobalInstall(_rwD3D9LightsOpen, 0x755D80);
    RH_ScopedGlobalInstall(_rwD3D9LightsClose, 0x755FE0);
    RH_ScopedGlobalInstall(_rpD3D9FindMinVertAndNumVertices, 0x756710);
    RH_ScopedGlobalInstall(_rpD3D9SortTriangles, 0x756770);
    RH_ScopedGlobalInstall(_rpD3D9TriStripToTriList, 0x756830);
    RH_ScopedGlobalInstall(_rwD3D9SetClippingForObject, 0x756D90);
    RH_ScopedGlobalInstall(RxD3D9AllInOneSetInstanceCallBack, 0x757380);
    RH_ScopedGlobalInstall(RxD3D9AllInOneGetInstanceCallBack, 0x757390);
    RH_ScopedGlobalInstall(RxD3D9AllInOneSetReinstanceCallBack, 0x7573A0);
    RH_ScopedGlobalInstall(RxD3D9AllInOneGetReinstanceCallBack, 0x7573B0);
}

// ---------------------------------------------------------------------------------------------------------------------
// Lote R18A (sin validar): plugin de geometria/sector de D3D9 (0x7587D0) y callback de instanciado por defecto del pipeline de atomos de D3D9 (0x7578C0).
// ---------------------------------------------------------------------------------------------------------------------

// 0x7587D0 (lote R18A, sin validar; nombre inferido: pareja de D3D9LightPluginAttach; cdecl sin argumentos, `ret`, 119 bytes; la llama _rpD3D9PluginAttach tras D3D9LightPluginAttach):
// RwEngineRegisterPlugin(0, 0x50F, 0x758890, 0x7588A0) (contador de plugins 0xC926E4, que no se guarda): si es negativo devuelve FALSE. RpGeometryRegisterPlugin(4, 0x50F, 0x758850,
// 0x758860, 0x758870) se guarda SIEMPRE en 0xC926E8 y si es negativo devuelve FALSE; RpWorldSectorRegisterPlugin(4, 0x50F, mismos callbacks) se guarda en 0xC926EC y devuelve
// `resultado >= 0`. Los callbacks (constructor/destructor/copia, que ponen a cero o copian el dword de flags de uso) siguen en el EXE y se pasan por direccion fija.
RwBool D3D9GeometryPluginAttach() {
    if (RwEngineRegisterPlugin(0, 0x50F,
                               reinterpret_cast<RwPluginObjectConstructor>(0x758890),
                               reinterpret_cast<RwPluginObjectDestructor>(0x7588A0)) < 0) {
        return FALSE;
    }
    const RwInt32 geometryOffset = RpGeometryRegisterPlugin(4, 0x50F,
                                                            reinterpret_cast<RwPluginObjectConstructor>(0x758850),
                                                            reinterpret_cast<RwPluginObjectDestructor>(0x758860),
                                                            reinterpret_cast<RwPluginObjectCopy>(0x758870));
    s_geometryUsageFlagsOffset = geometryOffset;
    if (geometryOffset < 0) {
        return FALSE;
    }
    const RwInt32 sectorOffset = RpWorldSectorRegisterPlugin(4, 0x50F,
                                                             reinterpret_cast<RwPluginObjectConstructor>(0x758850),
                                                             reinterpret_cast<RwPluginObjectDestructor>(0x758860),
                                                             reinterpret_cast<RwPluginObjectCopy>(0x758870));
    s_sectorUsageFlagsOffset = sectorOffset;
    return sectorOffset >= 0 ? TRUE : FALSE;
}

namespace {
// D3DVERTEXELEMENT9 (8 bytes). La pila del EXE tiene el arreglo en esp+0x40; D3D nunca devuelve mas de 64 elementos mas el D3DDECL_END.
struct DefaultInstElement {
    RwUInt16 stream;
    RwUInt16 offset;
    RwUInt8  type;
    RwUInt8  method;
    RwUInt8  usage;
    RwUInt8  usageIndex;
};
static_assert(sizeof(DefaultInstElement) == 8);
static_assert(sizeof(RxD3D9ResEntryHeader) == 0x40 && sizeof(RxD3D9VertexStream) == 0x10 && offsetof(RxD3D9ResEntryHeader, vertexDeclaration) == 0x34 && offsetof(RxD3D9ResEntryHeader, totalNumVertex) == 0x3C);
static_assert(offsetof(RpInterpolator, startMorphTarget) == 0x4 && offsetof(RpInterpolator, recipTime) == 0xC && offsetof(RpInterpolator, position) == 0x10);

constexpr uintptr_t kD3D9DevCapsAddress  = 0xC9BF1C; // D3DCAPS9::DevCaps (bit 0x10000 = HWTRANSFORMANDLIGHT)
constexpr uintptr_t kD3D9DevCaps2Address = 0xC9BFD4; // D3DCAPS9::DevCaps2 (bit 1 = STREAMOFFSET)
constexpr uintptr_t kD3D9DeclTypesAddress = 0xC9BFEC; // D3DCAPS9::DeclTypes (0x80 = DEC3N)
constexpr RwUInt32  kDefaultInstElementEnd = 0x11;   // D3DDECLTYPE_UNUSED
constexpr RwUInt32  kDefaultInstLockManaged = 0x800;   // D3DLOCK_NOSYSLOCK
constexpr RwUInt32  kDefaultInstLockDynamic = 0x2800;  // D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD

inline RwUInt32 D3D9CapsDword(uintptr_t address) { return *reinterpret_cast<const volatile RwUInt32*>(address); }
}

// 0x7578C0 (lote R18A, sin validar; nombre inferido: D3D9AtomicDefaultInstanceCallback del SDK; cdecl, 3 argumentos (RpAtomic*, RxD3D9ResEntryHeader*, reinstanciar), 2430 bytes; la llaman
// 0x758270 (callback de reinstanciado por defecto, por direccion) y el nodo AllInOne de atomos como callback de instanciado). Devuelve TRUE o FALSE si no se pudo crear un buffer.
//  1. Sin reinstanciar: totalNumVertex = numVertices de la geometria y libera los dos flujos (gestionado -> RwD3D9DestroyVertexBuffer(stride, totalNumVertex * stride, vb, offset)
//     con el totalNumVertex NUEVO; si no, con offset 0 y sin dynamicLock RwD3D9DynamicVertexBufferDestroy), dejando vb, offset, stride, geometryFlags, managed y dynamicLock a 0.
//     Un solo flujo salvo (si numMorphTargets > 1 o usage & 0xFFF) y DevCaps & 0x10000 y el pipeline del atomo no es el de skin (pluginId 0x12E): entonces 2 flujos (con
//     numMorphTargets > 1 usage |= 6) y _rpD3D9AddDynamicGeometry.
//  2. Arma los D3DVERTEXELEMENT9 (posicion FLOAT3; normales FLOAT3 o DEC3N si DeclTypes & 0x80; color D3DCOLOR; texcoords FLOAT2; tangentes (usage & 0x10000 con texcoords)): cada
//     uno va al flujo 0 si su bit de usage es dinamico (2, 4, 8, 0x10 << n; las tangentes con 0xFF2) o al ultimo flujo si no; Offset = stride actual, que se incrementa; los
//     geometryFlags del flujo reciben el bit de posicion (asignado a 2), normales (4), color (8) y cada texcoord (0x10 << n); las tangentes no marcan geometryFlags. Reduce el numero
//     de flujos a los del ultimo con stride != 0 (minimo 1), ordena los elementos por (flujo, offset) con el qsort del EXE (0x8247E0, comparador 0x758240) y crea la declaracion
//     (RwD3D9CreateVertexDeclaration, 0x7FAA30; el resultado se ignora). Crea los buffers: sin flujos multiples (o flujo 1 con DevCaps2 & 1) gestionados con RwD3D9CreateVertexBuffer
//     (0x7F5500); el resto dinamicos con RwD3D9DynamicVertexBufferCreate (0x7F5A00); un fallo devuelve FALSE. Con mas de un flujo useOffsets = 1 si DevCaps2 & 1 y baseIndex = minVert
//     de cada malla; con uno useOffsets = 0 y baseIndex = offset / stride + minVert. Los bucles por malla son do/while como en el EXE.
//  3. Reinstanciando: GetDeclaration de la declaracion del header (vtable +0x10) y numero de flujos = mayor flujo + 1.
//  4. Bloquea los buffers (Lock vtable +0x2C con offset y stride * totalNumVertex): sin reinstanciar todos con NOSYSLOCK y mascara dinamica 0xFFF; reinstanciando solo los flujos cuyos
//     geometryFlags coinciden con geometria->lockedSinceLastInst (o todos si es 0xFFF), con NOSYSLOCK si estan gestionados o NOSYSLOCK | DISCARD (y entonces la mascara incluye sus
//     geometryFlags) si no.
//  5. Rellena: morph (indices start/end de la interpolacion del atomo, a 0 si alguno >= numMorphTargets; escala = position * recipTime en float) si start != end y escala != 0.0 (la
//     constante 0.0 se lee de 0x858B50 y la comparacion es ordenada: NaN entra): posiciones con _rpD3D9VertexDeclarationInstV3dMorph y, con normales, tambien normales; si no, posiciones
//     (mascara & 2) y normales (normales y mascara & 4) con _rpD3D9VertexDeclarationInstV3d del morph target start; color (flags & 8 y mascara & 8) con
//     _rpD3D9VertexDeclarationInstColor por malla (guarda vertexAlpha; sin color y sin reinstanciar los vertexAlpha se ponen a 0); texcoords (mascara & 0xFF0) con
//     _rpD3D9VertexDeclarationInstV2d; tangentes (usage & 0x10000, mascara & 0xFF2) con _rpD3D9VertexDeclarationInstTangent (posiciones del morph target start y ULTIMO juego de
//     texcoords). Desbloquea (vtable +0x30) los buffers bloqueados y devuelve TRUE.
RwBool D3D9AtomicDefaultInstanceCallback(void* object, RxD3D9ResEntryHeader* header, RwBool reinstance) {
    RpAtomic* const atomic = static_cast<RpAtomic*>(object);
    RpGeometry* const geometry = atomic->geometry;
    RxD3D9VertexStream* const streams = header->vertexStream;
    const RwUInt32 geometryFlags = geometry->flags;
    RwUInt32 usage = static_cast<RwUInt32>(RpD3D9GeometryGetUsageFlags(geometry));
    const RwUInt32 numTexCoordSets = static_cast<RwUInt32>(geometry->numTexCoordSets);
    auto* const instances = reinterpret_cast<RxD3D9InstanceData*>(header + 1);

    DefaultInstElement elements[72];
    RwUInt32 numStreams;

    if (reinstance == 0) {
        header->totalNumVertex = static_cast<RwUInt32>(geometry->numVertices);
        for (RwUInt32 s = 0; s < 2; ++s) {
            RxD3D9VertexStream& stream = streams[s];
            if (stream.vertexBuffer != nullptr) {
                if (stream.managed != 0) {
                    RwD3D9DestroyVertexBuffer(stream.stride, header->totalNumVertex * stream.stride, stream.vertexBuffer, stream.offset);
                } else if (stream.offset == 0 && stream.dynamicLock == 0) {
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

        // Un flujo (los datos estaticos) o dos (datos dinamicos en el flujo 0, el resto en el 1).
        bool multiStream = false;
        const RwInt32 numMorphTargets = static_cast<RwInt32>(geometry->numMorphTargets);
        if ((numMorphTargets > 1 || (usage & 0xFFF) != 0) && (D3D9CapsDword(kD3D9DevCapsAddress) & 0x10000) != 0) {
            const RxPipeline* const pipeline = atomic->pipeline;
            if (pipeline == nullptr || pipeline->pluginId != 0x12E) {
                if (numMorphTargets > 1) {
                    usage |= 6;
                }
                _rpD3D9AddDynamicGeometry(geometry);
                multiStream = true;
            }
        }
        numStreams = multiStream ? 2 : 1;
        const RwUInt32 lastStream = numStreams - 1;
        const RwUInt32 declTypes = D3D9CapsDword(kD3D9DeclTypesAddress);
        const bool dec3n = (declTypes & 0x80) != 0;

        RwUInt32 count = 0;
        RwUInt32 s = (usage & 2) ? 0 : lastStream;
        elements[0] = { static_cast<RwUInt16>(s), static_cast<RwUInt16>(streams[s].stride), 2, 0, 0, 0 };
        streams[s].stride        = 12;
        streams[s].geometryFlags = 2;
        count = 1;
        if (geometryFlags & 0x10) {
            s = (usage & 4) ? 0 : lastStream;
            elements[count] = { static_cast<RwUInt16>(s), static_cast<RwUInt16>(streams[s].stride), static_cast<RwUInt8>(dec3n ? 0x0E : 2), 0, 3, 0 };
            streams[s].stride        += dec3n ? 4 : 12;
            streams[s].geometryFlags |= 4;
            ++count;
        }
        if (geometryFlags & 8) {
            s = (usage & 8) ? 0 : lastStream;
            elements[count] = { static_cast<RwUInt16>(s), static_cast<RwUInt16>(streams[s].stride), 4, 0, 0x0A, 0 };
            streams[s].stride        += 4;
            streams[s].geometryFlags |= 8;
            ++count;
        }
        for (RwUInt32 t = 0; t < numTexCoordSets; ++t) {
            s = (usage & (0x10u << t)) ? 0 : lastStream;
            elements[count] = { static_cast<RwUInt16>(s), static_cast<RwUInt16>(streams[s].stride), 1, 0, 5, static_cast<RwUInt8>(t) };
            streams[s].stride        += 8;
            streams[s].geometryFlags |= static_cast<RwUInt16>(0x10u << t);
            ++count;
        }
        if ((usage & 0x10000) != 0 && numTexCoordSets != 0) {
            s = (usage & 0xFF2) ? 0 : lastStream;
            elements[count] = { static_cast<RwUInt16>(s), static_cast<RwUInt16>(streams[s].stride), static_cast<RwUInt8>(dec3n ? 0x0E : 2), 0, 6, 0 };
            streams[s].stride += dec3n ? 4 : 12;
            ++count;
        }

        if (numStreams > 1) {
            do {
                if (streams[numStreams - 1].stride != 0) {
                    break;
                }
                --numStreams;
            } while (numStreams > 1);
        }
        using CompareFn = int(__cdecl*)(const void*, const void*);
        using QsortFn = void(__cdecl*)(void*, size_t, size_t, CompareFn);
        reinterpret_cast<QsortFn>(0x8247E0)(elements, count, 8, reinterpret_cast<CompareFn>(0x758240));
        elements[count] = { 0xFF, 0, static_cast<RwUInt8>(kDefaultInstElementEnd), 0, 0, 0 };
        RwD3D9CreateVertexDeclaration(elements, reinterpret_cast<void**>(&header->vertexDeclaration));

        for (RwUInt32 i = 0; i < numStreams; ++i) {
            RxD3D9VertexStream& stream = streams[i];
            const RwUInt32 size = header->totalNumVertex * stream.stride;
            bool managed;
            if (!multiStream) {
                managed = true;
            } else if (i == 0) {
                managed = false;
            } else {
                managed = (D3D9CapsDword(kD3D9DevCaps2Address) & 1) != 0;
            }
            RwBool created;
            if (managed) {
                stream.managed = 1;
                created = RwD3D9CreateVertexBuffer(stream.stride, size, &stream.vertexBuffer, &stream.offset);
            } else {
                stream.managed = 0;
                created = RwD3D9DynamicVertexBufferCreate(size, &stream.vertexBuffer);
            }
            if (created == 0) {
                return FALSE;
            }
        }
        if (numStreams > 1) {
            if ((D3D9CapsDword(kD3D9DevCaps2Address) & 1) != 0) {
                header->useOffsets = 1;
            }
            RwUInt32 meshes = header->numMeshes;
            RxD3D9InstanceData* instance = instances;
            do {
                instance->baseIndex = instance->minVert;
                ++instance;
            } while (--meshes != 0);
        } else {
            RwUInt32 meshes = header->numMeshes;
            header->useOffsets = 0;
            RxD3D9InstanceData* instance = instances;
            do {
                instance->baseIndex = streams[0].offset / streams[0].stride + instance->minVert;
                ++instance;
            } while (--meshes != 0);
        }
    } else {
        UINT numElements = 0;
        header->vertexDeclaration->GetDeclaration(reinterpret_cast<D3DVERTEXELEMENT9*>(elements), &numElements);
        numStreams = 0;
        for (RwUInt32 k = 0; elements[k].type != kDefaultInstElementEnd; ++k) {
            if (elements[k].stream >= numStreams) {
                numStreams = static_cast<RwUInt32>(elements[k].stream) + 1;
            }
        }
    }

    // Bloqueo de los buffers de vertices.
    void* locked[RWD3D9_MAX_VERTEX_STREAMS] = { nullptr, nullptr };
    RwUInt32 dynamicMask;
    if (reinstance == 0) {
        dynamicMask = 0xFFF;
        for (RwUInt32 i = 0; i < numStreams; ++i) {
            RxD3D9VertexStream& stream = streams[i];
            static_cast<IDirect3DVertexBuffer9*>(stream.vertexBuffer)->Lock(stream.offset, stream.stride * header->totalNumVertex, &locked[i], kDefaultInstLockManaged);
        }
    } else {
        dynamicMask = static_cast<RwUInt32>(geometry->lockedSinceLastInst);
        for (RwUInt32 i = 0; i < numStreams; ++i) {
            RxD3D9VertexStream& stream = streams[i];
            if (dynamicMask != 0xFFF && (dynamicMask & stream.geometryFlags) == 0) {
                continue;
            }
            const RwUInt32 size = stream.stride * header->totalNumVertex;
            RwUInt32 lockFlags;
            if (stream.managed != 0) {
                lockFlags = kDefaultInstLockManaged;
            } else {
                lockFlags = kDefaultInstLockDynamic;
                dynamicMask |= stream.geometryFlags;
            }
            static_cast<IDirect3DVertexBuffer9*>(stream.vertexBuffer)->Lock(stream.offset, size, &locked[i], lockFlags);
        }
    }

    // Busca el elemento de la declaracion con el uso y el indice de uso pedidos (sin comprobar el final, como el EXE).
    auto findElement = [&](RwUInt32 elementUsage, RwUInt32 elementUsageIndex) -> const DefaultInstElement& {
        RwUInt32 k = 0;
        while (elements[k].usage != elementUsage || elements[k].usageIndex != elementUsageIndex) {
            ++k;
        }
        return elements[k];
    };
    auto elementDestination = [&](const DefaultInstElement& element) -> RwUInt8* {
        return static_cast<RwUInt8*>(locked[element.stream]) + element.offset;
    };

    // Morph targets: la interpolacion del atomo.
    RwInt32 startTarget = 0;
    RwInt32 endTarget   = 0;
    float   morphScale  = 0.0f;
    if (geometry->numMorphTargets != 1) {
        const RwInt32 numMorphTargets = static_cast<RwInt32>(geometry->numMorphTargets);
        startTarget = static_cast<RwInt32>(atomic->interpolator.startMorphTarget);
        endTarget   = static_cast<RwInt32>(atomic->interpolator.endMorphTarget);
        if (!(startTarget < numMorphTargets && endTarget < numMorphTargets)) {
            startTarget = 0;
            endTarget   = 0;
        }
        morphScale = atomic->interpolator.position * atomic->interpolator.recipTime;
    }
    const RpMorphTarget* const morphTargets = geometry->morphTarget;
    const float zero = StaticRef<float>(0x858B50);
    const RwUInt32 numVerts = header->totalNumVertex;

    if (geometry->numMorphTargets != 1 && startTarget != endTarget && !(morphScale == zero)) {
        const DefaultInstElement& position = findElement(0, 0);
        _rpD3D9VertexDeclarationInstV3dMorph(position.type, elementDestination(position), morphTargets[startTarget].verts, morphTargets[endTarget].verts,
                                             morphScale, static_cast<RwInt32>(numVerts), streams[position.stream].stride);
        if (geometryFlags & 0x10) {
            const DefaultInstElement& normal = findElement(3, 0);
            _rpD3D9VertexDeclarationInstV3dMorph(normal.type, elementDestination(normal), morphTargets[startTarget].normals, morphTargets[endTarget].normals,
                                                 morphScale, static_cast<RwInt32>(numVerts), streams[normal.stream].stride);
        }
    } else {
        if (dynamicMask & 2) {
            const DefaultInstElement& position = findElement(0, 0);
            _rpD3D9VertexDeclarationInstV3d(position.type, elementDestination(position), morphTargets[startTarget].verts,
                                            static_cast<RwInt32>(numVerts), streams[position.stream].stride);
        }
        if ((geometryFlags & 0x10) && (dynamicMask & 4)) {
            const DefaultInstElement& normal = findElement(3, 0);
            _rpD3D9VertexDeclarationInstV3d(normal.type, elementDestination(normal), morphTargets[startTarget].normals,
                                            static_cast<RwInt32>(numVerts), streams[normal.stream].stride);
        }
    }

    // Color de vertices.
    if (geometryFlags & 8) {
        if (dynamicMask & 8) {
            const DefaultInstElement& color = findElement(0x0A, 0);
            const RwUInt32 stride = streams[color.stream].stride;
            RxD3D9InstanceData* instance = instances;
            RwUInt32 meshes = header->numMeshes;
            do {
                RwUInt8* const destination = static_cast<RwUInt8*>(locked[color.stream]) + instance->minVert * stride + color.offset;
                instance->vertexAlpha = _rpD3D9VertexDeclarationInstColor(destination, geometry->preLitLum + instance->minVert,
                                                                          static_cast<RwInt32>(instance->numVertices), stride);
                ++instance;
            } while (--meshes != 0);
        }
    } else if (reinstance == 0) {
        RwUInt32 meshes = header->numMeshes;
        RxD3D9InstanceData* instance = instances;
        do {
            instance->vertexAlpha = 0;
            ++instance;
        } while (--meshes != 0);
    }

    // Coordenadas de textura.
    if (numTexCoordSets != 0 && (dynamicMask & 0xFF0) != 0) {
        for (RwUInt32 t = 0; t < numTexCoordSets; ++t) {
            if (dynamicMask & (0x10u << t)) {
                const DefaultInstElement& texCoord = findElement(5, t);
                _rpD3D9VertexDeclarationInstV2d(texCoord.type, elementDestination(texCoord), reinterpret_cast<const RwV2d*>(geometry->texCoords[t]),
                                                static_cast<RwInt32>(numVerts), streams[texCoord.stream].stride);
            }
        }
    }

    // Tangentes.
    if ((usage & 0x10000) != 0 && numTexCoordSets != 0 && (dynamicMask & 0xFF2) != 0) {
        const DefaultInstElement& tangent = findElement(6, 0);
        _rpD3D9VertexDeclarationInstTangent(tangent.type, elementDestination(tangent), morphTargets[startTarget].verts, geometry->texCoords[numTexCoordSets - 1],
                                            header, streams[tangent.stream].stride);
    }

    // Desbloqueo.
    for (RwUInt32 i = 0; i < numStreams; ++i) {
        if (locked[i] != nullptr) {
            static_cast<IDirect3DVertexBuffer9*>(streams[i].vertexBuffer)->Unlock();
        }
    }
    return TRUE;
}

// Registro de los hooks del lote R18A de RpWorld (sin validar): plugin de geometria/sector de D3D9, callback de instanciado de atomos, mallas (RpBuildMesh, RpMeshHeader) y
// accesores de definicion de nodo, flags de uso y render del nodo AllInOne.
void RpWorldInjectHooksR18A() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpWorld");

    RH_ScopedGlobalInstall(RxD3D9AllInOneSetRenderCallBack, 0x7573E0);
    RH_ScopedGlobalInstall(D3D9AtomicDefaultInstanceCallback, 0x7578C0);
    RH_ScopedGlobalInstall(RxNodeDefinitionGetD3D9AtomicAllInOne, 0x7582E0);
    RH_ScopedGlobalInstall(D3D9GeometryPluginAttach, 0x7587D0);
    RH_ScopedGlobalInstall(RpD3D9GeometrySetUsageFlags, 0x7588B0);
    RH_ScopedGlobalInstall(_rpBuildMeshCreate, 0x758A90);
    RH_ScopedGlobalInstall(_rpBuildMeshDestroy, 0x758B80);
    RH_ScopedGlobalInstall(_rpMeshDestroy, 0x758BC0);
    RH_ScopedGlobalInstall(_rpBuildMeshAddTriangle, 0x758C00);
    RH_ScopedGlobalInstall(_rpMeshHeaderForAllMeshes, 0x758D30);
    RH_ScopedGlobalInstall(_rpMeshWrite, 0x758D70);
    RH_ScopedGlobalInstall(_rpMeshRead, 0x758EC0);
    RH_ScopedGlobalInstall(_rpMeshSize, 0x759090);
}

void RpWorldInjectHooksR19A() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpWorld");

    RH_ScopedGlobalInstall(_rpBuildMeshGenerateTriStrip, 0x7591D0);
    RH_ScopedGlobalInstall(_rpBuildStripListGenerate, 0x759790);
    RH_ScopedGlobalInstall(_rpBuildStripNodesCreate, 0x75A090);
    RH_ScopedGlobalInstall(_rpBuildStripEdgeFind, 0x75A1B0);
    RH_ScopedGlobalInstall(_rpBuildStripNodeConsume, 0x75A250);
    RH_ScopedGlobalInstall(_rpBuildStripExtend, 0x75A370);
}

// ---------------------------------------------------------------------------------------------------------------------
// Lote R20A (sin validar): union de tiras de RpBuildMesh, generador de listas de triangulos, optimizador de mallas, cache de
// vertex shaders de D3D9 (apertura, purga, matrices de entorno y relieve), tuberias por defecto del modulo de mundos, callbacks de
// flujo del plugin 0x1F de RpWorld/RpWorldSector y flujo nativo (D3D9) de las geometrias. Todo el codigo sigue el desensamblado del
// EXE instruccion por instruccion; no hay banco: el coordinador valida despues.
// ---------------------------------------------------------------------------------------------------------------------
namespace {
// Estado de la cache de vertex shaders de D3D9 (.data/.bss del EXE).
struct D3D9VertexShaderCacheEntry {  // 0x14 bytes; la entrada 0 (0xC926F8) es tambien la "entrada vacia" con todos los campos a -1
    RwUInt32 key0;                   // +0x00
    RwUInt32 key1;                   // +0x04
    void*    shader;                 // +0x08 (IDirect3DVertexShader9*, se libera con RwD3D9DeleteVertexShader)
    RwUInt32 field0C;                // +0x0C
    RwUInt32 field10;                // +0x10
};
static_assert(sizeof(D3D9VertexShaderCacheEntry) == 0x14);
inline D3D9VertexShaderCacheEntry* VertexShaderCacheEntries() {
    return reinterpret_cast<D3D9VertexShaderCacheEntry*>(0xC926F8); // la tabla continua en memoria mas alla de la entrada 0 (se indexa con el recuento)
}
static auto& s_vsCacheCount       = StaticRef<RwInt32>(0xC94BF8);                              // numero de entradas ocupadas
static auto& s_vsCacheLast        = StaticRef<D3D9VertexShaderCacheEntry*>(0xC94BFC);          // ultima entrada usada (0xC926F8 tras abrir o purgar)
static auto& s_vsConstantTable    = StaticRef<RwUInt32[64]>(0xC94AF8);                         // 0x100 bytes: contadores de constantes (acaba justo antes del recuento)
static auto& s_vsParamA           = StaticRef<RwUInt32>(0xC94C00);                             // parametro derivado de las capacidades del dispositivo
static auto& s_vsParamB           = StaticRef<RwUInt32>(0xC94C04);                             // MaxVertexShaderConst acotado a 0x100
static auto& s_worldRightsWord0   = StaticRef<RwInt32>(0xC94C18);                              // plugin 0x1F de RpWorld: primera palabra leida
static auto& s_worldRightsWord1   = StaticRef<RwInt32>(0xC94C14);                              // plugin 0x1F de RpWorld: segunda palabra (longitud 8)
static auto& s_sectorRightsWord0  = StaticRef<RwInt32>(0xC94C1C);                              // plugin 0x1F de RpWorldSector: primera palabra
static auto& s_sectorRightsWord1  = StaticRef<RwInt32>(0xC94C10);                              // plugin 0x1F de RpWorldSector: segunda palabra (longitud 8)
static auto& s_meshGeneratorCallBack = StaticRef<RpTriStripMeshCallBack>(0x8D6388);            // generador de mallas elegido con RpMeshSetTriStripMethod
static auto& s_meshGeneratorData     = StaticRef<void*>(0x8D638C);                             // sus datos
static auto& s_emptyMeshHeader       = StaticRef<RpMeshHeader>(0x8D6378);                      // cabecera de malla vacia estatica

constexpr uintptr_t kD3D9VertexShaderVersionAddress = 0xC9BFC4;     // D3DCAPS9::VertexShaderVersion
constexpr uintptr_t kD3D9MaxVertexShaderConstAddress = 0xC9BFC8;    // D3DCAPS9::MaxVertexShaderConst
constexpr uintptr_t kD3D9DynamicFlowControlDepthAddress = 0xC9BFFC; // D3DCAPS9::VS20Caps.DynamicFlowControlDepth
constexpr uintptr_t kEnvMapTransformMatrixAddress = 0x875250;       // RwMatrix estatica de .rdata usada por el efecto de entorno
constexpr uint32    kPipelineSlotDefaultAtomic = 0x3C;              // hueco de la tuberia de atomos por defecto
constexpr RwUInt32  kNativeGeometryVersion = 0x36003;
constexpr RwUInt32  kNativeGeometryFlag    = 0x1000000;             // rpGEOMETRYNATIVE
constexpr RwUInt32  kNativeGeometryPlatform = 9;                    // rwID_PLATFORM_D3D9
constexpr uintptr_t kNativePlatformIdAddress = 0x8D7384;            // dword 9 en .data (el EXE escribe sus 4 bytes)
constexpr RwUInt32  kNativeGeometryResHint = 0x3050D;
constexpr uintptr_t kResEntryHeaderDestroyAddress = 0x4C9990;       // RxD3D9ResEntryHeaderDestroy (recibe la RwResEntry)
constexpr RwUInt32  kMaxNativeDeclElements = 65;                    // D3D9 admite hasta 64 elementos mas el final

inline void SetVertexShaderConstantsExe(RwUInt32 reg, const void* data, RwUInt32 count) {
    _RwD3DDevice->SetVertexShaderConstantF(reg, static_cast<const float*>(data), count);
}

inline RwFrame* CurrentCameraFrame() {
    return static_cast<RwFrame*>(rwObjectGetParent(static_cast<RwGlobals*>(RwEngineInstance)->curCamera));
}

inline void DestroyResEntryExe(RwResEntry* entry) {
    reinterpret_cast<void(__cdecl*)(RwResEntry*)>(kResEntryHeaderDestroyAddress)(entry);
}

// Cabecera de recursos D3D9 que cuelga de una RwResEntry (+0x18) y sus instancias de malla (+0x40 de la cabecera, 0x24 bytes cada una).
inline RxD3D9ResEntryHeader* ResEntryHeader(RwResEntry* entry) {
    return reinterpret_cast<RxD3D9ResEntryHeader*>(reinterpret_cast<uint8_t*>(entry) + 0x18);
}
inline RxD3D9InstanceData* ResEntryInstances(RxD3D9ResEntryHeader* header) {
    return reinterpret_cast<RxD3D9InstanceData*>(header + 1);
}
}

// 0x75A660 (lote R20A, sin validar; nombre inferido; cdecl, 2 argumentos (RpBuildStripList** lista, modo de union), 627 bytes; llamador 0x7591D0): une las tiras de la lista en UNA sola.
// Sin lista (*list == NULL) devuelve FALSE. Si no: crea la tira resultado (lista libre 0xC9B8C0 + 4, pista 0x30502), numIndices 0 y capacidad = suma de (numIndices + 6) de todas;
// buffer de indices (capacidad * 4, pista 0x30502). Copia la primera tira, libera sus indices y la devuelve a la lista libre. Mientras queden tiras (`rest`): busca la primera cuyo PRIMER
// indice sea el ULTIMO de la resultante (busqueda 1; si numIndices es impar y modoUnion != 0 anade antes ese indice repetido) y, si no hay, la primera cuyo primer indice sea el
// PENULTIMO (busqueda 2; con numIndices par y modoUnion != 0 anade dos veces su primer indice, si no una vez). Sin ninguna coincidencia toma la primera de la lista: repite el ultimo
// indice y anade su primer indice (dos veces si numIndices es impar y modoUnion != 0, si no una). Despues copia sus indices, libera su buffer y la saca de la lista simple (cabeza o
// enlace anterior) a la lista libre. Al final *list = resultado, resultado->next = NULL y devuelve TRUE.
RwBool _rpBuildStripListJoin(RpBuildStripList** list, RwInt32 joinMode) {
    if (*list == nullptr) {
        return FALSE;
    }
    RpBuildStripList* const result = static_cast<RpBuildStripList*>(RwFreeListAlloc(MeshStripFreeListSlot(), kStripNodeHint));
    result->numIndices = 0;
    result->capacity   = 0;
    for (RpBuildStripList* strip = *list; strip != nullptr; strip = strip->next) {
        result->capacity += strip->numIndices + 6;
    }
    result->indices = static_cast<RwUInt16*>(RwMalloc(result->capacity * 4, kStripNodeHint));

    RpBuildStripList* const first = *list;
    for (RwUInt32 i = 0; i < first->numIndices; ++i) {
        result->indices[result->numIndices] = first->indices[i];
        ++result->numIndices;
    }
    RwFree(first->indices);
    RpBuildStripList* rest = first->next;
    first->indices = nullptr;
    RwFreeListFree(MeshStripFreeListSlot(), first);

    while (rest != nullptr) {
        RwUInt16* const indices = result->indices;
        const RwUInt32 count = result->numIndices;
        const RwUInt16 last = indices[count - 1];
        RpBuildStripList* candidate = nullptr;
        for (RpBuildStripList* strip = rest; strip != nullptr; strip = strip->next) {
            if (last == strip->indices[0]) {
                candidate = strip;
                break;
            }
        }
        if (candidate != nullptr) {
            if ((count & 1) != 0 && joinMode != 0) {
                indices[count] = candidate->indices[0];
                result->numIndices = count + 1;
            }
        } else {
            const RwUInt16 previous = indices[count - 2];
            for (RpBuildStripList* strip = rest; strip != nullptr; strip = strip->next) {
                if (previous == strip->indices[0]) {
                    candidate = strip;
                    break;
                }
            }
            if (candidate != nullptr) {
                if ((count & 1) == 0 && joinMode != 0) {
                    indices[count]     = candidate->indices[0];
                    indices[count + 1] = candidate->indices[0];
                    result->numIndices = count + 2;
                } else {
                    indices[count]     = candidate->indices[0];
                    result->numIndices = count + 1;
                }
            } else {
                candidate = rest;
                if ((count & 1) != 0 && joinMode != 0) {
                    indices[count]     = indices[count - 1];
                    indices[count + 1] = candidate->indices[0];
                    indices[count + 2] = candidate->indices[0];
                    result->numIndices = count + 3;
                } else {
                    indices[count]     = indices[count - 1];
                    indices[count + 1] = candidate->indices[0];
                    result->numIndices = count + 2;
                }
            }
        }

        for (RwUInt32 i = 0; i < candidate->numIndices; ++i) {
            result->indices[result->numIndices] = candidate->indices[i];
            ++result->numIndices;
        }
        RwFree(candidate->indices);
        candidate->indices = nullptr;
        if (rest == candidate) {
            rest = rest->next;
        } else {
            RpBuildStripList* previousLink = rest;
            while (previousLink->next != candidate) {
                previousLink = previousLink->next;
            }
            previousLink->next = candidate->next;
        }
        RwFreeListFree(MeshStripFreeListSlot(), candidate);
    }
    *list = result;
    result->next = nullptr;
    return TRUE;
}

// 0x75D530 (lote R20A, sin validar; sustituye la envoltura; nombre del SDK _rpTriListMeshGenerate; cdecl, 2 argumentos (RpBuildMesh, datos sin usar), 497 bytes): genera una RpMeshHeader de
// LISTAS de triangulos (flags 0). Vector de punteros a los triangulos (RwMalloc numTriangulos * 4, pista 0x10502; NULL -> error 0x80000013 con numTriangulos * 4 y NULL), ordenado con el
// qsort del EXE (0x8247E0) y el comparador 0x759640 (por direccion fija). Cuenta los grupos de material consecutivos y reserva la cabecera (6 * (n + 2 * grupos) + 0x10 bytes,
// _rpMeshHeaderCreate; NULL -> libera el vector, error 0x80000013 con el tamano y NULL). Cabecera: flags 0, numMeshes 1 (de 16 bits, crece con cada grupo), serialNum = contador de serie
// global (que se incrementa), totalIndicesInMesh = 3n, firstMeshOffset 0. Las RpMesh (12 bytes) van desde +0x10 y los indices (3 de 16 bits por triangulo) tras las `grupos` mallas; el
// bucle es do/while como en el EXE (lee ptrs[0] aunque n sea 0). Al final recorre las mallas con _rpMeshHeaderForAllMeshes(cabecera, 0x75D730, 0) (callback sin reconstruir), libera el vector
// y devuelve la cabecera.
RpMeshHeader* _rpTriListMeshGenerate(RpBuildMesh* buildMesh, void* data) {
    (void)data;
    RpBuildMeshTriangle** const pointers = static_cast<RpBuildMeshTriangle**>(RwMalloc(buildMesh->numTriangles * 4, kStripListHint));
    if (pointers == nullptr) {
        RaiseRwError(kErrorNoMemory, buildMesh->numTriangles * 4);
        return nullptr;
    }
    for (RwUInt32 i = 0; i < buildMesh->numTriangles; ++i) {
        pointers[i] = reinterpret_cast<RpBuildMeshTriangle*>(reinterpret_cast<RwUInt8*>(buildMesh->meshTriangles) + i * 0x14);
    }
    using CompareFn = int(__cdecl*)(const void*, const void*);
    using QsortFn = void(__cdecl*)(void*, size_t, size_t, CompareFn);
    reinterpret_cast<QsortFn>(0x8247E0)(pointers, buildMesh->numTriangles, 4, reinterpret_cast<CompareFn>(0x759640));

    RwUInt32 numGroups = 1;
    if (buildMesh->numTriangles >= 2) {
        RpMaterial* current = pointers[0]->material;
        for (RwUInt32 i = 1; i < buildMesh->numTriangles; ++i) {
            RpMaterial* const material = pointers[i]->material;
            if (material != current) {
                current = material;
                ++numGroups;
            }
        }
    }

    const RwUInt32 size = (buildMesh->numTriangles + numGroups * 2) * 6 + 0x10;
    RpMeshHeader* const header = _rpMeshHeaderCreate(size);
    if (header == nullptr) {
        RwFree(pointers);
        RaiseRwError(kErrorNoMemory, size);
        return nullptr;
    }
    header->flags     = 0;
    header->numMeshes = 1;
    RwUInt16& serial = *reinterpret_cast<RwUInt16*>(reinterpret_cast<uintptr_t>(static_cast<RwGlobals*>(RwEngineInstance)) + static_cast<uintptr_t>(s_meshSerialOffset));
    header->firstMeshOffset = 0;
    header->serialNum       = serial;
    header->totalIndicesInMesh = buildMesh->numTriangles * 3;
    serial = static_cast<RwUInt16>(serial + 1);

    RpMesh* mesh = reinterpret_cast<RpMesh*>(reinterpret_cast<RwUInt8*>(header) + 0x10);
    RxVertexIndex* cursor = reinterpret_cast<RxVertexIndex*>(reinterpret_cast<RwUInt8*>(mesh) + numGroups * 12);
    mesh->numIndices = 0;
    mesh->indices    = cursor;
    mesh->material   = pointers[0]->material;
    RwUInt32 triangleIndex = 0;
    do {
        if (pointers[triangleIndex]->material != mesh->material) {
            ++mesh;
            mesh->numIndices = 0;
            mesh->indices    = cursor;
            mesh->material   = pointers[triangleIndex]->material;
            ++header->numMeshes;
        }
        cursor[0] = pointers[triangleIndex]->vertIndex[0];
        cursor[1] = pointers[triangleIndex]->vertIndex[1];
        cursor[2] = pointers[triangleIndex]->vertIndex[2];
        cursor += 3;
        mesh->numIndices += 3;
        ++triangleIndex;
    } while (triangleIndex < buildMesh->numTriangles);

    _rpMeshHeaderForAllMeshes(header, reinterpret_cast<RpMeshCallBack>(0x75D730), nullptr);
    RwFree(pointers);
    return header;
}

// 0x75D970 (lote R20A, sin validar; sustituye la envoltura; nombre del SDK _rpMeshOptimise; cdecl, 2 argumentos (RpBuildMesh, flags), 93 bytes): sin malla devuelve NULL. Con 0 triangulos destruye
// la RpBuildMesh (_rpBuildMeshDestroy, 0x758B80) y devuelve la direccion de la cabecera vacia estatica 0x8D6378. Si no, elige el generador: con flags & 1 (rpGEOMETRYTRISTRIP) el callback
// global 0x8D6388 con sus datos 0x8D638C; si no, _rpTriListMeshGenerate (0x75D530) con datos NULL. Si el generador devuelve NULL devuelve NULL SIN destruir la RpBuildMesh; si no, la
// destruye y devuelve la cabecera.
RpMeshHeader* _rpMeshOptimise(RpBuildMesh* buildmesh, RwUInt32 flags) {
    if (buildmesh == nullptr) {
        return nullptr;
    }
    if (buildmesh->numTriangles == 0) {
        _rpBuildMeshDestroy(buildmesh);
        return &s_emptyMeshHeader;
    }
    RpTriStripMeshCallBack generator;
    void* generatorData;
    if ((flags & 1) != 0) {
        generator     = s_meshGeneratorCallBack;
        generatorData = s_meshGeneratorData;
    } else {
        generator     = _rpTriListMeshGenerate;
        generatorData = nullptr;
    }
    RpMeshHeader* const header = generator(buildmesh, generatorData);
    if (header == nullptr) {
        return nullptr;
    }
    _rpBuildMeshDestroy(buildmesh);
    return header;
}

// 0x75E270 (lote R20A, sin validar; sustituye la envoltura; cdecl, 1 argumento, `ret`, 46 bytes): fija la tuberia de sectores por defecto del modulo de tuberias (hueco 0x40): con NULL la deja
// igual a la del hueco 0x58 (la de D3D9). Devuelve (en EAX) el valor guardado: la tuberia recibida, o la del hueco 0x58 con NULL.
RxPipeline* RpWorldSetDefaultSectorPipeline(RxPipeline* pipeline) {
    if (pipeline == nullptr) {
        pipeline = PipelineModuleSlot(kPipelineSlotPlatformSector);
        PipelineModuleSlot(kPipelineSlotDefaultSector) = pipeline;
        return pipeline;
    }
    PipelineModuleSlot(kPipelineSlotDefaultSector) = pipeline;
    return pipeline;
}

// 0x75E2A0 (lote R20A, sin validar; sustituye la envoltura; cdecl, 1 argumento, `ret`, 46 bytes): igual para la tuberia de atomos por defecto (hueco 0x3C) con la de D3D9 del hueco 0x54.
RxPipeline* RpAtomicSetDefaultPipeline(RxPipeline* pipeline) {
    if (pipeline == nullptr) {
        pipeline = PipelineModuleSlot(kPipelineSlotPlatformAtomic);
        PipelineModuleSlot(kPipelineSlotDefaultAtomic) = pipeline;
        return pipeline;
    }
    PipelineModuleSlot(kPipelineSlotDefaultAtomic) = pipeline;
    return pipeline;
}

// 0x75E300 (lote R20A, sin validar; nombre inferido; cdecl, sin argumentos, 15 bytes): cierre de las tuberias del modulo: cierra la de sectores (0x74EB40), la de atomos (0x74EC00) y,
// con salto final, la de materiales (0x74EAC0).
void _rpWorldPipelinesClose() {
    _rpD3D9WorldSectorPipelineClose();
    _rpD3D9AtomicPipelineClose();
    _rpD3D9MaterialPipelineClose();
}

// 0x75E310 (lote R20A, sin validar; nombre inferido; cdecl, sin argumentos, 194 bytes): apertura de las tuberias del modulo. Pone a NULL los nueve huecos 0x3C..0x5C del modulo (cada uno
// relee el desplazamiento y RwEngineInstance), abre materiales (0x74EAB0), atomos (0x74EB80) y sectores (0x74EAD0) en ese orden y devuelve TRUE; si alguna falla cierra sectores, atomos y
// materiales (0x74EB40, 0x74EC00, 0x74EAC0) y devuelve FALSE.
RwBool _rpWorldPipelinesOpen() {
    for (uint32 offset = kPipelineSlotDefaultAtomic; offset <= 0x5C; offset += 4) {
        PipelineModuleSlot(offset) = nullptr;
    }
    if (_rpD3D9MaterialPipelineOpen() && _rpD3D9AtomicPipelineOpen() && _rpD3D9WorldSectorPipelineOpen()) {
        return TRUE;
    }
    _rpD3D9WorldSectorPipelineClose();
    _rpD3D9AtomicPipelineClose();
    _rpD3D9MaterialPipelineClose();
    return FALSE;
}

// 0x75E3E0 (lote R20A, sin validar; nombre inferido; cdecl, sin argumentos, 6 bytes): `mov eax, 1; ret`. Lo llama _rpTieMeshPluginAttach (0x750050) como ultima etapa.
RwBool _rpWorldPipelinesAttach() {
    return TRUE;
}

// 0x75EE60 (lote R20A, sin validar; sustituye la envoltura; nombre del SDK _rpD3D9VertexShaderCachePurge; cdecl, sin argumentos, 103 bytes): libera los vertex shaders de la cache (de la entrada
// 0 a recuento - 1: el puntero +8 de cada entrada de 0x14 bytes, si no es NULL, con RwD3D9DeleteVertexShader 0x7FAC90 y lo pone a 0; el recuento se relee en cada vuelta), pone el recuento a 0,
// la ultima entrada usada a la entrada 0 (0xC926F8) y los cinco campos de la entrada 0 a -1.
void _rpD3D9VertexShaderCachePurge() {
    D3D9VertexShaderCacheEntry* const entries = VertexShaderCacheEntries();
    if (s_vsCacheCount > 0) {
        RwInt32 index = 0;
        do {
            if (entries[index].shader != nullptr) {
                RwD3D9DeleteVertexShader(entries[index].shader);
                entries[index].shader = nullptr;
            }
            ++index;
        } while (index < s_vsCacheCount);
    }
    s_vsCacheCount = 0;
    entries[0].key0 = 0xFFFFFFFF;
    s_vsCacheLast = &entries[0];
    entries[0].key1    = 0xFFFFFFFF;
    entries[0].shader  = reinterpret_cast<void*>(0xFFFFFFFFu);
    entries[0].field0C = 0xFFFFFFFF;
    entries[0].field10 = 0xFFFFFFFF;
}

// 0x760CF0 (lote R20A, sin validar; nombre inferido; cdecl, sin argumentos, 246 bytes; la llama _rpD3D9AtomicPipelineOpen 0x74EB80): inicializa la cache de vertex shaders. Entrada 0 (0xC926F8..
// 0xC92708) a -1, recuento 0, ultima entrada = entrada 0, tabla de 0x100 bytes (0xC94AF8) a 0 y sus palabras 1, 2, 6, 7 y 9 a 2, palabras 3 y 10 a 4, palabra 11 a 6.
// Parametros segun D3DCAPS9::VertexShaderVersion & 0xFFFF (C9BFC4): >= 0x300: 0xC94C00 = 0x19; >= 0x200: 0x19 si DynamicFlowControlDepth (C9BFFC) > 0 (con signo), si no 0x14; >= 0x100:
// 0xA y el byte bajo de DeclTypes (C9BFEC) se enmascara con 0xC1; menos: 0xC94C00 = 0 y termina SIN tocar 0xC94C04. En los tres primeros casos 0xC94C04 = MaxVertexShaderConst (C9BFC8) acotado a 0x100 (sin signo).
void _rpD3D9VertexShaderCacheOpen() {
    D3D9VertexShaderCacheEntry* const entries = VertexShaderCacheEntries();
    entries[0].key0 = 0xFFFFFFFF;
    entries[0].key1 = 0xFFFFFFFF;
    entries[0].shader  = reinterpret_cast<void*>(0xFFFFFFFFu);
    entries[0].field0C = 0xFFFFFFFF;
    s_vsCacheCount = 0;
    entries[0].field10 = 0xFFFFFFFF;
    RwUInt32* const table = s_vsConstantTable;
    for (RwUInt32 i = 0; i < 64; ++i) {
        table[i] = 0;
    }
    table[1]  = 2;
    table[2]  = 2;
    table[6]  = 2;
    table[7]  = 2;
    table[9]  = 2;
    const RwUInt32 shaderVersion = D3D9CapsDword(kD3D9VertexShaderVersionAddress) & 0xFFFF;
    s_vsCacheLast = &entries[0];
    table[3]  = 4;
    table[10] = 4;
    table[11] = 6;
    if (shaderVersion >= 0x300) {
        s_vsParamA = 0x19;
    } else if (shaderVersion >= 0x200) {
        s_vsParamA = static_cast<RwInt32>(D3D9CapsDword(kD3D9DynamicFlowControlDepthAddress)) <= 0 ? 0x14u : 0x19u;
    } else if (shaderVersion >= 0x100) {
        s_vsParamA = 0xA;
        RwUInt32& declTypes = *reinterpret_cast<RwUInt32*>(kD3D9DeclTypesAddress);
        declTypes = (declTypes & 0xFFFFFF00u) | (declTypes & 0xC1u);
    } else {
        s_vsParamA = 0;
        return;
    }
    s_vsParamB = D3D9CapsDword(kD3D9MaxVertexShaderConstAddress);
    if (s_vsParamB >= 0x100) {
        s_vsParamB = 0x100;
    }
}

// 0x761A10 (lote R20A, sin validar; sustituye la envoltura; nombre del SDK _rpD3D9VertexShaderSetEnvMatrix; cdecl, 3 argumentos (marco, descriptor, despacho), `ret`, 349 bytes): sube las
// constantes del efecto de entorno al dispositivo (IDirect3DDevice9::SetVertexShaderConstantF, ranura 0x178). effect = descriptor->effect (byte 3 >> 2). Con effect distinto de 2 y 3: marco
// de la camara actual (curCamera->parent), posicion de su LTM (+0x30) llevada al espacio local (0x764E70) en un vector de 3 floats y matriz 4x4 de 0x764C60 inmediatamente DESPUES del
// vector (la constante de 1 registro lee 16 bytes, o sea el primer float de la matriz); effect 0xA: registro offsetEffect (1) y offsetEffect + 1 (3 filas); otros: offsetEffect + 2 (1) y
// offsetEffect + 3 (3). Con effect 2 o 3: con marco, LTM -> RwMatrixInvert -> RwMatrixMultiply con la matriz estatica 0x875250 -> 0x764A70; sin marco, 0x764B50 con 0x875250. Pone 0.5f en
// los floats 3 y 7 de la matriz de salida y sube 2 registros desde offsetEffect (effect 2) u offsetEffect + 2 (effect 3).
void _rpD3D9VertexShaderSetEnvMatrix(RwFrame* frame, const _rpD3D9VertexShaderDescriptor* desc, const _rpD3D9VertexShaderDispatchDescriptor* dispatch) {
    const RwUInt32 effect = desc->effect;
    if (effect != 2 && effect != 3) {
        struct { float position[3]; float matrix[16]; } locals;
        const RwMatrix* const cameraLtm = RwFrameGetLTM(CurrentCameraFrame());
        _rwD3D9VSGetPointInLocalSpace(&cameraLtm->pos, reinterpret_cast<RwV3d*>(locals.position));
        _rwD3D9VSGetWorldNormalizedTransposeMatrix(locals.matrix);
        if (effect == 0xA) {
            SetVertexShaderConstantsExe(dispatch->offsetEffect, locals.position, 1);
            SetVertexShaderConstantsExe(static_cast<RwUInt32>(dispatch->offsetEffect) + 1, locals.matrix, 3);
        } else {
            SetVertexShaderConstantsExe(static_cast<RwUInt32>(dispatch->offsetEffect) + 2, locals.position, 1);
            SetVertexShaderConstantsExe(static_cast<RwUInt32>(dispatch->offsetEffect) + 3, locals.matrix, 3);
        }
        return;
    }
    RwMatrix inverse;
    RwMatrix product;
    float matrix[16];
    if (frame != nullptr) {
        RwMatrixInvert(&inverse, RwFrameGetLTM(frame));
        RwMatrixMultiply(&product, &inverse, reinterpret_cast<const RwMatrix*>(kEnvMapTransformMatrixAddress));
        _rwD3D9VSGetWorldNormalizedMultiplyTransposeMatrix(matrix, &product);
    } else {
        _rwD3D9VSGetWorldNormalizedViewMultiplyTransposeMatrix(matrix, reinterpret_cast<const RwMatrix*>(kEnvMapTransformMatrixAddress));
    }
    matrix[3] = 0.5f;
    matrix[7] = 0.5f;
    RwUInt32 reg = dispatch->offsetEffect;
    if (effect != 2) {
        reg += 2;
    }
    SetVertexShaderConstantsExe(reg, matrix, 2);
}

// 0x761B70 (lote R20A, sin validar; sustituye la envoltura; nombre del SDK _rpD3D9VertexShaderSetBumpMatrix; cdecl, 3 argumentos (marco, factor float, despacho), `ret`, 182 bytes): sin
// marco usa el de la camara actual. RwMatrixInvert(LTM) -> 0x764960 (matriz 4x4 de salida); los floats 0, 1, 2, 4, 5 y 6 se multiplican por el factor (una `fmul` + `fstp` en la x87 a 24 bits:
// un producto float) y los 3 y 7 se ponen a 0. Sube 2 registros desde dispatch->offsetEffect.
void _rpD3D9VertexShaderSetBumpMatrix(RwFrame* frame, RwReal factor, const _rpD3D9VertexShaderDispatchDescriptor* dispatch) {
    if (frame == nullptr) {
        frame = CurrentCameraFrame();
    }
    RwMatrix inverse;
    float matrix[16];
    RwMatrixInvert(&inverse, RwFrameGetLTM(frame));
    _rwD3D9VSGetWorldMultiplyTransposeMatrix(matrix, &inverse);
    matrix[3] = 0.0f;
    matrix[7] = 0.0f;
    matrix[0] = matrix[0] * factor;
    matrix[1] = matrix[1] * factor;
    matrix[2] = matrix[2] * factor;
    matrix[4] = matrix[4] * factor;
    matrix[5] = matrix[5] * factor;
    matrix[6] = matrix[6] * factor;
    SetVertexShaderConstantsExe(dispatch->offsetEffect, matrix, 2);
}

// 0x761D60 (lote R20A, sin validar; nombre inferido; cdecl, callback de lectura del plugin 0x1F de RpWorld, `ret`, 60 bytes): RwStreamReadInt32 de 4 bytes a 0xC94C18 y, si la longitud del
// chunk es exactamente 8, otros 4 a 0xC94C14. Devuelve el flujo, o NULL si una lectura falla. No usa object, offsetInObject ni sizeInObject.
RwStream* _rpWorldRightsStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    if (!RwStreamReadInt32(stream, &s_worldRightsWord0, 4)) {
        return nullptr;
    }
    if (binaryLength == 8 && !RwStreamReadInt32(stream, &s_worldRightsWord1, 4)) {
        return nullptr;
    }
    return stream;
}

// 0x761DA0 (lote R20A, sin validar; nombre inferido; callback de escritura del mismo plugin, 62 bytes): escribe pluginId (+0x2C) y pluginData (+0x30) de la tuberia del mundo (`object + 0x6C`,
// sin comprobar nulo) con RwStreamWriteInt32 (4 bytes cada uno). Primera escritura NULL -> NULL; si no, devuelve el flujo si la segunda no es NULL (neg/sbb/and), si no NULL.
RwStream* _rpWorldRightsStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    const RxPipeline* const pipeline = static_cast<const RpWorld*>(object)->pipeline;
    if (!RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&pipeline->pluginId), 4)) {
        return nullptr;
    }
    return RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&pipeline->pluginData), 4) ? stream : nullptr;
}

// 0x761E00 (lote R20A, sin validar; nombre inferido; cdecl, callback de lectura del plugin 0x1F de RpWorldSector, 60 bytes): como 0x761D60 con los destinos 0xC94C1C y (longitud 8) 0xC94C10.
RwStream* _rpWorldSectorRightsStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    if (!RwStreamReadInt32(stream, &s_sectorRightsWord0, 4)) {
        return nullptr;
    }
    if (binaryLength == 8 && !RwStreamReadInt32(stream, &s_sectorRightsWord1, 4)) {
        return nullptr;
    }
    return stream;
}

// 0x761E40 (lote R20A, sin validar; nombre inferido; callback de escritura, 62 bytes): como 0x761DA0 con la tuberia del sector (`object + 0x7C`).
RwStream* _rpWorldSectorRightsStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    const RxPipeline* const pipeline = static_cast<const RpWorldSector*>(object)->pipeline;
    if (!RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&pipeline->pluginId), 4)) {
        return nullptr;
    }
    return RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&pipeline->pluginData), 4) ? stream : nullptr;
}

// 0x7637F0 (lote R20A, sin validar; nombre inferido: escritura nativa D3D9 de la geometria; cdecl, 2 argumentos (flujo, RpGeometry), 641 bytes): sin rpGEOMETRYNATIVE (0x1000000) devuelve el flujo.
// Con ella: entrada de recursos (+0x58), cabecera D3D9 (+0x18 de la entrada) y declaracion de vertices (GetDeclaration, vtable +0x10; el EXE usa un buffer de pila de solo 20 elementos).
// Tamano del chunk = entrada->size + 8 + 8 * elementos + 4 (+ 2 * totalNumIndex si hay buffer de indices) + por cada uno de los dos flujos 0x10 (+ totalNumVertex * stride si tiene buffer).
// Sustituye el puntero de material de cada instancia por su indice en la lista de materiales de la geometria (0x74E420, SIN restaurarlo si despues falla algo). Escribe: cabecera de chunk (tipo 1, tamano,
// version 0x36003, build 0xFFFF; 0x7ED270), plataforma (4 bytes de 0x8D7384, 9), entrada->size (4), cabecera + datos (entrada->size bytes desde +0x18), numero de elementos (4), elementos
// (8 cada uno), indices (Lock(0, 0, ...) y 2 * totalNumIndex bytes; puntero nulo -> NULL sin Unlock; siempre Unlock tras escribir) y por flujo su estructura de 0x10 bytes mas su
// buffer (Lock(offset, stride * totalNumVertex, 0x800); Lock con HRESULT distinto de 0 -> NULL sin Unlock; Unlock tras escribir). Cualquier escritura NULL devuelve NULL. Al final
// devuelve a cada instancia su puntero de material (0x74E2B0) y devuelve el flujo.
RwStream* _rpD3D9GeometryNativeWrite(RwStream* stream, RpGeometry* geometry) {
    if ((geometry->flags & kNativeGeometryFlag) == 0) {
        return stream;
    }
    RwResEntry* const entry = geometry->repEntry;
    RxD3D9ResEntryHeader* const header = ResEntryHeader(entry);
    RxD3D9InstanceData* const instances = ResEntryInstances(header);

    D3DVERTEXELEMENT9 elements[kMaxNativeDeclElements];
    UINT numElements = 0;
    header->vertexDeclaration->GetDeclaration(elements, &numElements);
    RwUInt32 chunkSize = static_cast<RwUInt32>(entry->size) + 8 + numElements * 8 + 4;
    if (header->indexBuffer != nullptr) {
        chunkSize += header->totalNumIndex * 2;
    }
    for (RwUInt32 i = 0; i < 2; ++i) {
        chunkSize += 0x10;
        if (header->vertexStream[i].vertexBuffer != nullptr) {
            chunkSize += header->totalNumVertex * header->vertexStream[i].stride;
        }
    }
    for (RwUInt32 i = 0; i < header->numMeshes; ++i) {
        instances[i].material = reinterpret_cast<RpMaterial*>(static_cast<intptr_t>(_rpMaterialListFindMaterialIndex(&geometry->matList, instances[i].material)));
    }

    if (_rwStreamWriteVersionedChunkHeader(stream, 1, static_cast<RwInt32>(chunkSize), kNativeGeometryVersion, 0xFFFF) == nullptr) {
        return nullptr;
    }
    if (RwStreamWrite(stream, reinterpret_cast<const void*>(kNativePlatformIdAddress), 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamWrite(stream, reinterpret_cast<const uint8_t*>(geometry->repEntry) + 8, 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamWrite(stream, ResEntryHeader(geometry->repEntry), static_cast<RwUInt32>(geometry->repEntry->size)) == nullptr) {
        return nullptr;
    }
    if (RwStreamWrite(stream, &numElements, 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamWrite(stream, elements, numElements * 8) == nullptr) {
        return nullptr;
    }
    if (header->indexBuffer != nullptr) {
        void* locked = nullptr;
        header->indexBuffer->Lock(0, 0, &locked, 0);
        if (locked == nullptr) {
            return nullptr;
        }
        RwStream* const written = RwStreamWrite(stream, locked, header->totalNumIndex * 2);
        header->indexBuffer->Unlock();
        if (written == nullptr) {
            return nullptr;
        }
    }
    for (RwUInt32 i = 0; i < 2; ++i) {
        RxD3D9VertexStream& vertexStream = header->vertexStream[i];
        if (RwStreamWrite(stream, &vertexStream, 0x10) == nullptr) {
            return nullptr;
        }
        if (vertexStream.vertexBuffer != nullptr) {
            void* locked = nullptr;
            IDirect3DVertexBuffer9* const vertexBuffer = static_cast<IDirect3DVertexBuffer9*>(vertexStream.vertexBuffer);
            if (vertexBuffer->Lock(vertexStream.offset, vertexStream.stride * header->totalNumVertex, &locked, kDefaultInstLockManaged) != 0) {
                return nullptr;
            }
            RwStream* const written = RwStreamWrite(stream, locked, vertexStream.stride * header->totalNumVertex);
            vertexBuffer->Unlock();
            if (written == nullptr) {
                return nullptr;
            }
        }
    }
    for (RwUInt32 i = 0; i < header->numMeshes; ++i) {
        instances[i].material = _rpMaterialListGetMaterial(&geometry->matList, static_cast<RwInt32>(reinterpret_cast<intptr_t>(instances[i].material)));
    }
    return stream;
}

// 0x763A80 (lote R20A, sin validar; nombre inferido: lectura nativa D3D9 de la geometria; cdecl, 2 argumentos (flujo, RpGeometry), 1058 bytes; el EXE usa un buffer de pila de 20 elementos
// para la declaracion). Devuelve la geometria, o NULL en cualquier fallo. RwStreamFindChunk(tipo 1; longitud, version; 0x7ED2D0; FALSE -> NULL); version fuera de 0x34000..0x36003 (sin signo): error
// 0x80000004 (pluginID 2) y NULL. Lee la plataforma (4 bytes, debe ser 9) y el tamano de la entrada (4). Reserva la RwResEntry (tamano + 0x18, pista 0x3050D; SIN comprobar NULL) y la deja en
// geometry->repEntry: enlace a 0, owner = geometria, size, ownerRef = &repEntry, destroyNotify = 0x4C9990. Lee `tamano` bytes a +0x18; pone a 0 el buffer de indices, los dos buffers de vertices
// y la declaracion; lee el numero de elementos (4) y los elementos y llama a RwD3D9CreateVertexDeclaration (0x7FAA30; resultado ignorado). Con totalNumIndex != 0 crea el buffer de indices
// (0x4C9970; fallo -> 0x4C9990 sobre la entrada y NULL) y, si existe, lo rellena (Lock(0, 0, ...), lectura de 2 * totalNumIndex bytes solo con puntero no nulo; una lectura distinta de lo pedido -> 0x4C9990 y
// NULL). Por cada uno de los dos flujos lee su estructura (0x10 bytes); con vertexBuffer no nulo (el valor del fichero actua de bandera) cuenta un flujo, crea el buffer (con mas de un morph
// target, con signo, dinamico: managed = 0, 0x7F5A00; si no gestionado: managed = 1, 0x7F5500) y lo rellena. La bandera `fallo` empieza a 1; un buffer que no se pudo crear con fallo a 1 se salta; si no
// se pone fallo a 1, Lock (flags 0x800 gestionado, 0x2800 dinamico; el resultado se ignora), lee stride * totalNumVertex bytes y Unlock; si la lectura es completa fallo = 0. Con fallo != 0
// destruye la entrada y NULL. Con mas de un flujo: useOffsets = 1 si DevCaps2 (0xC9BFD4) & 1 y baseIndex = minVert de cada malla; con 0 o 1 flujos useOffsets = 0 y baseIndex = (offset del
// flujo 0 / stride del flujo 0, division sin signo) + minVert. Los bucles por malla son do/while como en el EXE (con numMeshes == 0 se salen de la tabla). Por ultimo cada indice de material
// de las instancias se cambia por su puntero (0x74E2B0).
RpGeometry* _rpD3D9GeometryNativeRead(RwStream* stream, RpGeometry* geometry) {
    RwUInt32 length  = 0;
    RwUInt32 version = 0;
    if (!RwStreamFindChunk(stream, 1, &length, &version)) {
        return nullptr;
    }
    if (version < kStreamVersionMin || version > kStreamVersionMax) {
        RaiseRwError(kErrorBadVersion);
        return nullptr;
    }
    RwUInt32 platformId = 0;
    if (RwStreamRead(stream, &platformId, 4) != 4) {
        return nullptr;
    }
    length -= 4;
    if (platformId != kNativeGeometryPlatform) {
        return nullptr;
    }
    RwUInt32 entrySize = 0;
    if (RwStreamRead(stream, &entrySize, 4) != 4) {
        return nullptr;
    }
    length -= 4;

    RwResEntry* const entry = static_cast<RwResEntry*>(RwMalloc(entrySize + 0x18, kNativeGeometryResHint));
    geometry->repEntry = entry;
    entry->link.next = nullptr;
    entry->link.prev = nullptr;
    entry->owner     = geometry;
    entry->size      = static_cast<RwInt32>(entrySize);
    entry->ownerRef  = &geometry->repEntry;
    entry->destroyNotify = reinterpret_cast<RwResEntryDestroyNotify>(kResEntryHeaderDestroyAddress);
    RxD3D9ResEntryHeader* const header = ResEntryHeader(entry);
    RxD3D9InstanceData* const instances = ResEntryInstances(header);
    if (RwStreamRead(stream, header, entrySize) != entrySize) {
        return nullptr;
    }
    length -= entrySize;

    header->indexBuffer = nullptr;
    header->vertexStream[0].vertexBuffer = nullptr;
    header->vertexStream[1].vertexBuffer = nullptr;
    header->vertexDeclaration = nullptr;

    RwUInt32 numElements = 0;
    if (RwStreamRead(stream, &numElements, 4) != 4) {
        return nullptr;
    }
    length -= 4;
    D3DVERTEXELEMENT9 elements[kMaxNativeDeclElements];
    if (RwStreamRead(stream, elements, numElements * 8) != numElements * 8) {
        return nullptr;
    }
    length -= numElements * 8;
    RwD3D9CreateVertexDeclaration(elements, reinterpret_cast<void**>(&header->vertexDeclaration));

    if (header->totalNumIndex != 0) {
        if (!RwD3D9IndexBufferCreate(header->totalNumIndex, reinterpret_cast<void**>(&header->indexBuffer))) {
            DestroyResEntryExe(geometry->repEntry);
            return nullptr;
        }
    }
    if (header->indexBuffer != nullptr) {
        void* data = nullptr;
        RwUInt32 readBytes = 0;
        header->indexBuffer->Lock(0, 0, &data, 0);
        const RwUInt32 indexBytes = header->totalNumIndex * 2;
        if (data != nullptr) {
            readBytes = RwStreamRead(stream, data, indexBytes);
            header->indexBuffer->Unlock();
        }
        if (readBytes != indexBytes) {
            DestroyResEntryExe(geometry->repEntry);
            return nullptr;
        }
        length -= readBytes;
    }

    RwUInt32 numStreams = 0;
    RwUInt32 failed = 1;
    for (RwUInt32 i = 0; i < 2; ++i) {
        RxD3D9VertexStream& vertexStream = header->vertexStream[i];
        if (RwStreamRead(stream, &vertexStream, 0x10) != 0x10) {
            DestroyResEntryExe(geometry->repEntry);
            return nullptr;
        }
        length -= 0x10;
        if (vertexStream.vertexBuffer == nullptr) {
            continue;
        }
        ++numStreams;
        const RwUInt32 bufferSize = header->totalNumVertex * vertexStream.stride;
        RwBool created;
        if (geometry->numMorphTargets > 1) {
            vertexStream.managed = 0;
            created = RwD3D9DynamicVertexBufferCreate(bufferSize, &vertexStream.vertexBuffer);
        } else {
            vertexStream.managed = 1;
            created = RwD3D9CreateVertexBuffer(vertexStream.stride, bufferSize, &vertexStream.vertexBuffer, &vertexStream.offset);
        }
        if (!created && failed != 0) {
            continue;
        }
        failed = 1;
        void* data = nullptr;
        IDirect3DVertexBuffer9* const vertexBuffer = static_cast<IDirect3DVertexBuffer9*>(vertexStream.vertexBuffer);
        vertexBuffer->Lock(vertexStream.offset, bufferSize, &data, vertexStream.managed ? kDefaultInstLockManaged : kDefaultInstLockDynamic);
        const RwUInt32 got = RwStreamRead(stream, data, bufferSize);
        static_cast<IDirect3DVertexBuffer9*>(vertexStream.vertexBuffer)->Unlock();
        if (got == bufferSize) {
            failed = 0;
            length -= got;
        }
    }
    if (failed != 0) {
        DestroyResEntryExe(geometry->repEntry);
        return nullptr;
    }

    if (numStreams > 1) {
        if ((D3D9CapsDword(kD3D9DevCaps2Address) & 1) != 0) {
            header->useOffsets = 1;
        }
        RwUInt32 remaining = header->numMeshes;
        RxD3D9InstanceData* instance = instances;
        do {
            instance->baseIndex = instance->minVert;
            ++instance;
            --remaining;
        } while (remaining != 0);
    } else {
        header->useOffsets = 0;
        RwUInt32 remaining = header->numMeshes;
        RxD3D9InstanceData* instance = instances;
        do {
            instance->baseIndex = header->vertexStream[0].offset / header->vertexStream[0].stride + instance->minVert;
            ++instance;
            --remaining;
        } while (remaining != 0);
    }
    for (RwUInt32 i = 0; i < header->numMeshes; ++i) {
        instances[i].material = _rpMaterialListGetMaterial(&geometry->matList, static_cast<RwInt32>(reinterpret_cast<intptr_t>(instances[i].material)));
    }
    return geometry;
}

// 0x763EB0 (lote R20A, sin validar; nombre inferido: tamano nativo D3D9 de la geometria; cdecl, 1 argumento (RpGeometry), 106 bytes): sin rpGEOMETRYNATIVE devuelve 0. Con ella: entrada de
// recursos (+0x58), GetDeclaration de su declaracion (vtable +0x10) y tamano = entrada->size + 0x14 + 2 * (totalNumIndex + 4 * elementos) + 4 (los indices cuentan SIEMPRE, a diferencia
// de 0x7637F0) mas, por cada uno de los dos flujos, 0x10 (+ totalNumVertex * stride si tiene buffer de vertices).
RwInt32 _rpD3D9GeometryNativeSize(const RpGeometry* geometry) {
    if ((geometry->flags & kNativeGeometryFlag) == 0) {
        return 0;
    }
    RwResEntry* const entry = geometry->repEntry;
    RxD3D9ResEntryHeader* const header = ResEntryHeader(entry);
    D3DVERTEXELEMENT9 elements[kMaxNativeDeclElements];
    UINT numElements = 0;
    header->vertexDeclaration->GetDeclaration(elements, &numElements);
    RwUInt32 total = static_cast<RwUInt32>(entry->size) + 0x14 + (header->totalNumIndex + numElements * 4) * 2 + 4;
    for (RwUInt32 i = 0; i < 2; ++i) {
        total += 0x10;
        if (header->vertexStream[i].vertexBuffer != nullptr) {
            total += header->totalNumVertex * header->vertexStream[i].stride;
        }
    }
    return static_cast<RwInt32>(total);
}

// Registro de los hooks del lote R20A de RpWorld (sin validar).
void RpWorldInjectHooksR20A() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpWorld");

    RH_ScopedGlobalInstall(_rpBuildStripListJoin, 0x75A660);
    RH_ScopedGlobalInstall(_rpTriListMeshGenerate, 0x75D530);
    RH_ScopedGlobalInstall(_rpMeshOptimise, 0x75D970);
    RH_ScopedGlobalInstall(RpWorldSetDefaultSectorPipeline, 0x75E270);
    RH_ScopedGlobalInstall(RpAtomicSetDefaultPipeline, 0x75E2A0);
    RH_ScopedGlobalInstall(_rpWorldPipelinesClose, 0x75E300);
    RH_ScopedGlobalInstall(_rpWorldPipelinesOpen, 0x75E310);
    RH_ScopedGlobalInstall(_rpWorldPipelinesAttach, 0x75E3E0);
    RH_ScopedGlobalInstall(_rpD3D9VertexShaderCachePurge, 0x75EE60);
    RH_ScopedGlobalInstall(_rpD3D9VertexShaderCacheOpen, 0x760CF0);
    RH_ScopedGlobalInstall(_rpD3D9VertexShaderSetEnvMatrix, 0x761A10);
    RH_ScopedGlobalInstall(_rpD3D9VertexShaderSetBumpMatrix, 0x761B70);
    RH_ScopedGlobalInstall(_rpWorldRightsStreamRead, 0x761D60);
    RH_ScopedGlobalInstall(_rpWorldRightsStreamWrite, 0x761DA0);
    RH_ScopedGlobalInstall(_rpWorldSectorRightsStreamRead, 0x761E00);
    RH_ScopedGlobalInstall(_rpWorldSectorRightsStreamWrite, 0x761E40);
    RH_ScopedGlobalInstall(_rpD3D9GeometryNativeWrite, 0x7637F0);
    RH_ScopedGlobalInstall(_rpD3D9GeometryNativeRead, 0x763A80);
    RH_ScopedGlobalInstall(_rpD3D9GeometryNativeSize, 0x763EB0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Lote R21A (sin validar): ayudas de matrices del vertex shader de D3D9 del modulo de mundos (0x764650..0x764F60) y flujo nativo (D3D9) de los RpWorldSector (0x763F20, 0x7641C0, 0x7645C0).
// Estado global del EXE (.data/.bss, accedido por direccion fija): 0xC94C70 = matriz activa (RwMatrix*, o NULL); 0xC94C74 = inversa en cache; 0xC94C78 = inversa para normales en cache;
// 0xC94C7C = matriz ortonormalizada en cache; 0x8D7390 = copia (12 floats) de la matriz activa; 0x8D73D0, 0x8D7410, 0x8D7450 = matrices 4x4 de las caches; 0x8753D0 = identidad estatica;
// 0xC9BC80 = matriz de vista; 0x8E2458 = proyeccion; 0xC94C30 = vista * proyeccion (0xC97C64 apunta a ella cuando ya se calculo).
// Las rutinas 0x7672E1, 0x7675EE, 0x7676C7, 0x766AB7 y 0x766A32 son `jmp [tabla]` a la implementacion de D3DX9 elegida en tiempo de ejecucion y se llaman por direccion fija (__stdcall,
// devuelven el puntero de salida en EAX, que aqui no se usa): D3DXMatrixMultiply, D3DXMatrixTranspose, D3DXMatrixMultiplyTranspose, D3DXVec3TransformNormal y D3DXVec3TransformCoord.
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------
static_assert(offsetof(RpWorldSector, repEntry) == 0x34 && offsetof(RpWorld, flags) == 0x08 && offsetof(RpWorld, matList) == 0x10);

namespace {
constexpr uintptr_t kVSActiveMatrixAddress      = 0xC94C70;
constexpr uintptr_t kVSInverseCacheAddress      = 0xC94C74;
constexpr uintptr_t kVSNormalInverseAddress     = 0xC94C78;
constexpr uintptr_t kVSOrthoNormalCacheAddress  = 0xC94C7C;
constexpr uintptr_t kVSActiveMatrixCopyAddress  = 0x8D7390;
constexpr uintptr_t kVSInverseStorageAddress    = 0x8D73D0;
constexpr uintptr_t kVSNormalInverseStorageAddress = 0x8D7410;
constexpr uintptr_t kVSOrthoNormalStorageAddress = 0x8D7450;
constexpr uintptr_t kVSIdentityMatrixAddress    = 0x8753D0;
constexpr uintptr_t kVSViewMatrixAddress        = 0xC9BC80;
constexpr uintptr_t kVSProjectionMatrixAddress  = 0x8E2458;
constexpr uintptr_t kVSComposedMatrixAddress    = 0xC94C30;
constexpr uintptr_t kVSComposedPointerAddress   = 0xC97C64;
constexpr RwUInt32  kMatrixIdentityFlag         = 0x20000; // rwMATRIXINTERNALIDENTITY
constexpr RwUInt32  kMatrixTypeMask             = 3;       // rwMATRIXTYPEMASK (3 = ortonormal)

using VSMatrixTransposeFn = void*(__stdcall*)(void* out, const void* in);
using VSMatrixMultiplyFn  = void*(__stdcall*)(void* out, const void* a, const void* b);
using VSVec3TransformFn   = void*(__stdcall*)(void* out, const void* v, const void* matrix);

inline void* VSMatrixTranspose(void* out, const void* in) { return reinterpret_cast<VSMatrixTransposeFn>(0x7675EE)(out, in); }
inline void* VSMatrixMultiply(void* out, const void* a, const void* b) { return reinterpret_cast<VSMatrixMultiplyFn>(0x7672E1)(out, a, b); }
inline void* VSMatrixMultiplyTranspose(void* out, const void* a, const void* b) { return reinterpret_cast<VSMatrixMultiplyFn>(0x7676C7)(out, a, b); }
inline void* VSVec3TransformNormal(void* out, const void* v, const void* m) { return reinterpret_cast<VSVec3TransformFn>(0x766AB7)(out, v, m); }
inline void* VSVec3TransformCoord(void* out, const void* v, const void* m) { return reinterpret_cast<VSVec3TransformFn>(0x766A32)(out, v, m); }

inline RwMatrix*& VSActiveMatrix()      { return *reinterpret_cast<RwMatrix**>(kVSActiveMatrixAddress); }
inline RwMatrix*& VSInverseCache()      { return *reinterpret_cast<RwMatrix**>(kVSInverseCacheAddress); }
inline RwMatrix*& VSNormalInverse()     { return *reinterpret_cast<RwMatrix**>(kVSNormalInverseAddress); }
inline RwMatrix*& VSOrthoNormalCache()  { return *reinterpret_cast<RwMatrix**>(kVSOrthoNormalCacheAddress); }
inline RwMatrix* VSAt(uintptr_t address) { return reinterpret_cast<RwMatrix*>(address); }

// Los cuatro floats de relleno de una matriz 4x4 (indices 3, 7, 11 y 15) como los deja el EXE tras RwMatrixInvert / RwMatrixOrthoNormalize: 0, 0, 0 y 1.0f.
inline void VSSetMatrixPads(RwMatrix* matrix) {
    float* const f = reinterpret_cast<float*>(matrix);
    f[3]  = 0.0f;
    f[7]  = 0.0f;
    f[11] = 0.0f;
    f[15] = 1.0f;
}

// Copia de 16 dwords (`rep movsd`) sin pasar por la FPU.
inline void VSCopyMatrix(void* dst, const void* src) {
    memcpy(dst, src, 0x40);
}

// Copia de tres dwords (x, y, z) en ese orden.
inline void VSCopyVec3(void* dst, const void* src) {
    uint32* const d = static_cast<uint32*>(dst);
    const uint32* const s = static_cast<const uint32*>(src);
    d[0] = s[0];
    d[1] = s[1];
    d[2] = s[2];
}

// Inversa en cache por el camino de matriz ortonormal (0x764890 sobre 0x8D73D0) o la general (RwMatrixInvert 0x7F2070 + rellenos 0, 0, 0, 1.0f); la guarda en 0xC94C74.
inline RwMatrix* VSBuildInverse(RwMatrix* active, bool rigid) {
    RwMatrix* const inverse = VSAt(kVSInverseStorageAddress);
    if (rigid) {
        _rwD3D9VSInvertRigidMatrix(reinterpret_cast<float*>(inverse), reinterpret_cast<const float*>(active));
    } else {
        RwMatrixInvert(inverse, active);
        VSSetMatrixPads(inverse);
    }
    VSInverseCache() = inverse;
    return inverse;
}

// Matriz activa ortonormalizada en cache (0xC94C7C): si no existe, RwMatrixOrthoNormalize(0x8D7450, activa) y rellenos 0, 0, 0, 1.0f.
inline RwMatrix* VSOrthoNormalized(RwMatrix* active) {
    RwMatrix* cached = VSOrthoNormalCache();
    if (cached == nullptr) {
        RwMatrix* const storage = VSAt(kVSOrthoNormalStorageAddress);
        RwMatrixOrthoNormalize(storage, active);
        VSSetMatrixPads(storage);
        VSOrthoNormalCache() = storage;
        cached = storage;
    }
    return cached;
}
}

// 0x764650 (lote R21A, sin validar; sustituye la envoltura; cdecl, 1 argumento, `ret`, 143 bytes): guarda la matriz activa en 0xC94C70 y, si no es NULL, copia sus 12 floats (indices 0-2, 4-6, 8-10 y
// 12-14, como dwords; los cuatro de relleno NO se copian) a 0x8D7390. Siempre pone a NULL las caches 0xC94C74 (inversa), 0xC94C78 (inversa para normales) y 0xC94C7C (ortonormalizada).
void _rwD3D9VSSetActiveWorldMatrix(const RwMatrix* worldMatrix) {
    VSActiveMatrix() = const_cast<RwMatrix*>(worldMatrix);
    if (worldMatrix != nullptr) {
        const uint32* const src = reinterpret_cast<const uint32*>(worldMatrix);
        uint32* const dst = reinterpret_cast<uint32*>(kVSActiveMatrixCopyAddress);
        dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2];
        dst[4] = src[4]; dst[5] = src[5]; dst[6] = src[6];
        dst[8] = src[8]; dst[9] = src[9]; dst[10] = src[10];
        dst[12] = src[12]; dst[13] = src[13]; dst[14] = src[14];
    }
    VSInverseCache() = nullptr;
    VSNormalInverse() = nullptr;
    VSOrthoNormalCache() = nullptr;
}

// 0x7646E0 (lote R21A, sin validar; sustituye la envoltura; cdecl, 1 argumento (matriz 4x4 de salida), `ret`, 80 bytes): si 0xC97C64 es NULL calcula vista * proyeccion en 0xC94C30
// (D3DXMatrixMultiply(0xC94C30, 0xC9BC80, 0x8E2458)) y deja en 0xC97C64 su direccion. Con matriz activa: D3DXMatrixMultiplyTranspose(salida, 0x8D7390, compuesta); sin ella:
// D3DXMatrixTranspose(salida, compuesta). La compuesta se lee de 0xC97C64.
void _rwD3D9VSGetComposedTransformMatrix(void* matrix) {
    void*& composedPointer = *reinterpret_cast<void**>(kVSComposedPointerAddress);
    if (composedPointer == nullptr) {
        VSMatrixMultiply(reinterpret_cast<void*>(kVSComposedMatrixAddress), reinterpret_cast<const void*>(kVSViewMatrixAddress), reinterpret_cast<const void*>(kVSProjectionMatrixAddress));
        composedPointer = reinterpret_cast<void*>(kVSComposedMatrixAddress);
    }
    void* const composed = composedPointer;
    if (VSActiveMatrix() != nullptr) {
        VSMatrixMultiplyTranspose(matrix, reinterpret_cast<const void*>(kVSActiveMatrixCopyAddress), composed);
    } else {
        VSMatrixTranspose(matrix, composed);
    }
}

// 0x764730 (lote R21A, sin validar; sustituye la envoltura; cdecl, 1 argumento (matriz 4x4 de salida), 44 bytes): con matriz activa D3DXMatrixMultiplyTranspose(salida, 0x8D7390, vista 0xC9BC80);
// sin ella D3DXMatrixTranspose(salida, vista).
void _rwD3D9VSGetWorldViewTransposedMatrix(void* matrix) {
    if (VSActiveMatrix() != nullptr) {
        VSMatrixMultiplyTranspose(matrix, reinterpret_cast<const void*>(kVSActiveMatrixCopyAddress), reinterpret_cast<const void*>(kVSViewMatrixAddress));
    } else {
        VSMatrixTranspose(matrix, reinterpret_cast<const void*>(kVSViewMatrixAddress));
    }
}

// 0x7647B0 (lote R21A, sin validar; sustituye la envoltura; cdecl, 1 argumento (matriz 4x4 de salida), 214 bytes): copia (16 dwords) a la salida la inversa de la matriz activa. Cache 0xC94C74: si no
// es NULL se usa tal cual. Si es NULL: con matriz activa sin la marca de identidad (0x20000) y de tipo ortonormal (flags & 3 == 3) la inversa es 0x764890(0x8D73D0, activa) y se fijan 0xC94C78 y
// 0xC94C74 a 0x8D73D0; con otro tipo, RwMatrixInvert(0x8D73D0, activa) con rellenos 0, 0, 0, 1.0f y SOLO se fija 0xC94C74. Sin matriz activa o con identidad: 0xC94C74 = 0xC94C78 = 0x8753D0 (identidad).
void _rwD3D9VSGetInverseWorldMatrix(void* matrix) {
    RwMatrix* inverse = VSInverseCache();
    if (inverse == nullptr) {
        RwMatrix* const active = VSActiveMatrix();
        if (active != nullptr && (active->flags & kMatrixIdentityFlag) != kMatrixIdentityFlag) {
            if ((active->flags & kMatrixTypeMask) == kMatrixTypeMask) {
                _rwD3D9VSInvertRigidMatrix(reinterpret_cast<float*>(VSAt(kVSInverseStorageAddress)), reinterpret_cast<const float*>(active));
                VSNormalInverse() = VSAt(kVSInverseStorageAddress);
                VSInverseCache() = VSAt(kVSInverseStorageAddress);
                VSCopyMatrix(matrix, VSAt(kVSInverseStorageAddress));
                return;
            }
            RwMatrixInvert(VSAt(kVSInverseStorageAddress), active);
            VSSetMatrixPads(VSAt(kVSInverseStorageAddress));
            VSInverseCache() = VSAt(kVSInverseStorageAddress);
            VSCopyMatrix(matrix, VSAt(kVSInverseStorageAddress));
            return;
        }
        inverse = VSAt(kVSIdentityMatrixAddress);
        VSNormalInverse() = inverse;
        VSInverseCache() = inverse;
    }
    VSCopyMatrix(matrix, inverse);
}

// 0x764890 (lote R21A, sin validar; nombre inferido: inversa de una matriz ortonormal; cdecl, (salida, entrada), 141 bytes, `ret`): la parte de rotacion se escribe traspuesta (salida[0], [4], [8] =
// entrada[0], [1], [2]; salida[1], [5], [9] = entrada[4], [5], [6]; salida[2], [6], [10] = entrada[8], [9], [10]; dwords, sin FPU) y la traslacion es -(pos . ejes) con la x87 a 24 bits en el orden
// del EXE (MatrixInvertOrthoPosCpp, la misma secuencia que 0x7F2070). Los floats 3, 7, 11 y 15 de la salida NO se tocan. Con salida == entrada el resultado diferiria del EXE (este escribe la
// traslacion x antes de copiar la rotacion); los llamadores usan siempre 0x8D73D0 como salida.
void _rwD3D9VSInvertRigidMatrix(float* out, const float* in) {
    MatrixInvertOrthoPosCpp(out, in);
    uint32* const o = reinterpret_cast<uint32*>(out);
    const uint32* const i = reinterpret_cast<const uint32*>(in);
    o[0]  = i[0];
    o[4]  = i[1];
    o[8]  = i[2];
    o[1]  = i[4];
    o[5]  = i[5];
    o[9]  = i[6];
    o[2]  = i[8];
    o[6]  = i[9];
    o[10] = i[10];
}

// 0x764960 (lote R21A, sin validar; sustituye la envoltura; cdecl, (salida 4x4, matriz), 104 bytes): con matriz activa sin marca de identidad, RwMatrixMultiply(tmp, activa, matriz) y la fuente es tmp;
// si no, la fuente es la matriz recibida. D3DXMatrixTranspose(salida, fuente) y despues salida[12..14] = 0, salida[15] = 1.0f.
void _rwD3D9VSGetWorldMultiplyTransposeMatrix(void* worldMultiplyMatrix, const RwMatrix* matrix) {
    RwMatrix* const active = VSActiveMatrix();
    RwMatrix product;
    const void* source = matrix;
    if (active != nullptr && (active->flags & kMatrixIdentityFlag) != kMatrixIdentityFlag) {
        RwMatrixMultiply(&product, active, matrix);
        source = &product;
    }
    VSMatrixTranspose(worldMultiplyMatrix, source);
    float* const out = static_cast<float*>(worldMultiplyMatrix);
    out[12] = 0.0f;
    out[13] = 0.0f;
    out[14] = 0.0f;
    out[15] = 1.0f;
}

// 0x7649D0 (lote R21A, sin validar; sustituye la envoltura; cdecl, (salida 4x4, matriz), 159 bytes; el EXE alinea la pila a 16): con matriz activa sin marca de identidad, A = activa * vista (0xC9BC80) y
// B = A * matriz (RwMatrixMultiply(A, activa, vista); RwMatrixMultiply(B, A, matriz)); si no, B = vista * matriz. B[3], B[7], B[11] = 0 y B[15] = 1.0f y D3DXMatrixTranspose(salida, B).
void _rwD3D9VSGetWorldViewMultiplyTransposeMatrix(void* worldViewMultiplyMatrix, const RwMatrix* matrix) {
    RwMatrix* const active = VSActiveMatrix();
    RwMatrix first;
    RwMatrix second;
    if (active != nullptr && (active->flags & kMatrixIdentityFlag) != kMatrixIdentityFlag) {
        RwMatrixMultiply(&first, active, VSAt(kVSViewMatrixAddress));
        RwMatrixMultiply(&second, &first, matrix);
    } else {
        RwMatrixMultiply(&second, VSAt(kVSViewMatrixAddress), matrix);
    }
    VSSetMatrixPads(&second);
    VSMatrixTranspose(worldViewMultiplyMatrix, &second);
}

// 0x764A70 (lote R21A, sin validar; sustituye la envoltura; cdecl, (salida 4x4, matriz), 219 bytes): con matriz activa sin marca de identidad: si es ortonormal (flags & 3 == 3) tmp = activa * matriz;
// si no, usa la activa ortonormalizada en cache (0xC94C7C; si falta, RwMatrixOrthoNormalize(0x8D7450, activa) y rellenos 0, 0, 0, 1.0f) y tmp = ortonormalizada * matriz. Sin activa o con identidad la
// fuente es la matriz recibida. D3DXMatrixTranspose(salida, fuente) y salida[12..14] = 0, salida[15] = 1.0f.
void _rwD3D9VSGetWorldNormalizedMultiplyTransposeMatrix(void* worldMultiplyMatrix, const RwMatrix* matrix) {
    RwMatrix* const active = VSActiveMatrix();
    RwMatrix product;
    const void* source = matrix;
    if (active != nullptr && (active->flags & kMatrixIdentityFlag) != kMatrixIdentityFlag) {
        if ((active->flags & kMatrixTypeMask) == kMatrixTypeMask) {
            RwMatrixMultiply(&product, active, matrix);
        } else {
            RwMatrixMultiply(&product, VSOrthoNormalized(active), matrix);
        }
        source = &product;
    }
    VSMatrixTranspose(worldMultiplyMatrix, source);
    float* const out = static_cast<float*>(worldMultiplyMatrix);
    out[12] = 0.0f;
    out[13] = 0.0f;
    out[14] = 0.0f;
    out[15] = 1.0f;
}

// 0x764B50 (lote R21A, sin validar; sustituye la envoltura; cdecl, (salida 4x4, matriz), 269 bytes; el EXE alinea la pila a 16): con matriz activa sin marca de identidad, A = (activa si es ortonormal, si
// no la ortonormalizada en cache) * vista (0xC9BC80) y B = A * matriz; si no, B = vista * matriz. B[3], B[7], B[11] = 0 y B[15] = 1.0f y D3DXMatrixTranspose(salida, B).
void _rwD3D9VSGetWorldNormalizedViewMultiplyTransposeMatrix(void* worldViewMultiplyMatrix, const RwMatrix* matrix) {
    RwMatrix* const active = VSActiveMatrix();
    RwMatrix first;
    RwMatrix second;
    if (active != nullptr && (active->flags & kMatrixIdentityFlag) != kMatrixIdentityFlag) {
        if ((active->flags & kMatrixTypeMask) == kMatrixTypeMask) {
            RwMatrixMultiply(&first, active, VSAt(kVSViewMatrixAddress));
        } else {
            RwMatrixMultiply(&first, VSOrthoNormalized(active), VSAt(kVSViewMatrixAddress));
        }
        RwMatrixMultiply(&second, &first, matrix);
    } else {
        RwMatrixMultiply(&second, VSAt(kVSViewMatrixAddress), matrix);
    }
    VSSetMatrixPads(&second);
    VSMatrixTranspose(worldViewMultiplyMatrix, &second);
}

// 0x764C60 (lote R21A, sin validar; sustituye la envoltura; cdecl, 1 argumento (salida 4x4), 187 bytes): con matriz activa sin marca de identidad: ortonormal -> D3DXMatrixTranspose(salida, 0x8D7390)
// (la copia de 12 floats: sus cuatro floats de relleno son los que haya en memoria y acaban en salida[12..15]); si no -> D3DXMatrixTranspose(salida, ortonormalizada en cache). Sin activa o con identidad:
// copia la identidad estatica 0x8753D0 (16 dwords) y pone salida[3], [7], [11] = 0 y [15] = 1.0f.
void _rwD3D9VSGetWorldNormalizedTransposeMatrix(void* matrix) {
    RwMatrix* const active = VSActiveMatrix();
    if (active != nullptr && (active->flags & kMatrixIdentityFlag) != kMatrixIdentityFlag) {
        if ((active->flags & kMatrixTypeMask) == kMatrixTypeMask) {
            VSMatrixTranspose(matrix, reinterpret_cast<const void*>(kVSActiveMatrixCopyAddress));
        } else {
            VSMatrixTranspose(matrix, VSOrthoNormalized(active));
        }
        return;
    }
    VSCopyMatrix(matrix, VSAt(kVSIdentityMatrixAddress));
    float* const out = static_cast<float*>(matrix);
    out[3]  = 0.0f;
    out[7]  = 0.0f;
    out[11] = 0.0f;
    out[15] = 1.0f;
}

// 0x764D30 (lote R21A, sin validar; sustituye la envoltura; cdecl, (normal en el mundo, normal local de salida), 308 bytes): inversa para normales en cache 0xC94C78. Si es NULL: sin activa o con identidad
// se fijan 0xC94C74 = 0xC94C78 = 0x8753D0; ortonormal: inversa 0x764890 en 0x8D73D0 (si 0xC94C74 es NULL) y 0xC94C78 = esa inversa; otro tipo: inversa general RwMatrixInvert (si 0xC94C74 es NULL), despues
// RwMatrixOrthoNormalize(0x8D7410, inversa) con rellenos 0, 0, 0, 1.0f, 0xC94C78 = 0x8D7410 y D3DXVec3TransformNormal(salida, entrada, 0x8D7410). Con identidad la salida copia los tres dwords de la
// entrada; si no, D3DXVec3TransformNormal(salida, entrada, inversa).
void _rwD3D9VSGetNormalInLocalSpace(const RwV3d* normalWorldSpace, RwV3d* normalLocalSpace) {
    RwMatrix* normalInverse = VSNormalInverse();
    if (normalInverse == nullptr) {
        RwMatrix* const active = VSActiveMatrix();
        if (active == nullptr || (active->flags & kMatrixIdentityFlag) == kMatrixIdentityFlag) {
            normalInverse = VSAt(kVSIdentityMatrixAddress);
            VSInverseCache() = normalInverse;
            VSNormalInverse() = normalInverse;
            VSCopyVec3(normalLocalSpace, normalWorldSpace);
            return;
        }
        RwMatrix* inverse = VSInverseCache();
        if ((active->flags & kMatrixTypeMask) == kMatrixTypeMask) {
            if (inverse == nullptr) {
                inverse = VSBuildInverse(active, true);
            }
            VSNormalInverse() = inverse;
            normalInverse = inverse;
        } else {
            if (inverse == nullptr) {
                inverse = VSBuildInverse(active, false);
            }
            RwMatrix* const orthoNormal = VSAt(kVSNormalInverseStorageAddress);
            RwMatrixOrthoNormalize(orthoNormal, inverse);
            VSSetMatrixPads(orthoNormal);
            VSNormalInverse() = orthoNormal;
            VSVec3TransformNormal(normalLocalSpace, normalWorldSpace, orthoNormal);
            return;
        }
    }
    if (normalInverse == VSAt(kVSIdentityMatrixAddress)) {
        VSCopyVec3(normalLocalSpace, normalWorldSpace);
        return;
    }
    VSVec3TransformNormal(normalLocalSpace, normalWorldSpace, normalInverse);
}

// 0x764E70 (lote R21A, sin validar; sustituye la envoltura; cdecl, (punto en el mundo, punto local de salida), 234 bytes): inversa en cache 0xC94C74. Si es NULL: sin activa o con identidad se fija a la identidad
// 0x8753D0 y la salida copia los tres dwords de la entrada; ortonormal: 0x764890 en 0x8D73D0; otro tipo: RwMatrixInvert con rellenos 0, 0, 0, 1.0f; despues D3DXVec3TransformCoord(salida, entrada, inversa).
// Con la cache ya a la identidad copia; con cualquier otra, D3DXVec3TransformCoord.
void _rwD3D9VSGetPointInLocalSpace(const RwV3d* pointWorldSpace, RwV3d* pointLocalSpace) {
    RwMatrix* inverse = VSInverseCache();
    if (inverse == nullptr) {
        RwMatrix* const active = VSActiveMatrix();
        if (active != nullptr && (active->flags & kMatrixIdentityFlag) != kMatrixIdentityFlag) {
            inverse = VSBuildInverse(active, (active->flags & kMatrixTypeMask) == kMatrixTypeMask);
            VSVec3TransformCoord(pointLocalSpace, pointWorldSpace, inverse);
            return;
        }
        VSInverseCache() = VSAt(kVSIdentityMatrixAddress);
        VSCopyVec3(pointLocalSpace, pointWorldSpace);
        return;
    }
    if (inverse == VSAt(kVSIdentityMatrixAddress)) {
        VSCopyVec3(pointLocalSpace, pointWorldSpace);
        return;
    }
    VSVec3TransformCoord(pointLocalSpace, pointWorldSpace, inverse);
}

// 0x764F60 (lote R21A, sin validar; sustituye la envoltura; cdecl, (radio float, float* de salida), 203 bytes): con matriz activa cuyo tipo NO es ortonormal (flags & 3 != 3; aqui no se mira la marca de
// identidad): inversa en cache 0xC94C74 (si falta, RwMatrixInvert(0x8D73D0, activa) y rellenos 0, 0, 0, 1.0f), v = (0x3F13CD3A, 0x3F13CD3A, 0x3F13CD3A) (1/raiz de 3), D3DXVec3TransformNormal(v, v, inversa),
// s = float(((v.y*v.y) + (v.x*v.x)) + (v.z*v.z)) guardado como float y *salida = _rwInvSqrt(s) * radio (x87 a 24 bits, fmul + fstp). En cualquier otro caso *salida = radio.
void _rwD3D9VSGetRadiusInLocalSpace(RwReal radiusWorldSpace, RwReal* radiusLocalSpace) {
    RwMatrix* const active = VSActiveMatrix();
    if (active != nullptr && (active->flags & kMatrixTypeMask) != kMatrixTypeMask) {
        RwMatrix* inverse = VSInverseCache();
        if (inverse == nullptr) {
            inverse = VSBuildInverse(active, false);
        }
        float v[3];
        const uint32 third = 0x3F13CD3A;
        memcpy(&v[0], &third, 4);
        memcpy(&v[1], &third, 4);
        memcpy(&v[2], &third, 4);
        VSVec3TransformNormal(v, v, inverse);
        const float sumSquares = V3dSumSquaresCpp(v);
        *radiusLocalSpace = _rwInvSqrt(sumSquares) * radiusWorldSpace;
        return;
    }
    *radiusLocalSpace = radiusWorldSpace;
}

// 0x7645C0 (lote R21A, sin validar; nombre inferido: tamano nativo D3D9 del sector; cdecl, 1 argumento (RpWorldSector), 138 bytes): sin entrada de recursos (sector + 0x34 NULL) devuelve 0; con ella,
// RpWorldSectorGetWorld(sector) (0x74F4E0) y, si el mundo no tiene rpWORLDNATIVE (0x1000000 en world->flags), devuelve 0. Si no, GetDeclaration de la declaracion de la cabecera (vtable +0x10) y
// tamano = entrada->size + 0x14 + 2 * (totalNumIndex + 4 * elementos) + 4 mas, por cada uno de los dos flujos, 0x10 (+ totalNumVertex * stride si tiene buffer de vertices). Como 0x763EB0.
RwInt32 _rpD3D9WorldSectorNativeSize(const RpWorldSector* sector) {
    if (sector->repEntry == nullptr) {
        return 0;
    }
    const RpWorld* const world = RpWorldSectorGetWorld(sector);
    if ((world->flags & kNativeGeometryFlag) == 0) {
        return 0;
    }
    RwResEntry* const entry = sector->repEntry;
    RxD3D9ResEntryHeader* const header = ResEntryHeader(entry);
    D3DVERTEXELEMENT9 elements[kMaxNativeDeclElements];
    UINT numElements = 0;
    header->vertexDeclaration->GetDeclaration(elements, &numElements);
    RwUInt32 total = static_cast<RwUInt32>(entry->size) + 0x14 + (header->totalNumIndex + numElements * 4) * 2 + 4;
    for (RwUInt32 i = 0; i < 2; ++i) {
        total += 0x10;
        if (header->vertexStream[i].vertexBuffer != nullptr) {
            total += header->totalNumVertex * header->vertexStream[i].stride;
        }
    }
    return static_cast<RwInt32>(total);
}

// 0x763F20 (lote R21A, sin validar; nombre inferido: escritura nativa D3D9 del sector; cdecl, 2 argumentos (flujo, RpWorldSector), 657 bytes): world = RpWorldSectorGetWorld(sector); sin rpWORLDNATIVE
// devuelve el flujo. Con ella: entrada de recursos (sector + 0x34), cabecera D3D9 (+0x18) y GetDeclaration (el EXE usa un buffer de pila de 18 elementos). Tamano del chunk = entrada->size + 8 + 8 *
// elementos + 4 (+ 2 * totalNumIndex si hay buffer de indices) + por flujo 0x10 (+ totalNumVertex * stride si tiene buffer). Cambia el puntero de material de cada instancia por su indice en la lista de
// materiales del MUNDO (0x74E420 sobre world + 0x10; NO se restaura si despues falla algo). Escribe: cabecera de chunk (tipo 1, tamano, version 0x36003, build 0xFFFF; 0x7ED270), plataforma (4 bytes de
// 0x8D7384), entrada->size (4), cabecera + datos (entrada->size bytes), numero de elementos (4), elementos (8 cada uno), indices (Lock(0, 0, ...); con puntero no nulo escribe 2 * totalNumIndex bytes;
// Unlock SIEMPRE; sin escritura -> NULL tras el Unlock) y por flujo su estructura de 0x10 bytes y su buffer (Lock(offset, stride * totalNumVertex, 0x800): con HRESULT distinto de 0 se SALTA el buffer sin
// error; si no se escribe, Unlock y escritura NULL -> NULL). Cualquier escritura NULL devuelve NULL. Al final devuelve a cada instancia su puntero de material (0x74E2B0) y devuelve el flujo.
RwStream* _rpD3D9WorldSectorNativeWrite(RwStream* stream, const RpWorldSector* sector) {
    RpWorld* const world = RpWorldSectorGetWorld(sector);
    if ((world->flags & kNativeGeometryFlag) == 0) {
        return stream;
    }
    RwResEntry* const entry = sector->repEntry;
    RxD3D9ResEntryHeader* const header = ResEntryHeader(entry);
    RxD3D9InstanceData* const instances = ResEntryInstances(header);

    D3DVERTEXELEMENT9 elements[kMaxNativeDeclElements];
    UINT numElements = 0;
    header->vertexDeclaration->GetDeclaration(elements, &numElements);
    RwUInt32 chunkSize = static_cast<RwUInt32>(entry->size) + 8 + numElements * 8 + 4;
    if (header->indexBuffer != nullptr) {
        chunkSize += header->totalNumIndex * 2;
    }
    for (RwUInt32 i = 0; i < 2; ++i) {
        chunkSize += 0x10;
        if (header->vertexStream[i].vertexBuffer != nullptr) {
            chunkSize += header->totalNumVertex * header->vertexStream[i].stride;
        }
    }
    for (RwUInt32 i = 0; i < header->numMeshes; ++i) {
        instances[i].material = reinterpret_cast<RpMaterial*>(static_cast<intptr_t>(_rpMaterialListFindMaterialIndex(&world->matList, instances[i].material)));
    }

    if (_rwStreamWriteVersionedChunkHeader(stream, 1, static_cast<RwInt32>(chunkSize), kNativeGeometryVersion, 0xFFFF) == nullptr) {
        return nullptr;
    }
    if (RwStreamWrite(stream, reinterpret_cast<const void*>(kNativePlatformIdAddress), 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamWrite(stream, reinterpret_cast<const uint8_t*>(sector->repEntry) + 8, 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamWrite(stream, ResEntryHeader(sector->repEntry), static_cast<RwUInt32>(sector->repEntry->size)) == nullptr) {
        return nullptr;
    }
    if (RwStreamWrite(stream, &numElements, 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamWrite(stream, elements, numElements * 8) == nullptr) {
        return nullptr;
    }
    if (header->indexBuffer != nullptr) {
        void* locked = nullptr;
        RwStream* written = nullptr;
        header->indexBuffer->Lock(0, 0, &locked, 0);
        if (locked != nullptr) {
            written = RwStreamWrite(stream, locked, header->totalNumIndex * 2);
        }
        header->indexBuffer->Unlock();
        if (written == nullptr) {
            return nullptr;
        }
    }
    for (RwUInt32 i = 0; i < 2; ++i) {
        RxD3D9VertexStream& vertexStream = header->vertexStream[i];
        if (RwStreamWrite(stream, &vertexStream, 0x10) == nullptr) {
            return nullptr;
        }
        if (vertexStream.vertexBuffer != nullptr) {
            void* locked = nullptr;
            IDirect3DVertexBuffer9* const vertexBuffer = static_cast<IDirect3DVertexBuffer9*>(vertexStream.vertexBuffer);
            if (vertexBuffer->Lock(vertexStream.offset, vertexStream.stride * header->totalNumVertex, &locked, kDefaultInstLockManaged) == 0) {
                RwStream* const written = RwStreamWrite(stream, locked, vertexStream.stride * header->totalNumVertex);
                vertexBuffer->Unlock();
                if (written == nullptr) {
                    return nullptr;
                }
            }
        }
    }
    for (RwUInt32 i = 0; i < header->numMeshes; ++i) {
        instances[i].material = _rpMaterialListGetMaterial(&world->matList, static_cast<RwInt32>(reinterpret_cast<intptr_t>(instances[i].material)));
    }
    return stream;
}

// 0x7641C0 (lote R21A, sin validar; nombre inferido: lectura nativa D3D9 del sector; cdecl, 2 argumentos (flujo, RpWorldSector), 1012 bytes; el EXE usa un buffer de pila de 18 elementos para la
// declaracion). Igual que 0x763A80 (geometria) con estas diferencias: world = RpWorldSectorGetWorld(sector) se pide ANTES de buscar el chunk y su lista de materiales (world + 0x10) es la que
// convierte los indices en punteros (0x74E2B0); la RwResEntry cuelga de sector + 0x34 (ownerRef = &sector->repEntry); los buffers de vertices son SIEMPRE gestionados (managed = 1 antes de crear con
// 0x7F5500, Lock con flags 0x800); no hay camino dinamico por morph targets. Devuelve el sector, o NULL en cualquier fallo (FindChunk 0x7ED2D0; version fuera de 0x34000..0x36003 sin signo: error
// 0x80000004 (pluginID 2); plataforma distinta de 9; lecturas incompletas; indices o buffers sin crear con la bandera `fallo`). Bucles por malla do/while como en el EXE (con numMeshes == 0 se salen de
// la tabla).
RpWorldSector* _rpD3D9WorldSectorNativeRead(RwStream* stream, RpWorldSector* sector) {
    RpWorld* const world = RpWorldSectorGetWorld(sector);
    RwUInt32 length  = 0;
    RwUInt32 version = 0;
    if (!RwStreamFindChunk(stream, 1, &length, &version)) {
        return nullptr;
    }
    if (version < kStreamVersionMin || version > kStreamVersionMax) {
        RaiseRwError(kErrorBadVersion);
        return nullptr;
    }
    RwUInt32 platformId = 0;
    if (RwStreamRead(stream, &platformId, 4) != 4) {
        return nullptr;
    }
    length -= 4;
    if (platformId != kNativeGeometryPlatform) {
        return nullptr;
    }
    RwUInt32 entrySize = 0;
    if (RwStreamRead(stream, &entrySize, 4) != 4) {
        return nullptr;
    }
    length -= 4;

    sector->repEntry = static_cast<RwResEntry*>(RwMalloc(entrySize + 0x18, kNativeGeometryResHint));
    RwResEntry* const entry = sector->repEntry;
    entry->link.next = nullptr;
    entry->link.prev = nullptr;
    entry->owner     = sector;
    entry->size      = static_cast<RwInt32>(entrySize);
    entry->ownerRef  = &sector->repEntry;
    entry->destroyNotify = reinterpret_cast<RwResEntryDestroyNotify>(kResEntryHeaderDestroyAddress);
    RxD3D9ResEntryHeader* const header = ResEntryHeader(entry);
    RxD3D9InstanceData* const instances = ResEntryInstances(header);
    if (RwStreamRead(stream, header, entrySize) != entrySize) {
        return nullptr;
    }
    length -= entrySize;

    header->indexBuffer = nullptr;
    header->vertexStream[0].vertexBuffer = nullptr;
    header->vertexStream[1].vertexBuffer = nullptr;
    header->vertexDeclaration = nullptr;

    RwUInt32 numElements = 0;
    if (RwStreamRead(stream, &numElements, 4) != 4) {
        return nullptr;
    }
    length -= 4;
    D3DVERTEXELEMENT9 elements[kMaxNativeDeclElements];
    if (RwStreamRead(stream, elements, numElements * 8) != numElements * 8) {
        return nullptr;
    }
    length -= numElements * 8;
    RwD3D9CreateVertexDeclaration(elements, reinterpret_cast<void**>(&header->vertexDeclaration));

    if (header->totalNumIndex != 0) {
        if (!RwD3D9IndexBufferCreate(header->totalNumIndex, reinterpret_cast<void**>(&header->indexBuffer))) {
            DestroyResEntryExe(sector->repEntry);
            return nullptr;
        }
    }
    if (header->indexBuffer != nullptr) {
        void* data = nullptr;
        RwUInt32 readBytes = 0;
        header->indexBuffer->Lock(0, 0, &data, 0);
        const RwUInt32 indexBytes = header->totalNumIndex * 2;
        if (data != nullptr) {
            readBytes = RwStreamRead(stream, data, indexBytes);
            header->indexBuffer->Unlock();
        }
        if (readBytes != indexBytes) {
            DestroyResEntryExe(sector->repEntry);
            return nullptr;
        }
        length -= readBytes;
    }

    RwUInt32 numStreams = 0;
    RwUInt32 failed = 1;
    for (RwUInt32 i = 0; i < 2; ++i) {
        RxD3D9VertexStream& vertexStream = header->vertexStream[i];
        if (RwStreamRead(stream, &vertexStream, 0x10) != 0x10) {
            DestroyResEntryExe(sector->repEntry);
            return nullptr;
        }
        if (vertexStream.vertexBuffer == nullptr) {
            continue;
        }
        ++numStreams;
        const RwUInt32 bufferSize = header->totalNumVertex * vertexStream.stride;
        vertexStream.managed = 1;
        const RwBool created = RwD3D9CreateVertexBuffer(vertexStream.stride, bufferSize, &vertexStream.vertexBuffer, &vertexStream.offset);
        if (!created && failed != 0) {
            continue;
        }
        failed = 1;
        void* data = nullptr;
        IDirect3DVertexBuffer9* const vertexBuffer = static_cast<IDirect3DVertexBuffer9*>(vertexStream.vertexBuffer);
        vertexBuffer->Lock(vertexStream.offset, bufferSize, &data, kDefaultInstLockManaged);
        const RwUInt32 got = RwStreamRead(stream, data, bufferSize);
        static_cast<IDirect3DVertexBuffer9*>(vertexStream.vertexBuffer)->Unlock();
        if (got == bufferSize) {
            failed = 0;
            length -= got;
        }
    }
    if (failed != 0) {
        DestroyResEntryExe(sector->repEntry);
        return nullptr;
    }

    if (numStreams > 1) {
        if ((D3D9CapsDword(kD3D9DevCaps2Address) & 1) != 0) {
            header->useOffsets = 1;
        }
        RwUInt32 remaining = header->numMeshes;
        RxD3D9InstanceData* instance = instances;
        do {
            instance->baseIndex = instance->minVert;
            ++instance;
            --remaining;
        } while (remaining != 0);
    } else {
        header->useOffsets = 0;
        RwUInt32 remaining = header->numMeshes;
        RxD3D9InstanceData* instance = instances;
        do {
            instance->baseIndex = header->vertexStream[0].offset / header->vertexStream[0].stride + instance->minVert;
            ++instance;
            --remaining;
        } while (remaining != 0);
    }
    for (RwUInt32 i = 0; i < header->numMeshes; ++i) {
        instances[i].material = _rpMaterialListGetMaterial(&world->matList, static_cast<RwInt32>(reinterpret_cast<intptr_t>(instances[i].material)));
    }
    return sector;
}

// Registro de los hooks del lote R21A de RpWorld (sin validar).
void RpWorldInjectHooksR21A() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpWorld");

    RH_ScopedGlobalInstall(_rpD3D9WorldSectorNativeWrite, 0x763F20);
    RH_ScopedGlobalInstall(_rpD3D9WorldSectorNativeRead, 0x7641C0);
    RH_ScopedGlobalInstall(_rpD3D9WorldSectorNativeSize, 0x7645C0);
    RH_ScopedGlobalInstall(_rwD3D9VSSetActiveWorldMatrix, 0x764650);
    RH_ScopedGlobalInstall(_rwD3D9VSGetComposedTransformMatrix, 0x7646E0);
    RH_ScopedGlobalInstall(_rwD3D9VSGetWorldViewTransposedMatrix, 0x764730);
    RH_ScopedGlobalInstall(_rwD3D9VSGetInverseWorldMatrix, 0x7647B0);
    RH_ScopedGlobalInstall(_rwD3D9VSInvertRigidMatrix, 0x764890);
    RH_ScopedGlobalInstall(_rwD3D9VSGetWorldMultiplyTransposeMatrix, 0x764960);
    RH_ScopedGlobalInstall(_rwD3D9VSGetWorldViewMultiplyTransposeMatrix, 0x7649D0);
    RH_ScopedGlobalInstall(_rwD3D9VSGetWorldNormalizedMultiplyTransposeMatrix, 0x764A70);
    RH_ScopedGlobalInstall(_rwD3D9VSGetWorldNormalizedViewMultiplyTransposeMatrix, 0x764B50);
    RH_ScopedGlobalInstall(_rwD3D9VSGetWorldNormalizedTransposeMatrix, 0x764C60);
    RH_ScopedGlobalInstall(_rwD3D9VSGetNormalInLocalSpace, 0x764D30);
    RH_ScopedGlobalInstall(_rwD3D9VSGetPointInLocalSpace, 0x764E70);
    RH_ScopedGlobalInstall(_rwD3D9VSGetRadiusInLocalSpace, 0x764F60);
}

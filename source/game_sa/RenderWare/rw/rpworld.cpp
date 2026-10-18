/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RpWorld
 * Reconstrucción: COMPLETA — 21/21 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 21/21 entradas.
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

RpMaterial* RpMaterialCreate() {
    return ((RpMaterial*(__cdecl *)(void))0x74D990)();
}

RwBool RpMaterialDestroy(RpMaterial* material) {
    return ((RwBool(__cdecl *)(RpMaterial*))0x74DA20)(material);
}

RpMaterial* RpMaterialClone(RpMaterial* material) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*))0x74DA80)(material);
}

RpMaterial* RpMaterialSetTexture(RpMaterial* material, RwTexture* texture) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*, RwTexture*))0x74DBC0)(material, texture);
}

RwInt32 RpMaterialRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return ((RwInt32(__cdecl *)(RwInt32, RwUInt32, RwPluginObjectConstructor, RwPluginObjectDestructor, RwPluginObjectCopy))0x74DBF0)(size, pluginID, constructCB, destructCB, copyCB);
}

RwInt32 RpMaterialRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkReadCallBack, RwPluginDataChunkWriteCallBack, RwPluginDataChunkGetSizeCallBack))0x74DC20)(pluginID, readCB, writeCB, getSizeCB);
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

RwUInt32 RpMaterialStreamGetSize(const RpMaterial* material) {
    return ((RwUInt32(__cdecl *)(const RpMaterial*))0x74E010)(material);
}

RpMaterial* RpMaterialStreamRead(RwStream* stream) {
    return ((RpMaterial*(__cdecl *)(RwStream*))0x74DD30)(stream);
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

// Registro de los hooks del grupo g25b en rpworld.cpp (propio: el sufijo evita chocar con otros registradores de este archivo).
void RpWorldInjectHooks25b() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpWorld");

    RH_ScopedGlobalInstall(RpClumpGetNumAtomics, 0x7498E0);
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

// 0x74E1B0
RpMaterialList* _rpMaterialListInitialize(RpMaterialList* matList) {
    matList->space        = 0;
    matList->materials    = nullptr;
    matList->numMaterials = 0;
    return matList;
}

RpMaterialList* _rpMaterialListDeinitialize(RpMaterialList* matList) {
    return ((RpMaterialList*(__cdecl *)(RpMaterialList*))0x74E150)(matList);
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

RwInt32 _rpMaterialListAppendMaterial(RpMaterialList* matList, RpMaterial* material) {
    return ((RwInt32(__cdecl *)(RpMaterialList*, RpMaterial*))0x74E350)(matList, material);
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

RwUInt32 _rpMaterialListStreamGetSize(const RpMaterialList* matList) {
    return ((RwUInt32(__cdecl *)(const RpMaterialList*))0x74E450)(matList);
}

RpMaterialList* _rpMaterialListStreamRead(RwStream* stream, RpMaterialList* matList) {
    return ((RpMaterialList*(__cdecl *)(RwStream*, RpMaterialList*))0x74E600)(stream, matList);
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

const RpMorphTarget* RpMorphTargetCalcBoundingSphere(const RpMorphTarget* morphTarget, RwSphere* boundingSphere) {
    return ((const RpMorphTarget*(__cdecl *)(const RpMorphTarget*, RwSphere*))0x74C200)(morphTarget, boundingSphere);
}

RwInt32 RpGeometryAddMorphTargets(RpGeometry* geometry, RwInt32 mtcount) {
    return ((RwInt32(__cdecl *)(RpGeometry*, RwInt32))0x74C310)(geometry, mtcount);
}

RwInt32 RpGeometryAddMorphTarget(RpGeometry* geometry) {
    return ((RwInt32(__cdecl *)(RpGeometry*))0x74C4D0)(geometry);
}

RpGeometry* RpGeometryRemoveMorphTarget(RpGeometry* geometry, RwInt32 morphTarget) {
    return ((RpGeometry*(__cdecl *)(RpGeometry*, RwInt32))0x74C4E0)(geometry, morphTarget);
}

const RpGeometry* RpGeometryTriangleSetVertexIndices(const RpGeometry* geometry, RpTriangle* triangle, RwUInt16 vert1, RwUInt16 vert2, RwUInt16 vert3) {
    return ((const RpGeometry*(__cdecl *)(const RpGeometry*, RpTriangle*, RwUInt16, RwUInt16, RwUInt16))0x74C690)(geometry, triangle, vert1, vert2, vert3);
}

RpGeometry* RpGeometryTriangleSetMaterial(RpGeometry* geometry, RpTriangle* triangle, RpMaterial* material) {
    return ((RpGeometry*(__cdecl *)(RpGeometry*, RpTriangle*, RpMaterial*))0x74C6C0)(geometry, triangle, material);
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

RpGeometry* RpGeometryForAllMaterials(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* data) {
    return ((RpGeometry*(__cdecl *)(RpGeometry*, RpMaterialCallBack, void*))0x74C790)(geometry, fpCallBack, data);
}

RpGeometry* RpGeometryLock(RpGeometry* geometry, RwInt32 lockMode) {
    return ((RpGeometry*(__cdecl *)(RpGeometry*, RwInt32))0x74C7D0)(geometry, lockMode);
}

RpGeometry* RpGeometryUnlock(RpGeometry* geometry) {
    return ((RpGeometry*(__cdecl *)(RpGeometry*))0x74C800)(geometry);
}

const RpGeometry* RpGeometryForAllMeshes(const RpGeometry* geometry, RpMeshCallBack fpCallBack, void* data) {
    return ((const RpGeometry*(__cdecl *)(const RpGeometry*, RpMeshCallBack, void*))0x74CA60)(geometry, fpCallBack, data);
}

RpGeometry* RpGeometryCreate(RwInt32 numVert, RwInt32 numTriangles, RwUInt32 format) {
    return ((RpGeometry*(__cdecl *)(RwInt32, RwInt32, RwUInt32))0x74CA90)(numVert, numTriangles, format);
}

RwBool RpGeometryDestroy(RpGeometry* geometry) {
    return ((RwBool(__cdecl *)(RpGeometry*))0x74CCC0)(geometry);
}

RwInt32 RpGeometryRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return ((RwInt32(__cdecl *)(RwInt32, RwUInt32, RwPluginObjectConstructor, RwPluginObjectDestructor, RwPluginObjectCopy))0x74CD70)(size, pluginID, constructCB, destructCB, copyCB);
}

RwInt32 RpGeometryRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkReadCallBack, RwPluginDataChunkWriteCallBack, RwPluginDataChunkGetSizeCallBack))0x74CDA0)(pluginID, readCB, writeCB, getSizeCB);
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

RpGeometry* RpGeometryStreamRead(RwStream* stream) {
    return ((RpGeometry*(__cdecl *)(RwStream*))0x74D190)(stream);
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

RpAtomic* AtomicDefaultRenderCallBack(RpAtomic* atomic) {
    return ((RpAtomic * (__cdecl *)(RpAtomic*))0x7491C0)(atomic);
}

void _rpAtomicResyncInterpolatedSphere(RpAtomic* atomic) {
    ((void(__cdecl *)(RpAtomic*))0x7491F0)(atomic);
}

const RwSphere* RpAtomicGetWorldBoundingSphere(RpAtomic* atomic) {
    return ((const RwSphere*(__cdecl *)(RpAtomic*))0x749330)(atomic);
}

void RpClumpGtaCancelStream() {
    plugin::Call<0x72E700>();
}

RpClump* RpClumpForAllAtomics(RpClump* clump, RpAtomicCallBack callback, void* data) {
    return ((RpClump*(__cdecl *)(RpClump*, RpAtomicCallBack, void*))0x749B70)(clump, callback, data);
}

RpClump* RpClumpForAllLights(RpClump* clump, RpLightCallBack callback, void* data) {
    return ((RpClump*(__cdecl *)(RpClump*, RpLightCallBack, void*))0x749C00)(clump, callback, data);
}

RpClump* RpClumpForAllCameras(RpClump* clump, RwCameraCallBack callback, void* data) {
    return ((RpClump*(__cdecl *)(RpClump*, RwCameraCallBack, void*))0x749BB0)(clump, callback, data);
}

RpAtomic* RpAtomicSetFrame(RpAtomic* atomic, RwFrame* frame) {
    return ((RpAtomic*(__cdecl *)(RpAtomic*, RwFrame*))0x74BF20)(atomic, frame);
}

RpClump* RpClumpCreateSpace(const RwV3d* position, RwReal radius) {
    return ((RpClump*(__cdecl *)(const RwV3d*, RwReal))0x749970)(position, radius);
}

RpClump* RpClumpRender(RpClump* clump) {
    return ((RpClump*(__cdecl *)(RpClump*))0x749B20)(clump);
}

RpClump* RpClumpRemoveAtomic(RpClump* clump, RpAtomic* atomic) {
    return ((RpClump*(__cdecl *)(RpClump*, RpAtomic*))0x74A4C0)(clump, atomic);
}

RpClump* RpClumpAddAtomic(RpClump* clump, RpAtomic* atomic) {
    return ((RpClump*(__cdecl *)(RpClump*, RpAtomic*))0x74A490)(clump, atomic);
}

RpClump* RpClumpRemoveLight(RpClump* clump, RpLight* light) {
    return ((RpClump*(__cdecl *)(RpClump*, RpLight*))0x74A520)(clump, light);
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

RwBool RpClumpDestroy(RpClump* clump) {
    return ((RwBool(__cdecl *)(RpClump*))0x74A310)(clump);
}

RpClump* RpClumpCreate(void) {
    return ((RpClump*(__cdecl *)(void))0x74A290)();
}

RpClump* RpClumpClone(RpClump* clump) {
    return ((RpClump*(__cdecl *)(RpClump*))0x749F70)(clump);
}

RwBool RpAtomicDestroy(RpAtomic* atomic) {
    return ((RwBool(__cdecl *)(RpAtomic*))0x749DC0)(atomic);
}

RpAtomic* RpAtomicClone(RpAtomic* atomic) {
    return ((RpAtomic*(__cdecl *)(RpAtomic*))0x749E60)(atomic);
}

RpAtomic* RpAtomicCreate(void) {
    return ((RpAtomic*(__cdecl *)(void))0x749C50)();
}

RpAtomic* RpAtomicSetGeometry(RpAtomic* atomic, RpGeometry* geometry, RwUInt32 flags) {
    return ((RpAtomic*(__cdecl *)(RpAtomic*, RpGeometry*, RwUInt32))0x749D40)(atomic, geometry, flags);
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

RpClump* RpClumpStreamRead(RwStream* stream) {
    return ((RpClump*(__cdecl *)(RwStream*))0x74B420)(stream);
}

RpClump* RpClumpStreamWrite(RpClump* clump, RwStream* stream) {
    return ((RpClump*(__cdecl *)(RpClump*, RwStream*))0x74AA10)(clump, stream);
}

RpClumpChunkInfo* _rpClumpChunkInfoRead(RwStream* stream, RpClumpChunkInfo* clumpChunkInfo, RwInt32* bytesRead) {
    return ((RpClumpChunkInfo*(__cdecl *)(RwStream*, RpClumpChunkInfo*, RwInt32*))0x74BD40)(stream, clumpChunkInfo, bytesRead);
}

RwInt32 RpAtomicRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return ((RwInt32(__cdecl *)(RwInt32, RwUInt32, RwPluginObjectConstructor, RwPluginObjectDestructor, RwPluginObjectCopy))0x74BDA0)(size, pluginID, constructCB, destructCB, copyCB);
}

RwInt32 RpClumpRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return ((RwInt32(__cdecl *)(RwInt32, RwUInt32, RwPluginObjectConstructor, RwPluginObjectDestructor, RwPluginObjectCopy))0x74BDD0)(size, pluginID, constructCB, destructCB, copyCB);
}

RwInt32 RpAtomicRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkReadCallBack, RwPluginDataChunkWriteCallBack, RwPluginDataChunkGetSizeCallBack))0x74BE00)(pluginID, readCB, writeCB, getSizeCB);
}

// 0x74BE30
RwInt32 RpAtomicSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return _rwPluginRegistryAddPlgnStrmlwysCB(&s_atomicTKList, pluginID, alwaysCB);
}

// 0x74BE50
RwInt32 RpAtomicSetStreamRightsCallBack(RwUInt32 pluginID, RwPluginDataChunkRightsCallBack rightsCB) {
    return _rwPluginRegistryAddPlgnStrmRightsCB(&s_atomicTKList, pluginID, rightsCB);
}

RwInt32 RpClumpRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkReadCallBack, RwPluginDataChunkWriteCallBack, RwPluginDataChunkGetSizeCallBack))0x74BE70)(pluginID, readCB, writeCB, getSizeCB);
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

void _rpWorldSectorDeinstanceAll(RpSector* sector) {
    ((void(__cdecl *)(RpSector*))0x74ECA0)(sector);
}

void _rpWorldSectorDestroyRecurse(RpSector* sector) {
    ((void(__cdecl *)(RpSector*))0x74ED50)(sector);
}

RwBool _rpWorldForAllGlobalLights(RpLightCallBack callBack, void* data) {
    return ((RwBool(__cdecl *)(RpLightCallBack, void*))0x74EF10)(callBack, data);
}

RpWorldSector* _rpWorldSectorForAllLocalLights(RpWorldSector* sector, RpLightCallBack callBack, void* data) {
    return ((RpWorldSector*(__cdecl *)(RpWorldSector*, RpLightCallBack, void*))0x74EF60)(sector, callBack, data);
}

RpWorldSector* _rpSectorDefaultRenderCallBack(RpWorldSector* sector) {
    return ((RpWorldSector*(__cdecl *)(RpWorldSector*))0x74EEC0)(sector);
}

RpWorld* RpWorldLock(RpWorld* world) {
    return ((RpWorld*(__cdecl *)(RpWorld*))0x74F1A0)(world);
}

RpWorld* RpWorldUnlock(RpWorld* world) {
    return ((RpWorld*(__cdecl *)(RpWorld*))0x74F210)(world);
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

RpWorld* RpWorldForAllMaterials(RpWorld* world, RpMaterialCallBack fpCallBack, void* data) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RpMaterialCallBack, void*))0x74FBC0)(world, fpCallBack, data);
}

RpWorld* RpWorldForAllLights(RpWorld* world, RpLightCallBack fpCallBack, void* data) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RpLightCallBack, void*))0x74FC00)(world, fpCallBack, data);
}

RpWorld* RpWorldForAllWorldSectors(RpWorld* world, RpWorldSectorCallBack fpCallBack, void* data) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RpWorldSectorCallBack, void*))0x74FC70)(world, fpCallBack, data);
}

RpWorld* RpWorldRender(RpWorld* world) {
    return ((RpWorld*(__cdecl *)(RpWorld*))0x74F570)(world);
}

RwBool RpWorldDestroy(RpWorld* world) {
    return ((RwBool(__cdecl *)(RpWorld*))0x74F610)(world);
}

RpWorld* RpWorldCreate(RwBBox* boundingBox) {
    return ((RpWorld*(__cdecl *)(RwBBox*))0x74F760)(boundingBox);
}

RpWorld* RpWorldSetSectorRenderCallBack(RpWorld* world, RpWorldSectorCallBackRender fpCallBack) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RpWorldSectorCallBackRender))0x74F730)(world, fpCallBack);
}

RpWorldSectorCallBackRender RpWorldGetSectorRenderCallBack(const RpWorld* world) {
    return ((RpWorldSectorCallBackRender(__cdecl *)(const RpWorld*))0x74F750)(world);
}

RpWorld* RpWorldSectorGetWorld(const RpWorldSector* sector) {
    return ((RpWorld*(__cdecl *)(const RpWorldSector*))0x74F4E0)(sector);
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

void _rpD3D9AddDynamicGeometry(RpGeometry* geometry) {
    ((void(__cdecl *)(RpGeometry*))0x74E970)(geometry);
}

void _rpD3D9RemoveDynamicGeometry(RpGeometry* geometry) {
    ((void(__cdecl *)(RpGeometry*))0x74E9E0)(geometry);
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

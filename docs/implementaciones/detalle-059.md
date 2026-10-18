<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1161"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| _rwD3D9RenderStateVertexAlphaEnable(RwBool enable) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp:32](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1162"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float x) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:42](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FildMul(int32 n, float f) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:47](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FixedScale(const uint8* data, size_t i) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:52](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcEnvCamUV(RpAtomic* atomic, const uint8* matData, float* out) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:57](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnvWaveCoord(float p, float s, float rs, int32 n) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:73](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L73) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcEnvWaveUV(RpAtomic* atomic, uint8* matData, CustomEnvMapPipeAtomicData* atmData, float* out) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:79](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetEnvMapMaterial(const RwSurfaceProperties* surfProps, const RwRGBA* color, RwUInt32 flags, float spec, float power) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:151](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L151) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AmbientSaturated: source/game_sa/RenderWare/RenderWare.h:27 (0x008E2418); Posible dato fijo gEnvMapD3DMaterial: source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:35 (0x008D12D8) |
| * CreateEnvMapPool(int32 size) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:215](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L215) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DestroyEnvMapPool(Pool* pool) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:235](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L235) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsEnvironmentMappingSupported() | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:329](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L329) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1163"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * Raw(CustomEnvMapPipeMaterialData* data) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:12](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FromFixed8(uint8 b) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:17](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FromFixed255(uint8 b) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:22](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ToFixed8(float x) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:27](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ToFixed255(float x) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:32](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1164"></a>

## Globales y helpers: source/game_sa/Pipelines/PipelinesCommon.hpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetTextureStagesForLighting(bool isGeoTextured, RwTexture* tex) | [source/game_sa/Pipelines/PipelinesCommon.hpp:8](../../source/game_sa/Pipelines/PipelinesCommon.hpp#L8) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RxD3D9InstanceDataRender(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* mesh) | [source/game_sa/Pipelines/PipelinesCommon.hpp:28](../../source/game_sa/Pipelines/PipelinesCommon.hpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RxD3D9InstanceDataRenderLighting(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* mesh, uint32 rxGeoFlags, RwTexture* tex) | [source/game_sa/Pipelines/PipelinesCommon.hpp:40](../../source/game_sa/Pipelines/PipelinesCommon.hpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1165"></a>

## Globales y helpers: source/game_sa/Plant/PlantLocTri.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetPlantDensity_EaxEntry() | [source/game_sa/Plant/PlantLocTri.cpp:39](../../source/game_sa/Plant/PlantLocTri.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1166"></a>

## Globales y helpers: source/game_sa/Plant/ProcSurfaceInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProcSurfaceRandomInRange(float min, float max) | [source/game_sa/Plant/ProcSurfaceInfo.cpp:97](../../source/game_sa/Plant/ProcSurfaceInfo.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1167"></a>

## Globales y helpers: source/game_sa/PlantMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AtomicCreatePrelitIfNeeded(RpAtomic* atomic) | [source/game_sa/PlantMgr.cpp:32](../../source/game_sa/PlantMgr.cpp#L32) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x005DD100: fuera del catálogo consultado; ruta efectiva sin resolver |
| GeometrySetPrelitConstantColor(RpGeometry* geometry, RwRGBA color) | [source/game_sa/PlantMgr.cpp:37](../../source/game_sa/PlantMgr.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1168"></a>

## Globales y helpers: source/game_sa/PlayerInfoProcess.cpp.inc

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProcessPlayerStunts(CPlayerInfo& p) | [source/game_sa/PlayerInfoProcess.cpp.inc:4](../../source/game_sa/PlayerInfoProcess.cpp.inc#L4) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| ProcessPlayerVehicleInteraction(CPlayerInfo& p, uint32 playerIndex, CPad* pad) | [source/game_sa/PlayerInfoProcess.cpp.inc:61](../../source/game_sa/PlayerInfoProcess.cpp.inc#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x006919C0: fuera del catálogo consultado; ruta efectiva sin resolver; Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo bPlayersCanBeInSeparateCars: source/game_sa/GameLogic.h:52 (0x0096A8B3); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |

<a id="grupo-1169"></a>

## Globales y helpers: source/game_sa/PlayerRelationshipRecorder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetPlayerRelationshipRecorder() | [source/game_sa/PlayerRelationshipRecorder.cpp:125](../../source/game_sa/PlayerRelationshipRecorder.cpp#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1170"></a>

## Globales y helpers: source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * ClumpCollisionConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:38](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:44](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:50](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:63](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x0041B1D0: fuera del catálogo consultado; ruta efectiva sin resolver; Posible dato fijo PC_Scratch: source/game_sa/common.h:72 (0x00C8E0C8); Posible dato fijo ms_currentModel: source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.h:25 (0x009689E0) |
| * ClumpCollisionStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:92](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClumpCollisionGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:98](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1171"></a>

## Globales y helpers: source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| JPegErrorExit(j_common_ptr cinfo) | [source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp:27](../../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1172"></a>

## Globales y helpers: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * NodeNameConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:51](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |
| * NodeNameDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:60](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NodeNameCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:67](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |
| * NodeNameStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:75](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |
| * NodeNameStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:83](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |
| NodeNameStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:90](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L90) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |

<a id="grupo-1173"></a>

## Globales y helpers: source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * PipelineConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:50](../../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gPipelinePluginOffset: source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:25 (0x008D6080) |
| * PipelineCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:59](../../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gPipelinePluginOffset: source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:25 (0x008D6080) |
| * PipelineStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:67](../../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gPipelinePluginOffset: source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:25 (0x008D6080) |
| PipelineGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:74](../../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1174"></a>

## Globales y helpers: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| *& RpAnimBlendClumpGetData(RpClump* clump) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:28](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ClumpOffset: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:25 (0x00B5F878) |
| FrameInitCBskin(AnimBlendFrameData* fd, void* data) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:141](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpAnimBlendClumpInitSkinned(RpClump* clump) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:153](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L153) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FrameCountCB(RwFrame* f, void* data) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:184](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L184) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SetFrameDataFrameCB(RwFrame* f, void* data) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:191](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L191) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameInitCBnonskin(AnimBlendFrameData* fd, void* data) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:200](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L200) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpAnimBlendClumpInitNonSkinned(RpClump* clump) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:212](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L212) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & RpAnimBlendClumpGetAssociations(RpClump* clump) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:254](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L254) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ConvertPedNode2BoneTag(ePedNode pedNode) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:261](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L261) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ConvertBoneTag2BoneName(eBoneTag32 boneTag) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:286](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L286) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FillFrameArrayIndiciesNonSkinnedCB(AnimBlendFrameData* fd, void* data) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:348](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L348) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameFindByBoneTagCB(AnimBlendFrameData* fd, void* data) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:371](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L371) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gpAnimBlendFrameData: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:368 (0x00B5F87C) |
| FrameFindByNameCBskin(AnimBlendFrameData* fd, void* data) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:387](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L387) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gpAnimBlendFrameData: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:368 (0x00B5F87C) |
| FrameFindByNameCBnonskin(AnimBlendFrameData* fd, void* data) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:395](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L395) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gpAnimBlendFrameData: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:368 (0x00B5F87C) |
| FrameFindByKeyCBskin(AnimBlendFrameData* fd, void* data) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:416](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L416) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gpAnimBlendFrameData: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:368 (0x00B5F87C) |
| FrameFindByKeyCBnonskin(AnimBlendFrameData* fd, void* data) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:424](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L424) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gpAnimBlendFrameData: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:368 (0x00B5F87C) |
| * RpAnimBlendClumpFindAssociationIf_N(RpClump* clump, Fn&& Pred, size_t n) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:445](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L445) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpAnimBlendClumpGetAssociation(RpClump* clump, bool, CAnimBlendHierarchy* h) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:461](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L461) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::RpAnimBlendClumpGetAssociation-Hier 0x004D68E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpAnimBlendClumpGetAssociation(RpClump* clump, const char* name) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:468](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L468) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::RpAnimBlendClumpGetAssociation-AnimName 0x004D6870 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpAnimBlendClumpGetAssociation(RpClump* clump, uint32 animId) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:476](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L476) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::RpAnimBlendClumpGetAssociation-AnimId 0x004D68B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpAnimBlendClumpGetFirstAssociation(RpClump* clump) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:483](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L483) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::RpAnimBlendClumpGetFirstAssociation- 0x004D15E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpAnimBlendClumpGetFirstAssociation(RpClump* clump, uint32 flags) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:492](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L492) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::RpAnimBlendClumpGetFirstAssociation-Flags 0x004D6A70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpAnimBlendClumpCountAssociationsIf(RpClump* clump, Fn&& Pred) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:598](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L598) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeAdd(float a, float b) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:693](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L693) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeSub(float a, float b) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:694](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L694) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeMul(float a, float b) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:695](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L695) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalculateTotalBlendOfPartial(AnimBlendUpdateData* c, AnimBlendFrameData* fd, bool checkDontAddToBlend) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:697](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L697) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NormaliseQuatExe(CQuaternion* quat) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:724](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L724) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameUpdateCallBackNoVelocity(AnimBlendFrameData* fd, AnimBlendUpdateData* c) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:736](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L736) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameUpdateCallBackExeVelocity(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:837](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L837) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gpAnimBlendClump: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:688 (0x00B4EA0C) |
| FrameUpdateCallBackWithVelocityExtractionCompressedSkinned(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1019](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1019) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameUpdateCallBackWithVelocityExtractionCompressedNonSkinned(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1038](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1038) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameUpdateCallBackSkinnedWith3dVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1057](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1057) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameUpdateCallBackSkinnedWithVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1063](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1063) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameUpdateCallBackNonSkinnedWith3dVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1086](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1086) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameUpdateCallBackNonSkinnedWithVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1092](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1092) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpAnimBlendNodeUpdateKeyFrames(AnimBlendUpdateData* c, AnimBlendFrameData* frames, uint32 nFrames) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1124](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gpAnimBlendClump: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:688 (0x00B4EA0C) |
| * RpAnimBlendGetNextAssociation(CAnimBlendAssociation* association) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1267](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1267) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::RpAnimBlendGetNextAssociation-Any 0x004D6AB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpAnimBlendGetNextAssociation(CAnimBlendAssociation* association, uint32 flags) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1275](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1275) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::RpAnimBlendGetNextAssociation-Flags 0x004D6AD0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1175"></a>

## Globales y helpers: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RpAnimBlendClumpForEachAssociation(RpClump* clump, Fn&& IterFn) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.h:51](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.h#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1176"></a>

## Globales y helpers: source/game_sa/PointLights.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FogEmitSprite(const CPointLight& light, const float& alpha, const RwV3d& screen, float w, float h, int32 tab, uint32 timeValue, bool pointLight) | [source/game_sa/PointLights.cpp:156](../../source/game_sa/PointLights.cpp#L156) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FogDirectionalLight(const CPointLight& light, const float& intensity) | [source/game_sa/PointLights.cpp:164](../../source/game_sa/PointLights.cpp#L164) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FogPointLight(const CPointLight& light, const float& intensity, const float& size) | [source/game_sa/PointLights.cpp:196](../../source/game_sa/PointLights.cpp#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:40 (0x00B6F028) |

<a id="grupo-1177"></a>

## Globales y helpers: source/game_sa/Pools.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * ConstructPool(int32 capacity, uint32 elementSize) | [source/game_sa/Pools.cpp:132](../../source/game_sa/Pools.cpp#L132) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DestroyPool(void* poolPtr) | [source/game_sa/Pools.cpp:154](../../source/game_sa/Pools.cpp#L154) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetPedPool() | [source/game_sa/Pools.cpp:504](../../source/game_sa/Pools.cpp#L504) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPedPool: source/game_sa/Pools.cpp:64 (0x00B74490) |
| * GetVehiclePool() | [source/game_sa/Pools.cpp:509](../../source/game_sa/Pools.cpp#L509) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pVehiclePool: source/game_sa/Pools.cpp:65 (0x00B74494) |
| * GetBuildingPool() | [source/game_sa/Pools.cpp:514](../../source/game_sa/Pools.cpp#L514) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pBuildingPool: source/game_sa/Pools.cpp:66 (0x00B74498) |
| * GetObjectPool() | [source/game_sa/Pools.cpp:519](../../source/game_sa/Pools.cpp#L519) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pObjectPool: source/game_sa/Pools.cpp:67 (0x00B7449C) |
| * GetDummyPool() | [source/game_sa/Pools.cpp:524](../../source/game_sa/Pools.cpp#L524) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pDummyPool: source/game_sa/Pools.cpp:68 (0x00B744A0) |
| * GetColModelPool() | [source/game_sa/Pools.cpp:529](../../source/game_sa/Pools.cpp#L529) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pColModelPool: source/game_sa/Pools.cpp:69 (0x00B744A4) |
| * GetTaskPool() | [source/game_sa/Pools.cpp:534](../../source/game_sa/Pools.cpp#L534) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pTaskPool: source/game_sa/Pools.cpp:70 (0x00B744A8) |
| * GetPedIntelligencePool() | [source/game_sa/Pools.cpp:539](../../source/game_sa/Pools.cpp#L539) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPedIntelligencePool: source/game_sa/Pools.cpp:71 (0x00B744C0) |
| * GetPtrNodeSingleLinkPool() | [source/game_sa/Pools.cpp:544](../../source/game_sa/Pools.cpp#L544) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPtrNodeSingleLinkPool: source/game_sa/Pools.cpp:72 (0x00B74484) |
| * GetPtrNodeDoubleLinkPool() | [source/game_sa/Pools.cpp:549](../../source/game_sa/Pools.cpp#L549) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPtrNodeDoubleLinkPool: source/game_sa/Pools.cpp:73 (0x00B74488) |
| * GetEntryInfoNodePool() | [source/game_sa/Pools.cpp:554](../../source/game_sa/Pools.cpp#L554) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pEntryInfoNodePool: source/game_sa/Pools.cpp:74 (0x00B7448C) |
| * GetPointRoutePool() | [source/game_sa/Pools.cpp:559](../../source/game_sa/Pools.cpp#L559) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPointRoutePool: source/game_sa/Pools.cpp:75 (0x00B744B0) |
| * GetPatrolRoutePool() | [source/game_sa/Pools.cpp:564](../../source/game_sa/Pools.cpp#L564) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPatrolRoutePool: source/game_sa/Pools.cpp:76 (0x00B744B4) |
| * GetEventPool() | [source/game_sa/Pools.cpp:569](../../source/game_sa/Pools.cpp#L569) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pEventPool: source/game_sa/Pools.cpp:77 (0x00B744AC) |
| * GetNodeRoutePool() | [source/game_sa/Pools.cpp:574](../../source/game_sa/Pools.cpp#L574) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pNodeRoutePool: source/game_sa/Pools.cpp:78 (0x00B744B8) |
| * GetTaskAllocatorPool() | [source/game_sa/Pools.cpp:579](../../source/game_sa/Pools.cpp#L579) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pTaskAllocatorPool: source/game_sa/Pools.cpp:79 (0x00B744BC) |
| * GetPedAttractorPool() | [source/game_sa/Pools.cpp:584](../../source/game_sa/Pools.cpp#L584) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPedAttractorPool: source/game_sa/Pools.cpp:80 (0x00B744C4) |

<a id="grupo-1178"></a>

## Globales y helpers: source/game_sa/Population.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LoadGroupFile(const char* fileName, auto& outModelsInGroup, auto& outNumOfModelsPerGroup) | [source/game_sa/Population.cpp:170](../../source/game_sa/Population.cpp#L170) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessPossibleDriverModelsOfCar(eModelID carModelIndex, auto&& ProcessModel) | [source/game_sa/Population.cpp:609](../../source/game_sa/Population.cpp#L609) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PopulationTask(size_t size, Args... args) | [source/game_sa/Population.cpp:1910](../../source/game_sa/Population.cpp#L1910) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PopulationModelRadius(eModelID model) | [source/game_sa/Population.cpp:1914](../../source/game_sa/Population.cpp#L1914) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PopulationModelLoaded(eModelID model) | [source/game_sa/Population.cpp:1917](../../source/game_sa/Population.cpp#L1917) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PopulationCouple(eModelID maleModel, eModelID femaleModel, CVector position) | [source/game_sa/Population.cpp:1920](../../source/game_sa/Population.cpp#L1920) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:40 (0x00B6F028); Posible dato fijo ms_offsets: source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.h:24 (0x00C196E8) |
| PopulationDefaultTask(CPed* ped,ePedType type) | [source/game_sa/Population.cpp:1958](../../source/game_sa/Population.cpp#L1958) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x0048E610: CTaskComplexWanderCriminal::Constructor; ruta efectiva sin resolver; Colaborador por dirección 0x005A1D70: fuera del catálogo consultado; ruta efectiva sin resolver; Colaborador por dirección 0x00681B60: CTaskManager::SetTaskSecondary; ruta efectiva sin resolver; Conversión de dirección fija: 0x00A90CF0; Posible dato fijo MI_TELLY: source/game_sa/ModelIndices.cpp:69 (0x008CD5A8) |
| PopulationSunbathe(CPed* ped,const CVector& position) | [source/game_sa/Population.cpp:1976](../../source/game_sa/Population.cpp#L1976) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x0043E0C0: CPlaceable::SetHeading; ruta efectiva sin resolver; Colaborador por dirección 0x00632140: CTaskComplexSunbathe::CanSunbathe; ruta efectiva sin resolver; Colaborador por dirección 0x006EABA0: fuera del catálogo consultado; ruta efectiva sin resolver; Conversión de dirección fija: 0x00969159; Posible dato fijo g_surfaceInfos: source/game_sa/SurfaceInfos_c.h:79 (0x00B79538) |

<a id="grupo-1179"></a>

## Globales y helpers: source/game_sa/PostEffects.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeLog2TruncViaDouble(int32 value) | [source/game_sa/PostEffects.cpp:39](../../source/game_sa/PostEffects.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeLog2TruncKeepExtended(int32 value) | [source/game_sa/PostEffects.cpp:44](../../source/game_sa/PostEffects.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePow2Above(int32 exponentPlusOne) | [source/game_sa/PostEffects.cpp:50](../../source/game_sa/PostEffects.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x008220F0 |
| HazeScaleX(float k) | [source/game_sa/PostEffects.cpp:57](../../source/game_sa/PostEffects.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| HazeScaleY(float k) | [source/game_sa/PostEffects.cpp:58](../../source/game_sa/PostEffects.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| RippleWave(uint32 timeMs, float speed, float freq, float y, float xoffset) | [source/game_sa/PostEffects.cpp:61](../../source/game_sa/PostEffects.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RippleFinalU(uint32 timeMs, float speed, float freq, float y, float xoffset, int32 width, float* outU0, float* outU1) | [source/game_sa/PostEffects.cpp:67](../../source/game_sa/PostEffects.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FogSinCoord(float angle, float quarter, float radius, float half, float third) | [source/game_sa/PostEffects.cpp:72](../../source/game_sa/PostEffects.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FogCosCoord(float angle, float quarter, float radius, float half, float third) | [source/game_sa/PostEffects.cpp:75](../../source/game_sa/PostEffects.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeClampScaleChannel(uint8 channel, float mult) | [source/game_sa/PostEffects.cpp:80](../../source/game_sa/PostEffects.cpp#L80) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1180"></a>

## Globales y helpers: source/game_sa/ProjectileInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProjectileCreatorHeading(CEntity* creator) | [source/game_sa/ProjectileInfo.cpp:97](../../source/game_sa/ProjectileInfo.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CProjectileInfo::AddProjectile 0x00737C80 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProjectileThrownVelocity(float heading, float speed, float height) | [source/game_sa/ProjectileInfo.cpp:104](../../source/game_sa/ProjectileInfo.cpp#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & ProjectileField(CEntity* entity, size_t offset) | [source/game_sa/ProjectileInfo.cpp:246](../../source/game_sa/ProjectileInfo.cpp#L246) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProjectileDamping(float timestep) | [source/game_sa/ProjectileInfo.cpp:249](../../source/game_sa/ProjectileInfo.cpp#L249) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RemoveBackwardProjectileSteering(CVector& steering,const CVector& direction) | [source/game_sa/ProjectileInfo.cpp:256](../../source/game_sa/ProjectileInfo.cpp#L256) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddProjectileSteering(CVector& velocity, const CVector& steering, float acceleration, float timestep, bool fast) | [source/game_sa/ProjectileInfo.cpp:259](../../source/game_sa/ProjectileInfo.cpp#L259) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LimitProjectileSpeed(CProjectile* projectile) | [source/game_sa/ProjectileInfo.cpp:263](../../source/game_sa/ProjectileInfo.cpp#L263) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

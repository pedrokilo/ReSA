<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1321"></a>

## Globales y helpers: source/game_sa/Pickup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PickupIsVisibleKeepEdx(CPickup* self, uint32 entryEdx) | [source/game_sa/Pickup.cpp:32](../../source/game_sa/Pickup.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1322"></a>

## Globales y helpers: source/game_sa/Pickups.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PickupDistanceTo(const CPickup& pickup, const CVector& point) | [source/game_sa/Pickups.cpp:41](../../source/game_sa/Pickups.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PickupGlowByte(CEntity* entity, float distance, uint32 mask, float angleStep, float radius, float inverseRadius, float scale) | [source/game_sa/Pickups.cpp:201](../../source/game_sa/Pickups.cpp#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1323"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AtomicHasNVCPipeline(RpAtomic* atomic) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:142](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L142) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ObjPipeline: source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.h:26 (0x00C02C1C), source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.h:5 (0x00C02C68), source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:147 (0x00C02D24) |
| * NewEnvMapPipeMatData(CustomEnvMapPipeMaterialDataPool* pool) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:293](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L293) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CCustomBuildingDNPipeline::SetFxEnvTexture 0x005D9570 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1324"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| _rwD3D9RenderStateVertexAlphaEnable(RwBool enable) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp:32](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1325"></a>

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

<a id="grupo-1326"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * Raw(CustomEnvMapPipeMaterialData* data) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:12](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FromFixed8(uint8 b) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:17](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FromFixed255(uint8 b) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:22](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ToFixed8(float x) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:27](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ToFixed255(float x) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:32](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1327"></a>

## Globales y helpers: source/game_sa/Pipelines/PipelinesCommon.hpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetTextureStagesForLighting(bool isGeoTextured, RwTexture* tex) | [source/game_sa/Pipelines/PipelinesCommon.hpp:8](../../source/game_sa/Pipelines/PipelinesCommon.hpp#L8) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RxD3D9InstanceDataRender(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* mesh) | [source/game_sa/Pipelines/PipelinesCommon.hpp:28](../../source/game_sa/Pipelines/PipelinesCommon.hpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RxD3D9InstanceDataRenderLighting(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* mesh, uint32 rxGeoFlags, RwTexture* tex) | [source/game_sa/Pipelines/PipelinesCommon.hpp:40](../../source/game_sa/Pipelines/PipelinesCommon.hpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1328"></a>

## Globales y helpers: source/game_sa/Plant/PlantLocTri.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetPlantDensity_EaxEntry() | [source/game_sa/Plant/PlantLocTri.cpp:39](../../source/game_sa/Plant/PlantLocTri.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1329"></a>

## Globales y helpers: source/game_sa/Plant/ProcSurfaceInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProcSurfaceRandomInRange(float min, float max) | [source/game_sa/Plant/ProcSurfaceInfo.cpp:97](../../source/game_sa/Plant/ProcSurfaceInfo.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1330"></a>

## Globales y helpers: source/game_sa/PlantMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AtomicCreatePrelitIfNeeded(RpAtomic* atomic) | [source/game_sa/PlantMgr.cpp:32](../../source/game_sa/PlantMgr.cpp#L32) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x005DD100: fuera del catálogo consultado; ruta efectiva sin resolver |
| GeometrySetPrelitConstantColor(RpGeometry* geometry, RwRGBA color) | [source/game_sa/PlantMgr.cpp:37](../../source/game_sa/PlantMgr.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1331"></a>

## Globales y helpers: source/game_sa/PlayerInfoProcess.cpp.inc

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProcessPlayerStunts(CPlayerInfo& p) | [source/game_sa/PlayerInfoProcess.cpp.inc:4](../../source/game_sa/PlayerInfoProcess.cpp.inc#L4) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| ProcessPlayerVehicleInteraction(CPlayerInfo& p, uint32 playerIndex, CPad* pad) | [source/game_sa/PlayerInfoProcess.cpp.inc:61](../../source/game_sa/PlayerInfoProcess.cpp.inc#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x006919C0: fuera del catálogo consultado; ruta efectiva sin resolver; Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo bPlayersCanBeInSeparateCars: source/game_sa/GameLogic.h:52 (0x0096A8B3); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |

<a id="grupo-1332"></a>

## Globales y helpers: source/game_sa/PlayerRelationshipRecorder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Relationship6GetActive() | [source/game_sa/PlayerRelationshipRecorder.cpp:33](../../source/game_sa/PlayerRelationshipRecorder.cpp#L33) | Ensamblador · Envoltura al EXE | Histórica del catálogo: CPlayerRelationshipRecorder::RecordRelationshipWithPlayer 0x0061A1D0 | 1 / 1 | Trampolín ASM a 0x00681720; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Relationship6Kill() | [source/game_sa/PlayerRelationshipRecorder.cpp:39](../../source/game_sa/PlayerRelationshipRecorder.cpp#L39) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x006985E0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Relationship6Follow() | [source/game_sa/PlayerRelationshipRecorder.cpp:45](../../source/game_sa/PlayerRelationshipRecorder.cpp#L45) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00698610; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| & GetPlayerRelationshipRecorder() | [source/game_sa/PlayerRelationshipRecorder.cpp:163](../../source/game_sa/PlayerRelationshipRecorder.cpp#L163) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1333"></a>

## Globales y helpers: source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BreakablePluginAttach() | [source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp:43](../../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_BreakablePlugin: source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.h:11 (0x00BB4238) |
| * BreakableDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp:75](../../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_BreakablePlugin: source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.h:11 (0x00BB4238) |
| * BreakableStreamRead(RwStream* stream, int binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp:99](../../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp#L99) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_BreakablePlugin: source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.h:11 (0x00BB4238) |

<a id="grupo-1334"></a>

## Globales y helpers: source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * ClumpCollisionConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:38](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:44](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:50](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:63](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x0041B1D0: fuera del catálogo consultado; ruta efectiva sin resolver; Posible dato fijo PC_Scratch: source/game_sa/common.h:72 (0x00C8E0C8); Posible dato fijo ms_currentModel: source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.h:25 (0x009689E0) |
| * ClumpCollisionStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:92](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClumpCollisionGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:98](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1335"></a>

## Globales y helpers: source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| JPegErrorExit(j_common_ptr cinfo) | [source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp:27](../../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| JPegCompressScreen(RwCamera* camera, jpeg_destination_mgr& dst) | [source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp:55](../../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| JPegCompressScreenToFile(RwCamera* camera, const char* path) | [source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp:95](../../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1336"></a>

## Globales y helpers: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * NodeNameConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:51](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |
| * NodeNameDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:60](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NodeNameCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:67](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |
| * NodeNameStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:75](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |
| * NodeNameStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:83](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |
| NodeNameStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:90](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L90) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |
| NodeNamePluginAttach() | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:100](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L100) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |
| * GetFrameNodeName(RwFrame* frame) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:111](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |
| SetFrameNodeName(RwFrame* frame, const RwChar* name) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:119](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |

<a id="grupo-1337"></a>

## Globales y helpers: source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * PipelineConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:50](../../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gPipelinePluginOffset: source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:25 (0x008D6080) |
| * PipelineCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:59](../../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gPipelinePluginOffset: source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:25 (0x008D6080) |
| * PipelineStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:67](../../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gPipelinePluginOffset: source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:25 (0x008D6080) |
| PipelineGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:74](../../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1338"></a>

## Globales y helpers: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| *& RpAnimBlendClumpGetData(RpClump* clump) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:28](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo ClumpOffset: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:25 (0x00B5F878) |
| * ClumpAnimDestruct(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:47](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpAnimBlendPluginAttach() | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:99](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ClumpOffset: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:25 (0x00B5F878) |
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
| CalculateTotalBlendOfPartial(AnimBlendUpdateData* c, AnimBlendFrameData* fd, bool checkDontAddToBlend) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:690](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L690) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NormaliseQuatExe(CQuaternion* quat) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:717](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L717) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameUpdateCallBackNoVelocity(AnimBlendFrameData* fd, AnimBlendUpdateData* c) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:729](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L729) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameUpdateCallBackWithVelocityExtractionCompressedSkinned(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:817](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L817) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | Posible dato fijo gpAnimBlendClump: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:688 (0x00B4EA0C); ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FrameUpdateCallBackWithVelocityExtractionCompressedNonSkinned(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:923](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L923) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | Posible dato fijo gpAnimBlendClump: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:688 (0x00B4EA0C); ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FrameUpdateCallBackSkinnedWith3dVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1035](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1035) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | Posible dato fijo gpAnimBlendClump: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:688 (0x00B4EA0C); ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FrameUpdateCallBackSkinnedWithVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1132](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1132) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | Posible dato fijo gpAnimBlendClump: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:688 (0x00B4EA0C); ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FrameUpdateCallBackNonSkinnedWith3dVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1266](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1266) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | Posible dato fijo gpAnimBlendClump: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:688 (0x00B4EA0C); ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FrameUpdateCallBackNonSkinnedWithVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1365](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1365) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | Posible dato fijo gpAnimBlendClump: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:688 (0x00B4EA0C); ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FrameUpdateCallBackT(AnimBlendFrameData* fd, AnimBlendUpdateData* c) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1523](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1523) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gpAnimBlendClump: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:688 (0x00B4EA0C) |
| FrameUpdateCallBackW(AnimBlendFrameData* fd, void* data) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1707](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1707) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gpAnimBlendClump: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:688 (0x00B4EA0C) |
| FrameUpdateCallBackOffscreen(AnimBlendFrameData* fd, void* data) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1738](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1738) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gpAnimBlendClump: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:688 (0x00B4EA0C) |
| RpAnimBlendNodeUpdateKeyFrames(AnimBlendUpdateData* c, AnimBlendFrameData* frames, uint32 nFrames) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1751](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1751) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gpAnimBlendClump: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:688 (0x00B4EA0C) |
| * RpAnimBlendGetNextAssociation(CAnimBlendAssociation* association) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1889](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1889) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::RpAnimBlendGetNextAssociation-Any 0x004D6AB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpAnimBlendGetNextAssociation(CAnimBlendAssociation* association, uint32 flags) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1897](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1897) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::RpAnimBlendGetNextAssociation-Flags 0x004D6AD0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1339"></a>

## Globales y helpers: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RpAnimBlendClumpForEachAssociation(RpClump* clump, Fn&& IterFn) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.h:51](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.h#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1340"></a>

## Globales y helpers: source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RpGeometryGet2dFxCount(RpGeometry* geometry) | [source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp:165](../../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp#L165) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpGeometryGet2dFxAtIndex(RpGeometry* geometry, int32 iEffectInd) | [source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp:174](../../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp#L174) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * t2dEffectPluginConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp:180](../../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp#L180) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * t2dEffectPluginDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp:190](../../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * t2dEffectPluginCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp:209](../../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp#L209) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Rwt2dEffectPluginDataChunkReadCallBack(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp:222](../../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp#L222) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g2dEffectPluginOffset: source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.h:268 (0x00C3A1E0); Posible dato fijo ms_nTxdSlot: source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.h:267 (0x008D4948) |
| * Rwt2dEffectPluginDataChunkWriteCallBack(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp:424](../../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp#L424) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Rwt2dEffectPluginDataChunkGetSizeCallBack(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp:429](../../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp#L429) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/11/2026 (Ciudad de México):** 996 grupos / 12438 funciones reconstruidas; 11156 con evidencia histórica y 1282 pendientes de validación en 142 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1301"></a>

## Globales y helpers: source/game_sa/PedIK.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SinCosExe(float angle, float& outSin, float& outCos) | [source/game_sa/PedIK.cpp:45](../../source/game_sa/PedIK.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RotateXExe(CMatrix& matrix, float angle) | [source/game_sa/PedIK.cpp:50](../../source/game_sa/PedIK.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RotateYExe(CMatrix& matrix, float angle) | [source/game_sa/PedIK.cpp:54](../../source/game_sa/PedIK.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1302"></a>

## Globales y helpers: source/game_sa/PedIntelligence.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IncrementAngerAtPlayerRegs(CPedIntelligence* self, uint32 entryEdx, uint8 anger) | [source/game_sa/PedIntelligence.cpp:1066](../../source/game_sa/PedIntelligence.cpp#L1066) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_snTimeInMilliseconds: source/game_sa/Timer.h:44 (0x00B7CB84) |

<a id="grupo-1303"></a>

## Globales y helpers: source/game_sa/PedScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedScannerInjectHooksLote109A() | [source/game_sa/PedScanner.cpp:32](../../source/game_sa/PedScanner.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1304"></a>

## Globales y helpers: source/game_sa/PedType.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPedTypeGangExe(int32 pedType) | [source/game_sa/PedType.cpp:27](../../source/game_sa/PedType.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedType::IsPedTypeGang 0x005FE9C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1305"></a>

## Globales y helpers: source/game_sa/Pickup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PickupIsVisibleKeepEdx(CPickup* self, uint32 entryEdx) | [source/game_sa/Pickup.cpp:32](../../source/game_sa/Pickup.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1306"></a>

## Globales y helpers: source/game_sa/Pickups.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PickupDistanceTo(const CPickup& pickup, const CVector& point) | [source/game_sa/Pickups.cpp:41](../../source/game_sa/Pickups.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PickupGlowByte(CEntity* entity, float distance, uint32 mask, float angleStep, float radius, float inverseRadius, float scale) | [source/game_sa/Pickups.cpp:201](../../source/game_sa/Pickups.cpp#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1307"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AtomicHasNVCPipeline(RpAtomic* atomic) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:143](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ObjPipeline: source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.h:26 (0x00C02C1C), source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.h:5 (0x00C02C68), source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:147 (0x00C02D24) |
| DNSetIndices(void* indexBuffer) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:310](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L310) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DNSetVertexDeclaration(void* declaration) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:311](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L311) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DNVertexAlphaEnable(RwBool enable) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:312](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L312) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NewEnvMapPipeMatData(CustomEnvMapPipeMaterialDataPool* pool) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:424](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L424) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CCustomBuildingDNPipeline::SetFxEnvTexture 0x005D9570 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1308"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float x) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:42](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FildMul(int32 n, float f) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:47](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FixedScale(const uint8* data, size_t i) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:52](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcEnvCamUV(RpAtomic* atomic, const uint8* matData, float* out) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:57](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnvWaveCoord(float p, float s, float rs, int32 n) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:73](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L73) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcEnvWaveUV(RpAtomic* atomic, uint8* matData, CustomEnvMapPipeAtomicData* atmData, float* out) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:79](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnvMapPoolDestructor(RawPool* self, void* /*edx*/) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:241](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L241) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall EnvMapPoolScalarDeletingDestructor(RawPool* self, void* /*edx*/, uint8 flags) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:255](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L255) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: CCustomCarEnvMapPipeline::PoolScalarDeletingDestructor 0x005D9450 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateEnvMapPool(int32 size) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:282](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L282) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DestroyEnvMapPool(Pool* pool) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:292](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L292) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1309"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * Raw(CustomEnvMapPipeMaterialData* data) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:12](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FromFixed8(uint8 b) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:17](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FromFixed255(uint8 b) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:22](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ToFixed8(float x) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:27](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ToFixed255(float x) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:32](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1310"></a>

## Globales y helpers: source/game_sa/Pipelines/PipelinesCommon.hpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetTextureStagesForLighting(bool isGeoTextured, RwTexture* tex) | [source/game_sa/Pipelines/PipelinesCommon.hpp:8](../../source/game_sa/Pipelines/PipelinesCommon.hpp#L8) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1311"></a>

## Globales y helpers: source/game_sa/Plant/PlantLocTri.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetPlantDensity_EaxEntry() | [source/game_sa/Plant/PlantLocTri.cpp:39](../../source/game_sa/Plant/PlantLocTri.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1312"></a>

## Globales y helpers: source/game_sa/Plant/ProcSurfaceInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProcSurfaceRandomInRange(float min, float max) | [source/game_sa/Plant/ProcSurfaceInfo.cpp:109](../../source/game_sa/Plant/ProcSurfaceInfo.cpp#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1313"></a>

## Globales y helpers: source/game_sa/PlantMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GeometrySetPrelitConstantColor(RpGeometry* geometry, RwRGBA color) | [source/game_sa/PlantMgr.cpp:64](../../source/game_sa/PlantMgr.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1314"></a>

## Globales y helpers: source/game_sa/PlayerInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProcessCrosshair_Hook(uint32* crosshair, void* /*edx*/, uint32 playerIndex, CPad* pad) | [source/game_sa/PlayerInfo.cpp:118](../../source/game_sa/PlayerInfo.cpp#L118) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPlayerInfo::ProcessCrosshair 0x0056EC80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1315"></a>

## Globales y helpers: source/game_sa/PlayerInfoProcess.cpp.inc

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProcessPlayerStunts(CPlayerInfo& p) | [source/game_sa/PlayerInfoProcess.cpp.inc:4](../../source/game_sa/PlayerInfoProcess.cpp.inc#L4) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| ProcessPlayerVehicleInteraction(CPlayerInfo& p, uint32 playerIndex, CPad* pad) | [source/game_sa/PlayerInfoProcess.cpp.inc:61](../../source/game_sa/PlayerInfoProcess.cpp.inc#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo bPlayersCanBeInSeparateCars: source/game_sa/GameLogic.h:52 (0x0096A8B3); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |

<a id="grupo-1316"></a>

## Globales y helpers: source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * ClumpCollisionConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:38](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:44](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:50](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:63](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x0041B1D0: fuera del catálogo consultado; ruta efectiva sin resolver; Posible dato fijo PC_Scratch: source/game_sa/common.h:72 (0x00C8E0C8); Posible dato fijo ms_currentModel: source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.h:25 (0x009689E0) |
| * ClumpCollisionStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:92](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClumpCollisionGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:98](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1317"></a>

## Globales y helpers: source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| JPegErrorExit(j_common_ptr cinfo) | [source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp:27](../../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeLibJpegInjectHooks232A() | [source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp:1094](../../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp#L1094) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1318"></a>

## Globales y helpers: source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ErrExit(ExeJpegCompress* cinfo, int32 code) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:31](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ErrExit1(ExeJpegCompress* cinfo, int32 code, int32 p0) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:36](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ErrExit2(ExeJpegCompress* cinfo, int32 code, int32 p0, int32 p1) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:42](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitByte(ExeJpegCompress* cinfo, int32 value) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:50](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitMarker(ExeJpegCompress* cinfo, int32 mark) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:65](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Emit2Bytes(ExeJpegCompress* cinfo, int32 value) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:71](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| JdivRoundUp(int32 a, int32 b) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:77](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L77) | C++ · Envoltura al EXE | Pendiente en catálogo: LibJpegMemory::jdiv_round_up 0x005CF890 | 0 / 0 | Delegación por dirección 0x005CF890: LibJpegMemory::jdiv_round_up; ruta efectiva sin resolver |
| EmitDqtExe(int32 index) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:87](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: LibJpegCompress::EmitDqt 0x005C55D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitDhtExe(ExeJpegCompress* cinfo, uint32 /*edx*/) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:94](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: LibJpegCompress::EmitDht 0x005C5780 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitDriExe() | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:101](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: LibJpegCompress::EmitDri 0x005C5910 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitSofExe(ExeJpegCompress* cinfo, uint32 /*edx*/) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:106](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L106) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: LibJpegCompress::EmitSof 0x005C5940 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitSosExe() | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:112](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: LibJpegCompress::EmitSos 0x005C5AC0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitJfifApp0Exe() | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:117](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: LibJpegCompress::EmitJfifApp0 0x005C5CA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitAdobeApp14Exe() | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:122](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: LibJpegCompress::EmitAdobeApp14 0x005C5EC0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1319"></a>

## Globales y helpers: source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressApi.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeJpegSuppressTables(void* cinfo, int32 suppress) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressApi.cpp:61](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressApi.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: LibJpegCompressApi::jpeg_suppress_tables 0x005CFA50 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1320"></a>

## Globales y helpers: source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & At(void* base, uint32 offset) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp:13](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ErrExit(void* cinfo, int32 code) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp:18](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ErrExit1(void* cinfo, int32 code, int32 parm) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp:24](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AllocSmall(void* cinfo, int32 pool, uint32 size) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp:30](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AllocLarge(void* cinfo, int32 pool, uint32 size) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp:34](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| JRoundUp(long a, long b) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp:38](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeGrayscaleConvert(void* cinfo, uint8** inputBuf, uint8*** outputBuf, uint32 outputRow, int32 numRows) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp:113](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp#L113) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: LibJpegCompressEncode::grayscale_convert 0x005C35F0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeJinitHuffEncoder(void* cinfo) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp:242](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp#L242) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: LibJpegCompressEncode::jinit_huff_encoder 0x005C5210 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

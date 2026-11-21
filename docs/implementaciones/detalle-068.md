# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1341"></a>

## Globales y helpers: source/game_sa/PedIK.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SinCosExe(float angle, float& outSin, float& outCos) | [source/game_sa/PedIK.cpp:45](../../source/game_sa/PedIK.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RotateXExe(CMatrix& matrix, float angle) | [source/game_sa/PedIK.cpp:50](../../source/game_sa/PedIK.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RotateYExe(CMatrix& matrix, float angle) | [source/game_sa/PedIK.cpp:54](../../source/game_sa/PedIK.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1342"></a>

## Globales y helpers: source/game_sa/PedIntelligence.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IncrementAngerAtPlayerRegs(CPedIntelligence* self, uint32 entryEdx, uint8 anger) | [source/game_sa/PedIntelligence.cpp:1067](../../source/game_sa/PedIntelligence.cpp#L1067) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedIntelligence::IncrementAngerAtPlayer 0x00421050 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo m_snTimeInMilliseconds: source/game_sa/Timer.h:44 (0x00B7CB84) |

<a id="grupo-1343"></a>

## Globales y helpers: source/game_sa/PedScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedScannerInjectHooksLote109A() | [source/game_sa/PedScanner.cpp:32](../../source/game_sa/PedScanner.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1344"></a>

## Globales y helpers: source/game_sa/PedType.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPedTypeGangExe(int32 pedType) | [source/game_sa/PedType.cpp:27](../../source/game_sa/PedType.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedType::IsPedTypeGang 0x005FE9C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1345"></a>

## Globales y helpers: source/game_sa/Pickup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PickupIsVisibleKeepEdx(CPickup* self, uint32 entryEdx) | [source/game_sa/Pickup.cpp:33](../../source/game_sa/Pickup.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPickup::IsVisible 0x00454C70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1346"></a>

## Globales y helpers: source/game_sa/Pickups.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PickupDistanceTo(const CPickup& pickup, const CVector& point) | [source/game_sa/Pickups.cpp:41](../../source/game_sa/Pickups.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PickupGlowByte(CEntity* entity, float distance, uint32 mask, float angleStep, float radius, float inverseRadius, float scale) | [source/game_sa/Pickups.cpp:201](../../source/game_sa/Pickups.cpp#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1347"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AtomicHasNVCPipeline(RpAtomic* atomic) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:143](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ObjPipeline: source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.h:26 (0x00C02C1C), source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.h:5 (0x00C02C68), source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:147 (0x00C02D24) |
| DNSetIndices(void* indexBuffer) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:310](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L310) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DNSetVertexDeclaration(void* declaration) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:311](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L311) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DNVertexAlphaEnable(RwBool enable) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:312](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L312) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NewEnvMapPipeMatData(CustomEnvMapPipeMaterialDataPool* pool) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:424](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L424) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CCustomBuildingDNPipeline::SetFxEnvTexture 0x005D9570 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1348"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float x) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:42](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FildMul(int32 n, float f) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:47](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FixedScale(const uint8* data, size_t i) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:52](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcEnvCamUV(RpAtomic* atomic, const uint8* matData, float* out) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:57](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnvWaveCoord(float p, float s, float rs, int32 n) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:73](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L73) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcEnvWaveUV(RpAtomic* atomic, uint8* matData, CustomEnvMapPipeAtomicData* atmData, float* out) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:79](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * EnvMapPoolConstruct(RawPool* self, int32 size) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:225](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L225) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall EnvMapPoolConstructor(RawPool* self, void* /*edx*/, int32 size, const char* /*name*/) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:240](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L240) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnvMapPoolDestroy(RawPool* self) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:246](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L246) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall EnvMapPoolNewExe(RawPool* self, void* /*edx*/) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:285](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L285) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnvMapPoolDeleteExe(RawPool* self, void* /*edx*/, const void* obj) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:291](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L291) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateEnvMapPool(int32 size) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:301](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L301) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DestroyEnvMapPool(Pool* pool) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:311](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L311) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1349"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * Raw(CustomEnvMapPipeMaterialData* data) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:12](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FromFixed8(uint8 b) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:17](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FromFixed255(uint8 b) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:22](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ToFixed8(float x) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:27](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ToFixed255(float x) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:32](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1350"></a>

## Globales y helpers: source/game_sa/Pipelines/PipelinesCommon.hpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetTextureStagesForLighting(bool isGeoTextured, RwTexture* tex) | [source/game_sa/Pipelines/PipelinesCommon.hpp:8](../../source/game_sa/Pipelines/PipelinesCommon.hpp#L8) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1351"></a>

## Globales y helpers: source/game_sa/Plant/PlantLocTri.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetPlantDensity_EaxEntry() | [source/game_sa/Plant/PlantLocTri.cpp:40](../../source/game_sa/Plant/PlantLocTri.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPlantLocTri::GetPlantDensity 0x005DC210 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1352"></a>

## Globales y helpers: source/game_sa/Plant/ProcSurfaceInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProcSurfaceRandomInRange(float min, float max) | [source/game_sa/Plant/ProcSurfaceInfo.cpp:109](../../source/game_sa/Plant/ProcSurfaceInfo.cpp#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1353"></a>

## Globales y helpers: source/game_sa/PlantMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GeometrySetPrelitConstantColor(RpGeometry* geometry, RwRGBA color) | [source/game_sa/PlantMgr.cpp:64](../../source/game_sa/PlantMgr.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1354"></a>

## Globales y helpers: source/game_sa/PlayerInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProcessCrosshair_Hook(uint32* crosshair, void* /*edx*/, uint32 playerIndex, CPad* pad) | [source/game_sa/PlayerInfo.cpp:121](../../source/game_sa/PlayerInfo.cpp#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPlayerInfo::ProcessCrosshair 0x0056EC80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1355"></a>

## Globales y helpers: source/game_sa/PlayerInfoProcess.cpp.inc

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProcessPlayerStunts(CPlayerInfo& p) | [source/game_sa/PlayerInfoProcess.cpp.inc:4](../../source/game_sa/PlayerInfoProcess.cpp.inc#L4) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| ProcessPlayerVehicleInteraction(CPlayerInfo& p, uint32 playerIndex, CPad* pad) | [source/game_sa/PlayerInfoProcess.cpp.inc:61](../../source/game_sa/PlayerInfoProcess.cpp.inc#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo bPlayersCanBeInSeparateCars: source/game_sa/GameLogic.h:52 (0x0096A8B3); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |

<a id="grupo-1356"></a>

## Globales y helpers: source/game_sa/PlayerPedData.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlayerPedDataAssignExe(CPlayerPedData* self, const CPlayerPedData* rhsPtr, uint32 esi) | [source/game_sa/PlayerPedData.cpp:170](../../source/game_sa/PlayerPedData.cpp#L170) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1357"></a>

## Globales y helpers: source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * ClumpCollisionConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:41](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:47](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:53](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:97](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClumpCollisionGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:103](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1358"></a>

## Globales y helpers: source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| JPegErrorExit(j_common_ptr cinfo) | [source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp:37](../../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeLibJpegInjectHooks232A() | [source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp:1109](../../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp#L1109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1359"></a>

## Globales y helpers: source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ErrExit(ExeJpegCompress* cinfo, int32 code) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:50](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ErrExit1(ExeJpegCompress* cinfo, int32 code, int32 p0) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:55](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ErrExit2(ExeJpegCompress* cinfo, int32 code, int32 p0, int32 p1) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:61](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitByte(ExeJpegCompress* cinfo, int32 value) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:69](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitMarker(ExeJpegCompress* cinfo, int32 mark) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:84](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Emit2Bytes(ExeJpegCompress* cinfo, int32 value) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:90](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L90) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| JdivRoundUp(int32 a, int32 b) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:96](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L96) | C++ · Envoltura al EXE | Histórica del catálogo: LibJpegMemory::jdiv_round_up 0x005CF890 | 0 / 0 | Delegación por dirección 0x005CF890: LibJpegMemory::jdiv_round_up; ruta efectiva sin resolver |
| EmitDqtExe(int32 index) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:106](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L106) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: LibJpegCompress::EmitDqt 0x005C55D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitDhtExe(ExeJpegCompress* cinfo, uint32 /*edx*/) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:113](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L113) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: LibJpegCompress::EmitDht 0x005C5780 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitDriExe() | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:120](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: LibJpegCompress::EmitDri 0x005C5910 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitSofExe(ExeJpegCompress* cinfo, uint32 /*edx*/) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:125](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: LibJpegCompress::EmitSof 0x005C5940 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitSosExe() | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:131](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L131) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: LibJpegCompress::EmitSos 0x005C5AC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitJfifApp0Exe() | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:136](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L136) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: LibJpegCompress::EmitJfifApp0 0x005C5CA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitAdobeApp14Exe() | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp:141](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: LibJpegCompress::EmitAdobeApp14 0x005C5EC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1360"></a>

## Globales y helpers: source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressApi.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeJpegSuppressTables(void* cinfo, int32 suppress) | [source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressApi.cpp:79](../../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressApi.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: LibJpegCompressApi::jpeg_suppress_tables 0x005CFA50 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

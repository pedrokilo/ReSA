# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1321"></a>

## Globales y helpers: source/game_sa/PedGeometryAnalyser.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GeometryPlaneOffset(const CVector* plane, const CVector* previous) | [source/game_sa/PedGeometryAnalyser.cpp:329](../../source/game_sa/PedGeometryAnalyser.cpp#L329) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| GeometryRouteLength355Compute(const CVector* points,int count,GeometryRouteLength355* result) | [source/game_sa/PedGeometryAnalyser.cpp:570](../../source/game_sa/PedGeometryAnalyser.cpp#L570) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| GeometryRouteLength355Less(const GeometryRouteLength355* first,const GeometryRouteLength355* second) | [source/game_sa/PedGeometryAnalyser.cpp:600](../../source/game_sa/PedGeometryAnalyser.cpp#L600) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1322"></a>

## Globales y helpers: source/game_sa/PedGroupPlacer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GroupOriginCanBeUsed(const CVector& origin, float radius) | [source/game_sa/PedGroupPlacer.cpp:38](../../source/game_sa/PedGroupPlacer.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:39 (0x00B6F028) |
| InstallGroupPed(CPedGroup& group, CPed* ped, int32 groupId, bool leader, bool setLeader) | [source/game_sa/PedGroupPlacer.cpp:47](../../source/game_sa/PedGroupPlacer.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaceCircularGroup(ePedType type, uint32 numPeds, const CVector& origin, bool random) | [source/game_sa/PedGroupPlacer.cpp:54](../../source/game_sa/PedGroupPlacer.cpp#L54) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 2 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| PlaceGroupDelegated(CPedGroupPlacer* self, void*, ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType allocType) | [source/game_sa/PedGroupPlacer.cpp:194](../../source/game_sa/PedGroupPlacer.cpp#L194) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1323"></a>

## Globales y helpers: source/game_sa/PedGroups.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Task10RemoveMember() | [source/game_sa/PedGroups.cpp:50](../../source/game_sa/PedGroups.cpp#L50) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x005F80D0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Task10FlushIntelligence() | [source/game_sa/PedGroups.cpp:56](../../source/game_sa/PedGroups.cpp#L56) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x005F7350; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1324"></a>

## Globales y helpers: source/game_sa/PedIK.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AsinX87(float x) | [source/game_sa/PedIK.cpp:38](../../source/game_sa/PedIK.cpp#L38) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| AtanMinusX87(float y, float x, float rot) | [source/game_sa/PedIK.cpp:62](../../source/game_sa/PedIK.cpp#L62) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| SinCosX87(float angle, float& outSin, float& outCos) | [source/game_sa/PedIK.cpp:75](../../source/game_sa/PedIK.cpp#L75) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| SinMulX87(float angle, float k) | [source/game_sa/PedIK.cpp:90](../../source/game_sa/PedIK.cpp#L90) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| DivCosX87(float num, float angle) | [source/game_sa/PedIK.cpp:102](../../source/game_sa/PedIK.cpp#L102) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| RotateXX87(CMatrix& matrix, float angle) | [source/game_sa/PedIK.cpp:114](../../source/game_sa/PedIK.cpp#L114) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| RotateYX87(CMatrix& matrix, float angle) | [source/game_sa/PedIK.cpp:182](../../source/game_sa/PedIK.cpp#L182) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ArmHeadingDiffX87(float nx, float ny, float rot) | [source/game_sa/PedIK.cpp:248](../../source/game_sa/PedIK.cpp#L248) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1325"></a>

## Globales y helpers: source/game_sa/Pickup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PickupIsVisible(CPickup* pickup) | [source/game_sa/Pickup.cpp:28](../../source/game_sa/Pickup.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PickupIsVisibleKeepEdx() | [source/game_sa/Pickup.cpp:32](../../source/game_sa/Pickup.cpp#L32) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| PickupRevenueIncrement(uint32 product) | [source/game_sa/Pickup.cpp:262](../../source/game_sa/Pickup.cpp#L262) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1326"></a>

## Globales y helpers: source/game_sa/Pickups.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PickupDistanceTo(const CPickup& pickup, const CVector& point) | [source/game_sa/Pickups.cpp:40](../../source/game_sa/Pickups.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PickupGlowByte(CEntity* entity, float distance, uint32 mask, float angleStep, float radius, float inverseRadius, float scale) | [source/game_sa/Pickups.cpp:229](../../source/game_sa/Pickups.cpp#L229) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1327"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AtomicHasNVCPipeline(RpAtomic* atomic) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:142](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L142) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ObjPipeline: source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.h:26 (0x00C02C1C), source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.h:5 (0x00C02C68), source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:147 (0x00C02D24) |
| * NewEnvMapPipeMatData(CustomEnvMapPipeMaterialDataPool* pool) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:293](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L293) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CCustomBuildingDNPipeline::SetFxEnvTexture 0x005D9570 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1328"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| _rwD3D9RenderStateVertexAlphaEnable(RwBool enable) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp:32](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1329"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float x) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:41](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FildMul(int32 n, float f) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:46](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L46) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FixedScale(const uint8* data, size_t i) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:57](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcEnvCamUV(RpAtomic* atomic, const uint8* matData, float* out) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:62](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnvWaveCoord(float p, float s, float rs, int32 n) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:78](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcEnvWaveUV(RpAtomic* atomic, uint8* matData, CustomEnvMapPipeAtomicData* atmData, float* out) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:84](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetEnvMapMaterial(const RwSurfaceProperties* surfProps, const RwRGBA* color, RwUInt32 flags, float spec, float power) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:156](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L156) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AmbientSaturated: source/game_sa/RenderWare/RenderWare.h:27 (0x008E2418); Posible dato fijo gEnvMapD3DMaterial: source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:34 (0x008D12D8) |
| * CreateEnvMapPool(int32 size) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:220](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L220) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DestroyEnvMapPool(Pool* pool) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:240](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L240) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsEnvironmentMappingSupported() | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp:334](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp#L334) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1330"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * Raw(CustomEnvMapPipeMaterialData* data) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:12](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FromFixed8(uint8 b) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:17](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FromFixed255(uint8 b) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:22](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ToFixed8(float x) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:27](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ToFixed255(float x) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:32](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1331"></a>

## Globales y helpers: source/game_sa/Pipelines/PipelinesCommon.hpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetTextureStagesForLighting(bool isGeoTextured, RwTexture* tex) | [source/game_sa/Pipelines/PipelinesCommon.hpp:8](../../source/game_sa/Pipelines/PipelinesCommon.hpp#L8) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RxD3D9InstanceDataRender(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* mesh) | [source/game_sa/Pipelines/PipelinesCommon.hpp:28](../../source/game_sa/Pipelines/PipelinesCommon.hpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RxD3D9InstanceDataRenderLighting(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* mesh, uint32 rxGeoFlags, RwTexture* tex) | [source/game_sa/Pipelines/PipelinesCommon.hpp:40](../../source/game_sa/Pipelines/PipelinesCommon.hpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1332"></a>

## Globales y helpers: source/game_sa/Plant/PlantLocTri.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetPlantDensity_EaxWrapper() | [source/game_sa/Plant/PlantLocTri.cpp:37](../../source/game_sa/Plant/PlantLocTri.cpp#L37) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1333"></a>

## Globales y helpers: source/game_sa/Plant/ProcSurfaceInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProcSurfaceRandomInRange(float min, float max) | [source/game_sa/Plant/ProcSurfaceInfo.cpp:96](../../source/game_sa/Plant/ProcSurfaceInfo.cpp#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcSurfaceMulX87(float& v, float k) | [source/game_sa/Plant/ProcSurfaceInfo.cpp:102](../../source/game_sa/Plant/ProcSurfaceInfo.cpp#L102) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1334"></a>

## Globales y helpers: source/game_sa/PlantMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AtomicCreatePrelitIfNeeded(RpAtomic* atomic) | [source/game_sa/PlantMgr.cpp:31](../../source/game_sa/PlantMgr.cpp#L31) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x005DD100: fuera del catálogo consultado; ruta efectiva sin resolver |
| GeometrySetPrelitConstantColor(RpGeometry* geometry, RwRGBA color) | [source/game_sa/PlantMgr.cpp:36](../../source/game_sa/PlantMgr.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1335"></a>

## Globales y helpers: source/game_sa/PlayerInfoProcess.cpp.inc

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProcessPlayerStunts(CPlayerInfo& p) | [source/game_sa/PlayerInfoProcess.cpp.inc:4](../../source/game_sa/PlayerInfoProcess.cpp.inc#L4) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| ProcessPlayerVehicleInteraction(CPlayerInfo& p, uint32 playerIndex, CPad* pad) | [source/game_sa/PlayerInfoProcess.cpp.inc:61](../../source/game_sa/PlayerInfoProcess.cpp.inc#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x006919C0: fuera del catálogo consultado; ruta efectiva sin resolver; Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo bPlayersCanBeInSeparateCars: source/game_sa/GameLogic.h:52 (0x0096A8B3); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |

<a id="grupo-1336"></a>

## Globales y helpers: source/game_sa/PlayerRelationshipRecorder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Relationship6GetActive() | [source/game_sa/PlayerRelationshipRecorder.cpp:33](../../source/game_sa/PlayerRelationshipRecorder.cpp#L33) | Ensamblador · Envoltura al EXE | Histórica del catálogo: CPlayerRelationshipRecorder::RecordRelationshipWithPlayer 0x0061A1D0 | 1 / 1 | Trampolín ASM a 0x00681720; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Relationship6Kill() | [source/game_sa/PlayerRelationshipRecorder.cpp:39](../../source/game_sa/PlayerRelationshipRecorder.cpp#L39) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x006985E0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Relationship6Follow() | [source/game_sa/PlayerRelationshipRecorder.cpp:45](../../source/game_sa/PlayerRelationshipRecorder.cpp#L45) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00698610; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| & GetPlayerRelationshipRecorder() | [source/game_sa/PlayerRelationshipRecorder.cpp:163](../../source/game_sa/PlayerRelationshipRecorder.cpp#L163) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1337"></a>

## Globales y helpers: source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BreakablePluginAttach() | [source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp:43](../../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_BreakablePlugin: source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.h:11 (0x00BB4238) |
| * BreakableDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp:75](../../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_BreakablePlugin: source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.h:11 (0x00BB4238) |
| * BreakableStreamRead(RwStream* stream, int binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp:99](../../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp#L99) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_BreakablePlugin: source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.h:11 (0x00BB4238) |

<a id="grupo-1338"></a>

## Globales y helpers: source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * ClumpCollisionConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:38](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:44](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:50](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:63](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x0041B1D0: fuera del catálogo consultado; ruta efectiva sin resolver; Posible dato fijo PC_Scratch: source/game_sa/common.h:72 (0x00C8E0C8); Posible dato fijo ms_currentModel: source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.h:25 (0x009689E0) |
| * ClumpCollisionStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:92](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClumpCollisionGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:98](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1339"></a>

## Globales y helpers: source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| JPegErrorExit(j_common_ptr cinfo) | [source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp:27](../../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| JPegCompressScreen(RwCamera* camera, jpeg_destination_mgr& dst) | [source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp:55](../../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| JPegCompressScreenToFile(RwCamera* camera, const char* path) | [source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp:95](../../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1340"></a>

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

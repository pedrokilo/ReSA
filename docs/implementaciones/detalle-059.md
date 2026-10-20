<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 20/10/2026 (Ciudad de México):** 808 grupos / 8772 funciones reconstruidas; 8772 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1161"></a>

## Globales y helpers: source/game_sa/PedGroups.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeEdxAfterRemoveMember(const CPedGroup& group, int32 idx, uint32 edx) | [source/game_sa/PedGroups.cpp:43](../../source/game_sa/PedGroups.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeEdxAfterFlush(const CPedGroup& group, uint32 edx) | [source/game_sa/PedGroups.cpp:66](../../source/game_sa/PedGroups.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00B744A8 |
| EmptyGroupLikeExe(CPedGroup& group, ExeRegs* regs) | [source/game_sa/PedGroups.cpp:81](../../source/game_sa/PedGroups.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddGroupCore(ExeRegs* regs) | [source/game_sa/PedGroups.cpp:101](../../source/game_sa/PedGroups.cpp#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedGroups::AddGroup 0x005FB800 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo ms_activeGroups: source/game_sa/PedGroups.h:17 (0x00C098E0); Posible dato fijo ms_groups: source/game_sa/PedGroups.h:20 (0x00C09920) |
| InitCore(ExeRegs* regs) | [source/game_sa/PedGroups.cpp:119](../../source/game_sa/PedGroups.cpp#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedGroups::Init 0x005FB8C0 | 0 / 0 | Posible dato fijo ScriptReferenceIndex: source/game_sa/DecisionMakers/DecisionMakerTypes.h:45 (0x00C0AFF4), source/game_sa/PedGroups.h:16 (0x00C098D0), source/game_sa/Scripts/Scripted2dEffects.h:55 (0x00C3A120); Posible dato fijo ms_activeGroups: source/game_sa/PedGroups.h:17 (0x00C098E0); Posible dato fijo ms_groups: source/game_sa/PedGroups.h:20 (0x00C09920) |
| CleanUpForShutDownCore(ExeRegs* regs) | [source/game_sa/PedGroups.cpp:133](../../source/game_sa/PedGroups.cpp#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedGroups::CleanUpForShutDown 0x005FB930 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo ms_groups: source/game_sa/PedGroups.h:20 (0x00C09920) |
| AddGroupExe(uint32 entryEcx, uint32 entryEdx) | [source/game_sa/PedGroups.cpp:143](../../source/game_sa/PedGroups.cpp#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InitExe(uint32 entryEcx, uint32 entryEdx) | [source/game_sa/PedGroups.cpp:149](../../source/game_sa/PedGroups.cpp#L149) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CleanUpForShutDownExe(uint32 entryEcx, uint32 entryEdx) | [source/game_sa/PedGroups.cpp:157](../../source/game_sa/PedGroups.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1162"></a>

## Globales y helpers: source/game_sa/PedIK.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SinCosExe(float angle, float& outSin, float& outCos) | [source/game_sa/PedIK.cpp:45](../../source/game_sa/PedIK.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RotateXExe(CMatrix& matrix, float angle) | [source/game_sa/PedIK.cpp:50](../../source/game_sa/PedIK.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RotateYExe(CMatrix& matrix, float angle) | [source/game_sa/PedIK.cpp:54](../../source/game_sa/PedIK.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1163"></a>

## Globales y helpers: source/game_sa/PedIntelligence.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IncrementAngerAtPlayerRegs(CPedIntelligence* self, uint32 entryEdx, uint8 anger) | [source/game_sa/PedIntelligence.cpp:1047](../../source/game_sa/PedIntelligence.cpp#L1047) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_snTimeInMilliseconds: source/game_sa/Timer.h:44 (0x00B7CB84) |

<a id="grupo-1164"></a>

## Globales y helpers: source/game_sa/Pickup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PickupIsVisibleKeepEdx(CPickup* self, uint32 entryEdx) | [source/game_sa/Pickup.cpp:32](../../source/game_sa/Pickup.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1165"></a>

## Globales y helpers: source/game_sa/Pickups.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PickupDistanceTo(const CPickup& pickup, const CVector& point) | [source/game_sa/Pickups.cpp:41](../../source/game_sa/Pickups.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PickupGlowByte(CEntity* entity, float distance, uint32 mask, float angleStep, float radius, float inverseRadius, float scale) | [source/game_sa/Pickups.cpp:201](../../source/game_sa/Pickups.cpp#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1166"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AtomicHasNVCPipeline(RpAtomic* atomic) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:142](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L142) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ObjPipeline: source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.h:26 (0x00C02C1C), source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.h:5 (0x00C02C68), source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:147 (0x00C02D24) |
| DNSetIndices(void* indexBuffer) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:309](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L309) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DNSetVertexDeclaration(void* declaration) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:310](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L310) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DNVertexAlphaEnable(RwBool enable) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:311](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L311) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DNInstanceDataRender(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* mesh) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:312](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L312) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DNInstanceDataRenderLighting(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* mesh, uint32 rxGeoFlags, RwTexture* texture) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:313](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L313) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NewEnvMapPipeMatData(CustomEnvMapPipeMaterialDataPool* pool) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp:414](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp#L414) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CCustomBuildingDNPipeline::SetFxEnvTexture 0x005D9570 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1167"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| _rwD3D9RenderStateVertexAlphaEnable(RwBool enable) | [source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp:32](../../source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1168"></a>

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

<a id="grupo-1169"></a>

## Globales y helpers: source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * Raw(CustomEnvMapPipeMaterialData* data) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:12](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FromFixed8(uint8 b) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:17](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FromFixed255(uint8 b) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:22](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ToFixed8(float x) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:27](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ToFixed255(float x) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp:32](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1170"></a>

## Globales y helpers: source/game_sa/Pipelines/PipelinesCommon.hpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetTextureStagesForLighting(bool isGeoTextured, RwTexture* tex) | [source/game_sa/Pipelines/PipelinesCommon.hpp:8](../../source/game_sa/Pipelines/PipelinesCommon.hpp#L8) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RxD3D9InstanceDataRender(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* mesh) | [source/game_sa/Pipelines/PipelinesCommon.hpp:28](../../source/game_sa/Pipelines/PipelinesCommon.hpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RxD3D9InstanceDataRenderLighting(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* mesh, uint32 rxGeoFlags, RwTexture* tex) | [source/game_sa/Pipelines/PipelinesCommon.hpp:40](../../source/game_sa/Pipelines/PipelinesCommon.hpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1171"></a>

## Globales y helpers: source/game_sa/Plant/PlantLocTri.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetPlantDensity_EaxEntry() | [source/game_sa/Plant/PlantLocTri.cpp:39](../../source/game_sa/Plant/PlantLocTri.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1172"></a>

## Globales y helpers: source/game_sa/Plant/ProcSurfaceInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProcSurfaceRandomInRange(float min, float max) | [source/game_sa/Plant/ProcSurfaceInfo.cpp:97](../../source/game_sa/Plant/ProcSurfaceInfo.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1173"></a>

## Globales y helpers: source/game_sa/PlantMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GeometrySetPrelitConstantColor(RpGeometry* geometry, RwRGBA color) | [source/game_sa/PlantMgr.cpp:64](../../source/game_sa/PlantMgr.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1174"></a>

## Globales y helpers: source/game_sa/PlayerInfoProcess.cpp.inc

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProcessPlayerStunts(CPlayerInfo& p) | [source/game_sa/PlayerInfoProcess.cpp.inc:4](../../source/game_sa/PlayerInfoProcess.cpp.inc#L4) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| ProcessPlayerVehicleInteraction(CPlayerInfo& p, uint32 playerIndex, CPad* pad) | [source/game_sa/PlayerInfoProcess.cpp.inc:61](../../source/game_sa/PlayerInfoProcess.cpp.inc#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x006919C0: fuera del catálogo consultado; ruta efectiva sin resolver; Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo bPlayersCanBeInSeparateCars: source/game_sa/GameLogic.h:52 (0x0096A8B3); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |

<a id="grupo-1175"></a>

## Globales y helpers: source/game_sa/PlayerRelationshipRecorder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetPlayerRelationshipRecorder() | [source/game_sa/PlayerRelationshipRecorder.cpp:125](../../source/game_sa/PlayerRelationshipRecorder.cpp#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1176"></a>

## Globales y helpers: source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * ClumpCollisionConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:38](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:44](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:50](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ClumpCollisionStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:63](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x0041B1D0: fuera del catálogo consultado; ruta efectiva sin resolver; Posible dato fijo PC_Scratch: source/game_sa/common.h:72 (0x00C8E0C8); Posible dato fijo ms_currentModel: source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.h:25 (0x009689E0) |
| * ClumpCollisionStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:92](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClumpCollisionGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp:98](../../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1177"></a>

## Globales y helpers: source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| JPegErrorExit(j_common_ptr cinfo) | [source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp:27](../../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1178"></a>

## Globales y helpers: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * NodeNameConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:51](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |
| * NodeNameDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:60](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NodeNameCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:67](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |
| * NodeNameStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:75](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |
| * NodeNameStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:83](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |
| NodeNameStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:90](../../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp#L90) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNodeNamePluginOffset: source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp:25 (0x00C87C5C) |

<a id="grupo-1179"></a>

## Globales y helpers: source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * PipelineConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:50](../../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gPipelinePluginOffset: source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:25 (0x008D6080) |
| * PipelineCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:59](../../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gPipelinePluginOffset: source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:25 (0x008D6080) |
| * PipelineStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:67](../../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gPipelinePluginOffset: source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:25 (0x008D6080) |
| PipelineGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:74](../../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1180"></a>

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

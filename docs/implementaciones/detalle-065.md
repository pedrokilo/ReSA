<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 892 grupos / 10295 funciones reconstruidas; 10295 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1281"></a>

## Globales y helpers: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RpAnimBlendClumpForEachAssociation(RpClump* clump, Fn&& IterFn) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.h:51](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.h#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1282"></a>

## Globales y helpers: source/game_sa/PointLights.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FogEmitSprite(const CPointLight& light, const float& alpha, const RwV3d& screen, float w, float h, int32 tab, uint32 timeValue, bool pointLight) | [source/game_sa/PointLights.cpp:156](../../source/game_sa/PointLights.cpp#L156) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FogDirectionalLight(const CPointLight& light, const float& intensity) | [source/game_sa/PointLights.cpp:164](../../source/game_sa/PointLights.cpp#L164) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FogPointLight(const CPointLight& light, const float& intensity, const float& size) | [source/game_sa/PointLights.cpp:196](../../source/game_sa/PointLights.cpp#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |

<a id="grupo-1283"></a>

## Globales y helpers: source/game_sa/Pools.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * ConstructPool(int32 capacity, uint32 elementSize) | [source/game_sa/Pools.cpp:137](../../source/game_sa/Pools.cpp#L137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PoolDestructorBodyExe(void* poolPtr, void* /*edx*/) | [source/game_sa/Pools.cpp:162](../../source/game_sa/Pools.cpp#L162) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPools::PoolDestructorBody 0x004049C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DestroyPool(void* poolPtr) | [source/game_sa/Pools.cpp:176](../../source/game_sa/Pools.cpp#L176) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetPedPool() | [source/game_sa/Pools.cpp:516](../../source/game_sa/Pools.cpp#L516) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPedPool: source/game_sa/Pools.cpp:64 (0x00B74490) |
| * GetVehiclePool() | [source/game_sa/Pools.cpp:521](../../source/game_sa/Pools.cpp#L521) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pVehiclePool: source/game_sa/Pools.cpp:65 (0x00B74494) |
| * GetBuildingPool() | [source/game_sa/Pools.cpp:526](../../source/game_sa/Pools.cpp#L526) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pBuildingPool: source/game_sa/Pools.cpp:66 (0x00B74498) |
| * GetObjectPool() | [source/game_sa/Pools.cpp:531](../../source/game_sa/Pools.cpp#L531) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pObjectPool: source/game_sa/Pools.cpp:67 (0x00B7449C) |
| * GetDummyPool() | [source/game_sa/Pools.cpp:536](../../source/game_sa/Pools.cpp#L536) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pDummyPool: source/game_sa/Pools.cpp:68 (0x00B744A0) |
| * GetColModelPool() | [source/game_sa/Pools.cpp:541](../../source/game_sa/Pools.cpp#L541) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pColModelPool: source/game_sa/Pools.cpp:69 (0x00B744A4) |
| * GetTaskPool() | [source/game_sa/Pools.cpp:546](../../source/game_sa/Pools.cpp#L546) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pTaskPool: source/game_sa/Pools.cpp:70 (0x00B744A8) |
| * GetPedIntelligencePool() | [source/game_sa/Pools.cpp:551](../../source/game_sa/Pools.cpp#L551) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPedIntelligencePool: source/game_sa/Pools.cpp:71 (0x00B744C0) |
| * GetPtrNodeSingleLinkPool() | [source/game_sa/Pools.cpp:556](../../source/game_sa/Pools.cpp#L556) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPtrNodeSingleLinkPool: source/game_sa/Pools.cpp:72 (0x00B74484) |
| * GetPtrNodeDoubleLinkPool() | [source/game_sa/Pools.cpp:561](../../source/game_sa/Pools.cpp#L561) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPtrNodeDoubleLinkPool: source/game_sa/Pools.cpp:73 (0x00B74488) |
| * GetEntryInfoNodePool() | [source/game_sa/Pools.cpp:566](../../source/game_sa/Pools.cpp#L566) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pEntryInfoNodePool: source/game_sa/Pools.cpp:74 (0x00B7448C) |
| * GetPointRoutePool() | [source/game_sa/Pools.cpp:571](../../source/game_sa/Pools.cpp#L571) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPointRoutePool: source/game_sa/Pools.cpp:75 (0x00B744B0) |
| * GetPatrolRoutePool() | [source/game_sa/Pools.cpp:576](../../source/game_sa/Pools.cpp#L576) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPatrolRoutePool: source/game_sa/Pools.cpp:76 (0x00B744B4) |
| * GetEventPool() | [source/game_sa/Pools.cpp:581](../../source/game_sa/Pools.cpp#L581) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pEventPool: source/game_sa/Pools.cpp:77 (0x00B744AC) |
| * GetNodeRoutePool() | [source/game_sa/Pools.cpp:586](../../source/game_sa/Pools.cpp#L586) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pNodeRoutePool: source/game_sa/Pools.cpp:78 (0x00B744B8) |
| * GetTaskAllocatorPool() | [source/game_sa/Pools.cpp:591](../../source/game_sa/Pools.cpp#L591) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pTaskAllocatorPool: source/game_sa/Pools.cpp:79 (0x00B744BC) |
| * GetPedAttractorPool() | [source/game_sa/Pools.cpp:596](../../source/game_sa/Pools.cpp#L596) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPedAttractorPool: source/game_sa/Pools.cpp:80 (0x00B744C4) |

<a id="grupo-1284"></a>

## Globales y helpers: source/game_sa/Pools/PoolRawDelete.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PoolFreeSlotRaw(void* pool, const void* obj, int32 elementSize) | [source/game_sa/Pools/PoolRawDelete.h:6](../../source/game_sa/Pools/PoolRawDelete.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PoolFreeSlotRawShift(void* pool, const void* obj, int32 shift) | [source/game_sa/Pools/PoolRawDelete.h:17](../../source/game_sa/Pools/PoolRawDelete.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1285"></a>

## Globales y helpers: source/game_sa/Population.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LoadGroupFile(const char* fileName, auto& outModelsInGroup, auto& outNumOfModelsPerGroup) | [source/game_sa/Population.cpp:171](../../source/game_sa/Population.cpp#L171) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessPossibleDriverModelsOfCar(eModelID carModelIndex, auto&& ProcessModel) | [source/game_sa/Population.cpp:610](../../source/game_sa/Population.cpp#L610) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PopulationTask(size_t size, Args... args) | [source/game_sa/Population.cpp:1911](../../source/game_sa/Population.cpp#L1911) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PopulationModelRadius(eModelID model) | [source/game_sa/Population.cpp:1915](../../source/game_sa/Population.cpp#L1915) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PopulationModelLoaded(eModelID model) | [source/game_sa/Population.cpp:1918](../../source/game_sa/Population.cpp#L1918) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PopulationCouple(eModelID maleModel, eModelID femaleModel, CVector position) | [source/game_sa/Population.cpp:1921](../../source/game_sa/Population.cpp#L1921) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028); Posible dato fijo ms_offsets: source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.h:28 (0x00C196E8) |
| PopulationDefaultTask(CPed* ped,ePedType type) | [source/game_sa/Population.cpp:1959](../../source/game_sa/Population.cpp#L1959) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x0048E610: CTaskComplexWanderCriminal::Constructor; ruta efectiva sin resolver; Colaborador por dirección 0x00681B60: CTaskManager::SetTaskSecondary; ruta efectiva sin resolver; Conversión de dirección fija: 0x00A90CF0; Posible dato fijo MI_TELLY: source/game_sa/ModelIndices.cpp:69 (0x008CD5A8) |
| PopulationSunbathe(CPed* ped,const CVector& position) | [source/game_sa/Population.cpp:1977](../../source/game_sa/Population.cpp#L1977) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x0043E0C0: CPlaceable::SetHeading; ruta efectiva sin resolver; Colaborador por dirección 0x00632140: CTaskComplexSunbathe::CanSunbathe; ruta efectiva sin resolver; Conversión de dirección fija: 0x00969159; Posible dato fijo g_surfaceInfos: source/game_sa/SurfaceInfos_c.h:79 (0x00B79538) |

<a id="grupo-1286"></a>

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

<a id="grupo-1287"></a>

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

<a id="grupo-1288"></a>

## Globales y helpers: source/game_sa/Queue.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * InitialiseQueue(Queue* q, int32 size) | [source/game_sa/Queue.h:26](../../source/game_sa/Queue.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FinalizeQueue(Queue* q) | [source/game_sa/Queue.h:33](../../source/game_sa/Queue.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddToQueue(Queue* q, int32 i) | [source/game_sa/Queue.h:38](../../source/game_sa/Queue.h#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetFirstInQueue(Queue* q) | [source/game_sa/Queue.h:43](../../source/game_sa/Queue.h#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RemoveFirstInQueue(Queue* q) | [source/game_sa/Queue.h:47](../../source/game_sa/Queue.h#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1289"></a>

## Globales y helpers: source/game_sa/RGBA.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| lerp&lt;CRGBA&gt;(const CRGBA& from, const CRGBA& to, float t) | [source/game_sa/RGBA.h:231](../../source/game_sa/RGBA.h#L231) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1290"></a>

## Globales y helpers: source/game_sa/Radar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RadarExeFloat(uintptr_t address) | [source/game_sa/Radar.cpp:33](../../source/game_sa/Radar.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RadarStretchBaseX() | [source/game_sa/Radar.cpp:38](../../source/game_sa/Radar.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| RadarStretchBaseY() | [source/game_sa/Radar.cpp:39](../../source/game_sa/Radar.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| RadarFtol(float v) | [source/game_sa/Radar.cpp:41](../../source/game_sa/Radar.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformRadarPointToScreenSpaceImpl(CVector2D* out, const CVector2D* in) | [source/game_sa/Radar.cpp:444](../../source/game_sa/Radar.cpp#L444) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| * __cdecl RadarTransformScreenExe(CVector2D* out, const CVector2D* in) | [source/game_sa/Radar.cpp:468](../../source/game_sa/Radar.cpp#L468) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Limit(float& x, float& y) | [source/game_sa/Radar.cpp:910](../../source/game_sa/Radar.cpp#L910) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748) |
| RadarRawRect(float left, float plus4, float right, float plusC) | [source/game_sa/Radar.cpp:922](../../source/game_sa/Radar.cpp#L922) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RadarTraceStaticColour(const tRadarTrace& trace) | [source/game_sa/Radar.cpp:1783](../../source/game_sa/Radar.cpp#L1783) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo HudColour: source/game_sa/HudColours.cpp:23 (0x00BAB22C) |
| RadarGangPulse(float angle, int32 alphaInt) | [source/game_sa/Radar.cpp:1792](../../source/game_sa/Radar.cpp#L1792) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAirStripLocation(eAirstripLocation location) | [source/game_sa/Radar.cpp:2424](../../source/game_sa/Radar.cpp#L2424) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1291"></a>

## Globales y helpers: source/game_sa/Ragdoll/IKChain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| *& RawRefSlot(notsa::EntityRef&lt;T&gt;& ref) | [source/game_sa/Ragdoll/IKChain.cpp:56](../../source/game_sa/Ragdoll/IKChain.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1292"></a>

## Globales y helpers: source/game_sa/Ragdoll/IKChainManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GetPedIKManagerTask(CPed* ped, bool createIfNotExists = false) | [source/game_sa/Ragdoll/IKChainManager.cpp:168](../../source/game_sa/Ragdoll/IKChainManager.cpp#L168) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetPedIKLookAtTask(CPed* ped) | [source/game_sa/Ragdoll/IKChainManager.cpp:184](../../source/game_sa/Ragdoll/IKChainManager.cpp#L184) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1293"></a>

## Globales y helpers: source/game_sa/Remote.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & RemoteField(void* base, size_t offset) | [source/game_sa/Remote.cpp:34](../../source/game_sa/Remote.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1294"></a>

## Globales y helpers: source/game_sa/RenderWare/D3DIndexDataBuffer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetDesc(void* indexBuffer, IndexBufferDesc* desc) | [source/game_sa/RenderWare/D3DIndexDataBuffer.cpp:32](../../source/game_sa/RenderWare/D3DIndexDataBuffer.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1295"></a>

## Globales y helpers: source/game_sa/RenderWare/D3DResourceSystem.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * D3DDevice() | [source/game_sa/RenderWare/D3DResourceSystem.cpp:71](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00C97C28 |
| ExeFree(void* memory) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:75](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L75) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x008214BD: fuera del catálogo consultado; ruta efectiva sin resolver |
| * TextureBufferAt(int32 index) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:79](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * IndexDataBufferAt(int32 index) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:82](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreResource(void* array, int32 index, void* resource) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:85](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1296"></a>

## Globales y helpers: source/game_sa/RenderWare/D3DTextureBuffer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetLevelCount(void* texture) | [source/game_sa/RenderWare/D3DTextureBuffer.cpp:41](../../source/game_sa/RenderWare/D3DTextureBuffer.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLevelDesc(void* texture, uint32 level, SurfaceDesc* desc) | [source/game_sa/RenderWare/D3DTextureBuffer.cpp:45](../../source/game_sa/RenderWare/D3DTextureBuffer.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1297"></a>

## Globales y helpers: source/game_sa/RenderWare/RenderWare.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| *GetD3D9Device() | [source/game_sa/RenderWare/RenderWare.h:29](../../source/game_sa/RenderWare/RenderWare.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00C97C28 |
| *GetD3DViewTransform() | [source/game_sa/RenderWare/RenderWare.h:36](../../source/game_sa/RenderWare/RenderWare.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00C9BC80 |
| *GetD3DProjTransform() | [source/game_sa/RenderWare/RenderWare.h:40](../../source/game_sa/RenderWare/RenderWare.h#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x008E2458 |
| _rpMaterialSetDefaultSurfaceProperties(RwSurfaceProperties *surfProps) | [source/game_sa/RenderWare/RenderWare.h:44](../../source/game_sa/RenderWare/RenderWare.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RwStreamRead(RwStream* stream, size_t size = sizeof(T)) | [source/game_sa/RenderWare/RenderWare.h:63](../../source/game_sa/RenderWare/RenderWare.h#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1298"></a>

## Globales y helpers: source/game_sa/RenderWare/rw/rpanisot.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RpAnisotInjectHooks() | [source/game_sa/RenderWare/rw/rpanisot.cpp:26](../../source/game_sa/RenderWare/rw/rpanisot.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpAnisotPluginAttach() | [source/game_sa/RenderWare/rw/rpanisot.cpp:59](../../source/game_sa/RenderWare/rw/rpanisot.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1299"></a>

## Globales y helpers: source/game_sa/RenderWare/rw/rpdbgerr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * rwPLUGIN_ERRFUNC(RwInt32 code,...) | [source/game_sa/RenderWare/rw/rpdbgerr.cpp:69](../../source/game_sa/RenderWare/rw/rpdbgerr.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1300"></a>

## Globales y helpers: source/game_sa/RenderWare/rw/rphanim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RpHAnimInjectHooks() | [source/game_sa/RenderWare/rw/rphanim.cpp:23](../../source/game_sa/RenderWare/rw/rphanim.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpHAnimHierarchySetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) | [source/game_sa/RenderWare/rw/rphanim.cpp:30](../../source/game_sa/RenderWare/rw/rphanim.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpHAnimHierarchyCreate(RwInt32 numNodes, RwUInt32* nodeFlags, RwInt32* nodeIDs, RpHAnimHierarchyFlag flags, RwInt32 maxInterpKeyFrameSize) | [source/game_sa/RenderWare/rw/rphanim.cpp:34](../../source/game_sa/RenderWare/rw/rphanim.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpHAnimHierarchyCreateFromHierarchy(RpHAnimHierarchy* hierarchy, RpHAnimHierarchyFlag flags, RwInt32 maxInterpKeyFrameSize) | [source/game_sa/RenderWare/rw/rphanim.cpp:38](../../source/game_sa/RenderWare/rw/rphanim.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpHAnimHierarchyDestroy(RpHAnimHierarchy* hierarchy) | [source/game_sa/RenderWare/rw/rphanim.cpp:42](../../source/game_sa/RenderWare/rw/rphanim.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpHAnimHierarchyCreateSubHierarchy(RpHAnimHierarchy* parentHierarchy, RwInt32 startNode, RpHAnimHierarchyFlag flags, RwInt32 maxInterpKeyFrameSize) | [source/game_sa/RenderWare/rw/rphanim.cpp:46](../../source/game_sa/RenderWare/rw/rphanim.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpHAnimHierarchyAttach(RpHAnimHierarchy* hierarchy) | [source/game_sa/RenderWare/rw/rphanim.cpp:50](../../source/game_sa/RenderWare/rw/rphanim.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpHAnimHierarchyDetach(RpHAnimHierarchy* hierarchy) | [source/game_sa/RenderWare/rw/rphanim.cpp:54](../../source/game_sa/RenderWare/rw/rphanim.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpHAnimHierarchyAttachFrameIndex(RpHAnimHierarchy* hierarchy, RwInt32 nodeIndex) | [source/game_sa/RenderWare/rw/rphanim.cpp:58](../../source/game_sa/RenderWare/rw/rphanim.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpHAnimHierarchyDetachFrameIndex(RpHAnimHierarchy* hierarchy, RwInt32 nodeIndex) | [source/game_sa/RenderWare/rw/rphanim.cpp:62](../../source/game_sa/RenderWare/rw/rphanim.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpHAnimFrameSetHierarchy(RwFrame* frame, RpHAnimHierarchy* hierarchy) | [source/game_sa/RenderWare/rw/rphanim.cpp:66](../../source/game_sa/RenderWare/rw/rphanim.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpHAnimHierarchyGetMatrixArray(RpHAnimHierarchy* hierarchy) | [source/game_sa/RenderWare/rw/rphanim.cpp:77](../../source/game_sa/RenderWare/rw/rphanim.cpp#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpHAnimHierarchyGetNodeMatrix(RpHAnimHierarchy* hierarchy, RwInt32 nodeID) | [source/game_sa/RenderWare/rw/rphanim.cpp:82](../../source/game_sa/RenderWare/rw/rphanim.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpHAnimHierarchyUpdateMatrices(RpHAnimHierarchy* hierarchy) | [source/game_sa/RenderWare/rw/rphanim.cpp:88](../../source/game_sa/RenderWare/rw/rphanim.cpp#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpHAnimIDGetIndex(RpHAnimHierarchy* hierarchy, RwInt32 ID) | [source/game_sa/RenderWare/rw/rphanim.cpp:92](../../source/game_sa/RenderWare/rw/rphanim.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpHAnimPluginAttach() | [source/game_sa/RenderWare/rw/rphanim.cpp:96](../../source/game_sa/RenderWare/rw/rphanim.cpp#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpHAnimKeyFrameApply(void* matrix, void* voidIFrame) | [source/game_sa/RenderWare/rw/rphanim.cpp:100](../../source/game_sa/RenderWare/rw/rphanim.cpp#L100) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpHAnimKeyFrameBlend(void* voidOut, void* voidIn1, void* voidIn2, RwReal alpha) | [source/game_sa/RenderWare/rw/rphanim.cpp:104](../../source/game_sa/RenderWare/rw/rphanim.cpp#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpHAnimKeyFrameInterpolate(void* voidOut, void* voidIn1, void* voidIn2, RwReal time, void* customData) | [source/game_sa/RenderWare/rw/rphanim.cpp:108](../../source/game_sa/RenderWare/rw/rphanim.cpp#L108) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpHAnimKeyFrameAdd(void* voidOut, void* voidIn1, void* voidIn2) | [source/game_sa/RenderWare/rw/rphanim.cpp:112](../../source/game_sa/RenderWare/rw/rphanim.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpHAnimKeyFrameMulRecip(void* voidFrame, void* voidStart) | [source/game_sa/RenderWare/rw/rphanim.cpp:116](../../source/game_sa/RenderWare/rw/rphanim.cpp#L116) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpHAnimKeyFrameStreamRead(RwStream* stream, RtAnimAnimation* animation) | [source/game_sa/RenderWare/rw/rphanim.cpp:120](../../source/game_sa/RenderWare/rw/rphanim.cpp#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpHAnimKeyFrameStreamWrite(const RtAnimAnimation* animation, RwStream* stream) | [source/game_sa/RenderWare/rw/rphanim.cpp:124](../../source/game_sa/RenderWare/rw/rphanim.cpp#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpHAnimKeyFrameStreamGetSize(const RtAnimAnimation* animation) | [source/game_sa/RenderWare/rw/rphanim.cpp:128](../../source/game_sa/RenderWare/rw/rphanim.cpp#L128) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpHAnimFrameSetID(RwFrame* frame, RwInt32 id) | [source/game_sa/RenderWare/rw/rphanim.cpp:132](../../source/game_sa/RenderWare/rw/rphanim.cpp#L132) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpHAnimFrameGetID(RwFrame* frame) | [source/game_sa/RenderWare/rw/rphanim.cpp:136](../../source/game_sa/RenderWare/rw/rphanim.cpp#L136) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

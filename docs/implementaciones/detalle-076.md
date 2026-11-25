<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1501"></a>

## Globales y helpers: source/game_sa/StencilShadows.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StencilMulPoint(const CMatrix& m, const CVector& v) | [source/game_sa/StencilShadows.cpp:50](../../source/game_sa/StencilShadows.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StencilTransformFromObjectSpace(const CEntity* entity, const CVector& offset) | [source/game_sa/StencilShadows.cpp:60](../../source/game_sa/StencilShadows.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StencilSignedDistSq(const CVector& cam, CEntity* entity) | [source/game_sa/StencilShadows.cpp:71](../../source/game_sa/StencilShadows.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StencilObjectInvalid(CEntity* owner) | [source/game_sa/StencilShadows.cpp:96](../../source/game_sa/StencilShadows.cpp#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StencilToggleEdge(RxVertexIndex* edges, uint16& count, uint16 i0, uint16 i1) | [source/game_sa/StencilShadows.cpp:105](../../source/game_sa/StencilShadows.cpp#L105) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BuildShadowVolume(CStencilShadowObject* obj, bool forVehicle) | [source/game_sa/StencilShadows.cpp:127](../../source/game_sa/StencilShadows.cpp#L127) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo s_SunPosNrm: source/game_sa/StencilShadows.cpp:33 (0x008D5244) |

<a id="grupo-1502"></a>

## Globales y helpers: source/game_sa/Streaming.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeSector(float v) | [source/game_sa/Streaming.cpp:52](../../source/game_sa/Streaming.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeLodSector(float v) | [source/game_sa/Streaming.cpp:54](../../source/game_sa/Streaming.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MemoryUsedBelow(size_t goal) | [source/game_sa/Streaming.cpp:56](../../source/game_sa/Streaming.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_memoryUsedBytes: source/game_sa/Streaming.h:252 (0x008E4CB4) |

<a id="grupo-1503"></a>

## Globales y helpers: source/game_sa/Streaming.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsModelDFF(int32 model) | [source/game_sa/Streaming.h:88](../../source/game_sa/Streaming.h#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelTXD(int32 model) | [source/game_sa/Streaming.h:89](../../source/game_sa/Streaming.h#L89) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelCOL(int32 model) | [source/game_sa/Streaming.h:90](../../source/game_sa/Streaming.h#L90) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelIPL(int32 model) | [source/game_sa/Streaming.h:91](../../source/game_sa/Streaming.h#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelDAT(int32 model) | [source/game_sa/Streaming.h:92](../../source/game_sa/Streaming.h#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelIFP(int32 model) | [source/game_sa/Streaming.h:93](../../source/game_sa/Streaming.h#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelRRR(int32 model) | [source/game_sa/Streaming.h:94](../../source/game_sa/Streaming.h#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelSCM(int32 model) | [source/game_sa/Streaming.h:95](../../source/game_sa/Streaming.h#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetModelType(int32 model) | [source/game_sa/Streaming.h:97](../../source/game_sa/Streaming.h#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DFFToModelId(int32 relativeId) | [source/game_sa/Streaming.h:129](../../source/game_sa/Streaming.h#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TXDToModelId(int32 relativeId) | [source/game_sa/Streaming.h:130](../../source/game_sa/Streaming.h#L130) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| COLToModelId(int32 relativeId) | [source/game_sa/Streaming.h:131](../../source/game_sa/Streaming.h#L131) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IPLToModelId(int32 relativeId) | [source/game_sa/Streaming.h:132](../../source/game_sa/Streaming.h#L132) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DATToModelId(size_t relativeId) | [source/game_sa/Streaming.h:133](../../source/game_sa/Streaming.h#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IFPToModelId(int32 relativeId) | [source/game_sa/Streaming.h:134](../../source/game_sa/Streaming.h#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RRRToModelId(int32 relativeId) | [source/game_sa/Streaming.h:135](../../source/game_sa/Streaming.h#L135) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SCMToModelId(int32 relativeId) | [source/game_sa/Streaming.h:136](../../source/game_sa/Streaming.h#L136) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToDFF(int32 absId) | [source/game_sa/Streaming.h:139](../../source/game_sa/Streaming.h#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToTXD(int32 absId) | [source/game_sa/Streaming.h:140](../../source/game_sa/Streaming.h#L140) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToCOL(int32 absId) | [source/game_sa/Streaming.h:141](../../source/game_sa/Streaming.h#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToIPL(int32 absId) | [source/game_sa/Streaming.h:142](../../source/game_sa/Streaming.h#L142) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToDAT(int32 absId) | [source/game_sa/Streaming.h:143](../../source/game_sa/Streaming.h#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToIFP(int32 absId) | [source/game_sa/Streaming.h:144](../../source/game_sa/Streaming.h#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToRRR(int32 absId) | [source/game_sa/Streaming.h:145](../../source/game_sa/Streaming.h#L145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToSCM(int32 absId) | [source/game_sa/Streaming.h:146](../../source/game_sa/Streaming.h#L146) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1504"></a>

## Globales y helpers: source/game_sa/StuckCarCheck.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GetVehicleNoBoundsCheck(int32 handle) | [source/game_sa/StuckCarCheck.cpp:25](../../source/game_sa/StuckCarCheck.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1505"></a>

## Globales y helpers: source/game_sa/StuntJumpManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * RawPool() | [source/game_sa/StuntJumpManager.cpp:39](../../source/game_sa/StuntJumpManager.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StuntJumpPoolFlush(StuntJumpPoolRaw* pool, void* /*edx*/) | [source/game_sa/StuntJumpManager.cpp:63](../../source/game_sa/StuntJumpManager.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CStuntJumpManager::PoolFlush 0x0049C950 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StuntJumpPoolClear(StuntJumpPoolRaw* pool, void* /*edx*/) | [source/game_sa/StuntJumpManager.cpp:78](../../source/game_sa/StuntJumpManager.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CStuntJumpManager::PoolClear 0x0049C9A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StuntJumpPoolFlushSecondCopy(StuntJumpPoolRaw* pool, void* /*edx*/) | [source/game_sa/StuntJumpManager.cpp:134](../../source/game_sa/StuntJumpManager.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CStuntJumpManager::PoolFlushSecondCopy 0x0049C440 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall StuntJumpPoolGetAt(StuntJumpPoolRaw* pool, void* /*edx*/, int32 index) | [source/game_sa/StuntJumpManager.cpp:140](../../source/game_sa/StuntJumpManager.cpp#L140) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CStuntJumpManager::PoolGetAt 0x0049C3B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StuntJumpTimeStepInMS() | [source/game_sa/StuntJumpManager.cpp:148](../../source/game_sa/StuntJumpManager.cpp#L148) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:38 (0x00B7CB5C) |
| SetJumpState(eJumpState state) | [source/game_sa/StuntJumpManager.cpp:153](../../source/game_sa/StuntJumpManager.cpp#L153) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_jumpState: source/game_sa/StuntJumpManager.h:39 (0x00A9A898) |
| ResetAllJumps() | [source/game_sa/StuntJumpManager.cpp:418](../../source/game_sa/StuntJumpManager.cpp#L418) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StuntJumpTestCode() | [source/game_sa/StuntJumpManager.cpp:426](../../source/game_sa/StuntJumpManager.cpp#L426) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1506"></a>

## Globales y helpers: source/game_sa/TagManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsTagInArea(const CRect& area, const tTagDesc& tag) | [source/game_sa/TagManager.cpp:122](../../source/game_sa/TagManager.cpp#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1507"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedGroupDefaultTaskAllocatorFollowAnyMeansConstructorExe(CPedGroupDefaultTaskAllocatorFollowAnyMeans* self, void* /*edx*/) | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.cpp:7](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedGroupDefaultTaskAllocatorFollowAnyMeans::Constructor 0x005F6450 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1508"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocator.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TaskAllocatorDestructorBody(CTaskAllocator* self, void* /*edx*/) | [source/game_sa/Tasks/Allocators/TaskAllocator.cpp:26](../../source/game_sa/Tasks/Allocators/TaskAllocator.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocator::Destructor 0x0069BB40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall TaskAllocatorScalarDeletingDestructor(CTaskAllocator* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/Allocators/TaskAllocator.cpp:32](../../source/game_sa/Tasks/Allocators/TaskAllocator.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocator::ScalarDeletingDestructor 0x0069DAC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1509"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall AttackScalarDeletingDestructor(CTaskAllocatorAttack* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp:57](../../source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorAttack::ScalarDeletingDestructor 0x0069DA00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1510"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClosestThreatDistance(const CVector& a, const CVector& b) | [source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp:92](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall KillThreatsBasicScalarDeletingDestructor(CTaskAllocatorKillThreatsBasic* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp:163](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp#L163) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorKillThreatsBasic::ScalarDeletingDestructor 0x0069DA40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1511"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KillThreatsBasicRandomGroupDestructorBody(CTaskAllocatorKillThreatsBasicRandomGroup* self, void* /*edx*/) | [source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp:83](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorKillThreatsBasicRandomGroup::Destructor 0x005F6920 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1512"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall KillThreatsDrivebyScalarDeletingDestructor(CTaskAllocatorKillThreatsDriveby* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp:119](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorKillThreatsDriveby::ScalarDeletingDestructor 0x0069DA80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1513"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PlayerCommandAttackConstructorBody(CTaskAllocatorPlayerCommandAttack* self, void* /*edx*/, CPed* target, int32 groupTargetId, uint32 value) | [source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp:98](../../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorPlayerCommandAttack::Constructor 0x0069C480 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlayerCommandAttackDestructorBody(CTaskAllocatorPlayerCommandAttack* self, void* /*edx*/) | [source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp:109](../../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorPlayerCommandAttack::DestructorBody 0x0069DB40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PlayerCommandAttackScalarDeletingDestructor(CTaskAllocatorPlayerCommandAttack* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp:116](../../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp#L116) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorPlayerCommandAttack::ScalarDeletingDestructor 0x0069DB00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1514"></a>

## Globales y helpers: source/game_sa/Tasks/TaskComplexSequence.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SequenceDestructorBody(CTaskComplexSequence* self, void* /*edx*/) | [source/game_sa/Tasks/TaskComplexSequence.cpp:31](../../source/game_sa/Tasks/TaskComplexSequence.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexSequence::Destructor 0x006389F0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1515"></a>

## Globales y helpers: source/game_sa/Tasks/TaskComplexSequence.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsActive() const | [source/game_sa/Tasks/TaskComplexSequence.h:58](../../source/game_sa/Tasks/TaskComplexSequence.h#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1516"></a>

## Globales y helpers: source/game_sa/Tasks/TaskComplexUseSequence.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote145ABody_CTaskComplexUseSequence() | [source/game_sa/Tasks/TaskComplexUseSequence.cpp:59](../../source/game_sa/Tasks/TaskComplexUseSequence.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1517"></a>

## Globales y helpers: source/game_sa/Tasks/TaskManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeleteTaskAndNull(CTask*& task) | [source/game_sa/Tasks/TaskManager.cpp:58](../../source/game_sa/Tasks/TaskManager.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FindFirstTaskOfType(CTask* task, eTaskType type) | [source/game_sa/Tasks/TaskManager.cpp:64](../../source/game_sa/Tasks/TaskManager.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1518"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BeInShopDestructorBody(CTaskInteriorBeInShop* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp:34](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp#L34) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskInteriorBeInShop::Destructor 0x00675460 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146ABeInShopDestructorBody() | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp:48](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1519"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InteriorInfoHeading(const InteriorInfo_t* info) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp:61](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1520"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LieInBedTransformPointXY(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp:124](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

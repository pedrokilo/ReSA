<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 09/11/2026 (Ciudad de México):** 872 grupos / 9609 funciones reconstruidas; 9609 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1321"></a>

## Globales y helpers: source/game_sa/Streaming.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsModelDFF(int32 model) | [source/game_sa/Streaming.h:81](../../source/game_sa/Streaming.h#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelTXD(int32 model) | [source/game_sa/Streaming.h:82](../../source/game_sa/Streaming.h#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelCOL(int32 model) | [source/game_sa/Streaming.h:83](../../source/game_sa/Streaming.h#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelIPL(int32 model) | [source/game_sa/Streaming.h:84](../../source/game_sa/Streaming.h#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelDAT(int32 model) | [source/game_sa/Streaming.h:85](../../source/game_sa/Streaming.h#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelIFP(int32 model) | [source/game_sa/Streaming.h:86](../../source/game_sa/Streaming.h#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelRRR(int32 model) | [source/game_sa/Streaming.h:87](../../source/game_sa/Streaming.h#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelSCM(int32 model) | [source/game_sa/Streaming.h:88](../../source/game_sa/Streaming.h#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetModelType(int32 model) | [source/game_sa/Streaming.h:90](../../source/game_sa/Streaming.h#L90) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DFFToModelId(int32 relativeId) | [source/game_sa/Streaming.h:122](../../source/game_sa/Streaming.h#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TXDToModelId(int32 relativeId) | [source/game_sa/Streaming.h:123](../../source/game_sa/Streaming.h#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| COLToModelId(int32 relativeId) | [source/game_sa/Streaming.h:124](../../source/game_sa/Streaming.h#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IPLToModelId(int32 relativeId) | [source/game_sa/Streaming.h:125](../../source/game_sa/Streaming.h#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DATToModelId(size_t relativeId) | [source/game_sa/Streaming.h:126](../../source/game_sa/Streaming.h#L126) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IFPToModelId(int32 relativeId) | [source/game_sa/Streaming.h:127](../../source/game_sa/Streaming.h#L127) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RRRToModelId(int32 relativeId) | [source/game_sa/Streaming.h:128](../../source/game_sa/Streaming.h#L128) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SCMToModelId(int32 relativeId) | [source/game_sa/Streaming.h:129](../../source/game_sa/Streaming.h#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToDFF(int32 absId) | [source/game_sa/Streaming.h:132](../../source/game_sa/Streaming.h#L132) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToTXD(int32 absId) | [source/game_sa/Streaming.h:133](../../source/game_sa/Streaming.h#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToCOL(int32 absId) | [source/game_sa/Streaming.h:134](../../source/game_sa/Streaming.h#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToIPL(int32 absId) | [source/game_sa/Streaming.h:135](../../source/game_sa/Streaming.h#L135) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToDAT(int32 absId) | [source/game_sa/Streaming.h:136](../../source/game_sa/Streaming.h#L136) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToIFP(int32 absId) | [source/game_sa/Streaming.h:137](../../source/game_sa/Streaming.h#L137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToRRR(int32 absId) | [source/game_sa/Streaming.h:138](../../source/game_sa/Streaming.h#L138) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToSCM(int32 absId) | [source/game_sa/Streaming.h:139](../../source/game_sa/Streaming.h#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1322"></a>

## Globales y helpers: source/game_sa/StuckCarCheck.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GetVehicleNoBoundsCheck(int32 handle) | [source/game_sa/StuckCarCheck.cpp:25](../../source/game_sa/StuckCarCheck.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1323"></a>

## Globales y helpers: source/game_sa/StuntJumpManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * RawPool() | [source/game_sa/StuntJumpManager.cpp:52](../../source/game_sa/StuntJumpManager.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo mp_poolStuntJumps: source/game_sa/StuntJumpManager.h:28 (0x00A9A888) |
| * StuntJumpPoolNew(StuntJumpPoolRaw* pool) | [source/game_sa/StuntJumpManager.cpp:57](../../source/game_sa/StuntJumpManager.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StuntJumpTimeStepInMS() | [source/game_sa/StuntJumpManager.cpp:79](../../source/game_sa/StuntJumpManager.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| SetJumpState(eJumpState state) | [source/game_sa/StuntJumpManager.cpp:84](../../source/game_sa/StuntJumpManager.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_jumpState: source/game_sa/StuntJumpManager.h:33 (0x00A9A898) |
| ResetAllJumps() | [source/game_sa/StuntJumpManager.cpp:351](../../source/game_sa/StuntJumpManager.cpp#L351) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StuntJumpTestCode() | [source/game_sa/StuntJumpManager.cpp:359](../../source/game_sa/StuntJumpManager.cpp#L359) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1324"></a>

## Globales y helpers: source/game_sa/TagManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsTagInArea(const CRect& area, const tTagDesc& tag) | [source/game_sa/TagManager.cpp:122](../../source/game_sa/TagManager.cpp#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1325"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClosestThreatDistance(const CVector& a, const CVector& b) | [source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp:92](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1326"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PlayerCommandAttackConstructorBody(CTaskAllocatorPlayerCommandAttack* self, void* /*edx*/, CPed* target, int32 groupTargetId, uint32 value) | [source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp:98](../../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorPlayerCommandAttack::Constructor 0x0069C480 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1327"></a>

## Globales y helpers: source/game_sa/Tasks/TaskComplexSequence.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SequenceDestructorBody(CTaskComplexSequence* self, void* /*edx*/) | [source/game_sa/Tasks/TaskComplexSequence.cpp:31](../../source/game_sa/Tasks/TaskComplexSequence.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexSequence::Destructor 0x006389F0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1328"></a>

## Globales y helpers: source/game_sa/Tasks/TaskComplexSequence.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsActive() const | [source/game_sa/Tasks/TaskComplexSequence.h:57](../../source/game_sa/Tasks/TaskComplexSequence.h#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1329"></a>

## Globales y helpers: source/game_sa/Tasks/TaskManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeleteTaskAndNull(CTask*& task) | [source/game_sa/Tasks/TaskManager.cpp:58](../../source/game_sa/Tasks/TaskManager.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FindFirstTaskOfType(CTask* task, eTaskType type) | [source/game_sa/Tasks/TaskManager.cpp:64](../../source/game_sa/Tasks/TaskManager.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1330"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InteriorInfoHeading(const InteriorInfo_t* info) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp:61](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1331"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LieInBedTransformPointXY(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp:112](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1332"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SitAtDeskRandom(float scale) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp:79](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitAtDeskMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp:140](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp#L140) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1333"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetMaxEntityDist2DRegs(void* self, uint32 entryEdx, uint32 distanceBits) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp:12](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_snTimeInMilliseconds: source/game_sa/Timer.h:44 (0x00B7CB84) |

<a id="grupo-1334"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MakeSurePedHasWeaponInHand(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp:139](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1335"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall AttractorPartnerWaitConstructorBody(CTaskComplexAttractorPartnerWait* self, void* /*edx*/, uint32 flag, uint32 value) | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexAttractorPartnerWait::Constructor 0x00633250 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1336"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetAnimGroupOf(CVehicle* veh) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp:58](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1337"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DieDestructorBody(CTaskComplexDie* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexDie::Destructor 0x006300C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1338"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexDragPedFromCar__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1339"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarDriveWanderDestructorBody(CTaskComplexCarDriveWander* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote140ACarDriveWanderDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1340"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AddLeaderGroupEvent(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:97](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsCarTooFastToEnter(const CVehicle* car, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:106](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L106) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fMaxSpeed_CanDragPedOut: source/game_sa/CarEnterExit.h:17 (0x0086F104); Posible dato fijo ms_fMaxSpeed_PlayerCanDragPedOut: source/game_sa/CarEnterExit.h:18 (0x0086F108) |
| OpenTargetCarDoor(CVehicle* car, int32 door) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:116](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L116) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyCruiseSpeed(CVehicle* car, float cruiseSpeed) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:123](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

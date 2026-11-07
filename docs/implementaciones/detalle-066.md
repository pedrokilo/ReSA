<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 07/11/2026 (Ciudad de México):** 858 grupos / 9304 funciones reconstruidas; 9304 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1301"></a>

## Globales y helpers: source/game_sa/TagManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsTagInArea(const CRect& area, const tTagDesc& tag) | [source/game_sa/TagManager.cpp:121](../../source/game_sa/TagManager.cpp#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1302"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClosestThreatDistance(const CVector& a, const CVector& b) | [source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp:92](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1303"></a>

## Globales y helpers: source/game_sa/Tasks/TaskComplexSequence.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsActive() const | [source/game_sa/Tasks/TaskComplexSequence.h:57](../../source/game_sa/Tasks/TaskComplexSequence.h#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1304"></a>

## Globales y helpers: source/game_sa/Tasks/TaskManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeleteTaskAndNull(CTask*& task) | [source/game_sa/Tasks/TaskManager.cpp:58](../../source/game_sa/Tasks/TaskManager.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FindFirstTaskOfType(CTask* task, eTaskType type) | [source/game_sa/Tasks/TaskManager.cpp:64](../../source/game_sa/Tasks/TaskManager.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1305"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InteriorInfoHeading(const InteriorInfo_t* info) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp:60](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1306"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LieInBedTransformPointXY(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp:112](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1307"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SitAtDeskRandom(float scale) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp:78](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitAtDeskMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp:139](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1308"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetMaxEntityDist2DRegs(void* self, uint32 entryEdx, uint32 distanceBits) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp:12](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_snTimeInMilliseconds: source/game_sa/Timer.h:44 (0x00B7CB84) |

<a id="grupo-1309"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MakeSurePedHasWeaponInHand(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp:138](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp#L138) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1310"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetAnimGroupOf(CVehicle* veh) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp:58](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1311"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexDragPedFromCar__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1312"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AddLeaderGroupEvent(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:97](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsCarTooFastToEnter(const CVehicle* car, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:106](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L106) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fMaxSpeed_CanDragPedOut: source/game_sa/CarEnterExit.h:17 (0x0086F104); Posible dato fijo ms_fMaxSpeed_PlayerCanDragPedOut: source/game_sa/CarEnterExit.h:18 (0x0086F108) |
| OpenTargetCarDoor(CVehicle* car, int32 door) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:116](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L116) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyCruiseSpeed(CVehicle* car, float cruiseSpeed) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:123](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1313"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FollowNodeRouteNodePos(CNodeAddress addr) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:74](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ThePaths: source/game_sa/PathFind.h:633 (0x0096F050) |
| FollowNodeRouteNodeLoaded(CNodeAddress addr) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:79](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ThePaths: source/game_sa/PathFind.h:633 (0x0096F050) |

<a id="grupo-1314"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FootstepsSquaredMagnitude(const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:95](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::SquaredMagnitudeExe 0x00406DA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDistSqZXY(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:99](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDistSqXZY(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:103](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDot(const CVector& w, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:107](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L107) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDist2D(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:111](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1315"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| X87Round24(double value) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp:84](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1316"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToCarDoorIsVehicleInRange(CTaskComplexGoToCarDoorAndStandStill* task, uint32 entryEdx, const CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1317"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| NoPedOrNoHp(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:54](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & CriminalField(void* object, size_t offset) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:420](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L420) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CriminalHateEvent(CPed* target, CPed* recipient, eTaskType taskType) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:423](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L423) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1318"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * KillNewTask(Args... args) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp:90](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp#L90) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GiveGangsterPistolIfPlayerStopped(CPed* ped, CPed* targetPlayer) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp:101](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1319"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KillMeleeTargetDistance(CPed* ped, const CVector& targetPos) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp:134](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KillMeleeTargetDistanceSq(CPed* ped, const CVector& targetPos) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp:139](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1320"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComputeTargetDoor(CTaskComplexLeaveCar* task, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp:255](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp#L255) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RegisterPedLeaving(CTaskComplexLeaveCar* task, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp:262](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp#L262) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CreateLineUpUtility(CTaskComplexLeaveCar* task) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp:279](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp#L279) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NewLeaveCarSubTask(Args... args) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp:529](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp#L529) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

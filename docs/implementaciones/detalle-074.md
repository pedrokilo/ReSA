<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 19/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1461"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DriveToPointDestructorBody(CTaskComplexDriveToPoint* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexDriveToPoint::Destructor 0x0063CE70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1462"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote140ACarDriveWanderDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1463"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnterAnyCarAsDriverDestructorBody(CTaskComplexEnterAnyCarAsDriver* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexEnterAnyCarAsDriver::Destructor 0x0063D100 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABody_CTaskComplexEnterAnyCarAsDriver() | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp:57](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1464"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnterCarDestructorBody(CTaskComplexEnterCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:61](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L61) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexEnterCar::Destructor 0x0063DFA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABody_CTaskComplexEnterCar() | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:75](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddLeaderGroupEvent(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:117](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsCarTooFastToEnter(const CVehicle* car, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:126](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L126) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fMaxSpeed_CanDragPedOut: source/game_sa/CarEnterExit.h:17 (0x0086F104); Posible dato fijo ms_fMaxSpeed_PlayerCanDragPedOut: source/game_sa/CarEnterExit.h:18 (0x0086F108) |
| OpenTargetCarDoor(CVehicle* car, int32 door) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:136](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L136) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyCruiseSpeed(CVehicle* car, float cruiseSpeed) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:143](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1465"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnterCarAsDriverDestructorBody(CTaskComplexEnterCarAsDriver* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexEnterCarAsDriver::Destructor 0x00640330 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1466"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnterCarAsPassengerDestructorBody(CTaskComplexEnterCarAsPassenger* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexEnterCarAsPassenger::Destructor 0x00640380 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1467"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComplexEvasiveStepDestructorBody(CTaskComplexEvasiveStep* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexEvasiveStep::Destructor 0x00653350 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AComplexEvasiveStepDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1468"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFireOnFoot.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExtinguishFireOnFootDestructorBody(CTaskComplexExtinguishFireOnFoot* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFireOnFoot.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFireOnFoot.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexExtinguishFireOnFoot::DestructorBody 0x006598B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1469"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FacialDestructorBody(CTaskComplexFacial* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexFacial::Destructor 0x00690D90 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1470"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFallAndGetUpDestructorBody(CTaskComplexFallAndGetUp* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Revalidada en migración: CTaskComplexFallAndGetUp::DestructorBody 0x00678790 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall CTaskComplexFallAndGetUpScalarDeletingDestructor(CTaskComplexFallAndGetUp* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexFallAndGetUp::ScalarDeletingDestructor 0x0067CB50 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote154ABody_CTaskComplexFallAndGetUp() | [source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1471"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FallToDeathDestructorBody(CTaskComplexFallToDeath* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexFallToDeath::Destructor 0x006790B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1472"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFleeAnyMeansDestructorBody(CTaskComplexFleeAnyMeans* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFleeAnyMeans::Destructor 0x0065CD10 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexFleeAnyMeans() | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1473"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFleeEntityDestructorBody(CTaskComplexFleeEntity* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFleeEntity::Destructor 0x0065B9D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexFleeEntity() | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1474"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFleePointDestructorBody(CTaskComplexFleePoint* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp#L33) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFleePoint::Destructor 0x0065B410 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexFleePoint() | [source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1475"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFleeShootingDestructorBody(CTaskComplexFleeShooting* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFleeShooting::Destructor 0x0065C9A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexFleeShooting() | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1476"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFollowNodeRouteDestructorBody(CTaskComplexFollowNodeRoute* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L35) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFollowNodeRoute::Destructor 0x0066EB70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexFollowNodeRoute() | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:49](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FollowNodeRouteNodePos(CNodeAddress addr) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:94](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ThePaths: source/game_sa/PathFind.h:659 (0x0096F050) |
| FollowNodeRouteNodeLoaded(CNodeAddress addr) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:99](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ThePaths: source/game_sa/PathFind.h:659 (0x0096F050) |

<a id="grupo-1477"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & FieldAt(void* object, uint32 offset) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp:74](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FollowPatrolRouteGetTaskType(CTaskComplexFollowPatrolRoute* /*self*/, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp:80](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp#L80) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexFollowPatrolRoute::GetTaskType 0x00670A60 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall FollowPatrolRouteControlSubTask(CTaskComplex* self, void* /*edx*/, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp:134](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexFollowPatrolRoute::ControlSubTask 0x0066BB00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1478"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FootstepsSquaredMagnitude(const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:149](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L149) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::SquaredMagnitudeExe 0x00406DA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDistSqZXY(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:153](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L153) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDistSqXZY(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:157](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDot(const CVector& w, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:161](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L161) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDist2D(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:165](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L165) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1479"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote145ABody_CTaskComplexFollowPointRoute() | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp:57](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1480"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GangFollowerCalculateOffsetPosition(CTaskComplexGangFollower* self, void* /*edx*/, CVector* out) | [source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp:95](../../source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_offsets: source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.h:28 (0x00C196E8) |

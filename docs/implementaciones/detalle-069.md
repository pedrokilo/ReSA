<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 880 grupos / 9863 funciones reconstruidas; 9863 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1361"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DriveToPointDestructorBody(CTaskComplexDriveToPoint* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexDriveToPoint::Destructor 0x0063CE70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1362"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote140ACarDriveWanderDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1363"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnterAnyCarAsDriverDestructorBody(CTaskComplexEnterAnyCarAsDriver* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexEnterAnyCarAsDriver::Destructor 0x0063D100 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABody_CTaskComplexEnterAnyCarAsDriver() | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp:57](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1364"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnterCarDestructorBody(CTaskComplexEnterCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:61](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L61) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexEnterCar::Destructor 0x0063DFA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABody_CTaskComplexEnterCar() | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:75](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddLeaderGroupEvent(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:117](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsCarTooFastToEnter(const CVehicle* car, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:126](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L126) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fMaxSpeed_CanDragPedOut: source/game_sa/CarEnterExit.h:17 (0x0086F104); Posible dato fijo ms_fMaxSpeed_PlayerCanDragPedOut: source/game_sa/CarEnterExit.h:18 (0x0086F108) |
| OpenTargetCarDoor(CVehicle* car, int32 door) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:136](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L136) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyCruiseSpeed(CVehicle* car, float cruiseSpeed) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:143](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1365"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnterCarAsDriverDestructorBody(CTaskComplexEnterCarAsDriver* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexEnterCarAsDriver::Destructor 0x00640330 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1366"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnterCarAsPassengerDestructorBody(CTaskComplexEnterCarAsPassenger* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexEnterCarAsPassenger::Destructor 0x00640380 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1367"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComplexEvasiveStepDestructorBody(CTaskComplexEvasiveStep* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexEvasiveStep::Destructor 0x00653350 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AComplexEvasiveStepDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1368"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FacialDestructorBody(CTaskComplexFacial* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexFacial::Destructor 0x00690D90 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1369"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FallToDeathDestructorBody(CTaskComplexFallToDeath* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexFallToDeath::Destructor 0x006790B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1370"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFleePointDestructorBody(CTaskComplexFleePoint* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp#L33) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFleePoint::Destructor 0x0065B410 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexFleePoint() | [source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1371"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFollowNodeRouteDestructorBody(CTaskComplexFollowNodeRoute* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L35) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFollowNodeRoute::Destructor 0x0066EB70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexFollowNodeRoute() | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:49](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FollowNodeRouteNodePos(CNodeAddress addr) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:94](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ThePaths: source/game_sa/PathFind.h:633 (0x0096F050) |
| FollowNodeRouteNodeLoaded(CNodeAddress addr) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:99](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ThePaths: source/game_sa/PathFind.h:633 (0x0096F050) |

<a id="grupo-1372"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FootstepsSquaredMagnitude(const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:95](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::SquaredMagnitudeExe 0x00406DA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDistSqZXY(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:99](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDistSqXZY(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:103](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDot(const CVector& w, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:107](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L107) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDist2D(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:111](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1373"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFollowPointRouteDestructorBody(CTaskComplexFollowPointRoute* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp#L33) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexFollowPointRoute() | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1374"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGangJoinRespondDestructorBody(CTaskComplexGangJoinRespond* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGangJoinRespond::Destructor 0x00661720 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexGangJoinRespond() | [source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1375"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetUpAndStandStillDestructorBody(CTaskComplexGetUpAndStandStill* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexGetUpAndStandStill::Destructor 0x00678160 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1376"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| X87Round24(double value) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp:96](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1377"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGoToAttractorDestructorBody(CTaskComplexGoToAttractor* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp#L32) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGoToAttractor::Destructor 0x0066B6A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexGoToAttractor() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp:46](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1378"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToCarDoorIsVehicleInRange(CTaskComplexGoToCarDoorAndStandStill* task, uint32 entryEdx, const CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1379"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToPointAndStandStillDestructorBody(CTaskComplexGoToPointAndStandStill* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexGoToPointAndStandStill::Destructor 0x00668630 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AGoToPointDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp:52](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1380"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGoToPointAndStandStillAndAchieveHeadingDestructorBody(CTaskComplexGoToPointAndStandStillAndAchieveHeading* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGoToPointAndStandStillAndAchieveHeading::Destructor 0x00668D40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskComplexGoToPointAndStandStillAndAchieveHeading() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

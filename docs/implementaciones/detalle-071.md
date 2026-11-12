<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 893 grupos / 10321 funciones reconstruidas; 10321 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1401"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFleePointDestructorBody(CTaskComplexFleePoint* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp#L33) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFleePoint::Destructor 0x0065B410 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexFleePoint() | [source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1402"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFleeShootingDestructorBody(CTaskComplexFleeShooting* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFleeShooting::Destructor 0x0065C9A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexFleeShooting() | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1403"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFollowNodeRouteDestructorBody(CTaskComplexFollowNodeRoute* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L35) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFollowNodeRoute::Destructor 0x0066EB70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexFollowNodeRoute() | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:49](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FollowNodeRouteNodePos(CNodeAddress addr) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:94](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ThePaths: source/game_sa/PathFind.h:636 (0x0096F050) |
| FollowNodeRouteNodeLoaded(CNodeAddress addr) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:99](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ThePaths: source/game_sa/PathFind.h:636 (0x0096F050) |

<a id="grupo-1404"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FootstepsSquaredMagnitude(const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:105](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L105) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::SquaredMagnitudeExe 0x00406DA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDistSqZXY(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:109](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDistSqXZY(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:113](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L113) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDot(const CVector& w, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:117](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDist2D(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:121](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1405"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFollowPointRouteDestructorBody(CTaskComplexFollowPointRoute* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp#L33) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexFollowPointRoute() | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1406"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGangJoinRespondDestructorBody(CTaskComplexGangJoinRespond* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGangJoinRespond::Destructor 0x00661720 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexGangJoinRespond() | [source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1407"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGangLeaderDestructorBody(CTaskComplexGangLeader* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp:54](../../source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp#L54) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGangLeader::Destructor 0x0065DF30 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote160ABody_CTaskComplexGangLeader() | [source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp:68](../../source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1408"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetUpAndStandStillDestructorBody(CTaskComplexGetUpAndStandStill* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexGetUpAndStandStill::Destructor 0x00678160 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1409"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| X87Round24(double value) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp:96](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1410"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGoToAttractorDestructorBody(CTaskComplexGoToAttractor* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp#L32) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGoToAttractor::Destructor 0x0066B6A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexGoToAttractor() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp:46](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1411"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToCarDoorIsVehicleInRange(CTaskComplexGoToCarDoorAndStandStill* task, uint32 entryEdx, const CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexGoToCarDoorAndStandStillDestructorBody(CTaskComplexGoToCarDoorAndStandStill* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp#L42) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGoToCarDoorAndStandStill::Destructor 0x0064A580 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote159ABody_CTaskComplexGoToCarDoorAndStandStill() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp:56](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1412"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToPointAndStandStillDestructorBody(CTaskComplexGoToPointAndStandStill* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexGoToPointAndStandStill::Destructor 0x00668630 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AGoToPointDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp:52](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1413"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGoToPointAndStandStillAndAchieveHeadingDestructorBody(CTaskComplexGoToPointAndStandStillAndAchieveHeading* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGoToPointAndStandStillAndAchieveHeading::Destructor 0x00668D40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskComplexGoToPointAndStandStillAndAchieveHeading() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1414"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGoToPointAnyMeansDestructorBody(CTaskComplexGoToPointAnyMeans* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp#L33) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGoToPointAnyMeans::Destructor 0x0066B830 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote160ABody_CTaskComplexGoToPointAnyMeans() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1415"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToPointAimingDestructorCall(CTaskComplexGoToPointShooting* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGoToPointAiming::Destructor 0x00668870 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GoToPointShootingDestructorBody(CTaskComplexGoToPointShooting* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexGoToPointShooting::Destructor 0x0066DFA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147AAimingDestructorCall() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1416"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexHitByGunResponseDestructorBody(CTaskComplexHitByGunResponse* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexHitByGunResponse::Destructor 0x00631E30 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexHitByGunResponse() | [source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1417"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexHitResponseDestructorBody(CTaskComplexHitResponse* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexHitResponse::Destructor 0x00631DA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexHitResponse() | [source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1418"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InAirAndLandDestructorBody(CTaskComplexInAirAndLand* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexInAirAndLand::Destructor 0x00678CC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1419"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| JumpDestructorBody(CTaskComplexJump* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexJump::Destructor 0x0067A060 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1420"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| NoPedOrNoHp(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:55](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & CriminalField(void* object, size_t offset) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:421](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L421) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CriminalHateEvent(CPed* target, CPed* recipient, eTaskType taskType) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:424](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L424) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

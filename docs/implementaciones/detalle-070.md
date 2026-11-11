<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 884 grupos / 10008 funciones reconstruidas; 10008 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1381"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FacialDestructorBody(CTaskComplexFacial* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexFacial::Destructor 0x00690D90 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1382"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFallAndGetUpDestructorBody(CTaskComplexFallAndGetUp* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall CTaskComplexFallAndGetUpScalarDeletingDestructor(CTaskComplexFallAndGetUp* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexFallAndGetUp::ScalarDeletingDestructor 0x0067CB50 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote154ABody_CTaskComplexFallAndGetUp() | [source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1383"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FallToDeathDestructorBody(CTaskComplexFallToDeath* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexFallToDeath::Destructor 0x006790B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1384"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFleeAnyMeansDestructorBody(CTaskComplexFleeAnyMeans* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFleeAnyMeans::Destructor 0x0065CD10 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexFleeAnyMeans() | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1385"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFleeEntityDestructorBody(CTaskComplexFleeEntity* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFleeEntity::Destructor 0x0065B9D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexFleeEntity() | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1386"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFleePointDestructorBody(CTaskComplexFleePoint* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp#L33) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFleePoint::Destructor 0x0065B410 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexFleePoint() | [source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1387"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFleeShootingDestructorBody(CTaskComplexFleeShooting* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFleeShooting::Destructor 0x0065C9A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexFleeShooting() | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1388"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFollowNodeRouteDestructorBody(CTaskComplexFollowNodeRoute* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L35) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFollowNodeRoute::Destructor 0x0066EB70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexFollowNodeRoute() | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:49](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FollowNodeRouteNodePos(CNodeAddress addr) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:94](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ThePaths: source/game_sa/PathFind.h:633 (0x0096F050) |
| FollowNodeRouteNodeLoaded(CNodeAddress addr) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:99](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ThePaths: source/game_sa/PathFind.h:633 (0x0096F050) |

<a id="grupo-1389"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FootstepsSquaredMagnitude(const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:95](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::SquaredMagnitudeExe 0x00406DA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDistSqZXY(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:99](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDistSqXZY(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:103](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDot(const CVector& w, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:107](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L107) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDist2D(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:111](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1390"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFollowPointRouteDestructorBody(CTaskComplexFollowPointRoute* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp#L33) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexFollowPointRoute() | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1391"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGangJoinRespondDestructorBody(CTaskComplexGangJoinRespond* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGangJoinRespond::Destructor 0x00661720 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexGangJoinRespond() | [source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1392"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetUpAndStandStillDestructorBody(CTaskComplexGetUpAndStandStill* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexGetUpAndStandStill::Destructor 0x00678160 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1393"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| X87Round24(double value) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp:96](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1394"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGoToAttractorDestructorBody(CTaskComplexGoToAttractor* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp#L32) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGoToAttractor::Destructor 0x0066B6A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexGoToAttractor() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp:46](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1395"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToCarDoorIsVehicleInRange(CTaskComplexGoToCarDoorAndStandStill* task, uint32 entryEdx, const CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexGoToCarDoorAndStandStillDestructorBody(CTaskComplexGoToCarDoorAndStandStill* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp#L42) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGoToCarDoorAndStandStill::Destructor 0x0064A580 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote159ABody_CTaskComplexGoToCarDoorAndStandStill() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp:56](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1396"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToPointAndStandStillDestructorBody(CTaskComplexGoToPointAndStandStill* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexGoToPointAndStandStill::Destructor 0x00668630 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AGoToPointDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp:52](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1397"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGoToPointAndStandStillAndAchieveHeadingDestructorBody(CTaskComplexGoToPointAndStandStillAndAchieveHeading* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGoToPointAndStandStillAndAchieveHeading::Destructor 0x00668D40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskComplexGoToPointAndStandStillAndAchieveHeading() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1398"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToPointAimingDestructorCall(CTaskComplexGoToPointShooting* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGoToPointAiming::Destructor 0x00668870 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GoToPointShootingDestructorBody(CTaskComplexGoToPointShooting* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexGoToPointShooting::Destructor 0x0066DFA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147AAimingDestructorCall() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1399"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexHitByGunResponseDestructorBody(CTaskComplexHitByGunResponse* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexHitByGunResponse::Destructor 0x00631E30 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexHitByGunResponse() | [source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1400"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexHitResponseDestructorBody(CTaskComplexHitResponse* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexHitResponse::Destructor 0x00631DA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexHitResponse() | [source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

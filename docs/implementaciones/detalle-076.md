# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1501"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFireOnFoot.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExtinguishFireOnFootDestructorBody(CTaskComplexExtinguishFireOnFoot* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFireOnFoot.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFireOnFoot.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexExtinguishFireOnFoot::DestructorBody 0x006598B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1502"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FacialDestructorBody(CTaskComplexFacial* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexFacial::Destructor 0x00690D90 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1503"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFallAndGetUpDestructorBody(CTaskComplexFallAndGetUp* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Revalidada en migración: CTaskComplexFallAndGetUp::DestructorBody 0x00678790 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall CTaskComplexFallAndGetUpScalarDeletingDestructor(CTaskComplexFallAndGetUp* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexFallAndGetUp::ScalarDeletingDestructor 0x0067CB50 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote154ABody_CTaskComplexFallAndGetUp() | [source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1504"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FallToDeathDestructorBody(CTaskComplexFallToDeath* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexFallToDeath::Destructor 0x006790B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1505"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFleeAnyMeansDestructorBody(CTaskComplexFleeAnyMeans* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFleeAnyMeans::Destructor 0x0065CD10 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexFleeAnyMeans() | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1506"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFleeEntityDestructorBody(CTaskComplexFleeEntity* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFleeEntity::Destructor 0x0065B9D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexFleeEntity() | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1507"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFleePointDestructorBody(CTaskComplexFleePoint* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp#L33) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFleePoint::Destructor 0x0065B410 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexFleePoint() | [source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1508"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFleeShootingDestructorBody(CTaskComplexFleeShooting* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFleeShooting::Destructor 0x0065C9A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexFleeShooting() | [source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1509"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFollowNodeRouteDestructorBody(CTaskComplexFollowNodeRoute* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L35) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexFollowNodeRoute::Destructor 0x0066EB70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexFollowNodeRoute() | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:49](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FollowNodeRouteNodePos(CNodeAddress addr) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:94](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ThePaths: source/game_sa/PathFind.h:659 (0x0096F050) |
| FollowNodeRouteNodeLoaded(CNodeAddress addr) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp:99](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ThePaths: source/game_sa/PathFind.h:659 (0x0096F050) |

<a id="grupo-1510"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & FieldAt(void* object, uint32 offset) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp:74](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FollowPatrolRouteGetTaskType(CTaskComplexFollowPatrolRoute* /*self*/, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp:80](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp#L80) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexFollowPatrolRoute::GetTaskType 0x00670A60 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall FollowPatrolRouteControlSubTask(CTaskComplex* self, void* /*edx*/, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp:134](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexFollowPatrolRoute::ControlSubTask 0x0066BB00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1511"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FootstepsSquaredMagnitude(const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:149](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L149) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::SquaredMagnitudeExe 0x00406DA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDistSqZXY(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:153](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L153) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDistSqXZY(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:157](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDot(const CVector& w, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:161](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L161) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootstepsDist2D(const CVector& d) | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp:165](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp#L165) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1512"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote145ABody_CTaskComplexFollowPointRoute() | [source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp:57](../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1513"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GangFollowerCalculateOffsetPosition(CTaskComplexGangFollower* self, void* /*edx*/, CVector* out) | [source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp:95](../../source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_offsets: source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.h:28 (0x00C196E8) |

<a id="grupo-1514"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGangJoinRespondDestructorBody(CTaskComplexGangJoinRespond* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGangJoinRespond::Destructor 0x00661720 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexGangJoinRespond() | [source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1515"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGangLeaderDestructorBody(CTaskComplexGangLeader* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp:54](../../source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp#L54) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGangLeader::Destructor 0x0065DF30 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote160ABody_CTaskComplexGangLeader() | [source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp:68](../../source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1516"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetUpAndStandStillDestructorBody(CTaskComplexGetUpAndStandStill* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexGetUpAndStandStill::Destructor 0x00678160 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1517"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| X87Round24(double value) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp:97](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1518"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGoToAttractorDestructorBody(CTaskComplexGoToAttractor* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp#L32) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGoToAttractor::Destructor 0x0066B6A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexGoToAttractor() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp:46](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1519"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToCarDoorIsVehicleInRange(CTaskComplexGoToCarDoorAndStandStill* task, uint32 entryEdx, const CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexGoToCarDoorAndStandStill::IsVehicleInRange 0x006458A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexGoToCarDoorAndStandStillDestructorBody(CTaskComplexGoToCarDoorAndStandStill* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp#L43) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGoToCarDoorAndStandStill::Destructor 0x0064A580 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote159ABody_CTaskComplexGoToCarDoorAndStandStill() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp:57](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1520"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToPointAndStandStillDestructorBody(CTaskComplexGoToPointAndStandStill* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexGoToPointAndStandStill::Destructor 0x00668630 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GoToPointAndStandStillExeDestructorBody(CTaskComplexGoToPointAndStandStill* self) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp:62](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AGoToPointDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp:66](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

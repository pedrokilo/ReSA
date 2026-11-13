<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 897 grupos / 10369 funciones reconstruidas; 10369 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1421"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToCarDoorIsVehicleInRange(CTaskComplexGoToCarDoorAndStandStill* task, uint32 entryEdx, const CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexGoToCarDoorAndStandStillDestructorBody(CTaskComplexGoToCarDoorAndStandStill* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp#L42) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGoToCarDoorAndStandStill::Destructor 0x0064A580 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote159ABody_CTaskComplexGoToCarDoorAndStandStill() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp:56](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1422"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToPointAndStandStillDestructorBody(CTaskComplexGoToPointAndStandStill* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexGoToPointAndStandStill::Destructor 0x00668630 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AGoToPointDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp:52](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1423"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGoToPointAndStandStillAndAchieveHeadingDestructorBody(CTaskComplexGoToPointAndStandStillAndAchieveHeading* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGoToPointAndStandStillAndAchieveHeading::Destructor 0x00668D40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskComplexGoToPointAndStandStillAndAchieveHeading() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1424"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGoToPointAnyMeansDestructorBody(CTaskComplexGoToPointAnyMeans* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp#L33) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGoToPointAnyMeans::Destructor 0x0066B830 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote160ABody_CTaskComplexGoToPointAnyMeans() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1425"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToPointAimingDestructorCall(CTaskComplexGoToPointShooting* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexGoToPointAiming::Destructor 0x00668870 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GoToPointShootingDestructorBody(CTaskComplexGoToPointShooting* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexGoToPointShooting::Destructor 0x0066DFA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147AAimingDestructorCall() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1426"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexHitByGunResponseDestructorBody(CTaskComplexHitByGunResponse* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexHitByGunResponse::Destructor 0x00631E30 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexHitByGunResponse() | [source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1427"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexHitResponseDestructorBody(CTaskComplexHitResponse* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexHitResponse::Destructor 0x00631DA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexHitResponse() | [source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1428"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InAirAndLandDestructorBody(CTaskComplexInAirAndLand* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexInAirAndLand::Destructor 0x00678CC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1429"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| JumpDestructorBody(CTaskComplexJump* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexJump::Destructor 0x0067A060 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1430"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| NoPedOrNoHp(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:55](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & CriminalField(void* object, size_t offset) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:421](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L421) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CriminalHateEvent(CPed* target, CPed* recipient, eTaskType taskType) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:424](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L424) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1431"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexKillPedFromBoatDestructorBody(CTaskComplexKillPedFromBoat* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexKillPedFromBoat::Destructor 0x00622830 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote164ABody_CTaskComplexKillPedFromBoat() | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1432"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KillPedOnFootDestructorBody(CTaskComplexKillPedOnFoot* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp:50](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexKillPedOnFoot::Destructor 0x00620EF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * KillNewTask(Args... args) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp:108](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp#L108) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GiveGangsterPistolIfPlayerStopped(CPed* ped, CPed* targetPlayer) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp:119](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1433"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KillMeleeTargetDistance(CPed* ped, const CVector& targetPos) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp:134](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KillMeleeTargetDistanceSq(CPed* ped, const CVector& targetPos) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp:139](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1434"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RegisterPedLeaving(CTaskComplexLeaveCar* task, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp:260](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp#L260) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CreateLineUpUtility(CTaskComplexLeaveCar* task) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp:277](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp#L277) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NewLeaveCarSubTask(Args... args) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp:527](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp#L527) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1435"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LeaveCarAndDieBaseDestructorCall(CTaskComplexLeaveCarAndDie* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote152ABody_LeaveCarBaseDestructor() | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1436"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MedicTreatInjuredPedDestructorBody(CTaskComplexMedicTreatInjuredPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp#L39) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote143AMedicTreatInjuredPedDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp:53](../../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1437"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexObserveTrafficLightsDestructorBody(CTaskComplexObserveTrafficLights* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexObserveTrafficLights::Destructor 0x006317C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexObserveTrafficLights() | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1438"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall ObserveTrafficLightsAndAchieveHeadingConstructorBody(CTaskComplexObserveTrafficLightsAndAchieveHeading* self, void* /*edx*/, uint32 durationInMs, uint32 headingBits) | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexObserveTrafficLightsAndAchieveHeading::Constructor 0x00631910 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexObserveTrafficLightsAndAchieveHeading() | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp:67](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1439"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PartnerByteOf(const bool& flag) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:84](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReleasePartnerResources(CTaskComplexPartner& task) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:88](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PartnerByte(const bool& flag) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:125](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PartnerStateOfPartner(const CTaskComplexPartner& task) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:130](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L130) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PartnerTaskOfPartner(const CTaskComplexPartner& task) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:135](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L135) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PartnerForwardOf(const CPed& ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:140](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L140) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPlaceable::GetForwardVector 0x0041CCB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PartnerNormalise2D(float& x, float& y) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:149](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L149) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PartnerNewTask(Args... args) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:162](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L162) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PartnerStandStill() | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:170](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L170) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PartnerCounter70(CTaskComplexPartner& task) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:175](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L175) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1440"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexPartnerChatDestructorBodyExe(CTaskComplexPartnerChat* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

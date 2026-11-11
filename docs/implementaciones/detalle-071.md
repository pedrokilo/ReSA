<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 884 grupos / 9985 funciones reconstruidas; 9985 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1401"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| NoPedOrNoHp(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:55](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & CriminalField(void* object, size_t offset) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:421](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L421) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CriminalHateEvent(CPed* target, CPed* recipient, eTaskType taskType) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:424](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L424) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1402"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KillPedOnFootDestructorBody(CTaskComplexKillPedOnFoot* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp:50](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexKillPedOnFoot::Destructor 0x00620EF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * KillNewTask(Args... args) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp:108](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp#L108) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GiveGangsterPistolIfPlayerStopped(CPed* ped, CPed* targetPlayer) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp:119](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1403"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KillMeleeTargetDistance(CPed* ped, const CVector& targetPos) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp:134](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KillMeleeTargetDistanceSq(CPed* ped, const CVector& targetPos) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp:139](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1404"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RegisterPedLeaving(CTaskComplexLeaveCar* task, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp:260](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp#L260) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CreateLineUpUtility(CTaskComplexLeaveCar* task) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp:277](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp#L277) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NewLeaveCarSubTask(Args... args) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp:527](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp#L527) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1405"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LeaveCarAndDieBaseDestructorCall(CTaskComplexLeaveCarAndDie* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote152ABody_LeaveCarBaseDestructor() | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1406"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MedicTreatInjuredPedDestructorBody(CTaskComplexMedicTreatInjuredPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp#L39) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote143AMedicTreatInjuredPedDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp:53](../../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1407"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexObserveTrafficLightsDestructorBody(CTaskComplexObserveTrafficLights* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexObserveTrafficLights::Destructor 0x006317C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexObserveTrafficLights() | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1408"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall ObserveTrafficLightsAndAchieveHeadingConstructorBody(CTaskComplexObserveTrafficLightsAndAchieveHeading* self, void* /*edx*/, uint32 durationInMs, uint32 headingBits) | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexObserveTrafficLightsAndAchieveHeading::Constructor 0x00631910 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexObserveTrafficLightsAndAchieveHeadingDestructorBody(CTaskComplexObserveTrafficLightsAndAchieveHeading* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp#L47) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexObserveTrafficLightsAndAchieveHeading() | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp:61](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1409"></a>

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

<a id="grupo-1410"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexPartnerChatDestructorBodyExe(CTaskComplexPartnerChat* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1411"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PartnerDealDestructorBody(CTaskComplexPartnerDeal* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexPartnerDeal::Destructor 0x00684200 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1412"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PartnerShoveDestructorBody(CTaskComplexPartnerShove* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexPartnerShove::Destructor 0x00684750 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1413"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PassObjectDestructorBody(CTaskComplexPassObject* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexPassObject::Destructor 0x00661230 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote156ABody_CTaskComplexPassObject() | [source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1414"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexProstituteSolicitDestructorBody(CTaskComplexProstituteSolicit* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp#L34) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexProstituteSolicit::Destructor 0x00661AF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexProstituteSolicit() | [source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp:48](../../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1415"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSignalAtPedDestructorBody(CTaskComplexSignalAtPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSignalAtPed::Destructor 0x00660AB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote154ABody_CTaskComplexSignalAtPed() | [source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1416"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSmartFleeEntityDestructorBody(CTaskComplexSmartFleeEntity* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSmartFleeEntity::Destructor 0x0065C4D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexSmartFleeEntity() | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1417"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSmartFleePointDestructorBody(CTaskComplexSmartFleePoint* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSmartFleePoint::Destructor 0x0065BDB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexSmartFleePoint() | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1418"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexStareAtPedDestructorBody(CTaskComplexStareAtPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexStareAtPed::Destructor 0x00660310 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote153ABody_CTaskComplexStareAtPed() | [source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1419"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall StuckInAirConstructorBody(CTaskComplexStuckInAir* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:189](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L189) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexStuckInAir::Constructor 0x0067BA40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ComplexStuckInAirDestructorBody(CTaskComplexStuckInAir* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:213](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L213) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote128AComplexStuckInAirDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:219](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L219) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1420"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SunbatheWeatherAllows() | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:49](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00C8131C |
| SunbatheShouldLoad() | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:59](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo PlayerInFocus: source/game_sa/World.h:56 (0x00B7CD74); Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| CTaskComplexSunbatheDestructorBody(CTaskComplexSunbathe* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:72](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L72) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSunbathe::Destructor 0x00632050 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexSunbathe() | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:86](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

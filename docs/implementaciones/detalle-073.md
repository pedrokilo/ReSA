<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/11/2026 (Ciudad de México):** 912 grupos / 10706 funciones reconstruidas; 10706 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1441"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KillMeleeTargetDistance(CPed* ped, const CVector& targetPos) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp:134](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KillMeleeTargetDistanceSq(CPed* ped, const CVector& targetPos) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp:139](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1442"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * NewLeaveCarSubTask(Args... args) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp:566](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp#L566) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1443"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LeaveCarAndDieBaseDestructorCall(CTaskComplexLeaveCarAndDie* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote152ABody_LeaveCarBaseDestructor() | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1444"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MedicTreatInjuredPedDestructorBody(CTaskComplexMedicTreatInjuredPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp#L39) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote143AMedicTreatInjuredPedDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp:53](../../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1445"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexObserveTrafficLightsDestructorBody(CTaskComplexObserveTrafficLights* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexObserveTrafficLights::Destructor 0x006317C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexObserveTrafficLights() | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1446"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall ObserveTrafficLightsAndAchieveHeadingConstructorBody(CTaskComplexObserveTrafficLightsAndAchieveHeading* self, void* /*edx*/, uint32 durationInMs, uint32 headingBits) | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexObserveTrafficLightsAndAchieveHeading::Constructor 0x00631910 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexObserveTrafficLightsAndAchieveHeading() | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp:67](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1447"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PartnerByteOf(const bool& flag) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:98](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReleasePartnerResources(CTaskComplexPartner& task) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:102](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L102) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PartnerByte(const bool& flag) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:139](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PartnerStateOfPartner(const CTaskComplexPartner& task) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:144](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PartnerTaskOfPartner(const CTaskComplexPartner& task) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:149](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L149) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PartnerForwardOf(const CPed& ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:154](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L154) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPlaceable::GetForwardVector 0x0041CCB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PartnerNormalise2D(float& x, float& y) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:163](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L163) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PartnerNewTask(Args... args) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:176](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L176) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PartnerStandStill() | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:184](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L184) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PartnerCounter70(CTaskComplexPartner& task) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:189](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L189) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1448"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexPartnerChatDestructorBodyExe(CTaskComplexPartnerChat* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1449"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PartnerDealDestructorBody(CTaskComplexPartnerDeal* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexPartnerDeal::Destructor 0x00684200 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1450"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PartnerShoveDestructorBody(CTaskComplexPartnerShove* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexPartnerShove::Destructor 0x00684750 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1451"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PassObjectDestructorBody(CTaskComplexPassObject* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexPassObject::Destructor 0x00661230 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote156ABody_CTaskComplexPassObject() | [source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1452"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexProstituteSolicitDestructorBody(CTaskComplexProstituteSolicit* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp#L34) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexProstituteSolicit::Destructor 0x00661AF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexProstituteSolicit() | [source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp:48](../../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1453"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & FieldAt(void* object, uint32 offset) | [source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall ReactToAttackControlSubTask(CTaskComplexReactToAttack* self, void* /*edx*/, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp:85](../../source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexReactToAttack::ControlSubTask 0x00620E00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1454"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSeekCoverUntilTargetDeadDestructorBody(CTaskComplexSeekCoverUntilTargetDead* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSeekCoverUntilTargetDead::Destructor 0x00622C20 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote164ABody_CTaskComplexSeekCoverUntilTargetDead() | [source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1455"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSignalAtPedDestructorBody(CTaskComplexSignalAtPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSignalAtPed::Destructor 0x00660AB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote154ABody_CTaskComplexSignalAtPed() | [source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1456"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSmartFleeEntityDestructorBody(CTaskComplexSmartFleeEntity* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSmartFleeEntity::Destructor 0x0065C4D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexSmartFleeEntity() | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1457"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSmartFleePointDestructorBody(CTaskComplexSmartFleePoint* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSmartFleePoint::Destructor 0x0065BDB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexSmartFleePoint() | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1458"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexStareAtPedDestructorBody(CTaskComplexStareAtPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexStareAtPed::Destructor 0x00660310 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote153ABody_CTaskComplexStareAtPed() | [source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1459"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall StuckInAirConstructorBody(CTaskComplexStuckInAir* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:189](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L189) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexStuckInAir::Constructor 0x0067BA40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ComplexStuckInAirDestructorBody(CTaskComplexStuckInAir* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:213](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L213) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote128AComplexStuckInAirDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:219](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L219) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1460"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SunbatheWeatherAllows() | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:66](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00C8131C |
| CTaskComplexSunbatheDestructorBody(CTaskComplexSunbathe* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:81](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L81) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSunbathe::Destructor 0x00632050 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexSunbathe() | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:95](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

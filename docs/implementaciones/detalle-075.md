<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11707 con evidencia histórica y 734 pendientes de validación en 32 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1481"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| JumpDestructorBody(CTaskComplexJump* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexJump::Destructor 0x0067A060 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1482"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall CTaskComplexKillAllThreatsCloneExe(const CTaskComplexKillAllThreats* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1483"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| NoPedOrNoHp(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:55](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & CriminalField(void* object, size_t offset) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:421](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L421) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CriminalHateEvent(CPed* target, CPed* recipient, eTaskType taskType) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:424](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L424) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1484"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexKillPedFromBoatDestructorBody(CTaskComplexKillPedFromBoat* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexKillPedFromBoat::Destructor 0x00622830 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote164ABody_CTaskComplexKillPedFromBoat() | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1485"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KillPedOnFootDestructorBody(CTaskComplexKillPedOnFoot* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp:50](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexKillPedOnFoot::Destructor 0x00620EF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * KillNewTask(Args... args) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp:136](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp#L136) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GiveGangsterPistolIfPlayerStopped(CPed* ped, CPed* targetPlayer) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp:147](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp#L147) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1486"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KillMeleeTargetDistance(CPed* ped, const CVector& targetPos) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp:146](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp#L146) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KillMeleeTargetDistanceSq(CPed* ped, const CVector& targetPos) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp:151](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp#L151) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1487"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * NewLeaveCarSubTask(Args... args) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp:566](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp#L566) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1488"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LeaveCarAndDieBaseDestructorCall(CTaskComplexLeaveCarAndDie* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote152ABody_LeaveCarBaseDestructor() | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1489"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MedicTreatInjuredPedDestructorBody(CTaskComplexMedicTreatInjuredPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| Lote143AMedicTreatInjuredPedDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp:67](../../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1490"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexObserveTrafficLightsDestructorBody(CTaskComplexObserveTrafficLights* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexObserveTrafficLights::Destructor 0x006317C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexObserveTrafficLights() | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1491"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall ObserveTrafficLightsAndAchieveHeadingConstructorBody(CTaskComplexObserveTrafficLightsAndAchieveHeading* self, void* /*edx*/, uint32 durationInMs, uint32 headingBits) | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexObserveTrafficLightsAndAchieveHeading::Constructor 0x00631910 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexObserveTrafficLightsAndAchieveHeading() | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp:67](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1492"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PartnerByteOf(const bool& flag) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:98](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReleasePartnerResources(CTaskComplexPartner& task) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:102](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L102) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PartnerByte(const bool& flag) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:139](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PartnerStateOfPartner(const CTaskComplexPartner& task) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:144](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PartnerTaskOfPartner(const CTaskComplexPartner& task) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:149](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L149) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PartnerForwardOf(const CPed& ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:154](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L154) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPlaceable::GetForwardVector 0x0041CCB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PartnerNormalise2D(float& x, float& y) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:163](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L163) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector2D::Normalise 0x0044E480 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PartnerNewTask(Args... args) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:176](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L176) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PartnerStandStill() | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:184](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L184) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PartnerCounter70(CTaskComplexPartner& task) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp:189](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp#L189) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1493"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexPartnerChatDestructorBodyExe(CTaskComplexPartnerChat* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1494"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PartnerDealDestructorBody(CTaskComplexPartnerDeal* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexPartnerDeal::Destructor 0x00684200 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1495"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PartnerShoveDestructorBody(CTaskComplexPartnerShove* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexPartnerShove::Destructor 0x00684750 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1496"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PassObjectDestructorBody(CTaskComplexPassObject* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexPassObject::Destructor 0x00661230 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote156ABody_CTaskComplexPassObject() | [source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1497"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexProstituteSolicitDestructorBody(CTaskComplexProstituteSolicit* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp#L34) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexProstituteSolicit::Destructor 0x00661AF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexProstituteSolicit() | [source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp:48](../../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1498"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & FieldAt(void* object, uint32 offset) | [source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp:49](../../source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall ReactToAttackControlSubTask(CTaskComplexReactToAttack* self, void* /*edx*/, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp:111](../../source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexReactToAttack::ControlSubTask 0x00620E00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReactToAttackGetTaskType(CTaskComplexReactToAttack* /*self*/, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp:161](../../source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp#L161) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexReactToAttack::GetTaskType 0x00620B80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1499"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSeekCoverUntilTargetDeadDestructorBody(CTaskComplexSeekCoverUntilTargetDead* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSeekCoverUntilTargetDead::Destructor 0x00622C20 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote164ABody_CTaskComplexSeekCoverUntilTargetDead() | [source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1500"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSignalAtPedDestructorBody(CTaskComplexSignalAtPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSignalAtPed::Destructor 0x00660AB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote154ABody_CTaskComplexSignalAtPed() | [source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

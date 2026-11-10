<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 09/11/2026 (Ciudad de México):** 875 grupos / 9750 funciones reconstruidas; 9750 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1381"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KillMeleeTargetDistance(CPed* ped, const CVector& targetPos) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp:134](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KillMeleeTargetDistanceSq(CPed* ped, const CVector& targetPos) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp:139](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1382"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RegisterPedLeaving(CTaskComplexLeaveCar* task, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp:260](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp#L260) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CreateLineUpUtility(CTaskComplexLeaveCar* task) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp:277](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp#L277) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NewLeaveCarSubTask(Args... args) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp:527](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp#L527) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1383"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MedicTreatInjuredPedDestructorBody(CTaskComplexMedicTreatInjuredPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp#L39) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote143AMedicTreatInjuredPedDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp:53](../../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1384"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall ObserveTrafficLightsAndAchieveHeadingConstructorBody(CTaskComplexObserveTrafficLightsAndAchieveHeading* self, void* /*edx*/, uint32 durationInMs, uint32 headingBits) | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexObserveTrafficLightsAndAchieveHeading::Constructor 0x00631910 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1385"></a>

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

<a id="grupo-1386"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PartnerDealDestructorBody(CTaskComplexPartnerDeal* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexPartnerDeal::Destructor 0x00684200 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1387"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexProstituteSolicitDestructorBody(CTaskComplexProstituteSolicit* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp#L34) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexProstituteSolicit::Destructor 0x00661AF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexProstituteSolicit() | [source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp:48](../../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1388"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall StuckInAirConstructorBody(CTaskComplexStuckInAir* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:189](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L189) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexStuckInAir::Constructor 0x0067BA40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ComplexStuckInAirDestructorBody(CTaskComplexStuckInAir* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:213](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L213) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote128AComplexStuckInAirDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:219](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L219) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1389"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SunbatheWeatherAllows() | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00C8131C |
| SunbatheShouldLoad() | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:53](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo PlayerInFocus: source/game_sa/World.h:56 (0x00B7CD74); Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| CTaskComplexSunbatheDestructorBody(CTaskComplexSunbathe* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:66](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L66) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSunbathe::Destructor 0x00632050 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexSunbathe() | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:80](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L80) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1390"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TreatAccidentDestructorBody(CTaskComplexTreatAccident* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexTreatAccident::Destructor 0x00658AE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1391"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall UseEffectRunningConstructorBody(CTaskComplexUseEffectRunning* self, void* /*edx*/, C2dEffectPedAttractor* effect, CEntity* entity) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseEffectRunning::Constructor 0x00636780 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UseEffectRunningDestructorBody(CTaskComplexUseEffectRunning* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseEffectRunning::Destructor 0x00636850 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1392"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall UseEffectSprintingConstructorBody(CTaskComplexUseEffectSprinting* self, void* /*edx*/, C2dEffectPedAttractor* effect, CEntity* entity) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseEffectSprinting::Constructor 0x00636860 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UseEffectSprintingDestructorBody(CTaskComplexUseEffectSprinting* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseEffectSprinting::Destructor 0x00636930 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1393"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall UseGogglesConstructorBody(CTaskComplexUseGoggles* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp:81](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseGoggles::Constructor 0x00634EF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UseGogglesDestructorBody(CTaskComplexUseGoggles* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp:89](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp#L89) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseGoggles::Destructor 0x00634F20 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TaskComplexUseGogglesTestCode() | [source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp:109](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1394"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexUseScriptedBrainDestructorBody(CTaskComplexUseScriptedBrain* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexUseScriptedBrain::Destructor 0x00635900 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskComplexUseScriptedBrain() | [source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1395"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWaitForBusDestructorBody(CTaskComplexWaitForBus* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexWaitForBus::Destructor 0x00632930 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexWaitForBus() | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1396"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWaitForDryWeatherDestructorBody(CTaskComplexWaitForDryWeather* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexWaitForDryWeather::Destructor 0x00632830 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexWaitForDryWeather() | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1397"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaitForPedUnwind(EXCEPTION_RECORD* record, void* frame, CONTEXT*, void*) | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1398"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * NewEmptyPointRoute() | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:56](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddPointUnlessFull(CPointRoute& route, const CVector& pt) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:65](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopyPointRoute(CPointRoute& dst, const CPointRoute& src) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:73](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L73) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MulMatrixPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:78](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1399"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWalkRoundFireDestructorBody(CTaskComplexWalkRoundFire* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexWalkRoundFire::Destructor 0x00655780 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskComplexWalkRoundFire() | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1400"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWanderCriminalDestructorBody(CTaskComplexWanderCriminal* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexWanderCriminal::Destructor 0x0048E720 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexWanderCriminal() | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

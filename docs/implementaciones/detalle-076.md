<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1501"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSmartFleeEntityDestructorBody(CTaskComplexSmartFleeEntity* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSmartFleeEntity::Destructor 0x0065C4D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexSmartFleeEntity() | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1502"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSmartFleePointDestructorBody(CTaskComplexSmartFleePoint* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSmartFleePoint::Destructor 0x0065BDB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexSmartFleePoint() | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1503"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexStareAtPedDestructorBody(CTaskComplexStareAtPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexStareAtPed::Destructor 0x00660310 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote153ABody_CTaskComplexStareAtPed() | [source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1504"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall StuckInAirConstructorBody(CTaskComplexStuckInAir* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:189](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L189) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexStuckInAir::Constructor 0x0067BA40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ComplexStuckInAirDestructorBody(CTaskComplexStuckInAir* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:215](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L215) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexStuckInAir::DestructorBody 0x0067BA70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote128AComplexStuckInAirDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:221](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L221) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1505"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SunbatheWeatherAllows() | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:66](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00C8131C |
| CTaskComplexSunbatheDestructorBody(CTaskComplexSunbathe* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:81](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L81) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSunbathe::Destructor 0x00632050 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexSunbathe() | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:95](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1506"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TrackEntityDestructorBody(CTaskComplexTrackEntity* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexTrackEntity::Destructor 0x0065F460 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote158ABody_CTaskComplexTrackEntity() | [source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1507"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TreatAccidentDestructorBody(CTaskComplexTreatAccident* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexTreatAccident::Destructor 0x00658AE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1508"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexUseClosestFreeScriptedAttractorDestructorBody(CTaskComplexUseClosestFreeScriptedAttractor* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexUseClosestFreeScriptedAttractor::Destructor 0x00634720 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexUseClosestFreeScriptedAttractor() | [source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1509"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall UseEffectRunningConstructorBody(CTaskComplexUseEffectRunning* self, void* /*edx*/, C2dEffectPedAttractor* effect, CEntity* entity) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseEffectRunning::Constructor 0x00636780 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UseEffectRunningDestructorBody(CTaskComplexUseEffectRunning* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseEffectRunning::Destructor 0x00636850 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1510"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall UseEffectSprintingConstructorBody(CTaskComplexUseEffectSprinting* self, void* /*edx*/, C2dEffectPedAttractor* effect, CEntity* entity) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseEffectSprinting::Constructor 0x00636860 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UseEffectSprintingDestructorBody(CTaskComplexUseEffectSprinting* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseEffectSprinting::Destructor 0x00636930 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1511"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall UseGogglesConstructorBody(CTaskComplexUseGoggles* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp:81](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseGoggles::Constructor 0x00634EF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UseGogglesDestructorBody(CTaskComplexUseGoggles* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp:89](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp#L89) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseGoggles::Destructor 0x00634F20 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TaskComplexUseGogglesTestCode() | [source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp:121](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1512"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClearPhoneFlags(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp:46](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall UseMobilePhoneConstructor(CTaskComplexUseMobilePhone* self, void* /*edx*/, int32 duration) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp:54](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseMobilePhone::Constructor 0x006348A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UseMobilePhoneGetTaskType(void* /*self*/, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp:68](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseMobilePhone::GetTaskType 0x006348D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UseMobilePhoneDestructorBody(CTaskComplexUseMobilePhone* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp:74](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseMobilePhone::DestructorBody 0x006348E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NewPhoneAnimTaskImpl(int32 animId, int32 taskType, uintptr_t exeNameAddress) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp:127](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp#L127) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall UseMobilePhoneScalarDeletingDestructor(CTaskComplexUseMobilePhone* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp:207](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp#L207) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplex::Destructor 0x0061A3D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1513"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexUseScriptedBrainDestructorBody(CTaskComplexUseScriptedBrain* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexUseScriptedBrain::Destructor 0x00635900 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskComplexUseScriptedBrain() | [source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1514"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexUseWaterCannonDestructorBody(CTaskComplexUseWaterCannon* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Revalidada en migración: CTaskComplexUseWaterCannon::DestructorBody 0x006597B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall CTaskComplexUseWaterCannonScalarDeletingDestructor(CTaskComplexUseWaterCannon* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote153ABody_CTaskComplexUseWaterCannon() | [source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1515"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWaitForBusDestructorBody(CTaskComplexWaitForBus* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexWaitForBus::Destructor 0x00632930 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexWaitForBus() | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1516"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWaitForDryWeatherDestructorBody(CTaskComplexWaitForDryWeather* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexWaitForDryWeather::Destructor 0x00632830 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexWaitForDryWeather() | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1517"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaitForPedGetTaskType(CTaskComplexWaitForPed* /*self*/, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexWaitForPed::GetTaskType 0x006833C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaitForPedMakeAbortable(CTaskComplexWaitForPed* /*self*/, void* /*edx*/, CPed* /*ped*/, eAbortPriority /*priority*/, const CEvent* /*event*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp:50](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexWaitForPed::MakeAbortable 0x00683430 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CTaskComplexWaitForPed() | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp:66](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaitForPedUnwind(EXCEPTION_RECORD* record, void* frame, CONTEXT*, void*) | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp:93](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1518"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WalkAlongsidePedDestructorBody(CTaskComplexWalkAlongsidePed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp#L32) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexWalkAlongsidePed::Destructor 0x006832C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CTaskComplexWalkAlongsidePed() | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp:46](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1519"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * NewEmptyPointRoute() | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:57](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddPointUnlessFull(CPointRoute& route, const CVector& pt) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:66](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopyPointRoute(CPointRoute& dst, const CPointRoute& src) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:74](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MulMatrixPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:79](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1520"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWalkRoundFireDestructorBody(CTaskComplexWalkRoundFire* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexWalkRoundFire::Destructor 0x00655780 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskComplexWalkRoundFire() | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

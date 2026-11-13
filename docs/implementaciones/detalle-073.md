<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 13/11/2026 (Ciudad de México):** 901 grupos / 10540 funciones reconstruidas; 10540 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1441"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PartnerDealDestructorBody(CTaskComplexPartnerDeal* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexPartnerDeal::Destructor 0x00684200 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1442"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PartnerShoveDestructorBody(CTaskComplexPartnerShove* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexPartnerShove::Destructor 0x00684750 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1443"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PassObjectDestructorBody(CTaskComplexPassObject* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexPassObject::Destructor 0x00661230 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote156ABody_CTaskComplexPassObject() | [source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1444"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexProstituteSolicitDestructorBody(CTaskComplexProstituteSolicit* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp#L34) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexProstituteSolicit::Destructor 0x00661AF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexProstituteSolicit() | [source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp:48](../../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1445"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSeekCoverUntilTargetDeadDestructorBody(CTaskComplexSeekCoverUntilTargetDead* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSeekCoverUntilTargetDead::Destructor 0x00622C20 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote164ABody_CTaskComplexSeekCoverUntilTargetDead() | [source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1446"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSignalAtPedDestructorBody(CTaskComplexSignalAtPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSignalAtPed::Destructor 0x00660AB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote154ABody_CTaskComplexSignalAtPed() | [source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1447"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSmartFleeEntityDestructorBody(CTaskComplexSmartFleeEntity* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSmartFleeEntity::Destructor 0x0065C4D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexSmartFleeEntity() | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1448"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSmartFleePointDestructorBody(CTaskComplexSmartFleePoint* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSmartFleePoint::Destructor 0x0065BDB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexSmartFleePoint() | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1449"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexStareAtPedDestructorBody(CTaskComplexStareAtPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexStareAtPed::Destructor 0x00660310 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote153ABody_CTaskComplexStareAtPed() | [source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1450"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall StuckInAirConstructorBody(CTaskComplexStuckInAir* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:189](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L189) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexStuckInAir::Constructor 0x0067BA40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ComplexStuckInAirDestructorBody(CTaskComplexStuckInAir* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:213](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L213) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote128AComplexStuckInAirDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:219](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L219) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1451"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SunbatheWeatherAllows() | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:49](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00C8131C |
| SunbatheShouldLoad() | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:59](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo PlayerInFocus: source/game_sa/World.h:56 (0x00B7CD74); Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| CTaskComplexSunbatheDestructorBody(CTaskComplexSunbathe* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:72](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L72) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSunbathe::Destructor 0x00632050 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexSunbathe() | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:86](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1452"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TrackEntityDestructorBody(CTaskComplexTrackEntity* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexTrackEntity::Destructor 0x0065F460 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote158ABody_CTaskComplexTrackEntity() | [source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1453"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TreatAccidentDestructorBody(CTaskComplexTreatAccident* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexTreatAccident::Destructor 0x00658AE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1454"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexUseClosestFreeScriptedAttractorDestructorBody(CTaskComplexUseClosestFreeScriptedAttractor* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexUseClosestFreeScriptedAttractor::Destructor 0x00634720 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexUseClosestFreeScriptedAttractor() | [source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1455"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall UseEffectRunningConstructorBody(CTaskComplexUseEffectRunning* self, void* /*edx*/, C2dEffectPedAttractor* effect, CEntity* entity) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseEffectRunning::Constructor 0x00636780 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UseEffectRunningDestructorBody(CTaskComplexUseEffectRunning* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseEffectRunning::Destructor 0x00636850 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1456"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall UseEffectSprintingConstructorBody(CTaskComplexUseEffectSprinting* self, void* /*edx*/, C2dEffectPedAttractor* effect, CEntity* entity) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseEffectSprinting::Constructor 0x00636860 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UseEffectSprintingDestructorBody(CTaskComplexUseEffectSprinting* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseEffectSprinting::Destructor 0x00636930 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1457"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall UseGogglesConstructorBody(CTaskComplexUseGoggles* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp:81](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseGoggles::Constructor 0x00634EF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UseGogglesDestructorBody(CTaskComplexUseGoggles* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp:89](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp#L89) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseGoggles::Destructor 0x00634F20 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TaskComplexUseGogglesTestCode() | [source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp:109](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1458"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexUseScriptedBrainDestructorBody(CTaskComplexUseScriptedBrain* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexUseScriptedBrain::Destructor 0x00635900 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskComplexUseScriptedBrain() | [source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1459"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexUseWaterCannonDestructorBody(CTaskComplexUseWaterCannon* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall CTaskComplexUseWaterCannonScalarDeletingDestructor(CTaskComplexUseWaterCannon* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote153ABody_CTaskComplexUseWaterCannon() | [source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1460"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWaitForBusDestructorBody(CTaskComplexWaitForBus* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexWaitForBus::Destructor 0x00632930 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexWaitForBus() | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/11/2026 (Ciudad de México):** 1004 grupos / 13311 funciones reconstruidas; 11801 con evidencia histórica y 1510 pendientes de validación en 39 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1201"></a>

## Globales y helpers: source/game_sa/Events/EventDeadPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeadPedDestructorBody(CEventDeadPed* self, void* /*edx*/) | [source/game_sa/Events/EventDeadPed.cpp:29](../../source/game_sa/Events/EventDeadPed.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventDeadPed::Destructor 0x004ADF70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1202"></a>

## Globales y helpers: source/game_sa/Events/EventDeath.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeathDestructorBody(CEventDeath* self, void* /*edx*/) | [source/game_sa/Events/EventDeath.cpp:32](../../source/game_sa/Events/EventDeath.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventDeath::Destructor 0x004ADE90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1203"></a>

## Globales y helpers: source/game_sa/Events/EventDontJoinPlayerGroup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DontJoinPlayerGroupDestructorBody(CEventDontJoinPlayerGroup* self, void* /*edx*/) | [source/game_sa/Events/EventDontJoinPlayerGroup.cpp:30](../../source/game_sa/Events/EventDontJoinPlayerGroup.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventDontJoinPlayerGroup::Destructor 0x00609140 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote155ABody_CEventDontJoinPlayerGroup() | [source/game_sa/Events/EventDontJoinPlayerGroup.cpp:50](../../source/game_sa/Events/EventDontJoinPlayerGroup.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1204"></a>

## Globales y helpers: source/game_sa/Events/EventDraggedOutCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DraggedOutCarDestructorBody(CEventDraggedOutCar* self, void* /*edx*/) | [source/game_sa/Events/EventDraggedOutCar.cpp:27](../../source/game_sa/Events/EventDraggedOutCar.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventDraggedOutCar::Destructor 0x004AD330 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1205"></a>

## Globales y helpers: source/game_sa/Events/EventEscalator.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EscalatorDestructorBody(CEventEscalator* self, void* /*edx*/) | [source/game_sa/Events/EventEscalator.cpp:30](../../source/game_sa/Events/EventEscalator.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventEscalator::DestructorBody 0x005FF860 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CEventEscalator() | [source/game_sa/Events/EventEscalator.cpp:46](../../source/game_sa/Events/EventEscalator.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CEventOperatorDelete() | [source/game_sa/Events/EventEscalator.cpp:47](../../source/game_sa/Events/EventEscalator.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1206"></a>

## Globales y helpers: source/game_sa/Events/EventFireNearby.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FireNearbyDestructorBody(CEventFireNearby* self, void* /*edx*/) | [source/game_sa/Events/EventFireNearby.cpp:25](../../source/game_sa/Events/EventFireNearby.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventFireNearby::Destructor 0x004B1F80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1207"></a>

## Globales y helpers: source/game_sa/Events/EventGlobalGroup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote143AEventGroupDestructorBody() | [source/game_sa/Events/EventGlobalGroup.cpp:44](../../source/game_sa/Events/EventGlobalGroup.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1208"></a>

## Globales y helpers: source/game_sa/Events/EventGotKnockedOverByCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GotKnockedOverByCarDestructorBody(CEventGotKnockedOverByCar* self, void* /*edx*/) | [source/game_sa/Events/EventGotKnockedOverByCar.cpp:27](../../source/game_sa/Events/EventGotKnockedOverByCar.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventGotKnockedOverByCar::Destructor 0x004B1C10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1209"></a>

## Globales y helpers: source/game_sa/Events/EventGunAimedAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GunAimedAtDestructorBody(CEventGunAimedAt* self, void* /*edx*/) | [source/game_sa/Events/EventGunAimedAt.cpp:29](../../source/game_sa/Events/EventGunAimedAt.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventGunAimedAt::Destructor 0x004B07B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1210"></a>

## Globales y helpers: source/game_sa/Events/EventGunShotWhizzedBy.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GunShotWhizzedByDestructorBody(CEventGunShotWhizzedBy* self, void* /*edx*/) | [source/game_sa/Events/EventGunShotWhizzedBy.cpp:29](../../source/game_sa/Events/EventGunShotWhizzedBy.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventGunShotWhizzedBy::Destructor 0x004B2070 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1211"></a>

## Globales y helpers: source/game_sa/Events/EventHealthReallyLow.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HealthReallyLowDestructorBody(CEventHealthReallyLow* self, void* /*edx*/) | [source/game_sa/Events/EventHealthReallyLow.cpp:30](../../source/game_sa/Events/EventHealthReallyLow.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventHealthReallyLow::DestructorBody 0x005FF520 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1212"></a>

## Globales y helpers: source/game_sa/Events/EventHighAngerAtPlayer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HighAngerAtPlayerDestructorBody(CEventHighAngerAtPlayer* self, void* /*edx*/) | [source/game_sa/Events/EventHighAngerAtPlayer.cpp:30](../../source/game_sa/Events/EventHighAngerAtPlayer.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventHighAngerAtPlayer::DestructorBody 0x005FF6A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1213"></a>

## Globales y helpers: source/game_sa/Events/EventHitByWaterCannon.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HitByWaterCannonDestructorBody(CEventHitByWaterCannon* self, void* /*edx*/) | [source/game_sa/Events/EventHitByWaterCannon.cpp:25](../../source/game_sa/Events/EventHitByWaterCannon.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventHitByWaterCannon::Destructor 0x004B1320 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1214"></a>

## Globales y helpers: source/game_sa/Events/EventInAir.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InAirDestructorBody(CEventInAir* self, void* /*edx*/) | [source/game_sa/Events/EventInAir.cpp:25](../../source/game_sa/Events/EventInAir.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventInAir::Destructor 0x004B0D00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1215"></a>

## Globales y helpers: source/game_sa/Events/EventInWater.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InWaterDestructorBody(CEventInWater* self, void* /*edx*/) | [source/game_sa/Events/EventInWater.cpp:25](../../source/game_sa/Events/EventInWater.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventInWater::Destructor 0x004B13C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1216"></a>

## Globales y helpers: source/game_sa/Events/EventKnockOffBike.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KnockOffBikeDestructorBody(CEventKnockOffBike* self, void* /*edx*/) | [source/game_sa/Events/EventKnockOffBike.cpp:31](../../source/game_sa/Events/EventKnockOffBike.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventKnockOffBike::Destructor 0x004AFE70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall KnockOffBikeAssignExe(CEventKnockOffBike* self, void* /*edx*/, const CEventKnockOffBike* other) | [source/game_sa/Events/EventKnockOffBike.cpp:40](../../source/game_sa/Events/EventKnockOffBike.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventKnockOffBike::operator= 0x004B4AA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KnockOffScale(const CVector& v, float k) | [source/game_sa/Events/EventKnockOffBike.cpp:267](../../source/game_sa/Events/EventKnockOffBike.cpp#L267) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KnockOffCombine(const CVector& up, float upK, const CVector& side, float sideK) | [source/game_sa/Events/EventKnockOffBike.cpp:274](../../source/game_sa/Events/EventKnockOffBike.cpp#L274) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1217"></a>

## Globales y helpers: source/game_sa/Events/EventLeaderEntryExit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LeaderEntryExitDestructorBody(CEventLeaderEntryExit* self, void* /*edx*/) | [source/game_sa/Events/EventLeaderEntryExit.cpp:29](../../source/game_sa/Events/EventLeaderEntryExit.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventLeaderEntryExit::DestructorBody 0x0043E2B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1218"></a>

## Globales y helpers: source/game_sa/Events/EventLowAngerAtPlayer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LowAngerAtPlayerDestructorBody(CEventLowAngerAtPlayer* self, void* /*edx*/) | [source/game_sa/Events/EventLowAngerAtPlayer.cpp:30](../../source/game_sa/Events/EventLowAngerAtPlayer.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventLowAngerAtPlayer::DestructorBody 0x005FF5E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1219"></a>

## Globales y helpers: source/game_sa/Events/EventObjectCollision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ObjectCollisionDestructorBody(CEventObjectCollision* self, void* /*edx*/) | [source/game_sa/Events/EventObjectCollision.cpp:26](../../source/game_sa/Events/EventObjectCollision.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventObjectCollision::Destructor 0x004ACDD0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1220"></a>

## Globales y helpers: source/game_sa/Events/EventOnFire.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| OnFireDestructorBody(CEventOnFire* self, void* /*edx*/) | [source/game_sa/Events/EventOnFire.cpp:30](../../source/game_sa/Events/EventOnFire.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventOnFire::DestructorBody 0x005FF760 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

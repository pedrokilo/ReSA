<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11768 con evidencia histórica y 673 pendientes de validación en 25 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1201"></a>

## Globales y helpers: source/game_sa/Events/EventDontJoinPlayerGroup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DontJoinPlayerGroupDestructorBody(CEventDontJoinPlayerGroup* self, void* /*edx*/) | [source/game_sa/Events/EventDontJoinPlayerGroup.cpp:30](../../source/game_sa/Events/EventDontJoinPlayerGroup.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventDontJoinPlayerGroup::Destructor 0x00609140 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote155ABody_CEventDontJoinPlayerGroup() | [source/game_sa/Events/EventDontJoinPlayerGroup.cpp:50](../../source/game_sa/Events/EventDontJoinPlayerGroup.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1202"></a>

## Globales y helpers: source/game_sa/Events/EventDraggedOutCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DraggedOutCarDestructorBody(CEventDraggedOutCar* self, void* /*edx*/) | [source/game_sa/Events/EventDraggedOutCar.cpp:26](../../source/game_sa/Events/EventDraggedOutCar.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1203"></a>

## Globales y helpers: source/game_sa/Events/EventEscalator.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EscalatorDestructorBody(CEventEscalator* self, void* /*edx*/) | [source/game_sa/Events/EventEscalator.cpp:30](../../source/game_sa/Events/EventEscalator.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventEscalator::DestructorBody 0x005FF860 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CEventEscalator() | [source/game_sa/Events/EventEscalator.cpp:46](../../source/game_sa/Events/EventEscalator.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CEventOperatorDelete() | [source/game_sa/Events/EventEscalator.cpp:47](../../source/game_sa/Events/EventEscalator.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1204"></a>

## Globales y helpers: source/game_sa/Events/EventGlobalGroup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote143AEventGroupDestructorBody() | [source/game_sa/Events/EventGlobalGroup.cpp:44](../../source/game_sa/Events/EventGlobalGroup.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1205"></a>

## Globales y helpers: source/game_sa/Events/EventGotKnockedOverByCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GotKnockedOverByCarDestructorBody(CEventGotKnockedOverByCar* self, void* /*edx*/) | [source/game_sa/Events/EventGotKnockedOverByCar.cpp:26](../../source/game_sa/Events/EventGotKnockedOverByCar.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1206"></a>

## Globales y helpers: source/game_sa/Events/EventGunAimedAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GunAimedAtDestructorBody(CEventGunAimedAt* self, void* /*edx*/) | [source/game_sa/Events/EventGunAimedAt.cpp:28](../../source/game_sa/Events/EventGunAimedAt.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1207"></a>

## Globales y helpers: source/game_sa/Events/EventGunShotWhizzedBy.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GunShotWhizzedByDestructorBody(CEventGunShotWhizzedBy* self, void* /*edx*/) | [source/game_sa/Events/EventGunShotWhizzedBy.cpp:29](../../source/game_sa/Events/EventGunShotWhizzedBy.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventGunShotWhizzedBy::Destructor 0x004B2070 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1208"></a>

## Globales y helpers: source/game_sa/Events/EventHealthReallyLow.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HealthReallyLowDestructorBody(CEventHealthReallyLow* self, void* /*edx*/) | [source/game_sa/Events/EventHealthReallyLow.cpp:30](../../source/game_sa/Events/EventHealthReallyLow.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventHealthReallyLow::DestructorBody 0x005FF520 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1209"></a>

## Globales y helpers: source/game_sa/Events/EventHighAngerAtPlayer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HighAngerAtPlayerDestructorBody(CEventHighAngerAtPlayer* self, void* /*edx*/) | [source/game_sa/Events/EventHighAngerAtPlayer.cpp:30](../../source/game_sa/Events/EventHighAngerAtPlayer.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventHighAngerAtPlayer::DestructorBody 0x005FF6A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1210"></a>

## Globales y helpers: source/game_sa/Events/EventKnockOffBike.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KnockOffBikeDestructorBody(CEventKnockOffBike* self, void* /*edx*/) | [source/game_sa/Events/EventKnockOffBike.cpp:31](../../source/game_sa/Events/EventKnockOffBike.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventKnockOffBike::Destructor 0x004AFE70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall KnockOffBikeAssignExe(CEventKnockOffBike* self, void* /*edx*/, const CEventKnockOffBike* other) | [source/game_sa/Events/EventKnockOffBike.cpp:40](../../source/game_sa/Events/EventKnockOffBike.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventKnockOffBike::operator= 0x004B4AA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KnockOffScale(const CVector& v, float k) | [source/game_sa/Events/EventKnockOffBike.cpp:265](../../source/game_sa/Events/EventKnockOffBike.cpp#L265) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KnockOffCombine(const CVector& up, float upK, const CVector& side, float sideK) | [source/game_sa/Events/EventKnockOffBike.cpp:272](../../source/game_sa/Events/EventKnockOffBike.cpp#L272) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1211"></a>

## Globales y helpers: source/game_sa/Events/EventLeaderEntryExit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LeaderEntryExitDestructorBody(CEventLeaderEntryExit* self, void* /*edx*/) | [source/game_sa/Events/EventLeaderEntryExit.cpp:29](../../source/game_sa/Events/EventLeaderEntryExit.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventLeaderEntryExit::DestructorBody 0x0043E2B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1212"></a>

## Globales y helpers: source/game_sa/Events/EventLowAngerAtPlayer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LowAngerAtPlayerDestructorBody(CEventLowAngerAtPlayer* self, void* /*edx*/) | [source/game_sa/Events/EventLowAngerAtPlayer.cpp:30](../../source/game_sa/Events/EventLowAngerAtPlayer.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventLowAngerAtPlayer::DestructorBody 0x005FF5E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1213"></a>

## Globales y helpers: source/game_sa/Events/EventObjectCollision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ObjectCollisionDestructorBody(CEventObjectCollision* self, void* /*edx*/) | [source/game_sa/Events/EventObjectCollision.cpp:26](../../source/game_sa/Events/EventObjectCollision.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventObjectCollision::Destructor 0x004ACDD0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1214"></a>

## Globales y helpers: source/game_sa/Events/EventOnFire.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| OnFireDestructorBody(CEventOnFire* self, void* /*edx*/) | [source/game_sa/Events/EventOnFire.cpp:30](../../source/game_sa/Events/EventOnFire.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventOnFire::DestructorBody 0x005FF760 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1215"></a>

## Globales y helpers: source/game_sa/Events/EventPassObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PassObjectDestructorBody(CEventPassObject* self, void* /*edx*/) | [source/game_sa/Events/EventPassObject.cpp:30](../../source/game_sa/Events/EventPassObject.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPassObject::Destructor 0x0065DCF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1216"></a>

## Globales y helpers: source/game_sa/Events/EventPedCollisionWithPlayer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedCollisionWithPlayerDestructorBody(CEventPedCollisionWithPlayer* self, void* /*edx*/) | [source/game_sa/Events/EventPedCollisionWithPlayer.cpp:30](../../source/game_sa/Events/EventPedCollisionWithPlayer.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPedCollisionWithPlayer::DestructorBody 0x005FED80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1217"></a>

## Globales y helpers: source/game_sa/Events/EventPedToChase.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedToChaseDestructorBody(CEventPedToChase* self, void* /*edx*/) | [source/game_sa/Events/EventPedToChase.cpp:26](../../source/game_sa/Events/EventPedToChase.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1218"></a>

## Globales y helpers: source/game_sa/Events/EventPedToFlee.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedToFleeDestructorBody(CEventPedToFlee* self, void* /*edx*/) | [source/game_sa/Events/EventPedToFlee.cpp:26](../../source/game_sa/Events/EventPedToFlee.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1219"></a>

## Globales y helpers: source/game_sa/Events/EventPlayerCollisionWithPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlayerCollisionWithPedDestructorBody(CEventPlayerCollisionWithPed* self, void* /*edx*/) | [source/game_sa/Events/EventPlayerCollisionWithPed.cpp:30](../../source/game_sa/Events/EventPlayerCollisionWithPed.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPlayerCollisionWithPed::DestructorBody 0x005FEE80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1220"></a>

## Globales y helpers: source/game_sa/Events/EventPlayerCommandToGroupGather.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlayerCommandToGroupGatherDestructorBody(CEventPlayerCommandToGroupGather* self, void* /*edx*/) | [source/game_sa/Events/EventPlayerCommandToGroupGather.cpp:30](../../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPlayerCommandToGroupGather::DestructorBody 0x00609300 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PlayerCommandToGroupGatherScalarDeletingDestructor(CEventPlayerCommandToGroupGather* self, void* /*edx*/, uint8 flags) | [source/game_sa/Events/EventPlayerCommandToGroupGather.cpp:37](../../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPlayerCommandToGroupGather::ScalarDeletingDestructor 0x006092E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

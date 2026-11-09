<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 09/11/2026 (Ciudad de México):** 871 grupos / 9586 funciones reconstruidas; 9586 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1141"></a>

## Globales y helpers: source/game_sa/Events/EventDamage.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComputeHitAnimDelayTimer(float delay) | [source/game_sa/Events/EventDamage.cpp:447](../../source/game_sa/Events/EventDamage.cpp#L447) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1142"></a>

## Globales y helpers: source/game_sa/Events/EventDanger.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DangerDestructorBody(CEventDanger* self, void* /*edx*/) | [source/game_sa/Events/EventDanger.cpp:26](../../source/game_sa/Events/EventDanger.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventDanger::Destructor 0x004B26A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1143"></a>

## Globales y helpers: source/game_sa/Events/EventDeadPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeadPedDestructorBody(CEventDeadPed* self, void* /*edx*/) | [source/game_sa/Events/EventDeadPed.cpp:28](../../source/game_sa/Events/EventDeadPed.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1144"></a>

## Globales y helpers: source/game_sa/Events/EventDontJoinPlayerGroup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DontJoinPlayerGroupDestructorBody(CEventDontJoinPlayerGroup* self, void* /*edx*/) | [source/game_sa/Events/EventDontJoinPlayerGroup.cpp:30](../../source/game_sa/Events/EventDontJoinPlayerGroup.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventDontJoinPlayerGroup::Destructor 0x00609140 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1145"></a>

## Globales y helpers: source/game_sa/Events/EventDraggedOutCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DraggedOutCarDestructorBody(CEventDraggedOutCar* self, void* /*edx*/) | [source/game_sa/Events/EventDraggedOutCar.cpp:26](../../source/game_sa/Events/EventDraggedOutCar.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1146"></a>

## Globales y helpers: source/game_sa/Events/EventGotKnockedOverByCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GotKnockedOverByCarDestructorBody(CEventGotKnockedOverByCar* self, void* /*edx*/) | [source/game_sa/Events/EventGotKnockedOverByCar.cpp:26](../../source/game_sa/Events/EventGotKnockedOverByCar.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1147"></a>

## Globales y helpers: source/game_sa/Events/EventGunAimedAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GunAimedAtDestructorBody(CEventGunAimedAt* self, void* /*edx*/) | [source/game_sa/Events/EventGunAimedAt.cpp:28](../../source/game_sa/Events/EventGunAimedAt.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1148"></a>

## Globales y helpers: source/game_sa/Events/EventGunShotWhizzedBy.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GunShotWhizzedByDestructorBody(CEventGunShotWhizzedBy* self, void* /*edx*/) | [source/game_sa/Events/EventGunShotWhizzedBy.cpp:29](../../source/game_sa/Events/EventGunShotWhizzedBy.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventGunShotWhizzedBy::Destructor 0x004B2070 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1149"></a>

## Globales y helpers: source/game_sa/Events/EventKnockOffBike.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KnockOffBikeDestructorBody(CEventKnockOffBike* self, void* /*edx*/) | [source/game_sa/Events/EventKnockOffBike.cpp:31](../../source/game_sa/Events/EventKnockOffBike.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventKnockOffBike::Destructor 0x004AFE70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KnockOffScale(const CVector& v, float k) | [source/game_sa/Events/EventKnockOffBike.cpp:180](../../source/game_sa/Events/EventKnockOffBike.cpp#L180) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KnockOffCombine(const CVector& up, float upK, const CVector& side, float sideK) | [source/game_sa/Events/EventKnockOffBike.cpp:187](../../source/game_sa/Events/EventKnockOffBike.cpp#L187) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1150"></a>

## Globales y helpers: source/game_sa/Events/EventObjectCollision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ObjectCollisionDestructorBody(CEventObjectCollision* self, void* /*edx*/) | [source/game_sa/Events/EventObjectCollision.cpp:26](../../source/game_sa/Events/EventObjectCollision.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventObjectCollision::Destructor 0x004ACDD0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1151"></a>

## Globales y helpers: source/game_sa/Events/EventPassObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PassObjectDestructorBody(CEventPassObject* self, void* /*edx*/) | [source/game_sa/Events/EventPassObject.cpp:30](../../source/game_sa/Events/EventPassObject.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPassObject::Destructor 0x0065DCF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1152"></a>

## Globales y helpers: source/game_sa/Events/EventPedCollisionWithPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedCollisionWithPedDestructorBody(CEventPedCollisionWithPed* self, void* /*edx*/) | [source/game_sa/Events/EventPedCollisionWithPed.cpp:27](../../source/game_sa/Events/EventPedCollisionWithPed.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPedCollisionWithPed::Destructor 0x004ACA70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1153"></a>

## Globales y helpers: source/game_sa/Events/EventPedToChase.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedToChaseDestructorBody(CEventPedToChase* self, void* /*edx*/) | [source/game_sa/Events/EventPedToChase.cpp:26](../../source/game_sa/Events/EventPedToChase.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1154"></a>

## Globales y helpers: source/game_sa/Events/EventPedToFlee.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedToFleeDestructorBody(CEventPedToFlee* self, void* /*edx*/) | [source/game_sa/Events/EventPedToFlee.cpp:26](../../source/game_sa/Events/EventPedToFlee.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1155"></a>

## Globales y helpers: source/game_sa/Events/EventPlayerCommandToGroup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlayerCommandToGroupDestructorBody(CEventPlayerCommandToGroup* self, void* /*edx*/) | [source/game_sa/Events/EventPlayerCommandToGroup.cpp:26](../../source/game_sa/Events/EventPlayerCommandToGroup.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPlayerCommandToGroup::Destructor 0x004B2470 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1156"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialGetRunOver.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialGetRunOverDestructorBody(CEventPotentialGetRunOver* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialGetRunOver.cpp:25](../../source/game_sa/Events/EventPotentialGetRunOver.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1157"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialWalkIntoObjectDestructorBody(CEventPotentialWalkIntoObject* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialWalkIntoObject.cpp:26](../../source/game_sa/Events/EventPotentialWalkIntoObject.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialWalkIntoObject::Destructor 0x004AE670 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1158"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialWalkIntoPedDestructorBody(CEventPotentialWalkIntoPed* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialWalkIntoPed.cpp:27](../../source/game_sa/Events/EventPotentialWalkIntoPed.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialWalkIntoPed::Destructor 0x004AE7A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1159"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialWalkIntoVehicleDestructorBody(CEventPotentialWalkIntoVehicle* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp:26](../../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialWalkIntoVehicle::Destructor 0x004AE3C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1160"></a>

## Globales y helpers: source/game_sa/Events/EventScriptCommand.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DestructorUnwind(EXCEPTION_RECORD* record, void* frame, CONTEXT*, void*) | [source/game_sa/Events/EventScriptCommand.cpp:121](../../source/game_sa/Events/EventScriptCommand.cpp#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

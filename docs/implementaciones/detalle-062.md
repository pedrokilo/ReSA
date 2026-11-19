<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 19/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1221"></a>

## Globales y helpers: source/game_sa/Events/EventPassObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PassObjectDestructorBody(CEventPassObject* self, void* /*edx*/) | [source/game_sa/Events/EventPassObject.cpp:30](../../source/game_sa/Events/EventPassObject.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPassObject::Destructor 0x0065DCF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1222"></a>

## Globales y helpers: source/game_sa/Events/EventPedCollisionWithPlayer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedCollisionWithPlayerDestructorBody(CEventPedCollisionWithPlayer* self, void* /*edx*/) | [source/game_sa/Events/EventPedCollisionWithPlayer.cpp:30](../../source/game_sa/Events/EventPedCollisionWithPlayer.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPedCollisionWithPlayer::DestructorBody 0x005FED80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1223"></a>

## Globales y helpers: source/game_sa/Events/EventPedEnteredMyVehicle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedEnteredMyVehicleDestructorBody(CEventPedEnteredMyVehicle* self, void* /*edx*/) | [source/game_sa/Events/EventPedEnteredMyVehicle.cpp:26](../../source/game_sa/Events/EventPedEnteredMyVehicle.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPedEnteredMyVehicle::Destructor 0x004AEB90 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1224"></a>

## Globales y helpers: source/game_sa/Events/EventPedToChase.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedToChaseDestructorBody(CEventPedToChase* self, void* /*edx*/) | [source/game_sa/Events/EventPedToChase.cpp:27](../../source/game_sa/Events/EventPedToChase.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPedToChase::Destructor 0x004AF1C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1225"></a>

## Globales y helpers: source/game_sa/Events/EventPedToFlee.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedToFleeDestructorBody(CEventPedToFlee* self, void* /*edx*/) | [source/game_sa/Events/EventPedToFlee.cpp:27](../../source/game_sa/Events/EventPedToFlee.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPedToFlee::Destructor 0x004AF2D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1226"></a>

## Globales y helpers: source/game_sa/Events/EventPlayerCollisionWithPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlayerCollisionWithPedDestructorBody(CEventPlayerCollisionWithPed* self, void* /*edx*/) | [source/game_sa/Events/EventPlayerCollisionWithPed.cpp:30](../../source/game_sa/Events/EventPlayerCollisionWithPed.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPlayerCollisionWithPed::DestructorBody 0x005FEE80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1227"></a>

## Globales y helpers: source/game_sa/Events/EventPlayerCommandToGroupGather.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlayerCommandToGroupGatherDestructorBody(CEventPlayerCommandToGroupGather* self, void* /*edx*/) | [source/game_sa/Events/EventPlayerCommandToGroupGather.cpp:30](../../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPlayerCommandToGroupGather::DestructorBody 0x00609300 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PlayerCommandToGroupGatherScalarDeletingDestructor(CEventPlayerCommandToGroupGather* self, void* /*edx*/, uint8 flags) | [source/game_sa/Events/EventPlayerCommandToGroupGather.cpp:37](../../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPlayerCommandToGroupGather::ScalarDeletingDestructor 0x006092E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1228"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialGetRunOver.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialGetRunOverDestructorBody(CEventPotentialGetRunOver* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialGetRunOver.cpp:26](../../source/game_sa/Events/EventPotentialGetRunOver.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialGetRunOver::Destructor 0x004AE270 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1229"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoFire.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialWalkIntoFireDestructorBody(CEventPotentialWalkIntoFire* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialWalkIntoFire.cpp:25](../../source/game_sa/Events/EventPotentialWalkIntoFire.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialWalkIntoFire::Destructor 0x004B1F00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1230"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialWalkIntoObjectDestructorBody(CEventPotentialWalkIntoObject* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialWalkIntoObject.cpp:26](../../source/game_sa/Events/EventPotentialWalkIntoObject.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialWalkIntoObject::Destructor 0x004AE670 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1231"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialWalkIntoPedDestructorBody(CEventPotentialWalkIntoPed* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialWalkIntoPed.cpp:27](../../source/game_sa/Events/EventPotentialWalkIntoPed.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialWalkIntoPed::Destructor 0x004AE7A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1232"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialWalkIntoVehicleDestructorBody(CEventPotentialWalkIntoVehicle* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp:26](../../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialWalkIntoVehicle::Destructor 0x004AE3C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1233"></a>

## Globales y helpers: source/game_sa/Events/EventRevived.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RevivedDestructorBody(CEventRevived* self, void* /*edx*/) | [source/game_sa/Events/EventRevived.cpp:25](../../source/game_sa/Events/EventRevived.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventRevived::Destructor 0x004AECA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1234"></a>

## Globales y helpers: source/game_sa/Events/EventScriptCommand.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DestructorUnwind(EXCEPTION_RECORD* record, void* frame, CONTEXT*, void*) | [source/game_sa/Events/EventScriptCommand.cpp:126](../../source/game_sa/Events/EventScriptCommand.cpp#L126) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall ScriptCommandScalarDeletingDestructor(CEventScriptCommand* self, void* /*edx*/, uint8 flags) | [source/game_sa/Events/EventScriptCommand.cpp:171](../../source/game_sa/Events/EventScriptCommand.cpp#L171) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1235"></a>

## Globales y helpers: source/game_sa/Events/EventScriptedAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ScriptedAttractorDestructorBody(CEventScriptedAttractor* self, void* /*edx*/) | [source/game_sa/Events/EventScriptedAttractor.cpp:29](../../source/game_sa/Events/EventScriptedAttractor.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventScriptedAttractor::DestructorBody 0x005FEF70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1236"></a>

## Globales y helpers: source/game_sa/Events/EventSeenPanickedPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SeenPanickedPedDestructorBody(CEventSeenPanickedPed* self, void* /*edx*/) | [source/game_sa/Events/EventSeenPanickedPed.cpp:26](../../source/game_sa/Events/EventSeenPanickedPed.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventSeenPanickedPed::Destructor 0x004B2130 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1237"></a>

## Globales y helpers: source/game_sa/Events/EventSexyPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SexyPedDestructorBody(CEventSexyPed* self, void* /*edx*/) | [source/game_sa/Events/EventSexyPed.cpp:26](../../source/game_sa/Events/EventSexyPed.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventSexyPed::Destructor 0x004AEEA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1238"></a>

## Globales y helpers: source/game_sa/Events/EventSexyVehicle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SexyVehicleDestructorBody(CEventSexyVehicle* self, void* /*edx*/) | [source/game_sa/Events/EventSexyVehicle.cpp:26](../../source/game_sa/Events/EventSexyVehicle.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventSexyVehicle::Destructor 0x004AF0A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1239"></a>

## Globales y helpers: source/game_sa/Events/EventSoundQuiet.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SoundQuietDestructorBody(CEventSoundQuiet* self, void* /*edx*/) | [source/game_sa/Events/EventSoundQuiet.cpp:31](../../source/game_sa/Events/EventSoundQuiet.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall SoundQuietScalarDeletingDestructor(CEventSoundQuiet* self, void* /*edx*/, uint8 flags) | [source/game_sa/Events/EventSoundQuiet.cpp:41](../../source/game_sa/Events/EventSoundQuiet.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventSoundQuiet::Destructor 0x005DEA00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote165ABody_CEventSoundQuiet() | [source/game_sa/Events/EventSoundQuiet.cpp:50](../../source/game_sa/Events/EventSoundQuiet.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1240"></a>

## Globales y helpers: source/game_sa/Events/EventSpecial.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SpecialDestructorBody(CEventSpecial* self, void* /*edx*/) | [source/game_sa/Events/EventSpecial.cpp:34](../../source/game_sa/Events/EventSpecial.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventSpecial::Destructor 0x004B1B50 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

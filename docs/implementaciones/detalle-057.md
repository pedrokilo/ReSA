<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 07/11/2026 (Ciudad de México):** 851 grupos / 9290 funciones reconstruidas; 9290 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1121"></a>

## Globales y helpers: source/game_sa/Enums/eWeatherType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsExtraSunny(eWeatherType wt) | [source/game_sa/Enums/eWeatherType.h:39](../../source/game_sa/Enums/eWeatherType.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1122"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AcquaintancePedDestructorBody(CEventAcquaintancePed* self, void* /*edx*/) | [source/game_sa/Events/EventAcquaintancePed.cpp:26](../../source/game_sa/Events/EventAcquaintancePed.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventAcquaintancePed::Destructor 0x004AF890 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1123"></a>

## Globales y helpers: source/game_sa/Events/EventAreaCodes.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AreaCodesDestructorBody(CEventAreaCodes* self, void* /*edx*/) | [source/game_sa/Events/EventAreaCodes.cpp:26](../../source/game_sa/Events/EventAreaCodes.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventAreaCodes::Destructor 0x004B2210 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1124"></a>

## Globales y helpers: source/game_sa/Events/EventAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AttractorDestructorBody(CEventAttractor* self, void* /*edx*/) | [source/game_sa/Events/EventAttractor.cpp:28](../../source/game_sa/Events/EventAttractor.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventAttractor::Destructor 0x004AF400 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1125"></a>

## Globales y helpers: source/game_sa/Events/EventBuildingCollision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BuildingCollisionDestructorBody(CEventBuildingCollision* self, void* /*edx*/) | [source/game_sa/Events/EventBuildingCollision.cpp:26](../../source/game_sa/Events/EventBuildingCollision.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventBuildingCollision::Destructor 0x004ACFF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1126"></a>

## Globales y helpers: source/game_sa/Events/EventDamage.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComputeHitAnimDelayTimer(float delay) | [source/game_sa/Events/EventDamage.cpp:447](../../source/game_sa/Events/EventDamage.cpp#L447) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1127"></a>

## Globales y helpers: source/game_sa/Events/EventDanger.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DangerDestructorBody(CEventDanger* self, void* /*edx*/) | [source/game_sa/Events/EventDanger.cpp:26](../../source/game_sa/Events/EventDanger.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventDanger::Destructor 0x004B26A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1128"></a>

## Globales y helpers: source/game_sa/Events/EventGunShot.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GunShotDestructorBody(CEventGunShot* self, void* /*edx*/) | [source/game_sa/Events/EventGunShot.cpp:26](../../source/game_sa/Events/EventGunShot.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventGunShot::Destructor 0x004AC720 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1129"></a>

## Globales y helpers: source/game_sa/Events/EventKnockOffBike.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KnockOffBikeDestructorBody(CEventKnockOffBike* self, void* /*edx*/) | [source/game_sa/Events/EventKnockOffBike.cpp:31](../../source/game_sa/Events/EventKnockOffBike.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventKnockOffBike::Destructor 0x004AFE70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KnockOffScale(const CVector& v, float k) | [source/game_sa/Events/EventKnockOffBike.cpp:180](../../source/game_sa/Events/EventKnockOffBike.cpp#L180) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KnockOffCombine(const CVector& up, float upK, const CVector& side, float sideK) | [source/game_sa/Events/EventKnockOffBike.cpp:187](../../source/game_sa/Events/EventKnockOffBike.cpp#L187) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1130"></a>

## Globales y helpers: source/game_sa/Events/EventObjectCollision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ObjectCollisionDestructorBody(CEventObjectCollision* self, void* /*edx*/) | [source/game_sa/Events/EventObjectCollision.cpp:26](../../source/game_sa/Events/EventObjectCollision.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventObjectCollision::Destructor 0x004ACDD0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1131"></a>

## Globales y helpers: source/game_sa/Events/EventPedCollisionWithPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedCollisionWithPedDestructorBody(CEventPedCollisionWithPed* self, void* /*edx*/) | [source/game_sa/Events/EventPedCollisionWithPed.cpp:27](../../source/game_sa/Events/EventPedCollisionWithPed.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPedCollisionWithPed::Destructor 0x004ACA70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1132"></a>

## Globales y helpers: source/game_sa/Events/EventPlayerCommandToGroup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlayerCommandToGroupDestructorBody(CEventPlayerCommandToGroup* self, void* /*edx*/) | [source/game_sa/Events/EventPlayerCommandToGroup.cpp:26](../../source/game_sa/Events/EventPlayerCommandToGroup.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPlayerCommandToGroup::Destructor 0x004B2470 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1133"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialWalkIntoObjectDestructorBody(CEventPotentialWalkIntoObject* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialWalkIntoObject.cpp:26](../../source/game_sa/Events/EventPotentialWalkIntoObject.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialWalkIntoObject::Destructor 0x004AE670 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1134"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialWalkIntoPedDestructorBody(CEventPotentialWalkIntoPed* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialWalkIntoPed.cpp:27](../../source/game_sa/Events/EventPotentialWalkIntoPed.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialWalkIntoPed::Destructor 0x004AE7A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1135"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialWalkIntoVehicleDestructorBody(CEventPotentialWalkIntoVehicle* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp:26](../../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialWalkIntoVehicle::Destructor 0x004AE3C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1136"></a>

## Globales y helpers: source/game_sa/Events/EventScriptCommand.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DestructorUnwind(EXCEPTION_RECORD* record, void* frame, CONTEXT*, void*) | [source/game_sa/Events/EventScriptCommand.cpp:121](../../source/game_sa/Events/EventScriptCommand.cpp#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1137"></a>

## Globales y helpers: source/game_sa/Events/EventSeenPanickedPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SeenPanickedPedDestructorBody(CEventSeenPanickedPed* self, void* /*edx*/) | [source/game_sa/Events/EventSeenPanickedPed.cpp:26](../../source/game_sa/Events/EventSeenPanickedPed.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventSeenPanickedPed::Destructor 0x004B2130 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1138"></a>

## Globales y helpers: source/game_sa/Events/EventStuckInAir.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StuckInAirDestructorBody(CEventStuckInAir* self, void* /*edx*/) | [source/game_sa/Events/EventStuckInAir.cpp:26](../../source/game_sa/Events/EventStuckInAir.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventStuckInAir::Destructor 0x004B1520 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1139"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleCollision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehicleCollisionDestructorBody(CEventVehicleCollision* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleCollision.cpp:30](../../source/game_sa/Events/EventVehicleCollision.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventVehicleCollision::Destructor 0x004AC920 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1140"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleDied.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehicleDiedDestructorBody(CEventVehicleDied* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleDied.cpp:26](../../source/game_sa/Events/EventVehicleDied.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventVehicleDied::Destructor 0x004B0DA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

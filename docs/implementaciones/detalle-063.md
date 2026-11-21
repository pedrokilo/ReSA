# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1241"></a>

## Globales y helpers: source/game_sa/Events/EventHitByWaterCannon.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HitByWaterCannonDestructorBody(CEventHitByWaterCannon* self, void* /*edx*/) | [source/game_sa/Events/EventHitByWaterCannon.cpp:25](../../source/game_sa/Events/EventHitByWaterCannon.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventHitByWaterCannon::Destructor 0x004B1320 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1242"></a>

## Globales y helpers: source/game_sa/Events/EventInAir.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InAirDestructorBody(CEventInAir* self, void* /*edx*/) | [source/game_sa/Events/EventInAir.cpp:25](../../source/game_sa/Events/EventInAir.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventInAir::Destructor 0x004B0D00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1243"></a>

## Globales y helpers: source/game_sa/Events/EventInWater.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InWaterDestructorBody(CEventInWater* self, void* /*edx*/) | [source/game_sa/Events/EventInWater.cpp:25](../../source/game_sa/Events/EventInWater.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventInWater::Destructor 0x004B13C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1244"></a>

## Globales y helpers: source/game_sa/Events/EventKnockOffBike.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KnockOffBikeDestructorBody(CEventKnockOffBike* self, void* /*edx*/) | [source/game_sa/Events/EventKnockOffBike.cpp:31](../../source/game_sa/Events/EventKnockOffBike.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventKnockOffBike::Destructor 0x004AFE70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall KnockOffBikeAssignExe(CEventKnockOffBike* self, void* /*edx*/, const CEventKnockOffBike* other) | [source/game_sa/Events/EventKnockOffBike.cpp:40](../../source/game_sa/Events/EventKnockOffBike.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventKnockOffBike::operator= 0x004B4AA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KnockOffScale(const CVector& v, float k) | [source/game_sa/Events/EventKnockOffBike.cpp:267](../../source/game_sa/Events/EventKnockOffBike.cpp#L267) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KnockOffCombine(const CVector& up, float upK, const CVector& side, float sideK) | [source/game_sa/Events/EventKnockOffBike.cpp:274](../../source/game_sa/Events/EventKnockOffBike.cpp#L274) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1245"></a>

## Globales y helpers: source/game_sa/Events/EventLeaderEntryExit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LeaderEntryExitDestructorBody(CEventLeaderEntryExit* self, void* /*edx*/) | [source/game_sa/Events/EventLeaderEntryExit.cpp:29](../../source/game_sa/Events/EventLeaderEntryExit.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventLeaderEntryExit::DestructorBody 0x0043E2B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1246"></a>

## Globales y helpers: source/game_sa/Events/EventLowAngerAtPlayer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LowAngerAtPlayerDestructorBody(CEventLowAngerAtPlayer* self, void* /*edx*/) | [source/game_sa/Events/EventLowAngerAtPlayer.cpp:30](../../source/game_sa/Events/EventLowAngerAtPlayer.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventLowAngerAtPlayer::DestructorBody 0x005FF5E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1247"></a>

## Globales y helpers: source/game_sa/Events/EventObjectCollision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ObjectCollisionDestructorBody(CEventObjectCollision* self, void* /*edx*/) | [source/game_sa/Events/EventObjectCollision.cpp:26](../../source/game_sa/Events/EventObjectCollision.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventObjectCollision::Destructor 0x004ACDD0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1248"></a>

## Globales y helpers: source/game_sa/Events/EventOnFire.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| OnFireDestructorBody(CEventOnFire* self, void* /*edx*/) | [source/game_sa/Events/EventOnFire.cpp:30](../../source/game_sa/Events/EventOnFire.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventOnFire::DestructorBody 0x005FF760 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1249"></a>

## Globales y helpers: source/game_sa/Events/EventPassObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PassObjectDestructorBody(CEventPassObject* self, void* /*edx*/) | [source/game_sa/Events/EventPassObject.cpp:30](../../source/game_sa/Events/EventPassObject.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPassObject::Destructor 0x0065DCF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1250"></a>

## Globales y helpers: source/game_sa/Events/EventPedCollisionWithPlayer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedCollisionWithPlayerDestructorBody(CEventPedCollisionWithPlayer* self, void* /*edx*/) | [source/game_sa/Events/EventPedCollisionWithPlayer.cpp:30](../../source/game_sa/Events/EventPedCollisionWithPlayer.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPedCollisionWithPlayer::DestructorBody 0x005FED80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1251"></a>

## Globales y helpers: source/game_sa/Events/EventPedEnteredMyVehicle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedEnteredMyVehicleDestructorBody(CEventPedEnteredMyVehicle* self, void* /*edx*/) | [source/game_sa/Events/EventPedEnteredMyVehicle.cpp:26](../../source/game_sa/Events/EventPedEnteredMyVehicle.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPedEnteredMyVehicle::Destructor 0x004AEB90 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1252"></a>

## Globales y helpers: source/game_sa/Events/EventPedToChase.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedToChaseDestructorBody(CEventPedToChase* self, void* /*edx*/) | [source/game_sa/Events/EventPedToChase.cpp:27](../../source/game_sa/Events/EventPedToChase.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPedToChase::Destructor 0x004AF1C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1253"></a>

## Globales y helpers: source/game_sa/Events/EventPedToFlee.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedToFleeDestructorBody(CEventPedToFlee* self, void* /*edx*/) | [source/game_sa/Events/EventPedToFlee.cpp:27](../../source/game_sa/Events/EventPedToFlee.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPedToFlee::Destructor 0x004AF2D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1254"></a>

## Globales y helpers: source/game_sa/Events/EventPlayerCollisionWithPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlayerCollisionWithPedDestructorBody(CEventPlayerCollisionWithPed* self, void* /*edx*/) | [source/game_sa/Events/EventPlayerCollisionWithPed.cpp:30](../../source/game_sa/Events/EventPlayerCollisionWithPed.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPlayerCollisionWithPed::DestructorBody 0x005FEE80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1255"></a>

## Globales y helpers: source/game_sa/Events/EventPlayerCommandToGroupGather.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlayerCommandToGroupGatherDestructorBody(CEventPlayerCommandToGroupGather* self, void* /*edx*/) | [source/game_sa/Events/EventPlayerCommandToGroupGather.cpp:30](../../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPlayerCommandToGroupGather::DestructorBody 0x00609300 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PlayerCommandToGroupGatherScalarDeletingDestructor(CEventPlayerCommandToGroupGather* self, void* /*edx*/, uint8 flags) | [source/game_sa/Events/EventPlayerCommandToGroupGather.cpp:37](../../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPlayerCommandToGroupGather::ScalarDeletingDestructor 0x006092E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1256"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialGetRunOver.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialGetRunOverDestructorBody(CEventPotentialGetRunOver* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialGetRunOver.cpp:26](../../source/game_sa/Events/EventPotentialGetRunOver.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialGetRunOver::Destructor 0x004AE270 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1257"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoFire.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialWalkIntoFireDestructorBody(CEventPotentialWalkIntoFire* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialWalkIntoFire.cpp:25](../../source/game_sa/Events/EventPotentialWalkIntoFire.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialWalkIntoFire::Destructor 0x004B1F00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1258"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialWalkIntoObjectDestructorBody(CEventPotentialWalkIntoObject* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialWalkIntoObject.cpp:26](../../source/game_sa/Events/EventPotentialWalkIntoObject.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialWalkIntoObject::Destructor 0x004AE670 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1259"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialWalkIntoPedDestructorBody(CEventPotentialWalkIntoPed* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialWalkIntoPed.cpp:27](../../source/game_sa/Events/EventPotentialWalkIntoPed.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialWalkIntoPed::Destructor 0x004AE7A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1260"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialWalkIntoVehicleDestructorBody(CEventPotentialWalkIntoVehicle* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp:26](../../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialWalkIntoVehicle::Destructor 0x004AE3C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

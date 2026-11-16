<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/11/2026 (Ciudad de México):** 917 grupos / 10794 funciones reconstruidas; 10794 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1201"></a>

## Globales y helpers: source/game_sa/Events/EventPlayerCommandToGroupGather.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlayerCommandToGroupGatherDestructorBody(CEventPlayerCommandToGroupGather* self, void* /*edx*/) | [source/game_sa/Events/EventPlayerCommandToGroupGather.cpp:30](../../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPlayerCommandToGroupGather::DestructorBody 0x00609300 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PlayerCommandToGroupGatherScalarDeletingDestructor(CEventPlayerCommandToGroupGather* self, void* /*edx*/, uint8 flags) | [source/game_sa/Events/EventPlayerCommandToGroupGather.cpp:37](../../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPlayerCommandToGroupGather::ScalarDeletingDestructor 0x006092E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1202"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialGetRunOver.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialGetRunOverDestructorBody(CEventPotentialGetRunOver* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialGetRunOver.cpp:25](../../source/game_sa/Events/EventPotentialGetRunOver.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1203"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialWalkIntoObjectDestructorBody(CEventPotentialWalkIntoObject* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialWalkIntoObject.cpp:26](../../source/game_sa/Events/EventPotentialWalkIntoObject.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialWalkIntoObject::Destructor 0x004AE670 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1204"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialWalkIntoPedDestructorBody(CEventPotentialWalkIntoPed* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialWalkIntoPed.cpp:27](../../source/game_sa/Events/EventPotentialWalkIntoPed.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialWalkIntoPed::Destructor 0x004AE7A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1205"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PotentialWalkIntoVehicleDestructorBody(CEventPotentialWalkIntoVehicle* self, void* /*edx*/) | [source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp:26](../../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventPotentialWalkIntoVehicle::Destructor 0x004AE3C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1206"></a>

## Globales y helpers: source/game_sa/Events/EventScriptCommand.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DestructorUnwind(EXCEPTION_RECORD* record, void* frame, CONTEXT*, void*) | [source/game_sa/Events/EventScriptCommand.cpp:124](../../source/game_sa/Events/EventScriptCommand.cpp#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall ScriptCommandScalarDeletingDestructor(CEventScriptCommand* self, void* /*edx*/, uint8 flags) | [source/game_sa/Events/EventScriptCommand.cpp:168](../../source/game_sa/Events/EventScriptCommand.cpp#L168) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1207"></a>

## Globales y helpers: source/game_sa/Events/EventScriptedAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ScriptedAttractorDestructorBody(CEventScriptedAttractor* self, void* /*edx*/) | [source/game_sa/Events/EventScriptedAttractor.cpp:29](../../source/game_sa/Events/EventScriptedAttractor.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventScriptedAttractor::DestructorBody 0x005FEF70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1208"></a>

## Globales y helpers: source/game_sa/Events/EventSeenPanickedPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SeenPanickedPedDestructorBody(CEventSeenPanickedPed* self, void* /*edx*/) | [source/game_sa/Events/EventSeenPanickedPed.cpp:26](../../source/game_sa/Events/EventSeenPanickedPed.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventSeenPanickedPed::Destructor 0x004B2130 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1209"></a>

## Globales y helpers: source/game_sa/Events/EventSoundQuiet.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SoundQuietDestructorBody(CEventSoundQuiet* self, void* /*edx*/) | [source/game_sa/Events/EventSoundQuiet.cpp:31](../../source/game_sa/Events/EventSoundQuiet.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall SoundQuietScalarDeletingDestructor(CEventSoundQuiet* self, void* /*edx*/, uint8 flags) | [source/game_sa/Events/EventSoundQuiet.cpp:41](../../source/game_sa/Events/EventSoundQuiet.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventSoundQuiet::Destructor 0x005DEA00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote165ABody_CEventSoundQuiet() | [source/game_sa/Events/EventSoundQuiet.cpp:50](../../source/game_sa/Events/EventSoundQuiet.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1210"></a>

## Globales y helpers: source/game_sa/Events/EventStuckInAir.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StuckInAirDestructorBody(CEventStuckInAir* self, void* /*edx*/) | [source/game_sa/Events/EventStuckInAir.cpp:26](../../source/game_sa/Events/EventStuckInAir.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventStuckInAir::Destructor 0x004B1520 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1211"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleCollision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehicleCollisionDestructorBody(CEventVehicleCollision* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleCollision.cpp:30](../../source/game_sa/Events/EventVehicleCollision.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventVehicleCollision::Destructor 0x004AC920 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1212"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleDamageCollision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CollisionDestructorBody(CEventVehicleDamageCollision* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleDamageCollision.cpp:30](../../source/game_sa/Events/EventVehicleDamageCollision.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventVehicleDamageCollision::Destructor 0x006A0650 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1213"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleDamageWeapon.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WeaponDestructorBody(CEventVehicleDamageWeapon* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleDamageWeapon.cpp:30](../../source/game_sa/Events/EventVehicleDamageWeapon.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventVehicleDamageWeapon::Destructor 0x0061C310 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1214"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleDied.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehicleDiedDestructorBody(CEventVehicleDied* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleDied.cpp:26](../../source/game_sa/Events/EventVehicleDied.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventVehicleDied::Destructor 0x004B0DA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1215"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleOnFire.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehicleOnFireDestructorBody(CEventVehicleOnFire* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleOnFire.cpp:26](../../source/game_sa/Events/EventVehicleOnFire.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventVehicleOnFire::Destructor 0x004B1170 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1216"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleToSteal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehicleToStealDestructorBody(CEventVehicleToSteal* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleToSteal.cpp:25](../../source/game_sa/Events/EventVehicleToSteal.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventVehicleToSteal::Destructor 0x004AF700 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1217"></a>

## Globales y helpers: source/game_sa/Events/GroupEventHandler.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPedInPlayersGroup(CPedGroup* pg, CPed* p) | [source/game_sa/Events/GroupEventHandler.cpp:111](../../source/game_sa/Events/GroupEventHandler.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MaybeAdjustTaskOfGroupThreatEvent(const CEventEditableResponse& e, CPedGroup* pg, CPed* srcPed) | [source/game_sa/Events/GroupEventHandler.cpp:124](../../source/game_sa/Events/GroupEventHandler.cpp#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PickPartnerFollower(CPedGroup* pg, CPed* other) | [source/game_sa/Events/GroupEventHandler.cpp:786](../../source/game_sa/Events/GroupEventHandler.cpp#L786) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PartnerLookAts(CPedGroup* pg, CPed* closest, const char* purpose) | [source/game_sa/Events/GroupEventHandler.cpp:827](../../source/game_sa/Events/GroupEventHandler.cpp#L827) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1218"></a>

## Globales y helpers: source/game_sa/Explosion.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| U32ToFloat(uint32 value) | [source/game_sa/Explosion.cpp:56](../../source/game_sa/Explosion.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DoesNeedToVehProcessBombTimer(eExplosionType type) | [source/game_sa/Explosion.cpp:171](../../source/game_sa/Explosion.cpp#L171) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExplosionRand01() | [source/game_sa/Explosion.cpp:184](../../source/game_sa/Explosion.cpp#L184) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1219"></a>

## Globales y helpers: source/game_sa/FileLoader.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeCIacos(float x) | [source/game_sa/FileLoader.cpp:1244](../../source/game_sa/FileLoader.cpp#L1244) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AddTextureCB(RwTexture* texture, void* dict) | [source/game_sa/FileLoader.cpp:2630](../../source/game_sa/FileLoader.cpp#L2630) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetFilename(const char* filepath) | [source/game_sa/FileLoader.cpp:2652](../../source/game_sa/FileLoader.cpp#L2652) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1220"></a>

## Globales y helpers: source/game_sa/FileMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| createDirectory(const wchar_t* path) | [source/game_sa/FileMgr.cpp:39](../../source/game_sa/FileMgr.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

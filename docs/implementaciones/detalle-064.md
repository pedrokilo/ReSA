# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1261"></a>

## Globales y helpers: source/game_sa/Events/EventRevived.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RevivedDestructorBody(CEventRevived* self, void* /*edx*/) | [source/game_sa/Events/EventRevived.cpp:25](../../source/game_sa/Events/EventRevived.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventRevived::Destructor 0x004AECA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1262"></a>

## Globales y helpers: source/game_sa/Events/EventScriptCommand.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DestructorUnwind(EXCEPTION_RECORD* record, void* frame, CONTEXT*, void*) | [source/game_sa/Events/EventScriptCommand.cpp:126](../../source/game_sa/Events/EventScriptCommand.cpp#L126) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall ScriptCommandScalarDeletingDestructor(CEventScriptCommand* self, void* /*edx*/, uint8 flags) | [source/game_sa/Events/EventScriptCommand.cpp:171](../../source/game_sa/Events/EventScriptCommand.cpp#L171) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1263"></a>

## Globales y helpers: source/game_sa/Events/EventScriptedAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ScriptedAttractorDestructorBody(CEventScriptedAttractor* self, void* /*edx*/) | [source/game_sa/Events/EventScriptedAttractor.cpp:29](../../source/game_sa/Events/EventScriptedAttractor.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventScriptedAttractor::DestructorBody 0x005FEF70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1264"></a>

## Globales y helpers: source/game_sa/Events/EventSeenPanickedPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SeenPanickedPedDestructorBody(CEventSeenPanickedPed* self, void* /*edx*/) | [source/game_sa/Events/EventSeenPanickedPed.cpp:26](../../source/game_sa/Events/EventSeenPanickedPed.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventSeenPanickedPed::Destructor 0x004B2130 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1265"></a>

## Globales y helpers: source/game_sa/Events/EventSexyPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SexyPedDestructorBody(CEventSexyPed* self, void* /*edx*/) | [source/game_sa/Events/EventSexyPed.cpp:26](../../source/game_sa/Events/EventSexyPed.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventSexyPed::Destructor 0x004AEEA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1266"></a>

## Globales y helpers: source/game_sa/Events/EventSexyVehicle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SexyVehicleDestructorBody(CEventSexyVehicle* self, void* /*edx*/) | [source/game_sa/Events/EventSexyVehicle.cpp:26](../../source/game_sa/Events/EventSexyVehicle.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventSexyVehicle::Destructor 0x004AF0A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1267"></a>

## Globales y helpers: source/game_sa/Events/EventSoundQuiet.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SoundQuietDestructorBody(CEventSoundQuiet* self, void* /*edx*/) | [source/game_sa/Events/EventSoundQuiet.cpp:31](../../source/game_sa/Events/EventSoundQuiet.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall SoundQuietScalarDeletingDestructor(CEventSoundQuiet* self, void* /*edx*/, uint8 flags) | [source/game_sa/Events/EventSoundQuiet.cpp:41](../../source/game_sa/Events/EventSoundQuiet.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventSoundQuiet::Destructor 0x005DEA00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote165ABody_CEventSoundQuiet() | [source/game_sa/Events/EventSoundQuiet.cpp:50](../../source/game_sa/Events/EventSoundQuiet.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1268"></a>

## Globales y helpers: source/game_sa/Events/EventSpecial.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SpecialDestructorBody(CEventSpecial* self, void* /*edx*/) | [source/game_sa/Events/EventSpecial.cpp:34](../../source/game_sa/Events/EventSpecial.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventSpecial::Destructor 0x004B1B50 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1269"></a>

## Globales y helpers: source/game_sa/Events/EventStuckInAir.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StuckInAirDestructorBody(CEventStuckInAir* self, void* /*edx*/) | [source/game_sa/Events/EventStuckInAir.cpp:26](../../source/game_sa/Events/EventStuckInAir.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventStuckInAir::Destructor 0x004B1520 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1270"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleCollision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehicleCollisionDestructorBody(CEventVehicleCollision* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleCollision.cpp:30](../../source/game_sa/Events/EventVehicleCollision.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventVehicleCollision::Destructor 0x004AC920 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1271"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleDamageCollision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CollisionDestructorBody(CEventVehicleDamageCollision* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleDamageCollision.cpp:30](../../source/game_sa/Events/EventVehicleDamageCollision.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventVehicleDamageCollision::Destructor 0x006A0650 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1272"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleDamageWeapon.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WeaponDestructorBody(CEventVehicleDamageWeapon* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleDamageWeapon.cpp:30](../../source/game_sa/Events/EventVehicleDamageWeapon.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventVehicleDamageWeapon::Destructor 0x0061C310 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1273"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleDied.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehicleDiedDestructorBody(CEventVehicleDied* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleDied.cpp:26](../../source/game_sa/Events/EventVehicleDied.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventVehicleDied::Destructor 0x004B0DA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1274"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleOnFire.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehicleOnFireDestructorBody(CEventVehicleOnFire* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleOnFire.cpp:26](../../source/game_sa/Events/EventVehicleOnFire.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventVehicleOnFire::Destructor 0x004B1170 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1275"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleToSteal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehicleToStealDestructorBody(CEventVehicleToSteal* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleToSteal.cpp:25](../../source/game_sa/Events/EventVehicleToSteal.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventVehicleToSteal::Destructor 0x004AF700 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1276"></a>

## Globales y helpers: source/game_sa/Events/GroupEventHandler.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPedInPlayersGroup(CPedGroup* pg, CPed* p) | [source/game_sa/Events/GroupEventHandler.cpp:111](../../source/game_sa/Events/GroupEventHandler.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MaybeAdjustTaskOfGroupThreatEvent(const CEventEditableResponse& e, CPedGroup* pg, CPed* srcPed) | [source/game_sa/Events/GroupEventHandler.cpp:124](../../source/game_sa/Events/GroupEventHandler.cpp#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PickPartnerFollower(CPedGroup* pg, CPed* other) | [source/game_sa/Events/GroupEventHandler.cpp:786](../../source/game_sa/Events/GroupEventHandler.cpp#L786) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PartnerLookAts(CPedGroup* pg, CPed* closest, const char* purpose) | [source/game_sa/Events/GroupEventHandler.cpp:827](../../source/game_sa/Events/GroupEventHandler.cpp#L827) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1277"></a>

## Globales y helpers: source/game_sa/ExeProtectionStub.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Load32(uint32 address) | [source/game_sa/ExeProtectionStub.cpp:25](../../source/game_sa/ExeProtectionStub.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Store32(uint32 address, uint32 value) | [source/game_sa/ExeProtectionStub.cpp:30](../../source/game_sa/ExeProtectionStub.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SlotFn(uint32 slotAddress) | [source/game_sa/ExeProtectionStub.cpp:40](../../source/game_sa/ExeProtectionStub.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DecodeWord(const uint8* bytes) | [source/game_sa/ExeProtectionStub.cpp:45](../../source/game_sa/ExeProtectionStub.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ResolveProc(const ProcEntry& entry) | [source/game_sa/ExeProtectionStub.cpp:161](../../source/game_sa/ExeProtectionStub.cpp#L161) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1278"></a>

## Globales y helpers: source/game_sa/Explosion.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| U32ToFloat(uint32 value) | [source/game_sa/Explosion.cpp:56](../../source/game_sa/Explosion.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DoesNeedToVehProcessBombTimer(eExplosionType type) | [source/game_sa/Explosion.cpp:171](../../source/game_sa/Explosion.cpp#L171) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExplosionRand01() | [source/game_sa/Explosion.cpp:184](../../source/game_sa/Explosion.cpp#L184) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1279"></a>

## Globales y helpers: source/game_sa/FileLoader.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeCIacos(float x) | [source/game_sa/FileLoader.cpp:1244](../../source/game_sa/FileLoader.cpp#L1244) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AddTextureCB(RwTexture* texture, void* dict) | [source/game_sa/FileLoader.cpp:2630](../../source/game_sa/FileLoader.cpp#L2630) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetFilename(const char* filepath) | [source/game_sa/FileLoader.cpp:2652](../../source/game_sa/FileLoader.cpp#L2652) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1280"></a>

## Globales y helpers: source/game_sa/FileMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| createDirectory(const wchar_t* path) | [source/game_sa/FileMgr.cpp:39](../../source/game_sa/FileMgr.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

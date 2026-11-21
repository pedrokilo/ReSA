# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1541"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PartnerShoveDestructorBody(CTaskComplexPartnerShove* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexPartnerShove::Destructor 0x00684750 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1542"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PassObjectDestructorBody(CTaskComplexPassObject* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexPassObject::Destructor 0x00661230 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote156ABody_CTaskComplexPassObject() | [source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1543"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexProstituteSolicitDestructorBody(CTaskComplexProstituteSolicit* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp#L34) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexProstituteSolicit::Destructor 0x00661AF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexProstituteSolicit() | [source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp:48](../../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1544"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & FieldAt(void* object, uint32 offset) | [source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp:49](../../source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall ReactToAttackControlSubTask(CTaskComplexReactToAttack* self, void* /*edx*/, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp:111](../../source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexReactToAttack::ControlSubTask 0x00620E00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReactToAttackGetTaskType(CTaskComplexReactToAttack* /*self*/, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp:161](../../source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp#L161) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexReactToAttack::GetTaskType 0x00620B80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1545"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSeekCoverUntilTargetDeadDestructorBody(CTaskComplexSeekCoverUntilTargetDead* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSeekCoverUntilTargetDead::Destructor 0x00622C20 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote164ABody_CTaskComplexSeekCoverUntilTargetDead() | [source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1546"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSignalAtPedDestructorBody(CTaskComplexSignalAtPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSignalAtPed::Destructor 0x00660AB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote154ABody_CTaskComplexSignalAtPed() | [source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1547"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSmartFleeEntityDestructorBody(CTaskComplexSmartFleeEntity* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSmartFleeEntity::Destructor 0x0065C4D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexSmartFleeEntity() | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1548"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSmartFleePointDestructorBody(CTaskComplexSmartFleePoint* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSmartFleePoint::Destructor 0x0065BDB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexSmartFleePoint() | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1549"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexStareAtPedDestructorBody(CTaskComplexStareAtPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexStareAtPed::Destructor 0x00660310 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote153ABody_CTaskComplexStareAtPed() | [source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1550"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall StuckInAirConstructorBody(CTaskComplexStuckInAir* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:189](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L189) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexStuckInAir::Constructor 0x0067BA40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ComplexStuckInAirDestructorBody(CTaskComplexStuckInAir* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:215](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L215) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexStuckInAir::DestructorBody 0x0067BA70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote128AComplexStuckInAirDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp:221](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp#L221) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1551"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SunbatheWeatherAllows() | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:66](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00C8131C |
| CTaskComplexSunbatheDestructorBody(CTaskComplexSunbathe* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:81](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L81) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexSunbathe::Destructor 0x00632050 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexSunbathe() | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:95](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1552"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TrackEntityDestructorBody(CTaskComplexTrackEntity* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexTrackEntity::Destructor 0x0065F460 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote158ABody_CTaskComplexTrackEntity() | [source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1553"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TreatAccidentDestructorBody(CTaskComplexTreatAccident* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexTreatAccident::Destructor 0x00658AE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1554"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexUseClosestFreeScriptedAttractorDestructorBody(CTaskComplexUseClosestFreeScriptedAttractor* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexUseClosestFreeScriptedAttractor::Destructor 0x00634720 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexUseClosestFreeScriptedAttractor() | [source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1555"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall UseEffectRunningConstructorBody(CTaskComplexUseEffectRunning* self, void* /*edx*/, C2dEffectPedAttractor* effect, CEntity* entity) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseEffectRunning::Constructor 0x00636780 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UseEffectRunningDestructorBody(CTaskComplexUseEffectRunning* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseEffectRunning::Destructor 0x00636850 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1556"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall UseEffectSprintingConstructorBody(CTaskComplexUseEffectSprinting* self, void* /*edx*/, C2dEffectPedAttractor* effect, CEntity* entity) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseEffectSprinting::Constructor 0x00636860 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UseEffectSprintingDestructorBody(CTaskComplexUseEffectSprinting* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseEffectSprinting::Destructor 0x00636930 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1557"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall UseGogglesConstructorBody(CTaskComplexUseGoggles* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp:81](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseGoggles::Constructor 0x00634EF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UseGogglesDestructorBody(CTaskComplexUseGoggles* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp:89](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp#L89) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseGoggles::Destructor 0x00634F20 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TaskComplexUseGogglesTestCode() | [source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp:121](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1558"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClearPhoneFlags(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp:46](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall UseMobilePhoneConstructor(CTaskComplexUseMobilePhone* self, void* /*edx*/, int32 duration) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp:54](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseMobilePhone::Constructor 0x006348A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UseMobilePhoneGetTaskType(void* /*self*/, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp:68](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseMobilePhone::GetTaskType 0x006348D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UseMobilePhoneDestructorBody(CTaskComplexUseMobilePhone* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp:74](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexUseMobilePhone::DestructorBody 0x006348E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NewPhoneAnimTaskImpl(int32 animId, int32 taskType, uintptr_t exeNameAddress) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp:127](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp#L127) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall UseMobilePhoneScalarDeletingDestructor(CTaskComplexUseMobilePhone* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp:207](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp#L207) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplex::Destructor 0x0061A3D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1559"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexUseScriptedBrainDestructorBody(CTaskComplexUseScriptedBrain* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexUseScriptedBrain::Destructor 0x00635900 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskComplexUseScriptedBrain() | [source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1560"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexUseWaterCannonDestructorBody(CTaskComplexUseWaterCannon* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Revalidada en migración: CTaskComplexUseWaterCannon::DestructorBody 0x006597B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall CTaskComplexUseWaterCannonScalarDeletingDestructor(CTaskComplexUseWaterCannon* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote153ABody_CTaskComplexUseWaterCannon() | [source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

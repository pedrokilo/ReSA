# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1561"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWaitForBusDestructorBody(CTaskComplexWaitForBus* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexWaitForBus::Destructor 0x00632930 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexWaitForBus() | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1562"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWaitForDryWeatherDestructorBody(CTaskComplexWaitForDryWeather* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexWaitForDryWeather::Destructor 0x00632830 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexWaitForDryWeather() | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1563"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaitForPedGetTaskType(CTaskComplexWaitForPed* /*self*/, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexWaitForPed::GetTaskType 0x006833C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaitForPedMakeAbortable(CTaskComplexWaitForPed* /*self*/, void* /*edx*/, CPed* /*ped*/, eAbortPriority /*priority*/, const CEvent* /*event*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp:50](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexWaitForPed::MakeAbortable 0x00683430 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CTaskComplexWaitForPed() | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp:66](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaitForPedUnwind(EXCEPTION_RECORD* record, void* frame, CONTEXT*, void*) | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp:93](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1564"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WalkAlongsidePedDestructorBody(CTaskComplexWalkAlongsidePed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp#L32) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexWalkAlongsidePed::Destructor 0x006832C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CTaskComplexWalkAlongsidePed() | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp:46](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1565"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * NewEmptyPointRoute() | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:57](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddPointUnlessFull(CPointRoute& route, const CVector& pt) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:66](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopyPointRoute(CPointRoute& dst, const CPointRoute& src) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:74](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MulMatrixPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:79](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1566"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWalkRoundFireDestructorBody(CTaskComplexWalkRoundFire* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexWalkRoundFire::Destructor 0x00655780 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskComplexWalkRoundFire() | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1567"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall WanderScalarDeletingDestructor(CTaskComplexWander* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp:450](../../source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp#L450) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1568"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWanderCopDestructorBody(CTaskComplexWanderCop* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1569"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWanderCriminalDestructorBody(CTaskComplexWanderCriminal* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexWanderCriminal::Destructor 0x0048E720 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexWanderCriminal() | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1570"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWanderFleeDestructorBody(CTaskComplexWanderFlee* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexWanderFlee::Destructor 0x0065B370 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexWanderFlee() | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1571"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WanderMedicDestructorBody(CTaskComplexWanderMedic* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexWanderMedic::Destructor 0x00658850 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1572"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WanderStandardDestructorBody(CTaskComplexWanderStandard* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexWanderStandard::Destructor 0x0048E600 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1573"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GangHassleSeekRadius() | [source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp:95](../../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1574"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskGoToVehicleAndLeanDestructorBody(CTaskGoToVehicleAndLean* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp#L32) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskGoToVehicleAndLean::Destructor 0x00660EE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall CTaskGoToVehicleAndLeanScalarDeletingDestructor(CTaskGoToVehicleAndLean* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskGoToVehicleAndLean::ScalarDeletingDestructor 0x006644E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote154ABody_CTaskGoToVehicleAndLean() | [source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1575"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleAbseil.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AbseilDestructorBody(CTaskSimpleAbseil* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleAbseil.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAbseil.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleAbseil::DestructorBody 0x00659E00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1576"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AchieveHeadingDestructorBody(CTaskSimpleAchieveHeading* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleAchieveHeading::Destructor 0x00667E70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1577"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall SimpleAnimConstructorBody(CTaskSimpleAnim* self, void* /*edx*/, uint32 holdLastFrame) | [source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleAnim::Constructor 0x0061A6C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleAnimDestructorBody(CTaskSimpleAnim* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp:46](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleAnim() | [source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp:71](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1578"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall ArrestPedConstructorBody(CTaskSimpleArrestPed* self, void* /*edx*/, CEntity* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleArrestPed::Constructor 0x0068B620 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ArrestPedDestructorBody(CTaskSimpleArrestPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp:46](../../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleArrestPed__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp:72](../../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1579"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BeHitAtan2(float x, float y) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1580"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BeHitFinishAnimCB(CAnimBlendAssociation* /*anim*/, void* data) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleBeHitWhileMoving::FinishAnimCB 0x0061FF30 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeRand() | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp:50](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BeHitWhileMovingDestructorBody(CTaskSimpleBeHitWhileMoving* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp:57](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CAnimBlendAssociation::SetFinishCallback 0x004CEBE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall BeHitWhileMovingConstructor(CTaskSimpleBeHitWhileMoving* self, void* /*edx*/, int32 mode, int32 dirn) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp:81](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleBeHitWhileMoving::Constructor 0x0061FE30 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BeHitWhileMovingGetTaskType(void* /*self*/, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp:93](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleBeHitWhileMoving::GetTaskType 0x0061FE60 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BeHitWhileMovingProcessPed(CTaskSimpleBeHitWhileMoving* self, void* /*edx*/, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp:267](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp#L267) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleBeHitWhileMoving::ProcessPed 0x00620290 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

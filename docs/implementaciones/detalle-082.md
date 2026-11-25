<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1621"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskGoToVehicleAndLeanDestructorBody(CTaskGoToVehicleAndLean* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp#L32) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskGoToVehicleAndLean::Destructor 0x00660EE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall CTaskGoToVehicleAndLeanScalarDeletingDestructor(CTaskGoToVehicleAndLean* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskGoToVehicleAndLean::ScalarDeletingDestructor 0x006644E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote154ABody_CTaskGoToVehicleAndLean() | [source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1622"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleAbseil.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AbseilDestructorBody(CTaskSimpleAbseil* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleAbseil.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAbseil.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleAbseil::DestructorBody 0x00659E00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1623"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AchieveHeadingDestructorBody(CTaskSimpleAchieveHeading* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleAchieveHeading::Destructor 0x00667E70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1624"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall SimpleAnimConstructorBody(CTaskSimpleAnim* self, void* /*edx*/, uint32 holdLastFrame) | [source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleAnim::Constructor 0x0061A6C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleAnimDestructorBody(CTaskSimpleAnim* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp:46](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleAnim() | [source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp:71](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1625"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall ArrestPedConstructorBody(CTaskSimpleArrestPed* self, void* /*edx*/, CEntity* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleArrestPed::Constructor 0x0068B620 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ArrestPedDestructorBody(CTaskSimpleArrestPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp:46](../../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleArrestPed__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp:72](../../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1626"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BeHitAtan2(float x, float y) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1627"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BeHitFinishAnimCB(CAnimBlendAssociation* /*anim*/, void* data) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleBeHitWhileMoving::FinishAnimCB 0x0061FF30 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeRand() | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp:50](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BeHitWhileMovingDestructorBody(CTaskSimpleBeHitWhileMoving* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp:57](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CAnimBlendAssociation::SetFinishCallback 0x004CEBE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall BeHitWhileMovingConstructor(CTaskSimpleBeHitWhileMoving* self, void* /*edx*/, int32 mode, int32 dirn) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp:81](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleBeHitWhileMoving::Constructor 0x0061FE30 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BeHitWhileMovingGetTaskType(void* /*self*/, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp:93](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleBeHitWhileMoving::GetTaskType 0x0061FE60 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BeHitWhileMovingProcessPed(CTaskSimpleBeHitWhileMoving* self, void* /*edx*/, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp:267](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp#L267) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleBeHitWhileMoving::ProcessPed 0x00620290 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1628"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BeKickedFinishAnimCB(CAnimBlendAssociation* /*anim*/, void* data) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleBeKickedOnGround::FinishAnimCB 0x0061FD10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall BeKickedOnGroundConstructor(CTaskSimpleBeKickedOnGround* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp:72](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleBeKickedOnGround::Constructor 0x0061FC20 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BeKickedOnGroundGetTaskType(void* /*self*/, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp:82](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleBeKickedOnGround::GetTaskType 0x0061FC40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BeKickedOnGroundProcessPed(CTaskSimpleBeKickedOnGround* self, void* /*edx*/, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp:132](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp#L132) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleBeKickedOnGround::ProcessPed 0x0061FE00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1629"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleBikeJackedDestructorBody(CTaskSimpleBikeJacked* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleBikeJacked::Destructor 0x00648C40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskSimpleBikeJacked() | [source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1630"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote152ACallback_CarDriveRollDoor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp:73](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp#L73) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Sin(float angle) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp:186](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp#L186) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1631"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarForcePedOutDestructorBody(CTaskSimpleCarForcePedOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp:90](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp#L90) | C++ · Cuerpo fuente con errores de análisis | Revalidada en migración: CTaskSimpleCarForcePedOut::Destructor 0x00647790 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote159ABody_CTaskSimpleCarForcePedOut() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp:104](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1632"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUseDestructorBody(CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::Destructor 0x00646E50 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1633"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarJumpOutDestructorBody(CTaskSimpleCarJumpOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarJumpOut::Destructor 0x00647490 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote159ABody_CTaskSimpleCarJumpOut() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1634"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarSetPedInAsDriverDestructorBody(CTaskSimpleCarSetPedInAsDriver* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleCarSetPedInAsDriver::Destructor 0x00647170 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1635"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarSetPedOutDestructorBody(CTaskSimpleCarSetPedOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp:58](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleCarSetPedOut::Destructor 0x00647950 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1636"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarSetPedSlowDraggedOutDestructorBody(CTaskSimpleCarSetPedSlowDraggedOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarSetPedSlowDraggedOut::Destructor 0x00648430 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote155ABody_CTaskSimpleCarSetPedSlowDraggedOut() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1637"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarSlowDragPedOutDestructorBody(CTaskSimpleCarSlowDragPedOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarSlowDragPedOut::Destructor 0x00648070 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote159ABody_CTaskSimpleCarSlowDragPedOut() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1638"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarWaitForDoorNotToBeInUseDestructorBody(CTaskSimpleCarWaitForDoorNotToBeInUse* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarWaitForDoorNotToBeInUse::Destructor 0x00646C00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleCarWaitForDoorNotToBeInUse() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1639"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarWaitToSlowDownDestructorBody(CTaskSimpleCarWaitToSlowDown* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarWaitToSlowDown::Destructor 0x00646A00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleCarWaitToSlowDown() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1640"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ChokeFinishCallback(CAnimBlendAssociation*, void* data) | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomChokeTime() | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomChokeSpeed(float k, float base) | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

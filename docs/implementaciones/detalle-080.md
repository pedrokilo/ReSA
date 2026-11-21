# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1581"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BeKickedFinishAnimCB(CAnimBlendAssociation* /*anim*/, void* data) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleBeKickedOnGround::FinishAnimCB 0x0061FD10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall BeKickedOnGroundConstructor(CTaskSimpleBeKickedOnGround* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp:72](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleBeKickedOnGround::Constructor 0x0061FC20 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BeKickedOnGroundGetTaskType(void* /*self*/, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp:82](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleBeKickedOnGround::GetTaskType 0x0061FC40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BeKickedOnGroundProcessPed(CTaskSimpleBeKickedOnGround* self, void* /*edx*/, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp:132](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp#L132) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleBeKickedOnGround::ProcessPed 0x0061FE00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1582"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleBikeJackedDestructorBody(CTaskSimpleBikeJacked* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleBikeJacked::Destructor 0x00648C40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskSimpleBikeJacked() | [source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1583"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote152ACallback_CarDriveRollDoor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp:73](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp#L73) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Sin(float angle) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp:186](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp#L186) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1584"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarForcePedOutDestructorBody(CTaskSimpleCarForcePedOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp:90](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp#L90) | C++ · Cuerpo fuente con errores de análisis | Revalidada en migración: CTaskSimpleCarForcePedOut::Destructor 0x00647790 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote159ABody_CTaskSimpleCarForcePedOut() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp:104](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1585"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUseDestructorBody(CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::Destructor 0x00646E50 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1586"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarJumpOutDestructorBody(CTaskSimpleCarJumpOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarJumpOut::Destructor 0x00647490 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote159ABody_CTaskSimpleCarJumpOut() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1587"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarSetPedInAsDriverDestructorBody(CTaskSimpleCarSetPedInAsDriver* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleCarSetPedInAsDriver::Destructor 0x00647170 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1588"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarSetPedOutDestructorBody(CTaskSimpleCarSetPedOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp:58](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleCarSetPedOut::Destructor 0x00647950 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1589"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarSetPedSlowDraggedOutDestructorBody(CTaskSimpleCarSetPedSlowDraggedOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarSetPedSlowDraggedOut::Destructor 0x00648430 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote155ABody_CTaskSimpleCarSetPedSlowDraggedOut() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1590"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarSlowDragPedOutDestructorBody(CTaskSimpleCarSlowDragPedOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarSlowDragPedOut::Destructor 0x00648070 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote159ABody_CTaskSimpleCarSlowDragPedOut() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1591"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarWaitForDoorNotToBeInUseDestructorBody(CTaskSimpleCarWaitForDoorNotToBeInUse* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarWaitForDoorNotToBeInUse::Destructor 0x00646C00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleCarWaitForDoorNotToBeInUse() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1592"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarWaitToSlowDownDestructorBody(CTaskSimpleCarWaitToSlowDown* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarWaitToSlowDown::Destructor 0x00646A00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleCarWaitToSlowDown() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1593"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ChokeFinishCallback(CAnimBlendAssociation*, void* data) | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomChokeTime() | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomChokeSpeed(float k, float base) | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1594"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleClearLookAtDestructorBody(CTaskSimpleClearLookAt* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleClearLookAt::Destructor 0x006346A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskSimpleClearLookAt() | [source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1595"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClimbHeadingOf(float fx, float fy) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:72](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbAddHeading(float add, float fx, float fy) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:77](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbSubHeading(float val, float fx, float fy) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:82](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbHeadingSub(float fx, float fy, float sub) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:87](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbMatTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:92](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbSectorIndex(float v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:101](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbIsFlatNormal(const CVector& n) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:106](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L106) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbNormalFacesPed(const CPed* ped, const CColPoint& cp, CVector& outNormal) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:111](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleClimbDestructorBodyExe(CTaskSimpleClimb* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:131](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L131) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1596"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CowerDestructorBody(CTaskSimpleCower* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCower::Destructor 0x0048E900 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABody_CTaskSimpleCower() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1597"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CreateCarAndGetInDestructorBody(CTaskSimpleCreateCarAndGetIn* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCreateCarAndGetIn::Destructor 0x0064CEA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote171ABody_CTaskSimpleCreateCarAndGetIn() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1598"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeadAsinX87(float x) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp:81](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1599"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDie__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp:190](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1600"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDieInCarDestructorBody(CTaskSimpleDieInCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleDieInCar::Destructor 0x0062FC70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote159ABody_CTaskSimpleDieInCar() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-761"></a>

## CTaskSimpleGoToPointFine

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleGoToPointFine::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleGoToPointFine::CTaskSimpleGoToPointFine(float moveRatio, CVector targetPoint, float fRadius, CEntity* entity) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleGoToPointFine::Constructor 0x0065EEB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CTaskSimpleGoToPointFine::Constructor(float moveRatio, CVector targetPoint, float fRadius, CEntity* entity) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleGoToPointFine::Constructor 0x0065EEB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CTaskSimpleGoToPointFine::Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp:49](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleGoToPointFine::Clone 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleGoToPointFine::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp:54](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleGoToPointFine::MakeAbortable 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleGoToPointFine::Finish(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp:61](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleGoToPointFine::Finish 0x0065EF00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleGoToPointFine::ProcessPed(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp:69](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleGoToPointFine::ProcessPed 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleGoToPointFine::SetBlendedMoveAnim(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp:85](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleGoToPointFine::SetBlendedMoveAnim 0x0065EF80 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleGoToPointFine::SetTargetPos(CVector posn) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp:197](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp#L197) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleGoToPointFine::SetTargetPos 0x0065F330 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleGoToPointFine::SetTargetPos_Reversed(CTaskSimpleGoToPointFine*, void*, CVector) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp:207](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp#L207) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| CTaskSimpleGoToPointFine::BaseRatio(eMoveState moveState) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp:220](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp#L220) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleGoToPointFine::BaseRatio 0x0065EF30 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-762"></a>

## CTaskSimpleGunControl

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleGunControl::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleGunControl::CTaskSimpleGunControl(CEntity* targetEntity, CVector const& targetPos, CVector const& moveTarget, eGunCommand firingTask, int16 burstAmmoCnt, int32 leisureDurMs) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleGunControl::CTaskSimpleGunControl(const CTaskSimpleGunControl& o) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:55](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleGunControl::~CTaskSimpleGunControl() | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:62](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleGunControl::Destructor 0x0061F4D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleGunControl::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:80](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L80) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleGunControl::MakeAbortable 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleGunControl::ProcessPed(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:104](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleGunControl::ProcessPed 0x00000000 | 0 / 0 | Posible dato fijo m_State: source/game_sa/Conversations/PedToPlayerConversations.h:25 (0x00969A20) |
| CTaskSimpleGunControl::ForceStopDuckingMove(CPed* const ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:316](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L316) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleGunControl::GetPedToTarget(const CPed* ped, CVector& out) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:323](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L323) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h:71](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h#L71) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleGunControl::Destructor 0x0061F4D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-763"></a>

## CTaskSimpleHailTaxi

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHailTaxi::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHailTaxi.cpp:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHailTaxi.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-764"></a>

## CTaskSimpleHailTaxiAndPause

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHailTaxiAndPause::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHailTaxiAndPause.cpp:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHailTaxiAndPause.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-765"></a>

## CTaskSimpleHandsUp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHandsUp::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleHandsUp::Destructor 0x0048EA60 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-766"></a>

## CTaskSimpleHitByGunFromFront

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitByGunFromFront::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitByGunFromFront::CTaskSimpleHitByGunFromFront() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHitByGunFromFront::Constructor 0x0062EE70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitByGunFromFront::CTaskSimpleHitByGunFromFront(const CTaskSimpleHitByGunFromFront&) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-767"></a>

## CTaskSimpleHitByGunFromLeft

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitByGunFromLeft::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitByGunFromLeft::CTaskSimpleHitByGunFromLeft() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHitByGunFromLeft::Constructor 0x0062EFD0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitByGunFromLeft::CTaskSimpleHitByGunFromLeft(const CTaskSimpleHitByGunFromLeft&) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-768"></a>

## CTaskSimpleHitByGunFromRear

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitByGunFromRear::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitByGunFromRear::CTaskSimpleHitByGunFromRear() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHitByGunFromRear::Constructor 0x0062EF20 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitByGunFromRear::CTaskSimpleHitByGunFromRear(const CTaskSimpleHitByGunFromRear&) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-769"></a>

## CTaskSimpleHitByGunFromRight

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitByGunFromRight::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitByGunFromRight::CTaskSimpleHitByGunFromRight() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHitByGunFromRight::Constructor 0x0062F080 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitByGunFromRight::CTaskSimpleHitByGunFromRight(const CTaskSimpleHitByGunFromRight&) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-770"></a>

## CTaskSimpleHitFromBack

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitFromBack::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitFromBack::CTaskSimpleHitFromBack() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHitFromBack::Constructor 0x0062EC60 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitFromBack::CTaskSimpleHitFromBack(const CTaskSimpleHitFromBack& o) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-771"></a>

## CTaskSimpleHitFromBehind

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitFromBehind::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitFromBehind::CTaskSimpleHitFromBehind() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHitFromBehind::Constructor 0x00652EE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitFromBehind::CTaskSimpleHitFromBehind(const CTaskSimpleHitFromBehind& o) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-772"></a>

## CTaskSimpleHitFromFront

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitFromFront::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitFromFront::CTaskSimpleHitFromFront() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHitFromFront::Constructor 0x00637250 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitFromFront::CTaskSimpleHitFromFront(const CTaskSimpleHitFromFront& o) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-773"></a>

## CTaskSimpleHitFromLeft

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitFromLeft::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitFromLeft::CTaskSimpleHitFromLeft() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHitFromLeft::Constructor 0x0062ED10 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitFromLeft::CTaskSimpleHitFromLeft(const CTaskSimpleHitFromLeft& o) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-774"></a>

## CTaskSimpleHitFromRight

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitFromRight::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitFromRight::CTaskSimpleHitFromRight() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHitFromRight::Constructor 0x0062EDC0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitFromRight::CTaskSimpleHitFromRight(const CTaskSimpleHitFromRight& o) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-775"></a>

## CTaskSimpleHitHead

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitHead::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CTaskSimpleHitHead::Constructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHitHead::Constructor 0x00653060 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitHead::CTaskSimpleHitHead() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:35](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHitHead::Constructor 0x00653060 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitHead::~CTaskSimpleHitHead() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitHead::FinishAnimCB(CAnimBlendAssociation* anim, void* data) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:48](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHitHead::FinishAnimCB 0x00653150 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitHead::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:57](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHitHead::MakeAbortable 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitHead::ProcessPed(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:78](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHitHead::ProcessPed 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-776"></a>

## CTaskSimpleHitWall

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitWall::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitWall::CTaskSimpleHitWall() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHitWall::Constructor 0x00652F90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHitWall::CTaskSimpleHitWall(const CTaskSimpleHitWall&) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-777"></a>

## CTaskSimpleHoldEntity

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHoldEntity::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:151](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L151) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CTaskSimpleHoldEntity::Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, AnimationId animId, AssocGroupId groupId, bool bDisAllowDroppingOnAnimEnd) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:169](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CTaskSimpleHoldEntity::Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, const char* animName, const char* animBlockName, eAnimationFlags animFlags) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:170](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L170) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CTaskSimpleHoldEntity::Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:171](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L171) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHoldEntity::CTaskSimpleHoldEntity( CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, AnimationId animId, AssocGroupId groupId, bool bDisAllowDroppingOnAnimEnd ) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:174](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L174) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHoldEntity::Constructor-1 0x006913A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(CEntity* entityToHold, const CVector& posn, uint8 boneFrameId, uint8 boneFlags, AnimationId animId, AssocGroupId groupId, bool bDisAllowDroppingOnAnimEnd ) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:205](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L205) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, const char* animName, const char* animBlockName, eAnimationFlags animFlags) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:233](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L233) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHoldEntity::Constructor-2 0x00691470 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:258](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L258) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHoldEntity::Constructor-3 0x00691550 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHoldEntity::~CTaskSimpleHoldEntity() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:284](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L284) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CTaskSimpleHoldEntity::Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:300](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L300) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHoldEntity::Clone 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHoldEntity::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:308](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L308) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHoldEntity::MakeAbortable 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHoldEntity::ProcessPed(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:327](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L327) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHoldEntity::ProcessPed 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHoldEntity::SetPedPosition(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:437](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L437) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHoldEntity::SetPedPosition 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHoldEntity::ReleaseEntity() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:509](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L509) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHoldEntity::ReleaseEntity 0x006916E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHoldEntity::CanThrowEntity() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:514](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L514) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHoldEntity::CanThrowEntity 0x00691700 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHoldEntity::PlayAnim(AnimationId animId, AssocGroupId grpId) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:521](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L521) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHoldEntity::PlayAnim 0x00691720 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHoldEntity::FinishAnimHoldEntityCB(CAnimBlendAssociation* animAssoc, void* data) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:527](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L527) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHoldEntity::FinishAnimHoldEntityCB 0x00691740 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHoldEntity::StartAnim(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:545](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L545) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHoldEntity::StartAnim 0x00692FF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHoldEntity::DropEntity(CPed* ped, bool bAddEventSoundQuiet) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:577](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L577) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHoldEntity::DropEntity 0x006930F0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHoldEntity::ChoosePutDownHeight(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:652](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L652) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHoldEntity::ChoosePutDownHeight 0x00693440 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-778"></a>

## CTaskSimpleHurtPedWithCar

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHurtPedWithCar::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHurtPedWithCar::CTaskSimpleHurtPedWithCar(CVehicle* veh, float impulseMagnitude) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHurtPedWithCar::Constructor 0x00653EA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHurtPedWithCar::CTaskSimpleHurtPedWithCar(const CTaskSimpleHurtPedWithCar& o) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHurtPedWithCar::ProcessPed(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp:49](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleHurtPedWithCar::ProcessPed 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.h#L32) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleHurtPedWithCar::Destructor 0x00653F30 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-779"></a>

## CTaskSimpleIKChain

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKChain::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleIKChain::CTaskSimpleIKChain( const char* name, eBoneTag32 effectorBone, CVector effectorPos, eBoneTag32 pivotBone, CEntity* entity, eBoneTag32 offsetBone, CVector offsetPos, float speed, int32 time, int32 blendTime ) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleIKChain::Constructor 0x006339C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleIKChain::~CTaskSimpleIKChain() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:73](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L73) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleIKChain::Destructor 0x00633A90 | 0 / 0 | Posible dato fijo g_ikChainMan: source/game_sa/Ragdoll/IKChainManager.cpp:29 (0x00C15448) |
| * CTaskSimpleIKChain::Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:80](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L80) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleIKChain::Clone 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleIKChain::BlendOut(int32 blendOutTime) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:92](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleIKChain::BlendOut 0x00633C40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CTaskSimpleIKChain::GetIKChain() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:104](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleIKChain::GetIKChain 0x00633C70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleIKChain::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:109](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleIKChain::MakeAbortable 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleIKChain::ProcessPed(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:118](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L118) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleIKChain::ProcessPed 0x00000000 | 1 / 0 | Posible dato fijo g_ikChainMan: source/game_sa/Ragdoll/IKChainManager.cpp:29 (0x00C15448); ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| CTaskSimpleIKChain::CreateIKChain(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:191](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L191) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleIKChain::CreateIKChain 0x00000000 | 0 / 0 | Posible dato fijo g_ikChainMan: source/game_sa/Ragdoll/IKChainManager.cpp:29 (0x00C15448) |

<a id="grupo-780"></a>

## CTaskSimpleIKLookAt

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKLookAt::InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L23) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleIKLookAt::CTaskSimpleIKLookAt(Const char* name, CEntity* lookAtEntity, int32 time, eBoneTag32 pedBoneID, CVector lookAtOffset, bool useTorso, float speed, int32 blendTime, int8 priority ) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:46](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleIKLookAt::Constructor 0x00633E00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CTaskSimpleIKLookAt::Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:69](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleIKLookAt::Clone 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleIKLookAt::UpdateLookAtInfo(const char* strPurpose, CPed* ped, CEntity* targetPed, int32 time, eBoneTag32 pedBoneID, RwV3d lookAtOffset, bool useTorso, float fSpeed, int32 blendTime, int32 unused) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:83](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleIKLookAt::UpdateLookAtInfo 0x00634050 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CTaskSimpleIKLookAt::GetLookAtEntity() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:106](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L106) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleIKLookAt::GetLookAtEntity 0x00634120 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleIKLookAt::GetLookAtOffset() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:111](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleIKLookAt::GetLookAtOffset 0x00634130 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleIKLookAt::CreateIKChain(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:118](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L118) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleIKLookAt::CreateIKChain 0x00000000 | 0 / 0 | Posible dato fijo g_ikChainMan: source/game_sa/Ragdoll/IKChainManager.cpp:29 (0x00C15448) |

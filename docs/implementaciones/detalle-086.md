# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1701"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FightControlElapsed(float step) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp#L36) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FightControlShouldBlock(float random, float chance, float rate) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp:55](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp#L55) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1702"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleFightingControl | [source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.h:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1703"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleFinishBrain | [source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.h:16](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1704"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleGangDriveBy | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.h:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.h#L31) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.h:65](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.h#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1705"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleGetUp | [source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1706"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleGiveCPR | [source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.h:6](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1707"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleGoTo | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.h:7](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTargetPt() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetTargetPt(CVector pt) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.h:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTargetPtRadius() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.h:35](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetTargetPtRadius(float r) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.h:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1708"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleGoToPoint | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Constructor(eMoveState moveState, const CVector& targetPoint, float fRadius, bool bMoveTowardsTargetPoint, bool a6) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1709"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleGoToPointFine | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetMoveRatio(float value) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1710"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GunControlHeading(float x, float y) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:68](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1711"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleGunControl | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h#L26) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h:53](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h:54](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1712"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHailTaxi.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHailTaxi | [source/game_sa/Tasks/TaskTypes/TaskSimpleHailTaxi.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHailTaxi.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1713"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHailTaxiAndPause.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHailTaxiAndPause | [source/game_sa/Tasks/TaskTypes/TaskSimpleHailTaxiAndPause.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHailTaxiAndPause.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1714"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleRunTimedAnim | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHandsUp(uint32 duration) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHandsUp(const CTaskSimpleHandsUp& o) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h:16](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h:20](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1715"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitByGunFromFront | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1716"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitByGunFromLeft | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1717"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitByGunFromRear | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1718"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitByGunFromRight | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1719"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitFromBack | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1720"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitFromBehind | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

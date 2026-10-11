# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1781"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleSwim | [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h#L25) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h:59](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h:60](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1782"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleThrowControl | [source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1783"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleThrowProjectile | [source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.h:13](../../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.h#L13) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1784"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleTired | [source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(int32 tiredDurationMs) | [source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h#L32) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1785"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleTogglePedThreatScanner | [source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.h:20](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleTogglePedThreatScanner::GetTaskType 0x00492DD0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleTogglePedThreatScanner::Clone 0x00492D50 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1786"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerEvent.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleTriggerEvent | [source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerEvent.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerEvent.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1787"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleTriggerLookAt | [source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.h:13](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.h#L13) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1788"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleTurn180 | [source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.h:27](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.h:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1789"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleUninterruptable | [source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.h#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.h:12](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.h#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.h:13](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.h:14](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessPed(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1790"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleRunAnim | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.h:7](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1791"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleUseGun | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h#L19) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h:27](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLastGunCommand() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h:52](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CEntity* targetEntity, CVector vecTarget, eGunCommand nCommand, uint16 nBurstLength, bool bAimImmediate) | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h:63](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1792"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleWaitForBus | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1793"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleWaitForPizza | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1794"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleWaitUntilAreaCodesMatch | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleWaitUntilAreaCodesMatch(const CTaskSimpleWaitUntilAreaCodesMatch& o) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h:20](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1795"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleWaitUntilLeaderAreaCodesMatch | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h#L9) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleWaitUntilLeaderAreaCodesMatch(const CTaskSimpleWaitUntilLeaderAreaCodesMatch& o) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1796"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleWaitUntilPedIsInCar | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1797"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleWaitUntilPedIsOutCar | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CPed * ped, CVector const& pos) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h#L41) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1798"></a>

## Globales y helpers: source/game_sa/Text/Text.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GxtCharToAsciiOriginal(const GxtChar* src, uint8 start) | [source/game_sa/Text/Text.cpp:87](../../source/game_sa/Text/Text.cpp#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetGxtName() | [source/game_sa/Text/Text.cpp:190](../../source/game_sa/Text/Text.cpp#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:39 (0x00BA6748) |
| CheckFileEncoding(const char* file, uint16 version, uint16 encoding) | [source/game_sa/Text/Text.cpp:214](../../source/game_sa/Text/Text.cpp#L214) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1799"></a>

## Globales y helpers: source/game_sa/TheZones.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Zones13PlayerCoords() | [source/game_sa/TheZones.cpp:341](../../source/game_sa/TheZones.cpp#L341) | Ensamblador · Envoltura al EXE | Histórica del catálogo: CTheZones::SetCurrentZoneAsUnlocked 0x00572800 | 1 / 1 | Trampolín ASM a 0x0056E010; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Zones13Level() | [source/game_sa/TheZones.cpp:345](../../source/game_sa/TheZones.cpp#L345) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00572300; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Zones13Visited() | [source/game_sa/TheZones.cpp:349](../../source/game_sa/TheZones.cpp#L349) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00572130; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Zones13FloatToLong() | [source/game_sa/TheZones.cpp:353](../../source/game_sa/TheZones.cpp#L353) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00821B40; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1800"></a>

## Globales y helpers: source/game_sa/TimeCycle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float value) | [source/game_sa/TimeCycle.cpp:55](../../source/game_sa/TimeCycle.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & TC(A& arr, int32 hour, int32 weather) | [source/game_sa/TimeCycle.cpp:61](../../source/game_sa/TimeCycle.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcDirnLightToSunX87(CVector* out) | [source/game_sa/TimeCycle.cpp:90](../../source/game_sa/TimeCycle.cpp#L90) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| CalcVectorToSunX87(float angle, float* outX, float* outZ) | [source/game_sa/TimeCycle.cpp:112](../../source/game_sa/TimeCycle.cpp#L112) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

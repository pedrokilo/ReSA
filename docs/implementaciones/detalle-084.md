<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1661"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarOpenLockedDoorFromOutside | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.h:11](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.h:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1662"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarSetPedInAsDriver | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.h:13](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.h#L13) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.h:34](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1663"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarSetPedInAsPassenger | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.h#L17) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.h:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.h:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.h#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.h:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleCarSetPedInAsPassenger::Clone 0x00649D90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1664"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarSetPedOut | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.h:12](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.h:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.h:35](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1665"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarSetPedSlowDraggedOut | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.h:20](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1666"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarSetTempAction | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.h:12](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.h:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1667"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarShuffle | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.h:11](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1668"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarSlowBeDraggedOut | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.h:11](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.h:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1669"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarSlowDragPedOut | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.h:11](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.h:34](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.h:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CVehicle* veh, eTargetDoor targetDoor, CTaskUtilityLineUpPedWithCar* lineUpPedWithCarTask, bool isPedStatic) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.h:52](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.h#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.h:53](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.h#L53) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1670"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarWaitForDoorNotToBeInUse | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1671"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarWaitToSlowDown | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.h:7](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CVehicle* veh, SlowDownType type) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.h:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.h#L33) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1672"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleChat.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleRunTimedAnim | [source/game_sa/Tasks/TaskTypes/TaskSimpleChat.h:7](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChat.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleChat.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChat.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleChat.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChat.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1673"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ChokeFinishCallback(CAnimBlendAssociation*, void* data) | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomChokeTime() | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomChokeSpeed(float k, float base) | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1674"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleChoking | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.h:12](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.h:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1675"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleClearLookAt | [source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h:20](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1676"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClimbHeadingOf(float fx, float fy) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:72](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L72) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ClimbAddHeading(float add, float fx, float fy) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:83](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L83) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ClimbSubHeading(float val, float fx, float fy) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:95](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L95) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ClimbHeadingSub(float fx, float fy, float sub) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:107](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L107) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ClimbMatTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:119](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbSectorIndex(float v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:128](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L128) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbIsFlatNormal(const CVector& n) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:133](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbNormalFacesPed(const CPed* ped, const CColPoint& cp, CVector& outNormal) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:138](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L138) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1677"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleClimb | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h#L26) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHeightForPos() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h:62](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetIsInvalidClimb() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h:63](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1678"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleRunAnim | [source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleCower() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1679"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCreateCarAndGetIn | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CVector const& pos, int32 model) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h#L33) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1680"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeadAsinX87(float x) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp:69](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp#L69) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

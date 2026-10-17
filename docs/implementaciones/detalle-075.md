<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1481"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexDrivePointRoute | [source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.h:11](../../source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.h:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.h:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateFirstSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.h:38](../../source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.h#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.h:39](../../source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1482"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexDriveToPoint | [source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.h:13](../../source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.h#L13) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.h:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.h:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexDriveToPoint::GetTaskType 0x0063CE60 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1483"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexCarDriveWander | [source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.h:7](../../source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1484"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexEnterAnyCarAsDriver | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.h:12](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.h#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.h:13](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateNextSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.h:14](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.h:16](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.h:22](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.h#L22) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1485"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexEnterBoatAsDriver | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.h:8](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.h:22](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetTargetCar() const | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.h:25](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1486"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AddLeaderGroupEvent(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:94](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsCarTooFastToEnter(const CVehicle* car, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:103](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fMaxSpeed_CanDragPedOut: source/game_sa/CarEnterExit.h:17 (0x0086F104); Posible dato fijo ms_fMaxSpeed_PlayerCanDragPedOut: source/game_sa/CarEnterExit.h:18 (0x0086F108) |
| OpenTargetCarDoor(CVehicle* car, int32 door) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:113](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L113) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyCruiseSpeed(CVehicle* car, float cruiseSpeed) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:120](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1487"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexEnterCar | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.h:16](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.h#L16) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTargetCar() const | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.h:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetMoveState(eMoveState ms) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.h:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsTargetDoorOnTheLeft() const | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.h:38](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.h#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1488"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexEnterCarAsDriver | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.h:12](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.h:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.h:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexEnterCarAsDriver::GetTaskType 0x00640320 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1489"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexEnterCarAsDriverTimed | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.h:16](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.h#L16) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.h:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.h:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateNextSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.h:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1490"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexEnterCarAsPassenger | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.h:11](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.h:21](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.h:22](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexEnterCarAsPassenger::GetTaskType 0x00640370 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1491"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexEnterCarAsPassengerTimed | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.h:10](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.h:21](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.h:22](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateNextSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.h:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1492"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexEnterCarAsPassengerWait | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.h:10](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.h:22](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.h:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCar() const | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.h:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1493"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexEvasiveCower | [source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexEvasiveCower::GetTaskType 0x006554D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.h:22](../../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1494"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexEvasiveDiveAndGetUp | [source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.h:12](../../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.h:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.h:25](../../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.h:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1495"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexEvasiveStep | [source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.h:6](../../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.h:20](../../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexEvasiveStep::GetTaskType 0x00653340 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.h:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1496"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFireOnFoot.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexExtinguishFireOnFoot | [source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFireOnFoot.h:10](../../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFireOnFoot.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFireOnFoot.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFireOnFoot.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1497"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexExtinguishFires | [source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.h:9](../../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.h:25](../../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.h:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1498"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFacial.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFacial | [source/game_sa/Tasks/TaskTypes/TaskComplexFacial.h:13](../../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.h#L13) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexFacial.h:20](../../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexFacial.h:21](../../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/Tasks/TaskTypes/TaskComplexFacial.h:47](../../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.h#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1499"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFallAndGetUp | [source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1500"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexFallAndStayDown | [source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.h:10](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

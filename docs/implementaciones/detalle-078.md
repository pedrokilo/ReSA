<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1541"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexKillPedOnFootMelee | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.h:10](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.h:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.h:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1542"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexKillPedOnFootStealth | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.h:9](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.h:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.h:25](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1543"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexLeaveAnyCar | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.h:17](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateNextSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.h:21](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(int32 delayTime, bool sensibleLeaveCar, bool forceGetOut) | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.h:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.h:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.h#L28) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1544"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexLeaveBoat | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.h:8](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.h:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.h:25](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1545"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexLeaveCar | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.h:13](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.h#L13) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.h:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.h:39](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1546"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexLeaveCarAndDie | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.h:7](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.h:17](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1547"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexLeaveCarAndFlee | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.h:10](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.h:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.h:37](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.h:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.h#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1548"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexLeaveCarAndWander | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.h:11](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1549"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexLeaveCarAsPassengerWait | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.h:9](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.h:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.h:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1550"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexMedicTreatInjuredPed | [source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.h:7](../../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.h:22](../../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1551"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexMoveBackAndJump | [source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.h:9](../../source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexMoveBackAndJump(const CTaskComplexMoveBackAndJump&) | [source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.h:17](../../source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.h:21](../../source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.h:22](../../source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.h:25](../../source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1552"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexObserveTrafficLights | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.h:17](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexObserveTrafficLights::Destructor 0x006317C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexObserveTrafficLights::GetTaskType 0x006317B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexObserveTrafficLights::Clone 0x00636410 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateFirstSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.h:21](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1553"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexObserveTrafficLightsAndAchieveHeading | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h#L23) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(int32 durationInMs, float fTargetHeading) | [source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h:56](../../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1554"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexOnFire | [source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.h:12](../../source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexOnFire(const CTaskComplexOnFire&) | [source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.h:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.h:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1555"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexOpenDriverDoor | [source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.h:6](../../source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexOpenDriverDoor(CVehicle* vehicle) | [source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.h:12](../../source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.h#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.h:17](../../source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexOpenDriverDoor::GetTaskType 0x006403C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1556"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexOpenPassengerDoor | [source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexOpenPassengerDoor(CVehicle* vehicle, int32 targetSeat) | [source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.h:11](../../source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.h:16](../../source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexOpenPassengerDoor::Destructor 0x00640420 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1557"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartner.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexPartner | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.h:11](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexPartner.h:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1558"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexPartnerChat | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1559"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexPartnerDeal | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.h:15](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.h:16](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1560"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexPartnerGreet | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.h:20](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1521"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGoToPointAnyMeans | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h:9](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(int32 a2, CVector const& posn, float a4, int32 a5) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h:39](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(int32 moveState, CVector const& posn, CVehicle* vehicle, float radius, int32 a6) | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h#L41) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1522"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGoToPointShooting | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.h:7](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1523"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointUntilCollisionAndStandStill.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGoToPointUntilCollisionAndStandStill | [source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointUntilCollisionAndStandStill.h:10](../../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointUntilCollisionAndStandStill.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1524"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexGotoDoorAndOpen | [source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.h:9](../../source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.h:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CObject* door) | [source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.h:38](../../source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.h#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CVector const& start, CVector const& end) | [source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.h:39](../../source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.h:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.h#L40) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1525"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexHitByGunResponse | [source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.h:10](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.h:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.h:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateNextSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.h:25](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.h:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1526"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexHitPedWithCar | [source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.h:8](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.h:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.h:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1527"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexHitResponse | [source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.h:10](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.h:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.h:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateNextSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.h:25](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.h:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1528"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexInAirAndLand | [source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.h:17](../../source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1529"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexInWater.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexInWater | [source/game_sa/Tasks/TaskTypes/TaskComplexInWater.h:8](../../source/game_sa/Tasks/TaskTypes/TaskComplexInWater.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexInWater.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexInWater.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexInWater.h:20](../../source/game_sa/Tasks/TaskTypes/TaskComplexInWater.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexInWater.h:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexInWater.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1530"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexInvestigateDeadPed | [source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.h:11](../../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.h:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.h:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1531"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexInvestigateDisturbance | [source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h:11](../../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h:25](../../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1532"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexJump.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexJump | [source/game_sa/Tasks/TaskTypes/TaskComplexJump.h:11](../../source/game_sa/Tasks/TaskTypes/TaskComplexJump.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexJump.h:25](../../source/game_sa/Tasks/TaskTypes/TaskComplexJump.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexJump.h:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexJump.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1533"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexKillAllThreats | [source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.h:10](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1534"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| NoPedOrNoHp(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:54](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & CriminalField(void* object, size_t offset) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:420](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L420) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CriminalHateEvent(CPed* target, CPed* recipient, eTaskType taskType) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp:423](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp#L423) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1535"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexKillCriminal | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.h:13](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.h#L13) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.h:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.h:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1536"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexKillPedFromBoat | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.h:8](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.h:20](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1537"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexKillPedGroupOnFoot | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.h:12](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.h:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.h:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.h:38](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.h#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1538"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexKillPedOnFoot | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.h:13](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.h#L13) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.h:39](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.h:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.h#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1539"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexKillPedOnFootArmed | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.h:11](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.h:66](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.h#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.h:67](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.h#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1540"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KillMeleeTargetDistance(CPed* ped, const CVector& targetPos) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp:134](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KillMeleeTargetDistanceSq(CPed* ped, const CVector& targetPos) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp:139](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

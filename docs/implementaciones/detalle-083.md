<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1641"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleArrestPed | [source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1642"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BeHitAtan2(float x, float y) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1643"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleBeHit | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CPed* ped, ePedPieceTypes pedPieceType, int32 damageArmor, int32 damageHealth) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h#L38) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1644"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleBeHitWhileMoving | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1645"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleBeKickedOnGround | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1646"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleBikeJacked | [source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.h:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1647"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarAlign | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.h:34](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.h:35](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1648"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarCloseDoorFromInside | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.h:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.h:34](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1649"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarCloseDoorFromOutside | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h:34](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1650"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| X87Sin(float angle) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp:169](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1651"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarDrive | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.h:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.h#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetVehicle() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.h:60](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.h#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CVehicle* pVehicle, CTaskUtilityLineUpPedWithCar* pUtilityTask, int8_t bUpdateCurrentVehicle) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.h:66](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.h#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.h:67](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.h#L67) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1652"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarDriveTimed | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.h:7](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.h:20](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleCarDriveTimed::GetTaskType 0x0046F600 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1653"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarFallOut | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.h:11](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1654"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarForcePedOut | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1655"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarGetIn | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.h:14](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.h#L14) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.h:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.h#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.h:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1656"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarGetOut | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.h:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.h:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1657"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h#L15) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1658"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarJumpOut | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.h:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleCarJumpOut::ComputeAnimID_ToHook 0x00647690 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.h:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.h#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1659"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarOpenDoorFromOutside | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.h:13](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.h#L13) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.h:44](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.h:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.h#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1660"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarOpenLockedDoorFromOutside | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.h:11](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.h:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1701"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleGoToPoint | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Constructor(eMoveState moveState, const CVector& targetPoint, float fRadius, bool bMoveTowardsTargetPoint, bool a6) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1702"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleGoToPointFine | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetMoveRatio(float value) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1703"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GunControlHeading(float x, float y) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:68](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1704"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleGunControl | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h#L26) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h:53](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h:54](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1705"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHailTaxi.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHailTaxi | [source/game_sa/Tasks/TaskTypes/TaskSimpleHailTaxi.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHailTaxi.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1706"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHailTaxiAndPause.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHailTaxiAndPause | [source/game_sa/Tasks/TaskTypes/TaskSimpleHailTaxiAndPause.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHailTaxiAndPause.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1707"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleRunTimedAnim | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHandsUp(uint32 duration) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleHandsUp(const CTaskSimpleHandsUp& o) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h:16](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h:20](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1708"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitByGunFromFront | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1709"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitByGunFromLeft | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1710"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitByGunFromRear | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1711"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitByGunFromRight | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1712"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitFromBack | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1713"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitFromBehind | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1714"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitFromFront | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1715"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitFromLeft | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1716"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitFromRight | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1717"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitHead | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.h:16](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1718"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitWall | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.h:7](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.h:16](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1719"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HoldMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HoldMultiply3x3_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1720"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHoldEntity | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.h#L15) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.h:59](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.h#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHeldEntity() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.h:72](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.h#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

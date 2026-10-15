<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1681"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCreateCarAndGetIn | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CVector const& pos, int32 model) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h#L33) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1682"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeadAsinX87(float x) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp:69](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1683"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDead.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDead | [source/game_sa/Tasks/TaskTypes/TaskSimpleDead.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDead.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleDead::Destructor 0x006305F0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDead.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleDead::Clone 0x00636100 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1684"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDetonate.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDetonate | [source/game_sa/Tasks/TaskTypes/TaskSimpleDetonate.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDetonate.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1685"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDie__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp:153](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp#L153) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1686"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDie.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDie | [source/game_sa/Tasks/TaskTypes/TaskSimpleDie.h:14](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.h#L14) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDie.h:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1687"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDieInCar | [source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.h:14](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleDieInCar::Destructor 0x0062FC70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleDieInCar::Clone 0x00635EF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1688"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDoHandSignal | [source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h:20](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1689"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDrown | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.h:16](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1690"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDrownInCar | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h:12](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h:13](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h:14](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1691"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDuck | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h#L21) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_StartTime: source/game_sa/Conversations/PedToPlayerConversations.h:29 (0x009691B8) |
| GetControlType() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h:56](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h:59](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h:60](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1692"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDuckToggle | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleDuckToggle::Destructor 0x0062F6D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h:27](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1693"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDuckWhileShotsWhizzing | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1694"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleEvasiveDive | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.h:6](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.h:16](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1695"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleEvasiveStep | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.h:6](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.h:20](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1696"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleFacial | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.h#L32) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.h:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.h#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.h:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.h#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetFacialType() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.h:46](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.h#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StartTimer(int32 dur) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.h:47](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.h#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1697"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFall.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleFall | [source/game_sa/Tasks/TaskTypes/TaskSimpleFall.h:6](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleFall.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleFall.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsFinished() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleFall.h:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1698"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TaskLeafCleanupReference() | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L17) | Ensamblador · Envoltura al EXE | Histórica del catálogo: CTaskSimpleFight::ControlFight 0x0061C5E0 | 1 / 1 | Trampolín ASM a 0x00571A00; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| TaskLeafRegisterReference() | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L21) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00571B70; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FightCtor5RegisterRef() | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:159](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L159) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00571B70; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FightDtor12Cleanup() | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:225](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L225) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00571A00; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FightDtor12Callback() | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:229](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L229) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x004CEBC0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FightDtor12ReleaseBlock() | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:233](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L233) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x004D3FD0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FightDtor12Base() | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:237](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L237) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x0061A3A0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1699"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CMeleeInfo | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleFight | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h#L42) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h:68](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h:69](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1700"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleFightingControl | [source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.h:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

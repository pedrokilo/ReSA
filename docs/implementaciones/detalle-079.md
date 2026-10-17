<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1561"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexPartnerShove | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.h:15](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.h:16](../../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1562"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexPassObject | [source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.h:12](../../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.h:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.h:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1563"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexPlayHandSignalAnim | [source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.h:14](../../source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.h#L14) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.h:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1564"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexPolicePursuit | [source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.h:9](../../source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.h:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.h:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.h:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateNextSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.h:39](../../source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1565"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexPresentIDToCop.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexPresentIDToCop | [source/game_sa/Tasks/TaskTypes/TaskComplexPresentIDToCop.h:10](../../source/game_sa/Tasks/TaskTypes/TaskComplexPresentIDToCop.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexPresentIDToCop.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexPresentIDToCop.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1566"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexProstituteSolicit | [source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.h:12](../../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.h:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.h#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.h:46](../../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.h#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexProstituteSolicit::GetTaskType 0x00661AE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1567"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexReactToAttack | [source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.h:10](../../source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1568"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexReactToGunAimedAt | [source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.h:10](../../source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.h:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.h:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1569"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexRoadRage | [source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.h:9](../../source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.h:25](../../source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.h:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1570"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexScreamInCarThenLeave | [source/game_sa/Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.h:17](../../source/game_sa/Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.h#L17) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.h:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.h:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1571"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSeekCoverUntilTargetDead | [source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.h#L19) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.h:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.h:44](../../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1572"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSeekEntityAiming | [source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.h:9](../../source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.h:20](../../source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1573"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexShuffleSeats | [source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.h:13](../../source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.h#L13) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.h:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.h:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1574"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSignalAtPed | [source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.h:7](../../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.h:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.h:25](../../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1575"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSitDownThenIdleThenStandUp | [source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.h:8](../../source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.h:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.h:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1576"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSmartFleeEntity | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.h:9](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.h:46](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.h#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.h:47](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.h#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CEntity* entity_1, bool a3, float a4, int32 a5, int32 a6, float a7) | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.h:56](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.h#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.h:57](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.h#L57) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1577"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSmartFleePoint | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.h:12](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.h:37](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.h:38](../../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.h#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1578"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexStareAtPed | [source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.h:10](../../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.h:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.h:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1579"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexStealCar | [source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.h:11](../../source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.h:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.h:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1580"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexStuckInAir | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.h#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexStuckInAir(const CTaskComplexStuckInAir&) | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.h:12](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.h#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.h:15](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.h:16](../../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

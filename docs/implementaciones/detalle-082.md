<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1621"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWanderProstitute | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetWanderType() override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h:14](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h:15](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexWanderProstitute::GetWanderType 0x006726C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateFirstSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h:16](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexWanderProstitute::Clone 0x00673C80 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1622"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWanderStandard | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h:7](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h:17](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetWanderType() override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexWanderStandard::Clone 0x0048E530 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1623"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GangHassleSeekRadius() | [source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp:65](../../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1624"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskGangHasslePed | [source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h:7](../../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h:25](../../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h:26](../../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CPed* ped, int32 a3, int32 a4, int32 a5) | [source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h:34](../../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h:35](../../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h#L35) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1625"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskGangHassleVehicle | [source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h:11](../../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h:34](../../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h:35](../../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CVehicle* vehicle, int32 a3, uint8 a4, float a5, float a6) | [source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h:46](../../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h:47](../../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h#L47) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1626"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskGoToVehicleAndLean | [source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.h:6](../../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.h:21](../../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.h:22](../../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CVehicle* vehicle, int32 leanAnimDurationInMs) | [source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.h:34](../../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.h:35](../../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.h#L35) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1627"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskLeanOnVehicle | [source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.h:5](../../source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.h:24](../../source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.h:25](../../source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CEntity* vehicle, int32 leanAnimDurationInMs, uint8 a4) | [source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.h:34](../../source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.h:35](../../source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.h#L35) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1628"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleAchieveHeading | [source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.h:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.h:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1629"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleAffectSecondaryBehaviour | [source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.h#L15) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.h:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1630"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleAnim | [source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.h:12](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1631"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleArrestPed__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1632"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleArrestPed | [source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1633"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BeHitAtan2(float x, float y) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1634"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleBeHit | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CPed* ped, ePedPieceTypes pedPieceType, int32 damageArmor, int32 damageHealth) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h#L38) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1635"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleBeHitWhileMoving | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1636"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleBeKickedOnGround | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1637"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleBikeJacked | [source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.h:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1638"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarAlign | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.h:34](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.h:35](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1639"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarCloseDoorFromInside | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.h:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.h:34](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1640"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarCloseDoorFromOutside | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h:34](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

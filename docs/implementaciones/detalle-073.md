<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1441"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSeekEntityAnyMeans | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h:25](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h#L25) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ~CTaskComplexSeekEntityAnyMeans() | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h:54](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h:58](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h:62](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateNextSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h:66](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateFirstSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h:70](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h:77](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StartScanTimer() | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h:90](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h#L90) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetSeekPos(CPed* ped) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h:96](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1442"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSeekEntityAnyMeans&lt;CEntitySeekPosCalculatorXYOffset&gt; | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h:7](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexSeekEntityAnyMeansXYOffset(CEntity* entity, const CVector& offsetXY, int32 period = 1000, float radius = 8.f) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h:11](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h:17](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1443"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorRadiusAngleOffset&gt; | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.h:6](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1444"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorStandard&gt; | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.h:6](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1445"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetMaxEntityDist2DRegs(void* self, uint32 entryEdx, uint32 distanceBits) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp:12](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_snTimeInMilliseconds: source/game_sa/Timer.h:44 (0x00B7CB84) |

<a id="grupo-1446"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorXYOffset&gt; | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.h:6](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1447"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MakeSurePedHasWeaponInHand(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp:138](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp#L138) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1448"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexArrestPed | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h:7](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h:22](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h#L35) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1449"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexAttractorPartnerWait | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.h:7](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.h:17](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateNextSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.h:20](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.h:21](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1450"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexAvoidEntity | [source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.h:11](../../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1451"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexAvoidOtherPedWhileWandering | [source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.h:9](../../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPedToAvoid() const | [source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.h:47](../../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.h#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1452"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWanderCop | [source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexBeCop(eMoveState moveState, uint8 dir, CTask* task) | [source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.h:13](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.h:17](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1453"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexBeInCouple | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.h:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.h:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexBeInCouple::GetTaskType 0x00683770 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1454"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexBeInGroup | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.h:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.h:24](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetGroupID() const | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.h:30](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & GetGroup() const | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.h:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(int32 groupId, bool isLeader) | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.h:37](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.h:38](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.h#L38) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1455"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexBeStill | [source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.h:14](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexBeStill::Destructor 0x005F6700 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.h:15](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexBeStill::GetTaskType 0x005F66D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1456"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexCarDrive | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h:10](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h:20](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateSubTaskCannotGetInCar(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Drive(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor_0(CVehicle* vehicle) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor_1(CVehicle* vehicle, float speed, eModelID carModelIndexToCreate, eCarDrivingStyle carDrivingStyle) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h:36](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h:37](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h#L37) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1457"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexCarDriveMission | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h:9](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h:20](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h:21](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CVehicle* vehicle, CEntity* targetVehicle, eCarMission carDriveMission, eCarDrivingStyle carDrivingStyle, float fSpeed) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h#L29) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexCarDriveMissionFleeScene | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h#L33) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h:39](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexCarDriveMissionFleeScene::GetTaskType 0x004B89C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexCarDriveMissionKillPed | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h:52](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h#L52) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h:58](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h:59](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexCarDriveMissionKillPed::GetTaskType 0x004B8AA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1458"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexCarSlowBeDraggedOut | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h:11](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CVehicle* car, eTargetDoor a3, bool a4) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h:42](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h#L42) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1459"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetAnimGroupOf(CVehicle* veh) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp:58](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1460"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexCarSlowBeDraggedOutAndStandUp | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.h:8](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.h:20](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CVehicle* veh, int32 a3) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.h:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.h:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.h#L32) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

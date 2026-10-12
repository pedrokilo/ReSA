<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1441"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LieInBedTransformPointXY(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp:112](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1442"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskInteriorLieInBed | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.h:11](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.h:34](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.h:35](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1443"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskInteriorShopKeeper | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h:10](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h:20](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h:21](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StartServeAnim() | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h:26](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1444"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SitAtDeskRandom(float scale) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp:78](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitAtDeskMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp:139](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1445"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskInteriorSitAtDesk | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h:13](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h#L13) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h:27](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h:28](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1446"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskInteriorSitInChair | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.h:9](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.h#L9) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.h:18](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1447"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskInteriorUseInfo | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h:10](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h:20](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h:21](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1448"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEntitySeekPosCalculator | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.h:9](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1449"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEntitySeekPosCalculatorFixedPos | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.h:7](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1450"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEntitySeekPosCalculatorRadiusAngleOffset | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.h:6](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1451"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEntitySeekPosCalculatorStandard | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h:5](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1452"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEntitySeekPosCalculatorXYOffset | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h:9](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h#L9) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEntitySeekPosCalculatorXYOffset(const CVector& offset) | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h:16](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetOffset(CVector v) | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h:23](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1453"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSeekEntity | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:46](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L46) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_groups: source/game_sa/PedGroups.h:20 (0x00C09920) |
| InjectHooks() | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:74](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexSeekEntity( CEntity* entity, int32 seekInterval, int32 scanInterval, float maxEntityDist2D, float moveStateRadius, float followNodeThresholdHeightChange, bool bPlayTiredAnim, bool bFaceEntityWhenDone, T_PosCalc seekPosCalculator = {} // NOTSA ) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:95](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexSeekEntity(const CTaskComplexSeekEntity& o) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:119](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ~CTaskComplexSeekEntity() override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:137](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateSubTask(eTaskType type, CPed* ped) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:141](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:189](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L189) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:193](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L193) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:197](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L197) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateNextSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:215](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L215) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateFirstSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:248](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L248) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:277](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L277) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_groups: source/game_sa/PedGroups.h:20 (0x00C09920) |
| GetSeekPos(CPed* ped) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:386](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L386) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetSeekPos2D(CPed* ped) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:392](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L392) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetMoveState(eMoveState ms) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:396](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L396) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetIsTrackingEntity(bool v) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:400](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L400) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & GetSeekPosCalculator() | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:404](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L404) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetEntityMinDist2D(float v) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:410](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L410) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEntityToSeek() const | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:414](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L414) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetMoveStateRadius() const | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:418](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L418) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateSubTaskWhenPedIsTooFarFromEntity(CPed* ped, float pedToSeekPosDist2DSq) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:445](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L445) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1454"></a>

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

<a id="grupo-1455"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSeekEntityAnyMeans&lt;CEntitySeekPosCalculatorXYOffset&gt; | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h:7](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexSeekEntityAnyMeansXYOffset(CEntity* entity, const CVector& offsetXY, int32 period = 1000, float radius = 8.f) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h:11](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h:17](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1456"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorRadiusAngleOffset&gt; | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.h:6](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1457"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorStandard&gt; | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.h:6](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1458"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorXYOffset&gt; | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.h:6](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1459"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MakeSurePedHasWeaponInHand(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp:138](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp#L138) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1460"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexArrestPed | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h:7](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h:22](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h:35](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h#L35) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

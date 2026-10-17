<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1421"></a>

## Globales y helpers: source/game_sa/Tasks/TaskManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeleteTaskAndNull(CTask*& task) | [source/game_sa/Tasks/TaskManager.cpp:58](../../source/game_sa/Tasks/TaskManager.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FindFirstTaskOfType(CTask* task, eTaskType type) | [source/game_sa/Tasks/TaskManager.cpp:64](../../source/game_sa/Tasks/TaskManager.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1422"></a>

## Globales y helpers: source/game_sa/Tasks/TaskSimple.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimple | [source/game_sa/Tasks/TaskSimple.h:13](../../source/game_sa/Tasks/TaskSimple.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetSubTask() const final override | [source/game_sa/Tasks/TaskSimple.h:18](../../source/game_sa/Tasks/TaskSimple.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSimple() const final override | [source/game_sa/Tasks/TaskSimple.h:19](../../source/game_sa/Tasks/TaskSimple.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetPedPosition(CPed* ped) | [source/game_sa/Tasks/TaskSimple.h:37](../../source/game_sa/Tasks/TaskSimple.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1423"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskInteriorBeInHouse | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.h:10](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.h:22](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.h:23](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.h:26](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1424"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskInteriorBeInOffice | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.h:10](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.h:20](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.h:21](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.h:24](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1425"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskInteriorBeInShop | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.h:9](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.h:20](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.h:21](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetWanderType() override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.h:22](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1426"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InteriorInfoHeading(const InteriorInfo_t* info) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp:60](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1427"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskInteriorGoToInfo | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.h:9](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.h:19](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.h:20](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1428"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LieInBedTransformPointXY(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp:112](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1429"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskInteriorLieInBed | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.h:11](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.h:34](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.h:35](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1430"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskInteriorShopKeeper | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h:10](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h:20](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h:21](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StartServeAnim() | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h:26](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1431"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SitAtDeskRandom(float scale) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp:78](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitAtDeskMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp:139](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1432"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskInteriorSitAtDesk | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h:13](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h#L13) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h:27](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h:28](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1433"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskInteriorSitInChair | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.h:9](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.h#L9) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.h:19](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1434"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskInteriorUseInfo | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h:10](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h:20](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h:21](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1435"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEntitySeekPosCalculator | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.h:9](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1436"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEntitySeekPosCalculatorFixedPos | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.h:7](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1437"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEntitySeekPosCalculatorRadiusAngleOffset | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.h:6](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1438"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEntitySeekPosCalculatorStandard | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h:5](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1439"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEntitySeekPosCalculatorXYOffset | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h:9](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h#L9) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEntitySeekPosCalculatorXYOffset(const CVector& offset) | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h:16](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetOffset(CVector v) | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h:23](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1440"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexSeekEntity | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:49](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L49) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_groups: source/game_sa/PedGroups.h:20 (0x00C09920) |
| InjectHooks() | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:77](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexSeekEntity( CEntity* entity, int32 seekInterval, int32 scanInterval, float maxEntityDist2D, float moveStateRadius, float followNodeThresholdHeightChange, bool bPlayTiredAnim, bool bFaceEntityWhenDone, T_PosCalc seekPosCalculator = {} // NOTSA ) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:98](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexSeekEntity(const CTaskComplexSeekEntity& o) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:122](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ~CTaskComplexSeekEntity() override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:140](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L140) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateSubTask(eTaskType type, CPed* ped) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:144](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:192](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L192) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:196](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:200](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L200) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateNextSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:218](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L218) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateFirstSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:251](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L251) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:280](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L280) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_groups: source/game_sa/PedGroups.h:20 (0x00C09920) |
| GetSeekPos(CPed* ped) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:389](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L389) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetSeekPos2D(CPed* ped) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:395](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L395) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetMoveState(eMoveState ms) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:399](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L399) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetIsTrackingEntity(bool v) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:403](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L403) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & GetSeekPosCalculator() | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:407](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L407) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetEntityMinDist2D(float v) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:413](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L413) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEntityToSeek() const | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:417](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L417) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetMoveStateRadius() const | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:421](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L421) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateSubTaskWhenPedIsTooFarFromEntity(CPed* ped, float pedToSeekPosDist2DSq) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h:448](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h#L448) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

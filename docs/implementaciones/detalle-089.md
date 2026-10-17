<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1761"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSay.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleSay | [source/game_sa/Tasks/TaskTypes/TaskSimpleSay.h:11](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSay.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleSay(const CTaskSimpleSay& o) | [source/game_sa/Tasks/TaskTypes/TaskSimpleSay.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSay.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSay.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSay.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSay.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSay.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSay.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSay.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1762"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleScratchHead | [source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.h#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleScratchHead() | [source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.h#L9) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.h:12](../../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.h#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(const CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.h:13](../../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleScratchHead::Clone 0x0048DF60 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1763"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleSetCharDecisionMaker | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1764"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleSetCharIgnoreWeaponRangeFlag | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.h:20](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessPed(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1765"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleSetKindaStayInSamePlace | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.h:20](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessPed(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.h:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1766"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleSetPedAsAutoDriver | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1767"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleSetStayInSamePlace | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleSetStayInSamePlace::GetTaskType 0x0062F5B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(class CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleSetStayInSamePlace::Clone 0x00635BD0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1768"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleShakeFist | [source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.h:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.h:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1769"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleSitDown | [source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.h:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.h:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1770"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleSitIdle | [source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.h:11](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.h:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1771"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleSlideToCoord | [source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(const CVector& pos, float fAimingRotation, float radius) | [source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(const CVector& pos, float aimRot, float radius, const char* animName, const char* fileName, int32 flags, float frameDelta, bool isActiveSequence, int32 time) | [source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h:34](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h:35](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h#L35) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1772"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleStandStill | [source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h:12](../../source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(int32 nTime, bool Looped, bool bUseAnimIdleStance, float fBlendData) | [source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1773"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleStandUp | [source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.h:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1774"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StealthKillAtan2(float y, float x) | [source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp:64](../../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1775"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleStealthKill | [source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.h#L18) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.h:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.h#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.h:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.h#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1776"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WrapSwimAngle(float a) | [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp:96](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1777"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleSwim | [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h#L25) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h:59](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h:60](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1778"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleThrowControl | [source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1779"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleThrowProjectile | [source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.h:13](../../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.h#L13) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1780"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleTired | [source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(int32 tiredDurationMs) | [source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h#L32) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

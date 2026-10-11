# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1681"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ChokeFinishCallback(CAnimBlendAssociation*, void* data) | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomChokeTime() | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomChokeSpeed(float k, float base) | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1682"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleChoking | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.h:12](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.h:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1683"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleClearLookAt | [source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h:20](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1684"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClimbHeadingOf(float fx, float fy) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:72](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L72) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ClimbAddHeading(float add, float fx, float fy) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:83](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L83) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ClimbSubHeading(float val, float fx, float fy) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:95](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L95) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ClimbHeadingSub(float fx, float fy, float sub) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:107](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L107) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ClimbMatTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:119](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbSectorIndex(float v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:128](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L128) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbIsFlatNormal(const CVector& n) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:133](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbNormalFacesPed(const CPed* ped, const CColPoint& cp, CVector& outNormal) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:138](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L138) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1685"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleClimb | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h#L26) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHeightForPos() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h:62](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetIsInvalidClimb() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h:63](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1686"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleRunAnim | [source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleCower() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1687"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCreateCarAndGetIn | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CVector const& pos, int32 model) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h:33](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h#L33) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1688"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeadAsinX87(float x) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp:69](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp#L69) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1689"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDead.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDead | [source/game_sa/Tasks/TaskTypes/TaskSimpleDead.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDead.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleDead::Destructor 0x006305F0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDead.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleDead::Clone 0x00636100 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1690"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDetonate.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDetonate | [source/game_sa/Tasks/TaskTypes/TaskSimpleDetonate.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDetonate.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1691"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDie__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp:153](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp#L153) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1692"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDie.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDie | [source/game_sa/Tasks/TaskTypes/TaskSimpleDie.h:14](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.h#L14) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDie.h:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1693"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDieInCar | [source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.h:14](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleDieInCar::Destructor 0x0062FC70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleDieInCar::Clone 0x00635EF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1694"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDoHandSignal | [source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h:20](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1695"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDrown | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.h:16](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1696"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDrownInCar | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h:12](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h:13](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h:14](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1697"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDuck | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h#L21) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_StartTime: source/game_sa/Conversations/PedToPlayerConversations.h:29 (0x009691B8) |
| GetControlType() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h:56](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h:59](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h:60](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1698"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDuckToggle | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleDuckToggle::Destructor 0x0062F6D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h:27](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1699"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDuckWhileShotsWhizzing | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1700"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleEvasiveDive | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.h:6](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.h:16](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1621"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWaitForDryWeather | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.h:8](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.h:20](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.h:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1622"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TaskCtorRegisterReference() | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp:12](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp#L12) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00571B70; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1623"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWaitForPed | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.h:8](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.h#L8) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1624"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWalkAlongsidePed | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.h:9](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.h:21](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.h:22](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1625"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * NewEmptyPointRoute() | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:55](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddPointUnlessFull(CPointRoute& route, const CVector& pt) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:64](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopyPointRoute(CPointRoute& dst, const CPointRoute& src) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:72](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MulMatrixPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:80](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L80) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1626"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWalkRoundBuildingAttempt | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.h:14](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.h#L14) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.h:49](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.h#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.h:50](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.h#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1627"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWalkRoundCar | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.h:10](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GoingForDoor() const | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.h:22](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.h:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.h:28](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1628"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWalkRoundFire | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.h:11](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.h:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.h:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateNextSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.h:33](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1629"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWalkRoundObject | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.h:8](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.h:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.h:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1630"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWander.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWander | [source/game_sa/Tasks/TaskTypes/TaskComplexWander.h:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexWander.h#L27) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWander.h:54](../../source/game_sa/Tasks/TaskTypes/TaskComplexWander.h#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1631"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWanderCop | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.h:10](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetWanderType() override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.h:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.h:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1632"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWanderCriminal | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.h:6](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.h:17](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexWanderCriminal::Destructor 0x0048E720 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetWanderType() override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexWanderCriminal::Clone 0x0048E650 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1633"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWanderFlee | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.h:9](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.h:17](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetWanderType() override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ScanForStuff(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.h:19](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ControlSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.h:21](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1634"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWanderGang | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.h:6](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetWanderType() override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.h:17](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexWanderGang::GetWanderType 0x0066F630 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanJoinGang() | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.h:21](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexWanderGang::ScanForStuff 0x0066F640 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1635"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWanderMedic | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.h:6](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetWanderType() override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.h:12](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.h#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.h:13](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexWanderMedic::GetWanderType 0x00658810 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ScanForStuff(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.h:14](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexWanderMedic::Clone 0x006587A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1636"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWanderProstitute | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h:5](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetWanderType() override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h:14](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h:15](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexWanderProstitute::GetWanderType 0x006726C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CreateFirstSubTask(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h:16](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexWanderProstitute::Clone 0x00673C80 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1637"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexWanderStandard | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h:7](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h:17](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetWanderType() override | [source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h:18](../../source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskComplexWanderStandard::Clone 0x0048E530 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1638"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GangHassleSeekRadius() | [source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp:65](../../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1639"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskGangHasslePed | [source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h:7](../../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h:25](../../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h:26](../../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CPed* ped, int32 a3, int32 a4, int32 a5) | [source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h:34](../../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h:35](../../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h#L35) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1640"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskGangHassleVehicle | [source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h:11](../../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h:34](../../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h:35](../../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CVehicle* vehicle, int32 a3, uint8 a4, float a5, float a6) | [source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h:46](../../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h:47](../../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h#L47) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

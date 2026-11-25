<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1681"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleRunNamedAnimDestructorBody(CTaskSimpleRunNamedAnim* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleRunNamedAnim::Destructor 0x0061BF10 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleRunNamedAnim() | [source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp:50](../../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1682"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ScratchHeadBaseDestructorCall(CTaskSimpleScratchHead* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleAnim::DestructorBody 0x0061A6F0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ScratchHeadDestructorBody(CTaskSimpleScratchHead* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleScratchHead::Destructor 0x0048E930 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147AScratchHeadBaseDestructorCall() | [source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1683"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetKindaStayInSamePlaceDestructorBodyExe(CTaskSimpleSetKindaStayInSamePlace* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleSetKindaStayInSamePlace::Destructor 0x0062F650 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote183ABody_CTaskSimpleSetKindaStayInSamePlace() | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1684"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetStayInSamePlaceDestructorBody(CTaskSimpleSetStayInSamePlace* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleSetStayInSamePlace::Destructor 0x0062F5D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1685"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StealthKillAtan2(float y, float x) | [source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp:82](../../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote126AStealthKillDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp:226](../../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp#L226) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1686"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleSwimDestructorBody(CTaskSimpleSwim* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote160ABody_CTaskSimpleSwim() | [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp:54](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WrapSwimAngle(float a) | [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp:144](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1687"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TogglePedThreatScannerDestructorBody(CTaskSimpleTogglePedThreatScanner* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleTogglePedThreatScanner::Destructor 0x00492D40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1688"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote157ABody_CTaskSimpleTriggerLookAt() | [source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp:46](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1689"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleTurn180DestructorBody(CTaskSimpleTurn180* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp#L24) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleTurn180::Destructor 0x00631D00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskSimpleTurn180() | [source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1690"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| UninterruptableDestructorBody(CTaskSimpleUninterruptable* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleUninterruptable::DestructorBody 0x0043E3D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1691"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleUseAtmDestructorBody(CTaskSimpleUseAtm* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleUseAtm::Destructor 0x0048E960 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskSimpleUseAtm() | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1692"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleUseGunDestructorBodyExe(CTaskSimpleUseGun* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall CTaskSimpleUseGunConstructorExe(CTaskSimpleUseGun* self, void* /*edx*/, CEntity* targetEntity, uint32 posX, uint32 posY, uint32 posZ, uint32 command, uint32 burstLength, uint32 aimImmediate) | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp:64](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1693"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaitForBusDestructorBody(CTaskSimpleWaitForBus* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleWaitForBus::Destructor 0x00632A90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABody_CTaskSimpleWaitForBus() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1694"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall WaitForPizzaConstructorBody(CTaskSimpleWaitForPizza* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleWaitForPizza::Constructor 0x00632B30 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaitForPizzaDestructorBody(CTaskSimpleWaitForPizza* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleWaitForPizza::DestructorBody 0x00632B70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABody_CTaskSimpleWaitForPizza() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp:62](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1695"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleWaitUntilAreaCodesMatchDestructorBody(CTaskSimpleWaitUntilAreaCodesMatch* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleWaitUntilAreaCodesMatch::Destructor 0x006355E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskSimpleWaitUntilAreaCodesMatch() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1696"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LeaderAreaCodesMatchBaseDestructorCall(CTaskSimpleWaitUntilLeaderAreaCodesMatch* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LeaderAreaCodesMatchDestructorBody(CTaskSimpleWaitUntilLeaderAreaCodesMatch* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleWaitUntilLeaderAreaCodesMatch::Destructor 0x005F67E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ALeaderAreaCodesMatchBaseDestructorCall() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp:35](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote142ALeaderAreaCodesMatchDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1697"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaitUntilPedIsInCarDestructorBody(CTaskSimpleWaitUntilPedIsInCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleWaitUntilPedIsInCar::Destructor 0x006496A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AWaitUntilPedIsInCarDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1698"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaitUntilPedIsOutCarDestructorBody(CTaskSimpleWaitUntilPedIsOutCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleWaitUntilPedIsOutCar::Destructor 0x006494F0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AWaitUntilPedIsOutCarDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1699"></a>

## Globales y helpers: source/game_sa/Text/Text.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GetGxtName() | [source/game_sa/Text/Text.cpp:160](../../source/game_sa/Text/Text.cpp#L160) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:42 (0x00BA6748) |
| CheckFileEncoding(const char* file, uint16 version, uint16 encoding) | [source/game_sa/Text/Text.cpp:184](../../source/game_sa/Text/Text.cpp#L184) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1700"></a>

## Globales y helpers: source/game_sa/TheZones.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote130ATheZonesLockedOrUnlockedAddress() | [source/game_sa/TheZones.cpp:107](../../source/game_sa/TheZones.cpp#L107) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Zones13CellValue(float coord) | [source/game_sa/TheZones.cpp:366](../../source/game_sa/TheZones.cpp#L366) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Zones13LevelResidue(const CVector& pos, uint32 exePos, uint32& ecx, uint32& edx) | [source/game_sa/TheZones.cpp:374](../../source/game_sa/TheZones.cpp#L374) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo MapZoneArray: source/game_sa/TheZones.h:25 (0x00BA1908); Posible dato fijo PlayerInFocus: source/game_sa/World.h:57 (0x00B7CD74); Posible dato fijo Players: source/game_sa/World.h:55 (0x00B7CD98) |

<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/11/2026 (Ciudad de México):** 885 grupos / 10129 funciones reconstruidas; 10129 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1501"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall HandSignalConstructorBody(CTaskSimplePlayHandSignalAnim* self, void* /*edx*/, uint32 animId, uint32 blendBits, uint32 fatHands, uint32 holdLastFrame) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimplePlayHandSignalAnim::Constructor 0x0061AE50 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1502"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimplePlayerOnFireDestructorBody(CTaskSimplePlayerOnFire* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimplePlayerOnFire::Destructor 0x006335B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskSimplePlayerOnFire() | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1503"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimplePlayerOnFootDestructorBody(CTaskSimplePlayerOnFoot* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L43) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimplePlayerOnFoot::Destructor 0x006857D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote160ABody_CTaskSimplePlayerOnFoot() | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:57](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & ActiveWeaponOf(CPlayerPed* player) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:174](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L174) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MultiplyMatrixWithVectorX87(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:180](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L180) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FighterDirection(float angle) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:842](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L842) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FighterProject(CMatrix* matrix, float angle, float magnitude, bool roundTrig) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:850](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L850) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1504"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleRunAnimDestructorBody(CTaskSimpleRunAnim* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleRunAnim() | [source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1505"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleRunNamedAnimDestructorBody(CTaskSimpleRunNamedAnim* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleRunNamedAnim::Destructor 0x0061BF10 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleRunNamedAnim() | [source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp:50](../../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1506"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ScratchHeadBaseDestructorCall(CTaskSimpleScratchHead* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ScratchHeadDestructorBody(CTaskSimpleScratchHead* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleScratchHead::Destructor 0x0048E930 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147AScratchHeadBaseDestructorCall() | [source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1507"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetStayInSamePlaceDestructorBody(CTaskSimpleSetStayInSamePlace* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleSetStayInSamePlace::Destructor 0x0062F5D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1508"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StealthKillAtan2(float y, float x) | [source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp:82](../../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote126AStealthKillDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp:226](../../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp#L226) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1509"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleSwimDestructorBody(CTaskSimpleSwim* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote160ABody_CTaskSimpleSwim() | [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp:53](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WrapSwimAngle(float a) | [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp:136](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp#L136) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1510"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TogglePedThreatScannerDestructorBody(CTaskSimpleTogglePedThreatScanner* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleTogglePedThreatScanner::Destructor 0x00492D40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1511"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote157ABody_CTaskSimpleTriggerLookAt() | [source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1512"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleTurn180DestructorBody(CTaskSimpleTurn180* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp#L24) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleTurn180::Destructor 0x00631D00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskSimpleTurn180() | [source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1513"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| UninterruptableDestructorBody(CTaskSimpleUninterruptable* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleUninterruptable::DestructorBody 0x0043E3D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1514"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleUseAtmDestructorBody(CTaskSimpleUseAtm* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleUseAtm::Destructor 0x0048E960 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskSimpleUseAtm() | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1515"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleUseGunDestructorBodyExe(CTaskSimpleUseGun* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1516"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaitForBusDestructorBody(CTaskSimpleWaitForBus* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleWaitForBus::Destructor 0x00632A90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABody_CTaskSimpleWaitForBus() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1517"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaitForPizzaDestructorBody(CTaskSimpleWaitForPizza* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABody_CTaskSimpleWaitForPizza() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1518"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleWaitUntilAreaCodesMatchDestructorBody(CTaskSimpleWaitUntilAreaCodesMatch* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleWaitUntilAreaCodesMatch::Destructor 0x006355E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskSimpleWaitUntilAreaCodesMatch() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1519"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LeaderAreaCodesMatchBaseDestructorCall(CTaskSimpleWaitUntilLeaderAreaCodesMatch* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ALeaderAreaCodesMatchBaseDestructorCall() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote142ALeaderAreaCodesMatchDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp:35](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1520"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaitUntilPedIsInCarDestructorBody(CTaskSimpleWaitUntilPedIsInCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleWaitUntilPedIsInCar::Destructor 0x006496A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AWaitUntilPedIsInCarDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

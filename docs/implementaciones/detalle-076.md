<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/11/2026 (Ciudad de México):** 884 grupos / 10038 funciones reconstruidas; 10038 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1501"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleSwimDestructorBody(CTaskSimpleSwim* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote160ABody_CTaskSimpleSwim() | [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WrapSwimAngle(float a) | [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp:124](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1502"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TogglePedThreatScannerDestructorBody(CTaskSimpleTogglePedThreatScanner* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleTogglePedThreatScanner::Destructor 0x00492D40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1503"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleTriggerLookAtDestructorBody(CTaskSimpleTriggerLookAt* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp#L26) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00634500: fuera del catálogo consultado; ruta efectiva sin resolver |
| Lote157ABody_CTaskSimpleTriggerLookAt() | [source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1504"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleTurn180DestructorBody(CTaskSimpleTurn180* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp#L24) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleTurn180::Destructor 0x00631D00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskSimpleTurn180() | [source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1505"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| UninterruptableDestructorBody(CTaskSimpleUninterruptable* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleUninterruptable::DestructorBody 0x0043E3D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1506"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleUseAtmDestructorBody(CTaskSimpleUseAtm* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleUseAtm::Destructor 0x0048E960 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskSimpleUseAtm() | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1507"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleUseGunDestructorBodyExe(CTaskSimpleUseGun* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1508"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaitForBusDestructorBody(CTaskSimpleWaitForBus* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleWaitForBus::Destructor 0x00632A90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABody_CTaskSimpleWaitForBus() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1509"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaitForPizzaDestructorBody(CTaskSimpleWaitForPizza* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABody_CTaskSimpleWaitForPizza() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1510"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleWaitUntilAreaCodesMatchDestructorBody(CTaskSimpleWaitUntilAreaCodesMatch* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleWaitUntilAreaCodesMatch::Destructor 0x006355E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskSimpleWaitUntilAreaCodesMatch() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1511"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LeaderAreaCodesMatchBaseDestructorCall(CTaskSimpleWaitUntilLeaderAreaCodesMatch* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ALeaderAreaCodesMatchBaseDestructorCall() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote142ALeaderAreaCodesMatchDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp:35](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1512"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaitUntilPedIsInCarDestructorBody(CTaskSimpleWaitUntilPedIsInCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleWaitUntilPedIsInCar::Destructor 0x006496A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AWaitUntilPedIsInCarDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1513"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaitUntilPedIsOutCarDestructorBody(CTaskSimpleWaitUntilPedIsOutCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleWaitUntilPedIsOutCar::Destructor 0x006494F0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AWaitUntilPedIsOutCarDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1514"></a>

## Globales y helpers: source/game_sa/Text/Text.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GetGxtName() | [source/game_sa/Text/Text.cpp:160](../../source/game_sa/Text/Text.cpp#L160) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748) |
| CheckFileEncoding(const char* file, uint16 version, uint16 encoding) | [source/game_sa/Text/Text.cpp:184](../../source/game_sa/Text/Text.cpp#L184) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1515"></a>

## Globales y helpers: source/game_sa/TheZones.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote130ATheZonesLockedOrUnlockedAddress() | [source/game_sa/TheZones.cpp:109](../../source/game_sa/TheZones.cpp#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Zones13CellValue(float coord) | [source/game_sa/TheZones.cpp:368](../../source/game_sa/TheZones.cpp#L368) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Zones13LevelResidue(const CVector& pos, uint32 exePos, uint32& ecx, uint32& edx) | [source/game_sa/TheZones.cpp:376](../../source/game_sa/TheZones.cpp#L376) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo MapZoneArray: source/game_sa/TheZones.h:23 (0x00BA1908); Posible dato fijo PlayerInFocus: source/game_sa/World.h:56 (0x00B7CD74); Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo TotalNumberOfMapZones: source/game_sa/TheZones.h:22 (0x00BA1900); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |

<a id="grupo-1516"></a>

## Globales y helpers: source/game_sa/TimeCycle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float value) | [source/game_sa/TimeCycle.cpp:56](../../source/game_sa/TimeCycle.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & TC(A& arr, int32 hour, int32 weather) | [source/game_sa/TimeCycle.cpp:62](../../source/game_sa/TimeCycle.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1517"></a>

## Globales y helpers: source/game_sa/Timer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| QueryTimerFrequency(uint32& frequencyLow) | [source/game_sa/Timer.cpp:67](../../source/game_sa/Timer.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1518"></a>

## Globales y helpers: source/game_sa/Timer.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EachFrames(auto count) | [source/game_sa/Timer.h:120](../../source/game_sa/Timer.h#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1519"></a>

## Globales y helpers: source/game_sa/TrafficLights.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| NaviNodeDot(CVehicle* vehicle, const CCarPathLink& link) | [source/game_sa/TrafficLights.cpp:45](../../source/game_sa/TrafficLights.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ShouldCarStopForLightLink(CVehicle* vehicle, CCarPathLinkAddress& info, const CNodeAddress& address, bool bUnkn, int8 side, float range, bool requirePhysics) | [source/game_sa/TrafficLights.cpp:55](../../source/game_sa/TrafficLights.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ThePaths: source/game_sa/PathFind.h:636 (0x0096F050) |
| MatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/TrafficLights.cpp:144](../../source/game_sa/TrafficLights.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1520"></a>

## Globales y helpers: source/game_sa/TxdStore_Plugin.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * TxdParentConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/TxdStore_Plugin.cpp:22](../../source/game_sa/TxdStore_Plugin.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * TxdParentDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/TxdStore_Plugin.cpp:28](../../source/game_sa/TxdStore_Plugin.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * TxdParentCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/TxdStore_Plugin.cpp:33](../../source/game_sa/TxdStore_Plugin.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_txdPluginOffset: source/game_sa/TxdStore.h:22 (0x00C88018) |

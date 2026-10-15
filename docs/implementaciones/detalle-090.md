<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1781"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleUseGun | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h#L19) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h:27](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLastGunCommand() const | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h:52](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CEntity* targetEntity, CVector vecTarget, eGunCommand nCommand, uint16 nBurstLength, bool bAimImmediate) | [source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h:63](../../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1782"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleWaitForBus | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1783"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleWaitForPizza | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1784"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleWaitUntilAreaCodesMatch | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleWaitUntilAreaCodesMatch(const CTaskSimpleWaitUntilAreaCodesMatch& o) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h:20](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1785"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleWaitUntilLeaderAreaCodesMatch | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h#L9) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleWaitUntilLeaderAreaCodesMatch(const CTaskSimpleWaitUntilLeaderAreaCodesMatch& o) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1786"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleWaitUntilPedIsInCar | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1787"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleWaitUntilPedIsOutCar | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CPed * ped, CVector const& pos) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h#L41) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1788"></a>

## Globales y helpers: source/game_sa/Text/Text.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GxtCharToAsciiOriginal(const GxtChar* src, uint8 start) | [source/game_sa/Text/Text.cpp:87](../../source/game_sa/Text/Text.cpp#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetGxtName() | [source/game_sa/Text/Text.cpp:190](../../source/game_sa/Text/Text.cpp#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:39 (0x00BA6748) |
| CheckFileEncoding(const char* file, uint16 version, uint16 encoding) | [source/game_sa/Text/Text.cpp:214](../../source/game_sa/Text/Text.cpp#L214) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1789"></a>

## Globales y helpers: source/game_sa/TheZones.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Zones13PlayerCoords() | [source/game_sa/TheZones.cpp:341](../../source/game_sa/TheZones.cpp#L341) | Ensamblador · Envoltura al EXE | Histórica del catálogo: CTheZones::SetCurrentZoneAsUnlocked 0x00572800 | 1 / 1 | Trampolín ASM a 0x0056E010; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Zones13Level() | [source/game_sa/TheZones.cpp:345](../../source/game_sa/TheZones.cpp#L345) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00572300; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Zones13Visited() | [source/game_sa/TheZones.cpp:349](../../source/game_sa/TheZones.cpp#L349) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00572130; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Zones13FloatToLong() | [source/game_sa/TheZones.cpp:353](../../source/game_sa/TheZones.cpp#L353) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00821B40; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1790"></a>

## Globales y helpers: source/game_sa/TimeCycle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float value) | [source/game_sa/TimeCycle.cpp:56](../../source/game_sa/TimeCycle.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & TC(A& arr, int32 hour, int32 weather) | [source/game_sa/TimeCycle.cpp:62](../../source/game_sa/TimeCycle.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1791"></a>

## Globales y helpers: source/game_sa/Timer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetMillisecondTime() | [source/game_sa/Timer.cpp:53](../../source/game_sa/Timer.cpp#L53) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x005617C0: fuera del catálogo consultado; ruta efectiva sin resolver |
| QueryTimerFrequency(uint32& frequencyLow) | [source/game_sa/Timer.cpp:59](../../source/game_sa/Timer.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1792"></a>

## Globales y helpers: source/game_sa/Timer.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EachFrames(auto count) | [source/game_sa/Timer.h:120](../../source/game_sa/Timer.h#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1793"></a>

## Globales y helpers: source/game_sa/TrafficLights.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| NaviNodeDot(CVehicle* vehicle, const CCarPathLink& link) | [source/game_sa/TrafficLights.cpp:45](../../source/game_sa/TrafficLights.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ShouldCarStopForLightLink(CVehicle* vehicle, CCarPathLinkAddress& info, const CNodeAddress& address, bool bUnkn, int8 side, float range, bool requirePhysics) | [source/game_sa/TrafficLights.cpp:55](../../source/game_sa/TrafficLights.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ThePaths: source/game_sa/PathFind.h:619 (0x0096F050) |
| MatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/TrafficLights.cpp:144](../../source/game_sa/TrafficLights.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1794"></a>

## Globales y helpers: source/game_sa/TxdStore_Plugin.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * TxdParentConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/TxdStore_Plugin.cpp:22](../../source/game_sa/TxdStore_Plugin.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * TxdParentDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/TxdStore_Plugin.cpp:28](../../source/game_sa/TxdStore_Plugin.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * TxdParentCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/TxdStore_Plugin.cpp:33](../../source/game_sa/TxdStore_Plugin.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_txdPluginOffset: source/game_sa/TxdStore.h:22 (0x00C88018) |

<a id="grupo-1795"></a>

## Globales y helpers: source/game_sa/VehicleAnimGroupData.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| sVehicleAnimDataGroupFlags() | [source/game_sa/VehicleAnimGroupData.h:34](../../source/game_sa/VehicleAnimGroupData.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| sVehicleAnimDataSpecialFlags() | [source/game_sa/VehicleAnimGroupData.h:53](../../source/game_sa/VehicleAnimGroupData.h#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1796"></a>

## Globales y helpers: source/game_sa/VehicleRecording.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FindPlaybackIndexOrEnd(CVehicle* vehicle) | [source/game_sa/VehicleRecording.cpp:50](../../source/game_sa/VehicleRecording.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo bPlaybackGoingOn: source/game_sa/VehicleRecording.h:70 (0x0097D6F0); Posible dato fijo pVehicleForPlayback: source/game_sa/VehicleRecording.h:64 (0x0097D840) |
| * FrameAt(CVehicleStateEachFrame* buffer, uint32 byteOffset) | [source/game_sa/VehicleRecording.cpp:60](../../source/game_sa/VehicleRecording.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87UInt(uint32 value) | [source/game_sa/VehicleRecording.cpp:65](../../source/game_sa/VehicleRecording.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1797"></a>

## Globales y helpers: source/game_sa/VisibilityPlugins.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InsertIntoList(CLinkList&lt;CVisibilityPlugins::AlphaObjectInfo&gt;& list, TObject* obj, float dist, CVisibilityPlugins::RenderFunction callback) | [source/game_sa/VisibilityPlugins.cpp:183](../../source/game_sa/VisibilityPlugins.cpp#L183) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1798"></a>

## Globales y helpers: source/game_sa/Wanted.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WantedAsInt(eWantedLevel level) | [source/game_sa/Wanted.cpp:70](../../source/game_sa/Wanted.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WantedDistSqZYX(const CVector& from, const CVector& to) | [source/game_sa/Wanted.cpp:75](../../source/game_sa/Wanted.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1799"></a>

## Globales y helpers: source/game_sa/WaterCreature_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetHeadingX87_6E4(const CPlaceable* placeable) | [source/game_sa/WaterCreature_c.cpp:40](../../source/game_sa/WaterCreature_c.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ForwardTimesSpeed_6E4(const CPlaceable* placeable, float speed) | [source/game_sa/WaterCreature_c.cpp:53](../../source/game_sa/WaterCreature_c.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ForwardTimesStepSpeed_6E4(const CPlaceable* placeable, float step, float speed) | [source/game_sa/WaterCreature_c.cpp:64](../../source/game_sa/WaterCreature_c.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetHeading_6E4(CPlaceable* placeable, float heading) | [source/game_sa/WaterCreature_c.cpp:77](../../source/game_sa/WaterCreature_c.cpp#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomPercent_6E4() | [source/game_sa/WaterCreature_c.cpp:86](../../source/game_sa/WaterCreature_c.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1800"></a>

## Globales y helpers: source/game_sa/WaterLevel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaterInterpolateWave(float base, float alongX, float alongY, float fractionX, float fractionY, float level, bool lower) | [source/game_sa/WaterLevel.cpp:191](../../source/game_sa/WaterLevel.cpp#L191) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| GetWaterLayerParams(int32 layer) | [source/game_sa/WaterLevel.cpp:319](../../source/game_sa/WaterLevel.cpp#L319) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterVertexColor(const CRGBA& c, uint8 alpha) | [source/game_sa/WaterLevel.cpp:326](../../source/game_sa/WaterLevel.cpp#L326) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterBaseUV(int32 x, int32 y, int32 dy, const WaterLayerParams& l, float& baseU, float& baseV) | [source/game_sa/WaterLevel.cpp:331](../../source/game_sa/WaterLevel.cpp#L331) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterSetVertex(int32 idx, float x, float y, float z, uint32 color, float u, float v) | [source/game_sa/WaterLevel.cpp:339](../../source/game_sa/WaterLevel.cpp#L339) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TempBufferVertices: source/game_sa/common.h:287 (0x00C4D958) |
| WaterDebugColor(const CRGBA& c) | [source/game_sa/WaterLevel.cpp:345](../../source/game_sa/WaterLevel.cpp#L345) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterNearestDistance(double x, double y) | [source/game_sa/WaterLevel.cpp:908](../../source/game_sa/WaterLevel.cpp#L908) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| WaterRenderShifts() | [source/game_sa/WaterLevel.cpp:1521](../../source/game_sa/WaterLevel.cpp#L1521) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

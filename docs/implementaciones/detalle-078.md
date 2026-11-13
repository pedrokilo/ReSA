<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 13/11/2026 (Ciudad de México):** 902 grupos / 10558 funciones reconstruidas; 10558 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1541"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LeaderAreaCodesMatchBaseDestructorCall(CTaskSimpleWaitUntilLeaderAreaCodesMatch* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ALeaderAreaCodesMatchBaseDestructorCall() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote142ALeaderAreaCodesMatchDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp:35](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1542"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaitUntilPedIsInCarDestructorBody(CTaskSimpleWaitUntilPedIsInCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleWaitUntilPedIsInCar::Destructor 0x006496A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AWaitUntilPedIsInCarDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1543"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaitUntilPedIsOutCarDestructorBody(CTaskSimpleWaitUntilPedIsOutCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleWaitUntilPedIsOutCar::Destructor 0x006494F0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AWaitUntilPedIsOutCarDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1544"></a>

## Globales y helpers: source/game_sa/Text/Text.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GetGxtName() | [source/game_sa/Text/Text.cpp:160](../../source/game_sa/Text/Text.cpp#L160) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748) |
| CheckFileEncoding(const char* file, uint16 version, uint16 encoding) | [source/game_sa/Text/Text.cpp:184](../../source/game_sa/Text/Text.cpp#L184) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1545"></a>

## Globales y helpers: source/game_sa/TheZones.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote130ATheZonesLockedOrUnlockedAddress() | [source/game_sa/TheZones.cpp:106](../../source/game_sa/TheZones.cpp#L106) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Zones13CellValue(float coord) | [source/game_sa/TheZones.cpp:365](../../source/game_sa/TheZones.cpp#L365) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Zones13LevelResidue(const CVector& pos, uint32 exePos, uint32& ecx, uint32& edx) | [source/game_sa/TheZones.cpp:373](../../source/game_sa/TheZones.cpp#L373) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo MapZoneArray: source/game_sa/TheZones.h:23 (0x00BA1908); Posible dato fijo PlayerInFocus: source/game_sa/World.h:56 (0x00B7CD74); Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo TotalNumberOfMapZones: source/game_sa/TheZones.h:22 (0x00BA1900); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |

<a id="grupo-1546"></a>

## Globales y helpers: source/game_sa/TimeCycle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float value) | [source/game_sa/TimeCycle.cpp:56](../../source/game_sa/TimeCycle.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & TC(A& arr, int32 hour, int32 weather) | [source/game_sa/TimeCycle.cpp:62](../../source/game_sa/TimeCycle.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1547"></a>

## Globales y helpers: source/game_sa/Timer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| QueryTimerFrequency(uint32& frequencyLow) | [source/game_sa/Timer.cpp:67](../../source/game_sa/Timer.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1548"></a>

## Globales y helpers: source/game_sa/Timer.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EachFrames(auto count) | [source/game_sa/Timer.h:120](../../source/game_sa/Timer.h#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1549"></a>

## Globales y helpers: source/game_sa/TrafficLights.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| NaviNodeDot(CVehicle* vehicle, const CCarPathLink& link) | [source/game_sa/TrafficLights.cpp:45](../../source/game_sa/TrafficLights.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ShouldCarStopForLightLink(CVehicle* vehicle, CCarPathLinkAddress& info, const CNodeAddress& address, bool bUnkn, int8 side, float range, bool requirePhysics) | [source/game_sa/TrafficLights.cpp:55](../../source/game_sa/TrafficLights.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ThePaths: source/game_sa/PathFind.h:636 (0x0096F050) |
| MatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/TrafficLights.cpp:144](../../source/game_sa/TrafficLights.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1550"></a>

## Globales y helpers: source/game_sa/TxdStore.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TxdPoolFlushBody(TxdPoolRaw* pool) | [source/game_sa/TxdStore.cpp:43](../../source/game_sa/TxdStore.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TxdPoolFlush(TxdPoolRaw* pool, void* /*edx*/) | [source/game_sa/TxdStore.cpp:57](../../source/game_sa/TxdStore.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTxdStore::PoolFlush 0x00731800 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TxdPoolFlushSecondCopy(TxdPoolRaw* pool, void* /*edx*/) | [source/game_sa/TxdStore.cpp:62](../../source/game_sa/TxdStore.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTxdStore::PoolFlushSecondCopy 0x00731B30 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall TxdPoolScalarDeletingDestructor(TxdPoolRaw* pool, void* /*edx*/, uint8 flags) | [source/game_sa/TxdStore.cpp:68](../../source/game_sa/TxdStore.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTxdStore::PoolScalarDeletingDestructor 0x00731C20 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TxdStoreDefaultSlotExe(int32 index) | [source/game_sa/TxdStore.cpp:120](../../source/game_sa/TxdStore.cpp#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTxdStore::GetDefaultTxdSlot 0x007316D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo defaultTxds: source/game_sa/TxdStore.h:50 (0x00C88004) |

<a id="grupo-1551"></a>

## Globales y helpers: source/game_sa/TxdStore_Plugin.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * TxdParentConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/TxdStore_Plugin.cpp:22](../../source/game_sa/TxdStore_Plugin.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * TxdParentDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/TxdStore_Plugin.cpp:28](../../source/game_sa/TxdStore_Plugin.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * TxdParentCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/TxdStore_Plugin.cpp:33](../../source/game_sa/TxdStore_Plugin.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_txdPluginOffset: source/game_sa/TxdStore.h:22 (0x00C88018) |

<a id="grupo-1552"></a>

## Globales y helpers: source/game_sa/VehicleAnimGroupData.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall VehicleAnimGroupConstructorExe(CVehicleAnimGroup* self, void* /*edx*/) | [source/game_sa/VehicleAnimGroupData.cpp:36](../../source/game_sa/VehicleAnimGroupData.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1553"></a>

## Globales y helpers: source/game_sa/VehicleAnimGroupData.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| sVehicleAnimDataGroupFlags() | [source/game_sa/VehicleAnimGroupData.h:34](../../source/game_sa/VehicleAnimGroupData.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| sVehicleAnimDataSpecialFlags() | [source/game_sa/VehicleAnimGroupData.h:53](../../source/game_sa/VehicleAnimGroupData.h#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1554"></a>

## Globales y helpers: source/game_sa/VehicleRecording.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FindPlaybackIndexOrEnd(CVehicle* vehicle) | [source/game_sa/VehicleRecording.cpp:59](../../source/game_sa/VehicleRecording.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo bPlaybackGoingOn: source/game_sa/VehicleRecording.h:70 (0x0097D6F0); Posible dato fijo pVehicleForPlayback: source/game_sa/VehicleRecording.h:64 (0x0097D840) |
| * FrameAt(CVehicleStateEachFrame* buffer, uint32 byteOffset) | [source/game_sa/VehicleRecording.cpp:69](../../source/game_sa/VehicleRecording.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87UInt(uint32 value) | [source/game_sa/VehicleRecording.cpp:74](../../source/game_sa/VehicleRecording.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1555"></a>

## Globales y helpers: source/game_sa/VehicleScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CVehicleScannerInjectHooksLote109A() | [source/game_sa/VehicleScanner.cpp:24](../../source/game_sa/VehicleScanner.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1556"></a>

## Globales y helpers: source/game_sa/VisibilityPlugins.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InsertIntoList(CLinkList&lt;CVisibilityPlugins::AlphaObjectInfo&gt;& list, TObject* obj, float dist, CVisibilityPlugins::RenderFunction callback) | [source/game_sa/VisibilityPlugins.cpp:239](../../source/game_sa/VisibilityPlugins.cpp#L239) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1557"></a>

## Globales y helpers: source/game_sa/Wanted.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WantedAsInt(eWantedLevel level) | [source/game_sa/Wanted.cpp:72](../../source/game_sa/Wanted.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WantedDistSqZYX(const CVector& from, const CVector& to) | [source/game_sa/Wanted.cpp:77](../../source/game_sa/Wanted.cpp#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1558"></a>

## Globales y helpers: source/game_sa/WaterCreature_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetHeadingX87_6E4(const CPlaceable* placeable) | [source/game_sa/WaterCreature_c.cpp:41](../../source/game_sa/WaterCreature_c.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ForwardTimesSpeed_6E4(const CPlaceable* placeable, float speed) | [source/game_sa/WaterCreature_c.cpp:54](../../source/game_sa/WaterCreature_c.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ForwardTimesStepSpeed_6E4(const CPlaceable* placeable, float step, float speed) | [source/game_sa/WaterCreature_c.cpp:65](../../source/game_sa/WaterCreature_c.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetHeading_6E4(CPlaceable* placeable, float heading) | [source/game_sa/WaterCreature_c.cpp:78](../../source/game_sa/WaterCreature_c.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomPercent_6E4() | [source/game_sa/WaterCreature_c.cpp:87](../../source/game_sa/WaterCreature_c.cpp#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1559"></a>

## Globales y helpers: source/game_sa/WaterLevel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FlushWaterRenderBuffer() | [source/game_sa/WaterLevel.cpp:31](../../source/game_sa/WaterLevel.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo TempBufferVertices: source/game_sa/common.h:289 (0x00C4D958); Posible dato fijo aTempBufferIndices: source/game_sa/common.h:288 (0x00C4B958) |
| lerp(CRenPar rp1, CRenPar rp2, float t) | [source/game_sa/WaterLevel.cpp:60](../../source/game_sa/WaterLevel.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetWaterLayerParams(int32 layer) | [source/game_sa/WaterLevel.cpp:333](../../source/game_sa/WaterLevel.cpp#L333) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterVertexColor(const CRGBA& c, uint8 alpha) | [source/game_sa/WaterLevel.cpp:340](../../source/game_sa/WaterLevel.cpp#L340) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterBaseUV(int32 x, int32 y, int32 dy, const WaterLayerParams& l, float& baseU, float& baseV) | [source/game_sa/WaterLevel.cpp:345](../../source/game_sa/WaterLevel.cpp#L345) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterSetVertex(int32 idx, float x, float y, float z, uint32 color, float u, float v) | [source/game_sa/WaterLevel.cpp:353](../../source/game_sa/WaterLevel.cpp#L353) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TempBufferVertices: source/game_sa/common.h:289 (0x00C4D958) |
| WaterDebugColor(const CRGBA& c) | [source/game_sa/WaterLevel.cpp:359](../../source/game_sa/WaterLevel.cpp#L359) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeWaterWaveSetup(int32 x, int32 y) | [source/game_sa/WaterLevel.cpp:1141](../../source/game_sa/WaterLevel.cpp#L1141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo faWaveMultipliersX: source/game_sa/WaterLevel.h:78 (0x008D38C8); Posible dato fijo faWaveMultipliersY: source/game_sa/WaterLevel.h:79 (0x008D38E8); Posible dato fijo m_nWaterTimeOffset: source/game_sa/WaterLevel.h:73 (0x00C228A4); Posible dato fijo m_snTimeInMilliseconds: source/game_sa/Timer.h:44 (0x00B7CB84) |
| * WaterSinTable() | [source/game_sa/WaterLevel.cpp:1155](../../source/game_sa/WaterLevel.cpp#L1155) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterRenderShifts() | [source/game_sa/WaterLevel.cpp:1288](../../source/game_sa/WaterLevel.cpp#L1288) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Colaborador por dirección 0x00821B1E: fuera del catálogo consultado; ruta efectiva sin resolver; Conversión de dirección fija: 0x008D3824; Posible dato fijo TextureShiftFirstV: source/game_sa/WaterLevel.h:108 (0x00C21178); Posible dato fijo TextureShiftSecondU: source/game_sa/WaterLevel.h:112 (0x00C21184); Posible dato fijo TextureShiftSecondV: source/game_sa/WaterLevel.h:111 (0x00C21180); Posible dato fijo Wavyness: source/game_sa/Weather.h:41 (0x00C812E8); Posible dato fijo m_CurrentFlow: source/game_sa/WaterLevel.h:133 (0x00C22890) |
| & WaterColorR(int32 i) | [source/game_sa/WaterLevel.cpp:1634](../../source/game_sa/WaterLevel.cpp#L1634) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & WaterColorG(int32 i) | [source/game_sa/WaterLevel.cpp:1635](../../source/game_sa/WaterLevel.cpp#L1635) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & WaterColorB(int32 i) | [source/game_sa/WaterLevel.cpp:1636](../../source/game_sa/WaterLevel.cpp#L1636) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & WaterBufferVertex(int32 i) | [source/game_sa/WaterLevel.cpp:1637](../../source/game_sa/WaterLevel.cpp#L1637) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterPackColor(uint32 alpha, uint32 r, uint32 g, uint32 b) | [source/game_sa/WaterLevel.cpp:1639](../../source/game_sa/WaterLevel.cpp#L1639) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & WaterIndexWord(int32 i) | [source/game_sa/WaterLevel.cpp:1640](../../source/game_sa/WaterLevel.cpp#L1640) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * WaterVertexAt(uint16 index) | [source/game_sa/WaterLevel.cpp:2032](../../source/game_sa/WaterLevel.cpp#L2032) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_aVertices: source/game_sa/WaterLevel.h:96 (0x00C22910) |

<a id="grupo-1560"></a>

## Globales y helpers: source/game_sa/Weapon.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AsinDegreesX87(float x) | [source/game_sa/Weapon.cpp:595](../../source/game_sa/Weapon.cpp#L595) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & InstantHitField(void* p, size_t offset) | [source/game_sa/Weapon.cpp:1781](../../source/game_sa/Weapon.cpp#L1781) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InstantHitConstant(uintptr_t address) | [source/game_sa/Weapon.cpp:1782](../../source/game_sa/Weapon.cpp#L1782) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InstantHitAnimationDuration(CWeaponInfo* wi) | [source/game_sa/Weapon.cpp:1783](../../source/game_sa/Weapon.cpp#L1783) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InstantHitSwayAccumulate355(CVector* endpoint,const CVector* sineBasis,const CVector* cosineBasis,float amplitude,float sine,float cosine) | [source/game_sa/Weapon.cpp:1789](../../source/game_sa/Weapon.cpp#L1789) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InstantHitOscillation(uint32 milliseconds, float& sine, float& cosine) | [source/game_sa/Weapon.cpp:1794](../../source/game_sa/Weapon.cpp#L1794) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

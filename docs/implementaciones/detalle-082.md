<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1621"></a>

## Globales y helpers: source/game_sa/TimeCycle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float value) | [source/game_sa/TimeCycle.cpp:56](../../source/game_sa/TimeCycle.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & TC(A& arr, int32 hour, int32 weather) | [source/game_sa/TimeCycle.cpp:62](../../source/game_sa/TimeCycle.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1622"></a>

## Globales y helpers: source/game_sa/Timer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| QueryTimerFrequency(uint32& frequencyLow) | [source/game_sa/Timer.cpp:68](../../source/game_sa/Timer.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1623"></a>

## Globales y helpers: source/game_sa/Timer.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EachFrames(auto count) | [source/game_sa/Timer.h:120](../../source/game_sa/Timer.h#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1624"></a>

## Globales y helpers: source/game_sa/TrafficLights.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| NaviNodeDot(CVehicle* vehicle, const CCarPathLink& link) | [source/game_sa/TrafficLights.cpp:45](../../source/game_sa/TrafficLights.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ShouldCarStopForLightLink(CVehicle* vehicle, CCarPathLinkAddress& info, const CNodeAddress& address, bool bUnkn, int8 side, float range, bool requirePhysics) | [source/game_sa/TrafficLights.cpp:55](../../source/game_sa/TrafficLights.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ThePaths: source/game_sa/PathFind.h:659 (0x0096F050) |
| MatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/TrafficLights.cpp:144](../../source/game_sa/TrafficLights.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1625"></a>

## Globales y helpers: source/game_sa/TxdStore.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TxdPoolFlushBody(TxdPoolRaw* pool) | [source/game_sa/TxdStore.cpp:43](../../source/game_sa/TxdStore.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TxdPoolFlush(TxdPoolRaw* pool, void* /*edx*/) | [source/game_sa/TxdStore.cpp:57](../../source/game_sa/TxdStore.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTxdStore::PoolFlush 0x00731800 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TxdPoolFlushSecondCopy(TxdPoolRaw* pool, void* /*edx*/) | [source/game_sa/TxdStore.cpp:62](../../source/game_sa/TxdStore.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTxdStore::PoolFlushSecondCopy 0x00731B30 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall TxdPoolScalarDeletingDestructor(TxdPoolRaw* pool, void* /*edx*/, uint8 flags) | [source/game_sa/TxdStore.cpp:68](../../source/game_sa/TxdStore.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTxdStore::PoolScalarDeletingDestructor 0x00731C20 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TxdStoreDefaultSlotExe(int32 index) | [source/game_sa/TxdStore.cpp:120](../../source/game_sa/TxdStore.cpp#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTxdStore::GetDefaultTxdSlot 0x007316D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo defaultTxds: source/game_sa/TxdStore.h:50 (0x00C88004) |

<a id="grupo-1626"></a>

## Globales y helpers: source/game_sa/TxdStore_Plugin.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * TxdParentConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/TxdStore_Plugin.cpp:22](../../source/game_sa/TxdStore_Plugin.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * TxdParentDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/TxdStore_Plugin.cpp:28](../../source/game_sa/TxdStore_Plugin.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * TxdParentCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/TxdStore_Plugin.cpp:33](../../source/game_sa/TxdStore_Plugin.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_txdPluginOffset: source/game_sa/TxdStore.h:22 (0x00C88018) |

<a id="grupo-1627"></a>

## Globales y helpers: source/game_sa/VehicleAnimGroupData.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall VehicleAnimGroupConstructorExe(CVehicleAnimGroup* self, void* /*edx*/) | [source/game_sa/VehicleAnimGroupData.cpp:36](../../source/game_sa/VehicleAnimGroupData.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1628"></a>

## Globales y helpers: source/game_sa/VehicleAnimGroupData.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| sVehicleAnimDataGroupFlags() | [source/game_sa/VehicleAnimGroupData.h:34](../../source/game_sa/VehicleAnimGroupData.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| sVehicleAnimDataSpecialFlags() | [source/game_sa/VehicleAnimGroupData.h:53](../../source/game_sa/VehicleAnimGroupData.h#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1629"></a>

## Globales y helpers: source/game_sa/VehicleRecording.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FindPlaybackIndexOrEnd(CVehicle* vehicle) | [source/game_sa/VehicleRecording.cpp:59](../../source/game_sa/VehicleRecording.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo bPlaybackGoingOn: source/game_sa/VehicleRecording.h:70 (0x0097D6F0); Posible dato fijo pVehicleForPlayback: source/game_sa/VehicleRecording.h:64 (0x0097D840) |
| * FrameAt(CVehicleStateEachFrame* buffer, uint32 byteOffset) | [source/game_sa/VehicleRecording.cpp:69](../../source/game_sa/VehicleRecording.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87UInt(uint32 value) | [source/game_sa/VehicleRecording.cpp:74](../../source/game_sa/VehicleRecording.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1630"></a>

## Globales y helpers: source/game_sa/VehicleScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CVehicleScannerInjectHooksLote109A() | [source/game_sa/VehicleScanner.cpp:34](../../source/game_sa/VehicleScanner.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1631"></a>

## Globales y helpers: source/game_sa/VisibilityPlugins.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InsertIntoList(CLinkList&lt;CVisibilityPlugins::AlphaObjectInfo&gt;& list, TObject* obj, float dist, CVisibilityPlugins::RenderFunction callback) | [source/game_sa/VisibilityPlugins.cpp:239](../../source/game_sa/VisibilityPlugins.cpp#L239) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1632"></a>

## Globales y helpers: source/game_sa/Wanted.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WantedAsInt(eWantedLevel level) | [source/game_sa/Wanted.cpp:72](../../source/game_sa/Wanted.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WantedDistSqZYX(const CVector& from, const CVector& to) | [source/game_sa/Wanted.cpp:77](../../source/game_sa/Wanted.cpp#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1633"></a>

## Globales y helpers: source/game_sa/WaterCreature_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetHeadingX87_6E4(const CPlaceable* placeable) | [source/game_sa/WaterCreature_c.cpp:41](../../source/game_sa/WaterCreature_c.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ForwardTimesSpeed_6E4(const CPlaceable* placeable, float speed) | [source/game_sa/WaterCreature_c.cpp:54](../../source/game_sa/WaterCreature_c.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ForwardTimesStepSpeed_6E4(const CPlaceable* placeable, float step, float speed) | [source/game_sa/WaterCreature_c.cpp:65](../../source/game_sa/WaterCreature_c.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetHeading_6E4(CPlaceable* placeable, float heading) | [source/game_sa/WaterCreature_c.cpp:78](../../source/game_sa/WaterCreature_c.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomPercent_6E4() | [source/game_sa/WaterCreature_c.cpp:87](../../source/game_sa/WaterCreature_c.cpp#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1634"></a>

## Globales y helpers: source/game_sa/WaterLevel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FlushWaterRenderBuffer() | [source/game_sa/WaterLevel.cpp:31](../../source/game_sa/WaterLevel.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo TempBufferVertices: source/game_sa/common.h:289 (0x00C4D958); Posible dato fijo aTempBufferIndices: source/game_sa/common.h:288 (0x00C4B958) |
| lerp(CRenPar rp1, CRenPar rp2, float t) | [source/game_sa/WaterLevel.cpp:60](../../source/game_sa/WaterLevel.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetWaterLayerParams(int32 layer) | [source/game_sa/WaterLevel.cpp:357](../../source/game_sa/WaterLevel.cpp#L357) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterVertexColor(const CRGBA& c, uint8 alpha) | [source/game_sa/WaterLevel.cpp:364](../../source/game_sa/WaterLevel.cpp#L364) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterBaseUV(int32 x, int32 y, int32 dy, const WaterLayerParams& l, float& baseU, float& baseV) | [source/game_sa/WaterLevel.cpp:369](../../source/game_sa/WaterLevel.cpp#L369) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterSetVertex(int32 idx, float x, float y, float z, uint32 color, float u, float v) | [source/game_sa/WaterLevel.cpp:377](../../source/game_sa/WaterLevel.cpp#L377) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TempBufferVertices: source/game_sa/common.h:289 (0x00C4D958) |
| WaterDebugColor(const CRGBA& c) | [source/game_sa/WaterLevel.cpp:383](../../source/game_sa/WaterLevel.cpp#L383) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeWaterWaveSetup(int32 x, int32 y) | [source/game_sa/WaterLevel.cpp:1165](../../source/game_sa/WaterLevel.cpp#L1165) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo faWaveMultipliersX: source/game_sa/WaterLevel.h:78 (0x008D38C8); Posible dato fijo faWaveMultipliersY: source/game_sa/WaterLevel.h:79 (0x008D38E8); Posible dato fijo m_nWaterTimeOffset: source/game_sa/WaterLevel.h:73 (0x00C228A4); Posible dato fijo m_snTimeInMilliseconds: source/game_sa/Timer.h:44 (0x00B7CB84) |
| * WaterSinTable() | [source/game_sa/WaterLevel.cpp:1179](../../source/game_sa/WaterLevel.cpp#L1179) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterRenderShifts() | [source/game_sa/WaterLevel.cpp:1313](../../source/game_sa/WaterLevel.cpp#L1313) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Colaborador por dirección 0x00821B1E: fuera del catálogo consultado; ruta efectiva sin resolver; Conversión de dirección fija: 0x008D3824; Posible dato fijo TextureShiftFirstV: source/game_sa/WaterLevel.h:108 (0x00C21178); Posible dato fijo TextureShiftSecondU: source/game_sa/WaterLevel.h:112 (0x00C21184); Posible dato fijo TextureShiftSecondV: source/game_sa/WaterLevel.h:111 (0x00C21180); Posible dato fijo Wavyness: source/game_sa/Weather.h:41 (0x00C812E8); Posible dato fijo m_CurrentFlow: source/game_sa/WaterLevel.h:133 (0x00C22890) |
| & WaterColorR(int32 i) | [source/game_sa/WaterLevel.cpp:1659](../../source/game_sa/WaterLevel.cpp#L1659) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & WaterColorG(int32 i) | [source/game_sa/WaterLevel.cpp:1660](../../source/game_sa/WaterLevel.cpp#L1660) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & WaterColorB(int32 i) | [source/game_sa/WaterLevel.cpp:1661](../../source/game_sa/WaterLevel.cpp#L1661) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & WaterBufferVertex(int32 i) | [source/game_sa/WaterLevel.cpp:1662](../../source/game_sa/WaterLevel.cpp#L1662) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterPackColor(uint32 alpha, uint32 r, uint32 g, uint32 b) | [source/game_sa/WaterLevel.cpp:1664](../../source/game_sa/WaterLevel.cpp#L1664) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & WaterIndexWord(int32 i) | [source/game_sa/WaterLevel.cpp:1665](../../source/game_sa/WaterLevel.cpp#L1665) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * WaterVertexAt(uint16 index) | [source/game_sa/WaterLevel.cpp:2057](../../source/game_sa/WaterLevel.cpp#L2057) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_aVertices: source/game_sa/WaterLevel.h:96 (0x00C22910) |

<a id="grupo-1635"></a>

## Globales y helpers: source/game_sa/Weapon.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AsinDegreesX87(float x) | [source/game_sa/Weapon.cpp:595](../../source/game_sa/Weapon.cpp#L595) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & InstantHitField(void* p, size_t offset) | [source/game_sa/Weapon.cpp:1781](../../source/game_sa/Weapon.cpp#L1781) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InstantHitConstant(uintptr_t address) | [source/game_sa/Weapon.cpp:1782](../../source/game_sa/Weapon.cpp#L1782) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InstantHitAnimationDuration(CWeaponInfo* wi) | [source/game_sa/Weapon.cpp:1783](../../source/game_sa/Weapon.cpp#L1783) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InstantHitSwayAccumulate355(CVector* endpoint,const CVector* sineBasis,const CVector* cosineBasis,float amplitude,float sine,float cosine) | [source/game_sa/Weapon.cpp:1789](../../source/game_sa/Weapon.cpp#L1789) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InstantHitOscillation(uint32 milliseconds, float& sine, float& cosine) | [source/game_sa/Weapon.cpp:1794](../../source/game_sa/Weapon.cpp#L1794) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1636"></a>

## Globales y helpers: source/game_sa/WeaponEffects.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RenderPlayerCrosshair_Hook(uint32* crosshair, void* /*edx*/, int32 playerIndex) | [source/game_sa/WeaponEffects.cpp:344](../../source/game_sa/WeaponEffects.cpp#L344) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CWeaponEffects::RenderPlayerCrosshair 0x0056EF90 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1637"></a>

## Globales y helpers: source/game_sa/WeaponInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FindAnimGroupIndexByName(const char* name, bool& found) | [source/game_sa/WeaponInfo.cpp:208](../../source/game_sa/WeaponInfo.cpp#L208) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1638"></a>

## Globales y helpers: source/game_sa/Weather.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Rand() | [source/game_sa/Weather.cpp:85](../../source/game_sa/Weather.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandMulAddSub(float mul, float add, float sub) | [source/game_sa/Weather.cpp:88](../../source/game_sa/Weather.cpp#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Ftol(float f) | [source/game_sa/Weather.cpp:94](../../source/game_sa/Weather.cpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsRainyType(int16 w) | [source/game_sa/Weather.cpp:96](../../source/game_sa/Weather.cpp#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSunnyType(int16 w) | [source/game_sa/Weather.cpp:97](../../source/game_sa/Weather.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsExtraSunnyType(int16 w) | [source/game_sa/Weather.cpp:101](../../source/game_sa/Weather.cpp#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsHeatHazeType(int16 w) | [source/game_sa/Weather.cpp:105](../../source/game_sa/Weather.cpp#L105) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StepTowards(float& value, float target, float step) | [source/game_sa/Weather.cpp:110](../../source/game_sa/Weather.cpp#L110) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1639"></a>

## Globales y helpers: source/game_sa/WinsockLink.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WinsockLinkBaseDestructorBody(void* self, void* /*edx*/) | [source/game_sa/WinsockLink.cpp:26](../../source/game_sa/WinsockLink.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CWinsockLink::BaseDestructorBody 0x0059CBA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1640"></a>

## Globales y helpers: source/game_sa/World.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WldASectorCoord(float coord) | [source/game_sa/World.cpp:318](../../source/game_sa/World.cpp#L318) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldALodSectorCoord(float coord) | [source/game_sa/World.cpp:322](../../source/game_sa/World.cpp#L322) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & WldASector(int32 x, int32 y) | [source/game_sa/World.cpp:326](../../source/game_sa/World.cpp#L326) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aSectors: source/game_sa/World.h:75 (0x00B7D0B8) |
| & WldARepeatSector(int32 x, int32 y) | [source/game_sa/World.cpp:329](../../source/game_sa/World.cpp#L329) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aRepeatSectors: source/game_sa/World.h:77 (0x00B992B8) |
| WldADistSq(const CVector& a, const CVector& b, bool b2D) | [source/game_sa/World.cpp:333](../../source/game_sa/World.cpp#L333) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldADistKinda(const CVector& a, const CVector& b, bool b2D) | [source/game_sa/World.cpp:349](../../source/game_sa/World.cpp#L349) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldADistNearest(const CVector& a, const CVector& b, bool b2D) | [source/game_sa/World.cpp:365](../../source/game_sa/World.cpp#L365) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldAMultiply3x3VM(const CVector& v, const CMatrix& m) | [source/game_sa/World.cpp:369](../../source/game_sa/World.cpp#L369) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldABoundRadius(CEntity* entity) | [source/game_sa/World.cpp:377](../../source/game_sa/World.cpp#L377) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldAForEachList(int32 x, int32 y, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, Fn&& fn) | [source/game_sa/World.cpp:382](../../source/game_sa/World.cpp#L382) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WorldCopyColPointExe(CColPoint& dst, const CColPoint& src) | [source/game_sa/World.cpp:459](../../source/game_sa/World.cpp#L459) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldD_MatTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/World.cpp:591](../../source/game_sa/World.cpp#L591) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WorldSectorCoordExe(float coord) | [source/game_sa/World.cpp:2052](../../source/game_sa/World.cpp#L2052) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & WorldSectorExe(int32 x, int32 y) | [source/game_sa/World.cpp:2056](../../source/game_sa/World.cpp#L2056) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aSectors: source/game_sa/World.h:75 (0x00B7D0B8) |
| & WorldRepeatSectorExe(int32 x, int32 y) | [source/game_sa/World.cpp:2059](../../source/game_sa/World.cpp#L2059) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aRepeatSectors: source/game_sa/World.h:77 (0x00B992B8) |
| WldEMultiply3x3MV(const CMatrix& m, const CVector& v) | [source/game_sa/World.cpp:2095](../../source/game_sa/World.cpp#L2095) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldEMultiply3x3VM(const CVector& v, const CMatrix& m) | [source/game_sa/World.cpp:2104](../../source/game_sa/World.cpp#L2104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldEMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/World.cpp:2113](../../source/game_sa/World.cpp#L2113) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1901"></a>

## sItemDefinitionFlags

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| sItemDefinitionFlags(uint32 flags) | [source/game_sa/Enums/eItemDefinitionFlags.h:62](../../source/game_sa/Enums/eItemDefinitionFlags.h#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1902"></a>

## sVehAnimGroupGeneralTiming

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| sVehAnimGroupGeneralTiming() | [source/game_sa/VehicleAnimGroupData.h:67](../../source/game_sa/VehicleAnimGroupData.h#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1903"></a>

## sVehAnimGroupInOutTiming

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| operator\[\](eInOutTimingMode mode) | [source/game_sa/VehicleAnimGroupData.h:84](../../source/game_sa/VehicleAnimGroupData.h#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1904"></a>

## scm::ArrayAccess

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetElementTypeOf() | [source/game_sa/Scripts/RunningScript.h:150](../../source/game_sa/Scripts/RunningScript.h#L150) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1905"></a>

## scm::StringRef

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StringRef(char* str, size_t len, size_t cap) | [source/game_sa/Scripts/RunningScript.h:108](../../source/game_sa/Scripts/RunningScript.h#L108) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StringRef(ShortString& str) | [source/game_sa/Scripts/RunningScript.h:116](../../source/game_sa/Scripts/RunningScript.h#L116) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StringRef(LongString& str) | [source/game_sa/Scripts/RunningScript.h:120](../../source/game_sa/Scripts/RunningScript.h#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsNullTerminated() const | [source/game_sa/Scripts/RunningScript.h:131](../../source/game_sa/Scripts/RunningScript.h#L131) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1906"></a>

## tAudioZoneBox

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DrawWireFrame(CRGBA color, const CMatrix& transform) const | [source/game_sa/Audio/AudioZones.h:24](../../source/game_sa/Audio/AudioZones.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1907"></a>

## tBikeHandlingData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tBikeHandlingData::InitFromData(int32 id, const char* line) | [source/game_sa/cHandlingDataMgr.cpp:169](../../source/game_sa/cHandlingDataMgr.cpp#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gHandlingDataMgr: source/game_sa/cHandlingDataMgr.h:274 (0x00C2B9C8) |

<a id="grupo-1908"></a>

## tBinaryIplFile

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetObjInstances() const | [source/game_sa/tBinaryIplFile.h:26](../../source/game_sa/tBinaryIplFile.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCarGens() const | [source/game_sa/tBinaryIplFile.h:27](../../source/game_sa/tBinaryIplFile.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1909"></a>

## tBoatHandlingData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tBoatHandlingData::InitFromData(int32 id, const char* line) | [source/game_sa/cHandlingDataMgr.cpp:107](../../source/game_sa/cHandlingDataMgr.cpp#L107) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1910"></a>

## tBrightLight

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tBrightLight::Render() const | [source/game_sa/BrightLights.cpp:138](../../source/game_sa/BrightLights.cpp#L138) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo BrightTempIndicesStored: source/game_sa/BrightLights.cpp:27 (0x00C4B954); Posible dato fijo BrightTempVerticesStored: source/game_sa/BrightLights.cpp:26 (0x00C4B950); Posible dato fijo TempBufferVertices: source/game_sa/common.h:287 (0x00C4D958); Posible dato fijo aTempBufferIndices: source/game_sa/common.h:286 (0x00C4B958) |

<a id="grupo-1911"></a>

## tBuildingSwap

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tBuildingSwap() | [source/game_sa/Scripts/TheScripts.h:53](../../source/game_sa/Scripts/TheScripts.h#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tBuildingSwap(CBuilding* building, int32 nNewModelIndex, int32 nOldModelIndex) | [source/game_sa/Scripts/TheScripts.h:57](../../source/game_sa/Scripts/TheScripts.h#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Clear() | [source/game_sa/Scripts/TheScripts.h:63](../../source/game_sa/Scripts/TheScripts.h#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1912"></a>

## tColLighting

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tColLighting::GetCurrentLighting(float fScale) const | [source/game_sa/Collision/ColPoint.cpp:8](../../source/game_sa/Collision/ColPoint.cpp#L8) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_fDNBalanceParam: source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.h:24 (0x008D12C0) |
| tColLighting(const uint8 ucLighting) | [source/game_sa/Collision/ColPoint.h:22](../../source/game_sa/Collision/ColPoint.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1913"></a>

## tCompSearchStructById

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tCompSearchStructById(int32 id, RwFrame* frame) | [source/game_sa/Models/ClumpModelInfo.h:28](../../source/game_sa/Models/ClumpModelInfo.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1914"></a>

## tCompSearchStructByName

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tCompSearchStructByName(const char* name, RwFrame* frame) | [source/game_sa/Models/ClumpModelInfo.h:20](../../source/game_sa/Models/ClumpModelInfo.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1915"></a>

## tDirectionArrow

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tDirectionArrow::Render(RpClump* clump) | [source/game_sa/3dMarkers.cpp:749](../../source/game_sa/3dMarkers.cpp#L749) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_user3dMarkerColor: source/game_sa/3dMarkers.h:53 (0x00C7C620) |
| GetColor() const | [source/game_sa/3dMarkers.h:42](../../source/game_sa/3dMarkers.h#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInUse() const | [source/game_sa/3dMarkers.h:44](../../source/game_sa/3dMarkers.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1916"></a>

## tFlyingHandlingData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tFlyingHandlingData::InitFromData(int32 id, const char* line) | [source/game_sa/cHandlingDataMgr.cpp:135](../../source/game_sa/cHandlingDataMgr.cpp#L135) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1917"></a>

## tHandlingData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tHandlingData::InitFromData(int32 id, const char* line) | [source/game_sa/cHandlingDataMgr.cpp:50](../../source/game_sa/cHandlingDataMgr.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gHandlingDataMgr: source/game_sa/cHandlingDataMgr.h:274 (0x00C2B9C8) |
| & GetTransmission() | [source/game_sa/tHandlingData.h:26](../../source/game_sa/tHandlingData.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAnimGroupId() const | [source/game_sa/tHandlingData.h:27](../../source/game_sa/tHandlingData.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1918"></a>

## tHeliLight

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Init() | [source/game_sa/Entity/Vehicle/Heli.h:48](../../source/game_sa/Entity/Vehicle/Heli.h#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1919"></a>

## tMessage

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tMessage( const GxtChar* text, const GxtChar* strToInsert = {}, uint16 flags = {}, uint32 time = {}, bool previousBrief = {}, std::optional&lt;std::array&lt;int32, 6&gt;&gt; numbers = {} ) | [source/game_sa/Messages.h:14](../../source/game_sa/Messages.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ~tMessage() | [source/game_sa/Messages.h:36](../../source/game_sa/Messages.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsValid() const | [source/game_sa/Messages.h:41](../../source/game_sa/Messages.h#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTimeToDisappearAtMS() const | [source/game_sa/Messages.h:44](../../source/game_sa/Messages.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1920"></a>

## tMissionCleanupEntity

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tMissionCleanupEntity() | [source/game_sa/MissionCleanup.h:32](../../source/game_sa/MissionCleanup.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

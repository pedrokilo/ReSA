# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1461"></a>

## tAudioZoneBox

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DrawWireFrame(CRGBA color, const CMatrix& transform) const | [source/game_sa/Audio/AudioZones.h:24](../../source/game_sa/Audio/AudioZones.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1462"></a>

## tBikeHandlingData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tBikeHandlingData::InitFromData(int32 id, const char* line) | [source/game_sa/cHandlingDataMgr.cpp:169](../../source/game_sa/cHandlingDataMgr.cpp#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gHandlingDataMgr: source/game_sa/cHandlingDataMgr.h:274 (0x00C2B9C8) |

<a id="grupo-1463"></a>

## tBinaryIplFile

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetObjInstances() const | [source/game_sa/tBinaryIplFile.h:26](../../source/game_sa/tBinaryIplFile.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCarGens() const | [source/game_sa/tBinaryIplFile.h:27](../../source/game_sa/tBinaryIplFile.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1464"></a>

## tBoatHandlingData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tBoatHandlingData::InitFromData(int32 id, const char* line) | [source/game_sa/cHandlingDataMgr.cpp:107](../../source/game_sa/cHandlingDataMgr.cpp#L107) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1465"></a>

## tBrightLight

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tBrightLight::Render() const | [source/game_sa/BrightLights.cpp:138](../../source/game_sa/BrightLights.cpp#L138) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo BrightTempIndicesStored: source/game_sa/BrightLights.cpp:27 (0x00C4B954); Posible dato fijo BrightTempVerticesStored: source/game_sa/BrightLights.cpp:26 (0x00C4B950); Posible dato fijo TempBufferVertices: source/game_sa/common.h:287 (0x00C4D958); Posible dato fijo aTempBufferIndices: source/game_sa/common.h:286 (0x00C4B958) |

<a id="grupo-1466"></a>

## tBuildingSwap

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tBuildingSwap() | [source/game_sa/Scripts/TheScripts.h:53](../../source/game_sa/Scripts/TheScripts.h#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tBuildingSwap(CBuilding* building, int32 nNewModelIndex, int32 nOldModelIndex) | [source/game_sa/Scripts/TheScripts.h:57](../../source/game_sa/Scripts/TheScripts.h#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Clear() | [source/game_sa/Scripts/TheScripts.h:63](../../source/game_sa/Scripts/TheScripts.h#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1467"></a>

## tColLighting

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tColLighting::GetCurrentLighting(float fScale) const | [source/game_sa/Collision/ColPoint.cpp:8](../../source/game_sa/Collision/ColPoint.cpp#L8) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_fDNBalanceParam: source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.h:24 (0x008D12C0) |
| tColLighting(const uint8 ucLighting) | [source/game_sa/Collision/ColPoint.h:22](../../source/game_sa/Collision/ColPoint.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1468"></a>

## tCompSearchStructById

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tCompSearchStructById(int32 id, RwFrame* frame) | [source/game_sa/Models/ClumpModelInfo.h:28](../../source/game_sa/Models/ClumpModelInfo.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1469"></a>

## tCompSearchStructByName

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tCompSearchStructByName(const char* name, RwFrame* frame) | [source/game_sa/Models/ClumpModelInfo.h:20](../../source/game_sa/Models/ClumpModelInfo.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1470"></a>

## tDirectionArrow

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tDirectionArrow::Render(RpClump* clump) | [source/game_sa/3dMarkers.cpp:749](../../source/game_sa/3dMarkers.cpp#L749) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_user3dMarkerColor: source/game_sa/3dMarkers.h:53 (0x00C7C620) |
| GetColor() const | [source/game_sa/3dMarkers.h:42](../../source/game_sa/3dMarkers.h#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInUse() const | [source/game_sa/3dMarkers.h:44](../../source/game_sa/3dMarkers.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1471"></a>

## tFlyingHandlingData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tFlyingHandlingData::InitFromData(int32 id, const char* line) | [source/game_sa/cHandlingDataMgr.cpp:135](../../source/game_sa/cHandlingDataMgr.cpp#L135) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1472"></a>

## tHandlingData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tHandlingData::InitFromData(int32 id, const char* line) | [source/game_sa/cHandlingDataMgr.cpp:50](../../source/game_sa/cHandlingDataMgr.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gHandlingDataMgr: source/game_sa/cHandlingDataMgr.h:274 (0x00C2B9C8) |
| & GetTransmission() | [source/game_sa/tHandlingData.h:26](../../source/game_sa/tHandlingData.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAnimGroupId() const | [source/game_sa/tHandlingData.h:27](../../source/game_sa/tHandlingData.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1473"></a>

## tHeliLight

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Init() | [source/game_sa/Entity/Vehicle/Heli.h:48](../../source/game_sa/Entity/Vehicle/Heli.h#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1474"></a>

## tMessage

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tMessage( const GxtChar* text, const GxtChar* strToInsert = {}, uint16 flags = {}, uint32 time = {}, bool previousBrief = {}, std::optional&lt;std::array&lt;int32, 6&gt;&gt; numbers = {} ) | [source/game_sa/Messages.h:14](../../source/game_sa/Messages.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ~tMessage() | [source/game_sa/Messages.h:36](../../source/game_sa/Messages.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsValid() const | [source/game_sa/Messages.h:41](../../source/game_sa/Messages.h#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTimeToDisappearAtMS() const | [source/game_sa/Messages.h:44](../../source/game_sa/Messages.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1475"></a>

## tMissionCleanupEntity

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tMissionCleanupEntity() | [source/game_sa/MissionCleanup.h:32](../../source/game_sa/MissionCleanup.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1476"></a>

## tPickupReference

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tPickupReference() | [source/game_sa/Pickups.h:122](../../source/game_sa/Pickups.h#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tPickupReference(int32 value) | [source/game_sa/Pickups.h:125](../../source/game_sa/Pickups.h#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tPickupReference(int16 idx, int16 refIdx) | [source/game_sa/Pickups.h:128](../../source/game_sa/Pickups.h#L128) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tPickupReference(CPickup& pickup) | [source/game_sa/Pickups.h:132](../../source/game_sa/Pickups.h#L132) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo aPickUps: source/game_sa/Pickups.h:38 (0x009788C0) |

<a id="grupo-1477"></a>

## tRadarTrace

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tRadarTrace::GetStaticColour() const | [source/game_sa/Radar.cpp:2795](../../source/game_sa/Radar.cpp#L2795) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tRadarTrace::GetWorldPos() const | [source/game_sa/Radar.cpp:2804](../../source/game_sa/Radar.cpp#L2804) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tRadarTrace::GetRadarAndScreenPos(float* radarPointDist) const | [source/game_sa/Radar.cpp:2817](../../source/game_sa/Radar.cpp#L2817) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HasSprite() const | [source/game_sa/Radar.h:190](../../source/game_sa/Radar.h#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1478"></a>

## tRadioIndexHistory

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Reset() | [source/game_sa/Audio/Managers/AERadioTrackManager.h:99](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PutAtFirst(int32 index) | [source/game_sa/Audio/Managers/AERadioTrackManager.h:103](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1479"></a>

## tRadioSettings

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tRadioSettings(eRadioID currentStation = RADIO_OFF) | [source/game_sa/Audio/Managers/AERadioTrackManager.h:23](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Reset() | [source/game_sa/Audio/Managers/AERadioTrackManager.h:31](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SwitchToNextTrack() | [source/game_sa/Audio/Managers/AERadioTrackManager.h:35](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1480"></a>

## tRadioState

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Reset(bool paused = false) | [source/game_sa/Audio/Managers/AERadioTrackManager.h:77](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

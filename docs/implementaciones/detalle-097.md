# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1921"></a>

## tCompSearchStructById

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tCompSearchStructById(int32 id, RwFrame* frame) | [source/game_sa/Models/ClumpModelInfo.h:28](../../source/game_sa/Models/ClumpModelInfo.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1922"></a>

## tCompSearchStructByName

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tCompSearchStructByName(const char* name, RwFrame* frame) | [source/game_sa/Models/ClumpModelInfo.h:20](../../source/game_sa/Models/ClumpModelInfo.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1923"></a>

## tDirectionArrow

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tDirectionArrow::Render(RpClump* clump) | [source/game_sa/3dMarkers.cpp:769](../../source/game_sa/3dMarkers.cpp#L769) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_user3dMarkerColor: source/game_sa/3dMarkers.h:53 (0x00C7C620) |
| GetColor() const | [source/game_sa/3dMarkers.h:42](../../source/game_sa/3dMarkers.h#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInUse() const | [source/game_sa/3dMarkers.h:44](../../source/game_sa/3dMarkers.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1924"></a>

## tFlyingHandlingData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tFlyingHandlingData::InitFromData(int32 id, const char* line) | [source/game_sa/cHandlingDataMgr.cpp:134](../../source/game_sa/cHandlingDataMgr.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1925"></a>

## tHandlingData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tHandlingData::InitFromData(int32 id, const char* line) | [source/game_sa/cHandlingDataMgr.cpp:49](../../source/game_sa/cHandlingDataMgr.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gHandlingDataMgr: source/game_sa/cHandlingDataMgr.h:274 (0x00C2B9C8) |
| & GetTransmission() | [source/game_sa/tHandlingData.h:26](../../source/game_sa/tHandlingData.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAnimGroupId() const | [source/game_sa/tHandlingData.h:27](../../source/game_sa/tHandlingData.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1926"></a>

## tHeliLight

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Init() | [source/game_sa/Entity/Vehicle/Heli.h:48](../../source/game_sa/Entity/Vehicle/Heli.h#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1927"></a>

## tMessage

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tMessage( const GxtChar* text, const GxtChar* strToInsert = {}, uint16 flags = {}, uint32 time = {}, bool previousBrief = {}, std::optional&lt;std::array&lt;int32, 6&gt;&gt; numbers = {} ) | [source/game_sa/Messages.h:14](../../source/game_sa/Messages.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ~tMessage() | [source/game_sa/Messages.h:36](../../source/game_sa/Messages.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsValid() const | [source/game_sa/Messages.h:41](../../source/game_sa/Messages.h#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTimeToDisappearAtMS() const | [source/game_sa/Messages.h:44](../../source/game_sa/Messages.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1928"></a>

## tMissionCleanupEntity

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tMissionCleanupEntity() | [source/game_sa/MissionCleanup.h:32](../../source/game_sa/MissionCleanup.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1929"></a>

## tPickupReference

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tPickupReference() | [source/game_sa/Pickups.h:121](../../source/game_sa/Pickups.h#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tPickupReference(int32 value) | [source/game_sa/Pickups.h:124](../../source/game_sa/Pickups.h#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tPickupReference(int16 idx, int16 refIdx) | [source/game_sa/Pickups.h:127](../../source/game_sa/Pickups.h#L127) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tPickupReference(CPickup& pickup) | [source/game_sa/Pickups.h:131](../../source/game_sa/Pickups.h#L131) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo aPickUps: source/game_sa/Pickups.h:38 (0x009788C0) |

<a id="grupo-1930"></a>

## tRadarTrace

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tRadarTrace::GetStaticColour() const | [source/game_sa/Radar.cpp:2816](../../source/game_sa/Radar.cpp#L2816) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tRadarTrace::GetWorldPos() const | [source/game_sa/Radar.cpp:2825](../../source/game_sa/Radar.cpp#L2825) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tRadarTrace::GetRadarAndScreenPos(float* radarPointDist) const | [source/game_sa/Radar.cpp:2838](../../source/game_sa/Radar.cpp#L2838) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HasSprite() const | [source/game_sa/Radar.h:190](../../source/game_sa/Radar.h#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1931"></a>

## tRadioIndexHistory

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Reset() | [source/game_sa/Audio/Managers/AERadioTrackManager.h:99](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PutAtFirst(int32 index) | [source/game_sa/Audio/Managers/AERadioTrackManager.h:103](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1932"></a>

## tRadioSettings

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tRadioSettings(eRadioID currentStation = RADIO_OFF) | [source/game_sa/Audio/Managers/AERadioTrackManager.h:23](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Reset() | [source/game_sa/Audio/Managers/AERadioTrackManager.h:31](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SwitchToNextTrack() | [source/game_sa/Audio/Managers/AERadioTrackManager.h:35](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1933"></a>

## tRadioState

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Reset(bool paused = false) | [source/game_sa/Audio/Managers/AERadioTrackManager.h:77](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1934"></a>

## tReplayBikeBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tReplayBikeBlock::MakeBikeUpdateData(CBike& bike, int32 poolIdx) | [source/game_sa/ReplayBlockType.cpp:86](../../source/game_sa/ReplayBlockType.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tReplayBikeBlock::ExtractBikeUpdateData(CBike& bike, float interpolation) | [source/game_sa/ReplayBlockType.cpp:190](../../source/game_sa/ReplayBlockType.cpp#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1935"></a>

## tReplayBlockBase

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * As() | [source/game_sa/ReplayBlockType.h:65](../../source/game_sa/ReplayBlockType.h#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1936"></a>

## tReplayBmxBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tReplayBmxBlock::MakeBmxUpdateData(CBmx& bmx, int32 poolIdx) | [source/game_sa/ReplayBlockType.cpp:97](../../source/game_sa/ReplayBlockType.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tReplayBmxBlock::ExtractBmxUpdateData(CBmx& bmx, float interpolation) | [source/game_sa/ReplayBlockType.cpp:200](../../source/game_sa/ReplayBlockType.cpp#L200) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1937"></a>

## tReplayCameraBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetMatrix() | [source/game_sa/ReplayBlockType.h:169](../../source/game_sa/ReplayBlockType.h#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1938"></a>

## tReplayTrainBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tReplayTrainBlock::MakeTrainUpdateData(CTrain& train, int32 poolIdx) | [source/game_sa/ReplayBlockType.cpp:64](../../source/game_sa/ReplayBlockType.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tReplayTrainBlock::ExtractTrainUpdateData(CTrain& train, float interpolation) | [source/game_sa/ReplayBlockType.cpp:210](../../source/game_sa/ReplayBlockType.cpp#L210) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1939"></a>

## tReplayVehicleBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tReplayVehicleBlock::MakeVehicleUpdateData(CVehicle& veh, int32 poolIdx) | [source/game_sa/ReplayBlockType.cpp:6](../../source/game_sa/ReplayBlockType.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gbFirstPersonRunThisFrame: source/game_sa/Cam.cpp:41 (0x00B6EC20) |
| tReplayVehicleBlock::ExtractVehicleUpdateData(CVehicle& veh, float interpolation) | [source/game_sa/ReplayBlockType.cpp:106](../../source/game_sa/ReplayBlockType.cpp#L106) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo currArea: source/game_sa/Game.h:21 (0x00B72914) |

<a id="grupo-1940"></a>

## tSCMChunkHeader

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * As() | [source/game_sa/Scripts/SCMChunks.hpp:10](../../source/game_sa/Scripts/SCMChunks.hpp#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * As() const | [source/game_sa/Scripts/SCMChunks.hpp:16](../../source/game_sa/Scripts/SCMChunks.hpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

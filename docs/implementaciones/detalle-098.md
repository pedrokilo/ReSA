<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8581 funciones reconstruidas; 8581 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1941"></a>

## tMissionCleanupEntity

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tMissionCleanupEntity() | [source/game_sa/MissionCleanup.h:32](../../source/game_sa/MissionCleanup.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1942"></a>

## tPickupReference

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tPickupReference() | [source/game_sa/Pickups.h:121](../../source/game_sa/Pickups.h#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tPickupReference(int32 value) | [source/game_sa/Pickups.h:124](../../source/game_sa/Pickups.h#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tPickupReference(int16 idx, int16 refIdx) | [source/game_sa/Pickups.h:127](../../source/game_sa/Pickups.h#L127) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tPickupReference(CPickup& pickup) | [source/game_sa/Pickups.h:131](../../source/game_sa/Pickups.h#L131) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo aPickUps: source/game_sa/Pickups.h:38 (0x009788C0) |

<a id="grupo-1943"></a>

## tRadarTrace

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tRadarTrace::GetStaticColour() const | [source/game_sa/Radar.cpp:2496](../../source/game_sa/Radar.cpp#L2496) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tRadarTrace::GetWorldPos() const | [source/game_sa/Radar.cpp:2505](../../source/game_sa/Radar.cpp#L2505) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tRadarTrace::GetRadarAndScreenPos(float* radarPointDist) const | [source/game_sa/Radar.cpp:2518](../../source/game_sa/Radar.cpp#L2518) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HasSprite() const | [source/game_sa/Radar.h:190](../../source/game_sa/Radar.h#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1944"></a>

## tRadioIndexHistory

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Reset() | [source/game_sa/Audio/Managers/AERadioTrackManager.h:99](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PutAtFirst(int32 index) | [source/game_sa/Audio/Managers/AERadioTrackManager.h:103](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1945"></a>

## tRadioSettings

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tRadioSettings(eRadioID currentStation = RADIO_OFF) | [source/game_sa/Audio/Managers/AERadioTrackManager.h:23](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Reset() | [source/game_sa/Audio/Managers/AERadioTrackManager.h:31](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SwitchToNextTrack() | [source/game_sa/Audio/Managers/AERadioTrackManager.h:35](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1946"></a>

## tRadioState

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Reset(bool paused = false) | [source/game_sa/Audio/Managers/AERadioTrackManager.h:77](../../source/game_sa/Audio/Managers/AERadioTrackManager.h#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1947"></a>

## tReplayBikeBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tReplayBikeBlock::MakeBikeUpdateData(CBike& bike, int32 poolIdx) | [source/game_sa/ReplayBlockType.cpp:86](../../source/game_sa/ReplayBlockType.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tReplayBikeBlock::ExtractBikeUpdateData(CBike& bike, float interpolation) | [source/game_sa/ReplayBlockType.cpp:190](../../source/game_sa/ReplayBlockType.cpp#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1948"></a>

## tReplayBlockBase

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * As() | [source/game_sa/ReplayBlockType.h:65](../../source/game_sa/ReplayBlockType.h#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1949"></a>

## tReplayBmxBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tReplayBmxBlock::MakeBmxUpdateData(CBmx& bmx, int32 poolIdx) | [source/game_sa/ReplayBlockType.cpp:97](../../source/game_sa/ReplayBlockType.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tReplayBmxBlock::ExtractBmxUpdateData(CBmx& bmx, float interpolation) | [source/game_sa/ReplayBlockType.cpp:200](../../source/game_sa/ReplayBlockType.cpp#L200) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1950"></a>

## tReplayCameraBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetMatrix() | [source/game_sa/ReplayBlockType.h:169](../../source/game_sa/ReplayBlockType.h#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1951"></a>

## tReplayTrainBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tReplayTrainBlock::MakeTrainUpdateData(CTrain& train, int32 poolIdx) | [source/game_sa/ReplayBlockType.cpp:64](../../source/game_sa/ReplayBlockType.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tReplayTrainBlock::ExtractTrainUpdateData(CTrain& train, float interpolation) | [source/game_sa/ReplayBlockType.cpp:210](../../source/game_sa/ReplayBlockType.cpp#L210) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1952"></a>

## tReplayVehicleBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tReplayVehicleBlock::MakeVehicleUpdateData(CVehicle& veh, int32 poolIdx) | [source/game_sa/ReplayBlockType.cpp:6](../../source/game_sa/ReplayBlockType.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gbFirstPersonRunThisFrame: source/game_sa/Cam.cpp:42 (0x00B6EC20) |
| tReplayVehicleBlock::ExtractVehicleUpdateData(CVehicle& veh, float interpolation) | [source/game_sa/ReplayBlockType.cpp:106](../../source/game_sa/ReplayBlockType.cpp#L106) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo currArea: source/game_sa/Game.h:21 (0x00B72914) |

<a id="grupo-1953"></a>

## tSCMChunkHeader

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * As() | [source/game_sa/Scripts/SCMChunks.hpp:10](../../source/game_sa/Scripts/SCMChunks.hpp#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * As() const | [source/game_sa/Scripts/SCMChunks.hpp:16](../../source/game_sa/Scripts/SCMChunks.hpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1954"></a>

## tSCMGlobalVarChunk

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & ReadVariable(size_t offset) | [source/game_sa/Scripts/SCMChunks.hpp:31](../../source/game_sa/Scripts/SCMChunks.hpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1955"></a>

## tSCMScriptFileInfoChunk

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetMissionOffsets() const | [source/game_sa/Scripts/SCMChunks.hpp:64](../../source/game_sa/Scripts/SCMChunks.hpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1956"></a>

## tSCMStreamedScriptFileInfoChunk

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetStreamedScripts() const | [source/game_sa/Scripts/SCMChunks.hpp:85](../../source/game_sa/Scripts/SCMChunks.hpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1957"></a>

## tSCMUsedObjectsChunk

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetObjectNames() const | [source/game_sa/Scripts/SCMChunks.hpp:46](../../source/game_sa/Scripts/SCMChunks.hpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1958"></a>

## tScanLists

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VisitLists(Visitor&& visitor) | [source/game_sa/Renderer.h:30](../../source/game_sa/Renderer.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1959"></a>

## tScannerSlot

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsActive() const | [source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.h:10](../../source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1960"></a>

## tScriptAttachedAnimGroup

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tScriptAttachedAnimGroup() | [source/game_sa/Scripts/TheScripts.h:201](../../source/game_sa/Scripts/TheScripts.h#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

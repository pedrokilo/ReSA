# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1941"></a>

## tReplayBmxBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tReplayBmxBlock::MakeBmxUpdateData(CBmx& bmx, int32 poolIdx) | [source/game_sa/ReplayBlockType.cpp:97](../../source/game_sa/ReplayBlockType.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tReplayBmxBlock::ExtractBmxUpdateData(CBmx& bmx, float interpolation) | [source/game_sa/ReplayBlockType.cpp:200](../../source/game_sa/ReplayBlockType.cpp#L200) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1942"></a>

## tReplayCameraBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetMatrix() | [source/game_sa/ReplayBlockType.h:169](../../source/game_sa/ReplayBlockType.h#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1943"></a>

## tReplayTrainBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tReplayTrainBlock::MakeTrainUpdateData(CTrain& train, int32 poolIdx) | [source/game_sa/ReplayBlockType.cpp:64](../../source/game_sa/ReplayBlockType.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tReplayTrainBlock::ExtractTrainUpdateData(CTrain& train, float interpolation) | [source/game_sa/ReplayBlockType.cpp:210](../../source/game_sa/ReplayBlockType.cpp#L210) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1944"></a>

## tReplayVehicleBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tReplayVehicleBlock::MakeVehicleUpdateData(CVehicle& veh, int32 poolIdx) | [source/game_sa/ReplayBlockType.cpp:6](../../source/game_sa/ReplayBlockType.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gbFirstPersonRunThisFrame: source/game_sa/Cam.cpp:41 (0x00B6EC20) |
| tReplayVehicleBlock::ExtractVehicleUpdateData(CVehicle& veh, float interpolation) | [source/game_sa/ReplayBlockType.cpp:106](../../source/game_sa/ReplayBlockType.cpp#L106) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo currArea: source/game_sa/Game.h:21 (0x00B72914) |

<a id="grupo-1945"></a>

## tSCMChunkHeader

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * As() | [source/game_sa/Scripts/SCMChunks.hpp:10](../../source/game_sa/Scripts/SCMChunks.hpp#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * As() const | [source/game_sa/Scripts/SCMChunks.hpp:16](../../source/game_sa/Scripts/SCMChunks.hpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1946"></a>

## tSCMGlobalVarChunk

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & ReadVariable(size_t offset) | [source/game_sa/Scripts/SCMChunks.hpp:31](../../source/game_sa/Scripts/SCMChunks.hpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1947"></a>

## tSCMScriptFileInfoChunk

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetMissionOffsets() const | [source/game_sa/Scripts/SCMChunks.hpp:64](../../source/game_sa/Scripts/SCMChunks.hpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1948"></a>

## tSCMStreamedScriptFileInfoChunk

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetStreamedScripts() const | [source/game_sa/Scripts/SCMChunks.hpp:85](../../source/game_sa/Scripts/SCMChunks.hpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1949"></a>

## tSCMUsedObjectsChunk

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetObjectNames() const | [source/game_sa/Scripts/SCMChunks.hpp:46](../../source/game_sa/Scripts/SCMChunks.hpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1950"></a>

## tScanLists

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VisitLists(Visitor&& visitor) | [source/game_sa/Renderer.h:30](../../source/game_sa/Renderer.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1951"></a>

## tScannerSlot

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsActive() const | [source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.h:10](../../source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1952"></a>

## tScriptAttachedAnimGroup

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tScriptAttachedAnimGroup() | [source/game_sa/Scripts/TheScripts.h:201](../../source/game_sa/Scripts/TheScripts.h#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1953"></a>

## tScriptBrainWaitEntity

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Clear() | [source/game_sa/Scripts/TheScripts.h:294](../../source/game_sa/Scripts/TheScripts.h#L294) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1954"></a>

## tScriptCheckpoint

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tScriptCheckpoint() | [source/game_sa/Scripts/TheScripts.h:82](../../source/game_sa/Scripts/TheScripts.h#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetId() const | [source/game_sa/Scripts/TheScripts.h:89](../../source/game_sa/Scripts/TheScripts.h#L89) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsActive() const | [source/game_sa/Scripts/TheScripts.h:92](../../source/game_sa/Scripts/TheScripts.h#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1955"></a>

## tScriptConnectLodsObject

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tScriptConnectLodsObject() | [source/game_sa/Scripts/TheScripts.h:305](../../source/game_sa/Scripts/TheScripts.h#L305) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1956"></a>

## tScriptEffectSystem

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tScriptEffectSystem() | [source/game_sa/Scripts/TheScripts.h:101](../../source/game_sa/Scripts/TheScripts.h#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetId() const | [source/game_sa/Scripts/TheScripts.h:108](../../source/game_sa/Scripts/TheScripts.h#L108) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsActive() const | [source/game_sa/Scripts/TheScripts.h:111](../../source/game_sa/Scripts/TheScripts.h#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1957"></a>

## tScriptRectangle

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tScriptRectangle() | [source/game_sa/Scripts/TheScripts.h:183](../../source/game_sa/Scripts/TheScripts.h#L183) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1958"></a>

## tScriptResource

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tScriptResource() | [source/game_sa/ScriptResourceManager.h:23](../../source/game_sa/ScriptResourceManager.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1959"></a>

## tScriptSearchlight

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetId() | [source/game_sa/Scripts/TheScripts.h:239](../../source/game_sa/Scripts/TheScripts.h#L239) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsActive() const | [source/game_sa/Scripts/TheScripts.h:242](../../source/game_sa/Scripts/TheScripts.h#L242) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1960"></a>

## tScriptSequence

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tScriptSequence() | [source/game_sa/Scripts/TheScripts.h:119](../../source/game_sa/Scripts/TheScripts.h#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetId() const | [source/game_sa/Scripts/TheScripts.h:125](../../source/game_sa/Scripts/TheScripts.h#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsActive() const | [source/game_sa/Scripts/TheScripts.h:128](../../source/game_sa/Scripts/TheScripts.h#L128) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

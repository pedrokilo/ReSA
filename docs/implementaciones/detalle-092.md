<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 19/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1821"></a>

## tReplayBlockBase

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * As() | [source/game_sa/ReplayBlockType.h:65](../../source/game_sa/ReplayBlockType.h#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1822"></a>

## tReplayBmxBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tReplayBmxBlock::ExtractBmxUpdateData(CBmx& bmx, float interpolation) | [source/game_sa/ReplayBlockType.cpp:121](../../source/game_sa/ReplayBlockType.cpp#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CReplay::ExtractBmxBlock 0x0045BFF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1823"></a>

## tReplayCameraBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetMatrix() | [source/game_sa/ReplayBlockType.h:167](../../source/game_sa/ReplayBlockType.h#L167) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1824"></a>

## tReplayTrainBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tReplayTrainBlock::ExtractTrainUpdateData(CTrain& train, float interpolation) | [source/game_sa/ReplayBlockType.cpp:129](../../source/game_sa/ReplayBlockType.cpp#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: tReplayTrainBlock::ExtractTrainUpdateData 0x0045CDB0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1825"></a>

## tReplayVehicleBlock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tReplayVehicleBlock::ExtractVehicleUpdateData(CVehicle& veh, float interpolation) | [source/game_sa/ReplayBlockType.cpp:28](../../source/game_sa/ReplayBlockType.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo currArea: source/game_sa/Game.h:21 (0x00B72914) |

<a id="grupo-1826"></a>

## tSCMChunkHeader

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * As() | [source/game_sa/Scripts/SCMChunks.hpp:10](../../source/game_sa/Scripts/SCMChunks.hpp#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * As() const | [source/game_sa/Scripts/SCMChunks.hpp:16](../../source/game_sa/Scripts/SCMChunks.hpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1827"></a>

## tSCMGlobalVarChunk

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & ReadVariable(size_t offset) | [source/game_sa/Scripts/SCMChunks.hpp:31](../../source/game_sa/Scripts/SCMChunks.hpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1828"></a>

## tSCMScriptFileInfoChunk

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetMissionOffsets() const | [source/game_sa/Scripts/SCMChunks.hpp:64](../../source/game_sa/Scripts/SCMChunks.hpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1829"></a>

## tSCMStreamedScriptFileInfoChunk

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetStreamedScripts() const | [source/game_sa/Scripts/SCMChunks.hpp:85](../../source/game_sa/Scripts/SCMChunks.hpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1830"></a>

## tSCMUsedObjectsChunk

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetObjectNames() const | [source/game_sa/Scripts/SCMChunks.hpp:46](../../source/game_sa/Scripts/SCMChunks.hpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1831"></a>

## tScanLists

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VisitLists(Visitor&& visitor) | [source/game_sa/Renderer.h:30](../../source/game_sa/Renderer.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1832"></a>

## tScannerSlot

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsActive() const | [source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.h:10](../../source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1833"></a>

## tScriptAttachedAnimGroup

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tScriptAttachedAnimGroup() | [source/game_sa/Scripts/TheScripts.h:201](../../source/game_sa/Scripts/TheScripts.h#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1834"></a>

## tScriptBrainWaitEntity

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Clear() | [source/game_sa/Scripts/TheScripts.h:294](../../source/game_sa/Scripts/TheScripts.h#L294) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1835"></a>

## tScriptCheckpoint

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tScriptCheckpoint() | [source/game_sa/Scripts/TheScripts.h:82](../../source/game_sa/Scripts/TheScripts.h#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetId() const | [source/game_sa/Scripts/TheScripts.h:89](../../source/game_sa/Scripts/TheScripts.h#L89) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsActive() const | [source/game_sa/Scripts/TheScripts.h:92](../../source/game_sa/Scripts/TheScripts.h#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1836"></a>

## tScriptConnectLodsObject

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tScriptConnectLodsObject() | [source/game_sa/Scripts/TheScripts.h:305](../../source/game_sa/Scripts/TheScripts.h#L305) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1837"></a>

## tScriptEffectSystem

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tScriptEffectSystem() | [source/game_sa/Scripts/TheScripts.h:101](../../source/game_sa/Scripts/TheScripts.h#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetId() const | [source/game_sa/Scripts/TheScripts.h:108](../../source/game_sa/Scripts/TheScripts.h#L108) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsActive() const | [source/game_sa/Scripts/TheScripts.h:111](../../source/game_sa/Scripts/TheScripts.h#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1838"></a>

## tScriptRectangle

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tScriptRectangle() | [source/game_sa/Scripts/TheScripts.h:183](../../source/game_sa/Scripts/TheScripts.h#L183) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1839"></a>

## tScriptResource

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tScriptResource() | [source/game_sa/ScriptResourceManager.h:23](../../source/game_sa/ScriptResourceManager.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1840"></a>

## tScriptSearchlight

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetId() | [source/game_sa/Scripts/TheScripts.h:239](../../source/game_sa/Scripts/TheScripts.h#L239) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsActive() const | [source/game_sa/Scripts/TheScripts.h:242](../../source/game_sa/Scripts/TheScripts.h#L242) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

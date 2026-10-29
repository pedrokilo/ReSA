<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1441"></a>

## notsa::script::commands::zone

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::zone::RegisterHandlers() | [source/game_sa/Scripts/Commands/ZoneCommands.cpp:120](../../source/game_sa/Scripts/Commands/ZoneCommands.cpp#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1442"></a>

## notsa::script::detail

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProcessCommandMemFnWrapper(CRunningScript* S) | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:29](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CollectArgsAndCall(CRunningScript* S, eScriptCommands command, T_FnRet(*CommandFn)(T_FnArgs...), T_CollectedArgs&&... args) | [source/game_sa/Scripts/CommandParser/Parser.hpp:18](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NotImplemented(CRunningScript& S, eScriptCommands cmd) | [source/game_sa/Scripts/CommandParser/Parser.hpp:68](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CommandParser(CRunningScript* S) | [source/game_sa/Scripts/CommandParser/Parser.hpp:75](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddCommandHandler() | [source/game_sa/Scripts/CommandParser/Parser.hpp:81](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| safe_arithmetic_cast(F value) | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:84](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1443"></a>

## notsa::script::detail::StrongAlias

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & operator=(const T& o) | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:24](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1444"></a>

## notsa::script::detail::scriptthing

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetAt(uint32 idx) -&gt; tScriptSphere& | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:46](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ScriptSphereArray: source/game_sa/Scripts/TheScripts.h:403 (0x00A91268) |
| GetAt(uint32 idx) -&gt; tScriptEffectSystem& | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:48](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ScriptEffectSystemArray: source/game_sa/Scripts/TheScripts.h:404 (0x00A44110) |
| GetAt(uint32 idx) -&gt; tScriptSearchlight& | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:50](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ScriptSearchLightArray: source/game_sa/Scripts/TheScripts.h:405 (0x00A94D68) |
| GetAt(uint32 idx) -&gt; tScriptSequence& | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:52](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ScriptSequenceTaskArray: source/game_sa/Scripts/TheScripts.h:407 (0x00A43F68) |
| GetAt(uint32 idx) -&gt; tScriptCheckpoint& | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:54](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ScriptCheckpointArray: source/game_sa/Scripts/TheScripts.h:410 (0x00A44070) |
| GetAt(uint32 idx) -&gt; T& | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:56](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_effects: source/game_sa/Scripts/Scripted2dEffects.h:51 (0x00C3AB00) |
| GetId(uint32 idx) -&gt; uint32 | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:64](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetId(uint32 idx) -&gt; uint32 | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:66](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ScriptReferenceIndex: source/game_sa/DecisionMakers/DecisionMakerTypes.h:45 (0x00C0AFF4), source/game_sa/PedGroups.h:16 (0x00C098D0), source/game_sa/Scripts/Scripted2dEffects.h:55 (0x00C3A120) |
| IsActive(uint32 idx) -&gt; bool | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:70](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsActive(uint32 idx) -&gt; bool | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:72](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_activated: source/game_sa/Scripts/Scripted2dEffects.h:54 (0x00C3A1A0) |

<a id="grupo-1445"></a>

## sItemDefinitionFlags

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| sItemDefinitionFlags(uint32 flags) | [source/game_sa/Enums/eItemDefinitionFlags.h:62](../../source/game_sa/Enums/eItemDefinitionFlags.h#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1446"></a>

## sVehAnimGroupGeneralTiming

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| sVehAnimGroupGeneralTiming() | [source/game_sa/VehicleAnimGroupData.h:67](../../source/game_sa/VehicleAnimGroupData.h#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1447"></a>

## sVehAnimGroupInOutTiming

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| operator\[\](eInOutTimingMode mode) | [source/game_sa/VehicleAnimGroupData.h:84](../../source/game_sa/VehicleAnimGroupData.h#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1448"></a>

## scm::ArrayAccess

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetElementTypeOf() | [source/game_sa/Scripts/RunningScript.h:150](../../source/game_sa/Scripts/RunningScript.h#L150) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1449"></a>

## scm::StringRef

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StringRef(char* str, size_t len, size_t cap) | [source/game_sa/Scripts/RunningScript.h:108](../../source/game_sa/Scripts/RunningScript.h#L108) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StringRef(ShortString& str) | [source/game_sa/Scripts/RunningScript.h:116](../../source/game_sa/Scripts/RunningScript.h#L116) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StringRef(LongString& str) | [source/game_sa/Scripts/RunningScript.h:120](../../source/game_sa/Scripts/RunningScript.h#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsNullTerminated() const | [source/game_sa/Scripts/RunningScript.h:131](../../source/game_sa/Scripts/RunningScript.h#L131) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1450"></a>

## tAudioZoneBox

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DrawWireFrame(CRGBA color, const CMatrix& transform) const | [source/game_sa/Audio/AudioZones.h:24](../../source/game_sa/Audio/AudioZones.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1451"></a>

## tBikeHandlingData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tBikeHandlingData::InitFromData(int32 id, const char* line) | [source/game_sa/cHandlingDataMgr.cpp:169](../../source/game_sa/cHandlingDataMgr.cpp#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gHandlingDataMgr: source/game_sa/cHandlingDataMgr.h:274 (0x00C2B9C8) |

<a id="grupo-1452"></a>

## tBinaryIplFile

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetObjInstances() const | [source/game_sa/tBinaryIplFile.h:26](../../source/game_sa/tBinaryIplFile.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCarGens() const | [source/game_sa/tBinaryIplFile.h:27](../../source/game_sa/tBinaryIplFile.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1453"></a>

## tBoatHandlingData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tBoatHandlingData::InitFromData(int32 id, const char* line) | [source/game_sa/cHandlingDataMgr.cpp:107](../../source/game_sa/cHandlingDataMgr.cpp#L107) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1454"></a>

## tBrightLight

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tBrightLight::Render() const | [source/game_sa/BrightLights.cpp:138](../../source/game_sa/BrightLights.cpp#L138) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo BrightTempIndicesStored: source/game_sa/BrightLights.cpp:27 (0x00C4B954); Posible dato fijo BrightTempVerticesStored: source/game_sa/BrightLights.cpp:26 (0x00C4B950); Posible dato fijo TempBufferVertices: source/game_sa/common.h:287 (0x00C4D958); Posible dato fijo aTempBufferIndices: source/game_sa/common.h:286 (0x00C4B958) |

<a id="grupo-1455"></a>

## tBuildingSwap

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tBuildingSwap() | [source/game_sa/Scripts/TheScripts.h:53](../../source/game_sa/Scripts/TheScripts.h#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tBuildingSwap(CBuilding* building, int32 nNewModelIndex, int32 nOldModelIndex) | [source/game_sa/Scripts/TheScripts.h:57](../../source/game_sa/Scripts/TheScripts.h#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Clear() | [source/game_sa/Scripts/TheScripts.h:63](../../source/game_sa/Scripts/TheScripts.h#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1456"></a>

## tColLighting

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tColLighting::GetCurrentLighting(float fScale) const | [source/game_sa/Collision/ColPoint.cpp:8](../../source/game_sa/Collision/ColPoint.cpp#L8) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_fDNBalanceParam: source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.h:24 (0x008D12C0) |
| tColLighting(const uint8 ucLighting) | [source/game_sa/Collision/ColPoint.h:22](../../source/game_sa/Collision/ColPoint.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1457"></a>

## tCompSearchStructById

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tCompSearchStructById(int32 id, RwFrame* frame) | [source/game_sa/Models/ClumpModelInfo.h:28](../../source/game_sa/Models/ClumpModelInfo.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1458"></a>

## tCompSearchStructByName

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tCompSearchStructByName(const char* name, RwFrame* frame) | [source/game_sa/Models/ClumpModelInfo.h:20](../../source/game_sa/Models/ClumpModelInfo.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1459"></a>

## tDirectionArrow

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tDirectionArrow::Render(RpClump* clump) | [source/game_sa/3dMarkers.cpp:749](../../source/game_sa/3dMarkers.cpp#L749) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_user3dMarkerColor: source/game_sa/3dMarkers.h:53 (0x00C7C620) |
| GetColor() const | [source/game_sa/3dMarkers.h:42](../../source/game_sa/3dMarkers.h#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInUse() const | [source/game_sa/3dMarkers.h:44](../../source/game_sa/3dMarkers.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1460"></a>

## tFlyingHandlingData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tFlyingHandlingData::InitFromData(int32 id, const char* line) | [source/game_sa/cHandlingDataMgr.cpp:135](../../source/game_sa/cHandlingDataMgr.cpp#L135) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

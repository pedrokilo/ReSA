# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1821"></a>

## notsa::script::commands::stat

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::stat::RegisterHandlers() | [source/game_sa/Scripts/Commands/StatCommands.cpp:48](../../source/game_sa/Scripts/Commands/StatCommands.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1822"></a>

## notsa::script::commands::text

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::text::RegisterHandlers() | [source/game_sa/Scripts/Commands/TextCommands.cpp:309](../../source/game_sa/Scripts/Commands/TextCommands.cpp#L309) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo UseTextCommands: source/game_sa/Scripts/TheScripts.h:419 (0x00A44B67) |

<a id="grupo-1823"></a>

## notsa::script::commands::unused

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::unused::RegisterHandlers() | [source/game_sa/Scripts/Commands/UnusedCommands.cpp:20](../../source/game_sa/Scripts/Commands/UnusedCommands.cpp#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1824"></a>

## notsa::script::commands::utility

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::utility::RegisterHandlers() | [source/game_sa/Scripts/Commands/UtilityCommands.cpp:14](../../source/game_sa/Scripts/Commands/UtilityCommands.cpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1825"></a>

## notsa::script::commands::vehicle

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::vehicle::RegisterHandlers() | [source/game_sa/Scripts/Commands/VehicleCommands.cpp:139](../../source/game_sa/Scripts/Commands/VehicleCommands.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1826"></a>

## notsa::script::commands::zone

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::zone::RegisterHandlers() | [source/game_sa/Scripts/Commands/ZoneCommands.cpp:120](../../source/game_sa/Scripts/Commands/ZoneCommands.cpp#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1827"></a>

## notsa::script::detail

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProcessCommandMemFnWrapper(CRunningScript* S) | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:29](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CollectArgsAndCall(CRunningScript* S, eScriptCommands command, T_FnRet(*CommandFn)(T_FnArgs...), T_CollectedArgs&&... args) | [source/game_sa/Scripts/CommandParser/Parser.hpp:19](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NotImplemented(CRunningScript& S, eScriptCommands cmd) | [source/game_sa/Scripts/CommandParser/Parser.hpp:69](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CommandParser(CRunningScript* S) | [source/game_sa/Scripts/CommandParser/Parser.hpp:76](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L76) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddCommandHandler() | [source/game_sa/Scripts/CommandParser/Parser.hpp:82](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| safe_arithmetic_cast(F value) | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:84](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1828"></a>

## notsa::script::detail::StrongAlias

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & operator=(const T& o) | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:24](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1829"></a>

## notsa::script::detail::scriptthing

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetAt(uint32 idx) -&gt; tScriptSphere& | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:46](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ScriptSphereArray: source/game_sa/Scripts/TheScripts.h:403 (0x00A91268) |
| GetAt(uint32 idx) -&gt; tScriptEffectSystem& | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:48](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ScriptEffectSystemArray: source/game_sa/Scripts/TheScripts.h:404 (0x00A44110) |
| GetAt(uint32 idx) -&gt; tScriptSearchlight& | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:50](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ScriptSearchLightArray: source/game_sa/Scripts/TheScripts.h:405 (0x00A94D68) |
| GetAt(uint32 idx) -&gt; tScriptSequence& | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:52](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ScriptSequenceTaskArray: source/game_sa/Scripts/TheScripts.h:407 (0x00A43F68) |
| GetAt(uint32 idx) -&gt; tScriptCheckpoint& | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:54](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ScriptCheckpointArray: source/game_sa/Scripts/TheScripts.h:410 (0x00A44070) |
| GetAt(uint32 idx) -&gt; T& | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:56](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_effects: source/game_sa/Scripts/Scripted2dEffects.h:62 (0x00C3AB00) |
| GetId(uint32 idx) -&gt; uint32 | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:64](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetId(uint32 idx) -&gt; uint32 | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:66](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ScriptReferenceIndex: source/game_sa/DecisionMakers/DecisionMakerTypes.h:45 (0x00C0AFF4), source/game_sa/PedGroups.h:16 (0x00C098D0), source/game_sa/Scripts/Scripted2dEffects.h:66 (0x00C3A120) |
| IsActive(uint32 idx) -&gt; bool | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:70](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsActive(uint32 idx) -&gt; bool | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:72](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_activated: source/game_sa/Scripts/Scripted2dEffects.h:65 (0x00C3A1A0) |

<a id="grupo-1830"></a>

## notsa::script::sig

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & Table() | [source/game_sa/Scripts/CommandParser/Signature.hpp:173](../../source/game_sa/Scripts/CommandParser/Signature.hpp#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Record() | [source/game_sa/Scripts/CommandParser/Signature.hpp:179](../../source/game_sa/Scripts/CommandParser/Signature.hpp#L179) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1831"></a>

## notsa::script::sig::detail

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ArithKind() | [source/game_sa/Scripts/CommandParser/Signature.hpp:63](../../source/game_sa/Scripts/CommandParser/Signature.hpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityKind() | [source/game_sa/Scripts/CommandParser/Signature.hpp:74](../../source/game_sa/Scripts/CommandParser/Signature.hpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddIn(Builder& b) | [source/game_sa/Scripts/CommandParser/Signature.hpp:82](../../source/game_sa/Scripts/CommandParser/Signature.hpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddOutTuple(Builder& b, std::tuple&lt;Ts...&gt;*) | [source/game_sa/Scripts/CommandParser/Signature.hpp:137](../../source/game_sa/Scripts/CommandParser/Signature.hpp#L137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddOut(Builder& b) | [source/game_sa/Scripts/CommandParser/Signature.hpp:140](../../source/game_sa/Scripts/CommandParser/Signature.hpp#L140) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FromFn(CmdSig& s, R (*)(A...)) | [source/game_sa/Scripts/CommandParser/Signature.hpp:166](../../source/game_sa/Scripts/CommandParser/Signature.hpp#L166) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1832"></a>

## notsa::script::sig::detail::Builder

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| In(Kind k, uint8_t sub = 0) | [source/game_sa/Scripts/CommandParser/Signature.hpp:52](../../source/game_sa/Scripts/CommandParser/Signature.hpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Out(Kind k) | [source/game_sa/Scripts/CommandParser/Signature.hpp:56](../../source/game_sa/Scripts/CommandParser/Signature.hpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1833"></a>

## sItemDefinitionFlags

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| sItemDefinitionFlags(uint32 flags) | [source/game_sa/Enums/eItemDefinitionFlags.h:62](../../source/game_sa/Enums/eItemDefinitionFlags.h#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1834"></a>

## sVehAnimGroupGeneralTiming

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| sVehAnimGroupGeneralTiming() | [source/game_sa/VehicleAnimGroupData.h:67](../../source/game_sa/VehicleAnimGroupData.h#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1835"></a>

## sVehAnimGroupInOutTiming

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| operator\[\](eInOutTimingMode mode) | [source/game_sa/VehicleAnimGroupData.h:84](../../source/game_sa/VehicleAnimGroupData.h#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1836"></a>

## scm::ArrayAccess

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetElementTypeOf() | [source/game_sa/Scripts/RunningScript.h:150](../../source/game_sa/Scripts/RunningScript.h#L150) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1837"></a>

## scm::StringRef

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StringRef(char* str, size_t len, size_t cap) | [source/game_sa/Scripts/RunningScript.h:108](../../source/game_sa/Scripts/RunningScript.h#L108) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StringRef(ShortString& str) | [source/game_sa/Scripts/RunningScript.h:116](../../source/game_sa/Scripts/RunningScript.h#L116) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StringRef(LongString& str) | [source/game_sa/Scripts/RunningScript.h:120](../../source/game_sa/Scripts/RunningScript.h#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsNullTerminated() const | [source/game_sa/Scripts/RunningScript.h:131](../../source/game_sa/Scripts/RunningScript.h#L131) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1838"></a>

## tAudioZoneBox

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DrawWireFrame(CRGBA color, const CMatrix& transform) const | [source/game_sa/Audio/AudioZones.h:24](../../source/game_sa/Audio/AudioZones.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1839"></a>

## tBikeHandlingData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tBikeHandlingData::InitFromData(int32 id, const char* line) | [source/game_sa/cHandlingDataMgr.cpp:179](../../source/game_sa/cHandlingDataMgr.cpp#L179) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gHandlingDataMgr: source/game_sa/cHandlingDataMgr.h:274 (0x00C2B9C8) |

<a id="grupo-1840"></a>

## tBinaryIplFile

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetObjInstances() const | [source/game_sa/tBinaryIplFile.h:26](../../source/game_sa/tBinaryIplFile.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCarGens() const | [source/game_sa/tBinaryIplFile.h:27](../../source/game_sa/tBinaryIplFile.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

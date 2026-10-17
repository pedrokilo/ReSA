<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1901"></a>

## notsa::script::commands::draw

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::draw::RegisterHandlers() | [source/game_sa/Scripts/Commands/DrawCommands.cpp:114](../../source/game_sa/Scripts/Commands/DrawCommands.cpp#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1902"></a>

## notsa::script::commands::game

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::game::RegisterHandlers() | [source/game_sa/Scripts/Commands/GameCommands.cpp:190](../../source/game_sa/Scripts/Commands/GameCommands.cpp#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FailCurrentMission: source/game_sa/Scripts/TheScripts.h:446 (0x00A444B0) |

<a id="grupo-1903"></a>

## notsa::script::commands::generic

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::generic::RegisterHandlers() | [source/game_sa/Scripts/Commands/GenericCommands.cpp:9](../../source/game_sa/Scripts/Commands/GenericCommands.cpp#L9) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1904"></a>

## notsa::script::commands::math

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::math::RegisterHandlers() | [source/game_sa/Scripts/Commands/MathCommands.cpp:40](../../source/game_sa/Scripts/Commands/MathCommands.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1905"></a>

## notsa::script::commands::mission

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::mission::RegisterHandlers() | [source/game_sa/Scripts/Commands/MissionCommands.cpp:78](../../source/game_sa/Scripts/Commands/MissionCommands.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1906"></a>

## notsa::script::commands::object

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::object::RegisterHandlers() | [source/game_sa/Scripts/Commands/ObjectCommands.cpp:186](../../source/game_sa/Scripts/Commands/ObjectCommands.cpp#L186) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1907"></a>

## notsa::script::commands::pad

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::pad::RegisterHandlers() | [source/game_sa/Scripts/Commands/PadCommands.cpp:36](../../source/game_sa/Scripts/Commands/PadCommands.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1908"></a>

## notsa::script::commands::path

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::path::RegisterHandlers() | [source/game_sa/Scripts/Commands/PathCommands.cpp:82](../../source/game_sa/Scripts/Commands/PathCommands.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1909"></a>

## notsa::script::commands::ped

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::ped::RegisterHandlers() | [source/game_sa/Scripts/Commands/PedCommands.cpp:29](../../source/game_sa/Scripts/Commands/PedCommands.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1910"></a>

## notsa::script::commands::player

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::player::RegisterHandlers() | [source/game_sa/Scripts/Commands/PlayerCommands.cpp:461](../../source/game_sa/Scripts/Commands/PlayerCommands.cpp#L461) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1911"></a>

## notsa::script::commands::script

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::script::RegisterHandlers() | [source/game_sa/Scripts/Commands/ScriptCommands.cpp:86](../../source/game_sa/Scripts/Commands/ScriptCommands.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1912"></a>

## notsa::script::commands::sequence

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::sequence::RegisterHandlers() | [source/game_sa/Scripts/Commands/SequenceCommands.cpp:21](../../source/game_sa/Scripts/Commands/SequenceCommands.cpp#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1913"></a>

## notsa::script::commands::stat

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::stat::RegisterHandlers() | [source/game_sa/Scripts/Commands/StatCommands.cpp:48](../../source/game_sa/Scripts/Commands/StatCommands.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1914"></a>

## notsa::script::commands::text

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::text::RegisterHandlers() | [source/game_sa/Scripts/Commands/TextCommands.cpp:309](../../source/game_sa/Scripts/Commands/TextCommands.cpp#L309) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo UseTextCommands: source/game_sa/Scripts/TheScripts.h:419 (0x00A44B67) |

<a id="grupo-1915"></a>

## notsa::script::commands::unused

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::unused::RegisterHandlers() | [source/game_sa/Scripts/Commands/UnusedCommands.cpp:20](../../source/game_sa/Scripts/Commands/UnusedCommands.cpp#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1916"></a>

## notsa::script::commands::utility

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::utility::RegisterHandlers() | [source/game_sa/Scripts/Commands/UtilityCommands.cpp:14](../../source/game_sa/Scripts/Commands/UtilityCommands.cpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1917"></a>

## notsa::script::commands::vehicle

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::vehicle::RegisterHandlers() | [source/game_sa/Scripts/Commands/VehicleCommands.cpp:139](../../source/game_sa/Scripts/Commands/VehicleCommands.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1918"></a>

## notsa::script::commands::zone

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::zone::RegisterHandlers() | [source/game_sa/Scripts/Commands/ZoneCommands.cpp:120](../../source/game_sa/Scripts/Commands/ZoneCommands.cpp#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1919"></a>

## notsa::script::detail

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProcessCommandMemFnWrapper(CRunningScript* S) | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:29](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CollectArgsAndCall(CRunningScript* S, eScriptCommands command, T_FnRet(*CommandFn)(T_FnArgs...), T_CollectedArgs&&... args) | [source/game_sa/Scripts/CommandParser/Parser.hpp:18](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NotImplemented(CRunningScript& S, eScriptCommands cmd) | [source/game_sa/Scripts/CommandParser/Parser.hpp:68](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CommandParser(CRunningScript* S) | [source/game_sa/Scripts/CommandParser/Parser.hpp:75](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddCommandHandler() | [source/game_sa/Scripts/CommandParser/Parser.hpp:81](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| safe_arithmetic_cast(F value) | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:84](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1920"></a>

## notsa::script::detail::StrongAlias

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & operator=(const T& o) | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:24](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

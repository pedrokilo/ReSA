<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1861"></a>

## notsa::envmap

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * PoolNew(Pool* pool) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:104](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PoolDelete(Pool* pool, const void* obj) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:123](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1862"></a>

## notsa::script

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetScriptCommandName(eScriptCommands cmd) | [source/game_sa/Enums/eScriptCommands.h:2904](../../source/game_sa/Enums/eScriptCommands.h#L2904) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CommandHasCustomHandler() | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:21](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GTAProcessCommand(CRunningScript* S) | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:40](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHandlerOfCommand() | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:46](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IterateCommandIDs(auto&& functor) | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:56](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GenerateLUT() | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:61](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Read(CRunningScript* S) | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:111](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo UsedObjectArray: source/game_sa/Scripts/TheScripts.h:384 (0x00A44B70) |
| StoreArg(CRunningScript* S, T arg) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:18](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, const T& arg) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:30](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, const CVector& v3) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:53](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, const CVector2D& v2) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:59](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, CompareFlagUpdate flag) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:65](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, T&& value) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:74](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, T value) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:94](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, const MultiRet&lt;Ts...&gt;& arg) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:102](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L102) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| return_multiple(Ts&&... args) | [source/game_sa/Scripts/CommandParser/Utility.hpp:20](../../source/game_sa/Scripts/CommandParser/Utility.hpp#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FixAngleDegrees(float deg) | [source/game_sa/Scripts/CommandParser/Utility.hpp:38](../../source/game_sa/Scripts/CommandParser/Utility.hpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPedOrItsVehicle(CPed& ped) -&gt; CPhysical& | [source/game_sa/Scripts/CommandParser/Utility.hpp:49](../../source/game_sa/Scripts/CommandParser/Utility.hpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1863"></a>

## notsa::script::commands::basic

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::basic::RegisterHandlers() | [source/game_sa/Scripts/Commands/BasicCommands.cpp:230](../../source/game_sa/Scripts/Commands/BasicCommands.cpp#L230) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1864"></a>

## notsa::script::commands::camera

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::camera::RegisterHandlers() | [source/game_sa/Scripts/Commands/CameraCommands.cpp:308](../../source/game_sa/Scripts/Commands/CameraCommands.cpp#L308) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1865"></a>

## notsa::script::commands::character

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::character::RegisterHandlers() | [source/game_sa/Scripts/Commands/CharacterCommands.cpp:4046](../../source/game_sa/Scripts/Commands/CharacterCommands.cpp#L4046) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1866"></a>

## notsa::script::commands::cleo::audiostream

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::audiostream::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEOAudioStreamCommands.cpp:89](../../source/game_sa/Scripts/Commands/CLEO/CLEOAudioStreamCommands.cpp#L89) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1867"></a>

## notsa::script::commands::cleo::character

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::character::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEOCharacterCommands.cpp:6](../../source/game_sa/Scripts/Commands/CLEO/CLEOCharacterCommands.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1868"></a>

## notsa::script::commands::cleo::dynamiclibrary

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::dynamiclibrary::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEODynamicLibraryCommands.cpp:30](../../source/game_sa/Scripts/Commands/CLEO/CLEODynamicLibraryCommands.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1869"></a>

## notsa::script::commands::cleo::fs

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::fs::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEOFsCommands.cpp:54](../../source/game_sa/Scripts/Commands/CLEO/CLEOFsCommands.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1870"></a>

## notsa::script::commands::cleo::game

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::game::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEOGameCommands.cpp:14](../../source/game_sa/Scripts/Commands/CLEO/CLEOGameCommands.cpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1871"></a>

## notsa::script::commands::cleo::generic

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::generic::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEOGenericCommands.cpp:47](../../source/game_sa/Scripts/Commands/CLEO/CLEOGenericCommands.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1872"></a>

## notsa::script::commands::cleo::memory

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::memory::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEOMemoryCommands.cpp:83](../../source/game_sa/Scripts/Commands/CLEO/CLEOMemoryCommands.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1873"></a>

## notsa::script::commands::cleo::pad

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::pad::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEOPadCommands.cpp:14](../../source/game_sa/Scripts/Commands/CLEO/CLEOPadCommands.cpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1874"></a>

## notsa::script::commands::cleo::script

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::script::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEOScriptCommands.cpp:43](../../source/game_sa/Scripts/Commands/CLEO/CLEOScriptCommands.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1875"></a>

## notsa::script::commands::cleo::vehicle

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::vehicle::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEOVehicleCommands.cpp:33](../../source/game_sa/Scripts/Commands/CLEO/CLEOVehicleCommands.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1876"></a>

## notsa::script::commands::cleo::world

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::world::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEOWorldCommands.cpp:28](../../source/game_sa/Scripts/Commands/CLEO/CLEOWorldCommands.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1877"></a>

## notsa::script::commands::clock

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::clock::RegisterHandlers() | [source/game_sa/Scripts/Commands/ClockCommands.cpp:36](../../source/game_sa/Scripts/Commands/ClockCommands.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1878"></a>

## notsa::script::commands::comparasion

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::comparasion::RegisterHandlers() | [source/game_sa/Scripts/Commands/ComparasionCommands.cpp:13](../../source/game_sa/Scripts/Commands/ComparasionCommands.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1879"></a>

## notsa::script::commands::conversation

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::conversation::RegisterHandlers() | [source/game_sa/Scripts/Commands/ConversationCommands.cpp:74](../../source/game_sa/Scripts/Commands/ConversationCommands.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1880"></a>

## notsa::script::commands::draw

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::draw::RegisterHandlers() | [source/game_sa/Scripts/Commands/DrawCommands.cpp:114](../../source/game_sa/Scripts/Commands/DrawCommands.cpp#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

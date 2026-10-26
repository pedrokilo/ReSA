<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco ScriptsBrainsRequest31: CScriptsForBrains::RequestAttractorScriptBrainWithThisName (0x46AA80)

**Estado: PREPARADO (19/10/2026).** Fuente reconstruida y revisada contra el EXE; el banco NO se ha compilado ni ejecutado (lo hace el coordinador, en serie). Sin registro, sin tocar `docs/` ni `hooks.csv`. Plantilla: `reversing/DecisionMakeDecision27` (estructura, compuertas de ABI/x87) con colaboradores sustituidos como en `reversing/CTaskSimpleJetPack`.

## Comandos para el coordinador

```
python -I reversing/ScriptsBrainsRequest31/fixture_oracle.py        # autoprueba: compila Fixtures.hpp con cl, compara constantes y calcula la cobertura (ya ejecutada: ok)
python -B reversing/ScriptsBrainsRequest31/build_probe.py           # compila la sonda (solo el coordinador)
python -B reversing/ScriptsBrainsRequest31/run_local.py             # corre en el clon E:\JUEGOS\GTA San Andreas - CVehicle Test
python -B reversing/ScriptsBrainsRequest31/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # solo analisis (None si el registro no existe o no ha terminado)
python -B reversing/ScriptsBrainsRequest31/register_hook.py <carpetaEvidencia>   # solo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedInstall(RequestAttractorScriptBrainWithThisName, 0x46AA80)` (nuevo) y los hooks de los colaboradores `GetIndexOfScriptBrainWithThisName` (0x46AA30) y `CStreaming::RequestModel` (0x4087E0), ya en HEAD. Aplicar antes `source.patch` si el arbol no lo contiene (sin cambios en el `.h`). Arranque en el clon: `scriptsbrainsrequest31-runtime.start`; salida `scriptsbrainsrequest31-runtime.jsonl`.

## Fuente (FASE 1)

`source.patch` (git diff unificado contra HEAD, SOLO esta funcion; verificado con `git apply --check` sobre `git show HEAD:source/game_sa/ScriptsForBrains.cpp` en un directorio temporal, y encadenable con los parches de los otros dos bancos hermanos en cualquier orden) toca `source/game_sa/ScriptsForBrains.cpp`: sustituye `plugin::CallMethod<0x46AA80,...>` por el cuerpo C++ y añade el `RH_ScopedInstall`. Sin `__asm` ni llamadas al EXE por esta funcion.

Qué hace el EXE (0x46AA80..0x46AAD8, thiscall, ECX = this, pila: name, `ret 4`):

```
ESI = this ; push 5 ; push name ; call 0x46AA30 (GetIndexOfScriptBrainWithThisName(name, 5))
test ax,ax ; jl fin                                        (SOLO AX; los 16 bits altos de EAX no cuentan)
EAX = movsx ax ; ECX = EAX*5 ; EAX = movsx word [ESI + ECX*4] (= m_StreamedScriptIndex del cerebro)   ; ESI = this + ECX*4
EDX = EAX*5 ; cmp byte [EDX*4 + 0x964E08],1 ; je saltar    (estado de carga de la entrada de modelo 0x6676 + script; != 1 -> pedir)
push 0x10 ; push EAX+0x6676 ; call 0x4087E0 (CStreaming::RequestModel, cdecl) ; add esp,8
EAX = movsx word [ESI] ; EAX <<= 5 ; CL = byte [EAX + 0xA47B64] ; CL++ ; byte [EAX + 0xA47B64] = CL   (m_NumberOfUsers, uint8, 255 -> 0)
```
Efecto observable: `m_NumberOfUsers[script]++` (con desborde a 0) y, si el modelo no esta cargado (estado != 1, cualquier byte), `RequestModel(0x6676 + script, 0x10)`. 0x10 es `STREAMING_PRIORITY_REQUEST`.

## Revisión propia contra el EXE (FASE 2)

Tras escribir, se volvió a desensamblar y se contrastó linea a linea:

| EXE | fuente |
|---|---|
| `call 0x46AA30` con `(name, 5)` ; `test ax,ax ; jl` | `GetIndexOfScriptBrainWithThisName(name, 5)` devuelve `int16`; `if (index < 0) return;` |
| `movsx eax,word [esi+ecx*4]` | `m_aScriptForBrains[index].m_StreamedScriptIndex` (int16) |
| `cmp byte [edx*4+0x964E08],1` | `CStreaming::IsModelLoaded(SCMToModelId(script))`: `ms_aInfoForModel[0x6676+script].m_LoadState == LOADSTATE_LOADED` (el banco comprueba en ejecucion que `&m_LoadState` del modelo 0x6676 es 0x964E08 y que la entrada mide 20) |
| `push 0x10 ; push eax+0x6676 ; call 0x4087E0` | `CStreaming::RequestModel(modelId, STREAMING_PRIORITY_REQUEST)` |
| `byte [script*32 + 0xA47B64]++` | `CTheScripts::StreamedScripts.m_aScripts[script].m_NumberOfUsers++` (entradas de 32 bytes, `m_NumberOfUsers` en +4; comprobado en ejecucion: `&m_aScripts[0].m_NumberOfUsers == 0xA47B64`) |

El borrador del lote 27 (`git diff HEAD` del arbol principal) coincidia con el EXE; solo se cambio el literal `0x10` por `STREAMING_PRIORITY_REQUEST` (mismo valor) y se anotó el contraste. Sin discrepancias de comportamiento.

## Registros EAX/ECX/EDX de salida: ningun llamador los lee

Unico llamador en el EXE (`residue_scan.py 0x46AA80`): `0x533801` (`mov ecx,0xA90CF0 ; call 0x46AA80 ; jmp 0x533B9C` -> bucle: recarga `eax`/`ecx` desde la pila y `ebx`). La siguiente llamada, `0x4C4C70`, recibe ECX recien escrito y escribe `edx` (`mov edx,[ebx]`) antes de leerlo. Nadie lee EAX/ECX/EDX de salida, asi que el banco los **registra** pero no los exige.

## Diseño del banco

- Original = entrada 0x46AA80 del EXE con el hook apagado; candidato = hook encendido (`HookRoute`/`RegisteredTarget`).
- **Colaboradores sustituidos**, con el MISMO falso en la direccion del EXE y en el cuerpo C++ de la DLL (hook encendido; `Body()` resuelto antes de parchear; parches quitados ANTES de restaurar los hooks): `GetIndexOfScriptBrainWithThisName` (devuelve el EAX programado completo; AX = indice o negativo, los 16 bits altos son basura a proposito; registra this, `name`, tipo) y `CStreaming::RequestModel` (registra modelo y banderas). Se sustituye GetIndex para aislar la funcion del `strcmp`/`_stricmp` (CRT del EXE o de la DLL) y de la busqueda por nombre: su banco propio es otro.
- **Memoria real del juego** que toca la funcion, guardada y restaurada tras CADA lado y comprobada al final: `CTheScripts::StreamedScripts.m_aScripts` (82 entradas x 32 bytes desde 0xA47B60; solo cambia `m_NumberOfUsers` en +4) y la ventana de estados de carga `0x964E08 + script*20` (82 x 20 bytes). Cada caso fija los 82 `m_NumberOfUsers` y los 82 estados de carga y compara los bytes cambiados (`users_diff`, `info_diff`) con el oraculo.
- 4096 fixtures (`kind = n % 16`): indice -1 y negativos distintos (bit 15), script cargado/no cargado, estados 0/2..4 y no canonicos (5..255), usuarios 255 (desborda), 0/127/128/254, indice de cerebro 0 y 69, script 0 y 81, vecinos con estado y usuarios contrarios (detectan indice errado), basura en los 16 bits altos de EAX, cerebro con tipo != 5 (el falso decide) y aleatorio. Los scripts de los cerebros siempre estan en 0..81 (fuera de rango el EXE leeria memoria arbitraria y la DLL haria saltar el `assert` del `std::array`).
- Constantes de arena (`this` 0x40+0x578+0x40 = 0x5F8; `name` 0x20+0x20+0x20 = 0x60; ventanas 0xA40 y 0x668) iguales en `Fixtures.hpp` y `fixture_oracle.py`, comprobadas por la autoprueba (compila `Fixtures.hpp` con `cl` y compara constantes y todas las filas).
- Compuertas por lado: ESP, EBX/ESI/EDI/EBP, DF=0, x87 normalizado (la funcion no usa x87; se comprueba que lo deja intacto), MXCSR, `route_ok` (con los cuatro parches puestos), globales restaurados. `run_local.py` con oraculo independiente, cobertura minima calibrada (85 % de lo observado) y `analyze` que devuelve `None` si el `.jsonl` no existe. Probado sin juego con un registro sintetico perfecto (aceptado) y 8 corrupciones (rechazadas).

## Suposiciones y riesgos

- No se ha compilado: `Core.hpp` se comprobo sintacticamente (`cl /Zs`, x86, C++20) con un stub de `HookRoute`; `LiveComparison.hpp` usa tipos del juego (`CStreaming::GetInfo(...)`, `CTheScripts::StreamedScripts`, `tScriptForBrains`) y sus comprobaciones de layout se evaluan en ejecucion (`layout_ok`).
- Se escribe en memoria real del juego (usuarios de scripts y estados de carga de los modelos de script) durante el banco y se restaura cada lado; el banco corre dentro de un solo fotograma, en el hilo del juego, sin streaming concurrente sobre esos modelos.
- Se asume que la DLL llama a `GetIndexOfScriptBrainWithThisName` y `RequestModel` por sus cuerpos (Debug, sin inlining); si estuvieran en linea las trazas saldrian vacias y el oraculo lo delataria.
- El oraculo no cubre el `strcmp` de GetIndex (colaborador sustituido, validado por su propia entrada del registro).
- Indices de script fuera de 0..81 no se prueban (comportamiento sin definir en ambos lados).

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `register_hook.py` (plantilla), `source.patch`, `README.md`.

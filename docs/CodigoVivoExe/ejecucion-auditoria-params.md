<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoria de parametros de los comandos de script (EXE contra C++), 22/11/2026

Complemento de `contrib/audit_script_dispatchers.py` (que busca comandos sin caso ni manejador). Aqui se comprueba lo contrario: un comando que **si** tiene caso o manejador C++ pero que consume del script **menos, mas o distintos** parametros que el caso del EXE. Eso desalinea la IP del script ('Unknown param type: 64' y similares).

## Herramienta: `contrib/audit_script_params.py`

```
python contrib/audit_script_params.py [--csv salida.csv] [--solo N,M] [--regenerar-exe] [--sin-firmas]
```

Codigo de salida 1 si hay gravedad 1 (longitud distinta y no documentada). No necesita el juego ni `rea`: usa capstone sobre el PE (`D:/Grand Theft Auto San Andreas/gta-sa.exe`) con el mismo criterio de switch que `audit_script_dispatchers.py` (tabla 0x8A6168, jump table, reenvio entre despachadores).

**Lado EXE** (cache en `ejecucion-auditoria-params-exe.csv`, 2639 filas; `--regenerar-exe` la rehace). Por cada comando 0..2638 se recorre el grafo de control del caso anotando, en orden, las llamadas a las rutinas que consumen parametros: `CollectParameters(n)` (n valores, 'a'), `GetPointerToScriptVariable` y `GetIndexOfGlobalVariable` (1 variable, 'v'), `ReadTextLabelFromScript` (1 texto, 't'), `StoreParameters(n)` (n variables, 'v'), `ReadParametersForNewlyStartedScript` ('#', cuenta variable) y `UpdatePC`. El `n` se resuelve aunque no sea un `push imm` directo (constantes en registros `mov ebx, 1; push ebx`, argumentos apilados antes de un `jmp` a una cola comun, `ebx/edi/ebp` fijados en el prologo del despachador). Las ayudas 'Locate*Command', '*InAreaCheckCommand', 'PlayAnimScriptCommand', 'ScriptTaskPickUpObject' se expanden; sus filas `W:<nombre>` en el CSV se comparan ademas con su cuerpo C++. Un caso con ramas da el conjunto de secuencias posibles (las que acaban en bucle o jump table interna llevan marca J/L/*).

**Lado C++:**
* Manejadores `REGISTER_COMMAND_HANDLER(cmd, fn)`: la firma **exacta** de `notsa::script::sig::Table()` (la que ya construye `Signature.hpp` en tiempo de compilacion). Para poder leerla desde fuera se anadio `sig::DumpIfRequested()` (llamada al final de `InjectCustomCommandHooks`): con `GTASA_DUMP_SIGS=<archivo>` el juego vuelca la tabla (volcado en `ejecucion-auditoria-params-cpp-sig.csv`; sin la variable no hace nada). Si el manejador recibe `CRunningScript&` se suman las lecturas de su cuerpo (p. ej. `CharInAngledAreaCheck`). Sin CSV de firmas hay un fallback por texto (menos fiable, los `auto` quedan en gravedad 3).
* Sin manejador: el `case` del despachador. Se recorre el texto con ramas (`if`, bucles, `return` anticipado), se expanden las ayudas definidas en `source/game_sa/Scripts` (funciones y lambdas locales como `FinishCreateFx`, que hace el `StoreParameters(1)`) y se obtiene el conjunto de rutas de lectura posibles; el comando es correcto si alguna ruta C++ coincide con alguna ruta limpia del EXE.

**Comparacion** (cadena de unidades a/v/t): gravedad 0 = igual (1808), igual con cuenta variable (50), equivalente a/v (123: el C++ lee por valor lo que el EXE lee como puntero a variable, o texto frente a variable de texto: mismos bytes del script), excepcion documentada (2); 1 = distinta cantidad de unidades; 2 = texto frente a valor (cambia el formato del parametro); 3 = no resuelto; 4 = EXE atiende y no hay C++. Resultado completo en `ejecucion-auditoria-params.csv` (1983 comandos atendidos por el EXE y con C++).

Nota: `docs/ComandosScript.csv` numera la cola con el enum de Xbox (IS_XBOX_PLAYER2_PRESSING_START = 2638); en PC el 2638 es `DO_DEBUG_STUFF`. La herramienta lo fuerza para no dar un falso 'sin C++'.

## Discrepancias halladas y que se hizo

Primera pasada (sobre `1ec835719c`): 12 comandos reales corregidos y 2 que se dejan.

| Comando (id) | EXE | C++ antes | Accion |
| --- | --- | --- | --- |
| IS_AUSTRALIAN_GAME (1434) | 0x48EE42: `push 0; UpdateCompareFlag`, no lee parametros | `case 1434` hacia `CollectParameters(1)` (el comentario lo daba por bueno) | Quitada la lectura (`RunningScriptCommands1400To1499.cpp`) |
| SAVE_INT_TO_DEBUG_FILE (935), SAVE_FLOAT_TO_DEBUG_FILE (936) | `CollectParameters(1)` | `REGISTER_COMMAND_NOP` sin parametros | `NOP(..., int32)` / `NOP(..., float)` (`GameCommands.cpp`) |
| SAVE_TEXT_LABEL_TO_DEBUG_FILE (2059) | `ReadTextLabelFromScript` | NOP sin parametros en `TextCommands.cpp` que tapaba el `case` correcto de `ProcessCommands2000To2099` (el manejador gana a los casos) | Quitado el NOP; lo atiende el `case` |
| SAVE_STRING_TO_DEBUG_FILE (1462) | 0x48FAEC: copia 128 bytes crudos del script y avanza la IP (sin byte de tipo) | NOP con 1 `uint32` tipado | Manejador `SaveStringToDebugFile(CRunningScript&)`: `m_IP += 128` (`UnusedCommands.cpp`) |
| VIEW_INTEGER_VARIABLE (1629), VIEW_FLOAT_VARIABLE (1630), WATCH_INTEGER_VARIABLE (1631), WATCH_FLOAT_VARIABLE (1632) | variable global + etiqueta (`GetIndexOfGlobalVariable; ReadTextLabelFromScript`) | NOP `uint32, uint32` (leia la etiqueta como entero tipado) | `NOP(int32*/float*, const char*)` |
| WRITE_DEBUG (1634), WRITE_DEBUG_WITH_INT (1635), WRITE_DEBUG_WITH_FLOAT (1636) | etiqueta (+ valor) | NOP `uint32` / `uint32, uint32` | `NOP(const char*[, int32/float])` |
| DRAW_LIGHT (592), POP_CAR_BOOT_USING_PHYSICS (1291) | el caso (0x47F21F, 0x48CC39) devuelve sin leer ni hacer nada (comando muerto en el EXE) | el manejador lee 6 / 1 parametros y actua | **Se dejan** (excepcion documentada en la herramienta): un script que los usara tambien se desalinearia en el EXE, asi que ningun script del juego los trae; leer es lo mas seguro para mods |

Todo lo demas coincide o es equivalente. El resto de los resultados de la primera pasada eran falsos positivos de la propia herramienta, corregidos (el C++ no era el culpable): ramas con `return` anticipado y dos `StoreParameters`, ayudas/lambdas con el `StoreParameters` (`CreateFxOnEntity`, `FinishCreateFx`), `push ebx` con `ebx=1`, `n` apilado antes de un `jmp` a la cola comun (`GET_NTH_CLOSEST_CAR_NODE`), `GetIndexOfGlobalVariable` y plantillas con paquete de parametros (`PrintWithNumbers<...>`).

## Alcance y limites

* Compara **cuantos y de que clase** son los parametros leidos (valor/variable/texto) y en que orden; no el significado de cada parametro ni el efecto del comando (eso son los bancos por comando de `reversing/CmdBench`).
* Los casos con ramas se aceptan si alguna ruta coincide; un error solo en una rama poco frecuente puede pasar. Los comandos con cuenta variable (`START_NEW_SCRIPT`, ayudas 2D/3D) se comparan con `#` como 'una o mas a'.
* Lecturas inline del puntero de IP (como `SAVE_STRING_TO_DEBUG_FILE`) no las ve la herramienta: se revisaron a mano (unico caso).
* La cache del EXE fija el resultado para el ejecutable original de 5 189 632 bytes; `--regenerar-exe` solo hace falta si cambia el EXE o la herramienta.

## Verificacion

Ver el informe del commit: compilacion con `_build_AUDITPARAMS.sh` (EXIT=0), `python contrib/audit_hook_catalog.py --strict` (OK), `python contrib/audit_script_dispatchers.py` (0 huerfanos), `python contrib/audit_script_params.py` (codigo 0, solo las 2 excepciones documentadas) y humo en `reversing/FnBIS1` (`prueba_ropero.py`, `prueba_agua.py`, `prueba_marcadores.py`, `prueba_partida_nueva.py --segundos 300 --entrada`).

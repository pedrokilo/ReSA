<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/11/2026 (Ciudad de México):** 1004 grupos / 13311 funciones reconstruidas; 11801 con evidencia histórica y 1510 pendientes de validación en 39 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CmdBench: banco de validacion por comando de script

Motivo: `GET_CHAR_AREA_VISIBLE` (0x09E8) devolvia un booleano en vez del id del interior, rompia el ropero de CJ y nadie lo habia validado porque los
comandos de script no estan en `docs/hooks.csv` (el EXE los atiende dentro de despachadores por centenas, sin funcion propia por comando).
Este banco ejecuta cada manejador C++ reescrito (`REGISTER_COMMAND_HANDLER`, 668 sin CLEO) y el **manejador original del EXE** sobre el mismo estado y
con los mismos parametros, y compara el resultado.

## Piezas

| Archivo | Que es |
| --- | --- |
| `source/game_sa/Scripts/CommandParser/Signature.hpp` | Descripcion en tiempo de compilacion de la firma de cada manejador (parametros leidos del script, variables escritas). `AddCommandHandler` la registra (`notsa::script::sig::Table()`). Sin efecto en el juego. |
| `LiveComparison.hpp` | Sonda dentro del juego (`CmdBenchLive::Update()` desde `CGame::Process`, via `build_probe.py`). |
| `build_probe.py` | Compila el ASI de la sonda (`probe_build.py`), no acredita nada. |
| `run_local.py` | Lanza el juego de pruebas, lee `cmdbench-runtime.jsonl`, resume; si el juego cae en un comando lo omite y reintenta. `--ids`, `--risk alto,medio`, `--category Character`, `--cases N`. |
| `informe.py` | Resume los registros: por comando casos, diferencias y su tipo; `--detalle ID` imprime casos. |
| `exe_case.py` | Desensambla (capstone) el caso de un comando en el despachador del EXE (tabla 0x8A6168, tabla de saltos del switch). |
| `mutate.py` | Prueba negativa: introduce errores conocidos en manejadores y comprueba que el banco falla. |

## Como funciona un caso

1. Escena: el juego cargado desde la partida de pruebas (FastLoader); se crean fixtures reales (un peaton, un coche, un objeto; el jugador es el otro peaton).
2. Script sintetico: un `CRunningScript` propio con bytecode generado a partir de la firma del manejador (constantes de 8/16/32 bits, variables locales
   y globales de trabajo en `ScriptSpace+0xFE00`, floats, textos de 8 caracteres, manejadores de las fixtures, `-1` donde el parametro admite nulo) mas
   las variables de salida (rellenas con `0xCAFE0000+i`). Parametros deterministas por (comando, caso).
3. Estado de partida: instantanea de `.data` del EXE (0x8A4000, 4 MB: ScriptParams, estadisticas, camara, texto, pools, jugadores...) y de los bytes de las
   fixtures; por caso, las fixtures se **perturban** (enteros, banderas y flotantes que no son punteros) y en el 30 % el peaton conduce el coche.
4. Se ejecuta el lado C++ (`CustomCommandHandlerOf(cmd)`), se restaura el estado, se ejecuta el lado EXE (`s_OriginalCommandHandlerTable[cmd/100]`,
   con los tres despachadores reescritos 800/1200/1900 apagados), y otra vez cada lado para medir el ruido.
5. Se compara: valor de retorno, avance del IP, bandera de comparacion / AND-OR / NotFlag, variables de salida, **todos los bytes cambiados** en `.data`
   y en las fixtures, y si algun lado lanzo excepcion. Los bytes que cambian entre dos ejecuciones del mismo lado (hilos de audio/streaming) son ruido
   y se descartan. `ScriptParams` se restaura pero no se compara (el EXE deja ahi parametros y resultados; la DLL lee tipado).
6. Se registran por comando cuantos casos escribieron variables de salida / bandera / memoria en el EXE, para detectar comparaciones vacias.

## Lo que no cubre
Ver la seccion de resultados en `docs/ComandosScript.md`. Resumen de limites: estado de montones (pools de tareas, nodos de listas del mundo, texturas) fuera de
`.data`; comandos que dependen de tiempo (WAIT), de varios fotogramas o de scripts reales; firmas con tipos sin generador.

## Resultados (corrida del 21/11/2026, dominio `safe`, 32 casos por comando, build Debug de la rama `cmdbench`)

| Estado | Comandos |
| --- | ---: |
| validado (sin diferencias y con efecto observable en el EXE) | 533 |
| diferencia (abierta; ver `docs/ComandosScript.md`) | 50 |
| sin_efecto (sin diferencias pero el EXE no produjo efecto observable con los parametros generados) | 5 |
| no_original (el EXE no implementa el comando: rama por defecto del despachador, devuelve -1 sin leer parametros) | 27 |
| no_registrado (`path`/`draw::RegisterHandlers()` no se llaman: el juego usa el EXE) | 14 |
| no_probado (cae o bloquea el juego, necesita estado que el banco no da) | 39 |

Detalle por comando: `RESULTADOS.csv` (se regenera con `python resumen_banco.py --desde AAAAMMDD-HHMMSS --csv RESULTADOS.csv`, y `python contrib/inventario_comandos_script.py` lo vuelca a `docs/ComandosScript.csv` y `.md`). Hallazgos y causas de las diferencias abiertas: `hallazgos.json`.

Prueba negativa (`mutate.py`): ronda 1 (regresion real de `GET_CHAR_AREA_VISIBLE` + 4 mutaciones) detecta 3 de 5 (`GET_CHAR_AREA_VISIBLE`, `GET_CHAR_COORDINATES`, `GET_CHAR_HEADING`); no detecta `IS_CHAR_IN_ANY_CAR` (bInVehicle sin vehiculo no ocurre en las fixtures) ni `GET_CHAR_MODEL` (& 0xFF con modelos < 256): mutantes equivalentes con estas fixtures. Ronda 2 detecta 4 de 5 (`IS_CHAR_STOPPED`, `IS_CHAR_ON_SCREEN`, `GET_CHAR_HEALTH`, `GET_CHAR_ARMOUR`); `IS_CHAR_IN_ZONE` queda sin detectar (las zonas aleatorias nunca coinciden).

## Como repetirlo
1. `python reversing/CmdBench/build_probe.py` (CMAKE_BUILD_PARALLEL_LEVEL=1, _CL_=-MP1) compila la sonda y la deja en `_baseline` de la copia de pruebas.
2. `python -B reversing/CmdBench/run_local.py --cases 32 --domain safe --skip reversing/CmdBench/known-skip.txt --tag X` (toma el candado `D:/Original/gta_lock`, espera a que no haya `gta_sa.exe`, restaura ASI/INI/PDB al terminar). Un fichero `reversing/CmdBench/cmdbench.stop` cierra solo el juego lanzado por el ejecutor y detiene los reintentos. No matar el ejecutor con el juego abierto (deja la sonda instalada).
3. `python reversing/CmdBench/informe.py --desde AAAAMMDD-HHMMSS` / `--detalle ID`, `python reversing/CmdBench/exe_case.py ID` (desensamblado del caso del EXE).

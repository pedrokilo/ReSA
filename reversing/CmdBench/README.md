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

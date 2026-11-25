<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# SCRA: despachadores de comandos de script de las centenas 0-999

Rama `vivo-scra` (desde `origin/main` 59b1028202, 21/11/2026). Contexto: el EXE independiente (fase 8, [fase8-experimento.md](fase8-experimento.md) sec. 7) llega al menu y rompe al empezar partida nueva en `ProcessCommands1000To1099` (0x489500). El EXE despacha cada comando por centenas con la tabla `0x8A6168` (27 ranuras, `s_OriginalCommandHandlerTable`); `ProcessOneCommand` solo llega a ella si el comando no tiene manejador propio.

## 1. Que centenas faltaban

| Ranura | Comandos | EXE | Estado antes | Ahora |
| ---: | --- | --- | --- | --- |
| 0 | 0-99 | 0x465E60 | EXE | `ProcessCommands0To99` |
| 1 | 100-199 | 0x466DE0 | EXE | `ProcessCommands100To199` |
| 2 | 200-299 | 0x469390 | EXE | `ProcessCommands200To299` |
| 3, 4 | 300-499 | 0x47C100, 0x47D210 | C++ (P6B) | sin cambios |
| 5 | 500-599 | 0x47E090 | EXE | `ProcessCommands500To599` |
| 6 | 600-699 | 0x47F370 | EXE | `ProcessCommands600To699` (la tabla solo cubre 659..697) |
| 7 | 700-799 | 0x47FA30 | EXE | `ProcessCommands700To799` (703..798) |
| 8 | 800-899 | 0x481300 | C++ (239A) | sin cambios |
| 9 | 900-999 | 0x483BD0 | EXE | `ProcessCommands900To999` |

Ninguna centena cae en otro mecanismo: las 27 ranuras son despachadores del EXE; 10-27 son de otros agentes o ya estaban hechas (1200, 1500, 1700, 1900, 2000).

## 2. Politica de casos

Como en P6B: solo llevan `case` los comandos con estado `original` o `no_implementado` en `docs/ComandosScript.csv` que tienen caso en la tabla de saltos del EXE (las tablas de saltos y de indices se volcaron del EXE; 0x466C50, 0x468364, 0x469DD4/0x469E4C, 0x47F24C/0x47F304, 0x47F9BC/0x47FA04, 0x480F10/0x480FD8, 0x4856B8/0x4857C4). Todo el resto cae en `default:` -> `CRunningScript::ProcessHandledCommand(command)`: llama al manejador propio de C++ (mismo `OpcodeResult` que `ProcessOneCommand`; asi se conserva p. ej. el tipo de retorno de `GET_CHAR_AREA_VISIBLE`) o, si no hay, devuelve `OR_INTERRUPT` como el destino por defecto del EXE (`or al,0xFF`). Los comandos con manejador propio nunca llegan al despachador salvo con `GTASA_CMD_BYPASS`, que por tanto sigue usando el C++ propio.

| Ranura | Casos con cuerpo / vacios | Comandos |
| ---: | ---: | --- |
| 0 | 1 | 82 LINE (lee 6 parametros) |
| 1 | 14 | 152, 154, 155, 165-171, 173, 175-177 |
| 2 | 6 | 216, 226, 273, 274, 276, 281 |
| 5 | 25 (540 y 561 vacios) | 500, 502, 505-507, 522-525, 531-534, 539, 548, 551, 553, 567, 572, 573, 580, 581, 591 |
| 6 | 12 (676 y 677 vacios) | 659, 660, 662, 665, 667, 675, 679, 680, 682, 697 |
| 7 | 28 (741, 742, 755, 793 vacios) | 703, 705, 706, 714, 716, 720, 721, 723, 724, 731, 737, 740, 743-746, 749, 750, 760-762, 781, 792, 798 |
| 9 | 34 (991 vacio) | 900, 906, 908, 914, 916-919, 924, 927, 929, 930, 939, 943-945, 950, 951, 954, 956, 957, 963-965, 967, 969, 976, 978, 979, 983-985, 988 |

Total: 120 comandos del EXE pasan a C++ (7 despachadores, `RH_ScopedInstall` con `{ .stackArguments = 1 }`, hooks.csv reversed=1 validated=0). Segun `docs/ComandosScript.md`, los comandos `original` con despachador reescrito en `hooks.csv` pasan de 277 a 435 y los que siguen en el EXE bajan de 701 a 543 (quedan las ranuras 10-11, 13-14, 16, 18, 21-26 de otros agentes). `ProcessHandledCommand` da el mismo `OpcodeResult` que `ProcessOneCommand` para los comandos con manejador propio.

## 3. Detalles de fidelidad

Cada caso sigue el desensamblado (capstone, `reversing/FnAuditR28A/desens_exe.py`): mismo orden de `CollectParameters`, lecturas de `ScriptParams`, llamadas y truncados (`int16`/`uint8`); constantes de `.rdata` como `constexpr` con su direccion. Particularidades documentadas en el codigo: 152 usa `ExeCrt::Rand` (tres tiradas descartadas y la cuarta x 1/65536); 173 y 731 truncan con `_ftol` y se quedan el byte bajo; 591 reproduce `_ftol`/suma en doble redondeo; 723 deja la z de destino en 0 salvo z <= -100 (variable de pila no inicializada del EXE); 792 reutiliza el hueco de pila del strncpy; 750 deja la llamada de depuracion como comentario. Limites conocidos (sin validar, por indicacion): aritmetica `float` suelta puede diferir 1 ulp de la x87 a 24 bits (760, 761, 706, 927, 950); `new` de C++ no devuelve nulo (154, 965); 988 omite el 5.o argumento de `SetEntityBlip`.

Fuera de los archivos nuevos: `RunningScript.h/.cpp` (declaraciones, `ProcessHandledCommand`, enganches), `docs/hooks.csv` y `reversing/CmdBench/LiveComparison.hpp` (los siete despachadores se apagan junto a los demas para que el lado EXE del banco use el original). Conflictos esperados con otras ramas en esos tres archivos.

## 4. Verificacion

| Comprobacion | Resultado |
| --- | --- |
| Compilacion (`_build_VIVOSCRA.sh`, Debug x86, candado) | EXIT=0 (primer intento fallo solo por el miembro privado `CAnimManager::ms_numAnimAssocDefinitions` en 581; ahora `StaticRef<uint32>(0xB4EA28)`); pasan todos los `static_assert` de offsets de los 7 archivos |
| Humo ropero (`prueba_ropero.py cal_mal.args scra`) | BIEN (puerta y marcador) |
| Humo agua (`prueba_agua.py cal_mal.args scra`) | BIEN |
| `prueba_marcadores.py cal_mal.args scra` | BIEN (12 marcadores en memoria, 12 cambian en 1 s) |
| `audit_hook_catalog.py --strict` | OK |
| `audit_cpp_assembly.py` | 0 `__asm`/`naked` |
| Banco CmdBench por comando | no ejecutado (`build_probe.py` depende de `E:`, fuera del checkout parcial); los 7 despachadores quedan apagados en el lado EXE de `LiveComparison.hpp` |
| `inventario_comandos_script.py` | regenerado; se restauro el bloque `RESA-CURRENT-STATUS` de `ComandosScript.md` y se revirtio `reversing/CppOnlyMigration/summary.json` (efecto colateral de la auditoria) |

No se ejecuto una partida nueva con el EXE independiente (fase 8): lo comprobara quien integre junto con `ProcessCommands1000To1099`.

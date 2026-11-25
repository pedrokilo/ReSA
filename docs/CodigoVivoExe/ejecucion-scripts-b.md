<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ejecución SCRB: despachadores de comandos de script 1000-1499 (rama `vivo-scrb`)

Base: `origin/main` 59b1028202. Reconstruye en C++ con gancho activo (`RH_ScopedInstall(..., { .stackArguments = 1 })`) los cuatro despachadores por centenas que faltaban entre 1000 y 1499 (la 1200 ya existía). Sin validar comando a comando (regla vigente): compilación, auditorías, humo del juego y avance del EXE independiente.

| Ranura | Despachador | Dirección | Tamaño | Casos reconstruidos | Archivo |
| ---: | --- | --- | ---: | ---: | --- |
| 10 | `ProcessCommands1000To1099` | `0x489500` | 3.616 B | 27 | `RunningScriptCommands1000To1099.cpp` |
| 11 | `ProcessCommands1100To1199` | `0x48A320` | 4.720 B | 21 | `RunningScriptCommands1100To1199.cpp` |
| 13 | `ProcessCommands1300To1399` | `0x48CDD0` | 7.376 B | 19 | `RunningScriptCommands1300To1399.cpp` |
| 14 | `ProcessCommands1400To1499` | `0x48EAA0` | 8.976 B | 47 | `RunningScriptCommands1400To1499.cpp` |

Total: 114 comandos de estado `original` (sin manejador propio) reconstruidos desde el desensamblado. Declaraciones en `RunningScript.h`, ganchos en `RunningScript.cpp`, 4 filas nuevas en `docs/hooks.csv`.

## Criterio (igual que los despachadores P6B)

* Solo llevan caso los comandos del estado `original` de `docs/ComandosScript.csv`. Los reescritos, los `nop` y los no implementados no llevan caso porque `ProcessOneCommand` resuelve antes su manejador propio (ni se llega al despachador); el resto cae en `default` y devuelve `OR_INTERRUPT` (-1), que es lo que hace el EXE para los 50+ no implementados de cada ranura (los `nop` del EXE devuelven 0).
* Tablas de saltos: ranura 10 indirecta (`0x48A2B4` / `0x48A204`, base 1000), ranura 11 indirecta (`0x48B4D8` / `0x48B418`, base 1101: el 1100 va al `default`), ranura 13 indirecta (`0x48DDD4` / `0x48DD34`, base 1301), ranura 14 directa (`0x490C2C`, base 1403).
* Cada caso conserva el orden de `CollectParameters`/lecturas de `ScriptParams`/llamadas y las comparaciones x87 (p. ej. `CLEAR_AREA_OF_CHARS` ordena cada par solo con `>` ordenado; `CONVERT_METRES_TO_FEET_INT` redondea el producto una sola vez). Los casos de tarea (1465-1499) siguen los de `RunningScriptCommands1500To1599.cpp`.
* Cambios mínimos fuera de los archivos propios: `CTaskComplexEnterCarAsPassengerTimed::SetMoveState` (acceso público al campo privado que escribe el EXE en `TASK_ENTER_CAR_AS_PASSENGER_TIMED`).
* Las comprobaciones de desplazamientos crudos están como `static_assert` en cada archivo; los pocos campos sin miembro (p. ej. byte `0x3DB` de `CVehicle`, byte `0xB8` de `CPhysical`, `0x794` de la moto) se acceden por desplazamiento.

## Resultados

* Compilación (`_build_VIVOSCRB.sh`): `EXIT=0` tras corregir tipos y nombres de miembros en los archivos de 1100/1300/1400.
* `audit_hook_catalog.py --strict`: OK. `audit_cpp_assembly.py`: 0 `__asm`.
* Humo con el ASI de esta rama (`FnBIS1`, `cal_mal.args`): `prueba_ropero.py` BIEN, `prueba_agua.py` BIEN, `prueba_marcadores.py` BIEN (12 marcadores).
* EXE independiente (`gta_sa_standalone`, enlazado con los `.obj` de esta rama y `ejecutar_scrb.ps1`): la partida nueva **ya no rompe en `0x489500`**; sigue ejecutando el script de arranque durante 5 s más y rompe en `0x47FA30` (`ProcessCommands700To799`, ranura 7), cuyo despachador pertenece a otro agente (centenas 0-999). Registro: `reversing/FnFase8/ejecuciones/s1-20261122-000235`. Los despachadores 1100, 1300 y 1400 no llegaron a ejecutarse en esa pasada (el script no los alcanzó antes de la ruptura): sin evidencia dinámica propia, solo compilación y humo.

## Pendiente

* Bancos de validación (no se piden): ningún caso comparado contra el EXE.
* Si el script de arranque necesita un comando `original` de estas ranuras que no esté en la lista, el EXE independiente devolverá `OR_INTERRUPT` y el script se detendrá: revisar con el siguiente avance de la fase 8.
* Conflictos esperados con las demás ramas: `RunningScript.h`, `RunningScript.cpp`, `docs/hooks.csv`, `docs/ComandosScript.csv` y `.md` (regenerados con `contrib/inventario_comandos_script.py`; el `.md` conserva el encabezado de estado).

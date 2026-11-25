<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# SCRC: despachadores de comandos de script 1600-2699

Rama `vivo-scrc` (desde `origin/main` 9acc2209e0, 21/11/2026). Contexto: el EXE independiente (fase 8) rompe al empezar una partida nueva en un despachador de script sin reconstruir. El EXE despacha los comandos por centenas (tabla 0x8A6168); aquí se reconstruyen en C++ con hook activo (`RH_ScopedInstall(..., { .stackArguments = 1 })`, mismo patrón que `ProcessCommands2000To2099`) las centenas que faltaban entre 1600 y el último comando del juego.

## 1. Qué se hizo

La tabla 0x8A6168 tiene 27 ranuras útiles (0..2600); la ranura 2700 en adelante es basura (el último comando del juego es 2638). Faltaban 8 despachadores de mi tramo (1700, 1900 y 2000 ya estaban):

| Centena | Dirección | Tabla de saltos | Comandos | Con cuerpo | Vacíos (OR_CONTINUE) | OR_INTERRUPT explícito | Archivo |
| --- | --- | --- | ---: | ---: | ---: | ---: | --- |
| 1600 | 0x493FE0 | 0x495E00 | 1602-1699 (98) | 69 | 18 | 11 (+1600, 1601) | RunningScriptCommands1600To1699.cpp |
| 1800 | 0x46D050 | 0x46EDB4 | 100 | 72 | 23 | 5 | RunningScriptCommands1800To1899.cpp |
| 2100 | 0x470A90 | 0x472178 | 100 | 68 | 30 | 2 | RunningScriptCommands2100To2199.cpp |
| 2200 | 0x474900 | 0x476140 | 100 | 88 | 12 | 0 | RunningScriptCommands2200To2299.cpp |
| 2300 | 0x4762D0 | 0x477C0C | 100 | 82 | 18 | 0 | RunningScriptCommands2300To2399.cpp |
| 2400 | 0x478000 | 0x479AFC | 100 | 89 | 11 | 0 | RunningScriptCommands2400To2499.cpp |
| 2500 | 0x47A760 | 0x47BF4C | 100 | 87 | 10 | 3 | RunningScriptCommands2500To2599.cpp |
| 2600 | 0x479DA0 | 0x47A6C4 | 2600-2638 (39) | 36 | 3 | 0 (default) | RunningScriptCommands2600To2699.cpp |

Notas: el despachador 1600 resta 1602 (`lea eax,[ebp-642h]`), así que 1600 y 1601 caen en `default` (OR_INTERRUPT). Se implementaron todos los casos del EXE, incluidos los de comandos con manejador propio (el despachador del EXE los contenía). Se declaran en `RunningScript.h`, se registran en `RunningScript.cpp` y en `docs/hooks.csv` (reversed=1, validated=0).

Desviaciones respecto de los manejadores propios (manda el EXE): GET_CHAR_AREA_VISIBLE (2536) devuelve el id de área, no bool; 2526 IS_CHAR_GETTING_IN_TO_A_CAR es `(driver||passenger) && !GO_TO_CAR_DOOR`; SET_OBJECT_ANIM_CURRENT_TIME (2106) usa `m_fTotalTime`; SET_CHAR_ALLOWED_TO_DUCK (2134) y SET_CHAR_DROPS_WEAPONS_WHEN_DEAD (2174) con lógica invertida como el EXE; SET_ONLY_CREATE_GANG_MEMBERS (2400s) escribe `== 0`.

Efectos fuera de los ocho archivos: `TaskComplexUseMobilePhone.h/.cpp` exponen `CTaskComplexUseMobilePhoneConstructorExe` (0x6348A0) porque la clase sigue abstracta (comando 1800s TASK_USE_MOBILE_PHONE). Puentes por dirección (`plugin::CallAndReturn`): 0x6002F0 (`FindAttractorInRange`, estática sin declaración) en 2208 y 2332. Lecturas por dirección con `StaticRef`: tienda (0xA9A7F0, 0xA9A318; privados en `CShopping`), banderas varias de `.bss`; varios campos con `RawField` donde no hay miembro nombrado. Sin `__asm`.

## 2. Verificación

| Comprobación | Resultado |
| --- | --- |
| Compilación (`_build_VIVOSCRC.sh`, Debug x86, candado) | EXIT=0 (tras corregir miembros privados, `static_assert` de desplazamientos y un constructor abstracto) |
| Humo ropero (`prueba_ropero.py cal_mal.args scrc`) | BIEN (puerta y marcador) |
| Humo agua (`prueba_agua.py cal_mal.args scrc`) | BIEN |
| `prueba_marcadores.py cal_mal.args scrc` | BIEN (12 marcadores que cambian) |
| `audit_hook_catalog.py --strict` | OK |
| `audit_cpp_assembly.py` | 0 `__asm`/`naked` |
| CmdBench | no ejecutado (el banco usa fuera del checkout parcial la copia de `E:`) |

`docs/ComandosScript.csv`/`.md` regenerados con `contrib/inventario_comandos_script.py`: comandos *original* con despachador reescrito 277 -> 755; despachador del EXE 701 -> 223 (el bloque `RESA-CURRENT-STATUS` de `.md` se restauró). Los demás sin despachador (185) son las centenas 0-999 y 1000-1499 de otros agentes.

## 3. Riesgos conocidos (sin validar comando a comando)

* Aritmética flotante suelta en `float`: posible 1 ulp frente al x87 a 24 bits.
* Etiquetas de texto: buffers inicializados a cero (el EXE los deja sin inicializar); algunas etiquetas de 8 caracteres terminan en NUL propio.
* El EXE comprueba el resultado nulo de `CTask::operator new`; en C++ no existe esa rama.
* Se eliminaron `static_assert` de offsets de miembros privados/protegidos (`m_pRwObject`, `m_AreaCode`, `m_nCreatedBy`, `CGarage::m_nFlags`, `sizeof(CPed)`); se usan los accesores públicos.
* 1801/1866 replican literalmente el orden extraño de `chances/flags` del EXE; revisar contra un banco.
* Conflictos esperados con otros agentes en `RunningScript.cpp/.h` y `docs/hooks.csv` (no resueltos).

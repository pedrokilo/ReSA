<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Dependencies25E — 16 funciones de biblioteca del EXE (RenderWare y CRT estática de MSVC), lote 25 grupo E

Banco par a par, dentro del juego, de las funciones de la biblioteca de `reversing/OriginalDependencies2026-10-17/` (nivel «1 Muy fácil», ≤ 40 bytes) que se reconstruyeron en C++ puro:
EXE (hook RH apagado) frente a DLL (hook RH encendido), 1024 pares por función, 16384 pares. La 17.ª fila del lote (`QuickTime dispatch thunk` 0x819840) NO se reconstruye ni está en este banco
(no es viable en C++ puro; ver `g25e-classification.md`).

| Clase RH | Función | Entrada | Fuente (parche) |
| --- | --- | --- | --- |
| RwCore | `RxHeapGetGlobalHeap` | 0x8052F0 | `rwcore.cpp` |
| RwCore | `_rwFrameListDeinitialize` | 0x807720 | `rwcore.cpp` |
| RwCore | `RwD3D9RasterIsCompressed` | 0x820C90 | `rwcore.cpp` |
| RwPlCore | `_rwPluginRegistryGetPluginOffset` | 0x808470 | `rwplcore.cpp` |
| RwPlCore | `_rwSListDestroyEndEntries` / `GetNumEntries` / `GetEntry` / `GetBegin` / `GetEnd` | 0x809400 / 0x8094B0 / 0x809510 / 0x809530 / 0x809540 | `rwplcore.cpp` |
| RwPlCore | `_rwStringClose` / `_rwStringStreamGetSize` | 0x80A440 / 0x80A470 | `rwplcore.cpp` |
| CRT | `CrtAtexit` (`_atexit`) | 0x821D1E | `VehicleRecording.cpp` (junto a `CrtPrintf`) |
| CRT | `CrtLockFile2` / `CrtUnlockFile2` (`_lock_file2` / `_unlock_file2`) | 0x823FD1 / 0x824023 | `VehicleRecording.cpp` |
| CRT | `CrtMalloc` (`malloc` de la CRT del EXE; el catálogo de partida lo llamaba `CMemoryMgr::Malloc`, que es 0x72F420) | 0x824257 | `VehicleRecording.cpp` |
| CRT | `CrtErrno` (`_errno`) | 0x82B181 | `VehicleRecording.cpp` |

Todas son `cdecl` con `ret` sin parámetros. El hook de cada una es un `RH_ScopedGlobalInstall` (clases `RwCore`, `RwPlCore` y `CRT` del catálogo; `HookRoute` las localiza con `RegisteredTarget`).
Ningún llamador del EXE lee ECX/EDX tras ninguna de ellas, ni EAX tras las `void` (`_rwSListDestroyEndEntries`, `_rwStringClose`, `_lock_file2`, `_unlock_file2`): los llamadores se
desensamblaron y siguieron por todas las ramas (`g25e-classification.md`, sección «Residuo de registros»). Por eso no se usa `ResidueRegsCpp` y ECX/EDX solo se registran.

## Técnica

- Las funciones RenderWare se reconstruyen sobre los campos que lee el EXE (disposición de `RwSList`, `rwFrameList`, `RwPluginRegistry`/`RwPluginRegEntry`, `RwGlobals`) con aritmética de 32 bits con envoltura
  (el EXE usa `imul`/`add`/`sub` de 32 bits; en C++ se hace con `uint32`, sin desbordamiento con signo).
- Las funciones CRT trabajan sobre la CRT **del EXE** (datos y funciones internas por dirección): `_onexit` 0x821CE6, `_lock`/`_unlock` 0x82ADBE/0x82AD12, `_nh_malloc` 0x82422B + `_newmode` 0xC9ADCC,
  `_getptd` 0x827B3D y las entradas de importación `EnterCriticalSection`/`LeaveCriticalSection` (0x8580EC/0x8580E8, verificadas en la tabla de importación del PE). No se mezcla la CRT (UCRT) de la DLL.
- No se tocó ensamblador ni `naked` (`contrib/audit_cpp_assembly.py` da 0 apariciones).

## Qué se compara en cada par (todo en bits; original == candidato)

- Valor de retorno (EAX) de las 12 funciones con resultado; para `CrtMalloc` las clases del puntero (nulo y alineación a 8; la alineación a 16 solo se registra como `info_align16`, ver «Alineación del puntero de malloc») porque el puntero real difiere entre lados (cada lado asigna y libera su propio bloque).
- Memoria: el arena propio del banco completo (guardas de 16 bytes a cada lado), la variable global que cada función lee o escribe (0xC9A6B0, 0xB4E9E0, `_newmode`, errno del hilo, la tabla de
  cerrojos 0x8E31C0 y las 20 secciones críticas de flujo) justo tras la llamada, y la traza de los colaboradores doblados.
- ESP, EBX/ESI/EDI/EBP y x87 completo: FNSAVE de 108 bytes tras la llamada (TOP, FCW, FSW, FTW, FIP/FCS/FDP/FDS y los diez bytes de cada registro ST, vacíos incluidos) y MXCSR. Antes de cada llamada `PrimeX87()` deja
  un residuo conocido en los ocho registros. `kCompareDeadX87Registers = true` y no se relaja. Ninguna función ejecuta x87: el FIP debe ser el del último `fstp` de `PrimeX87` en ambos lados.
- Semillas distintas por caso en EAX/ECX/EDX antes de la llamada. ECX/EDX al volver se registran (`*_residue`) sin exigir igualdad (nadie los lee).
- `run_local.py` compara además cada lado con un oráculo independiente en Python (`fixture_oracle.py`, hecho a partir del desensamblado): EAX, arena final, traza, y los campos propios (estado del cerrojo,
  clases del puntero de `malloc`, `eax - ptd` y relectura de errno).

## Colaboradores y su tratamiento

| Colaborador | Funciones que lo usan | Tratamiento |
| --- | --- | --- |
| `rwfree` `[engine+0x138]` | `_rwFrameListDeinitialize` | puntero del motor sustituido en ambos lados por el mismo doble trazado; se restaura |
| `rwstrlen` `[engine+0x120]` | `_rwStringStreamGetSize` | ídem (devuelve la longitud del caso y traza el argumento) |
| `_onexit` 0x821CE6 | `CrtAtexit` | `EntryPatch` en la dirección del EXE (la DLL lo llama por esa misma dirección) en ambos lados; sin el doble `atexit` registraría una función del banco para el cierre del proceso |
| manejador de falta de memoria `[0xC9AC68]` | `CrtMalloc` | doble trazado que devuelve 0 (malloc devuelve NULL sin reintentar); se restaura |
| `_lock`/`_unlock`, `_getptd`, `_nh_malloc`, `EnterCriticalSection`/`LeaveCriticalSection` | `CrtLockFile2`, `CrtUnlockFile2`, `CrtErrno`, `CrtMalloc` | **reales** (otros hilos los usan; parchearlos sería inseguro). Se calienta antes (se crean las 20 secciones críticas de flujo 16+n y el montón) y se observa/restaura su estado |

Para los índices ≥ 20 de `_lock_file2`/`_unlock_file2` se usa una sección crítica propia del banco (`stream = &sección - 0x20`).

## Fixtures (Fixtures.hpp; fixture_oracle.py las repite; 1024 casos por función)

- **Palabras de estrés** (75 % tabla de 32 valores, 25 % aleatorias): 0, 1, 2, 3, 4, 7, 8, 0x7F, 0x80, 0xFF, 0x100, 0xFFFF, 0x10000, 0x3FFFFFFF, 0x40000000, 0x7FFFFFFF, 0x80000000, 0x80000001, 0xC0000000, 0xFFFF0000,
  0xFFFFFFFC..0xFFFFFFFF, 0x55555555, 0xAAAAAAAA, 0xDEADBEEF, direcciones del EXE usadas por las funciones…
- `RxHeapGetGlobalHeap`: el valor que se escribe en 0xC9A6B0.
- `_rwFrameListDeinitialize`: `numFrames` = 0 en 163 casos, `frames` = NULL con `numFrames` ≠ 0 en 113 (el EXE comprueba `numFrames`, no el puntero); traza de `rwfree`.
- `RwD3D9RasterIsCompressed`: los 256 valores del byte de banderas × 4 desplazamientos de extensión (0, 0x30, −0x50 y aleatorio) con envoltura de 32 bits; bytes vecinos de basura.
- `_rwPluginRegistryGetPluginOffset`: 0..8 entradas en ranuras permutadas, ids de la tabla {1, 0x10007, 0xFFFFFFFF, 0x80000000, 0x20, 0} con duplicados (574 casos), id presente/ausente/aleatorio, lista vacía (108).
- `_rwSList*`: `listElements`, `numElementsFilled`, `entrySize` y argumento de estrés (envoltura de `entrySize*arg` en 658 casos, `filled < amount` en 513, `entrySize = 0` en 24) más un cuarto de casos plausibles.
- `_rwStringClose`: sin estado (arena de basura que no debe tocar); `_rwStringStreamGetSize`: cadena nula (121) o del arena, longitud de estrés (80 casos con envoltura de `length + 4`).
- `CrtAtexit`: `func` de estrés (no se llama) y resultado del `_onexit` doblado: 0 (268) o no nulo (756).
- `CrtLockFile2`/`CrtUnlockFile2`: 640 casos con los 20 cerrojos de flujo reales (índice 0..19, 32 veces cada uno, incluido el límite 19) y 384 con índices ≥ 20 (20, 21, 22, 23, 24, 31, 32, 33, 63, 64, 100, 127, 128, 255,
  256, 1000, 65535, 65536, 0x7FFFFFFE, 0x7FFFFFFF) sobre la sección crítica del banco.
- `CrtMalloc`: 640 tamaños que el montón satisface (0, 1, 2, … 4097, 8192, 65535..65537, 1 MiB; el bloque se escribe y se libera con el `free` del EXE) y 384 que no puede satisfacer (0xC0000000, 0xF0000000, 0xFFFF0000,
  0xFFFFF000, 0xFFFFFFDF, 0xFFFFFFE0, 0xFFFFFFE1, 0xFFFFFFFF) con `_newmode` ∈ {0, 1, 2, 0x80000000, 0xFFFFFFFF, 0x100} y manejador doblado: se exige la traza `[size]` solo con `_newmode ≠ 0` y tamaño ≤ 0xFFFFFFE0 (por encima el
  EXE sale antes sin llamar al manejador).
- `CrtErrno`: el valor que se escribe en `_getptd()+8` es de estrés (25 ceros, 346 con el bit alto); se exige `eax - ptd == 8` y que la lectura a través del puntero devuelva ese valor.

## Alineación del puntero de malloc

La primera ejecución (corrida 20261017-211334) dio 148 de 1024 pares con una sola diferencia, `align16` (0 en un lado y 8 en el otro, con el mismo tamaño y en ambos sentidos; todos los tamaños satisfechos aparecen con 0 y con 8 también dentro de un mismo lado).
Causa: no es un defecto de `CrtMalloc` (los dos lados llaman a la misma `_nh_malloc`). `_heap_alloc` (0x8241B0) solo redondea el tamaño a 16 si `[0xC9C2F8] != 1` y usa el montón pequeño solo si vale 3 y el tamaño está bajo el umbral;
en cualquier otro caso devuelve lo que da `HeapAlloc` (0x8581F8), que solo garantiza alineación a 8. Qué dirección sale (múltiplo de 16 o no) depende de las listas libres del montón, es decir de las asignaciones anteriores del juego y de los
bloques liberados por el par previo, y no de los argumentos. Por eso `align16` no es una propiedad de la función y se deja de comparar (campo `info_align16`, solo informativo); `align8` (0 obligatorio) y la nulidad siguen comparándose y exigiéndose al oráculo.

## LockCount de las secciones críticas

`LockCount` depende del sistema operativo: libre = -1; tomada una vez sin espera = 0 antes de Windows 8 y **-2** desde Windows 8 (el bit 0 pasa a ser el indicador de «tomada» y cada espera resta 4). La primera corrida del oráculo
suponía 0 y vio -2 en el cerrojo 0 (`stdin`): es la codificación del SO, no otro hilo (una espera daría -6). El banco mide en `begin` (`cs_unlocked_lock_count`, `cs_locked_lock_count`) con su propia sección, sin otros hilos, y
`run_local.py` exige que el par medido sea (-1, 0) o (-1, -2); `CrtLockFile2` debe dejar ese valor «tomada» con `RecursionCount == 1` y dueño este hilo, y `CrtUnlockFile2` el valor «libre» con recursión 0 y sin dueño. Se mantiene la igualdad EXE/DLL
del registro completo de la sección. Si un hilo ajeno tomase un cerrojo de flujo, el valor se desviaría y el caso fallaría (no se excluye ningún cerrojo).

## Límites (qué NO cubre)

- `_lock_file2`/`_unlock_file2` con índice negativo (fuera del dominio del CRT: indexaría fuera de la tabla de cerrojos); solo se prueban 0..19 y ≥ 20.
- `malloc` con un `_newmode` ≠ 0 y un manejador real que sí reintenta; el valor numérico del puntero no se compara (difiere por naturaleza).
- `_atexit`: se dobla `_onexit`; el registro real en la tabla `atexit` del EXE no se ejercita (se vería al cerrar el proceso).
- No hay trazado de ramas de instrucciones: la cobertura la fijan las entradas serializadas más el oráculo independiente.
- **El banco no ha sido ejecutado dentro del juego** (encargo de solo análisis). Sí se hizo: (1) compilación de `LiveComparison.hpp` con las opciones del proyecto sin errores; (2) el arnés aislado `offline/`
  (los bytes REALES de las 16 funciones del EXE en un bloque reubicado frente a la reconstrucción C++ extraída tal cual del parche, con las mismas fixtures y los colaboradores doblados): EXE == C++ == oráculo en los
  16384 pares, compilado en Debug (`/Od /RTC1`) y en `/O2`, y detecta mutaciones de la reconstrucción; (3) `offline/synth_log.py`: registro sintético que el analizador acepta y 13 mutaciones que rechaza.

## Ejecución

```
python -B reversing/Dependencies25E/fixture_oracle.py                 # cobertura declarada de las fixtures
python -B reversing/Dependencies25E/build_probe.py
python -B reversing/Dependencies25E/run_local.py
python -B reversing/Dependencies25E/run_local.py --analyze <comparisons.jsonl>
```

o `python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpeta de salida> Dependencies25E`.

Comprobaciones sin juego: `offline/` (`python gen_exebytes.py`, `sh build.sh Od|O2`, `harness_Od.exe > salida.jsonl`, `python check_offline.py salida.jsonl`, `python synth_log.py`).

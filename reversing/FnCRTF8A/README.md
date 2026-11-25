<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FnCRTF8A: arranque y cierre del CRT propio, cuerpos de `std::exception` (21/11/2026)

Parte CRT de la fase 8 (rama `vivo-crtf8`, sin subir). Decisión vigente: se reconstruye en C++, se engancha cuando tiene sentido, se compila y se comprueba que no rompe; no se valida función por función. El juego no se ejecutó para este banco: es un banco **fuera del juego** con el método de `FnCRT1A`–`FnCRT5A` (el `gta_sa.exe` original, solo lectura, se mapea en `0x400000` dentro de un proceso de 32 bits y se llama por dirección; el estado del CRT vive en las mismas direcciones del EXE y es el mismo para las dos versiones).

## Qué compara

| Sección | EXE | C++ | Qué se compara |
|---|---|---|---|
| `dtor` | `0x822E83`, `0x822EBF`, `0x822EFA`, `0x822F94` (cuerpos) y `0x822F40`, `0x822F5C`, `0x822F78`, `0x822FBD` (con borrado escalar, bandera 0 y 1) | `ExeCrt::DtorBody822E83`/`EBF`/`EFA`/`F94` y `DeletingDtor822F40`/`F5C`/`F78`/`FBD` | los 12 bytes del objeto (vtable final, texto nulo o no, bandera), los bloques que libera el montón del CRT (montón del EXE y montón propio por separado), el puntero devuelto |
| `args` | `__setargv` `0x82DB26` (con `parse_cmdline` `0x82D9BA` dentro) | `ExeCrt::Setargv` | código de retorno, `__argc`, cada cadena de `__argv`, el nulo final y `_pgmptr`; líneas aleatorias con comillas, barras, tabuladores, líneas vacías o nulas, nombre entre comillas, y tabla `_mbctype` vacía o con bytes iniciales de doble byte (0x81–0x9F, 0xE0–0xFC); fallo de `malloc` inyectado en la 1.ª y la 2.ª reserva |
| `env` | `__crtGetEnvironmentStringsA` `0x82DBC8` y `__setenvp` `0x82D8F3` | `ExeCrt::CrtGetEnvironmentStrings`, `ExeCrt::Setenvp` | contenido del bloque de entorno con `f_use` en 0, 1 y 2 (API ancha y ANSI), `f_use` final, fallo de memoria; `Setenvp` sobre copias iguales del bloque con fallo inyectado en cada reserva posible: retorno, vector `_environ`, `_env_initialized`, `_aenvptr` liberado o no; sin bloque |
| `exit` | `_doexit` `0x823BDB` con `retcaller = 1` | `ExeCrt::DoExit` | orden de las llamadas de la tabla de `atexit`/`_onexit` (incluida una función que registra otra mientras se ejecuta: el extremo se relee), de las tablas `0x8A5A34..0x8A5A3C` y `0x8A5A40..0x8A5A48` (sustituidas por registradores), con `quick` 0 y 1, banderas `0xC9AC44` y `0xC9AC40` y el extremo final de la tabla |

No se puede comparar con `retcaller = 0` (sale por `CorExitProcess`/`ExitProcess` y terminaría el banco): ese tramo (`__crtExitProcess` `0x823AFE` y la rama final de `_doexit`) se sigue del desensamblado y se verifica por inspección.

El banco incluye `CrtString.cpp`, `CrtMem.cpp`, `CrtHeap.cpp` (con un `HeapAlloc` que falla en la reserva n-ésima, el mismo para el EXE por su IAT), `CrtState.cpp`, `CrtExceptionDtor.cpp` y `CrtOnexit.cpp`, y compila `CrtStartup.cpp` aparte (sus ayudantes anónimos chocan con los de `CrtState.cpp`). Se sustituyen por vacío `Ioinit`, `Initstdio`, `RunX87StaticInitializers`, `CurrentLocInfo` e `IsctypeMt` (no se llaman) y `StdException::Destruct` por su cuerpo del EXE (`0x822E2F`, ya comparado en `FnCRT3A`); `operator delete` global va al del CRT del EXE cuando el bloque es del montón del CRT.

## Uso

```
reversing\FnCRTF8A\build_bench.cmd D:\Original\_crtf8a_bench
D:\Original\_crtf8a_bench\bench_crtf8a.exe <gta_sa.exe> [--casos N] [--semilla S] [--solo dtor|args|env|exit]
sh reversing/FnCRTF8A/neg_run.sh        # prueba negativa: 15 mutaciones de una línea
```

`D:\Original\_crt1a_work\gta_sa.exe` es el EXE canónico (SHA-256 `72ae59e4…ecc9b`) sin espacios en la ruta.

## Resultado

`--casos 5000 --semilla 7`: **315.764 comparaciones, 0 diferencias** (dtor 180.000, args 53.322, env 77.442, exit 5.000); también con `--casos 3000` (semilla 1) y `--casos 200`.

Prueba negativa (`neg_run.sh`), cada mutación de una línea del C++ se recompila y el banco tiene que detectarla:

| # | Mutación | Resultado |
|---:|---|---|
| 1 | `DtorBody822E83` sin llamar a `Destruct` | detectada (vtable `0x887EA0` en vez de `0x887E80`) |
| 2 | vtable de `DtorBody822F94` `0x887EC4` → `0x887EC0` | detectada |
| 3 | `DtorBody822F94` sin liberar el texto | detectada (bloques liberados 1 / 0) |
| 4 | `DeletingDtor822F78` con el cuerpo de `0x822F94` | detectada |
| 5 | `ParseCmdline`: `numSlash /= 1` | detectada |
| 6 | comillas dobles dentro de comillas sin comprobar `inQuote` | detectada |
| 7 | nombre del programa: sin retroceder `p` al final de la línea | detectada |
| 8 | `__argc = numArgs` (sin restar 1) | detectada (el banco termina con violación de acceso al recorrer `argv`) |
| 9 | `Setenvp` copia también las variables que empiezan por `=` | detectada (un elemento de más en `_environ`: 112 / 113; con 1.500 casos el banco termina sin resultado por corrupción del montón) |
| 10 | `_env_initialized = 0` | detectada |
| 11 | `__crtGetEnvironmentStringsA` sin el `+ 1` del recuento ancho | detectada |
| 12 | `DoExit` sin la tabla `0x8A5A34..0x8A5A3C` | detectada |
| 13 | `0xC9AC40` siempre 0 | detectada |
| 14 | el extremo de la tabla de `atexit` no se relee tras cada llamada | detectada |
| 15 | `0xC9AC44` = 0 | detectada |

Mutaciones equivalentes (no observables) que se descartaron al preparar la prueba: fijar la vtable intermedia de `DtorBody822E83` sin llamar al destructor sí se ve (mutación 1), pero cambiar solo el valor intermedio cuando después `Destruct` la sobrescribe no cambia ningún byte; y `DtorBody822EBF`/`DtorBody822EFA` son el mismo cuerpo (una copia por clase en el EXE).

## Lo que no cubre el banco

* `__initmbctable` `0x833494` (`_setmbcp(-3)`, llena `_mbctype` con la página de códigos ANSI) **no está reconstruida** (la lleva la rama del locale: mbcs); `Setargv` lee `_mbctype` tal como esté. El banco prueba las dos situaciones (tabla vacía y tabla de doble byte) con el mismo contenido para el EXE y el C++.
* `exit` con salida real y el camino de `TerminateProcess` por salida reentrante.

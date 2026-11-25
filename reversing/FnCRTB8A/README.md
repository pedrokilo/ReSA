<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FnCRTB8A: restos del CRT del arranque propio (21/11/2026)

Parte CRT-B de la fase 8 (rama `vivo-crtb`, sin subir). Banco **fuera del juego** con el método de `FnCRT1A`–`FnCRT5A` y `FnCRTF8A`: el `gta_sa.exe` original (solo lectura) se mapea en `0x400000` dentro de un proceso de 32 bits y se llama por su dirección; el estado del CRT vive en las mismas direcciones del EXE y es el mismo para las dos versiones; cada caso se prepara igual y se ejecuta con el EXE y después con el C++, cada uno sobre su propio montón (uno nuevo por caso). El juego no se ejecuta.

## Qué compara

| Sección | EXE | C++ | Qué se compara |
|---|---|---|---|
| `mbcs` | `_setmbcp` `0x833344`, su núcleo `0x83319E`, `setSBCS` `0x832F74`, `setSBUpLow` `0x832F9D` (con `0x832F45` dentro), `__initmbctable` `0x833494`, `__updatetmbcinfo` `0x83312F`, `__mbschr` `0x836697`, `_strncnt` `0x834B6E` (usercall, con un adaptador de 17 bytes) | `ExeCrt::Setmbcp`, `SetmbcpCore`, `SetSbcs`, `SetSbUpLow`, `InitMbcTable`, `UpdateTMbcInfo`, `Mbschr`, `Strncnt` | valor de retorno, los 0x240 bytes de estado `0xC9C080..0xC9C2C0` (`__mblcid`, `__ismbcodepage`, `_mbctype`, `__mbcodepage`, `__mbulinfo`, mapa de caja; `__ptmbcinfo` normalizado), `_bSystemSet`, bandera de inicialización, contenido y reutilización del bloque global y del bloque del hilo (`ptd+0x60`), bloques que reserva y libera el montón, validez del montón tras cada operación |
| `tablac` | `0x826A72`, `0x8289F5` (con `0x8289C0`), `0x82AC54`, su filtro `0x82AC0E` (con `0x832ECF`), `__security_init_cookie` `0x8339CA`, `__fpmath` `0x821397` (con `0x82134F`, `0x826F04`, `0x826EC4`) | `TableC826A72`, `TableCSseAvailableInit`, `TableCUnhandledFilterInit`, `CrtUnhandledExceptionFilter`, `InitSecurityCookie`, `FpMath` | los 16 bytes alrededor de `0x8E37B4`; `0xC9C400/404` y el retorno; filtro anterior guardado en `0xC9AC74` y filtro que se instala (el del EXE `0x82AC0E` y el propio); retorno del filtro y llamadas al filtro anterior con anterior nulo, válido o inválido y registros de excepción variados; cookie `0x8E31BC` con las cinco llamadas del sistema fijadas (incluido el caso que sale 0 → `0xBB40E64E`); `__fpmath`: los 6 punteros de `0x8E30F0`, `_adjust_fdiv` y la palabra de control x87 (precisión y redondeo iniciales aleatorios) con `IsProcessorFeaturePresent` real, propia (con y sin el defecto) o ausente (prueba FDIV) |

Se interponen por la IAT del EXE y por macros del C++ (`bench_apistubs.h`): `GetCPInfo` (con fallo inyectable en la llamada n-ésima), `GetACP`, `GetOEMCP`, `GetSystemTimeAsFileTime`, `GetCurrentProcessId`, `GetCurrentThreadId`, `GetTickCount`, `QueryPerformanceCounter`, `SetUnhandledExceptionFilter` y `HeapAlloc` (fallo inyectable). `CrtLCMapStringA` y `CrtGetStringTypeA` del banco llaman al cuerpo del EXE (`0x82BB1E`, `0x830FB8`; ya comparados en `FnCRT4A`) para que las dos versiones usen el mismo ayudante.

Decisiones del banco (para que cada diferencia sea de verdad del C++ y no ruido): los LCID del estado inicial son válidos y la página de locale es de un byte (con un LCID no válido o una conversión que falla `LCMapStringA`/`GetStringTypeA` no escriben y queda basura de pila distinta en cada versión); se quita la página 54936 (GB18030) por lo mismo. La rama de `terminate` del filtro (excepción de C++ `0xE06D7363` con 3 parámetros) no se compara: el EXE llama a su `terminate` y el C++ al del CRT enlazado, por decisión (ver `docs/CodigoVivoExe/ejecucion-crt-fase8b.md`).

## Uso

```
reversing\FnCRTB8A\build_bench.cmd D:\Original\_crtb8a_bench
D:\Original\_crtb8a_bench\bench_crtb8a.exe <gta_sa.exe> [--casos N] [--semilla S] [--solo mbcs|tablac]
sh reversing/FnCRTB8A/neg_run.sh        # prueba negativa: 21 mutaciones de una línea
```

`D:\Original\_crt1a_work\gta_sa.exe` es el EXE canónico sin espacios en la ruta. Con `BENCH_TRACE=1` imprime cada caso y cada pasada.

## Resultado

`--casos 5000 --semilla 7`: **190.000 comparaciones, 0 diferencias** (`mbcs` 150.000, `tablac` 40.000); también `--casos 2000` con las semillas 1 y 3 (72.000 comparaciones cada una) y `--casos 2000 --semilla 5 --solo tablac` (16.000), 0 diferencias.

Prueba negativa (`neg_run.sh`): cada mutación de una línea del C++ se recompila y el banco tiene que detectarla. **Las 21 detectadas.**

| # | Archivo | Mutación | Resultado |
|---:|---|---|---|
| 1 | CrtMbcs | tabla de páginas con 4 entradas en vez de 5 | detectada (página 1361) |
| 2 | CrtMbcs | LCID del coreano `0x412` → `0x413` | detectada |
| 3 | CrtMbcs | bytes finales `c < 0xFF` → `c < 0xFE` | detectada |
| 4 | CrtMbcs | reutilizar el bloque global cuando su cuenta es 0 → cuando no lo es | detectada (bloques 1/0) |
| 5 | CrtMbcs | sin el atajo «misma página» de `_setmbcp` | detectada |
| 6 | CrtMbcs | `__mbschr` sin el caso del byte inicial al final de la cadena | detectada |
| 7 | CrtMbcs | `_strncnt` devuelve `count - left` | detectada (9/10) |
| 8 | CrtMbcs | `setSBUpLow` sin `chars[0] = 0x20` | detectada |
| 9 | CrtMbcs | `__updatetmbcinfo` sin incrementar la cuenta | detectada |
| 10 | CrtMbcs | `i + 0x20` → `i + 0x21` en el mapa de caja de respaldo | detectada |
| 11 | CrtMbcs | `_setmbcp` sin liberar el bloque reservado si falla | detectada (bloques) |
| 12 | CrtMbcs | rangos de bytes iniciales `c <= high` → `c < high` | detectada |
| 13 | CrtStartup | tabla `0x826A72`: 7 bytes en vez de 8 | detectada |
| 14 | CrtStartup | `0xC9C404` queda a 0 | detectada |
| 15 | CrtStartup | `IsValidCodePointer` invertida | detectada |
| 16 | CrtStartup | cookie por defecto `0xBB40E64F` | detectada |
| 17 | CrtStartup | cookie sin `GetTickCount` | detectada |
| 18 | CrtStartup | no se guarda el filtro anterior en `0xC9AC74` | detectada |
| 19 | CrtStartup | `FpMath`: último puntero `0x826E62` | detectada |
| 20 | CrtStartup | `FpMath`: precisión `0x10000` (`_PC_53`) → `0x20000` (`_PC_24`) | detectada |
| 21 | CrtStartup | `DetectFdivBug` niega la respuesta de la API | detectada |

El primer intento de banco dio falsas diferencias (no del C++): LCID no válidos y conversiones de páginas que fallan (por ejemplo cp 0 con locale de doble byte, o GB18030) dejan sin escribir las salidas de `LCMapStringA`/`GetStringTypeA`, y entonces queda basura de pila distinta en cada versión; se corrigió el estado inicial del banco, no el C++.

## Lo que no cubre el banco

* La rama de `terminate` del filtro de excepciones (ver arriba) y el `__try/__except` de la prueba de SSE2 cuando la instrucción *sí* falla (este equipo tiene SSE2).
* `Cinit` completa: ejecutaría los 1.668 inicializadores del EXE. Se comprobó ejecutando el ejecutable independiente (`docs/CodigoVivoExe/ejecucion-crt-fase8b.md`, sección 5).

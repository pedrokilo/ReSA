<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Dependencies21A — CRT::CrtPrintf (printf del CRT del EXE, 0x00821982)

Entrada 0x821982, cohorte de los 21 inciertos (grupo A). Clasificación: **biblioteca (CRT estática de MSVC, `printf` de `char*`, no `wprintf`)**; ver `ga-classification.md`.
`printf.dis.txt` es el desensamblado completo del EXE (envoltorio y ayudantes).

## Técnica

- Fuente: `CrtPrintf` (estática, `source/game_sa/VehicleRecording.cpp`, que ya contenía el envoltorio `plugin::Call<0x821982>`) y hook `ReversibleHooks::Install("CRT/CRT", "CrtPrintf", 0x821982, &CrtPrintf)`.
  Reproduce el envoltorio de 0x821982: `_lock_file2(1, stdout)` (0x823FD1), `try { _stbuf (0x827DEC); n = _output (0x827F4B)(stdout, fmt, va_list); _ftbuf (0x827E74) } finally { _unlock_file2 (0x824023) }`, con `__try/__finally` de C++ (sin `__asm`).
- El motor de formato (`_output`, 0x827F4B) NO se reimplementa: depende de las tablas de localización, de la conversión de coma flotante del CRT del EXE y del estado interno del flujo `FILE` 0x8E2C50; se llama por dirección. Lo propio y validable es el envoltorio.
- Registros: EAX = número de caracteres (conservado); ECX/EDX no los lee ningún llamador (ver `ga-audit.txt`).

## Banco

- 4096 pares (32 formatos x 2 modos x 64 variantes), `fixture_oracle.py` es la fuente única de las tablas (`FixtureTables.hpp` se regenera con `--emit`).
  Formatos: literal, vacío, `%d %u %x %X %o %#x %#o %+d % d %-6d %hd %hu %ld %lu %I64d %I64u %I64x %c %s %10s %-10s %.3s %% %5d %*d %p %ls`, el mensaje de World.cpp y once formatos de coma flotante (`%f %e %g %E %G %#.0f %.*f ...`) con NaN con carga, señalizante, ±inf, ±0, subnormales, ±1e30, ±DBL_MAX; enteros extremos; cadenas vacías, de 300 caracteres, con bytes altos y NULL.
- Modo 0: flujo con búfer propio (`_flag` 0x102): `_stbuf` devuelve 0 y la salida queda entre `_base` y `_ptr`. Modo 1: flujo sin búfer sobre un descriptor con FDEV: `_stbuf` asigna el búfer temporal y `_ftbuf` lo vuelca con `_write` (0x82B867), que el banco dobla en ambos lados (sin crédito; NO se escribe en ningún descriptor real).
- Se comparan en bits EXE (hook apagado) y DLL (hook activo): texto producido, EAX, FILE de stdout (0x20 bytes), búfer propio con guardas, contador y punteros del búfer temporal, estado de la sección crítica del cerrojo 0x11, banderas del descriptor 1, errno (`_getptd()+8`), FNSAVE completo (108 bytes, incluidos registros muertos, FIP/FDP) y MXCSR, ESP y registros no volátiles. ECX/EDX se registran sin exigir igualdad (nadie los lee).
- `run_local.py` exige además el texto exacto del oráculo en 2816 casos (formatos enteros/cadena/carácter) y EAX = longitud del texto en todos.
- Límites: no cubre formato inválido (invoca el manejador de parámetros inválidos), `%n`, ni la rama de `_flsbuf` con flujo sin búfer y no tty (asigna búfer y escribe en el descriptor real). El banco no ha sido compilado ni ejecutado (encargo de solo análisis).

## Ejecución

```
python -B reversing/Dependencies21A/fixture_oracle.py
python -B reversing/Dependencies21A/build_probe.py
python -B reversing/Dependencies21A/run_local.py
python -B reversing/Dependencies21A/run_local.py --analyze <comparisons.jsonl>
```

o `python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpeta de salida> Dependencies21A`.

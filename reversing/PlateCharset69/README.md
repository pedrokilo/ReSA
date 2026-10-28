<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PlateCharset69: CCustomCarPlateMgr::LoadPlatecharsetDat 0x6FDC00 (lote 69)

Estado: **APROBADO dentro del juego** (26/10/2026, worktree `D:\Original\wy-2`, HEAD 9a8eb46fa + `source.patch`). 16384 pares, cero diferencias, todas las compuertas de ABI/ruta/restauración en ambos lados, `fp_difference` vacío, 16 familias x 1024 casos,
cobertura de ramas (`coverage.json`) y salida de CADA lado igual a la del oráculo independiente (`fixture_oracle.py`, modelo del desensamblado). Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-platecharset69\20261026-154034-095157`.
Plantillas: `reversing/Helpers66A` (falsos idénticos en ambos lados), `reversing/Helpers67S` (arena, CRC, oráculo y autoprueba) y `reversing/Vehicle66`; contrato: `reversing/_handoff/BRIEF.md`.

| Función | Dirección | Hook (clase / nombre / tipo) | Fuente |
|---|---|---|---|
| `CCustomCarPlateMgr::LoadPlatecharsetDat(const char* filename, uint8* data)` (sin llamadores en el EXE, «unused») | 0x6FDC00 | `CCustomCarPlateMgr` / `LoadPlatecharsetDat` / S (`RH_ScopedInstall`) | `CustomCarPlateMgr.cpp` |

Fila de `docs/hooks.csv` (la añade el coordinador; no es virtual): `CCustomCarPlateMgr,LoadPlatecharsetDat,0x006FDC00,1,0,1,S`.

```
bash reversing/_handoff/bench_here.sh PlateCharset69    # compila la sonda y corre el banco (candado global del juego); usar CMAKE_BUILD_PARALLEL_LEVEL=1 y _CL_=-MP1
python -B reversing/PlateCharset69/build_probe.py
python -B reversing/PlateCharset69/run_local.py
python -B reversing/PlateCharset69/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -I reversing/PlateCharset69/fixture_oracle.py      # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/PlateCharset69/fixture_selfcheck.py   # compila fixture_dump.cpp (Fixtures.hpp tal cual), comprueba la arena y calcula la cobertura prevista (imprime «ok»; --write regenera coverage.json)
python -B reversing/PlateCharset69/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-platecharset69/<corrida>"   # sólo lo ejecuta el coordinador
```
Archivo de arranque en el clon: `platecharset69-runtime.start`; salida `platecharset69-runtime.jsonl`.

## Fuente (`source.patch`: sólo `CustomCarPlateMgr.cpp` respecto a HEAD)

Partió del borrador sin validar del lote 27 (`D:\Original\lote69_B.patch`), contrastado línea a línea con el desensamblado (`dis.sh 0x6FDC00 0x6FDD60`, atoi `0x822506`, strtok `0x82244B`) y corregido. Cambios sobre el borrador:

1. **Defecto real (corregido):** el borrador usaba `atoi` del CRT de la DLL. El `atoi` del EXE (0x82258E -> 0x822506) acumula los dígitos con **desbordamiento de 32 bits** (`eax = eax*10 + d`, sin saturar) y niega con `neg` si hay `-`; el del CRT de la DLL (UCRT) **satura** (`atoi("4294967297")` = 2147483647 en la DLL y 1 en el EXE; `"4294967551"`: 255 contra 0x100000FF -> ax = 0xFF; `"2147483648"`, `"-2147483649"`...).
   Con el `atoi` del CRT el banco da 3426/16384 pares distintos (comprobado). Se añade `PlatecharAtoi` (estático en `CustomCarPlateMgr.cpp`): salta blancos de la configuración regional «C» (9..13 y 32; tabla de tipos del CRT del EXE, bit 8), signo `-`/`+` opcional, dígitos con desbordamiento de `uint32`, `-` niega.
2. **El primer `strtok` no se comprueba** (0x6FDC8D..0x6FDC9A: tras `call strtok` salta directamente al `switch` con `esi = 0`): una línea vacía o formada solo por espacios/tabuladores llega a `atoi(NULL)` (lectura de la dirección 0). El borrador salía del bucle con `n = 0`, devolviendo 0 sin acceso inválido. Ahora es un `do { ... } while (token)`, que reproduce la estructura del EXE (el acceso inválido se reproduce, no se prueba: ver «Lo que NO prueba»).
3. Se mantiene (verificado): `SetDir("DATA")`, `OpenFile(filename, "r")`, `SetDir("")`; `LoadLine(file)` sin comprobar el archivo (con `OpenFile` nulo se llama igualmente a `LoadLine(NULL)`); `";the end"` por `repe cmpsb` de 9 bytes (igual a `strcmp`; incluye el NUL) antes de saltar las líneas que empiezan por `;`; separadores `" \t"` (los 3 bytes copiados de 0x85A510 incluyen el NUL); campos 1º/2º/3º por `atoi` con `cmp ax, 0FFh; jbe` (16 bits bajos sin signo, más de 255 -> 255); con menos de tres campos devuelve 0 **sin** `CloseFile`; palabra `0x80000000 | b2 << 16 | b1 << 8 | b0` escrita en `*out++`; fin de archivo o terminador -> `CloseFile(file)` y devuelve 1.
   Los tres bytes de estado (`b0..b2`) quedan sin inicializar en el EXE pero se escriben los tres antes de usarse (con >= 3 campos), así que `{}` en la DLL no cambia nada observable.
4. Hook añadido: `RH_ScopedInstall(LoadPlatecharsetDat, 0x6FDC00)` en `CCustomCarPlateMgr::InjectHooks` (no estaba; no había fila en `docs/hooks.csv`).

## Banco

Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Se llama a la entrada del EXE (cdecl, dos argumentos, `ret`) con `Gate` (siembra pila y EBX/ESI/EDI, mide ESP/no volátiles/DF/SEH/x87).

**Falsos idénticos en ambos lados** (parche de entrada en la dirección del EXE y en el cuerpo de la DLL, los cuatro con hook y comprobados en cada llamada): `CFileMgr::SetDir` 0x5387D0, `CFileMgr::OpenFile` 0x538900, `CFileLoader::LoadLine(FILESTREAM)` 0x536F80 y `CFileMgr::CloseFile` 0x5389D0.
Registran sus argumentos EN ORDEN (tipo, hash FNV-1a de las cadenas / valor del manejador, índice de línea) y entregan las líneas del caso desde un buffer controlado (`lineBuf`, 1024 bytes, relleno previo 0xDD, como el `ms_line` estático del real). `OpenFile` devuelve un manejador falso (o 0).
`strtok` (0x82244B) y `atoi` (0x82258E) **no** se sustituyen: el lado EXE ejecuta el CRT del EXE y el candidato el `strtok` del CRT de la DLL y `PlatecharAtoi`, de modo que el banco los compara de verdad (por eso el defecto 1 se ve). El campo `_token` del bloque de hilo del EXE (+0x18 de `0x827B3D`) se guarda y restaura alrededor de cada llamada.

**Se compara** (por lado, y entre lados): AL; la traza de eventos completa (`SetDir("DATA")`, `OpenFile(nombre, "r")`, `SetDir("")`, cada `LoadLine(manejador, índice)` y `CloseFile(manejador)` o su ausencia); el CRC-32 de la arena de salida completa (relleno previo + 40 dwords + guarda: detecta cualquier escritura de más, incluidas las guardas),
los dwords escritos; el hash del buffer de línea tras los `strtok` (el CRT escribe NUL en los separadores); y las compuertas de ABI. Además `run_local.py` exige que la salida de CADA lado coincida con la del oráculo en Python (modelo del desensamblado: `atoi`, `strtok`, el bucle y el relleno de la arena) y la cobertura de ramas.
EAX/ECX/EDX de salida sólo se registran (la función no tiene llamadores): se exige AL.

**Fixtures** (`Fixtures.hpp`, 16 familias x 1024): líneas buenas decimales; terminador `;the end` con líneas detrás que no deben leerse; comentarios y terminadores casi iguales (`;the end `, `;the ends`, `;The end`, ...); bordes numéricos (254..257, 65535..65537, 65791, `-1`, `-256`, `+255`, ceros a la izquierda, `12abc`, `0x10`, `1e3`, blancos `\v \f \r` delante o detrás, bytes altos);
desbordamientos de 32 bits (2147483647..2147483649, 4294967295..4294967297, 4294967551, números de 20-40 dígitos, negativos); número de campos variable (1..9, líneas largas de hasta 62 campos y 560 bytes); separadores iniciales, finales, múltiples y sólo tabuladores; `OpenFile` nulo; archivo vacío, sólo comentarios, terminador inmediato;
capacidad máxima (40 líneas buenas, 40 dwords); `data` alineado y desalineado (8 desplazamientos); relleno de arena (ruido, 0, 0xFF, 0xCD, 0xA5); EAX/ECX/EDX y semilla de pila extremos; nombres de archivo vacíos, largos, con bytes altos. Nunca se generan líneas vacías ni de sólo espacios/tabuladores.
Cobertura de ramas medida sobre las entradas con el oráculo (`fixture_selfcheck.py`, `coverage.json` = 60 % de lo previsto): `wrap32` 4480 casos, `clamp_hi` 9272, `short_line` 2077, `end_line` 3652, `lines_after_end` 1353, `near_end` 1908, `open_null` 800, `words_ge38` 915, `unaligned` 5818, etc.

**Sensibilidad comprobada con mutaciones** (cada una con el banco completo, luego restaurada): `atoi` del CRT en lugar de `PlatecharAtoi` -> 3426/16384 pares distintos; `strncmp(line, ";the end", 8)` en lugar de `strcmp` -> 1174/16384.

## Lo que el banco NO prueba (declarado)

- **Ninguna E/S de archivo real:** `SetDir`, `OpenFile`, `LoadLine` y `CloseFile` están sustituidos por falsos en ambos lados (sólo se prueba que la función los llama con esos argumentos y en ese orden). No se abre `DATA/PLATECHARSET.DAT` ni ningún archivo del juego, ni se prueba el `CFileLoader::LoadLine` real (que **sanea** los caracteres de control y las comas a espacios y salta los blancos iniciales: en el juego las líneas nunca llevan `\v`, `\r`, comas ni blancos al principio; el banco entrega esas líneas a propósito para ejercitar `atoi` y `strtok`).
- **Líneas vacías o sólo de separadores:** en el EXE llevan a `atoi(NULL)` (acceso a la dirección 0, `0x822506`: `movzx ecx, byte ptr [esi]` con `esi = 0`; con el `LoadLine` real una línea en blanco también llega ahí). La DLL lo reproduce (`PlatecharAtoi` no comprueba el puntero), pero el banco no genera esos casos porque cualquier acceso inválido aborta la corrida (`first_chance_av`).
- **Configuración regional:** `PlatecharAtoi` asume la «C» (blancos 9..13 y 32); el EXE consulta la tabla de tipos de su CRT. Un `setlocale` distinto en el juego cambiaría el EXE y no la DLL. Los bytes >= 0x80 sólo se prueban en la «C».
- **Más de 40 líneas / búfer de salida:** el EXE escribe sin acotar; el banco usa como máximo 40 dwords (la región de salida), sin comprobar escrituras más allá.
- **Estado interno de `strtok`:** el `strtok` de la DLL es el del CRT de la DLL (estado propio, distinto del del EXE); sólo se compara el resultado observable (tokens y NUL escritos en el buffer).
- EAX alto y ECX/EDX de salida (la función no tiene llamadores) y el código de retorno de `CloseFile` no se exigen más allá de AL.
- Es un banco de una sola función: no valida a quien la llame (nadie) ni los datos reales de matrículas.

## Hallazgos

- Defecto de producción corregido: `atoi` saturado en lugar de con desbordamiento (arriba). Es el único caso que difiere con entradas posibles de un archivo real (números de 10 o más dígitos).
- Intermitencias de la corrida (no del código): una ejecución de mutación se abortó sin escribir `comparisons.jsonl` («GTA sin progreso: el registro no crece desde hace 240 s») por un fallo de arranque de la ventana de GTA; se repitió con el mismo código y terminó con normalidad.

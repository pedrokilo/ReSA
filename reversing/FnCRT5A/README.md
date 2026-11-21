<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/11/2026 (Ciudad de México):** 1004 grupos / 13311 funciones reconstruidas; 11801 con evidencia histórica y 1510 pendientes de validación en 39 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FnCRT5A: capa de E/S del CRT del EXE (21/11/2026)

Lote CRT5A (continuación de [FnCRT4A](../FnCRT4A/README.md), [FnCRT3A](../FnCRT3A/README.md), [FnCRT2A](../FnCRT2A/README.md) y [FnCRT1A](../FnCRT1A/README.md)). Rama `crt5a` (sin subir). Decisión del 20/11: se reconstruye y se engancha (hook activo), se compila y se comprueba que no rompe; no se valida con bancos en juego. El juego no se ejecutó. La verificación es un banco **fuera del juego con archivos reales**: el `gta_sa.exe` original (solo lectura) se mapea en un proceso de prueba de 32 bits y se llama por dirección, y cada guion de operaciones se ejecuta primero con el código del EXE y luego con el propio, comparándolos operación a operación.

Esto cierra lo que CRT4A dejó pendiente («núcleos de archivo y `_flsbuf`/`_filbuf`, `_read`, `_write`, `_lseek`, `_open`/`_sopen`, `_close` y la tabla `__pioinfo`»).

Código nuevo en `source/game_sa/Crt/`:

- `CrtLowIo.{h,cpp}`: capa baja de descriptores (lowio) y bloqueos por descriptor. `CrtDosErrTable.inc` es la tabla `_errtable` del EXE (45 pares, generada leyendo el EXE).
- `CrtStdio.{h,cpp}`: núcleos de stdio (relleno/vaciado de buffers, lectura/escritura/búsqueda sin bloqueo, cierre, apertura de FILE, tabla de FILE `__piob`).
- `CrtFile.cpp` (CRT2A) deja de llamar por dirección a `_fread_nolock`, `_fwrite_nolock`, `_fseek_nolock`, `_fclose_nolock`, `_flush`, `_fflush_nolock`, `_flsall`, `_filbuf`, `_flsbuf`, `_fsopen`, `_ftell_nolock` e `_isatty`; `CrtFormat.cpp` (`write_char`) y `CrtInput.cpp` (`_inc`) llaman a `Flsbuf`/`Filbuf` propios. Se añaden a `CrtFile.cpp` `_snprintf` 0x825C29, `_vsnprintf` 0x825BD3, `_scprintf` 0x821C0D y `_vscprintf` 0x82310F (FILE falsa + `Output` + `_flsbuf`).
- `VehicleRecording.cpp` (lote 25) ya enganchaba `_errno` 0x82B181, `_lock_file2` 0x823FD1 y `_unlock_file2` 0x824023 con cuerpos propios: **no se duplican los ganchos**; esas tres funciones ahora solo reenvían a `ExeCrt::ErrnoPtr`, `ExeCrt::LockFile2` y `ExeCrt::UnlockFile2`, que son los cuerpos usados por la capa nueva.
- Registro en `source/InjectHooksMain.cpp` (`CrtLowIoInjectHooks`, `CrtStdioInjectHooks`; los cuatro de `snprintf` entran en `CrtFileInjectHooks`), 54 filas nuevas en `docs/hooks.csv` (clase `ExeCrt`, `validated=0`) y 4 en `docs/JuegoExe/excluidas.csv` (reconstruidas sin gancho, ver más abajo). Sin `__asm`, sin `naked`.

## Estado compartido con el EXE (no se duplica nada)

Todo el estado sigue en las **mismas direcciones del EXE**; la versión propia lo lee y lo escribe ahí, así que EXE y código propio pueden mezclarse función a función (el banco lo aprovecha: las dos versiones trabajan sobre el mismo `__pioinfo`).

| Dirección | Dato | Uso |
|---|---|---|
| 0xC9C300 | `__pioinfo[64]` | punteros a bloques de 32 `ioinfo` de 0x24 bytes (`osfhnd` +0, `osfile` +4, `pipech` +5, `lockinitflag` +8, `CRITICAL_SECTION` +0xC); el bloque mide 0x480 y se reserva con `malloc` del CRT |
| 0xC9C2FC | `_nhandle` | descriptores con bloque reservado (suma 32 por bloque) |
| 0x8E3450 | `__badioinfo` | `ioinfo` de relleno de `_filbuf`/`_flsbuf` cuando `FILE::_file == -1` |
| 0xC9C418 / 0xC9D420 | `__piob` / `_nstream` | tabla de FILE (0x200 entradas por defecto; las 20 primeras apuntan a `_iob` 0x8E2C30) |
| 0xC9AC4C | `_cflush` | contador de buffers dados (lo suben `_getbuf`, `__stbuf` y `__openfile`) |
| 0xC9B09C / 0xC9AF98 / 0xC9AC04 / 0xC9AC08 / 0x8E2EB4 | `_fmode`, `_commode`, `_umaskval`, `_osplatform`, `__app_type` | parámetros de `_sopen`/`__openfile` (en el juego `__app_type` vale 2, GUI: no se llama a `SetStdHandle`) |
| 0xC9AF94 | puntero a `InitializeCriticalSectionAndSpinCount` | lo resuelve `__crtInitCritSecAndSpinCount` la primera vez (o queda en el sustituto 0x833B96) |
| 0x8E31C0 | `_locktable[0x24]` | cerrojos numerados (`Lock`/`Unlock` de CRT2A): 0xA protege la creación del cerrojo de cada `ioinfo`, 0xB la tabla `__pioinfo`, 1 la tabla `__piob`, 0x10.. los FILE estándar |
| errno / `_doserrno` | `_getptd()+8` / `+0xC` | bloque por hilo de CRT2A (`Getptd`); `ExeCrt::ErrnoPtr` y `DoserrnoPtr` devuelven esas direcciones |

## Diseño y particularidades conservadas

Cada función sigue el desensamblado instrucción a instrucción, con las etiquetas y direcciones en los comentarios; los `try/finally` de bloqueo (SEH del EXE: `_SEH_prolog` 0x8286E0) pasan a objetos RAII (`FhGuard`, `FileGuard`, `File2Guard`), igual que en CRT2A–CRT4A. Cosas que el banco obligó a respetar o que se leyeron del EXE y se conservan:

- **Códigos de error distintos según la función.** `_close`, `_commit`, `_setmode` y `_chsize` ponen solo `errno = EBADF` cuando el descriptor es inválido; `_read`, `_write`, `_lseek` y `_lseeki64` ponen además `_doserrno = 0`; `_lseek_nolock` con manejador inválido solo `errno`. `_commit` con `FlushFileBuffers` fallido deja `_doserrno = GetLastError()` y `errno = EBADF`.
- **`_read_nolock` (0x82B44A):** modo texto (CR LF → LF, Ctrl-Z = fin de archivo, `FEOFLAG` salvo en dispositivos); si el último byte leído es CR se lee uno más (en archivos se vuelve atrás con `_lseek_nolock`, en tubos y dispositivos se guarda en `pipech`); `ERROR_BROKEN_PIPE` devuelve 0 aunque ya se hubiera entregado el carácter adelantado de `pipech`; la bandera `FCRLF` mira `buf[0]` (el primer byte entregado, que puede ser el de `pipech`).
- **`_write_nolock` (0x82B6C2):** `FAPPEND` hace `_lseeki64_nolock(fd, 0, SEEK_END)` antes; en modo texto traduce LF → CR LF en bloques que se cierran al llegar a 0x400 caracteres traducidos y devuelve `escrito - nº de LF` (una escritura parcial no es un error); sin nada escrito y sin error de Windows, `0x1A` en un dispositivo devuelve 0 y lo demás da `ENOSPC` (28).
- **`_sopen_helper` (0x833FF8):** `oflag`/`shflag`/`pmode` → `CreateFileA` con las mismas tablas de acceso, compartición y disposición (`0x600` y `0x200` truncan, `0x500` y `0x700` crean nuevo); `_O_TEMPORARY` añade `DELETE`, `FILE_FLAG_DELETE_ON_CLOSE` y, con `_osplatform == 2`, `FILE_SHARE_DELETE`; si `GetFileType` devuelve 0 se llama a `GetLastError` **después** de `CloseHandle`; en modo texto con lectura/escritura se recorta un `0x1A` final con `_chsize_nolock`; `_O_APPEND` pone `FAPPEND` solo en archivos normales.
- **`_alloc_osfhnd` (0x833E02):** bajo el cerrojo 0xB recorre los bloques; una `ioinfo` libre (sin `FOPEN`) se reserva tomando su cerrojo (que el llamador suelta); sin hueco reserva un bloque nuevo y suma 32 a `_nhandle`.
- **`__openfile` (0x82B019):** el primer carácter del modo es `r`, `w` o `a`; después `+ b t c n D R S T` una vez cada uno (`R`/`S` y `c`/`n` excluyentes); un carácter inválido **detiene la lectura del modo pero la apertura se hace igualmente** con lo acumulado.
- **`_flsbuf` / `_filbuf` / `_flush` / `_fread_nolock` / `_fwrite_nolock`:** copia al buffer lo que cabe, vacía y lee/escribe directo en bloques múltiplos del buffer, y el resto byte a byte; `_filbuf` pone `_IOEOF` (0x10) o `_IOERR` (0x20) según `_read` devuelva 0 o -1 y fija la bandera 0x2000 (Ctrl-Z) y el buffer de 0x1000 como el EXE.
- **`_ftell_nolock` (0x8260FF):** posición del descriptor ajustada por el buffer; en texto cuenta cada LF como CR LF (y reajusta con `_lseek` al final si el buffer es de lectura).
- **`__crtInitCritSecAndSpinCount`:** `__try/__except` solo convierte `STATUS_NO_MEMORY` en `ERROR_NOT_ENOUGH_MEMORY` y devuelve 0 (el único `__try` de todo el lote).
- **`_mtinitlocknum`:** crea el cerrojo con `malloc(0x18)` bajo el cerrojo 0xA con doble comprobación; el segundo hilo que gana la carrera libera el suyo.

## Tabla de funciones

Tamaños en bytes hasta la siguiente función del EXE (incluyen los fragmentos de desenrollado SEH). Todas con gancho `RH_ScopedGlobalInstall`, `validated=0`, salvo las marcadas.

Dirección | Función | Tamaño | Estado | Verificación
---|---|---|---|---
0x82B18A, 0x82B193 | `__doserrno`, `_dosmaperr` | 9, 115 | HECHAS, enganchadas | banco: 4.100 códigos (0..2099 y 2.000 aleatorios de 32 bits), 0 diferencias de `errno` y `_doserrno`
0x82B181 | `_errno` | 9 | HECHA (cuerpo `ErrnoPtr`); el gancho sigue en `VehicleRecording.cpp` | cubierta por todos los guiones
0x833D2C, 0x833D6D, 0x833DE0 | `_get_osfhandle`, `_lock_fhandle`, `_unlock_fhandle` | 65, 115, 34 | HECHAS, enganchadas | guiones de descriptores
0x833E02, 0x833C31, 0x833CAD | `_alloc_osfhnd`, `_set_osfhnd`, `_free_osfhnd` | 333, 124, 127 | HECHAS, enganchadas | guiones + banco de descriptores masivos (hasta 2.100 abiertos: bloques nuevos de `__pioinfo`) + `__app_type == 1` con `SetStdHandle`
0x833F4F | `_open_osfhandle` | 169 | HECHA, enganchada | guiones (manejadores creados con la API)
0x834334, 0x8342DF | `_sopen`, `_open` | 69, 69 | HECHAS, enganchadas | guiones (modos, compartición, creación, ruta inexistente, ^Z final)
0x833FF8 | `_sopen_helper` | 743 | HECHA **sin gancho** (`__usercall`: `shflag` en ECX); `excluidas.csv` (`usercall-cubierta`) | cubierta por `_sopen`/`_open`
0x82AE72, 0x82ADEF | `_close`, `_close_nolock` | 119, 131 | HECHAS, enganchadas | guiones
0x82B617, 0x82B44A | `_read`, `_read_nolock` | 135, 461 | HECHAS, enganchadas | guiones (binario y texto, CR/LF/^Z, lecturas de 0 a 9.000 bytes)
0x82B867, 0x82B6C2 | `_write`, `_write_nolock` | 135, 421 | HECHAS, enganchadas | guiones (texto con bloques de 0x400, append, escrituras parciales)
0x82B986, 0x82B912 | `_lseek`, `_lseek_nolock` | 135, 116 | HECHAS, enganchadas | guiones
0x83440C, 0x834389 | `_lseeki64`, `_lseeki64_nolock` | 152, 131 | HECHAS, enganchadas | guiones (también salto a más de 4 GiB y vuelta)
0x82BFFD | `_commit` | 160 | HECHA, enganchada | guiones (incluye `FlushFileBuffers` fallido en un manejador de solo lectura)
0x83633C, 0x836201 | `_chsize`, `_chsize_nolock` | 123, 315 | HECHAS, enganchadas | guiones (ampliar con ceros, reducir, fallos)
0x836A53, 0x8369F1 | `_setmode`, `_setmode_nolock` | 123, 98 | HECHAS, enganchadas | guiones
0x8334B2 | `_isatty` | 42 | HECHA, enganchada | guiones
0x82AD3F, 0x833BA6, 0x833B96 | `_mtinitlocknum`, `__crtInitCritSecAndSpinCount`, sustituto stdcall | 127, 139, 16 | HECHAS, enganchadas | banco unitario: 26 cerrojos dinámicos y la resolución del puntero (0xC9AF94) con `_osplatform` 2 y 1: 0 diferencias
0x82C0B9 | `_ioinit` | 510 | HECHA **sin gancho** (arranque, antes de cualquier gancho); `excluidas.csv` (`arranque-sin-gancho`) | banco: 60 arranques desde estado vacío, con y sin descriptores heredados (`STARTUPINFO::lpReserved2` simulado: hasta 2.100 descriptores, banderas y manejadores válidos e inválidos), 0 diferencias
0x82C2B7 | `_ioterm` | 76 | HECHA **sin gancho** (cierre del proceso); `excluidas.csv` | mismo banco (estado tras `_ioterm`)
0x823EE5 | `__initstdio` | 169 | HECHA **sin gancho** (arranque); `excluidas.csv` | mismo banco
0x82AF38, 0x828B31 | `_filbuf`, `_flsbuf` | 225, 281 | HECHAS, enganchadas | guiones de FILE
0x82AF0D | `_freebuf` | 43 | HECHA, enganchada | guiones de FILE
0x823D26, 0x823D83, 0x823DB1, 0x823ED6 | `_flush`, `_fflush_nolock`, `_flsall`, `_flushall` | 93, 46, 160, 9 | HECHAS, enganchadas | guiones de FILE (`_flsall` en los dos modos)
0x823438, 0x82356D | `_fread_nolock`, `_fwrite_nolock` | 299, 329 | HECHAS, enganchadas | guiones de FILE
0x8236C0, 0x8260FF | `_fseek_nolock`, `_ftell_nolock` | 206, 409 | HECHAS, enganchadas | guiones de FILE (texto y binario, SEEK_CUR convertido en SEEK_SET)
0x82313F | `_fclose_nolock` | 76 | HECHA, enganchada | guiones de FILE
0x8231DC, 0x82322C | `fgetc`, `getc` | 72, 72 | HECHAS, enganchadas | guiones de FILE
0x82327C, 0x82B019, 0x82B206 | `_fsopen`, `__openfile`, `__getstream` | 82, 360, 281 | HECHAS, enganchadas | guiones de FILE (33 cadenas de modo, válidas e inválidas)
0x82C303, 0x823F8E, 0x823EDF | `_fcloseall`, `_endstdio`, `__iob_func` | – | HECHAS, enganchadas | `_fcloseall` en los guiones; `_endstdio` y `__iob_func` por inspección (cuerpo de pocas líneas)
0x823FD1, 0x824023 | `_lock_file2`, `_unlock_file2` | 35, 35 | HECHAS (cuerpos `LockFile2`/`UnlockFile2`); los ganchos siguen en `VehicleRecording.cpp` | guiones de FILE
0x825C29, 0x825BD3, 0x821C0D, 0x82310F | `_snprintf`, `_vsnprintf`, `_scprintf`, `_vscprintf` | – | HECHAS, enganchadas | banco unitario: 360 casos (límites de 0 a 64 bytes, nueve formatos), 0 diferencias de valor devuelto y bytes del buffer

## Lo que sigue llamando por dirección al EXE (no reconstruido)

- En `CrtFile.cpp`: `_getenv_helper_nolock` 0x824046, 0x823BDB y `malloc` 0x824257 (de `Stbuf`); no son parte de esta capa.
- Fuera de la capa de descriptores y de los núcleos de stdio no se revisó el resto de la API pública de FILE (`fputc`/`putc`, `ungetc` público, `rewind`, `fscanf`, `tmpfile`/`remove`/`rename`/`_unlink`...); las que pasen por `_read`/`_write`/`_lseek`/`_filbuf`/`_flsbuf` ya usan las versiones propias a través de los ganchos.
- `memcpy` (0x8266F0) y `memset` (0x82D4C0) se sustituyen por `std::memcpy`/`std::memset` (mismo resultado byte a byte).

## Verificación

- **Compilación del proyecto completo**: `resultados/compilacion.txt` (EXIT=0, `gta_reversed.asi` enlazado; `contrib/audit_hook_catalog.py --strict`: OK).
- **Banco** (`build_bench.cmd <dir salida>` y `<dir>\bench_crt5a.exe <ruta de gta_sa.exe> --casos N --semilla S --solo fd|stdio|unit|init|many|std|snprintf`): `resultados/corrida_completa.txt`. Cada guion crea un directorio temporal con cuatro archivos de contenido aleatorio (vacíos, con CR/LF/^Z, binarios), ejecuta 25-65 operaciones aleatorias con el código del EXE y las repite con el propio sobre una copia idéntica; compara **por operación** valor devuelto, `errno`, `_doserrno`, bytes leídos, banderas (`osfile`, `pipech`) y posición real de cada descriptor, banderas/`cnt`/desplazamiento/tamaño de buffer de cada FILE y, al final, el contenido de los archivos. Cobertura medida (aperturas con éxito y fallidas, lecturas/escrituras con datos y con error, operaciones que fijaron `errno`) en `corrida_completa.txt`. Corrida completa (semilla 12345): 1.500 guiones de descriptores (71.580 operaciones: 4.827 aperturas con éxito y 9.959 fallidas, 7.841 lecturas/escrituras con datos, 36.547 operaciones que fijaron `errno`) y 1.500 de FILE (76.593 operaciones: 10.850 aperturas con éxito, 10.801 lecturas/escrituras con datos), más 4.100 códigos de `_dosmaperr`, 26 cerrojos, 360 casos de `snprintf`, 60 arranques, 5.941 operaciones con descriptores masivos y 3 con `__app_type` 1: **0 diferencias en todos los grupos**.
- **Condiciones del banco que se corrigieron durante el desarrollo** (no son diferencias del código): el contador acumulativo `_cflush` se compara como incremento por guion; la memoria del montón del CRT sale a cero (los buffers de 4 KiB llegan sin inicializar y una lectura del tramo no escrito da bytes distintos en cada pasada); los FILE libres se ponen a cero entre guiones (`__getstream` no reinicia `bufsiz`); no se lee de FILE de solo escritura ni se escribe en FILE de solo lectura, y en flujos de actualización (`+`) se intercala `fseek` al pasar de lectura a escritura (el EXE, como cualquier CRT, deja `cnt` negativo y copia fuera del buffer en esos usos: comportamiento indefinido que tumbaba el proceso de prueba); una diferencia se repite hasta 4 veces antes de contarse (el antivirus o el indexador abren a ratos los archivos recién escritos y producen una violación de compartición en una sola de las dos pasadas; en la corrida completa: 0 repeticiones).
- **Prueba negativa** (`neg_run.sh`, `resultados/prueba_negativa.txt`, 250 guiones por mutación, se detiene en la primera diferencia, 8 mutaciones en paralelo): 31 mutaciones de una línea (18 en `CrtLowIo.cpp`: Ctrl-Z de `_read`, `written - lfCount` de `_write`, borrado de `FEOFLAG` en `_lseek` y `_lseeki64`, atributo de solo lectura con `_umask`, disposición de `0x600`, `FNOINHERIT`, el `0x1A` final de `_sopen`, relleno de ceros de `_chsize`, modo anterior de `_setmode`, `_doserrno` de `_commit`, intervalo 0x13..0x24 de `_dosmaperr`, `_nhandle += 0x20`, `FAPPEND` de `_open_osfhandle`, compartición de `_open`, resolución de `InitializeCriticalSectionAndSpinCount`, `SetStdHandle` de `fd` 2, bandera 0x81 de `_ioinit`; 13 en `CrtStdio.cpp`: byte de `_flsbuf`, banderas EOF/ERR de `_filbuf`, bloque directo de `_fread_nolock`, comparación de `_fwrite_nolock`, ajuste de `_ftell_nolock`, signo de `SEEK_CUR` en `_fseek_nolock`, modo `a` de `__openfile`, cuenta de `_flsall`, retorno de `_close` en `_fclose_nolock`, `_freebuf`, `_IORW` de `_flush`, bandera 0x2000 de `_filbuf`, tamaño por defecto de `__piob`): **30 detectadas y 1 NO detectada** (`_freebuf` borra `_IOMYBUF` pero no `_IOSETVBUF` 0x400: el banco nunca activa esa bandera porque no llama a `setvbuf`, así que la mutación es equivalente en estos guiones).
- Los ganchos nuevos están con `validated=0`: no se ha probado nada dentro del juego (decisión del 20/11).

## Pendiente

- Probar dentro del juego (cuando se decida): los ganchos de `_read`/`_write`/`_lseek`/`_close` y `_fsopen` afectan a toda la E/S por FILE del juego (carga de `.dat`, `.ide`, partidas, `fopen` de `FileMgr`).
- No cubierto por el banco: cierre de los descriptores 1 y 2 compartiendo manejador (rama de `_close_nolock` que no cierra el manejador de Windows: cerrarlos rompería la salida del propio proceso de prueba), lecturas de tubo (`ERROR_BROKEN_PIPE`, `pipech`: el banco solo usa archivos y el dispositivo `NUL`), `malloc` fallido en `_alloc_osfhnd`/`_mtinitlocknum`/`__getstream` y bloqueo concurrente entre hilos.
- Resto de la API pública de FILE (ver arriba) y, fuera de esta capa, lo que quedaba de CRT4A: locale (`__updatetlocinfo`, `_tolower_lk`, `_isctype`, `_wctomb`, `_mbtowc`) y `pow` x87.

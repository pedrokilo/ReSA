<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FnCRT6A: revision de la API publica de FILE del CRT (21/11/2026)

Resultado: **no hay nada que reconstruir.** `fputc`/`putc`, `ungetc`, `rewind`, `fscanf`, `tmpfile`, `remove`, `rename` y `_unlink` **no existen en el `gta_sa.exe`**: el CRT es estatico y el enlazador solo incluye lo que el juego referencia. No se ha anadido codigo, ganchos ni filas de `hooks.csv`.

## Evidencia (EXE original en solo lectura, herramientas de esta carpeta)

- **Cerrojos de FILE:** `fputc`, `ungetc`, `rewind` y `fscanf` toman `_lock_file`. Los 11 llamadores de `_lock_file` (0x823FA2) del EXE son exactamente: `fclose` 0x8231B2, `fgetc` 0x8231EC, `getc` 0x82323C, `fread` 0x823530, `fwrite` 0x823683, `fseek` 0x82375E, `fgets` 0x8237BD, `fprintf` 0x823A43, `fflush` 0x823EA5, el contenedor de `ftell` 0x826270 y `fputs` 0x8262D3. Todos ya reconstruidos (CRT2A/CRT5A). Los 12 `_unlock_file` (0x823FF4) coinciden. Ninguno es `fputc`, `ungetc`, `rewind` ni `fscanf`.
- **`fscanf`:** `_input` (0x8290D4) solo tiene un llamador, `sscanf` 0x8220D7.
- **`fputc`/`putc`:** `_flsbuf` (0x828B31) solo se llama desde `_output`/`snprintf`/`scprintf`/`fwrite_nolock`/`__ftbuf` (0x821C00, 0x823102, 0x82362F, 0x825C1C, 0x825C73, 0x827EBF).
- **`rewind`:** `_lseek` (0x82B986) solo lo llaman `fseek`, `ftell` y `_flsbuf` (0x823729, 0x82611F, 0x8261EB, 0x82621D, 0x828C01).
- **`remove`/`_unlink`/`rename`/`tmpfile`:** la tabla de importaciones no tiene `MoveFileA`, `GetTempPathA` ni `GetTempFileNameA`; `DeleteFileA` (IAT 0x8580F8) solo se usa en 0x619103 (codigo del juego, cerca de `RsGrabScreen`) y en el trampolin 0x81E4DE. Ninguna rutina del CRT llama a `_dosmaperr` tras `DeleteFileA`.

## Fuera de alcance

0x823BDB (`doexit`: cerrojo 8, `_onexit`, tablas de terminadores 0x8A5A34..0x8A5A48; la llama `Call823BDB` 0x823CAE) no es API de FILE; pertenece al grupo de cierre del proceso (CrtOnexit) y no se ha tocado.

## Herramientas

`desens.py` (capstone), `refs.py` (llamadores de una direccion), `imports.py` (IAT del EXE), `gaps.py` (inicios de funcion probables sin nombre en el catalogo). Sin compilar ni ejecutar el juego.

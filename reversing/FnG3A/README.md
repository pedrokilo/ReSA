<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Lote G3A: 0x00746480 (separador de la línea de comandos) queda sustituido por el CRT

Última excluida del código del juego con trabajo pendiente (`usercall-parcial` en [FnG2A](../FnG2A/README.md)). Ahora figura como `sustituida-por-crt`. **No cambia código de producción.**

EXE analizado: `gta_sa.exe`, SHA256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b` (comprobado en `emu_orig.py`). Cuerpo de 0x746480 (300 bytes): SHA256 `82d2bb6e491f95133e0024727fe2fc2ea1ab30aa0cdac2d7492cb548bc0a7ee6`. No se ejecutó el juego.

## Semántica exacta (del desensamblado)

Firma: `int** sep(const char* cmd /*EAX*/, int* argc /*pila*/)`; `ret` simple (limpia la llamadora); conserva EBX, ESI, EDI, EBP; devuelve el bloque reservado.

- `n = strlen(cmd)`. **Pasada 1 (cuenta)** sobre `i = 0..n` (incluye el NUL): `"` conmuta `inq`; si el byte es > 0x20 (sin signo) o `inq`, es parte de un token (en `i == n` cierra); si no, es separador. Separador = **cualquier byte <= 0x20 fuera de comillas** (espacio, tab, CR, LF, controles); los bytes >= 0x80 son caracteres normales. Las comillas cuentan como inicio de token. Al cerrar un token, `cuenta++`.
- **Reserva**: `malloc(n + 1 + 4*cuenta)` (0x824257); tabla de `cuenta` punteros **sin terminador NULL**, y detrás una copia de la cadena (`n + 1` bytes). Sin límite de longitud. Nunca se libera (la llamadora la deja escapar).
- **Pasada 2 (relleno)**: mismo recorrido; un token empieza en el primer byte que **no** es `"`; las comillas iniciales se saltan. Al cerrar el token: si el byte anterior de la copia es `"` se pone a 0 (**solo una** comilla final), si no se pone a 0 el separador. Comillas intermedias: se conservan. Barras invertidas: sin tratamiento (no hay `\"` ni `""`). Comilla sin cerrar: abarca hasta el final. Cadena vacía: `argc = 0`.
- `*argc = cuenta` (de la pasada 1). Si un token solo tiene comillas (`""`, `"`), la pasada 2 no lo rellena y su ranura **queda sin inicializar** (basura del heap) aunque `argc` la cuenta.
- Primer token (nombre del programa): sin trato especial.

Llamadora 0x748710 (`NOTSA_WinMain`, `docs/hooks_notsa.csv` con `validated=0`, sin fila en `hooks.csv`): `eax = GetCommandLineA()`, `argc = [esp+0x14]`, `argv = eax devuelto`; después recorre `i = 1..argc-1` llamando `RsEventHandler(rsPREINITCOMMANDLINE = 0x25, argv[i])` y más tarde `RsEventHandler(rsCOMMANDLINE = 1, argv[i])`. No lo usa para nada más ni lo libera. El único consumidor efectivo es `rsPreInitCommandLine` (0x619530), que solo compara con `-vms`; `rsCOMMANDLINE` acaba en `rsFILELOAD`, que no tiene manejador.

## Método

1. `oracle.py`: oráculo en Python escrito **desde el desensamblado** (no desde el CRT).
2. `emu_orig.py`: tercera referencia: ejecuta el código real de 0x746480 (bytes sin parchear del EXE) con Unicorn, con `malloc` sustituido por una reserva rellena de 0xCD. Compara argc, bloque completo (tabla de punteros y copia), tamaño reservado, registros preservados y ESP.
3. `argvdump.cpp` (+ `crtbatch.cpp`, `build.bat`): programa MSVC (VS 18, UCRT, `/MD`) que escribe `__argc/__argv`; `crtbatch.exe` lo lanza con `CreateProcessA` y la línea de comandos **exacta en bytes** de cada caso (12 hilos). Resultado en `crt.out`. Comprobado también con `/MT` en 3000 casos (idéntico salvo el nombre del programa en la línea vacía).
4. `gen_cases.py` (semilla fija) genera `cases.txt` y `groups.txt`: 56 692 líneas completas (con nombre de programa): 105 de uso real (7 formas de exe, `-windowed`, `-width`, `-height`, `-vms`, rutas entre comillas, tabuladores), 21 con barra final antes de la comilla de cierre, 148 artificiales, 26 125 exhaustivas (todas las cadenas de hasta 6 símbolos sobre `" \t " \ a` y hasta 4 sobre 9 símbolos con 0x01, 0x7F, 0xE9), 23 998 aleatorias (alfabetos sesgados y los 255 bytes no nulos), 6 275 mezclas con `-vms`, 20 largas (hasta 32 000 bytes).
5. `compare.py` hace las comparaciones y escribe `resultado.json`.

## Resultado

- **EXE emulado frente al oráculo: 56 692 casos, 0 diferencias** (argc, bloque entero, tamaño, registros, ESP).
- **Oráculo frente al CRT** (56 692 casos): 13 018 iguales; 38 385 distintos sin efecto observable; 5 033 con comportamiento indefinido del original (ranura sin inicializar; el CRT da `""`); 256 con efecto sobre `-vms`.
- **Uso real: 105 de 105 idénticos** (grupo `real`).
- Los 256 con efecto sobre `-vms` (el único argumento que consume el juego) se deben solo a: barra invertida antes de la comilla (`-path "C:\x\" -vms`: el CRT trata `\"` como comilla literal y funde `-vms` en el argumento anterior; 7 de las 21 líneas del grupo `real-barra-final`), comillas dentro del argumento (`-v"m"s`, `-"vms"`), o CR/LF/controles como separador. Ningún lanzador, `.bat` ni acceso directo conocido genera esas líneas; el juego no reconoce `-path`.
- Diferencia aparte de `WinMain.cpp`: recorre `i = 0..argc-1` (el EXE, `1..argc-1`); solo afectaría si el nombre del programa fuese exactamente `-vms`.
- Línea de comandos vacía: el CRT inventa `argv[0]` con el nombre del programa y el original da `argc = 0`; el cargador de Windows nunca entrega una línea vacía.

## Decisión

`sustituida-por-crt` (opción a). Las lineas de uso real son idénticas, el efecto observable se limita a `-vms`, y las diferencias son entradas artificiales; reproducir el separador del EXE conservaría además un defecto (ranuras sin inicializar con `""` que acaban en `strcmp` con basura). No se toca código de producción. Diferencias registradas como A-J12 en [DesviacionesConocidas.md](../../docs/DesviacionesConocidas.md).

## Revalidación

`python -I reversing/FnG2A/inspect_excluidas.py <gta_sa.exe> --check` comprueba, para esta entrada, el SHA256 del EXE y del cuerpo, que el oráculo no tenga diferencias contra el EXE emulado (>= 16 384 casos), que las líneas de uso real sean todas idénticas y que el JSON de FnG2A refleje `resultado.json`.

Para repetirlo (Python con `unicorn`, `capstone`; MSVC): `cmd /c build.bat`, `python gen_cases.py cases.txt`, `crtbatch.exe argvdump.exe cases.txt crt.out 12`, `python compare.py`. Los ejecutables compilados no se versionan.

# FnCRT2A: continuación del CRT del EXE (20/11/2026)

Lote CRT2A (continuación de [FnCRT1A](../FnCRT1A/README.md)). Rama `crt2a` (sin subir). Decisión del 20/11: se reconstruye y se engancha (hook activo), se compila y se comprueba que no rompe; no se valida con bancos en juego. El juego no se ejecutó. Además hay un banco **fuera del juego** (método de FnCRT1A/FnLIB1A: el `gta_sa.exe` original, solo lectura, mapeado en un proceso de prueba y llamado por dirección) para las funciones puras o de estado simple.

Código: `source/game_sa/Crt/CrtState.{h,cpp}` (datos por hilo, rand, strtok, isprint, bloqueos), `CrtMath.{h,cpp}` (floor, ceil), `CrtFile.{h,cpp}` (envolturas de FILE, formato, std::exception). Registro en `source/InjectHooksMain.cpp` (`CrtStateInjectHooks`, `CrtMathInjectHooks`, `CrtFileInjectHooks`) y 33 filas nuevas en `docs/hooks.csv` (clase `ExeCrt`, `validated=0`). Sin `__asm`.

## Diseño

- **Estado por hilo (`_ptiddata`)**: `Getptd` (0x827B3D) se reconstruye entera (TlsGetValue con el índice del EXE 0x8E3110; si no hay bloque, `calloc(1, 0x88)` + `TlsSetValue`, `+0x54 = 0x8E3510`, `+0x14 = 1`, `+4 = -1`, `+0 = id del hilo`; conserva GetLastError). `rand`/`srand`/`strtok` leen y escriben ESE bloque (`+0x14` semilla, `+0x18` puntero de strtok): **no existe un generador paralelo**; la semilla del juego es la misma global del CRT lo llame el EXE o el código propio. Los llamadores propios de `0x821B1E`/`0x821B11` (CarCtrl, Cam, AutoPilot, Vehicle, Population, Tasks, etc.: 23 archivos) ahora llaman a `ExeCrt::Rand`/`ExeCrt::Srand`.
- **Math (floor 0x8219F0, ceil 0x823820)**: rama SSE2 reconstruida con la misma aritmética entera (cuenta `0x433 - exponente`, `psrlq/psllq`, cuenta >= 64 da 0) y `addsd/subsd` con intrínsecos SSE2 (inmunes a la precisión de 24 bits de la x87 que fija el juego). El EXE entra por SSE2 solo si `0xC9C400 != 0` y MXCSR/FPU tienen todas las excepciones enmascaradas; si no, va a la rama x87 (floor 0x828A60, ceil 0x82BA31): se invocan por dirección. Con NaN el EXE llama al manejador 0x82872C(&x, &x, &x, 0x3ED floor / 0x3EC ceil) y devuelve el argumento: igual aquí. Llamadores propios (Hud, Object, PedGeometryAnalyser) cambiados.
- **FILE y formato**: las envolturas públicas (`fread`, `fwrite`, `fseek`, `fclose`, `fflush`, `fgets`, `fputs`, `fopen`, `fprintf`, `sprintf`, `vsprintf`, `sscanf`, `getenv`) se reconstruyen con el mismo orden `lock -> operación -> unlock`; el `try/finally` SEH del EXE se sustituye por un objeto RAII (camino normal idéntico; con excepción también desbloquea). `_lock_file`/`_unlock_file`/`__lock`/`_unlock` se reconstruyen enteros. Los **núcleos** (`_fread_nolock`, `_fwrite_nolock`, `_fseek_nolock`, `_output` 0x827F4B, `_input` 0x8290D4, `_filbuf`, `_flsbuf`, `_fsopen`, `_flush`...) siguen siendo del EXE y se llaman por dirección: no se reconstruyeron (el motor de formato incluye `%f` con conversión de coma flotante: 1.904 bytes + biblioteca de conversión).
- **sprintf/vsprintf/sscanf**: FILE falsa de 0x20 bytes en la pila como el EXE (ptr/cnt/base/flag 0x42 o 0x49). `rwplcore.cpp` (`_rwStringOpen`) y `Stats.cpp`/`AudioZones.cpp`/`preset_view.cpp` llaman ahora a las versiones del proyecto.
- **std::exception** (ctor 0x822D97, ctor de copia 0x822DE5, destructor 0x822E2F) y `_CxxThrowException` (0x822D5D, copia la plantilla de 32 bytes de 0x887E5C y `RaiseException`): reconstruidos como funciones libres `__thiscall`/`__stdcall`.

## Tabla de las 57 raíces CRT del EXE (con las de CRT1A)

| Dirección | Función | Estado | Verificación |
|---|---|---|---|
| 0x8247E0 qsort, 0x826330 strlen, 0x8263C0 strcmp, 0x8214D0 strncmp, 0x826590 strcpy, 0x8265A0 strcat, 0x821F40 strncpy, 0x826450 strncat, 0x822650 strstr, 0x82413F free, 0x82422B _nh_malloc, 0x824269 realloc, 0x824416 calloc, 0x82119A operator new (+ 0x821195, 0x8213AE, 0x8214BD saltos) | 17 de CRT1A | hechas | banco FnCRT1A, 0 diferencias |
| 0x827B3D `__getptd` | HECHA, enganchada | banco: bloque creado (+4, +0x14, +0x54), último error conservado, segunda llamada devuelve el mismo bloque: 0 diferencias |
| 0x821B11 srand, 0x821B1E rand | HECHAS, enganchadas | banco: 1.800.096 valores (EXE/propio/alternados sobre la misma semilla), 0 diferencias |
| 0x82244B strtok | HECHA, enganchada | banco: 300.000 cadenas con delimitadores arbitrarios (bytes >= 0x80 incluidos), 0 diferencias |
| 0x8228B5 isprint | HECHA, enganchada (locale de un byte; con `mb_cur_max > 1` llama a `_isctype` 0x82AAE2 del EXE) | banco: -1..255 con tabla aleatoria, 0 diferencias |
| 0x8219F0 floor, 0x823820 ceil | HECHAS, enganchadas (rama SSE2 propia; x87 y NaN por dirección del EXE) | banco: 299.982 casos cada una (bordes, denormales, ±0, inf, 2^52, exponentes cercanos a 0..52, valores float), comparación de bits, 0 diferencias; NaN: solo se comprueba que el núcleo lo detecta (el manejador necesita el estado de arranque) |
| 0x82ADBE `__lock`, 0x82AD12 `_unlock`, 0x823FA2 `_lock_file`*, 0x823FF4 `_unlock_file`*, 0x8262A2 ferror | HECHAS, enganchadas | sin banco (solo compilación) |
| 0x823521 fread, 0x823674 fwrite, 0x82374F fseek, 0x82318B fclose, 0x823E86 fflush, 0x823798 fgets, 0x8262B8 fputs, 0x8232D8 fopen, 0x826261, 0x823CAE | HECHAS (envolturas), enganchadas | sin banco (requiere el estado de arranque de las FILE) |
| 0x823A30 fprintf (el análisis lo llamaba fwprintf), 0x821BB5 sprintf, 0x8230B8 vsprintf, 0x8220AD sscanf, 0x8240C7 getenv, 0x827DEC `__stbuf`, 0x827E74 `__ftbuf` | HECHAS (envolturas), enganchadas | sin banco |
| 0x822D5D `_CxxThrowException`, 0x822D97 / 0x822DE5 / 0x822E2F std::exception | HECHAS, enganchadas | sin banco |
| 0x8211FD, 0x821255 | PENDIENTES | construyen y lanzan una excepción con marcos de excepciones de C++ propios del EXE (`_EH_prolog` 0x825D74, destructores con tablas de desenrollado): no se pueden sustituir sin duplicar las tablas de desenrollado de MSVC |
| 0x821CE6 `__onexit` | PENDIENTE | el núcleo 0x821C3E recibe el argumento en EDI (convención propia): no se puede llamar desde C++ sin ensamblador; hay que reconstruir también 0x821C3E y la lista de atexit (0xC9D42C) |
| 0x8220F0 pow, 0x829B99 (pow SSE2 de Intel, 2.861 bytes), 0x8219F0-familia restante | PENDIENTES (pow, 0x829B99) | la tabla de coeficientes y las ramas de la biblioteca matemática de Intel (mas de 700 instrucciones SSE2 con tablas) piden un banco propio de pow con NaN/inf/denormales; no se empezó para no dejarla a medias |
| 0x85742B (fldcw [0xCAF004]) | PENDIENTE | solo se puede expresar con `__asm`/`_controlfp` (que no es idéntico: reinterpreta la palabra de control); no se encontró motivo para forzarlo |
| 0x856BC0 | PENDIENTE (no es CRT) | `mov ecx, 0xC803E8; jmp 0x59ACD0`: inicializador estático de un objeto del juego |
| 0x820A10 | PENDIENTE (no es CRT) | función del juego (usa 0x802F60, 0x80E9B0 y un vtable en 0xC97B24): mal clasificada como raíz CRT |

\* no son raíces de la lista, pero son dependencias de las anteriores y también están enganchadas.

## Verificación

- **Compilación del proyecto completo**: ver `resultados/compilacion.txt`.
- **Banco** (`build_bench.cmd <dir salida>` y `<dir>\bench_crt2a.exe <ruta de gta_sa.exe> --casos 300000`, opciones `--solo mate|ptd|rand|strtok|isprint`): `resultados/corrida_completa.txt`, 0 diferencias.
- **Prueba negativa** (`neg_run.sh`, `resultados/prueba_negativa.txt`): 8 mutaciones de una línea (floor con `<=`, ceil con otra condición, umbral de |x| < 1 desplazado, constante de rand, desplazamiento de rand, condición final de strtok, semilla inicial de getptd, máscara de isprint): **las 8 detectadas**.
- Los ganchos nuevos están con `validated=0`: no se ha probado nada dentro del juego (decisión del 20/11).

## Pendiente y notas

- No se cambiaron los usos de `std::sprintf`/`strcpy` de la DLL que los comentarios describen como «como el original» (MenuManager_Draw, TxdStore, PedAttractor...): usan la biblioteca de C de la DLL, no el CRT del EXE; pasar a `ExeCrt::Sprintf` cambia el formato en los límites (NaN, inf, cero negativo) y lo debe decidir el agente principal.
- Siguiente lote lógico del CRT: el motor de formato `_output` (0x827F4B) y `_input` (0x8290D4), la conversión de coma flotante (`_cfltcvt`), `pow` con su banco, `__onexit`, y los núcleos `_nolock` de FILE.

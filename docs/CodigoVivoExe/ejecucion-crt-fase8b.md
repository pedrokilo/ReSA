<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Restos del CRT del arranque propio (fase 8, parte CRT-B)

Rama `vivo-crtb` (desde `origin/main` 4a4a7b7328, 21/11/2026). Continúa [ejecucion-crt-fase8.md](ejecucion-crt-fase8.md), [fase8-arranque.md](fase8-arranque.md) y la sección de rupturas de [fase8-experimento.md](fase8-experimento.md). Código nuevo en `source/game_sa/Crt/CrtMbcs.{h,cpp}` y `CrtStartup.{h,cpp}`; banco fuera del juego en [`reversing/FnCRTB8A`](../../reversing/FnCRTB8A/README.md). No se tocó `DatosExe`, RenderWare/D3D9 ni el locale ya integrado (`__updatetlocinfo`, `_tolower_lk`, `_isctype`, `_wctomb`, `_mbtowc`); solo se **llama** a `CrtLCMapStringA`/`CrtGetStringTypeA`, que ya eran C++ propio.

## 1. Lo que hay ahora en `StandaloneStartup`

Orden de `entry` 0x824570, todo C++ propio sin `__asm` y sin ganchos (el arranque corre antes de que exista la DLL):

```
StartupEarly      GetVersionExA, __heap_init, __mtinit, __ioinit
StartupArguments  GetCommandLineA, entorno ANSI, __setargv (que ahora llama antes a __initmbctable, como 0x82DB30), __setenvp
Cinit             = __cinit 0x823B76
  _FPinit 0x8E2B74 = __fpmath 0x821397 (FpMath)
  tabla C 0x8A5A14: [nulo] TableC826A72 · Onexitinit · Initstdio · TableCSseAvailableInit · InitMbcTable · TableCUnhandledFilterInit   (se detiene en la primera que devuelve != 0)
  atexit(RtcTerminate 0x82BF1E)
  inicializadores de C++: InitSecurityCookie (0x8339CA, el primero) y las 1.489 constantes x87 (RunX87StaticInitializers)
```

Desaparece `ExtraTablaC` de `StandaloneExe.cpp` (el experimento): `FaseEntrada` solo llama a `ExeCrt::StandaloneStartup()`.

## 2. Qué se reconstruyó

| Función del EXE | Dirección | C++ (`ExeCrt::`) | Gancho | Notas |
| --- | --- | --- | --- | --- |
| `__initmbctable` | `0x833494` | `InitMbcTable` (`CrtMbcs.cpp`) | no (arranque) | `_setmbcp(-3)` la primera vez (bandera `0xC9D430`); la llaman `__setargv` y la tabla C |
| `_setmbcp` | `0x833344` | `Setmbcp` | sí | bajo el cerrojo `0xD`; `-2` OEM, `-3` ANSI, `-4` la del locale; reutiliza el bloque global si su cuenta es 0 o reserva otro de `0x220` bytes y lo copia (`ulinfo`, `_mbctype`, mapa de caja) |
| núcleo de `_setmbcp` | `0x83319E` | `SetmbcpCore` | sí | 5 páginas conocidas (tabla `0x8E39F0`, 4 grupos de rangos de bytes iniciales con las banderas de `0x8E39E8`), `GetCPInfo` para las demás, un byte para la 0; `-1` si la página no es válida y nunca hubo `-2/-3/-4` |
| `setSBCS`, `setSBUpLow` | `0x832F74`, `0x832F9D` | `SetSbcs`, `SetSbUpLow` | sí | tablas a cero; mapa de mayúsculas/minúsculas con `CrtGetStringTypeA` y `CrtLCMapStringA` (ya eran C++ del locale) o, sin información de la página, solo A–Z y a–z |
| LCID de la página | `0x832F45` | `MbcpToLcid` (ayudante anónimo) | no (usercall) | 932→0x411, 936→0x804, 949→0x412, 950→0x404, el resto 0 |
| `__updatetmbcinfo` | `0x83312F` | `UpdateTMbcInfo` | sí | el hilo (`ptd+0x60`, campo nuevo `ThreadData::mbcInfo` en `CrtState.h`) toma el bloque global y libera el que deja si su cuenta llega a 0 |
| `__mbschr` | `0x836697` | `Mbschr` | sí | sin página multibyte llama a `strchr`; con ella compara pares (byte inicial + final); un byte inicial sin final devuelve nulo |
| `_strncnt` | `0x834B6E` | `Strncnt` | no (usercall: cadena en EAX) | `strnlen`; nadie del C++ propio la llama |
| tabla C 1 | `0x826A72` | `TableC826A72` | no | 8 bytes a `0xFF` en `0x8E37B4` |
| `__sse2_available_init` | `0x8289F5` (+ `0x8289C0`) | `TableCSseAvailableInit` (+ `Sse2ExecutionProbe`) | no | `EFLAGS.ID` → `CPUID(1).EDX[26]` y una instrucción SSE2 bajo `__try`; deja 1 en `0xC9C400` y `0xC9C404` (los lee `UseSse2MathPath`: pow, floor, ceil) |
| tabla C 6 | `0x82AC54` | `TableCUnhandledFilterInit` | no | `SetUnhandledExceptionFilter`; el anterior en `0xC9AC74` |
| filtro | `0x82AC0E` (+ `0x832ECF`) | `CrtUnhandledExceptionFilter` (+ `IsValidCodePointer`) | no | excepción de C++ sin capturar → `std::terminate()` del CRT enlazado; si no, encadena con el filtro anterior si `IsBadCodePtr` lo da por válido, o devuelve 0 |
| `__security_init_cookie` | `0x8339CA` | `InitSecurityCookie` | no | mezcla hora, PID, TID, `GetTickCount` y `QueryPerformanceCounter` en `0x8E31BC`; `0xBB40E64E` si sale 0 |
| terminación de RTC | `0x82BF1E` | `RtcTerminate` | no | recorre un rango vacío; la registra `Cinit` con `atexit` como `__cinit` |
| `__fpmath` (`_FPinit`) | `0x821397` (+ `0x82134F`, `0x826F04`, `0x826EC4`) | `FpMath` (+ `FpInitDispatch`, `DetectFdivBug`, `FdivBugCheck`) | no | **la encontró la ejecución del ejecutable independiente** (primer INT3 de `Cinit`, ver sección 5): `__cinit` llama antes de la tabla C al puntero `_FPinit` (`0x8E2B74` = `0x821397`). Escribe seis punteros a rutinas del EXE en `0x8E30F0`, `_adjust_fdiv` (`0xC9AC00`) con `IsProcessorFeaturePresent(0)` buscada por nombre en KERNEL32 (o la prueba FDIV si no está), `_controlfp(_PC_53, _MCW_PC)` (en el EXE pasa por `0x825136`) y `fnclex` (aquí `_clearfp`, que además limpia las banderas de MXCSR) |
| `__cinit` | `0x823B76` | `Cinit` | no | ver sección 1 |

`__setargv` (ya reconstruido) llama ahora a `InitMbcTable()` al empezar, como el EXE (`0x82DB30`), así `IsLeadByte` ve los bytes iniciales de la página ANSI del sistema; la limitación «sin bytes iniciales de doble byte» de `ejecucion-crt-fase8.md` queda resuelta. Las 6 funciones enganchadas son 6 filas nuevas en `docs/hooks.csv` (clase `ExeCrt`, `validated=0`, instaladas por `CrtMbcsInjectHooks` desde `InjectHooksMain.cpp`). Las 13 sin gancho se listan en `docs/JuegoExe/excluidas.csv` (12 filas nuevas, `arranque-sin-gancho`; `0x832F45` y `0x834B6E` por ser usercall) y en `cierre-crtb.csv`.

### La rama x87 de `pow` (`0x822184`) y el SSE2

El experimento (`fase8-experimento.md`, ruptura 1) la alcanzaba porque `0xC9C400` quedaba a 0. Con `TableCSseAvailableInit` en `Cinit` la bandera sale 1 en cualquier procesador con SSE2 (todos los que ejecutan el juego) y `ExeCrt::Pow` entra por su núcleo SSE2 propio. `Pow` sigue llamando por dirección a `0x822184` solo si las excepciones de MXCSR o de la FPU están sin enmascarar; esa rama (el `pow` x87 de MSVC 7.1 con `_ctrlfp`, `__fload_withFB` y más de diez ayudantes de `0x828E..0x828F..`) **no se reconstruye**: el juego (Direct3D 9 con la FPU enmascarada) no la usa; en el ejecutable independiente caería en un INT3 (modo estricto) si alguien desenmascarara la FPU, y eso es lo que se quiere ver. Documentada como pendiente solo si aparece en una ejecución.


## 3. Decisiones (enlazar o reconstruir)

| Pieza | Decisión | Razón |
| --- | --- | --- |
| `TypeMatch` `0x826F2D` (79 B, usercall ESI/EDI) | **se enlaza** (no se reconstruye) | es el comparador de tipos de `FindHandler` del manejo de excepciones de C++ del EXE (`__CxxFrameHandler3`, solo lo llama `0x8275DB`). El código propio lo compila MSVC con su propio `__CxxFrameHandler3` enlazado, que lo trae; el del EXE solo sirve al código del EXE (que desaparece). Nada del C++ propio lo llama por dirección. Reconstruirlo exigiría reconstruir además `FindHandler`, `CatchIt` y `_CxxThrowException` enteros para que lo usaran, y no habría quien los llamara |
| terminate dentro del filtro `0x82AC0E` (`0x827A92`, `abort` `0x832EE7`) | **se enlaza** al `std::terminate()` del CRT de MSVC | `0x827A92` lee `_pterminate` del bloque del hilo del EXE y cae en `abort` (mensaje, `raise(SIGABRT)`, `_exit(3)`); el EXE propio solo registra `set_terminate` en el CRT enlazado. Es la única diferencia observable entre el filtro del EXE y el nuestro, y está comentada en el código |
| `__mbschr`, `_strncnt` | **se reconstruyen** (eran «enlazar» en `E2-cadenas-1`) | pequeñas, `__mbschr` depende de `__updatetmbcinfo`, que a su vez cuelga de `_setmbcp`; todo junto es un trozo cerrado de 6+2 funciones, ahora comparado con el EXE |


## 4. Inicializadores de `_initterm`

Estado de las 1.673 filas de `fase8-inicializadores.csv` (columnas nuevas `estado` y `nota`, generadas por `contrib/fase8_arranque.py`):

| Estado | Filas | Qué son |
| --- | ---: | --- |
| hecho | 1.496 | las 1.489 constantes x87 (`CrtStaticInitTable.inc`) y 7 del CRT: la tabla C completa (`0x826A72`, `__onexitinit`, `__initstdio`, `0x8289F5`, `__initmbctable`, `0x82AC54`) y `__security_init_cookie` `0x8339CA` |
| pendiente-DatosExe | 177 | 140 constructores de objetos estáticos del juego con su destructor en `atexit`, 32 asignaciones de datos y 5 llamadas que escriben en `.bss`/`.data` del juego |

Las 177 pendientes **no se tocaron** por el encargo (otro agente trabaja en `DatosExe`): ninguna es del CRT, todas escriben objetos o valores iniciales de globales del juego (`0x968A00`, `0xB6BA3C`, `0xC40304`, `0xC2BA08`…). Los constructores llaman a constructores de clases que ya son C++ (por ejemplo `0x40FB60`), pero necesitan la memoria del objeto, que define `DatosExe`; cuando `DatosExe` defina cada objeto como un global C++ con constructor, el inicializador del EXE sobra. Hasta entonces ese estado queda a cero en el ejecutable independiente (el experimento no ha visto aún efecto).


## 5. Verificación

### 5.1 Banco fuera del juego (método `FnCRT*A`, ver [`reversing/FnCRTB8A`](../../reversing/FnCRTB8A/README.md))

El EXE original (solo lectura) mapeado en `0x400000` y llamado por dirección, contra el C++, con las mismas entradas y el mismo estado; montón nuevo por caso; llamadas del sistema interpuestas para fijar valores.

| Sección | Comparaciones (semilla 7, 5.000 casos) | Diferencias |
| --- | ---: | ---: |
| `mbcs`: `_setmbcp`, su núcleo, `setSBCS`, `setSBUpLow`, `__initmbctable`, `__updatetmbcinfo`, `__mbschr`, `_strncnt` (150.000 casos con estados iniciales aleatorios, páginas de 1 y 2 bytes, bloque global y de hilo con cuentas 0/1/2, `GetCPInfo` que falla, `-2/-3/-4`) | 150.000 | 0 |
| `tablac`: `0x826A72`, `0x8289F5`, `0x82AC54` y su filtro `0x82AC0E`, `__security_init_cookie`, `__fpmath` con la API real, falsa o ausente | 40.000 | 0 |
| **Total** | **190.000** | **0** |

También con `--casos 2000` y las semillas 1 y 3 (72.000 comparaciones cada una) y la 5 solo `tablac` (16.000), 0 diferencias. **Prueba negativa** (`neg_run.sh`): 21 mutaciones de una línea del C++, **las 21 detectadas** (tabla en el README del banco).

No comparado: la rama de `terminate` del filtro (decisión de la sección 3) y el camino en que la instrucción SSE2 *falla* (este equipo tiene SSE2).

### 5.2 Opción `STANDALONE_STARTUP` y ejecutable independiente

* `WinMain.cpp` con `/DNOTSA_STANDALONE /DGTASA_STANDALONE_STARTUP=1` y las opciones del proyecto (`cl /Zs`): **sin errores** (solo los avisos previos de `NOMINMAX` y de nombres de tipo). La opción sigue apagada por defecto; en `WinMain` la usa `ExeCrt::StandaloneStartup()`.
* **Ejecutable independiente** (`reversing/FnFase8`, `enlazar.cmd` con los `.obj` de esta compilación Debug y `StandaloneExe.cpp` sin `ExtraTablaC`; `gta_sa_standalone` enlazado con 0 símbolos sin resolver): carpeta de pruebas **en D:** (`D:\Original\_crtb_run`, junctions de solo lectura a los datos del juego de D:; el `gta-sa.exe` original solo como fuente de datos: `--ruta-original`), lanzado con `reversing/FnFase8/ejecutar_crtb.ps1`.

| Ejecución (`reversing/FnFase8/ejecuciones/`) | Resultado |
| --- | --- |
| *primer intento* (antes de `FpMath`, no conservado) | INT3 en `0x821397` llamado desde `ExeCrt::Cinit+0x2E`: **`_FPinit` = `__fpmath`**, que `__cinit` llama antes de la tabla C. Reconstruida (sección 2) y añadida al banco |
| `crtb2-20261121-220343` (estricto) | `StandaloneStartup` correcto → CRT propio → `WinMain` propio → `DllMain` (ganchos) → `NOTSA_WinMain`; sin ningún INT3 en funciones del CRT. Primer INT3 a los 1.562 ms: `0x750430` (RpWorld, de la lista de pendientes de RenderWare/D3D9, otro agente) |
| `crtb2s-20261121-220407` (`--shims`) | igual que `t4` del experimento: dispositivo D3D9 y `RwCamera` creados; primer INT3 `0x750AC0` (RpWorld) a los 1.578 ms |
| `crtbc-20261121-220450` (`--censo --shims`, el retorno simulado deja seguir) | solo dos llamadas a código original en todo el arranque y el primer fotograma, ambas del controlador D3D9 de RenderWare (`0x7F98D0`, `0x7F99B0`); **0 en el CRT (0x820000+)** |

Quedan resueltas las rupturas 1 del experimento (SSE2/pow) y la pendiente de `__initmbctable`; las rupturas 2–5 son de RenderWare/D3D9 y no se han tocado.

### 5.3 Compilación y humo

* Compilación completa Debug de la rama (`_build_VIVOCRTB.sh`, dos veces: antes y después de `FpMath`): **`EXIT=0`**.
* Humo con el ASI de la rama (`AGUA_ASI=D:\Original\wv-crtb\bin\Debug\gta_reversed.asi`, `reversing/FnBIS1`, `cal_mal.args`): ropero **BIEN**, agua **BIEN**, `prueba_marcadores.py` **BIEN** (12 marcadores, los 12 cambian en 1 s). Repetido con la compilación final (con `FpMath`): ropero BIEN, agua BIEN, marcadores BIEN.
* `contrib/audit_hook_catalog.py --strict`: OK; `contrib/audit_cpp_assembly.py`: 0 `__asm`/`naked`.


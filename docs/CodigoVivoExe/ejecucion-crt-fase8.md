<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Parte CRT del arranque propio (fase 8): cuerpos de `std::exception`, `__setargv`, `__setenvp`, `_doexit` y revisión de huecos

Rama `vivo-crtf8` (desde `origin/main` 36af6bde60, 21/11/2026). Continúa [fase8-arranque.md](fase8-arranque.md) (P4/P5), [ejecucion-v48.md](ejecucion-v48.md) (V5 abierto) y [ejecucion-p6.md](ejecucion-p6.md) (huecos). Código nuevo en `source/game_sa/Crt/` y banco fuera del juego en [`reversing/FnCRTF8A`](../../reversing/FnCRTF8A/README.md). No se tocó `DatosExe`, `RunningScript`, el código de render D3D9 ni las funciones del locale (`__updatetlocinfo`, `_tolower_lk`, `_isctype`, `_wctomb`, `_mbtowc`).

## 1. Los cuatro cuerpos de `CrtExceptionDtor.cpp` (cierra 4 abiertas de V5)

`CrtExceptionDtor.cpp:53-56` llamaba por dirección (`Body(0x822E83/0x822EBF/0x822EFA/0x822F94)`) a los cuerpos de los destructores con borrado escalar `0x822F40/0x822F5C/0x822F78/0x822FBD`. Ahora son C++ propio (`DtorBody822E83`, `DtorBody822EBF`, `DtorBody822EFA`, `DtorBody822F94`) y **además se enganchan** (4 filas nuevas en `docs/hooks.csv`, clase `ExeCrt`, `validated=0`):

| Cuerpo | Qué hace en el EXE (desensamblado) |
| --- | --- |
| `0x822E83` | vtable `0x887EA0` y `jmp 0x822E2F` (`std::exception::~exception`) |
| `0x822EBF` y `0x822EFA` | vtable `0x887EAC` y `jmp 0x822E2F` (el EXE tiene una copia por clase) |
| `0x822F94` | vtable `0x887EC4`; bajo el bloqueo `0xE` libera el texto (`+4`) con `free` si no es nulo; **no** mira la bandera `+8` ni anula el puntero |

El único cuerpo que sigue por dirección en ese archivo es `0x4F3D10` (destructor de `std::string`, cuerpo de `0x8211F2`); no es de las 4 abiertas.

Resultado: `contrib/codigo_vivo_cierre_v48.py` (re-ejecutado) cierra las 4: V5-1 pasa de 59/70 a **63/70 cerradas**, V5 completo de 21 a **17 abiertas** (las 17 restantes son las de `ejecucion-v48.md` 3: cadenas de `entry`/carga retardada/locale que mueren al retirar el arranque del EXE).

## 2. `__setargv`, `__setenvp`, `_doexit` (sin gancho) e integración en `StandaloneStartup`

Todas en `CrtStartup.cpp/.h`, sin `__asm`, sobre las mismas direcciones del EXE que el resto del CRT propio (`DatosExe` las aporta en el ejecutable independiente):

| Función | Dirección | C++ | Notas |
| --- | --- | --- | --- |
| `__crtGetEnvironmentStringsA` | `0x82DBC8` | `ExeCrt::CrtGetEnvironmentStrings` | copia ANSI (`malloc` del CRT) del bloque de entorno; `f_use` (`0xC9AEE0`) 0/1/2 como el EXE |
| `parse_cmdline` | `0x82D9BA` | `ParseCmdline` (ayudante) | usercall (EAX, ECX, ESI): reglas de comillas y barras de `stdargv.c`; bytes iniciales de doble byte por `_mbctype` (`0xC9C0A0`) |
| `__setargv` | `0x82DB26` | `ExeCrt::Setargv` | `_pgmptr` (`0xC9AC38`, búfer `0xC9ADD8`), `__argc` (`0xC9AC1C`), `__argv` (`0xC9AC20`) |
| `__setenvp` | `0x82D8F3` | `ExeCrt::Setenvp` | `_environ` (`0xC9AC28`), `_env_initialized` (`0xC9D424`); libera `_aenvptr` (`0xC9AC50`) |
| `__crtExitProcess` | `0x823AFE` | `ExeCrt::CrtExitProcess` | `CorExitProcess` si `mscoree.dll` está cargada; `ExitProcess`; no vuelve |
| `_doexit` | `0x823BDB` | `ExeCrt::DoExit` | tabla de `atexit`/`_onexit` (`0xC9D42C..0xC9D428`, el extremo se relee tras cada llamada), terminadores `0x8A5A34..0x8A5A3C` y `0x8A5A40..0x8A5A48`, banderas `0xC9AC44`, `0xC9AC40`, `0xC9AC48` |

* **Integración.** `ExeCrt::StartupArguments()` (`GetCommandLineA` en `_acmdln`, entorno ANSI en `_aenvptr`, `Setargv` → fallo 8, `Setenvp` → fallo 9) se llama desde `StandaloneStartup()` entre `StartupEarly` (heap, hilos, E/S) y `Onexitinit`, el mismo orden que `entry`. `exit` (`0x823CAE`, enganchada desde CRT2A en `CrtFile.cpp`) ya no llama por dirección a `0x823BDB`: llama a `DoExit(code, 0, 0)`. El cuerpo `Call823BDB` conserva el nombre para no mover la fila de `hooks.csv`.
* **Opción `STANDALONE_STARTUP`.** Sigue apagada por defecto. `CrtStartup.cpp` compila siempre (compilación completa Debug de la rama: `EXIT=0`). Para comprobar el camino encendido sin una segunda compilación completa del proyecto como ejecutable independiente, `WinMain.cpp` se compiló en solo sintaxis (`cl /Zs`) con las mismas opciones del proyecto más `/DNOTSA_STANDALONE /DGTASA_STANDALONE_STARTUP=1`: **sin errores** (únicamente el aviso previo de `NOMINMAX`). No se usó en el humo.
* **Límite conocido.** `__initmbctable` `0x833494` (`_setmbcp(-3)`: tabla `_mbctype` según la página de códigos ANSI, `0x833344` y `0x83319E`) **no está reconstruida** (es mbcs/locale: la rama del locale); `StandaloneStartup` no la ejecuta y `Setargv` lee `_mbctype` tal como esté (sin bytes iniciales: correcto en páginas de un solo byte; en una de doble byte las líneas con caracteres de 2 bytes se partirían por el segundo byte si coincide con un espacio o una comilla). Queda como pendiente de mbcs.
* **Listadas como excluidas** (`docs/JuegoExe/excluidas.csv`, `arranque-sin-gancho`): las seis funciones de la tabla (6 filas nuevas) y `fase8-arranque-lotes.csv` pasa a `reconstruida-sin-gancho`.

### Verificación fuera del juego (método FnCRT*A)

Banco [`reversing/FnCRTF8A`](../../reversing/FnCRTF8A/README.md): EXE original mapeado en `0x400000` y llamado por dirección, comparado con el C++ con las mismas entradas.

| Sección | Comparaciones (semilla 7, 5.000 casos) | Diferencias |
| --- | ---: | ---: |
| cuerpos y borrados escalares de `std::exception` | 180.000 | 0 |
| `__setargv` / `parse_cmdline` (líneas aleatorias, con y sin bytes de doble byte, fallo de `malloc` inyectado) | 53.322 | 0 |
| `__crtGetEnvironmentStringsA` y `__setenvp` (`f_use` 0/1/2, fallo inyectado en cada reserva) | 77.442 | 0 |
| `_doexit` con `retcaller = 1` (orden de llamadas, función que registra otra durante la salida, `quick` 0/1, banderas, extremo de tabla) | 5.000 | 0 |
| **Total** | **315.764** | **0** |

Prueba negativa: 15 mutaciones de una línea, **las 15 detectadas** (tabla en el README del banco; dos candidatas iniciales eran equivalentes —vtable intermedia que `Destruct` sobrescribe y las dos copias idénticas de `0x822EBF`/`0x822EFA`— y se sustituyeron por mutaciones observables).

El tramo con `retcaller = 0` de `_doexit` y `__crtExitProcess` terminan el proceso y se comprueban por inspección del desensamblado (no por banco).

## 3. Las 5 pendientes «enlazar» de E2-cadenas

Revisadas contra el código ya integrado:

| Función | Dirección | Estado |
| --- | --- | --- |
| `_atol` | `0x822506` | **cubierta**: `CrtCtype.cpp` (rama del locale, ya en `main`) la engancha como `ExeCrt::Atol` y `_atoi` salta a ella. `CrtLocale.cpp:21` (`LocAtol`) la sigue llamando por dirección: pasa por el gancho, no depende del EXE en el ejecutable propio, pero conviene que la rama del locale use `ExeCrt::Atol` directamente |
| `__setenvp` | `0x82D8F3` | **hecha** en esta rama (sección 2) |
| `__mbschr` | `0x836697` | **no cubierta**: mbcs; `CrtLocale.cpp` no la reconstruye ni nada del C++ propio la llama; se enlaza con el CRT de MSVC (pendiente mbcs, junto con `__initmbctable`) |
| `_strncnt` | `0x834B6E` | igual que `__mbschr` |
| `TypeMatch` | `0x826F2D` | runtime de excepciones de C++ (`E2-excepciones`): se enlaza; no es de locale |

## 4. Hueco `0x82E924` (CRT, P6): cerrado

`0x82E924` (143 bytes) es `ctrlevent` de `signal.c`: el manejador de `SetConsoleCtrlHandler` que instala `signal` (`0x82E9F3`, el `push 0x82E924` está en `0x82EAE7`) para `SIGINT`/`SIGBREAK` (códigos 2 y 0x15, tablas `0xC9AEE4`/`0xC9AEE8`, bloqueo 0). **Nadie llama a `signal`** (ninguna llamada ni dword apunta a `0x82E9F3`), así que `ctrlevent` está muerta con ella. La clasificación anterior la dejaba `pendiente-dato` porque contaba como puntero de datos vivo un dword de la **tabla de ámbitos SEH** de su propia función (`0x88C280`, `push 0x88C280; call _SEH_prolog`), que apunta al *funclet* de desenrollado dentro de la misma pieza.

Cambio en `contrib/huecos_p6.py`: un puntero de una tabla de ámbitos SEH (`push tabla; call 0x8286E0`, ventana de `0x30` bytes) que cae en la misma función o hueco que el `push` se atribuye al propio `push` del prólogo (vive solo si vive esa función) en vez de contar como dato vivo. Efecto sobre las 914 piezas: solo cambian `0x82E924` (`pendiente-dato` → `muerta-por-reemplazo`) y `0x82BF1E` (sigue `crt-enlazar`, pierde una entrada de tabla espuria). Hueco `H6 0x82E924`: **cerrado** (clase `muerta-por-reemplazo`).

## 5. Las 9 piezas «citada-en-cpp» que podían ser funciones vivas

Detalle en [revision-citadas-p6.csv](revision-citadas-p6.csv) (lo escribe `huecos_p6.py`; la tabla `REVISION` del script guarda el motivo de cada una).

| Pieza | Qué es | Clase | Acción |
| --- | --- | --- | --- |
| `0x453FB0` | `ret`: destructor de los elementos de `CPhoneInfo` (eh vector) | comentario / muerta | **documentada**: solo la cita un comentario de `PhoneInfo.cpp`; el constructor `0x4541E0` ya es C++ y no llama al iterador |
| `0x5C91E0` | `ycck_cmyk_convert` de libjpeg (`jdcolor.c`), 344 bytes | libjpeg enlazada | **documentada**: `JPegCompress.cpp` la instala con `LIBJPEG_CB`, que con `GTASA_WITH_LIBJPEG` (activa en el proyecto) es la de `libs/libjpeg-6b` (`w_jdcolor.c` la exporta); decisión vigente de enlazar bibliotecas de terceros |
| `0x61A8A0` | `CTaskSimpleAnim::FinishRunAnimCB` | **viva, ya era C++ sin enganche** | **enganchada** (`RH_ScopedInstall`; el EXE la registra desde tareas no reemplazadas). Cuerpo idéntico: `or byte [data+0xC],1` (bit `m_bIsFinished`) y `[data+8] = 0` |
| `0x68B7D0` | `CTaskSimpleArrestPed::FinishAnimArrestPedCB` | **viva, ya era C++ sin enganche** | **enganchada** (`mov byte [data+0xC],1`, `[data+0x10] = 0`) |
| `0x8194D0` | `mov ax, 0F7D1h; ret` (OSErr -2095): destino de los tres punteros de QuickTime (`0x8E2A0C/10/14`) | **viva** (las tres rutas `QtmlInitialize`/`QtmlTerminate` y los datos iniciales la usan) | **reconstruida y enganchada** como `QtmlUnavailableStub` (`QTMLRuntime.cpp/.h`); los tres punteros se restauran con la dirección de la función propia en vez de la constante `0x8194D0` |
| `0x7F40F0` | callback de lectura de textura por defecto del módulo de texturas de RenderWare (966 bytes; `+0x14` de `0xC97B4C`, lo guarda `_rwTextureModuleOpen`) | **viva sin enganche** | **no reconstruida**: pasa a pendiente (V7/rwcore). Es la ruta de carga de cada textura y no hay banco de RenderWare; reconstruirla sin él sería un riesgo que la regla de no validar función a función no cubre |
| `0x7F5140` | generación de mipmaps por defecto (652 bytes, `+0x2C`) | **viva sin enganche** | pendiente (V7/rwcore), mismo motivo |
| `0x7F53D0` | búsqueda de textura por nombre por defecto (292 bytes, `+0x18`) | **viva sin enganche** | pendiente (V7/rwcore) |
| `0x7FFF00` | función estándar `0x09` (`RwImageFindRasterFormat`) del controlador D3D9, guardada en `_rwD3D9DeviceSystemStandards` (208 bytes) | **viva sin enganche** | pendiente: pertenece al controlador de D3D9 (otro agente) |

Efecto en `cierre-p6.csv`: los huecos con `0x7F40F0`, `0x7F5140`/`0x7F53D0` (hueco `0x7F513F`) y `0x7FFF00` (hueco `0x7FFEF8`) **dejan de estar cerrados** (antes cerraba `citada-en-cpp`, que solo significa «el C++ cita la dirección»). Resultado neto de huecos cerrados: de **259/270 a 257/270** (cierra `0x82E924`; se reabren 3 huecos de RenderWare con función viva sin enganche). Piezas: `reconstruida` 278 → 281, `citada-en-cpp` 175 → 167, `muerta-por-reemplazo` 91 → 93, `pendiente-dato` 23 → 26.

## 6. Verificación del conjunto

* Compilación completa de la rama (`_build_VIVOCRTF8.sh`, Debug x86, worktree `D:\Original\wv-crtf8`): `EXIT=0`, sin errores.
* Humo con la DLL de la rama sobre el EXE: `prueba_ropero.py cal_mal.args crtf8` **BIEN** (puerta y marcador, 15 entidades y 12 marcadores) y `prueba_agua.py cal_mal.args crtf8` **BIEN** (`UnderWaterness` 1.0, brillo medio 49,8).
* `contrib/audit_hook_catalog.py --strict`: OK. `contrib/audit_cpp_assembly.py`: 0 `__asm` ni `naked`.
* `contrib/codigo_vivo_cierre_v48.py --aplicar` y `contrib/codigo_vivo_plan.py` re-ejecutados (solo cambian `cierre-p1.csv`, `cierre.csv`, `funciones-vivas.csv`, `plan.csv`, `plan-lotes.md`); `contrib/huecos_p6.py` re-ejecutado (`huecos-p6*.csv`, `cierre-p6.csv`, `revision-citadas-p6.csv`).
* **No regenerado:** `docs/ReversedClasses.md`, `ValidationInventory.md` y `ProjectRoadmap.md` (`contrib/sync_docs.py` se detiene con «hooks.csv validated=1 (11798) no coincide con evidencia aprobada (0)» en este árbol parcial: faltan los registros de validación). `docs/hooks.csv` lleva 7 filas nuevas (4 `ExeCrt::DtorBody*`, `QTMLRuntime::QtmlUnavailableStub`, `CTaskSimpleAnim::FinishRunAnimCB`, `CTaskSimpleArrestPed::FinishAnimArrestPedCB`, todas `validated=0`); al integrar hay que ejecutar `sync_docs.py` desde el árbol completo. Las cabeceras de estado de `QTMLRuntime.cpp`, `TaskSimpleAnim.cpp` y `TaskSimpleArrestPed.cpp` ya quedaron actualizadas.

## 7. Pendiente

1. `__initmbctable`/`_setmbcp` (`0x833494`/`0x833344`), `__mbschr` y `_strncnt`: mbcs, con la rama del locale; `StandaloneStartup` no deja `_mbctype` inicializada.
2. Las cuatro funciones de RenderWare de la sección 5 (`0x7F40F0`, `0x7F5140`, `0x7F53D0` y `0x7FFF00`).
3. Activar `STANDALONE_STARTUP` en un ejecutable independiente real (necesita los inicializadores estáticos de `DatosExe`).
4. `CrtLocale.cpp`: sustituir `LocAtol` (llamada por dirección a `0x822506`) por `ExeCrt::Atol`.

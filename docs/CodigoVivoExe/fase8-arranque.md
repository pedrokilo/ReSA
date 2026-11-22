# Fase 8: arranque propio, CRT enlazado, inicializadores e importaciones (partes P4 y P5)

Rama `vivo-p5` (desde `origin/main` 410d9f2d84, 21/11/2026). Cubre los lotes `E2-*` de la parte P4 (CRT de MSVC, salvo locale, que lleva otro agente: `__updatetlocinfo`, `_tolower_lk`, `_isctype`, `_wctomb`, `_mbtowc`) y los lotes F1–F4 de la parte P5 de [plan-lotes.md](plan-lotes.md). Todo es estático salvo lo que se compiló y probó (ver sección 7). Resultados por función en [fase8-arranque-lotes.csv](fase8-arranque-lotes.csv); mapa de los 1.673 inicializadores en [fase8-inicializadores.csv](fase8-inicializadores.csv); dueños de los 44 funclets en [fase8-funclets-f4.csv](fase8-funclets-f4.csv). Todo se regenera con `python -X utf8 contrib/fase8_arranque.py`.

## 1. Hecho que manda el diseño

**La DLL se carga cuando el EXE ya ejecutó `entry` (0x824570).** El arranque del CRT (heap, hilos, E/S, `_cinit`, los 1.668 inicializadores de C++) corre entero antes de que exista ningún gancho. Consecuencias:

* Ninguna función del arranque se puede *enganchar*: el gancho nunca se ejecutaría. Por eso `Ioinit`, `Ioterm` y `Initstdio` (CRT5A) están reconstruidas **sin gancho** y listadas en `docs/JuegoExe/excluidas.csv` como `arranque-sin-gancho`. Esta parte añade siete más con el mismo tratamiento (sección 4).
* Con la DLL sobre el EXE no se puede *quitar* nada del arranque; solo se puede (a) reconstruirlo y comprobarlo contra el estado que dejó el EXE y (b) dejarlo listo para el ejecutable propio.
* El estado del CRT del EXE (montón 0xC9C2F4, índice TLS 0x8E3110, tabla de bloqueos 0x8E31C0, `_iob`, tabla de `atexit` 0xC9D42C…) vive en direcciones del EXE y el C++ de `source/game_sa/Crt/*` lo lee y escribe **en esas direcciones** (nada se duplica). En el ejecutable propio esas direcciones las aporta `DatosExe`; alguien tiene que dejarlas en el estado en que las deja `entry`. Ese es el trabajo de F1/F2.

## 2. Qué parte del arranque ya tenía el proyecto

| Pieza | Dónde | Estado |
| --- | --- | --- |
| `WinMain` (0x748710 `NOTSA_WinMain`) | `source/app/platform/win/WinMain.cpp` | enganchada; `entry` la llama (0x8246EC). `WinMain` propio solo existe con `NOTSA_STANDALONE` (hoy solo vuelca ganchos) |
| `__ioinit` 0x82C0B9, `__ioterm` 0x82C2B7, `__initstdio` 0x823EE5 | `CrtLowIo.cpp`, `CrtStdio.cpp` | reconstruidas sin gancho (CRT5A) |
| `_onexit` 0x821CE6 y su núcleo | `CrtOnexit.cpp` | enganchada (la tabla la crea 0x821CBE, ver abajo) |
| `malloc/free/realloc/calloc/new/delete`, `_set_new_handler` | `CrtHeap.cpp` | enganchadas (activas si `__active_heap == 1`) |
| `__getptd`, `rand/srand`, `strtok`, bloqueos `_lock/_unlock` | `CrtState.cpp` | enganchadas (leen el TLS del EXE) |
| `_cinit` 0x823B76, `__heap_init`, `__mtinit`, `_setargv/_setenvp`, `_doexit` 0x823BDB | — | **no** estaban (esta parte reconstruye heap/mt) |
| thunks `DelayLoad_*` | `source/app/platform/win/DelayLoadThunks.cpp` | envoltorios stdcall enganchados; los ayudantes con `jmp eax` no se pueden escribir en C++ (R8A) |
| protector `ExeProtectionStub` (MD5 + resolución de API 0x401000–0x4033E6) | `ExeProtectionStub.cpp` | engancha solo rutinas sueltas; lo que ejecuta el protector corre antes de la DLL; se retira con el protector (lote V1) |
| `DllMain` (`source/dllmain.cpp`) | — | lee INI y línea de comandos y llama a `InjectHooksMain`; no toca el CRT |

## 3. El arranque del EXE, paso a paso (`entry` 0x824570, MSVC 7.1 `WinMainCRTStartup`)

| # | Paso | Dirección | Después de esta parte |
| ---: | --- | --- | --- |
| 1 | `GetVersionExA` → [0xC9AC08] plataforma, [0xC9AC14] mayor, [0xC9AC18] menor, [0xC9AC0C] compilación, [0xC9AC10] mayor<<8+menor | prólogo de `entry` | `ExeCrt::OsVersionInit` |
| 2 | `__heap_init(1)` (fallo: `_amsg_exit(0x1C)`) con `__heap_select` 0x82C457 | 0x82C471 | `ExeCrt::HeapInit` / `HeapSelect` (el camino del montón de bloques pequeños, `__active_heap == 3` y 0x82C556, se trata como fallo: no se alcanza desde Windows 2000) |
| 3 | `__mtinit` (fallo: 0x10): `__mtinitlocks` 0x82AC74, `TlsAlloc` → [0x8E3110], bloque `_tiddata` de 0x88 bytes | 0x827D25 | `ExeCrt::Mtinit`, `Mtinitlocks`, `Mtterm` 0x827B0C, `Mtdeletelocks` 0x82ACBD |
| 4 | `__RTC_Initialize` (rango vacío 0x88E240..0x88E240) | 0x82BEDA | no hace nada |
| 5 | `__ioinit` (fallo: 0x1B) | 0x82C0B9 | ya reconstruida (CRT5A) |
| 6 | `GetCommandLineA`, `__crtGetEnvironmentStringsA` 0x82DBC8, `__setargv` 0x82DB26 (fallo 8), `__setenvp` 0x82D8F3 (fallo 9) | — | los hace el `mainCRTStartup` del enlazador (`__argc/__argv/environ` de la DLL/EXE propio); `getenv` del EXE (0x8240C7, enganchada) lee `_environ` del EXE: ver pendientes |
| 7 | `__cinit` 0x823B76: [0x8E2B74] (`_FPinit`), **tabla C** 0x8A5A14..0x8A5A30, `atexit(0x82BF1E)`, **tabla C++** 0x8A4000..0x8A5A10 | 0x823B76 | secciones 5 y 6 |
| 8 | `GetStartupInfoA`, `__wincmdln` 0x82D88A, `WinMain` 0x748710, `exit` (0x823CAE → `_doexit` 0x823BDB) | — | `WinMain` propio; el cierre usa `exit` del CRT enlazado |

Tabla C (7 entradas): 0x826A72 (8 bytes de 0x8E37B4), `__onexitinit` 0x821CBE (reserva 0x80 bytes en [0xC9D42C]), `__initstdio` 0x823EE5, 0x8289F5 (0xC9C400/4), 0x833494 (0xC9D430), 0x82AC54 (0xC9AC74) y `__security_init_cookie` 0x8339CA (0x8E31BC).

`ExeCrt::StandaloneStartup()` (`source/game_sa/Crt/CrtStartup.cpp`) encadena 1–3, 5, `__onexitinit`, `__initstdio` y las constantes x87 de la tabla C++. Se llama desde `WinMain` **solo** con `GTASA_STANDALONE_STARTUP` (opción CMake `<proyecto>_STANDALONE_STARTUP`, APAGADA). Compila siempre, así que no se pudre.

## 4. P4: lotes E2-* (CRT de MSVC, sin locale)

Resultado por función en [fase8-arranque-lotes.csv](fase8-arranque-lotes.csv). Criterio: **enlazar** lo que el CRT de MSVC del ejecutable propio ya aporta y que el C++ del proyecto no llama por dirección; **enganchar** lo que se pueda escribir en C++ puro con resultado idéntico; **descartar** funclets `try/finally` de rutinas ya con RAII.

| Lote | Funciones | Hecho |
| --- | ---: | --- |
| E2-excepciones-1 / -2 | 40 + 11 | **Decidido: enlazar.** Runtime de excepciones/SEH/señales (`__CxxFrameHandler`, `_UnwindNestedFrames`, `__SEH_prolog/epilog`, `__local_unwind2`, `_longjmp`, `_raise`, `_abort`, `terminate`, `__security_check_cookie`…). Usan marcos y registros propios (no se pueden escribir sin `__asm`) y el C++ propio no llama ninguna por dirección: en el ejecutable propio las aporta el CRT de MSVC. Con la DLL sobre el EXE siguen vivas porque las usa código del EXE aún no reemplazado |
| E2-montón-1 | 21 | **7 enganchadas**: `_msize` 0x828C4A (`HeapSize` del montón del CRT, solo con `__active_heap == 1`, bajo `GTASA_CRT_HEAP_HOOKS`) y los 6 destructores con borrado escalar de la familia `std::exception` (0x8211D6, 0x8212B9, 0x822F05, 0x822F40, 0x822F5C, 0x822F78; más 0x822FBD de E2-estado y los cuerpos 0x8211F2 y 0x8212AE) en `CrtExceptionDtor.cpp`. **1 reconstruida sin gancho** (`__heap_select`). 13 descartadas (cuerpos/tablas de std::exception y montón de bloques pequeños sin uso). Las 13 llamadas por dirección que el C++ hacía a `operator new` (0x82119A), `new[]` (0x821195), `delete` (0x8214BD) y `delete[]` (0x8213AE) pasaron a los operadores del proyecto (`::operator new/delete`, que ya usan `ExeCrt` si el montón es el del sistema): `LinkList.h`, `Game.cpp`, `WinsockLink.cpp`, `FxEmitter*.cpp`, `Heli.cpp`, `Plane.cpp`, `AEUserRadioTrackManager.cpp`, `PlayerRelationshipRecorder.cpp`, `D3DResourceSystem.cpp`. Quedan por dirección solo las ramas de `MemoryMgr.cpp` para `__active_heap != 1` y `DecisionMakerTypes.cpp` (ABI EDX:EAX vigilada, la toca otra rama) |
| E2-E/S-1 | 15 | **Cerrado: descartar.** Son los `finally` (`_unlock_file`) de `fclose/fread/fwrite/fseek/…` ya reconstruidas con RAII |
| E2-cadenas-1 | 12 | **7 enganchadas**: `memcpy` 0x8266F0 y `memmove` 0x822A20 (mismo algoritmo, ambas a `memmove`), `memset` 0x82D4C0, `strchr` 0x8213D0 (a las del CRT de MSVC, `CrtMem.cpp`), `_strdup` 0x82514C (malloc del EXE), `strtod` 0x826071 y `_fltin2` 0x82BF62 (sobre `CrtFloatIn`, `CrtStrtod.cpp`; el infinito es el del EXE, errno 0x22). El llamador `D3dxShaderSource.cpp` usa ya `ExeCrt::Strdup`. 5 por enlazar: `_atol` 0x822506 (usa la tabla de ctype del locale del hilo: agente de locale), `__mbschr` 0x836697 y `_strncnt` 0x834B6E (mbcs/locale), `TypeMatch` 0x826F2D (runtime de excepciones) y `__setenvp` 0x82D8F3 (arranque, ver 8.2) |
| E2-estado-1 | 15 | 1 enganchada (0x822FBD), **3 reconstruidas sin gancho** (`__mtinit`, `__mtterm`, `__mtdeletelocks`), 11 descartadas (funclets de desbloqueo y ayudantes del manejador de excepciones) |
| E2-sin-1 | 1 | **Cerrado**: `std::exception::what` 0x822E45 enganchada |

Cifras: 115 funciones en los siete lotes; 16 enganchadas, 4 reconstruidas sin gancho, 56 a enlazar y 39 a descartar (las funciones de locale no estaban en estos lotes: las lleva el otro agente). Las rutinas con hook nuevo van al catálogo (`ExeCrt`, 17 filas nuevas en `docs/hooks.csv`).

**Por qué `memcpy`/`memset`… se enlazan en vez de reescribirse.** `CrtString.cpp` evita deliberadamente los intrínsecos (bucles explícitos) para que el resultado no dependa de otra copia del CRT; aquí es lo contrario: son rutinas ensambladas cuyo resultado observable es el del estándar, así que la decisión «ENLAZAR el CRT de MSVC» se cumple llamando a las del CRT de la DLL. Al llegar al ejecutable propio el enlazador las aporta y los `RH_ScopedGlobalInstall` desaparecen.

## 5. F2: inicializadores estáticos (`_initterm`)

[fase8-inicializadores.csv](fase8-inicializadores.csv) lista los **1.673** (6 de la tabla C y 1.667 de la tabla C++; las entradas nulas no cuentan). Clasificación por cuerpo:

| Clase | Cantidad | Qué son | Tratamiento |
| --- | ---: | --- | --- |
| Constante x87 (`fld A; fdiv/fmul B; fstp D`, `fld A; fmul B; fdivr C; fstp D`, `fld A; fsub B; fstp D`) | 1.489 (1.487 de forma simple) | constantes que el compilador no plegó (p. ej. `grados * pi/180`); destino en `.bss` | **Reconstruidas**: `CrtStaticInit.cpp` + tabla generada `CrtStaticInitTable.inc` (formas 691 `A/B`, 518 `A*B`, 230 `C/(A*B)`, 48 `A-B`). `VerifyX87StaticInitializers` compara el cálculo en C++ con lo que dejó el EXE y se ejecuta al arrancar la DLL (traza `Constantes x87 de los inicializadores del CRT: N de M iguales…`); `RunX87StaticInitializers` las escribe en el ejecutable propio. Las dos restantes (`fcos` y una triple resta) quedan en DatosExe |
| Constructor + `atexit` (destructor estático) | 140 | objetos estáticos del juego (p. ej. 19 con 0x40FB60, 43 con 0x821D30) | pendiente: los aporta `DatosExe` como globales C++ propios (cada fila lleva constructor y destructor) |
| Asignación de datos | 35 | valores iniciales de globales | pendiente en `DatosExe` (8 de ellas citadas ya en el C++) |
| Llamada a constructor/función | 9 | `__onexitinit`, `__initstdio`, calloc/malloc de tablas… | las del CRT: `StandaloneStartup`; el resto, `DatosExe` |

Precisión: la x87 del CRT arranca en 53 bits (`_fpinit`), igual que `double` en C++ (`volatile` fuerza el redondeo tras cada operación aunque el compilador use x87); el destino `float` redondea al guardar. La verificación contra la memoria viva es la prueba de que esa hipótesis es cierta; si el humo la contradice, la traza da la primera dirección distinta.

## 6. F3 (importaciones y carga retardada) y F4 (funclets sin dueño)

**F3 (6 funciones, 120 bytes):** `DelayLoad_Direct3DCreate9` 0x807C10, `D3D9.DLL::Direct3DCreate9` 0x807C2B, `DelayLoad_DirectInput8Create` 0x817508, `DelayLoad_Ordinal_11` (DirectSoundCreate8) 0x818020, `DelayLoad_DirectDrawCreateEx` 0x818041 y `KERNEL32::RtlUnwind` 0x836AFC. Son ayudantes de `delayimp` y el thunk de importación: los genera el enlazador del ejecutable propio. Pasos exactos para el ejecutable propio: enlazar `d3d9.lib`, `dinput8.lib`, `dsound.lib` (y `ddraw.lib` si se conserva `WinPs.cpp`; `DelayLoadThunks.cpp` llama ya a `DirectInput8Create`/`DirectDrawCreateEx` por la IAT del EXE, hay que sustituir `kDirectInput8CreateSlot` 0x8E3EC4 y `kDirectDrawCreateExSlot` 0x8E3EBC por llamadas directas a la API, y retirar los dos `RH_ScopedGlobalInstall` 0x817523/0x81805C), y añadir `/DELAYLOAD:` solo si se quiere conservar la carga retardada. `RtlUnwind` lo importa el runtime de excepciones enlazado.

**F4 (44 funclets): no son huérfanos.** Sus tablas de desenrollado (0x88F3B0.. y 0x88F998..) cuelgan de dos `ehfuncinfo` (0x88F3FC, 10 estados; 0x88FAA4, 34 estados) cuyas envolturas (0x83821C y 0x838A54) cita un `push offset` en el prólogo de dos **despachadores de comandos de script** que Ghidra no separó como función: 0x472310 (comandos 2000–2099, 10 funclets) y 0x490DB0 (comandos 1500–1599, 34 funclets). Según `docs/ComandosScript.csv` ambos siguen siendo `despachador_exe` de 100 comandos cada uno con `despachador_reescrito = no`. Quedan vivos mientras esos despachadores sigan siendo del EXE; se descartan al reescribirlos (lote de comandos de script, fuera de esta parte). **Actualización (rama `vivo-f4`):** los dos despachadores ya están reescritos en C++ y los 44 funclets descartados: ver [ejecucion-f4.md](ejecucion-f4.md). Detalle por funclet en [fase8-funclets-f4.csv](fase8-funclets-f4.csv).

## 7. Verificación

* Compilación completa de la rama (`_build_VIVOP5.sh`, Debug x86): `EXIT=0`, los seis archivos nuevos de `Crt/` compilan. Humo con la DLL de la rama sobre el EXE: `prueba_ropero.py cal_mal.args` BIEN (varias corridas) y `prueba_agua.py cal_mal.args` BIEN.
* **Constantes x87 (F2):** con el juego en marcha, `contrib/fase8_verificar_x87.py` recalcula las 1.487 constantes con `float64` y las compara byte a byte con la memoria del proceso: **1.487 iguales, 0 distintas, 0 sin lectura**. Es la prueba de que la tabla y la aritmética de `CrtStaticInit.cpp` (mismas operaciones, 53 bits y redondeo a float al guardar) reproducen lo que calcula el EXE.
* `contrib/audit_hook_catalog.py --strict`: OK. `contrib/audit_cpp_assembly.py`: 0 `__asm`.
* La comprobación de las constantes x87 solo compara; no escribe memoria del EXE.

## 8. Pendiente y pasos exactos para el ejecutable propio

1. **Constructores estáticos y asignaciones de `_initterm` (175 filas):** convertirlas en globales C++ de `DatosExe` (cada fila de [fase8-inicializadores.csv](fase8-inicializadores.csv) trae destino, constructor y destructor). Hasta entonces `StandaloneStartup` deja esos objetos sin construir.
2. **`__setargv/__setenvp/_wincmdln`:** el `mainCRTStartup` del enlazador provee `__argc/__argv/_environ` propios; `getenv` (0x8240C7) y `_environ` del EXE (0xC9AC.. según `__setenvp`) deben leer los del CRT enlazado o reconstruirse (`__setenvp` 0x82D8F3, 105 bytes de lógica de copia).
3. **`exit/_doexit` 0x823BDB** (llamada hoy por `CrtFile.cpp` 0x823CAE): ejecuta la tabla de `atexit` ([0xC9D42C]..[0xC9D428]), las tablas de terminadores 0x8A5A34/0x8A5A40 y `ExitProcess`; en el ejecutable propio usar `exit` del CRT enlazado y migrar `Call823BDB` a él.
4. **Activar el arranque propio:** `cmake -D<proyecto>_STANDALONE_STARTUP=ON`; `WinMain` (bloque `NOTSA_STANDALONE`) llama a `ExeCrt::StandaloneStartup()` antes de cualquier función `ExeCrt` y devuelve su código si falla. Comprobar con la traza de las constantes x87 (debe dar «N de N iguales» con la DLL sobre el EXE) y arrancar sin el EXE original.
5. **Quitar `RH_ScopedGlobalInstall` de las rutinas enlazadas** (`CrtMem`, `CrtStrtod`, montón) cuando el EXE original deje de estar en el proceso; los llamadores propios ya usan `::operator new/delete` o `ExeCrt::*`.
6. **`__active_heap == 3`:** no se reproduce el montón de bloques pequeños (0x82C556 y 0x82C59E); sin uso desde Windows 2000.
7. **F3:** enlazar las importaciones directas como en la sección 6.

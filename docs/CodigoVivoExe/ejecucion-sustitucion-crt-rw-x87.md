<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Sustitución de tres bloques que dependían del EXE o estaban sin validar: CRT (montón), ayudantes de RenderWare y ramas x87

Rama `sustituir-crt-rw-x87` (desde `integracion-fase7b` 48e53f065c, 24/11/2026; worktree `D:\Original\wv-crt`). Todo en C++ sin `__asm`/`naked`; los hooks nuevos con `RH_ScopedGlobalInstall`; no se hizo push ni merge. Los números de las notas viejas estaban atrasados: este documento da el estado real encontrado antes de tocar nada.

## 1. CRT: las 6 envolturas de `MemoryMgr.cpp`

### Estado real encontrado

* `docs/implementaciones/envolturas-pendientes.csv` listaba 10 filas; 6 eran el CRT (`_free` 0x82413F, `Realloc` 0x824269, `operator new` 0x82119A, `new[]` 0x821195, `operator delete` 0x8214BD, `delete[]` 0x8213AE). Los números de línea del CSV (355-568) eran de una versión anterior del archivo.
* El C++ propio **ya existía** (`ExeCrt::Free/Realloc/Malloc/Calloc/OperatorNew/OperatorNewArray/OperatorDelete/OperatorDeleteArray`, `Crt/CrtHeap.cpp`, banco `FnCRT1A`/`FnNEWDEL` con 0 diferencias) y los ganchos (`GTASA_CRT_HEAP_HOOKS`, ON por defecto en `source/CMakeLists.txt` y en `cl.rsp`) estaban instalados, pero:
  * `CMemoryMgr::Free/Realloc/Malloc/Calloc/MallocAlign/FreeAlign` seguían llamando al EXE por dirección (`reinterpret_cast<...>(0x82413F)` etc.; con el gancho puesto caía en el `jmp` hacia `ExeCrt::Free`, pero la envoltura al EXE seguía en el código);
  * los operadores globales del proyecto (`#ifndef NOTSA_STANDALONE`) llamaban a `ExeCrt::*` solo con `__active_heap == 1` y, si no, al EXE por dirección;
  * los 13 ganchos de `CrtHeapInjectHooks` (namespace `ExeCrtHeap`) **no estaban en `docs/hooks.csv`**, así que `pending_wrappers.py` no descartaba las filas por dirección.
* Otras llamadas directas por dirección a `free`/`new` del CRT: `CrtCtype.cpp`, `CrtException.cpp`, `CrtLocale.cpp`, `MemoryHeap.cpp` (`Shutdown`), `D3DResourceSystem.cpp` (2 `operator new`), `rwplcore.cpp` (`rwfree`/`rwmalloc`/`rwrealloc`/`rwcalloc` por defecto).

### Qué se sustituyó

* `MemoryMgr.cpp`: `CMemoryMgr::Malloc/Free/Realloc/Calloc/MallocAlign/FreeAlign` y los cuatro operadores llaman directamente a `ExeCrt::*`. Se quitó el camino por dirección al EXE (era para `__active_heap != 1`, el montón de bloques pequeños de Windows 9x que `ExeCrt` no reproduce y que este juego no alcanza; y antes de que el EXE cree su montón el EXE tampoco podía asignar: `HeapAlloc` con manejador nulo). `FreeAlign` conserva la escritura sobre la ranura de argumento (la exige la comprobación `argument_after_matches`).
* `MemoryHeap.cpp` (`Shutdown`), `D3DResourceSystem.cpp` (`::operator new`), `rwplcore.cpp` (valores por defecto de `RwEngineInstance->memoryFuncs`: `&ExeCrt::Free`, `ExeCrt::Malloc/Realloc/Calloc`), `CrtCtype.cpp`, `CrtException.cpp`, `CrtLocale.cpp`: `free` por `ExeCrt::Free`. Quedan por dirección los `malloc` (0x824257) de `CrtLocale.cpp`/`CrtCtype.cpp` y el `calloc` de `CrtLocale.cpp` (están enganchados y catalogados: `CrtMalloc`, `ExeCrtHeap,Calloc`); no son parte de las seis.
* `docs/hooks.csv`: 13 filas `ExeCrtHeap,*` (`reversed=1`, `validated=0`); `python contrib/pending_wrappers.py`: **10 → 4** (quedan las 4 de QuickTime / `CAEMFDecoder`, que no son de este bloque). `python contrib/audit_hook_catalog.py --strict`: OK.

### Compatibilidad de memoria (qué se cuidó)

* **Un solo montón.** `ExeCrt::*` trabajan sobre el mismo `_crtheap` (0xC9C2F4) que el EXE vivo y los ganchos: en la ASI, el EXE llama a `malloc/free/new/delete` y, con `GTASA_CRT_HEAP_HOOKS`, caen en `ExeCrt` (mismo manejador); sin los ganchos caen en el código del EXE sobre el mismo manejador. Un bloque pedido por uno se libera, redimensiona o mide con otro. `new` del proyecto en la ASI = `ExeCrt::OperatorNew`; `delete` del EXE sobre memoria del proyecto va al mismo `HeapFree`.
* **Ejecutable independiente** (`NOTSA_STANDALONE`): los operadores globales son los del CRT enlazado (otro montón); la memoria de `CMemoryMgr` y del CRT propio es `_crtheap` creado por `ExeCrt::StandaloneStartup`. No hay `new`/`CMemoryMgr::Free` cruzados (`D3DResourceSystem` usa `::operator new`/`::operator delete` en los dos lados).
* **`rand`/locale/`errno`:** el montón no toca el estado de `rand` (0x821B1E, `ExeCrt::Rand`), del locale ni `errno`; no hay nada que preservar más allá de no cambiar esos módulos (no se tocó ninguno).

## 2. Los «ayudantes de RenderWare sin validar» y los «inmediatos x87»

### Estado real encontrado (las notas viejas no coinciden exactamente)

* No existe ninguna lista literal de «4 ayudantes de RenderWare» ni de «inmediatos x87» en `docs/` ni en `reversing/`. Lo que hay con `validated=0`:
  * `RwHelper`: **3** entradas (no 4): `RpClumpBoundingSphereCenterCallback` 0x734970, `RpClumpBoundingSphereRadiusCallback` 0x734ED0 (lote S2A) y `TexDictDestroyTextureCallback` 0x730E50 (S22A). Su compañera `RpClumpGetBoundingSphere` 0x734FC0 figura `validated=1`, pero el banco `Helpers66D` **no ejecutó** los dos callbacks (usó un falso de `RpClumpForAllAtomics`; `reversing/Helpers66D/README.md`). Se consideran los cuatro.
  * `RwMathX87` (rutinas x87 de `_rwcseg`): 7 entradas, `validated=0` en el registro aunque `reversing/FnP3` las banqueó con 0 diferencias (la evidencia se anuló por el hash de `InjectHooksMain.cpp`).
  * «Inmediatos x87»: la única mención es `docs/CodigoVivoExe/ejecucion-enlace-r5.md` §3: «tres inmediatos de CRT en `ExeCrt::Floor/Ceil/Pow` (0x828A60, 0x82BA31, 0x822184): ramas x87 sin SSE2/SSE4.1». Son llamadas por dirección al EXE desde `Floor`, `Ceil` y `Pow` (`CrtMath.cpp`, `CrtPow.cpp`).

### Comprobación puntual contra el EXE original (sin banco por función)

Método de `reversing/FnP3` (el `gta_sa.exe` original solo lectura se mapea en 0x400000 dentro de un proceso de prueba de 32 bits y se llama por su dirección; el juego no se ejecuta). Resultados en `reversing/FnP3/resultados/`.

| Qué | Cómo | Resultado |
|---|---|---|
| `RwMathX87` (0x857000, 0x85707F, 0x8570EF, 0x857160, 0x8572A0, 0x857404, 0x85742B) | `bench_p3.exe` (ya existía), repetido con la semilla 20261124, 400.000 casos por sección, x87 a 24 bits | 0 diferencias (`corrida_x87_2026-11-24.txt`) |
| 0x734970, 0x734ED0, 0x734FC0, 0x730E50 | **banco nuevo** `bench_rwhelper.cpp`: se extrae el código REAL de `RwHelper.cpp`/`rwtexdict.cpp` y se compila como Debug del proyecto con `X87Math.cpp` (sus opciones por archivo); los colaboradores son falsos deterministas con traza, idénticos en los dos lados (en la copia del EXE se parchean las siete llamadas). Se compara el retorno, todos los bytes de la escena, la traza de colaboradores y el global 0x8D60BC. Entradas con NaN, infinitos, subnormales, ceros con signo, indicador de resincronizado, global con cualquier byte y clump/esfera nulos; x87 a 24 bits | 4 x 500.000 casos, **0 diferencias**; prueba negativa `neg_rwhelper.py`: 8 de 8 mutaciones detectadas (`corrida_rwhelper_2026-11-24.txt`) |
| Ramas x87 de `floor`/`ceil` (0x828A60, 0x82BA31) | `bench_p3.exe --solo x87`: resultado en bits y `errno`, con `[0x8E3AE0]` = 0x9876 (valor de arranque del EXE) y 0, x87 a 24 y 53 bits, NaN (silenciosos y señalizadores), ±inf, subnormales, enteros, mitades, 2^52 | 3 x 1.000.000 casos, 0 diferencias; 2 mutaciones detectadas |

### Qué se sustituyó

* `Floor`/`Ceil` (`CrtMath.cpp`): la rama sin SSE2 ya no llama a 0x828A60/0x82BA31. Es el mismo núcleo de enteros de la rama SSE2 (`FloorCore`/`CeilCore`) con las excepciones de MXCSR enmascaradas durante la llamada; un NaN fija `errno = EDOM` y devuelve el argumento (lo que hace el EXE con `[0x8E3AE0] != 0`, que es siempre su valor). Para todo valor finito o infinito el resultado es el entero exacto, igual que `frndint` con el redondeo hacia abajo/arriba del EXE (comprobado). **No reproducido:** la excepción de operación inválida con la x87 sin enmascarar y un NaN señalizador (`fadd [0x85A310]`), y el `struct _exception` que el EXE (con `[0x8E3AE0] == 0`) entrega al `matherr` por defecto (que devuelve 0; solo `errno` es observable).
* `Pow` (`CrtPow.cpp`): la rama x87 (0x822184, usa `fyl2x` en 0x8221DA y, vía 0x828EF0.., `f2xm1` y `fscale`) **no es reconstruible bit a bit en C++ puro**: C++ no tiene intrínsecos de esas instrucciones, el compilador no las emite y la x87 las calcula con microcódigo propio de la CPU (no hay definición portable que copiar). Se sustituye por el núcleo SSE2 (`PowSse2`) con las excepciones de MXCSR enmascaradas: mismos dominios y errores (por `ExeCrt::MathError`), resultado correcto pero **no idéntico** al de la x87 en cualquier entrada, y no se midió la diferencia máxima (el intento de ejecutar 0x822184 en el proceso de prueba terminó en una violación de acceso con EIP fuera del módulo, probablemente por estado del CRT de coma flotante sin inicializar; no se diagnosticó y el código del intento se retiró del banco). La rama solo se alcanza sin SSE2 o con excepciones sin enmascarar: en la práctica nunca con el juego.

### Lo que no cambia de estado

Los flags `validated` de `docs/hooks.csv` **no se tocaron**: dependen de la evidencia aprobada del registro (`contrib/sync_validated_flags.py`; con 11.798 aprobadas frente a 11.792 del CSV actual el generador de documentos ya se negaba a correr antes de esta rama). La evidencia nueva vive en `reversing/FnP3/resultados/`; registrarla en el registro de validaciones no se hizo.

## 3. Compilación y pruebas con el juego (24/11/2026)

* **ASI**: `build-project.ps1` en `D:\Original\wv-crt` (candado `compile_lock`), `EXIT=0` (hubo un primer intento con un solo error, un `[[nodiscard]]` ignorado en `D3DResourceSystem.cpp`; corregido con `(void)`). `gta_reversed.asi` SHA256 `7f4acf1a…da94b`.
* **Independiente**: `reversing\FnFase8\enlazar.cmd` con los `.obj` de esa compilación: `link exit=0`, sin símbolos sin resolver (`salida\link.log`).
* **Independiente en marcha**: `probar_imagen.py` (candado `gta_lock`, carpeta propia `D:\Original\_crt_run`, ini con las partidas redirigidas a esa carpeta, sin `gta-sa.exe`): 120 s, capturas a t = 30/60/90/120 s (cinemática del aeropuerto con texturas; la de t = 120 mirada con la herramienta de imagen), salida por la guardia de tiempo (`0xDEAD0002`), CSV de código original vacío, sin excepciones (`reversing/FnFase8/ejecuciones/crt_sa-20261124-171932`).
* **ASI, humo** (`FnBIS1`, `AGUA_ASI` = mi ASI, copia de pruebas; sin prueba del agua): `prueba_ropero.py cal_mal.args`: **BIEN** (15 entidades, 12 marcadores, puerta y marcador); `prueba_marcadores.py cal_mal.args`: **BIEN** (12 marcadores en memoria, los 12 cambian en 1 s).
* **ASI, partida larga** (`reversing/FnP3/asi_larga.py`: 330 s con `ForegroundApp = 1`, el jugador se lleva cada 25 s a 10 puntos del mapa para forzar transmisión y reservas, con muertes y reapariciones): una ejecución completa de la ASI de la rama (340 s): **sin caídas durante la partida** (estado 9 todo el rato; una aserción `result` de un comando de script, «se continua», sin relación con el montón). Al cerrar (`CloseMainWindow` de `jugar.ps1 -Auto`) hubo una **violación de acceso en `CEntity::CleanUpOldReference` + 0x34 (`Entity.cpp:1811`)**. La misma partida con la ASI de la **base 48e53f065c sin estos cambios** (compilada aparte, mismas opciones; 350 s) **cae igual al cerrar** (mismo módulo, `0xc0000005`, desplazamiento 0x01C4C104; su memoria privada pasó de 59 a 485 MB), así que es previa a esta rama: una referencia a entidad ya liberada al apagar el juego tras muchos teletransportes. No se investigó. Una partida corta de 62 s con mi ASI cerró bien (código 0, sin registro de caída). Una segunda partida larga mía se perdió por la nota de método de abajo y una tercera no llegó a ejecutarse (el candado `gta_lock` lo tuvo otro agente más de 15 minutos y la cancelé): **solo hay una partida larga completa de la ASI de la rama**, sin medida de memoria.
* **Nota de método**: en una de mis ejecuciones otra prueba ajena (otro agente) mató mi juego a los 40 s (código de salida 1 de `taskkill`, sin registro de caída de Windows): las pruebas de `FnBIS1` adoptan como propio cualquier `gta_sa.exe` si su `jugar.ps1` pierde la carrera por `gta_lock`. No se tocó ningún proceso ajeno.

### No verificado

* Memoria de la ASI frente a fugas o mezclas durante horas (la partida larga dura 5,5 minutos).
* El montón del CRT con `__active_heap != 1` (Windows 9x): no se reproduce y se quitó el camino por dirección que lo llamaba.
* Las ramas x87 de `floor`/`ceil`/`pow` con la x87 realmente sin enmascarar dentro del juego (no se alcanzan), y la diferencia numérica de `pow` frente a la x87.
* Ninguna de las funciones nuevas está registrada en el registro de validaciones (`validated=0` en `docs/hooks.csv`); la evidencia es la de `reversing/FnP3/resultados/`.
* `docs/implementaciones/funciones.csv` / `ReversedClasses.md` no se regeneraron (el generador se negó a correr por una discrepancia previa de 11.792 frente a 11.798 validaciones aprobadas y, con `--update-source-status`, reescribía 57 archivos ajenos antes de fallar; se revirtió).

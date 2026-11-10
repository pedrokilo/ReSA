<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 883 grupos / 9890 funciones reconstruidas; 9890 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn145A: Shutdown de CAEWMADecoder y 21 destructores con borrado (lote 145A)

Estado: **APROBADO dentro del juego** (09/11/2026, worktree `wv-2` sobre main f0d9d49b02 + los cambios del lote; el encargo decia `wv-1`, ocupado por otro lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn145a/20261109-185059-031229` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn145A-20261109-185059-031229`; `comparisons.jsonl.gz` de 47,1 MB, bajo el limite de GitHub). 22 x 16384 = **360448 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `b322677330793769115d06daf00a23753cb0cb166721a904527c4aa3bbf43f6f`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Las 22 direcciones del listado se reconstruyen y validan (ninguna estaba enganchada: se comprobo el codigo, `docs/hooks.csv` y `git worktree list`, que solo muestra main, `wv-1`, `wv-3` y este `wv-2`). Las clases del listado se atribuian por cercania y **las 20 de tareas eran incorrectas** (y la A de `CAEWMADecoder` no es un destructor sino `Shutdown`): los nombres reales salen del desensamblado, de las vtables leidas del propio EXE y de las vtables que ya conoce el codigo.

| # | Funcion real (el listado decia) | Entrada | Bytes | Fuente (`source/game_sa/`) | Tipo |
|---|---|---|---|---|---|
| 0 | `CAEWMADecoder::Shutdown` (el listado: `CAEWMADecoder`, A) | 0x502BF0 | 32 | `Audio/Loaders/AEWMADecoder.cpp` | S |
| 1 | `CAEWMADecoder` destructor con borrado (ranura 6 de 0x862D90; el listado: B) | 0x502C40 | 27 | `Audio/Loaders/AEWMADecoder.cpp` | S |
| 2 | `CTaskComplexCarSlowBeDraggedOutAndStandUp` destructor con borrado (ranura 0 de 0x86EF80; el listado: `CTaskComplexCarSlowBeDraggedOut`) | 0x64C930 | 30 | `Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp` | S |
| 3 | `CTaskSimpleBikeJacked` destructor con borrado (ranura 0 de 0x86EFAC; el listado: `CTaskComplexCarSlowBeDraggedOut`) | 0x64C950 | 30 | `Tasks/TaskTypes/TaskSimpleBikeJacked.cpp` | S |
| 4 | `CTaskComplexWanderFlee` destructor con borrado (ranura 0 de 0x86F690; el listado: `CTaskComplexFleeAnyMeans`) | 0x65D0C0 | 30 | `Tasks/TaskTypes/TaskComplexWanderFlee.cpp` | S |
| 5 | `CTaskComplexFleePoint` destructor con borrado (ranura 0 de 0x86F6CC; el listado: `CTaskComplexFleeAnyMeans`) | 0x65D0E0 | 30 | `Tasks/TaskTypes/TaskComplexFleePoint.cpp` | S |
| 6 | `CTaskComplexFollowPointRoute` destructor con borrado (ranura 0 de 0x870078; el listado: `CTaskComplexFollowPointRoute`) | 0x671610 | 30 | `Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp` | S |
| 7 | `CTaskComplexFollowNodeRoute` destructor con borrado (ranura 0 de 0x8700A8; el listado: `CTaskComplexFollowPointRoute`) | 0x671730 | 30 | `Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp` | S |
| 8 | `CTaskComplexHitByGunResponse` destructor con borrado (ranura 0 de 0x86E06C; el listado: `CTaskComplexHitResponse`) | 0x638160 | 30 | `Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp` | S |
| 9 | `CTaskComplexSunbathe` destructor con borrado (ranura 0 de 0x86E0AC; el listado: `CTaskComplexHitResponse`) | 0x638180 | 30 | `Tasks/TaskTypes/TaskComplexSunbathe.cpp` | S |
| 10 | `CTaskSimpleGangDriveBy` destructor con borrado (ranura 0 de 0x86D944; el listado: `CTaskComplexKillPedOnFootArmed`) | 0x6275E0 | 30 | `Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp` | S |
| 11 | `CTaskComplexDestroyCar` destructor con borrado (ranura 0 de 0x86D968; el listado: `CTaskComplexKillPedOnFootArmed`) | 0x628780 | 30 | `Tasks/TaskTypes/TaskComplexDestroyCar.cpp` | S |
| 12 | `CTaskSimplePlayerOnFire` destructor con borrado (ranura 0 de 0x86E334; el listado: `CTaskComplexOnFire`) | 0x639310 | 30 | `Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp` | S |
| 13 | `CTaskSimpleIKManager` destructor con borrado (ranura 0 de 0x86E358; el listado: `CTaskComplexOnFire`) | 0x639330 | 30 | `Tasks/TaskTypes/TaskSimpleIKManager.cpp` | S |
| 14 | `CTaskComplexGangJoinRespond` destructor con borrado (ranura 0 de 0x86FB5C; el listado: `CTaskComplexPassObject`) | 0x664A80 | 30 | `Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp` | S |
| 15 | `CTaskComplexProstituteSolicit` destructor con borrado (ranura 0 de 0x86FB88; el listado: `CTaskComplexPassObject`) | 0x664AA0 | 30 | `Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp` | S |
| 16 | `CTaskComplexWaitForDryWeather` destructor con borrado (ranura 0 de 0x86E160; el listado: `CTaskComplexUseAttractor`) | 0x638900 | 30 | `Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp` | S |
| 17 | `CTaskComplexWaitForBus` destructor con borrado (ranura 0 de 0x86E18C; el listado: `CTaskComplexUseAttractor`) | 0x638920 | 30 | `Tasks/TaskTypes/TaskComplexWaitForBus.cpp` | S |
| 18 | `CTaskComplexAttractorPartnerWait` destructor con borrado (ranura 0 de 0x86E284; el listado: `CTaskComplexUsePairedAttractor`) | 0x638F00 | 30 | `Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp` | S |
| 19 | `CTaskComplexUseSequence` destructor con borrado (ranura 0 de 0x86E518; el listado: `CTaskComplexUsePairedAttractor`) | 0x639F90 | 30 | `Tasks/TaskComplexUseSequence.cpp` | S |
| 20 | `CTaskComplexWanderCriminal` destructor con borrado (ranura 0 de 0x85A23C; el listado: `CTaskComplexWanderCriminal`) | 0x48E700 | 30 | `Tasks/TaskTypes/TaskComplexWanderCriminal.cpp` | S |
| 21 | `CTaskComplexGoToAttractor` destructor con borrado (ranura 0 de 0x86FF3C; el listado: `CTaskComplexWanderCriminal`) | 0x670400 | 30 | `Tasks/TaskTypes/TaskComplexGoToAttractor.cpp` | S |

Tipo S = `RH_ScopedNamedGlobalInstall` (destructores con borrado: `RH_ScopedVMTInstall` no puede nombrar la ranura 0 del destructor virtual, como en los lotes 107A, 115A, 140A y 143A) y `RH_ScopedInstall` (Shutdown). Las 22 filas nuevas de `docs/hooks.csv` siguen el formato `Clase,Fn,0xADDR8digitos,1,0,1,S`.

## Funciones saltadas
Ninguna: las 22 son viables en C++ puro (ninguna necesita `__asm`, `naked`, registros que C++ no pueda leer ni opciones de compilacion nuevas; `X87Math.cpp` no se toca). Lo unico que cambia respecto al EXE son registros volatiles y EFLAGS que ningun llamador lee.

## Revision contra el EXE (desensamblado con `dumpbin /disasm` del EXE canonico y vtables leidas del PE)
- **0x502BF0 (0)**: `mov eax,[0B6BAD8h]; test eax,eax; je fin; mov eax,[0B6BAD4h]; push eax; call [858110h]; mov [0B6BAD8h],0; ret`: si `WMCreateSyncReader` (0xB6BAD8) no es nulo, llama a `FreeLibrary` (la casilla de IAT 0x858110) con el manejador de `wmvcore.dll` (0xB6BAD4) y pone a cero SOLO el puntero. **Correccion de produccion**: la version anterior de `CAEWMADecoder::Shutdown` tambien anulaba el manejador (`wmvCoreModule = nullptr`), que el EXE deja intacto; ya no lo hace. Un solo llamador en el codigo reconstruido (`AEUserRadioTrackManager.cpp`).
- **0x502C40 (1)**: la plantilla de 27 bytes de los lotes 124A / 125A / 140A: `push esi; mov esi,ecx; call 0x502760; test byte [esp+8],1; je +; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`. Ranura 6 de la vtable 0x862D90 (0x862D70 es `CAEVorbisDecoder`, con 0x502700 del lote 124A). 0x502760 es `~CAEWMADecoder` (con hook propio), 0x8214BD el `operator delete` del CRT (sin hook).
- **20 de tareas (2..21)**: la plantilla de 30 bytes `push esi; mov esi,ecx; call <cuerpo>; test byte [esp+8],1; je +; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4` con `CTask::operator delete` (0x61A5B0, con hook). Solo cuenta el bit 0 del BYTE bajo de la ranura de pila; devuelve `this`. Cada una es la ranura 0 de la vtable de su clase (el dword de la vtable coincide con la direccion de la funcion):
  - 0x64C930 = vtable 0x86EF80 de `CTaskComplexCarSlowBeDraggedOutAndStandUp`, cuerpo 0x648690 (con hook propio)
  - 0x64C950 = vtable 0x86EFAC de `CTaskSimpleBikeJacked`, cuerpo 0x648C40 (con hook propio)
  - 0x65D0C0 = vtable 0x86F690 de `CTaskComplexWanderFlee`, cuerpo 0x65B370 (con hook propio)
  - 0x65D0E0 = vtable 0x86F6CC de `CTaskComplexFleePoint`, cuerpo 0x65B410 (con hook propio)
  - 0x671610 = vtable 0x870078 de `CTaskComplexFollowPointRoute`, cuerpo 0x66E050 (SIN hook)
  - 0x671730 = vtable 0x8700A8 de `CTaskComplexFollowNodeRoute`, cuerpo 0x66EB70 (con hook propio)
  - 0x638160 = vtable 0x86E06C de `CTaskComplexHitByGunResponse`, cuerpo 0x631E30 (con hook propio)
  - 0x638180 = vtable 0x86E0AC de `CTaskComplexSunbathe`, cuerpo 0x632050 (con hook propio)
  - 0x6275E0 = vtable 0x86D944 de `CTaskSimpleGangDriveBy`, cuerpo 0x6218C0 (SIN hook)
  - 0x628780 = vtable 0x86D968 de `CTaskComplexDestroyCar`, cuerpo 0x621CB0 (con hook propio)
  - 0x639310 = vtable 0x86E334 de `CTaskSimplePlayerOnFire`, cuerpo 0x6335B0 (con hook propio)
  - 0x639330 = vtable 0x86E358 de `CTaskSimpleIKManager`, cuerpo 0x633830 (con hook propio)
  - 0x664A80 = vtable 0x86FB5C de `CTaskComplexGangJoinRespond`, cuerpo 0x661720 (con hook propio)
  - 0x664AA0 = vtable 0x86FB88 de `CTaskComplexProstituteSolicit`, cuerpo 0x661AF0 (con hook propio)
  - 0x638900 = vtable 0x86E160 de `CTaskComplexWaitForDryWeather`, cuerpo 0x632830 (con hook propio)
  - 0x638920 = vtable 0x86E18C de `CTaskComplexWaitForBus`, cuerpo 0x632930 (con hook propio)
  - 0x638F00 = vtable 0x86E284 de `CTaskComplexAttractorPartnerWait`, cuerpo 0x633290 (con hook propio)
  - 0x639F90 = vtable 0x86E518 de `CTaskComplexUseSequence`, cuerpo 0x6396C0 (SIN hook)
  - 0x48E700 = vtable 0x85A23C de `CTaskComplexWanderCriminal`, cuerpo 0x48E720 (con hook propio)
  - 0x670400 = vtable 0x86FF3C de `CTaskComplexGoToAttractor`, cuerpo 0x66B6A0 (con hook propio)

## Cambios de produccion
- En cada uno de los 21 `.cpp` de clase (`AEWMADecoder.cpp` y los 20 de tareas): una envoltura `XDestructorBody` (`__declspec(noinline)`, llama a `~X()` de la clase), la funcion `XScalarDeletingDestructor` (cuerpo; `CTask::operator delete(self)` o `::operator delete(self)` solo con el bit 0 del byte de banderas; devuelve `this`), el enganche `RH_ScopedNamedGlobalInstall(..., "ScalarDeletingDestructor", 0x...)` y una funcion libre exportada `Lote145ABody_<Clase>()` que devuelve la direccion de la envoltura (solo la usa el banco). No se toca ninguna cabecera.
- `AEWMADecoder.cpp`: ademas `RH_ScopedInstall(Shutdown, 0x502BF0)` y la correccion de `Shutdown` descrita arriba.
- 22 filas nuevas en `docs/hooks.csv`; registro de evidencia con `register_hook.py`. No se ejecuto `sync_docs` / `hooks2md`.

## Banco
Una funcion por ruta: original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Objetos sinteticos en tres arenas propias (A, B, C; ventana de 4 KiB con guardas de 0x40; `this` alineado, desalineado, al borde de la ventana o cerca del inicio) y, para `Shutdown`, la memoria real del juego 0xB6BAD4..0xB6BADB (manejador y `WMCreateSyncReader`: se guardan al empezar y se restauran tras cada lado).
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (`MakeCollab` / `H` / `U` / `E`, plantilla Fn143A): el cuerpo del destructor y `CTask::operator delete` 0x61A5B0 (con hook: se apagan y el falso va tambien en la funcion de la DLL: la envoltura `XDestructorBody`), los tres cuerpos sin hook (0x66E050 de `CTaskComplexFollowPointRoute`, 0x6218C0 de `CTaskSimpleGangDriveBy` y 0x6396C0 de `CTaskComplexUseSequence`, `U`: funcion propia de la DLL que el banco parchea) y el `operator delete` del CRT 0x8214BD (`E`, sin hook: la DLL lo llama por esa direccion).
El falso del cuerpo registra [direccion del EXE del cuerpo, `this` como etiqueta de arena, dword que habia en [ECX]], deja un dword del caso en [ECX] (la vtable que el destructor real escribiria) y devuelve un EAX que no es `this`; el de `operator delete` registra [direccion, puntero, dword de [puntero]]: prueba que se borra DESPUES de destruir. **FreeLibrary** (Shutdown) se sustituye en la IAT: la casilla 0x858110 del EXE (el banco comprueba que contiene la `FreeLibrary` real) y las casillas del ASI que apuntan a ella, solo alrededor de la llamada (patron del banco `Audio15`); el falso registra [0x858110, manejador] solo en el hilo del banco y devuelve un BOOL del caso.
Se observa: bytes cambiados de las tres arenas y de la memoria global, eventos de los falsos con todos sus argumentos, valor devuelto (`this` como etiqueta en los destructores; `Shutdown` es void), hash de las regiones globales y estado x87 final (vacio). **EAX, ECX, EDX y EFLAGS tras la llamada** quedan en `original_residue` / `candidate_residue` y no forman parte de la igualdad (volatiles; el unico valor que lee un llamador, EAX = `this` en los destructores con borrado, **es identico entre EXE y DLL en los 360448 pares**: va en la observacion `ret`). Las diferencias informativas (`pairs_with_different_eax_ecx_edx_info_only` del `summary.json`, que compara la fila de residuos completa, con EFLAGS) son de 8553 a 8805 de 16384 en cada destructor (EFLAGS aritmeticos) y 16384 en `Shutdown` (EAX previo y EFLAGS).
16 familias de 1024 casos por funcion (ver `Fixtures.hpp`): destructores: posicion de `this` x relleno de las arenas, vtable previa igual, con otro bit o ajena, banderas 0, 1, 2, 3, 0xFF, 0x100, 0x101, bits altos y aleatorias (solo cuenta el byte bajo); `Shutdown`: manejador 0 / 0xFFFFFFFF / de la imagen del EXE / aleatorio x `WMCreateSyncReader` nulo / aleatorio / valores limite, y BOOL del falso.
`python -I fixture_selfcheck.py` (sin juego) pasa los 360448 casos por el oraculo y mide la cobertura de ramas (marcas de `KEYS`, minimo 50).

## Oraculo independiente
`fixture_oracle.py` reimplementa solo lo observable desde el desensamblado (memoria escrita, eventos, valores devueltos, la tabla de cuerpos y de `operator delete`); no usa ninguna constante ni rama del C++ de produccion.

## Fuera de los fixtures
`this` nulo o crudo, objetos reales construidos por el EXE (el banco usa objetos sinteticos; no se destruye ninguna tarea real), el cuerpo real de los destructores y de `CTask::operator delete` / `operator delete` (falseados; los destructores de clase tienen sus propios bancos) y la `FreeLibrary` real con un manejador cargado (la IAT se sustituye por un falso; `wmvcore.dll` real no se descarga).

## Prueba negativa
Dos rondas de mutaciones (`mutate.py`, una por funcion y ronda, 22 por ronda; se aplican a las fuentes de produccion, se recompila la sonda y se corre con `FN145A_CASES=2048` casos por funcion; el banco y `run_local.py` leen la misma variable y la corrida aprobada usa 16384). Como los hooks de la DLL estan activos en el juego real antes de que empiece el banco, las mutaciones se condicionan a entradas que el juego no produce (banderas con el bit 1 o el 7, `this` desalineado, manejador 0xFFFFFFFF). **Las 22 funciones fallan en ambas rondas** (`negative-runs/`, rondas `20261109-191319-927035` y `20261109-192345-018767`, con `diferencias-por-funcion.txt` y `summary.json`; el lado EXE coincide con el oraculo en los 2048 pares de cada funcion y ronda: "lado EXE fuera del oraculo 0"). Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas, comprobado: `restauradas 21 fuentes`). SHA256 (8 primeros) de las fuentes buenas: AEWMADecoder.cpp 13cd9eca, CarSlowBeDraggedOutAndStandUp 6030fa66, BikeJacked 0dd019f6, WanderFlee 6f5a7ba5, FleePoint b954f4a2, FollowPointRoute ae5262a8, FollowNodeRoute d7356959, HitByGunResponse f6f95080, Sunbathe 5749353d, GangDriveBy 2f3827c1, DestroyCar b1202718, PlayerOnFire 13708b62, IKManager 8727a93b, GangJoinRespond e6b037e6, ProstituteSolicit 88cc548d, WaitForDryWeather b2596246, WaitForBus 48e82148, AttractorPartnerWait f52abd94, UseSequence ce2eb40a, WanderCriminal a3d59225, GoToAttractor 40f9db89 (`python -I mutate.py sha` imprime los completos).
- Ronda 1 (pares con diferencias de 2048; 8663 de 45056): `Shutdown` anulando tambien el manejador 1020; los 21 destructores borrando tambien con la bandera 2 (`flags & 3`): de 330 a 404 cada uno.
- Ronda 2 (15909 de 45056): `Shutdown` sin liberar con el manejador -1: 364; destruccion doble con la bandera 0x80 (`CAEWMADecoder` y las clases de indice par de la tabla: de 574 a 617) y retorno nulo con `this` desalineado (las de indice impar: de 876 a 919).

## Bancos repetidos
Citan las direcciones del lote (grep de `0x...` con y sin ceros y de la forma `...h` en `reversing/`, sin contar este banco, `_handoff`, `artifacts`, la evidencia archivada, el registro ni las copias fuente de `CppIntegerWorld10/runs`) un solo banco de codigo, **`Audio15`** (parchea 0x502BF0 con `both(0x502BF0, &CAEWMADecoder::Shutdown, FakeWmaShutdown)`, EXE y cuerpo de la DLL); los demas resultados son notas de texto de `TasksPendingEvidence` (vtables de `CTaskSimpleGangDriveBy` y `CTaskComplexDestroyCar`), que no ejecutan nada. Por la regla del lote no se repitieron otros bancos de tareas aunque ahora pasen por estos destructores.
- `Audio15` repetido con el codigo final (compilado y corrido bajo los candados compartidos): **aprobado sin adaptar**, 11008 pares, cero diferencias (`resultados-audio15/20261109-192934-046339`, 30 filas del registro actualizadas con `refresh_bank.py`, archivada en `reversing/CppTwelveLevel6Final/runs/Audio15-20261109-192934-046339`).

## Tiempo
Una corrida de los 360448 pares: unos 17 min (el `comparisons.jsonl` crudo ocupa 676 MB; se archiva como `.gz` y se borra el crudo). La compilacion incremental de la sonda tarda de 1 a 8 minutos segun el reparto de los candados compartidos; las rondas negativas de 2048 casos por funcion, unos 2 minutos de juego cada una.

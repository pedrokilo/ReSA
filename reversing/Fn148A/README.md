<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 879 grupos / 9839 funciones reconstruidas; 9839 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn148A: 20 funciones diminutas del EXE (lote 148A)

Estado: **APROBADO dentro del juego** (09/11/2026, worktree `wv-1` sobre main f990ef21b1 + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn148a/20261109-205508-963723` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn148A-20261109-205508-963723`; `comparisons.jsonl.gz` de 40,7 MB, bajo el limite de GitHub). 20 x 16384 = **327680 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `df93822590cb95fd7cb65bc23c559828c92c70f6c3b16936ef5edb1a9a7c5978` (la sonda se recompilo despues con las mismas fuentes buenas para la prueba negativa; el manifiesto actual tiene otro hash de binario pero los mismos SHA256 de fuentes); del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Ninguna de las 20 direcciones tenia hook (se comprobo codigo y `docs/hooks.csv`) ni estaba en otro worktree (`git worktree list`: main, wv-2, wv-3). El encargo decia "solo esas 6 funciones" al final pero la lista y el encabezado son de 20: se hicieron las 20. Las clases del listado eran por cercania; los nombres reales salen del desensamblado (capstone), de las vtables leidas del EXE y de los llamadores (`_handoff/dis148.py`, `xref148.py`).

| # | Funcion real (el listado decia) | Entrada | Bytes | Fuente (`source/game_sa/`) | Tipo |
|---|---|---|---|---|---|
| 0 | `FxPrim_c::Constructor` (FxPrimBP_c) | 0x4A9F20 | 21 | `Fx/FxPrim.h/.cpp` | S |
| 1 | `FxPrim_c::Destructor` cuerpo (FxPrimBP_c) | 0x4A9F40 | 15 | idem | S |
| 2 | `FxPrim_c::Enable` (FxPrimBP_c) | 0x4A9F50 | 10 | idem | S |
| 3 | `CAEAudioEntity::Destructor-2`, copia byte a byte de 0x5DE9E0 (FxPrimBP_c) | 0x4A9F60 | 14 | `Audio/Entities/AEAudioEntity.h/.cpp` | S |
| 4 | `FxSphere_c::Constructor` (FxPrimBP_c) | 0x4A9F70 | 10 | `Fx/FxSphere.h/.cpp` | S |
| 5 | `CPedGroupIntelligence::SetDefaultTask` | 0x5F8580 | 29 | `PedGroupIntelligence.cpp` | S |
| 6 | `CPedGroupIntelligence::ReportAllBarDefaultTasksFinished` (nombre propio) | 0x5F8750 | 42 | idem | S |
| 7 | `CTaskComplexCarDriveWander::Destructor` cuerpo | 0x63CB50 | 11 | `Tasks/TaskTypes/TaskComplexDriveWander.cpp` | S |
| 8 | `CTaskComplexCarDriveMission::ScalarDeletingDestructor` (CarDriveWander) | 0x643260 | 30 | `.../TaskComplexCarDriveMission.cpp` | S |
| 9 | `CTaskComplexDriveToPoint::ScalarDeletingDestructor` (CarDriveWander) | 0x643280 | 30 | `.../TaskComplexDriveToPoint.cpp` | S |
| 10 | `CTaskSimpleChat::ScalarDeletingDestructor` (CPedGroups) | 0x5F7FB0 | 30 | `.../TaskSimpleChat.cpp` | S |
| 11 | `CPedGroupMembership::Destructor` cuerpo (copia de `Flush` 0x5FB160) (CPedGroups) | 0x5FB990 | 42 | `PedGroupMembership.cpp` | S |
| 12 | `tUserList::IsPedTypeAllowed` (CPedIK) | 0x5FE960 | 49 | `Scripts/Scripted2dEffects.h/.cpp` | S |
| 13 | `IsPedTypeGang` (CPedIK) | 0x5FE9C0 | 23 | `PedType.cpp` (hook `CPedType`) | S |
| 14 | `CLinkList<CCollisionData*>::Shutdown` (CCollision) | 0x4161D0 | 24 | `Core/LinkList.h`, hook en `Collision/Collision.cpp` | S |
| 15 | `CLinkList<CCollisionData*>::Insert(T const&)` (CCollision) | 0x4161F0 | 64 | idem | S |
| 16 | `CTaskComplexInAirAndLand::GetTaskType` (240, ranura 4) | 0x678CB0 | 6 | `.../TaskComplexInAirAndLand.cpp` | V |
| 17 | `CTaskComplexInAirAndLand::Destructor` cuerpo | 0x678CC0 | 11 | idem | S |
| 18 | `CTaskSimpleInAir::ScalarDeletingDestructor` (InAirAndLand) | 0x67D340 | 30 | `.../TaskSimpleInAir.cpp` | S |
| 19 | `CTaskSimpleLand::ScalarDeletingDestructor` (InAirAndLand) | 0x67D360 | 30 | `.../TaskSimpleLand.cpp` | S |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000`); S = `RH_ScopedInstall` / `RH_ScopedNamedInstall` / `RH_ScopedNamedGlobalInstall` / `RH_ScopedOverloadedInstall`. 20 filas nuevas en `docs/hooks.csv` (sin `sync_docs`/`hooks2md`).

## Funciones saltadas
Ninguna: las 20 son viables en C++ puro. `X87Math.cpp` no se toca. Los cuerpos con `jmp` (3, 7, 17) son `mov [ecx], vtable; jmp <cuerpo>`; no hay saltos puros sin logica propia.

## Revision contra el EXE
- **0 / 1 (0x4A9F20 / 0x4A9F40)**: constructor y cuerpo no virtual de `FxPrim_c` (vtable del EXE 0x85A9E0, todas `_purecall`): el constructor pone +4 y +8 a cero y el BYTE +0xC a 1 y devuelve this; el cuerpo escribe vtable y anula +4 y +8 (EAX sale a 0; contrato void). Los envoltorios escriben la vtable del EXE (la de la DLL no vale).
- **2**: `mov al,[esp+4]; mov [ecx+0Ch],al; ret 4`: solo el byte bajo del argumento.
- **3 (0x4A9F60)**: byte a byte igual a `CAEAudioEntity::Destructor` 0x5DE9E0 (`mov [ecx],859988h; add ecx,8; jmp 0x4EF660`); solo lo alcanza el funclet de desenrollado 0x839928. Se engancha el mismo envoltorio a las dos direcciones (hook `Destructor-2`); el envoltorio ahora llama a `CAESound::Destructor` (noinline) en vez de `~CAESound` en linea.
- **4 (0x4A9F70)**: `FxSphere_c::FxSphere_c`: solo `[this+0x10] = 0`; devuelve this.
- **5 (0x5F8580)**: `SetTask(ped, tarea, this+0x1EC (m_DefaultPedTaskPairs), -1, 0)`, `ret 8`. **6 (0x5F8750)**: tres `ReportAllTasksFinished` (0x5F7730) con las tablas +0xC, +0x14C y +0xAC en ese orden; sin llamadores directos.
- **7, 17 (0x63CB50, 0x678CC0)**: cuerpos con vtable del EXE (0x86E96C / 0x8704AC) y salto a `~CTaskComplexCarDrive` (0x63CA40) / `~CTaskComplex` (0x61A3D0). 0x63CB50 ya existia como funcion propia sin hook (lote 140A): ahora se engancha y escribe la vtable del EXE antes de seguir con 0x63CA40.
- **8, 9, 10, 18, 19**: plantilla de 30 bytes (`call <cuerpo>; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi`): solo cuenta el bit 0 del BYTE bajo de las banderas; devuelve this. Cuerpos: 0x63CCC0, 0x63CE70, 0x5F7FD0 (con hook) y 0x678D50, 0x678EE0 (sin hook; funciones propias `Lote148ABody_*`).
- **11 (0x5FB990)**: para i = 0..7 lee `[this+4+4i]` en cada vuelta y, si no es nulo, `RemoveMember(i)` (0x5F80D0, ECX = this). Bytes identicos a `Flush` (0x5FB160, que no se toca).
- **12 (0x5FE960)**: `tUserList` (0x24 bytes): si el BYTE +0x20 es cero devuelve 1; si no, 1 si alguna de las 4 entradas tiene `[+4i] == -2` y `[+0x10+4i] == tipo` (comparaciones de 32 bits); devuelve AL. `AttractorScanner.cpp` la llama ya directamente.
- **13 (0x5FE9C0)**: cdecl, comparaciones CON SIGNO sobre [7, 16]; EAX completo 0/1.
- **14, 15**: instancias fuera de linea de la plantilla `CLinkList` (`links` en +0x30, listas usada/libre de 0xC bytes): `Shutdown` hace `delete[]` incondicional y DESPUES anula el puntero (la plantilla se corrigio a ese orden); `Insert` toma el primer libre, escribe el dato, lo desengancha y lo pone tras la cabeza usada.
- **16**: `mov eax,0F0h` (`TASK_COMPLEX_IN_AIR_AND_LAND` = 240, `static_assert`).

## Banco
Una funcion por ruta: original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido (plantilla Fn144A con los ayudantes H/U de Fn145A). Tres arenas propias (A, B, C; guardas de 0x40). Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (con hook, apagado durante el banco): `CAESound::Destructor` 0x4EF660, `SetTask` 0x5F7540, `ReportAllTasksFinished` 0x5F7730, `RemoveMember` 0x5F80D0 (pone a cero el hueco y, en una llamada elegida, escribe en otro hueco para probar que se relee cada hueco), 0x63CA40, 0x63CCC0, 0x63CE70, 0x5F7FD0, 0x61A3D0 y `CTask::operator delete` 0x61A5B0; los cuerpos 0x678D50 y 0x678EE0 sin hook (falso en el EXE y en la funcion propia de la DLL); `operator delete[]` 0x8213AE solo en el EXE (la DLL lo llama por esa direccion; el falso lee tambien `[this+0x30]` para probar el orden). Se observa: bytes cambiados de las arenas (en `Insert`, ademas dwords cambiados con los punteros a arena como etiquetas), eventos de los falsos, valor devuelto (this/etiqueta, AL o EAX), x87 vacia y ABI. **EAX, ECX y EDX tras la llamada** quedan en `original_residue` / `candidate_residue` (informativos; `pairs_with_different_eax_ecx_edx_info_only` en `summary.json`: 16384 en las funciones con EAX/ECX distintos por ser void).
16 familias de 1024 casos por funcion. `python -I fixture_selfcheck.py` (sin juego) pasa los 327680 casos por el oraculo y mide la cobertura de ramas.

## Fuera de los fixtures
`this` nulo o crudo, objetos reales construidos por el EXE, listas `CLinkList` con enlaces corruptos o libres inconsistentes, `Enable` con argumentos de pila que no sean dword, y la exactitud de los colaboradores falseados.

## Prueba negativa
Dos rondas de mutaciones (`mutate.py`, 20 por ronda; se compila la sonda con las fuentes mutadas bajo `compile_lock`, se restauran de inmediato y se corre con `FN148A_CASES=2048`; `neg_summary.py` resume). **Las 20 funciones fallan en ambas rondas** (`negative-runs/20261109-215424-329948`: 35394 de 40960 pares con diferencias; `negative-runs/20261110-001409-139789`: 29484 de 40960; en ambas "lado EXE fuera del oraculo 0"). Incidencias: la mutacion de `FxPrim_c::Destructor` de la ronda 1 apuntaba al destructor virtual (no al envoltorio) y no producia diferencias: se corrigio y se repitio la ronda. En la ronda 2, mutar `CLinkList::Insert` (devolver el siguiente) o `FxSphere_c::Constructor`/`IsPedTypeGang`/`GetTaskType` de forma que alcanzara codigo vivo del juego provocaba caidas o cuelgues del propio juego: la mutacion de `Insert` solo actua en la instancia de `CCollisionData*` fuera de la lista real 0x96592C y las demas se hicieron compatibles con el juego. Fuentes restauradas con comparacion SHA256 contra las copias buenas.

## Bancos repetidos
Citan las direcciones del lote (grep en `reversing/`, sin este banco ni evidencia archivada): `Fn140A` (0x63CB50), `Fn92C` (0x5FE960) y `KillPedOnFoot62` (0x5FE9C0). Ademas, como el lote cambia colaboradores (envoltorios a `noinline`, cuerpo de `CAEAudioEntity::Destructor`, plantilla `CLinkList`): `Small2` y `CppModelInfoRegression` (0x5DE9E0), `SimpleTasks52` (0x5F7FD0), `CTasks43` (0x63CCC0), `ColWorld59c` y `Game22` (0x4162E0/0x416260). 9 bancos, todos APROBADOS con el codigo final:
- Sin cambios en el banco: `Small2` (`resultados-small2/20261110-003706-247897`), `CppModelInfoRegression` (`resultados-cppmodelinforegression/20261110-003853-458470`; banco sin `validation-summary.json`, no se archiva), `SimpleTasks52` (`resultados-stasks52/20261110-003937-359981`), `CTasks43` (`resultados-ctasks43/20261110-004110-733056`), `ColWorld59c` (`resultados-colworld59c/20261110-004803-613172`), `Game22` (`resultados-game22/20261110-004918-828914`).
- Adaptados (fallaban por `collaborators_dll`/lista de colaboradores esperada al tener ahora hook): `Fn140A` (0x63CB50 pasa de `U` sin hook a `H` con hook; `resultados-fn140a/20261110-005134-369970`), `Fn92C` (0x5FE960 pasa a colaborador con hook; `resultados-fn92c/20261110-010536-821505`) y `KillPedOnFoot62` (0x5FE9C0 pasa de `kRealExe` a `kRealHooked`; `resultados-killpedonfoot62/20261110-010913-848399`).
Evidencia archivada en `reversing/CppTwelveLevel6Final/runs/<Banco>-<corrida>` y registro refrescado con `refresh_bank.py` (todos menos `CppModelInfoRegression`).

## Tiempo
Una corrida de los 327680 pares: unos 14 min (616 MB crudos, 40,7 MB comprimidos). Compilacion completa del worktree: unos 12 min (`CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1`); sondas incrementales: de segundos a unos minutos.

## Adaptacion del lote 152A (10/11/2026)
El cuerpo 0x678D50 (colaborador del destructor con borrado 0x67D340) paso a tener hook propio en el lote 152A (banco Fn152A). El destructor con borrado llama ahora a `CTaskSimpleInAirDestructorBodyExe` (el cuerpo con hook) y el banco lo falsea como colaborador con hook (`F`) en lugar de la funcion propia de la DLL (`U`). Repetido y aprobado: `reversing/CppTwelveLevel6Final/runs/Fn148A-20261110-035550-418461`.

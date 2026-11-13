<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 13/11/2026 (Ciudad de México):** 900 grupos / 10514 funciones reconstruidas; 10514 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn170A: 22 funciones diminutas de tareas, armas y rutas (lote 170A)

Estado: **APROBADO dentro del juego** (11/11/2026, worktree `wv-4` desde main 19c9abfe5e + los cambios del lote). Evidencia archivada en `reversing/CppTwelveLevel6Final/runs/Fn170A-20261111-210609-978226` (`comparisons.jsonl.gz`, 51 MB). 22 x 16384 = **360448 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `1fb1798db7e79a5695530aaa24e7edfbe8b2e2b8d51f0dca03b6fee485da4aef`; del EXE: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. Plantilla: Fn167A/Fn166A.

Las clases del listado eran en su mayoria incorrectas; los nombres reales salen del desensamblado, las vtables y los llamadores (tabla en `Fixtures.hpp`). Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`); el resto S.

| Funcion real | Entrada | Bytes | Tipo |
|---|---|---|---|
| `CTaskComplexDriveFireTruck::GetTaskType` | 0x659390 | 6 | V |
| `CTaskComplexDriveFireTruck` cuerpo del destructor | 0x6593A0 | 106 | S |
| `CTaskComplexUseSwatRope` constructor sin helicoptero (listado: FireTruck) | 0x659410 | 44 | S |
| `CTaskComplexUseSwatRope::GetTaskType` / `IsInterruptable` / `AbortUrgently` | 0x659440 / 0x659450 / 0x659460 | 6 / 5 / 5 | V |
| `CTaskComplexUseSwatRope` destructor con borrado | 0x65A3C0 | 30 | S |
| `CTaskComplexObserveTrafficLightsAndAchieveHeading::CreateSubTask` | 0x631990 | 218 | S |
| `CTaskSimpleUseGun` constructor / destructor con borrado (listado: FightingControl) | 0x61DE60 / 0x624DA0 | 185 / 30 | S |
| `CTaskSimpleFight::GetTaskType` | 0x61C520 | 6 | V |
| `CWeaponInfo::GetBaseComboByName` (funcion libre; listado: Fight) | 0x61DB30 | 217 | S |
| `CTaskComplexWanderProstitute` destructor con borrado / cuerpo (copia del de CTaskComplexWander) | 0x6726D0 / 0x6726F0 | 30 / 11 | S |
| `CTaskComplexPlayHandSignalAnim` destructor con borrado | 0x61C0D0 | 30 | S |
| `CWeaponInfo::GetAnimLoopFire` | 0x61C150 | 20 | S |
| `CPointRoute::Reverse` / `CNodeRoute::Reverse` | 0x667720 / 0x667900 | 116 / 43 | S |
| `CPatrolRoute` cuerpo del destructor (vector de 8 destructores triviales, 0x46A380 = `ret`) | 0x6677A0 | 19 | S |
| `CEntitySeekPosCalculator` cuerpo del destructor, 3.a copia | 0x667980 | 7 | S |
| `CTaskSimpleGangDriveBy` cuerpo del destructor (ya llamado por el banco Fn145A como colaborador sin hook) | 0x6218C0 | 148 | S |
| `CTaskComplexKillAllThreats::Clone` (listado: GangDriveBy) | 0x623750 | 106 | S |

## Saltada
- **0x674930** (`CTaskComplexFollowPatrolRoute` constructor, 182 B): llama a dos colaboradores sin reconstruir (0x66D440, constructor de `CPatrolRoute` con `__ehvec_ctor` de 8 elementos de 0x28, y 0x66D4B0, copia de ruta con bucles de cadenas) y la clase es una plantilla sin campos ni `InjectHooks` activo; no es un lote diminuto.

## Notas
- Los constructores/clones/cuerpos escriben la vtable del EXE (la del compilador dejaria la de la DLL). `CTaskComplexKillAllThreats::InjectHooks` se activa (solo `RH_ScopedClass` + enganche S de Clone). `GetBaseComboByName` pasa a `uint32` con `strcmp` (EAX completo; sin leer mas alla de la primera diferencia). `CPointRoute::Reverse`/`CNodeRoute::Reverse` ocultan la plantilla `Route::Reverse` con el contador con signo del EXE.
- El comentario de 155A que citaba 0x624D80 como destructor con borrado de UseGun era erroneo (es 0x624DA0).
- Colaboradores falseados con falso identico en el EXE y en la DLL (todos con hook): 0x61A390, 0x61A3B0, 0x61A3D0, 0x61A3A0, 0x61A5A0, 0x61A5B0, 0x571A00, 0x571B70, 0x4CEBC0, 0x4D3FD0, 0x667E20, 0x6594D0, 0x61DF30, 0x6726F0 (funcion probada del lote) y 0x61BDF0. Memoria global: el puntero 0xB4EA34 (tabla de grupos de animacion), guardado y restaurado.
- `python -I fixture_selfcheck.py` (sin juego) pasa los 360448 casos por el oraculo; todas las ramas (KEYS) superan el minimo.
## Prueba negativa
Dos rondas de mutaciones (`mutate.py`, 22 por ronda, una por funcion; se aplican a las fuentes, se recompila la sonda y se corre con `FN170A_CASES=2048` por funcion = 45056 pares; las mutaciones se condicionan a `this` o puntero desalineado, que el juego no produce). **Las 22 funciones fallan en ambas rondas** (`negative-runs/`: `ronda1-20261111-221730-896448` con 15731 de 45056 pares con diferencias y `ronda2-20261111-232822-711246` con 12424; el lado EXE coincide con el oraculo en todos los pares: "lado EXE fuera del oraculo 0"). Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas, "restauradas 15 fuentes", tras cada ronda). Incidencias: un primer intento de la ronda 2 se aborto por "GTA sin progreso" (cuelgue del equipo) y se repitio; el segundo intento dejo sin mutar la destructor con borrado de `CTaskSimpleUseGun` (mutacion 9 mal dirigida al constructor), se corrigio en `mutate.py` y se repitio la ronda 2 completa.

## Bancos que citan estas direcciones (no repetidos; para el pase final)
`Fn145A` y `Fn147A` (0x6218C0, ahora con hook: el banco 145A lo daba por sin hook), `FootWave4` (0x61DE60, constructor ahora con hook); `Fn111A` y `Fn113A` (solo README: 0x631990, 0x667900).

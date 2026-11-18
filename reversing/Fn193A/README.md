<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11695 con evidencia histórica y 746 pendientes de validación en 32 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn193A: 34 funciones de tareas y de Fx_c del EXE (lote 193A)

Estado: **APROBADO dentro del juego** (15/11/2026, worktree `wv-5` desde main 94bf1068ae + el commit del lote 193A sin validar 07eff439ab; plantilla `Fn190A`, con `Fn171A`, `Fn156A` y `Fn149A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn193a/20261115-210619-734160` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn193A-20261115-210619-734160`, `comparisons.jsonl.xz`). 34 x 16384 = **557056 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`, que modela la x87 de 24 bits con fracciones exactas). SHA256 de la sonda ejecutada: `446c30fc4ef7a0453bcbb65627d8467a6a4e9222e8947df6a174add7270b2d1f`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Ninguna direccion estaba saltada. Las 34 filas de `docs/hooks.csv` pasan a validated=1.

| # | Funcion real | Entrada | Tipo |
|---|---|---|---|
| 0 | `CTaskSimpleBeHitWhileMoving` destructor con borrado | 0x625BA0 | S |
| 1 | `CTaskSimpleBeHitWhileMoving` cuerpo del destructor | 0x61FE70 | S |
| 2 | `CTaskSimpleChoking` destructor con borrado | 0x625BC0 | S |
| 3 | `CTaskSimpleBeHit` destructor con borrado | 0x625BE0 | S |
| 4 | `CTaskComplexReactToAttack` destructor con borrado | 0x625C00 | S |
| 5 | `CTaskComplexReactToAttack` cuerpo del destructor | 0x620B90 | S |
| 6 | `CTaskComplexReactToAttack::CreateNextSubTask` | 0x625C20 | S |
| 7 | `CTaskComplexReactToAttack::CreateFirstSubTask` | 0x625C70 | S |
| 8-10 | destructores con borrado de `CTaskComplexKillPedOnFoot`, `...Stealth` y `...Melee` | 0x625E20, 0x626A80, 0x626AA0 | S |
| 11 | `CTaskComplexUseWaterCannon::GetTaskType` | 0x6597A0 | V |
| 12 | `CTaskComplexUseWaterCannon` cuerpo del destructor | 0x6597B0 | S |
| 13 | `CTaskComplexExtinguishFireOnFoot` constructor | 0x659870 | S |
| 14 | `CTaskComplexExtinguishFireOnFoot::GetTaskType` | 0x6598A0 | V |
| 15 | cuerpo del destructor | 0x6598B0 | S |
| 16 | `CreateNextSubTask` | 0x6598C0 | V |
| 17 | `ControlSubTask` | 0x659980 | V |
| 18 | `FindNearestFire` | 0x659990 | S |
| 19 | `Clone` | 0x659D70 | V |
| 20 | `CTaskSimpleAbseil` destructor con borrado | 0x659DE0 | S |
| 21 | `CTaskSimpleAbseil` cuerpo del destructor | 0x659E00 | S |
| 22 | `CTaskSimpleGiveCPR` destructor con borrado | 0x659E10 | S |
| 23 | `CTaskComplexTreatAccident` destructor con borrado | 0x659E70 | S |
| 24 | `CTaskComplexExtinguishFireOnFoot` destructor con borrado | 0x65A700 | S |
| 25 | `CreateFirstSubTask` | 0x65A720 | V |
| 26-30 | `Fx_c` constructor, `Update`, `Render`, `InitStaticSystems`, `ExitStaticSystems` | 0x49E620, 0x49E640, 0x49E650, 0x49E660, 0x49E850 | S |
| 31-33 | `CTaskComplexWaitForPed` `GetTaskType`, cuerpo del destructor y `MakeAbortable` | 0x6833C0, 0x6833D0, 0x683430 | S |

## Correcciones de fuente hechas al validar
- **x87 (denormales)**: una primera corrida corta encontro 1 caso de 256 en `CTaskComplexReactToAttack::CreateFirstSubTask`: el producto `x * -5.0f` del vector normalizado daba 0x80000000 en la DLL (SSE con DAZ/FTZ del juego aplana el denormal) y 0x80000005 en el EXE (x87). `source/CMakeLists.txt`: los archivos `TaskComplexReactToAttack.cpp` y `TaskComplexExtinguishFireOnFoot.cpp` (suma de cuadrados de `FindNearestFire`) se compilan con `/O2 /Ob0 /Oi /arch:IA32 /fp:fast` sin PCH (con el `#pragma warning` alrededor de `StdInc.h`). **Las cuatro funciones de 199A del mismo archivo `TaskComplexReactToAttack.cpp` (MakeAbortable, CreateSubTask, ControlSubTask, Clone; banco `Fn199A`) se recompilan con estas opciones: el coordinador debe repetir `Fn199A` en el pase final.**
- `CTaskComplexExtinguishFireOnFoot::CreateNextSubTask` y `CreateFirstSubTask` llaman a los `Constructor` enganchados de `CTaskSimpleGunControl` (0x61F3F0, con punteros como el EXE: objetivo `&m_pos`, destino nulo) y de `CTaskComplexGoToPointAndStandStill` (0x668120) y devuelven su EAX (`friend class CTaskComplexExtinguishFireOnFoot` en ambas cabeceras).
- Destructores con borrado de `CTaskSimpleChoking`, `CTaskSimpleBeHit`, `CTaskComplexKillPedOnFootStealth` y `...Melee`: llaman al `Destructor` enganchado (0x620370, 0x620810, 0x620FF0, 0x621090) en lugar del destructor de C++, para que el falso del banco lo intercepte (funcion amiga en las tres cabeceras con `Destructor` privado). Sin cambio de comportamiento.
- Conflictos del cherry-pick: `TaskComplexReactToAttack.cpp` (199A ya habia enganchado `CreateSubTask` 0x620C30; el lote usa esa funcion), `TaskSimpleAbseil.cpp`, `InjectHooksMain.cpp` y `docs/hooks.csv`.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40. Falsos identicos en la direccion del EXE y en el cuerpo de la DLL de los colaboradores con hook (ver `LiveComparison.hpp`); `CVector2D::Normalise` 0x44E480 (sin hook) solo en la direccion del EXE. Reales en ambos lados: la base `~CTaskComplex` en el cuerpo del destructor de `CTaskComplexUseWaterCannon` (subtarea sintetica con tabla virtual de falsos), el constructor base y el `Clone`/constructor de `CTaskComplexExtinguishFireOnFoot` (la vtable se clasifica) y la lista de `Fx_c::Constructor`. `CreateNextSubTask` usa una tabla virtual sintetica en `this` (ranura 9). Los falsos de `FindNearestFire` del gestor, `IsPlayer`, `GetPad`, `GetTarget` y `Normalise` modifican memoria (posicion del ped, banderas +0x46C) para comprobar el orden de lecturas y escrituras. 16 familias de 1024 casos por funcion; `python -I fixture_selfcheck.py` pasa los 557056 casos por el oraculo. Comando: bancos propios `_run193.sh` con una sola sesion del candado de juego (la corrida principal y las rondas negativas).

## Prueba negativa
`mutate.py` (34 mutaciones por ronda, condicionadas a `this` desalineado, o desalineado en 2 bytes en la ronda 2). **Las 34 fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261115-214703-272454` con 22264 de 69632 pares con diferencias y ronda 2 `20261115-220742-700023` con 7350; el lado EXE coincide con el oraculo en todos los pares. Incidencia: una primera ronda 2 dejo `FindNearestFire` sin diferencias porque la mutacion usaba el mismo umbral (100.0f); se corrigio (99.0f) y se repitio la ronda completa. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas) y sonda buena reconstruida.

## Fuera de los fixtures
`this` nulo o crudo, pools reales de tareas, animaciones, peds, pads y fuegos reales, objetos RenderWare, `FxManager_c` real, y la exactitud de los colaboradores falseados. Dos NaN distintos en una misma resta (x87 y SSE eligen distinto) no se generan.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x626A80 -> `KillPedOnFoot62` (README); 0x6597B0 -> `Fn153A` (su colaborador ahora tiene hook: **repetir**); 0x6598A0 -> `HooksActivados` (README); 0x659E10 -> `Fn150A` (**repetir**, colaborador); 0x49E640 -> `Game22`; 0x49E650 -> `Sys12` y `PostEffectsPending`; 0x49E660 y 0x49E850 -> `Fn184A`; 0x6833D0 -> `Fn151A` (**repetir**). Ademas, `Fn199A` por el cambio de opciones de compilacion de `TaskComplexReactToAttack.cpp`, y los bancos de `CTaskSimpleGunControl` y `CTaskComplexGoToPointAndStandStill` solo cambian de cabecera (amistad).

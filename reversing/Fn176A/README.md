<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 898 grupos / 10446 funciones reconstruidas; 10446 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn176A: 26 funciones del lote 176A (validacion del lote reconstruido sin validar)

Estado: **APROBADO dentro del juego** (12/11/2026, worktree `wv-2` desde main 7406ace9cf + cherry-pick de ab79ed2626 + correcciones de fuente impuestas por el banco; plantillas `Fn170A`, `Fn171A` y `Fn173A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn176a/20261112-172759-479826` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn176A-20261112-172759-479826`, `comparisons.jsonl.gz`, 57 MB). 26 x 16384 = **425984 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`, que ademas reimplementa la aritmetica x87 de 24 bits con fracciones exactas). SHA256 de la sonda ejecutada: `b7b5ef27facfa00769f354a0dd293d2ec5c6ff4976fd9266aa88a69bd5872e7e`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

| # | Funcion real | Entrada | Tipo | Colaboradores falseados (mismo falso en el EXE y en la DLL) |
|---|---|---|---|---|
| 0 | `CPedDamageResponseCalculator::IsBleedingWeapon` | 0x4AD800 | S | `CPed::IsPlayer` 5DF8F0 |
| 1 | `CEventDamage` destructor con borrado | 0x4B5CF0 | S | cuerpo 4AD960; el borrado del pool (0xB744AC) corre real sobre un pool sintetico |
| 2 | `CAEWaterCannonAudioEntity::UpdateGenericWaterCannonSound` | 0x502F80 | S | `CAESound::Initialise` 4EFE50, `SetPosition` 4EF880, `StopSound` 4EF1C0, `RequestNewSound` 4EFB10 |
| 3 | `FurnitureManager_c::Constructor` | 0x591260 | S | ninguno (el EXE corre los iteradores de vectores del CRT) |
| 4 | `FurnitureManager_c::Destructor` | 0x5912E0 | S | ninguno |
| 5 | `Interior_c::Destructor` | 0x591360 | S | ninguno |
| 6 | `CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::ComputeTarget` | 0x646F40 | S | `GetPositionToOpenCarDoor` 64E740 |
| 7 | `CTaskSimpleCarSetPedInAsPassenger::Constructor` | 0x646FE0 | S | base 61A390, `RegisterReference` 571B70 |
| 8 | `CTaskSimpleCarSetPedInAsPassenger` destructor con borrado | 0x64B5B0 | S | cuerpo 647080, `CTask::operator delete` 61A5B0 |
| 9 | `CTaskComplexHitPedWithCar::HitKillsPed` | 0x653AA0 | S | `IsPlayer` 5DF8F0 |
| 10 | `CTaskSimpleKillPedWithCar` destructor con borrado | 0x6563B0 | S | cuerpo 653C90, delete 61A5B0 |
| 11 | `CTaskSimpleKillPedWithCar::ShakePadIfPlayerVehicle` | 0x6563D0 | S | `FindPlayerVehicle` 56E0D0, `CPad::GetPad` 53FB70, `StartShake` 53F920 |
| 12 | `CTaskComplexWalkRoundCar` destructor con borrado | 0x657B60 | S | cuerpo 656B00, delete 61A5B0 |
| 13 | `CTaskInteriorSitInChair::GetTaskType` | 0x675C80 | S | ninguno |
| 14 | `CTaskInteriorSitInChair` cuerpo del destructor | 0x675C90 | S | `SetDeleteCallback` 4CEBC0, `~CTask` 61A3A0 |
| 15 | `CTaskInteriorSitInChair::Clone` | 0x675CF0 | S | `CTask::operator new` 61A5A0, constructor 675C30 |
| 16 | `CTaskInteriorSitInChair::MakeAbortable` | 0x675D60 | S | `IsLooking` 6181A0, `AbortLookAt` 618280, 4CEBC0 |
| 17 | `CTaskSimpleHoldEntity::GetTaskType` | 0x691460 | V | ninguno |
| 18 | `CTaskSimpleHoldEntity` cuerpo del destructor | 0x691630 | S | `CleanUpOldReference` 571A00, `SetFinishCallback` 4CEBE0, `RemoveAnimBlockRef` 4D3FD0, `~CTask` 61A3A0 |
| 19 | `CTaskSimplePickUpEntity` destructor con borrado | 0x693590 | S | cuerpo 691630 (funcion 18, via `jmp` de 0x6935B0), delete 61A5B0 |
| 20 | `CTaskSimplePutDownEntity` destructor con borrado | 0x6935C0 | S | cuerpo 691630 (via `jmp` de 0x6935E0), delete 61A5B0 |
| 21 | `CTaskComplexGoPickUpEntity` destructor con borrado | 0x6935F0 | S | cuerpo 691A50, delete 61A5B0 |
| 22 | `CTaskSimpleDuck` destructor con borrado | 0x6939D0 | S | cuerpo 692030, delete 61A5B0 |
| 23 | `CAEWaterCannonAudioEntity` cuerpo del destructor | 0x728AB0 | S | `Terminate` 5030A0, destructor de `CAESound` 4EF660 |
| 24 | `Input::HandlePadButtonDown` (EAX) | 0x7448B0 | S | `GetPad` 53FB70, `UpdateJoyButtonState` 52F510, 52DAB0, 530ED0 |
| 25 | `Input::HandlePadButtonUp` (EAX) | 0x744930 | S | 53FB70, 52F510, 52DC20, 531070 |

Las dos funciones con argumento en EAX (24 y 25) se llaman con EAX sembrado con el puntero al estado de pad (`Gate` recibe `seedEax`); salen con EAX = 1 y conservan EBX/ESI/EDI.

## Funciones corregidas por el banco
Una corrida parcial (255353 pares) y la completa siguiente mostraron 5 funciones con diferencias: se corrigio la fuente en 3 y se acoto el banco en 2 casos que no son defectos de la funcion. Ademas se corrigieron tres funciones por lectura del codigo antes de la primera corrida.
- **0x646FE0 constructor de `CTaskSimpleCarSetPedInAsPassenger`**: el constructor de la DLL dejaba la vtable de la DLL, no llamaba al cuerpo base 0x61A390 y no seguia la secuencia del EXE. Ahora es `SetPedInAsPassengerConstructorBody` (vtable 0x86EE04, base `CTaskSimpleBody::Constructor`, campos y `RegisterReference` en el orden del EXE).
- **0x691630 cuerpo del destructor de `CTaskSimpleHoldEntity`**: llamaba a `~CTaskSimpleHoldEntity()`, que deja las vtables de la DLL y no pasa por `CTask::Destructor`. Ahora la secuencia exacta del EXE (vtable 0x870B2C, bandera 0x400 fuera, `CleanUpOldReference`, 0x800, `SetFinishCallback`, `RemoveAnimBlockRef` con el desplazamiento aritmetico `(puntero - 0xB5D4A0) >> 5` en lugar de la resta de punteros con division que redondea hacia cero, `CTask::Destructor`). Pasa a ser funcion global declarada en `TaskSimpleHoldEntity.h` (`HoldEntityDestructorBody`); los destructores con borrado de `PickUp` (0x693590) y `PutDown` (0x6935C0) la llaman como el `jmp 0x691630` del EXE (antes destruian con el destructor generado, con otras vtables y sin pasar por el cuerpo).
- **0x675CF0 `CTaskInteriorSitInChair::Clone`**: devolvia la direccion reservada; el EXE devuelve el EAX del constructor. Ahora devuelve lo que deja el constructor.
- **0x646F40 `ComputeTarget` y 0x6563D0 `ShakePadIfPlayerVehicle`**: los archivos de ambas clases se compilaban con SSE y sin optimizar (cada `float` guardado). Opciones por archivo `/O2 /Ob0 /Oi /arch:IA32 /fp:fast` en `source/CMakeLists.txt` (con `StdInc.h` entre `#pragma warning(push, 0)` y `pop`, sin PCH), `float_control(precise, on)` en ambas funciones, y en `ComputeTarget` `dz` y `2*dx` guardados con `volatile float` (como los guarda el EXE) mientras `2*dy` queda en el registro (unos 3000 casos del banco solo dan el resultado correcto con esa secuencia: el doble desborda el float y la suma final vuelve al rango). En `ShakePadIfPlayerVehicle`, `std::sqrt(float)` llamaba a `sqrtf` del CRT (SSE, con los denormales aplanados por el MXCSR del juego: una velocidad denormal daba frecuencia 80 en lugar de 250); ahora `std::sqrt` de un `double` (fsqrt en linea).
- Acotados del banco (no son defectos en el uso real): NaN senalizantes en `ComputeTarget` (un NaN cargado con `fld` se silencia y uno en memoria no, y el resultado depende de que operando va en memoria); identificador de sonido y de banco de `UpdateGenericWaterCannonSound` fuera del rango de 16 bits (el EXE pasa el dword de la pila y la DLL un entero de 16 bits; los llamadores reales empujan constantes 0 y 3 y bancos pequenos).

Ninguna funcion se ha excluido.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40 (A mide 0x2200 para `FurnitureManager_c`); falsos identicos en la direccion del EXE y en el cuerpo de la DLL de los colaboradores con hook. Memoria global tocada y restaurada: puntero al pool de eventos 0xB744AC, `padNumber` 0xB73400, menu 0xBA67A4, botones 0xB70CF4, camara activa 0xB6F081 y modos 0xB6F1A8. 16 familias de 1024 casos por funcion; `python -I fixture_selfcheck.py` pasa los 425984 casos por el oraculo. `FurnitureManager_c::Constructor` registra los cambios de la arena A de forma compacta (numero de bytes y FNV-1a). Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn176A` (la ejecucion completa tarda ~1 h 30 min: `run_local.py` y `bench_here.sh` llevan timeout de 9000/9500 s).

## Prueba negativa
`mutate.py` (26 mutaciones por ronda, condicionadas a `this` desalineado o al bit 31 del identificador de pad). **Las 26 fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261112-180400-874222` (16930 de 53248 pares con diferencias) y ronda 2 `20261112-184143-915264` (16764 de 53248); el lado EXE coincide con el oraculo en todos los pares. Incidencia: la primera ronda 2 no compilo (la mutacion de `GetTaskType` usaba `self` sin nombre); se corrigio la mutacion y se repitio. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas).

## Fuera de los fixtures
`this` nulo o crudo, NaN senalizantes, ids de sonido o de banco de mas de 16 bits, destructores de elemento no triviales en `FurnitureManager_c`/`Interior_c`, pool de eventos fuera de la arena y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Del listado de `reversing/PendienteValidar/176A.md`: 0x4B5C2A -> `PedDamagePending`, `HooksActivados`, `Pending355`; 0x4B5CF0 -> `Event20`; 0x591360 -> `InteriorAbi`, `InteriorEvidence`; 0x646F40 -> `Fn108A`; 0x646FE0 -> `CarEnterWave4`; 0x653AA0, 0x6563B0, 0x6563D0 -> `Tasks355Evidence` (y `RequestedPadMutation*` para StartShake); 0x675C80 -> `DependenciesTaskCtor4`, `HooksActivados`; 0x675C90 -> `OriginalDependencies499`; 0x691630 -> `Fn151A` (**repetir**: el cuerpo ahora esta enganchado y cambio); 0x6935F0 -> `Helpers65B`, `PickUpControl68`. **Repetir ademas** los bancos de las clases cuyos archivos pasan a compilarse con `/O2 /arch:IA32 /fp:fast` (`TaskSimpleKillPedWithCar.cpp`, `TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp`: p. ej. `Tasks355Evidence`, `STasks7`, `Fn108A`) y los que usan el constructor de `CTaskSimpleCarSetPedInAsPassenger` y los destructores con borrado de `PickUp`/`PutDown` (`CarEnterWave4`, `PickUpControl68`, `Helpers65B`).

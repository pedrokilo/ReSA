<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas simples de coche (lista de 9)

Validadas el 02/10/2026 con **16.448 pares original/candidato dentro de GTA**, todos exactos: 45 funciones de 5 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskSimpleCarFallOut` | Constructor, Destructor, `Clone`, `FinishAnimFallOutCB`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `SetPedPosition`, `StartAnim` |
| `CTaskSimpleCarGetIn` | Constructor, Destructor, `Clone`, `FinishAnimCarGetInCB`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `SetPedPosition`, `StartAnim` |
| `CTaskSimpleCarOpenLockedDoorFromOutside` | Constructor, Destructor, `Clone`, `FinishAnimCarOpenLockedDoorFromOutsideCB`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `SetPedPosition`, `StartAnim` |
| `CTaskSimpleCarShuffle` | Constructor, Destructor, `Clone`, `FinishAnimCarShuffleCB`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `SetPedPosition`, `StartAnim` |
| `CTaskSimpleCarSlowBeDraggedOut` | Constructor, Destructor, `Clone`, `FinishAnimCarSlowBeDraggedOutCB`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `SetPedPosition`, `StartAnim` |

**Diferencias reales corregidas:**
- `CTaskSimpleCarFallOut`:
  - el constructor registra la referencia al vehículo y el destructor la limpia (la DLL guardaba un puntero sin registrar);
  - `Clone` llama al constructor (el constructor de copia implícito copiaba también la animación y el estado de fin);
  - `FinishAnimFallOutCB` **abre** la puerta (cerrada → abierta, dañada → abierta y dañada; la DLL la cerraba);
  - `ProcessPed` pasa a `ProcessOpenDoor` el tiempo actual de la animación, no 1,0.
- `CTaskSimpleCarGetIn`:
  - el constructor no escribe los dos campos sin uso (+0x10/+0x14);
  - el destructor quita el callback de la animación;
  - `FinishAnimCarGetInCB` corta la mezcla (`BlendDelta = −1000`) de la animación de la tarea;
  - `ProcessPed` (BMX): umbral `10 · (1/30)` multiplicando por la constante del EXE (0x858F10) y fuerza `forward · (paso · masa · 0,01)`.
- `CTaskSimpleCarOpenLockedDoorFromOutside`: `StartAnim` usa la animación LHS también en la puerta trasera izquierda; `ProcessPed` termina sin vehículo.
- `CTaskSimpleCarShuffle`:
  - `Clone` leía los campos del objeto nuevo (sin inicializar) en vez de los del original;
  - el destructor limpia la referencia antes de tocar la animación;
  - `FinishAnimCarShuffleCB` comprueba que haya animación;
  - `SetPedPosition` alinea solo si la mezcla es estrictamente mayor que 0,9.
- `CTaskSimpleCarSlowBeDraggedOut`:
  - `Clone` llama al constructor con `isDraggedBySelf = false` (la copia implícita copiaba todo);
  - el destructor limpia la referencia primero y no anula `m_Anim`;
  - `ProcessPed` pasa el tiempo actual de la animación a `ProcessOpenDoor`;
  - `SetPedPosition` guarda la asociación encontrada en `m_Anim` durante la alineación y luego la anula.

**Método:**
- Constructores en la dirección del EXE sobre un buffer con 0xCD (se compara el tipo en vez del vptr); los métodos virtuales por la entrada de la vtable del EXE; los callbacks como `__cdecl(anim, tarea)`, con los nombres de callback resueltos para el EXE y la DLL.
- Peatón falso (clump con tipo 2, `bInVehicle` y tipo de peatón controlados) y vehículo falso del tamaño de `CAutomobile` con vtable propia (`ProcessOpenDoor`, `IsDoorClosedU32`, `IsDoorMissingU32`), matriz, masa, tipo/subtipo (BMX), banderas y puertas del `CDamageManager`.
- Asociaciones con mezcla 0,9 y sus vecinos exactos, y tiempos en los umbrales 1/3 y 1,7 con sus vecinos.
- Se simulan y trazan animaciones, `RemoveCarSitAnim`, `CVehicleAnimGroup::GetGroup`, `CTaskUtilityLineUpPedWithCar::ProcessPed`, `SetPedPositionInCar`, `RemoveWeaponWhenEnteringVehicle`, `ApplyMoveForce` y el estado de puertas del `CDamageManager` (con la semántica del EXE: el hook de la DLL devuelve un `uint8` y el EXE compara `eax` entero).
- Puertas fuera de 8–11/18 excluidas en `CTaskSimpleCarGetIn::StartAnim` y en `CTaskSimpleCarSlowBeDraggedOut` (el EXE usa un valor sin inicializar); el destructor de `CTaskSimpleCarFallOut` se prueba sin animación (la DLL conserva un `FIX_BUGS` que quita el callback).
- Se guardan y restauran tras cada ejecución el paso del reloj, el truco `CHEAT_SMASH_N_BOOM`, el pool de tareas y la lista libre de referencias.

Repetir: `python reversing/STasks9/build_probe.py` y `python reversing/run.py --case stasks9`. Evidencia: [validation-summary.json](validation-summary.json).

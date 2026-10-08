<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas de cobertura, cambio de asiento y coche (lista de 10)

Validadas el 02/10/2026 con **16.256 pares original/candidato dentro de GTA**, todos exactos: 40 funciones de 4 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexSeekCoverUntilTargetDead` | `Constructor_Grp`, `Constructor_Ped`, Destructor, `Clone`, `GetTaskType`, `GetCoverPed`, `GetTargetPed`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexShuffleSeats` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `CreateSubTask`, `CreateTaskUtilityLineUpPedWithCar`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskSimpleCarAlign` | Constructor, Destructor, `Clone`, `GetTaskType`, `FinishAnimCarAlignCB`, `FixHeading`, `MakeAbortable`, `ProcessPed`, `SetPedPosition`, `StartAnim` |
| `CTaskSimpleCarCloseDoorFromInside` | Constructor, Destructor, `Clone`, `GetTaskType`, `ComputeAnimID`, `FinishAnimCarCloseDoorFromInsideCB`, `MakeAbortable`, `ProcessPed`, `SetPedPosition`, `StartAnim` |

**Diferencias reales corregidas:**
- `CTaskComplexSeekCoverUntilTargetDead`:
  - `CreateNextSubTask` estaba invertido: tras `STAND_STILL` termina y con cualquier otra subtarea vuelve a `CreateFirstSubTask`;
  - `CreateFirstSubTask` limpia objetivo y cubierto, calcula los dos y solo registra las referencias (objetivo y luego cubierto) si existen ambos;
  - la secuencia se crea antes que sus tareas y estas en orden (la lista de argumentos las creaba al revés), con el ángulo π exacto (0x40490FDB);
  - `GetTargetPed` usa grupo con signo (`< 0`, no solo −1) y recorre solo los 7 seguidores; `GetCoverPed` usa `GetPedsGroup`, 8 miembros y distancia `(z·z + x·x) + y·y` estricta;
  - `Clone` usa el constructor de grupo con grupo ≥ 0 y el de peatón (grupo −1) en otro caso; `ControlSubTask` limpia la referencia del objetivo sin anular el puntero antes de buscar otro.
- `CTaskComplexShuffleSeats`:
  - el constructor ya no pone a cero puertas y vector, y el destructor limpia la referencia al vehículo antes de borrar la utilidad;
  - `CreateSubTask`: `SET_PED_IN_AS_DRIVER` saca antes al peatón de los pasajeros, `SET_PED_OUT` usa el vehículo del peatón, `TASK_FINISHED` comprueba el peatón y los tipos desconocidos devuelven `nullptr`;
  - `CreateNextSubTask`: `SET_PED_OUT` sin abortar termina, una puerta de origen desconocida conserva la de destino y los tipos desconocidos devuelven `nullptr`;
  - `CreateTaskUtilityLineUpPedWithCar` compara directamente conductor y pasajeros 0 y 1 (también con peatón nulo); `ControlSubTask` consulta el tipo de la subtarea y devuelve la leída al entrar.
- `CTaskSimpleCarAlign`:
  - `FixHeading`: producto `(z·z' + y·y') + x·x'`, sigue solo si `0 < dot < 0,1` (NaN termina) y resta **dos veces** `x·dot` e `y·dot` del forward;
  - `StartAnim`: la animación alta solo con altura `> 4,4` (NaN usa la baja); `SetPedPosition` vuelve a calcular la posición de la puerta (sin usarla).
- `CTaskSimpleCarCloseDoorFromInside`:
  - el destructor restablece el callback de **fin** (no el de borrado);
  - `MakeAbortable` devuelve siempre falso y pasa el peatón a `ProcessOpenDoor` (también `ProcessPed` con la puerta que no se cierra);
  - `ComputeAnimID` con otra puerta no escribe la animación.

**Método:**
- Constructores en la dirección del EXE sobre un buffer con 0xCD (se compara el tipo en vez del vptr); métodos virtuales por la entrada de la vtable del EXE; callbacks como `__cdecl(anim, tarea)` con los nombres de callback resueltos para el EXE y la DLL.
- 8 peatones (matriz, armas con `IsTypeMelee` real, estados vivo/muriendo/muerto, posiciones empatadas y con NaN), 2 vehículos del tamaño de `CAutomobile` con vtable falsa (`ProcessOpenDoor`), conductor y pasajeros, grupo falso para `GetPedsGroup` y miembros en `CPedGroups::ms_groups`.
- `GetPedsGroup`, `RemoveDriver`/`RemovePassenger`, `CCarEnterExit` (puerta de salida, índice de pasajero, posición de la puerta, salud de vehículo y peatón), `BlendAnimation`, `SetFinishCallback`, `CVehicleAnimGroup::GetGroup`, `CTaskUtilityLineUpPedWithCar::ProcessPed` y el RNG simulados con traza sin reservas de memoria.
- `operator new`/`delete` del EXE sustituidos durante las llamadas de `CTaskComplexShuffleSeats`: la utilidad nueva sale de una arena propia y el borrado de la del destructor queda registrado.
- Umbrales exactos: altura 4,4 y sus vecinos, producto 0 y 0,1 y sus vecinos; banderas especiales de los grupos de animación de vehículo, `CTimer`, `ms_groups` y el pool de tareas se restauran en cada ejecución.
- Fuera de los fixtures: `CTaskComplexSeekCoverUntilTargetDead::ControlSubTask` con el peatón cubierto nulo o muerto (FIX_BUGS en la DLL busca otro con `GetCoverPed`; el EXE usa `GetTargetPed`), y puertas fuera de 8–11 donde `CTaskSimpleCarCloseDoorFromInside` pasa a `ProcessOpenDoor`/`BlendAnimation` una animación sin inicializar.

Repetir: `python reversing/CTasks10b/build_probe.py` y `python reversing/run.py --case ctasks10b`. Evidencia: [validation-summary.json](validation-summary.json).

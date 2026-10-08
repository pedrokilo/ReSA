<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6822 con evidencia histórica, 0 con vigencia acreditada y 1624 pendientes en 64 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas simples A (lista de 8)

Validadas el 02/10/2026 con **12.224 pares original/candidato dentro de GTA**, todos exactos: 40 funciones de 5 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskSimpleEvasiveDive` | Constructor, Destructor, `Clone`, `FinishAnimEvasiveDiveCB`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `StartAnim` |
| `CTaskSimpleEvasiveStep` | Constructor, Destructor, `Clone`, `FinishAnimEvasiveStepCB`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `StartAnim` |
| `CTaskSimpleGoToPointFine` | Constructor, `BaseRatio`, `Clone`, `Finish`, `MakeAbortable`, `ProcessPed`, `SetBlendedMoveAnim`, `SetTargetPos` |
| `CTaskSimpleIKLookAt` | Constructor, Destructor, `Clone`, `CreateIKChain`, `GetLookAtEntity`, `GetLookAtOffset`, `GetTaskType`, `UpdateLookAtInfo` |
| `CTaskSimpleIKManager` | Constructor, Destructor, `AddIKChainTask`, `Clone`, `GetTaskAtSlot`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |

**Diferencias reales corregidas:**
- `CTaskSimpleEvasiveDive`:
  - el destructor limpia la referencia al vehículo antes de quitar el callback de la animación (el vehículo pasa a puntero con referencia manual);
  - `StartAnim` llama a `FindPlayerWanted` antes de cada uno de los dos delitos;
  - `FinishAnimEvasiveDiveCB` no comprueba el tipo de la tarea.
- `CTaskSimpleEvasiveStep::ProcessPed`: grita si la entidad es un vehículo (byte 0x36 & 7 == 2), no si su estado es `STATUS_SIMPLE`.
- `CTaskSimpleGoToPointFine::SetBlendedMoveAnim`: las ramas son `<= 1`, `<= 2` y `<= 3`, así que con NaN sale sin tocar animaciones ni estado (antes andaba).
- `CTaskSimpleIKLookAt`:
  - el constructor no escribe 0x14, 0x16, 0x48, 0x4C ni 0x50 (constructor de `CTaskSimpleIKChain` en línea);
  - el hueso de desplazamiento se guarda y se pasa como dword en el constructor, `Clone`, `UpdateLookAtInfo` y `CreateIKChain`.
- `CTaskSimpleIKManager`:
  - `Clone` solo clona los huecos ocupados (antes llamaba a `Clone` sobre punteros nulos) y sin comprobar tipos;
  - `AddIKChainTask` busca hueco libre con cualquier hueco negativo y sobrescribe sin `assert`;
  - el constructor por defecto es propio: la copia ya no pone a cero el relleno.

**Método:**
- Peatones, clump, inteligencia, entidad, vehículos con conductor, asociaciones de animación, tareas de IK y evento falsos.
- `CWanted`, `IKChain_c` y `CTaskSimpleDuck` falsos en búferes propios.
- Los constructores corren sobre búferes 0xCD.
- En cada ejecución se restaura el pool de tareas; el reloj y el paso de tiempo se restauran al final.
- `GoToPointFine` se prueba también con una relación NaN silenciosa.
- Simulados y trazados:
  - animación (`BlendAnimation`, `AddAnimation`, asociaciones del clump, callbacks, tiempo y borrado por la vtable);
  - `Say`, `SetMoveState`, `SetMoveAnim`, `IsPlayer`, `FindPlayerWanted` y `RegisterCrime_Immediately`;
  - `GetTaskDuck`, `ControlDuckMove`, la IK de `CTaskSimpleGoTo` y los ángulos de `CGeneral`;
  - el gestor y las cadenas de IK, y el borrado, `Clone` y `ProcessPed` de las subtareas de IK.

Repetir: `python reversing/STasks8a/build_probe.py` y `python reversing/run.py --case stasks8a`. Evidencia: [validation-summary.json](validation-summary.json).

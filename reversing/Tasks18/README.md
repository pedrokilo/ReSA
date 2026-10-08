<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6822 con evidencia histórica, 0 con vigencia acreditada y 1624 pendientes en 64 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Gestor de tareas (lista de 18)

Validadas el 03/10/2026 con **12.160 pares original/candidato dentro de GTA**, todos exactos: 18 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskManager` | Constructor, Destructor, `GetActiveTask`, `FindActiveTaskByType`, `FindTaskByType`, `GetTaskSecondary`, `HasTaskSecondary`, `Flush`, `FlushImmediately`, `SetNextSubTask`, `StopTimers`, `GetSimplestActiveTask`, `AddSubTasks`, `ParentsControlChildren`, `SetTask`, `SetTaskSecondary`, `ClearTaskEventResponse`, `ManageTasks` |

**Diferencias reales corregidas:**
- `ManageTasks`:
  - la tarea primaria se procesa hasta **11** pasadas (el contador se incrementa antes de compararlo con 10), no 10;
  - vuelve a buscar la última tarea del árbol antes de `ProcessPed`, como el EXE;
  - borra la tarea y después pone el hueco a nulo.
- `HasTaskSecondary` compara sin comprobar nulos: con tarea nula devuelve cierto si algún hueco secundario está vacío.
- `FindActiveTaskByType` y `FindTaskByType` llaman a `GetSubTask` de la tarea encontrada antes de salir del bucle.
- `FlushImmediately`, `SetTask` y `SetTaskSecondary` borran la tarea antes de poner el hueco a nulo (el destructor ve el hueco aún lleno).

**Método:**
- Gestor falso en un buffer propio (0x30 bytes y 0x10 de guarda a 0xCD); peatón y evento falsos que solo se pasan como punteros.
- 12 tareas falsas simples o complejas con vtable trazada:
  - `GetSubTask`, `IsSimple` y `CTaskComplex::SetSubTask` (0x61A430) son las del EXE (la DLL llama a `SetSubTask` directamente porque es `final`);
  - `CreateFirstSubTask`, `CreateNextSubTask`, `ControlSubTask`, `ProcessPed`, `MakeAbortable`, `StopTimer`, `GetTaskType` y el destructor se trazan y se configuran por tarea con presupuestos finitos;
  - los árboles son acíclicos (una tarea solo crea o devuelve tareas de índice mayor) y el destructor anota el contenido del gestor al borrar.
- `CTask::IsTaskPtr` (assert de `AddSubTasks` en Debug) devuelve cierto sin traza durante el banco.
- Por ejecución se restaura `m_LastFreeSlot` del pool de tareas y se rellenan sus huecos libres con 0xCD; el constructor corre en la dirección del EXE sobre un buffer a 0xCD.
- Fuera de los fixtures:
  - `GetTaskSecondary(-1)` (FIX_BUGS devuelve nulo; el EXE lee el último hueco primario);
  - `SetNextSubTask` con una tarea simple (assert de la DLL);
  - `ControlSubTask` que cambia la subtarea cuando es nula (el EXE llamaría a `MakeAbortable` sobre nulo).

Repetir: `python reversing/Tasks18/build_probe.py` y `python reversing/run.py --case tasks18`. Evidencia: [validation-summary.json](validation-summary.json).

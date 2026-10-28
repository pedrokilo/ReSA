<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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

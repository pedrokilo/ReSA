<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Inteligencia de grupos de peatones (lista de 26)

Validadas el 04/10/2026 con **10.688 pares original/candidato dentro de GTA**, todos exactos: 25 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CPedGroupIntelligence` | Constructor, `Flush`, `AddEvent`, `SetTask`, `ReportFinishedTask` (implementación y envoltorio), `ReportAllTasksFinished`, `ReportAllBarScriptTasksFinished`, `IsGroupResponding`, `IsCurrentEventValid`, `SetGroupDecisionMakerType`, `SetPrimaryTaskAllocator`, `SetEventResponseTaskAllocator`, `SetScriptCommandTask`, `GetTaskMain`, `GetTaskSecondary`, `GetTaskSecondarySlot`, `GetTaskScriptCommand`, `GetTaskDefault`, `ProcessIgnorePlayerGroup`, `ComputeDefaultTasks`, `SetDefaultTaskAllocator`, `SetDefaultTaskAllocatorType`, `ComputeEventResponseTasks`, `Process` |

`FlushTasks` ya estaba validada en [FixPeds](../FixPeds/README.md). `SetEventResponseTask` (0x5F8510) sigue sin engancharse.

**Diferencias reales corregidas:**
- Constructor: no escribe el grupo, el evento actual ni el pendiente (los pone el constructor de `CPedGroup`), ni el relleno de los pares.
- `Flush`:
  - vacía los cuatro pares de cada miembro en un solo bucle (principal, secundario, por defecto y de script);
  - borra el evento actual antes que el pendiente.
- `AddEvent`:
  - devuelve `false` si el evento no tiene más prioridad que el pendiente (devolvía `true`);
  - borra el pendiente antes de clonar el nuevo;
  - mira el tipo con una sola llamada por la vtable (sin los casts con aserciones).
- `Process`:
  - recorre los 8 huecos de miembros por índice, también los nulos (la vista filtrada desplazaba los índices y no copiaba los huecos vacíos);
  - con eventos del mismo tipo mira si se puede interrumpir el pendiente, no el actual;
  - el resultado de `ProcessGroup` del asignador de respuesta se descarta (se guardaba como asignador principal);
  - borra el asignador de respuesta antes de calcular el nuevo y sin el `FIX_BUGS` (`GenericUB`) que anulaba el principal;
  - orden de las consultas de tipo como el EXE.
- `ProcessIgnorePlayerGroup`: sin evento actual no hace nada (borraba el asignador y vaciaba las tareas); pregunta el tipo dos veces.
- `SetScriptCommandTask` fuerza el cambio de tarea (`push 1`).
- `SetPrimaryTaskAllocator`: sin el `FIX_BUGS` (`GenericUB`) que anulaba el asignador de respuesta.
- `GetTaskSecondarySlot` devuelve -1 si el peatón no tiene par (era `NOTSA_UNREACHABLE`).
- `IsCurrentEventValid` pide la entidad origen tres veces; `SetTask` y `ReportFinishedTask` preguntan el tipo de la tarea del par antes que el de la nueva y `SetTask` borra la anterior después de guardar la nueva.

**Método:**
- La inteligencia vive en un buffer propio con 0x20 bytes de cola; el constructor se prueba sobre 0xCD.
- Grupos, peatones, eventos, tareas y asignadores son falsos, con vtables trazadas; los clones salen de pools fijos.
- Se simulan y trazan `FindPlayerPed`, `FindPlayerWanted`, `CPedGroups::GetPedsGroup`, `BaseEventTakesPriorityOverBaseEvent`, `ComputeResponseTaskType`, `WillRespond` y `CGroupEventHandler::ComputeEventResponseTasks` (en el EXE y en el cuerpo de la DLL).
- En `SetDefaultTaskAllocatorType` se cambia la vtable de los 6 asignadores estáticos del EXE durante la llamada.
- La traza es un array fijo que se pasa a json después de la llamada. No se tocan grupos reales.
- Pool de tareas: `m_LastFreeSlot` restaurado y huecos libres con 0xCD en cada ejecución.
- El fichero `pgi26-only.txt` en la carpeta del juego limita el banco a una función.

Repetir: `python reversing/PGI26/build_probe.py` y `python reversing/run.py --case pgi26`. Evidencia: [validation-summary.json](validation-summary.json).

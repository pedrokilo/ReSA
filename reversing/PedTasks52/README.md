<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6736 con evidencia histórica y 1710 pendientes en 77 grupos. Perfil histórico: 694 grupos / 6136 funciones. REA admite GTA SA x86; standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores de este documento conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas complejas, repartidores y peatones (lista de 52)

Validadas el 02/10/2026 con **11.072 pares original/candidato dentro de GTA**, todos exactos: 32 funciones con `validated=1`. `CPedToPlayerConversations::Update` ya estaba validada.

| Clase | Funciones |
|---|---|
| `CTaskComplexEnterCarAsDriver`, `CTaskComplexEnterCarAsPassenger`, `CTaskComplexGoToPointShooting` | `Clone`, Constructor, `GetTaskType` |
| `CTaskComplexLeaveCarAndDie` | `Clone`, `CreateFirstSubTask`, `GetTaskType` |
| `CTaskAllocator` | `Abort`, `IsFinished`, `ProcessGroup` |
| `CTaskAllocatorPlayerCommandAttack` | `AllocateTasks`, `GetType`, `ProcessGroup` |
| `CCivilianPed` | Constructor, Destructor, `ProcessControl` |
| `CCoverPoint` | `CanAccommodateAnotherPed`, `ReleaseCoverPointForPed`, `ReserveCoverPointForPed` |
| `CPedPlacement` | `IsPositionClearForPed`, `IsPositionClearOfCars` (peatón y posición) |
| `CPedToPlayerConversations` | `Clear`, `EndConversation` |
| `CSetPiece` | `TryToGenerateCopCar`, `TryToGenerateCopPed`, `Update` |

**Diferencias reales corregidas:**
- `CCivilianPed::Destructor`: 0x5DDC30 es el destructor con borrado (`ret 4`). La envoltura de la DLL no recibía el argumento, así que descuadraba la pila del llamador del EXE y nunca liberaba el peatón.
- `CTaskComplexEnterCar` (base de `AsDriver`/`AsPassenger`): sin inicializadores por valor en campos que el constructor del EXE no escribe.
- `CTaskAllocatorPlayerCommandAttack`:
  - `AllocateTasks` comprueba `m_PedTypeToAttack < 0`, llama dos veces a `GetLeader` y usa `SetTask` directamente;
  - `ProcessGroup` llama directamente al `AllocateTasks` de la clase base, como el EXE (la DLL despachaba al de esta clase).
- `CCoverPoint`:
  - `Reserve` sale si el peatón ya ocupa un hueco;
  - `Release` solo limpia los huecos de ese peatón.
- `CPedPlacement::IsPositionClearOfCars` (peatón): los coches que no son grandes se devuelven sin prueba de colisión; los demás necesitan `ProcessColModels`.
- `CPedToPlayerConversations::EndConversation` llama primero a `FindPlayerPed`.
- `CSetPiece`:
  - `TryToGenerateCopPed` tenía invertida la comprobación de objetos en colisión;
  - `TryToGenerateCopCar` normaliza la dirección y toma el radio de la model info;
  - `Update` reescrita: espera de 40 s entre generaciones (la DLL usaba 4), orden de las comprobaciones del área, prueba de aproximación solo en x/y, coches en estado de física, velocidad de los coches rápidos, y condiciones de salida con OR donde el EXE las tiene.

**Método:**
- Todo con objetos falsos.
- `CCivilianPed` en un búfer estático con el constructor, el destructor y `ProcessControl` de `CPed` simulados.
- Coches y policías generados con `operator new` y constructores simulados.
- Consultas del mundo simuladas y trazadas.
- `CTaskManager::SetTask` simulado (serializa y borra el árbol de tareas).
- Reloj, globales de conversaciones, model infos usadas, destinos de formación y el array de puntos de colisión restaurados por ejecución.

Repetir: `python reversing/PedTasks52/build_probe.py` y `python reversing/run.py --case ptasks52`. Evidencia: [validation-summary.json](validation-summary.json).

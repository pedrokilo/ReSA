<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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

<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas de salto, huida, tienda y coche (lista de 6)

Validadas el 02/10/2026 con **13.952 pares original/candidato dentro de GTA**, todos exactos: 42 funciones de 7 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexJump` | Constructor, `Clone`, `CreateFirstSubTask`, `CreateNextSubTask`, `CreateSubTask`, `MakeAbortable` |
| `CTaskComplexWanderFlee` | Constructor, Destructor, `Clone`, `ControlSubTask`, `GetWanderType`, `ScanForStuff` |
| `CTaskInteriorBeInShop` | Constructor, Destructor, `Clone`, `GetTaskType`, `GetWanderType`, `ScanForStuff` |
| `CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleCarSetPedSlowDraggedOut` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleCarSetTempAction` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleCarWaitToSlowDown` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |

**Diferencias reales corregidas:**
- `CTaskSimpleCarSetTempAction`:
  - `MakeAbortable` también reinicia el tiempo de la acción temporal (`SetTempAction(NONE, 0)` en línea), no solo la acción;
  - `ProcessPed` calculaba el puntero al autopiloto antes de tomar el vehículo del peatón (con `m_pVehicle` nulo escribía cerca de la dirección 0x390).
- `CTaskSimpleCarSetPedSlowDraggedOut::ProcessPed`:
  - la tarea de quedarse quieto se elige por `m_nCreatedBy == PED_MISSION` (+0x484), no por `m_nPedType`;
  - el rumbo nuevo es el del **vehículo** (`fpatan` del x87), no el del peatón;
  - la velocidad se anula después de buscar la Z, justo antes de colocar al peatón.
- `CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::Clone`: el constructor de copia leía las subtareas del objeto nuevo (sin inicializar) en vez de las del original.
- `CTaskInteriorBeInShop`:
  - `GetTaskType` devolvía el `Type` heredado de `CTaskComplexWander` (0x384) en vez de `TASK_INTERIOR_BE_IN_SHOP` (0x57C);
  - el constructor calcula la dirección como `(rand() & 0xFFFF) / 32768 * 8` truncado, no con `RandomNodeHeading`;
  - `ScanForStuff` usa `GetRandomNumberInRange(0, 100)` con umbral 50 (mirar o evento) y 80 (evento y dejar de buscar), no `DoCoinFlip`.
- `CTaskComplexWander::ControlSubTask` (lo hereda `CTaskComplexWanderFlee`): `Say(CTX_GLOBAL_CHAT)` con probabilidad 0,1 y el tipo de la subtarea se pide en cada comprobación, como el EXE.

**Método:**
- Constructores en la dirección del EXE sobre un buffer con 0xCD (se compara el tipo en vez del vptr); los métodos virtuales por la entrada de la vtable del EXE.
- En la ejecución original también se desactiva `CTaskComplexWander::ControlSubTask` (el `ControlSubTask` de WanderFlee del EXE es un salto a 0x674C30).
- Peatones con vtable falsa (`SetMoveAnim`), vehículos con vtable falsa (`CanPedStepOutCar`/`CanPedJumpOutCar`), subtareas y evento falsos con vtables trazadas, información de interiores en buffers propios.
- Se simulan y trazan las llamadas externas: RNG, `Say`, `IsInPlayersGroup`, `CTaskSimpleCarDrive::MakeAbortable/ProcessPed`, quitar conductor/pasajero, estado de policía, animaciones, estadísticas, armas, orientación, colisión, `SetTask`, desplazamientos de puertas, `FindZCoorForPed`, `IsRoomForPedToLeaveCar`, `GetPositionToOpenCarDoor`, `CTaskSimpleStandStill::ProcessPed`, los auxiliares de `CTaskComplexWander`, `FindClosestInteriorInfo`, `LookAt` y `CEventGroup::Add`.
- `CCarEnterExit::ComputeDoorFlag` (0x64E550) se sustituye en silencio: el EXE lo llama sin usar el resultado.
- Tareas nuevas del pool seguidas (m_LastFreeSlot restaurado y huecos libres con 0xCD); se guardan y restauran el reloj y `ms_vecPedGetUpAnimOffset`.

Repetir: `python reversing/CTasks6b/build_probe.py` y `python reversing/run.py --case ctasks6b`. Evidencia: [validation-summary.json](validation-summary.json).

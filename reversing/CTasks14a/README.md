<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Entrar en el coche y deambular (lista de 14)

Validadas el 02/10/2026 con **11.776 pares original/candidato dentro de GTA**, todos exactos: 27 funciones de 2 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexEnterCar` | Constructor, Destructor, `MakeAbortable`, `CreateNextSubTask`, `CreateFirstSubTask`, `ControlSubTask`, `CreateNextSubTask_AfterSimpleCarAlign`, `CreateSubTask`, `CreateTaskUtilityLineUpPedWithCar`, `GetTargetPos`, `GetCameraAvoidVehicle`, `PreparePedForVehicleEnter`, `PrepareVehicleForPedEnter`, `SetVehicleFlags` |
| `CTaskComplexWander` | Constructor, `ControlSubTask`, `CreateFirstSubTask`, `UpdateDir`, `UpdatePathNodes`, `CreateSubTask`, `ComputeTargetPos`, `ComputeTargetHeading`, `ValidNodes`, `ScanForBlockedNodes`, `ScanForBlockedNode` (dos versiones), `GetWanderTaskByPedType` |

**Diferencias reales corregidas:**
- `CTaskComplexEnterCar`:
  - destructor: suelta la referencia del coche antes de borrar la utilidad de alineación;
  - `CreateNextSubTask`: tras `GO_TO_CAR_DOOR` copia el punto objetivo (no el asiento); `LEAVE_CAR` vuelve a la puerta o a la barca (antes abortaba); con abandono `QUICK_DRAG_PED_OUT` termina y los tipos desconocidos devuelven `nullptr`; tras caerse llama a `CreateSubTask(FINISHED)` antes de `CreateFirstSubTask`; la velocidad del coche se compara con `fsqrt`; la puerta usable es `IsCarDoorReady || CarHasPartiallyOpenDoor`;
  - el arrastre lento solo se decide con gateo lento requerido, el peatón se guarda sin soltar la referencia anterior, `AreInSameGroup(arrastrado, peatón)`, la subtarea se crea antes de las tareas y eventos del arrastrado y el evento `DraggedOutCar` se añade como válido y vive hasta el final;
  - el conductor sacado del coche recibe `LeaveCarAndDie` (0x62F1A0) o `LeaveCar(…, 0, 0, true)` según `IsAlive`/salud del EXE (NaN incluido); la puerta del jugador con acelerador o freno a fondo se abre por componente antes de `CarHasDoorToClose`;
  - `CreateNextSubTask_AfterSimpleCarAlign`: decide la subtarea y después mira a los 8 asientos y al conductor con `GetRandomIntInRange`; en bicis la subtarea va antes de las tareas `BikeJacked` y el pasajero cae 200 ms después y sin el último argumento; NaN en la inclinación recoge la bici;
  - `CreateSubTask`: `LEAVE_CAR` usa el vehículo del peatón; `FALL_AND_GET_UP` reserva la tarea primero, mira `IsDoorMissingU32` antes del tipo y abre la puerta por componente (sin `assert`); el crucero del jugador exige **no** salir tras abrir la puerta; tipos desconocidos devuelven `nullptr`;
  - `ControlSubTask` devuelve la subtarea leída al entrar, mira `IsGroupLeader` antes de la subtarea y actualiza el punto de la puerta al final; `CreateFirstSubTask` pide el grupo siempre; `MakeAbortable` mantiene viva la tarea `StandStill` hasta el final; `CreateTaskUtilityLineUpPedWithCar` calcula el desplazamiento antes de reservar.
- `CTaskComplexWander`:
  - `UpdateDir` y `ScanForBlockedNodes` llaman a `UpdatePathNodes` con copias locales de los nodos (los miembros no cambian); el bloqueo compara el nodo nuevo con `m_LastNode` (antes comparaba direcciones) y la dirección de salida empieza como el EXE;
  - `ValidNodes` y `ScanForBlockedNodes` solo miran `m_wAreaId`; `ScanForBlockedNode` deja pasar NaN y suma 1 a z;
  - `ControlSubTask` devuelve la subtarea leída antes de `ScanForStuff`; `GetWanderTaskByPedType` reserva la tarea antes de la tirada y usa `GetRandomIntInRange` (0x407180).

**Método:**
- 6 peatones con vtable falsa (`SetMoveAnim` trazado; el 4 y el 5 hacen de jugadores 0 y 1), 2 vehículos con vtable falsa (`IsDoorMissingU32` trazado), datos de manejo propios, daños de puertas y ocupantes aleatorios.
- El objeto bajo prueba tiene vtable falsa: `CreateFirstSubTask`, `CreateNextSubTask_AfterSimpleCarAlign`, `ScanForStuff`, `UpdateDir` y `UpdatePathNodes` internos van trazados para probar cada función por separado.
- Simulados con traza: RNG, `CCarEnterExit` (salud, puertas, gateo, índices, posiciones), grupos y eventos de grupo, `CEventGroup::Add`, `AddTaskPrimaryMaybeInGroup`, `IsFriendlyWith`, `GetTaskDuck`, `SetMoveState`, `SetRealMoveAnim`, `SetLastTargetVehicle`, ayuda y texto, `PositionPedOutOfCollision`, `LookAt`, mando, `ProcessPed`/`MakeAbortable` de las tareas temporales, `TakeWidthIntoAccountForWandering`, `FindNextNodeWandering`, escáneres y `Say`.
- Nodos de camino propios en `ThePaths.m_pPathNodes[70/71]`; se guardan y restauran reloj, contador de fotogramas, `SkipState`, la ayuda ya mostrada y los jugadores 0 y 1.
- Pools de tareas y eventos restaurados en cada ejecución (slots libres con 0xCD); lo nuevo se serializa y se libera sin destructor. La utilidad de alineación nueva se serializa y se borra; constructores sobre buffers con 0xCD comparados enteros salvo el vptr (clases abstractas).
- Durante el banco el hook de `CDamageManager::GetDoorStatus_Component` (0x6C2250) se desactiva: el EXE devuelve el estado extendido a 32 bits y sus llamadores comparan `eax` entero, pero la versión del DLL deja basura en los bits altos.
- Fuera del banco: la rama `FIX_BUGS` de `CreateTaskUtilityLineUpPedWithCar` (con utilidad ya creada el EXE la pierde y reserva otra; el DLL la reutiliza): los casos que pueden crearla empiezan sin ella.

Repetir: `python reversing/CTasks14a/build_probe.py` y `python reversing/run.py --case ctasks14a`. Evidencia: [validation-summary.json](validation-summary.json).

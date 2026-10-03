# Respuesta editable, conducir, ser arrastrado y huir a un punto (lista de 11)

Validadas el 02/10/2026 con **12.672 pares original/candidato dentro de GTA**, todos exactos: 44 funciones de 4 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CEventEditableResponse` | Constructor, `Clone`, `HasEditableResponse`, `WillRespond`, `InformVehicleOccupants`, `InformRespectedFriends`, `InformGroup`, `TriggerLookAt`, `ComputeResponseTaskType` (peatón y grupo), `ComputeResponseTaskOfType` |
| `CTaskComplexCarDrive` | Constructores (`Constructor_0`, `Constructor_1`), Destructor, `Clone`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `CreateSubTask`, `CreateSubTaskCannotGetInCar`, `Drive`, `SetUpCar` |
| `CTaskComplexCarSlowBeDraggedOut` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `CreateSubTask`, `CreateTaskUtilityLineUpPedWithCar`, `PrepareVehicleForPedExit` |
| `CTaskComplexFleePoint` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `CreateSubTask`, `ComputeTargetPoint`, `SetFleePosition` |

**Diferencias reales corregidas:**
- `CEventEditableResponse`:
  - `InformRespectedFriends` recorre los 16 huecos del escáner y para cuando ha informado (cola aceptada) a `m_nDmNumPedsToScan` amigos; si el que informa es el jugador cuenta el respeto del amigo hacia él (antes se miraba si el amigo era el jugador); distancia `(z² + x²) + y²` con comparación estricta y sin comprobar `m_pWanted`;
  - `ComputeResponseTaskType` (peatón y grupo) y `ComputeResponseTaskOfType`: `GetEventType`, `ComputeEventSourceType` y `GetInstance` en el orden del EXE, `GetEventType` una sola vez en la versión de grupo y una sola llamada a `ComputeEventSourceType` en `ComputeResponseTaskOfType`.
- `CTaskComplexCarDrive`:
  - `CreateSubTaskCannotGetInCar` es virtual (slot 12): se engancha con `RH_ScopedVMTInstall` (el gancho plano entraba en bucle);
  - `CreateNextSubTask`: cualquier modelo negativo pide `CreateSubTaskCannotGetInCar`, `CREATE_CAR_AND_GET_IN` registra el vehículo del peatón sin limpiar la referencia anterior, `GO_TO_POINT_ANY_MEANS` termina y los tipos desconocidos devuelven `nullptr`;
  - `ControlSubTask` (`GO_TO_POINT_ANY_MEANS`) registra el vehículo sin limpiar la referencia anterior;
  - `CreateSubTask` devuelve `nullptr` en tipos desconocidos y no inicializa por valor `CTaskComplexEnterAnyCarAsDriver`.
- `CTaskComplexCarSlowBeDraggedOut`:
  - `MakeAbortable` estaba invertido: solo con prioridad inmediata aborta la subtarea, saca al peatón y devuelve `true`;
  - `CreateSubTask` pasa `!m_IsDraggedBySelf` a `CTaskSimpleCarSetPedOut` y devuelve `nullptr` en tipos desconocidos;
  - `CreateNextSubTask`: `SET_PED_OUT` termina y los tipos desconocidos devuelven `nullptr`;
  - `CreateFirstSubTask` no hace nada fuera del vehículo; `CreateTaskUtilityLineUpPedWithCar` no libera la utilidad anterior y el destructor limpia la referencia (sin anular el campo) antes de borrarla.
- `CTaskComplexFleePoint`:
  - el constructor no escribe el destino ni el relleno del temporizador y arranca el temporizador con cualquier duración distinta de -1;
  - `ComputeTargetPoint`: `CVector::Normalise` con z = 0, ángulo y separación con la versión float de `GetRandomNumberInRange` (la separación era entera) y `fcos`/`fsin` x87;
  - `CreateNextSubTask`: calcula el destino también tras `LEAVE_ANY_CAR` y decide con la distancia del destino al punto del que huye (`(z² + y²) + x²`, estricta);
  - `ControlSubTask`: cambio de subtarea antes de la mirada (orden de `rand`), temporizador sin comprobar la duración, `AbortLookAt` solo si está mirando y probabilidad `(int)(rand·(1/32768)·100) <= 5`;
  - `SetFleePosition` también actualiza si solo cambia la distancia segura; `CreateSubTask` devuelve `nullptr` en tipos desconocidos y no inicializa por valor la secuencia.

**Método:**
- 6 peatones con inteligencia propia (grupo de eventos, escáner de peatones, radio), conocidos, vehículo y datos de jugador con un `CWanted` falso; 2 vehículos con vtable falsa (`CanPedStepOutCar` trazado), piloto automático, conductor y pasajeros; un `CPedGroup` falso con líder y miembros.
- Evento falso derivado de `CEventEditableResponse`: su vtable se copia de la del EXE (0x85AB80) en cada ejecución, con el estado de los hooks, y se trazan el destructor, `GetEventType`, `GetSourceEntity`, `DoInformVehicleOccupants` y `CloneEditable` (clones en buffers propios).
- Simulados con traza: RNG, `CPed::Say`, `LookAt`/`IsLooking`/`AbortLookAt`, `CEventGroup::Add`, las colas de amigos y de grupo, `GetPedsGroup`, `ComputeEventSourceType`, las dos `MakeDecision`, `FindPlayerPed`, `CTaskSimpleCarSetPedSlowDraggedOut::ProcessPed` y operator new/delete (solo durante la llamada, registro sin reservas).
- `ComputeResponseTaskOfType`: el EXE pasa a `MakeDecision` dos locales sin inicializar (la DLL los pone a -1); su valor previo se enmascara en la traza (el simulado siempre los escribe).
- Pool de tareas restaurado en cada ejecución (slots libres con 0xCD); las tareas nuevas se serializan y se liberan sin destructor. Constructores sobre buffers con 0xCD comparados enteros. Distancias exactas al radio, NaN, temporizadores justo cumplidos y duraciones negativas.

Repetir: `python reversing/CTasks11/build_probe.py` y `python reversing/run.py --case ctasks11`. Evidencia: [validation-summary.json](validation-summary.json).

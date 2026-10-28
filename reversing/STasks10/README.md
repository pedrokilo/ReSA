<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas simples de coche (lista de 10)

Validadas el 02/10/2026 con **19.136 pares original/candidato dentro de GTA**, todos exactos: 50 funciones de 5 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskSimpleCarCloseDoorFromOutside` | Constructor, Destructor, `Clone`, `ComputeAnimID`, `FinishAnimCarCloseDoorFromOutsideCB`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `SetPedPosition`, `StartAnim` |
| `CTaskSimpleCarGetOut` | Constructor, Destructor, `Clone`, `ComputeAnimID_ToHook`, `FinishAnimCarGetOutCB`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `SetPedPosition`, `StartAnim` |
| `CTaskSimpleCarJumpOut` | Constructor, Destructor, `Clone`, `ComputeAnimID_ToHook`, `FinishAnimCarJumpOutCB`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `SetPedPosition`, `StartAnim` |
| `CTaskSimpleCarOpenDoorFromOutside` | Constructor, Destructor, `Clone`, `ComputeAnimID_ToHook`, `FinishAnimCarOpenDoorFromOutsideCB`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `SetPedPosition`, `StartAnim` |
| `CTaskSimpleCarSlowDragPedOut` | Constructor, Destructor, `Clone`, `ComputeAnimID_Wrapper`, `FinishAnimCarSlowDragPedOutCB`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `SetPedPosition`, `StartAnim` |

**Diferencias reales corregidas:**
- Todas: `ComputeAnimID` con una puerta fuera de 8–11 (18) no escribe la animación y usa el valor que ya tenía (la DLL abortaba); abrir la puerta en el `CDamageManager` va por componente (`Get/SetDoorStatus_Component`, cerrada → abierta, dañada → abierta y dañada), no por `eDoors`.
- `CTaskSimpleCarCloseDoorFromOutside`:
  - el destructor restaura el callback de **fin** (la DLL tocaba el de borrado);
  - `StartAnim` usa el resultado de `FindZCoorForPed` (la DLL lo descartaba), termina solo si está estrictamente más de 1 m por encima, consulta el motor antes que el mando y crea la animación con `AddAnimation` (no `BlendAnimation`);
  - `MakeAbortable` pasa el peatón a `ProcessOpenDoor` (la DLL pasaba `nullptr`).
- `CTaskSimpleCarGetOut`:
  - `ComputeAnimID` sobrescribe la bandera de volcado con `IsCarUpsideDown` (también a `false`);
  - `ProcessPed` devuelve `true` sin vehículo y el truco `SMASH_N_BOOM` solo afecta al jugador.
- `CTaskSimpleCarJumpOut`:
  - el constructor **registra** la referencia al vehículo (la DLL la limpiaba);
  - `MakeAbortable` pide el tipo del evento antes de mirar la animación;
  - `ProcessPed`: `pow(0,96, paso)` con el `_CIpow` del EXE, umbral 0,7333333 exacto (0x86F0E8), avión por subtipo, bit 0x8 del IK (no `bUseArm`), PI exacto del EXE y la tarea `CarSetPedOut` local vive hasta el final;
  - `SetPedPosition` llama a `FindZCoorForPed` con cualquier animación (el id solo decide el ajuste) y usa su resultado.
- `CTaskSimpleCarOpenDoorFromOutside`: el constructor registra el miembro `m_veh` (la DLL registraba la dirección del parámetro en la pila); `ProcessPed` invertía la condición (la puerta se abre a mano solo en automóviles antes del tiempo de apertura) y el grupo sale del id de la animación en curso.
- `CTaskSimpleCarSlowDragPedOut`: `ProcessPed` con aleatorio > 512 dice primero `SayJacking` del que saca y luego `SayJacked` del sacado (la DLL cambiaba los peatones); el constructor no inicializa por valor la base.

**Método:**
- Constructores en la dirección del EXE sobre un buffer con 0xCD (se compara el tipo en vez del vptr); los métodos virtuales por la entrada de la vtable del EXE; `ComputeAnimID` con las dos salidas prerrellenadas; los callbacks como `__cdecl(anim, tarea)`, con los nombres de callback resueltos para el EXE y la DLL.
- Peatones falsos (clump con tipo 2, matriz, banderas, IK, tipo de peatón) y vehículo falso del tamaño de `CAutomobile` con vtable propia (`OpenDoor`, `ProcessOpenDoor`, `GetDooorAngleOpenRatioU32`), matriz con `right.z`/`up.z` en los umbrales, velocidades, tipo/subtipo, conductor/pasajeros, fuego, cierre, motor y puertas del `CDamageManager`; evento falso con `GetEventType`.
- Tiempos de animación en los umbrales 0,7333333 / 0,45 / 0,35 / 0,15 / 0,07 y en el tiempo de apertura del grupo, con sus vecinos exactos; mezclas 0 y 1 exactas; alturas de `FindZCoorForPed` en `z − 1` y `z` con sus vecinos.
- Se simulan y trazan animaciones, `GetGroup`, la utilidad de alineación, `FindZCoorForPed` (EXE en el sitio, DLL por valor), `IsCarUpsideDown`, `CPad::GetTarget`, `CTaskSimpleCarSetPedOut::ProcessPed`, `GetHeading`/`SetHeading`/`LimitRadianAngle`, `SayJacked`/`SayJacking`, `CarHasDoorToOpen`, `ComputePassengerIndexFromCarDoor` y el estado de puertas del `CDamageManager` (con la semántica del EXE: el hook de la DLL devuelve un `uint8` y el EXE compara `eax` entero).
- Puertas fuera de 8–11 (18) solo en `ComputeAnimID` y en los constructores (el resto de llamadores del EXE leen una variable de pila sin inicializar); el conductor es siempre el peatón que devuelve `FindPlayerPed` (la DLL conserva un `FIX_BUGS` en `CTaskSimpleCarOpenDoorFromOutside::ProcessPed`).
- Se guardan y restauran tras cada ejecución el paso del reloj, el truco `CHEAT_SMASH_N_BOOM`, `CWorld::pIgnoreEntity`, los mandos 0 y 1, los grupos de animación de vehículo, el pool de tareas y la lista libre de referencias.

Repetir: `python reversing/STasks10/build_probe.py` y `python reversing/run.py --case stasks10`. Evidencia: [validation-summary.json](validation-summary.json).

<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6916 con evidencia histórica, 0 con vigencia acreditada y 1530 pendientes en 41 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas complejas A (lista de 10)

Validadas el 02/10/2026 con **15.488 pares original/candidato dentro de GTA**, todos exactos: 40 funciones de 4 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexDestroyCarArmed` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `CalculateSearchPositionAndRanges`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `CreateSubTask` |
| `CTaskComplexDestroyCarMelee` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `CalculateSearchPositionAndRanges`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `CreateSubTask` |
| `CTaskComplexKillPedOnFootMelee` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `CalculateSearchPositionAndRanges`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `CreateSubTask` |
| `CTaskComplexLeaveCarAsPassengerWait` | Constructor, Destructor, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `CreateSubTask`, `GetNextPassengerToLeave`, `SetPedInZerothPassengerSeat` |

**Diferencias reales corregidas:**
- `CTaskComplexDestroyCarArmed`:
  - el constructor ya no escribe el byte 0xC (crear primera subtarea) y, sin `FIX_BUGS`, registra la referencia del vehículo en la dirección de su parámetro en la pila como el EXE (referencia colgante; con `FIX_BUGS` se registra el campo);
  - `ControlSubTask` mira `bStayInSamePlace` (bit 21, 0x200000), no `bKindaStayInSamePlace`, y una distancia NaN no se queda con la subtarea;
  - `CreateFirstSubTask` dispara también con distancia NaN (`!(d > rango)`);
  - `CreateNextSubTask` (pausa) `!(rango² > distancia²)` (NaN y empate van a pausa);
  - `CalculateSearchPositionAndRanges` conserva un rango NaN en el mínimo de 0,8.
- `CTaskComplexDestroyCarMelee`:
  - el constructor ya no escribe el ángulo (+0x24);
  - `MakeAbortable`: velocidad al cuadrado frente a 0,0225 (0x406DA0 es `SquaredMagnitude`), producto escalar en orden z, y, x, `GetEventType` consultado dos veces como el EXE, la subtarea recibe el evento y cualquier prioridad desconocida marca abortar al terminar;
  - `CalculateSearchPositionAndRanges`: ángulo con `fpatan` (atan2(dx, -dy) a precisión completa);
  - `CreateFirstSubTask`/`ControlSubTask`: sumas z, x, y, NaN dentro en la primera y comparación estricta en la segunda; con la lucha en marcha distancia `sqrt` frente a rango + 0,6; tipos desconocidos devuelven la subtarea;
  - `CreateNextSubTask`: comparación estricta `rango² > distancia²`; `TASK_FINISHED` y tipos desconocidos devuelven `nullptr` sin tocar el temporizador.
- `CTaskComplexKillPedOnFootMelee`:
  - `MakeAbortable` propio (no el heredado de `CTaskComplex`);
  - `CreateSubTask` reinicia el temporizador de pausa en todos los tipos menos la pausa, la pausa dura 100 ms, se ignora el resultado de `ProcessPed` y la secuencia no se inicializa por valor;
  - `CreateNextSubTask`: bajo el agua se queda quieto (estaba invertido) y fuera vuelve a `CreateFirstSubTask`;
  - comparaciones exactas del EXE (`SquaredMagnitude` o sumas z, x, y según la rama), con NaN y empates como el original;
  - `ControlSubTask` trata `STAND_STILL` como la secuencia;
  - índice de combos sin aserción de `std::array`.
- `CTaskComplexLeaveCarAsPassengerWait`:
  - `GetNextPassengerToLeave` recorre `m_nMaxPassengers` asientos sin el tope de 8;
  - `CreateSubTask` (ir a un punto): tres `rand()` en el orden del EXE (frontal, signo `> 0,5`, lateral) y orden de sumas del desplazamiento;
  - tipos desconocidos devuelven `nullptr` y `CreateFirstSubTask` sin vehículo no llama a `CreateSubTask`.

**Desviación FIX_BUGS:** el constructor de `CTaskComplexDestroyCarArmed` del EXE registra la referencia del vehículo en la dirección de su parámetro en la pila (referencia colgante); con `FIX_BUGS` la DLL registra el campo +0x10. En `Constructor` y `Clone` el banco normaliza esa referencia (`stack` en el EXE, `object+16`/`T0+16` en la DLL) a `veh_ref` antes de comparar.

**Método:**
- 4 peatones con inteligencia propia (tarea de lucha secundaria falsa con juego de combos), armas, salud y flags; 2 vehículos con vtable falsa y modelo propio; `CBaseModelInfo`/`CColModel` falsos (se cambia temporalmente `ms_modelInfoPtrs[400]`); evento falso con `GetEventType` trazado y tareas falsas con vtable trazada (`MakeAbortable` responde según una máscara).
- Pool de tareas restaurado en cada ejecución (slots libres con 0xCD); las tareas nuevas se serializan y se liberan sin destructor. El reloj y el puntero del modelo se guardan y se restauran.
- RNG, `CPed::Say`, `CPed::SetMoveState`, `CInterestingEvents::Add` y los `ProcessPed` de las tareas locales (`CTaskSimpleStandStill`, `CTaskSimpleCarSetPedOut`, `CTaskSimpleCarSetPedInAsPassenger`) simulados con traza de sus campos.
- Distancias exactas (`d`, `d²` y NaN) respecto a los rangos de arma, de combos y de la esfera del modelo, velocidad exactamente 0,15 y 0,0225, referencias en la pila registradas como `stack` y temporizadores de pausa justo cumplidos.

Repetir: `python reversing/CTasks10a/build_probe.py` y `python reversing/run.py --case ctasks10a`. Evidencia: [validation-summary.json](validation-summary.json).

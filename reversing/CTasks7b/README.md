<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas complejas B (lista de 7)

Validadas el 02/10/2026 con **10.496 pares original/candidato dentro de GTA**, todos exactos: 35 funciones de 5 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexKillPedOnFootStealth` | Constructor, Destructor, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexLeaveAnyCar` | Constructor, Destructor, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexPartnerGreet` | Constructor, Destructor, `Clone`, `GetTaskType`, `CreateFirstSubTask`, `GetPartnerSequence`, `StreamRequiredAnims` |
| `CTaskComplexSeekEntityAiming` | Constructor, Destructor, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexSmartFleeEntity` | Constructor, Destructor, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |

**Diferencias reales corregidas:**
- `CTaskComplexKillPedOnFootStealth`:
  - `Clone` construía la copia con el `m_target` del objeto nuevo (sin inicializar) en lugar del original;
  - `ControlSubTask`: dirección objetivo − peatón normalizada, producto `(z + y) + x`, `CanSeeEntityWithLights` se llama antes de comparar el producto y ambas pruebas son estrictas (`> 0`, `> umbral`);
  - la espera de 5 s usa diferencia con signo (`jle`);
  - con `z <= -9999,9` la secuencia (quieto 1 s, `ROADCROSS`, rascarse) se crea cuando la subtarea **no** es ya una secuencia; con `z` NaN no se crea nada.
- `CTaskComplexPartnerGreet`:
  - `GetPartnerSequence` reconstruida (antes `NOTSA_UNREACHABLE`): girarse, saludo según el tipo, mano, pausas y charla con `3000 - (int)(rand · 1/32768 · -4000)` y segunda charla si `rand() < 0x3FFF`;
  - `StreamRequiredAnims`: `CAnimManager::StreamAnimBlock` tenía la lógica invertida; se transcribe el EXE en la clase (bloque cargado comparado con `== 1`);
  - el constructor copia "gangs" con `strcpy` (6 bytes) en lugar de `strcpy_s`.
- `CTaskComplexSeekEntityAiming`:
  - `Clone` pasaba el radio de apuntado como radio de búsqueda;
  - `ControlSubTask`: si la tarea secundaria de ataque existe pero no es `TASK_SIMPLE_USE_GUN` no se reemplaza; un radio o distancia NaN termina el apuntado;
  - tipos de subtarea desconocidos devuelven `nullptr`/la subtarea en lugar de `NOTSA_UNREACHABLE`.
- `CTaskComplexSmartFleeEntity`:
  - el constructor no escribe la posición ni el relleno del temporizador; `Clone` copia el estado de movimiento;
  - `CreateFirstSubTask` mira a la cabeza si la entidad es un **peatón** (tipo), no según el estado;
  - `CreateNextSubTask`: `STAND_STILL` con entidad vuelve a huir;
  - `ControlSubTask`: el temporizador solo se reinicia cuando la entidad se aleja más que la tolerancia (`tol² < (dz² + dy²) + dx²`, estricto), arranca sin comprobar el intervalo y el evento de pánico se añade si el estado de movimiento es `> 4` con signo, construido en la pila;
  - `CreateSubTask` implementa `STAND_STILL` y devuelve `nullptr` en tipos desconocidos.
- `CTaskComplexLeaveAnyCar`: el constructor no inicializa por valor la base.

**Método:**
- 4 peatones con inteligencia propia (ranura secundaria de ataque), 2 vehículos con vtable falsa y tareas falsas con vtable trazada.
- Las funciones aún no reconstruidas de `CTaskComplexKillPedOnFoot` (0x62B490, 0x626260, 0x625E70) y `CTaskComplexPartner::CreateFirstSubTask` (0x681F20) se simulan con traza.
- `CanPedTargetPed`, `CanSeeEntityWithLights`, `Say`, bloques de animación y streaming, `ControlGun`, eventos interesantes, `IKChainManager_c`, `SetFleePosition`, `CEventGroup::Add` y el RNG simulados con traza.
- `CPedAcquaintanceScanner::ms_fThresholdDotProduct` y el reloj se fijan por caso y se restauran; casos de borde: distancias exactamente iguales al radio/tolerancia, `z = -9999,9` y sus vecinos, NaN y relojes que dan la vuelta.
- Las tareas nuevas del pool (secuencias con sus subtareas, `UseGun` de la ranura secundaria) se serializan y se borran después.

Repetir: `python reversing/CTasks7b/build_probe.py` y `python reversing/run.py --case ctasks7b`. Evidencia: [validation-summary.json](validation-summary.json).

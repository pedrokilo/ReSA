<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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

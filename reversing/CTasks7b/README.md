<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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

## Adaptación a la base 0x681F20 con hook Virtual (2026-10-19)

**Estado: ADAPTADO, PENDIENTE DE COMPILAR/PROBAR.** Sin compilar ni ejecutar. Misma técnica que `reversing/PartnerDealFirst31` (aprobado en juego).

- Problema: `FakePartnerFirst` sólo estaba parcheado en la entrada del EXE (`p1e`, 0x681F20). Con `CTaskComplexPartner::CreateFirstSubTask` en C++ (`RH_ScopedVMTInstall(CreateFirstSubTask, 0x681F20)`), `CTaskComplexPartnerGreet::CreateFirstSubTask` llama al cuerpo de la DLL, que no pasaba por el falso.
- Solución (`PartnerSwitch` en `LiveComparison.hpp`): el cuerpo de la base se resuelve con su hook ENCENDIDO antes de crear parches; el MISMO `FakePartnerFirst` se parchea en 0x681F20 (original, hook apagado) y en el cuerpo de la DLL (candidato, hook encendido). Los 6 puntos de cambio `for(auto* h:hooks)h->State(x)` son ahora `SetAll(x)`, que también conmuta el hook de la base y re-aplica los parches en el orden seguro (EntryPatch::Enable(false) escribe los bytes guardados al construir, por eso se desactiva el parche del estado que se abandona antes de cambiar el hook y se activa el nuevo después).
- Compuerta por lado: se integra en el campo `switches` (la comparación original==candidato y `switches==true` ya existente): hook de la base en el estado del lado, 0x681F20 salta a `GetHookOurAddress()` sólo en el candidato (módulo ASI). Fin: `PartnerSwitch::Finish()` restaura 0x681F20, el cuerpo y el hook (estado y bloqueo); el evento final lleva `partner_restored` y se incluye en `state_restored`.
- Evento nuevo `partner_hook` (our, body, estado inicial) tras resolver; `run_local.py` lo exige (cuerpo en la DLL) y exige `partner_restored`.
- Sin cambios: tabla de funciones, fixtures, EXPECTED (10496 pares, calculado por la misma regex) y trazas de las demás funciones. 0x681F20 sólo la usa `CTaskComplexPartnerGreet::CreateFirstSubTask`; en el resto de casos el hook de la base queda apagado (estado del original) y su parche del EXE es el mismo que antes.
- Arreglos de banco antiguo: `build_probe.py` y `run_local.py` fijan `runtime_runner.GAME = E:/JUEGOS/GTA San Andreas - CVehicle Test`; `analyze` devuelve `None` si el `.jsonl` no existe. `build_probe.py` añade `TaskComplexPartner.cpp/.h` y los hooks `Simple`/`Virtual`.
- Supuestos: el hook se llama `CreateFirstSubTask` en la categoría `CTaskComplexPartner` y es Virtual ("V"). Si falta, el banco escribe `error` y termina. Si el compilador inlinara la base en Greet, la traza `partner_create_first` faltaría en el candidato (fallo visible).

Comandos: `python -B reversing/CTasks7b/build_probe.py`; `python -B reversing/CTasks7b/run_local.py`; `python -B reversing/CTasks7b/run_local.py --analyze <carpeta>/comparisons.jsonl`.

**Corrección (primera corrida, 100 % de fallos):** `SideOK(false)` exigía `!JumpsToDll(0x681F20)`, pero en el original 0x681F20 lleva el parche `E9 -> FakePartnerFirst` (que vive en el ASI), así que `JumpsToDll` era siempre verdadero y `switches` falso en el lado original. Ahora el original exige hook apagado y que 0x681F20 sea `E9` con destino exactamente `FakePartnerFirst`; el candidato sigue exigiendo que salte a `GetHookOurAddress()` (módulo ASI). (También se quitó la coma suelta tras `k3e(...)`.)

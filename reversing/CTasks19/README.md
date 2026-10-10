<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas complejas y repartidor (lista de 19)

Validadas el 02/10/2026 con **12.544 pares original/candidato dentro de GTA**, todos exactos: 36 funciones de 9 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTask` | `IsTaskPtr`, `StopTimer`, `operator new`, `operator delete` |
| `CTaskAllocatorKillOnFoot` | Constructor, Destructor, `AllocateTasks`, `GetType` |
| `CTaskComplexCarDriveMissionFleeScene`, `CTaskComplexCarDriveMissionKillPed`, `CTaskComplexClimb`, `CTaskComplexOpenDriverDoor` | Constructor, Destructor, `Clone`, `GetTaskType` |
| `CTaskComplexInAirAndLand` | Constructor, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexWanderGang`, `CTaskComplexWanderMedic` | Constructor, `Clone`, `GetWanderType`, `ScanForStuff` |

**Diferencias reales corregidas:**
- `CTask::IsTaskPtr`: el EXE recorre los slots ocupados de arriba abajo y solo acepta el inicio exacto de un slot; la DLL aceptaba punteros al interior de un slot.
- `CTask::operator delete` (alcance general: todas las tareas):
  - solo marca el slot libre y baja `m_LastFreeSlot` con comparación con signo (`CTaskPool::Delete` → `DeleteLegacy`);
  - sin relleno 0xDD;
  - la comprobación de nulo queda bajo `FIX_BUGS`.
- `RH_ScopedVirtualClass(CTask, …)`: la vtable tiene 7 entradas, no 6.
- `CTaskAllocatorKillOnFoot::AllocateTasks`:
  - también excluye `TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH`;
  - el global 0xC1A554 fuerza la rama de armas;
  - la duración de `LookAbout` está en [1000, 2000);
  - la segunda rama llama a `SetTask` directamente, no a `SetEventResponseTask`.
- Constructores de las misiones de coche (por las clases base):
  - `CTaskComplexCarDrive` escribe `m_bAsDriver = 1` y no toca los bytes 0x1D–0x1F;
  - `CTaskComplexCarDriveMission` guarda la misión como dword.
- `CTaskComplexInAirAndLand::CreateNextSubTask`: llama a `GetPadFromPlayer` antes de la comprobación de la animación, y lee izquierda/derecha antes de arriba/abajo.
- Constructores de los paseos (por `CTaskComplexWander`): solo escribe los `m_wAreaId` de los nodos y `flags = (flags & 0xF0) | (sensibly & 1)`.

**Método:**
- Peatones, vehículos, grupo, inteligencia, datos del jugador, pad, animaciones y subtareas falsos con vtables trazadas.
- Los constructores corren sobre búferes 0xCD.
- `operator new`/`delete` usan slots libres reales; el estado del pool se restaura en cada ejecución.
- Simulados y trazados:
  - listas de peatones y formaciones;
  - `IsTypeMelee`, `SetTask` y `SetEventResponseTask` (la tarea de pila se serializa al momento);
  - audio y pad;
  - `CEventGroup::Add`;
  - RNG.
- No cubierto:
  - `delete nullptr` (ruta `FIX_BUGS`);
  - pool de tareas lleno;
  - argumentos `bool` distintos de 0/1;
  - `AllocateTasks` con 0xC1A554 activo y sin objetivo (el EXE desreferencia nulo).

Repetir: `python reversing/CTasks19/build_probe.py` y `python reversing/run.py --case ctasks19`. Evidencia: [validation-summary.json](validation-summary.json).

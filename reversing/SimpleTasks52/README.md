<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas simples (lista de 52)

Validadas el 02/10/2026 con **11.904 pares original/candidato dentro de GTA**, todos exactos: 48 funciones de 16 clases con `validated=1`.

| Clases | Funciones |
|---|---|
| `CTaskSimpleHitByGunFromFront/Left/Rear/Right`, `CTaskSimpleHitFromBack/Behind/Front/Left/Right`, `CTaskSimpleHitWall`, `CTaskSimpleLookAbout` | Constructor, `Clone`, `IsInterruptable` |
| `CTaskSimplePutDownEntity` | Constructor, `Clone`, `GetTaskType` |
| `CTaskSimpleStandStill` | Constructor, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleTired` | Constructor, Destructor, `StartAnim` |
| `CTaskSimpleChat` | `Clone`, Destructor, `IsInterruptable` |
| `CTaskSimpleArrestPed` | `MakeAbortable`, `ProcessPed`, `StartAnim` |

**Diferencias reales corregidas:**
- `CTaskSimpleRunTimedAnim` (clase base de `LookAbout` y `Chat`, fuera de la lista):
  - el tipo de tarea es de 16 bits (la DLL escribía 32 y pisaba el relleno);
  - el constructor-1 pone `TASK_SIMPLE_TIMED_ANIM`;
  - sin inicializadores por valor que pongan a cero bytes que el EXE no escribe.
- `CTaskSimpleArrestPed::ProcessPed` usa la versión de `GetBonePosition` con parámetro de salida (0x5E4280), como el EXE.

**Método:**
- Constructores en un búfer rellenado con 0xCD (tipo en vez de vtable).
- Virtuales por la vtable del EXE.
- Pool de tareas con el slot libre restaurado y huecos rellenados con 0xCD.
- Peatones falsos con vtable trazada y clump.
- Se simulan y se trazan las animaciones, el estado de movimiento, el agacharse, `FindPlayerPed`, `SetPedState`, `Say`, las armas, `GetBonePosition`, la orientación y la puntería.
- En la ejecución original también se desactivan los hooks de los constructores base.

Repetir: `python reversing/SimpleTasks52/build_probe.py` y `python reversing/run.py --case stasks52`. Evidencia: [validation-summary.json](validation-summary.json).

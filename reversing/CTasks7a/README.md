<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas complejas A (lista de 7)

Validadas el 02/10/2026 con **13.120 pares original/candidato dentro de GTA**, todos exactos: 42 funciones de 6 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexChat` | Constructor, Destructor, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexDiveFromAttachedEntityAndGetUp` | Constructor, Destructor, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexDrivePointRoute` | Constructor, Destructor, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexGoToPointAndStandStill` | Constructor, `GoToPoint`, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexHitResponse` | Constructor, Destructor, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexInWater` | Constructor, Destructor, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |

**Diferencias reales corregidas:**
- `CTaskComplexChat`:
  - la vtable tiene 11 entradas, no 14 (en 0x8706CC empieza la de `CTaskComplexWalkAlongsidePed`);
  - `CreateFirstSubTask` llama a `Say` solo si el contexto es mayor que -1 (comparación con signo de 16 bits);
  - `ControlSubTask` busca la tarea del compañero con `CPedIntelligence::FindTaskByType` (0x600EE0, tareas primarias), no entre las activas.
- `CTaskComplexDiveFromAttachedEntityAndGetUp::CreateFirstSubTask`:
  - las direcciones salen de `ComputeEntityDirs` (0x5F1500) y, al esquivar un vehículo, se copia la dirección 1 o 3 de la entidad (no la normal del plano);
  - productos escalares en el orden del EXE (`(z·z + y·y) + x·x` y `(y·y + z·z) + x·x + d`) y planos con `ComputeEntityBoundingBoxPlanes` (0x5F3660);
  - el ángulo de la torreta es 2π exacto (0x40C90FDB).
- `CTaskComplexDrivePointRoute`:
  - `CreateNextSubTask` compara el índice ya incrementado con el número de puntos;
  - el constructor usa `CPointRoute::operator new` y copia solo los puntos usados (bucle con signo), como 0x63A040.
- `CTaskComplexGoToPointAndStandStill`:
  - el constructor no inicializa `m_bNewTarget` (el EXE conserva el bit 2 previo) y el radio mínimo es `c > r ? c : r` (un NaN se conserva) con comparación con signo del estado de movimiento;
  - `GoToPoint` también actualiza cuando el radio guardado es NaN (`fcomp st(0)`);
  - `CreateNextSubTask` devuelve `nullptr` con subtareas desconocidas.
- `CTaskComplexHitResponse::CreateFirstSubTask`: un lado fuera de 0..3 devuelve `nullptr`.
- `CTaskComplexInWater::CreateFirstSubTask`:
  - muere ahogado cuando `bDrownsInWater` **está** activo (la condición estaba invertida);
  - el líder se comprueba por `m_pPlayerData`, no por el tipo de peatón;
  - la tarea `GoToPoint` se busca con `FindTaskByType` (tareas primarias).

**Método:**
- 6 peatones con inteligencia propia, 2 vehículos con vtable falsa, grupo con líder, tarea de chat del compañero, ruta de puntos y tareas falsas (la falsa de `CTaskSimpleSwim` con entidad, punto de agarre, ángulo y superficie).
- Pools de tareas y de rutas de puntos restaurados en cada ejecución (slots libres con 0xCD); las rutas nuevas se serializan y se liberan, y la ruta del destructor sale del pool.
- `Say`, `GetPedTalking`, `DettachPedFromEntity`, `CInterestingEvents::Add`, `FindTaskByType`, `GetPedsGroup`, `IsSunbather`, las tres funciones de geometría de entidades y el RNG simulados con traza.
- Fuera de los fixtures: contextos 164, 167 y 168 en `CTaskComplexChat::CreateFirstSubTask` (FIX_BUGS amplía la tirada a 0..4); bytes altos del estilo de conducción de `CTaskComplexDrivePointRoute` (el DLL lo guarda en un byte más relleno a cero).

Repetir: `python reversing/CTasks7a/build_probe.py` y `python reversing/run.py --case ctasks7a`. Evidencia: [validation-summary.json](validation-summary.json).

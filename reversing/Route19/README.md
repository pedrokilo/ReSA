<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Seguir ruta de nodos (lista de 19)

Validadas el 03/10/2026 con **11.584 pares original/candidato dentro de GTA**, todos exactos: 19 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexFollowNodeRoute` | Constructor, Destructor, `Clone`, `GetTaskType`, `StopTimer`, `MakeAbortable`, `CreateNextSubTask`, `CreateFirstSubTask`, `ControlSubTask`, `GetSubTaskType`, `CalcGoToTaskType`, `CalcBlendRatio`, `CanGoStraightThere`, `SetTarget`, `CreateSubTask`, `GetLastWaypoint`, `GetNextWaypoint`, `ComputePathNodes`, `ComputeRoute` |

**Diferencias reales corregidas:**
- Constructor:
  - sin inicializadores por defecto: no se escriben los bits 1, 2 y 4 de las banderas, el relleno del temporizador, `m_CurrNode.m_wNodeId` ni los flotantes de mezcla sin mezcla;
  - `CPointRoute` se crea antes que `CNodeRoute`; solo se escribe el contador (y `m_wAreaId` de los nodos);
  - `m_StartNode` se copia de la dirección vacía global 0x8A5F44.
- `StopTimer` pausa el temporizador (`m_bStopped` y descuenta el tiempo) en vez de pararlo.
- `CreateFirstSubTask` y `CreateNextSubTask`: `CalcGoToTaskType` se usa **con** mezcla (estaba invertido); `CreateFirstSubTask` no toca las banderas.
- `CreateNextSubTask`: `TASK_SIMPLE_GO_TO_POINT_FINE` avanza como `GO_TO_POINT` (devolvía nulo); `GetTaskType` de la subtarea se llama como en el EXE.
- `SetTarget` con mezcla activa `m_bSpeedingUp` (se ponía a falso).
- `ControlSubTask`:
  - al frenar, ratio `<= 0` usa el ratio base;
  - al acelerar se mide desde el punto anterior y solo entonces se vuelve a `GO_TO_POINT`;
  - NaN sigue la ruta del EXE.
- `CalcGoToTaskType`:
  - direcciones en 2D (z = 0) e índice limitado con signo;
  - multiplicación por la constante 0x8700A4 y coseno 0.707;
  - límites con NaN como el EXE.
- `CalcBlendRatio`: suma z, y, x; `!(d² < r²)`; `fcos` del x87; máximo con el orden del EXE.
- `CanGoStraightThere`: compara la distancia (con `fsqrt`) y no el cuadrado.
- `ComputePathNodes`:
  - vacía la ruta al principio;
  - las distancias 2D son objetivo-nodo frente a objetivo-peatón;
  - en interiores sin interior del peatón la ruta va del objetivo al peatón y se invierte;
  - solo se comprueba `m_wAreaId` de los nodos.
- `ComputeRoute`: cada punto se añade solo si cabe (contador con signo).
- `MakeAbortable`: velocidad `< 0,015625` estricta y orden de llamadas del EXE.
- `CreateSubTask`: la tarea se crea antes del `Teleport`; tipos desconocidos devuelven nulo.
- `GetNextWaypoint`: comparación con signo.

**Método:**
- Peatón, inteligencia, vehículo, subtarea y evento falsos con vtables trazadas (`Teleport` en el hueco 14 del peatón).
- Rutas de nodos y puntos falsas con guardas; los punteros `ThePaths.m_pPathNodes[0..3]` apuntan a nodos falsos (o nulos) durante cada ejecución y se restauran; los datos reales no se tocan.
- Simulados y trazados: `FindGroundZFor3DCoord`, `CPathFind::ComputeRoute` y `These2NodesAreAdjacent`, el gestor de interiores, `IsWanderPathClear`, `IsEntityBlockingTarget`, `IsTemporaryEvent` y el RNG.
- Constructor, `Clone` y Destructor usan los pools reales de tareas, `CPointRoute` y `CNodeRoute` (último slot libre, IDs y slots libres con 0xCD restaurados por ejecución).
- Fuera de los fixtures:
  - nodos no cargados en el bucle de recorte sin interior (el EXE usa restos de la pila; la DLL los salta con `FIX_BUGS`);
  - `m_wAreaId == 0xFFFF` en rutas que recorre `ComputeRoute` (el EXE lee fuera del array de áreas);
  - subtarea nula en `MakeAbortable`/`ControlSubTask` (el EXE se cae).

Repetir: `python reversing/Route19/build_probe.py` y `python reversing/run.py --case route19`. Evidencia: [validation-summary.json](validation-summary.json).

<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8549 funciones reconstruidas; 8549 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9224 entradas binarias sin nombre catalogado; 820 destinos originales explícitos fuera del catálogo, 396 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 104 aprobadas dentro del juego; 131 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Grupo de eventos, rodear edificio y hierba (lista de 12)

Validadas el 02/10/2026 con **15.488 pares original/candidato dentro de GTA**, todos exactos: 36 funciones de 3 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CEventGroup` | Constructor, `Add`, `Flush`, `GetEventOfType`, `GetHighestPriorityEvent`, `HasEvent`, `HasEventOfType`, `HasScriptCommandOfTaskType`, `Remove`, `RemoveInvalidEvents`, `Reorganise`, `TickEvents` |
| `CTaskComplexWalkRoundBuildingAttempt` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `CreateSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `ControlSubTask`, `ComputeMoveDir`, `ComputeCrapRoute`, `ComputeRoute` |
| `CGrassRenderer` | `Initialise`, `Shutdown`, `AddTriPlant`, `DrawTriPlants`, `FlushTriPlantBuffer`, `GetPlantModelsTab`, `SetPlantModelsTab`, `SetCloseFarAlphaDist`, `SetCurrentScanCode`, `SetGlobalCameraPos`, `SetGlobalWindBending`, `GenPointInTriangle` |

**Diferencias reales corregidas:**
- `CEventGroup`:
  - `Add` sigue el orden de llamadas del EXE: `HasEditableResponse` se consulta dos veces, el tipo del evento solo se pide si no va a responder y otra vez antes de `InformRespectedFriends`, y la prioridad se pide antes que el tipo para `RecordEventForScript`;
  - `HasEventOfType` devuelve cierto con evento nulo y vuelve a pedir el tipo del evento buscado en cada iteración;
  - `Reorganise` pone a cero cada hueco no nulo antes de compactar (la DLL dejaba punteros repetidos tras el nuevo contador);
  - `HasScriptCommandOfTaskType` salta los huecos nulos (la DLL usaba `dyn_cast`, que hace `assert` con nulo).
- `CTaskComplexWalkRoundBuildingAttempt`:
  - `ComputeMoveDir` proyecta (destino − posición del **peatón**) en 2D sobre la normal, `(dy·ny + dx·nx) + nz·0`, y si `x² + y² < 1e-8` usa el vector derecho de la matriz del peatón (la DLL usaba `m_pos` y nunca normalizaba);
  - `ComputeRoute`:
    - vacía `m_route` al entrar y compara `++intentos >= 20` (la DLL comparaba `intentos + 1`);
    - el punto parte de `m_pos` (no del peatón), la primera línea de visión no ignora objetos (último parámetro 0) y la aleatoriedad es `((rand·1/32767)·0,4 − 0,2) + intentos`;
    - solo cuenta los intentos con la vista bloqueada para la regla de los 20 intentos (la DLL contaba todas las vueltas);
    - vacía la ruta mala antes de rellenarla y la copia con 0x63A040 (contador y solo los puntos usados);
  - `ComputeCrapRoute` siempre gasta los dos `rand` y la línea de visión aunque la ruta esté llena, suma `normal·[1, 2) + derecha·[-1, 1)` antes de la posición, y con choque a menos de 0,35 (o NaN) sale sin marcar la ruta;
  - `ControlSubTask` usa `(matriz · offset) − destino` con `(z² + y²) + x² > 16` (la DLL transformaba `offset − destino` y comparaba `>=`);
  - `CreateSubTask` calcula el rumbo con `Normalise` + `GetRadianAngleBetweenPoints(x, y, 0, 0)` + `LimitRadianAngle` y los tipos desconocidos devuelven `nullptr`;
  - `CreateNextSubTask` no limpia el flag del peatón con `FOLLOW_POINT_ROUTE` y los tipos desconocidos lo limpian y devuelven `nullptr`;
  - constructores: las rutas solo escriben el contador (0x41B7C0), `m_offset`/`m_targetPos` y los bits 6-7 de 0x49 no se tocan; el destructor borra `m_route` antes que `m_crapRoute`; matriz · punto con el orden de sumas de 0x59C670.
- `CGrassRenderer`:
  - `DrawTriPlants` transcrito del EXE: `srand` + `rand()` en vez de `std::mt19937`, distancias `sqrt((z² + y²) + x²)`, la calidad se consulta en cada planta (`>= 2` usa la distancia completa), el fundido es `((lejos + 20 − dist)·a)·0,05` con `floor` en double y recorte a [0, 255], los tres aleatorios se consumen aunque la planta no se dibuje, y el contador es con signo;
  - `AddTriPlant` compara el índice con signo; `SetPlantModelsTab` devuelve si el índice era válido (`al`).

**Método:**
- Grupo de eventos sobre 20 eventos falsos con vtable trazada; las respuestas editables y `RecordEventForScript` se simulan y trazan.
- La tarea corre sobre peatón, entidad, subtarea y rutas falsos; las líneas de visión, `CPointRoute::operator new/delete` y los ángulos de `CGeneral` se simulan; las subtareas nuevas salen del pool real y se comparan palabra a palabra.
- La hierba dibuja sobre atómicos, frames y geometrías falsos; `RwFrameTranslate`, `RwMatrixUpdate`, `RpGeometryForAllMaterials`, el callback de render y `CPPTriPlantBuffer::Flush` se simulan.
- Se guardan y restauran los globales de la hierba, `gTriPlantBuf`, la calidad de `g_fx`, el pool de tareas y la lista libre de referencias.
- Fuera de los fixtures:
  - huecos nulos donde el EXE se cae (`GetEventOfType`, `HasEventOfType`, `GetHighestPriorityEvent`, `TickEvents`, `Flush(true)`);
  - semillas distintas de 0,0 en `DrawTriPlants` (FIX_BUGS siembra con los bits de la semilla);
  - matrices nulas de peatón y entidad, rutas de más de 8 puntos y NaN en la hierba;
  - el valor de retorno de `GenPointInTriangle` (el EXE deja `v1` en eax).

Repetir: `python reversing/Mix12/build_probe.py` y `python reversing/run.py --case mix12`. Evidencia: [validation-summary.json](validation-summary.json).

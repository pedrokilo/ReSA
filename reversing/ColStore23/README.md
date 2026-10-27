<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8939 funciones reconstruidas; 8939 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Almacén de colisiones (lista de 23)

Validadas el 03/10/2026 con **10.752 pares original/candidato dentro de GTA**, todos exactos: 23 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CColStore` | `Initialise`, `Shutdown`, `AddColSlot`, `RemoveColSlot`, `AddRef`, `RemoveRef`, `AddCollisionNeededAtPosn`, `IncludeModelIndex`, `GetBoundingBox`, `LoadCol` (fichero y buffer), `RemoveCol`, `LoadAllCollision`, `LoadAllBoundingBoxes`, `RemoveAllCollision`, `BoundingBoxesPostProcess`, `SetCollisionRequired`, `SetIfCollisionIsRequired`, `SetIfCollisionIsRequiredReducedBB`, `RequestCollision`, `HasCollisionLoaded`, `EnsureCollisionIsInMemory`, `LoadCollision` |

**Diferencias reales corregidas:**
- El código de área del jugador y de las entidades de la limpieza de misión se lee sin signo (`movzx`), no con signo como `m_AreaCode` (afecta a `SetCollisionRequired`, `EnsureCollisionIsInMemory` y `LoadCollision`).
- `LoadCollision`:
  - el contador de referencias se compara con signo (`cmp word, 0` / `jg`): 0x8000–0xFFFF no cuentan;
  - la velocidad del vehículo solo se suma a X e Y.
- `EnsureCollisionIsInMemory` llama dos veces a `FindPlayerPed` y pasa `AREA_CODE_NONE` a `SetCollisionRequired`, que vuelve a buscar al jugador.
- `AddColSlot`:
  - toma el slot con `CPool::New` sin construir el `ColDef` y no guarda el nombre (la DLL lo copiaba y ponía a cero el resto);
  - las comparaciones de nombres son `_stricmp`/`_strnicmp` en el orden del EXE. `FindColSlot(nombre)` (NOTSA) devuelve -1, como `FindColSlot()` del EXE.
- `Initialise` y `Shutdown` construyen y destruyen el pool como el original (0x410F80 y en línea): sin relleno 0xFD del almacenamiento, sin escribir el byte +0x11 y con primer libre 0 al destruir.
- `RemoveCol` recorre los modelos con un índice de 32 bits (con `int16`, un final de 32767 no terminaba).
- Los bucles recorren 255 slots fijos, como el EXE.

**Método:**
- Pool falso de 255 `ColDef` con campos finitos y ~1/3 de slots ocupados; árbol cuaternario real (nodos del pool real de nodos y de `CPtrNodeSingleLink`, con `m_LastFreeSlot` restaurado en cada ejecución) que se vuelca y se libera tras cada caso.
- Se sustituyen y restauran los punteros del pool y del árbol, los pools de peatones y vehículos (pools falsos para la limpieza de misión), una ventana de 40 punteros de `ms_modelInfoPtrs` con model infos y modelos de colisión falsos, las entradas de streaming de los COL, la limpieza de misión, el área actual y los globales de `CColStore`.
- Stubs trazados (sin reservas) para:
  - `FindPlayerPed`, `FindPlayerVehicle`, el streaming y `CColAccel`;
  - la carga de ficheros COL de `CFileLoader` y `RemoveCollisionVolumes` (nunca se carga un COL real);
  - `GetScreenFadeStatus`, `MessageScreen` y `CTimer::Suspend/Resume`;
  - `operator new/delete` con un asignador lineal, solo en `Initialise` y `Shutdown`.
- Posiciones cerca de los bordes de las cajas (márgenes de 80 y 110) y NaN en las dos funciones de retrollamada.

Repetir: `python reversing/ColStore23/build_probe.py` y `python reversing/run.py --case colstore23`. Evidencia: [validation-summary.json](validation-summary.json).

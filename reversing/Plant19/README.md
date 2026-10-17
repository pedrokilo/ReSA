<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Gestor de plantas (lista de 19)

Validadas el 03/10/2026 con **10.688 pares original/candidato dentro de GTA**, todos exactos: 19 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CPlantMgr` | `Initialise`, `Shutdown`, `ReloadConfig`, `Update`, `PreUpdateOnceForNewCameraPos`, `Render`, `UpdateAmbientColor`, `CalculateWindBending`, `SetPlantFriendlyFlagInAtomicMI`, `MoveLocTriToList`, `MoveColEntToList`, `_ColEntityCache_Add`, `_ColEntityCache_FindInCache`, `_ColEntityCache_Remove`, `_ColEntityCache_Update`, `_ProcessEntryCollisionDataSections`, `_ProcessEntryCollisionDataSections_AddLocTris`, `_ProcessEntryCollisionDataSections_RemoveLocTris`, `_UpdateLocTris` |

**Diferencias reales corregidas:**
- `UpdateAmbientColor`: `64 - _ftol(c * 2,5 * -255)` y mínimo con 255 comparando 16 bits (la DLL usaba `max`, siempre 255).
- `CalculateWindBending`:
  - la semilla es un dword;
  - viento NaN entra en la rama de ráfagas;
  - el orden de sumas es `((1 - f) * o1 + f * o2) + 1`;
  - `fsin` de precisión completa multiplicado por 0,008/0,005 en vez de dividir por 125/200.
- `Initialise`: `grassModelsPtr` guarda punteros a las filas de `PC_PlantModelsTab`, no el primer atomic.
- `Shutdown`: atomics y texturas se destruyen por columnas, como el EXE.
- `Render`: la mezcla día/noche se calcula como el EXE (`(día/30·(1-b) + noche/30·b)·255`, canales `(c·m) >> 8`) y la visibilidad del espejo ya no se consulta dos veces.
- Valores de retorno que el EXE deja en `al`/`eax`: `SetPlantFriendlyFlagInAtomicMI`, `Update`, `PreUpdateOnceForNewCameraPos`, `_UpdateLocTris`, `_ColEntityCache_Update`, `_ProcessEntryCollisionDataSections` (bool) y `_ColEntityCache_Add` (entrada).
- `SetPlantFriendlyFlagInAtomicMI` y `_ProcessEntryCollisionDataSections`: número de triángulos con signo.
- `Update`: contadores de un byte sin signo (`& 0x1F`, `& 7`).
- `_ColEntityCache_Update`:
  - marca el código de búsqueda de cada edificio (la DLL nunca lo hacía);
  - sigue aunque `AddEntry` falle y solo se corta si se vacía la lista libre;
  - recorre los sectores como el EXE.
- `_ProcessEntryCollisionDataSections`: grupos de caras en el orden del EXE (hacia atrás) y con índices con signo.
- `_AddLocTris` y `_RemoveLocTris`:
  - distancias como `centro - punto`, puntos medios como media de diferencias y sumas `z² + y² + x²` (`z² + x² + y²` en el primer vértice de `Remove`);
  - `Remove` también mira el centro del triángulo;
  - transformación a un búfer distinto y máscara `i & 7`.
- `_CalcDistanceSqrToEntity`: orden de sumas `z² + y² + x²`.

**Método:**
- Entradas, triángulos, entidades, modelos y datos de colisión, model infos y listas de edificios de los sectores son falsos.
- RW, `CTxdStore`, `CStreaming`, `LoadModels`, `CGrassRenderer`, `CPlantLocTri::Add/Release`, `CPlantColEntEntry::AddEntry/ReleaseEntry`, superficies, `TestSphereBox`, la cámara y `rand` se simulan con trazas.
- `Update`, `PreUpdateOnceForNewCameraPos`, `_UpdateLocTris` y `_ProcessEntryCollisionDataSections` simulan las funciones del grupo que llaman.
- Se guardan y restauran los globales y tablas de `CPlantMgr`, los sectores, los model infos usados, el viento, el reloj, el código de búsqueda, el área, el espejo, la calidad de efectos y el balance día/noche.
- Fuera de los fixtures: `LoadModels` real (carga ficheros) y triángulos con índices negativos.

Repetir: `python reversing/Plant19/build_probe.py` y `python reversing/run.py --case plant19`. Evidencia: [validation-summary.json](validation-summary.json).

<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Coberturas y CPlaceable (lista de 16)

Validadas el 03/10/2026 con **19.680 pares original/candidato dentro de GTA**, todos exactos: 32 funciones de 2 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CCover` | `Init`, `Update`, `AddCoverPoint`, `RemoveCoverPointIfEntityLost`, `RemoveCoverPointsForThisEntity`, `ShouldThisBuildingHaveItsCoverPointsCreated`, `CalculateHorizontalSize`, `DoLineCheckWithinObject`, `DoesCoverPointStillProvideCover`, `Find2HighestPoints`, `FindAndReserveCoverPoint`, `FindCoordinatesCoverPoint`, `FindCoverPointsForThisBuilding`, `FindDirFromVector`, `FindVectorFromDir`, `FindVectorFromFirstToMissingVertex` |
| `CPlaceable` | `SetPosn` (xyz y vector), `SetOrientation`, `SetHeading`, `GetHeading`, `GetRoll`, `IsWithinArea` (xy y xyz), `RemoveMatrix`, `AllocateStaticMatrix`, `AllocateMatrix`, `SetMatrix`, `GetMatrix`, `InitMatrixArray`, `ShutdownMatrixArray`, `FreeStaticMatrix` |

**Diferencias reales corregidas:**
- `CCover::Init` y la limpieza de puntos (`RemoveCoverPointIfEntityLost`, `Update`) solo ponen el tipo a 0 y descuentan el contador (con signo); no limpian referencias ni el resto del punto.
- `CCover::RemoveCoverPointsForThisEntity`: `CleanUpOldReference` solo si la entidad no es nula, y después tipo y entidad a 0.
- `CCover::ShouldThisBuildingHaveItsCoverPointsCreated`: distancia real `√((z²+y²)+x²)` frente a radio + 30 (NaN no pasa).
- `CCover::Update`:
  - compara el byte del modo de replay;
  - objetos: `up.z < 0,9` con `GetMatrix`; vehículos: `|v| > 0,01`;
  - vehículos y objetos se recorren de la última ranura a la primera, con `|v| < 0,005`, `up.z > 0,95` (matriz sin comprobar) y distancia 2D `< 25`;
  - edificios: rectángulo de ±30 con cuatro `FindPlayerCoors`, sectores con `floor` y `_ftol2` (64 bits), y como mucho un edificio nuevo por llamada.
- `CCover::AddCoverPoint`:
  - contador con signo;
  - la posición de comparación es la dada o la de la entidad (`√((z²+y²)+x²)` contra 0,8 y 2,0);
  - la diferencia de dirección se hace en bytes (`< 32` o `> 223`);
  - el hueco libre se busca sin límite y la referencia no se registra para edificios sin IPL.
- `CCover::CalculateHorizontalSize`, `DoLineCheckWithinObject`, `Find2HighestPoints`: estaban sin reconstruir (`NOTSA_UNREACHABLE`); ahora siguen las comparaciones del EXE ante NaN, `CrossProduct` (0x59C730) y el orden de sumas.
- `CCover::DoesCoverPointStillProvideCover`: `(pos − punto)·FindVectorFromDir` sumando z, y, x.
- `CCover::FindAndReserveCoverPoint`:
  - elige el punto **más cercano** en 2D (< 15, |Δz| < 4) en vez del más lejano, y ya no exige que el peatón estuviera reservado;
  - el producto con la dirección usa el seno de precisión completa del x87 (asm);
  - para atacar exige distancia punto–objetivo < peatón–objetivo + 4.
- `CCover::FindCoordinatesCoverPoint`: transcrito del EXE (vehículo: el más cercano de los dos puntos de `FindCoverPointsBehindBox`; objeto: `Normalise` 0x59C910 y radio `max(bbMax.x, bbMax.y) + 0,4`).
- `CCover::FindCoverPointsForThisBuilding`: `Multiply3x3`/`CMatrix*CVector` con el orden de sumas del EXE, dirección con `fpatan` + `_ftol2` y uso como byte del efecto.
- `CCover::FindDirFromVector` devuelve el entero completo de `_ftol2`; `FindVectorFromFirstToMissingVertex` compara índices `int32` completos.
- `CPlaceable::GetHeading` y `GetRoll` devuelven ST0 sin redondear (`fpatan`); `GetRoll` usa la magnitud `√(x²+y²)` (no la magnitud al cuadrado) y solo invierte el signo si `up.z < 0`.
- `CPlaceable::RemoveMatrix` y `SetMatrix`: el heading sale de `fpatan` (no del `atan2` del CRT).

**Método:**
- Los puntos de cobertura, la lista de edificios procesados, `CReferences`, `gMatrixList`, los punteros de los pools de vehículos, objetos y edificios, el pool real de nodos dobles (cabecera, estados y almacenamiento), un bloque de 8×8 sectores, el código de búsqueda, el contador de fotogramas, el modo de replay y la model info 1 se guardan y se restauran en cada ejecución.
- La lista de edificios procesados usa nodos pedidos al pool real y sus edificios están en un pool de edificios falso (la DLL valida cada nodo y su `Item` contra esos pools). Entidades, pools de vehículos y objetos, nodos de sector, colisión, efectos 2D, `CMatrixLink` y `CPlaceable` son falsos; las listas falsas de matrices viven dentro de los centinelas de `gMatrixList`.
- Se simulan y trazan `FindPlayerCoors`, `FindPlayerVehicle`, `CObject::CanBeUsedToTakeCoverBehind`, `CBaseModelInfo::Get2dEffect` y `operator new/delete` (solo en `InitMatrixArray`/`ShutdownMatrixArray`).
- Los retornos en ST0 se comparan con sus 80 bits.
- Fuera de los fixtures (desviaciones `FIX_BUGS` o lecturas sin inicializar del EXE):
  - `AddCoverPoint` con `FIX_BUGS` solo mide la distancia a puntos de tipo 3: los demás puntos se colocan lejos;
  - `AddCoverPoint` sin posición ni entidad, o de tipo 3 sin posición;
  - segundo peatón en puntos de vehículo (`CanAccommodateAnotherPed` con `FIX_BUGS`);
  - la referencia `FIX_BUGS` que `Update` registra en el nodo de la lista de procesados (no entra en la comparación);
  - tipos de punto fuera de 0..3 en `FindAndReserveCoverPoint`.

Repetir: `python reversing/World16/build_probe.py` y `python reversing/run.py --case world16`. Evidencia: [validation-summary.json](validation-summary.json).

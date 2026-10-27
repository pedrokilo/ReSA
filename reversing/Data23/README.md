<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Audio, balas, modelos, garajes y datos (lista de 23)

Validadas el 02/10/2026 con **13.904 pares original/candidato dentro de GTA**, todos exactos: 32 funciones de 8 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAEPedWeaponAudioEntity` | `AddAudioEvent`, `Initialise`, `Service`, `Terminate` |
| `CBulletInfo` | `AddBullet`, `Initialise`, `Shutdown`, `Update` |
| `CDamageAtomicModelInfo` | `AsDamageAtomicModelInfoPtr`, `DeleteRwObject`, `Init`, `SetDamagedAtomic` |
| `CGarage` | `BuildRotatedDoorMatrix`, `CloseThisGarage`, `OpenThisGarage`, `StoreAndRemoveCarsForThisHideOut` |
| `CPedStats` | `GetPedStatType`, `Initialise`, `LoadPedStats`, `Shutdown` |
| `CPlantSurfPropMgr` | `AllocSurfProperties`, `GetSurfProperties`, `Initialise`, `LoadPlantsDat` |
| `CScriptResourceManager` | `AddToResourceManager`, `HasResourceBeenRequested`, `Initialise`, `RemoveFromResourceManager` |
| `CSpecialPlateHandler` | `Add`, `Find`, `Init`, `Remove` |

**Diferencias reales corregidas:**
- `CAEPedWeaponAudioEntity`:
  - `AddAudioEvent` no tiene la salida temprana con `WEAPON_UNARMED`;
  - `Service` comprueba `bInVehicle` (bit 8 de 0x46C), no `bCollidedWithMyVehicle`.
- `CBulletInfo`:
  - `AddBullet` devuelve `bool` y el tiempo de destrucción se convierte con signo.
  - `Update` tenía nueve diferencias:
    - un creador inválido se pone a nulo;
    - se comprueba que la bala existe antes de ver si caducó;
    - la bala caducada aún se procesa ese frame;
    - la comparación de tiempo es exacta;
    - `hitEntity` vive fuera del bucle;
    - la animación de muerte marca la bandera de reproducción;
    - `IsSphereVisible` recibe el punto de impacto;
    - un multiplicador de daño NaN se trata como el EXE;
    - el ángulo usa el `asin` x87 del EXE con la constante 0x42652EE0.
  - El índice de modelo del último edificio disparado se extiende con signo (`movsx`).
- `CGarage`:
  - `BuildRotatedDoorMatrix` usa `fsin`/`fcos` x87 y la constante exacta -π/2 (0xBFC90FDB; el `HALF_PI` de `common.h` es impreciso);
  - `StoreAndRemoveCarsForThisHideOut` compara el índice con signo.
- `CPedStats`: `Initialise` no pone a cero los nombres (`new[]` sin `()`); `LoadPedStats` copia el nombre con `strcpy`.
- `CPlantSurfPropMgr`: `Initialise` escribe campo por campo sin tocar el relleno; `LoadPlantsDat` solo prueba los 16 bits bajos del id de superficie.
- `CSpecialPlateHandler`: `Find` devuelve el índice (o -1) en EAX; `strcpy` en vez de `strcpy_s`; `Remove` compara con signo.

**Método:**
- Entidades falsas.
- Todas las dependencias simuladas y trazadas en la dirección del EXE y en el símbolo de la DLL.
- Todo el estado global tocado se restaura por ejecución.
- Fuera de los fixtures: nombres de más de 24 caracteres, ids de superficie ≥ 178, más de 15 matrículas, frames nulos en `DeleteRwObject` y floats leídos que no son múltiplos de 0,25.

- Arranque: el banco escribe `case_start` (función, caso, semilla) antes de cada caso. Al cargar la DLL (solo con `data23-runtime.start`) anula el `call SetDir` de `CAETrackLoader::GetDataStream` (0x4E0E3D): el hilo de streaming de audio cambiaba el directorio de trabajo global durante `CTheScripts::Init` y `fopen("main.scm")` devolvía NULL (0xC0000409 antes del primer caso). Un parámetro inválido del CRT en el arranque queda registrado como `boot_invalid_parameter` (RVA en el ASI).

Repetir: `python reversing/Data23/build_probe.py` y `python reversing/run.py --case data23`. Evidencia: [validation-summary.json](validation-summary.json).

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

Repetir: `python reversing/Data23/build_probe.py` y `python reversing/run.py --case data23`. Evidencia: [validation-summary.json](validation-summary.json).

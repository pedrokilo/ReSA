<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Eventos (lista de 70)

Validadas el 02/10/2026 con **13.120 pares original/candidato dentro de GTA**, todos exactos: 42 funciones de 21 clases de eventos con `validated=1`.

Cada clase tiene su constructor y una virtual (`AffectsPed`, `AffectsPedGroup`, `IsValid` o `CloneEditable`):

| Clases | Virtual |
|---|---|
| `AcquaintancePedHateBadlyLit`, `CarUpsideDown`, `CopCarBeingStolen`, `GunShotWhizzedBy`, `HitByWaterCannon`, `InAir`, `ObjectCollision`, `PotentialWalkIntoFire`, `PotentialWalkIntoObject`, `PotentialWalkIntoVehicle`, `SeenPanickedPed`, `VehicleToSteal` | `AffectsPed` |
| `LeaderEnteredCarAsDriver`, `LeaderExitedCarAsDriver`, `LeaderQuitEnteringCarAsDriver`, `PlayerCommandToGroup`, `PlayerCommandToGroupAttack` | `AffectsPedGroup` |
| `LeanOnVehicle`, `PassObject` | `IsValid` |
| `VehicleDamageCollision`, `VehicleDamageWeapon` | `CloneEditable` |

**Diferencias reales corregidas:**
- `AcquaintancePedHateBadlyLit::AffectsPed`:
  - diferencia de tiempo con signo;
  - suma de cuadrados z²+y²+x²;
  - NaN cuenta como "lejos".
- `GunShotWhizzedBy::AffectsPed`: el original usa un producto escalar, no la suma de los cuadrados de los productos.
- `LeanOnVehicle::IsValid` se llamaba a sí misma sin fin con un peatón nulo; ahora usa `CEvent::IsValid`, como el EXE.
- `PlayerCommandToGroup::AffectsPedGroup` (y `Attack`, que la llama): sin líder, o si el líder no es el jugador, devuelve falso.
- `PotentialWalkIntoFire`: con tamaño de fuego NaN el radio es 1,5.
- `PotentialWalkIntoObject::AffectsPed`:
  - los tamaños NaN pasan;
  - la aserción de `TASK_COMPLEX_AVOID_ENTITY` se sustituye por la comprobación real del EXE.
- `PotentialWalkIntoVehicle::AffectsPed`:
  - con `TASK_COMPLEX_WALK_ROUND_CAR` activa devuelve siempre falso;
  - `SetNewVehicle` recibe los bits 2-5 con signo del byte +0xD.

**Método:**
- Peatones, vehículos, objeto, inteligencias, tareas y eventos falsos, y un grupo de peatones falso.
- Model info falsa colocada temporalmente en `ms_modelInfoPtrs[1]`.
- Constructores en un búfer rellenado con 0xCD, comparado entero (tipo en vez de vtable).
- `CloneEditable` se compara por el evento creado en el pool de eventos, que se restaura en cada ejecución.
- Se simulan y se trazan `AreInSameGroup`, `IsInAir`, `ComputeEntityHitSide`, `FindPlayerPed`, `IsPlayerOnAMission`, `ComputeClosestSurfacePoint`, `GetTargetPos` y `SetNewVehicle`.

Repetir: `python reversing/Events70/build_probe.py` y `python reversing/run.py --case events70`. Evidencia: [validation-summary.json](validation-summary.json).

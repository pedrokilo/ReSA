<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7478 con evidencia histórica, 0 con vigencia acreditada y 968 pendientes en 17 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Gestor de fuegos (lista de 24)

Validadas el 03/10/2026 con **9.344 pares original/candidato dentro de GTA**, todos exactos: 24 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CFireManager` | `ClearAllScriptFireFlags`, Constructor, `CreateAllFxSystems`, `DestroyAllFxSystems`, Destructor, `ExtinguishPoint`, `ExtinguishPointWithWater`, `FindNearestFire`, `GetNextFreeFire`, `GetNumFiresInArea`, `GetNumFiresInRange`, `GetNumOfNonScriptFires`, `GetScriptFireCoords`, `Init`, `IsScriptFireExtinguished`, `PlentyFiresAvailable`, `RemoveAllScriptFires`, `RemoveScriptFire`, `SetScriptFireAudio`, `Shutdown`, `StartFire` (con objetivo y sin objetivo), `StartScriptFire`, `Update` |

**Diferencias reales corregidas:**
- `ExtinguishPoint` / `ExtinguishPointWithWater`: solo actúan si r² > d² estrictamente (igualdad y NaN no apagan).
- `ExtinguishPointWithWater` (`CFire::ExtinguishWithWater` en línea):
  - fuerza − agua × 0,02 × paso (sin `GetTimeStepInSeconds`);
  - partículas con `rand() & 0xFF`: x e y centradas (−128) × 0,01, z sin centrar × 0,005;
  - fuerza NaN no apaga; parte entera con `_ftol2` (32 bits bajos).
- `IsScriptFireExtinguished`, `RemoveScriptFire`, `SetScriptFireAudio`, `GetScriptFireCoords`: acceso directo con índice con signo (la DLL afirmaba que el fuego estaba activo); `RemoveScriptFire` recibe `int16`.
- `GetNumFiresInArea`: comparación eje por eje sin `CBoundingBox`.
- `DestroyAllFxSystems` usa `FxManager_c::DestroyFxSystem` (no `Kill`); `CreateAllFxSystems` solo crea si el fuego no tiene efecto.
- `GetNextFreeFire`: recicla el primer fuego que no sea a la vez de script y de primera generación (la condición estaba invertida).
- `StartFire` sin objetivo:
  - tiempo = (rand × 1/32767 × 0,29999995 + 1) × tiempo + ahora en x87 (enteros sin signo) y `_ftol2`;
  - generaciones: mínimo con signo contra el máximo del `gFireManager` global.
- `StartFire` con objetivo:
  - devuelve el fuego (la DLL devolvía siempre `nullptr`);
  - `m_pFire` del peatón antes que `IsPedInControl`;
  - `ReportCrime` solo con autor; llamada a `FindPlayerPed` descartada;
  - aleatorio entero de 0x407180; generaciones con signo.
- `StartScriptFire`: objetivos que no son peatón ni vehículo no crean efecto; generaciones con signo.
- `Update`:
  - disturbios: `fsin`/`fcos` x87, dirección punto − cámara, frente de la cámara sin matriz desde el heading, solo con suelo, entidad con el bit 0 de +0x1E y z > 0,5, comparación con el double 0,2;
  - grupos: el más fuerte parte de −1,0; `ceil` del CRT con `_ftol2`;
  - color base del EXE (64, 50, 32), multiplicador 0,39999998;
  - sombra con frontX = dir × 1,2 y sideY = dir × −1,2 (estaban intercambiados);
  - coronas: posición calculada (antes leía `point` sin inicializar), ids id..id+3 y derecha de `m_mCameraMatrix`.

**Método:**
- `CFireManager` falso con 60 fuegos y un fuego de relleno delante (índice −1); entidades falsas con desplazamientos crudos.
- Se simulan, en la dirección del EXE y en el símbolo de la DLL:
  - `rand`, `FindPlayerPed`, el gestor de efectos y sus sistemas;
  - referencias de entidades, nivel del agua, `IsPedInControl`, `IsPlayer`, `ReportCrime`, `GetNewUniqueScriptThingIndex`;
  - `CFire::ProcessFire`, disturbios, `FindGroundZFor3DCoord`, sombras estáticas y coronas.
- Cámara (matriz, placement, `m_mCameraMatrix`), reloj, `CReplay::Mode`, `g_fx` y `gFireManager` se guardan y se restauran.

Repetir: `python reversing/Fire24/build_probe.py` y `python reversing/run.py --case fire24`. Evidencia: [validation-summary.json](validation-summary.json).

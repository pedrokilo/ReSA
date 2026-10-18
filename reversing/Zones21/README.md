<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Zonas y zonas de culling (lista de 21)

Validadas el 03/10/2026 con **14.352 pares original/candidato dentro de GTA**, todos exactos: 42 funciones de 2 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CCullZones` | `Init`, `AddCullZone`, `AddTunnelAttributeZone`, `AddMirrorAttributeZone`, `FindAttributesForCoors`, `FindTunnelAttributesForCoors`, `FindMirrorAttributesForCoors`, `FindZoneWithStairsAttributeForPlayer`, `Update` y los 12 consultores de flags (`CamCloseInForPlayer`, `CamStairsForPlayer`, `Cam1stPersonForPlayer`, `NoPolice`, `PoliceAbandonCars`, `InRoomForAudio`, `FewerCars`, `FewerPeds`, `DoINeedToLoadCollision`, `CamNoRain`, `PlayerNoRain`, `DoExtraAirResistanceForPlayer`) |
| `CTheZones` | `Init`, `InitZonesPopulationSettings`, `ResetZonesRevealed`, `AssignZoneInfoForThisZone`, `CreateZone`, `PostZoneCreation`, `PointLiesWithinZone`, `ZoneIsEntirelyContainedWithinOtherZone`, `GetLevelFromPosition`, `FindSmallestZoneForPosition`, `GetZoneInfo`, `GetNavigationZone`, `GetMapZone`, `Calc2DDistanceBetween2Zones`, `FillZonesWithGangColours`, `FindZone`, `FindZoneByLabel`, `SetZoneRadarColours`, `Update`, `Save`, `Load` |

**Diferencias reales corregidas:**
- `CCullZones`:
  - `FindAttributesForCoors`/`FindTunnelAttributesForCoors` recorrían el array entero (1300/40 zonas) en vez de las `Num*` primeras y devolvían 16 bits; el EXE devuelve un entero de 32 bits con los flags extendidos con signo (`movsx`);
  - `FindMirrorAttributesForCoors` y `FindZoneWithStairsAttributeForPlayer` también recorrían el array entero; la segunda pide la posición del jugador una sola vez, antes del bucle;
  - `CZoneDef::IsPointWithin` (0x72D850): NaN falla todas las pruebas y las longitudes al cuadrado son enteros de 32 bits comparados exactos con el producto escalar;
  - `Add*Zone`: truncado con `_ftol2` de 64 bits antes de guardar la palabra/byte bajo;
  - `Update` llama dos veces a `FindPlayerPed` como el EXE.
- `CTheZones`:
  - `Init` no llama a `CreateZone`: no escribe el byte de nivel de las zonas vacías, deja las zonas de mapa con tipo 3 y escribe el nivel actual como dword;
  - `InitZonesPopulationSettings` no toca el color ni los 4 bits altos del byte de razas;
  - `AssignZoneInfoForThisZone`, `FindZone` y `FindZoneByLabel` comparan las 8 bytes de la etiqueta en bruto (bytes tras el NUL incluidos); `FindZoneByLabel` devuelve -1 si el tipo no es INFO (había un `assert`);
  - `CreateZone` ordena las coordenadas en float antes de truncarlas, pasa a mayúsculas el texto de origen (solo a-z), no limpia los bytes tras el NUL, ignora los tipos 2 y >3, guarda el tipo y llama a `AssignZoneInfoForThisZone` con el índice antes de incrementar el total;
  - `FindSmallestZoneForPosition` compara el tamaño sin signo y `GetLevelFromPosition` usa `PointLiesWithinZone` y devuelve 32 bits;
  - `FillZonesWithGangColours`: alfa 0 sin bandas y `min(suma*3, 120)` con mínimo 55 (antes 55 siempre y con truncado a 8 bits);
  - `SetZoneRadarColours` no escribe el alfa; índices sin signo en `GetZoneInfo`, `GetNavigationZone`, `GetMapZone`;
  - `Update` usa las variables estáticas del EXE, llama a 0x572800 y reinicia el contador después (se quitó el `FIX_BUGS` de `GetZoneWasVisited` de su camino);
  - `Save`/`Load` devuelven `true`, guardan desde las propias globales (nivel como dword) y recorren con contadores de 16 bits sin signo.

**Método:**
- Fixtures sobre las tablas reales del EXE (zonas de atributos, túneles y espejos; zonas de navegación y de mapa, infos, zonas visitadas), que se guardan y se restauran antes y después de cada ejecución y al final, junto con el contador de fotogramas, la hora, el área y `bPlayerIsOffTheMap`.
- Se simulan y trazan `FindPlayerCoors`, `FindPlayerPed`, `CPed::IsAlive`, `CPlayerPed::SetWantedLevelNoDrop`, `CCamera::GetGameCamPosition`, `CGangWars::CanPlayerStartAGangWarHere` y los buffers de guardado/carga (sin partidas reales).
- Fuera de los fixtures:
  - `AssignZoneInfoForThisZone` con índice menor que el total y sin coincidencia previa (el EXE no termina);
  - etiquetas de más de 7 caracteres en `FindZoneByLabel` (el EXE desborda la pila);
  - coordenadas NaN o fuera del rango de `int64` en los `Add*Zone`/`CreateZone`.

Repetir: `python reversing/Zones21/build_probe.py` y `python reversing/run.py --case zones21`. Evidencia: [validation-summary.json](validation-summary.json).

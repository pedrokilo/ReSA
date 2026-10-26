<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Lógica de juego (lista de 27)

Validadas el 04/10/2026 con **13.888 pares original/candidato dentro de GTA**, todos exactos: 27 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CGameLogic` | `CalcDistanceToForbiddenTrainCrossing`, `ClearSkip`, `DoWeaponStuffAtStartOf2PlayerGame`, `FindCityClosestToPoint`, `ForceDeathRestart`, `InitAtStartOfGame`, `IsCoopGameGoingOn`, `IsPlayerAllowedToGoInThisDirection`, `IsPlayerUse2PlayerControls`, `IsPointWithinLineArea`, `IsSkipWaitingForScriptToFadeIn`, `LaRiotsActiveHere`, `Load`, `PassTime`, `Remove2ndPlayerIfPresent`, `ResetStuffUponResurrection`, `RestorePedsWeapons`, `RestorePlayerStuffDuringResurrection`, `Save`, `SetPlayerWantedLevelForForbiddenTerritories`, `SetUpSkip`, `SkipCanBeActivated`, `SortOutStreamingAndMemory`, `StopPlayerMovingFromDirection`, `StorePedsWeapons`, `Update`, `UpdateSkip` |

**Diferencias reales corregidas:**
- `GameState` es un byte (`eGameLogicState : uint8`) y `ActivePlayers` (0x96A8B1) también: la DLL leía y escribía 4 bytes y pisaba `ActivePlayers`, `bPlayersCannotTargetEachOther` y `bPlayersCanBeInSeparateCars` (afecta también a las lecturas de `CAutomobile`, `CPlayerPed`, `CPlayerInfo`, `CDarkel` y el audio de voces).
- La bandera del mando que tocan `ClearSkip`, `SetUpSkip` y `UpdateSkip` es el bit 0 del byte 0x10F (`bPlayerSkipsToDestination`), no `bCamera`.
- `CalcDistanceToForbiddenTrainCrossing`:
  - tres comprobaciones independientes (`== 0`, `== 1` y `<= 1`), el tercer punto es (2770, 277);
  - producto escalar 3D (z del cruce 0, orden z, y, x) y la salida es `(cx - x, cy - y, -z)` (la DLL usaba 2D y z = 0).
- `IsPointWithinLineArea`: devuelve la paridad de los cruces (la DLL devolvía `true` en el primero).
- `IsPlayerAllowedToGoInThisDirection`: el jugador 1 solo puede moverse si el producto es `< 0` y el otro si es `> 0` (la DLL devolvía casi siempre `false`); orden z, y, x; jugador en foco con comparación sin signo.
- `IsPlayerUse2PlayerControls`: sin foco devuelve `true` y si no compara con `CWorld::Players[foco]`.
- `DoWeaponStuffAtStartOf2PlayerGame`: guarda las armas del jugador 1 (la DLL las restauraba); `GiveWeapon(tipo, munición)` al jugador 2 y copia `m_nChosenWeapon` del 1 al 2.
- `FindCityClosestToPoint`: orden LS, LV, SF (empates y NaN como el EXE).
- `LaRiotsActiveHere` y `SetPlayerWantedLevelForForbiddenTerritories`: `FindPlayerCoors` (la del vehículo) en cada comprobación; nivel de búsqueda con signo; límites estrictos.
- `PassTime`: enteros de 32 bits (la DLL acumulaba horas en un `uint8`) y un día cada vez (día de la semana con vuelta a 1 y +1 a la estadística por día).
- `Save`/`Load`: devuelven `true` y guardan las globales directamente.
- `RestorePedsWeapons`: el segundo modelo se comprueba con el estado de carga del primero (se quita el `FIX_BUGS`).
- `RestorePlayerStuffDuringResurrection`:
  - `CleanUpOldReference(&m_pVehicle)` sobre el propio peatón (la DLL pasaba la dirección de una copia local);
  - anula `m_vecMoveSpeed`, `m_pFire` se limpia después de `Extinguish` y `SetTextureAndModel` es la versión con hashes (0x5A8050);
  - `m_fDistanceToWater = 99999.9f` (0x47C34FF3).
- `ResetStuffUponResurrection` y `Update`: rumbo × 57.29578 (0x859878) en vez de `RadiansToDegrees`/`DegreesToRadians`.
- `SetUpSkip`: guarda `SkipHeading` y siempre `SkipVehicle`, y registra la referencia sobre `&SkipVehicle`.
- `UpdateSkip`:
  - primero la cruceta y luego la tarea de salir del coche; `SKIP_IN_PROGRESS` sigue solo con tiempo `>` temporizador;
  - velocidad del vehículo = `forward × 0.4f` y ángulo de cámara `GetHeading() - pi/2` en x87 sin redondeo intermedio;
  - `PassTime(_ftol2(dist × 0.05f + 23))`.
- `Update`:
  - partida a dos jugadores y distancias con `CWorld::Players` y la posición de los peatones (no `FindPlayerCoors`);
  - mensajes de foco como el EXE (contador sin signo, 15 s/60 s y solo si se muestra) y `!= NONE` para el temporizador de 30 s;
  - muerte y arresto con comprobaciones independientes, `Say(..., true, true, true)` y "BUSTED" solo para el jugador 2;
  - el dinero solo baja si es positivo; multa por defecto 100 (sin `NOTSA_UNREACHABLE`) y leída de `CWorld::Players[0]`;
  - búsqueda del punto de reinicio con la posición del peatón (y su entrada/salida); `m_nNumHoursDidntEat` solo al morir; misión sin ayuda ni `PassTime` y siempre con fundido;
  - `bFadeInAfterNextDeath/Arrest` se activan en la global (la DLL cambiaba un parámetro);
  - `CGeneral::GetRandomIntInRange(0, 4)` (0x407180) y `CVector::Normalise` para separar a los jugadores.

**Método:**
- Peatones (con inteligencia y `CPlayerPedData` dentro de `CWorld::Players`), vehículos, matrices, `CWanted`, `CPedClothesDesc`, fuego, entradas/salidas y tareas son falsos con offsets crudos y vtables trazadas (destructor y `Teleport`); `CWorld::Players` se sustituye por el fixture durante cada ejecución.
- Stubs con traza en un array fijo (sin reservas durante la llamada): estadísticas, reloj, pickups, cámara, `CPad::StopShaking`, mensajes y ayuda del HUD, `TheText.Get`, `CWorld`, referencias, `CPlayerInfo`/`CPlayerPed`/`CPed`, tareas, `CWanted::Reset`, fuego, audio, conocidos, scripts, streaming, garajes, tiendas, posefectos, ciclo de tiempo, ropa, `CTimer`, plano lejano RW, colocación en carretera, puntos de reinicio, punto de mira, `rand`, `RemovePassenger`, `Test2DLineAgainst2DLine` y el búfer de guardado (nunca ficheros reales). `FindPlayerPed/Vehicle/Coors/Wanted` se simulan sin traza.
- Los cuerpos de la DLL se resuelven antes de parchear y nunca se parchea un stub; el cambio de hook se comprueba después de `State()`.
- Se guardan y restauran en cada ejecución y al final: todas las globales de `CGameLogic` (estado, temporizadores, salto, armas guardadas, puntos de reinicio, `n2PlayerPedInFocus`, configuración), `CWorld::Players`, jugador en foco, `CTimer`, reloj, `currArea`, pila de entradas/salidas, cámara, mandos, banderas de `CRestart`, cuentas atrás, `bInPoliceStation`, `m_waterEnable`, cinemática, trucos y `m_BigMessage`.
- Fuera de los fixtures: `n2PlayerPedInFocus` fuera de 0..2 y jugadores sin `CPlayerPedData` (el EXE lee fuera de `CWorld::Players` o desreferencia nulos).

Repetir: `python reversing/GameLogic27/build_probe.py` y `python reversing/run.py --case gamelogic27`. Evidencia: [validation-summary.json](validation-summary.json).

<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9328 entradas binarias sin nombre catalogado; 924 destinos originales explícitos fuera del catálogo, 499 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Estadísticas (lista de 32)

Validadas el 04/10/2026 con **13.056 pares original/candidato dentro de GTA**, todos exactos: 32 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CStats` | `Init`, `GetStatValue`, `SetStatValue`, `IsStatFloat`, `GetStatID`, `IncrementStat`, `DecrementStat`, `ModifyStat`, `SetNewRecordStat`, `RegisterFastestTime`, `RegisterBestPosition`, `IsStatCapped`, `CheckForThreshold`, `SafeToShowThisStat`, `ConvertToMins`, `ConvertToSecs`, `FindCriminalRatingNumber`, `GetPercentageProgress`, `FindMaxNumberOfGroupMembers`, `ProcessReactionStatsOnIncrement`, `ProcessReactionStatsOnDecrement`, `DisplayScriptStatUpdateMessage`, `UpdateStatsWhenSprinting`, `UpdateStatsWhenRunning`, `UpdateStatsWhenOnMotorBike`, `UpdateStatsWhenFighting`, `LoadActionReactionStats`, `GetTimesMissionAttempted`, `RegisterMissionAttempted`, `RegisterMissionPassed`, `Load`, `Save` |

**Diferencias reales corregidas:**
- Estadísticas enteras:
  - `SetStatValue`, `IncrementStat`, `DecrementStat`, `SetNewRecordStat` y `RegisterFastestTime`/`RegisterBestPosition` guardan `ftol` truncado a **int16 con signo** (`movsx ax`);
  - el índice no se comprueba (stat 82..119 lee antes de `StatTypesInt`): fuera los `assert` y el `operator[]` de `std::array`.
- `RegisterFastestTime` y `RegisterBestPosition` se quedan con el **menor** valor (o el nuevo si la estadística vale 0); antes guardaban el mayor.
- `SetNewRecordStat` escribe siempre `max(actual, valor)` (NaN -> valor) y llama una sola vez a `CheckForStatsMessage`; `DecrementStat` no pasa por `SetStatValue` (una sola llamada).
- `IncrementStat`:
  - jugador de `CWorld::Players[PlayerInFocus]`, no `FindPlayerPed`;
  - calorías: diferencia de salud truncada a entero;
  - comer (335): guarda las horas sin comer limitadas a [0, 36] y recorta `value` a la salud que falta;
  - tope 1000 con NaN -> 1000; `value <= 0` o NaN sale.
- `FindCriminalRatingNumber`:
  - las visitas al hospital **restan** (·3);
  - dinero entero / 5000; balas **> 100** (estricto);
  - operaciones en x87 con los enteros cargados sin redondear (`fild`/`fiadd`) y `ftol` en cada etapa (suma, trampas, puntería, progreso), como el EXE;
  - progreso = hecho / total · 1000 (`GetPercentageProgress` en línea).
- `GetPercentageProgress`: `STAT_PROGRESS_MADE / STAT_TOTAL_PROGRESS` (no `/ 187`), 0 si el total es 0.
- NaN como el EXE en `CheckForThreshold`, `DecrementStat`, `ModifyStat`, `DisplayScriptStatUpdateMessage` (`value > 1`), los topes de grasa y los límites de tiempo de correr/esprintar/moto.
- Byte bajo de la estadística en `SafeToShowThisStat` y en `ProcessReactionStatsOnIncrement`/`OnDecrement`.
- `UpdateStatsWhen*`: `ftol` del ritmo y del paso de tiempo en ms (`ms_fTimeStep · 0.02f · 1000`, no `/ 50`), contadores sin signo cargados con `fild` (+2^32).
- `DisplayScriptStatUpdateMessage` (fuerza de la banda): repite `FindPlayerPed` y `FindMaxNumberOfGroupMembers` como el EXE.
- `LoadActionReactionStats`: `sscanf("%d %s %f")` sin comprobar el resultado; id y valor empiezan en 0 una vez y conservan el valor anterior.
- `Save` llama a `PopulateFavoriteRadioStationList` antes de guardar las emisoras; `Init` sigue el orden del EXE; `GetStatID` usa `sprintf`.

**Método:**
- Cada ejecución rellena de forma determinista todos los arrays de `CStats` (0xB78200..0xB79518), `CWorld::Players` con peatones falsos, el jugador en foco, `CTimer` y las banderas del mensaje de estadística.
- Se simulan y trazan `CheckForStatsMessage` (con hash de las estadísticas en cada llamada), `LoadStatUpdateConditions`, `PopulateFavoriteRadioStationList`, `UpdateFatAndMuscleStats`, `UpdateStatsAddToHealth`, `CHud::SetHelpMessageStatUpdate` y `FindPlayerPed`.
- `CFileMgr` y `CFileLoader::LoadLine` sirven líneas en memoria (nunca se lee `AR_STATS.DAT`); el búfer de guardado está simulado (nunca ficheros de partida).
- Todas las globales tocadas se guardan y se restauran en cada ejecución y al final.

Repetir: `python reversing/Stats32/build_probe.py` y `python reversing/run.py --case stats32`. Evidencia: [validation-summary.json](validation-summary.json).

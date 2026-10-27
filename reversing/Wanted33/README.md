<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Nivel de búsqueda (lista de 33)

Validadas el 04/10/2026 con **15.616 pares original/candidato dentro de GTA**, todos exactos: 33 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CWanted` | `Initialise`, `Reset`, `InitialiseStaticVariables`, `UpdateWantedLevel`, `SetMaximumWantedLevel`, `AreMiamiViceRequired`, `AreSwatRequired`, `AreFbiRequired`, `AreArmyRequired`, `NumOfHelisRequired`, `ResetPolicePursuit`, `UpdateCrimesQ`, `ClearQdCrimes`, `AddCrimeToQ`, `ReportCrimeNow`, `IsInPursuit`, `UpdateEachFrame`, `RegisterCrime`, `RegisterCrime_Immediately`, `SetWantedLevel`, `CheatWantedLevel`, `SetWantedLevelNoDrop`, `ClearWantedLevelAndGoOnParole`, `WorkOutPolicePresence`, `IsClosestCop`, `ComputePursuitCopToDisplace`, `RemovePursuitCop` (función y método), `RemoveExcessPursuitCops`, `Update`, `CanCopJoinPursuit` (función y método), `SetPursuitCop` |

**Diferencias reales corregidas:**
- Comparaciones con signo como el EXE (el enum `eWantedLevel` es sin signo en la DLL):
  - umbrales y tope del caos en `UpdateWantedLevel` y máximo con el caos previo a la condicional en `ReportCrimeNow`;
  - nivel en `AreMiamiViceRequired`, `NumOfHelisRequired`, `UpdateEachFrame`, `SetWantedLevel`, `CheatWantedLevel`, `SetWantedLevelNoDrop` y `Update`;
  - la estadística de estrellas ganadas/evadidas es un entero con signo (`fild`).
- Niveles fuera de 0..6: `SetMaximumWantedLevel` no cambia nada y `SetWantedLevel` no toca el caos (sin `assert` ni `NOTSA_UNREACHABLE`); `NumOfHelisRequired` devuelve 0.
- `AddCrimeToQ` devuelve falso al meter un crimen nuevo (devolvía `alreadyReported`, y `RegisterCrime_Immediately` no denunciaba el crimen); sin `assert` de tipo nulo.
- `ReportCrimeNow`:
  - multiplica por 0,333f en vez de dividir entre 3;
  - solo suma caos para los tipos 2..22 (la DLL sumaba también tipos mayores);
  - el caos se suma como en el EXE: `fiadd` del entero al float y truncado de 64 bits.
- `UpdateCrimesQ` y `Update` comparan `tiempo > inicio + N` (con desbordamiento) en vez de `tiempo - inicio > N`.
- `Update`:
  - con más de una estrella el nivel no baja fuera del campo/desierto o en un interior; en vehículo policial, avión o helicóptero nunca baja (la DLL usaba `CanSeeOutSideFromCurrArea` y otra combinación);
  - resta 2 solo por la región del tiempo, y repite `FindPlayerVehicle` en cada comprobación;
  - al acabar la persecución guarda el récord en la estadística 51 y el valor en la 52.
- `ClearWantedLevelAndGoOnParole` llama dos veces a `FindPlayerWanted`.
- `WorkOutPolicePresence`, `ComputePursuitCopToDisplace` e `IsClosestCop`: orden de sumas de las distancias del EXE; `ComputePursuitCopToDisplace` llama a `FindPlayerPed` por policía.
- `IsClosestCop` sigue el algoritmo del EXE (selección repetida del más cercano, estrictamente menor, con la posición de `FindPlayerPed` y no `FindPlayerCoors`).

**Método:**
- Los `CWanted` son falsos; `FindPlayerWanted` devuelve otro `CWanted` falso (o el mismo objeto).
- Policías, peatón del jugador y vehículos falsos con offsets crudos; pools de peatones y vehículos falsos (se sustituyen los punteros solo durante la llamada).
- Se simulan y trazan `FindPlayerPed/Vehicle/Coors/Wanted`, `CStats`, `CGangWars::GangWarFightingGoingOn`, el escáner de la policía, `CGameLogic::SetPlayerWantedLevelForForbiddenTerritories`, `CPed::IsAlive` y `CVehicle::IsLawEnforcementVehicle`; el `printf` del EXE se anula.
- Se guardan y restauran `MaximumWantedLevel`, `MaximumChaosLevel`, `UseNewsHeliInAdditionToPolice`, el reloj, el truco, `CDarkel`, la región del tiempo, el área y los punteros de los pools.
- Fuera de los fixtures:
  - listas de policías llenas con todos vivos a menos de 1 m del jugador (`RemoveExcessPursuitCops` no termina, tampoco en el EXE);
  - `IsClosestCop` con más de 10 policías a comprobar (el EXE escribe fuera de su pila);
  - multiplicadores NaN.

Repetir: `python reversing/Wanted33/build_probe.py` y `python reversing/run.py --case wanted33`. Evidencia: [validation-summary.json](validation-summary.json).

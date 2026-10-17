<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Control de coches CCarCtrl (lista de 22)

Validadas el 03/10/2026 con **10.304 pares original/candidato dentro de GTA**, todos exactos: 22 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CCarCtrl` | `Init`, `ReInit`, `InitSequence`, `ChooseBoatModel`, `ChooseCarModelToLoad`, `ChoosePoliceCarModel`, `ChooseGangCarModel`, `GetNewVehicleDependingOnCarModel`, `CreateCarForScript`, `IsAnyoneParking`, `IsThisVehicleInteresting`, `RemoveFromInterestingVehicleList`, `JoinCarWithRoadAccordingToMission`, `PossiblyFireHSMissile`, `PruneVehiclesOfInterest`, `RemoveCarsIfThePoolGetsFull`, `RemoveDistantCars`, `SlowCarDownForObject`, `SlowCarOnRailsDownForTrafficAndLights`, `FindMaxSteerAngle`, `GenerateRandomCars`, `ScriptGenerateOneEmergencyServicesCar` |

**Diferencias reales corregidas:**
- `Init` y `ReInit` limpian 11 grupos de coches de bandas (0xC0EA28 a 0xC0EC22), no 10.
- `InitSequence`: el sentido de la secuencia es el bit 4 de `rand` (no el bit 2).
- `ChooseCarModelToLoad`:
  - índice `_ftol2(n * (rand * 1/32768))` en vez de `GetRandomNumberInRange` entero;
  - el número de coches se lee con signo y no se comprueba que sea > 0;
  - el modelo se lee sin signo.
- `ChoosePoliceCarModel`: tirada con `GetRandomIntInRange` (0x407180) y `FindPlayerWanted(-1)` antes de cada consulta.
- `RemoveFromInterestingVehicleList` limpia todas las coincidencias (sin `break`).
- `JoinCarWithRoadAccordingToMission`: sin entidad objetivo no hace nada (la DLL la desreferenciaba).
- `PossiblyFireHSMissile`:
  - el producto escalar usa la dirección normalizada (antes la copia sin normalizar), en orden (z + y) + x;
  - usa `m_matrix` directamente (sin `GetMatrix`, que puede crear matriz).
- `RemoveCarsIfThePoolGetsFull`:
  - recorre el pool de atrás hacia delante (desempates);
  - distancia inicial 10.000.000 (no `FLT_MAX`);
  - suma (dz² + dx²) + dy².
- `RemoveDistantCars`:
  - recorre el pool de atrás hacia delante;
  - distancia 2D sqrt(dy² + dx²) (era 3D);
  - `FindPlayerCentreOfWorld(CWorld::PlayerInFocus)`.
- `SlowCarDownForObject`:
  - `*v = min(*v, (1 - (h - 7) * 0.07692308) * k)` (la DLL multiplicaba el mínimo por `k` y dividía por 13);
  - colisiones del model info (sin col model especial de vehículo) y `m_matrix` directo;
  - productos escalares en orden (z + y) + x y NaN como el x87.
- `SlowCarOnRailsDownForTrafficAndLights`: con velocidad máxima igual a la velocidad no llama a `ModifySpeed`; comparaciones NaN como el x87.
- `FindMaxSteerAngle`: si |v| > 0,7 devuelve 0,2; si no, min(0,9 - |v|, 0,7) sin límite inferior (el `clamp` cambiaba el caso |v| = 0,7).
- `GenerateRandomCars`:
  - solo en el área 0 (no `CanSeeOutSideFromCurrArea`);
  - los 100 coches iniciales solo cuando la cuenta atrás llega a 0;
  - faltaban trenes, aviones y servicios de emergencia (cada vez que cambian los bits 12+ del reloj).

**Método:**
- Vehículos FALSOS (bytes con desplazamientos del EXE) dentro de un pool de vehículos FALSO (se cambia el puntero 0xB74494); entidades, matrices, model infos y col models falsos.
- No se crea ni se borra ningún vehículo ni peatón: `operator new` de vehículos devuelve nulo, `GetNewVehicleDependingOnCarModel` se sustituye en `CreateCarForScript` y el destructor se traza por una vtable falsa.
- Se simulan y trazan rand, mundo, streaming, scripts, jugador, policía, semáforos, proyectiles y generadores.
- El arreglo reversible `CCarCtrl_RemoveDistantCars_UseAfterFree` se desactiva durante el banco (comportamiento del EXE) y se restaura al final.
- Se guardan y restauran por ejecución los globales de `CCarCtrl`, grupos de coches cargados y grupos de coches de `CPopulation`, estados de carga, contadores de uso de model infos, reloj, área, cinemática, jugador en foco y contador de generadores.
- Fuera de los fixtures:
  - camino de barco de `CreateCarForScript` y motos/BMX de `GetNewVehicleDependingOnCarModel` (exigen construir un vehículo real; con `new` nulo el EXE se cae);
  - número de secuencia 0 en `InitSequence` (división por cero).

Repetir: `python reversing/CarCtrl22/build_probe.py` y `python reversing/run.py --case carctrl22`. Evidencia: [validation-summary.json](validation-summary.json).

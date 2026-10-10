<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9328 entradas binarias sin nombre catalogado; 924 destinos originales explícitos fuera del catálogo, 499 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# IA de coches CCarAI (lista de 25)

Validadas el 04/10/2026 con **11.520 pares original/candidato dentro de GTA**, todos exactos: 25 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CCarAI` | `AddAmbulanceOccupants`, `AddFiretruckOccupants`, `AddPoliceCarOccupants`, `BackToCruisingIfNoWantedLevel`, `CarHasReasonToStop`, `EntitiesGoHeadOn`, `FindPoliceBikeMissionForWantedLevel`, `FindPoliceBoatMissionForWantedLevel`, `FindPoliceCarMissionForWantedLevel`, `FindPoliceCarSpeedForWantedLevel`, `FindSwitchDistanceClose`, `FindSwitchDistanceFar`, `GetCarToGoToCoors`, `GetCarToGoToCoorsAccurate`, `GetCarToGoToCoorsRacing`, `GetCarToGoToCoorsStraightLine`, `GetCarToParkAtCoors`, `MakeWayForCarWithSiren`, `MellowOutChaseSpeed`, `MellowOutChaseSpeedBoat`, `TellCarToBlockOtherCar`, `TellCarToFollowOtherCar`, `TellCarToRamOtherCar`, `TellOccupantsToLeaveCar`, `UpdateCarAI` |

**Diferencias reales corregidas:**
- `AddPoliceCarOccupants`: el piloto del Predator se ata en (0, 3, 2) con 2π exacto (0x40C90FDB); escopeta con `GetRandomNumberInRange(0, 1) < 0,25`; pasajero con `GetRandomIntInRange(0, 100) < 50`.
- `BackToCruisingIfNoWantedLevel`: mira primero `m_bEverybodyBackOff`, después el nivel del jugador, y apaga la sirena.
- `EntitiesGoHeadOn`: productos en orden (z + y) + x; sin matriz el forward sale de fsin/fcos del x87 sin redondear; NaN en el primero sigue.
- `FindPoliceCarMissionForWantedLevel`: `rand() & 3` directo y devuelve embestir de lejos (no freno de mano); fuera de rango, bloquear.
- `FindPoliceCarSpeedForWantedLevel`: aleatorio real en [12, 16] con `_ftol2`; velocidad por 54/72/75/78; fuera de rango 0.
- `MellowOutChaseSpeedBoat`: fuera de rango no cambia la velocidad.
- `GetCarToGoToCoors*`:
  - la cercanía se mide contra el destino actual (no la posición del coche), |dx| y |dy| estrictos y NaN cerca;
  - hora de inicio antes del estado y misión escrita directamente.
- `GetCarToParkAtCoors`: llama a `GetCarToGoToCoors` y pone `bParking` (no `bCanPark`).
- `MakeWayForCarWithSiren`:
  - sale si la sirena va a menos de 0,1; recorre el pool hacia atrás;
  - lado por producto cruzado, `WAIT/BRAKE` en la rama lateral, módulo contra 0,05 y comparaciones NaN del x87.
- `MellowOutChaseSpeed`: distancia (dz² + dy²) + dx², módulo de la velocidad del jugador < 0,07 y distancia < 30.
- `TellCarToBlockOtherCar`/`TellCarToRamOtherCar` implementadas (la DLL abortaba); referencias sin comprobar nulo.
- `TellOccupantsToLeaveCar`: `FindTaskByType`, todos los asientos hasta `m_nMaxPassengers` y un `rand` descartado por pasajero.
- `UpdateCarAI`:
  - helicópteros salen siempre; avión policía a (-6000, 10000, 500) con misión de vuelo;
  - faltaba `MISSION_BLOCKPLAYER_FARAWAY`; distancias con raíz y estrictas (`<`, `>`) como el EXE;
  - embestir/bloquear de cerca: temporizador `_ftol2(paso * 16,666666 + t)`, módulos contra 0,05/0,04 y Mellow en ambas ramas;
  - `BLOCKCAR_CLOSE` vuelve a lejos; `KILLPED_FARAWAY` usa la distancia cercana; `RAMCAR_CLOSE` mira si el objetivo es el coche del jugador;
  - `SLOWLY_DRIVE_TOWARDS_PLAYER_1/2`, `BLOCKPLAYER_FORWARDANDBACK` (dirección jugador - coche) y moto de policía (20000/2500, secuencia en orden del pool, pistola 22) como el EXE;
  - `CCullZones::PoliceAbandonCars` (no `CamCloseInForPlayer`);
  - atasco: condición sin invertir, ((semilla & 15) + 40) * 500 con signo, `bMadDriver` y 700 ms;
  - boca abajo: espera (no marcha atrás); persecución con el EXE (ángulos en línea, umbrales en módulo);
  - multiplicador de velocidad fija el objetivo si la diferencia es menor que el paso; `rand` del EXE en la bocina.

**Método:**
- Vehículos, peatones, inteligencias, datos de jugador, `CWanted`, model infos y tareas FALSOS con desplazamientos del EXE; pools de vehículos y peatones falsos (se cambian 0xB74494 y 0xB74490).
- No se crea ningún vehículo ni peatón: `SetUpDriver`/`SetupPassenger` devuelven peatones falsos.
- Las tareas nuevas van al pool real de tareas (huecos libres a 0xCD, `m_LastFreeSlot` restaurado); se copian en `SetTask`/`CEventGroup::Add` y se borran después; se comprueba que no queda ninguna.
- Se simulan y trazan jugador, rand, `CCarCtrl`, semáforos, `CCullZones`, métodos de vehículo/peatón/inteligencia, `SetTask`, eventos, referencias y la bocina (vtable).
- Stubs sin reservas de memoria (array fijo; el json se construye después de la llamada).
- Se guardan y restauran por ejecución reloj, fotogramas, paso, punteros de pool, zona de bandas, `bUseCarAI` y punteros de model info.

Repetir: `python reversing/CarAI25/build_probe.py` y `python reversing/run.py --case carai25`. Evidencia: [validation-summary.json](validation-summary.json).

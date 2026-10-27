<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CEventHandler: respuesta a tirar a un ped de la moto (grupo E)

Validadas el 07/10/2026 con **2.048 pares original/candidato dentro de GTA**, todos exactos: 1 función de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CEventHandler` | `ComputeKnockOffBikeResponse` |

**Diferencias reales corregidas:**
- `ComputeKnockOffBikeResponse` no estaba reconstruida (llamaba a 0x4B9FF0 con `plugin::CallMethod` y su hook tenía `.reversed = false`); ahora está rehecha desde el EXE (0x4B9FF0 a 0x4BA7A4) y el hook es normal:
  - según el tipo del evento (se consulta por la vtable, hasta cuatro veces como el EXE): `KnockOffBike` calcula el daño por caída (`CEventDamage` con `WEAPON_FALL`, torso, dirección 2) solo si `AffectsPed` y usa el propio evento; `Damage` construye un evento local con la velocidad del vehículo por 0,75, dirección según el byte del evento (0 = -adelante, 1 = derecha, 2 = adelante, otro = -derecha), intensidad 50 y como tipo el arma del daño; `VehicleDied` y `VehicleOnFire` hacen lo mismo con tipo 0x33 y dirección `(rand & 0xFFFF) / 32768 * 4` truncada; cualquier otro tipo desreferencia nulo en el EXE y aquí sale sin hacer nada;
  - si el vehículo es nulo o el ped no es conductor ni uno de los tres primeros pasajeros no hace nada más; si lo es, registra `INTERESTING_EVENT_16` y llama a `SetPedSafePosition` (si falla y el ped está en coche, borra el bit de colisión de la entidad), `SetPedOutCar` y `CalcForcesAndAnims`;
  - tipo 53 (caída): quita de pie/estaba de pie, marca en el aire, activa los bits 0x100 y 0x8000000 de las banderas físicas y crea `InAirAndLand` (vida mayor que 0) o `Die` (el resto, NaN incluido);
  - con vida menor o igual que 0 (aquí NaN cuenta como vivo): `Die` con el id de animación 0xBF sustituido por 0x6A, 0x19 o 0x1B según la dirección; con vida mayor y tipo distinto de 0: `FallAndGetUp` y, si es líder y está en un grupo, envía `CEventLeaderExitedCarAsDriver` al grupo, y si no es líder ni conductor y el líder va en coche sin evento de moto, envuelve la respuesta en una secuencia con `EnterCarAsPassenger`;
  - al final avisa con `CEventDraggedOutCar` a quien tira de la moto (si lo hay) y activa `bOnSideStand` de la moto (0x614 bit 4) o lo desactiva en las demás rutas, solo si el vehículo es una moto.

**Método:**
- Original con el hook de la función desactivado frente a candidato, con fixtures idénticos y la misma semilla; se llama a la dirección del EXE (`thiscall`) con un manejador falso de 0x34 bytes cuya respuesta empieza con un valor centinela.
- Se comparan el valor final de `m_EventResponseTask`, todas las tareas nuevas del pool de tareas (tipo y 0x80 bytes con los punteros traducidos, huecos libres a 0xCD y `m_LastFreeSlot` restaurado), el pool de eventos (cabecera, estados y almacenamiento), las listas de referencias, la memoria de los fixtures (peatones, matriz, vehículos, inteligencias, grupo) y la traza.
- Falsos: tres peatones (banderas, vida con casos límite como NaN e infinito, inteligencia propia), dos vehículos del tamaño de `CAutomobile` (velocidad, asientos, tipo y banderas de moto), una matriz con valores variados, un grupo y un evento con vtable trazada de los cuatro tipos (`KnockOffBike`, `Damage`, `VehicleDied`, `VehicleOnFire`).
- Simulados y trazados: `CEventKnockOffBike::SetPedSafePosition`, `SetPedOutCar` y `CalcForcesAndAnims` (con un vehículo falso revientan), `CEventDamage::AffectsPed`, `CPedDamageResponseCalculator::ComputeDamageResponse`, `CInterestingEvents::Add`, `CPedGroups::GetPedsGroup`, `CPedGroupMembership::IsLeader`/`GetLeader`, `CPedGroupIntelligence::AddEvent`, `CEventGroup::Add`/`GetEventOfType` y el RNG (`rand` 0x821B1E y `CGeneral::GetRandomNumber`). Los constructores, la asignación y los destructores de los eventos y las tareas se usan reales, de modo que se comparan el evento local, los registros de referencias y los bytes de relleno.
- Se restauran en cada ejecución y al final: estados de los pools de tareas y eventos, `m_LastFreeSlot`, referencias de los fixtures y el reloj.
- Fuera de los fixtures: un tipo de evento distinto de los cuatro anteriores (el EXE desreferencia nulo), un ped sin vehículo o sin matriz en los tipos `Damage`/`VehicleDied`/`VehicleOnFire`, y los bytes sin inicializar de la pila del evento local (+0x35 salvo el bit 0), que no se comparan.

Repetir: `python reversing/EvHE/build_probe.py` y `python reversing/run.py --case evhe`. Evidencia: [validation-summary.json](validation-summary.json).

<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco CVehB: CVehicle, grupo B (ocupantes, ciclo de vida y geometría)

Fecha: 06/10/2026. Estado: escrito, **sin compilar ni ejecutar** (pendiente de `build_probe.py` y `run_local.py`).
Prefijo de caso `cvehb`, espacio de nombres `CVehBLive`, ficheros `cvehb-runtime.start`, `cvehb-runtime.jsonl` y `cvehb-only.txt`.

## Método

* Original (hooks del grupo desactivados) frente a candidato (hooks activos) con fixtures idénticos sobre un **CVehicle falso**
  (búfer de 0xA18 bytes = `sizeof(CHeli)` en el hueco 3 de un pool de vehículos falso de 8 huecos) y **peatones falsos** (12 búferes de
  0x7C4 bytes; los 6 primeros forman un pool de peatones falso). Nunca se usan vehículos, peatones ni pools reales: los punteros de pool
  reales (0xB74490 / 0xB74494) solo se cambian durante la llamada y se restauran. `m_pReferences` queda a basura (las funciones
  de referencias están sustituidas por dobles), sin punteros a entidad real.
* Las funciones virtuales (`SetModelIndex`, `DeleteRwObject`, `GetHeightAboveRoad`, `GetTowHitchPos`, `GetTowBarPos`, `Save`, `Load`) se
  llaman por la vtable del EXE de CVehicle (0x871E80, huecos 5, 8, 53, 59, 60, 64, 65, comprobados volcando la vtable); el evento
  `switches` exige que el hueco apunte a la DLL solo en el candidato. Las llamadas virtuales que hace el código probado (BlowUpCar, hueco 41;
  destructor virtual, hueco 0, sobre el vehículo; destructor virtual y `FlagToDestroyWhenNextProcessed`, huecos 0 y 21, sobre peatones) van por
  tablas virtuales falsas con trazas; el resto de huecos son trampas (`virtual_inesperada`).
* Dobles trazados en la dirección del EXE **y** en el símbolo/cuerpo de la DLL (sin reservas de memoria dentro): RegisterReference /
  CleanUpOldReference, rand (0x821B1E, valores 0..0x7FFF como el CRT) + `CGeneral::GetRandomNumber`, FindPlayerPed, IsCoopGameGoingOn,
  RegisterKillByPlayer, RegisterCarBlownUpByPlayer, CWorld::Remove, CGarages::TriggerMessage, CPlayerInfo::AddHealth, CStreaming::RequestModel /
  SetModelIsDeletable (el EXE recibe 1 argumento y la DLL 2: dos dobles), CStats::IncrementStat / SetStatValue, CPlayerPed::DoesPlayerWantNewWeapon,
  CPed::GiveWeapon / GrantAmmo, CPopulation::AddPedInCar / AddPed / RemovePed (AddPedInCar coloca el peatón nuevo en el asiento pedido),
  CCarEnterExit::SetPedInCarDirect / ComputeTargetDoorToEnterAsPassenger, CPedClothesDesc::SetModel, CClothes::RebuildPlayer, LookAt del IK,
  CPhysical::ApplyMoveForce / ApplyTurnForce, CTaskManager::FindActiveTaskByType (tareas 700/701 falsas), CEventGroup::Add (se anotan los campos
  definidos del evento), ComputeDamageResponse, CEventDamage::AffectsPed, las bases del constructor/destructor (CPhysical, CAEVehicleAudioEntity,
  CAutoPilot; los dobles de las bases rellenan su parte con bytes deterministas por caso), CCarCtrl::UpdateCarCount, CReplay::RecordVehicleDeleted,
  CVehicle::DeleteRwObject (en el destructor), CRadar::ClearBlipForEntity, CFire::Extinguish, FxManager_c::DestroyFxSystem, RwTextureDestroy,
  CRopes::FindRope, CRope::Remove, CTxdStore::RemoveRef, RpClumpForAllAtomics, CEntity::DeleteRwObject / SetModelIndex, CustomCarPlate_TextureCreate,
  GetMaximumNumberOfPassengersFromNumberOfDoors, CColModel::RemoveCollisionVolumes, CVehicleSaveStructure::Construct / Extract y
  Save/LoadDataToWorkBuffer. Se ejecutan de verdad en ambos lados: `CPed::IsPlayer`, `RemovePassenger` (dentro de `SetupPassenger`), los
  constructores/destructores de `CEventVehicleDied`, `CEventDamage` y `CPedDamageResponseCalculator`, `CDamageManager::Get/SetEngineStatus` y
  `CVehicle::SetCollisionLighting`.
* Los cuerpos de la DLL de las bases se localizan por el orden de las llamadas dentro del constructor/destructor de la DLL, contrastados con
  los envoltorios de los hooks de `CPhysical` y `CAEVehicleAudioEntity` (como en CPedD / VehCtor62); si no se encuentran, el banco anota
  `warning` y marca esas dos funciones como fallo en vez de continuar.
* Se compara el vehículo entero (vptr normalizada), el resto del almacenamiento (otro vehículo y huecos vecinos), todos los peatones, el
  jugador, los búferes auxiliares, los globales tocados (Players, PlayerInFocus, colmodels especiales, ms_compsUsed, contadores de ambulancias
  y bomberos, estados de carga de modelos), el valor de retorno y la traza completa de llamadas salientes. Constructor: se enmascara el nibble alto
  del byte 0x42F (el EXE conserva lo que había; la DLL pone a cero el dword de flags entero, relleno sin nombre).
* El evento `layout` comprueba los desplazamientos de los campos de la DLL frente a los del EXE (y `sizeof` de CHeli, CVehicle, CCopPed,
  CPlayerInfo, CVehicleSaveStructure), y que `GetTaskManager` / `GetEventGroup` / `GetPlayerData` caen donde lee el EXE.

## Casos (11 520 pares)

| Función | Casos | Notas del fixture |
|---|---|---|
| Constructor 0x6D5F10 | 512 | búfer 0xCD, createdBy de 8 y 32 bits, CTimer variable, rand 0..0x7FFF |
| Destructor 0x6E2B40 | 512 | pool de vehículos falso, ocupantes, fuego, 3 partículas, placa, cuerda, ambulancia/bomberos, índice de colmodel especial, salud (NaN, 250) |
| SetModelIndex (hueco 5) | 512 | modelos RC / armas / vecinos de las dos tablas de saltos, ms_compsUsed, byte de pasajeros máximo |
| DeleteRwObject (hueco 8) | 256 | remapeo previo -1 / otros, clump falso o nulo |
| GetHeightAboveRoad (53) / GetTowHitchPos (59) / GetTowBarPos (60) | 256 c/u | colmodel con valores especiales (±0, inf, NaN), matrices generales |
| Save (64) / Load (65) | 128 c/u | Construct / Extract / SaveData / LoadData dobles |
| HeightAboveCeiling | 512 | modelos 0..7 y fuera de rango, alturas en los umbrales 500/800/950/1500, NaN, inf |
| IsSphereTouchingVehicle | 512 | colmodel del modelo o especial (0..3), caja con especiales, radio negativo/NaN |
| FindTyreNearestPoint | 512 | moto y no moto, ceros y especiales |
| Shutdown | 128 | 4 colmodels especiales con m_pColData nulo y no nulo |
| FillVehicleWithPeds | 512 | con y sin afro, conductor jugador / otro / nulo, estados de carga 0..3, asientos ocupados |
| KillPedsGettingInVehicle | 256 | 6 peatones: libres, dentro de vehículo, con colisión, tareas 700/701 hacia este u otro vehículo, AffectsPed alterna |
| KillPedsInVehicle | 256 | conductor y 8 asientos, coop |
| PickRandomPassenger | 256 | asientos vacíos/llenos, rand en los extremos |
| RemoveDriver | 512 | CADDY y los cuatro modelos de policía, bFreebies, streaming del palo de golf / escopeta, jugador falso o distinto |
| RemovePassenger | 512 | tren y no tren, máximo de pasajeros 0..9, contador a 0 |
| SetDriver | 1024 | modelos de la tabla de saltos y vecinos, bFreebies, moto y no moto, conductor jugador o no, PlayerInFocus 0/1 |
| SetUpDriver | 256 | creado por 0..5, conductor existente |
| SetupPassenger | 1024 | taxi / cabbie / stretch, tipos de peatón, gangPedType 14..23, modelo repetido, asiento propio ocupado |
| UpdatePassengerList / GetSpareHasslePosId / SetHasslePosId | 256 c/u | contador y asientos, subtipos, posiciones fuera de rango |
| DestroyVehicleAndDriverAndPassengers | 256 | estática (cdecl, sin this) con vehículo y ocupantes falsos |
| ActivateBomb / ActivateBombWhenEntered | 128 / 256 | estados de bomba 0..7, conductor, flag bDriverLastFrame, instalador nulo |
| ExtinguishCarFire | 256 | estado, salud (NaN), fuego, tipo, estado de motor 0..255 |
| ReactToVehicleDamage | 256 | conductor/pasajero 0 nulos o no, rand en el umbral 0x3FFF |
| ProcessCarAlarm / ProcessDelayedExplosion | 256 c/u | contadores en los extremos, paso de tiempo negativo y grande |

## No cubierto

* Constructor/destructor de los eventos y del calculador de daño (se ejecutan de verdad pero no se validan aquí).
* `CVehicle::SetDriver(nullptr)`: el EXE desreferencia el peatón nulo (RegisterReference sobre this nulo); la DLL lo tolera; no se prueba.
* `m_nMaxPassengers` > 8 salvo en `RemovePassenger` (máximo 9, lee el dword siguiente al array como hace el EXE).
* Overflow de float a entero (`_ftol2` fuera de rango) y NaN en `ProcessDelayedExplosion` / `ProcessCarAlarm`: dependerían de cómo compile la DLL
  `(int)(int64)float`.
* Efecto real sobre peatones/vehículos/pools del juego (dobles).

## Supuestos a comprobar al compilar

* Que existan y sean accesibles: `CPlayerInfo::AddHealth`, `CPhysical::ApplyTurnForce`, `FxManager_c::DestroyFxSystem` (no virtual),
  `CRope::Remove`, `CColModel::RemoveCollisionVolumes`, `CVehicle::CustomCarPlate_TextureCreate`, `CVehicleModelInfo::
  GetMaximumNumberOfPassengersFromNumberOfDoors` (estática), `CStreaming::ms_aInfoForModel[..].m_LoadState`, `CCarCtrl::NumAmbulancesOnDuty`.
* Que los cuerpos de la DLL se localicen (eventos `info` / `warning` al principio del log).
* Que el compilador no devirtualice `delete m_pDriver` / `delete vehicle` (llamada por vtable hueco 0, bandera 1).

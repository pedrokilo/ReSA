<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8547 funciones reconstruidas; 8547 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9226 entradas binarias sin nombre catalogado; 822 destinos originales explícitos fuera del catálogo, 398 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 102 aprobadas dentro del juego; 133 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# AutomobileFinal4: banco por pares de CAutomobile

Banco de comparación par a par dentro del juego completo (flujo Small2/PadMouse1/VehModelInfoFinal4) para las **99 funciones** de `CAutomobile` (`docs/hooks.csv`, EXE gta-sa.exe 1.0 US). **24192 pares** (99/99 funciones cubiertas, ninguna excluida). Fuentes probadas: `source/game_sa/Entity/Vehicle/Automobile.cpp` y `Automobile.h` (hashes en `artifacts/build-manifest.json`).

Uso: `CMAKE_BUILD_PARALLEL_LEVEL=2 _CL_=-MP2 bash reversing/_handoff/bench.sh AutomobileFinal4 3`. Las funciones se reparten en `CasesG1..CasesG5.hpp` (un solo banco).

## Qué se compara (original = hooks de la clase desactivados, candidato = hooks activos)

Cada par construye desde cero (semilla por función y caso) un mundo falso compartido: coches falsos de `CAutomobile` (0x988 bytes con vtable falsa copiada de las direcciones del EXE), matrices, marcos RenderWare, atomics, clumps, model infos, estructuras de vehículo, datos de handling y colmodels. Cada ejecución guarda y restaura todos los globales que toca.

Comparación: retorno, traza completa de llamadas con argumentos, hash del mundo falso, hash de la arena de `operator new/delete`, hash de los globales guardados y los campos de salida de cada caso. La palabra de control x87 es de 24 bits y el generador de números aleatorios se sustituye por el del EXE. Los registros llevan `original` y `candidate` no nulos.

## Qué se simula

Dobles trazados, parcheados en la dirección del EXE y en el símbolo o cuerpo de la DLL (`EntryPatch` con almacenamiento estático), de todo lo que toca estado real: RenderWare, audio, partículas, mundo, física, colisión y peatones, además de `operator new/delete` sobre una arena de 128 KB. Los dobles solo actúan en el hilo del banco; los hilos ajenos (audio) se ignoran. Los dobles con traza no tienen crédito.

## Corpus por función

| Función | EXE | Pares |
|---|---|---|
| `AddMovingCollisionSpeed` | virtual, ranura 49 de `0x871120` | 256 |
| `BlowUpCar` | virtual, ranura 41 de `0x871120` | 256 |
| `BlowUpCarCutSceneNoExtras` | virtual, ranura 42 de `0x871120` | 256 |
| `BlowUpCarsInPath` | `0x6AF110` | 128 |
| `BoostJumpControl` | `0x6A3A60` | 256 |
| `BreakTowLink` | virtual, ranura 62 de `0x871120` | 128 |
| `ClearHeliOrientation` | `0x6A2460` | 64 |
| `CloseAllDoors` | `0x6A4520` | 256 |
| `CloseBoot` | `0x6AFA20` | 192 |
| `Constructor` | `0x6B0A90` | 512 |
| `CustomCarPlate_AfterRenderingStop` | `0x6A2F30` | 128 |
| `CustomCarPlate_BeforeRenderingStart` | `0x6A2F00` | 128 |
| `Destructor` | `0x6A61E0` | 128 |
| `DoBurstAndSoftGroundRatios` | virtual, ranura 52 de `0x871120` | 256 |
| `DoHoverSuspensionRatios` | virtual, ranura 69 de `0x871120` | 128 |
| `DoSoftGroundResistance` | `0x6A4AF0` | 256 |
| `FindWheelWidth` | virtual, ranura 63 de `0x871120` | 192 |
| `FireTruckControl` | `0x729B60` | 256 |
| `Fix` | virtual, ranura 50 de `0x871120` | 256 |
| `FixDoor` | `0x6A35A0` | 256 |
| `FixPanel` | `0x6A3670` | 256 |
| `FixTyre` | `0x6A3580` | 128 |
| `GetAllWheelsOffGround` | `0x6A2F70` | 128 |
| `GetCarPitch` | `0x6A6050` | 256 |
| `GetCarRoll` | `0x6A6010` | 256 |
| `GetComponentWorldPosition` | virtual, ranura 25 de `0x871120` | 192 |
| `GetDooorAngleOpenRatio` | virtual, ranura 30 de `0x871120` | 192 |
| `GetDooorAngleOpenRatioU32` | virtual, ranura 29 de `0x871120` | 192 |
| `GetHeightAboveRoad` | virtual, ranura 53 de `0x871120` | 128 |
| `GetMovingCollisionOffset` | `0x6A2150` | 256 |
| `GetNumContactWheels` | virtual, ranura 55 de `0x871120` | 128 |
| `GetTowBarPos` | virtual, ranura 60 de `0x871120` | 256 |
| `GetTowHitchPos` | virtual, ranura 59 de `0x871120` | 256 |
| `HydraulicControl` | `0x6A07A0` | 384 |
| `IsComponentPresent` | virtual, ranura 26 de `0x871120` | 128 |
| `IsDoorClosed` | virtual, ranura 36 de `0x871120` | 192 |
| `IsDoorClosedU32` | virtual, ranura 35 de `0x871120` | 192 |
| `IsDoorFullyOpen` | virtual, ranura 34 de `0x871120` | 192 |
| `IsDoorFullyOpenU32` | virtual, ranura 33 de `0x871120` | 192 |
| `IsDoorMissing` | virtual, ranura 38 de `0x871120` | 192 |
| `IsDoorMissingU32` | virtual, ranura 37 de `0x871120` | 192 |
| `IsDoorReady` | virtual, ranura 32 de `0x871120` | 192 |
| `IsDoorReadyU32` | virtual, ranura 31 de `0x871120` | 192 |
| `IsInAir` | `0x6A6140` | 256 |
| `IsOpenTopCar` | virtual, ranura 39 de `0x871120` | 128 |
| `IsRoomForPedToLeaveCar` | virtual, ranura 45 de `0x871120` | 256 |
| `KnockPedOutCar` | `0x6A44C0` | 64 |
| `Load` | virtual, ranura 65 de `0x871120` | 64 |
| `NitrousControl` | `0x6A3EA0` | 384 |
| `OpenDoor` | virtual, ranura 27 de `0x871120` | 384 |
| `PlaceOnRoadProperly` | `0x6AF420` | 256 |
| `PlayCarHorn` | virtual, ranura 54 de `0x871120` | 128 |
| `PlayHornIfNecessary` | `0x6A3820` | 128 |
| `PopBoot` | `0x6AF910` | 192 |
| `PopBootUsingPhysics` | `0x6A44D0` | 128 |
| `PopDoor` | `0x6ADEF0` | 256 |
| `PopPanel` | `0x6ADF80` | 256 |
| `ProcessAI` | virtual, ranura 66 de `0x871120` | 512 |
| `ProcessAutoBusDoors` | `0x6A38A0` | 256 |
| `ProcessBuoyancy` | `0x6A8C00` | 512 |
| `ProcessCarOnFireAndExplode` | `0x6A7090` | 384 |
| `ProcessCarWheelPair` | `0x6A4EC0` | 1024 |
| `ProcessControl` | virtual, ranura 10 de `0x871120` | 512 |
| `ProcessControlCollisionCheck` | virtual, ranura 23 de `0x871120` | 256 |
| `ProcessControlInputs` | virtual, ranura 24 de `0x871120` | 384 |
| `ProcessEntityCollision` | virtual, ranura 22 de `0x871120` | 512 |
| `ProcessFlyingCarStuff` | virtual, ranura 68 de `0x871120` | 128 |
| `ProcessHarvester` | `0x6A9680` | 256 |
| `ProcessSuspension` | virtual, ranura 70 de `0x871120` | 512 |
| `ProcessSwingingDoor` | `0x6A9D70` | 384 |
| `RcbanditCheck1CarWheels` | `0x6B3F70` | 256 |
| `RcbanditCheckHitWheels` | `0x6B45E0` | 192 |
| `ReduceHornCounter` | `0x6A29A0` | 128 |
| `RemoveBonnetInPedCollision` | `0x6AA200` | 192 |
| `ResetSuspension` | virtual, ranura 67 de `0x871120` | 192 |
| `Save` | virtual, ranura 64 de `0x871120` | 64 |
| `ScanForCrimes` | `0x6ADFF0` | 256 |
| `SetAllTaxiLights` | `0x6A3760` | 64 |
| `SetBumperDamage` | `0x6B1350` | 256 |
| `SetBusDoorTimer` | `0x6A3860` | 128 |
| `SetDoorDamage` | `0x6B1600` | 384 |
| `SetHeliOrientation` | `0x6A2450` | 64 |
| `SetPanelDamage` | `0x6B1480` | 256 |
| `SetTaxiLight` | `0x6A3740` | 64 |
| `SetTotalDamage` | `0x6A27F0` | 256 |
| `SetTowLink` | virtual, ranura 61 de `0x871120` | 256 |
| `SetupDamageAfterLoad` | virtual, ranura 51 de `0x871120` | 256 |
| `SetupModelNodes` | `0x6A0770` | 192 |
| `SetupSuspensionLines` | virtual, ranura 48 de `0x871120` | 256 |
| `SpawnFlyingComponent` | `0x6A8580` | 384 |
| `StopNitroEffect` | `0x6A3E60` | 128 |
| `TankControl` | `0x6AE850` | 384 |
| `Teleport` | virtual, ranura 14 de `0x871120` | 192 |
| `TellHeliToGoToCoors` | `0x6A2390` | 128 |
| `TellPlaneToGoToCoors` | `0x6A2470` | 192 |
| `TowTruckControl` | `0x6A40F0` | 256 |
| `UpdateMovingCollision` | `0x6A1460` | 384 |
| `VehicleDamage` | virtual, ranura 56 de `0x871120` | 768 |
| `dmgDrawCarCollidingParticles` | `0x6A6DC0` | 192 |

## Aprobación (10/10/2026)

Corrida `20261010-142611-264011`: passed=true, 24192 pares, 99 funciones registradas con 0 fallos, evento complete (state_restored), saves y configuraciones intactos. Registrada con `register_automobilefinal4.py` (evidencia en `runs/`, `validation-summary.json`). Con este banco CAutomobile queda sin pendientes. Límites declarados en el alcance de cada función y en las secciones anteriores de este README.

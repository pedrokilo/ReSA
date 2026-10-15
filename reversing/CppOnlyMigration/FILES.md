<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Archivos pendientes de convertir a C++

40 archivos: 638 apariciones de ensamblador y 239 declaraciones naked.

Conteo lexico de source/game_sa, excluyendo comentarios y cadenas. Las dos columnas no se suman como funciones: un cuerpo naked puede contener ensamblador.

| Archivo | Ensamblador | naked |
| --- | ---: | ---: |
| [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp) | 6 | 4 |
| [source/game_sa/Birds.cpp](../../source/game_sa/Birds.cpp) | 3 | 3 |
| [source/game_sa/Cam.cpp](../../source/game_sa/Cam.cpp) | 102 | 3 |
| [source/game_sa/Camera.cpp](../../source/game_sa/Camera.cpp) | 43 | 1 |
| [source/game_sa/Core/Matrix.cpp](../../source/game_sa/Core/Matrix.cpp) | 10 | 10 |
| [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp) | 14 | 14 |
| [source/game_sa/Entity/Ped/Ped.cpp](../../source/game_sa/Entity/Ped/Ped.cpp) | 6 | 2 |
| [source/game_sa/Entity/Physical.cpp](../../source/game_sa/Entity/Physical.cpp) | 10 | 1 |
| [source/game_sa/Entity/Placeable.cpp](../../source/game_sa/Entity/Placeable.cpp) | 3 | 3 |
| [source/game_sa/Entity/Vehicle/Automobile.cpp](../../source/game_sa/Entity/Vehicle/Automobile.cpp) | 11 | 0 |
| [source/game_sa/Entity/Vehicle/Bike.cpp](../../source/game_sa/Entity/Vehicle/Bike.cpp) | 28 | 3 |
| [source/game_sa/Entity/Vehicle/Bmx.cpp](../../source/game_sa/Entity/Vehicle/Bmx.cpp) | 8 | 1 |
| [source/game_sa/Entity/Vehicle/Plane.cpp](../../source/game_sa/Entity/Vehicle/Plane.cpp) | 48 | 7 |
| [source/game_sa/Entity/Vehicle/Vehicle.cpp](../../source/game_sa/Entity/Vehicle/Vehicle.cpp) | 14 | 6 |
| [source/game_sa/Events/EventSource.cpp](../../source/game_sa/Events/EventSource.cpp) | 4 | 4 |
| [source/game_sa/Font.cpp](../../source/game_sa/Font.cpp) | 2 | 2 |
| [source/game_sa/Formation.cpp](../../source/game_sa/Formation.cpp) | 17 | 1 |
| [source/game_sa/Garage.cpp](../../source/game_sa/Garage.cpp) | 15 | 9 |
| [source/game_sa/General.cpp](../../source/game_sa/General.cpp) | 4 | 4 |
| [source/game_sa/Interior/Interior_c.cpp](../../source/game_sa/Interior/Interior_c.cpp) | 50 | 42 |
| [source/game_sa/MemoryMgr.cpp](../../source/game_sa/MemoryMgr.cpp) | 8 | 8 |
| [source/game_sa/Pad.cpp](../../source/game_sa/Pad.cpp) | 10 | 5 |
| [source/game_sa/PathFind.cpp](../../source/game_sa/PathFind.cpp) | 7 | 4 |
| [source/game_sa/PedGroups.cpp](../../source/game_sa/PedGroups.cpp) | 5 | 5 |
| [source/game_sa/PedIK.cpp](../../source/game_sa/PedIK.cpp) | 8 | 1 |
| [source/game_sa/PlayerRelationshipRecorder.cpp](../../source/game_sa/PlayerRelationshipRecorder.cpp) | 4 | 4 |
| [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp) | 6 | 0 |
| [source/game_sa/Radar.cpp](../../source/game_sa/Radar.cpp) | 10 | 2 |
| [source/game_sa/Ragdoll/BoneNode.cpp](../../source/game_sa/Ragdoll/BoneNode.cpp) | 4 | 4 |
| [source/game_sa/Shadows.cpp](../../source/game_sa/Shadows.cpp) | 56 | 1 |
| [source/game_sa/Stats.cpp](../../source/game_sa/Stats.cpp) | 69 | 44 |
| [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp) | 2 | 2 |
| [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp) | 4 | 4 |
| [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp) | 13 | 13 |
| [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp) | 3 | 3 |
| [source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp) | 6 | 4 |
| [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp) | 7 | 6 |
| [source/game_sa/TheZones.cpp](../../source/game_sa/TheZones.cpp) | 5 | 5 |
| [source/game_sa/WaterLevel.cpp](../../source/game_sa/WaterLevel.cpp) | 8 | 3 |
| [source/game_sa/cHandlingDataMgr.cpp](../../source/game_sa/cHandlingDataMgr.cpp) | 5 | 1 |

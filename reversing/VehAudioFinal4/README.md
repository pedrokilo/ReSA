<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 819 grupos / 8917 funciones reconstruidas; 8917 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CAEVehicleAudioEntity (banco final 4)

Banco par a par dentro del juego completo (original con los hooks de la clase desactivados frente a candidata con hooks activos) para las
**102 funciones** pendientes de `CAEVehicleAudioEntity` (`docs/hooks.csv`, `validated=0`): **20024 pares previstos**. La aprobación completa se registra únicamente tras una corrida sin diferencias y la reauditoría de sus registros.

Comando: `CMAKE_BUILD_PARALLEL_LEVEL=2 _CL_=-MP2 bash reversing/_handoff/bench.sh VehAudioFinal4 3`

## Estructura
- `Support.hpp`: mundo falso estatico (entidades de audio de 0x24C bytes con vptr 0x862CEC, vehiculos de 0x1000, matrices, transmisiones, peatones, reserva de sonidos), trazas sin reservas,
  falsos con traza en la direccion del EXE y en el simbolo de la DLL (`RequestNewSound`, `AreSoundsOfThisEventPlayingForThisEntity`, `AreSoundsPlayingInBankSlot`, `CancelSounds*`,
  `IsSoundBankLoaded`/`LoadSoundBank`, `rand` 0x821B1E + `CGeneral::GetRandomNumber`, `CPad::GetBrake/GetAccelerate`, `GetPositionRelativeToCamera_Placeable`, `GetDistanceAttenuation`,
  `CEntity::RegisterReference/CleanUpOldReference`, `CAudioEngine::StartRadio/StopRadio`), `PatchSet` con almacenamiento estatico y `Body()` calculado antes de parchear.
- `Ctx.hpp`: generador determinista, falsos aleatorios por caso, reserva de sonidos preexistentes y entidad de audio plausible.
- `CasesG1..G6.hpp`: tabla, globales guardados, falsos propios y ejecucion de cada funcion por grupo (G1 bancos/estado, G2 frecuencias/volumenes/eventos, G3 bocina/derrape/superficie,
  G4 aeronaves, G5 barco/tren/bici/especiales, G6 motor del jugador/ProcessVehicle/Service).
- `LiveComparison.hpp`: bucle de pares, resumen no nulo en registros pasantes, restauracion de hooks (incluidos los 3 bloqueados) y de globales.
- Sin E/S de audio real: gestor de sonidos y hardware simulados. Los hilos ajenos (audio) no se trazan (`Mine()`).

## Notas del metodo
- `m_tempSound` (+0x8..+0x7C) no se compara: el EXE lo usa de escritorio y la DLL puede usar un temporal. Los campos de `CAESound` que `Initialise` no escribe (relleno, tiempos, distancias a la camara,
  `m_IsAudioHardwareAware`) se ponen a cero en la copia que ve `RequestNewSound`.
- x87 a 24 bits durante cada llamada (`_controlfp`). Hay NaN silenciosos en comparaciones que los admiten; no se generan sNaN ni entradas que hagan al EXE leer pila sin inicializar (se documentan por funcion en los informes de grupo).
- `GetFreqForIdle` (0x4F5C60) y `GetVehicleDriveWheelSkidValue`/`GetVehicleNonDriveWheelSkidValue` (0x4F5F30/0x4F6000) preservan EDX que sus llamadores del EXE reutilizan: hooks naked (push/pop edx).
- El falso `G6_Vol` conserva ECX, igual que `GetVolForPlayerEngineSound` (0x4F5D00). El EXE llama a volumen en 0x4FC9C1 y a frecuencia en 0x4FC9CD sin recargar ECX; un falso C++ ordinario introducia un receptor incorrecto en la segunda llamada.
- En diferencias de `world`, `diff_world` muestra hasta 64 bytes discrepantes con desplazamiento relativo a `World`. Es solo diagnostico: no altera la igualdad ni enmascara campos adicionales.

## Correcciones de fuente (AEVehicleAudioEntity.cpp/.h, AEAudioEntity.cpp), con evidencia
- `ProcessPlayerJet` 0x50169B: maximo con signo de las palabras de 16 bits en +0x20/+0x22 (`cmp ax,cx; jg`), sin conversion float->int16 (C4244); 0x50190B pedales crudos `movsx`+`fild`+`fmul 1/255`.
- Constructor 0x4F63E0 / Destructor 0x6D0A10 (.h): solo las escrituras del EXE; vptr 0x859988 en +0x184 tras el destructor de `m_SkidSound` (0x4F2B80). `Initialise` 0x4F7670 y `GetVehicleAudioSettings` 0x4F5C10: copia `rep movsd` de 9 dwords (memcpy 0x24).
- `s_pVehicleAudioSettingsForRadio` = 0xB6B99C (4F4ED0 `mov eax,[B6B99C]`); `FlatTireSoundRollOffFactor` 2.0; constantes 0x862D00/0x862D04 exactas; `VolMin` -3.0 en 0x8CBC78; `StaticInitialise` de `CAEAudioEntity` 0x5B9D7C sin `CloseFile` si no abre.
- `ProcessPlayerVehicleEngine` 0x4FC413 (`jle 0x4FC471`, rama PLAYER_ID con aceleracion inhibida) y contador de crucero 0x4FC0AB..0x4FC10B (comparaciones con NaN); `ProcessVehicle` 0x501EA0 (`fmul 0.02f`, orden del producto escalar, tipo fuera de 0..9 sale).
- G2: `GetVolumeForDummyIdle/Rev` y `GetFrequencyForDummyIdle/Rev` (clamp con NaN 4F5218/4F5332, `!(>0.99)` 4F544C/4F556C), `GetFreqForPlayerEngineSound` 4F8123, `StartVehicleEngineSound` (m_ClientVariable -1.0, 4EFE50),
  `JustWreckedVehicle` 4FD1FC..4FD260 (`SetFlags(4,0)+StopSound`), `AddAudioEvent-0` 4F676E, `ProcessEngineDamage/Nitro/MovingParts` (NaN), `ProcessDummyVehicleEngine` 4F5578.
- G3: `GetSirenState` sin el bloque de depuracion NOTSA, `ProcessVehicleSkidding` usa `CTimer::ms_fTimeScale` (`fld [B7CB64]`), `PlayFlatTyre/Reverse/RoadNoiseSound` con `m_ClientVariable` -1.0, `ProcessReverseGear` NaN.
- G4: `ProcessAircraft` 0x501C5F (banco antes del chequeo de destruccion), `ProcessDummyProp` 0x4FD9CF/0x4FD9EE (palabras crudas), `ProcessAIHeli` 0x4FF1EA (NaN), `ProcessPlayerJet` (pedales crudos), `ProcessGenericJet` 0x4FFB0E (sin `abs`).
- G5: `ProcessBoatEngine` (0x500510 `bBoatEngineInWater`, constantes 0x862D50/0xB6BA74, orden de sumas, sin `step_to`), `ProcessDummyTrainEngine` (0x5008DC signo, 0x862D58/60), `ProcessPlayerTrainBrakes` (tabla 0x8CC1E8, `AudioUtility::GetPiecewiseLinear`),
  `ProcessDummyRCCar` 0x500E97, `ProcessDummyGolfCart` (tabla 0x8CBFD8, doble 0x859068), `ProcessBoatMovingOverWater` (NaN).

## Pares por funcion
| Grupo | Funcion | Direccion | Pares |
|---|---|---|---|
| G1 | Constructor | 0x4F63E0 | 64 |
| G1 | Destructor | 0x6D0A10 | 128 |
| G1 | StaticInitialise | 0x5B99F0 | 64 |
| G1 | Initialise | 0x4F7670 | 288 |
| G1 | Terminate | 0x4FB8C0 | 192 |
| G1 | StaticGetPlayerVehicleAudioSettingsForRadio | 0x4F4ED0 | 64 |
| G1 | StaticService | 0x4F4EC0 | 64 |
| G1 | EnableHelicoptors | 0x4F4EF0 | 64 |
| G1 | DisableHelicoptors | 0x4F4EE0 | 64 |
| G1 | EnableHelicoptor | 0x4F5C00 | 64 |
| G1 | DisableHelicoptor | 0x4F5BF0 | 64 |
| G1 | DemandBankSlot | 0x4F4E60 | 128 |
| G1 | RequestBankSlot | 0x4F4D10 | 192 |
| G1 | StoppedUsingBankSlot | 0x4F4DF0 | 96 |
| G1 | DoesBankSlotContainThisBank | 0x4F4E30 | 96 |
| G1 | CopHeli | 0x4F5C40 | 64 |
| G1 | GetVehicleAudioSettings | 0x4F5C10 | 232 |
| G1 | GetVehicleTypeForAudio | 0x4F4F00 | 256 |
| G1 | GetFlyingMetalVolume | 0x4F6150 | 128 |
| G1 | IsAccInhibited | 0x4F4F70 | 160 |
| G1 | IsAccInhibitedBackwards | 0x4F4FC0 | 80 |
| G1 | IsAccInhibitedForLowSpeed | 0x4F4FF0 | 80 |
| G1 | IsAccInhibitedForTime | 0x4F5020 | 64 |
| G1 | InhibitAccForTime | 0x4F5030 | 64 |
| G1 | InhibitCrzForTime | 0x4F5060 | 64 |
| G1 | IsCrzInhibitedForTime | 0x4F5050 | 64 |
| G1 | GetAccelAndBrake | 0x4F5080 | 256 |
| G1 | UpdateGasPedalAudio | 0x4F5EB0 | 160 |
| G1 | TurnOnRadioForVehicle | 0x4F5B20 | 64 |
| G1 | TurnOffRadioForVehicle | 0x4F5B60 | 64 |
| G1 | PlayerAboutToExitVehicleAsDriver | 0x4F5BA0 | 64 |
| G1 | JustFinishedAccelerationLoop | 0x4F5E50 | 128 |
| G2 | GetFreqForIdle | 0x4F5C60 | 64 |
| G2 | GetFrequencyForDummyIdle | 0x4F5310 | 64 |
| G2 | GetFrequencyForDummyRev | 0x4F54F0 | 64 |
| G2 | GetVolumeForDummyIdle | 0x4F51F0 | 96 |
| G2 | GetVolumeForDummyRev | 0x4F53D0 | 96 |
| G2 | GetFreqForPlayerEngineSound | 0x4F8070 | 192 |
| G2 | GetVolForPlayerEngineSound | 0x4F5D00 | 128 |
| G2 | UpdateParameters | 0 | 192 |
| G2 | StartVehicleEngineSound | 0x4F7F20 | 64 |
| G2 | CancelVehicleEngineSound | 0x4F55C0 | 96 |
| G2 | StopGenericEngineSound | 0x4F6320 | 64 |
| G2 | RequestNewPlayerCarEngineSound | 0x4F7A50 | 256 |
| G2 | UpdateGenericVehicleSound | 0x4FAD40 | 96 |
| G2 | JustWreckedVehicle | 0x4FD0B0 | 96 |
| G2 | JustGotOutOfVehicleAsDriver | 0x4FCF40 | 96 |
| G2 | JustGotInVehicleAsDriver | 0x4F5700 | 192 |
| G2 | AddAudioEvent-0 | 0x4F6420 | 768 |
| G2 | AddAudioEvent-1 | 0x4F7580 | 96 |
| G2 | ProcessDummyStateTransition | 0x4FCA10 | 256 |
| G2 | ProcessDummyVehicleEngine | 0x501480 | 192 |
| G2 | ProcessEngineDamage | 0x4FAE20 | 160 |
| G2 | ProcessNitro | 0x4FB070 | 160 |
| G2 | ProcessMovingParts | 0x4FB260 | 160 |
| G3 | GetHornState- | 0x4F61E0 | 192 |
| G3 | GetSirenState | 0x4F62A0 | 160 |
| G3 | PlayHornOrSiren | 0x4F99D0 | 352 |
| G3 | ProcessVehicleSirenAlarmHorn | 0x5002C0 | 320 |
| G3 | PlaySkidSound | 0x4F8360 | 256 |
| G3 | ProcessVehicleSkidding | 0x4F8F10 | 448 |
| G3 | GetVehicleDriveWheelSkidValue | 0x4F5F30 | 192 |
| G3 | GetVehicleNonDriveWheelSkidValue | 0x4F6000 | 192 |
| G3 | PlayFlatTyreSound | 0x4F8650 | 160 |
| G3 | PlayReverseSound | 0x4F87D0 | 160 |
| G3 | PlayRoadNoiseSound | 0x4F84D0 | 160 |
| G3 | ProcessVehicleFlatTyre | 0x4F8940 | 224 |
| G3 | ProcessVehicleRoadNoise | 0x4F8B00 | 256 |
| G3 | ProcessReverseGear | 0x4F8DF0 | 224 |
| G3 | ProcessRainOnVehicle | 0x4F92C0 | 192 |
| G4 | PlayAircraftSound | 0x4F93C0 | 256 |
| G4 | GetAircraftNearPosition | 0x4F96A0 | 128 |
| G4 | ProcessAircraft | 0x501C50 | 384 |
| G4 | ProcessPlayerProp | 0x4FD290 | 256 |
| G4 | ProcessDummyProp | 0x4FD8F0 | 256 |
| G4 | ProcessAIProp | 0x4FDFD0 | 256 |
| G4 | ProcessPlayerHeli | 0x4FE420 | 192 |
| G4 | ProcessDummyHeli | 0x4FE940 | 192 |
| G4 | ProcessAIHeli | 0x4FEE20 | 256 |
| G4 | ProcessPlayerSeaPlane | 0x4FF320 | 192 |
| G4 | ProcessDummySeaPlane | 0x4FF7C0 | 128 |
| G4 | ProcessPlayerJet | 0x501650 | 256 |
| G4 | ProcessDummyJet | 0x501960 | 256 |
| G4 | ProcessDummyRCPlane | 0x4FA7C0 | 192 |
| G4 | ProcessDummyRCHeli | 0x4FAA80 | 192 |
| G5 | UpdateBoatSound | 0x4F9E90 | 256 |
| G5 | ProcessBoatEngine | 0x5003A0 | 320 |
| G5 | ProcessBoatMovingOverWater | 0x4FA0C0 | 192 |
| G5 | UpdateTrainSound | 0x4FA1C0 | 256 |
| G5 | ProcessTrainTrackSound | 0x4FA3F0 | 256 |
| G5 | ProcessDummyTrainEngine | 0x500710 | 384 |
| G5 | ProcessPlayerTrainBrakes | 0x500AB0 | 256 |
| G5 | PlayTrainBrakeSound | 0x4FA630 | 192 |
| G5 | PlayBicycleSound | 0x4F9710 | 256 |
| G5 | GetBaseVolumeForBicycleTyre | 0x4F60B0 | 128 |
| G5 | ProcessDummyRCCar | 0x500DC0 | 192 |
| G5 | ProcessPlayerCombine | 0x500CE0 | 128 |
| G5 | ProcessDummyGolfCart | 0x501270 | 192 |
| G5 | ProcessSpecialVehicle | 0x501AB0 | 384 |
| G6 | ProcessPlayerVehicleEngine | 0x4FBB10 | 1200 |
| G6 | ProcessVehicle | 0x501E10 | 640 |
| G6 | Service | 0x502280 | 640 |

Total: 20024 pares.

## Fuera del banco / fuera de dominio (por funcion)
- `PlayAircraftSound` con tipo 0, 6 o >7: el EXE (0x4F9602) llama a `RequestNewSound` con un `CAESound` local sin construir.
- `Cancel/GetVol/GetFreq/RequestNewPlayerCarEngineSound` con tipo fuera del rango (4F55E3, 4F5D09, 4F81A5, 4F7AF5): el EXE lee pila sin inicializar.
- `ProcessPlayerTrainBrakes` con un tren que no sea del jugador: 0x500B87 lee un local sin inicializar.
- Ramas `Enabled=false` de los derrapes (0x8CBD80/81): `s_Config` es protegido.
- `UpdateParameters` con helicoptero y sonido cercano (usa `GetComponentWorldPosition` real).

## Aprobación (10/10/2026)

Corrida `20261010-151711-778299`: 102 funciones, 20024 pares, cero diferencias; passed/all_equal/state_restored=true, saves/configuraciones intactos. Reauditoría obligatoria sin registros pasantes nulos.

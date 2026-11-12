# Funciones pendientes del EXE, por clase

Generado de `docs/JuegoExe/funciones.csv` (12/11/2026). Total: **1,612** funciones en 134 clases/módulos (rango 0x401000–0x74FFFF): 1,590 sin reemplazar (A–D) + 22 envolturas pendientes. Ordenado de menor a mayor por tamaño (bytes pendientes de la clase) y, a igualdad, por número de funciones; dentro de cada clase, de menor a mayor tamaño de función. Se resume en la [sección 11 de ReversedClasses](../ReversedClasses.md#parciales).

Estados: **A** llamada desde código ya en C++ · **B** método virtual · **C** solo la llaman funciones pendientes · **D** sin llamadas ni vtable · **envoltura** ya tiene hook pero llama al EXE.

Aviso: la clase se atribuye por cercanía de direcciones y puede ser errónea cuando la confianza es baja; un nombre de catálogo sí es fiable.

## Índice

| Clase | Funciones | Bytes |
|---|---:|---:|
| CAudioZones | 1 | 139 |
| IKChain_c | 2 | 205 |
| CWaterLevel | 1 | 208 |
| WaterCreature_c | 2 | 211 |
| Input | 2 | 302 |
| CTaskSimpleHoldEntity | 8 | 302 |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | 3 | 304 |
| CTaskInteriorSitInChair | 4 | 304 |
| FurnitureManager_c | 3 | 306 |
| CAEWaterCannonAudioEntity | 2 | 309 |
| CPedDamageResponseCalculator | 3 | 319 |
| CTaskComplexHitPedWithCar | 4 | 319 |
| CStats | 2 | 320 |
| CTaskComplexOpenPassengerDoor | 4 | 327 |
| CTaskGoToVehicleAndLean | 3 | 331 |
| CTaskComplexDieInCar | 13 | 333 |
| CCopPed | 8 | 335 |
| CEventAcquaintancePedHateBadlyLit | 7 | 337 |
| CPedGeometryAnalyser | 4 | 338 |
| InteriorManager_c | 2 | 340 |
| CQuaternion | 3 | 341 |
| CPlane | 3 | 346 |
| CWeather | 2 | 352 |
| CTaskAllocatorPool | 7 | 362 |
| CTaskComplexGangFollower | 3 | 365 |
| CEventPlayerCommandToGroupGather | 8 | 372 |
| CColStore | 5 | 373 |
| CTaskComplexGoToPointAndStandStillAndAchieveHeading | 2 | 382 |
| VideoMode | 2 | 382 |
| CTaskComplexWalkRoundFire | 9 | 385 |
| CHandObject | 4 | 386 |
| CAEAmbienceTrackManager | 5 | 386 |
| CTaskSimpleCarFallOut | 5 | 388 |
| CTaskComplexGotoDoorAndOpen | 6 | 388 |
| CTxdStore | 6 | 388 |
| CEventKnockOffBike | 7 | 395 |
| CPopulation | 8 | 399 |
| CTaskSimpleDieInCar | 5 | 403 |
| CVehicleRecording | 3 | 417 |
| CTaskComplexWalkRoundBuildingAttempt | 4 | 421 |
| CTheZones | 5 | 422 |
| CTaskSimpleIKPointArm | 5 | 426 |
| CGame | 9 | 428 |
| CTaskSimpleHitFromFront | 21 | 430 |
| CAnimManager | 6 | 434 |
| Fx_c | 9 | 434 |
| CEventPotentialWalkIntoFire | 9 | 440 |
| CTaskComplexWanderCop | 9 | 442 |
| CObjectPool | 5 | 446 |
| COnscreenCounterEntry | 3 | 450 |
| CTaskComplexSeekEntityAiming | 6 | 451 |
| CTaskComplexPartnerGreet | 9 | 458 |
| Game | 3 | 460 |
| CShopping | 5 | 460 |
| CTaskComplexStealCar | 6 | 461 |
| CPlantMgr | 3 | 484 |
| CTaskSimpleDuckToggle | 3 | 488 |
| ProcSurfaceInfo_c | 4 | 489 |
| CEventAttractor | 8 | 490 |
| CPlayerPed | 3 | 496 |
| CTaskSimpleDuck | 8 | 496 |
| CEventScriptedAttractor | 13 | 527 |
| CPedAttractorPool | 12 | 530 |
| CStreaming | 17 | 535 |
| CStencilShadows | 5 | 543 |
| CTaskComplexRoadRage | 9 | 585 |
| CEventEditableResponse | 14 | 607 |
| Rs | 4 | 661 |
| CEntryExitManager | 11 | 675 |
| CTaskComplexFallAndGetUp | 13 | 719 |
| CQuadTreeNode<T> | 10 | 720 |
| CObject | 7 | 728 |
| CEventScriptCommand | 11 | 766 |
| CTaskSimpleThrowProjectile | 11 | 767 |
| CTaskComplexUseWaterCannon | 15 | 769 |
| CTrafficLights | 5 | 770 |
| CTaskComplexWaitForPed | 6 | 772 |
| CTaskComplexWander | 4 | 797 |
| CModelInfo | 22 | 814 |
| CTaskComplexInvestigateDisturbance | 7 | 858 |
| CVehicle | 33 | 861 |
| CVehicleSaveStructure | 4 | 874 |
| Ps | 6 | 876 |
| CPad | 11 | 895 |
| CEventSeenCop | 35 | 902 |
| CTaskComplexSunbathe | 6 | 935 |
| CEntityScanner | 5 | 958 |
| CTaskComplexMedicTreatInjuredPed | 13 | 974 |
| CTaskComplexGoToPointAndStandStill | 8 | 977 |
| CTaskSimpleBeHit | 8 | 1022 |
| CEventVehicleHitAndRun | 15 | 1037 |
| CPed | 6 | 1046 |
| CCam | 7 | 1059 |
| CTaskComplexStuckInAir | 11 | 1075 |
| CTrain | 6 | 1076 |
| CTaskComplexSeekEntityAnyMeansXYOffset | 4 | 1138 |
| CPlaceable | 24 | 1138 |
| CEventVehicleOnFire | 17 | 1145 |
| CEventVehicleCollision | 15 | 1149 |
| CTaskSimpleRunTimedAnim | 12 | 1175 |
| Interior_c | 4 | 1200 |
| CAttractorScanner | 3 | 1273 |
| CClothesBuilder | 13 | 1285 |
| CVehicleModelInfo | 15 | 1356 |
| CTaskComplexAvoidOtherPedWhileWandering | 10 | 1382 |
| CTaskInteriorUseInfo | 3 | 1483 |
| CPedIntelligencePool | 6 | 1552 |
| CEventDamage | 22 | 1596 |
| CTaskComplexWanderGang | 6 | 1750 |
| CPedAttractor | 28 | 1774 |
| CTaskSimpleThrowControl | 14 | 1802 |
| CTaskComplexUseClosestFreeScriptedAttractor | 13 | 1858 |
| CCarCtrl | 4 | 1928 |
| CPathFind | 14 | 2141 |
| SurfaceInfos_c | 2 | 2174 |
| CCullZones | 13 | 2476 |
| CAEUserRadioTrackManager | 30 | 2533 |
| CTaskComplexTurnToFaceEntityOrCoord | 14 | 2607 |
| CTaskComplexGoToPointAnyMeans | 6 | 2633 |
| CAutomobile | 10 | 2917 |
| CCustomCarEnvMapPipeline | 25 | 3547 |
| CPools | 52 | 3733 |
| CReplay | 10 | 4172 |
| CRunningScript | 4 | 5919 |
| RpAnimBlend | 16 | 6102 |
| CScriptsForBrains | 4 | 6307 |
| FxInfoManager_c | 116 | 6591 |
| CPedAttractorManager | 71 | 8097 |
| CTaskComplexSeekEntityRadiusAngleOffset | 5 | 8771 |
| CEntity | 42 | 9537 |
| CTheScripts | 14 | 9864 |
| CWeaponModelInfo | 41 | 13554 |
| Win | 85 | 19907 |
| FxEmitterBP_c | 141 | 28395 |
| **TOTAL (134 clases / módulos)** | **1,612** | **228,776** |

## CAudioZones (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00509230 | 139 | D |  | 0 | 0 |  | baja |

## IKChain_c (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00617FC0 | 102 | C |  | 1 | 0 |  | baja |
| 0x00618030 | 103 | C |  | 1 | 0 |  | baja |

## CWaterLevel (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006E67A0 | 208 | D |  | 0 | 0 |  | alta |

## WaterCreature_c (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006E3EB0 | 105 | C |  | 1 | 0 |  | baja |
| 0x006E3F20 | 106 | C |  | 1 | 0 |  | baja |

## Input (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x007448B0 | 123 | A |  | 1 | 1 |  | alta |
| 0x00744930 | 179 | A |  | 1 | 1 |  | alta |

## CTaskSimpleHoldEntity (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006935B0 | 5 | C |  | 1 | 0 |  | baja |
| 0x006935E0 | 5 | C |  | 1 | 0 |  | baja |
| 0x00691460 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00693590 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006935C0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006935F0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006939D0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00691630 | 166 | A |  | 1 | 1 |  | alta |

## CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0064B5B0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00646FE0 | 116 | A |  | 9 | 6 |  | baja |
| 0x00646F40 | 158 | A |  | 1 | 1 |  | baja |

## CTaskInteriorSitInChair (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00675C80 | 6 | D |  | 0 | 0 |  | baja |
| 0x00675C90 | 93 | C |  | 1 | 0 |  | baja |
| 0x00675D60 | 99 | D |  | 0 | 0 |  | baja |
| 0x00675CF0 | 106 | D |  | 0 | 0 |  | baja |

## FurnitureManager_c (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00591360 | 76 | D |  | 0 | 0 |  | baja |
| 0x005912E0 | 113 | C |  | 1 | 0 |  | baja |
| 0x00591260 | 117 | C |  | 1 | 0 |  | baja |

## CAEWaterCannonAudioEntity (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00728AB0 | 86 | C |  | 1 | 0 |  | baja |
| 0x00502F80 | 223 | A |  | 4 | 4 |  | alta |

## CPedDamageResponseCalculator (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AD800 | 46 | D |  | 0 | 0 |  | baja |
| 0x004B5CF0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B5C2A | 197 | D |  | 0 | 0 |  | baja |

## CTaskComplexHitPedWithCar (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006563B0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00657B60 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00653AA0 | 51 | D |  | 0 | 0 |  | alta |
| 0x006563D0 | 208 | A |  | 1 | 1 |  | baja |

## CStats (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00558F90 | 6 | A |  | 2 | 2 |  | alta |
| 0x0055D0E0 | 314 | A |  | 1 | 1 |  | baja |

## CTaskComplexOpenPassengerDoor (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006404C0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x006404D0 | 90 | A |  | 1 | 1 |  | baja |
| 0x00643950 | 100 | B |  | 0 | 0 | sí | baja |
| 0x00640430 | 131 | A |  | 5 | 4 |  | baja |

## CTaskGoToVehicleAndLean (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00660ED0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x006648A0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00664770 | 295 | A |  | 2 | 2 |  | baja |

## CTaskComplexDieInCar (13)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0062FF50 | 5 | B |  | 0 | 0 | sí | baja |
| 0x0062FF00 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0062FF40 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0062FF10 | 11 | C |  | 1 | 0 |  | baja |
| 0x0062FF60 | 11 | C |  | 1 | 0 |  | baja |
| 0x0062FF20 | 18 | D |  | 0 | 0 |  | baja |
| 0x006378D0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006378F0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637910 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637930 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637950 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006379D0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637970 | 96 | B |  | 0 | 0 | sí | baja |

## CCopPed (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005DE300 | 11 | C |  | 1 | 0 |  | baja |
| 0x005DE3B0 | 11 | C |  | 1 | 0 |  | baja |
| 0x005DE2E0 | 18 | D |  | 0 | 0 |  | baja |
| 0x005DE2C0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x005DE310 | 30 | D |  | 0 | 0 |  | baja |
| 0x005DE3E0 | 30 | D |  | 0 | 0 |  | baja |
| 0x005DE340 | 97 | A |  | 6 | 2 |  | baja |
| 0x005DE400 | 108 | D |  | 0 | 0 |  | baja |

## CEventAcquaintancePedHateBadlyLit (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005FF350 | 3 | B |  | 0 | 0 | sí | baja |
| 0x005FF2D0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FF2C0 | 11 | C |  | 2 | 0 |  | baja |
| 0x005FF360 | 30 | B |  | 0 | 0 | sí | baja |
| 0x004AFBF0 | 81 | C |  | 1 | 0 |  | baja |
| 0x004AFB50 | 102 | C |  | 1 | 0 |  | baja |
| 0x005FF2E0 | 104 | B |  | 0 | 0 | sí | baja |

## CPedGeometryAnalyser (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005F6300 | 30 | D |  | 0 | 0 |  | baja |
| 0x005F6200 | 90 | A |  | 9 | 4 |  | baja |
| 0x005F6190 | 108 | A |  | 5 | 4 |  | baja |
| 0x005F6280 | 110 | D |  | 0 | 0 |  | baja |

## InteriorManager_c (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00598580 | 151 | C |  | 1 | 0 |  | alta |
| 0x005984C0 | 189 | C |  | 1 | 0 |  | alta |

## CQuaternion (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0059CBA0 | 7 | C |  | 1 | 0 |  | baja |
| 0x0059CD50 | 27 | D |  | 0 | 0 |  | baja |
| 0x0059CA10 | 307 | A |  | 1 | 0 |  | baja |

## CPlane (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006CADB0 | 30 | B |  | 0 | 0 | sí | alta |
| 0x006CDC30 | 85 | A |  | 1 | 1 |  | baja |
| 0x006C9160 | 231 | C |  | 1 | 0 |  | alta |

## CWeather (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0072A620 | 17 | C |  | 1 | 0 |  | alta |
| 0x0072A640 | 335 | A |  | 3 | 3 |  | alta |

## CTaskAllocatorPool (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0069DB40 | 11 | C |  | 1 | 0 |  | baja |
| 0x0069DAC0 | 56 | B |  | 0 | 0 | sí | baja |
| 0x0069D9C0 | 59 | B |  | 0 | 0 | sí | baja |
| 0x0069DA00 | 59 | B |  | 0 | 0 | sí | baja |
| 0x0069DA40 | 59 | B |  | 0 | 0 | sí | baja |
| 0x0069DA80 | 59 | B |  | 0 | 0 | sí | baja |
| 0x0069DB00 | 59 | B |  | 0 | 0 | sí | baja |

## CTaskComplexGangFollower (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0065EBA0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x006634E0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0065ED40 | 329 | A |  | 2 | 2 |  | alta |

## CEventPlayerCommandToGroupGather (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00609270 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00609310 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00609320 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00609330 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00609300 | 11 | C |  | 1 | 0 |  | baja |
| 0x006092E0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00609280 | 90 | B |  | 0 | 0 | sí | baja |
| 0x006093B0 | 217 | C |  | 1 | 0 |  | baja |

## CColStore (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00410610 | 54 | D |  | 0 | 0 |  | alta |
| 0x00410FE0 | 54 | D |  | 0 | 0 |  | alta |
| 0x004110E0 | 71 | D |  | 0 | 0 |  | alta |
| 0x00410F80 | 91 | A |  | 1 | 1 |  | alta |
| 0x00411030 | 103 | A |  | 1 | 1 |  | alta |

## CTaskComplexGoToPointAndStandStillAndAchieveHeading (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0066E050 | 90 | A |  | 1 | 1 |  | baja |
| 0x00668D50 | 292 | A |  | 4 | 4 |  | alta |

## VideoMode (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00745DC0 | 130 | C |  | 1 | 0 |  | baja |
| 0x00745CC0 | 252 | C |  | 1 | 0 |  | baja |

## CTaskComplexWalkRoundFire (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00655DD0 | 5 | C |  | 1 | 0 |  | baja |
| 0x00655E00 | 5 | C |  | 1 | 0 |  | baja |
| 0x00655DB0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00655DE0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00655E30 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00655E80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006586A0 | 42 | A |  | 1 | 1 |  | baja |
| 0x006586D0 | 102 | D |  | 0 | 0 |  | baja |
| 0x006557A0 | 111 | B |  | 0 | 0 | sí | baja |

## CHandObject (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0059F000 | 5 | C |  | 1 | 0 |  | baja |
| 0x0059EFE0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0059F050 | 54 | A |  | 1 | 1 |  | baja |
| 0x0059EEB0 | 297 | A |  | 6 | 6 |  | baja |

## CAEAmbienceTrackManager (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004D6CC0 | 40 | A |  | 5 | 5 |  | alta |
| 0x004D7910 | 64 | A |  | 4 | 2 |  | baja |
| 0x004D6DA0 | 70 | A |  | 1 | 1 |  | alta |
| 0x004D6DF0 | 97 | A |  | 1 | 1 |  | alta |
| 0x004D7890 | 115 | A |  | 2 | 2 |  | baja |

## CTaskSimpleCarFallOut (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0064CD60 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0064CD80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0064CE80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00648E90 | 80 | A |  | 2 | 2 |  | alta |
| 0x0064CDA0 | 218 | A |  | 3 | 3 |  | baja |

## CTaskComplexGotoDoorAndOpen (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0066BB90 | 6 | B |  | 0 | 0 | sí | alta |
| 0x0066C4C0 | 11 | C |  | 1 | 0 |  | baja |
| 0x0066BC80 | 28 | B |  | 0 | 0 | sí | alta |
| 0x0066C3E0 | 86 | A |  | 3 | 1 |  | baja |
| 0x0066C450 | 106 | C |  | 3 | 0 |  | baja |
| 0x0066C280 | 151 | D |  | 0 | 0 |  | baja |

## CTxdStore (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x007316D0 | 13 | A |  | 1 | 1 |  | alta |
| 0x00731800 | 54 | D |  | 0 | 0 |  | alta |
| 0x00731B30 | 54 | D |  | 0 | 0 |  | alta |
| 0x00731C20 | 71 | D |  | 0 | 0 |  | alta |
| 0x00731AD0 | 92 | A |  | 1 | 1 |  | alta |
| 0x00731B80 | 104 | A |  | 2 | 2 |  | alta |

## CEventKnockOffBike (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AFCC0 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004B06F0 | 3 | B |  | 0 | 0 | sí | baja |
| 0x004AFCB0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004AFCE0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004AFCD0 | 7 | B |  | 0 | 0 | sí | alta |
| 0x004B0810 | 138 | D |  | 0 | 0 |  | baja |
| 0x004B4EE0 | 232 | D |  | 0 | 0 |  | baja |

## CPopulation (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00407000 | 26 | A |  | 1 | 1 |  | baja |
| 0x005BD510 | 43 | D |  | 0 | 0 |  | baja |
| 0x005BD540 | 43 | D |  | 0 | 0 |  | baja |
| 0x005BD570 | 43 | D |  | 0 | 0 |  | baja |
| 0x005BD5A0 | 43 | D |  | 0 | 0 |  | baja |
| 0x005BD5D0 | 43 | D |  | 0 | 0 |  | baja |
| 0x005BD4E0 | 45 | D |  | 0 | 0 |  | baja |
| 0x005BD420 | 113 | A |  | 1 | 1 |  | baja |

## CTaskSimpleDieInCar (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0062FCB0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0062FC80 | 41 | D |  | 0 | 0 |  | baja |
| 0x00636010 | 80 | B |  | 0 | 0 | sí | baja |
| 0x00635F90 | 124 | B |  | 0 | 0 | sí | baja |
| 0x00636060 | 152 | B |  | 0 | 0 | sí | baja |

## CVehicleRecording (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0045AF40 | 47 | A |  | 4 | 2 |  | baja |
| 0x0045AFB0 | 54 | A |  | 5 | 1 |  | baja |
| 0x0045AD40 | 316 | C |  | 1 | 0 |  | baja |

## CTaskComplexWalkRoundBuildingAttempt (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00657200 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00657750 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00655C10 | 104 | B |  | 0 | 0 | sí | baja |
| 0x00654840 | 257 | A |  | 4 | 4 |  | alta |

## CTheZones (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00572E70 | 24 | A |  | 1 | 1 |  | baja |
| 0x00572DB0 | 26 | A |  | 3 | 3 |  | baja |
| 0x00572130 | 69 | A |  | 3 | 3 |  | alta |
| 0x005D3020 | 149 | A |  | 1 | 1 |  | baja |
| 0x005D30C0 | 154 | A |  | 1 | 1 |  | baja |

## CTaskSimpleIKPointArm (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00634610 | 5 | B |  | 0 | 0 | sí | baja |
| 0x006344F0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00634620 | 76 | B |  | 0 | 0 | sí | baja |
| 0x00634440 | 163 | C |  | 5 | 0 |  | baja |
| 0x00634560 | 176 | B |  | 0 | 0 | sí | baja |

## CGame (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00730450 | 25 | D |  | 0 | 0 |  | baja |
| 0x0053BC60 | 26 | A |  | 1 | 1 |  | alta |
| 0x0053C860 | 39 | D |  | 0 | 0 |  | alta |
| 0x0053C890 | 39 | D |  | 0 | 0 |  | alta |
| 0x0072FEC0 | 46 | D |  | 0 | 0 |  | baja |
| 0x007301D0 | 46 | D |  | 0 | 0 |  | baja |
| 0x0072FEF0 | 66 | D |  | 0 | 0 |  | baja |
| 0x00730200 | 66 | D |  | 0 | 0 |  | baja |
| 0x007304B0 | 75 | A |  | 1 | 1 |  | baja |

## CTaskSimpleHitFromFront (21)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00637320 | 5 | C |  | 1 | 0 |  | baja |
| 0x00637350 | 5 | C |  | 1 | 0 |  | baja |
| 0x00637380 | 5 | C |  | 1 | 0 |  | baja |
| 0x006373B0 | 5 | C |  | 1 | 0 |  | baja |
| 0x006373E0 | 5 | C |  | 1 | 0 |  | baja |
| 0x00637410 | 5 | C |  | 1 | 0 |  | baja |
| 0x00637440 | 5 | C |  | 1 | 0 |  | baja |
| 0x00637470 | 5 | C |  | 1 | 0 |  | baja |
| 0x00637300 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637330 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637360 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637390 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006373C0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006373F0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637420 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637450 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637480 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006374A0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006374C0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006374E0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637500 | 30 | B |  | 0 | 0 | sí | baja |

## CAnimManager (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004D40A0 | 12 | D |  | 0 | 0 |  | alta |
| 0x004D59E0 | 23 | A |  | 2 | 2 |  | baja |
| 0x004D59B0 | 40 | C |  | 1 | 0 |  | baja |
| 0x004D5980 | 46 | C |  | 1 | 0 |  | baja |
| 0x004D40C0 | 64 | A |  | 2 | 2 |  | alta |
| 0x004D56F0 | 249 | C |  | 2 | 0 |  | baja |

## Fx_c (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0049EA70 | 5 | A |  | 3 | 0 |  | alta |
| 0x0049EA80 | 12 | D |  | 0 | 0 |  | alta |
| 0x0049EAE0 | 21 | A |  | 1 | 1 |  | alta |
| 0x004A1320 | 26 | A |  | 1 | 1 |  | baja |
| 0x004A11C0 | 27 | D |  | 0 | 0 |  | baja |
| 0x004A12D0 | 57 | A |  | 1 | 0 |  | baja |
| 0x0049EA90 | 66 | A |  | 1 | 1 |  | alta |
| 0x004A1280 | 67 | A |  | 1 | 1 |  | baja |
| 0x004A11E0 | 153 | A |  | 1 | 1 |  | baja |

## CEventPotentialWalkIntoFire (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B1EE0 | 3 | B |  | 0 | 0 | sí | baja |
| 0x004B1ED0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B1EF0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B1F90 | 45 | D |  | 0 | 0 |  | baja |
| 0x004B6990 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B69E0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6A30 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6A80 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6AD0 | 76 | B |  | 0 | 0 | sí | baja |

## CTaskComplexWanderCop (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00460CD0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00460D50 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00674ED0 | 11 | C |  | 3 | 0 |  | baja |
| 0x00460D60 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00674F90 | 30 | D |  | 0 | 0 |  | baja |
| 0x00674EA0 | 48 | A |  | 2 | 2 |  | baja |
| 0x00460D80 | 87 | A |  | 2 | 1 |  | baja |
| 0x00460CE0 | 102 | B |  | 0 | 0 | sí | baja |
| 0x00674F00 | 122 | D |  | 0 | 0 |  | baja |

## CObjectPool (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005A1EF0 | 36 | A |  | 2 | 2 |  | baja |
| 0x004048E0 | 41 | A |  | 297 | 1 |  | baja |
| 0x005A1F20 | 49 | A |  | 33 | 2 |  | baja |
| 0x005A1D10 | 90 | A |  | 5 | 4 |  | baja |
| 0x005A1DF0 | 230 | A |  | 2 | 2 |  | baja |

## COnscreenCounterEntry (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0044CD30 | 25 | A |  | 1 | 1 |  | baja |
| 0x0044CB10 | 167 | A |  | 1 | 0 |  | baja |
| 0x0044CC20 | 258 | A |  | 1 | 1 |  | baja |

## CTaskComplexSeekEntityAiming (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00695BC0 | 30 | B |  | 0 | 0 | sí | alta |
| 0x00695BE0 | 30 | B |  | 0 | 0 | sí | alta |
| 0x00695C00 | 30 | B |  | 0 | 0 | sí | alta |
| 0x00695890 | 96 | B |  | 0 | 0 | sí | alta |
| 0x00695DC0 | 113 | C |  | 1 | 0 |  | baja |
| 0x00695B20 | 152 | D |  | 0 | 0 |  | alta |

## CTaskComplexPartnerGreet (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00681E10 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00681E20 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00684F60 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00684F80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00684FA0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00684FC0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00684FE0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00684ED0 | 142 | B |  | 0 | 0 | sí | baja |
| 0x00684E30 | 154 | B |  | 0 | 0 | sí | baja |

## Game (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005BF3D0 | 44 | D |  | 0 | 0 |  | baja |
| 0x0053E8D0 | 63 | A |  | 3 | 3 |  | alta |
| 0x0053E600 | 353 | A |  | 2 | 2 |  | alta |

## CShopping (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0049C3B0 | 28 | A |  | 1 | 1 |  | baja |
| 0x0049C440 | 54 | D |  | 0 | 0 |  | baja |
| 0x0049ACD0 | 74 | C |  | 1 | 0 |  | alta |
| 0x0049BDD0 | 120 | C |  | 1 | 0 |  | alta |
| 0x0049ABD0 | 184 | A |  | 2 | 2 |  | alta |

## CTaskComplexStealCar (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0063B960 | 6 | B |  | 0 | 0 | sí | baja |
| 0x006410E0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00642250 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0063D9E0 | 116 | B |  | 0 | 0 | sí | baja |
| 0x0063BAE0 | 122 | A |  | 2 | 2 |  | baja |
| 0x0063BA00 | 157 | A |  | 3 | 3 |  | baja |

## CPlantMgr (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005DD1E0 | 57 | A |  | 1 | 0 |  | baja |
| 0x005DBE40 | 104 | A |  | 2 | 2 |  | alta |
| 0x005DD220 | 323 | A |  | 2 | 2 |  | baja |

## CTaskSimpleDuckToggle (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00635E60 | 142 | B |  | 0 | 0 | sí | baja |
| 0x00635CF0 | 168 | B |  | 0 | 0 | sí | baja |
| 0x00635DA0 | 178 | B |  | 0 | 0 | sí | baja |

## ProcSurfaceInfo_c (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005A3070 | 12 | D |  | 0 | 0 |  | baja |
| 0x005A3090 | 122 | C |  | 1 | 0 |  | baja |
| 0x005A3E20 | 126 | C |  | 1 | 0 |  | baja |
| 0x005A2F80 | 229 | A |  | 1 | 1 |  | baja |

## CEventAttractor (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AF3E0 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004AF3F0 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004AF3D0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004B76A0 | 33 | B |  | 0 | 0 | sí | baja |
| 0x004B74B0 | 97 | B |  | 0 | 0 | sí | baja |
| 0x004B7520 | 97 | D |  | 0 | 0 |  | baja |
| 0x004B7630 | 97 | D |  | 0 | 0 |  | baja |
| 0x004B7590 | 154 | B |  | 0 | 0 | sí | baja |

## CPlayerPed (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0060F8C0 | 1 | A |  | 1 | 1 |  | baja |
| 0x0060A9A0 | 30 | B |  | 0 | 0 | sí | alta |
| 0x0060D5B0 | 465 | A |  | 3 | 3 |  | alta |

## CTaskSimpleDuck (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006926E0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x006926F0 | 11 | C |  | 1 | 0 |  | baja |
| 0x00692DD0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00693B80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00694640 | 68 | B |  | 0 | 0 | sí | baja |
| 0x00692680 | 85 | C |  | 1 | 0 |  | baja |
| 0x00692D60 | 99 | B |  | 0 | 0 | sí | baja |
| 0x00692700 | 167 | B |  | 0 | 0 | sí | baja |

## CEventScriptedAttractor (13)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005FEF80 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FEF70 | 11 | C |  | 2 | 0 |  | baja |
| 0x005FF030 | 11 | C |  | 3 | 0 |  | baja |
| 0x005FF0F0 | 11 | C |  | 2 | 0 |  | baja |
| 0x005FF1B0 | 11 | C |  | 3 | 0 |  | baja |
| 0x005FEFF0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x005FF0B0 | 30 | D |  | 0 | 0 |  | baja |
| 0x005FF170 | 30 | D |  | 0 | 0 |  | baja |
| 0x005FF230 | 30 | D |  | 0 | 0 |  | baja |
| 0x005FF050 | 88 | D |  | 0 | 0 |  | baja |
| 0x005FF110 | 88 | D |  | 0 | 0 |  | baja |
| 0x005FF1D0 | 88 | D |  | 0 | 0 |  | baja |
| 0x005FEF90 | 93 | B |  | 0 | 0 | sí | baja |

## CPedAttractorPool (12)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005EAAA0 | 30 | A |  | 1 | 0 |  | baja |
| 0x005EAAF0 | 30 | A |  | 1 | 0 |  | baja |
| 0x005EAB10 | 30 | A |  | 1 | 0 |  | baja |
| 0x005EAE10 | 37 | A |  | 3 | 0 |  | baja |
| 0x005EAE70 | 37 | A |  | 3 | 0 |  | baja |
| 0x005EAEE0 | 37 | A |  | 3 | 0 |  | baja |
| 0x005EAAC0 | 41 | A |  | 1 | 0 |  | baja |
| 0x005EAE40 | 48 | A |  | 3 | 0 |  | baja |
| 0x005EAEA0 | 57 | A |  | 3 | 0 |  | baja |
| 0x005EACC0 | 59 | A |  | 1 | 0 |  | baja |
| 0x005EAB80 | 62 | A |  | 1 | 1 |  | baja |
| 0x005EAC50 | 62 | A |  | 1 | 0 |  | baja |

## CStreaming (17)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00407AC0 | 1 | A |  | 2 | 2 |  | alta |
| 0x00407BE0 | 1 | A |  | 10 | 10 |  | alta |
| 0x00407BF0 | 1 | A |  | 10 | 10 |  | alta |
| 0x00407F20 | 1 | A |  | 2 | 2 |  | alta |
| 0x00407A30 | 10 | A |  | 4 | 4 |  | alta |
| 0x004096C0 | 11 | A |  | 1 | 1 |  | alta |
| 0x00407D30 | 13 | A |  | 1 | 1 |  | alta |
| 0x00407DC0 | 13 | A |  | 1 | 1 |  | alta |
| 0x00407A10 | 17 | A |  | 1 | 1 |  | alta |
| 0x0040B490 | 18 | A |  | 2 | 1 |  | alta |
| 0x0040C1C0 | 18 | C |  | 1 | 0 |  | alta |
| 0x00407800 | 20 | C |  | 1 | 0 |  | alta |
| 0x00407670 | 27 | D |  | 0 | 0 |  | alta |
| 0x004082A0 | 31 | A |  | 6 | 1 |  | baja |
| 0x00408230 | 64 | A |  | 3 | 3 |  | baja |
| 0x005B8340 | 94 | A |  | 1 | 1 |  | baja |
| 0x0040C450 | 195 | A |  | 3 | 3 |  | alta |

## CStencilShadows (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00710BA0 | 30 | A |  | 1 | 1 |  | alta |
| 0x0070FA70 | 107 | A |  | 6 | 6 |  | alta |
| 0x00711280 | 134 | A |  | 3 | 3 |  | baja |
| 0x00711160 | 136 | A |  | 3 | 3 |  | baja |
| 0x007111F0 | 136 | A |  | 2 | 2 |  | baja |

## CTaskComplexRoadRage (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006221B0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00622230 | 6 | B |  | 0 | 0 | sí | baja |
| 0x006221C0 | 11 | C |  | 1 | 0 |  | baja |
| 0x006293A0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00629550 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00622180 | 41 | D |  | 0 | 0 |  | baja |
| 0x006221D0 | 89 | C |  | 2 | 0 |  | baja |
| 0x006293C0 | 178 | B |  | 0 | 0 | sí | baja |
| 0x00629480 | 194 | B |  | 0 | 0 | sí | baja |

## CEventEditableResponse (14)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00420F00 | 3 | B |  | 0 | 0 | sí | baja |
| 0x00420F20 | 3 | B |  | 0 | 0 | sí | baja |
| 0x00420F10 | 4 | B |  | 0 | 0 | sí | baja |
| 0x00420F40 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00420FD0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00420F30 | 11 | C |  | 14 | 0 |  | baja |
| 0x00420FB0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x004B58E0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B5930 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B5980 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B59D0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B5A20 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B5A70 | 76 | D |  | 0 | 0 |  | baja |
| 0x00420F50 | 88 | B |  | 0 | 0 | sí | baja |

## Rs (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00619440 | 5 | A |  | 12 | 12 |  | alta |
| 0x006198C0 | 65 | A |  | 1 | 1 |  | alta |
| 0x00619780 | 181 | A |  | 1 | 1 |  | alta |
| 0x00619910 | 410 | A |  | 1 | 1 |  | alta |

## CEntryExitManager (11)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004410E0 | 23 | A |  | 1 | 1 |  | baja |
| 0x0043F9B0 | 24 | C |  | 2 | 0 |  | alta |
| 0x0043EF00 | 30 | C |  | 3 | 0 |  | alta |
| 0x0043EEB0 | 54 | D |  | 0 | 0 |  | alta |
| 0x0043F780 | 54 | D |  | 0 | 0 |  | alta |
| 0x00441130 | 54 | A |  | 8 | 4 |  | baja |
| 0x0043EF90 | 59 | C |  | 1 | 0 |  | alta |
| 0x0043F950 | 71 | D |  | 0 | 0 |  | alta |
| 0x0043F720 | 91 | A |  | 1 | 1 |  | alta |
| 0x0043F7D0 | 103 | A |  | 1 | 1 |  | alta |
| 0x0043EF20 | 112 | C |  | 2 | 0 |  | alta |

## CTaskComplexFallAndGetUp (13)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006786F0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00678A00 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00678BC0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00678790 | 11 | A |  | 1 | 1 |  | alta |
| 0x00678A90 | 11 | C |  | 1 | 0 |  | baja |
| 0x0067CBC0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0067CC10 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006789D0 | 34 | D |  | 0 | 0 |  | baja |
| 0x0067CBE0 | 45 | B |  | 0 | 0 | sí | baja |
| 0x00678A10 | 110 | D |  | 0 | 0 |  | baja |
| 0x00678AA0 | 118 | B |  | 0 | 0 | sí | baja |
| 0x00678B20 | 145 | B |  | 0 | 0 | sí | baja |
| 0x00678BD0 | 167 | C |  | 1 | 0 |  | baja |

## CQuadTreeNode<T> (10)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005531C0 | 1 | A |  | 1 | 1 |  | baja |
| 0x005531D0 | 1 | A |  | 1 | 1 |  | baja |
| 0x00552C80 | 11 | A |  | 3 | 3 |  | alta |
| 0x00552BC0 | 49 | A |  | 1 | 1 |  | alta |
| 0x00552C90 | 49 | A |  | 7 | 4 |  | alta |
| 0x00552830 | 63 | A |  | 3 | 3 |  | alta |
| 0x00552AF0 | 92 | A |  | 1 | 1 |  | alta |
| 0x00552B50 | 104 | A |  | 2 | 1 |  | alta |
| 0x00552EE0 | 144 | D |  | 0 | 0 |  | baja |
| 0x00553010 | 206 | D |  | 0 | 0 |  | baja |

## CObject (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005D28F0 | 5 | C |  | 2 | 0 |  | baja |
| 0x005D28D0 | 15 | D |  | 0 | 0 |  | baja |
| 0x005D28B0 | 19 | C |  | 2 | 0 |  | baja |
| 0x005A2CB0 | 59 | A |  | 5 | 5 |  | baja |
| 0x0059F610 | 74 | A |  | 1 | 0 |  | alta |
| 0x005A2BD0 | 76 | B |  | 0 | 0 | sí | alta |
| 0x0059F660 | 480 | A |  | 6 | 2 |  | alta |

## CEventScriptCommand (11)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B0A40 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004B6520 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6570 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6660 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B66B0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6700 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B67A0 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B67F0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B64D0 | 77 | B |  | 0 | 0 | sí | baja |
| 0x004B65C0 | 77 | B |  | 0 | 0 | sí | baja |
| 0x004B6840 | 77 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleThrowProjectile (11)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00625B60 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00625B80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00625BA0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00625BC0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00625BE0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00625C00 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00625E20 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00626A80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00626AA0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00625C20 | 68 | B |  | 0 | 0 | sí | baja |
| 0x00625C70 | 429 | B |  | 0 | 0 | sí | baja |

## CTaskComplexUseWaterCannon (15)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00659E00 | 5 | C |  | 1 | 0 |  | baja |
| 0x006597A0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x006598A0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00659980 | 6 | B |  | 0 | 0 | sí | baja |
| 0x006597B0 | 11 | A |  | 1 | 1 |  | alta |
| 0x006598B0 | 11 | C |  | 1 | 0 |  | baja |
| 0x00659DE0 | 30 | D |  | 0 | 0 |  | baja |
| 0x00659E10 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00659E70 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0065A700 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00659870 | 43 | D |  | 0 | 0 |  | baja |
| 0x00659D70 | 106 | B |  | 0 | 0 | sí | baja |
| 0x00659990 | 120 | C |  | 1 | 0 |  | baja |
| 0x0065A720 | 155 | B |  | 0 | 0 | sí | baja |
| 0x006598C0 | 180 | B |  | 0 | 0 | sí | baja |

## CTrafficLights (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0049E640 | 10 | A |  | 1 | 1 |  | baja |
| 0x0049E650 | 10 | A |  | 2 | 2 |  | baja |
| 0x0049E620 | 15 | C |  | 1 | 0 |  | baja |
| 0x0049E850 | 242 | A |  | 1 | 0 |  | baja |
| 0x0049E660 | 493 | A |  | 2 | 0 |  | baja |

## CTaskComplexWaitForPed (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00683430 | 5 | D |  | 0 | 0 |  | baja |
| 0x00683440 | 5 | D |  | 0 | 0 |  | baja |
| 0x006833C0 | 6 | D |  | 0 | 0 |  | baja |
| 0x006833D0 | 90 | A |  | 1 | 1 |  | baja |
| 0x00683450 | 138 | D |  | 0 | 0 |  | baja |
| 0x006834E0 | 528 | D |  | 0 | 0 |  | baja |

## CTaskComplexWander (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0066F4A0 | 30 | B |  | 0 | 0 | sí | alta |
| 0x00669FB0 | 54 | A |  | 1 | 1 |  | baja |
| 0x00669FF0 | 265 | A |  | 1 | 1 |  | baja |
| 0x00673EF0 | 448 | D |  | 0 | 0 |  | alta |

## CModelInfo (22)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004C6200 | 5 | A |  | 9 | 9 |  | alta |
| 0x004C6B10 | 11 | D |  | 0 | 0 |  | baja |
| 0x004C6210 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C6230 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C6250 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C6270 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C6290 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C62B0 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C62D0 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C6350 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C5D10 | 36 | C |  | 1 | 0 |  | alta |
| 0x004C5D70 | 36 | C |  | 1 | 0 |  | alta |
| 0x004C5E30 | 36 | C |  | 1 | 0 |  | alta |
| 0x004C5E90 | 36 | C |  | 1 | 0 |  | alta |
| 0x004C5EF0 | 36 | C |  | 1 | 0 |  | alta |
| 0x004C5CB0 | 39 | C |  | 1 | 0 |  | alta |
| 0x004C5DD0 | 39 | C |  | 1 | 0 |  | alta |
| 0x004C67D0 | 39 | C |  | 1 | 0 |  | alta |
| 0x004C5F50 | 42 | C |  | 1 | 0 |  | alta |
| 0x004C6BD0 | 61 | D |  | 0 | 0 |  | baja |
| 0x004C62F0 | 90 | C |  | 1 | 0 |  | alta |
| 0x004C6B70 | 92 | D |  | 0 | 0 |  | baja |

## CTaskComplexInvestigateDisturbance (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0066CB90 | 30 | D |  | 0 | 0 |  | baja |
| 0x0066D6D0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0066D6F0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0066CBB0 | 97 | C |  | 1 | 0 |  | baja |
| 0x00674BB0 | 114 | B |  | 0 | 0 | sí | baja |
| 0x0066CA40 | 209 | A |  | 2 | 1 |  | baja |
| 0x0066D550 | 348 | C |  | 1 | 0 |  | baja |

## CVehicle (33)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006D6380 | 1 | B |  | 0 | 0 | sí | alta |
| 0x006D6390 | 1 | B |  | 0 | 0 | sí | alta |
| 0x006D63A0 | 1 | B |  | 0 | 0 | sí | alta |
| 0x006D63B0 | 1 | B |  | 0 | 0 | sí | alta |
| 0x006D63C0 | 1 | B |  | 0 | 0 | sí | alta |
| 0x006D6270 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006D6280 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006D6290 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006D6330 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006D6340 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006D63E0 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006D62A0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D62B0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D62C0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D62D0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D62E0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D62F0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D6300 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D6310 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D6320 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D6350 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D6360 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D6370 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D63D0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x006D3D50 | 9 | B |  | 0 | 0 | sí | alta |
| 0x006D56B0 | 9 | B |  | 0 | 0 | sí | alta |
| 0x006E2D90 | 49 | A |  | 72 | 7 |  | alta |
| 0x006E2DD0 | 49 | C |  | 21 | 0 |  | alta |
| 0x006D0BC0 | 62 | D |  | 0 | 0 |  | alta |
| 0x006D5DD0 | 74 | A |  | 1 | 1 |  | alta |
| 0x006E38A0 | 76 | B |  | 0 | 0 | sí | alta |
| 0x0049B0E0 | 217 | A |  | 1 | 1 |  | baja |
| 0x006D5E20 | 227 | A |  | 1 | 1 |  | alta |

## CVehicleSaveStructure (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005D24B0 | 172 | A |  | 1 | 1 |  | baja |
| 0x005D2560 | 186 | A |  | 1 | 1 |  | baja |
| 0x005D2740 | 234 | A |  | 1 | 1 |  | baja |
| 0x005D2620 | 282 | A |  | 1 | 1 |  | baja |

## Ps (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00745A70 | 5 | A |  | 2 | 2 |  | baja |
| 0x007457C0 | 44 | D |  | 0 | 0 |  | alta |
| 0x007457F0 | 73 | D |  | 0 | 0 |  | alta |
| 0x00745560 | 127 | C |  | 1 | 0 |  | alta |
| 0x00746480 | 300 | C |  | 1 | 0 |  | alta |
| 0x00745920 | 327 | C |  | 2 | 0 |  | baja |

## CPad (11)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00541D70 | 1 | A |  | 4 | 3 |  | alta |
| 0x00541E50 | 5 | A |  | 1 | 1 |  | baja |
| 0x0053F910 | 10 | A |  | 2 | 1 |  | alta |
| 0x00541E90 | 23 | A |  | 1 | 1 |  | baja |
| 0x00747000 | 32 | A |  | 1 | 1 |  | baja |
| 0x00541F40 | 38 | A |  | 6 | 6 |  | baja |
| 0x005404F0 | 45 | A |  | 2 | 2 |  | alta |
| 0x00541DD0 | 113 | A |  | 5 | 4 |  | baja |
| 0x00746ED0 | 148 | A |  | 1 | 1 |  | baja |
| 0x00747020 | 158 | D |  | 0 | 0 |  | baja |
| 0x00746D80 | 322 | A |  | 2 | 2 |  | baja |

## CEventSeenCop (35)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005FF780 | 3 | B |  | 0 | 0 | sí | baja |
| 0x005FF850 | 3 | B |  | 0 | 0 | sí | baja |
| 0x005FF860 | 5 | A |  | 2 | 1 |  | baja |
| 0x005FF3B0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FF770 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FF840 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FF870 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FF880 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FF890 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FF8A0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FF7E0 | 7 | B |  | 0 | 0 | sí | baja |
| 0x005FF3A0 | 11 | C |  | 2 | 0 |  | baja |
| 0x005FF460 | 11 | C |  | 3 | 0 |  | baja |
| 0x005FF520 | 11 | C |  | 5 | 0 |  | baja |
| 0x005FF5E0 | 11 | C |  | 2 | 0 |  | baja |
| 0x005FF6A0 | 11 | C |  | 3 | 0 |  | baja |
| 0x005FF760 | 11 | C |  | 2 | 0 |  | baja |
| 0x005FF440 | 18 | D |  | 0 | 0 |  | baja |
| 0x005FF500 | 18 | D |  | 0 | 0 |  | baja |
| 0x005FF5C0 | 18 | D |  | 0 | 0 |  | baja |
| 0x005FF680 | 18 | D |  | 0 | 0 |  | baja |
| 0x005FF740 | 18 | D |  | 0 | 0 |  | baja |
| 0x005FF820 | 18 | D |  | 0 | 0 |  | baja |
| 0x005FF420 | 30 | B |  | 0 | 0 | sí | baja |
| 0x005FF4E0 | 30 | D |  | 0 | 0 |  | baja |
| 0x005FF5A0 | 30 | D |  | 0 | 0 |  | baja |
| 0x005FF660 | 30 | D |  | 0 | 0 |  | baja |
| 0x005FF720 | 30 | D |  | 0 | 0 |  | baja |
| 0x005FF7F0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x005FF490 | 80 | D |  | 0 | 0 |  | baja |
| 0x005FF550 | 80 | D |  | 0 | 0 |  | baja |
| 0x005FF610 | 80 | D |  | 0 | 0 |  | baja |
| 0x005FF6D0 | 80 | D |  | 0 | 0 |  | baja |
| 0x005FF790 | 80 | B |  | 0 | 0 | sí | baja |
| 0x005FF3C0 | 88 | B |  | 0 | 0 | sí | baja |

## CTaskComplexSunbathe (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00638570 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00638590 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006385B0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00639E20 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00632190 | 90 | A |  | 4 | 3 |  | baja |
| 0x00638290 | 725 | A |  | 12 | 12 |  | baja |

## CEntityScanner (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005FFF30 | 55 | A |  | 1 | 1 |  | baja |
| 0x005FFE60 | 113 | A |  | 1 | 1 |  | baja |
| 0x006034B0 | 181 | A |  | 2 | 2 |  | baja |
| 0x00603A30 | 187 | A |  | 1 | 1 |  | baja |
| 0x00603570 | 422 | A |  | 1 | 1 |  | baja |

## CTaskComplexMedicTreatInjuredPed (13)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00658C20 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00659230 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0065B050 | 18 | B |  | 0 | 0 | sí | baja |
| 0x0065A0F0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0065A220 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0065B070 | 31 | B |  | 0 | 0 | sí | baja |
| 0x006591D0 | 83 | C |  | 1 | 0 |  | baja |
| 0x00659240 | 90 | C |  | 1 | 0 |  | baja |
| 0x00659B60 | 96 | B |  | 0 | 0 | sí | baja |
| 0x0065AFE0 | 97 | B |  | 0 | 0 | sí | baja |
| 0x006592A0 | 99 | C |  | 1 | 0 |  | baja |
| 0x00658C30 | 129 | A |  | 1 | 1 |  | alta |
| 0x0065A110 | 259 | C |  | 4 | 0 |  | baja |

## CTaskComplexGoToPointAndStandStill (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00668240 | 11 | A |  | 1 | 1 |  | alta |
| 0x004700E0 | 18 | C |  | 1 | 0 |  | baja |
| 0x0066DC70 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00470070 | 32 | C |  | 2 | 0 |  | baja |
| 0x00470100 | 48 | C |  | 2 | 0 |  | baja |
| 0x0046FEE0 | 61 | A |  | 2 | 2 |  | baja |
| 0x00668250 | 116 | A |  | 2 | 1 |  | alta |
| 0x006682D0 | 661 | A |  | 8 | 8 |  | alta |

## CTaskSimpleBeHit (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00620B80 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00620E00 | 41 | B |  | 0 | 0 | sí | baja |
| 0x00620BF0 | 64 | B |  | 0 | 0 | sí | baja |
| 0x00620B90 | 90 | C |  | 1 | 0 |  | baja |
| 0x00623300 | 112 | B |  | 0 | 0 | sí | baja |
| 0x00620B00 | 121 | C |  | 1 | 0 |  | baja |
| 0x00623370 | 137 | B |  | 0 | 0 | sí | baja |
| 0x00620C30 | 451 | C |  | 4 | 0 |  | baja |

## CEventVehicleHitAndRun (15)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AEA10 | 3 | B |  | 0 | 0 | sí | baja |
| 0x004AEA00 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004AEA40 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004AEA30 | 7 | B |  | 0 | 0 | sí | baja |
| 0x004AEA20 | 12 | B |  | 0 | 0 | sí | baja |
| 0x004AEDC0 | 35 | D |  | 0 | 0 |  | baja |
| 0x004AF100 | 35 | D |  | 0 | 0 |  | baja |
| 0x004AEC00 | 65 | D |  | 0 | 0 |  | baja |
| 0x004B7280 | 97 | D |  | 0 | 0 |  | baja |
| 0x004B72F0 | 97 | D |  | 0 | 0 |  | baja |
| 0x004B7360 | 97 | D |  | 0 | 0 |  | baja |
| 0x004B73D0 | 97 | D |  | 0 | 0 |  | baja |
| 0x004B7210 | 103 | D |  | 0 | 0 |  | baja |
| 0x004B7170 | 105 | D |  | 0 | 0 |  | baja |
| 0x004AEF00 | 272 | D |  | 0 | 0 |  | baja |

## CPed (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005E88E0 | 43 | A |  | 1 | 1 |  | alta |
| 0x005E47A0 | 49 | C |  | 3 | 0 |  | alta |
| 0x005E0540 | 74 | A |  | 1 | 1 |  | baja |
| 0x005E8C80 | 76 | B |  | 0 | 0 | sí | baja |
| 0x005E3630 | 400 | A |  | 1 | 1 |  | alta |
| 0x005E37C0 | 404 | A |  | 1 | 1 |  | alta |

## CCam (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00517760 | 12 | D |  | 0 | 0 |  | baja |
| 0x005B2AB0 | 12 | D |  | 0 | 0 |  | baja |
| 0x00517740 | 23 | D |  | 0 | 0 |  | baja |
| 0x00514010 | 27 | D |  | 0 | 0 |  | baja |
| 0x00513220 | 164 | A |  | 3 | 3 |  | alta |
| 0x0050A870 | 253 | C |  | 2 | 0 |  | baja |
| 0x005132D0 | 568 | A |  | 4 | 4 |  | alta |

## CTaskComplexStuckInAir (11)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0067BA70 | 11 | A |  | 1 | 1 |  | alta |
| 0x0067C750 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0067C130 | 80 | B |  | 0 | 0 | sí | baja |
| 0x0067C270 | 98 | B |  | 0 | 0 | sí | baja |
| 0x0067C200 | 106 | B |  | 0 | 0 | sí | baja |
| 0x0067C0C0 | 110 | B |  | 0 | 0 | sí | baja |
| 0x0067C180 | 120 | B |  | 0 | 0 | sí | baja |
| 0x0067C2E0 | 120 | B |  | 0 | 0 | sí | baja |
| 0x0067C360 | 129 | B |  | 0 | 0 | sí | baja |
| 0x0067C480 | 130 | B |  | 0 | 0 | sí | baja |
| 0x0067C3F0 | 141 | B |  | 0 | 0 | sí | baja |

## CTrain (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006F5EE0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006F59C0 | 22 | B |  | 0 | 0 | sí | alta |
| 0x006F6300 | 30 | B |  | 0 | 0 | sí | alta |
| 0x006F5930 | 103 | B |  | 0 | 0 | sí | alta |
| 0x006F7190 | 197 | B |  | 0 | 0 | sí | alta |
| 0x006F6370 | 719 | B |  | 0 | 0 | sí | alta |

## CTaskComplexSeekEntityAnyMeansXYOffset (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006956B0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00696260 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006956D0 | 97 | C |  | 1 | 0 |  | baja |
| 0x006962A0 | 981 | A |  | 7 | 7 |  | baja |

## CPlaceable (24)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00411BA0 | 4 | A |  | 6 | 6 |  | baja |
| 0x0043E1A0 | 5 | C |  | 5 | 0 |  | baja |
| 0x00411BF0 | 10 | A |  | 2 | 2 |  | baja |
| 0x00411A30 | 35 | A |  | 2 | 1 |  | baja |
| 0x0054F490 | 36 | A |  | 5 | 1 |  | alta |
| 0x0054F5D0 | 53 | B |  | 0 | 0 | sí | alta |
| 0x0054FA10 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FA60 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FAB0 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FB00 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FB50 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FBA0 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FBF0 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FC40 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FC90 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FCE0 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FD30 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FD80 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FDD0 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FE20 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FE70 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FEC0 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FF10 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054F440 | 77 | C |  | 1 | 0 |  | alta |

## CEventVehicleOnFire (17)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B1140 | 3 | B |  | 0 | 0 | sí | baja |
| 0x004B1130 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B1160 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B1150 | 7 | B |  | 0 | 0 | sí | baja |
| 0x004B7A40 | 43 | B |  | 0 | 0 | sí | baja |
| 0x004B7810 | 44 | D |  | 0 | 0 |  | baja |
| 0x004B7A70 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B7C80 | 78 | B |  | 0 | 0 | sí | baja |
| 0x004B77B0 | 84 | B |  | 0 | 0 | sí | baja |
| 0x004B7840 | 97 | B |  | 0 | 0 | sí | baja |
| 0x004B7960 | 97 | D |  | 0 | 0 |  | baja |
| 0x004B79D0 | 97 | B |  | 0 | 0 | sí | baja |
| 0x004B7B30 | 97 | B |  | 0 | 0 | sí | baja |
| 0x004B7BA0 | 97 | B |  | 0 | 0 | sí | baja |
| 0x004B78B0 | 101 | B |  | 0 | 0 | sí | baja |
| 0x004B7C10 | 101 | B |  | 0 | 0 | sí | baja |
| 0x004B7AC0 | 111 | B |  | 0 | 0 | sí | baja |

## CEventVehicleCollision (15)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AC900 | 3 | B |  | 0 | 0 | sí | baja |
| 0x004AC980 | 5 | B |  | 0 | 0 | sí | baja |
| 0x004AC8F0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004AC910 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B6E30 | 53 | D |  | 0 | 0 |  | baja |
| 0x004B6EE0 | 90 | D |  | 0 | 0 |  | baja |
| 0x004B6F40 | 97 | D |  | 0 | 0 |  | baja |
| 0x004B6FB0 | 101 | B |  | 0 | 0 | sí | baja |
| 0x004B7020 | 101 | B |  | 0 | 0 | sí | baja |
| 0x004B7090 | 105 | B |  | 0 | 0 | sí | baja |
| 0x004B6DC0 | 107 | D |  | 0 | 0 |  | baja |
| 0x004B6E70 | 107 | D |  | 0 | 0 |  | baja |
| 0x004B6CC0 | 121 | B |  | 0 | 0 | sí | baja |
| 0x004B6D40 | 121 | B |  | 0 | 0 | sí | baja |
| 0x004B6C40 | 126 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleRunTimedAnim (12)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0061AF50 | 5 | B |  | 0 | 0 | sí | baja |
| 0x0061BCA0 | 5 | C |  | 1 | 0 |  | baja |
| 0x0061AEA0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0061BC80 | 30 | D |  | 0 | 0 |  | baja |
| 0x0061BD80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0061ADF0 | 82 | C |  | 1 | 0 |  | baja |
| 0x0061AC90 | 112 | C |  | 1 | 0 |  | baja |
| 0x0061B980 | 118 | B |  | 0 | 0 | sí | baja |
| 0x0061AEB0 | 146 | C |  | 1 | 0 |  | baja |
| 0x0061BCB0 | 193 | D |  | 0 | 0 |  | baja |
| 0x0061AD10 | 222 | C |  | 1 | 0 |  | baja |
| 0x0061B890 | 226 | D |  | 0 | 0 |  | baja |

## Interior_c (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00597FE0 | 12 | D |  | 0 | 0 |  | baja |
| 0x005921D0 | 87 | D |  | 0 | 0 |  | alta |
| 0x0059A840 | 510 | A |  | 2 | 2 |  | baja |
| 0x0059AA40 | 591 | A |  | 2 | 2 |  | baja |

## CAttractorScanner (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00600180 | 364 | A |  | 1 | 1 |  | baja |
| 0x005FFFD0 | 429 | A |  | 2 | 1 |  | baja |
| 0x006002F0 | 480 | C |  | 2 | 0 |  | baja |

## CClothesBuilder (13)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005A44A0 | 24 | D |  | 0 | 0 |  | alta |
| 0x005A6DB0 | 41 | C |  | 1 | 0 |  | baja |
| 0x005A7570 | 41 | C |  | 1 | 0 |  | baja |
| 0x005A47E0 | 46 | D |  | 0 | 0 |  | alta |
| 0x005A73C0 | 49 | C |  | 1 | 0 |  | baja |
| 0x005A6E60 | 54 | D |  | 0 | 0 |  | baja |
| 0x005A72B0 | 54 | D |  | 0 | 0 |  | baja |
| 0x005A7490 | 83 | C |  | 1 | 0 |  | baja |
| 0x005A7350 | 104 | C |  | 1 | 0 |  | baja |
| 0x005A6FC0 | 122 | C |  | 4 | 0 |  | baja |
| 0x005A74F0 | 125 | C |  | 1 | 0 |  | baja |
| 0x005A42B0 | 203 | A |  | 1 | 1 |  | alta |
| 0x005A75B0 | 339 | C |  | 1 | 0 |  | baja |

## CVehicleModelInfo (15)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004C7650 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004C7660 | 7 | B |  | 0 | 0 | sí | alta |
| 0x004C7630 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C8E10 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C76D0 | 37 | B |  | 0 | 0 | sí | alta |
| 0x004C9580 | 49 | C |  | 1 | 0 |  | alta |
| 0x004C75E0 | 75 | D |  | 0 | 0 |  | alta |
| 0x004C9840 | 76 | D |  | 0 | 0 |  | baja |
| 0x004C9890 | 82 | B |  | 0 | 0 | sí | baja |
| 0x004C7410 | 85 | C |  | 2 | 0 |  | alta |
| 0x004C7670 | 95 | B |  | 0 | 0 | sí | alta |
| 0x004C94C0 | 106 | C |  | 1 | 0 |  | alta |
| 0x004C8D60 | 161 | C |  | 1 | 0 |  | alta |
| 0x004C95C0 | 183 | B |  | 0 | 0 | sí | alta |
| 0x004C9680 | 343 | B |  | 0 | 0 | sí | alta |

## CTaskComplexAvoidOtherPedWhileWandering (10)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0066AAD0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0066AD10 | 46 | C |  | 1 | 0 |  | baja |
| 0x0066AD40 | 48 | B |  | 0 | 0 | sí | baja |
| 0x0066AD70 | 48 | B |  | 0 | 0 | sí | baja |
| 0x0066AAE0 | 90 | C |  | 1 | 0 |  | baja |
| 0x0066D0C0 | 104 | B |  | 0 | 0 | sí | baja |
| 0x0066AA20 | 161 | C |  | 1 | 0 |  | baja |
| 0x00672530 | 204 | B |  | 0 | 0 | sí | baja |
| 0x0066ADE0 | 220 | B |  | 0 | 0 | sí | baja |
| 0x0066AB40 | 455 | C |  | 1 | 0 |  | baja |

## CTaskInteriorUseInfo (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00676D10 | 30 | D |  | 0 | 0 |  | baja |
| 0x006772C0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00676D30 | 1423 | D |  | 0 | 0 |  | baja |

## CPedIntelligencePool (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00605FB0 | 11 | C |  | 1 | 0 |  | baja |
| 0x00606000 | 11 | C |  | 1 | 0 |  | baja |
| 0x00606050 | 11 | C |  | 1 | 0 |  | baja |
| 0x00606010 | 49 | A |  | 1 | 1 |  | baja |
| 0x00606BA0 | 694 | A |  | 1 | 1 |  | baja |
| 0x00606580 | 776 | A |  | 1 | 1 |  | baja |

## CEventDamage (22)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004ADE90 | 7 | A |  | 7 | 3 |  | baja |
| 0x004AE2D0 | 69 | D |  | 0 | 0 |  | baja |
| 0x004B5E00 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B5E50 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B5EA0 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B5EF0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B5F40 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B5F90 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B5FE0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6030 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B60D0 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B6120 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B6170 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B61C0 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B6210 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B6260 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B62B0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6300 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6350 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B63A0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B63F0 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B6440 | 76 | B |  | 0 | 0 | sí | baja |

## CTaskComplexWanderGang (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0066F670 | 11 | C |  | 1 | 0 |  | baja |
| 0x0066F650 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0066F680 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0066F6A0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0066FD30 | 108 | C |  | 1 | 0 |  | baja |
| 0x0066F6C0 | 1541 | C |  | 1 | 0 |  | baja |

## CPedAttractor (28)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005E9680 | 5 | B |  | 0 | 0 | sí | baja |
| 0x005EB2A0 | 35 | A |  | 3 | 0 |  | alta |
| 0x005EB310 | 35 | A |  | 4 | 1 |  | alta |
| 0x005EB390 | 35 | A |  | 3 | 0 |  | alta |
| 0x005EC820 | 39 | D |  | 0 | 0 |  | baja |
| 0x005EB140 | 40 | A |  | 1 | 0 |  | alta |
| 0x005EB1B0 | 40 | A |  | 1 | 0 |  | alta |
| 0x005EB260 | 40 | A |  | 1 | 0 |  | alta |
| 0x005EB930 | 40 | A |  | 2 | 0 |  | baja |
| 0x005EB960 | 40 | A |  | 2 | 0 |  | baja |
| 0x005EB990 | 40 | A |  | 2 | 0 |  | baja |
| 0x005EB9D0 | 40 | A |  | 2 | 0 |  | baja |
| 0x005EBA00 | 40 | A |  | 2 | 0 |  | baja |
| 0x005EB2E0 | 46 | A |  | 3 | 0 |  | alta |
| 0x005EAFE0 | 49 | C |  | 10 | 0 |  | alta |
| 0x005EB170 | 50 | A |  | 1 | 0 |  | alta |
| 0x005EB1E0 | 50 | A |  | 1 | 0 |  | alta |
| 0x005EB220 | 50 | A |  | 1 | 0 |  | alta |
| 0x005EB340 | 55 | A |  | 3 | 0 |  | alta |
| 0x005EB5F0 | 67 | A |  | 10 | 10 |  | baja |
| 0x005EB6F0 | 68 | A |  | 2 | 2 |  | baja |
| 0x005EB640 | 75 | A |  | 10 | 10 |  | baja |
| 0x005EC4B0 | 76 | B |  | 0 | 0 | sí | alta |
| 0x005EB690 | 93 | A |  | 10 | 10 |  | baja |
| 0x005EB7B0 | 95 | A |  | 10 | 10 |  | baja |
| 0x005EB740 | 97 | A |  | 10 | 10 |  | baja |
| 0x005EC660 | 217 | A |  | 10 | 10 |  | baja |
| 0x005EC740 | 217 | A |  | 10 | 10 |  | baja |

## CTaskSimpleThrowControl (14)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0061FC40 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0061FE60 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0061FC20 | 26 | D |  | 0 | 0 |  | baja |
| 0x0061FE00 | 34 | B |  | 0 | 0 | sí | baja |
| 0x00620290 | 34 | B |  | 0 | 0 | sí | baja |
| 0x0061FE30 | 42 | D |  | 0 | 0 |  | baja |
| 0x0061FCC0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x0061FEE0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x0061FC50 | 103 | C |  | 1 | 0 |  | baja |
| 0x0061FE70 | 103 | C |  | 1 | 0 |  | baja |
| 0x00623120 | 111 | B |  | 0 | 0 | sí | baja |
| 0x00623190 | 130 | B |  | 0 | 0 | sí | baja |
| 0x0061FD20 | 219 | C |  | 1 | 0 |  | baja |
| 0x0061FF40 | 836 | C |  | 1 | 0 |  | baja |

## CTaskComplexUseClosestFreeScriptedAttractor (13)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006348D0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x006348E0 | 11 | C |  | 1 | 0 |  | baja |
| 0x00639660 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00639680 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006396A0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006348A0 | 47 | C |  | 1 | 0 |  | baja |
| 0x006348F0 | 62 | C |  | 2 | 0 |  | baja |
| 0x006396C0 | 108 | A |  | 1 | 1 |  | baja |
| 0x00636FD0 | 130 | B |  | 0 | 0 | sí | baja |
| 0x00634930 | 262 | B |  | 0 | 0 | sí | baja |
| 0x00634C10 | 330 | B |  | 0 | 0 | sí | baja |
| 0x00634D60 | 387 | B |  | 0 | 0 | sí | baja |
| 0x00634A60 | 425 | B |  | 0 | 0 | sí | baja |

## CCarCtrl (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00424160 | 40 | C |  | 16 | 0 |  | baja |
| 0x00422F80 | 117 | A |  | 1 | 1 |  | alta |
| 0x00422E10 | 353 | A |  | 1 | 0 |  | alta |
| 0x00421A50 | 1418 | A |  | 2 | 2 |  | alta |

## CPathFind (14)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0044D0E0 | 1 | A |  | 1 | 1 |  | alta |
| 0x0044D2B0 | 1 | A |  | 1 | 1 |  | alta |
| 0x0044D2C0 | 3 | A |  | 1 | 1 |  | alta |
| 0x0044D2D0 | 3 | A |  | 1 | 1 |  | alta |
| 0x0044D2E0 | 3 | A |  | 1 | 1 |  | alta |
| 0x0044D2F0 | 3 | A |  | 1 | 1 |  | alta |
| 0x00454920 | 28 | A |  | 2 | 2 |  | baja |
| 0x004541E0 | 30 | C |  | 1 | 0 |  | baja |
| 0x0044E480 | 62 | A |  | 12 | 11 |  | alta |
| 0x00453F40 | 81 | D |  | 0 | 0 |  | baja |
| 0x00452090 | 190 | A |  | 2 | 2 |  | alta |
| 0x0044FA30 | 285 | A |  | 1 | 0 |  | alta |
| 0x0044FB60 | 374 | D |  | 0 | 0 |  | alta |
| 0x0044FEE0 | 1077 | C |  | 1 | 0 |  | alta |

## SurfaceInfos_c (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0055F2B0 | 360 | A |  | 1 | 1 |  | alta |
| 0x0055EB90 | 1814 | A |  | 1 | 1 |  | alta |

## CCullZones (13)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0072F3B0 | 1 | A |  | 1 | 1 |  | baja |
| 0x0072EC60 | 11 | D |  | 0 | 0 |  | baja |
| 0x0072E360 | 60 | A |  | 6 | 3 |  | baja |
| 0x0072E220 | 79 | A |  | 1 | 1 |  | baja |
| 0x0072E870 | 96 | A |  | 1 | 0 |  | baja |
| 0x0072E080 | 180 | C |  | 1 | 0 |  | baja |
| 0x0072ED80 | 212 | A |  | 1 | 0 |  | baja |
| 0x0072E140 | 217 | A |  | 1 | 1 |  | baja |
| 0x0072E270 | 236 | A |  | 2 | 1 |  | baja |
| 0x0072EC80 | 238 | A |  | 1 | 0 |  | baja |
| 0x0072D850 | 273 | A |  | 4 | 4 |  | alta |
| 0x0072F180 | 418 | A |  | 1 | 1 |  | baja |
| 0x0072E3A0 | 455 | D |  | 0 | 0 |  | baja |

## CAEUserRadioTrackManager (30)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004F44B3 | 9 | D |  | 0 | 0 |  | alta |
| 0x004F4CA0 | 12 | A |  | 1 | 1 |  | baja |
| 0x004F3D50 | 14 | D |  | 0 | 0 |  | alta |
| 0x004F3D60 | 27 | D |  | 0 | 0 |  | alta |
| 0x004F4110 | 27 | D |  | 0 | 0 |  | alta |
| 0x004F3AE0 | 30 | A |  | 1 | 0 |  | alta |
| 0x004F3A15 | 32 | D |  | 0 | 0 |  | alta |
| 0x004F3B30 | 35 | A |  | 3 | 1 |  | alta |
| 0x004F3BA0 | 35 | C |  | 8 | 0 |  | alta |
| 0x004F4CE0 | 36 | A |  | 2 | 2 |  | baja |
| 0x004F3940 | 37 | A |  | 3 | 0 |  | alta |
| 0x004F3D80 | 39 | A |  | 1 | 0 |  | alta |
| 0x004F3F30 | 39 | D |  | 0 | 0 |  | alta |
| 0x004F4030 | 39 | C |  | 1 | 0 |  | alta |
| 0x004F3B00 | 40 | A |  | 1 | 0 |  | alta |
| 0x004F3CE0 | 40 | A |  | 2 | 0 |  | alta |
| 0x004F3AAD | 45 | D |  | 0 | 0 |  | alta |
| 0x004F3D10 | 47 | C |  | 2 | 0 |  | alta |
| 0x004F4130 | 47 | C |  | 1 | 0 |  | alta |
| 0x004F4170 | 57 | A |  | 2 | 0 |  | alta |
| 0x004F39C0 | 85 | A |  | 3 | 0 |  | alta |
| 0x004F4090 | 95 | A |  | 8 | 0 |  | alta |
| 0x004F4230 | 101 | D |  | 0 | 0 |  | alta |
| 0x004F3A3E | 102 | A |  | 1 | 0 |  | alta |
| 0x004F41B0 | 122 | A |  | 1 | 0 |  | alta |
| 0x004F3F60 | 159 | A |  | 7 | 0 |  | alta |
| 0x004F3E80 | 166 | A |  | 3 | 0 |  | alta |
| 0x004F3DB0 | 181 | A |  | 2 | 0 |  | alta |
| 0x004F3BD0 | 190 | A |  | 2 | 0 |  | alta |
| 0x004F4320 | 645 | A |  | 1 | 1 |  | alta |

## CTaskComplexTurnToFaceEntityOrCoord (14)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00670A60 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0066BB00 | 23 | B |  | 0 | 0 | sí | baja |
| 0x0066BA70 | 30 | D |  | 0 | 0 |  | baja |
| 0x006711F0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006712C0 | 30 | D |  | 0 | 0 |  | baja |
| 0x006712E0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0066BA90 | 104 | B |  | 0 | 0 | sí | baja |
| 0x006709F0 | 110 | C |  | 1 | 0 |  | baja |
| 0x00670A70 | 121 | C |  | 2 | 0 |  | baja |
| 0x00670D80 | 146 | B |  | 0 | 0 | sí | baja |
| 0x00671330 | 174 | D |  | 0 | 0 |  | baja |
| 0x0066D300 | 204 | D |  | 0 | 0 |  | baja |
| 0x00670B00 | 631 | C |  | 6 | 0 |  | baja |
| 0x00670E20 | 968 | B |  | 0 | 0 | sí | baja |

## CTaskComplexGoToPointAnyMeans (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0066B780 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00670830 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00672BB0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00673BA0 | 61 | C |  | 2 | 0 |  | baja |
| 0x00673070 | 878 | C |  | 8 | 0 |  | baja |
| 0x00673540 | 1628 | D |  | 0 | 0 |  | baja |

## CAutomobile (10)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006A6290 | 1 | B |  | 0 | 0 | sí | alta |
| 0x006A6280 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006B1330 | 30 | B |  | 0 | 0 | sí | alta |
| 0x006A3030 | 35 | B |  | 0 | 0 | sí | alta |
| 0x006A6590 | 49 | B |  | 0 | 0 | sí | alta |
| 0x006A2F80 | 167 | D |  | 0 | 0 |  | alta |
| 0x006A32B0 | 384 | B |  | 0 | 0 | sí | alta |
| 0x006A3060 | 584 | B |  | 0 | 0 | sí | alta |
| 0x006A3BD0 | 646 | A |  | 3 | 3 |  | alta |
| 0x006B0690 | 1018 | A |  | 2 | 2 |  | alta |

## CCustomCarEnvMapPipeline (25)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005DAAC0 | 21 | A |  | 1 | 1 |  | baja |
| 0x005D9180 | 48 | A |  | 1 | 1 |  | baja |
| 0x005D8F70 | 54 | D |  | 0 | 0 |  | baja |
| 0x005D8FC0 | 54 | D |  | 0 | 0 |  | baja |
| 0x005D9010 | 54 | D |  | 0 | 0 |  | baja |
| 0x005D90C0 | 54 | D |  | 0 | 0 |  | baja |
| 0x005D9210 | 54 | D |  | 0 | 0 |  | baja |
| 0x005D9360 | 54 | D |  | 0 | 0 |  | baja |
| 0x005D9450 | 71 | D |  | 0 | 0 |  | baja |
| 0x005D94B0 | 71 | D |  | 0 | 0 |  | baja |
| 0x005D9510 | 71 | D |  | 0 | 0 |  | baja |
| 0x005D8980 | 77 | A |  | 1 | 1 |  | alta |
| 0x005DA640 | 82 | A |  | 3 | 2 |  | baja |
| 0x005D9060 | 92 | A |  | 1 | 1 |  | baja |
| 0x005D91B0 | 92 | A |  | 1 | 1 |  | baja |
| 0x005D9300 | 93 | A |  | 1 | 1 |  | baja |
| 0x005D93B0 | 101 | A |  | 2 | 2 |  | baja |
| 0x005D9110 | 104 | A |  | 4 | 4 |  | baja |
| 0x005D9260 | 104 | A |  | 3 | 3 |  | baja |
| 0x005D8BE0 | 108 | A |  | 1 | 1 |  | alta |
| 0x005D8C50 | 118 | A |  | 1 | 1 |  | alta |
| 0x005D84C0 | 207 | A |  | 1 | 1 |  | alta |
| 0x005DA6A0 | 236 | A |  | 3 | 2 |  | baja |
| 0x005D8590 | 723 | A |  | 1 | 1 |  | alta |
| 0x005DA790 | 804 | A |  | 1 | 1 |  | baja |

## CPools (52)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005521D0 | 37 | A |  | 1 | 1 |  | baja |
| 0x005501E0 | 54 | D |  | 0 | 0 |  | alta |
| 0x005502B0 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550380 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550450 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550520 | 54 | D |  | 0 | 0 |  | alta |
| 0x005505D0 | 54 | D |  | 0 | 0 |  | alta |
| 0x005506A0 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550750 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550820 | 54 | D |  | 0 | 0 |  | alta |
| 0x005508F0 | 54 | D |  | 0 | 0 |  | alta |
| 0x005509C0 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550A90 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550B60 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550C30 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550D00 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550DD0 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550EA0 | 54 | D |  | 0 | 0 |  | alta |
| 0x005512F0 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551350 | 71 | D |  | 0 | 0 |  | alta |
| 0x005513B0 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551410 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551470 | 71 | D |  | 0 | 0 |  | alta |
| 0x005514D0 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551530 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551590 | 71 | D |  | 0 | 0 |  | alta |
| 0x005515F0 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551650 | 71 | D |  | 0 | 0 |  | alta |
| 0x005516B0 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551710 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551770 | 71 | D |  | 0 | 0 |  | alta |
| 0x005517D0 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551830 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551890 | 71 | D |  | 0 | 0 |  | alta |
| 0x005518F0 | 71 | D |  | 0 | 0 |  | alta |
| 0x00550570 | 91 | A |  | 1 | 1 |  | alta |
| 0x005506F0 | 91 | A |  | 1 | 1 |  | alta |
| 0x00550890 | 91 | A |  | 1 | 1 |  | alta |
| 0x00550960 | 91 | A |  | 1 | 1 |  | alta |
| 0x00550A30 | 91 | A |  | 1 | 1 |  | alta |
| 0x00550CA0 | 91 | A |  | 1 | 1 |  | alta |
| 0x00550250 | 92 | A |  | 1 | 1 |  | alta |
| 0x00550320 | 92 | A |  | 1 | 1 |  | alta |
| 0x005507C0 | 92 | A |  | 1 | 1 |  | alta |
| 0x00550BD0 | 92 | A |  | 1 | 1 |  | alta |
| 0x00550180 | 93 | A |  | 1 | 1 |  | alta |
| 0x005503F0 | 94 | A |  | 1 | 1 |  | alta |
| 0x005504C0 | 94 | A |  | 1 | 1 |  | alta |
| 0x00550640 | 94 | A |  | 1 | 1 |  | alta |
| 0x00550B00 | 94 | A |  | 1 | 1 |  | alta |
| 0x00550D70 | 94 | A |  | 1 | 1 |  | alta |
| 0x00550E40 | 94 | A |  | 1 | 1 |  | alta |

## CReplay (10)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0045BF90 | 81 | A |  | 1 | 1 |  | alta |
| 0x0045C070 | 81 | A |  | 1 | 1 |  | alta |
| 0x0045BFF0 | 116 | A |  | 1 | 1 |  | alta |
| 0x0045C0D0 | 116 | A |  | 1 | 1 |  | alta |
| 0x0045CD10 | 159 | A |  | 1 | 1 |  | alta |
| 0x0045CDB0 | 226 | A |  | 1 | 1 |  | alta |
| 0x0045C4B0 | 537 | A |  | 1 | 0 |  | alta |
| 0x0045DEF0 | 763 | A |  | 3 | 3 |  | alta |
| 0x0045B730 | 857 | A |  | 6 | 3 |  | alta |
| 0x0045BA90 | 1236 | A |  | 6 | 3 |  | alta |

## CRunningScript (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0048DE60 | 5 | A |  | 1 | 1 |  | baja |
| 0x0048B570 | 18 | C |  | 1 | 0 |  | baja |
| 0x0046B1A0 | 80 | C |  | 1 | 0 |  | baja |
| 0x0048B590 | 5816 | D |  | 0 | 0 |  | baja |

## RpAnimBlend (16)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004D6430 | 26 | D |  | 0 | 0 |  | alta |
| 0x004D5F20 | 27 | D |  | 0 | 0 |  | alta |
| 0x004D6340 | 39 | D |  | 0 | 0 |  | alta |
| 0x004D6270 | 40 | D |  | 0 | 0 |  | alta |
| 0x004D6310 | 46 | D |  | 0 | 0 |  | alta |
| 0x004D6240 | 47 | D |  | 0 | 0 |  | alta |
| 0x004D6450 | 73 | A |  | 1 | 1 |  | alta |
| 0x004D1610 | 101 | A |  | 10 | 4 |  | alta |
| 0x004D66A0 | 118 | A |  | 1 | 1 |  | alta |
| 0x004D6510 | 256 | A |  | 1 | 1 |  | alta |
| 0x004D1DB0 | 843 | A |  | 1 | 1 |  | alta |
| 0x004D2100 | 845 | A |  | 1 | 1 |  | alta |
| 0x004D1A50 | 850 | A |  | 1 | 1 |  | alta |
| 0x004D2450 | 910 | A |  | 1 | 1 |  | alta |
| 0x004D27F0 | 910 | A |  | 1 | 1 |  | alta |
| 0x004D1680 | 971 | A |  | 2 | 1 |  | alta |

## CScriptsForBrains (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0046A900 | 38 | C |  | 2 | 0 |  | alta |
| 0x0046A9C0 | 110 | C |  | 2 | 0 |  | alta |
| 0x0046A930 | 131 | C |  | 2 | 0 |  | alta |
| 0x0046B460 | 6028 | D |  | 0 | 0 |  | alta |

## FxInfoManager_c (116)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004A8DC0 | 1 | A |  | 18 | 3 |  | baja |
| 0x004A8DE0 | 1 | A |  | 35 | 7 |  | baja |
| 0x004A8460 | 7 | C |  | 18 | 0 |  | baja |
| 0x004A87F0 | 7 | C |  | 15 | 0 |  | baja |
| 0x004A89B0 | 7 | C |  | 27 | 0 |  | baja |
| 0x004A8B70 | 7 | C |  | 4 | 0 |  | baja |
| 0x004A9090 | 12 | D |  | 0 | 0 |  | baja |
| 0x004A8DD0 | 13 | A |  | 20 | 9 |  | baja |
| 0x004A8DB0 | 16 | A |  | 15 | 10 |  | baja |
| 0x004A8440 | 20 | A |  | 9 | 0 |  | baja |
| 0x004A87D0 | 20 | A |  | 8 | 0 |  | baja |
| 0x004A8990 | 20 | A |  | 13 | 0 |  | baja |
| 0x004A8B50 | 20 | A |  | 2 | 0 |  | baja |
| 0x004A6A70 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6AE0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6B50 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6BC0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6C30 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6CA0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6D10 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6D80 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6DF0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6E60 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6ED0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6F40 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6FB0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7020 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7090 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7390 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7400 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7470 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A74E0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7550 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A75C0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7630 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A76A0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7710 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7780 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A77F0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7860 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A78D0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7940 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A79B0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7A20 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7A90 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A8D30 | 27 | D |  | 0 | 0 |  | baja |
| 0x004A8D50 | 27 | D |  | 0 | 0 |  | baja |
| 0x004A8D70 | 27 | D |  | 0 | 0 |  | baja |
| 0x004A8D90 | 27 | D |  | 0 | 0 |  | baja |
| 0x004A8E70 | 54 | A |  | 11 | 11 |  | baja |
| 0x004A8E30 | 59 | A |  | 16 | 14 |  | baja |
| 0x004A6A90 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6B00 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6B70 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6BE0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6C50 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6CC0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6D30 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6DA0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6E10 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6E80 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6EF0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6F60 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6FD0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7040 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A70B0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A73B0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7420 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7490 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7500 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7570 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A75E0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7650 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A76C0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7730 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A77A0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7810 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7880 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A78F0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7960 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A79D0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7A40 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7AB0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A9010 | 67 | A |  | 3 | 3 |  | baja |
| 0x004A5DA0 | 79 | A |  | 1 | 1 |  | alta |
| 0x004A5E20 | 79 | A |  | 1 | 1 |  | alta |
| 0x004A61A0 | 79 | A |  | 1 | 1 |  | alta |
| 0x004A6760 | 79 | A |  | 1 | 1 |  | alta |
| 0x004A4C20 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A4D00 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A4DC0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A4E90 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A4F50 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5010 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A50E0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A51B0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5270 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5450 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5550 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5650 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A57C0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5980 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5A10 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5B20 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5CB0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5EA0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5FA0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A60B0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A6220 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A6320 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A6440 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A6550 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A6630 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A67D0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A68E0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A6A00 | 101 | A |  | 1 | 1 |  | alta |

## CPedAttractorManager (71)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005EE820 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005ED0A3 | 9 | D |  | 0 | 0 |  | baja |
| 0x005ED388 | 9 | D |  | 0 | 0 |  | baja |
| 0x005ED643 | 9 | D |  | 0 | 0 |  | baja |
| 0x005ED92A | 9 | D |  | 0 | 0 |  | baja |
| 0x005EDBE3 | 9 | D |  | 0 | 0 |  | baja |
| 0x005EE530 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EE600 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EE6D0 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EE7B0 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EE880 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EE950 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EEA20 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EEAF0 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EEBC0 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EEC90 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EC040 | 39 | D |  | 0 | 0 |  | baja |
| 0x005EC070 | 39 | A |  | 1 | 0 |  | baja |
| 0x005EC0B0 | 39 | D |  | 0 | 0 |  | baja |
| 0x005EC110 | 39 | A |  | 1 | 0 |  | baja |
| 0x005EC180 | 39 | D |  | 0 | 0 |  | baja |
| 0x005EC1B0 | 39 | A |  | 1 | 0 |  | baja |
| 0x005EC1F0 | 39 | C |  | 4 | 0 |  | baja |
| 0x005EC260 | 39 | C |  | 2 | 0 |  | baja |
| 0x005EC2D0 | 39 | D |  | 0 | 0 |  | baja |
| 0x005EC340 | 39 | C |  | 9 | 0 |  | baja |
| 0x005EC3B0 | 39 | D |  | 0 | 0 |  | baja |
| 0x005EC3E0 | 39 | D |  | 0 | 0 |  | baja |
| 0x005EC0E0 | 42 | A |  | 1 | 0 |  | baja |
| 0x005EC140 | 42 | A |  | 1 | 0 |  | baja |
| 0x005EC220 | 49 | A |  | 1 | 1 |  | baja |
| 0x005EE4E0 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EE5B0 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EE680 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EE760 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EE830 | 76 | B |  | 0 | 0 | sí | baja |
| 0x005EE900 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EE9D0 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EEAA0 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EEB70 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EEC40 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EE470 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EE540 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EE610 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EE6E0 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EE7C0 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EE890 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EE960 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EEA30 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EEB00 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EEBD0 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EF920 | 95 | A |  | 10 | 10 |  | baja |
| 0x005EE200 | 102 | A |  | 2 | 2 |  | baja |
| 0x005EE300 | 102 | A |  | 1 | 1 |  | baja |
| 0x005EE400 | 102 | A |  | 2 | 1 |  | baja |
| 0x005EDE50 | 109 | A |  | 1 | 0 |  | baja |
| 0x005EDD90 | 111 | A |  | 1 | 0 |  | baja |
| 0x005ECAB0 | 122 | A |  | 1 | 0 |  | baja |
| 0x005ECB30 | 122 | A |  | 1 | 0 |  | baja |
| 0x005ECBB0 | 122 | A |  | 1 | 0 |  | baja |
| 0x005ECC30 | 122 | A |  | 1 | 0 |  | baja |
| 0x005ECCB0 | 122 | A |  | 1 | 0 |  | baja |
| 0x005EE370 | 136 | A |  | 1 | 1 |  | baja |
| 0x005EE270 | 138 | A |  | 1 | 1 |  | baja |
| 0x005EDF10 | 148 | A |  | 1 | 1 |  | baja |
| 0x005ECF10 | 645 | A |  | 1 | 0 |  | baja |
| 0x005ED4B0 | 645 | A |  | 2 | 1 |  | baja |
| 0x005EDA50 | 645 | A |  | 1 | 0 |  | baja |
| 0x005EF980 | 659 | A |  | 8 | 8 |  | baja |
| 0x005ED1B0 | 739 | A |  | 1 | 0 |  | baja |
| 0x005ED750 | 739 | A |  | 1 | 0 |  | baja |

## CTaskComplexSeekEntityRadiusAngleOffset (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00493870 | 30 | B |  | 0 | 0 | sí | alta |
| 0x00496DC0 | 61 | A |  | 2 | 2 |  | baja |
| 0x004993D0 | 82 | A |  | 4 | 4 |  | baja |
| 0x00493960 | 136 | C |  | 1 | 0 |  | baja |
| 0x00496E00 | 8462 | D |  | 0 | 0 |  | baja |

## CEntity (42)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00403B7B | 2 | C |  | 3 | 0 |  | baja |
| 0x00542080 | 5 | C |  | 2 | 0 |  | baja |
| 0x00402DC5 | 9 | C |  | 4 | 0 |  | baja |
| 0x004033B2 | 11 | C |  | 18 | 0 |  | baja |
| 0x00447000 | 14 | C |  | 7 | 0 |  | baja |
| 0x00447050 | 14 | C |  | 1 | 0 |  | baja |
| 0x004032A4 | 15 | D |  | 0 | 0 |  | baja |
| 0x004032C5 | 18 | C |  | 1 | 0 |  | baja |
| 0x0046A2F0 | 18 | C |  | 2 | 0 |  | baja |
| 0x00463490 | 21 | C |  | 1 | 0 |  | baja |
| 0x00403C70 | 24 | C |  | 1 | 0 |  | baja |
| 0x0040335F | 26 | C |  | 1 | 0 |  | baja |
| 0x00536C20 | 27 | B |  | 0 | 0 | sí | baja |
| 0x00403379 | 29 | C |  | 1 | 0 |  | baja |
| 0x00403E00 | 32 | A |  | 4 | 4 |  | baja |
| 0x004634D0 | 32 | D |  | 0 | 0 |  | baja |
| 0x00533020 | 35 | A |  | 2 | 2 | sí | alta |
| 0x005335E0 | 47 | A |  | 5 | 5 |  | alta |
| 0x0046A340 | 49 | C |  | 1 | 0 |  | baja |
| 0x00533670 | 54 | A |  | 1 | 1 |  | alta |
| 0x004027CD | 58 | C |  | 1 | 0 |  | baja |
| 0x00401000 | 74 | C |  | 1 | 0 |  | baja |
| 0x005336B0 | 79 | A |  | 2 | 2 |  | alta |
| 0x00533610 | 81 | A |  | 3 | 3 |  | alta |
| 0x00403D07 | 88 | D |  | 0 | 0 |  | baja |
| 0x00403C14 | 92 | C |  | 1 | 0 |  | baja |
| 0x00403B03 | 120 | C |  | 1 | 0 |  | baja |
| 0x00403C88 | 127 | C |  | 1 | 0 |  | baja |
| 0x00403638 | 137 | C |  | 2 | 0 |  | baja |
| 0x004036C1 | 139 | C |  | 3 | 0 |  | baja |
| 0x00403B7D | 151 | C |  | 7 | 0 |  | baja |
| 0x00402A36 | 211 | C |  | 1 | 0 |  | baja |
| 0x0040104A | 321 | C |  | 2 | 0 |  | baja |
| 0x00402370 | 336 | C |  | 1 | 0 |  | baja |
| 0x0040118B | 350 | C |  | 1 | 0 |  | baja |
| 0x004028A2 | 404 | C |  | 1 | 0 |  | baja |
| 0x0040374C | 437 | C |  | 1 | 0 |  | baja |
| 0x00403902 | 513 | C |  | 1 | 0 |  | baja |
| 0x004033F5 | 525 | C |  | 1 | 0 |  | baja |
| 0x004024C0 | 581 | C |  | 1 | 0 |  | baja |
| 0x00401FBB | 949 | C |  | 1 | 0 |  | baja |
| 0x004012E9 | 3282 | C |  | 3 | 0 |  | baja |

## CTheScripts (14)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00486AE0 | 1 | C |  | 1 | 0 |  | alta |
| 0x0046B260 | 12 | C |  | 1 | 0 |  | baja |
| 0x004748D0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0046A7E0 | 33 | C |  | 1 | 0 |  | alta |
| 0x00465070 | 40 | A |  | 5 | 1 |  | baja |
| 0x00465040 | 41 | A |  | 98 | 1 |  | baja |
| 0x00474870 | 46 | C |  | 1 | 0 |  | baja |
| 0x004748A0 | 46 | C |  | 1 | 0 |  | baja |
| 0x004650A0 | 54 | A |  | 1 | 1 |  | baja |
| 0x00493420 | 88 | C |  | 2 | 0 |  | alta |
| 0x00493480 | 105 | C |  | 1 | 0 |  | alta |
| 0x00493360 | 185 | C |  | 1 | 0 |  | alta |
| 0x004654B0 | 226 | A |  | 22 | 4 |  | baja |
| 0x00481300 | 8957 | D |  | 0 | 0 |  | alta |

## CWeaponModelInfo (41)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004C5790 | 7 | C |  | 1 | 0 |  | baja |
| 0x004C9F70 | 19 | C |  | 1 | 0 |  | baja |
| 0x004C57A0 | 25 | D |  | 0 | 0 |  | baja |
| 0x004C9EA0 | 25 | C |  | 4 | 0 |  | baja |
| 0x004CC150 | 25 | C |  | 1 | 0 |  | baja |
| 0x004CB530 | 47 | C |  | 3 | 0 |  | baja |
| 0x004CBDE0 | 51 | C |  | 35 | 0 |  | baja |
| 0x004CC580 | 64 | C |  | 3 | 0 |  | baja |
| 0x004CBF80 | 65 | C |  | 3 | 0 |  | baja |
| 0x004CBFD0 | 65 | C |  | 1 | 0 |  | baja |
| 0x004CC020 | 65 | C |  | 2 | 0 |  | baja |
| 0x004CB560 | 78 | C |  | 7 | 0 |  | baja |
| 0x004CB5E0 | 82 | C |  | 1 | 0 |  | baja |
| 0x004CC170 | 82 | C |  | 1 | 0 |  | baja |
| 0x004CA540 | 85 | D |  | 0 | 0 |  | baja |
| 0x004CBD80 | 87 | A |  | 8 | 0 |  | baja |
| 0x004CBE80 | 87 | C |  | 2 | 0 |  | baja |
| 0x004CBE20 | 93 | C |  | 8 | 0 |  | baja |
| 0x004CBF20 | 93 | C |  | 1 | 0 |  | baja |
| 0x004CAD90 | 104 | C |  | 3 | 0 |  | baja |
| 0x004CC0C0 | 129 | C |  | 3 | 0 |  | baja |
| 0x004CB730 | 141 | C |  | 3 | 0 |  | baja |
| 0x004CB640 | 157 | A |  | 1 | 0 |  | baja |
| 0x004C9990 | 202 | C |  | 4 | 0 |  | baja |
| 0x004CCD70 | 236 | C |  | 1 | 0 |  | baja |
| 0x004CD250 | 265 | envoltura |  | 2 | 0 |  | baja |
| 0x004CB9C0 | 310 | C |  | 1 | 0 |  | baja |
| 0x004CD360 | 367 | C |  | 1 | 0 |  | baja |
| 0x004CBB00 | 402 | C |  | 1 | 0 |  | baja |
| 0x004CCE60 | 457 | C |  | 1 | 0 |  | baja |
| 0x004CB7C0 | 512 | C |  | 1 | 0 |  | baja |
| 0x004CD050 | 512 | C |  | 1 | 0 |  | baja |
| 0x004CA290 | 559 | C |  | 1 | 0 |  | baja |
| 0x004CAAE0 | 673 | C |  | 1 | 0 |  | baja |
| 0x004C9F90 | 744 | C |  | 1 | 0 |  | baja |
| 0x004CC1D0 | 906 | C |  | 1 | 0 |  | baja |
| 0x004CC5C0 | 907 | C |  | 1 | 0 |  | baja |
| 0x004C9AB0 | 967 | D |  | 0 | 0 |  | baja |
| 0x004CC970 | 990 | envoltura |  | 12 | 0 |  | baja |
| 0x004CA5A0 | 1339 | C |  | 1 | 0 |  | baja |
| 0x004CAEA0 | 1530 | C |  | 2 | 0 |  | baja |

## Win (85)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0074EAC0 | 1 | C |  | 2 | 0 |  | baja |
| 0x0074EAB0 | 6 | C |  | 1 | 0 |  | baja |
| 0x0074CCB0 | 9 | A |  | 7 | 3 |  | baja |
| 0x00746070 | 11 | A |  | 1 | 1 |  | baja |
| 0x00749D20 | 32 | D |  | 0 | 0 |  | baja |
| 0x0074A4C0 | 33 | A |  | 6 | 6 |  | baja |
| 0x0074BE00 | 34 | A |  | 4 | 1 |  | baja |
| 0x0074BE70 | 34 | A |  | 1 | 1 |  | baja |
| 0x0074CDA0 | 34 | A |  | 6 | 3 |  | baja |
| 0x0074DC20 | 34 | A |  | 5 | 2 |  | baja |
| 0x0074C690 | 35 | A |  | 3 | 3 |  | baja |
| 0x0074A490 | 37 | A |  | 16 | 12 |  | baja |
| 0x0074BDA0 | 39 | A |  | 7 | 3 |  | baja |
| 0x0074BDD0 | 39 | A |  | 4 | 3 |  | baja |
| 0x0074CD70 | 39 | A |  | 7 | 3 |  | baja |
| 0x0074DBC0 | 39 | A |  | 13 | 11 |  | baja |
| 0x0074DBF0 | 39 | A |  | 5 | 2 |  | baja |
| 0x0074A520 | 44 | envoltura |  | 1 | 0 |  | baja |
| 0x0074C7D0 | 44 | A |  | 13 | 8 |  | baja |
| 0x0074A1C0 | 52 | A |  | 1 | 0 |  | baja |
| 0x0074E010 | 52 | envoltura |  | 1 | 0 |  | baja |
| 0x0074EA40 | 55 | A |  | 1 | 0 |  | baja |
| 0x0074EB40 | 58 | C |  | 2 | 0 |  | baja |
| 0x00749120 | 60 | D |  | 0 | 0 |  | baja |
| 0x0074D7D0 | 60 | D |  | 0 | 0 |  | baja |
| 0x00749160 | 62 | D |  | 0 | 0 |  | baja |
| 0x0074D810 | 62 | D |  | 0 | 0 |  | baja |
| 0x0074FBC0 | 62 | envoltura |  | 0 | 0 |  | baja |
| 0x00749B20 | 65 | A |  | 9 | 9 |  | baja |
| 0x0074D870 | 65 | envoltura |  | 2 | 0 |  | baja |
| 0x00749BB0 | 67 | envoltura |  | 6 | 0 |  | baja |
| 0x00749C00 | 67 | envoltura |  | 5 | 0 |  | baja |
| 0x0074FFA0 | 68 | A |  | 2 | 0 |  | baja |
| 0x0074AE50 | 70 | A |  | 2 | 0 |  | baja |
| 0x0074EEC0 | 72 | envoltura |  | 0 | 0 |  | baja |
| 0x00749830 | 75 | A |  | 1 | 0 |  | baja |
| 0x0074FFF0 | 81 | A |  | 2 | 0 |  | baja |
| 0x0074C6C0 | 82 | A |  | 4 | 4 |  | baja |
| 0x0074E9E0 | 85 | envoltura |  | 1 | 0 |  | baja |
| 0x0074E150 | 86 | A |  | 9 | 1 |  | baja |
| 0x0074FC70 | 89 | envoltura |  | 1 | 0 |  | baja |
| 0x0074E450 | 91 | envoltura |  | 0 | 0 |  | baja |
| 0x0074DA20 | 94 | A |  | 14 | 7 |  | baja |
| 0x007486A0 | 99 | C |  | 1 | 0 |  | baja |
| 0x0074E970 | 99 | envoltura |  | 2 | 0 |  | baja |
| 0x007469A0 | 109 | A |  | 6 | 5 |  | baja |
| 0x0074EAD0 | 110 | C |  | 1 | 0 |  | baja |
| 0x0074A290 | 113 | A |  | 8 | 5 |  | baja |
| 0x0074D6D0 | 117 | A |  | 1 | 0 |  | baja |
| 0x007496A0 | 122 | D |  | 0 | 0 |  | baja |
| 0x00749D40 | 124 | A |  | 5 | 2 |  | baja |
| 0x0074BCC0 | 124 | D |  | 0 | 0 |  | baja |
| 0x0074EB80 | 124 | C |  | 1 | 0 |  | baja |
| 0x00749DC0 | 127 | A |  | 26 | 19 |  | baja |
| 0x0074F4E0 | 129 | envoltura |  | 6 | 0 |  | baja |
| 0x0074D990 | 131 | A |  | 2 | 2 |  | baja |
| 0x00748F70 | 134 | A |  | 1 | 1 |  | baja |
| 0x0074EC00 | 150 | C |  | 2 | 0 |  | baja |
| 0x0074CCC0 | 165 | A |  | 23 | 3 |  | baja |
| 0x0074ECA0 | 165 | envoltura |  | 3 | 0 |  | baja |
| 0x0074E350 | 193 | A |  | 3 | 1 |  | baja |
| 0x00749C50 | 201 | A |  | 4 | 1 |  | baja |
| 0x007460A0 | 205 | A |  | 1 | 1 |  | baja |
| 0x0074FDA0 | 237 | A |  | 1 | 1 |  | baja |
| 0x00747DB0 | 255 | D |  | 0 | 0 |  | baja |
| 0x00749E60 | 258 | A |  | 17 | 12 |  | baja |
| 0x0074C200 | 269 | A |  | 2 | 2 |  | baja |
| 0x0074F610 | 288 | A |  | 1 | 1 |  | baja |
| 0x007491F0 | 318 | A |  | 12 | 5 |  | baja |
| 0x00749330 | 327 | envoltura |  | 2 | 0 |  | baja |
| 0x0074ED50 | 364 | envoltura |  | 4 | 0 |  | baja |
| 0x00749520 | 372 | D |  | 0 | 0 |  | baja |
| 0x0074A310 | 377 | A |  | 21 | 14 |  | baja |
| 0x0074C310 | 443 | envoltura |  | 2 | 0 |  | baja |
| 0x0074CA90 | 543 | A |  | 4 | 3 |  | baja |
| 0x00749F70 | 592 | A |  | 1 | 1 |  | baja |
| 0x0074C800 | 595 | A |  | 14 | 11 |  | baja |
| 0x0074F210 | 710 | envoltura |  | 1 | 0 |  | baja |
| 0x0074DD30 | 724 | envoltura |  | 1 | 0 |  | baja |
| 0x0074E600 | 870 | envoltura |  | 1 | 0 |  | baja |
| 0x0074F760 | 923 | A |  | 1 | 1 |  | baja |
| 0x0074B060 | 952 | A |  | 1 | 0 |  | baja |
| 0x0074D190 | 1332 | envoltura |  | 6 | 0 |  | baja |
| 0x00748710 | 1947 | C |  | 1 | 0 |  | baja |
| 0x0074B420 | 2207 | A |  | 6 | 5 |  | baja |

## FxEmitterBP_c (141)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004A4950 | 1 | A |  | 1 | 1 |  | baja |
| 0x005CF880 | 1 | A |  | 2 | 0 |  | baja |
| 0x005CF870 | 3 | A |  | 1 | 0 |  | baja |
| 0x004A2BB0 | 5 | C |  | 1 | 0 |  | alta |
| 0x005CF840 | 5 | C |  | 1 | 0 |  | baja |
| 0x005CFA30 | 5 | A |  | 1 | 1 |  | baja |
| 0x005CFE00 | 5 | A |  | 3 | 3 |  | baja |
| 0x004A4940 | 7 | C |  | 64 | 0 |  | baja |
| 0x004A2570 | 8 | B |  | 0 | 0 | sí | alta |
| 0x005CF810 | 10 | C |  | 2 | 0 |  | baja |
| 0x005CF830 | 10 | C |  | 1 | 0 |  | baja |
| 0x004A18F0 | 11 | C |  | 1 | 0 |  | alta |
| 0x005CF800 | 16 | A |  | 3 | 0 |  | baja |
| 0x005CF820 | 16 | C |  | 1 | 0 |  | baja |
| 0x005CF890 | 16 | A |  | 14 | 0 |  | baja |
| 0x004A1900 | 18 | A |  | 1 | 1 |  | alta |
| 0x004A1920 | 18 | D |  | 0 | 0 |  | alta |
| 0x005C62D0 | 20 | D |  | 0 | 0 |  | baja |
| 0x005CF850 | 20 | C |  | 2 | 0 |  | baja |
| 0x004A4460 | 24 | B |  | 0 | 0 | sí | baja |
| 0x005CF8A0 | 24 | A |  | 3 | 0 |  | baja |
| 0x004A2550 | 26 | B |  | 0 | 0 | sí | alta |
| 0x004A2B20 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004A2B90 | 27 | B |  | 0 | 0 | sí | alta |
| 0x005CF940 | 27 | C |  | 5 | 0 |  | baja |
| 0x005C6BB0 | 28 | A |  | 2 | 0 |  | baja |
| 0x005C6BD0 | 28 | A |  | 4 | 0 |  | baja |
| 0x005C6B90 | 31 | C |  | 1 | 0 |  | baja |
| 0x005C69E0 | 40 | D |  | 0 | 0 |  | baja |
| 0x005C5910 | 41 | D |  | 0 | 0 |  | baja |
| 0x005CCCC0 | 48 | A |  | 1 | 0 |  | baja |
| 0x004A24D0 | 50 | A |  | 2 | 2 |  | alta |
| 0x005C6B50 | 56 | A |  | 3 | 0 |  | baja |
| 0x005CA6E0 | 59 | C |  | 1 | 0 |  | baja |
| 0x005CA7E0 | 59 | D |  | 0 | 0 |  | baja |
| 0x005CA260 | 63 | A |  | 1 | 0 |  | baja |
| 0x005CA840 | 65 | A |  | 1 | 0 |  | baja |
| 0x005C5210 | 66 | A |  | 1 | 0 |  | baja |
| 0x005C60B0 | 70 | D |  | 0 | 0 |  | baja |
| 0x005CF8C0 | 70 | C |  | 6 | 0 |  | baja |
| 0x005CD810 | 76 | A |  | 2 | 2 |  | baja |
| 0x005CD6B0 | 80 | D |  | 0 | 0 |  | baja |
| 0x005C63D0 | 81 | A |  | 1 | 0 |  | baja |
| 0x005C70D0 | 82 | A |  | 1 | 0 |  | baja |
| 0x005C79F0 | 88 | C |  | 4 | 0 |  | baja |
| 0x005C35F0 | 97 | D |  | 0 | 0 |  | baja |
| 0x005C8620 | 100 | A |  | 2 | 0 |  | baja |
| 0x005C5560 | 102 | C |  | 18 | 0 |  | baja |
| 0x005CFA50 | 102 | A |  | 1 | 0 |  | baja |
| 0x005C54F0 | 104 | C |  | 12 | 0 |  | baja |
| 0x005CEF30 | 104 | D |  | 0 | 0 |  | baja |
| 0x005CEFA0 | 104 | D |  | 0 | 0 |  | baja |
| 0x005C88F0 | 105 | D |  | 0 | 0 |  | baja |
| 0x005C4380 | 108 | A |  | 1 | 0 |  | baja |
| 0x005CCDF0 | 110 | A |  | 1 | 0 |  | baja |
| 0x005D01D0 | 112 | A |  | 1 | 1 |  | baja |
| 0x005C26B0 | 117 | A |  | 1 | 1 |  | baja |
| 0x005CC950 | 119 | A |  | 1 | 0 |  | baja |
| 0x005C90D0 | 120 | D |  | 0 | 0 |  | baja |
| 0x005CC8C0 | 121 | A |  | 1 | 0 |  | baja |
| 0x005C6600 | 127 | C |  | 3 | 0 |  | baja |
| 0x005C9710 | 127 | A |  | 1 | 0 |  | baja |
| 0x005C9C60 | 127 | C |  | 1 | 0 |  | baja |
| 0x005CC7D0 | 132 | A |  | 1 | 0 |  | baja |
| 0x005C5420 | 134 | A |  | 1 | 0 |  | baja |
| 0x005CC610 | 136 | D |  | 0 | 0 |  | baja |
| 0x005C87E0 | 143 | A |  | 1 | 1 |  | baja |
| 0x005CF1B0 | 148 | C |  | 2 | 0 |  | baja |
| 0x005C8690 | 149 | A |  | 1 | 1 |  | baja |
| 0x005CF250 | 151 | C |  | 2 | 0 |  | baja |
| 0x005CFFB0 | 151 | A |  | 1 | 0 |  | baja |
| 0x005CC1F0 | 153 | C |  | 1 | 0 |  | baja |
| 0x005CAB70 | 157 | C |  | 1 | 0 |  | baja |
| 0x005CC050 | 157 | C |  | 1 | 0 |  | baja |
| 0x005CAF00 | 164 | C |  | 1 | 0 |  | baja |
| 0x005CEDD0 | 167 | C |  | 1 | 0 |  | baja |
| 0x005CEE80 | 167 | C |  | 1 | 0 |  | baja |
| 0x005CA890 | 168 | A |  | 1 | 0 |  | baja |
| 0x005CBE50 | 169 | C |  | 1 | 0 |  | baja |
| 0x005CA630 | 170 | C |  | 1 | 0 |  | baja |
| 0x005C7920 | 174 | A |  | 1 | 0 |  | baja |
| 0x005CA720 | 180 | D |  | 0 | 0 |  | baja |
| 0x005C6A90 | 186 | A |  | 1 | 0 |  | baja |
| 0x005D0110 | 187 | A |  | 1 | 1 |  | baja |
| 0x005C5260 | 188 | A |  | 1 | 0 |  | baja |
| 0x005CED10 | 188 | C |  | 2 | 0 |  | baja |
| 0x005C7010 | 191 | A |  | 4 | 0 |  | baja |
| 0x005C8E20 | 192 | A |  | 1 | 0 |  | baja |
| 0x005C2730 | 194 | A |  | 1 | 1 |  | baja |
| 0x005C8EE0 | 195 | A |  | 2 | 0 |  | baja |
| 0x005CF960 | 198 | A |  | 1 | 1 |  | baja |
| 0x005C48F0 | 200 | C |  | 1 | 0 |  | baja |
| 0x005CAE30 | 207 | A |  | 1 | 0 |  | baja |
| 0x005CAA90 | 210 | C |  | 1 | 0 |  | baja |
| 0x005CFD20 | 218 | A |  | 1 | 1 |  | baja |
| 0x005CCE80 | 222 | D |  | 0 | 0 |  | baja |
| 0x005C9B80 | 223 | C |  | 3 | 0 |  | baja |
| 0x005CBB20 | 223 | C |  | 1 | 0 |  | baja |
| 0x005C4B90 | 224 | C |  | 1 | 0 |  | baja |
| 0x005C6100 | 224 | D |  | 0 | 0 |  | baja |
| 0x005C4640 | 226 | C |  | 8 | 0 |  | baja |
| 0x005CC0F0 | 256 | C |  | 3 | 0 |  | baja |
| 0x005C30D0 | 259 | A |  | 1 | 0 |  | baja |
| 0x005C7DF0 | 259 | A |  | 1 | 0 |  | baja |
| 0x005CFAC0 | 260 | A |  | 1 | 1 |  | baja |
| 0x005CF580 | 263 | C |  | 1 | 0 |  | baja |
| 0x005C7590 | 264 | A |  | 1 | 1 |  | baja |
| 0x005C9A60 | 286 | C |  | 8 | 0 |  | baja |
| 0x005CF6D0 | 297 | A |  | 2 | 0 |  | baja |
| 0x005CCB50 | 318 | D |  | 0 | 0 |  | baja |
| 0x005CEBC0 | 331 | C |  | 4 | 0 |  | baja |
| 0x005CA940 | 332 | C |  | 1 | 0 |  | baja |
| 0x005C7420 | 336 | A |  | 1 | 0 |  | baja |
| 0x005C5EC0 | 362 | C |  | 1 | 0 |  | baja |
| 0x005C5940 | 370 | C |  | 3 | 0 |  | baja |
| 0x005C5780 | 373 | C |  | 6 | 0 |  | baja |
| 0x005CC9D0 | 373 | A |  | 1 | 0 |  | baja |
| 0x005C4760 | 387 | C |  | 1 | 0 |  | baja |
| 0x005CFE20 | 394 | A |  | 1 | 0 |  | baja |
| 0x005C6850 | 396 | D |  | 0 | 0 |  | baja |
| 0x005CF010 | 403 | D |  | 0 | 0 |  | baja |
| 0x005CA490 | 415 | C |  | 1 | 0 |  | baja |
| 0x005C9350 | 416 | A |  | 1 | 0 |  | baja |
| 0x005C55D0 | 419 | C |  | 2 | 0 |  | baja |
| 0x005C6680 | 453 | C |  | 3 | 0 |  | baja |
| 0x005C6430 | 460 | A |  | 1 | 0 |  | baja |
| 0x005C5AC0 | 468 | C |  | 1 | 0 |  | baja |
| 0x005CD350 | 477 | A |  | 1 | 0 |  | baja |
| 0x005CA2A0 | 493 | C |  | 1 | 0 |  | baja |
| 0x005C43F0 | 520 | C |  | 2 | 0 |  | baja |
| 0x005C3700 | 531 | A |  | 1 | 0 |  | baja |
| 0x005C5CA0 | 536 | C |  | 1 | 0 |  | baja |
| 0x005C4D50 | 571 | C |  | 2 | 0 |  | baja |
| 0x005CBC00 | 587 | C |  | 1 | 0 |  | baja |
| 0x005CB8C0 | 599 | C |  | 1 | 0 |  | baja |
| 0x005C9790 | 704 | C |  | 2 | 0 |  | baja |
| 0x005C7130 | 714 | A |  | 3 | 0 |  | baja |
| 0x005CB2C0 | 751 | C |  | 1 | 0 |  | baja |
| 0x005CAFB0 | 777 | C |  | 4 | 0 |  | baja |
| 0x005CB5B0 | 781 | C |  | 1 | 0 |  | baja |
| 0x005C6BF0 | 801 | A |  | 2 | 0 |  | baja |

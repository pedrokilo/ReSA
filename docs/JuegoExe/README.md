<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/11/2026 (Ciudad de México):** 885 grupos / 10136 funciones reconstruidas; 10136 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Código del juego en el EXE: mapa de las 11,803 funciones

[Progreso del reversing](../ReversedClasses.md#cobertura-exe) · [Lista completa (CSV)](funciones.csv)

Región **0x401000–0x74FFFF**: 11,803 funciones, 3,007,767 bytes. Es el código propio del juego y la capa de plataforma; RenderWare, D3D y el CRT quedan fuera (se sustituyen enlazando bibliotecas equivalentes). Generado por `contrib/game_function_map.py`; cotas y método en `contrib/exe_coverage.py`.

## Estado

| Estado | Funciones | % func. | Bytes | % bytes |
| --- | ---: | ---: | ---: | ---: |
| D · Falta: sin llamadas ni vtable (¿muerto o por puntero?) | 346 | 2.9% | 56,451 | 1.9% |
| C · Falta: solo la llaman funciones aún no reemplazadas | 378 | 3.2% | 60,200 | 2.0% |
| B · Falta: método virtual | 453 | 3.8% | 24,195 | 0.8% |
| A · Falta: llamada desde código ya reemplazado | 576 | 4.9% | 90,432 | 3.0% |
| Envoltura pendiente (la DLL aún llama al EXE) | 22 | 0.2% | 7,112 | 0.2% |
| Reemplazada por C++ | 10,028 | 85.0% | 2,769,377 | 92.1% |

Las funciones de A–D son **lo que falta**. El grupo A es el piso para la independencia (la DLL ya depende de ellas); es una cota inferior porque hay 6,004 llamadas indirectas sin resolver. Las envolturas pendientes (que ya tienen hook pero llaman al EXE) se cuentan aparte, por eso A–D suman menos que en la tabla de cobertura de ReversedClasses. Listas: [A](pendientes-A.md) · [B](pendientes-B.md) · [C](pendientes-C.md) · [D](pendientes-D.md).

## Por bloque de direcciones (64 KB)

Ordenados de mayor a menor porcentaje reemplazado.

El código se enlaza por unidad de compilación, así que cada bloque suele corresponder a pocos módulos. Útil para ver dónde queda trabajo.

## Excluidas del objetivo

1 función(es) del rango del juego no se cuentan en las cifras de arriba porque no son viables en C++ puro (lista en [excluidas.csv](excluidas.csv)):

| Dirección | Bytes | Módulo estimado | Motivo |
| --- | ---: | ---: | ---: |
| 0x00746800 | 110 | VideoPlayer | __usercall anti-manipulacion: lee registros que C++ no puede leer; sin llamadores directos ni vtable; excluida del objetivo el 11/11/2026 (lote 161A, evidencia en reversing/Fn161A) |

| Bloque | Funciones | Reemplazadas | Faltan | Bytes que faltan | Módulos con más faltantes (estimado) |
| --- | ---: | ---: | ---: | ---: | ---: |
| 0x520000 | 62 | 100% | 0 | 0 | — |
| 0x560000 | 212 | 100% | 0 | 0 | — |
| 0x580000 | 129 | 100% | 0 | 0 | — |
| 0x4E0000 | 246 | 100% | 1 | 33 | — |
| 0x700000 | 138 | 99% | 2 | 944 | CShadows (1), CStencilShadows (1) |
| 0x6B0000 | 83 | 98% | 2 | 1,048 | CAutomobile (2) |
| 0x500000 | 305 | 97% | 10 | 1,015 | CAEWaterCannonAudioEntity (1) |
| 0x640000 | 425 | 96% | 18 | 1,328 | CTaskSimpleCarFallOut (1), CTaskComplexGoToBoatSteeringWheel (1) |
| 0x530000 | 275 | 96% | 12 | 853 | CEntity (5), CGame (3), Game (2) |
| 0x410000 | 162 | 95% | 8 | 422 | CColStore (5) |
| 0x710000 | 158 | 95% | 8 | 763 | CFont (3), CStencilShadows (1), Common (1) |
| 0x6C0000 | 135 | 95% | 7 | 564 | CHeli (3), CPlane (2) |
| 0x430000 | 200 | 94% | 12 | 849 | CEntryExitManager (9), CCheat (1) |
| 0x730000 | 278 | 94% | 17 | 805 | CTxdStore (6), CVisibilityPlugins (4) |
| 0x6F0000 | 182 | 93% | 12 | 1,375 | CTrain (6), cHandlingDataMgr (1) |
| 0x590000 | 255 | 93% | 18 | 3,127 | InteriorManager_c (2), CObject (2), Interior_c (1) |
| 0x6A0000 | 112 | 93% | 8 | 1,869 | CAutomobile (8) |
| 0x570000 | 110 | 93% | 8 | 333 | CMenuManager (5), CTheZones (1) |
| 0x6E0000 | 92 | 92% | 7 | 604 | CVehicle (3), CWaterLevel (2) |
| 0x510000 | 61 | 92% | 5 | 794 | CCam (2) |
| 0x5B0000 | 157 | 92% | 13 | 727 | — |
| 0x440000 | 194 | 91% | 17 | 2,367 | CPathFind (10) |
| 0x720000 | 203 | 91% | 18 | 3,026 | CWeather (2), CCullZones (1) |
| 0x4D0000 | 319 | 90% | 31 | 7,141 | RpAnimBlend (16), CAEAmbienceTrackManager (3), CAECutsceneTrackManager (3) |
| 0x450000 | 172 | 90% | 17 | 4,918 | CReplay (10), CPathFind (1) |
| 0x680000 | 184 | 90% | 19 | 1,482 | — |
| 0x420000 | 135 | 89% | 15 | 2,094 | CCarCtrl (3) |
| 0x690000 | 310 | 89% | 35 | 2,952 | CTaskComplexSeekEntityAiming (5), CTaskSimpleHoldEntity (2) |
| 0x610000 | 327 | 89% | 37 | 4,300 | Rs (4), CTaskSimpleFight (1) |
| 0x480000 | 111 | 88% | 13 | 15,036 | CTheScripts (2) |
| 0x630000 | 585 | 88% | 69 | 5,627 | CTaskComplexObserveTrafficLightsAndAchieveHeading (1) |
| 0x600000 | 237 | 87% | 30 | 4,562 | CPlayerPed (2) |
| 0x460000 | 164 | 87% | 21 | 7,205 | CScriptsForBrains (4), CTheScripts (1) |
| 0x470000 | 46 | 87% | 6 | 220 | — |
| 0x5D0000 | 335 | 86% | 47 | 5,881 | CCustomCarEnvMapPipeline (5), CPlantMgr (1) |
| 0x620000 | 282 | 84% | 44 | 3,142 | CTaskSimpleStandStill (1) |
| 0x4F0000 | 203 | 84% | 32 | 2,720 | CAEUserRadioTrackManager (28) |
| 0x6D0000 | 179 | 84% | 29 | 470 | CVehicle (29) |
| 0x540000 | 176 | 84% | 29 | 1,498 | CPlaceable (3), CPad (2), Common (2) |
| 0x400000 | 281 | 83% | 47 | 9,569 | CStreaming (14) |
| 0x660000 | 300 | 83% | 51 | 6,789 | CTaskComplexGoToPointAndStandStill (3), CTaskComplexGotoDoorAndOpen (2), CTaskGoToVehicleAndLean (1) |
| 0x490000 | 153 | 82% | 27 | 10,996 | CStuntJumpManager (4), Fx_c (4), CTheScripts (3) |
| 0x650000 | 387 | 81% | 72 | 4,113 | CTaskComplexMedicTreatInjuredPed (2), CTaskComplexUseWaterCannon (2), CTaskComplexGangFollower (2) |
| 0x670000 | 290 | 81% | 56 | 9,654 | CTaskComplexWanderProstitute (2), CTaskComplexFallAndGetUp (2), CTaskComplexWander (1) |
| 0x5A0000 | 116 | 80% | 23 | 2,314 | CClothesBuilder (3), CObject (1) |
| 0x5F0000 | 292 | 79% | 60 | 2,518 | — |
| 0x4C0000 | 371 | 77% | 84 | 14,923 | CModelInfo (19), CVehicleModelInfo (13), CDamageAtomicModelInfo (3) |
| 0x4B0000 | 360 | 73% | 96 | 7,367 | CEventScriptCommand (1) |
| 0x550000 | 257 | 72% | 71 | 7,172 | CPools (51), CQuadTreeNode<T> (6), SurfaceInfos_c (2) |
| 0x4A0000 | 470 | 65% | 166 | 8,178 | FxInfoManager_c (96), FxEmitterBP_c (9), CEvent (4) |
| 0x5E0000 | 254 | 54% | 117 | 11,447 | CPedAttractor (13), CPed (4) |
| 0x740000 | 194 | 49% | 79 | 16,270 | Ps (4), Input (2) |
| 0x5C0000 | 139 | 9% | 127 | 27,874 | — |

## Por módulo (estimado)

Módulos con alguna función por reconstruir, de menos a más funciones que faltan. Para funciones sin nombre el módulo es la heurística de vecindad descrita arriba; la columna «Confianza alta» cuenta las faltantes cuyo módulo coincide en ambos vecinos.

| Módulo | Funciones | Reemplazadas | Faltan | Bytes que faltan | Confianza alta |
| --- | ---: | ---: | ---: | ---: | ---: |
| CEventPassObject | 12 | 3 | 9 | 201 | 0 |
| CTaskComplexDriveFireTruck | 13 | 6 | 7 | 202 | 0 |
| CPedList | 9 | 7 | 2 | 203 | 0 |
| CCutsceneMgr | 38 | 35 | 3 | 204 | 0 |
| IKChain_c | 22 | 20 | 2 | 205 | 0 |
| CVisibilityPlugins | 91 | 84 | 7 | 205 | 4 |
| CDamageAtomicModelInfo | 9 | 4 | 5 | 210 | 3 |
| WaterCreature_c | 5 | 3 | 2 | 211 | 0 |
| CMenuManager | 66 | 61 | 5 | 214 | 5 |
| CTaskSimpleFightingControl | 9 | 7 | 2 | 215 | 0 |
| CTaskComplexObserveTrafficLightsAndAchieveHeading | 7 | 6 | 1 | 218 | 1 |
| CHeli | 26 | 22 | 4 | 218 | 3 |
| CWaterLevel | 48 | 46 | 2 | 219 | 2 |
| CAECutsceneTrackManager | 12 | 8 | 4 | 219 | 3 |
| CAESoundManager | 18 | 15 | 3 | 220 | 0 |
| CTaskSimpleFight | 18 | 16 | 2 | 223 | 1 |
| CTaskComplexWanderProstitute | 8 | 5 | 3 | 223 | 2 |
| CPtrNodeDoubleLinkPool | 8 | 3 | 5 | 225 | 0 |
| CEventNewGangMember | 7 | 1 | 6 | 230 | 0 |
| CEventLeanOnVehicle | 8 | 2 | 6 | 234 | 0 |
| CTaskSimpleRunNamedAnim | 13 | 7 | 6 | 235 | 0 |
| CTaskSimpleWaitForBus | 12 | 6 | 6 | 239 | 0 |
| CTaskSimpleStandStill | 10 | 4 | 6 | 241 | 1 |
| CAnimBlendHierarchy | 19 | 16 | 3 | 244 | 0 |
| CEventLeaderEnteredCarAsDriver | 11 | 2 | 9 | 244 | 0 |
| CCheat | 74 | 72 | 2 | 246 | 1 |
| CTaskComplexGoToBoatSteeringWheel | 10 | 6 | 4 | 249 | 1 |
| CFont | 41 | 38 | 3 | 251 | 3 |
| CTaskComplexCopInCar | 13 | 9 | 4 | 252 | 0 |
| CTaskSimpleGangDriveBy | 8 | 6 | 2 | 254 | 0 |
| Common | 20 | 17 | 3 | 260 | 3 |
| CAEWeatherAudioEntity | 9 | 5 | 4 | 264 | 0 |
| CTaskComplexWalkRoundObject | 6 | 3 | 3 | 273 | 0 |
| CAudioZones | 11 | 7 | 4 | 275 | 0 |
| CDecisionMakerTypes | 13 | 11 | 2 | 278 | 0 |
| CEvent | 12 | 4 | 8 | 280 | 4 |
| CTask | 11 | 5 | 6 | 289 | 0 |
| CStuntJumpManager | 11 | 7 | 4 | 296 | 4 |
| cHandlingDataMgr | 17 | 11 | 6 | 299 | 1 |
| Input | 7 | 5 | 2 | 302 | 2 |
| CTaskSimpleHoldEntity | 23 | 15 | 8 | 302 | 2 |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | 9 | 6 | 3 | 304 | 0 |
| CTaskInteriorSitInChair | 5 | 1 | 4 | 304 | 0 |
| FurnitureManager_c | 10 | 7 | 3 | 306 | 0 |
| CAEWaterCannonAudioEntity | 9 | 7 | 2 | 309 | 1 |
| CPedDamageResponseCalculator | 10 | 7 | 3 | 319 | 0 |
| CTaskComplexHitPedWithCar | 14 | 10 | 4 | 319 | 1 |
| CStats | 53 | 51 | 2 | 320 | 1 |
| CTaskComplexOpenPassengerDoor | 8 | 4 | 4 | 327 | 0 |
| CTaskGoToVehicleAndLean | 11 | 8 | 3 | 331 | 1 |
| CTaskComplexDieInCar | 21 | 8 | 13 | 333 | 0 |
| CCopPed | 15 | 7 | 8 | 335 | 0 |
| CPedGeometryAnalyser | 42 | 38 | 4 | 338 | 0 |
| InteriorManager_c | 23 | 21 | 2 | 340 | 2 |
| CQuaternion | 13 | 10 | 3 | 341 | 0 |
| CEventAcquaintancePedHateBadlyLit | 10 | 2 | 8 | 342 | 0 |
| CPlane | 26 | 23 | 3 | 346 | 2 |
| CWeather | 15 | 13 | 2 | 352 | 2 |
| CTaskAllocatorPool | 10 | 3 | 7 | 362 | 0 |
| CTaskComplexGangFollower | 11 | 8 | 3 | 365 | 2 |
| CEventPlayerCommandToGroupGather | 10 | 2 | 8 | 372 | 0 |
| CColStore | 29 | 24 | 5 | 373 | 5 |
| CTaskComplexGoToPointAndStandStillAndAchieveHeading | 9 | 7 | 2 | 382 | 1 |
| VideoMode | 6 | 4 | 2 | 382 | 0 |
| CTaskComplexWalkRoundFire | 17 | 8 | 9 | 385 | 0 |
| CHandObject | 7 | 3 | 4 | 386 | 0 |
| CAEAmbienceTrackManager | 12 | 7 | 5 | 386 | 3 |
| CTaskSimpleCarFallOut | 14 | 9 | 5 | 388 | 1 |
| CTaskComplexGotoDoorAndOpen | 14 | 8 | 6 | 388 | 2 |
| CTxdStore | 32 | 26 | 6 | 388 | 6 |
| CEventKnockOffBike | 18 | 11 | 7 | 395 | 4 |
| CPopulation | 76 | 68 | 8 | 399 | 0 |
| CTaskSimpleDieInCar | 10 | 5 | 5 | 403 | 0 |
| CVehicleRecording | 34 | 31 | 3 | 417 | 0 |
| CTaskComplexWalkRoundBuildingAttempt | 19 | 15 | 4 | 421 | 1 |
| CTheZones | 27 | 22 | 5 | 422 | 1 |
| CTaskSimpleIKPointArm | 12 | 7 | 5 | 426 | 0 |
| CGame | 44 | 35 | 9 | 428 | 3 |
| CTaskSimpleHitFromFront | 24 | 3 | 21 | 430 | 0 |
| CAnimManager | 39 | 33 | 6 | 434 | 2 |
| Fx_c | 37 | 28 | 9 | 434 | 4 |
| CEventPotentialWalkIntoFire | 14 | 5 | 9 | 440 | 0 |
| CTaskComplexWanderCop | 18 | 9 | 9 | 442 | 0 |
| CObjectPool | 9 | 4 | 5 | 446 | 0 |
| COnscreenCounterEntry | 6 | 3 | 3 | 450 | 0 |
| CTaskComplexSeekEntityAiming | 14 | 8 | 6 | 451 | 5 |
| CTaskComplexPartnerGreet | 17 | 8 | 9 | 458 | 0 |
| Game | 15 | 12 | 3 | 460 | 2 |
| CShopping | 36 | 31 | 5 | 460 | 3 |
| CTaskComplexStealCar | 22 | 16 | 6 | 461 | 0 |
| CPlantMgr | 23 | 20 | 3 | 484 | 1 |
| CTaskSimpleDuckToggle | 9 | 6 | 3 | 488 | 0 |
| ProcSurfaceInfo_c | 8 | 4 | 4 | 489 | 0 |
| CEventAttractor | 13 | 5 | 8 | 490 | 3 |
| CPlayerPed | 68 | 65 | 3 | 496 | 2 |
| CTaskSimpleDuck | 25 | 17 | 8 | 496 | 0 |
| CEventScriptedAttractor | 14 | 1 | 13 | 527 | 0 |
| CPedAttractorPool | 13 | 1 | 12 | 530 | 0 |
| CStreaming | 113 | 96 | 17 | 535 | 14 |
| CStencilShadows | 18 | 13 | 5 | 543 | 2 |
| CTaskComplexRoadRage | 18 | 9 | 9 | 585 | 0 |
| CEventEditableResponse | 26 | 12 | 14 | 607 | 0 |
| Rs | 35 | 31 | 4 | 661 | 4 |
| CEntryExitManager | 29 | 18 | 11 | 675 | 9 |
| CTaskComplexFallAndGetUp | 22 | 9 | 13 | 719 | 2 |
| CQuadTreeNode<T> | 22 | 12 | 10 | 720 | 6 |
| CObject | 60 | 53 | 7 | 728 | 3 |
| CEventScriptCommand | 20 | 9 | 11 | 766 | 1 |
| CTaskSimpleThrowProjectile | 25 | 14 | 11 | 767 | 0 |
| CTaskComplexUseWaterCannon | 20 | 5 | 15 | 769 | 2 |
| CTrafficLights | 17 | 12 | 5 | 770 | 0 |
| CTaskComplexWaitForPed | 7 | 1 | 6 | 772 | 0 |
| CTaskComplexWander | 18 | 14 | 4 | 797 | 2 |
| CModelInfo | 51 | 29 | 22 | 814 | 19 |
| CShadows | 31 | 30 | 1 | 837 | 1 |
| CVehicle | 200 | 167 | 33 | 861 | 32 |
| CVehicleSaveStructure | 6 | 2 | 4 | 874 | 0 |
| Ps | 29 | 23 | 6 | 876 | 4 |
| CPad | 93 | 82 | 11 | 895 | 3 |
| CEventSeenCop | 36 | 1 | 35 | 902 | 0 |
| CTaskComplexSunbathe | 15 | 9 | 6 | 935 | 0 |
| CEntityScanner | 18 | 13 | 5 | 958 | 0 |
| CTaskComplexMedicTreatInjuredPed | 22 | 9 | 13 | 974 | 2 |
| CTaskComplexGoToPointAndStandStill | 17 | 9 | 8 | 977 | 3 |
| CTaskSimpleBeHit | 16 | 8 | 8 | 1,022 | 0 |
| CEventVehicleHitAndRun | 33 | 18 | 15 | 1,037 | 0 |
| CPed | 158 | 152 | 6 | 1,046 | 4 |
| CCam | 70 | 63 | 7 | 1,059 | 2 |
| CTaskComplexStuckInAir | 17 | 6 | 11 | 1,075 | 1 |
| CTrain | 47 | 41 | 6 | 1,076 | 6 |
| CTaskComplexInvestigateDisturbance | 17 | 8 | 9 | 1,125 | 0 |
| CTaskComplexSeekEntityAnyMeansXYOffset | 9 | 5 | 4 | 1,138 | 0 |
| CPlaceable | 50 | 26 | 24 | 1,138 | 3 |
| CEventVehicleOnFire | 21 | 4 | 17 | 1,145 | 0 |
| CEventVehicleCollision | 19 | 4 | 15 | 1,149 | 0 |
| CTaskSimpleRunTimedAnim | 20 | 8 | 12 | 1,175 | 0 |
| Interior_c | 56 | 52 | 4 | 1,200 | 1 |
| CAttractorScanner | 4 | 1 | 3 | 1,273 | 0 |
| CClothesBuilder | 36 | 23 | 13 | 1,285 | 3 |
| CVehicleModelInfo | 82 | 67 | 15 | 1,356 | 13 |
| CTaskComplexAvoidOtherPedWhileWandering | 25 | 15 | 10 | 1,382 | 0 |
| CTaskInteriorUseInfo | 11 | 8 | 3 | 1,483 | 0 |
| CPedIntelligencePool | 10 | 4 | 6 | 1,552 | 0 |
| CEventDamage | 53 | 31 | 22 | 1,596 | 0 |
| CTaskComplexWanderGang | 10 | 4 | 6 | 1,750 | 0 |
| CPedAttractor | 48 | 20 | 28 | 1,774 | 13 |
| CTaskSimpleThrowControl | 20 | 6 | 14 | 1,802 | 0 |
| CTaskComplexUseClosestFreeScriptedAttractor | 23 | 10 | 13 | 1,858 | 0 |
| CCarCtrl | 109 | 105 | 4 | 1,928 | 3 |
| CPathFind | 87 | 73 | 14 | 2,141 | 11 |
| SurfaceInfos_c | 49 | 47 | 2 | 2,174 | 2 |
| CCullZones | 38 | 25 | 13 | 2,476 | 1 |
| CAEUserRadioTrackManager | 46 | 16 | 30 | 2,533 | 28 |
| CTaskComplexTurnToFaceEntityOrCoord | 23 | 9 | 14 | 2,607 | 0 |
| CTaskComplexGoToPointAnyMeans | 13 | 7 | 6 | 2,633 | 1 |
| CAutomobile | 113 | 103 | 10 | 2,917 | 10 |
| CCustomCarEnvMapPipeline | 62 | 37 | 25 | 3,547 | 5 |
| CPools | 71 | 19 | 52 | 3,733 | 51 |
| CReplay | 57 | 47 | 10 | 4,172 | 10 |
| CRunningScript | 43 | 39 | 4 | 5,919 | 0 |
| RpAnimBlend | 59 | 43 | 16 | 6,102 | 16 |
| CScriptsForBrains | 13 | 9 | 4 | 6,307 | 4 |
| FxInfoManager_c | 124 | 8 | 116 | 6,591 | 96 |
| CPedAttractorManager | 85 | 14 | 71 | 8,097 | 0 |
| CTaskComplexSeekEntityRadiusAngleOffset | 15 | 10 | 5 | 8,771 | 1 |
| CEntity | 113 | 71 | 42 | 9,537 | 5 |
| CTheScripts | 87 | 73 | 14 | 9,864 | 6 |
| CWeaponModelInfo | 45 | 4 | 39 | 12,299 | 0 |
| Win | 109 | 24 | 65 | 14,050 | 0 |
| FxEmitterBP_c | 155 | 14 | 141 | 28,395 | 9 |

170 de 782 módulos tienen algo pendiente.

## Cómo usarlo

- Filtra `funciones.csv` por `estado` = `A` y ordena por `llamadas_desde_reemplazadas` para empezar por lo más usado por código ya reconstruido.
- Una función de 16 bytes o menos suele ser un stub o thunk; mira `thunk` antes de tratarla como trabajo.
- Los grupos C y D pueden contener código muerto: confirma con análisis dinámico antes de reconstruir.
